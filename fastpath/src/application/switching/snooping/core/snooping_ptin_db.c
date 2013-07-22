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

static L7_RC_t snoopPTinClientInterfaceAdd(snoopPTinL3Interface_t *interfacePtr, L7_uint32 clientIdx);

static L7_RC_t snoopPTinClientInterfaceRemove(snoopPTinL3Interface_t *interfacePtr, L7_uint32 clientIdx);

static L7_RC_t snoopPTinZeroClients(L7_uint32* clients);

static L7_RC_t snoopPTinClientNoSourcesSubscribed(snoopPTinL3Interface_t *interfacePtr, L7_uint32 clientIdx);

static snoopPTinProxyGroup_t* snoopPTinBuildCSR(snoopPTinProxyInterface_t *interfacePtr, L7_uint32 *noOfRecords);

static snoopPTinProxyInterface_t* snoopPTinPendingReport2GeneralQuery(L7_uint32 vlanId, L7_BOOL* pendingReport, L7_uint32* timeout);

static snoopPTinProxyGroup_t* snoopPTinPendingReport2GroupQuery(snoopPTinL3InfoData_t* avlTreeEntry, snoopPTinProxyInterface_t* interfacePtr, L7_BOOL *pendingReport, L7_uint32 *timeout);

static L7_RC_t snoopPTinAddStaticSource(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_uint32 noOfSources,L7_inet_addr_t* sourceAddrList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr);

static L7_RC_t snoopPTinRemoveStaticSource(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_uint32 noOfSources,L7_inet_addr_t* sourceAddrList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr);

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
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u found within this Client List",clientIdx);
    return L7_SUCCESS;
  }
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u not found within this Client List",clientIdx);
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
char  debug_buf[IPV6_DISP_ADDR_LEN]={};

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR || sourcePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

#if 0 //We are not able to forward the traffic based on the source address
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
#endif

  /* Add the client to this source*/
  if (L7_SUCCESS != snoopPTinClientFind(sourcePtr->clients, clientIdx))
  {
     PTIN_SET_MASKBIT(sourcePtr->clients, clientIdx);
     ++sourcePtr->numberOfClients;
     LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u added to this Source Addr %s, number of Clients: %u",clientIdx,inetAddrPrint(&sourcePtr->sourceAddr, debug_buf), sourcePtr->numberOfClients);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Existing ClientIdx:%u in this Source Addr %s, number of Clients: %u",clientIdx,inetAddrPrint(&sourcePtr->sourceAddr, debug_buf), sourcePtr->numberOfClients);
  }

  return L7_SUCCESS;
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
L7_RC_t snoopPTinClientInterfaceAdd(snoopPTinL3Interface_t *interfacePtr, L7_uint32 clientIdx)
{
  /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Add the client to this interface*/
  if (L7_SUCCESS != snoopPTinClientFind(interfacePtr->clients, clientIdx))
  {
     PTIN_SET_MASKBIT(interfacePtr->clients, clientIdx);
     ++interfacePtr->numberOfClients;     
     LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u added to this Interface, number of Clients: %u",clientIdx,interfacePtr->numberOfClients);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Existing ClientIdx:%u in this Interface, number of Clients: %u",clientIdx,interfacePtr->numberOfClients);
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
//L7_uint32 i, clientSourceCnt;
  char  debug_buf[IPV6_DISP_ADDR_LEN]={};

  /* Argument validation */
  if (sourcePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

#if 0 //We are not able to forward the traffic based on the source address
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
#endif

  /* Remove the client from this source */
  if (L7_SUCCESS == snoopPTinClientFind(sourcePtr->clients, clientIdx))
  {
     PTIN_UNSET_MASKBIT(sourcePtr->clients, clientIdx);
     --sourcePtr->numberOfClients;
     LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u removed from this Source Addr %s, number of Clients: %u",clientIdx,inetAddrPrint(&sourcePtr->sourceAddr, debug_buf), sourcePtr->numberOfClients);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Non existing ClientIdx:%u in this Source Addr %s, number of Clients: %u",clientIdx,inetAddrPrint(&sourcePtr->sourceAddr, debug_buf), sourcePtr->numberOfClients);
  }
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Removes a client from the given interface
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
L7_RC_t snoopPTinClientInterfaceRemove(snoopPTinL3Interface_t *interfacePtr, L7_uint32 clientIdx)
{

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Remove the client from this interface */
  if (L7_SUCCESS == snoopPTinClientFind(interfacePtr->clients, clientIdx))
  {
     PTIN_UNSET_MASKBIT(interfacePtr->clients, clientIdx);
     --interfacePtr->numberOfClients;
     LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx:%u removed from this Interface, number of Clients left: %u",clientIdx,interfacePtr->numberOfClients);
  }

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Verifies if this client has no sources subscribed 
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
L7_RC_t snoopPTinClientNoSourcesSubscribed(snoopPTinL3Interface_t* interfacePtr, L7_uint32 clientIdx)
{
  L7_uint32 i;

  /* Argument validation */
  if (interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }


  /* Count the number of sources this client has */
  for (i = 0; i < interfacePtr->numberOfSources; ++i)
  {
    if (interfacePtr->sources[i].numberOfClients>0)
    {
      if (snoopPTinClientFind(interfacePtr->sources[i].clients, clientIdx))
      {
        return L7_FAILURE;      
      }
    }
  }
  return L7_SUCCESS; 
}

/*************************************************************************
 * @purpose Verifies if is there any active client
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
L7_RC_t snoopPTinZeroClients(L7_uint32* clients)
{
  L7_uint32 clientIdx;
  /* Argument validation */
  if (clients == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  PTIN_NONZEROMASK(clients, clientIdx);
  if(clientIdx==-1)
  {
     return L7_SUCCESS;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing Clients clientIdx:%d",clientIdx);
    return L7_FAILURE;  
  }
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
L7_RC_t snoopPTinSourceAdd(snoopPTinL3Interface_t* interfacePtr, L7_inet_addr_t* sourceAddr, L7_uint8* newSourceIdx)
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
      memcpy(&interfacePtr->sources[idx].sourceAddr,sourceAddr, sizeof(L7_inet_addr_t));
      //interfacePtr->sources[idx].sourceAddr = *sourceAddr;
      *newSourceIdx                         = idx;
      ++interfacePtr->numberOfSources;      

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
L7_RC_t snoopPTinProxySourceAdd(snoopPTinL3InfoData_t* avlPtr,L7_uint32 clientIdx, L7_inet_addr_t* sourceAddr)
{
#if 0
  L7_uint8 sourceIdx;
  L7_RC_t rc=L7_SUCCESS;
  char      debug_buf[IPV6_DISP_ADDR_LEN]={};

  /* Argument validation */
  if (avlPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if (L7_SUCCESS == snoopPTinSourceFind(avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources, sourceAddr, &sourceIdx))
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);
    rc=L7_FAILURE;                
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(sourceAddr, debug_buf), clientIdx);

    /* Add new source */
    if ((L7_SUCCESS != snoopPTinSourceAdd(&avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM], sourceAddr, &sourceIdx)) /*& (L7_SUCCESS != snoopPTinProxyDBSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))*/)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
      return L7_FAILURE;
    }
  }


  /* Add client if it does not exist */
    if (L7_SUCCESS != snoopPTinClientFind(avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].clients, clientIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u", clientIdx);
      if (L7_SUCCESS != snoopPTinClientAdd(&avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM], &avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
        return L7_FAILURE;
      }
    }

    return rc;
