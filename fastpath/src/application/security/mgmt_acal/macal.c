/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal.c
*
* @purpose Management Access Control and Administration List (MACAL) main functions
*
* @component Management Access Control and Administration List (MACAL)
*
* @comments none
*
* @create 05/05/2005
*
* @author stamboli
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "sysapi.h"
#include "macal.h"
#include "mgmt_acal_sid/macal_sid.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"

#include "usmdb_trapmgr_api.h"
#include "sslt_exports.h"
#include "acl_exports.h"

L7_int32              macalTaskId;
extern void           *macalQueue;
extern macalCfgData_t *macalCfgData;
osapiRWLock_t         macalRwLock;
L7_uint32             macalDebugLevel;

/*********************************************************************
* @purpose  Start Management Access Control and Administration List task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t macalStartTask()
{
  macalTaskId = osapiTaskCreate("macalTask", macalTask, 0, 0,
                                macalSidDefaultStackSize(),
                                macalSidDefaultTaskPriority(),
                                macalSidDefaultTaskSlice());

  if (macalTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "macalStartTask(): Could not create task macalTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_MGMT_ACAL_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "Unable to initialize macalTask()\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose task to handle all Management Access Control and Administration List management messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void macalTask()
{
  macalMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_MGMT_ACAL_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(macalQueue, (void *)&msg, MACAL_MSG_SIZE, L7_WAIT_FOREVER);
    switch (msg.msgId)
    {
      case (macalCnfgr):
        macalCnfgrParse(&msg.CmdData);
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
                "macalTask(): invalid message type:%d. %s:%d\n", msg.msgId, __FILE__, __LINE__);
        break;
    }
  } while (1);
}

/*********************************************************************
*
* @purpose  Register/Deregister with sysnet for incoming packet processing
*
* @param    L7_BOOL  bRegister   @b((input)) Register (when L7_TRUE)
*                                            Deregister (when L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t macalSysnetRegisterDeregister(L7_BOOL bRegister)
{
  sysnetPduIntercept_t sysnetPduIntercept;
  const L7_char8 *fnName = "macalSysnetRegisterDeregister()";
  static L7_BOOL registered = L7_FALSE;

  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_LOCAL_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_MGMT_ACAL_FILTER_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = macalPktIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "macalPktIntercept");

  if ((bRegister == L7_TRUE) && (registered == L7_FALSE))
  {
    registered = L7_TRUE;

    /* register with sysnet for local IP packets */
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
              "%s: Failed to register with sysnet\n", fnName);
      return L7_FAILURE;
    }
  }
  else if ((bRegister == L7_FALSE) && (registered == L7_TRUE))
  {
    registered = L7_FALSE;

    /* deregister with sysnet */
    if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
              "%s: Failed to deregister with sysnet\n", fnName);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Intercept IP Management Packets,
