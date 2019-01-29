/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   l7_rip_api.h
*
* @purpose    RIP Mapping Layer APIs
*
* @component  RIP Mapping Layer
*
* @comments   none
*
* @create     05/21/2001
*
* @author     gpaussa
* @end
*
**********************************************************************/

#ifndef _RIP_MAP_API_H_
#define _RIP_MAP_API_H_

#include "l3_comm_structs.h"

/************************************************************************
* RIP funtion table that contains the APIs that are available for the
* external components when RIP component exists.
************************************************************************/
typedef struct ripFuncTable_s
{

  /* Debug functions */
  void (*ripMapCfgDataShow)(void);
} ripFuncTable_t;

extern ripFuncTable_t ripFuncTable;

/*:ignore*/
/* Begin Function Prototypes */
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the RIP administrative mode 
*
* @param    *mode   @b{(output)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the RIP administrative mode
*
* @param    mode   @b{(input)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the number of route changes made by RIP to the 
*           IP Route Database
*
* @param    *val    @b{(output)} RIP route changes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRouteChangesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of responses sent to RIP queries from other systems
*
* @param    *val   @b{(output)} RIP query response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapQueriesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the RIP split horizon mode
*
* @param    *val   @b{(output)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipSplitHorizonModeGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP split horizon mode
*
* @param    val   @b{(input)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipSplitHorizonModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP auto-summarization mode
*
* @param    *val   @b{(output)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipAutoSummarizationModeGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP auto-summarization mode
*
* @param    val   @b{(input)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipAutoSummarizationModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP host route accept mode
*
* @param    *val        @b{(output)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipHostRoutesAcceptModeGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP host route accept mode
*
* @param    val         @b{(input)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipHostRoutesAcceptModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the default metric value
*
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
extern L7_RC_t ripMapRipDefaultMetricGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the default metric value
*
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
extern L7_RC_t ripMapRipDefaultMetricSet(L7_uint32 mode, L7_uint32 val);

/*********************************************************************
* @purpose  Clears the default metric value. Sets it to its default value.
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipDefaultMetricClear();

/*********************************************************************
* @purpose  Get the RIP default route advertise mode
*
* @param    *val        @b{(output)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipDefaultRouteAdvertiseModeGet(L7_uint32 *val);


/*********************************************************************
* @purpose  Set the RIP default route advertise mode
*
* @param    val         @b{(input)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipDefaultRouteAdvertiseModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Clear the RIP default route advertise mode. Sets it to its default value.
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapRipDefaultRouteAdvertiseModeClear();

/*********************************************************************
* @purpose  Get the filter used for filtering redistributed routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeFilterGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *val);


/*********************************************************************
* @purpose  Set the filter used for filtering redistributed routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeFilterSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode, 
                                      L7_uint32 val);

/*********************************************************************
* @purpose  Clear the filter used for filtering redistributed routes from
*           the specified source protocol. Sets it to its default value.
*
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
extern L7_RC_t ripMapRipRouteRedistributeFilterClear(L7_REDIST_RT_INDICES_t protocol);

/*********************************************************************
* @purpose  Check if the source protocol exists in route redistribution table
*
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
extern L7_RC_t ripMapRipRouteRedistributeSourceGet(L7_REDIST_RT_INDICES_t protocol);

/*********************************************************************
* @purpose  Get the next source protocol in route redistribution table
*
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
extern L7_RC_t ripMapRipRouteRedistributeSourceGetNext(L7_REDIST_RT_INDICES_t *protocol);

/*********************************************************************
* @purpose  Get the metric and match type used for redistributing routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeParmsGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *metric, 
                                      L7_OSPF_METRIC_TYPES_t *matchType);


/*********************************************************************
* @purpose  Set the route redistribution mode metric and match type used
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeParmsSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 parmsMode, 
                                      L7_uint32 metric, L7_uint32 matchType);


/*********************************************************************
* @purpose  Clear the route redistribution mode metric and match type used used 
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeParmsClear(L7_uint32 protocol);


/*********************************************************************
* @purpose  Get the metric for redistributing routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMetricGet(L7_uint32 protocol, L7_uint32 *metric);


/*********************************************************************
* @purpose  Set the route redistribution mode and metric used 
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMetricSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 metricMode, 
                                      L7_uint32 metric);


/*********************************************************************
* @purpose  Clear the route redistribution metric used 
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMetricClear(L7_REDIST_RT_INDICES_t protocol);


/*********************************************************************
* @purpose  Get the match type used for redistributing routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMatchTypeGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *matchType);


/*********************************************************************
* @purpose  Set the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMatchTypeSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 matchMode, 
                                      L7_uint32 matchType);


/*********************************************************************
* @purpose  Clear the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeMatchTypeClear(L7_REDIST_RT_INDICES_t protocol);


/*********************************************************************
* @purpose  Get the redistribution mode for redistributing routes from
*           the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeModeGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *mode);


/*********************************************************************
* @purpose  Set the route redistribution mode for the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeModeSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode);


/*********************************************************************
* @purpose  Clear the route redistribution mode for the specified source protocol.
*
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
extern L7_RC_t ripMapRipRouteRedistributeModeClear(L7_REDIST_RT_INDICES_t protocol);

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  INTERFACE ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the routing internal interface number for the specified
*           IP address
*
* @param    ipAddr      @b{(input)}  Routing interface IP address
* @param    *intIfNum   @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfEntryGet(L7_uint32 ipAddr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the next sequential routing internal interface number
*           after the interface corresponding to the specified IP address
*
* @param    ipAddr      @b{(input)}  Routing interface IP address
* @param    *intIfNum   @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The first routing interface is returned if the specified IP 
*           address does not match any existing interface.
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfEntryNext(L7_uint32 ipAddr, L7_uint32 *intIfNum);


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE STATS
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the number of RIP response packets received by the RIP 
*           process which were subsequently discarded for any reason
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Receive Bad Packet count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfRcvBadPacketsGet(L7_uint32 intIfNum, L7_uint32 *count);

/*********************************************************************
* @purpose  Get the number of routes contained in valid RIP packets that 
*           were ignored for any reason
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Receive Bad Routes count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfRcvBadRoutesGet(L7_uint32 intIfNum, L7_uint32 *count);

/*********************************************************************
* @purpose  Get the number of triggered RIP updates actually sent 
*           on this interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Sent Triggered Updates count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfSentUpdatesGet(L7_uint32 intIfNum, L7_uint32 *count);


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the administrative mode of a RIP routing interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *mode       @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the administrative mode of a RIP routing interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the routing domain value for this RIP interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *domain     @b{(output)} Routing domain
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments This MIB object is listed as obsolete.
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfDomainGet(L7_uint32 intIfNum, L7_uint32 *domain);

/*********************************************************************
* @purpose  Set the routing domain value for this RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    domain      @b{(input)} Routing domain
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments This MIB object is listed as obsolete.
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfDomainSet(L7_uint32 intIfNum, L7_uint32 domain);

/*********************************************************************
* @purpose  Get the RIP authentication type for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *authType   @b{(output)} Interface authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAuthTypeGet(L7_uint32 intIfNum, L7_uint32 *authType);

/*********************************************************************
* @purpose  Set the RIP authentication type for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    authType    @b{(input)} Authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments For security reasons, the authentication key is always
*           cleared whenever the auth type is changed.  If setting a 
*           new key value, be sure to do it AFTER setting the auth type. 
*       
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType);

/*********************************************************************
* @purpose  Get the RIP authentication key for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *key        @b{(output)} Authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments The key value is output as an ASCIIZ string of whose length
*           is that of the key without any padding.
*
* @comments RFC 1724 specifies that authentication must not be bypassed by 
*           reading the MIB, but this function assumes that will be taken
*           care of at a higher layer.  DO NOT USE THIS API WITHOUT THE
*           APPROPRIATE SAFEGUARDS TO PREVENT EXPOSURE OF THE AUTHENTICATION
*           KEY!
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAuthKeyGet(L7_uint32 intIfNum, L7_uchar8 *key);

/*********************************************************************
* @purpose  Set the RIP authentication key for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    key         @b{(input)} Pointer to authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments The authentication key is passed in here as an ASCII string 
*           for the convenience of the user interface functions.  The MIB
*           key definition, however, is an OCTET STRING, which means it
*           is treated as a byte array, not an ASCII string.  The
*           conversion is done here for use in lower layer code.
*
* @comments The key type is based on the current value of the authType 
*           code.  This implies that the authType field must be set
*           before the authKey.
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key);

/*********************************************************************
* @purpose  Get the RIP authentication key ID for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *keyId      @b{(output)} Interface authentication key ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyIdGet(L7_uint32 intIfNum, L7_uint32 *keyId);

/*********************************************************************
* @purpose  Set the RIP authentication key ID for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    keyId       @b{(input)} Authentication key ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId);

/*********************************************************************
* @purpose  Get the RIP version used for sending updates on the specified 
*           interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *version    @b{(output)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfVerSendGet(L7_uint32 intIfNum, L7_uint32 *version);

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfVerSendSet(L7_uint32 intIfNum, L7_uint32 version);

/*********************************************************************
* @purpose  Get the RIP version used for receiving updates from the specified 
*           interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *version    @b{(output)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfVerRecvGet(L7_uint32 intIfNum, L7_uint32 *version);

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfVerRecvSet(L7_uint32 intIfNum, L7_uint32 version);

/*********************************************************************
* @purpose  Get the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *metric     @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfDefaultMetricGet(L7_uint32 intIfNum, L7_uint32 *metric);

/*********************************************************************
* @purpose  Set the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(input)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfDefaultMetricSet(L7_uint32 intIfNum, L7_uint32 metric);


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  GLOBAL INFO (NON-MIB)
 *---------------------------------------------------------------------
 */
/*:end ignore*/


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE INFO (NON-MIB)
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the IP address of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *ipAddr     @b{(output)} Interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *ipAddr);

/*********************************************************************
* @purpose  Get the subnet mask of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *netMask    @b{(output)} Interface subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *netMask);

/*********************************************************************
* @purpose  Get the interface type for the specified RIP interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *type       @b{(output)} Interface type (see nimIntfTypes for list) 
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments     
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *type);

/*********************************************************************
* @purpose  Get the current state of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *ifState    @b{(output)} RIP interface state (L7_UP or L7_DOWN)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfStateGet(L7_uint32 intIfNum, L7_uint32 *ifState);

/*********************************************************************
* @purpose  Get the state flags of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *flags      @b{(output)} RIP interface state flags
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfStateFlagsGet(L7_uint32 intIfNum, L7_uint32 *flags);

/*********************************************************************
* @purpose  Get the number of times the specified RIP interface has changed
*           state from up to down
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Interface up-down transition count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t ripMapIntfLinkEventsCtrGet(L7_uint32 intIfNum, L7_uint32 *val);

/*---------------------------------------------------------------------
 *                        API FUNCTIONS  -  INIT
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Checks to see if the interface type is valid to participate in RIP
*
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL ripMapIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Checks to see if the interface is valid to participate in RIP
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL ripMapIntfIsValid(L7_uint32 intIfNum);


/*:ignore*/
/* End Function Prototypes */
/*:end ignore*/

#endif /* _RIP_MAP_API_H_ */
