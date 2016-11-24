
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseIsdpGlobal.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseIsdpGlobal_obj.h"
#include "usmdb_isdp_api.h"

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpClearStats
*
* @purpose Set 'IsdpClearStats'
 *@description  [IsdpClearStats] Clear ISDP stats.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpClearStats (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpClearStatsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpClearStats */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpClearStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpClearStatsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpTrafficCountersClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpClearEntries
*
* @purpose Set 'IsdpClearEntries'
 *@description  [IsdpClearEntries] Clear ISDP entries table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpClearEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpClearEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpClearEntries */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpClearEntriesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpClearEntriesValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpNeighborTableClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalRun
*
* @purpose Get 'IsdpGlobalRun'
 *@description  [IsdpGlobalRun] An indication of whether the Industry Standard
* Discovery Protocol is currently running.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalRun (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalRunValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpModeGet (&objIsdpGlobalRunValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalRunValue, sizeof (objIsdpGlobalRunValue));

  /* return the object value: IsdpGlobalRun */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpGlobalRunValue,
                           sizeof (objIsdpGlobalRunValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpGlobalRun
*
* @purpose Set 'IsdpGlobalRun'
 *@description  [IsdpGlobalRun] An indication of whether the Industry Standard
* Discovery Protocol is currently running.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpGlobalRun (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalRunValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpGlobalRun */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpGlobalRunValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalRunValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpModeSet (objIsdpGlobalRunValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalMessageInterval
*
* @purpose Get 'IsdpGlobalMessageInterval'
 *@description  [IsdpGlobalMessageInterval] The interval at which ISDP messages
* are to be generated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalMessageInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalMessageIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpTimerGet (&objIsdpGlobalMessageIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalMessageIntervalValue,
                     sizeof (objIsdpGlobalMessageIntervalValue));

  /* return the object value: IsdpGlobalMessageInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpGlobalMessageIntervalValue,
                           sizeof (objIsdpGlobalMessageIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpGlobalMessageInterval
*
* @purpose Set 'IsdpGlobalMessageInterval'
 *@description  [IsdpGlobalMessageInterval] The interval at which ISDP messages
* are to be generated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpGlobalMessageInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalMessageIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpGlobalMessageInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpGlobalMessageIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalMessageIntervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpTimerSet (objIsdpGlobalMessageIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalHoldTime
*
* @purpose Get 'IsdpGlobalHoldTime'
 *@description  [IsdpGlobalHoldTime] The time for the receiving device holds
* ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalHoldTime (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalHoldTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpHoldTimeGet (&objIsdpGlobalHoldTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalHoldTimeValue, sizeof (objIsdpGlobalHoldTimeValue));

  /* return the object value: IsdpGlobalHoldTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpGlobalHoldTimeValue,
                           sizeof (objIsdpGlobalHoldTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpGlobalHoldTime
*
* @purpose Set 'IsdpGlobalHoldTime'
 *@description  [IsdpGlobalHoldTime] The time for the receiving device holds
* ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpGlobalHoldTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalHoldTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpGlobalHoldTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpGlobalHoldTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalHoldTimeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpHoldTimeSet (objIsdpGlobalHoldTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceId
*
* @purpose Get 'IsdpGlobalDeviceId'
 *@description  [IsdpGlobalDeviceId] The device ID advertised by this device.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (L7_ISDP_DEVICE_ID_LEN);
  xLibS8_t objIsdpGlobalDeviceIdValue[L7_ISDP_DEVICE_ID_LEN];

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = sysapiRegistryGet(SERIAL_NUM, STR_ENTRY, objIsdpGlobalDeviceIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objIsdpGlobalDeviceIdValue, strlen (objIsdpGlobalDeviceIdValue));

  /* return the object value: IsdpGlobalDeviceId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpGlobalDeviceIdValue,
                           strlen (objIsdpGlobalDeviceIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalAdvertiseV2
*
* @purpose Get 'IsdpGlobalAdvertiseV2'
 *@description  [IsdpGlobalAdvertiseV2] An indication of whether the Industry
* Standard Discovery Protocol V2 is currently enabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalAdvertiseV2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalAdvertiseV2Value;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpV2ModeGet (&objIsdpGlobalAdvertiseV2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalAdvertiseV2Value, sizeof (objIsdpGlobalAdvertiseV2Value));

  /* return the object value: IsdpGlobalAdvertiseV2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpGlobalAdvertiseV2Value,
                           sizeof (objIsdpGlobalAdvertiseV2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseIsdpGlobal_IsdpGlobalAdvertiseV2
*
* @purpose Set 'IsdpGlobalAdvertiseV2'
 *@description  [IsdpGlobalAdvertiseV2] An indication of whether the Industry
* Standard Discovery Protocol V2 is currently enabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIsdpGlobal_IsdpGlobalAdvertiseV2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpGlobalAdvertiseV2Value;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IsdpGlobalAdvertiseV2 */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIsdpGlobalAdvertiseV2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIsdpGlobalAdvertiseV2Value, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbIsdpV2ModeSet (objIsdpGlobalAdvertiseV2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceIdFormatCpb
*
* @purpose Get 'IsdpGlobalDeviceIdFormatCpb'
 *@description  [IsdpGlobalDeviceIdFormatCpb] Indicate the Device ID format
* capability of the device.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceIdFormatCpb (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpGlobalDeviceIdFormatCpbValue;
  xLibS8_t deviceIdFormatCpb;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpDeviceIdFormatCapabilityGet(&deviceIdFormatCpb);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmdbIsdpDevIdFormatCapabilityStringGet(deviceIdFormatCpb, objIsdpGlobalDeviceIdFormatCpbValue,
                                                                                    sizeof(xLibStr256_t));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objIsdpGlobalDeviceIdFormatCpbValue,
                     sizeof (objIsdpGlobalDeviceIdFormatCpbValue));

  /* return the object value: IsdpGlobalDeviceIdFormatCpb */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpGlobalDeviceIdFormatCpbValue,
                           strlen (objIsdpGlobalDeviceIdFormatCpbValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceIdFormat
*
* @purpose Get 'IsdpGlobalDeviceIdFormat'
 *@description  [IsdpGlobalDeviceIdFormat] An indication of the format of Device
* ID contained in the corresponding instance of IsdpGlobalDeviceId.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpGlobal_IsdpGlobalDeviceIdFormat (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpGlobalDeviceIdFormatValue;
  xLibU32_t deviceIdFormat;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbIsdpDeviceIdFormatGet(&deviceIdFormat);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmdbIsdpDevIdFormatStringGet(deviceIdFormat, objIsdpGlobalDeviceIdFormatValue,
                                                                      sizeof(xLibStr256_t));
  }

  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objIsdpGlobalDeviceIdFormatValue,
                     sizeof (objIsdpGlobalDeviceIdFormatValue));

  /* return the object value: IsdpGlobalDeviceIdFormat */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objIsdpGlobalDeviceIdFormatValue,
                           strlen (objIsdpGlobalDeviceIdFormatValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
