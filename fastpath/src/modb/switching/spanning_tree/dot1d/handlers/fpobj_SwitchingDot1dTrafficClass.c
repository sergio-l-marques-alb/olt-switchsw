
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1dTrafficClass.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  27 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDot1dTrafficClass_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTrafficClass_dot1dTrafficClassPort
*
* @purpose Get 'dot1dTrafficClassPort'
 *@description  [dot1dTrafficClassPort] Traffic class port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTrafficClass_dot1dTrafficClassPort (void *wap, void *bufp)
{

  xLibU32_t objdot1dTrafficClassPortValue;
  xLibU32_t nextObjdot1dTrafficClassPortValue;
  xLibU32_t objdot1dTrafficClassPriorityValue;
  xLibU32_t nextObjdot1dTrafficClassPriorityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (&objdot1dTrafficClassPortValue, 0, sizeof (objdot1dTrafficClassPortValue));
  memset (&nextObjdot1dTrafficClassPortValue, 0, sizeof (nextObjdot1dTrafficClassPortValue));
  memset (&objdot1dTrafficClassPriorityValue, 0, sizeof (objdot1dTrafficClassPriorityValue));
  memset (&nextObjdot1dTrafficClassPriorityValue, 0, sizeof (nextObjdot1dTrafficClassPriorityValue));

  /* retrieve key: dot1dTrafficClassPort */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPort,
                          (xLibU8_t *) & objdot1dTrafficClassPortValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1dTrafficClassEntryNextGet(L7_UNIT_CURRENT,
                                     &nextObjdot1dTrafficClassPortValue,
                                     &nextObjdot1dTrafficClassPriorityValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dTrafficClassPortValue, owa.len);
    nextObjdot1dTrafficClassPortValue = objdot1dTrafficClassPortValue;
    do
    {
      owa.l7rc = usmDbDot1dTrafficClassEntryNextGet(L7_UNIT_CURRENT,
                                      &nextObjdot1dTrafficClassPortValue,
                                      &nextObjdot1dTrafficClassPriorityValue);
    }
    while ((objdot1dTrafficClassPortValue == nextObjdot1dTrafficClassPortValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dTrafficClassPortValue, owa.len);

  /* return the object value: dot1dTrafficClassPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dTrafficClassPortValue,
                           sizeof (objdot1dTrafficClassPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTrafficClass_dot1dTrafficClassPriority
*
* @purpose Get 'dot1dTrafficClassPriority'
 *@description  [dot1dTrafficClassPriority] traffic class priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTrafficClass_dot1dTrafficClassPriority (void *wap, void *bufp)
{

  xLibU32_t objdot1dTrafficClassPortValue;
  xLibU32_t nextObjdot1dTrafficClassPortValue;
  xLibU32_t objdot1dTrafficClassPriorityValue;
  xLibU32_t nextObjdot1dTrafficClassPriorityValue;
  L7_int32 priority = 0;          
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (&objdot1dTrafficClassPortValue, 0, sizeof (objdot1dTrafficClassPortValue));
  memset (&nextObjdot1dTrafficClassPortValue, 0, sizeof (nextObjdot1dTrafficClassPortValue));
  memset (&objdot1dTrafficClassPriorityValue, 0, sizeof (objdot1dTrafficClassPriorityValue));
  memset (&nextObjdot1dTrafficClassPriorityValue, 0, sizeof (nextObjdot1dTrafficClassPriorityValue));
  /* retrieve key: dot1dTrafficClassPort */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPort,
                          (xLibU8_t *) & objdot1dTrafficClassPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dTrafficClassPortValue, owa.len);

  /* retrieve key: dot1dTrafficClassPriority */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPriority,
                          (xLibU8_t *) & objdot1dTrafficClassPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdot1dTrafficClassPortValue = objdot1dTrafficClassPortValue;
    priority = 0;

    owa.l7rc = usmDbDot1dTrafficClassEntryGet(L7_UNIT_CURRENT,
                                     nextObjdot1dTrafficClassPortValue,
                                     priority);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dTrafficClassPriorityValue, owa.len);

    nextObjdot1dTrafficClassPortValue = objdot1dTrafficClassPortValue;
    priority = objdot1dTrafficClassPriorityValue;

    owa.l7rc = usmDbDot1dTrafficClassEntryNextGet(L7_UNIT_CURRENT,
                                    &nextObjdot1dTrafficClassPortValue,
                                    &priority);

  }

  if ((objdot1dTrafficClassPortValue != nextObjdot1dTrafficClassPortValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjdot1dTrafficClassPriorityValue = priority;
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dTrafficClassPriorityValue, owa.len);

  /* return the object value: dot1dTrafficClassPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dTrafficClassPriorityValue,
                           sizeof (objdot1dTrafficClassPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTrafficClass_Dot1dTrafficClass
*
* @purpose Get 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTrafficClass_Dot1dTrafficClass (void *wap, void *bufp)
{

  fpObjWa_t kwadot1dTrafficClassPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dTrafficClassPortValue;
  fpObjWa_t kwadot1dTrafficClassPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dTrafficClassPriorityValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTrafficClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dTrafficClassPort */
  kwadot1dTrafficClassPort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPort,
                   (xLibU8_t *) & keydot1dTrafficClassPortValue, &kwadot1dTrafficClassPort.len);
  if (kwadot1dTrafficClassPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTrafficClassPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTrafficClassPort);
    return kwadot1dTrafficClassPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPortValue, kwadot1dTrafficClassPort.len);

  /* retrieve key: dot1dTrafficClassPriority */
  kwadot1dTrafficClassPriority.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPriority,
                   (xLibU8_t *) & keydot1dTrafficClassPriorityValue,
                   &kwadot1dTrafficClassPriority.len);
  if (kwadot1dTrafficClassPriority.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTrafficClassPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTrafficClassPriority);
    return kwadot1dTrafficClassPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPriorityValue,
                           kwadot1dTrafficClassPriority.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTrafficClassGet(L7_UNIT_CURRENT, keydot1dTrafficClassPortValue,
                              keydot1dTrafficClassPriorityValue, &objDot1dTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTrafficClassValue,
                           sizeof (objDot1dTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dTrafficClass_Dot1dTrafficClass
*
* @purpose Set 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dTrafficClass_Dot1dTrafficClass (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTrafficClassValue;

  fpObjWa_t kwadot1dTrafficClassPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dTrafficClassPortValue;
  fpObjWa_t kwadot1dTrafficClassPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dTrafficClassPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dTrafficClass */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dTrafficClassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dTrafficClassValue, owa.len);

  /* retrieve key: dot1dTrafficClassPort */
  kwadot1dTrafficClassPort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPort,
                   (xLibU8_t *) & keydot1dTrafficClassPortValue, &kwadot1dTrafficClassPort.len);
  if (kwadot1dTrafficClassPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTrafficClassPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTrafficClassPort);
    return kwadot1dTrafficClassPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPortValue, kwadot1dTrafficClassPort.len);

  /* retrieve key: dot1dTrafficClassPriority */
  kwadot1dTrafficClassPriority.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPriority,
                   (xLibU8_t *) & keydot1dTrafficClassPriorityValue,
                   &kwadot1dTrafficClassPriority.len);
  if (kwadot1dTrafficClassPriority.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTrafficClassPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTrafficClassPriority);
    return kwadot1dTrafficClassPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPriorityValue,
                           kwadot1dTrafficClassPriority.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1dTrafficClassSet (L7_UNIT_CURRENT, keydot1dTrafficClassPortValue,
                              keydot1dTrafficClassPriorityValue, objDot1dTrafficClassValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingDot1dTrafficClass_Dot1dTrafficClass
*
* @purpose List 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingDot1dTrafficClass_Dot1dTrafficClass (void *wap, void *bufp)
{
return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_SwitchingDot1dTrafficClass_Dot1dTrafficClass,
      L7_DOT1P_MIN_TRAFFIC_CLASS, L7_DOT1P_MAX_TRAFFIC_CLASS, 1); 

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dTrafficClass_dot1dTrafficClassRestoreDefaults
*
* @purpose Set 'CosMapIntfIpPrecRestoreDefaults'
*
* @description [CosMapIntfIpPrecRestoreDefaults] Restore IP Precedence Queue Mapping to defaults for a specified interface
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dTrafficClass_dot1dTrafficClassRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwadot1dTrafficClassPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dTrafficClassPortValue;
  xLibU32_t objDot1dTrafficClassRestoreDefaultsValue;
  xLibU32_t priority=0;
  xLibU32_t queueID=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIntfIpPrecRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dTrafficClassRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dTrafficClassRestoreDefaultsValue, owa.len);

  /* retrieve key: CosMapIpPrecIntfIndex */
  kwadot1dTrafficClassPort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClass_dot1dTrafficClassPort,
                   (xLibU8_t *) & keydot1dTrafficClassPortValue, &kwadot1dTrafficClassPort.len);
  if (kwadot1dTrafficClassPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTrafficClassPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTrafficClassPort);
    return kwadot1dTrafficClassPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPortValue, kwadot1dTrafficClassPort.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(objDot1dTrafficClassRestoreDefaultsValue != 0)
  {
    for (priority = L7_DOT1P_MIN_PRIORITY; priority <= L7_DOT1P_MAX_PRIORITY; priority++)
    {
      if (usmDbDot1dDefaultTrafficClassGet(L7_UNIT_CURRENT, keydot1dTrafficClassPortValue, priority, &queueID) == L7_SUCCESS)
      {
        owa.l7rc = usmDbDot1dTrafficClassSet(L7_UNIT_CURRENT, keydot1dTrafficClassPortValue, priority, queueID);
        if (owa.l7rc != L7_SUCCESS)
        {
          break;
        }
      }
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_RESTORE_TRAFFIC_CLASS_FAILED;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

