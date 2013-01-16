/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesysSerialPort.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basesysSerialPort_obj.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_basesysSerialPort_sysAgentBasicConfigSerialBaudRate
*
* @purpose Get 'sysAgentBasicConfigSerialBaudRate'
*
* @description Baud Rate for the Serial Interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysAgentBasicConfigSerialBaudRate (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigSerialBaudRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAgentBasicConfigSerialBaudRateGet (L7_UNIT_CURRENT,
                                            &objsysAgentBasicConfigSerialBaudRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigSerialBaudRateValue,
                     sizeof (objsysAgentBasicConfigSerialBaudRateValue));

  /* return the object value: sysAgentBasicConfigSerialBaudRate */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objsysAgentBasicConfigSerialBaudRateValue,
                    sizeof (objsysAgentBasicConfigSerialBaudRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysSerialPort_sysAgentBasicConfigSerialBaudRate
*
* @purpose Set 'sysAgentBasicConfigSerialBaudRate'
*
* @description Baud Rate for the Serial Interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysSerialPort_sysAgentBasicConfigSerialBaudRate (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysAgentBasicConfigSerialBaudRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysAgentBasicConfigSerialBaudRate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objsysAgentBasicConfigSerialBaudRateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysAgentBasicConfigSerialBaudRateValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbAgentBasicConfigSerialBaudRateSet (L7_UNIT_CURRENT,
                                            objsysAgentBasicConfigSerialBaudRateValue);
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
* @function fpObjGet_basesysSerialPort_sysSerialTimeOut
*
* @purpose Get 'sysSerialTimeOut'
*
* @description the Timeout for the Serial Interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialTimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSerialTimeOutGet (L7_UNIT_CURRENT, &objsysSerialTimeOutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialTimeOutValue,
                     sizeof (objsysSerialTimeOutValue));

  /* return the object value: sysSerialTimeOut */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialTimeOutValue,
                           sizeof (objsysSerialTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysSerialPort_sysSerialTimeOut
*
* @purpose Set 'sysSerialTimeOut'
*
* @description the Timeout for the Serial Interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysSerialPort_sysSerialTimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysSerialTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysSerialTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialTimeOutValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSerialTimeOutSet (L7_UNIT_CURRENT, objsysSerialTimeOutValue);
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
* @function fpObjGet_basesysSerialPort_sysSerialPortParmCharSize
*
* @purpose Get 'sysSerialPortParmCharSize'
*
* @description The Serial Port Parameter Char size 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialPortParmCharSize (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialPortParmCharSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSerialPortParmGet (L7_UNIT_CURRENT,SYSAPISERIALPORT_CHARSIZE,
                            &objsysSerialPortParmCharSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialPortParmCharSizeValue,
                     sizeof (objsysSerialPortParmCharSizeValue));

  /* return the object value: sysSerialPortParmCharSize */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialPortParmCharSizeValue,
                    sizeof (objsysSerialPortParmCharSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysSerialPort_sysSerialPortParmFlowControl
*
* @purpose Get 'sysSerialPortParmFlowControl'
*
* @description The Serial Port Parameter flow control 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialPortParmFlowControl (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialPortParmFlowControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSerialPortParmGet (L7_UNIT_CURRENT, SYSAPISERIALPORT_FLOWCONTROL,
                            &objsysSerialPortParmFlowControlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialPortParmFlowControlValue,
                     sizeof (objsysSerialPortParmFlowControlValue));

  /* return the object value: sysSerialPortParmFlowControl */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialPortParmFlowControlValue,
                    sizeof (objsysSerialPortParmFlowControlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysSerialPort_sysSerialPortParmParityType
*
* @purpose Get 'sysSerialPortParmParityType'
*
* @description The Serial Port Parameter parity 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialPortParmParityType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialPortParmParityTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSerialPortParmGet (L7_UNIT_CURRENT, SYSAPISERIALPORT_PARITYTYPE,
                            &objsysSerialPortParmParityTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialPortParmParityTypeValue,
                     sizeof (objsysSerialPortParmParityTypeValue));

  /* return the object value: sysSerialPortParmParityType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialPortParmParityTypeValue,
                    sizeof (objsysSerialPortParmParityTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysSerialPort_sysSerialPortParmStopBits
*
* @purpose Get 'sysSerialPortParmStopBits'
*
* @description The Serial Port Parameter stop bit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialPortParmStopBits (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialPortParmStopBitsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSerialPortParmGet (L7_UNIT_CURRENT, SYSAPISERIALPORT_STOPBITS,
                            &objsysSerialPortParmStopBitsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysSerialPortParmStopBitsValue,
                     sizeof (objsysSerialPortParmStopBitsValue));

  /* return the object value: sysSerialPortParmStopBits */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialPortParmStopBitsValue,
                    sizeof (objsysSerialPortParmStopBitsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basesysSerialPort_sysSerialPortLinkStatus
*
* @purpose Get 'sysSerialPortLinkStatus'
*
* @description The Serial Port Link Status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysSerialPort_sysSerialPortLinkStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysSerialPortLinkStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objsysSerialPortLinkStatus =  usmDbSerialStatusGet (L7_UNIT_CURRENT); 

  FPOBJ_TRACE_VALUE (bufp, &objsysSerialPortLinkStatus,
                     sizeof (objsysSerialPortLinkStatus));

  /* return the object value: sysSerialPortParmStopBits */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysSerialPortLinkStatus,
                    sizeof (objsysSerialPortLinkStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

