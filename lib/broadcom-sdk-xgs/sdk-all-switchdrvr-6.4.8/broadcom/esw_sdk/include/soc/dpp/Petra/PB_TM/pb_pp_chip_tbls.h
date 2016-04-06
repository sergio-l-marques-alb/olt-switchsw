/* $Id: pb_pp_chip_tbls.h,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
/******************************************************************
*
* FILENAME:       soc_pb_pp_chip_tbls.h
*
* MODULE PREFIX:  soc_pb_pp_chip_tbls
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PB_PP_CHIP_TBLS_INCLUDED__
/* { */
#define __SOC_PB_PP_CHIP_TBLS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/Petra/petra_chip_defines.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* When defining a logical table (e.g, a table divided in HW to two represented
   in SW as one table), this macro is used as base_addr */
#define SOC_PB_PP_CHIP_TBLS_LOGICAL_TBL_BASE_ADDR (0xffffffff)

#define SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_NOF_FID_CLASS             (4)
#define SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX      (4)
#define SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_NOF_FID_CLASS             (4)
#define SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX      (4)

#define SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS          (4)
#define SOC_PB_PP_EPNI_SEM_RESULT_NOF_VIDS                        (2)

#define SOC_PB_PP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED              (2)

#define SOC_PB_PORTS_FEM_PP_CONTEXT_SIZE                         (6)
#define SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE                        (13)
#define SOC_PB_PORTS_FEM_PP_PORT_SIZE                            (6)

