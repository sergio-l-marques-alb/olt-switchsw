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
#include "simapi.h"
#include "log.h"
#include "sysapi.h"
#include "l7netapi.h"
#include "simapi.h"

#include "l7_socket.h"
/* this needs to get rolled into bsdsock.h */
#define ipcom_memset memset

#include "l7_dhcp.h"

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

static L7_uchar8 timeout_random = 0;
extern struct in_addr previaddr;

struct timeout_state
{
  L7_uint32 secs;
  L7_int32 countdown;
};
extern dhcpOptions_t dhcpOptions;

static L7_uint32 dhcpcDebugMsgLevel = 0;

L7_uint32 dhcpcDebugMsgLvlSet(L7_uint32 level);
static void show_bootp(const L7_char8 *intf, struct bootp *bp);

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
    LOG_MSG( "dhcp_size():Bad DHCP cookie" );
    return 0;
  }
  op += 4;
  while ( *op != TAG_END )
  {
    op += *(op+1)+2;
    if ( op > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      LOG_MSG( "dhcp_size(): Oversize DHCP packet in dhcp_size" );
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
      LOG_MSG( "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
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
          LOG_MSG( "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
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
      LOG_MSG( "%s :Oversize DHCP packet", __FUNCTION__);
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
        LOG_MSG( "%s:Oversize DHCP packet", __FUNCTION__ );
        return L7_FAILURE; 
      }
      op += *(op+1)+2;
    }
  }

  switch(tag)
  {
    case TAG_VEND_SPECIFIC:
    {
      dhcpVendorSpecificOption_t *vendSpecificOpt = &dhcpOptions.vendorSpecificOption;
      L7_uchar8  type, index = 0;

      memset(vendSpecificOpt, 0, sizeof(dhcpVendorSpecificOption_t));
      pOption = begin + 2; /* Get content offset.*/
      while ((pOption < (begin + len)) && (index < DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM))
      {
        type = *pOption;
        if (type > DHCP_VENDOR_SPECIFIC_SUBOPTIONS_NUM)
        {
          LOG_MSG("%s: Unsupported subOption(%d) in VendorSpecific Option in received DHCP pkt ",
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
    LOG_MSG("%s: Unsupported DHCP option requested for extraction.");
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
      LOG_MSG( "set_fixed_tag():Oversize DHCP packet in set_fixed_tag" );
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
      LOG_MSG( "set_fixed_tag():Wrong size in set_fixed_tag" );
      return L7_FALSE;          
    }
  }
  else
  { /* overwrite the end tag and install a new one*/
    if ( op + len + 2 > &ppkt->bp_vend[BP_VEND_LEN-1] )
    {
      LOG_MSG( "set_fixed_tag():Oversize DHCP packet in set_fixed_tag append" );
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
      LOG_MSG( "add_fixed_tag():Oversize DHCP packet in add_fixed_tag (%d)", addingTag );
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
      LOG_MSG( "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
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
        LOG_MSG( "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
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
      LOG_MSG( "add_fixed_tag():Oversize DHCP packet in add_fixed_tag(%d)", addingTag );
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
      LOG_MSG( "%s : Oversized DHCP packet", __FUNCTION__ );
      return L7_FALSE; 
    }
    if ( *op == tag )
      break;
    op += *(op+1)+2;
  }

  /* If tag is already found, check for its length.*/
  if ( *op == tag )
  {
    LOG_MSG( "%s : Tag already set with size(%d)", __FUNCTION__, *(op+1));
    return L7_FALSE;          
  }
  else
  { 
    switch (tag)
    {
    case TAG_DHCP_CLASSID:
      if (dhcpOptions.vendorClassOption.optionMode == L7_ENABLE)
      {
        optLen = strlen(dhcpOptions.vendorClassOption.vendorClassString);
        /* Check if the addition of the tag makes the packet beyond the max size.*/
        if ( (op + optLen + 2) > (&ppkt->bp_vend[BP_VEND_LEN-1]) )
        {
          LOG_MSG( "%s :Oversize DHCP packet with option(%d) append of len(%d)", 
                   __FUNCTION__, tag, optLen);
          return L7_FALSE; 
        }
        /* overwrite the end tag and install a new one*/
        *op = tag;
        *(op+1) = optLen;
        memcpy(op+2,dhcpOptions.vendorClassOption.vendorClassString, optLen);
        *(op + optLen + 2) = TAG_END;
      }
      else
      {
        return L7_TRUE;
      }
      break;
    default:
      LOG_MSG(" %s : Unsupported DHCP Option to set", __FUNCTION__);
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
static void reset_timeout( struct timeval *ptv, struct timeout_state *pstate )
{
  timeout_random++;
  pstate->countdown = 4; /* initial fast retries*/
  pstate->secs = 4 + (timeout_random & 3);
  ptv->tv_sec = 4;
  ptv->tv_usec = 65536 * (2 + (timeout_random & 3));
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
static L7_int32 next_timeout( struct timeval *ptv, struct timeout_state *pstate )
{
  if ( 0 > pstate->countdown-- )
    return L7_TRUE;
  timeout_random++;
  pstate->secs = (ptv->tv_sec * 2) - 2 + (timeout_random & 3);
  pstate->countdown = 2; 
  ptv->tv_sec = pstate->secs ;
  return pstate->secs < 50; /* If longer, too many tries...*/
}


/**************************************************************************
* @purpose  Alarm to notify T1, T2 timers and lease expiry
*
* @param    L7_uint32      Address for dhcp_lease structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
static void alarm_function(L7_uint32 data)
{
  struct dhcp_lease *lease = (struct dhcp_lease *)data;

  lease->alarm = L7_NULLPTR;
  lease->which |= lease->next;

  /* Step the lease on into its next state of being alarmed ;-)*/
  if ( lease->next & DHCP_LEASE_EX )
  {
    *(lease->dhcpState)=DHCPSTATE_DO_RELEASE;
    *(lease->configStatus) = L7_SYSCONFIG_MODE_NOT_COMPLETE;    
  }
  else if ( lease->next & DHCP_LEASE_T2 )
  {
    osapiTimerAdd((void*) alarm_function, (L7_uint32)lease, L7_NULL, lease->expiry , &lease->alarm );
    lease->next = DHCP_LEASE_EX;
    *(lease->dhcpState)= DHCPSTATE_REBINDING;
    *(lease->configStatus) = L7_SYSCONFIG_MODE_NOT_COMPLETE;    
  }
  else if ( lease->next & DHCP_LEASE_T1 )
  {
    osapiTimerAdd((void*) alarm_function, (L7_uint32)lease, L7_NULL, lease->t2 , &lease->alarm );
    lease->next = DHCP_LEASE_T2;
    *(lease->dhcpState)= DHCPSTATE_RENEWING;
    *(lease->configStatus) = L7_SYSCONFIG_MODE_NOT_COMPLETE;                                    
  }
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
static void get_lease_params( struct bootp *bootp, struct dhcp_lease *lease )
{
  L7_uint32 pkt_lease_time = 0;
  L7_uint32 pkt_t1 = 0;
  L7_uint32 pkt_t2 = 0;

  /* 
   * Lease time MUST be included (for DHCPOFFER and DHCPACK in response to 
   * a DHCPREQUEST) so it's okay to assume it's there.
   */
  l7_get_bootp_option( bootp, TAG_DHCP_LEASE_TIME, &pkt_lease_time );
  /* Retrieve T1 and T2 from packet, if specified */
  l7_get_bootp_option( bootp, TAG_DHCP_RENEWAL_TIME, &pkt_t1 );
  l7_get_bootp_option( bootp, TAG_DHCP_REBIND_TIME, &pkt_t2 );
  
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
static void del_lease_timer( struct dhcp_lease *lease )
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
static void no_lease( struct dhcp_lease *lease )
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
static void new_lease( struct dhcp_lease *lease )
{
  L7_uint32 now ;/*= cyg_current_time();*/
  L7_uint32 then;

  osapiTimerDescr_t *pTimerHolder;

  /*now=osapiUpTimeRaw();*/
  now=0;
  pTimerHolder= lease->alarm;

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

#if 0 /* for testing this mechanism*/
  lease->expiry = now + 5000; /* 1000 here makes for failure in the DHCP test*/
  lease->t2     = now + 3500;
  lease->t1     = now + 2500;
#endif

  lease->next = DHCP_LEASE_T1;
  osapiTimerAdd((void*) alarm_function, (L7_uint32) lease, L7_NULL, lease->t1 , &lease->alarm);    
}

/**************************************************************************
* @purpose  Does the DHCP INFORM and saves all offered options
*
* @param    L7_uchar8*       Interface name
*
* @param    struct bootp*    Ptr to boot structure
*
* @param    L7_short16*      Ptr to DHCP state
*
* @param    L7_short16*      Ptr to configuration state
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32 dhcpInformRequest( const L7_uchar8 *intf, struct bootp *res, L7_short16 *pState, L7_short16* pConfigState )
{
  L7_sockaddr_in_t   localAddr;          /* socket's local end point address */
  L7_sockaddr_in_t   broadcast_addr;     /* socket's destination end point address */
  L7_sockaddr_in_t   rx_addr;            /* address of respondent */

  L7_uint32 currentIpAddr = L7_NULL;     /* transmission interface IP address */
  L7_uint32 dnsSvrIpAddr  = L7_NULL;     /* DNS server IP address */
  L7_uint32 bsent         = L7_NULL;     /* bytes sent */
  L7_uint32 brecvd        = L7_NULL;     /* bytes received */
  L7_uint32 xid           = L7_NULL;     /* random transaction identifier */
  L7_int32  clientSocket  = L7_NULL;     /* socket descriptor */
  L7_int32  maxFdLength   = L7_NULL;
  L7_int32  addrlen       = sizeof(L7_sockaddr_in_t);
  L7_int32  optionSetOn   = 1;           /* used to set on some socket options */
  
  L7_uchar8 ttlValue    = DHCP_IP_TTL_NUMBER;   /* IP TTL number to be set in request packets */
  L7_uchar8 msgtype     = L7_NULL;              /* type of received message */
  L7_uchar8 mincookie[] = {99,130,83,99,255} ;  /* DHCP vendor section prologue */
  L7_uchar8 intfName[IFNAMSIZ];                 /* interface name */
  L7_uchar8 macAddress[BP_CHADDR_LEN];          /* mac address of transmitting interface to be set in request packets */
  L7_uchar8 sysBIA[L7_MAC_ADDR_LEN];            /* for xid randomness */
  L7_uchar8 srvportBIA[L7_MAC_ADDR_LEN];        /* for xid randomness */
  
  L7_char8  addrStr[OSAPI_INET_NTOA_BUF_SIZE];                 /* debug usage */
  L7_char8  domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX];  /* list of DNS servers received */

  L7_short16 requestCounter = L7_NULL;
  fd_set     readFd;    
    
  struct timeval        tv;
  struct ifreq          ifr;
  struct timeout_state  timeout_scratch;

  L7_inet_addr_t         dnsSvrInetAddr;
  /* IMPORTANT: xmit is the same as res throughout this; *received is a
     scratch buffer for reception; its contents are always copied to res
     when we are happy with them.  So we always transmit from the
     existing state.*/
  struct bootp rx_local;
  struct bootp *received = &rx_local;
  struct bootp *xmit     = res;
  
#ifdef _L7_OS_ECOS_
  struct sockaddr_in *addrp = L7_NULLPTR;
  struct ecos_rtentry route;
  
  memset(&route, 0, sizeof(route));
#endif

  *pConfigState = CONFIG_NONE;

  memset(&localAddr, 0, sizeof(localAddr));
  memset(&broadcast_addr, 0, sizeof(broadcast_addr));
  memset(&rx_addr, 0, sizeof(rx_addr));
  memset(intfName, 0, sizeof(intfName));
  memset(macAddress, 0, sizeof(macAddress));
  memset(sysBIA, 0, sizeof(sysBIA));
  memset(srvportBIA, 0, sizeof(srvportBIA));
  memset(addrStr, 0, sizeof(addrStr));
  memset(domainNameList, 0, sizeof(domainNameList));
  memset(&tv, 0, sizeof(tv));
  memset(&ifr, 0, sizeof(ifr));
  memset(&timeout_scratch, 0, sizeof(timeout_scratch));
  memset(&rx_local, 0, sizeof(rx_local));

  inetAddressZeroSet(L7_AF_INET, &dnsSvrInetAddr);

  FD_ZERO(&readFd);
  
  osapiSnprintf(intfName, sizeof(intfName), "%s0", intf);
  
  dhcpConfigSemGet ();
  
  if ( strcmp(bspapiServicePortNameGet(), intf) == 0 )
  {
    currentIpAddr = simGetServPortIPAddr();
    simGetServicePortBurnedInMac((L7_uchar8 *) macAddress);  
	
    /* If we ever run DHCP on both interfaces at the same time, then distiguish. */
    simGetServicePortBurnedInMac(&srvportBIA[0]);
    xid = (L7_uint32)res + 
          (L7_uint32) (0xffffffff & osapiUpTimeRaw()) + 
          (L7_uint32)(srvportBIA[2]<<24) + 
          (L7_uint32)(srvportBIA[3]<<16) + 
          (L7_uint32)(srvportBIA[4]<<8) + 
          (L7_uint32)(srvportBIA[5]); 
  }
  else if ( strcmp(L7_DTL_PORT_IF, intf) == 0 )
  {
    currentIpAddr = simGetSystemIPAddr();
	  
    if ( simGetSystemIPMacType() == L7_SYSMAC_BIA )
         simGetSystemIPBurnedInMac((L7_uchar8 *) macAddress);
    else
         simGetSystemIPLocalAdminMac((L7_uchar8 *) macAddress);
			
#ifndef LVL7_NSX
    fflush(0); /* ??? */
#endif

    /* If we ever run DHCP on both interfaces at the same time, then distinguish. */
    simGetSystemIPBurnedInMac(&sysBIA[0]);
    xid = (L7_uint32)res + 
          (L7_uint32) (0xffffffff & osapiUpTimeRaw()) + 
          (L7_uint32)(sysBIA[2]<<24) + 
          (L7_uint32)(sysBIA[3]<<16) + 
          (L7_uint32)(sysBIA[4]<<8) + 
          (L7_uint32)(sysBIA[5]);  
  }
  
  dhcpConfigSemFree();

  sleep(5);

  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &clientSocket) == L7_FAILURE)
  {
    LOG_MSG("dhcpInformRequest(): Failed to create socket");
    return L7_FALSE;
  }

  FD_ZERO(&readFd);
  FD_SET(clientSocket, &readFd);
  maxFdLength = clientSocket +1;

  if ( osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_REUSEADDR,(L7_char8 *) &optionSetOn, sizeof(optionSetOn)) != L7_SUCCESS )
  {
    LOG_MSG("dhcpInformRequest(): osapiSetsockopt SO_REUSEADDR failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

#ifdef _L7_OS_VXWORKS_
  if ( osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_BINDTODEVICE, intfName, IFNAMSIZ) != L7_SUCCESS )
  {
    LOG_MSG("dhcpInformRequest(): osapiSetsockopt SO_BINDTODEVICE failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }
#endif

  /* Add device number. Zero only for first interface device of X type */
  sprintf(ifr.ifr_name,"%s%s", intfName,"0");
  bzero ((caddr_t) &ifr.ifr_addr, sizeof (ifr.ifr_addr));  

  ifr.ifr_addr.sa_family = AF_INET;  

  if ( osapiSetsockopt(clientSocket, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &optionSetOn, sizeof(optionSetOn)) != L7_SUCCESS )
  {
    LOG_MSG("dhcpInformRequest(): osapiSetsockopt SO_BROADCAST failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

#ifdef _L7_OS_ECOS_
  cyg_route_reinit();
  addrp = (struct sockaddr_in *) &ifr.ifr_addr;
  memset(addrp, 0, sizeof(*addrp));
  addrp->sin_family = AF_INET;
  addrp->sin_len = sizeof(*addrp);
  addrp->sin_port = 0;
  addrp->sin_addr.s_addr = INADDR_ANY;

  if (ioctl(clientSocket, SIOCSIFADDR, &ifr)) {
      perror("SIOCSIFADDR");
      return false;
  }
  if (ioctl(clientSocket, SIOCSIFNETMASK, &ifr)) {
      perror("SIOCSIFNETMASK");
      return false;
  }
  if (ioctl(clientSocket, SIOCSIFADDR, &ifr)) {
      perror("SIOCSIFADDR");
      return false;
  }

  /* the broadcast address is 255.255.255.255 */
  memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
  if (ioctl(clientSocket, SIOCSIFBRDADDR, &ifr)) {
      perror("SIOCSIFBRDADDR");
      return false;
  }
  // Set up routing
  /* the broadcast address is 255.255.255.255 */
  memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
  memset(&route, 0, sizeof(route));
  memcpy(&route.rt_gateway, addrp, sizeof(*addrp));

  addrp->sin_family = AF_INET;
  addrp->sin_port = 0;
  addrp->sin_addr.s_addr = INADDR_ANY;
  memcpy(&route.rt_dst, addrp, sizeof(*addrp));
  memcpy(&route.rt_genmask, addrp, sizeof(*addrp));

  route.rt_dev = ifr.ifr_name;
  route.rt_flags = RTF_UP|RTF_GATEWAY;
  route.rt_metric = 0;
  if (ioctl(clientSocket, SIOCADDRT, &route)) 
  {
    if (errno != EEXIST) {
        perror("SIOCADDRT 3");
        return false;
    }
  }
#endif

  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = osapiHtonl(0xffffffff);

  /* Set the mcast TTL = 255 as Cisco DHCP client does  */
  if ( osapiSetsockopt(clientSocket, IPPROTO_IP, L7_IP_MULTICAST_TTL, (L7_char8 *) &ttlValue, sizeof(ttlValue)) != L7_SUCCESS )
  {
    LOG_MSG("dhcpInformRequest(): osapiSetsockopt IP_MULTICAST_TTL failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

  memset((L7_char8 *) &localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = L7_AF_INET;
  localAddr.sin_addr.s_addr = osapiHtonl(/*INADDR_ANY*/currentIpAddr);
  localAddr.sin_port = osapiHtons(IPPORT_BOOTPC);

  memset((L7_char8 *) &broadcast_addr, 0, sizeof(broadcast_addr));
  broadcast_addr.sin_family = L7_AF_INET;
  broadcast_addr.sin_addr.s_addr = osapiHtonl(INADDR_BROADCAST);
  broadcast_addr.sin_port = osapiHtons(IPPORT_BOOTPS);

  if( osapiSocketBind(clientSocket, (L7_sockaddr_t *)&localAddr, sizeof(localAddr)) != L7_SUCCESS )
  {
    LOG_MSG("dhcpInformRequest(): osapiSocketBind failed");
    osapiSocketClose (clientSocket);
    return L7_FALSE;
  }

  /* Now, we can launch into the DHCP state machine.  I think this will
     be the neatest way to do it; it returns from within the switch arms
     when all is well, or utterly failed */

  reset_timeout( &tv, &timeout_scratch );
  *pState = DHCP_INFORM_REQUEST_PHASE;

  while ( L7_TRUE )
  {
    switch ( *pState )
    {
      case DHCP_INFORM_REQUEST_PHASE:

        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP INFORM phase\n");
        }

        /* need to reset when dhcp fails and retries */
        FD_ZERO(&readFd);
        FD_SET(clientSocket, &readFd);

        /* Send the DHCPINFORM packet*/
        /* Fill in the BOOTP request - DHCPINFORM packet */
        bzero((char *)xmit, sizeof(*xmit));
        xmit->bp_op    = BOOTREQUEST;
        xmit->bp_htype = HTYPE_ETHERNET;
        xmit->bp_hlen  = L7_MAC_ADDR_LEN;
        xmit->bp_xid   = osapiHtonl(xid);
        xmit->bp_secs  = 0;
        xmit->bp_flags = osapiHtons(0x0000); /* BROADCAST FLAG*/
		memcpy(&xmit->bp_ciaddr, &currentIpAddr, sizeof(currentIpAddr));

        bcopy((L7_uchar8 *) macAddress, (L7_uchar8 *)&xmit->bp_chaddr, xmit->bp_hlen);
        bcopy(mincookie, xmit->bp_vend, sizeof(mincookie));

        /* Set DHCPINFORM message type */
        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPINFORM, 1 );
		
        set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

		add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif		
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);
        
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf( "---------DHCPPHASE_INFORM sending:\n" );
          show_bootp( intf, xmit );
        }   
 
        if ( osapiSocketSendto(clientSocket, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&broadcast_addr, sizeof(broadcast_addr),&bsent)  != L7_SUCCESS )
        {
          errNo = osapiErrnoGet();
          pErrString = strerror(errNo);
          *pState = DHCP_INFORM_FAILED_PHASE;
          break;
        }

        *pState = DHCP_INFORM_ACK_WAIT_PHASE;
        break;
	  
      case DHCP_INFORM_ACK_WAIT_PHASE:

        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP INFORM REQUEST RECV phase\n");
        }

        /* wait for an ACK - retry by going back to DHCPPHASE_INFORM;*/
        if ( osapiSelect(maxFdLength, &readFd, NULL, NULL, tv.tv_sec, tv.tv_usec) <= 0 )
        {
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pState = DHCP_INFORM_FAILED_PHASE;
            break;
          }
		  
          *pState = DHCP_INFORM_REQUEST_PHASE;
          break;
        }

        if ( osapiSocketRecvfrom( clientSocket, (L7_char8*) received, sizeof(struct bootp), MSG_DONTWAIT,
                                  (L7_sockaddr_t *)&rx_addr, &addrlen, &brecvd)  != L7_SUCCESS )

        {
          /* No packet arrived go to the next larger timeout and re-send:*/
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pState = DHCP_INFORM_FAILED_PHASE;
            break;
          }
          *pState = DHCP_INFORM_REQUEST_PHASE;
          break;
        }

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(rx_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPPHASE_INFORM_REQUEST_RECV received:\n" );
          sysapiPrintf( "...rx_addr is family %d, addr %s, port %d\n",
                        rx_addr.sin_family, 
                        addrStr,
                        osapiNtohs(rx_addr.sin_port) );
						
          show_bootp( intf, received );
        }
        
        if ( received->bp_xid != osapiHtonl(xid) )
        {   /* not the same transaction */
          if ( requestCounter < 4 )
          {
            requestCounter++;
          }
          else
          {
            *pState = DHCP_INFORM_FAILED_PHASE;
          }
          break;                  
        }

        /* check it was a DHCP reply */
        if ( l7_get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) )
        {
          if ( DHCPACK == msgtype )
          {
            /* Save the good packet in *xmit */
            bcopy( (L7_char8*)received, (L7_char8*) xmit, dhcp_size(received) );
            /* we like the packet, so reset the timeout for next time */
            reset_timeout( &tv, &timeout_scratch );

            if (l7_get_bootp_option(received, TAG_DOMAIN_SERVER, &dnsSvrIpAddr))
            {
              /* if domain name sever option is found in response,
                 add it dns name server list */
              inetAddressSet(L7_AF_INET, &dnsSvrIpAddr, &dnsSvrInetAddr);
              if (dnsClientNameServerEntryAdd(&dnsSvrInetAddr) != L7_SUCCESS)
              {
                LOG_MSG("\n\rdhcpInformRequest(): DNS name server entry add failed.");
              }
            }
            if (l7_get_bootp_option(received, TAG_DOMAIN_NAME, domainNameList))
            {
              /* if domain name List option is found in response,
                 add it to dns domain name List */
              if (dnsClientDomainNameListAdd(domainNameList) != L7_SUCCESS)
              {
                LOG_MSG("\n\rdhcpInformRequest(): DNS domain name"
                                        "list entry add failed.");
              }
            }
            *pState = DHCP_INFORM_BOUND_PHASE;
            break;
          }

          if ( DHCPNAK == msgtype )
          {
            /* we're bounced!*/
            *pState = DHCP_INFORM_REQUEST_PHASE;  /* So back the start of the rigmarole */
            reset_timeout( &tv, &timeout_scratch );
            break;
          }
		  
          /* otherwise it's something else, maybe another offer, or a bogus
             NAK from someone we are not asking!
             Just listen again, which implicitly discards it */          
        }
        break;

      case DHCP_INFORM_BOUND_PHASE:

        /* All done with socket*/
        osapiSocketClose(clientSocket);
        *pConfigState = CONFIG_SET;
        return L7_TRUE;
		
      case DHCP_INFORM_FAILED_PHASE:
	  
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP INFORM FAILED phase\n");
        }

        /* All done with socket */
        osapiSocketClose(clientSocket);

        dhcpConfigSemGet();
		
        if ( strcmp(bspapiServicePortNameGet(), intf) == 0 )
        {
          if ( simGetServPortConfigMode() != L7_SYSCONFIG_MODE_DHCP )
          {
            dhcpConfigSemFree ();
            return L7_FALSE;
          }
        }
		
        else if ( strcmp(L7_DTL_PORT_IF, intf) == 0 )
        {
          if ( simGetSystemConfigMode() != L7_SYSCONFIG_MODE_DHCP )
          {
            dhcpConfigSemFree ();
            return L7_FALSE;
          }
        }
		
        dhcpConfigSemFree ();
		
        return L7_FALSE;
    }
  }  
}

/**************************************************************************
* @purpose   the DHCP state machine - this does all the work
*
* @param     L7_uchar8*     Interface name
* @param     struct bootp*    Ptr to boot structure
* @param     L7_short16*    Ptr to DHCP state
* @param     struct dhcp_lease* Ptr to lease structure
* @param   L7_short16*    Ptr to configuration state
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32
do_dhcp(const L7_uchar8 *intf, struct bootp *res,
        L7_short16 *pstate, struct dhcp_lease *lease,L7_short16* pConfigState)
{
  L7_sockaddr_in_t cli_addr, broadcast_addr, server_addr, rx_addr;
  L7_int32 s, addrlen, maxFdLength;
  L7_int32 one = 1;
  L7_uchar8 ttlValue = DHCP_IP_TTL_NUMBER;
  L7_uchar8 mincookie[] = {99,130,83,99,255} ;
  struct timeval tv;
  struct timeout_state timeout_scratch;
  L7_ushort16 oldstate = *pstate;
  L7_uchar8 msgtype = 0, seen_bootp_reply = 0;
  L7_uchar8 * macAddress[BP_CHADDR_LEN];
  struct ifreq ifr;
  L7_uint32 xid=L7_NULL;
  fd_set  pReadFd;
  L7_short16 selectCounter=0, requestCounter=0, renewCounter=0, rebindCounter=0;
  L7_uchar8 ifname[IFNAMSIZ];
  L7_uchar8 sysBIA[L7_MAC_ADDR_LEN];     /* for xid randomness */
  L7_uchar8 srvportBIA[L7_MAC_ADDR_LEN]; /* for xid randomness */
  L7_uint32       servPortConfigMode, networkConfigMode, currentIpAddr;
  L7_uint32 errorMsg = L7_NULL, ifNetMask, logMsg = L7_NULL;
  L7_uint32 bsent,brecvd;
  L7_char8   addrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 serverIp = L7_NULL;
  L7_inet_addr_t  serverAddr;
  L7_uchar8  domainNameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX + 1];
  struct in_addr   local_bp_yiaddr;     /* client IP address */
  struct in_addr   local_bp_siaddr;     /* server IP address */
  struct in_addr   dhcpSvrAddr;   

  local_bp_yiaddr.s_addr = INADDR_ANY;
  local_bp_siaddr.s_addr = INADDR_ANY;


  local_bp_yiaddr.s_addr = INADDR_ANY;
  local_bp_siaddr.s_addr = INADDR_ANY;

#ifdef _L7_OS_ECOS_
  struct sockaddr_in *addrp;
  struct ecos_rtentry route;
#endif


  /* IMPORTANT: xmit is the same as res throughout this; *received is a
     scratch buffer for reception; its contents are always copied to res
     when we are happy with them.  So we always transmit from the
     existing state.*/
  struct bootp rx_local;
  struct bootp *received = &rx_local;
  struct bootp *xmit = res;
  *pConfigState= CONFIG_NONE;
  memset(ifname, 0, IFNAMSIZ);
  memset(&dhcpSvrAddr, 0, sizeof(dhcpSvrAddr));

  inetAddressZeroSet(L7_AF_INET,&serverAddr);
  if ( strcmp(bspapiServicePortNameGet(), intf)==0 )
      sprintf(ifname, "%s0", bspapiServicePortNameGet());
  else if ( strcmp(L7_DTL_PORT_IF, intf)==0 )
      sprintf(ifname, "%s0", L7_DTL_PORT_IF);

  /* First, get a socket on the interface in question.  But Zeroth, if
     needs be, bring it to the half-up broadcast only state if needs be.*/
  if ( DHCPSTATE_INIT        == oldstate
       || DHCPSTATE_FAILED   == oldstate
       || DHCPSTATE_NOTBOUND == oldstate
       || 0                  == oldstate )
  {
    /*
    DHCPServicePortIfSet();
    */

    /*
    sysapiPrintf("DHCP intf %s, Service Port %s\n", intf, bspapiServicePortNameGet());
    */

	dhcpConfigSemGet ();
     
    if ( strcmp(bspapiServicePortNameGet(), intf)==0 )
    {
      /*
      sysapiPrintf("DHCP using Service port %s0\n", bspapiServicePortNameGet());
      */
      simSetServPortIPAddr(0,L7_FALSE);
      osapiNetIPSet(ifname, 0);

      /* If we ever run DHCP on both interfaces at the same time, then distiguish. */
      simGetServicePortBurnedInMac(&srvportBIA[0]);
      xid = (L7_uint32)res + 
            (L7_uint32) (0xffffffff & osapiUpTimeRaw()) + 
            (L7_uint32)(srvportBIA[2]<<24) + 
            (L7_uint32)(srvportBIA[3]<<16) + 
            (L7_uint32)(srvportBIA[4]<<8) + 
            (L7_uint32)(srvportBIA[5]); 
    }
    else if ( strcmp(L7_DTL_PORT_IF, intf)==0 )
    {
      /*
      sysapiPrintf("DHCP using DTL port %s0\n", L7_DTL_PORT_IF);
      */
      simSetSystemIPAddr(0,L7_FALSE);
      osapiNetIPSet(ifname, 0);
#ifndef VXWORKS
      fflush(0);
#endif

      /* If we ever run DHCP on both interfaces at the same time, then distiguish. */
      simGetSystemIPBurnedInMac(&sysBIA[0]);
      xid = (L7_uint32)res + 
            (L7_uint32) (0xffffffff & osapiUpTimeRaw()) + 
            (L7_uint32)(sysBIA[2]<<24) + 
            (L7_uint32)(sysBIA[3]<<16) + 
            (L7_uint32)(sysBIA[4]<<8) + 
            (L7_uint32)(sysBIA[5]);  
    }
	dhcpConfigSemFree ();

    sleep(5);

#ifdef _L7_OS_LINUX_
    /* This is required on linux, but harmful on VxWorks. VxWorks installs a hidden
     * route to 255.255.255.255 at startup. Adding the same route here would either fail
     * or just bump the ref count in the stack's routing table. Then if DHCP fails 
     * for example if the mgmt interface is down, removing this route will remove
     * the route installed by VxWorks itself. Then sending to 255.255.255.255 will 
     * never work again. One might wonder why you would even do a routing table lookup
     * for a broadcast packet, but there it is. */
    osapimRouteEntryAdd (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif

    *pstate = DHCPSTATE_INIT;

  }
  else
    xid = osapiNtohl(res->bp_xid); /*use what's there already*/

  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &s) != L7_SUCCESS)
  {
    LOG_MSG("do_dhcp(): Failed to create socket with domain L7_AF_INET = %d, L7_SOCK_DGRAM = %d",
            L7_AF_INET, L7_SOCK_DGRAM);

#ifdef _L7_OS_LINUX_
    osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif
    return L7_FALSE;
  }

  FD_ZERO(&pReadFd);
  FD_SET(s, &pReadFd);
  maxFdLength= s +1;

  if ( osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_REUSEADDR,(L7_char8 *) &one, sizeof(one)) == L7_FAILURE )
  {
    LOG_MSG("do_dhcp(): osapiSetsockopt SO_REUSEADDR failed");

#ifdef _L7_OS_LINUX_
    osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1);
#endif

    osapiSocketClose (s);
    return L7_FALSE;
  }

