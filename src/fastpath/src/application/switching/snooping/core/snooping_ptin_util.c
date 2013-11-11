/**
 * snooping_ptin_util.c
 *
 *  Created on: 2 de Ago de 2012
 *      Author: Daniel Figueira
 *  Modified on 2013/04/14
 *      Author: Márcio Melo (marcio-d-melo@ptinovacao.pt) 
 * Notes:
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

#include <ctype.h>

/*********************************************************************
* Static Methods
*********************************************************************/
static L7_RC_t snoopPTinQueryFrameV3Build(L7_inet_addr_t* groupAddr, L7_BOOL sFlag, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_inet_addr_t *sources, L7_uint8 sourcesCnt);
static L7_RC_t snoopPTinReportFrameV3Build(L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupPtr, L7_uchar8 *buffer, L7_uint32 *length);
static L7_uchar8* snoopPTinGroupRecordV3Build(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType,L7_uint16 numberOfSources,snoopPTinProxySource_t* source, L7_uchar8 *buffer, L7_uint32 *length);
static L7_RC_t  snoopPTinPacketBuild      (L7_uint32 vlanId, snoop_cb_t* pSnoopCB, L7_inet_addr_t* destIp, L7_uchar8* buffer, L7_uint32* length, L7_uchar8* igmpFrameBuffer, L7_uint32 igmpFrameLength,L7_uint32 packetType);

static void     snoopPTinQuerySend        (L7_uint32 arg1);
static L7_RC_t snoopPTinReportSend(L7_uint32 vlanId, snoopPTinProxyGroup_t     *groupPtr, L7_uint32 noOfGroupRecords, ptin_IgmpProxyCfg_t* igmpCfg);

static snoopPTinProxyGroup_t* snoopPTinGroupRecordIncrementTransmissions(L7_uint32 noOfRecords,snoopPTinProxyGroup_t* groupPtr, L7_uint32* newNoOfRecords);

static L7_RC_t snoopPTinGroupRecordSourceIncrementTransmissions(snoopPTinProxyGroup_t* groupPtr);

