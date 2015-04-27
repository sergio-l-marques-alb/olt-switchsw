/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp_prot.c
*
* @purpose dhcp main implementation
*
* @component bootp
*
* @comments none
*
* @create 6/26/2001
*
* @author paresh
*
* @end
*             
**********************************************************************/

/*==========================================================================
  
        dhcp_prot.c
  
        DHCP protocol implementation for DHCP client
  
  ==========================================================================
  ####COPYRIGHTBEGIN####
                                                                            
   -------------------------------------------                              
   The contents of this file are subject to the Red Hat eCos Public License 
   Version 1.1 (the "License"); you may not use this file except in         
   compliance with the License.  You may obtain a copy of the License at    
   http:  www.redhat.com/                                                   
                                                                            
   Software distributed under the License is distributed on an "AS IS"      
   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
   License for the specific language governing rights and limitations under 
   the License.                                                             
                                                                            
   The Original Code is eCos - Embedded Configurable Operating System,      
   released September 30, 1998.                                             
                                                                            
   The Initial Developer of the Original Code is Red Hat.                   
   Portions created by Red Hat are                                          
   Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
   All Rights Reserved.                                                     
   -------------------------------------------                              
                                                                            
  ####COPYRIGHTEND####
  ==========================================================================
  #####DESCRIPTIONBEGIN####
  
   Author(s):   hmt
   Contributors: gthomas
   Date:        2000-07-01
   Purpose:     DHCP support
   Description: 
  
  ####DESCRIPTIONEND####
  
  ========================================================================*/

#include "l7_common.h"
#include "dtlapi.h"
#include "datatypes.h"
#include "default_cnfgr.h"

#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "sysapi.h"
#include "l7netapi.h"

#include "l7_socket.h"
/* this needs to get rolled into bsdsock.h */
#define ipcom_memset memset

#include "l7_dhcp.h"
#include "dhcp_debug.h"
#include "l7_ip_api.h"

/* cant include dtlapi.h without getting socket.h */
#define L7_DTL_PORT_IF "dtl"

#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/types.h>
#include "bspapi.h"
#include "dns_client_api.h"

#ifdef _L7_OS_ECOS_
#include <sys/ioctl.h> 
#include <cyg/infra/diag.h>
#include <network.h>
#endif

L7_uint32 errNo;
L7_char8 *pErrString;

L7_uint32 dhcpcDebugMsgLevel = 0;

L7_uint32 dhcpcDebugMsgLvlSet(L7_uint32 level);

#ifdef FEAT_METRO_CPE_V1_0
L7_ulong32 bootpDhcpServerAddress = 0;
#endif

/**************************************************************************
* @purpose  To get the actual packet size of an initialized buffer
*
* @param    bootp   DHCP structure
* 
*
* @returns  L7_int32 Size of the packet
*
* @comments None.
*
* @end
*************************************************************************/
static L7_int32 dhcp_size( struct bootp *ppkt )
{
  L7_uchar8 *op;
  L7_int32 size;

  op = &ppkt->bp_vend[0];       /* First check for the cookie!*/
  if ( op[0] !=  99 ||
       op[1] != 130 ||
       op[2] !=  83 ||
       op[3] !=  99 )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
            "dhcp_size():Bad DHCP cookie" );
    return 0;
  }
  op += 4;
  while ( *op != TAG_END )
  {
    op += *(op+1)+2;
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "dhcp_size(): Oversize DHCP packet in dhcp_size" );
      return 0;
    }
  }
  size=  op - (L7_uchar8 *)ppkt + 1;
  return size;
}

/**************************************************************************
* @purpose  To remove a bootp option value if it is present from an 
*           initialized buffer
*
* @param     bootp      DHCP structure
* @param     L7_uchar8    Option identifier
* @param     L7_uint32      Option Value
* @param     L7_int32       Option Value Length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
static L7_int32
remove_tag( struct bootp *ppkt,
               L7_uchar8 tag)
{
  L7_uchar8 *op, *begin, *end;
  L7_uint32 len;

  /* Initially this will only scan the options field.*/

  op = &ppkt->bp_vend[4];

  /*searching for option by looking at all the option tag values*/
  while ( *op != TAG_END )
  {
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
      return L7_FAILURE; 
    }
    if ( *op == tag )
      break;
    op += *(op+1)+2;
  }

  /*option was found, so remove it*/
  if ( *op == tag )
  {
    begin = op;
    /*move pointer past the tag to be removed*/
    op += *(op+1)+2;
    end = op; /*begin and end indicate the start and end of option to be removed*/

    while ( *op != TAG_END )
    {
        if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                  "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
          return L7_FAILURE; 
        }
        op += *(op+1)+2;
    }
    len = op - end + 1;        /*extra 1 to take into account the TAG_END byte*/
    memmove(begin, end, len); 
    return L7_SUCCESS;
  }

  /*option was not present in the first place*/
  return L7_SUCCESS;
}


/**************************************************************************
* @purpose  To extract a bootp option value if it is present from an 
*           initialized buffer
*
* @param     bootp   @b((input))  DHCP structure
* @param     tag     @b((input))  TAG identifier
* @param     option  @b((output)) Option Pointer to copy the extracted option
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
static L7_RC_t
get_tag( struct bootp *ppkt, L7_uchar8 tag)
{
  L7_uchar8 *op, *begin = L7_NULLPTR, *pOption;
  L7_uint32 len = 0;
  dhcpVendorSpecificOption_t venSpecOption;
  L7_bootp_dhcp_t  notifyInfo;

  /* Initially this will only scan the options field.*/

  op = &ppkt->bp_vend[4];

  /*searching for option by looking at all the option tag values*/
  while ( *op != TAG_END )
  {
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "%s :Oversize DHCP packet", __FUNCTION__);
      return L7_FAILURE; 
    }
    if ( *op == tag )
      break;
    op += *(op+1)+2;
  }

  if (*op == TAG_END)
  {
    /* The requested option tag is not found.*/
    return L7_FAILURE;
  }
  /*option was found, so remove it*/
  if ( *op == tag )
  {
    begin = op;
    len = *(begin +1);
    while ( *op != TAG_END )
    {
      if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                "%s:Oversize DHCP packet", __FUNCTION__ );
        return L7_FAILURE; 
      }
      op += *(op+1)+2;
    }
  }

  switch(tag)
  {
    case TAG_VEND_SPECIFIC:
    {
      dhcpVendorSpecificOption_t *vendSpecificOpt = &dhcpClientCB.dhcpOptions.vendorSpecificOption;
      L7_uchar8  type, index = 0;

      memset(vendSpecificOpt, 0, sizeof(dhcpVendorSpecificOption_t));
      pOption = begin + 2; /* Get content offset.*/
      while ((pOption < (begin + len)) && (index < DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM))
      {
        type = *pOption;
        if (type > DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                  "%s: Unsupported subOption(%d) in VendorSpecific Option in received DHCP pkt ",
                  __FUNCTION__, *pOption);
          return L7_FAILURE;
        }
        vendSpecificOpt->subOptions[index].type = *pOption;
        vendSpecificOpt->subOptions[index].len = *(pOption +1);
        memcpy(&(vendSpecificOpt->subOptions[index].option),
               (pOption + 2),
               vendSpecificOpt->subOptions[index].len);
        pOption += *(pOption+1)+2;
        vendSpecificOpt->numSubOptions++;
        index++;
      }
      memcpy(&venSpecOption, vendSpecificOpt, sizeof(dhcpVendorSpecificOption_t));
    }
    /* Notify the registered components for this Option-43.*/
    memset(&notifyInfo, 0, sizeof(notifyInfo));
    notifyInfo.vendSpecificOptions = &venSpecOption;
    notifyInfo.offeredOptionsMask |= BOOTP_DHCP_OPTION_VENDOR_SPECIFIC;

    dhcpNotifyRegisteredUsers(&notifyInfo);
    break;

    default:
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
            "%s: Unsupported DHCP option requested for extraction.");
  }

  /*option was not present in the first place*/
  return L7_FAILURE;
}

/* ------------------------------------------------------------------------
   Insert/set an option value in an initialized buffer*/


/**************************************************************************
* @purpose  To insert/set a bootp option valure in initialized buffer
*
* @param     bootp      DHCP structure
* @param     L7_uchar8    Option identifier
* @param     L7_uint32      Option Value
* @param     L7_int32       Option Valure Length
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
static L7_int32
set_fixed_tag( struct bootp *ppkt,
               L7_uchar8 tag,
               L7_uint32 value,
               L7_int32 len)
{
  L7_uchar8 *op;

  /* Initially this will only scan the options field.*/

  op = &ppkt->bp_vend[4];
  while ( *op != TAG_END )
  {
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
      return L7_FALSE; 
    }
    if ( *op == tag )
      break;
    op += *(op+1)+2;
  }

  if ( *op == tag )
  {
    if ( *(op+1) != len )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "set_fixed_tag():Wrong size in set_fixed_tag" );
      return L7_FALSE;          
    }
  }
  else
  { /* overwrite the end tag and install a new one*/
    if ( op + len + 2 > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "set_fixed_tag():Oversize DHCP packet in set_fixed_tag append" );
      return L7_FALSE; 
    }
    *op = tag;
    *(op+1) = len;
    *(op + len + 2) = TAG_END;
  }
  /* and insert the value.  Net order is BE.*/
  op += len + 2 - 1;              /* point to end of value*/
  while ( len-- > 0 )
  {
    *op-- = (L7_uchar8)(value & 255);
    value >>= 8;
  }
  return L7_TRUE;
}

