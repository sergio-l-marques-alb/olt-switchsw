/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   usmdb_qos_cos_api.h
*
* @purpose    Provide interface to QOS COS component APIs 
*
* @component  unitmgr
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*             
**********************************************************************/
#ifndef USMDB_QOS_COS_API_H
#define USMDB_QOS_COS_API_H

#include "l7_common.h"
#include "cos_exports.h"
#include "l7_cos_api.h"


/*********************************************************************
* @purpose  Determine if COS IP Precedence mapping per-interface cfg is allowed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An optimized helper function that checks for a specific
*           configurator COS feature.
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosMapIpPrecCfgPerIntfIsAllowed(void);

/*********************************************************************
* @purpose  Verify specified IP precedence mapping table index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    prec        @b{(input)}  IP precedence
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecIndexGet(L7_uint32 UnitIndex, L7_uint32 prec);

/*********************************************************************
* @purpose  Determine next sequential IP precedence mapping table index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    prevPrec    @b{(input)}  IP precedence to begin search
* @param    *pNext      @b{(output)} Ptr to next IP precedence
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecIndexGetNext(L7_uint32 UnitIndex, 
                                         L7_uint32 prec, 
                                         L7_uint32 *pNext);

/*************************************************************************
* @purpose  Get the assigned traffic class (queue) for this IP precedence
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecTrafficClassGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_uint32 prec, 
                                            L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP precedence
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecTrafficClassSet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_uint32 prec, 
                                            L7_uint32 val);

/*************************************************************************
* @purpose  Get default traffic class mapping for specified IP precedence value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments The default class mapping depends on the number of traffic
*           classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecDefaultTrafficClassGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 prec, 
                                                   L7_uint32 *pVal);

/*************************************************************************
* @purpose  Restore default IP precedence mappings for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecDefaultsRestore(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified IP Precedence mapping table interface index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecIntfIndexGet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential IP Precedence mapping table interface index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpPrecIntfIndexGetNext(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum, 
                                             L7_uint32 *pNext);

/*********************************************************************
* @purpose  Determine if COS IP DSCP mapping per-interface cfg is allowed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An optimized helper function that checks for a specific
*           configurator COS feature.
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosMapIpDscpCfgPerIntfIsAllowed(void);

/*********************************************************************
* @purpose  Verify specified IP DSCP mapping table index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    dscp        @b{(input)}  IP DSCP
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpIndexGet(L7_uint32 UnitIndex, L7_uint32 dscp);

/*********************************************************************
* @purpose  Determine next sequential IP DSCP mapping table index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    dscp        @b{(input)}  IP DSCP to begin search
* @param    *pNext      @b{(output)} Ptr to next IP DSCP
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpIndexGetNext(L7_uint32 UnitIndex, 
                                         L7_uint32 dscp, 
                                         L7_uint32 *pNext);

/*************************************************************************
* @purpose  Get the assigned traffic class (queue) for this IP DSCP
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpTrafficClassGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_uint32 dscp, 
                                            L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP DSCP
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpTrafficClassSet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_uint32 dscp, 
                                            L7_uint32 val);

/*************************************************************************
* @purpose  Get default traffic class mapping for specified IP DSCP value
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments The default class mapping depends on the number of traffic
*           classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpDefaultTrafficClassGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 dscp, 
                                                   L7_uint32 *pVal);

/*************************************************************************
* @purpose  Restore default IP DSCP mappings for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpDefaultsRestore(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified IP DSCP mapping table interface index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpIntfIndexGet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential IP DSCP mapping table interface index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapIpDscpIntfIndexGetNext(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum, 
                                             L7_uint32 *pNext);

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS mapping
*           table config
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosMapIntfIsValid(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if COS trust mode per-interface cfg is allowed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An optimized helper function that checks for a specific
*           configurator COS feature.
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosMapTrustModeCfgPerIntfIsAllowed(void);

/*************************************************************************
* @purpose  Get the COS trust mode for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to trust mode output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapTrustModeGet(L7_uint32 UnitIndex, 
                                   L7_uint32 intIfNum,
                                   L7_QOS_COS_MAP_INTF_MODE_t *pVal);

/*************************************************************************
* @purpose  Set the COS trust mode for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Trust mode value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapTrustModeSet(L7_uint32 UnitIndex, 
                                   L7_uint32 intIfNum,
                                   L7_QOS_COS_MAP_INTF_MODE_t val);

/*********************************************************************
* @purpose  Verify specified trust mode interface index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapTrustModeIntfIndexGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential trust mode interface index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapTrustModeIntfIndexGetNext(L7_uint32 UnitIndex, 
                                                L7_uint32 intIfNum, 
                                                L7_uint32 *pNext);

/*************************************************************************
* @purpose  Get the COS untrusted port default traffic class for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to untrusted traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosMapUntrustedPortDefaultTrafficClassGet(L7_uint32 UnitIndex,
                                                          L7_uint32 intIfNum,
                                                          L7_uint32 *pVal);

/*********************************************************************
* @purpose  Determine if COS queue configuration is allowed per-interface
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An optimized helper function that checks for a specific
*           configurator COS feature.
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosQueueCfgPerIntfIsAllowed(void);

/*********************************************************************
* @purpose  Determine if COS queue drop parms configuration is allowed
*           per-interface
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An optimized helper function that checks for a specific
*           configurator COS feature.
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosQueueDropCfgPerIntfIsAllowed(void);

/*********************************************************************
* @purpose  Verify specified queue config interface index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfIndexGet(L7_uint32 UnitIndex, 
                                     L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfIndexGetNext(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum, 
                                         L7_uint32 *pNext);

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS queue config
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosCosQueueIntfIsValid(L7_uint32 UnitIndex, 
                                    L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified queue ID index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    queueId     @b{(input)}  Queue id
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIdIndexGet(L7_uint32 UnitIndex, 
                                   L7_uint32 queueId);

/*********************************************************************
* @purpose  Determine next sequential queue ID index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    queueId     @b{(input)}  Queue id
* @param    *pNext      @b{(output)} Ptr to next queue ID
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIdIndexGetNext(L7_uint32 UnitIndex, 
                                       L7_uint32 queueId, 
                                       L7_uint32 *pNext);

/*********************************************************************
* @purpose  Verify specified queue drop precedence level index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    dropPrec    @b{(input)}  Drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDropPrecIndexGet(L7_uint32 UnitIndex, 
                                         L7_uint32 dropPrec);

/*********************************************************************
* @purpose  Determine next sequential queue drop precedence level index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    dropPrec    @b{(input)}  Drop precedence level
* @param    *pNext      @b{(output)} Ptr to next drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDropPrecIndexGetNext(L7_uint32 UnitIndex, 
                                             L7_uint32 dropPrec, 
                                             L7_uint32 *pNext);

/*************************************************************************
* @purpose  Restore default settings for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDefaultsRestore(L7_uint32 UnitIndex, 
                                        L7_uint32 intIfNum);

/*************************************************************************
* @purpose  Get the COS egress shaping rate for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to intf shaping rate output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfShapingRateGet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum, 
                                           L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the COS egress shaping rate for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Intf shaping rate value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfShapingRateSet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum, 
                                           L7_uint32 val);

/*************************************************************************
* @purpose  Get the COS interface parameters for this interface
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    intfShapingRate @b{(input)}  Interface shaping rate in kbps
* @param    intfShapingBurstSize @b{(input)}  Interface shaping burst size in kbits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfShapingStatusGet(L7_uint32 intIfNum, 
                                             L7_uint32 *intfShapingRate,
                                             L7_uint32 *intfShapingBurstSize);

/*************************************************************************
* @purpose  Get the COS interface shaping rate units
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfShapingRateUnitsGet(L7_RATE_UNIT_t *units);

/*************************************************************************
* @purpose  Get the COS queue management type for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to mgmt type output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Use the per-queue mgmt type API instead
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMgmtTypePerIntfGet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum,
                                           L7_QOS_COS_QUEUE_MGMT_TYPE_t *pVal);

/*************************************************************************
* @purpose  Set the COS queue management type for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Queue mgmt type value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Use the per-queue mgmt type API instead
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMgmtTypePerIntfSet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum,
                                           L7_QOS_COS_QUEUE_MGMT_TYPE_t val);

/*************************************************************************
* @purpose  Get the decay exponent for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to decay exponent output value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Only supported globally, not per-interface
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueWredDecayExponentGet(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum,
                                             L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the decay exponent for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Decay exponent value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Only supported globally, not per-interface
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueWredDecayExponentSet(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum,
                                             L7_uint32 val);

/*************************************************************************
* @purpose  Get the minimum bandwidth list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to min bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMinBandwidthListGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the minimum bandwidth list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to min bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMinBandwidthListSet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the minimum bandwidth for a specific queue on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue id
* @param    val         @b{(input)}  Min bandwidth value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMinBandwidthSet(L7_uint32 UnitIndex, 
                                        L7_uint32 intIfNum,
                                        L7_uint32 queueId,
                                        L7_uint32 val);

/*************************************************************************
* @purpose  Get the maximum bandwidth list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to max bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMaxBandwidthListGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the maximum bandwidth list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to max bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMaxBandwidthListSet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                                            L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the maximum bandwidth for a specific queue on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue id
* @param    val         @b{(input)}  Max bandwidth value    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMaxBandwidthSet(L7_uint32 UnitIndex, 
                                        L7_uint32 intIfNum,
                                        L7_uint32 queueId,
                                        L7_uint32 val);

/*************************************************************************
* @purpose  Get the scheduler type list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to scheduler type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueSchedulerTypeListGet(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum,
                                             L7_qosCosQueueSchedTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the scheduler type list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to scheduler type list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueSchedulerTypeListSet(L7_uint32 UnitIndex, 
                                             L7_uint32 intIfNum,
                                             L7_qosCosQueueSchedTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the scheduler type for a specific queue on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue id
* @param    val         @b{(input)}  Scheduler type value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueSchedulerTypeSet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum,
                                         L7_uint32 queueId,
                                         L7_uint32 val);

/*************************************************************************
* @purpose  Get the queue management type list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to queue mgmt type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Use the per-interface mgmt type API instead
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMgmtTypeListGet(L7_uint32 UnitIndex, 
                                        L7_uint32 intIfNum,
                                        L7_qosCosQueueMgmtTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the queue management type list for all queues on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to queue mgmt type list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Use the per-interface mgmt type API instead
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMgmtTypeListSet(L7_uint32 UnitIndex, 
                                        L7_uint32 intIfNum,
                                        L7_qosCosQueueMgmtTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the queue management type for a specific queue on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue id
* @param    val         @b{(input)}  Queue mgmt type value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueMgmtTypeSet(L7_uint32 UnitIndex, 
                                    L7_uint32 intIfNum,
                                    L7_uint32 queueId,
                                    L7_uint32 val);

/*************************************************************************
* @purpose  Get the queue taildrop / WRED config parms list for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to drop parms output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDropParmsListGet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum,
                                         L7_qosCosDropParmsList_t *pVal);

/*************************************************************************
* @purpose  Get the default COS queue config parms list 
*           for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to parms output list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDefaultConfigGet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum,
                                         L7_cosCfgParms_t *pVal);

/*************************************************************************
* @purpose  Set the queue taildrop / WRED config parms list for this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to drop parms list    
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDropParmsListSet(L7_uint32 UnitIndex, 
                                         L7_uint32 intIfNum,
                                         L7_qosCosDropParmsList_t *pVal);


/*************************************************************************
* @purpose  Restore default settings of all taildrop / WRED config parms 
*           on this interface
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number     
* @params   queueId     @b{(input)}  queue ID to de-configure
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueDropDefaultsRestore(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum, 
                                            L7_uint32 queueId);

/*************************************************************************
* @purpose  Get the number of configurable queues per port
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueNumQueuesPerPortGet(L7_uint32 UnitIndex, 
                                            L7_uint32 *pVal);

/*************************************************************************
* @purpose  Get the number of drop precedence levels supported per queue
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  This feature not supported
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueNumDropPrecLevelsGet(L7_uint32 UnitIndex, 
                                             L7_uint32 *pVal);

#endif /* USMDB_QOS_COS_API_H */