static snoopPTinProxyGroup_t* snoopPTinBuildCSR(snoopPTinProxyInterface_t *interfacePtr, L7_uint32 *noOfRecords,L7_uint8 robustnessVariable);

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
  if (!timerPtr || timerPtr->timer_running == 0)
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
L7_RC_t snoopPTinQuerySchedule(L7_uint16 vlanId, L7_inet_addr_t* groupAddr, L7_BOOL sFlag, L7_inet_addr_t *sources, L7_uint8 sourcesCnt)
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
static L7_RC_t snoopPTinQueryFrameV3Build(L7_inet_addr_t* groupAddr, L7_BOOL sFlag, L7_uchar8 *buffer, L7_uint32 *length, snoopOperData_t *pSnoopOperEntry, L7_inet_addr_t *sources, L7_uint8 sourcesCnt)
{
  L7_uchar8         *dataPtr, *chksumPtr, byteVal, i;
  L7_ushort16       shortVal;

  /* Argument validation */
  if (groupAddr==L7_NULLPTR || buffer == L7_NULLPTR || length == L7_NULLPTR || pSnoopOperEntry == L7_NULLPTR || sources == L7_NULLPTR)
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
  SNOOP_PUT_DATA(&groupAddr->addr.ipv4.s_addr, L7_IP_ADDR_LEN, dataPtr);

  /* Resv | S | QRV */
  if (sFlag)
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

  for (i=0; i<sourcesCnt; ++i)
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
L7_RC_t snoopPTinScheduleReportMessage(L7_uint32 vlanId, L7_inet_addr_t* groupAddr, L7_uint8  reportType,L7_uint32 timeOut, L7_BOOL isInterface,L7_uint32 noOfRecords, void* ptr, L7_uint8 robustnessVariable)
{
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint16           foundIdx=PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP,sourceIdx;
  L7_uint32           newNoOfRecords=0,noOfRecordsAux=noOfRecords;
  L7_int64  noOfPendingRecords;

  snoopPTinProxySource_t* sourcePtr;
  snoopPTinProxyGroup_t* groupPtr=L7_NULLPTR,*newgroupPtr=L7_NULLPTR;
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;

//void* newPtr=L7_NULLPTR;

  snoopPTinProxyTimer_t *proxyTimer;
  snoopPTinL3InfoData_t*avlTreeEntry;
  L7_RC_t               rc;


  /* Argument validation */
  if (groupAddr==L7_NULLPTR || ptr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "timeOut:%u; reportType:%u; isInterface:%u;  noOfRecords:%u; robustnessVariable:%u",timeOut,reportType,isInterface,noOfRecords,robustnessVariable);

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations, going to use default values!");
    igmpCfg.host.unsolicited_report_interval=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;    
    igmpCfg.host.max_records_per_report=PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT;
    igmpCfg.ipv4_addr.s_addr=L7_NULL_IP_ADDR;
    igmpCfg.igmp_cos=PTIN_IGMP_DEFAULT_COS;
  }
  else
  {
    if (igmpCfg.host.unsolicited_report_interval==0)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Using default value for Unsolicited Report Interval, %u",PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL);  
      igmpCfg.host.unsolicited_report_interval=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;    
    }
    if (igmpCfg.host.max_records_per_report==0)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Using default value for Max Records per Report, %u",PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT);  
      igmpCfg.host.max_records_per_report=PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT;
    }
    if (igmpCfg.igmp_cos==0x0)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Using default value for IGMPv3 CoS, %u",PTIN_IGMP_DEFAULT_COS);  
      igmpCfg.igmp_cos=PTIN_IGMP_DEFAULT_COS;
    }
  }

  if (isInterface==L7_TRUE)
  {
    interfacePtr=ptr;
    proxyTimer=&interfacePtr->timer;
  }
  else
  {
    groupPtr=ptr;
    proxyTimer=&groupPtr->timer;
  }

  if (timeOut==0)/*We need to send right away this Membership Report Message*/
  {
    if (proxyTimer->isFirstTransmission==L7_TRUE)
    {
      if (isInterface==L7_TRUE)
      {
        if ((groupPtr=snoopPTinBuildCSR(interfacePtr,&noOfRecordsAux,robustnessVariable))==L7_NULLPTR)
        {
          if (noOfRecordsAux>0)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinBuildCSR()");
            return L7_ERROR;
          }
          else
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "Membership Response to General Query silenty discarded, once we do not have any active groups");
            return L7_SUCCESS;
          }
        }
      }
      else
      {
        /*Let us verify if this is a response to Group Specific & Group & Source Specific Query*/
        if (reportType==L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY || reportType==L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY)
        {
          /*Let us verify if this group still has any clients*/
          if ((avlTreeEntry=snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT))==L7_NULLPTR || 
              avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_FALSE || 
              snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients)==L7_SUCCESS)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "Membership Response to Group Query silenty discarded, once this group is no longer active");
            return L7_SUCCESS;          
          }
          if (reportType==L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY)
          {
            if (groupPtr->numberOfSources==0)
            {
              LOG_ERR(LOG_CTX_PTIN_IGMP, "Report Type=SNOOP_PTIN_GROUP_AND_SOURCE_SPECIFIC_QUERY, while numberOfSources=0");
              return L7_ERROR;
            }
            sourcePtr=groupPtr->source;
            /*Let us verify if this source still has any clients*/
            for (sourceIdx=0;sourceIdx<groupPtr->numberOfSources && sourcePtr!=L7_NULLPTR;sourceIdx++)
            {
              /* Search for this source in the current source list */  
              if ((rc=snoopPTinSourceFind(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources, &sourcePtr->key.sourceAddr, &foundIdx))==L7_FAILURE)
              {
                LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinSourceFind()");
                return L7_FAILURE;
              }
              else if (rc==L7_SUCCESS && snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[foundIdx].clients)==L7_SUCCESS)
              {
                if (snoopPTinGroupRecordSourceRemove(groupPtr,&sourcePtr->key.sourceAddr)!=L7_SUCCESS)
                {
                  LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinGroupRecordSourceRemove()");
                  return L7_ERROR;
                }
                LOG_TRACE(LOG_CTX_PTIN_IGMP, "Membership Response to Group & Source Specific Query, source address silently discarded, once this source is no longer active");
              }
              sourcePtr++;
            }
          }
        }
      }
      proxyTimer->isFirstTransmission=L7_FALSE;
    }

    /*We need to split the report if the number of records is higher then igmpCfg.host.max_records_per_report*/
    newgroupPtr=groupPtr;
    for (noOfPendingRecords=noOfRecordsAux;noOfPendingRecords>0 && newgroupPtr!=L7_NULLPTR;noOfPendingRecords=noOfPendingRecords-igmpCfg.host.max_records_per_report)
    {
      if (noOfPendingRecords>igmpCfg.host.max_records_per_report)
      {
        if (snoopPTinReportSend(vlanId,newgroupPtr,igmpCfg.host.max_records_per_report,&igmpCfg)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinReportSend()");
          return L7_FAILURE;
        }
#if 0 
//      newgroupPtr=newgroupPtr+igmpCfg.host.max_records_per_report;       
#else //Since the group records are not sequencialy ordered on the memory, we cannot perform any arithmetic operation with pointers. Therefore we need to move the pointer record by record.
        L7_uint32 numberOfRecords;
        for (numberOfRecords=0;numberOfRecords<igmpCfg.host.max_records_per_report && newgroupPtr!=L7_NULLPTR ;numberOfRecords++)
        {
          newgroupPtr=newgroupPtr->nextGroupRecord;
        }
#endif
      }
      else
      {
        if (snoopPTinReportSend(vlanId,newgroupPtr,noOfPendingRecords,&igmpCfg)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinReportSend()");
          return L7_FAILURE;
        }
      }
    }    
    if ((newgroupPtr=snoopPTinGroupRecordIncrementTransmissions(noOfRecordsAux,groupPtr,&newNoOfRecords))==L7_NULLPTR && newNoOfRecords>0)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordIncrementTransmissions()");
      return L7_FAILURE;
    }

    if (newNoOfRecords>0)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Re-Scheduling Membership Report Message with timeout: %u ",igmpCfg.host.unsolicited_report_interval);  
      if (snoop_ptin_proxytimer_start(proxyTimer,igmpCfg.host.unsolicited_report_interval,reportType, isInterface,newNoOfRecords,newgroupPtr,robustnessVariable)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
        return L7_FAILURE;
      }
    }
  }
  else/*Schedule a Membership Report Message*/
  {
    if (snoop_ptin_proxytimer_start(proxyTimer,timeOut,reportType, isInterface,noOfRecords,ptr,robustnessVariable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_start()");
      return L7_FAILURE;
    }
    proxyTimer->isFirstTransmission=L7_TRUE;
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
  L7_uchar8         *dataPtr,*noRecordsPtr,*chksumPtr, byteVal;
  L7_ushort16       shortVal;
  L7_uint32 i;

  L7_uint32         groupRecordLength=0,totalGroupRecordLength=0;
  snoopPTinProxyGroup_t* groupPtrAux;

  char                debug_buf[IPV6_DISP_ADDR_LEN]={};

//Fixme
//We need to check if the MTU is reached or not!

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

  /* Reserved = 0x0000 */
  shortVal = 0x0000;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Number of Records (M)*/
  noRecordsPtr=dataPtr;//We save this ptr for future use if required
  shortVal=(L7_ushort16) noOfRecords;
  SNOOP_PUT_SHORT(shortVal, dataPtr);


  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Number of Group Records :%u",(L7_ushort16) noOfRecords);
  groupPtrAux=groupPtr;
  for (i=0;i<noOfRecords && groupPtrAux != L7_NULLPTR; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record (groupAddr: %s  recordType: %u)", inetAddrPrint(&groupPtrAux->key.groupAddr, debug_buf),groupPtrAux->recordType);

    if ( (dataPtr=snoopPTinGroupRecordV3Build(groupPtr->key.vlanId,&groupPtrAux->key.groupAddr,groupPtrAux->recordType,groupPtrAux->numberOfSources,groupPtrAux->source,dataPtr, &groupRecordLength))== L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopPTinGroupRecordV3Build()");
      return L7_FAILURE;
    }
    if (groupRecordLength==0)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "groupRecordLength :%u",groupRecordLength);
      return L7_FAILURE;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "groupRecordLength :%u",groupRecordLength);
    totalGroupRecordLength=totalGroupRecordLength+groupRecordLength;
    groupPtrAux=groupPtrAux->nextGroupRecord;
  }

  if (i!=noOfRecords)
  {
    shortVal=(L7_ushort16) i;
    SNOOP_PUT_SHORT(shortVal, noRecordsPtr);
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with groupPrt %u<%u",i,noOfRecords);
  }

  /* Update frame length */
  *length = SNOOP_IGMPv1v2_HEADER_LENGTH +totalGroupRecordLength;
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "IGMP Frame Size  :%u",*length);

  /* Determine Checksum */
  shortVal = snoopCheckSum((L7_ushort16 *) buffer, *length, 0);
  SNOOP_PUT_SHORT(shortVal, chksumPtr);

  return L7_SUCCESS;
}

