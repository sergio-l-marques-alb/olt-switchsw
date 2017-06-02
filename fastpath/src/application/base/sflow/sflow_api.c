/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sflow_api.c
*
* @purpose  API's of sFlowComponent
*
* @component sflow
*
* @create  10/21/2007
*
* @author  Rajesh G
* @end
*
**********************************************************************/
#include "comm_mask.h"
#include "commdefs.h"
#include "ipv6_commdefs.h"
#include "defaultconfig.h"
#include "comm_structs.h"
#include "l7_common.h"
#include "usmdb_registry_api.h"
#include "usmdb_util_api.h"
#include "l7utils_inet_addr_api.h"
#include "osapi.h"
#include "string.h"
#include "stdlib.h"
#include "nimapi.h"
#include "usmdb_sim_api.h"
#include "nvstoreapi.h"

#include "sflow_api.h"
#include "sflow_db.h"
#include "sflow_cnfgr.h"
#include "sflow_proto.h"
#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_outcalls.h"
extern SFLOW_agent_t agent;
/*********************************************************************
* @purpose  This function is used to get the Agent Version.
*
* @param    UnitIndex   unit index
* @param    version     Agennt version string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowAgentVersionGet(L7_char8 *version)
{
  L7_uchar8 buf[SFLOW_AGENT_VERSION_LEN], swVersion[SFLOW_SW_VERSION_LEN];
  if( version != L7_NULLPTR)
  {
    /* Software Version */
    if (usmDbSwVersionGet(L7_NULL, swVersion) != L7_SUCCESS)
    {
      memset(swVersion, 0, SFLOW_SW_VERSION_LEN);
    }

    memset(buf, 0, SFLOW_AGENT_VERSION_LEN);
    osapiSnprintf(buf , SFLOW_AGENT_VERSION_LEN, "%s;%s;%s", SFLOW_MIB_VERSION,
                  PROD_COMPANY_NAME, swVersion);
    strcpy(version, buf);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  This function is used to get the Agent Address Type.
