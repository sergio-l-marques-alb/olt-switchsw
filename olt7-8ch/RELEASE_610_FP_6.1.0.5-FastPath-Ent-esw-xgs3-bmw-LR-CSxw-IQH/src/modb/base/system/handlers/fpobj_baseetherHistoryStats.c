/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseetherHistoryStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  19 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseetherHistoryStats_obj.h"
#include "usmdb_mib_rmon_api.h"


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryIndex
*
* @purpose Get 'etherHistoryIndex'
*
* @description [etherHistoryIndex]  The history of which this entry is a part. The history identified by a particular value of this index is the same history as identified by the same value of historyControlIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryIndex (void *wap, void *bufp)
{

  xLibU32_t objetherHistoryIndexValue;
  xLibU32_t nextObjetherHistoryIndexValue;
  xLibU32_t objetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                          (xLibU8_t *) & objetherHistoryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objetherHistoryIndexValue = objetherHistorySampleIndexValue = 0;
    owa.l7rc =
      usmDbEtherHistoryEntryNext (L7_UNIT_CURRENT, objetherHistoryIndexValue,
                           &objetherHistorySampleIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objetherHistoryIndexValue, owa.len);
    nextObjetherHistoryIndexValue=objetherHistoryIndexValue;  

    do
    {
      
      objetherHistorySampleIndexValue = 0;
      owa.l7rc =
        usmDbEtherHistoryEntryNext (L7_UNIT_CURRENT, objetherHistoryIndexValue,
                             &objetherHistorySampleIndexValue);
    }
    while ((objetherHistoryIndexValue == nextObjetherHistoryIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjetherHistoryIndexValue = objetherHistoryIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjetherHistoryIndexValue, owa.len);

  /* return the object value: etherHistoryIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjetherHistoryIndexValue,
                           sizeof (objetherHistoryIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistorySampleIndex
*
* @purpose Get 'etherHistorySampleIndex'
*
* @description [etherHistorySampleIndex]  An index that uniquely identifies the particular sample this entry represents among all samples associated with the same historyControlEntry. This index starts at 1 and increases by one as each new sample is taken.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistorySampleIndex (void *wap, void *bufp)
{

  xLibU32_t objetherHistoryIndexValue;
  xLibU32_t nextObjetherHistoryIndexValue;
  xLibU32_t objetherHistorySampleIndexValue;
  xLibU32_t nextObjetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                          (xLibU8_t *) & objetherHistoryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objetherHistoryIndexValue, owa.len);

  /* retrieve key: etherHistorySampleIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                          (xLibU8_t *) & objetherHistorySampleIndexValue, &owa.len);
  nextObjetherHistoryIndexValue = objetherHistoryIndexValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objetherHistorySampleIndexValue = 0;
    owa.l7rc = 
      usmDbEtherHistoryEntryNext (L7_UNIT_CURRENT, objetherHistoryIndexValue,
                           &objetherHistorySampleIndexValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objetherHistorySampleIndexValue, owa.len);

    owa.l7rc = 
      usmDbEtherHistoryEntryNext (L7_UNIT_CURRENT, objetherHistoryIndexValue,
                           &objetherHistorySampleIndexValue);

  }

  if ((objetherHistoryIndexValue != nextObjetherHistoryIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjetherHistorySampleIndexValue = objetherHistorySampleIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjetherHistorySampleIndexValue, owa.len);

  /* return the object value: etherHistorySampleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjetherHistorySampleIndexValue,
                           sizeof (objetherHistorySampleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#if 0
/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryIndex_etherHistorySampleIndex
*
* @purpose Get 'etherHistoryIndex + etherHistorySampleIndex +'
*
* @description [etherHistoryIndex]:  The history of which this entry is a
*              part. The history identified by a particular value of this
*              index is the same history as identified by the same value of
*              historyControlIndex. 
*              [etherHistorySampleIndex]:  An index that uniquely identifies
*              the particular sample this entry represents among all samples
*              associated with the same historyControlEntry. This index
*              starts at 1 and increases by one as each new sample is
*              taken. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseetherHistoryStats_etherHistoryIndex_etherHistorySampleIndex (void
                                                                          *wap,
                                                                          void
                                                                          *bufp
                                                                          [],
                                                                          xLibU16_t
                                                                          keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaetherHistoryIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryIndexValue, nextObjetherHistoryIndexValue;
  fpObjWa_t owaetherHistorySampleIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistorySampleIndexValue,
    nextObjetherHistorySampleIndexValue;
  objetherHistoryIndexValue = 0;
  objetherHistorySampleIndexValue = 0;
  void *outetherHistoryIndex = (void *) bufp[--keyCount];
  void *outetherHistorySampleIndex = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outetherHistoryIndex);
  FPOBJ_TRACE_ENTER (outetherHistorySampleIndex);

  /* retrieve key: etherHistoryIndex */
  owaetherHistoryIndex.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                   (xLibU8_t *) & objetherHistoryIndexValue,
                   &owaetherHistoryIndex.len);
  if (owaetherHistoryIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: etherHistorySampleIndex */
    owaetherHistorySampleIndex.rc =
      xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                     (xLibU8_t *) & objetherHistorySampleIndexValue,
                     &owaetherHistorySampleIndex.len);
  }
  FPOBJ_TRACE_CURRENT_KEY (outetherHistoryIndex, &objetherHistoryIndexValue,
                           owaetherHistoryIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outetherHistorySampleIndex,
                           &objetherHistorySampleIndexValue,
                           owaetherHistorySampleIndex.len);
  owa.rc =
    usmDbEtherHistoryEntryNext (L7_UNIT_CURRENT, &objetherHistoryIndexValue,
                         &objetherHistorySampleIndexValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outetherHistoryIndex, owaetherHistoryIndex);
    FPOBJ_TRACE_EXIT (outetherHistorySampleIndex, owaetherHistorySampleIndex);
    return owa.rc;
  }
  
  nextObjetherHistoryIndexValue = objetherHistoryIndexValue;
  nextObjetherHistorySampleIndexValue = objetherHistorySampleIndexValue;
  
  FPOBJ_TRACE_CURRENT_KEY (outetherHistoryIndex, &nextObjetherHistoryIndexValue,
                           owaetherHistoryIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outetherHistorySampleIndex,
                           &nextObjetherHistorySampleIndexValue,
                           owaetherHistorySampleIndex.len);

  /* return the object value: etherHistoryIndex */
  xLibBufDataSet (outetherHistoryIndex,
                  (xLibU8_t *) & nextObjetherHistoryIndexValue,
                  sizeof (nextObjetherHistoryIndexValue));

  /* return the object value: etherHistorySampleIndex */
  xLibBufDataSet (outetherHistorySampleIndex,
                  (xLibU8_t *) & nextObjetherHistorySampleIndexValue,
                  sizeof (nextObjetherHistorySampleIndexValue));
  FPOBJ_TRACE_EXIT (outetherHistoryIndex, owaetherHistoryIndex);
  FPOBJ_TRACE_EXIT (outetherHistorySampleIndex, owaetherHistorySampleIndex);
  return XLIBRC_SUCCESS;
}

