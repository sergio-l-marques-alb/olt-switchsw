/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dhcps_main.c
*
* @purpose    DHCP Server Implementation
*
* @component  DHCP Server Component
*
* @comments   none
*
* @create     09/15/2003
*
* @author     athakur
* @end
*
**********************************************************************/
#define L7_MAC_ENET_BCAST                    /* For broadcast address */


#include "commdefs.h"
#include "l7_packet.h"
#include "l7_socket.h"
#include "l7_icmp.h"
#include "l7_dhcpsinclude.h"
#include "l7utils_inet_addr_api.h"
#include "wireless_api.h"

extern dhcpsMapCtrl_t   dhcpsMapCtrl_g;
extern L7_dhcpsMapCfg_t *pDhcpsMapCfgData;
extern L7_dhcpsMapLeaseCfg_t *pDhcpsMapLeaseCfgData;
extern dhcpsInfo_t      *pDhcpsInfo;
extern dhcpsAsyncMsg_t  *pDhcpsAsyncInfo;
extern void dhcpsMapDebugShow(void);

L7_netBufHandle netBufHandle;
dhcpsSendMessageNotifyList_t sendTypeInterestedCIds[L7_LAST_COMPONENT_ID];
L7_uint32 dhcpsSocketS;

static L7_uint32    pingSockFd = 0;
static struct icmp  icmpMsg;

/*********************************************************************
* @purpose  Actual DHCP Server Task
*
*
* @param
*
* @returns

*
* @notes    none
*
* @end
*********************************************************************/

void dhcps_Task(void)
{
  dhcpsMapMsg_t     dhcpsMsg;
  L7_uint32         status;
  dhcpsLeaseNode_t  *pExpiredLease;
  L7_uint32         lastCfgSaveTicks;

  osapiTaskInitDone(L7_DHCPS_TASK_SYNC);

  /* set last save time */
  lastCfgSaveTicks = simSystemUpTimeGet();

  while(1)
  {
    /* zero the message */
    memset(&dhcpsMsg, 0, sizeof(dhcpsMapMsg_t));

    /*  Get a Message From The Queue */
    status = osapiMessageReceive(dhcpsMap_Queue,
                                 &dhcpsMsg,
                                 sizeof(dhcpsMapMsg_t),
                                 DHCPS_QUEUE_WAIT_INTERVAL);

    /* lock dhcps data */
    osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

    if(status == L7_SUCCESS)
        processDhcpsPacket((dhcpsMapMsg_t *)&dhcpsMsg);

    /* poll scheduled leases for expiry */
    while(dhcpsGetExpiredLease(&pExpiredLease) == L7_SUCCESS)
    {
      switch(LEASE_DATA(pExpiredLease)->state)
      {
      case OFFERED_LEASE:
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                      "dhcps_Task: Deallocating expired offer for %x\n",
                      LEASE_DATA(pExpiredLease)->ipAddr);
        dhcpsDeallocateLease(&pExpiredLease);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        break;

      case ACTIVE_LEASE:
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                      "dhcps_Task: Active lease for %x expired\n",
                      LEASE_DATA(pExpiredLease)->ipAddr);
        dhcpsTransitionLeaseState(pExpiredLease, EXPIRED_LEASE);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        break;

      default:
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                      "dhcps_Task: Expired lease for %x is in unknown state %d\n",
                      LEASE_DATA(pExpiredLease)->ipAddr, LEASE_DATA(pExpiredLease)->state);
      }
    }

    /* save cfg if required */
    if( 0 != L7_DHCPS_CFG_SAVE_INTERVAL_SECS &&
       (simSystemUpTimeGet() - lastCfgSaveTicks) > L7_DHCPS_CFG_SAVE_INTERVAL_SECS)
    {
       if(L7_TRUE == pDhcpsMapLeaseCfgData->cfgHdr.dataChanged)
       {
         DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Saving lease config data\n");

         /* save config data */
         if( dhcpsSaveLeaseCfg(__FILE__, __LINE__) == L7_SUCCESS)
           pDhcpsMapLeaseCfgData->cfgHdr.dataChanged = L7_FALSE;
       }

       /* wait another xx */
       lastCfgSaveTicks = simSystemUpTimeGet();
     }

    /* release dhcp data lock */
    osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;

  }   /* End for while (1) */
}

