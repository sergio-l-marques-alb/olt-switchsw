/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ospf_vend_exten.h
* @purpose     OSPF vendor-specific API functions
* @component   OSPF Mapping Layer
* @comments    Does not contain any references to vendor headers or types.
* @create      04/12/2001
* @author      gpaussa
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _OSPF_VEND_EXTEN_H_
#define _OSPF_VEND_EXTEN_H_

#include "l7_common.h"
#include "ospf_util.h"


/* NOTE: Make sure L7_AUTH_MAX_SIMPLE_KEY_OSPF definition matches this value */
#define OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE   8   /* vendor code maximum */

/* NOTE: Make sure L7_AUTH_MAX_MD5_KEY_OSPF definition matches this value */
#define OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5      16  /* vendor code maximum */

/* define this as the larger of the two */
#define OSPF_MAP_EXTEN_AUTH_MAX_KEY          OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5


/*---------------------------------------------------*/
/* OSPF Mapping Layer vendor API function prototypes */
/*---------------------------------------------------*/


/*------------------*/
/* ospf_vend_ctrl.c */
/*------------------*/

/*********************************************************************
* @purpose  Initializes the OSPF task.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Causes Routing subsystem to be started if it is not already.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfTaskCreate(void);

/*********************************************************************
* @purpose  Deletes the Virata OSPF task.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The Virata ATIC subsystem cannot be stopped, but had it 
*           failed the system would have crashed.
*       
* @end
*********************************************************************/
void ospfMapOspfTaskDelete(void);


/*********************************************************************
* @purpose  Initializes the OSPF component.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Causes Routing subsystem to be started if it is not already.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVendorInit(void);

/*********************************************************************
* @purpose  Uninitialize the OSPF protocol engine. Does the reverse of
*           actions taken in ospfMapOspfVendorInit().
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVendorUninit(void);

/*********************************************************************
* @purpose  Creates an OSPF router interface.
*
* @param    intIfNum    router internal interface number
* @param    ipAddr      IP address of router interface
* @param    netMask     network/subnet mask for router interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The first created interface causes some remaining OSPF init
*           to be completed.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfCreate(L7_uint32 intIfNum, L7_uint32 ipAddr, 
                              L7_uint32 netMask);

/*********************************************************************
* @purpose  Creates an OSPF virtual interface.
*
* @param    index       index into the pOspfVlinkInfo needed to store
*                       the IFO handle returned by the vendor.
* @param    areaId      virtual transit areaId
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The first created interface causes some remaining OSPF init
*           to be completed.
*
* @notes    The function should only be called if OSPF is enabled on an
*           interface as it causes the interface to immediately be enabled
*           in the core OSPF stack.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfCreate(L7_uint32 index, L7_uint32 areaId, 
                                  L7_uint32 neighbor);

/*********************************************************************
* @purpose  Deletes an OSPF router interface.
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Cleans up resources associated with the router 
*           interface.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Deletes OSPF virtual interface.
*
* @param    index       index into the pOspfVlinkInfo needed to store
*                       the IFO handle returned by the vendor.
* @param    areaId      virtual transit areaId
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Cleans up resources associated with the router 
*           interface.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfDelete(L7_uint32 index, L7_uint32 areaId, L7_uint32 neighbor);

/*********************************************************************
* @purpose  Deletes an OSPF area.
*
* @param    areaId      AreaId of the area to be deleted
*
* @returns  L7_SUCCESS  Area was successfully deleted
* @returns  L7_ERROR    Specified area has not been configured
* @returns  L7_FAILURE  Area contains active interface(s) (these
*                       need to be deleted before removing the area)
*
* @notes    Cleans up resources associated with the area 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaDelete(L7_uint32 areaId);

/*********************************************************************
* @purpose  Dynamically changes the interface metric value.
*
* @param    intIfNum    router internal interface number
* @param    ifDataRate  current interface data rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Typically invoked when the interface link speed changes
*           so that the appropriate metric value can be advertised.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfIntfMetricChange(L7_uint32 intIfNum, L7_uint32 ifDataRate);

/*********************************************************************
* @purpose  Signals router interface UP condition to Routing code.
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
L7_RC_t ospfMapOspfIntfUp(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Signals router interface DOWN condition to Routing code.
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
L7_RC_t ospfMapOspfIntfDown(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Signals virtual interface DOWN condition to Routing code.
*
* @param    index     virtual link configuration index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A virtual link will get automatically activated and moved 
*           to "up" state when the virtual neighbor is discovered
*           hence we do not need the corresponding ospfMapOspfVirtIntfUp
*           function.  However the ospfMapOspfVirtIntfDown function
*           is necessary for the case when a user wants to explicitly
*           deactivate/delete a preconfigured virtual link.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfVirtIntfDown(L7_uint32 index);

/*-------------------*/
/* ospf_vend_exten.c */
/*-------------------*/

/*********************************************************************
* @purpose  Sets the Ospf Admin mode
*
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOspfAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the Ospf Router ID
*
* @param    routerID   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRouterIDSet(L7_uint32 routerID);

/*********************************************************************
* @purpose  Handle a change to a change in whether the router is an
*           ASBR. The router becomes an ASBR when it is configured to
*           redistribute from any source. We need to tell the protocol
*           engine of the change so that it can correctly set the E bit
*           in its router LSAs.
*
* @param    mode   @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsbrStatusSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Purge all external LSAs.
*
* @param    none
*
* @returns  L7_SUCCESS 
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenPurgeExternalLsas();

/*********************************************************************
* @purpose  Iterates through all routes in the OSPF redistributeion tree
*           and deletes those routes which are no longer used. For each
*           deleted route, notifies the protocol engine to stop 
*           advertising the route.
*
* @param    none
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE otherwise
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenUnusedRoutesDelete();

/*********************************************************************
*
* @purpose  Stop redistributing a given destination network.
*
* @param ipAddr @b{(input)} destination address
* @param netMask @b{(input)} destination network mask
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapWithdrawRedistRoute(L7_uint32 ipAddr, L7_uint32 netMask);

/*********************************************************************
* @purpose  When there is a configuration change that affects route
*           redistribution, reevaluate all the best routes in RTO against
*           the new redistribution policy.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRedistReevaluate();

/*********************************************************************
* @purpose  Configures if Ospf is compatible with RFC 1583
*
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If enabled, preference rules remain those as defined by
* @notes    RFC 1583.  If disabled, preference rules are as stated 
* @notes    in Section 16.4.1 of the OSPFv2 standard. When disabled,
* @notes    greater protection is provided against routing loops.   
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRfc1583CompatibilitySet(L7_uint32 mode);

/*********************************************************************
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVendExtenTrapModeSet(L7_uint32 trapType);

/*********************************************************************
* @purpose  Set the maximum number of non-default AS-external-LSAs entries 
*           that can be stored in the link-state database.
*          
* @param    val         number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The default value is -1. If the value is -1, then there
*           is no limit. When the number of non-default AS-external-LSAs
*           in a router's link-state database reaches ExtLsdbLimit, the
*           router enters overflow state. The router never holds more than
*           ExtLsdbLimit non-default AS-external-LSAs in it database
*           ExtLsdbLimit MUST be set identically in all routers attached
*           to the OSPF backbone and/or any regular OSPF area. (that is,
*           OSPF stub areas and NSSAs are excluded.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbLimitSet ( L7_int32 val );

/*********************************************************************
* @purpose  Report to the OSPF protocol a change to maximum paths.
*
* @param    maxPaths         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMaxPathsSet ( L7_uint32 maxPaths);

/*********************************************************************
* @purpose  Set a Bit Mask indicating whether the router is
*           forwarding IP multicast (Class D) datagrams.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is based on algorithms defined in Multicast Extensions to OSPF rfc1850.
*           Bit 0, indicates that router can forward IP multicast datagrams in the router's
*           directly attached areas (called intra-area multicast routing).
*
*           Bit 1, indicates that router can forward IP multicast datagrams between OSPF
*           areas (called inter-area multicast routing).
*
*           Bit 2, indicates that router can forward IP multicast datagrams between Auto-
*           nomous Systems (called inter-AS multicast routing).
*
*           Only certain combinations of bit settings are allowed:
*           0 (the default) - multicast forwarding is not enabled.
*           1 - intra-area multicasting only.
*           3 - intra-area and inter-area multicasting.
*           5 - intra-area and inter-AS multicasting.
*           7 - multicasting to all areas. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMulticastExtensionsSet ( L7_int32 val );

/*********************************************************************
* @purpose  Set the number of seconds, that after entering overflow
*           state a router waits before attempting to leave overflow
*           state.
*          
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This allows the router to again originate non-default
*           AS-external LSAs.
*           The default value is 0 - the router will not leave
*           overflow state until it is restarted.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExitOverflowIntervalSet ( L7_int32 val );

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
L7_RC_t ospfMapExtenSpfDelaySet ( L7_int32 spfDelay );

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
L7_RC_t ospfMapExtenSpfHoldtimeSet ( L7_int32 spfHoldtime );

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
*           Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDemandExtensionsSet ( L7_int32 val );


/*********************************************************************
* @purpose  Set the router's support for TOS routing.
*          
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports TOS routing
*           L7_FALSE: The router does not support TOS routing
*
* @end
*********************************************************************/
L7_RC_t ospfMapTOSSupportSet ( L7_int32 val );

