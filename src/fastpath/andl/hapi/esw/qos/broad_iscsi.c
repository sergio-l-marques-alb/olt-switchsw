/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_iscsi.c
*
* @purpose   This file contains all the routines for iSCSI feature
*
* @component hapi
*
* @comments The iSCSI feature is implemented using CAP.
*
* @create    23-JUN-2008
*
* @end
*
**********************************************************************/

#include "osapi_support.h"
#include "broad_qos.h"
#include "iscsi_exports.h"
#include "broad_iscsi.h"
#include "iscsi_packet.h"

typedef struct
{
  BROAD_POLICY_t           iscsiPolicy;
  BROAD_POLICY_RULE_t      ruleId[2];
  L7_uint32                identifier;
  L7_uint32                initiatorIp;
  L7_uint32                targetIp;
  L7_ushort16              initiatorPort;
  L7_ushort16              targetPort;
  L7_uint32                counterId;
} HAPI_BROAD_ISCSI_CONNECTION_POLICY_t;

HAPI_BROAD_ISCSI_CONNECTION_POLICY_t   broadIscsiConnectionPolicyTable[L7_ISCSI_MAX_CONNECTIONS];
#define broadIscsiConnectionPolicyTableSize L7_ISCSI_MAX_CONNECTIONS

typedef struct
{
  BROAD_POLICY_t           iscsiPolicy;
  BROAD_POLICY_RULE_t      ruleId[2];
  L7_uint32                dstIp;
  L7_ushort16              dstPort;
} HAPI_BROAD_ISCSI_TARGET_PORT_POLICY_t;

HAPI_BROAD_ISCSI_TARGET_PORT_POLICY_t   broadIscsiTargetPortPolicyTable[L7_ISCSI_MAX_TARGET_TCP_PORTS];
#define broadIscsiTargetPortPolicyTableSize L7_ISCSI_MAX_TARGET_TCP_PORTS

static L7_BOOL hapiBroadQosIscsiIsActive = L7_FALSE;

