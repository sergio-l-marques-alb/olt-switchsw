/*
 * snooping_ptin_util.c
 *
 *  Created on: 2 de Ago de 2012
 *      Author: Daniel Figueira
 */

#include "snooping_ptin_util.h"
#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_ptin_grouptimer.h"
#include "snooping_ptin_sourcetimer.h"
#include "snooping_ptin_proxytimer.h"


#include "snooping_ptin_db.h"

#include "ptin_debug.h"
#include "ptin_igmp.h"

#include "comm_structs.h"
#include "l7_mgmd_api.h"
#include "arpa/inet.h"

#include "snooping.h"//MMELO
#include "avl_api.h"//MMELO

/*********************************************************************
* Static Methods
*********************************************************************/
static L7_RC_t snoopPTinQueryFrameV3Build(L7_inet_addr_t groupAddr, L7_BOOL sFlag, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_inet_addr_t *sources, L7_uint8 sourcesCnt);
static L7_RC_t snoopPTinReportFrameV3Build(L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr, L7_uchar8 *buffer, L7_uint32 *length);
static L7_RC_t snoopPTinGroupRecordV3Build(snoopPTinProxyGroup_t* groupRecord,L7_uchar8 *buffer, L7_uint32 *length);
static L7_RC_t  snoopPTinPacketBuild      (L7_uint32 vlanId, snoop_cb_t* pSnoopCB, L7_inet_addr_t groupAddr, L7_uchar8* buffer, L7_uint32* length, L7_uchar8* igmpFrameBuffer, L7_uint32 igmpFrameLength,L7_uint32 packetType);

static void     snoopPTinQuerySend        (L7_uint32 arg1);
static L7_RC_t snoopPTinReportSend(L7_uint32 vlanId, snoopPTinProxyGroup_t     *groupPtr, L7_uint32 noOfGroupRecords);

static L7_RC_t snoopPTinGroupRecordDecrementRetransmission(L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr, L7_uint32* newNoOfRecords, snoopPTinProxyGroup_t* newgroupPtr);

static L7_RC_t snoopPTinGroupRecordSourceDecrementRetransmission(snoopPTinProxyGroup_t* groupPtr);