/*********************************************************************
* @purpose  Allows a user to set the value of MtuIgnore Flag.
*
* @param    val        @b{(input)} L7_TRUE / L7_FALSE
* @param    intIfNum   @b{(input)} L7_uint32
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    L7_TRUE - Router ignores MTU value while forming adjacencies
*           L7_FALSE- Router forms adjacency taking into account the MTU Value
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfMtuIgnoreSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Configures the value of MaxIpMTUsize
*
* @param    ipMtu   @b{(input)} L7_uint32
* @param    intIfNum   @b{(input)} L7_uint32
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This MTU value is used in DD packets
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu);


/*********************************************************************
* @purpose  Get the current version number of the OSPF protocol.
*
* @param    *val        version number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVersionNumberGet ( L7_int32 *val );

/*********************************************************************
* @purpose  Get a Bit Mask indicating whether the router is
*           forwarding IP multicast (Class D) datagrams.
*          
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is based on algorithms defined in Multicast Extensions to OSPF rfc1850.
*           Bit 0, indicates that router can forward IP multicast datagrams in the router's
*           directly attached areas (called intra-area multicast routing).
*
*           Bit 1, indicates that router can forward IP multicast datagrams between OSPF
*           areas (called inter-area multicast routing).
*
*           Bit 2, indicates that router can forward IP multicast datagrams between Auto-
*           nomous Systems (called inter-AS multicast routing).
*
*           Only certain combinations of bit settings are allowed:
*           0 (the default) - multicast forwarding is not enabled.
*           1 - intra-area multicasting only.
*           3 - intra-area and inter-area multicasting.
*           5 - intra-area and inter-AS multicasting.
*           7 - multicasting to all areas. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMulticastExtensionsGet ( L7_int32 *val );

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
*           Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDemandExtensionsGet ( L7_int32 *val );

/*********************************************************************
* @purpose  Get the opaque AS LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) count value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOpaqueASLSACountGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the external LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) external LSA count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSACountGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the self originated external LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenSelfOrigExternalLSACountGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the external LSA checksum value
*
* @param    *val        pointer to output location
*                         @b{(output) external LSA checksum value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSAChecksumGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of new link-state advertisements that have
*           been originated
*
* @param    *val        pointer to output location
*                         @b{(output) new LSAs originated count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNewLSAOrigGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of link-state advertisements received for
*           new instantiations
*
* @param    *val        pointer to output location
*                         @b{(output) LSAs received count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNumLSAReceivedGet(L7_uint32 *val);

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
L7_RC_t ospfMapExtenIntfAdminModeSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the Ospf Admin mode for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode);

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
L7_RC_t ospfMapExtenIntfAreaIdSetTest ( L7_uint32 ipAddress, L7_int32 intIfNum,
                                        L7_uint32 val );
/*********************************************************************
* @purpose  Sets the Ospf Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAreaIdPack(L7_uint32 intIfNum, L7_uint32 area);

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
L7_RC_t ospfMapExtenIntfPrioritySetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                          L7_int32 val );

/*********************************************************************
* @purpose  Sets the Ospf Priority for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    priority   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfPrioritySet(L7_uint32 intIfNum, L7_uint32 priority);

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
L7_RC_t ospfMapExtenIntfHelloIntervalSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                              L7_uint32 seconds);

/*********************************************************************
* @purpose  Sets the Ospf Hello Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfHelloIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapExtenIntfDeadIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                            L7_uint32 seconds);

/*********************************************************************
* @purpose  Sets the Ospf Dead Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDeadIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapExtenIntfRxmtIntervalSetTest(L7_uint32 ipAddr,
                                            L7_uint32 intIfNum,
                                            L7_uint32 seconds);

/*********************************************************************
* @purpose  Sets the Ospf Retransmit Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfRxmtIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapExtenIntfNbmaPollIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                                L7_uint32 seconds);

/*********************************************************************
* @purpose  Sets the Ospf Nbma Poll Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfNbmaPollIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapExtenIntfTransitDelaySetTest (L7_uint32 ipAddr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 seconds);

/*********************************************************************
* @purpose  Sets the Ospf Transit Delay for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfTransitDelaySet(L7_uint32 intIfNum, L7_uint32 seconds);


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
L7_RC_t ospfMapExtenIntfLsaAckIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapExtenIntfAuthTypeSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 authType);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Type for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType);

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    key      @b{(input)}
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
L7_RC_t ospfMapExtenIntfAuthKeySetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                        L7_uchar8 *key);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    key   @b{(input)} authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key);

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key Id for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    keyId    @b{(input)} key identifier
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
L7_RC_t ospfMapExtenIntfAuthKeyIdSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 keyId);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key Id for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    keyId       @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId);

/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfVirtTransitAreaIdSet(L7_uint32 intIfNum, L7_uint32 area);

/*********************************************************************
* @purpose  Sets the Ospf Virtual Neighbor for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    neighbor   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfVirtIntfNeighborSet(L7_uint32 intIfNum, L7_uint32 areaId, 
                                            L7_uint32 neighbor);

/*********************************************************************
* @purpose  Get the number times the intra-area route table has been 
*           calculated
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) SPF calculation count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNumSPFRunsGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of reachable area border routers
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area border router count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaBorderCountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number link-state advertisements in this 
*           area's link-state database (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area LSA count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSACountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the 32-bit unsigned checksum of the link-state advertisements 
*           LS checksums in this contained in this area's link-state database
*           (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area LSA checksum value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSAChecksumGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Check if the area exists in the "active" OSPF stack
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    The core OSPF stack may not allow for pre-configuration of
*           parameters.  This check tests if the area currently exists
*           in the core stack.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaExists(L7_uint32 areaId);


/*********************************************************************
* @purpose  Free the area in the "active" OSPF stack
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS, if area has not been configured
* @returns  L7_FAILURE, otherwise
*
* @notes    The current stack does not allow for individual removal
*           of an area. An instance of an area is automatically created
*           when it becomes associated with an interface.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaFree(L7_uint32 areaId);

/*********************************************************************
* @purpose  Sets the specified Area Range
*
* @param    areaID  @b{(input)}
* @param    Area Range Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeSet(L7_uint32 areaId, struct L7_ospfAreaAddrRangeInfo_s *areaAddrRange);
/*********************************************************************
* @purpose  Gets the Ospf LSA Acknowledgement Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * seconds   @b{(output)} Number of seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLSAAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);
/*********************************************************************
* @purpose  Gets the Ospf State for the specified interface 
*
* @param    ipAddr @b{(input)}   ip Address
* @param    intIfNum @b{(input)} internal interface number
* @param    *val   @b{(output)} State val
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfStateGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Designated Router Id for the specified interface 
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} internal interface number
* @param    * val   @b{(output)} Designated Router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDrIdGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Backup Designated Router Id for the 
*           specified interface 
*
* @param    ipAddr @b{(input)}   Ip Address of the Interface
* @param    intIfNum @b{(input)} internal interface number
* @param    * val   @b{(output)} Backup Designated Router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfBackupDrIdGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                      L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of times this  OSPF  interface  has 
*            changed its state 
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} internal interface number
* @param    *val   @b{(output)} Link Events Counter
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLinkEventsCounterGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                             L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSAs for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSAsGet(L7_uint32 intIfNum, L7_uint32 *val);
/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSA Checksum for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Checksum
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSACksumGet(L7_uint32 intIfNum, L7_uint32 *val);
/*********************************************************************
* @purpose  Gets the Ospf IP Address for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *val);
/*********************************************************************
* @purpose  Gets the Ospf Subnet Mask for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *val);
/*********************************************************************
* @purpose  Check if the Router is a OSPF Area Border Router
*
* @param    *mode        pointer to output location
*                         @b{(output) L7_TRUE or L7_FALSE}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAbrStatusGet(L7_uint32 *mode);
/*********************************************************************
* @purpose  Check if the capability of Storing Opaque LSAs 
*
* @param    *mode        pointer to output location
*                         @b{(output) L7_TRUE or L7_FALSE}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOpaqueLSAModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Get the Exit Overflow Interval value 
*
* @param    *val        pointer to output location
*                         @b{(output) Exit Overflow Interval value }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExitOverflowIntervalGet(L7_uint32 *val);
/*********************************************************************
* @purpose  Get the External LSA Limit 
*
* @param    *val        pointer to output location
*                         @b{(output) External LSA Limit }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSALimitGet(L7_int32 *val);

/*********************************************************************
* @purpose  Get the AdminStat of the router
*
* @param    *val        pointer to output location
*                         @b{(output) AdminStat }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAdminStatGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Check the External Routing Capability for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) external routing capability val}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaExternalRoutingCapabilityGet(L7_uint32 areaId, L7_uint32 *val);
/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA type for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Type}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSATypeGet(L7_uint32 areaId, L7_uint32 *val);
/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Age for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Age}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAAgeGet(L7_uint32 areaId, L7_int32 Type, 
                                       L7_uint32 Lsid, L7_uint32 RouterId, 
                                       L7_uint32 *val);
/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Checksum for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Checksum}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSACksumGet(L7_uint32 areaId, L7_int32 Type, 
                                         L7_uint32 Lsid, L7_uint32 RouterId,
                                         L7_uint32 *val);
/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Advertisement packet for the 
*           specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Advertisement Packet}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAAdvertiseGet(L7_uint32 areaId, L7_int32 Type, 
                                             L7_uint32 Lsid, L7_uint32 RouterId,
                                             L7_char8 **buf, L7_uint32 *buf_len);
/*********************************************************************
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGet(L7_uint32 *p_areaId);
/*********************************************************************
* @purpose  Get the Ospf Area Id for the next area, of the Ospf Router ,
*           after the area Id specified
*
* @param    areaId      @b{(input)} area identifier
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGetNext(L7_uint32 areaId, L7_uint32 *p_areaId);

/*********************************************************************
* @purpose  Test if the ospf demand value is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDemandAgingSetTest( L7_uint32 ipAddr,
                                            L7_int32 intIfNum,
                                            L7_int32 val );

/*********************************************************************
* @purpose  Sets the Ospf Demand Aging for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val   @b{(input)}  (TRUE/FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDemandAgingSet(L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t ospfMapExtenIfTypeSetTest ( L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the OSPF interface type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   needs intIfNum only.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTypeSet(L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
* @purpose  Convert a LVL7 Cfg ifType to a Vendor ifType
*
* @param    cfgIfType   LVL7 Cfg ifType
* @param    pVendIfType return value for vendor ifType
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if given invalid parameter
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapVendIfTypeGet(L7_uint32 cfgIfType, L7_uint32 *pVendIfType);

/*********************************************************************
* @purpose  Test if the entry status is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val        status
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
L7_RC_t ospfMapExtenIfStatusSetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                      L7_int32 val );

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    intIfNum   interface
* @param    val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative. needs intIfNum only. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfStatusSet(L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
* @purpose  Test if Ospf multicast forwarding for the specified
*           interface is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The way multicasts should be forwarded on this interface;
*             not forwarded, forwarded as data link multicasts, or
*             forwarded as data link unicasts.  Data link multicasting
*             is not meaningful on point to point and NBMA interfaces,
*             and setting ospfMulticastForwarding to 0 effectively
*             disables all mutlicast forwarding.
*
*             Possible values:
*               blocked (1)
*               multicast (2)
*               unicast (3) "
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingSetTest ( L7_uint32 ipAddr,
                                                     L7_int32 intIfNum,
                                                     L7_int32 val );

/*********************************************************************
* @purpose  Delete the way multicasts should be forwarded.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    val         multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting ospfMulticastForwarding to 0 disables all multicast forwarding.needs intIfNum only.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingSet(L7_uint32 intIfNum, L7_int32 val);
                                                                              
/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfVirtTransitAreaIdSet(L7_uint32 intIfNum, L7_uint32 area);

/*********************************************************************
* @purpose  Gets the Ospf LSA Acknowledgement Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * seconds   @b{(output)} Number of seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLSAAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSAs for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSAsGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSA Checksum for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Checksum
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSACksumGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf IP Address for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Subnet Mask for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *val);

/* ospfIf */
/*********************************************************************
* @purpose  Get the Interface Entry.
*
* @param    ipAddr      Ip address
* @param    intf        interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfEntryGet(L7_uint32 ipAddr, L7_uint32 intf);

/*********************************************************************
* @purpose  Get the next Interface Entry.
*
* @param    *ipAddr    Ip address
* @param    *intf  interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfEntryNext(L7_uint32 *ipAddr, L7_uint32 *intf);

/*********************************************************************
* @purpose  Get the priority of this interface.  
*
* @param    IPAddress   Ip address
* @param    rtrIfNum    routing interface
* @param    *val        Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that router is not eligible to become
*           the designated router on this particular  network.needs only intIfNum.   
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRtrPriorityGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                     L7_int32 *val);

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link state 
*           update packet over this interface.
*
* @param    ipAddr      ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTransitDelayGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                      L7_int32 *val);

/*********************************************************************
* @purpose  Get the number of seconds between link-state advertisement retransmissions, 
*           for  adjacencies belonging to this interface.
*
* @param    ipAddr   Ip address
* @param    rtrIfNum routing interface
* @param    *val     seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRetransIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                         L7_int32 *val);

/*********************************************************************
* @purpose  Get the time in seconds between Hello packets that router
*           sends on the interface.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfHelloIntervalGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                       L7_int32 *val);

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets have 
*           not been seen before it's neighbors declare the router down.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This should be some multiple of Hello interval. This value 
*           must be the same for all routers attached to a common network.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRtrDeadIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                         L7_int32 *val);

/*********************************************************************
* @purpose  Get the larger time interval, in seconds, between the Hello packets
*           sent to an inactive nonbroadcast multiaccess neighbor.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfPollIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                      L7_int32 *val);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    IPAddr      Ip address
* @param    intIfNum    internal interface number
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfStatusGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                L7_int32 *val);

/*********************************************************************
* @purpose  Get the way multicasts should be forwarded on this interface.
*
* @param    IPAddr   Ip address
* @param    intIfNum internal interface number
* @param    *val     multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingGet(L7_uint32 ipAddr,
                                             L7_uint32 intIfNum,
                                             L7_int32 *val);

/*********************************************************************
* @purpose  To indicate whether Demand OSPF procedures should be performed on this interface.
*
* @param    IPAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Demand OSPF procedures include hello supression to FULL neighbors
*           and setting the DoNotAge flag on proogated LSAs.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfDemandGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum, L7_int32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Area Id for the specified interface 
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface number  
* @param    *area       area id
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
L7_RC_t ospfMapExtenIfAreaIdGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Interface Type
*
* @param    IPAddress   Ip address
* @param    rtrIfNum    routing interface number  
* @param    *val        integer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
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
L7_RC_t ospfMapExtenIfTypeGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                              L7_int32 *val);

/*********************************************************************
* @purpose  Get the OSPF interface's administrative status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfAdminStatGet(L7_uint32 intIfNum, L7_int32 *val);

/* ospfIf */
/*********************************************************************
* @purpose  Get a particular TOS metric for a non-virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricEntryGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS);

/*********************************************************************
* @purpose  Get the next TOS metric for a non-virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *IPAddress  Ip address
* @param    *LessIf     interface
* @param    *TOS        TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricEntryNext(L7_uint32 *ipAddress, L7_uint32 *intIfNum, L7_int32 *TOS);

/*********************************************************************
* @purpose  Get the OSPF interface metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    *val        metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The default value of the TOS 0 Metric is 10^8 / ifSpeed.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 *val);

/*********************************************************************
* @purpose  Get the OSPF interface metric value status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 *val);

/*********************************************************************
* @purpose  Test to set the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueSetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val);

/*********************************************************************
* @purpose  Delete the metric value.
*
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueSet( L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val);

/*********************************************************************
* @purpose  Set the passive mode of an interface
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(input)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfPassiveModeSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.needs only intIfNum.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusSetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val);

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.needs only intIfNum.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusSet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val);

/* ospfVirtIf */
/*********************************************************************
* @purpose  Determines if the specified interface is a virtual interface
*
* @param    intIfNum    Internal interface number
*
* @returns  L7_TRUE     if the specified interface exists & is a virtual
*                       interface.
* @returns  L7_FALSE    if the specified interface does not exist or
*                       is not a virtual interface
*
* @notes    none.
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsVirtIfGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get a single Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEntryGet(L7_uint32 AreaId, L7_uint32 Neighbor);

/*********************************************************************
* @purpose  Get the next Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     Id
* @param    *Neighbor   entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEntryNext(L7_uint32 *AreaId, L7_uint32 *Neighbor);

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfTransitDelayGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRetransIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This value must be the same for the virtual neighbor.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfHelloIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This value must be the same for the virtual neighbor.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the OSPF virtual interface state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        interface state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStateGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the OSPF virtual interface metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfMetricGet(void *IFO_Id, L7_uint32 *metric);

/*********************************************************************
* @purpose  Get the number of state changes or error events on this Virtual Link.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEventsGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStatusGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val);

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
L7_RC_t ospfMapExtenVirtIfTransitDelaySetTest(L7_uint32 AreaId,
                                              L7_uint32 Neighbor,
                                              L7_int32 val);

/*********************************************************************
* @purpose  Set the interface transit delay
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfTransitDelaySet(L7_uint32 index, L7_int32 val);

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
L7_RC_t ospfMapExtenVirtIfRetransIntervalSetTest(L7_uint32 AreaId,
                                                 L7_uint32 Neighbor,
                                                 L7_int32 val);

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRetransIntervalSet(L7_uint32 index, L7_int32 val);

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
L7_RC_t ospfMapExtenVirtIfHelloIntervalSetTest( L7_uint32 AreaId,
                                                L7_uint32 Neighbor,
                                                L7_int32 val);

/*********************************************************************
* @purpose  Delete the time.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfHelloIntervalSet(L7_uint32 index, L7_int32 val);

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
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalSetTest( L7_uint32 AreaId,
                                                  L7_uint32 Neighbor,
                                                  L7_int32 val);

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalSet(L7_uint32 index, L7_int32 val);

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
L7_RC_t ospfMapExtenVirtIfStatusSetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 val);

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStatusSet(L7_uint32 index, L7_int32 val);

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
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
L7_RC_t ospfMapExtenVirtIfAuthTypeSetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val);

/*********************************************************************
* @purpose  Set the authentication type.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthTypeSet(L7_uint32 index, L7_int32 val);

/*********************************************************************
* @purpose  Tests if the Authentication Key is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key
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
L7_RC_t ospfMapExtenVirtIfAuthKeySetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                         L7_uchar8 *buf);

/*********************************************************************
* @purpose  Set the Authentication Key.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    *buf        authentication key
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeySet(L7_uint32 index, L7_uchar8 *buf);

/*********************************************************************
* @purpose  Tests if the Authentication Key Id is settable.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    keyId       key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeyIdSetTest(L7_uint32 index, L7_uint32 keyId);

/*********************************************************************
* @purpose  Set the Authentication Key Id.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    keyId       key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeyIdSet(L7_uint32 index, L7_uint32 keyId);

/*********************************************************************
* @purpose  Get the Ospf Router Id for the first neighbour of the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    ipAddress		 L7_uint32 ipAddress of the neighbor               
* @param    * p_routerId   @b{(output)} router Id of the Neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrRouterIdGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId);

/*********************************************************************
* @purpose  Get the Ospf Router Id for the next neighbour, after the
*           specified router Id of the neighbour, of the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *p_routerId      L7_uint32 next router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrRouterIdGetNext(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId);

/*********************************************************************
* @purpose  Get the Ospf IP Address of the specified neighbour router Id
*           of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIPAddrGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Interface index of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 interface index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIfIndexGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Options of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 options
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrOptionsGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Area of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 area
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrAreaGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf dead timer remining of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrDeadTimerRemainingGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  How long adjacency for a given neighbor on given interface 
*           has been in Full state.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrUptimeGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Priority of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 priority
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPriorityGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf State of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrStateGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Interface State of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIntfStateGet(L7_uint32 intIfNum, L7_uint32 routerId,
                                    L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Events Counter of the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 events counter
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Check if Ospf Hellos are suppressed to the specified neighbour 
*           router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 Hellos Suppressed ? L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrHellosSuppressedGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the current length of the retransmission queue of the 
*           specified neighbour router Id of the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 LS Retransmission Queue Length
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrLSRetransQLenGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/* ospfNbr */
/*********************************************************************
* @purpose  Get a single neighbor entry information.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEntryGet(L7_uint32 IpAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the next neighbor entry information.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    *IpAddr             Ip address
* @param    *intIfNum           internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEntryNext(L7_uint32 *IpAddr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusGet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    'dynamic' and 'permanent' refer to how the neighbor became known.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrPermanenceGet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
* @purpose  Test to set the priority of this neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that neighbor is not eligible to become
*           the designated router on this particular network.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPrioritySetTest(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
* @purpose  Delete the priority of this neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that neighbor is not eligible to become
*           the designated router on this particular network.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPrioritySet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusSetTest(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusSet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val);

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/


/*********************************************************************
* @purpose  Sets the specified Area Range
*
* @param    areaID  @b{(input)}
* @param    Area Range Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeSet(L7_uint32 areaId, struct L7_ospfAreaAddrRangeInfo_s *areaAddrRange);

/*********************************************************************
* @purpose  Get the number times the intra-area route table has been 
*           calculated
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) SPF calculation count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNumSPFRunsGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of reachable area border routers
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area border router count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaBorderCountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the link-state advertisements statistics for this
*           area's link-state database (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSAStatGet(L7_uint32 areaId, L7_OspfAreaDbStats_t * stats);

/*********************************************************************
* @purpose  Get the 32-bit unsigned checksum of the link-state advertisements 
*           LS checksums in this contained in this area's link-state database
*           (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area LSA checksum value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSAChecksumGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the area summary.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The variable ospfAreaSummary controls the import of
*           summary LSAs into stub areas. It has no effect on other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummaryGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of Autonomous System border routers 
*           reachable  within  this area.  
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is initially zero, and is calculated in each SPF Pass.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsBdrRtrCountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the OSPF area entry status.
*          
* @param    areaId      type
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if the area is currently actie
* @returns  L7_FAILURE  otherwise
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get new OSPF area entry.
*          
* @param    AreaId      new entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    entry information describes the configured parameters and
*           cumulative statistics of one of the router's attached areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaEntryGet(L7_uint32 areaId);

/*********************************************************************
* @purpose  Get next OSPF area entry.
*          
* @param    AreaId      new entry
* @param    nextAreaId  next area id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    entry information describes the configured parameters and
*           cumulative statistics of one of the router's attached areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaEntryGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId);

/*********************************************************************
* @purpose  Get the area's support for importing AS external link-state advertisements.
*          
* @param    AreaId      type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Test if the area's support for importing AS external link-state
*           advertisements is settable
*          
* @param    areaId      type
* @param    val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternSetTest(L7_uint32 areaId, L7_uint32 val);

/*********************************************************************
* @purpose  Set the area's support for importing AS external link-state
*           advertisements.
*          
* @param    AreaId      type
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternSet(L7_uint32 areaId, L7_uint32 val);

/*********************************************************************
* @purpose  Test if the area summary is settable
*          
* @param    areaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*             
*             Valid outputs:
*               noAreaSummary (1)
*               sendAreaSummary (2)
*
*             If it is noAreaSummary, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is sendAreaSummary, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummarySetTest ( L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Delete the area summary.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    val         variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    If it is noAreaSummary, router will neither originate nor propagate 
*           summary LSAs into stub area. It will rely on its default route.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummarySet(L7_uint32 areaId, L7_uint32 val);

/*********************************************************************
* @purpose  Test if the OSPF area entry status is settable.
*          
* @param    areaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusSetTest ( L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Delete the OSPF area entry status.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusSet(L7_uint32 areaId, L7_uint32 val);

/*********************************************************************
* @purpose  Check the External Routing Capability for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) external routing capability val}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaExternalRoutingCapabilityGet(L7_uint32 areaId, L7_uint32 *val);

/* ospfStub */
/*********************************************************************
* @purpose  Get the area entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaEntryGet(L7_uint32 AreaId, L7_int32 TOS);

/*********************************************************************
* @purpose  Get the next area entry.
*
* @param    *AreaId     next area id
* @param    *TOS        next TOS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaEntryNext(L7_uint32 *AreaId, L7_int32 *TOS);

/*********************************************************************
* @purpose  Get the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of  
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val);

/*********************************************************************
* @purpose  Test to set the the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    metric      value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of  
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricSetTest(L7_uint32 AreaId, L7_int32 TOS, L7_uint32 metric);

/*********************************************************************
* @purpose  Get the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val);

/*********************************************************************
* @purpose  Test to set the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusSetTest(L7_uint32 AreaId, L7_int32 TOS, L7_int32 val);

/*********************************************************************
* @purpose  set the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId     type
* @param    TOS         entry
* @param    val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusSet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 val);

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    *val        metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricTypeGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val);

/*********************************************************************
* @purpose  Test to Set the stub metric type.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricTypeSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType );

/*********************************************************************
* @purpose  Set the stub metric configuration for the areaID and TOS
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    metric      metric 
* @param    metricType  metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricCfgSet(L7_uint32 AreaId, L7_int32 TOS,
                                     L7_uint32 metric, L7_uint32 metricType);


/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA type for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Type}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSATypeGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Age for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Age}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAAgeGet(L7_uint32 areaId, L7_int32 Type, 
                                       L7_uint32 Lsid, L7_uint32 RouterId, 
                                       L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Checksum for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Checksum}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSACksumGet(L7_uint32 areaId, L7_int32 Type, 
                                         L7_uint32 Lsid, L7_uint32 RouterId,
                                         L7_uint32 *val);

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
L7_RC_t ospfMapExtenAbrEntryGet ( L7_uint32 destinationIp,
                                  L7_RtbEntryInfo_t *p_rtbEntry );

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
L7_RC_t ospfMapExtenAsbrEntryGet ( L7_uint32 destinationIp,
                                   L7_RtbEntryInfo_t *p_rtbEntry );

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
L7_RC_t ospfMapExtenAbrEntryNext ( L7_uint32 *destinationIp,
                                   L7_RtbEntryInfo_t *p_rtbEntry );

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
L7_RC_t ospfMapExtenAsbrEntryNext ( L7_uint32 *destinationIp,
                                    L7_RtbEntryInfo_t *p_rtbEntry );

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
L7_RC_t ospfAbrEntry_Lookup(L7_uint32 destinationIp,
                            L7_RtbEntryInfo_t *p_rtbEntry);

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
L7_RC_t ospfAsbrEntry_Lookup(L7_uint32 destinationIp,
                             L7_RtbEntryInfo_t *p_rtbEntry);

/*********************************************************************
* @purpose  Get the Next ABR Entry corresponding to destIp and
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
L7_RC_t ospfAbrEntryNext(L7_uint32 *destinationIp,
                         L7_RtbEntryInfo_t *p_rtbEntry);

/*********************************************************************
* @purpose  Get the Next ASBR Entry corresponding to destIp and
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
L7_RC_t ospfAsbrEntryNext(L7_uint32 *destinationIp,
                          L7_RtbEntryInfo_t *p_rtbEntry);

/* ospfLsdb */
/*********************************************************************
 * @purpose            Get an entry in the ospfLsdbTable.
 *
 *
 * @param LsdbAreaId   @b{(input)}  the area identifier (IP address)
 * @param LsdbType     @b{(input)}  the type of the link state advertisement
 * @param LsdbId       @b{(input)}  identifies the piece of the routing
 *                                  domain that is being described by
 *                                  the adverti
 *                                  sement (IP adrress)
 * @param LsdbRouterId  @b{(input)} originating router in the Autonomous
 *                                  System(IP address).
 * @param p_Lsa         @b{(input)} pointer to LSA entry structure
 *
 * @returns            pointer to the appropriate entry of ospfLsdbTable
 *                     on success
 * @returns            NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t ospfLsdbTable_Lookup(L7_uint32 LsdbAreaId, L7_uint32 LsdbType,
                             L7_uint32 LsdbId, L7_uint32 LsdbRouterId,
                             L7_ospfLsdbEntry_t *p_Lsa);

 /*********************************************************************
 * @purpose                  Get the next entry in the ospfLsdbTable.
 *
 *
 * @param prevLsdbAreaId     @b{(input)} the area identifier (IP address)
 * @param prevLsdbType       @b{(input)} the type of the link state
 *                                       advertisement
 * @param prevLsdbId         @b{(input)} identifies the piece of the routing
 *                                       domain that is being described by the
 *                                       advertisement (IP adrress)
 * @param prevLsdbRouterId   @b{(input)} originating router in the Autonom
 *                                       ous System(IP address)
 *
 * @param p_LsdbAreaId       @b{(output)}  the area identifier (IP address)
 * @param p_LsdbType         @b{(output)}  the type of the link state
 *                                         advertisement
 * @param p_LsdbId           @b{(output)}  ID of the routing domain piece
 *                                         that is being described by the
 *                                         advertisement.
 * @param p_LsdbRouterId     @b{(output)}  originating router ID in the
 *                                         Autonomous System.
 *
 * @returns          pointer to the next (after prevAreaId) entry in the
 *                   ospfAreaTable and the next area id on success
 * @returns          NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t ospfLsdbTable_GetNext(L7_uint32 *p_LsdbAreaId, L7_uint32 *p_LsdbType,
                              L7_uint32 *p_LsdbId, L7_uint32 *p_LsdbRouterId,
                              L7_ospfLsdbEntry_t *p_Lsa);


/*********************************************************************
* @purpose  Get the process's Link State Database entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
*                   
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbEntryGet(L7_uint32 areaId, L7_uint32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    *Type       entry
* @param    *Lsid       Link State id
* @param    *RouterId   Database entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbEntryNext(L7_uint32 *areaId, L7_uint32 *Type,
                                  L7_uint32 *Lsid, L7_uint32 *RouterId,
                                  L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    sequence number field is a signed 32-bit integer.   
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbSequenceGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_uint32 *val);

/* ospfExtLsdb */
/*********************************************************************
 * @purpose               Get an entry in the ospfExtLsdbTable.
 *
 * @param extLsdbType     @b{(input)}  the type of link state advertisement
 * @param extLsdbLsid     @b{(input)}  IP address or Router ID that specifies the
 *                                     piece of the routing domain describing in
 *                                     the advertisement
 * @param extlsdbRtrId    @b{(input)}  IP address of the originating router
 *
 * @returns     pointer to the appropriate entry of ospfExtLsdbTable on success
 * @returns     NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t ospfExtLsdbTable_Lookup(L7_uint32 extLsdbType, L7_uint32 extLsdbId,
                                L7_uint32 extLsdbRtrId,
                                L7_ospfLsdbEntry_t *p_Lsa);

 /*********************************************************************
 * @purpose                    Get the next entry in the ospfExtLsdbTable.
 *
 * @param prevExtLsdbType      @b{(input)}  the type of the external-LSA
 * @param prevExtLsdbLsid      @b{(input)}  ID of the described piece of
 *                                          routing domain
 * @param prevExtlsdbRtrId     @b{(input)}  the originating router ID
 *
 * @param prevExtLsdbType      @b{(output)}  the type of the external-LSA
 * @param prevExtLsdbLsid      @b{(output)}  ID of the described piece of
 *                                           routing domain
 * @param p_ExtlsdbRtrId       @b{(output)}  the originating router ID
 *
 * @returns     pointer to the next (after the previous) entry in the
 *              ospfExtLsdbTable on success
 * @returns     NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t ospfExtLsdbTable_GetNext(L7_uint32 *p_ExtLsdbType, L7_uint32 *p_ExtLsdbLsid, 
                                 L7_uint32 *p_ExtlsdbRtrId,
                                 L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get a single Link State entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbEntryGet(L7_uint32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId,
                                    L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the next Link State entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Type       link state type
* @param    *Lsid       link state id
* @param    *RouterId   router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbEntryNext(L7_uint32 *Type, L7_uint32 *Lsid,
                                     L7_uint32 *RouterId,
                                     L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    It is used to detect old and duplicate link state advertisements.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbSequenceGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the age of the link state advertisement in seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        age in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAgeGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val);

/*********************************************************************
* @purpose  Get the checksum of the complete contents of the advertisement, 
*           excepting the age field.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbChecksumGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val);

/*********************************************************************
* @purpose  Get the entire Link State Advertisement, including its header.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *buf        link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAdvertisementGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the length of the link state advertisement.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAdvertisementLengthGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_uint32 *val);

/* ospfAreaAggregate */
/*********************************************************************
* @purpose  Get the entry in range of IP addresses specified by an IP address/IP network mask pair.   
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEntryGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask);

/*********************************************************************
* @purpose  Get the next entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaID     area id
* @param    *Type       link state type
* @param    *Net        network
* @param    *Mask       mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEntryNext(L7_uint32 *AreaID, L7_uint32 *Type, L7_int32 *Net, L7_uint32 *Mask);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val);

/*********************************************************************
* @purpose  Get the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    *val        subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement 
*           of indicated aggregate, or result in subnet's not being 
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val);

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusSetTest(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val);

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val);

/*********************************************************************
* @purpose  Test to set the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement 
*           of indicated aggregate, or result in subnet's not being 
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectSetTest(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val);

/*********************************************************************
* @purpose  Delete the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement 
*           of indicated aggregate, or result in subnet's not being 
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val);


/*********************************************************************
* @purpose  Get the active/inactive status of the area aggregate range
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_SUCCESS  if active 
* @returns  L7_FAILURE  otherwise 
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryStatusGet ( L7_uint32 areaId,
                                              L7_uint32 lsdbType,
                                              L7_int32 ipAddr, 
                                              L7_uint32 netMask);

/*********************************************************************
* @purpose  Get the area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryGet(L7_uint32 AreaId, L7_uint32 RangeNet);

/*********************************************************************
* @purpose  Get the next area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    *RangeNet   range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryNext(L7_uint32 *AreaID, L7_uint32 *RangeNet);

/*********************************************************************
* @purpose  Get the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        IpAddress
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeMaskGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the entry status.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeStatusGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 *val);

/*********************************************************************
* @purpose  Get the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of the indicated summary, or result in the subnet's not
*           being advertised at all outside the area.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEffectGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 *val);

/*********************************************************************
* @purpose  Delete the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    val         IpAddress
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeMaskSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_uint32 val);

/*********************************************************************
* @purpose  Delete the entry status.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeStatusSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 val);

/*********************************************************************
* @purpose  Delete the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEffectSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 val);

/* ospfHost */
/*********************************************************************
* @purpose  Get the host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostEntryGet(L7_uint32 IpAddress, L7_int32 TOS );

/*********************************************************************
* @purpose  Get the next host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *IPAddress  Ip address
* @param    *TOS        host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostEntryNext(L7_uint32 *IpAddress, L7_int32 *TOS );

/*********************************************************************
* @purpose  Get the Metric to be advertised.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the Metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricSetTest( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Delete the Metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricSet( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusSetTest(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusSet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Get the Area the Host Entry is to be found within.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, the area that a subsuming OSPF interface is in, or 0.0.0.0.needs AreaId too.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostAreaIDGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

/*
**********************************************************************
*                    API FUNCTIONS  -  NSSA CONFIG  (RFC 3101)
**********************************************************************
*/

