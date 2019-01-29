
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basefanStatusTable.c
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
#include "_xe_basefanStatusTable_obj.h"
#include "usmdb_boxs_api.h"

/*******************************************************************************
* @function fpObjGet_basefanStatusTable_unitIndex
*
* @purpose Get 'unitIndex'
 *@description  [unitIndex] unitIndex is the value of valid unit number in the
* stack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_unitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objunitIndexValue;
  xLibU32_t nextObjunitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (objunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
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
* @function fpObjGet_basefanStatusTable_fanIndex
*
* @purpose Get 'fanIndex'
 *@description  [fanIndex] fan index value used to identify fan for the given
* stack member.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_fanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfanIndexValue;
  xLibU32_t nextObjfanIndexValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t numFans;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: fanIndex */
  owa.len = sizeof (objfanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_fanIndex,
                          (xLibU8_t *) & objfanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjfanIndexValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objfanIndexValue, owa.len);
    if (usmDbBoxsNumOfFansGet(keyunitIndexValue, &numFans) == L7_SUCCESS)
    {
      if (objfanIndexValue < numFans)
      {
        nextObjfanIndexValue = objfanIndexValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjfanIndexValue, owa.len);

  /* return the object value: fanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjfanIndexValue,
                           sizeof (nextObjfanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basefanStatusTable_fanType
*
* @purpose Get 'fanType'
 *@description  [fanType] value indicating whether the fan module is fixed or
* removable.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_fanType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfanTypeValue;
  xLibU32_t objfanStateValue;
  xLibU32_t objfanSpeedValue;
  xLibU32_t objfanDutyValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keyfanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: fanIndex */
  owa.len = sizeof (keyfanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_fanIndex,
                          (xLibU8_t *) & keyfanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsFanStatusGet(keyunitIndexValue, keyfanIndexValue, &objfanTypeValue,
                                   &objfanStateValue, &objfanSpeedValue, &objfanDutyValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objfanTypeValue, sizeof (objfanTypeValue));

  /* return the object value: fanType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objfanTypeValue, sizeof (objfanTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basefanStatusTable_fanState
*
* @purpose Get 'fanState'
 *@description  [fanState] Status indicating whether the fan is running or
* stopped.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_fanState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfanStateValue;
  xLibU32_t objfanSpeedValue;
  xLibU32_t objfanTypeValue;
  xLibU32_t objfanDutyValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keyfanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: fanIndex */
  owa.len = sizeof (keyfanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_fanIndex,
                          (xLibU8_t *) & keyfanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsFanStatusGet(keyunitIndexValue, keyfanIndexValue, &objfanTypeValue,
                                   &objfanStateValue, &objfanSpeedValue, &objfanDutyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objfanStateValue, sizeof (objfanStateValue));

  /* return the object value: fanState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objfanStateValue, sizeof (objfanStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basefanStatusTable_fanSpeed
*
* @purpose Get 'fanSpeed'
 *@description  [fanSpeed] value indicating the speed of the fan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_fanSpeed (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfanSpeedValue;
  xLibU32_t objfanTypeValue;
  xLibU32_t objfanStateValue;
  xLibU32_t objfanDutyValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keyfanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: fanIndex */
  owa.len = sizeof (keyfanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_fanIndex,
                          (xLibU8_t *) & keyfanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsFanStatusGet(keyunitIndexValue, keyfanIndexValue, &objfanTypeValue,
                                   &objfanStateValue, &objfanSpeedValue, &objfanDutyValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objfanSpeedValue, sizeof (objfanSpeedValue));

  /* return the object value: fanSpeed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objfanSpeedValue, sizeof (objfanSpeedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basefanStatusTable_fanDuty
*
* @purpose Get 'fanDuty'
 *@description  [fanDuty] value indicating duty cycle of the fan.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_fanDuty (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfanDutyValue;
  xLibU32_t objfanSpeedValue;
  xLibU32_t objfanStateValue;
  xLibU32_t objfanTypeValue;

  xLibU32_t keyunitIndexValue;
  xLibU32_t keyfanIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* retrieve key: fanIndex */
  owa.len = sizeof (keyfanIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_fanIndex,
                          (xLibU8_t *) & keyfanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsFanStatusGet(keyunitIndexValue, keyfanIndexValue, &objfanTypeValue,
                                   &objfanStateValue, &objfanSpeedValue, &objfanDutyValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objfanDutyValue, sizeof (objfanDutyValue));

  /* return the object value: fanDuty */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objfanDutyValue, sizeof (objfanDutyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basefanStatusTable_GlobalFanStatus
*
* @purpose Get 'GlobalFanStatus'
 *@description  [GlobalFanStatus] Global fan Status of the unit.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basefanStatusTable_GlobalFanStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalFanStatusValue;

  xLibU32_t keyunitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: unitIndex */
  owa.len = sizeof (keyunitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basefanStatusTable_unitIndex,
                          (xLibU8_t *) & keyunitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyunitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbBoxsGlobalStatusGet(keyunitIndexValue,  &objGlobalFanStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalFanStatusValue, sizeof (objGlobalFanStatusValue));

  /* return the object value: GlobalFanStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalFanStatusValue, sizeof (objGlobalFanStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

