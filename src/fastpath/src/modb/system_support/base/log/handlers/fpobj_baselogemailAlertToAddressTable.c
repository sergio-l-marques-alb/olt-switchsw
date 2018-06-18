
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baselogemailAlertToAddressTable.c
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
#include "_xe_baselogemailAlertToAddressTable_obj.h"
#include "usmdb_log_api.h"
#include "_xe_baselogemailAlertSubjectTable_obj.h"

/*******************************************************************************
* @function fpObjGet_baselogemailAlertToAddressTable_toAddressTableMessageType
*
* @purpose Get 'toAddressTableMessageType'
 *@description  [toAddressTableMessageType] Log email Alert Subject
* Message Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertToAddressTable_toAddressTableMessageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertToAddressTableMessageTypeValue;
  xLibU32_t nextObjLogemailAlertToAddressTableMessageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LogemailAlerttoAddressTableMessageType */
  owa.len = sizeof (objLogemailAlertToAddressTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableMessageType,
                          (xLibU8_t *) & objLogemailAlertToAddressTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLogemailAlertToAddressTableMessageTypeValue = L7_LOG_EMAIL_ALERT_URGENT; 
    owa.l7rc = L7_SUCCESS ;
  }
  else
  {  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertToAddressTableMessageTypeValue, owa.len);
    if(objLogemailAlertToAddressTableMessageTypeValue == L7_LOG_EMAIL_ALERT_NON_URGENT)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjLogemailAlertToAddressTableMessageTypeValue = objLogemailAlertToAddressTableMessageTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }    
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogemailAlertToAddressTableMessageTypeValue, owa.len);

  /* return the object value: LogemailAlertSubjectTableMessageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogemailAlertToAddressTableMessageTypeValue,
                           sizeof (nextObjLogemailAlertToAddressTableMessageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertToAddressTable_toAddressTableToAddress
*
* @purpose Get 'toAddressTableToAddress'
 *@description  [toAddressTableToAddress] Email Alert To Address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertToAddressTable_toAddressTableToAddress (void *wap, void *bufp)
{    
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objLogemailAlertToAddressTableToAddressValue;
  xLibStr256_t nextObjLogemailAlertToAddressTableToAddressValue;
  xLibU32_t keyLogemailAlertToAddressTableMessageTypeValue;
  xLibU32_t msgType;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: LogemailAlertToAddressTableMessageType */
  owa.len = sizeof (keyLogemailAlertToAddressTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableMessageType,
                          (xLibU8_t *) & keyLogemailAlertToAddressTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogemailAlertToAddressTableMessageTypeValue, owa.len);
  msgType = keyLogemailAlertToAddressTableMessageTypeValue;

  /* retrieve key: LogemailAlertToAddressTableToAddress */
  owa.len = sizeof (objLogemailAlertToAddressTableToAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableToAddress,
                          (xLibU8_t *) & objLogemailAlertToAddressTableToAddressValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_STR256(nextObjLogemailAlertToAddressTableToAddressValue);
    owa.l7rc = usmDbLogEmailAlertNextToAddrGet (L7_UNIT_CURRENT,&keyLogemailAlertToAddressTableMessageTypeValue,
                                                  nextObjLogemailAlertToAddressTableToAddressValue);
   
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertToAddressTableToAddressValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjLogemailAlertToAddressTableToAddressValue, objLogemailAlertToAddressTableToAddressValue);
    
    owa.l7rc = usmDbLogEmailAlertNextToAddrGet (L7_UNIT_CURRENT,&keyLogemailAlertToAddressTableMessageTypeValue,
                                                  nextObjLogemailAlertToAddressTableToAddressValue);
  }

  if ((owa.l7rc == L7_SUCCESS) && (keyLogemailAlertToAddressTableMessageTypeValue != msgType))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLogemailAlertToAddressTableToAddressValue, owa.len);

  /* return the object value: LogemailAlertToAddressTableToAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLogemailAlertToAddressTableToAddressValue,
                           sizeof (nextObjLogemailAlertToAddressTableToAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertToAddressTable_toAddressTableStatus
*
* @purpose Get 'toAddressTableStatus'
 *@description  [toAddressTableStatus] Log email Alert To Address
* Table Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertToAddressTable_toAddressTableStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objLogemailAlertToAddressTableStatusValue;
  xLibU32_t keyLogemailAlertToAddressTableMessageTypeValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: LogemailAlertToAddressTableMessageType */
  owa.len = sizeof (keyLogemailAlertToAddressTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableMessageType,
                          (xLibU8_t *) & keyLogemailAlertToAddressTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogemailAlertToAddressTableMessageTypeValue, owa.len);
  /* get the value from application */
  objLogemailAlertToAddressTableStatusValue = L7_ROW_STATUS_ACTIVE;

  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertToAddressTableStatusValue, sizeof(objLogemailAlertToAddressTableStatusValue));

  /* return the object value: LogemailAlertToAddressTableStatus */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objLogemailAlertToAddressTableStatusValue,
                      sizeof(objLogemailAlertToAddressTableStatusValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baselogemailAlertToAddressTable_toAddressTableStatus
*
* @purpose Set 'toAddressTableStatus'
 *@description  [toAddressTableStatus] Log email Alert To Address
* Table Status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertToAddressTable_toAddressTableStatus (void *wap, void *bufp)
{    
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogemailAlertToAddressTableStatusValue;
  xLibU32_t keyLogemailAlertToAddressTableMessageTypeValue;  
  xLibStr256_t objLogemailAlertToAddressTableToAddressValue;
  L7_LOG_EMAIL_ALERT_ERROR_t errorValue;
  xLibU32_t tmpMsgType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogemailAlertToAddressTableStatus */
  owa.len = sizeof (objLogemailAlertToAddressTableStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLogemailAlertToAddressTableStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogemailAlertToAddressTableStatusValue, owa.len);

  /* retrieve key: LogemailAlertToAddressTableMessageType */
  owa.len = sizeof(keyLogemailAlertToAddressTableMessageTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableMessageType,
                        (xLibU8_t *) & keyLogemailAlertToAddressTableMessageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLogemailAlertToAddressTableMessageTypeValue, owa.len);

  /* retrieve key: LogemailAlertToAddressTableToAddress */
  owa.len = sizeof(objLogemailAlertToAddressTableToAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertToAddressTable_toAddressTableToAddress,
                        (xLibU8_t *) &objLogemailAlertToAddressTableToAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLogemailAlertToAddressTableToAddressValue, owa.len);

  if (objLogemailAlertToAddressTableStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    if (keyLogemailAlertToAddressTableMessageTypeValue == L7_LOGGING_EMAIL_ALERT_BOTH)
    {
      for(tmpMsgType = L7_LOG_EMAIL_ALERT_URGENT; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
      {
        owa.l7rc = usmDbLogEmailAlertToAddrSet(USMDB_UNIT_CURRENT, tmpMsgType,
                                         objLogemailAlertToAddressTableToAddressValue, &errorValue);
      }
    }
    else
    {
      owa.l7rc = usmDbLogEmailAlertToAddrSet(L7_UNIT_CURRENT,keyLogemailAlertToAddressTableMessageTypeValue,
                                   objLogemailAlertToAddressTableToAddressValue, &errorValue);
    }
    if (errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID)
    {
      owa.rc = XLIBRC_INVALID_EMAIL_ADDRESS;    /* TODO: Change if required */
    }
    else if (errorValue == L7_LOG_EMAIL_ALERT_MAX_EMAIL_ADDRESSES_CONFIGURED)
    {
      owa.rc = XLIBRC_MAX_EMAIL_ADDRESSES_EXCEEDED;    /* TODO: Change if required */
    }
    else if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ROW_STATUS_ADD;    /* TODO: Change if required */
    }
  }
  else if (objLogemailAlertToAddressTableStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the value in application */
    owa.l7rc = usmDbLogEmailAlertToAddrRemove(L7_UNIT_CURRENT,keyLogemailAlertToAddressTableMessageTypeValue,
                                              objLogemailAlertToAddressTableToAddressValue);
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

