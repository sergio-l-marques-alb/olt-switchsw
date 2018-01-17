/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 * @filename  vrrp_debug.c
 *
 * @purpose   vrrp Debug functions
 *
 * @component vrrp
 *
 * @comments  none
 *
 * @create
 *
 * @author    P.Sowjanya
 *
 * @end
 *
 **********************************************************************/

#include "support_api.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"

#include "log.h"
/*#include "debug.h"*/

#include "vrrp_debug.h"
#include "vrrp_debug_api.h"
#include "vrrp_config.h"
#include "vrrp_util.h"
#include "vrrptask.h"
#define VRRP_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf:%d/%d/%d(%d,%s),SrcIP:%s,DestIp:%s,Version:%d,PktType:%d,VrId: %d,Priority: %d,Count IP Addrs:%d,AuthType:%d,Adver int:%d,AuthData :%s,Addresses:%s"
#define VRRP_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d,%s),SrcIP:%s,DestIp :%s,Version:%d,PktType:%d,VrId:%d,Priority:%d,Count IP Addrs:%d,AuthType:%d,Adver int:%d,AuthData :%s,Addresses:%s"

static L7_BOOL vrrpDebugPacketTraceFlag = L7_FALSE;

extern vrrpDebugCfg_t vrrpDebugCfg;


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
void vrrpDebugRegister(void)
{
  supportDebugDescr_t supportDebugDescr;

  memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

  supportDebugDescr.componentId = L7_VRRP_MAP_COMPONENT_ID;

  /* Register User Control Parms */
  supportDebugDescr.userControl.notifySave = vrrpDebugSave;
  supportDebugDescr.userControl.hasDataChanged = vrrpDebugHasDataChanged;
  supportDebugDescr.userControl.clearFunc = vrrpDebugRestore;

  /* Register Internal Parms */

  (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
 *
 * @purpose Trace vrrp packets received
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void vrrpDebugPacketRxTrace(L7_uint32 intIfNum, L7_uchar8* msg)
{
    L7_uint32 unit, slot, port;
      L7_uint32 i;
      L7_ipHeader_t       *ipHdr;
      vrrpPkt_t *pdu;
      L7_uchar8 destIp[20];
      L7_uchar8 srcIp[20];
      L7_uchar8 address[L7_L3_NUM_IP_ADDRS*20];
      L7_uchar8 auth_data[VRRP_AUTH_DATA_LEN];
      L7_uchar8 ipAddress[20];
      L7_uchar8 version, type;
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      if ( vrrpDebugPacketTraceFlag != L7_TRUE)
      return;

      ipHdr = (L7_ipHeader_t*)msg;
      pdu = (vrrpPkt_t *)((L7_char8 *)ipHdr + sizeof(L7_ipHeader_t));

      memset(address,0,sizeof(address));
      memset (auth_data, 0, VRRP_AUTH_DATA_LEN);

      for (i=0; i<pdu->naddr; i++)
      {
        osapiInetNtoa(pdu->ipaddress[i],ipAddress);
        osapiStrncat(address,ipAddress,sizeof(ipAddress));
        if ((i+1) != pdu->naddr)
        {
          osapiStrncat(address,",", sizeof(","));
        }
      }

      if (pdu->auth_type == L7_AUTH_TYPE_SIMPLE_PASSWORD)
      {
        memcpy(auth_data, (L7_uchar8*)&(pdu->ipaddress[pdu->naddr]), VRRP_AUTH_DATA_LEN);
      }
      osapiInetNtoa(ipHdr->iph_dst,destIp);
      osapiInetNtoa(ipHdr->iph_src,srcIp);
      version = pdu->version_type >> 4;
      type = pdu->version_type & 0x0f;
  /* Convert from intIfNum to USP */

    if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
      VRRP_USER_TRACE(VRRP_DEBUG_PACKET_RX_FORMAT,
      unit, slot, port, intIfNum, ifName, srcIp,destIp, version, type, pdu->vrid, pdu->priority, pdu->naddr, pdu->auth_type, pdu->adver_int, auth_data, address);
      }
  return;
}

/*********************************************************************
 *
 * @purpose Trace vrrp packets transmitted
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void vrrpDebugPacketTxTrace(L7_uint32 intIfNum, L7_uchar8* vrrp_buff)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 destIp[20];
  L7_uchar8 srcIp[20];
  L7_uint32 i;
  L7_ipHeader_t ipHdr;
  L7_uchar8 ipAddress[20];
  L7_uchar8 address[L7_L3_NUM_IP_ADDRS*20];
  L7_uchar8 auth_data[VRRP_AUTH_DATA_LEN];
  vrrpPkt_t *pdu;
  L7_uchar8 version, type;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

   memset(address,0,sizeof(address));
   memset(auth_data,0,VRRP_AUTH_DATA_LEN);
   if ( vrrpDebugPacketTraceFlag != L7_TRUE)
         return;
   memcpy(&ipHdr, vrrp_buff, sizeof(L7_ipHeader_t));
   pdu =(vrrpPkt_t *) (vrrp_buff + sizeof(L7_ipHeader_t));

   osapiInetNtoa(ipHdr.iph_dst,destIp);
   osapiInetNtoa(ipHdr.iph_src,srcIp);

    for (i=0; i< pdu->naddr; i++)
    {
      if(pdu->ipaddress[i] == 0)
        break;
      osapiInetNtoa(pdu->ipaddress[i],ipAddress);
      osapiStrncat(address,ipAddress,sizeof(ipAddress));
      if ((i+1) != pdu->naddr)
      {
        osapiStrncat(address,",", sizeof(","));
      }
    }
   version = pdu->version_type >> 4;
   type = pdu->version_type & 0x0f;

   if (pdu->auth_type == L7_AUTH_TYPE_SIMPLE_PASSWORD)
   {
     memcpy(auth_data, (L7_uchar8*)&(pdu->ipaddress[pdu->naddr]), VRRP_AUTH_DATA_LEN);
   }

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)

  {
    VRRP_USER_TRACE(VRRP_DEBUG_PACKET_TX_FORMAT,
        unit, slot, port, intIfNum, ifName, srcIp, destIp, version, type, pdu->vrid, pdu->priority,pdu->naddr, pdu->auth_type, pdu->adver_int, auth_data, address);
  }

  return;
}
/*********************************************************************
 * @purpose  Get the current status of displaying vrrp packet debug info
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
L7_BOOL vrrpDebugPacketTraceFlagGet()
{
  return vrrpDebugPacketTraceFlag;
}


/*********************************************************************
 * @purpose  Turns on/off the displaying of vrrp packet debug info
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
L7_RC_t vrrpDebugPacketTraceFlagSet(L7_BOOL flag)
{
  vrrpDebugPacketTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */

  vrrpDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Save configuration settings for vrrp trace data
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
L7_RC_t vrrpDebugPacketTraceFlagSave()
{
  vrrpDebugCfg.cfg.vrrpDebugPacketTraceFlag = vrrpDebugPacketTraceFlag;
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
void vrrpDebugCfgUpdate(void)
{
  vrrpDebugPacketTraceFlagSave();

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
void vrrpDebugCfgRead(void)
{

  /* reset the debug flags*/
  memset((void*)&vrrpDebugCfg, 0 ,sizeof(vrrpDebugCfg_t));

  (void)sysapiSupportCfgFileGet(L7_VRRP_MAP_COMPONENT_ID, VRRP_DEBUG_CFG_FILENAME,
                                (L7_char8 *)&vrrpDebugCfg, (L7_uint32)sizeof(vrrpDebugCfg_t),
                                &vrrpDebugCfg.checkSum, VRRP_DEBUG_CFG_VER_CURRENT,
                                vrrpDebugBuildDefaultConfigData, L7_NULL);

  vrrpDebugCfg.hdr.dataChanged = L7_FALSE;
}