/*********************************************************************
* @purpose  Set the lease state and take appropriate packet send action
*
* @param    pLease        @b{(input)} Pointer to the lease node
* @param    pDhcpPacket   @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} Length of dhcp packet
* @param    intIfNum      @b{(input)} Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dhcpsHandleLease(dhcpsLeaseNode_t *pLease, dhcpsPacket_t *pDhcpPacket,
                         L7_uint32 dhcpPacketLength, L7_uint32 incomingIntfNum)
{
  L7_BOOL          isDhcpPacket = L7_FALSE;
  L7_uint32        subnetMask = 0, requestedIPAddr = 0, leaseTime = 0, serverID = 0;
  L7_uint32        messageType = 0, optionOverload = 0;
  L7_uchar8        clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8        clientIdLen = 0;
  dhcpsOption_t    *pParameterList= L7_NULLPTR;
  dhcpsOption_t    *option82 = NULL;

  memset(clientIdentifier, 0, sizeof(clientIdentifier));
  if (dhcpsParseDhcpPacket(pDhcpPacket, dhcpPacketLength, &isDhcpPacket,
                           clientIdentifier, &clientIdLen, &messageType,
                           &optionOverload, &subnetMask, &requestedIPAddr,
                           &leaseTime, &serverID, &pParameterList, &option82) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to parse DHCP packet");
    return L7_FAILURE;
  }

  if(L7_FALSE == isDhcpPacket)
  {
    /* BOOTP packet */

    LEASE_DATA(pLease)->IsBootp = L7_TRUE;

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process BOOTP REQUEST set lease for addr %x ACTIVE\n", LEASE_DATA(pLease)->ipAddr);

    dhcpsTransitionLeaseState(pLease, ACTIVE_LEASE);
    dhcpsSendBootpReply(pLease, pDhcpPacket, incomingIntfNum);

    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
  }
  else
  {
    /* DHCP packet */
    if(LEASE_DATA(pLease)->state == EXPIRED_LEASE)
    {
      /* set lease time */
      LEASE_DATA(pLease)->leaseTime = POOL_DATA(pLease->parentPool)->leaseTime;
      if((leaseTime >= (L7_DHCPS_LEASETIME_MIN *60)) &&
         (leaseTime < (L7_DHCPS_LEASETIME_MAX * 60)))
      {
        LEASE_DATA(pLease)->leaseTime = leaseTime;
      }
      LEASE_DATA(pLease)->leaseExpiration = simSystemUpTimeGet() + (LEASE_DATA(pLease)->leaseTime);

      if(LEASE_DATA(pLease)->ipAddr == requestedIPAddr)
      {
        /* Requested IP for a matching EXPIRED LEASE is same as the Lease's IP.
         * Send Offer.  */
      }
      else if(requestedIPAddr != 0)
      {
        /* Requested IP for a matching EXPIRED LEASE is different from the Lease's IP.
         * Try to create a new lease for this requested IP.  */

        /* excluded? */
        if(dhcpsCheckIfExcluded(pLease->parentPool, requestedIPAddr, L7_TRUE) == L7_TRUE)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
            "Ignoring discover requested IP option (excluded IP)\n");

          /* This case is never touched but return from here to be clean and safe */
          return L7_SUCCESS;
        }
        else
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "process DISCOVER allocating lease for requested IP\n");
          if(dhcpsCreateFreeLease(pLease->parentPool, requestedIPAddr,
                                  clientIdentifier, clientIdLen, pDhcpPacket,
                                  dhcpPacketLength, leaseTime, incomingIntfNum, L7_FALSE) != L7_SUCCESS)
          {
            DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
              "process DISCOVER unable to create lease for requested IP %x\n",
              requestedIPAddr);
          }
          else
          {
            /* Successfully posted the "to be offered address" to the ping async task.
             * Return from here */
            return L7_SUCCESS;
          }
        }
      }
      else
      {
        /* No requested IP for a matching EXPIRED LEASE. Just offer the ip
         * in the expired lease.  */
      }
    }

    /* transition lease to offered */
    dhcpsTransitionLeaseState(pLease, OFFERED_LEASE);
    dhcpsSendOfferMessage(pLease, pDhcpPacket, incomingIntfNum, pParameterList, option82);

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "process DISCOVER Offered Fresh IP %x\n", LEASE_DATA(pLease)->ipAddr);
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for the reachability of the next free ip
*
* @param    pPool        @b{(input)} Pointer to the pool node
* @param    index        @b{(input)} index into async array
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t dhcpsTryNextFreeIp(dhcpsPoolNode_t *pPool, L7_uint32 index)
{
  L7_RC_t          rc = L7_FAILURE;
  L7_BOOL          isDhcpPacket = L7_FALSE;
  L7_uint32        subnetMask = 0, requestedIPAddr = 0, leaseTime = 0, serverID = 0;
  L7_uint32        messageType = 0, optionOverload;
  L7_uchar8        clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8        clientIdLen = 0, *clientId = L7_NULLPTR;
  dhcpsOption_t    *pParameterList= L7_NULLPTR;
  dhcpsOption_t    *option82 = NULL;

  L7_uint32        ipAddr        = pDhcpsAsyncInfo[index].ipAddr;
  L7_uint32        getNextFreeIp = pDhcpsAsyncInfo[index].getNextFreeIp;

  clientId = clientIdentifier;

  pDhcpsAsyncInfo[index].ipAddr = 0; /* = 0 to create a slot before trying for next free ip */

  /* Try next free ip, if the flag getNextFreeIp is set */
  if(getNextFreeIp == L7_TRUE)
  {
    if (dhcpsParseDhcpPacket((dhcpsPacket_t *)pDhcpsAsyncInfo[index].dhcpPacketData,
                             pDhcpsAsyncInfo[index].dhcpPacketLength, &isDhcpPacket,
                             clientIdentifier, &clientIdLen, &messageType,
                             &optionOverload, &subnetMask, &requestedIPAddr,
                             &leaseTime, &serverID, &pParameterList, &option82) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
             "Failed to parse DHCP packet.");
      return L7_FAILURE;
    }

    if(L7_FALSE == isDhcpPacket)
    {
      /* For BOOTP REQUEST packet */
      clientIdLen = 0;
      clientId    = 0;
    }
    while(dhcpsGetFreeIpAddr(pPool, &ipAddr) == L7_SUCCESS)
    {
      rc = dhcpsCreateFreeLease(pPool, ipAddr, clientId, clientIdLen,
                                (dhcpsPacket_t *)pDhcpsAsyncInfo[index].dhcpPacketData,
                                pDhcpsAsyncInfo[index].dhcpPacketLength, leaseTime,
                                pDhcpsAsyncInfo[index].intIfNum, L7_TRUE);
      if((rc == L7_SUCCESS) || (rc == L7_TABLE_IS_FULL))
      {
        break;
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  DHCP Task that sends and processes ping packets for
*           rechability detection.
*
* @param    none
*
* @returns

*
* @notes    This task uses select to schedule sending echo requests and
*           receiving the echo responses for all the DISCOVER messages
*           that are asynchronously processed for the availability of the
*           ip address in the network.
*           If select returns with a timeout (timeout is 200 msec), for
*           each DISCOVER message, we send the next echo request with
*           increased seqno till the number of pings count. If the
*           ping timeout for the ipaddress is reached, we offer that
*           ip address to the client.
*           If select returns with a read event, might be we received an
*           echo response for one of the previously sent echo requests.
*           If we get a matching response from a host, mark that lease
*           as abandoned and repeat the ping with the next free lease.
*
* @end
*********************************************************************/
void dhcpsPingTask(void)
{
  fd_set readFds;
  L7_sockaddr_t     rcvAddr;
  L7_uchar8         pingBuf[DHCPS_PING_MAX_RCV_LEN];
  L7_uint32         srcAddr, rcvAddrLen, dataLen;
  L7_uint32         fd_size, rc, pingPktNoConfigured, i;
  L7_ipHeader_t     *ipHdr;
  L7_ushort16       ipLen;
  struct icmp       *icmpPtr;
  dhcpsLeaseNode_t  *pLease = L7_NULLPTR;

  for(;;)
  {
    FD_ZERO(&readFds);
    FD_SET(pingSockFd, &readFds);
    fd_size = pingSockFd + 1;

    rc = osapiSelect(fd_size, &readFds, L7_NULL, L7_NULL, 0, DHCPS_PING_SELECT_TIMEOUT * 1000);

    if(rc < 0)
    {
      /* select error, try select again */
      continue;
    }

    /* lock dhcps data */
    osapiSemaTake(dhcpsMapCtrl_g.dhcpsDataSemId, L7_WAIT_FOREVER);

    pingPktNoConfigured = pDhcpsMapCfgData->dhcps.dhcpsPingPktNo;

    if(rc == 0)
    {
      /* select timed out */
      for(i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
      {
        if(pDhcpsAsyncInfo[i].ipAddr == 0)
        {
          continue;
        }
        if(pDhcpsAsyncInfo[i].pingsIssued < pingPktNoConfigured)
        {
          pDhcpsAsyncInfo[i].pingsIssued++;
        }
        pDhcpsAsyncInfo[i].pingTimeoutCnt--;

        if(pDhcpsAsyncInfo[i].pingTimeoutCnt == 0)
        {
          /* Ping operation for this ipaddress timed out. Send Offer message */
          if(dhcpsFindLeaseNodeByIpAddr(pDhcpsAsyncInfo[i].ipAddr, &pLease) == L7_SUCCESS)
          {
            dhcpsHandleLease(pLease, (dhcpsPacket_t *)pDhcpsAsyncInfo[i].dhcpPacketData,
                             pDhcpsAsyncInfo[i].dhcpPacketLength, pDhcpsAsyncInfo[i].intIfNum);
          }

          /* clear the async message */
          pDhcpsAsyncInfo[i].ipAddr = 0;
        }
        else if(pDhcpsAsyncInfo[i].pingsIssued < pingPktNoConfigured)
        {
          dhcpsSendEchoRequest(pDhcpsAsyncInfo[i].ipAddr, pDhcpsAsyncInfo[i].pingsIssued);
        }
      }
    }
    else
    {
      if(! FD_ISSET(pingSockFd, &readFds))
      {
        continue;
      }

      /* We might have received echo response to one of our echo requests.
       * We go with an assumtion that select would have unblocked immediately without any wait.
       * Read event can still happen when select is halfway through its wait */

      rcvAddrLen = sizeof(rcvAddr);
      if(osapiSocketRecvfrom(pingSockFd, pingBuf, DHCPS_PING_MAX_RCV_LEN, 0,
                             &rcvAddr, &rcvAddrLen, &dataLen) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
               "Failed to receive Echo responses.");
        continue;
      }

      ipHdr = (L7_ipHeader_t *)pingBuf;
      ipLen = (ipHdr->iph_versLen & 0xf) * 4;
      icmpPtr = (struct icmp *)(pingBuf + ipLen);

      srcAddr = osapiNtohl(ipHdr->iph_src);

      if((icmpPtr->icmp_type == DHCPS_ICMP_ECHOREPLY) &&
         (icmpPtr->icmp_id == osapiHtons(DHCPS_PING_TEST_ID)))
      {
        for(i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
        {
          if(pDhcpsAsyncInfo[i].ipAddr == srcAddr)
          {
            if(dhcpsFindLeaseNodeByIpAddr(pDhcpsAsyncInfo[i].ipAddr, &pLease) == L7_SUCCESS)
            {
              if(L7_TRUE == pDhcpsMapCfgData->dhcps.dhcpConflictLogging)
              {
                dhcpsTransitionLeaseState(pLease, ABANDONED_LEASE);
                LEASE_DATA(pLease)->conflictDetectionMethod = L7_DHCPS_PING;
              }

              if(LEASE_DATA(pLease)->state == EXPIRED_LEASE)
              {
                /* Lease corresponding to the Client key is an expired one.
                 * And the ip address of the lease is reachable, abandon this
                 * lease and try creating new free lease and process it */
                pDhcpsAsyncInfo[i].getNextFreeIp = L7_TRUE;
                pDhcpsAsyncInfo[i].ipAddr        = 0;
              }

              if(dhcpsTryNextFreeIp(pLease->parentPool, i) != L7_SUCCESS)
              {
                DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "No free IP address to offer in pool %x\n",
                              POOL_DATA(pLease->parentPool)->ipAddr);
              }
            }
            break;
          }
        }
      }
    }

    /* release dhcp data lock */
    osapiSemaGive(dhcpsMapCtrl_g.dhcpsDataSemId) ;
  }   /* end of for (;;) */
}

/*********************************************************************
* @purpose  Process Bootp/Dhcp request coming Server and
*           Bootp/Dhcp reply back to Client
*
* @param    msg  @b{(input)} Actual Dhcp Message arrived from interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t processDhcpsPacket(dhcpsMapMsg_t *msg)
{
  L7_uint32       intIfNum;
  L7_BOOL         isBroadcast;
  dhcpsPacket_t  *pDhcpPacket      = L7_NULLPTR;
  L7_uint32       dhcpPacketLength = 0;

  /* Get the interface number */
  intIfNum          = osapiNtohl(msg->intf);
  pDhcpPacket       = (dhcpsPacket_t *)msg->dhcpPacketData;
  dhcpPacketLength  = osapiNtohl(msg->length);
  isBroadcast       = (osapiNtohl(msg->destIp) == L7_IP_LTD_BCAST_ADDR)
                      ? L7_TRUE
                      : L7_FALSE;

  /* Ensure DHCP Server mode is enabled */
  if (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_DISABLE)
  {
    pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
  }
  else if (pDhcpsMapCfgData->dhcps.dhcpsAdminMode == L7_ENABLE)
  {
    if(L7_NULLPTR == pDhcpPacket)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO, "processDhcpsPacket discarded NULL packet buffer\n");
      pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
    }
    else if((pDhcpPacket->header.op) != DHCPS_BOOTPREQUEST)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO, "processDhcpsPacket discarded Bootp reply\n");
      pDhcpsInfo->dhcpsStats.numOfMalformedMessages++;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO, "BOOTPREQUEST received %u bytes\n",dhcpPacketLength);

      /* process in detail */
      if(processDhcpsMessages(pDhcpPacket, dhcpPacketLength, intIfNum, isBroadcast) != L7_SUCCESS)
      {
        pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Parse DHCP packet contents with options
*
* @param    pDhcpPacket       @b{(input)} Pointer to the Dhcp packet received
* @param    dhcpPacketLength  @b{(input)} Length of the Dhcp packet
* @param    isDhcpPacket      @b{(output)} If packet is bootp or dhcp
* @param    clientId          @b{(output)} Client Identifier
* @param    clientIdLen       @b{(output)} Length of client identifier
* @param    messageType       @b{(output)} Message Type
* @param    optionOverload    @b{(output)} Overload option
* @param    subnetMask        @b{(output)} Subnet Mask option
* @param    requestedIPAddr   @b{(output)} IP address option
* @param    leaseTime         @b{(output)} Requested lease time
* @param    serverID          @b{(output)} ServerID
* @param    ppParameterList   @b{(output)} parameter list
* @param    option82          @b{(output)} start of option 82, if included
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsParseDhcpPacket(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                             L7_BOOL *isDhcpPacket,
                             L7_uchar8 *clientId, L7_uchar8 *clientIdLen,
                             L7_uint32 *messageType, L7_uint32 *optionOverload,
                             L7_uint32 *subnetMask, L7_uint32 *requestedIPAddr,
                             L7_uint32 *leaseTime, L7_uint32 *serverID,
                             dhcpsOption_t **ppParameterList,
                             dhcpsOption_t **option82)
{
  L7_int32         msgOptionsLength;
  L7_uchar8        *pOptionBuf;
  dhcpsOption_t    *pOption;
  L7_uint32        optionLen;

  if (!pDhcpPacket || !isDhcpPacket || !clientId || !clientIdLen ||
      !messageType || !optionOverload || !subnetMask || !requestedIPAddr ||
      !leaseTime || !serverID || !ppParameterList || !option82)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Invalid arguments are specified for DHCP packet parsing.");
    return L7_FAILURE;
  }

  if (dhcpPacketLength < sizeof(dhcpsHeader_t))
  {
    pDhcpsInfo->dhcpsStats.numOfMalformedMessages++;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "dhcpsParseDhcpPacket found malformed msg with Bad Packet Length\n");
    return L7_FAILURE;
  }

  memset(clientId, 0, L7_DHCPS_CLIENT_ID_MAXLEN);
  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "In dhcpsParseDhcpPacket function\n");

  /* Validate the options and process the option contents.*/
  msgOptionsLength = dhcpPacketLength - sizeof(dhcpsHeader_t) - DHCPS_MAGIC_COOKIE_LENGTH;

  pOptionBuf   = pDhcpPacket->options;
  pOption      = (dhcpsOption_t *) pOptionBuf;
  optionLen    = pOption->length + 2; /* 2 for type and length bytes */

  while(msgOptionsLength > 0)
  {
    switch(pOption->type)
    {
      case DHCPS_OPTION_TYPE_PAD:
        optionLen = 1;
        break;

      case DHCPS_OPTION_TYPE_END:
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "END option, remaining extra bytes %d\n", msgOptionsLength);
        optionLen        = 1;
        msgOptionsLength = 0;
        break;

      case DHCPS_OPTION_TYPE_MESSAGE:
        *isDhcpPacket = L7_TRUE;
        *messageType = pOption->value[0];
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "dhcpsParseDhcpPacket recd msg with Message Type = %d\n",*messageType);
        break;

      case DHCPS_OPTION_TYPE_OPTION_OVERLOAD:
        *optionOverload = pOption->value[0];
        break;

    case DHCPS_OPTION_TYPE_REQUEST_IP_ADDR:
        {
          L7_uint32 tmp;
          memcpy(&tmp,pOption->value,sizeof(tmp));
          *requestedIPAddr = osapiNtohl(tmp);
        }
        /* if requested IP address is 0.0.0.0, server will ignore and assign from pool. */
        break;

    case DHCPS_OPTION_TYPE_SUBNET_MASK:
        {
          L7_uint32 tmp;
          memcpy(&tmp,pOption->value,sizeof(tmp));
          *subnetMask = osapiNtohl(tmp);
        }

        break;

    case DHCPS_OPTION_TYPE_LEASE_TIME:
        {
          L7_uint32 tmp;
          memcpy(&tmp,pOption->value,sizeof(tmp));
          *leaseTime = osapiNtohl(tmp);
        }

        break;

    case DHCPS_OPTION_TYPE_SERVER_ID:
        {
          L7_uint32 tmp;
          memcpy(&tmp,pOption->value,sizeof(tmp));
          *serverID = osapiNtohl(tmp);
        }

        if (0 == (*serverID))
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "dhcpsParseDhcpPacket dropping message with serverID=0\n");
          pDhcpsInfo->dhcpsStats.numOfMalformedMessages++;
          return L7_FAILURE;
        }

        break;

      case DHCPS_OPTION_TYPE_CLIENT_ID:
        memcpy(clientId, pOption->value, pOption->length);
        *clientIdLen = pOption->length;
        break;

      case DHCPS_OPTION_TYPE_PARAMETER_LIST:
        *ppParameterList = (dhcpsOption_t*) &(pOption->type);
        break;

      case DHCPS_OPTION_TYPE_AGENT_INFO:
        *option82 = pOption;

      default:
        break;
    }

    /* next op */
    msgOptionsLength -= optionLen;
    pOptionBuf       += optionLen;
    pOption          = (dhcpsOption_t *) pOptionBuf;
    optionLen        = pOption->length + 2; /* 2 for type and length bytes */
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Dhcp request coming from the client and send
*           Dhcp reply back to Client
*
* @param    pDhcpPacket       @b{(input)} Pointer to the Dhcp packet received
* @param    dhcpPacketLength  @b{(input)} Length of the Dhcp packet
* @param    intIfNum          @b{(input)} Interface number
* @param    isBroadcast       @b{(input)} whether the packet has been broadcasted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t processDhcpsMessages(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                             L7_uint32 intIfNum, L7_BOOL isBroadcast)
{
  L7_BOOL          isDhcpPacket = L7_FALSE;
  L7_uint32        subnetMask = 0, requestedIPAddr = 0, leaseTime = 0, serverID = 0;
  L7_uint32        messageType = 0, optionOverload;
  L7_uchar8        clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8        clientIdLen = 0;
  dhcpsOption_t    *pParameterList= L7_NULLPTR;
  dhcpsOption_t    *option82 = NULL;      /* points to start of option 82 (type field) in received msg. */


  if (dhcpsParseDhcpPacket(pDhcpPacket, dhcpPacketLength, &isDhcpPacket,
                           clientIdentifier, &clientIdLen, &messageType,
                           &optionOverload, &subnetMask, &requestedIPAddr,
                           &leaseTime, &serverID, &pParameterList, &option82) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to parse DHCP packet");
    return L7_FAILURE;
  }

  /* send message to state machine for processing */
  if(L7_TRUE == isDhcpPacket)
  {
    if ((messageType < DHCPS_MESSAGE_TYPE_START) || (messageType > DHCPS_MESSAGE_TYPE_END))
    {
      pDhcpsInfo->dhcpsStats.numOfMalformedMessages++;
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "processDhcpsMessages found malformed msg with Erroneous Message Type %d\n",messageType);
      return L7_FAILURE;
    }

    /* Process the received message. */
    switch (messageType)
    {
     case DHCPS_MESSAGE_TYPE_DISCOVER:
       pDhcpsInfo->dhcpsStats.numOfDhcpsDiscoverReceived++;
       dhcpsProcessDiscoverMessage(requestedIPAddr, clientIdentifier, clientIdLen,
                                   leaseTime, pDhcpPacket, dhcpPacketLength,
                                   intIfNum, pParameterList, option82);
       break;

     case DHCPS_MESSAGE_TYPE_REQUEST:
       pDhcpsInfo->dhcpsStats.numOfDhcpsRequestReceived++;
       if ((pDhcpPacket->header.ciaddr != 0) && (serverID == 0) && (requestedIPAddr == 0))
       {
         /* client is in RENEWING or REBINDING  state */
         dhcpsProcessRenewRebindRequest(clientIdentifier, clientIdLen, isBroadcast,
                                        pDhcpPacket, intIfNum, pParameterList, option82);
       }
       else if ((pDhcpPacket->header.ciaddr == 0) && (serverID != 0) && (requestedIPAddr != 0))
       {
         /* client is in SELECTING state */
         dhcpsProcessSelectingRequest(requestedIPAddr, serverID, clientIdentifier,
                                      clientIdLen, pDhcpPacket, intIfNum, pParameterList,
                                      option82);
       }
       else if ((pDhcpPacket->header.ciaddr == 0) && (serverID == 0) && (requestedIPAddr != 0))
       {
         /* client is in INIT-REBOOT state */
         dhcpsProcessInitRebootRequest(requestedIPAddr, clientIdentifier, clientIdLen,
                                       pDhcpPacket, intIfNum, pParameterList, option82);
       }
       else
       {
         DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                       "Discarding DHCPREQUEST for unknown client state: ciaddr %x, reqip %x serverid %x\n",
                       osapiNtohl(pDhcpPacket->header.ciaddr),
                       requestedIPAddr, serverID);
         return L7_FAILURE;
       }

       break;

     case DHCPS_MESSAGE_TYPE_DECLINE:
        pDhcpsInfo->dhcpsStats.numOfDhcpsDeclineReceived++;
        dhcpsProcessDeclineMessage(requestedIPAddr, clientIdentifier, clientIdLen, pDhcpPacket, intIfNum);
        break;

     case DHCPS_MESSAGE_TYPE_RELEASE:
        pDhcpsInfo->dhcpsStats.numOfDhcpsReleaseReceived++;
        dhcpsProcessReleaseMessage(clientIdentifier, clientIdLen, pDhcpPacket, intIfNum);
        break;

     case DHCPS_MESSAGE_TYPE_INFORM:
        pDhcpsInfo->dhcpsStats.numOfDhcpsInformReceived++;
        dhcpsProcessInformMessage(clientIdentifier, clientIdLen, leaseTime,
                                  pDhcpPacket, intIfNum, pParameterList, option82);
        break;

     default:
       DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "processDhcpsMessages discared msg with Unknown Message Type %d\n",messageType);
       return L7_FAILURE;
    }
  }/* End of if for DHCP_PKT */
  else
  {
     pDhcpsInfo->dhcpsStats.numOfBootpOnlyReceived++;

     /* must have null sname & filename */
     if(dhcpsStrnlen(pDhcpPacket->header.sname,DHCPS_SNAME_LEN ) > 0 || dhcpsStrnlen(pDhcpPacket->header.file, DHCPS_FILE_LEN) > 0)
     {
       pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
       DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO, "processDhcpsMessages discared BOOTP msg requesting specific file/server\n");
       return L7_FAILURE;
     }

     /* process BOOTPREQUEST */
     dhcpsProcessBootpRequestMessage(pDhcpPacket, dhcpPacketLength, intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create and send the DHCPOFFER message by populating the relevant fields
*
* @param    pLease            @b{(input)} Pointer to the lease node
* @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    intIfNum          @b{(input)} Interface number
* @param    pParameterList    @b{(input)} Option Parameter List
* @param    option82          @b{(input)} Start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSendOfferMessage(dhcpsLeaseNode_t * pLease, dhcpsPacket_t * pDhcpPacket,
                              L7_uint32 intIfNum, dhcpsOption_t *pParameterList,
                              dhcpsOption_t *option82)
{

  dhcpsPacket_t *pMsg;
  L7_uchar8      buffer[L7_DHCP_PACKET_LEN];
  L7_uint32      buflen;
  L7_uchar8      *pOptionBuf = L7_NULLPTR;
  dhcpsOption_t  *pOption    = L7_NULLPTR;
  L7_IP_ADDR_t   offeredIP;
  L7_IP_ADDR_t   intfIpAddr = 0;
  L7_IP_MASK_t   mask;
  L7_uint32      longValue;
  L7_uchar8      overloadState = DHCPS_BOTH_CAN_BE_OVERLOADED;
  L7_BOOL        clientOverload;
  L7_BOOL        isSysMgmtIntf = L7_FALSE;

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "\nCreate Offer Message");

  pMsg = (dhcpsPacket_t *)buffer;
  memset(pMsg, 0, L7_DHCP_PACKET_LEN);

  buflen = sizeof(dhcpsHeader_t);

  /* Fill in the header and magic cookie */
  pMsg->header.op     = DHCPS_BOOTPREPLY;
  pMsg->header.htype  = DHCPS_HW_TYPE;
  pMsg->header.hlen   = L7_MAC_ADDR_LEN;
  pMsg->header.hops   = 0;
  memcpy(&pMsg->header.xid, &pDhcpPacket->header.xid, sizeof(pMsg->header.xid));
  memcpy(pMsg->header.chaddr, pDhcpPacket->header.chaddr, sizeof(pDhcpPacket->header.chaddr));
  memcpy(&pMsg->header.giaddr, &pDhcpPacket->header.giaddr, sizeof(pMsg->header.giaddr));
  memcpy(&pMsg->header.flags, &pDhcpPacket->header.flags, sizeof(pMsg->header.flags));

  /* Copy the siaddr field*/
  if( POOL_DATA(pLease->parentPool)->nextServer !=0)
  {
    longValue = osapiHtonl(POOL_DATA(pLease->parentPool)->nextServer);
    memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));
  }
  else
  {
    if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
           "Server Id get failure\r\n");
      return L7_FAILURE;
    }

    longValue = osapiHtonl(intfIpAddr);
    memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));
  }

  /* Offered IP will be taken from the lease structure */
  offeredIP =  osapiHtonl(LEASE_DATA(pLease)->ipAddr);
  memcpy(&pMsg->header.yiaddr, &offeredIP, sizeof(pMsg->header.yiaddr));

  longValue = osapiHtonl(DHCPS_MAGIC_COOKIE_VALUE);
  memcpy(pMsg->magicCookie, &longValue, sizeof(longValue));
  buflen += DHCPS_MAGIC_COOKIE_LENGTH;


  if( pParameterList != L7_NULLPTR)
  {
    clientOverload = dhcpsClientSupportsLongOptions(pParameterList->value,pParameterList->length);
  }
  else
  {
    clientOverload = L7_FALSE;
  }

  /* Fill in the message option (mandatory) */
  pOptionBuf = pMsg->options;

  /* message type option */
  pOption =  (dhcpsOption_t *)pMsg->options;
  pOption->type   = DHCPS_OPTION_TYPE_MESSAGE;
  pOption->length   = 1;
  pOption->value[0] = DHCPS_MESSAGE_TYPE_OFFER;
  buflen = DHCPS_OPTION_VALUE_OFFSET + 1 + buflen;

  /* server id option */
  if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "Server Id get failure\r\n");
    return L7_FAILURE;
  }
  else
  {
    if (intfIpAddr == simGetSystemIPAddr() && mask == simGetSystemIPNetMask())
    {
      isSysMgmtIntf = L7_TRUE;
    }
  }

  longValue = osapiHtonl(intfIpAddr);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_SERVER_ID, (L7_uchar8*)&longValue, sizeof(L7_IP_ADDR_t), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "Couldn't add the server Id\r\n");
    return L7_FAILURE;
  }

  /* lease time option (mandatory) */
  longValue = osapiHtonl(LEASE_DATA(pLease)->leaseTime);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_LEASE_TIME, (L7_uchar8*)&longValue, sizeof(longValue), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "Couldn't add the lease Time Id\r\n");
    return L7_FAILURE;
  }

  /* Option add function call */
  if( L7_SUCCESS != dhcpsAppendOptions(pLease->parentPool, pMsg, &buflen, pParameterList, &overloadState, clientOverload))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsAppendOptions function returns failure\r\n");
    return L7_FAILURE;
  }
  if(dhcpsAddOverloadOption(pMsg, &buflen, overloadState) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendOfferMessage : dhcpsAddOverloadOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* If client message included option 82, copy option 82 into reply. */
  if (option82)
  {
    if (dhcpsOption82Copy(option82, pMsg, &buflen) != L7_SUCCESS)
    {
      /* increment error counter. Send response w/o option 82. */
      pDhcpsInfo->dhcpsStats.opt82CopyFailed++;
    }
  }

  if(dhcpsAddSingleByteOption(DHCPS_OPTION_TYPE_END, pMsg, &buflen) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendOfferMessage : dhcpsAddSingleByteOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* Send Function */
  if(dhcpsSendPacket(pMsg, buflen, intIfNum, isSysMgmtIntf) == L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfDhcpsOfferSent++;
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Failure in sending DHCPOFFER message\n")
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create and send the DHCPACK message by populating the relevant fields
*
* @param    pLease         @b{(input)} Pointer to the lease node
* @param    pDhcpPacket    @b{(input)} Pointer to the dhcp Packet
* @param    intIfNum       @b{(input)} Interface number
* @param    pParameterList @b{(input)} Option Parameter List
* @param    option82       @b{(input)} Start of option 82 in client message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  dhcpsSendACKMessage(dhcpsLeaseNode_t *pLease,
                             dhcpsPacket_t *pDhcpPacket,
                             L7_uint32 intIfNum,
                             dhcpsOption_t *pParameterList,
                             dhcpsOption_t *option82)
{
  dhcpsPacket_t *pMsg;
  L7_uchar8      buffer[L7_DHCP_PACKET_LEN];
  L7_uint32      buflen = 0;
  L7_uchar8      *pOptionBuf = L7_NULLPTR;
  dhcpsOption_t  *pOption    = L7_NULLPTR;
  L7_IP_ADDR_t   offeredIP;
  L7_IP_ADDR_t   intfIpAddr = 0;
  L7_IP_MASK_t   mask;
  L7_uint32      longValue;
  L7_uchar8      overloadState = DHCPS_BOTH_CAN_BE_OVERLOADED;
  L7_BOOL        clientOverload;
  L7_BOOL        isSysMgmtIntf = L7_FALSE;
  L7_uint32      wlIntIfNum, tempIntIfNum = intIfNum;
  L7_uint32 index;
  L7_enetMacAddr_t chAddr;


  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Sending Ack Message\n");

  pMsg = (dhcpsPacket_t *)buffer;
  memset(pMsg, 0, L7_DHCP_PACKET_LEN);

  buflen = sizeof(dhcpsHeader_t);

  /* Fill in the header and magic cookie */
  pMsg->header.op     = DHCPS_BOOTPREPLY;
  pMsg->header.htype  = DHCPS_HW_TYPE;
  pMsg->header.hlen   = L7_MAC_ADDR_LEN;
  pMsg->header.hops   = 0;
  memcpy(&pMsg->header.xid, &pDhcpPacket->header.xid, sizeof(pMsg->header.xid));
  memcpy(pMsg->header.chaddr, pDhcpPacket->header.chaddr, sizeof(pDhcpPacket->header.chaddr));

  /* ciaddr and yiaddr */
  offeredIP =  osapiHtonl(LEASE_DATA(pLease)->ipAddr);
  memcpy(&pMsg->header.yiaddr, &offeredIP, sizeof(L7_IP_ADDR_t));
  memcpy(&pMsg->header.ciaddr, &pDhcpPacket->header.ciaddr, sizeof(L7_IP_ADDR_t));

  memcpy(&pMsg->header.giaddr, &pDhcpPacket->header.giaddr, sizeof(L7_IP_ADDR_t));
  memcpy(&pMsg->header.flags, &pDhcpPacket->header.flags, sizeof(pMsg->header.flags));

  /* In wireless case, the intIfNum needs to be re-calculated */
  if ((wtnnlFuncTable.wtnnlIntIfNumGet != L7_NULLPTR) &&
      (wtnnlFuncTable.wtnnlIntIfNumGet(pMsg->header.ciaddr, &wlIntIfNum) == L7_SUCCESS))
  {
    tempIntIfNum = wlIntIfNum;
  }

  /* Copy the siaddr field*/
  if( POOL_DATA(pLease->parentPool)->nextServer !=0)
  {
    longValue = osapiHtonl(POOL_DATA(pLease->parentPool)->nextServer);
    memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));
  }
  else
  {
    if (dhcpsGetServerID(tempIntIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
      return L7_FAILURE;

    longValue = osapiHtonl(intfIpAddr);
    memcpy(&pMsg->header.siaddr,&longValue,sizeof(pMsg->header.siaddr));
  }

  longValue = osapiHtonl(DHCPS_MAGIC_COOKIE_VALUE);
  memcpy(pMsg->magicCookie, &longValue, sizeof(longValue));
  buflen += DHCPS_MAGIC_COOKIE_LENGTH;

  pOptionBuf = pMsg->options;
  if( pParameterList != L7_NULLPTR)
  {
    clientOverload = dhcpsClientSupportsLongOptions(pParameterList->value,pParameterList->length);
  }
  else
  {
    clientOverload = L7_FALSE;
  }

  /* message type option */
  pOption =  (dhcpsOption_t*) pOptionBuf;
  pOption->type   = DHCPS_OPTION_TYPE_MESSAGE;
  pOption->length   = 1;
  pOption->value[0] = DHCPS_MESSAGE_TYPE_ACK;
  buflen  = DHCPS_OPTION_VALUE_OFFSET + 1 + buflen;

  /* server id option */
  if (dhcpsGetServerID(tempIntIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: Server Id get failure\r\n");
      return L7_FAILURE;
  }
  else
  {
    if (intfIpAddr == simGetSystemIPAddr() && mask == simGetSystemIPNetMask())
    {
      isSysMgmtIntf = L7_TRUE;
    }
  }

  longValue = osapiHtonl(intfIpAddr);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_SERVER_ID, (L7_uchar8*)&longValue, sizeof(L7_IP_ADDR_t), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: Couldn't add the server Id\r\n");
    return L7_FAILURE;
  }

  /* lease time option (mandatory) */
  longValue = osapiHtonl(LEASE_DATA(pLease)->leaseTime);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_LEASE_TIME, (L7_uchar8*)&longValue, sizeof(longValue), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: Couldn't add the lease Time Id\r\n");
    return L7_FAILURE;
  }

  /* Option add function call */
  if( L7_SUCCESS != dhcpsAppendOptions(pLease->parentPool, pMsg, &buflen, pParameterList, &overloadState, clientOverload))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: dhcpsAppendOptions function returns failure\r\n");
    return L7_FAILURE;
  }

  if(dhcpsAddOverloadOption(pMsg, &buflen, overloadState) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: dhcpsAddOverloadOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* If client message included option 82, copy option 82 into reply. */
  if (option82)
  {
    if (dhcpsOption82Copy(option82, pMsg, &buflen) != L7_SUCCESS)
    {
      /* increment error counter. Send response w/o option 82. */
      pDhcpsInfo->dhcpsStats.opt82CopyFailed++;
    }
  }

  if(dhcpsAddSingleByteOption(DHCPS_OPTION_TYPE_END, pMsg , &buflen) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendACKMessage: dhcpsAddSingleByteOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* Send Function */
  if(dhcpsSendPacket(pMsg, buflen, intIfNum, isSysMgmtIntf) == L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent++;
    /* Notify registered users about sending the ACK */
    for(index=0;index<L7_LAST_COMPONENT_ID;index++)
    {
      if (sendTypeInterestedCIds[index].registrar_ID !=0)
      {
        memcpy ((void *)&chAddr.addr, pDhcpPacket->header.chaddr,L7_ENET_MAC_ADDR_LEN);
        sendTypeInterestedCIds[index].notify_dhcps_send_type(DHCPS_MESSAGE_TYPE_ACK,
                                        chAddr,
                                        offeredIP,
                                       osapiHtonl(LEASE_DATA(pLease)->leaseTime) );
      }
    }

  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Failure in sending DHCPACK message\n")
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create and send the DHCPACK message by populating the relevant fields
*           for DHCPINFORM message
*
* @param    pPool           @b{(input)} Pointer to the pool node
* @param    pDhcpPacket     @b{(input)} Pointer to the dhcp Packet
* @param    intIfNum        @b{(input)} Interface number
* @param    pParameterList  @b{(input)} Option Parameter List
* @param    option82        @b{(input)} Start of option 82 in client message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The server MUST NOT set the lease time and the yiaddr field while
*           sending ACK for DHCPINFORM message
*
* @end
*********************************************************************/
L7_RC_t dhcpsSendInformACKMessage(dhcpsPoolNode_t *pPool,
                                  dhcpsPacket_t *pDhcpPacket,
                                  L7_uint32 intIfNum,
                                  dhcpsOption_t *pParameterList,
                                  dhcpsOption_t *option82)
{
  dhcpsPacket_t *pMsg;
  L7_uchar8      buffer[L7_DHCP_PACKET_LEN];
  L7_uint32      buflen = 0;
  L7_uchar8      *pOptionBuf = L7_NULLPTR;
  dhcpsOption_t  *pOption    = L7_NULLPTR;
  L7_IP_ADDR_t   intfIpAddr = 0;
  L7_IP_MASK_t   mask;
  L7_uint32      longValue;
  L7_uchar8      overloadState = DHCPS_BOTH_CAN_BE_OVERLOADED;
  L7_BOOL        clientOverload;
  L7_BOOL        isSysMgmtIntf = L7_FALSE;

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "\nCreate Ack Message for DHCPINFORM");

  pMsg = (dhcpsPacket_t *)buffer;
  memset(pMsg, 0, L7_DHCP_PACKET_LEN);

  buflen = sizeof(dhcpsHeader_t);

  /* Fill in the header and magic cookie */
  pMsg->header.op = DHCPS_BOOTPREPLY;
  pMsg->header.htype = DHCPS_HW_TYPE;
  pMsg->header.hlen = L7_MAC_ADDR_LEN;
  pMsg->header.hops = 0;
  memcpy(&pMsg->header.xid, &pDhcpPacket->header.xid, sizeof(pMsg->header.xid));
  memcpy(pMsg->header.chaddr, pDhcpPacket->header.chaddr, sizeof(pDhcpPacket->header.chaddr));

  /* ciaddr */
  memcpy(&pMsg->header.ciaddr, &pDhcpPacket->header.ciaddr, sizeof(pMsg->header.ciaddr));

  memcpy(&pMsg->header.giaddr, &pDhcpPacket->header.giaddr, sizeof(pMsg->header.giaddr));
  memcpy(&pMsg->header.flags, &pDhcpPacket->header.flags, sizeof(pMsg->header.flags));

  /* Copy the siaddr field*/
  if( POOL_DATA(pPool)->nextServer !=0)
  {
    longValue = osapiHtonl(POOL_DATA(pPool)->nextServer);
    memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));
  }
  else
  {
    if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
      return L7_FAILURE;

    longValue = osapiHtonl(intfIpAddr);
    memcpy(&pMsg->header.siaddr,&longValue,sizeof(pMsg->header.siaddr));
  }

  longValue = osapiHtonl(DHCPS_MAGIC_COOKIE_VALUE);
  memcpy(pMsg->magicCookie, &longValue, sizeof(longValue));
  buflen += DHCPS_MAGIC_COOKIE_LENGTH;

  pOptionBuf = pMsg->options;
  if( pParameterList != L7_NULLPTR)
  {
    clientOverload = dhcpsClientSupportsLongOptions(pParameterList->value,pParameterList->length);
  }
  else
  {
    clientOverload = L7_FALSE;
  }

  /* message type option */
  pOption = (dhcpsOption_t*) pOptionBuf;
  pOption->type   = DHCPS_OPTION_TYPE_MESSAGE;
  pOption->length   = 1;
  pOption->value[0] = DHCPS_MESSAGE_TYPE_ACK;
  buflen   = DHCPS_OPTION_VALUE_OFFSET + 1 + buflen;

  /* server id option */
  if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendInformACKMessage: Server Id get failure\r\n");
      return L7_FAILURE;
  }
  else
  {
    if (intfIpAddr == simGetSystemIPAddr() && mask == simGetSystemIPNetMask())
    {
      isSysMgmtIntf = L7_TRUE;
    }
  }

  longValue = osapiHtonl(intfIpAddr);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_SERVER_ID, (L7_uchar8*)&longValue, sizeof(L7_IP_ADDR_t), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendInformACKMessage: Couldn't add the server Id\r\n");
    return L7_FAILURE;
  }

  /* Option add function call */
  if( L7_SUCCESS != dhcpsAppendOptions(pPool, pMsg, &buflen, pParameterList, &overloadState,clientOverload))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendInformACKMessage: dhcpsAppendOptions function returns failure\r\n");
    return L7_FAILURE;
  }

  if(dhcpsAddOverloadOption( pMsg, &buflen, overloadState) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendInformACKMessage: dhcpsAddOverloadOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* If client message included option 82, copy option 82 into reply. */
  if (option82)
  {
    if (dhcpsOption82Copy(option82, pMsg, &buflen) != L7_SUCCESS)
    {
      /* increment error counter. Send response w/o option 82. */
      pDhcpsInfo->dhcpsStats.opt82CopyFailed++;
    }
  }

  if(dhcpsAddSingleByteOption(DHCPS_OPTION_TYPE_END, pMsg, &buflen) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendInformACKMessage: dhcpsAddSingleByteOption function returns failure\r\n");
    return L7_FAILURE;
  }

  /* Send Function */
  if(dhcpsSendPacket(pMsg, buflen, intIfNum, isSysMgmtIntf) == L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfDhcpsAckSent++;
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Failure in sending DHCPACK message for DHCPINFORM\n")
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create and send the DHCPNAK message by populating the relevant fields
*
* @param    pLease        @b{(input)} Pointer to the lease node
* @param    intIfNum      @b{(input)} Interface number
* @param    option82      @b{(input)} Start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSendNAKMessage(dhcpsPacket_t *pDhcpPacket, L7_uint32 intIfNum, dhcpsOption_t *option82)
{
  dhcpsPacket_t  *pMsg;
  L7_uchar8      buffer[L7_DHCP_PACKET_LEN];
  L7_uint32      buflen = 0;
  L7_uchar8      *pOptionBuf = L7_NULLPTR;
  dhcpsOption_t  *pOption    = L7_NULLPTR;
  L7_IP_ADDR_t   intfIpAddr = 0;
  L7_IP_MASK_t   mask;
  L7_uint32      longValue;
  L7_uchar8      overloadState = DHCPS_NOT_OVERLOADED;
  L7_BOOL        clientOverload = L7_FALSE;
  L7_BOOL        isSysMgmtIntf = L7_FALSE;
  L7_uint32 index;
  L7_enetMacAddr_t chAddr;


  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "\nCreate Nack Message");

  pMsg = (dhcpsPacket_t *)buffer;
  memset(pMsg, 0, L7_DHCP_PACKET_LEN);

  buflen = sizeof(dhcpsHeader_t);

  /* Fill in the header and magic cookie */
  pMsg->header.op = DHCPS_BOOTPREPLY;
  pMsg->header.htype = DHCPS_HW_TYPE;
  pMsg->header.hlen = L7_MAC_ADDR_LEN;
  pMsg->header.hops = 0;
  memcpy(&pMsg->header.xid, &pDhcpPacket->header.xid, sizeof(pMsg->header.xid));
  memcpy(pMsg->header.chaddr, pDhcpPacket->header.chaddr, sizeof(pDhcpPacket->header.chaddr));
  memcpy(&pMsg->header.giaddr, &pDhcpPacket->header.giaddr, sizeof(pMsg->header.giaddr));

  /* ciaddr (According to RFC its 0 in Nack)*/

  longValue = osapiHtonl(DHCPS_MAGIC_COOKIE_VALUE);
  memcpy(pMsg->magicCookie, &longValue, sizeof(longValue));
  buflen += DHCPS_MAGIC_COOKIE_LENGTH;

  pOptionBuf = pMsg->options;
  /* message type option */
  pOption = (dhcpsOption_t*) pOptionBuf;
  pOption->type   = DHCPS_OPTION_TYPE_MESSAGE;
  pOption->length   = 1;
  pOption->value[0] = DHCPS_MESSAGE_TYPE_NACK;
  buflen   = DHCPS_OPTION_VALUE_OFFSET + 1 + buflen;

  /* server id option */
  if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendNAKMessage: Couldn't Get server Id\r\n");
      return L7_FAILURE;
  }
  else
  {
    if (intfIpAddr == simGetSystemIPAddr() && mask == simGetSystemIPNetMask())
    {
      isSysMgmtIntf = L7_TRUE;
    }
  }

  longValue = osapiHtonl(intfIpAddr);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_SERVER_ID, (L7_uchar8*)&longValue, sizeof(L7_IP_ADDR_t), &overloadState, clientOverload)== L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendNAKMessage: Couldn't Set server Id\r\n");
      return L7_FAILURE;
  }

  /* If client message included option 82, copy option 82 into reply. */
  if (option82)
  {
    if (dhcpsOption82Copy(option82, pMsg, &buflen) != L7_SUCCESS)
    {
      /* increment error counter. Send response w/o option 82. */
      pDhcpsInfo->dhcpsStats.opt82CopyFailed++;
    }
  }

  /* end option (mandatory) */
  if(dhcpsAddSingleByteOption(DHCPS_OPTION_TYPE_END, pMsg, &buflen) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendNAKMessage: dhcpsAddSingleByteOption function retuns failure\r\n");
      return L7_FAILURE;
  }

  /* Send Function */
  if(dhcpsSendPacket(pMsg, buflen, intIfNum, isSysMgmtIntf) == L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfDhcpsNackSent++;
    for(index=0;index<L7_LAST_COMPONENT_ID;index++)
    {
      if (sendTypeInterestedCIds[index].registrar_ID !=0)
      {
        memcpy ((void *)&chAddr.addr, pDhcpPacket->header.chaddr,L7_ENET_MAC_ADDR_LEN);
        sendTypeInterestedCIds[index].notify_dhcps_send_type(DHCPS_MESSAGE_TYPE_NACK,
                                        chAddr,
                                        0,
                                       0) ;
      }
    }

  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Failure in sending DHCPNACK message\n")
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Create and send the BOOTPREPLY by populating the relevant fields
*
* @param    pLease        @b{(input)} Pointer to the lease node
* @param    pDhcpPacket   @b{(input)} Pointer to the dhcp Packet
* @param    intIfNum      @b{(input)} Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  dhcpsSendBootpReply(dhcpsLeaseNode_t *pLease,
                             dhcpsPacket_t *pDhcpPacket,
                             L7_uint32 intIfNum)
{
  dhcpsPacket_t *pMsg;
  L7_uchar8      buffer[L7_DHCP_PACKET_LEN];
  L7_uint32      buflen = 0;
  L7_IP_ADDR_t   offeredIP;
  L7_IP_ADDR_t   intfIpAddr = 0;
  L7_IP_MASK_t   mask;
  dhcpsPoolNode_t *pPool     = L7_NULLPTR;
  L7_uchar8      *pOptionBuf = L7_NULLPTR;
  L7_uint32      longValue;
  L7_uchar8      overloadState = DHCPS_NOT_OVERLOADED;
  L7_BOOL        clientOverload = L7_FALSE;
  L7_BOOL        isSysMgmtIntf = L7_FALSE;

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Create BOOTPREPLY Message\n");

  pPool = pLease->parentPool;
  pMsg = (dhcpsPacket_t *)buffer;
  memset(pMsg, 0, L7_DHCP_PACKET_LEN);

  buflen = sizeof(dhcpsHeader_t);

  /* Fill in the header */
  pMsg->header.op = DHCPS_BOOTPREPLY;
  pMsg->header.htype = DHCPS_HW_TYPE;
  pMsg->header.hlen = L7_MAC_ADDR_LEN;
  pMsg->header.hops = 0;
  memcpy(&pMsg->header.xid, &pDhcpPacket->header.xid, sizeof(pMsg->header.xid));
  memcpy(pMsg->header.chaddr, pDhcpPacket->header.chaddr, sizeof(pDhcpPacket->header.chaddr));
  memcpy(&pMsg->header.giaddr, &pDhcpPacket->header.giaddr, sizeof(pMsg->header.giaddr));
  memcpy(&pMsg->header.flags, &pDhcpPacket->header.flags, sizeof(pMsg->header.flags));
  memcpy(&pMsg->header.ciaddr, &pDhcpPacket->header.ciaddr, sizeof(pMsg->header.ciaddr));
  /* Copy the siaddr field*/
  if( POOL_DATA(pLease->parentPool)->nextServer !=0)
  {
    longValue = osapiHtonl(POOL_DATA(pLease->parentPool)->nextServer);
    memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));
  }
  else
  {
    if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
      return L7_FAILURE;

    longValue = osapiHtonl(intfIpAddr);
    memcpy(&pMsg->header.siaddr,&longValue,sizeof(pMsg->header.siaddr));
  }

  /* Fill in the server IP */
  if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "\nCould not get the Server ID");
    return L7_ERROR;
  }
  longValue = osapiHtonl(intfIpAddr);
  memcpy(&pMsg->header.siaddr, &longValue, sizeof(pMsg->header.siaddr));

  /* if ciaddr == 0, fill in yiaddr (by relating chaddr to IP address) from the lease */
  if (pDhcpPacket->header.ciaddr == 0)
  {
    offeredIP =  osapiHtonl(LEASE_DATA(pLease)->ipAddr);
    memcpy(&pMsg->header.yiaddr, &offeredIP, sizeof(pMsg->header.yiaddr));
  }

  /* magic cookie */
  longValue = osapiHtonl(DHCPS_MAGIC_COOKIE_VALUE);
  memcpy(pMsg->magicCookie, &longValue, sizeof(longValue));
  buflen += DHCPS_MAGIC_COOKIE_LENGTH;

  /* Bootp Vendor options */
  pOptionBuf = pMsg->options;

  if (dhcpsGetServerID(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendBootpReply: Server Id get failure\r\n");
      return L7_FAILURE;
  }
  else
  {
    if (intfIpAddr == simGetSystemIPAddr() && mask == simGetSystemIPNetMask())
    {
      isSysMgmtIntf = L7_TRUE;
    }
  }

  longValue = osapiHtonl(intfIpAddr);
  if(dhcpsAddTLVOption(pMsg, &buflen, DHCPS_OPTION_TYPE_SERVER_ID, (L7_uchar8*)&longValue, sizeof(L7_IP_ADDR_t), &overloadState, clientOverload) == L7_FAILURE)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendBootpReply: Couldn't add the server Id\r\n");
      return L7_FAILURE;
  }

  /* Option add function call */
  if( L7_SUCCESS != dhcpsAppendOptions(pLease->parentPool, pMsg, &buflen, L7_NULLPTR, &overloadState,clientOverload))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendBootpReply: dhcpsAppendOptions function returns failure\r\n");
    return L7_FAILURE;
  }

  if(dhcpsAddSingleByteOption(DHCPS_OPTION_TYPE_END, pMsg, &buflen) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
         "dhcpsSendBootpReply: dhcpsAddSingleByteOption function returns failure\r\n");
    return L7_FAILURE;
  }

  if(buflen > DHCPS_BOOTP_PKT_LENGTH)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
      "dhcpsSendBootpReply: pkt length %u exceeded DHCPS_BOOTP_PKT_LENGTH\n", buflen);
    return L7_FAILURE;
  }
  else if (buflen < DHCPS_BOOTP_PKT_LENGTH)
  {
    pOptionBuf = (L7_uchar8 *)pMsg + buflen;
    /* pad option */
    memset(pOptionBuf, 0, DHCPS_BOOTP_PKT_LENGTH - buflen);
    pOptionBuf = (DHCPS_BOOTP_PKT_LENGTH - buflen) + pOptionBuf;
    buflen = DHCPS_BOOTP_PKT_LENGTH;
  }

  /* Send Function - there are cases defined in RFC 951 */
  if(dhcpsSendBootpReplyPacket(pMsg, buflen, intIfNum,
                 LEASE_DATA(pLease)->ipAddr, isSysMgmtIntf) == L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfBootpOnlySent++;
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Failure in sending BOOTPREPLY message\n")
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sends a dhcp Pakcet after building IP and UDP headers to
*           a send a raw IP and by Broadcast or Unicast Ethernet address
*           on the specified interface (to the specified hwaddr)
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    destIpAddr       @b{(input)} Destination IP Address on which packet is to be sent
* @param    destPort         @b{(input)} Destination port on which packet is to be sent
* @param    pDestMacAddr     @b{(input)} Poniter to the destination mac address
* @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsRawPacketSend(L7_uint32 intIfNum, dhcpsPacket_t* dhcpPacket,
                           L7_uint32 dhcpPacketLength, L7_IP_ADDR_t destIpAddr,
                           L7_ushort16 destPort, L7_uchar8 * pDestMacAddr,
                           L7_BOOL isSysMgmtIntf)
{
    L7_uchar8 dhcpBuffer[DHCPS_BUFFER_LEN];
    L7_ushort16 etype;
    L7_ushort16 flag = 0;
    L7_netBufHandle pBufHandle;
    L7_uchar8 destMacAddr[L7_MAC_ADDR_LEN];
    L7_uint32 len    = L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + dhcpPacketLength;
    L7_uint32 vlanId;
    L7_uint32 tempIntIfNum = 0;

    /* bcast or unicast? */
    if(L7_NULLPTR == pDestMacAddr)
    {
      memcpy( destMacAddr, &(L7_ENET_BCAST_MAC_ADDR), L7_MAC_ADDR_LEN );
      destIpAddr = L7_IP_LTD_BCAST_ADDR;
      destPort   = osapiHtons((L7_ushort16)DHCPS_CLIENT_PORT); /* bcast is always to client */
    }
    else
    {
      memcpy( destMacAddr, pDestMacAddr, L7_MAC_ADDR_LEN );
      destIpAddr = osapiHtonl(destIpAddr);
      destPort   = osapiHtons(destPort);
    }

    /* build the IP */
    memset((char *)dhcpBuffer, 0, DHCPS_BUFFER_LEN);
    if(dhcpsPacketBuild(intIfNum, dhcpBuffer, dhcpPacket, destIpAddr, destPort, dhcpPacketLength) != L7_SUCCESS)
    {
      pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
      return L7_FAILURE;
    }

    /* encapsulate in eth */
    flag |= L7_LL_ENCAP_ENET;
    etype=L7_ETYPE_IP;
    if (isSysMgmtIntf)
    {
      tempIntIfNum = 0;
    }
    else
    {
#ifdef L7_ROUTING_PACKAGE
      flag |= L7_LL_USE_L3_SRC_MAC;
#endif
      tempIntIfNum = intIfNum;
    }
    if (sysNetPduHdrEncapsulate(destMacAddr, tempIntIfNum, &flag,
                                len,0,etype,&pBufHandle, dhcpBuffer) != L7_SUCCESS)
    {
      pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
      return L7_FAILURE;
    }

    vlanId = simMgmtVlanIdGet();
#ifdef L7_ROUTING_PACKAGE
    if(ipMapVlanRtrIntIfNumToVlanId(intIfNum, &vlanId) == L7_FAILURE)
    {
      vlanId = simMgmtVlanIdGet();
    }
#endif

    /* send via dtl */
    if(dtlIpBufSend(intIfNum, vlanId, pBufHandle)==L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"dtlIfBufSend ok\n");
      return L7_SUCCESS;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"dtlIfBufSend failed\n");
      pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
      return L7_FAILURE;
    }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Builds dhcp packet by adding UDP and IP headers