#else
  if (clientIdx==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
#endif
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
L7_RC_t snoopPTinProxySourceRemove(snoopPTinL3InfoData_t* avlPtr,L7_uint32 clientIdx, L7_inet_addr_t* sourceAddr)
{
  L7_RC_t rc=L7_SUCCESS;
//L7_uint8 sourceIdx;
//
//char      debug_buf[IPV6_DISP_ADDR_LEN]={};
//
///* Argument validation */
//if (avlPtr == L7_NULLPTR )
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
//  return L7_FAILURE;
//}
//
//if (L7_SUCCESS == snoopPTinSourceFind(avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources, sourceAddr, &sourceIdx))
//{
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);
//
///* Remove client */
//  if (L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, clientIdx))
//  {
//    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing clientIdx %u", clientIdx);
//    if (L7_SUCCESS != snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
//    {
//      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to remove clientIdx %u", clientIdx);
//      return L7_FAILURE;
//    }
//  }
//}
//else
//{
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(sourceAddr, debug_buf), clientIdx);
//
//  /* Add new source */
//  if ((L7_SUCCESS != snoopPTinSourceAdd(&avlPtr->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM], sourceAddr, &sourceIdx)) /*& (L7_SUCCESS != snoopPTinProxyDBSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))*/)
//  {
//    LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
//    return L7_FAILURE;
//  }
//}
//
//
// snoopPTinSourceRemove(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[i]);
//
//

    return rc;

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
L7_RC_t snoopPTinSourceFind(snoopPTinL3Source_t *sourceList, L7_inet_addr_t* sourceAddr, L7_int8* foundIdx)
{
  L7_int16 idx;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};
  /* Argument validation */
  if (sourceList == L7_NULLPTR || foundIdx == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Input Source Address: %s", inetAddrPrint(sourceAddr, debug_buf));
  for (idx = 0; idx < PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP; ++idx)
  {    
    if (sourceList[idx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Existing Source Address: %s", inetAddrPrint(&sourceList[idx].sourceAddr, debug_buf));
    }
    if ((sourceList[idx].status != PTIN_SNOOP_SOURCESTATE_INACTIVE) && (sourceList[idx].sourceAddr.addr.ipv4.s_addr==sourceAddr->addr.ipv4.s_addr)) /*L7_INET_ADDR_COMPARE(&(sourceList[idx].sourceAddr),&sourceAddr)==0) */
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Found: Source Address: %s", inetAddrPrint(&sourceList[idx].sourceAddr, debug_buf));
      *foundIdx = idx;
      return L7_SUCCESS;
    }
  }
  *foundIdx=PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP+1;

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
  interfacePtr->isStatic   = L7_FALSE;
  interfacePtr->numberOfClients=0;

//if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM!=interfaceIdx)
//{
//
//}
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
    if ( (rootInterfacePtr=snoopPTinProxyInterfaceAdd(vlanId)) ==L7_NULLPTR)
    {      
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
      return L7_FAILURE;      
    }
     if (snoopPTinGroupRecordRemoveAll(rootInterfacePtr,mcastGroupAddr)!=L7_SUCCESS)
     {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupDeleteAll()");
      return L7_FAILURE;
     }
    if((rootGroupPtr=snoopPTinGroupRecordAdd(rootInterfacePtr,L7_IGMP_CHANGE_TO_INCLUDE_MODE,mcastGroupAddr) )==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (snoopPTinReportSchedule(vlanId,mcastGroupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,1,rootGroupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }
  
// memset(interfacePtr, 0x00, sizeof(*interfacePtr));

  memset(interfacePtr, 0x00, sizeof(*interfacePtr));
  
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
L7_RC_t snoopPTinMembershipReportIsIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  ptin_IgmpProxyCfg_t igmpCfg;

  L7_uint32           noOfRecords=0;
  L7_inet_addr_t* sourceAddr;
  L7_int8  sourceIdx = -1;   

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr== L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Root Interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Leaf interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  /* Add client if it does not exist */
  if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].clients, clientIdx))
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u added to interface %u", clientIdx,intIfNum);
    if (L7_SUCCESS != snoopPTinClientInterfaceAdd(&avlTreeEntry->interfaces[intIfNum], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Interface client list is full");
      return L7_FAILURE;
    }
  }
  
  sourceAddr=sourceList;
  /* Add new sources */
  while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
  {
    /* Search for this source in the current source list */  
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx) && sourceIdx!=-1)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(sourceAddr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s)", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
        if (noOfRecords==0)
          noOfRecords=1;                  
      }
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);    
    }

    /* Set source-timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */        
    if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
      return L7_FAILURE;
    }    

    --noOfSources;
    sourceAddr++;
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
L7_RC_t snoopPTinMembershipReportIsExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint16           i; 

  L7_uint32 noOfRecords=0;

  L7_int8        sourceIdx = 0;
  L7_inet_addr_t*  sourceAddr;
    
  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr== L7_NULLPTR )
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
  sourceAddr=sourceList;
  /* Add new sources */
  while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
  {
    /* Search for this source in the current source list */
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
    {
      /* Mark source as active */
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status = PTIN_SNOOP_SOURCESTATE_ACTIVE;
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }    
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %d", inetAddrPrint(sourceAddr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
        if (noOfRecords==0)
          noOfRecords=1;                   
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
      if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
        return L7_FAILURE;
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
      if (L7_SUCCESS != snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to remove clientIdx %u", clientIdx);
        return L7_FAILURE;
      }    
    }

    --noOfSources;
    sourceAddr++;
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

      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,&avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        if (noOfRecords==0)
          noOfRecords=1;                       
      }
    }
  }

  /* Remove client if it does exist */
  if ((L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].clients, clientIdx)) && L7_SUCCESS ==snoopPTinClientNoSourcesSubscribed(&avlTreeEntry->interfaces[intIfNum], clientIdx)) 
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx %u removed from this interface :%u", clientIdx,intIfNum );
    if (L7_SUCCESS != snoopPTinClientInterfaceRemove(&avlTreeEntry->interfaces[intIfNum], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to remove Client from Interface Client List");
      return L7_FAILURE;
    }
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
L7_RC_t snoopPTinMembershipReportToIncludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t      sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]={}; //List of sources with source-timer active
  
  L7_uint16           sources2QueryCnt = 0;    
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32 i,noOfRecords=0 ;

  L7_int8  sourceIdx = -1;
  L7_inet_addr_t* sourceAddr;  

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr== L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }


  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Root Interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Leaf Interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }


  if ( (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM) && (noOfSources==0) && (L7_SUCCESS==snoopPTinZeroClients(avlTreeEntry->interfaces[intIfNum].clients)))
  {                       
    groupPtr->numberOfSources=noOfSources;
    noOfRecords=1;
  }

   /* Add client if it does not exist */
  if (L7_SUCCESS != snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].clients, clientIdx))
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u added to interface %u", clientIdx,intIfNum);
    if (L7_SUCCESS != snoopPTinClientInterfaceAdd(&avlTreeEntry->interfaces[intIfNum], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Interface client list is full");
      return L7_FAILURE;
    }
  }

  sourceAddr=sourceList; 
  /* Add new sources */
  while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
  {   
    /* Search for this source in the current source list */    
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);            
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(sourceAddr, debug_buf), clientIdx);

      /* Add new source */
      if ((L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx)) /*& (L7_SUCCESS != snoopPTinProxyDBSourceAdd(&avlTreeEntry->interfaces[intIfNum], ipv4Addr, &sourceIdx))*/)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
        if (noOfRecords==0)
          noOfRecords=1;                           
      }     
    }

    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */       
    if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Client list for this multicast group is full");
      return L7_FAILURE;
    }    
     
    /*Add Source to Not Query List*/
    avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].status=PTIN_SNOOP_SOURCESTATE_TOREMOVE;  

    --noOfSources;
    sourceAddr++;
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
L7_RC_t snoopPTinMembershipReportToExcludeProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t           sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]={}; //List of sources 2 Query
  L7_uint16                sources2QueryCnt = 0;

  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  ptin_IgmpProxyCfg_t igmpCfg;
  L7_uint32           i,noOfRecords=0;

  L7_inet_addr_t*      sourceAddr;  

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr== L7_NULLPTR )
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
  sourceAddr=sourceList;
  while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
  {
    L7_int8            sourceIdx = -1;        

    /* Search for this source in the current source list */    
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);

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
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s with clientIdx %u", inetAddrPrint(sourceAddr, debug_buf), clientIdx);

      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
        if (noOfRecords==0)
             noOfRecords=1;                 
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

        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(sourceAddr, debug_buf));
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
    if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
      return L7_FAILURE;
    }    

    --noOfSources;
    sourceAddr++;
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
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,&avlTreeEntry->interfaces[intIfNum].sources[i].sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        if (noOfRecords==0)
             noOfRecords=1;        
      }
    
    }
  }

  /* Set group-timer to GMI */
  if (L7_SUCCESS != snoop_ptin_grouptimer_start(&avlTreeEntry->interfaces[intIfNum].groupTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum))
  {
     LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start grouptimer");
     return L7_FAILURE;
  }


  /* Remove client if it does exist */
  if ((L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].clients, clientIdx)) && L7_SUCCESS ==snoopPTinClientNoSourcesSubscribed(&avlTreeEntry->interfaces[intIfNum], clientIdx)) 
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx %u removed from this interface :%u", clientIdx,intIfNum );
    if (L7_SUCCESS != snoopPTinClientInterfaceRemove(&avlTreeEntry->interfaces[intIfNum], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to remove Client from Interface Client List");
      return L7_FAILURE;
    }
    if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM==intIfNum)
    {
      noOfRecords=1;
      snoopPTinGroupRecordSourceRemoveAll(groupPtr);
      groupPtr->key.recordType=L7_IGMP_CHANGE_TO_INCLUDE_MODE;
    }
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
    *noOfRecordsPtr=noOfRecords;
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
L7_RC_t snoopPTinMembershipReportAllowProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  ptin_IgmpProxyCfg_t igmpCfg;  
  
  L7_uint32           noOfRecords=0; 
  L7_inet_addr_t* sourceAddr;
  L7_int8  sourceIdx = -1;    
   

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr== L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }  
 
  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Root Interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Leaf Interface - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  
  if ( (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM) && (noOfSources==0) && (L7_SUCCESS==snoopPTinZeroClients(avlTreeEntry->interfaces[intIfNum].clients)))
  {                       
    groupPtr->key.recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;
    groupPtr->numberOfSources=noOfSources;
    noOfRecords=1;
  }

  /* Add client if it does not exist */  
  if (L7_SUCCESS != snoopPTinClientInterfaceAdd(&avlTreeEntry->interfaces[intIfNum], clientIdx))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Interface client list is full");
    return L7_FAILURE;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New clientIdx %u added to interface %u", clientIdx,intIfNum);
  }


  sourceAddr=sourceList;
  while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
  {       
    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx) && sourceIdx!=-1)
    {
      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }           
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
          return L7_FAILURE;
        }
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
        if (noOfRecords==0)
             noOfRecords=1;
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s on idx %u", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);           
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);
    }
    
    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr) && (L7_SUCCESS==snoopPTinZeroClients(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients)))
    {
      if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
      if (noOfRecords==0)                    
        noOfRecords=1;
    }

    /* Add client if it does not exist */    
    if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
      return L7_FAILURE;
    }
    
    --noOfSources;
    sourceAddr++;
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
L7_RC_t snoopPTinMembershipReportBlockProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_ushort16 noOfSources, L7_inet_addr_t* sourceList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{
  L7_inet_addr_t      sources2Query[PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP]={}; //List of sources 2 Query
  L7_uint16           sources2QueryCnt = 0;  

  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  L7_int8            sourceIdx=-1;
  L7_inet_addr_t*      sourceAddr;
  ptin_IgmpProxyCfg_t igmpCfg;  
  L7_uint32           noOfRecords=0;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR ||  groupPtr == L7_NULLPTR )
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
  
  if ( (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM) && (noOfSources==0) && (L7_SUCCESS==snoopPTinZeroClients(avlTreeEntry->interfaces[intIfNum].clients)))
  { 
    groupPtr->numberOfSources=noOfSources;
    noOfRecords=1;
  }  

  if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {    
    sourceAddr=sourceList;
    while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
    {
      /* Search for this source in the current source list */
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
      {
        /* Remove this client from this source */        
        if(L7_SUCCESS!=snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to remove Client clientIdx:%u",clientIdx);
          return L7_FAILURE;
        }          
        if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr) && (L7_SUCCESS==snoopPTinZeroClients(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients)))
        {
          if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
          if (noOfRecords==0)                    
            noOfRecords=1;
        }            
      
        /* Add source to to the list of sources to query */
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(sourceAddr, debug_buf));
        if (snoop_ptin_sourcetimer_timeleft(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer) >
            (igmpCfg.querier.last_member_query_count * SNOOP_MAXRESP_INTVL_ROUND(igmpCfg.querier.last_member_query_interval, SNOOP_IGMP_FP_DIVISOR)))
        {
          sources2Query[sources2QueryCnt]=*sourceAddr;
          ++sources2QueryCnt;
        }
      }      
      else if (sourceIdx==-1)
      {
        return L7_FAILURE;
      }    
      --noOfSources;
      sourceAddr++;
    }
  }
  else if (avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_EXCLUDE)
  {
    sourceAddr=sourceList;   
    while (noOfSources > 0 && sourceAddr !=L7_NULLPTR)
    {
      /* Search for this source in the current source list */      
      if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
      {
        /* Add source to querySourceList if source-timer is not active */
        if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer.isRunning == L7_FALSE)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Adding source %s to Q(G,S)", inetAddrPrint(sourceAddr, debug_buf));
          sources2Query[sources2QueryCnt] = avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceAddr;
          ++sources2QueryCnt;
        }
      }
      else if (sourceIdx==-1)
      {
        return L7_FAILURE;
      }
      else
      {
        /* Add new source */
        if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
          return L7_FAILURE;
        }       
        if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
        {
          if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Source Added to Group Record (vlanId:%u, groupAddr:%s recordType:%u sourceAddr:%s", groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,inetAddrPrint(sourceAddr, debug_buf2));   
          if (noOfRecords==0)                    
            noOfRecords=1;
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
      sourceAddr++;
    }  
  }

  /* Remove client if it does exist */
  if ((L7_SUCCESS == snoopPTinClientFind(avlTreeEntry->interfaces[intIfNum].clients, clientIdx)) && L7_SUCCESS ==snoopPTinClientNoSourcesSubscribed(&avlTreeEntry->interfaces[intIfNum], clientIdx)) 
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "ClientIdx %u removed from this interface :%u", clientIdx,intIfNum );
    if (L7_SUCCESS != snoopPTinClientInterfaceRemove(&avlTreeEntry->interfaces[intIfNum], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to remove Client");
      return L7_FAILURE;
    }
//  if(SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM==intIfNum)
//  {
//    noOfRecords=1;
//    snoopPTinGroupRecordSourceRemoveAll(groupPtr);
//    groupPtr->key.recordType=L7_IGMP_CHANGE_TO_INCLUDE_MODE;
//  }
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
    *noOfRecordsPtr=noOfRecords;
  }

  return L7_SUCCESS;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*MGMD Proxy*/


