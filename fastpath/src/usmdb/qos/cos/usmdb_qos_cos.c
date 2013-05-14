/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_qos_cos.c
* @purpose     USMDB COS API functions
* @component   cos
* @comments    none
* @create      03/12/2004
* @author      gpaussa
* @end
*             
**********************************************************************/


#include <string.h>
#include "l7_common.h"
#include "cnfgr.h"
#include "usmdb_qos_cos_api.h"
#include "usmdb_util_api.h"
#include "l7_cos_api.h"
#include "dot1q_exports.h"
#include "cos_util.h"

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
L7_BOOL usmDbQosCosMapIpPrecCfgPerIntfIsAllowed(void)
{
  static L7_BOOL  intfCfgSupp = L7_FALSE;
  static L7_BOOL  valueIsSet = L7_FALSE;

  /* this is just a performance optimization...otherwise would be calling 
   * the configurator each time for a value that doesn't change after
   * initialization
   */
  if (valueIsSet == L7_FALSE)
  {
    /* check if device allows COS config to be specified per-interface */
    intfCfgSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                        L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID);
    valueIsSet = L7_TRUE;
  }
  return intfCfgSupp;
}

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
L7_RC_t usmDbQosCosMapIpPrecIndexGet(L7_uint32 UnitIndex, L7_uint32 prec)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIpPrecIndexGet(prec);
}

