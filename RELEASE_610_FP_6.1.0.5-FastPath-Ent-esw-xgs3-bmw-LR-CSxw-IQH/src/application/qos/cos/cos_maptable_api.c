/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_map_api.c
*
* @purpose    COS component mapping table API implementation
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

#include "l7_common.h"
#include "cnfgr.h"
#include "dot1q_api.h"
#include "l7_cos_api.h"
#include "cos_util.h"
#include "cos_config.h"
#include "cos_cnfgr.h"
#include "log.h"


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
L7_RC_t cosMapIpPrecIndexGet(L7_uint32 prec)
{
  if (prec >= L7_QOS_COS_MAP_NUM_IPPREC)
    return L7_FAILURE;

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpPrecIndexGetNext(L7_uint32 prec, L7_uint32 *pNext)
{
  if (prec >= (L7_QOS_COS_MAP_NUM_IPPREC - 1))
    return L7_FAILURE;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  *pNext = prec + 1;
  return L7_SUCCESS;
}

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
                                    L7_uint32 *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  /* check IP precedence in range */
  if (cosMapIpPrecIndexGet(prec) != L7_SUCCESS)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *pVal = pCfg->mapping.ipPrecMapTable[prec];
  return L7_SUCCESS;
}

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
                                    L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfg;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  /* check IP precedence in range */
  if (cosMapIpPrecIndexGet(prec) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value against queue ID range */
  if ((val < L7_QOS_COS_QUEUE_ID_MIN) ||
      (val > L7_QOS_COS_QUEUE_ID_MAX))
    return L7_FAILURE;

  /* also check value against operational number of traffic classes supported */
  if ((L7_uchar8)val > (pCosInfo_g->numTrafficClasses[intIfNum] - 1))
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->mapping.ipPrecMapTable[prec] == (L7_uchar8)val)
    return L7_SUCCESS;

  /* only apply the config change if currently in IP precedence trust mode and
   * if per-intf config is supported
   */
  if (pCfg->mapping.intfTrustMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
    if (cosIntfIsWriteableCond(intIfNum, L7_FALSE, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE)
      (void)cosMapIpPrecTrafficClassApply(intIfNum, prec, val); /* rc ignored here */

  pCfg->mapping.ipPrecMapTable[prec] = (L7_uchar8)val;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpPrecTrafficClassGlobalSet(L7_uint32 prec, L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum, globIntIfNum;

  /* check IP precedence in range */
  if (cosMapIpPrecIndexGet(prec) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value against queue ID range */
  if ((val < L7_QOS_COS_QUEUE_ID_MIN) ||
      (val > L7_QOS_COS_QUEUE_ID_MAX))
    return L7_FAILURE;

  /* also check value against operational number of traffic classes supported */
  if ((L7_uchar8)val > (pCosInfo_g->globalNumTrafficClasses - 1))
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  globIntIfNum = 0;
  intIfNum = 0;
  while (cosMapIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosMapIpPrecTrafficClassSet(intIfNum, prec, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global IP precedence mapping on intf %u "
                "(prec=%u class=%u)\n", intIfNum, prec, val);
      }

      /* if global-only config supported, find and remember intIfNum to use
       * for global 'apply' call below
       */
      if (globIntIfNum == 0)
        if (cosIntfIsWriteableCond(intIfNum, L7_TRUE, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE)
          globIntIfNum = intIfNum;
    }
  }

  if (pCfgGlob->mapping.ipPrecMapTable[prec] != (L7_uchar8)val)
  {
    if (globIntIfNum != 0)
      (void)cosMapIpPrecTrafficClassApply(globIntIfNum, prec, val); /* rc ignored here */

    pCfgGlob->mapping.ipPrecMapTable[prec] = (L7_uchar8)val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

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
                                           L7_uint32 *pVal)
{
  /* called function checks input parms */
  return cosDefaultMappingIpPrecGet(intIfNum, prec, pVal);
}

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
L7_RC_t cosMapIpPrecDefaultsRestore(L7_uint32 intIfNum)
{
  L7_uint32     i, val;

  /* update each interface config value using default mapping */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
  {
    if ((cosDefaultMappingIpPrecGet(intIfNum, i, &val) != L7_SUCCESS) ||
        (cosMapIpPrecTrafficClassSet(intIfNum, i, val) != L7_SUCCESS))
    {
      LOG_MSG("Unable to set default IP precedence mapping for intf %u, "
              "prec=%u class=%u\n", intIfNum, i, val);
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpPrecDefaultsGlobalRestore(void)
{
  L7_uint32     i, val;

  /* update each global config value using default mapping */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
  {
    if ((cosDefaultMappingIpPrecGet(L7_ALL_INTERFACES, i, &val) != L7_SUCCESS) ||
        (cosMapIpPrecTrafficClassGlobalSet(i, val) != L7_SUCCESS))
    {
      LOG_MSG("Unable to set global default IP precedence mapping for "
              "prec=%u traffic class=%u\n", i, val);
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpDscpIndexGet(L7_uint32 dscp)
{
  if (dscp >= L7_QOS_COS_MAP_NUM_IPDSCP)
    return L7_FAILURE;

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpDscpIndexGetNext(L7_uint32 dscp, L7_uint32 *pNext)
{
  if (dscp >= (L7_QOS_COS_MAP_NUM_IPDSCP - 1))
    return L7_FAILURE;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  *pNext = dscp + 1;
  return L7_SUCCESS;
}

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
                                    L7_uint32 *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  /* check IP DSCP in range */
  if (cosMapIpDscpIndexGet(dscp) != L7_SUCCESS)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *pVal = pCfg->mapping.ipDscpMapTable[dscp];
  return L7_SUCCESS;
}

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
                                    L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfg;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  /* check IP DSCP in range */
  if (cosMapIpDscpIndexGet(dscp) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value against queue ID range */
  if ((val < L7_QOS_COS_QUEUE_ID_MIN) ||
      (val > L7_QOS_COS_QUEUE_ID_MAX))
    return L7_FAILURE;

  /* also check value against operational number of traffic classes supported */
  if ((L7_uchar8)val > (pCosInfo_g->numTrafficClasses[intIfNum] - 1))
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->mapping.ipDscpMapTable[dscp] == (L7_uchar8)val)
    return L7_SUCCESS;

  /* only apply the config change if currently in IP DSCP trust mode and
   * if per-intf config is supported
   */
  if (pCfg->mapping.intfTrustMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
    if (cosIntfIsWriteableCond(intIfNum, L7_FALSE, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE)
      (void)cosMapIpDscpTrafficClassApply(intIfNum, dscp, val); /* rc ignored here */

  pCfg->mapping.ipDscpMapTable[dscp] = (L7_uchar8)val;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpDscpTrafficClassGlobalSet(L7_uint32 dscp, L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum, globIntIfNum;

  /* check IP DSCP in range */
  if (cosMapIpDscpIndexGet(dscp) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value against queue ID range */
  if ((val < L7_QOS_COS_QUEUE_ID_MIN) ||
      (val > L7_QOS_COS_QUEUE_ID_MAX))
    return L7_FAILURE;

  /* also check value against operational number of traffic classes supported */
  if ((L7_uchar8)val > (pCosInfo_g->globalNumTrafficClasses - 1))
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  globIntIfNum = 0;
  intIfNum = 0;
  while (cosMapIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosMapIpDscpTrafficClassSet(intIfNum, dscp, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global IP DSCP mapping on intf %u "
                "(dscp=%u class=%u)\n", intIfNum, dscp, val);
      }

      /* if global-only config supported, find and remember intIfNum to use
       * for global 'apply' call below
       */
      if (globIntIfNum == 0)
        if (cosIntfIsWriteableCond(intIfNum, L7_TRUE, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE)
          globIntIfNum = intIfNum;
    }
  }

  if (pCfgGlob->mapping.ipDscpMapTable[dscp] != (L7_uchar8)val)
  {
    if (globIntIfNum != 0)
      (void)cosMapIpDscpTrafficClassApply(globIntIfNum, dscp, val); /* rc ignored here */

    pCfgGlob->mapping.ipDscpMapTable[dscp] = (L7_uchar8)val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

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
                                           L7_uint32 *pVal)
{
  /* called function checks input parms */
  return cosDefaultMappingIpDscpGet(intIfNum, dscp, pVal);
}

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
L7_RC_t cosMapIpDscpDefaultsRestore(L7_uint32 intIfNum)
{
  L7_uint32     i, val;

  /* update each interface config value using default mapping */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
  {
    if ((cosDefaultMappingIpDscpGet(intIfNum, i, &val) != L7_SUCCESS) ||
        (cosMapIpDscpTrafficClassSet(intIfNum, i, val) != L7_SUCCESS))
    {
      LOG_MSG("Unable to set default IP DSCP mapping for intf %u, "
              "prec=%u class=%u\n", intIfNum, i, val);
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIpDscpDefaultsGlobalRestore(void)
{
  L7_uint32     i, val;

  /* update each global config value using default mapping */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
  {
    if ((cosDefaultMappingIpDscpGet(L7_ALL_INTERFACES, i, &val) != L7_SUCCESS) ||
        (cosMapIpDscpTrafficClassGlobalSet(i, val) != L7_SUCCESS))
    {
      LOG_MSG("Unable to set global default IP DSCP mapping for "
              "prec=%u traffic class=%u\n", i, val);
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIntfIndexGet(L7_uint32 intIfNum)
{
  return cosIntfIndexGet(intIfNum);
}

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
L7_RC_t cosMapIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext)
{
  return cosIntfIndexGetNext(intIfNum, pNext);
}

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
L7_BOOL cosMapIntfIsValid(L7_uint32 intIfNum)
{
  /* treat L7_ALL_INTERFACES global config indicator as valid value */
  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (cosMapIntfIndexGet(intIfNum) != L7_SUCCESS)
      return L7_FALSE;
  }

  return L7_TRUE;
}

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
                               L7_QOS_COS_MAP_INTF_MODE_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *pVal = (L7_QOS_COS_MAP_INTF_MODE_t)pCfg->mapping.intfTrustMode;
  return L7_SUCCESS;
}

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
                               L7_QOS_COS_MAP_INTF_MODE_t val)
{
  L7_cosCfgParms_t  *pCfg;
#if defined(FEAT_METRO_CPE_V1_0)
  L7_cosCfgParms_t  *pCfgGlob;
#endif

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  /* check proposed value against trust mode range */
  if ((val < L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED) ||
      (val > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP))
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

#if defined(FEAT_METRO_CPE_V1_0)
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* Do not allow config change of interface mode when global mode is different.
     Allow only untrusted */
  if (pCfgGlob->mapping.intfTrustMode != val && val != L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->mapping.intfTrustMode == (L7_uchar8)val && 
      val != L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
    return L7_SUCCESS;
#else
  if (pCfg->mapping.intfTrustMode == (L7_uchar8)val)
    return L7_SUCCESS;
#endif
  /* NOTE: Passing new trust mode via pCfg, so must store updated value
   *       before calling apply function.
   */
  pCfg->mapping.intfTrustMode = (L7_uchar8)val;

  /* setting flag to true to force DTL call even if changing to trust-dot1p mode */
  (void)cosMapIntfTrustModeApply(intIfNum, pCfg, L7_TRUE);  /* rc ignored here */

  if (pCfg->mapping.intfTrustMode != (L7_uchar8)val)
   pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

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
L7_RC_t cosMapIntfTrustModeGlobalSet(L7_QOS_COS_MAP_INTF_MODE_t val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum;

  /* check proposed value against trust mode range */
  if ((val < L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED) ||
      (val > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP))
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  if (pCfgGlob->mapping.intfTrustMode != (L7_uchar8)val)
  {
    pCfgGlob->mapping.intfTrustMode = (L7_uchar8)val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosMapIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosMapIntfTrustModeSet(intIfNum, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global \'%s\' trust mode on intf %u\n",
                cosMapIntfModeStr[val], intIfNum);
      }
    }
  } /* endfor i */


  return L7_SUCCESS;
}

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
                                                  L7_uint32 *pVal)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uint32         portDefaultTrafficClass;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  if (intIfNum == L7_ALL_INTERFACES)
    portDefaultTrafficClass = pCosInfo_g->globalPortDefaultTrafficClass;
  else
    portDefaultTrafficClass = pCosInfo_g->portDefaultTrafficClass[intIfNum];

  *pVal = portDefaultTrafficClass;
  return L7_SUCCESS;
}

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
*           the corresponding traffic class value in the dot1p table is
*           updated, since that affects the queue to which the default
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
                                     L7_uint32 portDefaultTrafficClass)
{
  L7_cosCfgParms_t  *pCfg;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (intIfNum >= L7_MAX_INTERFACE_COUNT))
    return;                   /* invalid internal interface number */

  /* ignore any value that is out of range for this platform */
  if ((portDefaultPriority < L7_DOT1P_MIN_PRIORITY) ||
      (portDefaultPriority > L7_DOT1P_MAX_PRIORITY))
    return;

  if ((portDefaultTrafficClass < L7_DOT1P_MIN_TRAFFIC_CLASS) ||
      (portDefaultTrafficClass > L7_DOT1P_MAX_TRAFFIC_CLASS))
    return;

  if (pCosInfo_g == L7_NULLPTR)
    return;

  /* record the new values in the COS info structure, but stop here if the
   * portDefaultTrafficClass is unchanged
   *
   * NOTE:  When a global change occurs, the dot1p component calls this update
   *        function for each interface, so no need to iterate through all
   *        interfaces again here.
   */
  if (intIfNum == L7_ALL_INTERFACES)
  {
    pCosInfo_g->globalPortDefaultPriority = (L7_uchar8)portDefaultPriority;
    if (pCosInfo_g->globalPortDefaultTrafficClass == (L7_uchar8)portDefaultTrafficClass)
      return;
    pCosInfo_g->globalPortDefaultTrafficClass = (L7_uchar8)portDefaultTrafficClass;
  }
  else
  {
    pCosInfo_g->portDefaultPriority[intIfNum] = (L7_uchar8)portDefaultPriority;
    if (pCosInfo_g->portDefaultTrafficClass[intIfNum] == (L7_uchar8)portDefaultTrafficClass)
      return;
    pCosInfo_g->portDefaultTrafficClass[intIfNum] = (L7_uchar8)portDefaultTrafficClass;
  }

  /* stop here if interface is not configurable for COS */
  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return;

  /* re-apply the current COS interface trust mode, which will use the new 
   * port default traffic class info
   *
   * NOTE:  No need to 'apply' a global config change since dot1p component
   *        will call this function separately for each interface.
   */
  if (intIfNum != L7_ALL_INTERFACES)
  {
    /* no need to apply this change if currently in 'trust dot1p' mode */
    if (pCfg->mapping.intfTrustMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      return;

    if (cosConfigIntfMapTableDataApply(intIfNum, pCfg, L7_FALSE) != L7_SUCCESS)
    {
      LOG_MSG("%s: Unable to apply port default traffic class COS mapping to intf %u \n", 
              __FUNCTION__, intIfNum);
      return;
    }
  }
}

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
                                   L7_uint32 numTrafficClasses)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uchar8         *pNum;

  if ((intIfNum != L7_ALL_INTERFACES) &&
      (intIfNum >= L7_MAX_INTERFACE_COUNT))
    return;                   /* invalid internal interface number */

  /* ignore any value that is out of range for this platform */
  if ((numTrafficClasses < 1) ||
      (numTrafficClasses > L7_DOT1P_NUM_TRAFFIC_CLASSES))
    return;

  if (pCosInfo_g == L7_NULLPTR)
    return;

  /* record the new value in the COS info structure, if different
   *
   * NOTE:  When a global change occurs, the dot1p component calls this update
   *        function for each interface, so no need to iterate through all
   *        interfaces again here.
   */
  if (intIfNum == L7_ALL_INTERFACES)
    pNum = &pCosInfo_g->globalNumTrafficClasses;
  else
    pNum = &pCosInfo_g->numTrafficClasses[intIfNum];

  if (*pNum == (L7_uchar8)numTrafficClasses)
    return;

  *pNum = (L7_uchar8)numTrafficClasses;

  /* stop here if interface is not configurable for COS */
  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return;

  /* re-initialize the COS mapping tables */
  cosDefaultMappingConfigBuild(intIfNum, &pCfg->mapping);

  /* NOTE:  No need to 'apply' a global config change since dot1p component
   *        will call this function separately for each interface.
   */
  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (cosConfigIntfMapTableDataApply(intIfNum, pCfg, L7_FALSE) != L7_SUCCESS)
    {
      LOG_MSG("%s: Unable to apply default COS map table config to intf %u \n", 
              __FUNCTION__, intIfNum);
      return;
    }
  }
}

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
L7_BOOL cosMapDot1pMappingIsActive(L7_uint32 intIfNum)
{
  L7_BOOL                     rc = L7_FALSE;
  L7_QOS_COS_MAP_INTF_MODE_t  intfTrustMode;

  /* the COS component must be in a ready state before evaluating its
   * interface trust mode configuration value (not ready is treated the 
   * same as if the COS component does not exist)
   */
  if (COS_IS_READY != L7_TRUE)
    return L7_TRUE;

  /* NOTE:  intIfNum validity is checked within the called function chain */
  if (cosMapIntfTrustModeGet(intIfNum, &intfTrustMode) == L7_SUCCESS)
    if (intfTrustMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      rc = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Check if all COS Mapping Table contents are valid for this
*           interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pCfgMap    @b{(input)}  Ptr to COS mapping table config parms
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Internal helper function, not part of component API.
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_BOOL cosMapTableContentIsValid(L7_uint32 intIfNum, L7_cosMapCfg_t *pCfgMap)
{
  L7_uint32         i, numTrafficClasses;

  if ((pCfgMap == L7_NULLPTR) || (pCosInfo_g == L7_NULLPTR))
    return L7_FALSE;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    numTrafficClasses = pCosInfo_g->globalNumTrafficClasses;
  }
  else
  {
    /* Check general intIfNum boundary conditions */
    if ((intIfNum == 0) || (intIfNum >= L7_MAX_INTERFACE_COUNT))
      return L7_FALSE;

    numTrafficClasses = pCosInfo_g->numTrafficClasses[intIfNum];
  }

  /* check for any COS mapping table entry whose value exceeds the
   * allowed maximum traffic class value
   */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
    if (pCfgMap->ipPrecMapTable[i] > (numTrafficClasses-1))
      return L7_FALSE;

  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
    if (pCfgMap->ipDscpMapTable[i] > (numTrafficClasses-1))
      return L7_FALSE;

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Display the current COS Mapping Table contents
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of 0 or L7_ALL_INTERFACES causes global tables 
*           to be displayed.
*
* @end
*********************************************************************/
void cosMapTableShow(L7_uint32 intIfNum)
{
  L7_cosMapCfg_t        *pMap;
  L7_uint32             msgLvlReqd;
  L7_cosCfgIntfParms_t  *pCfgIntf;

  /* specify required COS message level to display this output */
  msgLvlReqd = COS_MSGLVL_ON;

  if (intIfNum == 0)
    intIfNum = L7_ALL_INTERFACES;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    pMap = &pCosCfgData_g->cosGlobal.cfg.mapping;
  }
  else
  {
    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      pMap = &pCfgIntf->cfg.mapping;
    }
    else
    {
      COS_PRT(msgLvlReqd, "\nCould not find configuration for interface %u\n\n", 
              intIfNum);
      return;
    }
  }

  /* display all relevant mapping tables */
  cosMapPortDefaultPriorityTableShow(intIfNum, msgLvlReqd);
  cosMapDot1pTableShow(intIfNum, msgLvlReqd);
  cosMapIpPrecTableShow(intIfNum, msgLvlReqd);
  cosMapIpDscpTableShow(intIfNum, msgLvlReqd);

  /* display interface trust mode */
  COS_PRT(msgLvlReqd, "  Interface Trust Mode:  %s\n",
          cosMapIntfModeStr[pMap->intfTrustMode]);
  COS_PRT(msgLvlReqd, "\n");
}

/*********************************************************************
* @purpose  Display formatted port default priority mapping table
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    msgLvlReqd  @b{(input)}  COS debug message level required
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of L7_ALL_INTERFACES causes global table
*           to be displayed.
*
* @end
*********************************************************************/
void cosMapPortDefaultPriorityTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd)
{
  L7_uint32     i, tc;

  /* the same port default traffic class value is used for all priority levels */
  if (intIfNum == L7_ALL_INTERFACES)
    tc = pCosInfo_g->globalPortDefaultTrafficClass;
  else
    tc = pCosInfo_g->portDefaultTrafficClass[intIfNum];

  /* display port default priority mapping table 
   * (NOTE: This information comes from dot1p config, not cos)
   */
  COS_PRT(msgLvlReqd, " Def Pri 802.1p Map:  0 1 2 3 4 5 6 7 \n");
  COS_PRT(msgLvlReqd, "                      - - - - - - - - \n");
  COS_PRT(msgLvlReqd, "           queue id:  ");
  for (i = 0; i < (L7_DOT1P_MAX_PRIORITY+1); i++)
  {
    COS_PRT(msgLvlReqd, "%1u ", tc);
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display formatted dot1p mapping table
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    msgLvlReqd  @b{(input)}  COS debug message level required
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of L7_ALL_INTERFACES causes global table
*           to be displayed.
*
* @end
*********************************************************************/
void cosMapDot1pTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd)
{
  L7_uint32     i, tc;

  /* display dot1p mapping table (NOTE: This is part of dot1p config, not cos) */
  COS_PRT(msgLvlReqd, "    User 802.1p Map:  0 1 2 3 4 5 6 7 \n");
  COS_PRT(msgLvlReqd, "                      - - - - - - - - \n");
  COS_PRT(msgLvlReqd, "           queue id:  ");
  for (i = 0; i < (L7_DOT1P_MAX_PRIORITY+1); i++)
  {
    if (dot1dPortTrafficClassGet(intIfNum, i, &tc) != L7_SUCCESS)
    {
      COS_PRT(msgLvlReqd, "\nUnable to obtain config info from dot1p component");
      break;
    }
    COS_PRT(msgLvlReqd, "%1u ", tc);
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display formatted COS IP precedence mapping table
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    msgLvlReqd  @b{(input)}  COS debug message level required
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of L7_ALL_INTERFACES causes global table
*           to be displayed.
*
* @end
*********************************************************************/
void cosMapIpPrecTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd)
{
  L7_cosMapCfg_t        *pMap;
  L7_cosCfgIntfParms_t  *pCfgIntf;
  L7_uint32             i;

  if (pCosCfgData_g == L7_NULLPTR)
    return;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    pMap = &pCosCfgData_g->cosGlobal.cfg.mapping;
  }
  else
  {
    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      pMap = &pCfgIntf->cfg.mapping;
    }
    else
    {
      COS_PRT(msgLvlReqd, "\nCould not find configuration for interface %u\n\n", 
              intIfNum);
      return;
    }
  }

  /* display IP precedence mapping table */
  COS_PRT(msgLvlReqd, "  IP Precedence Map:  0 1 2 3 4 5 6 7 \n");
  COS_PRT(msgLvlReqd, "                      - - - - - - - - \n");
  COS_PRT(msgLvlReqd, "           queue id:  ");
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
  {
    COS_PRT(msgLvlReqd, "%1u ", pMap->ipPrecMapTable[i]);
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

/*********************************************************************
* @purpose  Display formatted COS IP DSCP mapping table
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    msgLvlReqd  @b{(input)}  COS debug message level required
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of L7_ALL_INTERFACES causes global table
*           to be displayed.
*
* @end
*********************************************************************/
void cosMapIpDscpTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd)
{
  L7_cosMapCfg_t        *pMap;
  L7_cosCfgIntfParms_t  *pCfgIntf;
  L7_uint32             i, colmax;

  if (pCosCfgData_g == L7_NULLPTR)
    return;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    pMap = &pCosCfgData_g->cosGlobal.cfg.mapping;
  }
  else
  {
    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      pMap = &pCfgIntf->cfg.mapping;
    }
    else
    {
      COS_PRT(msgLvlReqd, "\nCould not find configuration for interface %u\n\n", 
              intIfNum);
      return;
    }
  }

  /* display IP DSCP mapping table */
  COS_PRT(msgLvlReqd, "  IP DSCP Map: \\                       1 1 1 1 1 1 \n");
  COS_PRT(msgLvlReqd, "                \\  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 \n");
  COS_PRT(msgLvlReqd, "                 \\ - - - - - - - - - - - - - - - - ");

  colmax = 16;
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
  {
    if ((i % colmax) == 0)
    {
      COS_PRT(msgLvlReqd, "\n               %2u| ", i);
    }

    COS_PRT(msgLvlReqd, "%1u ", pMap->ipDscpMapTable[i]);
  }
  COS_PRT(msgLvlReqd, "\n\n");
}