/*************************************************************************
 * @purpose Add a Proxy Interface
 *
 * @param   vlanId  Interface to which the source list belongs
 * @param   interfacePtr    Address of the new source
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
snoopPTinProxyInterface_t* snoopPTinProxyInterfaceAdd(L7_uint32 vlanId)
{   
snoopPTinProxyInterface_t* interfacePtr;


  if((interfacePtr=snoopPTinProxyInterfaceEntryAdd(vlanId))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceEntryAdd()");
    return L7_NULLPTR;
  }  
  
//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy Interface Added (vlanId:%u)",interfacePtr->key.vlanId);
  return interfacePtr;
}

/*************************************************************************
 * @purpose Add a Group Record to a Proxy Interface
 *
 * @param   interfacePtr  Interface to which the source list belongs
 * @param   recordType    Address of the new source
 * @param   groupAddr     Multicast Address of the Group
 *  @param   groupPtr     Multicast Address of the Group
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
snoopPTinProxyGroup_t* snoopPTinGroupRecordAdd(snoopPTinProxyInterface_t* interfacePtr,L7_uint8 recordType, L7_inet_addr_t* groupAddr)
{
  snoopPTinProxyGroup_t*  groupPtr,*groupPtrAux;
  L7_uint32 i;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};  


   /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  /*Let us check if this group record was already created*/
  if ((groupPtr=snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId,groupAddr,recordType,L7_MATCH_EXACT))!=L7_NULLPTR)
  {    
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Existing Group Record (vlanId:%u groupAddr:%s recordType:%u)",interfacePtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType);

    groupPtr->retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS;    
    groupPtr->robustnessVariable=PTIN_IGMP_DEFAULT_ROBUSTNESS;

    if (groupPtr->interfacePtr != interfacePtr)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Fixing interfacePtr");
      groupPtr->interfacePtr=interfacePtr;
    }   
  }
