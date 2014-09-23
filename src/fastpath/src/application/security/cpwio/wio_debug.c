/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_debug.c
*
* @purpose   Debugging utilities for captive portal wired interface owner.
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/


#include <string.h>
#include <stdio.h>
#include "l7_common.h"
#include "wio_util.h"
#include "wio_outcalls.h"
#include "default_cnfgr.h"


/* Global status data */
extern wioInfo_t *wioInfo;

extern L7_char8 *wioInitStateNames[];


/*********************************************************************
* @purpose  Write a captive portal wired interface trace message.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wioTraceWrite(L7_uchar8 *wioTrace)
{
  L7_uint32 msecSinceBoot, secSinceBoot, msecs;
  L7_uchar8 debugMsg[WIO_MAX_TRACE_LEN];

  if (wioTrace == NULL)
    return L7_SUCCESS;

  msecSinceBoot = osapiTimeMillisecondsGet();
  secSinceBoot = msecSinceBoot / 1000;
  msecs = msecSinceBoot % 1000;

  osapiSnprintf(debugMsg, WIO_MAX_TRACE_LEN, "\n%d.%03d %s",
                secSinceBoot, msecs, wioTrace);
  printf("%s", debugMsg);
  return L7_SUCCESS;
}

/* Set trace flags */
void wioTraceFlagsSet(L7_uint32 wioTraceFlags)
{
  wioInfo->wioTraceFlags = wioTraceFlags;
}

/* Get help remembering which bits are which. */
void wioDebugHelp(void)
{

  printf("\nCaptive portal wired interface owner traces:");
  printf("\n0x1,        client list");
  printf("\n0x2,        pending client list");
  printf("\n0x4,        changes to auth server IPv4 address");
  printf("\n0x8,        interface blocking");
  printf("\n0x10,       configurator initialization");
  printf("\n0x20,       captive portal state on interfaces");

  if (wioInfo)
  {
    printf("\nCurrent trace flags:  %#x", wioInfo->wioTraceFlags);
  }

  printf("\nOther debug utilities:");
  printf("\nwioDebugStatsShow(void)                  print debug statistics");
  printf("\nwioStatsClear(void)                      clear debug statistics");
  printf("\nwioIntfStatsClear(L7_uint32 intIfNum)    clear interface stats");
  printf("\nwioGlobalStateShow(void)                 show global state");
  printf("\nwioIntfStateShow(L7_uint32 intIfNum)     show interface state");
  printf("\nwioClientListShow(void)                  show client list");
  printf("\nwioConnListShow(L7_uint32 intIfNum)      show connection list");
}

