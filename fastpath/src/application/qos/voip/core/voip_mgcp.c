/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_mgcp.c
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
#include <stdio.h>
#include <stdlib.h>
#include "voip_exports.h"
#include "voip_mgcp.h"
#include "voip_sdp.h"
#include "osapi_support.h"
#include "dtl_voip.h"

static voipMgcpCa_t *voipMgcpCaList=L7_NULLPTR;

/*********************************************************************
* @purpose Look up for a MGCP Call agent
*
* @param   voipMgcpCa_t   **list    List of MGCP call agents
* @param   const L7_uchar8 *ip      Agent IP to be searched
* @param   voipMgcpCa_t   **entry    MGCP call agent entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCaLookup(voipMgcpCa_t **list,const L7_uchar8 *ip, voipMgcpCa_t **entry)
{
  while (*list != L7_NULLPTR)
  {
    if (memcmp((*list)->ip, ip, 4)==0)
    {
       /*VOIP_DBG(("_mgcp_ca_lookup : CA ip matched\n"));*/
       *entry = *list;
       return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Look up for a MGCP Call ID
*
* @param   voipMgcpCa_t   *ca       List of MGCP call agents
* @param   const L7_uchar8 *id      Call Id to be searched
* @param   voipMgcpCall_t  **entry  MGCP call entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCallLookup(voipMgcpCa_t *ca,const L7_uchar8 *id,voipMgcpCall_t **entry)
{
   voipMgcpCall_t *list = ca->call;

   while (list != L7_NULLPTR)
   {
     if (list->msg == L7_NULLPTR)
     {
        /*VOIP_DBG(("_mgcp_calls_lookup : list->msg is L7_NULLPTR\n"));*/
        return L7_FAILURE;
     }
     if (list->msg->callId == L7_NULLPTR)
     {
        /*VOIP_DBG(("_mgcp_calls_lookup : list->msg->callId is L7_NULLPTR\n"));*/
        return L7_FAILURE;
     }
     if (strcmp(list->msg->callId,id)==0)
     {
        /*VOIP_DBG(("_mgcp_calls_lookup : call id match\n"));*/
        *entry = list;
        return L7_SUCCESS;
     }
     list = list->next;
   }
   return L7_FAILURE;
}
/*********************************************************************
* @purpose Insert a new MGCP Call agent entry
*
* @param   voipMgcpCa_t   **list    List of MGCP call agents
* @param   const L7_uchar8 *ip      agent Ip to be inserted
* @param   voipMgcpCa_t   **ins      MGCP call agent insterted entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED(if entry
*           already exists )
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCaInsert( voipMgcpCa_t **list,const L7_uchar8 *ip, voipMgcpCa_t **ins)
{
  voipMgcpCa_t *entry = L7_NULLPTR;
  L7_RC_t rc;

  if ((rc = voipMgcpCaLookup(list, ip, &entry)) == L7_SUCCESS)
  {
    *ins = entry;
    return L7_ALREADY_CONFIGURED;
  }
  if ((entry = (voipMgcpCa_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
       sizeof(voipMgcpCa_t))) == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpCaInsert could not allocate memory\n");
    return L7_FAILURE;
  }
  memset(entry, 0x00, sizeof(voipMgcpCa_t));
  /* set ip*/
  memcpy(entry->ip, ip, 4);
  entry->next = *list;
  *list = entry;
  *ins = entry;

  /* VOIP_DBG(("add mgcp CA ip = %d.%d.%d.%d\n",
                 entry->ip[0],entry->ip[1],entry->ip[2],entry->ip[3]));*/
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Insert a new MGCP Call entry
*
* @param   voipMgcpCa_t   *list     List of MGCP call agents
* @param   const L7_uchar8  *ip     Ip to be inserted
* @param   voipMgcpMessage_t *m     pointer to voipMgcpMessage_t
* @param   voipMgcpCall_t   **ins    MGCP inserted call entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCallInsert(voipMgcpCa_t *list,const L7_uchar8 *ip,voipMgcpMessage_t *m,voipMgcpCall_t **ins)
{
  voipMgcpCall_t *entry = L7_NULLPTR;
  L7_RC_t rc;

  if ((rc = voipMgcpCallLookup(
               list,
               m->callId,
               &entry)) == L7_SUCCESS)
  {
    *ins = entry;
    return L7_ALREADY_CONFIGURED;
  }

  /*VOIP_DBG(("_mgcp_calls_insert call id = %s, tran id = %s\n",
               m->callId, m->tran_id));*/
  if ((entry = (voipMgcpCall_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
               sizeof(voipMgcpCall_t ))) == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpCallInsert could not allocate memory\n");
    return L7_FAILURE;
  }
  memset(entry, 0x00, sizeof(voipMgcpCall_t));
  entry->msg = m;
  memcpy(entry->ip, ip, 4);
  list->count++;
  entry->next = list->call;
  list->call = entry;
  *ins = entry;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Remove MGCP Call agent entry
*
* @param   voipMgcpCa_t   **list  List of MGCP call agents
* @param   voipMgcpCa_t   *del   call agent entry to be deleted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCaRemove(voipMgcpCa_t **list, voipMgcpCa_t *del)
{
  voipMgcpCa_t *entry;
  while (*list != L7_NULLPTR)
  {
     if ((*list) == del)
     {
       entry = *list;
        /*VOIP_DBG(("remove mgcp CA ip = %d.%d.%d.%d\n",
                 entry->ip[0],entry->ip[1],entry->ip[2],entry->ip[3]));*/
       *list = entry->next;
       osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,del);
       return L7_SUCCESS;
     }
     list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Remove MGCP Call entry
*
* @param   voipMgcpCa_t   *list     List of MGCP call agents
* @param   voipMgcpCall_t   *del    MGCP call entry to be removed
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes   Memory is freed by calling function
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCallRemove(voipMgcpCa_t *list,voipMgcpCall_t *del)
{
  voipMgcpCall_t *entry = list->call;
  voipMgcpCall_t *prev = list->call;

  while(entry != L7_NULLPTR)
  {
    if (entry == del)
    {
      /*VOIP_DBG(("remove mgcp call id = %s\n", del->msg->callId)); */
      /* the first node */
      if (prev == entry)
      {
        list->call = entry->next;
      }
      else
      {
         prev->next = entry->next;
      }
      list->count--;
      return L7_SUCCESS;
    }
    prev = entry;
    entry = entry->next;
  }
  return L7_FAILURE;
}
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
L7_RC_t voipMgcpMessageInit (voipMgcpMessage_t **mgcp)
{
  *mgcp = (voipMgcpMessage_t *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID, sizeof(voipMgcpMessage_t));
  if (*mgcp == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "MGCP:Could not allocate memory\n");
    return L7_FAILURE;
  }
  memset (*mgcp, 0x00, sizeof (voipMgcpMessage_t));

  (*mgcp)->mgcpCommand = L7_NULLPTR;
  (*mgcp)->returnCode = 0;
  (*mgcp)->tranId = L7_NULLPTR;
  (*mgcp)->callId = L7_NULLPTR;
  (*mgcp)->connectionId = L7_NULLPTR;
  (*mgcp)->notifiedId = L7_NULLPTR;
  return L7_SUCCESS;

}
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
void voipMgcpMessageFree (voipMgcpMessage_t *mgcp)
{
  if (mgcp == L7_NULLPTR)
    return;
  if(mgcp->mgcpCommand!=L7_NULLPTR)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp->mgcpCommand);
    mgcp->mgcpCommand = L7_NULLPTR;
  }
  if(mgcp->tranId!=L7_NULLPTR)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp->tranId);
    mgcp->tranId = L7_NULLPTR;
  }
  if(mgcp->callId!=L7_NULLPTR)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp->callId);
    mgcp->callId = L7_NULLPTR;

  }
  if(mgcp->notifiedId!=L7_NULLPTR)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp->notifiedId);
    mgcp->notifiedId = L7_NULLPTR;
  }
  if(mgcp->connectionId!=L7_NULLPTR)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp->connectionId);
    mgcp->connectionId = L7_NULLPTR;
  }
  osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,mgcp);

}
/*********************************************************************
* @purpose Enable/disable a MGCP session
*
* @param   voipFpEntry_t *s         Pointer to voipFpEntry_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t voipSetupMgcpSession(voipFpEntry_t *s, L7_uint32 enable)
{
  dtlVoipProfileParamSet(enable, L7_QOS_VOIP_PROTOCOL_MGCP,
                         s->src_ip,s->dst_ip,s->l4_src_port,s->l4_dst_port);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose To check if OK message is for recevied CRCX from the same IP
*
* @param   voipMgcpCa_t *ca           pointer to list of MGCP call agents
* @param   const L7_uchar8 *ip      Ip to be looked
* @param   L7_uchar8       *tranId  Transaction Id to be looked up
* @param   voipMgcpCall_t   **entry  MGCP call entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCallLookupTranId(voipMgcpCa_t *ca, const L7_uchar8 *ip, L7_uchar8 *tranId, voipMgcpCall_t **entry)
{
  voipMgcpCall_t *list = ca->call;
  while (list != L7_NULLPTR)
  {
    if ((strcmp(list->msg->tranId, tranId) == 0)&& (strncmp(list->ip, ip, 4) == 0))
    {
      /*VOIP_DBG(("_mgcp_calls_lookup_tranid : transaction id match\n"));*/
      *entry = list;
      return L7_SUCCESS;
    }
    list = list->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Update the Entry with SDP field values
