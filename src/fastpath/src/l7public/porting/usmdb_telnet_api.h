/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  usmdb_telnet.c
*
* @purpose   Provide interface to API's for Outbound Telnet UI components
*
* @component unitmgr
*
* @comments
*
* @create    10/03/2004
*
* @author    anindya
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef _USMDB_TELNET_API_H_
#define _USMDB_TELNET_API_H_


/*********************************************************************
* @purpose  Get Outbound Telnet admin mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *pMode      @b{(output)} admin mode
*
* @returns  L7_SUCCESS  admin mode Get
* @returns  L7_FAILURE  Failed to Get admin mode
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetAdminModeGet(L7_uint32 UnitIndex, L7_uint32  *pMode);

/*********************************************************************
* @purpose  Set Outbound Telnet admin mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    mode        @b{(input)} admin mode
*
* @returns  L7_SUCCESS  admin mode set
* @returns  L7_FAILURE  Failed to set admin mode
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetAdminModeSet(L7_uint32 UnitIndex, L7_uint32  mode);

/*********************************************************************
* @purpose   Get the maximum number of Outbound Telnet sessions allowed
*
* @param     UnitIndex           @b{(input)}  Unit for this operation
* @param     *pMaxNoOfSessions   @b{(output)} Maximum No. of sessions
*                                             allowed
*
* @returns   L7_SUCCESS  Got Outbound Telnet max no of sessions allowed
* @returns   L7_FAILURE  Failed to get max no of sessions
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetMaxSessionsGet(L7_uint32 UnitIndex, L7_uint32 *pMaxNoOfSessions);

/*********************************************************************
* @purpose   Set the maximum number of Outbound Telnet sessions allowed
*
* @param     UnitIndex         @b{(input)} Unit for this operation
* @param     maxNoOfSessions   @b{(input)} Maximum No. of sessions
*                                          allowed
*
* @returns   L7_SUCCESS  Outbound Telnet max no of sessions is set
* @returns   L7_FAILURE  Failed to set max no of sessions
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetMaxSessionsSet(L7_uint32 UnitIndex, L7_uint32 maxNoOfSessions);

/*********************************************************************
* @purpose   Get number of active Outbound Telnet sessions
*
* @param     UnitIndex              @b{(input)}  Unit for this operation
* @param     *pNoOfActiveSessions   @b{(output)} No. of active sessions
*
* @returns   L7_SUCCESS  Got no. of active Outbound Telnet sessions
* @returns   L7_FAILURE  Failed to get no of active sessions
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetNoOfActiveSessionsGet(L7_uint32 UnitIndex, L7_uint32 *pNoOfActiveSessions);

/*********************************************************************
* @purpose   Get the Outbound Telnet login inactivity timeout
*
* @param     UnitIndex   @b{(input)}  Unit for this operation
* @param     *pTimeout   @b{(output)} Login inactivity timeout value
*
* @returns   L7_SUCCESS  Got Outbound Telnet login inactivity timeout
* @returns   L7_FAILURE  Failed to get login inactivity timeout
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetTimeoutGet(L7_uint32 UnitIndex, L7_uint32 *pTimeout);

/*********************************************************************
* @purpose   Set the Outbound Telnet login inactivity timeout
*
* @param     UnitIndex   @b{(input)} Unit for this operation
* @param     timeout     @b{(input)} Login inactivity timeout value
*
* @returns   L7_SUCCESS  Outbound Telnet login inactivity timeout is set
* @returns   L7_FAILURE  Failed to set login inactivity timeout
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetTimeoutSet(L7_uint32 UnitIndex, L7_uint32 timeout);

/*********************************************************************
* @purpose   To reset the maximum number of outbound telnet sessions
*            allowed to its default value
*
* @param     UnitIndex   @b{(input)}  Unit for this operation
*
* @returns   L7_SUCCESS  Outbound Telnet max no of sessions is set
* @returns   L7_FAILURE  Failed to set max no of sessions
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetMaxSessionsReset(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose   To reset the Outbound Telnet login inactivity timeout to
*            its default value
*
* @param     UnitIndex   @b{(input)} Unit for this operation
*
* @returns   L7_SUCCESS  Outbound Telnet login inactivity timeout is set
* @returns   L7_FAILURE  Failed to set login inactivity timeout
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTelnetTimeoutReset(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    L7_uint32       UnitIndex      @b((input)) Unit no.
* @param    telnetParams_t  telnetparams   @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/
extern L7_int32 usmDbTelnetNewCon(L7_uint32 UnitIndex, telnetParams_t *telnetparams);

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
extern void usmDbTelnetDisconnect(L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
*
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    L7_uint32       UnitIndex      @b((input)) Unit no.
* @param    telnetParams_t  telnetparams   @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/
extern L7_RC_t usmdbObtServerInputHandle(L7_uint32 UnitIndex, L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
*
* @purpose  Create and allocte a socket on the remote host and also
*           populate the linked list
*
* @param    L7_uint32       UnitIndex      @b((input)) Unit no.
* @param    telnetParams_t  telnetparams   @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes   Check return code to determine result
*
* @end
*********************************************************************/
extern L7_RC_t usmdbObtClientInputHandle(L7_uint32 UnitIndex, L7_uint32 clientSock, L7_uint32 serverSock);

/*********************************************************************
* @purpose Call the session login.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 *val       ptr to login sesion
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE   - if reached max connections
*
* @notes This should be eliminated once the web and cli are updated
*        to call cliWeb directly.
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbObtLoginSessionStart(L7_uint32 index);

/*********************************************************************
* @purpose Call the session login.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 *val       ptr to login sesion
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE   - if reached max connections
*
* @notes This should be eliminated once the web and cli are updated
*        to call cliWeb directly.
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbObtLoginSessionEnd(L7_uint32 index);

/*********************************************************************
*
* @purpose Get the telnet session type.  
*
* @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUpdateObtActivityTime(L7_uint32 UnitIndex, L7_uint32 index);


#endif /* _USMDB_TELNET_API_H_ */

