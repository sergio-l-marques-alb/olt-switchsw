
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baselogemailAlertStatistics.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  02 December 2009, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baselogemailAlertStatistics_obj.h"
#include "usmdb_log_api.h"

/*******************************************************************************
* @function fpObjGet_baselogemailAlertStatistics_statsemailsSentCount
*
* @purpose Get 'statsemailsSentCount'
 *@description  [statsemailsSentCount] No Of emails Sent So far    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertStatistics_statsemailsSentCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertStatsemailsSentCountValue;
  emailAlertingStatsData_t emailAlertStatistics;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc =usmDbLogEmailAlertStatsGet(USMDB_UNIT_CURRENT,&emailAlertStatistics);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }
  objLogemailAlertStatsemailsSentCountValue= emailAlertStatistics.noEmailsSent;
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertStatsemailsSentCountValue, sizeof(objLogemailAlertStatsemailsSentCountValue));

  /* return the object value: LogemailAlertStatsemailsSentCount */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertStatsemailsSentCountValue,
                      sizeof(objLogemailAlertStatsemailsSentCountValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertStatistics_statsemailsFailureCount
*
* @purpose Get 'statsemailsFailureCount'
 *@description  [statsemailsFailureCount] No Of emails Failed so
* far    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertStatistics_statsemailsFailureCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertsStatsemailsFailureCountValue;
  emailAlertingStatsData_t emailAlertStatistics;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc =usmDbLogEmailAlertStatsGet(USMDB_UNIT_CURRENT,&emailAlertStatistics);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }
  objLogemailAlertsStatsemailsFailureCountValue= emailAlertStatistics.noEmailFailures;
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertsStatsemailsFailureCountValue, sizeof(objLogemailAlertsStatsemailsFailureCountValue));

  /* return the object value: LogemailAlertsStatsemailsFailureCount */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertsStatsemailsFailureCountValue,
                      sizeof(objLogemailAlertsStatsemailsFailureCountValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertStatistics_statsTimeSinceLastEmail
*
* @purpose Get 'statsTimeSinceLastEmail'
 *@description  [statsTimeSinceLastEmail] Time since last email    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertStatistics_statsTimeSinceLastEmail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertStatsTimeSinceLastEmailValue;
  emailAlertingStatsData_t emailAlertStatistics;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc =usmDbLogEmailAlertStatsGet(USMDB_UNIT_CURRENT,&emailAlertStatistics);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }
  /* return the value in time ticks */
  objLogemailAlertStatsTimeSinceLastEmailValue= (emailAlertStatistics.timeSinceLastMail * 100);
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertStatsTimeSinceLastEmailValue, sizeof(objLogemailAlertStatsTimeSinceLastEmailValue));

  /* return the object value: LogemailAlertStatsTimeSinceLastEmail */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertStatsTimeSinceLastEmailValue,
                      sizeof(objLogemailAlertStatsTimeSinceLastEmailValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertStatistics_statsClear
*
* @purpose Set 'statsClear'
 *@description  [statsClear] This is to clear the stats    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertStatistics_statsClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertStatsClearValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertStatsClear */
  owa.len = sizeof(objLogemailAlertStatsClearValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objLogemailAlertStatsClearValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertStatsClearValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertStatsClear(L7_UNIT_CURRENT);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

