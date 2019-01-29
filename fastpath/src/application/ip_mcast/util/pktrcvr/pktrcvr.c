/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pktrcvr.c

* @purpose    Packet Receiver library functions
*
* @component  Multicast System Support Utilities
*
* @comments   none
*
* @create     08-Oct-06
*
* @author     Kamlesh Agrawal
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "osapi.h"
#include "string.h"
#include "l7_socket.h"
#include "osapi_support.h"
#include "buff_api.h"
#include "l7_ip6_api.h"
#include "heap_api.h"
#include "pktrcvr.h"
#include "l7_mgmd_api.h"
#include "l7_pimdm_api.h"
#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"
#include "mcast_v6.h"
#include "mcast_debug.h"

#include <stdio.h>

#define MCAST_MAX_PDU 4096  /* Need to come up with this number */
#define PKT_RCVR_TIMEOUT 5  /* This is to allow modules to come in and go randomly */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Creation of a Packet Receiver
*
* @param    evCompId      @b{(input)}Component ID of the application calling 
*                                    this function.
* @param    maxPktFds     @b{(input)}Maximum number of FDs the application 
*                                    is intersted on listening                 
*
* @returns  pointer to the packet receiver structure, if success
* @returns  L7_NULLPTR, if failure
*
* @notes    This API is provided to create a packet receiver for a  
*           component.  This function is re-entrant.  
*
* @end
*
*********************************************************************/
extern void*
pktRcvrCreate (L7_uint32 componentId, L7_uint32 maxPktFds)
{
  L7_PktRcvr_t  *pktRcvr = L7_NULLPTR;
  L7_uint32     i=0;

  /* Allocate memory for the Packet Receiver */
  if ((pktRcvr = (L7_PktRcvr_t*) osapiMalloc (componentId, sizeof (L7_PktRcvr_t) + 
                                             (maxPktFds * sizeof(L7_fdList_t))))
                                             == L7_NULLPTR)
  {
    return (L7_NULLPTR);
  }

  /* Clear the packet receiver memory */
  memset(pktRcvr, 0, sizeof (L7_PktRcvr_t));

  pktRcvr->componentId = componentId;

  /*Store the maxPktFd */
  pktRcvr->maxPktFds = maxPktFds;
  
  /*Initialize all the FDs with -1 */
  for(i=0;i< maxPktFds;i++)
  {
     (pktRcvr->fdList[i]).pktSockFd = -1;
  }
  return (void *)pktRcvr;
} 

/*********************************************************************
*
* @purpose  Deletion of an Packet reciever
*
* @param    pktRcvr   @b{(input)}Data structure for this   
*                                pkt rcvr deletion request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to destroy packet receiver for a  
*           component.  This function is re-entrant.
*
* @end
*
*********************************************************************/
extern L7_RC_t
pktRcvrDestroy (L7_PktRcvr_t** pktRcvr)

