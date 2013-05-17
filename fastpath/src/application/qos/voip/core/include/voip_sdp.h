/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sdp.h
*
* @purpose SDP message parsing routines
*
* @component VOIP |SDP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_SDP_H
#define VOIP_SDP_H
#include "voip_parse.h"

typedef struct voipSdpConnection_s
{
  L7_uchar8 *nettype;             /* Network Type */
  L7_uchar8 *addrtype;            /* Network Address Type */
  L7_uchar8 *addr;                /* Address */
  L7_uchar8 *addrMulticastTtl;    /* TTL value for multicast address */
  L7_uchar8 *addrMulticastNum;    /* Number of multicast address */
}voipSdpConnection_t;

typedef struct voipSdpMedia_s
{
  L7_uchar8 *media;              /* media type */
  L7_uchar8 *port;               /* port number */
  L7_uchar8 *numberOfPort;       /* number of port */
  L7_uchar8 *protocol;           /* protocol to be used */
  voipList_t *connections;       /* list of sdp_connection_t */
}voipSdpMedia_t;

typedef struct voipSdpMessage_s
{
  voipSdpConnection_t *c_connection;   /* Connection information */
  voipList_t *m_medias;                /* list of supported media (voipSdpMedia_t) */
}voipSdpMessage_t;

/*********************************************************************
* @purpose Allocate memory and initialize SDP connection
*
* @param   voipSdpConnection_t **conn (input) Pointer to SDP connection
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSdpConnectionInit (voipSdpConnection_t **conn);

/*********************************************************************
* @purpose Free the memory allocated by SDP connection
*
* @param   voipSdpConnection_t *conn (input) Pointer to SDP connection
*
* @returs  none
*
* @notes   none
*
* @end
*********************************************************************/
void voipSdpConnectionFree (voipSdpConnection_t *conn);

/*********************************************************************
* @purpose Allocate memory and initialize SDP Media
*
* @param   voipSdpMedia_t **media (input) Pointer to SDP media
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSdpMediaInit (voipSdpMedia_t **media);

/*********************************************************************
* @purpose Dealloacte SDP Media memory
*
* @param   voipSdpMedia_t *media (input) Pointer to SDP media
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSdpMediaFree (voipSdpMedia_t *media);

/*********************************************************************
* @purpose Allocate memory and initialize SDP Message
*
* @param   voipSdpMessage_t ** sdp (input) Pointer to SDP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSdpMessageInit (voipSdpMessage_t ** sdp);

/*********************************************************************
* @purpose Parse SDP message
*
* @param   voipSdpMessage_t * sdp (input) Pointer to SDP message
* @param   const L7_uchar8 *buf    Pointer to Packet data
*
* @returns L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSdpMessageParse (voipSdpMessage_t * sdp, const L7_uchar8 *buf);

/*********************************************************************
* @purpose Free the memory allcoated to SDP Message
*
* @param   voipSdpMessage_t * sdp (input) Pointer to SDP message
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSdpMessageFree (voipSdpMessage_t * sdp);

#endif
