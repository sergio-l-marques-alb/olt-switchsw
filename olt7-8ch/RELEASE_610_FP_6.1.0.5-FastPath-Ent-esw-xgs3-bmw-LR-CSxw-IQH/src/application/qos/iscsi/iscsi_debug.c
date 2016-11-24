/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
*
* @filename    iscsi_debug.c
* @purpose    
* @component   
* @comments    none
* @create      
*              
* @end
*             
*********************************************************************/
#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "iscsi.h"
#include "iscsi_api.h"
#include "iscsi_db.h"

extern iscsiCfgData_t  *iscsiCfgData;

void iscsiDebugEnable(L7_uint32 value)
{
  iscsiDebug = value;
}

L7_uint32 iscsiDebugEnableGet(void)
{
  return iscsiDebug;
}

/************************ DEBUG FUNCTIONS ****************************/
void iscsiConnectionRecordDump(L7_uint32 i)
{
  L7_inet_addr_t       targetIpAddress;
  L7_uint32            targetTcpPort;
  L7_inet_addr_t       initiatorIpAddress;
  L7_uint32            initiatorTcpPort;
  L7_uint32            cid;                /* CID from iSCSI protocol */
  L7_uchar8            targetIpAddrString[64];     /* leave plenty of room for IPv6 address strings */
  L7_uchar8            initiatorIpAddrString[64];

  (void)iscsiConnectionTargetIpAddressGet(i, &targetIpAddress);
  (void)iscsiConnectionTargetTcpPortGet(i, &targetTcpPort);
  (void)iscsiConnectionInitiatorIpAddressGet(i, &initiatorIpAddress);
  (void)iscsiConnectionInitiatorTcpPortGet(i, &initiatorTcpPort);
  (void)iscsiConnectionCidGet(i, &cid);

  if ((osapiInetNtop(targetIpAddress.family, (L7_uchar8 *)&targetIpAddress.addr.ipv4.s_addr, targetIpAddrString, sizeof(targetIpAddrString)) != L7_NULLPTR) && 
      (osapiInetNtop(initiatorIpAddress.family, (L7_uchar8 *)&initiatorIpAddress.addr.ipv4.s_addr, initiatorIpAddrString, sizeof(initiatorIpAddrString)) != L7_NULLPTR))
  {
    sysapiPrintf("  connectionTable[%u].targetIpAddress= %s, .targetTcpPort = %d, .initiatorIpAddress = %s, .initiatorTcpPort = %d, .cid=%d\n",
                 i,
                 targetIpAddrString,
                 targetTcpPort,
                 initiatorIpAddrString,
                 initiatorTcpPort,
                 cid);
  }
  else
  {
    sysapiPrintf("  connectionTable[%u] - ERROR: on conversion of targetIpAddress or initiatorIpAddress to string\n",
                 i);
  }
}

void iscsiDebugBuffToStr(L7_uchar8 *text, L7_uchar8 *buffer, L7_uint32 bufferLength)
{
  L7_uint32 i;
  L7_uchar8 formatBuff[3];  /* just big enough to hold string expressing a byte of data (plus the terminating \0 */

  for (i=0; i<bufferLength; i++)
  {
    (void)osapiSnprintf(formatBuff, sizeof(formatBuff), "%2.2x", buffer[i]);
    (void)osapiStrncat(&text[2*i], formatBuff, 2);
  }
}

void iscsiSessionRecordDump(L7_uint32 i)
{
  L7_uchar8            targetName[ISCSI_NAME_LENGTH_MAX+1];
  L7_uchar8            initiatorName[ISCSI_NAME_LENGTH_MAX+1];
  L7_uchar8            isid[6];
  L7_uchar8            isid_string[(sizeof(isid)+3)] = "0x";
  L7_clocktime         startTime;         /* time when session first detected */
  L7_uint32            silentTime;        /* number of seconds since activity detected on session */

  (void)iscsiSessionTargetNameGet(i, targetName);
  (void)iscsiSessionInitiatorNameGet(i, initiatorName);
  (void)iscsiSessionIsidGet(i, isid);
  (void)iscsiSessionStartTimeGet(i, &startTime);
  (void)iscsiSessionSilentTimeGet(i, &silentTime);

  iscsiDebugBuffToStr(isid_string+2, isid, sizeof(isid));
  sysapiPrintf("sessionTable[%u].targetName = %s, .initiatorName = %s, .isid = %s, .startTime = %u, .silentTime = %u\n",
               i,
               targetName,
               initiatorName,
               isid_string,
               startTime.seconds,
               silentTime);
}

