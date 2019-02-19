/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_rtmbuf.c
*
* @purpose    temporary placeholder for rtm_dup function.WIll be removed once 
*	      the function is moved to the appropriate location
*
*
* @comments   
*
* @create     06/12/2006
*
* @author     
* @end
*
**********************************************************************/
#include "log.h"
#include "buff_api.h"
#include "sysapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
#include "l3end_api.h"
#include "l7_ip_api.h"
#include "datatypes.h"
#include "rtip.h"
#include "rtiprecv.h"
#include "osapi.h"
#include "dtl_l3_mcast_api.h"
#include "l7_ip6_api.h" 
#include "l7_packet.h"
#include "mcast_debug.h"

L7_uint32 rtmBufPoolAllocFailed =0;
L7_uint32 rtmMbufAllocFailed =0;
/*********************************************************************
*
* @purpose  Duplicate the given message buffer
*
* @param    pBuf  - pointer to the buffer to duplicate
*
* @returns  pDupBuf - pointer to the duplicate buffer, if operation successful
*           L7_NULLPTR - if operation failed
*
* @notes    This function should ideally reside elsewhere ... probably in rtmbuf.c
*
* @end
*********************************************************************/
struct rtmbuf *rtm_dup(struct rtmbuf *pBuf)
{
  struct rtmbuf *pDupBuf = L7_NULLPTR;
  L7_uint32     frameHdl;
  L7_uchar8     *bufferAddr = L7_NULLPTR;
  L7_uint32     bufferPoolId;
  L7_uint32     dataLen;
  L7_uchar8     *pDataStart = L7_NULLPTR;
  L7_uchar8     *pOldDataStart = L7_NULLPTR;


  /* Basic sanity checks */
  if (pBuf == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "%s, %d : Given pBuf is NULL", __FUNCTION__, __LINE__);
    return L7_NULLPTR;
  }

  /* Retrieve the Buffer Pool ID and verify if it is valid */
  bufferPoolId = rtMemPoolIdGet();
  if (bufferPoolId == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "%s, %d : Unable to get bufferPoolId", __FUNCTION__, __LINE__);
    return L7_NULLPTR;
  }

  /* Allocate the buffer */
  if (bufferPoolAllocate(bufferPoolId, &bufferAddr) != L7_SUCCESS)
  {
    rtmBufPoolAllocFailed++;
    /*LOG_MSG("\n %s, %d : Unable to allocate buffer", __FUNCTION__, __LINE__);*/
    return L7_NULLPTR;
  }

  /* Populate the buffer info over to the duplicate buffer */
  pDupBuf = (struct rtmbuf *)bufferAddr;
  memset(pDupBuf, 0, sizeof(struct rtmbuf));
  pDupBuf->rtm_pkthdr.rcvif = pBuf->rtm_pkthdr.rcvif;
  rtcopyMbuf(pBuf, pDupBuf);

  /* Get a free Rx MBuf */
  frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_NORMAL, L7_MBUF_FRAME_ALIGNED);
  if (frameHdl == L7_NULL)
  {
    bufferPoolFree(bufferPoolId, bufferAddr);
    rtmMbufAllocFailed++;
   /* LOG_MSG("\n %s, %d : Failed to get free MBuf ", __FUNCTION__, __LINE__);*/
    return L7_NULLPTR;
  }

  /* Copy the data from the old data frame over to the new one
     and assign it to the duplicate buffer */
  pDupBuf->rtm_bufhandle = (void *)frameHdl;
  SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)(pDupBuf->rtm_bufhandle), pDataStart);
  SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)(pBuf->rtm_bufhandle), pOldDataStart);
  SYSAPI_NET_MBUF_GET_DATALENGTH((L7_netBufHandle)(pBuf->rtm_bufhandle), dataLen);
  memcpy(pDataStart, pOldDataStart, dataLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH((L7_netBufHandle)(pDupBuf->rtm_bufhandle), dataLen);

  /* If the packet is vlan tagged then it needs to be stripped for application to process.
     If the outgoing ports are tagged then the lower layer should tag the packets again */
  if (sysNetDataStripVlanTag((L7_uint32)(pDupBuf->rtm_bufhandle)) == L7_SUCCESS)
    SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)(pDupBuf->rtm_bufhandle), pDataStart);

  /* Setup the data length and start and return to caller */
  pDupBuf->rtm_data = (caddr_t)(pDataStart + sysNetDataOffsetGet(pDataStart));
  pDupBuf->rtm_len  = dataLen - sysNetDataOffsetGet(pDataStart);
  pDupBuf->rtm_pkthdr.len = pDupBuf->rtm_len;

  return pDupBuf;
}

void rtmDebugShow()
{
    sysapiPrintf("Number of BufferPool Allocate Failed: ", rtmBufPoolAllocFailed);
    sysapiPrintf("Number of NetMbuf Get Failed: ", rtmMbufAllocFailed);    
}
