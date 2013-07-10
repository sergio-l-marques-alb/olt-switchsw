/*
 * snooping_ptin_db.c
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#include "snooping_ptin_db.h"
#include "snooping_ptin_grouptimer.h"
#include "snooping_ptin_sourcetimer.h"
#include "snooping_ptin_querytimer.h"

#include "snooping_ptin_proxytimer.h"

#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "comm_structs.h"

#include "ptin_debug.h"
#include "ptin_igmp.h"

/*********************************************************************
* Static Methods
*********************************************************************/
static L7_RC_t  snoopPTinClientFind  (L7_uint32 *clientList, L7_uint32 clientIdx);
static L7_RC_t  snoopPTinClientAdd   (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);
static L7_RC_t  snoopPTinClientRemove(snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);

/*************************************************************************
 * @purpose Search a specific client in a given client list
 *
 * @param clientList    List of clients on which to search for the given
 *                      client
 * @param clientIdx     Index of the requested client
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoopPTinClientFind(L7_uint32 *clientList, L7_uint32 clientIdx)
{
  if (PTIN_IS_MASKBITSET(clientList, clientIdx))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*************************************************************************
 * @purpose Adds a new client to the given source in the first free index
 *
 * @param interfacePtr  Interface in which the source is
 * @param sourcePtr     Source in which the client should be added
 * @param clientIdx     Index of the requested client
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @todo Add client to L2
 *
 *************************************************************************/