void iscsiDataDump(void)
{
  L7_int32 i = -1;

  while (iscsiDbSessionIterate(i, &i) == L7_SUCCESS)
  {
    iscsiSessionRecordDump(i);
  }

  i = -1;
  while (iscsiDbConnectionIterate(i, &i) == L7_SUCCESS)
  {
    iscsiConnectionRecordDump(i);
  }
}

void iscsiSessionDataWalk(void)
{
  L7_uint32 sessId;
  L7_uint32 connId;

  if (iscsiSessionFirstGet(&sessId) == L7_SUCCESS)
  {
    do
    {
      iscsiSessionRecordDump(sessId);
      if (iscsiConnectionFirstGet(sessId, &connId) == L7_SUCCESS)
      {
        do
        {
          iscsiConnectionRecordDump(connId);
        } while (iscsiConnectionNextGet(sessId, connId, &connId) == L7_SUCCESS);
        sysapiPrintf("\n");
      }
    } while (iscsiSessionNextGet(sessId, &sessId) == L7_SUCCESS);
  }
  else
  {
    sysapiPrintf("The Session table has no entries.\n");
  }
}

void iscsiTportCfgWalk(void)
{
  L7_uint32 id;
  L7_uint32          portNumber;
  L7_inet_addr_t     ipAddress;
  L7_uchar8          targetIpAddrString[64];     /* leave plenty of room for IPv6 address strings */
  L7_uchar8          targetName[ISCSI_NAME_LENGTH_MAX+1];  /* save room for terminating '\0' */

  if (iscsiTargetTableFirstGet(&id, L7_FALSE) == L7_SUCCESS)
  {
    do
    {
      (void)iscsiTargetTableEntryTcpPortGet(id, &portNumber);
      (void)iscsiTargetTableEntryTargetNameGet(id, targetName);
      (void)iscsiTargetTableEntryIpAddressGet(id, &ipAddress);

      if (osapiInetNtop(ipAddress.family, (L7_uchar8 *)&ipAddress.addr.ipv4.s_addr, targetIpAddrString, sizeof(targetIpAddrString)) == L7_NULLPTR)
        sysapiPrintf("iscsiTargetCfgData[%d]: port=%d, IP=%s, name=%s\n", id, portNumber, "conversion error", targetName);
      else
        sysapiPrintf("iscsiTargetCfgData[%d]: port=%d, IP=%s, name=%s\n", id, portNumber, targetIpAddrString, targetName);

    } while (iscsiTargetTableNextGet(id, &id, L7_FALSE) == L7_SUCCESS);
  }
  else
  {
    sysapiPrintf("The Target Port table has no entries.\n");
  }
}