#ifdef _L7_OS_VXWORKS_
  if ( osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_BINDTODEVICE, ifname, IFNAMSIZ) )
  {
    LOG_MSG("do_dhcp(): osapiSetsockopt SO_BINDTODEVICE failed");
    osapiSocketClose (s);
    return L7_FALSE;
  }
#endif

  if ( DHCPSTATE_INIT        == oldstate
       || DHCPSTATE_FAILED   == oldstate
       || DHCPSTATE_NOTBOUND == oldstate
       || 0                  == oldstate
       || DHCPSTATE_REBINDING == oldstate )
  {
    sprintf(ifr.ifr_name,"%s%s", intf,"0");/* Add device number, zero only for 
                          first interface device of X type  */
    bzero ((caddr_t) &ifr.ifr_addr, sizeof (ifr.ifr_addr));  

    ifr.ifr_addr.sa_family = AF_INET;  

    if (osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &one, sizeof(one))== L7_FAILURE)
    {
      LOG_MSG("do_dhcp(): osapiSetsockopt SO_BROADCAST failed");

#ifdef _L7_OS_LINUX_
      osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

      osapiSocketClose (s);
      return L7_FALSE;
    }

#ifdef _L7_OS_ECOS_
    cyg_route_reinit();
    addrp = (struct sockaddr_in *) &ifr.ifr_addr;
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;

    if (ioctl(s, SIOCSIFADDR, &ifr)) {
        perror("SIOCSIFADDR");
        return false;
    }
    if (ioctl(s, SIOCSIFNETMASK, &ifr)) {
        perror("SIOCSIFNETMASK");
        return false;
    }
    if (ioctl(s, SIOCSIFADDR, &ifr)) {
        perror("SIOCSIFADDR");
        return false;
    }

    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    if (ioctl(s, SIOCSIFBRDADDR, &ifr)) {
        perror("SIOCSIFBRDADDR");
        return false;
    }
    // Set up routing
    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    memset(&route, 0, sizeof(route));
    memcpy(&route.rt_gateway, addrp, sizeof(*addrp));

    addrp->sin_family = AF_INET;
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;
    memcpy(&route.rt_dst, addrp, sizeof(*addrp));
    memcpy(&route.rt_genmask, addrp, sizeof(*addrp));

    route.rt_dev = ifr.ifr_name;
    route.rt_flags = RTF_UP|RTF_GATEWAY;
    route.rt_metric = 0;

    if (ioctl(s, SIOCADDRT, &route)) {
        if (errno != EEXIST) {
            perror("SIOCADDRT 3");
            return false;
        }
    }
