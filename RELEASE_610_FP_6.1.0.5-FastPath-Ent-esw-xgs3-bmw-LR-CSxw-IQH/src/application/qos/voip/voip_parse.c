/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_parse.c
*
* @purpose VOIP Singalling protocol parsing utilities
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "voip.h"
#include "voip_exports.h"
#include "voip_parse.h"
#include "voip_sip.h"
#include "voip_mgcp.h"
#include "voip_sccp.h"
#include "voip_h323.h"

extern void                   *voipQueue;
/*********************************************************************
* @purpose  Process an incoming packet
*
* @param    L7_uint32       intIfNum   @b((input)) NIM internal 
*           interface number
* @param    L7_netBufHandle bufHandle  @b((input)) pointer to the ethernet
*           packet received
* @param    L7_uint32       protocol   @b((input)) SIP|MGCP|H323|SCCP
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    NONE
*
* @end
*********************************************************************/
L7_RC_t voipPduReceiveProcess(L7_uint32 intIfNum,
                              L7_uchar8 *voipBuf,L7_uint32 protocol,L7_uint32 pkt_size)
{
  L7_uint32 offset=0;

  offset = sysNetDataOffsetGet(voipBuf);
  switch(protocol)
  {
    case L7_QOS_VOIP_PROTOCOL_SIP:
      /* Destn IP , SIP Header ,interface */
      voipSipMessageParse((L7_uchar8 *)(voipBuf + offset + 16),(L7_uchar8 *)voipBuf,pkt_size-offset-28);
      break;
    case L7_QOS_VOIP_PROTOCOL_MGCP:
      /*voipMgcpMessageParse((L7_uchar8 *)(voipBuf+offset+12),(L7_uchar8 *)voipBuf,pkt_size-offset-28,intIfNum);*/
       break;
    case L7_QOS_VOIP_PROTOCOL_SCCP:
       voipSccpMessageParse(voipBuf,pkt_size);  
       break;
    case L7_QOS_VOIP_PROTOCOL_H323:
       voipQ931MessageParse((L7_uchar8 *)(voipBuf),pkt_size-offset);
       break;   
     default:
       voipH245MessageParse((L7_uchar8 *)(voipBuf),pkt_size-offset);
       break;
  }
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,(L7_uchar8 *)voipBuf);
   return L7_SUCCESS;      
}

