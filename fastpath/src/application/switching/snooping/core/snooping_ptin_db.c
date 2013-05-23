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
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t *interfacePtr, L7_uint32 sourceAddr, L7_uint8* newSourceIdx)
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
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_uint32 sourceAddr, L7_uint8* foundIdx)
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
    if ((sourceList[idx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE) && (sourceList[idx].sourceAddr == sourceAddr))
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
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr)
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
 * @purpose Remove an interface entry, reseting all its fields and
 *          stopping the group and query timers
 *
 * @param   interfacePtr  Interface to remove
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoopPTinInterfaceRemove(snoopPTinL3Interface_t *interfacePtr)
{
  snoopPTinL3Source_t     *sourcePtr;
  L7_uint32               i;

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
void snoopPTinInitializeInterface(snoopPTinL3Interface_t *interfacePtr)
{
  /* Argument validation */
  if (interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return;
  }

  memset(interfacePtr, 0x00, sizeof(*interfacePtr));
  interfacePtr->active     = L7_TRUE;
  interfacePtr->filtermode = PTIN_SNOOP_FILTERMODE_INCLUDE;
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
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
  
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

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
    L7_uint32 ipv4Addr;

    /* Search for this source in the current source list */
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);
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
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint16           i;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

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
    L7_uint32       ipv4Addr;

    /* Search for this source in the current source list */
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      /* Mark source as active */
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status = PTIN_SNOOP_SOURCESTATE_ACTIVE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
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
    }
  }

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
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  L7_uint32           sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer active
  L7_uint16           sources2QueryCnt = 0;  
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

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
    L7_uint32 ipv4Addr;

    /* Search for this source in the current source list */
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);     

      /* Enqueue sources 2 Query with timer > 0 */
      if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_TRUE)      
      {
        sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
        ++sources2QueryCnt;      
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
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

    --noOfSources;
  }
 
  /* send Q(G,A-B) */
  snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);

  /* If filter-mode is EXCLUDE: send Q(G) */
  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
  {      
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, L7_NULLPTR, 0);
  }

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
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  L7_uint32           sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources 2 Query
  L7_uint16           sources2QueryCnt = 0;
  
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32           i;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
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
    L7_uint32           ipv4Addr;

    /* Search for this source in the current source list */
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);

      /* Restore source status */
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status = PTIN_SNOOP_SOURCESTATE_ACTIVE;
      
      /* If filter-mode is (INCLUDE) or (EXCLUDE and source-timer is equal to 0), save this source. In the end of this method, all saved sources will be sent in a Q(G,S) */
      if(avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE || avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_TRUE)
      {
        sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
        ++sources2QueryCnt;    
      }
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
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
    }
  }

  /* Set group-timer to GMI */
  if (L7_SUCCESS != snoop_ptin_grouptimer_start(&avlTreeEntry->interfaces[intIfNum].groupTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum))
  {
     LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start grouptimer");
     return L7_FAILURE;
  }

  /* Send a Q(G,S)*/
  if (sources2QueryCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);
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
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  while (noOfSources > 0)
  {
    L7_uint8  sourceIdx = 0;
    L7_uint32 ipv4Addr;

    /* Search for this source in the current source list */
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s on idx %u", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);
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
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t *avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_uchar8 **sourceList)
{
  L7_uint32           sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources 2 Query
  L7_uint16           sources2QueryCnt = 0;
  char                debug_buf[46];
  L7_uint8            sourceIdx;
  L7_uint32           ipv4Addr;
  ptin_IgmpProxyCfg_t igmpCfg;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    while (noOfSources > 0)
    {
      /* Search for this source in the current source list */
      SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
      {
        /* Remove this client from this source */
        if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
        {
          snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx);
        }

        /* Add source to to the list of sources to query */
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf));
        if (snoop_ptin_sourcetimer_timeleft(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer) > (igmpCfg.querier.last_member_query_count * igmpCfg.querier.last_member_query_interval))
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
      SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
      {
        /* Add source to querySourceList if source-timer is not active */
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_FALSE)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf));
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
        else
        {
          L7_uint32 group_timer;

          /* Set source-timer to GT */
          group_timer = snoop_ptin_grouptimer_timeleft(&avlTreeEntry->interfaces[intIfNum].groupTimer);
          if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, group_timer, avlTreeEntry, intIfNum, sourceIdx))
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
            return L7_FAILURE;
          }
        }
      }

      --noOfSources;
    }  
  }

  /* Send a Q(G,S)*/
  if (sources2QueryCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, L7_TRUE, sources2Query, sources2QueryCnt);
  }

  return L7_SUCCESS;
}