/**************************************************************************
* @purpose  To add a bootp option value in initialized buffer
*
* @param     bootp        DHCP structure
* @param     L7_uchar8    Option identifier
* @param     L7_uchar8    Option Value
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
static L7_int32
add_fixed_tag( struct bootp *ppkt,
               L7_uchar8 addingTag,
               L7_uchar8 value)
{
  struct bp_option *tagEndPtr = L7_NULLPTR;
  struct bp_option *bp_TagPtr = L7_NULLPTR;

  /* Initially this will only scan the options field.*/
  tagEndPtr = bp_TagPtr = (struct bp_option*)&ppkt->bp_vend[4];

  /* first find the end of options list */
  while( tagEndPtr->tag != TAG_END )
  {
    /* check whether we are out of the options boundary */
    if ( (L7_uchar8*)tagEndPtr > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "add_fixed_tag():Oversize DHCP packet in add_fixed_tag (%d)", addingTag );
      return L7_FALSE;
    }
    tagEndPtr = (struct bp_option *)(&(tagEndPtr->data) + tagEndPtr->dataLen);  /* jump to next existing TAG */
  }

  /* find option to set. Continue while TAG_END isn't met */
  while ( bp_TagPtr->tag != TAG_END )  
  {
    /* check whether we are out of the options boundary */
    if ( (L7_uchar8*)bp_TagPtr > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
      return L7_FALSE;
    }
    if ( bp_TagPtr->tag == addingTag )
      break;
    bp_TagPtr = (struct bp_option *)(&(bp_TagPtr->data) + bp_TagPtr->dataLen);  /* jump to next existing TAG */
  }

  /* if option already exists */
  if ( bp_TagPtr->tag == addingTag )
  {
    if( bp_TagPtr->tag == TAG_DHCP_PARM_REQ_LIST )
    {
      L7_uint32 optCounter;
      
      /* if option value to add already exists in option fields do return */
      for(optCounter = 0; optCounter < bp_TagPtr->dataLen; optCounter++)
      {
        if( value == *( &(bp_TagPtr->data) + optCounter) )
        {
          return L7_TRUE;
        }
      }
      /* otherwise install new value in option fields. 
         Check whether adding new option value would cause oversize of packet */
      if( (L7_uchar8*)tagEndPtr + 1 > &ppkt->bp_vend[ BP_VEND_LEN-1] )
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
        return L7_FALSE;
      }
      /* make empty field for insertion by moving existing data fields and TAG_END field */
      memmove( &(bp_TagPtr->data) + 1, &(bp_TagPtr->data), (L7_uchar8*)tagEndPtr - (L7_uchar8*)bp_TagPtr - 1);
      /* increase length field */
      bp_TagPtr->dataLen += 1;
    }
  }
  /* if option doesn't already exist - insert a new option before the TAG_END */
  else
  {
    /* check if adding new option tag, length, value fields wouldn't cause oversize of packet */
    if( (L7_uchar8*)tagEndPtr + 3 > &ppkt->bp_vend[ BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
      return L7_FALSE;
    }
    bp_TagPtr->tag = addingTag;        /* set new tag */
    bp_TagPtr->dataLen = 1;            /* set data field length */
    *( &(bp_TagPtr->data) + 1 ) = TAG_END; /* overwrite the end tag */
  }
  /* and insert the value.  Net order is BE.*/
  bp_TagPtr->data = value;

  return L7_TRUE;
}
/**************************************************************************
* @purpose  To insert/set a bootp option valure in initialized buffer
*
* @param     ppkt       @b((input)) DHCP packet structure
* @param     tag        @b((input)) DHCP Option identifier
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
static
L7_BOOL set_variable_tag(struct bootp *ppkt, L7_uchar8 tag)
{
  L7_uchar8 *op;
  L7_uint32 optLen;

  /* Initially this will scan the options field to check if the 
     DHCP packet has already reached its maximum size.*/
  op = &(ppkt->bp_vend[4]);

  while ( *op != TAG_END )
  {
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "%s : Oversized DHCP packet", __FUNCTION__ );
      return L7_FALSE; 
    }
    if ( *op == tag )
      break;
    op += *(op+1)+2;
  }

  /* If tag is already found, check for its length.*/
  if ( *op == tag )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
            "%s : Tag already set with size(%d)", __FUNCTION__, *(op+1));
    return L7_FALSE;          
  }
  else
  { 
    switch (tag)
    {
    case TAG_DHCP_CLASSID:
      if (dhcpClientCB.dhcpOptions.vendorClassOption.optionMode == L7_ENABLE)
      {
        optLen = strlen(dhcpClientCB.dhcpOptions.vendorClassOption.vendorClassString);
        /* Check if the addition of the tag makes the packet beyond the max size.*/
        if ( (op + optLen + 2) > (&ppkt->bp_vend[BP_VEND_LEN-1]) )
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                  "%s :Oversize DHCP packet with option(%d) append of len(%d)",
                   __FUNCTION__, tag, optLen);
          return L7_FALSE; 
        }
        /* overwrite the end tag and install a new one*/
        *op = tag;
        *(op+1) = optLen;
        memcpy(op+2, dhcpClientCB.dhcpOptions.vendorClassOption.vendorClassString, optLen);
        *(op + optLen + 2) = TAG_END;
      }
      else
      {
        return L7_TRUE;
      }
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              " %s : Unsupported DHCP Option to set", __FUNCTION__);
      return L7_FALSE;
    }
  }
  return L7_TRUE;  
}

/**************************************************************************
* @purpose  Initialize the timer values
*
* @param     timeval        Ptr to timeval structure
* @param     timeout_state  Ptr to state variable
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
void
reset_timeout (dhcpClientInfo_t *clientIntfInfo)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  clientIntfInfo->timeout_random++;
  clientIntfInfo->timeout_scratch.countdown = 4; /* initial fast retries*/
  clientIntfInfo->timeout_scratch.secs = 4 + (clientIntfInfo->timeout_random & 3);
  clientIntfInfo->tv.tv_sec = 4;
  clientIntfInfo->tv.tv_usec =
                  65536 * (2 + (clientIntfInfo->timeout_random & 3));
  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Timeout Timer Reset to %d Secs, %d Microsecs",
               clientIntfInfo->tv.tv_sec, clientIntfInfo->tv.tv_usec);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}


/**************************************************************************
* @purpose   To  find out next timer value
*
* @param     timeval        Ptr to timeval structure
* @param     timeout_state  Ptr to state variable
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32
next_timeout (dhcpClientInfo_t *clientIntfInfo)
{
  L7_int32 nextTimeout = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if ( 0 > clientIntfInfo->timeout_scratch.countdown-- )
    return L7_TRUE;

  clientIntfInfo->timeout_random++;
  clientIntfInfo->timeout_scratch.secs =
    (clientIntfInfo->tv.tv_sec * 2) - 2 + (clientIntfInfo->timeout_random & 3);
  clientIntfInfo->timeout_scratch.countdown = 2; 
  clientIntfInfo->tv.tv_sec = clientIntfInfo->timeout_scratch.secs ;

  nextTimeout = clientIntfInfo->timeout_scratch.secs < 50; /* If longer, too many tries...*/

  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Next Timeout - %d", nextTimeout);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return nextTimeout;
}

