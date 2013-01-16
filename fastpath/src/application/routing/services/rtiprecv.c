/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rtproto.c
*
* @purpose The purpose of this file is to forward IP traffic.
*          This code rplaces VxWorks IP forwarding function.
*
* @component VxWorks IPM
*
* @comments none
*
* @create 03/19/2001
*
* @author alt
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "log.h"
#include "l7_common.h"
#include "l3end_api.h"
#include "dtlapi.h"
#include "l7_ipmap_arp_api.h"
#include "l7_ip_api.h"
#include "rto_api.h"
#include "sysapi.h"
#include "rtmbuf.h"
#include "l7_packet.h"
#include "rtip_var.h"
#include "rtiprecv.h"
#include "buff_api.h"

#define RTPROTO_DEBUG   0     /* change this definition to 1 for debugging */

extern struct rtm_ifnet *gRtmIfnet;
extern L7_uint32 ipForwardBufAllocFail;
extern L7_uint32 ipForwardNotRtrIntf;
        
/* Future Function: Get rid of rtetherbroadcastaddr. Use nimGetBroadcastMacAddr() */
        
char rtetherbroadcastaddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/*********************************************************************
*
* @purpose  Transmit  routed IP frames. Any transformations on the frame are 
*           done here.
*
* @param    bufHandle  Buffer Handle
* @param    *pduInfo   pointer to PDU info structure which has intIfNum and vlanId
*
* @notes    none 
*
* @end
*********************************************************************/
void rtLvl7IPRecv(L7_netBufHandle bufHandle,
                  sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *data;
  struct rtmbuf *m;
  L7_uint32 datalen;
  L7_enetHeader_t *et;
  short flags = 0;
  L7_RC_t rc;
  L7_uchar8 *buffer_addr;
  L7_uint32 buffer_pool_id;
  L7_uint32 rtmDataOffset;
  L7_uint32 rtrIfNum;

  /* Get the router interface number */
  if (ipMapIntIfNumToRtrIntf(pduInfo->intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    ipForwardNotRtrIntf++;

    SYSAPI_NET_MBUF_FREE(bufHandle); /* Free resources and return */
    return;
  }

  buffer_pool_id = rtMemPoolIdGet ();

  m = 0;
  rc = bufferPoolAllocate (buffer_pool_id, &buffer_addr);
  if (rc != L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle); /* Free resources and return */
    SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM, 
                              L7_PLATFORM_CTR_RX_IP_IN_DISCARDS);
    ipForwardBufAllocFail++;
    return;
  }
  m= (struct rtmbuf *) buffer_addr;

  memset(m, 0, sizeof( *m));

  m->rtm_type = MT_DATA;
  m->rtm_flags |=  M_PKTHDR | M_EXT;
  
  /* Get ifnet structure */
  m->rtm_pkthdr.rcvif = &gRtmIfnet[rtrIfNum];

  /* Get the received Physical port number */
  m->rtm_pkthdr.rxPort = pduInfo->rxPort;

  m->rtm_bufhandle = (void *)bufHandle;
  SYSAPI_NET_MBUF_GET_DATASTART(m->rtm_bufhandle, data);

  rtmDataOffset = sysNetDataOffsetGet(data);
  m->rtm_data = (caddr_t)(data + rtmDataOffset);
  SYSAPI_NET_MBUF_GET_DATALENGTH(m->rtm_bufhandle, datalen);

  m->rtm_len = (L7_int32) (datalen -  rtmDataOffset);
  m->rtm_pkthdr.len = m->rtm_len;

  et = (L7_enetHeader_t *) data;
  if (bcmp((caddr_t) rtetherbroadcastaddr, (caddr_t) et->dest.addr, sizeof(rtetherbroadcastaddr)) == 0)
    flags |= M_BCAST;
  if (et->dest.addr[0] & 1)
    flags |= M_MCAST;

  m->rtm_flags |= flags;

  ip_input(m);
}
