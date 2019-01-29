/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/common/bgp/strlib_bgp_common.c                                                      
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

#include "strlib_bgp_common.h"

L7_char8 *pStrInfo_bgp_Num802 = "802";
L7_char8 *pStrInfo_bgp_AddrAggregationMode = "Address Aggregation Mode";
L7_char8 *pStrInfo_bgp_AddrAggregationTblIsFull = "Address aggregation table is full.";
L7_char8 *pStrInfo_bgp_Appletalk = "AppleTalk";
L7_char8 *pStrInfo_bgp_AutoRestartMode = "Auto Restart Mode";
L7_char8 *pStrInfo_bgp_Bbn = "BBN";
L7_char8 *pStrInfo_bgp_Banyanvines = "BanyanVines";
L7_char8 *pStrInfo_bgp_Best = "Best";
L7_char8 *pStrInfo_bgp_ConfSeq = "CONF-SEQ";
L7_char8 *pStrInfo_bgp_ConfSet = "CONF-SET";
L7_char8 *pStrInfo_bgp_CalculateMedMode = "Calculate MED Mode";
L7_char8 *pStrInfo_bgp_ClusterId_1 = "Cluster ID";
L7_char8 *pStrInfo_bgp_Confederation = "Confederation";
L7_char8 *pStrInfo_bgp_ConfederationMbr = "Confederation Member";
L7_char8 *pStrInfo_bgp_DecnetIv = "DECnet IV";
L7_char8 *pStrInfo_bgp_DampingFactor = "Damping Factor";
L7_char8 *pStrInfo_bgp_DecayDecrement = "Decay Decrement";
L7_char8 *pStrInfo_bgp_E163 = "E.163";
L7_char8 *pStrInfo_bgp_E164 = "E.164";
L7_char8 *pStrInfo_bgp_Empty = "EMPTY";
L7_char8 *pStrInfo_bgp_EnteringAValOfWillSetConfedIdTo = "Entering a value of 0 will set the Confed ID to N/A.";
L7_char8 *pStrInfo_bgp_EstablishedTime = "Established Time";
L7_char8 *pStrInfo_bgp_EstablishedTransitions = "Established Transitions";
L7_char8 *pStrInfo_bgp_EventState = "Event State";
L7_char8 *pStrInfo_bgp_F169 = "F.169";
L7_char8 *pStrInfo_bgp_Hdlc = "HDLC";
L7_char8 *pStrInfo_bgp_KeepaliveTimerIsCfguredToHaveAValGreaterThanHoldtimer = "Keepalive timer is configured to have a value greater than the Holdtimer";
L7_char8 *pStrInfo_bgp_Labeldist = "LabelDist";
L7_char8 *pStrErr_bgp_Last_1 = "Last Error";
L7_char8 *pStrInfo_bgp_LocalAutonomousSys = "Local Autonomous System";
L7_char8 *pStrInfo_bgp_LocalId = "Local Identifier";
L7_char8 *pStrInfo_bgp_LocalIntfAddr = "Local Interface Address";
L7_char8 *pStrInfo_bgp_MatchType = "Match Type";
L7_char8 *pStrInfo_bgp_MsgSendLimit = "Message Send Limit";
L7_char8 *pStrInfo_bgp_MultiProto = "MultiProtocol";
L7_char8 *pStrInfo_bgp_Mcast_1 = "Multicast";
L7_char8 *pStrInfo_bgp_Nsap = "NSAP";
L7_char8 *pStrInfo_bgp_NegotiatedVer = "Negotiated Version";
L7_char8 *pStrInfo_bgp_NextHopSelfMode = "Next Hop Self Mode";
L7_char8 *pStrInfo_bgp_Origin = "Origin";
L7_char8 *pStrInfo_bgp_Peer = "Peer";
L7_char8 *pStrInfo_bgp_PeerAdminStatus = "Peer Admin Status";
L7_char8 *pStrInfo_bgp_PeerId = "Peer ID";
L7_char8 *pStrInfo_bgp_PeerState = "Peer State";
L7_char8 *pStrInfo_bgp_PenaltyIncrement = "Penalty Increment";
L7_char8 *pStrInfo_bgp_PolicyIdx = "Policy Index";
L7_char8 *pStrInfo_bgp_PrefixLen_4 = "Prefix/Length";
L7_char8 *pStrInfo_bgp_PropagateMedMode = "Propagate MED Mode";
L7_char8 *pStrInfo_bgp_RangeBetween = "Range Between";
L7_char8 *pStrInfo_bgp_RangeEqualTo = "Range Equal To";
L7_char8 *pStrInfo_bgp_RangeGreaterThan = "Range Greater Than";
L7_char8 *pStrInfo_bgp_RangeIpAddr = "Range IP Address";
L7_char8 *pStrInfo_bgp_RangeLessThan = "Range Less Than";
L7_char8 *pStrInfo_bgp_RemoteAs = "Remote AS";
L7_char8 *pStrInfo_bgp_RemoteAddr = "Remote Address";
L7_char8 *pStrInfo_bgp_RemotePort = "Remote Port";
L7_char8 *pStrInfo_bgp_ReuseLimit = "Reuse Limit";
L7_char8 *pStrInfo_bgp_ReuseSize = "Reuse Size";
L7_char8 *pStrInfo_bgp_RouteCommunity = "Route Community";
L7_char8 *pStrInfo_bgp_RouteFlapMode = "Route Flap Mode";
L7_char8 *pStrInfo_bgp_RouteLocalMed = "Route Local MED";
L7_char8 *pStrInfo_bgp_RouteLocalOrigin = "Route Local Origin";
L7_char8 *pStrInfo_bgp_RouteLocalPref = "Route Local Preference";
L7_char8 *pStrInfo_bgp_RouteReflectorMode = "Route Reflector Mode";
L7_char8 *pStrInfo_bgp_RouteREflect = "RouteReflection";
L7_char8 *pStrInfo_bgp_Seq = "SEQ";
L7_char8 *pStrInfo_bgp_Set = "SET";
L7_char8 *pStrInfo_bgp_SuppRessLimit = "Suppress Limit";
L7_char8 *pStrInfo_bgp_SuppRessMode = "Suppress Mode";
L7_char8 *pStrInfo_bgp_TimeCreated = "Time Created";
L7_char8 *pStrInfo_bgp_TimeSuppressed = "Time Suppressed";
L7_char8 *pStrInfo_bgp_TotalMsgsRcvd = "Total Messages Received";
L7_char8 *pStrInfo_bgp_TotalMsgsSent = "Total Messages Sent";
L7_char8 *pStrInfo_bgp_UcastMcast = "Unicast-Multicast";
L7_char8 *pStrInfo_bgp_UnknownAttrs = "Unknown Attributes";
L7_char8 *pStrInfo_bgp_UpdatesRcvd = "Updates Received";
L7_char8 *pStrInfo_bgp_X121 = "X.121";
L7_char8 *pStrInfo_bgp_Donotsend = "donotsend";
L7_char8 *pStrInfo_bgp_Egp = "egp";
L7_char8 *pStrInfo_bgp_Feasible = "feasible";
L7_char8 *pStrInfo_bgp_Igp = "igp";
L7_char8 *pStrInfo_bgp_Incomplete = "incomplete";
L7_char8 *pStrInfo_bgp_Modify = "modify";
L7_char8 *pStrInfo_bgp_Unfeasible = "unfeasible";