L7_RC_t snoopPTinClientAdd(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx)
{
  L7_uint32 i, clientSourceCnt;

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR || sourcePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Count the number of sources this client has */
  for (i = 0, clientSourceCnt = 0; i < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++i)
  {
    if (snoopPTinClientFind(interfacePtr->sources[i].clients, clientIdx))
    {
      ++clientSourceCnt;
      break;
    }
  }

  /* If does not exist in any source, add it to the L2 table */
  if (clientSourceCnt == 0)
  {
    snoopPTinL2ClientAdd(); // PLACEHOLDER    
  }

  /* Add the client to this source*/
  if (L7_SUCCESS != snoopPTinClientFind(sourcePtr->clients, clientIdx))
  {
     PTIN_SET_MASKBIT(sourcePtr->clients, clientIdx);
     ++sourcePtr->numberOfClients;
  }

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Removes a client from the given source
 *
 * @param interfacePtr  Interface in which the source is
 * @param sourcePtr     Source from which the client should be removed
 * @param clientIdx     Index of the requested client
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @todo Remove client from L2
 *
 *************************************************************************/
L7_RC_t snoopPTinClientRemove(snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx)
{
  L7_uint32 i, clientSourceCnt;

  /* Argument validation */
  if (sourcePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Count the number of sources this client has */
  for (i = 0, clientSourceCnt = 0; i < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++i)
  {
    if (snoopPTinClientFind(sourcePtr->clients, clientIdx))
    {
      ++clientSourceCnt;
    }
  }

  /* If this client is only on one source, remove it from the L2 table */
  if (clientSourceCnt == 1)
  {
    snoopPTinL2ClientRemove(); // PLACEHOLDER
  }

  /* Remove the client from this source */
  if (L7_SUCCESS != snoopPTinClientFind(sourcePtr->clients, clientIdx))
  {
     PTIN_UNSET_MASKBIT(sourcePtr->clients, clientIdx);
     --sourcePtr->numberOfClients;
  }

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t* interfacePtr, L7_inet_addr_t sourceAddr, L7_uint8* newSourceIdx)
{
  L7_uint16 idx;

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR || newSourceIdx == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  for (idx = 0; idx < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++idx)
  {
    if (interfacePtr->sources[idx].status == PTIN_SNOOP_SOURCESTATE_INACTIVE)
    {
      memset(&interfacePtr->sources[idx], 0x00, sizeof(snoopPTinL3Source_t));
      interfacePtr->sources[idx].status     = PTIN_SNOOP_SOURCESTATE_ACTIVE;
      interfacePtr->sources[idx].sourceAddr = sourceAddr;
      *newSourceIdx                         = idx;
      ++interfacePtr->numberOfSources;      

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*************************************************************************
 * @purpose Search a specific source in a given source list
 *
 * @param   sourceList  List of sources on which to search for the given
 *                      source
 * @param   sourceAddr  Address of the requested source
 * @param   foundIdx    If source is found, this will hold the source
 *                      index in the source list. If passed as L7_NULLPTR
 *                      it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_inet_addr_t sourceAddr, L7_uint8* foundIdx)
{
  L7_uint16 idx;

  /* Argument validation */
  if (sourceList == L7_NULLPTR || foundIdx == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  for (idx = 0; idx < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++idx)
  {
    if ((sourceList[idx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE) && L7_INET_ADDR_COMPARE(&(sourceList[idx].sourceAddr),&sourceAddr)==L7_TRUE) 
    {
      *foundIdx = idx;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*************************************************************************
 * @purpose Remove a source entry, reseting all its fields and stopping
 *          the associated timer
 *
 * @param   interfacePtr        Interface in which the source is
 * @param   sourcePtr           Source to remove
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @note This method also removes every client associated with this source
 *
 *************************************************************************/
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t* interfacePtr, snoopPTinL3Source_t* sourcePtr)
{
  /* Argument validation */
  if (interfacePtr == L7_NULLPTR || sourcePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Remove clients associated with this source */
  memset(sourcePtr->clients, 0x00, PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE);

  snoop_ptin_sourcetimer_stop(&sourcePtr->sourceTimer);
  memset(sourcePtr, 0x00, sizeof(*sourcePtr));
  --interfacePtr->numberOfSources;  

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Used to initialize an interface
 *
 * @param   interfacePtr  Pointer to group's interface structure
 *          (snoopPTinL3Interface_t)
 *
 * @todo    Set query-timer.
 *
 *************************************************************************/
L7_RC_t snoopPTinInitializeInterface(snoopPTinL3Interface_t *interfacePtr,L7_uint32 vlanId, L7_inet_addr_t* mcastGroupAddr,L7_uint16 interfaceIdx)
{
  /* Argument validation */
  if (interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  memset(interfacePtr, 0x00, sizeof(*interfacePtr));
  interfacePtr->active     = L7_TRUE;
  interfacePtr->filtermode = PTIN_SNOOP_FILTERMODE_INCLUDE;

  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM!=interfaceIdx)
  {
    /*Open L2 Port on Switch*/
    if(snoopGroupIntfAdd(vlanId,mcastGroupAddr, interfaceIdx)!=L7_SUCCESS)
    {
     LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopGroupIntfAdd()");
     return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Remove an interface entry, reseting all its fields and
 *          stopping the group and query timers
 *
 * @param   interfacePtr  Interface to remove
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoopPTinInterfaceRemove(snoopPTinL3Interface_t *interfacePtr,L7_uint32 vlanId, L7_inet_addr_t *mcastGroupAddr,L7_uint16 interfaceIdx)
{
  snoopPTinL3Source_t     *sourcePtr;
  L7_uint32               i;

  snoopPTinProxyInterface_t *rootInterfacePtr=L7_NULLPTR;
  snoopPTinProxyGroup_t     *rootGroupPtr=L7_NULLPTR;


  /* Argument validation */
  if (interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Remove every source whose timer has expired */
  for (i = 0; i < sizeof(interfacePtr->sources); ++i)
  {
    sourcePtr = &interfacePtr->sources[i];
    if ((sourcePtr->status == PTIN_SNOOP_SOURCESTATE_ACTIVE) && (sourcePtr->sourceTimer.isRunning == L7_FALSE))
    {
      snoopPTinSourceRemove(interfacePtr, sourcePtr);
    }
  }
  memset(interfacePtr, 0x00, sizeof(*interfacePtr));

  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM!=interfaceIdx)
  {   
    /*Close L2 Port on Switch*/
    if(snoopGroupIntfRemove(vlanId,mcastGroupAddr,interfaceIdx)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopGroupIntfRemove()");
      return L7_FAILURE;
    }
  }
  else
  {
    if ( (rootInterfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId, L7_MATCH_EXACT)) ==L7_NULLPTR)
    {
      if (snoopPTinGroupRecordInterfaceAdd(vlanId,rootInterfacePtr)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
        return L7_FAILURE;
      }                                   
    }
     if (snoopPTinGroupDeleteAll(rootInterfacePtr,mcastGroupAddr)!=L7_SUCCESS)
     {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupDeleteAll()");
      return L7_FAILURE;
     }
    if(snoopPTinGroupRecordGroupAdd(rootInterfacePtr,L7_IGMP_CHANGE_TO_INCLUDE_MODE,mcastGroupAddr ,rootGroupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
      return L7_FAILURE;
    }
    if (snoopPTinReportSchedule(vlanId,mcastGroupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,1,rootGroupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }   
  
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type MODE_IS_INCLUDE
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
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0;
  L7_uint8            recordType=L7_IGMP_ALLOW_NEW_SOURCES;   

  L7_uint32           noOfRecords=0;



  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /*Output Variables Validation*/
  if (noOfRecordsPtr == L7_NULLPTR || groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /* Add new sources */
  while (noOfSources > 0)
  {
    L7_uint8  sourceIdx = 0;
    L7_inet_addr_t ipv4Addr;

    /* Search for this source in the current source list */
    inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(&ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=ipv4Addr;           
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&ipv4Addr, debug_buf), sourceIdx);
    }

    /* Set source-timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
        return L7_FAILURE;
      }
    }

    --noOfSources;
  }

  if (sources2ReportCnt>0)
  {
    if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr))
    {

      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
      return L7_FAILURE;
    }
    noOfRecords++;
  }
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type MODE_IS_EXCLUDE
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
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint16           i;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0;  

  L7_uint32 noOfRecords=0;

///*Output Variables Initialization*/
//*noOfRecords=0;
//groupPtr=L7_NULLPTR;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)  
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);  
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);


  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  
  /*
   * Start by marking every current source as toremove.
   *
   * Then, for each source in sourceList:
   *        New source    - Add
   *        Known source  - Re-mark as active
   *
   * Finally, remove all sources in sourceList still marked as toremove.
   */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    if(avlTreeEntry->interfaces[intIfNum].sources[i].status == PTIN_SNOOP_SOURCESTATE_ACTIVE)
    {
      avlTreeEntry->interfaces[intIfNum].sources[i].status = PTIN_SNOOP_SOURCESTATE_TOREMOVE;
    }
  }

  /* Add new sources */
  while (noOfSources > 0)
  {
    L7_uint8        sourceIdx = 0;
    L7_inet_addr_t  ipv4Addr;

    /* Search for this source in the current source list */
    inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      /* Mark source as active */
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status = PTIN_SNOOP_SOURCESTATE_ACTIVE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %d", inetAddrPrint(&ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=ipv4Addr;                  
      }
    }

    /*
     * If filter-mode:
     *      INCLUDE - set source-timer to 0
     *              - add client
     *      EXCLUDE - set source-timer to GMI
     *              - remove client
     */
    if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
    {
      /* Set source-timer to 0 */
      if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, 0, avlTreeEntry, intIfNum, sourceIdx))
      {
         LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
         return L7_FAILURE;
      }

      /* Add client if it does not exist */
      if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
          return L7_FAILURE;
        }
      }
    }
    else if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
    {
      /* Set source-timer to GMI */
      if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
      {
         LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
         return L7_FAILURE;
      }

      /* Remove client */
      if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to remove clientIdx %u", clientIdx);
          return L7_FAILURE;
        }
      }
    }

    --noOfSources;
  }

  /* Set group-timer to GMI */
  if (L7_SUCCESS != snoop_ptin_grouptimer_start(&avlTreeEntry->interfaces[intIfNum].groupTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum))
  {
     LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start grouptimer");
     return L7_FAILURE;
  }

  /* Remove every source still marked as toremove */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    if (avlTreeEntry->interfaces[intIfNum].sources[i].status == PTIN_SNOOP_SOURCESTATE_TOREMOVE)
    {
      snoopPTinSourceRemove(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[i]);

      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr;                  
      }
    }
  }


  if (sources2ReportCnt>0)
  {
    if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,L7_IGMP_BLOCK_OLD_SOURCES,sources2Report,sources2ReportCnt,groupPtr))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
      return L7_FAILURE;
    }
    noOfRecords++;
  }  
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type CHANGE_TO_INCLUDE
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
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t      sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer active
  L7_uint16           sources2QueryCnt = 0;    
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32 i,noOfRecords=0 ;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0;
  L7_uint8            recordType=L7_IGMP_ALLOW_NEW_SOURCES;

