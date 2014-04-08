/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#include "ptin_utils_inet_addr_api.h"
#include "ptin_mgmd_core.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_service_api.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_proxytimer.h"
#include "ptin_mgmd_routercmtimer.h"
#include "ptin_mgmd_proxycmtimer.h"
#include "ptin_mgmd_querytimer.h"
#include "ptin_mgmd_avl_api.h"
#include "ptin_mgmd_service_api.h"
#include "ptin_mgmd_cnfgr.h"
#include "ptin_mgmd_groupsourcespecifictimer.h"

#include <ctype.h>
#include <time.h>
#include <stdlib.h>

/*********************************************************************
* Static Methods
*********************************************************************/
static RC_t                   snoopPTinReportFrameV3Build(uint32 noOfRecords, mgmdGroupRecord_t* groupPtr, uchar8 *buffer, uint32 *length);
static RC_t                   ptinIgmpV2FrameBuild(uint8 igmpType,mgmdGroupRecord_t* groupPtr, uchar8 *buffer, uint32 *length);
static uchar8*                snoopPTinGroupRecordV3Build(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType,uint16 numberOfSources,snoopPTinSourceRecord_t* source, uchar8 *buffer, uint32 *length);
static RC_t                   snoopPTinPacketBuild(uint32 serviceId, ptinMgmdControlPkt_t* mcastPacket, uchar8* igmpFrameBuffer, uint32 igmpFrameLength,uint32 packetType);
static RC_t                   snoopPTinReportSend(uint32 serviceId, mgmdGroupRecord_t     *groupPtr, uint32 noOfGroupRecords, ptin_IgmpProxyCfg_t* igmpCfg);
static mgmdGroupRecord_t*     snoopPTinGroupRecordIncrementTransmissions(uint32 noOfRecords,mgmdGroupRecord_t* groupPtr, uint32* newNoOfRecords,uint8 robustnessVariable);
static RC_t                   snoopPTinGroupRecordSourceIncrementTransmissions(mgmdGroupRecord_t* groupPtr,uint8 robustnessVariable);
static mgmdGroupRecord_t*     mgmdBuildIgmpv3CSR(mgmdProxyInterface_t *interfacePtr, uint32 *noOfRecords);
static void                   ptinMgmdDumpGroupSpecificQuery(void);


/*****************************************************************
* @purpose  calculates the selected delay from the max response time
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns  the selected delay for a
   response is randomly selected in the range (0, [Max Resp Time]) where
   Max Resp Time is derived from Max Resp Code in the received Query
   message.
* @notes  
*

* @end
*********************************************************************/
int32 ptinMgmd_generate_random_response_delay (int32 maxResponseTime)
{
  int32 selectedDelay;
#if 0
  if(maxResponseTime<=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL)
    selectedDelay=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  else
  {
    selectedDelay = rand() % maxResponseTime +1;
    if (selectedDelay<PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL)
      selectedDelay=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  }
#else
  selectedDelay = rand() % maxResponseTime +1;
#endif
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Selected Delay :%u",selectedDelay);  
  return(selectedDelay);
}

/*********************************************************************
 * @purpose Method responsible for scheduling Group-Specific or
 *          Group/Source-Specific Queries
 *
 * @param   serviceId   Service ID
 * @param   groupAddr   IGMP Group address
 * @param   sFlag       Suppress router-side processing flag
 * @param   sources     Source list
 * @param   sourcesCnt  Number of sources in source list
 *
 * @returns  SUCCESS
 * @returns  FAILURE
 *
 * @see RFC 3376 6.6.3.1/6.6.3.2
 *
 *********************************************************************/
RC_t ptinMgmdScheduleReportMessage(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr, uint8  igmpType,uint32 timeOut, BOOL isInterface,uint32 noOfRecords, void* ptr)
{
  ptin_IgmpProxyCfg_t        igmpCfg;  
  uint32                     newNoOfRecords=0;                             
  int64                      noOfPendingRecords;  
  mgmdGroupRecord_t         *groupPtr=PTIN_NULLPTR,*newgroupPtr=PTIN_NULLPTR;
  mgmdProxyInterface_t      *interfacePtr=PTIN_NULLPTR;
  void*                      ptrVoid; //groupPtr or interfacePtr
  mgmdProxyInterfaceTimer_t *proxyTimer;
  ptinMgmdGroupInfoData_t   *avlTreeEntry;
  
  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  /* Argument validation */
  if (groupAddr==PTIN_NULLPTR || ptr == PTIN_NULLPTR )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments [groupAddr=%p ptr=%p] }", groupAddr, ptr);
    return FAILURE;
  }
   
 /* Get proxy configurations */
  if (ptin_mgmd_igmp_proxy_config_get(&igmpCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations!");
    return FAILURE;
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "timeOut:%u; igmpType:0x%x; isInterface:%u;  noOfRecords:%u; robustnessVariable:%u",timeOut,igmpType,isInterface,noOfRecords,igmpCfg.host.robustness);

  if (isInterface==TRUE)
  {    
    interfacePtr=(mgmdProxyInterface_t*) ptr;
    proxyTimer=&interfacePtr->timer;
    newgroupPtr=groupPtr=interfacePtr->firstGroupRecord;    
  }
  else
  {
    newgroupPtr=groupPtr=(mgmdGroupRecord_t*) ptr;
    proxyTimer=&groupPtr->timer;
    interfacePtr=groupPtr->interfacePtr;
  }

  if (timeOut!=0)
  {    
    /*Schedule a Membership Report Message to answer to a Query Event*/
    if (ptin_mgmd_proxytimer_start(proxyTimer,timeOut,igmpType, isInterface,noOfRecords,ptr)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()}");
      return FAILURE;
    }
    proxyTimer->isFirstTransmission=TRUE;
    return SUCCESS;
  }
  //else
  /*We need to send right away this Membership Report Message*/  

  //This applies only to the Current Group Records
  if (proxyTimer->isFirstTransmission==TRUE)
  {
    //Response to a General Query 
    if (isInterface==TRUE)
    {             
      if ((groupPtr=mgmdBuildIgmpv3CSR(interfacePtr,&noOfRecords))==PTIN_NULLPTR)
      {
        if (noOfRecords>0)
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed snoopPTinBuildCSR()}");
          return FAILURE;
        }        
        else
        {
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Membership Response to General Query silenty discarded, once we do not have any active groups}");
          return SUCCESS;
        }
      }
      if (interfacePtr->firstGroupRecord==PTIN_NULLPTR)
      {         
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "First Group Record has a NULL Pointer}");  
        return FAILURE;
      }
    }
    //Response to a Group Specific Query or Group and Source
    else
    {      
      if (igmpType==PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY || igmpType==PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY)
      {
        /*Let us verify if this group still has any clients*/
        if ((avlTreeEntry=ptinMgmdL3EntryFind(serviceId, groupAddr, AVL_EXACT))==PTIN_NULLPTR || 
            avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==FALSE)            
        {
          PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Membership Response to Group Query silenty discarded, once this group is no longer active");
          return SUCCESS;          
        }
        if (igmpType==PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY)
        {
          if (groupPtr->numberOfSources==0)
          {
            PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Report Type=SNOOP_PTIN_GROUP_AND_SOURCE_SPECIFIC_QUERY, while numberOfSources=0");
            return SUCCESS;
          }
        }
      }
    }
    proxyTimer->isFirstTransmission=FALSE;
  }
  //End Current Group Record

  /*We need to split the report if the number of records is higher then igmpCfg.host.max_records_per_report*/
  for (noOfPendingRecords=noOfRecords;noOfPendingRecords>0 && newgroupPtr!=PTIN_NULLPTR;noOfPendingRecords-=igmpCfg.host.max_records_per_report)
  {    
    if (noOfPendingRecords>igmpCfg.host.max_records_per_report)
    {
      if (snoopPTinReportSend(serviceId,newgroupPtr,igmpCfg.host.max_records_per_report,&igmpCfg)!=SUCCESS)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinReportSend()");
        return FAILURE;
      }
#if 0 
//      newgroupPtr=newgroupPtr+igmpCfg.host.max_records_per_report;       
#else //Since the group records are not sequencialy ordered on the memory, we cannot perform any arithmetic operation with pointers. Therefore we need to move the pointer record by record.
      uint32 numberOfRecords;
      for (numberOfRecords=0;numberOfRecords<igmpCfg.host.max_records_per_report && newgroupPtr!=PTIN_NULLPTR ;numberOfRecords++)
      {
        newgroupPtr=newgroupPtr->nextGroupRecord;
      }
#endif
    }
    else
    {
      if (snoopPTinReportSend(serviceId,newgroupPtr,noOfPendingRecords,&igmpCfg)!=SUCCESS)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinReportSend()");
        return FAILURE;
      }
    }   
  }
//________________________________________________________________________________________________________________

  //If this igmp packet is a response to a Query the robustness variable assumes the value of 1.  
  uint8 robustness;
  if(igmpType==PTIN_IGMP_MEMBERSHIP_GENERAL_QUERY || igmpType==PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY || igmpType==PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY)
  {
    robustness=1;
  }
  else
  {
    robustness=igmpCfg.host.robustness;
  }

  if ((newgroupPtr=snoopPTinGroupRecordIncrementTransmissions(noOfRecords,groupPtr,&newNoOfRecords,robustness))==PTIN_NULLPTR && newNoOfRecords>0)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordIncrementTransmissions()");
    return FAILURE;
  }

  if (newNoOfRecords>0)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Re-Scheduling Membership Report Message with random value between [0 - %u]ms",igmpCfg.host.unsolicited_report_interval*1000);        
    {//We need to save the pointer 
      if (isInterface==TRUE)
          ptrVoid=ptr;          
      else
          ptrVoid=newgroupPtr;
          proxyTimer=&newgroupPtr->timer;
    }    
    if (ptin_mgmd_proxytimer_start(proxyTimer,ptinMgmd_generate_random_response_delay(igmpCfg.host.unsolicited_report_interval*1000),igmpType, isInterface,newNoOfRecords,ptrVoid)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
      return FAILURE;
    }
  }   

  if(ptin_mgmd_extended_debug)
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");

  return SUCCESS;
}