/**************************************************************************
* @purpose  Timeout Function to resend Discover or Request
*
* @param    data      Internal Interface Number
* @param    arg2      Mgmt Interface Type
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void next_timeout_function (L7_uint32 data, L7_uint32 arg2)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 intIfNum = 0;
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  L7_uint32 nextTimeOut = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  intIfNum = data;
  mgmtPortType = (L7_MGMT_PORT_TYPE_t) arg2;

  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Timeout occured for for intIfNum-%d, mgmtPortType-%d",
               intIfNum, mgmtPortType);

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d, "
                 "mgmtPortType-%d", intIfNum, mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "DHCP Client is Not in USE on intIfNum - %d, "
                 "mgmtPortType - %d", intIfNum, mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return;
  }

  clientIntfInfo->nextTimeout = L7_NULLPTR;

  nextTimeOut = next_timeout (clientIntfInfo);
  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Next Timeout Value - %d", nextTimeOut);

  /* Calculate the Next Timeout */
  if (nextTimeOut == 0)
  {
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

    memset (buf, 0, sizeof(buf));
    if (mgmtPortType == L7_MGMT_SERVICEPORT)
    {
      osapiSnprintf (buf, sizeof(buf), "Service Port");
    }
    else if (mgmtPortType == L7_MGMT_NETWORKPORT)
    {
      osapiSnprintf (buf, sizeof(buf), "Network Port");
    }
    else if (mgmtPortType == L7_MGMT_IPPORT)
    {
      osapiSnprintf (buf, sizeof(buf), "intIfNum - %d", intIfNum);
    }
    else
    {
      osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
      return;
    }

    if ((++clientIntfInfo->numTrails) < DHCP_CLIENT_NUM_ADDRESS_TRAILS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Resetting the Timeout.  Running Trail - %d "
                   "of the Timeout mechanism", (clientIntfInfo->numTrails + 1));
      reset_timeout (clientIntfInfo);
      dhcpClientIPAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_METHOD_DHCP,
                                    mgmtPortType, L7_FALSE);
      osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
      return;
    }

    L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_DHCP_CLIENT_COMPONENT_ID,
             "Failed to acquire an IP address on %s; DHCP Server did not respond.", buf);
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire an IP address on %s; "
                 "DHCP Server did not respond", buf);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return;
  }

  if (clientIntfInfo->sockFD == 0)
  {
    if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      dhcpClientSocketClose (clientIntfInfo);
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
      return;
    }
  }

  if ((clientIntfInfo->dhcpState == DHCPSTATE_INIT) ||
      (clientIntfInfo->dhcpState == DHCPSTATE_SELECTING))
  {
    clientIntfInfo->dhcpState = DHCPSTATE_INIT;
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Invoking DHCP State Machine with State - %d",
                 clientIntfInfo->dhcpState);
    clientIntfInfo->retryCount++;
    doDhcp (clientIntfInfo);
  }
  else if ((clientIntfInfo->dhcpState == DHCPSTATE_REQUESTING) ||
      (clientIntfInfo->dhcpState == DHCPSTATE_REQUEST_RECV))
  {
    clientIntfInfo->dhcpState = DHCPSTATE_REQUESTING;
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Invoking DHCP State Machine with State - %d",
                 clientIntfInfo->dhcpState);
    clientIntfInfo->retryCount++;
    doDhcp (clientIntfInfo);
  }
  else
  {
    /* Do Nothing */
  }

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/**************************************************************************
* @purpose  Start the Interface Timer to the Next Timeout Time
*
* @param    clientIntfInfo      Pointer to the Client Interface
*                               information structure
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void
dhcpClientNextTimeoutTimerStart (dhcpClientInfo_t *clientIntfInfo)
{
  L7_uint32 timeOutMilliSec = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "intIfNum-%d, mgmtPortType-%d",
               clientIntfInfo->intIfNum, clientIntfInfo->mgmtPortType);

  timeOutMilliSec = (clientIntfInfo->tv.tv_sec * 1000) + (clientIntfInfo->tv.tv_usec/1000);
  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Starting Timer for %d Milli Sec", timeOutMilliSec);
  osapiTimerAdd ((void*) next_timeout_function, (L7_uint32) clientIntfInfo->intIfNum,
                 (L7_uint32) clientIntfInfo->mgmtPortType, timeOutMilliSec,
                 &clientIntfInfo->nextTimeout);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/**************************************************************************
* @purpose  Stop the Interface Timer
*
* @param    clientIntfInfo      Pointer to the Client Interface
*                               information structure
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void
dhcpClientNextTimeoutTimerStop (dhcpClientInfo_t *clientIntfInfo)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "intIfNum-%d, mgmtPortType-%d",
               clientIntfInfo->intIfNum, clientIntfInfo->mgmtPortType);

  if (clientIntfInfo->nextTimeout != L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Stopping Timer");
    osapiTimerFree (clientIntfInfo->nextTimeout);
    clientIntfInfo->nextTimeout = L7_NULLPTR;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/**************************************************************************
* @purpose  Alarm to notify T1, T2 timers and lease expiry
*
* @param    data      Internal Interface Number
* @param    arg2      Mgmt Interface Type
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void alarm_function (L7_uint32 data, L7_uint32 arg2)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  dhcpLeaseInfo_t *lease = L7_NULLPTR;
  L7_uint32 intIfNum = 0;
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  L7_uint32 timeOut = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  intIfNum = data;
  mgmtPortType = (L7_MGMT_PORT_TYPE_t) arg2;

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d, "
                 "mgmtPortType-%d", intIfNum, mgmtPortType);
    return;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "DHCP Client is Not in USE on intIfNum - %d, "
                 "mgmtPortType - %d", intIfNum, mgmtPortType);
    return;
  }

  if (clientIntfInfo->sockFD == 0)
  {
    if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      dhcpClientSocketClose (clientIntfInfo);
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      return;
    }
  }

  lease = &clientIntfInfo->leaseInfo;
  lease->alarm = L7_NULLPTR;
  lease->which |= lease->next;

  /* Step the lease on into its next state of being alarmed ;-)*/
  if (lease->next & DHCP_LEASE_EX)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Lease has Expired.  Releasing ...");
    clientIntfInfo->dhcpState = DHCPSTATE_DO_RELEASE;
    doDhcp (clientIntfInfo);
    return;
  }
  else if (lease->next & DHCP_LEASE_T2)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Lease T2 has Expired.  Rebinding ...");
    timeOut = lease->expiry;
    lease->next = DHCP_LEASE_EX;
    clientIntfInfo->dhcpState = DHCPSTATE_REBINDING;
  }
  else if (lease->next & DHCP_LEASE_T1)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Lease T1 has Expired.  Renewing ...");
    timeOut = lease->t2;
    lease->next = DHCP_LEASE_T2;
    clientIntfInfo->dhcpState = DHCPSTATE_RENEWING;
  }

  DHCPC_TRACE (DHCPC_DEBUG_TIMER, "Adding new Lease Timer for %d msecs", timeOut);
  osapiTimerAdd ((void*) alarm_function, (L7_uint32) clientIntfInfo->intIfNum,
                 (L7_uint32) clientIntfInfo->mgmtPortType, timeOut,
                 &lease->alarm);

  doDhcp (clientIntfInfo);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/**************************************************************************
* @purpose  Read lease renew / rebind / expiry times from packet
*
* @param    bootp*             Ptr to dhcp packet
* @param    dhcp_lease*        Ptr to lease structure
*
* @returns  Void
*
* @comments Added for defect 4356.
*
* @end
*************************************************************************/
static void get_lease_params( struct bootp *bootp, dhcpLeaseInfo_t *lease )
{
  L7_uint32 pkt_lease_time = 0;
  L7_uint32 pkt_t1 = 0;
  L7_uint32 pkt_t2 = 0;

  /* 
   * Lease time MUST be included (for DHCPOFFER and DHCPACK in response to 
   * a DHCPREQUEST) so it's okay to assume it's there.
   */
  l7_get_bootp_option( bootp, TAG_DHCP_LEASE_TIME, &pkt_lease_time, sizeof(pkt_lease_time) );
  /* Retrieve T1 and T2 from packet, if specified */
  l7_get_bootp_option( bootp, TAG_DHCP_RENEWAL_TIME, &pkt_t1, sizeof(pkt_t1) );
  l7_get_bootp_option( bootp, TAG_DHCP_REBIND_TIME, &pkt_t2, sizeof(pkt_t2) );
  

  if (pkt_lease_time != lease->net_expiry) {
      lease->net_t1 = 0;
      lease->net_t2 = 0;
  }

  /* If not specified, values will be 0 */
  if ( 0 != pkt_lease_time) {
    lease->net_expiry = pkt_lease_time;
  }
  if ( 0 != pkt_t1 ) {
    lease->net_t1 = pkt_t1;
  }
  if ( 0 != pkt_t2 ) {
    lease->net_t2 = pkt_t2;
  }
  if ((osapiNtohl(lease->net_t1) >= osapiNtohl(lease->net_t2)) || 
      (osapiNtohl(lease->net_t1) >= osapiNtohl(lease->expiry))) {
    lease->net_t1 = 0;
  }
  if (osapiNtohl(lease->net_t2) >= osapiNtohl(lease->expiry)) {
    lease->net_t2 = 0;
  }
}


/**************************************************************************
* @purpose  Delete timer from any existing lease
*
* @param    dhcp_lease*        Ptr to lease structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
static void del_lease_timer( dhcpLeaseInfo_t *lease )
{
  osapiTimerDescr_t *pTimerHolder;
  pTimerHolder= lease->alarm;
  if ( pTimerHolder )
  {
    osapiTimerFree(pTimerHolder);  /* Destroy the timer */
    pTimerHolder = 0;
    lease->alarm = L7_NULLPTR;
  }
}


/**************************************************************************
* @purpose  Initialize the lease structure
*
* @param    dhcp_lease*        Ptr to lease structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
void no_lease (dhcpLeaseInfo_t *lease)
{
  del_lease_timer( lease );
  lease->net_t1 = lease->net_t2 = lease->net_expiry = 0;
}


/**************************************************************************
* @purpose  Initialize the new lease structure values
*
* @param     dhcp_lease*    Ptr to lease structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
static void new_lease (dhcpClientInfo_t *clientIntfInfo)
{
  dhcpLeaseInfo_t *lease = L7_NULLPTR;
  L7_uint32 now ;/*= cyg_current_time();*/
  L7_uint32 then;
  osapiTimerDescr_t *pTimerHolder;

  lease = &clientIntfInfo->leaseInfo;
  //lease->expiry_uptime = osapiUpTimeMillisecondsGet();
  lease->expiry_uptime = osapiTimeMillisecondsGet64();
  now=0;
  pTimerHolder = lease->alarm;

  /* Silence any jabbering from past lease on this interface*/
  del_lease_timer( lease );
  lease->which = lease->next = 0;

  if ( 0xffffffff == lease->net_expiry )
  {
    lease->expiry = 0xffffffff;
    lease->t2     = 0xffffffff;
    lease->t1     = 0xffffffff;
    return; /* it's an infinite lease*/
  }
  
  /* Calculate T1 and T2's default values if they weren't specified */
  if ( 0 == lease->net_t1 ) {
      lease->net_t1 = osapiHtonl(osapiNtohl(lease->net_expiry) / 2);
  }
  if ( 0 == lease->net_t2 ) {
      lease->net_t2 = osapiHtonl((osapiNtohl(lease->net_expiry) / 8) * 7);
  }

  /*set T1 timer value*/
  then = (L7_double64)(osapiNtohl(lease->net_t1));
  then *= 1000; /* into mS*/
  lease->t1 = now + then;

  /*set T2 timer value*/
  then = (L7_double64)(osapiNtohl(lease->net_t2));
  then *= 1000; /* into mS - we know there is room in a tick_count_t*/
  lease->t2 = now + then - lease->t1; /*relative time */

  /* extract the lease time and scale it &c to now.*/
  then = (L7_long32)(osapiNtohl(lease->net_expiry));
  then *= 1000; 
  lease->expiry = now + then - (lease->t2 + lease->t1); /*relative time*/

  lease->next = DHCP_LEASE_T1;
  osapiTimerAdd ((void*) alarm_function, (L7_uint32) clientIntfInfo->intIfNum,
                 (L7_uint32) clientIntfInfo->mgmtPortType, lease->t1,
                 &lease->alarm);
}