#if 0
  /*Let us check if this group record was previouly saved with a different record type*/
  else if ((groupPtr=snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId,groupAddr,MGMD_GROUP_REPORT_TYPE_MAX,L7_MATCH_EXACT))!=L7_NULLPTR)
  {    
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Re-using Group Record (vlanId:%u groupAddr:%s recordType:%u)",interfacePtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType);

    groupPtr->retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS;    
    groupPtr->robustnessVariable=PTIN_IGMP_DEFAULT_ROBUSTNESS;
    groupPtr->interfacePtr=interfacePtr;
    groupPtr->key.recordType=recordType;

    if(groupPtr->nextGroupRecord!=L7_NULLPTR || groupPtr->previousGroupRecord!=L7_NULLPTR)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Resetting SourcePtr->NextSrc & SourcePtr->PreviousSrc");
      groupPtr->nextGroupRecord=L7_NULLPTR;    
      groupPtr->previousGroupRecord=L7_NULLPTR;
    }
  }  
#endif
  else if((groupPtr=snoopPTinProxyGroupEntryAdd(interfacePtr,groupAddr,recordType))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryAdd()");
    return L7_NULLPTR;
  }            
  

  if ((groupPtrAux=interfacePtr->groupRecord)==L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "This is the first group record added to this interface");
    interfacePtr->groupRecord=groupPtr;
    interfacePtr->numberOfGroupRecords++; 
  }
  else
  {    
    for (i=0;i<interfacePtr->numberOfGroupRecords && groupPtrAux!=L7_NULLPTR;i++)
    {
      if(groupPtrAux->nextGroupRecord==L7_NULLPTR)
      {
        if (groupPtrAux!=groupPtr)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "There are %u group records in this interface (previous groupAddr:%s recordType:%u)",i+1,  inetAddrPrint(&groupPtrAux->key.groupAddr, debug_buf),groupPtrAux->key.recordType);
          groupPtr->previousGroupRecord=groupPtrAux;
          groupPtrAux->nextGroupRecord=groupPtr;
          interfacePtr->numberOfGroupRecords++; 
        }
        break;        
      }
      groupPtrAux=groupPtrAux->nextGroupRecord;
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record (vlanId: %u groupAddr: %s recordType: %u  noOfGroupRecords: %u)", groupPtr->interfacePtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf), groupPtr->key.recordType,  interfacePtr->numberOfGroupRecords);
  return groupPtr;
}


/*************************************************************************
 * @purpose Add a new Source Address to an existing Group Record
 *
 * @param   groupPtr  Interface to which the source list belongs
 * @param   sourceAddr    Address of the new source
 * @param   sourcePtr  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordSourcedAdd(snoopPTinProxyGroup_t* groupPtr,L7_inet_addr_t* sourceAddr)
{
  snoopPTinProxySource_t*  sourcePtr,*sourcePtrAux;
 
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
  L7_uint32 i;

#if 0
  L7_inet_addr_t groupAddrAux;
#endif

  L7_uint8 flagsourceAdded=0;

  /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

#if 0
  memset(&groupAddrAux, 0x00, sizeof(L7_inet_addr_t));
  groupAddrAux.family=groupPtr->key.groupAddr.family;
  inetAddressZeroSet(groupAddrAux.family, &groupAddrAux);  
#endif

  if ((sourcePtr=snoopPTinProxySourceEntryFind(&groupPtr->key.groupAddr,sourceAddr,L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Existing Source :%s, restoring retransmission variable",inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));    

    if  (sourcePtr->key.sourceAddr.addr.ipv4.s_addr!=sourcePtr->key.sourceAddr.addr.ipv4.s_addr)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "The Source Addr %s stored in the AVL Tree is different from Input Source Address %s ",inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
    }
    sourcePtr->retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS;    
    sourcePtr->retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS;
    if (sourcePtr->groupPtr != groupPtr)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Fixing groupPtr");
      sourcePtr->groupPtr=groupPtr;      
    }
  }
#if 0
  else if ((sourcePtr=snoopPTinProxySourceEntryFind(&groupAddrAux,sourceAddr,L7_MATCH_EXACT))!=L7_NULLPTR)
  {    
    memcpy(&sourcePtr->key.groupAddr,&groupPtr->key.groupAddr,sizeof(L7_inet_addr_t));
    sourcePtr->groupPtr=groupPtr;
    sourcePtr->retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS;    
    sourcePtr->robustnessVariable=PTIN_IGMP_DEFAULT_ROBUSTNESS;

    if(sourcePtr->nextSource!=L7_NULLPTR || sourcePtr->previousSource!=L7_NULLPTR)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Resetting SourcePtr->NextSrc & SourcePtr->PreviousSrc");
      sourcePtr->nextSource=L7_NULLPTR;    
      sourcePtr->previousSource=L7_NULLPTR;
    }
  }
#endif
  else if ((sourcePtr=snoopPTinProxySourceEntryAdd(groupPtr,sourceAddr))==L7_NULLPTR)  
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryFind()");
    return L7_FAILURE;
  }
//else
//{
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New Source Address added :%s)", inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf));
//}

  if ((sourcePtrAux=groupPtr->source)==L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "This is the first source address in this group record");
    groupPtr->source=sourcePtr;    
    
    groupPtr->numberOfSources++;
  }
  else
  {    
    for (i=0;i<groupPtr->numberOfSources && sourcePtrAux!=L7_NULLPTR;i++)
    {
      if(sourcePtrAux->nextSource==L7_NULLPTR)
      {
        if (sourcePtrAux!=sourcePtr)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "There are %u sources in this group record (previous sourceAddr:%s)",i+1,  inetAddrPrint(&sourcePtrAux->key.sourceAddr, debug_buf));
          sourcePtr->previousSource=sourcePtrAux;
          sourcePtrAux->nextSource=sourcePtr;
          groupPtr->numberOfSources++;
          flagsourceAdded=1;
        }
        break;        
      }
      sourcePtrAux=sourcePtrAux->nextSource;
    }
    if (i+1!=groupPtr->numberOfSources-flagsourceAdded)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Pointers are not properly configured %u<%u",i+1,groupPtr->numberOfSources-flagsourceAdded);
    }
  } 
  
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record (vlanId: %u groupAddr: %s groupPtr: %u recordType: %u, sourceAddr: %s noOfSources: %u)", groupPtr->interfacePtr->key.vlanId, inetAddrPrint(&sourcePtr->groupPtr->key.groupAddr, debug_buf),&groupPtr, groupPtr->key.recordType,  inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf2),groupPtr->numberOfSources);

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Find a Group Record
 *
 * @param   vlanId  Interface to which the source list belongs
 * @param   groupAddr    Address of the new source
 * @param   recordType  If source is successfully added, this will hold
 *                        the new source index in the source list. If
 *                        passed as L7_NULLPTR, it is not used
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE    Source list is full
 *
 *************************************************************************/
L7_RC_t snoopPTinGroupRecordFind(L7_uint32 vlanId,L7_inet_addr_t   *groupAddr,L7_uint8 recordType, snoopPTinProxyGroup_t*  groupPtr )
{  
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;

  groupPtr=L7_NULLPTR;

  if ((interfacePtr=snoopPTinProxyInterfaceEntryFind(vlanId,L7_MATCH_EXACT))!=L7_NULLPTR)
  {
    if ((groupPtr=snoopPTinProxyGroupEntryFind(vlanId,groupAddr,recordType,L7_MATCH_EXACT))!=L7_NULLPTR)
    {    
      return L7_SUCCESS;
    }        
  }
  return L7_FAILURE;
}

/*************************************************************************
 * @purpose Add a Group Record and a  Source  List
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
L7_RC_t snoopPTinGroupRecordAddSourceList( L7_uint32  vlanId, L7_inet_addr_t *groupAddr, L7_uint8 recordType, L7_inet_addr_t   *sourceAddr, L7_uint32 sourceCnt,snoopPTinProxyGroup_t *groupPtr)
{  
  L7_uint32 i;
  snoopPTinProxyInterface_t* interfacePtr=L7_NULLPTR;  
  L7_inet_addr_t   *sourceAddrTmp=L7_NULLPTR;

    /* Argument validation */
  if (groupAddr == L7_NULLPTR || sourceAddr == L7_NULLPTR || groupPtr== L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  if ((interfacePtr=snoopPTinProxyInterfaceAdd(vlanId))== L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
    return L7_FAILURE;
  }

  if ((groupPtr=snoopPTinGroupRecordAdd( interfacePtr,recordType,groupAddr))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
    return L7_FAILURE;
  }
  sourceAddrTmp=sourceAddr;
  for (i=0;i<sourceCnt && sourceAddrTmp!=L7_NULLPTR;i++)
  {    
    if (snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddrTmp)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");
      return L7_FAILURE;
    }
    sourceAddrTmp++;      
  }

  return L7_SUCCESS;
  
}