*
* @param    intIfnum         @b{(input)}  Interface num. on which data to be sent
* @param    dhcpbuff         @b{(output)} buffer to contain the packet
* @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    destIpAddr       @b{(input)} Destination IP Address
* @param    destPort         @b{(input)} Destnation Port
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dhcpsPacketBuild(L7_uint32 intIfNum, L7_uchar8* dhcp_buff, dhcpsPacket_t* dhcpPacket, L7_IP_ADDR_t destIpAddr, L7_ushort16 destPort, L7_uint32 dhcpPacketLength)
{
    L7_uint32 srcAddr;
    L7_IP_MASK_t mask;

    /* ip header */
    if(dhcpsPacketIpHeaderBuild(intIfNum, dhcp_buff,destIpAddr, dhcpPacketLength)==L7_FAILURE)
        return L7_FAILURE;

    dhcp_buff += sizeof(L7_ipHeader_t);

    /* upd header */
    if(dhcpsGetServerID(intIfNum, &srcAddr, &mask) == L7_FAILURE)
        return L7_FAILURE;

    if(dhcpsPacketUdpHeaderBuild(dhcp_buff, srcAddr, destIpAddr, destPort, dhcpPacket,dhcpPacketLength)==L7_FAILURE)
        return L7_FAILURE;

    /* packet data */
    memcpy((dhcp_buff+sizeof(L7_udp_header_t)), (L7_uchar8 *)dhcpPacket, dhcpPacketLength);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Builds IP header
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpbuff         @b{(output)} buffer to contain the packet
* @param    destIpAddr       @b{(input)} Destination IP Address
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
* @param    dhcpbuff         @b{(input/output)} buffer to contain the packet
*
* @returns  L7_SUCCESS
* @retuns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t dhcpsPacketIpHeaderBuild(L7_uint32 intIfNum, L7_uchar8* dhcp_buff, L7_IP_ADDR_t destIpAddr, L7_uint32 dhcpPacketLength)
{
    L7_uint32 length;
    L7_uint32 srcAddr;
    L7_IP_MASK_t mask;
    L7_uint32 rc;
    static L7_uint32  iph_ident;
    L7_ipHeader_t * ip = (L7_ipHeader_t *)(dhcp_buff);

    rc = dhcpsGetServerID(intIfNum, &srcAddr, &mask);
    if(rc == L7_FAILURE)
    {
        return rc;
    }

    iph_ident=0;
    ip->iph_versLen =(L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN ;
    length= L7_IP_HDR_LEN;
    ip->iph_tos = 0;
    ip->iph_len = length + sizeof(L7_udp_header_t)+ dhcpPacketLength;
    ip->iph_len = osapiHtons(ip->iph_len);
    ip->iph_ident  = osapiHtons(iph_ident++);
    ip->iph_flags_frag  = 0;
    ip->iph_ttl = IP_BOOTP_DHCP_RELAY_TTL;
    ip->iph_prot= IP_PROT_UDP;
    ip->iph_src = osapiHtonl(srcAddr);
    ip->iph_dst = /*osapiHtonl already in network order */(destIpAddr);
    ip->iph_csum= 0;
    ip->iph_csum= dhcpsCheckSum( (L7_ushort16*)ip, length, 0 );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Builds UDP header
*
* @param    dhcpbuff         @b{(input)}Buffer to contain the packet
* @param    srcAddr          @b{(input)} Source IP address
* @param    destAddr         @b{(input)} Destination IP address
* @param    destPort         @b{(input)} Destination Port
* @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
* @param    dhcpbuff         @b{(input/output)} buffer to contain the packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsPacketUdpHeaderBuild(L7_uchar8* dhcp_buff,L7_uint32 srcAddr, L7_uint32 destAddr, L7_ushort16 destPort,
dhcpsPacket_t* dhcpPacket, L7_uint32 dhcpPacketLength)
{
    L7_udp_header_t   *udp;
    struct psuedohdr_s psuedohdr;

    L7_uchar8 psuedo_packet[DHCPS_UDP_PSUEDO_PACKET_LEN];

    udp = (L7_udp_header_t *) dhcp_buff;
    udp->destPort = destPort;
    udp->sourcePort =osapiHtons((L7_ushort16)DHCPS_SERVER_PORT);
    udp->length = osapiHtons(sizeof(L7_udp_header_t) + dhcpPacketLength);
    udp->checksum = 0;

    psuedohdr.protocol = IP_PROT_UDP;
    psuedohdr.length = osapiHtons(sizeof(L7_udp_header_t) + dhcpPacketLength);
    psuedohdr.place_holder = 0;
    psuedohdr.source_address = osapiHtonl(srcAddr);
    psuedohdr.dest_address = destAddr;

    memcpy(psuedo_packet,&psuedohdr, sizeof(psuedohdr));
    memcpy((psuedo_packet + sizeof(psuedohdr)), dhcp_buff , sizeof(L7_udp_header_t)+ dhcpPacketLength);
    memcpy((psuedo_packet + sizeof(psuedohdr) + sizeof(L7_udp_header_t)), dhcpPacket,  dhcpPacketLength);
    udp->checksum = dhcpsCheckSum( (L7_ushort16 *)psuedo_packet, sizeof(psuedohdr)+ sizeof(L7_udp_header_t) + dhcpPacketLength, 0);

    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Calculates checksum on a buffer
*
* @param    addr         @b{(input)} data buffer
* @param    len          @b{(input)} length of data
* @param    csum         @b{(input)} checksum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 dhcpsCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum)
{
    register L7_uint32 nleft = len;
    const    L7_ushort16 *w = addr;
    register L7_ushort16 answer;
    register L7_uint32 sum = csum;

    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    if(nleft == 1)
        sum += osapiHtons(*(u_char *)w << 8);
    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);                     /* add carry */
    answer = ~sum;                          /* truncate to 16 bits */
    return(answer);
}
/*********************************************************************
* @purpose  Show the transition of the lease states
*
* @param    pLease            @b{(input/output)} Pointer to the lease node
* @param    newState          @b{(input)} Lease state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTransitionLeaseState(dhcpsLeaseNode_t * pLease, L7_int32 newState)
{
  /* exit old state */
  switch(LEASE_DATA(pLease)->state)
  {
  case FREE_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) exiting FREE state\n", (L7_uint32)pLease);
    break;

  case EXPIRED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) exiting EXPIRED state\n", (L7_uint32)pLease);
    break;

  case OFFERED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) exiting OFFERED state\n", (L7_uint32)pLease);
    dhcpsUnscheduleLease(pLease);
    break;

  case ACTIVE_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) exiting ACTIVE state\n", (L7_uint32)pLease);
    dhcpsUnscheduleLease(pLease);
    break;

  case ABANDONED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) exiting ABANDONED state\n", (L7_uint32)pLease);
    /* reset ticks */
    pLease->leaseStartTime = 0;
    break;

  default:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"Lease (%u) exiting UNKNOWN state\n", (L7_uint32)pLease);
  }

  LEASE_DATA(pLease)->state = newState;

  /* enter new state */
  switch(LEASE_DATA(pLease)->state)
  {
  case FREE_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) entering FREE state\n", (L7_uint32)pLease);
    break;

  case EXPIRED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) entering EXPIRED state\n", (L7_uint32)pLease);
    break;

  case OFFERED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) entering OFFERED state\n", (L7_uint32)pLease);
    dhcpsScheduleLease(pLease, DHCPS_OFFER_VALIDITY_DURATION_SECS);
    break;

  case ACTIVE_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) entering ACTIVE state\n", (L7_uint32)pLease);
    dhcpsScheduleLease(pLease, LEASE_DATA(pLease)->leaseTime);
    break;

  case ABANDONED_LEASE:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,"Lease (%u) entering ABANDONED state\n", (L7_uint32)pLease);
    /* set ticks at which abandoned */
    pLease->leaseStartTime = simSystemUpTimeGet();
    break;

  default:
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"Lease (%u) entering UNKNOWN state\n", (L7_uint32)pLease);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize the ping socket
*
* @param    None
*
* @returns  L7_SUCCESS If socket is successfully initialized
* @returns  L7_FAILURE If socket is failed to be initialized
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSockInit(void)
{
  if(pingSockFd == 0)
  {
    if(osapiSocketCreate(L7_AF_INET, L7_SOCK_RAW, IPPROTO_ICMP,
                         &pingSockFd) != L7_SUCCESS)
    {
      pingSockFd = 0;
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCPS_MAP_COMPONENT_ID,
             "Failed to create DHCP ping socket");
      return L7_FAILURE;
    }

    bzero ( (L7_char8 *) &icmpMsg, sizeof (struct icmp) );
    icmpMsg.icmp_type = ICMP_ECHO;
    icmpMsg.icmp_code = 0;
    icmpMsg.icmp_id = osapiHtons(DHCPS_PING_TEST_ID);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send a ping request
*
* @param    ipAddr            @b{(input)} IP Address to ping
* @param    seqNo             @b{(input)} ICMP Sequence Number
*
* @returns  L7_SUCCESS If Echo requests are sent successfully
* @returns  L7_ERROR   If stack returns failure on any socket operation
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dhcpsSendEchoRequest(L7_IP_ADDR_t ipAddr, L7_ushort16 seqNo)
{
  L7_sockaddr_in_t destAddr;
  L7_uint32 bytesSent = 0, rc = L7_FAILURE;

  bzero ((L7_char8 *) &destAddr, sizeof (destAddr));
  destAddr.sin_family = L7_AF_INET ;
  destAddr.sin_addr.s_addr = osapiHtonl(ipAddr);

  /* send first ICMP Echo Request */
  icmpMsg.icmp_cksum = 0;
  icmpMsg.icmp_seq = osapiHtons(seqNo);
  icmpMsg.icmp_cksum = inetChecksum((void *)&icmpMsg, sizeof(struct icmp));

  rc = osapiSocketSendto(pingSockFd, (L7_uchar8 *)&icmpMsg, ICMP_MINLEN, 0,
                         (L7_sockaddr_t *)&destAddr, sizeof(destAddr), &bytesSent);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
           "Failed to send dhcp ping packet  for  %x",ipAddr);
    return L7_FAILURE;
  }