*               verify access against the MACAL rules
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_DISCARD  if management is not allowed; drop the packet
* @returns  SYSNET_PDU_RC_IGNORED  if management is allowed; ignore and continue processing
*
* @notes    none
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t macalPktIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_char8       *data;
  L7_uint32       offset, i;
  L7_ipHeader_t  *ipHdr;
  L7_uint32      intIfNum = pduInfo->intIfNum;
  L7_uint32      vlanId = pduInfo->vlanId;
  L7_BOOL       permitMatchFound = L7_FALSE; /* default is DENY */
  L7_ushort16 dest_port;
  L7_ushort16 src_port;
  L7_tcpHeader_t *tcpUdpHdr;
  L7_char8  ipAddrString[32];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  offset = sysNetDataOffsetGet(data);
  ipHdr = (L7_ipHeader_t *)(data + offset);

  if (ipHdr->iph_prot != IP_PROT_TCP &&
      ipHdr->iph_prot != IP_PROT_UDP)
  {  /* Don't process if not TCP/UDP packet. We should not reach here. */
    return SYSNET_PDU_RC_IGNORED;
  }

  /* UDP and TCP headers are actually the same wrt destination port */
  tcpUdpHdr = (L7_tcpHeader_t *)(data + offset + L7_IP_HDR_LEN);

  if (ipHdr->iph_prot == IP_PROT_TCP)
  {
    /* only need to check TCP SYN packets */
    if (!(tcpUdpHdr->dataOffsetCtrlFlags & MACAL_TCP_SYN_BIT))
    {
      return SYSNET_PDU_RC_IGNORED;
    }
  }
  else if (ipHdr->iph_prot == IP_PROT_UDP)
  {
    /* must allow DHCP client and server packets */
    if ((tcpUdpHdr->destPort == UDP_PORT_DHCP_SERV) ||
        (tcpUdpHdr->destPort == UDP_PORT_DHCP_CLNT))
    {
      return SYSNET_PDU_RC_IGNORED;
    }
  }
  dest_port = tcpUdpHdr->destPort;
  src_port = tcpUdpHdr->srcPort;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MACAL_DEBUG_MSG("MACL Trace: Packet Ignored, cannot take lock.");
    return SYSNET_PDU_RC_IGNORED;
  }

  if (macalCfgData->consoleOnly == L7_TRUE)
  {
    (void)osapiReadLockGive(macalRwLock);

    LOG_MACAL_DEBUG_MSG("MACL Trace: Packet does not pass MACL => drop (serial console only).");
    return SYSNET_PDU_RC_DISCARD;
  }

  if (0 == vlanId)  /* some paths to this point have cleared the pduInfo. This indicator means
                     * the pduInfo should be re-constructed, at least the parts used.
                     */
  {
    /* Packet Trace Code */
    L7_char8   *pEtype;
    L7_ushort16 protocol_type;
    L7_ushort16 temp16;

    pEtype = data + L7_ENET_HDR_SIZE;
    bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

    temp16 = osapiNtohs(protocol_type);
    protocol_type = temp16;

    if (protocol_type == L7_ETYPE_8021Q)
    {
      /* This is an Vlan tagged Frame */
      pEtype = data + L7_ENET_HDR_SIZE + 2;
      bcopy (pEtype, (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16)); /*Endian*/

      vlanId = pduInfo->vlanId = osapiNtohs(protocol_type) & 0x0FFF;
    }
  }

  LOG_MACAL_DEBUG_MSG("MACL Trace: Packet received on Vlan 0x%04X, I/F %d, %s SRC %d.%d.%d.%d, protocol %d, src_port %d, dest_port %d.",
                      pduInfo->vlanId, pduInfo->intIfNum, ifName, FMTIPADDR(ipHdr->iph_src), ipHdr->iph_prot, src_port, dest_port);

  if (macalDebugLevel != 0) {
    macalDebugPktDump((L7_uchar8 *)data);
  }

  /* Go thru the rules until DENY match is found. If a PERMIT match is found,
     set the permitMatchFound flag but keep going thru the rules to see if there
     is any DENY match.*/
  for (i = 1; i <= MACAL_MAX_NUM_RULES; i++)
  {
    if (macalCfgData->macalList.rule[i].activationStatus == L7_DEACTIVATED)
    {
      continue;
    }
    /* Check if the interfaces match if that match is required for this rule */
    if ((!(macalCfgData->macalList.rule[i].configMask & MACAL_IF)) ||
        ((macalCfgData->macalList.rule[i].configMask & MACAL_IF) &&
         (macalCfgData->macalList.rule[i].ifNum == intIfNum)))
    {
      /* Now check if the VLAN IDs match if that match is required for this rule */
      if ((!(macalCfgData->macalList.rule[i].configMask & MACAL_VLAN))||
          ((macalCfgData->macalList.rule[i].configMask & MACAL_VLAN) &&
           (macalCfgData->macalList.rule[i].vlanId == vlanId)))
      {
        /* Now check if IP address + mask match if that match is required for this rule */
        /* Assumption: Src IP Addr and Src Mask are always set together; if Addr is set then
           the Mask has to be set */
        L7_uint32 mask = macalCfgData->macalList.rule[i].srcMask;
        if ((!(macalCfgData->macalList.rule[i].configMask & MACAL_SRCIP))||
            ((macalCfgData->macalList.rule[i].configMask & MACAL_SRCIP) &&
             ((macalCfgData->macalList.rule[i].srcIp & mask) == (ipHdr->iph_src & mask))))
        {
          /* Now check if the service matches if that match is required for this rule */
          if ((!(macalCfgData->macalList.rule[i].configMask & MACAL_SERVICE))||
              ((macalCfgData->macalList.rule[i].configMask & MACAL_SERVICE) &&
               ((macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_TELNET &&
                 dest_port == MACAL_TELNET_PORT) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_HTTP &&
                 dest_port == MACAL_HTTP_PORT) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_HTTPS &&
                 dest_port == MACAL_HTTPS_PORT) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_SNMP &&
                 dest_port == MACAL_SNMP_PORT) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_SNTP &&
                 (dest_port == MACAL_SNTP_PORT || src_port == MACAL_SNTP_PORT)) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_TFTP &&
                 ((dest_port == MACAL_TFTP_DOWNLOAD_L4PORT) || (dest_port == MACAL_TFTP_UPLOAD_L4PORT))) ||
                (macalCfgData->macalList.rule[i].serviceType == MACAL_PROTOCOL_SSH &&
                 dest_port == MACAL_SSH_PORT))))
          {
            if (macalCfgData->macalList.rule[i].action == MACAL_ACTION_PERMIT)
            {
              LOG_MACAL_DEBUG_MSG("MACL Trace: Rule %d matches with permit action.", i);
              permitMatchFound = L7_TRUE;
              break;
            }
            else /* DENY action */
            {
              LOG_MACAL_DEBUG_MSG("MACL Trace: Rule %d matches with deny action.", i);
              permitMatchFound = L7_FALSE;
              break;
            }
          }
          else
          {/* Service type match is required and the service types don't match; continue to the next rule */
            LOG_MACAL_DEBUG_MSG("MACL Trace: Mismatch on rule %d. Rule services %d, ingress service %d.", i,
                                macalCfgData->macalList.rule[i].serviceType, dest_port);
            continue;
          }
        }
        else
        {/* IP address match is required and the addresses don't match; continue to the next rule */
          LOG_MACAL_DEBUG_MSG("MACL Trace: Mismatch on rule %d. Rule IP %d.%d.%d.%d, ingress IP %d.%d.%d.%d.", i,
                              FMTIPADDR(macalCfgData->macalList.rule[i].srcIp & mask), FMTIPADDR(ipHdr->iph_src));
          continue;
        }
      }
      else
      {/* VLAN ID match is required and the IDs don't match; continue to the next rule */
        LOG_MACAL_DEBUG_MSG("MACL Trace: Mismatch on rule %d. Rule VLAN %d, ingress VLAN %d.", i,
                            macalCfgData->macalList.rule[i].vlanId, vlanId);
        continue;
      }
    }
    else
    { /* Interface match is required and the interfaces don't match; continue to the next rule */
      LOG_MACAL_DEBUG_MSG("MACL Trace: Mismatch on rule %d. Rule interface %d, %s, ingress interface %d.", i,
                          macalCfgData->macalList.rule[i].ifNum, intIfNum, ifName);
      continue;
    }
  } /* end for stmt */

  (void)osapiReadLockGive(macalRwLock);

  if (permitMatchFound == L7_TRUE)
  {
    LOG_MACAL_DEBUG_MSG("MACL Trace: Packet Permitted.");
    return SYSNET_PDU_RC_IGNORED;
  }

  osapiInetNtoa(ipHdr->iph_src, ipAddrString);
  if ((dest_port == MACAL_TELNET_PORT) || (dest_port == MACAL_SSH_PORT))
  {
    usmDbTrapMgrManagementACLViolationTrapSend("CLI", ipAddrString);
  }
  else if ((dest_port == MACAL_HTTP_PORT) || (dest_port == MACAL_HTTPS_PORT))
  {
    usmDbTrapMgrManagementACLViolationTrapSend("Web", ipAddrString);
  }
  else if (dest_port == MACAL_SNMP_PORT)
  {
    usmDbTrapMgrManagementACLViolationTrapSend("SNMP", ipAddrString);
  }
  else if ((dest_port == MACAL_SNTP_PORT) || (src_port == MACAL_SNTP_PORT))
  {
    usmDbTrapMgrManagementACLViolationTrapSend("SNTP", ipAddrString);
  }
  else if (dest_port == MACAL_TFTP_DOWNLOAD_L4PORT || dest_port == MACAL_TFTP_UPLOAD_L4PORT)
  {
    usmDbTrapMgrManagementACLViolationTrapSend("TFTP", ipAddrString);
  }

  LOG_MACAL_DEBUG_MSG("MACL Trace: Packet does not pass MACL => drop (true violation).");

  return SYSNET_PDU_RC_DISCARD;
}

