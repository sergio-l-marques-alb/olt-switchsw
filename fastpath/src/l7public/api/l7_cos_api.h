/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   l7_cos_api.h
*
* @purpose    COS component API functions
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*             
**********************************************************************/
#ifndef L7_COS_API_H
#define L7_COS_API_H

#include "l7_common.h"
#include "acl_api.h"
#include "cos_exports.h"
#include "cos_exports.h"
#include "defaultconfig.h"

/* the following values are established here if not already defined in platform.h */
#ifndef L7_MAX_CFG_QUEUES_PER_PORT
  #define L7_MAX_CFG_QUEUES_PER_PORT            1
#endif
#ifndef L7_MAX_CFG_DROP_PREC_LEVELS
  #define L7_MAX_CFG_DROP_PREC_LEVELS           1
#endif

#define L7_QOS_COS_QUEUE_ID_MIN                 0
#define L7_QOS_COS_QUEUE_ID_MAX                 (L7_MAX_CFG_QUEUES_PER_PORT-1)
#define L7_QOS_COS_QUEUE_ID_ALL                 (L7_MAX_CFG_QUEUES_PER_PORT)


#define L7_QOS_COS_DROP_PREC_LEVEL_MIN          1
#define L7_QOS_COS_DROP_PREC_LEVEL_MAX          L7_MAX_CFG_DROP_PREC_LEVELS


/********************************************
*
*  Common data types used with COS component
*
*********************************************/

/* NOTE:  The following list definitions are suited for use with USMDB APIs */

/* QOS COS queue bandwidth configuration parameter list */
typedef struct L7_qosCosQueueBwList_s
{
  L7_uint32           bandwidth[L7_MAX_CFG_QUEUES_PER_PORT];
} L7_qosCosQueueBwList_t;

/* QOS COS queue scheduler type configuration parameter list */
typedef struct L7_qosCosQueueSchedTypeList_s
{
  L7_QOS_COS_QUEUE_SCHED_TYPE_t   schedType[L7_MAX_CFG_QUEUES_PER_PORT];
} L7_qosCosQueueSchedTypeList_t;

/* QOS COS queue management type configuration parameter list */
typedef struct L7_qosCosQueueMgmtTypeList_s
{
  L7_QOS_COS_QUEUE_MGMT_TYPE_t    mgmtType[L7_MAX_CFG_QUEUES_PER_PORT];
} L7_qosCosQueueMgmtTypeList_t;

/* QOS COS drop per-precedence parameters, last precedence (+1) is for non-TCP traffic */
typedef struct L7_qosCosDropParmsPerQueue_s
{
    L7_QOS_COS_QUEUE_MGMT_TYPE_t    mgmtType;
    L7_uchar8                       minThreshold[L7_MAX_CFG_DROP_PREC_LEVELS+1]; /* WRED thresh, percentage (0-100) */
    L7_uchar8                       wredMaxThreshold[L7_MAX_CFG_DROP_PREC_LEVELS+1]; /* WRED thresh, percentage (0-100) */
    L7_uchar8                       dropProb[L7_MAX_CFG_DROP_PREC_LEVELS+1];     /* percentage (0-100) */
    L7_uchar8                       tailDropMaxThreshold[L7_MAX_CFG_DROP_PREC_LEVELS+1]; /* percentage (0-100) */
} L7_qosCosDropParmsPerQueue_t;

/* QOS COS drop per-precedence per-queue parameters */
typedef struct L7_qosCosDropParmsList_s
{
  L7_qosCosDropParmsPerQueue_t queue[L7_MAX_CFG_QUEUES_PER_PORT];
} L7_qosCosDropParmsList_t;

/* these set mask types are used to isolate individual changes in a list of values */

/* QOS COS queue-based set masks */
typedef struct L7_qosCosQueueListMask_s
{
  L7_BOOL             setMask[L7_MAX_CFG_QUEUES_PER_PORT];
} L7_qosCosQueueListMask_t;

/* QOS COS drop-precedence-based set masks */
typedef struct L7_qosCosDropPrecListMask_s
{
  L7_BOOL             setMask[L7_MAX_CFG_DROP_PREC_LEVELS+1];
} L7_qosCosDropPrecListMask_t;



