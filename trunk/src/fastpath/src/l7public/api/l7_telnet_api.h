/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   l7_telnet_api.h
*
* @purpose    Outbound Telnet Mapping Layer APIs
*
* @component  Outbound Telnet Mapping Layer
*
* @comments   none
*
* @create     02/27/2004
*
* @author     anindya
*
* @end
*
**********************************************************************/

#ifndef _TELNET_MAP_API_H_
#define _TELNET_MAP_API_H_

/* Begin Function Prototypes */
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
* @purpose  Get Outbound Telnet admin mode
*
* @param    *pMode        @b{(output)} Admin mode
*
* @returns  L7_SUCCESS  Admin mode Get
* @returns  L7_FAILURE  Failed to Get admin mode
*
* @comments  Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapAdminModeGet(L7_uint32 *pMode);

/*********************************************************************
* @purpose  Set Outbound Telnet admin mode
*
* @param    mode        @b{(input)} Admin mode
*
* @returns  L7_SUCCESS  Admin mode Set
* @returns  L7_FAILURE  Failed to Set admin mode
*
* @comments  Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the maximum number of Outbound Telnet sessions allowed
*
* @param    *pMaxNoOfSessions   @b{(output)} Maximum No. of sessions
*                                            allowed
*
* @returns  L7_SUCCESS  Maximum number of Outbound Telnet sessions Get
* @returns  L7_FAILURE  Failed to Get maximum number of Outbound Telnet
*                       sessions
*
* @comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapMaxSessionsGet(L7_uint32 *pMaxNoOfSessions);

/*********************************************************************
* @purpose  Set the maximum number of Outbound Telnet sessions allowed
*
* @param    maxNoOfSessions   @b{(input)} Maximum No. of sessions
*                                         allowed
*
* @returns  L7_SUCCESS  Maximum number of Outbound Telnet sessions Set
* @returns  L7_FAILURE  Failed to Set maximum number of Outbound Telnet
*                       sessions 
*
* @comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapMaxSessionsSet(L7_uint32 maxNoOfSessions);

/*********************************************************************
* @purpose   Get number of active Outbound Telnet sessions
*
* @param     *pNoOfActiveSessions   @b{(output)} No. of active sessions
*
* @returns   L7_SUCCESS  Got no. of active Outbound Telnet sessions
* @returns   L7_FAILURE  Failed to get no of active sessions
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/

L7_RC_t telnetMapNoOfActiveSessionsGet(L7_uint32 *pNoOfActiveSessions);

/*********************************************************************
* @purpose  Get the Outbound Telnet login inactivity timeout
*
* @param    *pTimeout   @b{(output)} Login inactivity timeout value
*
* @returns  L7_SUCCESS  Outbound Telnet login inactivity timeout Get
* @returns  L7_FAILURE  Failed to Get Outbound Telnet login inactivity
*                       timeout
*
* @Comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapTimeoutGet(L7_uint32 *pTimeout);

/*********************************************************************
* @purpose  Set the Outbound Telnet login inactivity timeout
*
* @param    timeout   @b{(input)} Login inactivity timeout value
*
* @returns  L7_SUCCESS  Outbound Telnet login inactivity timeout Set
* @returns  L7_FAILURE  Failed to Set Outbound Telnet login inactivity
*                       timeout
*
* @comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapTimeoutSet(L7_uint32 timeout);

/*********************************************************************
* @purpose  To reset the maximum number of outbound telnet sessions
*           allowed to its default value
*
* @param    void
*
* @returns  L7_SUCCESS  Maximum number of outbound telnet sessions
*                       allowed is Reset to its default value
* @returns  L7_FAILURE  Failed to Reset maximum number of outbound
*                       telnet sessions allowed to its default value
*
* @comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapMaxSessionsReset(void);

/*********************************************************************
* @purpose  To reset the Outbound Telnet login inactivity timeout to
*           its default value
*
* @param    void
*
* @returns  L7_SUCCESS  Outbound Telnet login inactivity timeout is Reset
*                       to its default value
* @returns  L7_FAILURE  Failed to Reset outbound telnet login inactivity
*                       timeout to its default value
*
* @comments Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t telnetMapTimeoutReset(void);

/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/

L7_int32 telnetMapNewCon(telnetParams_t *telnetParams);

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

void telnetMapDisconnect(L7_uint32 clientSock, L7_uint32 serverSock);


/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t obtMapServerInputHandle(L7_uint32 clientSock, L7_uint32 serverSock);


/*********************************************************************
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    telnetParams_t telnetParams  @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/

L7_RC_t obtMapClientInputHandle(L7_uint32 clientSock, L7_uint32 serverSock);

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

L7_RC_t cliObtLoginSessionStart(L7_uint32 loginSession);

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

L7_RC_t cliObtLoginSessionEnd(L7_uint32 loginSession);

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
L7_RC_t cliLoginSessionUpdateObtActivityTime( L7_uint32 index);

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

L7_RC_t cliWebLoginSessionObtActive( L7_uint32 index, L7_BOOL *val);

#endif /* _TELNET_MAP_API_H_*/
