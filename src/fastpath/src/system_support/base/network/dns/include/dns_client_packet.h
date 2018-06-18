/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_packet.h
*
* @purpose DNS query packet definitions
*
* @component DNS client
*
* @comments none
*
* @create 03/09/2005
*
* @author dfowler
* @end
*
**********************************************************************/
#ifndef DNS_CLIENT_PACKET_H
#define DNS_CLIENT_PACKET_H

#include "dns_client.h"


/*  
 * DNS Message Format
 *
 *  +---------------------+
 *  |        Header       |
 *  +---------------------+
 *  |       Question      | the question for the name server
 *  +---------------------+
 *  |        Answer       | RRs answering the question
 *  +---------------------+
 *  |      Authority      | RRs pointing toward an authority
 *  +---------------------+
 *  |      Additional     | RRs holding additional information
 *  +---------------------+
 */    

/*
 *    DNS HEADER SECTION 
 *                                   1  1  1  1  1  1
 *     0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |                      ID                       |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |                    QDCOUNT                    |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |                    ANCOUNT                    |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |                    NSCOUNT                    |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *   |                    ARCOUNT                    |
 *   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 */    

#define DNS_MSG_HDR_QR_BIT_MASK       0x80
#define DNS_MSG_HDR_OPCODE_BIT_MASK   0x78  /* we are only interested in zero */
#define DNS_MSG_HDR_AA_BIT_MASK       0x04
#define DNS_MSG_HDR_TC_BIT_MASK       0x02
#define DNS_MSG_HDR_RD_BIT_MASK       0x01
#define DNS_MSG_HDR_RA_BIT_MASK       0x80  /* this bit is in rcode field */
#define DNS_MSG_HDR_RCODE_BIT_MASK    0x0F

typedef struct
{
  L7_ushort16 id;
  L7_uchar8   bits;
  L7_uchar8   rcode;  /* Z reserved for future use, always zero */
  L7_ushort16 qdcount;
  L7_ushort16 ancount;
  L7_ushort16 nscount;
  L7_ushort16 arcount;
} dnsMessageHeader_t;

/* other message sections are variable sizes */

#define DNS_UDP_MSG_SIZE_MAX            512
#define DNS_MSG_RR_NAME_PTR_MASK        0xC0    /* 11XX XXXX - Xs indicate packet offset */
#define DNS_MSG_RR_NAME_OFFSET_MASK     0x3FFF  /* offset in packet */


#endif /* DNS_CLIENT_PACKET_H */