static L7_uchar8* snoopPTinGroupRecordV3Build(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType,L7_uint16 numberOfSources,snoopPTinProxySource_t* source, L7_uchar8 *buffer, L7_uint32 *length)
{

  L7_uchar8         *dataPtr,*noSourcesPtr, byteVal;
  L7_ushort16       shortVal;
  L7_uint32         ipv4Addr;
  snoopPTinProxySource_t* sourcePtr;
  L7_uint32 i;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};

  /* Argument validation */
  if (buffer == L7_NULLPTR || length == L7_NULLPTR || groupAddr==L7_NULLPTR || (numberOfSources>0 && source==L7_NULLPTR))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  if (numberOfSources>0 && source==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "He have a null pointer, while having the number of sources different of 0: %u",numberOfSources);
    return L7_NULLPTR;
  }

  dataPtr = buffer;

  /* Record Type */
  byteVal = recordType;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Aux Data Len */
  byteVal = 0x00;;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /*Number of Sources*/
  noSourcesPtr=dataPtr;//We save this ptr for future use if required
  shortVal = numberOfSources;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /*Multicast Address*/
  if (groupAddr->family!=L7_AF_INET)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid IP Family");
    return L7_NULLPTR;
  }

  ipv4Addr=groupAddr->addr.ipv4.s_addr;
  SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Number of Sources :%u", numberOfSources);
  sourcePtr=source;  
  for (i=0;i<numberOfSources && sourcePtr != L7_NULLPTR;i++)
  {
    /*Source Address*/
    if (groupAddr->family!=L7_AF_INET)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid IP Family");
      return L7_NULLPTR;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source (sourceAddr: %s)", inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));

    inetAddressGet(L7_AF_INET, &sourcePtr->key.sourceAddr,  &ipv4Addr );  
    SNOOP_PUT_DATA(&ipv4Addr, L7_IP_ADDR_LEN, dataPtr);

    sourcePtr=sourcePtr->nextSource;
  }

  if (i!=numberOfSources)
  {
    shortVal = i;
    SNOOP_PUT_SHORT(shortVal, noSourcesPtr);
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with groupRecord %u<%u",i,numberOfSources);
  }

  /* Update frame length */
  *length = SNOOP_IGMPV3_RECORD_GROUP_HEADER_MIN_LENGTH + L7_IP_ADDR_LEN * numberOfSources;
