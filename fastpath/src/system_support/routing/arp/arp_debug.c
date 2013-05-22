/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  arp_debug.c
*
* @purpose   arp Debug functions
*
* @component arp
*
* @comments  none
*
* @create
*
* @author    sowjanya
*
* @end
*
**********************************************************************/
#include "support_api.h"
#include "l7_common.h"
#include "osapi.h"

#include "log.h"
#include "buffer.h"
#include "buffer.ext"
#include "frame.h"
#include "nvstoreapi.h"
/*#include "debug.h"*/

#include "arp_debug.h"
#include "arp_debug_api.h"
#include "ipmap_arp.h"
#include "nimapi.h"

#define ARP_DEBUG_PACKET_RX_FORMAT "Pkt RX -  SrcMac: %s DestMac: %s SrcIP: %s DestIP: %s Type: %s H/wType: %s ProType: %s"
#define ARP_DEBUG_PACKET_TX_FORMAT "Pkt TX -  Intf: %d/%d/%d(%d,%s),SrcMac: %s DestMac: %s SrcIP: %s DestIP: %s Type: %s H/wType: %s ProType: %s"

static L7_BOOL arpDebugPacketTraceFlag = L7_FALSE;

arpDebugCfg_t arpDebugCfg;
L7_BOOL arpPacketDebugFlag;

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
void arpDebugRegister(void)
{
  supportDebugDescr_t supportDebugDescr;

  memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

  supportDebugDescr.componentId = L7_IP_MAP_COMPONENT_ID;

  /* Register User Control Parms */
  supportDebugDescr.userControl.notifySave = arpDebugSave;
  supportDebugDescr.userControl.hasDataChanged = arpDebugHasDataChanged;
  supportDebugDescr.userControl.clearFunc = arpDebugRestore;

  (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
*
* @purpose Trace arp packets received
*
* @param   intIfNum     @b{(input)} Internal Interface Number
* @param   vlanId       @b{(input)} Vlan ID
* @param   *igmpPacket  @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void arpDebugPacketRxTrace( t_ARPFrame *arpFrame)
{
  L7_char8 srcMac[20];
  L7_char8 dstMac[20];
  L7_char8 srcIp[20];
  L7_char8 dstIp[20];
  L7_char8 type[25];
  L7_char8 hardwareType[25];
  L7_char8 protocolType[25];
  word hwType;
  word prType;
  word opCode;


  if ( arpDebugPacketTraceFlag != L7_TRUE)
    return;

  osapiSnprintf(srcIp, sizeof(srcIp), "%d.%d.%d.%d",arpFrame->srcIP[0],arpFrame->srcIP[1],
      arpFrame->srcIP[2],arpFrame->srcIP[3]);
  osapiSnprintf(dstIp, sizeof(dstIp), "%d.%d.%d.%d",arpFrame->dstIP[0],arpFrame->dstIP[1],
      arpFrame->dstIP[2],arpFrame->dstIP[3]);


  osapiSnprintf(srcMac, sizeof(srcMac), "%02x:%02x:%02x:%02x:%02x:%02x",arpFrame->srcMAC[0],arpFrame->srcMAC[1],
      arpFrame->srcMAC[2],arpFrame->srcMAC[3],arpFrame->srcMAC[4],arpFrame->srcMAC[5]);
  osapiSnprintf(dstMac, sizeof(dstMac), "%02x:%02x:%02x:%02x:%02x:%02x",arpFrame->dstMAC[0],arpFrame->dstMAC[1],
      arpFrame->dstMAC[2],arpFrame->dstMAC[3],arpFrame->dstMAC[4],arpFrame->dstMAC[5]);

  L7_NTOHS(&arpFrame->opCode, &opCode);
  if (opCode == ARP_REQUEST)
  {
    osapiSnprintf(type, sizeof(type), "ARP_REQUEST");
  }
  else if (opCode == ARP_RESPONSE)
  {
    osapiSnprintf(type, sizeof(type), "ARP_RESPONSE");
  }
  else
    osapiSnprintf(type, sizeof(type), "UnKnown");

  L7_NTOHS(&arpFrame->hwType, &hwType);
   if(hwType != HW_ETHERNET)
   {
     osapiSnprintf(hardwareType, sizeof(hardwareType), "Unknown");

   }
   else
   {
     osapiSnprintf(hardwareType, sizeof(hardwareType), "Ethernet");
   }
  L7_NTOHS(&arpFrame->prType, &prType);
   if(prType != ETHERTYPE_IP)
   {
     osapiSnprintf(protocolType, sizeof(protocolType), "Unknown");
   }
   else
   {
     osapiSnprintf(protocolType, sizeof(protocolType), "IP");
   }

  ARP_USER_TRACE(ARP_DEBUG_PACKET_RX_FORMAT, srcMac, dstMac, srcIp, dstIp,type,hardwareType, protocolType);

  return;
}

/*********************************************************************
*
* @purpose Trace arp packets transmitted
*
* @param   intIfNum     @b{(input)} Internal Interface Number
* @param   vlanId       @b{(input)} Vlan ID
* @param   *igmpPacket  @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void arpDebugPacketTxTrace(L7_uint32 intIfNum,t_ARPFrame *arpFrame)
{
  L7_uint32 unit, slot, port;
  L7_char8 srcMac[20];
  L7_char8 destMac[20];
  L7_char8 srcIp[20];
  L7_char8 dstIp[20];
  L7_char8 type[25],hardwareType[25], protocolType[25];
  word opCode,hwType,prType;
  nimUSP_t usp;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if(arpDebugPacketTraceFlag != L7_TRUE)
    return;

  osapiSnprintf(srcIp, sizeof(srcIp), "%d.%d.%d.%d",arpFrame->srcIP[0],arpFrame->srcIP[1],
      arpFrame->srcIP[2],arpFrame->srcIP[3]);
  osapiSnprintf(dstIp, sizeof(dstIp), "%d.%d.%d.%d",arpFrame->dstIP[0],arpFrame->dstIP[1],
      arpFrame->dstIP[2],arpFrame->dstIP[3]);

  osapiSnprintf(srcMac, sizeof(srcMac), "%02x:%02x:%02x:%02x:%02x:%02x",arpFrame->srcMAC[0],arpFrame->srcMAC[1],
      arpFrame->srcMAC[2],arpFrame->srcMAC[3],arpFrame->srcMAC[4],arpFrame->srcMAC[5]);
  osapiSnprintf(destMac, sizeof(destMac), "%02x:%02x:%02x:%02x:%02x:%02x",arpFrame->dstMAC[0],arpFrame->dstMAC[1],
      arpFrame->dstMAC[2],arpFrame->dstMAC[3],arpFrame->dstMAC[4],arpFrame->dstMAC[5]);

  L7_NTOHS(&(arpFrame->opCode), &opCode);
  if (opCode == ARP_REQUEST)
  {
    osapiSnprintf(type, sizeof(type), "ARP_REQUEST");
  }
  else if (opCode == ARP_RESPONSE)
  {
    osapiSnprintf(type, sizeof(type), "ARP_RESPONSE");
  }
  else osapiSnprintf(type, sizeof(type), "UnKnown");

  L7_NTOHS(&arpFrame->hwType, &hwType);
  if(hwType != HW_ETHERNET)
  {
    osapiSnprintf(hardwareType, sizeof(hardwareType), "Unknown");

  }
  else
  {
    osapiSnprintf(hardwareType, sizeof(hardwareType), "Ethernet");
  }

  L7_NTOHS(&arpFrame->prType, &prType);
  if(prType != ETHERTYPE_IP)
  {
    osapiSnprintf(protocolType, sizeof(protocolType), "Unknown");
  }
  else
  {
    osapiSnprintf(protocolType, sizeof(protocolType), "IP");
  }


  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    unit = usp.unit;
    slot = usp.slot;
    port = usp.port;
    ARP_USER_TRACE(ARP_DEBUG_PACKET_TX_FORMAT,
        unit, slot, port, intIfNum, ifName, srcMac, destMac, srcIp, dstIp,type, hardwareType, protocolType);
  }

  return;
}


/*********************************************************************
* @purpose  Get the current status of displaying arp packet debug info
*
* @param    none
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL arpDebugPacketTraceFlagGet()
{
  return  arpDebugPacketTraceFlag;
}

/*********************************************************************
* @purpose  Turns on/off the displaying of arp packet debug info
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
L7_RC_t arpDebugPacketTraceFlagSet(L7_BOOL flag)
{
  arpDebugPacketTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */

  arpDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Save configuration settings for arp trace data
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
L7_RC_t arpDebugPacketTraceFlagSave()
{
  arpDebugCfg.cfg.arpDebugPacketTraceFlag = arpDebugPacketTraceFlag;
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
void arpDebugCfgUpdate(void)
{
  arpDebugPacketTraceFlagSave();

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
void arpDebugCfgRead(void)
{
  /* reset the debug flags*/
  memset((void*)&arpDebugCfg, 0 ,sizeof(arpDebugCfg_t));
  (void)sysapiSupportCfgFileGet(L7_IP_MAP_COMPONENT_ID, ARP_DEBUG_CFG_FILENAME,
                                (L7_char8 *)&arpDebugCfg, (L7_uint32)sizeof(arpDebugCfg_t),
                                &arpDebugCfg.checkSum, ARP_DEBUG_CFG_VER_CURRENT,
                                arpDebugBuildDefaultConfigData, L7_NULL);

  arpDebugCfg.hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
 * @purpose  Saves arp configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    arpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t arpDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  arpDebugCfgUpdate();

  if (arpDebugHasDataChanged() == L7_TRUE)
  {
    arpDebugCfg.hdr.dataChanged = L7_FALSE;
    arpDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&arpDebugCfg,
        (L7_uint32)(sizeof(arpDebugCfg) - sizeof(arpDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_IP_MAP_COMPONENT_ID, ARP_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&arpDebugCfg, (L7_uint32)sizeof(arpDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ARP_MAP_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",ARP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores arp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    arpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t arpDebugRestore(void)
{
  L7_RC_t rc;

  arpDebugBuildDefaultConfigData(ARP_DEBUG_CFG_VER_CURRENT);

  arpDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = arpApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if arp debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL arpDebugHasDataChanged(void)
{
  return arpDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default arp config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void arpDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  arpDebugCfg.hdr.version = ver;
  arpDebugCfg.hdr.componentID = L7_IP_MAP_COMPONENT_ID;
  arpDebugCfg.hdr.type = L7_CFG_DATA;
  arpDebugCfg.hdr.length = (L7_uint32)sizeof(arpDebugCfg);
  strcpy((L7_char8 *)arpDebugCfg.hdr.filename, ARP_DEBUG_CFG_FILENAME);
  arpDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&arpDebugCfg.cfg, 0, sizeof(arpDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply arp debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t arpApplyDebugConfigData(void)
{
  L7_RC_t rc;
  rc = arpDebugPacketTraceFlagSet(arpDebugCfg.cfg.arpDebugPacketTraceFlag);

  return rc;
}