///*Output Variables Initialization*/
//*noOfRecords=0;
//groupPtr=L7_NULLPTR;
  
  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }


  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }
 
  /* Add new sources */
  while (noOfSources > 0)
  {
    L7_uint8  sourceIdx = 0;
    L7_inet_addr_t ipv4Addr;

    /* Search for this source in the current source list */
    inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&ipv4Addr, debug_buf), sourceIdx);            
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(&ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if ((L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx)) /*& (L7_SUCCESS != snoopPTinProxyDBSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))*/)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=ipv4Addr;                  
      }     
    }

    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
        return L7_FAILURE;
      }
    }   
    /*Add Source to Not Query List*/
    avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status=PTIN_SNOOP_SOURCESTATE_TOREMOVE;  

    --noOfSources;
  }

  /* Add to Query sources with timer > 0 && Not in B */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {    
      if (avlTreeEntry->interfaces[intIfNum].sources[i].status== PTIN_SNOOP_SOURCESTATE_ACTIVE &&  avlTreeEntry->interfaces[intIfNum].sources[i].sourceTimer.isRunning == L7_TRUE)      
      {
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(&(avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr), debug_buf));
            sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr;
            ++sources2QueryCnt;     
      }
      
      if (avlTreeEntry->interfaces[intIfNum].sources[i].status== PTIN_SNOOP_SOURCESTATE_TOREMOVE)        
      {
        /*Restore Source State to Active*/
        avlTreeEntry->interfaces[intIfNum].sources[i].status= PTIN_SNOOP_SOURCESTATE_ACTIVE;
      }                        
  }

  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM != intIfNum)
  {  
    /* send Q(G,A-B) */
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Group & Source Specific Query G=%s",inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf));
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);

    /* If filter-mode is EXCLUDE: send Q(G) */
    if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
    {      
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Group Specific Query G=%s",inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf));
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, L7_NULLPTR, 0);
    }
  }  
  else if (sources2ReportCnt>0)
  {  
    if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
      return L7_FAILURE;
    }
    noOfRecords++;  
  }
  *noOfRecordsPtr=noOfRecords;
    
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type CHANGE_TO_EXCLUDE
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
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t           sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources 2 Query
  L7_uint16                sources2QueryCnt = 0;
  
  L7_inet_addr_t          sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8                sources2ReportCnt=0;
  L7_uint8                recordType=L7_IGMP_BLOCK_OLD_SOURCES;

  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32           i,noOfRecords=0;

  /*Output Variables Initialization*/
//*noOfRecords=0;
//groupPtr=L7_NULLPTR;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }
   
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM && avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;
  }
  /*
   * Mark every current source as toremove
   *
   * Then, for each source in sourceList:
   *        New source    - Add
   *        Known source  - Re-mark as active
   *
   * Finally, remove all sources in sourceList still marked as toremove.
   */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
     if(avlTreeEntry->interfaces[intIfNum].sources[i].status == PTIN_SNOOP_SOURCESTATE_ACTIVE)
     {
       avlTreeEntry->interfaces[intIfNum].sources[i].status = PTIN_SNOOP_SOURCESTATE_TOREMOVE;
     }
  }

  /* Add new sources */
  while (noOfSources > 0)
  {
    L7_uint8            sourceIdx = 0;
    L7_inet_addr_t      ipv4Addr;

    /* Search for this source in the current source list */
    inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&ipv4Addr, debug_buf), sourceIdx);

      /* Restore source status */
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status = PTIN_SNOOP_SOURCESTATE_ACTIVE;
      
      /* If filter-mode is (INCLUDE) or (EXCLUDE and source-timer is equal to 0), save this source. In the end of this method, all saved sources will be sent in a Q(G,S) */
      if(avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE || avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_TRUE)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(&(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr), debug_buf));
        sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
        ++sources2QueryCnt;    
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(&ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=ipv4Addr;                  
      }
      /* If filter-mode is exclude, set source-timer to GT, save this source. */
      if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
      {
          L7_uint32 group_timer;        
          group_timer = snoop_ptin_grouptimer_timeleft(&avlTreeEntry->interfaces[intIfNum].groupTimer);

          /* Set source-timer to GT */                
        if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, group_timer, avlTreeEntry, intIfNum, sourceIdx))
        {
           LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
           return L7_FAILURE;
        }

        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(&ipv4Addr, debug_buf));
        sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
        ++sources2QueryCnt;
      }
      else
      {
        /* Set source-timer to 0 */
        if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, 0, avlTreeEntry, intIfNum, sourceIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
          return L7_FAILURE;
        }
      }
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
        return L7_FAILURE;
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing clientIdx %u", clientIdx);
    }

    --noOfSources;
  }

  /*
   *  - Remove every source still marked as toremove
   *  - Set group-timer to GMI
   *  - Set filter-mode to EXCLUDE
   *  - Send Q(G,S), where S are all new sources
   */
  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    avlTreeEntry->interfaces[intIfNum].filtermode = PTIN_SNOOP_FILTERMODE_EXCLUDE;   
  }

  /* Remove every source still marked as toremove */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    if (avlTreeEntry->interfaces[intIfNum].sources[i].status == PTIN_SNOOP_SOURCESTATE_TOREMOVE)
    {
      snoopPTinSourceRemove(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[i]);

      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr;                  
      }
    
    }
  }

  /* Set group-timer to GMI */
  if (L7_SUCCESS != snoop_ptin_grouptimer_start(&avlTreeEntry->interfaces[intIfNum].groupTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum))
  {
     LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start grouptimer");
     return L7_FAILURE;
  }

  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM != intIfNum)
  {  
    /* Send a Q(G,S)*/
    if (sources2QueryCnt > 0)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Group & Source Specific Query G=%s", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf));
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);
    }
  }   
  else
  {
    if (sources2ReportCnt>0)         
    {
      if(L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr) )
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
        return L7_FAILURE;
      }
      noOfRecords++;
    }
  }  
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type ALLOW_NEW_SOURCES
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
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0;
  L7_uint8            recordType=L7_IGMP_ALLOW_NEW_SOURCES;        
  
  L7_uint32           noOfRecords=0;  

