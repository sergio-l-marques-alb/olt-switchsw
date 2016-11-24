/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sccp.c
*
* @purpose SCCP message parsing routines
*
* @component VOIP |SCCP (Skinny Call control Protocol)
*
* @comments none
*
* @create 05/12/2007
*
* @author pmurali 
* @end
*
**********************************************************************/
#include <stdio.h>
#include "voip.h"
#include "voip_exports.h"
#include "voip_sccp.h"
#include "voip_debug.h"
#include "osapi_support.h"
#include "voip_control.h"
#include "dtl_voip.h"
#include "usmdb_util_api.h"
static voipSccpCms_t *voipSccpCmsList=L7_NULLPTR; 

/*********************************************************************
* @purpose Look up for a SCCP cms
*
* @param   voipSccpCms_t   **list    List of SCCP cms
* @param   const L7_uchar8 *ip       Ip to be searched
* @param   voipSccpCms_t   **entry    SCCP Cms entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCmsLookup (voipSccpCms_t **list,const L7_uchar8 *callManager, voipSccpCms_t **entry)
{
  while (*list != L7_NULLPTR) 
  {
    if (strcmp((*list)->callManager, callManager) == 0) 
    {
       *entry = *list;
       return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Look up for a SCCP call
*
* @param   voipSccpCms_t   *cm     List of SCCP cms
* @param   L7_uint32       id       Id to be searched
* @param   voipSccpCall_t  **entry   SCCP Call entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCallLookup(voipSccpCms_t *cm,L7_uint32 id,voipSccpCall_t **entry)
{
  voipSccpCall_t *list = cm->calls;
  while(list!=L7_NULLPTR)
  {
    if(list->conferenceId == id)
    {
      *entry = list;
      return L7_SUCCESS;
    }
    list = list->next;
  }
  return L7_FAILURE;  
}
/*********************************************************************
* @purpose Insert a new SCCP cms entry
*
* @param   voipSccpCms_t   **list   List of SCCP cms
* @param   const L7_uchar8 *ip      Ip to be inserted
* @param   voipSccpCms_t   **ins    Pointer to inserted  entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED(if entry
*           already exists )
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCmsInsert(voipSccpCms_t **list,const L7_uchar8 *callManager, voipSccpCms_t **ins)
{
  voipSccpCms_t   *entry = L7_NULLPTR;
  L7_RC_t rv;

  if ((rv = voipSccpCmsLookup(list, callManager, &entry)) == L7_SUCCESS) 
  {
      *ins = entry;
      return L7_ALREADY_CONFIGURED;
  }
  if ((entry = (voipSccpCms_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
     sizeof(voipSccpCms_t))) == L7_NULLPTR) 
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SCCP,"voipSccpCmsInsert could not allocate memory\n");
    return L7_FAILURE;
  }
  memset(entry, 0x00, sizeof(voipSccpCms_t));
  /* set ip */
  osapiStrncpy(entry->callManager,callManager,sizeof(entry->callManager));
  entry->next = *list;
  *list = entry;
  *ins = entry;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Insert a new SCCP call entry
