/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    ospf_debug.c
* @purpose     OSPF Debug functions
* @component   ospf
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "ospf_debug_api.h"
#include "ospf_config.h"
#include "support_api.h"
#include "spobj.h"

static L7_BOOL ospfMapDebugPacketTraceFlag = L7_FALSE;
extern ospfMapDebugCfg_t ospfMapDebugCfg;
void parseOspfPacket(L7_BOOL txFlag,L7_char8 *interface, L7_uint32 source, L7_uint32 dest,t_S_PckHeader *p_Hdr);


#define OSPF_DEBUG_PACKET_RX_FORMAT     "Pkt RX - Intf:%s SrcIp:%s DestIp:%s AreaId:%s Type:%s"
#define OSPF_DEBUG_PACKET_TX_FORMAT     "Pkt TX - Intf:%s SrcIp:%s DestIp:%s AreaId:%s Type:%s"



/*********************************************************************
* @purpose  Turns on/off the displaying of ospf packet debug info
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
L7_RC_t ospfMapDebugPacketTraceFlagSet(L7_BOOL flag)
{
  ospfMapDebugPacketTraceFlag = flag;
  ospfMapDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current status of displaying ospf packet debug info
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
L7_BOOL ospfMapDebugPacketTraceFlagGet()
{
  return ospfMapDebugPacketTraceFlag;
}


/*********************************************************************
* @purpose  Save configuration settings for ospf trace data
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
L7_RC_t ospfMapDebugPacketTraceFlagSave()
{
  ospfMapDebugCfg.cfg.ospfMapDebugPacketTraceFlag = ospfMapDebugPacketTraceFlag;
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
void ospfMapDebugCfgUpdate(void)
{ 
  ospfMapDebugPacketTraceFlagSave();

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
void ospfMapDebugCfgRead(void)
{ 
      /* reset the debug flags*/
    memset((void*)&ospfMapDebugCfg, 0 ,sizeof(ospfMapDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_OSPF_MAP_COMPONENT_ID, OSPFMAP_DEBUG_CFG_FILENAME, 
                         (L7_char8 *)&ospfMapDebugCfg, (L7_uint32)sizeof(ospfMapDebugCfg_t), 
                         &ospfMapDebugCfg.checkSum, OSPFMAP_DEBUG_CFG_VER_CURRENT, 
                         ospfMapDebugBuildDefaultConfigData, L7_NULL);

    ospfMapDebugCfg.hdr.dataChanged = L7_FALSE;

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
void ospfMapDebugRegister(void)
{ 
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_OSPF_MAP_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = ospfMapDebugSave;
    supportDebugDescr.userControl.hasDataChanged = ospfMapDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = ospfMapDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */

    (void) supportDebugRegister(supportDebugDescr);
}

void ospfMapDebugPacketRxTrace(L7_char8 *interface, L7_uint32 source, L7_uint32 dest,L7_char8 *buff)
{

  if(ospfMapDebugPacketTraceFlag != L7_TRUE)
      return;

  parseOspfPacket(L7_FALSE,interface,source,dest,(t_S_PckHeader *)buff);

  return;
}

void ospfMapDebugPacketTxTrace(L7_char8 *interface, L7_uint32 source, L7_uint32 dest,L7_char8 *buff)
{

  if(ospfMapDebugPacketTraceFlag != L7_TRUE)
      return;

  parseOspfPacket(L7_TRUE,interface,source,dest,(t_S_PckHeader *)buff);

  return;
}


void parseOspfPacket(L7_BOOL txFlag,L7_char8 *interface, L7_uint32 source, L7_uint32 dest,t_S_PckHeader *p_Hdr)
{
  
  L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 dstAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 area[OSAPI_INET_NTOA_BUF_SIZE]; 
  char typeString[250];

  osapiInetNtoa(source, srcAddrStr);
  osapiInetNtoa(dest, dstAddrStr);
  osapiInetNtoa(A_GET_4B(p_Hdr->AreaId), area);
  
  switch(p_Hdr->Type)
  {
    case S_HELLO:
      {
          t_S_Hello *p_Hlo;
          L7_uchar8 netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 dRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 bRouterStr[OSAPI_INET_NTOA_BUF_SIZE];
    
    
          p_Hlo = (t_S_Hello *) GET_PAYLOAD_PTR(p_Hdr);
          osapiInetNtoa(A_GET_4B(p_Hlo->NetworkMask), netMaskStr);
          osapiInetNtoa(A_GET_4B(p_Hlo->DesignatedRouter), dRouterStr);
          osapiInetNtoa(A_GET_4B(p_Hlo->BackupRouter), bRouterStr);
          osapiSnprintf(typeString,sizeof(typeString),"HELLO NetMask:%s DesigRouter:%s Backup:%s",
                 netMaskStr,dRouterStr,bRouterStr);        
      }
      break;
    case  S_DB_DESCR:
      {
          t_S_DbDscr *p_DbDscr;  
          L7_uchar8 flagStr[10] = "";
          L7_uchar8 optStr[20];
    
          p_DbDscr =  (t_S_DbDscr *) GET_PAYLOAD_PTR(p_Hdr);
          if (p_DbDscr->Flags & S_INIT_FLAG)
               strncat(flagStr, "I/", 2);
           if (p_DbDscr->Flags & S_MORE_FLAG)
               strncat(flagStr, "M/", 2);
           if (p_DbDscr->Flags & S_MASTER_FLAG)
               strncat(flagStr, "MS", 2); 
           OspfOptionsStr(p_DbDscr->Options, optStr);
    
           osapiSnprintf(typeString,sizeof(typeString),"DB_DSCR Mtu:%d Options:%s Flags:%s Seq:%lu",
                 A_GET_2B(p_DbDscr->InterfaceMtu),optStr,flagStr,A_GET_4B(p_DbDscr->SeqNum));
      }
      break;
    case  S_LS_REQUEST:
      {
          t_S_LsReq *p_LsReq;
    
          p_LsReq = (t_S_LsReq *) GET_PAYLOAD_PTR(p_Hdr);
          
          osapiSnprintf(typeString,sizeof(typeString),
                "LS_REQ Length:%d",A_GET_2B(p_Hdr->Length));      
      }
      break;
    case  S_LS_UPDATE:
      {
          t_S_LsaHeader *p_LsaHdr;
    
          p_LsaHdr = (t_S_LsaHeader *)GET_PAYLOAD_PTR(p_Hdr);
          osapiSnprintf(typeString,sizeof(typeString),
                        "LS_UPD Length:%d",A_GET_2B(p_Hdr->Length));      
      }
      break;
    case  S_LS_ACK:
      {
          t_S_LsaHeader *p_LsaHdr;
    
          p_LsaHdr = (t_S_LsaHeader *)GET_PAYLOAD_PTR(p_Hdr);
          osapiSnprintf(typeString,sizeof(typeString),
                "LS_ACK Length:%d",A_GET_2B(p_Hdr->Length));      
      }
      break;
    default:
      return;  
 }

  if(txFlag) 
  {
      OSPF_USER_TRACE(OSPF_DEBUG_PACKET_TX_FORMAT,interface,srcAddrStr,dstAddrStr,area,typeString);
  }      
  else
  {
      OSPF_USER_TRACE(OSPF_DEBUG_PACKET_RX_FORMAT,interface,srcAddrStr,dstAddrStr,area,typeString);
  }

  return;

}

