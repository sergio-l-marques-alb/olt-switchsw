/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sip.c
*
* @purpose SIP message parsing routines
*
* @component VOIP |SIP
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
#include <stdio.h>
#include <stdlib.h>
#include "l7_common.h"
#include "voip_exports.h"
#include "voip_parse.h"
#include "voip_sip.h"
#include "voip_sdp.h"
#include "voip_debug.h"
#include "voip_control.h"
#include "osapi_support.h"
#include "dtl_voip.h"
#include "usmdb_util_api.h"

voipSipCall_t       *voipSipCallList=L7_NULLPTR;
/*********************************************************************
* @purpose Allocate memory and initialize SIP Message
*
* @param   voipSipMessage_t ** sip (input) Pointer to SIP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipMessageInit (voipSipMessage_t ** sip)
{
  *sip = (voipSipMessage_t *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
                                            (sizeof (voipSipMessage_t)));
  
  if (*sip == L7_NULLPTR) 
  {
    return L7_FAILURE;
  }
  memset (*sip, 0, sizeof (voipSipMessage_t));

  (*sip)->call_id = L7_NULLPTR;
  (*sip)->content_type = L7_NULLPTR;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Free the memory allcoated to SIP Message
*
* @param   voipSipMessage_t * sip (input) Pointer to SIP message
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSipMessageFree (voipSipMessage_t * sip)
{
  if (sip == L7_NULLPTR)
    return;

  if(sip->sip_method!=L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,sip->sip_method);
  }
  if (sip->call_id != L7_NULLPTR) 
  {
      osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID, sip->call_id);
  }
  if (sip->content_type != L7_NULLPTR) 
  {
      osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID, sip->content_type);
  }
  osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID, sip);
}


/*********************************************************************
* @purpose Look up for a SIP Call
*
* @param   voipSipCall_t   **list    List of SIP calls
* @param   const L7_uchar8 *id       call Id to be searched 
* @param   voipSipCall_t   **entry    SIP call entry 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSipCallLookup(voipSipCall_t **list, 
                  const L7_uchar8 *id, 
                  voipSipCall_t **entry)
{
  while (*list != L7_NULLPTR) 
  {   
    if (strcmp((*list)->call_id, id) == 0) 
    {
      *entry = *list;
      return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Insert a new SIP Call entry
*
* @param   voipSipCall_t   **list    List of SIP calls
* @param   const L7_uchar8 *id       call Id to be inserted
* @param   voipSipCall_t   **ins    SIP call entry to be inserted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSipCallInsert(voipSipCall_t **list, 
                  const L7_uchar8 *id, 
                  voipSipCall_t **ins)
{
    voipSipCall_t   *entry;

    entry = (voipSipCall_t *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
                                                 sizeof(voipSipCall_t));
    if(entry == L7_NULLPTR)
    {
      LOG_MSG("voipSipCallInsert could not allocate memory\n");
      return L7_FAILURE;
    }
    memset(entry, 0, sizeof(voipSipCall_t));

    entry->call_id = (L7_uchar8 *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
                                                            (strlen(id)+1));
    if(entry->call_id == L7_NULLPTR)
    {
      LOG_MSG("voipSipCallInsert (entry->call_id) could not allocate memory\n");
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, entry);
      return L7_FAILURE;
    }
    
    memset(entry->call_id,0x00,(strlen(id)+1));
    strcpy(entry->call_id,id);
    /*Insert it as first entry */
    entry->next = *list;
    *list = entry;
    *ins = entry;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Stop prioritizing VoIP packets/Remove the call from SIP 