void iscsiTestConnectionAdd(L7_uchar8 *targetIpAddrString,
                            L7_uint32 targetTcpPort,
                            L7_uchar8 *targetIQNString,
                            L7_uchar8 *initiatorIpAddrString,
                            L7_uint32 initiatorTcpPort,
                            L7_uchar8 *initiatorIQNString,
                            L7_uchar8 *isid,
                            L7_uint32 cid
                            )
{
  L7_inet_addr_t targetIp, initiatorIp;
  L7_uint32 targetIp4AddrH, initiatorIp4AddrH;
  L7_uint32 index;

  (void)inetAddressZeroSet(L7_AF_INET, &targetIp); 
  (void)inetAddressZeroSet(L7_AF_INET, &initiatorIp); 
  targetIp.family = L7_AF_INET;
  initiatorIp.family = L7_AF_INET;
  (void)osapiInetPton(L7_AF_INET , targetIpAddrString, (L7_uchar8 *)&targetIp.addr.ipv4.s_addr);
  (void)osapiInetPton(L7_AF_INET , initiatorIpAddrString, (L7_uchar8 *)&initiatorIp.addr.ipv4.s_addr);

  /* the APIs below take IP addresses as L7_uint32 types in Host byte order, the L7_inet_addr_t is in Network byte order */
  targetIp4AddrH = osapiNtohl(targetIp.addr.ipv4.s_addr);
  initiatorIp4AddrH = osapiNtohl(initiatorIp.addr.ipv4.s_addr);

  if (iscsiTargetTableLookUp(targetTcpPort, targetIp4AddrH, &index) == L7_SUCCESS)
  {
    iscsiConnectionCreate(targetIQNString, initiatorIQNString, isid, 
                          targetIp4AddrH, targetTcpPort, initiatorIp4AddrH, initiatorTcpPort, 
                          cid, index);
  }
  else
  {
    sysapiPrintf("Not configured to monitor target %s:%u\n", targetIpAddrString, targetTcpPort);
  }
}


void iscsiTestConnectionDelete(L7_uchar8 *targetIpAddrString,
                               L7_uint32 targetTcpPort,
                               L7_uchar8 *initiatorIpAddrString,
                               L7_uint32 initiatorTcpPort
                               )
{
  L7_inet_addr_t targetIp, initiatorIp;
  L7_uint32 targetIp4AddrH, initiatorIp4AddrH;
  L7_int32 connectionId;

  (void)inetAddressZeroSet(L7_AF_INET, &targetIp); 
  (void)inetAddressZeroSet(L7_AF_INET, &initiatorIp); 
  targetIp.family = L7_AF_INET;
  initiatorIp.family = L7_AF_INET;
  (void)osapiInetPton(L7_AF_INET , targetIpAddrString, (L7_uchar8 *)&targetIp.addr.ipv4.s_addr);
  (void)osapiInetPton(L7_AF_INET , initiatorIpAddrString, (L7_uchar8 *)&initiatorIp.addr.ipv4.s_addr);

  /* the APIs below take IP addresses as L7_uint32 types in Host byte order, the L7_inet_addr_t is in Network byte order */
  targetIp4AddrH = osapiNtohl(targetIp.addr.ipv4.s_addr);
  initiatorIp4AddrH = osapiNtohl(initiatorIp.addr.ipv4.s_addr);

  connectionId = iscsiConnectionLookup(targetIp4AddrH, targetTcpPort, initiatorIp4AddrH, initiatorTcpPort);
  if (connectionId >= 0)
  {
    iscsiConnectionDelete(connectionId);
  }
  else
  {
    sysapiPrintf("Connection not found.\n");
  }
}

void iscsiTestSessionDelete(L7_uchar8 *targetIpAddrString,
                            L7_uint32 targetTcpPort,
                            L7_uchar8 *initiatorIpAddrString,
                            L7_uint32 initiatorTcpPort
                            )
{
  L7_inet_addr_t targetIp, initiatorIp;
  L7_uint32 targetIp4AddrH, initiatorIp4AddrH;
  L7_int32 sessionId, connectionId;

  (void)inetAddressZeroSet(L7_AF_INET, &targetIp); 
  (void)inetAddressZeroSet(L7_AF_INET, &initiatorIp); 
  targetIp.family = L7_AF_INET;
  initiatorIp.family = L7_AF_INET;
  (void)osapiInetPton(L7_AF_INET , targetIpAddrString, (L7_uchar8 *)&targetIp.addr.ipv4.s_addr);
  (void)osapiInetPton(L7_AF_INET , initiatorIpAddrString, (L7_uchar8 *)&initiatorIp.addr.ipv4.s_addr);

  /* the APIs below take IP addresses as L7_uint32 types in Host byte order, the L7_inet_addr_t is in Network byte order */
  targetIp4AddrH = osapiNtohl(targetIp.addr.ipv4.s_addr);
  initiatorIp4AddrH = osapiNtohl(initiatorIp.addr.ipv4.s_addr);

  connectionId = iscsiConnectionLookup(targetIp4AddrH, targetTcpPort, initiatorIp4AddrH, initiatorTcpPort);
  if (connectionId >= 0)
  {
    if ((sessionId = iscsiDbConnectionSessionIdGet(connectionId)) >= 0)
    {
      iscsiSessionDelete(sessionId);
    }
    else
    {
      sysapiPrintf("Couldn't find associated session record.\n");
    }
  }
  else
  {
    sysapiPrintf("Connection not found.\n");
  }
}