#ifdef _L7_OS_LINUX_
  osapiL7IcmpOutEchosInc();
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Wrapper to osapiPing
*
* @param    ipAddr            @b{(input)} IP Address to ping
* @param    numPackets        @b{(input)} number of packets to send
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dhcpsPing(L7_IP_ADDR_t ipAddr)
{
  L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];

  if(0 == pDhcpsMapCfgData->dhcps.dhcpsPingPktNo)
    return 0;

  /* ipAddr is in host order! */
  osapiInetNtoa(ipAddr, ipStr);
  return osapiPingTimed(ipStr, pDhcpsMapCfgData->dhcps.dhcpsPingPktNo,
                        L7_DHCPS_DEFAULT_MILLISECONDS_BETWEEN_PINGS);
}

/*********************************************************************
* @purpose  Locate a free IP by searching for gaps in allocated leases
*           addresses and also ensure that the address is not explicitly
*           excluded
*
* @param    pPool            @b{(input)} Pointer to pool node
* @param    pIpAddr          @b{(output)} Pointer to free IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsGetFreeIpAddr(dhcpsPoolNode_t * pPool, L7_IP_ADDR_t * pIpAddr)
{
  dhcpsLeaseNode_t  *pLease       = L7_NULLPTR;
  dhcpsLeaseNode_t  *pOldestLease = L7_NULLPTR;
  L7_IP_ADDR_t      ipAddr;
  L7_IP_ADDR_t      maxIpAddr;
  L7_IP_ADDR_t      minIpAddr;
  L7_BOOL           isManualExcluded;

  if(MANUAL_POOL == pPool->poolData->poolType)
  {
    minIpAddr = pPool->poolData->ipAddr - 1;
    maxIpAddr = pPool->poolData->ipAddr + 1;
    isManualExcluded = L7_FALSE;
  }
  else
  {
    minIpAddr = pPool->poolData->ipAddr;
    maxIpAddr = pPool->poolData->ipAddr + ~(pPool->poolData->ipMask);
    isManualExcluded = L7_TRUE;
  }


  if(*pIpAddr < minIpAddr + 1)
  {
    ipAddr    = minIpAddr + 1;
  }
  else
  {
    ipAddr    = *pIpAddr + 1;
  }

  while(ipAddr < maxIpAddr)
  {
    if(dhcpsFindLeaseByIp(pPool,ipAddr, &pLease) == L7_SUCCESS)
    {
      /* keep track of oldest (non excluded) expired IP for offer! */
      if(EXPIRED_LEASE == LEASE_DATA(pLease)->state)
      {
        /* excluded? */
        if(dhcpsCheckIfExcluded(pPool, ipAddr, isManualExcluded) == L7_TRUE)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
            "GetFreeIP deallocating excluded lease for %x\n", LEASE_DATA(pLease)->ipAddr);
          dhcpsDeallocateLease(&pLease);
        }
        else if(L7_NULLPTR != pOldestLease)
        {
          if(pLease->leaseEndTime < pOldestLease->leaseEndTime)
          {
            pOldestLease = pLease;
          }
        }
        else
        {
          pOldestLease = pLease;
        }
      }
    }
    else
    {
      if(dhcpsCheckIfExcluded(pPool, ipAddr, isManualExcluded) == L7_FALSE)
      {
        /* its free and not excluded! */
        *pIpAddr = ipAddr;
        return L7_SUCCESS;
      }
    }

    ipAddr++;
  }

  /* recover resources? */
  if(L7_NULLPTR != pOldestLease)
  {
    *pIpAddr = LEASE_DATA(pOldestLease)->ipAddr;

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "GetFreeIP deallocating oldest excluded lease in pool to free IP %x\n", LEASE_DATA(pLease)->ipAddr);
    dhcpsDeallocateLease(&pOldestLease);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the Server ID by giving the interface number
