
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baselogPersistTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  06 November 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baselogPersistTable_obj.h"
#include "usmdb_log_api.h"

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistIndex
*
* @purpose Get 'persistIndex'
 *@description  [persistIndex] <HTML>The index to message entry in the in-memory
* log table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistIndex (void *wap, void *bufp)
{
  xLibU32_t objpersistIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objpersistIndexValue));
  xLibU32_t nextObjpersistIndexValue;
  xLibU32_t unit;

  xLibS8_t tempBuffer[L7_LOG_MESSAGE_LENGTH];
  memset(tempBuffer,0x00,sizeof(tempBuffer));
  FPOBJ_TRACE_ENTER (bufp);

  usmDbUnitMgrNumberGet(&unit);

  /* retrieve key: persistIndex */
  owa.len = sizeof (objpersistIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistIndex,
                          (xLibU8_t *) & objpersistIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpersistIndexValue = 0;
    objpersistIndexValue  = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpersistIndexValue, owa.len);
  owa.l7rc = usmDbLogMsgPersistentGetNext (unit,
                                    objpersistIndexValue,tempBuffer,
                                    &nextObjpersistIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpersistIndexValue, owa.len);

  /* return the object value: persistIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpersistIndexValue,
                           sizeof (nextObjpersistIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistText
*
* @purpose Get 'persistText'
 *@description  [persistText] <HTML>TO DO   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistText (void *wap, void *bufp)
{

  xLibU32_t keypersistIndexValue;
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (keypersistIndexValue));
  xLibS8_t objpersistTextValue[L7_LOG_MESSAGE_LENGTH];
  xLibU32_t unit;
  
  FPOBJ_TRACE_ENTER (bufp);

  usmDbUnitMgrNumberGet(&unit);
  
  /* retrieve key: persistIndex */
  owa.len = sizeof (keypersistIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistIndex,
                          (xLibU8_t *) & keypersistIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(objpersistTextValue,0x00,sizeof(objpersistTextValue));
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypersistIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogMsgPersistentGet (unit, keypersistIndexValue, objpersistTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpersistTextValue, strlen (objpersistTextValue));

  (void) stripQuotes(objpersistTextValue);
  /* return the object value: persistText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpersistTextValue, strlen (objpersistTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistSeverity
*
* @purpose Get 'persistSeverity'
 *@description  [persistSeverity] <HTML>Severity of the log message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpersistSeverityValue;

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);


  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  
  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = extractSeverity (objmsgTextValue, &objpersistSeverityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpersistSeverityValue, sizeof (objpersistSeverityValue));

  /* return the object value: persistSeverity */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpersistSeverityValue,
                           sizeof (objpersistSeverityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistTimeStamp
*
* @purpose Get 'persistTimeStamp'
 *@description  [persistTimeStamp] <HTML>Time Stamp of the given Message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistTimeStamp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objpersistTimeStampValue;

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objpersistTimeStampValue,0x00,sizeof(objpersistTimeStampValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = extractTime (objmsgTextValue, objpersistTimeStampValue,sizeof(objpersistTimeStampValue));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpersistTimeStampValue, strlen (objpersistTimeStampValue));

  /* return the object value: persistTimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpersistTimeStampValue,
                           strlen (objpersistTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistComponent
*
* @purpose Get 'persistComponent'
 *@description  [persistComponent] <HTML>Component of the Given Log Message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistComponent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objpersistComponentValue;

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objpersistComponentValue,0x00,sizeof(objpersistComponentValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = extractComponent (objmsgTextValue, (xLibU8_t *)objpersistComponentValue);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpersistComponentValue, strlen (objpersistComponentValue));

  /* return the object value: persistComponent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpersistComponentValue,
                           strlen (objpersistComponentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogPersistTable_persistDescription
*
* @purpose Get 'persistDescription'
 *@description  [persistDescription] <HTML>Message Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogPersistTable_persistDescription (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objpersistDescriptionValue[L7_LOG_MESSAGE_LENGTH];

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objpersistDescriptionValue,0x00,sizeof(objpersistDescriptionValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogPersistTable_persistText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = extractLogDesc (objmsgTextValue, objpersistDescriptionValue,sizeof(objpersistDescriptionValue));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objpersistDescriptionValue, strlen (objpersistDescriptionValue));

  (void) stripQuotes(objpersistDescriptionValue);
  /* return the object value: persistDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpersistDescriptionValue,
                           strlen (objpersistDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