/*********************************************************************
*
* @purpose  Returns whether the iSCSI component is actively monitoring
*           at least one target TCP port.  This function returns the
*           status found in a static variable which is updated each
*           time the tables are updated. This technique allows for
*           minimal performance impact when this status is being checked.
*
* @param    none
*
* @returns  L7_TRUE       iSCSI configured to monitor a TCP port
* @returns  L7_FALSE      iSCSI not monitoring any TCP ports
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQosIscsiActiveStatusGet(void)
{
  return hapiBroadQosIscsiIsActive;
}

/*********************************************************************
*
* @purpose  Examines the contents of the iSCSI HAPI tables to determine
*           whether the system is configured to monitor at least one
*           target port/address. This is used to update the global status
*           variable used in hapiBroadQosIscsiActiveStatusGet(). This function
*           is called after updates to the tables.
*
* @param    none
*
* @returns  L7_TRUE       iSCSI configured to monitor a TCP port
* @returns  L7_FALSE      iSCSI not monitoring any TCP ports
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQosIscsiActiveStatusCheck(void)
{
  L7_uint32 i;

  for (i=0; i < broadIscsiTargetPortPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID != broadIscsiTargetPortPolicyTable[i].iscsiPolicy)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;

}

/*********************************************************************
*
* @purpose  Finds the connection table entry associated with the given
*           identifier and returns the index and policyId created for
*           that connection.
*
* @param    L7_uint32  identifier  the id of the connection record to find
*           BROAD_POLICY_t *policyId  returns the policyId if found
*           L7_uint32 *index          returns the table index if found
*
* @returns  L7_SUCCESS    connection record matching identifier found
* @returns  L7_FAILURE    connection record not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionIdentifierLookUp(L7_uint32 identifier, BROAD_POLICY_t *policyId, L7_uint32 *index)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i=0; i < broadIscsiConnectionPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID != broadIscsiConnectionPolicyTable[i].iscsiPolicy)
    {
      if (broadIscsiConnectionPolicyTable[i].identifier == identifier)
      {
        found = L7_TRUE;
        break;
      }
    }
  }

  if (L7_TRUE == found)
  {
    *policyId = broadIscsiConnectionPolicyTable[i].iscsiPolicy;
    *index = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Finds the first connection table entry associated with the given
*           counter and returns the index and policyId. Multiple connection
*           records are combined to report the total counter data for the
*           entire iSCSI session. These connections are identified by using
*           a common counter ID.
*
* @param    L7_uint32  counterId  the id of the counter to find connections for
*           BROAD_POLICY_t *policyId  returns the policyId if found
*           L7_uint32 *index          returns the table index if found
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_FAILURE    record not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionCounterGetFirst(L7_uint32 counterId, BROAD_POLICY_t *policyId, L7_uint32 *index)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i=0; i < broadIscsiConnectionPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID != broadIscsiConnectionPolicyTable[i].iscsiPolicy)
    {
      if (broadIscsiConnectionPolicyTable[i].counterId == counterId)
      {
        found = L7_TRUE;
        break;
      }
    }
  }

  if (L7_TRUE == found)
  {
    *policyId = broadIscsiConnectionPolicyTable[i].iscsiPolicy;
    *index = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Finds the next connection table entry associated with the given
*           counter and returns the index and policyId. Multiple connection
*           records are combined to report the total counter data for the
*           entire iSCSI session. These connections are identified by using
*           a common counter ID.
*
* @param    L7_uint32  counterId  the id of the counter to find connections for
*           L7_uint32 index       the index of the current connection record
*           BROAD_POLICY_t *policyId  returns the policyId if found
*           L7_uint32 *nextIndex          returns the table index if found
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_FAILURE    record not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionCounterGetNext(L7_uint32 counterId, L7_uint32 index, BROAD_POLICY_t *policyId, L7_uint32 *nextIndex)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i = index+1; i < broadIscsiConnectionPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID != broadIscsiConnectionPolicyTable[i].iscsiPolicy)
    {
      if (broadIscsiConnectionPolicyTable[i].counterId == counterId)
      {
        found = L7_TRUE;
        break;
      }
    }
  }

  if (L7_TRUE == found)
  {
    *policyId = broadIscsiConnectionPolicyTable[i].iscsiPolicy;
    *nextIndex = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Finds a free connection table location and allocates it.
*
* @param    L7_uint32 *index        table index if found
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_FAILURE    record not available
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionPolicyAllocate(L7_uint32 *index)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i=0; i < broadIscsiConnectionPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID == broadIscsiConnectionPolicyTable[i].iscsiPolicy)
    {
      found = L7_TRUE;
      break;
    }
  }

  if (L7_TRUE == found)
  {
    *index = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Frees connection table location.
*
* @param    L7_uint32 index        table index
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_ERROR      record not in use or index out of range
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionPolicyFree(L7_uint32 index)
{
  if ((index < broadIscsiConnectionPolicyTableSize) &&
      (BROAD_POLICY_INVALID != broadIscsiConnectionPolicyTable[index].iscsiPolicy))
  {
    memset(&broadIscsiConnectionPolicyTable[index], 0, sizeof(HAPI_BROAD_ISCSI_CONNECTION_POLICY_t));
    broadIscsiConnectionPolicyTable[index].iscsiPolicy = BROAD_POLICY_INVALID;
    broadIscsiConnectionPolicyTable[index].ruleId[0] = BROAD_POLICY_RULE_INVALID;
    broadIscsiConnectionPolicyTable[index].ruleId[1] = BROAD_POLICY_RULE_INVALID;
    return L7_SUCCESS;
  }
  else
  {
    return L7_ERROR;
  }
}

/*********************************************************************
*
* @purpose  Finds the target port table entry associated with the given
*           information and returns the index and policyId created for
*           that connection.
*
* @param    L7_uint32  dstIp    the IP address, if any, of the target record to find,
*                               set to 0 if not needed
*           L7_uint32  dstPOrt  the TCP port of the target record to find
*           BROAD_POLICY_t *policyId  returns the policyId if found
*           L7_uint32 *index          returns the table index if found
*
* @returns  L7_SUCCESS    connection record matching identifier found
* @returns  L7_FAILURE    connection record not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiTargetPortPolicyLookUp(L7_uint32 dstIp, L7_ushort16 dstPort, BROAD_POLICY_t *policyId, L7_uint32 *index)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i=0; i < broadIscsiTargetPortPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID != broadIscsiTargetPortPolicyTable[i].iscsiPolicy)
    {
      if ((broadIscsiTargetPortPolicyTable[i].dstIp == dstIp) &&
          (broadIscsiTargetPortPolicyTable[i].dstPort == dstPort))
      {
        found = L7_TRUE;
        break;
      }
    }
  }
  if (L7_TRUE == found)
  {
    *policyId = broadIscsiTargetPortPolicyTable[i].iscsiPolicy;
    *index = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Finds a free target port table location and allocates it.
*
* @param    L7_uint32 *index        table index if found
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_FAILURE    record not available
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiTargetPortPolicyAllocate(L7_uint32 *index)
{
  L7_uint32 i;
  L7_BOOL   found = L7_FALSE;

  for (i=0; i < broadIscsiTargetPortPolicyTableSize; i++)
  {
    if (BROAD_POLICY_INVALID == broadIscsiTargetPortPolicyTable[i].iscsiPolicy)
    {
      found = L7_TRUE;
      break;
    }
  }
  if (L7_TRUE == found)
  {
    *index = i;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Frees target port table location.
*
* @param    L7_uint32 index        table index
*
* @returns  L7_SUCCESS    record matching identifier found
* @returns  L7_ERROR      record not in use or index out of range
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiTargetPortPolicyFree(L7_uint32 index)
{
  if ((index < broadIscsiTargetPortPolicyTableSize) &&
      (BROAD_POLICY_INVALID != broadIscsiTargetPortPolicyTable[index].iscsiPolicy))
  {
    memset(&broadIscsiTargetPortPolicyTable[index], 0, sizeof(HAPI_BROAD_ISCSI_TARGET_PORT_POLICY_t));
    broadIscsiTargetPortPolicyTable[index].iscsiPolicy = BROAD_POLICY_INVALID;
    broadIscsiTargetPortPolicyTable[index].ruleId[0] = BROAD_POLICY_RULE_INVALID;
    broadIscsiTargetPortPolicyTable[index].ruleId[1] = BROAD_POLICY_RULE_INVALID;
    return L7_SUCCESS;
  }
  else
  {
    return L7_ERROR;
  }
}

L7_RC_t hapiBroadIscsiPolicyRuleCounterAdd(BROAD_POLICY_RULE_t rule,
                                           DAPI_t *dapi_g)
{
  BROAD_PORT_t              *hapiPortPtr;
  DAPI_USP_t                dapiUsp;
  L7_RC_t                   result;

  if ((result = CPU_USP_GET(&dapiUsp)) == L7_SUCCESS)
  {
    hapiPortPtr = HAPI_PORT_GET(&dapiUsp, dapi_g);
    result = hapiBroadPolicyRuleCounterAdd(rule, BROAD_COUNT_PACKETS);
  }

  if (result != L7_SUCCESS)
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_NONE)
      sysapiPrintf("error in hapiBroadIscsiPolicyRuleCounterAdd() - result = %u rule = %u \n", result, rule);
  }
  return result;
}

/*********************************************************************
*
* @purpose  Adds a target TCP port and, optionally, IP address to
*           monitor for iSCSI session traffic.
*
*
* @returns  L7_SUCCESS    target port is being monitored
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiTargetPortAdd(DAPI_USP_t * usp, DAPI_CMD_t cmd, void *data, DAPI_t * dapi_g)
{
  DAPI_QOS_CMD_t            *iscsiCmd = (DAPI_QOS_CMD_t*)data;
  L7_uint32                 index;
  BROAD_POLICY_t            policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t       ruleId1 = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_RULE_t       ruleId2 = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_ACTION_t     action;
  L7_RC_t                   rc = L7_SUCCESS;
  L7_uchar8                 ipAddr[4], port[2];
  L7_uchar8                 opcode;
  L7_uchar8                 opcode_mask = 0x3f;
  L7_uchar8                 exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_POLICY_FIELD_t      opcodeField[] = {BROAD_FIELD_ISCSI_OPCODE, BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS};
  L7_uint32                 opcodeFieldIdx, numOpcodeFields;

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiTargetPortAdd() called to add targettcpPortNumber = %u ipAddress = %u.%u.%u.%u\n",
                 iscsiCmd->cmdData.iscsiTargetPortAdd.tcpPortNumber,
                 ((iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress & 0xFF000000) >> 24),
                 ((iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress & 0x00FF0000) >> 16),
                 ((iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress & 0x0000FF00) >> 8),
                 (iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress & 0x000000FF));

  /* see if we already have an entry matching this data */
  if (hapiBroadQosIscsiTargetPortPolicyLookUp(iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress,
                                              iscsiCmd->cmdData.iscsiTargetPortAdd.tcpPortNumber,
                                              &policyId, &index) == L7_SUCCESS)
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_MED)
      sysapiPrintf(" - target data already in table : policyId = %u index = %u\n",
                   policyId, index);
    return L7_SUCCESS;
  }

  if (hapiBroadQosIscsiTargetPortPolicyAllocate(&index) != L7_SUCCESS)
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
      sysapiPrintf(" - unable to allocate target port policy table entry\n");
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ISCSI_COMPONENT_ID, "Failed to allocate target port policy table entry");
    return L7_FAILURE;
  }

  /* set up and install policy to trap iSCSI LOGIN and LOGOUT REQUEST packets to CPU for this target data */
  do
  {
    if ((rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_ISCSI)) != L7_SUCCESS)
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyCreate() - rc = %u\n", rc);
      break;
    }

    numOpcodeFields = sizeof(opcodeField) / sizeof(BROAD_POLICY_FIELD_t);
    for (opcodeFieldIdx = 0; opcodeFieldIdx < numOpcodeFields; opcodeFieldIdx++)
    {
      if ((rc = hapiBroadPolicyPriorityRuleAdd(&ruleId1, BROAD_POLICY_RULE_PRIORITY_HIGHEST)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyPriorityRuleAdd() - rc = %u\n", rc);
        break;
      }

      memcpy(ipAddr, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress), sizeof(ipAddr));
      memcpy(port, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiTargetPortAdd.tcpPortNumber), sizeof(port));

      if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_DPORT, port, exact_match)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() for BROAD_FIELD_DPORT - rc = %u\n", rc);
        break;
      }

      if (0 != iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress)
      {
        if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_DIP, ipAddr, exact_match)) != L7_SUCCESS)
        {
          if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
            sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() for BROAD_FIELD_DIP - rc = %u\n", rc);
          break;
        }
      }

      opcode = ISCSI_OPCODE_LOGIN_REQ;
      if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, opcodeField[opcodeFieldIdx], &opcode, &opcode_mask)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() for BROAD_FIELD_ISCSI_OPCODE - rc = %u\n", rc);
        break;
      }


      if ((rc = hapiBroadPolicyRuleActionAdd(ruleId1, BROAD_ACTION_SET_COSQ, iscsiCmd->cmdData.iscsiTargetPortAdd.traffic_class, 0, 0)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleActionAdd() for BROAD_ACTION_SET_COSQ - rc = %u, traffic_class = %u\n",
                       rc, iscsiCmd->cmdData.iscsiTargetPortAdd.traffic_class);
        break;
      }

      if ((rc = hapiBroadPolicyRuleActionAdd(ruleId1, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleActionAdd() for BROAD_ACTION_COPY_TO_CPU - rc = %u\n", rc);
        break;
      }

      if (iscsiCmd->cmdData.iscsiTargetPortAdd.taggingEnabled == L7_TRUE)
      {
        switch (iscsiCmd->cmdData.iscsiTargetPortAdd.tagMode)
        {
        case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
          if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID, L7_ISCSI_DOT1P_MARKING_FEATURE_ID))
          {
            action = BROAD_ACTION_SET_USERPRIO;
          }
          else
          {
            action = BROAD_ACTION_LAST;
          }
          break;
        case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
          action = BROAD_ACTION_SET_DSCP;
          break;
        default:
          action = BROAD_ACTION_LAST;
          break;
        }

        if (action < BROAD_ACTION_LAST)
        {
          if ((rc = hapiBroadPolicyRuleActionAdd(ruleId1, action, iscsiCmd->cmdData.iscsiTargetPortAdd.tagData, 0, 0)) != L7_SUCCESS)
          {
            if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
              sysapiPrintf(" - error from hapiBroadPolicyRuleActionAdd() - rc = %u, action = %u, tagData = %u\n",
                           rc, action, iscsiCmd->cmdData.iscsiTargetPortAdd.tagData);
            break;
          }
        }
      }

      /* add counter to these rules to facilitate debugging */
      if ((rc = hapiBroadIscsiPolicyRuleCounterAdd(ruleId1, dapi_g)) != L7_SUCCESS)
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadIscsiPolicyRuleCounterAdd() - rc = %u\n", rc);
        break;
      }

      rc = hapiBroadPolicyRuleCopy(ruleId1, &ruleId2);
      if (L7_SUCCESS == rc)
      {
          /* update new rule with iscsi Logout opcode value */
          opcode = ISCSI_OPCODE_LOGOUT_REQ;
          if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId2, opcodeField[opcodeFieldIdx], &opcode, &opcode_mask)) != L7_SUCCESS)
          {
            if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
              sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() for BROAD_FIELD_ISCSI_OPCODE - rc = %u\n", rc);
            break;
          }
      }
      else
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleCopy() - rc = %u\n", rc);
        break;
      }
    }
    if (rc != L7_SUCCESS)
    {
      break;
    }

  } while(0);

  if (rc == L7_SUCCESS)
  {
    if ((rc = hapiBroadPolicyCommit(&policyId)) == L7_SUCCESS)
    {
      /* store information in table, if everything went well */
      broadIscsiTargetPortPolicyTable[index].dstIp = iscsiCmd->cmdData.iscsiTargetPortAdd.ipAddress;
      broadIscsiTargetPortPolicyTable[index].dstPort = iscsiCmd->cmdData.iscsiTargetPortAdd.tcpPortNumber;
      broadIscsiTargetPortPolicyTable[index].iscsiPolicy = policyId;
      broadIscsiTargetPortPolicyTable[index].ruleId[0] = ruleId1;
      broadIscsiTargetPortPolicyTable[index].ruleId[1] = ruleId2;

      hapiBroadQosIscsiIsActive = hapiBroadQosIscsiActiveStatusCheck();
    }
    else
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyCommit() - rc = %u\n", rc);
    }
  }
  else
  {
    hapiBroadPolicyCreateCancel();
  }

  if (rc != L7_SUCCESS)
  {
    /* problem encountered, perform any needed clean up */
    if (policyId != BROAD_POLICY_INVALID)
    {
      (void)hapiBroadPolicyDelete(policyId);
      (void)hapiBroadQosIscsiTargetPortPolicyFree(index);
    }
  }

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiTargetPortAdd() returning - rc = %u\n", rc);

  return rc;
}