*
* @param    incomingIfNum    @b{(input)}  Interface number
* @param    pIpAddr          @b{(output)} Pointer to IP Address
* @param    pIpMask          @b{(output)} Pointer to IP Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsGetServerID(L7_uint32 incomingIfNum, L7_IP_ADDR_t * pIpAddr, L7_IP_MASK_t * pIpMask)
{
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 rtrIfNum;
#endif

  if( nimCheckIfNumber(incomingIfNum) != L7_SUCCESS )
  {
    /* Interface does not exists. retrun failure */
     return L7_FAILURE;
  }

#ifdef L7_ROUTING_PACKAGE
  if (ipMapIntIfNumToRtrIntf(incomingIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    if (ipMapRtrIntfIpAddressGet(incomingIfNum, pIpAddr, pIpMask) == L7_SUCCESS)
    {
       if (*pIpAddr > 0 )
        return L7_SUCCESS;
    }
  }
  else
  {
#endif
     *pIpAddr = simGetSystemIPAddr();
     *pIpMask = simGetSystemIPNetMask();

     if (*pIpAddr > 0 )
      return L7_SUCCESS;
     else
      return L7_FAILURE;

#ifdef L7_ROUTING_PACKAGE
  } /* end of else */
#endif

return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a free lease and async message to be processed by
*           pingTask
*
* @param    dhcpsPoolNode_t  @b{(input)}  Pointer to Pool node
* @param    ipAddr           @b{(input)} IP Address
* @param    clientIdentifier @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength @b{(input)} Length of dhcp packet
* @param    reqLeaseTime     @b{(input)} Requested Lease Time
* @param    incomingIfNum    @b{(input)} Interface number
* @param    getNextFreeIp    @b{(input)} flag to get next free ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsCreateFreeLease(dhcpsPoolNode_t *pPool, L7_IP_ADDR_t ipAddr,
                             L7_uchar8 *clientIdentifier, L7_uchar8 clientIdLen,
                             dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                             L7_uint32 reqLeaseTime, L7_uint32 incomingIntfNum,
                             L7_BOOL getNextFreeIp)
{
  L7_uint32         i = 0;
  L7_RC_t           rc        = L7_FAILURE;
  dhcpsLeaseNode_t  *pLease   = L7_NULLPTR;


  rc = dhcpsAllocateLease(pPool, ipAddr, clientIdentifier, clientIdLen,
                          pDhcpPacket->header.chaddr, pDhcpPacket->header.htype,
                          pDhcpPacket->header.hlen, &pLease);

  if(L7_SUCCESS == rc)
  {
    /* set lease time */
    if((reqLeaseTime >= (L7_DHCPS_LEASETIME_MIN * 60)) &&
       (reqLeaseTime < (L7_DHCPS_LEASETIME_MAX * 60)))
    {
      LEASE_DATA(pLease)->leaseTime = reqLeaseTime;
    }
    else
    {
      LEASE_DATA(pLease)->leaseTime = POOL_DATA(pPool)->leaseTime;
    }
#ifdef L7_NSF_PACKAGE
    LEASE_DATA(pLease)->leaseExpiration = simSystemUpTimeGet() + (LEASE_DATA(pLease)->leaseTime);
#endif /* L7_NSF_PACKAGE */
    LEASE_DATA(pLease)->state = FREE_LEASE;

    if(0 == pDhcpsMapCfgData->dhcps.dhcpsPingPktNo)
    {
      /* No need to send ping packets. Update the lease state and take
       * appropriate action */

      dhcpsHandleLease(pLease, pDhcpPacket, dhcpPacketLength, incomingIntfNum);

      rc = L7_SUCCESS;
    }
    else
    {
      /* Send first ping request and let the pingTask handle the reachability
       * detection asynchronously */
      if(dhcpsSendEchoRequest(ipAddr, 0) != L7_SUCCESS)
      {
        /* socket send failure */
        dhcpsDeallocateLease(&pLease);
        pLease = L7_NULLPTR;

        rc = L7_FAILURE;
      }
      else
      {
        /* create an async message entry for this free lease */
        for (i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
        {
          if(pDhcpsAsyncInfo[i].ipAddr == 0)
          {
            /* found a free slot, fill async msg contents here */

            pDhcpsAsyncInfo[i].ipAddr           = ipAddr;
            pDhcpsAsyncInfo[i].ipAddrShadow     = ipAddr;
            pDhcpsAsyncInfo[i].dhcpPacketLength = dhcpPacketLength;
            pDhcpsAsyncInfo[i].intIfNum         = incomingIntfNum;
            pDhcpsAsyncInfo[i].pingsIssued      = 0;
            pDhcpsAsyncInfo[i].getNextFreeIp    = getNextFreeIp;
            pDhcpsAsyncInfo[i].pingTimeoutCnt   = ((DHCPS_PING_TIMEOUT) / (DHCPS_PING_SELECT_TIMEOUT));
            if((dhcpsPacket_t *)pDhcpsAsyncInfo[i].dhcpPacketData != pDhcpPacket)
            {
              memcpy(pDhcpsAsyncInfo[i].dhcpPacketData, pDhcpPacket, L7_DHCP_PACKET_LEN);
            }

            rc = L7_SUCCESS;
            break;
          }
        }

        if(i == L7_DHCPS_MAX_ASYNC_MSGS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                  "Failed to process DHCP Discover message"
                  " as maximum(%d) discovery messages are being processed.",
                  L7_DHCPS_MAX_ASYNC_MSGS);

          dhcpsDeallocateLease(&pLease);
          pLease = L7_NULLPTR;

          rc = L7_TABLE_IS_FULL;
        }
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Process DHCPREQUEST when the client is in SELECTING state
*
* @param    requestedIP      @b{(input)}  Requested IP
* @param    serverID         @b{(input)} Server IP
* @param    clientID         @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum    @b{(input)} Interface number
* @param    pParameterList   @b{(input)} Option Parameter List
* @param    option82         @b{(input)} start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessSelectingRequest(L7_IP_ADDR_t requestedIP, L7_IP_ADDR_t serverID,
                                     L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                     dhcpsPacket_t *pDhcpPacket,
                                     L7_uint32 incomingIfNum,
                                     dhcpsOption_t *pParameterList,
                                     dhcpsOption_t *option82)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_IP_ADDR_t      serverAddr      = 0;
  L7_IP_MASK_t      serverMask      = 0;
  L7_IP_ADDR_t      incomingSubnetAddr  = 0;  /* subnet addr of (percived) client subnet */
  dhcpsClientLeaseKey_t  clientKey;
  dhcpsClientLeaseKey_t  leaseKey;

  if(dhcpsGetServerID(incomingIfNum, &serverAddr, &serverMask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process SELECTING REQUEST recd invalid incoming ifnum %x\n", incomingIfNum);
    return L7_FAILURE;
  }

  /* set client subnet (server subnet or via relay?) */
  if(pDhcpPacket->header.giaddr == 0)
  {
    /* pkt recd directly */
    incomingSubnetAddr = serverAddr;
  }
  else
  {
    /* pkt recd via relay */
    incomingSubnetAddr = osapiNtohl(pDhcpPacket->header.giaddr);
  }

  memset(&clientKey, 0, sizeof(dhcpsClientLeaseKey_t));
  memset(&leaseKey, 0, sizeof(dhcpsClientLeaseKey_t));

  /* find lease for the requested addr */
  if(dhcpsFindLeaseNodeByIpAddr(requestedIP, &pLease) == L7_SUCCESS)
  {
    getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
      pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
      LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype,
      LEASE_DATA(pLease)->hwAddrLength, &leaseKey);
  }

  /* selected this server? */
  if(serverID == serverAddr)
  {
    if(L7_NULLPTR == pLease)
    {
      /* dont have lease! */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process SELECTING REQUEST could not find lease for requested IP %x\n", requestedIP);
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      return L7_SUCCESS;
    }

    /* ensure incomingSubnetAddr matches lease pool */
    if((incomingSubnetAddr & POOL_DATA(pLease->parentPool)->ipMask) !=
       (POOL_DATA(pLease->parentPool)->ipAddr &  POOL_DATA(pLease->parentPool)->ipMask))
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process SELECTING REQUEST requested IP %x doesnt match incoming subnet\n", requestedIP);
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      return L7_SUCCESS;
    }

    /* match client to lease */
    if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
    {
      /* offered lease? */
      if(LEASE_DATA(pLease)->state == OFFERED_LEASE)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process SELECTING REQUEST ACK'd %x", requestedIP);
        dhcpsTransitionLeaseState(pLease, ACTIVE_LEASE);
        dhcpsSendACKMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        rc = L7_SUCCESS;
      }
      else if(LEASE_DATA(pLease)->state == ACTIVE_LEASE)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process SELECTING REQUEST ACK'd %x (already active)", requestedIP);
        dhcpsSendACKMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
        rc = L7_SUCCESS;
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process SELECTING REQUEST requested ip lease not in offered or active state\n");
      }
    }
    else
    {
      /* message from unexpected client! */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process SELECTING REQUEST message from different client\n");
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
  }
  else
  {
    /* selected diff server */
    if(L7_NULLPTR != pLease)
    {
      /* match client to lease */
      if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
      {
        /* deallocate as client selected other server */
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process SELECTING REQUEST deallocating lease for addr %x as client selected server %x\n",
          requestedIP, serverAddr);
        dhcpsDeallocateLease(&pLease);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        rc = L7_SUCCESS;
      }
    }
  }

  return rc;
}
/*********************************************************************
* @purpose  Process DHCPREQUEST when the client is in INIT-REBOOT state
*
* @param    requestedIP      @b{(input)}  Requested IP
* @param    clientID         @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum    @b{(input)} Interface number
* @param    pParameterList   @b{(input)} Option Parameter List
* @param    option82         @b{(input)} start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessInitRebootRequest(L7_IP_ADDR_t requestedIP,
                                      L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                      dhcpsPacket_t *pDhcpPacket,
                                      L7_uint32 incomingIfNum,
                                      dhcpsOption_t *pParameterList,
                                      dhcpsOption_t *option82)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_IP_ADDR_t      serverAddr      = 0;
  L7_IP_MASK_t      serverMask      = 0;
  L7_IP_ADDR_t      incomingSubnetAddr  = 0;  /* subnet addr of (percived) client subnet */
  dhcpsClientLeaseKey_t  clientKey;
  dhcpsClientLeaseKey_t  leaseKey;

  if(dhcpsGetServerID(incomingIfNum, &serverAddr, &serverMask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process INIT-REBOOT recd invalid incoming ifnum %x\n", incomingIfNum);
    return L7_FAILURE;
  }

  /* set client subnet (server subnet or via relay?) */
  if(pDhcpPacket->header.giaddr == 0)
  {
    /* pkt recd directly */
    incomingSubnetAddr = serverAddr;
  }
  else
  {
    /* pkt recd via relay */
    incomingSubnetAddr = osapiNtohl(pDhcpPacket->header.giaddr);
  }

  /* find lease for the requested addr */
  if(dhcpsFindLeaseNodeByIpAddr(requestedIP, &pLease) == L7_SUCCESS)
  {
    getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
      pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                 LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype,
                 LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

    /* check reqested IP against percieved client subnet */
    if((incomingSubnetAddr & POOL_DATA(pLease->parentPool)->ipMask) !=
       (POOL_DATA(pLease->parentPool)->ipAddr &  POOL_DATA(pLease->parentPool)->ipMask))
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process INIT-REBOOT requested IP %x doesnt match incoming subnet\n", requestedIP);
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    /* match client to lease */
    else if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
    {
      /* ACK if active lease */
      if(LEASE_DATA(pLease)->state == ACTIVE_LEASE)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process INIT-REBOOT req ACK'd %x", requestedIP);
        dhcpsSendACKMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        rc = L7_SUCCESS;
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process INIT-REBOOT found lease for %x, but lease not active!\n", requestedIP);
      }
    }
    else
    {
      /* message from unexpected client! */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process INIT-REBOOT req for %x leased to different client\n", requestedIP);
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process INIT-REBOOT req for addr %x does not match any existing lease\n",
      requestedIP);
  }
  return rc;
}
/*********************************************************************
* @purpose  Process DHCPREQUEST when the client is in RENEW or REBIND state
*
* @param    clientID         @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    isBroadcast      @b{(input)} Was packet received as broadcast
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum    @b{(input)} Interface number
* @param    pParameterList   @b{(input)} Option Parameter List
* @param    option82         @b{(input)} Start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessRenewRebindRequest (L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                        L7_BOOL isBroadcast,
                                        dhcpsPacket_t *pDhcpPacket,
                                        L7_uint32 incomingIfNum,
                                        dhcpsOption_t *pParameterList,
                                        dhcpsOption_t *option82)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_IP_ADDR_t      requestedIP     = 0;
  L7_IP_ADDR_t      serverAddr      = 0;
  L7_IP_MASK_t      serverMask      = 0;
  L7_IP_ADDR_t      incomingSubnetAddr  = 0;  /* subnet addr of (percived) client subnet */
  dhcpsClientLeaseKey_t  clientKey;
  dhcpsClientLeaseKey_t  leaseKey;

  if(dhcpsGetServerID(incomingIfNum, &serverAddr, &serverMask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process RENEW/REBIND recd invalid incoming ifnum %x\n", incomingIfNum);
    return L7_FAILURE;
  }

  /* set client subnet (server subnet or via relay?) */
  if(pDhcpPacket->header.giaddr == 0)
  {
    /* pkt recd directly */
    incomingSubnetAddr = serverAddr;
  }
  else
  {
    /* pkt recd via relay */
    incomingSubnetAddr = osapiNtohl(pDhcpPacket->header.giaddr);
  }

  requestedIP = osapiNtohl(pDhcpPacket->header.ciaddr);

  /* find lease for the requested addr */
  if(dhcpsFindLeaseNodeByIpAddr(requestedIP, &pLease) == L7_SUCCESS)
  {
    getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
      pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                 LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype,
                 LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

    /* match client to lease */
    if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
    {
      /* check reqested IP against percieved client subnet
       * for REBIND case only (when pkt is recd via broadcast)
       */
      if(L7_TRUE == isBroadcast)
      {
        if((incomingSubnetAddr & POOL_DATA(pLease->parentPool)->ipMask) !=
           (POOL_DATA(pLease->parentPool)->ipAddr &  POOL_DATA(pLease->parentPool)->ipMask))
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
            "process REBIND requested IP %x does not match incoming subnet\n", requestedIP);
          dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
          DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
          return (L7_SUCCESS);
        }
      }

      /* reschedule and ACK if active lease */
      if(LEASE_DATA(pLease)->state == ACTIVE_LEASE)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process RENEW/REBIND req ACK'd %x", requestedIP);

        /* re-enter active state (resets timers) */
        if (POOL_DATA(pLease->parentPool)->leaseTime != LEASE_DATA(pLease)->leaseTime)
         {
           DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                         "process RENEW/REBIND changed lease for %x,from %x to %x \n", requestedIP,
                         (LEASE_DATA(pLease)->leaseTime)/60,
                         (POOL_DATA(pLease->parentPool)->leaseTime)/60);
           LEASE_DATA(pLease)->leaseTime = POOL_DATA(pLease->parentPool)->leaseTime;
         }

        dhcpsTransitionLeaseState(pLease, ACTIVE_LEASE);
        dhcpsSendACKMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        rc = L7_SUCCESS;
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process RENEW/REBIND found lease for %x, but lease not active!\n", requestedIP);
      }
    }
    else
    {
      /* silent if client doesnt match */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process RENEW/REBIND req for %x leased to different client\n", requestedIP);
    }
  }
  else
  {
    if(L7_TRUE == isBroadcast)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process REBIND ignoring req for addr %x no lease with (this) server\n",
        requestedIP);
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process RENEW sending NAK req for addr %x, no lease with server\n",
        requestedIP);
      dhcpsSendNAKMessage(pDhcpPacket, incomingIfNum, option82);
    }
  }

  return rc;
}
/*********************************************************************
* @purpose  Process DHCPDECLINE message
*
* @param    requestedIP      @b{(input)}  Requested IP
* @param    clientID         @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum    @b{(input)} Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessDeclineMessage(L7_uint32 requestedIP, L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                  dhcpsPacket_t *pDhcpPacket,
                                  L7_uint32 incomingIfNum)
{
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsClientLeaseKey_t  clientKey;
  dhcpsClientLeaseKey_t  leaseKey;

  /* find lease for the requested addr */
  if(dhcpsFindLeaseNodeByIpAddr(requestedIP, &pLease) == L7_SUCCESS)
  {
    getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
      pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                 LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype,
                 LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

    /* match client to lease */
    if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
    {
      /* abandon lease */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process DECLINE set lease for addr %x ABANDONED\n",
        requestedIP);
      dhcpsTransitionLeaseState(pLease, ABANDONED_LEASE);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process DECLINE for addr %x does not match existing lease client\n",
        requestedIP);
    }
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process DECLINE for addr %x does not match any existing lease\n",
      requestedIP);
  }

  return rc;
}
/*********************************************************************
* @purpose  Process DHCPRELEASE message
*
* @param    clientID         @b{(input)} Client Identifier
* @param    clientIdLen      @b{(input)} Client Identifier Length
* @param    pDhcpPacket      @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum    @b{(input)} Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessReleaseMessage(L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                  dhcpsPacket_t *pDhcpPacket,
                                  L7_uint32 incomingIfNum)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_IP_ADDR_t      requestedIP     = 0;
  dhcpsClientLeaseKey_t  clientKey;
  dhcpsClientLeaseKey_t  leaseKey;

  requestedIP = osapiNtohl(pDhcpPacket->header.ciaddr);

  /* find lease for the requested addr */
  if(dhcpsFindLeaseNodeByIpAddr(requestedIP, &pLease) == L7_SUCCESS)
  {
    getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
      pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);
    getClientKey(LEASE_DATA(pLease)->clientIdentifier, LEASE_DATA(pLease)->clientIdLength,
                 LEASE_DATA(pLease)->hwAddr, LEASE_DATA(pLease)->hAddrtype,
                 LEASE_DATA(pLease)->hwAddrLength, &leaseKey);

    /* match client to lease */
    if(memcmp(&clientKey, &leaseKey, sizeof(dhcpsClientLeaseKey_t)) == 0)
    {
      /* expire lease */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process RELEASE set lease for addr %x EXPIRED\n",
        requestedIP);
      dhcpsTransitionLeaseState(pLease, EXPIRED_LEASE);
      DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
      rc = L7_SUCCESS;
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process RELEASE for addr %x client does not match existing lease client\n",
        requestedIP);
    }
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process RELEASE for addr %x does not match any existing lease\n",
      requestedIP);
  }

  return rc;
}
/*********************************************************************
* @purpose  Process DHCPINFORM message
*
* @param    clientID            @b{(input)} Client Identifier
* @param    clientIdLen         @b{(input)} Client Identifier Length
* @param    leaseTimeRequested  @b{(input)} Requested Lease Time
* @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
* @param    incomingIfNum       @b{(input)} Interface number
* @param    pParameterList      @b{(input)} Option Parameter List
* @param    option82            @b{(input)} start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessInformMessage(L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                  L7_uint32 leaseTimeRequested,
                                  dhcpsPacket_t *pDhcpPacket,
                                  L7_uint32 incomingIfNum,
                                  dhcpsOption_t *pParameterList,
                                  dhcpsOption_t *option82)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsPoolNode_t   *pPool          = L7_NULLPTR;
  L7_IP_ADDR_t      requestedIP     = 0;

  dhcpsClientLeaseKey_t  clientKey;

  requestedIP = osapiNtohl(pDhcpPacket->header.ciaddr);

  /* get key */
  getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
    pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);

  /* match pool for address */
  if(dhcpsMatchPool(requestedIP, &clientKey, &pPool) == L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process INFORM found pool for addr %x\n", requestedIP);
    rc = dhcpsSendInformACKMessage(pPool, pDhcpPacket, incomingIfNum, pParameterList, option82);
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process INFORM could not find a matching pool for addr %x\n",
      requestedIP);
  }

  return rc;
}
/*********************************************************************
* @purpose  Process BOOTPREQUEST message
*
* @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength    @b{(input)} Length of dhcp packet
* @param    incomingIfNum       @b{(input)} Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessBootpRequestMessage(dhcpsPacket_t *pDhcpPacket,
                                        L7_uint32 dhcpPacketLength,
                                        L7_uint32 incomingIfNum)
{
  dhcpsPoolNode_t   *pPool          = L7_NULLPTR;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  L7_IP_ADDR_t      requestedIP     = 0;
  L7_IP_ADDR_t      ipAddr          = 0;
  dhcpsClientLeaseKey_t  clientKey;
  L7_IP_ADDR_t      serverAddr      = 0;
  L7_IP_MASK_t      serverMask      = 0;
  L7_IP_ADDR_t      incomingSubnetAddr  = 0;  /* subnet addr of (percived) client subnet */
  L7_RC_t           rc              = L7_FAILURE;

  if(dhcpsGetServerID(incomingIfNum, &serverAddr, &serverMask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process DISCOVER recd invalid incoming ifnum %x\n", incomingIfNum);
    return L7_FAILURE;
  }

  /* get key */
  getClientKey(L7_NULLPTR, 0, pDhcpPacket->header.chaddr,
    pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);

  /* set client subnet (server subnet or via relay?) */
  if(pDhcpPacket->header.giaddr == 0)
  {
    /* pkt recd directly */
    incomingSubnetAddr = serverAddr;
  }
  else
  {
    /* pkt recd via relay */
    incomingSubnetAddr = osapiNtohl(pDhcpPacket->header.giaddr);
  }

  requestedIP = osapiNtohl(pDhcpPacket->header.ciaddr);

  /* match pool for incoming interface */
  if(dhcpsMatchPool(incomingSubnetAddr, &clientKey, &pPool) == L7_SUCCESS)
  {
    /* must be manual */
    if(POOL_DATA(pPool)->poolType != MANUAL_POOL)
    {
      if(L7_TRUE != pDhcpsMapCfgData->dhcps.dhcpBootpAuto)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process BOOTP REQUEST matching dynamic pool, but the allocation from dynamic pool is disabled\n");
        return L7_FAILURE;
      }

      /* lease out a free address */
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
        "process BOOTP REQUEST automatically allocating from dyn pool\n");

      if(dhcpsFindClientLease(&clientKey, pPool, &pLease) != L7_SUCCESS)
      {
        /* Lease not found.
         * Try to get next free ip for the free lease in pingTask context */

        while (dhcpsGetFreeIpAddr(pPool, &ipAddr) == L7_SUCCESS)
        {
          rc = dhcpsCreateFreeLease(pPool, ipAddr, L7_NULLPTR, 0, pDhcpPacket,
                                    dhcpPacketLength, DHCPS_INFINITE_LEASE_TIME,
                                    incomingIfNum, L7_TRUE);
          if((rc == L7_SUCCESS) || (rc == L7_TABLE_IS_FULL))
          {
            /* Means, we either created a free lease entry and posted to pingTask
             * or the number of free leases being processed by ping task reached max */
            break;
          }
        }
      }
      else
      {
        /* matching lease is found. return it back to the client */
        dhcpsHandleLease(pLease, pDhcpPacket, dhcpPacketLength, incomingIfNum);
      }
    }
    else
    {
      /* manual pool addr must match ciaddr (if any) */
      if(POOL_DATA(pPool)->ipAddr != requestedIP && 0 != requestedIP)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process BOOTP REQUEST ciaddr %x does not match manual pool addr %x\n",
          requestedIP, POOL_DATA(pPool)->ipAddr);
        return L7_FAILURE;
      }

      /* find lease */
      if(dhcpsFindClientLease(&clientKey, pPool, &pLease) != L7_SUCCESS)
      {
        /* Lease not found.
         * Try to assign this manual pool's ip if free for the free lease in pingTask context */

        rc = dhcpsCreateFreeLease(pPool, POOL_DATA(pPool)->ipAddr, L7_NULLPTR, 0,
                                  pDhcpPacket, dhcpPacketLength, DHCPS_INFINITE_LEASE_TIME,
                                  incomingIfNum, L7_FALSE);
      }
      else
      {
        /* matching lease is found. return it back to the client */
        dhcpsHandleLease(pLease, pDhcpPacket, dhcpPacketLength, incomingIfNum);
      }
    }

    if(rc != L7_SUCCESS )
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                    "process BOOTP REQUEST unable to create free lease");
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                    "process BOOTP REQUEST posted to pingTask");
    }
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process BOOTP REQUEST on subnet %x could not find any matching pool\n",
      incomingSubnetAddr);

    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Process DHCPDISCOVER message
