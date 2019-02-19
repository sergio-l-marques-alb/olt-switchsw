/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename doscontrol_exports.h
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

#ifndef __DOS_CONTROL_EXPORTS_H_
#define __DOS_CONTROL_EXPORTS_H_


typedef enum
{
    DOSCONTROL_SIPDIP,
    DOSCONTROL_SMACDMAC,
    DOSCONTROL_FIRSTFRAG,
    DOSCONTROL_TCPFRAG,
    DOSCONTROL_TCPFLAG,
    DOSCONTROL_TCPFLAGSEQ,
    DOSCONTROL_L4PORT,
    DOSCONTROL_ICMP,
    DOSCONTROL_ICMPV6,
    DOSCONTROL_ICMPFRAG,
    DOSCONTROL_TCPFINURGPSH,
    DOSCONTROL_TCPOFFSET,
    DOSCONTROL_TCPSYNFIN,
    DOSCONTROL_TCPSYN,
    DOSCONTROL_TCPPORT,
    DOSCONTROL_UDPPORT,
    DOSCONTROL_PINGFLOODING,
    DOSCONTROL_SMURFATTACK,
    DOSCONTROL_SYNACKFLOODING,    
    DOSCONTROL_MAX

} L7_DOSCONTROL_TYPE;

typedef enum
{
  L7_DOSCONTROL_FEATURE_SUPPORTED = 0,        /* general support statement */
  L7_DOSCONTROL_SIPDIP_FEATURE_ID,
   L7_DOSCONTROL_SMACDMAC_FEATURE_ID,
  L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID,
  L7_DOSCONTROL_TCPFRAG_FEATURE_ID,
  L7_DOSCONTROL_TCPFLAG_FEATURE_ID,
   L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID,
  L7_DOSCONTROL_L4PORT_FEATURE_ID,
  L7_DOSCONTROL_ICMP_FEATURE_ID,
   L7_DOSCONTROL_ICMPV4_FEATURE_ID,
   L7_DOSCONTROL_ICMPV6_FEATURE_ID,
   L7_DOSCONTROL_ICMPFRAG_FEATURE_ID,
   L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID,
   L7_DOSCONTROL_TCPOFFSET_FEATURE_ID,
   L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID,
   L7_DOSCONTROL_TCPSYN_FEATURE_ID,
   L7_DOSCONTROL_TCPPORT_FEATURE_ID,
   L7_DOSCONTROL_UDPPORT_FEATURE_ID,
  L7_DOSCONTROL_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_DOSCONTROL_FEATURE_IDS_t;




/******************** conditional Override *****************************/

#ifdef INCLUDE_DOSCONTROL_EXPORTS_OVERRIDES
#include "doscontrol_exports_overrides.h"
#endif


#endif /* __DOS_CONTROL_EXPORTS_H_*/