void iscsiSessionDataTestConn(L7_uint32 number)
{
  L7_uint32 targetTcpPort, initiatorTcpPort, index, i, cid;
  L7_inet_addr_t targetIp, initiatorIp;
  L7_uint32 targetIp4AddrH, initiatorIp4AddrH;
  L7_uchar8 targetIpAddrString[64] = "192.168.77.0";
  L7_uchar8 initiatorIpAddrString[64] = "192.168.78.0";
  L7_uchar8 targetIQNName[224];
  L7_uchar8 initiatorIQNName[224];
  L7_uchar8 isid[6] = {0x00, 0xab, 0x0c, 0xd0, 0x0e, 0x00};


  (void)inetAddressZeroSet(L7_AF_INET, &targetIp); 
  (void)inetAddressZeroSet(L7_AF_INET, &initiatorIp); 
  targetIp.family = L7_AF_INET;
  initiatorIp.family = L7_AF_INET;
  (void)osapiInetPton(L7_AF_INET , targetIpAddrString, (L7_uchar8 *)&targetIp.addr.ipv4.s_addr);
  (void)osapiInetPton(L7_AF_INET , initiatorIpAddrString, (L7_uchar8 *)&initiatorIp.addr.ipv4.s_addr);

  /* the APIs below take IP addresses as L7_uint32 types in Host byte order, the L7_inet_addr_t is in Network byte order */
  targetIp4AddrH = osapiNtohl(targetIp.addr.ipv4.s_addr);
  initiatorIp4AddrH = osapiNtohl(initiatorIp.addr.ipv4.s_addr);

  targetTcpPort = 3260;
  initiatorTcpPort = 2300;
  cid = 77;

  iscsiTargetTableLookUp(targetTcpPort, 0, &index);
  for (i=0; i<number; i++)
  {
    isid[5] += i;
    targetIp4AddrH++;
    initiatorIp4AddrH++;
    initiatorTcpPort++;
    osapiSnprintf(targetIQNName, sizeof(targetIQNName), "iqn:target_%u", i);
    osapiSnprintf(initiatorIQNName, sizeof(initiatorIQNName), "iqn:initiator_%u", i);

    iscsiConnectionCreate(targetIQNName, initiatorIQNName, isid, 
                          targetIp4AddrH, targetTcpPort, 
                          initiatorIp4AddrH, initiatorTcpPort, 
                          cid, index);
  }
}

void iscsiTportAdd(L7_uint32 tcpPort,
                   L7_uchar8 *ipAddrString,
                   L7_uchar8 *targetName)
{
  L7_inet_addr_t targetIpN;

  (void)inetAddressZeroSet(L7_AF_INET, &targetIpN); 
  (void)osapiInetPton(L7_AF_INET , ipAddrString, (L7_uchar8 *)&targetIpN.addr.ipv4.s_addr);

  if (iscsiTargetTcpPortAdd(tcpPort, &targetIpN, targetName) == L7_SUCCESS)
  {
    sysapiPrintf("Target port added.\n");
  }
  else
  {
    sysapiPrintf("Error.\n");
  }
}

void iscsiTportDelete(L7_uint32 tcpPort,
                      L7_uchar8 *ipAddrString)
{
  L7_inet_addr_t targetIp;

  (void)inetAddressZeroSet(L7_AF_INET, &targetIp); 
  (void)osapiInetPton(L7_AF_INET , ipAddrString, (L7_uchar8 *)&targetIp.addr.ipv4.s_addr);

  if (iscsiTargetTcpPortDelete(tcpPort, &targetIp) == L7_SUCCESS)
      sysapiPrintf("Target port deleted.\n");
    else
      sysapiPrintf("Error attempting to delete.\n");
  }