#endif
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = osapiHtonl(0xffffffff);

      /* Set the mcast TTL = 255 as Cisco DHCP client does  */
      if ( osapiSetsockopt(s, IPPROTO_IP, L7_IP_MULTICAST_TTL,
                           (L7_char8 *) &ttlValue, sizeof(ttlValue)) == L7_FAILURE )
      {
        LOG_MSG("do_dhcp(): osapiSetsockopt IP_MULTICAST_TTL failed");
       
#ifdef _L7_OS_LINUX_
      osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

        osapiSocketClose (s);
        return L7_FALSE;
      }
  }

  memset((L7_char8 *) &cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family = L7_AF_INET;
  cli_addr.sin_addr.s_addr = osapiHtonl(INADDR_ANY);
  cli_addr.sin_port = osapiHtons(IPPORT_BOOTPC);

  memset((L7_char8 *) &broadcast_addr, 0, sizeof(broadcast_addr));
  broadcast_addr.sin_family = L7_AF_INET;
  broadcast_addr.sin_addr.s_addr = osapiHtonl(0xffffffff);
  broadcast_addr.sin_port = osapiHtons(IPPORT_BOOTPS);

  memset((L7_char8 *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = L7_AF_INET;
  server_addr.sin_addr.s_addr = osapiHtonl(INADDR_BROADCAST); /* overwrite later*/
  server_addr.sin_port = osapiHtons(IPPORT_BOOTPS);

  if(osapiSocketBind(s, (L7_sockaddr_t *)&cli_addr, sizeof(cli_addr)) != L7_SUCCESS)
  {
    LOG_MSG("do_dhcp(): osapiSocketBind failed");

#ifdef _L7_OS_LINUX_
      osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

    osapiSocketClose (s);
    return L7_FALSE;
  }

  /* Now, we can launch into the DHCP state machine.  I think this will
     be the neatest way to do it; it returns from within the switch arms
     when all is well, or utterly failed.*/

  reset_timeout( &tv, &timeout_scratch );

  while ( 1 )
  {
    switch ( *pstate )
    {
      
      case DHCPSTATE_INIT:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP INIT state\n");
        }

        /* need to reset when dhcp fails and retries */
        FD_ZERO(&pReadFd);
        FD_SET(s, &pReadFd);

        /* Send the DHCPDISCOVER packet*/

		dhcpConfigSemGet ();
        if ( strcmp(bspapiServicePortNameGet(), intf)==0 )
        {
          simGetServicePortBurnedInMac((L7_uchar8 *) macAddress);
        }
        else if ( strcmp(L7_DTL_PORT_IF, intf)==0 )
        {
          if ( simGetSystemIPMacType() == L7_SYSMAC_BIA )
            simGetSystemIPBurnedInMac((L7_uchar8 *) macAddress);
          else
            simGetSystemIPLocalAdminMac((L7_uchar8 *) macAddress);
        }
		dhcpConfigSemFree ();

        /* Fill in the BOOTP request - DHCPDISCOVER packet*/
        bzero((char *)xmit, sizeof(*xmit));
        xmit->bp_op = BOOTREQUEST;
        xmit->bp_htype = HTYPE_ETHERNET;
        xmit->bp_hlen = L7_MAC_ADDR_LEN;
        xmit->bp_xid = osapiHtonl(xid);
        xmit->bp_secs = 0;
        xmit->bp_flags = osapiHtons(0x8000); /* BROADCAST FLAG*/

        bcopy((L7_uchar8 *) macAddress, (L7_uchar8 *)&xmit->bp_chaddr, xmit->bp_hlen);
        bcopy(mincookie, xmit->bp_vend, sizeof(mincookie));

        /* Set DHCP DISCOVER int the packet*/
        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPDISCOVER, 1 );
        if ( previaddr.s_addr != 0 )
          /* Request the previous IP address, would like to have the same*/
          set_fixed_tag( xmit, TAG_DHCP_REQ_IP, osapiNtohl(previaddr.s_addr), 4 );

        set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );
        set_variable_tag(xmit, TAG_DHCP_CLASSID);

        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif		
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);
        
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf( "---------DHCPSTATE_INIT sending:\n" );
          show_bootp( intf, xmit );
        }   

        if ( osapiSocketSendto(s, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&broadcast_addr, sizeof(broadcast_addr),&bsent)  != L7_SUCCESS ) 
        {
          /*
          sysapiPrintf("Could not send to 0x%x\n",
		         osapiNtohl(broadcast_addr.sin_addr.s_addr));
          */
          errNo=osapiErrnoGet();
          pErrString=strerror(errNo);
          if (dhcpcDebugMsgLevel > 0)
          {
            sysapiPrintf("\nosapiSocketSendto() failed in do_dhcp() with error %d, %s.", errNo, pErrString);
          }
          *pstate = DHCPSTATE_FAILED;
          break;
        }

        seen_bootp_reply = 0;
        *pstate = DHCPSTATE_SELECTING;
        break;

      case DHCPSTATE_SELECTING:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP SELECTING state\n");
        }

        /* This is a separate state so that we can listen again
           without* retransmitting.listen for the DHCPOFFER reply*/

        addrlen = sizeof(rx_addr);

        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("DHCP SELECTING: calling select, timeout=%ld\n", tv.tv_sec);
        }

        if ( osapiSelect(maxFdLength, &pReadFd, NULL, NULL, tv.tv_sec, tv.tv_usec) <= 0 )
        {
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pstate = DHCPSTATE_FAILED;
            break;
          }
          *pstate = DHCPSTATE_INIT; /* to retransmit*/
          break;
        }

        if ( osapiSocketRecvfrom(s,(L7_char8*) received, sizeof(struct bootp), MSG_DONTWAIT,
                      ( L7_sockaddr_t *)&rx_addr, &addrlen, &brecvd)  != L7_SUCCESS )
        {
          /* No packet arrived (this time)*/
          if ( seen_bootp_reply )
          { /* then already have a bootp reply
  / Save the good packet in *xmit*/
            bcopy((L7_char8*) received ,(L7_char8*) xmit, dhcp_size(received) );
            *pstate = DHCPSTATE_BOOTP_FALLBACK;
            reset_timeout( &tv, &timeout_scratch );
            break;
          }
          /* go to the next larger timeout and re-send:*/
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pstate = DHCPSTATE_FAILED;
            break;
          }
          *pstate = DHCPSTATE_INIT; /* to retransmit*/
          break;
        }

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(rx_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_SELECTING received:\n" );
          sysapiPrintf( "...rx_addr is family %d, addr %s, port %d\n",
                  rx_addr.sin_family,
                  addrStr,
                  osapiNtohs(rx_addr.sin_port) );
          show_bootp( intf, received );
        }
            
        if ( received->bp_xid != osapiHtonl(xid) )
        {   /* not the same transaction;*/
          if ( selectCounter < 4 )
            selectCounter++;
          else
          {
            *pstate = DHCPSTATE_FAILED;
          }

          break;      /* listen again...    */
        }

        if ( 0 == received->bp_siaddr.s_addr )
        {
          /* then fill in from the options... */
          l7_get_bootp_option( received, TAG_DHCP_SERVER_ID,
                            &received->bp_siaddr.s_addr );
        }

        /* see if it was a DHCP reply or a bootp reply; it could be
                 either.                                                  */
        if ( l7_get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) )
        {
          if ( DHCPOFFER == msgtype )
          {
             /* Get subnet mask */
             if (l7_get_bootp_option( received, TAG_SUBNET_MASK, &ifNetMask ) != L7_TRUE)
                 ifNetMask = osapiHtonl(0xFFFFFF00); /* At least it's not uninitialized */
             /*
             ** Check for IP Address conflicts with other interfaces
             */
			 dhcpConfigSemGet ();
             if (sysapiIPConfigConflict((strcmp(bspapiServicePortNameGet(), intf) ? 
                                        FD_CNFGR_NIM_MIN_CPU_INTF_NUM : FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM),
                                        osapiNtohl(received->bp_yiaddr.s_addr),
                                        ifNetMask, 
                                        (L7_uint32 *)&errorMsg) == L7_FALSE)
             { 
               /* No conflict */

               /* all is well Save the good packet in *xmit  */
               bcopy( (L7_char8*)received, (L7_char8*)xmit, dhcp_size(received) );
               /* we like the packet, so reset the timeout for the next time*/
               reset_timeout( &tv, &timeout_scratch );
               /* Grab T1, T2, lease time from this packet if specified */
               get_lease_params( received, lease );
               get_tag(received, TAG_VEND_SPECIFIC);
               *pstate = DHCPSTATE_REQUESTING;
             }
             else /* IP Addr conflict */
             {
               /* Log one error per request */
               if (logMsg == L7_NULL)
               {
                 logMsg++;
                 LOG_MSG("DHCP: Conflict w/offered IP Addr, error=%u\r\n", errorMsg);
               }
             }
			 dhcpConfigSemFree ();
          }
        }
        else 
        {
          /* No TAG_DHCP_MESS_TYPE entry so it's a bootp reply*/
          seen_bootp_reply = 1; /* (keep the bootp packet in received)*/
        }

        /* If none of the above state changes occurred, we got a packet
           that "should not happen", OR we have a bootp reply in our
           hand; so listen again with the same timeout, without
           retrying the send, in the hope of getting a DHCP reply.*/
        break;

      case DHCPSTATE_REQUESTING:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP REQUESTING state\n");
        }

        /* Just send what you got with a DHCPREQUEST in the message type.
           then wait for an ACK in DHCPSTATE_REQUEST_RECV.
  
           Fill in the BOOTP request - DHCPREQUEST packet*/
        xmit->bp_op = BOOTREQUEST;
        xmit->bp_flags = osapiHtons(0x8000); /* BROADCAST FLAG*/

        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
        xmit->bp_giaddr.s_addr = 0x0;
        set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );


        /* And this will be a new one:       */
        set_fixed_tag( xmit, TAG_DHCP_REQ_IP, osapiNtohl(xmit->bp_yiaddr.s_addr), 4 );
        /* for subnet mask & default gateway @2975*/
        /* for DNS and TFTp servers IPS, config filename and hostname */
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);		
#endif
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

        set_variable_tag(xmit, TAG_DHCP_CLASSID);

        /* Per RFC 2131 Section 4.4.1 These fields should be initilized to 0 in the   **
        ** BOOTREQUEST message.    Local copies are made for validating the ACK.      */
        local_bp_yiaddr.s_addr = xmit->bp_yiaddr.s_addr;
        local_bp_siaddr.s_addr = xmit->bp_siaddr.s_addr;
        xmit->bp_ciaddr.s_addr = 0;
        xmit->bp_yiaddr.s_addr = 0;
        xmit->bp_siaddr.s_addr = 0;
        xmit->bp_giaddr.s_addr = 0;

        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf( "---------DHCPSTATE_REQUESTING sending:\n" );
          show_bootp( intf, xmit );
        }
        
        if ( osapiSocketSendto(s, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&broadcast_addr, sizeof(broadcast_addr),&bsent)  != L7_SUCCESS )

        {
          *pstate = DHCPSTATE_FAILED;
          break;
        }


        *pstate = DHCPSTATE_REQUEST_RECV;
        break;

      case DHCPSTATE_REQUEST_RECV:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP REQUEST RECV state\n");
        }

        /* wait for an ACK or a NACK - retry by going back to
           DHCPSTATE_REQUESTING; NACK means go back to INIT.*/

        addrlen = sizeof(rx_addr);

        if ( osapiSelect(maxFdLength, &pReadFd, NULL, NULL, tv.tv_sec, tv.tv_usec) <= 0 )
        {
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pstate = DHCPSTATE_FAILED;
            break;
          }
          *pstate = DHCPSTATE_REQUESTING;
          break;
        }

        if ( osapiSocketRecvfrom(s,(L7_char8*) received, sizeof(struct bootp), MSG_DONTWAIT,
                      ( L7_sockaddr_t *)&rx_addr, &addrlen, &brecvd)  != L7_SUCCESS )

        {
          /* No packet arrived
             go to the next larger timeout and re-send:*/
          if ( ! next_timeout( &tv, &timeout_scratch ) )
          {
            *pstate = DHCPSTATE_FAILED;
            break;
          }
          *pstate = DHCPSTATE_REQUESTING;
          break;
        }

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(rx_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_REQUEST_RECV received:\n" );
          sysapiPrintf( "...rx_addr is family %d, addr %s, port %d\n",
                  rx_addr.sin_family,
                  addrStr,
                  osapiNtohs(rx_addr.sin_port) );
          show_bootp( intf, received );
        }
        
        if ( received->bp_xid != osapiHtonl(xid) )
        {   /* not the same transaction;*/
          if ( requestCounter < 4 )
            requestCounter++;
          else
          {
            *pstate = DHCPSTATE_FAILED;
          }
          break;                  /* listen again...*/
        }

        if ( 0 == received->bp_siaddr.s_addr )
        {
          /* We MUST NOT fill TAG_DHCP_SERVER_ID in received->bp_siaddr.s_addr,
             because here DHCP resquesting process ends and all parameters should be returned as they are */
          l7_get_bootp_option( received, TAG_DHCP_SERVER_ID, &(dhcpSvrAddr.s_addr) );
        }
        else
        {
          dhcpSvrAddr.s_addr = received->bp_siaddr.s_addr;
        }

        /* check if it was a DHCP reply*/
        if ( l7_get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) )
        {
          if ( DHCPACK == msgtype /* Same offer & server? */
               && received->bp_yiaddr.s_addr == local_bp_yiaddr.s_addr
               && dhcpSvrAddr.s_addr == local_bp_siaddr.s_addr )
          {
#ifdef FEAT_METRO_CPE_V1_0
            bootpDhcpServerAddress = dhcpSvrAddr.s_addr;
#endif
            /* ACK packet might not have non-zero 'siaddr' from some SERVERs. So pick it up
             * from the DHCP SERVER ID option instead */
            l7_get_bootp_option( received, TAG_DHCP_SERVER_ID, &received->bp_siaddr.s_addr );

            /* Save the good packet in *xmit*/
            bcopy( (L7_char8*)received,(L7_char8*) xmit, dhcp_size(received) );
            /* we like the packet, so reset the timeout for next time*/
            reset_timeout( &tv, &timeout_scratch );
            /* Record the new lease and set up timers &c*/
            get_lease_params( received, lease );
            get_tag(received, TAG_VEND_SPECIFIC);
            new_lease( lease );
            if (l7_get_bootp_option(received, TAG_DOMAIN_SERVER, &serverIp))
            {
              /* if domain name sever option is found in response,
               * add it dns name server list. 
               */
              inetAddressSet(L7_AF_INET, &serverIp, &serverAddr);
              if (dnsClientNameServerEntryAdd(&serverAddr) != L7_SUCCESS)
              {
                LOG_MSG("\n\rdo_dhcp(): DNS name server entry add failed.");
              }
            }
            if (l7_get_bootp_option(received, TAG_DOMAIN_NAME, domainNameList))
            {
              /* if domain name List option is found in response,
               * add it to dns domain name List. 
               */
              if (dnsClientDomainNameListAdd(domainNameList) != L7_SUCCESS)
              {
                LOG_MSG("\n\rdo_dhcp(): DNS domain name"
                                        "list entry add failed.");
              }
            }
            *pstate = DHCPSTATE_BOUND;
            break;
          }
          if ( DHCPNAK == msgtype /* Same server?*/
               && dhcpSvrAddr.s_addr == xmit->bp_siaddr.s_addr )
          {
            /* we're bounced!*/
            *pstate = DHCPSTATE_INIT;  /* So back the start of the rigmarole.*/
            reset_timeout( &tv, &timeout_scratch );
            break;
          }
          /* otherwise it's something else, maybe another offer, or a bogus
             NAK from someone we are not asking!
             Just listen again, which implicitly discards it.*/
        }
        break;

      case DHCPSTATE_BOUND:
        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(xmit->bp_yiaddr.s_addr), addrStr);
          sysapiPrintf("\nDHCP BOUND state: addr %s\n", addrStr);
        }

        /* We are happy now, we have our address.*/
        /* So save this address for requesting the same IP address again*/
        previaddr.s_addr = xmit->bp_yiaddr.s_addr;

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif
        /* All done with socket*/
        osapiSocketClose(s);
        *pConfigState= CONFIG_SET;
        return L7_TRUE;


      case DHCPSTATE_RENEWING:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP RENEWING state\n");
        }

        /* Just send what you got with a DHCPREQUEST in the message
           type UNICAST straight to the server.  Then wait for an ACK.*/

        /* Fill in the BOOTP request - DHCPREQUEST packet*/
        xmit->bp_op = BOOTREQUEST;
        xmit->bp_flags = osapiHtons(0); /* No BROADCAST FLAG*/
        /* Use the *client* address here:*/
        xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
        set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

        /*as per RFC, these fields MUST NOT be present in DHCP REQUEST*/
        remove_tag( xmit, TAG_DHCP_SERVER_ID);
        remove_tag( xmit, TAG_DHCP_REQ_IP);

        /* @2975 - Request that the DHCP server give us a default gateway
               if it knows about one - to be nice to users of 
         MS Windows NT 3.5/4.0 DHCP servers, per MSKB Q183199 */
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif		
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

        set_variable_tag(xmit, TAG_DHCP_CLASSID);

        /* Set unicast address to *server* */
        server_addr.sin_addr.s_addr = res->bp_siaddr.s_addr;

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(server_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_RENEWING sending:\n" );
          sysapiPrintf( "UNICAST to family %d, addr %s, port %d\n",
                  server_addr.sin_family,
                  addrStr,
                  osapiNtohs(server_addr.sin_port) );
          show_bootp( intf, xmit );
        }        

                    /* UNICAST address of the server:*/
        if ( osapiSocketSendto(s, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&server_addr, sizeof(server_addr),&bsent)  != L7_SUCCESS )

        {
          *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

          osapiSocketClose (s);
          return L7_TRUE;
        }

        *pstate = DHCPSTATE_RENEW_RECV;
        break;

      case DHCPSTATE_RENEW_RECV:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP RENEW RECV state\n");
        }

        /* wait for an ACK or a NACK - retry by going back to
           DHCPSTATE_RENEWING; NACK means go to NOTBOUND. */

        addrlen = sizeof(rx_addr);

        tv.tv_sec = ((lease->t2) > 50000) ? 25 : ((lease->t2)/1000)/2;
        tv.tv_usec = 0;
        
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf( "---------DHCPSTATE_RENEW_RECV setup:\n" );
          sysapiPrintf( "wait time is %ld\n", tv.tv_sec);
        }
    
        if ( osapiSelect(maxFdLength, &pReadFd, NULL, NULL, tv.tv_sec, tv.tv_usec) <= 0 )
        {
          reset_timeout( &tv, &timeout_scratch );
          *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

          osapiSocketClose (s);
          return L7_TRUE;
        }

        if ( osapiSocketRecvfrom(s,(L7_char8*) received, sizeof(struct bootp), MSG_DONTWAIT,
                      ( L7_sockaddr_t *)&rx_addr, &addrlen, &brecvd)  != L7_SUCCESS )

        {
          /* No packet arrived wait for next time out */
          reset_timeout( &tv, &timeout_scratch );
          *pstate = DHCPSTATE_BOUND;


#ifdef _L7_OS_LINUX_
          osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

          osapiSocketClose (s);
          return L7_TRUE;
        }

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(rx_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_RENEW_RECV received:\n" );
          sysapiPrintf( "...rx_addr is family %d, addr %s, port %d\n",
                  rx_addr.sin_family,
                  addrStr,
                  osapiNtohs(rx_addr.sin_port) );
          show_bootp( intf, received );
        }
        
        if ( received->bp_xid != osapiHtonl(xid) )
        {   /* not the same transaction;*/
          if ( renewCounter < 4 )
            renewCounter++;     /* listen again... */
          else
          {
            reset_timeout( &tv, &timeout_scratch );
            *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
        osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif
            osapiSocketClose (s);
            return L7_TRUE;
          }
          break;                  
        }

        if ( 0 == received->bp_siaddr.s_addr )
        {
          /* then fill in from the options... */
          l7_get_bootp_option( received, TAG_DHCP_SERVER_ID,
                            &received->bp_siaddr.s_addr );
        }

        /* check it was a DHCP reply*/
        if ( l7_get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) )
        {
          if ( DHCPACK == msgtype  /* Same offer? */
               && received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr )
          {
            /* Save the good packet in *xmit */
            bcopy( (L7_uchar8*)received, (L7_uchar8*)xmit, dhcp_size(received) );
            /* we like the packet, so reset the timeout for next time */
            reset_timeout( &tv, &timeout_scratch );
            /* Record the new lease and set up timers &c */
            get_lease_params( received, lease );
            new_lease( lease );
            get_tag(received, TAG_VEND_SPECIFIC);
            *pstate = DHCPSTATE_BOUND;
            break;
          }
          if ( DHCPNAK == msgtype )
          { /* we're bounced! */
            *pstate = DHCPSTATE_NOTBOUND;  /* So quit out. */
            reset_timeout( &tv, &timeout_scratch );
            break;
          }
          /* otherwise it's something else, maybe another offer.
             Just listen again, which implicitly discards it.*/
        }
        break;

      case DHCPSTATE_REBINDING:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP REBINDING state\n");
        }

        /* Just send what you got with a DHCPREQUEST in the message type.
           Then wait for an ACK.  This one is BROADCAST.
  
           Fill in the BOOTP request - DHCPREQUEST packet */
        xmit->bp_op = BOOTREQUEST;
        xmit->bp_flags = osapiHtons(0); /* no BROADCAST FLAG*/
        /* Use the *client* address here: */
        xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPREQUEST, 1 );
        set_fixed_tag( xmit, TAG_DHCP_MAX_MSGSZ, BP_MINPKTSZ, 2 );

        /*as per RFC, these fields MUST NOT be present in DHCP REQUEST*/
        remove_tag( xmit, TAG_DHCP_SERVER_ID);
        remove_tag( xmit, TAG_DHCP_REQ_IP);

        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_BOOT_FILENAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_GATEWAY);