/*************************************************************************
 * @purpose Remove all group records associated with a Proxy Interface
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
L7_RC_t snoopPTinGroupRecordRemoveAll(snoopPTinProxyInterface_t* interfacePtr,L7_inet_addr_t* groupAddr)
{
  L7_uint        i;  
  
  /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
    
  for (i=0;i<MGMD_GROUP_REPORT_TYPE_MAX;i++)
  {    
    if (snoopPTinGroupRecordRemove(interfacePtr,groupAddr,i)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordDelete()");
      return L7_FAILURE;
    }    
  } 
  return L7_SUCCESS;    
}

/*************************************************************************
 * @purpose Remove a Proxy Interface 
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
L7_RC_t snoopPTinProxyInterfaceRemove(snoopPTinProxyInterface_t* interfacePtr)
{  
  snoopPTinProxyGroup_t*    groupPtrTmp;
  L7_uint32 vlanId;

   /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
  
  groupPtrTmp=interfacePtr->groupRecord;
  

  while(groupPtrTmp!=L7_NULLPTR)
  {
    if  (snoopPTinGroupRecordRemoveAll(interfacePtr,&groupPtrTmp->key.groupAddr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupDeleteAll()");
      return L7_FAILURE;

    }
  }  
        
  vlanId=interfacePtr->key.vlanId;
  if(snoopPTinProxyInterfaceEntryDelete(interfacePtr->key.vlanId)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceEntryDelete()");      
    return L7_FAILURE;
  } 
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy Interface Removed (vlanId:%u)",vlanId); 
  
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Remove a Group Record
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
L7_RC_t snoopPTinGroupRecordRemove(snoopPTinProxyInterface_t* interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType)
{  
  snoopPTinProxyGroup_t*    groupPtr;
  snoopPTinProxyGroup_t*    groupPtrAux;
  L7_BOOL                   isRunning=L7_FALSE;

  char                      debug_buf[IPV6_DISP_ADDR_LEN]={};

   /* Argument validation */
  if (interfacePtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
   
  if ((groupPtr=snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId,groupAddr,recordType,L7_MATCH_EXACT))==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryFind()");
    return L7_SUCCESS;
  }


  if (snoopPTinGroupRecordSourceRemoveAll(groupPtr)!= L7_SUCCESS)
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

  groupPtrAux=interfacePtr->groupRecord;
  while (groupPtrAux !=L7_NULLPTR)
  {
    if (groupPtrAux==groupPtr)
    { 
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Group record found within interface!");
      if(groupPtrAux->previousGroupRecord==L7_NULLPTR)/*First Element*/
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "First group record of this interface");
        interfacePtr->groupRecord=groupPtrAux->nextGroupRecord;
      }
      else
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "This is not the first group record of this interface");        
        groupPtrAux->previousGroupRecord->nextGroupRecord=groupPtrAux->nextGroupRecord;
      }      
      if(groupPtrAux->nextGroupRecord!=L7_NULLPTR)
        groupPtrAux->nextGroupRecord->previousGroupRecord=groupPtrAux->previousGroupRecord;              
      break;
    }
    groupPtrAux=groupPtrAux->nextGroupRecord;
  }   
  
  if (groupPtrAux==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Unable to find group record (groupAddr:%s recordType:%u) within interface (vlanId:%u)",inetAddrPrint(groupAddr, debug_buf),recordType,interfacePtr->key.vlanId);
    return L7_SUCCESS;
  }
  
  groupPtr->nextGroupRecord=L7_NULLPTR;
  groupPtr->previousGroupRecord=L7_NULLPTR;
  groupPtr->retransmissions=0;
  groupPtr->interfacePtr=L7_NULLPTR;

  
#if 1 //We do not remove the Group Entry
  if(snoopPTinProxyGroupEntryDelete(interfacePtr,groupAddr,recordType)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryDelete()");      
    return L7_FAILURE;
  }
//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record Removed (groupAddr:%s recordType:%u) from interface (vlanId:%u)",inetAddrPrint(groupAddr, debug_buf),recordType,interfacePtr->key.vlanId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record Removed");
#else //Instead we change the record type to MGMD_GROUP_REPORT_TYPE_MAX
  if(snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId,groupAddr,MGMD_GROUP_REPORT_TYPE_MAX,L7_MATCH_EXACT)==L7_NULLPTR)
  {    
    groupPtr->key.recordType=MGMD_GROUP_REPORT_TYPE_MAX;  
//Should we change also the VLAN ID to MAX VLANs?
//  groupPtr->key.vlanId=L7_MAX_VLANS;
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Group Record Saved (groupAddr:%s recordType:%u) from interface (vlanId:%u)",inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,groupPtr->key.vlanId);
  }
  else
  {
    if(snoopPTinProxyGroupEntryDelete(interfacePtr,groupAddr,recordType)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyGroupEntryDelete()");      
      return L7_FAILURE;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record Removed (groupAddr:%s recordType:%u) from interface (vlanId:%u)",inetAddrPrint(groupAddr, debug_buf),recordType,interfacePtr->key.vlanId);
  }
#endif
  

  --interfacePtr->numberOfGroupRecords;
  
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Remove all Sources from an existing Group Record
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
L7_RC_t snoopPTinGroupRecordSourceRemoveAll(snoopPTinProxyGroup_t*   groupPtr)
{  
//char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};

  L7_uint32      i;
  L7_BOOL flagSourceRemoved=L7_FALSE;
  
  snoopPTinProxySource_t*    sourcePtr,*sourcePtrAux;
  L7_inet_addr_t             groupAddrAux;

   /* Argument validation */
  if (groupPtr == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  memset(&groupAddrAux, 0x00, sizeof(L7_inet_addr_t));
  groupAddrAux.family=groupPtr->key.groupAddr.family;
  inetAddressZeroSet(groupAddrAux.family, &groupAddrAux);  
   
  if (groupPtr->source == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Group Record Empty");
    groupPtr->numberOfSources=0;
    return L7_SUCCESS;
  }

  sourcePtr=groupPtr->source;
  for(i=0;i<groupPtr->numberOfSources && sourcePtr!=L7_NULLPTR;i++)
  {
    if(sourcePtr->nextSource==L7_NULLPTR)/*Last Source*/
      break;
    sourcePtr=sourcePtr->nextSource;
  }
  if(i+1!=groupPtr->numberOfSources)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with sourcePrt %u<%u",i+1,groupPtr->numberOfSources);
  }

  for (i=0;i<groupPtr->numberOfSources && sourcePtr!=L7_NULLPTR;i++)
  {
    sourcePtrAux=sourcePtr;
    sourcePtr=sourcePtr->previousSource;

    sourcePtrAux->retransmissions=0;
    sourcePtrAux->groupPtr=L7_NULLPTR;   


#if 0
    //We do not remove the Source Entry. Instead we change the group Addr to 0   

    if (snoopPTinProxySourceEntryFind(&groupAddrAux,&sourcePtrAux->key.sourceAddr,L7_MATCH_EXACT)==L7_NULLPTR)
    {       
      inetAddressZeroSet(sourcePtrAux->key.groupAddr.family, &sourcePtrAux->key.groupAddr);
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Source Address Saved on Group Record (vlanId: %u groupAddr:%s sourceAddr:%s)",groupPtr->key.vlanId, inetAddrPrint(&sourcePtrAux->key.groupAddr, debug_buf), inetAddrPrint(&sourcePtrAux->key.sourceAddr, debug_buf2));    
    //Should we change also the VLAN ID to MAX VLANs?
    //  groupPtr->key.vlanId=L7_MAX_VLANS;
         
    }
    else
    {
      if(snoopPTinProxySourceEntryDelete(groupPtr,&sourcePtrAux->key.sourceAddr)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryDelete()");      
        return L7_FAILURE;
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed from Group Record (vlanId: %u groupAddr:%s recordType:%u sourceAddr:%s)",groupPtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType, inetAddrPrint(&sourcePtrAux->key.sourceAddr, debug_buf2));    
      flagSourceRemoved=L7_TRUE;
    }
#else
  if(snoopPTinProxySourceEntryDelete(groupPtr,&sourcePtrAux->key.sourceAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryDelete()");      
    return L7_FAILURE;
  }
//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed from Group Record (vlanId: %u groupAddr:%s recordType:%u sourceAddr:%s)",groupPtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType, inetAddrPrint(&sourcePtrAux->key.sourceAddr, debug_buf2));
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed");
  flagSourceRemoved=L7_TRUE;
#endif

    sourcePtrAux->previousSource=L7_NULLPTR;
    sourcePtrAux->nextSource=L7_NULLPTR;    
  }

  if(i!=groupPtr->numberOfSources)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Problems with groupPtr %u<%u",i,groupPtr->numberOfSources);
  }

  groupPtr->source=L7_NULLPTR;   
  groupPtr->numberOfSources=0;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Remove a Source Address from an existing Group Record
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
L7_RC_t snoopPTinGroupRecordSourceRemove(snoopPTinProxyGroup_t*   groupPtr, L7_inet_addr_t *sourceAddr)
{ 
  
  snoopPTinProxySource_t*    sourcePtr;
  snoopPTinProxySource_t*    sourcePtrTmp;

  L7_inet_addr_t             groupAddrAux;
  L7_uint32 i;

  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};

   /* Argument validation */
  if (groupPtr == L7_NULLPTR /*|| sourceAddr==L7_NULLPTR*/)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }
    
  if ((sourcePtr=snoopPTinProxySourceEntryFind(&groupPtr->key.groupAddr,sourceAddr,L7_MATCH_EXACT))==L7_NULLPTR)
  {
     LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryFind()");
     return L7_SUCCESS;
  }

  sourcePtrTmp=groupPtr->source;
  for(i=0;i<groupPtr->numberOfSources && sourcePtrTmp !=L7_NULLPTR;i++)
  {
    if (sourcePtrTmp ==sourcePtr)
    { 
      if(sourcePtrTmp->previousSource==L7_NULLPTR)/*First Element*/   
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "First source address of this group record");
        groupPtr->source=sourcePtrTmp->nextSource;
      }
      else
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "This is not the first source address of this group record");                
        sourcePtrTmp->previousSource->nextSource=sourcePtrTmp->nextSource;      
      }
      if(sourcePtrTmp->nextSource!=L7_NULLPTR)
        sourcePtrTmp->nextSource->previousSource=sourcePtrTmp->previousSource;        
      break;
    }
    sourcePtrTmp=sourcePtrTmp->nextSource;
  }  


  if(sourcePtrTmp==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Unable to find Source Address within Group Record (vlanId: %u groupAddr:%s recordType:%u, sourceAddr:%s)",groupPtr->interfacePtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,  inetAddrPrint(sourceAddr, debug_buf2));
    return L7_SUCCESS;
  }
  
  sourcePtr->previousSource=L7_NULLPTR;
  sourcePtr->nextSource=L7_NULLPTR;
  sourcePtr->retransmissions=0;