/*********************************************************************
 * @purpose Add a new client to the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinL2ClientAdd()
{
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding client to L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Remove a existing client from the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinL2ClientRemove()
{
  LOG_WARNING(LOG_CTX_PTIN_IGMP, "Removing client from L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Add a new client to the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinProxyL2ClientAdd()
{
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding client to L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Remove a existing client from the L2 tables
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinProxyL2ClientRemove()
{
  LOG_WARNING(LOG_CTX_PTIN_IGMP, "Removing client from L2");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Check if a given timer is running
 *
 * @param   timerPtr  Pointer to timer
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 *********************************************************************/
L7_BOOL snoopPTinIsTimerRunning(osapiTimerDescr_t *timerPtr)
{
  if(!timerPtr || timerPtr->timer_running == 0)
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
 * @purpose Method responsible for scheduling Group-Specific or
 *          Group/Source-Specific Queries
 *
 * @param   vlanId      Vlan ID
 * @param   groupAddr   IGMP Group address
 * @param   sFlag       Suppress router-side processing flag
 * @param   sources     Source list
 * @param   sourcesCnt  Number of sources in source list
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @see RFC 3376 6.6.3.1/6.6.3.2
 *
 *********************************************************************/
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_inet_addr_t groupAddr, L7_BOOL sFlag, L7_inet_addr_t *sources, L7_uint8 sourcesCnt)
{
//Commented by MMElo
  //  snoop_eb_t            *pSnoopEB;
//  snoopPTinQueryData_t  queryData;
//  ptin_IgmpProxyCfg_t   igmpCfg;
//  L7_uint32             i;
//
//  /* Argument validation */
//  if (sources == L7_NULLPTR)
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
//    return L7_FAILURE;
//  }
//
//  pSnoopEB = snoopEBGet();
//
//  /* Get proxy configurations */
//  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
//    return L7_FAILURE;
//  }
//
//  /* Set query data and push it to the Query Queue. QueuePos is determined by the push method */
//  queryData.queryTimer = L7_NULLPTR;
//  queryData.queuePos = 0;
//  queryData.vlanId = vlanId;
//  queryData.groupAddr = groupAddr;
//  queryData.sFlag = sFlag;
//  queryData.retransmissions = igmpCfg.querier.last_member_query_count;
//  for(i=0; i< sourcesCnt; ++i)
//  {
//    queryData.sourceList[i] = sources[i];
//  }
//  queryData.sourcesCnt = sourcesCnt;
//
//  if (snoopPTinQueryQueuePush(&queryData) != L7_SUCCESS)
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP, "Query buffer is full");
//    return L7_FAILURE;
//  }
//
//  /* Schedule LMQC Group-Specific Query transmissions */
////snoopPTinQuerySend((L7_uint32) &pSnoopEB->snoopPTinQueryQueue[queryData.queuePos]);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a IGMPv3 Frame for Membership Query messages
*
* @param    groupAddr       IGMP Group address
* @param    sFlag           Suppress router-side processing flag
* @param    buffer          Buffer in which the IGMPv3 Frame will be
*                           placed
* @param    length          IGMPv3 Frame length
* @param    pSnoopOperEntry Snoop Operational data
* @param    sources         Source list
* @param    sourcesCnt      Number of sources in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @todo     Fix Max Resp Code
*
*********************************************************************/
static L7_RC_t snoopPTinQueryFrameV3Build(L7_inet_addr_t groupAddr, L7_BOOL sFlag, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_inet_addr_t *sources, L7_uint8 sourcesCnt)
{
  L7_uchar8         *dataPtr, *chksumPtr, byteVal, i;
  L7_ushort16       shortVal;

  /* Argument validation */
  if (buffer == L7_NULLPTR || length == L7_NULLPTR || pSnoopOperEntry == L7_NULLPTR || sources == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Type = 0x11 */
  byteVal = 0x11;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Max response code */
  byteVal = 0;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  chksumPtr = dataPtr;
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  
  /* Group Address */
  SNOOP_PUT_DATA(&groupAddr.addr.ipv4.s_addr, L7_IP_ADDR_LEN, dataPtr);

  /* Resv | S | QRV */
  if(sFlag)
  {
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV | 0x08;
  }
  else
  {
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV;
  }
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* QQIC */
  byteVal = pSnoopOperEntry->snoopQuerierInfo.qqic;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Number of Sources */
  shortVal = sourcesCnt;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  for(i=0; i<sourcesCnt; ++i)
  {
    /* Source Address */
    SNOOP_PUT_DATA(&sources[i], L7_IP_ADDR_LEN, dataPtr);
  }

  /* Determine Checksum */
  shortVal = snoopCheckSum((L7_ushort16 *) buffer, SNOOP_IGMPV3_HEADER_MIN_LENGTH + (L7_IP_ADDR_LEN * sourcesCnt), 0);
  SNOOP_PUT_SHORT(shortVal, chksumPtr);

  /* Update frame length */
  *length = SNOOP_IGMPV3_HEADER_MIN_LENGTH + (L7_IP_ADDR_LEN * sourcesCnt);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Method responsible for scheduling Group-Specific or
 *          Group/Source-Specific Queries
 *
 * @param   vlanId      Vlan ID
 * @param   groupAddr   IGMP Group address
 * @param   sFlag       Suppress router-side processing flag
 * @param   sources     Source list
 * @param   sourcesCnt  Number of sources in source list
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @see RFC 3376 6.6.3.1/6.6.3.2
 *
 *********************************************************************/
L7_RC_t snoopPTinReportSchedule(L7_uint32 vlanId, L7_inet_addr_t groupAddr, L7_uint8  reportType,L7_uint32 selectedDelay, L7_BOOL isInterface,L7_uint32 noOfRecords, snoopPTinProxyGroup_t * groupPtr )
{
  
  L7_uint32 newNoOfRecords=0;
  snoopPTinProxyGroup_t* newgroupPtr=L7_NULLPTR;

  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;
  
  /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (selectedDelay==0)
  {
    snoopPTinReportSend(vlanId,groupPtr,noOfRecords);

    if(snoopPTinGroupRecordDecrementRetransmission(noOfRecords,groupPtr,&newNoOfRecords, newgroupPtr)!=L7_SUCCESS)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordDecrementRetransmission()");
    }
    if(newgroupPtr!=L7_NULLPTR && newNoOfRecords>0)
    {
      if (isInterface==L7_TRUE) 
      {
        interfacePtr=(snoopPTinProxyInterface_t *) groupPtr->key.memAddr;
        if (snoop_ptin_proxytimer_start(&interfacePtr->timer,SNOOP_PTIN_UNSOLICITED_REPORT_INTERVAL,reportType, isInterface,noOfRecords,groupPtr)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
          return L7_FAILURE;
        }    
      }
      else
      {
        if (snoop_ptin_proxytimer_start(&groupPtr->timer,SNOOP_PTIN_UNSOLICITED_REPORT_INTERVAL,reportType, isInterface,noOfRecords,groupPtr)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
          return L7_FAILURE;
        }    

      }
    }
  }
  else
  {
    if (isInterface==L7_TRUE)
    {
      interfacePtr=(snoopPTinProxyInterface_t *) groupPtr->key.memAddr;
      if (snoop_ptin_proxytimer_start(&interfacePtr->timer,selectedDelay,reportType, isInterface,noOfRecords,groupPtr)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
        return L7_FAILURE;
      }
    }
    else
    {
      if (snoop_ptin_proxytimer_start(&groupPtr->timer,selectedDelay,reportType, isInterface,noOfRecords,groupPtr)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
        return L7_FAILURE;
      }
    }
  }
   
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create an IGMPv3 Frame for Membership Report messages
*
* @param    groupAddr       IGMP Group address
* @param    sFlag           Suppress router-side processing flag
* @param    buffer          Buffer in which the IGMPv3 Frame will be
*                           placed
* @param    length          IGMPv3 Frame length
* @param    pSnoopOperEntry Snoop Operational data
* @param    sources         Source list
* @param    sourcesCnt      Number of sources in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @todo     Fix Max Resp Code
*
*********************************************************************/
L7_RC_t snoopPTinReportFrameV3Build(L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr, L7_uchar8 *buffer, L7_uint32 *length)
{ 
  L7_uchar8         *dataPtr, *chksumPtr, byteVal;
  L7_ushort16       shortVal;
  L7_uint32 i;

//Fixme
//We need to check if the MTU is reached or not!

//L7_uint32 i,j,igmpFrameLength = SNOOP_IGMPv1v2_HEADER_LENGTH/*,ipv4Addr*/;

  /* Argument validation */
  if (groupPtr ==L7_NULLPTR || buffer == L7_NULLPTR || length == L7_NULLPTR  || noOfRecords==0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Type = 0x22 */
  byteVal = 0x22;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

 /* Reserved = 0x00 */
  byteVal = 0x00;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  chksumPtr = dataPtr;
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);
 
//Fixme
/*The number of records needs to be obtained, since we do set a maximum number of sources per record*/
//if (sources2ReportCnt<PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD)
//    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Group Record Full Creating new Group Record");
  /* Number of Records (M)*/
  shortVal=(L7_ushort16 ) noOfRecords;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  for(i=0;i<noOfRecords && groupPtr != L7_NULLPTR; i++)
  {
    if(snoopPTinGroupRecordV3Build(groupPtr,buffer, length)!= L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopPTinGroupRecordV3Build()");
      return L7_FAILURE;
    }
    groupPtr=groupPtr->next;
  }

  /* Update frame length */
  *length = *length + SNOOP_IGMPv1v2_HEADER_LENGTH ;

  /* Determine Checksum */
  shortVal = snoopCheckSum((L7_ushort16 *) buffer, *length, 0);
  SNOOP_PUT_SHORT(shortVal, chksumPtr);

  return L7_SUCCESS;
}


static L7_RC_t snoopPTinGroupRecordV3Build(snoopPTinProxyGroup_t* groupRecord,L7_uchar8 *buffer, L7_uint32 *length)
{

  L7_uchar8         *dataPtr, byteVal;
  L7_ushort16       shortVal;
  L7_uint32         ipv4Addr;
  snoopPTinProxySource_t* sourceRecord;

  /* Argument validation */
  if (buffer == L7_NULLPTR || length == L7_NULLPTR || groupRecord==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

   /* Record Type */
  byteVal = groupRecord->key.recordType;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

   /* Aux Data Len */
  byteVal = 0x00;;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /*Number of Sources*/
  shortVal = groupRecord->numberOfSources;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /*Multicast Address*/
  if (groupRecord->key.groupAddr.family!=L7_AF_INET)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid IP Family");
    return L7_FAILURE;
  }

  ipv4Addr=groupRecord->key.groupAddr.addr.ipv4.s_addr;
  SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

  sourceRecord=groupRecord->source;
  while ( sourceRecord != L7_NULLPTR)
  {
    /*Source Address*/
  if (groupRecord->key.groupAddr.family!=L7_AF_INET)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid IP Family");
    return L7_FAILURE;
  }

  inetAddressGet(L7_AF_INET, &sourceRecord->key.sourceAddr,  &ipv4Addr );  
  SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

  sourceRecord=groupRecord->source->next;
  }

  /* Update frame length */
  *length = *length + SNOOP_IGMPV3_RECORD_GROUP_HEADER_MIN_LENGTH + L7_IP_ADDR_LEN * groupRecord->numberOfSources;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Create Mac and IP Frames and place the provided IGMP
 *          frame at the end of the new packet
 *
 * @param    vlanId           Vlan ID
 * @param    pSnoopCB         Snooping Control block
 * @param    groupAddr        IGMP Group address
 * @param    buffer           Buffer in which the entire packet will be
 *                            placed
 * @param    length           Packet length
 * @param    igmpFrameBuffer  IGMPv3 Frame
 * @param    igmpFrameLength  IGMPv3 Frame length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *********************************************************************/
L7_RC_t snoopPTinPacketBuild(L7_uint32 vlanId, snoop_cb_t* pSnoopCB, L7_inet_addr_t groupAddr, L7_uchar8* buffer, L7_uint32* length, L7_uchar8* igmpFrameBuffer, L7_uint32 igmpFrameLength,L7_uint32 packetType)
{
  L7_uchar8           *dataPtr, *macHdrStartPtr, *ipHdrStartPtr, *chksumPtr, baseMac[L7_MAC_ADDR_LEN], destMac[L7_MAC_ADDR_LEN];
  L7_inet_addr_t      querierAddr, destIp;
  static L7_ushort16  iph_ident = 1;
  L7_ushort16         shortVal;
  L7_uint32      ipv4Addr;
  L7_uchar8           byteVal;

  /* Argument validation */
  if (pSnoopCB == L7_NULLPTR || buffer == L7_NULLPTR || length == L7_NULLPTR || igmpFrameBuffer == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Build MAC Header */
  {
    macHdrStartPtr = dataPtr;

    destIp.family = L7_AF_INET;
    destIp.addr.ipv4.s_addr = (groupAddr.addr.ipv4.s_addr == 0)? L7_IP_ALL_HOSTS_ADDR : groupAddr.addr.ipv4.s_addr;

    /* Validate the group address being reported */
    snoopMulticastMacFromIpAddr(&destIp, destMac);

    /* Validate MAC address */
    if (snoopMacAddrCheck(destMac, L7_AF_INET) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* Get base MAC address (could be BIA or LAA) and use it as src MAC */
    if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    {
      simGetSystemIPBurnedInMac(baseMac);
    }
    else
    {
      simGetSystemIPLocalAdminMac(baseMac);
    }

    /* Set source and dest MAC */
    SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);
    SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);

    /* OuterVlan */
    shortVal = L7_ETYPE_8021Q;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
    shortVal = vlanId;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* IP Ether type */
    shortVal = L7_ETYPE_IP;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
  }

  /* Build IP Header */
  {
    ipHdrStartPtr = dataPtr;

    /* IP Version */
    byteVal = (L7_IP_VERSION << 4) | (L7_IP_HDR_VER_LEN + (IGMP_IP_ROUTER_ALERT_LENGTH / 4));
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* TOS */
    byteVal = SNOOP_IP_TOS;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Payload Length */
    shortVal = L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Identified */
    shortVal = iph_ident++;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Fragment flags */
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* TTL */
    byteVal = SNOOP_IP_TTL;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Protocol */
    byteVal = IGMP_PROT;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Checksum = 0*/
    chksumPtr = dataPtr;
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);

    /* Source Address - Snooping Switches send it with null source address */
    if (snoopQuerierAddressReady(vlanId, pSnoopCB, &querierAddr) == L7_FALSE)
    {
      inetAddressZeroSet(L7_AF_INET, &querierAddr);
    }
    if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress, &ipv4Addr) != L7_SUCCESS)
    {
      ipv4Addr = 0;
    }
    SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

    /* Destination Address */
    if (packetType==SNOOP_PTIN_MEMBERSHIP_REPORT)
    {
      ipv4Addr = L7_IP_IGMPV3_REPORT_ADDR;
    }
    else /*if  (packetType==SNOOP_PTIN_MEMBERSHIP_QUERY)*/
    {
      ipv4Addr = (groupAddr.addr.ipv4.s_addr == 0)? L7_IP_ALL_HOSTS_ADDR : groupAddr.addr.ipv4.s_addr;
    }
    
    SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

    /* IP Options */
    byteVal = IGMP_IP_ROUTER_ALERT_TYPE;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    byteVal = IGMP_IP_ROUTER_ALERT_LENGTH;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    byteVal = 0;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    /* Determine Checksum */
    shortVal = snoopCheckSum((L7_ushort16 *) ipHdrStartPtr, L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, chksumPtr);
  }

  /* Update frame length */
  *length = L7_ENET_HDR_SIZE + 4 + L7_ENET_ENCAPS_HDR_SIZE + L7_IP_HDR_LEN + IGMP_IP_ROUTER_ALERT_LENGTH + igmpFrameLength;

  /* Verify packet size */
  if (*length > L7_MAX_FRAME_SIZE)
  {
    return L7_FAILURE;
  }

  /* Add IGMP Frame to the end of the new MAC+IP Frame */
  SNOOP_PUT_DATA(igmpFrameBuffer, igmpFrameLength, dataPtr);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Send LMQC Group or Group/Source Speficic Queries
 *
 * @param   arg1  Pointer to a snoopPTinQueryData_t structure
 *
 *********************************************************************/