/* ------------------------------------------------------------------------
   Bring an interface down, failed to initialize it or lease is expired
   Also part of normal startup, bring down for proper reinitialization*/


/* Scans specified buffer for specified option */
#define SCANTAG( ptrM, ptrEnd ) {                \
      L7_uchar8 *opM = (ptrM);                   \
      while (*opM != TAG_END && opM <= ptrEnd) { \
          if (*opM == tag) {                     \
              for (i = 0;  i < *(opM+1);  i++) { \
                  *val++ = *(opM+i+2);           \
              }                                  \
              return L7_TRUE;                    \
          }                                      \
          opM += *(opM+1)+2;                     \
      }                                          \
    }
	
/**************************************************************************
* @purpose  Bring down the interface by deleting IP interface 
*
* @param    struct bootp*      Ptr to bootp structure
* @param    L7_uchar8*       Tag Identity
* @param    Void*              Option Value
* @param    optlen     @b{input}   Option Value size
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_short16
l7_get_bootp_option(struct bootp *bp, L7_uchar8 tag, void *opt, L7_uint32 optlen)
{
  L7_uchar8 *val = (L7_uchar8 *)opt;
  L7_int32 i;
  L7_ushort16 optover = L7_NULL;
  L7_uchar8 * op;
  L7_uchar8 *bufEnd;

  op= &bp->bp_vend[4];
  bufEnd = &bp->bp_vend[BP_VEND_LEN - 1];
  
  while ( *op != TAG_END && op <= bufEnd )
  {
    if ( *op == tag )
    {
      for ( i = 0;  (i < *(op+1)) && (i< (optlen));  i++ )
      {
        if ((tag == TAG_DOMAIN_NAME) && (i == (optlen-1)))
        {
          break;
        }
        *val++ = *(op+i+2);
      }
      if (tag == TAG_DOMAIN_NAME)
      {
        *val = '\0';
      }
      return L7_TRUE;                        
    }
    op += *(op+1)+2;                        
  }    

  if (tag == TAG_DHCP_OPTOVER) return L7_FALSE;

  if ( l7_get_bootp_option( bp, TAG_DHCP_OPTOVER, &optover, sizeof(optover) ) == L7_TRUE )
  {
    if ( optover == DHCP_OPTION_OVERLOAD_SNAME || optover == DHCP_OPTION_OVERLOAD_SNAME_BOOTFILE) /* then the file field also holds options*/
    {
	  bufEnd = &bp->bp_file[BP_FILE_LEN - 1];
   	  SCANTAG( &bp->bp_file[0], bufEnd );
    }

    if ( optover == DHCP_OPTION_OVERLOAD_SNAME || optover == DHCP_OPTION_OVERLOAD_SNAME_BOOTFILE) /* then the sname field also holds options*/
    {
	  bufEnd = &bp->bp_sname[BP_SNAME_LEN - 1];
	  SCANTAG( &bp->bp_sname[0], bufEnd );	
    }
  }
  
  else
  {
    /* The options in the dhcp packet contains neigher the TAG asked for nor the TAG_END */
    if (dhcpcDebugMsgLevel > 0)
    {
      sysapiPrintf ("l7_get_bootp_option(): TAG %d not found in the buffer\n", tag);
    }
  }
  
  return L7_FALSE;
}

/**************************************************************************
* @purpose  Add DNS options to DNS Client
*
* @param    received @b{(input)} Ptr to boot structure
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void dhcpClientDnsOptionsAdd(struct bootp *received)
{
  L7_uint32  serverIp[L7_DNS_NAME_SERVER_ENTRIES] = {L7_NULL};
  L7_uchar8  domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX + 1];
  L7_inet_addr_t serverInetAddr;

  memset(domainNameList, 0, sizeof(domainNameList));

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (l7_get_bootp_option (received, TAG_DOMAIN_SERVER, &serverIp, sizeof(serverIp)) == L7_TRUE)
  {
    /* if domain name sever option is found in response,
     * add it dns name server list. 
     */
    L7_uint32 i=0;
    for (i=0; i<L7_DNS_NAME_SERVER_ENTRIES;i++)
    {
      if (serverIp[i] == 0)
      {
        break;
      }
      inetAddressSet (L7_AF_INET, &serverIp[i], &serverInetAddr);
      if (dnsClientNameServerEntryAdd (&serverInetAddr) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DNS name server entry add failed.");
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                "DNS name server entry add failed.");
      }
    }
  }
  if (l7_get_bootp_option (received, TAG_DOMAIN_NAME, domainNameList, sizeof(domainNameList)) == L7_TRUE)
  {
    /* if domain name List option is found in response,
     * add it to dns domain name List. 
     */
    if (dnsClientDomainNameListAdd (domainNameList) != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DNS domain name list entry add failed.");
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "DNS domain name list entry add failed.");
    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
}

/**************************************************************************
* @purpose  Remove DNS options from DNS Client
*
* @param    xmit @b{(input)} Ptr to boot structure
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
static void dhcpClientDnsOptionsRemove(struct bootp *xmit)
{
  L7_uint32  serverIp[L7_DNS_NAME_SERVER_ENTRIES] = {L7_NULL};
  L7_uchar8  domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX + 1];
  L7_inet_addr_t serverInetAddr;

  memset(domainNameList, 0, sizeof(domainNameList));

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (l7_get_bootp_option (xmit, TAG_DOMAIN_SERVER, &serverIp, sizeof(serverIp)) == L7_TRUE)
  {
    /* if domain name sever option is found in response,
     * delete from dns name server list. 
     */
    L7_uint32 i=0;
    for (i=0; i<L7_DNS_NAME_SERVER_ENTRIES;i++)
    {
      if (serverIp[i] == 0)
      {
        break;
      }
      inetAddressSet (L7_AF_INET, &serverIp[i], &serverInetAddr);
      if (dnsClientNameServerEntryRemove (&serverInetAddr) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DNS name server entry delete failed.");
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
                "DNS name server entry delete failed.");
      }
    }
  }
  if (l7_get_bootp_option (xmit, TAG_DOMAIN_NAME, domainNameList, sizeof(domainNameList)) == L7_TRUE)
  {
    /* if domain name List option is found in response,
     * delete from dns domain name List. 
     */
    if (dnsClientDomainNameListRemove (domainNameList) != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DNS domain name list entry delete failed.");
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
              "DNS domain name list entry delete failed.");

    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
}