*
* @param   voipFpEntry_t    *entry      Pointer to voipFpEntry_t
* @param   voipSdpMessage_t *sdp        Pointer to SDP message
* @param   L7_uint32 source             if the values in SDP
*          are on source or destination side
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpCallUpdateSdp(voipFpEntry_t *entry, voipSdpMessage_t *sdp, L7_uint32 source)
{
  L7_uint32 i, size;
  voipSdpMedia_t *media = L7_NULLPTR;

  if (sdp == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "Should not happen!!! sdp = NULL\n");
    return L7_FAILURE;
  }

  size = voipListSize(sdp->m_medias);
  if (size == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "Should not happen!!! sdp->m_medias = 0\n");
    return L7_FAILURE;
  }
  for(i = 0; i < size; i++)
  {
    media = voipListGet(sdp->m_medias, i);
    if (strcmp(media->media, "audio") == 0)
      break;
  }
  if (source)
  {
    /* TBD not checking if any connections are present in media */
    /* set ip and port */
    if(osapiInetPton(L7_AF_INET,sdp->c_connection->addr,(L7_uchar8 *)&entry->src_ip)!=L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "osapiInetPton source failed ip = %s\n", sdp->c_connection->addr);
       return L7_FAILURE;
    }
    entry->l4_src_port = atoi(media->port);
  }
  else
  {
     /* set ip and port */
     if(osapiInetPton(L7_AF_INET,sdp->c_connection->addr,(L7_uchar8 *)&entry->dst_ip)!=L7_SUCCESS)
     {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
               "osapiInetPton destn failed ip = %s\n", sdp->c_connection->addr);
       return L7_FAILURE;
     }
     entry->l4_dst_port = atoi(media->port);
     /*VOIP_DBG(("entry->dst_ip = %d.%d.%d.%d\n", ipstr[0],ipstr[1],ipstr[2],ipstr[3]));*/
     /* VOIP_DBG(("entry->l4_dst_port = %d\n", entry->l4_dst_port));*/
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse MGCP message startline request
*
* @param   voipMgcpMessage_t *dest   Pointer to MGCP Message
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   const L7_uchar8 **headers Pointer to header
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes  COMMANDANME SP TRANSID SP ENDPOINT SP PROTOCOL VERSION
*         SP space
* @end
*********************************************************************/
static L7_RC_t
voipMgcpMessageStartlineParseReq (voipMgcpMessage_t *dest, const L7_uchar8 *buf,
                                   const L7_uchar8 **headers)
{
  const L7_uchar8 *p1;
  const L7_uchar8 *p2;

  dest->mgcpCommand = L7_NULLPTR;
  dest->returnCode = 0;

  *headers = buf;

  /* The first token is the command name: */
  p2 = strchr (buf, ' ');
  if (p2 == L7_NULLPTR)
  { /* Missing Space */
    return L7_FAILURE;
  }
  if (*(p2 + 1) == '\0' || *(p2 + 2) == '\0')
  {
    return L7_FAILURE;
  }
  if (p2 - buf == 0)
  {
    /* space is first character */
    return L7_FAILURE;
  }
  dest->mgcpCommand = (L7_uchar8 *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,(p2 - buf + 1));
  if( dest->mgcpCommand == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpMessageStartlineParseReq : could not allocate memory\n");
    return L7_FAILURE;
  }
  voipStrncpy (dest->mgcpCommand, buf, p2 - buf);

  /* Look up CRCX, MDCX, and DLCX only TBD not checking MDCX ??*/
  if (MGCPMSG_IS_CRCX(dest) || MGCPMSG_IS_DLCX(dest))
  {
    /* The second token is a trasaction id */
    p1 = strchr (p2 + 1, ' ');
    if (p1 == L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, dest->mgcpCommand);
      dest->mgcpCommand = L7_NULLPTR;
      return L7_FAILURE;
    }
    dest->tranId = (L7_uchar8 *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,(p1 - p2));
    if( dest->tranId == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "voipMgcpMessageStartlineParseReq : could not allocate memory\n");
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, dest->mgcpCommand);
      dest->mgcpCommand = L7_NULLPTR;
      return L7_FAILURE;
    }
    voipClrncpy  (dest->tranId, p2 + 1, (p1 - p2 - 1));
    /*VOIP_DBG(("%s %s\n", dest->mgcp_command, dest->tran_id));*/
    /* The third token is a endpoint id */
    p2 = strchr (p1 + 1, ' ');
    if (p2 == L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, dest->mgcpCommand);
      dest->mgcpCommand = L7_NULLPTR;
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, dest->tranId);
      dest->tranId = L7_NULLPTR;
      return L7_FAILURE;
    }
    /* find the the version and the beginning of headers */
    {
      const L7_uchar8 *hp = p2;

      while ((*hp != '\r') && (*hp != '\n'))
      {
        if (*hp)
          hp++;
        else
        {
          /* assume voipMgcpMessageFree will free memory allocated before */
          return L7_FAILURE;
        }
      }
      hp++;
      if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0]))
        hp++;
      (*headers) = hp;
    }
    return L7_SUCCESS;
  } /* if CRCX or MDCX or DLCX */
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Parse MGCP message startline response
*
* @param   voipMgcpMessage_t *dest   Pointer to MGCP Message
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   const L7_uchar8 **headers Pointer to header
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes RESPONSECODE(3 digit) SP TRANSID SP COMMENT
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpMessageStartlineParseResp (voipMgcpMessage_t *dest, const L7_uchar8 *buf,
                                   const L7_uchar8 **headers)
{
  const L7_uchar8 *tran_id;
  const L7_uchar8 *comment;

  dest->mgcpCommand = L7_NULLPTR;
  *headers = buf;

  if (sscanf (buf, "%d", &dest->returnCode) != 1)
  {
    /* Non-numeric status code */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpMessageStartlineParseResp can't find return code!\n");
    return L7_FAILURE;
  }
  if (dest->returnCode == 0)
  {
    return L7_FAILURE;
  }
  /* Look up OK only */
  if (MGCPMSG_IS_STATUS_200(dest))
  {
    /* search for same transaction id */
    /* Could be like 200 1002 OK */
    tran_id = strchr(buf, ' ');
    if (tran_id == L7_NULL)
    {
      return L7_FAILURE;
    }

    comment = strchr(tran_id+1, ' ');
    if (comment == L7_NULL)
    {
      return L7_FAILURE;
    }

    dest->tranId = (L7_uchar8*) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID, comment - tran_id);
    voipClrncpy (dest->tranId, tran_id + 1, (comment - tran_id - 1));
    /*VOIP_DBG(("OK %s\n", dest->tran_id));*/
    {
     const L7_uchar8 *hp = comment;
     while ((*hp != '\r') && (*hp != '\n'))
     {
       if (*hp)
         hp++;
       else
       {
         return L7_FAILURE;
       }
     }
     hp++;
     if ((*hp) && ('\r' == hp[-1]) && ('\n' == hp[0])) {
        hp++;
     }

     /* We only care about OK with connection id,The next line in
        response ie is the header should start with I ex. I: 1 */
     if (hp[0] != 'I' && hp[0] != 'i')
     {
       return L7_FAILURE;
     }
     (*headers) = hp;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse MGCP message startline
*
* @param   voipMgcpMessage_t * dest  Pointer to MGCP message
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   const L7_uchar8 **headers Pointer to MGCP message Header
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes MGCPMessage = MGCPCommand / MGCPResponse
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpMessageStartlineParse (voipMgcpMessage_t *dest, const L7_char8 *buf,
                                const L7_uchar8 **headers)
{

  if ((buf[0] >= '0') && (buf[0] <= '9'))
    return voipMgcpMessageStartlineParseResp(dest, buf, headers);
  else
    return voipMgcpMessageStartlineParseReq (dest, buf, headers);
}
/*********************************************************************
* @purpose Process the header options received in Packet
*
* @param   voipMgcpMessage_t *mgcp  Pointer to MGCP message
* @param   L7_uchar8 *hname         Header name
* @param   const L7_uchar8 *hvalue  Header value
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpMessageSetHeader (voipMgcpMessage_t *mgcp, L7_char8 *hname, const L7_char8 *hvalue)
{
  L7_uint32 hvalue_len;

  if (hvalue == L7_NULLPTR)
  {
      return L7_SUCCESS;
  }
  hvalue_len = strlen (hvalue);
  if (hname[0] == 'C')
  {
    /* save call id */
    mgcp->callId = (L7_uchar8 *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,hvalue_len+1);
    voipStrncpy (mgcp->callId, hvalue, hvalue_len);
  }
  else if (hname[0] == 'I')
  {
    /* save connection id */
    mgcp->connectionId =(L7_uchar8 *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,hvalue_len+1);
    voipStrncpy (mgcp->connectionId, hvalue, hvalue_len);
  }
  else if (hname[0] == 'N')
  {
    /* save notified id */
    mgcp->notifiedId = (L7_uchar8 *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,hvalue_len+1);
    voipStrncpy (mgcp->notifiedId, hvalue, hvalue_len);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse MGCP header fields
*
* @param   voipMgcpMessage_t *mgcp  Pointer to MGCP message
* @param   const L7_uchar8 *start   start of header
* @param   const L7_uchar8 **body   Pointer to SIP message body
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  header = "header-name" HCOLON header-value *(COMMA header-value)
*                   CRLF
*         Header may consist of multiply header fields each sepearted by
*         CRLF we care for Call-id(C),Connection Id(I),Notified id(N)
*         To indicate end of header fields there should be atleast 1
*         empty line with CRLF at end
* @end
*********************************************************************/
static L7_RC_t
voipMgcpHeadersParse (voipMgcpMessage_t *mgcp, const L7_uchar8 *start,
                   const L7_uchar8 **body)
{
  const L7_uchar8 *colon_index;      /* index of ':' */
  L7_uchar8 *hname;
  L7_uchar8 *hvalue;
  const L7_uchar8 *end;
  L7_RC_t rc;

  for (;;)
  {
    if (start[0] == '\0')
    {
      *body = start;
      return L7_SUCCESS;
    }
    rc = voipFindNextCrlf (start, &end);
    if (rc == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "MGCP:voipMgcpHeadersParse no CRLF\n");
      return L7_FAILURE;            /* this is an error case!     */
    }
    /* For those with bodies the list of headers MUST always end with  */
    /* CRLFCRLF (also CRCR and LFLF are allowed) */
    if ((start[0] == '\r') || (start[0] == '\n'))
    {
      *body = start;
      return L7_SUCCESS; /* end of header found        */
    }

    /* find the header name */
    colon_index = strchr (start, ':');
    if (colon_index == L7_NULLPTR)
    {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
               "MGCP:voipMgcpHeadersParse no colon\n");
       return L7_FAILURE;            /* this is also an error case */
    }
    if (end <= colon_index)
    {
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
               "MGCP:voipMgcpHeadersParse message is incorrect\n");
       return L7_FAILURE;
    }
    hname = (L7_uchar8 *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,colon_index - start + 1);

    voipClrncpy (hname, start, colon_index - start);
    {
      const L7_uchar8 *endTemp;
      /* END of header is (end-2) if header separation is CRLF */
      /* END of header is (end-1) if header separation is CR or LF */
        if ((end[-2] == '\r') || (end[-2] == '\n'))
          endTemp = end - 2;
        else
          endTemp = end - 1;
        if ((endTemp) - colon_index < 2)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                  "should not happen, empty header %s!!!\n", hname);
          hvalue = L7_NULLPTR;        /* some headers (subject) can be empty */
        }
        else
        {
          hvalue = (L7_uchar8 *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,(endTemp) - colon_index + 1);
          /* one more space after colon? */
          voipClrncpy (hvalue, colon_index + 2, (endTemp) - colon_index - 2);
        }
      }

      /* hvalue MAY contains multiple value. In this case, they   */
      /* are separated by commas. But, a comma may be part of a   */
      /* quoted-string ("here, and there" is an example where the */
      /* comma is not a separator!) */
      rc = voipMgcpMessageSetHeader (mgcp, hname, hvalue);
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,hname);
      if (hvalue != L7_NULLPTR)
        osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,hvalue);
      /* continue on the next header */
      start = end;
    }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose Parse MGCP packet and extract SDP message
