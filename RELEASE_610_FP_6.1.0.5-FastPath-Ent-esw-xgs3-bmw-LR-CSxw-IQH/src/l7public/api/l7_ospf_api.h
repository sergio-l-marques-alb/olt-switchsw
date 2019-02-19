/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l7_ospf_api.h
*
* @purpose   Ospf General Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for general ospf
*            variables, which are global to the ospf process (rfc 1850)
*
* @create    03/12/2001
*
* @author    anayar
*
* @end
*             
**********************************************************************/

#ifndef L7_OSPF_API_H
#define L7_OSPF_API_H

#include "l3_comm_structs.h"
#include "l3_commdefs.h"

/************************************************************************
* OSPF funtion table that contains the APIs that are available for the
* external components when OSPF component exists.
************************************************************************/
typedef struct ospfFuncTable_s
{
  /* Debug functions */
  void (*ospfMapCfgDataShow)(void);
  void (*ospfMapExtenRTOShow)(void);
  void (*ospfMapExtenIFOShow)(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, 
                              L7_BOOL virtFlag);
  void (*ospfMapExtenAROShow)(L7_uint32 areaId);
  void (*ospfMapExtenNBOShow)(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);
} ospfFuncTable_t;

extern ospfFuncTable_t ospfFuncTable;

/*
***********************************************************************
*                     API FUNCTIONS  -  OSPF UTILITY FUNCTIONS
***********************************************************************
*/


/*********************************************************************
* @purpose  Set OSPF Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes    
*       
* @end
*********************************************************************/
void ospfMapTraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes    
*       
* @end
*********************************************************************/
L7_uint32 ospfMapTraceModeGet(void);

/*********************************************************************
* @purpose  Determine if the interface exists to the OSPF component 
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapOspfIntfExists(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the OSPF component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapOspfInitialized(void);

/*********************************************************************
* @purpose  Determine whether OSPF originates a default route.
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapOriginatingDefaultRoute(void);

/*********************************************************************
* @purpose  Determine if the OSPF interface has been initialized
*
* @param    void
*
* @returns  L7_TRUE: ospf initialized on the interface
* @returns  L7_FALSE: ospf is not initialized on the interface.
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapOspfIntfInitialized(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Calculates the interface metric based on the interface speed.
*
* @param    intIfNum   internal interface number
* @param    ifMetric*  storage to return the calculated metric.
*
* @returns  L7_SUCCESS
*
* @notes    This function calculates the metric based on the speed of the
*           link as received from the network interface manager.  In the
*           event that NIM returns a failure a speed of 100full is
*           assumed.
*
*           This function may be used to calculate the metric ONLY IF
*           there is no user configured value for the metric.
*           i.e pOspfMapCfgData->ckt[usp.slot][usp.port].metric == 
*                             L7_OSPF_INTF_METRIC_NOT_CONFIGURED
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricCalculate(L7_uint32 intIfNum, L7_uint32 *ifMetric);

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
L7_RC_t ospfMapStatusGet(L7_ospfStatus_t *status);

/*
***********************************************************************
*                     API FUNCTIONS  -  GLOBAL CONFIG
***********************************************************************
*/

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
L7_RC_t ospfMapRouterIDGet(L7_uint32 *routerID);

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
*           By convention, to ensure uniqueness, this should default
*           to the value of one of the router's IP interface addresses."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRouterIDSet(L7_uint32 routerID);

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
L7_RC_t ospfMapOspfAdminModeGet(L7_uint32 *mode);

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
L7_RC_t ospfMapOspfAdminModeSet(L7_uint32 mode);

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
L7_RC_t ospfMapVersionNumberGet ( L7_int32 *val );

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
L7_RC_t ospfMapAbrStatusGet(L7_uint32 *mode);

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
* @notes    With release G, user no longer configures a single ASBR
*           admin mode parameter. Instead, the user configures 
*           redistribution independently for each source.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAsbrAdminModeGet(L7_uint32 *mode);

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
L7_RC_t ospfMapOpaqueASLSACountGet(L7_uint32 *val);

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
L7_RC_t ospfMapExternalLSACountGet(L7_uint32 *val);

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
L7_RC_t ospfMapSelfOrigExternalLSACountGet(L7_uint32 *val);

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
L7_RC_t ospfMapExternalLSAChecksumGet(L7_uint32 *val);

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
L7_RC_t ospfMapNewLSAOrigGet(L7_uint32 *val);

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
L7_RC_t ospfMapNumLSAReceivedGet(L7_uint32 *val);

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
L7_RC_t ospfMapExternalLSALimitGet(L7_int32 *val);

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
L7_RC_t ospfMapExtLsdbLimitSet ( L7_int32 val );

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
L7_RC_t ospfMapExitOverflowIntervalGet(L7_uint32 *val);

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
L7_RC_t ospfMapExitOverflowIntervalSet ( L7_int32 val );

/*********************************************************************
* @purpose  Set the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   @b{(input)} maxPaths - the maximum number of paths allowed
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if maxPaths is out of range
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapMaxPathsSet(L7_uint32 maxPaths);

/*********************************************************************
* @purpose  Get the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   @b{(output)} maxPaths - the maximum number of paths allowed
* 
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if maxPaths is NULL
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapMaxPathsGet(L7_uint32 *maxPaths);

/*********************************************************************
* @purpose  Set the auto cost reference bandwidth to OSPF links.
*
* @param   @b{(input)} autoCostRefBw - the allowed reference bandwidth.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if autoCostRefBw is out of range
*
* @notes    autoCostRefBw may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW , inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapAutoCostRefBwSet(L7_uint32 autoCostRefBw);

/*********************************************************************
* @purpose  Get the autocost reference bandwidth for OSPF links.
*
* @param    autoCostRefBw @b{(output)} the reference bandwidth  allowed
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if autoCostRefBw is out of range
*
* @notes    autoCostRefBw  may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapAutoCostRefBwGet(L7_uint32 *autoCostRefBw);

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
L7_RC_t ospfMapDemandExtensionsGet ( L7_int32 *val );

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
L7_RC_t ospfMapDemandExtensionsSet ( L7_int32 val );

/*********************************************************************
* @purpose  Get a Bit Mask indicating whether the router is
*           forwarding IP multicast (Class D) datagrams.
*          
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This is based on algorithms defined in Multicast 
*             Extensions to OSPF rfc1850.  Bit 0, indicates that 
*             router can forward IP multicast datagrams in the router's
*             directly attached areas (called intra-area multicast 
*             routing).
*
*             Bit 1, indicates that router can forward IP multicast
*                    datagrams between OSPF areas (called inter-area
*                    multicast routing).
*
*             Bit 2, indicates that router can forward IP multicast
*                     datagrams between Autonomous Systems
*                     (called inter-AS multicast routing).
*
*             Only certain combinations of bit settings are allowed:
*             0 (the default) - multicast forwarding is not enabled.
*             1 - intra-area multicasting only.
*             3 - intra-area and inter-area multicasting.
*             5 - intra-area and inter-AS multicasting.
*             7 - multicasting to all areas. "
*
* @end
*********************************************************************/
L7_RC_t ospfMapMulticastExtensionsGet ( L7_int32 *val );

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
* @notes    " This is based on algorithms defined in Multicast 
*             Extensions to OSPF rfc1850.  Bit 0, indicates that 
*             router can forward IP multicast datagrams in the router's
*             directly attached areas (called intra-area multicast 
*             routing).
*
*             Bit 1, indicates that router can forward IP multicast
*                    datagrams between OSPF areas (called inter-area
*                    multicast routing).
*
*             Bit 2, indicates that router can forward IP multicast
*                     datagrams between Autonomous Systems
*                     (called inter-AS multicast routing).
*
*             Only certain combinations of bit settings are allowed:
*             0 (the default) - multicast forwarding is not enabled.
*             1 - intra-area multicasting only.
*             3 - intra-area and inter-area multicasting.
*             5 - intra-area and inter-AS multicasting.
*             7 - multicasting to all areas. "
*
* @end
*
* @end
*********************************************************************/
L7_RC_t ospfMapMulticastExtensionsSet ( L7_int32 val );

/*********************************************************************
* @purpose  Gets the Opaque LSAs Storing Capability
*
* @param    *mode  @b{(output)}     L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOpaqueLSAModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the Opaque LSAs Storing Capability
*
* @param    mode  @b{(output)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOpaqueLSAModeSet(L7_uint32 mode);

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
L7_RC_t ospfMapRfc1583CompatibilityGet(L7_uint32 *mode);

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
L7_RC_t ospfMapRfc1583CompatibilitySet(L7_uint32 mode);

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
L7_RC_t ospfMapTrapModeSet(L7_uint32 trapType);


/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* The OSPF Area Data Structure contains information regarding the various  */
/* areas. The interfaces and virtual links are configured as part of these  */
/* areas. Area 0.0.0.0, by definition, is the backbone Area.                */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Creates an ospf area
*
* @param    areaID  @b{(input)}  areaid for the area to be created
* @param    index   @b{(output)} index value in the area array
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    This function is called when an area is automatically 
*           created via an 'Apply Function'. This generally occurs
*           when a user tries to configure a non-existant area.
*           
*           The area is created with default values. 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCreate(L7_uint32 areaId, L7_uint32 *index);

