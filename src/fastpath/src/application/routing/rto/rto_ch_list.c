/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto_ch_list.c
*
* @purpose RTO organizes all routes into a change list.
*
* @component Routing Table Object for IP Mapping Layer
*
* @comments
*
* @create 6/9/2005
*
* @author rrice
* @end
*
**********************************************************************/
/*
Each component interested in best route changes registers a callback function
with RTO. Instead of RTO immediately pushing individual route changes to each
of its best route clients, RTO simply notifies clients when a change has
occurred. In response, clients query RTO for the list of outstanding changes
at a time convenient for them. Some clients, such as IP MAP for forwarding
table updates, may prefer faster notification than others. Each client can
request that RTO return a set of N changes, thus eliminating the inefficiency
of sending a notification for each individual change, but also limiting the
size of the change notification to whatever the client can handle in a single
notification.

RTO keeps track of the clients it has notified that changes exist but which
have not yet retrieved those changes. When a change occurs, RTO only notifies
clients that are not already pending on previous changes.

RTO maintains a "change list," which includes every best route. Routes on the
change list are ordered by last change time. The last route on the change list
is the route most recently changed. When RTO changes or deletes an existing
route, the route is removed from its current position in the change list and
is appended to the end of the change list. A route may be moved to the end of
the change list even if RTO has not sent the route's previous change to all
clients. When a route changes several times in a short interval, clients may
be lucky enough to be spared notification of each change, only hearing the final
result.

RTO keeps track of the changes it has reported to each client. Since the change
list is ordered by change time, RTO can do this simply by remembering each
client's place in the change list. When a client requests the next N changes,
RTO finds that client's place in the change list and reports the next N entries
on the list. If fewer than N changes are pending, RTO reports all pending
changes and reports the number of changes included in the response. The response
explicitly indicates if more changes are outstanding.

Routing protocols that register as best route clients must receive change
notices for their own routes. This rule is because a protocol's own route may
replace another protocol's route as the best route. The notification that the
protocol's own route is best implicitly withdraws the other protocol's route.
If the other protocol's route was previously redistributed, the redistributed
route is withdrawn.

When RTO deletes a best route, it must retain the deleted route on the change
list until it has notified all clients of the deletion, or until the deleted
route is replaced with a new route. RTO needs an efficient way to know when it
has notified all clients that a route has been deleted so that RTO can remove
the route from its own routing table. To facilitate this, RTO will label each
best route with a change sequence number. When RTO reports a set of changes to
a client, RTO will note if any of the changes reported is a delete. If a change
is a delete and the client is the last client to receive the delete, RTO
removes the route from its routing table. A client is the last client to
receive the change if all other clients' position in the change list has a
higher sequence number.

The only route to some destinations may have a preference of 255. In such
a case, there is no "best route" to the destination. Radix nodes for such
routes are not linked into the change list. When such a route is deleted,
it is immediately removed from the radix tree, since there is no need for
client notification.

Functions in this file declared static assume the caller already holds
a read or write lock for RTO.

*/

#include "stdlib.h"
#include "string.h"
#include "log.h"
#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "rto_api.h"
#include "rto.h"
#include "radix_api.h"
#include "osapi_support.h"


/* When a new client registers with RTO, RTO initializes the client-specific
 * change type in all elements on the change list. The new client receives a
 * route ADD for each element whose change type is ADD or MODIFY. The new
 * client is not informed of deleted routes. */
e_RTO_RT_CHANGE_TYPE rtoNewClientChTypeMap[] = {RTO_NOOP, RTO_RT_ADD, RTO_NOOP,
                                                RTO_RT_ADD};

/* Mapping from e_RTO_RT_CHANGE_TYPE to RTO_ROUTE_EVENT_t. Note that
 * RTO_NOOP cannot be mapped. */
RTO_ROUTE_EVENT_t rtoChTypeMap[] = {RTO_ADD_ROUTE, RTO_ADD_ROUTE,
  RTO_DELETE_ROUTE, RTO_CHANGE_ROUTE};

L7_uchar8 *rtChangeTypeStr[] = {"No-op", "ADD", "DEL", "MOD"};