/*********************************************************************
* @purpose  Create an IGMPv3 Frame for Membership Report messages
*
* @param    noOfRecords     Number of Group Records 
* @param    groupPtr        Group Record Pointer
* @param    buffer          Buffer in which the IGMPv3 Frame will be
*                           placed
* @param    length          IGMPv3 Frame length
*
* @returns  SUCCESS
* @returns  FAILURE
*
*
*********************************************************************/
static RC_t snoopPTinReportFrameV3Build(uint32 noOfRecords, mgmdGroupRecord_t* groupPtr, uchar8 *buffer, uint32 *length)
{
  uchar8         *dataPtr,*chksumPtr, byteVal;
  ushort16       shortVal;
  uint32 i;

  uint32         groupRecordLength=0,totalGroupRecordLength=0;
  mgmdGroupRecord_t* groupPtrAux;

  char                debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]={};

  /* Argument validation */
  if (groupPtr ==PTIN_NULLPTR || buffer == PTIN_NULLPTR || length == PTIN_NULLPTR  || noOfRecords==0)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return FAILURE;
  }

  dataPtr = buffer;

  /* Type = 0x22 */
  byteVal = 0x22;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /* Reserved = 0x00 */
  byteVal = 0x00;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  chksumPtr = dataPtr;
  shortVal = 0;
  PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

  /* Reserved = 0x00 */
  shortVal = 0x0000;
  PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

  /* Number of Records (M)*/
  shortVal=(ushort16) noOfRecords;
  PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Number of Group Records :%u",(ushort16) noOfRecords);
  groupPtrAux=groupPtr;
  for (i=0;i<noOfRecords && groupPtrAux != PTIN_NULLPTR; i++)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group Record (groupAddr: %s  recordType: 0x%x)", ptin_mgmd_inetAddrPrint(&groupPtrAux->key.groupAddr, debug_buf),groupPtrAux->recordType);

    if ( (dataPtr=snoopPTinGroupRecordV3Build(groupPtr->key.serviceId,&groupPtrAux->key.groupAddr,groupPtrAux->recordType,groupPtrAux->numberOfSources,groupPtrAux->firstSource,dataPtr, &groupRecordLength))== PTIN_NULLPTR)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinGroupRecordV3Build()");
      return FAILURE;
    }
    if (groupRecordLength==0)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "groupRecordLength :%u",groupRecordLength);
      return FAILURE;
    }
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "groupRecordLength :%u",groupRecordLength);
    totalGroupRecordLength=totalGroupRecordLength+groupRecordLength;
    groupPtrAux=groupPtrAux->nextGroupRecord;
  }

  if (i!=noOfRecords)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Problems with groupPrt %u<%u",i,noOfRecords);
  }

  /* Update frame length */
  *length = MGMD_IGMPv1v2_HEADER_LENGTH +totalGroupRecordLength;
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "IGMP Frame Size  :%u",*length);

  /* Determine Checksum */
  shortVal = ptinMgmdCheckSum((ushort16 *) buffer, *length, 0);
  PTIN_MGMD_PUT_SHORT(shortVal, chksumPtr);

  return SUCCESS;
}


/*********************************************************************
* @purpose  Create an IGMPv2 Frame for Membership Report messages
* 
* @param    igmpType        IGMPv2 Type
* @param    groupPtr        Group Record Prt
* @param    buffer          Buffer in which the IGMPv2 Frame will be
*                           placed
* @param    length          IGMPv2 Frame length
*
* @returns  SUCCESS
* @returns  FAILURE
*
*
*********************************************************************/
static RC_t ptinIgmpV2FrameBuild(uint8 igmpType,mgmdGroupRecord_t* groupPtr, uchar8 *buffer, uint32 *length)
{
  uchar8         *dataPtr,*chksumPtr, byteVal;
  ushort16       shortVal;
  uint32         ipv4Addr;

  /* Argument validation */
  if (groupPtr ==PTIN_NULLPTR || buffer == PTIN_NULLPTR || length == PTIN_NULLPTR )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return FAILURE;
  }

  dataPtr = buffer;

  /* Type */
  byteVal = igmpType;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /* Max Resp Time = 0x00 */
  byteVal = 0x00;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  chksumPtr = dataPtr;
  shortVal = 0;
  PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

  /* Group Address*/
  ipv4Addr=groupPtr->key.groupAddr.addr.ipv4.s_addr;
  PTIN_MGMD_PUT_DATA(&ipv4Addr, PTIN_IP_ADDR_LEN, dataPtr);
  
  /* Update frame length */
  *length = MGMD_IGMPv1v2_HEADER_LENGTH;
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "IGMP Frame Size  :%u",*length);

  /* Determine Checksum */
  shortVal = ptinMgmdCheckSum((ushort16 *) buffer, *length, 0);
  PTIN_MGMD_PUT_SHORT(shortVal, chksumPtr);

  return SUCCESS;
}

static uchar8* snoopPTinGroupRecordV3Build(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr,uint8 recordType,uint16 numberOfSources,snoopPTinSourceRecord_t* source, uchar8 *buffer, uint32 *length)
{
  uchar8               *dataPtr, byteVal;
  ushort16             shortVal;
  uint32               ipv4Addr;
  snoopPTinSourceRecord_t  *sourcePtr;
  uint32               i;
  char                    debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]   = {};
  ptin_mgmd_externalapi_t externalApi; 
  
  /* Argument validation */
  if (buffer == PTIN_NULLPTR || length == PTIN_NULLPTR || groupAddr==PTIN_NULLPTR || (numberOfSources>0 && source==PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return PTIN_NULLPTR;
  }

  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return PTIN_NULLPTR;
  }

  if (numberOfSources>0 && source==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "He have a null pointer, while having the number of sources different of 0: %u",numberOfSources);
    return PTIN_NULLPTR;
  }

  dataPtr = buffer;

  /* Record Type */
  byteVal = recordType;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /* Aux Data Len */
  byteVal = 0x00;;
  PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

  /*Number of Sources*/
  shortVal = numberOfSources;
  PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

  /*Multicast Address*/
  if (groupAddr->family!=PTIN_MGMD_AF_INET)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid IP Family");
    return PTIN_NULLPTR;
  }

  ipv4Addr=groupAddr->addr.ipv4.s_addr;
  PTIN_MGMD_PUT_DATA(&ipv4Addr, PTIN_IP_ADDR_LEN, dataPtr);

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Number of Sources :%u", numberOfSources);
  sourcePtr=source;  
  for (i=0;i<numberOfSources && sourcePtr != PTIN_NULLPTR;i++)
  {
    /*Source Address*/
    if (groupAddr->family!=PTIN_MGMD_AF_INET)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid IP Family");
      return PTIN_NULLPTR;
    }
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Source (sourceAddr: %s)", ptin_mgmd_inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));

    ptin_mgmd_inetAddressGet(PTIN_MGMD_AF_INET, &sourcePtr->key.sourceAddr,  &ipv4Addr );  
    PTIN_MGMD_PUT_DATA(&ipv4Addr, PTIN_IP_ADDR_LEN, dataPtr);

    sourcePtr=sourcePtr->nextSource;
  }

  if (i!=numberOfSources)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Problems with groupRecord %u<%u",i,numberOfSources);
  }

  /* Update frame length */
  *length = MGMD_IGMPV3_RECORD_GROUP_HEADER_MIN_LENGTH + PTIN_IP_ADDR_LEN * numberOfSources;
//bufferOut=dataPtr;

  PTIN_MGMD_PORT_MASK_t portList;  
  if (externalApi.portList_get(serviceId, PTIN_MGMD_PORT_TYPE_ROOT, &portList)!=SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get ptin_mgmd_port_getList()");
    return PTIN_NULLPTR;
  }
  
  uint32         portId; /* Loop through internal interface numbers */
  /* Increment Counter on all root interfaces in this VLAN with multicast routers attached */
  for (portId = 1; portId <= PTIN_MGMD_MAX_PORT_ID; portId++)
  {
    if (PTIN_MGMD_CLIENT_IS_MASKBITSET(portList.value,portId))
      ptin_mgmd_stat_increment_field(portId, serviceId, (uint32)-1, ptinMgmdRecordType2IGMPStatField(recordType,SNOOP_STAT_FIELD_TX));
  }  
  

  return dataPtr;
}

/*********************************************************************
 * @purpose Create Mac and IP Frames and place the provided IGMP
 *          frame at the end of the new packet
 *
 * @param    serviceId        Service ID
 * @param    pSnoopCB         Snooping Control block
 * @param    groupAddr        IGMP Group address
 * @param    buffer           Buffer in which the entire packet will be
 *                            placed
 * @param    length           Packet length
 * @param    igmpFrameBuffer  IGMPv3 Frame
 * @param    igmpFrameLength  IGMPv3 Frame length
 *
 * @returns  SUCCESS
 * @returns  FAILURE
 *
 *********************************************************************/