//bufferOut=dataPtr;

  L7_INTF_MASK_t mcastRtrAttached;  
  if (ptin_igmp_rootIntfs_getList(vlanId, &mcastRtrAttached)==L7_SUCCESS)
  {
    L7_uint32         intf; /* Loop through internal interface numbers */
    /* Increment Counter on all root interfaces in this VLAN with multicast routers attached */
    for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
    {
      if (L7_INTF_ISMASKBITSET(mcastRtrAttached,intf)) 
        ptin_igmp_stat_increment_field(intf, vlanId, (L7_uint32)-1, snoopRecordType2IGMPStatField(recordType,SNOOP_STAT_FIELD_TX));
    }  
  }

  return dataPtr;
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
L7_RC_t snoopPTinPacketBuild(L7_uint32 vlanId, snoop_cb_t* pSnoopCB, L7_inet_addr_t* destIp, L7_uchar8* buffer, L7_uint32* length, L7_uchar8* igmpFrameBuffer, L7_uint32 igmpFrameLength,L7_uint32 packetType)
{
  L7_uchar8           *dataPtr, *macHdrStartPtr, *ipHdrStartPtr, *chksumPtr, baseMac[L7_MAC_ADDR_LEN], destMac[L7_MAC_ADDR_LEN];
  L7_inet_addr_t      querierAddr;
  static L7_ushort16  iph_ident = 1;
  L7_ushort16         shortVal;
  L7_uint32      ipv4Addr;
  L7_uchar8           byteVal;

  /* Argument validation */
  if (pSnoopCB == L7_NULLPTR || buffer == L7_NULLPTR || length == L7_NULLPTR || igmpFrameBuffer == L7_NULLPTR || destIp==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  dataPtr = buffer;

  /* Build MAC Header */
  {
    macHdrStartPtr = dataPtr;

//  destIp.family = L7_AF_INET;
//  destIp.addr.ipv4.s_addr = (groupAddr->addr.ipv4.s_addr == 0)? L7_IP_ALL_HOSTS_ADDR : groupAddr->addr.ipv4.s_addr;

    /* Validate the group address being reported */
    snoopMulticastMacFromIpAddr(destIp, destMac);

    /* Validate MAC address */
    if (snoopMacAddrCheck(destMac, L7_AF_INET) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid MAC Address :%06x",destMac);
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
    SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
    shortVal = ((pSnoopCB->snoopCfgData->snoopAdminIGMPPrio & 0x07)<<13) | (vlanId & 0x0fff);
    SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes  

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
    if (packetType!=L7_IGMP_MEMBERSHIP_QUERY)
    {
      ipv4Addr = L7_IP_IGMPV3_REPORT_ADDR;
    }
    else /*if  (packetType==SNOOP_PTIN_MEMBERSHIP_QUERY)*/
    {
      ipv4Addr = (destIp->addr.ipv4.s_addr == 0)? L7_IP_ALL_HOSTS_ADDR : destIp->addr.ipv4.s_addr;
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
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Packet Size Invalid length : %u > L7_MAX_FRAME_SIZE",*length,L7_MAX_FRAME_SIZE);
    return L7_FAILURE;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Packet Size length : %u" ,*length);
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
  if (queryData->queryTimer != L7_NULLPTR)
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
  rc = snoopPTinQueryFrameV3Build(&queryData->groupAddr, queryData->sFlag, igmpFrame, &igmpFrameLength, pSnoopOperEntry, queryData->sourceList, queryData->sourcesCnt);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Query frame");
    return;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(mcastPacket.vlanId, pSnoopCB, &queryData->groupAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength,L7_IGMP_MEMBERSHIP_QUERY);
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
L7_RC_t snoopPTinReportSend(L7_uint32 vlanId, snoopPTinProxyGroup_t     *groupPtr, L7_uint32 noOfGroupRecords, ptin_IgmpProxyCfg_t* igmpCfg)
{
  L7_uchar8             igmpFrame[L7_MAX_FRAME_SIZE]={0};
  L7_uint32             igmpFrameLength=0;
//snoopOperData_t       *pSnoopOperEntry;
  L7_RC_t               rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t mcastPacket;  
  snoop_cb_t            *pSnoopCB;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Preparing to send a Membership Report Message with %u group records",noOfGroupRecords);

  /* Validate arguments */
  if (groupPtr==L7_NULLPTR || igmpCfg==L7_NULLPTR)
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

  /* Initialize mcastPacket structure */
  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));

//pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT);



  pSnoopCB->snoopCfgData->snoopAdminIGMPPrio=igmpCfg->igmp_cos;

  mcastPacket.cbHandle = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId = vlanId;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx = (L7_uint32) -1;
  mcastPacket.msgType = IP_PROT_IGMP;

  mcastPacket.srcAddr.family = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4=igmpCfg->ipv4_addr;

  mcastPacket.destAddr.family = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_IP_IGMPV3_REPORT_ADDR;

  /* Build header frame for IGMPv3 Query with no sources */   
  rc = snoopPTinReportFrameV3Build(noOfGroupRecords, groupPtr, igmpFrame,&igmpFrameLength );
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv3)");
    return L7_FAILURE;
  }

  /* Build MAC+IP frames and add the IGMP frame to the same packet */
  rc = snoopPTinPacketBuild(vlanId, pSnoopCB, &mcastPacket.destAddr, mcastPacket.payLoad, &mcastPacket.length, igmpFrame, igmpFrameLength,L7_IGMP_V3_MEMBERSHIP_REPORT);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building Membership Report Frame (IGMPv3)");
    return L7_FAILURE;
  }

  /* Send Packet to root interfaces */
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

  return rc;
}


