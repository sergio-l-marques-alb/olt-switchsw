/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_mib_ospf_api.h
*
* @purpose usmdb support for LVL7 extensions to rfc1850 OSPF support
*                  
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_MIB_OSPF_API_H
#define USMDB_MIB_OSPF_API_H

#include <l3_comm_structs.h>
#include "usmdb_mib_ospfv3_api.h"

/* ospfGeneral */
/*********************************************************************
* @purpose  Get the router id.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        returned Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRouterIdGet ( L7_uint32 UnitIndex, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the administrative status of OSPF in the router.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        status value is either enable or disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 'enabled' denotes that the OSPF Process is active on at least one interface;  
*           'disabled' disables it on all interfaces.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAdminStatGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the current version number of the OSPF protocol.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        version number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVersionNumberGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the flag to note whether this router is an area border router.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaBdrRtrStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the flag to note whether this router is configured 
*           as an Autonomous System border router.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfASBdrRtrStatusGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of opaque AS (LS type 11) link-state
*           advertisements in the link-state database.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfOpaqueASLSACountGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of external (LS type 5) link-state
*           advertisements in the link-state database.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExternalLSACountGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of self originated external (LS type 5) 
*           link-state advertisements in the link-state database.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfSelfOrigExternalLSACountGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the sum of the LS checksums of external link-state 
*           advertisements contained in the link-state database. 
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        sum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This 32-bit unsigned sum can be used to determine if there has been a change in a 
*           router's link state database, and to compare link-state database of two routers.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExternalLSAChecksumGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the router's support for type-of-service routing.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfTOSSupportGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of new link-state advertisements that have been originated.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This number is incremented each time the router originates a new LSA.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNewLSAOrigGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of link-state advertisements received 
*           determined to be new instantiations.
*           
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This number does not include newer instantiations 
*           of self-originated link-state advertisements.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNumLSAReceivedGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the maximum number of non-default AS-external-LSAs entries 
*           that can be stored in the link-state database.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This number does not include newer instantiations 
*           of self-originated link-state advertisements.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExtLsdbLimitGet ( L7_uint32 UnitIndex, L7_int32 *val );

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
L7_RC_t usmDbOspfMulticastExtensionsGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds that, after entering OverflowState,
*           a router will attempt to leave OverflowState.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This allows the router to again originate non-default AS-external-LSAs.
*           When set to 0, router will not leave OverflowState until restarted.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExitOverflowIntervalGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Gets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfspfDelayTimeGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Gets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfspfHoldTimeGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Get the router's support for demand routing.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDemandExtensionsGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
* @purpose  Set a 32-bit integer uniquely identifying the router 
*           in the Autonomous System.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         set Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRouterIdSet ( L7_uint32 UnitIndex, L7_uint32 val );

/*********************************************************************
* @purpose  Set the OSPF administrative status in the router.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         status value is either enable or disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAdminStatSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Set the flag to note whether the router is configured.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfASBdrRtrStatusSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Set the router's support for type-of-service routing.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfTOSSupportSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Set the maximum number of non-default AS-external-LSAs entries 
*           that can be stored in the link-state database.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExtLsdbLimitSet ( L7_uint32 UnitIndex, L7_int32 val );

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
L7_RC_t usmDbOspfMulticastExtensionsSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Set the number of seconds, that after entering overflow
*           state a router waits before attempting to leave overflow
*           state.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfExitOverflowIntervalSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Sets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfspfDelaySet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
* @purpose  Sets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfspfHoldTimeSet ( L7_uint32 UnitIndex, L7_int32 val );


/*********************************************************************
* @purpose  Set the router's support for demand routing.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDemandExtensionsSet ( L7_uint32 UnitIndex, L7_int32 val );


/* ospfArea */
/*********************************************************************
* @purpose  Determine if an OSPF area entry exists in configuration
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area ID
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  otherwise
*
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId );

/*********************************************************************
* @purpose  Get next OSPF area entry.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     next entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaEntryNext ( L7_uint32 UnitIndex, L7_uint32 currAreaId,
                                 L7_uint32 *areaId );

/*********************************************************************
* @purpose  Get the authentication type specified for an area.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    intIfNum    type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Additional authentication types may be assigned locally on a per Area basis.
*           This param was OBSOLETED by RFC1850
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAuthTypeGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the area's support for importing AS external link-state advertisements.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfImportAsExternGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of runs.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        number of times
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The number of times that the intra-area route table 
*           has been calculated using this area's link-state database.  
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNumSPFRunsGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of area border routers reachable within this area. 
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is initially zero, and is calculated in each SPF Pass.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaBorderRtrCountGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of Autonomous System border routers 
*           reachable  within  this area.  
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is initially zero, and is calculated in each SPF Pass.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsBdrRtrCountGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of count.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The total number of link-state advertisements in this
*           area's link-state database, excluding AS External LSA's.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSACountGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get Area LSA statistics
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
* @param    *val        pointer to LSA stats
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLsaStatsGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_OspfAreaDbStats_t * stats);

/*********************************************************************
* @purpose  Get the 32-bit unsigned sum.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        sum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This sum  excludes external (LS type 5) link-state advertisements.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSACksumSumGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the area summary.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaSummaryGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF area entry status.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaStatusGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 *val );

/*********************************************************************
* @purpose  Delete the authentication type specified for an area.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    obsolete
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAuthTypeSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Test if the area's support for importing AS external link-state
*           advertisements is settable
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfImportAsExternSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                         L7_int32 val );

/*********************************************************************
* @purpose  Set the area's support for importing AS external link-state
*           advertisements.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfImportAsExternSet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                     L7_int32 val );

/*********************************************************************
* @purpose  Test if the area summary is settable
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaSummarySetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                      L7_int32 val );

/*********************************************************************
* @purpose  Set the area summary.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaSummarySet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                  L7_int32 val );

/*********************************************************************
* @purpose  Test if the OSPF area entry status is settable.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAreaStatusSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                     L7_int32 val );

/*********************************************************************
* @purpose  Delete the OSPF area entry status.
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAreaStatusSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 val );

/*********************************************************************
* @purpose  Deletes an ospf area
*
* @param    UnitIndex @b{(input)} The unit for this operation
* @param    areaID    @b{(input)} AreaId of the area to be deleted
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
L7_RC_t usmDbOspfAreaDelete(L7_uint32 UnitIndex, L7_uint32 areaId);

/* ospfStub */
/*********************************************************************
* @purpose  Get the area entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    TOS         entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubAreaEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 TOS );

/*********************************************************************
* @purpose  Get the next area entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     type
* @param    *TOS        entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubAreaEntryNext ( L7_uint32 UnitIndex, L7_uint32 *areaId, L7_int32 *TOS );

/*********************************************************************
* @purpose  Get the stub metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area ID 
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
L7_RC_t usmDbOspfStubMetricGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 TOS, L7_uint32 *metric );

/*********************************************************************
* @purpose  Get the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     type
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
L7_RC_t usmDbOspfStubStatusGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfStubMetricTypeGet ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                     L7_uint32 TOS, L7_int32 *metricType );

/*********************************************************************
* @purpose  Test if we can set the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    TOS         entry
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubMetricSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                     L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Delete the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    TOS         entry
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubMetricSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Test if we can set the metric type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    TOS         entry
* @param    val         metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubMetricTypeSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                     L7_uint32 TOS, L7_uint32 metricType);

/*********************************************************************
* @purpose  Delete the default route metric type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    TOS         entry
* @param    val         metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubMetricTypeSet ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                     L7_uint32 TOS, L7_uint32 metricType);

/* Routing Entries corresponding to Border Routers */