*           database
*
* @param    const L7_uchar8 *id      call Id to be removed
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end      
*********************************************************************/
L7_RC_t 
voipSipCallEnd(L7_uchar8 *callId)
{
   L7_RC_t         rc;
   voipSipCall_t   *entry;

   if (callId == L7_NULLPTR)
     return L7_FAILURE;

   rc = voipSipCallRemove(&voipSipCallList, callId, &entry);
   if ((rc == L7_SUCCESS) && (entry->valid == L7_TRUE))
   {
     rc = voipSetupSipSession(&(entry->db), L7_DISABLE);
     if (rc != L7_SUCCESS)
     {
       VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Failed to delete the session ");
       return rc;
     }
     osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,entry->call_id);
     osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,entry);
   }
   return rc;
}
/*********************************************************************
* @purpose Remove SIP Call entry
*
* @param   voipSipCall_t   **list   List of SIP calls
* @param   const L7_uchar8 *id      call Id to be removed
* @param   voipSipCall_t   **del     Pointer to SIP call remeved entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSipCallRemove(voipSipCall_t **list, const L7_uchar8 *id, voipSipCall_t **del)
{
  voipSipCall_t   *entry;
  L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];

  while (*list != L7_NULLPTR) 
  {
    if (strcmp((*list)->call_id, id) == 0) 
    {
      entry = *list;
      *list = entry->next;
      *del  = entry;

      /* Log the event */
      usmDbInetNtoa(entry->db.src_ip, strSrcIPaddr);
      usmDbInetNtoa(entry->db.dst_ip, strDstIPaddr);

      LOG_MSG("Removed SIP call id = %s\r\n Source IP = %s, Dest IP = %s\r\n"
               "Source Port = %d, Dest Port = %d\r\n", 
               entry->call_id, strSrcIPaddr, strDstIPaddr,
               entry->db.l4_src_port, entry->db.l4_dst_port);

      return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Enable/disable a SIP session
*
* @param   voipFpEntry_t *s         Pointer to voipFpEntry_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
* @param   L7_uint32 interface      interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSetupSipSession(voipFpEntry_t *s, L7_uint32 enable)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];

  rc = dtlVoipProfileParamSet(enable, L7_QOS_VOIP_PROTOCOL_SIP,
                              s->src_ip,s->dst_ip,s->l4_src_port,s->l4_dst_port); 

  if (rc == L7_SUCCESS)
  {
     if (enable == L7_ENABLE)
       voipTimerAdd();
     else if (enable == L7_DISABLE)
      voipTimerDelete();
    
    /* Log the event */
    usmDbInetNtoa(s->src_ip, strSrcIPaddr);
    usmDbInetNtoa(s->dst_ip, strDstIPaddr);

    LOG_MSG("Set up new SIP\r\n Source IP = %s, Dest IP = %s\r\n"
             "Source Port = %d, Dest Port = %d\r\n", 
             strSrcIPaddr, strDstIPaddr,
             s->l4_src_port, s->l4_dst_port);
  }
  return rc;
}
/*********************************************************************
* @purpose Parse SIP message startline request
*
* @param   voipSipMessage_t *dest    Pointer to sip Message 
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   const L7_uchar8 **headers Pointer to header
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes   Request-Line = Method SP Request-URI SP SIP-Version CRLF
*          SP = space
*          CRLF = carriage return Line feed  
*
* @end
*********************************************************************/
static L7_RC_t
voipSipMessageStartlineParseReq (voipSipMessage_t * dest, const L7_uchar8 *buf,
                                   const L7_uchar8 **headers)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 *next = L7_NULLPTR;

  dest->sip_method = L7_NULLPTR;
  dest->status_code = 0;

  *headers = buf;
  
  /* Intialize dest->sip_method with what is in packet, 
   * next would point to Request_URI 
   */
  rc = voipSetNextToken (&(dest->sip_method), (L7_uchar8 *)buf, ' ', &next);

  if (rc != L7_SUCCESS) 
  {
     VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Failed to get the next method");
     return L7_FAILURE;
  }

  /* find the beginning of headers by skipping the pointer till CRLF is met*/
  {
    const L7_uchar8 *hp = next;

    while ((*hp != '\r') && (*hp != '\n')) 
    {
      if (*hp)
      {
        hp++;
      }
      else 
      {
        osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,dest->sip_method);
        dest->sip_method = L7_NULLPTR;
        return L7_FAILURE;
      }
    }
    hp++;
    if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0]))
    {
      hp++;
    }
    (*headers) = hp;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse SIP message startline Response