/********************* DEBUG Infrastrcture *********************/

/*********************************************************************
* @purpose  Dump a packet that has been handed to the management ACL interceptor
*
* @param    *pkt    Pointer to raw frame
*
* @returns  nothing
*
* @end
*********************************************************************/
void macalDebugPktDump(L7_uchar8 *pkt)
{
  L7_char8  buffer[MACAL_DEBUG_PKT_BUF_SIZE + 1];
  int       n, row,  column;

  osapiSnprintf(buffer, MACAL_DEBUG_PKT_BUF_SIZE, "===================\n");
  for (row = 0; row < MACAL_DEBUG_PKT_LINES; row++)
  {
    n = strlen(buffer);
    if (n >= MACAL_DEBUG_PKT_BUF_SIZE) break;
    osapiSnprintf(&buffer[n], MACAL_DEBUG_PKT_BUF_SIZE-n, "%04x ", row * 16);
    for (column = 0; column < 16; column++)
    {
      n = strlen(buffer);
      if (n >= MACAL_DEBUG_PKT_BUF_SIZE) break;
      osapiSnprintf(&buffer[n], MACAL_DEBUG_PKT_BUF_SIZE-n, "%2.2x ", pkt[row*16 + column]);
    }
    n = strlen(buffer);
    if (n >= MACAL_DEBUG_PKT_BUF_SIZE) break;
    osapiSnprintf(&buffer[n], MACAL_DEBUG_PKT_BUF_SIZE-n, "\n");
  }
  n = strlen(buffer);
  osapiSnprintf(&buffer[n], MACAL_DEBUG_PKT_BUF_SIZE-n, "===================\n");
  LOG_MACAL_DEBUG_MSG("%s", buffer);
}

/**************************************************************************
* @purpose  To set Management ACL debug level
*
* @param    level   Debug level
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*************************************************************************/
L7_uint32 macalDebugLvlSet(L7_uint32 level)
{
  macalDebugLevel = level;

  return L7_SUCCESS;
}