///*Output Variables Initialization*/
//*noOfRecords=0;
//groupPtr=L7_NULLPTR;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }  
 
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  while (noOfSources > 0)
  {
    L7_uint8  sourceIdx = 0;
    L7_inet_addr_t ipv4Addr;   

   inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);

    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }           
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        sources2Report[sources2ReportCnt++]=ipv4Addr;         
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s on idx %u", inetAddrPrint(&ipv4Addr, debug_buf), sourceIdx);           
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&ipv4Addr, debug_buf), sourceIdx);
    }

    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
        return L7_FAILURE;
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing clientIdx %u", clientIdx);
    }

    --noOfSources;
  }

  if (sources2ReportCnt>0)       
  {
    if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
      return L7_FAILURE;
    }
    noOfRecords++;
  }
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type BLOCK_OLD_SOURCES
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
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t      sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources 2 Query
  L7_uint16           sources2QueryCnt = 0;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0;
  L7_uint8            recordType;

  char                debug_buf[IPV6_DISP_ADDR_LEN];
  L7_uint8            sourceIdx;
  L7_inet_addr_t      ipv4Addr;
  ptin_IgmpProxyCfg_t igmpCfg;

  L7_uint32           noOfRecords=0;

///*Output Variables Initialization*/
//*noOfRecords=0;
//groupPtr=L7_NULLPTR;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);   
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
    recordType=L7_IGMP_BLOCK_OLD_SOURCES;
  else
    recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;

  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    while (noOfSources > 0)
    {
      /* Search for this source in the current source list */
      inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
      {
        /* Remove this client from this source */
        if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
        {
          snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx);          
        }

        /* Add source to to the list of sources to query */
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(&ipv4Addr, debug_buf));
        if (snoop_ptin_sourcetimer_timeleft(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer) >
            (igmpCfg.querier.last_member_query_count * SNOOP_MAXRESP_INTVL_ROUND(igmpCfg.querier.last_member_query_interval, SNOOP_IGMP_FP_DIVISOR)))
        {
          sources2Query[sources2QueryCnt]=ipv4Addr;
          ++sources2QueryCnt;
        }
      }

      --noOfSources;
    }
  }
  else if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
  {
    while (noOfSources > 0)
    {
      /* Search for this source in the current source list */
      inetAddressSet(L7_AF_INET, *sourceList,&ipv4Addr);
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
      {
        /* Add source to querySourceList if source-timer is not active */
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_FALSE)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(&ipv4Addr, debug_buf));
          sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
          ++sources2QueryCnt;
        }
      }
      else
      {
        /* Add new source */
        if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
          return L7_FAILURE;
        }       
        if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
        {
          sources2Report[sources2ReportCnt++]=ipv4Addr;                      
        }      
        L7_uint32 group_timer;

        /* Set source-timer to GT */
        group_timer = snoop_ptin_grouptimer_timeleft(&avlTreeEntry->interfaces[intIfNum].groupTimer);
        if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, group_timer, avlTreeEntry, intIfNum, sourceIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
          return L7_FAILURE;
        }        
      }
      --noOfSources;
    }  
  }

  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM!=intIfNum)
  {
    /* Send a Q(G,S)*/
    if (sources2QueryCnt > 0)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Group & Source Specific Query G=%s", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf));
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);
    }
  }
  else
  {
    if ( sources2ReportCnt>0)
    {
       if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr))
       {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");       
          return L7_FAILURE;
       }
    }
    noOfRecords++;
  }
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*MGMD Proxy*/


/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordInterfaceAdd(L7_uint32 vlanId, snoopPTinProxyInterface_t* interfacePtr)
{   

  interfacePtr=L7_NULLPTR;

  if ( (interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  if(snoopPTinProxyInterfaceEntryAdd(vlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceEntryAdd()");
    return L7_FAILURE;
  }
  if ( (interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,L7_MATCH_EXACT))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceEntryFind()");
    return L7_FAILURE;
  }    
  
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordGroupAdd(snoopPTinProxyInterface_t* interfacePtr,L7_uint8 recordType, L7_inet_addr_t* groupAddr,snoopPTinProxyGroup_t*  groupPtr )
{
  groupPtr=L7_NULLPTR;

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,groupAddr,recordType,L7_MATCH_EXACT))!=L7_NULLPTR)
  {    
    return L7_SUCCESS;
  }   
     
  if(snoopPTinProxyGroupEntryAdd((L7_uint32) &interfacePtr,groupAddr,recordType)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryAdd()");
    return L7_FAILURE;
  }
  if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,groupAddr,recordType,L7_MATCH_EXACT))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryFind()");
    return L7_FAILURE;
  }
      
  if (interfacePtr->groupRecord==L7_NULLPTR)
  {
    interfacePtr->groupRecord=groupPtr;  
  }
  else
  {    
    interfacePtr->groupRecord->nextGroupRecord=groupPtr;
    groupPtr->previousGroupRecord=interfacePtr->groupRecord;    
  }  
  interfacePtr->numberOfGroupRecords++; 
  return L7_SUCCESS;
}


