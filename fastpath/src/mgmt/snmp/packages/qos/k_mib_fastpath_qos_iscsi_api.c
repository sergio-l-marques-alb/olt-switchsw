/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_qos_iscsi.c
*
* @purpose  Wrapper functions for FASTPATH iSCSI MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 10/20/2008
*
* @author jeffr
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_qos_iscsi_api.h"
#include "usmdb_util_api.h"
#include "iscsi_exports.h"
#include "osapi_support.h"

#ifdef L7_ISCSI_PACKAGE
  #include "usmdb_qos_iscsi.h"
#endif

L7_RC_t
snmpAgentIscsiEnableGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIscsiAdminModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentIscsiFlowAccelerationEnable_true;
      break;
    case L7_DISABLE:
      *val = D_agentIscsiFlowAccelerationEnable_false;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiAgingTimeOutGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIscsiTimeOutIntervalGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;    
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiQosTypeGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_QOS_COS_MAP_INTF_MODE_t temp_val;

  rc = usmDbIscsiTagFieldGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
      *val = D_agentIscsiFlowAccelerationQosType_vpt;
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
      *val = D_agentIscsiFlowAccelerationQosType_dscp;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiQosVptValueGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIscsiVlanPriorityGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiQosDscpValueGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIscsiDscpGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiQosRemarkGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIscsiMarkingModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentIscsiFlowAccelerationQosRemark_true;
      break;
    case L7_DISABLE:
      *val = D_agentIscsiFlowAccelerationQosRemark_false;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiEnableSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val;

  switch (val)
  {
  case D_agentIscsiFlowAccelerationEnable_true:
    temp_val = L7_ENABLE;
    break;
  case D_agentIscsiFlowAccelerationEnable_false:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (L7_SUCCESS == rc)
    rc = usmDbIscsiAdminModeSet(temp_val);

  return rc;
}

L7_RC_t
snmpAgentIscsiAgingTimeOutSet(L7_uint32 val)
{
  return(usmDbIscsiTimeOutIntervalSet(val));
}

L7_RC_t
snmpAgentIscsiQosTypeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t temp_val;

  switch (val)
  {
  case D_agentIscsiFlowAccelerationQosType_vpt:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
    break;
  case D_agentIscsiFlowAccelerationQosType_dscp:
    temp_val = L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (L7_SUCCESS == rc)
    rc = usmDbIscsiTagFieldSet(temp_val);

  return rc;
}


L7_RC_t
snmpAgentIscsiQosVptValueSet(L7_uint32 val)
{
  return(usmDbIscsiVlanPrioritySet(val));
}

L7_RC_t
snmpAgentIscsiQosDscpValueSet(L7_uint32 val)
{
  return(usmDbIscsiDscpSet(val));
}

L7_RC_t
snmpAgentIscsiQosRemarkSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val;

  switch (val)
  {
  case D_agentIscsiFlowAccelerationQosRemark_true:
    temp_val = L7_ENABLE;
    break;
  case D_agentIscsiFlowAccelerationQosRemark_false:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (L7_SUCCESS == rc)
    rc = usmDbIscsiMarkingModeSet(temp_val);

  return rc;
}