//We do not remove the Source Entry. Instead we change the group Addr to 0
  memset(&groupAddrAux, 0x00, sizeof(L7_inet_addr_t));
  groupAddrAux.family=groupPtr->key.groupAddr.family;
  inetAddressZeroSet(groupAddrAux.family, &groupAddrAux);  

#if 0
  if (snoopPTinProxySourceEntryFind(&groupAddrAux,sourceAddr,L7_MATCH_EXACT)==L7_NULLPTR)
  {       
    inetAddressZeroSet(sourcePtr->key.groupAddr.family, &sourcePtr->key.groupAddr);
//Should we change also the VLAN ID to MAX VLANs?
//  groupPtr->key.vlanId=L7_MAX_VLANS;
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Saved on Group Record (vlanId: %u groupAddr:%s sourceAddr:%s)",groupPtr->interfacePtr->key.vlanId,inetAddrPrint(&sourcePtr->key.groupAddr, debug_buf), inetAddrPrint(&sourcePtr->key.sourceAddr, debug_buf2));    

     sourcePtr->groupPtr=L7_NULLPTR;
  }
  else
  {
    if(snoopPTinProxySourceEntryDelete(groupPtr,sourceAddr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryDelete()");      
      return L7_FAILURE;
    }
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed from Group Record (vlanId: %u groupAddr:%s recordType:%u, sourceAddr:%s)",groupPtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,  inetAddrPrint(sourceAddr, debug_buf2));    
  }
#else
  if(snoopPTinProxySourceEntryDelete(groupPtr,sourceAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxySourceEntryDelete()");      
    return L7_FAILURE;
  }
//LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed from Group Record (vlanId: %u groupAddr:%s recordType:%u, sourceAddr:%s)",groupPtr->key.vlanId, inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),groupPtr->key.recordType,  inetAddrPrint(sourceAddr, debug_buf2));
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address Removed");
#endif
  

  
  --groupPtr->numberOfSources;
  
   

  return L7_SUCCESS;        
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*************************************************************************
 * @purpose Processes General Query
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
snoopPTinProxyGroup_t* snoopPTinGeneralQueryProcess(L7_uint32 vlanId, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *noOfRecordsPtr, L7_uint32 *timeout)
{
//char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  snoopPTinProxyInterface_t *interfacePtr;
  snoopPTinProxyGroup_t* groupPtr;
  L7_uint32   timeLeft,noOfRecords=0;    
  L7_BOOL     pendingReport; 


   /* Argument validation */
  if (sendReport == L7_NULLPTR || noOfRecordsPtr==L7_NULLPTR || timeout==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }


  /*Output Arguments*/
//*sendReport=L7_FALSE;
//*noOfRecordsPtr=0;
//*timeout=0xFFFFFFFF;

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_NULLPTR;
  }

  if((interfacePtr=snoopPTinPendingReport2GeneralQuery(vlanId, &pendingReport, &timeLeft))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GeneralQuery()");
    return L7_NULLPTR;
  }

  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {
/*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);               
    *sendReport=L7_FALSE; 
    return L7_NULLPTR;
  }
  else if (pendingReport==L7_TRUE)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }
  *sendReport=L7_TRUE;

 /*     2. If the received Query is a General Query, the interface timer is
used to schedule a response to the General Query after the
selected delay. Any previously pending response to a General
Query is canceled.*/  

  if ((groupPtr=snoopPTinBuildCSR(interfacePtr,&noOfRecords))==L7_NULLPTR && noOfRecords>0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinBuildCSR()");
    return L7_NULLPTR;
  }
  
  *noOfRecordsPtr=noOfRecords;

  return groupPtr;
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
snoopPTinProxyGroup_t* snoopPTinGroupSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx,L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *timeout)
{
  snoopPTinProxyInterface_t *interfacePtr;  
  L7_uint32   timeLeft=0;
  L7_BOOL     pendingReport=L7_FALSE;
  snoopPTinProxyGroup_t* groupPtr;

//char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

//Initialize Output Variables
  *sendReport=L7_FALSE;  
  *timeout=selectedDelay;

    /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_NULLPTR;
  }

  if((interfacePtr=snoopPTinPendingReport2GeneralQuery(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &pendingReport, &timeLeft))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GeneralQuery()");
    return L7_NULLPTR;
  }
  /*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {

    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);                  
    return L7_NULLPTR;
  }
  else if (pendingReport==L7_TRUE)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }


  if((groupPtr=snoopPTinPendingReport2GroupQuery(avlTreeEntry, interfacePtr, &pendingReport, &timeLeft))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GroupQuery()");
    return L7_NULLPTR;
  }

/*     3. If the received Query is a Group-Specific Query or a Group-and-Source-Specific Query and there is no pending response to a
    previous Query for this group, then the group timer is used to
    schedule a report. If the received Query is a Group-and-Source-Specific Query, the list of queried sources is recorded to be used
    when generating a response.*/ 
//if (pendingReport==L7_FALSE)
//{
//  *sendReport=L7_TRUE;
//  return L7_NULLPTR;
//}

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
//    groupPtr=L7_NULLPTR;
      return L7_NULLPTR;
    }
    else 
    {
      *timeout=selectedDelay;
      *sendReport=L7_TRUE;
      if(snoopPTinGroupRecordSourceRemoveAll(groupPtr)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinGroupRecordSourceRemoveAll()");
        return L7_NULLPTR;
      }
    }
  }
  
  *sendReport=L7_TRUE;
  return groupPtr;
}