/* List of best route clients. The client callback uniquely identifies a client. */
static RtoBestRouteClient *BestRouteClientList = L7_NULLPTR;

/* head and tail of best route change list */
static rtoRouteData_t *rtoChangeListHead = L7_NULLPTR;
static rtoRouteData_t *rtoChangeListTail = L7_NULLPTR;

extern radixTree_t rtoRouteTreeData;
extern rto_stats_t rtoStats;
extern L7_uint32 rtoTraceFlags;
extern osapiRWLock_t rtoRwLock;


static L7_RC_t rtoClientChangeListInit(L7_uint32 clientIndex);
static RtoBestRouteClient *rtoTrailingClient(void);
static L7_RC_t rtoChangeListPurgeRoute(rtoRouteData_t *pData);
static L7_RC_t rtoChangeListPurge(rtoRouteData_t *start, rtoRouteData_t *stop);
static RtoBestRouteClient *rtoClientFind(L7_VOIDFUNCPTR_t callback);
static L7_BOOL rtoBrClientExists(void);
static L7_RC_t rtoChangeListLink(rtoRouteData_t *pData);
static L7_RC_t rtoChangeListUnlink(rtoRouteData_t *pData);
static L7_uint32 rtoNextChangeSeqNo(void);
static L7_BOOL rtoClientPendingOnChange(RtoBestRouteClient *client,
                                        rtoRouteData_t *change);
static L7_BOOL rtoChangeIsEarlier(L7_uint32 ch1, L7_uint32 ch2);
static void rtoNotifyBestRouteClients(void);
static L7_RC_t rtoChangeListReadd(rtoRouteData_t *pData);

void rtoClientListShow(void);
void rtoChangeListShow(L7_uint32 detail);

/*********************************************************************
* @purpose  Allocate the best route client list.
*
* @param  void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called during p1 init.
*
* @end
*******************************************************************/
L7_RC_t rtoClientListAllocate(void)
{
  BestRouteClientList = osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                                    sizeof(RtoBestRouteClient) * RTO_MAX_BR_CLIENTS);
  if (BestRouteClientList == L7_NULLPTR)
    return L7_FAILURE;

  memset(BestRouteClientList, 0, sizeof(RtoBestRouteClient) * RTO_MAX_BR_CLIENTS);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deallocate the best route client list.