L7_RC_t
snmpAgentIscsiTargetConfigEntryDataGet(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData, 
                                       L7_int32 nominator,
                                       L7_int32 entryId)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  CLR_VALID(nominator, agentIscsiFlowAccelerationTargetConfigEntryData->valid);

  switch (nominator)
  {
  case I_agentIscsiFlowAccelerationTargetConfigStatus:
    /* all entries are active */
    rc = L7_SUCCESS;
    agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigStatus = D_agentIscsiFlowAccelerationTargetConfigStatus_active;
    SET_VALID(I_agentIscsiFlowAccelerationTargetConfigStatus, agentIscsiFlowAccelerationTargetConfigEntryData->valid);
    break;

  case I_agentIscsiFlowAccelerationTargetConfigName:
    memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
    if ((usmDbIscsiTargetTableEntryTargetNameGet(entryId, snmp_buffer) == L7_SUCCESS) &&
        (strlen((char *)snmp_buffer) > 0) &&
        (SafeMakeOctetString(&agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigName, 
                             snmp_buffer, strlen((char *)snmp_buffer)) == L7_TRUE))
    {
      SET_VALID(I_agentIscsiFlowAccelerationTargetConfigName, agentIscsiFlowAccelerationTargetConfigEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  default:
    /* unknown nominator */
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiTargetConfigEntryGet(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData, 
                                   L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_inet_addr_t ipAddr;
  L7_uint32 entryId;

  memset(&ipAddr, 0, sizeof(ipAddr));
  ipAddr.family = L7_AF_INET;
  ipAddr.addr.ipv4.s_addr = osapiHtonl(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigAddr);

  if (usmDbIscsiTargetTableEntryGet(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort, 
                                    &ipAddr, &entryId) == L7_SUCCESS)
  {
    if (snmpAgentIscsiTargetConfigEntryDataGet(agentIscsiFlowAccelerationTargetConfigEntryData, nominator, entryId) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiTargetConfigEntryGetNext(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_inet_addr_t ipAddr;
  L7_uint32 entryId;
  L7_uint32 tcpPort;

  memset(&ipAddr, 0, sizeof(ipAddr));
  ipAddr.family = L7_AF_INET;
  ipAddr.addr.ipv4.s_addr = osapiHtonl(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigAddr);


  while (usmDbIscsiTargetTableEntryNextGet(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort, 
                                           &ipAddr, &entryId) == L7_SUCCESS)
  {
    if ((usmDbIscsiTargetTableEntryTcpPortGet(entryId, &tcpPort) == L7_SUCCESS) &&
        (usmDbIscsiTargetTableEntryIpAddressGet(entryId, &ipAddr) == L7_SUCCESS))
    {
      agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigAddr =
        osapiNtohl(ipAddr.addr.ipv4.s_addr);
      agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort = 
        tcpPort;
      if (snmpAgentIscsiTargetConfigEntryDataGet(agentIscsiFlowAccelerationTargetConfigEntryData, nominator, entryId) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiTargetConfigEntrySet(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData)
{
  L7_inet_addr_t ipAddr;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  memset(snmp_buffer, 0, sizeof(snmp_buffer));
  memset(&ipAddr, 0, sizeof(ipAddr));
  ipAddr.family = L7_AF_INET;
  ipAddr.addr.ipv4.s_addr = 
    osapiHtonl(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigAddr);

  if (VALID(I_agentIscsiFlowAccelerationTargetConfigName, agentIscsiFlowAccelerationTargetConfigEntryData->valid))
  {
    memcpy(snmp_buffer, 
           agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigName->octet_ptr, 
           agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigName->length);
    if (usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationTargetConfigStatus, agentIscsiFlowAccelerationTargetConfigEntryData->valid))
  {
    /* destroy -> delete */
    if (agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigStatus == D_agentIscsiFlowAccelerationTargetConfigStatus_destroy)
    {
      return(usmDbIscsiTargetTcpPortDelete(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort,
                                           &ipAddr));
    }
    /* create_and_go -> add */
    if ((agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigStatus == D_agentIscsiFlowAccelerationTargetConfigStatus_createAndGo) ||
        (agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigStatus == D_agentIscsiFlowAccelerationTargetConfigStatus_active))
    {
      if (usmDbIscsiTargetTcpPortAdd(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort,
                                     &ipAddr,
                                     snmp_buffer) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
    }
  }
  else
  {
    /* just update existing with name data */
    if (VALID(I_agentIscsiFlowAccelerationTargetConfigName, agentIscsiFlowAccelerationTargetConfigEntryData->valid))
      if (usmDbIscsiTargetTcpPortAdd(agentIscsiFlowAccelerationTargetConfigEntryData->agentIscsiFlowAccelerationTargetConfigTcpPort,
                                     &ipAddr,
                                     snmp_buffer) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
  }



  return L7_ERROR;
}

L7_RC_t
snmpAgentIscsiFlowAccelerationSessionEntryGet(agentIscsiFlowAccelerationSessionEntry_t *agentIscsiFlowAccelerationSessionEntryData, 
                                 L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 val;
  L7_clocktime time, timeNow;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  CLR_VALID(nominator, agentIscsiFlowAccelerationSessionEntryData->valid);

  switch (nominator)
  {
  case I_agentIscsiFlowAccelerationTargetName:
    memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
    if ((usmDbIscsiSessionTargetNameGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationTargetName, 
                             snmp_buffer, strlen((char *)snmp_buffer)) == L7_TRUE))
    {
      SET_VALID(I_agentIscsiFlowAccelerationTargetName, agentIscsiFlowAccelerationSessionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationInitiatorName:
    memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
    if ((usmDbIscsiSessionInitiatorNameGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationInitiatorName, 
                             snmp_buffer, strlen((char *)snmp_buffer)) == L7_TRUE))
    {
      SET_VALID(I_agentIscsiFlowAccelerationInitiatorName, agentIscsiFlowAccelerationSessionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationSessionISID:
    memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
    if ((usmDbIscsiSessionIsidGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, snmp_buffer) == L7_SUCCESS) &&
        ((SafeMakeOctetString(&agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionISID, snmp_buffer, ISCSI_ISID_FIELD_LENGTH)) == L7_TRUE))
    {
      SET_VALID(I_agentIscsiFlowAccelerationSessionISID, agentIscsiFlowAccelerationSessionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationSessAgingTime:
    if (usmDbIscsiSessionSilentTimeGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, 
                                       &val) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessAgingTime = val;
      SET_VALID(I_agentIscsiFlowAccelerationSessAgingTime, agentIscsiFlowAccelerationSessionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationSessionUpTime:
    if (usmDbIscsiSessionStartTimeGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, 
                                      &time) == L7_SUCCESS)
    {
      osapiClockTimeRaw(&timeNow);
      agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionUpTime = timeNow.seconds - time.seconds;
      SET_VALID(I_agentIscsiFlowAccelerationSessionUpTime, agentIscsiFlowAccelerationSessionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;


  default:
    /* unknown nominator */
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiFlowAccelerationSessionEntryGetNext(agentIscsiFlowAccelerationSessionEntry_t *agentIscsiFlowAccelerationSessionEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbIscsiSessionNextByEntryIdGet(agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex, 
                                  &agentIscsiFlowAccelerationSessionEntryData->agentIscsiFlowAccelerationSessionIndex) == L7_SUCCESS)
  {
    if (snmpAgentIscsiFlowAccelerationSessionEntryGet(agentIscsiFlowAccelerationSessionEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentIscsiFlowAccelerationConnectionEntryGet(agentIscsiFlowAccelerationConnectionEntry_t *agentIscsiFlowAccelerationConnectionEntryData, 
                                                 L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 val;
  L7_inet_addr_t ipAddr;

  CLR_VALID(nominator, agentIscsiFlowAccelerationConnectionEntryData->valid);

  switch (nominator)
  {
  case I_agentIscsiFlowAccelerationConnectionTargetAddr:
    if (usmDbIscsiConnectionTargetIpAddressGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &ipAddr) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionTargetAddr = osapiNtohl(ipAddr.addr.ipv4.s_addr);
      SET_VALID(I_agentIscsiFlowAccelerationConnectionTargetAddr, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationConnectionTargetPort:
    if (usmDbIscsiConnectionTargetTcpPortGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &val) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionTargetPort = val;
      SET_VALID(I_agentIscsiFlowAccelerationConnectionTargetPort, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationConnectionInitiatorAddr:
    if (usmDbIscsiConnectionInitiatorIpAddressGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &ipAddr) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionInitiatorAddr = osapiNtohl(ipAddr.addr.ipv4.s_addr);
      SET_VALID(I_agentIscsiFlowAccelerationConnectionInitiatorAddr, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationConnectionInitiatorPort:
    if (usmDbIscsiConnectionInitiatorTcpPortGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &val) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionInitiatorPort = val;
      SET_VALID(I_agentIscsiFlowAccelerationConnectionInitiatorPort, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationConnectionCID:
    if (usmDbIscsiConnectionCidGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &val) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionCID = val;
      SET_VALID(I_agentIscsiFlowAccelerationConnectionCID, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  case I_agentIscsiFlowAccelerationConnectionSessionIndex:
    if (usmDbIscsiConnectionSessionIdGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, &val) == L7_SUCCESS)
    {
      agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionSessionIndex = val;
      SET_VALID(I_agentIscsiFlowAccelerationConnectionSessionIndex, agentIscsiFlowAccelerationConnectionEntryData->valid);
      rc = L7_SUCCESS;
    }
    break;

  default:
    /* unknown nominator */
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentIscsiFlowAccelerationConnectionEntryGetNext(agentIscsiFlowAccelerationConnectionEntry_t *agentIscsiFlowAccelerationConnectionEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbIscsiConnectionNextByEntryIdGet(agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex, 
                                     &agentIscsiFlowAccelerationConnectionEntryData->agentIscsiFlowAccelerationConnectionIndex) == L7_SUCCESS)
  {
    if (snmpAgentIscsiFlowAccelerationConnectionEntryGet(agentIscsiFlowAccelerationConnectionEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}