/*********************************************************************
*
* @purpose  Deletes a target TCP port and, optionally, IP address from
*           monitoring for iSCSI session traffic.
*
*
* @returns  L7_SUCCESS    target port is being monitored
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiTargetPortDelete(DAPI_USP_t * usp, DAPI_CMD_t cmd, void *data, DAPI_t * dapi_g)
{
  DAPI_QOS_CMD_t            *iscsiCmd = (DAPI_QOS_CMD_t*)data;
  L7_uint32                 index;
  BROAD_POLICY_t            policyId = BROAD_POLICY_INVALID;
  L7_RC_t                   rc = L7_SUCCESS;

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiTargetPortDelete() called to delete targettcpPortNumber = %u ipAddress = %u.%u.%u.%u\n",
                 iscsiCmd->cmdData.iscsiTargetPortDelete.tcpPortNumber,
                 ((iscsiCmd->cmdData.iscsiTargetPortDelete.ipAddress & 0xFF000000) >> 24),
                 ((iscsiCmd->cmdData.iscsiTargetPortDelete.ipAddress & 0x00FF0000) >> 16),
                 ((iscsiCmd->cmdData.iscsiTargetPortDelete.ipAddress & 0x0000FF00) >> 8),
                 (iscsiCmd->cmdData.iscsiTargetPortDelete.ipAddress & 0x000000FF));

  /* see if we already have an entry matching this data */
  if (hapiBroadQosIscsiTargetPortPolicyLookUp(iscsiCmd->cmdData.iscsiTargetPortDelete.ipAddress,
                                              iscsiCmd->cmdData.iscsiTargetPortDelete.tcpPortNumber,
                                              &policyId, &index) == L7_SUCCESS)
  {
    if ((rc = hapiBroadPolicyDelete(policyId)) == L7_SUCCESS)
    {
        rc = hapiBroadQosIscsiTargetPortPolicyFree(index);
        if ((rc != L7_SUCCESS) && (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW))
          sysapiPrintf(" - error from hapiBroadQosIscsiTargetPortPolicyFree() - rc = %u, index = %u\n", rc, index);
    }
    else
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyDelete() - rc = %u, policyId = %u\n", rc, policyId);
    }

    hapiBroadQosIscsiIsActive = hapiBroadQosIscsiActiveStatusCheck();
  }
  else
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_MED)
      sysapiPrintf(" - tried to delete nonexistent entry\n");
  }
  /*
   * if table entry not found we will return success and not complain, error only if we
   * found the entry but could not delete it from policy manager
   */
  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiTargetPortDelete() returning - rc = %u\n", rc);
  return rc;
}

