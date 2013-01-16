/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sdp.c
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
#include "voip_exports.h"
#include "voip_sdp.h"
#include "voip_debug.h"

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
L7_RC_t voipSdpConnectionInit (voipSdpConnection_t **conn)
{
  *conn = (voipSdpConnection_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID ,(sizeof (voipSdpConnection_t)));

  if (*conn == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipSdpConnectionInit Could not allocate memeory\n");
    return L7_FAILURE;
  }
  (*conn)->nettype = L7_NULLPTR;
  (*conn)->addrtype = L7_NULLPTR;
  (*conn)->addr = L7_NULLPTR;
  (*conn)->addrMulticastTtl = L7_NULLPTR;
  (*conn)->addrMulticastNum = L7_NULLPTR;
  return L7_SUCCESS;
}
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
void voipSdpConnectionFree (voipSdpConnection_t *conn)
{
  if (conn == L7_NULLPTR)
    return;
  if (conn->nettype != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn->nettype);
  if (conn->addrtype != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn->addrtype);
  if (conn->addr != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn->addr);
  if (conn->addrMulticastTtl != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn->addrMulticastTtl);
  if (conn->addrMulticastNum != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn->addrMulticastNum);
  osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,conn);

}
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
L7_RC_t voipSdpMediaInit (voipSdpMedia_t **media)
{
  *media = (voipSdpMedia_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID , (sizeof (voipSdpMedia_t)));

  if (*media == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipSdpMediaInit Could not allocate memeory\n");
    return L7_FAILURE;
  }
  (*media)->media = L7_NULLPTR;
  (*media)->port = L7_NULLPTR;
  (*media)->numberOfPort = L7_NULLPTR;
  (*media)->connections = (voipList_t *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID ,(sizeof (voipList_t)));

  if ((*media)->connections == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipSdpMediaInit(connections) Could not allocate memeory\n");
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,*media);
    return L7_FAILURE;
  }
  voipListInit ((*media)->connections);
  return L7_SUCCESS;
}
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
void voipSdpMediaFree (voipSdpMedia_t *media)
{
  if (media == L7_NULLPTR)
    return;
  if (media->media != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media->media);
  if (media->port != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media->port);
  if (media->numberOfPort != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media->numberOfPort);
  if (media->protocol != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media->protocol);
  if (media->connections != L7_NULLPTR)
  {
    L7_uint32 i, size;
    size = voipListSize(media->connections);
    for(i = 0; i< size; i++)
    {
      voipSdpConnection_t *c;
      c = (voipSdpConnection_t *)voipListGet(media->connections, i);
      voipSdpConnectionFree (c);
    }
    for(i = 0; i< size; i++)
    {
      voipListRemoveNode(media->connections,0);
    }
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media->connections);
  }
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,media);

}
/*********************************************************************
* @purpose Allocate memory and initialize SDP Message
*
* @param   voipSdpMessage_t **sdp (input) Pointer to SDP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSdpMessageInit (voipSdpMessage_t **sdp)
{
  (*sdp) = (voipSdpMessage_t *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID, sizeof (voipSdpMessage_t));

  if (*sdp == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipSdpMessageInit : Could not allocate memory\n");
    return L7_FAILURE;
  }

  (*sdp)->c_connection = L7_NULLPTR;
  (*sdp)->m_medias = (voipList_t *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,sizeof (voipList_t));
  if ((*sdp)->m_medias == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipSdpMessageInit (m_medias) Could not allocate memory\n");
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,*sdp);
    return L7_FAILURE;
  }
  voipListInit ((*sdp)->m_medias);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse SDP Connection attribute
*
* @param   voipSdpMessage_t *sdp  Pointer to SDP message
* @param   const L7_uchar8 *buf   Pointer to packet data
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    c=nettype addrtype (multicastaddr | addr)
*
* @end
*********************************************************************/
static L7_RC_t
voipSdpMessageParseC (voipSdpMessage_t *sdp, L7_uchar8 *buf)
{
  L7_uchar8 *equal;
  L7_uchar8 *tmp;
  L7_uchar8 *tmp_next;
  voipSdpConnection_t *cHeader;
  L7_RC_t rc;

  /* c=nettype addrtype (multicastaddr | addr) */
  equal = buf;
  while ((*equal != '=') && (*equal != '\0'))
    equal++;
  /* if attribute value was missing */
  if (*equal == '\0')
    return L7_FAILURE;

  tmp = equal + 1;
  rc = voipSdpConnectionInit (&cHeader);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }
  /* nettype is "IN" and will be extended */
  rc = voipSetNextToken (&(cHeader->nettype), tmp, ' ', &tmp_next);
  if (rc != L7_SUCCESS)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP,"Search for network type failed");
    return L7_FAILURE;
  }
  tmp = tmp_next;

  /* nettype is "IP4" or "IP6" and will be extended */
  rc = voipSetNextToken (&(cHeader->addrtype), tmp, ' ', &tmp_next);
  if (rc != L7_SUCCESS)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Search for Addr type failed");
    return L7_FAILURE;
  }
  tmp = tmp_next;

  /* there we have a multicast or unicast address
   * multicast can be ip/ttl [/integer]
   * unicast is FQDN or ip (no ttl, no integer)
   * is MULTICAST?
   */
  {
    L7_uchar8 *crlf, *slash=L7_NULLPTR;
    L7_uint32 mcastflag = 0;

    crlf = tmp + 1;
    while ((*crlf != '\r') && (*crlf != '\n') && (*crlf != '\0'))
    {
      if (*crlf == '/')
      {
        mcastflag = 1;
        break;
      }
      crlf++;
    }
    /* it's a multicast address! */
    if (mcastflag == 1)
    {
      rc = voipSetNextToken (&(cHeader->addr), tmp, '/', &tmp_next);
      if (rc != L7_SUCCESS)
      {
         VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Search for MCAST address failed");
         return L7_FAILURE;
      }
      tmp = tmp_next;
      slash = strchr (tmp+ 1, '/');
      if (slash != L7_NULLPTR)
      {
        /* optional integer is there! */
        rc = voipSetNextToken(&(cHeader->addrMulticastTtl), tmp,
                                     '/', &tmp_next);
        if (rc != L7_SUCCESS)
        {
          VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Search for optional integer failed");
          return L7_FAILURE;
        }
        tmp = tmp_next;
        rc = voipSetNextToken(&(cHeader->addrMulticastNum), tmp,
                                     '\r', &tmp_next);
        if (rc != L7_SUCCESS)
        {
          rc = voipSetNextToken(&(cHeader->addrMulticastNum), tmp,
                                     '\n', &tmp_next);
          if (rc != L7_SUCCESS)
          {
            voipSdpConnectionFree (cHeader);
            return L7_FAILURE;
          }
        }
      }/* slash != L7_NULLPTR */
      else
      {
        rc = voipSetNextToken(&(cHeader->addrMulticastTtl), tmp,
                                     '\r', &tmp_next);
        if (rc != L7_SUCCESS)
        {
          rc = voipSetNextToken(&(cHeader->addrMulticastTtl),
                                         tmp, '\n', &tmp_next);
          if (rc != L7_SUCCESS)
          {
            voipSdpConnectionFree (cHeader);
            return L7_FAILURE;
          }

        }
      }
    } /* mcastflag == 1 */
    else
    {
        /* in this case, we have a unicast address */
        rc = voipSetNextToken (&(cHeader->addr), tmp, '\r', &tmp_next);
        if (rc != L7_SUCCESS)
        {
          rc = voipSetNextToken (&(cHeader->addr), tmp, '\n', &tmp_next);
          if (rc != L7_SUCCESS)
          {
             VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Search for unicast addr failed.");
             voipSdpConnectionFree (cHeader);
             return L7_FAILURE;
          }
       }
     }
   }
  /* add the connection at the correct place:
   * if there is no media line yet, then the "c=" is the
   * global one.
   */
  rc = voipListSize (sdp->m_medias);
  if (rc == 0)
    sdp->c_connection = cHeader;
  else
  {
    voipSdpMedia_t *last = (voipSdpMedia_t *) voipListGet (sdp->m_medias, 0);
    voipListAdd (last->connections, cHeader);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse SDP Media attribute
*
* @param   voipSdpMessage_t *sdp  Pointer to SDP message
* @param   const L7_uchar8 *buf   Pointer to packet data
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   m=media port ["/"integer] proto *(payload_number)
*
* @end
*********************************************************************/
static L7_RC_t
voipSdpMessageParseM (voipSdpMessage_t * sdp, L7_uchar8 *buf)
{
  L7_uchar8 *equal;
  L7_uchar8 *tmp;
  L7_uchar8 *tmp_next;
  L7_RC_t   rc;
  voipSdpMedia_t *mHeader;
  L7_uchar8 *slash;
  L7_uchar8 *space;

  equal = buf;
  /* m=media port ["/"integer] proto *(payload_number) */
  while ((*equal != '=') && (*equal != '\0'))
    equal++;
  /* Missing VALUE */
  if (*equal == '\0')
    return L7_FAILURE;

  tmp = equal + 1;

  rc = voipSdpMediaInit (&mHeader);
  if (rc != L7_SUCCESS)
    return rc;


  /* media is "audio" "video" "application" "data" or other... */
  rc = voipSetNextToken (&(mHeader->media), tmp, ' ', &tmp_next);
  if (rc != L7_SUCCESS)
  {
      voipSdpMediaFree (mHeader);
      return L7_FAILURE;
  }
  tmp = tmp_next;

  slash = strchr (tmp, '/');
  space = strchr (tmp, ' ');
  if (space == L7_NULLPTR)
  { /* not possible! */
    voipSdpMediaFree (mHeader);
    return L7_FAILURE;
  }
  if ((slash != L7_NULLPTR) && (slash < space))
  {
    /* a number of port is specified! */
    rc = voipSetNextToken (&(mHeader->port), tmp, '/', &tmp_next);
    if (rc != L7_SUCCESS)
    {
      voipSdpMediaFree (mHeader);
      return L7_FAILURE;
    }
    tmp = tmp_next;
    rc =  voipSetNextToken (&(mHeader->numberOfPort), tmp, ' ', &tmp_next);
    if (rc != L7_SUCCESS)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Get numberOfPort failed");
      voipSdpMediaFree (mHeader);
      return L7_FAILURE;
    }
    tmp = tmp_next;
  }
  else
  {
    rc = voipSetNextToken (&(mHeader->port), tmp, ' ', &tmp_next);
    if (rc != L7_SUCCESS)
    {
       VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Get port failed");
       voipSdpMediaFree (mHeader);
       return L7_FAILURE;
    }
    tmp = tmp_next;
  }

  rc = voipSetNextToken (&(mHeader->protocol), tmp, ' ', &tmp_next);
  if (rc != L7_SUCCESS)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Get protocol failed!!!\n");
    voipSdpMediaFree (mHeader);
    return L7_FAILURE;
  }
  voipListAdd (sdp->m_medias, mHeader);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Parse SDP message
