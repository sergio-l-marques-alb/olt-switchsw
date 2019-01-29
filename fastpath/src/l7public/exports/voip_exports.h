/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __VOIP_EXPORTS_H_
#define __VOIP_EXPORTS_H_

typedef enum
{
  L7_QOS_VOIP_PROTOCOL_SIP=1,
  L7_QOS_VOIP_PROTOCOL_MGCP,
  L7_QOS_VOIP_PROTOCOL_SCCP,
  L7_QOS_VOIP_PROTOCOL_H323,
  L7_QOS_VOIP_PROTOCOL_H245,
  L7_QOS_VOIP_FEATURE_ID_TOTAL
} L7_QOS_VOIP_PROTOCOL_t;


/*--------------------------------------*/
/*  VOIP Constants                      */
/*--------------------------------------*/

#define VOIPBANDWIDTH_0       0
#define VOIPBANDWIDTH_64      64
#define VOIPBANDWIDTH_128     128
#define VOIPBANDWIDTH_256     256
#define VOIPBANDWIDTH_512     512
#define VOIPBANDWIDTH_1024    1024

typedef enum 
{
  VOIP_PROTO_H323 = 0,
  VOIP_PROTO_MGCP,
  VOIP_PROTO_SCCP,
  VOIP_PROTO_SIP,
  VOIP_PROTO_ALL,
} voipProtocol_t;


#define L7_VOIP_MAX_CALLS          PROD_MAX_VOIP_CALLS

#define L7_SIP_L4_PORT    0x13C4
#define L7_SCCP_L4_PORT   0x07d0
#define L7_MGCP_L4_PORT   0x0AA7
#define L7_H323_L4_PORT   0x06B8


/******************** conditional Override *****************************/

#ifdef INCLUDE_VOIP_EXPORTS_OVERRIDES
#include "voip_exports_overrides.h"
#endif

#endif /* __VOIP_EXPORTS_H_*/