/*********************************************************************
* @purpose  Verify specified IP precedence mapping table index exists
*
* @param    prec        @b{(input)}  IP precedence
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecIndexGet(L7_uint32 prec);

/*********************************************************************
* @purpose  Determine next sequential IP precedence mapping table index
*
* @param    prec        @b{(input)}  IP precedence to begin search
* @param    *pNext      @b{(output)} Ptr to next IP precedence
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecIndexGetNext(L7_uint32 prec, L7_uint32 *pNext);

/*************************************************************************
* @purpose  Get the assigned traffic class (queue) for this IP precedence
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecTrafficClassGet(L7_uint32 intIfNum, L7_uint32 prec,
                                    L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP precedence
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecTrafficClassSet(L7_uint32 intIfNum, L7_uint32 prec,
                                    L7_uint32 val);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP precedence
*           globally on all interfaces
*
* @param    prec        @b{(input)}  IP precedence     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecTrafficClassGlobalSet(L7_uint32 prec, L7_uint32 val);

/*************************************************************************
* @purpose  Get default traffic class mapping for specified IP precedence value
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    prec        @b{(input)}  IP precedence     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments The default class mapping depends on the number of traffic
*           classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecDefaultTrafficClassGet(L7_uint32 intIfNum, L7_uint32 prec, 
                                           L7_uint32 *pVal);

/*************************************************************************
* @purpose  Restore default IP precedence mappings for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecDefaultsRestore(L7_uint32 intIfNum);

/*************************************************************************
* @purpose  Restore default IP precedence mappings globally for all interfaces
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cosMapIpPrecDefaultsGlobalRestore(void);

/*********************************************************************
* @purpose  Verify specified IP DSCP mapping table index exists
*
* @param    dscp        @b{(input)}  IP DSCP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpIndexGet(L7_uint32 dscp);

/*********************************************************************
* @purpose  Determine next sequential IP DSCP mapping table index
*
* @param    dscp        @b{(input)}  IP DSCP to begin search
* @param    *pNext      @b{(output)} Ptr to next IP DSCP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpIndexGetNext(L7_uint32 dscp, L7_uint32 *pNext);

/*************************************************************************
* @purpose  Get the assigned traffic class (queue) for this IP DSCP
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpTrafficClassGet(L7_uint32 intIfNum, L7_uint32 dscp, 
                                    L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP DSCP
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpTrafficClassSet(L7_uint32 intIfNum, L7_uint32 dscp, 
                                    L7_uint32 val);

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP DSCP
*           globally for all interfaces
*
* @param    dscp        @b{(input)}  IP DSCP     
* @param    val         @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpTrafficClassGlobalSet(L7_uint32 dscp, L7_uint32 val);

/*************************************************************************
* @purpose  Get default traffic class mapping for specified IP DSCP value
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    dscp        @b{(input)}  IP DSCP     
* @param    *pVal       @b{(output)} Ptr to traffic class output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments The default class mapping depends on the number of traffic
*           classes supported in the system.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpDefaultTrafficClassGet(L7_uint32 intIfNum, L7_uint32 dscp, 
                                           L7_uint32 *pVal);

/*************************************************************************
* @purpose  Restore default IP DSCP mappings for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpDefaultsRestore(L7_uint32 intIfNum);

/*************************************************************************
* @purpose  Restore default IP DSCP mappings globally for all interfaces
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cosMapIpDscpDefaultsGlobalRestore(void);

/*********************************************************************
* @purpose  Verify specified mapping table interface index exists
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfIndexGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential mapping table interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext);

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS mapping
*           table config
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
L7_BOOL cosMapIntfIsValid(L7_uint32 intIfNum);

/*************************************************************************
* @purpose  Get the COS trust mode for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to trust mode output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfTrustModeGet(L7_uint32 intIfNum, 
                               L7_QOS_COS_MAP_INTF_MODE_t *pVal);

/*************************************************************************
* @purpose  Set the COS trust mode for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Trust mode value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfTrustModeSet(L7_uint32 intIfNum, 
                               L7_QOS_COS_MAP_INTF_MODE_t val);

/*************************************************************************
* @purpose  Set the COS trust mode globally for all interfaces
*
* @param    val         @b{(input)}  Trust mode value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosMapIntfTrustModeGlobalSet(L7_QOS_COS_MAP_INTF_MODE_t val);

/*************************************************************************
* @purpose  Get the COS untrusted port default traffic class for this interface
*
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
L7_RC_t cosMapUntrustedPortDefaultTrafficClassGet(L7_uint32 intIfNum, 
                                                  L7_uint32 *pVal);

/*********************************************************************
* @purpose  Handle update to port default priority
*
* @param    intIfNum                @b{(input)}  Internal interface number
* @param    portDefaultPriority     @b{(input)}  Port default priority
* @param    portDefaultTrafficClass @b{(input)}  Port default traffic class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is sourced as an outcall from the dot1p component
*           whenever the default user priority changes for the specified
*           interface (or globally).  This is also called by dot1p when
*           the corresponding traffic class value in the dot1p table 
*           changes, since that affects the queue to which the default
*           priority traffic is directed.
*
* @comments A change in the default port priority traffic class requires
*           the COS mapping tables to be reissued when in any interface
*           trust mode other than 'trust dot1p' so that the non-trusted
*           traffic is restricted to the default port priority queue.
*
* @end
*********************************************************************/
void cosMapPortDefaultPriorityUpdate(L7_uint32 intIfNum, 
                                     L7_uint32 portDefaultPriority,
                                     L7_uint32 portDefaultTrafficClass);

