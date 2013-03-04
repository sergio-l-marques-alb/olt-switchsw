
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baselogMemoryTable.c
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
#include "_xe_baselogMemoryTable_obj.h"

#include "usmdb_log_api.h"

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_memoryIndex
*
* @purpose Get 'memoryIndex'
* @description  [memoryIndex] <HTML>The index to message entry in the in-memory
* log table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_memoryIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmemoryIndexValue;
  xLibU32_t nextObjmemoryIndexValue;
  xLibS8_t tempBuffer[L7_LOG_MESSAGE_LENGTH];

  memset(tempBuffer,0x0,sizeof(tempBuffer));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: memoryIndex */
  owa.len = sizeof (objmemoryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_memoryIndex,
                          (xLibU8_t *) & objmemoryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objmemoryIndexValue = 0;
    nextObjmemoryIndexValue = 0; 
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmemoryIndexValue, owa.len);
  owa.l7rc = usmDbLogMsgInMemoryGetNext (L7_UNIT_CURRENT, objmemoryIndexValue,
                                  tempBuffer,&nextObjmemoryIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmemoryIndexValue, owa.len);

  /* return the object value: memoryIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmemoryIndexValue,
                           sizeof (nextObjmemoryIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_msgText
*
* @purpose Get 'msgText'
* @description  [msgText] <HTML>TO DO   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_msgText (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];

  xLibU32_t keymemoryIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  /* retrieve key: memoryIndex */
  owa.len = sizeof (keymemoryIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_memoryIndex,
                          (xLibU8_t *) & keymemoryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymemoryIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogMsgInMemoryGet (L7_UNIT_CURRENT, keymemoryIndexValue,
                             objmsgTextValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmsgTextValue, strlen (objmsgTextValue));

  /* return the object value: msgText */
  (void) stripQuotes(objmsgTextValue);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmsgTextValue, strlen (objmsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_msgSeverity
*
* @purpose Get 'msgSeverity'
 *@description  [msgSeverity] <HTML>Severity of the log message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_msgSeverity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmsgSeverityValue;
 
  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);


  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  
  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_msgText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = extractSeverity (objmsgTextValue, &objmsgSeverityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmsgSeverityValue, sizeof (objmsgSeverityValue));

  /* return the object value: msgSeverity */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmsgSeverityValue, sizeof (objmsgSeverityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_msgTimeStamp
*
* @purpose Get 'msgTimeStamp'
 *@description  [msgTimeStamp] <HTML>Time Stamp of the given Message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_msgTimeStamp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objmsgTimeStampValue;

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objmsgTimeStampValue,0x00,sizeof(objmsgTimeStampValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_msgText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc = extractTime (objmsgTextValue, objmsgTimeStampValue,sizeof(objmsgTimeStampValue));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmsgTimeStampValue, strlen (objmsgTimeStampValue));

  /* return the object value: msgTimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmsgTimeStampValue, strlen (objmsgTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_msgComponent
*
* @purpose Get 'msgComponent'
 *@description  [msgComponent] <HTML>Component of the Given Log Message   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_msgComponent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objmsgComponentValue;

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objmsgComponentValue,0x00,sizeof(objmsgComponentValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_msgText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = extractComponent (objmsgTextValue, (xLibU8_t *)objmsgComponentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmsgComponentValue, strlen (objmsgComponentValue));

  /* return the object value: msgComponent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmsgComponentValue, strlen (objmsgComponentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogMemoryTable_msgDescription
*
* @purpose Get 'msgDescription'
 *@description  [msgDescription] <HTML>Message Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogMemoryTable_msgDescription (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objmsgDescriptionValue[L7_LOG_MESSAGE_LENGTH];

  xLibU8_t objmsgTextValue[L7_LOG_MESSAGE_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmsgTextValue,0x00,sizeof(objmsgTextValue));
  memset(objmsgDescriptionValue,0x00,sizeof(objmsgDescriptionValue)); 

  /* retrieve dependent object: msgText */
  owa.len = sizeof (objmsgTextValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogMemoryTable_msgText,
                          (xLibU8_t *) objmsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = extractLogDesc (objmsgTextValue, objmsgDescriptionValue,sizeof(objmsgDescriptionValue));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmsgDescriptionValue, strlen (objmsgDescriptionValue));

  (void)stripQuotes(objmsgDescriptionValue);
  /* return the object value: msgDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmsgDescriptionValue,
                           strlen (objmsgDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