*
* @param    ipAddrRequested     @b{(input)} Requested IP Address
* @param    clientID            @b{(input)} Client Identifier
* @param    clientIdLen         @b{(input)} Client Identifier Length
* @param    leaseTimeRequested  @b{(input)} Requested Lease Time
* @param    pDhcpPacket         @b{(input)} Pointer to dhcp packet
* @param    dhcpPacketLength   @b{(input)} Length of the packet
* @param    incomingIfNum       @b{(input)} Interface number
* @param    pParameterList      @b{(input)} Option Parameter List
* @param    option82            @b{(input)} Start of option 82 in client packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsProcessDiscoverMessage(L7_uint32 ipAddrRequested,
                                    L7_uchar8 *clientID, L7_uchar8 clientIdLen,
                                    L7_uint32 leaseTimeRequested,
                                    dhcpsPacket_t *pDhcpPacket,
                                    L7_uint32 dhcpPacketLength,
                                    L7_uint32 incomingIfNum,
                                    dhcpsOption_t *pParameterList,
                                    dhcpsOption_t *option82)
{
  L7_RC_t           rc              = L7_FAILURE;
  dhcpsPoolNode_t   *pPool          = L7_NULLPTR;
  dhcpsLeaseNode_t  *pLease         = L7_NULLPTR;
  dhcpsLeaseNode_t  *pTempLease     = L7_NULLPTR;
  dhcpsLeaseNode_t  *pActiveLease   = L7_NULLPTR;
  dhcpsLeaseNode_t  *pOfferedLease  = L7_NULLPTR;
  dhcpsLeaseNode_t  *pReqAddrLease  = L7_NULLPTR;
  dhcpsLeaseNode_t  *pExpiredLease  = L7_NULLPTR;
  L7_BOOL           isLeaseInvalid  = L7_FALSE;
  L7_IP_ADDR_t      ipAddr          = 0, expiredLeaseIpAddr = 0;
  L7_IP_ADDR_t      serverAddr      = 0;
  L7_IP_MASK_t      serverMask      = 0;
  L7_IP_ADDR_t      incomingSubnetAddr  = 0;  /* subnet addr of (percived) client subnet */
  dhcpsClientLeaseKey_t  clientKey;
  L7_BOOL           matchingExpiredLeaseFound = L7_FALSE;
  L7_BOOL           createFreeLease = L7_TRUE;
  L7_uint32         i = 0;

  /* get incoming interface details */
  if(dhcpsGetServerID(incomingIfNum, &serverAddr, &serverMask) != L7_SUCCESS)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process DISCOVER recd invalid incoming ifnum %x\n", incomingIfNum);
    return L7_FAILURE;
  }

  /* get key */
  getClientKey(clientID, clientIdLen, pDhcpPacket->header.chaddr,
    pDhcpPacket->header.htype, pDhcpPacket->header.hlen, &clientKey);

  /* set client subnet (server subnet or via relay?) */
  if(pDhcpPacket->header.giaddr == 0)
  {
    /* pkt recd directly */
    incomingSubnetAddr = serverAddr;
  }
  else
  {
    /* pkt recd via relay */
    incomingSubnetAddr = osapiNtohl(pDhcpPacket->header.giaddr);
  }

  /* have matching pool? */
  if(dhcpsMatchPool(incomingSubnetAddr, &clientKey, &pPool) == L7_SUCCESS)
  {
    /* ensure pool for direct request matches server subnet */
    if((incomingSubnetAddr == serverAddr) &&
       ((pPool->poolData->ipAddr & pPool->poolData->ipMask) !=
        (serverAddr & serverMask) ))
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                    "Pool for discover from %x does not match server subnet\n", incomingSubnetAddr);
      return L7_FAILURE;
    }
  }
  else
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "No pool configured for discover on subnet %x\n", incomingSubnetAddr);
    return L7_FAILURE;
  }

  /* requested ip is not relevant for a manual pool! */
  if(MANUAL_POOL == pPool->poolData->poolType)
    ipAddrRequested = 0;

  /* process existing leases for this client
   * order of preference:
   * 1. active lease
   * 2. offered lease
   * 3. requested lease (check with ping)
   * 4. expired lease   (check with ping)
   * 5. fresh ip lease  (check with ping)
   */

  dhcpsFindClientLease(&clientKey, pPool, &pLease);
  while (pLease != L7_NULLPTR)
  {
    /* dont process expired/offered/abandoned address leases that have been excluded */
    isLeaseInvalid = L7_FALSE;
    if(dhcpsCheckIfExcluded(pPool, LEASE_DATA(pLease)->ipAddr, L7_FALSE) == L7_TRUE &&
                            LEASE_DATA(pLease)->state != ACTIVE_LEASE)
    {
      isLeaseInvalid = L7_TRUE;
    }

    /* process only if valid */
    if(L7_FALSE == isLeaseInvalid)
    {
      switch (LEASE_DATA(pLease)->state)
      {
      case OFFERED_LEASE:

        if(L7_NULLPTR != pOfferedLease)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
            "process DISCOVER deallocating DUPLICATE OFFERED lease ip %x \n",
            LEASE_DATA(pOfferedLease)->ipAddr);

          if(pOfferedLease == pLease)
          {
            DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Error: Process DISCOVER iteration failure for previously offered lease\n");
            dhcpsMapDebugShow(); /* DEBUG TBD */
            break;
          }
          dhcpsDeallocateLease(&pOfferedLease);
          DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        }


        pOfferedLease = pLease;
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process DISCOVER found OFFERED LEASE ip %x for RE-OFFER\n", LEASE_DATA(pLease)->ipAddr);
        break;

      case ACTIVE_LEASE:

        if(L7_NULLPTR != pActiveLease)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
            "process DISCOVER deallocating DUPLICATE ACTIVE lease ip %x \n", LEASE_DATA(pLease)->ipAddr);

          if(pActiveLease == pLease)
          {
            DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "Error: Process DISCOVER iteration failure for previously active lease\n");
            dhcpsMapDebugShow(); /* DEBUG TBD */
            break;
          }

          dhcpsDeallocateLease(&pActiveLease);
          DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
        }


        pActiveLease = pLease;
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
          "process DISCOVER found ACTIVE LEASE ip %x for OFFER\n", LEASE_DATA(pLease)->ipAddr);
        break;

      case EXPIRED_LEASE:
        if(0 == pDhcpsMapCfgData->dhcps.dhcpsPingPktNo)
        {
          /* If ping is required, lets do the below action in ping task's context */

          /* is this requested? */
          if(LEASE_DATA(pLease)->ipAddr == ipAddrRequested)
          {
            pReqAddrLease = pLease;
            DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
              "process DISCOVER requested ip %x for OFFER\n", LEASE_DATA(pLease)->ipAddr);
          }
          else if(L7_NULLPTR != pExpiredLease)
          {
            /* else save oldest as a candidate */
            if(pLease->leaseEndTime < pExpiredLease->leaseEndTime)
            {
              pExpiredLease = pLease;
            }
          }
          else
          {
            pExpiredLease = pLease;
          }
          /* set lease time */
          LEASE_DATA(pLease)->leaseTime = POOL_DATA(pLease->parentPool)->leaseTime;
          if((leaseTimeRequested >= (L7_DHCPS_LEASETIME_MIN *60)) &&
             (leaseTimeRequested < (L7_DHCPS_LEASETIME_MAX * 60)))
            LEASE_DATA(pLease)->leaseTime = leaseTimeRequested;
#ifdef L7_NSF_PACKAGE
          LEASE_DATA(pLease)->leaseExpiration = simSystemUpTimeGet() + (LEASE_DATA(pLease)->leaseTime);
#endif /* L7_NSF_PACKAGE */
        }
        else
        {
          /* If there is a matching EXPIRED LEASE for this client key,
           * ping and offer shall happen asynchronously */
          matchingExpiredLeaseFound = L7_TRUE;
          expiredLeaseIpAddr = LEASE_DATA(pLease)->ipAddr;
        }

        break;

      case FREE_LEASE:

        /* This means the lease entry is already created and is in the process of detecting
         * ping reachability for next free ip in the pingTask context */

        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI, "process DISCOVER found a FREE LEASE in the pool for IP %x. \
                      Means ping detection is in progress...\n", LEASE_DATA(pLease)->ipAddr);
        DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;

        return L7_SUCCESS;
        /* passthru */

      case ABANDONED_LEASE:

        /* ignore abandoned leases */
        break;

      default:
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                      "process DISCOVER ignoring lease in unknown state %x\n",
                      LEASE_DATA(pLease)->state);

      }
    }

    /* get next lease */
    pTempLease = pLease;
    dhcpsFindNextLeaseByClient(pPool, &pLease);

    /* check for circular loops */
    if(pLease == pTempLease)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_HI,
                    "process DISCOVER: Error circular loop while iterating leases\n");
      break;
    }

    if(L7_TRUE == isLeaseInvalid)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "process DISCOVER deallocating excluded lease for %x\n", LEASE_DATA(pTempLease)->ipAddr);
      dhcpsDeallocateLease(&pTempLease);
      isLeaseInvalid = L7_FALSE;
    }
  } /* end of while */

  /* selected active/offered lease to offer? */
  if(L7_NULLPTR != pActiveLease)
  {
    pLease = pActiveLease;
    dhcpsTransitionLeaseState(pLease, OFFERED_LEASE);
    dhcpsSendOfferMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process DISCOVER OFFERED ACTIVE IP %x\n", LEASE_DATA(pLease)->ipAddr);
    return L7_SUCCESS;
  }
  else if(L7_NULLPTR != pOfferedLease)
  {
    pLease = pOfferedLease;
    dhcpsTransitionLeaseState(pLease, OFFERED_LEASE);
    dhcpsSendOfferMessage(pLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process DISCOVER RE-OFFERED IP %x\n", LEASE_DATA(pLease)->ipAddr);
    return L7_SUCCESS;
  }

  if(matchingExpiredLeaseFound == L7_FALSE)
  {
  /* try to create free lease if ip requested (and not found lease yet) */

  if(L7_NULLPTR == pReqAddrLease && ipAddrRequested != 0)
  {
    /* excluded? */
    if(dhcpsCheckIfExcluded(pPool, ipAddrRequested, L7_TRUE) == L7_TRUE)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
        "Ignoring discover requested IP option (excluded IP)\n");
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "process DISCOVER allocating lease for requested IP\n");
        if(dhcpsCreateFreeLease(pPool, ipAddrRequested, clientID, clientIdLen, pDhcpPacket,
                                dhcpPacketLength, leaseTimeRequested,
                                incomingIfNum, L7_FALSE) != L7_SUCCESS)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
            "process DISCOVER unable to create free lease for requested IP %x\n",
          ipAddrRequested);
      }
      else
      {
        createFreeLease = L7_FALSE;
      }
    }
  }
    else if(L7_NULLPTR != pReqAddrLease)
  {
    /* have a lease for Requested IP to offer */

    /* transition lease to offered */
    dhcpsTransitionLeaseState(pReqAddrLease, OFFERED_LEASE);
    dhcpsSendOfferMessage(pReqAddrLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;

    createFreeLease = L7_FALSE;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process DISCOVER requested ip %x OFFERED\n", LEASE_DATA(pReqAddrLease)->ipAddr);
  }
  else if(L7_NULLPTR != pExpiredLease)
  {
    /* have expired IP to offer */

    /* transition lease to offered */
    dhcpsTransitionLeaseState(pExpiredLease, OFFERED_LEASE);
    dhcpsSendOfferMessage(pExpiredLease, pDhcpPacket, incomingIfNum, pParameterList, option82);
    DHCPS_SET_LEASE_CONFIG_DATA_DIRTY;
    rc = L7_SUCCESS;

    createFreeLease = L7_FALSE;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
      "process DISCOVER Offered Expired IP %x\n", LEASE_DATA(pExpiredLease)->ipAddr);
  }
  }
  else
  {
    createFreeLease = L7_FALSE;

    /* Send the ping request and let the pingTask handle the reachability
     * detection asynchronously */
    if(dhcpsSendEchoRequest(expiredLeaseIpAddr, 0) == L7_SUCCESS)
    {
      /* create an async message entry for this expired lease */
      for (i=0; i<L7_DHCPS_MAX_ASYNC_MSGS; i++)
      {
        if(pDhcpsAsyncInfo[i].ipAddr == 0)
        {
          /* found a free slot, fill async msg contents here */

          pDhcpsAsyncInfo[i].ipAddr           = expiredLeaseIpAddr;
          pDhcpsAsyncInfo[i].ipAddrShadow     = expiredLeaseIpAddr;
          pDhcpsAsyncInfo[i].dhcpPacketLength = dhcpPacketLength;
          pDhcpsAsyncInfo[i].intIfNum         = incomingIfNum;
          pDhcpsAsyncInfo[i].pingsIssued      = 0;
          pDhcpsAsyncInfo[i].getNextFreeIp    = L7_FALSE;
          pDhcpsAsyncInfo[i].pingTimeoutCnt   = ((DHCPS_PING_TIMEOUT) / (DHCPS_PING_SELECT_TIMEOUT));
          memcpy(pDhcpsAsyncInfo[i].dhcpPacketData, pDhcpPacket, L7_DHCP_PACKET_LEN);

          rc = L7_SUCCESS;
          break;
        }
      }

      if(i == L7_DHCPS_MAX_ASYNC_MSGS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
                "Failed to process DHCP Discover message"
                " as maximum(%d) discovery messages are being processed.",
                L7_DHCPS_MAX_ASYNC_MSGS);
        rc = L7_FAILURE;
      }
    }
    else
    {
      createFreeLease = L7_TRUE;
    }
  }

  if(createFreeLease == L7_TRUE)
  {
    /* lease out a free address */
    while (dhcpsGetFreeIpAddr(pPool, &ipAddr) == L7_SUCCESS)
    {
      /* Lease not found.
      * Try to get next free ip for the free lease in pingTask context */
      rc = dhcpsCreateFreeLease(pPool, ipAddr, clientID, clientIdLen, pDhcpPacket,
                                dhcpPacketLength, leaseTimeRequested,
                                incomingIfNum, L7_TRUE);

      if((rc == L7_SUCCESS) || (rc == L7_TABLE_IS_FULL))
      {
        /* Means, we either created a free lease entry and posted to pingTask
         * or the number of free leases being processed by ping task reached max */
        break;
      }
    }

    if(rc != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "No free IP address to offer in pool %x\n", POOL_DATA(pPool)->ipAddr);
    }
    else
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                    "process DHCP DISCOVER posted to pingTask");
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if BROADCAST bit in the flags field of the packet is on
*
* @param    pDhcpPacket   @b{(input)} Pointer to the dhcp Packet
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsIsBootPBroadcast(dhcpsPacket_t *pDhcpPacket)
{
  L7_BOOL   BROADCAST_REQUEST;

  /* check for BROADCAST field in the packet.. if it's ON, broadcast the reply, otherwise Unicast it*/
  /*BOOTP_BROADCAST */
  if( osapiNtohl(pDhcpPacket->header.flags) & (DHCPS_BOOTP_BROADCAST))
  {
    BROADCAST_REQUEST = L7_TRUE;
  }
  else
  {
    BROADCAST_REQUEST = L7_FALSE;
  }

  return  BROADCAST_REQUEST;
}
/*********************************************************************
* @purpose  Send Dhcp reply back to Client
*
* @param    pDhcpPacket      @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    intIfNum         @b{(input)} Interface Number
* @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSendPacket(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength,
                        L7_uint32 intIfNum, L7_BOOL isSysMgmtIntf)
{
  /* send via relay? */
  if(pDhcpPacket->header.giaddr != 0)
  {
    /* unicast to relay */
    if(dhcpsPacketUnicast(pDhcpPacket, dhcpPacketLength,
            osapiNtohl(pDhcpPacket->header.giaddr), DHCPS_SERVER_PORT) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unicast failed to IP %x\n", pDhcpPacket->header.giaddr);
      return L7_FAILURE;
    }
  }
  else
  {
    /* no relay! */

    /* client has an address? */
    if(pDhcpPacket->header.ciaddr != 0)
    {
      /* unicast to ciaddr */
      if(dhcpsPacketUnicast(pDhcpPacket, dhcpPacketLength,
            osapiNtohl(pDhcpPacket->header.ciaddr), DHCPS_CLIENT_PORT) != L7_SUCCESS)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unicast failed to IP %x\n", pDhcpPacket->header.ciaddr);
        return L7_FAILURE;
      }
    }
    else
    {
      /* client does not have an address => broadcast requested? */
      if (pDhcpPacket->header.yiaddr == 0 || dhcpsIsBootPBroadcast(pDhcpPacket) == L7_TRUE)
      {
        /* broadcast*/
        if(dhcpsRawPacketSend(intIfNum, pDhcpPacket, dhcpPacketLength,
                              0, 0, L7_NULLPTR, isSysMgmtIntf) != L7_SUCCESS)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Broadcast failed in interface IP %u\n", intIfNum);
          return L7_FAILURE;
        }
      }
      else
      {
        /* raw unicast to chaddr and yiaddr */
        if(dhcpsRawPacketSend(intIfNum, pDhcpPacket, dhcpPacketLength,
              osapiNtohl(pDhcpPacket->header.yiaddr), DHCPS_CLIENT_PORT,
              pDhcpPacket->header.chaddr, isSysMgmtIntf) != L7_SUCCESS)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Raw Unicast failed to IP %x\n",
                        osapiNtohl(pDhcpPacket->header.yiaddr));
          return L7_FAILURE;
        }
      }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sends a dhcp Pakcet to a Unicast IP
*
* @param    pDhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength  @b{(input)} length of the Dhcp Packet
* @param    destIPAddr        @b{(input)} Destination IP Address
* @param    destPort          @b{(input)} Destination Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsPacketUnicast(dhcpsPacket_t *pDhcpPacket, L7_uint32 dhcpPacketLength, L7_IP_ADDR_t destIPAddr, L7_ushort16 destPort)
{
  L7_uint32      dhcpsSocketS;
  L7_uint32      opt = 1;
  L7_uint32      bytesSent;
  L7_RC_t        rc;
  L7_sockaddr_in_t saddr, srcaddr;


  rc = osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &dhcpsSocketS);
  if(rc != L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"dhcpsPacketUnicast : osapiSocketCreate failed creating server socket\n");
    return L7_FAILURE;
  }

  srcaddr.sin_family = L7_AF_INET;
  srcaddr.sin_port = osapiHtons(DHCPS_SERVER_PORT);
  srcaddr.sin_addr.s_addr = L7_INADDR_ANY;

  rc = osapiSocketBind(dhcpsSocketS, (L7_sockaddr_t *)&srcaddr, sizeof(srcaddr));
  if(rc != L7_SUCCESS) {
    /* log the problem but send the message anyway */
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"dhcpsPacketUnicast : osapiSocketBind failed\n");
  }

  if(osapiSetsockopt(dhcpsSocketS, L7_SOL_SOCKET, L7_SO_REUSEADDR, (L7_uchar8 *)&opt, sizeof(opt)) != L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"dhcpsPacketUnicast : osapiSetsockopt failed on server socket\n");
    return L7_FAILURE;
  }

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons(destPort);
  saddr.sin_addr.s_addr = osapiHtonl(destIPAddr);
  rc = osapiSocketSendto(dhcpsSocketS ,(L7_uchar8 *)(pDhcpPacket),
                               dhcpPacketLength, 0, (L7_sockaddr_t *)&saddr,sizeof(saddr), &bytesSent);
  if(rc != L7_SUCCESS)
  {
    pDhcpsInfo->dhcpsStats.numOfBootpDhcpPacketsDiscarded++;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,"Request could not be relayed to Server\n");
    osapiSocketClose(dhcpsSocketS);
    return L7_FAILURE;
  }
  else
  {
    osapiSocketClose(dhcpsSocketS);
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Send Bootp reply back to Client
*
* @param    pDhcpPacket      @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    intIfNum         @b{(input)} Interface number
* @param    offeredIP        @b{(input)} Offered IP
* @param    isSysMgmtIntf    @b{(input)} indicates send on mgmt interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSendBootpReplyPacket(dhcpsPacket_t *pDhcpPacket,
                                  L7_uint32 dhcpPacketLength,
                                  L7_uint32 intIfNum, L7_IP_ADDR_t offeredIP,
                                  L7_BOOL isSysMgmtIntf)
{
  if (pDhcpPacket->header.giaddr != 0)
  {
    /* unicast to server port on relay */
    if(dhcpsPacketUnicast(pDhcpPacket, dhcpPacketLength,
          osapiNtohl(pDhcpPacket->header.giaddr), DHCPS_SERVER_PORT) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unicast failed to IP %x port %u\n",
        pDhcpPacket->header.giaddr, DHCPS_SERVER_PORT);
      return L7_FAILURE;
    }
  }
  else if (pDhcpPacket->header.ciaddr != 0)
  {
    /* unicast to client */
    if(dhcpsPacketUnicast(pDhcpPacket, dhcpPacketLength,
          osapiNtohl(pDhcpPacket->header.ciaddr), DHCPS_CLIENT_PORT) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Unicast failed to IP %x port %u\n",
           osapiNtohl(pDhcpPacket->header.ciaddr), DHCPS_CLIENT_PORT);
      return L7_FAILURE;
    }
  }
  else
  {
    /* Broadcast to 0xffffffff */
    if(dhcpsRawPacketSend(intIfNum, pDhcpPacket, dhcpPacketLength,
                          0, 0, L7_NULLPTR, isSysMgmtIntf) != L7_SUCCESS)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Broadcast failed in interface IP %u\n", intIfNum);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Append value of the TLV structure to the DHCP message buffer
*
* @param    pOption          @b{(input/output)} Pointer to the DHCP option
* @param    pValue           @b{(input)} Ponter to value of DHCP option
* @param    valueLen         @b{(input)} Length of the DHCP option
* @param    bufLen           @b{(input)} Length of the buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsTLVOptionAppendValue(dhcpsOption_t * pOption, L7_uchar8 * pValue, L7_uint32 valueLen, L7_uint32 bufLen)
{
  L7_uchar8 * pDest = L7_NULLPTR;

  if(L7_NULLPTR == pOption || L7_NULLPTR == pValue)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED, "Error: dhcpsTLVOptionAppendValue got null args\n");
    return L7_ERROR;
  }

  /* enough space to hold TLV and existing value length + new valueLen? */
  if(bufLen + (pOption->length + DHCPS_OPTION_VALUE_OFFSET + valueLen) > L7_DHCP_PACKET_LEN)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsTLVOptionAppendValue exceeded max msg length for option %u\n",
                  pOption->type);
    return L7_FAILURE;
  }

  /* append (concatenate) value */
  pDest = pOption->value + pOption->length;
  memcpy(pDest, pValue, valueLen);

  /* incr. length */
  pOption->length += valueLen;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Add single byte option to the DHCP message buffer