/* print debug stats */
void wioDebugStatsShow(void)
{
  L7_uint32 i;
  wioIntfDebugStats_t intfStats;
  L7_BOOL ifStatsPrinted = L7_FALSE;

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;


  /* global stats */
  /* information */
  printf("\nMessage queue high water                           %u",
         wioInfo->debugStats.msgQHighWater);
  printf("\nFrame queue high water                             %u",
         wioInfo->debugStats.frameQHighWater);
  printf("\nClient moved from one wired intf to another        %u",
         wioInfo->debugStats.clientMoves);
  printf("\nOutbound packets intercepted                       %u",
         wioInfo->debugStats.packetsOut);
  printf("\nNumber of outgoing TCP packets intercepted         %u",
         wioInfo->debugStats.tcpOut);
  printf("\nRemote address changes                             %u",
         wioInfo->debugStats.remoteAddrChange);
  /* errors */
  printf("\n");
  printf("\nConnection buffer list exhausted                   %u", 
         wioInfo->debugStats.outOfConnBuffers);
  printf("\nNo authentication server address                   %u",
         wioInfo->debugStats.noAuthServerAddr);
  printf("\nMessage too big for pseudo packet buffer           %u",
         wioInfo->debugStats.messageTooBig);
  printf("\nFailed to add client                               %u",
         wioInfo->debugStats.clientAddFailed);
  printf("\nFailed to remove client                            %u",
         wioInfo->debugStats.clientRemoveFailed);
  printf("\nNumber of outgoing packets w/ outbuffer too small  %u",
         wioInfo->debugStats.outBufferTooSmall);
  printf("\nNumber of non-IP outbound packets                  %u",
         wioInfo->debugStats.nonIpPacketsOut);
  printf("\nFrame message queue full                           %u",
         wioInfo->debugStats.frameMsgQFull);
  printf("\nMessage queue receive errors                       %u",
         wioInfo->debugStats.msgQRxError);
  printf("\nFailure forwarding ethernet frame                  %u",
         wioInfo->debugStats.frameTxFailures);
  printf("\nMbuf allocation failed                             %u",
         wioInfo->debugStats.mbufFailures);
  printf("\nEthernet encapsulation failed                      %u",
         wioInfo->debugStats.encapsFailed);

  /* per interface stats */
  memset(&intfStats, 0, sizeof(wioIntfDebugStats_t));
  for (i = 1; i <= WIO_MAX_INTERFACES; i++)
  {
    wioIntfInfo_t *intfInfo = wioIntfInfoGet(i);

    if (!intfInfo)
      continue;

    /* Only print an interface if it has one or more non-zero stats */
    if (memcmp(&intfStats, &intfInfo->debugStats, sizeof(wioIntfDebugStats_t)))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(i, L7_SYSNAME, ifName);

      ifStatsPrinted = L7_TRUE;
      printf("\n\nInterface %s", ifName);
      /* information */
      printf("\nNumber of incoming IPv4 packets intercepted     %u",
             intfInfo->debugStats.packetsIn);
      printf("\nNumber of incoming ARP packets intercepted      %u",
             intfInfo->debugStats.arpPacketsIn);
      printf("\nNumber of incoming ARP packets from auth client %u",
             intfInfo->debugStats.arpPacketsAuthClient);
      printf("\nNumber of incoming ARP packets to local MAC     %u",
             intfInfo->debugStats.arpPacketsLocal);
      printf("\nNumber of incoming ARP packets to be forwarded  %u",
             intfInfo->debugStats.arpPacketsForward);
      printf("\nNumber of outgoing packets intercepted          %u",
             intfInfo->debugStats.packetsOut);
      printf("\nNumber of incoming packets blocked              %u",
             intfInfo->debugStats.packetsInBlocked);
      printf("\nNumber of outgoing packets blocked              %u",
             intfInfo->debugStats.packetsOutBlocked);
      printf("\nNumber of new clients reported to CP            %u",
             intfInfo->debugStats.newClients);
      printf("\nNumber of pkts passed for unauth clients        %u",
             intfInfo->debugStats.unauthPacketsPassed);
      printf("\nNumber of pkts dropped for unauth clients       %u",
             intfInfo->debugStats.unauthPacketsDropped);
      printf("\nNumber of pkts redirected to auth server        %u",
             intfInfo->debugStats.redirectedPacketsIn);
      printf("\nNumber of pkts given new source address         %u",
             intfInfo->debugStats.sourceAddrChange);

      printf("\n");
      printf("\nFailed to modify outgoing packet                %u",
             intfInfo->debugStats.failedToModSrcAddr);
      printf("\nNumber of incoming pkts with invalid src addr   %u",
             intfInfo->debugStats.invalidSrcAddr);
      printf("\nNumber of discarded ARP packets                 %u",
             intfInfo->debugStats.arpPacketsDiscarded);
      printf("\nNumber of ARP packets too long for msg queue    %u",
             intfInfo->debugStats.frameTooLong);
      printf("\nFailed to add client MAC to FDB                 %u",
             intfInfo->debugStats.fdbAddFailure);
      printf("\nFailed to remove client MAC from FDB            %u",
             intfInfo->debugStats.fdbDelFailure);
    }
  }

  osapiSemaGive(wioInfo->wioLock);
  if (!ifStatsPrinted)
  {
    printf("\n\nAll interface statistics are 0.");
  }
}

/* clear global and interface stats */
/* don't take semaphore. May be called by code that has semaphore. */
void wioStatsClear(void)
{
  L7_uint32 i;

  /* Global stats */
  memset(&wioInfo->debugStats, 0, sizeof(wioDebugStats_t));

  /* Interface stats */
  for (i = 1; i <= WIO_MAX_INTERFACES; i++)
    wioIntfStatsClear(i);
}

/* Clear interface statistics */
/* don't take semaphore. May be called by code that has semaphore. */
void wioIntfStatsClear(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo;

  intfInfo = wioIntfInfoGet(intIfNum);
  if (!intfInfo)
  {
    osapiSemaGive(wioInfo->wioLock);
    return;
  }

  memset(&intfInfo->debugStats, 0, sizeof(wioIntfDebugStats_t));
}