*
* @param   voipSipMessage_t * dest   Pointer to SIP message
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   const L7_uchar8 **headers Pointer to SIP message Header
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  Status-Line = SIP-Version SP Status-Code SP Reason-Phrase CRLF
*
* @end
*********************************************************************/
static int
voipSipMessageStartlineParseResp (voipSipMessage_t * dest, const L7_uchar8 *buf,
                                  const L7_uchar8 **headers)
{
  const L7_uchar8 *statuscode;
  const L7_uchar8 *reasonphrase;

  dest->sip_method = L7_NULLPTR;

  *headers = buf;
  
  statuscode = strchr (buf, ' ');       /* search for first SPACE */
  if (statuscode == L7_NULLPTR) 
  {
     VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "No status code in the packet");
     return L7_FAILURE;
  }
  
  reasonphrase = strchr (statuscode + 1, ' ');
  if (reasonphrase == L7_NULLPTR) 
  {
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "No reason phrase in the packet");
    return L7_FAILURE;
  }
  
  if (sscanf(statuscode + 1, "%d", &dest->status_code) != 1) 
  {
    /* Non-numeric status code */
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Non-numeric status code in the packet");
    return L7_FAILURE;
  }

  if (dest->status_code == 0)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Status_code is 0 in that packet");
    return L7_FAILURE;
  }
  /* Search for CRLF and find header field */
  {
    const char *hp = reasonphrase;

    while ((*hp != '\r') && (*hp != '\n')) 
    {
      if (*hp)
      {
        hp++;
      }
      else 
      {
        return L7_FAILURE;
      }
    }
    hp++;
    if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0]))
    {
      hp++;
    }
    (*headers) = hp;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse SIP message startline 
*
* @param   voipSipMessage_t * dest   Pointer to SIP message
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   const L7_uchar8 **headers Pointer to SIP message Header
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  startline can either be a request-line or response-line
*
* @end
*********************************************************************/
static L7_RC_t
voipSipMessageStartlineParse (voipSipMessage_t * dest, const L7_uchar8 *buf,
                                const L7_uchar8 **headers)
{

  if (strncmp (buf, (const  L7_uchar8 *) "SIP/", 4) == 0)
    return voipSipMessageStartlineParseResp (dest, buf, headers);
  else
    return voipSipMessageStartlineParseReq  (dest, buf, headers);
}
/*********************************************************************
* @purpose Parse SIP message header
*
* @param   voipSipMessage_t *sip    Pointer to SIP message
* @param   const L7_uchar8 *start   start of header 
* @param   const L7_uchar8 **body   Pointer to SIP message body
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  header = "header-name" HCOLON header-value *(COMMA header-value)
*                   CRLF
*         Header may consist of multiply header fields each sepearted by
*         CRLF we are just bothered with two Call-id, and content-type.
*         To indicate end of header fields there should be atleast 1
*         empty line with CRLF at end
* @end
*********************************************************************/
static L7_RC_t
voipSipHeaderParse (voipSipMessage_t *sip, const L7_uchar8 *start,
                   const L7_uchar8 **body)
{
  const L7_uchar8 *colon;      /* Pointer to ':'(HCOLON) */
  L7_uchar8       *hname;
  L7_uchar8       *hvalue;
  const L7_uchar8 *end;
  L7_RC_t rc;

  for (;;) 
  {
    /* Could not understand this case */
    if (start[0] == '\0') 
    {
      /* final CRLF is missing */
      return L7_SUCCESS;
    }
    /* After the function call end will point to next header field */
    rc = voipFindNextCrlf (start, &end);
    if (rc == L7_FAILURE) 
    {
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "No CRLF");
      return rc;            /* this is an error case!     */
    }

    /* The list of headers MUST always end with  
     * CRLFCRLF (also CRCR and LFLF are allowed) 
     */
    if ((start[0] == '\r') || (start[0] == '\n')) 
    {
      *body = start;
       return L7_SUCCESS;       /* end of header found        */
    }

    /* find the header name */
    colon = strchr (start, ':');
    if (colon == L7_NULLPTR) 
    {
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "No colon\n");
      return L7_FAILURE;            /* this is also an error case */
    }
    /* only colon is present no header name */
    if (colon - start + 1 < 2)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Message is incorrect:\
                         only colon is present no header name");
      return L7_FAILURE;
    }
    /* colon is present after the end */
    if (end <= colon) 
    {
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP,"Colon at wrong place:\
                               colon is present after the end.");
      return L7_FAILURE;
    }
    
    hname = (L7_uchar8 *)osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,
                                             ((colon - start) + 1));
    if(hname == L7_NULLPTR)
    {
      LOG_MSG("SIP:voipSipHeaderParse could not allocate memory\n");
      return L7_FAILURE;
    }
    voipClrncpy (hname, start, colon - start);
    /* LOOK for Header field*/ 
    {
      const L7_uchar8 *endTemp;

        /* END of header is (end_of_header-2) if header separation is CRLF 
         * END of header is (end_of_header-1) if header separation is CR or LF 
         */
        if ((end[-2] == '\r') || (end[-2] == '\n'))
          endTemp = end - 2;
        else
          endTemp = end - 1;
        if ((endTemp) - colon < 2)
          hvalue = L7_NULLPTR;        /* some headers (subject) can be empty */
        else 
        {
          hvalue = (L7_uchar8 *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID ,
                                                        (end) - colon + 1);
          voipClrncpy (hvalue, colon + 1, (endTemp) - colon - 1);
        }
      }
      /* check for various header fields 
       * Unique indentifier for SIP session 
       */ 
      if (strncmp (hname, "Call-ID", 7) == 0) 
      {
        sip->call_id = hvalue;
      } 
      else if (strncmp (hname, "Content-Type", 12) == 0) 
      {
        sip->content_type = hvalue;
      } 
      else 
      {
        if (hvalue != L7_NULLPTR)
          osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,hvalue);
      }
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,hname);
      /* continue on the next header */
      start = end;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Update the Entry with SDP field values