/*********************************************************************
* @purpose  Handle update to number of operational traffic classes
*
* @param    intIfNum          @b{(input)}  Internal interface number
* @param    numTrafficClasses @b{(input)}  Number of traffic classes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is sourced as an outcall from the dot1p component
*           whenever the operational number of traffic classes is 
*           changed for the specified interface (or globally).  This
*           value is less than or equal to the L7_DOT1P_NUM_TRAFFIC_CLASSES
*           defined in platform.h.  It only restricts the COS priority
*           queue mapping table entry values (it does not limit the 
*           physical number of queues that the user can configure via
*           the COS queue parameters).
*
* @comments A change in the number of traffic classes effectively causes
*           the interface COS mapping tables to be reset to the new set
*           of defaults per the corresponding cosMapTableInit[] row.
*
* @end
*********************************************************************/
void cosMapNumTrafficClassesUpdate(L7_uint32 intIfNum, 
                                   L7_uint32 numTrafficClasses);

/*********************************************************************
* @purpose  Check if 802.1p user priority mapping is active
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This is sourced as an outcall from the dot1p component
*           so it can determine whether the 802.1p user priority is
*           trusted for mapping to COS queues.  The only time 802.1p
*           is considered not trusted is when an active COS component
*           says so.
*
* @end
*********************************************************************/
L7_BOOL cosMapDot1pMappingIsActive(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified queue config interface index exists
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfIndexGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext);

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS queue
*           config
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
L7_BOOL cosQueueIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Verify specified queue ID index exists
*
* @param    queueId     @b{(input)}  Queue id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIdIndexGet(L7_uint32 queueId);

