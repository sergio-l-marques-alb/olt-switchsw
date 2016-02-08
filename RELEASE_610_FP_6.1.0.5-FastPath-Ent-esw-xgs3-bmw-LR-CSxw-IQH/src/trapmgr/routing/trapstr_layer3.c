/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename trapstr_layer3.c
*
* @purpose Trap Manager Layer 3 String File
*
* @component trapstr_layer3.c
*
* @comments none
*
* @created 04/18/2001
*
* @author kdesai
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <l7_common.h>
#include <trapstr_layer3.h>

L7_char8 virtIfStateChange_str[] = "Virtual Interface State Change: Router Id: %s Area Id: %s Neighbor: %s State: %s";
L7_char8 nbrStateChange_str[] = "Neighbor State Change: interface: %s Router Id: %s Neighbor: %s Neighbor Router: %s Neighbor State: %s";
L7_char8 virtNbrStateChange_str[] = "Virtual Neighbor State Change: Router Id: %s Virtual Nbr Area: %s ";
L7_char8 ifConfigError_str[] = "Interface Configuration Error: interface: %s Router Id: %s IP Address: %s Packet Src: %s Error Type: %s Packet Type: %s";
L7_char8 virtIfConfigError_str[] = "Virtual Interface Configuration Error: Router Id: %s Area Id: %s Neighbor: %s Error Type: %s Packet Type: %s";
L7_char8 ifAuthFailure_str[] = "Interface Authentication Failure: interface: %s Router Id: %s IP Address: %s Packet Src: %s Error Type: %s Packet Type: %s";
L7_char8 virtIfAuthFailure_str[] = "Virtual Interface Authentication Failure: Router Id: %s Area Id: %s Neighbor: %s Error Type: %s Packet Type: %s";
L7_char8 ifRxBadPacket_str[] = "Interface Received Bad Packet: interface: %s Router Id: %s IP Address: %s Packet Src: %s Packet Type: %s";
L7_char8 virtIfRxBadPacket_str[] = "Virtual Interface Received Bad Packet: Router Id: %s Area Id: %s Neighbor: %s Packet Type: %s";
L7_char8 txRetransmit_str[] = "Interface Packet Retransmitted: interface: %s Router Id: %s IP Address: %s Neighbor Rtr: %s Packet Type: %s Lsdb Type: %s Lsdb Id: %s Lsdb Rtr Id: %s";
L7_char8 virtTxRetransmit_str[] = "Virtual Interface Packet Retransmitted: Router Id: %s Area Id: %s Neighbor: %s Packet Type: %s Lsdb Type: %s Lsdb Id: %s Lsdb Router Id: %s";
L7_char8 originateLsa_str[] = "New LSA originated: Router Id: %s Lsdb Area Id: %s Lsdb Type: %s Lsdb Id: %s Lsdb Router Id: %s";
L7_char8 maxAgeLsa_str[] = "LSA has aged to max age: Router Id: %s Lsdb Area Id: %s Lsdb Type: %s Lsdb Id: %s Lsdb Router Id: %s";
L7_char8 lsdbOverflow_str[] = "Lsdb Overflow: Router Id: %s extLsdbLimit: %d";
L7_char8 lsdbApproachingOverflow_str[] = "Lsdb Approaching Overflow:  Router Id: %s extLsdbLimit: %d";
L7_char8 ifStateChange_str[] = "Interface State Change: interface: %s Router Id: %s IP Address: %s State: %s";
L7_char8 ifRxPacket_str[] = "Interface Packet Received: ";
L7_char8 rtbEntryInfo_str[] = "Routing Table Entry Information: ";
L7_char8 vrrp_new_master_str[] = "VRRP New Master: Unit: %d IP Address: %s";
L7_char8 vrrp_auth_fail_str[] = "VRRP Authentication Failure: Unit: %d IP Address: %d Error Type: %d";

/*OSPF States*/

L7_char8 ospf_intf_down[] = "down";
L7_char8 ospf_intf_loopback[] = "loopback";
L7_char8 ospf_intf_waiting[] = "waiting";
L7_char8 ospf_intf_pointtopoint[] = "point-to-point";
L7_char8 ospf_intf_desig_rtr[] = "designated-router";
L7_char8 ospf_intf_bkup_desig_rtr[] = "backup-designated-router";
L7_char8 ospf_intf_other_desig_rtr[] = "other-designated-router";

/*Neighbor States*/
L7_char8 ospf_down[] = "Down";
L7_char8 ospf_attempt[] = "Attempt";
L7_char8 ospf_init[] = "Init";
L7_char8 ospf_twoway[] = "2-Way";
L7_char8 ospf_exchangestart[] = "Exchange-Start";
L7_char8 ospf_exchange[] = "Exchange";
L7_char8 ospf_loading[] = "Loading";
L7_char8 ospf_full[] = "Full";

L7_char8 defaultvalue[] = " ";

/*Config Error Types*/
L7_char8 bad_version[] = "badVersion";
L7_char8 area_mismatch[] = "areaMismatch";
L7_char8 unknown_nbma_nbr[] = "unknownNbmaNbr";
L7_char8 unknown_virtual_nbr[] = "unknownVirtualNbr";
L7_char8 auth_type_mismatch[] = "authTypeMismatch";
L7_char8 auth_failure[] = "authFailure";
L7_char8 net_mask_mismatch[] = "netMaskMismatch";
L7_char8 hello_interval_mismatch[] = "helloIntervalMismatch";
L7_char8 dead_interval_mismatch[] = "deadIntervalMismatch";
L7_char8 option_mismatch[] = "optionMismatch";

/* OSPF Packet Types */
L7_char8 packet_hello[]   = "hello";
L7_char8 packet_db_descr[] = "dbDescript";
L7_char8 packet_ls_request[] = "lsReq";
L7_char8 packet_ls_update[] = "lsUpdate";
L7_char8 packet_ls_ack[]      = "lsAck";

/* OSPF LSDB Types */
L7_char8 lsdb_illegal_lsa[] = "illegal lsa";
L7_char8 lsdb_router_lsa[] = "router links";
L7_char8 lsdb_network_lsa[] = "network links";
L7_char8 lsdb_ipnet_summary_lsa[] = "summary network";
L7_char8 lsdb_asbr_summary_lsa[] = "summary asbdr";
L7_char8 lsdb_as_external_lsa[] = "as external";
L7_char8 lsdb_group_member_lsa[] = "multicast group";
L7_char8 lsdb_tmp1_lsa[] = "temp lsa 1";
L7_char8 lsdb_tmp2_lsa[] = "temp lsa 2";
L7_char8 lsdb_link_opaque_lsa[] = "link opaque";
L7_char8 lsdb_area_opaque_lsa[] = "area opaque";
L7_char8 lsdb_as_opaque_lsa[] = "as opaque";
L7_char8 lsdb_last_lsa_type[] = "last lsa";
L7_char8 bgp_established_str[] = "BGP Established: Unit: %d RemoteAddr: %s  LastError: 0x%02x%02x State: %d";
L7_char8 bgp_backward_transition_str[] = "BGP Backward Transition: Unit: %d RemoteAddr: %s  LastError: 0x%02x%02x State: %d";
