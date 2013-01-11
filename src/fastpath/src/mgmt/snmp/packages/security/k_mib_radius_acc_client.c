/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius_acc_client.c
*
* Purpose: System-specific code for Radius Accounting Client MIB
*
* Created by: Colin Verne 05/12/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_radius_acc_client_api.h>
#include "osapi_support.h"
#include "usmdb_common.h"
#include "radius_exports.h"
#include "usmdb_radius_api.h"

radiusAccClient_t *
k_radiusAccClient_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
  static radiusAccClient_t radiusAccClientData;

  L7_uint32 nasIdSize;
#ifdef I_radiusAccClientAddress
  L7_BOOL nasIpMode = L7_TRUE;
#endif /* I_radiusAccClientAddress */
  static L7_BOOL firstTime = L7_TRUE;
  char nasIdBuffer[L7_RADIUS_NAS_IDENTIFIER_SIZE+1];
    
  ZERO_VALID(radiusAccClientData.valid);

  bzero(nasIdBuffer, sizeof(nasIdBuffer));

  if (firstTime == L7_TRUE)
  {
    radiusAccClientData.radiusAccClientIdentifier = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  switch (nominator)
  {
  case I_radiusAccClientInvalidServerAddresses:
    if (usmDbRadiusAcctServerStatInvalidAddressesGet(USMDB_UNIT_CURRENT,  
                                                     &radiusAccClientData.radiusAccClientInvalidServerAddresses) == L7_SUCCESS)
    SET_VALID(nominator, radiusAccClientData.valid);
    break;

  case I_radiusAccClientIdentifier:
      nasIdSize = sizeof( nasIdBuffer );
      usmDbRadiusNASIdentifierGet(USMDB_UNIT_CURRENT, nasIdBuffer, &nasIdSize );
      if (SafeMakeOctetString(&radiusAccClientData.radiusAccClientIdentifier, nasIdBuffer, nasIdSize) == L7_TRUE )
          SET_VALID(nominator, radiusAccClientData.valid);
    break;

#ifdef I_radiusAccClientAddress
  case I_radiusAccClientAddress:
     if(usmDbRadiusAttribute4Get(USMDB_UNIT_CURRENT,&nasIpMode,&radiusAccClientData.radiusAccClientAddress) == L7_SUCCESS)
     SET_VALID(nominator,radiusAccClientData.valid);
    break;
#endif /* I_radiusAccClientAddress */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, radiusAccClientData.valid))
    return(NULL);

  return(&radiusAccClientData);
}

radiusAccServerEntry_t *
k_radiusAccServerEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 radiusAccServerIndex)
{
  static radiusAccServerEntry_t radiusAccServerEntryData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  ZERO_VALID(radiusAccServerEntryData.valid);

  radiusAccServerEntryData.radiusAccServerIndex = radiusAccServerIndex;
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  SET_VALID(I_radiusAccServerIndex, radiusAccServerEntryData.valid);

  if ((searchType == EXACT) ? 
      (snmpRadiusAccServerEntryGet(USMDB_UNIT_CURRENT, radiusAccServerEntryData.radiusAccServerIndex,
                                   snmp_buffer, &addrType) != L7_SUCCESS) :
      ((snmpRadiusAccServerEntryGet(USMDB_UNIT_CURRENT, radiusAccServerEntryData.radiusAccServerIndex,
                                   snmp_buffer, &addrType) != L7_SUCCESS) &&
       (snmpRadiusAccServerEntryNextGet(USMDB_UNIT_CURRENT, &radiusAccServerEntryData.radiusAccServerIndex,
                                    snmp_buffer, &addrType) != L7_SUCCESS)))
  {
    ZERO_VALID(radiusAccServerEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case I_radiusAccServerIndex:
    break;
  case I_radiusAccServerAddress:
    if (usmDbRadiusAccountingServerIPByIndexGet(USMDB_UNIT_CURRENT,radiusAccServerEntryData.radiusAccServerIndex,
                                                 &radiusAccServerEntryData.radiusAccServerAddress)== L7_SUCCESS)
    {
       SET_VALID(I_radiusAccServerAddress, radiusAccServerEntryData.valid);
    }
    break;

  case I_radiusAccClientServerPortNumber:
    if (usmDbRadiusAccountingHostNameServerPortNumGet(USMDB_UNIT_CURRENT,snmp_buffer,addrType,
                                              &radiusAccServerEntryData.radiusAccClientServerPortNumber) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientServerPortNumber, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientRoundTripTime:
    if (usmDbRadiusAcctServerStatRTTGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                        &radiusAccServerEntryData.radiusAccClientRoundTripTime) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientRoundTripTime, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientRequests:
    if (usmDbRadiusAcctServerStatReqGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                        &radiusAccServerEntryData.radiusAccClientRequests) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientRequests, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientRetransmissions:
    if (usmDbRadiusAcctServerStatRetransGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                            &radiusAccServerEntryData.radiusAccClientRetransmissions) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientRetransmissions, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientResponses:
    if (usmDbRadiusAcctServerStatResponseGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                             &radiusAccServerEntryData.radiusAccClientResponses) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientResponses, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientMalformedResponses:
    if (usmDbRadiusAcctServerStatMalformedResponseGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                                      &radiusAccServerEntryData.radiusAccClientMalformedResponses) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientMalformedResponses, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientBadAuthenticators:
    if (usmDbRadiusAcctServerStatBadAuthGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                            &radiusAccServerEntryData.radiusAccClientBadAuthenticators) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientBadAuthenticators, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientPendingRequests:
    if (usmDbRadiusAcctServerStatPendingReqGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                               &radiusAccServerEntryData.radiusAccClientPendingRequests) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientPendingRequests, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientTimeouts:
    if (usmDbRadiusAcctServerStatTimeoutsGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                             &radiusAccServerEntryData.radiusAccClientTimeouts) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientTimeouts, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientUnknownTypes:
    if (usmDbRadiusAcctServerStatUnknownTypeGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                                &radiusAccServerEntryData.radiusAccClientUnknownTypes) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientUnknownTypes, radiusAccServerEntryData.valid);
    break;

  case I_radiusAccClientPacketsDropped:
    if (usmDbRadiusAcctServerStatPktsDroppedGet(USMDB_UNIT_CURRENT,snmp_buffer,
                                                &radiusAccServerEntryData.radiusAccClientPacketsDropped) == L7_SUCCESS)
      SET_VALID(I_radiusAccClientPacketsDropped, radiusAccServerEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, radiusAccServerEntryData.valid))
    return(NULL);

  return(&radiusAccServerEntryData);
}
