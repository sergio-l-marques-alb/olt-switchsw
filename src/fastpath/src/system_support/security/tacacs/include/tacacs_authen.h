/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  tacacs_authen.h
*
* @purpose   TACACS+ Client definitions related to authentication.
*
* @component tacacs+
*
* @comments  This file consists of constant definitions related to
*            Authentication.
*
* @create    03/17/2005
*
* @author    gkiran
*            dfowler 06/23/05
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_TACACS_AUTHEN_H
#define INCLUDE_TACACS_AUTHEN_H

/******************************************************
**        Internal function prototypes               **
******************************************************/
L7_RC_t tacacsAuthSessionGet(userMgrAuthRequest_t *request,
                             L7_uint32             correlator,
                             tacacsAuthSession_t **session);
void tacacsContactNextServer(tacacsAuthSession_t *session);
void tacacsAuthenticationStart(tacacsAuthSession_t *session);
void tacacsAuthStateBeginProcess(tacacsAuthSession_t *session);
void tacacsAuthResponseProcess(tacacsRxPacket_t *rxPacket);
void tacacsServerRemoveProcess(L7_uint32 ip);
void tacacsServerModifyProcess(L7_uint32 ip);
L7_uint32 tacacsAuthResponseSessionIdGet(tacacsRxPacket_t *rxPacket);
void tacacsAuthFailureSend(tacacsAuthSession_t *session,
                           tacacsAuthStatus_t   status);
void tacacsServerConnectFailedProcess(L7_uint32 ip,
                                      L7_uint32 port,
                                      L7_uint32 socket);
void tacacsServerConnectedProcess(L7_uint32 ip,
                                  L7_uint32 port,
                                  L7_uint32 socket);

#endif /* INCLUDE_TACACS_AUTHEN_H */
