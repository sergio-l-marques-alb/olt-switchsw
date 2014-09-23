/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_mgcp.h
*
* @purpose MGCP message parsing routines
*
* @component VOIP |MGCP (MEDIA GATEWAY CONTROL PROTOCOL)
*
* @comments RFC 2705
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_MGCP_H
#define VOIP_MGCP_H
#include "voip_parse.h"


typedef struct voipMgcpMessage_s
{
  L7_uchar8 *mgcpCommand;               /*< Command (MGCP command only) */
  L7_uint32 returnCode;                 /*< Return Code (MGCP Response only) */
  L7_uchar8 *tranId;                    /*< Transcatio ID */
  L7_uchar8 *callId;                    /*< Call-ID header */
  L7_uchar8 *connectionId;              /*< Connection-ID header */
  L7_uchar8 *notifiedId;                /* notified entity ID with port number */
}voipMgcpMessage_t;

#ifndef MGCP_MESSAGE_MAX_LENGTH
#define MGCP_MESSAGE_MAX_LENGTH 4000
#endif

#define MGCPMSG_IS_RESPONSE(msg) ((msg)->returnCode!=0)
#define MGCPMSG_IS_REQUEST(msg)  ((msg)->returnCode==0)
#define MGCPMSG_IS_CRCX(msg)   (0==strcmp((msg)->mgcpCommand,"CRCX"))
#define MGCPMSG_IS_MDCX(msg)  (0==strcmp((msg)->mgcpCommand,"MDCX"))
#define MGCPMSG_IS_DLCX(msg)  (0==strcmp((msg)->mgcpCommand,"DLCX"))
#define MGCPMSG_IS_STATUS_200(msg) ((msg)->returnCode == 200)

typedef struct voipMgcpCall_s 
{
  voipMgcpMessage_t      *msg;
  L7_uchar8               ip[4]; /* Gateway IP */
  voipFpEntry_t           db;
  struct voipMgcpCall_s   *next;
} voipMgcpCall_t;

typedef struct voipMgcpCa_s
{
  L7_uchar8            ip[4];  /* Call Agent IP */
  voipMgcpCall_t       *call;
  L7_uint32            count;
  struct voipMgcpCa_s  *next;
} voipMgcpCa_t;



/*********************************************************************
* @purpose Allocate memory and initialize MGCP Message
*
* @param   voipMgcpMessage_t **mgcp (input) Pointer to MGCP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipMgcpMessageInit (voipMgcpMessage_t **mgcp);

/*********************************************************************
* @purpose Free the memory allcoated to MGCP Message
*
* @param   voipMgcpMessage_t *mgcp (input) Pointer to MGCP message
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipMgcpMessageFree (voipMgcpMessage_t * mgcp);

/*********************************************************************
* @purpose Parse MGCP message
*
* @param   const L7_uchar8 *ipstr    Pointer to IpAddress (Source Ip)
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipMgcpMessageParse (const L7_uchar8 *ip, const L7_uchar8 *buf, 
                              L7_uint32 length);
#endif
