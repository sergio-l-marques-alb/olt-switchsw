/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseStormControl.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  14 April 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseStormControl_obj.h"
#include "commdefs.h"
#include "usmdb_policy_api.h"


/*******************************************************************************
* @function fpObjGet_baseStormControl_Dot3FlowControlMode
*
* @purpose Get 'Dot3FlowControlMode'
*
* @description [Dot3FlowControlMode]: Config switchconfig flowcontrol allows
*              you to enable or disable 802.3x flow control for the switch.
*              This value applies to only full-duplex mode ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_Dot3FlowControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot3FlowControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlFlowControlModeGet (L7_UNIT_CURRENT, &objDot3FlowControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot3FlowControlModeValue, sizeof (objDot3FlowControlModeValue));

  /* return the object value: Dot3FlowControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot3FlowControlModeValue,
                           sizeof (objDot3FlowControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_Dot3FlowControlMode
*
* @purpose Set 'Dot3FlowControlMode'
*
* @description [Dot3FlowControlMode]: Config switchconfig flowcontrol allows
*              you to enable or disable 802.3x flow control for the switch.
*              This value applies to only full-duplex mode ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_Dot3FlowControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot3FlowControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot3FlowControlMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot3FlowControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot3FlowControlModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlFlowControlModeSet (L7_UNIT_CURRENT, objDot3FlowControlModeValue);
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
* @function fpObjGet_baseStormControl_BroadcastControlMode
*
* @purpose Get 'BroadcastControlMode'
*
* @description [BroadcastControlMode]: The switch config broadcast allows
*              you to enable or disable broadcast storm recovery mode. When
*              you specify Enable for BroadcastStorm Recovery and the broadcast
*              traffic on any Ethernet port exceedsthe configured
*              threshold, the switch blocks (discards) the broadcast traffic.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_BroadcastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormModeGet (L7_UNIT_CURRENT, &objBroadcastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlModeValue, sizeof (objBroadcastControlModeValue));

  /* return the object value: BroadcastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastControlModeValue,
                           sizeof (objBroadcastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_BroadcastControlMode
*
* @purpose Set 'BroadcastControlMode'
*
* @description [BroadcastControlMode]: The switch config broadcast allows
*              you to enable or disable broadcast storm recovery mode. When
*              you specify Enable for BroadcastStorm Recovery and the broadcast
*              traffic on any Ethernet port exceedsthe configured
*              threshold, the switch blocks (discards) the broadcast traffic.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_BroadcastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastControlMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlBcastStormModeSet (L7_UNIT_CURRENT, objBroadcastControlModeValue);
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
* @function fpObjGet_baseStormControl_MulticastControlMode
*
* @purpose Get 'MulticastControlMode'
*
* @description [MulticastControlMode]: The switch config multicast allows
*              you to enable or disable multicast storm recovery mode. When
*              you specify Enable for multicastStorm Recovery and the multicast
*              traffic on any Ethernet port exceeds the configured
*              threshold, the switch blocks (discards) the multicast traffic.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_MulticastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormModeGet (L7_UNIT_CURRENT, &objMulticastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlModeValue, sizeof (objMulticastControlModeValue));

  /* return the object value: MulticastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastControlModeValue,
                           sizeof (objMulticastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_MulticastControlMode
*
* @purpose Set 'MulticastControlMode'
*
* @description [MulticastControlMode]: The switch config multicast allows
*              you to enable or disable multicast storm recovery mode. When
*              you specify Enable for multicastStorm Recovery and the multicast
*              traffic on any Ethernet port exceeds the configured
*              threshold, the switch blocks (discards) the multicast traffic.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_MulticastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastControlMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlMcastStormModeSet (L7_UNIT_CURRENT, objMulticastControlModeValue);
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
* @function fpObjGet_baseStormControl_UnicastControlMode
*
* @purpose Get 'UnicastControlMode'
*
* @description [UnicastControlMode]: The switch config unicast allows you
*              to enable or disable unicast storm recovery mode. When you
*              specify Enable for unicast Storm Recovery and the unknown unicast
*              traffic on any Ethernet port exceeds the configured
*              threshold, the switch blocks (discards) the unknown unicast
*              traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_UnicastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormModeGet (L7_UNIT_CURRENT, &objUnicastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlModeValue, sizeof (objUnicastControlModeValue));

  /* return the object value: UnicastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastControlModeValue,
                           sizeof (objUnicastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_UnicastControlMode
*
* @purpose Set 'UnicastControlMode'
*
* @description [UnicastControlMode]: The switch config unicast allows you
*              to enable or disable unicast storm recovery mode. When you
*              specify Enable for unicast Storm Recovery and the unknown unicast
*              traffic on any Ethernet port exceeds the configured
*              threshold, the switch blocks (discards) the unknown unicast
*              traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_UnicastControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastControlMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlUcastStormModeSet (L7_UNIT_CURRENT, objUnicastControlModeValue);
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
* @function fpObjGet_baseStormControl_BroadcastControlThreshold
*
* @purpose Get 'BroadcastControlThreshold'
*
* @description [BroadcastControlThreshold]: Configures the broadcast storm
*              recovery threshold for this portas a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_BroadcastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdValue;
  L7_RATE_UNIT_t rate_unit;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objBroadcastControlThresholdValue, &rate_unit);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlThresholdValue,
                     sizeof (objBroadcastControlThresholdValue));

  /* return the object value: BroadcastControlThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastControlThresholdValue,
                    sizeof (objBroadcastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_BroadcastControlThreshold
*
* @purpose Set 'BroadcastControlThreshold'
*
* @description [BroadcastControlThreshold]: Configures the broadcast storm
*              recovery threshold for this portas a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_BroadcastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdValue;

  xLibU32_t tempBroadcastControlThresholdValue;
  xLibU32_t objBroadcastControlThresholdUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastControlThreshold */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastControlThresholdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlThresholdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &tempBroadcastControlThresholdValue,
                                                   &objBroadcastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempBroadcastControlThresholdValue != objBroadcastControlThresholdValue)
    {
      owa.l7rc = usmDbSwDevCtrlBcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objBroadcastControlThresholdValue,
                                                       objBroadcastControlThresholdUnitValue);
    }
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
* @function fpObjGet_baseStormControl_BroadcastControlThresholdUnit
*
* @purpose Get 'BroadcastControlThresholdUnit'
 *@description  [BroadcastControlThresholdUnit] Configures the broadcast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_BroadcastControlThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdUnitValue;
  xLibU32_t objBroadcastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objBroadcastControlThresholdValue,
                                                   &objBroadcastControlThresholdUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlThresholdUnitValue,
                     sizeof (objBroadcastControlThresholdUnitValue));

  /* return the object value: BroadcastControlThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastControlThresholdUnitValue,
                           sizeof (objBroadcastControlThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseStormControl_BroadcastControlThresholdUnit
*
* @purpose Set 'BroadcastControlThresholdUnit'
 *@description  [BroadcastControlThresholdUnit] Configures the broadcast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_BroadcastControlThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdUnitValue;
  xLibU32_t tempBroadcastControlThresholdUnitValue;
  xLibU32_t objBroadcastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastControlThresholdUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastControlThresholdUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlThresholdUnitValue, owa.len);

  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objBroadcastControlThresholdValue,
                                                   &tempBroadcastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempBroadcastControlThresholdUnitValue != objBroadcastControlThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlBcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objBroadcastControlThresholdValue,
                                                       objBroadcastControlThresholdUnitValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseStormControl_MulticastControlThreshold
*
* @purpose Get 'MulticastControlThreshold'
*
* @description [MulticastControlThreshold]: Configures the multicast storm
*              recovery threshold for this por as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_MulticastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdValue;
  xLibU32_t objMulticastControlThresholdUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objMulticastControlThresholdValue,
                                                   &objMulticastControlThresholdUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlThresholdValue,
                     sizeof (objMulticastControlThresholdValue));

  /* return the object value: MulticastControlThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastControlThresholdValue,
                    sizeof (objMulticastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_MulticastControlThreshold
*
* @purpose Set 'MulticastControlThreshold'
*
* @description [MulticastControlThreshold]: Configures the multicast storm
*              recovery threshold for this por as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_MulticastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdValue;

  xLibU32_t objMulticastControlThresholdUnitValue;
  xLibU32_t tempMulticastControlThresholdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastControlThreshold */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastControlThresholdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlThresholdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &tempMulticastControlThresholdValue,
                                                   &objMulticastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempMulticastControlThresholdValue != objMulticastControlThresholdValue)
    {
      owa.l7rc = usmDbSwDevCtrlMcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objMulticastControlThresholdValue,
                                                       objMulticastControlThresholdUnitValue);
    }
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
* @function fpObjGet_baseStormControl_MulticastControlThresholdUnit
*
* @purpose Get 'MulticastControlThresholdUnit'
 *@description  [MulticastControlThresholdUnit] Configures the multicast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_MulticastControlThresholdUnit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdUnitValue;
  xLibU32_t objMulticastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objMulticastControlThresholdValue,
                                                   &objMulticastControlThresholdUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlThresholdUnitValue,
                     sizeof (objMulticastControlThresholdUnitValue));

  /* return the object value: MulticastControlThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastControlThresholdUnitValue,
                           sizeof (objMulticastControlThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseStormControl_MulticastControlThresholdUnit
*
* @purpose Set 'MulticastControlThresholdUnit'
 *@description  [MulticastControlThresholdUnit] Configures the multicast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_MulticastControlThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdUnitValue;
  xLibU32_t tempMulticastControlThresholdUnitValue;
  xLibU32_t objMulticastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastControlThresholdUnit */
  objMulticastControlThresholdUnitValue = L7_RATE_UNIT_PPS;
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objMulticastControlThresholdValue,
                                                   &tempMulticastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempMulticastControlThresholdUnitValue != objMulticastControlThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlMcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objMulticastControlThresholdValue,
                                                       objMulticastControlThresholdUnitValue);
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlThresholdUnitValue, owa.len);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseStormControl_UnicastControlThreshold
*
* @purpose Get 'UnicastControlThreshold'
*
* @description [UnicastControlThreshold]: Configures the unicast storm recovery
*              threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_UnicastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdValue;
  xLibU32_t objUnicastControlThresholdUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objUnicastControlThresholdValue,
                                                   &objUnicastControlThresholdUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlThresholdValue,
                     sizeof (objUnicastControlThresholdValue));

  /* return the object value: UnicastControlThreshold */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastControlThresholdValue,
                           sizeof (objUnicastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseStormControl_UnicastControlThreshold
*
* @purpose Set 'UnicastControlThreshold'
*
* @description [UnicastControlThreshold]: Configures the unicast storm recovery
*              threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_UnicastControlThreshold (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdValue;

  xLibU32_t tempUnicastControlThresholdValue;
  xLibU32_t objUnicastControlThresholdUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastControlThreshold */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastControlThresholdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlThresholdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &tempUnicastControlThresholdValue,
                                                   &objUnicastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempUnicastControlThresholdValue != objUnicastControlThresholdValue)
    {
      owa.l7rc = usmDbSwDevCtrlUcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objUnicastControlThresholdValue,
                                                       objUnicastControlThresholdUnitValue);
    }
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
* @function fpObjGet_baseStormControl_UnicastControlThresholdUnit
*
* @purpose Get 'UnicastControlThresholdUnit'
 *@description  [UnicastControlThresholdUnit] Configures the unicast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStormControl_UnicastControlThresholdUnit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdUnitValue;
  xLibU32_t objUnicastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objUnicastControlThresholdValue,
                                                   &objUnicastControlThresholdUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlThresholdUnitValue,
                     sizeof (objUnicastControlThresholdUnitValue));

  /* return the object value: UnicastControlThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastControlThresholdUnitValue,
                           sizeof (objUnicastControlThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseStormControl_UnicastControlThresholdUnit
*
* @purpose Set 'UnicastControlThresholdUnit'
 *@description  [UnicastControlThresholdUnit] Configures the unicast storm
* recovery threshold unit for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseStormControl_UnicastControlThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdUnitValue;
  xLibU32_t tempUnicastControlThresholdUnitValue;
  xLibU32_t objUnicastControlThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastControlThresholdUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastControlThresholdUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlThresholdUnitValue, owa.len);

  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdGet (L7_UNIT_CURRENT,
                                                   &objUnicastControlThresholdValue,
                                                   &tempUnicastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempUnicastControlThresholdUnitValue != objUnicastControlThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlUcastStormThresholdSet (L7_UNIT_CURRENT,
                                                       objUnicastControlThresholdValue,
                                                       objUnicastControlThresholdUnitValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
