
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseRMONAlarmControl.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  17 October 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseRMONAlarmControl_obj.h"
#include "usmdb_mib_rmon_api.h"
#include "rmon_exports.h"

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmFreeIndex
*
* @purpose Get 'AlarmFreeEntry'
*
* @description  An index that uniquely identifies an entry in the alarm 
*              table. Each such entry defines a set of samples at a particular
*              interval for an interface on the device.
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmFreeIndex (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t i;
  xLibU32_t nextObjAlarmFreeIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  for(i = 1; i <= L7_RMON_ALARM_ENTRY_MAX_NUM; i++)
  {
    if ( usmDbAlarmEntryGet(L7_UNIT_CURRENT, i ) != L7_SUCCESS )
      break;
  }
  if ( i <= L7_RMON_ALARM_ENTRY_MAX_NUM)
    nextObjAlarmFreeIndexValue = i;
  else
    nextObjAlarmFreeIndexValue = 1;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAlarmFreeIndexValue, owa.len);

  /* return the object value: historyControlFreeEntry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAlarmFreeIndexValue,
                           sizeof (nextObjAlarmFreeIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmEntryIndex
*
* @purpose Get 'AlarmEntryIndex'
 *@description  [AlarmEntryIndex]  An index that uniquely identifies an entry in
* the  Alarm table. Entry is a list of parameters that set up a
* periodic checking for alarm conditions.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmEntryIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmEntryIndexValue;
  xLibU32_t nextObjAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (objAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & objAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAlarmEntryIndexValue = 0;
    owa.l7rc = usmDbAlarmEntryNext(L7_UNIT_CURRENT, &nextObjAlarmEntryIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAlarmEntryIndexValue, owa.len);
    nextObjAlarmEntryIndexValue = objAlarmEntryIndexValue;
    owa.l7rc = usmDbAlarmEntryNext(L7_UNIT_CURRENT, &nextObjAlarmEntryIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAlarmEntryIndexValue, owa.len);

  /* return the object value: AlarmEntryIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAlarmEntryIndexValue,
                           sizeof (nextObjAlarmEntryIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmObjectID
*
* @purpose Get 'AlarmObjectID'
 *@description  [AlarmObjectID]  This object identifies the indicates the object
* identifier of the variable to be sampled.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmObjectID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAlarmObjectIDValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmVariableGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmObjectIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAlarmObjectIDValue, strlen (objAlarmObjectIDValue));

  /* return the object value: AlarmObjectID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAlarmObjectIDValue,
                           strlen (objAlarmObjectIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmObjectID
*
* @purpose Set 'AlarmObjectID'
 *@description  [AlarmObjectID]  This object identifies the indicates the object
* identifier of the variable to be sampled.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmObjectID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAlarmObjectIDValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmObjectID */
  owa.len = sizeof (objAlarmObjectIDValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAlarmObjectIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAlarmObjectIDValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmVariableSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmObjectIDValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmCounterValue
*
* @purpose Get 'AlarmCounterValue'
 *@description  [AlarmCounterValue] This object indicates the alarm value, the
* value of statistics during the last sample period.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmCounterValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmCounterValueValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmValueGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmCounterValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmCounterValueValue, sizeof (objAlarmCounterValueValue));

  /* return the object value: AlarmCounterValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmCounterValueValue,
                           sizeof (objAlarmCounterValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmSampleType
*
* @purpose Get 'AlarmSampleType'
 *@description  [AlarmSampleType]  This object indicates the alarm sample type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmSampleType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmSampleTypeValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmSampleTypeGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmSampleTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmSampleTypeValue, sizeof (objAlarmSampleTypeValue));

  /* return the object value: AlarmSampleType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmSampleTypeValue,
                           sizeof (objAlarmSampleTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseRMONAlarmControl_AlarmSampleType 
*
* @purpose Get 'baseRMONAlarmControl_AlarmSampleType'
*
* @description  The type of notification that the probe will make about this
*              event. In the case of log, an entry is made in the log table
*              for each event. In the case of snmp-trap, an SNMP trap is sent
*              to one or more management stations.*
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseRMONAlarmControl_AlarmSampleType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAlarmSampleTypeValue;
  xLibU32_t nexyObjAlarmSampleTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmSampleType,
                          (xLibU8_t *) &objAlarmSampleTypeValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nexyObjAlarmSampleTypeValue = L7_XUI_SAMPLE_TYPE_ABSOLUTE;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAlarmSampleTypeValue, owa.len);
    if (objAlarmSampleTypeValue == L7_XUI_SAMPLE_TYPE_ABSOLUTE)  
    {
      nexyObjAlarmSampleTypeValue = objAlarmSampleTypeValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nexyObjAlarmSampleTypeValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nexyObjAlarmSampleTypeValue,
                    sizeof (objAlarmSampleTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmSampleType
*
* @purpose Set 'AlarmSampleType'
 *@description  [AlarmSampleType]  This object indicates the alarm sample type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmSampleType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmSampleTypeValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmSampleType */
  owa.len = sizeof (objAlarmSampleTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmSampleTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmSampleTypeValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmSampleTypeSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmSampleTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmRisingThld
*
* @purpose Get 'AlarmRisingThld'
 *@description  [AlarmRisingThld] This object indicates the rising threshold for
* the sampled statistics. When the current sampled value is
* greater than or equal to this threshold, and value at last sampling
* interval was less than this threshold, a single event will be
* generated.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmRisingThld (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmRisingThldValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmRisingThresholdGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmRisingThldValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmRisingThldValue, sizeof (objAlarmRisingThldValue));

  /* return the object value: AlarmRisingThld */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmRisingThldValue,
                           sizeof (objAlarmRisingThldValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmRisingThld
*
* @purpose Set 'AlarmRisingThld'
 *@description  [AlarmRisingThld] This object indicates the rising threshold for
* the sampled statistics. When the current sampled value is
* greater than or equal to this threshold, and value at last sampling
* interval was less than this threshold, a single event will be
* generated.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmRisingThld (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmRisingThldValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmRisingThld */
  owa.len = sizeof (objAlarmRisingThldValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmRisingThldValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmRisingThldValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmRisingThresholdSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmRisingThldValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmRisingEvent
*
* @purpose Get 'AlarmRisingEvent'
 *@description  [AlarmRisingEvent] This object indicates the index of eventEntry
* that is used when a rising threshold is crossed.If this value is
* zero, no associated event will be generated,as zero is not a
* valid event index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmRisingEvent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmRisingEventValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmRisingEventIndexGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmRisingEventValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmRisingEventValue, sizeof (objAlarmRisingEventValue));

  /* return the object value: AlarmRisingEvent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmRisingEventValue,
                           sizeof (objAlarmRisingEventValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmRisingEvent
*
* @purpose Set 'AlarmRisingEvent'
 *@description  [AlarmRisingEvent] This object indicates the index of eventEntry
* that is used when a rising threshold is crossed.If this value is
* zero, no associated event will be generated,as zero is not a
* valid event index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmRisingEvent (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmRisingEventValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmRisingEvent */
  owa.len = sizeof (objAlarmRisingEventValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmRisingEventValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmRisingEventValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmRisingEventIndexSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmRisingEventValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmFallingThld
*
* @purpose Get 'AlarmFallingThld'
 *@description  [AlarmFallingThld] This object indicates the alarm falling
* threshold for the sampled statistic. When the current sampled value is
* less than or equal to this threshold, and value at last sampling
* interval was greater than this threshold, a single event will be
* generated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmFallingThld (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmFallingThldValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmFallingThresholdGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmFallingThldValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmFallingThldValue, sizeof (objAlarmFallingThldValue));

  /* return the object value: AlarmFallingThld */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmFallingThldValue,
                           sizeof (objAlarmFallingThldValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmFallingThld
*
* @purpose Set 'AlarmFallingThld'
 *@description  [AlarmFallingThld] This object indicates the alarm falling
* threshold for the sampled statistic. When the current sampled value is
* less than or equal to this threshold, and value at last sampling
* interval was greater than this threshold, a single event will be
* generated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmFallingThld (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmFallingThldValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmFallingThld */
  owa.len = sizeof (objAlarmFallingThldValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmFallingThldValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmFallingThldValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmFallingThresholdSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmFallingThldValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmFallingEvent
*
* @purpose Get 'AlarmFallingEvent'
 *@description  [AlarmFallingEvent] This object indicates the index of
* eventEntry that is used when a falling threshold is crossed. If this value
* is zero, no associated event will be generated,as zero is not a
* valid event index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmFallingEvent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmFallingEventValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmFallingEventIndexGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmFallingEventValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmFallingEventValue, sizeof (objAlarmFallingEventValue));

  /* return the object value: AlarmFallingEvent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmFallingEventValue,
                           sizeof (objAlarmFallingEventValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmFallingEvent
*
* @purpose Set 'AlarmFallingEvent'
 *@description  [AlarmFallingEvent] This object indicates the index of
* eventEntry that is used when a falling threshold is crossed. If this value
* is zero, no associated event will be generated,as zero is not a
* valid event index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmFallingEvent (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmFallingEventValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmFallingEvent */
  owa.len = sizeof (objAlarmFallingEventValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmFallingEventValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmFallingEventValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmFallingEventIndexSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmFallingEventValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmStartupAlarm
*
* @purpose Get 'AlarmStartupAlarm'
 *@description  [AlarmStartupAlarm] This object indicates the start up alarm.
* The alarm that may be sent when the entry is first set to valid.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmStartupAlarm (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmStartupAlarmValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmStartupAlarmGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmStartupAlarmValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmStartupAlarmValue, sizeof (objAlarmStartupAlarmValue));

  /* return the object value: AlarmStartupAlarm */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmStartupAlarmValue,
                           sizeof (objAlarmStartupAlarmValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmStartupAlarm
*
* @purpose Set 'AlarmStartupAlarm'
 *@description  [AlarmStartupAlarm] This object indicates the start up alarm.
* The alarm that may be sent when the entry is first set to valid.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmStartupAlarm (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmStartupAlarmValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmStartupAlarm */
  owa.len = sizeof (objAlarmStartupAlarmValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmStartupAlarmValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmStartupAlarmValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmStartupAlarmSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmStartupAlarmValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseRMONAlarmControl_AlarmStartupAlarm 
*
* @purpose Get 'baseRMONAlarmControl_AlarmStartupAlarm'
*
* @description  The type of notification that the probe will make about this
*              event. In the case of log, an entry is made in the log table
*              for each event. In the case of snmp-trap, an SNMP trap is sent
*              to one or more management stations.*
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseRMONAlarmControl_AlarmStartupAlarm (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAlarmStartupAlarmValue;
  xLibU32_t nextObjAlarmStartupAlarmValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmStartupAlarm,
                          (xLibU8_t *) &objAlarmStartupAlarmValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAlarmStartupAlarmValue = L7_XUI_STARTUP_ALARM_RISING;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAlarmStartupAlarmValue, owa.len);
    if ((objAlarmStartupAlarmValue == L7_XUI_STARTUP_ALARM_FALLING) ||  
        (objAlarmStartupAlarmValue == L7_XUI_STARTUP_ALARM_RISING))
    {
      nextObjAlarmStartupAlarmValue = objAlarmStartupAlarmValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAlarmStartupAlarmValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAlarmStartupAlarmValue,
                    sizeof (objAlarmStartupAlarmValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmInterval
*
* @purpose Get 'AlarmInterval'
 *@description  [AlarmInterval] This object the alarm interval in seconds. 
* During this interval data is sampled and compared with rising and
* falling thresholds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmIntervalValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmIntervalGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, &objAlarmIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAlarmIntervalValue, sizeof (objAlarmIntervalValue));

  /* return the object value: AlarmInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmIntervalValue,
                           sizeof (objAlarmIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmInterval
*
* @purpose Set 'AlarmInterval'
 *@description  [AlarmInterval] This object the alarm interval in seconds. 
* During this interval data is sampled and compared with rising and
* falling thresholds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAlarmIntervalValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmInterval */
  owa.len = sizeof (objAlarmIntervalValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAlarmIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmIntervalValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmIntervalSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseRMONAlarmControl_AlarmOwner
*
* @purpose Get 'AlarmOwner'
 *@description  [AlarmOwner] This object indicates the entity that configured
* the entry and is using the resources assigned to it.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmOwner (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAlarmOwnerValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAlarmOwnerGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAlarmOwnerValue, strlen (objAlarmOwnerValue));

  /* return the object value: AlarmOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAlarmOwnerValue, strlen (objAlarmOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmOwner
*
* @purpose Set 'AlarmOwner'
 *@description  [AlarmOwner] This object indicates the entity that configured
* the entry and is using the resources assigned to it.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmOwner (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAlarmOwnerValue;

  xLibU32_t keyAlarmEntryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmOwner */
  owa.len = sizeof (objAlarmOwnerValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAlarmOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAlarmOwnerValue, owa.len);

  /* retrieve key: AlarmEntryIndex */
  owa.len = sizeof (keyAlarmEntryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAlarmOwnerSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, objAlarmOwnerValue);
  if (owa.l7rc == L7_SUCCESS)
  {
     owa.l7rc = usmDbAlarmStatusSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue, 
                                    RMON_ALARM_ENTRY_STATUS_VALID);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
xLibRC_t fpObjGet_baseRMONAlarmControl_AlarmStatus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAlarmEntryIndexValue;
  xLibU32_t objAlarmStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AlarmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbAlarmStatusGet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue,
                                  &objAlarmStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AlarmStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAlarmStatusValue,
                           sizeof (objAlarmStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseRMONAlarmControl_AlarmStatus
*
* @purpose Set 'AlarmStatus'
*
* @description  The status of this Alarm entry. Each instance of the 
*              media-specific table associated with this AlarmEntry 
*              will be deleted by the agent if this AlarmEntry is 
*              not equal to valid(1). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseRMONAlarmControl_AlarmStatus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAlarmStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAlarmEntryIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AlarmStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAlarmStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAlarmStatusValue, owa.len);

  /* retrieve key: AlarmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseRMONAlarmControl_AlarmEntryIndex,
                          (xLibU8_t *) & keyAlarmEntryIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAlarmEntryIndexValue, kwa.len);

  if (objAlarmStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    owa.l7rc =
      usmDbAlarmStatusSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue,
                                    RMON_ALARM_ENTRY_STATUS_CREATEREQUEST);
  } 
  else if (objAlarmStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc =
      usmDbAlarmStatusSet(L7_UNIT_CURRENT, keyAlarmEntryIndexValue,
                                    RMON_ALARM_ENTRY_STATUS_INVALID);
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
