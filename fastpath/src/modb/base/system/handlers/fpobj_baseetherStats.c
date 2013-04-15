/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseetherStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseetherStats_obj.h"
#include "usmdb_mib_rmon_api.h"
#include "usmdb_util_api.h"
#include "xlib_private.h"
#include "usmdb_2233_stats_api.h"
/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsIndex
*
* @purpose Get 'etherStatsIndex'
*
* @description  The value of this object uniquely identifies this etherStats 
*              entry. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsIndexValue;
  xLibU32_t nextObjetherStatsIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & objetherStatsIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjetherStatsIndexValue = 1;	
    owa.l7rc = usmDbExtIfNumberCheck (L7_UNIT_CURRENT,
                                     nextObjetherStatsIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objetherStatsIndexValue, owa.len);
    owa.l7rc =
      usmDbGetNextVisibleExtIfNumber (objetherStatsIndexValue,
                                      &nextObjetherStatsIndexValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjetherStatsIndexValue, owa.len);
  /* return the object value: etherStatsIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjetherStatsIndexValue,
                           sizeof (objetherStatsIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsDataSource
*
* @purpose Get 'etherStatsDataSource'
*
* @description  This object identifies the source of the data that this etherStats 
*              entry is configured to analyze. This source can be any 
*              ethernet interface on this device. In order to identify a particular 
*              interface, this object shall i 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsDataSource (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objetherStatsDataSourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsDataSourceGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                  &objetherStatsDataSourceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objetherStatsDataSourceValue,
                           sizeof (objetherStatsDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseetherStats_etherStatsDataSource
*
* @purpose Set 'etherStatsDataSource'
*
* @description  This object identifies the source of the data that this etherStats 
*              entry is configured to analyze. This source can be any 
*              ethernet interface on this device. In order to identify a particular 
*              interface, this object shall i 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseetherStats_etherStatsDataSource (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objetherStatsDataSourceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: etherStatsDataSource */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objetherStatsDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objetherStatsDataSourceValue, owa.len);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbEtherStatsDataSourceSet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                  objetherStatsDataSourceValue);
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
* @function fpObjGet_baseetherStats_etherStatsDropEvents
*
* @purpose Get 'etherStatsDropEvents'
*
* @description  The total number of events in which packets were dropped by the 
*              probe due to lack of resources. Note that this number is not 
*              necessarily the number of packets dropped; it is just the 
*              number of times this condition has been 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsDropEvents (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsDropEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsDropEventsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                  &objetherStatsDropEventsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsDropEvents */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsDropEventsValue,
                           sizeof (objetherStatsDropEventsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsOctets
*
* @purpose Get 'etherStatsOctets'
*
* @description  The total number of octets of data (including those in bad packets) 
*              received on the network (excluding framing bits but including 
*              FCS octets). This object can be used as a reasonable 
*              estimate of 10-Megabit etherne 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objetherStatsOctetsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInOctetsGet(L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                       &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objetherStatsOctetsValue, 0x00, sizeof(objetherStatsOctetsValue));
  memcpy(objetherStatsOctetsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: etherStatsOctets */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objetherStatsOctetsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts
*
* @purpose Get 'etherStatsPkts'
*
* @description  The total number of packets (including bad packets, broadcast 
*              packets, and multicast packets) received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEtherStatsPktsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                     &objetherStatsPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPktsValue,
                           sizeof (objetherStatsPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsBroadcastPkts
*
* @purpose Get 'etherStatsBroadcastPkts'
*
* @description  The total number of good packets received that were directed 
*              to the broadcast address. Note that this does not include multicast 
*              packets. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsBroadcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objetherStatsBroadcastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIfHCInBroadcastPktsGet(L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                     &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objetherStatsBroadcastPktsValue, 0x00, sizeof(objetherStatsBroadcastPktsValue));
  memcpy(objetherStatsBroadcastPktsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: etherStatsBroadcastPkts */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objetherStatsBroadcastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsMulticastPkts
*
* @purpose Get 'etherStatsMulticastPkts'
*
* @description  The total number of good packets received that were directed 
*              to a multicast address. Note that this number does not include 
*              packets directed to the broadcast address. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsMulticastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objetherStatsMulticastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIfHCInMulticastPktsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                     &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objetherStatsMulticastPktsValue, 0x00, sizeof(objetherStatsMulticastPktsValue));
  memcpy(objetherStatsMulticastPktsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: etherStatsMulticastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objetherStatsMulticastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsCRCAlignErrors
*
* @purpose Get 'etherStatsCRCAlignErrors'
*
* @description  The total number of packets received that had a length (excluding 
*              framing bits, but including FCS octets) of between 64 and 
*              1518 octets, inclusive, but had either a bad Frame Check Sequence 
*              (FCS) with an integral n 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsCRCAlignErrors (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsCRCAlignErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsCRCAlignErrorsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                      &objetherStatsCRCAlignErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsCRCAlignErrors */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsCRCAlignErrorsValue,
                    sizeof (objetherStatsCRCAlignErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsUndersizePkts
*
* @purpose Get 'etherStatsUndersizePkts'
*
* @description  The total number of packets received that were less than 64 octets 
*              long (excluding framing bits, but including FCS octets) 
*              and were otherwise well formed. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsUndersizePkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsUndersizePktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsUndersizePktsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                     &objetherStatsUndersizePktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsUndersizePkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsUndersizePktsValue,
                           sizeof (objetherStatsUndersizePktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsOversizePkts
*
* @purpose Get 'etherStatsOversizePkts'
*
* @description  The total number of packets received that were longer than 1518 
*              octets (excluding framing bits, but including FCS octets) 
*              and were otherwise well formed. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsOversizePkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsOversizePktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsOversizePktsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                    &objetherStatsOversizePktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsOversizePkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsOversizePktsValue,
                           sizeof (objetherStatsOversizePktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsFragments
*
* @purpose Get 'etherStatsFragments'
*
* @description  The total number of packets received that were less than 64 octets 
*              in length (excluding framing bits but including FCS octets) 
*              and had either a bad Frame Check Sequence (FCS) with an 
*              integral number of octets (FCS Error) or a 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsFragments (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsFragmentsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsFragmentsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                 &objetherStatsFragmentsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsFragments */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsFragmentsValue,
                           sizeof (objetherStatsFragmentsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsJabbers
*
* @purpose Get 'etherStatsJabbers'
*
* @description  The total number of packets received that were longer than 1518 
*              octets (excluding framing bits, but including FCS octets), 
*              and had either a bad Frame Check Sequence (FCS) with an integral 
*              number of octets (FCS Error) or a 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsJabbers (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsJabbersValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsJabbersGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                               &objetherStatsJabbersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsJabbers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsJabbersValue,
                           sizeof (objetherStatsJabbersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsCollisions
*
* @purpose Get 'etherStatsCollisions'
*
* @description  The best estimate of the total number of collisions on this Ethernet 
*              segment. The value returned will depend on the location 
*              of the RMON probe. Section 8.2.1.3 (10BASE-5) and section 
*              10.3.1.3 (10BASE-2) of IEEE standard 8 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsCollisions (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsCollisionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsCollisionsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                  &objetherStatsCollisionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsCollisions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsCollisionsValue,
                           sizeof (objetherStatsCollisionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts64Octets
*
* @purpose Get 'etherStatsPkts64Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were 64 octets in length (excluding framing bits but including 
*              FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts64Octets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts64OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts64OctetsGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                    &objetherStatsPkts64OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts64Octets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts64OctetsValue,
                           sizeof (objetherStatsPkts64OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts65to127Octets
*
* @purpose Get 'etherStatsPkts65to127Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were between 65 and 127 octets in length inclusive (excluding 
*              framing bits but including FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts65to127Octets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts65to127OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts65to127OctetsGet (L7_UNIT_CURRENT,
                                         keyetherStatsIndexValue,
                                         &objetherStatsPkts65to127OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts65to127Octets */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts65to127OctetsValue,
                    sizeof (objetherStatsPkts65to127OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts128to255Octets
*
* @purpose Get 'etherStatsPkts128to255Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were between 128 and 255 octets in length inclusive (excluding 
*              framing bits but including FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts128to255Octets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts128to255OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts128to255OctetsGet (L7_UNIT_CURRENT,
                                          keyetherStatsIndexValue,
                                          &objetherStatsPkts128to255OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts128to255Octets */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts128to255OctetsValue,
                    sizeof (objetherStatsPkts128to255OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts256to511Octets
*
* @purpose Get 'etherStatsPkts256to511Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were between 256 and 511 octets in length inclusive (excluding 
*              framing bits but including FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts256to511Octets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts256to511OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts256to511OctetsGet (L7_UNIT_CURRENT,
                                          keyetherStatsIndexValue,
                                          &objetherStatsPkts256to511OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts256to511Octets */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts256to511OctetsValue,
                    sizeof (objetherStatsPkts256to511OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts512to1023Octets
*
* @purpose Get 'etherStatsPkts512to1023Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were between 512 and 1023 octets in length inclusive (excluding 
*              framing bits but including FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts512to1023Octets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts512to1023OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts512to1023OctetsGet (L7_UNIT_CURRENT,
                                           keyetherStatsIndexValue,
                                           &objetherStatsPkts512to1023OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts512to1023Octets */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts512to1023OctetsValue,
                    sizeof (objetherStatsPkts512to1023OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsPkts1024to1518Octets
*
* @purpose Get 'etherStatsPkts1024to1518Octets'
*
* @description  The total number of packets (including bad packets) received 
*              that were between 1024 and 1518 octets in length inclusive (excluding 
*              framing bits but including FCS octets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsPkts1024to1518Octets (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsPkts1024to1518OctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherStatsPkts1024to1518OctetsGet (L7_UNIT_CURRENT,
                                            keyetherStatsIndexValue,
                                            &objetherStatsPkts1024to1518OctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsPkts1024to1518Octets */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsPkts1024to1518OctetsValue,
                    sizeof (objetherStatsPkts1024to1518OctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherStats_etherStatsOwner
*
* @purpose Get 'etherStatsOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsOwner (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objetherStatsOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEtherStatsOwnerGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                      objetherStatsOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objetherStatsOwnerValue,
                           strlen (objetherStatsOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseetherStats_etherStatsOwner
*
* @purpose Set 'etherStatsOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseetherStats_etherStatsOwner (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objetherStatsOwnerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: etherStatsOwner */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objetherStatsOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objetherStatsOwnerValue, owa.len);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEtherStatsOwnerSet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                      objetherStatsOwnerValue);
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
* @function fpObjGet_baseetherStats_etherStatsStatus
*
* @purpose Get 'etherStatsStatus'
*
* @description  The status of this etherStats entry. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherStats_etherStatsStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEtherStatsStatusGet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                       &objetherStatsStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherStatsStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherStatsStatusValue,
                           sizeof (objetherStatsStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseetherStats_etherStatsStatus
*
* @purpose Set 'etherStatsStatus'
*
* @description  The status of this etherStats entry. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseetherStats_etherStatsStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherStatsStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherStatsIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: etherStatsStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objetherStatsStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objetherStatsStatusValue, owa.len);

  /* retrieve key: etherStatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseetherStats_etherStatsIndex,
                          (xLibU8_t *) & keyetherStatsIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherStatsIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEtherStatsStatusSet (L7_UNIT_CURRENT, keyetherStatsIndexValue,
                                       objetherStatsStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
