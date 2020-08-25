/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_debug.c
*
* @purpose   DHCP snooping 
*
* @component DHCP snooping
*
* @comments none
*
* @create 4/6/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/


#define L7_MAC_ENET_BCAST

#include "l7_common.h"
#include "osapi.h"
#include "log.h" 
#include "nimapi.h"
#include "comm_mask.h"
#include "l7_packet.h"

#include "ds_util.h"
#include "ds_cfg.h"
#include "ds_cnfgr.h"

extern dsCfgData_t *dsCfgData;
extern dsCnfgrState_t dsCnfgrState;
extern osapiRWLock_t dsCfgRWLock;


/* Global status data */
extern dsInfo_t *dsInfo;

/* Array of per interface working data. Indexed on internal interface number */
extern dsIntfInfo_t *dsIntfInfo;

extern L7_uchar8 *dsInitStateNames[];

extern void *Ds_Event_Queue;
extern void *Ds_Packet_Queue;
extern L7_uchar8 DS_DHCP_OPTIONS_COOKIE[];



/*********************************************************************
* @purpose  Print debug stats to console
*
* @param    void
*
* @returns  void
*
* @notes    for debugging
*
* @end
*********************************************************************/
void dsDebugStats(void)
{
  L7_uint32 i;

  printf("\n\nDHCP Snooping debug stats...");
  printf("\n%-50s  %12u", 
         "Error receiving from DHCP snooping message queues", 
         dsInfo->debugStats.msgRxError);
  printf("\n%-50s  %12u",
         "Failure putting message on frame message queue", 
         dsInfo->debugStats.frameMsgTxError);
  printf("\n%-50s  %12u",
         "Failure putting message on event message queue", 
         dsInfo->debugStats.eventMsgTxError);
  printf("\n%-50s  %12u",
         "Illegal source IP address in snooped packet", 
         dsInfo->debugStats.badSrcAddr);
  printf("\n%-50s  %12u", 
         "Number of DHCP messages dropped as control plane is not ready", 
         dsInfo->debugStats.msgsDroppedControlPlaneNotReady);
  printf("\n%-50s  %12u",
         "Number of DHCP messages intercepted", 
         dsInfo->debugStats.msgsIntercepted);
  printf("\n%-50s  %12u",
         "Number of DHCP messages processed", 
         dsInfo->debugStats.msgsReceived);
  printf("\n%-50s  %12u",
         "Number of DHCP messages filtered", 
         dsInfo->debugStats.msgsFiltered);
  printf("\n%-50s  %12u",
         "Number of DHCP messages forwarded", 
         dsInfo->debugStats.msgsForwarded);
  printf("\n%-50s  %12u",
         "Number of DHCP message tx failures", 
         dsInfo->debugStats.txFail);
  printf("\n%-50s  %12u",
         "Rx REL or DECL from client not in bindings db", 
         dsInfo->debugStats.releaseDeclineUnkClient);
  printf("\n%-50s  %12u",
         "Failure adding binding to bindings table", 
         dsInfo->debugStats.bindingAddFail);
  printf("\n%-50s  %12u",
         "Failure updating binding in bindings table", 
         dsInfo->debugStats.bindingUpdateFail);
  printf("\n%-50s  %12u", 
         "Number of bindings added to bindings table", 
         dsInfo->debugStats.bindingsAdded);
  printf("\n%-50s  %12u", 
         "Number of bindings removed from bindings table", 
         dsInfo->debugStats.bindingsRemoved);

  printf("\n%-50s  %12u", 
         "Number of DHCP messages dropped as static binding already exists", 
         dsInfo->debugStats.msgDroppedBindingExists);
  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
    if (dsInfo->debugStats.msgsInterceptedIntf[i])
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 msgStr[128];
      nimGetIntfName(i, L7_SYSNAME, ifName);
      osapiSnprintf(msgStr, 127, "Number of messages intercepted on interface %s",
                    ifName);
      printf("\n%-50s  %12u",
             msgStr, dsInfo->debugStats.msgsInterceptedIntf[i]);
    }
  }
  printf("\n");
}