/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordSourcedAdd(snoopPTinProxyGroup_t* groupPtr,L7_inet_addr_t*   sourceAddr, snoopPTinProxySource_t* sourcePtr)
{
 
  sourcePtr=L7_NULLPTR;
  char                debug_buf[IPV6_DISP_ADDR_LEN];

  /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if ((sourcePtr=snoopPTinProxySourceEntryFind((L7_uint32) &groupPtr,sourceAddr,L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing Source :%s, restoring retransmission variable",inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));
    sourcePtr->retransmissions=sourcePtr->robustnessVariable;
    return L7_SUCCESS;
  }
  if(snoopPTinProxySourceEntryAdd((L7_uint32) &groupPtr,sourceAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryAdd()");
    return L7_FAILURE;
  }
   if ((sourcePtr=snoopPTinProxySourceEntryFind((L7_uint32) &groupPtr,sourceAddr,L7_MATCH_EXACT))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryFind()");
    return L7_FAILURE;
  }

  if (groupPtr->source==L7_NULLPTR)
  {
    groupPtr->source=sourcePtr;
  }
  else
  {
    groupPtr->source->nextSource=sourcePtr;
    sourcePtr->previousSource=groupPtr->source;
  }  
  
  groupPtr->numberOfSources++;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordGroupFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, snoopPTinProxyGroup_t*  groupPtr )
{  
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;

  groupPtr=L7_NULLPTR;

  if ((interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,groupAddr,recordType,L7_MATCH_EXACT))!=L7_NULLPTR)
    {    
      return L7_SUCCESS;
    }        
  }
  return L7_FAILURE;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordSourceListAdd( L7_uint32  vlanId, L7_inet_addr_t *groupAddr, L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, L7_uint32 sourceCnt,snoopPTinProxyGroup_t *groupPtr)
{  
  L7_uint32 i;
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;
  snoopPTinProxySource_t*    sourcePtr=L7_NULLPTR;
  L7_inet_addr_t   *sourceAddrTmp=L7_NULLPTR;

    /* Argument validation */
  if (groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR /*sourceCnt==0*/)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (snoopPTinGroupRecordInterfaceAdd(vlanId,interfacePtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
    return L7_FAILURE;
  }

  if (snoopPTinGroupRecordGroupAdd( interfacePtr,recordType,groupAddr,groupPtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
    return L7_FAILURE;
  }
  sourceAddrTmp=sourceAddr;
  for (i=0;i<sourceCnt && sourceAddrTmp!=L7_NULLPTR;i++)
  {    
    if (snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddrTmp,sourcePtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");
      return L7_FAILURE;
    }
    sourceAddrTmp++;      
  }

  return L7_SUCCESS;
  
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupDeleteAll(snoopPTinProxyInterface_t* interfacePtr,L7_inet_addr_t* groupAddr)
{
  L7_uint        i;  

  snoopPTinProxyGroup_t*     groupPtr;

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
    
  for (i=0;i<MGMD_GROUP_REPORT_TYPE_MAX;i++)
  {
    if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,groupAddr,i,L7_MATCH_EXACT))!=L7_NULLPTR)
    {
      if (snoopPTinGroupRecordDelete(interfacePtr,groupAddr,i)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordDelete()");
        return L7_FAILURE;
      }
    }
  } 
  return L7_SUCCESS;    
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinInterfaceDelete(snoopPTinProxyInterface_t* interfacePtr)
{  
  snoopPTinProxyGroup_t*    groupPtrTmp;

   /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
  
  groupPtrTmp=interfacePtr->groupRecord;
  

  while(groupPtrTmp!=L7_NULLPTR)
  {
    if  (snoopPTinGroupDeleteAll(interfacePtr,&groupPtrTmp->key.groupAddr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupDeleteAll()");
      return L7_FAILURE;

    }
  }  
        

  if(snoopPTinProxyInterfaceEntryDelete(interfacePtr->key.vlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceEntryDelete()");      
    return L7_FAILURE;
  }  
  
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordDelete(snoopPTinProxyInterface_t* interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType)
{  
  snoopPTinProxyGroup_t*    groupPtr;
  snoopPTinProxyGroup_t*    groupPtrTmp;
  L7_BOOL                   isRunning=L7_FALSE;

   /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
   
  if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,groupAddr,recordType,L7_MATCH_EXACT))==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryFind()");
    return L7_SUCCESS;
  }


  if (snoopPTinGroupRecordSourceDeleteAll(groupPtr)!= L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDeleteAll()");
    return L7_FAILURE;
  }
  isRunning=snoop_ptin_proxytimer_isRunning(&groupPtr->timer);
  if (isRunning)
  {
     if (snoop_ptin_proxytimer_stop(&groupPtr->timer)!=L7_SUCCESS)
     {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoop_ptin_proxytimer_stop()");
       return L7_FAILURE;
     }
  }

  groupPtrTmp=interfacePtr->groupRecord;
  while (groupPtrTmp !=L7_NULLPTR)
  {
    if (&groupPtrTmp ==&groupPtr)
    {    
      groupPtrTmp->previousGroupRecord->nextGroupRecord=groupPtrTmp->nextGroupRecord;
      break;
    }
    groupPtrTmp=groupPtrTmp->nextGroupRecord;
  }   
  
  if(snoopPTinProxyGroupEntryDelete((L7_uint32) &interfacePtr,groupAddr,recordType)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryDelete()");      
    return L7_FAILURE;
  }

  --interfacePtr->numberOfGroupRecords;
  
  return L7_SUCCESS;
}


/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordSourceDeleteAll(snoopPTinProxyGroup_t*   groupPtr)
{  
  L7_uint32      i;
  
  snoopPTinProxySource_t*    sourcePtr;

   /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
   
  if (groupPtr->source == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Group Record Empty");
    groupPtr->numberOfSources=0;
    return L7_SUCCESS;
  }
  sourcePtr=groupPtr->source;

  for (i=0;i<groupPtr->numberOfSources && sourcePtr!=L7_NULLPTR;i++)
  {
    if (snoopPTinGroupRecordSourceDelete(groupPtr, &sourcePtr->key.sourceAddr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceDelete()");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Adds a new source to the given source list in the first free
 *          index
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   newSourceIdx  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordSourceDelete(snoopPTinProxyGroup_t*   groupPtr, L7_inet_addr_t *sourceAddr)
{ 
  
  snoopPTinProxySource_t*    sourcePtr;
  snoopPTinProxySource_t*    sourcePtrTmp;

   /* Argument validation */
  if (groupPtr == L7_NULLPTR /*|| sourceAddr==L7_NULLPTR*/)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
    
  if ((sourcePtr=snoopPTinProxySourceEntryFind((L7_uint32) &groupPtr,sourceAddr,L7_MATCH_EXACT))==L7_NULLPTR)
  {
     LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryFind()");
     return L7_SUCCESS;
  }

  sourcePtrTmp=groupPtr->source;
  while (sourcePtrTmp !=L7_NULLPTR)
  {
    if (&sourcePtrTmp ==&sourcePtr)
    {    
      sourcePtrTmp->previousSource->nextSource=sourcePtrTmp->nextSource;
      break;
    }
    sourcePtrTmp=sourcePtrTmp->nextSource;
  }     

  if(snoopPTinProxySourceEntryDelete((L7_uint32) &groupPtr,sourceAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryDelete()");      
    return L7_FAILURE;
  }

  --groupPtr->numberOfSources;

  return L7_SUCCESS;        
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type ALLOW_NEW_SOURCES
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
L7_RC_t snoopPTinGeneralQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr,L7_uint32 *noOfRecordsPtr, L7_uint32 *timeout)
{
//char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  snoopPTinProxyInterface_t *interfacePtr=L7_NULLPTR;
  L7_uint32   timeLeft;    
  L7_BOOL     pendingReport=L7_FALSE; 


//Initialize Output Variables
//*sendReport=L7_TRUE;
//groupPtr=L7_NULLPTR;
//*timeout=selectedDelay;
//*noOfRecords=0;

 /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }


  if(snoopPTinPendingReport2GeneralQuery(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &pendingReport, &timeLeft, interfacePtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GeneralQuery()");
    return L7_FAILURE;
  }
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {
/*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);               
    *sendReport=L7_FALSE; 
    return L7_SUCCESS;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }

 /*     2. If the received Query is a General Query, the interface timer is
used to schedule a response to the General Query after the
selected delay. Any previously pending response to a General
Query is canceled.*/  

  if (snoopPTinBuildCSR(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,groupPtr, noOfRecordsPtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinBuildCSR()");
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
}




/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type ALLOW_NEW_SOURCES
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
L7_RC_t snoopPTinGroupSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr, L7_uint32 *timeout)
{
  snoopPTinProxyInterface_t *interfacePtr=L7_NULLPTR;  
  L7_uint32   timeLeft;
  L7_BOOL     pendingReport=L7_FALSE;    

//char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

//Initialize Output Variables
  *sendReport=L7_FALSE;
  groupPtr=L7_NULLPTR;
  *timeout=selectedDelay;

    /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  if(snoopPTinPendingReport2GeneralQuery(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &pendingReport, &timeLeft, interfacePtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GeneralQuery()");
    return L7_FAILURE;
  }
  /*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {

    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);                  
    return L7_SUCCESS;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }


  if(snoopPTinPendingReport2GroupQuery(avlTreeEntry, interfacePtr, &pendingReport, &timeLeft, groupPtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GroupQuery()");
    return L7_FAILURE;
  }

/*     3. If the received Query is a Group-Specific Query or a Group-and-Source-Specific Query and there is no pending response to a
    previous Query for this group, then the group timer is used to
    schedule a report. If the received Query is a Group-and-Source-Specific Query, the list of queried sources is recorded to be used
    when generating a response.*/ 
  if (pendingReport==L7_FALSE)
  {
    *sendReport=L7_TRUE;
    return L7_SUCCESS;
  }

  /*     4. If there already is a pending response to a previous Query
scheduled for this group, and either the new Query is a Group-Specific Query or the recorded source-list associated with the
group is empty, then the group source-list is cleared and a single
response is scheduled using the group timer. The new response is
scheduled to be sent at the earliest of the remaining time for the
pending report and the selected delay.*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {
    if (groupPtr->numberOfSources==0)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a Group Specific Query with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);     
      *sendReport=L7_FALSE;
      groupPtr=L7_NULLPTR;
      return L7_SUCCESS;
    }
    else
    {
      *timeout=selectedDelay;
      *sendReport=L7_TRUE;
    }
  }
    
  
  //if (snoopPTinReportSchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, SNOOP_PTIN_GROUP_SPECIFIC_QUERY ,selectedDelay, L7_FALSE )!=L7_SUCCESS)
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinGroupDeleteAll()");
//  return L7_FAILURE;
//}
  *sendReport=L7_TRUE;
  return L7_SUCCESS;
}


/*************************************************************************
 * @purpose Processes a Source Specific Membership Report with record
 *          type ALLOW_NEW_SOURCES
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
L7_RC_t snoopPTinGroupSourceSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList, L7_uint32 selectedDelay, L7_BOOL *sendReport, snoopPTinProxyGroup_t* groupPtr, L7_uint32 *timeout)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  snoopPTinProxyInterface_t *interfacePtr=L7_NULLPTR;
  snoopPTinProxySource_t *sourcePtr=L7_NULLPTR;  
  L7_uint32   timeLeft;    
  L7_BOOL     pendingReport=L7_FALSE;
  

//Initialize Output Variables
  groupPtr=L7_NULLPTR;
  *sendReport=L7_FALSE;
  *timeout=selectedDelay;


  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
    
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }
  
 /*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {

    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);                  
    *sendReport=L7_FALSE;
    return L7_SUCCESS;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }


  if(snoopPTinPendingReport2GroupQuery(avlTreeEntry, interfacePtr, &pendingReport, &timeLeft, groupPtr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GroupQuery()");
    return L7_FAILURE;
  }


  /*     4. If there already is a pending response to a previous Query
scheduled for this group, and either the new Query is a Group-Specific Query or the recorded source-list associated with the
group is empty, then the group source-list is cleared and a single
response is scheduled using the group timer. The new response is
scheduled to be sent at the earliest of the remaining time for the
pending report and the selected delay.*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {
    if (groupPtr->numberOfSources==0)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a Group Specific Query with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);     
      *timeout=timeLeft;
    }
    else
    {
      *timeout=selectedDelay;      
    }
  }

  while (noOfSources > 0)
  {
    L7_uint8  sourceIdx = 0;
    L7_inet_addr_t sourceAddr;   

    inetAddressSet(L7_AF_INET, *sourceList,&sourceAddr);

    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[rootIntIdx].sources, sourceAddr, &sourceIdx))
    {      
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Inexisting Source %s on idx %u", inetAddrPrint(&sourceAddr, debug_buf), sourceIdx);           
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&sourceAddr, debug_buf), sourceIdx);           
      *sendReport=L7_TRUE;
        
      if ((sourcePtr=snoopPTinProxySourceEntryFind((L7_uint32) &groupPtr,&sourceAddr,L7_MATCH_EXACT))==L7_NULLPTR)
      {
        snoopPTinGroupRecordSourcedAdd(groupPtr,&sourceAddr, sourcePtr);
        
      }
      else
      {
        sourcePtr->retransmissions=sourcePtr->robustnessVariable;
      }                        
    }
  }
  
  if (sendReport==L7_FALSE)
  {
    /*Should we give a pointer*/
    snoopPTinGroupRecordDelete(interfacePtr, &groupPtr->key.groupAddr,groupPtr->key.recordType);
  }
  return L7_SUCCESS;
}


/******************************************************
 

******************************************************/
L7_RC_t snoopPTinPendingReport2GeneralQuery(L7_uint32 vlanId, L7_BOOL* pendingReport, L7_uint32* timeout, snoopPTinProxyInterface_t* interfacePtr)
{  
  
//Initialize Output Variables
  *timeout=0xFFFFFFFF;
  *pendingReport=L7_FALSE;
  interfacePtr=L7_NULLPTR;

  if ((interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,  L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    *pendingReport=snoop_ptin_proxytimer_isRunning(&interfacePtr->timer);
    if (pendingReport)
      *timeout=snoop_ptin_proxytimer_timeleft(&interfacePtr->timer);       
  }
  else
  {
    if(snoopPTinProxyInterfaceEntryAdd(vlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopPTinProxyInterfaceEntryAdd()");
      return L7_FAILURE;
    }
    if ((interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,  L7_MATCH_EXACT))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopPTinProxyInterfaceEntryFind()");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/******************************************************
 

******************************************************/
L7_RC_t snoopPTinPendingReport2GroupQuery(snoopPTinL3InfoData_t* avlTreeEntry, snoopPTinProxyInterface_t* interfacePtr, L7_BOOL *pendingReport, L7_uint32 *timeout, snoopPTinProxyGroup_t* groupPtr)
{  
  
//Initialize Output Variables
  *timeout=0xFFFFFFFF;
  *pendingReport=L7_FALSE;
  groupPtr=L7_NULLPTR;

  L7_uint8 recordType;

   /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR ||  interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    recordType=L7_IGMP_MODE_IS_INCLUDE;
#if 0 //Should we remove any pending reports with a different filter here?
    
    if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,L7_IGMP_MODE_IS_EXCLUDE, L7_MATCH_EXACT))!=L7_NULLPTR)
    {
      if (snoopPTinGroupRecordDelete(interfacePtr, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,L7_IGMP_MODE_IS_EXCLUDE)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopPTinGroupRecordDelete()");
        return L7_FAILURE;
      }
     }      
#endif    
  }
  else
  {
    recordType=L7_IGMP_MODE_IS_EXCLUDE;
#if 0 //Should we remove any pending reports with a different filter here?    
    if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,L7_IGMP_MODE_IS_INCLUDE, L7_MATCH_EXACT))!=L7_NULLPTR)
    {
      if (snoopPTinGroupRecordDelete(interfacePtr, &avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,L7_IGMP_MODE_IS_INCLUDE)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopPTinGroupRecordDelete()");
        return L7_FAILURE;
      }
    }    
#endif
  }  
  if ((groupPtr=snoopPTinProxyGroupEntryFind((L7_uint32) &interfacePtr,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType, L7_MATCH_EXACT))==L7_NULLPTR)                 
  {         
    if (snoopPTinGroupRecordGroupAdd(interfacePtr,recordType,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,groupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinGroupRecordGroupAdd()");
      return L7_FAILURE;
    }
  }
  else
  { 
    *pendingReport=snoop_ptin_proxytimer_isRunning(&groupPtr->timer);
    if (*pendingReport)
    {
      *timeout=snoop_ptin_proxytimer_timeleft(&groupPtr->timer);            
    }    
  }

  return L7_SUCCESS;
}

L7_RC_t snoopPTinBuildCSR(L7_uint32 vlanId,snoopPTinProxyGroup_t *groupPtr, L7_uint32 *noOfRecordsPtr)
{
  snoopPTinL3InfoData_t groupList[L7_MAX_GROUP_REGISTRATION_ENTRIES],*groupListPtr=L7_NULLPTR;
  L7_uint noOfGroups=L7_MAX_GROUP_REGISTRATION_ENTRIES;

  L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
  L7_uint8            sources2ReportCnt=0; 

  L7_uint32 i,j,noOfRecords=0;
  L7_uint8 recordType=L7_IGMP_MODE_IS_EXCLUDE;  

  snoop_eb_t         *pSnoopEB;
  

  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return L7_FAILURE;
  }

  avlGetAllElements(&pSnoopEB->snoopL3AvlTree, &groupList, &noOfGroups);

  *groupListPtr=groupList[0];
  for (i=0;i<noOfGroups && groupListPtr !=L7_NULLPTR;i++)
  {
    if(groupListPtr->snoopPTinL3InfoDataKey.vlanId==vlanId && groupListPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_TRUE)
    {
      if (groupListPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE)
      {
        recordType=L7_IGMP_MODE_IS_INCLUDE;        
      }
      else
      {
        recordType=L7_IGMP_MODE_IS_EXCLUDE;
      }
      for(j=0;j<groupListPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources;j++)
      {
        if (groupListPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[j].status==PTIN_SNOOP_SOURCESTATE_ACTIVE)
        {          
          sources2Report[sources2ReportCnt++]=groupListPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[j].sourceAddr;            
        }
      }
      if ( sources2ReportCnt>0)
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(vlanId,&groupListPtr->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt, groupPtr))
        {        
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
          return L7_FAILURE;        
        }
        sources2ReportCnt=0;
        noOfRecords++;
        
      }
                
    }
  groupListPtr++;    
  }  

  *noOfRecordsPtr=noOfRecords;
  return L7_SUCCESS;       
}

static L7_RC_t snoopPTinAddStaticSource(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_uint32 noOfSources,L7_inet_addr_t* sourceAddrList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{    
 char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

//L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_RECORD];
//L7_uint8            sources2ReportCnt=0;
//L7_uint8            recordType=L7_IGMP_ALLOW_NEW_SOURCES;
  
  L7_uint32           noOfRecords=0;  
  L7_inet_addr_t*     sourceAddr;

  snoopPTinProxySource_t* sourcePtr=L7_NULLPTR;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceAddrList == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }  
 
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  sourceAddr=sourceAddrList;
  while (noOfSources > 0 && sourceAddr!=L7_NULLPTR)
  {
    L7_uint8  sourceIdx = 0;
   

    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, *sourceAddr, &sourceIdx))
    {

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], *sourceAddr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }           
      if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
//      sources2Report[sources2ReportCnt++]=*sourceAddr;        
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr,sourcePtr ))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
          return L7_FAILURE;
        }
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s on idx %u", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);           
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);
    }

    /*Set Source as static*/
    avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].isStatic=L7_FALSE;

    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
        return L7_FAILURE;
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing clientIdx %u", clientIdx);
    }

    --noOfSources;
    sourceAddr++;
  }