*
* @param   voipFpEntry_t    *entry      Pointer to voipFpEntry_t
* @param   voipSdpMessage_t *sdp        Pointer to SDP message
* @param   L7_uint32 source             If the Port/IP are of
*          source or destination side   
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  
*
* @end
*********************************************************************/
static L7_RC_t
voipSipCallUpdateSdp(voipFpEntry_t *entry, 
                     voipSdpMessage_t *sdp, 
                     L7_uint32 source)
{
  L7_uint32 i, size;
  voipSdpMedia_t *media = L7_NULLPTR;

  size = voipListSize(sdp->m_medias);
  if (size == 0)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Number of media paramaetrs in SDP header are 0");
    return L7_FAILURE;
  }
  for(i = 0; i < size; i++)
  {
    media = voipListGet(sdp->m_medias, i);
    if (strcmp(media->media, "audio") == 0)
      break;
  }
  if (i == size)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Can't find audio type in media parameter.");
    return L7_FAILURE;
  }
  if (source)
  {
    /* TBD not checking if any connections are present in media 
     * set ip and port 
     */
    if(osapiInetPton(L7_AF_INET,sdp->c_connection->addr,
                    (L7_uchar8 *)&entry->src_ip)!=L7_SUCCESS)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"osapiInetPton source failed ip = %s\n", 
                                                      sdp->c_connection->addr);
       return L7_FAILURE;
    }
    entry->l4_src_port = atoi(media->port);
  }
  else
  {
    /* set ip and port */
    if(osapiInetPton(L7_AF_INET,sdp->c_connection->addr,
                    (L7_uchar8 *)&entry->dst_ip)!=L7_SUCCESS)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"osapiInetPton destn failed ip = %s\n", 
                                                     sdp->c_connection->addr);
      return L7_FAILURE;
    }
    entry->l4_dst_port = atoi(media->port); 
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Parse SIP message and extract SDP message
*
* @param   const L7_uchar8 *ipstr    Pointer to IpAddress
* @param   voipSipMessage_t * sip    Pointer to SIP Message 
* @param   voipSdpMessage_t * sdp    Pointer to SDP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t voipSipMessageParseSdp(const L7_uchar8 *ipstr, 
                                      voipSipMessage_t *sip, 
                                      voipSdpMessage_t *sdp)
{
  voipSipCall_t       *entry = L7_NULLPTR;
  L7_RC_t rc;


  /* Trace with Packet Type*/  
  VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Message type = %s\r\n", sip->sip_method);

  /* Process the packet type request*/
  if (MSG_IS_REQUEST(sip)) 
  {
    if (MSG_IS_INVITE(sip)) 
    {
      rc = voipSipCallLookup(&voipSipCallList, sip->call_id, &entry);
      if (rc == L7_SUCCESS) 
      {
        if (entry->valid) 
        {
          /* Have to deal with SIP proxy server */
          L7_uint32 dstip;
          dstip  = (ipstr[0] << 24) | (ipstr[1] << 16) |
                   (ipstr[2] << 8) | (ipstr[3] << 0);

          if (dstip == entry->db.src_ip) 
          {
            entry->db.dst_ip = 0;
            voipSetupSipSession(&(entry->db), L7_ENABLE);
          } 
          else if (dstip == entry->db.dst_ip) 
          {
            entry->db.src_ip = 0;
            voipSetupSipSession(&(entry->db), L7_ENABLE);
          } 
          else 
          {
            LOG_MSG("SIP:voipSipMessageParseSdp should not happen!!!\n");
          }
        } 
        else 
        {
          /* TODO: should check if parameter changed? */
        }
      } 
      else 
      {
        if (sdp == L7_NULLPTR) 
        {
          VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "No SDP header in the packet");
          return L7_FAILURE;
        }
        rc = voipSipCallInsert(&voipSipCallList, sip->call_id,&entry);
        if(rc != L7_SUCCESS)
        {
          return rc;   
        }          
        rc = voipSipCallUpdateSdp(&(entry->db), sdp, 1);
        if (rc != L7_SUCCESS)
          return L7_FAILURE;
      }
      return L7_SUCCESS;
    } 
    else if ((MSG_IS_CANCEL(sip))||(MSG_IS_BYE(sip)))
    {
      return voipSipCallEnd(sip->call_id);
    }
  }/* MSG_IS_REQUEST(sip) */ 
  else 
  {
    if(sip->status_code == SIP_OK) 
    {
      if (sip->call_id != L7_NULLPTR) 
      {
        rc = voipSipCallLookup(&voipSipCallList, sip->call_id, &entry);
        /* call id found */
        if (rc == L7_SUCCESS) 
        {
          if (entry->valid == L7_FALSE)
          { 
            if (sdp == L7_NULLPTR) 
            {
             /* Not the OK for INVITE .... */
             return L7_FAILURE;
             }
            rc = voipSipCallUpdateSdp(&(entry->db), sdp, 0);
            if (rc != L7_SUCCESS)
               return L7_FAILURE;

            entry->valid = L7_TRUE;
            /* check if we run out of entries */
           
            rc = voipSetupSipSession(&(entry->db), L7_ENABLE);
            if (rc != L7_SUCCESS) 
            {
              VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Failed to install policy");
              rc = voipSipCallRemove(&voipSipCallList, sip->call_id, &entry);
              if (rc == L7_SUCCESS) 
              {
                osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,entry->call_id);
                osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,entry);
              }
              return L7_FAILURE;
            }
          } 
          else 
          {
            /* TODO: should check if parameter changed */
          }
        }
      }
    }/* sip->status_code == SIP_OK */ 
    else if (sip->status_code >= 300) 
    {
      return voipSipCallEnd(sip->call_id);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse SIP message
*
* @param   const L7_uchar8 *ipstr    Pointer to IpAddress(Destn IP)
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   L7_uint32        length   length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipMessageParse (const L7_uchar8 *ipstr, 
                             const L7_uchar8 *packetBuf,
                             L7_uint32 length)
{
  L7_RC_t            rc;
  const L7_uchar8   *nextHeaderIndex;
  L7_uchar8         *tmp;
  L7_uchar8         *beg, *startOfBody;
  voipSipMessage_t  *sip =L7_NULLPTR;
  voipSdpMessage_t  *sdp = L7_NULLPTR;
  L7_uint32         offset =0;
  L7_uint32         i, size;
  voipSdpMedia_t *media = L7_NULLPTR;

   
  offset = sysNetDataOffsetGet((L7_uchar8 *)packetBuf); 
  beg = (L7_uchar8*)packetBuf;
  tmp = (L7_uchar8*)(packetBuf + offset +28);/* pointer to SIP Header */

  voipSipMessageInit(&sip);

  /* parse request or status line */
  rc = voipSipMessageStartlineParse (sip, tmp, &nextHeaderIndex);
  if (rc == L7_FAILURE)
  {
    voipSipMessageFree(sip);
    return L7_FAILURE;
  }

  tmp = (L7_uchar8 *) nextHeaderIndex;

  /* parse headers */
  rc = voipSipHeaderParse (sip, tmp, &nextHeaderIndex);
  if (rc == L7_FAILURE)
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP,"Failed to parse SIP headers");
    voipSipMessageFree(sip);
    return L7_FAILURE;
  }
  tmp = (L7_uchar8 *) nextHeaderIndex;

  VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP, 
            "\r\nSIP Method = %s, Status Code = %d,\r\n"
            "CallId = %s, ContentType = %s\r\n",
            sip->sip_method, sip->status_code, sip->call_id, sip->content_type);

  /* start parsing SDP, get rid of the first CRLF */
  if ('\r' == tmp[0])
  {
    if ('\n' == tmp[1])
      startOfBody = tmp + 2;
    else
      startOfBody = tmp + 1;
  }
  else if ('\n' == tmp[0])
  {
     startOfBody = tmp + 1;
  }
  else
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SIP, "messsage does not end with CRLFCRLF");
    voipSipMessageFree(sip);
    return L7_FAILURE;
  }
  /* Request or Response with SDP body */
  if (startOfBody[0] != '\0')
  {
    voipSdpMessageInit(&sdp);

    rc = voipSdpMessageParse(sdp, startOfBody);
    if (rc ==L7_FAILURE)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "Parsing of SDP Header failed");
      voipSipMessageFree(sip);
      voipSdpMessageFree(sdp);
      sdp = L7_NULLPTR;
      return L7_FAILURE;
    }