/*********************************************************************
* @purpose  Clear debug stats.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsDebugStatsClear(void)
{
  memset(&dsInfo->debugStats, 0, sizeof(dsInfo->debugStats));
}

/*********************************************************************
* @purpose  Validate the config and state of the DHCP snooping component. 
*
* @param    void
*
* @returns  L7_SUCCESS if config and state are valid.
*           L7_ERROR if config or state is not valid.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsValidate(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t retval;
  L7_BOOL vlanEnabled;   /* L7_TRUE if one or more VLANs enabled for DHCP snooping */
  L7_BOOL intfUntrusted;   /* L7_TRUE if one or more interfaces are trusted */
  L7_INTF_MASK_t invTrust;  /* inverse of trust config */
  L7_uint32 intIfNum;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (dsCnfgrState != DHCP_SNOOP_PHASE_EXECUTE)
  {
    printf("\nDHCP snooping is in init state %s", dsInitStateNames[dsCnfgrState]);
    osapiReadLockGive(dsCfgRWLock);
    return L7_ERROR;
  }

  if ((dsCfgData->dsGlobalAdminMode != L7_ENABLE) &&
      (dsCfgData->dsGlobalAdminMode != L7_DISABLE))
  {
    printf("\nDHCP Snooping global admin mode is invalid");
    rc = L7_ERROR;
  }

  for (intIfNum = 1; intIfNum < DS_MAX_INTF_COUNT; intIfNum++)
  {
    /* Check VLAN count on all interfaces. */
    L7_ushort16 vlanCount = dsIntfInfo[intIfNum].dsNumVlansEnabled;
    if (vlanCount > L7_PLATFORM_MAX_VLAN_ID)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      printf("\nVLAN count %u on interface %s is invalid.", vlanCount, ifName);
      rc = L7_ERROR;
    }

    /* Warn if trusted interface configured to log invalid packets. Harmless, 
     * but useless, since packets are not checked on trusted ports. */
    if (_dsIntfLogInvalidGet(intIfNum) && _dsIntfTrustGet(intIfNum))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      printf("\nInterface %s is trusted and configured to log invalid packets.", 
             ifName);
    }
  }

  /* Warn if disabled globally, but enabled on a VLAN. */
  L7_VLAN_NONZEROMASK(dsCfgData->dsVlans, vlanEnabled);
  if ((dsCfgData->dsGlobalAdminMode == L7_DISABLE) && vlanEnabled)
  {
    printf("\nDHCP snooping is disabled globally but enabled on one or more VLANS");
  }

  /* Warn if enabled globally, but not enabled on any VLAN. */
  if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) && !vlanEnabled)
  {
    printf("\nDHCP snooping is enabled globally but not enabled on an VLAN");
  }

  /* Warn if enabled globally, but no interfaces are untrusted. */
  L7_INTF_MASKEQ(invTrust, dsCfgData->dsTrust);    /* make copy of trust config */
  L7_INTF_MASKINV(invTrust);                       /* invert */
  L7_INTF_NONZEROMASK(invTrust, intfUntrusted);
  if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) && !intfUntrusted)
  {
    printf("\nDHCP snooping is enabled but all interfaces are trusted");
  }
  L7_INTF_MASKEQ(invTrust, dsCfgData->dsTrust);    /* make copy of trust config */
  L7_INTF_NONZEROMASK(invTrust, intfUntrusted);
  /* Warn if enbaled globally , but no trusted ports are there */
  if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) &&
       (vlanEnabled)&&
       (!intfUntrusted) ) 
  {
    printf("\nDHCP snooping is enabled but all interfaces are untrusted,DHCP messages will not go out");
  }

  /* Warn if a non-tentative binding has no IP address, or a tentative
   * binding has an IP address. */
  retval = dsBindingsValidate();
  if (retval != L7_SUCCESS)
  {
    rc = retval;
  }
  
  osapiReadLockGive(dsCfgRWLock);
  if (rc == L7_SUCCESS)
  {
    printf("\nDHCP snooping config and state is valid.");
  }
  else
  {
    printf("\nDHCP snooping configuration or state is invalid.");
  }

  return rc;
}

