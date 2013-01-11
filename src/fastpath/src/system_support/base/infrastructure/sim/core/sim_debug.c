

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    sim_debug.c
* @purpose     sim debug functions
* @component   sim
* @comments    none
* @create      08/25/2006
* @author      ssuvarna
* @end
*
**********************************************************************/

#include "log.h"
#include "l7_common.h"
#include "l7_icmp.h"
#include "sim.h"
#include "sim_debug_api.h"
#include "support_api.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"

#include "usmdb_sim_api.h"
#include "nvstoreapi.h"
#define PING_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d,%s), SRC_IP:%s, DEST_IP:%s, Type:%s"
#define PING_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d,%s), SRC_IP:%s, DEST_IP:%s, Type:%s"

#define PING_USER_TRACE(__fmt__, __args__... )                              \
          if (pingDebugPacketTraceFlag == L7_TRUE)                          \
          {                                                                 \
            LOG_USER_TRACE(L7_SIM_COMPONENT_ID, __fmt__,##__args__);        \
          }

static L7_BOOL pingDebugPacketTraceFlag = L7_FALSE;
extern simDebugCfg_t simDebugCfg;
extern simOperInfo_t *simOperInfo;

/*********************************************************************
*
* @purpose Trace ping packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void pingDebugPacketRxTrace(L7_uint32 intIfNum, L7_uchar8 *buff)
{
  L7_ipHeader_t       *ipHdr;
  struct icmp         *icmpPtr;
  L7_uint32 unit, slot, port;
  L7_char8 srcIp[20];
  L7_char8 destIp[20];
  L7_char8 type[20];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if(pingDebugPacketTraceFlag != L7_TRUE)
      return;

  ipHdr = (L7_ipHeader_t *)(&buff[0]);

  osapiInetNtoa(osapiNtohl(ipHdr->iph_dst),destIp);
  osapiInetNtoa(osapiNtohl(ipHdr->iph_src),srcIp);

  icmpPtr = (struct icmp *)((L7_uchar8 *)ipHdr + L7_IP_HDR_LEN);

  if (icmpPtr->icmp_type == ICMP_ECHO)
  {
      osapiSnprintf(type, sizeof(type), "ECHO_REQUEST");
  }
  else if(icmpPtr->icmp_type == ICMP_ECHOREPLY)
  {
      osapiSnprintf(type, sizeof(type), "ECHO_REPLY");
  }
  else
  {
      return;
  }

  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    PING_USER_TRACE(PING_DEBUG_PACKET_RX_FORMAT,
                      unit, slot, port,intIfNum,ifName,srcIp,destIp,type);
  }


  return;
}

/*********************************************************************
*
* @purpose Trace ping packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void pingDebugPacketTxTrace(L7_uint32 intIfNum, L7_uchar8 *buff)
{L7_enetHeader_t     *enetHdr;
  L7_enet_encaps_t    *encap;
  L7_ipHeader_t       *ipHdr;
  struct icmp         *icmpPtr;
  L7_uint32 unit, slot, port;
  L7_char8 srcIp[20];
  L7_char8 destIp[20];
  L7_char8 type[20];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if(pingDebugPacketTraceFlag != L7_TRUE)
      return;

  enetHdr = (L7_enetHeader_t *)(&buff[0]);
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  ipHdr = (L7_ipHeader_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);

  osapiInetNtoa(osapiNtohl(ipHdr->iph_dst),destIp);
  osapiInetNtoa(osapiNtohl(ipHdr->iph_src),srcIp);

  icmpPtr = (struct icmp *)((L7_uchar8 *)ipHdr + L7_IP_HDR_LEN);

  if (icmpPtr->icmp_type == ICMP_ECHO)
  {
      osapiSnprintf(type, sizeof(type), "ECHO_REQUEST");
  }
  else if(icmpPtr->icmp_type == ICMP_ECHOREPLY)
  {
      osapiSnprintf(type, sizeof(type), "ECHO_REPLY");
  }
  else
  {
      return;
  }

  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    PING_USER_TRACE(PING_DEBUG_PACKET_TX_FORMAT,
                      unit, slot, port,intIfNum,ifName,srcIp,destIp,type);
  }
  return;
}

/*********************************************************************
* @purpose  Get the current status of displaying ping packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL pingDebugPacketTraceFlagGet()
{
  return pingDebugPacketTraceFlag;
}


/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pingDebugPacketTraceFlagSet(L7_BOOL flag)
{
  pingDebugPacketTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
    */

  simDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for ping trace data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pingDebugPacketTraceFlagSave()
{
  simDebugCfg.cfg.pingDebugPacketTraceFlag = pingDebugPacketTraceFlag;
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simDebugCfgUpdate(void)
{
  pingDebugPacketTraceFlagSave();

}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simDebugCfgRead(void)
{
      /* reset the debug flags*/
    memset((void*)&simDebugCfg, 0 ,sizeof(simDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_SIM_COMPONENT_ID, SIM_DEBUG_CFG_FILENAME,
                         (L7_char8 *)&simDebugCfg, (L7_uint32)sizeof(simDebugCfg_t),
                         &simDebugCfg.checkSum, SIM_DEBUG_CFG_VER_CURRENT,
                         simDebugBuildDefaultConfigData, L7_NULL);

    simDebugCfg.hdr.dataChanged = L7_FALSE;

}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_SIM_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = simDebugSave;
    supportDebugDescr.userControl.hasDataChanged = simDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = simDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */

    (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
* @purpose  To clear the running config and come up with factory defaults
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/

void simDebugClearConfig()
{
  usmDbResetConfigActionSet(0,3);
}

/*********************************************************************
* @purpose  To set the system config mode
*
* @param    DHCP/NONE
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
int simDebugSetSystemConfigMode(int mode)
{
  if (mode != 1 && mode != 3)
  {
    printf("Mode '%d' - Invalid value for mode !!!\nEnter 1 (for NONE) or 3 (for DHCP)\n", mode);
    return -1;
  }
  else
  {
    printf("Setting config mode to '%s' ... ", (mode == 1) ? "NONE" : "DHCP");
    simSetSystemConfigMode(mode);
  }

  return 0;
}

/*********************************************************************
* @purpose  To save the running config
*
* @param    DHCP/NONE
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simSaveDebugConfig()
{
  (void)nvStoreSave(4);
}

/*********************************************************************
* @purpose  To display the address conflicts stats in the switch
*
* @param    none
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simAddrConflictStatsShow()
{
  char      ifname[20];
  L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];

  if (((usmDbComponentPresentCheck(0, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)) &&
      (simOperInfo->servPortIpAddress))
  {
    memset(ipAddrStr, 0, sizeof(ipAddrStr));
    osapiSnprintf(ifname, sizeof(ifname), "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
    sysapiPrintf("Interface - Service Port (%s)\n", ifname);
    osapiInetNtoa(simOperInfo->servPortIpAddress, ipAddrStr);
    sysapiPrintf("    Address               -   %s\n", ipAddrStr);
    sysapiPrintf("    Number of conflicts   -   %d\n\n",
                 simOperInfo->servicePortConflictStats.numOfConflictsDetected);
  }
  if(simOperInfo->systemIpAddress)
  {
    memset(ipAddrStr, 0, sizeof(ipAddrStr));
    osapiSnprintf(ifname, sizeof(ifname), "%s0", L7_DTL_PORT_IF);
    sysapiPrintf("Interface - Network Port (%s)\n", ifname);
    osapiInetNtoa(simOperInfo->servPortIpAddress, ipAddrStr);
    sysapiPrintf("    Address               -   %s\n", ipAddrStr);
    sysapiPrintf("    Number of conflicts   -   %d\n\n",
                 simOperInfo->networkPortConflictStats.numOfConflictsDetected);
  }

#ifdef L7_ROUTING_PACKAGE
  ipMapAddrConflictStatsShow();
#endif
}