/*********************************************************************
* @purpose DUMP configuration structures 
*
* @param   none
*
* @returns void
*
* @notes    none
*
* @end
*********************************************************************/
void iscsiDebugCfgShow()
{
  L7_uint32 i;
  L7_uchar8 *selector_text;

  sysapiPrintf("Dumping iSCSI configuration:\n");
  if (iscsiCfgData == L7_NULLPTR)
  {
    sysapiPrintf("  iscsiCfgData pointer is NULL\n");
    return;
  }

  sysapiPrintf(" iSCSI admin mode = %s\n", iscsiCfgData->adminMode?"enabled":"disabled");
  sysapiPrintf(" session time out interval = %d seconds\n", iscsiCfgData->sessionTimeOutInterval);
  switch (iscsiCfgData->tagFieldSelector)
  {
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    selector_text = "VLAN priority tag";
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    selector_text = "IP precedence";
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    selector_text = "IP DSCP";
    break;
  default:
    selector_text = "unknown value";
  }
  sysapiPrintf(" tag selector = %s\n", selector_text);
  sysapiPrintf(" VLAN priority value = %d\n", iscsiCfgData->vlanPriorityValue);
  sysapiPrintf(" DSCP value = %d\n", iscsiCfgData->dscpValue);
  sysapiPrintf(" IP precedence value = %d\n", iscsiCfgData->precValue);
  sysapiPrintf(" marking enabled = %s\n", iscsiCfgData->markingEnabled?"enabled":"disabled");

  for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
  {
    if (iscsiCfgData->iscsiTargetCfgData[i].inUse == L7_TRUE)
    {
      sysapiPrintf(" target port[%d]: port=%d addr=%8.8x name='%s' isDefault=%s isDeleted=%s\n", 
                   i, 
                   iscsiCfgData->iscsiTargetCfgData[i].portNumber, 
                   iscsiCfgData->iscsiTargetCfgData[i].ipAddress,  
                   iscsiCfgData->iscsiTargetCfgData[i].targetName,
                   iscsiCfgData->iscsiTargetCfgData[i].defaultCfgEntry?"TRUE":"FALSE",
                   iscsiCfgData->iscsiTargetCfgData[i].deletedDefaultCfgEntry?"TRUE":"FALSE");
    }
  }

  sysapiPrintf(" iSCSI Flow Acceleration resource use:\n");
  sysapiPrintf("   porting constants:\n");
  sysapiPrintf("     L7_ISCSI_MAX_TARGET_TCP_PORTS = %u\n", L7_ISCSI_MAX_TARGET_TCP_PORTS);
  sysapiPrintf("     L7_ISCSI_MAX_SESSIONS         = %u\n", L7_ISCSI_MAX_SESSIONS);
  sysapiPrintf("     L7_ISCSI_MAX_CONNECTIONS      = %u\n", L7_ISCSI_MAX_CONNECTIONS);
  sysapiPrintf("   memory allocated at PHASE_1_INIT:\n");
  sysapiPrintf("     sizeof(iscsiCfgData)       = %u bytes\n", L7_ISCSI_CFG_DATA_SIZE);
  sysapiPrintf("     message queue              = %u bytes (ISCSI_MSG_COUNT = %u, ISCSI_MSG_SIZE = %u)\n", 
                          ISCSI_MSG_COUNT*ISCSI_MSG_SIZE,
                          ISCSI_MSG_COUNT,
                          ISCSI_MSG_SIZE);
  sysapiPrintf("     'iSCSI pkt buf' bufferPool = %u bytes (buffer count = %u, buffer size = %u)\n", 
                          ISCSI_PACKET_BUFFER_SIZE*ISCSI_PACKET_BUFFER_COUNT,
                          ISCSI_PACKET_BUFFER_COUNT,
                          ISCSI_PACKET_BUFFER_SIZE);
}