/**************************************************************************
* @purpose   the DHCP state machine - this does all the work
*
* @param   intf               Interface name
* @param   requestedIpAddr    Requested IPv4 address. May be 0. 
* @param   res                DHCP message buffer
* @param   pstate             DHCP client state (RFC 2131) on this interface
* @param   lease              lease data
* @param   action (output)    Set to CONFIG_SET on output to indicate that the 
*                             DHCP server has given the client information 
*                             (e.g., an IP address) that the DHCP client should 
*                             save. Otherwise, CONFIG_NONE.
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None
*
* @end
*************************************************************************/
L7_RC_t
doDhcp (dhcpClientInfo_t *clientIntfInfo)
{
  struct bootp *xmit = L7_NULLPTR;
  struct bootp *received = L7_NULLPTR;
  L7_DHCP_CLIENT_STATE_t *dhcpState = &clientIntfInfo->dhcpState;
  L7_MGMT_PORT_TYPE_t mgmtPortType = clientIntfInfo->mgmtPortType;
  L7_uchar8 mincookie[] = {99,130,83,99,255} ;
  L7_sockaddr_in_t broadcastAddr;
  L7_sockaddr_in_t serverAddr;
  L7_sockaddr_in_t clientInitialAddr;
  L7_uint32 bytesSent = 0, clientIntIfNum;
  L7_uchar8 msgType = 0;
  L7_char8 buf1[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 buf2[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 buf3[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 buf4[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 errNo = 0;
  L7_char8 *pErrString = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (clientIntfInfo->sockFD <= 0)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid Socket FD - %d for intIfNum - %d, "
                 "mgmtPortType - %d", clientIntfInfo->sockFD,
                 clientIntfInfo->intIfNum, mgmtPortType);
    return L7_FAILURE;
  }

  memset((L7_char8*) &broadcastAddr, 0, sizeof(broadcastAddr));
  broadcastAddr.sin_family = L7_AF_INET;
  broadcastAddr.sin_addr.s_addr = osapiHtonl (0xffffffff);
  broadcastAddr.sin_port = osapiHtons (IPPORT_BOOTPS);

  memset((L7_char8*) &clientInitialAddr, 0, sizeof(clientInitialAddr));
  clientInitialAddr.sin_family = L7_AF_INET;
  clientInitialAddr.sin_addr.s_addr = osapiHtonl (INADDR_ANY);
  clientInitialAddr.sin_port = osapiHtons (IPPORT_BOOTPC);

  memset((L7_char8*) &serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = L7_AF_INET;
  serverAddr.sin_addr.s_addr = osapiHtonl (INADDR_BROADCAST); /* overwrite later */
  serverAddr.sin_port = osapiHtons (IPPORT_BOOTPS);

  if ((*dhcpState == DHCPSTATE_INIT) ||
      (*dhcpState == DHCPSTATE_REBINDING) ||
      (*dhcpState == DHCPSTATE_INFORM_REQUEST))
  {
#ifdef _L7_OS_LINUX_
    /* This is required on linux, but harmful on VxWorks. VxWorks installs a hidden
     * route to 255.255.255.255 at startup. Adding the same route here would either fail
     * or just bump the ref count in the stack's routing table. Then if DHCP fails 
     * for example if the mgmt interface is down, removing this route will remove
     * the route installed by VxWorks itself. Then sending to 255.255.255.255 will 
     * never work again. One might wonder why you would even do a routing table lookup
     * for a broadcast packet, but there it is. */
    if (clientIntfInfo->retryCount == 0)
    {
      if (osapimRouteEntryAdd (clientIntfInfo->intfName, 0xffffffff, 0x0,
                               0xffffffff, 0, 0, 1)
                             != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Broadcast Route Addition in the IP Stack "
                     "Failed for Interface - %s", clientIntfInfo->intfName);
        return L7_FAILURE;
      }
    }
#endif
  }

  switch (*dhcpState)
  {
    case DHCPSTATE_INIT:
    {
#ifdef L7_NSF_PACKAGE
      L7_uint32 requestedIpAddress = 0;
#endif /* L7_NSF_PACKAGE */

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_INIT state");

      /* Fill in the BOOTP request - DHCPDISCOVER packet*/
      xmit = &clientIntfInfo->txBuf;
      bzero ((char *)xmit, sizeof(*xmit));
      xmit->bp_op = BOOTREQUEST;
      xmit->bp_htype = HTYPE_ETHERNET;
      xmit->bp_hlen = L7_MAC_ADDR_LEN;
      xmit->bp_xid = osapiHtonl(clientIntfInfo->transID);
      xmit->bp_secs = 0;
      xmit->bp_flags = osapiHtons(0x8000); /* BROADCAST FLAG*/
      bcopy ((L7_uchar8*) clientIntfInfo->macAddr, (L7_uchar8*) &xmit->bp_chaddr, xmit->bp_hlen);
      bcopy (mincookie, xmit->bp_vend, sizeof(mincookie));

      /* Set DHCP DISCOVER in the packet*/
      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPDISCOVER, 1);

#ifdef L7_NSF_PACKAGE
      if (mgmtPortType == L7_MGMT_SERVICEPORT)
      {
        requestedIpAddress = dhcpRequestedServPortAddrGet (requestedIpAddress);
      }
      else if (mgmtPortType == L7_MGMT_NETWORKPORT)
      {
        requestedIpAddress = dhcpRequestedNetworkAddrGet (requestedIpAddress);
      }
#ifdef L7_ROUTING_PACKAGE
      else if (mgmtPortType == L7_MGMT_IPPORT)
      {
        ipMapIntfCheckpointDhcpAddrGet (clientIntfInfo->intIfNum,
                                        &requestedIpAddress);
      }
#endif /* L7_ROUTING_PACKAGE */
      else
      {
        return L7_FAILURE;
      }

      /* Request the previous IP address, would like to have the same */
      if (requestedIpAddress != 0)
      {
        DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Checkpointed Address - %s",
                     dhcpClientDebugAddrPrint(requestedIpAddress, buf1));
        set_fixed_tag (xmit, TAG_DHCP_REQ_IP, requestedIpAddress, 4);
      }
#endif /* L7_NSF_PACKAGE */

      set_fixed_tag (xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2);
      set_variable_tag (xmit, TAG_DHCP_CLASSID);

      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "Sending DHCP DISCOVER Message ... Size - %d", dhcp_size(xmit));
      if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_TX) == L7_TRUE)
      {
        show_bootp (clientIntfInfo->intfName, xmit);
      }      

      rc = osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                              0, (L7_sockaddr_t*) &broadcastAddr,
                              sizeof(broadcastAddr),&bytesSent);
      if (rc != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP DISCOVER Message send failed; "
                     "rc - %d", rc);
        if (rc == L7_ERROR)
        {
          errNo = osapiErrnoGet();
          pErrString = strerror(errNo);
          DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP DISCOVER Message send failed; "
                       "Error No-%d, Error String-%s", errNo, pErrString);
        }
        *dhcpState = DHCPSTATE_INIT;
      }
      else
      {
        if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX) == L7_TRUE)
        {
          dhcpDebugPacketRxTxTrace (clientIntfInfo, xmit, L7_FALSE, bytesSent);
        }
        DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP DISCOVER Message sent successfully ..."
                     "bytesSent - %d", bytesSent);
        DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_SELECTING state ...");
        *dhcpState = DHCPSTATE_SELECTING;
      }

      dhcpClientNextTimeoutTimerStart (clientIntfInfo);

    } /* END - case DHCPSTATE_INIT */
    break;

    case DHCPSTATE_SELECTING:
    {
      L7_uint32 ifNetMask = 0;
      L7_uint32 errorMsg = 0;

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_SELECTING state");

      received = &clientIntfInfo->dhcpInfo.rxBuf;

      /* Check for Client MAC Address */
      if (memcmp (clientIntfInfo->macAddr, received->bp_chaddr, L7_MAC_ADDR_LEN)
               != 0)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client MAC Address Mismatch");
        return L7_FAILURE;
      }

      /* Check for Client Transaction ID */
      if (received->bp_xid != osapiHtonl(clientIntfInfo->transID))
      {
        if (clientIntfInfo->dhcpSelectingStateCounter < 4)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID Mismatch");
          clientIntfInfo->dhcpSelectingStateCounter++;
          return L7_FAILURE;
        }
        else
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID counter overflow");
        }
        break;
      }

      if (received->bp_siaddr.s_addr == 0)
      {
        /* then fill in from the options... */
        l7_get_bootp_option (received, TAG_DHCP_SERVER_ID,
                             &received->bp_siaddr.s_addr, sizeof(received->bp_siaddr.s_addr));
      }

      /* see if it was a DHCP reply or a bootp reply; it could be either. */
      if (l7_get_bootp_option (received, TAG_DHCP_MESS_TYPE, &msgType, sizeof(msgType)))
      {
        if (DHCPOFFER == msgType)
        {
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP OFFER Message Received ...\n"
               "From: %s, Server Address: %s, Offered Address:%s",
               dhcpClientDebugAddrPrint(clientIntfInfo->rxAddr.sin_addr.s_addr, buf1),
               dhcpClientDebugAddrPrint(received->bp_siaddr.s_addr, buf2),
               dhcpClientDebugAddrPrint(received->bp_yiaddr.s_addr, buf3));

          if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_RX) == L7_TRUE)
          {
            show_bootp (clientIntfInfo->intfName, received);
          }
          if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_RX) == L7_TRUE)
          {
             dhcpDebugPacketRxTxTrace (clientIntfInfo, received, L7_TRUE,
                                       clientIntfInfo->bytesRcvd);
          }

          /* Get subnet mask */
          if (l7_get_bootp_option (received, TAG_SUBNET_MASK, &ifNetMask, sizeof(ifNetMask)) != L7_TRUE)
          {
            ifNetMask = osapiHtonl (0xFFFFFF00); /* At least it's not uninitialized */
          }
          if (L7_MGMT_SERVICEPORT == clientIntfInfo->mgmtPortType)
          {
            clientIntIfNum = FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM;
          }
          else if(L7_MGMT_NETWORKPORT == clientIntfInfo->mgmtPortType)
          {
            clientIntIfNum = FD_CNFGR_NIM_MIN_CPU_INTF_NUM;
          }
          else
          {
            clientIntIfNum = clientIntfInfo->intIfNum;
          }
          /* Check for IP Address conflicts with other interfaces */
          if (sysapiIPConfigConflict (clientIntIfNum,
                                      osapiNtohl(received->bp_yiaddr.s_addr),
                                      ifNetMask, (L7_uint32*) &errorMsg)
                                   != L7_TRUE)
          {
            /* No conflict - all is well Save the good packet in *xmit */
            xmit = &clientIntfInfo->txBuf;
            bcopy ((L7_char8*) received, (L7_char8*) xmit, dhcp_size(received));
            /* Grab T1, T2, lease time from this packet if specified */
            get_lease_params (received, &clientIntfInfo->leaseInfo);
            get_tag (received, TAG_VEND_SPECIFIC);
            reset_timeout (clientIntfInfo);
            dhcpClientNextTimeoutTimerStop (clientIntfInfo);
            DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_REQUESTING state ...");
            *dhcpState = DHCPSTATE_REQUESTING;
          }
          else /* IP Addr conflict */
          {
            DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Offered IP address is conflicting "
                         "with the existing configuration ... Waiting for more"
                         "Offer Messages");
            break;
          }
        }
      }

    } /* END - case DHCPSTATE_SELECTING */

    /* Fall through and send the Request packet if the OFFER is acceptable */

    case DHCPSTATE_REQUESTING:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_REQUESTING state");

      xmit = &clientIntfInfo->txBuf;

      /* Just send what you got with a DHCPREQUEST in the message type.
         then wait for an ACK in DHCPSTATE_REQUEST_RECV.
         Fill in the BOOTP request - DHCPREQUEST packet*/
      xmit->bp_op = BOOTREQUEST;
      xmit->bp_flags = osapiHtons (0x8000); /* BROADCAST FLAG*/

      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1);
      xmit->bp_giaddr.s_addr = 0x0;
      set_fixed_tag (xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2);

      /* And this will be a new one:       */
      set_fixed_tag (xmit, TAG_DHCP_REQ_IP, osapiNtohl(xmit->bp_yiaddr.s_addr), 4);
      /* for subnet mask & default gateway @2975*/
      /* for DNS and TFTp servers IPS, config filename and hostname */
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

      set_variable_tag (xmit, TAG_DHCP_CLASSID);

      clientIntfInfo->dhcpInfo.offeredIpAddr.s_addr = xmit->bp_yiaddr.s_addr;
      l7_get_bootp_option (xmit, TAG_DHCP_SERVER_ID, &clientIntfInfo->dhcpInfo.server_ip,
                           sizeof(clientIntfInfo->dhcpInfo.server_ip));
      /* Per RFC 2131 Section 4.4.1 These fields should be initilized to 0 in the
       * BOOTREQUEST message. Local copies are made for validating the ACK. */
      xmit->bp_ciaddr.s_addr = 0;
      xmit->bp_yiaddr.s_addr = 0;
      xmit->bp_siaddr.s_addr = 0;
      xmit->bp_giaddr.s_addr = 0;

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "Sending DHCP REQUEST Message");
      if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_TX) == L7_TRUE)
      {
        show_bootp (clientIntfInfo->intfName, xmit);
      }

      rc = osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                             0, (L7_sockaddr_t*) &broadcastAddr,
                              sizeof(broadcastAddr),&bytesSent);
      if (rc != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP REQUEST Message send failed; "
                     "rc - %d", rc);
        if (rc == L7_ERROR)
        {
          errNo = osapiErrnoGet();
          pErrString = strerror(errNo);
          DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP REQUEST Message send failed; "
                       "Error No-%d, Error String-%s", errNo, pErrString);
        }
        *dhcpState = DHCPSTATE_REQUESTING;
      }
      else
      {
        if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX) == L7_TRUE)
        {
          dhcpDebugPacketRxTxTrace (clientIntfInfo, xmit, L7_FALSE, bytesSent);
        }
        DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP REQUEST Message sent successfully ..."
                     "bytesSent - %d", bytesSent);
        DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_REQUEST_RECV state ...");
        *dhcpState = DHCPSTATE_REQUEST_RECV;
      }

      dhcpClientNextTimeoutTimerStart (clientIntfInfo);

    } /* END - case DHCPSTATE_REQUESTING */
    break;

    case DHCPSTATE_REQUEST_RECV:
    {
      struct in_addr recvdServerAddr;

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_REQUEST_RECV state");

      received = &clientIntfInfo->dhcpInfo.rxBuf;

      /* wait for an ACK or a NACK - retry by going back to
       * DHCPSTATE_REQUESTING; NACK means go back to INIT.*/

      /* Check for Client MAC Address */
      if (memcmp (clientIntfInfo->macAddr, received->bp_chaddr, L7_MAC_ADDR_LEN)
               != 0)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client MAC Address Mismatch");
        return L7_FAILURE;
      }

      if ( received->bp_xid != osapiHtonl(clientIntfInfo->transID))
      {   /* not the same transaction;*/
        if (clientIntfInfo->dhcpReqRecvStateCounter < 4)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID Mismatch");
          clientIntfInfo->dhcpReqRecvStateCounter++;
          return L7_FAILURE;
        }
        else
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID counter overflow");
          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_FAILED state");
          *dhcpState = DHCPSTATE_FAILED;
        }
        break;
      }

      /* Cache the server ip from whom the ACK packet is received.
       * This IP would be used later while sending unicast RENEW or RELEASE */
      l7_get_bootp_option (received, TAG_DHCP_SERVER_ID, &recvdServerAddr, sizeof(recvdServerAddr));

      /* check if it was a DHCP reply*/
      if (l7_get_bootp_option (received, TAG_DHCP_MESS_TYPE, &msgType, sizeof(msgType) ) )
      {
          if (msgType == DHCPACK)
          {
            DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP ACK Message Received ...\n"
                 "From: %s, Server Address: %s, Offered Address:%s",
                 dhcpClientDebugAddrPrint(clientIntfInfo->rxAddr.sin_addr.s_addr, buf1),
                 dhcpClientDebugAddrPrint(recvdServerAddr.s_addr, buf2),
                 dhcpClientDebugAddrPrint(received->bp_yiaddr.s_addr, buf3));

            if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_RX) == L7_TRUE)
            {
              show_bootp (clientIntfInfo->intfName, received);
            }
            if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_RX) == L7_TRUE)
            {
              dhcpDebugPacketRxTxTrace (clientIntfInfo, received, L7_TRUE,
                                        clientIntfInfo->bytesRcvd);
            }
          }
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "YiAddr in Offer - %s, YiAddr in Ack - %s ...\n"
                       "SiAddr in Offer - %s, SiAddr in Ack - %s",
               dhcpClientDebugAddrPrint(clientIntfInfo->dhcpInfo.offeredIpAddr.s_addr, buf1),
               dhcpClientDebugAddrPrint(received->bp_yiaddr.s_addr, buf2),
               dhcpClientDebugAddrPrint(clientIntfInfo->dhcpInfo.server_ip.s_addr, buf3),
               dhcpClientDebugAddrPrint(recvdServerAddr.s_addr, buf4));

        if ((DHCPACK == msgType) &&
            (received->bp_yiaddr.s_addr == clientIntfInfo->dhcpInfo.offeredIpAddr.s_addr) &&
            (recvdServerAddr.s_addr == clientIntfInfo->dhcpInfo.server_ip.s_addr))
        {
#ifdef FEAT_METRO_CPE_V1_0
          bootpDhcpServerAddress = clientIntfInfo->dhcpInfo.server_ip.s_addr;
#endif

          /* Save the good packet in *xmit*/
          xmit = &clientIntfInfo->txBuf;
          bcopy ((L7_char8*) received, (L7_char8*) xmit, dhcp_size(received));

          reset_timeout (clientIntfInfo);
          dhcpClientNextTimeoutTimerStop (clientIntfInfo);
          /* Record the new lease and set up timers &c*/
          get_lease_params (received, &clientIntfInfo->leaseInfo);
          new_lease (clientIntfInfo);
          get_tag (received, TAG_VEND_SPECIFIC);

          /* Add the DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsAdd(received);

          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_BOUND State");
          *dhcpState = DHCPSTATE_BOUND;

          if (dhcpClientIPAddressUpdate (clientIntfInfo) != L7_SUCCESS)
          {
            DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Address Updation Failed");
          }
          break;
        }
        if ((DHCPNAK == msgType) &&
            (recvdServerAddr.s_addr == clientIntfInfo->dhcpInfo.server_ip.s_addr))
        {
          /* we're bounced!*/
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP NACK Received");
          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_INIT State and retry");
          *dhcpState = DHCPSTATE_INIT;  /* So back the start of the rigmarole.*/
          reset_timeout (clientIntfInfo);
          dhcpClientNextTimeoutTimerStop (clientIntfInfo);
          doDhcp (clientIntfInfo);
        }
        /* otherwise it's something else, maybe another offer, or a bogus
           NAK from someone we are not asking!
           Just listen again, which implicitly discards it.*/
      }
    } /* END - case DHCPSTATE_REQUEST_RECV */
    break;

    case DHCPSTATE_BOUND:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_BOUND state");

      /* It looks like nothing needs to be done in this state.
       * Socket will be closed only after releasing the IP Address.
       */
      DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Nothing to be Done here");
    } /* END - case DHCPSTATE_BOUND */
    break;

    case DHCPSTATE_RENEWING:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_RENEWING state");

      /* Just send what you got with a DHCPREQUEST in the message
         type UNICAST straight to the server.  Then wait for an ACK.*/
      xmit = &clientIntfInfo->txBuf;

      /* Fill in the BOOTP request - DHCPREQUEST packet*/
      xmit->bp_op = BOOTREQUEST;
      xmit->bp_flags = osapiHtons(0); /* No BROADCAST FLAG*/
      /* Use the *client* address here:*/
      xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1);
      set_fixed_tag (xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2);

      /*as per RFC, these fields MUST NOT be present in DHCP REQUEST*/
      remove_tag (xmit, TAG_DHCP_SERVER_ID);
      remove_tag (xmit, TAG_DHCP_REQ_IP);

      /* @2975 - Request that the DHCP server give us a default gateway
             if it knows about one - to be nice to users of 
       MS Windows NT 3.5/4.0 DHCP servers, per MSKB Q183199 */
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
      add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

      set_variable_tag (xmit, TAG_DHCP_CLASSID);

      serverAddr.sin_addr.s_addr = clientIntfInfo->dhcpInfo.server_ip.s_addr;

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "Sending DHCP RENEW Request Message");
      if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_TX) == L7_TRUE)
      {
        show_bootp (clientIntfInfo->intfName, xmit);
      }

      /* UNICAST address of the server:*/
      if (osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                             0, (L7_sockaddr_t*) &serverAddr,
                             sizeof(serverAddr),&bytesSent)
                          != L7_SUCCESS) 
      {
        errNo = osapiErrnoGet();
        pErrString = strerror(errNo);
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP RENEW Request Message send failed; "
                     "Error No-%d, Error String-%s", errNo, pErrString);
        DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_BOUND state");
        if (xmit->bp_yiaddr.s_addr != 0)
        {
          *dhcpState = DHCPSTATE_BOUND;
        }
        else
        {
          *dhcpState = DHCPSTATE_FAILED;
        }
        break;
      }

      if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX) == L7_TRUE)
      {
        dhcpDebugPacketRxTxTrace (clientIntfInfo, xmit, L7_FALSE, bytesSent);
      }

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP RENEW Request Message sent successfully ..."
                   "bytesSent - %d", bytesSent);
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_RENEW_RECV state");
      *dhcpState = DHCPSTATE_RENEW_RECV;

    } /* END - case DHCPSTATE_RENEWING */
    break;

    case DHCPSTATE_RENEW_RECV:
    {
      struct timeval tv;

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_RENEW_RECV state");

      received = &clientIntfInfo->dhcpInfo.rxBuf;
      xmit = &clientIntfInfo->txBuf;

      /* wait for an ACK or a NACK - retry by going back to
       * DHCPSTATE_RENEWING; NACK means go to NOTBOUND. */

      tv.tv_sec = ((clientIntfInfo->leaseInfo.t2) > 50000) ? 25 :
                   ((clientIntfInfo->leaseInfo.t2)/1000)/2;
      tv.tv_usec = 0;

      DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Wait Time is %ld\n", tv.tv_sec);
      
      if (received->bp_xid != osapiHtonl (clientIntfInfo->transID))
      {   /* not the same transaction;*/
        if (clientIntfInfo->dhcpRenewRecvStateCounter < 4 )
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID Mismatch");
          clientIntfInfo->dhcpRenewRecvStateCounter++;
        }
        else
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID counter overflow");
          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_BOUND state");
          *dhcpState = DHCPSTATE_BOUND;
        }
        break;                  
      }

      if (0 == received->bp_siaddr.s_addr)
      {
        /* then fill in from the options... */
        l7_get_bootp_option (received, TAG_DHCP_SERVER_ID,
                             &received->bp_siaddr.s_addr, sizeof(received->bp_siaddr.s_addr));
      }

      /* check it was a DHCP reply*/
      if (l7_get_bootp_option (received, TAG_DHCP_MESS_TYPE, &msgType, sizeof(msgType)))
      {
        if ((DHCPACK == msgType) &&
            (received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr))
        {
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP ACK Message Received ...\n"
               "From: %s, Server Address: %s, Offered Address:%s",
               dhcpClientDebugAddrPrint(clientIntfInfo->rxAddr.sin_addr.s_addr, buf1),
               dhcpClientDebugAddrPrint(received->bp_siaddr.s_addr, buf2),
               dhcpClientDebugAddrPrint(received->bp_yiaddr.s_addr, buf3));

          if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_RX) == L7_TRUE)
          {
            show_bootp (clientIntfInfo->intfName, received);
          }
          if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_RX) == L7_TRUE)
          {
            dhcpDebugPacketRxTxTrace (clientIntfInfo, received, L7_TRUE,
                                      clientIntfInfo->bytesRcvd);
          }

          /* Remove the old DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsRemove(xmit);

          /* Save the good packet in *xmit */
          bcopy ((L7_uchar8*)received, (L7_uchar8*)xmit, dhcp_size(received));
          /* we like the packet, so reset the timeout for next time */
          /* Record the new lease and set up timers &c */
          get_lease_params (received, &clientIntfInfo->leaseInfo);
          new_lease (clientIntfInfo);
          get_tag (received, TAG_VEND_SPECIFIC);

          /* Add the DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsAdd(received);

          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_BOUND State");
          *dhcpState = DHCPSTATE_BOUND;
          if (dhcpClientIPAddressUpdate (clientIntfInfo) != L7_SUCCESS)
          {
            DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Address Updation Failed");
          }
          break;
        }
        if (DHCPNAK == msgType)
        { /* we're bounced! */
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP NACK Received");
          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_NOTBOUND State");
          *dhcpState = DHCPSTATE_NOTBOUND;  /* So quit out. */
          break;
        }
        /* otherwise it's something else, maybe another offer.
           Just listen again, which implicitly discards it.*/
      }
      else
      {
        DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "Invalid DHCP Message Type received");
      }

    } /* END - case DHCPSTATE_RENEW_RECV */
    break;

    case DHCPSTATE_REBINDING:
    {

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_REBINDING state");

      xmit = &clientIntfInfo->txBuf;

      /* Just send what you got with a DHCPREQUEST in the message type.
       * Then wait for an ACK.  This one is BROADCAST.

      Fill in the BOOTP request - DHCPREQUEST packet */
      xmit->bp_op = BOOTREQUEST;
      xmit->bp_flags = osapiHtons(0); /* no BROADCAST FLAG*/
      /* Use the *client* address here: */
      xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1);
      set_fixed_tag (xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2);

      /*as per RFC, these fields MUST NOT be present in DHCP REQUEST*/
      remove_tag (xmit, TAG_DHCP_SERVER_ID);
      remove_tag (xmit, TAG_DHCP_REQ_IP);

      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

      set_variable_tag (xmit, TAG_DHCP_CLASSID);

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "Sending DHCP REBIND Request Message");
      if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_TX) == L7_TRUE)
      {
        show_bootp (clientIntfInfo->intfName, xmit);
      }

      if (osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                             0, (L7_sockaddr_t*) &broadcastAddr,
                             sizeof(broadcastAddr), &bytesSent)
                          != L7_SUCCESS) 
      {
        errNo = osapiErrnoGet();
        pErrString = strerror(errNo);
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP REBIND Request Message send failed; "
                     "Error No-%d, Error String-%s", errNo, pErrString);
        DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_BOUND state");

        *dhcpState = DHCPSTATE_BOUND;
        break;
      }

      if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX) == L7_TRUE)
      {
        dhcpDebugPacketRxTxTrace (clientIntfInfo, xmit, L7_FALSE, bytesSent);
      }

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP REBIND Request Message sent successfully ..."
                   "bytesSent - %d", bytesSent);
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_REBIND_RECV state");
      *dhcpState = DHCPSTATE_REBIND_RECV;

    } /* END - case DHCPSTATE_REBINDING */
    break;

    case DHCPSTATE_REBIND_RECV:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_REBIND_RECV state");

      /* wait for an ACK or a NACK - retry by going back to
       * DHCPSTATE_REBINDING; NACK means go to NOTBOUND. */

      received = &clientIntfInfo->dhcpInfo.rxBuf;
      xmit = &clientIntfInfo->txBuf;

      if (received->bp_xid != osapiHtonl (clientIntfInfo->transID))
      {   /* not the same transaction;    */
        if (clientIntfInfo->dhcpRebindRecvStateCounter < 4)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID Mismatch");
          clientIntfInfo->dhcpRebindRecvStateCounter++;
        }
        else
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Transaction ID counter overflow");
          DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving back to DHCPSTATE_BOUND state");
          *dhcpState = DHCPSTATE_BOUND;
        }
        break;
      }

      if (0 == received->bp_siaddr.s_addr)
      {
        /* then fill in from the options...    */
        l7_get_bootp_option (received, TAG_DHCP_SERVER_ID,
                             &received->bp_siaddr.s_addr, sizeof(received->bp_siaddr.s_addr));
      }

      /* check it was a DHCP reply */
      if (l7_get_bootp_option (received, TAG_DHCP_MESS_TYPE, &msgType, sizeof(msgType)))
      {
        if ((DHCPACK == msgType) &&
            (received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr))
        {
          DHCPC_TRACE (DHCPC_DEBUG_PKT_RX, "DHCP ACK Message Received ...\n"
               "From: %s, Server Address: %s, Offered Address:%s",
               dhcpClientDebugAddrPrint(clientIntfInfo->rxAddr.sin_addr.s_addr, buf1),
               dhcpClientDebugAddrPrint(received->bp_siaddr.s_addr, buf2),
               dhcpClientDebugAddrPrint(received->bp_yiaddr.s_addr, buf3));

          if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_RX) == L7_TRUE)
          {
            show_bootp (clientIntfInfo->intfName, received);
          }
          if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_RX) == L7_TRUE)
          {
            dhcpDebugPacketRxTxTrace (clientIntfInfo, received, L7_TRUE,
                                      clientIntfInfo->bytesRcvd);
          }

          /* Remove the old DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsRemove(xmit);

          /* Save the good packet in *xmit   */
          bcopy ((char*)received, (L7_uchar8*) xmit, dhcp_size(received));
          reset_timeout (clientIntfInfo);
          /* Record the new lease and set up timers &c  */
          get_lease_params (received, &clientIntfInfo->leaseInfo);
          new_lease (clientIntfInfo);
          get_tag (received, TAG_VEND_SPECIFIC);

          /* Add the DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsAdd(received);
          
          *dhcpState = DHCPSTATE_BOUND;
          break;
        }
        else if (DHCPNAK == msgType)
        { /* we're bounced! */
          *dhcpState = DHCPSTATE_NOTBOUND;  /* So back the start of the rigmarole.*/
          break;
        }
        /* otherwise it's something else, maybe another offer.
           Just listen again, which implicitly discards it.*/
      }

    } /* END - case DHCPSTATE_REBIND_RECV */
    break;

    case DHCPSTATE_BOOTP_FALLBACK:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_BOOTP_FALLBACK state\n");
      }

      /* And no lease should have become active, but JIC*/
      no_lease (&clientIntfInfo->leaseInfo);
      reset_timeout (clientIntfInfo);
      *dhcpState = DHCPSTATE_FAILED;

    } /* END - case DHCPSTATE_BOOTP_FALLBACK */
    break;

    case DHCPSTATE_NOTBOUND:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_NOTBOUND state\n");
      }

      /* And no lease should have become active, but JIC*/
      no_lease (&clientIntfInfo->leaseInfo);
      reset_timeout (clientIntfInfo);
      *dhcpState = DHCPSTATE_FAILED;

    } /* END - case DHCPSTATE_NOTBOUND */
    break;

    case DHCPSTATE_FAILED:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_FAILED state\n");
      }

      no_lease (&clientIntfInfo->leaseInfo);
      reset_timeout (clientIntfInfo);

    } /* END - case DHCPSTATE_FAILED */
    break;

    case DHCPSTATE_DO_RELEASE:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_DO_RELEASE state");

      xmit = &clientIntfInfo->txBuf;

      /* We have been forced here by external means, to release the
       * lease for graceful shutdown.
       * Just send what you got with a DHCPRELEASE in the message
       * type UNICAST straight to the server.  No ACK.  Then go to
       * NOTBOUND state. */

      xmit->bp_op = BOOTREQUEST;
      xmit->bp_flags = osapiHtons(0); /* no BROADCAST FLAG*/
      /* Use the *client* address here: */
      xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;
      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPRELEASE, 1);

      serverAddr.sin_addr.s_addr = clientIntfInfo->dhcpInfo.server_ip.s_addr;

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "Sending DHCP RELEASE Message to Server - %s",
                   dhcpClientDebugAddrPrint(serverAddr.sin_addr.s_addr, buf1));
      if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_PKT_TX) == L7_TRUE)
      {
        show_bootp (clientIntfInfo->intfName, xmit);
      }

      osapiSleep(1); /* To enable IP Stack to send the packet out on the wire */
      /* UNICAST address of the server:*/
      if (osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                             0, (L7_sockaddr_t*) &serverAddr, sizeof(serverAddr),
                             &bytesSent)
                          != L7_SUCCESS) 
      {
        errNo = osapiErrnoGet();
        pErrString = strerror(errNo);
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP RELEASE Message send failed; "
                     "Error No-%d, Error String-%s", errNo, pErrString);
        DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_FAILED state");
      }

      if (dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX) == L7_TRUE)
      {
        dhcpDebugPacketRxTxTrace (clientIntfInfo, xmit, L7_FALSE, bytesSent);
      }

      /* Remove the old DNS Server and Domain Name information in DNS Client */
      dhcpClientDnsOptionsRemove(xmit);

      DHCPC_TRACE (DHCPC_DEBUG_PKT_TX, "DHCP RELEASE Message sent successfully ..."
                   "bytesSent - %d", bytesSent);

      if (clientIntfInfo->actImmediate != L7_TRUE)
      {
        if (dhcpClientIPAddressUpdate (clientIntfInfo) != L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Address Updation Failed");
        }
      }

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "Moving to DHCPSTATE_NOTBOUND state");
      *dhcpState = DHCPSTATE_NOTBOUND;

    } /* END - case DHCPSTATE_DO_RELEASE */
    break;

    case DHCPSTATE_INFORM_REQUEST:
    {
      L7_sockaddr_in_t localAddr; /* socket's local end point address */
      L7_uint32 configuredIpAddress = 0;

      DHCPC_TRACE (DHCPC_DEBUG_STATE, "In DHCPSTATE_INFORM_REQUEST state");

      if (mgmtPortType == L7_MGMT_SERVICEPORT)
      {
        configuredIpAddress = dhcpConfiguredServPortAddrGet ();
      }
      else if (mgmtPortType == L7_MGMT_NETWORKPORT)
      {
        configuredIpAddress = dhcpConfiguredNetworkAddrGet ();
      }
      else
      {
        return L7_FAILURE;
      }

      memset((L7_char8 *) &localAddr, 0, sizeof(localAddr));
      localAddr.sin_family = L7_AF_INET;
      localAddr.sin_addr.s_addr = osapiHtonl(configuredIpAddress);
      localAddr.sin_port = osapiHtons(IPPORT_BOOTPC);
      if( osapiSocketBind (clientIntfInfo->sockFD, (L7_sockaddr_t*) &localAddr,
                           sizeof(localAddr))
                        != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Binding Failed for configured "
                     "Address - 0x%x", configuredIpAddress);
        return L7_FAILURE;
      }

      xmit = &clientIntfInfo->txBuf;

      /* Fill in the BOOTP request - DHCPINFORM packet */
      xmit->bp_op    = BOOTREQUEST;
      xmit->bp_htype = HTYPE_ETHERNET;
      xmit->bp_hlen  = L7_MAC_ADDR_LEN;
      xmit->bp_xid   = osapiHtonl(clientIntfInfo->transID);
      xmit->bp_secs  = 0;
      xmit->bp_flags = osapiHtons(0x0000); /* BROADCAST FLAG*/
      memcpy (&xmit->bp_ciaddr, &configuredIpAddress, sizeof(configuredIpAddress));

      bcopy ((L7_uchar8*) clientIntfInfo->macAddr, (L7_uchar8*) &xmit->bp_chaddr, xmit->bp_hlen);
      bcopy (mincookie, xmit->bp_vend, sizeof(mincookie));

      /* Set DHCPINFORM message type */
      set_fixed_tag (xmit, TAG_DHCP_MESS_TYPE, DHCPINFORM, 1);

      set_fixed_tag(xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2);

      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
      add_fixed_tag (xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf ( "---------DHCPPHASE_INFORM sending:\n" );
        show_bootp (clientIntfInfo->intfName, xmit);
      }   

      if (osapiSocketSendto (clientIntfInfo->sockFD, (L7_char8*) xmit, dhcp_size(xmit),
                             0, (L7_sockaddr_t*) &broadcastAddr,
                             sizeof(broadcastAddr),&bytesSent)
                          != L7_SUCCESS) 
      {
        if (dhcpcDebugMsgLevel > 0)
        {
          errNo = osapiErrnoGet();
          pErrString = strerror(errNo);
          sysapiPrintf ("\nosapiSocketSendto() failed in do_dhcp() with error %d, %s.", errNo, pErrString);
        }
        *dhcpState = DHCPSTATE_FAILED;
        break;
      }

      *dhcpState = DHCPSTATE_INFORM_ACK_WAIT;

    } /* END - case DHCPSTATE_INFORM_REQUEST */
    break;

    case DHCPSTATE_INFORM_ACK_WAIT:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_INFORM_ACK_WAIT state\n");
      }

      received = &clientIntfInfo->dhcpInfo.rxBuf;

      if (dhcpcDebugMsgLevel > 0)
      {
        L7_char8 addrStr[OSAPI_INET_NTOA_BUF_SIZE];

        memset (addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
        osapiInetNtoa (osapiNtohl(clientIntfInfo->rxAddr.sin_addr.s_addr), addrStr);
        sysapiPrintf ("---------DHCPPHASE_INFORM_REQUEST_RECV received:\n");
        sysapiPrintf ("...rx_addr is family %d, addr %s, port %d\n",
                      clientIntfInfo->rxAddr.sin_family, addrStr,
                      osapiNtohs(clientIntfInfo->rxAddr.sin_port));
        show_bootp (clientIntfInfo->intfName, received);
      }

      if  (received->bp_xid != osapiHtonl (clientIntfInfo->transID))
      {   /* not the same transaction */
        if (clientIntfInfo->dhcpRebindRecvStateCounter < 4 )
        {
          clientIntfInfo->dhcpRebindRecvStateCounter++;
        }
        else
        {
          *dhcpState = DHCP_INFORM_FAILED_PHASE;
        }
        break;
      }

      /* check it was a DHCP reply */
      if (l7_get_bootp_option (received, TAG_DHCP_MESS_TYPE, &msgType, sizeof(msgType)))
      {
        if (DHCPACK == msgType)
        {
          xmit = &clientIntfInfo->txBuf;
          /* Save the good packet in *xmit */
          bcopy ((L7_char8*) received, (L7_char8*) xmit, dhcp_size(received));

          /* Add the DNS Server and Domain Name information in DNS Client */
          dhcpClientDnsOptionsAdd(received);
            
          *dhcpState = DHCPSTATE_INFORM_BOUND;
          break;
        }

        if (DHCPNAK == msgType)
        {
          /* we're bounced!*/
          *dhcpState = DHCPSTATE_INFORM_REQUEST;  /* So back the start of the rigmarole */
          doDhcp (clientIntfInfo);
          break;
        }
  
        /* otherwise it's something else, maybe another offer, or a bogus
           NAK from someone we are not asking!
           Just listen again, which implicitly discards it */          
      }

    } /* END - case DHCPSTATE_INFORM_ACK_WAIT */
    break;

    case DHCPSTATE_INFORM_FAILED:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_INFORM_FAILED state\n");
      }
 
    } /* END - case DHCPSTATE_INFORM_FAILED */
    break;

    case DHCPSTATE_INFORM_BOUND:
    {
      if (dhcpcDebugMsgLevel > 0)
      {
        sysapiPrintf("\nDHCPSTATE_INFORM_BOUND state\n");
      }
 
    } /* END - case DHCPSTATE_INFORM_BOUND */
    break;

    default:
    {
      DHCPC_TRACE (DHCPC_DEBUG_STATE, "Invalid DHCP State - %d", *dhcpState);
      return L7_FAILURE;
    } /* END - default */
    break;

  } /* END - Switch */

  if ((*dhcpState == DHCPSTATE_BOUND) ||
      (*dhcpState == DHCPSTATE_BOOTP_FALLBACK) ||
      (*dhcpState == DHCPSTATE_NOTBOUND) ||
      (*dhcpState == DHCPSTATE_FAILED) ||
      (*dhcpState == DHCPSTATE_INFORM_FAILED))
  {
#ifdef _L7_OS_LINUX_
    if (osapimRouteEntryDelete (clientIntfInfo->intfName, 0xffffffff, 0x0,
                                0xffffffff, 0, 0, 1)
                             != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Broadcast Route deletion failed in the "
                   "Linux stack for intfName - %s", clientIntfInfo->intfName);
    }
#endif
    dhcpClientSocketClose (clientIntfInfo);
  }

  if ((*dhcpState == DHCPSTATE_BOOTP_FALLBACK) ||
      (*dhcpState == DHCPSTATE_NOTBOUND) ||
      (*dhcpState == DHCPSTATE_FAILED) ||
      (*dhcpState == DHCPSTATE_INFORM_FAILED))
  {
    dhcpClientIntfInfoDeInit (clientIntfInfo);
  }

  return L7_SUCCESS;
}

