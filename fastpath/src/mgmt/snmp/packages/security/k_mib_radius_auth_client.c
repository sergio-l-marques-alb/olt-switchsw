/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius_auth_client.c
*
* Purpose: System-specific code for Radius Authentication Client MIB
*
* Created by: Colin Verne 05/10/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_radius_auth_client_api.h>
#include "osapi_support.h"
#include "usmdb_common.h"
#include "radius_exports.h"
#include "usmdb_radius_api.h"

radiusAuthClient_t *
k_radiusAuthClient_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
  static radiusAuthClient_t radiusAuthClientData;
  L7_uint32 nasIdSize;
  static L7_BOOL firstTime = L7_TRUE;
  char nasIdBuffer[L7_RADIUS_NAS_IDENTIFIER_SIZE+1];
    
  ZERO_VALID(radiusAuthClientData.valid);

  bzero(nasIdBuffer, sizeof(nasIdBuffer));

  if (firstTime == L7_TRUE)
  {
    radiusAuthClientData.radiusAuthClientIdentifier = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  switch (nominator)
  {
  case I_radiusAuthClientInvalidServerAddresses:
    if (usmDbRadiusStatInvalidAddressesGet(USMDB_UNIT_CURRENT,  
                                           &radiusAuthClientData.radiusAuthClientInvalidServerAddresses) == L7_SUCCESS)
    SET_VALID(nominator, radiusAuthClientData.valid);
    break;

  case I_radiusAuthClientIdentifier:
      nasIdSize = sizeof( nasIdBuffer );
      usmDbRadiusNASIdentifierGet(USMDB_UNIT_CURRENT, nasIdBuffer, &nasIdSize );
      if (SafeMakeOctetString(&radiusAuthClientData.radiusAuthClientIdentifier, nasIdBuffer, nasIdSize) == L7_TRUE )
          SET_VALID(nominator, radiusAuthClientData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, radiusAuthClientData.valid))
    return(NULL);
  return(&radiusAuthClientData);
}

radiusAuthServerEntry_t *
k_radiusAuthServerEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 radiusAuthServerIndex)
{
  static radiusAuthServerEntry_t radiusAuthServerEntryData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  radiusAuthServerEntryData.radiusAuthServerIndex = radiusAuthServerIndex;
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  SET_VALID(I_radiusAuthServerIndex, radiusAuthServerEntryData.valid);

  if ((searchType == EXACT) ? 
      (snmpRadiusAuthServerEntryGet(USMDB_UNIT_CURRENT, radiusAuthServerEntryData.radiusAuthServerIndex,
                                   snmp_buffer, &addrType) != L7_SUCCESS) :
      ((snmpRadiusAuthServerEntryGet(USMDB_UNIT_CURRENT, radiusAuthServerEntryData.radiusAuthServerIndex,
                                   snmp_buffer, &addrType) != L7_SUCCESS) &&
       (snmpRadiusAuthServerEntryNextGet(USMDB_UNIT_CURRENT, &radiusAuthServerEntryData.radiusAuthServerIndex,
                                    snmp_buffer, &addrType) != L7_SUCCESS)))
  {
    ZERO_VALID(radiusAuthServerEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case I_radiusAuthServerIndex:
    break;
  case I_radiusAuthServerAddress:
    if (usmDbRadiusServerIPByIndexGet(USMDB_UNIT_CURRENT,radiusAuthServerEntryData.radiusAuthServerIndex,&radiusAuthServerEntryData.radiusAuthServerAddress) == L7_SUCCESS )
      SET_VALID(I_radiusAuthServerAddress, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientServerPortNumber:
    if (usmDbRadiusHostNameServerPortNumGet(USMDB_UNIT_CURRENT, snmp_buffer, addrType,
                                                 &radiusAuthServerEntryData.radiusAuthClientServerPortNumber) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientServerPortNumber, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientRoundTripTime:
    if (usmDbRadiusServerStatRTTGet(USMDB_UNIT_CURRENT,snmp_buffer, 
                                              &radiusAuthServerEntryData.radiusAuthClientRoundTripTime) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientRoundTripTime, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientAccessRequests:
    if (usmDbRadiusServerStatAccessReqGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                               &radiusAuthServerEntryData.radiusAuthClientAccessRequests) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientAccessRequests, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientAccessRetransmissions:
    if (usmDbRadiusServerStatAccessRetransGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                                      &radiusAuthServerEntryData.radiusAuthClientAccessRetransmissions) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientAccessRetransmissions, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientAccessAccepts:
    if (usmDbRadiusServerStatAccessAcceptGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                              &radiusAuthServerEntryData.radiusAuthClientAccessAccepts) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientAccessAccepts, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientAccessRejects:
    if (usmDbRadiusServerStatAccessRejectGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                              &radiusAuthServerEntryData.radiusAuthClientAccessRejects) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientAccessRejects, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientAccessChallenges:
    if (usmDbRadiusServerStatAccessChallengeGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                                 &radiusAuthServerEntryData.radiusAuthClientAccessChallenges) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientAccessChallenges, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientMalformedAccessResponses:
    if (usmDbRadiusServerStatMalformedAccessResponseGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                                         &radiusAuthServerEntryData.radiusAuthClientMalformedAccessResponses) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientMalformedAccessResponses, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientBadAuthenticators:
    if (usmDbRadiusServerStatBadAuthGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                                  &radiusAuthServerEntryData.radiusAuthClientBadAuthenticators) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientBadAuthenticators, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientPendingRequests:
    if (usmDbRadiusServerStatPendingReqGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                                &radiusAuthServerEntryData.radiusAuthClientPendingRequests) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientPendingRequests, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientTimeouts:
    if (usmDbRadiusServerStatTimeoutsGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                         &radiusAuthServerEntryData.radiusAuthClientTimeouts) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientTimeouts, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientUnknownTypes:
    if (usmDbRadiusServerStatUnknownTypeGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                             &radiusAuthServerEntryData.radiusAuthClientUnknownTypes) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientUnknownTypes, radiusAuthServerEntryData.valid);
    break;

  case I_radiusAuthClientPacketsDropped:
    if (usmDbRadiusServerStatPktsDroppedGet(USMDB_UNIT_CURRENT, snmp_buffer,
                                               &radiusAuthServerEntryData.radiusAuthClientPacketsDropped) == L7_SUCCESS)
      SET_VALID(I_radiusAuthClientPacketsDropped, radiusAuthServerEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, radiusAuthServerEntryData.valid))
    return(NULL);

  return(&radiusAuthServerEntryData);
}

