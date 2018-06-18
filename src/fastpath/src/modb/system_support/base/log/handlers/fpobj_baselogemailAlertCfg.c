/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baselogemailAlertCfg.c
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
#include "_xe_baselogemailAlertCfg_obj.h"
#include "usmdb_log_api.h"

/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_adminStatus
*
* @purpose Get 'adminStatus'
 *@description  [adminStatus] Global Admin mode for email Alert
* Logging    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_adminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertAdminStatusValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertStatusGet (L7_UNIT_CURRENT,  &objLogemailAlertAdminStatusValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertAdminStatusValue, sizeof(objLogemailAlertAdminStatusValue));

  /* return the object value: LogemailAlertAdminStatus */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertAdminStatusValue,
                      sizeof(objLogemailAlertAdminStatusValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_adminStatus
*
* @purpose Set 'adminStatus'
 *@description  [adminStatus] Global Admin mode for email Alert
* Logging    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_adminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertAdminStatusValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertAdminStatus */
  owa.len = sizeof(objLogemailAlertAdminStatusValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objLogemailAlertAdminStatusValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertStatusSet (L7_UNIT_CURRENT, objLogemailAlertAdminStatusValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_fromAddr
*
* @purpose Get 'fromAddr'
 *@description  [fromAddr] email from Address    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_fromAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objLogemailAlertfromAddrValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertFromAddrGet (L7_UNIT_CURRENT,  objLogemailAlertfromAddrValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, objLogemailAlertfromAddrValue, strlen(objLogemailAlertfromAddrValue));

  /* return the object value: LogemailAlertfromAddr */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objLogemailAlertfromAddrValue,
                      strlen(objLogemailAlertfromAddrValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_fromAddr
*
* @purpose Set 'fromAddr'
 *@description  [fromAddr] email from Address    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_fromAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objLogemailAlertfromAddrValue;
  L7_LOG_EMAIL_ALERT_ERROR_t errorValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertfromAddr */
  owa.len = sizeof(objLogemailAlertfromAddrValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)objLogemailAlertfromAddrValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogemailAlertfromAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertFromAddrSet (L7_UNIT_CURRENT, objLogemailAlertfromAddrValue,
                                            &errorValue);
  if (errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID)
  {
    owa.rc = XLIBRC_INVALID_EMAIL_ADDRESS;    /* TODO: Change if required */
  }
  else if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_logDuration
*
* @purpose Get 'logDuration'
 *@description  [logDuration] email alert duration    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_logDuration (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertlogDurationValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertLogTimeGet (L7_UNIT_CURRENT,  &objLogemailAlertlogDurationValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertlogDurationValue, sizeof(objLogemailAlertlogDurationValue));

  /* return the object value: LogemailAlertlogDuration */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertlogDurationValue,
                      sizeof(objLogemailAlertlogDurationValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_logDuration
*
* @purpose Set 'logDuration'
 *@description  [logDuration] email alert duration    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_logDuration (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertlogDurationValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertlogDuration */
  owa.len = sizeof(objLogemailAlertlogDurationValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objLogemailAlertlogDurationValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertlogDurationValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlerLogTimeSet (L7_UNIT_CURRENT, objLogemailAlertlogDurationValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_UrgentSeverity
*
* @purpose Get 'UrgentSeverity'
 *@description  [urgentSeverity] email alert Urgent logging
* severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_urgentSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertCriticalSeverityValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertUrgentSeverityGet (L7_UNIT_CURRENT,  &objLogemailAlertCriticalSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertCriticalSeverityValue, sizeof(objLogemailAlertCriticalSeverityValue));

  /* return the object value: LogemailAlertCriticalSeverity */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertCriticalSeverityValue,
                      sizeof(objLogemailAlertCriticalSeverityValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_criticalSeverity
*
* @purpose Set 'criticalSeverity'
 *@description  [criticalSeverity] email alert Critical logging
* severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_urgentSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertCriticalSeverityValue;
  xLibU32_t nonurgentSeverity;
  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertCriticalSeverity */
  owa.len = sizeof(objLogemailAlertCriticalSeverityValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objLogemailAlertCriticalSeverityValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertCriticalSeverityValue, owa.len);

  owa.l7rc = usmDbLogEmailAlertNonUrgentSeverityGet(USMDB_UNIT_CURRENT, &nonurgentSeverity);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }

  if(objLogemailAlertCriticalSeverityValue > nonurgentSeverity)
  {
    owa.rc = XLIBRC_INVALID_SEVERITY_LEVEL_URGENT;
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertUrgentSeveritySet (L7_UNIT_CURRENT, objLogemailAlertCriticalSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_nonUrgentSeverity
*
* @purpose Get 'nonUrgentSeverity'
 *@description  [nonUrgentSeverity] email alert non urgent
* logging severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_nonUrgentSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertNonCriticalSeverityValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertNonUrgentSeverityGet (L7_UNIT_CURRENT,  &objLogemailAlertNonCriticalSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertNonCriticalSeverityValue, sizeof(objLogemailAlertNonCriticalSeverityValue));

  /* return the object value: LogemailAlertNonCriticalSeverity */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertNonCriticalSeverityValue,
                      sizeof(objLogemailAlertNonCriticalSeverityValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_nonUrgetntSeverity
*
* @purpose Set 'nonUrgentSeverity'
 *@description  [nonUrgentSeverity] email alert non urgent
* logging severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_nonUrgentSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertNonCriticalSeverityValue;
  xLibU32_t urgentSeverity;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertNonCriticalSeverity */
  owa.len = sizeof(objLogemailAlertNonCriticalSeverityValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objLogemailAlertNonCriticalSeverityValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertNonCriticalSeverityValue, owa.len);

  owa.l7rc = usmDbLogEmailAlertUrgentSeverityGet(USMDB_UNIT_CURRENT, &urgentSeverity);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }

  if(objLogemailAlertNonCriticalSeverityValue < urgentSeverity)
  {
    owa.rc = XLIBRC_INVALID_SEVERITY_LEVEL_NON_URGENT;
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertNonUrgentSeveritySet (L7_UNIT_CURRENT, objLogemailAlertNonCriticalSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baselogemailAlertTest_testMessageType
*
* @purpose Get 'testMessageType'
* @description  [testMessageType] Log email Alert Test
* Message Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_testMessageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertTestMessageTypeValue;
  xLibU32_t nextObjLogemailAlertTestMessageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogemailAlertTestMessageType */
  owa.len = sizeof (objLogemailAlertTestMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertCfg_testMessageType,
                          (xLibU8_t *) & objLogemailAlertTestMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLogemailAlertTestMessageTypeValue = L7_LOG_EMAIL_ALERT_URGENT; 
    owa.l7rc = L7_SUCCESS ;
  }
  else
  {  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertTestMessageTypeValue, owa.len);
    if(objLogemailAlertTestMessageTypeValue == L7_LOGGING_EMAIL_ALERT_BOTH)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjLogemailAlertTestMessageTypeValue = objLogemailAlertTestMessageTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }    
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogemailAlertTestMessageTypeValue, owa.len);

  /* return the object value: LogemailAlertTestMessageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogemailAlertTestMessageTypeValue,
                           sizeof (nextObjLogemailAlertTestMessageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertTest_testMessageBody
*
* @purpose Set 'testMessageBody'
 *@description  [testMessageBody] Email Alert Message Body.   
* @notes       
*
* @return
*******************************************************************************/
 xLibRC_t fpObjSet_baselogemailAlertCfg_testMessageBody (void *wap, void *bufp)
 {
   fpObjWa_t owa = FPOBJ_INIT_WA2();
   xLibU32_t objLogemailAlertTestMessageTypeValue;    
   xLibStr256_t objLogemailAlertTestMessageBodyValue;
 
   FPOBJ_TRACE_ENTER(bufp);
 
   /* retrieve object: LogemailAlertTestMessageBody */
   owa.len = sizeof(objLogemailAlertTestMessageBodyValue);
   owa.rc = xLibBufDataGet(bufp,
               (xLibU8_t *)objLogemailAlertTestMessageBodyValue,
               &owa.len);
   if(owa.rc != XLIBRC_SUCCESS)
   {
     FPOBJ_TRACE_EXIT(bufp, owa);
     return owa.rc;
   }
   FPOBJ_TRACE_VALUE (bufp, objLogemailAlertTestMessageBodyValue, owa.len);
 
   /* retrieve key: LogemailAlertTestMessageType */
   owa.len = sizeof(objLogemailAlertTestMessageTypeValue);
   owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertCfg_testMessageType,
                         (xLibU8_t *) & objLogemailAlertTestMessageTypeValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   {
     owa.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }  
   
   FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertTestMessageTypeValue, owa.len);
 
   /* set the value in application */
   if(objLogemailAlertTestMessageTypeValue == L7_LOG_EMAIL_ALERT_NON_URGENT )
   {
     L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CLI_WEB_COMPONENT_ID,"%s",
               objLogemailAlertTestMessageBodyValue);
   }
   else if(objLogemailAlertTestMessageTypeValue == L7_LOG_EMAIL_ALERT_URGENT)
   {
     L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_CLI_WEB_COMPONENT_ID,"%s",
               objLogemailAlertTestMessageBodyValue);  
   }
   else if(objLogemailAlertTestMessageTypeValue == L7_LOGGING_EMAIL_ALERT_BOTH)
   {
     L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CLI_WEB_COMPONENT_ID,"%s",
               objLogemailAlertTestMessageBodyValue);
     L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_CLI_WEB_COMPONENT_ID,"%s",
               objLogemailAlertTestMessageBodyValue); 
   }
   else
   {
     owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
   }
   FPOBJ_TRACE_EXIT(bufp, owa);
   return owa.rc;
 
 }
 
/*******************************************************************************
* @function fpObjGet_baselogemailAlertCfg_trapsSeverity
*
* @purpose Get 'trapsSeverity'
* @description  [trapsSeverity] email alert traps severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertCfg_trapsSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertTrapsSeverityValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertTrapSeverityGet(L7_UNIT_CURRENT,
                                               &objLogemailAlertTrapsSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertTrapsSeverityValue,
                     sizeof(objLogemailAlertTrapsSeverityValue));

  /* return the object value: LogemailAlertTrapsSeverity */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertTrapsSeverityValue,
                      sizeof(objLogemailAlertTrapsSeverityValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertCfg_trapsSeverity
*
* @purpose Set 'trapsSeverity'
* @description  [trapsSeverity] email alert traps severity    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertCfg_trapsSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertTrapsSeverityValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertTrapsSeverity */
  owa.len = sizeof(objLogemailAlertTrapsSeverityValue);
  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&objLogemailAlertTrapsSeverityValue,
                          &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertTrapsSeverityValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertTrapSeveritySet(L7_UNIT_CURRENT,
                                               objLogemailAlertTrapsSeverityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

