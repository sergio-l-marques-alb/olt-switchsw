/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_parse.c
*
* @purpose VOIP Singalling protocol parsing utility routines
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
#ifndef VOIP_PARSE_H
#define VOIP_PARSE_H

#include "l7_common.h"
#include "sysnet_api.h"
#include "osapi.h"
#include "log.h"
#include "voip_parse_util.h"

typedef struct voipFpEntry_s
{
  L7_uint32           src_ip;
  L7_uint32           dst_ip;
  L7_ushort16         l4_src_port;
  L7_ushort16         l4_dst_port;
}voipFpEntry_t;

typedef struct L7_ipHdr_s 
{
  /* version / header length / type of service */
  L7_ushort16 _v_hl_tos;
  /* total length */
  L7_ushort16 _len;
  /* identification */
  L7_ushort16 _id;
  /* fragment offset field */
  L7_ushort16 _offset;
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
  /* time to live / protocol*/
  L7_ushort16 _ttl_proto;
  /* checksum */
  L7_ushort16 _chksum;
  /* source and destination IP addresses */
  L7_uint32 _src_ip;
  L7_uint32 _dst_ip;
} L7_ipHdr_t;

typedef struct L7_tcpHdr_s 
{
  /* Src and Dst Port */
  L7_ushort16 _src_port;
  L7_ushort16 _dst_port;
  /* Sequence Number */
  L7_uint32 _seq_no;
  /* Ack. Number */
  L7_uint32 _ack_no;
  /* length, Res, ECN & Ctrl */
  L7_ushort16 _len;
#define TCP_LENMASK 0xf000   /* mask for len bits */
} L7_tcpHdr_t;

/*********************************************************************
* @purpose checks whether a signalling protocol is supported or not
*
* @param  L7_uint32 protocol  (input)  protocol
*
* @returns L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL voipIsProtocolSupported(L7_uint32 protocol);
/*********************************************************************
* @purpose  Send a message to the voipTask to process incoming PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the voip PDU received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
* @param    L7_uint32            protocol   SIP|MGCP|H323|SCCP
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipPduReceiveCallback(L7_netBufHandle bufHandle,
                               sysnet_pdu_info_t *pduInfo,L7_uint32 protcol);

/*********************************************************************
* @purpose  Process an incoming packet
*
* @param    L7_uint32       intIfNum   @b((input)) NIM internal
*           interface number
* @param    L7_netBufHandle bufHandle  @b((input)) pointer to the ethernet
*           packet received
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipPduReceiveProcess(L7_uint32 intIfNum,
                              L7_uchar8 *buf, L7_uint32 protocol,L7_uint32 length);

/*********************************************************************
* @purpose Initialize SCCP call manangement station entries
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSccpCmsInit();

/*********************************************************************
* @purpose Initialize MGCP call agent entries
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipMgcpCaInit();

/*********************************************************************
* @purpose  Initialize Sip Call entries
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSipCallInit();

#endif
