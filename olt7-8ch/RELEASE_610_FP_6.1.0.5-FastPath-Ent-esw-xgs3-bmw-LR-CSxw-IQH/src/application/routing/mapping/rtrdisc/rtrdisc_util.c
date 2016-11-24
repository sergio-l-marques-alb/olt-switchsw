/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtrdisc_util.c
*
* @purpose   
*
* @component  
*
* @comments  none
*
* @create    11/20/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include <stdlib.h>

#include "l7_common.h"
#include "l7_common_l3.h"
#include "log.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "sysapi.h"
#include "sysnet_api.h"
#include "osapi_support.h"

#include "ip_util.h"
#include "rtip.h"
#include "rtip_icmp.h"
#include "l3_default_cnfgr.h"

#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "rtrdisc_util.h"
#include "rtrdisc_api.h"
#include "rtrdisc_cnfgr.h"

extern rtrDiscCfgData_t *rtrDiscCfgData;
extern rtrDiscCnfgrState_t rtrDiscCnfgrState; 
extern rtrDiscIntf_t  *rtrDiscIntf;
extern rtrDiscIntfInfo_t      *pRtrDiscIntfInfo;   
extern osapiRWLock_t    rtrDiscIntfRWLock;
extern L7_uint32 *rtrDiscMapTbl;
extern osapiTimerDescr_t *pRtrDiscTimerHolder;

/*********************************************************************
* @purpose  Processes the timer tick messages
*
* @param    parm1       not used
* @param    parm2       not used
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscTimerTick(void * parm1, L7_uint32 parm2)
{
    L7_RC_t rc;
    rtrDiscMsg_t msg;

    if (pRtrDiscInfo->rtrDiscProcessingQueue == L7_NULLPTR)
    {
        return;
    }

    memset(&msg, 0, sizeof(rtrDiscMsg_t) );
    msg.msgId = RTR_DISC_TICK;

    rc = osapiMessageSend( pRtrDiscInfo->rtrDiscProcessingQueue, &msg, sizeof(rtrDiscMsg_t), 
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

    if (rc != L7_SUCCESS)
    {
        LOG_MSG("rtrDiscTimerTick: osapiMessageSend() failed\n");
    }
}

/*********************************************************************
* @purpose  Processes the timer tick messages
*
* @param    
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscTimerTickProcess()
{
  struct timeval tv;
  rtrDiscIntf_t *present;
  L7_RC_t rtrDiscRC;
  L7_uint32    ticksLeft;
  static const char *routine_name = "rtrDiscTimerTick()";
  rtrDiscIntfCfgData_t *pCfg;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  /* schedule next tick */
  osapiTimerAdd ( (void*)rtrDiscTimerTick, L7_NULL, L7_NULL, 
                  RTR_DISC_TASK_TIMER, &pRtrDiscTimerHolder);

  osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);

  rtrDiscRC = ipMapRtrAdminModeGet();
  if ( rtrDiscRC != L7_ENABLE )
  {

    osapiWriteLockGive(rtrDiscIntfRWLock);
    return;
  }

  present = rtrDiscIntf;
  while (present != L7_NULLPTR)
  {
    /*Get the unit, slot, port of the interface number */
    if ((present->state == L7_TRUE) &&
        ((rtrDiscMapIntfIsConfigurable(present->intIfNum, &pCfg)) == L7_TRUE))
    {
      /*Check if the interface is enabled for router discovery*/
      if (pCfg->advertise == L7_TRUE)
      {
        /*Decrement the ticks and check if it is zero*/
        /*If it is, then send router discovery*/
        present->ticksLeft --;
        if (present->ticksLeft == 0)
        {
          /* Get the list of IP addresses configured on an interface */
          if (L7_SUCCESS == ipMapRtrIntfIpAddrListGet(present->intIfNum, 
               ipAddrList))
          {
            /*Send a router discovery message on this interface */
            if ((rtrDiscAdvPktSend(present->intIfNum, ipAddrList, 
                                   pCfg->advAddress,
                                   pCfg->preferenceLevel, 
                                   pCfg->advLifetime)) != L7_SUCCESS)
            {
              /* Failed to send router discovery advertisement 
                 on the interface
              */

              RTR_DISC_ERROR("%s %d: %s: ERROR: Failed to send rtrdisc adv pkt on interface %d\n",
                          __FILE__, __LINE__, routine_name, present->intIfNum);                            
            }
          }
          else
          {
               RTR_DISC_ERROR("%s %d: %s: ERROR: Failed to get list of configured IP addresses\n",
                           __FILE__, __LINE__, routine_name);
          }

          /*Then select a random interval time for next message */
          rtrDiscGetTimeOfDay(&tv, (struct timezone *)0);
          srand(tv.tv_sec);
          ticksLeft = (L7_uint32) rand();
          ticksLeft = (L7_uint32)((ticksLeft / ((L7_double64)RAND_MAX + 1)) * (pCfg->maxAdvInt - pCfg->minAdvInt));
          ticksLeft += pCfg->minAdvInt;
          if (present->numInitialAdvLeft != 0)
          {
            present->numInitialAdvLeft -= 1;
            if (ticksLeft >= MAX_INITIAL_ADVERT_INTERVAL)
            {
              ticksLeft = MAX_INITIAL_ADVERT_INTERVAL;
            }
          }
          present->ticksLeft = ticksLeft;
        }
      }
    }
    present = present->next;
  }


  osapiWriteLockGive(rtrDiscIntfRWLock);
}

