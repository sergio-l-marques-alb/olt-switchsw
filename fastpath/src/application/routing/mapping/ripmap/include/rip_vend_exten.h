/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    rip_vend_exten.h
* @purpose     RIP vendor-specific API functions
* @component   RIP Mapping Layer
* @comments    Does not contain any references to vendor headers or types.
* @create      05/19/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef _RIP_VEND_EXTEN_H_
#define _RIP_VEND_EXTEN_H_

#include "rip_config.h"

/*--------------------------------------------------*/
/* RIP Mapping Layer vendor API function prototypes */
/*--------------------------------------------------*/


/*-----------------*/
/* rip_vend_ctrl.c */
/*-----------------*/

/*********************************************************************
* @purpose  Initializes the vendor RIP component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapVendInit(void);


L7_RC_t ripMapVendDeinit(void);

/*********************************************************************
* @purpose  Initializes RIP control context structure
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapVendCtrlCtxInit(void);

/*********************************************************************
* @purpose  Signals router interface UP condition to vendor code
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapVendIntfUp(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Signals router interface DOWN condition to vendor code
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapVendIntfDown(L7_uint32 intIfNum);


/*------------------*/
/* rip_vend_exten.c */
/*------------------*/

/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  GLOBAL CONFIG
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Sets the RIP Admin mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipAdminModeSet(L7_uint32 mode);

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
L7_RC_t ripMapExtenRipSplitHorizonModeSet(L7_uint32 val);

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
L7_RC_t ripMapExtenRipAutoSummarizationModeSet(L7_uint32 val);

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
L7_RC_t ripMapExtenRipHostRoutesAcceptModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Set the default metric in the protocol code
*
* @param    val         @b{(input)} Default Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipDefaultMetricSet(L7_uint32 val);


/*********************************************************************
* @purpose  Set the RIP default route advertisement mode and metric
*
* @param    mode         @b{(input)} Default Route advertisement mode
* @param    metric       @b{(input)} Default Route metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipDefaultRouteAdvertiseSet(L7_uint32 mode, L7_uint32 metric);

/*********************************************************************
* @purpose  Set the Route Redistribute Filter
*
* @param    protocol         @b{(input)} The source protocol
* @param    val              @b{(input)} ACL used for filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeFilterSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 val);

/*********************************************************************
* @purpose  Applies the changed ACL filter to evaluate the routes to be
*           redistributed.
*
* @param    val              @b{(input)} ACL used for filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistFilterChange(L7_uint32 val);

/*********************************************************************
* @purpose  Set the route redistribute parms
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeParmsSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 metric, L7_OSPF_METRIC_TYPES_t matchType);

/*********************************************************************
* @purpose  Set the route redistribute metric
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeMetricSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 metric);

/*********************************************************************
* @purpose  Set the route redistribute matchType
*
* @param    protocol         @b{(input)} The source protocol
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeMatchTypeSet(L7_REDIST_RT_INDICES_t protocol, L7_OSPF_METRIC_TYPES_t matchType);

/*********************************************************************
* @purpose  Set the route redistribute mode
*
* @param    protocol         @b{(input)} The source protocol
* @param    mode             @b{(input)} Enable / Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeModeSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode);

/*********************************************************************
* @purpose  Recalculate the redistribution route table
*
* @param    *flashUpdateNeeded     @b{(input)} Flash update needed flag.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeRecalculate(L7_BOOL *flashUpdateNeeded);

/*********************************************************************
* @purpose  Purge all the redistributed external routes from RIP
*           map datastructures and in the RIP vendor code
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipPurgeRedistRoutes(void);

/*********************************************************************
* @purpose  Check if route can be redistributed.
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protocol           @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
*
* @returns  L7_SUCCESS    If the route can be redistributed
* @returns  L7_FAILURE    If the route cannot be redistributed
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeRouteCheck(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask, L7_IP_ADDR_t gateway, L7_REDIST_RT_INDICES_t protocol, L7_RTO_PROTOCOL_INDICES_t protoSpecific);

/*********************************************************************
* @purpose  Add the route to be redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
* @param    rtFlags            @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteAdd(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask,
                            L7_IP_ADDR_t gateway,
                            L7_RTO_PROTOCOL_INDICES_t protoSpecific,
                            L7_uint32 rtFlags);

/*********************************************************************
* @purpose  Delete the route that was redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  RTO can't provide the next hop or protocol of a deleted route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteDelete(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Modify the route that was redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
* @param    rtFlags            @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteChange(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask,
                               L7_IP_ADDR_t gateway,
                               L7_RTO_PROTOCOL_INDICES_t protoSpecific,
                               L7_uint32 rtFlags);

/*********************************************************************
* @purpose  Adds the redistributed route into the rip protocol route table
*
* @param    dst                @b{(input)} Network ip address
* @param    mask               @b{(input)} subnet mask of the network
* @param    gw                 @b{(input)} Gateway ip address
* @param    proto              @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
* @param    metric             @b{(input)} Metric of the route
*
* @returns  L7_SUCCESS
*
* @comments Sends a message to the protocol task to add the route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistRouteAdd(L7_IP_ADDR_t dst, L7_IP_MASK_t mask,
                                          L7_IP_ADDR_t gw, L7_REDIST_RT_INDICES_t proto, L7_uint32 metric);

/*********************************************************************
* @purpose  Deletes the redistributed route from the rip protocol route table
*
* @param    dst                @b{(input)} Network ip address
* @param    mask               @b{(input)} subnet mask of the network
* @param    gw                 @b{(input)} Gateway ip address
* @param    proto              @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
*
* @returns  L7_SUCCESS
*
* @comments Sends a message to the protocol task to delete the route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistRouteDelete(L7_IP_ADDR_t dst, L7_IP_MASK_t mask,
                                             L7_IP_ADDR_t gw, L7_REDIST_RT_INDICES_t proto);

/*********************************************************************
* @purpose  Convert the route type from rto route type to redistribution route type.
*
* @param    routeType         @b{(input)} RTO route tyep
* @param    protocol          @b{(output)} Redistribution route type
* @param    rtFlags           @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistRouteTypeConvert(L7_uint32 routeType,
                                               L7_REDIST_RT_INDICES_t *protocol,
                                               L7_uint32 rtFlags);

/*********************************************************************
* @purpose  Convert the route type from rto ospf route type to redistribution ospf route type.
*
* @param    rtoOspfType         @b{(input)} RTO ospf route type
* @param    *redistOspfType     @b{(output)} Redistribution ospf route type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistOspfMatchTypeConvert(L7_uint32 rtoOspfType, L7_OSPF_METRIC_TYPES_t *redistOspfType);




/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  INTERFACE CONFIG
----------------------------------------------------------------------
*/

