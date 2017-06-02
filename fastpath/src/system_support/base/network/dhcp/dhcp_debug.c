/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   dhcp_debug.c
*
* @purpose    
*
* @component  DHCP Client
*
* @comments   none
*
* @create     
*
* @author     ckrishna
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "dhcp_debug.h"
#include "l7_dhcp.h"
#include "sysapi.h"
#include "l7_ip_api.h"
#include "osapi.h"
#include "osapi_support.h"

/*******************************************************************************
**                        General Definitions                                 **
*******************************************************************************/
#define DHCPC_NUM_FLAG_BYTES     ((DHCPC_DEBUG_LAST + 7) / 8)

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
static L7_uchar8 dhcpClientDebugFlags[DHCPC_NUM_FLAG_BYTES];
static L7_BOOL dhcpClientIsDebugEnabled = L7_FALSE;

/* Just for Debugging purpose to print the States ...
 * This has to be in sync with L7_DHCP_CLIENT_STATE_t.
 */
static L7_uchar8 *dhcpDebugState[] =
{
  "...",
  "Init",
  "Selecting",
  "Requesting",
  "Request Receive",
  "Bound",
  "Renewing",
  "Renew Receive",
  "Rebinding",
  "Rebing Receive",
  "BootP Fallback",
  "Not Bound",
  "Failed",
  "Release",
  "Inform Request",
  "Inform Ack Wait",
  "Inform Failed",
  "Inform Bound"
};

/* Just for Debugging purpose to print the Management Port Type ...
 * This has to be in sync with L7_MGMT_PORT_TYPE_t.
 */
L7_uchar8 *dhcpDebugMgmtPortType[] =
{
  "Service Port",
  "Network Port",
  "Router Interface"
};

/* Following functions have been copied from Redhat/eCos bootp_support.c */
static L7_char8 *_bootp_op[] = {"", "REQUEST", "REPLY"};
static L7_char8 *_bootp_hw_type[] = {"", "Ethernet", "Exp Ethernet", "AX25",
  "Pronet", "Chaos", "IEEE802", "Arcnet"};

static L7_char8 *_dhcpmsgs[] = {"","DISCOVER", "OFFER", "REQUEST", "DECLINE",
  "ACK", "NAK", "RELEASE"};