#if 0
static int convert_ipaddr2uint64(const char *ipaddr, L7_uint32 *value_uint64)
{
  const char *start_ipaddr;
  L7_uint8  address[4] = { 0, 0, 0, 0};
  L7_uint8  index;
  L7_uint32 multiplier;

  // Validate argument
  if (ipaddr==L7_NULLPTR || *ipaddr=='\0' || value_uint64==L7_NULLPTR)
    return -1;

  // Search for the next non space/tab character
  for (; (*ipaddr==' ' || *ipaddr=='\t') && *ipaddr!='\0'; ipaddr++ );

  start_ipaddr = ipaddr;

  // Search for the end of the argument
  for (; *ipaddr!='\0' && (*ipaddr=='.' || isdigit(*ipaddr)) ; ipaddr++ );

  if (start_ipaddr==ipaddr)
    return -1;

  // Initialize Decimal multiplier
  multiplier = 1;
  // Run all characters starting from the last one
  for (index=0,--ipaddr; index<4 && ipaddr>=start_ipaddr; ipaddr--)
  {
    // If character is a decimal digit...
    if (isdigit(*ipaddr))
    {
      address[index] += (L7_uint8) (*ipaddr-'0')*multiplier;   // update address array
      multiplier*=10;                                       // update decimal multiplier for next digit
    }
    // Other, is a dot character
    else
    {
      index++;                                              // Increment address array index
      multiplier=1;                                         // Reinitialize decimal multiplier
    }
  }

  // Calculate uint32 value
  *value_uint64 = ((L7_uint32) address[0]<<0) | ((L7_uint32) address[1]<<8) | ((L7_uint32) address[2]<<16) | ((L7_uint32) address[3]<<24);

  return 0;
}
#endif

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
void snoopPTinMcastgroupPrint(L7_int32 vlanId,L7_uint32 groupAddrText)
{
  char                  debug_buf[IPV6_DISP_ADDR_LEN]={};
  snoopPTinL3InfoData_t *snoopEntry;
  L7_inet_addr_t        groupAddr;
  memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
  groupAddr.family=1;

  //char                 groupAddrStr[INET_ADDRSTRLEN];
  //char                *groupAddrPtr=L7_NULLPTR;


  if ( vlanId<0 || vlanId>4095 /*|| addrStr==L7_NULLPTR*/)
  {
    printf("Invalid Arguments: %d", vlanId);  
    return;
  }


//if (convert_ipaddr2uint64(groupAddrText,&groupAddr.addr.ipv4.s_addr)<0)
//{
//  printf("Invalid ip address\r\n");
//  return;
//}

  groupAddr.addr.ipv4.s_addr=groupAddrText;
//if (inetIsInMulticast(&groupAddr)!=L7_TRUE)
//{
//  printf("Invalid Multicast IP Address : %s", inetAddrPrint(&groupAddr,debug_buf));
//  return;
//}

  printf("-----------------------------------------\n");
  /* Search for the requested multicast group */
  if (L7_NULLPTR != (snoopEntry = snoopPTinL3EntryFind(vlanId, &groupAddr, L7_MATCH_EXACT)))
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
        printf("              |Static:         %s\n", snoopEntry->interfaces[ifIdx].isStatic?"Yes":"No");        
        printf("              |Filter-Mode:    %s\n", snoopEntry->interfaces[ifIdx].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE?"Include":"Exclude");
        printf("              |Nbr of Sources: %u\n", snoopEntry->interfaces[ifIdx].numberOfSources);        
        printf("              |Group-Timer:    %u\n", snoop_ptin_grouptimer_timeleft(&snoopEntry->interfaces[ifIdx].groupTimer));                
        printf("              |Nbr of Clients: %u\n", snoopEntry->interfaces[ifIdx].numberOfClients);        
        printf("              |Clients: ");
        L7_int8 clientIdx;
        for (clientIdx=(PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1); clientIdx>=0; --clientIdx)
        {
          printf("%08X", snoopEntry->interfaces[ifIdx].clients[clientIdx]);
        }                      
        printf("\n");
        for (sourceIdx=0; sourceIdx<PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++sourceIdx)
        {
          if (snoopEntry->interfaces[ifIdx].sources[sourceIdx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE)
          {
            L7_int8 clientIdx;
            printf("                       |Source: %s\n", inetAddrPrint(&(snoopEntry->interfaces[ifIdx].sources[sourceIdx].sourceAddr), debug_buf));
            printf("                                |Static:%s\n", snoopEntry->interfaces[ifIdx].sources[sourceIdx].isStatic?"Yes":"No");
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
  printf("-----------------------------------------\n");
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
void snoopPTinGroupRecordPrint(L7_uint32 vlanId,L7_uint32 groupAddrText,L7_uint8 recordType)
{
  char                  debug_buf[IPV6_DISP_ADDR_LEN]={};
  L7_uint32 i;

  L7_inet_addr_t        groupAddr;
  memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
  groupAddr.family=1;


  snoopPTinProxyInterface_t *interfacePtr;
  snoopPTinProxyGroup_t *groupPtr;    
  snoopPTinProxySource_t *sourcePtr;    

  if ( vlanId<0 || vlanId>4095 /*|| addrStr==L7_NULLPTR*/)
  {
    printf("Invalid Arguments: %d", vlanId);  
    return;
  }

  if ((interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId, AVL_EXACT)) == L7_NULLPTR)
  {
    printf("Interface not initialized for vlanId: %d", vlanId);  
    return;
  }

  groupAddr.addr.ipv4.s_addr=groupAddrText;

  if ((groupPtr=snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId, &groupAddr,recordType, AVL_EXACT)) == L7_NULLPTR)
  {
    printf("Group Record not initialized: (groupAddr: %s recordType:%u)", inetAddrPrint(&groupAddr,debug_buf), recordType);  
    return;

  }

  printf("|Group Address :%s\n", inetAddrPrint(&groupPtr->key.groupAddr,debug_buf));
  printf("|Record Type:    %u\n",groupPtr->recordType );
  printf("|Robustness Variable:    %u\n",groupPtr->robustnessVariable);
  printf("|Retransmissions:    %u\n",groupPtr->retransmissions);
  printf("|Nbr of Sources: %u\n", groupPtr->numberOfSources);  
  sourcePtr=groupPtr->source;  
  for (i=0;i<groupPtr->numberOfSources && sourcePtr!=L7_NULLPTR; i++)
  {
    printf("  |Source Address: %s\n", inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));
    printf("  |Robustness Variable:    %u\n",sourcePtr->robustnessVariable);
    printf("  |Retransmissions:    %u\n",sourcePtr->retransmissions);
    sourcePtr=sourcePtr->nextSource;
  }  
}


static snoopPTinProxyGroup_t* snoopPTinGroupRecordIncrementTransmissions(L7_uint32 noOfRecords,snoopPTinProxyGroup_t* groupPtr, L7_uint32* newNoOfRecords)
{  
  L7_RC_t                 rc=L7_SUCCESS;
  L7_uint32               i,intf,noOfActiveInterfaces=0;

  snoopPTinProxyGroup_t   *newgroupPtr=groupPtr;
  snoopPTinProxyGroup_t   *groupPtrAux=groupPtr;

  
  L7_inet_addr_t          *groupAddr[SNOOP_IGMPv3_MAX_GROUP_RECORD_PER_REPORT];
  L7_uint8                recordType[SNOOP_IGMPv3_MAX_GROUP_RECORD_PER_REPORT];
  L7_uint32               noOfGroupRecord2remove=0;
  L7_BOOL                 flagFirstGroupRecordRemoved=L7_FALSE,flagPendingGroupRecord=L7_FALSE;
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;

  char                debug_buf[IPV6_DISP_ADDR_LEN];

  L7_INTF_MASK_t mcastRtrAttached;
  L7_uint8 intIfList[L7_MAX_INTERFACE_COUNT];


  /* Argument validation */
  if (groupPtr == L7_NULLPTR || newNoOfRecords==L7_NULLPTR || noOfRecords==0 ||
      (interfacePtr=groupPtr->interfacePtr)==L7_NULLPTR || groupPtr->interfacePtr->key.vlanId<PTIN_IGMP_MIN_VLAN_ID || groupPtr->interfacePtr->key.vlanId>PTIN_IGMP_MAX_VLAN_ID)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  if (ptin_igmp_rootIntfs_getList(groupPtrAux->key.vlanId, &mcastRtrAttached)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed ptin_igmp_rootIntfs_getList()");
    return L7_NULLPTR;
  }

  /*IGMPv3 Stats*/
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    if (L7_INTF_ISMASKBITSET(mcastRtrAttached,intf))
    {
      intIfList[noOfActiveInterfaces++]=intf;
    }
  }
  /*End IGMPv3 Stats*/

  for (i=0;i<noOfRecords&&groupPtrAux!=L7_NULLPTR;i++)
  {
    /*Increment IGMPv3 Stats*/
    for (intf = 1; intf <= noOfActiveInterfaces; intf++)
    {
      ptin_igmp_stat_increment_field(intIfList[intf], groupPtrAux->key.vlanId, 0, snoopRecordType2IGMPStatField(groupPtrAux->recordType,SNOOP_STAT_FIELD_TX));          
    }
    /*End Stats*/

    if (snoopPTinGroupRecordSourceIncrementTransmissions(groupPtrAux)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinGroupRecordSourceIncrementTransmissions()");
//    return L7_NULLPTR;
    }
    if (++groupPtrAux->retransmissions>=groupPtrAux->robustnessVariable)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding Group Record %s with recorType %u  (retransmissions:%u) to Removal List", inetAddrPrint(&groupPtrAux->key.groupAddr, debug_buf),groupPtrAux->recordType,groupPtrAux->retransmissions);      
      groupAddr[noOfGroupRecord2remove]=&groupPtrAux->key.groupAddr;
      recordType[noOfGroupRecord2remove++]=groupPtrAux->key.recordType;                  

      if (groupPtrAux==groupPtr /*&& groupPtr->nextGroupRecord!=L7_NULLPTR*/)/*First Group Record*/
      {
        newgroupPtr=groupPtr->nextGroupRecord;
        flagFirstGroupRecordRemoved=L7_TRUE;
      }
      else if (flagPendingGroupRecord==L7_FALSE && flagFirstGroupRecordRemoved==L7_TRUE)
      {
        newgroupPtr=groupPtr->nextGroupRecord;
      }
    }
    else
    {
      if (flagPendingGroupRecord==L7_FALSE)
        flagPendingGroupRecord=L7_TRUE;
    }
    groupPtrAux=groupPtrAux->nextGroupRecord;
  }

  if (i!=noOfRecords)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with groupRecordPtr %u<%u",i,noOfRecords);
  }


  if (((*newNoOfRecords)=noOfRecords-noOfGroupRecord2remove)==0)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Going to remove all Group Records");
    newgroupPtr=L7_NULLPTR;    
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Number of Group Records pending to be Retransmitted %u",*newNoOfRecords);
    if (newgroupPtr==L7_NULLPTR)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid Group Record Pointer");
    }
  }

  for (i=0;i<noOfGroupRecord2remove;i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to Remove  Group Record:  vlanId:%u GroupAdd=%s, RecordType=%u",interfacePtr->key.vlanId, inetAddrPrint(groupAddr[i], debug_buf),recordType[i]);     
    if ((rc=snoopPTinGroupRecordRemove(interfacePtr,groupAddr[i], recordType[i]))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");        
    }
  }

  return newgroupPtr;
}


