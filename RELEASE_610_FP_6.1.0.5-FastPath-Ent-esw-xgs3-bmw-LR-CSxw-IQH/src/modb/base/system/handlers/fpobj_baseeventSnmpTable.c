/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseeventSnmpTable.c
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
#include "_xe_baseeventSnmpTable_obj.h"
#include "usmdb_mib_rmon_api.h"
#include "rmon_exports.h"

/*******************************************************************************
* @function fpObjGet_baseeventSnmpTable_eventFreeIdx
*
* @purpose Get free event index
*
* @description  An index that uniquely identifies an entry in the event
*              table. Each such entry defines a set of samples at a particular
*              interval for an interface on the device.
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventFreeIdx (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t i;
  xLibU32_t nextObjeventFreeIdxValue;
  FPOBJ_TRACE_ENTER (bufp);

  for(i = 1; i <= L7_RMON_EVENT_ENTRY_MAX_NUM; i++)
  {
    if ( usmDbEventEntryGet(L7_UNIT_CURRENT, i ) != L7_SUCCESS )
      break;
  }
  if ( i < L7_RMON_EVENT_ENTRY_MAX_NUM )
    nextObjeventFreeIdxValue = i;
  else
    nextObjeventFreeIdxValue = 1;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjeventFreeIdxValue, owa.len);

  /* return the object value: historyControlFreeEntry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjeventFreeIdxValue,
                           sizeof (nextObjeventFreeIdxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseeventSnmpTable_eventIndex
*
* @purpose Get 'eventIndex'
*
* @description  An index that uniquely identifies an entry in the event table. 
*              Each such entry defines one event that is to be generated when 
*              the appropriate conditions occur. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventIndexValue;
  xLibU32_t nextObjeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & objeventIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjeventIndexValue = 0;
    owa.l7rc = usmDbEventEntryNext(L7_UNIT_CURRENT, &nextObjeventIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objeventIndexValue, owa.len);
    nextObjeventIndexValue = objeventIndexValue;
    owa.l7rc = usmDbEventEntryNext (L7_UNIT_CURRENT, &nextObjeventIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjeventIndexValue, owa.len);

  /* return the object value: eventIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjeventIndexValue,
                           sizeof (objeventIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseeventSnmpTable_eventDescription
*
* @purpose Get 'eventDescription'
*
* @description  A comment describing this event entry. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventDescription (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventDescriptionGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                       objeventDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objeventDescriptionValue,
                           strlen (objeventDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseeventSnmpTable_eventDescription
*
* @purpose Set 'eventDescription'
*
* @description  A comment describing this event entry. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseeventSnmpTable_eventDescription (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventDescriptionValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: eventDescription */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objeventDescriptionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objeventDescriptionValue, owa.len);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEventDescriptionSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                       objeventDescriptionValue);
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
* @function fpObjGet_baseeventSnmpTable_eventType
*
* @purpose Get 'eventType'
*
* @description  The type of notification that the probe will make about this 
*              event. In the case of log, an entry is made in the log table 
*              for each event. In the case of snmp-trap, an SNMP trap is sent 
*              to one or more management stations. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventTypeGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                &objeventTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objeventTypeValue,
                           sizeof (objeventTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseeventSnmpTable_eventType 
*
* @purpose Get 'baseeventSnmpTable_eventType'
*
* @description  The type of notification that the probe will make about this
*              event. In the case of log, an entry is made in the log table
*              for each event. In the case of snmp-trap, an SNMP trap is sent
*              to one or more management stations.*
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseeventSnmpTable_eventType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventTypeValue;
  xLibU32_t nextObjEventTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventType,
                          (xLibU8_t *) &objeventTypeValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjEventTypeValue = L7_XUI_EVENT_NONE;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objeventTypeValue, owa.len);
    if ((objeventTypeValue == L7_XUI_EVENT_NONE) || (objeventTypeValue == L7_XUI_EVENT_LOG) 
        || (objeventTypeValue == L7_XUI_EVENT_TRAP))
    {
      nextObjEventTypeValue = objeventTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
    else 
    {
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjEventTypeValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjEventTypeValue,
                    sizeof (objeventTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseeventSnmpTable_eventType
*
* @purpose Set 'eventType'
*
* @description  The type of notification that the probe will make about this 
*              event. In the case of log, an entry is made in the log table 
*              for each event. In the case of snmp-trap, an SNMP trap is sent 
*              to one or more management stations. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseeventSnmpTable_eventType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: eventType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objeventTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objeventTypeValue, owa.len);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEventTypeSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                objeventTypeValue);
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
* @function fpObjGet_baseeventSnmpTable_eventCommunity
*
* @purpose Get 'eventCommunity'
*
* @description  If an SNMP trap is to be sent, it will be sent to the SNMP community 
*              specified by this octet string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventCommunity (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventCommunityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventCommunityGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                     objeventCommunityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventCommunity */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objeventCommunityValue,
                           strlen (objeventCommunityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseeventSnmpTable_eventCommunity
*
* @purpose Set 'eventCommunity'
*
* @description  If an SNMP trap is to be sent, it will be sent to the SNMP community 
*              specified by this octet string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseeventSnmpTable_eventCommunity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventCommunityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: eventCommunity */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objeventCommunityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objeventCommunityValue, owa.len);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEventCommunitySet (L7_UNIT_CURRENT, keyeventIndexValue,
                                     objeventCommunityValue);
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
* @function fpObjGet_baseeventSnmpTable_eventLastTimeSent
*
* @purpose Get 'eventLastTimeSent'
*
* @description  The value of sysUpTime at the time this event entry last generated 
*              an event. If this entry has not generated any events, this 
*              value will be zero. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventLastTimeSent (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventLastTimeSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventLastTimeSentGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                        &objeventLastTimeSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventLastTimeSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objeventLastTimeSentValue,
                           sizeof (objeventLastTimeSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseeventSnmpTable_eventOwner
*
* @purpose Get 'eventOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. If this object contains a string 
*              starting with 'monitor' and has associated entries in the log 
*              table, all connected management sta 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventOwner (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventOwnerGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                 objeventOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objeventOwnerValue,
                           strlen (objeventOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseeventSnmpTable_eventOwner
*
* @purpose Set 'eventOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. If this object contains a string 
*              starting with 'monitor' and has associated entries in the log 
*              table, all connected management sta 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseeventSnmpTable_eventOwner (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objeventOwnerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: eventOwner */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objeventOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objeventOwnerValue, owa.len);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEventOwnerSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                 objeventOwnerValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbEventStatusSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                    RMON_EVENT_ENTRY_STATUS_VALID);
  }

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
* @function fpObjGet_baseeventSnmpTable_eventStatus
*
* @purpose Get 'eventStatus'
*
* @description  The status of this event entry. If this object is not equal to 
*              valid(1), all associated log entries shall be deleted by the 
*              agent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseeventSnmpTable_eventStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEventStatusGet (L7_UNIT_CURRENT, keyeventIndexValue,
                                  &objeventStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: eventStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objeventStatusValue,
                           sizeof (objeventStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseeventSnmpTable_eventStatus
*
* @purpose Set 'eventStatus'
*
* @description  The status of this event entry. If this object is not equal to 
*              valid(1), all associated log entries shall be deleted by the 
*              agent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseeventSnmpTable_eventStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objeventStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyeventIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: eventStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objeventStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objeventStatusValue, owa.len);

  /* retrieve key: eventIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseeventSnmpTable_eventIndex,
                          (xLibU8_t *) & keyeventIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyeventIndexValue, kwa.len);

  if (objeventStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    owa.l7rc = usmDbEventStatusSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                    RMON_EVENT_ENTRY_STATUS_CREATEREQUEST);
  }
  else if (objeventStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbEventStatusSet (L7_UNIT_CURRENT, keyeventIndexValue,
                                    RMON_EVENT_ENTRY_STATUS_INVALID);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