/*********************************************************************
*
* @purpose  Adds a TCP connection for iSCSI traffic.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionAdd(DAPI_USP_t * usp, DAPI_CMD_t cmd, void *data, DAPI_t * dapi_g)
{
  DAPI_QOS_CMD_t            *iscsiCmd = (DAPI_QOS_CMD_t*)data;
  L7_uint32                 index;
  BROAD_POLICY_t            policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t       ruleId1 = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_RULE_t       ruleId2 = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_ACTION_t     action;
  L7_RC_t                   rc = L7_SUCCESS;
  L7_uchar8                 targetIpAddr[4];
  L7_uchar8                 initiatorIpAddr[4];
  L7_uchar8                 targetPort[2];
  L7_uchar8                 initiatorPort[2];
  L7_uchar8                 exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};


  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiConnectionAdd() called to add: identifier = %u\n",
                 "  targetTcpPortNumber = %u targetIpAddress = %u.%u.%u.%u\n",
                 "  initiatorTcpPortNumber = %u initiatorIpAddress = %u.%u.%u.%u\n",
                 "  counterId = %u,  traffic_class = %u, taggingEnabled = %s, tagMode = %u, tagData = %u\n",
                 iscsiCmd->cmdData.iscsiConnectionAdd.identifier,
                 iscsiCmd->cmdData.iscsiConnectionAdd.targetTcpPort,
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress & 0xFF000000) >> 24),
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress & 0x00FF0000) >> 16),
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress & 0x0000FF00) >> 8),
                 (iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress & 0x000000FF),
                 iscsiCmd->cmdData.iscsiConnectionAdd.initiatorTcpPort,
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress & 0xFF000000) >> 24),
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress & 0x00FF0000) >> 16),
                 ((iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress & 0x0000FF00) >> 8),
                 (iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress & 0x000000FF),
                 iscsiCmd->cmdData.iscsiConnectionAdd.counterIdentifier,
                 iscsiCmd->cmdData.iscsiConnectionAdd.traffic_class,
                 iscsiCmd->cmdData.iscsiConnectionAdd.taggingEnabled ? "TRUE" : "FALSE",
                 iscsiCmd->cmdData.iscsiConnectionAdd.tagMode,
                 iscsiCmd->cmdData.iscsiConnectionAdd.tagData);

  /* see if we already have an entry matching this data */
  if (hapiBroadQosIscsiConnectionIdentifierLookUp(iscsiCmd->cmdData.iscsiConnectionAdd.identifier, &policyId, &index) == L7_SUCCESS)
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_MED)
      sysapiPrintf(" - connection data already in table : policyId = %u index = %u\n",
                   policyId, index);
    return L7_SUCCESS;
  }

  if (hapiBroadQosIscsiConnectionPolicyAllocate(&index) != L7_SUCCESS)
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
      sysapiPrintf(" - unable to allocate connection policy table entry\n");
    return L7_FAILURE;
  }

  do
  {
    if ((rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_ISCSI)) != L7_SUCCESS)
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyCreate() - rc = %u\n", rc);
      break;
    }

    if ((rc = hapiBroadPolicyPriorityRuleAdd(&ruleId1, BROAD_POLICY_RULE_PRIORITY_DEFAULT)) != L7_SUCCESS)
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyPriorityRuleAdd() - rc = %u\n", rc);
      break;
    }

    memcpy(initiatorIpAddr, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress), sizeof(initiatorIpAddr));
    memcpy(targetIpAddr, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress), sizeof(targetIpAddr));
    memcpy(initiatorPort, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiConnectionAdd.initiatorTcpPort), sizeof(initiatorPort));
    memcpy(targetPort, (L7_uchar8*)&(iscsiCmd->cmdData.iscsiConnectionAdd.targetTcpPort), sizeof(targetPort));

    /* rule1 is initiator -> target */
    if (((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_SIP, initiatorIpAddr, exact_match)) != L7_SUCCESS) ||
        ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_DIP, targetIpAddr, exact_match)) != L7_SUCCESS) ||
        ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_SPORT, initiatorPort, exact_match)) != L7_SUCCESS) ||
        ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId1, BROAD_FIELD_DPORT, targetPort, exact_match)) != L7_SUCCESS))
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() initiator -> target rule - rc = %u\n", rc);
      break;
    }

    if ((rc = hapiBroadPolicyRuleActionAdd(ruleId1, BROAD_ACTION_SET_COSQ, iscsiCmd->cmdData.iscsiConnectionAdd.traffic_class, 0, 0)) != L7_SUCCESS)
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyRuleActionAdd() for BROAD_ACTION_SET_COSQ - rc = %u, traffic_class = %u\n",
                     rc, iscsiCmd->cmdData.iscsiTargetPortAdd.traffic_class);
      break;
    }

    if (iscsiCmd->cmdData.iscsiConnectionAdd.taggingEnabled == L7_TRUE)
    {
      switch (iscsiCmd->cmdData.iscsiConnectionAdd.tagMode)
      {
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID, L7_ISCSI_DOT1P_MARKING_FEATURE_ID))
        {
          action = BROAD_ACTION_SET_USERPRIO;
        }
        else
        {
          action = BROAD_ACTION_LAST;
        }
        break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
        action = BROAD_ACTION_SET_TOS;
        break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
        action = BROAD_ACTION_SET_DSCP;
        break;
      default:
        action = BROAD_ACTION_LAST;
        break;
      }

      if (action < BROAD_ACTION_LAST)
      {
        if ((rc = hapiBroadPolicyRuleActionAdd(ruleId1, action, iscsiCmd->cmdData.iscsiConnectionAdd.tagData, 0, 0)) != L7_SUCCESS)
        {
          if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
            sysapiPrintf(" - error from hapiBroadPolicyRuleActionAdd() - rc = %u, action = %u, tagData = %u\n",
                         rc, action, iscsiCmd->cmdData.iscsiTargetPortAdd.tagData);
          break;
        }
      }
    }

    if ((rc = hapiBroadIscsiPolicyRuleCounterAdd(ruleId1, dapi_g)) != L7_SUCCESS)
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadIscsiPolicyRuleCounterAdd() - rc = %u\n", rc);
      break;
    }

    rc = hapiBroadPolicyRuleCopy(ruleId1, &ruleId2);
    if (L7_SUCCESS == rc)
    {
      /* rule2 is target -> initiator */
      if (((rc = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_DIP, initiatorIpAddr, exact_match)) != L7_SUCCESS) ||
          ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_SIP, targetIpAddr, exact_match)) != L7_SUCCESS) ||
          ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_DPORT, initiatorPort, exact_match)) != L7_SUCCESS) ||
          ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_SPORT, targetPort, exact_match)) != L7_SUCCESS))
      {
        if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
          sysapiPrintf(" - error from hapiBroadPolicyRuleQualifierAdd() target -> initiator rule - rc = %u\n", rc);
        break;
      }
    }
    else
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyRuleCopy() - rc = %u\n", rc);
      break;
    }

  } while(0);

  if (rc == L7_SUCCESS)
  {
    if ((rc = hapiBroadPolicyCommit(&policyId)) == L7_SUCCESS)
    {
      /* store information in table, since everything went well */
      broadIscsiConnectionPolicyTable[index].identifier = iscsiCmd->cmdData.iscsiConnectionAdd.identifier;
      broadIscsiConnectionPolicyTable[index].initiatorIp = iscsiCmd->cmdData.iscsiConnectionAdd.initiatorIpAddress;
      broadIscsiConnectionPolicyTable[index].targetIp = iscsiCmd->cmdData.iscsiConnectionAdd.targetIpAddress;
      broadIscsiConnectionPolicyTable[index].initiatorPort = iscsiCmd->cmdData.iscsiConnectionAdd.initiatorTcpPort;
      broadIscsiConnectionPolicyTable[index].targetPort = iscsiCmd->cmdData.iscsiConnectionAdd.targetTcpPort;
      broadIscsiConnectionPolicyTable[index].counterId = iscsiCmd->cmdData.iscsiConnectionAdd.counterIdentifier;
      broadIscsiConnectionPolicyTable[index].iscsiPolicy = policyId;
      broadIscsiConnectionPolicyTable[index].ruleId[0] = ruleId1;
      broadIscsiConnectionPolicyTable[index].ruleId[1] = ruleId2;
    }
    else
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyCommit() - rc = %u\n", rc);
    }
  }
  else
  {
    hapiBroadPolicyCreateCancel();
  }

  if (rc != L7_SUCCESS)
  {
    /* problem encountered, perform any needed clean up */
    if (policyId != BROAD_POLICY_INVALID)
    {
      (void)hapiBroadPolicyDelete(policyId);
      (void)hapiBroadQosIscsiConnectionPolicyFree(index);
    }
  }

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiConnectionAdd() returning - rc = %u\n", rc);

  return rc;
}

