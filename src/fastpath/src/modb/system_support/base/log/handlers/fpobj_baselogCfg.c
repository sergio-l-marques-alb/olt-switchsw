/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baselogCfg.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baselogCfg_obj.h"
#include "usmdb_cmd_logger_api.h"
#include "usmdb_common.h"
#include "usmdb_log_api.h"
#include "usmdb_util_api.h"	
#include "defaultconfig.h"
#include "commdefs.h"


/*******************************************************************************
* @function fpObjGet_baselogCfg_GlobalLogAdminStatus
*
* @purpose Get 'GlobalLogAdminStatus'
*
* @description global Admin mode for Logging 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_GlobalLogAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objGlobalLogAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objGlobalLogAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogGlobalAdminStatusGet (
                                &objGlobalLogAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalLogAdminStatusValue,
                     sizeof (objGlobalLogAdminStatusValue));

  /* return the object value: GlobalLogAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalLogAdminStatusValue,
                    sizeof (objGlobalLogAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_SwitchAuditAdminStatus
*
* @purpose Get 'SwitchAuditAdminStatus'
*
* @description Admin mode for auditing
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_SwitchAuditAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objAuditAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objAuditAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCmdLoggerAuditAdminModeGet (&objAuditAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuditAdminStatusValue,
                     sizeof (objAuditAdminStatusValue));

  /* return the object value: AuditAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) &objAuditAdminStatusValue,
                    sizeof (objAuditAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_CliLoggingAdminStatus
*
* @purpose Get 'CliLoggingAdminStatus'
*
* @description Admin mode for cli commands logging
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_CliLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objCliLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objCliLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCmdLoggerAdminModeGet(&objCliLoggingAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCliLoggingAdminStatusValue,
                     sizeof (objCliLoggingAdminStatusValue));

  /* return the object value: CliLoggingAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) &objCliLoggingAdminStatusValue,
                    sizeof (objCliLoggingAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_WebLoggingAdminStatus
*
* @purpose Get 'WebLoggingAdminStatus'
*
* @description Admin mode for web logging
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_WebLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objWebLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objWebLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCmdLoggerWebAdminModeGet(&objWebLoggingAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objWebLoggingAdminStatusValue,
                     sizeof (objWebLoggingAdminStatusValue));

  /* return the object value: WebLoggingAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) &objWebLoggingAdminStatusValue,
                    sizeof (objWebLoggingAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_SnmpLoggingAdminStatus
*
* @purpose Get 'SnmpLoggingAdminStatus'
*
* @description Admin mode for Snmp logging
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_SnmpLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objSnmpLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objSnmpLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCmdLoggerSnmpAdminModeGet(&objSnmpLoggingAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnmpLoggingAdminStatusValue,
                     sizeof (objSnmpLoggingAdminStatusValue));

  /* return the object value: SnmpLoggingAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) &objSnmpLoggingAdminStatusValue,
                    sizeof (objSnmpLoggingAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_GlobalLogAdminStatus
*
* @purpose Set 'GlobalLogAdminStatus'
*
* @description global Admin mode for Logging 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_GlobalLogAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objGlobalLogAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objGlobalLogAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objGlobalLogAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalLogAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogGlobalAdminStatusSet (objGlobalLogAdminStatusValue);
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
* @function fpObjSet_baselogCfg_SwitchAuditAdminStatus
*
* @purpose Set 'SwitchAuditAdminStatus'
*
* @description mode for switch auditing
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_SwitchAuditAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objAuditAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objAuditAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objAuditAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuditAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCmdLoggerAuditAdminModeSet(objAuditAdminStatusValue);
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
* @function fpObjSet_baselogCfg_CliLoggingAdminStatus
*
* @purpose Set 'CliLoggingAdminStatus'
*
* @description global Admin mode for Cli logging 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_CliLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objCliLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objCliLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCliLoggingAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCliLoggingAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCmdLoggerAdminModeSet(objCliLoggingAdminStatusValue);
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
* @function fpObjSet_baselogCfg_WebLoggingAdminStatus
*
* @purpose Set 'WebLoggingAdminStatus'
*
* @description global Admin mode for Web logging 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_WebLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objWebLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objWebLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objWebLoggingAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objWebLoggingAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCmdLoggerWebAdminModeSet(objWebLoggingAdminStatusValue);
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
* @function WebLoggingAdminStatus
*
* @purpose Set 'WebLoggingAdminStatus'
*
* @description global Admin mode for SNMP logging 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_SnmpLoggingAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objSnmpLoggingAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objSnmpLoggingAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalLogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objSnmpLoggingAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnmpLoggingAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCmdLoggerSnmpAdminModeSet(objSnmpLoggingAdminStatusValue);
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
* @function fpObjGet_baselogCfg_LogMsgPersistentMsgIndex
*
* @purpose Get 'LogMsgPersistentMsgIndex'
*
* @description Table index for logging message in persistent log table. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogMsgPersistentMsgIndex (void *wap, void *bufp)
{
  xLibU32_t objLogMsgPersistentMsgIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogMsgPersistentMsgIndexValue));
  xLibU32_t nextObjLogMsgPersistentMsgIndexValue;
  xLibS8_t tempBuffer[L7_LOG_MESSAGE_LENGTH];
  memset(tempBuffer,0x00,sizeof(tempBuffer));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogMsgPersistentMsgIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogMsgPersistentMsgIndex,
                          (xLibU8_t *) & objLogMsgPersistentMsgIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLogMsgPersistentMsgIndexValue = 0;
    objLogMsgPersistentMsgIndexValue  = 0;
    
    owa.l7rc =
           usmDbLogMsgPersistentGetNext (L7_ALL_UNITS,
                                         objLogMsgPersistentMsgIndexValue,tempBuffer,
                                         &nextObjLogMsgPersistentMsgIndexValue);
  }
  else
  { 
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogMsgPersistentMsgIndexValue, owa.len);
    owa.l7rc =
         usmDbLogMsgPersistentGetNext (L7_ALL_UNITS,
                                       objLogMsgPersistentMsgIndexValue,tempBuffer,
                                       &nextObjLogMsgPersistentMsgIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogMsgPersistentMsgIndexValue, owa.len);

  /* return the object value: LogMsgPersistentMsgIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogMsgPersistentMsgIndexValue,
                    sizeof (objLogMsgPersistentMsgIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogMsgPersistentMsgText
*
* @purpose Get 'LogMsgPersistentMsgText'
*
* @description The text corresponding to persistent log message. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogMsgPersistentMsgText (void *wap, void *bufp)
{
  xLibU32_t keyLogMsgPersistentMsgIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogMsgPersistentMsgIndexValue));
  xLibS8_t objLogMsgPersistentMsgTextValue[L7_LOG_MESSAGE_LENGTH];
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogMsgPersistentMsgTextValue));

  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogMsgPersistentMsgTextValue,0x0,sizeof(objLogMsgPersistentMsgTextValue));

  /* retrieve key: LogMsgPersistentMsgIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogMsgPersistentMsgIndex,
                          (xLibU8_t *) & keyLogMsgPersistentMsgIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogMsgPersistentMsgIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogMsgPersistentGet (L7_ALL_UNITS, keyLogMsgPersistentMsgIndexValue,
                              objLogMsgPersistentMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  (void)stripQuotes(objLogMsgPersistentMsgTextValue);
  /* return the object value: LogMsgPersistentMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLogMsgPersistentMsgTextValue,
                           strlen (objLogMsgPersistentMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryMsgIndex
*
* @purpose Get 'LogInMemoryMsgIndex'
*
* @description The index to message entry in the in-memory log table. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryMsgIndex (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryMsgIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryMsgIndexValue));
  xLibU32_t nextObjLogInMemoryMsgIndexValue;
  xLibS8_t tempBuffer[L7_LOG_MESSAGE_LENGTH];

  memset(tempBuffer,0x0,sizeof(tempBuffer));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogInMemoryMsgIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogInMemoryMsgIndex,
                          (xLibU8_t *) & objLogInMemoryMsgIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objLogInMemoryMsgIndexValue = 0;
    nextObjLogInMemoryMsgIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogInMemoryMsgIndexValue, owa.len);
  owa.l7rc =
      usmDbLogMsgInMemoryGetNext (L7_UNIT_CURRENT, objLogInMemoryMsgIndexValue,
                                  tempBuffer,&nextObjLogInMemoryMsgIndexValue);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogInMemoryMsgIndexValue, owa.len);

  /* return the object value: LogInMemoryMsgIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogInMemoryMsgIndexValue,
                           sizeof (nextObjLogInMemoryMsgIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryMsgText
*
* @purpose Get 'LogInMemoryMsgText'
*
* @description Message text info for inmemory logged messages. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryMsgText (void *wap, void *bufp)
{
  xLibU32_t keyLogInMemoryMsgIndexValue=0;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogInMemoryMsgIndexValue));
  xLibS8_t objLogInMemoryMsgTextValue[L7_LOG_MESSAGE_LENGTH];
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryMsgTextValue));
  xLibU32_t nextObjLogInMemoryMsgIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogInMemoryMsgTextValue,0x0,L7_LOG_MESSAGE_LENGTH);

  /* retrieve key: LogInMemoryMsgIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogInMemoryMsgIndex,
                          (xLibU8_t *) & keyLogInMemoryMsgIndexValue, &kwa.len);

   if (kwa.rc != XLIBRC_SUCCESS)
   {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;     
   }
  /* get the value from application */
    owa.l7rc = usmDbLogMsgInMemoryGetNext (L7_UNIT_CURRENT, keyLogInMemoryMsgIndexValue-1,
                             objLogInMemoryMsgTextValue,&nextObjLogInMemoryMsgIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  (void)stripQuotes(objLogInMemoryMsgTextValue);
  /* return the object value: LogInMemoryMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLogInMemoryMsgTextValue,
                           strlen (objLogInMemoryMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryBehavior
*
* @purpose Get 'LogInMemoryBehavior'
*
* @description Configures the behavior of the In Memory Log when it becomes full. 
*              A value of wrap(1) will cause the oldest log message to 
*              be removed, making room for the new message. A value of stop-on-full(2) 
*              will prevent any further logging. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryBehavior (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryBehaviorValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryBehaviorValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogInMemoryBehaviorGet (L7_UNIT_CURRENT, &objLogInMemoryBehaviorValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryBehaviorValue,
                     sizeof (objLogInMemoryBehaviorValue));

  /* return the object value: LogInMemoryBehavior */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemoryBehaviorValue,
                           sizeof (objLogInMemoryBehaviorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemoryBehavior
*
* @purpose Set 'LogInMemoryBehavior'
*
* @description Configures the behavior of the In Memory Log when it becomes full. 
*              A value of wrap(1) will cause the oldest log message to 
*              be removed, making room for the new message. A value of stop-on-full(2) 
*              will prevent any further logging. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemoryBehavior (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryBehaviorValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryBehaviorValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemoryBehavior */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogInMemoryBehaviorValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryBehaviorValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogInMemoryBehaviorSet (L7_UNIT_CURRENT, objLogInMemoryBehaviorValue);
  if (owa.l7rc != L7_SUCCESS &&  owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemoryClear
*
* @purpose Set 'LogInMemoryClear'
*
* @description Clears all the In Memory Logs. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemoryClear (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryClearValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryClearValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemoryClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogInMemoryClearValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryClearValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogInMemoryLogClear (L7_UNIT_CURRENT);
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
* @function fpObjGet_baselogCfg_LogHostTableIndex
*
* @purpose Get 'LogHostTableIndex'
*
* @description Index to syslog host entry in syslog host table. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTableIndex (void *wap, void *bufp)
{
  xLibU32_t objLogHostTableIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableIndexValue));
  xLibU32_t nextObjLogHostTableIndexValue;
  L7_ROW_STATUS_t temp_status;
  xLibU32_t temp_index = 1;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogHostTableIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & objLogHostTableIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLogHostTableIndexValue = 0;
	objLogHostTableIndexValue = 0;
  }
	
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogHostTableIndexValue, owa.len);
  temp_index = objLogHostTableIndexValue+1;
  owa.l7rc =  L7_FAILURE;
  while (temp_index <= L7_LOG_MAX_HOSTS)
  {
    if  (usmDbLogHostTableRowStatusGet(USMDB_UNIT_CURRENT, temp_index, &temp_status) == L7_SUCCESS)
    {
      /* check if it's a valid entry */
      if (temp_status != L7_ROW_STATUS_INVALID &&
          temp_status != L7_ROW_STATUS_DESTROY)
      {
        nextObjLogHostTableIndexValue = temp_index;
        owa.l7rc =  L7_SUCCESS;
      break;
      }
    }
    temp_index++;
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogHostTableIndexValue, owa.len);

  /* return the object value: LogHostTableIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogHostTableIndexValue,
                           sizeof (objLogHostTableIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogHostTableIpAddress
*
* @purpose Get 'LogHostTableIpAddress'
*
* @description Syslog Host table IP Address. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTableIpAddress (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibStr256_t objLogHostTableIpAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressValue));
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogHostTableIpAddressValue,0x0,sizeof(objLogHostTableIpAddressValue)); 
  
  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogHostTableIpAddressGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                   objLogHostTableIpAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogHostTableIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLogHostTableIpAddressValue,
                           strlen (objLogHostTableIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogHostTableIpAddress
*
* @purpose Set 'LogHostTableIpAddress'
*
* @description Syslog Host table IP Address. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogHostTableIpAddress (void *wap, void *bufp)
{
  xLibStr256_t objLogHostTableIpAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressValue));
 
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  
  L7_uint32  ipaddr = L7_NULL;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogHostTableIpAddressValue,0x0,sizeof(objLogHostTableIpAddressValue));
  
   /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);


  /* retrieve object: LogHostTableIpAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objLogHostTableIpAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogHostTableIpAddressValue, owa.len);
  
  owa.l7rc = usmDbIPHostAddressValidate(objLogHostTableIpAddressValue, &ipaddr, &addrType);
  if (owa.l7rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    owa.rc = XLIBRC_INVALID_IP_ADDRESS;  /* TODO: add error string */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc == L7_FAILURE)
  {
    /* Invalid Host Name*/
    owa.rc = XLIBRC_INVALID_HOST_NAME;  /* TODO: add error string */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbLogHostTableHostSet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                              objLogHostTableIpAddressValue);
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
* @function fpObjGet_baselogCfg_LogHostTableIpAddressType
*
* @purpose Get 'LogHostTableIpAddressType'
*
* @description Syslog Host table IP Address Type. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTableIpAddressType (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibU32_t objLogHostTableIpAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressTypeValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogHostTableAddressTypeGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                     &objLogHostTableIpAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogHostTableIpAddressType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogHostTableIpAddressTypeValue,
                    sizeof (objLogHostTableIpAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogHostTableIpAddressType
*
* @purpose Set 'LogHostTableIpAddressType'
*
* @description Syslog Host table IP Address Type. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogHostTableIpAddressType (void *wap, void *bufp)
{
  xLibU32_t objLogHostTableIpAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressTypeValue));
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogHostTableIpAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogHostTableIpAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogHostTableIpAddressTypeValue, owa.len);


  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogHostTableAddressTypeSet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                     objLogHostTableIpAddressTypeValue);
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
* @function fpObjGet_baselogCfg_LogHostTablePort
*
* @purpose Get 'LogHostTablePort'
*
* @description Syslog Host table port number. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTablePort (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibU32_t objLogHostTablePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTablePortValue));
  xLibU16_t portNum = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogHostTablePortGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue, &portNum);
  objLogHostTablePortValue = (xLibU32_t)portNum;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogHostTablePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogHostTablePortValue,
                           sizeof (objLogHostTablePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogHostTablePort
*
* @purpose Set 'LogHostTablePort'
*
* @description Syslog Host table port number. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogHostTablePort (void *wap, void *bufp)
{
  xLibU32_t objLogHostTablePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTablePortValue));
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogHostTablePort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogHostTablePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogHostTablePortValue, owa.len);


  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);
  

  /* set the value in application */
  owa.l7rc =
    usmDbLogHostTablePortSet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                              (xLibU16_t)objLogHostTablePortValue);
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
* @function fpObjGet_baselogCfg_LogHostTableSeverityFilter
*
* @purpose Get 'LogHostTableSeverityFilter'
*
* @description Configures the minimum severity that will be stored in the In 
*              Memory log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTableSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibU32_t objLogHostTableSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableSeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogHostTableSeverityGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                  &objLogHostTableSeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogHostTableSeverityFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogHostTableSeverityFilterValue,
                    sizeof (objLogHostTableSeverityFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogHostTableSeverityFilter
*
* @purpose Set 'LogHostTableSeverityFilter'
*
* @description Configures the minimum severity that will be stored in the In 
*              Memory log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogHostTableSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogHostTableSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableSeverityFilterValue));
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogHostTableSeverityFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogHostTableSeverityFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogHostTableSeverityFilterValue, owa.len);


  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogHostTableSeveritySet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                  objLogHostTableSeverityFilterValue);
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
* @function fpObjGet_baselogCfg_LogHostTableDescription
*
* @purpose Get 'LogHostTableDescription'
*
* @description the server description. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogHostTableDescription (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibStr256_t objLogHostTableDescriptionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableDescriptionValue));
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogHostTableDescriptionValue,0x0,sizeof(objLogHostTableDescriptionValue)); 
  
  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLogHostTableDescriptionGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                                   objLogHostTableDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogHostTableDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLogHostTableDescriptionValue,
                           strlen (objLogHostTableDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogHostTableDescription
*
* @purpose Set 'LogHostTableDescription'
*
* @description the server description. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogHostTableDescription (void *wap, void *bufp)
{
  xLibStr256_t objLogHostTableDescriptionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogHostTableDescriptionValue));
 
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogHostTableDescriptionValue,0x0,sizeof(objLogHostTableDescriptionValue));
  
   /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);


  /* retrieve object: LogHostTableDescription */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objLogHostTableDescriptionValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogHostTableDescriptionValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogHostTableDescriptionSet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                              objLogHostTableDescriptionValue);
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
* @function fpObjGet_baselogCfg_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: Syslog Host table row status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_RowStatus (void *wap, void *bufp)
{
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));
  xLibU32_t objRowStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objRowStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogHostTableRowStatusGet (L7_UNIT_CURRENT, keyLogHostTableIndexValue,
                              &objRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: Syslog Host table row status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_RowStatus (void *wap, void *bufp)
{
  xLibU32_t objRowStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objRowStatusValue));
  xLibU32_t keyLogHostTableIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (keyLogHostTableIndexValue));

  xLibStr256_t objLogHostTableIpAddressValue;
  fpObjWa_t owaIpAddressValue = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressValue));

  xLibU32_t objLogHostTableIpAddressTypeValue;
  fpObjWa_t owaIpAddressTypeValue = FPOBJ_INIT_WA (sizeof (objLogHostTableIpAddressTypeValue));

  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32  ipAddr = L7_NULL;
	xLibStr256_t tempIpAddrVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);


  if(objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
	/* Get the ip address */

	/* retrieve object: LogHostTableIpAddress */
	owaIpAddressValue.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIpAddress,
                          (xLibU8_t *) objLogHostTableIpAddressValue,
	                         &owaIpAddressValue.len);

	if (owaIpAddressValue.rc != XLIBRC_SUCCESS)
	{
	  FPOBJ_TRACE_EXIT (bufp, owaIpAddressValue);
	  return owaIpAddressValue.rc;
	}
	FPOBJ_TRACE_VALUE (bufp, objLogHostTableIpAddressValue, owaIpAddressValue.len);

	/* Get the ip address Type */
	owaIpAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIpAddressType,
	                         (xLibU8_t *) & objLogHostTableIpAddressTypeValue,
	                         &owaIpAddressTypeValue.len);
	if (owaIpAddressTypeValue.rc != XLIBRC_SUCCESS)
	{
	  FPOBJ_TRACE_EXIT (bufp, owaIpAddressTypeValue);
	  return owa.rc;
	}
	FPOBJ_TRACE_VALUE (bufp, &objLogHostTableIpAddressTypeValue, owaIpAddressTypeValue.len);

	owa.l7rc = usmDbIPHostAddressValidate(objLogHostTableIpAddressValue, 
		                                 &ipAddr, &addrType);
	if (owa.l7rc == L7_ERROR)
        {
          /* Invalid Ip Address */
          owa.rc = XLIBRC_INVALID_IP_ADDRESS;  /* TODO: add error string */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
        else if (owa.l7rc == L7_FAILURE)
        {
          /* Invalid Host Name*/
          owa.rc = XLIBRC_INVALID_HOST_NAME;  /* TODO: add error string */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }

  /* retrieve key: LogHostTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
                          (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
  if (kwa.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);
    if( (usmDbLogHostTableIpAddressGet ( L7_UNIT_CURRENT , keyLogHostTableIndexValue, tempIpAddrVal) == L7_SUCCESS) &&
        (strcmp ( tempIpAddrVal,objLogHostTableIpAddressValue) == 0))
   
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
	owa.l7rc = usmDbLogHostAdd(L7_UNIT_CURRENT, objLogHostTableIpAddressValue, objLogHostTableIpAddressTypeValue, &keyLogHostTableIndexValue);
	if(owa.l7rc == L7_TABLE_IS_FULL)
	{
	    owa.rc = XLIBRC_TABLE_IS_FULL;  /* TODO: add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
	}
	else if(owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;  /* TODO: add error string */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
	}
	
   /* push key into filter */
	xLibFilterSet(wap,XOBJ_baselogCfg_LogHostTableIndex,0,
                      (xLibU8_t *) &keyLogHostTableIndexValue,
                      sizeof(keyLogHostTableIndexValue));
  
  }
  else if( objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
      /* retrieve key: LogHostTableIndex */
		kwa.rc = xLibFilterGet (wap, XOBJ_baselogCfg_LogHostTableIndex,
		                      (xLibU8_t *) & keyLogHostTableIndexValue, &kwa.len);
		if (kwa.rc != XLIBRC_SUCCESS)
		{
      kwa.rc = XLIBRC_LOG_HOST_INDEX_FAILURE;
		  FPOBJ_TRACE_EXIT (bufp, kwa);
		  return kwa.rc;
		}
		FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogHostTableIndexValue, kwa.len);

		owa.l7rc = usmDbLogHostTableRowStatusSet(L7_UNIT_CURRENT, keyLogHostTableIndexValue, L7_ROW_STATUS_DESTROY);
		if(owa.l7rc != L7_SUCCESS)
		{
		  owa.rc = XLIBRC_SUCCESS;  /* TODO: add error string */
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;
		}
	}

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogSyslogFacility
*
* @purpose Get 'LogSyslogFacility'
*
* @description Facility for Syslog 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogFacility (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogFacilityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogFacilityValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogSyslogDefaultFacilityGet (L7_UNIT_CURRENT,
                                      &objLogSyslogFacilityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogFacilityValue,
                     sizeof (objLogSyslogFacilityValue));

  /* return the object value: LogConsoleSeverityFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogSyslogFacilityValue,
                    sizeof (objLogSyslogFacilityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogSyslogFacility
*
* @purpose Set 'LogSyslogFacility'
*
* @description Facility for Syslog 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogSyslogFacility (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogFacilityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogFacilityValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogConsoleSeverityFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogSyslogFacilityValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogFacilityValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogSyslogDefaultFacilitySet (L7_UNIT_CURRENT,
                                      objLogSyslogFacilityValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogConsoleSeverityFilter
*
* @purpose Get 'LogConsoleSeverityFilter'
*
* @description Severity filter for console logs 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogConsoleSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleSeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogConsoleSeverityFilterGet (L7_UNIT_CURRENT,
                                      &objLogConsoleSeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleSeverityFilterValue,
                     sizeof (objLogConsoleSeverityFilterValue));

  /* return the object value: LogConsoleSeverityFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogConsoleSeverityFilterValue,
                    sizeof (objLogConsoleSeverityFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogConsoleSeverityFilter
*
* @purpose Set 'LogConsoleSeverityFilter'
*
* @description Severity filter for console logs 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogConsoleSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleSeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogConsoleSeverityFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogConsoleSeverityFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleSeverityFilterValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogConsoleSeverityFilterSet (L7_UNIT_CURRENT,
                                      objLogConsoleSeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogPersistentSeverityFilter
*
* @purpose Get 'LogPersistentSeverityFilter'
*
* @description This control specifies the minimum severity to log to the startup 
*              and operation log. Messages with an equal or lower numerical 
*              severity are logged. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogPersistentSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogPersistentSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogPersistentSeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogPersistentSeverityFilterGet (L7_UNIT_CURRENT,
                                         &objLogPersistentSeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogPersistentSeverityFilterValue,
                     sizeof (objLogPersistentSeverityFilterValue));

  /* return the object value: LogPersistentSeverityFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogPersistentSeverityFilterValue,
                    sizeof (objLogPersistentSeverityFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogPersistentSeverityFilter
*
* @purpose Set 'LogPersistentSeverityFilter'
*
* @description This control specifies the minimum severity to log to the startup 
*              and operation log. Messages with an equal or lower numerical 
*              severity are logged. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogPersistentSeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogPersistentSeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogPersistentSeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogPersistentSeverityFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogPersistentSeverityFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogPersistentSeverityFilterValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogPersistentSeverityFilterSet (L7_UNIT_CURRENT,
                                         objLogPersistentSeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_LogConsoleComponentFilter
*
* @purpose Get 'LogConsoleComponentFilter'
*
* @description [LogConsoleComponentFilter]: component filter of logging console
*              (if different.) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogConsoleComponentFilter (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleComponentFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleComponentFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  /*owa.l7rc =
    usmDbGetUnknown (L7_UNIT_CURRENT, &objLogConsoleComponentFilterValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleComponentFilterValue,
                     sizeof (objLogConsoleComponentFilterValue));

  /* return the object value: LogConsoleComponentFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogConsoleComponentFilterValue,
                    sizeof (objLogConsoleComponentFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogConsoleComponentFilter
*
* @purpose Set 'LogConsoleComponentFilter'
*
* @description [LogConsoleComponentFilter]: component filter of logging console
*              (if different.) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogConsoleComponentFilter (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleComponentFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleComponentFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogConsoleComponentFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogConsoleComponentFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleComponentFilterValue, owa.len);

  /* set the value in application */
  /*owa.l7rc =
    usmDbSetUnknown (L7_UNIT_CURRENT, objLogConsoleComponentFilterValue);*/
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
* @function fpObjGet_baselogCfg_LogPersistentAdminStatus
*
* @purpose Get 'LogPersistentAdminStatus'
*
* @description This control disables/enables logging for the persistent startup 
*              log. Setting this value to disable does not clear the current 
*              contents of the log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogPersistentAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogPersistentAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogPersistentAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogPersistentAdminStatusGet (L7_UNIT_CURRENT,
                                      &objLogPersistentAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogPersistentAdminStatusValue,
                     sizeof (objLogPersistentAdminStatusValue));

  /* return the object value: LogPersistentAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogPersistentAdminStatusValue,
                    sizeof (objLogPersistentAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogPersistentAdminStatus
*
* @purpose Set 'LogPersistentAdminStatus'
*
* @description This control disables/enables logging for the persistent startup 
*              log. Setting this value to disable does not clear the current 
*              contents of the log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogPersistentAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogPersistentAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogPersistentAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogPersistentAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogPersistentAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogPersistentAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogPersistentAdminStatusSet (L7_UNIT_CURRENT,
                                      objLogPersistentAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogPersistentLogCount
*
* @purpose Get 'LogPersistentLogCount'
*
* @description The count of valid entries in the persistent log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogPersistentLogCount (void *wap, void *bufp)
{
  xLibU32_t objLogPersistentLogCountValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogPersistentLogCountValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogPersistentLogCountGet (L7_UNIT_CURRENT,
                                   &objLogPersistentLogCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogPersistentLogCountValue,
                     sizeof (objLogPersistentLogCountValue));

  /* return the object value: LogPersistentLogCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogPersistentLogCountValue,
                           sizeof (objLogPersistentLogCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogConsoleAdminStatus
*
* @purpose Get 'LogConsoleAdminStatus'
*
* @description Admin mode for console logs 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogConsoleAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogConsoleAdminStatusGet (L7_UNIT_CURRENT,
                                   &objLogConsoleAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleAdminStatusValue,
                     sizeof (objLogConsoleAdminStatusValue));

  /* return the object value: LogConsoleAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogConsoleAdminStatusValue,
                           sizeof (objLogConsoleAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogConsoleAdminStatus
*
* @purpose Set 'LogConsoleAdminStatus'
*
* @description Admin mode for console logs 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogConsoleAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogConsoleAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogConsoleAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogConsoleAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogConsoleAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogConsoleAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogConsoleAdminStatusSet (L7_UNIT_CURRENT,
                                   objLogConsoleAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogMessageReceivedTime
*
* @purpose Get 'LogMessageReceivedTime'
*
* @description The local time when a message was last received by the log subsystem 
*              specified as the number of non-leap seconds since 00:00:00 
*              UTC on January 1 1970. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogMessageReceivedTime (void *wap, void *bufp)
{
  xLibStr256_t objLogMessageReceivedTimeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogMessageReceivedTimeValue));
  xLibU32_t tempTimeValue;
 
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogMessageReceivedTimeValue,0x0,sizeof(objLogMessageReceivedTimeValue)); 

  /* get the value from application */
  owa.l7rc =
    usmDbLogMessageReceivedTimeGet (L7_UNIT_CURRENT,
                                    &tempTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogMessageReceivedTimeValue,
                     strlen (objLogMessageReceivedTimeValue));

  /* return the object value: LogMessageReceivedTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLogMessageReceivedTimeValue,
                           strlen (objLogMessageReceivedTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogMessagesReceived
*
* @purpose Get 'LogMessagesReceived'
*
* @description The number of messages received by the log process. This includes 
*              messages that are dropped or ignored. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogMessagesReceived (void *wap, void *bufp)
{
  xLibU32_t objLogMessagesReceivedValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogMessagesReceivedValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogMessagesReceivedGet (L7_UNIT_CURRENT, &objLogMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogMessagesReceivedValue,
                     sizeof (objLogMessagesReceivedValue));

  /* return the object value: LogMessagesReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogMessagesReceivedValue,
                           sizeof (objLogMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogSyslogAdminStatus
*
* @purpose Get 'LogSyslogAdminStatus'
*
* @description For Enabling and Disabling logging to configured syslog hosts. 
*              Setting this to disable stops logging to all syslog hosts. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogSyslogAdminStatusGet (L7_UNIT_CURRENT,
                                  &objLogSyslogAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogAdminStatusValue,
                     sizeof (objLogSyslogAdminStatusValue));

  /* return the object value: LogSyslogAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogSyslogAdminStatusValue,
                           sizeof (objLogSyslogAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogSyslogAdminStatus
*
* @purpose Set 'LogSyslogAdminStatus'
*
* @description For Enabling and Disabling logging to configured syslog hosts. 
*              Setting this to disable stops logging to all syslog hosts. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogSyslogAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogSyslogAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogSyslogAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogSyslogAdminStatusSet (L7_UNIT_CURRENT,
                                  objLogSyslogAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogSyslogMaxHosts
*
* @purpose Get 'LogSyslogMaxHosts'
*
* @description Maximum number of hosts that can be configured for logging syslog 
*              messages. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogMaxHosts (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogMaxHostsValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogMaxHostsValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogMaxSyslogHostsGet (L7_UNIT_CURRENT, &objLogSyslogMaxHostsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogMaxHostsValue,
                     sizeof (objLogSyslogMaxHostsValue));

  /* return the object value: LogSyslogMaxHosts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogSyslogMaxHostsValue,
                           sizeof (objLogSyslogMaxHostsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogSyslogLocalPort
*
* @purpose Get 'LogSyslogLocalPort'
*
* @description This is the port on the local host from which syslog messages 
*              are sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogLocalPort (void *wap, void *bufp)
{
  xLibU16_t objLogSyslogLocalPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogLocalPortValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogLocalPortGet (L7_UNIT_CURRENT, &objLogSyslogLocalPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogLocalPortValue,
                     sizeof (objLogSyslogLocalPortValue));

  /* return the object value: LogSyslogLocalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogSyslogLocalPortValue,
                           sizeof (objLogSyslogLocalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogSyslogLocalPort
*
* @purpose Set 'LogSyslogLocalPort'
*
* @description This is the port on the local host from which syslog messages 
*              are sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogSyslogLocalPort (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogLocalPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogLocalPortValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogSyslogLocalPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogSyslogLocalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogLocalPortValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogLocalPortSet (L7_UNIT_CURRENT, (xLibU16_t)objLogSyslogLocalPortValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogCliCommandsAdminStatus
*
* @purpose Get 'LogCliCommandsAdminStatus'
*
* @description Administratively enable/disable the logging of the CLI Commands 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogCliCommandsAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogCliCommandsAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogCliCommandsAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbCmdLoggerAdminModeGet (
                                &objLogCliCommandsAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogCliCommandsAdminStatusValue,
                     sizeof (objLogCliCommandsAdminStatusValue));

  /* return the object value: LogCliCommandsAdminStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogCliCommandsAdminStatusValue,
                    sizeof (objLogCliCommandsAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogCliCommandsAdminStatus
*
* @purpose Set 'LogCliCommandsAdminStatus'
*
* @description Administratively enable/disable the logging of the CLI Commands 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogCliCommandsAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogCliCommandsAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogCliCommandsAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogCliCommandsAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogCliCommandsAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogCliCommandsAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbCmdLoggerAdminModeSet (objLogCliCommandsAdminStatusValue);
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
* @function fpObjGet_baselogCfg_LogMessagesDropped
*
* @purpose Get 'LogMessagesDropped'
*
* @description The number of messages that could not be processed due to error 
*              or lack of resources. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogMessagesDropped (void *wap, void *bufp)
{
  xLibU32_t objLogMessagesDroppedValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogMessagesDroppedValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogMessagesDroppedGet (L7_UNIT_CURRENT, &objLogMessagesDroppedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogMessagesDroppedValue,
                     sizeof (objLogMessagesDroppedValue));

  /* return the object value: LogMessagesDropped */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogMessagesDroppedValue,
                           sizeof (objLogMessagesDroppedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryLogCount
*
* @purpose Get 'LogInMemoryLogCount'
*
* @description The count of valid entries in the in-memory log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryLogCount (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryLogCountValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryLogCountValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogInMemoryLogCountGet (L7_UNIT_CURRENT, &objLogInMemoryLogCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryLogCountValue,
                     sizeof (objLogInMemoryLogCountValue));

  /* return the object value: LogInMemoryLogCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemoryLogCountValue,
                           sizeof (objLogInMemoryLogCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryAdminStatus
*
* @purpose Get 'LogInMemoryAdminStatus'
*
* @description Administratively enable/disable the In Memory log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogInMemoryAdminStatusGet (L7_UNIT_CURRENT,
                                    &objLogInMemoryAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryAdminStatusValue,
                     sizeof (objLogInMemoryAdminStatusValue));

  /* return the object value: LogInMemoryAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemoryAdminStatusValue,
                           sizeof (objLogInMemoryAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemoryAdminStatus
*
* @purpose Set 'LogInMemoryAdminStatus'
*
* @description Administratively enable/disable the In Memory log. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemoryAdminStatus (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryAdminStatusValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryAdminStatusValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemoryAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogInMemoryAdminStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogInMemoryAdminStatusSet (L7_UNIT_CURRENT,
                                    objLogInMemoryAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemorySeverityFilter
*
* @purpose Get 'LogInMemorySeverityFilter'
*
* @description severity filter of in memory log 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemorySeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogInMemorySeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemorySeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogInMemorySeverityFilterGet (L7_UNIT_CURRENT,
                                       &objLogInMemorySeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemorySeverityFilterValue,
                     sizeof (objLogInMemorySeverityFilterValue));

  /* return the object value: LogInMemorySeverityFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemorySeverityFilterValue,
                    sizeof (objLogInMemorySeverityFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemorySeverityFilter
*
* @purpose Set 'LogInMemorySeverityFilter'
*
* @description severity filter of in memory log 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemorySeverityFilter (void *wap, void *bufp)
{
  xLibU32_t objLogInMemorySeverityFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemorySeverityFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemorySeverityFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogInMemorySeverityFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemorySeverityFilterValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogInMemorySeverityFilterSet (L7_UNIT_CURRENT,
                                       objLogInMemorySeverityFilterValue);
  if (owa.l7rc != L7_SUCCESS &&  owa.l7rc != L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryComponentFilter
*
* @purpose Get 'LogInMemoryComponentFilter'
*
* @description [LogInMemoryComponentFilter]: component filter of in memory
*              log (if different.) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryComponentFilter (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryComponentFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryComponentFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  /*owa.l7rc =
    usmDbGetUnknown (L7_UNIT_CURRENT, &objLogInMemoryComponentFilterValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryComponentFilterValue,
                     sizeof (objLogInMemoryComponentFilterValue));

  /* return the object value: LogInMemoryComponentFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemoryComponentFilterValue,
                    sizeof (objLogInMemoryComponentFilterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemoryComponentFilter
*
* @purpose Set 'LogInMemoryComponentFilter'
*
* @description [LogInMemoryComponentFilter]: component filter of in memory
*              log (if different.) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemoryComponentFilter (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryComponentFilterValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryComponentFilterValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemoryComponentFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objLogInMemoryComponentFilterValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryComponentFilterValue, owa.len);

  /* set the value in application */
 /* owa.l7rc =
    usmDbSetUnknown (L7_UNIT_CURRENT, objLogInMemoryComponentFilterValue);*/
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
* @function fpObjGet_baselogCfg_LogSyslogMessagesRelayed
*
* @purpose Get 'LogSyslogMessagesRelayed'
*
* @description The number of messages forwarded by the syslog function to a syslog 
*              host. Messages forwarded to multiple hosts are counted 
*              once for each host. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogMessagesRelayed (void *wap, void *bufp)
{
  xLibU32_t objLogSyslogMessagesRelayedValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogMessagesRelayedValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLogSyslogMessagesRelayedGet (L7_UNIT_CURRENT,
                                      &objLogSyslogMessagesRelayedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogSyslogMessagesRelayedValue,
                     sizeof (objLogSyslogMessagesRelayedValue));

  /* return the object value: LogSyslogMessagesRelayed */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogSyslogMessagesRelayedValue,
                    sizeof (objLogSyslogMessagesRelayedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baselogCfg_LogSyslogMessageDeliveredTime
*
* @purpose Get 'LogSyslogMessageDeliveredTime'
*
* @description The local time when a message was last delivered to a syslog host 
*              specified as the number of non-leap seconds since 00:00:00 
*              UTC on January 1 1970. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogSyslogMessageDeliveredTime (void *wap,
                                                            void *bufp)
{
  xLibStr256_t objLogSyslogMessageDeliveredTimeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogSyslogMessageDeliveredTimeValue));
  xLibU32_t tempTimeValue;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogSyslogMessageDeliveredTimeValue,0x0,sizeof(objLogSyslogMessageDeliveredTimeValue));
 
  /* get the value from application */
  owa.l7rc =
    usmDbLogSyslogMessageDeliveredTimeGet (L7_UNIT_CURRENT,
                                           &tempTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogSyslogMessageDeliveredTimeValue,
                     strlen (objLogSyslogMessageDeliveredTimeValue));

  /* return the object value: LogSyslogMessageDeliveredTime */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objLogSyslogMessageDeliveredTimeValue,
                    strlen (objLogSyslogMessageDeliveredTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogCLICmdLoggerEntryAdd
*
* @purpose Set 'LogCLICmdLoggerEntryAdd'
*
* @description Validate the current user configuration and Log CLI commands 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogCLICmdLoggerEntryAdd (void *wap, void *bufp)
{
  xLibStr256_t objLogCLICmdLoggerEntryAddValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogCLICmdLoggerEntryAddValue));
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objLogCLICmdLoggerEntryAddValue,0x0,sizeof(objLogCLICmdLoggerEntryAddValue));

  /* retrieve object: LogCLICmdLoggerEntryAdd */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objLogCLICmdLoggerEntryAddValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogCLICmdLoggerEntryAddValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbCmdLoggerEntryAdd (objLogCLICmdLoggerEntryAddValue);
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
* @function fpObjSet_baselogCfg_LogInPersistentLogClear
*
* @purpose Set 'LogInPersistentLogClear'
 *@description  [LogInPersistentLogClear] <HTML>clear the persistent operational
* log   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInPersistentLogClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogInPersistentLogClearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInPersistentLogClear */
  owa.len = sizeof (objLogInPersistentLogClearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLogInPersistentLogClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInPersistentLogClearValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbLogPersistentLogClear(L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogCfg_LogInMemoryBufferSize
*
* @purpose Get 'LogInMemoryBufferSize'
*
* @description [LogInMemoryBufferSize]: 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogCfg_LogInMemoryBufferSize (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryBufferSizeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryBufferSizeValue));
  FPOBJ_TRACE_ENTER (bufp);

owa.l7rc =
    usmDbLogInMemoryBufferSizeGet ( &objLogInMemoryBufferSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryBufferSizeValue,owa.len);

  /* return the object value: LogInMemoryComponentFilter */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objLogInMemoryBufferSizeValue,
                    sizeof (objLogInMemoryBufferSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogCfg_LogInMemoryBufferSize
*
* @purpose Set 'LogInMemoryBufferSize'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogCfg_LogInMemoryBufferSize (void *wap, void *bufp)
{
  xLibU32_t objLogInMemoryBufferSizeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objLogInMemoryBufferSizeValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInMemoryComponentFilter */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objLogInMemoryBufferSizeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInMemoryBufferSizeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbLogInMemoryBufferSizeSet ( objLogInMemoryBufferSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