/*************************************************************************
 * @purpose Processes a Source Specific Query
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
snoopPTinProxyGroup_t* snoopPTinGroupSourceSpecifcQueryProcess(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 rootIntIdx, L7_ushort16 noOfSources, L7_uchar8** sourceList, L7_uint32 selectedDelay, L7_BOOL *sendReport, L7_uint32 *timeout)
{
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  ptin_IgmpProxyCfg_t igmpCfg;

  snoopPTinProxyInterface_t*  interfacePtr;
  snoopPTinProxySource_t*     sourcePtr;  
  snoopPTinProxyGroup_t*      groupPtr; 
  L7_uint32                   timeLeft,ipv4Addr;    
  L7_BOOL                     pendingReport=L7_FALSE;
  L7_inet_addr_t              sourceAddr;   
   L7_int8                    sourceIdx = -1; 

//Initialize Output Variables  
  *sendReport=L7_FALSE;
  *timeout=selectedDelay;
   


  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceList == L7_NULLPTR || *sourceList == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }
    
  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_NULLPTR;
  }
  
  if((interfacePtr=snoopPTinPendingReport2GeneralQuery(avlTreeEntry->snoopPTinL3InfoDataKey.vlanId, &pendingReport, &timeLeft))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GeneralQuery()");
    return L7_NULLPTR;
  }
 /*     1. If there is a pending response to a previous General Query
scheduled sooner than the selected delay, no additional response
needs to be scheduled*/
  if (pendingReport==L7_TRUE && timeLeft<selectedDelay)
  {

    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",timeLeft,selectedDelay);                     
    return L7_NULLPTR;
  }
  else if (pendingReport==L7_TRUE)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",timeLeft,selectedDelay);           
  }

  if((groupPtr=snoopPTinPendingReport2GroupQuery(avlTreeEntry, interfacePtr, &pendingReport, &timeLeft))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed snoopPTinPendingReport2GroupQuery()");
    return L7_NULLPTR;
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
    SNOOP_GET_ADDR(&ipv4Addr, *sourceList);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &sourceAddr);

    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[rootIntIdx].sources, &sourceAddr, &sourceIdx) && sourceIdx!=-1)
    {      
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Inexisting Source %s on idx %u", inetAddrPrint(&sourceAddr, debug_buf), sourceIdx);           
    }
    else if (sourceIdx==-1)
    {
      return L7_NULLPTR;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(&sourceAddr, debug_buf), sourceIdx);           
      if (*sendReport==L7_FALSE)
        *sendReport=L7_TRUE;
        
      if ((sourcePtr=snoopPTinProxySourceEntryFind(&groupPtr->key.groupAddr,&sourceAddr,L7_MATCH_EXACT))==L7_NULLPTR)
      {
        snoopPTinGroupRecordSourcedAdd(groupPtr,&sourceAddr);        
      }
//    else
//    {
//      sourcePtr->retransmissions=sourcePtr->robustnessVariable;
//    }
    }
    --noOfSources;
  }
  
  if (*sendReport==L7_FALSE)
  {
    /*Should we give a pointer?*/
    snoopPTinGroupRecordRemove(interfacePtr, &groupPtr->key.groupAddr,groupPtr->key.recordType);
    return L7_NULLPTR;
  }

  return groupPtr;
  
}

/*************************************************************************
 * @purpose Verifies if Membership Report to a General Query exists
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
static snoopPTinProxyInterface_t* snoopPTinPendingReport2GeneralQuery(L7_uint32 vlanId, L7_BOOL* pendingReport, L7_uint32* timeout)
{  
   snoopPTinProxyInterface_t* interfacePtr;


   /* Argument validation */
  if (pendingReport == L7_NULLPTR || timeout == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

   //Initialize Output Arguments
//*timeout=0xFFFFFFFF;
//*pendingReport=L7_FALSE;

  if ((interfacePtr=snoopPTinProxyInterfaceEntryAdd(vlanId))==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopPTinProxyInterfaceEntryFind()");
    return L7_NULLPTR;
  }

  
  *pendingReport=snoop_ptin_proxytimer_isRunning(&interfacePtr->timer);
  if (*pendingReport==L7_TRUE)
  {
    *timeout=snoop_ptin_proxytimer_timeleft(&interfacePtr->timer);    
    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Pending report to general query exists (timeout %u)",*timeout);     
  }
  
  return interfacePtr;
}

/*************************************************************************
 * @purpose Verifies if  a Membership Report to a Group Specific Query
 * exists
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
static snoopPTinProxyGroup_t* snoopPTinPendingReport2GroupQuery(snoopPTinL3InfoData_t* avlTreeEntry, snoopPTinProxyInterface_t* interfacePtr, L7_BOOL *pendingReport, L7_uint32 *timeout)
{  
  snoopPTinProxyGroup_t* groupPtr;
  L7_uint8 recordType;

//Initialize Output Variables
  *timeout=0xFFFFFFFF;
  *pendingReport=L7_FALSE;  

 
   /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR ||  interfacePtr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
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
  if ((groupPtr=snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType, L7_MATCH_EXACT))==L7_NULLPTR)                 
  {         
    if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinGroupRecordGroupAdd()");
      return L7_NULLPTR;
    }
  }
  else
  { 
    *pendingReport=snoop_ptin_proxytimer_isRunning(&groupPtr->timer);
    if (*pendingReport==L7_TRUE)
    {
      *timeout=snoop_ptin_proxytimer_timeleft(&groupPtr->timer);            
    }    
  }
  return groupPtr;
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
static snoopPTinProxyGroup_t* snoopPTinBuildCSR(snoopPTinProxyInterface_t *interfacePtr, L7_uint32 *noOfRecordsPtr)
{
  snoopPTinL3InfoData_t *avl_info;  
  snoopPTinL3InfoDataKey_t avl_key;

  char                debug_buf[IPV6_DISP_ADDR_LEN]={};

//L7_inet_addr_t      sources2Report[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD]={};
//L7_uint8            sources2ReportCnt=0;

  snoopPTinProxyGroup_t*     groupPtr;
  snoopPTinProxyGroup_t*     firstGroupPtr=L7_NULLPTR;

  L7_uint32 i,j=0,noOfRecords=0;
  L7_uint8 recordType/*=L7_IGMP_MODE_IS_EXCLUDE*/;  

  snoop_eb_t         *pSnoopEB;

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

  memset(&avl_key,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  while ( ( avl_info = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &avl_key, L7_MATCH_GETNEXT) ) != L7_NULLPTR )
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Element Number %u",++j);
    /* Prepare next key */
    memcpy(&avl_key, &avl_info->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));

    if(avl_info->snoopPTinL3InfoDataKey.vlanId==interfacePtr->key.vlanId && avl_info->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_TRUE)
    {
      if (avl_info->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].filtermode==PTIN_SNOOP_FILTERMODE_INCLUDE)
      {
        recordType=L7_IGMP_MODE_IS_INCLUDE;        
      }
      else
      {
        recordType=L7_IGMP_MODE_IS_EXCLUDE;
      }

      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  recordType:%u", inetAddrPrint(&avl_info->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf),recordType);    

      if((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&avl_info->snoopPTinL3InfoDataKey.mcastGroupAddr ))== L7_NULLPTR)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
        return L7_NULLPTR;
      }
      if (firstGroupPtr==L7_NULLPTR)
      {
        LOG_NOTICE(LOG_CTX_PTIN_IGMP, "First Group Ptr Found");
        firstGroupPtr=groupPtr;
      }

      for(i=0;i<avl_info->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources;i++)
      {
        if (avl_info->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[i].status==PTIN_SNOOP_SOURCESTATE_ACTIVE)
        {                 
          if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,&avl_info->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[i].sourceAddr))
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourcedAdd()");       
            return L7_NULLPTR;
          }                           
        }
      }
    }  
    ++noOfRecords;    
  }  
  
  *noOfRecordsPtr=noOfRecords;
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Number of Group Records to be sent :%u",*noOfRecordsPtr);
  return firstGroupPtr;       
}