*
* @param   voipSdpMessage_t *sdp  Pointer to SDP message
* @param   const L7_uchar8 *buf   Pointer to packet data
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   The SDP is specified in RFC 4566 , In this application we
*          are concenrned with only two attriutes 1)connection 2)media
*
*
* @end
*********************************************************************/
L7_RC_t voipSdpMessageParse (voipSdpMessage_t * sdp, const L7_uchar8 *buf)
{
  /*
   An example SDP description is:

      v=0
      o=test 2830844536 2830343387 IN IP4 XX.XX.XX.X
      s=SDP Seminar
      i=A Seminar on the session description protocol
      u=http://nowebaddres.com
      e=test@//nowebaddres.com.com
 ==>  c=IN IP4 224.1.15.12/127
      t=2873397496 2873404696
      a=recvonly
 ==>  m=audio 49170 RTP/AVP 32
 ==>  m=video 51372 RTP/AVP 101
      a=rtpmap:99 h263-1998/90000
  */
  L7_uchar8 *ptr;
  L7_RC_t rc;
  const L7_uchar8 *start;
  const L7_uchar8 *end;

  ptr   = (L7_uchar8 *) buf;
  start = buf;

  /* Process SDP fields */
  for (;;)
  {
    if ((start[0] == '\r') ||
        (start[0] == '\n') ||
        (start[0] == '\0') )
    {
       return L7_SUCCESS; /* end of SDP found */
    }
    rc = voipFindNextCrlf (start, &end);
    if (rc == L7_FAILURE)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"End of SDP Header");
      return L7_ERROR;
    }
    ptr = (L7_uchar8 *) start;
    /* if Field is connection description */
    if (start[0] == 'c')
    {
      rc = voipSdpMessageParseC (sdp, ptr);
      if (rc == L7_FAILURE)
      {
        VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Failed to parse connection description in SDP header");
        return L7_FAILURE;
      }
    }
    /* if Field is media description */
    else if (start[0] == 'm')
    {
      rc = voipSdpMessageParseM (sdp, ptr);
      if (rc == L7_FAILURE)
      {
        VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Failed to parse media description in SDP header");
        return L7_FAILURE;
      }
    }
    start = end;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Free the memory allcoated to SDP Message
*
* @param   voipSdpMessage_t *sip (input) Pointer to SDP message
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSdpMessageFree (voipSdpMessage_t *sdp)
{
  L7_uint32 i, size;
  if (sdp == L7_NULLPTR)
    return;

  voipSdpConnectionFree (sdp->c_connection);
  size = voipListSize(sdp->m_medias);
  for(i = 0; i < size; i++)
  {
    voipSdpMedia_t *m;
    m = (voipSdpMedia_t *)voipListGet(sdp->m_medias, i);
    voipSdpMediaFree (m);
  }
  for(i = 0; i< size; i++)
  {
    voipListRemoveNode(sdp->m_medias,0);
  }
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,sdp->m_medias);
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,sdp);
}