/*********************************************************************
* @purpose  Write a DHCP snooping trace message.
*
* @param    void
*
* @returns  L7_SUCCESS if config and state are valid.
*           L7_ERROR if config or state is not valid.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsTraceWrite(L7_uchar8 *dsTrace)
{
  L7_uint32 stackUptime;
  L7_uint32 secs;
  L7_uint32 msecs;
  L7_uchar8 debugMsg[DS_MAX_TRACE_LEN];
  L7_int32 eventQLen = -1, pktQLen = -1;

  if (dsTrace == NULL)
    return L7_SUCCESS;

  stackUptime = simSystemUpTimeMsecGet();
  secs = stackUptime / 1000;
  msecs = stackUptime % 1000;

  if (osapiMsgQueueGetNumMsgs((void*)Ds_Event_Queue, &eventQLen) != L7_SUCCESS)
  {
    eventQLen = -1;
  }
  if (osapiMsgQueueGetNumMsgs((void*)Ds_Packet_Queue, &pktQLen) != L7_SUCCESS)
  {
    pktQLen = -1;
  }

  osapiSnprintf(debugMsg, DS_MAX_TRACE_LEN, "\n%d.%03d (%d,%d) %s",
                secs, msecs, eventQLen, pktQLen, dsTrace);
  printf("%s\r\n", debugMsg);
  return L7_SUCCESS;
}

void dsTraceFlagsSet(L7_uint32 dsTraceFlags)
{
  dsCfgData->dsTraceFlags = dsTraceFlags;
}

/* 14 is eth header. 3 is DHCP msg type option. 6 is lease time option */
#define TEST_MSG_LEN (14 + L7_IP_HDR_LEN + L7_UDP_HDR_LEN + L7_DHCP_HDR_LEN + L7_DHCP_MAGIC_COOKIE_LEN + 3 + 6)