{
  osapiFree ((*pktRcvr)->componentId, *pktRcvr);
  *pktRcvr = L7_NULLPTR;

  return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Free the Packet Receiver IPv6 Buffer 
*
* @param    fdList  @b{(input)} FD List of the component    
* @param    buffer  @b{(input)} Buffer to be freed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*
*********************************************************************/
static L7_RC_t
pktRcvrBufferFree (L7_fdList_t *fdList, L7_uchar8 *buffer)
{
  if ((fdList == L7_NULLPTR) || (buffer == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (fdList->bufPool != L7_NULL)
  {
    bufferPoolFree(fdList->bufPool, buffer);
  }
  else
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,
                     "PktRcvr: Invalid Component ID (%d) trying to free IPv6 "
                     "PktRcvr Pkts", fdList->id );
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Receiving an event with the Packet Receiver                
*
* @param    L7_PktRcvr_t  pktRcvr   Data structure for this   
*                                   Packet Receiver
*                         eventMsg  The Msg to be posted
*                                   to the component
*                         blockFlag Specifies whether the
*                                   receiving from the Pipe
*                                   is a blocking call
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to receive an event and dequeue  
*           the corresponding message from the Packet Receiver's
*           Message List and invoking the function pointer
*           corresponding to the received event.  The eventMsg
*           that is returned has to be freed by the caller.
*
* @end
*********************************************************************/
extern L7_RC_t pktRcvrReceive (L7_PktRcvr_t *pktRcvr)
{
  L7_long32         evTimeSec = 0, evTimeMSec = 0;
  L7_int32          retSel;
  L7_uint32         ii=0;
  L7_PktRcvrMsg_t   pEventMsg;
  L7_uchar8*        recvBuf = L7_NULLPTR;
  L7_uchar8         buffer[L7_MULTICAST_MAX_IP_MTU];
  L7_uint32         inBytes = L7_MULTICAST_MAX_IP_MTU;
  L7_sockaddr_in6_t sockFrom, sockTo;
  L7_uint32         fromLen, toLen;
  L7_uint32         rtrIfNum, intIfNum, hops, flags = L7_NULL;
  L7_RC_t           retVal = L7_FAILURE;
  L7_BOOL           rtrAlert;

  while(1)
  {
    /* Initialize the Read FD */
    FD_ZERO (&pktRcvr->evRdFds);

    for(ii=0;ii < pktRcvr->maxPktFds;ii++)
    {
      if((pktRcvr->fdList[ii]).pktSockFd != -1)
      {
         FD_SET((pktRcvr->fdList[ii]).pktSockFd,&pktRcvr->evRdFds);
         MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_PKT_RCVR, 
           "FD_SET is TRUE for id =%d\n",
                        (pktRcvr->fdList[ii]).id);
      }
    }

    /* L7_NULL specifies infinite timeout */
    evTimeSec  = PKT_RCVR_TIMEOUT;
    evTimeMSec = L7_NULL;

    /* Block for the event at the Socket as specified by the timeval */
    retSel = osapiSelect (pktRcvr->fds_size + 1, &pktRcvr->evRdFds, NULL, NULL,
                          evTimeSec, evTimeMSec); 

    /* Act according to the return value of select.
     * ZERO (0) indicates select() has timed out.
     * NEGATIVE (-1) indicates select() has return ERROR.
     * ONE (1) indicates select() is success.
     */
    if ((retSel == 0) || (retSel == -1))
    {
      /* Problem in receiving.  Nothing much we can do here.
       * Return back.
       */
      /* Update stats and do a debug trace */
      return (L7_FAILURE);
    }
    
   /*Loop through all the socket fds to check if the FD has been set */
    for(ii =0 ;ii <pktRcvr->maxPktFds;ii++)
    {
      recvBuf = L7_NULLPTR;
      flags = L7_NULL;
      if(((pktRcvr->fdList[ii]).pktSockFd != -1) &&
         (FD_ISSET((pktRcvr->fdList[ii]).pktSockFd,&pktRcvr->evRdFds)))
      {
        /* Replace the following with a debug trace */
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_PKT_RCVR, "Recd an IPv6 pkt for proto %d\n",
                        (pktRcvr->fdList[ii]).id);

        memset(&sockFrom, 0, sizeof(L7_sockaddr_in6_t));
        memset(&sockTo, 0, sizeof(L7_sockaddr_in6_t));
        fromLen = toLen = sizeof(L7_sockaddr_in6_t);
        memset(&pEventMsg,0, sizeof(L7_PktRcvrMsg_t));
        pEventMsg.event_type = (pktRcvr->fdList[ii]).evType;

        if ((pktRcvr->fdList[ii]).bufPool != L7_NULL)
        {
          if (bufferPoolAllocate((pktRcvr->fdList[ii]).bufPool, (L7_uchar8**) &recvBuf)
                               != L7_SUCCESS)
          {
            /* Buffer pool for this particular FD is FULL. Just discard the incoming packet */
  
            if (osapiPktInfoRecv((pktRcvr->fdList[ii]).pktSockFd,
                                  buffer,
                                  &inBytes, &flags, 
                                  (L7_sockaddr_t *)(&sockFrom), &fromLen,
                                  (L7_sockaddr_t *)(&sockTo), &toLen, 
                                  &intIfNum, &hops, NULL)
                               != L7_SUCCESS)
            {
              MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "osapiPktInfoRecv Failed; "
                               " FD-%d", (pktRcvr->fdList[ii]).pktSockFd);
            }
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "PktRcvr: Failed to allocate "
                             "buffer for component ID (%d) for IPv6 Pkts",
                             (pktRcvr->fdList[ii]).id);
            return (L7_FAILURE);
          }
        }
        else
        {
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "PktRcvr: Failed to allocate "
                           "buffer for component ID (%d) for IPv6 Pkts",
                           (pktRcvr->fdList[ii]).id);
          return L7_FAILURE;
        }
        if(recvBuf == L7_NULLPTR)
        {
          /* Buffer pool for this particular FD is FULL. Just discard the incoming packet */
          if (osapiPktInfoRecv((pktRcvr->fdList[ii]).pktSockFd,
                                buffer,
                                &inBytes, &flags, 
                                (L7_sockaddr_t *)(&sockFrom), &fromLen,
                                (L7_sockaddr_t *)(&sockTo), &toLen, 
                                &intIfNum, &hops, NULL)
                             != L7_SUCCESS)
          {
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "osapiPktInfoRecv Failed; "
                             " FD-%d", (pktRcvr->fdList[ii]).pktSockFd);
          }
          return (L7_FAILURE);
        }

        memset(recvBuf, 0, L7_MULTICAST_MAX_IP_MTU);
        /* Memory is available. Recv. the incoming packet
         * and notify the FD owner */
        if (osapiPktInfoRecv((pktRcvr->fdList[ii]).pktSockFd,
                             recvBuf,
                             &inBytes, &flags, 
                             (L7_sockaddr_t *)(&sockFrom), &fromLen,
                             (L7_sockaddr_t *)(&sockTo), &toLen, 
                             &intIfNum, &hops, &rtrAlert) != L7_SUCCESS)
        {
          /* Update some stats if required abt failure in osapi packet recv */
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "osapiPktInfoRecv Failed; "
                           " FD-%d", (pktRcvr->fdList[ii]).pktSockFd);
          pktRcvrBufferFree (&pktRcvr->fdList[ii], recvBuf);
          return (L7_FAILURE);
        }

        if(mcastIp6MapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
        {
          /* Update some stats if required abt IPv6 packets recd. on 
           * BAD interfaces */
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "\nIfaceConversionFail: id = %d ,poolId = %d, buffer = %p \n", 
                          (&pktRcvr->fdList[ii])->id, (&pktRcvr->fdList[ii])->bufPool, recvBuf);
          pktRcvrBufferFree (&pktRcvr->fdList[ii], recvBuf);
          return L7_FAILURE;
        }
        pEventMsg.event_type = (pktRcvr->fdList[ii]).evType;

        pEventMsg.recvBuf.family = L7_AF_INET6;
        pEventMsg.recvBuf.rtrIfNum = rtrIfNum;
        pEventMsg.recvBuf.length = inBytes;
        inetAddressSet(L7_AF_INET6, &(sockFrom.sin6_addr), &(pEventMsg.recvBuf.srcAddr));
        inetAddressSet(L7_AF_INET6, &(sockTo.sin6_addr), &(pEventMsg.recvBuf.destAddr));
        pEventMsg.recvBuf.payLoad = recvBuf;
        pEventMsg.recvBuf.numHops = hops;
    	pEventMsg.recvBuf.ipRtrAlert = rtrAlert;

        /* This check is done as currently no mcast protocols process loop-back packets. */
        if (inetIsLocalAddress(&(pEventMsg.recvBuf.srcAddr), rtrIfNum) == L7_TRUE)
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "Packet received with local sourceAddr on iface = %d address ", rtrIfNum);
          pktRcvrBufferFree (&pktRcvr->fdList[ii], recvBuf);
          return L7_FAILURE;
        }

        switch ((pktRcvr->fdList[ii]).id)
        {
          case L7_IPV6_PKT_RCVR_ID_MGMD:
            retVal = mgmdMapComponentCallback(L7_AF_INET6, (pktRcvr->fdList[ii]).evType, 
                                            sizeof(mcastControlPkt_t), 
                                            (void *)&(pEventMsg.recvBuf));
            break;
          case L7_IPV6_PKT_RCVR_ID_PIMDM:
            retVal = pimdmMapEventChangeCallback (L7_AF_INET6, (pktRcvr->fdList[ii]).evType, 
                                                sizeof(mcastControlPkt_t), 
                                                (void *)&(pEventMsg.recvBuf));
            break;
          case L7_IPV6_PKT_RCVR_ID_PIMSM:
            retVal = pimsmMapCommonCallback(L7_AF_INET6, (pktRcvr->fdList[ii]).evType, 
                                             sizeof(mcastControlPkt_t), 
                                             (void *)&(pEventMsg.recvBuf));

            break;
          case L7_IPV6_PKT_RCVR_ID_MFC:
              retVal = mcastMapProtocolMsgSend((pktRcvr->fdList[ii]).evType,
                                              (void *)&(pEventMsg.recvBuf), 
                                              sizeof(mcastControlPkt_t));                                               
            break;
          default:
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Packet Receiver: Invalid Protocol.\n");
            break;
        }

        if (retVal != L7_SUCCESS)
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "\n CallbackFail: id = %d ,poolId = %d, buffer = %p \n",
                          (&pktRcvr->fdList[ii])->id, (&pktRcvr->fdList[ii])->bufPool, 
                          recvBuf);
          pktRcvrBufferFree (&pktRcvr->fdList[ii], recvBuf);
          return L7_FAILURE;
        }

        return L7_SUCCESS; 
      }/*end of if */
    }/*end of for */
   /*Code Ends */
  } /* FOREVER */
} 