*
* @param   const L7_uchar8  *ip       Pointer to IpAddress
* @param   voipMgcpMessage_t *mgcp    Pointer to MGCP Message
* @param   voipSdpMessage_t  *sdp     Pointer to SDP message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipMgcpMessageParseSdp(const L7_uchar8 *ip, voipMgcpMessage_t *mgcp,
                        voipSdpMessage_t *sdp)
{
  L7_RC_t rv;
  voipMgcpCa_t   *caentry = L7_NULLPTR;
  voipMgcpCall_t  *callentry = L7_NULLPTR;

  if (MGCPMSG_IS_REQUEST(mgcp))
  {
    if (MGCPMSG_IS_CRCX(mgcp))
    {
      /*VOIP_DBG(("#############CRCX...\n"));*/
      rv = voipMgcpCaInsert(&voipMgcpCaList, ip , &caentry);
      if (rv != L7_SUCCESS)
      {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                 "voipMgcpCaInsert Failed\n");
         return L7_FAILURE;
      }
      if (mgcp->callId == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "voipMgcpMessageParseSdp : mgcp->call_id = NULL...\n");
        return L7_FAILURE;
      }
      if ((rv = voipMgcpCallLookup(caentry, mgcp->callId, &callentry)) == L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                  "voipMgcpCallLookupCallId: Entry already exist...\n");
          return L7_FAILURE;
      }
      rv = voipMgcpCallInsert(caentry, ip+4, mgcp, &callentry);
      if(rv == L7_FAILURE)
        return rv;
      else
      /* 1. Don't deal with SDP of CRCX. */
      /* 2. Don't free MGCP message. */
        return L7_SUCCESS;
    }
    else if (MGCPMSG_IS_DLCX(mgcp))
    {
      /*VOIP_DBG(("#############DLCX...\n"));*/
      rv = voipMgcpCaLookup(&voipMgcpCaList, ip , &caentry);
      if (rv == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "DLCX received, but CA not found!\n");
        return L7_FAILURE;
      }
      /* Some DLCX don't have call id? */
      if (mgcp->callId == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "DLCX received, but without call id!\n");
        return L7_FAILURE;
      }
      rv = voipMgcpCallLookup(caentry, mgcp->callId , &callentry);
      if (rv == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "DLCX received, but call id not found!\n");
        return L7_FAILURE;
      }
      rv = voipMgcpCallRemove(caentry, callentry);
      if (rv == L7_SUCCESS)
      {
        rv = voipSetupMgcpSession(&(callentry->db), L7_DISABLE);
        if (rv != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                  "MGCP: voipSetupMGCPSession(disable) failed\n");
        }
        voipMgcpMessageFree(callentry->msg);
        osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,callentry);
      }
      if (caentry->count == 0)
      {
        voipMgcpCaRemove(&voipMgcpCaList, caentry);
      }
      return L7_FAILURE;
    }
  }
  else if (MGCPMSG_IS_STATUS_200(mgcp))
  {
    L7_uint32 flag = 0;
    rv = voipMgcpCaLookup(&voipMgcpCaList, ip, &caentry);
    if (rv == L7_FAILURE)
    {
      rv = voipMgcpCaLookup(&voipMgcpCaList, ip+4, &caentry);
      if (rv == L7_FAILURE)
      {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                 "OK received, but CA not found!\n");
         return L7_FAILURE;
      }
      flag = 1;
    }
    else
    {
       flag = 0;
    }
    if (flag == 0)
    {
      /* don't care OK from CA to GW */
      return L7_FAILURE;
    }
    else
    {
      rv = voipMgcpCallLookupTranId(caentry, ip, mgcp->tranId, &callentry);
      if (rv == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "call found, call id = %s\n", callentry->msg->callId);
        if (mgcp->connectionId == L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                  "!!!OK without connection id!!!\n");
          return L7_FAILURE;
        }
        /* TODO:
         *   case 1. this may be OK for MDCX, have to check if port changed.
         *   case 2. two connection ids with the same call id, have to update
         *           src and dst IP/Port.
        */
        if (callentry->msg->connectionId == L7_NULLPTR)
        {
          /* save connection id */
          if (mgcp->connectionId == L7_NULLPTR)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                    "OK of CRCX received, but without connection id inside!!!!\n");
            return L7_FAILURE;
          }
          callentry->msg->connectionId = (L7_uchar8 *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
                                          strlen(mgcp->connectionId)+1);
          if(callentry->msg->connectionId == L7_NULLPTR)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                    "Fail to allocate memory\n");
            return L7_FAILURE;
          }
          strcpy(callentry->msg->connectionId,mgcp->connectionId);
          if (sdp!=L7_NULLPTR)
          {
            rv = voipMgcpCallUpdateSdp(&(callentry->db), sdp, 1);
            if (rv == L7_FAILURE)
            {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                      "voipMgcpCallUpdateSdp retured failured\n");
              return rv;
            }
            rv = voipSetupMgcpSession(&(callentry->db), L7_ENABLE);
            /* check if we run out of entries */
            if (rv == L7_FAILURE)
            {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                      "MGCP: Setup_MGCP_Session(enable) failed\n");
              rv = voipMgcpCallRemove(caentry, callentry);
              if (rv == L7_SUCCESS)
              {
                 voipMgcpMessageFree(callentry->msg);
                 osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,callentry);
              }
              if (caentry->count == 0)
              {
                voipMgcpCaRemove(&voipMgcpCaList, caentry);
              }
            }
          }
          else
          {
             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                     "OK of CRCX received, but NO SDP inside!!!!\n");
          }
        } /* if (callentry->msg->connection_id == NULL) */
      }
    } /* if flag == 1 */
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose Parse MGCP message
*
* @param   const L7_uchar8 *ipstr    Pointer to IpAddress(Source IP)
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipMgcpMessageParse (const L7_uchar8 *ip, const L7_uchar8 *packetBuf,
                              L7_uint32 length)
{
  L7_RC_t rc;
  const L7_uchar8 *next;
  L7_uchar8 *beg, *tmp, *start;
  voipMgcpMessage_t *mgcp;
  voipSdpMessage_t *sdp=L7_NULLPTR;
  L7_uint32 offset =0;

  #if 0
  tmp =  (L7_uchar8 *) osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,length);
  if (tmp == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "Could not allocate memory.\n");
    return L7_FAILURE;
  }
  #endif
  offset = sysNetDataOffsetGet((L7_uchar8 *)packetBuf);
  beg = (L7_uchar8 *)packetBuf;
  tmp = (L7_uchar8 *)(packetBuf+ offset + 28);
  #if 0
  memset(tmp,0x00,length);
  memcpy (tmp, buf, length); /* may contain binary data */
  /*tmp[length-4] = '\0';*/
  #endif

  voipMgcpMessageInit(&mgcp);
  /* parse request or status line */
  rc = voipMgcpMessageStartlineParse (mgcp, tmp, &next);
  if (rc == L7_FAILURE)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,beg);
    beg = L7_NULLPTR;
    voipMgcpMessageFree(mgcp);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpMessageStartlineParse Failed\n");
    return L7_FAILURE;
  }
  tmp = (L7_char8 *) next;

  if (tmp[0] == '\0')
  {
    /* Request or Response with no headers */
    goto mgcp_parse;
  }
  /* parse headers */
  rc = voipMgcpHeadersParse (mgcp, tmp, &next);
  if (rc == L7_FAILURE)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,beg);
    beg = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipMgcpHeadersParse Failed\n");
    voipMgcpMessageFree(mgcp);
    return L7_FAILURE;
  }
  tmp = (L7_uchar8 *) next;
  if (tmp[0] == '\0')
  {
    goto mgcp_parse;
  }
  /* start parsing SDP, get rid of the first CRLF */
  if ('\r' == tmp[0])
  {
    if ('\n' == tmp[1])
      start = tmp + 2;
    else
      start = tmp + 1;
  }
  else if ('\n' == tmp[0])
  {
     start = tmp + 1;
  }
  else
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,beg);
    beg = L7_NULLPTR;
    voipMgcpMessageFree(mgcp);
    return L7_FAILURE; /* message does not end with CRLFCRLF, CRCR or LFLF */
  }
  /* MGCP don't have content length as SIP does, so deal with sdp here ... */
  voipSdpMessageInit(&sdp);
  if(voipSdpMessageParse(sdp,start)!= L7_SUCCESS)
  {
    osapiFree (L7_FLEX_QOS_VOIP_COMPONENT_ID,beg);
    beg = L7_NULLPTR;
    voipMgcpMessageFree(mgcp);
    voipSdpMessageFree(sdp);
    sdp = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "MGCP: voipSdpMessageParse failed\n");
    return L7_FAILURE;
  }