/*********************************************************************
*
* @purpose  Removes a TCP connection for iSCSI traffic.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiConnectionRemove(DAPI_USP_t * usp, DAPI_CMD_t cmd, void *data, DAPI_t * dapi_g)
{
  DAPI_QOS_CMD_t            *iscsiCmd = (DAPI_QOS_CMD_t*)data;
  L7_uint32                 index;
  BROAD_POLICY_t            policyId = BROAD_POLICY_INVALID;
  L7_RC_t                   rc;

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiConnectionRemove() called to remove: identifier = %u\n", iscsiCmd->cmdData.iscsiConnectionRemove.identifier);

  /* see if we already have an entry matching this data */
  if ((rc = hapiBroadQosIscsiConnectionIdentifierLookUp(iscsiCmd->cmdData.iscsiConnectionRemove.identifier, &policyId, &index)) == L7_SUCCESS)
  {
    if ((rc = hapiBroadPolicyDelete(policyId)) == L7_SUCCESS)
    {
      rc = hapiBroadQosIscsiConnectionPolicyFree(index);
      if ((rc != L7_SUCCESS) && (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW))
        sysapiPrintf(" - error from hapiBroadQosIscsiConnectionPolicyFree() - rc = %u, index = %u\n", rc, index);
    }
    else
    {
      if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
        sysapiPrintf(" - error from hapiBroadPolicyDelete() - rc = %u, policyId = %u\n", rc, policyId);
    }
  }
  else
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_MED)
      sysapiPrintf(" - tried to delete nonexistent entry\n");
  }
  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiConnectionRemove() returning - rc = %u\n", rc);
  return rc;
}

