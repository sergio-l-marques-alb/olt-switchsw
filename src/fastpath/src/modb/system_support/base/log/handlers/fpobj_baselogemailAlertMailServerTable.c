
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2010
*
********************************************************************************
*
* @filename fpobj_baselogemailAlertMailServerTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  10 March 2010, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baselogemailAlertMailServerTable_obj.h"
#include "usmdb_log_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerAddrType
*
* @purpose Get 'mailServerTableServerAddrType'
 *@description  [mailServerTableServerAddrType] Email Alert Mail Server IP
* Address Type.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerAddrType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmailServerTableServerAddrTypeValue;
  xLibU32_t nextObjmailServerTableServerAddrTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mailServerTableServerAddrType */
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                          (xLibU8_t *) & objmailServerTableServerAddrTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmailServerTableServerAddrTypeValue = L7_IP_ADDRESS_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmailServerTableServerAddrTypeValue, owa.len);

    if(objmailServerTableServerAddrTypeValue == L7_IP_ADDRESS_TYPE_IPV6)
    {
      nextObjmailServerTableServerAddrTypeValue = L7_IP_ADDRESS_TYPE_DNS;
      owa.l7rc = L7_SUCCESS;
    }
    else if(objmailServerTableServerAddrTypeValue == L7_IP_ADDRESS_TYPE_IPV4)
    {
      nextObjmailServerTableServerAddrTypeValue = L7_IP_ADDRESS_TYPE_IPV6;
      owa.l7rc = L7_SUCCESS;
    }
	 else if(objmailServerTableServerAddrTypeValue == L7_IP_ADDRESS_TYPE_DNS)
    {
      owa.l7rc = L7_FAILURE;
    }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmailServerTableServerAddrTypeValue, owa.len);

  /* return the object value: mailServerTableServerAddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmailServerTableServerAddrTypeValue,
                           sizeof (nextObjmailServerTableServerAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerAddr
*
* @purpose Get 'mailServerTableServerAddr'
 *@description  [mailServerTableServerAddr] Log email Alert Mail Server Address.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  xLibStr256_t nextObjmailServerTableServerAddrValue;
  xLibU32_t addrType;
  FPOBJ_TRACE_ENTER(bufp);

  FPOBJ_CLR_STR256(objmailServerTableServerAddrValue);
  FPOBJ_CLR_STR256(nextObjmailServerTableServerAddrValue);

  /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                              (xLibU8_t *) &keymailServerTableServerAddrTypeValue,
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);
  addrType = keymailServerTableServerAddrTypeValue;
  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) objmailServerTableServerAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
    owa.l7rc = usmDbLogEmailAlertNextMailServerAddressGet(L7_UNIT_CURRENT,
                           &keymailServerTableServerAddrTypeValue, objmailServerTableServerAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmailServerTableServerAddrValue, owa.len);
    owa.l7rc = usmDbLogEmailAlertNextMailServerAddressGet(L7_UNIT_CURRENT,
                           &keymailServerTableServerAddrTypeValue, objmailServerTableServerAddrValue);
  }

  if ((owa.l7rc == L7_SUCCESS) && (keymailServerTableServerAddrTypeValue != addrType))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjmailServerTableServerAddrValue,objmailServerTableServerAddrValue);	

  FPOBJ_TRACE_NEW_KEY(bufp, nextObjmailServerTableServerAddrValue, owa.len);

  /* return the object value: mailServerTableServerAddr */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)nextObjmailServerTableServerAddrValue,
                      strlen(nextObjmailServerTableServerAddrValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerSecurity
*
* @purpose Get 'mailServerTableServerSecurity'
 *@description  [mailServerTableServerSecurity] Email Alert Mail Server Security.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerSecurity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibU32_t objmailServerTableServerSecurityValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  FPOBJ_TRACE_ENTER(bufp);

  /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue,
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertMailServerSecurityGet(L7_UNIT_CURRENT,objmailServerTableServerAddrValue,
                                                     &objmailServerTableServerSecurityValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableServerSecurityValue,
                     strlen(objmailServerTableServerSecurityValue));

  /* return the object value: mailServerTableServerSecurity */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objmailServerTableServerSecurityValue,
                          sizeof(objmailServerTableServerSecurityValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertMailServerTable_mailServerTableServerSecurity
*
* @purpose Set 'mailServerTableServerSecurity'
 *@description  [mailServerTableServerSecurity] Email Alert Mail Server Security. 
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertMailServerTable_mailServerTableServerSecurity (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objmailServerTableServerSecurityValue;
  xLibStr256_t objmailServerTableServerAddrValue; 

  FPOBJ_TRACE_ENTER(bufp);


  /* retrieve object: mailServerTableServerSecurity */
  owa.len = sizeof(objmailServerTableServerSecurityValue);
  owa.rc = xLibBufDataGet(bufp,
                          (xLibU8_t *)&objmailServerTableServerSecurityValue,
                          &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableServerSecurityValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertMailServerSecuritySet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                     objmailServerTableServerSecurityValue);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableloginID
*
* @purpose Get 'mailServerTableloginID'
 *@description  [mailServerTableloginID] Email Alert loginID.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableloginID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibStr256_t objmailServerTableloginIDValue;
  xLibStr256_t objmailServerTableServerAddrValue;

  FPOBJ_TRACE_ENTER(bufp);

  /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue,
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);

  /* retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue,
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertMailServerUsernameGet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                     objmailServerTableloginIDValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmailServerTableloginIDValue, strlen(objmailServerTableloginIDValue));

  /* return the object value: mailServerTableloginID */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objmailServerTableloginIDValue,
                          strlen(objmailServerTableloginIDValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertMailServerTable_mailServerTableloginID
*
* @purpose Set 'mailServerTableloginID'
 *@description  [mailServerTableloginID] Email Alert loginID.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertMailServerTable_mailServerTableloginID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objmailServerTableloginIDValue;
  xLibStr256_t objmailServerTableServerAddrValue;

  FPOBJ_TRACE_ENTER(bufp);


  /* retrieve object: mailServerTableloginID */
  owa.len = sizeof(objmailServerTableloginIDValue);
  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)objmailServerTableloginIDValue,
                          &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objmailServerTableloginIDValue, owa.len);

  /* retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertMailServerUsernameSet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                     objmailServerTableloginIDValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTablePasswd
*
* @purpose Get 'mailServerTablePasswd'
 *@description  [mailServerTablePasswd] Email Alert Passwd.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTablePasswd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibStr256_t objmailServerTablePasswdValue;
  xLibStr256_t objmailServerTableServerAddrValue;

  FPOBJ_TRACE_ENTER(bufp);

  /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);

  /* retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);
  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertMailServerPasswdGet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                   objmailServerTablePasswdValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmailServerTablePasswdValue, strlen(objmailServerTablePasswdValue));

  /* return the object value: mailServerTablePasswd */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objmailServerTablePasswdValue,
                          strlen(objmailServerTablePasswdValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertMailServerTable_mailServerTablePasswd
*
* @purpose Set 'mailServerTablePasswd'
 *@description  [mailServerTablePasswd] Email Alert Passwd.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertMailServerTable_mailServerTablePasswd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objmailServerTablePasswdValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  FPOBJ_TRACE_ENTER(bufp);


  /* retrieve object: mailServerTablePasswd */
  owa.len = sizeof(objmailServerTablePasswdValue);
  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)objmailServerTablePasswdValue,
                          &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objmailServerTablePasswdValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertMailServerPasswdSet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                   objmailServerTablePasswdValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableStatus
*
* @purpose Get 'mailServerTableStatus'
 *@description  [mailServerTableStatus] Log email Alert Mail Server Table Status   
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  xLibU32_t objmailServerTableStatusValue;

  FPOBJ_TRACE_ENTER(bufp);
   /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* get the value from application */
  objmailServerTableStatusValue = L7_ROW_STATUS_ACTIVE;

  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableStatusValue, sizeof(objmailServerTableStatusValue));

  /* return the object value: LogemailAlertToAddressTableStatus */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objmailServerTableStatusValue,
                          sizeof(objmailServerTableStatusValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertMailServerTable_mailServerTableStatus
*
* @purpose Set 'mailServerTableStatus'
 *@description  [mailServerTableStatus] Log email Alert Mail Server Table Status   
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertMailServerTable_mailServerTableStatus (void *wap, void *bufp)
{     
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibU32_t objmailServerTableStatusValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  L7_IP_ADDRESS_TYPE_t addresstype = L7_IP_ADDRESS_TYPE_UNKNOWN;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mailServerTableStatus */
  owa.len = sizeof (objmailServerTableStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmailServerTableStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableStatusValue, owa.len);

  /* retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue,
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);
  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                          (xLibU8_t *) & objmailServerTableServerAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmailServerTableServerAddrValue, owa.len);

  addresstype = keymailServerTableServerAddrTypeValue;

  if (objmailServerTableStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    owa.l7rc = usmDbLogEmailAlertMailServerAddressSet(L7_UNIT_CURRENT, addresstype,
                                                      objmailServerTableServerAddrValue);
    if (owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_MAX_MAIL_SERVERS_EXCEEDED;    /* TODO: Change if required */
    }
    else if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ROW_STATUS_ADD;    /* TODO: Change if required */
    }
  }
  else if (objmailServerTableStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the value in application */
    owa.l7rc = usmDbLogEmailAlertMailServerAddressRemove(L7_UNIT_CURRENT,
                                                         addresstype, objmailServerTableServerAddrValue);
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

/*******************************************************************************
* @function fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerPort
*
* @purpose Get 'mailServerTableServerPort'
 *@description  [mailServerTableServerPort] Email Alert Mail Server Port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baselogemailAlertMailServerTable_mailServerTableServerPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t keymailServerTableServerAddrTypeValue;
  xLibU32_t objmailServerTableServerPortValue;
  xLibStr256_t objmailServerTableServerAddrValue;
  FPOBJ_TRACE_ENTER(bufp);

  /*retrieve key: mailServerTableServerAddrType */
  owa.len = sizeof(keymailServerTableServerAddrTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddrType,
                         (xLibU8_t *) &keymailServerTableServerAddrTypeValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keymailServerTableServerAddrTypeValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, 
                         &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLogEmailAlertMailServerPortGet(L7_UNIT_CURRENT,objmailServerTableServerAddrValue,
                                                 &objmailServerTableServerPortValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableServerPortValue,
                     strlen(objmailServerTableServerPortValue));

  /* return the object value: mailServerTableServerPort */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objmailServerTableServerPortValue,
                          sizeof(objmailServerTableServerPortValue));
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baselogemailAlertMailServerTable_mailServerTableServerPort
*
* @purpose Set 'mailServerTableServerPort'
 *@description  [mailServerTableServerPort] Email Alert Mail Server Port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baselogemailAlertMailServerTable_mailServerTableServerPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objmailServerTableServerPortValue;
  xLibStr256_t objmailServerTableServerAddrValue;

  FPOBJ_TRACE_ENTER(bufp);


  /* retrieve object: mailServerTableServerSecurity */
  owa.len = sizeof(objmailServerTableServerPortValue);
  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&objmailServerTableServerPortValue,
                          &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmailServerTableServerPortValue, owa.len);

  /* retrieve key: mailServerTableServerAddr */
  owa.len = sizeof(objmailServerTableServerAddrValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baselogemailAlertMailServerTable_mailServerTableServerAddr,
                         (xLibU8_t *) &objmailServerTableServerAddrValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &objmailServerTableServerAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLogEmailAlertMailServerPortSet(L7_UNIT_CURRENT, objmailServerTableServerAddrValue,
                                                 objmailServerTableServerPortValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}

