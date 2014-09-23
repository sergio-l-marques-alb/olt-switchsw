
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basetempSensorStatusTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  25 September 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basetempSensorStatusTable_obj.h"
#include "usmdb_boxs_api.h"

/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_unitIndex
*
* @purpose Get 'unitIndex'
 *@description  [unitIndex] unitIndex is the value of valid stack number in the
* stack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_unitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objunitIndexValue;
  xLibU32_t nextObjunitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (objunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & objunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbUnitMgrStackMemberGetFirst(&nextObjunitIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objunitIndexValue, owa.len);
    owa.l7rc = usmDbUnitMgrStackMemberGetNext(objunitIndexValue, &nextObjunitIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjunitIndexValue, owa.len);

  /* return the object value: unitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjunitIndexValue,
                           sizeof (nextObjunitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_tempSensorIndex
*
* @purpose Get 'tempSensorIndex'
 *@description  [tempSensorIndex] Index value used to identify the temperature
* sensor for the given stack member.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_tempSensorIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objtempSensorIndexValue;
  xLibU32_t nextObjtempSensorIndexValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t numTempSensors;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: tempSensorIndex */
  owa.len = sizeof (objtempSensorIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_tempSensorIndex,
                          (xLibU8_t *) & objtempSensorIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjtempSensorIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objtempSensorIndexValue, owa.len);
    if (usmDbBoxsNumOfTempSensorsGet(keyunitIndexValue, &numTempSensors) == L7_SUCCESS)
    {
      if (objtempSensorIndexValue < numTempSensors)
      {
        nextObjtempSensorIndexValue = objtempSensorIndexValue + 1;
        owa.l7rc = L7_SUCCESS;
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjtempSensorIndexValue, owa.len);

  /* return the object value: tempSensorIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjtempSensorIndexValue,
                           sizeof (nextObjtempSensorIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_tempSensorType
*
* @purpose Get 'tempSensorType'
 *@description  [tempSensorType] value indicating whether the sensor module is
* fixed or removable.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_tempSensorType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objtempSensorTypeValue;
  xLibU32_t objtempSensorStateValue;
  xLibU32_t objtempSensorValueValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keytempSensorIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: tempSensorIndex */
  owa.len = sizeof (keytempSensorIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_tempSensorIndex,
                          (xLibU8_t *) & keytempSensorIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytempSensorIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsTempSensorStatusGet(keyunitIndexValue, keytempSensorIndexValue, 
                                          &objtempSensorTypeValue, &objtempSensorStateValue, &objtempSensorValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objtempSensorTypeValue, sizeof (objtempSensorTypeValue));

  /* return the object value: tempSensorType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtempSensorTypeValue,
                           sizeof (objtempSensorTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_tempSensorState
*
* @purpose Get 'tempSensorState'
 *@description  [tempSensorState] Status indicating whether the temperature
* sensor working condition.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_tempSensorState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objtempSensorStateValue;
  xLibU32_t objtempSensorTypeValue;
  xLibU32_t objtempSensorValueValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keytempSensorIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: tempSensorIndex */
  owa.len = sizeof (keytempSensorIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_tempSensorIndex,
                          (xLibU8_t *) & keytempSensorIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytempSensorIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsTempSensorStatusGet(keyunitIndexValue, keytempSensorIndexValue,
                                          &objtempSensorTypeValue, &objtempSensorStateValue, &objtempSensorValueValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objtempSensorStateValue, sizeof (objtempSensorStateValue));

  /* return the object value: tempSensorState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtempSensorStateValue,
                           sizeof (objtempSensorStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_tempSensorValue
*
* @purpose Get 'tempSensorValue'
 *@description  [tempSensorValue] value to indicate the temperature of the CPU
* and MAC.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_tempSensorValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objtempSensorTypeValue;
  xLibU32_t objtempSensorValueValue;
  xLibU32_t objtempSensorStateValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keytempSensorIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: tempSensorIndex */
  owa.len = sizeof (keytempSensorIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_tempSensorIndex,
                          (xLibU8_t *) & keytempSensorIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytempSensorIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsTempSensorStatusGet(keyunitIndexValue, keytempSensorIndexValue,
                                          &objtempSensorTypeValue, &objtempSensorStateValue, &objtempSensorValueValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objtempSensorValueValue, sizeof (objtempSensorValueValue));

  /* return the object value: tempSensorValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtempSensorValueValue,
                           sizeof (objtempSensorValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basetempSensorStatusTable_GlobalTempValue
*
* @purpose Get 'GlobalTempValue'
 *@description  [GlobalTempValue] Get the general temperature of the switch. 
 * If we have several temp sensors.this will be the highest value (the most dangerous)
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetempSensorStatusTable_GlobalTempValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalTempValue;

  xLibU32_t keyunitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basetempSensorStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsGeneralTempGet(keyunitIndexValue,  &objGlobalTempValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalTempValue, sizeof (objGlobalTempValue));

  /* return the object value: tempSensorValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalTempValue,
                           sizeof (objGlobalTempValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