/*********************************************************************
* @purpose  Get the information associated with NSSA configuration
*
* @param    areaId       area id
* @param    *metric      pointer to stub area metric
* @param    *metricType  pointer to stub area metric type
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAEntryGet ( L7_uint32 areaId, L7_uint32 *metric, 
                                   L7_uint32 *metricType);

/*********************************************************************
* @purpose  Given a NSSA area specification, return the area ID
*           of the next NSSA area
*
* @param    areaId      area id
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS);

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    areaId  area identifier
* @param    status  used to create and delete nssa configurations
*                   (L7_OSPF_ROW_STATUS_t)    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*             This funtion is primarily used to create/delete NSSAs.
*             In order to change specific config parms individual 
*             functions have been provided.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAStatusSet (L7_uint32 areaId, L7_int32 status);

/*********************************************************************
* @purpose  Set the NSSA Translator Role of the specified NSSA
*
* @param    areaId    areaID
* @param    nssaTR   NSSA translator role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable Specifies whether or not an NSSA border router will
*        unconditionally translate Type-7 LSAs into Type-5 LSAs.  When
*        it is set to Always, an NSSA border router always translates
*        Type-7 LSAs into Type-5 LSAs regardless of the translator state
*        of other NSSA border routers.  When it is set to Candidate, an
*        NSSA border router participates in the translator election
*        process described in Section 3.1.  The default setting is
*        Candidate"
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorRoleSet (L7_uint32 areaId, L7_uint32 nssaTR);

/*********************************************************************
* @purpose  Get the NSSA Translator State of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId    areaID
* @param    *nssaTR   NSSA translator state (L7_OSPF_NSSA_TRANSLATOR_STATE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable specifies the translator state of a NSSA border 
*        router.  The translator state is determined by translator 
*        election taking into account the user configured NSSA Translator
*        Role and is a read-only parm."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorStateGet (L7_uint32 areaId, 
                                            L7_uint32 *nssaTRState);

/*********************************************************************
* @purpose  Set the Translator Stability Interval of the specified NSSA
*
* @param    areaId     areaID
* @param    *stabInt   TranslatorStabilityInterval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable defines the length of time an elected Type-7 
*        translator will continue to perform its translator duties once 
*        it has determined that its translator status has been deposed by
*        another NSSA border router translator as described in Section
*        3.1 and 3.3.  The default setting is 40 seconds."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorStabilityIntervalSet (L7_uint32 areaId, 
                                                        L7_uint32 stabInt);

/*********************************************************************
* @purpose  Set the import summary configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " When set to enabled, OSPF's summary routes are imported into
*        the NSSA as Type-3 summary-LSAs.  When set to disabled, summary
*        routes are not imported into the NSSA.  The default setting is
*        enabled."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAImportSummariesSet (L7_uint32 areaId, L7_uint32 impSum);

/*********************************************************************
* @purpose  Set the T3 default cost for the specified NSSA
*
* @param    areaId     areaID
* @param    impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultCostSet (L7_uint32 areaId, L7_uint32 defCost);

/*********************************************************************
* @purpose  Set the route redistribution configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    redist    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_ENABLE/ L7_DISABLE (Used when the router is a NSSA ABR 
*          and you want the redistribute command to import routes only 
*          into the normal areas, but not into the NSSA area)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSARedistributeSet (L7_uint32 areaId, L7_uint32 redist);

/*********************************************************************
* @purpose  Set the default information origination configuration for 
*           the specified NSSA
*
* @param    areaId      areaID
* @param    defInfoOrg L7_TRUE/ L7_FALSE
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultInfoSet(L7_uint32 areaId, L7_uint32 defInfoOrg,
                                       L7_uint32 metricType, L7_uint32 metric);

/*********************************************************************
* @purpose  Set the default information origination configuration for 
*           the specified NSSA
*
* @param    areaId        areaID
* @param    defInfoOrg  L7_TRUE/ L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into 
*          the NSSA area. This argument takes effect only on a NSSA ABR)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultInfoOriginateSet (L7_uint32 areaId, L7_uint32 defInfoOrg);

/*********************************************************************
* @purpose  Set the metric type of the default route for the NSSA.
*
* @param    areaId      areaID
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultMetricTypeSet ( L7_uint32 areaId, L7_uint32 metricType );

/*********************************************************************
* @purpose  Set the value of the default metric for the given NSSA
*
* @param    areaId      areaID
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultMetricSet ( L7_uint32 areaId, L7_uint32 metric );

/*********************************************************************
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGet(L7_uint32 *p_areaId);

/*********************************************************************
* @purpose  Get the Ospf Area Id for the next area, of the Ospf Router ,
*           after the area Id specified
*
* @param    areaId      @b{(input)} area identifier
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGetNext(L7_uint32 areaId, L7_uint32 *p_areaId);

/*********************************************************************
* @purpose  Convert Vendor trap type to Lvl7 Trap type.
*
* @param    vendTrap    vendor specific ospf trap
* @param    l7Trap      Ospf trap type.
*                       This value is specified by the enum L7_OSPF_TRAP_t
*                       defined in l3_commdefs.h as follows:
*                       
*                       typedef enum
*                       {
*                         L7_OSPF_TRAP_RESERVED = 0,
*                         L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE,
*                         L7_OSPF_TRAP_NBR_STATE_CHANGE,    
*                         L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE,
*                         L7_OSPF_TRAP_IF_CONFIG_ERROR,      
*                         L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR, 
*                         L7_OSPF_TRAP_IF_AUTH_FAILURE,      
*                         L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE, 
*                         L7_OSPF_TRAP_RX_BAD_PACKET,        
*                         L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET,
*                         L7_OSPF_TRAP_TX_RETRANSMIT,        
*                         L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT,
*                         L7_OSPF_TRAP_ORIGINATE_LSA,         
*                         L7_OSPF_TRAP_MAX_AGE_LSA,           
*                         L7_OSPF_TRAP_LS_DB_OVERFLOW,        
*                         L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW,
*                         L7_OSPF_TRAP_IF_STATE_CHANGE,           
*                         L7_OSPF_TRAP_IF_RX_PACKET,              
*                         L7_OSPF_TRAP_RTB_ENTRY_INFO,
*                         L7_OSPF_TRAP_LAST             
*
*                       }  L7_OSPF_TRAP_t;
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVendorTrapToL7Trap(L7_uint32 vendTrap, L7_uint32 *l7Trap);

/*********************************************************************
* @purpose  Get the Virtual neighbor entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEntryGet(L7_uint32 Area, L7_uint32 RtrId);

/*********************************************************************
* @purpose  Get the Virtual neighbor next entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Area       area id
* @param    *RtrId      router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEntryNext(L7_uint32 *Area, L7_uint32 *RtrId);

/*********************************************************************
* @purpose  Get the IP address this Virtual Neighbor is using.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        Ip address         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrIpAddrGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get a Bit Mask corresponding to the neighbor's options field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        bit mask         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Bit 1, indicates that system will operate on Type of Service metrics other than
*               TOS 0. If zero, neighbor will ignore all metrics except the TOS 0 metric.
*           Bit 2, indicates that system implements OSPF Multicast Routing.needs intIfNum also
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrOptionsGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Virtual Neighbor Relationship state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrStateGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of times this virtual link has changed its state,
*           or an error has occurred.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEventsGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the current length of the retransmission queue.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrLsRetransQLenGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Indicate whether Hellos are being suppressed to the neighbor.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs intIfNum also.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrHelloSuppressedGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA LS Id for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA LSId}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSALSIdGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Router Id for the specified 
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA RouterId}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSARouterIdGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Sequence for the specified 
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Sequence}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSASequenceGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Options for the specified 
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Options}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAOptionsGet(L7_uint32 areaId, L7_int32 Type, 
                                           L7_uint32 Lsid, L7_uint32 RouterId,
                                           L7_uchar8 *val);

/*********************************************************************
* @purpose  Get the flags field for a router LSA
*
* @param    areaId  @b{(input)}  area ID where router LSA is flooded
* @param    routerId @b{(input)} router ID of originating router
* @param    rtrLsaFlags  @b{(output)} flags field in router LSA
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRouterLsaFlagsGet(L7_uint32 areaId, L7_uint32 routerId, 
                                      L7_uchar8 *rtrLsaFlags);

/*********************************************************************
* @purpose  Configures if the specified Area is a stub area
*
* @param    areaID  @b{(input)}
* @param    stubArea   @b{(input)}   L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIsStubAreaSet(L7_uint32 areaID, L7_uint32 stubArea);

/*********************************************************************
* @purpose  Deletes the specified Area Range
*
* @param    areaID  @b{(input)}
* @param    Area Range Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeDelete(L7_uint32 areaId, struct L7_ospfAreaAddrRangeInfo_s *areaAddrRange);

/*********************************************************************
* @purpose  Sets the specified Stub Area
*
* @param    areaID  @b{(input)}
* @param    Stub Area Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStubAreaSet(L7_uint32 areaID, struct ospfAreaStubInfo_s *stubInfo);

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
L7_uint32 ospfMapExtenSpfStatsGet(L7_uint32 maxStats, L7_OspfSpfStats_t *spfStats);

/*********************************************************************
* @purpose  Get OSPFv2 interface statistics 
*
* @param    intIfNum  @b{(input)}    interface whose stats are to be retrieved
* @param    intfStats @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats);

/*********************************************************************
* @purpose  Tell the OSPF engine to originate a default route.
*          
* @param    none 
*
* @returns  L7_SUCCESS  if success
*
* @notes    A default route not originated if no default exists in
*           the forwarding table, unless OSPF is configured to 
*           originate unconditionally.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDefRouteOrig ();

/*********************************************************************
* @purpose  Tell the OSPF engine not to originate a default route.
*          
* @param    none 
*
* @returns  L7_SUCCESS  if successful
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDefRouteNoOrig ();

/*********************************************************************
*
* @purpose Determine whether OSPF is currently redistributing a route
*          to a given destination.
*
* @param @b{(input)} destAddr - destination IP address
* @param @b{(input)} destMask - destination network mask
*
* @returns L7_TRUE if OSPF is currently redistributing this prefix.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL ospfMapPreviouslyRedistributed(L7_uint32 destAddr, L7_uint32 destMask);

/*********************************************************************
* @purpose  Creates the OSPF socket
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRawSockInit(void);

/*********************************************************************
* @purpose  Deletes the OSPF socket
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRawSockDelete(void);

/*********************************************************************
* @purpose  Get opaque capability support.
*
* @param    opaqueEnabled   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOspfOpaqueCapabilityGet(L7_uint32 *opaqueEnabled);

/*********************************************************************
* @purpose  Set opaque capability support.
*
* @param    opaqueEnabled   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOspfOpaqueCapabilitySet(L7_uint32 opaqueEnabled);

/*********************************************************************
* @purpose  Get the process's AreaOpaque Link State Database entry.
*
* @param    AreaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaOpaqueLsdbEntryGet(L7_uint32 areaId, L7_uint32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's AreaOpaque Next Link State Database entry.
*
* @param    AreaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaOpaqueLsdbEntryNext(L7_uint32 *areaId, L7_int32 *Type,
                                 L7_uint32 *Lsid, L7_uint32 *RouterId,
                                 L7_ospfOpaqueLsdbEntry_t *p_Lsa);

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
* @notes    sequence number field is a signed 32-bit integer.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType10LsaSequenceGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Opaquee LSA Age for the specified Area
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
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType10LsaAgeGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the type-10 LSA.
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
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType10LsaChecksumGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_uint32 *val);

/*********************************************************************
* @purpose  Get the type-10 advertisement of the LSA.
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
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType10LsaAdvertisementGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the process's LinkOpaque Link State Database entry.
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLinkOpaqueLsdbEntryGet(L7_uint32 ipAddr, L7_uint32 ifIndex,
                                L7_uint32 LsdbType, L7_uint32 LsdbId,
                                L7_uint32 LsdbRouterId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's LinkOpaque Next Link State Database entry.
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLinkOpaqueLsdbEntryNext(L7_uint32 *ipAddr,
                                            L7_uint32 *ifIndex,
                                            L7_int32  *Type,
                                            L7_uint32 *Lsid,
                                            L7_uint32 *RouterId,
                                            L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    sequence number field is a signed 32-bit integer.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType9LsaSequenceGet(L7_uint32 ipAddr,
                                        L7_uint32 ifIndex,
                                        L7_int32  Type,
                                        L7_uint32 Lsid,
                                        L7_uint32 RouterId,
                                        L7_uint32 *val);
/*********************************************************************
* @purpose  Get the Ospf Link Opaque LSA Age
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType9LsaAgeGet(L7_uint32 ipAddr,
                                   L7_uint32 ifIndex,
                                   L7_int32  Type,
                                   L7_uint32 Lsid,
                                   L7_uint32 RouterId,
                                   L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the type-9 LSA.
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType9LsaChecksumGet(L7_uint32 ipAddr,
                                        L7_uint32 ifIndex,
                                        L7_int32  Type,
                                        L7_uint32 Lsid,
                                        L7_uint32 RouterId,
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Get the type-9 advertisement of the LSA.
*
* @param    ipAddr      IpAddress
* @param    ifIndex     Interface Index
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType9LsaAdvertisementGet(L7_uint32 ipAddr,
                                             L7_int32  ifIndex,
                                             L7_int32  Type,
                                             L7_uint32 Lsid,
                                             L7_uint32 RouterId,
                                             L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the process's AsOpaque Link State Database entry.
*
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsOpaqueLsdbEntryGet(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                L7_uint32 LsdbRouterId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's AsOpaque Next Link State Database entry.
*
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising Router
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsOpaqueLsdbEntryNext(L7_int32  *Type,
                                            L7_uint32 *Lsid,
                                            L7_uint32 *RouterId,
                                            L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    sequence number field is a signed 32-bit integer.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType11LsaSequenceGet(L7_int32  Type,
                                        L7_uint32 Lsid,
                                        L7_uint32 RouterId,
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf As Opaque LSA Age
*
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType11LsaAgeGet(L7_int32  Type,
                                   L7_uint32 Lsid,
                                   L7_uint32 RouterId,
                                   L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the type-11 LSA.
*
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType11LsaChecksumGet(L7_int32  Type,
                                        L7_uint32 Lsid,
                                        L7_uint32 RouterId,
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Get the type-11 advertisement of the LSA.
*
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenType11LsaAdvertisementGet(L7_int32  Type,
                                             L7_uint32 Lsid,
                                             L7_uint32 RouterId,
                                             L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the checksum Sum of the type-11 LSA.
*
* @param    *val        checksumSum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenASOpaqueLSAChecksumSumGet(L7_uint32 *val);

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
L7_RC_t ospfMapExtenIsStubRtrGet(L7_BOOL *isStubRtr);

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
L7_RC_t ospfMapExtenExtLsdbOverflowGet(L7_BOOL *extLsdbOverflow);

/*********************************************************************
* @purpose  Get global OSPFv2 status information
*
* @param    *status   @b{(output)     pointer to output location                       
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStatusGet(L7_ospfStatus_t *status);

/*********************************************************************
* @purpose  Clear OSPF neighbors.
*
* @param    intIfNum - If non-zero, restrict clear to neighbors on this interface.
* @param    routerId   If non-zero, restrict clear to neighbors with this router ID. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId);

/*********************************************************************
* @purpose  Clear OSPF statistics.
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
L7_RC_t ospfMapExtenCountersClear(void);

#endif /* _OSPF_VEND_EXTEN_H_ */

