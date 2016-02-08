
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_debug.c
*
* @purpose   Dynamic ARP Inspection debug utilities
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

#include "l7_common.h"
#include "osapi.h"
#include "log.h" 
#include "nimapi.h"
#include "comm_mask.h"
#include "l7_packet.h"

#include "dai_cfg.h"
#include "dai_util.h"
#include "dai_cnfgr.h"
#include "dhcp_snooping_api.h"

extern daiCfgData_t     *daiCfgData;
extern daiCnfgrState_t  daiCnfgrState;
extern osapiRWLock_t    daiCfgRWLock;

/* Global status data */
extern daiInfo_t        *daiInfo;

/* Array of per interface working data. Indexed on internal interface number */
extern daiIntfInfo_t    *daiIntfInfo;

extern L7_uchar8 *daiInitStateNames[];

extern void *dai_Event_Queue;
extern void *dai_Packet_Queue;

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
void daiDebugStats(void)
{
  printf("\n\nDynamic ARP Inspection debug stats...");
  printf("\n%-50s  %12u",
         "DAI Task Message Queue Receive Error",
         daiInfo->debugStats.msgRxError);

  printf("\n%-50s  %12u",
         "DAI Task Packet Message Queue Send Error",
         daiInfo->debugStats.frameMsgTxError);

  printf("\n%-50s  %12u",
         "DAI Task Event Message Queue Send Error",
         daiInfo->debugStats.eventMsgTxError);

  printf("\n%-50s  %12u",
         "ARP packets intercepted on Port Routing Interfaces",
         daiInfo->debugStats.pktsOnPortRoutingIf);

  printf("\n%-50s  %12u",
         "ARP packets intercepted on Routing VLANs",
         daiInfo->debugStats.pktsOnVlanRoutingIf);

  printf("\n%-50s  %12u",
         "ARP packets intercepted on Non-Routing VLANs",
         daiInfo->debugStats.pktsOnVlanNonRoutingIf);

  printf("\n%-50s  %12u",
         "ARP packets not intended but received",
         daiInfo->debugStats.pktsNotHandled);

  printf("\n%-50s  %12u",
         "ARP packets given to ARP Application",
         daiInfo->debugStats.pktsToArpAppln);

  printf("\n%-50s  %12u",
         "ARP packets given to system",
         daiInfo->debugStats.pktsToSystem);

  printf("\n%-50s  %12u",
         "ARP packets intercepted by DAI",
         daiInfo->debugStats.pktsIntercepted);

  printf("\n%-50s  %12u",
         "Packets queued to DAI Task",
         daiInfo->debugStats.pktsQueued);

  printf("\n%-50s  %12u",
         "Mbuf Failures in DAI",
         daiInfo->debugStats.daiMbufFailures);

  printf("\n%-50s  %12u",
         "ARP packets verified to be valid",
         daiInfo->debugStats.pktsValidated);

  printf("\n%-50s  %12u",
         "Packet TX Failures in DAI",
         daiInfo->debugStats.pktTxFailures);
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
void daiDebugStatsClear(void)
{
  memset(&daiInfo->debugStats, 0, sizeof(daiInfo->debugStats));
}

/*********************************************************************
* @purpose  Validate the config and state of the DAI component. 
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
L7_RC_t daiValidate(void)
{
  L7_BOOL vlanEnabled;      /* L7_TRUE if one or more VLANs enabled for DAI */
  L7_BOOL intfUntrusted;    /* L7_TRUE if one or more interfaces are trusted */
  L7_BOOL flag;
  L7_INTF_MASK_t invTrust;  /* inverse of trust config */
  L7_uint32 i, ipAddr, val;
  L7_uchar8 macAddr[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 aclName[L7_ARP_ACL_NAME_LEN_MAX+1];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (daiCnfgrState != DAI_PHASE_EXECUTE)
  {
    printf("\nERROR: DAI is in init state %s", daiInitStateNames[daiCnfgrState]);
    osapiReadLockGive(daiCfgRWLock);
    return L7_ERROR;
  }

  /* Check VLAN count on all interfaces. */
  for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    L7_ushort16 vlanCount = daiIntfInfo[i].daiNumVlansEnabled;
    if (vlanCount > DAI_MAX_VLAN_COUNT)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(i, L7_SYSNAME, ifName);
      printf("\nERROR: VLAN count %u on interface %s is invalid.", vlanCount, ifName);
      osapiReadLockGive(daiCfgRWLock);
      return L7_ERROR;
    }
  }

  /* Warn if DAI is not enabled on any VLANs. */
  L7_VLAN_NONZEROMASK(daiCfgData->daiVlans, vlanEnabled);
  if (!vlanEnabled)
  {
    printf("Warning: DAI not enabled on any VLANs\n");
  }

  /* Warn if no interfaces are untrusted. */
  L7_INTF_MASKEQ(invTrust, daiCfgData->daiTrust);    /* make copy of trust config */
  L7_INTF_MASKINV(invTrust);                       /* invert */
  L7_INTF_NONZEROMASK(invTrust, intfUntrusted);
  if (!intfUntrusted)
  {
    printf("Warning: All interfaces are trusted for DAI\n");
  }

  /* If the configured ARP ACL on a vlan is not yet created */
  for(i=1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    memset(aclName, 0, L7_ARP_ACL_NAME_LEN_MAX+1);
    _daiVlanArpAclGet(i, aclName);
    if(strcmp(aclName, ARP_ACL_NULL) != 0)
    {
      if(_arpAclGet(aclName) != L7_SUCCESS)
      {
        printf("Warning: Configured ARP ACL - %s on Vlan %d doesn't exist.\n",
               aclName, i);
      }
    }
  }

  /* The ARP ACL has no rules configured in it. If this ACL is
   * configured as static on a vlan, it can result in ARP packet drops */
  for(i=1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    memset(aclName, 0, L7_ARP_ACL_NAME_LEN_MAX+1);
    _daiVlanArpAclGet(i, aclName);
    if(strcmp(aclName, ARP_ACL_NULL) != 0)
    {
      if(_arpAclGet(aclName) == L7_SUCCESS)
      {
        ipAddr = 0;
        memset(macAddr, 0, L7_ENET_MAC_ADDR_LEN);
        if((_arpAclRuleInAclNextGet(aclName, ipAddr, macAddr, &ipAddr, macAddr) != L7_SUCCESS) &&
           (_daiVlanStaticFlagGet(i)) && _daiVlanEnableGet(i))
        {
          printf("ERROR: Static ARP ACL - %s on DAI enabled Vlan %d has no rules.\n       This would drop ARP pkt on untrusted port in this VLAN\n", aclName, i);
        }
      }
    }
  }

  /* If DHCP Snooping flex component is not present in this package, that
   * may result in ARP packet drops */
  if(cnfgrIsComponentPresent(L7_DHCP_SNOOPING_COMPONENT_ID) == L7_FALSE)
  {
    printf("Warning: DHCP Snooping component is not present.\n         It can drop ARP pkt on untrusted port if matching ARP ACL Rule is not found\n");

    printf("\nNOTE: If you see ARP packet drops, you can as well verify with - devshell ipsgValidate()\n");
    /* Return here since the checks below depend on DHCP Snooping configuration/state */
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }

  /* If DHCP Snooping is globally disabled, that
   * can result in ARP packet drops, if matching static snooping entry doesn't exist */
  if(dsFuncTable.dsAdminModeGet &&
     (dsFuncTable.dsAdminModeGet(&val) == L7_SUCCESS) &&
     (val == L7_DISABLE))
  {
    printf("Warning: DHCP Snooping is globally disabled.\n         It can drop ARP pkt on untrusted port if matching ARP ACL Rule or snooping entry is not found\n");

    printf("\nNOTE: If you see ARP packet drops, you can as well verify with - devshell ipsgValidate()\n");
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }

  /* If DHCP snooping is not enabled on ingress vlan,
   * the ARP packets may be dropped */
  for(i=1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if(_daiVlanEnableGet(i))
    {
      if(dsFuncTable.dsVlanConfigGet &&
         (dsFuncTable.dsVlanConfigGet(i, &val) == L7_SUCCESS) &&
         (val == L7_DISABLE))
      {
        printf("Warning: DHCP Snooping is not enabled on DAI enabled VLAN %d.\n         It can drop ARP pkt on untrusted port if matching ARP ACL Rule or snooping entry is not found\n", i);
      }
    }
  }

  /* If the ingress interface is trusted for DHCP snooping,
   * the ARP packets may be dropped */
  for(i=1; i< DAI_MAX_INTF_COUNT; i++)
  {
    if(!daiIntfIsValid(i))
      continue;

    if((! _daiIntfTrustGet(i)) && (daiIntfIsInspected(i)))
    {
      if((dsFuncTable.dsIntfTrustGet &&
          (dsFuncTable.dsIntfTrustGet(i, &flag) == L7_SUCCESS) &&
          (flag == L7_TRUE)) ||
         (dsFuncTable.dsPortEnabledGet &&
          (dsFuncTable.dsPortEnabledGet(i, &flag) == L7_SUCCESS) &&
          (flag == L7_FALSE)))
      {
        nimGetIntfName(i, L7_SYSNAME, ifName);
        printf("Warning: Interface %s is trusted for DHCP Snooping but untrusted for DAI.\n         It can result in ARP pkt drop if matching ARP ACL Rule or snooping entry is not found\n", ifName);
      }
    }
  }

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Write a DAI trace message.
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
L7_RC_t daiTraceWrite(L7_uchar8 *daiTrace)
{
  L7_uint64 msecSinceBoot, secSinceBoot, msecs;
  L7_uchar8 debugMsg[DAI_MAX_TRACE_LEN];
  L7_int32 eventQLen = -1, pktQLen = -1;

  if (daiTrace == NULL)
    return L7_SUCCESS;

  msecSinceBoot = osapiTimeMillisecondsGet();
  secSinceBoot = msecSinceBoot / 1000;
  msecs = msecSinceBoot % 1000;

  if (osapiMsgQueueGetNumMsgs((void*)dai_Event_Queue, &eventQLen) != L7_SUCCESS)
  {
    eventQLen = -1;
  }
  if (osapiMsgQueueGetNumMsgs((void*)dai_Packet_Queue, &pktQLen) != L7_SUCCESS)
  {
    pktQLen = -1;
  }

  osapiSnprintf(debugMsg, DAI_MAX_TRACE_LEN, "\n%d.%03d (%d,%d) %s",
                secSinceBoot, msecs, eventQLen, pktQLen, daiTrace);
  printf(debugMsg);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the DAI trace flags
*
* @param    daiTraceFlags  - Trace Flags
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiTraceFlagsSet(L7_uint32 daiTraceFlags)
{
  daiCfgData->daiTraceFlags = daiTraceFlags;
}

/*********************************************************************
* @purpose  Print the DAI trace flags
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiTraceFlagsDump(void)
{
  printf("\nDAI Trace Flags settings :\n");
  printf("--------------------------\n\n");
  printf("Configurator events    (0x1) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_INIT) ? "Enabled" : "Disabled"));
  printf("VLAN membership events (0x2) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_VLAN_EVENTS) ? "Enabled" : "Disabled"));
  printf("Port Trust events      (0x4) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_PORT_STATE) ? "Enabled" : "Disabled"));
  printf("Frame TX               (0x8) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_FRAME_TX) ? "Enabled" : "Disabled"));
  printf("Frame TX in detail    (0x10) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_FRAME_TX_DETAIL) ? "Enabled" : "Disabled"));
  printf("Frame RX              (0x20) : %s\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_FRAME_RX) ? "Enabled" : "Disabled"));
  printf("Frame RX in detail    (0x40) : %s\n\n",
         (((daiCfgData->daiTraceFlags) & DAI_TRACE_FRAME_RX_DETAIL) ? "Enabled" : "Disabled"));
}

/*********************************************************************
* @purpose  To print the queue depths currently
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiDebugQueueStats(void)
{
  L7_int32 eventQLen = -1, pktQLen = -1;

  if (osapiMsgQueueGetNumMsgs((void*)dai_Event_Queue, &eventQLen) != L7_SUCCESS)
  {
    eventQLen = -1;
  }
  if (osapiMsgQueueGetNumMsgs((void*)dai_Packet_Queue, &pktQLen) != L7_SUCCESS)
  {
    pktQLen = -1;
  }
  printf("\n Event Queue Depth : %d\n",eventQLen);
  printf(" Packet Queue Depth : %d\n",pktQLen);
}

/*********************************************************************
* @purpose  To print above debug commands for help
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void daiDebugHelp(void)
{
  printf("\nDAI Debug commands supported:\n");
  printf("-----------------------------\n");
  printf("1. daiDebugStats()           - Dump the DAI debug statistics.\n");
  printf("2. daiDebugStatsClear()      - Clear the DAI debug statistics.\n");
  printf("3. daiValidate()             - Validate the DAI configuration and warn user.\n");
  printf("4. daiTraceFlagsSet()        - Set the flags for DAI tracing.\n");
  printf("5. daiTraceFlagsDump()       - Dump the flags for DAI tracing.\n");
  printf("6. daiDebugQueueStats()      - Print the current event and packet queue lengths in DAI.\n\n");
}