/*********************************************************************
* @purpose  Determine next sequential IP precedence mapping table index
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    prec        @b{(input)}  IP precedence to begin search
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
                                         L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIpPrecIndexGetNext(prec, pNext);
}

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
                                            L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapIpPrecCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapIpPrecTrafficClassGet(intIfNum, prec, pVal);
}

/*************************************************************************
* @purpose  To know COS  queue scheduler configuration supported global
*           or on this interface
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
L7_RC_t usmDbQosCosQueueSchedulerConfigSupportOnIntfGet( L7_uint32 UnitIndex, 
                                                          L7_uint32 intIfNum)
{
  
  if( intIfNum == L7_ALL_INTERFACES)
  {
    if( cnfgrIsFeaturePresent( L7_FLEX_QOS_COS_COMPONENT_ID ,
                            L7_COS_QUEUE_SCHED_TYPE_GLOBAL_FEATURE_ID)!= L7_TRUE )
    {
      return L7_FALSE;
    }
  }
  else
  {
    if( cnfgrIsFeaturePresent( L7_FLEX_QOS_COS_COMPONENT_ID ,
                          L7_COS_QUEUE_SCHED_TYPE_PER_INTF_FEATURE_ID)!=L7_TRUE)
    {
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}

/*************************************************************************
* @purpose  To know per  COS  queue scheduler configuration 
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
L7_RC_t usmDbQosCosQueueSchedulerConfigSupportOnCosQueueGet( L7_uint32 UnitIndex, 
                                                          L7_uint32 cosQueueNum)
{
  
  if( cnfgrIsFeaturePresent( L7_FLEX_QOS_COS_COMPONENT_ID ,
                  L7_COS_QUEUE_SCHED_TYPE_PER_COS_QUEUE_GLOBAL_ONLY_FEATURE_ID)!= L7_TRUE )
  { 
    return L7_TRUE;
  }
  else
  {
    if( cosQueueNum == L7_QOS_COS_QUEUE_ID_ALL )
    {
      return L7_TRUE;
    }
    else
    {
      return L7_FALSE;
    }

  }
}

/*************************************************************************
* @purpose  To know if global config  cos queues  only is supported 
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
L7_RC_t usmDbQosCosQueueSchedulerGlobalOnlyFeatureGet( L7_uint32 UnitIndex) 
{
  
  return ( cnfgrIsFeaturePresent( L7_FLEX_QOS_COS_COMPONENT_ID ,
                  L7_COS_QUEUE_SCHED_TYPE_PER_COS_QUEUE_GLOBAL_ONLY_FEATURE_ID));
}



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
                                            L7_uint32 val)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosMapIpPrecTrafficClassGlobalSet(prec, val);

  else if (usmDbQosCosMapIpPrecCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosMapIpPrecTrafficClassSet(intIfNum, prec, val);

  return rc;
}

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
                                                   L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapIpPrecCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapIpPrecDefaultTrafficClassGet(intIfNum, prec, pVal);
}

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
                                            L7_uint32 intIfNum)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosMapIpPrecDefaultsGlobalRestore();

  else if (usmDbQosCosMapIpPrecCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosMapIpPrecDefaultsRestore(intIfNum);

  return rc;
}

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
                                         L7_uint32 intIfNum)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGet(intIfNum);
}

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
                                             L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGetNext(intIfNum, pNext);
}

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
L7_BOOL usmDbQosCosMapIpDscpCfgPerIntfIsAllowed(void)
{
  static L7_BOOL  intfCfgSupp = L7_FALSE;
  static L7_BOOL  valueIsSet = L7_FALSE;

  /* this is just a performance optimization...otherwise would be calling 
   * the configurator each time for a value that doesn't change after
   * initialization
   */
  if (valueIsSet == L7_FALSE)
  {
    /* check if device allows COS config to be specified per-interface */
    intfCfgSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                        L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID);
    valueIsSet = L7_TRUE;
  }
  return intfCfgSupp;
}

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
L7_RC_t usmDbQosCosMapIpDscpIndexGet(L7_uint32 UnitIndex, L7_uint32 dscp)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIpDscpIndexGet(dscp);
}

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
                                         L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIpDscpIndexGetNext(dscp, pNext);
}

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
                                            L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapIpDscpCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapIpDscpTrafficClassGet(intIfNum, dscp, pVal);
}

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
                                            L7_uint32 val)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosMapIpDscpTrafficClassGlobalSet(dscp, val);

  else if (usmDbQosCosMapIpDscpCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosMapIpDscpTrafficClassSet(intIfNum, dscp, val);

  return rc;
}

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
                                                   L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapIpDscpCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapIpDscpDefaultTrafficClassGet(intIfNum, dscp, pVal);
}

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
                                            L7_uint32 intIfNum)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosMapIpDscpDefaultsGlobalRestore();

  else if (usmDbQosCosMapIpDscpCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosMapIpDscpDefaultsRestore(intIfNum);

  return rc;
}

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
                                         L7_uint32 intIfNum)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGet(intIfNum);
}

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
                                             L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGetNext(intIfNum, pNext);
}

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
L7_BOOL usmDbQosCosMapIntfIsValid(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return cosMapIntfIsValid(intIfNum);
}

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
L7_BOOL usmDbQosCosMapTrustModeCfgPerIntfIsAllowed(void)
{
  static L7_BOOL  intfCfgSupp = L7_FALSE;
  static L7_BOOL  valueIsSet = L7_FALSE;

  /* this is just a performance optimization...otherwise would be calling 
   * the configurator each time for a value that doesn't change after
   * initialization
   */
  if (valueIsSet == L7_FALSE)
  {
    /* check if device allows COS config to be specified per-interface */
    intfCfgSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                        L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID);
    valueIsSet = L7_TRUE;
  }
  return intfCfgSupp;
}

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
                                   L7_QOS_COS_MAP_INTF_MODE_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_TRUST_MODE_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapTrustModeCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapIntfTrustModeGet(intIfNum, pVal);
}

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
                                   L7_QOS_COS_MAP_INTF_MODE_t val)
{
  L7_RC_t       rc;
  L7_uint32     compId, featureId;

  /* check proposed value */
  compId = L7_FLEX_QOS_COS_COMPONENT_ID;
  featureId = L7_COS_FEATURE_SUPPORTED;
  switch (val)
  {
  case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    compId = L7_DOT1P_COMPONENT_ID;
    featureId = L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID;
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    featureId = L7_COS_MAP_IPPREC_FEATURE_ID;
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    featureId = L7_COS_MAP_IPDSCP_FEATURE_ID;
    break;
  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }
  if (cnfgrIsFeaturePresent(compId, featureId) != L7_TRUE) 
    return L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_TRUST_MODE_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosMapIntfTrustModeGlobalSet(val);
  else if (usmDbQosCosMapTrustModeCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosMapIntfTrustModeSet(intIfNum, val);
  else
    rc = L7_NOT_SUPPORTED;

  return rc;
}

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
                                            L7_uint32 intIfNum)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGet(intIfNum);
}

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
                                                L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosMapIntfIndexGetNext(intIfNum, pNext);
}

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
                                                          L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_MAP_TRUST_MODE_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosMapTrustModeCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosMapUntrustedPortDefaultTrafficClassGet(intIfNum, pVal);
}

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
L7_BOOL usmDbQosCosQueueCfgPerIntfIsAllowed(void)
{
  static L7_BOOL  intfCfgSupp = L7_FALSE;
  static L7_BOOL  valueIsSet = L7_FALSE;

  /* this is just a performance optimization...otherwise would be calling 
   * the configurator each time for a value that doesn't change after
   * initialization
   */
  if (valueIsSet == L7_FALSE)
  {
    /* some devices do not allow COS config to be specified per-interface */
    intfCfgSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                        L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID);
    valueIsSet = L7_TRUE;
  }
  return intfCfgSupp;
}

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
L7_BOOL usmDbQosCosQueueDropCfgPerIntfIsAllowed(void)
{
  static L7_BOOL  intfCfgSupp = L7_FALSE;
  static L7_BOOL  valueIsSet = L7_FALSE;

  /* this is just a performance optimization...otherwise would be calling 
   * the configurator each time for a value that doesn't change after
   * initialization
   */
  if (valueIsSet == L7_FALSE)
  {
    /* some devices do not allow COS drop parms config to be specified 
     * per-interface
     */
    intfCfgSupp = cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                        L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID);
    valueIsSet = L7_TRUE;
  }
  return intfCfgSupp;
}