/*********************************************************************
* @purpose  Processes the solication pkts from hosts
*
* @param    intIfNum		Interface number on which the packet arrived
* @param    netBufHandle	Pointer to the packet
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscSolicitPktProcess(L7_uint32 intIfNum, L7_netBufHandle netBufHandle)
{
  rtrDiscIntfCfgData_t *pCfg;
  rtrDiscIntf_t *present;
  struct ip *ip;
  struct icmp *icmp;
  struct timeval tv;

  L7_ushort16 ckSum, oldCkSum;
  L7_uchar8 *data;
  L7_uint32 hlen, ticksLeft,pktLen;
  L7_uchar8 type, code;
  L7_IP_ADDR_t advAddr;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32 count;
  L7_BOOL match = L7_FALSE;

  if ((ipMapRtrAdminModeGet()) != L7_ENABLE)
    return;

  osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  do  /* One pass loop */
  {
    present = rtrDiscIntf;
    while (present != L7_NULLPTR)
    {
      if (present->intIfNum == intIfNum)
      {
        break;
      }
      present = present->next;
    }
    if (present == L7_NULLPTR)
      break;


    SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
    ip = (struct ip *)(data + sysNetDataOffsetGet(data));
    hlen = ip->ip_hl << 2;
    pktLen = osapiNtohs(ip->ip_len);
    if (pktLen < hlen + ICMP_MINLEN)
    {
      break;
    }
    if (ip->ip_dst.s_addr != osapiHtonl(0xE0000002) && ip->ip_dst.s_addr != osapiHtonl(0xffffffff))
    {
      break;
    }
    pktLen -= hlen;
    icmp = (struct icmp *)((L7_uchar8 *)ip + hlen);
    type = icmp->icmp_type;
    code = icmp->icmp_code;
    /*Check if it is solicitation message and that the icmp packet is atleast
    8 bytes or longer
    */
    if (type != 10 || code != 0 || pktLen < 8)
    {
      break;
    }
    /*Check if it has arrived from a host on the same subnet as the interface.
    */
    if (ipMapRtrIntfIpAddrListGet(present->intIfNum, ipAddrList) != L7_SUCCESS)
    {
      break;
    }

    if ( 0 != ip->ip_src.s_addr)
    {
      for (count =0; count < L7_L3_NUM_IP_ADDRS ; count++)
      {
        if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
        {
          continue; 
        }

        if ((ipAddrList[count].ipAddr & ipAddrList[count].ipMask) ==
              (osapiNtohl(ip->ip_src.s_addr) & ipAddrList[count].ipMask))
        {
          match = L7_TRUE;
          break;
        }
      }

      if (L7_FALSE == match)
      {
        break;
      }
    }	

    /*Validate the checksum of the icmp pkt*/
    oldCkSum = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    ckSum = rtrDiscChksmCalc((L7_ushort16 *)icmp, pktLen);
    if (ckSum != oldCkSum)
      break;

	if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
	{
	  break;
	}

	advAddr = pCfg->advAddress;

    if (ip->ip_dst.s_addr == osapiHtonl(0xffffffff) && advAddr != 0xffffffff)
    {
      RTR_DISC_ERROR("ERROR: Received broadcast solicitation on multicast config interface %d\n",
                    present->intIfNum);                             

    }
    /*Delay response upto a max of MAX_RESPONSE_DELAY*/
    rtrDiscGetTimeOfDay(&tv, (struct timezone *)0);
    srand(tv.tv_sec);
    ticksLeft = rand();
    ticksLeft = (L7_uint32)((ticksLeft / ((L7_double64)RAND_MAX + 1)) * MAX_RESPONSE_DELAY);
    ticksLeft = ticksLeft == 0 ? 1 : ticksLeft;
    if (present->ticksLeft > ticksLeft)
    {
      present->ticksLeft = ticksLeft;

    }
  } while (1);  /* End of one pass loop */


  osapiWriteLockGive(rtrDiscIntfRWLock);
  return;
}
/*********************************************************************
* @purpose  Send the router discovery advertisement packet
*
* @param    intIfNum       Interface on which the packet has to be sent
* @param    pSrcIPAddrList List of IP Addresses configured on the interface
* @param    dstAddress     Destination address of the interface
*
* @returns  L7_SUCCESS  Packet has been sent successfully
* @returns  L7_FAILURE  Packet has not been sent
*
* @notes    The parameters include source address because, in the future, if
* @notes    more than 1 ip address is configured on a interface, we can know
* @notes    the source address that has to be used.
*
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvPktSend(L7_uint32 intIfNum, 
                          L7_rtrIntfIpAddr_t *pSrcIPAddrList,
                          L7_IP_ADDR_t dstIPAddr, L7_int32 prefLevel, 
                          L7_ushort16 lifeTime)
{
  L7_uchar8 pktBuffer[L7_IP_HDR_LEN + RTR_DISC_PKT_SIZE];
  struct icmp * icmp;
  L7_ipHeader_t * ip;
  L7_ushort16 etype;
  L7_ushort16 flag = 0;
  L7_netBufHandle pBufHandle;
  L7_enetMacAddr_t dstMacAddr;
  L7_uint32 datalen;
  L7_uchar8 *dataStart;
  struct icmp_ra_addr *ra_addr;
  L7_uint32 ipAddrNum = 0;
  L7_uint32 count;

  bzero((char *)pktBuffer, L7_IP_HDR_LEN + RTR_DISC_PKT_SIZE);
  ip = (L7_ipHeader_t *)pktBuffer;
  ip->iph_versLen = (L7_IP_VERSION << 4) | L7_IP_HDR_VER_LEN;
  ip->iph_tos = 0;
  ip->iph_ident = osapiHtons(1); /* arbitrary number */
  ip->iph_flags_frag  = 0;
  ip->iph_ttl = 1;
  ip->iph_prot  = IP_PROT_ICMP;
  ip->iph_src = osapiHtonl(pSrcIPAddrList[0].ipAddr);

  ip->iph_dst = osapiHtonl(dstIPAddr);
  ip->iph_csum = 0;

  icmp = (struct icmp *)(pktBuffer + L7_IP_HDR_LEN);
  icmp->icmp_type = 9;
  icmp->icmp_code = 0;
  icmp->icmp_cksum = 0;   
  icmp->icmp_num_addrs = 0;
  icmp->icmp_wpa = 0;
  icmp->icmp_lifetime = osapiHtons(lifeTime);
  ra_addr = &icmp->icmp_radv;

  for (count = 0; count< L7_L3_NUM_IP_ADDRS; count++)
  {
    if (pSrcIPAddrList[count].ipAddr != L7_NULL_IP_ADDR)
    {
      ra_addr->ira_addr = osapiHtonl(pSrcIPAddrList[count].ipAddr);
      ra_addr->ira_preference = osapiHtonl(prefLevel);
      icmp->icmp_num_addrs = icmp->icmp_num_addrs + 1;
      icmp->icmp_wpa       = icmp->icmp_wpa + 2;  /* Number of words */
      ipAddrNum            = ipAddrNum + 1;
      ra_addr++;
    }
  }

  ip->iph_len = L7_IP_HDR_LEN + RTR_DISCOVERY_PKT_SIZE(ipAddrNum);
  ip->iph_len = osapiHtons(ip->iph_len);

  ip->iph_csum  = rtrDiscChksmCalc((L7_ushort16 *)ip, L7_IP_HDR_LEN);
  icmp->icmp_cksum = rtrDiscChksmCalc((L7_ushort16 *)icmp, RTR_DISCOVERY_PKT_SIZE(ipAddrNum));

  flag |= L7_LL_ENCAP_ENET;
  etype=L7_ETYPE_IP;
  if (dstIPAddr == 0xE0000001)
  {
    dstMacAddr.addr[0] = 0x01;
    dstMacAddr.addr[1] = 0x00;
    dstMacAddr.addr[2] = 0x5E;
    dstMacAddr.addr[3] = 0x00;
    dstMacAddr.addr[4] = 0x00;
    dstMacAddr.addr[5] = 0x01;
  }
  else if (dstIPAddr == 0xFFFFFFFF)
  {
    dstMacAddr.addr[0] = 0xFF;
    dstMacAddr.addr[1] = 0xFF;
    dstMacAddr.addr[2] = 0xFF;
    dstMacAddr.addr[3] = 0xFF;
    dstMacAddr.addr[4] = 0xFF;
    dstMacAddr.addr[5] = 0xFF;
  }
  else
    return L7_FAILURE;

  if (sysNetPduHdrEncapsulate(dstMacAddr.addr, intIfNum, &flag, 
                              L7_IP_HDR_LEN + RTR_DISCOVERY_PKT_SIZE(ipAddrNum),
                              0, etype, &pBufHandle, pktBuffer) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATALENGTH(pBufHandle, datalen);
  if (datalen < L7_EFFECTIVE_MIN_ENET_PKT_SIZE)
  {
	SYSAPI_NET_MBUF_GET_DATASTART(pBufHandle, dataStart);
	memset(dataStart + datalen, 0 , L7_EFFECTIVE_MIN_ENET_PKT_SIZE - datalen);
    datalen = L7_EFFECTIVE_MIN_ENET_PKT_SIZE;
    SYSAPI_NET_MBUF_SET_DATALENGTH(pBufHandle, datalen);
  }

  if (ipmRouterIfBufSend(intIfNum, pBufHandle) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  compute checksum
*
* @param    icmp		message to compute checksum
* @param	len			Length of the message
*
* @returns  calculated checksum (ones complement) of the icmp struct
*
* @comments    none
*
* @end
*************************************************************************/
L7_ushort16 rtrDiscChksmCalc (L7_ushort16 *data, L7_uint32 len)
{
  L7_ushort16 *dataPtr = data;
  L7_long32 sum;
  L7_ushort16 answer;
  L7_int32 index;

  sum = 0 ;

  for ( index = 0 ; index < len ; index+=2 )
  {
    sum = sum + *dataPtr ;
    dataPtr++ ;
  }

  while (sum >> 16)
  {
    sum = (sum >> 16) + (sum & 0xffff) ;
  }

  answer = ~sum ;
  return(answer) ;
}

/**************************************************************************
* @purpose  compute time of day
*
* @param    tv		    timeval structure
* @param	not_used	timezone structure
*
* @returns  
*
* @comments    none
*
* @end
*************************************************************************/
void rtrDiscGetTimeOfDay( struct timeval *tv , void *not_used )
{
  L7_clocktime ct;
  osapiClockTimeRaw ( &ct );
  tv->tv_sec = ct.seconds;
  tv->tv_usec = ct.nanoseconds / 1000;   
}

/**************************************************************************
* @purpose  Apply the max adv interval
*
* @param    intIfNum	Internal interface number
* @param	time		Max adv interval time in sec
*
* @returns  
*
* @comments    
*
* @end
*************************************************************************/
void rtrDiscMaxAdvIntervalApply(L7_uint32 intIfNum, L7_uint32 time)
{
  rtrDiscIntf_t *present;


  osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
  present = rtrDiscIntf;
  while (present != L7_NULLPTR)
  {
    if (present->intIfNum == intIfNum)
    {
      present->ticksLeft = time;

      osapiWriteLockGive(rtrDiscIntfRWLock);
      return;
    }
    present = present->next;
  }

  osapiWriteLockGive(rtrDiscIntfRWLock);
}

/*********************************************************************
* @purpose  Process router discovery frames
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_PROCESSED  if frame has been processed; continue processing it
* @returns  SYSNET_PDU_RC_IGNORED    if frame has been ignored; continue processing it
*
* @notes    none
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t rtrDiscPktIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc)
{
  L7_uchar8 *data;
  struct ip *ip_header;
  struct icmp *icmp_header;
  L7_uint32 hlen;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  ip_header = (struct ip *)(data + sysNetDataOffsetGet(data));
  hlen = ip_header->ip_hl << 2;
  icmp_header = (struct icmp *)((L7_uchar8 *)ip_header + hlen);
  if (ip_header->ip_p == IPPROTO_ICMP && icmp_header->icmp_type == ICMP_ROUTERSOLICIT)
  {
    /* It is router discovery packet. Process it.*/
    rtrDiscSolicitPktProcess(pduInfo->intIfNum, bufHandle);
    /* Even if we peocess it successfully, let somebody else also take it, i.e. don't free it. */
    return SYSNET_PDU_RC_PROCESSED;
  }

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL rtrDiscMapIntfConfigEntryGet(L7_uint32 intIfNum, rtrDiscIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rtrDiscRC;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (RTR_DISC_IS_READY != L7_TRUE)
    return L7_FALSE;

  if ((rtrDiscRC = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&rtrDiscCfgData->rtrDiscIntfCfgData[i].configId, &configIdNull))
      {
        rtrDiscMapTbl[intIfNum] = i;
        pRtrDiscIntfInfo[intIfNum].intIfNum = intIfNum;
        memset(&(pRtrDiscIntfInfo[intIfNum].acquiredList),0,sizeof(AcquiredMask));
        *pCfg = &rtrDiscCfgData->rtrDiscIntfCfgData[i];
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}