static L7_RC_t snoopPTinGroupRecordSourceIncrementTransmissions(snoopPTinProxyGroup_t* groupPtr)
{  
  L7_RC_t                   rc=L7_SUCCESS;  
  snoopPTinProxySource_t    *sourcePtrTmp;
  L7_inet_addr_t            *sourceAddr[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD];
  L7_uint32                 i,noOfSources2Remove=0;

  char                      debug_buf[IPV6_DISP_ADDR_LEN];

  /* Argument validation */
  if (groupPtr == L7_NULLPTR || (groupPtr->source==L7_NULLPTR && groupPtr->numberOfSources!=0) || (groupPtr->source!=L7_NULLPTR && groupPtr->numberOfSources==0) 
      || groupPtr->numberOfSources>PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (groupPtr->numberOfSources==0)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "This Group record was no sources");
    return L7_SUCCESS;
  }

  sourcePtrTmp=groupPtr->source;
  for (i=0;i<groupPtr->numberOfSources && sourcePtrTmp!=L7_NULLPTR;i++)
  {
    if (++sourcePtrTmp->retransmissions>=sourcePtrTmp->robustnessVariable)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding Source Address %s from the group record (retransmissions:%u) to Removal List", inetAddrPrint(&sourcePtrTmp->key.sourceAddr, debug_buf),sourcePtrTmp->retransmissions); 
      sourceAddr[noOfSources2Remove++]=&sourcePtrTmp->key.sourceAddr;      
    }
    sourcePtrTmp=sourcePtrTmp->nextSource;
  }

  if (i!=groupPtr->numberOfSources)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with sourcePtr %u<%u",i,groupPtr->numberOfSources);
  }

  if ( groupPtr->numberOfSources==noOfSources2Remove)/*We should remove all sources within this group record*/
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Going to Remove all Sources from Group Record  (groupAddr:%s recordType:%u)", inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->recordType);  
    rc=snoopPTinGroupRecordSourceRemoveAll(groupPtr);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemoveAllSources()");
    }
  }
  else
  {
    for (i=0;i<noOfSources2Remove;i++)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to remove Source from Group Record  %s", inetAddrPrint(sourceAddr[i], debug_buf));     
      rc=snoopPTinGroupRecordSourceRemove(groupPtr,sourceAddr[i]);
      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");        
      }
    }
  }  

  return rc;
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
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
static snoopPTinProxyGroup_t* snoopPTinBuildCSR(snoopPTinProxyInterface_t *interfacePtr, L7_uint32 *noOfRecordsPtr,L7_uint8 robustnessVariable)
{
  snoopPTinL3InfoData_t     *avlTreeEntry;  
  snoopPTinL3InfoDataKey_t  avlTreeKey;
  L7_BOOL                   newEntry;

  char                      debug_buf[IPV6_DISP_ADDR_LEN]={};

  snoopPTinProxyGroup_t*    groupPtr;
  snoopPTinProxyGroup_t*    firstGroupPtr=L7_NULLPTR;

  L7_uint32                 sourceIdx,groupIdx=0,noOfRecords=0;
  L7_uint8                  recordType;  

  snoop_eb_t                *pSnoopEB;

  /* Argument validation */
  if (interfacePtr ==L7_NULLPTR || noOfRecordsPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }


  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return L7_NULLPTR;
  }

  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Building Current State Records of vlanId:%u",interfacePtr->key.vlanId);
