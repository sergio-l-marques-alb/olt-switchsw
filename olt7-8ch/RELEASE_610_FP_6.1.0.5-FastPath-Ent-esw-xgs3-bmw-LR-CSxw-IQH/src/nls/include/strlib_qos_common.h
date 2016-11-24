/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_qos_common.h                                                      
*                                                                     
* @purpose    Strings Library                                                      
*                                                                     
* @component  Common Strings Library                                                      
*                                                                     
* @comments   none                                                    
*                                                                     
* @create     01/10/2007                                                      
*                                                                     
* @author     Rama Sasthri, Kristipati                                
*                                                                     
* @end                                                                
*                                                                     
*********************************************************************/

#ifndef STRLIB_QOS_COMMON_C
#define STRLIB_QOS_COMMON_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_qos_Any;
extern L7_char8 *pStrInfo_qos_AsSignQueue;
extern L7_char8 *pStrInfo_qos_ClassName_1;
extern L7_char8 *pStrInfo_qos_ClassType;
extern L7_char8 *pStrInfo_qos_ClassOfService;
extern L7_char8 *pStrInfo_qos_CosVal_1;
extern L7_char8 *pStrInfo_qos_ConformAction;
extern L7_char8 *pStrInfo_qos_ConformDscpVal;
extern L7_char8 *pStrInfo_qos_ConformIpPrecedenceVal;
extern L7_char8 *pStrInfo_qos_DecayExponent;
extern L7_char8 *pStrInfo_qos_DstIpMask;
extern L7_char8 *pStrInfo_qos_DstIpAddress;
extern L7_char8 *pStrInfo_qos_DstL4EndPort;
extern L7_char8 *pStrInfo_qos_DstL4Port;
extern L7_char8 *pStrInfo_qos_DstL4PortKeyword;
extern L7_char8 *pStrInfo_qos_DstL4StartPort;
extern L7_char8 *pStrInfo_qos_DstLayer4Port;
extern L7_char8 *pStrInfo_qos_DstMacAddr;
extern L7_char8 *pStrInfo_qos_DiffservAdminMode;
extern L7_char8 *pStrInfo_qos_Drop;
extern L7_char8 *pStrInfo_qos_EtherType;
extern L7_char8 *pStrInfo_qos_ExceedAction;
extern L7_char8 *pStrInfo_qos_ExceedDscpVal;
extern L7_char8 *pStrInfo_qos_ExceedIpPrecedenceVal;
extern L7_char8 *pStrInfo_qos_Expedite;
extern L7_char8 *pStrInfo_qos_FlowLabel;
extern L7_char8 *pStrInfo_qos_Icmp;
extern L7_char8 *pStrInfo_qos_IpDscp;
extern L7_char8 *pStrInfo_qos_IpPrecedence;
extern L7_char8 *pStrInfo_qos_IpTos;
extern L7_char8 *pStrInfo_qos_Inbound;
extern L7_char8 *pStrInfo_qos_IntfShapingRate;
extern L7_char8 *pStrErr_qos_TrafficShapeBwWebParamRound;
extern L7_char8 *pStrErr_qos_TrafficShapeBwWebParamRounded;
extern L7_char8 *pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars;
extern L7_char8 *pStrInfo_qos_Mac_1;
extern L7_char8 *pStrInfo_qos_MarkCos;
extern L7_char8 *pStrInfo_qos_MarkCosAsCos2;
extern L7_char8 *pStrInfo_qos_MarkIpDscp;
extern L7_char8 *pStrInfo_qos_MarkIpPrecedence;
extern L7_char8 *pStrInfo_qos_MarkSecondaryCos;
extern L7_char8 *pStrInfo_qos_MatchEvery;
extern L7_char8 *pStrInfo_qos_MaxDropProbability;
extern L7_char8 *pStrInfo_qos_MaxThresh_1;
extern L7_char8 *pStrInfo_qos_MinThresh_2;
extern L7_char8 *pStrInfo_qos_MirrorIntf;
extern L7_char8 *pStrInfo_qos_NoMoreAttrsCanBeSpecified;
extern L7_char8 *pStrErr_qos_ColorModeAwareNotSpecified;
extern L7_char8 *pStrInfo_qos_Out;
extern L7_char8 *pStrInfo_qos_Outbound;
extern L7_char8 *pStrInfo_qos_Pkts;
extern L7_char8 *pStrInfo_qos_PoliceSimple;
extern L7_char8 *pStrInfo_qos_PoliceSingleRate;
extern L7_char8 *pStrInfo_qos_PoliceTwoRate;
extern L7_char8 *pStrInfo_qos_PolicingStyle;
extern L7_char8 *pStrInfo_qos_PolicyName_1;
extern L7_char8 *pStrInfo_qos_PolicyType_1;
extern L7_char8 *pStrInfo_qos_QosAcl;
extern L7_char8 *pStrInfo_qos_QosDiffServ;
extern L7_char8 *pStrInfo_qos_QueueMgmtType;
extern L7_char8 *pStrInfo_qos_RandomDrop;
extern L7_char8 *pStrInfo_qos_RedirectIntf;
extern L7_char8 *pStrInfo_qos_RefClass;
extern L7_char8 *pStrInfo_qos_SamplingRate;
extern L7_char8 *pStrInfo_qos_SchedulerType;
extern L7_char8 *pStrInfo_qos_SecondaryClassOfService;
extern L7_char8 *pStrInfo_qos_SecondaryCosVal_1;
extern L7_char8 *pStrInfo_qos_SecondaryVlan;
extern L7_char8 *pStrInfo_qos_SecondaryVlanRange;
extern L7_char8 *pStrInfo_qos_SentOctets;
extern L7_char8 *pStrInfo_qos_ShapeAverage;
extern L7_char8 *pStrInfo_qos_ShapeDelayedOctets;
extern L7_char8 *pStrInfo_qos_ShapeDelayedPkts;
extern L7_char8 *pStrInfo_qos_SrcIpAddr;
extern L7_char8 *pStrInfo_qos_SrcIpMask;
extern L7_char8 *pStrInfo_qos_SrcL4EndPort;
extern L7_char8 *pStrInfo_qos_SrcL4Port;
extern L7_char8 *pStrInfo_qos_SrcL4PortKeyword;
extern L7_char8 *pStrInfo_qos_SrcL4StartPort;
extern L7_char8 *pStrInfo_qos_SrcLayer4Port;
extern L7_char8 *pStrInfo_qos_SrcMacAddr;
extern L7_char8 *pStrInfo_qos_Tcp;
extern L7_char8 *pStrInfo_qos_TosBits;
extern L7_char8 *pStrInfo_qos_TosMask;
extern L7_char8 *pStrInfo_qos_TailDropThresh_1;
extern L7_char8 *pStrInfo_qos_MaxOfAclsIsAlreadyCfgured;
extern L7_char8 *pStrErr_qos_DiffservClassNotAMbrPolicy;
extern L7_char8 *pStrInfo_qos_Udp;
extern L7_char8 *pStrInfo_qos_UnableToCreateAccessList;
extern L7_char8 *pStrInfo_qos_UnableToRenameAccessList;
extern L7_char8 *pStrInfo_qos_VlanRange;
extern L7_char8 *pStrInfo_qos_WredDecayExponent;
extern L7_char8 *pStrErr_qos_AddMatchCriteria_1;
extern L7_char8 *pStrInfo_qos_Domain_1;
extern L7_char8 *pStrInfo_qos_Echo;
extern L7_char8 *pStrInfo_qos_Ftpdata;
extern L7_char8 *pStrInfo_qos_Smtp;
extern L7_char8 *pStrInfo_qos_Strict_2;
extern L7_char8 *pStrInfo_qos_Tcp_1;
extern L7_char8 *pStrInfo_qos_Udp_1;
extern L7_char8 *pStrErr_qos_CfgCosTrustUntrusted;
extern L7_char8 *pStrInfo_qos_Www;

extern L7_char8 *pStrInfo_qos_matchCriteira;
extern L7_char8 *pStrInfo_qos_matchValues;
extern L7_char8 *pStrInfo_qos_matchExcluded;
extern L7_char8 *pStrInfo_qos_ip;

#endif