#ifndef L7_PRODUCT_SMARTPATH
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_DOMAIN_SERVER);
#endif		
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_TFTP_SERVER_NAME);
        add_fixed_tag( xmit, TAG_DHCP_PARM_REQ_LIST, TAG_SUBNET_MASK);

        set_variable_tag(xmit, TAG_DHCP_CLASSID);

        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf( "---------DHCPSTATE_REBINDING sending:\n" );
          sysapiPrintf( intf, xmit );
        }
   
        if ( osapiSocketSendto(s, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&broadcast_addr, sizeof(broadcast_addr),&bsent)  != L7_SUCCESS )
        {
          *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif
          osapiSocketClose (s);
          return L7_TRUE;
        }

        *pstate = DHCPSTATE_REBIND_RECV;
        break;

      case DHCPSTATE_REBIND_RECV:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP REBIND RECV state\n");
        }

        /* wait for an ACK or a NACK - retry by going back to
           DHCPSTATE_REBINDING; NACK means go to NOTBOUND. */

        addrlen = sizeof(rx_addr);

        tv.tv_sec = ((lease->expiry) > 50000) ? 25 : ((lease->expiry)/1000)/2;
        tv.tv_usec = 0;
        
        if ( osapiSelect(maxFdLength, &pReadFd, NULL, NULL, tv.tv_sec, tv.tv_usec) <= 0 )
        {
          reset_timeout( &tv, &timeout_scratch );
          *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
        osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

          osapiSocketClose (s);
          return L7_TRUE;
        }

        if ( osapiSocketRecvfrom(s,(L7_char8*) received, sizeof(struct bootp), MSG_DONTWAIT,
                      ( L7_sockaddr_t *)&rx_addr, &addrlen, &brecvd)  != L7_SUCCESS )

        {
          /* No packet arrived wait for next time out */
          reset_timeout( &tv, &timeout_scratch );
          *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
          osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

          osapiSocketClose (s);
          return L7_TRUE;
        }

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(rx_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_REBIND_RECV received:\n" );
          sysapiPrintf( "...rx_addr is family %d, addr %s, port %d\n",
                  rx_addr.sin_family,
                  addrStr,
                  osapiNtohs(rx_addr.sin_port) );
          show_bootp( intf, received );
        }
        
        if ( received->bp_xid != osapiHtonl(xid) )
        {   /* not the same transaction;    */
          if ( rebindCounter < 4 )
            rebindCounter++;      /* listen again... */
          else
          {
            reset_timeout( &tv, &timeout_scratch );
            *pstate = DHCPSTATE_BOUND;

#ifdef _L7_OS_LINUX_
        osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

            osapiSocketClose (s);
            return L7_TRUE;
          }
          break;                  
        }

        if ( 0 == received->bp_siaddr.s_addr )
        {
          /* then fill in from the options...    */
          l7_get_bootp_option( received, TAG_DHCP_SERVER_ID,
                            &received->bp_siaddr.s_addr );
        }

        /* check it was a DHCP reply */
        if ( l7_get_bootp_option( received, TAG_DHCP_MESS_TYPE, &msgtype ) )
        {
          if ( DHCPACK == msgtype  /* Same offer?*/
               && received->bp_yiaddr.s_addr == xmit->bp_yiaddr.s_addr )
          {
            /* Save the good packet in *xmit   */
            bcopy((char *)received, (L7_uchar8*)xmit, dhcp_size(received) );
            /* we like the packet, so reset the timeout for next time */
            reset_timeout( &tv, &timeout_scratch );
            /* Record the new lease and set up timers &c  */
            get_lease_params( received, lease );
            new_lease( lease );
            get_tag(received, TAG_VEND_SPECIFIC);
            *pstate = DHCPSTATE_BOUND;
            *pConfigState= CONFIG_SET;
            break;
          }
          else if ( DHCPNAK == msgtype )
          { /* we're bounced! */
            *pstate = DHCPSTATE_NOTBOUND;  /* So back the start of the rigmarole.*/
            reset_timeout( &tv, &timeout_scratch );
            break;
          }
          /* otherwise it's something else, maybe another offer.
             Just listen again, which implicitly discards it.*/
        }
        break;

      case DHCPSTATE_BOOTP_FALLBACK:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP BOOTP FALLBACK state\n");
        }

        /* All done with socket*/
        osapiSocketClose(s);
        /* And no lease should have become active, but JIC*/
        no_lease( lease );
        *pstate = DHCPSTATE_FAILED;
        break;


      case DHCPSTATE_NOTBOUND:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP NOTBOUND state\n");
        }

        /* All done with socket*/
        osapiSocketClose(s);
        /* No lease active        */
        no_lease( lease );
        /* Bring down the interface*/
        *pstate = DHCPSTATE_FAILED;
        break;


      case DHCPSTATE_FAILED:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP FAILED state\n");
        }

        /* All done with socket */
        osapiSocketClose(s);
        /* No lease active*/
        no_lease( lease );

		dhcpConfigSemGet ();
        if ( strcmp(bspapiServicePortNameGet(), intf) == 0 )
        {
          servPortConfigMode = simGetServPortConfigMode();
          if ( servPortConfigMode != L7_SYSCONFIG_MODE_DHCP )
		  {
			  dhcpConfigSemFree ();
			  
 
#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

			  return L7_FALSE;
		  }

          currentIpAddr = simGetServPortIPAddr();
          if ( currentIpAddr != L7_NULL )
          {
              simSetServPortIPAddr(0,L7_FALSE);
          }
          simSetServPortIPNetMask(L7_NULL, L7_FALSE);
          simSetServPortIPGateway(L7_NULL, L7_FALSE);
        }
        else if ( strcmp(L7_DTL_PORT_IF, intf) == 0 )
        {
          networkConfigMode = simGetSystemConfigMode();
          if ( networkConfigMode != L7_SYSCONFIG_MODE_DHCP )
		  {
			  dhcpConfigSemFree ();
         
#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif
			  return L7_FALSE;
		  }

          currentIpAddr = simGetSystemIPAddr();
          if ( currentIpAddr != L7_NULL )
            {
              simSetSystemIPAddr(0,L7_FALSE);
          }
          simSetSystemIPNetMask(L7_NULL);
          simSetSystemIPGateway(L7_NULL, L7_FALSE);
        }
		dhcpConfigSemFree ();

        /* Unconditionally down the interface.*/
        do_dhcp_down_net( intf, pstate);

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

        return L7_FALSE;

      case DHCPSTATE_DO_RELEASE:
        if (dhcpcDebugMsgLevel > 0)
        {
          sysapiPrintf("\nDHCP DO RELEASE state\n");
        }

        /* We have been forced here by external means, to release the
           lease for graceful shutdown.
  
           Just send what you got with a DHCPRELEASE in the message
           type UNICAST straight to the server.  No ACK.  Then go to
           NOTBOUND state.*/

        xmit->bp_op = BOOTREQUEST;
        xmit->bp_flags = osapiHtons(0); /* no BROADCAST FLAG*/
        /* Use the *client* address here: */
        xmit->bp_ciaddr.s_addr = xmit->bp_yiaddr.s_addr;

        set_fixed_tag( xmit, TAG_DHCP_MESS_TYPE, DHCPRELEASE, 1 );

        /* Set unicast address to *server*  */
        server_addr.sin_addr.s_addr = res->bp_siaddr.s_addr;

        if (dhcpcDebugMsgLevel > 0)
        {
          memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
          osapiInetNtoa(osapiNtohl(server_addr.sin_addr.s_addr), addrStr);
          sysapiPrintf( "---------DHCPSTATE_DO_RELEASE sending:\n" );
          sysapiPrintf( "UNICAST to family %d, addr %s, port %d\n",
                  server_addr.sin_family,
                  addrStr,
                  osapiNtohs(server_addr.sin_port) );
          show_bootp( intf, xmit );                                 
        }

                    /* UNICAST address of the server:*/
        if ( osapiSocketSendto(s, (L7_char8 *)xmit, dhcp_size(xmit), 0, 
                    (L7_sockaddr_t *)&server_addr, sizeof(server_addr),&bsent)  != L7_SUCCESS )

        {
          *pstate = DHCPSTATE_FAILED;
          break;
        }
        if (l7_get_bootp_option(xmit, TAG_DOMAIN_SERVER, &serverIp))
        {
          /* if domain name sever option is found in response,
           * delete from dns name server list. 
           */
          inetAddressSet(L7_AF_INET, &serverIp, &serverAddr);
          if (dnsClientNameServerEntryRemove(&serverAddr) != L7_SUCCESS)
          {
            LOG_MSG("\n\rdo_dhcp(): DNS name server entry delete failed.");
          }
        }
        if (l7_get_bootp_option(xmit, TAG_DOMAIN_NAME, domainNameList))
        {
          /* if domain name List option is found in response,
           * delete from dns domain name List. 
           */
          if (dnsClientDomainNameListRemove(domainNameList) != L7_SUCCESS)
          {
            LOG_MSG("\n\rdo_dhcp(): DNS domain name"
                                    "list entry delete failed.");
          }
        }

        *pstate = DHCPSTATE_NOTBOUND;
        break;

      default:
        LOG_MSG("DHCP default state -- should not be here\n");
        no_lease( lease );

