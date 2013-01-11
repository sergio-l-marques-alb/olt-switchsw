/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baselogemailAlertSubjectTable.c
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
#include "_xe_baselogemailAlertSubjectTable_obj.h"
#include "usmdb_log_api.h"

/*******************************************************************************
* @function fpObjGet_baselogemailAlertSubjectTable_subjectTableMessageType
*
* @purpose Get 'subjectTableMessageType'
 *@description  [subjectTableMessageType] Log email Alert Subject
* Message Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertSubjectTable_subjectTableMessageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertSubjectTableMessageTypeValue;
  xLibU32_t nextObjLogemailAlertSubjectTableMessageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogemailAlertSubjectTableMessageType */
  owa.len = sizeof (objLogemailAlertSubjectTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertSubjectTable_subjectTableMessageType,
                          (xLibU8_t *) & objLogemailAlertSubjectTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLogemailAlertSubjectTableMessageTypeValue = L7_LOG_EMAIL_ALERT_URGENT; 
    owa.l7rc = L7_SUCCESS ;
  }
  else
  {  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertSubjectTableMessageTypeValue, owa.len);
    if(objLogemailAlertSubjectTableMessageTypeValue == L7_LOG_EMAIL_ALERT_NON_URGENT)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjLogemailAlertSubjectTableMessageTypeValue = objLogemailAlertSubjectTableMessageTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }    
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogemailAlertSubjectTableMessageTypeValue, owa.len);

  /* return the object value: LogemailAlertSubjectTableMessageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogemailAlertSubjectTableMessageTypeValue,
                           sizeof (nextObjLogemailAlertSubjectTableMessageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertSubjectTable_subjectTableSubject
*
* @purpose Get 'subjectTableSubject'
 *@description  [subjectTableSubject] Email Alert Subject.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertSubjectTable_subjectTableSubject (void *wap, void *bufp)
{  
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objLogemailAlertSubjectTableSubjectValue;
  xLibU32_t keyLogemailAlertSubjectTableMessageTypeValue;
  xLibStr256_t tmpSubjectValue;

  FPOBJ_TRACE_ENTER (bufp);
  FPOBJ_CLR_STR256(objLogemailAlertSubjectTableSubjectValue);
  FPOBJ_CLR_STR256(tmpSubjectValue);

  /* retrieve key: LogemailAlertSubjectTableMessageType */
  owa.len = sizeof(keyLogemailAlertSubjectTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertSubjectTable_subjectTableMessageType,
                          (xLibU8_t *) & keyLogemailAlertSubjectTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogemailAlertSubjectTableMessageTypeValue, owa.len);

  if(keyLogemailAlertSubjectTableMessageTypeValue == L7_LOGGING_EMAIL_ALERT_BOTH)
  {
    usmDbLogEmailAlertSubjectGet(L7_UNIT_CURRENT, L7_LOG_EMAIL_ALERT_URGENT,
                                 objLogemailAlertSubjectTableSubjectValue);
    FPOBJ_CPY_STR256(tmpSubjectValue, objLogemailAlertSubjectTableSubjectValue);
    FPOBJ_CLR_STR256(objLogemailAlertSubjectTableSubjectValue);
    usmDbLogEmailAlertSubjectGet(L7_UNIT_CURRENT, L7_LOG_EMAIL_ALERT_NON_URGENT,
                                 objLogemailAlertSubjectTableSubjectValue);
    if (!FPOBJ_CMP_STR256(tmpSubjectValue, objLogemailAlertSubjectTableSubjectValue))
    {
      /* Do Nothing */
    }
    else
    {
      FPOBJ_CLR_STR256(objLogemailAlertSubjectTableSubjectValue);
    }
  }
  else
  {
    if(keyLogemailAlertSubjectTableMessageTypeValue != L7_LOG_EMAIL_ALERT_URGENT)
    {
      keyLogemailAlertSubjectTableMessageTypeValue = L7_LOG_EMAIL_ALERT_NON_URGENT;
    }
  
    /* get the value from application */
    owa.l7rc = usmDbLogEmailAlertSubjectGet(L7_UNIT_CURRENT, keyLogemailAlertSubjectTableMessageTypeValue,
                                            objLogemailAlertSubjectTableSubjectValue);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;  
    }
  }

  FPOBJ_TRACE_VALUE (bufp, objLogemailAlertSubjectTableSubjectValue, strlen(objLogemailAlertSubjectTableSubjectValue));

  /* return the object value: LogemailAlertSubjectTableSubject */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogemailAlertSubjectTableSubjectValue,
                           sizeof (objLogemailAlertSubjectTableSubjectValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertSubjectTable_subjectTableSubject
*
* @purpose Set 'subjectTableSubject'
 *@description  [subjectTableSubject] Email Alert Subject.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertSubjectTable_subjectTableSubject (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objLogemailAlertSubjectTableSubjectValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: LogemailAlertSubjectTableSubject */
  owa.len = sizeof(objLogemailAlertSubjectTableSubjectValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)objLogemailAlertSubjectTableSubjectValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLogemailAlertSubjectTableSubjectValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objLogemailAlertSubjectTableSubjectValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
#endif  
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertSubjectTable_subjectTableStatus
*
* @purpose Get 'subjectTableStatus'
 *@description  [subjectTableStatus] Log email Alert To Address
* Table Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertSubjectTable_subjectTableStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertSubjectTableStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  objLogemailAlertSubjectTableStatusValue = L7_ROW_STATUS_ACTIVE;

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertSubjectTableStatusValue,
                     sizeof (objLogemailAlertSubjectTableStatusValue));

  /* return the object value: LogemailAlertSubjectTableStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogemailAlertSubjectTableStatusValue,
                           sizeof (objLogemailAlertSubjectTableStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertSubjectTable_subjectTableStatus
*
* @purpose Set 'subjectTableStatus'
 *@description  [subjectTableStatus] Log email Alert To Address
* Table Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertSubjectTable_subjectTableStatus (void *wap, void *bufp)
{  
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertSubjectTableStatusValue;
  xLibU32_t objLogemailAlertSubjectTableMessageTypeValue;  
  xLibStr256_t objLogemailAlertSubjectTableSubjectValue;
  xLibU32_t tmpMsgType;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogemailAlertSubjectTableStatus */
  owa.len = sizeof (objLogemailAlertSubjectTableStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLogemailAlertSubjectTableStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertSubjectTableStatusValue, owa.len);

  /* retrieve key: LogemailAlertSubjectTableMessageType */
  owa.len = sizeof(objLogemailAlertSubjectTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertSubjectTable_subjectTableMessageType,
                        (xLibU8_t *) & objLogemailAlertSubjectTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertSubjectTableMessageTypeValue, owa.len);

  if (objLogemailAlertSubjectTableStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* retrieve object: LogemailAlertSubjectTableSubject */
    owa.len = sizeof(objLogemailAlertSubjectTableSubjectValue);
    owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertSubjectTable_subjectTableSubject,
                          (xLibU8_t *) &objLogemailAlertSubjectTableSubjectValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* set the value in application */
    if (objLogemailAlertSubjectTableMessageTypeValue == L7_LOGGING_EMAIL_ALERT_BOTH)
    {
      for(tmpMsgType = L7_LOG_EMAIL_ALERT_URGENT; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
      {
        owa.l7rc = usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT, tmpMsgType,
                                         objLogemailAlertSubjectTableSubjectValue);
      }
    }
    else
    {
      owa.l7rc = usmDbLogEmailAlertSubjectSet(L7_UNIT_CURRENT,objLogemailAlertSubjectTableMessageTypeValue,
                                     objLogemailAlertSubjectTableSubjectValue);
    }
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ROW_STATUS_ADD;    /* TODO: Change if required */
    }
  }
  else if (objLogemailAlertSubjectTableStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the value in application */
    if(objLogemailAlertSubjectTableMessageTypeValue == L7_LOG_EMAIL_ALERT_URGENT) 
    {
      owa.l7rc = usmDbLogEmailAlertSubjectSet(L7_UNIT_CURRENT,objLogemailAlertSubjectTableMessageTypeValue,
                                              "Urgent Log Messages");      
    }
    else if(objLogemailAlertSubjectTableMessageTypeValue == L7_LOG_EMAIL_ALERT_NON_URGENT) 
    {
       owa.l7rc = usmDbLogEmailAlertSubjectSet(L7_UNIT_CURRENT,objLogemailAlertSubjectTableMessageTypeValue,
                                               "Non Urgent Log Messages");
    }
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ROW_STATUS_DELETE;   
    }
  }
  else
  {
     owa.rc = XLIBRC_SUCCESS;    
  } /* end of if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO | L7_ROW_STATUS_DESTROY ) */
  
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