void snoopPTinQuerySend(L7_uint32 arg1)
{
  L7_uchar8             igmpFrame[L7_MAX_FRAME_SIZE]={0};
  L7_uint32             igmpFrameLength=0;
  snoopOperData_t       *pSnoopOperEntry;
  L7_RC_t               rc = L7_SUCCESS;
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
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting pSnoopCB");
    return;
  }

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != L7_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Not IPv4 packet");
    return;
  }

  pSnoopOperEntry = snoopOperEntryGet(queryData->vlanId, pSnoopCB, L7_MATCH_EXACT);

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return;
  }

  /* Free timer */
  if(queryData->queryTimer != L7_NULLPTR)
  {
    osapiTimerFree(queryData->queryTimer);
  }
  queryData->queryTimer = L7_NULLPTR;

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId = queryData->vlanId;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx = (L7_uint32) -1;
  mcastPacket.msgType = IP_PROT_IGMP;
  mcastPacket.srcAddr.family = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  /* Build header frame for IGMPv3 Query with no sources */
  rc = snoopPTinQueryFrameV3Build(queryData->groupAddr, queryData->sFlag, igmpFrame, &igmpFrameLength, pSnoopOperEntry, queryData->sourceList, queryData->sourcesCnt);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(mcastPacket.vlanId, pSnoopCB, queryData->groupAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength,SNOOP_PTIN_MEMBERSHIP_QUERY);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Send Packet to client interfaces */
  rc = snoopPacketClientIntfsForward(&mcastPacket, L7_IGMP_MEMBERSHIP_QUERY);
  if (rc == L7_SUCCESS)
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
                  (L7_uint32) queryData,
                  0,
                  SNOOP_MAXRESP_INTVL_ROUND(igmpCfg.querier.last_member_query_interval * 1000, SNOOP_IGMP_FP_DIVISOR),
                  &queryData->queryTimer);
  }
  else
  {
   // snoopPTinQueryQueuePop(queryData->queuePos);
  }
}