#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

        osapiSocketClose(s);
        return L7_FALSE;
    }
  }
  /* NOTREACHED */

 
#ifdef _L7_OS_LINUX_
       osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff, 0, 0, 1); 
#endif

  osapiSocketClose (s);
  return L7_FALSE;
}


/* ------------------------------------------------------------------------
   Bring an interface down, failed to initialize it or lease is expired
   Also part of normal startup, bring down for proper reinitialization*/


/**************************************************************************
* @purpose  Bring down the interface by deleting IP interface 
*
* @param    L7_char8*      Interface Name
* @param    L7_short16*    Ptr to DHCP state
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32
do_dhcp_down_net(const L7_char8 *intf, L7_short16 *pstate)
{
  L7_sockaddr_in_t *addrp;
  struct ifreq ifr;
  L7_char8   addrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8   ifname[IFNAMSIZ];
  L7_sockaddr_in_t *addrInt;
  L7_int32   s;

  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &s) == L7_FAILURE)
  {
    LOG_MSG("do_dhcp_down_net(): socket creation failed");
    return L7_FALSE;
  }

  addrp = (L7_sockaddr_in_t *) &ifr.ifr_addr;

  sprintf(ifname,"%s%s",intf, "0");
  addrInt= (L7_sockaddr_in_t *)(&ifr.ifr_addr);

  /* Remove any existing address*/
  if ( DHCPSTATE_FAILED   == *pstate
       || DHCPSTATE_INIT    == *pstate
       || DHCPSTATE_NOTBOUND  == *pstate
       || 0         == *pstate )
  {
    /* it was configured for broadcast only, "half-up"*/
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;

#ifdef _L7_OS_VXWORKS_
    addrp->sin_len = sizeof(*addrp);
#endif /* _L7_OS_VXWORKS_ */
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = osapiHtonl(INADDR_ANY);
  }
  else
  {
    /* get the specific address that was used*/
    if ( osapiNetIPGetAddr(ifr.ifr_name, (L7_uint32 *)&addrInt->sin_addr.s_addr) != L7_SUCCESS )
    {
      perror("SIOCGIFADDR 1");
      osapiSocketClose (s);
      return L7_FALSE;
    }
  }

  /*Delete route entries for this interface */

  addrInt= (L7_sockaddr_in_t *)(&ifr.ifr_addr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(addrInt->sin_addr.s_addr), addrStr);

  /* All done with socket*/
  osapiSocketClose(s);

  return L7_TRUE;
}