*
* @param    optionType       @b{(input)} Type of option
* @param    pMsg             @b{(input)} Poninter to Message Structure
* @param    pBufLen          @b{(input/output)} Pointer to length of the buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAddSingleByteOption(L7_uchar8 optionType, dhcpsPacket_t *pMsg , L7_uint32 * pBufLen)
{
  dhcpsOption_t *pOption;
  if(L7_NULLPTR == pMsg || L7_NULLPTR == pBufLen)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsAddSingleByteOption got null args for option %u\r\n",
                  optionType);
    return L7_ERROR;
  }

  if(*pBufLen < L7_DHCP_PACKET_LEN)
  {
    pOption = (dhcpsOption_t*) ((L7_uchar8 *)(pMsg)+ *pBufLen);
    pOption->type = optionType;
    *pBufLen = *pBufLen + 1;
    return L7_SUCCESS;

  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                "Error: dhcpsAddSingleByteOption exceeded max msg length for option %u\r\n",
                optionType);

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Save the DHCP option
*
* @param    pLastOption      @b{(input)}  Pointer to the DHCP option
* @param    ppMsgBuf         @b{(output)} Pointer to pointer to option
*                                         of dhcp message
* @param    pBufLen         @b{(output)}  Pointer to buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsSaveTLVOption(dhcpsOption_t *pLastOption, L7_uchar8 ** ppMsgBuf, L7_uint32 *pBufLen)
{
  if(L7_NULLPTR == pLastOption)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsSaveTLVOption got null DHCP option\n");
    return L7_ERROR;
  }

  if(L7_NULLPTR == ppMsgBuf || L7_NULLPTR == pBufLen)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsSaveTLVOption got null args for option %u\n",
                  pLastOption->type);
    return L7_ERROR;
  }

  if(L7_NULLPTR == *ppMsgBuf)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsSaveTLVOption got msgbuf for option %u\n",
                  pLastOption->type);
    return L7_ERROR;
  }

  *pBufLen  =  *pBufLen + pLastOption->length + DHCPS_OPTION_VALUE_OFFSET;
  *ppMsgBuf = (*ppMsgBuf) + (pLastOption->length + DHCPS_OPTION_VALUE_OFFSET);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate a DHCP option code
*
* @param    code      @b{(input)}  DHCP option code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsValidateOptionCode(L7_uchar8 code)
{
  /* check if configuration is explictly dis-allowed */
  if(DHCPS_OPTION_NOT_ALLOWED == pDhcpsInfo->dhcpsOptionInfo[code].isAllowed)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "Option validation failed for code %x: Configuration not allowed\r\n",
      code);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Validate a DHCP option
*
* @param    code      @b{(input)}  DHCP option code
* @param    data      @b{(input)}  DHCP option data
* @param    length    @b{(input)}  DHCP option length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsValidateOption(L7_uchar8 code, L7_uchar8 * data, L7_uint32 length)
{
  L7_ushort16 tempShort= 0;
  L7_ushort16 i= 0;

  /* check if code configuration is allowed */
  if(dhcpsValidateOptionCode(code) != L7_SUCCESS)
    return L7_FAILURE;

  /* enforce RFC2132 length */
  if(DHCPS_OPTION_RFC2132 == pDhcpsInfo->dhcpsOptionInfo[code].isAllowed)
  {
    /* all 2132 options have a 1 byte length */
    if(length > 255)
    {
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
        "Option validation failed for code %x: rfc 2132 options cannot exceed 255\r\n",
        code);
      return L7_FAILURE;
    }
  }

  /* check for fixed length */
  if(pDhcpsInfo->dhcpsOptionInfo[code].fixedLength > 0 &&
    length != pDhcpsInfo->dhcpsOptionInfo[code].fixedLength)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "Option validation failed for code %d: Length must be %d\r\n",
       code, pDhcpsInfo->dhcpsOptionInfo[code].fixedLength);
      return L7_FAILURE;
  }

  /* check for min length */
  if(pDhcpsInfo->dhcpsOptionInfo[code].minLength > 0 &&
      length < pDhcpsInfo->dhcpsOptionInfo[code].minLength)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "Option validation failed for code %d: Length must be more than %d\r\n",
       code, pDhcpsInfo->dhcpsOptionInfo[code].minLength);
      return L7_FAILURE;
  }

  /* check for multiple of */
  if(pDhcpsInfo->dhcpsOptionInfo[code].multipleOf > 0 &&
      0 != (length % pDhcpsInfo->dhcpsOptionInfo[code].multipleOf))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      "Option validation failed for code %d: Length must be multiple of %d\r\n",
       code, pDhcpsInfo->dhcpsOptionInfo[code].multipleOf);
      return L7_FAILURE;
  }

  /* check for option code specific data validations */
  switch(code)
  {
    case 19: /* IP Forwarding Enable/Disable Option */
    case 20: /* Non-Local Source Routing Enable/Disable Option */
    case 27: /* All Subnets are Local Option */
    case 29: /* Perform Mask Discovery Option */
    case 30: /* Mask Supplier Option */
    case 31: /* Perform Router Discovery Option */
    case 34: /* Trailer Encapsulation Option */
    case 36: /* Ethernet Encapsulation Option */
    case 39: /* TCP Keepalive Garbage Option */
      /* check value is 0 or 1 */
      if(*data != 0 && *data != 1)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "Option validation failed code %d: Value must be 0 or 1\r\n", code);
        return L7_FAILURE;
      }
      break;

    case 22: /* Maximum Datagram Reassembly Size */
      /* check value is >= 576 */
      tempShort = *((L7_ushort16*)data);
      if(tempShort < 576)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "Option validation failed code %d: Value must be 576 or larger\r\n", code);
        return L7_FAILURE;
      }
      break;

    case 23: /* Default IP Time-to-live */
      /* check value is >= 1 */
      tempShort = *((L7_ushort16*)data);
      if(tempShort < 1 )
      {
        /* There is no need to verify that value should be less than 256 as
           option length (1byte) already verified, so value can't be greater
           than 255 */
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "Option validation failed code %d: Value must be between 1 and 255\r\n", code);
        return L7_FAILURE;
      }
      break;

    case 25: /* Path MTU Plateau Table Option */
      /* check value is >= 68 for each MTU */
      for (i =0; i < length; )
      {
        tempShort = *((L7_ushort16*)(data + i));
        i = i + 2;

        if(tempShort < 68)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
            "Option validation failed code %d: Value must be 68 or larger\r\n", code);
          return L7_FAILURE;
        }
      }
      break;

    case 26: /* Interface MTU Option */
      /* check value is >= 68 */
      tempShort = *((L7_ushort16*)data);
      if(tempShort < 68)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "Option validation failed code %d: Value must be 68 or larger\r\n", code);
        return L7_FAILURE;
      }
      break;

    case 37: /* TCP Default TTL Option */
      /* TBD check value is > 1 */
      if(*data == 0)
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "Option validation failed code %d: Value must be 1 or larger\r\n", code);
        return L7_FAILURE;
      }
      break;
    }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add Options to the DHCP message buffer
*
* @param    pPool            @b{(input)} Pointer to the lease node
* @param    pMsg             @b{(input)} Pointer to Message Buffer
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    pParameterList   @b{(input)} Option Parameter List
* @param    pOverloadState    @b{(output)} To indicate option overload type possible
* @param    clientOverload   @b{(output)} Client supports split or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAppendOptions(dhcpsPoolNode_t * pPool,
                          dhcpsPacket_t *pMsg,
                          L7_uint32 *dhcpPacketLength,
                          dhcpsOption_t *pParameterList,
                          L7_uchar8 *pOverloadState,
                          L7_BOOL clientOverload)