//if (sources2ReportCnt>0)
//{
//  if (L7_SUCCESS != snoopPTinGroupRecordSourceListAdd(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType,sources2Report,sources2ReportCnt,groupPtr))
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
//    return L7_FAILURE;
//  }
//  noOfRecords++;
//}
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

L7_RC_t snoopPTinAddStaticGroup(L7_uint32 vlanId, L7_uint32 intIfNum,L7_inet_addr_t *groupAddr,L7_uint32 noOfSources,L7_inet_addr_t* sourceAddr)
{
  L7_uint32 clientIdx;
  snoopPTinL3InfoData_t  *snoopEntry; 

  L7_RC_t rc=L7_SUCCESS;

  L7_uint32 recordType=L7_IGMP_ALLOW_NEW_SOURCES;

  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;
  snoopPTinProxyGroup_t *groupPtr=L7_NULLPTR;

  L7_uint32 noOfRecords=1;

  /* Create new entry in AVL tree for VLAN+IP if necessary */
  if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT)))
  {
    if (L7_SUCCESS != snoopPTinL3EntryAdd( vlanId,groupAddr))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add L3 Entry");
      return L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinL3EntryAdd(%u,%u)",*groupAddr,vlanId);
    }
    if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT)))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add&Find L3 Entry");
      return L7_FAILURE;
    }
  }

 /* If Leaf interface is not used, initialize it */
  if (snoopEntry->interfaces[intIfNum].active == L7_FALSE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing interface idx: %u", intIfNum);
    snoopPTinInitializeInterface(&snoopEntry->interfaces[intIfNum],vlanId,groupAddr,intIfNum);            
  }
  /*Set Group as static*/
  snoopEntry->interfaces[intIfNum].isStatic=L7_TRUE;
  
  /* If root interface is not used, initialize it */
  if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active == L7_FALSE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing root interface idx: %u", SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);
    snoopPTinInitializeInterface(&snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM],vlanId,groupAddr,intIfNum);    
  }
  
   /*Set Group as static*/
  snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic=L7_TRUE;

  if ( (interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId, L7_MATCH_EXACT)) ==L7_NULLPTR)
  {
    if ((rc=snoopPTinGroupRecordInterfaceAdd(vlanId,interfacePtr))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
      return L7_FAILURE;
    }                                   
  }
  if (noOfSources==0)
  {
    recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;    
  }   
  if((rc=snoopPTinGroupRecordGroupAdd(interfacePtr,recordType,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr ,groupPtr))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
    return L7_FAILURE;
  }    

  clientIdx=PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1;
  

  rc=snoopPTinAddStaticSource(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM,intIfNum ,noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: Failed to snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }

  rc=snoopPTinAddStaticSource(snoopEntry, intIfNum, clientIdx, noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf interface: Failed to  snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }

  if (noOfSources==0 || noOfRecords>0)
  {
    if (snoopPTinReportSchedule(vlanId,groupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,noOfRecords, groupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }
  
  return rc;

}


#if 0
L7_RC_t snoopPTinRemoveStaticGroup(L7_uint32 vlanId, L7_uint32 intIfNum,L7_inet_addr_t *groupAddr,L7_uint32 noOfSources,L7_inet_addr_t* sourceAddr)
{
  L7_uint32 clientIdx;
  snoopPTinL3InfoData_t  *snoopEntry; 

  L7_RC_t rc=L7_SUCCESS;

  L7_uint32 recordType=L7_IGMP_BLOCK_OLD_SOURCES;

  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;
  snoopPTinProxyGroup_t *groupPtr=L7_NULLPTR;

  L7_uint32 noOfRecords=1;

  /* Create new entry in AVL tree for VLAN+IP if necessary */
  if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT)))
  {
    if (L7_SUCCESS != snoopPTinL3EntryAdd( vlanId,groupAddr))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add L3 Entry");
      return L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinL3EntryAdd(%u,%u)",*groupAddr,vlanId);
    }
    if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT)))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add&Find L3 Entry");
      return L7_FAILURE;
    }
  }

 /* If Leaf interface is not used, initialize it */
  if (snoopEntry->interfaces[intIfNum].active == L7_FALSE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing interface idx: %u", intIfNum);
    snoopPTinInitializeInterface(&snoopEntry->interfaces[intIfNum],vlanId,groupAddr,intIfNum);            
  }
  /*Set Group as static*/
  snoopEntry->interfaces[intIfNum].isStatic=L7_TRUE;
  
  /* If root interface is not used, initialize it */
  if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active == L7_FALSE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing root interface idx: %u", SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);
    snoopPTinInitializeInterface(&snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM],vlanId,groupAddr,intIfNum);    
  }
  
   /*Set Group as static*/
  snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic=L7_TRUE;

  if ( (interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId, L7_MATCH_EXACT)) ==L7_NULLPTR)
  {
    if ((rc=snoopPTinGroupRecordInterfaceAdd(vlanId,interfacePtr))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
      return L7_FAILURE;
    }                                   
  }
  if (noOfSources==0)
  {
    recordType=L7_IGMP_MODE_IS_INCLUDE;    
  }   
  if((rc=snoopPTinGroupRecordGroupAdd(interfacePtr,recordType,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr ,groupPtr))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
    return L7_FAILURE;
  }    

  clientIdx=PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1;
  

  rc=snoopPTinAddStaticSource(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM,intIfNum ,noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: Failed to snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }

  rc=snoopPTinAddStaticSource(snoopEntry, intIfNum, clientIdx, noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf interface: Failed to  snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }

  if (noOfSources==0 || noOfRecords>0)
  {
    if (snoopPTinReportSchedule(vlanId,groupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,noOfRecords, groupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }
  
  return rc;

}

#endif