*
* @param  void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t rtoClientListDeallocate(void)
{
  if (BestRouteClientList != L7_NULLPTR)
  {
    osapiFree(L7_IP_MAP_COMPONENT_ID, BestRouteClientList);
    BestRouteClientList = NULL;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicates whether any best route clients are registered.
*
* @param  void
*
* @returns  L7_TRUE if one or more best route clients are registered.
*           L7_FALSE otherwise
*
* @notes
*
* @end
*******************************************************************/
static L7_BOOL rtoBrClientExists(void)
{
  L7_uint32 i;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if (BestRouteClientList[i].callback)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Find a best route client with a given callback.
*
* @param  void
*
* @returns  L7_SUCCESS
*
* @notes   Caller must already have the RTO read or write lock since
*          the function returns a pointer to protected data.
*
* @end
*******************************************************************/
static RtoBestRouteClient *rtoClientFind(L7_VOIDFUNCPTR_t callback)
{
  L7_uint32 i;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if (BestRouteClientList[i].callback == callback)
    {
      return &BestRouteClientList[i];
    }
  }
  return NULL;
}

/*********************************************************************
* @purpose  Register a callback function to be called when best routes
*           are added, changed, or deleted.
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.
* @param  funcPtr @b{(input)}     Pointer to the callback function.
*
* @returns  L7_SUCCESS  if the callback function is registered.
* @returns  L7_FAILURE  if the callback function is not registered,
*                       either because of an error in the input
*                       parameters or because the maximum number of
*                       callbacks are already registered.
*
* @notes    If a caller attempts to register a callback function
*           that is already registered, the function returns
*           L7_SUCCESS without registering the function a second
*           time.
* @notes
*
* @end
*******************************************************************/
L7_RC_t rtoBestRouteClientRegister (L7_uchar8 *clientName, void (*funcPtr)(void))
{
  L7_uint32 i;

    if (funcPtr == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
              "Best route client registration failed for %s. NULL function pointer.\n",
              clientName);
      return L7_FAILURE;
    }
    osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER);

    /* First traverse the entire list to be sure this isn't a duplicate. */
    if (rtoClientFind(funcPtr))
    {
      /* Calling application may be coded in a way that duplicate
       * attempts to register are expected, but log a message in
       * case this is inadvertant.
       */
      osapiWriteLockGive(rtoRwLock);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_IP_MAP_COMPONENT_ID,
              "Attempt to register best route notification for %s callback twice with RTO.\n",
              clientName);
      return L7_SUCCESS;
    }

    for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
    {
      if (BestRouteClientList[i].callback == L7_NULLPTR)
      {
        BestRouteClientList[i].callback = funcPtr;
        strncpy(BestRouteClientList[i].clientName, clientName, RTO_CLIENT_NAME_LEN);
        BestRouteClientList[i].clientName[RTO_CLIENT_NAME_LEN] = '\0';
        BestRouteClientList[i].clientIndex = i;
        BestRouteClientList[i].changePending = L7_FALSE;
        BestRouteClientList[i].nextChange = rtoChangeListHead;
        if (rtoClientChangeListInit(i) != L7_SUCCESS)
        {
          BestRouteClientList[i].callback = NULL;
          osapiWriteLockGive(rtoRwLock);
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
                  "Failure initialiing RTO change list for client %s.", clientName);
          return L7_FAILURE;
        }

        osapiWriteLockGive(rtoRwLock);
        if (rtoTraceFlags & RTO_TRACE_CHANGE_LIST)
        {
          L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
          osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO added best route client %s", clientName);
          rtoTraceWrite(traceBuf);
        }
        return L7_SUCCESS;
      }
    }

    osapiWriteLockGive(rtoRwLock);
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Deregister a callback function to be called when best routes
*           are added, changed, or deleted.
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.
* @param  funcPtr @b{(input)}     Pointer to the callback function.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t rtoBestRouteClientDeregister (L7_uchar8 *clientName, void (*funcPtr)(void))
{
  RtoBestRouteClient *client = NULL;
  RtoBestRouteClient *newTrailingClient = NULL;

  /* If the client being deregistered is the trailing client, tcNextChange is the
   * change list element this client was pending on. */
  rtoRouteData_t *tcNextChange = NULL;
  rtoRouteData_t *newTcNextChange = NULL;     /* same for new trailing client */
  rtoRouteData_t *lastPurgeCand = NULL;

  if (funcPtr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER);
  client = rtoClientFind(funcPtr);
  if (!client)
  {
    osapiWriteLockGive(rtoRwLock);
    return L7_ERROR;
  }

  /* if client is the trailing client, find the new trailing client and
   * delete deleted routes in between. */
  if ((client->nextChange) && (client == rtoTrailingClient()))
  {
    tcNextChange = client->nextChange;
  }
  if (rtoTraceFlags & RTO_TRACE_CHANGE_LIST)
  {
    L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO removed best route client %s", client->clientName);
    rtoTraceWrite(traceBuf);
  }
  memset(client, 0, sizeof(RtoBestRouteClient));

  /* if we deregistered the trailing client, we may be able to delete
   * change list entries. */
  if (tcNextChange)
  {
    /* Find new trailing client */
    newTrailingClient = rtoTrailingClient();
    if (newTrailingClient)
      newTcNextChange = newTrailingClient->nextChange;
    if (newTcNextChange)
      lastPurgeCand = newTcNextChange->prevChange;
    else
      lastPurgeCand = rtoChangeListTail;
    if (lastPurgeCand)
      rtoChangeListPurge(tcNextChange, lastPurgeCand);
  }
  osapiWriteLockGive(rtoRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize the client-specific change type on all routes
*           for a newly-registered client. If routes exist, notify the
*           client.
*
* @param    clientIndex  @b{(input)}  Client's index to the best route client list
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t rtoClientChangeListInit(L7_uint32 clientIndex)
{
  rtoRouteData_t *pData = rtoChangeListHead;
  while (pData != L7_NULLPTR)
  {
    pData->clientChType[clientIndex] = rtoNewClientChTypeMap[pData->changeType];
    pData = pData->nextChange;
  }

  BestRouteClientList[clientIndex].nextChange = rtoChangeListHead;

  if (rtoChangeListHead)
  {
    /* Tell client to come and get it */
    (BestRouteClientList[clientIndex].callback)();
    BestRouteClientList[clientIndex].changePending = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the "trailing client."  The trailing client is the client
*           pending on the oldest change.
*
* @param    void
*
* @returns  Pointer to the trailing client (an entry on the client list)
*           or NULL, if no clients are pending on any changes.
*
* @notes
*
* @end
*********************************************************************/
static RtoBestRouteClient *rtoTrailingClient(void)
{
  L7_uint32 i;
  RtoBestRouteClient *trailingClient = NULL;
  L7_uint32 tcChangeSeqNo = 0;
  rtoRouteData_t *changeListElem;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if (BestRouteClientList[i].callback)
    {
      changeListElem = BestRouteClientList[i].nextChange;
      if (changeListElem)
      {
        if (!trailingClient)
        {
          trailingClient = &BestRouteClientList[i];
          tcChangeSeqNo = changeListElem->changeSeqNo;
        }
        else
        {
          if (rtoChangeIsEarlier(changeListElem->changeSeqNo, tcChangeSeqNo))
          {
            trailingClient = &BestRouteClientList[i];
            tcChangeSeqNo = changeListElem->changeSeqNo;
          }
        }
      }
    }
  }
  return trailingClient;
}

/*********************************************************************
* @purpose  After all best route clients have been notified that a route
*           has been deleted, remove the route from RTO.
*
* @param    start @b{(input)} first change list element to consider for deletion
* @param    stop  @b{(input)} last change list element to consider for deletion
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    stop may be before start in the change list. In this case, do nothing.
*
* @end
*********************************************************************/
static L7_RC_t rtoChangeListPurge(rtoRouteData_t *start, rtoRouteData_t *stop)
{
  rtoRouteData_t *pData = start;
  rtoRouteData_t *pDataNext = NULL;

  if (!stop)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Error purging change list. No stop.");
    return L7_FAILURE;
  }

  if (rtoChangeIsEarlier(stop->changeSeqNo, start->changeSeqNo))
    return L7_SUCCESS;

  while (pData)
  {
    if (pData == stop)
      pDataNext = NULL;
    else
      pDataNext = pData->nextChange;

    if (pData->changeType == RTO_RT_DEL)
      rtoChangeListPurgeRoute(pData);
    pData = pDataNext;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Link a new route to the end of the change list.
*
* @param    pData @b{(input)} route to be added
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t rtoChangeListLink(rtoRouteData_t *pData)
{
  pData->changeSeqNo = rtoNextChangeSeqNo();
  if (rtoChangeListTail)
  {
    rtoChangeListTail->nextChange = pData;
    pData->prevChange = rtoChangeListTail;
  }
  else
  {
    rtoChangeListHead = pData;
    pData->prevChange = NULL;
  }
  rtoChangeListTail = pData;
  pData->nextChange = NULL;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Unlink a route from the change list.
*
* @param    pData @b{(input)} route to be removed
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t rtoChangeListUnlink(rtoRouteData_t *pData)
{
  pData->changeType = RTO_NOOP;

  if (rtoChangeListHead == pData)
    rtoChangeListHead = pData->nextChange;

  if (rtoChangeListTail == pData)
    rtoChangeListTail = pData->prevChange;

  if (pData->prevChange)
    pData->prevChange->nextChange = pData->nextChange;

  if (pData->nextChange)
    pData->nextChange->prevChange = pData->prevChange;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  After all best route clients have been notified that a route
*           has been deleted, remove the route from RTO.
*
* @param    pData @b{(input)} route to be removed from RTO
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t rtoChangeListPurgeRoute(rtoRouteData_t *pData)
{
  L7_uint32 i;

  /* If a client points to this route, bump the client to the next route. */
  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if (BestRouteClientList[i].nextChange == pData)
    {
      BestRouteClientList[i].nextChange = pData->nextChange;
    }
  }

  rtoChangeListUnlink(pData);

  /* There may still be a non-best route to the destination, such as a
   * route with a pref of 255. Don't remove from radix tree in that case. */
  if (pData->nextRouteInfo == NULL)
  {
    pData = radixDeleteEntry(&rtoRouteTreeData, pData);
    if (pData == L7_NULLPTR)
      L7_LOG_ERROR(0);
    rtoStats.radix_entries--;
  }
  else if (pData->nextRouteInfo->flags & RTO_BEST_ROUTE_NODE)
  {
    L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uint32 maskLen;
    osapiInetNtoa(osapiNtohl(pData->network.addr), destAddrStr);
    maskLen = rtoMaskLength(osapiNtohl(pData->netmask.addr));
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
            "Purging destination %s/%d from routing table while best route exists.",
            destAddrStr, maskLen);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether a client is pending on a given change.
*           The client is pending on the change if the client has not
*           yet been notified of the change.
*
* @param    client @b{(input)} the client in question
* @param    change @b{(input)} the change in question
*
* @returns  L7_TRUE if the client is still pending on the change
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL rtoClientPendingOnChange(RtoBestRouteClient *client,
                                        rtoRouteData_t *change)
{
  rtoRouteData_t *nextChange = client->nextChange;
  L7_uint32 changeSeqNo = change->changeSeqNo;
  L7_uint32 clientSeqNo;

  if (!nextChange)
    return L7_FALSE;

  clientSeqNo = nextChange->changeSeqNo;

  return !rtoChangeIsEarlier(changeSeqNo, clientSeqNo);
}

/*********************************************************************
* @purpose  Determine if one change is earlier than another.
*
* @param    ch1 @b{(input)} first change sequence number
* @param    ch2 @b{(input)} second change sequence number
*
* @returns  L7_TRUE if the first change is earlier than the second
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL rtoChangeIsEarlier(L7_uint32 ch1, L7_uint32 ch2)
{
  if (ch1 > ch2)
  {
    if ((ch1 - ch2) > 0xEFFFFFFF)
      /* seq no wrap. ch1 is earlier. */
      return L7_TRUE;
    else
      return L7_FALSE;
  }
  else if (ch2 > ch1)
  {
    if ((ch2 - ch1) > 0xEFFFFFFF)
      /* seq no wrap. ch2 is earlier. */
      return L7_FALSE;
    else
      return L7_TRUE;
  }
  else
    /* same */
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Add a new route to the end of the change list.
*
* @param    pData @b{(input)} route being added
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rtoChangeListAdd(rtoRouteData_t *pData)
{
  L7_uint32 i;

  if (pData->changeType == RTO_RT_DEL)
  {
    /* Destination was in RTO before this ADD as a pending delete. */
    return rtoChangeListReadd(pData);
  }

  rtoChangeListLink(pData);

  /* set change type for element and for each client. Set nextChange pointer
   * for those clients that have received all previous changes. */
  pData->changeType = RTO_RT_ADD;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    RtoBestRouteClient *client = &BestRouteClientList[i];
    if (client->callback)
    {
      pData->clientChType[i] = RTO_RT_ADD;
      if (client->nextChange == NULL)
      {
        client->nextChange = pData;
      }
    }
  }

  rtoNHResNotify(pData, RTO_RT_ADD);
  rtoNotifyBestRouteClients();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Move a deleted route to the end of the change list.
*
* @param    pData @b{(input)} route to be moved
*
* @returns  L7_SUCCESS
*
* @notes    For each client whose nextChange pointer is at or before the
*           deleted route, the change type is set to RTO_RT_MOD.
*
* @end
*********************************************************************/
static L7_RC_t rtoChangeListReadd(rtoRouteData_t *pData)
{
  RtoBestRouteClient *client;
  L7_uint32 i;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    client = &BestRouteClientList[i];
    if (client->callback == NULL)
      continue;
    if ((rtoClientPendingOnChange(client, pData)) &&
        (pData->clientChType[i] == RTO_RT_DEL))
    {
      /* client never got the delete, so send it a modify instead of an add */
      pData->clientChType[i] = RTO_RT_MOD;
    }
    else
    {
      pData->clientChType[i] = RTO_RT_ADD;
    }

    /* update clients' nextChange pointers */
    if (client->nextChange == pData)
    {
      client->nextChange = pData->nextChange;
    }
    if (client->nextChange == NULL)
    {
      client->nextChange = pData;
    }
  }

  rtoChangeListUnlink(pData);
  rtoChangeListLink(pData);

  pData->changeType = RTO_RT_ADD;

  rtoNHResNotify(pData, RTO_RT_ADD);
  rtoNotifyBestRouteClients();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Move a modified route to the end of the change list.
*
* @param    pData @b{(input)} modified route
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rtoChangeListMod(rtoRouteData_t *pData)
{
  RtoBestRouteClient *client;
  L7_uint32 i;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    client = &BestRouteClientList[i];
    if (client->callback == NULL)
      continue;
    if (rtoClientPendingOnChange(client, pData))
    {
      /* client never got the previous change. If prev change was ADD or MOD,
       * leave it. If prev change was DEL, we have an error. */
      if (pData->clientChType[i] == RTO_RT_DEL)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "For client %s, MODIFY follows DELETE.",
                BestRouteClientList[i].clientName);
        pData->clientChType[i] = RTO_RT_MOD;
      }
    }
    else
    {
      pData->clientChType[i] = RTO_RT_MOD;
    }

    /* Update clients' nextChange pointers */
    if (client->nextChange == pData)
    {
      client->nextChange = pData->nextChange;
    }
    if (client->nextChange == NULL)
    {
      client->nextChange = pData;
    }
  }

  rtoChangeListUnlink(pData);
  rtoChangeListLink(pData);

  pData->changeType = RTO_RT_MOD;

  rtoNHResNotify(pData, RTO_RT_MOD);
  rtoNotifyBestRouteClients();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Move a deleted route to the end of the change list.
*
* @param    pData @b{(input)} modified route
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rtoChangeListDel(rtoRouteData_t *pData)
{
  RtoBestRouteClient *client;
  L7_uint32 i;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    client = &BestRouteClientList[i];
    if (client->callback == NULL)
      continue;
    if (rtoClientPendingOnChange(client, pData))
    {
      /* client never got the previous change. If prev change was ADD,
       * client doesn't need to hear delete. If prev change was MOD,
       * client needs to hear a delete. If prev change was DEL, we have
       * a problem. */
      switch (pData->clientChType[i])
      {
      case RTO_RT_ADD:
        pData->clientChType[i] = RTO_NOOP;
        break;
      case RTO_RT_MOD:
        pData->clientChType[i] = RTO_RT_DEL;
        break;
      case RTO_RT_DEL:
      case RTO_NOOP:
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "For client %s, DELETE follows %s.",
                BestRouteClientList[i].clientName,
                rtChangeTypeStr[pData->clientChType[i]]);
      }
    }
    else
    {
      pData->clientChType[i] = RTO_RT_DEL;
    }

    /* Update clients' nextChange pointers */
    if (client->nextChange == pData)
    {
      client->nextChange = pData->nextChange;
    }
    if (client->nextChange == NULL)
    {
      client->nextChange = pData;
    }
  }

  rtoNHResNotify(pData, RTO_RT_DEL);
  if (rtoBrClientExists())
  {
    rtoChangeListUnlink(pData);
    rtoChangeListLink(pData);
    pData->changeType = RTO_RT_DEL;
    rtoNotifyBestRouteClients();
  }
  else
  {
    /* No best route clients, so route can be immediately removed from RTO */
    rtoChangeListPurgeRoute(pData);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the sequence number for a new change list element
*           being appended to the list.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    0 is considered an invalid sequence number
*
* @end
*********************************************************************/
static L7_uint32 rtoNextChangeSeqNo(void)
{
  L7_uint32 lastNum;        /* seq no of last element on list */

  if (!rtoChangeListTail)
    return 1;               /* start with 1, not 0 */

  lastNum = rtoChangeListTail->changeSeqNo;

  if (lastNum == 0xFFFFFFFF)
    return 1;

  return (lastNum + 1);
}

/* A testing and debugging utility */
L7_BOOL skipNotify = L7_FALSE;
void setSkipNotify(L7_BOOL val)
{
  skipNotify = val;
  printf("\nskipNotify set to %s", (skipNotify ? "TRUE" : "FALSE"));
}

/*********************************************************************
* @purpose  Tell best route clients that a best route has changed.
*
* @param    void
*
* @returns  void
*
* @notes   If a client is already pending on changes, don't notify again.
*
* @end
*********************************************************************/
static void rtoNotifyBestRouteClients(void)
{
  L7_uint32 i;

  if(skipNotify)   /* notification can be turned off for testing and debugging. */
    return;

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if ((BestRouteClientList[i].callback) && !BestRouteClientList[i].changePending)
    {
      if (rtoTraceFlags & RTO_TRACE_NOTIF)
      {
        L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
        osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO notifying %s of best route changes.",
                BestRouteClientList[i].clientName);
        rtoTraceWrite(traceBuf);
      }
      BestRouteClientList[i].changePending = L7_TRUE;
      (BestRouteClientList[i].callback) ();
    }
  }
}

/****************************************************************
* @purpose  Request a set of the next N best route changes
*           for a specific client.
*
* @param  callback @b{(input)} callback function pointer uniquely
*                              identifying client
* @param  maxChanges @b {(input)} maximum number of changes
*                                 client is willing to receive
* @param  numChanges @b {(output)} number of changes returned
* @param  moreChanges @b {(output)} L7_TRUE if RTO has more
*                                   changes to report to this
*                                   client
* @param  routes @b {(output)} RTO copies the changed routes to
*               this buffer. The client allocates the buffer
*               large enough to hold maxChanges.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_FAILURE  If not successful
*
* @notes
*
* @end
***************************************************************/
L7_RC_t rtoRouteChangeRequest (L7_VOIDFUNCPTR_t callback,
                              L7_uint32 maxChanges,
                              L7_uint32 *numChanges,
                              L7_BOOL *moreChanges,
                              rtoRouteChange_t *routes)
{
  RtoBestRouteClient *client = NULL;
  rtoRouteData_t *firstToReport = NULL;
  rtoRouteData_t *nextToReport = NULL;
  L7_routeEntry_t *routeEntry;
  rtoRouteInfo_t *routeInfo;
  rtoNextHop_t *nextHop;
  L7_uint32 activeNextHops;

  *numChanges = 0;

  /* clear the output buffer */
  memset(routes, 0, maxChanges * sizeof(rtoRouteChange_t));

  osapiWriteLockTake(rtoRwLock, L7_WAIT_FOREVER);
  client = rtoClientFind(callback);
  if (!client)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
           "Failed to find RTO client for route change request.");
    osapiWriteLockGive(rtoRwLock);
    return L7_FAILURE;
  }

  if (client == rtoTrailingClient())
  {
    firstToReport = client->nextChange;
  }

  nextToReport = client->nextChange;
  while ((*numChanges < maxChanges) && nextToReport)
  {
    /* set client-specific change type */
    if (nextToReport->clientChType[client->clientIndex] == RTO_NOOP)
    {
      nextToReport = nextToReport->nextChange;
      continue;
    }

    routes[*numChanges].changeType =
      rtoChTypeMap[nextToReport->clientChType[client->clientIndex]];

    routeEntry = &routes[*numChanges].routeEntry;
    routeEntry->ipAddr = osapiNtohl(nextToReport->network.addr);
    routeEntry->subnetMask = osapiNtohl(nextToReport->netmask.addr);
    routeInfo = nextToReport->nextRouteInfo;
    /* deletes have no routeInfo */
    if (routeInfo && (routeInfo->flags & RTO_BEST_ROUTE_NODE))
    {
      routeEntry->protocol = routeInfo->protocol;
      routeEntry->metric = routeInfo->metric;
      routeEntry->pref = routeInfo->preference1;
      if(routeInfo->flags & RTO_REJECT_ROUTE)
         routeEntry->flags |= L7_RTF_REJECT;
      activeNextHops = 0;
      nextHop = routeInfo->nextHops;
      while (nextHop)
      {
        routeEntry->ecmpRoutes.equalCostPath[activeNextHops].arpEntry.intIfNum =
          nextHop->intIfNum;

        routeEntry->ecmpRoutes.equalCostPath[activeNextHops].arpEntry.ipAddr =
          nextHop->nextHopIP;

        activeNextHops++;

        /* truncate ecmp if necessary */
        if (routeInfo->flags & RTO_ECMP_TRUNC)
          break;

        nextHop = nextHop->nextNh;
      }
      routeEntry->ecmpRoutes.numOfRoutes = activeNextHops;
    }

    nextToReport = nextToReport->nextChange;
    (*numChanges)++;
  }

  client->changePending = *moreChanges = (nextToReport ? L7_TRUE : L7_FALSE);
  client->nextChange = nextToReport;

  if (firstToReport)
  {
    /* client was the trailing client. Purge deleted routes which all
     * clients now know about. */
    RtoBestRouteClient *trailingClient = rtoTrailingClient();
    rtoRouteData_t *lastPurgeCand = NULL;
    if (trailingClient)
    {
      lastPurgeCand = trailingClient->nextChange->prevChange;
    }
    else
    {
      lastPurgeCand = rtoChangeListTail;
    }
    if (lastPurgeCand)
      rtoChangeListPurge(firstToReport, lastPurgeCand);
  }
  if (rtoTraceFlags & RTO_TRACE_NOTIF)
  {
    L7_uchar8 traceBuf[RTO_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, RTO_TRACE_LEN_MAX, "RTO sending %u best route changes to client %s. More = %s.",
            *numChanges, client->clientName, (*moreChanges ? "Y" : "N"));
    rtoTraceWrite(traceBuf);
  }
  osapiWriteLockGive(rtoRwLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the change list.
*
* @param    detail  - How much detail is printed
*               1 -  print destinations, seqno, change type
*               2 -  also print client-specific change types
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rtoChangeListShow(L7_uint32 detail)
{
  rtoRouteData_t *pData;
  L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 maskLen;

  sysapiPrintf("%18s  %13s  %11s", "Prefix/Mask Len", "Change Seq No", "Change Type");

  if (osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER) == L7_FAILURE)
    return;

  pData = rtoChangeListHead;
  while (pData != L7_NULLPTR)
  {
    osapiInetNtoa(osapiNtohl(pData->network.addr), destAddrStr);
    maskLen = rtoMaskLength(osapiNtohl(pData->netmask.addr));
    sysapiPrintf("\n%15s/%d  %13u  %11s", destAddrStr, maskLen, pData->changeSeqNo,
                 rtChangeTypeStr[pData->changeType]);
    if (detail >= 2)
    {
      L7_uint32 i;
      for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
      {
        if (BestRouteClientList[i].callback)
        {
          sysapiPrintf("\n    %16s:  %5s", BestRouteClientList[i].clientName,
                       rtChangeTypeStr[pData->clientChType[i]]);
        }
      }
    }
    pData = pData->nextChange;
  }

  osapiReadLockGive(rtoRwLock);
}

/*********************************************************************
* @purpose  Print the best route client list.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void rtoClientListShow(void)
{
  L7_uint32 i;
  RtoBestRouteClient *client;
  L7_uint32 seqno;

  sysapiPrintf("\n%16s  %14s  %11s  %10s", "Client Name", "Change Pending",
               "Next Change", "Callback");
  osapiReadLockTake(rtoRwLock, L7_WAIT_FOREVER);

  for (i = 0; i < RTO_MAX_BR_CLIENTS; i++)
  {
    if (BestRouteClientList[i].callback)
    {
      client = &BestRouteClientList[i];
      seqno = 0;
      if (client->nextChange)
      {
        seqno = client->nextChange->changeSeqNo;
      }
      sysapiPrintf("\n%16s  %14s  %11u  0x%08x",
                   client->clientName, (client->changePending ? "Y" : "N"),
                   seqno, client->callback);
    }

  }
  osapiReadLockGive(rtoRwLock);
}