*
* @param   voipSccpCms_t   *cm   List of SCCP cms
* @param   voipSccpMoo_t    *m   Pointer to voipSccpMoo_t
* @param   voipSccpCall_t  **ins Pointer to inserted entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED(if entry
*           already exists )
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCallInsert(voipSccpCms_t *cm,voipOperSccp_t m,voipSccpCall_t **ins)
{
  voipSccpCall_t *entry = L7_NULLPTR;
  L7_RC_t rv;

  if ((rv = voipSccpCallLookup(
             cm,
             m.lel_conferenceId,
             &entry)) == L7_SUCCESS)
  {
    *ins = entry;
    return L7_ALREADY_CONFIGURED;
  }
  if ((entry = (voipSccpCall_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,
       sizeof(voipSccpCall_t))) == L7_NULLPTR) 
  {
    LOG_MSG("voipSccpCallInsert Could not allocate memory\n");
    return L7_FAILURE;
  }
  
  memset(entry, 0x00, sizeof(voipSccpCall_t));
  entry->remoteIP = m.bel_remoteIpAddr;
  entry->remotePort =  osapiHtole32(m.lel_remotePortNumber);
  cm->count++;
  entry->next = cm->calls;
  cm->calls = entry;
  *ins = entry;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Remove SCCP cms entry
*
* @param   voipSccpCms_t   **list  List of SCCP Cms
* @param   voipSccpCms_t   *cm     entry to be deleted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCmsRemove(voipSccpCms_t **list,voipSccpCms_t *cm)
{
  voipSccpCms_t *entry;
  
  while (*list != L7_NULLPTR) 
  {
    if ((*list) == cm) 
    {
      entry = *list;
      *list = entry->next;
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,cm);
      return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Remove SCCP call entry
*
* @param   voipSccpCms_t   **cm  List of SCCP Cms
* @param   voipSccpCall_t   *del     entry to be deleted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSccpCallRemove(voipSccpCms_t **cm,voipSccpCall_t *del)
{
  L7_char8  strRemoteIPaddr[L7_CLI_MAX_STRING_LENGTH];
  voipSccpCall_t *entry = (*cm)->calls;
  voipSccpCall_t *prev  = (*cm)->calls;
  while (entry != L7_NULLPTR) 
  {
    if (entry == del) 
    {
      /* the first node */
      if (prev == entry) 
      {
        (*cm)->calls = entry->next;
      } 
      else 
      {
        prev->next = entry->next;
      }
      osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,del);
      (*cm)->count--;
      if ((*cm)->count == 0)
      {
        /* Log the event */
        usmDbInetNtoa((*cm)->calls->remoteIP, strRemoteIPaddr);

        LOG_MSG("Established new SCCP call. Call Manager IP = %s\r\n ConferenceId = %d\r\n"
                 "Remote IP = %s, Remote Port = %d\r\n", 
                 (*cm)->callManager, (*cm)->calls->conferenceId,
                 strRemoteIPaddr, (*cm)->calls->remotePort);

        voipSccpCmsRemove(&voipSccpCmsList, *cm);
      }
      return L7_SUCCESS;
    }
    prev = entry;
    entry = entry->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Enable/disable a SCCP session
*
* @param   voipSccpCall_t *t        Pointer to voipSccpCall_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t voipSetupSccpSession(voipSccpCall_t *t, L7_BOOL callStatus)
{
  L7_uint32   srcIp;
  L7_uint32   dstIp;
  L7_ushort16 srcL4port;
  L7_ushort16 dstL4port;
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8  strRemoteIPaddr[L7_CLI_MAX_STRING_LENGTH];

  /*For SCCP the values for both source and destination are same */

  srcIp = t->remoteIP;
  dstIp = t->remoteIP;
  srcL4port = t->remotePort;
  dstL4port = t->remotePort;

  rc = dtlVoipProfileParamSet(callStatus,
                              L7_QOS_VOIP_PROTOCOL_SCCP,
                              srcIp,
                              dstIp,
                              srcL4port,
                              dstL4port); 

  if (rc == L7_SUCCESS)
  {
    if (callStatus == L7_ENABLE)
      voipTimerAdd();
    else if(callStatus == L7_DISABLE)
     voipTimerDelete();

    /* Log the event */
    usmDbInetNtoa(t->remoteIP, strRemoteIPaddr);

    LOG_MSG("Established new SCCP call. ConferenceId = %d\r\n"
             "Remote IP = %s, Remote Port = %d\r\n", 
             t->conferenceId,strRemoteIPaddr, t->remotePort);

  } 
  return rc;
}

/*********************************************************************
* @purpose Parse SCCP message
*
* @param   const L7_uchar8 *buf      Pointer to data packet
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSccpMessageParse (const L7_uchar8 *packetBuf, L7_uint32 length)
{
  L7_RC_t            rv;
  L7_uint32          offset=0;
  voipOperSccp_t     callInfo;
  voipSccpCms_t     *cmentry;
  voipSccpCall_t    *callentry = L7_NULLPTR;
  L7_ipHdr_t        *iph;
  L7_tcpHdr_t       *tcph;
  L7_uint32          iphdrlen, tcpdataoff, tcpdatalen;
  L7_uchar8         *tmp;

  offset = sysNetDataOffsetGet((L7_uchar8 *)packetBuf);
  tmp = (L7_uchar8 *)(packetBuf + 18);

  offset=0  ;

  /* Get TCP data offset = IP header len + TCP header len */
  iph = (L7_ipHdr_t *)tmp;
  iphdrlen = (osapiNtohs(iph->_v_hl_tos) >> 8) & 0x0f;
  tcph = (L7_tcpHdr_t *)(tmp+iphdrlen*4);
  tcpdataoff = iphdrlen +  (osapiNtohs(tcph->_len) >> 12);
  /* Since tcp len is number of 32 bit word */
  tcpdataoff <<= 2;

  /* Get TCP data length */
  tcpdatalen =  osapiNtohs(iph->_len) - tcpdataoff;
  if (tcpdatalen <= 0) /* No TCP data */
  {
     return L7_FAILURE;
  }
  offset = tcpdataoff;
  if (tcpdatalen > 4) 
  {
    /* sccp data */
    tmp = (L7_uchar8 *)(tmp+offset);

    VOIP_GET_LONG(callInfo.length,tmp); 
    VOIP_GET_LONG(callInfo.lel_messageId,tmp+8); 
    VOIP_GET_LONG(callInfo.lel_conferenceId,tmp+12);    

   /* all the fileds in SCCP packet are in little endian 
    * except remote IP address
    */

    callInfo.length = osapiHtole32(callInfo.length);
    callInfo.lel_messageId = osapiHtole32(callInfo.lel_messageId);

    if ((callInfo.length == 0) || (callInfo.length == 4)) 
    {

       VOIP_TRACE(VOIP_DEBUG_ERROR, VOIP_PROTO_SCCP, "Received pkt is too small %d",
                                      callInfo.length);
       return L7_FAILURE;
    }

    /* Drop all types of SCCP packets except StartMediaTransmission and StopMediaTransmission
     */
    if (callInfo.lel_messageId == StartMediaTransmission) 
    {
      /* Trace with info Packet type */
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SCCP, "Packet type StartMediaTransmission\r\n");

      VOIP_GET_LONG(callInfo.bel_remoteIpAddr,tmp+20); 
      VOIP_GET_LONG(callInfo.lel_remotePortNumber,tmp+24); 

      rv = voipSccpCmsInsert(&voipSccpCmsList, callInfo.callManager, &cmentry);
      if (rv != L7_SUCCESS) 
      {
        return L7_FAILURE;
      }
      rv = voipSccpCallInsert(cmentry, callInfo, &callentry);
      if (rv != L7_SUCCESS) 
      {
        return L7_FAILURE; 
      }
       
      rv = voipSetupSccpSession(callentry, L7_ENABLE);
      if (rv != L7_SUCCESS) 
      {
        voipSccpCallRemove(&cmentry, callentry);
        VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SCCP, "voipSetupSccpSession(enable) failed\n");
      }
    } 
    else if (callInfo.lel_messageId == StopMediaTransmission) 
    {
      /* Trace with info Packet type */
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SCCP, "Packet type StopMediaTransmission\r\n");

      rv = voipSccpCmsLookup(&voipSccpCmsList,callInfo.callManager, &cmentry);
      if (rv == L7_FAILURE) 
      {
         return L7_FAILURE;
      }
      rv = voipSccpCallLookup(cmentry, callInfo.lel_conferenceId, &callentry);
      if (rv == L7_FAILURE) 
      {
         return L7_FAILURE;
      }
      rv = voipSetupSccpSession(callentry, L7_DISABLE);
      if (rv!=L7_SUCCESS) 
      {
        VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_SCCP, "voipSetupSccpSession(disable) failed\n");
      }
      voipSccpCallRemove(&cmentry, callentry);
    
    }
    else 
    {
      /* Trace with Reason to drop*/
      VOIP_TRACE(VOIP_DEBUG_INFO, VOIP_PROTO_SCCP, "Received pkt is not required");
      return L7_FAILURE;
    }
  } /* if ( tcpdatalen > 4) */
  else 
  {
    return L7_FAILURE;
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
L7_RC_t voipSccpClearInterfaceEntries()
{
  voipSccpCms_t *cms = voipSccpCmsList;
  voipSccpCall_t *entry = NULL;

  if (cms != L7_NULLPTR)
  entry = cms->calls;
  while(cms != L7_NULLPTR)
  {
    while(entry != L7_NULLPTR)
    {
      voipSccpCallRemove(&cms,entry);
      if(voipSccpCmsList!=L7_NULLPTR)
      {
        cms = voipSccpCmsList;
        entry = voipSccpCmsList->calls;
      }
      else
      {
        return L7_SUCCESS;
      }
    }
    cms = voipSccpCmsList;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  vvoipSccpTimerAction
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Get the total number of packets belongs to the each call and
*           compare with previous count.End the call if the previoues
*           and current count is equal
*
* @end
*********************************************************************/

L7_RC_t voipSccpTimerAction()
{
  voipSccpCms_t *cms= voipSccpCmsList;
  voipSccpCall_t *entry=L7_NULLPTR;
  L7_ulong64     counter;

  if (cms != L7_NULLPTR)
   entry = cms->calls;

  while(cms!=L7_NULLPTR)
  {
    while(entry != L7_NULLPTR)
    {
      if (dtlVoipSessionStatsGet(entry->remoteIP,entry->remoteIP,
                                 entry->remotePort,entry->remotePort,
                                 &counter) == L7_SUCCESS)
      {
        if((entry->prev_counter.high == counter.high) &&
           (entry->prev_counter.low == counter.low))
        {
          if (voipSetupSccpSession(entry,L7_DISABLE) == L7_SUCCESS)
          {       
            voipSccpCallRemove(&cms, entry);
            if(voipSccpCmsList!=L7_NULLPTR)
            {
              cms = voipSccpCmsList;
              entry = voipSccpCmsList->calls;
            }
            else
            {
              return L7_SUCCESS;
            }    
          } 
        }
      } 
    }
    cms = voipSccpCmsList;
  } 
  return L7_SUCCESS;
}
/************************ DEBUG FUNCTION ****************************/
void voipSccpCallDump()
{
  voipSccpCms_t *cms= voipSccpCmsList;
  voipSccpCall_t *entry=L7_NULLPTR;

  if (cms != L7_NULLPTR)
   entry = cms->calls;

  printf("\r\n SCCP call database info");
  printf("\r\n -----------------------"); 
  while(cms!=L7_NULLPTR)
  {
    printf("\r\n call manager ipaddress=%s",cms->callManager);
    while(entry != L7_NULLPTR)
    {
      printf("\r\n Remote ipaddress =0x%x",entry->remoteIP);
      printf("\r\n Remote port =0x%x",entry->remotePort);
      printf("\r\n call conferenceId =%d",entry->conferenceId);
      entry = entry->next;
    }
    cms = cms->next;
  }
  printf("\r\n -------End------------");
}
   
