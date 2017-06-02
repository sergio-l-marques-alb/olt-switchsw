/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPGlobalConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  6 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPGlobalConfig_obj.h"
#include "usmdb_lldp_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPGlobalConfig_ReinitDelay
*
* @purpose Get 'ReinitDelay'
*
* @description [ReinitDelay]: The ReinitDelay indicates the delay (in units
*              of seconds) from when AdminStatus object of a particular
*              port becomes 'disabled' until re-initialization will be attempted.
*              The default value for ReintDelay object is two seconds.
*              The value of this object must 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPGlobalConfig_ReinitDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objReinitDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpTxReinitDelayGet ( &objReinitDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objReinitDelayValue, sizeof (objReinitDelayValue));

  /* return the object value: ReinitDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objReinitDelayValue,
                           sizeof (objReinitDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPGlobalConfig_ReinitDelay
*
* @purpose Set 'ReinitDelay'
*
* @description [ReinitDelay]: The ReinitDelay indicates the delay (in units
*              of seconds) from when AdminStatus object of a particular
*              port becomes 'disabled' until re-initialization will be attempted.
*              The default value for ReintDelay object is two seconds.
*              The value of this object must 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPGlobalConfig_ReinitDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objReinitDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ReinitDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objReinitDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objReinitDelayValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpTxReinitDelaySet ( objReinitDelayValue);
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
* @function fpObjGet_SwitchingLLDPGlobalConfig_MessageTxInterval
*
* @purpose Get 'MessageTxInterval'
*
* @description [MessageTxInterval]: The interval at which LLDP frames are
*              transmitted on behalf of this LLDP agent.The default value
*              for MessageTxInterval object is 30 seconds.The value of this
*              object must be restored from non-volatile storage after a
*              re-initialization of the management sys 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPGlobalConfig_MessageTxInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMessageTxIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpTxIntervalGet ( &objMessageTxIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMessageTxIntervalValue,
                     sizeof (objMessageTxIntervalValue));

  /* return the object value: MessageTxInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMessageTxIntervalValue,
                           sizeof (objMessageTxIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPGlobalConfig_MessageTxInterval
*
* @purpose Set 'MessageTxInterval'
*
* @description [MessageTxInterval]: The interval at which LLDP frames are
*              transmitted on behalf of this LLDP agent.The default value
*              for MessageTxInterval object is 30 seconds.The value of this
*              object must be restored from non-volatile storage after a
*              re-initialization of the management sys 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPGlobalConfig_MessageTxInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMessageTxIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MessageTxInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMessageTxIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMessageTxIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLldpTxIntervalSet ( objMessageTxIntervalValue);
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
* @function fpObjGet_SwitchingLLDPGlobalConfig_NotificationInterval
*
* @purpose Get 'NotificationInterval'
*
* @description [NotificationInterval]: This object controls the transmission
*              of LLDP notifications. the agent must not generate more
*              than one RemTablesChange notification-event in the indicated
*              period, where a 'notification-event' is the transmission
*              of a single notification PDU type to a list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPGlobalConfig_NotificationInterval (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotificationIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpNotificationIntervalGet ( &objNotificationIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotificationIntervalValue,
                     sizeof (objNotificationIntervalValue));

  /* return the object value: NotificationInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNotificationIntervalValue,
                           sizeof (objNotificationIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPGlobalConfig_NotificationInterval
*
* @purpose Set 'NotificationInterval'
*
* @description [NotificationInterval]: This object controls the transmission
*              of LLDP notifications. the agent must not generate more
*              than one RemTablesChange notification-event in the indicated
*              period, where a 'notification-event' is the transmission
*              of a single notification PDU type to a list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPGlobalConfig_NotificationInterval (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotificationIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotificationInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNotificationIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotificationIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLldpNotificationIntervalSet ( objNotificationIntervalValue);
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
* @function fpObjGet_SwitchingLLDPGlobalConfig_MessageTxHoldMultiplier
*
* @purpose Get 'MessageTxHoldMultiplier'
*
* @description [MessageTxHoldMultiplier]: The time-to-live value expressed
*              as a multiple of the MessageTxInterval object. The actual
*              time-to-live value used in LLDP frames, transmitted on behalf
*              of this LLDP agent,can be expressed by the following formula:
*              TTL = min(65535,(MessageTxInterval * Me 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPGlobalConfig_MessageTxHoldMultiplier (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMessageTxHoldMultiplierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpTxHoldGet ( &objMessageTxHoldMultiplierValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMessageTxHoldMultiplierValue,
                     sizeof (objMessageTxHoldMultiplierValue));

  /* return the object value: MessageTxHoldMultiplier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMessageTxHoldMultiplierValue,
                           sizeof (objMessageTxHoldMultiplierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPGlobalConfig_MessageTxHoldMultiplier
*
* @purpose Set 'MessageTxHoldMultiplier'
*
* @description [MessageTxHoldMultiplier]: The time-to-live value expressed
*              as a multiple of the MessageTxInterval object. The actual
*              time-to-live value used in LLDP frames, transmitted on behalf
*              of this LLDP agent,can be expressed by the following formula:
*              TTL = min(65535,(MessageTxInterval * Me 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPGlobalConfig_MessageTxHoldMultiplier (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMessageTxHoldMultiplierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MessageTxHoldMultiplier */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMessageTxHoldMultiplierValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMessageTxHoldMultiplierValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLldpTxHoldSet ( objMessageTxHoldMultiplierValue);
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
* @function fpObjGet_SwitchingLLDPGlobalConfig_TxDelay
*
* @purpose Get 'TxDelay'
*
* @description [TxDelay]: The TxDelay indicates the delay (in units of seconds)
*              between successive LLDP frame transmissions initiated
*              by value/status changes in the LLDP local systems MIB. The
*              recommended value for the TxDelay is set by the following
*              formula: 1  TxDelay 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPGlobalConfig_TxDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpTxDelayGet ( &objTxDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTxDelayValue, sizeof (objTxDelayValue));

  /* return the object value: TxDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxDelayValue,
                           sizeof (objTxDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPGlobalConfig_TxDelay
*
* @purpose Set 'TxDelay'
*
* @description [TxDelay]: The TxDelay indicates the delay (in units of seconds)
*              between successive LLDP frame transmissions initiated
*              by value/status changes in the LLDP local systems MIB. The
*              recommended value for the TxDelay is set by the following
*              formula: 1  TxDelay 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPGlobalConfig_TxDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TxDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTxDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTxDelayValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpTxDelaySet ( objTxDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
