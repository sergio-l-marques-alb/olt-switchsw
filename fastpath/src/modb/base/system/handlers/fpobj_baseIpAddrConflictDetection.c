/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseIpAddrConflictDetection.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  15 January 2009
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseIpAddrConflictDetection_obj.h"
#include "usmdb_sim_api.h"

/*******************************************************************************
* @function fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictDetectStatus
*
* @purpose Get 'sysIPAddrConflictDetectStatus'
*
* @description  Status of the last detected IP address conflict in the system.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictDetectStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_BOOL objsysIPAddrConflictDetectStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIPAddrConflictDetectStatusGet(&objsysIPAddrConflictDetectStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIPAddrConflictDetectStatusValue, sizeof (objsysIPAddrConflictDetectStatusValue));

  /* return the object value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysIPAddrConflictDetectStatusValue,
                           sizeof (objsysIPAddrConflictDetectStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastDetectIP
*
* @purpose Get 'sysIPAddrConflictLastDetectIP'
*
* @description  The last detected IP that has address conflict.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastDetectIP (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIPAddrConflictLastDetectIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIPAddrConflictLastDetectIPGet (&objsysIPAddrConflictLastDetectIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIPAddrConflictLastDetectIPValue,
                     sizeof (objsysIPAddrConflictLastDetectIPValue));

  /* return the object value */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysIPAddrConflictLastDetectIPValue,
                    sizeof (objsysIPAddrConflictLastDetectIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
  
/*******************************************************************************
* @function fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastDetectMAC
*
* @purpose Get 'sysIPAddrConflictLastDetectMAC'
*
* @description The mac address corresponding to the last detected conflicting IP.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastDetectMAC (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objsysIPAddrConflictLastDetectMACValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
  usmDbIPAddrConflictLastDetectMACGet (objsysIPAddrConflictLastDetectMACValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysIPAddrConflictLastDetectMACValue,
                      L7_MAC_ADDR_LEN);

  /* return the object value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysIPAddrConflictLastDetectMACValue,
                      L7_MAC_ADDR_LEN);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
  
/*******************************************************************************
* @function fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastReportedTime
*
* @purpose Get 'sysIPAddrConflictLastReportedTime'
*
* @description  Time since the last IP address conflict was detected in the system.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpAddrConflictDetection_sysIPAddrConflictLastReportedTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIPAddrConflictLastReportedTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIPAddrConflictLastDetectTimeGet (&objsysIPAddrConflictLastReportedTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the uptime value in time ticks */
  objsysIPAddrConflictLastReportedTimeValue = (objsysIPAddrConflictLastReportedTimeValue * 100);

  /* return the object value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysIPAddrConflictLastReportedTimeValue,
                           sizeof (objsysIPAddrConflictLastReportedTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseIpAddrConflictDetection_sysIPAddrConflictDetectStatusClear
*
* @purpose Reset the 'sysIPAddrConflictDetectStatus '
*
* @description  Reset the status of the last detected IP address conflict in the system.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpAddrConflictDetection_sysIPAddrConflictDetectStatusClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIPAddrConflictDetectStatusClearValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysIPAddrConflictDetectStatusClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIPAddrConflictDetectStatusClearValue, owa.len);
  /* set the value in application */
  owa.l7rc = usmDbIPAddrConflictDetectStatusClear ();
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
* @function fpObjSet_baseIpAddrConflictDetection_sysIPAddrConflictDetectRun
*
* @purpose Triggers the 'sysIPAddrConflictDetectRun '
*
* @description  Triggers the active IP address conflict detection in the system.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpAddrConflictDetection_sysIPAddrConflictDetectRun (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysIPAddrConflictDetectRunValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysIPAddrConflictDetectRunValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysIPAddrConflictDetectRunValue, owa.len);
  /* set the value in application */
  owa.l7rc = usmDbIPAddrConflictDetectRun ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}  
