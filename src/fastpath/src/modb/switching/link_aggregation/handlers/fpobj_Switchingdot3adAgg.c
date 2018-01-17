/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot3adAgg.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot3ad-object.xml
*
* @create  5 March 2008
*
* @author  Radha
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot3adAgg_obj.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggInterface
*
* @purpose Get 'AggInterface'
*
* @description [AggInterface]: Tthe aggregator interface 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggInterfaceValue;
  xLibU32_t nextObjAggInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & objAggInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    objAggInterfaceValue = 0;
    owa.l7rc = usmDbDot3adAggEntryGetNext(L7_UNIT_CURRENT,objAggInterfaceValue,
                                          &nextObjAggInterfaceValue);
   
  }
  else
  {
    owa.l7rc = usmDbDot3adAggEntryGetNext(L7_UNIT_CURRENT,
                                         objAggInterfaceValue,
                                         &nextObjAggInterfaceValue);
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAggInterfaceValue, owa.len);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAggInterfaceValue, owa.len);

  /* return the object value: AggInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAggInterfaceValue,
                           sizeof (objAggInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggActorAdminKey
*
* @purpose Get 'AggActorAdminKey'
*
* @description [AggActorAdminKey]: The current administrative value of the
*              Key for the aggregator 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggActorAdminKey (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggActorAdminKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggActorAdminKeyGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                             &objAggActorAdminKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggActorAdminKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggActorAdminKeyValue,
                           sizeof (objAggActorAdminKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAgg_AggActorAdminKey
*
* @purpose Set 'AggActorAdminKey'
*
* @description [AggActorAdminKey]: The current administrative value of the
*              Key for the aggregator 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAgg_AggActorAdminKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggActorAdminKeyValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggActorAdminKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggActorAdminKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggActorAdminKeyValue, owa.len);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggActorAdminKeySet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                    objAggActorAdminKeyValue);
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
* @function fpObjGet_Switchingdot3adAgg_AggActorOperKey
*
* @purpose Get 'AggActorOperKey'
*
* @description [AggActorOperKey]: The current operational value of the Key
*              for the aggregator 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggActorOperKey (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggActorOperKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggActorOperKeyGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                   &objAggActorOperKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggActorOperKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggActorOperKeyValue,
                           sizeof (objAggActorOperKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggActorSystemID
*
* @purpose Get 'AggActorSystemID'
*
* @description [AggActorSystemID]: The MAC address used as a unique identifier
*              for the system that contains this aggregator 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggActorSystemID (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggActorSystemIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggActorSystemIDGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                    objAggActorSystemIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggActorSystemID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggActorSystemIDValue,
                           strlen (objAggActorSystemIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggActorSystemPriority
*
* @purpose Get 'AggActorSystemPriority'
*
* @description [AggActorSystemPriority]: The priority value associated with
*              the system's actor ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggActorSystemPriority (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggActorSystemPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggActorSystemPriorityGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                          &objAggActorSystemPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggActorSystemPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggActorSystemPriorityValue,
                           sizeof (objAggActorSystemPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAgg_AggActorSystemPriority
*
* @purpose Set 'AggActorSystemPriority'
*
* @description [AggActorSystemPriority]: The priority value associated with
*              the system's actor ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAgg_AggActorSystemPriority (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggActorSystemPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggActorSystemPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggActorSystemPriorityValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggActorSystemPriorityValue, owa.len);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggActorSystemPrioritySet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                          objAggActorSystemPriorityValue);
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
* @function fpObjGet_Switchingdot3adAgg_AggAggregateOrIndividual
*
* @purpose Get 'AggAggregateOrIndividual'
*
* @description [AggAggregateOrIndividual]: Determines if the aggregator represents
*              an aggregate or an individual link 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggAggregateOrIndividual (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggAggregateOrIndividualValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggAggregateOrIndividualGet (L7_UNIT_CURRENT,
                                            keyAggInterfaceValue,
                                            &objAggAggregateOrIndividualValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggAggregateOrIndividual */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggAggregateOrIndividualValue,
                    sizeof (objAggAggregateOrIndividualValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggCollectorMaxDelay
*
* @purpose Get 'AggCollectorMaxDelay'
*
* @description [AggCollectorMaxDelay]: Delay that can be imposed on the frame
*              collector between receving a frame from and aggregator
*              parser and either delivering it to a MAC client or discarding
*              it 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggCollectorMaxDelay (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggCollectorMaxDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggCollectorMaxDelayGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                        &objAggCollectorMaxDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggCollectorMaxDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggCollectorMaxDelayValue,
                           sizeof (objAggCollectorMaxDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot3adAgg_AggCollectorMaxDelay
*
* @purpose Set 'AggCollectorMaxDelay'
*
* @description [AggCollectorMaxDelay]: Delay that can be imposed on the frame
*              collector between receving a frame from and aggregator
*              parser and either delivering it to a MAC client or discarding
*              it 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot3adAgg_AggCollectorMaxDelay (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggCollectorMaxDelayValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AggCollectorMaxDelay */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAggCollectorMaxDelayValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAggCollectorMaxDelayValue, owa.len);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot3adAggCollectorMaxDelaySet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                        objAggCollectorMaxDelayValue);
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
* @function fpObjGet_Switchingdot3adAgg_AggPartnerOperKey
*
* @purpose Get 'AggPartnerOperKey'
*
* @description [AggPartnerOperKey]: The current operational value of the key
*              for the aggregators current protocol partner 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggPartnerOperKey (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPartnerOperKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPartnerOperKeyGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                     &objAggPartnerOperKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPartnerOperKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAggPartnerOperKeyValue,
                           sizeof (objAggPartnerOperKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggPartnerSystemID
*
* @purpose Get 'AggPartnerSystemID'
*
* @description [AggPartnerSystemID]: MAC address of current partner of aggregator
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggPartnerSystemID (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggPartnerSystemIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPartnerSystemIDGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                      objAggPartnerSystemIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPartnerSystemID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggPartnerSystemIDValue,
                           strlen (objAggPartnerSystemIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggPartnerSystemPriority
*
* @purpose Get 'AggPartnerSystemPriority'
*
* @description [AggPartnerSystemPriority]: Priority value associated with
*              partner's system ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggPartnerSystemPriority (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAggPartnerSystemPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPartnerSystemPriorityGet (L7_UNIT_CURRENT,
                                            keyAggInterfaceValue,
                                            &objAggPartnerSystemPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggPartnerSystemPriority */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAggPartnerSystemPriorityValue,
                    sizeof (objAggPartnerSystemPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot3adAgg_AggMACAddress
*
* @purpose Get 'AggMACAddress'
*
* @description [AggMACAddress]: The MAC address assigned to the aggregator
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot3adAgg_AggMACAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAggInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAggMACAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AggInterface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot3adAgg_AggInterface,
                          (xLibU8_t *) & keyAggInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAggInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3adAggPhysAddressGet (L7_UNIT_CURRENT, keyAggInterfaceValue,
                                  objAggMACAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AggMACAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAggMACAddressValue,
                           strlen (objAggMACAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