mgcp_parse:
  rc = voipMgcpMessageParseSdp(ip, mgcp, sdp);
  if(rc!= L7_SUCCESS)
  {
    voipMgcpMessageFree(mgcp);
  }
  if (sdp != L7_NULLPTR)
  {
    voipSdpMessageFree(sdp);
  }
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,beg);
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
L7_RC_t voipMgcpClearInterfaceEntries()
{
 return L7_SUCCESS;
}

#if 0
/********************* DEBUG FUNCTIONS **************************/
void voipMgcpDebug(L7_uint32 interface)
{
  voipMgcpCa_t *temp = voipMgcpCaList[interface];
  voipMgcpCall_t *call = L7_NULLPTR;
  L7_uint32 i;

  while(temp!=L7_NULLPTR)
  {
    printf("Couunt %d\n",temp->count);
    call = temp->call;
    for(i=0;i<temp->count;i++)
    {
       printf("tranId = %s\n",call->msg->tranId);
       printf("callId = %s\n",call->msg->callId);
       printf("connectionId = %s\n",call->msg->connectionId);
       printf("notifiedId = %s\n",call->msg->notifiedId);

       printf("db.src_ip %u\n",call->db.src_ip);
       printf("db.dst_ip %u\n",call->db.dst_ip);
       printf("db.l4src =%u\n",call->db.l4_src_port);
       printf("db.l4dest = %u\n",call->db.l4_dst_port);

       call = call->next;
    }
    temp = temp->next;
  }
}