/* Print global state */
void wioGlobalStateShow(void)
{
  L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  printf("\nInit state:  %s", wioInitStateNames[wioInfo->cnfgrState]);

  if (wioCpIsActive())
    printf("\nCP is active on one or more wired interfaces.");
  else
    printf("\nCP not active on any wired interface.");

  osapiInetNtoa(wioInfo->authServerAddr, ipAddrStr);
  printf("\nAuthentication server address:  %s", ipAddrStr);

  if (wioInfo->authServerIntf != 0)
  {
    nimGetIntfName(wioInfo->authServerIntf, L7_SYSNAME, ifName);
    printf("\nAuthentication server address on interface:  %s", ifName);
  }

  if (wioInfo->isRegWithSysnet)
    printf("\nRegistered with sysnet");
  else
    printf("\nNot registered with sysnet");

  printf("\nConnection buffer pool ID:  %u", wioInfo->connPoolId);

  printf("\nMessage queue high water:  %u (max = %u)", 
         wioInfo->debugStats.msgQHighWater, WIO_QUEUE_MSG_COUNT);

  printf("\nFrame queue high water:  %u (max = %u)", 
         wioInfo->debugStats.frameQHighWater, WIO_FRAME_QUEUE_MSG_COUNT);

  printf("\nwioInfo->wioLock:  %#x", (L7_uint32) wioInfo->wioLock);
  printf("\nwioInfo->wioClientIpList:  %#x", (L7_uint32) wioInfo->wioClientIpList);
  printf("\nwioInfo->wioClientConnList:  %#x", (L7_uint32) wioInfo->wioClientConnList);
  printf("\nCurrent number of clients:  %u", avlTreeCount(&wioInfo->wioClientList.treeData));

  osapiSemaGive(wioInfo->wioLock);
}

/* print interface state for a specific interface */
void wioIntfStateShow(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  intfInfo = wioIntfInfoGet(intIfNum);
  if (!intfInfo)
  {
    osapiSemaGive(wioInfo->wioLock);
    return;
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  printf("\nCaptive portal wired interface state for %s", ifName);

  if (intfInfo->wioIntfState == WIO_INTF_STATE_ENABLED)
    printf("\nCaptive portal is enabled");
  else
    printf("\nCaptive portal is disabled");

  if (intfInfo->wioIntfBlock == WIO_INTF_BLOCKED)
    printf("\nInterface is blocked");
  else
    printf("\nInterface not blocked");

  if (wioOptWebServerPortGet(intIfNum) != 0)
    printf("\nOptional web server port:  %u", wioOptWebServerPortGet(intIfNum));
  else
    printf("\nNo optional web server port");

  if (wioOptSecureWebServerPort1Get(intIfNum) != 0)
    printf("\nOptional https port:  %u", wioOptSecureWebServerPort1Get(intIfNum));
  else
    printf("\nNo optional https port 1");

  if (wioOptSecureWebServerPort2Get(intIfNum) != 0)
    printf("\nOptional https port:  %u", wioOptSecureWebServerPort2Get(intIfNum));
  else
    printf("\nNo optional https port 2");

  printf("\nCapabilities mask:  %#x", intfInfo->capabilities);
  osapiSemaGive(wioInfo->wioLock);
}

/* Print the client list */
void wioClientListShow(void)
{
  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;
  _wioClientListShow();
  osapiSemaGive(wioInfo->wioLock);
}

void wioConnListShow(L7_uint32 intIfNum)
{
  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;
  _wioConnListShow(intIfNum);
  osapiSemaGive(wioInfo->wioLock);
}

void wioClientListClear(void)
{
  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;
  wioClientsOnIntfRemove(0);
  osapiSemaGive(wioInfo->wioLock);
}


/* Add a few clients for testing */
void wioTestClientsAdd(L7_uint32 numClients, L7_uchar8 macByte, 
                       L7_uint32 ipAddr, L7_uint32 intIfNum, L7_uint32 vlanId)
{
  L7_uint32 i;

  L7_enetMacAddr_t macAddr = {{macByte,0,0,0,0,0}};

  for (i = 0; i < numClients; i++)
  {
    macAddr.addr[5]++;
    if (macAddr.addr[5] == 0)
    {
      macAddr.addr[4]++;
      if (macAddr.addr[4] == 0)
      {
        macAddr.addr[3]++;
      }
    }
    wioClientAdd(&macAddr, ipAddr + i * 255, intIfNum, vlanId);
    wioClientAuthReq(intIfNum, &macAddr, ipAddr + i * 255);
  }
}


