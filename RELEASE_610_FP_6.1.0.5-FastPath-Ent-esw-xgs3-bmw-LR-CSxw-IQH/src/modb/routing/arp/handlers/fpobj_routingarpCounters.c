/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingarpCounters.c
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
#include "_xe_routingarpCounters_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingarpCounters_totalEntryCountPeak
*
* @purpose Get 'totalEntryCountPeak'
*
* @description [totalEntryCountPeak]: Peak number of entries recorded in the
*              ARP cache over time. This value is restarted whenever the
*              ARP cache size is changed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpCounters_totalEntryCountPeak (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtotalEntryCountPeakValue;
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
  objtotalEntryCountPeakValue = pStats.cachePeak;
   
  FPOBJ_TRACE_VALUE (bufp, &objtotalEntryCountPeakValue,
                     sizeof (objtotalEntryCountPeakValue));

  /* return the object value: totalEntryCountPeak */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtotalEntryCountPeakValue,
                           sizeof (objtotalEntryCountPeakValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpCounters_staticEntryCountCurrent
*
* @purpose Get 'staticEntryCountCurrent'
*
* @description [staticEntryCountCurrent]: Current number of configured static
*              ARP entries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpCounters_staticEntryCountCurrent (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstaticEntryCountCurrentValue;
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
  objstaticEntryCountCurrentValue = pStats.staticCurrent;

  FPOBJ_TRACE_VALUE (bufp, &objstaticEntryCountCurrentValue,
                     sizeof (objstaticEntryCountCurrentValue));

  /* return the object value: staticEntryCountCurrent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objstaticEntryCountCurrentValue,
                           sizeof (objstaticEntryCountCurrentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingarpCounters_totalEntryCountCurrent
*
* @purpose Get 'totalEntryCountCurrent'
*
* @description [totalEntryCountCurrent]: Current number of entries in the
*              ARP cache. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingarpCounters_totalEntryCountCurrent (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtotalEntryCountCurrentValue;
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
  objtotalEntryCountCurrentValue = pStats.cacheCurrent;

  FPOBJ_TRACE_VALUE (bufp, &objtotalEntryCountCurrentValue,
                     sizeof (objtotalEntryCountCurrentValue));

  /* return the object value: totalEntryCountCurrent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtotalEntryCountCurrentValue,
                           sizeof (objtotalEntryCountCurrentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
