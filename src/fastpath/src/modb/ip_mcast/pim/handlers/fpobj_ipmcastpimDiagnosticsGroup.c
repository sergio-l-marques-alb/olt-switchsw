
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimDiagnosticsGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimDiagnosticsGroup_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimOutAsserts
*
* @purpose Get 'pimOutAsserts'
*
* @description [pimOutAsserts] The number of Asserts sent by this router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimOutAsserts (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimOutAssertsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimOutAssertsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimOutAssertsValue, sizeof (objpimOutAssertsValue));

  /* return the object value: pimOutAsserts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimOutAssertsValue,
                           sizeof (objpimOutAssertsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimInAsserts
*
* @purpose Get 'pimInAsserts'
*
* @description [pimInAsserts] The number of Asserts received by this router. Asserts are multicast to all routers on a network. This counter is incremented by all routers that receive an assert, not only those routers that are contesting the assert.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimInAsserts (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInAssertsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInAssertsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInAssertsValue, sizeof (objpimInAssertsValue));

  /* return the object value: pimInAsserts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInAssertsValue,
                           sizeof (objpimInAssertsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertInterface
*
* @purpose Get 'pimLastAssertInterface'
*
* @description [pimLastAssertInterface] The interface on which this router most recently sent or received an assert, or zero if this router has not sent or received an assert.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertInterface (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimLastAssertInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimLastAssertInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimLastAssertInterfaceValue,
                     sizeof (objpimLastAssertInterfaceValue));

  /* return the object value: pimLastAssertInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimLastAssertInterfaceValue,
                           sizeof (objpimLastAssertInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertGroupAddressType
*
* @purpose Get 'pimLastAssertGroupAddressType'
*
* @description [pimLastAssertGroupAddressType] The address type of the multicast group address in the most recently sent or received assert. If this router has not sent or received an assert, then this object is set to unknown(0).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertGroupAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimLastAssertGroupAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimLastAssertGroupAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimLastAssertGroupAddressTypeValue,
                     sizeof (objpimLastAssertGroupAddressTypeValue));

  /* return the object value: pimLastAssertGroupAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimLastAssertGroupAddressTypeValue,
                           sizeof (objpimLastAssertGroupAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertGroupAddress
*
* @purpose Get 'pimLastAssertGroupAddress'
*
* @description [pimLastAssertGroupAddress] The multicast group address in the most recently sent or received assert. The InetAddressType is given by the pimLastAssertGroupAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertGroupAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimLastAssertGroupAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimLastAssertGroupAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimLastAssertGroupAddressValue,
                     sizeof (objpimLastAssertGroupAddressValue));

  /* return the object value: pimLastAssertGroupAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimLastAssertGroupAddressValue,
                           sizeof (objpimLastAssertGroupAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertSourceAddressType
*
* @purpose Get 'pimLastAssertSourceAddressType'
*
* @description [pimLastAssertSourceAddressType] The address type of the source address in the most recently sent or received assert.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertSourceAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimLastAssertSourceAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimLastAssertSourceAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimLastAssertSourceAddressTypeValue,
                     sizeof (objpimLastAssertSourceAddressTypeValue));

  /* return the object value: pimLastAssertSourceAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimLastAssertSourceAddressTypeValue,
                           sizeof (objpimLastAssertSourceAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertSourceAddress
*
* @purpose Get 'pimLastAssertSourceAddress'
*
* @description [pimLastAssertSourceAddress] The source address in the most recently sent or received assert. The InetAddressType is given by the pimLastAssertSourceAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimLastAssertSourceAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimLastAssertSourceAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objpimLastAssertSourceAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpimLastAssertSourceAddressValue,
                     sizeof (objpimLastAssertSourceAddressValue));

  /* return the object value: pimLastAssertSourceAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimLastAssertSourceAddressValue,
                           sizeof (objpimLastAssertSourceAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimNeighborLossNotificationPeriod
*
* @purpose Get 'pimNeighborLossNotificationPeriod'
*
* @description [pimNeighborLossNotificationPeriod] The minimum time that must elapse between pimNeighborLoss notifications originated by this router. The maximum value 65535 represents an 'infinite' time, in which case, no pimNeighborLoss notifications are ever sent.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimNeighborLossNotificationPeriod (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborLossNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimNeighborLossNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimNeighborLossNotificationPeriodValue,
                     sizeof (objpimNeighborLossNotificationPeriodValue));

  /* return the object value: pimNeighborLossNotificationPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborLossNotificationPeriodValue,
                           sizeof (objpimNeighborLossNotificationPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimDiagnosticsGroup_pimNeighborLossNotificationPeriod
*
* @purpose Set 'pimNeighborLossNotificationPeriod'
*
* @description [pimNeighborLossNotificationPeriod] The minimum time that must elapse between pimNeighborLoss notifications originated by this router. The maximum value 65535 represents an 'infinite' time, in which case, no pimNeighborLoss notifications are ever sent.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimDiagnosticsGroup_pimNeighborLossNotificationPeriod (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborLossNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimNeighborLossNotificationPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpimNeighborLossNotificationPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimNeighborLossNotificationPeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimNeighborLossNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimNeighborLossCount
*
* @purpose Get 'pimNeighborLossCount'
*
* @description [pimNeighborLossCount] The number of neighbor loss events that have occurred.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimNeighborLossCount (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborLossCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimNeighborLossCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimNeighborLossCountValue, sizeof (objpimNeighborLossCountValue));

  /* return the object value: pimNeighborLossCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborLossCountValue,
                           sizeof (objpimNeighborLossCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterNotificationPeriod
*
* @purpose Get 'pimInvalidRegisterNotificationPeriod'
*
* @description [pimInvalidRegisterNotificationPeriod] The minimum time that must elapse between pimInvalidRegister notifications originated by this router. The default value of 65535 represents an 'infinite' time, in which case, no pimInvalidRegister notifications are ever sent.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterNotificationPeriod (void *wap,
                                                                                   void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidRegisterNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidRegisterNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidRegisterNotificationPeriodValue,
                     sizeof (objpimInvalidRegisterNotificationPeriodValue));

  /* return the object value: pimInvalidRegisterNotificationPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidRegisterNotificationPeriodValue,
                           sizeof (objpimInvalidRegisterNotificationPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterNotificationPeriod
*
* @purpose Set 'pimInvalidRegisterNotificationPeriod'
*
* @description [pimInvalidRegisterNotificationPeriod] The minimum time that must elapse between pimInvalidRegister notifications originated by this router. The default value of 65535 represents an 'infinite' time, in which case, no pimInvalidRegister notifications are ever sent.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterNotificationPeriod (void *wap,
                                                                                   void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidRegisterNotificationPeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInvalidRegisterNotificationPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpimInvalidRegisterNotificationPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidRegisterNotificationPeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimInvalidRegisterNotificationPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterMsgsRcvd
*
* @purpose Get 'pimInvalidRegisterMsgsRcvd'
*
* @description [pimInvalidRegisterMsgsRcvd] The number of invalid PIM Register messages that have been received by this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimDiagnosticsGroup_pimInvalidRegisterMsgsRcvd (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInvalidRegisterMsgsRcvdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimInvalidRegisterMsgsRcvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimInvalidRegisterMsgsRcvdValue,
                     sizeof (objpimInvalidRegisterMsgsRcvdValue));

  /* return the object value: pimInvalidRegisterMsgsRcvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInvalidRegisterMsgsRcvdValue,
                           sizeof (objpimInvalidRegisterMsgsRcvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