#endif

/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryIntervalStart
*
* @purpose Get 'etherHistoryIntervalStart'
*
* @description [etherHistoryIntervalStart]:  The value of sysUpTime at the
*              start of the interval over which this sample was measured.
*              If the probe keeps track of the time of day, it should start
*              the first sample of the history at a time such that when
*              the next hour 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryIntervalStart (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryIntervalStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryIntervalStartGet (L7_UNIT_CURRENT,
                                       keyetherHistoryIndexValue,
                                       keyetherHistorySampleIndexValue,
                                       &objetherHistoryIntervalStartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryIntervalStart */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryIntervalStartValue,
                    sizeof (objetherHistoryIntervalStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryDropEvents
*
* @purpose Get 'etherHistoryDropEvents'
*
* @description [etherHistoryDropEvents]:  The total number of events in which
*              packets were dropped by the probe due to lack of resources
*              during this sampling interval. Note that this number is
*              not necessarily the number of packets dropped, it is just
*              the number o 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryDropEvents (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryDropEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryDropEventsGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                    keyetherHistorySampleIndexValue,
                                    &objetherHistoryDropEventsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryDropEvents */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryDropEventsValue,
                           sizeof (objetherHistoryDropEventsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryOctets
*
* @purpose Get 'etherHistoryOctets'
*
* @description [etherHistoryOctets]:  The total number of octets of data (including
*              those in bad packets) received on the network (excluding
*              framing bits but including FCS octets). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryOctets (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryOctetsGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                keyetherHistorySampleIndexValue,
                                &objetherHistoryOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryOctetsValue,
                           sizeof (objetherHistoryOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryPkts
*
* @purpose Get 'etherHistoryPkts'
*
* @description [etherHistoryPkts]:  The number of packets (including bad packets)
*              received during this sampling interval. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryPktsGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                              keyetherHistorySampleIndexValue,
                              &objetherHistoryPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryPktsValue,
                           sizeof (objetherHistoryPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryBroadcastPkts
*
* @purpose Get 'etherHistoryBroadcastPkts'
*
* @description [etherHistoryBroadcastPkts]:  The number of good packets received
*              during this sampling interval that were directed to
*              the broadcast address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryBroadcastPkts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryBroadcastPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryBroadcastPktsGet (L7_UNIT_CURRENT,
                                       keyetherHistoryIndexValue,
                                       keyetherHistorySampleIndexValue,
                                       &objetherHistoryBroadcastPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryBroadcastPkts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryBroadcastPktsValue,
                    sizeof (objetherHistoryBroadcastPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryMulticastPkts
*
* @purpose Get 'etherHistoryMulticastPkts'
*
* @description [etherHistoryMulticastPkts]:  The number of good packets received
*              during this sampling interval that were directed to
*              a multicast address. Note that this number does not include
*              packets addressed to the broadcast address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryMulticastPkts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryMulticastPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryMulticastPktsGet (L7_UNIT_CURRENT,
                                       keyetherHistoryIndexValue,
                                       keyetherHistorySampleIndexValue,
                                       &objetherHistoryMulticastPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryMulticastPkts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryMulticastPktsValue,
                    sizeof (objetherHistoryMulticastPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryCRCAlignErrors
*
* @purpose Get 'etherHistoryCRCAlignErrors'
*
* @description [etherHistoryCRCAlignErrors]:  The number of packets received
*              during this sampling interval that had a length (excluding
*              framing bits but including FCS octets) between 64 and 1518
*              octets, inclusive, but had either a bad Frame Check Sequence
*              (FCS) with a 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryCRCAlignErrors (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryCRCAlignErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryCRCAlignErrorsGet (L7_UNIT_CURRENT,
                                        keyetherHistoryIndexValue,
                                        keyetherHistorySampleIndexValue,
                                        &objetherHistoryCRCAlignErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryCRCAlignErrors */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryCRCAlignErrorsValue,
                    sizeof (objetherHistoryCRCAlignErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryUndersizePkts
*
* @purpose Get 'etherHistoryUndersizePkts'
*
* @description [etherHistoryUndersizePkts]:  The number of packets received
*              during this sampling interval that were less than 64 octets
*              long (excluding framing bits but including FCS octets) and
*              were otherwise well formed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryUndersizePkts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryUndersizePktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryUndersizePktsGet (L7_UNIT_CURRENT,
                                       keyetherHistoryIndexValue,
                                       keyetherHistorySampleIndexValue,
                                       &objetherHistoryUndersizePktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryUndersizePkts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryUndersizePktsValue,
                    sizeof (objetherHistoryUndersizePktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryOversizePkts
*
* @purpose Get 'etherHistoryOversizePkts'
*
* @description [etherHistoryOversizePkts]:  The number of packets received
*              during this sampling interval that were longer than 1518 octets
*              (excluding framing bits but including FCS octets) but
*              were otherwise well formed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryOversizePkts (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryOversizePktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryOversizePktsGet (L7_UNIT_CURRENT,
                                      keyetherHistoryIndexValue,
                                      keyetherHistorySampleIndexValue,
                                      &objetherHistoryOversizePktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryOversizePkts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryOversizePktsValue,
                    sizeof (objetherHistoryOversizePktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryFragments
*
* @purpose Get 'etherHistoryFragments'
*
* @description [etherHistoryFragments]:  The total number of packets received
*              during this sampling interval that were less than 64 octets
*              in length (excluding framing bits but including FCS octets)
*              had either a bad Frame Check Sequence (FCS) with an integral
*              num 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryFragments (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryFragmentsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryFragmentsGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                   keyetherHistorySampleIndexValue,
                                   &objetherHistoryFragmentsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryFragments */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryFragmentsValue,
                           sizeof (objetherHistoryFragmentsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryJabbers
*
* @purpose Get 'etherHistoryJabbers'
*
* @description [etherHistoryJabbers]:  The number of packets received during
*              this sampling interval that were longer than 1518 octets
*              (excluding framing bits but including FCS octets), and had
*              either a bad Frame Check Sequence (FCS) with an integral number
*              of 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryJabbers (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryJabbersValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryJabbersGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                 keyetherHistorySampleIndexValue,
                                 &objetherHistoryJabbersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryJabbers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryJabbersValue,
                           sizeof (objetherHistoryJabbersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryCollisions
*
* @purpose Get 'etherHistoryCollisions'
*
* @description [etherHistoryCollisions]:  The best estimate of the total number
*              of collisions on this Ethernet segment during this sampling
*              interval. The value returned will depend on the location
*              of the RMON probe. Section 8.2.1.3 (10BASE-5) and section
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryCollisions (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryCollisionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryCollisionsGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                    keyetherHistorySampleIndexValue,
                                    &objetherHistoryCollisionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryCollisions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryCollisionsValue,
                           sizeof (objetherHistoryCollisionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseetherHistoryStats_etherHistoryUtilization
*
* @purpose Get 'etherHistoryUtilization'
*
* @description [etherHistoryUtilization]:  The best estimate of the mean physical
*              layer network utilization on this interface during
*              this sampling interval, in hundredths of a percent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseetherHistoryStats_etherHistoryUtilization (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistoryIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyetherHistorySampleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objetherHistoryUtilizationValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: etherHistoryIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistoryIndex,
                           (xLibU8_t *) & keyetherHistoryIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistoryIndexValue, kwa1.len);

  /* retrieve key: etherHistorySampleIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_baseetherHistoryStats_etherHistorySampleIndex,
                   (xLibU8_t *) & keyetherHistorySampleIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyetherHistorySampleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEtherHistoryUtilizationGet (L7_UNIT_CURRENT, keyetherHistoryIndexValue,
                                     keyetherHistorySampleIndexValue,
                                     &objetherHistoryUtilizationValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: etherHistoryUtilization */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objetherHistoryUtilizationValue,
                           sizeof (objetherHistoryUtilizationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
