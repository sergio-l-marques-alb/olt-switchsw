/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrLoginSession.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  12 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseUsrMgrLoginSession_obj.h"
#include "usmdb_user_mgmt_api.h"
#include "ewnet.h"
#include "session.h"


xLibRC_t fpObjUtil_baseUsrMgrLoginSession_LoginSessionFirstGet (xLibU32_t *loginIndex)
{
  L7_BOOL validLogin = L7_FALSE;

  *loginIndex = 0;

  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  while (*loginIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, *loginIndex, &validLogin);
    if (validLogin == L7_TRUE)
      return L7_SUCCESS;
    *loginIndex = *loginIndex + 1;
  }

  return L7_FAILURE;
}

xLibRC_t fpObjUtil_baseUsrMgrLoginSession_LoginSessionNextGet (xLibU32_t loginIndex,
	                                                      xLibU32_t *loginIndexNext)
{
  L7_BOOL validLogin = L7_FALSE;

   if (loginIndex < 0 || loginIndex > FD_CLI_DEFAULT_MAX_CONNECTIONS)
   	return L7_FAILURE;
   
  loginIndex = loginIndex+1;
  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  while (loginIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, loginIndex, &validLogin);
    if (validLogin == L7_TRUE)
    {
      *loginIndexNext = loginIndex;
      return L7_SUCCESS;
    }
    loginIndex = loginIndex + 1;
  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginSession_SessionType
*
* @purpose Set 'SessionType'
*
* @description [SessionType]: Pass the Type Of session. Choose Unknown to
*              get count of all connection types. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginSession_SessionType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSessionTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SessionType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSessionTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSessionTypeValue, owa.len);
 /* This object is a dummy place holde rto shoose the type to be used by NoOfActiveSessions */
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

  
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_NoOfActiveSessions
*
* @purpose Get 'NoOfActiveSessions'
*
* @description [NoOfActiveSessions]: the number of active login sessions on
*              switch 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_NoOfActiveSessions (void *wap,
                                                             void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNoOfActiveSessionsValue;

   fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  xLibU32_t allSessions = 0;;
  
  FPOBJ_TRACE_ENTER (bufp);

 

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionType,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  if(keySessionIndexValue == L7_LOGIN_TYPE_UNKNWN)
  {
  	allSessions = 1;	
  }
  else
  {
      allSessions = 0;
  }
  

  /* get the value from application */
  objNoOfActiveSessionsValue =
    usmDbLoginNumSessionsActiveGet (allSessions,
                                    keySessionIndexValue);
  owa.l7rc = L7_SUCCESS;
  
  FPOBJ_TRACE_VALUE (bufp, &objNoOfActiveSessionsValue,
                     sizeof (objNoOfActiveSessionsValue));

  /* return the object value: NoOfActiveSessions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNoOfActiveSessionsValue,
                           sizeof (objNoOfActiveSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_SessionIndex
*
* @purpose Get 'SessionIndex'
*
* @description [SessionIndex]: determine if the given login session exists
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_SessionIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSessionIndexValue;
  xLibU32_t nextObjSessionIndexValue;

  L7_BOOL validSession;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & objSessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjSessionIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSessionIndexValue, owa.len);
	nextObjSessionIndexValue = objSessionIndexValue+1;
  }
   validSession = L7_FALSE;
   while ((nextObjSessionIndexValue < FD_UI_DEFAULT_MAX_CONNECTIONS) && (validSession == L7_FALSE))
   {
     if (nextObjSessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
     {
       if (usmDbLoginSessionValidEntry(0, nextObjSessionIndexValue, &validSession) != L7_SUCCESS)
       {
         validSession = L7_FALSE;
       }
     }
     else  /* Web Session */
     {
       validSession = EwaSessionIsActive(nextObjSessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS);
     }
     if (validSession == L7_FALSE)
     {
       nextObjSessionIndexValue++;
     }
   }

   if (validSession == L7_TRUE)
   {
     owa.l7rc = L7_SUCCESS;
   }
	else
	{
	  owa.l7rc = L7_FAILURE;
	}

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSessionIndexValue, owa.len);

  /* return the object value: SessionIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSessionIndexValue,
                           sizeof (objSessionIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_RemoteIpAddr
*
* @purpose Get 'RemoteIpAddr'
 *@description  [RemoteIpAddr] the remote IP address for current telnet session   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_RemoteIpAddr (void *wap, void *bufp)
{


  fpObjWa_t kwaSessionIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objRemoteIpAddrValue;
  L7_BOOL validLogin = L7_FALSE;
  EwaSession session;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwaSessionIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                                      (xLibU8_t *) & keySessionIndexValue, &kwaSessionIndex.len);
  if (kwaSessionIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSessionIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSessionIndex);
    return kwaSessionIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwaSessionIndex.len);

  /* get the value from application */


  if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, keySessionIndexValue, &validLogin) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
  }
  else
  {
    if (EwaSessionGet(keySessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS, &session) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
    else
    {
      validLogin = session.active;
    }
  }

  if ( validLogin == L7_TRUE )
  {
  
     if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
    {
      owa.l7rc = usmDbLoginSessionRemoteIpAddrGet(L7_UNIT_CURRENT, keySessionIndexValue, &objRemoteIpAddrValue);
    }
    else
    {
      memcpy(&objRemoteIpAddrValue, &session.inetAddr, sizeof(L7_inet_addr_t));
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RemoteIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRemoteIpAddrValue, sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;



}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginSession_RemoteIpAddr
*
* @purpose Set 'RemoteIpAddr'
 *@description  [RemoteIpAddr] the remote IP address for current telnet session   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginSession_RemoteIpAddr (void *wap, void *bufp)
{


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objRemoteIpAddrValue;

  fpObjWa_t kwaSessionIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RemoteIpAddr */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRemoteIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRemoteIpAddrValue, owa.len);

  /* retrieve key: SessionIndex */
  kwaSessionIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                                      (xLibU8_t *) & keySessionIndexValue, &kwaSessionIndex.len);
  if (kwaSessionIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSessionIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSessionIndex);
    return kwaSessionIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwaSessionIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbLoginSessionRemoteIpAddrSet (L7_UNIT_CURRENT, keySessionIndexValue, &objRemoteIpAddrValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;



}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_Type
*
* @purpose Get 'Type'
*
* @description [Type]: the login session type 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_Type (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;

  L7_BOOL validLogin = L7_FALSE;
  EwaSession session;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */

  if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, keySessionIndexValue, &validLogin) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
  }
  else
  {
    if (EwaSessionGet(keySessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS, &session) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
    else
    {
      validLogin = session.active;
    }
  }

  if ( validLogin == L7_TRUE )
  {
  
     if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
    {
      owa.l7rc = usmDbLoginSessionTypeGet(L7_UNIT_CURRENT, keySessionIndexValue, &objTypeValue);
    }
    else
    {
      objTypeValue = session.type;
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue,
                           sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginSession_Type
*
* @purpose Set 'Type'
*
* @description [Type]: the login session type 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginSession_Type (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Type */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbLoginSessionTypeSet (L7_UNIT_CURRENT, keySessionIndexValue,
                                       objTypeValue);
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
* @function fpObjGet_baseUsrMgrLoginSession_InterfaceIndex
*
* @purpose Get 'InterfaceIndex'
*
* @description [InterfaceIndex]: the login session interface index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_InterfaceIndex (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLoginSessionIfIndexGet (L7_UNIT_CURRENT, keySessionIndexValue,
                                          &objInterfaceIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: InterfaceIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInterfaceIndexValue,
                           sizeof (objInterfaceIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginSession_InterfaceIndex
*
* @purpose Set 'InterfaceIndex'
*
* @description [InterfaceIndex]: the login session interface index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginSession_InterfaceIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InterfaceIndex */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInterfaceIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInterfaceIndexValue, owa.len);


  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbLoginSessionIfIndexSet (L7_UNIT_CURRENT, keySessionIndexValue,
                                          objInterfaceIndexValue);
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
* @function fpObjGet_baseUsrMgrLoginSession_UserName
*
* @purpose Get 'UserName'
*
* @description [UserName]: the login session username. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_UserName (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserNameValue;

  L7_BOOL validLogin = L7_FALSE;
  EwaSession session;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */

    if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, keySessionIndexValue, &validLogin) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
  }
  else
  {
    if (EwaSessionGet(keySessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS, &session) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
    else
    {
      validLogin = session.active;
    }
  }

  if ( validLogin == L7_TRUE )
  {
  
     if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
    {
      owa.l7rc = usmDbLoginSessionUserGet(L7_UNIT_CURRENT, keySessionIndexValue, objUserNameValue);
    }
    else
    {
      osapiStrncpySafe(objUserNameValue, session.uname, sizeof(objUserNameValue));
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUserNameValue,
                           strlen (objUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_ActiveTime
*
* @purpose Get 'ActiveTime'
*
* @description [ActiveTime]: the login session time 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_ActiveTime (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objActiveTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_BOOL validLogin = L7_FALSE;
  EwaSession session;

  L7_uint32 current_time;

   current_time = osapiUpTimeRaw();
	

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */


  if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, keySessionIndexValue, &validLogin) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
  }
  else
  {
    if (EwaSessionGet(keySessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS, &session) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
    else
    {
      validLogin = session.active;
    }
  }

  if ( validLogin == L7_TRUE )
  {
  
     if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
    {
      owa.l7rc = usmDbLoginSessionTimeGet(L7_UNIT_CURRENT, keySessionIndexValue, &objActiveTimeValue);
    }
    else
    {
      objActiveTimeValue = current_time - session.first_access;
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ActiveTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objActiveTimeValue,
                           sizeof (objActiveTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_StartTime
*
* @purpose Get 'StartTime'
*
* @description [StartTime]: The login session start time 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_StartTime (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStartTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLoginSessionStartTimeGet (L7_UNIT_CURRENT, keySessionIndexValue,
                                   &objStartTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StartTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeValue,
                           sizeof (objStartTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_IdleTime
*
* @purpose Get 'IdleTime'
*
* @description [IdleTime]: The login session idle time 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_IdleTime (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIdleTimeValue;

  L7_BOOL validLogin = L7_FALSE;
  EwaSession session;

    L7_uint32 current_time;

   current_time = osapiUpTimeRaw();

	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */

    if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (usmDbLoginSessionValidEntry(L7_UNIT_CURRENT, keySessionIndexValue, &validLogin) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
  }
  else
  {
    if (EwaSessionGet(keySessionIndexValue-FD_CLI_DEFAULT_MAX_CONNECTIONS, &session) != L7_SUCCESS)
    {
      validLogin = L7_FALSE;
    }
    else
    {
      validLogin = session.active;
    }
  }

  if ( validLogin == L7_TRUE )
  {
  
     if (keySessionIndexValue < FD_CLI_DEFAULT_MAX_CONNECTIONS)
    {
      owa.l7rc = usmDbLoginSessionIdleTimeGet(L7_UNIT_CURRENT, keySessionIndexValue, &objIdleTimeValue);
    }
    else
    {
      objIdleTimeValue = current_time - session.last_access;
      owa.l7rc = L7_SUCCESS;
    }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IdleTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIdleTimeValue,
                           sizeof (objIdleTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginSession_Status
*
* @purpose Get 'Status'
*
* @description [Status]: the login session status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginSession_Status (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginSession_Status
*
* @purpose Set 'Status'
*
* @description [Status]: the login session status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginSession_Status (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySessionIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: SessionIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginSession_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {

    owa.l7rc =usmDbLoginSessionResetConnectionSet(L7_UNIT_CURRENT, keySessionIndexValue); 
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return XLIBRC_SUCCESS;
}