/*********************************************************************
*
* @purpose  Retrieves counter data associated with a group of connection
*           table entries that share a common counter identifier.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiCounterGet(DAPI_USP_t * usp, DAPI_CMD_t cmd, void *data, DAPI_t * dapi_g)
{
  DAPI_QOS_CMD_t *qosCmd = (DAPI_QOS_CMD_t *)data;
  BROAD_POLICY_t policyId;
  L7_uint32 index;
  L7_uint32 i;
  BROAD_POLICY_STATS_t stats[2];
  L7_uint32 numStats;
  L7_ulong64 temp;
  L7_RC_t rc = L7_FAILURE;

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiCounterGet() called: identifier = %u\n", qosCmd->cmdData.iscsiConnectionCounterGet.identifier);

  numStats = 2;
  temp.high = 0;
  temp.low  = 0;

  if (hapiBroadQosIscsiConnectionCounterGetFirst(qosCmd->cmdData.iscsiConnectionCounterGet.identifier, &policyId, &index) == L7_SUCCESS)
  {
    do
    {
      rc = hapiBroadPolicyStatsGet(policyId, broadIscsiConnectionPolicyTable[index].ruleId[0], &stats[0]);
      if (rc == L7_SUCCESS)
      {
        rc = hapiBroadPolicyStatsGet(policyId, broadIscsiConnectionPolicyTable[index].ruleId[1], &stats[1]);
  
        if (rc == L7_SUCCESS)
        {
          /* accumulate stats */
          for (i=0; i<numStats; i++)
          {
            temp.high = temp.high +
                        ((stats[i].statMode.counter.count >> 32) & 0xfffffff);
            temp.low  = temp.low +
                        ((stats[i].statMode.counter.count) & 0xfffffff);
          }
        }
      }
    } while (hapiBroadQosIscsiConnectionCounterGetNext(qosCmd->cmdData.iscsiConnectionCounterGet.identifier, index, &policyId, &index) == L7_SUCCESS);
  }
  else
  {
    if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_MED)
      sysapiPrintf("  - no connections found with this counter identifier\n");
  }

  if ((qosCmd->cmdData.iscsiConnectionCounterGet.counter_value != L7_NULLPTR) &&
      (rc == L7_SUCCESS))
  {
    qosCmd->cmdData.iscsiConnectionCounterGet.counter_value->high = temp.high;
    qosCmd->cmdData.iscsiConnectionCounterGet.counter_value->low  = temp.low;
  }

  if (hapiBroadIscsiDebugLevel() > ISCSI_DEBUG_LOW)
    sysapiPrintf("hapiBroadQosIscsiCounterGet() returning: rc = %u\n", rc);

  return rc;
}

