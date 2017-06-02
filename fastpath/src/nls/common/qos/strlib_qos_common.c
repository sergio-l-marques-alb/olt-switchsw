/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/common/qos/strlib_qos_common.c                                                      
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

#include "strlib_qos_common.h"

L7_char8 *pStrInfo_qos_Any = "Any";
L7_char8 *pStrInfo_qos_AsSignQueue = "Assign Queue";
L7_char8 *pStrInfo_qos_ClassName_1 = "Class Name";
L7_char8 *pStrInfo_qos_ClassType = "Class Type";
L7_char8 *pStrInfo_qos_ClassOfService = "Class of Service";
L7_char8 *pStrInfo_qos_CosVal_1 = "CoS Value";
L7_char8 *pStrInfo_qos_ConformAction = "Conform Action";
L7_char8 *pStrInfo_qos_ConformDscpVal = "Conform DSCP Value";
L7_char8 *pStrInfo_qos_ConformIpPrecedenceVal = "Conform IP Precedence Value";
L7_char8 *pStrInfo_qos_DecayExponent = "Decay Exponent";
L7_char8 *pStrInfo_qos_DstIpMask = "Destination IP Wildcard Mask";
L7_char8 *pStrInfo_qos_DstIpAddress = "Destination IP Address";
L7_char8 *pStrInfo_qos_DstL4EndPort = "Destination L4 End Port";
L7_char8 *pStrInfo_qos_DstL4Port = "Destination L4 Port";
L7_char8 *pStrInfo_qos_DstL4PortKeyword = "Destination L4 Port Keyword";
L7_char8 *pStrInfo_qos_DstL4StartPort = "Destination L4 Start Port";
L7_char8 *pStrInfo_qos_DstLayer4Port = "Destination Layer 4 Port";
L7_char8 *pStrInfo_qos_DstMacAddr = "Destination MAC Address";
L7_char8 *pStrInfo_qos_DiffservAdminMode = "DiffServ Admin Mode";
L7_char8 *pStrInfo_qos_Drop = "Drop";
L7_char8 *pStrInfo_qos_EtherType = "Ethertype";
L7_char8 *pStrInfo_qos_ExceedAction = "Exceed Action";
L7_char8 *pStrInfo_qos_ExceedDscpVal = "Exceed DSCP Value";
L7_char8 *pStrInfo_qos_ExceedIpPrecedenceVal = "Exceed IP Precedence Value";
L7_char8 *pStrInfo_qos_Expedite = "Expedite";
L7_char8 *pStrInfo_qos_FlowLabel = "Flow Label";
L7_char8 *pStrInfo_qos_Icmp = "ICMP";
L7_char8 *pStrInfo_qos_IpDscp = "IP DSCP";
L7_char8 *pStrInfo_qos_IpPrecedence = "IP Precedence";
L7_char8 *pStrInfo_qos_IpTos = "IP TOS";
L7_char8 *pStrInfo_qos_Inbound = "Inbound";
L7_char8 *pStrInfo_qos_IntfShapingRate = "Interface Shaping Rate";
L7_char8 *pStrErr_qos_TrafficShapeBwWebParamRound = "Shaping Rate value is not multiple of 64";
L7_char8 *pStrErr_qos_TrafficShapeBwWebParamRounded = "Shaping Rate value is rounded to multiple of 64";
L7_char8 *pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars = "Invalid ACL Name. Name string may include alphabetic, numeric, dash, dot or underscore characters only. Name must start with a letter and the size of the name string must be less than or equal to 31 characters.";
L7_char8 *pStrInfo_qos_Mac_1 = "MAC";
L7_char8 *pStrInfo_qos_MarkCos = "Mark CoS";
L7_char8 *pStrInfo_qos_MarkCosAsCos2 = "Mark CoS as Secondary CoS";
L7_char8 *pStrInfo_qos_MarkIpDscp = "Mark IP DSCP";
L7_char8 *pStrInfo_qos_MarkIpPrecedence = "Mark IP Precedence";
L7_char8 *pStrInfo_qos_MarkSecondaryCos = "Mark Secondary CoS";
L7_char8 *pStrInfo_qos_MatchEvery = "Match Every";
L7_char8 *pStrInfo_qos_MaxDropProbability = "Maximum Drop Probability";
L7_char8 *pStrInfo_qos_MaxThresh_1 = "Maximum Threshold";
L7_char8 *pStrInfo_qos_MinThresh_2 = "Minimum Threshold";
L7_char8 *pStrInfo_qos_MirrorIntf = "Mirror Interface";
L7_char8 *pStrInfo_qos_NoMoreAttrsCanBeSpecified = "No more attributes can be specified.";
L7_char8 *pStrErr_qos_ColorModeAwareNotSpecified = "Not Specified";
L7_char8 *pStrInfo_qos_Out = "Out";
L7_char8 *pStrInfo_qos_Outbound = "Outbound";
L7_char8 *pStrInfo_qos_Pkts = "Packets";
L7_char8 *pStrInfo_qos_PoliceSimple = "Police Simple";
L7_char8 *pStrInfo_qos_PoliceSingleRate = "Police Single Rate";
L7_char8 *pStrInfo_qos_PoliceTwoRate = "Police Two Rate";
L7_char8 *pStrInfo_qos_PolicingStyle = "Policing Style";
L7_char8 *pStrInfo_qos_PolicyName_1 = "Policy Name";
L7_char8 *pStrInfo_qos_PolicyType_1 = "Policy Type";
L7_char8 *pStrInfo_qos_QosAcl = "QoS ACL";
L7_char8 *pStrInfo_qos_QosDiffServ = "QoS DiffServ";
L7_char8 *pStrInfo_qos_QueueMgmtType = "Queue Management Type";
L7_char8 *pStrInfo_qos_RandomDrop = "Random Drop";
L7_char8 *pStrInfo_qos_RedirectIntf = "Redirect Interface";
L7_char8 *pStrInfo_qos_TimeRangeName = "Time Range Name";
L7_char8 *pStrInfo_qos_TimeRangeStatus = "Rule Status";
L7_char8 *pStrInfo_qos_TimeRangeStatusActive = "active";
L7_char8 *pStrInfo_qos_TimeRangeStatusInactive = "inactive";
L7_char8 *pStrInfo_qos_RefClass = "Reference Class";
L7_char8 *pStrInfo_qos_SamplingRate = "Sampling Rate";
L7_char8 *pStrInfo_qos_SchedulerType = "Scheduler Type";
L7_char8 *pStrInfo_qos_SecondaryClassOfService = "Secondary Class of Service";
L7_char8 *pStrInfo_qos_SecondaryCosVal_1 = "Secondary CoS Value";
L7_char8 *pStrInfo_qos_SecondaryVlan = "Secondary VLAN";
L7_char8 *pStrInfo_qos_SecondaryVlanRange = "Secondary VLAN Range";
L7_char8 *pStrInfo_qos_SentOctets = "Sent Octets";
L7_char8 *pStrInfo_qos_ShapeAverage = "Shape Average";
L7_char8 *pStrInfo_qos_ShapeDelayedOctets = "Shape Delayed Octets";
L7_char8 *pStrInfo_qos_ShapeDelayedPkts = "Shape Delayed Packets";
L7_char8 *pStrInfo_qos_SrcIpAddr = "Source IP Address";
L7_char8 *pStrInfo_qos_SrcIpMask = "Source IP Wildcard Mask";
L7_char8 *pStrInfo_qos_SrcL4EndPort = "Source L4 End Port";
L7_char8 *pStrInfo_qos_SrcL4Port = "Source L4 Port";
L7_char8 *pStrInfo_qos_SrcL4PortKeyword = "Source L4 Port Keyword";
L7_char8 *pStrInfo_qos_SrcL4StartPort = "Source L4 Start Port";
L7_char8 *pStrInfo_qos_SrcLayer4Port = "Source Layer 4 Port";
L7_char8 *pStrInfo_qos_SrcMacAddr = "Source MAC Address";
L7_char8 *pStrInfo_qos_Tcp = "TCP";
L7_char8 *pStrInfo_qos_TosBits = "TOS Bits";
L7_char8 *pStrInfo_qos_TosMask = "TOS Mask";
L7_char8 *pStrInfo_qos_TailDropThresh_1 = "Tail Drop Threshold";
L7_char8 *pStrInfo_qos_MaxOfAclsIsAlreadyCfgured = "The maximum of %d ACLs or maximum number of rules is already configured.";
L7_char8 *pStrErr_qos_DiffservClassNotAMbrPolicy = "This Diffserv Class is not a member of this policy.";
L7_char8 *pStrInfo_qos_Udp = "UDP";
L7_char8 *pStrInfo_qos_UnableToCreateAccessList = "Unable to create Access List.";
L7_char8 *pStrInfo_qos_UnableToRenameAccessList = "Unable to rename Access List.";
L7_char8 *pStrInfo_qos_VlanRange = "VLAN Range";
L7_char8 *pStrInfo_qos_WredDecayExponent = "WRED Decay Exponent";
L7_char8 *pStrErr_qos_AddMatchCriteria_1 = "add the match criteria.";
L7_char8 *pStrInfo_qos_Domain_1 = "domain";
L7_char8 *pStrInfo_qos_Echo = "echo";
L7_char8 *pStrInfo_qos_Ftpdata = "ftpdata";
L7_char8 *pStrInfo_qos_Smtp = "smtp";
L7_char8 *pStrInfo_qos_Strict_2 = "strict";
L7_char8 *pStrInfo_qos_Tcp_1 = "tcp";
L7_char8 *pStrInfo_qos_Udp_1 = "udp";
L7_char8 *pStrErr_qos_CfgCosTrustUntrusted = "untrusted";
L7_char8 *pStrInfo_qos_Www = "www";
L7_char8 *pStrInfo_qos_matchCriteira = "Match Criteria";
L7_char8 *pStrInfo_qos_matchValues = "Values";
L7_char8 *pStrInfo_qos_matchExcluded = "Excluded";
L7_char8 *pStrInfo_qos_ip = "IP";


