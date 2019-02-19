
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


#ifndef USMDB_OSPFV3_API_H
#define USMDB_OSPFV3_API_H
#include "l3_commdefs.h"
#include "usmdb_mib_ospfv3_api.h"

/*********************************************************************
* @purpose  Get general OSPFv3 status information.
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
L7_RC_t usmDbOspfv3StatusGet(L7_ospfStatus_t *status);

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
L7_RC_t usmDbOspfv3AdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3AdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

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
L7_RC_t usmDbOspfv3AsbrAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3MaxPathsGet(L7_uint32 *maxPaths);

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
L7_RC_t usmDbOspfv3MaxPathsSet(L7_uint32 UnitIndex, L7_uint32 val);

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
L7_RC_t usmDbOspfv3AutoCostRefBwGet( L7_uint32 *autoCostRefBw);

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
L7_RC_t usmDbOspfv3AutoCostRefBwSet(L7_uint32 UnitIndex, L7_uint32 autoCostRefBw);

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
L7_RC_t usmDbOspfv3AsbrAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

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
L7_RC_t usmDbOspfv3IntfMtuIgnoreGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfMtuIgnoreSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_BOOL val);

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
L7_BOOL usmDbOspfv3IsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the Ospf Admin mode for the specified interface
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
L7_RC_t usmDbOspfv3IntfAdminModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAdminModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 val);

/*********************************************************************
* @purpose  Gets the Ospf Area Id for the specified interface
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
L7_RC_t usmDbOspfv3IntfAreaIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                               L7_uint32 *val);

/*********************************************************************
* @purpose  Indicates whether an OSPFv3 area has been configured for 
*           a given area ID. 
*
* @param     areaId @b{(input)}
*
* @returns  L7_TRUE if area has been configured
* @returns  L7_FALSE otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfv3AreaIsConfigured(L7_uint32 areaId);

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
*           return area x. usmDbOspfv3AreaIdGet() depends on the actual 
*           protocol state.
*           This API would be appropriate for getting information for
*           show running-config in the CLI.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaCfgGetFirst(L7_uint32 UnitIndex, L7_uint32 *areaId);

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
*           return area x. usmDbOspfv3AreaIdGetNext() and 
*           usmDbOspfv3AreaEntryNext() depend on the actual protocol state.
*           This API would be appropriate for getting information for
*           show running-config in the CLI.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaCfgGetNext(L7_uint32 UnitIndex, L7_uint32 areaId,
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
L7_RC_t usmDbOspfv3IntfAreaIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfHelloIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfHelloIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_uint32 val);

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
L7_RC_t usmDbOspfv3IntfDeadIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfDeadIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfRxmtIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfRxmtIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfNbmaPollIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfNbmaPollIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfPassiveModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfPassiveModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                      L7_BOOL passiveMode);

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
L7_RC_t usmDbOspfv3IntfTransitDelayGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfTransitDelaySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthenticationGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthenticationSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
*           to be kept hidden.  Use usmDbOspfv3IfAuthKeyGet instead.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3IntfAuthKeyGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
*           the same thing as usmDbOspfv3IntfAuthKeyGet, outputting the
*           actual auth key value.
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3IntfAuthKeyActualGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthKeySet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthKeyIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfAuthKeyIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfVirtTransitAreaIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfVirtTransitAreaIdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfVirtIntfNeighborGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3IntfVirtIntfNeighborSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
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
L7_RC_t usmDbOspfv3AbrAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the Opaque LSAs Storing Capability
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
L7_RC_t usmDbOspfv3OpaqueLSAModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Opaque LSAs Storing Capability
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    mode       @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3MapOpaqueLSAModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

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
L7_RC_t usmDbOspfv3ExitOverFlowIntervalGet(L7_uint32 UnitIndex, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3ExternalLSALimitGet(L7_uint32 UnitIndex, L7_int32 *val);


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
L7_RC_t usmDbOspfv3IntfLSAAckIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3IntfStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get OSPFv3 interface statistics 
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
L7_RC_t usmDbOspfv3IntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats);

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
L7_RC_t usmDbOspfv3IntfDrIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3IntfBackupDrIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);



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
L7_RC_t usmDbOspfv3IntfLocalLinkOpaqueLSAsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val );


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
L7_RC_t usmDbOspfv3IntfLocalLinkOpaqueLSACksumGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val );


/*********************************************************************
* @purpose  Gets the Ospf link local IP Address for the specified interface
*
* @param    intIfNum    @b{(input)} L7_uint32 interface for this operation
* @param    val         @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3IntfIPAddrGet(L7_uint32 intIfNum, L7_in6_addr_t *val);


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
L7_RC_t usmDbOspfv3IntfSubnetMaskGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3IntfLinkEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3AreaExternalRoutingCapabilityGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA type for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaLSADbLSATypeGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Age for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaLSADbLSAAgeGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                    L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA LS Id for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA LS Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaLSADbLSALSIdGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Router Id for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaLSADbLSARouterIdGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the Ospf lsa-database LSA Sequence for this area
*
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Sequence
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3AreaLSADbLSASequenceGet(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3AreaLSADbLSAOptionsGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                        L7_int32 Type, L7_uint32 Lsid,
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
L7_RC_t usmDbOspfv3RouterLsaFlagsGet(L7_uint32 unit, L7_uint32 areaId, 
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
L7_RC_t usmDbOspfv3AreaLSADbLSACksumGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                      L7_int32 Type, L7_uint32 Lsid,
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
L7_RC_t usmDbOspfv3AreaLSADbLSAAdvertiseGet(L7_uint32 UnitIndex, L7_uint32 areaId,
                                          L7_int32 Type, L7_uint32 Lsid,
                                          L7_uint32 RouterId, L7_char8 **buf,
                                          L7_uint32 *buf_len);


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
L7_RC_t usmDbOspfv3AreaIdGet(L7_uint32 UnitIndex, L7_uint32 *p_areaId);


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
L7_RC_t usmDbOspfv3AreaIdGetNext(L7_uint32 UnitIndex, L7_uint32 areaId, L7_uint32 *p_areaId);


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
L7_RC_t usmDbOspfv3NeighbourRouterIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId);


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
L7_RC_t usmDbOspfv3NeighbourRouterIdGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId);


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
L7_RC_t usmDbOspfv3NeighbourIPAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_in6_addr_t *val);


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
L7_RC_t usmDbOspfv3NeighbourIfIndexGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3NeighbourOptionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3NeighbourPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf If id of the specified neighbour 
*           router Id of the specified interface 
*          
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 if id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3NeighbourIfIdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf area of the specified neighbour 
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
L7_RC_t usmDbOspfv3NeighbourAreaGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

/*********************************************************************
* 
* @purpose  Get the Ospf dead time remaining of the specified neighbour 
*           router Id of the specified interface 
*          
* @param    UnitIndex L7_uint32 the unit for this operation
* @param    iniIfNum L7_uint32 the interface number
* @param    routerId    L7_uint32 router ID of the neighbour
* @param    *val      L7_uint32 time in msec
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t usmDbOspfv3NeighbourDeadTimerRemainingGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3NeighbourStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3NeighbourIntfStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
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
L7_RC_t usmDbOspfv3NeighbourEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3RtrIntfAllNbrEventsCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

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
L7_RC_t usmDbOspfv3NeighbourPermanenceValGet(L7_uint32 UnitIndex,L7_uint32 intIfNum, L7_uint32 routerId,  L7_uint32 *val );


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
L7_RC_t usmDbOspfv3NeighbourHellosSuppressedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);


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
L7_RC_t usmDbOspfv3NeighbourLSRetransQLenGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val);




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
L7_BOOL usmDbOspfv3IntfExists(L7_uint32 UnitIndex, L7_uint32 intIfNum);


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
L7_BOOL usmDbOspfv3Initialized(L7_uint32 UnitIndex);

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
L7_RC_t usmDbOspfv3RtrIntfAllVirtIfEventsGet (L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val );

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
L7_RC_t usmDbOspfv3Restore(L7_uint32 UnitIndex);

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
L7_RC_t usmDbOspfv3MapSelfOrigExtLsaFlush(void);

/*********************************************************************
* @purpose  Clear OSPFv3 statistics.
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
L7_RC_t usmDbOspfv3CountersClear(void);

/*********************************************************************
* @purpose  Clear OSPFv3 neighbors.
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
L7_RC_t usmDbOspfv3MapNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId);

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
L7_RC_t usmDbOspfv3TrapFlagsGet(L7_uint32 unitIndex, L7_uint32 *trapFlags);

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
L7_RC_t usmDbOspfv3RedistributionSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistributeSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistributeGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistributeRevert(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DistListSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DistListClear(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DistListGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricClear(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricTypeSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricTypeRevert(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistMetricTypeGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3TagSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3TagGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistSubnetsSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RedistSubnetsGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RouteRedistributeGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3RouteRedistributeGetNext(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultMetricSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultMetricClear(L7_uint32 unitIndex);

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
L7_RC_t usmDbOspfv3DefaultMetricGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteOrigSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteOrigRevert(L7_uint32 unitIndex);

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
L7_RC_t usmDbOspfv3DefaultRouteOrigGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteAlwaysSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteAlwaysGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteMetricSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteMetricClear(L7_uint32 unitIndex);

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
L7_RC_t usmDbOspfv3DefaultRouteMetricGet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteMetricTypeSet(L7_uint32 unitIndex,
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
L7_RC_t usmDbOspfv3DefaultRouteMetricTypeRevert(L7_uint32 unitIndex);

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
L7_RC_t usmDbOspfv3DefaultRouteMetricTypeGet(L7_uint32 unitIndex,
                                           L7_OSPF_EXT_METRIC_TYPES_t
                                           *defRouteMetType);

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
L7_RC_t usmDbOspfv3PassiveModeGet(L7_uint32 UnitIndex, L7_BOOL *passiveMode);

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
L7_RC_t usmDbOspfv3PassiveModeSet(L7_uint32 UnitIndex, L7_BOOL passiveMode);

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
L7_RC_t usmDbOspfv3LsaDecode(L7_char8 * lsaInfo, outputFuncPtr output,
                             void *context);

/*********************************************************************
* @purpose  Turns on/off the displaying of OspfV3 packet debug info
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
L7_RC_t usmDbOspfv3PacketDebugTraceFlagSet(L7_BOOL flag);

/*********************************************************************
* @purpose  Returns the OspfV3 packet debug info
*            
* @param    
*                      
* @returns  Trace Flag
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL usmDbOspfv3PacketDebugTraceFlagGet();
#endif /* USMDB_OSPFV3_API_H */