static RC_t  snoopPTinPacketBuild(uint32 serviceId, ptinMgmdControlPkt_t* mcastPacket, uchar8* igmpFrameBuffer, uint32 igmpFrameLength,uint32 packetType)
{                                    
  uchar8          *dataPtr,  *ipHdrStartPtr, *chksumPtr;
  static ushort16 iph_ident = 1;
  ushort16        shortVal;
  uint32          ipv4Addr;
  uchar8          byteVal; 

  _UNUSED_(serviceId);

    /* Argument validation */
  if (mcastPacket == PTIN_NULLPTR || igmpFrameBuffer == PTIN_NULLPTR )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return FAILURE;
  }

  dataPtr = mcastPacket->framePayload;

  /* Build IP Header */
  {
    ipHdrStartPtr = dataPtr;

    /* IP Version */
    byteVal = (PTIN_IP_VERSION << 4) | (PTIN_IP_HDR_VER_LEN + (IGMP_IP_ROUTER_ALERT_LENGTH / 4));
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

    /* TOS */
    byteVal = MGMD_IP_TOS;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

    /* Payload Length */
    shortVal = PTIN_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;
    PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

    /* Identified */
    shortVal = iph_ident++;
    PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

    /* Fragment flags */
    shortVal = 0;
    PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

    /* TTL */
    byteVal = MGMD_IP_TTL;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

    /* Protocol */
    byteVal = IGMP_PROT;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

    /* Checksum = 0*/
    chksumPtr = dataPtr;
    shortVal = 0;
    PTIN_MGMD_PUT_SHORT(shortVal, dataPtr);

    /* Source Address - Proxy source address */    
    PTIN_MGMD_PUT_DATA(&mcastPacket->srcAddr.addr.ipv4.s_addr, PTIN_IP_ADDR_LEN, dataPtr);

    /* Destination Address */  
    switch(packetType)
    { 
      case PTIN_IGMP_V2_LEAVE_GROUP:
      {
        ipv4Addr=PTIN_MGMD_IGMP_ALL_ROUTERS_ADDR;
        break;
      }
       case PTIN_IGMP_V2_MEMBERSHIP_REPORT:
      {
        ipv4Addr=mcastPacket->destAddr.addr.ipv4.s_addr;
        break;
      }
      case PTIN_IGMP_MEMBERSHIP_QUERY:
      {
        ipv4Addr=PTIN_MGMD_IGMP_ALL_HOSTS_ADDR;
        break;
      }
      case PTIN_IGMP_V3_MEMBERSHIP_REPORT:
      {
        ipv4Addr=PTIN_MGMD_IGMPV3_REPORT_ADDR;
        break;
      }
      default:
      {
        PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet [%u]: Unknown IGMP type...Packet silently discarded",packetType);
        return FAILURE;
      }
    }    
    PTIN_MGMD_PUT_DATA(&ipv4Addr, PTIN_IP_ADDR_LEN, dataPtr);

    /* IP Options */
    byteVal = IGMP_IP_ROUTER_ALERT_TYPE;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);
    byteVal = IGMP_IP_ROUTER_ALERT_LENGTH;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);
    byteVal = 0;
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);
    PTIN_MGMD_PUT_BYTE(byteVal, dataPtr);

    /* Determine Checksum */
    shortVal = ptinMgmdCheckSum((ushort16 *) ipHdrStartPtr, PTIN_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH, 0);
    PTIN_MGMD_PUT_SHORT(shortVal, chksumPtr);
  }

  /* Update frame length */
//mcastPacket->length = L7_ENET_HDR_SIZE + 4 + L7_ENET_ENCAPS_HDR_SIZE + L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;
  mcastPacket->frameLength =  PTIN_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;

  /* Verify packet size */
  if (mcastPacket->frameLength > PTIN_MGMD_MAX_FRAME_SIZE)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Packet Size Invalid (length : %u > PTIN_MAX_FRAME_SIZE",mcastPacket->frameLength,PTIN_MGMD_MAX_FRAME_SIZE);
    return FAILURE;
  }

  /* Add IGMP Frame to the end of the new MAC+IP Frame */
  PTIN_MGMD_PUT_DATA(igmpFrameBuffer, igmpFrameLength, dataPtr);

  return SUCCESS;
}

#if 0
/*********************************************************************
 * @purpose Send LMQC Group or Group/Source Speficic Queries
 *
 * @param   arg1  Pointer to a snoopPTinQueryData_t structure
 *
 *********************************************************************/
void snoopPTinQuerySend(uint32 arg1)
{
#if 0
  uchar8             igmpFrame[L7_MAX_FRAME_SIZE]={0};
  uint32             igmpFrameLength=0;
  snoopOperData_t       *pSnoopOperEntry;
  L7_RC_t               rc = SUCCESS;
  mgmdSnoopControlPkt_t mcastPacket;
  snoopPTinQueryData_t  *queryData;
  snoop_cb_t            *pSnoopCB;
  ptin_IgmpProxyCfg_t   igmpCfg;

  queryData = (snoopPTinQueryData_t *) arg1;

  /* Validate arguments */
  if (queryData->vlanId < PTIN_VLAN_MIN || queryData->vlanId > PTIN_VLAN_MAX)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting pSnoopCB");
    return;
  }

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != PTIN_MGMD_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Not IPv4 packet");
    return;
  }

  pSnoopOperEntry = snoopOperEntryGet(queryData->vlanId, pSnoopCB, L7_MATCH_EXACT);

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return;
  }

  /* Free timer */
  if (queryData->queryTimer != PTIN_NULLPTR)
  {
    osapiTimerFree(queryData->queryTimer);
  }
  queryData->queryTimer = PTIN_NULLPTR;

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = snoopCBGet(PTIN_MGMD_AF_INET);
  mcastPacket.vlanId = queryData->vlanId;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx = (uint32) -1;
  mcastPacket.msgType = IP_PROT_IGMP;
  mcastPacket.srcAddr.family = PTIN_MGMD_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr = PTIN_NULL_IP_ADDR;
  mcastPacket.destAddr.family = PTIN_MGMD_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = PTIN_NULL_IP_ADDR;

  /* Build header frame for IGMPv3 Query with no sources */
  rc = snoopPTinQueryFrameV3Build(&queryData->groupAddr, queryData->sFlag, igmpFrame, &igmpFrameLength, pSnoopOperEntry, queryData->sourceList, queryData->sourcesCnt);
  if (rc != SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(mcastPacket.vlanId, pSnoopCB, &queryData->groupAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength,L7_IGMP_MEMBERSHIP_QUERY);
  if (rc != SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Send Packet to client interfaces */
  rc = snoopPacketClientIntfsForward(&mcastPacket, L7_IGMP_MEMBERSHIP_QUERY);
  if (rc == SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Packet transmitted to client interfaces. (queue idx %d)", queryData->queuePos);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error transmitting to client interfaces");
  }
  --queryData->retransmissions;

  /* If retransmissions > 0, schedule another Group-Specific Query message. Otherwise free the position in the query buffer */
  if (queryData->retransmissions > 0)
  {
    osapiTimerAdd((void *) snoopPTinQuerySend,
                  (uint32) queryData,
                  0,
                  SNOOP_MAXRESP_INTVL_ROUND(igmpCfg.querier.last_member_query_interval * 1000, SNOOP_IGMP_FP_DIVISOR),
                  &queryData->queryTimer);
  }
  else
  {
    // snoopPTinQueryQueuePop(queryData->queuePos);
  }
#endif
}

#endif

/*********************************************************************
 * @purpose Send LMQC Group or Group/Source Speficic Queries
 *
 * @param   arg1  Pointer to a snoopPTinQueryData_t structure
 *
 *********************************************************************/
RC_t snoopPTinReportSend(uint32 serviceId, mgmdGroupRecord_t *groupPtr, uint32 noOfGroupRecords, ptin_IgmpProxyCfg_t* igmpCfg)
{
  uchar8             igmpFrame[PTIN_MGMD_MAX_FRAME_SIZE]={0};
  uint32             igmpFrameLength=0;
  RC_t               rc = SUCCESS;
  ptinMgmdControlPkt_t mcastPacket;  
  uint8               igmpType;
  mgmdGroupRecord_t   *groupPtrAux=groupPtr;
  uint32              i;
  
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Preparing to send a Membership Report Message with %u group records",noOfGroupRecords);

  /* Validate arguments */
  if (groupPtr==PTIN_NULLPTR || igmpCfg==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return FAILURE;
  }

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mcastPacket));

  /* Get Mgmd Control Block */
  if (( mcastPacket.cbHandle = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }

  if (ptin_mgmd_position_service_identifier_get_or_set(serviceId, &mcastPacket.posId) != SUCCESS 
         || mcastPacket.posId>=PTIN_MGMD_MAX_SERVICES)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid Position Service Identifier [%u]", mcastPacket.posId);    
    return FAILURE;
  }
   
  mcastPacket.serviceId = serviceId;  
  mcastPacket.clientId = (uint32) -1;
  mcastPacket.msgType = PTIN_MGMD_IP_PROT_IGMP;
  mcastPacket.family=PTIN_MGMD_AF_INET;
  mcastPacket.srcAddr.family = PTIN_MGMD_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr=igmpCfg->ipv4_addr;   

  if(mcastPacket.cbHandle->proxyCM[mcastPacket.posId].compatibilityMode==PTIN_MGMD_COMPATIBILITY_V3)
  {  
    mcastPacket.destAddr.family = PTIN_MGMD_AF_INET;
    mcastPacket.destAddr.addr.ipv4.s_addr =groupPtr->key.groupAddr.addr.ipv4.s_addr;
    igmpType=PTIN_IGMP_V3_MEMBERSHIP_REPORT;        

    /* Build header frame for IGMPv3 */       
    if (snoopPTinReportFrameV3Build(noOfGroupRecords, groupPtr, igmpFrame,&igmpFrameLength ) != SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv3)");
      return FAILURE;
    }
     /* Build IP frames and add the IGMP frame to the same packet */  
    if (snoopPTinPacketBuild(serviceId, &mcastPacket, igmpFrame, igmpFrameLength,igmpType) != SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv3)");
      return FAILURE;
    }
    /* Send Packet to root interfaces */   
    rc=ptinMgmdPacketSend(&mcastPacket, igmpType,PTIN_MGMD_PORT_TYPE_ROOT);
  }
  else if (mcastPacket.cbHandle->proxyCM[mcastPacket.posId].compatibilityMode==PTIN_MGMD_COMPATIBILITY_V2)
  {    
    /*Leave Group*/
    if(groupPtr->key.recordType==PTIN_MGMD_CHANGE_TO_INCLUDE_MODE)
    {
      groupPtr->retransmissions=igmpCfg->host.robustness;//Leaves are sent only once
      igmpType=PTIN_IGMP_V2_LEAVE_GROUP;      
    }
    /*Join Group*/
    else 
    {
      if(groupPtr->key.recordType!=PTIN_MGMD_BLOCK_OLD_SOURCES)
      {      
        igmpType=PTIN_IGMP_V2_MEMBERSHIP_REPORT;
      }
      else
      {
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid record type [%u] when operating in IGMPv2 mode, group record silently ignored",groupPtr->key.recordType);
        return SUCCESS;
      }
    }
    for(i=0;i<noOfGroupRecords && groupPtrAux!=PTIN_NULLPTR;i++)
    {
      mcastPacket.destAddr.family = PTIN_MGMD_AF_INET;
      mcastPacket.destAddr.addr.ipv4.s_addr = groupPtrAux->key.groupAddr.addr.ipv4.s_addr;
     
      /* Build header frame for IGMPv2 */       
      if (ptinIgmpV2FrameBuild(igmpType,groupPtrAux,igmpFrame,&igmpFrameLength) != SUCCESS)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv2)");
        return FAILURE;
      }
      /* Build IP frames and add the IGMP frame to the same packet */  
      if (snoopPTinPacketBuild(serviceId, &mcastPacket, igmpFrame, igmpFrameLength,igmpType) != SUCCESS)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv2)");
        return FAILURE;
      }
      /* Send Packet to root interfaces */   
      rc=ptinMgmdPacketSend(&mcastPacket, igmpType,PTIN_MGMD_PORT_TYPE_ROOT);
      if(rc!=SUCCESS) break;
      igmpFrame[PTIN_MGMD_MAX_FRAME_SIZE-1]=0;
      igmpFrameLength=0;
      groupPtrAux=groupPtrAux->nextGroupRecord;
    }
  }
  else
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid compatibility mode [%u] for service [%u], record silently ignored", mcastPacket.cbHandle->proxyCM[mcastPacket.posId].compatibilityMode,serviceId);
    return FAILURE;
  }   
  return rc;
}