/*********************************************************************
*
* @purpose  Initialize iSCSI component of QOS package
*
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t hapiBroadQosIscsiInit (DAPI_t * dapiPtr)
{
  L7_uint32 i;

  for (i=0; i<broadIscsiConnectionPolicyTableSize; i++)
  {
    broadIscsiConnectionPolicyTable[i].iscsiPolicy = BROAD_POLICY_INVALID;
  }

  for (i=0; i<broadIscsiTargetPortPolicyTableSize; i++)
  {
    broadIscsiTargetPortPolicyTable[i].iscsiPolicy = BROAD_POLICY_INVALID;
  }

  hapiBroadQosIscsiIsActive = hapiBroadQosIscsiActiveStatusCheck();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadQosIscsiPortInit
*
* @purpose  Per-port iSCSI init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosIscsiPortInit (DAPI_PORT_t * dapiPortPtr)
{

  dapiPortPtr->cmdTable[DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD] = (HAPICTLFUNCPTR_t) hapiBroadQosIscsiTargetPortAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE] = (HAPICTLFUNCPTR_t) hapiBroadQosIscsiTargetPortDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_ISCSI_CONNECTION_ADD] = (HAPICTLFUNCPTR_t) hapiBroadQosIscsiConnectionAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE] = (HAPICTLFUNCPTR_t) hapiBroadQosIscsiConnectionRemove;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_ISCSI_COUNTER_GET] = (HAPICTLFUNCPTR_t) hapiBroadQosIscsiCounterGet;
  return L7_SUCCESS;
}

static void hapiBroadIscsiConnectionTableEntryShow(L7_uint32 i)
{
  L7_inet_addr_t  ip_addr;
  L7_uchar8 initiatorIpAddrStr[64];
  L7_uchar8 targetIpAddrStr[64];
  L7_uchar8 ruleStatsString[64];
  BROAD_POLICY_STATS_t stats;

  ip_addr.family = L7_AF_INET;

  ip_addr.addr.ipv4.s_addr = broadIscsiConnectionPolicyTable[i].initiatorIp;
  (void)inetAddrHtop(&ip_addr, initiatorIpAddrStr);

  ip_addr.addr.ipv4.s_addr = broadIscsiConnectionPolicyTable[i].targetIp;
  (void)inetAddrHtop(&ip_addr, targetIpAddrStr);

  if (hapiBroadPolicyStatsGet(broadIscsiConnectionPolicyTable[i].iscsiPolicy, broadIscsiConnectionPolicyTable[i].ruleId[0], &stats) == L7_SUCCESS)
  {
    osapiSnprintf(ruleStatsString, sizeof(ruleStatsString), "%llu", stats.statMode.counter.count);
  }
  else
  {
    osapiSnprintf(ruleStatsString, sizeof(ruleStatsString), "Error");
  }

  sysapiPrintf("index=%u, policyId=%u, ruleId[0]=%u, ruleId[1]=%u, id=%u, initIP=%s, targetIP=%s, initPort=%u, targetPort=%u, counterId=%u, count = %s\n",
                i,
                broadIscsiConnectionPolicyTable[i].iscsiPolicy,
                broadIscsiConnectionPolicyTable[i].ruleId[0],
                broadIscsiConnectionPolicyTable[i].ruleId[1],
                broadIscsiConnectionPolicyTable[i].identifier,
                initiatorIpAddrStr,
                targetIpAddrStr,
                broadIscsiConnectionPolicyTable[i].initiatorPort,
                broadIscsiConnectionPolicyTable[i].targetPort,
                broadIscsiConnectionPolicyTable[i].counterId,
                ruleStatsString
                );

}

void hapiBroadIscsiConnectionTableShow(L7_int32 index)
{
  L7_uint32 i;

  if ((index < 0) || (index >= broadIscsiConnectionPolicyTableSize))
  {
    /* dump entire table */
    sysapiPrintf("Dumping broadIscsiConnectionPolicyTable[]\n");
    for (i=0; i<broadIscsiConnectionPolicyTableSize; i++)
    {
      if (BROAD_POLICY_INVALID != broadIscsiConnectionPolicyTable[i].iscsiPolicy)
      {
        hapiBroadIscsiConnectionTableEntryShow(i);
      }
    }
  }
  else if (BROAD_POLICY_INVALID == broadIscsiConnectionPolicyTable[index].iscsiPolicy)
  {
    sysapiPrintf("Index out of range or entry not in use.  Use index = 0 to all entries in use.\n");
  }
  else
  {
    /* dump single entry */
    hapiBroadIscsiConnectionTableEntryShow(index);
  }
}


