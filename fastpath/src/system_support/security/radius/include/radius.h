/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius.h
*
* @purpose RADIUS Client base protocol
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_RADIUS_H
#define INCLUDE_RADIUS_H

#include "async_event_api.h"

#define RADIUS_RANDOM_NUMBER_SEED 65535.0

/*
** Internal function prototypes
*/

L7_RC_t radiusResponseAuthenticatorValidate(radiusServerEntry_t *serverEntry, 
                                            radiusPacket_t *packet, 
                                            L7_char8 *requestAuthenticator);

L7_RC_t radiusMessageAuthenticatorValidate(radiusServerEntry_t *serverEntry, 
                                           radiusPacket_t *packet, 
                                           radiusRequest_t *request);

void radiusAuthenticatorSet(L7_char8 *authenticator, 
                            L7_uint32 authenticatorLength);

L7_RC_t radiusDigestCalculate(L7_char8 *digest, 
                              L7_char8 *secret, 
                              L7_uint32 secretLength, 
                              L7_char8 *authVector);

L7_RC_t radiusUserPasswordEncrypt(radiusServerEntry_t *serverEntry, 
                                  radiusRequest_t *request);

void radiusSecretConvert(radiusServerEntry_t *nextServerEntry, 
                         radiusRequest_t *requestList);

radiusPacket_t *radiusUserAuthenticateCreate(radiusServerEntry_t *serverEntry, 
                                             radiusRequest_t *request);

radiusPacket_t *radiusRequestPacketCreate(radiusServerEntry_t *serverEntry, 
                                          radiusRequest_t *request);

radiusPacket_t *radiusAccountingPacketCreate(radiusServerEntry_t *serverEntry, 
                                           radiusRequest_t *request);

void radiusAccountResponseProcess(radiusServerEntry_t *serverEntry, 
                                  radiusPacket_t *packet, 
                                  radiusRequest_t *request);

void radiusAccessChallengeProcess(radiusServerEntry_t *serverEntry, 
                                  radiusPacket_t *packet, 
                                  radiusRequest_t *request);

void radiusAccessRejectProcess(radiusServerEntry_t *serverEntry, 
                               radiusPacket_t *packet, 
                               radiusRequest_t *request);

void radiusAccessAcceptProcess(radiusServerEntry_t *serverEntry, 
                               radiusPacket_t *packet, 
                               radiusRequest_t *request);

L7_RC_t radiusResponseAttributesValidate(radiusServerEntry_t *serverEntry, 
                                         radiusPacket_t *packet, 
                                         radiusRequest_t *request);

L7_RC_t radiusRequestInfoProcess(radiusRequestInfo_t *requestInfo);

#ifdef L7_ROUTING_PACKAGE
L7_RC_t radiusRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *response);
#endif
#endif /* INCLUDE_RADIUS_H */