/* Run all cells in AVL tree */    

  memset(&avlTreeKey,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  while ( ( avlTreeEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &avlTreeKey, L7_MATCH_GETNEXT) ) != L7_NULLPTR )
  {

    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));

    if (avlTreeEntry->snoopPTinL3InfoDataKey.vlanId==interfacePtr->key.vlanId && 
        avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_TRUE && 
#if (PTIN_BOARD_IS_LINECARD)
        avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic==L7_FALSE && 
#endif
        snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients)==L7_ALREADY_CONFIGURED)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Group Address Number %u",++groupIdx);
      if (avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE)
      {
        recordType=L7_IGMP_MODE_IS_INCLUDE;        
      }
      else
      {
        recordType=L7_IGMP_MODE_IS_EXCLUDE;
      }

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  recordType:%u", inetAddrPrint(&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf),recordType);    

      if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,&newEntry,robustnessVariable))== L7_NULLPTR)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
        return L7_NULLPTR;
      }
      //Verify is this Group Record was created before
      if (newEntry==L7_FALSE)
      {
       //Verify if the timer is running for this Group Record. If so, stop it.
        if (snoop_ptin_proxytimer_isRunning(&groupPtr->timer)==L7_TRUE)
        {
          if (snoop_ptin_proxytimer_stop(&groupPtr->timer)!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_stop()");
          }
        }
        //Remove Group Record and Add it again
        if (snoopPTinGroupRecordRemove(interfacePtr,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, recordType)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemove()");        
          return L7_NULLPTR;
        }
        else
        {
          if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,&newEntry,robustnessVariable))== L7_NULLPTR)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
            return L7_NULLPTR;
          }
        }
      }
      ++noOfRecords;           

      if (firstGroupPtr==L7_NULLPTR)
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "First Group Ptr Added");
        firstGroupPtr=groupPtr;
      }

      for (sourceIdx=0;sourceIdx<PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP;sourceIdx++)
      {
        if (avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].status==PTIN_SNOOP_SOURCESTATE_ACTIVE &&  
            avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].sourceTimer.isRunning==L7_TRUE &&
            snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].clients)==L7_ALREADY_CONFIGURED)
        {
          if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,&avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].sourceAddr,robustnessVariable))
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
            return L7_NULLPTR;
          }
        }
      }       
    }
  }  

  *noOfRecordsPtr=noOfRecords;
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Number of Group Records to be sent :%u",*noOfRecordsPtr);
  return firstGroupPtr;       
}


