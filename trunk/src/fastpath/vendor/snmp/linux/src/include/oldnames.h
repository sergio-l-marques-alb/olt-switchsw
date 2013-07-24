/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_OLDNAMES_H
#define SR_OLDNAMES_H

#ifdef  __cplusplus
extern "C" {
#endif

#define acl_insert_sort AclInsertSort
#define add_bitstring AddBitString
#define add_ele_to_tree_level AddEleToTreeLevel
#define add_ele_to_tree AddEleToTree
#define add_len AddLen
#define add_name_oid_to_hash AddNameOIDToHash
#define add_null AddNull
#define add_octetstring AddOctetString
#define add_oid AddOID
#define add_signedinteger AddInt
#define add_uint64 AddUInt64
#define add_unsignedinteger AddUInt
#define add_varbind AddVarBind
#define baseclock BaseClock
#define build_authentication BuildAuthentication
#define build_next_vb BuildNextVarBind
#define build_pdu BuildPdu
#define byteswap ByteSwap
#define call_make_authentication CallMakeAuthentication
#define cat_oid CatOID
#define check_class_mib_view CheckClassMIBView
#define check_digest CheckDigest
#define check_end_of_mib CheckEndOfMIB
#define check_mib_view CheckMIBView
#define check_security_tables CheckSecurityTables
#define chk_oid CheckOID
#define clone_bitstring CloneBitString
#define clone_octetstring CloneOctetString
#define clone_oid CloneOID
#define clone_partial_oid ClonePartialOID
#define clone_uint64 CloneUInt64
#define clone_varbind_list CloneVarBindList
#define clone_varbind CloneVarBind
#define close_up CloseUp
#define cmp_n_oid CmpNOID
#define cmp_oid_class CmpOIDClass
#define cmp_oid_inst CmpOIDInst
#define cmp_oid_values CmpOID
#define compute_view_vectors ComputeViewVectors
#define context_insert_sort ContextInsertSort
#define create_vb CreateVarBind
#define decchar2l DecChar2L
#define delete_acl_element DeleteAclElement
#define delete_context_element DeleteContextElement
#define delete_party_element DeletePartyElement
#define delete_view_element DeleteViewElement
#define do_cluster_init DoClusterInit
#define dolenlen DoLenLen
#define echo_check EchoCheck
#define filesize FileSize
#define find_len_bitstring FindLenBitString
#define find_len_octetstring FindLenOctetString
#define find_len_oid FindLenOID
#define find_len_signedinteger FindLenInt
#define find_len_uint64 FindLenUInt64
#define find_len_unsignedinteger FindLenUInt
#define find_len_varbind FindLenVarBind
#define find_max_clock_offset FindMaxClockOffset
#define free_acl_element FreeAclElement
#define free_authentication FreeAuthentication
#define free_bitstring FreeBitString
#define free_context_element FreeContextElement
#define free_octetstring FreeOctetString
#define free_oid FreeOID
#define free_party_element FreePartyElement
#define free_pdu FreePdu
#define free_uint64 FreeUInt64
#define free_varbind_list FreeVarBindList
#define free_varbind FreeVarBind
#define free_view_element FreeViewElement
#define get_acl_entry GetAclEntry
#define get_acl_index GetAclIndex
#define get_context_entry GetContextEntry
#define get_context_index GetContextIndex
#define get_context_location GetContextLocation
#define get_enumerated_int_from_string GetEnumFromString
#define get_implied_index_length GetImpliedIndexLength
#define get_local_ip_address GetLocalIPAddress
#define get_mib_table_index GetMIBTableIndex
#define get_party_entry GetPartyEntry
#define get_party_index GetPartyIndex
#define get_party_location GetPartyLocation
#define get_party_pos GetPartyPos
#define get_response GetResponse
#define get_str_from_sub_ids GetStringFromSubIds
#define get_timenow GetTimeNow
#define get_type GetType
#define get_variable_index_length GetVariableIndexLength
#define get_variable_type GetVariableType
#define get_view_entry GetViewEntry
#define get_view_index GetViewIndex
#define hexchar2l HexChar2L
#define init_acl_entries InitAclEntries
#define init_clusters InitClusters
#define init_context_entries InitContextEntries
#define init_name_oid_hash_array InitNameOIDHashArray
#define init_new_oid_routines InitNewOIDRoutines
#define init_oid_tree InitOIDTree
#define init_parties InitParties
#define init_secure InitSecure
#define init_util_fnames InitUtilFileNames
#define init_view_entries InitViewEntries
#define initialize_io InitializeIO
#define initialize_trap_io InitializeTrapIO
#define initialize_traprcv_io InitializeTrapRcvIO
#define inst2ip Inst2IP
#define inst_to_fixed_octetstring InstToFixedOctetString
#define inst_to_implied_octetstring InstToImpliedOctetString
#define inst_to_int InstToInt
#define inst_to_ip InstToIP
#define inst_to_uint InstToUInt
#define inst_to_variable_octetstring InstToVariableOctetString
#define link_varbind LinkVarBind
#define make_authentication MakeAuthentication
#define make_bitstring MakeBitString
#define make_digest MakeDigest
#define make_dot_from_obj_id MakeDotFromOID
#define make_freeze_file_name MakeFreezeFileName
#define make_len_oid MakeLenOID
#define make_obj_id_from_dot MakeOIDFromDot
#define make_obj_id_from_hex MakeOIDFromHex
#define make_octet_from_hex MakeOctetStringFromHex
#define make_octet_from_text MakeOctetStringFromText
#define make_octetstring MakeOctetString
#define make_oid_sub_len MakeOIDSubLen
#define make_oid MakeOID
#define make_pdu MakePdu
#define make_req_id MakeReqId
#define make_v1_message MakeV1Message
#define make_v2_message_with_clocks MakeV2MessageWithClocks
#define make_v2_message MakeV2Message
#define MakeVarbind MakeVarBind
#define match_game MatchGame
#define merge_mib_from_file MergeMIBFromFile
#define modifydate ModifyDate
#define new_mib_from_file NewMIBFromFile
#define octchar2l OctChar2L
#define oid_tree_dive OIDTreeDive
#define parse_authentication ParseAuthentication
#define parse_bitstring ParseBitString
#define parse_implicit_sequence ParseImplicitSequence
#define parse_length ParseLength
#define parse_mib_line ParseMIBLine
#define parse_null ParseNull
#define parse_octetstring ParseOctetString
#define parse_oid ParseOID
#define parse_pdu ParsePdu
#define parse_sequence ParseSequence
#define parse_signedinteger ParseInt
#define parse_sub_id_decimal ParseSubIdDecimal
#define parse_sub_id_hex ParseSubIdHex
#define parse_sub_id_octal ParseSubIdOctal
#define parse_type ParseType
#define parse_uint64 ParseUInt64
#define parse_unsignedinteger ParseUInt
#define parse_varbind ParseVarBind
#define party_insert_sort PartyInsertSort
#define print_ascii PrintAscii
#define print_error_code PrintErrorCode
#define print_met_info PrintMetInfo
#define print_octet_string_out PrintOctetString
#define print_oid_out PrintOID
#define print_packet_out PrintPacket
#define print_route_info PrintRouteInfo
#define print_varbind_list PrintVarBindList
#define received_packet ReceivedPacket
#define recompute_view_vectors RecomputeViewVectors
#define send_request SendRequest
#define set_progname SetProgname
#define str2l Str2L
#define sync_clocks SyncClocks
#define time_out TimeOut
#define upperit UpperIt
#define view_insert_sort ViewInsertSort
#define write_acl_file WriteAclFile
#define write_context_file WriteContextFile
#define write_parties WriteParties
#define write_view_file WriteViewFile
#define Make_obj_id_from_dot MakeOIDFromDot
#define MakeVarbindWithNull MakeVarBindWithNull
#define MakeVarbindWithValue MakeVarBindWithValue

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif /* SR_OLDNAMES_H */
