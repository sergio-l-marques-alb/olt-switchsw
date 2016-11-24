/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_txrx.c
*
* @purpose All functions related to transmission and reception functions.
*
* @component tacacs+
*
* @comments none
*
* @create 03/17/2005
*
* @author gkiran 
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "osapi_support.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_authen.h"
#include "tacacs_control.h"
#include "tacacs_txrx.h"

extern void            *tacacsQueue;
extern void            *tacacsSemaphore;
extern tacacsOprData_t *tacacsOprData;

/*********************************************************************
*
* @purpose Write TACACS packet to a socket
*
* @param   L7_uchar8 *packet   @b{(input)}  pointer to packet buffer
* @param   L7_uint32  length   @b{(input)}  packet length
* @param   L7_uint32  socket   @b{(input)}  socket connected to server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t tacacsPacketWrite(L7_uint32         socket,
                                 tacacsTxPacket_t *packet)
{
  L7_int32 bytes = -1;

  bytes = osapiWrite(socket, packet->packet, packet->length - packet->bytesWritten);
  if (bytes < 0)
  {
    LOG_MSG("TACACS+: Error writing to open socket\n");
    return L7_FAILURE;
  }
  packet->bytesWritten += bytes;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function reads packet bytes from a connection into a 
*          temporary packet buffer. After a full packet is received,
*          i.e. bytesRead == length, the poll task will send packet
*          to the TACACS queue for processing. 
*
*
* @param   L7_uint32         socket  @b{(input)} socket ready to read
* @param   tacacsRxPacket_t *packet  @b{(input/output)} packet buffer
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
static L7_RC_t tacacsPacketRead(L7_uint32         socket,
                                tacacsRxPacket_t *packet)
{
  L7_int32 bytes = -1;

  /* if length == 0, we haven't read entire header yet */
  if (packet->length == 0)                                              
  {
    bytes = osapiRead(socket,
                      &packet->packet[packet->bytesRead],
                      L7_TACACS_PKT_HEADER_LEN - packet->bytesRead);
    if (bytes <= 0)
    {
      return L7_FAILURE;
    }
    packet->bytesRead += bytes;

    if (packet->bytesRead == L7_TACACS_PKT_HEADER_LEN)
    {
      /* have entire header, determine packet length */
      packet->length = osapiNtohl(*((L7_uint32 *)&packet->packet[L7_TACACS_PKT_LENGTH_OFFSET]));
      if ((packet->length == 0) ||
          (packet->length > (TACACS_RX_PKT_BUF_SIZE - L7_TACACS_PKT_HEADER_LEN)))
      {
        /* we are always expecting more than a header, and
           don't accept more than we can store */
        return L7_FAILURE;
      }
      /* initialize bytesRead for packet body */
      packet->bytesRead = 0;
    } else
    {
      /* need to read rest of header when ready */
      return L7_SUCCESS;
    }
  } /* end of if - bytes read */

  bytes = osapiRead(socket,
                    &packet->packet[L7_TACACS_PKT_HEADER_LEN + packet->bytesRead],
                    packet->length - packet->bytesRead);
  if (bytes <= 0)
  {
    return L7_FAILURE;
  }
  packet->bytesRead += bytes;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get a receive packet buffer, find either packet in process 
