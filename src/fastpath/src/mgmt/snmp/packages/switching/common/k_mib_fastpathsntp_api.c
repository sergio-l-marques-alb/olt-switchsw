/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathsntp_api.c
*
* @purpose    Wrapper functions for Fastpath SNTP MIB
*
* @component  SNMP
*
* @comments
*
* @create     2/24/2004
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpathsntp_api.h"
#include "usmdb_sntp_api.h"
#include "sntp_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

/****************************************************************************************/

L7_RC_t
snmpAgentSntpClientVersionGet( L7_uint32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbSntpClientVersionGet(USMDB_UNIT_CURRENT, &temp_val);

  switch (temp_val)
  {
  case 1:
    *val = D_agentSntpClientVersion_version1;
    break;
  case 2:
    *val = D_agentSntpClientVersion_version2;
    break;
  case 3:
    *val = D_agentSntpClientVersion_version3;
    break;
  case 4:
    *val = D_agentSntpClientVersion_version4;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientSupportedModeGet( L7_char8 *val )
{
  L7_RC_t rc;
  L7_SNTP_SUPPORTED_MODE_t temp_val;

  rc = usmDbSntpClientSupportedModeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    /* zero out bits in the bitmask */
    *val = (0x80 >> D_agentSntpClientSupportedMode_disabled);
  
    if (temp_val & L7_SNTP_SUPPORTED_UNICAST)
    {
      *val |= (0x80 >> D_agentSntpClientSupportedMode_unicast);
    }
  
    if (temp_val & L7_SNTP_SUPPORTED_BROADCAST)
    {
      *val |= (0x80 >> D_agentSntpClientSupportedMode_broadcast);
    }
  
    if (temp_val & L7_SNTP_SUPPORTED_MULTICAST)
    {
      *val |= (0x80 >> D_agentSntpClientSupportedMode_multicast);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientModeGet( L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_CLIENT_MODE_t temp_val;

  rc = usmDbSntpClientModeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNTP_CLIENT_DISABLED:
      *val = D_agentSntpClientMode_disabled;
      break;

    case L7_SNTP_CLIENT_UNICAST:
      *val = D_agentSntpClientMode_unicast;
      break;

    case L7_SNTP_CLIENT_BROADCAST:
      *val = D_agentSntpClientMode_broadcast;
      break;

    case L7_SNTP_CLIENT_MULTICAST:
      *val = D_agentSntpClientMode_multicast;
      break;

    default:
      rc = L7_FAILURE;
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentSntpClientModeTest( L7_uint32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
    case D_agentSntpClientMode_disabled:
      rc = L7_SUCCESS;
      break;

    case D_agentSntpClientMode_unicast:
      if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) == L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      break;

    case D_agentSntpClientMode_broadcast:
      if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) == L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      break;

    case D_agentSntpClientMode_multicast:
      if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_MULTICAST_FEATURE_ID) == L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      break;

    default:
      break;
  }

  return rc;
}


L7_RC_t
snmpAgentSntpClientModeSet( L7_uint32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_SNTP_CLIENT_MODE_t temp_val = 0;

  switch (val)
  {
  case D_agentSntpClientMode_disabled:
    temp_val = L7_SNTP_CLIENT_DISABLED;
    rc = L7_SUCCESS;
    break;

  case D_agentSntpClientMode_unicast:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_CLIENT_UNICAST;
      rc = L7_SUCCESS;
    }
    break;

  case D_agentSntpClientMode_broadcast:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_CLIENT_BROADCAST;
      rc = L7_SUCCESS;
    }
    break;

  case D_agentSntpClientMode_multicast:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_MULTICAST_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_CLIENT_MULTICAST;
      rc = L7_SUCCESS;
    }
    break;

  default:
    break;
  }


  if (rc == L7_SUCCESS)
  {
    rc = usmDbSntpClientModeSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}