{
  L7_uint32      i;
  L7_uint32      tempCode;
  L7_BOOL        packetPoolOptions[L7_DHCPS_OPTION_CODE_MAX];
  L7_RC_t        rc;

  memcpy(packetPoolOptions,pPool->optionFlag, sizeof(packetPoolOptions));
  if (pParameterList != L7_NULLPTR)
  {
    for (i =0; i < pParameterList->length; i++)
    {
      tempCode = pParameterList->value[i];

      if (packetPoolOptions[tempCode] == L7_TRUE)
      {
        /* Set the value to L7_FALSE if configured option has been appended*/
        packetPoolOptions[tempCode] = L7_FALSE;
        rc = dhcpsAddFixedOption(pPool, pMsg, dhcpPacketLength, tempCode, pOverloadState, clientOverload);
        if( rc == L7_NOT_EXIST)
        {
          rc = dhcpsAddPackedOption(pPool, pMsg, dhcpPacketLength, tempCode, pOverloadState, clientOverload);
        }
        if( rc == L7_FAILURE)
        {
          DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
               "Failed to append value for option code %d\r\n", tempCode);
          return L7_FAILURE;
        }
      }
    }
  }

  for ( i =0; i < L7_DHCPS_OPTION_CODE_MAX; i++ )
  {
    if(packetPoolOptions[i] == L7_TRUE)
    {
      rc = dhcpsAddFixedOption(pPool, pMsg, dhcpPacketLength, i, pOverloadState,clientOverload);
      if( rc != L7_NOT_EXIST)
        packetPoolOptions[i] = L7_FALSE;
      if( rc == L7_FAILURE)
      {
         /* This is the critical error case and we cann't proceeed further*/
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
               "Failed to append value for option code %d\r\n", i);
        return L7_FAILURE;
      }
    }
  }

  for ( i =0; i < L7_DHCPS_OPTION_CODE_MAX; i++ )
  {
    if(packetPoolOptions[i] == L7_TRUE)
    {
      /* Set the value to 0 if configured option has been appended*/
      packetPoolOptions[i] = L7_FALSE;

      rc = dhcpsAddPackedOption(pPool, pMsg, dhcpPacketLength, i, pOverloadState, clientOverload);
      if( rc == L7_FAILURE)
      {
         /* This is the case when maximum packet size reached, We should stop appending value and            should return back*/
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
               "Failed to append value for option code %d\r\n", i);
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To add fixed Options in the message.
*
* @param    pPool            @b{(input)} Pointer to the lease node
* @param    pMsg             @b{(input)} Pointer to Message Buffer
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    optionType       @b{(input)} Type of DHCP option
* @param    pOverloadState   @b{(output)} To indicate option overload type possible
* @param    clientOverload   @b{(output)} Client supports split or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   - In case of critical error.
* @returns  L7_ERROR     - If unable to add value in the packet.
* @returns  L7_NOT_EXIST - If code is not part of fixed option
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAddFixedOption(dhcpsPoolNode_t * pPool,
                       dhcpsPacket_t *pMsg,
                       L7_uint32 *dhcpPacketLength,
                       L7_uchar8  optionType,
                       L7_uchar8  *pOverloadState,
                       L7_BOOL clientOverload)
{
  L7_uchar8 buffer[DHCPS_MAX_OPTION_LEN];
  L7_uint32      longValue;
  L7_IP_ADDR_t * pIpAddr;
  L7_uint32      addrCount;
  L7_RC_t        rc = L7_NOT_EXIST;

  /* Check the option tye to be added and append the corresponding value in message field*/
  switch(optionType)
  {
    case DHCPS_OPTION_TYPE_SUBNET_MASK :
      longValue = osapiHtonl(POOL_DATA(pPool)->ipMask);

      rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_SUBNET_MASK,
                  (L7_uchar8*)&longValue, sizeof(longValue), pOverloadState, clientOverload);
      break;

    case DHCPS_OPTION_TYPE_ROUTER :
      memset(buffer, 0, sizeof(buffer));

      /* iterate Routers from pool */
      pIpAddr = POOL_DATA(pPool)->defaultrouterAddrs;
      addrCount = 0;
      while(*pIpAddr != 0 && addrCount < L7_DHCPS_DEFAULT_ROUTER_MAX)
      {
        longValue = osapiHtonl(*pIpAddr);
        memcpy( (L7_uchar8*) &buffer[addrCount* sizeof(L7_IP_ADDR_t)], (L7_uchar8*)&longValue,sizeof(L7_IP_ADDR_t));
        pIpAddr++;
        addrCount++;
      }

      /* skip if no routers */
      if(addrCount> 0)
      {
        rc = dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_ROUTER,
                buffer, addrCount*sizeof(L7_IP_ADDR_t), pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption:  Value is not configured for this option %u: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;


    case DHCPS_OPTION_TYPE_DNS_SERVER:
      memset(buffer, 0, sizeof(buffer));
      /* iterate DNSs from pool */
      pIpAddr = POOL_DATA(pPool)->DNSServerAddrs;
      addrCount = 0;
      while(*pIpAddr != 0 && addrCount < L7_DHCPS_DNS_SERVER_MAX)
      {
        longValue = osapiHtonl(*pIpAddr);
        memcpy( (L7_uchar8*) &buffer[addrCount* sizeof(L7_IP_ADDR_t)], (L7_uchar8*)&longValue,sizeof(L7_IP_ADDR_t));
        pIpAddr++;
        addrCount++;
      }

      /* skip if no DNSs */
      if(addrCount> 0)
      {
          rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_DNS_SERVER,
                  (L7_uchar8*) buffer, addrCount*sizeof(L7_IP_ADDR_t), pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;

    case DHCPS_OPTION_TYPE_HOST_NAME:
      longValue = dhcpsStrnlen(POOL_DATA(pPool)->hostName,L7_DHCPS_HOST_NAME_MAXLEN);
      if(MANUAL_POOL == POOL_DATA(pPool)->poolType && longValue > 0)
      {
        rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_HOST_NAME,
                  POOL_DATA(pPool)->hostName, longValue, pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;

    case DHCPS_OPTION_TYPE_DOMAINNAME:
      longValue = dhcpsStrnlen(POOL_DATA(pPool)->domainName, L7_DHCPS_DOMAIN_NAME_MAXLEN);
      if(longValue > 0)
      {
        rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_DOMAINNAME,
                  (L7_uchar8*)POOL_DATA(pPool)->domainName, longValue, pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;


    case DHCPS_OPTION_TYPE_NETBIOSNAMESERVER:
      memset(buffer, 0, sizeof(buffer));
      /* iterate DNSs from pool */
      pIpAddr = POOL_DATA(pPool)->netBiosNameServer;
      addrCount = 0;
      while(*pIpAddr != 0 && addrCount < L7_DHCPS_DNS_SERVER_MAX)
      {
        longValue = osapiHtonl(*pIpAddr);
        memcpy( &buffer[addrCount* sizeof(L7_IP_ADDR_t)], (L7_uchar8*)&longValue,sizeof(L7_IP_ADDR_t));
        pIpAddr++;
        addrCount++;
      }

      /* skip if no DNSs */
      if(addrCount> 0)
      {
        rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_NETBIOSNAMESERVER, buffer, addrCount*sizeof(L7_IP_ADDR_t), pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;


    case DHCPS_OPTION_TYPE_NETBIOSNODETYPE:
      if(POOL_DATA(pPool)->netBiosNodeType != 0)
      {
        longValue = 1;
        rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_NETBIOSNODETYPE,
                  (L7_uchar8*) &POOL_DATA(pPool)->netBiosNodeType, longValue, pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
        rc =  L7_ERROR;
      }
      break;
    case DHCPS_OPTION_TYPE_BOOTFILE_NAME:
      longValue = dhcpsStrnlen(POOL_DATA(pPool)->clientBootFile, L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
      if(longValue > 0)
      {
        if( pMsg->header.file[0] == '\0')
        {
          /* Copy the bootfile field*/
          memcpy(&pMsg->header.file, POOL_DATA(pPool)->clientBootFile, L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
          rc = L7_SUCCESS;
        }
        else
          rc =  dhcpsAddTLVOption(pMsg, dhcpPacketLength, DHCPS_OPTION_TYPE_BOOTFILE_NAME,
                  (L7_uchar8*)POOL_DATA(pPool)->clientBootFile, longValue, pOverloadState, clientOverload);
      }
      else
      {
        DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
          "dhcpsAddFixedOption: Value is not configured for this option %d: \r\n",optionType);
      }
      break;

    default :
      break;
  }
  return rc;
}

/*********************************************************************
* @purpose  To add an option when option has been stored in packed format.
*
* @param    pPool         @b{(input)} Pointer to the pool node
* @param    pMsg          @b{(input)} Pointer to Message Buffer
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    optionType       @b{(input)} Type of DHCP option
* @param    pOverloadState     @b{(output)} To indicate option overload type possible
* @param    clientOverload   @b{(output)} Client supports split or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   - In case of critical error.
* @returns  L7_ERROR     - If unable to add value in the packet.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAddPackedOption(dhcpsPoolNode_t * pPool,
                       dhcpsPacket_t *pMsg,
                       L7_uint32 *dhcpPacketLength,
                       L7_uchar8  optionType,
                       L7_uchar8  *pOverloadState,
                       L7_BOOL clientOverload)
{
  L7_uchar8 buffer[DHCPS_MAX_OPTION_LEN];
  L7_uint32      longValue;
  L7_uchar8      dataFormat, status;

  /* IF option is present in options (compact form) array*/
  if( dhcpsGetTLVOption((L7_uchar8*)POOL_DATA(pPool)->options, optionType, buffer, &longValue, &dataFormat, &status) != L7_SUCCESS || status == L7_DHCPS_NOT_READY)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
      " dhcpsAddPackedOption: Value is not configured for this option %d: \r\n",optionType);
    return L7_ERROR;
  }
  else
  {
    return dhcpsAddTLVOption(pMsg, dhcpPacketLength, optionType,
           buffer, longValue, pOverloadState,clientOverload);
  }
}

/*********************************************************************
* @purpose  Add Option to the DHCP message buffer
*
* @param    pMsg             @b{(output)} Pointer to dhcp packet
* @param    bufLen           @b{(output)} length of the buffer
* @param    optionType       @b{(input)} Type of DHCP option
* @param    pValue           @b{(input)} Ponter to value of DHCP option
* @param    valueLen         @b{(input)} Length of the DHCP option
* @param    pOverloadState        @b{(output)} To indicate option overload type possible
* @param    clntOverloadSupported @b{(output)} Client supports split or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAddTLVOption(dhcpsPacket_t* pMsg, L7_uint32* pBufLen, L7_uchar8 optionType, L7_uchar8 * pValue, L7_uint32 valueLen, L7_uchar8* pOverloadState, L7_BOOL clntOverloadSupported)
{
  dhcpsOption_t *pOption = (dhcpsOption_t*) ((L7_uchar8 *) (pMsg)+ *pBufLen);
  dhcpsOption_t *pTemp;

  L7_int32 optAllowedLength;
  L7_int32 optOverflowAllowLen;
  L7_int32 optFileLen;
  L7_int32 optSnameLen;
  L7_int32 optFileSnameLen;

  L7_int32 fileNameLen;
  L7_int32 fieldTempLength;

  if(L7_NULLPTR == pMsg || L7_NULLPTR == pValue)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsAddTLVOption got null args for option %u\n",
                  optionType);
    return L7_FAILURE;
  }

  if(DHCPS_BOOTP_PKT_LENGTH < (*pBufLen + DHCPS_OPTION_VALUE_OFFSET + valueLen +1) && *pOverloadState ==  DHCPS_NOT_OVERLOADED)
  {
    return L7_ERROR;
  }

  optAllowedLength = DHCPS_OPTION_FIELD_LENGTH - *pBufLen - DHCPS_OPTION_VALUE_OFFSET; /*BuffLen - TL offset length*/

  /* Maximum option length allowed in field is 255 bytes*/
  optAllowedLength = optAllowedLength>DHCPS_MAX_UNDIVIDED_OPT_LENGTH ? DHCPS_MAX_UNDIVIDED_OPT_LENGTH:optAllowedLength;


  /* Maximum option length allowed in field is 255 bytes when overload option need to insert*/
  optOverflowAllowLen = DHCPS_OPTION_FIELD_LENGTH - *pBufLen - DHCPS_OPTION_VALUE_OFFSET - DHCPS_OVERFLOW_OPT_LENGTH;

  optOverflowAllowLen = optOverflowAllowLen>DHCPS_MAX_UNDIVIDED_OPT_LENGTH ? DHCPS_MAX_UNDIVIDED_OPT_LENGTH:optOverflowAllowLen;


  /* Maximum option length that can be copied if file and option buffer to be used*/
  fileNameLen  = dhcpsStrnlen((pMsg->header).file, DHCPS_FILE_LEN);
  fileNameLen = fileNameLen > DHCPS_FILE_LEN? DHCPS_FILE_LEN:fileNameLen;
  if( fileNameLen == 0)
  {
    optFileLen = optOverflowAllowLen + DHCPS_FILE_LEN - DHCPS_FIELD_USED_COST;
                 /* 1 byte for end field and 2 for opt field to be used in file field*/
  }
  else
  {
    fieldTempLength = DHCPS_OPTION_FIELD_LENGTH - (*pBufLen + 2*DHCPS_OPTION_VALUE_OFFSET +DHCPS_OVERFLOW_OPT_LENGTH +fileNameLen);  /* Two offset for option and file field )*/

    fieldTempLength = fieldTempLength>DHCPS_MAX_UNDIVIDED_OPT_LENGTH ? DHCPS_MAX_UNDIVIDED_OPT_LENGTH:fieldTempLength;

    optFileLen = DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST + fieldTempLength;  /* Option allowed length + File allowed length*/
  }

  /* Maximum option length that can be copied if sname and option buffer to be used*/
  optSnameLen = optOverflowAllowLen + DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST;

  /* Maximum option length that can be copied if sname, file  and option buffer to be used*/
  optFileSnameLen = optFileLen + DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST;


  if( (valueLen > DHCPS_MAX_UNDIVIDED_OPT_LENGTH)  && (*pOverloadState == DHCPS_NOT_OVERLOADED  || *pOverloadState ==  DHCPS_BOTH_OVERLOADED || clntOverloadSupported == L7_FALSE) )
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsAddTLVOption exceeded max msg length for option %u\r\n",
                  optionType);
    return L7_ERROR;
  }
  else if(optAllowedLength < (L7_int32) valueLen && (*pOverloadState == DHCPS_NOT_OVERLOADED || *pOverloadState == DHCPS_BOTH_OVERLOADED ))
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsAddTLVOption exceeded max msg length for option %u\r\n",
                  optionType);
    return L7_ERROR;
  }
  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
     "Info: dhcpsAddTLVOption Max Allow length = %d , value len = %u \r\n",
      optOverflowAllowLen, valueLen);
  /* Start Packing*/
  if ( optOverflowAllowLen >= (L7_int32) valueLen )
  {
    dhcpsCopyTLV(pOption, optionType, pValue, valueLen);
    *pBufLen   = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
                  "Info: dhcpsAddTLVOption option type %u added in option field\r\n ",
                  optionType);
    return L7_SUCCESS;
  }

  if( optAllowedLength >= (L7_int32) valueLen && *pOverloadState == DHCPS_NOT_OVERLOADED)
  {
    dhcpsCopyTLV(pOption, optionType, pValue, valueLen);
    *pBufLen   = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
                  "Info: dhcpsAddTLVOption option type %u added in option field\r\n ",
                  optionType);
    return L7_SUCCESS;
  }

  /* Case where option can come in sname+ opt field */
  if( optSnameLen >= (L7_int32) valueLen  && (*pOverloadState != DHCPS_SNAME_OVERLOADED) && (*pOverloadState !=DHCPS_BOTH_OVERLOADED))
  {
    /* Put the overflow option type 2 */
    if(*pOverloadState == DHCPS_BOTH_CAN_BE_OVERLOADED)
    {
      *pOverloadState = DHCPS_SNAME_OVERLOADED;
    }
    else if( *pOverloadState == DHCPS_FILE_OVERLOADED)
    {
      *pOverloadState = DHCPS_BOTH_OVERLOADED;
    }

    fieldTempLength = valueLen - (DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST);
    if (valueLen > (DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST))
    {
      /* Copy the First fragment into option field*/
      dhcpsCopyTLV(pOption, optionType, pValue, fieldTempLength);

      /* Copy the Second fragment into sname field*/
      dhcpsCopyTLV((dhcpsOption_t*) (pMsg->header).sname, optionType, pValue+ fieldTempLength, DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST );

      pTemp = (dhcpsOption_t*) (pMsg->header.sname + DHCPS_SNAME_LEN -1);

      /* Put the End field in SNAME Field*/
      pTemp->type = DHCPS_OPTION_TYPE_END;

      *pBufLen = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
    }
    else
    {
      /* Copy the entire portion in sname field*/
      dhcpsCopyTLV((dhcpsOption_t*) (pMsg->header.sname), optionType, pValue, valueLen);

      pTemp = (dhcpsOption_t*) (pMsg->header.sname + valueLen+DHCPS_OPTION_VALUE_OFFSET);

      /* Put the End field in SNAME Field*/
      pTemp->type = DHCPS_OPTION_TYPE_END;

      /* Put the PAD field in rest sname space*/
      if( DHCPS_SNAME_LEN - valueLen - DHCPS_FIELD_USED_COST >0)
        memset(pMsg->header.sname + valueLen + DHCPS_FIELD_USED_COST, 0, DHCPS_SNAME_LEN - valueLen -DHCPS_FIELD_USED_COST);
    }

    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
                "Info: dhcpsAddTLVOption Option is overloaded and using the sname field\r\n");
    return L7_SUCCESS;
  }

  /* Case where option can come in file+ opt field */
  if( optFileLen >= (L7_int32) valueLen   && *pOverloadState != DHCPS_FILE_OVERLOADED && (*pOverloadState != DHCPS_BOTH_OVERLOADED))
  {
    /* Check if it is possible to move the file field or not*/
    if( fileNameLen ==0 || optOverflowAllowLen >=  fileNameLen )
    {
      /* Put the overflow option type 1 */
      if(*pOverloadState == DHCPS_BOTH_CAN_BE_OVERLOADED)
      {
        *pOverloadState = DHCPS_FILE_OVERLOADED;
      }
      else if( *pOverloadState == DHCPS_SNAME_OVERLOADED)
      {
        *pOverloadState = DHCPS_BOTH_OVERLOADED;
      }

      /* Copy the file value in option field*/
      if(fileNameLen >0)
      {
        dhcpsCopyTLV(pOption, DHCPS_OPTION_TYPE_BOOTFILE_NAME, pMsg->header.file, fileNameLen);
        *pBufLen = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
        pOption = (dhcpsOption_t*) ((L7_uchar8*) pMsg+ *pBufLen);
      }

      fieldTempLength = valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST);
      if( valueLen > (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST))
      {
        /* Copy the First fragment into option field*/
        dhcpsCopyTLV(pOption, optionType, pValue, fieldTempLength);

        /* Copy Rest in file field*/
        dhcpsCopyTLV((dhcpsOption_t*) pMsg->header.file, optionType, pValue+fieldTempLength, valueLen - fieldTempLength);

        pTemp = (dhcpsOption_t*) (pMsg->header.file + DHCPS_FILE_LEN -1);

        /* Put the End field in file Field*/
        pTemp->type = DHCPS_OPTION_TYPE_END;

        *pBufLen = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
        pOption = (dhcpsOption_t*) ((L7_uchar8*) pMsg + *pBufLen);
      }
      else
      {
        /* Copy the entire portion in file field*/
        dhcpsCopyTLV((dhcpsOption_t*) (pMsg->header).file, optionType, pValue, valueLen);

        pTemp = (dhcpsOption_t*) (pMsg->header.file + valueLen+2);

        /* Put the End field in FILE Field*/
        pTemp->type = DHCPS_OPTION_TYPE_END;

        /* Put the PAD field in rest file space*/
        if( DHCPS_FILE_LEN - valueLen - DHCPS_FIELD_USED_COST >0)
          memset(pMsg->header.file + valueLen + DHCPS_FIELD_USED_COST, 0, DHCPS_FILE_LEN - valueLen -DHCPS_FIELD_USED_COST);
      }
      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
           "Info: dhcpsAddTLVOption Option is overloaded and using the file field\r\n");
      return L7_SUCCESS;
    }
  }

  /* Case where option need to be divided into 3 part*/
  if( optFileSnameLen>= (L7_int32) valueLen  && *pOverloadState == DHCPS_BOTH_CAN_BE_OVERLOADED)
  {
    if( fileNameLen ==0 || optOverflowAllowLen >=  fileNameLen )
    {
      /* Put the overflow option type 2 */
      *pOverloadState = DHCPS_BOTH_OVERLOADED;

      /* Copy the file value in option field*/
      if(fileNameLen >0)
      {
        dhcpsCopyTLV(pOption, DHCPS_OPTION_TYPE_BOOTFILE_NAME, pMsg->header.file, fileNameLen);
        *pBufLen   = pOption->length + fileNameLen + *pBufLen;
        pOption = (dhcpsOption_t*) ((L7_uchar8*) pMsg + *pBufLen);
      }

      fieldTempLength = valueLen - ((DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) + (DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST));
      if( valueLen > ((DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) + (DHCPS_SNAME_LEN -DHCPS_FIELD_USED_COST)))
      {
        /* Copy the First fragment into option field*/
        dhcpsCopyTLV(pOption, optionType, pValue, fieldTempLength);

        /* Copy Second Fragement in file field*/
        dhcpsCopyTLV((dhcpsOption_t*) pMsg->header.file, optionType, pValue+fieldTempLength, DHCPS_FILE_LEN - DHCPS_FIELD_USED_COST);

       /* Put the End field in file Field*/
       pTemp = (dhcpsOption_t*) (pMsg->header.file + DHCPS_FILE_LEN -1);
       pTemp->type = DHCPS_OPTION_TYPE_END;

       /* Copy Third Fragement in sname field*/
       dhcpsCopyTLV((dhcpsOption_t*) pMsg->header.sname, optionType, pValue+ fieldTempLength + DHCPS_FILE_LEN - DHCPS_FIELD_USED_COST, DHCPS_SNAME_LEN - DHCPS_FIELD_USED_COST);

       pTemp = (dhcpsOption_t*) (pMsg->header.sname + DHCPS_SNAME_LEN -1);
       /* Put the End field in SNAME Field*/
       pTemp->type = DHCPS_OPTION_TYPE_END;

       *pBufLen = pOption->length + DHCPS_OPTION_VALUE_OFFSET + *pBufLen;
       pOption = (dhcpsOption_t*) ((L7_uchar8*) pMsg + *pBufLen);
     }
     else
     {
        /* Copy First Fragement in file field*/
       dhcpsCopyTLV((dhcpsOption_t*) pMsg->header.file, optionType, pValue,DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST);

       /* Put the End field in file Field*/
       pTemp = (dhcpsOption_t*) (pMsg->header.file + DHCPS_FILE_LEN -1 );
       pTemp->type = DHCPS_OPTION_TYPE_END;


       /* Copy Second Fragement in sname field*/
       dhcpsCopyTLV((dhcpsOption_t*) pMsg->header.sname, optionType, pValue+ DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST, valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST));

      pTemp = (dhcpsOption_t*) (pMsg->header.sname + valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) +DHCPS_OPTION_VALUE_OFFSET);

      /* Put the End field in SNAME Field*/
      pTemp->type = DHCPS_OPTION_TYPE_END;

      /* Put the PAD field in rest sname space*/
      if(DHCPS_SNAME_LEN - (valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) +DHCPS_OPTION_VALUE_OFFSET+1) >0)
      memset(pMsg->header.sname + valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) +DHCPS_OPTION_VALUE_OFFSET+1, 0, DHCPS_SNAME_LEN - ( valueLen - (DHCPS_FILE_LEN -DHCPS_FIELD_USED_COST) +DHCPS_OPTION_VALUE_OFFSET+1));
     }

      DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_LO,
                  "Info: dhcpsAddTLVOption Option is overloaded and using the both fields\r\n");
      return L7_SUCCESS;
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Copt the TLV to the DHCP message buffer
*
* @param    pOption          @b{(output)} Pointer to dhcp option structure
* @param    optionType       @b{(input)} Type of DHCP option
* @param    pValue           @b{(input)} Ponter to value of DHCP option
* @param    valueLen         @b{(input)} Length of the DHCP option
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsCopyTLV(dhcpsOption_t* pOption, L7_uchar8 optionType, L7_uchar8 * pValue, L7_uint32 valueLen)
{

    pOption->type   = optionType;
    pOption->length = valueLen;
    memcpy(pOption->value, pValue, valueLen);
}

/*********************************************************************
* @purpose  check if client supports long options as per rfc 3396
*
* @param    requestList      @b{(output)} Pointer to list of requested options
* @param    length           @b{(input)} Length of the requested options list
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpsClientSupportsLongOptions(L7_uchar8 * requestList, L7_uchar8 length)
{
  /* trawl requested codes for one that is known to support
     long options as per rfc 3396
  */
  while(length > 0)
  {
    switch(*requestList)
    {
      case 121: /* Classless Static Route Option RFC 3442 */
        return L7_TRUE;
    }

    /* next code */
    requestList++;
    length--;
  }

  /* none matched! */
  return L7_FALSE;
}

/*********************************************************************
* @purpose  To add the Overload option
*
* @param    pMsg             @b{(input)} Poninter to Message Structure
* @param    pBufLen          @b{(input/output)} Pointer to length of the buffer
* @param    overloadState    @b{(output)} To indicate option overload type possible
*
* @returns  L7_ERROR
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpsAddOverloadOption(dhcpsPacket_t *pMsg, L7_uint32 * pBufLen, L7_uchar8 overloadState )
{
  dhcpsOption_t *pOption;
  if(L7_NULLPTR == pMsg || L7_NULLPTR == pBufLen)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsAddOverloadOption got null args \r\n");
    return L7_ERROR;
  }

  if( overloadState == DHCPS_BOTH_CAN_BE_OVERLOADED || overloadState == DHCPS_NOT_OVERLOADED)
  {
    return L7_SUCCESS;
  }
  else if(*pBufLen < L7_DHCP_PACKET_LEN -3)
  {
    pOption = (dhcpsOption_t*) ((L7_uchar8 *)(pMsg)+ *pBufLen);
    pOption->type = DHCPS_OPTION_TYPE_OPTION_OVERLOAD;
    pOption->length = 1;
    pOption->value[0]= overloadState;
    *pBufLen = *pBufLen + DHCPS_OVERFLOW_OPT_LENGTH;
    return L7_SUCCESS;
  }

  DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
       "Error: dhcpsAddOverloadOption exceeded max msg length for option %u\n",
       DHCPS_OPTION_TYPE_OPTION_OVERLOAD);

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Copy option 82 from a client packet to the reply
*
* @param    option82      @b{(input)}  pointer to option 82 in client packet
* @param    response      @b{(in/out)} Pointer to length of the buffer
* @param    responseLen   @b{(output)} length of response packet (bytes)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    If client packet includes option 82, copy option verbatim to
*           response. RFC 3046 says this should be the last option in the
*           response, and that it cannot go in the overloaded sname or
*           file fields.
*
* @end
*********************************************************************/
L7_RC_t dhcpsOption82Copy(dhcpsOption_t *option82, dhcpsPacket_t *response,
                          L7_uint32 *responseLen)
{
  dhcpsOption_t *pOption;
  L7_uint32 opt82Len;           /* number of bytes in option 82 (incl type/val fields) */

  if (!response || !option82)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: dhcpsOption82Copy got null arg \r\n");
    return L7_ERROR;
  }

  opt82Len = option82->length + 2;   /* add 2 for type and length fields */

  /* Make sure there is room for option 82. Leave room for END option. */
  if (*responseLen + opt82Len >= L7_DHCP_PACKET_LEN)
  {
    DHCPS_MAP_PRT(DHCPS_MAP_MSGLVL_MED,
                  "Error: Not enough room for option 82.\n");
    return L7_FAILURE;
  }

  pOption = (dhcpsOption_t*) ((L7_uchar8 *)(response) + *responseLen);
  pOption->type = DHCPS_OPTION_TYPE_AGENT_INFO;
  pOption->length = option82->length;
  memcpy(pOption->value, option82->value, option82->length);
  *responseLen += opt82Len;
  return L7_SUCCESS;
}

/***********************************************************************
 * @purpose  Register a component to inform type of DHCP server message
 *           going out.
 *
 * @param    notifyFunctionList   pointer to structure with function pointer
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendTypeNotifyRegister(L7_uint32 registrar_ID,
      L7_uint32 (*notify_func)(L7_uint32 sendMsgType,
                               L7_enetMacAddr_t chAddr,
                               L7_uint32 ipAddr,
                               L7_uint32 leaseTime))
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_RC_t rc;

  if ((rc = cnfgrApiComponentNameGet(registrar_ID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
            "Invalid registrar ID %d is specified for registration",
            (L7_int32)registrar_ID);
    return(L7_FAILURE);
  }
  if (sendTypeInterestedCIds[registrar_ID].registrar_ID!= 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
            "Registrar ID %d, %s is already registered", (L7_int32)registrar_ID, name);
    return(L7_FAILURE);
  }

  sendTypeInterestedCIds[registrar_ID].registrar_ID = registrar_ID;
  sendTypeInterestedCIds[registrar_ID].notify_dhcps_send_type=
                                                 notify_func;

  return(L7_SUCCESS);

}

/***********************************************************************
 * @purpose  Deregister a registered component.
 *
 * @param    cId To be deregistered componet ID
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendTypeNotifyDeRegister(L7_uint32 registrar_ID)
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_RC_t rc;

  if ((rc = cnfgrApiComponentNameGet(registrar_ID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

    if (registrar_ID >= L7_LAST_COMPONENT_ID)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
              "Invalid registrar ID %d is specified for unregistration",
              (L7_int32)registrar_ID);
      return(L7_FAILURE);
   }
   if (sendTypeInterestedCIds[registrar_ID].registrar_ID == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCPS_MAP_COMPONENT_ID,
            "Registrar ID %d, %s is already unregistered", (L7_int32)registrar_ID, name);
    return(L7_FAILURE);
  }
  memset ((void *)&sendTypeInterestedCIds[registrar_ID],0,
                 sizeof(dhcpsSendMessageNotifyList_t ));

  return(L7_SUCCESS);
}

/* </JSGD> */