/*********************************************************************
*
* @purpose
*
* @param
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL
dhcpClientDebugFlagCheck (DHCPC_TRACE_LVL_t traceLevel)
{
  if (dhcpClientIsDebugEnabled != L7_TRUE)
    return L7_FALSE;

  if (traceLevel >= DHCPC_DEBUG_LAST)
    return L7_FALSE;

  if ((dhcpClientDebugFlags[traceLevel/8] & (1 << (traceLevel % 8))) != 0)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing in DHCP CLIENT.
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugEnable (void)
{
  if (dhcpClientIsDebugEnabled == L7_TRUE)
  {
    DHCPC_DEBUG_PRINTF ("\nDHCP CLIENT Debug Tracing is already Enabled.\n");
    return (L7_FAILURE);
  }

  dhcpClientIsDebugEnabled = L7_TRUE;
  /*DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is Enabled.\n");*/

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing in DHCP CLIENT
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugDisable (void)
{
  if (dhcpClientIsDebugEnabled != L7_TRUE)
  {
    DHCPC_DEBUG_PRINTF ("\nDHCP CLIENT Debug Tracing is already Disabled.\n");
    return (L7_FAILURE);
  }

  dhcpClientIsDebugEnabled = L7_FALSE;
  /*DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is Disabled.\n");*/

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes    This command sets all the trace levels of debug tracing except
*           DHCPC_DEBUG_PKT_SRVC_TX and DHCPC_DEBUG_PKT_SRVC_RX.
*           These two flags will be set when the user issues
*           "debug dhcp packet" command.
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugAllSet (void)
{
  L7_uint32 traceLevel;
  for (traceLevel = 0; traceLevel < (DHCPC_DEBUG_LAST - 2); traceLevel++)
  dhcpClientDebugFlags[traceLevel/8] |= (1 << (traceLevel % 8));
  DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is Enabled for All Trace Levels.\n");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS if Debug trace was successfully disabled.
* @returns  L7_FAILURE if there was an error disabling Debug Trace.
*
* @notes    This command resets all the trace levels of debug tracing except
*           DHCPC_DEBUG_PKT_SRVC_TX and DHCPC_DEBUG_PKT_SRVC_RX.
*           These two flags will be reset when the user issues
*           "no debug dhcp packet" command.
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugAllReset (void)
{
  L7_uint32 traceLevel;
  for (traceLevel = 0; traceLevel < (DHCPC_DEBUG_LAST - 2); traceLevel++)
  dhcpClientDebugFlags[traceLevel/8] &= (~(1 << (traceLevel % 8)));
  DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is Disabled for All Trace Levels.\n");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific Trace Level
*
* @param    traceLevel @b{ (input) } Trace Level to enable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugFlagSet (DHCPC_TRACE_LVL_t traceLevel)
{
  if (traceLevel >= DHCPC_DEBUG_LAST)
  {
    DHCPC_DEBUG_PRINTF ("Invalid DHCP CLIENT Trace Level.\n");
    return L7_FAILURE;
  }

  dhcpClientDebugFlags[traceLevel/8] |= (1 << (traceLevel % 8));
  /*DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is enabled for Trace Level - %d.\n",
                      traceLevel);*/

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific Trace Level
*
* @param    traceLevel @b{ (input) } Trace Level to disable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
dhcpClientDebugFlagReset (DHCPC_TRACE_LVL_t traceLevel)
{
  if (traceLevel >= DHCPC_DEBUG_LAST)
  {
    DHCPC_DEBUG_PRINTF ("Invalid DHCP CLIENT Trace Level.\n");
    return L7_FAILURE;
  }

  dhcpClientDebugFlags[traceLevel/8] &= (~(1 << (traceLevel % 8)));
  /*DHCPC_DEBUG_PRINTF ("DHCP CLIENT Debug Tracing is disabled for Trace Level - %d.\n",
                      traceLevel);*/

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Dump Client Intf Info contents
*
* @param    traceLevel @b{ (input) } Trace Level to disable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
void
dhcpClientDebugClientIntfInfoShow (L7_uint32 rtrIfNum,
                                   L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 intIfNum = 0;
  L7_char8 buf[OSAPI_INET_NTOA_BUF_SIZE];

  if ((mgmtPortType != L7_MGMT_SERVICEPORT) &&
      (mgmtPortType != L7_MGMT_NETWORKPORT) &&
      (mgmtPortType != L7_MGMT_IPPORT))
  {
    DHCPC_DEBUG_PRINTF ("Invalid mgmtPortType - %d.\n", mgmtPortType);
    DHCPC_DEBUG_PRINTF ("Use 0 for Service port, 1 for Network Port, 2 for Inband Interface.\n");
    return;
  }

  if (mgmtPortType == L7_MGMT_IPPORT)
  {
#ifdef L7_ROUTING_PACKAGE
    if (ipMapRtrIntfToIntIfNum (rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      DHCPC_DEBUG_PRINTF ("Invalid rtrIfNum - %d.\n", rtrIfNum);
      return;
    }
#else /* L7_ROUTING_PACKAGE */
    DHCPC_DEBUG_PRINTF ("Routing Package is not supported in this build.\n");
    return;
#endif /* L7_ROUTING_PACKAGE */
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_DEBUG_PRINTF ("Client Intf Info Get Failed for rtrIfNum-%d, "
                        "mgmtPortType-%d", rtrIfNum, mgmtPortType);
    return;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_DEBUG_PRINTF ("DHCP Client is not Active on this Interface.\n");
    return;
  }

  DHCPC_DEBUG_PRINTF ("=============================================================\n");
  if (mgmtPortType == L7_MGMT_IPPORT)
  {
    DHCPC_DEBUG_PRINTF ("DHCP Client Information for Router Interface - %d.\n", rtrIfNum);
  }
  else
  {
    DHCPC_DEBUG_PRINTF ("DHCP Client Information for %s.\n", dhcpDebugMgmtPortType[mgmtPortType]);
  }
  DHCPC_DEBUG_PRINTF ("=============================================================\n");

  DHCPC_DEBUG_PRINTF ("sockFD - %d.\n", clientIntfInfo->sockFD);
  DHCPC_DEBUG_PRINTF ("intIfNum - %d.\n", clientIntfInfo->intIfNum);
  DHCPC_DEBUG_PRINTF ("mgmtPortType - %d %s.\n", clientIntfInfo->mgmtPortType, dhcpDebugMgmtPortType[clientIntfInfo->mgmtPortType]);
  DHCPC_DEBUG_PRINTF ("intfName - '%s'.\n", clientIntfInfo->intfName);

  DHCPC_DEBUG_PRINTF ("macAddr - '");
  DHCPC_DEBUG_PRINTF ("%02X:%02X:%02X:%02X:%02X:%02X'.\n",
                      clientIntfInfo->macAddr[0], clientIntfInfo->macAddr[1],
                      clientIntfInfo->macAddr[2], clientIntfInfo->macAddr[3],
                      clientIntfInfo->macAddr[4], clientIntfInfo->macAddr[5]);
  DHCPC_DEBUG_PRINTF ("transID - 0x%x.\n", clientIntfInfo->transID);
  DHCPC_DEBUG_PRINTF ("Leased IP Address - %s.\n",
                      dhcpClientDebugAddrPrint(clientIntfInfo->dhcpInfo.offeredIpAddr.s_addr, buf));
  DHCPC_DEBUG_PRINTF ("Gateway Address - %s.\n",
                      dhcpClientDebugAddrPrint(clientIntfInfo->txBuf.bp_giaddr.s_addr, buf));
  DHCPC_DEBUG_PRINTF ("Server Address - %s.\n",
                      dhcpClientDebugAddrPrint(clientIntfInfo->dhcpInfo.server_ip.s_addr, buf));
  DHCPC_DEBUG_PRINTF ("Received From - %s.\n",
                      dhcpClientDebugAddrPrint(clientIntfInfo->rxAddr.sin_addr.s_addr, buf));
  DHCPC_DEBUG_PRINTF ("dhcpState - %d %s.\n", clientIntfInfo->dhcpState, dhcpDebugState[clientIntfInfo->dhcpState]);

  DHCPC_DEBUG_PRINTF ("Lease Info ....\n");
  DHCPC_DEBUG_PRINTF ("   T1 - %d secs.\n", clientIntfInfo->leaseInfo.t1);
  DHCPC_DEBUG_PRINTF ("   T2 - %d secs.\n", clientIntfInfo->leaseInfo.t2);
  DHCPC_DEBUG_PRINTF ("   Expiry - %d secs.\n", clientIntfInfo->leaseInfo.expiry);
  DHCPC_DEBUG_PRINTF ("   Net T1 - %d secs.\n", clientIntfInfo->leaseInfo.net_t1);
  DHCPC_DEBUG_PRINTF ("   Net T2 - %d secs.\n", clientIntfInfo->leaseInfo.net_t2);
  DHCPC_DEBUG_PRINTF ("   Net Expiry - %d secs.\n", clientIntfInfo->leaseInfo.net_expiry);

  DHCPC_DEBUG_PRINTF ("retryCount - %d.\n", clientIntfInfo->retryCount);
  DHCPC_DEBUG_PRINTF ("bytesRcvd - %d.\n", clientIntfInfo->bytesRcvd);
  DHCPC_DEBUG_PRINTF ("dhcpSelectingStateCounter - %d.\n", clientIntfInfo->dhcpSelectingStateCounter);
  DHCPC_DEBUG_PRINTF ("dhcpReqRecvStateCounter - %d.\n", clientIntfInfo->dhcpReqRecvStateCounter);
  DHCPC_DEBUG_PRINTF ("dhcpRenewRecvStateCounter - %d.\n", clientIntfInfo->dhcpRenewRecvStateCounter);
  DHCPC_DEBUG_PRINTF ("dhcpRebindRecvStateCounter - %d.\n", clientIntfInfo->dhcpRebindRecvStateCounter);
  DHCPC_DEBUG_PRINTF ("numTrails - %d.\n", clientIntfInfo->numTrails);

  DHCPC_DEBUG_PRINTF ("=============================================================\n");
  return;
}

/*********************************************************************
*
* @purpose  Dump Client Intf Info contents
*
* @param    traceLevel @b{ (input) } Trace Level to disable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_uchar8*
dhcpClientDebugAddrPrint (L7_uint32 addr,
                          L7_uchar8 *buf)
{
  if (buf == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  memset (buf, 0, OSAPI_INET_NTOA_BUF_SIZE);

  osapiInetNtoa (osapiNtohl(addr), buf);

  return buf;
}

/**************************************************************************
* @purpose  To show bootp parameters
*
* @param    L7_char8*      Interface Name
* @param    L7_short16*    Ptr to bootp structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
void
show_bootp(const L7_char8 *intf, struct bootp *bp)
{
  L7_int32  i, len;
  L7_uchar8 *op, *ap = 0, optover;
  L7_uchar8 name[128];
  struct    in_addr addr[32];
  L7_char8  addrStr[OSAPI_INET_NTOA_BUF_SIZE];

  sysapiPrintf("\n\n--------------------------------------\n");
  sysapiPrintf("\nBOOTP[%s] op: %s\n", intf, _bootp_op[bp->bp_op]);
  sysapiPrintf("       htype: %s\n", _bootp_hw_type[bp->bp_htype]);
  sysapiPrintf("        hlen: %d\n", bp->bp_hlen );
  sysapiPrintf("        hops: %d\n", bp->bp_hops );
  sysapiPrintf("         xid: 0x%lx\n", osapiNtohl(bp->bp_xid) );
  sysapiPrintf("        secs: %d\n", bp->bp_secs );
  sysapiPrintf("       flags: 0x%x\n", bp->bp_flags );
  sysapiPrintf("       hw_addr: ");
  for ( i = 0;  i < bp->bp_hlen;  i++ )
  {
    sysapiPrintf("%02x", bp->bp_chaddr[i]);
    if ( i != (bp->bp_hlen-1) ) sysapiPrintf(":");
  }
  sysapiPrintf("\n");

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_ciaddr.s_addr), addrStr);
  sysapiPrintf("     client IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_yiaddr.s_addr), addrStr);
  sysapiPrintf("         my IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_siaddr.s_addr), addrStr);
  sysapiPrintf("     server IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_giaddr.s_addr), addrStr);
  sysapiPrintf("    gateway IP: %s\n", addrStr);

  optover = 0; /* See whether sname and file are overridden for options*/
  (void)l7_get_bootp_option( bp, TAG_DHCP_OPTOVER, &optover, sizeof(optover) );
  if ( !(1 & optover) && bp->bp_sname[0] )
    sysapiPrintf("        server: %s\n", bp->bp_sname);
  if ( ! (2 & optover) && bp->bp_file[0] )
    sysapiPrintf("          file: %s\n", bp->bp_file);
  if ( bp->bp_vend[0] )
  {
    sysapiPrintf("  options:\n");
    op = &bp->bp_vend[4];
    while ( *op != TAG_END )
    {
      switch ( *op )
      {
        case TAG_SUBNET_MASK:
        case TAG_GATEWAY:
        case TAG_IP_BROADCAST:
        case TAG_DOMAIN_SERVER:
          ap = (L7_uchar8 *)&addr[0];
          len = *(op+1);
          for ( i = 0;  i < len;  i++ )
          {
            *ap++ = *(op+i+2);
          }
          if ( *op == TAG_SUBNET_MASK )   ap =  "  subnet mask";
          if ( *op == TAG_GATEWAY )       ap =  "      gateway";
          if ( *op == TAG_IP_BROADCAST )  ap =  " IP broadcast";
          if ( *op == TAG_DOMAIN_SERVER ) ap =  "domain server";
          sysapiPrintf("      %s: ", ap);
          ap = (L7_uchar8 *)&addr[0];
          while ( len > 0 )
          {
            memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
            osapiInetNtoa(osapiNtohl((L7_uint32)(((struct in_addr *)ap)->s_addr)),
                          addrStr);
            sysapiPrintf("%s", addrStr);
            len -= sizeof(struct in_addr);
            ap += sizeof(struct in_addr);
            if ( len ) sysapiPrintf(", ");
          }
          sysapiPrintf("\n");
          break;
        case TAG_DOMAIN_NAME:
        case TAG_HOST_NAME:
          for ( i = 0;  i < *(op+1);  i++ )
          {
            name[i] = *(op+i+2);
          }
          name[*(op+1)] = '\0';
          if ( *op == TAG_DOMAIN_NAME ) ap =  " domain name";
          if ( *op == TAG_HOST_NAME )   ap =  "   host name";
          sysapiPrintf("       %s: %s\n", ap, name);
          break;
        case TAG_DHCP_MESS_TYPE:
          sysapiPrintf("        DHCP message: %d %s\n",
                  op[2], _dhcpmsgs[op[2]] );
          break;
        case TAG_DHCP_REQ_IP:
          sysapiPrintf("        DHCP requested ip: %d.%d.%d.%d\n",
                  op[2], op[3], op[4], op[5] );  
          break;
        case TAG_DHCP_LEASE_TIME   :
        case TAG_DHCP_RENEWAL_TIME :
        case TAG_DHCP_REBIND_TIME  :
          sysapiPrintf("        DHCP time %d: %d\n",
                  *op, ((((((op[2]<<8)+op[3])<<8)+op[4])<<8)+op[5]) );

          break;
        case TAG_DHCP_SERVER_ID    :
          sysapiPrintf("        DHCP server id: %d.%d.%d.%d\n",
                  op[2], op[3], op[4], op[5] );  
          break;

        case TAG_DHCP_OPTOVER      :
        case TAG_DHCP_PARM_REQ_LIST:
        case TAG_DHCP_TEXT_MESSAGE :
        case TAG_DHCP_MAX_MSGSZ    :
        case TAG_DHCP_CLASSID      :
        case TAG_DHCP_CLIENTID     :
          sysapiPrintf("        DHCP option: %x/%d.%d:", *op, *op, *(op+1));
          if ( 1 == op[1] )
          {
            sysapiPrintf( " %d", op[2] );
          }
          else if ( 2 == op[1] )
          {
            sysapiPrintf( " %d", (op[2]<<8) + op[3] );
          }
          else if ( 4 == op[1] )
          {
            sysapiPrintf( " %d", ((((((op[2]<<8)+op[3])<<8)+op[4])<<8)+op[5]) );
          }
          else
          {
            for ( i = 2; i < 2 + op[1]; i++ )
              sysapiPrintf(" %d",op[i]);
          }
          sysapiPrintf("\n");
          break;

        default:
          sysapiPrintf("        Unknown option: %x/%d.%d:", *op, *op, *(op+1));
          for ( i = 2; i < 2 + op[1]; i++ )
            sysapiPrintf(" %d",op[i]);
          sysapiPrintf("\n");
          break;
      }                
      /*temp op += *(op+1)+2; */
      op += *(op+1)+2;
    }
  }
  sysapiPrintf("\n--------------------------------------\n\n");
  return;
}

/**************************************************************************
* @purpose  To set DHCP client debug level
*
* @param    level   Debug level
* 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
L7_uint32
dhcpcDebugMsgLvlSet( L7_uint32 level )
{
  dhcpcDebugMsgLevel = level;

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  To set DHCP client debug level
*
* @param    level   Debug level
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
L7_uint32
dhcpClientDebugCBShow (void)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_bootp_dhcp_t dhcpParams;
  L7_char8 buf1[OSAPI_INET_NTOA_BUF_SIZE];

  for (rtrIfNum = 0; rtrIfNum < DHCP_CLIENT_MAX_INTERFACES; rtrIfNum++)
  {
    DHCPC_DEBUG_PRINTF ("=============================================================\n");
    if ((clientIntfInfo = dhcpClientCB.clientIntfInfo[rtrIfNum]) != L7_NULLPTR)
    {
      if (clientIntfInfo->inUse == L7_TRUE)
      {
        DHCPC_DEBUG_PRINTF ("sockFD - %d.\n", clientIntfInfo->sockFD);
        DHCPC_DEBUG_PRINTF ("intIfNum - %d.\n", clientIntfInfo->intIfNum);
        DHCPC_DEBUG_PRINTF ("rtrIfNum - %d.\n", clientIntfInfo->rtrIfNum);
        if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                                     &dhcpParams)
                                  == L7_SUCCESS)
        {
          DHCPC_DEBUG_PRINTF ("IP Address - %s.\n",
                              dhcpClientDebugAddrPrint(dhcpParams.ip, buf1));
          DHCPC_DEBUG_PRINTF ("Netmask - %s.\n",
                              dhcpClientDebugAddrPrint(dhcpParams.netMask, buf1));
          DHCPC_DEBUG_PRINTF ("Gateway - %s.\n",
                              dhcpClientDebugAddrPrint(dhcpParams.gateway,buf1));
        }
      }
      else
      {
        DHCPC_DEBUG_PRINTF ("Client is Not Active on rtrIfNum - %d.\n", rtrIfNum);
      }
    }
    else
    {
      DHCPC_DEBUG_PRINTF ("Client Info is NULL for rtrIfNum - %d.\n", rtrIfNum);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Trace dhcp packets received
*
* @param   clientIntfInfo @b{(input)} ptr to client interface info
* @param   bp             @b{(input)} Ptr to bootp structure
* @param   rxTrace        @b{(input)} Receive trace or Transmit trace
* @param   pktlen         @b{(input)} Packet length
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dhcpDebugPacketRxTxTrace(dhcpClientInfo_t *clientIntfInfo, struct bootp *bp,
                              L7_BOOL rxTrace, L7_uint32 pktlen)
{
  L7_uchar8 *op, optover;
  L7_char8  clientaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8  myaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8  serveraddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8  msgType[L7_CLI_MAX_STRING_LENGTH];
  L7_MGMT_PORT_TYPE_t mgmtPortType = clientIntfInfo->mgmtPortType;
  L7_uint32 intIfNum = clientIntfInfo->intIfNum;
  L7_uchar8 str[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 intfName[L7_CLI_MAX_STRING_LENGTH];
  nimUSP_t usp;

  memset(clientaddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_ciaddr.s_addr), clientaddr);

  memset(myaddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_yiaddr.s_addr), myaddr);

  memset(serveraddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_siaddr.s_addr), serveraddr);

  optover = 0; /* See whether sname and file are overridden for options*/
  (void)l7_get_bootp_option( bp, TAG_DHCP_OPTOVER, &optover, sizeof(optover) );
  if ( bp->bp_vend[0] )
  {
    op = &bp->bp_vend[4];
    while ( *op != TAG_END )
    {
      switch ( *op )
      {
        case TAG_DHCP_MESS_TYPE:
          memset(msgType, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiSnprintf(msgType, sizeof(msgType), "%s", _dhcpmsgs[op[2]]);
          break;
        case TAG_DHCP_SERVER_ID    :
          memset(serveraddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiSnprintf(serveraddr, sizeof(serveraddr), "%d.%d.%d.%d", op[2], op[3], op[4], op[5]);
          break;
        default:
          break;
      }
      op += *(op+1)+2;
    }
  }

  if (mgmtPortType == L7_MGMT_IPPORT)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      return;
    }
    memset(str, 0, L7_CLI_MAX_STRING_LENGTH);
    memset(intfName, 0, L7_CLI_MAX_STRING_LENGTH);

#ifdef L7_STACKING_PACKAGE
    osapiSnprintf(str, sizeof(str), "%d/%d/%d", usp.unit, usp.slot, usp.port);
#else 
    osapiSnprintf(str, sizeof(str), "%d/%d", usp.slot, usp.port);
#endif /* L7_STACKING_PACKAGE */

    osapiStrncpySafe(intfName, "interface ", L7_CLI_MAX_STRING_LENGTH);
    OSAPI_STRNCAT(intfName, str);
  }
  else if (mgmtPortType == L7_MGMT_NETWORKPORT)
  {
    memset(intfName, 0, L7_CLI_MAX_STRING_LENGTH);
    osapiStrncpySafe(intfName, "Network Port", L7_CLI_MAX_STRING_LENGTH);
  }
  else if (mgmtPortType == L7_MGMT_SERVICEPORT)
  {
    memset(intfName, 0, L7_CLI_MAX_STRING_LENGTH);
    osapiStrncpySafe(intfName, "Service Port", L7_CLI_MAX_STRING_LENGTH);
  }

  if (clientIntfInfo->dhcpState == DHCPSTATE_RENEWING)
  {
    memset(serveraddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
    osapiInetNtoa(osapiNtohl(clientIntfInfo->dhcpInfo.server_ip.s_addr), serveraddr);
  }
  else if (clientIntfInfo->dhcpState == DHCPSTATE_RENEW_RECV)
  {
    memset(clientaddr, 0, OSAPI_INET_NTOA_BUF_SIZE);
    memset(str, 0, L7_CLI_MAX_STRING_LENGTH);
    osapiInetNtoa(osapiNtohl(bp->bp_yiaddr.s_addr), clientaddr);
    osapiStrncpySafe(str, "Leased IP", L7_CLI_MAX_STRING_LENGTH);
  }
  else if (clientIntfInfo->dhcpState == DHCPSTATE_SELECTING)
  {
    memset(str, 0, L7_CLI_MAX_STRING_LENGTH);
    osapiStrncpySafe(str, "Offered IP", L7_CLI_MAX_STRING_LENGTH);
  }
  else if (clientIntfInfo->dhcpState == DHCPSTATE_REQUEST_RECV)
  {
    memset(str, 0, L7_CLI_MAX_STRING_LENGTH);
    osapiStrncpySafe(str, "Leased IP", L7_CLI_MAX_STRING_LENGTH);
  }
  else
  {
    /* Do Nothing */
  }

  if (rxTrace == L7_TRUE)
  {
    DHCPC_USER_TRACE("Received DHCP %s message with Src %s, Destn %s and %s %s on %s of pktlen = %d bytes",
                     msgType, serveraddr, clientaddr, str, myaddr, intfName, pktlen);
  }
  else
  {
    DHCPC_USER_TRACE("Sent DHCP %s message with Src %s and Destn %s on %s of pktlen = %d bytes",
                     msgType, clientaddr, serveraddr, intfName, pktlen);
  }
  return;
}

/*********************************************************************
*
* @purpose To check if serviceability debugs are enabled or not
*
* @param    traceLevel @b{ (input) } Trace Level to check debug tracing
*                                    is enabled or not
*                            
* @returns  L7_TRUE, if success
* @returns  L7_FALSE, if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL
dhcpClientDebugServiceabilityFlagCheck (DHCPC_TRACE_LVL_t traceLevel)
{
  if ((traceLevel != DHCPC_DEBUG_PKT_SRVC_RX) && (traceLevel != DHCPC_DEBUG_PKT_SRVC_TX))
  {
    return L7_FALSE;
  }

  if ((dhcpClientDebugFlags[traceLevel/8] & (1 << (traceLevel % 8))) != 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}