/*************************************************************************
 * @purpose Debug method that prints stored information for a specific
 *          multicast group
 *
 * @param   groupAddr   Multicast group address
 * @param   serviceId      Vlan Id
 *
 * @return  none
 *
 *************************************************************************/
void ptinMgmdPositionIdDump(void)
{
  uint32           iterator;
  ptin_mgmd_cb_t  *pMgmdCB;

  if((pMgmdCB=mgmdCBGet(PTIN_MGMD_AF_INET))==PTIN_NULLPTR)
  {   
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get pMgmdCB family:%u", PTIN_MGMD_AF_INET);   
    return;
  }

  for (iterator=0;iterator<PTIN_MGMD_MAX_SERVICES;iterator++)
  {
    printf("Position Service Identifier: %u\n", iterator); 
    printf("Active : %s\n",pMgmdCB->proxyCM[iterator].inUse?"TRUE":"FALSE");
    printf("Service Identifier: %u\n", pMgmdCB->proxyCM[iterator].serviceId);           
  }
}
/*************************************************************************
 * @purpose Debug method that prints stored information for a specific
 *          multicast group
 *
 * @param   groupAddr   Multicast group address
 * @param   serviceId      Vlan Id
 *
 * @return  none
 *
 *************************************************************************/
void ptinMgmdMcastgroupPrint(int32 serviceId,uint32 groupAddrText)
{
  char                     debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {0};
  ptinMgmdGroupInfoData_t *groupEntry;
  ptinMgmdSource_t        *sourcePtr;
  ptin_mgmd_inet_addr_t    groupAddr;
  ptin_mgmd_cb_t          *pMgmdCB; 
  uint32                   posId;

  memset(&groupAddr, 0x00, sizeof(groupAddr));
  groupAddr.family=PTIN_MGMD_AF_INET;

  if((pMgmdCB=mgmdCBGet(groupAddr.family))==PTIN_NULLPTR)
  {   
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get pMgmdCB family:%u", groupAddr.family);   
    return;
  }

  if (ptin_mgmd_position_service_identifier_get_or_set(serviceId, &posId) != SUCCESS 
         || posId>=PTIN_MGMD_MAX_SERVICES)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid Position Identifier [%u]", posId);    
    return;
  }

  groupAddr.addr.ipv4.s_addr=groupAddrText;

  printf("-----------------------------------------\n");
  /* Search for the requested multicast group */
  if (PTIN_NULLPTR != (groupEntry = ptinMgmdL3EntryFind(serviceId, &groupAddr, AVL_EXACT)))
  {
    uint32 portId;

    printf("Group: %s       serviceId: %u positionId: %u\n", ptin_mgmd_inetAddrPrint(&(groupEntry->ptinMgmdGroupInfoDataKey.groupAddr), debug_buf), groupEntry->ptinMgmdGroupInfoDataKey.serviceId,posId);
    printf("-----------------------------------------\n");

    for (portId=0; portId<= PTIN_MGMD_MAX_PORT_ID; ++portId)
    {
      if (groupEntry->ports[portId].active == TRUE)
      {
        uint32 sourceId; 

        printf("Interface: %02u |\n", portId);                
        printf("              |Static:             %s\n", groupEntry->ports[portId].isStatic?"Yes":"No");        
        printf("              |Filter-Mode:        %s\n", groupEntry->ports[portId].filtermode==PTIN_MGMD_FILTERMODE_INCLUDE?"Include":"Exclude");
        if(PTIN_MGMD_ROOT_PORT == portId)
        {
          printf("              |proxyCM:            %u\n", pMgmdCB->proxyCM[posId].compatibilityMode);
          printf("              |proxyCM-Timer:      %u\n", ptin_mgmd_proxycmtimer_timeleft(&pMgmdCB->proxyCM[posId]));
        }
        else
        {
          printf("              |routerCM:           %u\n", groupEntry->ports[portId].groupCMTimer.compatibilityMode);
          printf("              |routerCM-Timer:     %u\n", ptin_mgmd_routercmtimer_timeleft(&groupEntry->ports[portId].groupCMTimer));
        }
        printf("              |Nbr of Sources:     %u\n", groupEntry->ports[portId].numberOfSources);        
        printf("              |Group-Timer:        %u\n", ptin_mgmd_grouptimer_timeleft(&groupEntry->ports[portId].groupTimer));                
        printf("              |Nbr of Clients:     %u\n", groupEntry->ports[portId].numberOfClients);        
        printf("              |Clients: ");
        int8 clientIdx;
        int16 clientBitmapSize;

        if(PTIN_MGMD_ROOT_PORT == portId)
        {
          clientBitmapSize=PTIN_MGMD_ROOT_CLIENT_BITMAP_SIZE;
        }
        else
        {
          clientBitmapSize=PTIN_MGMD_CLIENT_BITMAP_SIZE;
        }
        for (clientIdx=(clientBitmapSize-1); clientIdx>=0; --clientIdx)
        {
          printf("%02X", groupEntry->ports[portId].clients[clientIdx]);
        }                      
        printf("\n");
        for (sourcePtr=groupEntry->ports[portId].firstSource, sourceId = 0; sourcePtr!=PTIN_NULLPTR && sourceId<groupEntry->ports[portId].numberOfSources  ;sourcePtr=sourcePtr->next, ++sourceId)        
        {          
          int8 clientIdx;
          printf("                       |Source: %s\n", ptin_mgmd_inetAddrPrint(&sourcePtr->sourceAddr, debug_buf));
          printf("                                |Static:         %s\n", sourcePtr->isStatic?"Yes":"No");
          printf("                                |status:         %s\n", sourcePtr->status==PTIN_MGMD_SOURCESTATE_ACTIVE?"Active":"ToRemove");            
          printf("                                |Timer Running:  %s\n", ptin_mgmd_sourcetimer_isRunning(&sourcePtr->sourceTimer)?"Yes":"No");
          printf("                                |Source-Timer:   %u\n", ptin_mgmd_sourcetimer_timeleft(&sourcePtr->sourceTimer));
          printf("                                |Nbr of Clients: %u\n", sourcePtr->numberOfClients);            
          printf("                                |Clients: ");            
          for (clientIdx=(clientBitmapSize-1); clientIdx>=0; --clientIdx)
          {
            printf("%02X", sourcePtr->clients[clientIdx]);
          }
          printf("\n");          
        }
      }
    }
  }
  else
  {
    printf("Unknown Group %s VlanId %u\n", ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), serviceId);    
  }
  printf("-----------------------------------------\n");
}


/*************************************************************************
 * @purpose Debug method that prints stored information for a specific
 *          multicast group
 *
 * @param   groupAddr   Multicast group address
 * @param   serviceId      Vlan Id
 *
 * @return  none
 *
 *************************************************************************/
void ptinMgmdGroupRecordPrint(uint32 serviceId,uint32 groupAddrText,uint8 recordType)
{
  char                  debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]={};
  uint32 i;

  ptin_mgmd_inet_addr_t        groupAddr;
  memset(&groupAddr, 0x00, sizeof(groupAddr));
  groupAddr.family=PTIN_MGMD_AF_INET;


  mgmdProxyInterface_t *interfacePtr;
  mgmdGroupRecord_t *groupPtr;    
  snoopPTinSourceRecord_t *sourcePtr;    

  if ((interfacePtr=ptinMgmdProxyInterfaceEntryFind(serviceId, AVL_EXACT)) == PTIN_NULLPTR)
  {
    printf("Interface not initialized for serviceId: %d", serviceId);  
    return;
  }

  groupAddr.addr.ipv4.s_addr=groupAddrText;

  if ((groupPtr=ptinMgmdProxyGroupEntryFind(interfacePtr->key.serviceId, &groupAddr,recordType, AVL_EXACT)) == PTIN_NULLPTR)
  {
    printf("Group Record not initialized: (groupAddr: %s recordType:%u)", ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf), recordType);  
    return;

  }

  printf("|Group Address :%s\n", ptin_mgmd_inetAddrPrint(&groupPtr->key.groupAddr,debug_buf));
  printf("|Record Type:    %u\n",groupPtr->recordType );
  printf("|Retransmissions:    %u\n",groupPtr->retransmissions);
  printf("|Nbr of Sources: %u\n", groupPtr->numberOfSources);  
  sourcePtr=groupPtr->firstSource;  
  for (i=0;i<groupPtr->numberOfSources && sourcePtr!=PTIN_NULLPTR; i++)
  {
    printf("  |Source Address: %s\n", ptin_mgmd_inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));    
    printf("  |Retransmissions:    %u\n\n",sourcePtr->retransmissions);
    sourcePtr=sourcePtr->nextSource;
  }  
}

