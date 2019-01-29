/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\packages\base\k_mib_fastpath_dhcpclient.c
*
* @purpose  Provide interface to DHCP Client Private MIB
*
* @component SNMP
*
* @comments
*
* @create 8/1/2010
*
* @author ashadeep
*
* @end
*
**********************************************************************/
/* The DHCP Client Lease parameters support exists only for Routing Interfaces
 * as on today.  If the same needs to be extended for Network and Service
 * ports, Remove the '#ifdef L7_ROUTING_PACKAGE' below and extend the MIB.
 */
#include <k_private_base.h>

#ifdef L7_ROUTING_PACKAGE
#include "k_mib_fastpath_dhcpclient_api.h"
#include "usmdb_common.h"
#endif /* L7_ROUTING_PACKAGE */

agentdhcp4ClientLeaseParametersEntry_t *
k_agentdhcp4ClientLeaseParametersEntry_get(int serialNum, ContextInfo *contextInfo,
                                           int nominator,
                                           int searchType,
                                           SR_INT32 agentdhcp4ClientInterfaceIndex)
{
#ifdef L7_ROUTING_PACKAGE
  static agentdhcp4ClientLeaseParametersEntry_t agentdhcp4ClientLeaseParametersEntryData;
  L7_uint32 intIfIndex;
  L7_uchar8 snmp_buf[SNMP_BUFFER_LEN];

  ZERO_VALID(agentdhcp4ClientLeaseParametersEntryData.valid);
  agentdhcp4ClientLeaseParametersEntryData.agentdhcp4ClientInterfaceIndex = agentdhcp4ClientInterfaceIndex;
  SET_VALID(I_agentdhcp4ClientInterfaceIndex, agentdhcp4ClientLeaseParametersEntryData.valid);

  if((searchType == EXACT ?
     (snmpAgentDhcpClientInterfaceGet (USMDB_UNIT_CURRENT, agentdhcp4ClientLeaseParametersEntryData.agentdhcp4ClientInterfaceIndex) != L7_SUCCESS) :
     ((snmpAgentDhcpClientInterfaceGet (USMDB_UNIT_CURRENT, agentdhcp4ClientLeaseParametersEntryData.agentdhcp4ClientInterfaceIndex) != L7_SUCCESS) &&
     (snmpAgentDhcpClientInterfaceNextGet (USMDB_UNIT_CURRENT, &agentdhcp4ClientLeaseParametersEntryData.agentdhcp4ClientInterfaceIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentdhcp4ClientLeaseParametersEntryData.agentdhcp4ClientInterfaceIndex, &intIfIndex) != L7_SUCCESS ))
  {
    ZERO_VALID(agentdhcp4ClientLeaseParametersEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentdhcp4ClientInterfaceIndex:
    break;

  case I_agentdhcp4ClientIpAddress:
    if (snmpAgentDhcpClientIPAddressGet(USMDB_UNIT_CURRENT, intIfIndex,
                                        &agentdhcp4ClientLeaseParametersEntryData.
                                        agentdhcp4ClientIpAddress) == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientIpAddress, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientSubnetMask:
    if (snmpAgentDhcpClientNetworkMaskGet(USMDB_UNIT_CURRENT, intIfIndex,
                                          &agentdhcp4ClientLeaseParametersEntryData.
                                          agentdhcp4ClientSubnetMask) == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientSubnetMask, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientDhcpServerAddress:
    if (snmpAgentDhcpClientDhcpServerIPAddressGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                  &agentdhcp4ClientLeaseParametersEntryData.
                                                  agentdhcp4ClientDhcpServerAddress)
                                                  == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientDhcpServerAddress, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientState:
    if (snmpAgentDhcpClientStateGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    &agentdhcp4ClientLeaseParametersEntryData.
                                    agentdhcp4ClientState) == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientState, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientTransactionID:
    if (snmpAgentDhcpClientTransactionIdGet(USMDB_UNIT_CURRENT, intIfIndex,
                                            snmp_buf) == L7_SUCCESS)
    {
      if (SafeMakeOctetStringFromText(&agentdhcp4ClientLeaseParametersEntryData.
                                      agentdhcp4ClientTransactionID, snmp_buf)
                                      == L7_TRUE)
      {
        SET_VALID(I_agentdhcp4ClientTransactionID, agentdhcp4ClientLeaseParametersEntryData.valid);
      }
    }
    break;

  case I_agentdhcp4ClientLeaseTime:
    if (snmpAgentDhcpClientLeaseTimeInfoGet(USMDB_UNIT_CURRENT, intIfIndex,
                                            &agentdhcp4ClientLeaseParametersEntryData.
                                            agentdhcp4ClientLeaseTime)
                                            == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientLeaseTime, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientRenewTime:
    if (snmpAgentDhcpClientRenewTimeInfoGet(USMDB_UNIT_CURRENT, intIfIndex,
                                            &agentdhcp4ClientLeaseParametersEntryData.
                                            agentdhcp4ClientRenewTime)
                                            == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientRenewTime, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientRebindTime:
    if (snmpAgentDhcpClientRebindTimeInfoGet(USMDB_UNIT_CURRENT, intIfIndex,
                                             &agentdhcp4ClientLeaseParametersEntryData.
                                             agentdhcp4ClientRebindTime)
                                             == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientRebindTime, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  case I_agentdhcp4ClientRetryCount:
    if (snmpAgentDhcpClientRetryCountGet(USMDB_UNIT_CURRENT, intIfIndex,
                                         &agentdhcp4ClientLeaseParametersEntryData.
                                         agentdhcp4ClientRetryCount)
                                         == L7_SUCCESS)
      SET_VALID(I_agentdhcp4ClientRetryCount, agentdhcp4ClientLeaseParametersEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentdhcp4ClientLeaseParametersEntryData.valid))
    return(NULL);
  return(&agentdhcp4ClientLeaseParametersEntryData);
#else
  return(NULL);
#endif /* L7_ROUTING_PACKAGE */
}

