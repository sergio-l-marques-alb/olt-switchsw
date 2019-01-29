/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot3adAggPort.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot3ad-object.xml
*
* @create  5 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot3adAggPort_obj.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPort
*
* @purpose Get 'AggPort'
*
* @description [AggPort]: The aggregator port 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortValue;
  xLibU32_t nextObjAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & objAggPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
     objAggPortValue = 0;
     owa.l7rc = usmDbDot3adAggPortIndexGetNext(L7_UNIT_CURRENT, objAggPortValue, &nextObjAggPortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAggPortValue, owa.len);
    owa.l7rc = usmDbDot3adAggPortIndexGetNext(L7_UNIT_CURRENT, objAggPortValue, &nextObjAggPortValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAggPortValue, owa.len);

  /* return the object value: AggPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAggPortValue,
                           sizeof (objAggPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorAdminKey
*
* @purpose Get 'AggPortActorAdminKey'
*
* @description [AggPortActorAdminKey]: Current administrative value of the
*              key for the aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorAdminKey (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorAdminKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggPortActorAdminKeyGet (L7_UNIT_CURRENT, keyAggPortValue,
                                                 &objAggPortActorAdminKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorAdminKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortActorAdminKeyValue,
                           sizeof (objAggPortActorAdminKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortActorAdminKey
*
* @purpose Set 'AggPortActorAdminKey'
*
* @description [AggPortActorAdminKey]: Current administrative value of the
*              key for the aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortActorAdminKey (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorAdminKeyValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortActorAdminKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortActorAdminKeyValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortActorAdminKeyValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot3adAggPortActorAdminKeySet (L7_UNIT_CURRENT, keyAggPortValue,
                                                 objAggPortActorAdminKeyValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorAdminState
*
* @purpose Get 'AggPortActorAdminState'
*
* @description [AggPortActorAdminState]: The 8 bits corresponding to the admin
*              values of actor_state 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorAdminState (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorAdminStateValue;
  xLibU8_t aggPortActorAdminStateVal = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorOperStateGet( L7_UNIT_CURRENT , keyAggPortValue, &aggPortActorAdminStateVal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(( aggPortActorAdminStateVal & 0x2) == 0x2)
  {
    objAggPortActorAdminStateValue = L7_XUI_LACP_TIMEOUT_SHORT;
  }
  else
  {
    objAggPortActorAdminStateValue = L7_XUI_LACP_TIMEOUT_LONG;
  }


  /* return the object value: AggPortActorAdminState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objAggPortActorAdminStateValue,
                           sizeof (objAggPortActorAdminStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortActorAdminState
*
* @purpose Set 'AggPortActorAdminState'
*
* @description [AggPortActorAdminState]: The 8 bits corresponding to the admin
*              values of actor_state 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortActorAdminState (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorAdminStateValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  xLibU8_t  aggPortActorState=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortActorAdminState */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objAggPortActorAdminStateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAggPortActorAdminStateValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */

  if (usmDbDot3adAggPortActorAdminStateGet( L7_UNIT_CURRENT, keyAggPortValue, &aggPortActorState) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if( objAggPortActorAdminStateValue ==  L7_XUI_LACP_TIMEOUT_SHORT)
  {
    aggPortActorState = aggPortActorState | 0x2;
  }
  else
  {
    aggPortActorState = aggPortActorState  & (~(0x2));
  }
 
  owa.l7rc =
    usmDbDot3adAggPortActorAdminStateSet (L7_UNIT_CURRENT, keyAggPortValue,
                                           &aggPortActorState);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorOperKey
*
* @purpose Get 'AggPortActorOperKey'
*
* @description [AggPortActorOperKey]: Thecurrent operational value of the
*              key for the aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorOperKey (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorOperKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorOperKeyGet (L7_UNIT_CURRENT, keyAggPortValue,
                                       &objAggPortActorOperKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorOperKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortActorOperKeyValue,
                           sizeof (objAggPortActorOperKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortActorOperKey
*
* @purpose Set 'AggPortActorOperKey'
*
* @description [AggPortActorOperKey]: Thecurrent operational value of the
*              key for the aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortActorOperKey (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorOperKeyValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortActorOperKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortActorOperKeyValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortActorOperKeyValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortActorOperKeySet (L7_UNIT_CURRENT, keyAggPortValue,
                                       objAggPortActorOperKeyValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorOperState
*
* @purpose Get 'AggPortActorOperState'
*
* @description [AggPortActorOperState]: The 8 bits corresponding to the admin
*              values of actor_state 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorOperState (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortActorOperStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorOperStateGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         objAggPortActorOperStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorOperState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPortActorOperStateValue,
                           strlen (objAggPortActorOperStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorPort
*
* @purpose Get 'AggPortActorPort'
*
* @description [AggPortActorPort]: The port number locally assigned to this
*              aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorPort (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggPortActorPortGet (L7_UNIT_CURRENT, keyAggPortValue,
                                             &objAggPortActorPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortActorPortValue,
                           sizeof (objAggPortActorPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorPortPriority
*
* @purpose Get 'AggPortActorPortPriority'
*
* @description [AggPortActorPortPriority]: The priority assigned to this aggregation
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorPortPriority (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorPortPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorPortPriorityGet (L7_UNIT_CURRENT, keyAggPortValue,
                                            &objAggPortActorPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorPortPriority */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortActorPortPriorityValue,
                    sizeof (objAggPortActorPortPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortActorPortPriority
*
* @purpose Set 'AggPortActorPortPriority'
*
* @description [AggPortActorPortPriority]: The priority assigned to this aggregation
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortActorPortPriority (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorPortPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortActorPortPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortActorPortPriorityValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortActorPortPriorityValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortActorPortPrioritySet (L7_UNIT_CURRENT, keyAggPortValue,
                                            objAggPortActorPortPriorityValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorSystemID
*
* @purpose Get 'AggPortActorSystemID'
*
* @description [AggPortActorSystemID]: The system id for the system that contains
*              this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorSystemID (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortActorSystemIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorSystemIDGet (L7_UNIT_CURRENT, keyAggPortValue,
                                        objAggPortActorSystemIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorSystemID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPortActorSystemIDValue,
                           strlen (objAggPortActorSystemIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortActorSystemPriority
*
* @purpose Get 'AggPortActorSystemPriority'
*
* @description [AggPortActorSystemPriority]: The priority value associated
*              with the system actor's ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortActorSystemPriority (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorSystemPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortActorSystemPriorityGet (L7_UNIT_CURRENT, keyAggPortValue,
                                              &objAggPortActorSystemPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortActorSystemPriority */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortActorSystemPriorityValue,
                    sizeof (objAggPortActorSystemPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortActorSystemPriority
*
* @purpose Set 'AggPortActorSystemPriority'
*
* @description [AggPortActorSystemPriority]: The priority value associated
*              with the system actor's ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortActorSystemPriority (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortActorSystemPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortActorSystemPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortActorSystemPriorityValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortActorSystemPriorityValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortActorSystemPrioritySet (L7_UNIT_CURRENT, keyAggPortValue,
                                              objAggPortActorSystemPriorityValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortAggregateOrIndividual
*
* @purpose Get 'AggPortAggregateOrIndividual'
*
* @description [AggPortAggregateOrIndividual]: Determine whether an aggregation
*              port is able to aggregate 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortAggregateOrIndividual (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortAggregateOrIndividualValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortAggregateOrIndividualGet (L7_UNIT_CURRENT,
                                                keyAggPortValue,
                                                &objAggPortAggregateOrIndividualValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortAggregateOrIndividual */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortAggregateOrIndividualValue,
                    sizeof (objAggPortAggregateOrIndividualValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortAttachedAggID
*
* @purpose Get 'AggPortAttachedAggID'
*
* @description [AggPortAttachedAggID]: The identifier value of the aggregrator
*              this port is currently attached to 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortAttachedAggID (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortAttachedAggIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortAttachedAggIDGet (L7_UNIT_CURRENT, keyAggPortValue,
                                        &objAggPortAttachedAggIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortAttachedAggID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortAttachedAggIDValue,
                           sizeof (objAggPortAttachedAggIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortList
*
* @purpose Get 'AggPortList'
*
* @description [AggPortList]: The list of ports included in an aggregator
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortList (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortListValue;
  L7_uint32 length;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggPortListGet (L7_UNIT_CURRENT, keyAggPortValue,
                                        objAggPortListValue, &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPortListValue,
                           strlen (objAggPortListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminKey
*
* @purpose Get 'AggPortPartnerAdminKey'
*
* @description [AggPortPartnerAdminKey]: The aggregation port's partner key
*              (administrative) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminKey (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminKeyGet (L7_UNIT_CURRENT, keyAggPortValue,
                                          &objAggPortPartnerAdminKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortPartnerAdminKeyValue,
                           sizeof (objAggPortPartnerAdminKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminKey
*
* @purpose Set 'AggPortPartnerAdminKey'
*
* @description [AggPortPartnerAdminKey]: The aggregation port's partner key
*              (administrative) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminKey (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminKeyValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortPartnerAdminKeyValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortPartnerAdminKeyValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminKeySet (L7_UNIT_CURRENT, keyAggPortValue,
                                          objAggPortPartnerAdminKeyValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminPort
*
* @purpose Get 'AggPortPartnerAdminPort'
*
* @description [AggPortPartnerAdminPort]: The administrative value of the
*              port number for the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminPort (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminPortGet (L7_UNIT_CURRENT, keyAggPortValue,
                                           &objAggPortPartnerAdminPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortPartnerAdminPortValue,
                           sizeof (objAggPortPartnerAdminPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminPort
*
* @purpose Set 'AggPortPartnerAdminPort'
*
* @description [AggPortPartnerAdminPort]: The administrative value of the
*              port number for the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminPort (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminPortValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggPortPartnerAdminPortValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortPartnerAdminPortValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminPortSet (L7_UNIT_CURRENT, keyAggPortValue,
                                           objAggPortPartnerAdminPortValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminPortPriority
*
* @purpose Get 'AggPortPartnerAdminPortPriority'
*
* @description [AggPortPartnerAdminPortPriority]: The administrative value
*              of the port priority of the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminPortPriority (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminPortPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminPortPriorityGet (L7_UNIT_CURRENT,
                                                   keyAggPortValue,
                                                   &objAggPortPartnerAdminPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminPortPriority */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objAggPortPartnerAdminPortPriorityValue,
                    sizeof (objAggPortPartnerAdminPortPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminPortPriority
*
* @purpose Set 'AggPortPartnerAdminPortPriority'
*
* @description [AggPortPartnerAdminPortPriority]: The administrative value
*              of the port priority of the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminPortPriority (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminPortPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminPortPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objAggPortPartnerAdminPortPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortPartnerAdminPortPriorityValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminPortPrioritySet (L7_UNIT_CURRENT,
                                                   keyAggPortValue,
                                                   objAggPortPartnerAdminPortPriorityValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminState
*
* @purpose Get 'AggPortPartnerAdminState'
*
* @description [AggPortPartnerAdminState]: The 8 bits corresponding to the
*              admin values of the partner_state 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminState (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerAdminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminStateGet (L7_UNIT_CURRENT, keyAggPortValue,
                                            objAggPortPartnerAdminStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPortPartnerAdminStateValue,
                           strlen (objAggPortPartnerAdminStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminState
*
* @purpose Set 'AggPortPartnerAdminState'
*
* @description [AggPortPartnerAdminState]: The 8 bits corresponding to the
*              admin values of the partner_state 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminState (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerAdminStateValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminState */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAggPortPartnerAdminStateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAggPortPartnerAdminStateValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminStateSet (L7_UNIT_CURRENT, keyAggPortValue,
                                            objAggPortPartnerAdminStateValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminSystemID
*
* @purpose Get 'AggPortPartnerAdminSystemID'
*
* @description [AggPortPartnerAdminSystemID]: The aggregation port's partner
*              system ID (administrative) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminSystemID (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerAdminSystemIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminSystemIDGet (L7_UNIT_CURRENT, keyAggPortValue,
                                               objAggPortPartnerAdminSystemIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminSystemID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objAggPortPartnerAdminSystemIDValue,
                    strlen (objAggPortPartnerAdminSystemIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminSystemID
*
* @purpose Set 'AggPortPartnerAdminSystemID'
*
* @description [AggPortPartnerAdminSystemID]: The aggregation port's partner
*              system ID (administrative) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminSystemID (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerAdminSystemIDValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminSystemID */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAggPortPartnerAdminSystemIDValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAggPortPartnerAdminSystemIDValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminSystemIDSet (L7_UNIT_CURRENT, keyAggPortValue,
                                               objAggPortPartnerAdminSystemIDValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminSystemPriority
*
* @purpose Get 'AggPortPartnerAdminSystemPriority'
*
* @description [AggPortPartnerAdminSystemPriority]: The current administrative
*              priority associated with the partner's system ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerAdminSystemPriority (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminSystemPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminSystemPriorityGet (L7_UNIT_CURRENT,
                                                     keyAggPortValue,
                                                     &objAggPortPartnerAdminSystemPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerAdminSystemPriority */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objAggPortPartnerAdminSystemPriorityValue,
                    sizeof (objAggPortPartnerAdminSystemPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminSystemPriority
*
* @purpose Set 'AggPortPartnerAdminSystemPriority'
*
* @description [AggPortPartnerAdminSystemPriority]: The current administrative
*              priority associated with the partner's system ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAggPort_AggPortPartnerAdminSystemPriority (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerAdminSystemPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggPortPartnerAdminSystemPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objAggPortPartnerAdminSystemPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggPortPartnerAdminSystemPriorityValue, owa.len);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerAdminSystemPrioritySet (L7_UNIT_CURRENT,
                                                     keyAggPortValue,
                                                     objAggPortPartnerAdminSystemPriorityValue);
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
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperKey
*
* @purpose Get 'AggPortPartnerOperKey'
*
* @description [AggPortPartnerOperKey]: The aggregation port's partner system
*              ID (operational) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperKey (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerOperKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperKeyGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         &objAggPortPartnerOperKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortPartnerOperKeyValue,
                           sizeof (objAggPortPartnerOperKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperPort
*
* @purpose Get 'AggPortPartnerOperPort'
*
* @description [AggPortPartnerOperPort]: The operational value of the port
*              number for the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperPort (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerOperPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperPortGet (L7_UNIT_CURRENT, keyAggPortValue,
                                          &objAggPortPartnerOperPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortPartnerOperPortValue,
                           sizeof (objAggPortPartnerOperPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperPortPriority
*
* @purpose Get 'AggPortPartnerOperPortPriority'
*
* @description [AggPortPartnerOperPortPriority]: The priority value assigned
*              to this aggregation port by the partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperPortPriority (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerOperPortPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperPortPriorityGet (L7_UNIT_CURRENT,
                                                  keyAggPortValue,
                                                  &objAggPortPartnerOperPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperPortPriority */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortPartnerOperPortPriorityValue,
                    sizeof (objAggPortPartnerOperPortPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperState
*
* @purpose Get 'AggPortPartnerOperState'
*
* @description [AggPortPartnerOperState]: The 8 bits corresponding to the
*              current values of the partner_state from the most recently
*              received LACPDU (by the partner) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperState (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerOperStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperStateGet (L7_UNIT_CURRENT, keyAggPortValue,
                                           objAggPortPartnerOperStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPortPartnerOperStateValue,
                           strlen (objAggPortPartnerOperStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperSystemID
*
* @purpose Get 'AggPortPartnerOperSystemID'
*
* @description [AggPortPartnerOperSystemID]: The aggregation port's partner
*              system ID (operational) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperSystemID (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPortPartnerOperSystemIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperSystemIDGet (L7_UNIT_CURRENT, keyAggPortValue,
                                              objAggPortPartnerOperSystemIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperSystemID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objAggPortPartnerOperSystemIDValue,
                    strlen (objAggPortPartnerOperSystemIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperSystemPriority
*
* @purpose Get 'AggPortPartnerOperSystemPriority'
*
* @description [AggPortPartnerOperSystemPriority]: The current operational
*              priority associated with the partner's system ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortPartnerOperSystemPriority (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortPartnerOperSystemPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortPartnerOperSystemPriorityGet (L7_UNIT_CURRENT,
                                                    keyAggPortValue,
                                                    &objAggPortPartnerOperSystemPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortPartnerOperSystemPriority */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objAggPortPartnerOperSystemPriorityValue,
                    sizeof (objAggPortPartnerOperSystemPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortSelectedAggID
*
* @purpose Get 'AggPortSelectedAggID'
*
* @description [AggPortSelectedAggID]: The aggregrator this port has selected
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortSelectedAggID (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortSelectedAggIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortSelectedAggIDGet (L7_UNIT_CURRENT, keyAggPortValue,
                                        &objAggPortSelectedAggIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortSelectedAggID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortSelectedAggIDValue,
                           sizeof (objAggPortSelectedAggIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsIllegalRx
*
* @purpose Get 'AggPortStatsIllegalRx'
*
* @description [AggPortStatsIllegalRx]: The number of invalid LACPDUs received
*              on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsIllegalRx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsIllegalRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsIllegalRxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         &objAggPortStatsIllegalRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsIllegalRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsIllegalRxValue,
                           sizeof (objAggPortStatsIllegalRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsLACPDUsRx
*
* @purpose Get 'AggPortStatsLACPDUsRx'
*
* @description [AggPortStatsLACPDUsRx]: The number of valid LACPDUs received
*              on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsLACPDUsRx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsLACPDUsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsLACPDUsRxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         &objAggPortStatsLACPDUsRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsLACPDUsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsLACPDUsRxValue,
                           sizeof (objAggPortStatsLACPDUsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsLACPDUsTx
*
* @purpose Get 'AggPortStatsLACPDUsTx'
*
* @description [AggPortStatsLACPDUsTx]: The number of valid LACPDUs transmitted
*              on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsLACPDUsTx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsLACPDUsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsLACPDUsTxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         &objAggPortStatsLACPDUsTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsLACPDUsTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsLACPDUsTxValue,
                           sizeof (objAggPortStatsLACPDUsTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerPDUsRx
*
* @purpose Get 'AggPortStatsMarkerPDUsRx'
*
* @description [AggPortStatsMarkerPDUsRx]: The number of marker PDUs received
*              on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerPDUsRx (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsMarkerPDUsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsMarkerPDUsRxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                            &objAggPortStatsMarkerPDUsRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsMarkerPDUsRx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsMarkerPDUsRxValue,
                    sizeof (objAggPortStatsMarkerPDUsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerPDUsTx
*
* @purpose Get 'AggPortStatsMarkerPDUsTx'
*
* @description [AggPortStatsMarkerPDUsTx]: The number of marker PDUs transmitted
*              on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerPDUsTx (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsMarkerPDUsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsMarkerPDUsTxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                            &objAggPortStatsMarkerPDUsTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsMarkerPDUsTx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsMarkerPDUsTxValue,
                    sizeof (objAggPortStatsMarkerPDUsTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerResponsePDUsRx
*
* @purpose Get 'AggPortStatsMarkerResponsePDUsRx'
*
* @description [AggPortStatsMarkerResponsePDUsRx]: The number of marker response
*              PDUs received on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerResponsePDUsRx (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsMarkerResponsePDUsRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsMarkerResponsePDUsRxGet (L7_UNIT_CURRENT,
                                                    keyAggPortValue,
                                                    &objAggPortStatsMarkerResponsePDUsRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsMarkerResponsePDUsRx */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objAggPortStatsMarkerResponsePDUsRxValue,
                    sizeof (objAggPortStatsMarkerResponsePDUsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerResponsePDUsTx
*
* @purpose Get 'AggPortStatsMarkerResponsePDUsTx'
*
* @description [AggPortStatsMarkerResponsePDUsTx]: The number of marker response
*              PDUs transmitted on this aggregation port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsMarkerResponsePDUsTx (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsMarkerResponsePDUsTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsMarkerResponsePDUsTxGet (L7_UNIT_CURRENT,
                                                    keyAggPortValue,
                                                    &objAggPortStatsMarkerResponsePDUsTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsMarkerResponsePDUsTx */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objAggPortStatsMarkerResponsePDUsTxValue,
                    sizeof (objAggPortStatsMarkerResponsePDUsTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAggPort_AggPortStatsUnknownRx
*
* @purpose Get 'AggPortStatsUnknownRx'
*
* @description [AggPortStatsUnknownRx]: The number of unknown frames 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAggPort_AggPortStatsUnknownRx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPortStatsUnknownRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggPort */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAggPort_AggPort,
                          (xLibU8_t *) & keyAggPortValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggPortValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPortStatsUnknownRxGet (L7_UNIT_CURRENT, keyAggPortValue,
                                         &objAggPortStatsUnknownRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPortStatsUnknownRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPortStatsUnknownRxValue,
                           sizeof (objAggPortStatsUnknownRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
