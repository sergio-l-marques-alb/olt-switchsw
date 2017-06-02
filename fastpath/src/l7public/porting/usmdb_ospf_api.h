
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  usmdb_ospfmap.c
*
* @purpose    Provide interface to ospf API's for unitmgr components
*
* @component  unitmgr
*
* @comments   none
*
* @create     03/21/2001
*
* @author     anayar
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef USMDB_OSPF_API_H
#define USMDB_OSPF_API_H
#include "l3_commdefs.h"
#include "usmdb_mib_ospf_api.h"

/*********************************************************************
* @purpose  Get general OSPFv2 status information.
*
* @param    *status     contains output data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is status, rather than configuration, information.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStatusGet(L7_ospfStatus_t *status);

/*********************************************************************
* @purpose  Gets the Ospf Admin Mode
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Admin mode
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Gets the ASBR Admin Mode
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsbrAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   UnitIndex @b{(input)} the unit for this operation. 
* @param   maxPaths  @b{(output)}  the maximum number of paths allowed
* 
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if maxPaths is NULL
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfMaxPathsGet(L7_uint32 *maxPaths);

/*********************************************************************
* @purpose  Set the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   UnitIndex   @b{(input)} the unit for this operation. 
* @param   maxPaths @b{(input)} the maximum number of paths allowed
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if maxPaths is out of range
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfMaxPathsSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Set the auto cost reference bandwidth for OSPF links.
*
* @param   autoCostRefBw @b{(output)} the autoCost reference bandwidth allowed
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if autoCostRefBw is NULL
*
* @notes    autoCostRefBw may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW, inclusive
*
* @end
 *********************************************************************/
L7_RC_t usmDbOspfAutoCostRefBwGet( L7_uint32 *autoCostRefBw);