/**************************************************************************
* @purpose  Send DHCPRELEASE to dhcp server 
*
* @param    L7_char8*      Interface Name
* @param    struct bootp*   Ptr to bootp structure
* @param    L7_short16*    Ptr to DHCP state
* @param    struct dhcp_lease*  Ptr to lease structure
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32
do_dhcp_release(const L7_char8 *intf, struct bootp *res,
                L7_short16 *pstate, struct dhcp_lease *lease)
{
  L7_short16 configState;
  configState=CONFIG_NONE;
  if ( 0                           != *pstate
       && DHCPSTATE_INIT           != *pstate
       && DHCPSTATE_NOTBOUND       != *pstate
       && DHCPSTATE_FAILED         != *pstate
       && DHCPSTATE_BOOTP_FALLBACK != *pstate )
  {
    *pstate = DHCPSTATE_DO_RELEASE;
    do_dhcp( intf, res, pstate, lease, &configState); /* to send the release packet*/
    osapiSleep( 1 );             /* to let it leave the building  */
  }
  return L7_TRUE;
}


/* Following functions have been copied from Redhat/eCos bootp_support.c */

static L7_char8 *_bootp_op[] = {"", "REQUEST", "REPLY"};
static L7_char8 *_bootp_hw_type[] = {"", "Ethernet", "Exp Ethernet", "AX25",
  "Pronet", "Chaos", "IEEE802", "Arcnet"};