#define SOC_PB_PP_IHB_TCAM_DATA_WIDTH 5
#define SOC_PB_TCAM_NOF_BANKS         4
#define SOC_PB_PP_TCAM_NOF_LINES      512


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* Blocks definition { */

  /* Port Mine Table Physical Port: If set, Physical Port is        */
  /* local to this Device,Each address contains 4 entries           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01300000 */

   /* PhysicalPortMine0:                                           */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD physical_port_mine0;

   /* PhysicalPortMine1:                                           */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD physical_port_mine1;

   /* PhysicalPortMine2:                                           */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD physical_port_mine2;

   /* PhysicalPortMine3:                                           */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD physical_port_mine3;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL;

  /* Pinfo Llr: port info for LLR                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

   /* VlanClassificationProfile: Used to access the LLVP table     */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vlan_classification_profile;

   /* PcpProfile: Profile for the PCP decoding table               */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_profile;

   /* IgnoreIncomingPcp: If set then the user priority in the      */
   /* incoming VLAN tag is ignored when calculating the            */
   /* Traffic-Class                                                */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ignore_incoming_pcp;

   /* TcDpTosEnable: If set then TOS based traffic class and       */
   /* drop precedence resolution is enabled.                       */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_dp_tos_enable;

   /* TcDpTosIndex: Index into one of two TCDP-to-TOS table        */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_dp_tos_index;

   /* TcL4ProtocolEnable: If set then L4 based traffic class       */
   /* resolution is enabled                                        */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_l4_protocol_enable;

   /* TcSubnetEnable: If set then subnet based Traffic-Class       */
   /* resolution is enabled                                        */
   /* range: 9:9, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_subnet_enable;

   /* TcPortProtocolEnable: If set the Port-Protocol based         */
   /* Traffic-Class resolution is enabled                          */
   /* range: 10:10, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tc_port_protocol_enable;

   /* DefaultTrafficClass: The default Traffic-Class per port      */
   /* range: 13:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_traffic_class;

   /* IgnoreIncomingUp: If set then user priority in the           */
   /* incoming tag is ignored                                      */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ignore_incoming_up;

   /* IncomingUpMapProfile: Profile for the User-Priority          */
   /* mapping                                                      */
   /* range: 16:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD incoming_up_map_profile;

   /* UseDei: If set then the incoming DEI is used                 */
   /* range: 17:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD use_dei;

   /* DropPrecedenceProfile: Profile for the                       */
   /* DropPrecedenceMapPcp Table                                   */
   /* range: 19:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drop_precedence_profile;

   /* SaLookupEnable: If set then SA lookup at the link layer      */
   /* is allowed                                                   */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sa_lookup_enable;

   /* ActionProfileSaDropIndex: Selects one of four profile        */
   /* actions in case SA-Drop indication is set in the SA          */
   /* lookup result.                                               */
   /* range: 22:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD action_profile_sa_drop_index;

   /* EnableSaAuthentication: Enables                              */
   /* Action-Profile-SA-Authentication-Failed in case SA not       */
   /* found                                                        */
   /* range: 23:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_sa_authentication;

   /* IgnoreIncomingVid: If set then the port ignores the          */
   /* incoming VID, if exists                                      */
   /* range: 24:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ignore_incoming_vid;

   /* VidIpv4SubnetEnable: If set then subnet based VID            */
   /* classification is enabled for this port.                     */
   /* range: 25:25, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vid_ipv4_subnet_enable;

   /* VidPortProtocolEnable: If set then protocol                  */
   /* (Ethernet-Type) based VID classification is enabled for      */
   /* this port.                                                   */
   /* range: 26:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vid_port_protocol_enable;

   /* ProtocolProfile: Used to access the                          */
   /* Port-Protocol-to-VID/TC tables                               */
   /* range: 29:27, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD protocol_profile;

   /* DefaultInitialVid: Default initial VID                       */
   /* range: 41:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_initial_vid;

   /* NonAuthorizedMode_8021x: Enables trapping 802.1x packets     */
   /* range: 42:42, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD non_authorized_mode_8021x;

   /* reserved_mc_trap_profile:                                    */
   /* range: 44:43, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD reserved_mc_trap_profile;

   /* EnableArpTrap: Enables Arp Trap                              */
   /* range: 45:45, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_arp_trap;

   /* EnableIgmpTrap: Enables Igmp Trap                            */
   /* range: 46:46, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_igmp_trap;

   /* EnableMldTrap: Enables Mld Trap                              */
   /* range: 47:47, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_mld_trap;

   /* EnableDhcpTrap: Enables Dhcp Trap                            */
   /* range: 48:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_dhcp_trap;

   /* GeneralTrapEnable: Enable for each general trap              */
   /* range: 52:49, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD general_trap_enable;

   /* DefaultCpuTrapCode: Default Cpu-Trap-Code                    */
   /* range: 60:53, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_cpu_trap_code;

   /* DefaultActionProfileFwd: Default forwarding action           */
   /* strength                                                     */
   /* range: 63:61, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_action_profile_fwd;

   /* DefaultActionProfileSnp: Default snoop action strength       */
   /* range: 65:64, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_action_profile_snp;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PINFO_LLR_TBL;

  /* Llr Llvp: Link layer VLAN processing table. Entry into         */
  /* table is \{VLAN-Processing-Profile(3b), Outer-Tag(2),          */
  /* Outer-Tag-Is-Priority(1), Inner-Tag(2)\}                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00210000 */

   /* IncomingVidExist: If set then the incoming packet has a      */
   /* VLAN tag that is not a priority tag. Here we refer to        */
   /* the outer most tag.                                          */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD incoming_vid_exist;

   /* IncomingTagExist: If set then the incoming packet has a      */
   /* VLAN tag (this VLAN tag may or may not be a priority         */
   /* tag) Here we refer to the outer most tag.                    */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD incoming_tag_exist;

   /* IncomingSTagExist: If set then the packet has an S-Tag.      */
   /* Here we refer to the outer most tag.                         */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD incoming_stag_exist;

   /* AcceptableFrameTypeAction: One of four actions to            */
   /* perform. Used to identify invalid packet format              */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD acceptable_frame_type_action;

   /* OuterCompIndex: Used for VLAN range compression              */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD outer_comp_index;

   /* InnerCompIndex: Used for VLAN range compression              */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inner_comp_index;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LLR_LLVP_TBL;

  /* Ll Mirror Profile: For each VID-Index and Incoming port        */
  /* indicates the mirror profile                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00220000 */

   /* LlMirrorProfile: Maps \{VID-Index(3), In-PP-Port(6)\}        */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ll_mirror_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL;

  /* Subnet Classify: Subnet based VLAN classification              */
  /* configuration. The IPv4 source IP is compared to these         */
  /* values. Reset Value for all lines:Value: 54'h0 Mask:            */
  /* 54'h20_0000_0000_0000                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00230000 */

   /* Ipv4SubnetValid: If set then Ipv4SubnetVid may be used.      */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_subnet_valid;

   /* Ipv4SubnetTcValid: If set then Ipv4SubnetTc may be used.     */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_subnet_tc_valid;

   /* Ipv4SubnetTc: Traffic class value for matched subnet         */
   /* range: 4:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_subnet_tc;

   /* Ipv4SubnetVid: VID value for each matched subnet             */
   /* range: 16:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ipv4_subnet_vid;

   /* Ipv4SubnetMask: IP mask for subnet based VLAN                */
   /* classification. Indicates number of In-valid bits at the     */
   /* IP-Subnet                                                    */
   /* range: 21:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ipv4_subnet_mask;

   /* Ipv4Subnet: IP for subnet based VLAN classification          */
   /* range: 53:22, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ipv4_subnet;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL;

  /* Port Protocol: This table is used for port-protocol            */
  /* based Traffic-Class and Initial-VID resolution. Input           */
  /* into table is \{Ethernet-Type-Code,                            */
  /* In-PP-Port. Port-Profile\}                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00240000 */

   /* TrafficClassValid: Valid bit for Traffic Class               */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD traffic_class_valid;

   /* TrafficClass: Traffic Class                                  */
   /* range: 3:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD traffic_class;

   /* VidValid: Valid bit for Initial VID                          */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vid_valid;

   /* Vid: Initial VID                                             */
   /* range: 16:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PORT_PROTOCOL_TBL;

  /* Tos 2 Cos: This table maps the IP-header. TOS to                */
  /* Traffic-Class and Drop-Precedence                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00250000 */

   /* Dp: Drop Precedence                                          */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD dp;

   /* Tc: Traffic-Class                                            */
   /* range: 4:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc;

   /* Valid: Entry Valid                                           */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_TOS_2_COS_TBL;

  /* Reserved Mc: Trap-Code for IEEE reserved multicast Entry       */
  /* into table is \{In-PP-Port. Trap-Profile(2), DA[5:0]\}          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00260000 */

   /* CpuTrapCode: Cpu-Trap-Code for IEEE reserved multicast       */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cpu_trap_code;

   /* Snp: snoop action strength IEEE reserved multicast           */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD snp;

   /* Fwd: forwarding action strength for IEEE reserved            */
   /* multicast                                                    */
   /* range: 7:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fwd;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_RESERVED_MC_TBL;

  /* Large Em Pldt Format1: The LargeEM has several formats.        */
  /* Format1 is used by the forwarding lookups block and its        */
  /* encoding is determined by the application. All encodings       */
  /* have the base format indicated here. Each application is       */
  /* elaborated in the subsequent sections.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630000 */

   /* Destination: Destination in EM-Encoding (See Data-Sheet      */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* FormatSpecificData: Format Specific Data - see below for     */
   /* further details                                              */
   /* range: 40:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD format_specific_data;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL;

  /* Large Em Pldt Format1 Asd Tp2p: Large-EM Entry - Format        */
  /* I - Transparent P2P data (applicable for AC and I-SID)         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630001 */

   /* Destination: Destination in EM-Encoding (See Data-Sheet      */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* OutLif: The Out-LIF associated with this entry.              */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif;

   /* OutLifValid: If set the Out-LIF is valid.                    */
   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif_valid;

   /* Identifier: If equal to 2'b00 then OutLIF and                */
   /* OutLIFValid are used. Otherwise, EeiLsb is used.             */
   /* range: 37:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD identifier;

   /* P2pService: must be set to 1'b1                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD p2p_service;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL;

  /* Large Em Pldt Format1 Asd Pbp: Large-EM Entry - Format I       */
  /* - B-MACT data (i.e. PBP)                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630002 */

   /* Destination: Destination in EM-Encoding (See Data-Sheet      */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* LearnDestination: Learn-Destination and Learn-ASD are        */
   /* determined as follows: If Learn-Type is set then:  .         */
   /* Learn-Destination (16) Src-System-Port-ID (as determined     */
   /* by the port termination) . type(2)                           */
   /* Learn-Destination-or-ASD is NULL ? 2'b00 : 2'b11 (i.e.       */
   /* Out-LIF) . Learn-ASD (24) \{SA-Drop = 1'b0,Identifier =      */
   /* 3'b0,type (2),Reserved = 2'b00,Out-LIF =                     */
   /* Learn-Destination-or-ASD\} Else (i.e. Learn-type is          */
   /* unset) . Learn-Destination (16) Learn-Destination-or-ASD     */
   /* // usually a FEC pointer . Learn-ASD (24) = NULL Note:       */
   /* Learn-Destination is in EM coding                            */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_destination;

   /* IsidDomain: Used for service delineation in the I-SID        */
   /* lookup in the Small-EM or Large-EM                           */
   /* range: 37:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isid_domain;

   /* LearnType: Determines encoding of the previous field.        */
   /* range: 38:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_type;

   /* SaDrop: If this bit is set for SA lookup then the            */
   /* forwarding action is changed according to configured         */
   /* action CfgActionProfileSaDrop                                */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sa_drop;

   /* IsDynamic: If set, then the entry is dynamic and             */
   /* participates in the aging process. If reset, it is           */
   /* static and does not participate in the aging process.        */
   /* However, the aging machine decrements the Age-Status         */
   /* bits.                                                        */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_dynamic;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL;

  /* Large Em Pldt Format1 Asd Ilm: Large-EM Entry - Format I       */
  /* - ILM data                                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630003 */

   /* MplsLabel: The MPLS label associated with this entry         */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* MplsLabel: The MPLS label associated with this entry         */
   /* (used in push and swap commands).                            */
   /* range: 35:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls_label;

   /* MplsCommand: The MPLS command to perform (refer to dune      */
   /* packet formats document).                                    */
   /* range: 39:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls_command;

   /* P2pService: must be set to 1'b0                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD p2p_service;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL;

  /* Large Em Pldt Format1 Asd Sp2p: Format I - P2P Service         */
  /* (applicable for PWE)                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630004 */

   /* Destination: Destination in EM-Encoding (See Data-Sheet      */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* OutLif: The Out-LIF associated with this entry.              */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif;

   /* OutLifValid: If set the Out-LIF is valid.                    */
   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif_valid;

   /* TpidProfile: The TPID profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 38:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* HasCw: If set then a control word follows the forwarding     */
   /* header.                                                      */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD has_cw;

   /* P2pService: must be set to 1'b1                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD p2p_service;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL;

  /* Large Em Pldt Format1 Asd Ethernet: Large-EM Entry -           */
  /* Format I - Ethernet data (MACT: normal, VPLS, RBridge)         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630005 */

   /* Destination: Destination in EM-Encoding (See Data-Sheet      */
   /* for further details)                                         */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* Asd: Application Specific Data                               */
   /* range: 39:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD asd;

   /* IsDynamic: If set, then the entry is dynamic and             */
   /* participates in the aging process. If reset, it is           */
   /* static and does not participate in the aging process.        */
   /* However, the aging machine decrements the Age-Status         */
   /* bits.                                                        */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_dynamic;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL;


  /* Large Em Pldt Format2: Format2 of the Large-EM is used         */
  /* by the link layer processing. The various encodings are        */
  /* detailed below. All formats share the fields described         */
  /* in this section.                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630006 */

   /* PermittedSystemPort: If authentication is required then      */
   /* this is the allowed SystemPortID in EM encoding.             */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD permitted_system_port;

   /* FormatSpecificData: Format Specific Data - see below for     */
   /* further details                                              */
   /* range: 40:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD format_specific_data;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL;
  /* Large Em Pldt Format2 Trill: Format when incoming              */
  /* PP-Port is TRILL                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630007 */

   /* PermittedSystemPort: If authentication is required then      */
   /* this is the allowed SystemPortID in EM encoding.             */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD permitted_system_port;

   /* Adjacent: The Out-LIF of the Adjacent RBridge. Usually       */
   /* this is an EEP. Used to identify the adjacent RBridge        */
   /* when forwarding and filtering.                               */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD adjacent;

   /* DropIfVidDiffer: Must be set to 1'b0                         */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drop_if_vid_differ;

   /* PermitAllPorts:                                              */
   /* range: 37:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD permit_all_ports;

   /* OverrideVidInTagged: Must be set to 1'b0                     */
   /* range: 38:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD override_vid_in_tagged;

   /* UseVidInUntagged: Must be set to 1'b0                        */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD use_vid_in_untagged;

   /* IsDynamic: May be used by the Large-EM aging machine to      */
   /* delete entries.                                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_dynamic;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL;

  /* Large Em Pldt Format2 Auth: Format for PBB and                 */
  /* authentication.                                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630008 */

   /* PermittedSystemPort: If authentication is required then      */
   /* this is the allowed SystemPortID in EM encoding.             */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD permitted_system_port;

   /* Vid: This value may be used for SA based VID.                */
   /* range: 27:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vid;

   /* AcceptUntagged: When authentication is required, then if     */
   /* set then untagged packets are allowed.                       */
   /* range: 35:35, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD accept_untagged;

   /* DropIfVidDiffer: When authentication is required, then       */
   /* if set then packets with a tag different than the VID        */
   /* specified in this entry are dropped.                         */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drop_if_vid_differ;

   /* PermitAllPorts: If unset then only packets arriving from     */
   /* the port specified in this entry are allowed.                */
   /* range: 37:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD permit_all_ports;

   /* OverrideVidInTagged: If set then the VID specified in        */
   /* this entry is used instead of the VID on the packet.         */
   /* range: 38:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD override_vid_in_tagged;

   /* UseVidInUntagged: If set the the VID specified in this       */
   /* entry is used for untagged packets.                          */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD use_vid_in_untagged;

   /* IsDynamic: May be used by the Large-EM aging machine to      */
   /* delete entries.                                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_dynamic;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL;

  /* Flush Db: Flush DB                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00760000 */

   /* CompareValid: if set, this entry is valid                    */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD compare_valid;

   /* CompareKey_16MsbsData: This data masked by CompareKey        */
   /* 16MsbsMask will be compared to the 14Msbs of the             */
   /* entry-key                                                    */
   /* range: 16:1, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD compare_key_16_msbs_data;

   /* CompareKey_16MsbsMask: See Above                             */
   /* range: 32:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD compare_key_16_msbs_mask;

   /* ComparePayloadData: This data masked by                      */
   /* ComparePayloadMask will be compared to the entry payload     */
   /* range: 73:33, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD compare_payload_data;

   /* ComparePayloadMask: see above                                */
   /* range: 114:74, access type: RW, default value: 0x0           */
   SOC_PETRA_TBL_FIELD compare_payload_mask;

   /* ActionDrop: if set, a successful lookup is considered as     */
   /* failed.                                                      */
   /* range: 115:115, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD action_drop;

   /* ActionTransplantPayloadData: A successful lookup returns     */
   /* this value.                                                  */
   /* range: 156:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD action_transplant_payload_data;

   /* ActionTransplantPayloadMask: foreach bit, if set, the        */
   /* original-payload bit will be replaced with the               */
   /* transplant-payload bit                                       */
   /* range: 197:157, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD action_transplant_payload_mask;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_FLUSH_DB_TBL;

  /* Large Em Fid Counter Db:                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00780000 */

   /* EntryCount: Holds the current number of entries with         */
   /* FID=index in the MACT                                        */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD entry_count;

   /* ProfilePointer: Pointer to one of 8 possible profiles        */
   /* range: 17:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD profile_pointer;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL;

  /* Large Em Fid Counter Profile Db:                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00790000 */

   /* Limit: Limit on the number of entries in the MACT            */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD limit;

   /* InterruptEn: if set, an interrupt will be raised when        */
   /* limit is reached                                             */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD interrupt_en;

   /* MessageEn: if set, an event is generated when limit is       */
   /* reached                                                      */
   /* range: 16:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD message_en;

   /* IsLinkLayerFid: if set, this FID is a mapping of             */
   /* Link-Layer-FID Note: This property should be configured      */
   /* per VSI and not per FID                                      */
   /* range: 17:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_link_layer_fid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL;

  /* Large Em Aging Configuration Table:                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007a0000 */

   /* DeleteEntryEven: if set, Delete entry from MACT              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD delete_entry[SOC_PB_PP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

   /* CreateAgedOutEventEven: if set, Create 'Aged-out Event'      */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD create_aged_out_event[SOC_PB_PP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

   /* CreateRefreshEventEven: if set, Create 'Refresh Event'       */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD create_refresh_event[SOC_PB_PP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL;

  /* Tm Port Pp Context Config: This table holds the per TM         */
  /* port related parameters resolving the PP context and           */
  /* Packet-Format-Qualifier0. In the FEM result, the lower 3b       */
  /* are the Packet-Format-Qualifier0, and the upper 3b are         */
  /* the PP-Context.                                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* PpContextOffset1: Offset in byte resolution of the lower     */
   /* word to input to the field extraction macro.                 */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_context_offset1;

   /* PpContextOffset2: Offset in byte resolution of the upper     */
   /* word to input to the field extraction macro.                 */
   /* range: 11:6, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pp_context_offset2;

   /* PpContextProfile: The profile of the field extraction        */
   /* macro.                                                       */
   /* range: 13:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_context_profile;

   /* PpContextValueToUse: Determines the input to the field       */
   /* extraction macro as follows: 0 - \{Header[Offset2],          */
   /* Header[Offset1]\} 1 - \{Header[offset2], Offset1[5:0]\}      */
   /* 2 - \{Header[offset2], PpPortPpContextValue,                 */
   /* PpPortPacketFormatQualifier0Value\} 3 - Reserved             */
   /* range: 15:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_context_value_to_use;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL;

  /* Pp Port Info: This table holds the per PP-Port                 */
  /* parameters for initial parsing.                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* OuterHeaderStart: Offset in byte resolution to outer         */
   /* header start.                                                */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD outer_header_start;

   /* TpidProfile: Specifies the TPID-Profile the port should      */
   /* use.                                                         */
   /* range: 7:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tpid_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_PORT_INFO_TBL;

  /* Pp Port Values: This table holds the per PP-Port values        */
  /* for port termination.                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* PpPortPacketFormatQualifier0Value: Per PP-port Packet        */
   /* format qualifer[0] value.                                    */
   /* range: 15:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_port_packet_format_qualifier0_value;

   /* PpPortPpContextValue: Per PP-Port PP-context value.          */
   /* range: 18:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_port_pp_context_value;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_PORT_VALUES_TBL;

  /* Pp Port Fem Bit Select Table: This table holds the bit         */
  /* select configuration for the PP-Port field extraction          */
  /* macro                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00050000 */

   /* PpPortFemBitSelect: Field extraction macro bit select        */
   /* configuration.                                               */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_port_fem_bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL;

  /* Pp Port Fem Map Index Table: This table holds the action       */
  /* index configuration for the PP-Port field extraction           */
  /* macro                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00060000 */

   /* PpPortFemMapIndex: Field extraction macro map index.         */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_port_fem_map_index;

   /* PpPortFemMapData: Field extraction macro map data.           */
   /* range: 5:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_port_fem_map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL;

  /* Pp Port Fem Field Select Map: This table holds the field       */
  /* select configuration for the PP-Port field extraction          */
  /* macro.                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00070000 */

   /* PpPortFemFieldSelect0: Field extraction macro field          */
   /* select.                                                      */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_port_fem_field_select[SOC_PB_PORTS_FEM_PP_PORT_SIZE];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL;

  /* Src System Port Fem Bit Select Table: This table holds         */
  /* the bit select configuration for the Src-System-Port           */
  /* field extraction macro                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00080000 */

   /* SrcSystemPortFemBitSelect: Field extraction macro bit        */
   /* select configuration.                                        */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD src_system_port_fem_bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL;

  /* Src System Port Fem Map Index Table: This table holds          */
  /* the action index configuration for the Src-System-Port         */
  /* field extraction macro                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00090000 */

   /* SrcSystemPortFemMapIndex: Field extraction macro map         */
   /* index.                                                       */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD src_system_port_fem_map_index;

   /* SrcSystemPortFemMapData: Field extraction macro map          */
   /* data.                                                        */
   /* range: 5:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD src_system_port_fem_map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL;

  /* Src System Port Fem Field Select Map: This table holds         */
  /* the field select configuration for the Src-System-Port         */
  /* field extraction macro. It contains all 4 maps.                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000a0000 */

   /* SrcSystemPortFemFieldSelect0: Field extraction macro         */
   /* field select.                                                */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD src_system_port_fem_field_select[SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL;

  /* Pp Context Fem Bit Select Table: This table holds the          */
  /* bit select configuration for the Pp-Context field              */
  /* extraction macro                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000b0000 */

   /* PpContextFemBitSelect: Field extraction macro bit select     */
   /* configuration.                                               */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_context_fem_bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL;

  /* Pp Context Fem Map Index Table: This table holds the           */
  /* action index configuration for the Pp-Context field            */
  /* extraction macro                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000c0000 */

   /* PpContextFemMapIndex: Field extraction macro map index.      */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_context_fem_map_index;

   /* PpContextFemMapData: Field extraction macro map data.        */
   /* range: 5:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_context_fem_map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL;

  /* Pp Context Fem Field Select Map: This table holds the          */
  /* field select configuration for the Pp-Context field            */
  /* extraction macro.                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000d0000 */

   /* PpContextFemFieldSelect0: Field extraction macro field       */
   /* select.                                                      */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_context_fem_field_select[SOC_PB_PORTS_FEM_PP_CONTEXT_SIZE];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL;

  /* Parser Program1: Parser program memory                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000e0000 */

   /* NextAddrBase: Next address base in the program memory        */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD next_addr_base;

   /* MacroSel: Macro select, 0-7: 0-3: Custom macro 4:            */
   /* Ethernet macro 5: IPv4 macro 6: IPv6 macro 7: MPLS macro     */
   /* range: 9:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD macro_sel;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL;

  /* Parser Program2: Parser program memory. Must be                */
  /* identical in content to ParserProgram1.                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000f0000 */

   /* NextAddrBase: Next address base in the program memory        */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD next_addr_base;

   /* MacroSel: Macro select, 0-7: 0-3: Custom macro 4:            */
   /* Ethernet macro 5: IPv4 macro 6: IPv6 macro 7: MPLS macro     */
   /* range: 9:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD macro_sel;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL;

  /* Packet Format Table: This table maps the Parser-End-Leaf       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* ParserPmfProfile: Parser-PMF-profile.                        */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD parser_pmf_profile;

   /* KeyProgramProfile: Key program profile.                      */
   /* range: 5:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_program_profile;

   /* PacketFormatCode: Packet-Format-Code.                        */
   /* range: 11:6, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD packet_format_code;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL;

  /* Parser Custom Macro Parameters: Parser custom macro            */
  /* configuration parameters, 0-3 are for set 0, 4-7 are for       */
  /* set 1                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00110000 */

   /* CstmWordSelect: Selects a 32-bit word from start of          */
   /* packet in byte resolution.                                   */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cstm_word_select;

   /* CstmMaskLeft: Specifies how many bits from the MSB of        */
   /* the selected word should be zeroed.                          */
   /* range: 8:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cstm_mask_left;

   /* CstmMaskRight: Specifies how many bits from the LSB of       */
   /* the selected word should be zeroed.                          */
   /* range: 13:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cstm_mask_right;

   /* CstmConditionSelect: Specifies which comparator              */
   /* condition should be tested.                                  */
   /* range: 18:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_condition_select;

   /* CstmComparatorMode: Specifies the comparator's mode of       */
   /* operation. May be: 2'b11 - Compare all 32 bits 2'b01 -       */
   /* Compare 16 LSBs of selected data to 16 LSBs of               */
   /* comparator values 2'b10 - Compare 16 LSBs of selected        */
   /* data to 16 MSBs of comparator values 2'b00 - Reserved        */
   /* range: 20:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_comparator_mode;

   /* CstmComparatorMask: Specifies for each comparator value      */
   /* whether it is used in the compare action.                    */
   /* range: 35:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_comparator_mask;

   /* CstmShiftSel: Indicates header size selection:: 0 - If       */
   /* condition holds, use Shift-A otherwise use Shift-B 1 -       */
   /* use (masked-header shifted by Shift-A[1:0])(5) +             */
   /* Shift-B(5). if Shift-A(4) is set shift to the right,         */
   /* otherwise shift to the left.                                 */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_shift_sel;

   /* CstmShiftA: Specifies header size if the selected            */
   /* condition holds.                                             */
   /* range: 41:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_shift_a;

   /* CstmShiftB: Specifies header size if the selected            */
   /* condition doesn't hold.                                      */
   /* range: 46:42, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cstm_shift_b;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL;

  /* Parser Eth Protocols: The parser identifies these              */
  /* Ethernet -Type and DSAP/SSAP values                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00120000 */

   /* EthTypeProtocol: DSAP/SSAP value                             */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD eth_type_protocol;

   /* EthSapProtocol: Ethernet type value                          */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD eth_sap_protocol;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL;

  /* Parser Ip Protocols: The parser identifes these IP             */
  /* protocol values                                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00130000 */

   /* IpProtocol:                                                  */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ip_protocol;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL;

  /* Parser Custom Macro Protocols: Parser custom macro             */
  /* protocols. 15 shared by all macros                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00140000 */

   /* CstmProtocol:                                                */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cstm_protocol;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL;

  /* Isem Management Request: Management requests                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00900000 */

   /* IsemType: The command type: 3'b000 = Delete 3'b001 =         */
   /* Insert 3'b100 = Defrag                                       */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD isem_type;

   /* IsemStamp: If different than zero then the EMC will          */
   /* generate an ACK event upon completion                        */
   /* range: 10:3, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD isem_stamp;

   /* IsemKey: Key, Valid only for delete, insert, refresh and     */
   /* learn commands                                               */
   /* range: 44:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isem_key;

   /* IsemPayload: Payload. Valid only for insert, refresh and     */
   /* learn commands                                               */
   /* range: 60:45, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isem_payload;

   /* IsemAgeStatus: Age status. Valid only for insert,            */
   /* refresh and learn commands                                   */
   /* range: 62:61, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isem_age_status;

   /* IsemSelf: If set then the command source is a DSP            */
   /* generated by this device. Valid only for insert, refresh     */
   /* and learn commands                                           */
   /* range: 63:63, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isem_self;

   /* IsemRefreshedByDsp: Refreshed by DSP indication. Valid       */
   /* only for insert, refresh and learn commands                  */
   /* range: 64:64, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isem_refreshed_by_dsp;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL;

  /* Vsi Isid: This table holds, per VSI, either the                */
  /* System-VSI or the I-SID. System configuration determines       */
  /* this tables functionality. Accessed by FLP, EGQ, EVF            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01100000 */

   /* Isid:                                                        */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi_or_isid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_ISID_TBL;

  /* Vsi My Mac: MyMac per VSIAccessed by VTT, EGQ                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01110000 */

   /* MyMacLsb: The postfix of the VSI's MAC address.              */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD my_mac_lsb;

   /* EnableMyMac: If set then this VSI is allowed to              */
   /* terminate an Ethernet header.                                */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD enable_my_mac;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_MY_MAC_TBL;

  /* Vsi Topology Id: Per-VSI Topology-IDAccessed by VTT, EGQ       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01120000 */

   /* VsiTopologyId: Topology ID for each VSI                      */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vsi_topology_id;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL;

  /* Vsi Fid Class: Used to access the FidClass2Fid table3'h7       */
  /* means no shared FIDAccessed by FLP, LEL, MRQ                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01130000 */

   /* FidClass0:                                                   */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fid_class[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_NOF_FID_CLASS];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_FID_CLASS_TBL;

  /* Vsi Da Not Found: Per VSI Pointer to one of four action        */
  /* triplets (per UC/MC/BC)Accessed by FLP                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01140000 */

   /* ProfileIndex0: VsiDaNotFoundProfileIndex0                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD profile_index[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL;

  /* Bvd Topology Id: Per-B-VID Topology-IDAccessed by FLP,         */
  /* EGQ                                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01150000 */

   /* BvdTopologyId: Topology ID for each B-VID                    */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bvd_topology_id;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL;

  /* Bvd Fid Class: Per-B-VID FID-ClassFID-Class determined         */
  /* FID1: VsdSharedBfid0: B-VIDAccessed by FLP, LLR, LEL,          */
  /* MRQ                                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01160000 */

   /* FidClass0:                                                   */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fid_class[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_NOF_FID_CLASS];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_BVD_FID_CLASS_TBL;

  /* Bvd Da Not Found: Per-B-VID                                    */
  /* Da-Not-Found-Profile-IndexUsed by FLP as \{1'b0,               */
  /* ProfileIndex(1b)\}Accessed by FLP                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01170000 */

   /* ProfileIndex0: BvdDaNotFoundActionProfileIndex0              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD profile_index[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL;

  /* Fid Class 2 Fid: maps FID-Class [0..6] to a 14bit              */
  /* FIDReset Value for all linesValue: 14'd0 Mask: 14'h0           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01180000 */

   /* Fid:                                                         */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL;

  /* Vlan Range Compression Table: 32 VLAN compression ranges       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00300000 */

   /* VlanRangeUpperLimit: Specify the upper limit of the VLAN     */
   /* range                                                        */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vlan_range_upper_limit;

   /* VlanRangeLowerLimit: Specify the lower limit of the VLAN     */
   /* range                                                        */
   /* range: 23:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_range_lower_limit;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL;

  /* Vtt In Pp Port Vlan Config: This table hold the VLAN           */
  /* range compression selection per port.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00310000 */

   /* RangeValid0:                                                 */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD range_valid0;

   /* RangeValid1:                                                 */
   /* range: 63:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD range_valid1;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL;

  /* Designated Vlan Table: This table hold 8 designated            */
  /* VLANs, per pp-port designated-vlan index.                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00320000 */

   /* DesignatedVlan: Designated VLAN ID                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD designated_vlan;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL;

  /* Vsi Port Membership: This table holds a bit per                */
  /* \{Initial-VID, PP-Port\} for the VLAN-Port membership.         */
  /* Table is accessed with Initial-VID, and each line holds        */
  /* one bit per pp-port.                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00330000 */

   /* VlanPortMemberLine: Bit N is the VLAN-Port-Member value      */
   /* for PP-Port N.                                               */
   /* range: 63:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vlan_port_member_line;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL;

  /* Vtt In Pp Port Config: This table holds VLAN translation       */
  /* and tunnel termination In-PP-Port configurations.              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00340000 */

   /* DesignatedVlanIndex: Specifies one of 8 designated VLAN      */
   /* IDs.                                                         */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD designated_vlan_index;

   /* VlanDomain: Specifies per port the VLAN-Domain.              */
   /* range: 8:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vlan_domain;

   /* DefaultSemBase: The default SEM result table base            */
   /* address (if lookup fails).                                   */
   /* range: 22:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD default_sem_base;

   /* DefaultSemOpcode: The default SEM result table offset        */
   /* opcode (if lookup fails).                                    */
   /* range: 24:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD default_sem_opcode;

   /* PortIsPbp: If set, Port is PBP.                              */
   /* range: 25:25, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD port_is_pbp;

   /* SmallEmKeyProfile: Specifies the small EM key program        */
   /* profile for the port.                                        */
   /* range: 28:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD small_em_key_profile;

   /* LearnAc: If set, Learn-ASD field will be the AC.             */
   /* Otherwise NULL.                                              */
   /* range: 29:29, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_ac;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL;

  /* Sem Result Table:                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350000 */

   /* SemResultTable: The Small-EM result table is accessed        */
   /* with the SEM-Index which is a result of the Small-EM         */
   /* lookup. The entry format is dependent on the key into        */
   /* the Small-EM and the application. The table contains 16K     */
   /* entries.                                                     */
   /* range: 61:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sem_result_table;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL;

  /* Sem Result Table Ac P2p To Ac: This is the format of a         */
  /* P2P AC that is connected to an AC.                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350001 */

   /* OutLif: The Out-LIF of the packet. Expected to be an AC.     */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD out_lif;

   /* VlanEditVid: A VID that may be used by the ingress VLAN      */
   /* editing.                                                     */
   /* range: 27:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_vid;

   /* VlanEditPcpDeiProfile: The PCP-DEI-Profile to use in the     */
   /* VLAN edit command resolution process.                        */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_pcp_dei_profile;

   /* VlanEditProfile: The profile to use in the VLAN edit         */
   /* command resolution process.                                  */
   /* range: 34:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_profile;

   /* MefL2CpProfile: Each AC is associated with a Layer-2         */
   /* Control Protocol profile.                                    */
   /* range: 35:35, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mef_l2_cp_profile;

   /* CfmMaxLevel: The CFM trap level for this entry. CFM          */
   /* packets with a level less than or equal to this value        */
   /* may be trapped.                                              */
   /* range: 38:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_max_level;

   /* CfmTrapValid: If set then CFM trapping is enabled for        */
   /* this AC.                                                     */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_trap_valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* Destination: The packet destination in EM-Encoding.          */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* Type: Equal to 2'b00 Encoded as: 2'b00 - AC-P2P-to-AC        */
   /* 2'b01 - AC-P2P-to-PWE 2'b10 - AC-P2P-to-PBB 2'b11 -          */
   /* AC-MP                                                        */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL;

  /* Sem Result Table Ac P2p To Pwe: This is the format of a        */
  /* P2P AC that is connected to a PWE.                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350002 */

   /* OutLifOrVcLabel: There are two options for encoding the      */
   /* destination. The first is to specify the destination as      */
   /* a System-Port-ID together with an Out-LIF containing an      */
   /* EEP. The EEP points to a PWE entry. The second is to         */
   /* specify the destination as a FEC pointer together with a     */
   /* VC-Label-Command. If the most significant nibble of this     */
   /* entry is equal to '0' then this field is an Out-LIF,         */
   /* otherwise it is a VC-Label-Command.                          */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD out_lif_or_vc_label;

   /* VlanEditPcpDeiProfile: The PCP-DEI-Profile to use in the     */
   /* VLAN edit command resolution process.                        */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_pcp_dei_profile;

   /* VlanEditProfile: The profile to use in the VLAN edit         */
   /* command resolution process.                                  */
   /* range: 34:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_profile;

   /* MefL2CpProfile: Each AC is associated with a Layer-2         */
   /* Control Protocol profile.                                    */
   /* range: 35:35, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mef_l2_cp_profile;

   /* CfmMaxLevel: The CFM trap level for this entry. CFM          */
   /* packets with a level less than or equal to this value        */
   /* may be trapped.                                              */
   /* range: 38:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_max_level;

   /* CfmTrapValid: If set then CFM trapping is enabled for        */
   /* this AC.                                                     */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_trap_valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* Destination: The packet destination in EM-Encoding.          */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* Type: Equal to 2'b01 Encoded as: 2'b00 - AC-P2P-to-AC        */
   /* 2'b01 - AC-P2P-to-PWE 2'b10 - AC-P2P-to-PBB 2'b11 -          */
   /* AC-MP                                                        */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL;

  /* Sem Result Table Ac P2p To Pbb: This is the format of a        */
  /* P2P AC that is connected to a PBP.                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350003 */

   /* Isid: The I-SID of the destination.                          */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD isid;

   /* VlanEditPcpDeiProfile: The PCP-DEI-Profile to use in the     */
   /* VLAN edit command resolution process.                        */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_pcp_dei_profile;

   /* VlanEditProfile: The profile to use in the VLAN edit         */
   /* command resolution process.                                  */
   /* range: 34:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_profile;

   /* MefL2CpProfile: Each AC is associated with a Layer-2         */
   /* Control Protocol profile.                                    */
   /* range: 35:35, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mef_l2_cp_profile;

   /* CfmMaxLevel: The CFM trap level for this entry. CFM          */
   /* packets with a level less than or equal to this value        */
   /* may be trapped.                                              */
   /* range: 38:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_max_level;

   /* CfmTrapValid: If set then CFM trapping is enabled for        */
   /* this AC.                                                     */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_trap_valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* Destination: The packet destination in EM-Encoding.          */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* Type: Equal to 2'b10 Encoded as: 2'b00 - AC-P2P-to-AC        */
   /* 2'b01 - AC-P2P-to-PWE 2'b10 - AC-P2P-to-PBB 2'b11 -          */
   /* AC-MP                                                        */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL;

  /* Sem Result Table Ac Mp: This is the format of a                */
  /* multipoint AC.                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350004 */

   /* Vsi: The VSI that the AC is connected to. If equal to        */
   /* '-1' then the VSI is equal to \{2'b00, Initial-VID\}         */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* TtLearnEnable: If set then this AC allows learning.          */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tt_learn_enable;

   /* OrientationIsHub: If set then the AC is core-facing          */
   /* (i.e. hub) otherwise it is customer facing (i.e. spoke).     */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD orientation_is_hub;

   /* VlanEditVid: A VID that may be used by the ingress VLAN      */
   /* editing.                                                     */
   /* range: 27:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_vid;

   /* VlanEditPcpDeiProfile: The PCP-DEI-Profile to use in the     */
   /* VLAN edit command resolution process.                        */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_pcp_dei_profile;

   /* VlanEditProfile: The profile to use in the VLAN edit         */
   /* command resolution process.                                  */
   /* range: 34:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vlan_edit_profile;

   /* MefL2CpProfile: Each AC is associated with a Layer-2         */
   /* Control Protocol profile.                                    */
   /* range: 35:35, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mef_l2_cp_profile;

   /* CfmMaxLevel: The CFM trap level for this entry. CFM          */
   /* packets with a level less than or equal to this value        */
   /* may be trapped.                                              */
   /* range: 38:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_max_level;

   /* CfmTrapValid: If set then CFM trapping is enabled for        */
   /* this AC.                                                     */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfm_trap_valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* LearnDestination: If non-zero then this is the value         */
   /* learned. Otherwise, the source System-Port-ID is             */
   /* learned.                                                     */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_destination;

   /* Type: Equal to 2'b11 Encoded as: 2'b00 - AC-P2P-to-AC        */
   /* 2'b01 - AC-P2P-to-PWE 2'b10 - AC-P2P-to-PBB 2'b11 -          */
   /* AC-MP                                                        */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL;

  /* Sem Result Table Isid P2p: This is the format of a PBP         */
  /* with a P2P service.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350005 */

   /* Vsi: The VSI that the LIF is connected to. Usually will       */
   /* have a constant value of: Const-I-SID-P2P-VSI. May have      */
   /* a value of Device-Cfg-MAC-in-MAC-VSI if it is used for       */
   /* I-SID to I-SID Translation service                           */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* ISIDorOutLIForVcLabel: There are three options for           */
   /* encoding the destination. The first is to specify the        */
   /* destination as a System-Port-ID together with an Out-LIF     */
   /* containing an EEP. The EEP points to a PWE entry. The        */
   /* second is to specify the destination as a FEC pointer        */
   /* together with a VC-Label-Command. The third is when VSI      */
   /* is equal to Device-Cfg-Mac-in-MAC-VSI. In this case the      */
   /* destination is a System-Port-ID and this entry is the        */
   /* I-SID. (The first two cases are distinguished by the         */
   /* most significant nibble of this field; when equal to '0      */
   /* this entry is an Out-LIF, otherwise it is VC-Label).         */
   /* range: 37:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isidor_out_lifor_vc_label;

   /* TpidProfile: The TPID-Profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 39:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile_index;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* Destination: The packet destination in EM-Encoding.          */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* ServiceTypeLsb: The service type is equal to \{1'b1,         */
   /* Type, Service-Type-Lsb\}. This value may be used by          */
   /* subsequent blocks (e.g. the PMF).                            */
   /* range: 60:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type_lsb;

   /* Type: Equal to 1'b0 Encoded as: 1'b0 - I-SID-P2P 1'b1 -      */
   /* I-SID-MP                                                     */
   /* range: 61:61, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL;

  /* Sem Result Table Isid Mp: This is the format of a PBP          */
  /* with a multipoint service.                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350006 */

   /* Vsi: The VSI that the LIF is connected to.                   */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* DestinationValid: If set, destination is valid.              */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination_valid;

   /* OrientationIsHub: If set then the AC is core-facing          */
   /* (i.e. hub) otherwise it is customer facing (i.e. spoke).     */
   /* range: 16:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD orientation_is_hub;

   /* TtLearnEnable: If set then this AC allows learning.          */
   /* range: 17:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tt_learn_enable;

   /* TpidProfile: The TPID-Profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 39:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* Destination: Destination in EM-Encoding. Should be set       */
   /* to NULL, indicating that a forwarding decision isn't         */
   /* made at this point.                                          */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* ServiceTypeLsb: The service type is equal to \{1'b1,         */
   /* Type, Service-Type-Lsb\}. This value may be used by          */
   /* subsequent blocks (e.g. the PMF).                            */
   /* range: 60:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type_lsb;

   /* Type: Equal to 1'b1 Encoded as: 1'b0 - I-SID-P2P 1'b1 -      */
   /* I-SID-MP                                                     */
   /* range: 61:61, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL;

  /* Sem Result Table Trill: TRILL encapsulated packets are         */
  /* terminated according to this entry.                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350007 */

   /* Vsi: The VSI that the AC is connected to. If equal to        */
   /* '-1' then the VSI is equal to \{2'b00, Initial-VID\}         */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* TtLearnEnable: If set then this LIF allows learning.         */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tt_learn_enable;

   /* TpidProfile: The TPID-Profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 17:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* ServiceType: This value may be used by subsequent blocks     */
   /* (e.g. the PMF).                                              */
   /* range: 43:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type;

   /* LearnDestination: This is the learn destination of TRILL     */
   /* terminated packets. Encoded in EM-encoding.                  */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_destination;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL;

  /* Sem Result Table Ip Tt: IP tunnels are terminated              */
  /* according to this entry.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350008 */

   /* InRif: The In-RIF of packets terminated by the tunnel.       */
   /* Valid only if In-RIF-Valid is set.                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD in_rif;

   /* InRifValid: If set then the In-RIF should be updated         */
   /* when terminating the tunnel.                                 */
   /* range: 12:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD in_rif_valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 40:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* ServiceType: This value may be used by subsequent blocks     */
   /* (e.g. the PMF).                                              */
   /* range: 43:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL;

  /* Sem Result Table Label Pwe P2p: P2P service MPLS labels        */
  /* are terminated according to this entry.                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350009 */

   /* Vsi: Usually will have a constant value of:                  */
   /* Cfg-Label-PWE-P2P-VSI May have a value of                    */
   /* Device-Cfg-MAC-in-MAC-VSI if it is used for PWE to I-SID     */
   /* Translation service                                          */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* ModelIsPipe: If set then the MPLS model is pipe,             */
   /* otherwise it is uniform.                                     */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD model_is_pipe;

   /* DestinationValid: If set, destination is valid.              */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination_valid;

   /* TpidProfile: The TPID-Profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 17:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* HasCw: If set then there is a 4B control word above this     */
   /* label.                                                       */
   /* range: 18:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD has_cw;

   /* Valid: If set then this entry is valid.                      */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD valid;

   /* ISIDorOutLIForVcLabel: There are three options for           */
   /* encoding the destination. The first is to specify the        */
   /* destination as a System-Port-ID together with an Out-LIF     */
   /* containing an EEP. The EEP points to a PWE entry. The        */
   /* second is to specify the destination as a FEC pointer        */
   /* together with a VC-Label-Command. The third is when VSI      */
   /* is equal to Device-Cfg-Mac-in-MAC-VSI. In this case the      */
   /* destination is a System-Port-ID and this entry is the        */
   /* I-SID. (The first two cases are distinguished by the         */
   /* most significant nibble of this field; when equal to '0      */
   /* this entry is an Out-LIF, otherwise it is VC-Label).         */
   /* range: 43:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD isidor_out_lifor_vc_label;

   /* Destination: Destination in EM encoding                      */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination;

   /* Type: Equal to 2'b00 Encoded as 2'b00 - Label-PWE P2P        */
   /* 2'b01 - Label-PWE MP 2'b10 - Label-VRL 2'b11 - Label-LSP     */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL;

  /* Sem Result Table Label Pwe Mp: Multipoint service MPLS         */
  /* labels are terminated according to this entry.                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x0035000a */

   /* Vsi: The VSI that this LIF is connected to.                  */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsi;

   /* OrientationIsHub: If set then the LIF is core-facing         */
   /* (i.e. hub) otherwise it is customer facing (i.e. spoke).     */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD orientation_is_hub;

   /* TtLearnEnable: If set then this LIF allows learning.         */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tt_learn_enable;

   /* TpidProfile: The TPID-Profile of the internal Ethernet       */
   /* header.                                                      */
   /* range: 17:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* HasCw: If set then there is a 4B control word above this     */
   /* label.                                                       */
   /* range: 18:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD has_cw;

   /* Valid: If set then this entry is valid.                      */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD valid;

   /* LearnAsd: This is the ASD used for learning.                 */
   /* range: 43:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_asd;

   /* LearnDestination: This is the destination used for           */
   /* learning.                                                    */
   /* range: 59:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD learn_destination;

   /* Type: Equal to 2'b01 Encoded as 2'b00 - Label-PWE P2P        */
   /* 2'b01 - Label-PWE MP 2'b10 - Label-VRL 2'b11 - Label-LSP     */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL;

  /* Sem Result Table Label Vrl: MPLS labels with an IP             */
  /* header above ther are terminated according to this             */
  /* entry.                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x0035000b */

   /* InRif: The In-RIF of packets terminated by the tunnel.       */
   /* Valid only if In-RIF-Valid is set.                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD in_rif;

   /* InRifValid: If set then the In-RIF should be updated         */
   /* when terminating the tunnel.                                 */
   /* range: 12:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD in_rif_valid;

   /* ModelIsPipe: If set then the MPLS model is pipe,             */
   /* otherwise it is uniform.                                     */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD model_is_pipe;

   /* Valid: If set then this entry is valid.                      */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 40:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* ServiceType: This value may be used by subsequent blocks     */
   /* (e.g. the PMF).                                              */
   /* range: 43:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type;

   /* Type: Equal to 2'b10 Encoded as 2'b00 - Label-PWE P2P        */
   /* 2'b01 - Label-PWE MP 2'b10 - Label-VRL 2'b11 - Label-LSP     */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL;

  /* Sem Result Table Label Lsp: MPLS labels that are not BOS       */
  /* are terminated according to this entry.                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x0035000c */

   /* InRif: The In-RIF of packets terminated by the tunnel.       */
   /* Valid only if In-RIF-Valid is set.                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD in_rif;

   /* InRifValid: If set then the In-RIF should be updated         */
   /* when terminating the tunnel.                                 */
   /* range: 12:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD in_rif_valid;

   /* ModelIsPipe: If set then the MPLS model is pipe,             */
   /* otherwise it is uniform.                                     */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD model_is_pipe;

   /* Valid: If set then this entry is valid.                      */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD valid;

   /* CosProfile: If the CoS-Profile is non-zero the packet's      */
   /* TC and DP are updated.                                       */
   /* range: 40:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos_profile;

   /* ServiceType: This value may be used by subsequent blocks     */
   /* (e.g. the PMF).                                              */
   /* range: 43:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD service_type;

   /* Type: Equal to 2'b11 Encoded as 2'b00 - Label-PWE P2P        */
   /* 2'b01 - Label-PWE MP 2'b10 - Label-VRL 2'b11 - Label-LSP     */
   /* range: 61:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL;

  /* Sem Opcode Ip Offsets: This table holds 2 SEM-opcode           */
  /* resolution tables: IPv6 and IPv4based tables. It is            */
  /* accessed with SEM-Opcode-Key=\{Sem-Opcode(2),                  */
  /* IP. TOS(8)\}.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00360000 */

   /* Ipv6SemOffset: IPv6- Offset to SEM-Result table from         */
   /* Base.                                                        */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv6_sem_offset;

   /* Ipv6AddOffsetToBase: Ipv6- If set, Offset will be added      */
   /* to base. Otherwise, base is used to access SEM-Result.       */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv6_add_offset_to_base;

   /* Ipv6OpcodeValid: IPv6- If set, the entry is valid.           */
   /* Otherwise, per-PP-Port default values will be used.          */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv6_opcode_valid;

   /* Ipv4SemOffset: IPv4- Offset to SEM-Result table from         */
   /* Base.                                                        */
   /* range: 7:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_sem_offset;

   /* Ipv4AddOffsetToBase: IPv4- If set, Offset will be added      */
   /* to base. Otherwise, base is used to access SEM-Result.       */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_add_offset_to_base;

   /* Ipv4OpcodeValid: IPv4- If set, the entry is valid.           */
   /* Otherwise, per-PP-Port default values will be used.          */
   /* range: 9:9, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ipv4_opcode_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL;

  /* Sem Opcode Tc Dp Offsets: This table holds TC-DP               */
  /* SEM-opcode resolution table. It is accessed with               */
  /* SEM-Opcode-Key=\{Sem-Opcode(2), TC(3), DP(2)\}.                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00370000 */

   /* TcDpSemOffset: TC-DP- Offset to SEM-Result table from        */
   /* Base.                                                        */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_dp_sem_offset;

   /* TcDpAddOffsetToBase: TC-DP- If set, Offset will be added     */
   /* to base. Otherwise, base is used to access SEM-Result.       */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_dp_add_offset_to_base;

   /* TcDpOpcodeValid: TC-DP- If set, the entry is valid.          */
   /* Otherwise, per-PP-Port default values will be used.          */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc_dp_opcode_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL;

  /* Sem Opcode Pcp Dei Offsets: This table holds PCP-DEI           */
  /* SEM-opcode resolution table. It is accessed with               */
  /* SEM-Opcode-Key=\{Sem-Opcode(2), Outer-Tag(2), PCP(3),          */
  /* DEI(1)\}.                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00380000 */

   /* PcpDeiSemOffset: PCP-DEI- Offset to SEM-Result table         */
   /* from Base.                                                   */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_dei_sem_offset;

   /* PcpDeiAddOffsetToBase: PCP-DEI- If set, Offset will be       */
   /* added to base. Otherwise, base is used to access             */
   /* SEM-Result.                                                  */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_dei_add_offset_to_base;

   /* PcpDeiOpcodeValid: PCP-DEI- If set, the entry is valid.      */
   /* Otherwise, per-PP-Port default values will be used.          */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_dei_opcode_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL;

  /* Stp Table: This table is accessed with the PP-Port. Each       */
  /* line holds the STP state per topology-ID.                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00390000 */

   /* PerPortStpState: Bits (2N+1):2N holds the STP state for      */
   /* Topology-ID N                                                */
   /* range: 127:0, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD per_port_stp_state;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_STP_TABLE_TBL;

  /* Vrid My Mac Map:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003a0000 */

   /* VridMyMacMap: Accessed according to VRID-My-Mac-Mode, in     */
   /* each mode th 8 MSBs are used to access the line, the 5       */
   /* LSBs are used to select a bit in the line: Per-port mode     */
   /* - bitmap is accessed with \{PP port, DA[6:0]\} Per-4096      */
   /* VSIs mode - bitmap is accessed with \{VSI[11:0], DA[0]\}     */
   /* Per-256 VSIs mode - bitmap is accessed with \{VSI[7:0],      */
   /* DA[4:0]\}                                                    */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vrid_my_mac_map;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL;

  /* Mpls Label Range Encountered:                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003b0000 */

   /* MplsLabelRangeEncountered: If bit N is set, Label index      */
   /* N in pipe and uniform ranges (staggred) was encountered.     */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD mpls_label_range_encountered;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL;

  /* Mpls Tunnel Termination Valid:                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003c0000 */

   /* MplsTunnelTerminationValid: If bit N is set, tunnel          */
   /* termination is valid for label index N in pipe and           */
   /* uniform ranges (staggred) was encountered.                   */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD mpls_tunnel_termination_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL;

  /* Ip Over Mpls Exp Mapping:                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003d0000 */

   /* IpOverMplsExpMapping: Maps Terminated DSCP-EXP to            */
   /* DSCP-EXP values, accessed by Terminated-DSCP-EXP[2:0].       */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ip_over_mpls_exp_mapping;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL;

  /* Vtt Llvp: VTT VLAN processing table. Entry into table is       */
  /* \{VLAN-Classification-Profile (3b),                            */
  /* Packet-Format-Qualifier[1]. Ethernet-Tag-Format (5),            */
  /* Key-Program-Profile (2)\}                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003e0000 */

   /* IncomingTagStructure: Used together with the                 */
   /* VLAN-Edit-Profile (which a result of the port x VLAN         */
   /* translation) to generate the VLAN edit command.              */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD incoming_tag_structure;

   /* PcpDeiProfile: This is the port-based PCP-DEI mapping        */
   /* profile.                                                     */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_dei_profile;

   /* Ivec: This is the port-based ingress VLAN edit command.      */
   /* range: 10:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ivec;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT_LLVP_TBL;

  /* Llvp Prog Sel:                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003f0000 */

   /* LlvpProgSel: This table is accessed with                     */
   /* \{Key-Program-Profile (2),                                   */
   /* Packet-Format-Qualifier[1][4:0],                             */
   /* In-PP-Port. Small-EM-Key-Profile (3) | \{2'b0,                */
   /* My-B-MAC\}\} to select one of 16 key generation programs     */
   /* for SEM lookup. In second key generation, it is accessed     */
   /* with \{Key-Program-Profile (2), 5'b00100 | \{3'b0,           */
   /* MPLS-Label-Index\} In-PP-Port. Small-EM-Key-Profile (3) |     */
   /* \{2'b0, My-MAC\}\}.                                          */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_prog_sel;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL;

  /* Vtt1st Key Construction0: This table access uses the           */
  /* LLVP program index to retrieve the first 2 instruction         */
  /* for SEM first lookup key generation.                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00400000 */

   /* Key_inst0:                                                   */
   /* range: 17:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_inst0;

   /* Key_inst1:                                                   */
   /* range: 35:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_inst1;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL;

  /* Vtt1st Key Construction1: This table access uses the           */
  /* LLVP program index to retrieve the last 2 instruction          */
  /* for SEM first lookup key generation.                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00410000 */

   /* Key_inst2: See Vtt 1st key construction0 table for           */
   /* details on instruction                                       */
   /* range: 17:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_inst2;

   /* Key_inst3: See Vtt 1st key construction0 table for           */
   /* details on instruction                                       */
   /* range: 35:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_inst3;

   /* AndMask:                                                     */
   /* range: 39:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD and_mask;

   /* OrMask:                                                      */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD or_mask;

   /* SemLookupEnable: If set, SEM lookup will be done.            */
   /* range: 44:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sem_lookup_enable;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL;

  /* Vtt2nd Key Construction: This table access uses the LLVP       */
  /* program index to retrieve the 2 instruction for SEM            */
  /* second lookup key generation.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00420000 */

   /* Key_inst0:                                                   */
   /* range: 17:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_inst0;

   /* Key_inst1:                                                   */
   /* range: 35:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_inst1;

   /* AndMask:                                                     */
   /* range: 39:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD and_mask;

   /* OrMask:                                                      */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD or_mask;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL;

  /* Ingress Vlan Edit Command Table: This table maps the           */
  /* incoming tag structure and the VLAN edit profile to the        */
  /* ingress vlan edit command. Accessed by                          */
  /* \{Packet-Is-MEF-L2CP, LLVP-Incoming-Tag-Structure,             */
  /* AC-VLAN-Edit-Profile\}.                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00430000 */

   /* Ivec: Ingress VLAN edit command.                             */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ivec;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL;

  /* Vlan Edit Pcp Dei Map: Accessed by                             */
  /* \{VLAN-Edit-PCP-DEI-Profile, PCP-DEI-Map-Key\}.                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00440000 */

   /* PcpDei:                                                      */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pcp_dei;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL;

  /* Pbb Cfm Table: Access with BVID.                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00450000 */

   /* PbbCfmMaxLevel: CFM max level                                */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pbb_cfm_max_level;

   /* PbbCfmTrapValid: CFM trap valid                              */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pbb_cfm_trap_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL;

  /* Sem Result Accessed:                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00460000 */

   /* SemResultAccessed: bit per SEM result tale entry, if         */
   /* set, SEM result (LIF) accessed.                              */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sem_result_accessed;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL;

  /* In Rif Config Table: In-Rif configuration table                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00470000 */

   /* Vrf: VRF value                                               */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vrf;

   /* UcRpfEnable: Unicast RPF enable                              */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD uc_rpf_enable;

   /* EnableRoutingMc: If set, MC routing is enabled.              */
   /* range: 9:9, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD enable_routing_mc;

   /* EnableRoutingUc: If set, UC routing is enabled.              */
   /* range: 10:10, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_routing_uc;

   /* EnableRoutingMpls: If set, MPLS routing is enabled.          */
   /* range: 11:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enable_routing_mpls;

   /* InRifCosProfile: COS-Profile in equal to                     */
   /* In-Rif-Cos-Profile -1.                                       */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD in_rif_cos_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL;

  /* Tc Dp Map Table: Traffic class and drop precedence map         */
  /* table. Accessed with \{Cos-Profile,                             */
  /* Cos-Profile-Map-Key-Lsb\}                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00480000 */

   /* TrafficClass: traffic class                                  */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD traffic_class;

   /* DropPrecedence: drop precedence                              */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD drop_precedence;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL;

  /* Pinfo Fer: port info for FER                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b00000 */

   /* LagLbKeyCount: Number of headers to use for the vector       */
   /* construction (zero to three)                                 */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_lb_key_count;

   /* LagLbKeyStart: 1'b0 - Header below the forwarding header     */
   /* is the first header used. 1'b1 - Forwarding header is        */
   /* the first header used.                                       */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_lb_key_start;

   /* LbProfile: Profile for accessing the LbPfcProfile Table      */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lb_profile;

   /* EcmpLbKeyCount: 1'b0 - Use one vector 1'b1 - Use two         */
   /* vectors                                                      */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ecmp_lb_key_count;

   /* LbBosSearch: If set then the LB vector construction          */
   /* searches for a BOS label.                                    */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lb_bos_search;

   /* LbIncludeBosHdr: If LbBosSearch then the Bos-header is       */
   /* included at the LB vector                                    */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lb_include_bos_hdr;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PINFO_FER_TBL;

  /* Ecmp Group Size: Reset Value for all linesValue: 9'd0          */
  /* Mask: 9'h1ff                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b10000 */

   /* EcmpGroupSize: Specifies the ECMP-group-size                 */
   /* range: 8:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ecmp_group_size;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL;

  /* Lb Pfc Profile: For each port profile and packet format        */
  /* code specifies for each header the load balancing vector       */
  /* index                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b20000 */

   /* LbVectorIndex3: LB-Vector-Profile for header 3               */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lb_vector_index3;

   /* LbVectorIndex1: LB-Vector-Profile for header 1               */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lb_vector_index1;

   /* LbVectorIndex5: LB-Vector-Profile for header 5               */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD lb_vector_index5;

   /* LbVectorIndex4: LB-Vector-Profile for header 4               */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD lb_vector_index4;

   /* LbVectorIndex2: LB-Vector-Profile for header 2               */
   /* range: 19:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD lb_vector_index2;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL;

  /* Lb Vector Program Map: Resete Value:Line 0: Value:             */
  /* 49'h0_0000_0000_0000 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* NoneLine 1: Value: 49'h1_5555_5554_0000 Mask:                  */
  /* 49'h1_FFFF_FFFF_FFFF // EthernetLine 2: Value:                 */
  /* 49'h1_0015_1500_4055 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* FCLine 3: Value: 49'h1_0000_1055_5500 Mask:                    */
  /* 49'h1_FFFF_FFFF_FFFF // IPv4Line 4: Value:                     */
  /* 49'h1_5555_5500_0400 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* IPv6Line 5: Value: 49'h0_5540_0000_0000 Mask:                  */
  /* 49'h1_FFFF_FFFF_FFFF // MPLSx1Line 6: Value:                   */
  /* 49'h0_5540_5540_0000 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* MPLSx2_label1Line 7: Value: 49'h0_0000_5540_0000 Mask:         */
  /* 49'h1_FFFF_FFFF_FFFF // MPLSx2_label2Line 8: Value:            */
  /* 49'h0_5540_5540_5540 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* MPLSx3_label1Line 9: Value: 49'h0_0000_5540_5540 Mask:         */
  /* 49'h1_FFFF_FFFF_FFFF // MPLSx3_label2Line a: Value:            */
  /* 49'h0_0000_0000_5540 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* MPLSx3_label3Line b: Value: 49'h0_0055_5500_0000 Mask:         */
  /* 49'h1_FFFF_FFFF_FFFF // TrillLine c: Value:                    */
  /* 49'h0_5555_0000_0000 Mask: 49'h1_FFFF_FFFF_FFFF //             */
  /* L4Reset Value:for the rest of the lines:Value:                 */
  /* 49'h0_0000_0000_0000 Mask: 49'h0_0000_0000_0000                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b30000 */

   /* ChunkSize: Chunk size for each LB-Vector-Index. Encoded      */
   /* as: 1'b0 - 4b 1'b1 - 8b                                      */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD chunk_size;

   /* ChunkBitmap: Chunk-Bitmap for each LB-Vector-Index 2'b00     */
   /* - append NULL to the LB-Vector 2'b01 - append (chunk n)      */
   /* to the LB-Vector 2'b10 - append (chunk n XOR chunk n+2)      */
   /* to the LB-Vector 2'b11 - append (chunk n XOR chunk n+4)      */
   /* to the LB-Vector                                             */
   /* range: 48:1, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD chunk_bitmap;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL;

  /* Fec Super Entry: The FEC-Super-Entry table is a single         */
  /* table with 8K entries, accessed with FEC-Pointer[13:1].        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b40000 */

   /* ProtectionPointer: protection pointer A value of '0'         */
   /* indicates no protection. A value of '-1' indicates           */
   /* Facility protection Otherwise, protection is Path            */
   /* range: 10:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD protection_pointer;

   /* EcmpGroupSizeIndex: 0 - no ECMP, i.e., single FEC 1-15 -     */
   /* mapping index to a configured ECMP group size between 2      */
   /* and 288 Note: the default group sizes for                    */
   /* ECMP-Group-Size-Index 1?15 of is 2?16 respectively           */
   /* range: 14:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ecmp_group_size_index;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL;

  /* Fec Entry[0..1]:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b50000 */

   /* FecEntry: The FEC entry table is divided into two            */
   /* tables. The various formats are detailed below.              */
   /* range: 44:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fec_entry;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_TBL;

  /* Fec Entry General: Here the values common to all FEC           */
  /* entry formats are described.                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b50001 */

   /* Destination: Destination - 16b encoding                      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL;

  /* Fec Entry Eth Or Trill: FEC entry format when                  */
  /* Forwarding-Code is Ethernet or TRILL.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b50002 */

   /* Destination: Destination - 16b encoding                      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* OutLifOrDistTreeNick: Dist-Tree-Nick (16) or Out-LIF         */
   /* (16) as indicated by Is-Out-LIF                              */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif_or_dist_tree_nick;

   /* IsOutLif: Determines the encoding of                         */
   /* OutLifOrDistTreeNick                                         */
   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_out_lif;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL;

  /* Fec Entry Ip: FEC entry format when Forwarding-Code is         */
  /* IP                                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b50003 */

   /* Destination: Destination - 16b encoding                      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* OutLifLsb: Out-LIF = Out-LIF-Type == 0 ? \{2'b00, 1'b0,      */
   /* Out-LIF-lsb (13)\} (EEP) : \{2'b10, 1'b0, Out-LIF-lsb        */
   /* (13)\} (AC)                                                  */
   /* range: 28:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif_lsb;

   /* UcRpfMode: Valid only if In-RIF-UC-RPF-Enable 1'b0 -         */
   /* Strict - SIP lookup must succeed and In-RIF must be          */
   /* equal to Out-RIF from SIP lookup 1'b1 - Loose - SIP          */
   /* lookup must succeed. Note: Success means that SIP lookup     */
   /* did not return the default FEC entry. NEED TO DEFINE         */
   /* DEFAULT ENTRY                                                */
   /* range: 29:29, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD uc_rpf_mode;

   /* McRpfMode: 2'b0 - no RPF check 2'b1 - Explicit -             */
   /* Expected In-RIF is explicitly specified in this entry        */
   /* (i.e. by the DIP lookup) 2'b2 - Use-SIP-As-Is 2'b3 -         */
   /* Use-SIP                                                      */
   /* range: 31:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mc_rpf_mode;

   /* RIF: Pointer to either an ARP-Entry, or Tunnel               */
   /* information as inferred from the destination type. A         */
   /* zero value indicates Null                                    */
   /* range: 43:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD rif;

   /* OutLifType: If reset then Out-LIF is EEP, else it is an      */
   /* AC.                                                          */
   /* range: 44:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif_type;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL;

  /* Fec Entry Default:                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b50004 */

   /* Destination: Destination - 16b encoding                      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;
   
   /* OutLif: MPLS or default - Out-Lif                            */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_lif;

   /* OutVsiLsb: MPLS or default - The Out-VSI is equal to         */
   /* \{4'b0, Out-VSI\}                                            */
   /* range: 43:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD out_vsi_lsb;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL;

  /* Fec Entry Accessed[0..1]: Flag indicating entry was            */
  /* accessed and its content was used for forwarding the           */
  /* packet.                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b70000 */

   /* FecEntryAccessed:                                            */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fec_entry_accessed;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL;

  /* Path Select: Indicate which fec entry (Even/Odd) in case       */
  /* protection is path                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00b90000 */

   /* PathSelect0:                                                 */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select0;

   /* PathSelect1:                                                 */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select1;

   /* PathSelect2:                                                 */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select2;

   /* PathSelect3:                                                 */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select3;

   /* PathSelect4:                                                 */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select4;

   /* PathSelect5:                                                 */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select5;

   /* PathSelect6:                                                 */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select6;

   /* PathSelect7:                                                 */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD path_select7;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PATH_SELECT_TBL;

  /* Destination Status: Valid bit for each destination             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00ba0000 */

   /* DestinationValid0:                                           */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD destination_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_DESTINATION_STATUS_TBL;

  /* Fwd Act Profile:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00bb0000 */

   /* FwdActTrap: Flag that indicates that the a CPU-Trap-Code     */
   /* (and corresponding Cpu-Trap-Qualifier) should be             */
   /* inserted into the PP-Header                                  */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fwd_act_trap;

   /* FwdActControl: Flag indicating that certain filters          */
   /* should not be applied                                        */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fwd_act_control;

   /* FwdActDestination:                                           */
   /* range: 18:2, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fwd_act_destination;

   /* FwdActDestinationValid: If set, Destination field in the     */
   /* forwarding action should be replaced with the value          */
   /* above                                                        */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_destination_valid;

   /* FwdActDestinationAddVsi: If set, the resolved                */
   /* Destination will equal Destination+VSI                       */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_destination_add_vsi;

   /* FwdActDestinationAddVsiShift:                                */
   /* range: 22:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_destination_add_vsi_shift;

   /* FwdActTrafficClass:                                          */
   /* range: 25:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_traffic_class;

   /* FwdActTrafficClassValid: If set, Traffic-Class field in      */
   /* the forwarding action should be replaced with the value      */
   /* above                                                        */
   /* range: 26:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_traffic_class_valid;

   /* FwdActDropPrecedence:                                        */
   /* range: 28:27, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_drop_precedence;

   /* FwdActDropPrecedenceValid: If set, Drop-Precedence field     */
   /* in the forwarding action should be replaced with the         */
   /* value above                                                  */
   /* range: 29:29, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_drop_precedence_valid;

   /* FwdActMeterPointer:                                          */
   /* range: 42:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_meter_pointer;

   /* FwdActMeterPointerSelector: 0 - Set as Meter-Pointer A 1     */
   /* - Set as Meter-Pointer B                                     */
   /* range: 43:43, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_meter_pointer_selector;

   /* FwdActCounterPointer:                                        */
   /* range: 55:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_counter_pointer;

   /* FwdActCounterPointerSelector: 0 - Set as Counter-Pointer     */
   /* A 1 - Set as Counter-Pointer B                               */
   /* range: 56:56, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_counter_pointer_selector;

   /* FwdActDropPrecedenceMeterCommand: Identifies how to          */
   /* apply the meter result to the packet copy made with this     */
   /* action                                                       */
   /* range: 58:57, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_drop_precedence_meter_command;

   /* FwdActDropPrecedenceMeterCommandValid:                       */
   /* range: 59:59, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_drop_precedence_meter_command_valid;

   /* FwdActFwdOffsetIndex:                                        */
   /* range: 62:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_fwd_offset_index;

   /* FwdActFwdOffsetIndexValid: If set, Offset-Index field in     */
   /* the forwarding action should be replaced with the value      */
   /* above                                                        */
   /* range: 63:63, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_fwd_offset_index_valid;

   /* FwdActEthernetPolicerPointer: Ethernet Policer to apply      */
   /* to the packet. This policer result affects the DP that       */
   /* is used the ingress TM                                       */
   /* range: 72:64, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_ethernet_policer_pointer;

   /* FwdActEthernetPolicerPointerValid: If set,                   */
   /* Ethernet-Policer-Pointer field in the forwarding action      */
   /* should be replaced with the value above                      */
   /* range: 73:73, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_ethernet_policer_pointer_valid;

   /* FwdActLearnDisable: If set, then learning is disabled        */
   /* for this forward action                                      */
   /* range: 74:74, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fwd_act_learn_disable;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL;

  /* Snoop Action: Translate Snoop-Code to Snoop-Action             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00bc0000 */

   /* SnoopAction: Snoop-Action                                    */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD snoop_action;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_SNOOP_ACTION_TBL;

  /* Pinfo Flp: port info for FLP                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* ActionProfileSaNotFoundIndex: Selects one of four            */
   /* profile actions in case SA was not found in the SA           */
   /* lookup                                                       */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_profile_sa_not_found_index;

   /* LearnEnable: Enables learning for sa-lookup                  */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD learn_enable;

   /* ActionProfileSaDropIndex: Selects one of four profile        */
   /* actions in case SA-Drop indication is set in the SA          */
   /* lookup result.                                               */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_profile_sa_drop_index;

   /* EnableUnicastSameInterfaceFilter: If set then this port      */
   /* filters unicast packets sent to the same interface they      */
   /* came from                                                    */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD enable_unicast_same_interface_filter;

   /* ProgramTranslationProfile: Profile to translate the          */
   /* program                                                      */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program_translation_profile;

   /* SaLookupEnable: Enables SA-lookup                            */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD sa_lookup_enable;

   /* ActionProfileDaNotFoundIndex: Selects one of four            */
   /* profile actions in case DA was not found                     */
   /* range: 9:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_profile_da_not_found_index;
   /* TransparentP2pServiceEnable */
   /* Enable P2P service */
   /* range: 11:11, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD transparent_p2p_service_enable;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PINFO_FLP_TBL;

  /* Program Translation Map: Translate Program selected            */
  /* based on the forwarding-code to a new                          */
  /* program\{Program-index(4b), TTC==NONE(1b),                     */
  /* In-Port. ProgramTranslationProfile(1b)\}Reset Value:Line        */
  /* 0: Value: 16'h0000 Mask: 16'hFFFF // P2PLine 1: Value:         */
  /* 16'h1111 Mask: 16'hFFFF // ETH_COMPATIBLE_MCLine 2:            */
  /* Value: 16'h2222 Mask: 16'hFFFF //                              */
  /* ETH_NOT_COMPATIBLE_MCLine 3: Value: 16'h3333 Mask:             */
  /* 16'hFFFF // TRILL_MCLine 4: Value: 16'h4444 Mask:              */
  /* 16'hFFFF // TRILL_UCLine 5: Value: 16'h5555 Mask:              */
  /* 16'hFFFF // IPV4UC_L3VPN_EN_RPF_EN Line 6: Value:              */
  /* 16'h6666 Mask: 16'hFFFF // IPV4UC_L3VPN_EN_RPF_DISLine         */
  /* 7: Value: 16'h7777 Mask: 16'hFFFF //                           */
  /* IPV4UC_L3VPN_DIS_RPF_ENLine 8: Value: 16'h8888 Mask:           */
  /* 16'hFFFF // IPV4UC_L3VPN_DIS_RPF_DISLine 9: Value:             */
  /* 16'h9999 Mask: 16'hFFFF // IPV4MC_L3VPN_EN Line a:             */
  /* Value: 16'hAAAA Mask: 16'hFFFF // IPV4MC_L3VPN_DISLine         */
  /* b: Value: 16'hBBBB Mask: 16'hFFFF // IPV6UCLine c:             */
  /* Value: 16'hCCCC Mask: 16'hFFFF // IPV6MCLine d: Value:         */
  /* 16'hDDDD Mask: 16'hFFFF // LSRLine e: Value: 16'hEEEE          */
  /* Mask: 16'hFFFF // UD                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00110000 */

   /* Program11: Program for (TTC == NONE) &                       */
   /* (ProgramTranslationProfile = 1)                              */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program11;

   /* Program10: Program for (TTC == NONE) &                       */
   /* (ProgramTranslationProfile = 0)                              */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program10;

   /* Program01: Program for (TTC != NONE) &                       */
   /* (ProgramTranslationProfile = 1)                              */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program01;

   /* Program00: Program for (TTC != NONE) &                       */
   /* (ProgramTranslationProfile = 0)                              */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD program00;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL;

  /* Flp Key Program Map: Program Instruction Table                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00120000 */

   /* KeyA_Instruction0: 1st instruction to perform on key-A       */
   /* (64b)                                                        */
   /* range: 17:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_a_instruction0;

   /* KeyA_Instruction1: 2nd instruction to perform on key-A       */
   /* (64b)                                                        */
   /* range: 35:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_a_instruction1;

   /* KeyB_Instruction0: 1st instruction to perform on key-B       */
   /* (72b)                                                        */
   /* range: 53:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_b_instruction0;

   /* KeyB_Instruction1: 2nd instruction to perform on key-B       */
   /* (72b)                                                        */
   /* range: 71:54, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_b_instruction1;

   /* KeyB_Instruction2: 3rd instruction to perform on key-B       */
   /* (72b)                                                        */
   /* range: 89:72, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD key_b_instruction2;

   /* KeyB_Instruction3: 4th instruction to perform on key-B       */
   /* (72b)                                                        */
   /* range: 107:90, access type: RW, default value: 0x0           */
   SOC_PETRA_TBL_FIELD key_b_instruction3;

   /* ElkLkpValid: Perform ELK-lookup with Lem_2ndLkp key          */
   /* range: 108:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD elk_lkp_valid;

   /* Lem_1stLkpValid: Perform L-EM 1st lookup                     */
   /* range: 109:109, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_1st_lkp_valid;

   /* Lem_1stLkpKeySelect: For the 1st lookup in L-EM indicate     */
   /* if to use 0: \{FID, SA\} 1: key-A                            */
   /* range: 110:110, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_1st_lkp_key_select;

   /* Lem_1stLkpKeyType: indicate if 1st lookup in L-EM is 0:      */
   /* Destination lookup - no AGET access 1: Source lookup -       */
   /* with AGET access                                             */
   /* range: 111:111, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_1st_lkp_key_type;

   /* Lem_1stLkpAndValue: Specifies how to modify the 4 MSB of     */
   /* L-EM key. The modification is 'bitwise AND' on the MSB       */
   /* of the key. First the AND mask is applied, and then the      */
   /* OR mask                                                      */
   /* range: 115:112, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_1st_lkp_and_value;

   /* Lem_1stLkpOrValue: Specifies how to modify the 4 MSB of      */
   /* L-EM key. The modification is 'bitwise AND' on the MSB       */
   /* of the key. First the AND mask is applied, and then the      */
   /* OR mask                                                      */
   /* range: 119:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_1st_lkp_or_value;

   /* Lem_2ndLkpValid: Perform L-EM 2nd lookup                     */
   /* range: 120:120, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_2nd_lkp_valid;

   /* Lem_2ndLkpKeySelect: For the 2nd lookup in L-EM indicate     */
   /* if to use 0: Key-A 1: Key-B                                  */
   /* range: 121:121, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_2nd_lkp_key_select;

   /* Lem_2ndLkpAndValue: Look at Lem_1stLkAndValue                */
   /* range: 125:122, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_2nd_lkp_and_value;

   /* Lem_2ndLkpOrValue: Look at Lem_1stLkpOrValue                 */
   /* range: 129:126, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lem_2nd_lkp_or_value;

   /* Lpm_1stLkpValid: Indicate if to search with key-A the        */
   /* 1st lookup                                                   */
   /* range: 130:130, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_1st_lkp_valid;

   /* Lpm_1stLkpAndValue: Look at Lem_1stLkAndValue                */
   /* range: 132:131, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_1st_lkp_and_value;

   /* Lpm_1stLkpOrValue: Look at Lem_1stLkpOrValue                 */
   /* range: 134:133, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_1st_lkp_or_value;

   /* Lpm_2ndLkpValid: Indicate if to search with key-B the        */
   /* 2nd lookup                                                   */
   /* range: 135:135, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_2nd_lkp_valid;

   /* Lpm_2ndLkpAndValue: Look at Lem_1stLkAndValue                */
   /* range: 137:136, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_2nd_lkp_and_value;

   /* Lpm_2ndLkpOrValue: Look at Lem_1stLkpOrValue                 */
   /* range: 139:138, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD lpm_2nd_lkp_or_value;

   /* TcamKeySize: 0:- 72 bit search with key-B 1: 144 bit         */
   /* Search with \{8'b0, Key-B, Key-A\}                           */
   /* range: 140:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD tcam_key_size;

   /* TcamBankValid: Bitmap - which tcam to perform the search     */
   /* range: 144:141, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD tcam_bank_valid;

   /* TcamAndValue: Look at Lem_1stLkAndValue                      */
   /* range: 148:145, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD tcam_and_value;

   /* TcamOrValue: Look at Lem_1stLkpOrValue                       */
   /* range: 152:149, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD tcam_or_value;

   /* DataProcessingProfile: Index to the way lookup data is       */
   /* processed                                                    */
   /* range: 156:153, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD data_processing_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL;

  /* Unknown Da Action Profiles: Maps \{DA-Type[0..2],              */
  /* Orientation-Is-Hub(1), DA-Not-Found-Profile-Index              */
  /* (2)\}to an Action-ProfileDa-Type is: 0: MC1: BC2:              */
  /* UCReset Value for all linesValue: 8'd0 Mask: 8'h1f             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00130000 */

   /* CpuTrapCodeLsb: cpu-trap code is                             */
   /* \{CONST_PP_CPU_TRAP_CODE_FLP_UNKNOWN_DA_PREFIX,              */
   /* CpuTrapCodeLsb\}                                             */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cpu_trap_code_lsb;

   /* Snp: Strength of Snoop Action                                */
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD snp;

   /* Fwd: Strength of Forward Action                              */
   /* range: 7:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fwd;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL;

  /* Vrf Config: Maps for each VRF(8b) a                            */
  /* Default-Fec-Pointer:\{VRF,0\} : For IPv4-UC\{VRF,1\} :         */
  /* For IPv4-MC                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00140000 */

   /* Destination: Destination - Used EM-Encoding                  */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD destination;

   /* L3vpnDefaultRouting: If set then default routing is          */
   /* attempted for this VRF                                       */
   /* range: 16:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD l3vpn_default_routing;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_VRF_CONFIG_TBL;

  /* Header Profile: Maps each of 8 possible Header profile         */
  /* table                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00d10000 */

   /* HeaderProfileLearnDisable: If set, packets with this         */
   /* header-profile will not generate PPH-Learn-Extension         */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_learn_disable;

   /* HeaderProfileAlwaysAddPphLearnExt: If set, then a PPH        */
   /* learn extension is always added to the PPH                   */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_always_add_pph_learn_ext;

   /* HeaderProfileStVsqPtrTcMode: Determines the statistic        */
   /* VSQ pointer format: 00 - User Defined 01 - LLFC 10 -         */
   /* CBFC2 11 - CBFC8                                             */
   /* range: 3:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_st_vsq_ptr_tc_mode;

   /* HeaderProfileBuildPph: If set, packets with this             */
   /* header-profile will generate PPH                             */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_build_pph;

   /* HeaderProfileFtmhPphPresent: Dermines the value of the       */
   /* PPH-Present bit in the FTMH (when constructed)               */
   /* range: 5:5, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_ftmh_pph_present;

   /* HeaderProfileBuildFtmh: If set, packets with this            */
   /* header-profile will generate FTMH                            */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD header_profile_build_ftmh;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_HEADER_PROFILE_TBL;

  /* Snp Act Profile:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00d20000 */

   /* SnpActSnpSamplingProbability: Probability of Snoop           */
   /* execution. Probability is calculated as follows:             */
   /* SnpActSnpSamplingProbability/1024                     */
   /* range: 9:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD snp_act_snp_sampling_probability;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL;

  /* Mrr Act Profile:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00d30000 */

   /* MrrActMrrSamplingProbability: Probability of Mirror          */
   /* execution. Probability is calculated as follows:             */
   /* MrrActMrrSamplingProbability/1024                     */
   /* range: 9:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD mrr_act_mrr_sampling_probability;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL;

  /* Lpm1: Lpm level 1                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00e00000 */

   /* Lpm1:                                                        */
   /* range: 22:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD lpm;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_LPM1_TBL;

  /* Pinfo PMF Key Gen Var:                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00320000 */

   /* PinfoPMF_KeyGenVar: Defines general variable per             */
   /* In-PP-Port that can be used in the programmable keys.        */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pinfo_pmf_key_gen_var;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL;

  /* Program Selection Map0:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00330000 */

   /* ProgramSelectionMap0: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table: EEI[15:8]      */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map0;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL;

  /* Program Selection Map1:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00340000 */

   /* ProgramSelectionMap1: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table: EEI[7:0]       */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map1;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL;

  /* Program Selection Map2:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00350000 */

   /* ProgramSelectionMap2: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* SEM-Index[7:0]                                               */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map2;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL;

  /* Program Selection Map3:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00360000 */

   /* ProgramSelectionMap3: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* \{Packet-Format-Qualifier[0][1:0], SEM-Index[13:8]\}         */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map3;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL;

  /* Program Selection Map4:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00370000 */

   /* ProgramSelectionMap4: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* \{Forwarding-Code (4), Tunnel-Termination-Code (4)\}         */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map4;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL;

  /* Program Selection Map5:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00380000 */

   /* ProgramSelectionMap5: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* \{Large/Small-EM-First/Second-Lkp-Found (4),                 */
   /* LPM-First/Second-Lkp-Not-Default (2), TCAM-Found (1),        */
   /* ELK-Found (1)\}                                              */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map5;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL;

  /* Program Selection Map6:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00390000 */

   /* ProgramSelectionMap6: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* \{Parser-PMF-Profile (4), Port-PMF-Profile (3)\}             */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map6;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL;

  /* Program Selection Map7:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003a0000 */

   /* ProgramSelectionMap7: Used to select one of 31 programs      */
   /* that the PMF executes. Address to this table:                */
   /* \{LLVP-Incoming-Tag-Structure (4),                           */
   /* Packet-Format-Code. PMF-Profile (3)\}                         */
   /* range: 30:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD program_selection_map7;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL;

  /* Program Vars Table: This table holds the variables for         */
  /* each program. Table Address: ProgramSelelectTable-output.       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003b0000 */

   /* FirstPassKeyProfileIndex: Selects one of 8 profiles for      */
   /* the First-Pass-TCAM-Lookup. Used as address to the           */
   /* following tables: TCAM_1stPassKeyProfileResolvedData         */
   /* Direct_1stPassKeyProfileResolvedData                         */
   /* FEMXX_1stPassKeyProfileResolvedData                          */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD first_pass_key_profile_index;

   /* SecondPassKeyProfileIndex: Selects one of 8 profiles for     */
   /* the First-Pass-TCAM-Lookup. Used as address to the           */
   /* following tables: TCAM_2ndPassKeyProfileResolvedData         */
   /* Direct_2ndPassKeyProfileResolvedData                         */
   /* FEMXX_2ndPassKeyProfileResolvedData                          */
   /* (Second-Pass-Profile-Action will override this value)        */
   /* range: 5:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD second_pass_key_profile_index;

   /* TagSelectionProfileIndex: Selects one of 8 profiles to       */
   /* generate: 1) statistics tag packet data 2)                   */
   /* lag_lb_key_select 3) ecmp_lb_key_select 4)                   */
   /* stacking_route_history_bitmap Used to select on field of     */
   /* the following registers: StatisticsKeySelect                 */
   /* LAG_LB_KeySelect ECMP_LB_KeySelect                           */
   /* StackingRouteHistorySelect                                   */
   /* range: 8:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tag_selection_profile_index;

   /* BytesToRemoveHeader: Specifies which header is the base      */
   /* for removal. Encoded as follows: 2'b00 - start of packet     */
   /* 2'b01 - first header 2'b11 - forwarding header 2'b11 -       */
   /* header after the forwarding header                           */
   /* range: 10:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD bytes_to_remove_header;

   /* BytesToRemoveOffset: Number of bytes to remove from          */
   /* specified header, The number of bytes to remove is           */
   /* calculated as follows: Case (Bytes-To-Remove-Header)         */
   /* 2'b00: Header-Offsets[0] + Bytes-To-Remove-Offset 2'b01:     */
   /* Header-Offsets[1] + Bytes-To-Remove-Offset 2'b10:            */
   /* Header-Offsets[Forwarding-Offset-Index] +                    */
   /* Bytes-To-Remove-Offset 2'b11:                                */
   /* Header-Offsets[Forwarding-Offset-Index+1] +                  */
   /* Bytes-To-Remove-Offset                                       */
   /* range: 15:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bytes_to_remove_offset;

   /* SystemHeaderProfile: Controls how system headers are         */
   /* appended to the packet                                       */
   /* range: 18:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD system_header_profile;

   /* CopyProgramVariable: Used as Internal-Field-Variable         */
   /* that can be used to build the keys. (see program             */
   /* Instruction Table)                                           */
   /* range: 26:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD copy_program_variable;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL;

  /* Key A Program Instruction Table[0..3]: Holds two               */
  /* instructions per copy engine; each instruction copies to       */
  /* the key up to 32 bits from the packet header or from           */
  /* internal fields (PP-info).                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x003c0000 */

   /* Inst0_Valid: If set, then the instruction is valid and       */
   /* can updated the key; otherwise, the instruction doesn't      */
   /* update the key.                                              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_valid;

   /* Inst0_SourceSelect: 0 - source select is Packet Header 1     */
   /* - source select is Internal-Fields (PP-info)                 */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_source_select;

   /* Inst0_HeaderOffsetSelect: If (Source-Select ==               */
   /* Packet-Header), then this field selects one of 5 header      */
   /* offsets, which affects StartNibble pointer: StartNibble      */
   /* = HeaderOffsets[HeaderOffsetSelect] +NibbleFieldOffset       */
   /* else if (Source-Select == PP-info), then this field does     */
   /* NOT affect the Start Nibble, and in this case                */
   /* StartNibble = NibbleFieldOffset                              */
   /* range: 4:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_header_offset_select;

   /* Inst0_NiblleFieldOffset: If (Source-Select ==                */
   /* Packet-Header), then this field is represented in 2's        */
   /* complement and StartNibble =                                 */
   /* HeaderOffsets[HeaderOffsetSelect] +NibbleFieldOffset         */
   /* else if (Source-Select == PP-info), then this field is       */
   /* always is a positive value and StartNibble =                 */
   /* NibbleFieldOffset                                            */
   /* range: 12:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD inst0_niblle_field_offset;

   /* Inst0_BitCount: Concatenate to the key (BitCount+1) bits     */
   /* - up to 32 bits to the LSB of the key.                       */
   /* range: 17:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst0_bit_count;

   /* Inst1_Valid: See Inst0 description.                          */
   /* range: 18:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_valid;

   /* Inst1_SourceSelect: See Inst0 description.                   */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_source_select;

   /* Inst1_HeaderOffsetSelect: See Inst0 description.             */
   /* range: 22:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_header_offset_select;

   /* Inst1_NiblleFieldOffset: See Inst0 description.              */
   /* range: 30:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_niblle_field_offset;

   /* Inst1_BitCount: See Inst0 description.                       */
   /* range: 35:31, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_bit_count;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL;

  /* Key B Program Instruction Table[0..3]: See                     */
  /* KeyAProgramInstructionTable                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00400000 */

   /* Inst0_Valid: See KeyAProgramInstructionTable                 */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_valid;

   /* Inst0_SourceSelect: See KeyAProgramInstructionTable          */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_source_select;

   /* Inst0_HeaderOffsetSelect: See                                */
   /* KeyAProgramInstructionTable                                  */
   /* range: 4:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst0_header_offset_select;

   /* Inst0_NiblleFieldOffset: See KeyAProgramInstructionTable     */
   /* range: 12:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD inst0_niblle_field_offset;

   /* Inst0_BitCount: See KeyAProgramInstructionTable              */
   /* range: 17:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst0_bit_count;

   /* Inst1_Valid: See KeyAProgramInstructionTable                 */
   /* range: 18:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_valid;

   /* Inst1_SourceSelect: See KeyAProgramInstructionTable          */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_source_select;

   /* Inst1_HeaderOffsetSelect: See                                */
   /* KeyAProgramInstructionTable                                  */
   /* range: 22:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_header_offset_select;

   /* Inst1_NiblleFieldOffset: See KeyAProgramInstructionTable     */
   /* range: 30:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_niblle_field_offset;

   /* Inst1_BitCount: See KeyAProgramInstructionTable              */
   /* range: 35:31, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst1_bit_count;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL;

  /* TCAM 1st Pass Key Profile Resolved Data: This table            */
  /* defines with which keys the PMF accesses the TCAM. Table        */
  /* Address: FirstPassKeyProfileIndex from                         */
  /* Program-Vars-Table. Table Data: see detailed items below.       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00420000 */

   /* BankA_KeySelect: 1 : L2_Key <=> 144bit 2 : L3_Key <=>        */
   /* 144bit 3 : \{KeyBuff_B, KeyBuff_A\}; <=> 288bit 4 :          */
   /* KeyBuff_A[71:0] <=> 72bit 5 : KeyBuff_A[103:32] <=>          */
   /* 72bit 6 : KeyBuff_A[143:0] <=> 144bit 7 :                    */
   /* KeyBuff_A[175:32] <=> 144bit 8 : KeyBuff_B[71:0] <=>         */
   /* 72bit 9 : KeyBuff_B[103:32] <=> 72bit 10 :                   */
   /* KeyBuff_B[143:0] <=> 144bit 11 : KeyBuff_B[175:32] <=>       */
   /* 144bit else : No-Lookup                                      */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bank_a_key_select;

   /* BankA_DB_ID_AND_Value: specifies how to modify the 4 MSB     */
   /* of the key. The modification is 'bitwise AND' on the MSB     */
   /* of the selected key. First the AND mask is applied, and      */
   /* then the OR mask                                             */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bank_a_db_id_and_value;

   /* BankA_DB_ID_OR_Value: specifies how to modify the 4 MSB      */
   /* of the key. The modification is 'bitwise OR' on the MSB      */
   /* of the selected key. First the AND mask is applied, and      */
   /* then the OR mask                                             */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD bank_a_db_id_or_value;

   /* BankB_KeySelect: See above                                   */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_key_select;

   /* BankB_DB_ID_AND_Value: See above                             */
   /* range: 19:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_db_id_and_value;

   /* BankB_DB_ID_OR_Value: See above                              */
   /* range: 23:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_db_id_or_value;

   /* BankC_KeySelect: See above                                   */
   /* range: 27:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_key_select;

   /* BankC_DB_ID_AND_Value: See above                             */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_db_id_and_value;

   /* BankC_DB_ID_OR_Value: See above                              */
   /* range: 35:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_db_id_or_value;

   /* BankD_KeySelect: See above                                   */
   /* range: 39:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_key_select;

   /* BankD_DB_ID_AND_Value: See above                             */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_db_id_and_value;

   /* BankD_DB_ID_OR_Value: See above                              */
   /* range: 47:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_db_id_or_value;

   /* TCAM_PD1_Members: Defines which of the 4 banks is a          */
   /* member of Priority-Decoder1. Each bit represent one of       */
   /* the four banks.                                              */
   /* range: 51:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_pd1_members;

   /* TCAM_PD2_Members: Defines which two of the four banks is     */
   /* a member of Priority-Decoder2. Each two bits represent       */
   /* the bank number.                                             */
   /* range: 55:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_pd2_members;

   /* TCAM_SEL3_Member: Selects one of the four banks.             */
   /* range: 57:56, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_sel3_member;

   /* TCAM_SEL4_Member: Selects one of the four banks.             */
   /* range: 59:58, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_sel4_member;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* TCAM 2nd Pass Key Profile Resolved Data: This table            */
  /* defines with which keys the PMF accesses the TCAM. The         */
  /* keys may be updated by a second-pass-action. Table              */
  /* Address: If second-pass-action is valid, then the              */
  /* address is by SecondPassKeyProfileTable; else the              */
  /* address is by SecondPassKeyProfileIndex from                   */
  /* Program-Vars-Table                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00430000 */

   /* BankA_KeySelect: See TCAM_1stPassKeyProfileResolvedData      */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bank_a_key_select;

   /* BankA_DB_ID_AND_Value: See                                   */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bank_a_db_id_and_value;

   /* BankA_DB_ID_OR_Value: See                                    */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD bank_a_db_id_or_value;

   /* BankB_KeySelect: See TCAM_1stPassKeyProfileResolvedData      */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_key_select;

   /* BankB_DB_ID_AND_Value: See                                   */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 19:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_db_id_and_value;

   /* BankB_DB_ID_OR_Value: See                                    */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 23:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_b_db_id_or_value;

   /* BankC_KeySelect: See TCAM_1stPassKeyProfileResolvedData      */
   /* range: 27:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_key_select;

   /* BankC_DB_ID_AND_Value: See                                   */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_db_id_and_value;

   /* BankC_DB_ID_OR_Value: See                                    */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 35:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_c_db_id_or_value;

   /* BankD_KeySelect: See TCAM_1stPassKeyProfileResolvedData      */
   /* range: 39:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_key_select;

   /* BankD_DB_ID_AND_Value: See                                   */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 43:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_db_id_and_value;

   /* BankD_DB_ID_OR_Value: See                                    */
   /* TCAM_1stPassKeyProfileResolvedData                           */
   /* range: 47:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank_d_db_id_or_value;

   /* TCAM_PD1_Members: See TCAM_1stPassKeyProfileResolvedData     */
   /* range: 51:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_pd1_members;

   /* TCAM_PD2_Members: See TCAM_1stPassKeyProfileResolvedData     */
   /* range: 55:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_pd2_members;

   /* TCAM_SEL3_Member: See TCAM_1stPassKeyProfileResolvedData     */
   /* range: 57:56, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_sel3_member;

   /* TCAM_SEL4_Member: See TCAM_1stPassKeyProfileResolvedData     */
   /* range: 59:58, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_sel4_member;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* Direct Action Table:                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00460000 */

   /* DirectActionTable: The table is accessed twice-in the        */
   /* first pass lookup and in the second pass lookup. In the      */
   /* first-pass, it is accessed according to                      */
   /* Direct_1stPassKeyProfileResolvedData. DirectKeySelect,        */
   /* and in the second pass it is accessed according to           */
   /* Direct_2ndPassKeyProfileResolvedData. DirectKeySelect.        */
   /* The table data width is 20 bit, which can be later used      */
   /* in the Field-Extraction-Macro                                */
   /* range: 19:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD direct_action_table;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL;


  /* Tcam Banks:                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00f00000  */
   /* TcamBank: TCAM bank                                         */
   /* range: 0:0, whether to write/read vbi.            */
   SOC_PETRA_TBL_FIELD vbe;
   /* range: 1:1, mskdatae  to read/write the data/mask or not.
      access type: W, default value: 0x0   */
   SOC_PETRA_TBL_FIELD mskdatae;
   /* range: 2:2, set for Write  access type: W, default value: 0x0           */
   SOC_PETRA_TBL_FIELD wr;
   /* range: 3:3, set for Read, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD rd;
   /* range: 4:4, set for compare, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cmp;
   /* range: 5:5, set for clear all entries access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD flush;
   /* range: 149:6, value for write/compare, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD di;
   /* range: 150:150, valid or not valid entry. access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vbi;
   /* range: 152:151, 1 --> 144, 2 --> 72, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cfg;
   /* range: 156:153, 144 --> 1111, 72 --> 0011 (first) or 1100 second),
   access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD hqsel;
   /* range: 300:157 mask, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mdi;
   /* range: 301:301 mask, access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD reserved;
   /* range: 310:302, address  offset inside the bank. (taken from address).
   access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD adr;
   /* range: 312:311, bank index. (taken from address).
   access type: W, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bank;
} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_BANK_IN_TBL;

/* Tcam Banks:                                                   */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* : 0x00f00000  */
  /* TCAM bank                                         */
  /* range: 0:0, Is it valid entry! Output for read       */
  SOC_PETRA_TBL_FIELD vbo;
  /* range: 144:1, Value of the entry  access type: R, default value: 0x0   */
  SOC_PETRA_TBL_FIELD dataout;
  /* range: 155:145, Address offset in the bank. In sub words offset. set for Write  access type: R, default value: 0x0           */
  SOC_PETRA_TBL_FIELD hadr;
  /* range: 156:156, Hit Found or not. For compare, access type: R, default value: 0x0            */
  SOC_PETRA_TBL_FIELD hit;
  /* range: 157:157, multi-hit, access type: R, default value: 0x0            */
  SOC_PETRA_TBL_FIELD mhit;
  /* range: 301:158, mask access type: R, default value: 0x0            */
  SOC_PETRA_TBL_FIELD mdo;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_BANK_OUT_TBL;

  /* Tcam Action[0..3]: TCAM action table for each bank             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00f10000 */

   /* Low: Action for EGQ, PMF, FLP                                */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD low;

   /* High: Action for EGQ, PMF, FLP                               */
   /* range: 63:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD high;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_ACTION_TBL;

  /* Per Port Configuration Table(PPCT): Per Port                   */
  /* Configuration Table                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x002b0000 */

   /* OUT_PP_PORT: Outgoing packet processing port                 */
   /* range: 5:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD out_pp_port;

   /* MTU: Maximum transfer unit.                                  */
   /* range: 19:6, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD mtu;

   /* DST_SYSTEM_PORT_ID : Destination system port                 */
   /* range: 32:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dst_system_port_id;

   /* LB_KEY_MAX: Load-Balance maximum key                         */
   /* range: 40:33, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD lb_key_max;

   /* LB_KEY_MIN: Load-Balance minimum key                         */
   /* range: 48:41, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD lb_key_min;

   /* IS_STACKING_PORT: Set if port is stacking                    */
   /* range: 49:49, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD is_stacking_port;

   /* PEER_TM_DOMAIN_ID:                                           */
   /* range: 53:50, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD peer_tm_domain_id;

   /* PORT_TYPE: Port type                                         */
   /* range: 55:54, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_type;

   /* CNM_INTRCPT_FC_VEC_INDEX: OTM-Port To FC-Vector Index        */
   /* Table                                                        */
   /* range: 69:56, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cnm_intrcpt_fc_vec_index_5_0;

   /* CNM_INTRCPT_FC_VEC_INDEX: OTM-Port To FC-Vector Index        */
   /* Table                                                        */
   /* range: 69:56, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cnm_intrcpt_fc_vec_index_12_6;

   /* CNM_INTRCPT_FC_VEC_INDEX: OTM-Port To FC-Vector Index        */
   /* Table                                                        */
   /* range: 69:56, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cnm_intrcpt_fc_vec_index_13_13;

   /* CNM_INTRCPT_EN: CNM-Intercept-Enable bit per OTM-Port        */
   /* range: 70:70, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cnm_intrcpt_en;

   /* PORT_PROFILE: Per port profile used in TM resolution         */
   /* range: 73:73, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ad_count_out_port_flag;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_PPCT_TBL;

  /* packet processing Per Port Configuration Table(PP PPCT):       */
  /* Packet Processing Per Port Configuration Table                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x002c0000 */

   /* PVLAN_PORT_TYPE: PVLAN port type                             */
   /* range: 1:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD pvlan_port_type;

   /* ORIENTATION_IS_HUB: If set, then port orientation is         */
   /* HUB.                                                         */
   /* range: 2:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD orientation_is_hub;

   /* EEI_TYPE: EEI type                                           */
   /* range: 3:3, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD eei_type;

   /* UNKNOWN_BC_DA_ACTION_FILTER: If set, enables unknown         */
   /* broadcast DA filtering.                                      */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD unknown_bc_da_action_filter;

   /* UNKNOWN_MC_DA_ACTION_FILTER: If set, enables unknown         */
   /* multicast DA filtering.                                      */
   /* range: 5:5, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD unknown_mc_da_action_filter;

   /* UNKNOWN_UC_DA_ACTION_FILTER: If set, enables unknown         */
   /* unicast DA filtering.                                        */
   /* range: 6:6, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD unknown_uc_da_action_filter;

   /* ENABLE_SRC_EQUAL_DST_FILTER: If set, enables source          */
   /* equal destination filtering.                                 */
   /* range: 7:7, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD enable_src_equal_dst_filter;

   /* ACCEPTABLE_FRAME_TYPE_PROFILE: Acceptable frame type         */
   /* profile.                                                     */
   /* range: 9:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD acceptable_frame_type_profile;

   /* EGRESS_VSI_FILTERING_ENABLE: If set, enables VSI             */
   /* filtering.                                                   */
   /* range: 10:10, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD egress_vsi_filtering_enable;

   /* DISABLE_FILTERING: If set, disables packet-processing        */
   /* filtering.                                                   */
   /* range: 11:11, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD disable_filtering;

   /* ACL_PROFILE: ACL profile. Used in Key resolution.            */
   /* range: 13:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD acl_profile;

   /* ACL_DATA: ACL data used in key creation.                     */
   /* range: 19:14, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD acl_data;

   /* DISABLE_LEARNING: Disable learning                           */
   /* range: 20:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD disable_learning;

   /* PORT_IS_PBP:                                                 */
   /* range: 21:21, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_is_pbp;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_PP_PPCT_TBL;

  /* Ingress Vlan Edit Command Map: Ingress VLAN editing            */
  /* command                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004b0000 */

   /* IngressVlanEditCommandMap: The Index is 6 bits width.        */
   /* Regarding 64 indexes, each index points to a 14-bit edit     */
   /* command. Maps the ingress VLAN edit command to an edit       */
   /* command. Contains the following fields: TPID-Profile (3)     */
   /* Outer-VID-Source (2) Outer-PCP-DEI-Source (2)                */
   /* Inner-VID-Source (2) Inner-PCP-DEI-Source (2)                */
   /* Tags-To-Remove (2)                                           */
   /* range: 16:0, access type: RW, default value: 0x0             */

   /* range: 2:0 */
   SOC_PETRA_TBL_FIELD tpid_profile_link;
   /* range: 4:3 */
   SOC_PETRA_TBL_FIELD edit_command_outer_vid_source;
   /* range: 6:5 */
   SOC_PETRA_TBL_FIELD edit_command_outer_pcp_dei_source;
   /* range: 8:7 */
   SOC_PETRA_TBL_FIELD edit_command_inner_vid_source;
   /* range: 10:9 */
   SOC_PETRA_TBL_FIELD edit_command_inner_pcp_dei_source;
   /* range: 12:11 */
   SOC_PETRA_TBL_FIELD edit_command_bytes_to_remove;
   /* range: 14:13 */
   SOC_PETRA_TBL_FIELD outer_tpid_ndx;
   /* range: 16:15 */
   SOC_PETRA_TBL_FIELD inner_tpid_ndx;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL;

  /* Vsi Membership Memory(Vsi Membership): VSI membership          */
  /* table                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00600000 */

   /* VsiMembership: Each entry is 64-bit bitmap. Each bit in      */
   /* the bitmap indicates whether the corresponding outgoing      */
   /* PP port is member in the VSI.                                */
   /* range: 63:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD vsi_membership;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL;

  /* Ttl Scope Memory(Ttl Scope): TTL scope table. This table        */
  /* defines TTL value for each outgoing VSI.                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00610000 */

   /* TtlScope: The VSI is the index of the table. The output      */
   /* is the TTL.                                                  */
   /* range: 7:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD ttl_scope;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_TTL_SCOPE_TBL;

  /* Auxiliary Data Memory(Aux Table): Auxiliary Data               */
  /* table. This table mode may be SPLIT HORIZON or PVLAN_TYPE       */
  /* mode according to AuxiliaryDataTableMode register.             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00620000 */

   /* Auxtable: When table mode is SPLIT HORIZON then: index       */
   /* is \{EEI[12:3], EEI[2:0]\}, and result is one bit            */
   /* indicating whether outgoing orientation is HUB or not.       */
   /* When table mode is PVLAN_TYPE then: index is                 */
   /* \{source-system-port[11:2],source_system_port[1:0]\},        */
   /* and result is 2 bit PVLAN type.                              */
   /* range: 7:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD auxtable;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_AUX_TABLE_TBL;

  /* Eep Orientation Memory(Eep Orientation): EEP orientation       */
  /* Table                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630000 */

   /* EepOrientation: The index to the table is \{EEP[12:3],       */
   /* EEP[2:0], and result is one bit indicating whether           */
   /* outgoing orientation is HUB or not.                          */
   /* range: 7:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD eep_orientation;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL;


  /* Cfm Trap: CFM trap information. OUT-LIF[13:0] is the            */
  /* index of the table.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00640000 */

   /* CfmMaxLevel: The Maximum Level per OUT-LIF. When             */
   /* CFM-Header. MD-Level <= CfmMaxLevel then trap the packet.     */
   /* range: 2:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD cfm_max_level;

   /* CfmTrapValid: If set then CFM filter is enabled.             */
   /* range: 3:3, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD cfm_trap_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_CFM_TRAP_TBL;


  /* Action Profile Table: Action profile table                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00670000 */

   /* ActionProfileTable: The index of this table is 3-bits        */
   /* width. The output is action profile which 33-bits width      */

   /* TBL DEF CHANGE: Replaced single 33b field with actual fields */

   /* range: 0:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD otm_valid;

   /* range: 7:1, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD otm;

   /* range: 8:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD discard;

   /* range: 11:9, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tc;

   /* range: 12:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tc_valid;

   /* range: 14:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dp;

   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dp_valid;

   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cud;

   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cud_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL;


/* Egress Encapsulation Entry - MPLS Tunnel Format:               */
  /* MPLS1-Command is pop-into-Eth                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030001 */

   /* Next-EEP:                                                    */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD next_eep;

   /* Next-VSI-lsb: Next-VSI = \{2'b00, Next-VSI-lsb\} Only        */
   /* first 4K VSI may be routed                                   */
   /* range: 25:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD next_vsi_lsb;

   /* MPLS2-Label:                                                 */
   /* range: 45:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls2_label;

   /* MPLS2-Command:                                               */
   /* MPLS2 command is one of four 'push' options if               */
   /* MPLS2-Label is not NULL. The 4-bit command is                */
   /* {2'b0, MPLS2-Command}                                        */
   /* range: 47:46, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls2_command;

   /* Has-CW: If set then label has control word above it.         */
   /* range: 26:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD has_cw;

   /* TPID-Profile: Selects TPIDs and their meaning for            */
   /* Ethernet header parsing                                      */
   /* range: 28:27, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tpid_profile;

   /* MPLS1-Label:                                                 */
   /* range: 67:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls1_label;

   /* MPLS1-Command: 0 to 7: Push-0 to Push-7 (select a            */
   /* profile at egress) 8: pop-into-MPLS-pipe (no copy            */
   /* TTL/EXP) //LSR 9: pop-into-MPLS-uniform (copy TTL/EXP)       */
   /* //LSR 10: pop-into-IPv4-pipe //LSR 11:                       */
   /* pop-into-IPv4-uniform //LSR 12: pop-into-IPv6-pipe //LSR     */
   /* 13: pop-into-IPv6-uniform //LSR 14: pop-into-Eth //LSR       */
   /* 15: swap //LSR Note: 'nop' is encoded as Push-0 with         */
   /* (MPLS-Label == 0)                                            */
   /* range: 71:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mpls1_command;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL;

  /* Egress Encapsulation Entry - IP Tunnel Format:                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030002 */

   /* Next-EEP: IP tunnel must point to an link layer              */
   /* encapsulation entry.                                         */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD next_eep;

   /* Next-VSI-lsb: Next-VSI = \{2'b00, Next-VSI-lsb\}             */
   /* range: 25:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD next_vsi_lsb;

   /* GRE-Enable: If set then the IPv4 tunnel has a GRE            */
   /* header.                                                      */
   /* range: 26:26   , access type: RW, default value: 0x0         */
   SOC_PETRA_TBL_FIELD gre_enable;

   /* IPv4-TOS-Index: Index into a IPv4-TOS table                  */
   /* range: 30:27, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ipv4_tos_index;

   /* IPv4-TTL-Index: Index into a IPv4-TTL table                  */
   /* range: 32:31, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ipv4_ttl_index;

   /* IPv4-Src-Index: Index into a IPv4-Src table                  */
   /* range: 36:33, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ipv4_src_index;

   /* IPv4-Dst:                                                    */
   /* range: 68:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ipv4_dst;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL;

  /* Encapsulation-Memory Link Layer Entry Format:                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030003 */

   /* VID: Either the B-VID or designated VLAN, as interpreted     */
   /* by the application.                                          */
   /* range: 11:0 , access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD vid;

   /* VID-Valid: If set then this entry contains a VID Used in     */
   /* two applications PBB and TRILL                               */
   /* range: 12:12   , access type: RW, default value: 0x0         */
   SOC_PETRA_TBL_FIELD vid_valid;

   /* Dest-MAC:                                                    */
   /* range: 60:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dest_mac;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL;

/* TBL DEF CHANGE: use bank1/2 as a single logical table. Structures below
   are used for mapping between the logical tables above to the banks */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00030000/0x00040000 */
} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_BANK_TBL;

  /* Tx Tag Table: 4K entries - used by Link-Layer module.          */
  /* Index is Outer-VID. Used to determine VLAN tag                 */
  /* manipulation.                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00050000 */

   /* Entry: Index is Outer-VID. Each entry is bitmap of 64        */
   /* bits. For each PP port indicates: '0' - Not to transmit      */
   /* the Outer-VID. '1'- To transmit the Outer-VID.               */
   /* range: 63:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD entry;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL;

  /* Spanning Tree Protocol State Memory (STP):                     */
  /* Spanning-Tree-Protocol State Table                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00060000 */

   /* EgressStpState: Index is Topology-ID retrieved from          */
   /* Ingress Topology-ID table. Each entry is bitmap of 64        */
   /* bits. For each PP port indicates: '1' - Discard '0'-         */
   /* Forwarding                                                   */
   /* range: 63:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD egress_stp_state;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_STP_TBL;

  /* Small Em Result Memory: SEM Result table. These values          */
  /* are used to construct VLAN tags at the output.                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00070000 */

   /* PCP-DEI-Profile: These values are used to construct VLAN     */
   /* tags at the output                                           */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD pcp_dei_profile;

   /* VID2: These values are used to construct VLAN tags at        */
   /* the output                                                   */
   /* range: 15:4, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD vid2;

   /* VID1: These values are used to construct VLAN tags at        */
   /* the output                                                   */
   /* range: 27:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD vid[SOC_PB_PP_EPNI_SEM_RESULT_NOF_VIDS];

   /* VLAN-Edit-Profile: These values are used to construct        */
   /* VLAN tags at the output                                      */
   /* range: 31:28, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD vlan_edit_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL;

  /* Pcp Dei Table: Processor Configuration Parameters.             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00080000 */

   /* PcpEncTable: Set the primary VLAN bits [15:12]. The PCP      */
   /* is a table of 1024 entries. This table is accessed with      */
   /* index = \{PCP-DEI-Profile (4), PCP-DEI-Map-Key (6)\}         */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD pcp_enc_table;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL;

  /* PP PCT: Packet Processing Per Port Configuration               */
  /* Table. Table with 64 entries, mapping PP-Port to                */
  /* packet-processing parameters.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00090000 */

   /* ACCEPTABLE_FRAME_TYPE_PROFILE: Mapping profile for           */
   /* AcceptableFrameType table.                                   */
   /* range: 1:0  , access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD acceptable_frame_type_profile;

   /* PORT_IS_PBP: Port Is PBP.                                    */
   /* range: 2:2    , access type: UNDEF, default value: 0x0       */
   SOC_PETRA_TBL_FIELD port_is_pbp;

   /* EGRESS_STP_FILTER_ENABLE : not used yet                      */
   /* range: 3:3    , access type: UNDEF, default value: 0x0       */
   SOC_PETRA_TBL_FIELD egress_stp_filter_enable;

   /* EGRESS_VSI_FILTER_ENABLE : Enable VSI membership             */
   /* filtering.                                                   */
   /* range: 4:4    , access type: UNDEF, default value: 0x0       */
   SOC_PETRA_TBL_FIELD egress_vsi_filter_enable;

   /* DISABLE_FILTER : Disable filtering.                          */
   /* range: 5:5    , access type: UNDEF, default value: 0x0       */
   SOC_PETRA_TBL_FIELD disable_filter;

   /* DEFAULT_SEM_RESULT_0 : Default results for Small-EM          */
   /* database.                                                    */
   /* range: 6:6    , access type: UNDEF, default value: 0x0       */
   SOC_PETRA_TBL_FIELD default_sem_result_0;

   /* DEFAULT_SEM_RESULT_14_1 : Default results for Small-EM       */
   /* database.                                                    */
   /* range: 20:7, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD default_sem_result_14_1;

   /* EXP_MAP_PROFILE : Mapping profile for Ipv4ExpToTosMap        */
   /* and Ipv6ExpToTCMap tables.                                   */
   /* range: 22:21, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD exp_map_profile;

   /* C_TPID_INDEX :                                               */
   /* range: 24:23, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD c_tpid_index;

   /* S_TPID_INDEX :                                               */
   /* range: 26:25, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD s_tpid_index;

   /* TAG_PROFILE :                                                */
   /* range: 28:27, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tag_profile;

   /* VLAN_DOMAIN : VLAN Domain is a Soc_petra-B internal              */
   /* optimization mechanism. The motivation is to minimize        */
   /* the number of (in-Port/Out-Port x VID/VSI) keys that are     */
   /* used by Ingress and Egress PP for Service Delineation        */
   /* and VLAN translation. The concept is that several local      */
   /* ports that share the same VLAN definitions will be           */
   /* grouped together as a single "port" for the mapping          */
   /* mentioned above. Therefore, a VLAN-Domain (6b value) can     */
   /* be treated as a Local-Port(6b value), and in the basic       */
   /* implementation will have exact same value as the             */
   /* Local-Port.                                                  */
   /* range: 34:29, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD vlan_domain;

   /* EEI_TYPE :                                                   */
   /* range: 35:35, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD eei_type;

   /* CEP_C_VLAN_EDIT : The PP-Port is CEP port. Used for          */
   /* different Vlan manipulations.                                */
   /* range: 36:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cep_c_vlan_edit;

   /* LLVP_PROFILE:                                                */
   /* range: 39:37, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD llvp_profile;

   /* MPLS_ETHERTYPE_SELECT:                                       */
   /* range: 40:40, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD mpls_ethertype_select;

   /* COUNT_MODE:                                                  */
   /* range: 42:41, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD count_mode;

   /* COUNT_ENABLE:                                                */
   /* range: 43:43, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD count_enable;

   /* COUNTER_COMPENSATION:                                        */
   /* range: 49:44, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_compensation;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_PP_PCT_TBL;

  /* Llvp Table: Table with 256 entries, mapping                    */
  /* \{TPID-Profile (3), Packet-Tag-Format (5)\} to Vlan edit       */
  /* command parameters.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000a0000 */

   /* LLVP_C_TAG_OFFSET: These values are used to construct        */
   /* VLAN tags at the output                                      */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_c_tag_offset;

   /* LLVP_PACKET_HAS_C_TAG: These values are used to              */
   /* construct VLAN tags at the output                            */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_packet_has_c_tag;

   /* LLVP_PACKET_HAS_UP: These values are used to construct       */
   /* VLAN tags at the output                                      */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_packet_has_up;

   /* LLVP_PACKET_HAS_PCP_DEI: These values are used to            */
   /* construct VLAN tags at the output                            */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_packet_has_pcp_dei;

   /* LLVP_INCOMING_TAG_FORMAT: These values are used to           */
   /* construct VLAN tags at the output                            */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD llvp_incoming_tag_format;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_LLVP_TABLE_TBL;

  /* Egress Edit Cmd: Egress VLAN editing command                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000b0000 */

   /* EgressEditCmd: The index is 7 bits wide. Regarding 128       */
   /* indexes, each index points to a 16-bit edit command. The     */
   /* command encoded as follows: OUTER_TPID_INDEX 15:14           */
   /* INNER_TPID_INDEX 13:12 OUTER_VID_SOURCE 11:9                 */
   /* INNER_VID_SOURCE 8:6 OUTER_PCP_DEI_SOURCE 5:4                */
   /* INNER_PCP_DEI_SOURCE 3:2 TAGS_TO_REMOVE 1:0                  */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD outer_tpid_index;
   SOC_PETRA_TBL_FIELD inner_tpid_index;
   SOC_PETRA_TBL_FIELD outer_vid_source;
   SOC_PETRA_TBL_FIELD inner_vid_source;
   SOC_PETRA_TBL_FIELD outer_pcp_dei_source;
   SOC_PETRA_TBL_FIELD inner_pcp_dei_source;
   SOC_PETRA_TBL_FIELD tags_to_remove;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL;

  /* Program Vars: Programmable editor values. Mapping               */
  /* \{Out-TM-Port.profile(4),System-MC,PPH. Valid,EEI. Valid,Learn-Record. Valid,Fwd-Code(4)\}  */
  /* to the parameters in the table bellow.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000c0000 */

   /* ProgramIndex: Select one of the 16 program pointers.         */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD program_index;

   /* NewHeaderSize: The size of the constructed header            */
   /* range: 7:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD new_header_size;

   /* AddNetworkHeader: If set, then the PP header generated       */
   /* by the link layer editor is appended to the packet           */
   /* range: 8:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD add_network_header;

   /* RemoveNetworkHeader: If set, then the PP header, as          */
   /* indicated by the link layer editor, is removed from the      */
   /* packet                                                       */
   /* range: 9:9, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD remove_network_header;

   /* SystemHeaderSize: The number of bytes, containing the        */
   /* system headers, to remove from the header                    */
   /* range: 14:10, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD system_header_size;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_PROGRAM_VARS_TBL;

  /* Ingress Vlan Edit Command Map: Ingress VLAN editing            */
  /* command                                                        */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x004b0000 */

  /* IngressVlanEditCommandMap: The Index is 6 bits width.        */
  /* Regarding 64 indexes, each index points to a 14-bit edit     */
  /* command. Maps the ingress VLAN edit command to an edit       */
  /* command. Contains the following fields: TPID-Profile (3)     */
  /* Outer-VID-Source (2) Outer-PCP-DEI-Source (2)                */
  /* Inner-VID-Source (2) Inner-PCP-DEI-Source (2)                */
  /* Tags-To-Remove (2)                                           */
  /* range: 12:0, access type: RW, default value: 0x0             */

  /* range: 2:0 */
  SOC_PETRA_TBL_FIELD tpid_profile_link;
  /* range: 4:3 */
  SOC_PETRA_TBL_FIELD edit_command_outer_vid_source;
  /* range: 6:5 */
  SOC_PETRA_TBL_FIELD edit_command_outer_pcp_dei_source;
  /* range: 8:7 */
  SOC_PETRA_TBL_FIELD edit_command_inner_vid_source;
  /* range: 10:9 */
  SOC_PETRA_TBL_FIELD edit_command_inner_pcp_dei_source;
  /* range: 12:11 */
  SOC_PETRA_TBL_FIELD edit_command_bytes_to_remove;
  /* range: 14:13 */
  SOC_PETRA_TBL_FIELD outer_tpid_ndx;
  /* range: 16:15 */
  SOC_PETRA_TBL_FIELD inner_tpid_ndx;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL;

  /* Ip Exp Map:                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00140000 */

   /* IpExpMap:                                                    */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ip_exp_map;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_IP_EXP_MAP_TBL;

  /* Etpp Debug: Etpp Debug Information                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00150000 */

   /* EtppDebug:                                                   */
   /* range: 31:0, access type: R, default value: 0x0              */
   SOC_PETRA_TBL_FIELD etpp_debug;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_ETPP_DEBUG_TBL;


/* Esem management request                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00280000 */

   /* EtppDebug:                                                   */
   
   SOC_PETRA_TBL_FIELD esem_request;
} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_ESEM_MGMT_REQUEST_TBL;


/* Block definition: IHP  */
typedef struct
{
  SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL port_mine_table_physical_port_tbl;

  SOC_PB_PP_IHP_PINFO_LLR_TBL pinfo_llr_tbl;

  SOC_PB_PP_IHP_LLR_LLVP_TBL llr_llvp_tbl;

  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL ll_mirror_profile_tbl;

  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL subnet_classify_tbl;

  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL port_protocol_tbl;

  SOC_PB_PP_IHP_TOS_2_COS_TBL tos_2_cos_tbl;

  SOC_PB_PP_IHP_RESERVED_MC_TBL reserved_mc_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL large_em_pldt_format1_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL large_em_pldt_format1_asd_tp2p_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL large_em_pldt_format1_asd_pbp_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL large_em_pldt_format1_asd_ilm_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL large_em_pldt_format1_asd_sp2p_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL large_em_pldt_format1_asd_ethernet_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL large_em_pldt_format2_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL large_em_pldt_format2_trill_tbl;

  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL large_em_pldt_format2_auth_tbl;

  SOC_PB_PP_IHP_FLUSH_DB_TBL flush_db_tbl;

  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL large_em_fid_counter_db_tbl;

  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL large_em_fid_counter_profile_db_tbl;

  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL large_em_aging_configuration_table_tbl;

  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL tm_port_pp_context_config_tbl;

  SOC_PB_PP_IHP_PP_PORT_INFO_TBL pp_port_info_tbl;

  SOC_PB_PP_IHP_PP_PORT_VALUES_TBL pp_port_values_tbl;

  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL pp_port_fem_bit_select_table_tbl;

  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL pp_port_fem_map_index_table_tbl;

  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL pp_port_fem_field_select_map_tbl;

  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL src_system_port_fem_bit_select_table_tbl;

  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL src_system_port_fem_map_index_table_tbl;

  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL src_system_port_fem_field_select_map_tbl;

  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL pp_context_fem_bit_select_table_tbl;

  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL pp_context_fem_map_index_table_tbl;

  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL pp_context_fem_field_select_map_tbl;

  SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL parser_program1_tbl;

  SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL parser_program2_tbl;

  SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL packet_format_table_tbl;

  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL parser_custom_macro_parameters_tbl;

  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL parser_eth_protocols_tbl;

  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL parser_ip_protocols_tbl;

  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL parser_custom_macro_protocols_tbl;

  SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL isem_management_request_tbl;

  SOC_PB_PP_IHP_VSI_ISID_TBL vsi_isid_tbl;

  SOC_PB_PP_IHP_VSI_MY_MAC_TBL vsi_my_mac_tbl;

  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL vsi_topology_id_tbl;

  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL vsi_fid_class_tbl;

  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL vsi_da_not_found_tbl;

  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL bvd_topology_id_tbl;

  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL bvd_fid_class_tbl;

  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL bvd_da_not_found_tbl;

  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL fid_class_2_fid_tbl;

  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL vlan_range_compression_table_tbl;

  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL vtt_in_pp_port_vlan_config_tbl;

  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL designated_vlan_table_tbl;

  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL vsi_port_membership_tbl;

  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL vtt_in_pp_port_config_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL sem_result_table_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL sem_result_table_ac_p2p_to_ac_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL sem_result_table_ac_p2p_to_pwe_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL sem_result_table_ac_p2p_to_pbb_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL sem_result_table_ac_mp_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL sem_result_table_isid_p2p_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL sem_result_table_isid_mp_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL sem_result_table_trill_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL sem_result_table_ip_tt_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL sem_result_table_label_pwe_p2p_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL sem_result_table_label_pwe_mp_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL sem_result_table_label_vrl_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL sem_result_table_label_lsp_tbl;

  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL sem_opcode_ip_offsets_tbl;

  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL sem_opcode_tc_dp_offsets_tbl;

  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL sem_opcode_pcp_dei_offsets_tbl;

  SOC_PB_PP_IHP_STP_TABLE_TBL stp_table_tbl;

  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL vrid_my_mac_map_tbl;

  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL mpls_label_range_encountered_tbl;

  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL mpls_tunnel_termination_valid_tbl;

  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL ip_over_mpls_exp_mapping_tbl;

  SOC_PB_PP_IHP_VTT_LLVP_TBL vtt_llvp_tbl;

  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL llvp_prog_sel_tbl;

  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL vtt1st_key_construction0_tbl;

  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL vtt1st_key_construction1_tbl;

  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL vtt2nd_key_construction_tbl;

  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL ingress_vlan_edit_command_table_tbl;

  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL vlan_edit_pcp_dei_map_tbl;

  SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL pbb_cfm_table_tbl;

  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL sem_result_accessed_tbl;

  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL in_rif_config_table_tbl;

  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL tc_dp_map_table_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_TBLS;

/* Block definition: IHB  */
typedef struct
{
  SOC_PB_PP_IHB_PINFO_FER_TBL pinfo_fer_tbl;

  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL ecmp_group_size_tbl;

  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL lb_pfc_profile_tbl;

  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL lb_vector_program_map_tbl;

  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL fec_super_entry_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_TBL fec_entry_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL fec_entry_general_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL fec_entry_eth_or_trill_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL fec_entry_ip_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL fec_entry_default_tbl;

  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL fec_entry_accessed_tbl;

  SOC_PB_PP_IHB_PATH_SELECT_TBL path_select_tbl;

  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL destination_status_tbl;

  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL fwd_act_profile_tbl;

  SOC_PB_PP_IHB_SNOOP_ACTION_TBL snoop_action_tbl;

  SOC_PB_PP_IHB_PINFO_FLP_TBL pinfo_flp_tbl;

  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL program_translation_map_tbl;

  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL flp_key_program_map_tbl;

  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL unknown_da_action_profiles_tbl;

  SOC_PB_PP_IHB_VRF_CONFIG_TBL vrf_config_tbl;

  SOC_PB_PP_IHB_HEADER_PROFILE_TBL header_profile_tbl;

  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL snp_act_profile_tbl;

  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL mrr_act_profile_tbl;

  SOC_PB_PP_IHB_LPM1_TBL lpm_tbl[6];

  SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL pinfo_pmf_key_gen_var_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL program_selection_map0_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL program_selection_map1_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL program_selection_map2_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL program_selection_map3_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL program_selection_map4_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL program_selection_map5_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL program_selection_map6_tbl;

  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL program_selection_map7_tbl;

  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL program_vars_table_tbl;

  SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL key_a_program_instruction_table_tbl[SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS];

  SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL key_b_program_instruction_table_tbl[SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS];

  SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL tcam_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL tcam_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL direct_action_table_tbl;

  SOC_PB_PP_IHB_TCAM_BANK_IN_TBL tcam_bank_in_tbl;
  
  SOC_PB_PP_IHB_TCAM_BANK_OUT_TBL tcam_bank_out_tbl;

  SOC_PB_PP_IHB_TCAM_ACTION_TBL tcam_action_tbl[SOC_PB_TCAM_NOF_BANKS];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TBLS;

/* Block definition: EGQ  */
typedef struct
{
  SOC_PB_PP_EGQ_PPCT_TBL ppct_tbl;

  SOC_PB_PP_EGQ_PP_PPCT_TBL pp_ppct_tbl;

  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL ingress_vlan_edit_command_map_tbl;

  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL vsi_membership_tbl;

  SOC_PB_PP_EGQ_TTL_SCOPE_TBL ttl_scope_tbl;

  SOC_PB_PP_EGQ_AUX_TABLE_TBL aux_table_tbl;

  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL eep_orientation_tbl;

  SOC_PB_PP_EGQ_CFM_TRAP_TBL cfm_trap_tbl;

  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL action_profile_table_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_TBLS;

/* Block definition: EPN  */
typedef struct
{
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL egress_encapsulation_entry_mpls_tunnel_format_tbl;

  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL egress_encapsulation_entry_ip_tunnel_format_tbl;

  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL encapsulation_memory_link_layer_entry_format_tbl;

  /* TBL DEF CHANGE: use bank1/2 as a single logical table. Structs below are used for mapping between the logical tables above to the banks */
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_BANK_TBL egress_encapsulation_bank1_tbl;

  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_BANK_TBL egress_encapsulation_bank2_tbl;

  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL tx_tag_table_tbl;

  SOC_PB_PP_EPNI_STP_TBL stp_tbl;

  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL small_em_result_memory_tbl;

  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL pcp_dei_table_tbl;

  SOC_PB_PP_EPNI_PP_PCT_TBL pp_pct_tbl;

  SOC_PB_PP_EPNI_LLVP_TABLE_TBL llvp_table_tbl;

  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL egress_edit_cmd_tbl;

  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL program_vars_tbl;

  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL ingress_vlan_edit_command_map_tbl;

  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL ip_exp_map_tbl;

  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL etpp_debug_tbl;

  SOC_PB_PP_EPNI_ESEM_MGMT_REQUEST_TBL esem_mgmt_request_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_TBLS;
/* Blocks definition } */

typedef struct
{
  SOC_PB_PP_IHP_TBLS ihp;
  SOC_PB_PP_IHB_TBLS ihb;
  SOC_PB_PP_EGQ_TBLS egq;
  SOC_PB_PP_EPNI_TBLS epni;
} __ATTRIBUTE_PACKED__ SOC_PB_PP_TBLS;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* soc_pb_pp_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT SOC_PETRA_TBLS  **soc_pb_pp_tbls - pointer to soc_pb_pp_
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_pp_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_pb_pp_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_pp_tbls_get(
    SOC_SAND_OUT SOC_PB_PP_TBLS  **soc_pb_pp_tbls
  );

/*****************************************************
*NAME
* soc_pb_pp_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *soc_pb_pp_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
SOC_PB_PP_TBLS*
  soc_pb_pp_tbls(void);

/*****************************************************
*NAME
*  soc_pb_pp_tbls_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Soc_pb_pp_pp tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_pp_tbls_init(void);
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_CHIP_TBLS_INCLUDED__*/
#endif