/*********************************************************************
* @purpose  Set the auto cost reference bandwidth for OSPF links.
*
* @param   UnitIndex   @b{(input)} the unit for this operation.
* @param   autoCostRefBw @b{(input)} the autoCost reference bandwidth allowed
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if autoCostRefBw is out of range
*
* @notes    autoCostRefBw may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW, inclusive
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAutoCostRefBwSet(L7_uint32 UnitIndex, L7_uint32 autoCostRefBw);

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
L7_RC_t usmDbOspfStubRtrModeSet(OSPF_STUB_ROUTER_CFG_t stubRtrMode, 
                                L7_uint32 startupDuration);

/*********************************************************************
* @purpose  Set the configured stub router advertisement mode.
*
* @param    overrideSummaryMetric   @b{(input)} if L7_ENABLE, override the metric
*                                               in summary LSAs when in stub router mode.
* @param    summaryLsaMetric        @b{(input)} metric value
*
* @returns  L7_SUCCESS if configuration is successfully stored
*           L7_ERROR if the metric is out of range
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubRtrSumLsaMetricOverride(L7_uint32 overrideSummaryMetric,
                                             L7_uint32 summaryLsaMetric);

/*********************************************************************
* @purpose  Get the configured stub router advertisement mode.
*
* @param    stubRtrMode     @b{(output)} the configured stub router mode
* @param    startupDuration @b{(output)} If the configured stub router mode is
*                                        stub router at startup, the startup 
*                                        time in seconds is set here. May be NULL
*                                        if caller doesn't care about startup time.
* @param    overrideSummaryMetric @b{(output)}  L7_ENABLE or L7_DISABLE. May be NULL.
* @param    summaryMetric   @b{(output)} Metric to set in summary LSAs in stub 
*                                        router mode, if overriding summary metric.
*                                        May be NULL. Only set if overrideSummaryMetric
*                                        is L7_ENABLE.
*
* @returns  L7_SUCCESS if configuration is successfully retrieved
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfStubRtrModeGet(OSPF_STUB_ROUTER_CFG_t *stubRtrMode, 
                                L7_uint32 *startupDuration, 
                                L7_uint32 *overrideSummaryMetric, 
                                L7_uint32 *summaryMetric);

/*********************************************************************
* @purpose  Sets the ASBR Admin Mode
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAsbrAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Determines if Ospf is compatible with RFC 1583
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRfc1583CompatibilityGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Configures if Ospf is compatible with RFC 1583
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRfc1583CompatibilitySet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Determines the value of MtuIgnore flag
*
* @param    UnitIndex   @b{(input)}
* @param    intIfNum    @b{(input)}
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfMtuIgnoreGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_BOOL *val);

/*********************************************************************
* @purpose  Configures the value of MtuIgnore flag
*
* @param    UnitIndex   @b{(input)}
* @param    intIfNum    @b{(input)}
* @param    val         @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfMtuIgnoreSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_BOOL val);

#ifdef L7_NSF_PACKAGE
/*********************************************************************
* @purpose  Get the graceful restart NSF support configuration
*
* @param    nsfSupport @b{(output)} whether this router will do graceful restart
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNsfSupportGet(OSPF_NSF_SUPPORT_t *nsfSupport);

/*********************************************************************
* @purpose  Set the graceful restart NSF support configuration
*
* @param    nsfSupport @b{(input)} whether this router will do graceful restart
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNsfSupportSet(OSPF_NSF_SUPPORT_t nsfSupport);

/*********************************************************************
* @purpose  Get the graceful restart interval (seconds)
*
* @param    restartInterval @b{(output)} number of seconds helpful neighbors
*                                        overlook our infirmities
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRestartIntervalGet(L7_uint32 *restartInterval);

/*********************************************************************
* @purpose  Configure the graceful restart interval in seconds.
*
* @param    restartInterval @b{(input)} number of seconds helpful neighbors
*                                       overlook our infirmities
*
* @returns  L7_SUCCESS 
*           L7_ERROR if restart interval is out of range
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRestartIntervalSet(L7_uint32 restartInterval);
#endif

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
L7_RC_t usmDbOspfHelperSupportGet(OSPF_HELPER_SUPPORT_t *helperSupport);

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
L7_RC_t usmDbOspfHelperSupportSet(OSPF_HELPER_SUPPORT_t helperSupport);

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
L7_RC_t usmDbOspfStrictLsaCheckingGet(L7_BOOL *strictLsaChecking);

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
L7_RC_t usmDbOspfStrictLsaCheckingSet(L7_BOOL strictLsaChecking);

/*********************************************************************
* @purpose  Gets the Ospf Admin mode configured for the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfConfigAdminModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Admin mode effective on the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAdminModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Admin mode for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAdminModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Area Id configured on the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfConfigAreaIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                               L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Area Id effective on the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAreaIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                               L7_uint32 *val);

/*********************************************************************
* @purpose  Get the first configured OSPF area ID. 
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    areaID      @b{(input)}
* @param    *nextAreaId @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @comments  This API gets the first configured area ID and does not take 
*            into account whether an
*           area is actually in use on this router. For example, if 
*           the user has configured area x to be a stub area, but has
*           not assigned any interface to the area, then this API will
*           return area x. usmDbOspfAreaIdGet() depends on the actual 
*           protocol state.
*           This API would be appropriate for getting information for
*           show running-config in the CLI.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaCfgGetFirst(L7_uint32 UnitIndex, L7_uint32 *areaId);

/*********************************************************************
* @purpose  Gets the area ID of the subsequent area given a
*           starting point area ID. 
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    areaID      @b{(input)}
* @param    *nextAreaId @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @comments  This API traverses the areas that 
*           are configured and does not take into account whether an
*           area is actually in use on this router. For example, if 
*           the user has configured area x to be a stub area, but has
*           not assigned any interface to the area, then this API will
*           return area x. usmDbOspfAreaIdGetNext() and 
*           usmDbOspfAreaEntryNext() depend on the actual protocol state.
*           This API would be appropriate for getting information for
*           show running-config in the CLI.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaCfgGetNext(L7_uint32 UnitIndex, L7_uint32 areaId,
                                L7_uint32 *nextAreaId);

/*********************************************************************
* @purpose  Sets the Ospf Area Id for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAreaIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                               L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Secondary addresses advertisability setting
*           configured on the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(output)} secondaries flag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfConfigSecondariesFlagGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                              L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Ospf Secondary addresses advertisability setting
*           effective on the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(output)} secondaries flag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfSecondariesFlagGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                        L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Secondary addresses advertisability
*           for the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(input)} secondaries flag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfSecondariesFlagSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                        L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Priority for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Priority for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Hello Interval for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfHelloIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Hello Interval for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfHelloIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uint32 val);

/*********************************************************************
* @purpose  Get the passive mode for the specified interface
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    passiveMode @b{(output)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfPassiveModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                    L7_BOOL *passiveMode);

/*********************************************************************
* @purpose  Sets the passive mode for the specified interface
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    passiveMode @b{(input)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfPassiveModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                    L7_BOOL passiveMode);

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
L7_RC_t usmDbOspfNetworkAreaEntryGet (L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                      L7_uint32 wildcardMask, L7_int32 areaId);

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
L7_RC_t usmDbOspfNetworkAreaEntryNext(L7_uint32 UnitIndex, L7_uint32 *ipAddr,
                                      L7_uint32 *wildcardMask, L7_int32 *areaId);

/*********************************************************************
* @purpose  Creates/Updates the network configured in an ospf area
*
* @param    UnitIndex    @b{(input)} the unit for this operation
* @param    ipAddr       @b{(input)} network address
* @param    wildcardMask @b{(input)} ospf wildcardmask with don't-care
*                                    bits
* @param    areaId       @b{(input)} area for this network
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNetworkAreaEntryCreate(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                        L7_uint32 wildcardMask, L7_uint32 areaId);

/*********************************************************************
* @purpose  Deletes the network configured in an ospf area
*
* @param    UnitIndex    @b{(input)} the unit for this operation
* @param    ipAddr       @b{(input)} network address
* @param    wildcardMask @b{(input)} ospf wildcardmask with don't-care
*                                    bits
* @param    areaId       @b{(input)} area for this network
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNetworkAreaEntryDelete(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                        L7_uint32 wildcardMask, L7_uint32 areaId);

/*********************************************************************
* @purpose  Gets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    spfDelay    @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfSpfDelayGet(L7_uint32 UnitIndex, L7_uint32 *spfDelay);

/*********************************************************************
* @purpose  Sets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    spfDelay    @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfSpfDelaySet(L7_uint32 UnitIndex, L7_uint32 spfDelay);

/*********************************************************************
* @purpose  Gets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    spfHoldtime @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfSpfHoldtimeGet(L7_uint32 UnitIndex, L7_uint32 *spfHoldtime);

/*********************************************************************
* @purpose  Sets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    spfHoldtime @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfSpfHoldtimeSet(L7_uint32 UnitIndex, L7_uint32 spfHoldtime);

/*********************************************************************
* @purpose  Get the default passive mode on the interfaces
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    passiveMode @b{(output)} Passive Mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfPassiveModeGet(L7_uint32 UnitIndex, L7_BOOL *passiveMode);

/*********************************************************************
* @purpose  Sets the passive mode on the interfaces
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    passiveMode @b{(input)} Passive Mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfPassiveModeSet(L7_uint32 UnitIndex, L7_BOOL passiveMode);

/*********************************************************************
* @purpose  Gets the Ospf Dead Interval for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfDeadIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Dead Interval for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfDeadIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Retransmit Interval for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfRxmtIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Retransmit Interval for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfRxmtIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Nbma Poll Interval for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfNbmaPollIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Nbma Poll Interval for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfNbmaPollIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                         L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Transit Delay for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfTransitDelayGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Transit Delay for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfTransitDelaySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                     L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Authentication information for the specified interface
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    intIfNum    @b{(input)}  internal interface number
* @param    type        @b{(output)} authentication type
* @param    key         @b{(output)} authentication key
* @param    keyId       @b{(output)} authentication key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The keyId is only used for MD5 authentication; it is otherwise
*           ignored.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthenticationGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                       L7_uint32 *type, L7_uchar8 *key,
                                       L7_uint32 *keyId);

/*********************************************************************
* @purpose  Sets the Ospf Authentication information for the specified interface
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
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
L7_RC_t usmDbOspfIntfAuthenticationSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                       L7_uint32 type, L7_uchar8 *key,
                                       L7_uint32 keyId);

/*********************************************************************
* @purpose  Gets the Ospf Authentication Type for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Type for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The auth type must always be set before the key, since setting
*           the auth type causes the existing key to be cleared.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Authentication Key for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The authentication key is output as an ASCIIZ string, not
*           a byte array.
*
* @notes    This API does reveal the contents of the authentication
*           key, thus it should not be used if the key contents are
*           to be kept hidden.  Use usmDbOspfIfAuthKeyGet instead.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                L7_uchar8 *val);

/*********************************************************************
* @purpose  Gets the actual Ospf Authentication Key for the specified interface
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    intIfNum    @b{(input)}L7_uint32 internal interface number
* @param    val         @b{(output)}L7_uchar8* output buffer location
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This function is provided for symmetry with the Virtual Intf
*           API functions.  Since this is the non-MIB API file, it does
*           the same thing as usmDbOspfIntfAuthKeyGet, outputting the
*           actual auth key value.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthKeyActualGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uchar8 *val);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The authentication key must always be set after the auth type.
*
* @notes    The authentication key is passed as an ASCIIZ string, not
*           a byte array.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                L7_uchar8 *val);

/*********************************************************************
* @purpose  Gets the Ospf Authentication Key ID for the specified interface
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)} L7_uint32 key ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthKeyIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key ID for the specified interface
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(input)}L7_uint32 key ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfAuthKeyIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Virtual Transit Area Id for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfVirtTransitAreaIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                          L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface
*
* @param    val   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfVirtTransitAreaIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                          L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Virtual Neighbor for the specified interface
*
* @param    val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfVirtIntfNeighborGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                         L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ospf Virtual Neighbor for the specified interface
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfVirtIntfNeighborSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                         L7_uint32 areaId, L7_uint32 neighbor);

/*********************************************************************
* @purpose  Gets the ABR Admin Mode
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAbrAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of  seconds  that,  after  entering
*           OverflowState,  a  router will attempt to leave
*           OverflowState
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)} L7_uint32 Exit Overflow Interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExitOverFlowIntervalGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the  maximum   number   of   non-default   AS-
*           external-LSAs entries that can be stored in the
*          link-state database
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)} External LSA Limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfExternalLSALimitGet(L7_uint32 UnitIndex, L7_int32 *val);


/*********************************************************************
* @purpose  Gets the Ospf LSA Acknowledgement Interval for the specified interface
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfLSAAckIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the OSPF Interface State
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t usmDbOspfIntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats);

/*********************************************************************
* @purpose  Gets the OSPF Designated Router Id
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfDrIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the OSPF Backup Designated Router Id
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfBackupDrIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);



/*********************************************************************
* @purpose  Gets the total number of Local Link Opaque LSAs
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfLocalLinkOpaqueLSAsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val );


/*********************************************************************
* @purpose  Gets the checksum of Local Link Opaque LSAs
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfLocalLinkOpaqueLSACksumGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val );


/*********************************************************************
* @purpose  Gets the Ospf IP Address for the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfIPAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the Ospf Subnet Mask for the specified interface
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfSubnetMaskGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the number of times this  OSPF  interface  has
*            changed its state
*
* @param    UnitIndex   @b{(input)}L7_uint32 the unit for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfIntfLinkEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Retrieve the Aging Interval for the specified AreaID
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 Aging Interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaAgingIntervalGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Check the External Routing Capability for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 routing capability val
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaExternalRoutingCapabilityGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA type for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA type
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    This API is deprecated. Use usmDbOspfLsdbEntryGet() and
*           usmDbOspfLsdbEntryNext(). 
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSADbLSATypeGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Age for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    This API is deprecated. Use usmDbOspfLsdbEntryGet() and
*           usmDbOspfLsdbEntryNext(). 
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSADbLSAAgeGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                    L7_uint32 intIfNum, L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Options for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
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
L7_RC_t usmDbOspfAreaLSADbLSAOptionsGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                        L7_uint32 intIfNum, L7_int32 Type, L7_uint32 Lsid,
                                        L7_uint32 RouterId, L7_uchar8 *val);

/*********************************************************************
* 
* @purpose  Get the flags from an OSPF router LSA 
*          
* @param    unit      the unit for this operation
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
L7_RC_t usmDbOspfRouterLsaFlagsGet(L7_uint32 unit, L7_uint32 areaId, 
                                   L7_uint32 routerId, L7_uchar8 *rtrLsaFlags);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Checksum for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSADbLSACksumGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                      L7_uint32 intIfNum, L7_int32 Type, L7_uint32 Lsid,
                                      L7_uint32 RouterId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Advertisement packet for the
*           specified area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uchar8 LSA Advertisement Packet
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaLSADbLSAAdvertiseGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                          L7_uint32 intIfNum,
                                          L7_int32 Type, L7_uint32 Lsid,
                                          L7_uint32 RouterId, L7_char8 **buf,
                                          L7_uint32 *buf_len);

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
L7_RC_t usmDbOspfAreaEntryExists(L7_uint32 AreaId);

/*********************************************************************
*
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    *p_areaId      L7_uint32 area Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaIdGet(L7_uint32 UnitIndex, L7_uint32 *p_areaId);


/*********************************************************************
*
* @purpose  Get the Ospf Area Id for the next area, of the Ospf Router ,
*           after the area Id specified
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId L7_uint32 the areaId after for next operation
* @param    *p_areaId      L7_uint32 next area Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfAreaIdGetNext(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *p_areaId);


/*********************************************************************
*
* @purpose  Get the Ospf Router Id for the first neighbour of the
*           specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    intIfNum L7_uint32 the interface number
* @param    *p_routerId      L7_uint32 router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourRouterIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId);


/*********************************************************************
*
* @purpose  Get the Ospf Router Id for the next neighbour, after the
*           specified router Id of the neighbour, of the
*           specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *p_routerId      L7_uint32 next router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourRouterIdGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId);


/*********************************************************************
*
* @purpose  Get the Ospf IP Address of the specified neighbour router Id
*           of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourIPAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf Interface index of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourIfIndexGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf Options of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 options
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourOptionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf Area of the specified neighbour 
*           router Id of the specified interface 
*          
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 area
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourAreaGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf Dead timer remaining of the specified neighbour 
*           router Id of the specified interface 
*          
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 time in millisecs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourDeadTimerRemainingGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the number of seconds since adjacency last went to 
*           Full state with a given neighbor.  
*         
* @param    iniIfNum   the interface number
* @param    routerId   router ID of the neighbour
* @param    *val       time in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t usmDbOspfNbrUptimeGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf Priority of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf State of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf Interface State of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourIntfStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                       L7_uint32 routerId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf Events counter of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    ipAddress   ip ADdress of the neihbour
* @param    *val      L7_uint32 events counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf Events counter of all neighbour on the specified
*           interface
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    iniIfNum    L7_uint32 the interface number
* @param    *val        L7_uint32 events counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRtrIntfAllNbrEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf Permanence value of the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 permanence value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourPermanenceValGet(L7_uint32 UnitIndex,L7_uint32 intIfNum, L7_uint32 routerId,  L7_uint32 *val );


/*********************************************************************
*
* @purpose  Check if Ospf Hellos are suppressed to the specified neighbour
*           router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 Hellos Suppressed ? L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourHellosSuppressedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the current length of the retransmission queue of the
*           specified neighbour router Id of the specified interface
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 LS Retransmission Queue Length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfNeighbourLSRetransQLenGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
L7_RC_t usmDbOspfHelpfulNbrStatusGet(L7_uint32 intIfNum, L7_uint32 nbrIpAddr, 
                                     L7_ospfHelpfulNbrStatus_t *nbrStatus);


/*********************************************************************
* @purpose  Determine if the interface exists to the OSPF component
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfIntfExists(L7_uint32 UnitIndex, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Determine if the OSPF component has been initialized
*
* @param    UnitIndex  @b{(input)} the unit for this operation
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfInitialized(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Get the number of state changes or error events on this
* @purpose  interface for all virtual links
*
* @param    intIfNum    The internal interface number
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
L7_RC_t usmDbOspfRtrIntfAllVirtIfEventsGet (L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val );

/*********************************************************************
* @purpose  Restores OSPF user config file to factory defaults
*
* @param    UnitIndex L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This routine is for LVL7 internal use only.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRestore(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Allows a user to determine configured ospf trap flags
*
* @param    unitIndex Unit Index
* @param    trapFlags      bit mask indicating ospf traps that are
*                          enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfTrapFlagsGet(L7_uint32 unitIndex, L7_uint32 *trapFlags);

/*********************************************************************
* @purpose  Set configuration parameters for OSPF's redistribution
*           of routes from other sources.
*
* @param    unitIndex - the unit for this operation
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
*                      which OSPF may redistribute, or b) redistMetric is
*                      out of range.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRedistributionSet(L7_uint32 unitIndex,
                                   L7_REDIST_RT_INDICES_t sourceProto,
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
* @param    unitIndex - the unit for this operation
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
*
* @notes    Does not change any other redistribution configuration options
*           (metric, metric type, etc).
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRedistributeSet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL redistribute);

/*********************************************************************
* @purpose  Get whether OSPF redistributes from a specific
*           source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistributeGet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL *redistribute);

/*********************************************************************
* @purpose  For a given source of redistributed routes, revert the
*           redistribution configuration to the defaults.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistributeRevert(L7_uint32 unitIndex,
                                    L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Specify an access list that OSPF uses to filter routes
*           from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDistListSet(L7_uint32 unitIndex,
                             L7_REDIST_RT_INDICES_t sourceProto,
                             L7_uint32 distList);

/*********************************************************************
* @purpose  Remove a distribute list from serving as a route filter
*           for routes from a given source protocol. Call this function
*           when the user removes the distribute list option.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDistListClear(L7_uint32 unitIndex,
                               L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the number of the access list that OSPF uses to filter routes
*           from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDistListGet(L7_uint32 unitIndex,
                             L7_REDIST_RT_INDICES_t sourceProto,
                             L7_uint32 *distList);

/*********************************************************************
* @purpose  Set the metric that OSPF advetises for routes learned from
*           another protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricSet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_int32 redistMetric);

/*********************************************************************
* @purpose  Clear the redistribution metric for a given source protocol.
*           Call this function when the redistribute metric option is
*           removed from the configuration.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricClear(L7_uint32 unitIndex,
                                   L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the metric that OSPF advetises for routes learned from
*           another protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricGet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_int32 *redistMetric);

/*********************************************************************
* @purpose  Set the metric that OSPF uses when it redistributes routes
*           from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricTypeSet(L7_uint32 unitIndex,
                                     L7_REDIST_RT_INDICES_t sourceProto,
                                     L7_OSPF_EXT_METRIC_TYPES_t metType);

/*********************************************************************
* @purpose  Revert to the default the metric that OSPF uses when it
*           redistributes routes from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricTypeRevert(L7_uint32 unitIndex,
                                        L7_REDIST_RT_INDICES_t sourceProto);

/*********************************************************************
* @purpose  Get the metric that OSPF uses when it redistributes routes
*           from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistMetricTypeGet(L7_uint32 unitIndex,
                                     L7_REDIST_RT_INDICES_t sourceProto,
                                     L7_OSPF_EXT_METRIC_TYPES_t *metType);

/*********************************************************************
* @purpose  Specify the tag that OSPF uses when it advertises routes
*           learned from a given protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfTagSet(L7_uint32 unitIndex,
                        L7_REDIST_RT_INDICES_t sourceProto,
                        L7_uint32 tag);

/*********************************************************************
* @purpose  Get the tag that OSPF uses when it advertises routes
*           learned from a given protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfTagGet(L7_uint32 unitIndex,
                        L7_REDIST_RT_INDICES_t sourceProto,
                        L7_uint32 *tag);

/*********************************************************************
* @purpose  Specify whether OSPF is allowed to redistribute subnetted
*           routes from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistSubnetsSet(L7_uint32 unitIndex,
                                  L7_REDIST_RT_INDICES_t sourceProto,
                                  L7_BOOL subnets);

/*********************************************************************
* @purpose  Get whether OSPF is allowed to redistribute subnetted
*           routes from a given source protocol.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfRedistSubnetsGet(L7_uint32 unitIndex,
                                  L7_REDIST_RT_INDICES_t sourceProto,
                                  L7_BOOL *subnets);

/*********************************************************************
* @purpose  Indicates whether a sourceIndex is a valid
*           source from which OSPF may redistribute.
*
* @param    unitIndex - the unit for this operation
* @param    sourceIndex - A source for redistributed routes.
*
* @returns  L7_SUCCESS if sourceIndex corresponds to a valid
*                      source of routes for OSPF to redistribute.
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRouteRedistributeGet(L7_uint32 unitIndex,
                                      L7_REDIST_RT_INDICES_t sourceIndex);

/*********************************************************************
* @purpose  Given a route redistribution source, set nextSourceIndex
*           to the next valid source of redistributed routes.
*
* @param    unitIndex - the unit for this operation
* @param    sourceIndex - Corresponds to a source for redistributed routes.
* @param    nextSourceIndex - The source index following sourceIndex.
*
* @returns  L7_SUCCESS if there is a valid nextSourceIndex
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfRouteRedistributeGetNext(L7_uint32 unitIndex,
                                          L7_REDIST_RT_INDICES_t sourceIndex,
                                          L7_REDIST_RT_INDICES_t *nextSourceIndex);

/*********************************************************************
* @purpose  Set the default metric for OSPF.
*
* @param    unitIndex - the unit for this operation
* @param    defMetric - User configured value for the default metric.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultMetricSet(L7_uint32 unitIndex,
                                  L7_uint32 defMetric);

/*********************************************************************
* @purpose  Clear the default metric for OSPF. This function should
*           be called when the default metric becomes unconfigured.
*
* @param    unitIndex - the unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultMetricClear(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get the OSPF default metric.
*
* @param    unitIndex - the unit for this operation
* @param    defMetric - return value. Only valid if L7_SUCCESS returned.
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_NOT_EXIST if no default metric is configured.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultMetricGet(L7_uint32 unitIndex,
                                  L7_uint32 *defMetric);

/*********************************************************************
* @purpose  Set configuration parameters for origination of a default route.
*           This API is useful when the caller has a value for each of
*           the four arguments.
*
* @param    unitIndex - the unit for this operation
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
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if defRouteMetric is out of range.
*
* @notes    always, defRouteMetric, and defRouteMetType may be set
*           even if originateDefault is false. Their values will be stored,
*           but OSPF ignores the
*           values for these options unless originateDefault is true.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteSet(L7_uint32 unitIndex,
                                 L7_BOOL originateDefault,
                                 L7_BOOL always,
                                 L7_int32 defRouteMetric,
                                 L7_OSPF_EXT_METRIC_TYPES_t defRouteMetType);

/*********************************************************************
* @purpose  Specify whether OSPF should originate a default route.
*
* @param    unitIndex - the unit for this operation
* @param    originateDefault - L7_TRUE if OSPF should originate a
*                              default route. Default is L7_FALSE.
*
* @returns  L7_SUCCESS
*
* @notes    Does not change the value of other options -- always,
*           defRouteMetric, defRouteMetType
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteOrigSet(L7_uint32 unitIndex,
                                     L7_BOOL originateDefault);

/*********************************************************************
* @purpose  Revert the default route origination configuration to
*           the defaults.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDefaultRouteOrigRevert(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get whether OSPF should originate a default route.
*
* @param    unitIndex - the unit for this operation
* @param    originateDefault - return value
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteOrigGet(L7_uint32 unitIndex,
                                     L7_BOOL *originateDefault);

/*********************************************************************
* @purpose  Specify whether OSPF's origination of a default route
*           is conditioned on the presence of a default route in
*           the forwarding table.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDefaultRouteAlwaysSet(L7_uint32 unitIndex,
                                       L7_BOOL always);

/*********************************************************************
* @purpose  Get whether OSPF's origination of a default route
*           is conditioned on the presence of a default route in
*           the forwarding table.
*
* @param    unitIndex - the unit for this operation
* @param    always - L7_TRUE if OSPF should originate a default route
*                    even if the forwarding table does not contain a
*                    default route.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteAlwaysGet(L7_uint32 unitIndex,
                                       L7_BOOL *always);

/*********************************************************************
* @purpose  Specify the metric advertised for the default route.
*
* @param    unitIndex - the unit for this operation
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
L7_RC_t usmDbOspfDefaultRouteMetricSet(L7_uint32 unitIndex,
                                       L7_int32 defRouteMetric);

/*********************************************************************
* @purpose  Clear the default route metric for OSPF. Call this function
*           if the user removes the metric option for default route
*           origination.
*
* @param    unitIndex - the unit for this operation
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteMetricClear(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get the metric advertised for the default route.
*
* @param    unitIndex - the unit for this operation
* @param    defRouteMetric - Metric for default route.
*
* @returns  L7_SUCCESS if a metric is configured
* @returns  L7_NOT_EXIST if no metric is configured
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteMetricGet(L7_uint32 unitIndex,
                                       L7_int32 *defRouteMetric);

/*********************************************************************
* @purpose  Specify the metric type for advertisement of the default route.
*
* @param    unitIndex - the unit for this operation
* @param    defRouteMetType - OSPF metric type for default route.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteMetricTypeSet(L7_uint32 unitIndex,
                                           L7_OSPF_EXT_METRIC_TYPES_t
                                           defRouteMetType);

/*********************************************************************
* @purpose  Revert to the default metric type for advertisement
*           of the default route. Call this function when the user
*           removes the configuration of the metric type.
*
* @param    unitIndex - the unit for this operation
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteMetricTypeRevert(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get the metric type for advertisement of the default route.
*
* @param    unitIndex - the unit for this operation
* @param    defRouteMetType - OSPF metric type for default route.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfDefaultRouteMetricTypeGet(L7_uint32 unitIndex,
                                           L7_OSPF_EXT_METRIC_TYPES_t
                                           *defRouteMetType);

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
L7_RC_t usmDbOspfMapSelfOrigExtLsaFlush(void);

/*********************************************************************
* @purpose  Clear OSPFv2 statistics.
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
L7_RC_t usmDbOspfCountersClear(void);

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
L7_RC_t usmDbOspfStubRouterClear(void);

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
L7_RC_t usmDbOspfMapNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId);

/*********************************************************************
* @purpose  Check if an interface is valid for OSPF.
*
* @param    unitIndex - the unit for this operation
* @param    intIfNum - internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get SPF stats
*
* @param    unitIndex - the unit for this operation
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
L7_uint32 usmDbOspfSpfStatsGet(L7_uint32 unitIndex, L7_uint32 maxStats, 
                               L7_OspfSpfStats_t *spfStats);

/*********************************************************************
* @purpose  Turns on/off the displaying of Ospf packet debug info
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
L7_RC_t usmDbOspfPacketDebugTraceFlagSet(L7_BOOL flag);

/*********************************************************************
* @purpose  Returns the Ospf packet debug info
*            
* @param    
*                      
* @returns  Trace Flag
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfPacketDebugTraceFlagGet();

/*********************************************************************
* @purpose  To decode OSPF LSA's.
*
* @param    lsaInfo @b{(input)} L7_char8
* @param    output  @b{(input)} outputFuncPtr
* @param    context @b{(input)} context
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfLsaDecode(L7_char8 * lsaInfo, outputFuncPtr output,
                                 void *context);


#endif /* USMDB_OSPF_API_H */