static mgmdGroupRecord_t* snoopPTinGroupRecordIncrementTransmissions(uint32 noOfRecords,mgmdGroupRecord_t* groupPtr, uint32* newNoOfRecords,uint8 robustnessVariable)
{  
  uint32                    groupRecordId;
  mgmdGroupRecord_t        *newgroupPtr,
                           *groupPtrAux,
                           *groupPtrAux2;  
  mgmdProxyInterface_t     *interfacePtr;  
  PTIN_MGMD_PORT_MASK_t     portList;
  uchar8                    portId;
  ptin_mgmd_externalapi_t   externalApi; 

  /* Argument validation */
  if (groupPtr == PTIN_NULLPTR || newNoOfRecords==PTIN_NULLPTR || noOfRecords==0 || (interfacePtr=groupPtr->interfacePtr)==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return PTIN_NULLPTR;
  }

  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return PTIN_NULLPTR;
  }

  if (externalApi.portList_get(groupPtr->key.serviceId,PTIN_MGMD_PORT_TYPE_ROOT,  &portList)!=SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed ptin_igmp_rootIntfs_getList()");
    return PTIN_NULLPTR;
  }

  for (newgroupPtr=groupPtrAux=groupPtr, groupRecordId=0, *newNoOfRecords=0; groupRecordId<noOfRecords && groupPtrAux!=PTIN_NULLPTR; groupRecordId++, groupPtrAux=groupPtrAux2)
  {
    groupPtrAux2=groupPtrAux->nextGroupRecord;

//This eventually needs to be moved to where the packet is been constructed
    /*Increment IGMPv3 Stats*/
    for (portId = 1; portId <= PTIN_MGMD_MAX_PORT_ID; portId++)
    {
      if (PTIN_MGMD_CLIENT_IS_MASKBITSET(portList.value,portId))
      {
        ptin_mgmd_stat_increment_field(portId, groupPtrAux->key.serviceId, (uint32)-1, ptinMgmdRecordType2IGMPStatField(groupPtrAux->recordType,SNOOP_STAT_FIELD_TX));          
      }
    }
    /*End Stats*/
  
    if (snoopPTinGroupRecordSourceIncrementTransmissions(groupPtrAux,robustnessVariable)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed snoopPTinGroupRecordSourceIncrementTransmissions()");
    //return PTIN_NULLPTR;
    }

    if (++groupPtrAux->retransmissions>=robustnessVariable)
    {
      if (ptinMgmdGroupRecordRemove(interfacePtr,&groupPtrAux->key.groupAddr, groupPtrAux->key.recordType)!=SUCCESS)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");        
      }
      //Once we removed this group record, we need to check if we need also to move the groupRecordPtr
      if(newgroupPtr==groupPtrAux)
      {
        newgroupPtr=groupPtrAux2;
      }
    }
    else
    {      
      (*newNoOfRecords)++;
    }
  }

  return newgroupPtr;
}


static RC_t snoopPTinGroupRecordSourceIncrementTransmissions(mgmdGroupRecord_t* groupPtr,uint8 robustnessVariable)
{
  snoopPTinSourceRecord_t    *sourcePtr,
                             *sourcePtrAux;

  /* Argument validation */
  if (groupPtr == PTIN_NULLPTR || robustnessVariable<PTIN_MIN_ROBUSTNESS_VARIABLE || robustnessVariable>PTIN_MAX_ROBUSTNESS_VARIABLE)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return FAILURE;
  }

  for (sourcePtr=groupPtr->firstSource; sourcePtr!=PTIN_NULLPTR ;sourcePtr=sourcePtrAux)  
  {      
    sourcePtrAux=sourcePtr->next;
    if (++sourcePtr->retransmissions>=robustnessVariable)
    {
      ptinMgmdGroupRecordSourceRemove(groupPtr,&sourcePtr->key.sourceAddr);
    }
  }
  return SUCCESS;
}

/*************************************************************************
 * @purpose Build the Current State Records
 *
 * @param   snoopEntry  AVL tree entry for the requested multicast group
 * @param   intIfNum    Number of the interface through which the report
 *                      arrived
 * @param   noOfSources Number of sources included in the Membership
 *                      Report
 * @param   sourceList  List of the sources included in the Membership
 *                      Report
 *
 * @returns SUCCESS
 * @returns FAILURE
 *
 *************************************************************************/
static mgmdGroupRecord_t* mgmdBuildIgmpv3CSR(mgmdProxyInterface_t* interfacePtr, uint32* noOfRecordsPtr)
{
  ptinMgmdGroupInfoData_t    *groupEntry=PTIN_NULLPTR;
  ptinMgmdGroupInfoDataKey_t  avlTreeKey;
  ptinMgmdSource_t      *sourcePtr;
  BOOL                      newEntry=FALSE;
  char                      debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {};
  mgmdGroupRecord_t        *groupPtr=PTIN_NULLPTR;                           
  BOOL                      firstGroupAdded = FALSE;
  uint32                    sourceId,groupIdx = 0, noOfRecords = 0;
  uint8                     recordType;
  ptin_mgmd_eb_t           *pSnoopEB=PTIN_NULLPTR; 

  /* Argument validation */
  if (interfacePtr ==PTIN_NULLPTR || noOfRecordsPtr == PTIN_NULLPTR )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return PTIN_NULLPTR;
  }

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return PTIN_NULLPTR;
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Building Current State Records of serviceId:%u",interfacePtr->key.serviceId);
/* Run all cells in AVL tree */    

  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( groupEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {

    /* Prepare next key */
    memcpy(&avlTreeKey, &groupEntry->ptinMgmdGroupInfoDataKey, sizeof(ptinMgmdGroupInfoDataKey_t));

    if (groupEntry->ptinMgmdGroupInfoDataKey.serviceId==interfacePtr->key.serviceId && 
        groupEntry->ports[PTIN_MGMD_ROOT_PORT].active==TRUE )        
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group Address Number %u",++groupIdx);
      if (groupEntry->ports[PTIN_MGMD_ROOT_PORT].filtermode==PTIN_MGMD_FILTERMODE_INCLUDE)
      {
        recordType=PTIN_MGMD_MODE_IS_INCLUDE;        
      }
      else
      {
        recordType=PTIN_MGMD_MODE_IS_EXCLUDE;
      }

      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group Address: %s, recordType:%u", ptin_mgmd_inetAddrPrint(&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, debug_buf),recordType);    

      if ((groupPtr=ptinMgmdGroupRecordAdd(interfacePtr,recordType,&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr,&newEntry))== PTIN_NULLPTR)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
        return PTIN_NULLPTR;
      }
      //Verify if this Group Record was created before
      if (newEntry==FALSE)
      {        
        //Remove Group Record and Add it again
        if (ptinMgmdGroupRecordRemove(interfacePtr,&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr, recordType)!=SUCCESS)
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemove()");        
          return PTIN_NULLPTR;
        }
        else
        {
          if ((groupPtr=ptinMgmdGroupRecordAdd(interfacePtr,recordType,&groupEntry->ptinMgmdGroupInfoDataKey.groupAddr,&newEntry))== PTIN_NULLPTR)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
            return PTIN_NULLPTR;
          }
        }
      }
      ++noOfRecords;      

      if (firstGroupAdded==FALSE && groupPtr!=PTIN_NULLPTR)
      {
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "First Group Ptr Added");
        firstGroupAdded=TRUE;
        interfacePtr->firstGroupRecord=groupPtr;                
      }

      uint16 portId=PTIN_MGMD_ROOT_PORT;
      for (sourcePtr=groupEntry->ports[portId].firstSource, sourceId = 0; sourcePtr!=PTIN_NULLPTR && sourceId<groupEntry->ports[portId].numberOfSources  ;sourcePtr=sourcePtr->next, ++sourceId)                 
      {
        if (sourcePtr->status==PTIN_MGMD_SOURCESTATE_ACTIVE &&  
            ptin_mgmd_sourcetimer_isRunning(&sourcePtr->sourceTimer)==TRUE)
        {
          if (FAILURE == ptinMgmdGroupRecordSourcedAdd(groupPtr,&sourcePtr->sourceAddr))
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
            return PTIN_NULLPTR;
          }
        }
      }       
    }
  }  

  *noOfRecordsPtr=noOfRecords;
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Number of Group Records to be sent :%u",*noOfRecordsPtr);
  return (interfacePtr->firstGroupRecord);       
}



/*************************************************************************
 * @purpose Build the Current State Records for IGMPv2
 *
 * @param   serviceId   Service Identifier
 * 
 * @returns SUCCESS
 * @returns FAILURE
 *
 *************************************************************************/
