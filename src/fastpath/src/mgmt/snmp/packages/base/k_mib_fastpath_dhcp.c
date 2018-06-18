/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_dhcp_private.c
*
* @purpose  Provide interface to DHCP Private MIB
*
* @component SNMP
*
* @comments
*
* @create 10/07/2003
*
* @author Kumar Manish
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_fastpath_dhcp_api.h"
#include "cnfgr.h"
#include "usmdb_common.h"
#include "dhcps_exports.h"
#include "usmdb_dhcps_api.h"

#ifdef L7_DHCPS_PACKAGE
agentDhcpServerGroup_t *
k_agentDhcpServerGroup_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{

  static agentDhcpServerGroup_t agentDhcpServerGroupData;

  switch (nominator)
  {
  case -1:

  case I_agentDhcpServerAdminMode:
    if (snmpAgentDhcpsAdminModeGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerAdminMode, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPingPktNos:
    if (usmDbDhcpsNoOfPingPktGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerPingPktNos) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPingPktNos, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerAutomaticBindingsNos:
    if (usmDbDhcpsNoOfActiveLeasesGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerAutomaticBindingsNos) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerAutomaticBindingsNos, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerExpiredBindingsNos:
    if (usmDbDhcpsNoOfExpiredLeasesGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerExpiredBindingsNos) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerExpiredBindingsNos, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerMalformedMessagesReceived:
    if (usmDbDhcpsMalformedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerMalformedMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerMalformedMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerDISCOVERMessagesReceived:
    if (usmDbDhcpsDiscoverReceivedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerDISCOVERMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerDISCOVERMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerREQUESTMessagesReceived:
    if (usmDbDhcpsRequestReceivedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerREQUESTMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerREQUESTMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerDECLINEMessagesReceived:
    if (usmDbDhcpsDeclineReceivedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerDECLINEMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerDECLINEMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerRELEASEMessagesReceived:
    if (usmDbDhcpsReleaseReceivedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerRELEASEMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerRELEASEMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerINFORMMessagesReceived:
    if (usmDbDhcpsInformReceivedGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerINFORMMessagesReceived) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerINFORMMessagesReceived, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerOFFERMessagesSent:
    if (usmDbDhcpsOfferSentGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerOFFERMessagesSent) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerOFFERMessagesSent, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerACKMessagesSent:
    if (usmDbDhcpsAckSentGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerACKMessagesSent) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerACKMessagesSent, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerNAKMessagesSent:
    if (usmDbDhcpsNackSentGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerNAKMessagesSent) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerNAKMessagesSent, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerClearStatistics:
    agentDhcpServerGroupData.agentDhcpServerClearStatistics = D_agentDhcpServerClearStatistics_disable;
    SET_VALID(I_agentDhcpServerClearStatistics, agentDhcpServerGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerBootpAutomatic:
    if (snmpAgentDhcpsBootpAutomaticGet(USMDB_UNIT_CURRENT, &agentDhcpServerGroupData.agentDhcpServerBootpAutomatic) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerBootpAutomatic, agentDhcpServerGroupData.valid);
    break;


  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator >= 0 && !VALID(nominator, agentDhcpServerGroupData.valid))
    return(NULL);
  return(&agentDhcpServerGroupData);

}


#ifdef SETS
int
k_agentDhcpServerGroup_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerGroup_set(agentDhcpServerGroup_t *data,
                           ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentDhcpServerAdminMode, data->valid) &&
      snmpAgentDhcpsAdminModeSet(USMDB_UNIT_CURRENT, data->agentDhcpServerAdminMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerAdminMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpServerPingPktNos, data->valid) &&
      snmpAgentDhcpsNoOfPingPktSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPingPktNos) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerPingPktNos, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpServerClearStatistics, data->valid) &&
      snmpAgentDhcpsClearStatistics(USMDB_UNIT_CURRENT, data->agentDhcpServerClearStatistics) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerClearStatistics, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpServerBootpAutomatic, data->valid) &&
      snmpAgentDhcpsBootpAutomaticSet(USMDB_UNIT_CURRENT, data->agentDhcpServerBootpAutomatic) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerBootpAutomatic, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;

}

  #ifdef SR_agentDhcpServerGroup_UNDO
/* add #define SR_agentDhcpServerGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerGroup family.
 */
int
agentDhcpServerGroup_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerGroup_UNDO */

#endif /* SETS */


agentDhcpServerPoolConfigEntry_t *
k_agentDhcpServerPoolConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_UINT32 agentDhcpServerPoolIndex)
{
  static L7_BOOL firstTime = L7_TRUE;
  L7_RC_t rc =L7_FAILURE;
  static agentDhcpServerPoolConfigEntry_t agentDhcpServerPoolConfigEntryData;

  char snmp_buffer[SNMP_BUFFER_LEN];

  ZERO_VALID(agentDhcpServerPoolConfigEntryData.valid);
  bzero(snmp_buffer, sizeof(snmp_buffer));
  agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex = agentDhcpServerPoolIndex;
  SET_VALID(I_agentDhcpServerPoolIndex, agentDhcpServerPoolConfigEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolName = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDefRouter = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDNSServer = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolNetbiosNameServer = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDomainName = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolBootfile = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientIdentifier = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientName = MakeOctetString(NULL, 0);
    agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientHWAddr = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if (snmpDhcpsPoolValidate(USMDB_UNIT_CURRENT,
                              agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex) != L7_SUCCESS)
      return(NULL);
  }

  else if (searchType == NEXT)
  {
    if (snmpDhcpsPoolValidate(USMDB_UNIT_CURRENT,
                              agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex) != L7_SUCCESS)
    {
      rc = snmpDhcpsPoolGetNext(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex);
      if (rc != L7_SUCCESS)
      {
        return(NULL);
      }
    }
  }


  switch (nominator)
  {
  case -1:
  case I_agentDhcpServerPoolIndex :
    if (nominator != -1) break;

  case I_agentDhcpServerPoolName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsPoolNameGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolName, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolDefRouter :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsPoolDefRouterGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDefRouter, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolDefRouter, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolDNSServer :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsPoolDnsServerGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDNSServer, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolDNSServer, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolLeaseTime :
    if (snmpAgentDhcpsPoolLeaseTimeGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                       &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolLeaseTime) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolLeaseTime, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolType :
    if (snmpAgentDhcpsPoolTypeGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                  &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolType) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolType, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolNetbiosNameServer :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsNetbiosNameServerAddressGet(USMDB_UNIT_CURRENT, 
                                                  agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, 
                                                  snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolNetbiosNameServer, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolNetbiosNameServer, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerPoolNetbiosNodeType :
    if (snmpAgentDhcpsNetbiosNodeTypeGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                         &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolNetbiosNodeType) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolNetbiosNodeType, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerPoolNextServer:
    if (snmpAgentDhcpsNextServerGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                    &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolNextServer) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolNextServer, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolDomainName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsDomainNameGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolDomainName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolDomainName, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolBootfile :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsBootfileGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolBootfile, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolBootfile, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerPoolRowStatus :
    if (snmpAgentDhcpsPoolRowStatusGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                       &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolRowStatus) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolRowStatus, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolAllocationName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpsPoolNameGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolAllocationName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolAllocationName, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerDynamicPoolIpAddress:
    if (snmpAgentDhcpsServerDynamicPoolIpAddressGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                    &agentDhcpServerPoolConfigEntryData.agentDhcpServerDynamicPoolIpAddress) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerDynamicPoolIpAddress, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerDynamicPoolIpMask:
    if (snmpAgentDhcpsServerDynamicPoolIpMaskGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                 &agentDhcpServerPoolConfigEntryData.agentDhcpServerDynamicPoolIpMask) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerDynamicPoolIpMask, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerDynamicPoolIpPrefixLength:
    if (snmpAgentDhcpsServerDynamicPoolIpPrefixGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                   &agentDhcpServerPoolConfigEntryData.agentDhcpServerDynamicPoolIpPrefixLength) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerDynamicPoolIpPrefixLength, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolAllocationType:
    if (snmpAgentDhcpsPoolTypeGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                  &agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolAllocationType) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerPoolAllocationType, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerManualPoolClientIdentifier:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpServerManualPoolClientIdentifierGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientIdentifier, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerManualPoolClientIdentifier, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerManualPoolClientName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpServerManualPoolClientNameGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerManualPoolClientName, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerManualPoolClientHWAddr:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentDhcpServerManualPoolClientHWAddrGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientHWAddr, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentDhcpServerManualPoolClientHWAddr, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerManualPoolClientHWType:
    if (snmpAgentDhcpServerManualPoolClientHWTypeGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                     &agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolClientHWType) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerManualPoolClientHWType, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerManualPoolIpAddress:
    if (snmpAgentDhcpServerManualPoolIpAddressGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                  &agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolIpAddress) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerManualPoolIpAddress, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerManualPoolIpMask:
    if (snmpAgentDhcpServerManualPoolIpMaskGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                               &agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolIpMask) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerManualPoolIpMask, agentDhcpServerPoolConfigEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerManualPoolIpPrefixLength:
    if (snmpAgentDhcpServerManualPoolIpPrefixLengthGet(USMDB_UNIT_CURRENT, agentDhcpServerPoolConfigEntryData.agentDhcpServerPoolIndex,
                                                       &agentDhcpServerPoolConfigEntryData.agentDhcpServerManualPoolIpPrefixLength) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerManualPoolIpPrefixLength, agentDhcpServerPoolConfigEntryData.valid);
    break;


  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentDhcpServerPoolConfigEntryData.valid))
    return(NULL);

  return(&agentDhcpServerPoolConfigEntryData);
}

#ifdef SETS
int
k_agentDhcpServerPoolConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerPoolConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerPoolConfigEntry_set_defaults(doList_t *dp)
{
  agentDhcpServerPoolConfigEntry_t *data = (agentDhcpServerPoolConfigEntry_t *) (dp->data);

  if ((data->agentDhcpServerPoolName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDhcpServerPoolDefRouter = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDhcpServerPoolDNSServer = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpServerPoolConfigEntry_set(agentDhcpServerPoolConfigEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{

  char snmp_buffer[SNMP_BUFFER_LEN];


  /* If entry doesn't exist, then throw error */
  if (snmpDhcpsPoolValidate(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex) != L7_SUCCESS)
  {
    return(NO_CREATION_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolDefRouter, data->valid) && (data->agentDhcpServerPoolDefRouter != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolDefRouter->octet_ptr, data->agentDhcpServerPoolDefRouter->length);
    if (snmpAgentDhcpsPoolDefRouterSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                       snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolDNSServer, data->valid) && (data->agentDhcpServerPoolDNSServer != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolDNSServer->octet_ptr, data->agentDhcpServerPoolDNSServer->length);
    if (snmpAgentDhcpsPoolDnsServerSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                       snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerPoolLeaseTime, data->valid))
  {
    if (snmpAgentDhcpsPoolLeaseTimeSet(USMDB_UNIT_CURRENT,
                                       data->agentDhcpServerPoolIndex,
                                       data->agentDhcpServerPoolLeaseTime) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolNetbiosNameServer, data->valid) && (data->agentDhcpServerPoolNetbiosNameServer != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolNetbiosNameServer->octet_ptr, data->agentDhcpServerPoolNetbiosNameServer->length);
    if (snmpAgentDhcpsNetbiosNameServerAddressSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                                  snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerPoolNetbiosNodeType, data->valid))
  {
    if (snmpAgentDhcpsNetbiosNodeTypeSet(USMDB_UNIT_CURRENT,
                                         data->agentDhcpServerPoolIndex,
                                         data->agentDhcpServerPoolNetbiosNodeType) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolNextServer, data->valid) &&
      (snmpAgentDhcpsNextServerSet(USMDB_UNIT_CURRENT,
                                   data->agentDhcpServerPoolIndex,
                                   data->agentDhcpServerPoolNextServer) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);


  if (VALID(I_agentDhcpServerPoolDomainName, data->valid) && (data->agentDhcpServerPoolDomainName != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolDomainName->octet_ptr, 
           data->agentDhcpServerPoolDomainName->length);
    if (snmpAgentDhcpsDomainNameSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                    snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerPoolBootfile, data->valid) && (data->agentDhcpServerPoolBootfile != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolBootfile->octet_ptr, 
           data->agentDhcpServerPoolBootfile->length);
    if (snmpAgentDhcpsBootfileSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                  snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }




  if (VALID(I_agentDhcpServerPoolRowStatus, data->valid))
  {
    if (snmpAgentDhcpsPoolRowStatusSet(USMDB_UNIT_CURRENT,
                                       data->agentDhcpServerPoolIndex,
                                       data->agentDhcpServerPoolRowStatus) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerDynamicPoolIpAddress, data->valid) &&
      (snmpAgentDhcpsServerDynamicPoolIpAddressSet(USMDB_UNIT_CURRENT,
                                                   data->agentDhcpServerPoolIndex,
                                                   data->agentDhcpServerDynamicPoolIpAddress) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);


  if (VALID(I_agentDhcpServerDynamicPoolIpMask, data->valid) &&
      (snmpAgentDhcpsServerDynamicPoolIpMaskSet(USMDB_UNIT_CURRENT,
                                                data->agentDhcpServerPoolIndex,
                                                data->agentDhcpServerDynamicPoolIpMask) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentDhcpServerDynamicPoolIpPrefixLength, data->valid) &&
      (snmpAgentDhcpsServerDynamicPoolIpPrefixSet(USMDB_UNIT_CURRENT,
                                                  data->agentDhcpServerPoolIndex,
                                                  data->agentDhcpServerDynamicPoolIpPrefixLength) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentDhcpServerManualPoolClientIdentifier, data->valid) && (data->agentDhcpServerManualPoolClientIdentifier != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerManualPoolClientIdentifier->octet_ptr, data->agentDhcpServerManualPoolClientIdentifier->length);
    if (snmpAgentDhcpServerManualPoolClientIdentifierSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                                         snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerManualPoolClientName, data->valid) && (data->agentDhcpServerManualPoolClientName != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerManualPoolClientName->octet_ptr, data->agentDhcpServerManualPoolClientName->length);
    if (snmpAgentDhcpServerManualPoolClientNameSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                                   snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerManualPoolClientHWAddr, data->valid) && (data->agentDhcpServerManualPoolClientHWAddr != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerManualPoolClientHWAddr->octet_ptr, data->agentDhcpServerManualPoolClientHWAddr->length);
    if (snmpAgentDhcpServerManualPoolClientHWAddrSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolIndex,
                                                     snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerManualPoolClientHWType, data->valid) &&
      (snmpAgentDhcpServerManualPoolClientHWTypeSet(USMDB_UNIT_CURRENT,
                                                    data->agentDhcpServerPoolIndex,
                                                    data->agentDhcpServerManualPoolClientHWType) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentDhcpServerManualPoolIpAddress, data->valid) &&
      (snmpAgentDhcpServerManualPoolIpAddressSet(USMDB_UNIT_CURRENT,
                                                 data->agentDhcpServerPoolIndex,
                                                 data->agentDhcpServerManualPoolIpAddress) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);


  if (VALID(I_agentDhcpServerManualPoolIpMask, data->valid) &&
      (snmpAgentDhcpServerManualPoolIpMaskSet(USMDB_UNIT_CURRENT,
                                              data->agentDhcpServerPoolIndex,
                                              data->agentDhcpServerManualPoolIpMask) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentDhcpServerManualPoolIpPrefixLength, data->valid) &&
      (snmpAgentDhcpServerManualPoolIpPrefixLengthSet(USMDB_UNIT_CURRENT,
                                                      data->agentDhcpServerPoolIndex,
                                                      data->agentDhcpServerManualPoolIpPrefixLength) != L7_SUCCESS))
    return(COMMIT_FAILED_ERROR);

  return(NO_ERROR);

}

  #ifdef SR_agentDhcpServerPoolConfigEntry_UNDO
/* add #define SR_agentDhcpServerPoolConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerPoolConfigEntry family.
 */
int
agentDhcpServerPoolConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerPoolConfigEntry_UNDO */

#endif /* SETS */

agentDhcpServerPoolConfigGroup_t *
k_agentDhcpServerPoolConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{

  static agentDhcpServerPoolConfigGroup_t agentDhcpServerPoolConfigGroupData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    agentDhcpServerPoolConfigGroupData.agentDhcpServerPoolNameCreate = MakeOctetString("", 0);
    firstTime = L7_FALSE;
  }

  switch (nominator)
  {
  case -1:
    /* pass through */

  case I_agentDhcpServerPoolNameCreate:
    /* returns a blank string as read has no meaning */
    SET_VALID(I_agentDhcpServerPoolNameCreate, agentDhcpServerPoolConfigGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerExcludedAddressRangeCreate:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentDhcpsPoolExcludedAddressRangeCreateGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigGroupData.agentDhcpServerExcludedAddressRangeCreate, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerExcludedAddressRangeCreate, agentDhcpServerPoolConfigGroupData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolOptionCreate:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentDhcpsPoolOptionCreateGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetStringFromText(&agentDhcpServerPoolConfigGroupData.agentDhcpServerPoolOptionCreate, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentDhcpServerPoolOptionCreate, agentDhcpServerPoolConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator >= 0 && !VALID(nominator, agentDhcpServerPoolConfigGroupData.valid))
    return(NULL);
  return(&agentDhcpServerPoolConfigGroupData);

}

#ifdef SETS
int
k_agentDhcpServerPoolConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerPoolConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerPoolConfigGroup_set(agentDhcpServerPoolConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{

  char snmp_buffer[SNMP_BUFFER_LEN];

  if (VALID(I_agentDhcpServerPoolNameCreate, data->valid) && (data->agentDhcpServerPoolNameCreate != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolNameCreate->octet_ptr, data->agentDhcpServerPoolNameCreate->length);
    if (snmpDhcpsPoolCreate(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerExcludedAddressRangeCreate, data->valid) && (data->agentDhcpServerExcludedAddressRangeCreate!= NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerExcludedAddressRangeCreate->octet_ptr, data->agentDhcpServerExcludedAddressRangeCreate->length);
    if (snmpAgentDhcpsPoolExcludedAddressRangeCreateSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolOptionCreate, data->valid) && (data->agentDhcpServerPoolOptionCreate!= NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDhcpServerPoolOptionCreate->octet_ptr, data->agentDhcpServerPoolOptionCreate->length);
    if (snmpAgentDhcpsPoolOptionCreateSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  return NO_ERROR;
}

  #ifdef SR_agentDhcpServerPoolConfigGroup_UNDO
/* add #define SR_agentDhcpServerPoolConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerPoolConfigGroup family.
 */
int
agentDhcpServerPoolConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerPoolConfigGroup_UNDO */

#endif /* SETS */


agentDhcpServerExcludedAddressRangeEntry_t *
k_agentDhcpServerExcludedAddressRangeEntry_get(int serialNum, ContextInfo *contextInfo,
                                               int nominator,
                                               int searchType,
                                               SR_UINT32 agentDhcpServerExcludedRangeIndex)
{

  static agentDhcpServerExcludedAddressRangeEntry_t agentDhcpServerExcludedAddressRangeEntryData;

  ZERO_VALID(agentDhcpServerExcludedAddressRangeEntryData.valid);

  agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedRangeIndex
  = agentDhcpServerExcludedRangeIndex;
  SET_VALID(I_agentDhcpServerExcludedRangeIndex, agentDhcpServerExcludedAddressRangeEntryData.valid);

  if (usmDbDhcpsSNMPExcludeAddressRangeGet(USMDB_UNIT_CURRENT,
                                           agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedRangeIndex,
                                           &agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedStartIpAddress,
                                           &agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedEndIpAddress) != L7_SUCCESS)
  {
    if (searchType != EXACT && usmDbDhcpsSNMPExcludeAddressRangeGetNext(USMDB_UNIT_CURRENT,
                                                                        agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedRangeIndex,
                                                                        &agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedRangeIndex) == L7_SUCCESS)
    {
      if (usmDbDhcpsSNMPExcludeAddressRangeGet(USMDB_UNIT_CURRENT,
                                               agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedRangeIndex,
                                               &agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedStartIpAddress,
                                               &agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedEndIpAddress) != L7_SUCCESS)
      {
        return(NULL);
      }
    }
    else
    {
      return(NULL);
    }
  }

  switch (nominator)
  {
  
  case -1:
  case I_agentDhcpServerExcludedRangeIndex:
    if (nominator != -1) break;

  case I_agentDhcpServerExcludedStartIpAddress:
    SET_VALID(I_agentDhcpServerExcludedStartIpAddress, agentDhcpServerExcludedAddressRangeEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerExcludedEndIpAddress:
    SET_VALID(I_agentDhcpServerExcludedEndIpAddress, agentDhcpServerExcludedAddressRangeEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerExcludedAddressRangeStatus:
    agentDhcpServerExcludedAddressRangeEntryData.agentDhcpServerExcludedAddressRangeStatus =
    D_agentDhcpServerExcludedAddressRangeStatus_active;
    SET_VALID(I_agentDhcpServerExcludedAddressRangeStatus, agentDhcpServerExcludedAddressRangeEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }  /* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, agentDhcpServerExcludedAddressRangeEntryData.valid))
    return(NULL);

  return(&agentDhcpServerExcludedAddressRangeEntryData);
}

#ifdef SETS
int
k_agentDhcpServerExcludedAddressRangeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerExcludedAddressRangeEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerExcludedAddressRangeEntry_set_defaults(doList_t *dp)
{
  agentDhcpServerExcludedAddressRangeEntry_t *data = (agentDhcpServerExcludedAddressRangeEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpServerExcludedAddressRangeEntry_set(agentDhcpServerExcludedAddressRangeEntry_t *data,
                                               ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentDhcpServerExcludedAddressRangeStatus, data->valid) &&
      (data->agentDhcpServerExcludedAddressRangeStatus ==
       D_agentDhcpServerExcludedAddressRangeStatus_destroy))
  {
    /* Delete this excluded IP address range */
    if (snmpDhcpsExcludedAddressRangeDelete(USMDB_UNIT_CURRENT,
                                            data->agentDhcpServerExcludedRangeIndex) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  else
    return(COMMIT_FAILED_ERROR);


  return(NO_ERROR);
}

  #ifdef SR_agentDhcpServerExcludedAddressRangeEntry_UNDO
/* add #define SR_agentDhcpServerExcludedAddressRangeEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerExcludedAddressRangeEntry family.
 */
int
agentDhcpServerExcludedAddressRangeEntry_undo(doList_t *doHead, doList_t *doCur,
                                              ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerExcludedAddressRangeEntry_UNDO */

#endif /* SETS */

agentDhcpServerPoolOptionEntry_t *
k_agentDhcpServerPoolOptionEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_UINT32 agentDhcpServerPoolOptionIndex,
                                     SR_UINT32 agentDhcpServerPoolOptionCode)
{

  static L7_BOOL firstTime = L7_TRUE;
  L7_RC_t rc = L7_FAILURE;
  static agentDhcpServerPoolOptionEntry_t agentDhcpServerPoolOptionEntryData;

  char snmp_buffer[SNMP_BUFFER_LEN];

  ZERO_VALID(agentDhcpServerPoolOptionEntryData.valid);

  bzero(snmp_buffer, sizeof(snmp_buffer));
  agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIndex = agentDhcpServerPoolOptionIndex;
  agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionCode =  agentDhcpServerPoolOptionCode;
  SET_VALID(I_agentDhcpServerPoolOptionIndex, agentDhcpServerPoolOptionEntryData.valid);
  SET_VALID(I_agentDhcpServerPoolOptionCode, agentDhcpServerPoolOptionEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    agentDhcpServerPoolOptionEntryData.agentDhcpServerOptionPoolName = MakeOctetString(NULL, 0);
    agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionAsciiData = MakeOctetString(NULL, 0);
    agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionHexData = MakeOctetString(NULL, 0);
    agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIpAddressData = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }



  /* get ENTRY data */
  rc = snmpAgentDhcpsPoolOptionEntryGet(USMDB_UNIT_CURRENT, 
                                        agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIndex,
                                        agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionCode,
                                        &agentDhcpServerPoolOptionEntryData);

  if (rc != L7_SUCCESS)
  {
    /* GET-NEXT? */
    if (searchType == NEXT)
    {
      if (snmpAgentDhcpsPoolOptionEntryGetNext(USMDB_UNIT_CURRENT, 
                                               agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIndex,
                                               agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionCode,
                                               &agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIndex,
                                               &agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionCode) == L7_SUCCESS)
      {
        rc = snmpAgentDhcpsPoolOptionEntryGet(USMDB_UNIT_CURRENT,
                                              agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionIndex,
                                              agentDhcpServerPoolOptionEntryData.agentDhcpServerPoolOptionCode,
                                              &agentDhcpServerPoolOptionEntryData);
      }
    }

    if (rc != L7_SUCCESS)
    {
      ZERO_VALID(agentDhcpServerPoolOptionEntryData.valid);
      return(NULL);
    }
  }



  switch (nominator)
  {
  case -1:
  case I_agentDhcpServerPoolOptionIndex :
    if (nominator != -1) break;

  case I_agentDhcpServerPoolOptionCode :
    if (nominator != -1) break;

  case I_agentDhcpServerOptionPoolName :
    SET_VALID(I_agentDhcpServerOptionPoolName, agentDhcpServerPoolOptionEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerPoolOptionAsciiData :
    SET_VALID(I_agentDhcpServerPoolOptionAsciiData, agentDhcpServerPoolOptionEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolOptionHexData :

    SET_VALID(I_agentDhcpServerPoolOptionHexData, agentDhcpServerPoolOptionEntryData.valid);
    if (nominator != -1) break;


  case I_agentDhcpServerPoolOptionIpAddressData :
    SET_VALID(I_agentDhcpServerPoolOptionIpAddressData, agentDhcpServerPoolOptionEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerPoolOptionStatus :
    SET_VALID(I_agentDhcpServerPoolOptionStatus, agentDhcpServerPoolOptionEntryData.valid);
    break;

  default:
    return(NULL);
  } 
  return(&agentDhcpServerPoolOptionEntryData);
}

#ifdef SETS
int
k_agentDhcpServerPoolOptionEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerPoolOptionEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerPoolOptionEntry_set_defaults(doList_t *dp)
{
  agentDhcpServerPoolOptionEntry_t *data = (agentDhcpServerPoolOptionEntry_t *) (dp->data);

  if ((data->agentDhcpServerOptionPoolName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDhcpServerPoolOptionAsciiData = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDhcpServerPoolOptionHexData = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDhcpServerPoolOptionIpAddressData = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpServerPoolOptionEntry_set(agentDhcpServerPoolOptionEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{

  char snmp_buffer[4*L7_DHCPS_POOL_OPTION_LENGTH];

  if (VALID(I_agentDhcpServerPoolOptionAsciiData, data->valid) && (data->agentDhcpServerPoolOptionAsciiData != NULL))
  {
    bzero(snmp_buffer, sizeof(snmp_buffer));
    memcpy(snmp_buffer, data->agentDhcpServerPoolOptionAsciiData->octet_ptr, data->agentDhcpServerPoolOptionAsciiData->length);
    if (snmpAgentDhcpsPoolOptionAsciiDataSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolOptionIndex,
                                             data->agentDhcpServerPoolOptionCode,
                                             snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolOptionHexData, data->valid) && (data->agentDhcpServerPoolOptionHexData != NULL))
  {
    bzero(snmp_buffer, sizeof(snmp_buffer));
    memcpy(snmp_buffer, data->agentDhcpServerPoolOptionHexData->octet_ptr, data->agentDhcpServerPoolOptionHexData->length);
    if (snmpAgentDhcpsPoolOptionHexDataSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolOptionIndex,
                                           data->agentDhcpServerPoolOptionCode,
                                           snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentDhcpServerPoolOptionIpAddressData, data->valid) && (data->agentDhcpServerPoolOptionIpAddressData != NULL))
  {
    bzero(snmp_buffer, sizeof(snmp_buffer));
    memcpy(snmp_buffer, data->agentDhcpServerPoolOptionIpAddressData->octet_ptr, data->agentDhcpServerPoolOptionIpAddressData->length);
    if (snmpAgentDhcpsPoolOptionIPDataSet(USMDB_UNIT_CURRENT, data->agentDhcpServerPoolOptionIndex,
                                          data->agentDhcpServerPoolOptionCode,
                                          snmp_buffer) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }


  if (VALID(I_agentDhcpServerPoolOptionStatus, data->valid))
  {
    if (snmpAgentDhcpsPoolOptionRowStatusSet(USMDB_UNIT_CURRENT,
                                             data->agentDhcpServerPoolOptionIndex,
                                             data->agentDhcpServerPoolOptionCode,
                                             data->agentDhcpServerPoolOptionStatus) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);

}

  #ifdef SR_agentDhcpServerPoolOptionEntry_UNDO
/* add #define SR_agentDhcpServerPoolOptionEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerPoolOptionEntry family.
 */
int
agentDhcpServerPoolOptionEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerPoolOptionEntry_UNDO */

#endif /* SETS */


agentDhcpServerLeaseGroup_t *
k_agentDhcpServerLeaseGroup_get(int serialNum, ContextInfo *contextInfo,
                                int nominator)
{
  static agentDhcpServerLeaseGroup_t agentDhcpServerLeaseGroupData;

  switch (nominator)
  {
  
  case -1:
    /* pass through */

  case I_agentDhcpServerLeaseClearAllBindings:
    agentDhcpServerLeaseGroupData.agentDhcpServerLeaseClearAllBindings = D_agentDhcpServerLeaseClearAllBindings_disable;
    SET_VALID(I_agentDhcpServerLeaseClearAllBindings, agentDhcpServerLeaseGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }  /* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, agentDhcpServerLeaseGroupData.valid))
    return(NULL);

  return(&agentDhcpServerLeaseGroupData);
}

#ifdef SETS
int
k_agentDhcpServerLeaseGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerLeaseGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerLeaseGroup_set(agentDhcpServerLeaseGroup_t *data,
                                ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentDhcpServerLeaseClearAllBindings, data->valid) &&
      snmpAgentDhcpsBindingClearAll(USMDB_UNIT_CURRENT, data->agentDhcpServerLeaseClearAllBindings) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerLeaseClearAllBindings, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

  #ifdef SR_agentDhcpServerLeaseGroup_UNDO
/* add #define SR_agentDhcpServerLeaseGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerLeaseGroup family.
 */
int
agentDhcpServerLeaseGroup_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerLeaseGroup_UNDO */

#endif /* SETS */


agentDhcpServerLeaseEntry_t *
k_agentDhcpServerLeaseEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_UINT32 agentDhcpServerLeaseIPAddress)
{

  L7_RC_t                            rc = L7_FAILURE;
  char                               snmp_buffer[SNMP_BUFFER_LEN];
  static agentDhcpServerLeaseEntry_t agentDhcpServerLeaseEntryData;
  static L7_BOOL                     firstTime = L7_TRUE;


  bzero(snmp_buffer, sizeof(snmp_buffer));

  if (L7_TRUE == firstTime)
  {
    agentDhcpServerLeaseEntryData.agentDhcpServerLeaseHWAddress = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  /* set index fld */
  agentDhcpServerLeaseEntryData.agentDhcpServerLeaseIPAddress = agentDhcpServerLeaseIPAddress;

  /* invalidate all! */
  ZERO_VALID(agentDhcpServerLeaseEntryData.valid);


  /* get ENTRY data */
  rc = snmpAgentDhcpsServerLeaseEntryGet(USMDB_UNIT_CURRENT, 
                                         agentDhcpServerLeaseEntryData.agentDhcpServerLeaseIPAddress,
                                         &agentDhcpServerLeaseEntryData);

  if (rc != L7_SUCCESS)
  {
    /* GET-NEXT? */
    if (searchType == NEXT)
    {
      if (snmpDhcpsServerLeaseGetNext(USMDB_UNIT_CURRENT, 
                                      agentDhcpServerLeaseEntryData.agentDhcpServerLeaseIPAddress,
                                      &agentDhcpServerLeaseEntryData.agentDhcpServerLeaseIPAddress) == L7_SUCCESS)
      {
        rc = snmpAgentDhcpsServerLeaseEntryGet(USMDB_UNIT_CURRENT,
                                               agentDhcpServerLeaseEntryData.agentDhcpServerLeaseIPAddress,
                                               &agentDhcpServerLeaseEntryData);
      }
    }

    if (rc != L7_SUCCESS)
    {
      return(NULL);
    }
  }

  switch (nominator)
  {
  case -1:
   /* pass through */

  case I_agentDhcpServerLeaseIPAddress:
    SET_VALID(I_agentDhcpServerLeaseIPAddress, 
              agentDhcpServerLeaseEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerLeaseIPMask:
    SET_VALID(I_agentDhcpServerLeaseIPMask, 
              agentDhcpServerLeaseEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerLeaseHWAddress:
    SET_VALID(I_agentDhcpServerLeaseHWAddress, 
              agentDhcpServerLeaseEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerLeaseRemainingTime:
    SET_VALID(I_agentDhcpServerLeaseRemainingTime, 
              agentDhcpServerLeaseEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerLeaseType:
    SET_VALID(I_agentDhcpServerLeaseType, 
              agentDhcpServerLeaseEntryData.valid);
    if (nominator != -1) break;

  case I_agentDhcpServerLeaseStatus:
    agentDhcpServerLeaseEntryData.agentDhcpServerLeaseStatus = D_agentDhcpServerLeaseStatus_active;

    SET_VALID(I_agentDhcpServerLeaseStatus, 
              agentDhcpServerLeaseEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  return(&agentDhcpServerLeaseEntryData);
}

#ifdef SETS
int
k_agentDhcpServerLeaseEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerLeaseEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerLeaseEntry_set_defaults(doList_t *dp)
{
  agentDhcpServerLeaseEntry_t *data = (agentDhcpServerLeaseEntry_t *) (dp->data);

  if ((data->agentDhcpServerLeaseHWAddress = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpServerLeaseEntry_set(agentDhcpServerLeaseEntry_t *data,
                                ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentDhcpServerLeaseStatus, data->valid) &&
      (data->agentDhcpServerLeaseStatus ==
       D_agentDhcpServerLeaseStatus_destroy))
  {
    /* Clear this IP address lease */
    if (snmpDhcpsServerLeaseDelete(USMDB_UNIT_CURRENT,
                                   data->agentDhcpServerLeaseIPAddress) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  else
    return(COMMIT_FAILED_ERROR);


  return(NO_ERROR);
}

  #ifdef SR_agentDhcpServerLeaseEntry_UNDO
/* add #define SR_agentDhcpServerLeaseEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerLeaseEntry family.
 */
int
agentDhcpServerLeaseEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerLeaseEntry_UNDO */

#endif /* SETS */

agentDhcpServerAddressConflictGroup_t *
k_agentDhcpServerAddressConflictGroup_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator)
{

  static agentDhcpServerAddressConflictGroup_t agentDhcpServerAddressConflictGroupData;

  switch (nominator)
  {
  
  case -1:
    break;


  case I_agentDhcpServerClearAllAddressConflicts:
    agentDhcpServerAddressConflictGroupData.agentDhcpServerClearAllAddressConflicts = D_agentDhcpServerClearAllAddressConflicts_disable;
    SET_VALID(I_agentDhcpServerClearAllAddressConflicts, agentDhcpServerAddressConflictGroupData.valid);
    break;


  case I_agentDhcpServerAddressConflictLogging:
    if (snmpAgentDhcpsConflictLoggingGet(USMDB_UNIT_CURRENT, &agentDhcpServerAddressConflictGroupData.agentDhcpServerAddressConflictLogging) == L7_SUCCESS)
      SET_VALID(I_agentDhcpServerAddressConflictLogging, agentDhcpServerAddressConflictGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }  /* end of switch(nominator)  */

  if (nominator >= 0 && !VALID(nominator, agentDhcpServerAddressConflictGroupData.valid))
    return(NULL);

  return(&agentDhcpServerAddressConflictGroupData);

}

#ifdef SETS
int
k_agentDhcpServerAddressConflictGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerAddressConflictGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerAddressConflictGroup_set(agentDhcpServerAddressConflictGroup_t *data,
                                          ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentDhcpServerClearAllAddressConflicts, data->valid) &&
      snmpAgentDhcpsAddressConflictClearAll(USMDB_UNIT_CURRENT, data->agentDhcpServerClearAllAddressConflicts) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerClearAllAddressConflicts, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpServerAddressConflictLogging, data->valid) &&
      snmpAgentDhcpsConflictLoggingSet(USMDB_UNIT_CURRENT, data->agentDhcpServerAddressConflictLogging) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpServerAddressConflictLogging, data->valid);
    return COMMIT_FAILED_ERROR;
  }


  return NO_ERROR;
}

  #ifdef SR_agentDhcpServerAddressConflictGroup_UNDO
/* add #define SR_agentDhcpServerAddressConflictGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerAddressConflictGroup family.
 */
int
agentDhcpServerAddressConflictGroup_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerAddressConflictGroup_UNDO */

#endif /* SETS */

agentDhcpServerAddressConflictEntry_t *
k_agentDhcpServerAddressConflictEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_UINT32 agentDhcpServerAddressConflictIP)
{

  L7_RC_t                            rc = L7_FAILURE;
  static agentDhcpServerAddressConflictEntry_t agentDhcpServerAddressConflictEntryData;

  /* invalidate all! */
  ZERO_VALID(agentDhcpServerAddressConflictEntryData.valid);

  /* set index field */
  agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictIP = agentDhcpServerAddressConflictIP;
  SET_VALID(I_agentDhcpServerAddressConflictIP,agentDhcpServerAddressConflictEntryData.valid);

  /* get ENTRY data */
  rc = snmpAgentDhcpsServerConflictLoggingEntryGet(USMDB_UNIT_CURRENT, 
                                                   agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictIP,
                                                   &agentDhcpServerAddressConflictEntryData);

  if (rc != L7_SUCCESS)
  {
    /* Implementing GET-NEXT function */
    if (searchType == NEXT)
    {
      if (snmpDhcpsServerConflictLoggingGetNext(USMDB_UNIT_CURRENT, 
                                                agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictIP,
                                                &agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictIP) == L7_SUCCESS)
      {
        rc = snmpAgentDhcpsServerConflictLoggingEntryGet(USMDB_UNIT_CURRENT,
                                                         agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictIP,
                                                         &agentDhcpServerAddressConflictEntryData);
      }
    }

    if (rc != L7_SUCCESS)
    {
      ZERO_VALID(agentDhcpServerAddressConflictEntryData.valid);
      return(NULL);
    }
  }

  switch (nominator)
  {
  case -1: /* for set */
    break;


  case I_agentDhcpServerAddressConflictIP:
    break;


  case I_agentDhcpServerAddressConflictDetectionType:
    SET_VALID(I_agentDhcpServerAddressConflictDetectionType, 
              agentDhcpServerAddressConflictEntryData.valid);
    break;


  case I_agentDhcpServerAddressConflictDetectionTime:
    SET_VALID(I_agentDhcpServerAddressConflictDetectionTime, 
              agentDhcpServerAddressConflictEntryData.valid);
    break;


  case I_agentDhcpServerAddressConflictStatus:
    agentDhcpServerAddressConflictEntryData.agentDhcpServerAddressConflictStatus 
    = D_agentDhcpServerAddressConflictStatus_active; 
    SET_VALID(I_agentDhcpServerAddressConflictStatus, 
              agentDhcpServerAddressConflictEntryData.valid);
    break;

  default:
    return(NULL);
  }

  return(&agentDhcpServerAddressConflictEntryData);
}

#ifdef SETS
int
k_agentDhcpServerAddressConflictEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDhcpServerAddressConflictEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpServerAddressConflictEntry_set_defaults(doList_t *dp)
{
  agentDhcpServerAddressConflictEntry_t *data = (agentDhcpServerAddressConflictEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpServerAddressConflictEntry_set(agentDhcpServerAddressConflictEntry_t *data,
                                          ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentDhcpServerAddressConflictStatus, data->valid) &&
      (data->agentDhcpServerAddressConflictStatus ==
       D_agentDhcpServerAddressConflictStatus_destroy))
  {
    /* Clear this conflicting address */
    if (snmpDhcpsServerAddressConflictDelete(USMDB_UNIT_CURRENT,
                                             data->agentDhcpServerAddressConflictIP) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  else
    return(COMMIT_FAILED_ERROR);


  return(NO_ERROR);

}

  #ifdef SR_agentDhcpServerAddressConflictEntry_UNDO
/* add #define SR_agentDhcpServerAddressConflictEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpServerAddressConflictEntry family.
 */
int
agentDhcpServerAddressConflictEntry_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDhcpServerAddressConflictEntry_UNDO */

#endif /* SETS */

#endif