/*********************************************************************
* @purpose  Get the ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    UnitIndex          L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAbrEntryGet ( L7_uint32 UnitIndex, 
                               L7_uint32 destinationIp,
                               L7_RtbEntryInfo_t *p_rtbEntry );

/*********************************************************************
* @purpose  Get the ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    UnitIndex          L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAsbrEntryGet ( L7_uint32 UnitIndex, 
                                L7_uint32 destinationIp,
                                L7_RtbEntryInfo_t *p_rtbEntry );

/*********************************************************************
* @purpose  Get the next ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    UnitIndex          L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAbrEntryNext ( L7_uint32 UnitIndex, 
                                L7_uint32 *destinationIp,
                                L7_RtbEntryInfo_t *p_rtbEntry );

/*********************************************************************
* @purpose  Get the next ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    UnitIndex          L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAsbrEntryNext ( L7_uint32 UnitIndex, 
                                 L7_uint32 *destinationIp,
                                 L7_RtbEntryInfo_t *p_rtbEntry );

/* ospfLsdb */
/*********************************************************************
* @purpose  Get the process's Link State Database entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfLsdbEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_ospfLsdbEntry_t *p_Lsa );

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     type
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
L7_RC_t usmDbOspfLsdbEntryNext ( L7_uint32 UnitIndex, L7_uint32 *areaId, L7_int32 *Type, L7_uint32 *Lsid, L7_uint32 *RouterId, L7_ospfLsdbEntry_t *p_Lsa );

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfLsdbSequenceGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                   L7_int32 Type, L7_uint32 Lsid,
                                   L7_uint32 RouterId, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the age of the link state in seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLsdbAgeGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the checksum of complete contents of advertisement,
*           excepting the age field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLsdbChecksumGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the entire Link State Advertisement, including its header.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *buf        Link State Ad
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLsdbAdvertisementGet ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                        L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, 
                                        L7_char8 **buf, L7_uint32 *buf_len );


/* ospfArea */
/*********************************************************************
* @purpose  Create an area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
* @param    advertise   L7_TRUE or L7_FALSE
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
L7_RC_t usmDbOspfAreaRangeCreate ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                   L7_uint32 ipAddr, L7_uint32 netMask );

/*********************************************************************
* @purpose  Delete an area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAreaRangeDelete ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                    L7_uint32 ipAddr, L7_uint32 netMask );

/*********************************************************************
* @purpose  Get the area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
* @param    RangeNet    range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaRangeEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet );

/*********************************************************************
* @purpose  Get the next area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     type
* @param    *RangeNet   range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaRangeEntryNext ( L7_uint32 UnitIndex, L7_uint32 *areaId, L7_uint32 *RangeNet );

/*********************************************************************
* @purpose  Get the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeMaskGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entry status.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeStatusGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_int32 *val );

/*********************************************************************
* @purpose  Get the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeEffectGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_int32 *val );

/*********************************************************************
* @purpose  Delete the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeMaskSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_uint32 val );

/*********************************************************************
* @purpose  Delete the entry status.  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeStatusSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_int32 val );

/*********************************************************************
* @purpose  Delete the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      type
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
L7_RC_t usmDbOspfAreaRangeEffectSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 RangeNet, L7_int32 val );


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
L7_RC_t usmDbOspfHostEntryGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, 
                                L7_int32 TOS );

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
L7_RC_t usmDbOspfHostEntryNext ( L7_uint32 UnitIndex,  
                                 L7_uint32 *IpAddress, L7_int32 *TOS );

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
L7_RC_t usmDbOspfHostMetricGet ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

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
L7_RC_t usmDbOspfHostStatusGet ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val );

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
* @notes    By default, the area that a subsuming OSPF interface is in, or 0.0.0.0.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfHostAreaIDGet ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_uint32 *val );

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
L7_RC_t usmDbOspfHostMetricSetTest ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

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
L7_RC_t usmDbOspfHostMetricSet ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   The unit for this operation
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
L7_RC_t usmDbOspfHostStatusSetTest ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );

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
L7_RC_t usmDbOspfHostStatusSet ( L7_uint32 UnitIndex,  
                                 L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val );


/* ospfIf */
/*********************************************************************
* @purpose  Determines if the interface entry exists
*
* @param    UnitIndex   The unit for this operation
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if the entry exists
* @returns  L7_FAILURE  if the entry does not exist
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfEntryGet ( L7_uint32 UnitIndex, L7_uint32 ipAddr, L7_int32 intIfNum );

/*********************************************************************
* @purpose  Get the next Interface Entry.
*
* @param    UnitIndex   The unit for this operation
* @param    *IpAddr     Ip address
* @param    *intIfNum     interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfEntryNext ( L7_uint32 UnitIndex, L7_uint32 *IpAddress, L7_int32 *intIfNum );

/*********************************************************************
* @purpose  Get the area id.
*
* @param    UnitIndex   The unit for this operation
* @param    IpAddr      Ip address
* @param    intIfNum    internal interface number
* @param    *val        area id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAreaIdGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr,
                               L7_int32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the OSPF interface type.
*
* @param    intIfNum    internal interface number
* @param    *val        integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfTypeGet ( L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF interface's administrative status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAdminStatGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the priority of this interface.  
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that router is not eligible to become
*           the designated router on this particular  network.   
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrPriorityGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link state 
*           update packet over this interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfTransitDelayGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds between link-state advertisement retransmissions, 
*           for  adjacencies belonging to this interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRetransIntervalGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the time in seconds between Hello packets that router sends on the interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfHelloIntervalGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets have 
*           not been seen before it's neighbors declare the router down.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This should be some multiple of Hello interval. This value 
*           must be the same for all routers attached to a common network.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrDeadIntervalGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the larger time interval, in seconds, between the Hello packets
*           sent to an inactive nonbroadcast multiaccess neighbor.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfPollIntervalGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF Interface State.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfStateGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the IP Address of the Designated Router.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfDesignatedRouterGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress, 
                                         L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the IP Address of the Backup Designated Router.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfBackupDesignatedRouterGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                               L7_int32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the number of times this OSPF interface has
*           changed its state, or an error has occurred.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfEventsGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                               L7_int32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the Authentication Key.  
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Per RFC 1850, the authentication key is never shown via
*           the MIB, but rather, an output string of length 0 is provided.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeyGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                L7_int32 intIfNum, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Get the authentication key id specified for an interface. 
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    *val        authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeyIdGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                  L7_int32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfStatusGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                               L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the way multicasts should be forwarded on this interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Always sets val to blocked, indicating no multicast forwarding.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMulticastForwardingGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                            L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  To indicate whether Demand OSPF procedures should be performed on this interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Demand OSPF procedures include hello supression to FULL neighbors
*           and setting the DoNotAge flag on proogated LSAs.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfDemandGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                               L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the authentication type specified for an interface. 
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *val        authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Additional authentication types may be assigned locally.
*                                                                 * @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthTypeGet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                 L7_int32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Test if the area id is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         area id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAreaIdSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                   L7_int32 intIfNum, L7_uint32 val );

/*********************************************************************
* @purpose  Set the area id.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         area id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAreaIdSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                               L7_int32 intIfNum, L7_uint32 val );

/*********************************************************************
* @purpose  Test if the OSPF interface type is settable.
*
* @param    intIfNum    internal interface number  
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfTypeSetTest ( L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the OSPF interface type.
*
* @param    intIfNum    internal interface number
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfTypeSet ( L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Return the default OSPF network type for a given interface
*
* @param    intIfNum  internal interface ID
* @param    netType   return value for OSPF network type
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultIfType(L7_uint32 intIfNum, L7_uint32 *netType);

/*********************************************************************
* @purpose  Test if the OSPF interface's administrative status is
*           settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 'disabled' denotes that the interface is external to OSPF.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAdminStatSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                      L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Set the OSPF interface's administrative status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 'disabled' denotes that the interface is external to OSPF.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAdminStatSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                  L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the priority of this interface is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that router is not eligible to become
*           the designated router on this particular  network.   
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrPrioritySetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                        L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Set the priority of this interface.  
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that router is not eligible to become
*           the designated router on this particular  network.   
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrPrioritySet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                    L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Tests if the Ospf Transit Delay for the specified interface 
*           is settable.
*
* @param    UnitIndex   The unit for this operation
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
L7_RC_t usmDbOspfIfTransitDelaySetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                         L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfTransitDelaySet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                     L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Tests if the Ospf Retransmit Interval for the specified interface 
*           is settable.
*
* @param    UnitIndex   The unit for this operation
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
L7_RC_t usmDbOspfIfRetransIntervalSetTest ( L7_uint32 UnitIndex,
                                            L7_uint32 ipAddress,
                                            L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRetransIntervalSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                        L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the hello interval is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfHelloIntervalSetTest ( L7_uint32 UnitIndex, L7_uint32 IpAddress,
                                          L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the time in seconds.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfHelloIntervalSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress,
                                      L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the Ospf Router dead interval is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrDeadIntervalSetTest ( L7_uint32 UnitIndex,
                                            L7_uint32 ipAddress,
                                            L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfRtrDeadIntervalSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                        L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the Ospf interface poll interval is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfPollIntervalSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                         L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the time interval.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfPollIntervalSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                     L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The authentication key is passed as an ASCIIZ string, not
*           a byte array.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeySetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                    L7_int32 intIfNum, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Delete the Authentication Key.  
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The authentication key must always be set after the auth type.
*
* @notes    The authentication key is passed as an ASCIIZ string, not
*           a byte array.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeySet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                L7_int32 intIfNum, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Test if the authentication key id is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeyIdSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                      L7_int32 intIfNum, L7_uint32 val );

/*********************************************************************
* @purpose  Set the authentication key id.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthKeyIdSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                  L7_int32 intIfNum, L7_uint32 val );

/*********************************************************************
* @purpose  Tests if the Ospf ifDemand is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Indicates whether Demand OSPF procedures should be performed on this interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfDemandSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                   L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the truth value.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Indicates whether Demand OSPF procedures should be performed on this interface.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfDemandSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                               L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthTypeSetTest ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                     L7_int32 intIfNum, L7_int32 val );

/*********************************************************************
* @purpose  Delete the authentication type.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The auth type must always be set before the key, since setting
*           the auth type causes the existing key to be cleared.
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfAuthTypeSet ( L7_uint32 UnitIndex, L7_uint32 ipAddress,
                                 L7_int32 intIfNum, L7_int32 val );


/* ospfIfMetric */
/*********************************************************************
* @purpose  Get a particular TOS metric for a non-virtual interface.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricEntryGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS );

/*********************************************************************
* @purpose  Get the next TOS metric for a non-virtual interface.
*
* @param    UnitIndex   The unit for this operation
* @param    *IPAddress  Ip address
* @param    *intIfNum     interface
* @param    *TOS        TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricEntryNext ( L7_uint32 UnitIndex, L7_uint32 *IpAddress, L7_int32 *intIfNum, L7_int32 *TOS );

/*********************************************************************
* @purpose  Get the OSPF interface metric value.
*
* @param    UnitIndex   The unit for this operation
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    *val        metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The default value of the TOS 0 Metric is 10^8 / ifSpeed.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricValueGet ( L7_uint32 UnitIndex, L7_int32 intIfNum, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Determine if the OSPF metric has been configured
*
* @param    UnitIndex   The unit for this operation
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
L7_BOOL usmDbOspfIsIfMetricConfigured (L7_uint32 UnitIndex, L7_uint32 IpAddress, 
                                       L7_int32 intIfNum, L7_int32 TOS);

/*********************************************************************
* @purpose  Get the OSPF interface metric value status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS, L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the metric value.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricValueSetTest ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Delete the metric value.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricValueSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricStatusSetTest ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS, L7_int32 val );

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   The unit for this operation
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIfMetricStatusSet ( L7_uint32 UnitIndex, L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS, L7_int32 val );


/* ospfVirtIf */
/*********************************************************************
* @purpose  Create a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      transit area
* @param    Neighbor    remote router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfCreate ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                L7_uint32 Neighbor );

/*********************************************************************
* @purpose  Delete a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfVirtIfDelete (L7_uint32 UnitIndex, L7_uint32 areaId,
                               L7_uint32 Neighbor);

/*********************************************************************
* @purpose  Get a single Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                  L7_uint32 Neighbor );

/*********************************************************************
* @purpose  Get the next Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *areaId     Id
* @param    *Neighbor   entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfEntryNext ( L7_uint32 UnitIndex, L7_uint32 *areaId,
                                   L7_uint32 *Neighbor );

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfTransitDelayGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfRetransIntervalGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfHelloIntervalGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfRtrDeadIntervalGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF virtual interface state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfStateGet ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                  L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the OSPF virtual interface metric.  This is the sum of
*           the OSPF metrics of the links in the path to the virtual 
*           neighbor.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      The OSPF area the virtual link traverses
* @param    neighbor    The router ID of the virtual neighbor
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfMetricGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                  L7_uint32 neighbor, L7_uint32 *metric );

/*********************************************************************
* @purpose  Get the number of state changes or error events on this Virtual Link.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfEventsGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

/*********************************************************************
* @purpose  Get the authentication info specified for a virtual interface.
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    areaId      @b{(input)}  Id
* @param    Neighbor    @b{(input)}  entry info
* @param    type        @b{(output)} authentication type
* @param    key         @b{(output)} authentication key
* @param    keyId       @b{(output)} authentication key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The keyId is only meaningful for MD5 authentication; 
*           otherwise, it should be ignored.
*
* @notes    This function outputs the actual key value, so should not be
*           used in support of the OSPF MIB.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthenticationGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                           L7_uint32 Neighbor, L7_uint32 *type,
                                           L7_uchar8 *key, L7_uint32 *keyId );

/*********************************************************************
* @purpose  Get the authentication type specified for a virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    *val        authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Additional authentication types may be assigned locally.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthTypeGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                     L7_uint32 Neighbor, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the Authentication Key.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Per RFC 1850, the authentication key is never shown via
*           the MIB, but rather, an output string of length 0 is provided.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeyGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Get the Authentication Key.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The authentication key is output as an ASCIIZ string, not
*           a byte array.
*
* @notes    This API does provide the contents of the authentication
*           key, thus it should not be used if the key contents are 
*           to be kept hidden, such as when supporting the OSPF MIB.
*           Use usmDbOspfVirtIfAuthKeyGet instead.
*       
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeyActualGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                          L7_uint32 Neighbor, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Get the authentication key id specified for a virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    *val        authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeyIdGet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                      L7_uint32 Neighbor, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
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
L7_RC_t usmDbOspfVirtIfStatusGet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 *val );

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
L7_RC_t usmDbOspfVirtIfTransitDelaySetTest ( L7_uint32 UnitIndex,
                                             L7_uint32 areaId,
                                             L7_uint32 Neighbor,
                                             L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfTransitDelaySet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Tests if the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface is settable
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
L7_RC_t usmDbOspfVirtIfRetransIntervalSetTest ( L7_uint32 UnitIndex,
                                                L7_uint32 areaId,
                                                L7_uint32 Neighbor,
                                                L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfRetransIntervalSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Tests if the time, in seconds, between the Hello packets that 
*           the router sends on the interface is settable
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfVirtIfHelloIntervalSetTest ( L7_uint32 UnitIndex,
                                              L7_uint32 areaId,
                                              L7_uint32 Neighbor,
                                              L7_int32 val );

/*********************************************************************
* @purpose  Delete the time.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfHelloIntervalSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Tests if the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down
*           is settable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfVirtIfRtrDeadIntervalSetTest ( L7_uint32 UnitIndex,
                                                L7_uint32 areaId,
                                                L7_uint32 Neighbor,
                                                L7_int32 val );

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfRtrDeadIntervalSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Set the authentication info specified for a virtual interface.
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    areaId      @b{(input)} Id
* @param    Neighbor    @b{(input)} entry info
* @param    type        @b{(input)} authentication type
* @param    key         @b{(input)} authentication key
* @param    keyId       @b{(input)} authentication key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The keyId is only used for MD5 authentication; it is otherwise
*           ignored.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthenticationSet ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                           L7_uint32 Neighbor, L7_uint32 type,
                                           L7_uchar8 *key, L7_uint32 keyId );

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfVirtIfAuthTypeSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                         L7_uint32 Neighbor, L7_uint32 val );

/*********************************************************************
* @purpose  Delete the authentication type.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The auth type must always be set before the key, since setting
*           the auth type causes the existing key to be cleared.
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthTypeSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_uint32 val );

/*********************************************************************
* @purpose  Tests if the Authentication Key is settable.
*             
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*   
* @notes    The authentication key is passed as an ASCIIZ string, not
*           a byte array.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeySetTest ( L7_uint32 UnitIndex,
                                        L7_uint32 areaId,
                                        L7_uint32 Neighbor,
                                        L7_uchar8 *buf );

/*********************************************************************
* @purpose  Delete the Authentication Key.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The authentication key must always be set after the auth type.
*
* @notes    The authentication key is passed as an ASCIIZ string, not
*           a byte array.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeySet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_uchar8 *buf );

/*********************************************************************
* @purpose  Test if the authentication key id is settable for a virtual intf.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeyIdSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                          L7_uint32 Neighbor, L7_uint32 val );

/*********************************************************************
* @purpose  Set the authentication key id for a virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         authentication key id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfAuthKeyIdSet ( L7_uint32 UnitIndex,
                                      L7_uint32 areaId,
                                      L7_uint32 Neighbor,
                                      L7_uint32 val );

/*********************************************************************
* @purpose  Test if the entry status is settable.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfVirtIfStatusSetTest ( L7_uint32 UnitIndex, L7_uint32 areaId,
                                       L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      Id
* @param    Neighbor    entry info
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.  
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtIfStatusSet ( L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 Neighbor, L7_int32 val );

/*********************************************************************
* @purpose  Check if a virtual link has been configured across the
*           specified area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_BOOL usmDbOspfVirtIfCheck (L7_uint32 UnitIndex, L7_uint32 areaId);

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
L7_RC_t usmDbOspfNbrEntryGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum );

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
L7_RC_t usmDbOspfNbrEntryNext ( L7_uint32 UnitIndex, L7_uint32 *IpAddr, L7_uint32 *intIfNum );

/*********************************************************************
* @purpose  Get the IpAddress identifying neighboring router in Autonomous System.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                router ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrRtrIdGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get a Bit Mask corresponding to the neighbor's options field.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrOptionsGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the priority of this neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrPriorityGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the Neighbor's relationship State.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrStateGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the Neighbor's relationship Interface State.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address representing neighbor's routerId
* @param    intIfNum            internal interface number
* @param    *val                state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrIntfStateGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr,
                                   L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Get the number of times this neighbor relationship
*           has changed state, or an error has occurred.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                number of state changes 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrEventsGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the current length of the retransmission queue.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrLsRetransQLenGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_uint32 *val );

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
L7_RC_t usmDbOspfNbmaNbrStatusGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

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
L7_RC_t usmDbOspfNbmaNbrPermanenceGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Indicate whether Hellos are being suppressed to the neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrHelloSuppressedGet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 *val );

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
L7_RC_t usmDbOspfNbrPrioritySetTest ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val );

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
L7_RC_t usmDbOspfNbrPrioritySet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val );

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
L7_RC_t usmDbOspfNbmaNbrStatusSetTest ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val );

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
L7_RC_t usmDbOspfNbmaNbrStatusSet ( L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val );


/* ospfVirtNbr */
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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrEntryGet ( L7_uint32 UnitIndex, 
                                   L7_uint32 Area, L7_uint32 RtrId );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrEntryNext ( L7_uint32 UnitIndex, 
                                    L7_uint32 *Area, L7_uint32 *RtrId );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrIpAddrGet ( L7_uint32 UnitIndex, 
                                    L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val );

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
*           Bit 2, indicates that system implements OSPF Multicast Routing.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrOptionsGet ( L7_uint32 UnitIndex, 
                                     L7_uint32 Area, L7_uint32 RtrId, L7_int32 *val );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrStateGet ( L7_uint32 UnitIndex, 
                                   L7_uint32 Area, L7_uint32 RtrId, L7_int32 *val );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrEventsGet ( L7_uint32 UnitIndex, 
                                    L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrLsRetransQLenGet ( L7_uint32 UnitIndex, 
                                           L7_uint32 Area, L7_uint32 RtrId,
                                           L7_uint32 *val );

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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfVirtNbrHelloSuppressedGet ( L7_uint32 UnitIndex,
                                             L7_uint32 Area,
                                             L7_uint32 RtrId, L7_int32 *val );


/* ospfExtLsdb */
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
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExtLsdbEntryGet ( L7_uint32 UnitIndex, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_ospfLsdbEntry_t *p_Lsa );

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
L7_RC_t usmDbOspfExtLsdbEntryNext ( L7_uint32 UnitIndex, L7_int32 *Type, L7_uint32 *Lsid, L7_uint32 *RouterId, L7_ospfLsdbEntry_t *p_Lsa );

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
L7_RC_t usmDbOspfExtLsdbSequenceGet ( L7_uint32 UnitIndex, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_uint32 *val );

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
L7_RC_t usmDbOspfExtLsdbAgeGet ( L7_uint32 UnitIndex, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val );

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
L7_RC_t usmDbOspfExtLsdbChecksumGet ( L7_uint32 UnitIndex, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val );

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
L7_RC_t usmDbOspfExtLsdbAdvertisementGet ( L7_uint32 UnitIndex, L7_int32 Type, 
                                           L7_uint32 Lsid, L7_uint32 RouterId, 
                                           L7_char8 **buf, L7_uint32 *len );

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
L7_RC_t usmDbOspfExtLsdbAdvertisementLengthGet ( L7_uint32 UnitIndex, L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_uint32 *val );

/* ospfAreaAggregate */
/*********************************************************************
* @purpose  Check if the specified area aggregate range exists
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
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
L7_RC_t usmDbOspfAreaAggregateEntryGet ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                         L7_uint32 Type, L7_int32 Net, 
                                         L7_uint32 Mask );


/*********************************************************************
* @purpose  Get the next area range entry for a given OSPF area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaID     area id
* @param    *Type       link state type. Set value (not pointer) to 0 to get 
*                       first range in this area.
* @param    *Net        network
* @param    *Mask       mask
*
* @returns  L7_SUCCESS  if the next range is successfully retrieved
* @returns  L7_FAILURE  if no more area ranges are configured
* @returns  L7_ERROR    if the input parameters are in error.
*
* @notes    The above parameters are used to both pass input and receive output.
*           This API only returns ranges configured on the area specified by 
*           AreaId.
*
*           Unless Type is 0 (in which case the function returns the first range
*           in the area), AreaID, Type, Net, and Mask must describe an area
*           range currently in the configuration. These parameters are used to
*           find the range previously returned, and then find the next value. If
*           the previous range cannot be found, L7_ERROR is returned.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaAggregateEntryNext ( L7_uint32 UnitIndex, L7_uint32 *AreaID, 
                                          L7_uint32 *Type, L7_int32* Net, 
                                          L7_uint32 *Mask );

/*********************************************************************
* @purpose  Get the next area range entry for any OSPF area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaID     area id. May be modified if there are no more ranges configured
*                       for the input area, but there are ranges configured for other
*                       areas.
* @param    *Type       link state type. Set value (not pointer) to 0 to get first range.
* @param    *Net        network
* @param    *Mask       mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no more area ranges are configured
*
* @notes    The above parameters are used to both pass input and receive output.
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
L7_RC_t usmDbOspfAnyAreaAggregateEntryNext ( L7_uint32 UnitIndex, L7_uint32 *AreaID, 
                                             L7_uint32 *Type, L7_int32 *Net, L7_uint32 *Mask );

/*********************************************************************
* @purpose  Check if the entry is active.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status  (L7_TRUE, if active, 
*                                      L7_FALSE, if inactive or nonexistent)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaAggregateStatusGet ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                          L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, 
                                          L7_int32 *val );

/*********************************************************************
* @purpose  Get area range advertisement configuration
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    *val        subnet effect  (L7_OSPF_AREA_AGGREGATE_STATUS_t)
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
L7_RC_t usmDbOspfAreaAggregateEffectGet ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                          L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, 
                                          L7_int32 *val );

/*********************************************************************
* @purpose  Test to set the OSPF Area Range
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    command     entry status (L7_CREATE or L7_DELETE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Note that the implementation provides no mechanism to make an
*           area range temporarily inoperative.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaAggregateStatusSetTest ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                          L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, 
                                          L7_int32 command );

/*********************************************************************
* @purpose  Create or delete the OSPF Area Range
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    command     entry status (L7_CREATE or L7_DELETE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Note that the implementation provides no mechanism to make an
*           area range temporarily inoperative.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaAggregateStatusSet ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                          L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, 
                                          L7_int32 command );

/*********************************************************************
* @purpose  Test to set area range advertisement configuration
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect  (L7_OSPF_AREA_AGGREGATE_STATUS_t)
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
L7_RC_t usmDbOspfAreaAggregateEffectSetTest ( L7_uint32 UnitIndex, L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val );

/*********************************************************************
* @purpose  Set area range advertisement configuration
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect  (L7_OSPF_AREA_AGGREGATE_STATUS_t)
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
L7_RC_t usmDbOspfAreaAggregateEffectSet ( L7_uint32 UnitIndex, L7_uint32 AreaID, 
                                          L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, 
                                          L7_int32 val );


/*********************************************************************
*
* @purpose  Allows a user to set the specified trap to either
*           is enabled or disabled
*
* @param    unitIndex Unit Index
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
L7_RC_t usmDbOspfTrapModeSet(L7_uint32 unitIndex, L7_uint32 mode, L7_uint32 trapType);


/*********************************************************************
*
* @purpose  Allows a user to determine whether the specified trap
*           is enabled or disabled
*
* @param    unitIndex Unit Index
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
L7_RC_t usmDbOspfTrapModeGet(L7_uint32 unitIndex, L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
* @purpose  Set the summary LSA mode for a given stub area.
*          
* @param    areaId      type
* @param    val         variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*             
*             Valid inputs:
*               L7_ENABLE
*               L7_DISABLE
*
*             If it is L7_DISABLE, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is L7_ENABLE, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfMapAreaSummarySet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 val);

/*********************************************************************
* @purpose  Get the summary LSA mode for a given stub area.
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
*               L7_ENABLE
*               L7_DISABLE
*
*             If it is L7_DISABLE, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is L7_ENABLE, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfMapAreaSummaryGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_int32 *val);

/*********************************************************************
* @purpose  Get the information associated with NSSA configuration
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSAEntryGet ( L7_uint32 UnitIndex, L7_uint32 areaId, 
                                L7_uint32 *metric, L7_uint32 *metricType);

/*********************************************************************
* @purpose  Given a NSSA area specification, return the area ID
*           of the next NSSA area
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area id
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNSSAEntryNext (L7_uint32 UnitIndex, L7_uint32 *areaId, 
                                L7_uint32 *TOS);

/*********************************************************************
* @purpose  Get the NSSA Translator Role of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSATranslatorRoleGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                        L7_uint32 *nssaTR);

/*********************************************************************
* @purpose  Set the NSSA Translator Role of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSATranslatorRoleSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                        L7_uint32 nssaTR);

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
L7_RC_t usmDbOspfNSSATranslatorStateGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                         L7_uint32 *nssaTRState);

/*********************************************************************
* @purpose  Get the Translator Stability Interval of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSATranslatorStabilityIntervalGet (L7_uint32 UnitIndex, 
                                                     L7_uint32 areaId, 
                                                     L7_uint32 *stabInt);

/*********************************************************************
* @purpose  Set the Translator Stability Interval of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSATranslatorStabilityIntervalSet (L7_uint32 UnitIndex, 
                                                     L7_uint32 areaId, 
                                                     L7_uint32 stabInt);

/*********************************************************************
* @purpose  Get the import summary configuration for the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSAImportSummariesGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                         L7_uint32 *impSum);

/*********************************************************************
* @purpose  Set the import summary configuration for the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSAImportSummariesSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                         L7_uint32 impSum);

/*********************************************************************
* @purpose  Get the route redistribution configuration for the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSARedistributeGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                      L7_uint32 *redist);

/*********************************************************************
* @purpose  Set the route redistribution configuration for the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSARedistributeSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                      L7_uint32 redist);

/*********************************************************************
* @purpose  Get the default information origination configuration for 
*           the specified NSSA, includes metric & metric type
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultInfoGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                     L7_uint32 *defInfoOrg, 
                                     L7_OSPF_STUB_METRIC_TYPE_t *metricType, 
                                     L7_uint32 *metric);

/*********************************************************************
* @purpose  Set the default information origination configuration for 
*           the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultInfoSet(L7_uint32 UnitIndex, L7_uint32 areaId, 
                                    L7_uint32 defInfoOrg, 
                                    L7_OSPF_STUB_METRIC_TYPE_t metricType,
                                    L7_uint32 metric);

/*********************************************************************
* @purpose  Get the default information origination configuration for 
*           the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultInfoOriginateGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                              L7_uint32 *defInfoOrg);

/*********************************************************************
* @purpose  Set the default information origination configuration for 
*           the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultInfoOriginateSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                              L7_uint32 defInfoOrg);

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultMetricTypeGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                           L7_OSPF_STUB_METRIC_TYPE_t *metricType);

/*********************************************************************
* @purpose  Set the metric type of the default route for the NSSA.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfNSSADefaultMetricTypeSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                           L7_OSPF_STUB_METRIC_TYPE_t metricType );

/*********************************************************************
* @purpose  Get the NSSA metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      area ID
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNSSADefaultMetricGet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                       L7_uint32 *metric );

/*********************************************************************
* @purpose  Set the value of the default metric for the given NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId      areaID
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNSSADefaultMetricSet (L7_uint32 UnitIndex, L7_uint32 areaId, 
                                       L7_uint32 metric );

/*********************************************************************
* @purpose  Get if the opaque capability is enabled.
*
* @param    opaqueEnabled         OpaqueCapability support.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfOpaqueLsaSupportGet(L7_uint32 *opaqueEnabled);

/*********************************************************************
* @purpose  Set the opaque capability to enabled.
*
* @param    opaqueEnabled         OpaqueCapability support.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfOpaqueLsaSupportSet(L7_uint32 opaqueEnabled);

/*********************************************************************
* @purpose  Get the process's AreaOpaque Link State Database entry.
*
* @param    areaId      area ID
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertising RouterId
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaOpaqueLsdbEntryGet (L7_uint32 areaId,
                                L7_int32 Type, L7_uint32 Lsid,
                                L7_uint32 RouterId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's AreaOpaque Link State Database next entry.
*
* @param    areaId      area ID
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertising RouterId
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaOpaqueLsdbEntryNext(L7_uint32 *areaId,
                                L7_int32 *Type, L7_uint32 *Lsid,
                                L7_uint32 *RouterId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    areaId      area
* @param    Type        type
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
L7_RC_t usmDbOspfAreaOpaqueLsdbSequenceGet ( L7_uint32 areaId,
                                   L7_int32 Type, L7_uint32 Lsid,
                                   L7_uint32 RouterId, L7_uint32 *val );

/*********************************************************************
* @purpose  Get the age of the link state in seconds.
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaOpaqueLsdbAgeGet ( L7_uint32 areaId,
                              L7_int32 Type, L7_uint32 Lsid,
                              L7_uint32 RouterId, L7_int32 *val );
/*********************************************************************
* @purpose  Get the checksum of the type-10 area-opaque LSA.
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaOpaqueLsdbChecksumGet ( L7_uint32 areaId,
                              L7_int32 Type, L7_uint32 Lsid,
                              L7_uint32 RouterId, L7_int32 *val );

/*********************************************************************
* @purpose  Get the type-10 Advertisement LSA.
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaOpaqueLsdbAdvertisementGet( L7_uint32 areaId,
                              L7_int32 Type, L7_uint32 Lsid,
                              L7_uint32 RouterId, L7_char8 **buf,
                              L7_uint32 *len);

/*********************************************************************
* @purpose  Get the process's LinkOpaque Link State Database entry.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbEntryGet (L7_uint32 ipAddr, L7_int32 ifIndex,
                                L7_int32 type, L7_uint32 lsId,
                                L7_uint32 rtrId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's LinkOpaque Link State Database next entry.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbEntryNext (L7_uint32 *ipAddr, L7_int32 *ifIndex,
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
* @param    val         Pointer to sequence Number.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbSequenceGet (L7_uint32 ipAddr, L7_int32 ifIndex,
                                L7_int32 type, L7_uint32 lsId,
                                L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the age of the link state in seconds.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    val         Pointer to LS AGE.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbAgeGet (L7_uint32 ipAddr, L7_int32 ifIndex,
                                L7_int32 type, L7_uint32 lsId,
                                L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the type-9 Link-opaque LSA.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    val         Pointer to LS CHECKSUM.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbChecksumGet (L7_uint32 ipAddr, L7_int32 ifIndex,
                                L7_int32 type, L7_uint32 lsId,
                                L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the type-9 Advertisement LSA.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    buf         Pointer to LSA.
* @param    len         Pointer to length of LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLinkOpaqueLsdbAdvertisementGet( L7_uint32 ipAddr,
                              L7_uint32 ifIndex, L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the process's AsOpaque Link State Database entry.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbEntryGet ( L7_int32 type, L7_uint32 lsId,
                                        L7_uint32 rtrId,
                                        L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the process's AsOpaque Link State Database next entry.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbEntryNext (L7_int32 *type, L7_uint32 *lsId,
                                        L7_uint32 *rtrId,
                                        L7_ospfOpaqueLsdbEntry_t *p_Lsa);

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    val         Pointer to sequence Number.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbSequenceGet (L7_int32 type, L7_uint32 lsId,
                                          L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the age of the link state in seconds.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    val         Pointer to LS AGE.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbAgeGet (L7_int32 type, L7_uint32 lsId,
                                     L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the checksum of the type-11 Link-opaque LSA.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    val         Pointer to LS CHECKSUM.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbChecksumGet (L7_int32 type, L7_uint32 lsId,
                                          L7_uint32 rtrId, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the type-11 Advertisement LSA.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    buf         Pointer to LSA.
* @param    len         Pointer to length of LSA.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsOpaqueLsdbAdvertisementGet(L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get checksumSum of the AS OPAQUE LSA.
*
* @param    val         CHECKsum SUM.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfASOpaqueLSAChecksumSumGet(L7_uint32 *val);

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
L7_RC_t usmDbOspfIsStubRtrGet(L7_BOOL *isStubRtr);

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
L7_RC_t usmDbOspfExtLsdbOverflowGet(L7_BOOL *extLsdbOverflow);

#endif
