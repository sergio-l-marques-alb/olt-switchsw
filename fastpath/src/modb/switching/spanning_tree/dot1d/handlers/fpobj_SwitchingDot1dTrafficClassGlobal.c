
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1dTrafficClassGlobal.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  09 September 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDot1dTrafficClassGlobal_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassPriority
*
* @purpose Get 'dot1dTrafficClassPriority'
 *@description  [dot1dTrafficClassPriority] traffic class priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1dTrafficClassPriorityValue;
  xLibU32_t port;
  xLibU32_t nextObjdot1dTrafficClassPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  port = L7_ALL_INTERFACES;

  /* retrieve key: dot1dTrafficClassPriority */
  owa.len = sizeof (objdot1dTrafficClassPriorityValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassPriority,
                          (xLibU8_t *) & objdot1dTrafficClassPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdot1dTrafficClassPriorityValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    nextObjdot1dTrafficClassPriorityValue = objdot1dTrafficClassPriorityValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dTrafficClassPriorityValue, owa.len);
    if(nextObjdot1dTrafficClassPriorityValue < DOT1D_MAX_TRAFFIC_CLASS_PRIORITY)
    {
      (nextObjdot1dTrafficClassPriorityValue++);  
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dTrafficClassPriorityValue, owa.len);

  /* return the object value: dot1dTrafficClassPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dTrafficClassPriorityValue,
                           sizeof (nextObjdot1dTrafficClassPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass
*
* @purpose Get 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1dTrafficClassValue;

  xLibU32_t keydot1dTrafficClassPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dTrafficClassPriority */
  owa.len = sizeof (keydot1dTrafficClassPriorityValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassPriority,
                          (xLibU8_t *) & keydot1dTrafficClassPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPriorityValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTrafficClassGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES,keydot1dTrafficClassPriorityValue,
                              &objDot1dTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1dTrafficClassValue, sizeof (objDot1dTrafficClassValue));

  /* return the object value: Dot1dTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTrafficClassValue,
                           sizeof (objDot1dTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass
*
* @purpose List 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass (void *wap, void *bufp)
{
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1dTrafficClassValue;
  xLibU32_t nextObjDot1dTrafficClassValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objDot1dTrafficClassValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass,
                          (xLibU8_t *) & objDot1dTrafficClassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjDot1dTrafficClassValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dTrafficClassValue, owa.len);
    owa.l7rc =
      usmDbGetNextUnknown (L7_UNIT_CURRENT, objDot1dTrafficClassValue,
                           &nextObjDot1dTrafficClassValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDot1dTrafficClassValue, owa.len);

  /* return the object value: Dot1dTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1dTrafficClassValue,
                           sizeof (nextObjDot1dTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass,
      L7_DOT1P_MIN_TRAFFIC_CLASS, L7_DOT1P_MAX_TRAFFIC_CLASS, 1);

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass
*
* @purpose Set 'Dot1dTrafficClass'
 *@description  [Dot1dTrafficClass] traffic class
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dTrafficClassGlobal_Dot1dTrafficClass (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDot1dTrafficClassValue;

  xLibU32_t keydot1dTrafficClassPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dTrafficClass */
  owa.len = sizeof (objDot1dTrafficClassValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dTrafficClassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dTrafficClassValue, owa.len);

  /* retrieve key: dot1dTrafficClassPriority */
  owa.len = sizeof (keydot1dTrafficClassPriorityValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassPriority,
                          (xLibU8_t *) & keydot1dTrafficClassPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dTrafficClassPriorityValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1dTrafficClassSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, keydot1dTrafficClassPriorityValue,
                              objDot1dTrafficClassValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dTrafficClass_dot1dTrafficClassRestoreDefaults
*
* @purpose Set 'Restore Defaults'
*
* @description [CosQueueIntfRestoreDefaults]: Restore egress shaping rate
*              for specified interface to default value
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dTrafficClassGlobal_dot1dTrafficClassRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
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

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(objDot1dTrafficClassRestoreDefaultsValue != 0)
  {
    for (priority = L7_DOT1P_MIN_PRIORITY; priority <= L7_DOT1P_MAX_PRIORITY; priority++)
    {
      if (usmDbDot1dDefaultTrafficClassGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, priority, &queueID) == L7_SUCCESS)
      {
        owa.l7rc = usmDbDot1dTrafficClassSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, priority, queueID);
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