RC_t mgmdBuildIgmpv2CSR(uint32 serviceId,uint32 maxResponseTime)
{
  ptinMgmdGroupInfoData_t   *avlTreeEntry;
  ptinMgmdGroupInfoDataKey_t avlTreeKey;
  BOOL                       newEntry=FALSE;
  char                       debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {};
  mgmdGroupRecord_t         *groupPtr;                             
  uint32                     noOfRecords = 0;  
  mgmdProxyInterface_t*      interfacePtr;
  ptin_mgmd_eb_t            *pSnoopEB; 
  

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return FAILURE;
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Building Current State Records of serviceId:%u",serviceId);

  if ( (interfacePtr=ptinMgmdProxyInterfaceAdd(serviceId)) == PTIN_NULLPTR)
  {    
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceAdd()");
    return FAILURE;
  }

  /* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {

    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));

    if (avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId==interfacePtr->key.serviceId && 
        avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==TRUE)          
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Group Address: %s", ptin_mgmd_inetAddrPrint(&avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr, debug_buf));    
      if ((groupPtr=ptinMgmdGroupRecordAdd(interfacePtr,PTIN_MGMD_MODE_IS_INCLUDE,&avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr,&newEntry))== PTIN_NULLPTR)
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
        return FAILURE;
      }      
      ptin_measurement_timer_start(30,"ptinMgmdScheduleReportMessage");
      if (ptinMgmdScheduleReportMessage(serviceId,&groupPtr->key.groupAddr,PTIN_IGMP_MEMBERSHIP_QUERY,ptinMgmd_generate_random_response_delay(maxResponseTime),FALSE,1, groupPtr)!=SUCCESS)
      {
       ptin_measurement_timer_stop(30);
       PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
       return FAILURE;
      }
      ptin_measurement_timer_stop(30);
      ++noOfRecords;      
    }
  }  
  
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Number of Group Records to be sent :%u",noOfRecords);
  return SUCCESS;       
}

/*************************************************************************
 * @purpose Dump Query AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdDumpGeneralQuery(void)
{
  ptinMgmdQuerierInfoData_t     *avlTreeEntry;  
  ptinMgmdQuerierInfoDataKey_t   avlTreeKey;
  ptin_mgmd_cb_t                *pMgmdCB;

  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdCBGet()");
    return;
  }
  
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdDumpGeneralQuery");
  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdCB->mgmdPTinQuerierAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));

    printf("-----------------------------------------\n");
    printf("General Query ServiceId:%u\n", avlTreeEntry->key.serviceId);      
    printf("      |Timer Running   :%s\n", ptin_mgmd_querytimer_isRunning(&avlTreeEntry->querierTimer)?"Yes":"No");
    printf("      |Query Timer     :%u\n", ptin_mgmd_querytimer_timeleft(&avlTreeEntry->querierTimer));    
    printf("      |Startup Flag    :%s\n",avlTreeEntry->startUpQueryFlag?"True":"False");  
    printf("-----------------------------------------\n");
    
  }  
  ptinMgmdDumpGroupSpecificQuery();  
}


/*************************************************************************
 * @purpose Clean All Query AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdCleanAllGeneralQuery(void)
{
  ptinMgmdQuerierInfoData_t     *avlTreeEntry;  
  ptinMgmdQuerierInfoDataKey_t   avlTreeKey;
  ptin_mgmd_cb_t                *pMgmdCB;

  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdCBGet()");
    return;
  }
  
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdCleanAllGeneralQuery");
  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdCB->mgmdPTinQuerierAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));
    //Stop Query Timer   
    ptin_mgmd_querytimer_stop(&avlTreeEntry->querierTimer);    
    //Remove Query Entry      
    ptinMgmdQueryEntryDelete(avlTreeKey.serviceId,PTIN_MGMD_AF_INET);
  }  
}


/*************************************************************************
 * @purpose Stop All Query AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdGeneralQueryStopAll(void)
{
  ptinMgmdQuerierInfoData_t     *avlTreeEntry;  
  ptinMgmdQuerierInfoDataKey_t   avlTreeKey;
  ptin_mgmd_cb_t                *pMgmdCB;

  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdCBGet()");
    return;
  }
  
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdGeneralQueryStopAll");
  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdCB->mgmdPTinQuerierAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {    
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "TimerPointer:%p",avlTreeEntry->querierTimer.timerHandle);
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));
    //Stop Query Timer   
    ptin_mgmd_querytimer_stop(&avlTreeEntry->querierTimer);        
  }  
}

/*************************************************************************
 * @purpose Start All Query AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdStartAllGeneralQuery(void)
{
  ptinMgmdQuerierInfoData_t     *avlTreeEntry;  
  ptinMgmdQuerierInfoDataKey_t   avlTreeKey;
  ptin_mgmd_cb_t                *pMgmdCB;
  ptin_IgmpProxyCfg_t            igmpGlobalCfg;

  if ((pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdCBGet()");
    return;
  }

  if (ptin_mgmd_igmp_proxy_config_get(&igmpGlobalCfg)!=SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to get IGMP Proxy Configurations"); 
    return;
  }
  
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdStartAllGeneralQuery");
   
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdCB->mgmdPTinQuerierAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));

    avlTreeEntry->startUpQueryFlag=TRUE;
    avlTreeEntry->querierTimer.startUpQueryCount=1;

    //Send Right Away a General Query
    ptinMgmdGeneralQuerySend(avlTreeEntry->key.serviceId,PTIN_MGMD_AF_INET);

    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "TimerPointer:%p",avlTreeEntry->querierTimer.timerHandle);

    if(ptin_mgmd_querytimer_start(&avlTreeEntry->querierTimer, igmpGlobalCfg.querier.startup_query_interval,(void*) avlTreeEntry,PTIN_MGMD_AF_INET)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to start query timer()");
      return;
    }    
  }  
}

/*************************************************************************
 * @purpose Dump Group and Group Source Specific Query AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdDumpGroupSpecificQuery(void)
{
  groupSourceSpecificQueriesAvl_t     *avlTreeEntry;  
  groupSourceSpecificQueriesAvlKey_t   avlTreeKey;
  ptin_mgmd_eb_t                      *pMgmdEB;
  char                                 debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN] = {0};
  groupSourceSpecificQueriesSource_t  *sourcePtr;

  if ((pMgmdEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdCBGet()");
    return;
  }
  
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdDumpGroupSpecificQuery");
  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdEB->groupSourceSpecificQueryAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));

    printf("-----------------------------------------\n");
    if(avlTreeEntry->numberOfSources==0)
    {
       printf("Group Specific Query Query\n");
    }
    else
    {
      printf("Group & Source Specific Query Query\n");
    }    
    printf("ServiceId                  :%u\n", avlTreeEntry->key.serviceId);  
    printf("GroupAddr                  :%s\n", ptin_mgmd_inetAddrPrint(&(avlTreeEntry->key.groupAddr), debug_buf));  
    printf("PortId                     :%u\n", avlTreeEntry->key.portId);  
    printf("SupressRouterSideProcessing:%s\n", avlTreeEntry->supressRouterSideProcessing?"True":"False");
    printf("Retransmissions            :%u\n", avlTreeEntry->retransmissions);
    sourcePtr=avlTreeEntry->firstSource;
    while (sourcePtr!=PTIN_NULLPTR)    
    {
      printf("   SourceAddr:%s",ptin_mgmd_inetAddrPrint(&(sourcePtr->sourceAddr),debug_buf));
      printf("Retransmissions            :%u\n",sourcePtr->retransmissions);
      sourcePtr=sourcePtr->next;
    }
  } 
}

/*************************************************************************
 * @purpose Open Ports for Static Groups 
 *
 *
 *
 *************************************************************************/
void ptinMgmdStaticGroupPortOpen(void)
{
  ptinMgmdGroupInfoData_t     *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t   avlTreeKey;
  ptin_mgmd_eb_t              *pSnoopEB;
  ptin_mgmd_externalapi_t      externalApi;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return;
  }

 /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Going to open ports for static entries");  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    if(avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==TRUE && avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].isStatic==TRUE)
    {
      uint32 portId;
      for(portId=1;portId<=PTIN_MGMD_MAX_PORT_ID;portId++)
      {
        if(avlTreeEntry->ports[portId].active==TRUE && avlTreeEntry->ports[portId].isStatic==TRUE)
        {
          if (avlTreeEntry->ports[portId].numberOfSources==0)
          {
            /*Open L2 Port on Switch*/
            if (externalApi.port_open(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId, portId, avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, PTIN_MGMD_ANY_IPv4_HOST, avlTreeEntry->ports[portId].isStatic) != SUCCESS)
            {
              PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to ptin_mgmd_port_open()");
              return;
            }
          }
          else
          { 
            ptinMgmdSource_t    *sourcePtr;            
            for (sourcePtr=avlTreeEntry->ports[portId].firstSource; sourcePtr!=PTIN_NULLPTR; sourcePtr=sourcePtr->next)
            { 
              if(sourcePtr->status==PTIN_MGMD_SOURCESTATE_ACTIVE && sourcePtr->isStatic==TRUE)
              {
               /*Open L2 Port on Switch*/
               if (externalApi.port_open(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId, portId, avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, sourcePtr->sourceAddr.addr.ipv4.s_addr, sourcePtr->isStatic) != SUCCESS)
               {
                 PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to ptin_mgmd_port_open()");
                 return;
               }
              }              
            }            
          }
        }
      }
    }
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));
  }  
}


/*************************************************************************
 * @purpose Close Ports for Static Groups 
 *
 *
 *
 *************************************************************************/
void ptinMgmdStaticGroupPortClose(void)
{
  ptinMgmdGroupInfoData_t     *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t   avlTreeKey;
  ptin_mgmd_eb_t              *pSnoopEB;
  ptin_mgmd_externalapi_t      externalApi;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return;
  }

 /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Going to close ports for static entries");  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    if(avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==TRUE && avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].isStatic==TRUE)
    {
      uint32 portId;
      for(portId=1;portId<=PTIN_MGMD_MAX_PORT_ID;portId++)
      {
        if(avlTreeEntry->ports[portId].active==TRUE && avlTreeEntry->ports[portId].isStatic==TRUE)
        {
          if (avlTreeEntry->ports[portId].numberOfSources==0)
          {
            /*Open L2 Port on Switch*/
            if (externalApi.port_open(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId, portId, avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, PTIN_MGMD_ANY_IPv4_HOST, avlTreeEntry->ports[portId].isStatic) != SUCCESS)
            {
              PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to ptin_mgmd_port_open()");
              return;
            }
          }
          else
          { 
            ptinMgmdSource_t    *sourcePtr;           
            for (sourcePtr=avlTreeEntry->ports[portId].firstSource; sourcePtr!=PTIN_NULLPTR; sourcePtr=sourcePtr->next)
            { 
              if(sourcePtr->status==PTIN_MGMD_SOURCESTATE_ACTIVE && sourcePtr->isStatic==TRUE)
              {
               /*Open L2 Port on Switch*/
               if (externalApi.port_close(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId, portId, avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr, sourcePtr->sourceAddr.addr.ipv4.s_addr) != SUCCESS)
               {
                 PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to ptin_mgmd_port_open()");
                 return;
               }
              }              
            }            
          }
        }
      }
    }
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));
  }  
}

/*************************************************************************
 * @purpose Dump IGMPv3 AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdDumpL3AvlTree(void)
{
  ptinMgmdGroupInfoData_t     *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t   avlTreeKey;
  ptin_mgmd_eb_t              *pSnoopEB;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

/* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinDumpL3AvlTree");  
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));

    ptinMgmdMcastgroupPrint(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId,avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr.addr.ipv4.s_addr);
  }
  printf("Number of used groups: %u | Number of used sources: %u\n", ptin_mgmd_avlTreeCount(&pSnoopEB->ptinMgmdGroupAvlTree),ptin_fifo_numFreeElements(pSnoopEB->sourcesQueue));  
}

/*************************************************************************
 * @purpose Clean IGMPv3 Group Record AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdGroupRemoveAll(void)
{
  ptin_mgmd_eb_t              *pSnoopEB;
  ptinMgmdGroupInfoData_t     *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t   avlTreeKey;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }
 
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdGroupRemoveAll");
  printf("Number of used sources: %u\n", ptin_fifo_numFreeElements(pSnoopEB->sourcesQueue));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));

    ptinMgmdInterfaceRemove(avlTreeEntry,PTIN_MGMD_ROOT_PORT);      
  }
  ptin_mgmd_avlPurgeAvlTree(&pSnoopEB->ptinMgmdGroupAvlTree,PTIN_MGMD_MAX_GROUPS);
  ptinMgmdDumpL3AvlTree();
}

/*************************************************************************
 * @purpose Clean Static or Dynamic Multicast Group Entries
 *
 * @param   isStatic    : Static: 1 / Dynamic: 0 
 *
 *************************************************************************/