/*********************************************************************
* @purpose  Inject a DHCP packet for testing purposes
*
* @param    void
*
* @returns  L7_SUCCESS if successful
*
* @notes    Puts the packet on the DHCP snooping message queue.
*
* @end
*********************************************************************/
L7_RC_t dsTestPacket(L7_uint32 rxIntf,
                     L7_ushort16 vlanId,
                     L7_uchar8 *clientHwAddr,
                     L7_uint32 clientIpAddr,
                     L7_BOOL fromClient, 
                     L7_dhcp_pkt_type_t pktType)
{
  L7_uchar8 frame[TEST_MSG_LEN];
  L7_uchar8 *pos = frame;
  L7_enetMacAddr_t clientMac;
  L7_enetMacAddr_t srcMac;
  L7_enetMacAddr_t destMac;
  L7_enetMacAddr_t serverMac = {{0x00, 0x00, 0x0a, 0x01, 0x01, 0xfe}};
  L7_ushort16 etype = osapiHtons(L7_ETYPE_IP);
  L7_ipHeader_t *ipHeader;
  L7_uint32 serverIpAddr = 0x0A0101FE;        /* 10.1.1.254 */
  L7_uint32 srcIpAddr;
  L7_uint32 destIpAddr;
  L7_ushort16 srcUdpPort, sVlanId = L7_NULL;
  L7_ushort16 destUdpPort;
  L7_udp_header_t *udpHeader;
  L7_dhcp_packet_t *dhcpHeader;
  L7_uchar8 op;
  L7_uint32 yiaddr;
  L7_uint32 siaddr;
  L7_RC_t rc;
  L7_uint32 leaseTime;

  if (dsStringToMac(clientHwAddr, &clientMac) != L7_SUCCESS)
    return L7_FAILURE;

  /* Figure out source and destination addresses */
  if (fromClient)
  {
    memcpy(srcMac.addr, clientMac.addr, L7_ENET_MAC_ADDR_LEN);
    memcpy(destMac.addr, &L7_ENET_BCAST_MAC_ADDR, L7_ENET_MAC_ADDR_LEN);
    srcIpAddr = 0;
    destIpAddr = 0xFFFFFFFF;
    srcUdpPort = UDP_PORT_BOOTP_CLNT;
    destUdpPort = UDP_PORT_DHCP_SERV;
    op = L7_DHCP_BOOTP_REQUEST;
    yiaddr = 0;
    siaddr = 0;
  }
  else
  {
    memcpy(srcMac.addr, serverMac.addr, L7_ENET_MAC_ADDR_LEN);
    memcpy(destMac.addr, clientMac.addr, L7_ENET_MAC_ADDR_LEN);
    srcIpAddr = serverIpAddr;
    destIpAddr = clientIpAddr;
    srcUdpPort = UDP_PORT_DHCP_SERV;
    destUdpPort = UDP_PORT_BOOTP_CLNT;
    op = L7_DHCP_BOOTP_REPLY;
    yiaddr = clientIpAddr;
    siaddr = serverIpAddr;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* build ethernet header */
  
  memcpy(pos, destMac.addr, L7_ENET_MAC_ADDR_LEN);
  pos += L7_ENET_MAC_ADDR_LEN;
  memcpy(pos, srcMac.addr, L7_ENET_MAC_ADDR_LEN);
  pos += L7_ENET_MAC_ADDR_LEN;
  memcpy(pos, &etype, sizeof(etype));
  pos += sizeof(etype);

  /* IP header */
  ipHeader = (L7_ipHeader_t*) pos;
  ipHeader->iph_versLen = 0x45;
  ipHeader->iph_tos = 0;
  ipHeader->iph_len = osapiHtons(TEST_MSG_LEN - 14);
  ipHeader->iph_ident = 1;
  ipHeader->iph_flags_frag = 0;
  ipHeader->iph_ttl = 64;
  ipHeader->iph_prot = IP_PROT_UDP;
  ipHeader->iph_csum = 0;               /* won't be checked; so don't bother */
  ipHeader->iph_src = osapiHtonl(srcIpAddr);
  ipHeader->iph_dst = osapiHtonl(destIpAddr);
  pos += L7_IP_HDR_LEN;

  /* UDP Header */
  udpHeader = (L7_udp_header_t*) pos;
  udpHeader->sourcePort = osapiHtons(srcUdpPort);     
  udpHeader->destPort = osapiHtons(destUdpPort);       
  udpHeader->length = osapiHtons(L7_UDP_HDR_LEN + L7_DHCP_HDR_LEN + L7_DHCP_MAGIC_COOKIE_LEN + 3);
  udpHeader->checksum = 0;
  pos += L7_UDP_HDR_LEN;

  /* DHCP header */
  dhcpHeader = (L7_dhcp_packet_t*) pos;
  dhcpHeader->op = op;      
  dhcpHeader->htype = L7_DHCP_HTYPE_ETHER;   
  dhcpHeader->hlen = L7_ENET_MAC_ADDR_LEN;    
  dhcpHeader->hops = 0;    
  dhcpHeader->xid = osapiHtonl(0xAAAAAAAA);     
  dhcpHeader->secs = osapiHtons(0);    
  dhcpHeader->flags = 0;   
  dhcpHeader->ciaddr = osapiHtonl(0);  
  dhcpHeader->yiaddr = osapiHtonl(yiaddr);  
  dhcpHeader->siaddr = osapiHtonl(siaddr);  
  dhcpHeader->giaddr = 0;  
  memcpy(dhcpHeader->chaddr, clientMac.addr, L7_ENET_MAC_ADDR_LEN);
  pos += L7_DHCP_HDR_LEN;

  /* Magic cookie */
  memcpy(pos, &DS_DHCP_OPTIONS_COOKIE, L7_DHCP_MAGIC_COOKIE_LEN);
  pos += L7_DHCP_MAGIC_COOKIE_LEN;

  /* DHCP packet type */
  *pos = DHO_DHCP_MESSAGE_TYPE;
  pos++;
  *pos = 1;
  pos++;
  *pos = pktType;
  pos++;

  /* lease time*/
  *pos = DHO_DHCP_LEASE_TIME;
  pos++;
  *pos = 4;
  pos++;
  leaseTime = osapiHtonl(1440);     /* 1 day */
  memcpy(pos, &leaseTime, 4);

  printf("\nqueueing test packet to DHCP snooping");
  L7_uint client_idx = (L7_uint)-1;
  rc = dsPacketQueue(frame, TEST_MSG_LEN, vlanId, rxIntf, sVlanId, &client_idx );

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}
/*********************************************************************
* @purpose  displays the Subscription config of the DHCP L2 Relay component. 
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsDebugIntfSubscriptionDump()
{
  L7_uint32 intIfNum = 0, sIndex = 0;
  /* Subscription table.*/
  printf("\n IntIfNum  SubscriptionName  L2RelayMode   CidMode    RIdValue");
  printf("\n -------------------------------------------------------------");
  for (intIfNum = 1; intIfNum < DS_MAX_INTF_COUNT; intIfNum++)
  {
    for (sIndex = 0; sIndex < L7_DOT1AD_SERVICES_MAX_PER_INTF; sIndex++)
    {
      dsL2RelaySubscriptionCfg_t  *subscCfg = &( dsCfgData->dsIntfCfg[intIfNum].dsL2RelaySubscrptionCfg[sIndex]);
      printf("\n %d  %s  %s  %s  %s ", intIfNum, 
             subscCfg->subscriptionName,
             (subscCfg->l2relay == L7_ENABLE) ? "Enabled" : "Disabled",
             (subscCfg->circuitId == L7_ENABLE) ? "Enabled" : "Disabled",
             subscCfg->remoteId);
    }
  }
}

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  displays the config and state of the DHCP L2 Relay component. 
*
* @param    void
*
* @returns  L7_SUCCESS if config and state are valid.
*           L7_ERROR if config or state is not valid.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayCfgDump(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8  rIdStr[DS_MAX_REMOTE_ID_STRING];
  L7_BOOL relayMode, cIdMode, rIdMode; 
  L7_uint32 intIfNum, vlanId;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (dsCnfgrState != DHCP_SNOOP_PHASE_EXECUTE)
  {
    printf("\nDHCP L2 Relay is in init state %s", dsInitStateNames[dsCnfgrState]);
    osapiReadLockGive(dsCfgRWLock);
    return L7_ERROR;
  }

  if ((dsCfgData->dsL2RelayAdminMode != L7_ENABLE) &&
      (dsCfgData->dsL2RelayAdminMode != L7_DISABLE))
  {
    printf("\nDHCP L2 Relay global admin mode is invalid");
    rc = L7_ERROR;
  }
  printf("\n DHCP Admin Mode : %s", dsCfgData->dsL2RelayAdminMode == L7_ENABLE ? "Enabled" : "Disabled");


  printf("\n Iface    L2RelayMode  TrustMode ");
  printf("\n ----------------------------------");
  for (intIfNum = 1; intIfNum < DS_MAX_INTF_COUNT; intIfNum++)
  {
    printf("\n %d   %s   %s", intIfNum,
           (_dsIntfL2RelayGet(intIfNum) == L7_TRUE) ? "Enabled" : "Disabled" ,
           (_dsIntfL2RelayTrustGet(intIfNum) == L7_TRUE) ? "Trusted" : "Distrusted" );
  }

  printf("\n VlanId   L2RelayMode   CidMode    RIdValue");
  printf("\n ------------------------------------------");
  for (vlanId = 1; vlanId < L7_PLATFORM_MAX_VLAN_ID; vlanId++)
  {
    relayMode = _dsVlanL2RelayGet(vlanId);
    cIdMode = _dsVlanL2RelayCircuitIdGet(vlanId);
    rIdMode = _dsVlanL2RelayRemoteIdGet(vlanId);
    osapiStrncpy(rIdStr, "--NULL--", DS_MAX_REMOTE_ID_STRING);
    if (relayMode == L7_FALSE && cIdMode == L7_FALSE && rIdMode == L7_FALSE)
    {
      continue;
    }
    if (rIdMode == L7_TRUE)
    {
      osapiStrncpy(rIdStr, dsCfgData->dsL2RelayRemoteId[vlanId], DS_MAX_REMOTE_ID_STRING);
    }
    printf("\n %d   %s   %s   %s ", vlanId,
           (relayMode == L7_TRUE) ? "Enabled" : "Disabled" ,
           (cIdMode == L7_TRUE) ? "Enabled" : "Disabled", 
           rIdStr);
  }

  dsDebugIntfSubscriptionDump();
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}
#endif