/*********************************************************************
* @purpose  Deletes an ospf area
*
* @param    areaID  @b{(input)} AreaId of the area to be deleted
*
* @returns  L7_SUCCESS  Area was successfully deleted
* @returns  L7_ERROR    Specified area has not been configured
* @returns  L7_FAILURE  Area contains active interface(s) (these
*                       need to be deleted before removing the area)
*
* @notes    This function is called when a 'no area' command is
*           issued via the user interface.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaDelete(L7_uint32 areaId);

/*********************************************************************
* @purpose  Frees an ospf area
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaFree(L7_uint32 areaId);

/*********************************************************************
* @purpose  Determine if OSPF area exists
*          
* @param    areaId      area ID         
*                                                            
* @returns  L7_SUCCESS, if area exists
* @returns  L7_FAILURE  if area does not exist      
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaEntryExists ( L7_uint32 AreaId );

/*********************************************************************
* @purpose  Determines if the specified area entry exists in the ospf
*           mib tables
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS  if the area exists
* @returns  L7_FAILURE, if the area does not exist
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaGet(L7_uint32 areaId);

/*********************************************************************
* @purpose  Gets the area id of the subsequent area given a
*           starting point area id.
*
* @param    areaID        @b{(input)}  current area
* @param    *nextAreaId   @b{(output)} next area entry
*                                                    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId);

/*********************************************************************
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*          
* @param    *p_areaId      L7_uint32 area Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine returns the first entry in an ordered list.
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaIdFirst(L7_uint32 *p_areaId);

/*********************************************************************
* @purpose  Gets the first configured OSPF area ID.
*
* @param    areaID  @b{(output)}
*
* @returns  L7_SUCCESS if an area is configured
* @returns  L7_FAILURE, otherwise
*
* @comments This function doesn't depend on operational state.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCfgGetFirst(L7_uint32 *areaId);

/*********************************************************************
* @purpose  Given an OSPF area ID, gets the next configured OSPF area ID.
*
* @param    areaId  @b{(input)}
* @param    nextAreaId  @b{(output)}
*
* @returns  L7_SUCCESS if a next area ID is returned
* @returns  L7_FAILURE if there is not another area or if the input 
*                      area is not found.
*
* @comments This function doesn't depend on operational state.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaCfgGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId);

/*********************************************************************
* @purpose  Gets the array index associated with the area
*
* @param    areaID  @b{(input)}
* @param    *index   @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIndexGet(L7_uint32 areaId, L7_uint32 *index);

/*********************************************************************
* @purpose  Get the area's support for importing AS external 
*           link-state advertisements.
*          
* @param    AreaId      type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area's support for importing AS external link-state
*             advertisements."
*
* @end
*********************************************************************/
L7_RC_t ospfMapImportAsExternGet ( L7_uint32 AreaId, L7_int32 *val );

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
L7_RC_t ospfMapImportAsExternSetTest ( L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Set the area's support for importing AS external 
*           link-state advertisements.
*          
* @param    AreaId      type
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area's support for importing AS external link-state
*             advertisements."
*
* @end
*********************************************************************/
L7_RC_t ospfMapImportAsExternSet ( L7_uint32 AreaId, L7_int32 val );

/*********************************************************************
* @purpose  Retrieve the number of SPF Runs for the specified AreaID
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 number of SPF runs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times that the intra-area route table
*             has been calculated using this area's link-state
*             database. This is typically done using Djikstra's
*             algorithm."
*        
* @end
*********************************************************************/
L7_RC_t ospfMapNumSPFRunsGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of Autonomous System border routers 
*           reachable  within  this area.  
*          
* @param    AreaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of Autonomous System border routers
*             reachable within this area.  This is initially zero
*             and is calculated in each SPF Pass."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAsBdrRtrCountGet ( L7_uint32 AreaId, L7_uint32 *val );

/*********************************************************************
* @purpose  Retrieve the Area Border Count for the specified AreaID
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area border count 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of area border routers reachable within
*             this area. This is initially zero, and is calculated
*             in each SPF Pass."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaBorderCountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Retrieve the Area LSA Count for the specified AreaID
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area LSA count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The total number of link-state advertisements in this
*             area's link-state database, excluding AS External LSAs"
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSACountGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Retrieve the Area LSA Statistics
*
* @param    areaId    L7_uint32 area ID
* @param    *stats    area LSA stats
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSAStatGet(L7_uint32 areaId, L7_OspfAreaDbStats_t *stats);

/*********************************************************************
* 
* @purpose  Retrieve the Area LSA Checksum for the specified AreaID
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 area LSA checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The 32-bit unsigned sum of the link-state advertisements'
*             LS checksums contained in this area's link-state database.
*             This sum excludes external (LS type 5) link-state
*             advertisements. The sum can be used to determine if there
*             has been a change in a router's link state database, and
*             to compare the link-state database of two routers."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSAChecksumGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the area summary.
*          
* @param    AreaId      type
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
L7_RC_t ospfMapAreaSummaryGet ( L7_uint32 AreaId, L7_int32 *val );

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
L7_RC_t ospfMapAreaSummarySetTest ( L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Delete the area summary.
*          
* @param    AreaId      type
* @param    val         variable
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
L7_RC_t ospfMapAreaSummarySet ( L7_uint32 AreaId, L7_int32 val );

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
L7_RC_t ospfMapAreaStatusGet ( L7_uint32 areaId, L7_uint32 *val );

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
L7_RC_t ospfMapAreaStatusSetTest ( L7_uint32 AreaId, L7_int32 val );

/*********************************************************************
* @purpose  Set the OSPF area entry status.
*          
* @param    AreaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative.
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaStatusSet ( L7_uint32 AreaId, L7_int32 val );

/*********************************************************************
* 
* @purpose  Check the External Routing Capability for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 external routing capability
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaExternalRoutingCapabilityGet(L7_uint32 areaId, L7_uint32 *val);
/*
**********************************************************************
*                    API FUNCTIONS  -  STUB AREA CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* OSPF Area Default Metric Table:                                          */
/* The OSPF Area Default Metric Table describes the metrics that a default  */
/* Area Border Router will advertise into a Stub Area.                      */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Check if TOS Type is supported 
*
* @param    TOS         TOS value 
*
*
* @returns  L7_TRUE    if TOS value is supported 
* @returns  L7_FALSE     otherwise
*
*
* @end
*********************************************************************/
L7_BOOL ospfMapTosIsValid (  L7_uint32 TOS);



/*********************************************************************
* @purpose  Get the information associated with stub area configuration
*
* @param    AreaId      area id 
* @param    TOS         TOS  (L7_TOSTYPES)
* @param    *metric     pointer to stub area metric
* @param    *metric     pointer to stub area metric type
*
* @returns  L7_SUCCESS  if stub area configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    " The metric for a given Type of Service that will be
*             advertised by a default Area Border Router into a stub
*             area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubAreaEntryGet ( L7_uint32 AreaId, L7_uint32 TOS, 
                                  L7_uint32 *metric, L7_uint32 *metricType);

/*********************************************************************
* @purpose  Given a stub area specification, return the area ID and TOS
*           of the next stub area
*
* @param    areaId      area id 
* @param    TOS         TOS  (L7_TOSTYPES)
*
* @returns  L7_SUCCESS  if stub area configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    " The metric for a given Type of Service that will be
*             advertised by a default Area Border Router into a stub
*             area."
*
*           This routine returns the configured values for the stub area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubAreaEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS);

/*********************************************************************
* @purpose  Gets the Default Metric for the specified Area 
*
* @param    areaID  @b{(input)}
* @param    *metric  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    " The metric value applied at the indicated type of
*             service.  By default, this equals the least metric
*             at the type of service among the interfaces of 
*             other areas."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaDefaultMetricGet(L7_uint32 areaId, L7_uint32 *metric);

/*********************************************************************
* @purpose  Sets the Default Metric for the specified Area
*
* @param    areaID  @b{(input)}
* @param    metric   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    " The metric value applied at the indicated type of
*             service.  By default, this equals the least metric
*             at the type of service among the interfaces of 
*             other areas."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAreaDefaultMetricSet(L7_uint32 areaId, L7_uint32 metric);



/*********************************************************************
* @purpose  Check if the stub entry is active.
*
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        L7_TRUE, if entry is active, L7_FALSE, otherwise
*
*
* @returns  L7_SUCCESS    if entry could be found
* @returns  L7_FAILUE     otherwise
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubStatusGet ( L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    AreaId      type
* @param    TOS         entry
* @param    *val        metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeGet ( L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Test to Set the stub metric type.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType );

/*********************************************************************
* @purpose  Delete the default route metric type.
*
* @param    AreaId      type
* @param    TOS         entry
* @param    val         metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricTypeSet ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType );


/*********************************************************************
* @purpose  Get the stub metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      area ID 
* @param    TOS         TOS type (L7_TOSTYPES)
* @param    *metric     stubMetric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of  
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricGet ( L7_uint32 AreaId, L7_uint32 TOS, L7_uint32 *metric );

/*********************************************************************
* @purpose  Test to Set the stub metric for the given area and TOS
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metric     metric 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @notes     If metric is set to 0, metric is calculated as a factor   
*            of ifSpeed.           
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metric );

/*********************************************************************
* @purpose  Set the stub metric for the given area and TOS
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    *metric     metric 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @notes     If metric is set to 0, metric is calculated as a factor   
*            of ifSpeed.           
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricSet ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metric );

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA RANGE CONFIG
**********************************************************************
*/

/*--------------------------------------------------------------------*/
/* The Address Range Table acts as an adjunct to the Area Table; It   */
/* describes those Address Range Summaries that are configured to be  */
/* propagated from an Area to reduce the amount of information about  */
/* it which is known beyond its borders.                              */
/* Note: This table was obsoleted by rfc 1850                         */
/*--------------------------------------------------------------------*/

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA AGGREGATE CONFIG
**********************************************************************
*/

/*-------------------------------------------------------------------------*/
/* The OSPF Area Aggregate Table:                                          */
/* This table replaces the OSPF Area Summary Table, being and extension    */
/* of that for CIDR routers. An Area Aggregate entry is a range of IP      */
/* addresses specified by an IP address/IP network mask pair. For example  */
/* class B address range of X.X.X.X with a network mask of 255.255.0.0     */
/* includes all IP addresses from X.X.0.0 to X.X.255.255. Note that        */
/* if ranges are configured such that one range subsumes another range     */
/* (e.g., 10.0.0.0 mask 255.0.0.0 and 10.1.0.0 mask 255.255.0.0 the        */
/* most specific match is the preferred one.                               */
/*-------------------------------------------------------------------------*/


/*********************************************************************
* @purpose  Create an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
* @param    advertise   advertise aggregate area (L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeCreate ( L7_uint32 areaId, 
                                          L7_uint32 lsdbType, 
                                          L7_uint32 ipAddr,
                                          L7_uint32 netMask, 
                                          L7_uint32 advertise );

/*********************************************************************
* @purpose  Delete an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*             obsolete
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeDelete ( L7_uint32 areaId, 
                                 L7_uint32 lsdbType, 
                                 L7_uint32 ipAddr, 
                                 L7_uint32 netMask );

/*********************************************************************
* @purpose  Get the first area range for a given OSPF area. 
*
* @param  areaId @b{(input)} Area ID of the area the range applies to  
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when 
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if there is not another range
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaRangeFirst(L7_uint32 *areaId, 
                              L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t *rangeType, 
                              L7_int32 *prefix, L7_uint32 *mask);

/*********************************************************************
* @purpose  Get next area range for a given OSPF area. 
*
* @param  areaId @b{(input)} Area ID of the area the range applies to  
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when 
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
*                      0 to get the first range of either type.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if no next range
* @returns  L7_ERROR if input parameters are invalid or previous range not found
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
*           If rangeType is 0 on input, function assumes caller is looking 
*           for the first range entry. If rangeType is non-zero, function
*           assumes input parameters identify a range entry and gets the
*           next entry. Note that a range can have a prefix and mask of 0.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEntryNext(L7_uint32 *areaId, 
                                 L7_uint32 *rangeType, 
                                 L7_int32 *prefix, L7_uint32 *mask);

/*********************************************************************
* @purpose  Get next area range for any OSPF area. 
*
* @param  areaId @b{(input/output)} Area ID of the area the range applies to  
* @param  rangeType  @b{(input/output)} Whether the range is used when
*                      generating type 3 summary LSAs or used when 
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
*                      0 to get the first range of either type.
* @param  prefix  @b{(input/output)}  Network prefix
* @param  mask    @b{(input/output)}  Network mask
*
* @returns  L7_SUCCESS  if a next range is returned
* @returns  L7_FAILURE  if no next range
* @returns  L7_ERROR if input parameters are invalid or previous range not found
*
* @notes    Function returns either type of area range (summary or
*           NSSA), regardless of the input rangeType.
*
*           This function is provided to enable SNMP to walk all area range
*           entries for all areas for the ospfAreaAggregateTable in RFC 1850.
*           SNMP treats the combination of the 4 arguments as the key. After
*           retrieving the first area range, SNMP will call this function
*           again with the "key" incremented, thinking that might be the 
*           next possible entry. Thus, the input parameters are not exactly
*           those of an existing area range. We have to find the area range
*           with the next larger "key." We get this behavior from the OSPF
*           MIB code, but note that OSPF MIB data is only available is OSPF
*           has already been initialized. Since SNMP does not work off the 
*           same data as CLI and Web, the results may differ, esp when OSPF
*           has not been initialized yet.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAnyAreaAggrEntryNext(L7_uint32 *areaId, 
                                    L7_uint32 *rangeType, 
                                    L7_int32 *prefix, L7_uint32 *mask);

/*********************************************************************
* @purpose  Find a specific area range.
*
* @param  areaId @b{(input)} Area ID of the area the range applies to  
* @param  rangeType  @b{(input)} Whether the range is used when
*                      generating type 3 summary LSAs or used when 
*                      translating T7 LSAs to T5 LSAs at the border of
*                      an NSSA. One of L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t.
* @param  prefix  @b{(input)}  Network prefix
* @param  mask    @b{(input)}  Network mask
*
* @returns  L7_SUCCESS  if an area range with these parameters is configured
* @returns  L7_FAILURE  otherwise
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEntryGet (L7_uint32 areaId, L7_uint32 rangeType,
                                 L7_int32 prefix, L7_uint32 mask);


/*********************************************************************
* @purpose  Get the entry in range of IP addresses specified by an 
*           IP address/IP network mask pair.   
*
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area the Address Aggregate is to be found within."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateEntryGet ( L7_uint32 AreaID, L7_uint32 Type,
                                       L7_int32 Net, L7_uint32 Mask );

/*********************************************************************
* @purpose  Get the next entry.
*
* @param    *AreaID     area id
* @param    *Type       link state type
* @param    *Net        network
* @param    *Mask       mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area the Address Aggregate is to be found within."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateEntryNext ( L7_uint32 *AreaID, L7_uint32 *Type,
                                        L7_int32 *Net, L7_uint32 *Mask );

/*********************************************************************
* @purpose  Get the SNMP row status of an area range.
*
* @param    areaId    @b{(input)}  area id
* @param    lsdbType  @b{(input)}  link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr    @b{(input)}  network
* @param    netMask   @b{(input)}  mask
* @param    *val      @b{(output)} entry status  (e_RowStatus)
*
* @returns  L7_SUCCESS, if entry found
* @returns  L7_FAILURE  otherwise
*
* @notes    This function is only used by SNMP. Because we are interested
*           in the row status, goes to the protocol SNMP tables, rather 
*           than using the configuration data. This function only succeeds
*           if OSPF is initialized.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusGet ( L7_uint32 areaId,
                                   L7_uint32 lsdbType,
                                   L7_uint32 ipAddr,
                                   L7_uint32 netMask,
                                   L7_uint32 *val );

/*********************************************************************
* @purpose  Test to set the status of area aggregate entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status  (L7_TRUE, if active, 
*                                      L7_FALSE, if inactive or nonexistent)
*
* @returns  L7_SUCCESS, if entry could be found
* @returns  L7_FAILURE  otherwise
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusSetTest ( L7_uint32 areaId,  
                                        L7_uint32 lsdbType,
                                        L7_uint32 ipAddr,  
                                        L7_uint32 netMask,
                                        L7_uint32 val );


/*********************************************************************
* @purpose  Set the status of area aggregate entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status  (L7_TRUE, if active, 
*                                      L7_FALSE, if inactive or nonexistent)
*
* @returns  L7_SUCCESS, if entry could be found
* @returns  L7_FAILURE  otherwise
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrStatusSet ( L7_uint32 areaId,  
                                        L7_uint32 lsdbType,
                                        L7_uint32 ipAddr,  
                                        L7_uint32 netMask,
                                        L7_uint32 val );

/*********************************************************************
* @purpose  Get area range advertisement configuration
*
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        subnet effect  (L7_TRUE, if advertise, L7_FALSE, otherwise)
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
L7_RC_t ospfMapAreaAggrEffectGet ( L7_uint32 AreaID, L7_uint32 Type,
                                        L7_int32 Net, L7_uint32 Mask, L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the subnet effect.
*
* @param    area      area id
* @param    Type        link state type
* @param    Net         network                       
* @param    Mask        mask
* @param    advertise   advertise range (L7_BOOL)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure, if aggregate range is not configured
*
* @notes    " Subnets subsumed by ranges either trigger the advertisement 
*             of indicated aggregate, or result in subnet's not being 
*             advertised at all outside the area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEffectSetTest ( L7_uint32 areaId, L7_uint32 lsdbType,
                                        L7_int32 ipAddr, L7_uint32 netMask, L7_BOOL advertise );

/*********************************************************************
* @purpose  Set the subnet effect.
*
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    advertise   advertise range (L7_BOOL)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Subnets subsumed by ranges either trigger the advertisement 
*             of indicated aggregate, or result in subnet's not being 
*             advertised at all outside the area."
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggrEffectSet ( L7_uint32 areaId, L7_uint32 lsdbType,
                                        L7_int32 ipAddr, L7_uint32 netMask, 
                                        L7_BOOL advertise );

/*
***********************************************************************
*                     API FUNCTIONS  -  NETWORK AREA
***********************************************************************
*/
/*********************************************************************
* @purpose  Find a specific network area object.
*
* @param  ipAddr        @b{(input)} network configured for ospf
* @param  wildcardMask  @b{(input)} wildcard mask
* @param  areaId        @b{(input)} areaid for this network
*
* @returns  L7_SUCCESS  if the specified network area object is found
* @returns  L7_FAILURE  otherwise
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryGet (L7_uint32 ipAddr, L7_uint32 wildcardMask,
                                    L7_int32 areaId);

/***************************************************************************
* @purpose  Get next network area configured for OSPF.
*
* @param  ipAddr     @b{(input/output)} network of the current object
* @param  wildcardMask @b{(input/output)} wildcardMask of the current object
* @param  areaId     @b{(input/output)}  areaId of the current object
*
* @returns  L7_SUCCESS  if a next network area object exists
* @returns  L7_FAILURE  if no next object exists
* @returns  L7_ERROR if input parameters are invalid or previous network
*                    area object is not found
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryNext(L7_uint32 *ipAddr, L7_uint32 *wildcardMask,
                                    L7_int32 *areaId);

/*
***********************************************************************
*                     API FUNCTIONS  -  HOST CONFIG
***********************************************************************
*/

/*--------------------------------------------------------------------------*/
/* The Host/Metric Table indicates what hosts are directly attached to the  */
/* Router, and what metrics and types of service should be advertised for   */
/* them.                                                                    */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get the host entry.
*
* @param    IPAddress   Ip address
* @param    TOS         host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A metric to be advertised, for a given type of service,
*             when a given host is reachable."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostEntryGet ( L7_uint32 IpAddress, L7_int32 TOS );

/*********************************************************************
* @purpose  Get the next host entry.
*
* @param    *IPAddress  Ip address
* @param    *TOS        host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A metric to be advertised, for a given type of service,
*             when a given host is reachable."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostEntryNext ( L7_uint32 *IpAddress, L7_int32 *TOS );

/*********************************************************************
* @purpose  Get the Metric to be advertised.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the Metric.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricSetTest ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Set the Metric.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Metric to be Advertised."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostMetricSet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
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
L7_RC_t ospfMapHostStatusGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
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
L7_RC_t ospfMapHostStatusSetTest ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
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
L7_RC_t ospfMapHostStatusSet ( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Get the Area the Host Entry is to be found within.
*
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Area the Host Entry is to be found within.  By
*             default, the area that a subsuming OSPF interface is
*             in, or 0.0.0.0."
*
* @end
*********************************************************************/
L7_RC_t ospfMapHostAreaIDGet ( L7_uint32 IpAddress, L7_int32 TOS, L7_uint32 *val );

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
L7_RC_t ospfMapIntfEntryGet ( L7_uint32 ipAddr, L7_int32 intIfNum );

/*********************************************************************
* @purpose  Get the next Interface Entry.
*
* @param    *IpAddr     Ip address
* @param    *intIfNum     interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The OSPF Interface Entry describes one interface from
*             the viewpoint of OSPF."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEntryNext ( L7_uint32 *IpAddr, L7_int32 *intIfNum );

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
L7_RC_t ospfMapIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Area Id for the specified interface 
*
* @param    intIfNum    internal interface number  
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
L7_RC_t ospfMapIntfAreaIdGet(L7_uint32 intIfNum, L7_uint32 *area);

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
                                   L7_uint32 val );

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
L7_RC_t ospfMapIntfAreaIdSet(L7_uint32 intIfNum, L7_uint32 area);

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
* @notes    " The OSPF interface type.  By way of default, this field
*             may be intuited from the corresponding value of ifType
*             Broadcast LANs, such as Ethernet and IEEE 802.5, take
*             the value 'broadcast', X.25 and similar technologies
*             take the value 'nbma', and links that are definitively
*             point to point take the value 'pointToPoint'."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *type);

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
L7_RC_t ospfMapIntfTypeSetTest ( L7_int32 intIfNum, L7_int32 val );

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
L7_RC_t ospfMapIntfTypeSet ( L7_int32 intIfNum, L7_int32 val );

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
L7_RC_t ospfMapDefaultIfType(L7_uint32 intIfNum, L7_uint32 *ospfNetType);

/*********************************************************************
* @purpose  Gets the Ospf Admin mode for the specified interface
*
* @param    ipAddr      Ip address
* @param    intIfNum    internal interface number  
* @param    *val        status
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
L7_RC_t ospfMapIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

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
                                     L7_uint32 mode);

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
L7_RC_t ospfMapIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode);

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
L7_RC_t ospfMapIntfSecondariesFlagGet(L7_uint32 intIfNum, L7_uint32 *secondaries);

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
L7_RC_t ospfMapIntfSecondariesFlagSet(L7_uint32 intIfNum, L7_uint32 secondaries);

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
L7_RC_t ospfMapIntfPriorityGet(L7_uint32 intIfNum, L7_uint32 *priority);

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
                                     L7_int32 val );

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
L7_RC_t ospfMapIntfPrioritySet(L7_uint32 intIfNum, L7_uint32 priority);

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
L7_RC_t ospfMapIntfTransitDelayGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
                                       L7_uint32 seconds);

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
L7_RC_t ospfMapIntfTransitDelaySet(L7_uint32 intIfNum, L7_uint32 seconds);


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
L7_RC_t ospfMapIntfLsaAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);


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
L7_RC_t ospfMapIntfLsaAckIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapIntfRxmtIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
                                       L7_uint32 seconds);

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
L7_RC_t ospfMapIntfRxmtIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapIntfHelloIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
                                         L7_uint32 seconds);

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
L7_RC_t ospfMapIntfHelloIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapSpfDelayGet(L7_uint32 *spfDelay);

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
L7_RC_t ospfMapSpfDelaySet(L7_uint32 spfDelay);

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
L7_RC_t ospfMapSpfHoldtimeGet(L7_uint32 *spfHoldtime);

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
L7_RC_t ospfMapSpfHoldtimeSet(L7_uint32 spfHoldtime);

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
L7_RC_t ospfMapPassiveModeGet(L7_BOOL *passiveMode);

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
L7_RC_t ospfMapPassiveModeSet(L7_BOOL passiveMode);

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
L7_RC_t ospfMapIntfDeadIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
                                       L7_uint32 seconds);

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
L7_RC_t ospfMapIntfDeadIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapIntfNbmaPollIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
                                           L7_uint32 seconds);

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
L7_RC_t ospfMapIntfNbmaPollIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds);

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
L7_RC_t ospfMapIntfEffectivePassiveModeGet(L7_uint32 intIfNum, L7_BOOL *passiveMode);

/*********************************************************************
* @purpose  Gets the passive mode of the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *passiveMode  @b{(output)} Passive Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPassiveModeGet(L7_uint32 intIfNum, L7_BOOL *passiveMode);

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
L7_RC_t ospfMapIntfPassiveModeSet(L7_uint32 intIfNum, L7_BOOL passiveMode);

/*********************************************************************
* @purpose  Get the effective admin mode on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    adminMode  @b{(output)} effective admin mode
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAdminModeGet(L7_uint32 intIfNum,
                                         L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Set the effective admin mode on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    adminMode  @b{(input)} effective admin mode
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAdminModeSet(L7_uint32 intIfNum,
                                         L7_uint32 adminMode);

/*********************************************************************
* @purpose  Get the effective areaId on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    areaId     @b{(output)} effective areaId
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAreaIdGet(L7_uint32 intIfNum,
                                      L7_uint32 *areaId);

/*********************************************************************
* @purpose  Set the effective areaId on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    areaId     @b{(input)} effective areaId
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAreaIdSet(L7_uint32 intIfNum,
                                      L7_uint32 areaId);

/*********************************************************************
* @purpose  Get the effective secondaries flag on the interface
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    secondariesFlag @b{(output)} effective secondaries flag
*                                        that indicates what all secondary
*                                        addresses are advertised
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveSecondariesFlagGet(L7_uint32 intIfNum,
                                               L7_uint32 *secondariesFlag);

/*********************************************************************
* @purpose  Set the effective secondaries flag on the interface
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    secondariesFlag @b{(input)} effective secondaries flag
*                                       that indicates what all secondary
*                                       addresses are advertised
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveSecondariesFlagSet(L7_uint32 intIfNum,
                                               L7_uint32 secondariesFlag);

/*********************************************************************
* @purpose  Create/Update a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_SUCCESS  if entry is successfully added to the list
* @returns  L7_FAILURE  if entry can't be added to the list
*
* @notes    This command defines the interfaces covered by the network
*           ipAddr on which OSPF runs and also defines the area ID
*           for those interfaces.
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryCreate (L7_uint32 ipAddr,
                                       L7_uint32 wildcardMask,
                                       L7_uint32 areaId);

/*********************************************************************
* @purpose  Delete a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_SUCCESS  if the network command is successfully deleted
* @returns  L7_FAILURE  if the network command has not existed
*
* @notes    This command defines the interfaces covered by the network
*           ipAddr on which OSPF runs and also defines the area ID
*           for those interfaces.
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaEntryDelete (L7_uint32 ipAddr,
                                       L7_uint32 wildcardMask,
                                       L7_uint32 areaId);

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
L7_RC_t ospfMapIntfStateGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats);

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
L7_RC_t ospfMapIntfLSAAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds);

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
L7_RC_t ospfMapIntfLocalLinkOpaqueLSAsGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfLocalLinkOpaqueLSACksumGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfDrIdGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfBackupDrIdGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfLinkEventsCounterGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t ospfMapIntfStatusGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 *val );

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
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 *val );

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
* @notes    This function is not supported.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandSetTest( L7_uint32 ipAddr, L7_int32 intIfNum,
                                  L7_int32 val );

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
* @notes    This function is not supported.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandSet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 val );

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
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthTypeGet(L7_uint32 intIfNum, L7_uint32 *authType);

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
                                    L7_uint32 authType);

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
L7_RC_t ospfMapIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType);

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
L7_RC_t ospfMapIntfMetricGet(L7_uint32 intIfNum, L7_uint32 *metric);

/*********************************************************************
* @purpose  Gets the Ospf Authentication Key for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *key  @b{(output)}
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
L7_RC_t ospfMapIntfAuthKeyGet(L7_uint32 intIfNum, L7_uchar8 *key);

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
                                   L7_uchar8 *key);

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
L7_RC_t ospfMapIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key);

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
L7_RC_t ospfMapIntfAuthKeyIdGet(L7_uint32 intIfNum, L7_uint32 *keyId);

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
                                     L7_uint32 keyId);

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
L7_RC_t ospfMapIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId);

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
L7_RC_t ospfMapIntfVirtTransitAreaIdGet(L7_uint32 intIfNum, L7_uint32 *area);

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
L7_RC_t ospfMapIntfVirtTransitAreaIdSet(L7_uint32 intIfNum, L7_uint32 area);

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
L7_RC_t ospfMapIntfVirtIntfNeighborGet(L7_uint32 intIfNum, L7_uint32 *neighbor);

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
                                       L7_uint32 neighbor);

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
L7_RC_t ospfMapIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu);

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
L7_RC_t ospfMapIntfIpMtuGet( L7_uint32 intIfNum, L7_uint32 *maxIpMtu );


/*********************************************************************
* @purpose  Allows a user to get the value of MtuIgnore Flag.
*
* @param    intfNum
* @param    *val      L7_TRUE / L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    L7_TRUE - Router ignores MTU value while forming adjacencies
*           L7_FALSE- Router forms adjacency taking into account the MTU Value
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMtuIgnoreGet(L7_uint32 intfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Allows a user to set the MtuIgnore Flag.
*
* @param    intfNum
* @param    val      L7_TRUE / L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    L7_TRUE - Router ignores MTU value while forming adjacencies
*           L7_FALSE- Router forms adjacency taking into account the MTU Value
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMtuIgnoreSet(L7_uint32 intfNum, L7_BOOL val);


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
L7_RC_t ospfMapIntfMetricEntryGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 TOS );

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
L7_RC_t ospfMapIntfMetricEntryNext ( L7_uint32 *ipAddr, L7_int32 *intIfNum, L7_int32 *TOS );

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
L7_RC_t ospfMapIntfMetricValueGet ( L7_int32 intIfNum, L7_int32 TOS, L7_int32 *val );

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
L7_BOOL ospfMapIsIntfMetricConfigured (L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS);

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
                                  L7_int32 TOS, L7_int32 val );

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
                                  L7_int32 TOS, L7_int32 val );

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
                                   L7_int32 TOS, L7_int32 *val );

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
                                   L7_int32 TOS, L7_int32 val );

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
                                   L7_int32 TOS, L7_int32 val );

/*
***********************************************************************
*                     API FUNCTIONS  -  RTB Entries
***********************************************************************
*/

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
L7_RC_t ospfMapAsbrEntryGet ( L7_uint32 destinationIp,
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
L7_RC_t ospfMapAbrEntryNext ( L7_uint32 *destinationIp,
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
L7_RC_t ospfMapAsbrEntryNext ( L7_uint32 *destinationIp,
                               L7_RtbEntryInfo_t *p_rtbEntry );

/*
***********************************************************************
*                     API FUNCTIONS  -  LSDB CONFIG
***********************************************************************
*/

/*----------------------------------------------------------------------*/
/* OSPF Link State Database                                             */
/* The Link State Database contains the Link State Advertisements from  */
/* throughout the areas that the device is attached to.                 */
/*----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get the process's Link State Database entry.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
*                   
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single link-state Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsdbEntryGet ( L7_uint32 AreaId, L7_uint32 Type,
                              L7_uint32 Lsid, L7_uint32 RouterId,
                              L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    *AreaId     type
* @param    *Type       entry
* @param    *Lsid       Link State id
* @param    *RouterId   Database entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single link-state Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsdbEntryNext ( L7_uint32 *AreaId, L7_uint32 *Type,
                               L7_uint32 *Lsid, L7_uint32 *RouterId,
                               L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA type for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The type of link state advertisment.  Each link state
*             type has a separate advertisement format.
*
*             Link State Advertisement Types:
*               routerLink  (1)
*               networkLink (2)
*               summaryLink (3)
*               asSummaryLink (4)
*               asExternalLink (5)
*               multicastLink (6)
*               nssaExternalLink (7) "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSATypeGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA LS Id for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA LS Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Link State ID is an LS Type Specific field
*             containing either a Router ID or an IP Address; it
*             identifies the piece of the routing domain that is
*             being described by the advertisement."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSALSIdGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Router Id for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The 32 bit number that uniquely identifies the 
*             originating router in the Autonomous System."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSARouterIdGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    AreaId      type
* @param    Type        entry
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
L7_RC_t ospfMapLsdbSequenceGet ( L7_uint32 AreaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_uint32 *val );

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Sequence for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Sequence
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
L7_uint32 ospfMapAreaLSADbLSASequenceGet(L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Age for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
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
L7_uint32 ospfMapAreaLSADbLSAAgeGet(L7_uint32 areaId, L7_int32 Type, 
                                    L7_uint32 Lsid, L7_uint32 RouterId, 
                                    L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Options for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Options
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSAOptionsGet(L7_uint32 areaId, L7_int32 Type, 
                                        L7_uint32 Lsid, L7_uint32 RouterId,
                                        L7_uchar8 *val);

/*********************************************************************
* 
* @purpose  Get the flags field of an OSPF router LSA
*          
* @param    areaId    area ID where router LSA is flooded
* @param    routerId  router ID of originating router
* @param    rtrLsaFlags   flags field in router LSA
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t ospfMapRouterLsaFlagsGet(L7_uint32 areaId, L7_uint32 routerId, 
                                 L7_uchar8 *rtrLsaFlags);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Checksum for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the checksum of the complete content of
*             the advertisement, excepting the age field.  The age
*             field is excepted so that an advertisement's age can
*             be incremented without updating the checksum.  The
*             checksum used is the same that is used for ISO
*             connectionless datagrams; it is commonly referred to
*             as the Fletcher checksum."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSACksumGet(L7_uint32 areaId, L7_int32 Type, 
                                      L7_uint32 Lsid, L7_uint32 RouterId,
                                      L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Advertisement packet for the
*           specified area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uchar8 LSA Advertise packet
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSAAdvertiseGet(L7_uint32 areaId, L7_int32 Type, 
                                          L7_uint32 Lsid, L7_uint32 RouterId,
                                          L7_char8 **buf, L7_uint32 *buf_len);

/*
***********************************************************************
*                     API FUNCTIONS  -  EXT LSDB CONFIG
***********************************************************************
*/

/*----------------------------------------------------------------------*/
/* OSPF Link State Database, External                                   */
/* The Link State Database contains the Link State Advertisements from  */
/* throughout the areas that the device is attached to.                 */
/* This table is identical to the OSPF LSDB Table in format, but        */
/* contains only External Link State Advertisements. The purpose is     */
/* to allow external LSAs to be displayed once for the router rather    */
/* than once in each non-stub area.                                     */
/*----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get a single Link State entry.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single Link State Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbEntryGet ( L7_uint32 Type, L7_uint32 Lsid,
                                 L7_uint32 RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the next Link State entry.
*
* @param    *Type       link state type
* @param    *Lsid       link state id
* @param    *RouterId   router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single Link State Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbEntryNext ( L7_uint32 *Type, L7_uint32 *Lsid,
                                  L7_uint32 *RouterId,
                                  L7_ospfLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number field.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32 bit integer.
*             It is used to detect old and duplicate link state
*             advertisements.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number the
*             more recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbSequenceGet ( L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the age of the link state advertisement in seconds.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        age in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the age of the link state advertisement
*             in seconds."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAgeGet ( L7_int32 Type, L7_uint32 Lsid,
                               L7_uint32 RouterId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the checksum of the complete contents of the advertisement, 
*           excepting the age field.  
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the checksum of the complete content of
*             the advertisement, excepting the age field.  The age
*             field is excepted so that an advertisement's age can
*             be incremented without updating the checksum.  The
*             checksum used is the same that is used for ISO
*             connectionless datagrams; it is commonly referred to
*             as the Fletcher checksum."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbChecksumGet ( L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entire Link State Advertisement, including its header.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *buf        link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAdvertisementGet ( L7_int32 Type, L7_uint32 Lsid,
                                         L7_uint32 RouterId, L7_char8 **buf,
										 L7_uint32 *len);

/*********************************************************************
* @purpose  Get the length of the Link State Advertisement
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        length of link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAdvertisementLengthGet ( L7_int32 Type, L7_uint32 Lsid,
                                         L7_uint32 RouterId, L7_uint32 *val );
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
L7_RC_t ospfMapNbrEntryGet ( L7_uint32 IpAddr, L7_uint32 intIfNum );

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
L7_RC_t ospfMapNbrEntryNext ( L7_uint32 *IpAddr, L7_uint32 *intIfNum );

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
L7_uint32 ospfMapNeighborIPAddrGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
L7_uint32 ospfMapNeighborIfIndexGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Router Id for the first Neighbor of the 
*           specified interface
* 
* @param    intIfNum         L7_uint32 interface number
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
L7_uint32 ospfMapNbrRouterIdGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId);

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
*
* @notes    " A 32-bit integer (represented as a type IpAddress)
*             uniquely identifying the neighboring router in the
*             Autonomous System.
*             Default Value: 0.0.0.0 "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNeighborRouterIdGetNext(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId);

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
L7_uint32 ospfMapNbrOptionsGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_uint32 ospfMapNbrAreaGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_uint32 ospfMapNbrDeadTimerRemainingGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of seconds that OSPF has had a Full adjacency
*           with a given neighbor.
* 
* @param    intIfNum   interface number
* @param    routerId   the specified router Id of the Neighbor
* @param    *val       time in secs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrUptimeGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
L7_uint32 ospfMapNbrPriorityGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
                                L7_uint32 intIfNum, L7_int32 val );

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
                                L7_uint32 intIfNum, L7_int32 val );

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
L7_uint32 ospfMapNbrStateGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_uint32 ospfMapNbrIntfStateGet(L7_uint32 intIfNum, L7_uint32 routerId,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Ospf Events counter of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    ipADdress		ip address of the neighbor
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
L7_uint32 ospfMapNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_uint32 ospfMapRtrIntfAllNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_uint32 ospfMapNbrLSRetransQLenGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
                                  L7_uint32 intIfNum, L7_int32 *val );

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
								  L7_int32 val );

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
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
L7_RC_t ospfMapNbmaNbrStatusSet ( L7_uint32 IpAddr,
                                  L7_uint32 intIfNum, L7_int32 val );

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
L7_uint32 ospfMapNbmaNbrPermanenceGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_uint32 ospfMapNbrHellosSuppressedGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
* @purpose  Clear OSPF neighbors.
*
* @param    intIfNum - If non-zero, restrict clear to neighbors on this interface.
* @param    routerId   If non-zero, restrict clear to neighbors with this router ID. 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId);

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
L7_RC_t ospfMapVirtNbrEntryGet ( L7_uint32 Area, L7_uint32 RtrId );

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
L7_RC_t ospfMapVirtNbrEntryNext ( L7_uint32 *Area, L7_uint32 *RtrId );

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
                                  L7_uint32 RtrId, L7_uint32 *val );

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
                                   L7_uint32 RtrId, L7_int32 *val );

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
                                 L7_uint32 RtrId, L7_int32 *val );

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
                                  L7_uint32 RtrId, L7_uint32 *val );

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
                                         L7_uint32 RtrId, L7_uint32 *val );

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
                                           L7_uint32 RtrId, L7_int32 *val );

/*
***********************************************************************
*                     API FUNCTIONS  -  VIRT IF CONFIG
***********************************************************************
*/

/*
***********************************************************************
*                     API FUNCTIONS  -  VIRT IF CONFIG
***********************************************************************
*/

/*********************************************************************
* @purpose  Get a single Virtual Interface entry information.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Information about a single Virtual interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEntryGet ( L7_uint32 AreaId, L7_uint32 Neighbor );

/*********************************************************************
* @purpose  Get the next Virtual Interface entry information.
*
* @param    intIfNum    internal interface number
* @param    *AreaId     Id
* @param    *Neighbor   entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Information about a single Virtual interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEntryNext ( L7_uint32 *AreaId, L7_uint32 *Neighbor );


/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The estimated number of seconds it takes to transmit 
*             a link-state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfTransitDelayGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 *val );

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
L7_RC_t ospfMapVirtIfTransitDelaySetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                           L7_int32 val );

/*********************************************************************
* @purpose  Sets the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
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
L7_RC_t ospfMapVirtIfTransitDelaySet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 val );

/*********************************************************************
* @purpose  Get the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
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
L7_RC_t ospfMapVirtIfRetransIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 *val );

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
L7_RC_t ospfMapVirtIfRetransIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                              L7_int32 val );

/*********************************************************************
* @purpose  Sets the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
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
L7_RC_t ospfMapVirtIfRetransIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val );

/*********************************************************************
* @purpose  Get the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
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
L7_RC_t ospfMapVirtIfHelloIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 *val );

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
L7_RC_t ospfMapVirtIfHelloIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                            L7_int32 val );

/*********************************************************************
* @purpose  Set the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
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
L7_RC_t ospfMapVirtIfHelloIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 val );

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
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
L7_RC_t ospfMapVirtIfRtrDeadIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 *val );

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
L7_RC_t ospfMapVirtIfRtrDeadIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                              L7_int32 val );

/*********************************************************************
* @purpose  Set the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    intIfNum    internal interface number
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
L7_RC_t ospfMapVirtIfRtrDeadIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val );

/*********************************************************************
* @purpose  Get the OSPF virtual interface state.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        interface state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    "OSPF Virtual interface states:
*               down (1)          These use the same coding
*               pointToPoint (4)  as the ospfIfTable "
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfStateGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF virtual interface metric. This is the sum of
*           the OSPF metrics of the links in the path to the virtual 
*           neighbor.
*
* @param    intIfNum    internal interface number
* @param    areaId      The OSPF area the virtual link traverses
* @param    neighbor    The router ID of the virtual neighbor
* @param    *metric     virtual link metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfMetricGet ( L7_uint32 areaId, L7_uint32 neighbor,
                                L7_uint32 *metric );

/*********************************************************************
* @purpose  Get the number of state changes or error events on this Virtual Link.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of state changes or error events on this
*             virtual link."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEventsGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of state changes or error events on this
* @purpose  interface for all virtual links
*
* @param    intIfNum	The internal interface number
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of state changes or error events on this
*             virtual link."
*
* @end
*********************************************************************/
L7_RC_t ospfMapRtrIntfAllVirtIfEventsGet ( L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        row status
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
L7_RC_t ospfMapVirtIfStatusGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 *val );

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
L7_RC_t ospfMapVirtIfStatusSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                     L7_int32 val );

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    intIfNum    internal interface number
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
L7_RC_t ospfMapVirtIfStatusSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 val );

/*********************************************************************
* @purpose  Get the authentication type specified for a virtual interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        authentication type
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
L7_RC_t ospfMapVirtIfAuthTypeGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                   L7_int32 *val );

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
* @param    intIfNum    internal interface number
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
L7_RC_t ospfMapVirtIfAuthTypeSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 val );

/*********************************************************************
* @purpose  Delete the authentication type.
*
* @param    intIfNum    internal interface number
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
* @notes    For security reasons, the authentication key is always
*           cleared whenever the auth type is changed.  If setting a 
*           new key value, be sure to do it AFTER setting the auth type. 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthTypeSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                   L7_int32 val );

/*********************************************************************
* @purpose  Get the Authentication Key.
*
* @param    intIfNum    internal interface number
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
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                  L7_uchar8 *buf );

/*********************************************************************
* @purpose  Tests if the Authentication Key is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
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
L7_RC_t ospfMapVirtIfAuthKeySetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                      L7_uchar8 *buf );

/*********************************************************************
* @purpose  Set the Authentication Key.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
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
L7_RC_t ospfMapVirtIfAuthKeySet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                  L7_uchar8 *buf );

/*********************************************************************
* @purpose  Get the authentication key id specified for a virtual interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                    L7_uint32 *val );

/*********************************************************************
* @purpose  Test if the authentication key id is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_uint32 val );

/*********************************************************************
* @purpose  Set the authentication key id.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                    L7_uint32 val );

/*********************************************************************
* @purpose  Create a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    areaId      transit area
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfCreate (L7_uint32 areaId, L7_uint32 neighbor);

/*********************************************************************
* @purpose  Delete a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    areaId      transit area
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfDelete (L7_uint32 areaId, L7_uint32 neighbor);

/*********************************************************************
* @purpose  Check if a virtual link has been configured across the
*           specified area.
*
* @param    areaId      transit area
*
* @returns  L7_TRUE   if a virtual link has been configured across the
*                     specified area
* @returns  L7_FALSE  if a virtual link has not been configured across
*                     the specified area
*
* @notes    none.
*
* @end
*********************************************************************/
L7_BOOL ospfMapVirtIfCheck (L7_uint32 areaId);

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
L7_RC_t ospfMapDefaultMetricSet(L7_uint32 defMetric);

/*********************************************************************
* @purpose  Clear the default metric for OSPF, leaving no default metric
*           configured.
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultMetricClear();

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
L7_RC_t ospfMapDefaultMetricGet(L7_uint32 *defMetric);

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
                               L7_OSPF_EXT_METRIC_TYPES_t defRouteMetType);

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
L7_RC_t ospfMapDefaultRouteOrigSet(L7_BOOL originateDefault);

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
L7_RC_t ospfMapDefaultRouteOrigRevert();

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
L7_RC_t ospfMapDefaultRouteOrigGet(L7_BOOL *originateDefault);

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
L7_RC_t ospfMapDefaultRouteAlwaysSet(L7_BOOL always);

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
L7_RC_t ospfMapDefaultRouteAlwaysGet(L7_BOOL *always);

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
L7_RC_t ospfMapDefaultRouteMetricSet(L7_int32 defRouteMetric);

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
L7_RC_t ospfMapDefaultRouteMetricClear();

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
L7_RC_t ospfMapDefaultRouteMetricGet(L7_int32 *defRouteMetric);

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
                                         defRouteMetType);

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
L7_RC_t ospfMapDefaultRouteMetricTypeRevert();

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
                                         *defRouteMetType);

/*********************************************************************
* @purpose  Set configuration parameters for OSPF's redistribution 
*           of routes from other sources. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - Whether OSPF redistributes from sourceProto
* @param    distList - Number of access list used to filter routes from
*                      sourceProto.
* @param    redistMetric - Metric OSPF advertises for routes from
*                          sourceProto
* @param    metType - Metric type OSPF advertises for routes from 
*                     sourceProto
* @param    tag - OSPF tag advertised with routes from sourceProto
* @param    subnets - OSPF only redistributes subnetted routes if this
*                     option is set to true.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if a) sourceProto does not identify a protocol from
*                      which OSPF may redistribute, or b) redistMetric 
*                      is out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributionSet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL redistribute,
                                 L7_uint32 distList,
                                 L7_int32 redistMetric,
                                 L7_OSPF_EXT_METRIC_TYPES_t metType,
                                 L7_uint32 tag,
                                 L7_BOOL subnets);

/*********************************************************************
* @purpose  Specify whether OSPF redistributes from a specific
*           source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - Whether OSPF redistributes from sourceProto
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeSet(L7_REDIST_RT_INDICES_t sourceProto,
                               L7_BOOL redistribute);

/*********************************************************************
* @purpose  Get whether OSPF redistributes from a specific
*           source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeGet(L7_REDIST_RT_INDICES_t sourceProto,
                               L7_BOOL *redistribute);

/*********************************************************************
* @purpose  For a given source of redistributed routes, revert the
*           redistribution configuration to the defaults.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    Defaults as follows:
*              redistribute: FALSE
*              distribute list: 0
*              metric: not configured
*              metric type: external type 2
*              tag: 0
*              subnets: FALSE   
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeRevert(L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Specify an access list that OSPF uses to filter routes
*           from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    distList - Number of access list used to filter routes from
*                      sourceProto.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListSet(L7_REDIST_RT_INDICES_t sourceProto,
                             L7_uint32 distList);

/*********************************************************************
* @purpose  Remove a distribute list from serving as a route filter
*           for routes from a given source protocol. Call this function 
*           when the user removes the distribute list option. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListClear(L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the number of the access list that OSPF uses to filter routes
*           from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    distList - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @returns  L7_NOT_EXIST if no distribute list is configured.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListGet(L7_REDIST_RT_INDICES_t sourceProto,
                             L7_uint32 *distList);

/*********************************************************************
* @purpose  Set the metric that OSPF advetises for routes learned from
*           another protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistMetric - Metric OSPF advertises for routes from
*                          sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if a) sourceProto does not identify a protocol from
*                      which OSPF may redistribute, or b) redistMetric is
*                      out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricSet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_int32 redistMetric);

/*********************************************************************
* @purpose  Clear the redistribution metric for a given source protocol.
*           Call this function when the redistribute metric option is
*           removed from the configuration. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricClear(L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the metric that OSPF advetises for routes learned from
*           another protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistMetric - return value.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @returns  L7_NOT_EXIST if no redistribution metric is configured
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricGet(L7_REDIST_RT_INDICES_t sourceProto,
                               L7_int32 *redistMetric);

/*********************************************************************
* @purpose  Set the metric that OSPF uses when it redistributes routes 
*           from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    metType - Metric type OSPF advertises for routes from 
*                     sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeSet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_OSPF_EXT_METRIC_TYPES_t metType);

/*********************************************************************
* @purpose  Revert to the default the metric that OSPF uses when it  
*           redistributes routes from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeRevert(L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the metric that OSPF uses when it redistributes routes 
*           from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    metType - return value.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeGet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_OSPF_EXT_METRIC_TYPES_t *metType);

/*********************************************************************
* @purpose  Specify the tag that OSPF uses when it advertises routes
*           learned from a given protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    tag - OSPF tag advertised with routes from sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTagSet(L7_REDIST_RT_INDICES_t sourceProto,
                        L7_uint32 tag);

/*********************************************************************
* @purpose  Get the tag that OSPF uses when it advertises routes
*           learned from a given protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    tag - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTagGet(L7_REDIST_RT_INDICES_t sourceProto,
                        L7_uint32 *tag);

/*********************************************************************
* @purpose  Specify whether OSPF is allowed to redistribute subnetted 
*           routes from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    subnets - OSPF only redistributes subnetted routes if this
*                     option is set to true.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistSubnetsSet(L7_REDIST_RT_INDICES_t sourceProto,
                                  L7_BOOL subnets);

/*********************************************************************
* @purpose  Get whether OSPF is allowed to redistribute subnetted 
*           routes from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    subnets - return value.
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistSubnetsGet(L7_REDIST_RT_INDICES_t sourceProto,
                                  L7_BOOL *subnets);

/*********************************************************************
* @purpose  Indicates whether a sourceIndex is a valid
*           source from which OSPF may redistribute.
*
* @param    sourceIndex - Corresponds to a source for redistributed routes.
*
* @returns  L7_SUCCESS if sourceIndex is a valid
*                      source of routes for OSPF to redistribute.
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapRouteRedistributeGet(L7_REDIST_RT_INDICES_t sourceIndex);

/*********************************************************************
* @purpose  Given a route redistribution source, set nextSourceIndex
*           to the next valid source of redistributed routes.
*
* @param    sourceIndex - A source for redistributed routes.
* @param    nextSourceIndex - The source index following sourceIndex. 
*
* @returns  L7_SUCCESS if there is a valid nextSourceIndex
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapRouteRedistributeGetNext(L7_REDIST_RT_INDICES_t sourceIndex,
                                        L7_REDIST_RT_INDICES_t *nextSourceIndex);

/*********************************************************************
* @purpose  Flush and reoriginate all self-originated external LSAs.
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
L7_RC_t ospfMapSelfOrigExtLsaFlush(void);

/*
***********************************************************************
*                     API FUNCTIONS  -  OSPF TRAPS
***********************************************************************
*/

/*---------------------------------------------------------------------------*/
/* OSPF is an event driven routing protocol, where an event can be a change  */
/* in an OSPF interface's link-level status, the expiration of an OSPF       */
/* timer or the reception of an OSPF protocol packet. Many of the actions    */
/* that OSPF takes as a result of these events will result in a change of    */
/* the routing topology. As routing topologies become large and complex      */
/* it is often difficult to locate the source of a topology change or        */
/* unpredicted routing path by polling a large number of routers. Another    */
/* approach is to notify a network manager of potentially critical OSPF      */
/* events with SNMP traps. These apis issue ospf traps by invoking the       */
/* LVL7 Trap manager which in turn calls SNMP to send the traps.             */
/*---------------------------------------------------------------------------*/

/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  Routerid        The ip address of the router originating the trap.
*         VirtIfAreaId    The area ID of the Virtual Interface to which
*                         this RTO is attached.
*         VirtIfNeighbor  The Neighbor Router ID to which this interface
*                         is connected.
*         VirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or 
*         progresses to a terminal state. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtIfStateChange(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                     L7_uint32 virtIfNeighbor, L7_uint32 virtIfState);

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    routerId    The ip address of the router originating the trap.
*           nbrIpAddr   The IP address this neighbor is using as this 
*                       IP Source Address.
*           intIfNum    On an interface having and IP Address, "0".  
*                                On an interface without an address, the 
*                                corresponding value of ifIndex for this
*                                interface in the Internet Standard MIB 
*                                ifTable.  This index is usually used in 
*                                an unnumbered interface as a reference to
*                                the local router's control application 
*                                interface table.
*           nbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                        router in the AS (Autonomous System).
*           nbrState     The new NBR state.  NBR states are defined in the 
*                        OSPF MIB.(pg. 126)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the neighbor state regresses or 
*         progresses to a terminal state.  When a neighbor transitions 
*         from or to Full on non-broadcast multi-access and broadcast 
*         networks, the trap should be generated by the designated router.
*         A designated router transitioning to Down will be noted by 
*         ospfIfStateChange. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapNbrStateChange(L7_uint32 routerId, L7_uint32 nbrIpAddr,
                                  L7_uint32 intIfNum,
                                  L7_uint32 nbrRtrId, L7_uint32 nbrState);

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a 
*           virtual OSPF neighbor.
*
* @param    routerId      The originator of the trap
*           virtNbrArea
*           virtNbrRtrId
*           virtNbrState  The new state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This  trap  should  be generated  when  the  neighbor
*          state regresses (e.g., goes from Attempt or Full 
*          to  1-Way  or Down)  or progresses to a terminal 
*          state (e.g., 2-Way or Full).  When an  neighbor 
*          transitions from  or  to Full on non-broadcast
*          multi-access and broadcast networks, the trap should
*          be generated  by the designated router.  A designated      
*          router transitioning to Down will be  noted  by      
*          ospfIfStateChange." 
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtNbrStateChange(L7_uint32 routerId, L7_uint32 virtNbrArea,
                                      L7_uint32 virtNbrRtrId, L7_uint32 virtNbrState);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId          The originator of the trap.
*           ifIpAddress
*           intIfNum          internal interface number
*           packetSrc         The source IP Address
*           configErrorType   Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapIfConfigError(L7_uint32 routerId, L7_uint32 ifIpAddress,
                                 L7_uint32 intIfNum, L7_uint32 packetSrc,
                                 L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId        The originator of the Trap
*           virtIfAreaId
*           virtIfNeighbor
*           configErrorType Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtIfConfigError(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                     L7_uint32 virtIfNeighbor,
                                     L7_uint32 configErrorType,
                                     L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          ifIpAddress
*          intIfNum         internal interface number
*          packetSrc        The source IP Address
*          configErrorType  authType mismatch or auth failure
*          packetType 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapIfAuthFailure(L7_uint32 routerId, L7_uint32 ifIpAddress,
                                 L7_uint32 intIfNum, L7_uint32 packetSrc,
                                 L7_uint32 configErrorType, L7_uint32 packetType);

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          virtIfAreaId
*          virtIfNeighbor
*          configErrorType  Auth type mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtIfAuthFailure(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                     L7_uint32 virtIfNeighbor,
                                     L7_uint32 configErrorType,
                                     L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           non-virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the Trap
*           ifIpAddress
*           intIfNum        internal interface number
*           packetSrc       The source ip address
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapIfRxBadPacket(L7_uint32 routerId, L7_uint32 ifIpAddress,
                                 L7_uint32 intIfNum, L7_uint32 packetSrc,
                                 L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtIfRxBadPacket(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                     L7_uint32 virtIfNeighbor, L7_uint32 packetType);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           non-virtual interface.
*
* @param    routerId        The originator of the trap
*           ifIpAddress
*           intIfNum        internal interface number
*           nbrRtrId        Destination
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapTxRetransmit(L7_uint32 routerId, L7_uint32 ifIpAddress,
                                L7_uint32 intIfNum, L7_uint32 nbrRtrId,
                                L7_uint32 packetType, L7_uint32 lsdbType,
                                L7_uint32 lsdbLsid, L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           virtual interface.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapVirtTxRetransmit(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                    L7_uint32 virtIfNeighbor, L7_uint32 packetType,
                                    L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                    L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    routerId        The originator of the trap.
*           lsdbAreaId      0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This trap should not be invoked for simple refreshes of  
*           LSAs  (which happesn every 30 minutes), but       
*           instead will only be invoked  when  an  LSA  is       
*           (re)originated due to a topology change.  Addi-       
*           tionally, this trap does not include LSAs  that       
*           are  being  flushed  because  they have reached       
*           MaxAge.
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapOriginateLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                                L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state 
*           database has aged to MaxAge.
*
* @param    routerId         The originator of the trap
*           lsdbAreaId       0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapMaxAgeLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                             L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                             L7_uint32 lsdbRouterId);

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state 
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapLsdbOverflow(L7_uint32 routerId, L7_uint32 extLsdbLimit);

/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of      
*           ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapLsdbApproachingOverflow(L7_uint32 routerId,
                                       L7_uint32 extLsdbLimit);

/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a 
*           non-virtual OSPF interface.
*
* @param    routerId      The originator of the trap
*           ifIpAddress
*           intIfNum      internal interface number
*           ifState       New state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This trap should  be  generated when  the interface state 
*          regresses (e.g., goes from Dr to Down) or progresses  to  a  terminal      
*          state  (i.e.,  Point-to-Point, DR Other, Dr, or Backup).
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapIfStateChange(L7_uint32 routerId, L7_uint32 ifIpAddress,
                                 L7_uint32 intIfNum, L7_uint32 ifState);

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet was received on a non-virtual interface. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapIfRxPacket();

/*********************************************************************
*
* @purpose  Signifies that an entry has been made in the OSPF routing table. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ATIC's proprietary Routing Table Entry Information trap.
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapRtbEntryInfo();

/*********************************************************************
* @purpose  Sends a message to the OSPF thread to restore OSPF user 
*           config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This routine is for LVL7 internal use only. 
*       
* @end
*********************************************************************/
L7_RC_t ospfRestore(void);

/*********************************************************************
* @purpose  Restores OSPF user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This routine is for LVL7 internal use only. 
*       
* @end
*********************************************************************/
L7_RC_t ospfRestoreProcess(void);

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
L7_RC_t ospfMapNSSAEntryGet ( L7_uint32 areaId, L7_uint32 *metric, 
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
L7_RC_t ospfMapNSSAEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS);

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
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSAStatusSet (L7_uint32 areaId, L7_int32 status);

/*********************************************************************
* @purpose  Get the NSSA Translator Role of the specified NSSA
*
* @param    areaId    areaID
* @param    *nssaTR   NSSA translator role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t)
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
L7_RC_t ospfMapNSSATranslatorRoleGet (L7_uint32 areaId, L7_uint32 *nssaTR);

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
L7_RC_t ospfMapNSSATranslatorRoleSet (L7_uint32 areaId, L7_uint32 nssaTR);

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
L7_RC_t ospfMapNSSATranslatorStateGet (L7_uint32 areaId, L7_uint32 *nssaTRState);

/*********************************************************************
* @purpose  Get the Translator Stability Interval of the specified NSSA
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
L7_RC_t ospfMapNSSATranslatorStabilityIntervalGet (L7_uint32 areaId, 
                                                   L7_uint32 *stabInt);

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
L7_RC_t ospfMapNSSATranslatorStabilityIntervalSet (L7_uint32 areaId, 
                                                   L7_uint32 stabInt);

/*********************************************************************
* @purpose  Get the import summary configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    *impSum    L7_ENABLE/L7_DISABLE
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
L7_RC_t ospfMapNSSAImportSummariesGet (L7_uint32 areaId, L7_uint32 *impSum);

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
L7_RC_t ospfMapNSSAImportSummariesSet (L7_uint32 areaId, L7_uint32 impSum);

/*********************************************************************
* @purpose  Get the route redistribution configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    *redist    L7_ENABLE/L7_DISABLE
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
L7_RC_t ospfMapNSSARedistributeGet (L7_uint32 areaId, L7_uint32 *redist);

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
L7_RC_t ospfMapNSSARedistributeSet (L7_uint32 areaId, L7_uint32 redist);

/*********************************************************************
* @purpose  Get the default information origination configuration for 
*           the specified NSSA, includes metric & metric type
*
* @param    areaId      areaID
* @param    *defInfoOrg L7_TRUE/ L7_FALSE
* @param    *metricType metric type 
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoGet (L7_uint32 areaId, L7_uint32 *defInfoOrg, 
                                   L7_uint32 *metric,
                                   L7_OSPF_STUB_METRIC_TYPE_t *metricType);

/*********************************************************************
* @purpose  Set the default information origination configuration for 
*           the specified NSSA
*
* @param    areaId      areaID
* @param    defInfoOrg L7_TRUE/ L7_FALSE
* @param    metricType metric type 
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoSet(L7_uint32 areaId, L7_uint32 defInfoOrg,
                                  L7_OSPF_STUB_METRIC_TYPE_t metricType, 
                                  L7_uint32 metric);

/*********************************************************************
* @purpose  Get the default information origination configuration for 
*           the specified NSSA
*
* @param    areaId        areaID
* @param    *defInfoOrg  L7_TRUE/ L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into 
*          the NSSA area. This argument takes effect only on a NSSA ABR)."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultInfoOriginateGet (L7_uint32 areaId, L7_uint32 *defInfoOrg);

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
L7_RC_t ospfMapNSSADefaultInfoOriginateSet (L7_uint32 areaId, L7_uint32 defInfoOrg);

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    areaId      areaID
* @param    *metricType metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricTypeGet ( L7_uint32 areaId, 
                                          L7_OSPF_STUB_METRIC_TYPE_t *metricType );

/*********************************************************************
* @purpose  Set the metric type of the default route for the NSSA.
*
* @param    areaId      areaID
* @param    metricType metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricTypeSet ( L7_uint32 areaId, 
                                          L7_OSPF_STUB_METRIC_TYPE_t metricType );

/*********************************************************************
* @purpose  Get the NSSA metric value.
*
* @param    areaId      area ID
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapNSSADefaultMetricGet ( L7_uint32 areaId, L7_uint32 *metric );

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
L7_RC_t ospfMapNSSADefaultMetricSet ( L7_uint32 areaId, L7_uint32 metric );

/* End Function Prototypes */

/*********************************************************************
* @purpose  Determine if the interface type is valid in ospf
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in ospf
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL ospfMapIsValidIntf(L7_uint32 intIfNum);

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
L7_uint32 ospfMapSpfStatsGet(L7_uint32 maxStats, L7_OspfSpfStats_t *spfStats);

/*********************************************************************
* @purpose  Get if the OpaqueCapability is enabled.
*
* @param    opaqueEnabled - pointer to value for tracing OSPF.
*
* @returns  L7_SUCCESS if support is present.
* @returns  L7_FAILURE if support is not present.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfOpaqueLsaSupportGet(L7_uint32 *opaqueEnabled);

/*********************************************************************
* @purpose  Set if the OpaqueCapability is enabled.
*
* @param    opaqueEnabled - value for opaqueCapability.
*
* @returns  L7_SUCCESS if support is present.
* @returns  L7_FAILURE if support is not present.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfOpaqueLsaSupportSet(L7_uint32 opaqueEnabled);

/*********************************************************************
* @purpose  Get the process's Area Opaque Link State Database entry.
*
* @param    AreaId      area Id
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertizing RouterId
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaOpaqueLsdbEntryGet( L7_uint32 AreaId, L7_uint32 Type,
                              L7_uint32 Lsid, L7_uint32 RouterId,
                              L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's Area Opaque Link State Database next entry.
*
* @param    AreaId      area Id
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertizing RouterId
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaOpaqueLsdbEntryNext( L7_uint32 *AreaId, L7_int32 *Type,
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
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapType10LsaSequenceGet ( L7_uint32 AreaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_uint32 *val );

/*********************************************************************
* @purpose  Get the Ospf Area Opaque LSA Age for the specified Area
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaAgeGet(L7_uint32 areaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the Area Opaque LSA.
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaChecksumGet(L7_uint32 areaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the Area Opaque LSA.
*
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaAdvertisementGet(L7_uint32 areaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_char8 **buf, L7_uint32 *len);

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
                              L7_uint32 rtrId, L7_ospfOpaqueLsdbEntry_t *p_Lsa);

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
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

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
                                   L7_uint32 rtrId, L7_uint32 *val );

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
                              L7_uint32 rtrId, L7_uint32 *val );

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
                              L7_uint32 rtrId, L7_uint32 *val);

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
                                L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len);

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
                              L7_uint32 rtrId, L7_ospfOpaqueLsdbEntry_t *p_Lsa);

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
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

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
                                   L7_uint32 rtrId, L7_uint32 *val );

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
                              L7_uint32 rtrId, L7_uint32 *val );

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
                              L7_uint32 rtrId, L7_uint32 *val);

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
                                L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len);

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
L7_RC_t ospfMapOspfASOpaqueLSAChecksumSumGet(L7_uint32 *val);

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
L7_RC_t ospfMapIsStubRtrGet(L7_BOOL *isStubRtr);

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
L7_RC_t ospfMapExtLsdbOverflowGet(L7_BOOL *extLsdbOverflow);

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
L7_RC_t ospfMapLsaDecode(L7_char8 *lsaHdr, outputFuncPtr output,
    void * context);

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
L7_RC_t ospfMapCountersClear(void);

#endif /* L7_OSPF_API_H */