/* ######################################################################
 * NOTE:  All intf config 'set' functions return with a successful return
 *        code in the event the desired RIP interface structure is not
 *        found.  This allows for pre-configuration of RIP routing
 *        interfaces (the config gets picked up in rip_ifinit() when the
 *        interface structure is created).
 *
 * NOTE:  All intf config functions accept a pointer to a RIP vendor
 *        interface structure, if known.  Otherwise, this p parm must be
 *        set to L7_NULL.  The intIfNum parm is required in either case.
 *        (This mechanism allows these same functions to be used both when
 *        handling config requests from the user interface as well as when
 *        the vendor RIP code asks for its interface config information.)
 * ######################################################################
 */

/*********************************************************************
* @purpose  Set the RIP authentication type for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    authType    @b{(input)} Authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthTypeSet(L7_uint32 intIfNum, void *p, L7_uint32 authType);

/*********************************************************************
* @purpose  Set the RIP authentication key for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    key         @b{(input)} Authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The authentication key is always passed using the maximum
*           allowed key length, and is already padded with zeroes on
*           the right hand side.
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthKeySet(L7_uint32 intIfNum, void *p, L7_uchar8 *key);

/*********************************************************************
* @purpose  Set the RIP authentication key id for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    keyId       @b{(input)} Authentication key id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthKeyIdSet(L7_uint32 intIfNum, void *p, L7_uint32 keyId);

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    version     @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfVerSendSet(L7_uint32 intIfNum, void *p, L7_uint32 version);

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    version     @b{(input)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfVerRecvSet(L7_uint32 intIfNum, void *p, L7_uint32 version);


/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  GLOBAL
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Get the number of route changes made by RIP to the
*           IP Route Database
*
* @param    *val        @b{(output)} RIP route changes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteChangesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of responses sent to RIP queries from other systems
*
* @param    *val        @b{(output)} RIP query response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenQueriesGet(L7_uint32 *val);


/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  INTERFACE
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Get the number of RIP response packets received by the RIP
*           process which were subsequently discarded for any reason
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Receive Bad Packet count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfRcvBadPacketsGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of routes contained in valid RIP packets that
*           were ignored for any reason
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Receive Bad Routes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfRcvBadRoutesGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of triggered RIP updates actually sent
*           on this interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Sent Triggered Updates count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfSentUpdatesGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the current operational state of the specified RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} RIP interface state (L7_UP or L7_DOWN)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfStateGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the state flags of the specified RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} RIP interface state flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfStateFlagsGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of times this RIP interface has changed
*           its state (from up to down)
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Link Events Counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfLinkEventsCtrGet(L7_uint32 intIfNum, void *p, L7_uint32 *val);

#endif /* _RIP_VEND_EXTEN_H_ */