void voipMgcpCRCX()
{
char ip[8];
char *buf="CRCX 2879 aaln/S1/SU0/1@dc2001.ctl0.test.com MGCP 0.1\
N: mgcp.TEsT.atl0.test.com:2525\
C: 64\
M: recvonly";
ip[0]='1';
ip[1]='1';
ip[2]='1';
ip[3]='1';
ip[4]='2';
ip[5]='2';
ip[6]='2';
ip[7]='2';

voipMgcpMessageParse(ip,buf,strlen(buf),2);
}
void voipMgcpResp()
{
char ip[8];

char *buf="200 2879 OK\
I: 18\
v=0\
c=IN IP4 1.1.1.1\
m=audio 49170 RTP/AVP 32\
m=video 51372 RTP/AVP 101\
a=rtpmap:99 h263-1998/90000";

ip[0]='2';
ip[1]='2';
ip[2]='2';
ip[3]='2';
ip[4]='1';
ip[5]='1';
ip[6]='1';
ip[7]='1';

voipMgcpMessageParse(ip,buf,strlen(buf),2);

}
void voipMgcpResp1()
{
char ip[8];

char *buf="200 2879 OK\
I: 18\
v=0\
c=IN IP4 1.1.1.1\
m=audio 49170 RTP/AVP 32\
m=video 51372 RTP/AVP 101\
a=rtpmap:99 h263-1998/90000";

ip[0]='1';
ip[1]='1';
ip[2]='1';
ip[3]='1';
ip[4]='2';
ip[5]='2';
ip[6]='2';
ip[7]='2';

voipMgcpMessageParse(ip,buf,strlen(buf),2);

}

void voipMgcpDLCX()
{
char ip[8];
char *buf="DLCX 2879 aaln/S1/SU0/1@dc2001.ctl0.test.com MGCP 0.1\
I: 18\
C: 64";
ip[0]='1';
ip[1]='1';
ip[2]='1';
ip[3]='1';
ip[4]='2';
ip[5]='2';
ip[6]='2';
ip[7]='2';

voipMgcpMessageParse(ip,buf,strlen(buf),2);
}
#endif