#if 0
    VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "SDP Details: Network Type = %s\r\n"
              "Network Address Type = %s, Address = %s,\r\n"
              "Multicast Address TTL = %s, Number of multicast address = %s\r\n",
              sdp->c_connection->nettype,
              sdp->c_connection->addrtype,
              sdp->c_connection->addr,
              sdp->c_connection->addrMulticastTtl,
              sdp->c_connection->addrMulticastNum);
#endif
    size = voipListSize(sdp->m_medias);
    for(i = 0; i < size; i++)
    {
      media = voipListGet(sdp->m_medias, i);
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SIP, "SDP Media details: media type = %s\r\n"
                "port number = %s, number of port = %s, protocol to be used = %s\r\n",
                media->media, media->port, media->numberOfPort, media->protocol);
    }
  }

  voipSipMessageParseSdp(ipstr, sip, sdp);

  voipSipMessageFree(sip);
  if (sdp != L7_NULLPTR)
  {
    voipSdpMessageFree(sdp);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear all call entries corrosponding to an interface
*
* @param    L7_uint32 interface       Interface number for which entries
*           need to be cleared
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipClearInterfaceEntries()
{
  voipSipCall_t *entry = voipSipCallList;

  while(entry!=L7_NULLPTR)
  {
    voipSipCallEnd(entry->call_id);
    entry = entry->next;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  voipSipTimerAction 
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes i) Get the total number of packets belongs to the each call and 
*           compare with previous count.End the call if the previoues 
*           and current count is equal 
*    
*        
* @end
*********************************************************************/
L7_RC_t voipSipTimerAction()
{
  voipSipCall_t *entry = voipSipCallList;
  L7_ulong64     counter;

  while(entry!=L7_NULLPTR)
  {
    if (entry->valid == L7_TRUE)
    {
      if (dtlVoipSessionStatsGet(entry->db.src_ip,
                                 entry->db.dst_ip,
                                 entry->db.l4_src_port,
                                 entry->db.l4_dst_port,
                                 &counter) == L7_SUCCESS)
      {
        if((entry->prev_counter.high == counter.high) &&
           (entry->prev_counter.low == counter.low)) 
        {
           if (voipSetupSipSession(&(entry->db), L7_DISABLE) == L7_SUCCESS)
            voipSipCallEnd(entry->call_id);
        }
      }
    }
    entry = entry->next;
  }
  return L7_SUCCESS;
}

/************************ DEBUG FUNCTION ****************************/ 
void voipSipCallDump()
{
  voipSipCall_t *temp = voipSipCallList;
  L7_uint32   i=0;

  printf("\r\n SIP call database info");
  printf("\r\n -----------------------");

  while(temp!=L7_NULLPTR)
  {
    printf("\r\nindex           =%d",i);
    printf("\r\nCall Id         =%s", temp->call_id);
    printf("\r\nvalid           = %d",temp->valid);
    printf("\r\nsrc ipaddress   =0x%x",temp->db.src_ip);
    printf("\r\ndst ipaddress   =0x%x",temp->db.dst_ip);
    printf("\r\nsrc port        =0x%x",temp->db.l4_src_port);
    printf("\r\ndest port       = 0x%x",temp->db.l4_dst_port);
    temp = temp->next;
    i++;
  }
  printf("\r\n ------End-------------");
}


