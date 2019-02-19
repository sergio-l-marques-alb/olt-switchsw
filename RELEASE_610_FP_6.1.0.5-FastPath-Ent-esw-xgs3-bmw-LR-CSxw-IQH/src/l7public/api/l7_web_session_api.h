/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_web_session_api.h
*
* @purpose Code to support cookie based sessions
*
* @component 
*
* @comments Moved the API used in USMDB into this new file 
*      - Rama Sasthri, Kristipati
*
* @create 01/13/2005
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef L7_WEB_SESSION_API_H
#define L7_WEB_SESSION_API_H

/*********************************************************************
*
* @purpose Performs a lookup in the session table for the specified session
*          
* @param sIndex    session Index 
*
* @returns L7_TRUE or L7_FALSE
*
* @end
*
*********************************************************************/
L7_BOOL EwaSessionIsActive(L7_uint32 sIndex);

/*********************************************************************
*
* @purpose Get the idle time for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionIdleTimeGet(L7_uint32 sIndex, L7_uint32 *idleTime);

/*********************************************************************
*
* @purpose Get the up time for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionUpTimeGet(L7_uint32 sIndex, L7_uint32 *upTime);

/*********************************************************************
*
* @purpose Get the type for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionTypeGet(L7_uint32 sIndex, L7_uint32 *sType);

/*********************************************************************
*
* @purpose Get the user for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionUserGet(L7_uint32 sIndex, L7_char8 *user);

/*********************************************************************
*
* @purpose Get the inet address for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionInetAddrGet(L7_uint32 sIndex, L7_inet_addr_t *inetAddr);

void ewaSessionHttpMaxConnectionsSet(L7_ushort16 value);
void ewaSessionHttpSoftTimeOutSet(L7_ushort16 value);
void ewaSessionHttpHardTimeOutSet(L7_ushort16 value);

#endif /* L7_WEB_SESSION_API_H */