L7_RC_t
snmpAgentSntpClientLastAttemptStatusGet( L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_PACKET_STATUS_t temp_val;

  rc = usmDbSntpLastAttemptStatusGet(USMDB_UNIT_CURRENT, &temp_val);

  switch (temp_val)
  {
  case L7_SNTP_STATUS_OTHER:
    *val = D_agentSntpClientLastAttemptStatus_other;
    break;
  case L7_SNTP_STATUS_SUCCESS:
    *val = D_agentSntpClientLastAttemptStatus_success;
    break;
  case L7_SNTP_STATUS_REQUEST_TIMED_OUT:
    *val = D_agentSntpClientLastAttemptStatus_requestTimedOut;
    break;
  case L7_SNTP_STATUS_BAD_DATE_ENCODED:
    *val = D_agentSntpClientLastAttemptStatus_badDateEncoded;
    break;
  case L7_SNTP_STATUS_VERSION_NOT_SUPPORTED:
    *val = D_agentSntpClientLastAttemptStatus_versionNotSupported;
    break;
  case L7_SNTP_STATUS_SERVER_UNSYNCHRONIZED:
    *val = D_agentSntpClientLastAttemptStatus_serverUnsychronized;
    break;
  case L7_SNTP_STATUS_KISS_OF_DEATH:
    *val = D_agentSntpClientLastAttemptStatus_serverKissOfDeath;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientServerAddressTypeGet( L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_ADDRESS_TYPE_t temp_val;

  rc = usmDbSntpServerAddressTypeGet(USMDB_UNIT_CURRENT, &temp_val);

  switch (temp_val)
  {
  case L7_SNTP_ADDRESS_UNKNOWN:
    *val = D_agentSntpClientServerAddressType_unknown;
    break;
  case L7_SNTP_ADDRESS_IPV4:
    *val = D_agentSntpClientServerAddressType_ipv4;
    break;
  case L7_SNTP_ADDRESS_IPV6:
    *val = D_agentSntpClientServerAddressType_ipv6;
    break;
  case L7_SNTP_ADDRESS_DNS:
    *val = D_agentSntpClientServerAddressType_dns;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/****************************************************************************************/

L7_RC_t
snmpAgentSntpClientUcastServerEntryGet( L7_uint32 agentSntpClientUcastServerIndex)
{
  L7_RC_t rc;
  L7_SNTP_SERVER_STATUS_t temp_val;

  rc = usmDbSntpServerTableRowStatusGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, &temp_val);

  if (rc == L7_SUCCESS &&
      (temp_val == L7_SNTP_SERVER_STATUS_INVALID ||
       temp_val == L7_SNTP_SERVER_STATUS_DESTROY))
    rc = L7_FAILURE;

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerEntryNextGet( L7_uint32 *agentSntpClientUcastServerIndex)
{
  L7_uint32 temp_val;
  L7_uint32 temp_index;

  if (usmDbSntpServerMaxEntriesGet(USMDB_UNIT_CURRENT, &temp_val) == L7_SUCCESS)
  {
    for (temp_index = *agentSntpClientUcastServerIndex + 1; temp_index <= temp_val; temp_index++)
    {
      if (snmpAgentSntpClientUcastServerEntryGet(temp_index) == L7_SUCCESS)
      {
        *agentSntpClientUcastServerIndex = temp_index;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentSntpClientUcastServerAddressTypeGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_ADDRESS_TYPE_t temp_val;

  rc = usmDbSntpServerTableAddressTypeGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNTP_ADDRESS_UNKNOWN:
      *val = D_agentSntpClientUcastServerAddressType_unknown;
      break;
    case L7_SNTP_ADDRESS_IPV4:
      *val = D_agentSntpClientUcastServerAddressType_ipv4;
      break;
    case L7_SNTP_ADDRESS_IPV6:
      *val = D_agentSntpClientUcastServerAddressType_ipv6;
      break;
    case L7_SNTP_ADDRESS_DNS:
      *val = D_agentSntpClientUcastServerAddressType_dns;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerAddressTypeSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_SNTP_ADDRESS_TYPE_t temp_val = 0;

  switch (val)
  {
  case D_agentSntpClientUcastServerAddressType_ipv4:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_PROTO_IPV4_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_ADDRESS_IPV4;
      rc = L7_SUCCESS;
    }
    break;
  case D_agentSntpClientUcastServerAddressType_ipv6:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_PROTO_IPV6_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_ADDRESS_IPV6;
      rc = L7_SUCCESS;
    }
    break;
  case D_agentSntpClientUcastServerAddressType_dns:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_PROTO_DNS_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_SNTP_ADDRESS_DNS;
      rc = L7_SUCCESS;
    }
    break;
  default:
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSntpServerTableAddressTypeSet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerVersionGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_ushort16 temp_val;

  rc = usmDbSntpServerTableVersionGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case 1:
      *val = D_agentSntpClientUcastServerVersion_version1;
      break;
    case 2:
      *val = D_agentSntpClientUcastServerVersion_version2;
      break;
    case 3:
      *val = D_agentSntpClientUcastServerVersion_version3;
      break;
    case 4:
      *val = D_agentSntpClientUcastServerVersion_version4;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerVersionSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 temp_val = 0;

  switch (val)
  {
  case D_agentSntpClientUcastServerVersion_version1:
    temp_val = 1;
    break;
  case D_agentSntpClientUcastServerVersion_version2:
    temp_val = 2;
    break;
  case D_agentSntpClientUcastServerVersion_version3:
    temp_val = 3;
    break;
  case D_agentSntpClientUcastServerVersion_version4:
    temp_val = 4;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSntpServerTableVersionSet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerLastAttemptStatusGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_PACKET_STATUS_t temp_val;

  rc = usmDbSntpServerStatsLastUpdateStatusGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, &temp_val);

  switch (temp_val)
  {
  case L7_SNTP_STATUS_OTHER:
    *val = D_agentSntpClientLastAttemptStatus_other;
    break;
  case L7_SNTP_STATUS_SUCCESS:
    *val = D_agentSntpClientLastAttemptStatus_success;
    break;
  case L7_SNTP_STATUS_REQUEST_TIMED_OUT:
    *val = D_agentSntpClientLastAttemptStatus_requestTimedOut;
    break;
  case L7_SNTP_STATUS_BAD_DATE_ENCODED:
    *val = D_agentSntpClientLastAttemptStatus_badDateEncoded;
    break;
  case L7_SNTP_STATUS_VERSION_NOT_SUPPORTED:
    *val = D_agentSntpClientLastAttemptStatus_versionNotSupported;
    break;
  case L7_SNTP_STATUS_SERVER_UNSYNCHRONIZED:
    *val = D_agentSntpClientLastAttemptStatus_serverUnsychronized;
    break;
  case L7_SNTP_STATUS_KISS_OF_DEATH:
    *val = D_agentSntpClientLastAttemptStatus_serverKissOfDeath;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerRowStatusGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_SNTP_SERVER_STATUS_t temp_val;

  rc = usmDbSntpServerTableRowStatusGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNTP_SERVER_STATUS_ACTIVE:
      *val = D_agentSntpClientUcastServerRowStatus_active;
      break;
    case L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE:
      *val = D_agentSntpClientUcastServerRowStatus_notInService;
      break;
    case L7_SNTP_SERVER_STATUS_NOT_READY:
      *val = D_agentSntpClientUcastServerRowStatus_notReady;
      break;
    case L7_SNTP_SERVER_STATUS_CREATE_AND_GO:
      *val = D_agentSntpClientUcastServerRowStatus_createAndGo;
      break;
    case L7_SNTP_SERVER_STATUS_CREATE_AND_WAIT:
      *val = D_agentSntpClientUcastServerRowStatus_createAndWait;
      break;
    case L7_SNTP_SERVER_STATUS_DESTROY:
      *val = D_agentSntpClientUcastServerRowStatus_destroy;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSntpClientUcastServerRowStatusSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_SNTP_SERVER_STATUS_t temp_val = 0;

  switch (val)
  {
  case D_agentSntpClientUcastServerRowStatus_active:
    temp_val = L7_SNTP_SERVER_STATUS_ACTIVE;
    break;
  case D_agentSntpClientUcastServerRowStatus_notInService:
    temp_val = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
    break;
  case D_agentSntpClientUcastServerRowStatus_notReady:
    temp_val = L7_SNTP_SERVER_STATUS_NOT_READY;
    break;
  case D_agentSntpClientUcastServerRowStatus_createAndGo:
    temp_val = L7_SNTP_SERVER_STATUS_CREATE_AND_GO;
    break;
  case D_agentSntpClientUcastServerRowStatus_createAndWait:
    temp_val = L7_SNTP_SERVER_STATUS_CREATE_AND_WAIT;
    break;
  case D_agentSntpClientUcastServerRowStatus_destroy:
    temp_val = L7_SNTP_SERVER_STATUS_DESTROY;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSntpServerTableRowStatusSet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerIndex, temp_val);
  }

  return rc;
}

/****************************************************************************************/
