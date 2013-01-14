/*
 * snooping_ptin_db.c
 *
 *  Created on: 23 de Jul de 2012
 *      Author: Daniel Figueira
 */

#include "snooping_ptin_db.h"

#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "comm_structs.h"

#include "ptin_debug.h"
#include "ptin_igmp.h"

/*********************************************************************
* Static Methods
*********************************************************************/
static void     snoopPTinSourceTimerExpired   (L7_uint32 arg1, L7_uint32 arg2);
static void     snoopPTinGroupTimerExpired    (L7_uint32 arg1);
static L7_RC_t  snoopPTinClientFind           (L7_uint32 *clientList, L7_uint32 clientIdx);
static L7_RC_t  snoopPTinClientAdd            (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);
static L7_RC_t  snoopPTinClientRemove         (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx);
static L7_RC_t  snoopPTinSourceFind           (snoopPTinL3Source_t *sourceList, L7_uint32 sourceAddr, L7_uint8* foundIdx);
static L7_RC_t  snoopPTinSourceAdd            (snoopPTinL3Interface_t *interfacePtr, L7_uint32 sourceAddr, L7_uint8* newSourceIdx);
static L7_RC_t  snoopPTinSourceRemove         (snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourceList, L7_uint8 *sourcesCnt);
static L7_RC_t  snoopPTinInterfaceRemove      (snoopPTinL3Interface_t *interfacePtr);


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

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  /* Count the number of sources this client has */
  for (i = 0, clientSourceCnt = 0; i < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++i)
  {
    if (snoopPTinClientFind(sourcePtr->clients, i))
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

  /* Add the client to this source */
  PTIN_SET_MASKBIT(sourcePtr->clients, clientIdx);
  ++sourcePtr->numberOfClients;

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
L7_RC_t snoopPTinClientRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint32 clientIdx)
{
  L7_uint32 i, clientSourceCnt;

  /* Count the number of sources this client has */
  for (i = 0, clientSourceCnt = 0; i < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++i)
  {
    if (snoopPTinClientFind(sourcePtr->clients, i))
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
  PTIN_UNSET_MASKBIT(interfacePtr->sources[i].clients, clientIdx);
  --sourcePtr->numberOfClients;

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

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  for (idx = 0; idx < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++idx)
  {
    if (L7_FALSE == interfacePtr->sources[idx].active)
    {
      interfacePtr->sources[idx].active = L7_TRUE;
      interfacePtr->sources[idx].sourceAddr = sourceAddr;
      ++interfacePtr->numberOfSources;
      if (newSourceIdx != L7_NULLPTR)
      {
        *newSourceIdx = idx;
      }

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

  for (idx = 0; idx < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++idx)
  {
    if (sourceList[idx].active && (sourceList[idx].sourceAddr == sourceAddr))
    {
      if (foundIdx != L7_NULLPTR)
      {
        *foundIdx = idx;
      }

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
 * @param   sourcesCnt          Pointer to the current number of sources
 *                              in the interface
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @note This method also removes every client associated with this source
 *
 *************************************************************************/
L7_RC_t snoopPTinSourceRemove(snoopPTinL3Interface_t *interfacePtr, snoopPTinL3Source_t *sourcePtr, L7_uint8 *sourcesCnt)
{
  L7_uint32 i, clientsCnt;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  clientsCnt = PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE/(sizeof(L7_uint32)*8);

  /* Remove clients associated with this source */
  for(i = 0; i < clientsCnt; ++i)
  {
    if (snoopPTinClientFind(sourcePtr->clients, i))
    {
      snoopPTinClientRemove(interfacePtr, sourcePtr, i);
    }
  }

  osapiTimerFree(sourcePtr->sourceTimer);
  memset(sourcePtr, 0x00, sizeof(snoopPTinL3Source_t));
  --(*sourcesCnt);

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
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  osapiTimerFree(interfacePtr->queryTimer);
  osapiTimerFree(interfacePtr->groupTimer);
  memset(interfacePtr, 0x00, sizeof(snoopPTinL3Interface_t));

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose This method is called when a source-timer expires
 *
 * @param   arg1  Pointer to group's interface structure
 *                (snoopPTinL3Interface_t *)
 * @param   arg2  Pointer to source associated with this timer
 *                (snoopPTinL3Source_t *)
 *
 * @see     RFC 3376 6.3
 *
 * @todo    When the filter-mode is EXCLUDE, are the clients removed?
 *
 *************************************************************************/
void snoopPTinSourceTimerExpired(L7_uint32 arg1, L7_uint32 arg2)
{
  snoopPTinL3Interface_t  *interfacePtr = (snoopPTinL3Interface_t *) arg1;
  snoopPTinL3Source_t     *sourcePtr = (snoopPTinL3Source_t *) arg2;
  char                    debug_buf[16];

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  /* Free group-timer pointer */
  osapiTimerFree(interfacePtr->groupTimer);
  interfacePtr->groupTimer = L7_NULLPTR;

  if (interfacePtr->filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    /* Remove source */
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing source %s", snoopPTinIPv4AddrPrint(sourcePtr->sourceAddr, debug_buf));
    snoopPTinSourceRemove(interfacePtr, sourcePtr, &interfacePtr->numberOfSources);

    /* If no more sources remain, remove group */
    if (interfacePtr->numberOfSources == 0)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group has no more sources, thus it is being removed.");
      snoopPTinInterfaceRemove(interfacePtr);
    }
  }
}

/*************************************************************************
 * @purpose This method is called when a group-timer expires
 *
 * @param   arg1  Pointer to group's interface structure
 *                (snoopPTinL3Interface_t *)
 *
 * @see     RFC 3376 6.2.2
 *
 *************************************************************************/
void snoopPTinGroupTimerExpired(L7_uint32 arg1)
{
  snoopPTinL3Interface_t  *interfacePtr = (snoopPTinL3Interface_t *) arg1;
  L7_uint16               i;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  /* Free group-timer pointer */
  osapiTimerFree(interfacePtr->groupTimer);
  interfacePtr->groupTimer = L7_NULLPTR;

  /* Group-timer triggers only affect groups in filter-mode EXCLUDE */
  if (interfacePtr->filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
  {
    /* Remove every source whose timer has expired */
    for (i = 0; i < sizeof(interfacePtr->sources); ++i)
    {
      if ((interfacePtr->sources[i].sourceTimer != L7_NULLPTR) && (interfacePtr->sources[i].sourceTimer->timer_running == 0))
      {
        snoopPTinSourceRemove(interfacePtr, &interfacePtr->sources[i], &interfacePtr->numberOfSources);
      }
    }

    /* If no sources remain, remove group. Otherwise, switch to filter-mode INCLUDE */
    if (interfacePtr->numberOfSources == 0)
    {
      snoopPTinInterfaceRemove(interfacePtr);
    }
    else
    {
      interfacePtr->filtermode = PTIN_SNOOP_FILTERMODE_INCLUDE;
    }
  }
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
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "");

  interfacePtr->active = L7_TRUE;
  interfacePtr->filtermode = PTIN_SNOOP_FILTERMODE_INCLUDE;
  memset(interfacePtr->sources, 0x00, sizeof(interfacePtr->sources));
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

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

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

    /* Set source-timer to GMI */
    osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
    avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
    osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx],
                  igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
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

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Set group-timer to GMI */
  osapiTimerFree(avlTreeEntry->interfaces[intIfNum].groupTimer);
  avlTreeEntry->interfaces[intIfNum].groupTimer = L7_NULLPTR;
  osapiTimerAdd((void *) snoopPTinGroupTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], L7_NULL, igmpCfg.querier.group_membership_interval * 1000,
                &avlTreeEntry->interfaces[intIfNum].groupTimer);

  /*
   * Start by marking every current source as inactive.
   *
   * Then, for each source in sourceList:
   *        New source    - Add
   *        Known source  - Re-mark as active
   *
   * Finally, remove all sources in sourceList still marked as inactive.
   */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    avlTreeEntry->interfaces[intIfNum].sources[i].active = L7_FALSE;
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
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].active = L7_TRUE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
    }

    /*
     * If filter-mode:
     *      INCLUDE - set source-timer to 0
     *              - remove client
     *      EXCLUDE - set source-timer to GMI
     *              - add client
     */
    if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
    {
      /* Set source-timer to 0 */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], L7_NULL, 0, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      /* Add client if it does not exist */
      if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
          return L7_FAILURE;
        }
      }
    }
    else if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
    {
      /* Set source-timer to GMI */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], L7_NULL, igmpCfg.querier.group_membership_interval * 1000,
                    &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      /* Remove client */
      if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientRemove(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is empty");
          return L7_FAILURE;
        }
      }
    }

    --noOfSources;
  }

  /* Remove every source still marked as inactive */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    if (!avlTreeEntry->interfaces[intIfNum].sources[i].active)
    {
      snoopPTinSourceRemove(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[i], &avlTreeEntry->interfaces[intIfNum].numberOfSources);
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
  L7_uint32           sourcesLowerEqualZero[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer lower or equal to LMQT
  L7_uint32           sourcesGreaterZero[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer greater than LMQT
  L7_uint16           sourcesLowerEqualZeroCnt = 0;
  L7_uint16           sourcesGreaterZeroCnt = 0;
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32           i;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /* Split current sources into two arrays, one for sources with timer > 0, another for sources with timer <= 0 */
  for (i = 0; i < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++i)
  {
    if (snoopPTinIsTimerRunning(avlTreeEntry->interfaces[intIfNum].sources[i].sourceTimer))
    {
      sourcesGreaterZero[sourcesGreaterZeroCnt] = avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr;
      ++sourcesGreaterZeroCnt;
    }
    else
    {
      sourcesLowerEqualZero[sourcesLowerEqualZeroCnt] = avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr;
      ++sourcesLowerEqualZeroCnt;
    }
  }

  /*
   * If filter-mode is EXCLUDE:
   *    - send Q(G)
   *    - send Q(G,S), where S = { all sources with timer greater than 0 }
   * else:
   *    - send Q(G,S), where S = { all sources }
   */
  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
  {
    /* Send Q(G) */
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_TRUE, L7_NULLPTR, 0);

    /* Send Q(G,S) with all sources whose timer is greater than 0 */
    if (sourcesGreaterZeroCnt > 0)
    {
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_TRUE, sourcesGreaterZero, sourcesGreaterZeroCnt);
    }
  }
  else
  {
    /* Send Q(G,S) with all sources whose timer is greater than 0 */
    if (sourcesGreaterZeroCnt > 0)
    {
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_TRUE, sourcesGreaterZero, sourcesGreaterZeroCnt);
    }
    /* Send Q(G,S) with all sources whose timer is lower or equal to 0 */
    if (sourcesLowerEqualZeroCnt > 0)
    {
      snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_FALSE, sourcesLowerEqualZero, sourcesLowerEqualZeroCnt);
    }
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

      /* Set source-timer to GMI */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx],
                    igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
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

      /* Set source timer to GMI */
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], L7_NULL, igmpCfg.querier.group_membership_interval * 1000,
                    &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
    }

    /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
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
  L7_uint32           sourcesLowerEqualZero[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer lower or equal to LMQT
  L7_uint32           sourcesGreaterZero[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer greater than LMQT
  L7_uint16           sourcesLowerEqualZeroCnt = 0;
  L7_uint16           sourcesGreaterZeroCnt = 0;
  char                debug_buf[46];
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32           i;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /*
   * Mark every current source as inactive.
   *
   * Then, for each source in sourceList:
   *        New source    - Add
   *        Known source  - Re-mark as active
   *
   * Finally, remove all sources in sourceList still marked as inactive.
   */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    avlTreeEntry->interfaces[intIfNum].sources[i].active = L7_FALSE;
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

      /* If filter-mode is (INCLUDE) or (EXCLUDE and source-timer is equal to 0), save this source. In the end of this method, all saved sources will be sent in a Q(G,S) */
      if(avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE || snoopPTinIsTimerRunning(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer))
      {
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer->time_count > 0)
        {
          sourcesGreaterZero[sourcesGreaterZeroCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
          ++sourcesGreaterZeroCnt;
        }
        else
        {
          sourcesLowerEqualZero[sourcesLowerEqualZeroCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
          ++sourcesLowerEqualZeroCnt;
        }
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

    /* If filter-mode is exclude, set source-timer to GMI */
    if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
    {
      /* Set source-timer to GMI */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx],
                    igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      /* Add client if it does not exist */
      if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
          return L7_FAILURE;
        }
      }
    }
    else
    {
      /* Set source-timer to 0 */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], 0,
                    &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      /* Remove client */
      if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientRemove(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is empty");
          return L7_FAILURE;
        }
      }
    }

    --noOfSources;
  }

  /*
   *  - Remove every source still marked as inactive
   *  - Set group-timer to GMI
   *  - Set filter-mode to EXCLUDE
   *  - Send Q(G,S), where S are all new sources
   */
  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    avlTreeEntry->interfaces[intIfNum].filtermode = PTIN_SNOOP_FILTERMODE_EXCLUDE;
  }

  /* Remove every source still marked as inactive */
  for (i = 0; i < sizeof(avlTreeEntry->interfaces[intIfNum].sources); ++i)
  {
    if (!avlTreeEntry->interfaces[intIfNum].sources[i].active)
    {
      snoopPTinSourceRemove(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[i], &avlTreeEntry->interfaces[intIfNum].numberOfSources);
    }
  }

  /* Set group-timer to GMI */
  osapiTimerFree(avlTreeEntry->interfaces[intIfNum].groupTimer);
  avlTreeEntry->interfaces[intIfNum].groupTimer = L7_NULLPTR;
  osapiTimerAdd((void *) snoopPTinGroupTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], L7_NULL, igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].groupTimer);

  /* Send a Q(G,S) according to RFC 3376 6.6.3.2 */
  if (sourcesGreaterZeroCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_TRUE, sourcesGreaterZero, sourcesGreaterZeroCnt);
  }
  if (sourcesLowerEqualZeroCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_FALSE, sourcesLowerEqualZero, sourcesLowerEqualZeroCnt);
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

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

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
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, ipv4Addr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf), sourceIdx);

      /* Set source-timer to GMI */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx],
                    igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      /* Add client if it does not exist */
      if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
        if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
          return L7_FAILURE;
        }
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

      /* Add new client */
      if (L7_SUCCESS != snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
        return L7_FAILURE;
      }

      /* Set source timer to GMI */
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], L7_NULL, igmpCfg.querier.group_membership_interval * 1000,
                    &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
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
  L7_uint32           sourcesLowerEqualLMQT[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer lower or equal to LMQT
  L7_uint32           sourcesGreaterLMQT[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]; //List of sources with source-timer greater than LMQT
  L7_uint16           sourcesLowerEqualLMQTCnt = 0;
  L7_uint16           sourcesGreaterLMQTCnt = 0;
  char                debug_buf[46];
  L7_uint8            sourceIdx;
  L7_uint32           ipv4Addr;
  ptin_IgmpProxyCfg_t igmpCfg;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Group Address: %s", snoopPTinIPv4AddrPrint(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, debug_buf));

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
          snoopPTinClientRemove(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx);
        }

        /* Add source to to the list of sources to query */
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf));
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer->time_count > (igmpCfg.querier.last_member_query_count * igmpCfg.querier.last_member_query_interval))
        {
          sourcesGreaterLMQT[sourcesGreaterLMQTCnt] = ipv4Addr;
          ++sourcesGreaterLMQTCnt;
        }
        else
        {
          sourcesLowerEqualLMQT[sourcesLowerEqualLMQTCnt] = ipv4Addr;
          ++sourcesLowerEqualLMQTCnt;
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
        if (!avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer || avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer->timer_running == 0)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf));
          sourcesLowerEqualLMQT[sourcesLowerEqualLMQTCnt] = ipv4Addr;
          ++sourcesLowerEqualLMQTCnt;
        }
      }
      else
      {
        /* Add new source */
        if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
          return L7_FAILURE;
        }

        /* Add source to querySourceList */
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", snoopPTinIPv4AddrPrint(ipv4Addr, debug_buf));
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer->time_count > (igmpCfg.querier.last_member_query_count * igmpCfg.querier.last_member_query_interval))
        {
          sourcesGreaterLMQT[sourcesGreaterLMQTCnt] = ipv4Addr;
          ++sourcesGreaterLMQTCnt;
        }
        else
        {
          sourcesLowerEqualLMQT[sourcesLowerEqualLMQTCnt] = ipv4Addr;
          ++sourcesLowerEqualLMQTCnt;
        }
      }

      /* Add this client to this source */
      if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
      {
        snoopPTinClientAdd(avlTreeEntry->interfaces, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx);
      }

      /* Set source-timer to GMI */
      osapiTimerFree(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer = L7_NULLPTR;
      osapiTimerAdd((void *) snoopPTinSourceTimerExpired, (L7_uint32) &avlTreeEntry->interfaces[intIfNum], (L7_uint32) &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx],
                    igmpCfg.querier.group_membership_interval * 1000, &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer);

      --noOfSources;
    }
  }

  /* Send both Group-Source Specific Query according to RFC 3376 6.6.3.2 */
  if (sourcesGreaterLMQTCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_TRUE, sourcesGreaterLMQT, sourcesGreaterLMQTCnt);
  }
  if (sourcesLowerEqualLMQTCnt > 0)
  {
    snoopPTinQuerySchedule(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr.addr.ipv4.s_addr, L7_FALSE, sourcesLowerEqualLMQT, sourcesLowerEqualLMQTCnt);
  }

  return L7_SUCCESS;
}