void ptinMgmdStaticOrDynamicGroupRemoveAll(BOOL isStatic)
{
  ptin_mgmd_eb_t              *pSnoopEB;
  ptinMgmdGroupInfoData_t     *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t   avlTreeKey;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }
 
  /* Run all cells in AVL tree */
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "ptinMgmdGroupRemoveAll");
  printf("Number of used sources: %u\n", ptin_fifo_numFreeElements(pSnoopEB->sourcesQueue));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));
    if (avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==TRUE && avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].isStatic==isStatic)
    {
       ptinMgmdInterfaceRemove(avlTreeEntry,PTIN_MGMD_ROOT_PORT); 
    }        
  } 
}

/*************************************************************************
 * @purpose Dump IGMPv3 Group Record AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdDumpGroupRecordAvlTree(void)
{
  mgmdGroupRecord_t     *avlTreeEntry;  
  mgmdGroupRecordKey_t  avlTreeKey;

  ptin_mgmd_eb_t                *pSnoopEB;
  char                           debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]={};

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }


  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinDumpGroupRecordAvlTree");
/* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(& pSnoopEB->snoopPTinProxyGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {

    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));

//  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "serviceId:%u  groupAddr  %s recordType:%u",avlTreeEntry->key.serviceId,inetAddrPrint(&avlTreeEntry->key.groupAddr, debug_buf),avlTreeEntry->key.recordType);
    printf("\nserviceId: %u Group: %s     recordType:%u\n",avlTreeEntry->key.serviceId,ptin_mgmd_inetAddrPrint(&avlTreeEntry->key.groupAddr, debug_buf),avlTreeEntry->key.recordType);

    ptinMgmdGroupRecordPrint(avlTreeEntry->key.serviceId,avlTreeEntry->key.groupAddr.addr.ipv4.s_addr,avlTreeEntry->key.recordType);

  }
  printf("Number of used Group Records: %u \n", ptin_mgmd_avlTreeCount(&pSnoopEB->snoopPTinProxyGroupAvlTree));  
}

/*************************************************************************
 * @purpose Clean All IGMPv3 Group Record AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdGroupRecordRemoveAll(void)
{
  ptin_mgmd_eb_t        *pSnoopEB;

  mgmdGroupRecord_t     *avlTreeEntry;  
  mgmdGroupRecordKey_t  avlTreeKey;  

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

   /* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(& pSnoopEB->snoopPTinProxyGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));
    ptinMgmdGroupRecordRemove(avlTreeEntry->interfacePtr,&avlTreeEntry->key.groupAddr,avlTreeEntry->key.recordType);    
  }  
  ptin_mgmd_avlPurgeAvlTree(&pSnoopEB->snoopPTinProxyGroupAvlTree,PTIN_MGMD_MAX_GROUPS);
  ptinMgmdDumpGroupRecordAvlTree();
}

/*************************************************************************
 * @purpose Clean All IGMPv3 Group Record AVL Tree
 *
 *
 *
 *************************************************************************/
void ptinMgmdGroupSpecificQueriesRemoveAll(void)
{
  ptin_mgmd_eb_t        *pMgmdEB;

  groupSourceSpecificQueriesAvl_t     *avlTreeEntry;  
  groupSourceSpecificQueriesAvlKey_t   avlTreeKey;  

  if ((pMgmdEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to mgmdEBGet()");
    return;
  }

   /* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pMgmdEB->groupSourceSpecificQueryAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));
    ptin_mgmd_groupsourcespecifictimer_remove_entry(avlTreeEntry);    
  }  
  ptin_mgmd_avlPurgeAvlTree(&pMgmdEB->groupSourceSpecificQueryAvlTree,PTIN_MGMD_MAX_GROUPS);
  ptinMgmdDumpGroupSpecificQuery();  
}




/*************************************************************************
 * @purpose Clean IGMPv3 Group Record AVL Tree 
 *
 *
 *
 *************************************************************************/
RC_t ptinMgmdCleanUpGroupRecordAvlTree(uint32 serviceId)
{
  mgmdGroupRecord_t     *avlTreeEntry;  
  mgmdGroupRecordKey_t  avlTreeKey;  

  ptin_mgmd_eb_t                *pSnoopEB;

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return FAILURE;
  }

  /* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(avlTreeKey));
  while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(& pSnoopEB->snoopPTinProxyGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
  {
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(avlTreeKey));
    if(serviceId==avlTreeEntry->key.serviceId)
    {
      ptinMgmdGroupRecordRemove(avlTreeEntry->interfacePtr,&avlTreeEntry->key.groupAddr,avlTreeEntry->key.recordType);
    }
  }
  return SUCCESS;
}



/**********************************************************************
* @purpose Send packet to a specific interfaces
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   control packet
*
* @returns SUCCESS
* @returns FAILURE
*
* @end
*
*********************************************************************/
RC_t ptinMgmdPacketPortSend(ptinMgmdControlPkt_t *mcastPacket, uint8 igmp_type, uint16 portId)
{
  ptin_mgmd_externalapi_t externalApi;
  RC_t                    rc = SUCCESS;
  uint8                   igmp_stat_field;
  BOOL                    clientListGet=FALSE;

  /* Send packet */        
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Packet will be transmited to client_idx=%u in portIdx=%u serviceId=%u family=%u", 
            mcastPacket->clientId, portId, mcastPacket->serviceId,mcastPacket->family);

  if(mcastPacket->cbHandle != PTIN_NULLPTR && mcastPacket->cbHandle->mgmdProxyCfg.admin!=PTIN_MGMD_ENABLE)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"No packet sent! Proxy Admin=%u",mcastPacket->cbHandle->mgmdProxyCfg.admin?"Enable":"Disable");
    return SUCCESS;
  }
  
  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return FAILURE;
  }
  
  if(SUCCESS != (rc = externalApi.tx_packet(mcastPacket->framePayload, mcastPacket->frameLength, mcastPacket->serviceId, portId, mcastPacket->clientId,mcastPacket->family)))
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to transmit packet [client_idx=%u portIdx=%u serviceId=%u family=%u]", 
            mcastPacket->clientId, portId, mcastPacket->serviceId, mcastPacket->family);
    return rc;
  }

  if(ptin_mgmd_packet_trace)
  {    
    uint32 i;
    printf("Tx:PayloadLength:%d\n",mcastPacket->frameLength);
    for (i=0;i<mcastPacket->frameLength;i++)
      printf("%02x ",mcastPacket->framePayload[i]);

    printf("\n");    
  }

   /* Update statistics*/
  switch (igmp_type)
  {
    case PTIN_IGMP_MEMBERSHIP_QUERY:
      igmp_stat_field=SNOOP_STAT_FIELD_GENERAL_QUERY_TX;
      clientListGet=TRUE;      
      break;
    case PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY:
      igmp_stat_field=SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX;
      clientListGet=TRUE;
      break;
    case PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY:
      igmp_stat_field=SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX;
      clientListGet=TRUE;
      break;
    case PTIN_IGMP_V1_MEMBERSHIP_REPORT:
    case PTIN_IGMP_V2_MEMBERSHIP_REPORT:
      igmp_stat_field=SNOOP_STAT_FIELD_JOIN_TX;
      break;
    case PTIN_IGMP_V3_MEMBERSHIP_REPORT:
      igmp_stat_field=SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX;
      break;
    case PTIN_IGMP_V2_LEAVE_GROUP:
      igmp_stat_field=SNOOP_STAT_FIELD_LEAVE_TX;
      break;
    default:    
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unknown IGMP Type:%u",igmp_type);
      return FAILURE;  
  }

  if (clientListGet==TRUE) 
  {
    PTIN_MGMD_CLIENT_MASK_t clientBitmap = {{0}};
    uint32                  noOfClients=0;
    uint32                  noOfClientsFound=0;
    uint32                  clientIdx;

    //Increment client statistics for this port
    if(SUCCESS != (rc = externalApi.clientList_get(mcastPacket->serviceId, portId, &clientBitmap, &noOfClients)))
    {
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to get service clients [serviceId=%u portIdx=%u]", mcastPacket->serviceId, portId);
      return rc;
    }  

    //If any client was found
    if(noOfClients>0)
    {
      for (clientIdx = 0; clientIdx < PTIN_MGMD_MAX_CLIENTS; ++clientIdx)
      {        
        if (PTIN_MGMD_CLIENT_IS_MASKBITSET(clientBitmap.value, clientIdx))
        {          
          ptin_mgmd_stat_increment_clientOnly(portId, clientIdx, igmp_stat_field);

          //Break if all clients were already found
          if(noOfClientsFound++>=noOfClients)
          {
            break;
          }
        }
      }   
    }
  }
  else
  {
    ptin_mgmd_stat_increment_field(portId, mcastPacket->serviceId, mcastPacket->clientId, igmp_stat_field);
  }
    
  return SUCCESS;
}