*
* @param    UnitIndex   unit index
* @param    type        Pointer to the Agent Address Type
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowAgentAddressTypeGet(L7_uint32 *type)
{
  L7_inet_addr_t ipAddr;
  if(sFlowAgentAddrGet(&ipAddr) != L7_SUCCESS)
  {
    *type = L7_NULL;
    return L7_FAILURE;
  }
  else
  {
    *type = ipAddr.family;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is used to get the Agent Address .
*
* @param    UnitIndex   unit index
* @param    strIpAddr   Agent Address in Printable format
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t sFlowAgentAddressGet(L7_uchar8 *strIpAddr)
{
  L7_inet_addr_t ipAddr;
  L7_RC_t rc = L7_SUCCESS;
  if(sFlowAgentAddrGet(&ipAddr) != L7_SUCCESS)
  {
    rc =  L7_FAILURE;
  }
  if(ipAddr.family== L7_AF_INET)
  {
    osapiInetNtoa(ipAddr.addr.ipv4.s_addr, strIpAddr);
  }
  else if(inetAddrPrint(&ipAddr, strIpAddr) == L7_NULLPTR)
  {
    rc =  L7_FAILURE;
  }
  return rc;

}
/*********************************************************************
* @purpose  Checks if the specified receiver index is valid or not
*
* @param    index  @b{(input)} receiver index
*
* @returns  L7_SUCCESS - Valid receiver index
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrGet(L7_uint32 index)
{
   if(index > 0 && index <= L7_SFLOW_MAX_RECEIVERS)
   {
     return L7_SUCCESS;
   }
   return L7_FAILURE;
}
/*********************************************************************
* @purpose  Gets next valid receiver index if it exists
*
* @param    index      @b{(input)}  receiver index
* @param   *nextIndex  @b{(output)} receiver index
*
* @returns  L7_SUCCESS - No next valid receiver present
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrNextGet(L7_uint32 index, L7_uint32 *nextIndex)
{
  if(index == 0)
  {
    *nextIndex = 1;
    return L7_SUCCESS;
  }
  else if(index < L7_SFLOW_MAX_RECEIVERS)
  {
    *nextIndex = index + 1;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  This function is used to get the timeout value
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    time       @b{(input)}  Pointer to Receiver Timeout value
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrTimeoutGet(L7_uint32 index, L7_uint32 *time)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
     osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *time = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrTimeout;
     osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the timeout value
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    time       @b{(input)}  Timeout value
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrTimeoutSet(L7_uint32 index, L7_uint32 time)
{
  L7_char8 buffer[L7_SFLOW_OWNER_STRING_LEN];
  L7_RC_t         rc = L7_SUCCESS;

  memset(buffer, 0 , L7_SFLOW_OWNER_STRING_LEN);
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  if( time > L7_SFLOW_MAX_TIMEOUT)
  {
     time = L7_SFLOW_MAX_TIMEOUT;
  }
  else
  {
    if(sFlowRcvrOwnerGet(index, buffer) == L7_SUCCESS)
    {
      if(strlen(buffer) > 0)
      {
        rc = sFlowRcvrOwnerSet(index, buffer, time);
      }
      else
      {
        rc = L7_FAILURE;
      }
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to get the IP Address Type
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  Pointer to Receiver IP Address
*                                   type
*
* @returns  L7_SUCCESS
*
* @notes
*   1 Represents IPv4
*   2 Represents Ipv6
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressTypeGet(L7_uint32 index, L7_uint32 *version)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
     osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *version = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddressType;
     osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set address type
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    type       @b{(input)}  L7_AF_INET or L7_AF_INET6
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressTypeSet(L7_uint32 index, L7_uint32 type)
{
  L7_RC_t         rc = L7_SUCCESS;

  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    if(type != L7_AF_INET && type != L7_AF_INET6)
    {
      rc = L7_FAILURE;
    }
    else
    {
      osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
      if (type != agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddressType)
      {
        agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddressType = type;
        /* set flag to indicate configuration has changed */
        agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
      }
      osapiWriteLockGive(agent.sFlowCfgRWLock);
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to get the IP Address
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  Pointer to Receiver IP Address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressGet(L7_uint32 index, L7_inet_addr_t *ipAddr)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    memcpy(ipAddr, &(agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddress),
            sizeof(L7_inet_addr_t));
    osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the IP Address
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  IP Address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressSet(L7_uint32 index, L7_inet_addr_t *inetAddr)
{
  sFlowEventMsg_t msg;
  L7_inet_addr_t  prevAddr;

  if (index < 1 || index > SFLOW_MAX_RECEIVERS ||
      inetAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if ((L7_INET_IS_ADDR_BROADCAST(inetAddr)) ||
      inetIsInMulticast(inetAddr) == L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if (memcmp(inetAddr, &agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddress,
             sizeof(L7_inet_addr_t)) == 0)
  {
    /* No Change in configuration */
    osapiWriteLockGive(agent.sFlowCfgRWLock);
    return L7_SUCCESS;
  }

  memcpy(&prevAddr, &agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddress,
         sizeof(L7_inet_addr_t));
  /* Update configuration */
  memcpy(&agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddress,
         inetAddr, sizeof(L7_inet_addr_t));
  agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrAddressType = inetAddr->family;
  /* set flag to indicate configuration has changed */
  agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;

  if (inetIsAddressZero(&prevAddr) == L7_FALSE &&
      inetIsAddressZero(inetAddr) == L7_FALSE)
  {
    /* No Change in operational modes for the pollers/samplers */
    osapiWriteLockGive(agent.sFlowCfgRWLock);
    return L7_SUCCESS;
  }
  else
  {
    /* Notify application of the change */
    msg.msgId         = SFLOW_RCVR_ADDR_SET;
    msg.receiverIndex = index;
    memcpy(&msg.u.addrVal, inetAddr, sizeof(L7_inet_addr_t));
    if (osapiMessageSend(agent.sFlowEventQueue, &msg, SFLOW_MSG_SIZE, L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
      if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
                "sFlowRcvrAddressSet: Failed to give msgQueue semaphore\n");
      }
      osapiWriteLockGive(agent.sFlowCfgRWLock);
      return L7_SUCCESS;
    }
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  This function is used to get the port
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    port       @b{(input)}  Pointer to port
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrPortGet(L7_uint32 index, L7_uint32 *port)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *port= agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrPort;
    osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the port
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    port       @b{(input)}  Port
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrPortSet(L7_uint32 index, L7_uint32 port)
{
  L7_RC_t         rc = L7_SUCCESS;

  if (index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else if (port < L7_SFLOW_MIN_PORT || port >  L7_SFLOW_MAX_PORT)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    if (port != agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrPort)
    {
      agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrPort = port;
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to get the sFlow Datagram Version
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    version    @b{(input)}  Pointer to Datagram Version
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrDatagramVersionGet(L7_uint32 index, L7_uint32 *version)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *version = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrDatagramVersion;
    osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the sFlow Datagram Version
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    version    @b{(input)}  Datagram Version
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowRcvrDatagramVersionSet(L7_uint32 index, L7_uint32 version)
{
  L7_RC_t         rc = L7_SUCCESS;

  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    if( version != SFLOW_DATAGRAM_VERSION5 )
    {
      version = SFLOW_DATAGRAM_VERSION5;
    }
    osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    if (version != agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrDatagramVersion)
    {
      agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrDatagramVersion = version;
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to get the sFlow receiver snmp mode
*           associated with the Receiver Index
*
* @param    index    @b{(input)}  Receiver Index
* @param    mode     @b{(input)}  L7_ENABLE - Collector maintains
*                                 the receiver record
*                                 L7_DISABLE - Receiver record is
*                                 persistent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrSnmpModeGet(L7_uint32 index, L7_uint32 *mode)
{
  L7_RC_t         rc = L7_SUCCESS;
  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *mode = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrSnmpMode;
    osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the sFlow receiver snmp mode
*           associated with the Receiver Index
*
* @param    index    @b{(input)}  Receiver Index
* @param    mode     @b{(input)}  L7_ENABLE - Collector maintains
*                                 the receiver record
*                                 L7_DISABLE - Receiver record is
*                                 persistent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowRcvrSnmpModeSet(L7_uint32 index, L7_uint32 mode)
{
  L7_RC_t         rc = L7_SUCCESS;

  if(index < 1 || index > SFLOW_MAX_RECEIVERS ||
     (mode != L7_ENABLE && mode != L7_DISABLE))
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    if (mode != agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrSnmpMode)
    {
      agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrSnmpMode = mode;
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to get the Datagram Size
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    dSize      @b{(input)}  Pointer to Datagram size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrMaximumDatagramSizeGet(L7_uint32 index, L7_uint32 *dSize)
{
  L7_RC_t         rc = L7_SUCCESS;

  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    *dSize = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrMaxDatagramSize;
    osapiReadLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the Datagram Size
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    dSize      @b{(input)}  Datagram size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrMaximumDatagramSet(L7_uint32 index, L7_uint32 dSize)
{
  L7_RC_t         rc = L7_SUCCESS;

  if( index < 1 || index > SFLOW_MAX_RECEIVERS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    if (dSize < SFLOW_MIN_DATAGRAM_SIZE)
    {
      dSize = SFLOW_MIN_DATAGRAM_SIZE;
    }
    else if (dSize > SFLOW_MAX_DATAGRAM_SIZE)
    {
      dSize = SFLOW_MAX_DATAGRAM_SIZE;
    }

    osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    if (dSize != agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrMaxDatagramSize)
    {
      agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrMaxDatagramSize = dSize;
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
  }
  return rc;
}

/*********************************************************************
* @purpose  This function is used to get the Owner String
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    buffer     @b{(input)}  Pointer to Receiver Buffer
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrOwnerGet(L7_uint32 index, L7_uchar8 *buffer)
{
  L7_RC_t         rc = L7_SUCCESS;

  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if(index < 1 || index > SFLOW_MAX_RECEIVERS ||
     buffer == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  else
  {
    memcpy(buffer, agent.sFlowCfg->receiverGroup[index- 1].sFlowRcvrOwner,
           L7_SFLOW_OWNER_STRING_LEN);
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the Owner String
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    buffer     @b{(input)}  Receiver Buffer
*
* @returns  L7_SUCCESS              successful addition
* @returns  L7_ALREADY_CONFIGURED   Receiver already configured
* @returns  L7_FAILURE              Other errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrOwnerSet(L7_uint32 index, L7_uchar8 *buffer,
                          L7_uint32 timeout)
{
  L7_uchar8      *owner;
  sFlowEventMsg_t msg;

  if (index < 1 || index > SFLOW_MAX_RECEIVERS ||
      buffer == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  owner = agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrOwner;

  if(strlen(buffer) > 0 && strlen(owner) > 0)
  {
    if (strcmp(buffer, owner) != 0)
    {
      /* Trying to acquire an existing resource */
      osapiWriteLockGive(agent.sFlowCfgRWLock);
      return L7_ALREADY_CONFIGURED;
    }
  }

  if (timeout == agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrTimeout &&
      memcmp(buffer, owner, L7_SFLOW_OWNER_STRING_LEN) == 0)
  {
     /* No Change in configuration */
      osapiWriteLockGive(agent.sFlowCfgRWLock);
      return L7_SUCCESS;
  }

  /* Update receiver configuration */
  memcpy(owner, buffer, L7_SFLOW_OWNER_STRING_LEN);
  agent.sFlowCfg->receiverGroup[index - 1].sFlowRcvrTimeout = timeout;
  /* set flag to indicate configuration has changed */
  agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;

  /* Notify application to take action */
  msg.msgId         = SFLOW_OWNER_STRING_TIMEOUT_SET;
  msg.receiverIndex = index;
  memcpy(msg.u.stringVal, buffer, L7_SFLOW_OWNER_STRING_LEN);
  msg.uintVal       = timeout;

  if (osapiMessageSend(agent.sFlowEventQueue, &msg, SFLOW_MSG_SIZE, L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
              "sFlowRcvrOnwerSet: Failed to give msgQueue semaphore\n");
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
    return L7_SUCCESS;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return L7_FAILURE;
}
/* fs functions */
/*********************************************************************
* @purpose  Returns sflowFsEntry status
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    instance will be all ways 1
*
* @end
*********************************************************************/
L7_RC_t sFlowFsEntryGet(L7_uint32 UnitIndex, L7_char8 *index,
                          L7_uint32 instance)
{
  L7_uint32 ifIndex;
  if( (index[0]=='\0') &&(instance == 1 ))
  {
    return L7_FAILURE;
  }
  if( (strcmp(L7_SFLOW_IFINDEX, index) < 0) &&(instance == 1 ))
  {
    ifIndex = atoi( index+(sizeof(L7_SFLOW_IFINDEX)));
    return usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex);
 }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Returns next sflowFsEntry index
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowFsEntryGetNext(L7_uint32 UnitIndex, L7_char8 *index,
                            L7_uint32 *instance )
{
  L7_uint32 ifIndex, nextIndex;
  L7_uint32 intIfIndex;
  L7_uint32 inclintIfType = L7_PHYSICAL_INTF;
  L7_uint32 exclintIfType = 0;
  if((index[0]=='\0') )
  {
    strcpy(index,"ifIndex.");
    ifIndex=0;
    if (usmDbIntIfNumTypeFirstGet(UnitIndex, inclintIfType, exclintIfType,
                                   &intIfIndex) == L7_SUCCESS)
    {
      if( usmDbExtIfNumFromIntIfNum(intIfIndex, &ifIndex) == L7_SUCCESS)
      {
        memset(index, 0, sizeof(index));
        sprintf(index,"%s.%d",L7_SFLOW_IFINDEX, ifIndex);
        *instance =1;
        return L7_SUCCESS;
      }
      return L7_FAILURE;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  if( (strcmp(L7_SFLOW_IFINDEX, index) < 0) )
  {
    ifIndex = atoi( index+(sizeof(L7_SFLOW_IFINDEX)));
    if (usmDbNextPhysicalExtIfNumberGet(UnitIndex, ifIndex, &nextIndex) == L7_SUCCESS)
    {
      index[0] = '\0';
      sprintf(index,"%s.%d",L7_SFLOW_IFINDEX, nextIndex);
      *instance =1;
      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  This function is used get the maximum header size
*           associated with the sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    maxHeaderSize   @b{(input)}  Pointer to Maximum header
                                         size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  sFlowFsMaximumHeaderSizeGet( L7_uint32 intIfNum, L7_uint32 instance,
                   L7_uint32 *maxHeaderSize)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pSampler= sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    *maxHeaderSize = pSampler->samplerCfg->sFlowFsMaximumHeaderSize;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  This function is used set the maximum header size
*           associated with the sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    maxHeaderSize   @b{(input)}  Maximun Header size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
***********************************************************************/
L7_RC_t  sFlowFsMaximumHeaderSizeSet( L7_uint32 intIfNum, L7_uint32 instance,
                   L7_uint32 maxHeaderSize)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pSampler= sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (maxHeaderSize < SFLOW_MIN_HEADER_SIZE)
    {
      maxHeaderSize = SFLOW_MIN_HEADER_SIZE;
    }
    if (maxHeaderSize > SFLOW_MAX_HEADER_SIZE)
    {
      maxHeaderSize = SFLOW_MAX_HEADER_SIZE;
    }

    if (pSampler->samplerCfg->sFlowFsMaximumHeaderSize != maxHeaderSize)
    {
      pSampler->samplerCfg->sFlowFsMaximumHeaderSize = maxHeaderSize;
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return rc;
}

/* snmp sflow FS set functions */

/*********************************************************************
* @purpose  This function is used get the Receiver Index
*           associated with the sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Pointer to receiver index
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowFsReciverGet(L7_uint32 intIfNum, L7_uint32 instance,
             L7_uint32 *receiver)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pSampler= sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    *receiver=  pSampler->samplerCfg->sFlowFsReceiver;
    rc = L7_SUCCESS;
  }
  else
  {
    *receiver = 0;
    rc = L7_FAILURE;
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);

  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the Receiver for
*           sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Receiver Index
*
* @returns  L7_SUCCESS         On successful creation of sampler
* @returns  L7_TABLE_IS_FULL   no more space for new sampler
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing cfg
* @returns  L7_FAILURE    other failure
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsReceiverSet(L7_uint32 intIfNum, L7_uint32 instance,
                           L7_uint32 receiver)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;

  if (receiver > SFLOW_MAX_RECEIVERS)
  {
    return L7_FAILURE;
  }

  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if (receiver != L7_NULL &&
     (pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (pSampler->samplerCfg->sFlowFsReceiver != receiver)
    {
      rc = L7_ALREADY_CONFIGURED;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
    return rc;
  }

  rc = sFlowSamplerReceiverApply(dsIndex, instance, receiver);
  if (rc == L7_SUCCESS)
  {
    /* set flag to indicate configuration has changed */
    agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  This function is used get the Packet Sampling Rate
*           associated with the sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    samplingRate    @b{(input)}  Pointer to sampling rate
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsPacketSamplingRateGet(L7_uint32 intIfNum, L7_uint32 instance,
                  L7_uint32 *samplingRate)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pSampler= sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    *samplingRate =  pSampler->samplerCfg->sFlowFsPacketSamplingRate;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);

  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the sampling rate
*           associated with the sFlow Sampler
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    samplingRate    @b{(input)}  Sampling rate
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsPacketSamplingRateSet(L7_uint32 intIfNum, L7_uint32 instance,
                  L7_uint32 samplingRate)
{
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  sFlowEventMsg_t msg;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);

  if(samplingRate != L7_NULL &&
     samplingRate < L7_SFLOW_MIN_SAMPLING_RATE)
  {
    samplingRate = L7_SFLOW_MIN_SAMPLING_RATE;
  }

  if (samplingRate > L7_SFLOW_MAX_SAMPLING_RATE)
  {
    samplingRate = L7_SFLOW_MAX_SAMPLING_RATE;
  }
  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);

  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (samplingRate == pSampler->samplerCfg->sFlowFsPacketSamplingRate)
    {
      /* No change in configuration */
      osapiWriteLockGive(agent.sFlowCfgRWLock);
      return L7_SUCCESS;
    }

    /* Update config */
    pSampler->samplerCfg->sFlowFsPacketSamplingRate = samplingRate;
    /* set flag to indicate configuration has changed */
    agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;

    /* Notify application of the change */
    msg.msgId = SFLOW_SAMPLING_RATE_SET;
    msg.dataSource = dsIndex;
    msg.sflowInstance = instance;
    msg.uintVal = samplingRate;
    if (osapiMessageSend(agent.sFlowEventQueue, &msg, SFLOW_MSG_SIZE, L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
      if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
                "sFlowFsPacketSamplingRateSet: Failed to give msgQueue semaphore\n");
      }
      rc = L7_SUCCESS;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);

  return rc;
}
/** END OF SF FUNCTIONS **/
/******* CP FUNCTIONS **********/
/*********************************************************************
* @purpose  Returns sflowCpEntry status
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowCpEntryGet(L7_uint32 UnitIndex, L7_uchar8 *index,
                             L7_uint32 instance)
{
  L7_uint32 ifIndex;
  if( (index[0]=='\0') &&(instance == 1 ))
  {
    return L7_FAILURE;
  }
  if( (strcmp(L7_SFLOW_IFINDEX, index) < 0) &&(instance == 1 ))
  {
    ifIndex = atoi( index+(sizeof(L7_SFLOW_IFINDEX)));
    return usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex);
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns next sflowCpEntry index
*
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowCpEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *index,
                                     L7_uint32 *instance)
{
  L7_uint32 ifIndex, nextIndex;
  L7_uint32 intIfIndex;
  L7_uint32 inclintIfType = L7_PHYSICAL_INTF;
  L7_uint32 exclintIfType = 0;
  if((index[0]=='\0') )
  {
    strcpy(index,"ifIndex.");
    ifIndex=0;
    if (usmDbIntIfNumTypeFirstGet(UnitIndex, inclintIfType, exclintIfType,
                                   &intIfIndex) == L7_SUCCESS)
    {
      if( usmDbExtIfNumFromIntIfNum(intIfIndex, &ifIndex) == L7_SUCCESS)
      {
        memset(index, 0, sizeof(index));
        sprintf(index,"%s.%d",L7_SFLOW_IFINDEX, ifIndex);
        *instance =1;
        return L7_SUCCESS;
      }
      return L7_FAILURE;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  if( (strcmp(L7_SFLOW_IFINDEX, index) < 0) )
  {
    ifIndex = atoi( index+(sizeof(L7_SFLOW_IFINDEX)));
    if (usmDbNextPhysicalExtIfNumberGet(UnitIndex, ifIndex, &nextIndex) == L7_SUCCESS)
    {
      index[0] = '\0';
      sprintf(index,"%s.%d",L7_SFLOW_IFINDEX, nextIndex);
      *instance =1;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  This function is used get the Receiver Idex
*           associated with the sFlow Counter Poller
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Pointer to Receiver Index
*
* @returns  L7_SUCCESS         On successful creation of poller object
* @returns  L7_TABLE_IS_FULL   no more space for new poller objects
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing object
* @returns  L7_FAILURE    other failure
*
* @notes
*
* @end
***********************************************************************/
L7_RC_t sFlowCpReciverGet( L7_uint32 intIfNum, L7_uint32 instance,
             L7_uint32 *receiver)
{
  SFLOW_poller_t *pPoller = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;/* default instance */


  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;
  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT);
  if (pPoller != L7_NULLPTR)
  {
    *receiver = pPoller->pollerCfg->sFlowCpReceiver;
    rc = L7_SUCCESS;
  }
  else
  {
    *receiver = 0;
    rc = L7_FAILURE;
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  This function is used to set the Receiver index
*          associated with the sFlow Counter Poller
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Receiver index
*
* @returns  L7_SUCCESS
*
* @notes    Check if a poller is already present. If present return
*            error.
*            failure.
*
* @end
***********************************************************************/
L7_RC_t sFlowCpReceiverSet(L7_uint32 intIfNum, L7_uint32 instance,
                           L7_uint32 receiver)
{
  SFLOW_poller_t *pPoller = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;

  if (receiver > SFLOW_MAX_RECEIVERS)
  {
    return L7_FAILURE;
  }

  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if (receiver != L7_NULL &&
     (pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (pPoller->pollerCfg->sFlowCpReceiver != receiver)
    {
      rc = L7_ALREADY_CONFIGURED;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
    return rc;
  }

  rc = sFlowPollerReceiverApply(dsIndex, instance, receiver);
  if (rc == L7_SUCCESS)
  {
    /* set flag to indicate configuration has changed */
    agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  This function is used get the Poller Interval
*           associated with the sFlow Counter Poller
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    interval        @b{(input)}  Pointer to poller interval
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowCpIntervalGet( L7_uint32 intIfNum, L7_uint32 instance,
                   L7_uint32 *interval)
{
  SFLOW_poller_t *pPoller = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;

  osapiReadLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    *interval = pPoller->pollerCfg->sFlowCpInterval;
     rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiReadLockGive(agent.sFlowCfgRWLock);

  return rc;
}
/*********************************************************************
* @purpose  This function is used set the counter poller interval
*           associated with the sFlow Counter Poller
*
* @param    intIfNum        @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    interval        @b{(input)}  Poller Interval
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowCpIntervalSet(L7_uint32 intIfNum, L7_uint32 instance,
                           L7_uint32 interval)
{
  SFLOW_poller_t *pPoller = L7_NULLPTR;
  sFlowEventMsg_t msg;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uint32       dsIndex;

  dsIndex = SFLOW_VAL_TO_DS_INDEX(intIfNum, L7_SFLOW_DS_TYPE_IFINDEX);

  if (interval < L7_SFLOW_MIN_POLLING_INTERVAL)
  {
    interval = L7_SFLOW_MIN_POLLING_INTERVAL;
  }
  if (interval > L7_SFLOW_MAX_POLLING_INTERVAL)
  {
    interval = L7_SFLOW_MAX_POLLING_INTERVAL;
  }
  osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (interval == pPoller->pollerCfg->sFlowCpInterval)
    {
      osapiWriteLockGive(agent.sFlowCfgRWLock);
      return rc;
    }

    pPoller->pollerCfg->sFlowCpInterval = interval;
    /* set flag to indicate configuration has changed */
    agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;

    msg.msgId         = SFLOW_POLL_INTERVAL_SET;
    msg.dataSource    = dsIndex;
    msg.sflowInstance = instance;
    msg.uintVal       = interval;
    if (osapiMessageSend(agent.sFlowEventQueue, &msg, SFLOW_MSG_SIZE, L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
      if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
                "sFlowCpIntervalSet: Failed to give msgQueue semaphore\n");
      }
      rc = L7_SUCCESS;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }
  osapiWriteLockGive(agent.sFlowCfgRWLock);
  return rc;
}

/********* END OF CP FUNCTIONS ****/

