/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_api.c
*
* @purpose TACACS+ Client API's
*
* @component tacacas+
*
* @comments
*
* @create 04/10/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "commdefs.h"
#include "defaultconfig.h"
#include "log.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_authen.h"
#include "tacacs_control.h"
#include "osapi.h"
#include "osapi_support.h"

extern void                *tacacsQueue;
extern void                *tacacsSemaphore;
extern tacacsNotifyEntry_t *tacacsNotifyList;
extern tacacsCfg_t         *tacacsCfg;
extern tacacsOprData_t     *tacacsOprData;

/*************************************************************************
*
* @purpose Authenticate a user with a TACACS+ server
*
* @param L7_char8  *username     @b{(input)} name of user to be authenticated.
* @param L7_char8  *password     @b{(input)} password for the user.
* @param L7_uint32  correlator   @b{(input)} correlator.
* @param L7_uint32  componentId  @b{(input)} component Id.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t tacacsUserAuthenticate(userMgrAuthRequest_t *request,
                               L7_uint32  correlator)
{
  tacacsAuthSession_t *session;
  tacacsMsg_t          msg;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(request->component, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /*
   * It is not necessarily an error to have NULL pointers or zero length
   * strings for the username or password.  This is because the prompt for
   * each may be supplied by the TACACS+ server in the ASKUSER/ASKPASS
   * messages.
   */
  if ((L7_NULLPTR != request->pUserName) &&
      (strlen(request->pUserName) > (L7_TACACS_USER_NAME_MAX-1)))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID, "TACACS+: user name is too long.");
    return L7_FAILURE;
  }

  if ((L7_NULLPTR != request->pPwd) &&
      (strlen(request->pPwd) > (L7_TACACS_PASSWORD_MAX-1)))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID, "TACACS+: password is too long.");
    return L7_FAILURE;
  }

  if (request->component >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID,
            "TACACS+: received request from invalid componentId %u.", request->component);
    return L7_FAILURE;
  }

  /*
   * However, a user name must be supplied for enable since the server will
   * not prompt for a user name.
   */
  if ((ACCESS_LEVEL_ENABLE == request->mode) &&
      ((L7_NULLPTR         == request->pUserName) ||
       (1                  >  strlen(request->pUserName))))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID, "TACACS+: user name in enable request is missing.");
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsNotifyList[request->component].notifyFunction == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID,
            "TACACS+: received request from unregistered componentId %u, %s.", request->component, name);
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  if (tacacsAuthSessionGet(request, correlator, &session) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TACACS_COMPONENT_ID,
           "TACACS+: authentication error, no authentication sessions available");
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  /* session processing takes place on the client task */
  msg.event = TACACS_EVENT_AUTH_REQUEST;
  msg.data.session = session;
  if (osapiMessageSend(tacacsQueue,
                       &msg,
                       sizeof(tacacsMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: authentication error, failed to send TACACS_EVENT_AUTH_REQUEST.");
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*************************************************************************
*
* @purpose Command Authorization with a TACACS+ server
*
* @param L7_char8  *username     @b{(input)} name of user to be authenticated.
* @param L7_char8  *password     @b{(input)} password for the user.
* @param L7_uint32  correlator   @b{(input)} correlator.
* @param L7_uint32  componentId  @b{(input)} component Id.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t tacacsUserCmdAuthorize( userMgrAuthRequest_t *request,
                                L7_uint32  correlator)
{
  tacacsAuthSession_t *session;
  tacacsMsg_t          msg;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(request->component, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if ((request->pUserName == L7_NULLPTR) ||
      (strlen(request->pUserName) < 1) ||
      (strlen(request->pUserName) > (L7_TACACS_USER_NAME_MAX-1)) ||
      (request->component > L7_LAST_COMPONENT_ID))
  {
    return L7_REQUEST_DENIED;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsNotifyList[request->component].notifyFunction == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: received request from unregistered componentId %u, %s.", request->component, name);
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  if (tacacsAuthSessionGet(request, correlator, &session) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: authorization error, no authorization sessions available");
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  /* session processing takes place on the client task */
  msg.event = TACACS_EVENT_AUTH_REQUEST;
  msg.data.session = session;
  if (osapiMessageSend(tacacsQueue,
                       &msg,
                       sizeof(tacacsMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: authorization error, failed to send TACACS_EVENT_AUTH_REQUEST.");
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}




/*********************************************************************
*
* @purpose Register a routine to be called when a TACACS+ response is
*          received from a server for a previously submitted request.
*
* @param L7_uint32 componentId @b((input)) routine registrar id (See L7_COMPONENT_ID_t)
* @param *notify @b((input)) pointer to a routine to be invoked upon a respones.
*                            Each routine has the following parameters:
*                            (status, correlator, *attributes, attributesLen)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsResponseRegister(L7_uint32 componentId,
                               L7_uint32 (*notify)(L7_uint32 status,
                                                   L7_uint32 correlator,
                                                   L7_uint32 accessLevel))
{
  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  tacacsNotifyList[componentId].notifyFunction = notify;
  osapiSemaGive(tacacsSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Deregister all routines to be called when a tacacs response is
*          received from a server for a previously submitted request.
*
* @param   L7_uint32 componentId  @b{(input)}  one of L7_COMPONENT_IDS_t
*
* @returns L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsResponseDeregister(L7_uint32 componentId)
{
  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  tacacsNotifyList[componentId].notifyFunction = L7_NULLPTR;
  osapiSemaGive(tacacsSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the TACACS+ server port number.
*
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} Address type DNS or IP address
* @param L7_uchar8   serverAddress @b{(input)} the Hostname or
*                                              IP address of the server.
* @param L7_ushort16 port          @b{(input)} the port number value to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPortNumberSet(L7_IP_ADDRESS_TYPE_t type,
                                          L7_uchar8  *serverAddress,
                                          L7_ushort16 port)
{
  L7_uint32   serverIdx = 0;
  tacacsMsg_t msg;

  memset(&msg, 0, sizeof(tacacsMsg_t));

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }

  if (tacacsCfg->servers[serverIdx].port != port)
  {
    tacacsCfg->servers[serverIdx].port = port;
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server port number.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_short16 *port         @b{(output)}pointer to port number
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPortNumberGet(L7_IP_ADDRESS_TYPE_t type,
                                          L7_uchar8  *serverAddress,
                                          L7_short16 *port)
{
  L7_uint32  serverIdx = 0;

  if (port == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *port = tacacsCfg->servers[serverIdx].port;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the TACACS+ server time out value.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 timeout        @b{(input)} connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes timeout == 0 indicates to use global timeout configuration,
*        by default each server uses global value unless set.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerTimeoutSet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8  *serverAddress,
                                       L7_uchar8   timeout)
{
  L7_uint32 serverIdx = 0;

  if (timeout > L7_TACACS_TIMEOUT_MAX)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  if (timeout == 0)
  {
    if (tacacsCfg->servers[serverIdx].useGblTimeout == L7_FALSE)
    {
      tacacsCfg->servers[serverIdx].useGblTimeout = L7_TRUE;
      tacacsCfg->servers[serverIdx].timeout = FD_TACACS_PLUS_TIMEOUT_VALUE;
      tacacsCfg->header.dataChanged = L7_TRUE;
    }
  }
  else if ((tacacsCfg->servers[serverIdx].useGblTimeout == L7_TRUE) ||
           (tacacsCfg->servers[serverIdx].timeout != timeout))
  {
    tacacsCfg->servers[serverIdx].useGblTimeout = L7_FALSE;
    tacacsCfg->servers[serverIdx].timeout = timeout;
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server time out.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 *timeout       @b{(output)} pointer to timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerTimeoutGet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8  *serverAddress,
                                       L7_uchar8 *timeout)
{
  L7_uint32 serverIdx = 0;

  if (timeout == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *timeout = tacacsCfg->servers[serverIdx].timeout;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server time out global flag.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *useGlobal     @b{(output)} L7_TRUE, using global timeout
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerUseGlobalTimeoutGet(L7_IP_ADDRESS_TYPE_t type,
                                                L7_uchar8  *serverAddress,
                                                L7_BOOL    *useGlobal)
{
  L7_uint32 serverIdx = 0;

  if (useGlobal == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *useGlobal = tacacsCfg->servers[serverIdx].useGblTimeout;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the TACACS+ server secret key.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_char8  *key           @b{(input)} encryption key.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes key == L7_NULLPTR or "" indicates to use global key configuration,
*        by default each server uses global value unless set.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSecretKeySet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_char8   *key)
{
  L7_uint32 serverIdx = 0;
  L7_uint32 length = 0;


  length = strlen(key);
  if (length > L7_TACACS_KEY_LEN_MAX)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  if ((key == L7_NULLPTR) ||
      (length == 0))
  {
    if (tacacsCfg->servers[serverIdx].useGblKey == L7_FALSE)
    {
      tacacsCfg->servers[serverIdx].useGblKey = L7_TRUE;
      memset(tacacsCfg->servers[serverIdx].key, 0, L7_TACACS_KEY_LEN_MAX+1);
      tacacsCfg->header.dataChanged = L7_TRUE;
    }
  }
  else if ((tacacsCfg->servers[serverIdx].useGblKey == L7_TRUE) ||
           (strcmp(tacacsCfg->servers[serverIdx].key, key) != 0))
  {
    tacacsCfg->servers[serverIdx].useGblKey = L7_FALSE;
    strcpy(tacacsCfg->servers[serverIdx].key, key);
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server key configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uchar8 *key           @b{(output)} key string
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSecretKeyGet(L7_IP_ADDRESS_TYPE_t type,
                                         L7_uchar8  *serverAddress,
                                         L7_uchar8  *key)
{
  L7_uint32 serverIdx = 0;

  if (key == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  strcpy(key, tacacsCfg->servers[serverIdx].key);
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server key global flag.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8 serverAddress  @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *useGlobal     @b{(output)} L7_TRUE, using global key
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerUseGlobalKeyGet(L7_IP_ADDRESS_TYPE_t type,
                                            L7_uchar8  *serverAddress,
                                            L7_BOOL    *useGlobal)
{
  L7_uint32 serverIdx = 0;

  if (useGlobal == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *useGlobal = tacacsCfg->servers[serverIdx].useGblKey;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To set the TACACS+ server priority.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_ushort16  priority    @b{(input)} the server priority.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPrioritySet(L7_IP_ADDRESS_TYPE_t type,
                                        L7_uchar8  *serverAddress,
                                        L7_ushort16 priority)
{
  L7_uint32 serverIdx = 0;

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  if (tacacsCfg->servers[serverIdx].priority != priority)
  {
    tacacsCfg->servers[serverIdx].priority = priority;
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server priority configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_ushort16  *priority   @b{(output)} pointer to server priority
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerPriorityGet(L7_IP_ADDRESS_TYPE_t type,
                                        L7_uchar8  *serverAddress,
                                        L7_ushort16 *priority)
{
  L7_uint32 serverIdx = 0;

  if (priority == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *priority = tacacsCfg->servers[serverIdx].priority;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To Set the TACACS+ server Source Ip Address.
*
* @param L7_uint32 ipAddress  @b{(input)} the IP address of the server.
* @param L7_uint32 sourceIP   @b{(input)} the Source Ip Address to be set.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes need some ipAddress value to indicate global configuration
*         TBD - 255.255.255.255?
*        NOT IMPLEMENTED, place holder for L3 multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerSrcIpSet(L7_uint32 ipAddress, L7_uint32 sourceIP)
{
  /* this configuration is intended for L3, the src ip address must be
     validated against valid router interface ip addresses, in an L2
     environment there is only one valid outgoing interface, the
     mgmt interface, which is the default configuration, do not
     allow modification from the default */
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Get the TACACS+ server source IP configuration
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_uint32 *sourceIP      @b{(output)} pointer to source IP
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerSrcIpGet(L7_IP_ADDRESS_TYPE_t type,
                                     L7_uchar8  *serverAddress,
                                     L7_uint32 *sourceIP)
{
  L7_uint32 serverIdx = 0;

  if (sourceIP == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  *sourceIP = tacacsCfg->servers[serverIdx].sourceIP;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the first configured TACACS+ server.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(output)} Address type DNS or IP address
* @param L7_uchar8  pServerAddress @b{(output)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure, no servers configured.
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerFirstGet(L7_IP_ADDRESS_TYPE_t *type,
                                     L7_uchar8  *pServerAddress)
{
  if (pServerAddress == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(pServerAddress, 0, strlen(pServerAddress));
  return tacacsHostNameServerNextGet(pServerAddress, type, pServerAddress);
}

/*********************************************************************
*
* @purpose Get the next configured TACACS+ server.
*
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_IP_ADDRESS_TYPE_t type@b{(inout)} Address type DNS or IP address
* @param L7_uchar8  pNextServerAddress @b{(output)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes ipAddress of 0 returns first configured server.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerNextGet(L7_uchar8  *serverAddress,
                                    L7_IP_ADDRESS_TYPE_t *type,
                                    L7_uchar8  *pNextServerAddress)
{
  L7_uint32 serverIdx = 0;

  if (pNextServerAddress == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (serverAddress[0] != 0)
  {
    if((tacacsServerIPHostNameIndexGet(*type, serverAddress,
                                       &serverIdx) != L7_SUCCESS)
       && (serverIdx == L7_TACACS_MAX_SERVERS-1))
    {
      osapiSemaGive(tacacsSemaphore);
      return L7_FAILURE;
    }
    serverIdx++;
  }

  /* If max servers are reached or both ip and hostName are non existent */
  if ((serverIdx == L7_TACACS_MAX_SERVERS) ||
      ((tacacsCfg->servers[serverIdx].ip == 0) &&
       (tacacsCfg->servers[serverIdx].hostName.host.hostName[0] == 0)))
  {
      osapiSemaGive(tacacsSemaphore);
      return L7_FAILURE;
  }

  if (tacacsCfg->servers[serverIdx].hostName.hostAddrType ==
                            L7_IP_ADDRESS_TYPE_IPV4)
  {
    *type =  L7_IP_ADDRESS_TYPE_IPV4;
    strcpy(pNextServerAddress,
           osapiInet_ntoa(tacacsCfg->servers[serverIdx].ip));
  }
  else if (tacacsCfg->servers[serverIdx].hostName.hostAddrType ==
                              L7_IP_ADDRESS_TYPE_DNS)
  {
    *type =  L7_IP_ADDRESS_TYPE_DNS;
    strcpy(pNextServerAddress, tacacsCfg->servers[serverIdx].
                              hostName.host.hostName);
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a new TACACS+ server configuration.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerAdd(L7_IP_ADDRESS_TYPE_t type,
                                L7_uchar8  *serverAddress)
{
  L7_uint32 i = 0;
  L7_uint32 ipAddr = L7_NULL;
  tacacsMsg_t          msg;

  if((serverAddress[0] == 0) ||
     (type != L7_IP_ADDRESS_TYPE_DNS &&
      type != L7_IP_ADDRESS_TYPE_IPV4))
  {
    return L7_FAILURE;
  }
  if(type == L7_IP_ADDRESS_TYPE_IPV4)
  {
    ipAddr = osapiInet_addr(serverAddress);
    if (ipAddr == 0)
    {
      /* not a valid ip address */
      return L7_FAILURE;
    }
  }
  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    if (((type == L7_IP_ADDRESS_TYPE_IPV4) &&
        (tacacsCfg->servers[i].ip == ipAddr)) ||
        ((type == L7_IP_ADDRESS_TYPE_DNS) &&
        (strcmp(tacacsCfg->servers[i].hostName.host.hostName,
                serverAddress) == 0)))
    {
      /* entry already exists */
      osapiSemaGive(tacacsSemaphore);
      return L7_ALREADY_CONFIGURED;
    }
    if (tacacsCfg->servers[i].hostName.hostAddrType ==
                        L7_IP_ADDRESS_TYPE_UNKNOWN)
    {
      break;
    }
  }
  if (i < L7_TACACS_MAX_SERVERS)
  {
    tacacsBuildDefaultServerConfigData(L7_TACACS_CFG_VER_CURRENT, &tacacsCfg->servers[i]);
    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      tacacsCfg->servers[i].ip = ipAddr;
      tacacsCfg->servers[i].hostName.hostAddrType =
                         L7_IP_ADDRESS_TYPE_IPV4;
    }
    else
    {
      tacacsCfg->servers[i].ip = 0;
      osapiStrncpySafe(tacacsCfg->servers[i].hostName.host.hostName,
                       serverAddress,
                       sizeof(tacacsCfg->servers[i].hostName.host.hostName));
      tacacsCfg->servers[i].hostName.hostAddrType =
                         L7_IP_ADDRESS_TYPE_DNS;
      msg.event = TACACS_EVENT_HOSTNAME_RESOLVE;
      osapiStrncpySafe(msg.data.hostName, serverAddress, sizeof(msg.data.hostName));
      if (osapiMessageSend(tacacsQueue, &msg, sizeof(tacacsMsg_t),
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                "TACACS+: DNS Resolution error, failed to send TACACS_EVENT_HOSTNAME_RESOLVE.");
      }
    }
    tacacsCfg->header.dataChanged = L7_TRUE;
    osapiSemaGive(tacacsSemaphore);
    return L7_SUCCESS;
  }
  /* list is full */
  osapiSemaGive(tacacsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose To delete TACACS+ server configuration
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerRemove(L7_IP_ADDRESS_TYPE_t type,
                                   L7_uchar8  *serverAddress)
{
  L7_uint32 i = 0;
  L7_BOOL found = L7_FALSE;
  L7_uint32 ipAddr = 0;

  if ((serverAddress[0] == 0) ||
     (type != L7_IP_ADDRESS_TYPE_DNS && type != L7_IP_ADDRESS_TYPE_IPV4))
  {
    /* not a valid ip address */
    return L7_FAILURE;
  }

  if(type == L7_IP_ADDRESS_TYPE_IPV4)
  {
    ipAddr = osapiInet_addr(serverAddress);
  }
  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    if (found == L7_TRUE && i > 0)
    {
      /* move entries after removed entry down one */
      if (tacacsCfg->servers[i].hostName.hostAddrType !=
                L7_IP_ADDRESS_TYPE_UNKNOWN)
      {
        memcpy(&tacacsOprData->servers[i-1], &tacacsOprData->servers[i], sizeof(tacacsServerOprData_t));
        memset(&tacacsOprData->servers[i], 0, sizeof(tacacsServerOprData_t));
        memcpy(&tacacsCfg->servers[i-1], &tacacsCfg->servers[i], sizeof(tacacsServerCfg_t));
        memset(&tacacsCfg->servers[i], 0, sizeof(tacacsServerCfg_t));
      }
    }
    else if ((type == L7_IP_ADDRESS_TYPE_IPV4)&&
            (tacacsCfg->servers[i].ip == ipAddr))
    {
      memset(&tacacsOprData->servers[i], 0, sizeof(tacacsServerOprData_t));
      memset(&tacacsCfg->servers[i], 0, sizeof(tacacsServerCfg_t));
      found = L7_TRUE;
    }
    else if (strcmp(tacacsCfg->servers[i].hostName.host.hostName,
                    serverAddress) == 0)
    {
      ipAddr = tacacsCfg->servers[i].ip;
      memset(&tacacsOprData->servers[i], 0, sizeof(tacacsServerOprData_t));
      memset(&tacacsCfg->servers[i], 0, sizeof(tacacsServerCfg_t));
      found = L7_TRUE;
    }
  }

  /* For server with host name if IP resolution fails there would be no active
       session. Hence no need of posting the message */
  if (found == L7_TRUE && ipAddr != 0)
  {
    tacacsMsg_t msg;
    /* we need to take care of any sessions waiting on this server */
    msg.event = TACACS_EVENT_SERVER_REMOVE;
    msg.data.ip = ipAddr;
    if (osapiMessageSend(tacacsQueue,
                         &msg,
                         sizeof(tacacsMsg_t),
                         L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
              "TACACS+: failed to send server remove message\n");
    }

    tacacsCfg->header.dataChanged = L7_TRUE;
    osapiSemaGive(tacacsSemaphore);
    return L7_SUCCESS;
  }
  else if( found == L7_TRUE)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_SUCCESS;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose To set the TACACS+ global source IP configuration
*
* @param L7_uint32 ipAddress  @b{(input)} source IP address.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSrcIpSet(L7_uint32 ipAddress)
{
  /* this configuration is intended for L3, the src ip address must be
     validated against valid router interface ip addresses, in an L2
     environment there is only one valid outgoing interface, the
     mgmt interface, which is the default configuration, do not
     allow modification from the default */
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose To get the TACACS+ global source IP configuration
*
* @param L7_uint32 *ipAddress  @b{(output)} pointer to source IP.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSrcIpGet(L7_uint32 *ipAddress)
{
  if (ipAddress == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *ipAddress = tacacsCfg->sourceIP;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To set the TACACS+ global timeout configuration.
*
* @param L7_uchar8 timeout  @b{(input)} connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblTimeoutSet(L7_uchar8 timeout)
{
  if ((timeout < L7_TACACS_TIMEOUT_MIN) ||
      (timeout > L7_TACACS_TIMEOUT_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsCfg->timeout != timeout)
  {
    tacacsCfg->timeout = timeout;
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get the TACACS+ global timeout configuration
*
* @param L7_uchar8 *timeout  @b{(output)} pointer to connection timeout in seconds.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblTimeoutGet(L7_uchar8 *timeout)
{
  if (timeout == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  *timeout = tacacsCfg->timeout;
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To set the TACACS+ global key configuration
*
* @param L7_uchar8 *key  @b{(input)} global key string.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSecretKeySet(L7_uchar8 *key)
{
  L7_short16 length = 0;

  if (key == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  length = strlen(key);
  if ((length < L7_TACACS_KEY_LEN_MIN) ||
      (length > L7_TACACS_KEY_LEN_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (strcmp(tacacsCfg->key, key) != 0)
  {
    strcpy(tacacsCfg->key, key);
    tacacsCfg->header.dataChanged = L7_TRUE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get the TACACS+ Gloabal Secret Key.
*
* @param L7_uchar8 *key  @b{(output)} the Global Secret Key to be get.
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsGblSecretKeyGet(L7_uchar8 *key)
{
  if (key == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  strcpy(key, tacacsCfg->key);
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get the TACACS+ server connection status.
*
* @param L7_IP_ADDRESS_TYPE_t type@b{(input)} Address type DNS or IP address
* @param L7_uchar8  serverAddress @b{(input)} the Hostname or
*                                             IP address of the server.
* @param L7_BOOL   *status        @b{(output)} L7_TRUE(connected) or
*                                              L7_FALSE(disconnected)
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes only a single-connection server will ever be connected,
*        and single-connection is not implemented, keep configuration
*        and operational structures as placeholder.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsHostNameServerConnectionStatusGet(L7_IP_ADDRESS_TYPE_t type,
                                                L7_uchar8  *serverAddress,
                                                L7_BOOL    *status)
{
  L7_uint32 serverIdx = 0;

  if (status == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  if (tacacsServerIPHostNameIndexGet(type, serverAddress,
                                     &serverIdx) != L7_SUCCESS)
  {
    osapiSemaGive(tacacsSemaphore);
    return L7_FAILURE;
  }
  if ((tacacsCfg->servers[serverIdx].singleConnection == L7_TRUE) &&
      (tacacsOprData->servers[serverIdx].socket != 0) &&
      (tacacsOprData->servers[serverIdx].status == TACACS_SERVER_STATUS_CONNECTED))
  {
    *status = L7_TRUE;
  }
  else
  {
    *status = L7_FALSE;
  }
  osapiSemaGive(tacacsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Provide debug devshell command to set unencrypted flag.
*
* @param L7_BOOL flag @b{(input)} L7_TRUE(unencrypted) or L7_FALSE(encrypted)
*
* @returns void
*
* @notes This should not be used in operation, for debug/support only.
*        unencrypted flag is NOT saved with configuration, so must
*        be set after each reload if desired.
*
* @end
*
*********************************************************************/
void tacacsDebugUnencryptedFlagSet(L7_BOOL flag)
{
  osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
  tacacsOprData->unencrypted = flag;
  osapiSemaGive(tacacsSemaphore);
}