/*********************************************************************
* @purpose  Determine next sequential queue ID index
*
* @param    queueId     @b{(input)}  Queue id
* @param    *pNext      @b{(output)} Ptr to next queue ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIdIndexGetNext(L7_uint32 queueId, L7_uint32 *pNext);

/*********************************************************************
* @purpose  Verify specified queue drop precedence level index exists
*
* @param    dropPrec    @b{(input)}  Drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropPrecIndexGet(L7_uint32 dropPrec);

/*********************************************************************
* @purpose  Determine next sequential queue drop precedence level index
*
* @param    dropPrec    @b{(input)}  Drop precedence level
* @param    *pNext      @b{(output)} Ptr to next drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropPrecIndexGetNext(L7_uint32 dropPrec, L7_uint32 *pNext);

/*************************************************************************
* @purpose  Restore default settings for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDefaultsRestore(L7_uint32 intIfNum);

/*************************************************************************
* @purpose  Restore default settings for all queues globally on all interfaces
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cosQueueDefaultsGlobalRestore(void);

/*************************************************************************
* @purpose  Get the COS egress shaping rate for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to intf shaping rate output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateGet(L7_uint32 intIfNum, L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the COS egress shaping rate for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Intf shaping rate value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateSet(L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t cosQueueIntfShapingStatusGet(L7_uint32 intIfNum, 
                                     L7_uint32 *intfShapingRate,
                                     L7_uint32 *intfShapingBurstSize);

/*************************************************************************
* @purpose  Set the COS egress shaping rate globally for all interfaces
*
* @param    val         @b{(input)}  Intf shaping rate value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateGlobalSet(L7_uint32 val);

/*************************************************************************
* @purpose  Get the COS queue management type for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to mgmt type output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfGet(L7_uint32 intIfNum, 
                                   L7_QOS_COS_QUEUE_MGMT_TYPE_t *pVal);

/*************************************************************************
* @purpose  Set the COS queue management type for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Queue mgmt type value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfSet(L7_uint32 intIfNum, 
                                   L7_QOS_COS_QUEUE_MGMT_TYPE_t val);

/*************************************************************************
* @purpose  Set the COS queue management type globally for all interfaces
*
* @param    val         @b{(input)}  Queue mgmt type value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfGlobalSet(L7_QOS_COS_QUEUE_MGMT_TYPE_t val);

/*************************************************************************
* @purpose  Get the WRED decay exponent for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to decay exponent output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentGet(L7_uint32 intIfNum, L7_uint32 *pVal);

/*************************************************************************
* @purpose  Set the WRED decay exponent for this interface
*
* @param    intIfNum    @b{(input)} Internal interface number     
* @param    val         @b{(input)} Decay exponent value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentSet(L7_uint32 intIfNum, L7_uint32 val);

/*************************************************************************
* @purpose  Set the WRED decay exponent globally for all interfaces
*
* @param    val         @b{(input)} Decay exponent value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentGlobalSet(L7_uint32 val);

/*************************************************************************
* @purpose  Get the minimum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to min bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthListGet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the minimum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to min bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthListSet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the minimum bandwidth list for all queues globally on all
*           interfaces
*
* @param    *pVal       @b{(input)}  Ptr to min bandwidth list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthGlobalListSet(L7_qosCosQueueBwList_t *pVal, 
                                          L7_qosCosQueueListMask_t *pListMask);

/*************************************************************************
* @purpose  Get the maximum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to max bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthListGet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the maximum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to max bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthListSet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal);

/*************************************************************************
* @purpose  Set the maximum bandwidth list for all queues globally on all
*           interfaces
*
* @param    *pVal       @b{(input)}  Ptr to max bandwidth list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthGlobalListSet(L7_qosCosQueueBwList_t *pVal,
                                          L7_qosCosQueueListMask_t *pListMask);

/*************************************************************************
* @purpose  Get the scheduler type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to scheduler type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeListGet(L7_uint32 intIfNum,
                                     L7_qosCosQueueSchedTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the scheduler type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to scheduler type list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeListSet(L7_uint32 intIfNum,
                                     L7_qosCosQueueSchedTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the scheduler type list for all queues globally on all
*           interfaces
*
* @param    *pVal       @b{(input)}  Ptr to scheduler type list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeGlobalListSet(L7_qosCosQueueSchedTypeList_t *pVal,
                                           L7_qosCosQueueListMask_t *pListMask);

/*************************************************************************
* @purpose  Get the queue management type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to queue mgmt type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeListGet(L7_uint32 intIfNum,
                                L7_qosCosQueueMgmtTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the queue management type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to queue mgmt type list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeListSet(L7_uint32 intIfNum,
                                L7_qosCosQueueMgmtTypeList_t *pVal);

/*************************************************************************
* @purpose  Set the queue management type list for all queues globally on 
*           all interfaces
*
* @param    *pVal       @b{(input)}  Ptr to queue mgmt type list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeGlobalListSet(L7_qosCosQueueMgmtTypeList_t *pVal,
                                      L7_qosCosQueueListMask_t *pListMask);

/*************************************************************************
* @purpose  Get the queue WRED / taildrop config parms list for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to drop parms output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsListGet(L7_uint32 intIfNum,
                                 L7_qosCosDropParmsList_t *pVal);

/*************************************************************************
* @purpose  Set the queue WRED / taildrop config parms list for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to drop parms list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsListSet(L7_uint32 intIfNum, 
                                 L7_qosCosDropParmsList_t *pVal);

/*************************************************************************
* @purpose  Set the queue WRED / taildrop config parms list globally for all interfaces
*
* @param    *pVal       @b{(input)}  Ptr to drop parms list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsGlobalListSet(L7_qosCosDropParmsList_t *pVal);

/*************************************************************************
* @purpose  Restore default settings of all WRED / taildrop 
*           config parms on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue ID to de-configure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropDefaultsRestore(L7_uint32 intIfNum, L7_uint32 queueId);

/*************************************************************************
* @purpose  Get the number of configurable queues per port
*
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueNumQueuesPerPortGet(L7_uint32 *pVal);

/*************************************************************************
* @purpose  Get the number of drop precedence levels supported per queue
*
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueNumDropPrecLevelsGet(L7_uint32 *pVal);

#endif /* L7_COS_API_H */