static void hapiBroadIscsiTargetPortTableEntryShow(L7_uint32 i)
{
  L7_inet_addr_t  ip_addr;
  L7_uchar8 targetIpAddrStr[64];
  L7_uchar8 ruleStatsString[64];
  BROAD_POLICY_STATS_t stats;

  ip_addr.family = L7_AF_INET;

  ip_addr.addr.ipv4.s_addr = broadIscsiTargetPortPolicyTable[i].dstIp;
  (void)inetAddrHtop(&ip_addr, targetIpAddrStr);

  if (hapiBroadPolicyStatsGet(broadIscsiTargetPortPolicyTable[i].iscsiPolicy, broadIscsiTargetPortPolicyTable[i].ruleId[0], &stats) == L7_SUCCESS)
  {
    osapiSnprintf(ruleStatsString, sizeof(ruleStatsString), "%llu", stats.statMode.counter.count);
  }
  else
  {
    osapiSnprintf(ruleStatsString, sizeof(ruleStatsString), "Error");
  }

  sysapiPrintf("index=%u, policyId=%u, ruleId[0]=%u, ruleId[1]=%u, destIP=%s, destPort=%u, hit-count = %s\n",
                i,
                broadIscsiTargetPortPolicyTable[i].iscsiPolicy,
                broadIscsiTargetPortPolicyTable[i].ruleId[0],
                broadIscsiTargetPortPolicyTable[i].ruleId[1],
                targetIpAddrStr,
                broadIscsiTargetPortPolicyTable[i].dstPort,
                ruleStatsString
                );

}

void hapiBroadIscsiTargetPortTableShow(L7_int32 index)
{
  L7_uint32 i;
  L7_BOOL   entryInUse = L7_FALSE, activeStatus;

  if ((index >= broadIscsiTargetPortPolicyTableSize) || (index < 0))
  {
    /* dump entire table */
    sysapiPrintf("Dumping broadIscsiTargetPortPolicyTable[]\n");
    for (i=0; i<broadIscsiTargetPortPolicyTableSize; i++)
    {
      if (BROAD_POLICY_INVALID != broadIscsiTargetPortPolicyTable[i].iscsiPolicy)
      {
        hapiBroadIscsiTargetPortTableEntryShow(i);
        entryInUse = L7_TRUE;
      }
    }

    activeStatus = hapiBroadQosIscsiActiveStatusGet();
    sysapiPrintf("  Active status:  hapiBroadActiveStatusGet() returns %s - %s\n",
                 activeStatus ? "TRUE" : "FALSE",
                 (activeStatus == entryInUse) ? "OK" : "ERROR");
  }
  else if (BROAD_POLICY_INVALID == broadIscsiTargetPortPolicyTable[index].iscsiPolicy)
  {
    sysapiPrintf("Index out of range or entry not in use.  Use hapiBroadQosIscsiTargetPortTableShow(0) to all entries in use.\n");
  }
  else
  {
    /* dump single entry */
    hapiBroadIscsiTargetPortTableEntryShow(index);
  }
}

/* Tracing */

static BROAD_ISCSI_DEBUG_LEVEL_t debugOutput = ISCSI_DEBUG_NONE;

void hapiBroadIscsiDebugEnable(BROAD_ISCSI_DEBUG_LEVEL_t val)
{
    if (val > ISCSI_DEBUG_HIGH)
        val = ISCSI_DEBUG_HIGH;

    debugOutput = val;
}

void hapiBroadDebugIscsiEnable(BROAD_ISCSI_DEBUG_LEVEL_t val)
{
    /* an alias for convenience */
    hapiBroadIscsiDebugEnable(val);
}

BROAD_ISCSI_DEBUG_LEVEL_t hapiBroadIscsiDebugLevel()
{
    return debugOutput;
}