static L7_char8 *_dhcpmsgs[] = {"","DISCOVER", "OFFER", "REQUEST", "DECLINE",
  "ACK", "NAK", "RELEASE"};


/**************************************************************************
* @purpose  To show bootp parameters
*
* @param    L7_char8*      Interface Name
* @param    L7_short16*    Ptr to bootp structure
*
* @returns  Void
*
* @comments None.
*
* @end
*************************************************************************/
void
show_bootp(const L7_char8 *intf, struct bootp *bp)
{
  L7_int32  i, len;
  L7_uchar8 *op, *ap = 0, optover;
  L7_uchar8 name[128];
  struct    in_addr addr[32];
  L7_char8  addrStr[OSAPI_INET_NTOA_BUF_SIZE];

  sysapiPrintf("BOOTP[%s] op: %s\n", intf, _bootp_op[bp->bp_op]);
  sysapiPrintf("       htype: %s\n", _bootp_hw_type[bp->bp_htype]);
  sysapiPrintf("        hlen: %d\n", bp->bp_hlen );
  sysapiPrintf("        hops: %d\n", bp->bp_hops );
  sysapiPrintf("         xid: 0x%lx\n", osapiNtohl(bp->bp_xid) );
  sysapiPrintf("        secs: %d\n", bp->bp_secs );
  sysapiPrintf("       flags: 0x%x\n", bp->bp_flags );
  sysapiPrintf("       hw_addr: ");
  for ( i = 0;  i < bp->bp_hlen;  i++ )
  {
    sysapiPrintf("%02x", bp->bp_chaddr[i]);
    if ( i != (bp->bp_hlen-1) ) sysapiPrintf(":");
  }
  sysapiPrintf("\n");

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_ciaddr.s_addr), addrStr);
  sysapiPrintf("     client IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_yiaddr.s_addr), addrStr);
  sysapiPrintf("         my IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_siaddr.s_addr), addrStr);
  sysapiPrintf("     server IP: %s\n", addrStr);

  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  osapiInetNtoa(osapiNtohl(bp->bp_giaddr.s_addr), addrStr);
  sysapiPrintf("    gateway IP: %s\n", addrStr);

  optover = 0; /* See whether sname and file are overridden for options*/
  (void)l7_get_bootp_option( bp, TAG_DHCP_OPTOVER, &optover );
  if ( !(1 & optover) && bp->bp_sname[0] )
    sysapiPrintf("        server: %s\n", bp->bp_sname);
  if ( ! (2 & optover) && bp->bp_file[0] )
    sysapiPrintf("          file: %s\n", bp->bp_file);
  if ( bp->bp_vend[0] )
  {
    sysapiPrintf("  options:\n");
    op = &bp->bp_vend[4];
    while ( *op != TAG_END )
    {
      switch ( *op )
      {
        case TAG_SUBNET_MASK:
        case TAG_GATEWAY:
        case TAG_IP_BROADCAST:
        case TAG_DOMAIN_SERVER:
          ap = (L7_uchar8 *)&addr[0];
          len = *(op+1);
          for ( i = 0;  i < len;  i++ )
          {
            *ap++ = *(op+i+2);
          }
          if ( *op == TAG_SUBNET_MASK )   ap =  "  subnet mask";
          if ( *op == TAG_GATEWAY )       ap =  "      gateway";
          if ( *op == TAG_IP_BROADCAST )  ap =  " IP broadcast";
          if ( *op == TAG_DOMAIN_SERVER ) ap =  "domain server";
          sysapiPrintf("      %s: ", ap);
          ap = (L7_uchar8 *)&addr[0];
          while ( len > 0 )
          {
            memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
            osapiInetNtoa(osapiNtohl((L7_uint32)(((struct in_addr *)ap)->s_addr)),
                          addrStr);
            sysapiPrintf("%s", addrStr);
            len -= sizeof(struct in_addr);
            ap += sizeof(struct in_addr);
            if ( len ) sysapiPrintf(", ");
          }
          sysapiPrintf("\n");
          break;
        case TAG_DOMAIN_NAME:
        case TAG_HOST_NAME:
          for ( i = 0;  i < *(op+1);  i++ )
          {
            name[i] = *(op+i+2);
          }
          name[*(op+1)] = '\0';
          if ( *op == TAG_DOMAIN_NAME ) ap =  " domain name";
          if ( *op == TAG_HOST_NAME )   ap =  "   host name";
          sysapiPrintf("       %s: %s\n", ap, name);
          break;
        case TAG_DHCP_MESS_TYPE:
          sysapiPrintf("        DHCP message: %d %s\n",
                  op[2], _dhcpmsgs[op[2]] );
          break;
        case TAG_DHCP_REQ_IP:
          sysapiPrintf("        DHCP requested ip: %d.%d.%d.%d\n",
                  op[2], op[3], op[4], op[5] );  
          break;
        case TAG_DHCP_LEASE_TIME   :
        case TAG_DHCP_RENEWAL_TIME :
        case TAG_DHCP_REBIND_TIME  :
          sysapiPrintf("        DHCP time %d: %d\n",
                  *op, ((((((op[2]<<8)+op[3])<<8)+op[4])<<8)+op[5]) );

          break;
        case TAG_DHCP_SERVER_ID    :
          sysapiPrintf("        DHCP server id: %d.%d.%d.%d\n",
                  op[2], op[3], op[4], op[5] );  
          break;

        case TAG_DHCP_OPTOVER      :
        case TAG_DHCP_PARM_REQ_LIST:
        case TAG_DHCP_TEXT_MESSAGE :
        case TAG_DHCP_MAX_MSGSZ    :
        case TAG_DHCP_CLASSID      :
        case TAG_DHCP_CLIENTID     :
          sysapiPrintf("        DHCP option: %x/%d.%d:", *op, *op, *(op+1));
          if ( 1 == op[1] )
          {
            sysapiPrintf( " %d", op[2] );
          }
          else if ( 2 == op[1] )
          {
            sysapiPrintf( " %d", (op[2]<<8) + op[3] );
          }
          else if ( 4 == op[1] )
          {
            sysapiPrintf( " %d", ((((((op[2]<<8)+op[3])<<8)+op[4])<<8)+op[5]) );
          }
          else
          {
            for ( i = 2; i < 2 + op[1]; i++ )
              sysapiPrintf(" %d",op[i]);
          }
          sysapiPrintf("\n");
          break;

        default:
          sysapiPrintf("        Unknown option: %x/%d.%d:", *op, *op, *(op+1));
          for ( i = 2; i < 2 + op[1]; i++ )
            sysapiPrintf(" %d",op[i]);
          sysapiPrintf("\n");
          break;
      }                
      /*temp op += *(op+1)+2; */
      op += *(op+1)+2;
    }
  }
}

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
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments None.
*
* @end
*************************************************************************/
L7_short16
l7_get_bootp_option(struct bootp *bp, L7_uchar8 tag, void *opt)
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
      for ( i = 0;  i < *(op+1);  i++ )
      {
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

  if ( l7_get_bootp_option( bp, TAG_DHCP_OPTOVER, &optover ) == L7_TRUE )
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
* @purpose  To set DHCP client debug level
*
* @param    level   Debug level
* 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
L7_uint32
dhcpcDebugMsgLvlSet( L7_uint32 level )
{
  dhcpcDebugMsgLevel = level;

  return L7_SUCCESS;
}

/* EOF dhcp_prot.c*/