/*************************************************************************
 * @purpose Dump IGMPv3 AVL Tree
 *
 *
 *
 *************************************************************************/
void snoopPTinDumpL3AvlTree(void)
{
  snoopPTinL3InfoData_t     *avlTreeEntry;  
  snoopPTinL3InfoDataKey_t  avlTreeKey;
  snoop_eb_t                *pSnoopEB;

  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

/* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "snoopPTinDumpL3AvlTree");
  while ( ( avlTreeEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &avlTreeKey, L7_MATCH_GETNEXT) ) != L7_NULLPTR )
  {    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));

    snoopPTinMcastgroupPrint(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr);
  }
}


/*************************************************************************
 * @purpose Dump IGMPv3 Group Record AVL Tree
 *
 *
 *
 *************************************************************************/
void snoopPTinDumpGroupRecordAvlTree(void)
{
  snoopPTinProxyGroup_t     *avlTreeEntry;  
  snoopPTinProxyGroupKey_t  avlTreeKey;
  
  snoop_eb_t                *pSnoopEB;
  char                      debug_buf[IPV6_DISP_ADDR_LEN]={};

  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }


  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "snoopPTinDumpGroupRecordAvlTree");
/* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  while ( ( avlTreeEntry = avlSearchLVL7(& pSnoopEB->snoopPTinProxyGroupAvlTree, &avlTreeKey, L7_MATCH_GETNEXT) ) != L7_NULLPTR )
  {
    
    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->key, sizeof(snoopPTinProxyGroupKey_t));

//  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "vlanId:%u  groupAddr  %s recordType:%u",avlTreeEntry->key.vlanId,inetAddrPrint(&avlTreeEntry->key.groupAddr, debug_buf),avlTreeEntry->key.recordType);
    printf("Vlan ID: %u Group: %s     recordType:%u\n\n",avlTreeEntry->key.vlanId,inetAddrPrint(&avlTreeEntry->key.groupAddr, debug_buf),avlTreeEntry->key.recordType);

    snoopPTinGroupRecordPrint(avlTreeEntry->key.vlanId,avlTreeEntry->key.groupAddr.addr.ipv4.s_addr,avlTreeEntry->key.recordType);

  }
}