/*********************************************************************
*
* @purpose  Addition of a Packet FD to the Packet Receiver 
*
* @param    sockFd      @b{(input)} Socket FD on which the application wants to 
*                                   poll for control packets   
* @param    eventType   @b{(input)} Event Type the application is interested in                 
* @param    buffPoolID  @b{(input)} Buffer pool to be used for pkt receive
* @param    protocol    @b{(input)} Protocol to send the message to
* @param    pktRcvr     @b{(input)} Data structure for the packer receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to add socket Fds in the packet receiver's structure.
            The fdlist is a sorted one.FD with highest value is the 1st element.  
*
* @end
*
*********************************************************************/
L7_RC_t
pktRcvrSocketFdRegister (L7_int32 sockFd, L7_uint32 evTypeId, L7_uint32 bufPoolId,
                         L7_IPV6_PKT_RCVR_ID_TYPE_t id, L7_PktRcvr_t *pktRcvr)
{
 
  if (id >= L7_IPV6_PKT_RCVR_ID_MAX)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
      "Failed to register, invalid  Id= %d\n", id );
    return L7_FAILURE;
  }
  if(L7_NULLPTR == pktRcvr)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
       "Failed to register, invalid  pktRcvr ptr" );
    return L7_FAILURE;
  }

  (pktRcvr->fdList[id]).pktSockFd = sockFd;
  (pktRcvr->fdList[id]).evType = evTypeId;
  (pktRcvr->fdList[id]).bufPool = bufPoolId;
  (pktRcvr->fdList[id]).id = id;

  if (pktRcvr->fds_size < sockFd) 
  {
      pktRcvr->fds_size = sockFd;
  }


 MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, 
       "Success to register pktRcvr with id = %d", id );
 return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deletion of a Packet FD from the Packet Receiver
*
* @param    sockFd  @b{(input)}Socket FD to be removed
* @param    pktRcvr @b{(input)}Data structure for the packet Receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to remove a socket Fd from the packet 
*           receiver structure.
*
* @end
*
*********************************************************************/
L7_RC_t 
pktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_TYPE_t id, L7_PktRcvr_t *pktRcvr)
{

  if (id >= L7_IPV6_PKT_RCVR_ID_MAX)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
      "Failed to de-register, invalid  Id= %d\n", id );
    return L7_FAILURE;
  }
  if(L7_NULLPTR == pktRcvr)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
       "Failed to de-register, invalid  pktRcvr ptr" );
    return L7_FAILURE;
  }
  memset(&(pktRcvr->fdList[id]),0,sizeof(L7_fdList_t));
  (pktRcvr->fdList[id]).pktSockFd = -1;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, 
       "Success to de-register pktRcvr with id = %d", id );
  return L7_SUCCESS;
}

