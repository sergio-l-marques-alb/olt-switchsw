/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename trapstr_layer3.h
*
* @purpose Trap Manager Layer 3 String File
*
* @component trapstr_layer3.h
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



extern L7_char8 virtIfStateChange_str[];
extern L7_char8 nbrStateChange_str[];
extern L7_char8 virtNbrStateChange_str[];
extern L7_char8 ifConfigError_str[];
extern L7_char8 virtIfConfigError_str[];
extern L7_char8 ifAuthFailure_str[];
extern L7_char8 virtIfAuthFailure_str[];
extern L7_char8 ifRxBadPacket_str[];
extern L7_char8 virtIfRxBadPacket_str[];
extern L7_char8 txRetransmit_str[];
extern L7_char8 virtTxRetransmit_str[];
extern L7_char8 originateLsa_str[];
extern L7_char8 maxAgeLsa_str[];
extern L7_char8 lsdbOverflow_str[];
extern L7_char8 lsdbApproachingOverflow_str[];
extern L7_char8 ifStateChange_str[];
extern L7_char8 ifRxPacket_str[];
extern L7_char8 rtbEntryInfo_str[];
extern L7_char8 vrrp_new_master_str[];
extern L7_char8 vrrp_auth_fail_str[];

/*OSPF States*/
extern L7_char8 ospf_intf_down[];
extern L7_char8 ospf_intf_loopback[];
extern L7_char8 ospf_intf_waiting[];
extern L7_char8 ospf_intf_pointtopoint[];
extern L7_char8 ospf_intf_desig_rtr[];
extern L7_char8 ospf_intf_bkup_desig_rtr[];
extern L7_char8 ospf_intf_other_desig_rtr[];

/*Neighbor States*/
extern L7_char8 ospf_down[];
extern L7_char8 ospf_attempt[];
extern L7_char8 ospf_init[];
extern L7_char8 ospf_twoway[];
extern L7_char8 ospf_exchangestart[];
extern L7_char8 ospf_exchange[];
extern L7_char8 ospf_loading[];
extern L7_char8 ospf_full[];

extern L7_char8 defaultvalue[];

/*Config Error Types*/
extern L7_char8 bad_version[];
extern L7_char8 area_mismatch[];
extern L7_char8 unknown_nbma_nbr[];
extern L7_char8 unknown_virtual_nbr[];
extern L7_char8 auth_type_mismatch[];
extern L7_char8 auth_failure[];
extern L7_char8 net_mask_mismatch[];
extern L7_char8 hello_interval_mismatch[];
extern L7_char8 dead_interval_mismatch[];
extern L7_char8 option_mismatch[];

/* OSPF Packet Types */
extern L7_char8 packet_hello[];
extern L7_char8 packet_db_descr[];
extern L7_char8 packet_ls_request[];
extern L7_char8 packet_ls_update[];
extern L7_char8 packet_ls_ack[];

/* OSPF LSDB Types */
extern L7_char8 lsdb_illegal_lsa[];
extern L7_char8 lsdb_router_lsa[];
extern L7_char8 lsdb_network_lsa[];
extern L7_char8 lsdb_ipnet_summary_lsa[];
extern L7_char8 lsdb_asbr_summary_lsa[];
extern L7_char8 lsdb_as_external_lsa[];
extern L7_char8 lsdb_group_member_lsa[];
extern L7_char8 lsdb_tmp1_lsa[];
extern L7_char8 lsdb_tmp2_lsa[];
extern L7_char8 lsdb_link_opaque_lsa[];
extern L7_char8 lsdb_area_opaque_lsa[];
extern L7_char8 lsdb_as_opaque_lsa[];
extern L7_char8 lsdb_last_lsa_type[];
extern L7_char8 bgp_established_str[];
extern L7_char8 bgp_backward_transition_str[];