/*************************************************************************
 * @purpose Add a Static Source for a Given Group
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
static L7_RC_t snoopPTinAddStaticSource(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_uint32 noOfSources,L7_inet_addr_t* sourceAddrList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{    
  char                debug_buf[IPV6_DISP_ADDR_LEN];
  
  L7_uint32           noOfRecords=0;  
  L7_inet_addr_t*     sourceAddr;

  ptin_IgmpProxyCfg_t igmpCfg;

  L7_int8  sourceIdx = -1;

  /* Argument validation */
  if (avlTreeEntry == L7_NULLPTR || sourceAddrList == L7_NULLPTR )
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


  if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Proxy - Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);    
  }
  else
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Address: %s,  IntIfnum:%d, VLANID=%d", inetAddrPrint(&(avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr), debug_buf),intIfNum, avlTreeEntry->snoopPTinL3InfoDataKey.vlanId);

  sourceAddr=sourceAddrList;
  while (noOfSources > 0 && sourceAddr!=L7_NULLPTR)
  {     

    /* Search for this source in the current source list */    
    if (L7_SUCCESS != snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx) &&sourceIdx!=-1)
    {
      /* Add new source */
      if (L7_SUCCESS != snoopPTinSourceAdd(&avlTreeEntry->interfaces[intIfNum], sourceAddr, &sourceIdx))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Source list for this multicast group is full");
        return L7_FAILURE;
      }           
      if(L7_SUCCESS == snoopPTinProxySourceAdd(avlTreeEntry,intIfNum, sourceAddr))
      {  
        if (L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
          return L7_FAILURE;
        }
        if (noOfRecords==0)
          noOfRecords=1;
      }
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "New source %s on idx %u", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);           
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Existing source %s on idx %d", inetAddrPrint(sourceAddr, debug_buf), sourceIdx);
    }

    /*Set Source as static*/
    avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].isStatic=L7_TRUE;

    /* Set source timer to GMI */
    if (L7_SUCCESS != snoop_ptin_sourcetimer_start(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].sourceTimer, igmpCfg.querier.group_membership_interval, avlTreeEntry, intIfNum, sourceIdx))
    {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start sourcetimer");
       return L7_FAILURE;
    }

    /* Add client if it does not exist */    
    if (L7_SUCCESS != snoopPTinClientAdd(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client list is full");
      return L7_FAILURE;
    }
    
    --noOfSources;
    sourceAddr++;
  }

  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Add a Static Group and Source List
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
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinL3EntryAdd(%u,%u)",groupAddr,vlanId);
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

  if ( (interfacePtr=snoopPTinProxyInterfaceAdd(vlanId)) ==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
    return L7_FAILURE;    
  }
  if (noOfSources==0 && snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources==0)
  {
    recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;    
  }   
  if((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr))== L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
    return L7_FAILURE;
  }  

  rc=snoopPTinAddStaticSource(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM,intIfNum ,noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: Failed to snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }
  PTIN_SET_MASKBIT(snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients, intIfNum);

  clientIdx=PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1;
  rc=snoopPTinAddStaticSource(snoopEntry, intIfNum, clientIdx, noOfSources,sourceAddr,&noOfRecords,groupPtr);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf interface: Failed to  snoopPTinAddStaticSource()");
    return L7_FAILURE;
  }  
  PTIN_SET_MASKBIT(snoopEntry->interfaces[intIfNum].clients, clientIdx);

  if (noOfSources==0 || noOfRecords>0)
  {
    noOfSources=1;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (snoopPTinReportSchedule(vlanId,groupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,noOfRecords, groupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }
  
  return rc;

}

/*************************************************************************
 * @purpose Remove a Static Source for a given Group
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
static L7_RC_t snoopPTinRemoveStaticSource(snoopPTinL3InfoData_t* avlTreeEntry, L7_uint32 intIfNum, L7_uint32 clientIdx, L7_uint32 noOfSources,L7_inet_addr_t* sourceAddrList,L7_uint32 *noOfRecordsPtr, snoopPTinProxyGroup_t* groupPtr)
{    
  char                debug_buf[IPV6_DISP_ADDR_LEN];

  L7_uint32           noOfRecords=0;  
  L7_inet_addr_t*     sourceAddr;

  L7_int8            sourceIdx = -1;  

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

  sourceAddr=sourceAddrList;
  while (noOfSources > 0 && sourceAddr!=L7_NULLPTR)
  {
    /* Search for this source in the current source list */    
    if (L7_SUCCESS == snoopPTinSourceFind(avlTreeEntry->interfaces[intIfNum].sources, sourceAddr, &sourceIdx))
    {
      /*Set Source as static*/
      avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].isStatic=L7_FALSE;

      /* Remove this client from this source */      
      if(L7_SUCCESS!=snoopPTinClientRemove(&avlTreeEntry->interfaces[intIfNum].sources[sourceIdx], clientIdx))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to remove clientIdx %u", clientIdx);
        return L7_FAILURE;          
      }      
      
      /* If there is no more clients within this source, remove this source*/          
       if (intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM && 
          avlTreeEntry->interfaces[intIfNum].filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE && 
          avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].numberOfClients==L7_FALSE &&
          L7_SUCCESS != snoopPTinGroupRecordSourcedAdd(groupPtr,sourceAddr))
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordSourceListAdd()");
        return L7_FAILURE;
        if (noOfRecords==0)
          noOfRecords=1;
      }
      if (avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].numberOfClients==L7_FALSE && 
          snoopPTinSourceRemove(&avlTreeEntry->interfaces[intIfNum], &avlTreeEntry->interfaces[intIfNum].sources[sourceIdx])!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinSourceRemove()");
        return L7_FAILURE;
      }    
     
    }
    else if (sourceIdx==-1)
    {
      return L7_FAILURE;
    }
    --noOfSources;
    sourceAddr++;      
  }
 
  *noOfRecordsPtr=noOfRecords;

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Remove a Static Group and Sources
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
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinL3EntryFind()");
      return L7_SUCCESS;       
  }

  /* If root interface is  used*/
  if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active == L7_TRUE)
  {
     /*Set Group as static*/
    snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic=L7_FALSE;

    if ( (interfacePtr=snoopPTinProxyInterfaceAdd(vlanId)) ==L7_NULLPTR)
    {      
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordInterfaceAdd()");
      return L7_FAILURE;                                         
    }
    if (noOfSources==0 && snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources==0)
    {
      recordType=L7_IGMP_CHANGE_TO_INCLUDE_MODE;    
    }   

    if((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
      return L7_FAILURE;
    }    

    rc=snoopPTinRemoveStaticSource(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM,intIfNum ,noOfSources,sourceAddr,&noOfRecords,groupPtr);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: Failed to snoopPTinRemoveStaticSource()");
      return L7_FAILURE;
    }
    
    PTIN_UNSET_MASKBIT(snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients, intIfNum);       
    
    /* If no sources remain and no clients remain, remove group*/
    if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources == 0 && --snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients==L7_FALSE)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Removing interface");
      rc=snoopPTinInterfaceRemove(&snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM],vlanId,groupAddr,SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);
      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: Failed to snoopPTinInterfaceRemove()");
        return L7_FAILURE;
      }
    }     
  }

 /* If Leaf interface is used*/
  if (snoopEntry->interfaces[intIfNum].active == L7_TRUE)
  {
    /*Set Group as static*/
    snoopEntry->interfaces[intIfNum].isStatic=L7_FALSE;
    /*Set the ClientIdx as the Last One*/
    clientIdx=PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE-1;  
    rc=snoopPTinRemoveStaticSource(snoopEntry, intIfNum, clientIdx, noOfSources,sourceAddr,&noOfRecords,groupPtr);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf interface: Failed to  snoopPTinRemoveStaticSource()");
      return L7_FAILURE;
    }
    
    PTIN_UNSET_MASKBIT(snoopEntry->interfaces[intIfNum].clients, clientIdx);     
   
    /* If no sources remain and no clients remain, remove group*/
    if (--snoopEntry->interfaces[intIfNum].numberOfClients==L7_FALSE && snoopEntry->interfaces[intIfNum].numberOfSources == 0)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Removing interface");
      rc=snoopPTinInterfaceRemove(&snoopEntry->interfaces[intIfNum],vlanId,groupAddr,intIfNum);
      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf Interface: Failed to snoopPTinInterfaceRemove()");
        return L7_FAILURE;
      }
    }
  }
  
  if (noOfSources==0 || noOfRecords>0)
  {
    noOfRecords=1;
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (snoopPTinReportSchedule(vlanId,groupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,noOfRecords, groupPtr)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }  
  
  return L7_SUCCESS;

}