*          for this socket or return a new rxPacket buffer.
*
* @param L7_uint32          socket @b{(input)}  socket ready to read
* @param tacacsRxPacket_t **packet @b((output)) packet buffer
*
* @returns L7_SUCCESS, packet returned
*          L7_FAILURE, no packet available
*                      (should never happen we have a packet per session)
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t tacacsRxPacketGet(L7_uint32          socket,
                                 tacacsRxPacket_t **packet)
{
  tacacsRxPacket_t *p = L7_NULLPTR;
  L7_uint32         i = 0;

  for (i = 0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    if (tacacsOprData->rxPackets[i].socket == 0)
    {
      p = &tacacsOprData->rxPackets[i];
      continue;
    }
    if ((tacacsOprData->rxPackets[i].socket == socket) &&
        (tacacsOprData->rxPackets[i].bytesRead != tacacsOprData->rxPackets[i].length))
    {
      /* packet read in progress */
      p = &tacacsOprData->rxPackets[i];
      break;
    }
  }

  if (p != L7_NULLPTR)
  {
    p->socket = socket;  /* for new packet, make sure we save socket */
    *packet = p;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose helper function to pass socket error to main task.
*
* @param   tacacsAuthSession_t *session @((input)) current session
*
* @returns void
*
* @comments 
*
* @end
*
*********************************************************************/
static void tacacsEventSocketErrorSend(tacacsAuthSession_t *session)
{
  tacacsMsg_t msg;

  memset(&msg, 0, sizeof(tacacsMsg_t));
  /* some type of error on socket, send error message for session */
  LOG_MSG("TACACS+: received socket error.");
  msg.event = TACACS_EVENT_SOCKET_ERROR;
  msg.data.session = session;
  if (osapiMessageSend(tacacsQueue, 
                       &msg, 
                       sizeof(tacacsMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("TACACS+: failed to send socket error message.");
    return;
  }
  return;
}

/*********************************************************************
*
* @purpose helper function to pass packet received to main task.
*
* @param   tacacsAuthSession_t *session @((input)) session state
*
* @returns void
*
* @comments 
*
* @end
*
*********************************************************************/
static void tacacsEventResponseReceivedSend(tacacsRxPacket_t *packet)
{
  tacacsMsg_t msg;

  memset(&msg, 0, sizeof(tacacsMsg_t));
  msg.event = TACACS_EVENT_RESPONSE_RECEIVED;
  msg.data.packet = packet;
  if (osapiMessageSend(tacacsQueue, 
                       &msg, 
                       sizeof(tacacsMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("TACACS+: failed to send response received message.\n");
    memset(packet, 0, sizeof(tacacsRxPacket_t)); /* drop the packet */
    return;
  }
  return;
}

/*********************************************************************
*
* @purpose This function polls all open sockets for sessions waiting
*          to write a request or read a response.
*
* @param   none
*
* @returns void
*
* @comments To implement single-connection we will need to support read/write of
*           packets for multiple sessions on the same socket, use select logic for
*           read/write of packets, ensure one session tx packet is completely 
*           written, rx packet is completely read before another read/write.
*           NOTE: single-connection was not implemented since there are no servers
*           that support it correctly at this time, but leave it coded this way, 
*           then it will be easy to add single-connection logic in the future.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsAuthSessionsPoll(void)
{
  tacacsAuthSession_t       *session = L7_NULLPTR;
  fd_set                     readfds;  /* fds ready to read  */
  fd_set                     writefds; /* fds ready to write */
  fd_set                     errfds;   /* catch socket errors on a connection */
  L7_uint32                  maxfd = 0;
  L7_uint32                  i = 0;
  tacacsRxPacket_t          *packet = L7_NULLPTR;

  FD_ZERO(&readfds); 
  FD_ZERO(&writefds); 
  FD_ZERO(&errfds); 
  if (osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
    {
      session = &tacacsOprData->sessions[i];
      if (session->socket != 0)
      {
        if (session->state == TACACS_AUTH_STATE_RESPONSE_WAIT)
        {
          FD_SET(session->socket, &readfds);
          FD_SET(session->socket, &errfds);
          if (session->socket > maxfd)
          {
            maxfd = session->socket;
          }
        }
        if (session->state == TACACS_AUTH_STATE_READY_TO_SEND)
        {
          FD_SET(session->socket, &writefds);
          FD_SET(session->socket, &errfds);
          if (session->socket > maxfd)
          {
            maxfd = session->socket;
          }
        }
      }
    }
    (void)osapiSemaGive(tacacsSemaphore); /* Releasing Semaphore */
  }

  if (maxfd == 0)
  {
    /* no connections open, wait before trying again */
    return L7_FAILURE;
  }

  /* if there is a single-connection there could be more than one session
     using the same socket, clear the fd in each case so we always do a 
     select again before writing or reading a packet on the same socket */

  if (osapiSelect(maxfd + 1, 
                  &readfds, 
                  &writefds, 
                  &errfds, 
                  L7_TACACS_SESSION_POLL_TIMEOUT, 
                  0) > 0)
  {
    if (osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      for (i=0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
      {
        session = &tacacsOprData->sessions[i];
        if (session->socket != 0)
        {
          if (FD_ISSET(session->socket, &errfds))
          {
            FD_CLR(session->socket, &errfds);
            session->state = TACACS_AUTH_STATE_SOCKET_ERROR;
            tacacsEventSocketErrorSend(session);
            continue;
          }
          if ((session->state == TACACS_AUTH_STATE_READY_TO_SEND) &&
              (FD_ISSET(session->socket, &writefds)))
          {
            FD_CLR(session->socket, &writefds);
            if (tacacsPacketWrite(session->socket, &session->txPacket) != L7_SUCCESS)
            {
              LOG_MSG("TACACS+: failed to send packet to server %s\n", 
                      osapiInet_ntoa(session->server));
              session->state = TACACS_AUTH_STATE_SOCKET_ERROR;
              tacacsEventSocketErrorSend(session);
              continue;
            }
            /* check if packet is fully written */
            if (session->txPacket.bytesWritten == session->txPacket.length)
            {
              /* now wait for response */
              session->state = TACACS_AUTH_STATE_RESPONSE_WAIT;
            }
            continue;
          }
          if ((session->state == TACACS_AUTH_STATE_RESPONSE_WAIT) &&
              (FD_ISSET(session->socket, &readfds)))
          {
            FD_CLR(session->socket, &readfds);
            /* get new or partially read packet for this socket */
            if (tacacsRxPacketGet(session->socket, &packet) != L7_SUCCESS)
            {
              /* this shouldn't ever happen don't try to do anymore */
              LOG_MSG("TACACS+: failed to get rx packet in receive\n");
              break;
            }
            if (tacacsPacketRead(session->socket, packet) != L7_SUCCESS)
            {
              LOG_MSG("TACACS+: failed to read packet from server %s\n", 
                      osapiInet_ntoa(session->server));
              memset(packet, 0, sizeof(tacacsRxPacket_t)); /* drop the packet */
              session->state = TACACS_AUTH_STATE_SOCKET_ERROR;
              tacacsEventSocketErrorSend(session);
              continue;
            }
            /* if we are done reading packet, match to session
                and update session state appropriately */
            if (packet->bytesRead == packet->length)
            {
              L7_uint32 id = tacacsAuthResponseSessionIdGet(packet);
              if (id != session->sessionId)
              {
                /* to add single-connection, would need to search for socket/sessionId */
                LOG_MSG("TACACS+: invalid session id received from server %s\n",
                        osapiInet_ntoa(session->server));
                memset(packet, 0, sizeof(tacacsRxPacket_t)); /* drop the packet */
                continue;
              }
              session->state = TACACS_AUTH_STATE_RESPONSE_RECEIVED;
              tacacsEventResponseReceivedSend(packet);
              continue;
            }
          }
        }
      }
      osapiSemaGive(tacacsSemaphore);
    }
  }

  return L7_SUCCESS;
}