/*********************************************************************
 * @purpose Send LMQC Group or Group/Source Speficic Queries
 *
 * @param   arg1  Pointer to a snoopPTinQueryData_t structure
 *
 *********************************************************************/
L7_RC_t snoopPTinReportSend(L7_uint32 vlanId, snoopPTinProxyGroup_t     *groupPtr, L7_uint32 noOfGroupRecords)
{
  L7_uchar8             igmpFrame[L7_MAX_FRAME_SIZE]={0};
  L7_uint32             igmpFrameLength=0;
  snoopOperData_t       *pSnoopOperEntry;
  L7_RC_t               rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t mcastPacket;  
  snoop_cb_t            *pSnoopCB;
  ptin_IgmpProxyCfg_t   igmpCfg;
  

  /* Validate arguments */
  if (groupPtr==L7_NULLPTR )
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting pSnoopCB");
    return L7_FAILURE;
  }

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != L7_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Not IPv4 packet");
    return L7_FAILURE;
  }

  pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT);

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId = vlanId;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx = (L7_uint32) -1;
  mcastPacket.msgType = IP_PROT_IGMP;
  mcastPacket.srcAddr.family = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  /* Build header frame for IGMPv3 Query with no sources */   
  rc = snoopPTinReportFrameV3Build(noOfGroupRecords, groupPtr, igmpFrame,&igmpFrameLength );

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return L7_FAILURE;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(vlanId, pSnoopCB, groupPtr->key.groupAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength,SNOOP_PTIN_MEMBERSHIP_REPORT);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return L7_FAILURE;
  }

  /* Send Packet to root interfaces */
  rc = snoopPacketRtrIntfsForward(&mcastPacket, L7_IGMP_V3_MEMBERSHIP_REPORT);
  if (rc == L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Packet transmitted to root interfaces.");
    rc = snoopPacketRtrIntfsForward(&mcastPacket, L7_IGMP_V3_MEMBERSHIP_REPORT);
    if (rc == L7_SUCCESS)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Packet transmitted to root interfaces.");
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Error transmitting to root interfaces");
      return L7_FAILURE;
    }
  }
  
  return rc;
}

