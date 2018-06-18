
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_util.c
*
* @purpose   Dynamic ARP Inspection utility routines
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create 09/06/2007
*
* @author Kiran Kumar Kella
*
* @end
*
**********************************************************************/

#include <ctype.h>
#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "sysnet_api_ipv4.h"

#include "dai_util.h"
#include "dai_cfg.h"
#include "dai_cnfgr.h"

#include "dot1q_api.h"

extern daiCfgData_t     *daiCfgData;

/* Global status data */
daiInfo_t *daiInfo = NULL;

/* Array of per interface working data. Indexed on internal interface number */
daiIntfInfo_t *daiIntfInfo = NULL;

/* Array of per vlan working data. Indexed on router interface number */
daiVlanInfo_t *daiVlanInfo = NULL;

L7_uchar8 *daiFailureNames[] = {"SRC MAC FAILURE", "DST MAC FAILURE",
                                "IP VALID FAILURE", "ACL MATCH FAILURE",
                                "DHCP SNOOP DB MATCH FAILURE"};

#define DAI_TRACE_LEN_MAX 255

/*********************************************************************
* @purpose  Note that configuration data has changed
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void daiConfigDataChange(void)
{
  daiInfo->cfgDataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Note that configuration data is now in sync with persistent
*           storage.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void daiCfgDataNotChanged(void)
{
  daiInfo->cfgDataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Clear DAI vlan statistics for all vlans.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _daiVlanStatsClear(void)
{
  L7_uint32 i;

  for (i = 1; i < DAI_MAX_VLAN_COUNT; i++)
  {
    memset(&(daiVlanInfo[i].stats), 0, sizeof(daiVlanStats_t));
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply a change to a VLAN's enable status for DAI
*
* @param    vlanId    @b((input)) VLAN ID
* @param    vlanCfg   @b((input)) L7_ENABLE if DAI enabled on VLAN
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableApply(L7_uint32 vlanId, L7_uint32 vlanCfg)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;

  /* For each port in the VLAN, update the VLAN enabled count. */
  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        L7_BOOL intfWasInspected = daiIntfIsInspected(i);
        if (vlanCfg == L7_ENABLE)
        {
          daiIntfInfo[i].daiNumVlansEnabled++;
          if (!intfWasInspected && daiIntfIsInspected(i))
          {
            daiIntfEnable(i);
          }
        }
        else
        {
          daiIntfInfo[i].daiNumVlansEnabled--;
          if (intfWasInspected && !daiIntfIsInspected(i))
          {
            daiIntfDisable(i);
          }
        }
      }
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Apply a change to an interface's trust for DAI
*
* @param    intIfNum   @b((input)) internal interface number
* @param    trust      @b((input)) L7_TRUE if interface is trusted
*                                  for DAI
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiIntfTrustApply(L7_uint32 intIfNum, L7_BOOL trust)
{
  if (daiIntfIsInspected(intIfNum))
  {
    if (daiCfgData->daiTraceFlags & DAI_TRACE_PORT_STATE)
    {
      L7_uchar8 traceMsg[DAI_MAX_TRACE_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiSnprintf(traceMsg, DAI_MAX_TRACE_LEN,
                    "%s DAI on %s port %s on trust change",
                    ((trust) ? "Disabling" : "Enabling"),
                    ((trust) ? "trusted" : "untrusted"), ifName);
      daiTraceWrite(traceMsg);
    }

    /* Enable/Disable DAI on port in hardware. */
    if (dtlDynamicArpInspectionConfig(intIfNum, trust) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
              "DAI failed to set interface %s as %s in hardware.",
              ifName, ((trust) ? "trusted" : "untrusted"));
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the rate limit params on an interface
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiIntfRateLimitApply(L7_uint32 intIfNum)
{
  /* reset the received packet count per second and number of
   * consecutive intervals with pps > rate limit,
   * to initial values */
  daiIntfInfo[intIfNum].pktRxCount = 0;
  daiIntfInfo[intIfNum].consecutiveInterval = 1;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether intIfNum is a member of a VLAN
*           where DAI is enabled.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    DAI is considered enabled on a port if the port
*           is a member of one or more VLANs where DAI is enabled.
*
* @end
*********************************************************************/
L7_BOOL daiIntfIsInspected(L7_uint32 intIfNum)
{
  if (intIfNum >= DAI_MAX_INTF_COUNT)
    return L7_FALSE;

  if (daiIntfInfo[intIfNum].daiNumVlansEnabled > 0)
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Initialize the interface data structures for DAI
*
* @param    intIfnum   @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void daiIntfCreate(L7_uint32 intIfNum)
{
  memset(&daiIntfInfo[intIfNum], 0,  sizeof(daiIntfInfo_t));
  daiIntfInfo[intIfNum].consecutiveInterval = 1;

  if (FD_DAI_TRUST == L7_ENABLE)
  {
    _daiIntfTrust(intIfNum);
  }
  else
  {
    _daiIntfUntrust(intIfNum);
  }
  daiCfgData->intfCfg[intIfNum].rate_limit     = FD_DAI_RATE_LIMIT;
  daiCfgData->intfCfg[intIfNum].burst_interval = FD_DAI_BURST_INTERVAL;
}

/*********************************************************************
* @purpose  Attach the interface for DAI
*
* @param    intIfnum   @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void daiIntfAttach(L7_uint32 intIfNum)
{
  daiIntfTrustApply(intIfNum, _daiIntfTrustGet(intIfNum));
}

/*********************************************************************
* @purpose  Enable Dynamic ARP Inspection on a port.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t daiIntfEnable(L7_uint32 intIfNum)
{
  L7_BOOL trusted = _daiIntfTrustGet(intIfNum);

  if (! trusted)
  {
    if (daiCfgData->daiTraceFlags & DAI_TRACE_PORT_STATE)
    {
      L7_uchar8 traceMsg[DAI_MAX_TRACE_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiSnprintf(traceMsg, DAI_MAX_TRACE_LEN,
                    "Enabling DAI on untrusted port %s.", ifName);
      daiTraceWrite(traceMsg);
    }

    /* Enable DAI on port in hardware (i.e., make it untrusted). */
    if (dtlDynamicArpInspectionConfig(intIfNum, L7_FALSE)
                                      != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
              "Failed to enable DAI in hardware on interface %s",
              ifName);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable Dynamic ARP Inspection on a port.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t daiIntfDisable(L7_uint32 intIfNum)
{
  L7_BOOL trusted = _daiIntfTrustGet(intIfNum);

  if (daiCfgData->daiTraceFlags & DAI_TRACE_PORT_STATE)
  {
    L7_uchar8 traceMsg[DAI_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceMsg, DAI_MAX_TRACE_LEN,
                  "Disabling DAI on %s port %s.",
                  ((trusted) ? "trusted" : "untrusted"), ifName);
    daiTraceWrite(traceMsg);
  }

  /* Disable DAI on port in hardware (i.e., make it trusted). */
  if (dtlDynamicArpInspectionConfig(intIfNum, L7_TRUE)
                                    != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DAI_COMPONENT_ID,
            "Failed to disable DAI in hardware on interface %s",
            ifName);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if the ACL Name is a valid string (i.e., if it
*           contains alpha-numeric characters)
*
* @param    aclName @b((input)) In string format
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t daiAclNameStringCheck(L7_uchar8 *aclName)
{
  L7_uint32 i=0; /* index into string */

  for (i=0; i<strlen(aclName); i++)
  {
    /* PTin modified: DAI */
    if (!isalnum((L7_int32)aclName[i]) && aclName[i]!='-' && aclName[i]!='_')
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert a MAC address to a string.
*
* @param    macAddr    @b((input)) MAC address to be converted
* @param    macAddrStr @b((output)) In string format
*
* @returns  void
*
* @notes    macAddrStr must be at least DAI_MAC_STR_LEN.
*
* @end
*********************************************************************/
void daiMacToString(L7_uchar8 *macAddr, L7_uchar8 *macAddrStr)
{
  osapiSnprintf(macAddrStr, DAI_MAC_STR_LEN, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAddr[0], macAddr[1], macAddr[2],
                macAddr[3], macAddr[4], macAddr[5]);
}

/*********************************************************************
* @purpose  Convert a MAC address in string format to hex.
*
* @param    macAddr  @b((input)) MAC string to be converted
* @param    mac      @b((output)) In hex format
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    stolen from cliConvertMac(). It's a little lame (requires
*           string to contain 2 characters between each colon).
*
* @end
*********************************************************************/
L7_RC_t daiStringToMac(L7_uchar8 *macStr, L7_uchar8 *macAddr)
{
  L7_uint32 i, j, digit_count = 0;
  L7_uchar8 mac_address[20];   /* more than enough */

  if (strlen(macStr) != 17)
  {      /* test string length */
    return L7_FAILURE;
  }

  for ( i=0,j=0; i<17; i++,j++ )
  {
    digit_count++;
    switch ( macStr[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FAILURE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FAILURE;
      break;
    }
  }
  for ( i = 0; i < 6; i++ )
  {
    macAddr[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Log an Ethernet header.
*
* @param    frame      @b{(input)} start of ethernet frame
* @param    traceDest  @b{(input)} whether to send output to log or console
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void daiLogEthernetHeader(L7_enetHeader_t *ethHdr, daiTraceDest_t traceDest)
{
  L7_uchar8 traceBuf[DAI_TRACE_LEN_MAX];
  L7_uchar8 srcMacStr[DAI_MAC_STR_LEN];
  L7_uchar8 dstMacStr[DAI_MAC_STR_LEN];
  L7_enet_encaps_t *tol;
  L7_ushort16 typeOrLen;

  daiMacToString(ethHdr->dest.addr, dstMacStr);
  daiMacToString(ethHdr->src.addr, srcMacStr);
  tol = (L7_enet_encaps_t*) ((L7_uchar8*)ethHdr + sizeof(L7_enetHeader_t));
  typeOrLen = osapiNtohs(tol->type);
  osapiSnprintf(traceBuf, DAI_TRACE_LEN_MAX,
                "DAI: Ethernet header- dest %s, src %s, type/len %#x.",
                dstMacStr, srcMacStr, typeOrLen);
  if (traceDest == DAI_TRACE_LOG)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DAI_COMPONENT_ID, traceBuf);
  }
  else
  {
    printf("\n\n%s", traceBuf);
  }
}

/*********************************************************************
* @purpose  Log the contents of ARP packet
*
* @param    frame    @b{(input)} ethernet frame
* @param    vlanId   @b{(input)} VLAN ID
* @param    intIfNum @b{(input)} internal interface number
* @param    traceDest  @b{(input)} whether to send output to log or console
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void daiLogArpPacket(L7_uchar8 *frame, L7_ushort16 vlanId,
                     L7_uint32 intIfNum, daiTraceDest_t traceDest)
{
  L7_uint32 offset;
  L7_ether_arp_t *arp_pkt;
  L7_uchar8 traceBuf[DAI_TRACE_LEN_MAX];
  L7_uchar8 s_ipaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 t_ipaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 s_macaddr[DAI_MAC_STR_LEN];
  L7_uchar8 t_macaddr[DAI_MAC_STR_LEN];

  offset = sysNetDataOffsetGet(frame);
  arp_pkt = (L7_ether_arp_t *)(frame + offset);

  osapiInetNtoa(GET_IP_FROM_PKT(arp_pkt->arp_spa), s_ipaddr);
  osapiInetNtoa(GET_IP_FROM_PKT(arp_pkt->arp_tpa), t_ipaddr);

  daiMacToString(arp_pkt->arp_sha, s_macaddr);
  daiMacToString(arp_pkt->arp_tha, t_macaddr);

  osapiSnprintf(traceBuf, DAI_TRACE_LEN_MAX,
                "DAI: ARP PKT- op %s, sender mac %s, sender ip %s, target mac %s,"
                " target ip %s",
                ((arp_pkt->arp_op == osapiHtons(L7_ARPOP_REQUEST)) ? "Request" : "Reply"),
                s_macaddr, s_ipaddr, t_macaddr, t_ipaddr);

  if (traceDest == DAI_TRACE_LOG)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DAI_COMPONENT_ID, traceBuf);
  }
  else
  {
    printf("\n\n%s", traceBuf);
  }
}

/*********************************************************************
* @purpose  Log an ARP packet
*
* @param    frame    @b{(input)} ethernet frame
* @param    vlanId   @b{(input)} VLAN ID
* @param    intIfNum @b{(input)} internal interface number
* @param    failure  @b{(input)} type of DAI failure to log
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void daiLogAndDropPacket(L7_uchar8 *frame, L7_ushort16 vlanId,
                         L7_uint32 intIfNum, daiFailure_t failure)
{
  daiVlanInfo[vlanId].stats.dropped++;
  switch(failure)
  {
    case SOURCE_MAC_FAILURE:
      daiVlanInfo[vlanId].stats.sMacFailures++;
      break;
    case DEST_MAC_FAILURE:
      daiVlanInfo[vlanId].stats.dMacFailures++;
      break;
    case IP_VALID_FAILURE:
      daiVlanInfo[vlanId].stats.ipValidFailures++;
      break;
    case ACL_MATCH_FAILURE:
      daiVlanInfo[vlanId].stats.aclDrops++;
      break;
    case DHCP_SNOOP_DB_MATCH_FAILURE:
      daiVlanInfo[vlanId].stats.dhcpDrops++;
      break;
    default:
      /* Invalid Failure type, just return */
      return;
  }

  if(_daiVlanLogInvalidGet(vlanId))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DAI_COMPONENT_ID,
            "DAI dropped ARP frame rcvd on i/f %s in vlan %d, due to - %s",
            ifName, vlanId, daiFailureNames[failure]);
    daiLogEthernetHeader((L7_enetHeader_t *)frame, DAI_TRACE_LOG);
    daiLogArpPacket(frame, vlanId, intIfNum, DAI_TRACE_LOG);
  }
}

