
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseTrapLog.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 April 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseTrapLog_obj.h"
#include "usmdb_trapmgr_api.h"
#include "trapmgr_exports.h"
#include "usmdb_common.h"

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogTotalSinceLastReset
*
* @purpose Get 'TrapLogTotalSinceLastReset'
*
* @description [TrapLogTotalSinceLastReset] total TrapLog Since LastReset
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogTotalSinceLastReset (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapLogTotalSinceLastResetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbInfoTrapsSinceLastResetSwGet (L7_UNIT_CURRENT, &objTrapLogTotalSinceLastResetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLogTotalSinceLastResetValue,
                     sizeof (objTrapLogTotalSinceLastResetValue));

  /* return the object value: TrapLogTotalSinceLastReset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLogTotalSinceLastResetValue,
                           sizeof (objTrapLogTotalSinceLastResetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogTotalSinceLastViewed
*
* @purpose Get 'TrapLogTotalSinceLastViewed'
*
* @description [TrapLogTotalSinceLastViewed] total TrapLog Since Last viewed
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogTotalSinceLastViewed (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapLogTotalSinceLastViewedValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapNumUnDisplayedTrapEntriesGet (L7_UNIT_CURRENT, &objTrapLogTotalSinceLastViewedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLogTotalSinceLastViewedValue,
                     sizeof (objTrapLogTotalSinceLastViewedValue));

  /* return the object value: TrapLogTotalSinceLastViewed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLogTotalSinceLastViewedValue,
                           sizeof (objTrapLogTotalSinceLastViewedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogCapacity
*
* @purpose Get 'TrapLogCapacity'
*
* @description [TrapLogCapacity] Total allowed Trap Log Capacity
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogCapacity (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapLogCapacityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objTrapLogCapacityValue = TRAPMGR_TABLE_SIZE;

  FPOBJ_TRACE_VALUE (bufp, &objTrapLogCapacityValue, sizeof (objTrapLogCapacityValue));

  /* return the object value: TrapLogCapacity */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLogCapacityValue,
                           sizeof (objTrapLogCapacityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapLog_TrapLogClear
*
* @purpose Set 'TrapLogClear'
*
* @description [TrapLogClear] Clear Traps logged
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapLog_TrapLogClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapLogClearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLogClear */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLogClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLogClearValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCtrlClearTrapLogSwSet (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogIndex
*
* @purpose Get 'TrapLogIndex'
*
* @description [TrapLogIndex] Trap Log Index
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrapLogIndexValue;
  xLibU32_t nextObjTrapLogIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  usmDbTrapLogEntry_t trapLogEntry;
  /* retrieve key: TrapLogIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTrapLog_TrapLogIndex,
                          (xLibU8_t *) & objTrapLogIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjTrapLogIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTrapLogIndexValue, owa.len);
   	nextObjTrapLogIndexValue = objTrapLogIndexValue + 1 ;
  }

  owa.l7rc = usmDbTrapLogEntryGet(L7_UNIT_CURRENT, USMDB_USER_DISPLAY, nextObjTrapLogIndexValue, &trapLogEntry);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTrapLogIndexValue, owa.len);

  /* return the object value: TrapLogIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTrapLogIndexValue,
                           sizeof (objTrapLogIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogSystemUptime
*
* @purpose Get 'TrapLogSystemUptime'
*
* @description [TrapLogSystemUptime] Trap Log system Up time
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogSystemUptime (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapLogIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapLogIndexValue;
  xLibU32_t keyTrapLogIndexTemp;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTrapLogSystemUptimeValue;
  FPOBJ_TRACE_ENTER (bufp);
  usmDbTrapLogEntry_t trapLogEntry;

  /* retrieve key: TrapLogIndex */
  kwaTrapLogIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapLog_TrapLogIndex,
                                      (xLibU8_t *) & keyTrapLogIndexValue, &kwaTrapLogIndex.len);
  if (kwaTrapLogIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapLogIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapLogIndex);
    return kwaTrapLogIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapLogIndexValue, kwaTrapLogIndex.len);

  keyTrapLogIndexTemp = usmDbTrapLogEntryLastReceivedGet(); 

  keyTrapLogIndexValue = keyTrapLogIndexTemp - keyTrapLogIndexValue;

  /* get the value from application */
  owa.l7rc = usmDbTrapLogEntryGet(L7_UNIT_CURRENT, USMDB_USER_NONE, keyTrapLogIndexValue, &trapLogEntry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(objTrapLogSystemUptimeValue,trapLogEntry.timestamp);

  /* return the object value: TrapLogSystemUptime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTrapLogSystemUptimeValue,
                           strlen (objTrapLogSystemUptimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTrapLog_TrapLogTrapMessage
*
* @purpose Get 'TrapLogTrapMessage'
*
* @description [TrapLogTrapMessage] Trap log message
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapLog_TrapLogTrapMessage (void *wap, void *bufp)
{

  fpObjWa_t kwaTrapLogIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTrapLogIndexValue;
  xLibU32_t keyTrapLogIndexTemp;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTrapLogTrapMessageValue;
  FPOBJ_TRACE_ENTER (bufp);
  usmDbTrapLogEntry_t trapLogEntry;

  /* retrieve key: TrapLogIndex */
  kwaTrapLogIndex.rc = xLibFilterGet (wap, XOBJ_baseTrapLog_TrapLogIndex,
                                      (xLibU8_t *) & keyTrapLogIndexValue, &kwaTrapLogIndex.len);
  if (kwaTrapLogIndex.rc != XLIBRC_SUCCESS)
  {
    kwaTrapLogIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTrapLogIndex);
    return kwaTrapLogIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTrapLogIndexValue, kwaTrapLogIndex.len);

  keyTrapLogIndexTemp = usmDbTrapLogEntryLastReceivedGet(); 

  keyTrapLogIndexValue = keyTrapLogIndexTemp - keyTrapLogIndexValue;


  /* get the value from application */
  owa.l7rc = usmDbTrapLogEntryGet(L7_UNIT_CURRENT, USMDB_USER_NONE, keyTrapLogIndexValue, &trapLogEntry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiStrncpySafe(objTrapLogTrapMessageValue,trapLogEntry.message, sizeof(objTrapLogTrapMessageValue));

  /* return the object value: TrapLogTrapMessage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTrapLogTrapMessageValue,
                           strlen (objTrapLogTrapMessageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
