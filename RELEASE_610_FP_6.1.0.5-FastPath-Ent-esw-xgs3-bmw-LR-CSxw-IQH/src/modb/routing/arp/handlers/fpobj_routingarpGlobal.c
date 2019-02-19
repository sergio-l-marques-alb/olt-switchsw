/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingarpGlobal.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to arp-object.xml
*
* @create  16 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingarpGlobal_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingarpGlobal_maxRetries
*
* @purpose Get 'maxRetries'
*
* @description [maxRetries]: Configures the ARP count of maximum request for
*              retries Allowable range: 0-10 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_maxRetries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmaxRetriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpArpRetriesGet (L7_UNIT_CURRENT, &objmaxRetriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmaxRetriesValue, sizeof (objmaxRetriesValue));

  /* return the object value: maxRetries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmaxRetriesValue,
                           sizeof (objmaxRetriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpGlobal_maxRetries
*
* @purpose Set 'maxRetries'
*
* @description [maxRetries]: Configures the ARP count of maximum request for
*              retries Allowable range: 0-10 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_maxRetries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmaxRetriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: maxRetries */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmaxRetriesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmaxRetriesValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpRetriesSet (L7_UNIT_CURRENT, objmaxRetriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpGlobal_dynamicRenew
*
* @purpose Get 'dynamicRenew'
*
* @description [dynamicRenew]: This indicates the current setting of ARP dynamic
*              renew mode. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_dynamicRenew (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdynamicRenewValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpArpDynamicRenewGet (L7_UNIT_CURRENT, &objdynamicRenewValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdynamicRenewValue,
                     sizeof (objdynamicRenewValue));

  /* return the object value: dynamicRenew */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdynamicRenewValue,
                           sizeof (objdynamicRenewValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpGlobal_dynamicRenew
*
* @purpose Set 'dynamicRenew'
*
* @description [dynamicRenew]: This indicates the current setting of ARP dynamic
*              renew mode. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_dynamicRenew (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdynamicRenewValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dynamicRenew */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdynamicRenewValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdynamicRenewValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpDynamicRenewSet (L7_UNIT_CURRENT, objdynamicRenewValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpGlobal_cacheSize
*
* @purpose Get 'cacheSize'
*
* @description [cacheSize]: Configures the ARP maximum number of entries in
*              the cache. Allowable range is platform-specific. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_cacheSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objcacheSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpArpCacheSizeGet (L7_UNIT_CURRENT, &objcacheSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcacheSizeValue, sizeof (objcacheSizeValue));

  /* return the object value: cacheSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcacheSizeValue,
                           sizeof (objcacheSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpGlobal_cacheSize
*
* @purpose Set 'cacheSize'
*
* @description [cacheSize]: Configures the ARP maximum number of entries in
*              the cache. Allowable range is platform-specific. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_cacheSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objcacheSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cacheSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcacheSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcacheSizeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpCacheSizeSet (L7_UNIT_CURRENT, objcacheSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpGlobal_staticEntryCountMax
*
* @purpose Get 'staticEntryCountMax'
*
* @description [staticEntryCountMax]: Maximum number of configurable static
*              ARP entries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_staticEntryCountMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstaticEntryCountMaxValue;
  L7_arpCacheStats_t pStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpArpCacheStatsGet (L7_UNIT_CURRENT, &pStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objstaticEntryCountMaxValue = pStats.staticMax;

  FPOBJ_TRACE_VALUE (bufp, &objstaticEntryCountMaxValue,
                     sizeof (objstaticEntryCountMaxValue));

  /* return the object value: staticEntryCountMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objstaticEntryCountMaxValue,
                           sizeof (objstaticEntryCountMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpGlobal_responseTime
*
* @purpose Get 'responseTime'
*
* @description [responseTime]: Configures the ARP request response timeout
*              in seconds. Allowable range: 1-10 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_responseTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objresponseTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpArpRespTimeGet (L7_UNIT_CURRENT, &objresponseTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objresponseTimeValue,
                     sizeof (objresponseTimeValue));

  /* return the object value: responseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objresponseTimeValue,
                           sizeof (objresponseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpGlobal_responseTime
*
* @purpose Set 'responseTime'
*
* @description [responseTime]: Configures the ARP request response timeout
*              in seconds. Allowable range: 1-10 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_responseTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objresponseTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: responseTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objresponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objresponseTimeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpRespTimeSet (L7_UNIT_CURRENT, objresponseTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpGlobal_ageoutTime
*
* @purpose Get 'ageoutTime'
*
* @description [ageoutTime]: Configures the ARP entry ageout time in seconds.Allowable
*              range: 15-21600 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpGlobal_ageoutTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objageoutTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpArpAgeTimeGet (L7_UNIT_CURRENT, &objageoutTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objageoutTimeValue, sizeof (objageoutTimeValue));

  /* return the object value: ageoutTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objageoutTimeValue,
                           sizeof (objageoutTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingarpGlobal_ageoutTime
*
* @purpose Set 'ageoutTime'
*
* @description [ageoutTime]: Configures the ARP entry ageout time in seconds.Allowable
*              range: 15-21600 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_ageoutTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objageoutTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ageoutTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objageoutTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objageoutTimeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpArpAgeTimeSet (L7_UNIT_CURRENT, objageoutTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingarpGlobal_removeArpEntry
*
* @purpose Set 'removeArpEntry'
*
* @description [removeArpEntry]: Allows the user to remove certain entries from the ARP Table
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_removeArpEntry (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemoveArpEntryVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: removeArpEntry */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRemoveArpEntryVal, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRemoveArpEntryVal, owa.len);

  /* set the value in application */
  switch(objRemoveArpEntryVal)
  {
    case L7_ARP_REMOVE_ALL_DYNAMIC:
      owa.l7rc = usmDbIpArpCacheClear(L7_UNIT_CURRENT, L7_FALSE);
      break;
    case L7_ARP_REMOVE_ALL_DYN_AND_GW:
      owa.l7rc = usmDbIpArpCacheClear(L7_UNIT_CURRENT, L7_TRUE);
      break;
    default:
      owa.l7rc = L7_SUCCESS;
      break;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingarpGlobal_removeArpEntryIP
*
* @purpose Set 'removeArpEntryIP'
*
* @description [removeArpEntryIP]: Allows the user to remove an ARP entry corresponding to an IP Address.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingarpGlobal_removeArpEntryIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t removeArpEntryIP;
  xLibU32_t objRemoveArpEntryVal;
  L7_arpEntry_t arpEntry;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: removeArpEntryIP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & removeArpEntryIP, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &removeArpEntryIP, owa.len);

  /* retrieve arp entry removal command */
  owa.len = sizeof(objRemoveArpEntryVal);
  owa.rc = xLibFilterGet (wap, XOBJ_routingarpGlobal_removeArpEntry,
                          (xLibU8_t *)&objRemoveArpEntryVal, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (wap, owa);
    return owa.rc;
  }

  /* set the value in application */
  switch(objRemoveArpEntryVal)
  {
    case L7_ARP_REMOVE_SPECIFIC_DYN_AND_GW:
      owa.l7rc = usmDbIpArpEntryGet(L7_UNIT_CURRENT, removeArpEntryIP, L7_INVALID_INTF, &arpEntry);
      if(owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_INVALID_ARPREMOVEENTRY;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      else
      {
        owa.l7rc = usmDbIpArpEntryPurge(L7_UNIT_CURRENT, removeArpEntryIP, L7_INVALID_INTF);
      }
      break;
    case L7_ARP_REMOVE_SPECIFIC_STATIC_ARP:
      owa.l7rc = usmDbIpMapStaticArpDelete(L7_UNIT_CURRENT, removeArpEntryIP, L7_NULL);
      if(owa.l7rc == L7_NOT_EXIST)
      {
        owa.rc = XLIBRC_INVALID_ARPREMOVEENTRY;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      break;
    default:
      owa.l7rc = L7_SUCCESS;
      break;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