/*********************************************************************
* @purpose  Verify specified queue config interface index exists
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfIndexGet(L7_uint32 UnitIndex, 
                                     L7_uint32 intIfNum)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueIntfIndexGet(intIfNum);
}

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
                                         L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueIntfIndexGetNext(intIfNum, pNext);
}

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
                                    L7_uint32 intIfNum)
{
  return cosQueueIntfIsValid(intIfNum);
}

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
                                   L7_uint32 queueId)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueIdIndexGet(queueId);
}

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
                                       L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueIdIndexGetNext(queueId, pNext);
}

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
                                         L7_uint32 dropPrec)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueDropPrecIndexGet(dropPrec);
}

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
                                             L7_uint32 *pNext)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueDropPrecIndexGetNext(dropPrec, pNext);
}

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
                                        L7_uint32 intIfNum)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosQueueDefaultsGlobalRestore();

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosQueueDefaultsRestore(intIfNum);

  return rc;
}

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
                                           L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueIntfShapingRateGet(intIfNum, pVal);
}

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
                                           L7_uint32 val)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosQueueIntfShapingRateGlobalSet(val);

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosQueueIntfShapingRateSet(intIfNum, val);

  return rc;
}

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
                                             L7_uint32 *intfShapingBurstSize)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return(cosQueueIntfShapingStatusGet(intIfNum, intfShapingRate, intfShapingBurstSize));
}

/*************************************************************************
* @purpose  Get the COS interface shaping rate units
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosCosQueueIntfShapingRateUnitsGet(L7_RATE_UNIT_t *units)
{
  if (units != L7_NULLPTR)
  {
    *units = L7_QOS_COS_INTF_SHAPING_RATE_UNITS;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

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
                                           L7_QOS_COS_QUEUE_MGMT_TYPE_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueMgmtTypePerIntfGet(intIfNum, pVal);
}

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
                                           L7_QOS_COS_QUEUE_MGMT_TYPE_t val)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) != L7_TRUE) 
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosQueueMgmtTypePerIntfGlobalSet(val);

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosQueueMgmtTypePerIntfSet(intIfNum, val);

  return rc;
}

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
                                             L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE)
      return L7_NOT_SUPPORTED;

    /* some devices allow WRED decay exponent to be set only system-wide */
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                              L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID)
        == L7_TRUE)
      return L7_NOT_SUPPORTED;
  }

  return cosQueueWredDecayExponentGet(intIfNum, pVal);
}

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
                                             L7_uint32 val)
{
  L7_RC_t       rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  /* some devices allow WRED decay exponent to be set only system-wide
   *
   * NOTE:  This is an additional consideration beyond whether any COS
   *        queue parm can be configured per interface
   */
  if ((intIfNum != L7_ALL_INTERFACES) &&
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                             L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID)
      == L7_TRUE))
  {
    return L7_NOT_SUPPORTED;
  }

  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosQueueWredDecayExponentGlobalSet(val);

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
    rc = cosQueueWredDecayExponentSet(intIfNum, val);

  return rc;
}

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
                                            L7_qosCosQueueBwList_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MIN_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueMinBandwidthListGet(intIfNum, pVal);
}

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
                                            L7_qosCosQueueBwList_t *pVal)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 i;
  L7_qosCosQueueListMask_t  listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MIN_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* indicate all list values are being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_TRUE;

    rc = cosQueueMinBandwidthGlobalListSet(pVal, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMinBandwidthListSet(intIfNum, pVal);
  }

  return rc;
}

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
                                        L7_uint32 val)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 listIndex = queueId - L7_QOS_COS_QUEUE_ID_MIN;
  L7_uint32                 i;
  L7_qosCosQueueBwList_t    list;
  L7_qosCosQueueListMask_t  listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MIN_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cosQueueMinBandwidthListGet(intIfNum, &list) != L7_SUCCESS)
    return L7_FAILURE;

  /* update the individual list item value */
  list.bandwidth[listIndex] = val;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* initialize list setMask to no values being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_FALSE;

    /* indicate specific list item being set */
    listMask.setMask[listIndex] = L7_TRUE;

    rc = cosQueueMinBandwidthGlobalListSet(&list, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMinBandwidthListSet(intIfNum, &list);
  }

  return rc;
}

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
                                            L7_qosCosQueueBwList_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MAX_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueMaxBandwidthListGet(intIfNum, pVal);
}

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
                                            L7_qosCosQueueBwList_t *pVal)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 i;
  L7_qosCosQueueListMask_t  listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MAX_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* indicate all list values are being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_TRUE;

    rc = cosQueueMaxBandwidthGlobalListSet(pVal, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMaxBandwidthListSet(intIfNum, pVal);
  }

  return rc;
}

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
                                        L7_uint32 val)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 listIndex = queueId - L7_QOS_COS_QUEUE_ID_MIN;
  L7_uint32                 i;
  L7_qosCosQueueBwList_t    list;
  L7_qosCosQueueListMask_t  listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MAX_BW_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cosQueueMaxBandwidthListGet(intIfNum, &list) != L7_SUCCESS)
    return L7_FAILURE;

  /* update the individual list item value */
  list.bandwidth[listIndex] = val;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* initialize list setMask to no values being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_FALSE;

    /* indicate specific list item being set */
    listMask.setMask[listIndex] = L7_TRUE;

    rc = cosQueueMaxBandwidthGlobalListSet(&list, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMaxBandwidthListSet(intIfNum, &list);
  }

  return rc;
}

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
                                             L7_qosCosQueueSchedTypeList_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueSchedulerTypeListGet(intIfNum, pVal);
}

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
                                             L7_qosCosQueueSchedTypeList_t *pVal)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 i;
  L7_qosCosQueueListMask_t  listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* indicate all list values are being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_TRUE;

    rc = cosQueueSchedulerTypeGlobalListSet(pVal, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueSchedulerTypeListSet(intIfNum, pVal);
  }

  return rc;
}

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
                                         L7_uint32 val)
{
  L7_RC_t                       rc = L7_NOT_SUPPORTED;
  L7_uint32                     listIndex = queueId - L7_QOS_COS_QUEUE_ID_MIN;
  L7_uint32                     i;
  L7_qosCosQueueSchedTypeList_t list;
  L7_qosCosQueueListMask_t      listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cosQueueSchedulerTypeListGet(intIfNum, &list) != L7_SUCCESS)
    return L7_FAILURE;

  /* update the individual list item value */
  list.schedType[listIndex] = val;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* initialize list setMask to no values being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_FALSE;

    /* indicate specific list item being set */
    listMask.setMask[listIndex] = L7_TRUE;

    rc = cosQueueSchedulerTypeGlobalListSet(&list, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueSchedulerTypeListSet(intIfNum, &list);
  }

  return rc;
}

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
                                        L7_qosCosQueueMgmtTypeList_t *pVal)
{
  /* if device allows only per-interface config, then per-queue config is
   * not supported
   */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
    return L7_NOT_SUPPORTED;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueMgmtTypeListGet(intIfNum, pVal);
}

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
                                        L7_qosCosQueueMgmtTypeList_t *pVal)
{
  L7_RC_t                   rc = L7_NOT_SUPPORTED;
  L7_uint32                 i;
  L7_qosCosQueueListMask_t  listMask;

  /* if device allows only per-interface config, then per-queue config is
   * not supported
   */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* indicate all list values are being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_TRUE;

    rc = cosQueueMgmtTypeGlobalListSet(pVal, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMgmtTypeListSet(intIfNum, pVal);
  }

  return rc;
}

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
                                    L7_uint32 val)
{
  L7_RC_t                       rc = L7_NOT_SUPPORTED;
  L7_uint32                     listIndex = queueId - L7_QOS_COS_QUEUE_ID_MIN;
  L7_uint32                     i;
  L7_qosCosQueueMgmtTypeList_t  list;
  L7_qosCosQueueListMask_t      listMask;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (cosQueueMgmtTypeListGet(intIfNum, &list) != L7_SUCCESS)
    return L7_FAILURE;

  /* update the individual list item value */
  list.mgmtType[listIndex] = val;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    /* initialize list setMask to no values being set */
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
      listMask.setMask[i] = L7_FALSE;

    /* indicate specific list item being set */
    listMask.setMask[listIndex] = L7_TRUE;

    rc = cosQueueMgmtTypeGlobalListSet(&list, &listMask);
  }

  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueMgmtTypeListSet(intIfNum, &list);
  }

  return rc;
}

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
                                         L7_qosCosDropParmsList_t *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE)
  {
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) != L7_TRUE)
          return L7_NOT_SUPPORTED;
  }

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueDropCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueDropParmsListGet(intIfNum, pVal);
}

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
                                         L7_qosCosDropParmsList_t *pVal)
{
  L7_RC_t                     rc = L7_NOT_SUPPORTED;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE)
  {
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) != L7_TRUE)
          return L7_NOT_SUPPORTED;
  }
 
  if (intIfNum == L7_ALL_INTERFACES)
  {
    rc = cosQueueDropParmsGlobalListSet(pVal);
  }
  else if (usmDbQosCosQueueCfgPerIntfIsAllowed() == L7_TRUE)
  {
    rc = cosQueueDropParmsListSet(intIfNum, pVal);
  } 

  return rc;
}

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
                                                L7_cosCfgParms_t *pVal) 
{
  L7_uint32 i;

  for (i=0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++) 
  {
      cosDefaultQueueConfigBuild(&(pVal->queue[i]), i);
  }
  return L7_SUCCESS;
}

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
                                            L7_uint32 queueId)
{
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE)
    {
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                                  L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) != L7_TRUE)
            return L7_NOT_SUPPORTED;
    }

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (usmDbQosCosQueueDropCfgPerIntfIsAllowed() == L7_FALSE))
    return L7_NOT_SUPPORTED;

  return cosQueueDropDefaultsRestore(intIfNum, queueId);
}

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
                                            L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueNumQueuesPerPortGet(pVal);
}

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
                                             L7_uint32 *pVal)
{
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_FEATURE_SUPPORTED) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return cosQueueNumDropPrecLevelsGet(pVal);
}

