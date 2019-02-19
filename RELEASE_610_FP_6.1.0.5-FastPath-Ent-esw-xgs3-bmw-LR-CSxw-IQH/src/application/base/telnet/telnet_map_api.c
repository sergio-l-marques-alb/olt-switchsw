/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  telnet_map_api.c
*
* @purpose   Outbound Telnet API functions
*
* @component Outbound Telnet Mapping Layer
*
* @comments      
*
* @create    02/27/2004
*
* @author    anindya
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_telnetinclude.h"
#include "login_sessions_api.h"


extern L7_telnetMapCfg_t       *pTelnetMapCfgData;
extern telnetMapCtrl_t         telnetMapCtrl_g;


/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Get the Outbound Telnet administrative mode 
*
* @param    *pMode   @b{(output)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t telnetMapAdminModeGet(L7_uint32 *pMode)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    *pMode = 0;                /* neither enabled nor diabled */
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  *pMode = pTelnetMapCfgData->telnet.telnetAdminMode;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the  Outbound Telnet administrative mode
*
* @param    mode   @b{(input)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t telnetMapAdminModeSet(L7_uint32 mode)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if Outbound Telnet component not started by configurator */
  if (pTelnetMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  /* assumes current admin mode already established, so nothing more to do here */
  if (mode == pTelnetMapCfgData->telnet.telnetAdminMode)
    return L7_SUCCESS;

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  pTelnetMapCfgData->telnet.telnetAdminMode = mode;
  TELNET_SET_CONFIG_DATA_DIRTY;
  rc = L7_SUCCESS;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return rc;
}


/*********************************************************************
* @purpose  Get the maximum number of Outbound Telnet sessions allowed
*
* @param    *pMaxNoOfSessions   @b{(output)} Maximum No. of sessions
*                                            allowed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t telnetMapMaxSessionsGet(L7_uint32 *pMaxNoOfSessions)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  *pMaxNoOfSessions = pTelnetMapCfgData->telnet.telnetMaxSessions;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the maximum number of Outbound Telnet sessions allowed 
*
* @param    maxNoOfSessions   @b{(input)} Maximum No. of sessions
*                                         allowed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t telnetMapMaxSessionsSet(L7_uint32 maxNoOfSessions)
{
  L7_RC_t       rc = L7_FAILURE;

  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  pTelnetMapCfgData->telnet.telnetMaxSessions = maxNoOfSessions;
  TELNET_SET_CONFIG_DATA_DIRTY;
  rc = L7_SUCCESS;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return rc;
}


/*********************************************************************
* @purpose   Get number of active Outbound Telnet sessions
*
* @param     *pNoOfActiveSessions   @b{(output)} No. of active sessions
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t telnetMapNoOfActiveSessionsGet(L7_uint32 *pNoOfActiveSessions)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  *pNoOfActiveSessions = pTelnetMapCfgData->telnet.telnetNoOfActiveSessions;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the Outbound Telnet login inactivity timeout
*
* @param    *pTimeout   @b{(output)} Login inactivity timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t telnetMapTimeoutGet(L7_uint32 *pTimeout)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  *pTimeout = pTelnetMapCfgData->telnet.telnetTimeout;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the Outbound Telnet login inactivity timeout
*
* @param    timeout   @b{(input)} Login inactivity timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t telnetMapTimeoutSet(L7_uint32 timeout)
{
  L7_RC_t       rc = L7_FAILURE;

  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  pTelnetMapCfgData->telnet.telnetTimeout = timeout;
  TELNET_SET_CONFIG_DATA_DIRTY;
  rc = L7_SUCCESS;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return rc;
}


/*********************************************************************
* @purpose  To reset the maximum number of outbound telnet sessions
*           allowed to its default value
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t telnetMapMaxSessionsReset(void)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  pTelnetMapCfgData->telnet.telnetMaxSessions = L7_TELNET_DEFAULT_MAX_SESSIONS;
  TELNET_SET_CONFIG_DATA_DIRTY;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To reset the Outbound Telnet login inactivity timeout to
*           its default value
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t telnetMapTimeoutReset(void)
{
  if (pTelnetMapCfgData == L7_NULL)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  pTelnetMapCfgData->telnet.telnetTimeout = L7_TELNET_DEFAULT_MAX_SESSION_TIMEOUT;
  TELNET_SET_CONFIG_DATA_DIRTY;

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  Socket created
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_int32 telnetMapNewCon(telnetParams_t *telnetParams)
{
  return telnetNewCon(telnetParams);
}

/*********************************************************************
* @purpose  Close an Outbound Telnet session
*
* @param    clientSock       	@b{(input)}  Client socket
* @param    serverSock       	@b{(input)}  Server socket
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetMapDisconnect(L7_uint32 clientSock, L7_uint32 serverSock)
{ 
  telnetDisconnect(clientSock, serverSock);
}


#ifdef L7_CLI_PACKAGE
/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  Socket created
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t obtMapServerInputHandle(L7_uint32 clientSock, L7_uint32 serverSock)
{
  return telnetServerInputHandle(clientSock, serverSock);
} 

/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  Socket created
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t obtMapClientInputHandle(L7_uint32 clientSock, L7_uint32 serverSock)
{
  return telnetClientInputHandle(clientSock, serverSock);
}

/*********************************************************************
* @purpose Call the session login.
*
* @param L7_uint32 *val       ptr to login sesion
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE   - if reached max connections
*
* @end
*
*********************************************************************/
L7_RC_t cliObtLoginSessionStart(L7_uint32 loginSession)
{
  obtLoginSessionStart(loginSession);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Call the session login.
*
* @param L7_uint32 *val       ptr to login sesion
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE   - if reached max connections
*
* @end
*
*********************************************************************/
L7_RC_t cliObtLoginSessionEnd(L7_uint32 loginSession)
{
  obtLoginSessionEnd(loginSession);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the telnet session type.  
*
* @param L7_uint32 index      index value
* @param L7_uint32 *val       ptr to session type
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t cliLoginSessionUpdateObtActivityTime( L7_uint32 index)
{
  loginSessionUpdateObtActivityTime(index);
  return L7_SUCCESS;
}
#endif /* L7_CLI_PACKAGE */