/**********************************************************************
* @purpose Send packet to all interfaces with multicast hosts/routers attached
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   control packet
*
* @returns SUCCESS
* @returns FAILURE
*
* @notes   This function will send to all interfaces within the specified
*          ServiceId where multicast hosts/routers have detected, except for the
*          interface on which the packet arrived.
*
* @end
*
*********************************************************************/
RC_t ptinMgmdPacketSend(ptinMgmdControlPkt_t *mcastPacket, uint8 igmp_type, uchar8 portType)
{
  uint32                  portId;
  PTIN_MGMD_PORT_MASK_t   portList;
  ptin_mgmd_externalapi_t externalApi;
  BOOL                    packetSent  = FALSE; 

  if(mcastPacket->cbHandle != PTIN_NULLPTR && mcastPacket->cbHandle->mgmdProxyCfg.admin==PTIN_MGMD_ENABLE)
  {

    if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
      return FAILURE;
    }
    
    ptin_measurement_timer_start(32,"externalApi.portList_get");      
     /* Forward frame to all ports in this ServiceId with hosts attached */  
    if (externalApi.portList_get(mcastPacket->serviceId, portType, &portList)!=SUCCESS)
    {
      ptin_measurement_timer_stop(32);
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to get ptin_mgmd_port_getList()");
      return ERROR;
    }
    ptin_measurement_timer_stop(32);

    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Preparing to transmit packet to port type:%u with payload length: %u",portType,mcastPacket->frameLength);
    for (portId = 1; portId <= PTIN_MGMD_MAX_PORT_ID; portId++)
    {
      if (PTIN_MGMD_PORT_IS_MASKBITSET(portList.value,portId))
      {
        /* Send packet */  
        ptin_measurement_timer_start(31,"ptinMgmdPacketPortSend");      
        ptinMgmdPacketPortSend(mcastPacket, igmp_type, portId);
        ptin_measurement_timer_stop(31);
        if(packetSent==FALSE)
          packetSent=TRUE;
      }
    }
    if(packetSent==FALSE)
    {
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"No packet sent! We do not have any active ports configured (serviceId=%u portType=%u client_idx=%u  family=%u)!",mcastPacket->serviceId,portType,mcastPacket->clientId,mcastPacket->family);
    }    
  }
  return SUCCESS;
}

/*********************************************************************
* @purpose  Compute the checksum
*
* @param    addr   @b{(input)}  Pointer to the data on which check sum
*                               needs to be computed
* @param    len    @b{(input)}  Length of the data
* @param    csum   @b{(input)}  Initial checksum value
*
* @returns  Computed check sum

* @notes    none
*
* @end
*********************************************************************/
ushort16 ptinMgmdCheckSum(ushort16 *addr, ushort16 len, ushort16 csum)
{
  register uint32 nleft = len;
  const    ushort16 *w = addr;
  register ushort16 answer;
  register uint32 sum = csum;

  /*  Our algorithm is simple, using a 32 bit accumulator (sum),
   *  we add sequential 16 bit words to it, and at the end, fold
   *  back all the carry bits from the top 16 bits into the lower
   *  16 bits.
   */
  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }


  if (nleft == 1)
  {
    sum += htons(*(uchar8 *)w << 8);
  }

  sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
  sum += (sum >> 16);                     /* add carry */
  answer = ~sum;                          /* truncate to 16 bits */

  /* We need to convert again from network order to host order */
  answer = ntohs(answer);

  return(answer);
}

uint8 ptinMgmdPacketType2IGMPStatField(uint8 packetType,uint8 fieldType)
{
  switch (packetType)
  {
  case PTIN_IGMP_MEMBERSHIP_QUERY: /*To avoid defining a new type, we consider the Memmbership Query Message 0x11 to be equal to a General Query*/
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GENERAL_QUERY_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }
  case PTIN_IGMP_V1_MEMBERSHIP_REPORT:
  case PTIN_IGMP_V2_MEMBERSHIP_REPORT:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_JOIN_TX;   
//  case SNOOP_STAT_FIELD_TOTAL_RX:
//    return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX;
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_JOIN_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_JOIN_INVALID_RX;   
  case SNOOP_STAT_FIELD_DROPPED_RX:
    return SNOOP_STAT_FIELD_IGMP_DROPPED;
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_IGMP_V2_LEAVE_GROUP:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_LEAVE_TX;   
//  case SNOOP_STAT_FIELD_TOTAL_RX:
//    return SNOOP_STAT_FIELD_LEAVES_RECEIVED;
  case SNOOP_STAT_FIELD_VALID_RX:
    return SNOOP_STAT_FIELD_LEAVE_VALID_RX;
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID;
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_IGMP_DROPPED;
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_IGMP_V3_MEMBERSHIP_REPORT:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  default:
    return SNOOP_STAT_FIELD_ALL;
  }
}

RC_t ptinMgmdServiceRemove(uint32 serviceId)
{
  mgmdProxyInterface_t              *proxy_interface;
  char                               debugBuf[PTIN_MGMD_IPV6_DISP_ADDR_LEN]; 
  ptin_mgmd_eb_t                    *pSnoopEB;
  ptin_mgmd_cb_t                    *pSnoopCB;
  ptinMgmdGroupInfoData_t           *avlTreeEntry;  
  ptinMgmdGroupInfoDataKey_t         avlTreeKey;
  groupSourceSpecificQueriesAvl_t   *queriesAvlTreeEntry;
  groupSourceSpecificQueriesAvlKey_t queriesAvlTreeKey;
  uint32                             posId;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Starting to remove service %u", serviceId);

  if ((pSnoopEB = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return FAILURE;
  }

  /* Initialize mcastPacket structure */
  if ((pSnoopCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
    return FAILURE;
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Clearing statistics...", serviceId);
  {
    ptin_mgmd_stats_service_clear(serviceId);
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Clearing learnt channels...", serviceId);
  {
    memset(&avlTreeKey, 0x00, sizeof(avlTreeKey));
    while ( ( avlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->ptinMgmdGroupAvlTree, &avlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
    {
      // Prepare next key
      memcpy(&avlTreeKey, &avlTreeEntry->ptinMgmdGroupInfoDataKey, sizeof(avlTreeKey));

      if(avlTreeEntry->ptinMgmdGroupInfoDataKey.serviceId == serviceId)
      {
        // Triggering the removal of the root interface will remove the entire AVL entry
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP," - Removing %s", ptin_mgmd_inetAddrPrint(&avlTreeEntry->ptinMgmdGroupInfoDataKey.groupAddr, debugBuf));
        ptinMgmdInterfaceRemove(avlTreeEntry, PTIN_MGMD_ROOT_PORT);
      }
    }
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Clearing pending reports...", serviceId);
  {
    proxy_interface = ptinMgmdProxyInterfaceEntryFind(serviceId, AVL_EXACT);
    if(PTIN_NULLPTR != proxy_interface)
    {
      mgmdGroupRecord_t *group_record;

      group_record = proxy_interface->firstGroupRecord;
      while(PTIN_NULLPTR != group_record)
      {
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP," - Removing %s", ptin_mgmd_inetAddrPrint(&group_record->key.groupAddr, debugBuf));
        ptinMgmdGroupRecordRemove(proxy_interface, &group_record->key.groupAddr, group_record->key.recordType);
        group_record = group_record->nextGroupRecord;
      }
    }
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Clearing pending Q(G,S)...", serviceId);
  {
    memset(&queriesAvlTreeKey, 0x00, sizeof(queriesAvlTreeKey));
    while ( ( queriesAvlTreeEntry = ptin_mgmd_avlSearchLVL7(&pSnoopEB->groupSourceSpecificQueryAvlTree, &queriesAvlTreeKey, AVL_NEXT) ) != PTIN_NULLPTR )
    {
      // Prepare next key
      memcpy(&queriesAvlTreeKey, &queriesAvlTreeEntry->key, sizeof(queriesAvlTreeKey));

      if(queriesAvlTreeEntry->key.serviceId == serviceId)
      {
        PTIN_MGMD_TIMER_CB_t controlBlock;

        // Triggering the removal of the root interface will remove the entire AVL entry
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP," - Removing %s", ptin_mgmd_inetAddrPrint(&queriesAvlTreeEntry->key.groupAddr, debugBuf));
        if(SUCCESS != ptinMgmdGroupSourceSpecificQueryAVLTreeEntryDelete(&queriesAvlTreeEntry->key.groupAddr, queriesAvlTreeEntry->key.serviceId, queriesAvlTreeEntry->key.portId))
        {
          return FAILURE;
        }
        ptin_mgmd_groupsourcespecifictimer_CB_get(&controlBlock);
        ptin_mgmd_timer_free(controlBlock, queriesAvlTreeEntry->timerHandle);
        queriesAvlTreeEntry->timerHandle=PTIN_NULLPTR;
      }
    }
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Clearing compatibility-mode settings...", serviceId);
  {
    if( ptin_mgmd_position_service_identifier_get(serviceId, &posId)==SUCCESS && posId<PTIN_MGMD_MAX_SERVICES)
    {
      //Restore compatibility-mode
      ptin_mgmd_proxycmtimer_stop(&pSnoopCB->proxyCM[posId]);
      pSnoopCB->proxyCM[posId].compatibilityMode = PTIN_MGMD_COMPATIBILITY_V3; 
      ptin_mgmd_position_service_identifier_unset(serviceId);
    }
    else
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid Position Identifier [%u]", posId);          
    }    
  }
  return SUCCESS;
}

/**
* @purpose Resetting MGMD to default configurations
*  
* @param  family[in] : Specifies which version to reset [0-ALL; 4-IGMP; 6-MLD]
*
* @return RC_t 
*  
* @note Currently, the family input argument is ignored 
*/
RC_t ptinMgmdResetDefaults(uint8 family)
{
  _UNUSED_(family); //Currently, the family input argument is ignored 
  ptin_mgmd_cb_t *pMgmdCB;
  uint32          i;

  if (( pMgmdCB = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
   PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Error getting pMgmdCB");
   return FAILURE;
  }

  //Remove all multicast groups
  ptinMgmdGroupRemoveAll();

  //Remove all pending group records
  ptinMgmdGroupRecordRemoveAll();

  //Remove all general queriers
  ptinMgmdCleanAllGeneralQuery();

  //Clean all whitelist entries
  ptinMgmdWhitelistClean();

  //Clear all recorded statistics
  ptin_mgmd_statistics_reset_all();

  //Stop all proxy CM timers
  for(i=0; i<PTIN_MGMD_MAX_SERVICES; ++i)
  {
    ptin_mgmd_proxycmtimer_stop(&pMgmdCB->proxyCM[i]); //The stop method also frees the timer
  }

  //Load default configurations
  ptin_mgmd_igmp_proxy_defaultcfg_load();

  return SUCCESS;
}