/*************************************************************************
 * @purpose Convert an IP address in integer format to string
 *
 * @param   ip      IP address to convert
 * @param   buffer  Buffer in which the converted string will be placed.
 *                  Must have at least 15 bytes.
 *
 * @return  String with IP address
 *
 *************************************************************************/
char* snoopPTinIPv4AddrPrint(L7_uint32 ip, char* buffer)
{
  L7_uchar8 bytes[4];

  if (buffer == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  sprintf(buffer, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);

  return buffer;
}

/*********************************************************************
*
* @purpose Given an address and a buffer, fill in the buffer with a
*          displayable rendition of the address, appropriate to the
*          address family.
*
* @param   L7_uchar8       *buf   (output buffer)
* @param   L7_uint32        len   (length of buffer)
* @param   L7_inet_addr_t  *addr  (address)
*
* @returns none
*
* @notes   Fills in address; returns '<INV_FAM_xx>' where xx is the
*          hexadecimal value of the family if it's not IPv4 or IPv6.
*          Assumes valid buf, len, addr are passed in.
*
* @end
*
*********************************************************************/
char *snoopPTinIPAddrPrint(const L7_inet_addr_t addr, L7_uchar8 *buf)
{
const L7_uchar8 *rp=L7_NULLPTR;
if (L7_AF_INET == addr.family)
 {
   /* It's IPv4; parse as IPv4 */
   inet_ntop(L7_AF_INET, &addr.addr.ipv4, buf, sizeof(buf));
   return (L7_uchar8 *)rp;
 }
 else if (L7_AF_INET6 == addr.family)
 {
   /* It's IPv6; parse as IPv6 */
   inet_ntop(L7_AF_INET6, &addr.addr.ipv6, buf, sizeof(buf));
   return (L7_uchar8 *)rp;
 }
 else
 {
   /* It's not covered already; indicate invalid */
   LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid IP Address Family :%d",addr.family);
   return (L7_uchar8 *) L7_NULL;
 }
}

/*************************************************************************
 * @purpose Debug method that prints stored information for a specific
 *          multicast group
 *
 * @param   groupAddr   Multicast group address
 * @param   vlanId      Vlan Id
 *
 * @return  none
 *
 *************************************************************************/
void snoopPTinMcastgroupPrint(L7_INTF_MASK_t rootIntfList, L7_uint32 vlanId,L7_inet_addr_t groupAddr)
{
  char                  debug_buf[IPV6_DISP_ADDR_LEN];
  snoopPTinL3InfoData_t *snoopEntry;
//L7_inet_addr_t        groupAddr;
      
  //char                 groupAddrStr[INET_ADDRSTRLEN];
  //char                *groupAddrPtr=L7_NULLPTR;


  if ( vlanId<0 || vlanId>4095 /*|| addrStr==L7_NULLPTR*/)
  {
    printf("Invalid Arguments: %d", vlanId);  
    return;
  }

//  groupAddrPtr=addrStr;
//  L7_uint16 i;
//  for(i=0; i< INET_ADDRSTRLEN; i++)
//  {
////    printf("Char %s",groupAddrPtr );
////    if (groupAddrPtr =='\0')
////    {
//////      groupAddrStr[i]='\0';
////      break;
////    }
////    else
////    {
//////    groupAddrStr[i]=*groupAddrPtr;
////      groupAddrPtr++;
////    }
//  }
  
//  groupAddrPtr=addrStr;  
  //int atoi ( const char * str );
   

//char  *ptr=groupAddrStr;
//ptr=ptr+IPV6_DISP_ADDR_LEN+1;
//*ptr='\0';

//groupAddr.addr.ipv4.s_addr= (L7_uint32) inet_addr(groupAddrStr);
//
//   groupAddr.addr.ipv4.s_addr= (L7_uint32) inet_addr("234.0.0.1");
//
////  groupAddr.addr.ipv4.s_addr=groupAddrStr;
//  groupAddr.family=L7_AF_INET;


  if (inetIsInMulticast(&groupAddr)!=L7_TRUE)        
  {
    printf("Invalid Multicast IP Address : %s", inetAddrPrint(&groupAddr,debug_buf));  
    return;
  }

  /* Search for the requested multicast group */
  if (L7_NULLPTR != (snoopEntry = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT)))
  {
    L7_uint32 ifIdx;

    printf("Group: %s       Vlan ID: %u\n", inetAddrPrint(&(snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf), snoopEntry->snoopPTinL3InfoDataKey.vlanId);
    printf("-----------------------------------------\n");

    for (ifIdx=0; ifIdx<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; ++ifIdx)
    {
      if (snoopEntry->interfaces[ifIdx].active == L7_TRUE)
      {
        L7_uint32 sourceIdx; 

        printf("Interface: %02u |\n", ifIdx);
        printf("              |Filter-Mode:    %s\n", snoopEntry->interfaces[ifIdx].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE?"Include":"Exclude");
        printf("              |Nbr of Sources: %u\n", snoopEntry->interfaces[ifIdx].numberOfSources);
        printf("              |Group-Timer:    %u\n", snoop_ptin_grouptimer_timeleft(&snoopEntry->interfaces[ifIdx].groupTimer));
        for (sourceIdx=0; sourceIdx<PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++sourceIdx)
        {
          if (snoopEntry->interfaces[ifIdx].sources[sourceIdx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE)
          {
            L7_int8 clientIdx;

            printf("                       |Source: %s\n", inetAddrPrint(&(snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceAddr), debug_buf));
            printf("                                |status:         %s\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].status==PTIN_SNOOP_SOURCESTATE_ACTIVE?"Active":"ToRemove");
            printf("                                |Source-Timer:   %u\n", snoop_ptin_sourcetimer_timeleft(&snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceTimer));
            printf("                                |Nbr of Clients: %u\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].numberOfClients);
            printf("                                |Clients: ");
            for (clientIdx=(PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1); clientIdx>=0; --clientIdx)
            {
              printf("%08X", snoopEntry->interfaces[ifIdx].sources[sourceIdx].clients[clientIdx]);
            }
            printf("\n");
          }
        }
      }
    }
  }
  else
  {
    printf("Unknown Group %s VlanId %u\n", inetAddrPrint(&groupAddr, debug_buf), vlanId);
  }
}



static L7_RC_t snoopPTinGroupRecordDecrementRetransmission(L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr, L7_uint32 *newNoOfRecords, snoopPTinProxyGroup_t* newgroupPtr)
{  
  L7_RC_t rc=L7_SUCCESS;
  L7_uint32 i;
  snoopPTinProxyGroup_t* groupPtrTmp;
  char                debug_buf[IPV6_DISP_ADDR_LEN];


/*Initialize Output Variables*/
  *newNoOfRecords=noOfRecords;
  newgroupPtr=L7_NULLPTR;

   /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
  groupPtrTmp=groupPtr;
  for (i=0;i<noOfRecords&&groupPtrTmp!=L7_NULLPTR;i++)
  {
    rc=snoopPTinGroupRecordSourceDecrementRetransmission(groupPtrTmp);
    if (groupPtrTmp->retransmissions>1)
    {    
      --groupPtrTmp->retransmissions;
    }
    else    
    {
      if (groupPtrTmp==groupPtr && groupPtr->nextGroupRecord!=L7_NULLPTR)
        newgroupPtr=groupPtr->nextGroupRecord;
      newNoOfRecords--;

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing  Group Record:  GroupAdd=%s, RecordType=%u", inetAddrPrint(&groupPtrTmp->key.groupAddr, debug_buf),groupPtrTmp->key.recordType);     
      if((rc=snoopPTinGroupRecordDelete((snoopPTinProxyInterface_t*) groupPtrTmp->key.memAddr,&groupPtrTmp->key.groupAddr , groupPtrTmp->key.recordType))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");        
      }
    }
    groupPtrTmp=groupPtrTmp->nextGroupRecord;
  }
  return rc;
}


static L7_RC_t snoopPTinGroupRecordSourceDecrementRetransmission(snoopPTinProxyGroup_t* groupPtr)
{  
  L7_RC_t rc=L7_SUCCESS;
  L7_uint32 i;
  snoopPTinProxySource_t* sourcePtrTmp;
  char                debug_buf[IPV6_DISP_ADDR_LEN];

   /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
  sourcePtrTmp=groupPtr->source;
  for(i=0;i<groupPtr->numberOfSources && sourcePtrTmp!=L7_NULLPTR;i++)
  {
    if (sourcePtrTmp->retransmissions>1)
      --sourcePtrTmp->retransmissions;
    else    
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing Source from Group Record  %s", inetAddrPrint(&sourcePtrTmp->key.sourceAddr, debug_buf));     
      if((rc=snoopPTinGroupRecordSourceDelete(groupPtr,&sourcePtrTmp->key.sourceAddr))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");        
      }
    }
    sourcePtrTmp=sourcePtrTmp->nextSource;
  }
  return rc;
}
