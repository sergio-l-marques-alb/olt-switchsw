/* $Id: pb_pp_chip_regs.h,v 1.9 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
* FILENAME:       soc_pb_pp_chip_regs.h
*
* MODULE PREFIX:  soc_pb_pp_chip_regs
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

#ifndef __SOC_PB_PP_CHIP_REGS_INCLUDED__
/* { */
#define __SOC_PB_PP_CHIP_REGS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_BLK_NOF_INSTANCES_ECI                                                                (1)
#define SOC_PB_PP_BLK_NOF_INSTANCES_OLP                                                                (1)
#define SOC_PB_PP_BLK_NOF_INSTANCES_IHP                                                                (1)
#define SOC_PB_PP_BLK_NOF_INSTANCES_IHB                                                                (1)
#define SOC_PB_PP_BLK_NOF_INSTANCES_EGQ                                                                (1)
#define SOC_PB_PP_BLK_NOF_INSTANCES_EPNI                                                               (1)

#define SOC_PB_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE                                       (2-0+1)
#define SOC_PB_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE                                              (3-0+1)
#define SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE                                                 (3-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_RANGE_LOW_REG_ARRAY_SIZE                                         (2-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_RANGE_HIGH_REG_ARRAY_SIZE                                        (2-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_VALUE_CONFIGS_REG_ARRAY_SIZE                                     (15-0+1)
#define SOC_PB_PP_IHB_REGS_TRAP_IF_ACCESSED_REG_ARRAY_SIZE                                             (1-0+1)
#define SOC_PB_PP_IHB_REGS_PACKET_HEADER_SIZE_RANGE_REG_ARRAY_SIZE                                     (2-0+1)

#define SOC_PB_PP_ECI_TPID_CONFIGURATION_REGISTER__REG_NOF_REGS                                        (2)
#define SOC_PB_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS                                            (2)
#define SOC_PB_PP_OLP_DSP_HEADER_REG_NOF_REGS                                                          (2)
#define SOC_PB_PP_OLP_DSP_HEADER_REG_MULT_NOF_REGS                                                     (8)
#define SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS                                                (6)
#define SOC_PB_PP_IHP_ISEM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS                                           (2)
#define SOC_PB_PP_IHP_LARGE_EM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS                                       (2)
#define SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS                                          (2)
#define SOC_PB_PP_IHP_MY_BMAC_UC_LSB_BITMAP_REG_MULT_NOF_REGS                                          (8)
#define SOC_PB_PP_IHP_PCP_DECODING_TABLE_REG_MULT_NOF_REGS                                             (4)
#define SOC_PB_PP_IHP_L4_PORT_RANGES_2_TC_TABLE_REG_MULT_NOF_REGS                                      (4)
#define SOC_PB_PP_IHP_DROP_PRECEDENCE_MAP_PCP_REG_MULT_NOF_REGS                                        (2)
#define SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS                                              (2)
#define SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS                                        (2)
#define SOC_PB_PP_IHB_CPU_LOOKUP_KEY_MSB_REG_REG_MULT_NOF_REGS                                         (3)
#define SOC_PB_PP_IHB_ALL_ESADI_RBRIDGES_REG_MULT_NOF_REGS                                             (2)
#define SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS                                           (4)
#define SOC_PB_PP_IHB_MEF_L2CP_DROP_BITMAP_REG_MULT_NOF_REGS                                           (4)
#define SOC_PB_PP_IHP_MEF_L2CP_TRANSPARENT_BITMAP_REG_MULT_NOF_REGS                                    (4)
#define SOC_PB_PP_EGQ_ACCEPTABLE_FRAME_TYPE_TABLE_REG_MULT_NOF_REGS                                    (4)
#define SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS                                                  (4)
#define SOC_PB_PP_EPNI_IPV4_TOS_REG_MULT_NOF_REGS                                                      (4)
#define SOC_PB_PP_EPNI_IPV4_TTL_REG_MULT_NOF_REGS                                                      (4)
#define SOC_PB_PP_EPNI_IPV4_SIP_REG_MULT_NOF_REGS                                                      (16)
#define SOC_PB_PP_EPNI_ITAG_TC_DP_MAP_REG_MULT_NOF_REGS                                                (4)

#define SOC_PB_PP_IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_NOF_FLDS                                    (4)
#define SOC_PB_PP_IHP_SEM_RESULT_OPCODES_NOF_FLDS                                                      (4)
#define SOC_PB_PP_IHP_TRAFFIC_CLASS_L4_RANGE_CFG_REG_NOF_FLDS                                          (3)
#define SOC_PB_PP_IHP_MPLS_EXP_REG_NOF_FLDS                                                            (8)

#define SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES                                                     (4)

#define SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS            (3)
#define SOC_PB_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG       (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS + 1)

#define SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS   (4)
#define SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES            (SOC_SAND_REG_SIZE_BITS / SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS)

#define SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS (5)
#define SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES               (4)

#define SOC_PB_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE                                          (2-0+1)
#define SOC_PB_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE                                                 (3-0+1)
#define SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE                                                    (3-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_RANGE_LOW_REG_ARRAY_SIZE                                            (2-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_RANGE_HIGH_REG_ARRAY_SIZE                                           (2-0+1)
#define SOC_PB_PP_IHP_REGS_MPLS_LABEL_VALUE_CONFIGS_REG_ARRAY_SIZE                                        (15-0+1)
#define SOC_PB_PP_IHB_REGS_TRAP_IF_ACCESSED_REG_ARRAY_SIZE                                                (1-0+1)
#define SOC_PB_PP_IHB_REGS_PACKET_HEADER_SIZE_RANGE_REG_ARRAY_SIZE                                        (2-0+1)
#define SOC_PB_PP_IHP_VRID_MY_MAC_CONFIG_REG_MULT_NOF_REGS                                                2
#define SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS                                               2
#define SOC_PB_PP_EPNI_IP_DSCP_TO_EXP_MAP_REG_MULT_NOF_REGS                                               6
#define SOC_PB_PP_EPNI_ACCEPTABLE_FRAME_TYPE_REG_MULT_NOF_REGS                                            2
#define PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS                                                            8
#define PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS                                                             8
#define PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_REGS                                                           (4)

#define SOC_PB_PP_REGS_TCAM_NOF_BANKS   4
#define SOC_PB_PP_REGS_TCAM_NOF_CYCLES  2

#define SOC_PB_PP_IHP_PMF_CPU_TRAP_CODE_PRO_NOF_REGS       (256/32)

#define SOC_PB_PP_IHP_PMF_PKT_HDR_SZ_RNG_NOF_REGS          (3)
#define SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_REGS              (5)
#define SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_FLDS              (2)

#define PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS            (4)

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

typedef SOC_PETRA_REG_ADDR             SOC_PB_PP_REG_ADDR;
typedef SOC_PETRA_REG_FIELD            SOC_PB_PP_REG_FIELD;
typedef struct
{
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6257 */

    /* LargeEmReqMffIsKey: If set, MactReqMff field contain         */
    /* \{System-VSI(16bit),MAC(48bit)\}, Else \{Key(63bit),         */
    /* Reserved(1bit)\}                                             */
    /* range: 0:0, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD large_em_req_mff_is_key;

    /* LargeEmReqMffMac: MAC                                        */
    /* range: 31:1, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD large_em_req_mff_mac;

  } __ATTRIBUTE_PACKED__ reg_0;

  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6258 */

    /* LargeEmReqMffMac: MAC                                        */
    /* range: 16:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD large_em_req_mff_mac;

  } __ATTRIBUTE_PACKED__ reg_1;

  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6259 */

    /* LargeEmReqMffKeyDbProfile: MSB of Key, used by OLP to        */
    /* determine action                                             */
    /* range: 0:0, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD large_em_req_mff_key_db_profile;

    /* LargeEmReqPartOfLag: Reserved, set to 1'b0                   */
    /* range: 1:1, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD large_em_req_part_of_lag;

    /* LargeEmReqCommand: Command type. May be one of the           */
    /* following: 3'b000 Delete, 3'b001 Insert 3'b010 Refresh       */
    /* 3'b011 Learn 3'b100 Defrag 3'b101 ACK 3'b110 Move 3'b111     */
    /* Change-fail                                                  */
    /* range: 4:2, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD large_em_req_command;

    /* LargeEmReqStamp: If different than zero then the EMC         */
    /* will generate an ACK event upon completion                   */
    /* range: 12:5, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD large_em_req_stamp;

    /* LargeEmReqPayloadDestination: 16 bit destination encoded     */
    /* as followed: \{1'b1, 1'b1, Flow-ID(14)\} \{1'b1, 1'b0,       */
    /* FEC-Pointer(14)\} \{1'b0, 1'b1, Multicast-ID(14)\}           */
    /* \{1'b0, 1'b0, 1'b1, Trap-Qualifier(8),                       */
    /* Action-Profile(5)\} \{1'b0, 1'b0, 1'b0,                      */
    /* System-Port-ID(13)\}                                         */
    /* range: 28:13, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_payload_destination;

    /* MactReqPayloadAsd: Application Specific Data                 */
    /* range: 31:29, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_payload_asd;

  } __ATTRIBUTE_PACKED__ reg_2;

  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x625a */

    /* MactReqPayloadAsd: Application Specific Data                 */
    /* range: 31:29, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_payload_asd;

    /* LargeEmReqPayloadIsDynamic: If set, then entry is            */
    /* dynamic; otherwise, static                                   */
    /* range: 21:21, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_payload_is_dynamic;

    /* LargeEmReqAgePayloadAgeStatus: Age status. Valid only        */
    /* for insert, refresh and learn commands                       */
    /* range: 23:22, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_age_payload_age_status;

    /* LargeEmReqAgePayload:                                        */
    /* range: 24:22, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_age_payload;

    /* LargeEmReqSuccess: Reserved                                  */
    /* range: 25:25, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_success;

    /* LargeEmReqReason: Reserved                                   */
    /* range: 27:26, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_reason;

    /* LargeEmReqResreved: Reserved                                 */
    /* range: 29:28, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_resreved;

    /* LargeEmReqSelf: If set, then the command source is a DSP     */
    /* generated by this device.                                    */
    /* range: 30:30, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_self;

    /* LargeEmReqQualifier: 0 - this request originated from        */
    /* the CPU interface. 1 - this request originated from the      */
    /* OLP interface. This determines where the ACK message         */
    /* will be sent to (0 - CPU reply FIFO, 1 - Event FIFO)         */
    /* range: 31:31, access type: RW, default value: 0x0            */
    SOC_PB_PP_REG_FIELD large_em_req_qualifier;

  } __ATTRIBUTE_PACKED__ reg_3;
} __ATTRIBUTE_PACKED__ SOC_PB_PP_LEM_ACCESS_FORMAT;

/* Blocks definition { */

/* Block definition: ECI 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* TPID-Configuration-Register-0: Up to four TPIDs are used       */
  /* by the device. This register contains two of these             */
  /* values.                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0013 */

     /* TPID0: Configurable TPID value.                              */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD tpid0;

     /* TPID1: Configurable TPID value.                              */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tpid1;

  } __ATTRIBUTE_PACKED__ tpid_configuration_register_reg[SOC_PB_PP_ECI_TPID_CONFIGURATION_REGISTER__REG_NOF_REGS];

  /* SA-Lookup-Type:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0015 */

     /* SA-Lookup-Type: Determines the SA lookup in the              */
     /* Large-EM. Decoded as follows: 2'b00: LL-PBP-SA-Lookup -      */
     /* Lookup for provider backbone ports. 2'b01:                   */
     /* LL-SA-Based-VID-or-Authentication - Lookup for SA based      */
     /* VID or authentication applications. 2'b10:                   */
     /* FLP-Learn-Lookup - Lookup for SA - standard bridge           */
     /* application.                                                 */
     /* range: 1:0, access type: RW, default value: 0x2              */
     SOC_PB_PP_REG_FIELD sa_lookup_type;

  } __ATTRIBUTE_PACKED__ sa_lookup_type_reg;

  /* System-Headers-Configurations0 Register: Configuration         */
  /* for system headers stack configuration.                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0016 */

     /* FTMH-Ext: Determines when an FTMH extension is present       */
     /* in the system headers as follows: 2'b00 - Never 2'b01 -      */
     /* System mutlicast packets only 2'b1x - Always                 */
     /* range: 1:0, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD ftmh_ext;

     /* FTMH-LB-Key-Ext-Enable: If set then an FTMH load             */
     /* balancing extension is present in the system headers         */
     /* stack.                                                       */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ftmh_lb_key_ext_enable;

     /* FTMH-LB-Key-Ext-Mode: If set, the FTMH load balancing        */
     /* extension contains an 8-bit load balancing key and an        */
     /* 8-bit stacking route history bitmap. Otherwise the           */
     /* extension contains a 16-bit stacking route history           */
     /* bitmap.                                                      */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ftmh_lb_key_ext_mode;

     /* Add-PPH-EEP-Ext: If set then a PPH EEP extension is          */
     /* present                                                      */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD add_pph_eep_ext;

     /* PPH-Soc_petraA-Compatible: If set then PPH is in soc_petra-A         */
     /* compatible mode. In this mode the PPH MFF contains the       */
     /* CUD (which is the ARP pointer in IP applications).           */
     /* range: 5:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD pph_petra_a_compatible;

  } __ATTRIBUTE_PACKED__ system_headers_configurations0_reg;

  /* TPID-Configuration-Register-2: TPID configuration              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x003a */

     /* ITAG-TPID: TPID value for an I-Tag                           */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD itag_tpid;

  } __ATTRIBUTE_PACKED__ tpid_configuration_register_2_reg;

  /* TPID-Profile-Configuration-Register: Tpid Profile Index        */
  /* configurations. Each of the four TPID profiles selects         */
  /* one of 4 possible TPIDs to search for.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x003f */

     /* TPID-Profile-Outer-Index0: 2-bit Outer-TPID index for        */
     /* every TPID profile. This vector is accessed with             */
     /* TPID-Profile(2).                                             */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tpid_profile_outer_index0;

     /* TPID-Profile-Outer-Index1: 2-bit Outer-TPID index for        */
     /* every TPID profile. This vector is accessed with             */
     /* TPID-Profile(2).                                             */
     /* range: 15:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD tpid_profile_outer_index1;

     /* TPID-Profile-Inner-Index0: 2-bit Inner-TPID index for        */
     /* every TPID profile. This vector is accessed with             */
     /* TPID-Profile(2).                                             */
     /* range: 23:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tpid_profile_inner_index0;

     /* TPID-Profile-Inner-Index1: 2-bit Inner-TPID index for        */
     /* every TPID profile. This vector is accessed with             */
     /* TPID-Profile(2).                                             */
     /* range: 31:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tpid_profile_inner_index1;

  } __ATTRIBUTE_PACKED__ tpid_profile_configuration_reg_reg;

  /* Ethernet-Type-Trill-Configuration-Register: Holds              */
  /* special Ethernet-Type values                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0087 */

     /* Ethertype-TRILL: Ethernet type for TRILL protocol.           */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ethertype_trill;

  } __ATTRIBUTE_PACKED__ ethernet_type_trill_configuration_reg_reg;

  /* Ethernet-Type-Mpls-Configuration-Register: Holds special       */
  /* Ethernet-Type values                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0088 */

     /* Ethertype-MPLS0: Ethernet type for MPLS0 protocol.           */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ethertype_mpls0;

  } __ATTRIBUTE_PACKED__ ethernet_type_mpls_configuration_reg_reg;

  /* My-TRILL-Nickname:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x0089 */

     /* My-TRILL-Nickname: The device TRILL nickname                 */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_trill_nickname;

  } __ATTRIBUTE_PACKED__ my_trill_nickname_reg;

  /* MAC-in-MAC-VSI:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x008a */

     /* MAC-in-MAC-VSI: VSI value for MAC-in-MAC applications.       */
     /* The PPH EEI contains an I-SID if the outgoing VSI of the     */
     /* packet is equal to this value.                               */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mac_in_mac_vsi;

  } __ATTRIBUTE_PACKED__ mac_in_mac_vsi_reg;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_ECI_REGS;
/* Block definition: OLP 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4400 */

     /* UnexpSopErr: If set, indicates that an unexpected SOP        */
     /* (start of packet) has occurred on the Dune Special           */
     /* Packet Reception unit (DSPR).                                */
     /* range: 6:6, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_sop_err;

     /* UnexpEopErr: If set, indicates that an unexpected EOP        */
     /* (end of packet) has occurred on the Dune Special Packet      */
     /* Reception unit (DSPR).                                       */
     /* range: 7:7, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_eop_err;

     /* UnexpValidBytesErr: If set, indicates that an unexpected     */
     /* valid bytes has occurred on the Dune Special Packet          */
     /* Reception unit (DSPR).                                       */
     /* range: 8:8, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_valid_bytes_err;

  } __ATTRIBUTE_PACKED__ interrupt_reg;

  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register.                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4410 */

     /* UnexpSopErrMask: Writing 0 masks the corresponding           */
     /* interrupt source                                             */
     /* range: 6:6, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_sop_err_mask;

     /* UnexpEopErrMask: Writing 0 masks the corresponding           */
     /* interrupt source                                             */
     /* range: 7:7, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_eop_err_mask;

     /* UnexpValidBytesErrMask: Writing 0 masks the                  */
     /* corresponding interrupt source                               */
     /* range: 8:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD unexp_valid_bytes_err_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;

  /* General Configuration0: DSP Engines Configuration              */
  /* Register0                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4454 */

     /* DspEthType: Ethernet Type of packets received by Dune        */
     /* Special Packet Reception unit (DSPR).                        */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dsp_eth_type;

     /* DisDspEthType: Indicates that received DSPs have an          */
     /* Ethernet type field that should be checked                   */
     /* range: 16:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dis_dsp_eth_type;

     /* BytesToSkip: Number of bytes to skip from the start of a     */
     /* DSP until the Ethernet type (if exists) or until the         */
     /* first DSP command (if Ethernet type doesn't exist).          */
     /* range: 25:20, access type: RW, default value: 0xc            */
     SOC_PB_PP_REG_FIELD bytes_to_skip;

     /* DaFwdType: The DA forwarding action type. This value is      */
     /* used for Soc_petra-A compatibility when generating a DSP         */
     /* command.                                                     */
     /* range: 30:28, access type: RW, default value: 0x1            */
     SOC_PB_PP_REG_FIELD da_fwd_type;

  } __ATTRIBUTE_PACKED__ general_configuration0_reg;

  /* General Configuration1: DSP Engines Configuration              */
  /* Register3                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4502 */

     /* SourceDevice: The device that caused generation of this      */
     /* message.                                                     */
     /* range: 10:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD source_device;

  } __ATTRIBUTE_PACKED__ general_configuration1_reg;

  /* Dsp Event Table: DSP Event Table.                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4505 */

     /* DspEventTable: DSP Event Table.                              */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dsp_fifo_learning[SOC_PB_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG];
     SOC_PB_PP_REG_FIELD dsp_fifo_shadow[SOC_PB_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG];
     SOC_PB_PP_REG_FIELD dsp_loopback[SOC_PB_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG];

  } __ATTRIBUTE_PACKED__ dsp_event_table_reg[SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS];


  /* Dsp Event Table: DSP Event Table.                              */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x4505 */

    /* DspEventTable: DSP Event Table.                              */
    /* range: 31:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD dsp_fifo;

  } __ATTRIBUTE_PACKED__ dsp_event_table_raw_reg[SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS];

  /* Dsp Engine Configuration: DSP Engines Configuration            */
  /* Register1.                                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x450b */

     /* DspGenerationEn1: If set generation of DSP is enabled.       */
     /* range: 0:0, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD dsp_generation_en;

     /* MaxCmdDelay1: Maximum command delay in clocks.               */
     /* range: 9:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD max_cmd_delay;

     /* MaxDspCmd1: Maxium number of commands in a packet. The       */
     /* limit is eight commands (4'd8).                              */
     /* range: 15:12, access type: RW, default value: 0x1            */
     SOC_PB_PP_REG_FIELD max_dsp_cmd;

     /* MinDsp1: DSP minimum size in bytes: If packet is             */
     /* smaller, 0 padding is added.                                 */
     /* range: 22:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD min_dsp;

     /* DspHeaderSize1: DSP Header size. The actual size is one      */
     /* more byte than this configuration.                           */
     /* range: 28:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dsp_header_size;

  } __ATTRIBUTE_PACKED__ dsp_engine_configuration_reg[SOC_PB_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS];

  /* Dsp Header1: DSP Header.                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x4458 */

     /* DspHeader1: DSP Header.                                      */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dsp_header1;

  } __ATTRIBUTE_PACKED__ dsp_header_reg[SOC_PB_PP_OLP_DSP_HEADER_REG_NOF_REGS][SOC_PB_PP_OLP_DSP_HEADER_REG_MULT_NOF_REGS];

} __ATTRIBUTE_PACKED__ SOC_PB_PP_OLP_REGS;
/* Block definition: IHP  */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6000 */

     /* IsemInterrupt: If set, one or more of the                    */
     /* IsemInterruptRegister bits is asserted                       */
     /* range: 0:0, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_interrupt;

     /* LargeEmInterrupt: If set, one or more of the                 */
     /* MactInterruptRegister bits is asserted                       */
     /* range: 1:1, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_interrupt;

     /* PrsrInterrupt: If set, one or more of the                    */
     /* PrsrInterruptRegister bits is asserted                       */
     /* range: 2:2, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD prsr_interrupt;

     /* VttIllegalRange: If set, MPLS range index is above           */
     /* 16383. Cleaer When VttillegalRangeLabel is read.             */
     /* range: 3:3, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD vtt_illegal_range;

  } __ATTRIBUTE_PACKED__ interrupt_reg;

  /* Isem Interrupt Register: This register contains the            */
  /* interrupt sources residing in this unit.                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6001 */

     /* IsemErrorCamTableFull: If set,                               */
     /* IsemErrorCamTableFullCounter is not equal to 0               */
     /* range: 0:0, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_cam_table_full;

     /* IsemErrorTableCoherency: If set,                             */
     /* IsemErrorTableCoherencyCounter is not equal to 0             */
     /* range: 1:1, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_table_coherency;

     /* IsemErrorDeleteUnknownKey: If set,                           */
     /* IsemErrorDeleteUnknownKeyCounter is not equal to 0           */
     /* range: 2:2, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_delete_unknown_key;

     /* IsemErrorReachedMaxEntryLimit: If set,                       */
     /* IsemErrorReachedMaxEntryLimitCounter is not equal to 0       */
     /* range: 3:3, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_reached_max_entry_limit;

     /* IsemWarningInsertedExisting: If set,                         */
     /* IsemWarningInsertedExistingCounter is not equal to 0         */
     /* range: 4:4, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_warning_inserted_existing;

     /* IsemManagementUnitFailureValid: Asserts when,                */
     /* IsemManagementUnitFailure register contains valid data       */
     /* range: 5:5, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_management_unit_failure_valid;

     /* IsemManagementCompleted: If set, Management operation is     */
     /* completed, i.e. can send new IsemManagementRequest           */
     /* range: 6:6, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_management_completed;

  } __ATTRIBUTE_PACKED__ isem_interrupt_reg;

  /* Large Em Interrupt Register: This register contains the        */
  /* interrupt sources residing in this unit.                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6002 */

     /* LargeEmErrorCamTableFull: If set,                            */
     /* MactErrorCamTableFullCounter is not equal to 0               */
     /* range: 0:0, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_cam_table_full;

     /* LargeEmErrorTableCoherency: If set,                          */
     /* MactErrorTableCoherencyCounter is not equal to 0             */
     /* range: 1:1, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_table_coherency;

     /* LargeEmErrorDeleteUnknownKey: If set,                        */
     /* MactErrorDeleteUnknownKeyCounter is not equal to 0           */
     /* range: 2:2, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_delete_unknown_key;

     /* LargeEmErrorReachedMaxEntryLimit: If set,                    */
     /* MactErrorReachedMaxEntryLimitCounter is not equal to 0       */
     /* range: 3:3, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_reached_max_entry_limit;

     /* LargeEmWarningInsertedExisting: If set,                      */
     /* MactWarningInsertedExistingCounter is not equal to 0         */
     /* range: 4:4, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_inserted_existing;

     /* LargeEmErrorLearnRequestOverStatic: If set,                  */
     /* MactErrorLearnRequestOverStaticCounter is not equal to 0     */
     /* range: 5:5, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_learn_request_over_static;

     /* LargeEmWarningLearnOverExisting: If set,                     */
     /* MactWarningLearnOverExistingCounter is not equal to 0        */
     /* range: 6:6, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_learn_over_existing;

     /* LargeEmErrorChangeFailNonExist: If set,                      */
     /* MactErrorChangeFailNonExistCounter is not equal to 0         */
     /* range: 7:7, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_fail_non_exist;

     /* LargeEmErrorChangeRequestOverStatic: If set,                 */
     /* MactErrorChangeRequestOverStaticCounter is not equal to      */
     /* 0                                                            */
     /* range: 8:8, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_request_over_static;

     /* LargeEmWarningChangeNonExistFromOther: If set,               */
     /* MactWarningChangeNonExistFromOtherCounter is not equal       */
     /* to 0                                                         */
     /* range: 9:9, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_other;

     /* LargeEmWarningChangeNonExistFromSelf: If set,                */
     /* MactWarningChangeNonExistFromSelfCounter is not equal to     */
     /* 0                                                            */
     /* range: 10:10, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_self;

     /* LargeEmManagementUnitFailureValid: If asserted,              */
     /* MactManagementUnitFailure register contains valid data       */
     /* range: 11:11, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_management_unit_failure_valid;

     /* LargeEmMngmntReqFidExceedLimit: If asserted,                 */
     /* LARGE_EM-Last-FID-exceed-limit-FID contain valid data.           */
     /* range: 12:12, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit;

     /* LargeEmMngmntReqFidExceedLimitStaticAllowed: If              */
     /* asserted, LARGE_EM-Last-FID-exceed-limit-Static-FID contain      */
     /* valid data                                                   */
     /* range: 13:13, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_static_allowed;

     /* LargeEmMngmntReqSystemVsiNotFound: If asserted,              */
     /* LARGE_EM-System-VSI-not-found contains valid data                */
     /* range: 14:14, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_system_vsi_not_found;

     /* LargeEmEventReady: If asserted, an event is ready for        */
     /* reading                                                      */
     /* range: 15:15, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_ready;

     /* LargeEmEventFifoEventDrop: If set, one of the Event-FIFO     */
     /* drop counters is not equal to 0                              */
     /* range: 16:16, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_event_drop;

     /* LargeEmEventFifoHighThresholdReached: If set, Event FIFO     */
     /* reached high threshold                                       */
     /* range: 17:17, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_high_threshold_reached;

     /* LargeEmReplyReady: Asserted when reply FIFO is not empty     */
     /* range: 18:18, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_reply_ready;

     /* LargeEmReplyFifoReplyDrop: If set, Reply-FIFO drop           */
     /* counters is not equal to 0                                   */
     /* range: 19:19, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_drop;

     /* LargeEmLookupBurstFifoDrop: If set, Lookup-Burst-FIFO        */
     /* drop counter is not equal to 0                               */
     /* range: 20:20, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_burst_fifo_drop;

     /* LargeEmAgeReachedEndIndex: If asserted, the aging            */
     /* machine has reached EndIndex                                 */
     /* range: 21:21, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_age_reached_end_index;

     /* LargeEmFluReachedEndIndex: If asserted, the flush            */
     /* machine has reached EndIndex                                 */
     /* range: 22:22, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_flu_reached_end_index;

     /* LargeEmAmsgDrop: If asserted, the AMSG FIFO dropped an       */
     /* entry                                                        */
     /* range: 23:23, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_amsg_drop;

     /* LargeEmFmsgDrop: If asserted, the FMSG FIFO dropped an       */
     /* entry                                                        */
     /* range: 24:24, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_fmsg_drop;

     /* LargeEmLookupRequestContention: If asserted, Link-Layer      */
     /* Lookup and Source/Destination Lookup were requested          */
     /* simultaneously, only the Source/Destination Lookup will      */
     /* be answered, this suggests a mis-configuration               */
     /* range: 25:25, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_request_contention;

     /* LargeEmSrcOrLlLookupOnWrongCycle: If asserted,               */
     /* Link-Layer Lookup Or Source Lookup were requested on the     */
     /* wrong cycle, these request will not be answered, this        */
     /* suggests a mis-configuration.                                */
     /* range: 26:26, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_src_or_ll_lookup_on_wrong_cycle;

     /* LargeEmFcntCounterOverflow: If asserted, one of the          */
     /* FID-Counter in the FID-DB overflowed                         */
     /* range: 27:27, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_fcnt_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_interrupt_reg;

  /* Prsr Interrupt Register: This register contains the            */
  /* interrupt sources residing in this unit.                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6003 */

     /* MaxHeaderStackExceeded: If set, packet trying to parse       */
     /* more the MaxHeaderStack headers. Cleared when                */
     /* HeaderStackExceedProgramAddress is read.                     */
     /* range: 0:0, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD max_header_stack_exceeded;

     /* IllegalAddressEncountered: If set, illegel parsing           */
     /* program address encountered. Cleared when                    */
     /* ProgramIllegelAdress is read.                                */
     /* range: 1:1, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD illegal_address_encountered;

  } __ATTRIBUTE_PACKED__ prsr_interrupt_reg;

  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6010 */

     /* IsemInterruptMask: Writing 0 masks the corresponding         */
     /* interrupt source                                             */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_interrupt_mask;

     /* MactInterruptMask: Writing 0 masks the corresponding         */
     /* interrupt source                                             */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_interrupt_mask;

     /* PrsrInterruptMask: Writing 0 masks the corresponding         */
     /* interrupt source                                             */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD prsr_interrupt_mask;

     /* VttIllegalRangeMask: Writing 0 masks the corresponding       */
     /* interrupt source                                             */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD vtt_illegal_range_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;

  /* Isem Interrupt Mask Register: Each bit in this register        */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6011 */

     /* IsemErrorCamTableFullMask: Writing 0 masks the               */
     /* corresponding interrupt source                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_cam_table_full_mask;

     /* IsemErrorTableCoherencyMask: Writing 0 masks the             */
     /* corresponding interrupt source                               */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_table_coherency_mask;

     /* IsemErrorDeleteUnknownKeyMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_delete_unknown_key_mask;

     /* IsemErrorReachedMaxEntryLimitMask: Writing 0 masks the       */
     /* corresponding interrupt source                               */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_reached_max_entry_limit_mask;

     /* IsemWarningInsertedExistingMask: Writing 0 masks the         */
     /* corresponding interrupt source                               */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_warning_inserted_existing_mask;

     /* IsemManagementUnitFailureValidMask: Writing 0 masks the      */
     /* corresponding interrupt source                               */
     /* range: 5:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_management_unit_failure_valid_mask;

     /* IsemManagementCompletedMask: Writing 0 masks the             */
     /* corresponding interrupt source                               */
     /* range: 6:6, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_management_completed_mask;

  } __ATTRIBUTE_PACKED__ isem_interrupt_mask_reg;

  /* Large Em Interrupt Mask Register: Each bit in this             */
  /* register corresponds to an interrupt source in the             */
  /* Interrupt Register. The interrupt source is masked by          */
  /* writing 0 to the relevant bit in this register                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6012 */

     /* LargeEmErrorCamTableFullMask: Writing 0 masks the            */
     /* corresponding interrupt source                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_cam_table_full_mask;

     /* LargeEmErrorTableCoherencyMask: Writing 0 masks the          */
     /* corresponding interrupt source                               */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_table_coherency_mask;

     /* LargeEmErrorDeleteUnknownKeyMask: Writing 0 masks the        */
     /* corresponding interrupt source                               */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_delete_unknown_key_mask;

     /* LargeEmErrorReachedMaxEntryLimitMask: Writing 0 masks        */
     /* the corresponding interrupt source                           */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_reached_max_entry_limit_mask;

     /* LargeEmWarningInsertedExistingMask: Writing 0 masks the      */
     /* corresponding interrupt source                               */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_inserted_existing_mask;

     /* LargeEmErrorLearnRequestOverStaticMask: Writing 0 masks      */
     /* the corresponding interrupt source                           */
     /* range: 5:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_learn_request_over_static_mask;

     /* LargeEmWarningLearnOverExistingMask: Writing 0 masks the     */
     /* corresponding interrupt source                               */
     /* range: 6:6, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_learn_over_existing_mask;

     /* LargeEmErrorChangeFailNonExistMask: Writing 0 masks the      */
     /* corresponding interrupt source                               */
     /* range: 7:7, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_fail_non_exist_mask;

     /* LargeEmErrorChangeRequestOverStaticMask: Writing 0 masks     */
     /* the corresponding interrupt source                           */
     /* range: 8:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_request_over_static_mask;

     /* LargeEmWarningChangeNonExistFromOtherMask: Writing 0         */
     /* masks the corresponding interrupt source                     */
     /* range: 9:9, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_other_mask;

     /* LargeEmWarningChangeNonExistFromSelfMask: Writing 0          */
     /* masks the corresponding interrupt source                     */
     /* range: 10:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_self_mask;

     /* LargeEmManagementUnitFailureValidMask: Writing 0 masks       */
     /* the corresponding interrupt source                           */
     /* range: 11:11, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_management_unit_failure_valid_mask;

     /* LargeEmMngmntReqFidExceedLimitMask: Writing 0 masks the      */
     /* corresponding interrupt source                               */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_mask;

     /* LargeEmMngmntReqFidExceedLimitStaticAllowedMask: Writing     */
     /* 0 masks the corresponding interrupt source                   */
     /* range: 13:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_static_allowed_mask;

     /* LargeEmMngmntReqSystemVsiNotFoundMask: Writing 0 masks       */
     /* the corresponding interrupt source                           */
     /* range: 14:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_system_vsi_not_found_mask;

     /* LargeEmEventReadyMask: Writing 0 masks the corresponding     */
     /* interrupt source                                             */
     /* range: 15:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_ready_mask;

     /* LargeEmEventFifoEventDropMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 16:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_event_drop_mask;

     /* LargeEmEventFifoHighThresholdReachedMask: Writing 0          */
     /* masks the corresponding interrupt source                     */
     /* range: 17:17, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_high_threshold_reached_mask;

     /* LargeEmReplyReadyMask: Writing 0 masks the corresponding     */
     /* interrupt source                                             */
     /* range: 18:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_reply_ready_mask;

     /* LargeEmReplyFifoReplyDropMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 19:19, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_drop_mask;

     /* LargeEmLookupBurstFifoDropMask: Writing 0 masks the          */
     /* corresponding interrupt source                               */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_burst_fifo_drop_mask;

     /* LargeEmAgeReachedEndIndexMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 21:21, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_age_reached_end_index_mask;

     /* LargeEmFluReachedEndIndexMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 22:22, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_flu_reached_end_index_mask;

     /* LargeEmAmsgDropMask: Writing 0 masks the corresponding       */
     /* interrupt source                                             */
     /* range: 23:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_amsg_drop_mask;

     /* LargeEmFmsgDropMask: Writing 0 masks the corresponding       */
     /* interrupt source                                             */
     /* range: 24:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_fmsg_drop_mask;

     /* LargeEmLookupRequestContentionMask: Writing 0 masks the      */
     /* corresponding interrupt source                               */
     /* range: 25:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_request_contention_mask;

     /* LargeEmSrcOrLlLookupOnWrongCycleMask: Writing 0 masks        */
     /* the corresponding interrupt source                           */
     /* range: 26:26, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_src_or_ll_lookup_on_wrong_cycle_mask;

     /* LargeEmFcntCounterOverflowMask: Writing 0 masks the          */
     /* corresponding interrupt source                               */
     /* range: 27:27, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_fcnt_counter_overflow_mask;

  } __ATTRIBUTE_PACKED__ large_em_interrupt_mask_reg;

  /* Prsr Interrupt Mask Register: Each bit in this register        */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6013 */

     /* MaxHeaderStackExceededMask: Writing 0 masks the              */
     /* corresponding interrupt source                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD max_header_stack_exceeded_mask;

     /* IllegalAddressEncounteredMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD illegal_address_encountered_mask;

  } __ATTRIBUTE_PACKED__ prsr_interrupt_mask_reg;

  /* Soc_petra C bugfix        */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6050 */

    /* range: 0:0, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD llr_authentication_chicken_bit;

    /* range: 1:1, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD mact_aging_high_res_chicken_bit;

    /* range: 6:6, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD soc_petra_a_compatible;

  } __ATTRIBUTE_PACKED__ soc_petra_c_bugfix_reg;

  /* Soc_petra C bugfix 2       */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6052 */

    /* range: 0:0, access type: RW, default value: 0x0              */
    SOC_PB_PP_REG_FIELD disable_llc_fix;

  } __ATTRIBUTE_PACKED__ soc_petra_c_bugfix_reg_2;

  /* Ll Mirror Vid01: Special Incoming-Vid to access                */
  /* LL-Mirror-Profile-Table                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d0 */

     /* LlMirrorVid0: If VID equal to this register then VID         */
     /* index=0                                                      */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ll_mirror_vid0;

     /* LlMirrorVid1: If VID equal to this register then VID         */
     /* index=1                                                      */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ll_mirror_vid1;

  } __ATTRIBUTE_PACKED__ ll_mirror_vid01_reg;

  /* Ll Mirror Vid23: Special Incoming-Vid to access                */
  /* LL-Mirror-Profile-Table                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d1 */

     /* LlMirrorVid2: If VID equal to this register then VID         */
     /* index=2                                                      */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ll_mirror_vid2;

     /* LlMirrorVid3: If VID equal to this register then VID         */
     /* index=3                                                      */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ll_mirror_vid3;

  } __ATTRIBUTE_PACKED__ ll_mirror_vid23_reg;

  /* Ll Mirror Vid45: Special Incoming-Vid to access                */
  /* LL-Mirror-Profile-Table                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d2 */

     /* LlMirrorVid4: If VID equal to this register then VID         */
     /* index=4                                                      */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ll_mirror_vid4;

     /* LlMirrorVid5: If VID equal to this register then VID         */
     /* index=5 If VID doesn't match none of the above then VID      */
     /* index=6 If VID doesn't exist (Or not ethernet header)        */
     /* then VID index=7                                             */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ll_mirror_vid5;

  } __ATTRIBUTE_PACKED__ ll_mirror_vid45_reg;

  /* Eth Mc Bmac Addr Prefix: Ethernet My-Bmac address prefix       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d3 */

     /* EthMcBmacAddrPrefix: Compare Ethernet-DA to                  */
     /* \{EthMcBmacAddrPrefix, I-SID\} in order to identify          */
     /* My-Bmac                                                      */
     /* range: 23:0, access type: RW, default value: 24'h011e83      */
     SOC_PB_PP_REG_FIELD eth_mc_bmac_addr_prefix;

  } __ATTRIBUTE_PACKED__ eth_mc_bmac_addr_prefix_reg;

  /* My BMac Uc Msb Config: My-B-MAC bits 47:8 configuration        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d4 */

     /* MyBMacUcMsb: My-B-MAC bits 47:8 configuration                */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_bmac_uc_msb;

  } __ATTRIBUTE_PACKED__ my_bmac_uc_msb_config_reg[SOC_PB_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS];

  /* My BMac Uc Lsb Bitmap: If bit N is set, then                   */
  /* \{My-B-Mac-Lsb,N\} is identified as My-B-Mac.                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60d6 */

     /* MyBMacUcLsbBitmap: My-B-MAC bits 7:0 configuration           */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_bmac_uc_lsb_bitmap;

  } __ATTRIBUTE_PACKED__ my_bmac_uc_lsb_bitmap_reg[SOC_PB_PP_IHP_MY_BMAC_UC_LSB_BITMAP_REG_MULT_NOF_REGS];

  /* Link Layer Lookup Cfg: Configuration for Initial Vlan          */
  /* Classification and PBP SA lookup                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60de */

     /* SaBasedVidPrefix: Prefix for the Large-EM lookup if SA       */
     /* based VID is performed                                       */
     /* range: 14:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sa_based_vid_prefix;

     /* PbpMactPrefix: Prefix for the large-EM lookup if port is     */
     /* an Egress IB-BEB                                             */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD pbp_mact_prefix;

  } __ATTRIBUTE_PACKED__ link_layer_lookup_cfg_reg;

  /* Action Profile Pbp Sa Drop Map:                                */
  /* In-PP-Port. Action-Profile-PBP-SA-Drop-Index selects one        */
  /* of the four Action-Profile specified by this register          */
  /* when PBP lookup is successful but the result indicates         */
  /* the packet should be dropped.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60df */

     /* ActionProfilePbpSaDrop0Fwd: Action-Profile if                */
     /* In-PP-Port. Action-Profile-PBP-SA-Drop-Index==0               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop0_fwd;

     /* ActionProfilePbpSaDrop0Snp: Snoop for the case above         */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop0_snp;

     /* ActionProfilePbpSaDrop1Fwd: Action-Profile if                */
     /* In-PP-Port. Action-Profile-PBP-SA-Drop-Index==1               */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop1_fwd;

     /* ActionProfilePbpSaDrop1Snp: Snoop for the case above         */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop1_snp;

     /* ActionProfilePbpSaDrop2Fwd: Action-Profile if                */
     /* In-PP-Port. Action-Profile-PBP-SA-Drop-Index==2               */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop2_fwd;

     /* ActionProfilePbpSaDrop2Snp: Snoop for the case above         */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop2_snp;

     /* ActionProfilePbpSaDrop3Fwd: Action-Profile if                */
     /* In-PP-Port. Action-Profile-PBP-SA-Drop-Index==3               */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop3_fwd;

     /* ActionProfilePbpSaDrop3Snp: Snoop for the case above         */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_sa_drop3_snp;

  } __ATTRIBUTE_PACKED__ action_profile_pbp_sa_drop_map_reg;

  /* Pbp Te Bvid Range: Range for PBP traffic engineered            */
  /* BVID.                                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e0 */

     /* PbpTeBvidRangeLow: Range is inclusive, i.e. Low <= VID       */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pbp_te_bvid_range_low;

     /* PbpTeBvidRangeHigh: Range is inclusive, i.e. VID <=High      */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD pbp_te_bvid_range_high;

  } __ATTRIBUTE_PACKED__ pbp_te_bvid_range_reg;

  /* Pbp Action Profiles: Egress IB-BEB Profile Actions             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e1 */

     /* ActionProfilePbpTeTransplantFwd: If the SA was not           */
     /* received on the expected port then the packet is snooped     */
     /* to the CPU                                                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_te_transplant_fwd;

     /* ActionProfilePbpTeTransplantSnp: Snoop for the case          */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_te_transplant_snp;

     /* ActionProfilePbpTeUnknownTunnelFwd: When the B-SA isn't      */
     /* found this trap allows notification to the CPU (usually      */
     /* using a snoop command.)                                      */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_te_unknown_tunnel_fwd;

     /* ActionProfilePbpTeUnknownTunnelSnp: Snoop for the case       */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_pbp_te_unknown_tunnel_snp;

     /* ActionProfilePbpTransplantFwd: Action if B-VID is in the     */
     /* PBP TE range and System-Port-ID from Large-EM lookup is      */
     /* different than Src-System-Port-ID                            */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_transplant_fwd;

     /* ActionProfilePbpTransplantSnp: Snoop for the case above      */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_transplant_snp;

     /* ActionProfilePbpLearnSnoopFwd: Action if B-VID is not in     */
     /* PBP TE range and Large-EM lookup failed. This allows         */
     /* snooping the packet to the CPU for learning.                 */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_learn_snoop_fwd;

     /* ActionProfilePbpLearnSnoopSnp: Snoop for the case above      */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_pbp_learn_snoop_snp;

  } __ATTRIBUTE_PACKED__ pbp_action_profiles_reg;

  /* Initial Vlan Action Profiles: These set of actions is          */
  /* used by the Initial VLAN classification process                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e2 */

     /* ActionProfileSaAuthenticationFailedFwd: Action to            */
     /* perform in case SA based VID lookup fails.                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_authentication_failed_fwd;

     /* ActionProfileSaAuthenticationFailedSnp: Snoop for the        */
     /* case above                                                   */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_authentication_failed_snp;

     /* ActionProfilePortNotPermittedFwd: Action to perform in       */
     /* case SA based VID results in an unpermitted port.            */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_port_not_permitted_fwd;

     /* ActionProfilePortNotPermittedSnp: Snoop for the case         */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_port_not_permitted_snp;

     /* ActionProfileUnexpectedVidFwd: Action to perform in case     */
     /* SA based VID results in an unexpected VID.                   */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_unexpected_vid_fwd;

     /* ActionProfileUnexpectedVidSnp: Snoop for the case above      */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_unexpected_vid_snp;

  } __ATTRIBUTE_PACKED__ initial_vlan_action_profiles_reg;

  /* Filtering Action Profiles: Filtering Ethernet illegal          */
  /* packets                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e3 */

     /* ActionProfileSaMulticastFwd: Action if a multicast SA is     */
     /* received.                                                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_multicast_fwd;

     /* ActionProfileSaMulticastSnp: Snoop for the case above        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_multicast_snp;

     /* ActionProfileSaEqualsDaFwd: Action if the SA equals the      */
     /* DA                                                           */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_equals_da_fwd;

     /* ActionProfileSaEqualsDaSnp: Snoop for the case above         */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_equals_da_snp;

     /* ActionProfile_8021xFwd: Action for 802.1x filter             */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_8021x_fwd;

     /* ActionProfile_8021xSnp: Snoop for the case above             */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_8021x_snp;

     /* Protocol_8021x: The 802.1x identified protocol code.         */
     /* This code should match the Ether-Type configuration in       */
     /* the parser.                                                  */
     /* range: 18:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD protocol_8021x;

  } __ATTRIBUTE_PACKED__ filtering_action_profiles_reg;

  /* Action Profile Acceptable Frame Types: One of four trap        */
  /* codes to apply as a function of port profile and tag           */
  /* structure                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e4 */

     /* ActionProfileAcceptableFrameType0Fwd: One of four            */
     /* actions to perform. Used to identify invalid packet          */
     /* format.                                                      */
     /* range: 5n+2:5n, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_acceptable_frame_type_fwd[SOC_PB_PP_IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_NOF_FLDS];

     /* ActionProfileAcceptableFrameType0Snp: Snoop for the case     */
     /* above                                                        */
     /* range: 5n+4:5+3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_acceptable_frame_type_snp[SOC_PB_PP_IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_NOF_FLDS];

  } __ATTRIBUTE_PACKED__ action_profile_acceptable_frame_types_reg;

  /* Pcp Decoding Table: This table is used to decodes PCP to       */
  /* Traffic-Class and Drop-Eligible                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e5 */

     /* PcpDecoding: Map \{PCP-profile(2b),                          */
     /* Incoming. UP-PCP(3b)\} to \{TC(3b), DE(1b)\}                  */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD pcp_decoding;

  } __ATTRIBUTE_PACKED__ pcp_decoding_table_reg[SOC_PB_PP_IHP_PCP_DECODING_TABLE_REG_MULT_NOF_REGS];

  /* Traffic Class L4 Range[0..2]: Traffic-Class resolution         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60e9 */

     /* TcL4RangeMin: Lower limit for layer 4 port range             */
     /* range: 15:0, access type: RW, default value: 0xffff          */
     SOC_PB_PP_REG_FIELD tc_l4_range_min;

     /* TcL4RangeMax: Upper limit for layer 4 port range             */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tc_l4_range_max;

  } __ATTRIBUTE_PACKED__ traffic_class_l4_range_reg[SOC_PB_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE];

  /* Traffic Class L4 Range Cfg: This register specifies for        */
  /* each of the layer 4 port range whether it is inclusive         */
  /* or exclusive.                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ec */

     /* TcL4RangeOutside0/1/2: Specifying if the tested value is         */
     /* inside the range or outside TrafficClassL4Range0             */
     /* range: 0:0, access type: RW, default value: 0x0              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tc_l4_range_outside[SOC_PB_PP_IHP_TRAFFIC_CLASS_L4_RANGE_CFG_REG_NOF_FLDS];

  } __ATTRIBUTE_PACKED__ traffic_class_l4_range_cfg_reg;

  /* L4 Port Ranges 2 Tc Table: Decodes the layer 4 range           */
  /* comparison results to Traffic-Class                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ed */

     /* L4PortRanges_2Tc: Maps \{Is-TCP(1b),                         */
     /* Source-Port-Range(2b), Dest-Port-Range(2b)\} to              */
     /* \{valid(1b), TC(3b)\} Is-TCP is set if the layer 4           */
     /* protocol is TCP A value of 2'b11 indicates no range          */
     /* match.                                                       */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD l4_port_ranges_2_tc;

  } __ATTRIBUTE_PACKED__ l4_port_ranges_2_tc_table_reg[SOC_PB_PP_IHP_L4_PORT_RANGES_2_TC_TABLE_REG_MULT_NOF_REGS];

  /* Incoming Up Map[0..3]: Maps the Incoming-UP-PCP(5b) to a       */
  /* User-Priority(3b)Incoming-UP-PCP=\{In-PP-Port. Incoming-UP-Map-Profile(2),  */
  /* Incoming-UP-PCP(3)\}                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60f1 */

     /* IncomingUserPriority: Each register contains 8 values of     */
     /* User-Priority                                                */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD incoming_user_priority;

  } __ATTRIBUTE_PACKED__ incoming_up_map_reg[SOC_PB_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE];

  /* Traffic Class To User Priority: Maps Traffic-Class to          */
  /* User-Priority                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60f5 */

     /* Tc_2Up: Map TC(3b) to UP(3b)                                 */
     /* range: 23:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD tc_2_up;

  } __ATTRIBUTE_PACKED__ traffic_class_to_user_priority_reg;

  /* De To Dp Map: Maps Drop-Eligibility to Drop-Precedence         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60f6 */

     /* DpWhenDeIs_0: Drop-Prcedence for the case                    */
     /* Drop-Eligibility is 0                                        */
     /* range: 1:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dp_when_de_is_0;

     /* DpWhenDeIs_1: Drop-Prcedence for the case                    */
     /* Drop-Eligibility is 1                                        */
     /* range: 3:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dp_when_de_is_1;

     /* DefaultDropPrecedence: Default-Drop-Precedence               */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD default_drop_precedence;

  } __ATTRIBUTE_PACKED__ de_to_dp_map_reg;

  /* Drop Precedence Map Pcp: Maps Incoming-UP-PCP to drop          */
  /* precedence                                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60f7 */

     /* Pcp_2Dp: Maps                                                */
     /* \{In-PP-Port. Cfg-Drop-Precedence-Profile(2),                 */
     /* Incoming-UP-PCP (3)\}                                        */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pcp_2_dp;

  } __ATTRIBUTE_PACKED__ drop_precedence_map_pcp_reg[SOC_PB_PP_IHP_DROP_PRECEDENCE_MAP_PCP_REG_MULT_NOF_REGS];

  /* Mac Layer Trap Arp: Configuration for Link Layer ARP           */
  /* trap                                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60f9 */

     /* ActionProfileMyArpFwd: Action if an ARP packet with          */
     /* MyArpIp1/2 is identified                                     */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_my_arp_fwd;

     /* ActionProfileMyArpSnp: Snoop for the case above              */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_my_arp_snp;

     /* ActionProfileArpFwd: Action if an ARP packet is              */
     /* identified                                                   */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_arp_fwd;

     /* ActionProfileArpSnp: Snoop for the case above                */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_arp_snp;

     /* ArpTrapIgnoreDa: If set then the ARP trap ignores the        */
     /* packet's DA                                                  */
     /* range: 10:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD arp_trap_ignore_da;

  } __ATTRIBUTE_PACKED__ mac_layer_trap_arp_reg;

  /* My Arp Ip1: ARP packets with this IP have a special trap       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60fa */

     /* MyArpIp1: ARP packets with this IP have a special trap       */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_arp_ip1;

  } __ATTRIBUTE_PACKED__ my_arp_ip1_reg;

  /* My Arp Ip2: ARP packets with this IP have a special trap       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60fb */

     /* MyArpIp2: ARP packets with this IP have a special trap       */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_arp_ip2;

  } __ATTRIBUTE_PACKED__ my_arp_ip2_reg;

  /* Mac Layer Trap Igmp: Configuration for IGMP trapping           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60fc */

     /* ActionProfileIgmpMembershipQueryFwd: Action if an IGMP       */
     /* packet with L3 type = 8'h11 is identified                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_igmp_membership_query_fwd;

     /* ActionProfileIgmpMembershipQuerySnp: Snoop for the case      */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_igmp_membership_query_snp;

     /* ActionProfileIgmpReportLeaveMsgFwd: Action if IGMP           */
     /* packet with L3 type = 8'h12, 8'h16, 8'h17 or 8'h22 is        */
     /* identified                                                   */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_igmp_report_leave_msg_fwd;

     /* ActionProfileIgmpReportLeaveMsgSnp: Snoop for the case       */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_igmp_report_leave_msg_snp;

     /* ActionProfileIgmpUndefinedFwd: Action if an IGMP Packet      */
     /* is identified                                                */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_igmp_undefined_fwd;

     /* ActionProfileIgmpUndefinedSnp: Snoop for the case above      */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_igmp_undefined_snp;

  } __ATTRIBUTE_PACKED__ mac_layer_trap_igmp_reg;

  /* Mac Layer Trap Icmp: Configuration for ICMPv6 trapping         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60fd */

     /* ActionProfileIcmpv6MldMcListenerQueryFwd: Action if an       */
     /* ICMPv6 listener query is identified                          */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_mc_listener_query_fwd;

     /* ActionProfileIcmpv6MldMcListenerQuerySnp: Snoop for the      */
     /* case above                                                   */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_mc_listener_query_snp;

     /* ActionProfileIcmpv6MldReportDoneMsgFwd: Action if an         */
     /* ICMPv6 report or done is identified                          */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_report_done_msg_fwd;

     /* ActionProfileIcmpv6MldReportDoneMsgSnp: Snoop for the        */
     /* case above                                                   */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_report_done_msg_snp;

     /* ActionProfileIcmpv6MldUndefinedFwd: Action if an ICMPv6      */
     /* with undefined port                                          */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_undefined_fwd;

     /* ActionProfileIcmpv6MldUndefinedSnp: Snoop for the case       */
     /* above                                                        */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_icmpv6_mld_undefined_snp;

  } __ATTRIBUTE_PACKED__ mac_layer_trap_icmp_reg;

  /* Mac Layer Trap Dhcp: Configuration for DHCP trapping           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60fe */

     /* ActionProfileDhcpServerFwd: Action if a DHCP packet sent     */
     /* to a server is identified.                                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_dhcp_server_fwd;

     /* ActionProfileDhcpServerSnp: Snoop for the case above         */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_dhcp_server_snp;

     /* ActionProfileDhcpClientFwd: Action if a DHCP packet to a     */
     /* client is identified.                                        */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_dhcp_client_fwd;

     /* ActionProfileDhcpClientSnp: Snoop for the case above         */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_dhcp_client_snp;

     /* ActionProfileDhcpv6ServerFwd: Action if a DHCPV6 packet      */
     /* to a server is identified.                                   */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_dhcpv6_server_fwd;

     /* ActionProfileDhcpv6ServerSnp: Snoop for the case above       */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_dhcpv6_server_snp;

     /* ActionProfileDhcpv6ClientFwd: Action if a DHCPV6 packet      */
     /* to a server is identified                                    */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_dhcpv6_client_fwd;

     /* ActionProfileDhcpv6ClientSnp: Snoop for the case above       */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_dhcpv6_client_snp;

  } __ATTRIBUTE_PACKED__ mac_layer_trap_dhcp_reg;

  /* General Trap[0..3]: General trap configuration. These          */
  /* are the two traps with full capability. Something is            */
  /* up with address skip. Next trap is ok.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ff */

     /* GeneralTrapDa: The DA to match for general trap              */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD general_trap_da;

  } __ATTRIBUTE_PACKED__ general_trap_reg_0[SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE];

  /* General Trap[0..3]: General trap configuration. These          */
  /* are the two traps with full capability. Something is            */
  /* up with address skip. Next trap is ok.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6100 */

     /* GeneralTrapDa: The DA to match for general trap              */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD general_trap_da;

     /* GeneralTrapDaBits: The number of In-valid bits from lsb      */
     /* in the DA to ignore when matching the DA to                  */
     /* GeneralTrapDa                                                */
     /* range: 21:16, access type: RW, default value: 0x3f           */
     SOC_PB_PP_REG_FIELD general_trap_da_bits;

     /* GeneralTrapEthernetType: The Ethernet-Type as decoded at     */
     /* the qualifier to match for general trap                      */
     /* range: 25:22, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_ethernet_type;

     /* GeneralTrapSubType: The Sub-Type to match for general        */
     /* trap                                                         */
     /* range: 31:26, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_sub_type;

  } __ATTRIBUTE_PACKED__ general_trap_reg_1[SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE];

  /* General Trap[0..3]: General trap configuration. These          */
  /* are the two traps with full capability. Something is            */
  /* up with address skip. Next trap is ok.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6101 */

     /* GeneralTrapSubType: The Sub-Type to match for general        */
     /* trap                                                         */
     /* range: 1:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD general_trap_sub_type;

     /* GeneralTrapSubTypeMask: The Sub-Type-Mask to match for       */
     /* general trap                                                 */
     /* range: 9:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD general_trap_sub_type_mask;

     /* GeneralTrapIpProtocol: The IP protocol to match for          */
     /* general trap                                                 */
     /* range: 13:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_ip_protocol;

     /* GeneralTrapL4Port: The layer 4 ports (and mask) to match     */
     /* for general trap                                             */
     /* range: 31:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_l4_port;

  } __ATTRIBUTE_PACKED__ general_trap_reg_2[SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE];

  /* General Trap[0..3]: General trap configuration. These          */
  /* are the two traps with full capability. Something is            */
  /* up with address skip. Next trap is ok.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6102 */

     /* GeneralTrapL4Port: The layer 4 ports (and mask) to match     */
     /* for general trap                                             */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD general_trap_l4_port;

     /* GeneralTrapL4PortMask: Mask for GeneralTrapL4Port            */
     /* range: 31:14, access type: RW, default value: 32'hfffff      */
     SOC_PB_PP_REG_FIELD general_trap_l4_port_mask;

  } __ATTRIBUTE_PACKED__ general_trap_reg_3[SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE];

  /* General Trap[0..3]: General trap configuration. These          */
  /* are the two traps with full capability. Something is            */
  /* up with address skip. Next trap is ok.                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6103 */

     /* GeneralTrapL4PortMask: Mask for GeneralTrapL4Port            */
     /* range: 13:0, access type: RW, default value: 32'hfffff       */
     SOC_PB_PP_REG_FIELD general_trap_l4_port_mask;

     /* GeneralTrapEnableBmp: For each general trap specifies        */
     /* which conditions are to be tested. 0: don't test Bit 0 -     */
     /* DA Bit 1 - Ethernet-Type Bit 2 - Sub-Type Bit 3 -            */
     /* IP-Protocol Bit 4 - L4 ports                                 */
     /* range: 18:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_enable_bmp;

     /* GeneralTrapInverseBmp: These bitmaps allow for testing       */
     /* the inverse conditions. Bit 0 - DA Bit 1 - Ethernet-Type     */
     /* Bit 2 - Sub-Type Bit 3 - IP-Protocol Bit 4 - L4 ports        */
     /* range: 23:19, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_inverse_bmp;

     /* GeneralTrapActionProfileFwd: The Trap-Code for each of       */
     /* the four general MAC layer traps                             */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_action_profile_fwd;

     /* GeneralTrapActionProfileSnp: Snoop for the case above        */
     /* range: 28:27, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD general_trap_action_profile_snp;

  } __ATTRIBUTE_PACKED__ general_trap_reg_4[SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE];

  /* Dbg Llr Trap0: Sticky bit for each trap in the LLR.            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6113 */

     /* DbgLlrTrap0: Rise when taken                                 */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_llr_trap0;

  } __ATTRIBUTE_PACKED__ dbg_llr_trap0_reg;

  /* Dbg Llr Trap1: Sticky bit for each trap in the LLR.            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6114 */

     /* DbgLlrTrap1: Rise when taken                                 */
     /* range: 9:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_llr_trap1;

  } __ATTRIBUTE_PACKED__ dbg_llr_trap1_reg;

  /* Large Em Flu Machine Hit Counter: Number of entries            */
  /* changed (delete or transplant) by the last operation of        */
  /* the flush machine                                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6240 */

     /* LargeEmFluMachineHitCounter: This register is clear on       */
     /* read.                                                        */
     /* range: 16:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_hit_counter;

     /* LargeEmFluMachineHitCounterOverflow:                         */
     /* range: 17:17, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_hit_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_flu_machine_hit_counter_reg;

  /* Large Em Lookup Arbiter Link Layer Lookup Counter:             */
  /* Counts Link-Layer-Lookups                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6242 */

     /* LargeEmLookupArbiterLinkLayerLookupCounter: This             */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_link_layer_lookup_counter;

     /* LargeEmLookupArbiterLinkLayerLookupCounterOverflow:          */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_link_layer_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_arbiter_link_layer_lookup_counter_reg;

  /* Large Em Lookup Arbiter Source Lookup Counter: Counts          */
  /* Source-Lookups                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6243 */

     /* LargeEmLookupArbiterSourceLookupCounter: This register       */
     /* is clear on read.                                            */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_source_lookup_counter;

     /* LargeEmLookupArbiterSourceLookupCounterOverflow:             */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_source_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_arbiter_source_lookup_counter_reg;

  /* Large Em Lookup Arbiter Destination Lookup Counter:            */
  /* Counts Destination-Lookups                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6244 */

     /* LargeEmLookupArbiterDestinationLookupCounter: This           */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_destination_lookup_counter;

     /* LargeEmLookupArbiterDestinationLookupCounterOverflow:        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_destination_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_arbiter_destination_lookup_counter_reg;

  /* Large Em Lookup Arbiter Learn Lookup Counter: Counts           */
  /* Learn-Lookups                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6245 */

     /* LargeEmLookupArbiterLearnLookupCounter: This register is     */
     /* clear on read.                                               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_learn_lookup_counter;

     /* LargeEmLookupArbiterLearnLookupCounterOverflow:              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_arbiter_learn_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_arbiter_learn_lookup_counter_reg;

  /* Large Em Learn Filter Properties: General properties of        */
  /* the learn FILTER                                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6246 */

     /* LargeEmLearnFilterDropDuplicate: If set, filter examines     */
     /* each entry before transmission; if it is a duplicate, it     */
     /* is dropped and a counter is incremented                      */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_drop_duplicate;

     /* LargeEmLearnFilterEntryTtl: Entry Time to leave in           */
     /* Filter, counted in LearnFilterResolution units               */
     /* range: 9:4, access type: RW, default value: 0x20             */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_entry_ttl;

     /* LargeEmLearnFilterRes: The resolution of the filter          */
     /* timer: 2'b00 - 32clks 2'b01 - 64clks 2'b10 - 128clks         */
     /* 2'b11 - 256clks                                              */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_res;

     /* LargeEmLearnFilterWatermark: Stores the highest entry        */
     /* count observed in the filter                                 */
     /* range: 19:16, access type: WMK, default value: 0x0           */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_watermark;

     /* LargeEmLearnFilterDropDuplicateCounter: Number of            */
     /* duplicate entries dropped. This register is clear on         */
     /* read This register is clear on read.                         */
     /* range: 30:22, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_drop_duplicate_counter;

     /* LargeEmLearnFilterDropDuplicateCounterOverflow:              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_filter_drop_duplicate_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_learn_filter_properties_reg;

  /* Large Em Ingress Learn Type:                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6247 */

     /* LargeEmIngressLearnType: Determines whether the LARGE_EM or      */
     /* the ingress PP pipe perform SA lookup. Decoded as: 1'b0      */
     /* - Pipe Learning 1'b1 - Opportunistic Learning This           */
     /* register must be configured to match register                */
     /* SA-Lookup-Type                                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_ingress_learn_type;

  } __ATTRIBUTE_PACKED__ large_em_ingress_learn_type_reg;

  /* Large Em Lookup Filter Properties: General properties of       */
  /* Lookup Filter                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6248 */

     /* LargeEmLookupFilterDropDuplicate: If set, filter             */
     /* examines each entry before transmission; if it is a          */
     /* duplicate, it is dropped and a counter is incremented        */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_drop_duplicate;

     /* LargeEmLookupFilterEntryTtl: Entry Time to leave in          */
     /* Filter, counted in LearnFilterResolution units               */
     /* range: 9:4, access type: RW, default value: 0x20             */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_entry_ttl;

     /* LargeEmLookupFilterRes: The resolution of the filter         */
     /* timer: 2'b00 - 32clks 2'b01 - 64clks 2'b10 - 128clks         */
     /* 2'b11 - 256clks                                              */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_res;

     /* LargeEmLookupFilterWatermark: Stores the highest entry       */
     /* count observed in the filter                                 */
     /* range: 19:16, access type: WMK, default value: 0x0           */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_watermark;

     /* LargeEmLookupFilterDropDuplicateCounter: Number of           */
     /* duplicate entries dropped. This register is clear on         */
     /* read This register is clear on read.                         */
     /* range: 30:24, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_drop_duplicate_counter;

     /* LargeEmLookupFilterDropDuplicateCounterOverflow:             */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_filter_drop_duplicate_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_filter_properties_reg;

  /* Large Em Lookup Burst Fifo Properties: General                 */
  /* properties of Lookup Filter                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6249 */

     /* LargeEmLookupBurstFifoWatermark: Stores the highest          */
     /* entry count observed in the fifo                             */
     /* range: 6:0, access type: WMK, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_burst_fifo_watermark;

     /* LargeEmLookupBurstFifoDropCounter: Number of entries         */
     /* dropped due to full burst FIFO This register is clear on     */
     /* read.                                                        */
     /* range: 15:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_lookup_burst_fifo_drop_counter;

     /* LargeEmLookupBurstFifoDropCounterOverflow:                   */
     /* range: 16:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_lookup_burst_fifo_drop_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_lookup_burst_fifo_properties_reg;

  /* Large Em Learn Lookup General Configuration: General           */
  /* configuration of learn lookup                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624a */

     /* LargeEmLearnLookupLargeEm: If set, LARGE_EM learn lookups        */
     /* are allowed                                                  */
     /* range: 0:0, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_mact;

     /* LargeEmLearnLookupElk: If set, ELK learn lookups are         */
     /* allowed                                                      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_elk;

     /* LargeEmLearnLookupElkBits63To58: This value will be used     */
     /* in bits 63 to 58 of the EGW data                             */
     /* range: 7:2, access type: RW, default value: 6'b010100        */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_elk_bits63_to58;

     /* LargeEmLearnLookupElkBits127To122: This value will be        */
     /* used in bits 127 to 122 of the EGW data                      */
     /* range: 13:8, access type: RW, default value: 6'b010000       */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_elk_bits127_to122;

     /* LargeEmLearnLookupAccessVsiDbOnServiceMac: If set,           */
     /* VSI-DB will be accessed when learn-key. DB is Service-MAC     */
     /* range: 14:14, access type: RW, default value: 0x1            */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_access_vsi_db_on_service_mac;

     /* LargeEmLearnLookupAccessVsiDbOnBackboneMac: If set,          */
     /* VSI-DB will be accessed when learn-key. DB is                 */
     /* BackBone-MAC                                                 */
     /* range: 15:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_access_vsi_db_on_backbone_mac;

  } __ATTRIBUTE_PACKED__ large_em_learn_lookup_general_configuration_reg;

  /* Large Em Learn Lookup Ingress Lookup Counter: General          */
  /* configuration of learn lookup                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624b */

     /* LargeEmLearnLookupIngressLookupCounter: This register is     */
     /* clear on read.                                               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_ingress_lookup_counter;

     /* LargeEmLearnLookupIngressLookupCounterOverflow:              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_ingress_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_learn_lookup_ingress_lookup_counter_reg;

  /* Large Em Learn Lookup Egress Lookup Counter: Counts            */
  /* lookups accepted from the Egress                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624c */

     /* LargeEmLearnLookupEgressLookupCounter: This register is      */
     /* clear on read.                                               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_egress_lookup_counter;

     /* LargeEmLearnLookupEgressLookupCounterOverflow:               */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_lookup_egress_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_learn_lookup_egress_lookup_counter_reg;

  /* Large Em Lookup Lookup Mode: This register determnes the       */
  /* type of events generated by the MAC table in various           */
  /* cases according to DB.                                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624d */

     /* LargeEmLookupAllowedEventsDynamic: Types of event to         */
     /* create when entry found in table is dynamic 0: don't         */
     /* create events. 1: create refresh 2: create transplant 3:     */
     /* create transplant/refresh                                    */
     /* range: 1:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_lookup_allowed_events_dynamic[SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES];

     /* LargeEmLookupAllowedEventsStatic: Types of event to          */
     /* create when entry found in table is static 0: don't          */
     /* create events. 1: create refresh 2: create transplant 3:     */
     /* create transplant/refresh                                    */
     /* range: 3:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_lookup_allowed_events_static[SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES];

     /* LargeEmLookupAllowedEventsLearn: If set, learn event can     */
     /* be created by the learn-lookup                               */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_lookup_allowed_events_learn[SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES];

  } __ATTRIBUTE_PACKED__ large_em_lookup_lookup_mode_reg;

  /* Large Em Learn Not Needed Drop Counter: Counts lookups         */
  /* dropped in learn logic (learn event was not created or         */
  /* learn event not allowed)                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624e */

     /* LargeEmLearnNotNeededDropCounter: This register is clear     */
     /* on read.                                                     */
     /* range: 14:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_learn_not_needed_drop_counter;

     /* LargeEmLearnNotNeededDropCounterOverflow:                    */
     /* range: 15:15, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_learn_not_needed_drop_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_learn_not_needed_drop_counter_reg;

  /* Large Em Mngmnt Req Configuration: General configuration       */
  /* of the management requests FIFO                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x624f */

     /* LargeEmMngmntReqRequestFifoHighThreshold: When               */
     /* Management request FIFO is below this threshold, Defrag      */
     /* machine may send defrag messages. Valid only when            */
     /* Defrag-Machine configured to work in threshold-mode          */
     /* range: 7:0, access type: RW, default value: 0xa              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_request_fifo_high_threshold;

     /* LargeEmMngmntReqAllowStaticExceed: If set, insert and        */
     /* learn commands with static payload will not fail if FID      */
     /* entry limit is reached                                       */
     /* range: 8:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_allow_static_exceed;

     /* LargeEmMngmntReqAcknowledgeOnlyFailure: If set, an ACK       */
     /* event will be generated only on failed event with valid      */
     /* stamp. Otherwise, an ACK will be generated on every          */
     /* request with valid stamp                                     */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_acknowledge_only_failure;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_configuration_reg;

  /* Large Em Mngmnt Req Counters: Counters for the                 */
  /* Management Requests                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6250 */

     /* LargeEmMngmntReqReplyDropReplyNotNeededCounter: Number       */
     /* of replies dropped due to reply not needed. This             */
     /* register is clear on read. This register is clear on         */
     /* read.                                                        */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_reply_drop_reply_not_needed_counter;

     /* LargeEmMngmntReqReplyDropReplyNotNeededCounterOverflow:      */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_reply_drop_reply_not_needed_counter_overflow;

     /* LargeEmMngmntReqFidExceedLimitCounter: Number of             */
     /* requests that failed due to FID entry imit. This             */
     /* register is clear on read. This register is clear on         */
     /* read.                                                        */
     /* range: 14:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_counter;

     /* LargeEmMngmntReqFidExceedLimitCounterOverflow:               */
     /* range: 15:15, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_counter_overflow;

     /* LargeEmMngmntReqFidExceedLimitStaticAllowedCounter:          */
     /* Number of requests with static payload that were             */
     /* accepted even though FID entry limit was exceeded This       */
     /* register is clear on read. This register is clear on         */
     /* read.                                                        */
     /* range: 22:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_static_allowed_counter;

     /*                                                              */
     /* LargeEmMngmntReqFidExceedLimitStaticAllowedCounterOverflow:  */
     /* range: 23:23, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_fid_exceed_limit_static_allowed_counter_overflow;

     /* LargeEmMngmntReqSystemVsiNotFoundCounter: Number of          */
     /* requests that failed due to failure in translating           */
     /* System-VSI to a local value. This register is clear on       */
     /* read. This register is clear on read.                        */
     /* range: 30:24, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_system_vsi_not_found_counter;

     /* LargeEmMngmntReqSystemVsiNotFoundCounterOverflow:            */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_system_vsi_not_found_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_counters_reg;

  /* Large Em Mngmnt Req Request Fifo Entry Count:                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6251 */

     /* LargeEmMngmntReqRequestFifoEntryCount: Reflects the          */
     /* current number of requests in the Request -FIFO              */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_request_fifo_entry_count;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_request_fifo_entry_count_reg;

  /* Large Em Mngmnt Req Request Counter:                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6252 */

     /* LargeEmMngmntReqRequestCounter: Number of requests           */
     /* inserted to the Request FIFO This register is clear on       */
     /* read. This register is clear on read.                        */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_request_counter;

     /* LargeEmMngmntReqRequestCounterOverflow:                      */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_request_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_request_counter_reg;

  /* Large Em Mngmnt Req Request Fifo Watermark:                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6253 */

     /* LargeEmMngmntReqRequestFifoWatermark: Stores the highest     */
     /* entry count observed in the request FIFO                     */
     /* range: 7:0, access type: WMK, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_request_fifo_watermark;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_request_fifo_watermark_reg;

  /* Large Em Mngmnt Req Exceed Limit Fid:                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6254 */

     /* LargeEmMngmntReqExceedLimitFid: The FID of the last          */
     /* request that failed due to FID entry limit. This             */
     /* register is clear on read. This register is clear on         */
     /* read.                                                        */
     /* range: 13:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_exceed_limit_fid;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_exceed_limit_fid_reg;

  /* Large Em Mngmnt Req Exceed Limit Static Allowed Fid:           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6255 */

     /* LargeEmMngmntReqExceedLimitStaticAllowedFid: The FID of      */
     /* the last request with static payload that was accepted       */
     /* even though FID entry limit was exceeded. This register      */
     /* is clear on read. This register is clear on read.            */
     /* range: 13:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_exceed_limit_static_allowed_fid;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_exceed_limit_static_allowed_fid_reg;

  /* Large Em Mngmnt Req System Vsi Not Found System Vsi:           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6256 */

     /* LargeEmMngmntReqSystemVsiNotFoundSystemVsi: The              */
     /* System-VSI of the last request that failed due to            */
     /* System-VSI not found. This register is clear on read.        */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_req_system_vsi_not_found_system_vsi;

  } __ATTRIBUTE_PACKED__ large_em_mngmnt_req_system_vsi_not_found_system_vsi_reg;

  /* Large Em Cpu Request Request: This register is used to         */
  /* insert an entry into the request FIFO                          */
  SOC_PB_PP_LEM_ACCESS_FORMAT large_em_cpu_request_request_reg;

  /* Large Em Cpu Request Trigger: When Reading this bit as         */
  /* 1'b0, CpuRequest may be written to the                         */
  /* MactCpuRequestRequest Register                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x625b */

     /* LargeEmCpuRequestTrigger: When writing 1'b1 to this bit      */
     /* the CpuRequestRequest data will be written to the            */
     /* request FIFO. Write completes when this bit is read as       */
     /* 1'b1                                                         */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_cpu_request_trigger;

  } __ATTRIBUTE_PACKED__ large_em_cpu_request_trigger_reg;

  /* Large Em Olp Request Request:                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x625c */

     /* LargeEmOlpRequestRequest: Format is identical to that of     */
     /* CpuRequestRequest                                            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_olp_request_request;

  } __ATTRIBUTE_PACKED__ large_em_olp_request_request_reg_0;

  /* Large Em Olp Request Request:                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x625d */

     /* LargeEmOlpRequestRequest: Format is identical to that of     */
     /* CpuRequestRequest                                            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_olp_request_request;

  } __ATTRIBUTE_PACKED__ large_em_olp_request_request_reg_1;

  /* Large Em Olp Request Request:                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x625e */

     /* LargeEmOlpRequestRequest: Format is identical to that of     */
     /* CpuRequestRequest                                            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_olp_request_request;

  } __ATTRIBUTE_PACKED__ large_em_olp_request_request_reg_2;

  /* Large Em Olp Request Request:                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x625f */

     /* LargeEmOlpRequestRequest: Format is identical to that of     */
     /* CpuRequestRequest                                            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_olp_request_request;

  } __ATTRIBUTE_PACKED__ large_em_olp_request_request_reg_3;

  /* Large Em Olp Request Trigger: When Reading this bit as         */
  /* 1'b0, Olp Request may be written to the                        */
  /* MactOlpRequestRequest Register                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6260 */

     /* LargeEmOlpRequestTrigger: When writing 1'b1 to this bit      */
     /* the OlpRequestRequest data will be written to the            */
     /* request FIFO. Write completes when this bit is read as       */
     /* 1'b1                                                         */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_olp_request_trigger;

  } __ATTRIBUTE_PACKED__ large_em_olp_request_trigger_reg;

  /* Large Em Age Machine Configuration: Aging configurations       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6261 */

     /* LargeEmAgeAgingEnable: If set, the aging machine is          */
     /* active                                                       */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_aging_enable;

     /* LargeEmAgeMachinePause: If set, the aging machine            */
     /* pauses. The aging process will continue from the same        */
     /* position when this bit is reset                              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_machine_pause;

     /* LargeEmAgeMachineAccessShaper: The time (in clocks)          */
     /* between accesses of the age machine to the EMC               */
     /* range: 9:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_machine_access_shaper;

     /* LargeEmAgeStamp: The stamp of events created by the          */
     /* aging machine                                                */
     /* range: 17:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_age_stamp;

  } __ATTRIBUTE_PACKED__ large_em_age_machine_configuration_reg;

  /* Large Em Age Machine Current Index:                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6262 */

     /* LargeEmAgeMachineCurrentIndex: Writing: A non-zero value     */
     /* starts the aging machine. The age machine scans the LARGE_EM     */
     /* from this value (counting down to zero). Reading:            */
     /* Returns the value of the current index the age machine       */
     /* is on. (Note: This doesn't consider AMSG indices)            */
     /* range: 16:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_age_machine_current_index;

  } __ATTRIBUTE_PACKED__ large_em_age_machine_current_index_reg;

  /* Large Em Age Machine Status:                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6263 */

     /* LargeEmAgeMachineActive: If set, the aging machine is        */
     /* not in the idle state                                        */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_machine_active;

  } __ATTRIBUTE_PACKED__ large_em_age_machine_status_reg;

  /* Large Em Age Machine Meta Cycle:                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6264 */

     /* LargeEmAgeMachineMetaCycle: The time (in 16-clock            */
     /* resolution) of the aging meta cycle                          */
     /* range: 31:0, access type: RW, default value: 32'hffffffff    */
     SOC_PB_PP_REG_FIELD large_em_age_machine_meta_cycle;

  } __ATTRIBUTE_PACKED__ large_em_age_machine_meta_cycle_reg;

  /* Large Em Age Aging Mode: Maps \{DB(2bit),Is-Dynamic\} to       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6191 */

     /* LargeEmAgeAgingCfgPtr: Pointer to one of 4 aging             */
     /* configuration tables                                         */
     /* range: 1:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_aging_cfg_ptr[SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES];

     /* LargeEmAgeOwnSystemPhysicalPorts: If set, consider local     */
     /* system-physical-ports as owned                               */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_own_system_physical_ports[SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES];

     /* LargeEmAgeOwnSystemLagPorts: If set, consider local          */
     /* system-lag-ports as owned                                    */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_own_system_lag_ports[SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES];

  } __ATTRIBUTE_PACKED__ large_em_age_aging_mode_reg;

  /* Large Em Age Aging Resolution: Maps                            */
  /* \{DB(2bit),Is-Dynamic\} to                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6268 */

     /* LargeEmAgeAgingResolution: Determines the outcome of a       */
     /* Read & Age Operation performed by Aging machine,             */
     /* \{Entry. Age-payload. Status, Entry. Age-payload. RDB\}      */
     /* behaviour: If set :                                          */
     /* 111,011->110->101->100->010->001->000->000 If cleared:       */
     /* 111,110->010->001->000->000                                  */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_age_aging_resolution;

  } __ATTRIBUTE_PACKED__ large_em_age_aging_resolution_reg;

  /* Large Em Flu Machine Configuration: Flushing                   */
  /* configurations                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x626a */

     /* LargeEmFluMachinePause: If set, the flush machine stops,     */
     /* flushing will continue from the same position when this      */
     /* bit is cleared                                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_pause;

     /* LargeEmFluMachineAccessShaper: The time (in clocks)          */
     /* between accesses of the flush machine to the EMC             */
     /* range: 8:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_access_shaper;

  } __ATTRIBUTE_PACKED__ large_em_flu_machine_configuration_reg;

  /* Large Em Flu Machine Current Index:                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x626b */

     /* LargeEmFluMachineCurrentIndex: Writing a non-zero value      */
     /* to this register activates the flush machine. The flush      */
     /* machine scans the LARGE_EM from this index down to               */
     /* MactFluMachineEndIndex. The current index the flush          */
     /* machine operates on is the value read.                       */
     /* range: 16:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_current_index;

  } __ATTRIBUTE_PACKED__ large_em_flu_machine_current_index_reg;

  /* Large Em Flu Machine Status:                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x626c */

     /* LargeEmFluMachineActive: If set, the flush machine is        */
     /* not in idle state                                            */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_active;

  } __ATTRIBUTE_PACKED__ large_em_flu_machine_status_reg;

  /* Large Em Flu Machine End Index:                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x626d */

     /* LargeEmFluMachineEndIndex: The index at which the flush      */
     /* machine stops                                                */
     /* range: 16:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_flu_machine_end_index;

  } __ATTRIBUTE_PACKED__ large_em_flu_machine_end_index_reg;

  /* Large Em Fid Db Configuration:                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x626e */

     /* LargeEmFidDbCheckFidLimit: If cleared, per FID entry         */
     /* limit is disabled.                                           */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_fid_db_check_fid_limit;

  } __ATTRIBUTE_PACKED__ large_em_fid_db_configuration_reg;

  /* LargeEmEvent: This register is used to                        */
  /* get an event from the event FIFO                              */
  SOC_PB_PP_LEM_ACCESS_FORMAT large_em_event_reg;

  /* Large Em Event Fifo Configuration: Event FIFO general          */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6273 */

     /* LargeEmEventFifoFullThreshold: Dynamic threshold for         */
     /* FIFO full report - when this value is exceeded               */
     /* Event-FIFO will not receive event from any source            */
     /* range: 6:0, access type: RW, default value: 0x7f             */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_full_threshold;

     /* LargeEmEventFifoAccessFidDb: If set, then the event FIFO     */
     /* maps an FID to a Is-Link-Layer-Fid                           */
     /* range: 7:7, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_access_fid_db;

     /* LargeEmEventFifoAccessVsiDb: If set, then the event FIFO     */
     /* maps an FID to a System-VSI                                  */
     /* range: 8:8, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_access_vsi_db;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_configuration_reg;

  /* Large Em Event Fifo High Threshold:                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6274 */

     /* LargeEmEventFifoHighThreshold: Dynamic threshold for         */
     /* FIFO High report. When this value is exceeded,               */
     /* Event-FIFO will not accept events from aging                 */
     /* machine/request-FIFO                                         */
     /* range: 6:0, access type: RW, default value: 0x7f             */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_high_threshold;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_high_threshold_reg;

  /* Large Em Event Fifo Event Drop Counters: Event FIFO            */
  /* Counters                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6275 */

     /* LargeEmEventFifoMrqEventDropCounter: Number of events        */
     /* dropped from management-request-FIFO This register is        */
     /* clear on read.                                               */
     /* range: 8:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_mrq_event_drop_counter;

     /* LargeEmEventFifoMrqEventDropCounterOverflow:                 */
     /* range: 9:9, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_mrq_event_drop_counter_overflow;

     /* LargeEmEventFifoAgeEventDropCounter: Number of events        */
     /* dropped from age-machine This register is clear on read.     */
     /* range: 18:10, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_age_event_drop_counter;

     /* LargeEmEventFifoAgeEventDropCounterOverflow:                 */
     /* range: 19:19, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_age_event_drop_counter_overflow;

     /* LargeEmEventFifoLrfEventDropCounter: Number of events        */
     /* dropped from Learn-FIFO This register is clear on read.      */
     /* range: 28:20, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_lrf_event_drop_counter;

     /* LargeEmEventFifoLrfEventDropCounterOverflow:                 */
     /* range: 29:29, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_lrf_event_drop_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_event_drop_counters_reg;

  /* Large Em Event Fifo Event Fifo Entry Count:                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6276 */

     /* LargeEmEventFifoEventFifoEntryCount: Number of events in     */
     /* the event FIFO                                               */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_event_fifo_entry_count;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_event_fifo_entry_count_reg;

  /* Large Em Event Fifo Watermark:                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6277 */

     /* LargeEmEventFifoWatermark: Stores the highest entry          */
     /* count observed in the Event FIFO                             */
     /* range: 7:0, access type: WMK, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_watermark;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_watermark_reg;

  /* Large Em Event Fifo Event Counter: Flushing                    */
  /* configurations                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6278 */

     /* LargeEmEventFifoEventCounter: Returns the number of          */
     /* events read from the event FIFO This register is clear       */
     /* on read This register is clear on read.                      */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_event_counter;

     /* LargeEmEventFifoEventCounterOverflow:                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_event_fifo_event_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_event_fifo_event_counter_reg;

  /* Mact Cpu Reply: This register is used to                        */
  /* see the answer of an entry into the request FIFO                */
  SOC_PB_PP_LEM_ACCESS_FORMAT large_em_reply_reg;

  /* Large Em Reply Fifo Configuration: CPU Reply FIFO              */
  /* general configuration                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x627d */

     /* LargeEmReplyFifoFullThreshold: Dynamic threshold for         */
     /* FIFO full report. When this value is exceeded,               */
     /* CPU-Reply-FIFO will not receive event from any source        */
     /* range: 6:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_full_threshold;

  } __ATTRIBUTE_PACKED__ large_em_reply_fifo_configuration_reg;

  /* Large Em Reply Fifo Reply Drop Counter: Reply FIFO             */
  /* Counters                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x627e */

     /* LargeEmReplyFifoReplyDropCounter: Number of entries          */
     /* (from the request FIFO) dropped This register is clear       */
     /* on read. This register is clear on read.                     */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_drop_counter;

     /* LargeEmReplyFifoReplyDropCounterOverflow:                    */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_drop_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_reply_fifo_reply_drop_counter_reg;

  /* Large Em Reply Fifo Reply Fifo Entry Count:                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x627f */

     /* LargeEmReplyFifoReplyFifoEntryCount: Number of entries       */
     /* in the reply-FIFO                                            */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_fifo_entry_count;

  } __ATTRIBUTE_PACKED__ large_em_reply_fifo_reply_fifo_entry_count_reg;

  /* Large Em Reply Fifo Watermark: CPU Reply FIFO general          */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6280 */

     /* LargeEmReplyFifoWatermark: Stores the highest entry          */
     /* count observed in the CPU Reply FIFO                         */
     /* range: 7:0, access type: WMK, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_watermark;

  } __ATTRIBUTE_PACKED__ large_em_reply_fifo_watermark_reg;

  /* Large Em Reply Fifo Reply Counter:                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6281 */

     /* LargeEmReplyFifoReplyCounter: Number of replies read         */
     /* from the Reply-FIFO This register is clear on read. This     */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_counter;

     /* LargeEmReplyFifoReplyCounterOverflow:                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_reply_fifo_reply_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_reply_fifo_reply_counter_reg;

  /* Large Em Error Learn Request Over Static Counter: Number       */
  /* of times a learn request of a dynamic entry over an            */
  /* existing static entry was attempted.                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6282 */

     /* LargeEmErrorLearnRequestOverStaticCounter: This register     */
     /* is clear on read.                                            */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_learn_request_over_static_counter;

     /* LargeEmErrorLearnRequestOverStaticCounterOverflow:           */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_learn_request_over_static_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_learn_request_over_static_counter_reg;

  /* Large Em Warning Learn Over Existing Counter: Number of        */
  /* times a learn request over an existing entry was done.         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6283 */

     /* LargeEmWarningLearnOverExistingCounter: This register is     */
     /* clear on read.                                               */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_learn_over_existing_counter;

     /* LargeEmWarningLearnOverExistingCounterOverflow:              */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_learn_over_existing_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_warning_learn_over_existing_counter_reg;

  /* Large Em Error Change Fail Non Exist Counter: Number of        */
  /* times a change-fail request of a non-existing key was          */
  /* attempted.                                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6284 */

     /* LargeEmErrorChangeFailNonExistCounter: This register is      */
     /* clear on read.                                               */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_fail_non_exist_counter;

     /* LargeEmErrorChangeFailNonExistCounterOverflow:               */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_fail_non_exist_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_change_fail_non_exist_counter_reg;

  /* Large Em Error Change Request Over Static Counter:             */
  /* Number of times a change of a dynamic entry over a             */
  /* static entry was attempted                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6285 */

     /* LargeEmErrorChangeRequestOverStaticCounter: This             */
     /* register is clear on read.                                   */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_request_over_static_counter;

     /* LargeEmErrorChangeRequestOverStaticCounterOverflow:          */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_change_request_over_static_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_change_request_over_static_counter_reg;

  /* Large Em Warning Change Non Exist From Other Counter:          */
  /* Number of times a change of a non-existing entry was           */
  /* done and the request originated from a device other than       */
  /* this                                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6286 */

     /* LargeEmWarningChangeNonExistFromOtherCounter: This           */
     /* register is clear on read.                                   */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_other_counter;

     /* LargeEmWarningChangeNonExistFromOtherCounterOverflow:        */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_other_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_warning_change_non_exist_from_other_counter_reg;

  /* Large Em Warning Change Non Exist From Self Counter:           */
  /* Number of times a change of a non existing entry was           */
  /* done and the request originated from this device               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6287 */

     /* LargeEmWarningChangeNonExistFromSelfCounter: This            */
     /* register is clear on read.                                   */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_self_counter;

     /* LargeEmWarningChangeNonExistFromSelfCounterOverflow:         */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_change_non_exist_from_self_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_warning_change_non_exist_from_self_counter_reg;

  /* Ihp Enablers: Global enablers for the IHP                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6080 */

     /* EnableDataPath: If set, IHP data path is enable.             */
     /* Otherwise IHP data path is disabled and packets are not      */
     /* accepted.                                                    */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_data_path;

     /* EnableMactBubbles: Is set, then packet rate is limited       */
     /* when the LARGE_EM management is not allowed to acess the EM      */
     /* table.                                                       */
     /* range: 1:1, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD enable_mact_bubbles;

     /* EnableTcamBubbles: If set, then packet rate is limited       */
     /* when the TCAM CPU controller is not allowed access to        */
     /* the TCAM.                                                    */
     /* range: 2:2, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD enable_tcam_bubbles;

     /* EnableIndirectBubbles: If set, then packet rate is           */
     /* limited when the CPU is not allowed indirect access.         */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_indirect_bubbles;

     /* ForceBubbles: If set, then packet rate is limited by         */
     /* SyncCounter clock cycles.                                    */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD force_bubbles;

     /* MaxHeaderStack: Set the max header stack to be parsed in     */
     /* IHP, valid values are 1-5, 0 is treated as 1.                */
     /* range: 26:24, access type: RW, default value: 0x2            */
     SOC_PB_PP_REG_FIELD max_header_stack;

     /* AcceptOnePacket: If set, IHP will accept a single packet     */
     /* from IRE, and de-assert sync. Otherwise, all packets are     */
     /* accepted. To accept another packet, user should              */
     /* de-assert and assert back this bit. Note: This is a          */
     /* debug feature, and should be used accordingly.               */
     /* range: 28:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD accept_one_packet;

  } __ATTRIBUTE_PACKED__ ihp_enablers_reg;

  /* Sync Counter:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6081 */

     /* SyncCounter: If needed, every SyncCounter number of          */
     /* clocks a bubble will be inserted to the IHP pipe, to         */
     /* allow database managements and/or CPU indirect access at     */
     /* an assured rate. Value of 0 disables the bubble              */
     /* insertion.                                                   */
     /* range: 9:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD sync_counter;

  } __ATTRIBUTE_PACKED__ sync_counter_reg;

  /* Undef Program Data:                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6082 */

     /* UndefProgramData: Used to set an undefined program data,     */
     /* to help debug the parsing program                            */
     /* range: 9:0, access type: RW, default value: 0x7f             */
     SOC_PB_PP_REG_FIELD undef_program_data;

  } __ATTRIBUTE_PACKED__ undef_program_data_reg;

  /* Pp Context Custom Macro Select: Holds a bit per PP             */
  /* context for custom macro select                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6083 */

     /* PpCtxtCstmMcroSel: Per PP context, select a set of 4         */
     /* custom macros to use.                                        */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD pp_ctxt_cstm_mcro_sel;

  } __ATTRIBUTE_PACKED__ pp_context_custom_macro_select_reg;

  /* Mpls Label Range[0..2]Low:                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6084 */

     /* MplsLabelRangeLow: The Low limit for MPLS label range.       */
     /* MPLS label is tested for MplsLabelRangeXHigh>= Label >=      */
     /* MplsLabelRangeXLow. Range 0: MPLS-Label-Range-Pipe Range     */
     /* 1: MPLS-Label-Range-Uniform Range 2:                         */
     /* MPLS-Label-Range-Sem-Result                                  */
     /* range: 19:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_range_low;

  } __ATTRIBUTE_PACKED__ mpls_label_range_low_reg[3];

  /* Mpls Label Range[0..2]High:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6087 */

     /* MplsLabelRangeHigh: The Upper limit for MPLS label           */
     /* range. MPLS label is tested for MplsLabelRangeXHigh>=        */
     /* Label >= MplsLabelRangeXLow. Range 0:                        */
     /* MPLS-Label-Range-Uniform Range 1: MPLS-Label-Range-Pipe      */
     /* Range 2: MPLS-Label-Range-Sem-Result                         */
     /* range: 19:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_range_high;

  } __ATTRIBUTE_PACKED__ mpls_label_range_high_reg[3];

  /* Gre Eth Type Ipv4:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608a */

     /* GreEthTypeIpv4: Ethernet type value for GRE.protocol is      */
     /* IPv4                                                         */
     /* range: 15:0, access type: RW, default value: 0x800           */
     SOC_PB_PP_REG_FIELD gre_eth_type_ipv4;

  } __ATTRIBUTE_PACKED__ gre_eth_type_ipv4_reg;

  /* Gre Eth Type Ipv6:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608b */

     /* GreEthTypeIpv6: Ethernet type value for GRE.protocol is      */
     /* IPv6                                                         */
     /* range: 15:0, access type: RW, default value: 0x86dd          */
     SOC_PB_PP_REG_FIELD gre_eth_type_ipv6;

  } __ATTRIBUTE_PACKED__ gre_eth_type_ipv6_reg;

  /* Gre Eth Type Mpls:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608c */

     /* GreEthTypeMpls: Ethernet type value for GRE.protocol is      */
     /* MPLS                                                         */
     /* range: 15:0, access type: RW, default value: 0x8847          */
     SOC_PB_PP_REG_FIELD gre_eth_type_mpls;

  } __ATTRIBUTE_PACKED__ gre_eth_type_mpls_reg;

  /* Gre Eth Type Custom:                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608d */

     /* GreEthTypeCustom: Ethernet type value for configruable       */
     /* GRE.protocol                                                 */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD gre_eth_type_custom;

  } __ATTRIBUTE_PACKED__ gre_eth_type_custom_reg;

  /* Program Illegel Address: Holds the illegel address read.       */
  /* Valid only when IllegelAddressEncountered is asserted.         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608e */

     /* ProgramAddress: Holds the program address. This register     */
     /* is clear on read.                                            */
     /* range: 10:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD program_address;

     /* ProgramMemIndex: If set, illegel entry read from program     */
     /* memory 2. Other wise, from program memory 1.                 */
     /* range: 12:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD program_mem_index;

  } __ATTRIBUTE_PACKED__ program_illegel_address_reg;

  /* Header Stack Exceed Program Address:                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x608f */

     /* HeaderStackExceedProgramAddress: Holds the last program      */
     /* address used before header stack exceeded. This register     */
     /* is clear on read.                                            */
     /* range: 10:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD header_stack_exceed_program_address;

  } __ATTRIBUTE_PACKED__ header_stack_exceed_program_address_reg;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6090 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_0;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6091 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_1;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6092 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_2;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6093 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_3;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6094 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_4;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6095 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_5;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6096 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_6;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6097 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_7;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6098 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_8;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6099 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_9;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609a */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_10;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609b */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_11;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609c */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_12;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609d */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_13;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609e */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_14;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x609f */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_15;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a0 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_16;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a1 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_17;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a2 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_18;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a3 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_19;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a4 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_20;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a5 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_21;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a6 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_22;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a7 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_23;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a8 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_24;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60a9 */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_25;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60aa */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_26;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ab */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_27;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ac */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_28;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ad */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_29;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ae */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_30;

  /* Last Received Header Reg: Holds the last received              */
  /* header, it's size, tm_port, and the calculated values in       */
  /* port termination. This regsiter is cleared when                */
  /* LastRcvdPort register is read.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60af */

     /* LastRcvdHdr: Holds the last received header.                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_hdr;

  } __ATTRIBUTE_PACKED__ last_received_header_reg_31;

  /* Last Generated Values: Holds the last generated values         */
  /* in port termination. This regsiter is cleared when             */
  /* LastRcvdPort is read.                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60b0 */

     /* LastPpPort:                                                  */
     /* range: 5:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD last_pp_port;

     /* LastPpContext:                                               */
     /* range: 10:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_pp_context;

     /* LastQual0:                                                   */
     /* range: 14:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD last_qual0;

     /* LastSrcSysPort:                                              */
     /* range: 28:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD last_src_sys_port;

  } __ATTRIBUTE_PACKED__ last_generated_values_reg;

  /* Last Received Port: Holds the last received header, it's       */
  /* size, tm_port, and the calculated values in port               */
  /* termination. This regsiter is cleared when LastRcvdPort        */
  /* register is read.                                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60b1 */

     /* LastRcvdTmPort: Holds the last received TM port value.       */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD last_rcvd_tm_port;

     /* LastRcvdPacketSize: Holds the packet size of the last        */
     /* received packet.                                             */
     /* range: 14:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_rcvd_packet_size;

     /* LastRcvdValid: If set, Last received registers are           */
     /* valid. This register is clear on read.                       */
     /* range: 16:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD last_rcvd_valid;

  } __ATTRIBUTE_PACKED__ last_received_port_reg;

  /* Rcvd Packet Counter: Counts the received packets from          */
  /* the IRE.                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60b2 */

     /* RcvdPacketCounter: Counts the received packets from the      */
     /* IRE. This register is clear on read.                         */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD rcvd_packet_counter;

     /* RcvdPacketCounterOverflow:                                   */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD rcvd_packet_counter_overflow;

  } __ATTRIBUTE_PACKED__ rcvd_packet_counter_reg;

  /* Isem Lookup Arbiter Counters: Management                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x62a0 */

     /* IsemLookupArbiterCounter: Counts lookups This register       */
     /* is clear on read.                                            */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_lookup_arbiter_counter;

     /* IsemLookupArbiterCounterOverflow:                            */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD isem_lookup_arbiter_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_lookup_arbiter_counters_reg;

  /* Vsd Shared Bfid:                                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6060 */

     /* VsdSharedBfid: Shared B-FID. This value is used when         */
     /* B-FID-Class is equal to 3'b111                               */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD vsd_shared_bfid;

  } __ATTRIBUTE_PACKED__ vsd_shared_bfid_reg;

  /* Vtt General Configs:                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6180 */

     /* VridMyMacMode: VRID configuration. Decoded as follows: 0     */
     /* - VRID-My-Mac is disabled 1 - Per port mode 2 - Per 4096     */
     /* VSIs mode 3 - Per 256 VSIs mode                              */
     /* range: 1:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD vrid_my_mac_mode;

     /* AlwaysDo2ndLookup: If set, VTT will always perform           */
     /* second lookup in Small-EM. Otherwise, 2nd lookup is done     */
     /* only if DA==My-Mac                                           */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD always_do2nd_lookup;

     /* TrillVsiFromOuterEth: If set, and forwarding header is       */
     /* TRILL, VSI will not be changed (use the LL-VSI).             */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD trill_vsi_from_outer_eth;

     /* CfmPpContext: PP context used for CFM trapping.              */
     /* range: 6:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD cfm_pp_context;

     /* MyBMacParserPmfProfile: If a Mac-In-Mac packet is            */
     /* terminated then the Parser-Pmf-Profile is updated to         */
     /* this value.                                                  */
     /* range: 11:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_bmac_parser_pmf_profile;

     /* Ipv4OverMplsParserPmfProfile: If MPLS label is               */
     /* terminated with an IPv4 header above it then                 */
     /* Parser-Pmf-Profile is updated to this value.                 */
     /* range: 15:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_over_mpls_parser_pmf_profile;

     /* Ipv6OverMplsParserPmfProfile: If MPLS label is               */
     /* terminated with an IPv6 header above it then                 */
     /* Parser-Pmf-Profile is updated to this value.                 */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv6_over_mpls_parser_pmf_profile;

     /* EthOverPweParserPmfProfile: If an MPLS label is              */
     /* terminated with an Ethernet header above it then             */
     /* Parser-Pmf-Profile is updated to this value.                 */
     /* range: 23:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD eth_over_pwe_parser_pmf_profile;

     /* InnerEthEncapsulation:                                       */
     /* Packet-Format-Qualifer.encapsualtion value for inner         */
     /* Ethernet header.                                             */
     /* range: 25:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD inner_eth_encapsulation;

     /* VttForwardingStrength: The strength of the VTT               */
     /* forrwading decision.                                         */
     /* range: 30:28, access type: RW, default value: 0x2            */
     SOC_PB_PP_REG_FIELD vtt_forwarding_strength;

     /* MacInMac: If set, device operates in Mac-in-Mac mode.        */
     /* This affected access to VSI configuration tables (for        */
     /* resolving I-SID).                                            */
     /* range: 31:31, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mac_in_mac;

  } __ATTRIBUTE_PACKED__ vtt_general_configs_reg;

  /* Vsi Values0: This register hold the VSI default values         */
  /* per application                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6181 */

     /* AcP2pToAcVsi: The VSI value for AC point to point to AC      */
     /* application.                                                 */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ac_p2p_to_ac_vsi;

     /* AcP2pToPweVsi: The VSI value for AC point to point to        */
     /* PWE application.                                             */
     /* range: 29:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ac_p2p_to_pwe_vsi;

  } __ATTRIBUTE_PACKED__ vsi_values0_reg;

  /* Vsi Values1: This register hold the VSI default values         */
  /* per application                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6182 */

     /* LabelPweP2pVsi: The VSI value for PWE label point to         */
     /* point.                                                       */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD label_pwe_p2p_vsi;

     /* TrillDefaultVsi: The VSI value for TRILL with untaged        */
     /* inner-ethernet.                                              */
     /* range: 29:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_default_vsi;

  } __ATTRIBUTE_PACKED__ vsi_values1_reg;

  /* Service Type Values: This register holds various service       */
  /* types.                                                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6183 */

     /* ServiceTypeLabelPweP2p: Service type for PWE label to        */
     /* point to point service.                                      */
     /* range: 2:0, access type: RW, default value: 0x7              */
     SOC_PB_PP_REG_FIELD service_type_label_pwe_p2p;

     /* ServiceTypeLabelPweMp: Service type for PWE label to         */
     /* multipoint service.                                          */
     /* range: 6:4, access type: RW, default value: 0x6              */
     SOC_PB_PP_REG_FIELD service_type_label_pwe_mp;

  } __ATTRIBUTE_PACKED__ service_type_values_reg;

  /* Asd Ac Prefix:                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6184 */

     /* AsdAcPrefix: Application specifc data AC prefix.             */
     /* range: 9:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD asd_ac_prefix;

  } __ATTRIBUTE_PACKED__ asd_ac_prefix_reg;

  /* My Mac Msb Config: My-MAC bits configuration                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6185 */

     /* MyMacMsb: My-MAC bits configuration                          */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_mac_msb;

  } __ATTRIBUTE_PACKED__ my_mac_msb_config_reg[SOC_PB_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS];

  /* All Rbridges Mac Config: MAC address for All-RBridges in       */
  /* TRILL network.                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6187 */

     /* AllRbridgesMac: MAC address for All-RBridges in TRILL        */
     /* network.                                                     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD all_rbridges_mac;

  } __ATTRIBUTE_PACKED__ all_rbridges_mac_config_reg[SOC_PB_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS];

  /* Vrid My Mac Config: My-Mac for VRID                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6189 */

     /* VridMyMac: My-Mac for VRID                                   */
     /* range: 31:0, access type: RW, default value: 48'h00005e000100 */
     SOC_PB_PP_REG_FIELD vrid_my_mac;

  } __ATTRIBUTE_PACKED__ vrid_my_mac_config_reg_0;

  /* Vrid My Mac Config: My-Mac for VRID                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618a */

     /* VridMyMac: My-Mac for VRID                                   */
     /* range: 15:0, access type: RW, default value: 48'h00005e000100 */
     SOC_PB_PP_REG_FIELD vrid_my_mac;

  } __ATTRIBUTE_PACKED__ vrid_my_mac_config_reg_1;

  /* Vtt Trap Strengths0: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618b */

     /* PortNotVlanMemberFwd: VLAN port membership failure.          */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD port_not_vlan_member_fwd;

     /* PortNotVlanMemberSnp: VLAN port membership failure.          */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD port_not_vlan_member_snp;

     /* MyBmacAndLearnNullFwd: My-BMAC but learn data is null.       */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_bmac_and_learn_null_fwd;

     /* MyBmacAndLearnNullSnp: My-BMAC but learn data is null.       */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_bmac_and_learn_null_snp;

     /* MyBmacUnknownIsidFwd: My-BMAC and known ISID.                */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_bmac_unknown_isid_fwd;

     /* MyBmacUnknownIsidSnp: My-BMAC and known ISID.                */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_bmac_unknown_isid_snp;

     /* HeaderSizeErrFwd: My-BMAC but learn data is null.            */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD header_size_err_fwd;

     /* HeaderSizeErrSnp: My-BMAC but learn data is null.            */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD header_size_err_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths0_reg;

  /* Vtt Trap Strengths1: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618c */

     /* IllegalPfcFwd: packet has an illegal Packet-Format-Code.     */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD illegal_pfc_fwd;

     /* IllegalPfcSnp: packet has an illegal Packet-Format-Code.     */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD illegal_pfc_snp;

     /* CfmAccelaratedIngressFwd: CFM accelerated ingress.           */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD cfm_accelarated_ingress_fwd;

     /* CfmAccelaratedIngressSnp: CFM accelerated ingress.           */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD cfm_accelarated_ingress_snp;

     /* StpStateBlockFwd: STP state blocked.                         */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD stp_state_block_fwd;

     /* StpStateBlockSnp: STP state blocked.                         */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD stp_state_block_snp;

     /* StpStateLearnFwd: STP state learn.                           */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD stp_state_learn_fwd;

     /* StpStateLearnSnp: STP state learn.                           */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD stp_state_learn_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths1_reg;

  /* Vtt Trap Strengths2: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618d */

     /* IpCompMcInvalidIpFwd: IP compatible MC is invalid.           */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ip_comp_mc_invalid_ip_fwd;

     /* IpCompMcInvalidIpSnp: IP compatible MC is invalid.           */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ip_comp_mc_invalid_ip_snp;

     /* MyMacAndIpDisableFwd: DA is My-Mac but IP routing is         */
     /* disabled.                                                    */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_mac_and_ip_disable_fwd;

     /* MyMacAndIpDisableSnp: DA is My-Mac but IP routing is         */
     /* disabled.                                                    */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_mac_and_ip_disable_snp;

     /* TrillVersionFwd: TRILL version error.                        */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_version_fwd;

     /* TrillVersionSnp: TRILL version error.                        */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_version_snp;

     /* TrillInvalidTtlFwd: TRILL is invalid.                        */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_invalid_ttl_fwd;

     /* TrillInvalidTtlSnp: TRILL is invalid.                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_invalid_ttl_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths2_reg;

  /* Vtt Trap Strengths3: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618e */

     /* TrillChbhFwd: TRILL header with CHbH asserted.               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD trill_chbh_fwd;

     /* TrillChbhSnp: TRILL header with CHbH asserted.               */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD trill_chbh_snp;

     /* TrillNoReverseFecFwd: TRILL when no reverse FEC.             */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD trill_no_reverse_fec_fwd;

     /* TrillNoReverseFecSnp: TRILL when no reverse FEC.             */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_no_reverse_fec_snp;

     /* TrillCiteFwd: TRILL header with CItE asserted.               */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_cite_fwd;

     /* TrillCiteSnp: TRILL header with CItE asserted.               */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_cite_snp;

     /* TrillIllegelInnerMcFwd: TRILL with inner header of MC or     */
     /* BC.                                                          */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_illegel_inner_mc_fwd;

     /* TrillIllegelInnerMcSnp: TRILL with inner header of MC or     */
     /* BC.                                                          */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_illegel_inner_mc_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths3_reg;

  /* Vtt Trap Strengths4: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x618f */

     /* MyMacAndMplsDisableFwd: DA is My-MAC but MPLS routing is     */
     /* disabled.                                                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD my_mac_and_mpls_disable_fwd;

     /* MyMacAndMplsDisableSnp: DA is My-MAC but MPLS routing is     */
     /* disabled.                                                    */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD my_mac_and_mpls_disable_snp;

     /* MyMacAndArpFwd: DA is My-MAC and ARP.                        */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD my_mac_and_arp_fwd;

     /* MyMacAndArpSnp: DA is My-MAC and ARP.                        */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_mac_and_arp_snp;

     /* MyMacAndUnknownL3Fwd: DA is My-MAC and unknown L3.           */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_mac_and_unknown_l3_fwd;

     /* MyMacAndUnknownL3Snp: DA is My-MAC and unknown L3.           */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD my_mac_and_unknown_l3_snp;

     /* MplsNoResourcesFwd: MPLS termination has no resources.       */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_no_resources_fwd;

     /* MplsNoResourcesSnp: MPLS termination has no resources.       */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_no_resources_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths4_reg;

  /* Vtt Trap Strengths5: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6190 */

     /* MplsInvalidLabelInRangeFwd: MPLS label in range, but not     */
     /* valid.                                                       */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_invalid_label_in_range_fwd;

     /* MplsInvalidLabelInRangeSnp: MPLS label in range, but not     */
     /* valid.                                                       */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_invalid_label_in_range_snp;

     /* MplsPweNoBosFwd: Label is PWE but not BOS.                   */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_pwe_no_bos_fwd;

     /* MplsPweNoBosSnp: Label is PWE but not BOS.                   */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_pwe_no_bos_snp;

     /* MplsPweNoBosLabel14Fwd: Label 14 is PWE but not BOS.         */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_pwe_no_bos_label14_fwd;

     /* MplsPweNoBosLabel14Snp: Label 14 is PWE but not BOS.         */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_pwe_no_bos_label14_snp;

     /* MplsTtl0Fwd: Label TTL is 0.                                 */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_ttl0_fwd;

     /* MplsTtl0Snp: Label TTL is 0.                                 */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_ttl0_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths5_reg;

  /* Vtt Trap Strengths6: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6191 */

     /* MplsControlWordTrapFwd: MPLS has control word and first      */
     /* nibble is '1'.                                               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_control_word_trap_fwd;

     /* MplsControlWordTrapSnp: MPLS has control word and first      */
     /* nibble is '1'.                                               */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_control_word_trap_snp;

     /* MplsControlWordDropFwd: MPLS has control word and first      */
     /* nibble is not '1' or '0'.                                    */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_control_word_drop_fwd;

     /* MplsControlWordDropSnp: MPLS has control word and first      */
     /* nibble is not '1' or '0'.                                    */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_control_word_drop_snp;

     /* MplsLspBosFwd: MPLS label is LSP and BOS.                    */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_lsp_bos_fwd;

     /* MplsLspBosSnp: MPLS label is LSP and BOS.                    */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_lsp_bos_snp;

     /* MplsVrfNoBosFwd: MPLS label is VRF and no BOS.               */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_vrf_no_bos_fwd;

     /* MplsVrfNoBosSnp: MPLS label is VRF and no BOS.               */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_vrf_no_bos_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths6_reg;

  /* Vtt Trap Strengths7: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6192 */

     /* MplsLabelValue0Fwd: Forwarding strength for MPLS label       */
     /* value 0-15 , profile 0.                                      */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_label_value0_fwd;

     /* MplsLabelValue0Snp: Forwarding strength for MPLS label       */
     /* value 0-15 , profile 0.                                      */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_label_value0_snp;

     /* MplsLabelValue1Fwd: Forwarding strength for MPLS label       */
     /* value 0-15, profile 1                                        */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_value1_fwd;

     /* MplsLabelValue1Snp: Forwarding strength for MPLS label       */
     /* value 0-15, profile 1                                        */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_value1_snp;

     /* MplsLabelValue2Fwd: Forwarding strength for MPLS label       */
     /* value 0-15, profile 2                                        */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_value2_fwd;

     /* MplsLabelValue2Snp: Forwarding strength for MPLS label       */
     /* value 0-15, profile 2                                        */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_value2_snp;

     /* MplsLabelValue3Fwd: Forwarding strength for MPLS label       */
     /* value 0-15, profile 3                                        */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_value3_fwd;

     /* MplsLabelValue3Snp: Forwarding strength for MPLS label       */
     /* value 0-15, profile 3                                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_value3_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths7_reg;

  /* Vtt Trap Strengths8: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6193 */

     /* MplsInvalidLabelInSemFwd: MPLS label in SEM, but not         */
     /* valid.                                                       */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_invalid_label_in_sem_fwd;

     /* MplsInvalidLabelInSemSnp: MPLS label in SEM, but not         */
     /* valid.                                                       */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_invalid_label_in_sem_snp;

     /* Ipv4VersionErrorFwd: Version is different than 4             */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_version_error_fwd;

     /* Ipv4VersionErrorSnp: Version is different than 4             */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_version_error_snp;

     /* Ipv4ChecksumErrorFwd: Checksum error                         */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_checksum_error_fwd;

     /* Ipv4ChecksumErrorSnp: Checksum error                         */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_checksum_error_snp;

     /* Ipv4HeaderLengthErrorFwd: IHL is less than 5                 */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_header_length_error_fwd;

     /* Ipv4HeaderLengthErrorSnp: IHL is less than 5                 */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_header_length_error_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths8_reg;

  /* Vtt Trap Strengths9: VTT action profiles                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6194 */

     /* Ipv4TotalLengthErrorFwd: Total-Length is less than 20        */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ipv4_total_length_error_fwd;

     /* Ipv4TotalLengthErrorSnp: Total-Length is less than 20        */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ipv4_total_length_error_snp;

     /* Ipv4Ttl0Fwd: TTL == 0                                        */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_ttl0_fwd;

     /* Ipv4Ttl0Snp: TTL == 0                                        */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_ttl0_snp;

     /* Ipv4HasOptionsFwd: IHL is greater than 5                     */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_has_options_fwd;

     /* Ipv4HasOptionsSnp: IHL is greater than 5                     */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_has_options_snp;

     /* Ipv4Ttl1Fwd: TTL == 1                                        */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_ttl1_fwd;

     /* Ipv4Ttl1Snp: TTL == 1                                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_ttl1_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths9_reg;

  /* Vtt Trap Strengths10: VTT action profiles                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6195 */

     /* Ipv4SipEqualDipFwd: SIP == DIP                               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ipv4_sip_equal_dip_fwd;

     /* Ipv4SipEqualDipSnp: SIP == DIP                               */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ipv4_sip_equal_dip_snp;

     /* Ipv4DipZeroFwd: DIP == 0                                     */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_dip_zero_fwd;

     /* Ipv4DipZeroSnp: DIP == 0                                     */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_dip_zero_snp;

     /* Ipv4SipIsMcFwd: SIP is multicast                             */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_sip_is_mc_fwd;

     /* Ipv4SipIsMcSnp: SIP is multicast                             */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_sip_is_mc_snp;

     /* Ipv4FragmentedFwd: packet is fragmented                      */
     /* range: 26:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_fragmented_fwd;

     /* Ipv4FragmentedSnp: packet is fragmented                      */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ipv4_fragmented_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths10_reg;

  /* Vtt Trap Strengths11: VTT action profiles                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6196 */

     /* HeaderSizeErrOverMplsFwd:                                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD header_size_err_over_mpls_fwd;

     /* HeaderSizeErrOverMplsSnp:                                    */
     /* range: 5:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD header_size_err_over_mpls_snp;

  } __ATTRIBUTE_PACKED__ vtt_trap_strengths11_reg;

  /* Mpls Label Value Over Pwe: Label value to trap over PWE,       */
  /* when PWE label is not BOS.                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6197 */

     /* MplsLabelValueOverPwe: Label value to trap over PWE,         */
     /* when PWE label is not BOS.                                   */
     /* range: 3:0, access type: RW, default value: 0xe              */
     SOC_PB_PP_REG_FIELD mpls_label_value_over_pwe;

  } __ATTRIBUTE_PACKED__ mpls_label_value_over_pwe_reg;

  /* Sem Opcode Usage:                                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x60ee */

     /* SemOpcodeUseL3:                                              */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD sem_opcode_use_l3[SOC_PB_PP_IHP_SEM_RESULT_OPCODES_NOF_FLDS];

     /* SemOpcodeUseTcDp:                                            */
     /* range: 7:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD sem_opcode_use_tc_dp[SOC_PB_PP_IHP_SEM_RESULT_OPCODES_NOF_FLDS];

  } __ATTRIBUTE_PACKED__ sem_opcode_usage_reg;

  /* Cos Profile Usage: Per Cos-Profile, configure to use L2        */
  /* or L3 parameters                                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6199 */

     /* CosProfileUseL3:                                             */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD cos_profile_use_l3;

     /* CosProfileUseL2:                                             */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD cos_profile_use_l2;

  } __ATTRIBUTE_PACKED__ cos_profile_usage_reg;

  /* Mpls Label Value Bos Action Index:                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x619a */

     /* MplsLabelValueBosActionIndex: Bits (2N+1):2N is the          */
     /* action index for Label N.                                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_value_bos_action_index;

  } __ATTRIBUTE_PACKED__ mpls_label_value_bos_action_index_reg;

  /* Mpls Label Value No Bos Action Index:                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x619b */

     /* MplsLabelValueNoBosActionIndex: Bits (2N+1):2N is the        */
     /* action index for Label N.                                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_value_no_bos_action_index;

  } __ATTRIBUTE_PACKED__ mpls_label_value_no_bos_action_index_reg;

  /* Mpls Range Configs[0..2]:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x619c */

     /* RangeInRif: In-Rif value for range.                          */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD range_in_rif;

     /* RangeInRifValid: In-Rif valid. If set, In-Rif value          */
     /* overrides previous In-RIF decision.                          */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD range_in_rif_valid;

     /* RangeCosProfile: COS-Profile for range.                      */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD range_cos_profile;

     /* RangeIsSimple: If set, range is simple.                      */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD range_is_simple;

     /* RangeIsPipe: If set, range model is pipe, Otherwise,         */
     /* model is uniform                                             */
     /* range: 24:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD range_is_pipe;

  } __ATTRIBUTE_PACKED__ mpls_range_configs_reg[3];

  /* Mpls Range Configs[0..2]:                                      */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x619c */

    /* RangeInRif: In-Rif value for range.                          */
    /* range: 11:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD mpls_label_range_base;

  } __ATTRIBUTE_PACKED__ mpls_label_range_base_reg[3];

  /* Mpls Label Value Configs[0..15]: Configurations for MPLS       */
  /* label values 0-15.                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61a2 */

     /* MplsValueInRif: In-Rif value.                                */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_value_in_rif;

     /* MplsValueInRifValid: In-Rif valid. If set, In-Rif value      */
     /* overrides previous In-RIF decision.                          */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_value_in_rif_valid;

     /* MplsValueCosProfile: COS profile value.                      */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_value_cos_profile;

     /* MplsValueModelIsPipe: If set, termination model is pipe.     */
     /* Otherwise termination model is uniform.                      */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_value_model_is_pipe;

  } __ATTRIBUTE_PACKED__ mpls_label_value_configs_reg[16];

  /* Mpls Label Has Ip:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61b2 */

     /* MplsLabelHasIp: One bit per MPLS label value 0-15. If        */
     /* set, the corresponding label has IP above it.                */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_has_ip;

  } __ATTRIBUTE_PACKED__ mpls_label_has_ip_reg;

  /* Mef L2cp Transparent Bitmap: Specifies if the DA is an MEF            */
  /* layer 2 control protocol service frame.                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61b3 */

     /* MefL2cpTransparentBitmap: acseesed with [\{MEF-L2CP-Profile,        */
     /* DA[5:0]\}                                                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mef_l2cp_transparent_bitmap;

  } __ATTRIBUTE_PACKED__ mef_l2cp_transparent_bitmap_reg[SOC_PB_PP_IHP_MEF_L2CP_TRANSPARENT_BITMAP_REG_MULT_NOF_REGS];

  /* Clear Mpls Label Encountered Bit: This register is used        */
  /* to clear a single MPLS label encountered bit from a            */
  /* table                                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61b7 */

     /* MplsLabelEncounteredIndex: The bit index in the table.       */
     /* For Label range index, valid values are 0-16383. For SEM     */
     /* label, valid values are 0-8191.                              */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_label_encountered_index;

     /* MplsLabelEncounteredIndexIsRange: If set, the index is a     */
     /* range index. Otherwise, it is SEM label index                */
     /* range: 16:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mpls_label_encountered_index_is_range;

     /* ClearMplsLabelEncounteredBitTrigger: When set, the logic     */
     /* will clear the bit. Once reset, clear is done.               */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD clear_mpls_label_encountered_bit_trigger;

  } __ATTRIBUTE_PACKED__ clear_mpls_label_encountered_bit_reg;

  /* Vtt Program Encountered: Sticky bit per proram used.           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61b8 */

     /* VttProgramFirstLookup:                                       */
     /* range: 15:0, access type: RC, default value: 0x0             */
     SOC_PB_PP_REG_FIELD vtt_program_first_lookup;

     /* VttProgramSecondLookup:                                      */
     /* range: 31:16, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD vtt_program_second_lookup;

  } __ATTRIBUTE_PACKED__ vtt_program_encountered_reg;

  /* Vtt Illegal Range Label: Illegal label range index             */
  /* status. Valid when IllegalRangeLabel interrupt is              */
  /* asserted.                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61b9 */

     /* VttIllegalRangeLabel: This register is clear on read.        */
     /* range: 19:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD vtt_illegal_range_label;

     /* VttIllegalRangeLabelRangeIndex:                              */
     /* range: 21:20, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD vtt_illegal_range_label_range_index;

  } __ATTRIBUTE_PACKED__ vtt_illegal_range_label_reg;

  /* Vtt Trap Encountered1: Sticky bit per VTT trap.                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61ba */

     /* VttTrapsEncountered1:                                        */
     /* range: 31:0, access type: RC, default value: 0x0             */
     SOC_PB_PP_REG_FIELD vtt_traps_encountered1;

  } __ATTRIBUTE_PACKED__ vtt_trap_encountered1_reg;

  /* Vtt Trap Encountered2: Sticky bit per VTT trap.                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61bb */

     /* VttTrapsEncountered2:                                        */
     /* range: 12:0, access type: RC, default value: 0x0             */
     SOC_PB_PP_REG_FIELD vtt_traps_encountered2;

  } __ATTRIBUTE_PACKED__ vtt_trap_encountered2_reg;

  /* Large Em Reset Status Register:                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6200 */

     /* LargeEmKeytResetDone: If set, then the Key table has         */
     /* finished initialization                                      */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_keyt_reset_done;

  } __ATTRIBUTE_PACKED__ large_em_reset_status_reg;

  /* Large Em Management Unit Configuration Register:               */
  /* Configuration for EM table management unit                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6206 */

     /* LargeEmMngmntUnitEnable: If set, then the management         */
     /* unit is enabled                                              */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_enable;

     /* LargeEmMngmntUnitActive: If set, then the management         */
     /* unit is currently active                                     */
     /* range: 4:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_active;

  } __ATTRIBUTE_PACKED__ large_em_management_unit_configuration_reg;

  /* Large Em Management Unit Failure: Describes the first          */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6209 */

     /* LargeEmMngmntUnitFailureValid: If set, then the there is     */
     /* a management unit failure. All other fields in this          */
     /* register are valid only when this bit is set. This           */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_failure_valid;

     /* LargeEmMngmntUnitFailureReason: Specifies reason for EMC     */
     /* management failure: 10 - Cam table full 09 - Table           */
     /* coherency 08 - Delete unknown key 07 - Reached max entry     */
     /* limit 06 - Inserted existing 05 - Learn request over         */
     /* static 04 - Learn over existing 03 - Change-fail non         */
     /* exist 02 - Change request over static 01 - Change            */
     /* non-exist from other 00 - Change non-exist from self         */
     /* range: 11:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_failure_reason;

     /* LargeEmMngmntUnitFailureKey: Holds the Key that              */
     /* operation failed on.                                         */
     /* range: 31:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ large_em_management_unit_failure_reg_0;

  /* Large Em Management Unit Failure: Describes the first          */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620a */

     /* LargeEmMngmntUnitFailureKey: Holds the Key that              */
     /* operation failed on.                                         */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ large_em_management_unit_failure_reg_1;

  /* Large Em Management Unit Failure: Describes the first          */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620b */

     /* LargeEmMngmntUnitFailureKey: Holds the Key that              */
     /* operation failed on.                                         */
     /* range: 10:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ large_em_management_unit_failure_reg_2;

  /* Large Em Diagnostics: Diagnostics register                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620c */

     /* LargeEmDiagnosticsLookup: When writing 1'b1 to this bit      */
     /* a lookup operation starts. When reading this bit as 1'b1     */
     /* a lookup operation is ongoing.                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_lookup;

     /* LargeEmDiagnosticsRead: When writing 1'b1 to this bit a      */
     /* read operation starts. When reading this bit as 1'b1 a       */
     /* read operation is ongoing.                                   */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_read;

     /* LargeEmDiagnosticsReadAge: When writing 1'b1 to this bit     */
     /* a read-and-age operation starts. When reading this bit       */
     /* as 1'b1 a read-and-age operation is ongoing                  */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_read_age;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_reg;

  /* Large Em Diagnostics Index:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620d */

     /* LargeEmDiagnosticsIndex: Address to read from. Used          */
     /* together with read and read-and-age operations               */
     /* range: 16:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_index;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_index_reg;

  /* Large Em Diagnostics Key:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620e */

     /* MactDiagnosticsKey: Key to lookup. Used together with        */
     /* lookup operation                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_key;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_key_reg[SOC_PB_PP_IHP_LARGE_EM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS];

  /* Large Em Diagnostics Key:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x620f */

     /* LargeEmDiagnosticsKey: Key to lookup. Used together with     */
     /* lookup operation                                             */
     /* range: 30:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_diagnostics_key;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_key_reg_1;

  /* Large Em Diagnostics Lookup Result: The result of the          */
  /* diagnostics operation                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6210 */

     /* LargeEmEntryFound: If set, then the entry looked up by       */
     /* the diagnostics command was found. Valid only for a          */
     /* lookup operation.                                            */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_entry_found;

     /* LargeEmEntryPayloadDestination: The payload of the entry        */
     /* range: 16:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entry_payload_dest;

     /* LargeEmEntryPayloadAsd: The payload of the entry                */
     /* range: 31:17, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entry_payload_asd;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_lookup_result_reg_0;

  /* Large Em Diagnostics Lookup Result: The result of the          */
  /* diagnostics operation                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6211 */

     /* LargeEmEntryPayloadAsd: The payload of the entry               */
     /* range: 8:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entry_payload_asd;

     /* LargeEmEntryPayloadIsDyn: The payload of the entry              */
     /* range: 9:9, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_entry_payload_is_dyn;

     /* LargeEmEntryAgeStat: The Age status of the entry. For a         */
     /* read-and-age operation the age status before the aging       */
     /* is returned                                                  */
     /* range: 13:10, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_entry_age_stat;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_lookup_result_reg_1;

  /* Large Em Diagnostics Read Result: The result of the            */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6212 */

     /* LargeEmEntryValid: If set, then the index that was read      */
     /* contains a valid entry                                       */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_entry_valid;

     /* LargeEmEntryKey: The key of the entry read                   */
     /* range: 31:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entry_key;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_read_result_reg_0;

  /* Large Em Diagnostics Read Result: The result of the            */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6213 */

     /* LargeEmEntryKey: The key of the entry read                   */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entry_key;

  } __ATTRIBUTE_PACKED__ large_em_diagnostics_read_result_reg_1;

  /* Large Em EMCDefrag Configuration Register: Configuration       */
  /* for EM table defragmentation machine.                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6214 */

     /* LargeEmDefragEnable: If set, then the defragmentation        */
     /* machine is active.                                           */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_defrag_enable;

     /* LargeEmDefragMode: The defragmentation mode. May be one      */
     /* of the following: 1'b0 - Periodical 1'b1 - Threshold         */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_defrag_mode;

     /* LargeEmDefragPeriod: The duration (in clocks) between        */
     /* defragmentation commands.                                    */
     /* range: 31:8, access type: RW, default value: 0x1000          */
     SOC_PB_PP_REG_FIELD large_em_defrag_period;

  } __ATTRIBUTE_PACKED__ large_em_emcdefrag_configuration_reg;

  /* Large Em Requests Counter: Number of Requests processed        */
  /* by the MTM. This includes Defrag Requests that have been       */
  /* generated by the Defrag Machine                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6215 */

     /* LargeEmRequestsCounter: This register is clear on read.      */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_requests_counter;

     /* LargeEmRequestsCounterOverflow:                              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD large_em_requests_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_requests_counter_reg;

  /* Large Em Error Delete Unknown Key Counter: Number of           */
  /* times a Delete request of an Unknown Key was attempted         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6217 */

     /* LargeEmErrorDeleteUnknownKeyCounter: This register is        */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_delete_unknown_key_counter;

     /* LargeEmErrorDeleteUnknownKeyCounterOverflow:                 */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_delete_unknown_key_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_delete_unknown_key_counter_reg;

  /* Large Em Error Reached Max Entry Limit Counter: Number         */
  /* of times an insertion failed because the entry limit was       */
  /* reached                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6218 */

     /* LargeEmErrorReachedMaxEntryLimitCounter: This register       */
     /* is clear on read.                                            */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_reached_max_entry_limit_counter;

     /* LargeEmErrorReachedMaxEntryLimitCounterOverflow:             */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_reached_max_entry_limit_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_reached_max_entry_limit_counter_reg;

  /* Large Em Error Cam Table Full Counter: Number of times         */
  /* an insertion failed because the CAM was full                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6219 */

     /* LargeEmErrorCamTableFullCounter: This register is clear      */
     /* on read.                                                     */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_cam_table_full_counter;

     /* LargeEmErrorCamTableFullCounterOverflow:                     */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_error_cam_table_full_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_error_cam_table_full_counter_reg;

  /* Large Em Warning Inserted Existing Counter: Number of          */
  /* times an insert request of an existing entry was done.         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x621a */

     /* LargeEmWarningInsertedExistingCounter: This register is      */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_inserted_existing_counter;

     /* LargeEmWarningInsertedExistingCounterOverflow:               */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_warning_inserted_existing_counter_overflow;

  } __ATTRIBUTE_PACKED__ large_em_warning_inserted_existing_counter_reg;

  /* Large Em Entries Counter:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x621b */

     /* LargeEmEntriesCounter: Number of entries in the EM-DB        */
     /* range: 16:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD large_em_entries_counter;

  } __ATTRIBUTE_PACKED__ large_em_entries_counter_reg;

  /* Large Em Cam Entries Counter:                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x621c */

     /* LargeEmCamEntriesCounter: Number of entries in the           */
     /* auxiliary CAM table                                          */
     /* range: 5:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD large_em_cam_entries_counter;

  } __ATTRIBUTE_PACKED__ large_em_cam_entries_counter_reg;

  /* Isem Reset Status Register:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61c0 */

     /* IsemKeytResetDone: If set, then the Key table has            */
     /* finished initialization                                      */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_keyt_reset_done;

  } __ATTRIBUTE_PACKED__ isem_reset_status_reg;

  /* Isem Management Unit Configuration Register:                   */
  /* Configuration for EM table management unit                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61c6 */

     /* IsemMngmntUnitEnable: If set, then the management unit       */
     /* is enabled                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_enable;

     /* IsemMngmntUnitActive: If set, then the management unit       */
     /* is currently active                                          */
     /* range: 4:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_active;

  } __ATTRIBUTE_PACKED__ isem_management_unit_configuration_reg;

  /* Isem Management Unit Failure: Describes the first              */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61c9 */

     /* IsemMngmntUnitFailureValid: If set, then the there is a      */
     /* management unit failure. All other fields in this            */
     /* register are valid only when this bit is set. This           */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_failure_valid;

     /* IsemMngmntUnitFailureReason: Specifies reason for EMC        */
     /* management failure: 10 - Cam table full 09 - Table           */
     /* coherency 08 - Delete unknown key 07 - Reached max entry     */
     /* limit 06 - Inserted existing 05 - Learn request over         */
     /* static 04 - Learn over existing 03 - Change-fail non         */
     /* exist 02 - Change request over static 01 - Change            */
     /* non-exist from other 00 - Change non-exist from self         */
     /* range: 11:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_failure_reason;

     /* IsemMngmntUnitFailureKey: Holds the Key that operation       */
     /* failed on.                                                   */
     /* range: 31:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ isem_management_unit_failure_reg_0;

  /* Isem Management Unit Failure: Describes the first              */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61ca */

     /* IsemMngmntUnitFailureKey: Holds the Key that operation       */
     /* failed on.                                                   */
     /* range: 13:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ isem_management_unit_failure_reg_1;

  /* Isem Diagnostics: Diagnostics register                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61cb */

     /* IsemDiagnosticsLookup: When writing 1'b1 to this bit a       */
     /* lookup operation starts. When reading this bit as 1'b1 a     */
     /* lookup operation is ongoing.                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_diagnostics_lookup;

     /* IsemDiagnosticsRead: When writing 1'b1 to this bit a         */
     /* read operation starts. When reading this bit as 1'b1 a       */
     /* read operation is ongoing.                                   */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_diagnostics_read;

     /* IsemDiagnosticsReadAge: When writing 1'b1 to this bit a      */
     /* read-and-age operation starts. When reading this bit as      */
     /* 1'b1 a read-and-age operation is ongoing                     */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_diagnostics_read_age;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_reg;

  /* Isem Diagnostics Index:                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61cc */

     /* IsemDiagnosticsIndex: Address to read from. Used             */
     /* together with read and read-and-age operations               */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_diagnostics_index;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_index_reg;

  /* Isem Diagnostics Key:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61cd */

     /* IsemDiagnosticsKey: Key to lookup. Used together with        */
     /* lookup operation                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_diagnostics_key;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_key_reg[SOC_PB_PP_IHP_ISEM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS];

  /* Isem Diagnostics Lookup Result: The result of the              */
  /* diagnostics operation                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61cf */

     /* IsemEntryFound: If set, then the entry looked up by the      */
     /* diagnostics command was found. Valid only for a lookup       */
     /* operation.                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_entry_found;

     /* IsemEntryPayload: The payload of the entry                   */
     /* range: 16:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_entry_payload;

     /* IsemEntryAgeStat: The Age status of the entry. For a         */
     /* read-and-age operation the age status before the aging       */
     /* is returned                                                  */
     /* range: 20:17, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD isem_entry_age_stat;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_lookup_result_reg;

  /* Isem Diagnostics Read Result: The result of the                */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d0 */

     /* IsemEntryValid: If set, then the index that was read         */
     /* contains a valid entry                                       */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_entry_valid;

     /* IsemEntryKey: The key of the entry read                      */
     /* range: 31:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_entry_key;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_read_result_reg_0;

  /* Isem Diagnostics Read Result: The result of the                */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d1 */

     /* IsemEntryKey: The key of the entry read                      */
     /* range: 2:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_entry_key;

  } __ATTRIBUTE_PACKED__ isem_diagnostics_read_result_reg_1;

  /* Isem EMCDefrag Configuration Register: Configuration for       */
  /* EM table defragmentation machine.                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d2 */

     /* IsemDefragEnable: If set, then the defragmentation           */
     /* machine is active.                                           */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_defrag_enable;

     /* IsemDefragMode: The defragmentation mode. May be one of      */
     /* the following: 1'b0 - Periodical 1'b1 - Threshold            */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_defrag_mode;

     /* IsemDefragPeriod: The duration (in clocks) between           */
     /* defragmentation commands.                                    */
     /* range: 31:8, access type: RW, default value: 0x1000          */
     SOC_PB_PP_REG_FIELD isem_defrag_period;

  } __ATTRIBUTE_PACKED__ isem_emcdefrag_configuration_reg;

  /* Isem Requests Counter: Number of Requests processed by         */
  /* the MTM. This includes Defrag Requests that have been          */
  /* generated by the Defrag Machine                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d3 */

     /* IsemRequestsCounter: This register is clear on read.         */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_requests_counter;

     /* IsemRequestsCounterOverflow:                                 */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD isem_requests_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_requests_counter_reg;

  /* Isem Error Delete Unknown Key Counter: Number of times a       */
  /* Delete request of an Unknown Key was attempted                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d5 */

     /* IsemErrorDeleteUnknownKeyCounter: This register is clear     */
     /* on read.                                                     */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_delete_unknown_key_counter;

     /* IsemErrorDeleteUnknownKeyCounterOverflow:                    */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_delete_unknown_key_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_error_delete_unknown_key_counter_reg;

  /* Isem Error Reached Max Entry Limit Counter: Number of          */
  /* times an insertion failed because the entry limit was          */
  /* reached                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d6 */

     /* IsemErrorReachedMaxEntryLimitCounter: This register is       */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_reached_max_entry_limit_counter;

     /* IsemErrorReachedMaxEntryLimitCounterOverflow:                */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_reached_max_entry_limit_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_error_reached_max_entry_limit_counter_reg;

  /* Isem Error Cam Table Full Counter: Number of times an          */
  /* insertion failed because the CAM was full                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d7 */

     /* IsemErrorCamTableFullCounter: This register is clear on      */
     /* read.                                                        */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_cam_table_full_counter;

     /* IsemErrorCamTableFullCounterOverflow:                        */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_error_cam_table_full_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_error_cam_table_full_counter_reg;

  /* Isem Warning Inserted Existing Counter: Number of times        */
  /* an insert request of an existing entry was done.               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d8 */

     /* IsemWarningInsertedExistingCounter: This register is         */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_warning_inserted_existing_counter;

     /* IsemWarningInsertedExistingCounterOverflow:                  */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_warning_inserted_existing_counter_overflow;

  } __ATTRIBUTE_PACKED__ isem_warning_inserted_existing_counter_reg;

  /* Isem Entries Counter:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61d9 */

     /* IsemEntriesCounter: Number of entries in the EM-DB           */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD isem_entries_counter;

  } __ATTRIBUTE_PACKED__ isem_entries_counter_reg;

  /* Isem Cam Entries Counter:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x61da */

     /* IsemCamEntriesCounter: Number of entries in the              */
     /* auxiliary CAM table                                          */
     /* range: 5:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD isem_cam_entries_counter;

  } __ATTRIBUTE_PACKED__ isem_cam_entries_counter_reg;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHP_REGS;
/* Block definition: IHB  */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6400 */

     /* BankA_requestCollisionInt: More than one block (IPMF,        */
     /* EGQ and FL) sends a lookup request to TcamA                  */
     /* range: 0:0, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_a_request_collision_int;

     /* BankB_requestCollisionInt: More than one block (IPMF,        */
     /* EGQ and FL) sends a lookup request to TcamB                  */
     /* range: 1:1, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_b_request_collision_int;

     /* BankC_requestCollisionInt: More than one block (IPMF,        */
     /* EGQ and FL) sends a lookup request to TcamC                  */
     /* range: 2:2, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_c_request_collision_int;

     /* BankD_requestCollisionInt: More than one block (IPMF,        */
     /* EGQ and FL) sends a lookup request to TcamD                  */
     /* range: 3:3, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_d_request_collision_int;

     /* LemRpfNotFecPtrInt: RPF-Fec pointer returned from the        */
     /* LEM is not a fec pointer                                     */
     /* range: 4:4, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD lem_rpf_not_fec_ptr_int;

     /* NifPhysicalErr: Error found on RX NIF - sob, eob, err or     */
     /* be. Anyway machine sync according sob.                       */
     /* range: 5:5, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD nif_physical_err;

     /* SeqRxBigerSeqExpAndSmallerSeqTx: Received reply from ELK     */
     /* with sequence between expected to transmitted (which         */
     /* means replies were lost)                                     */
     /* range: 6:6, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD seq_rx_biger_seq_exp_and_smaller_seq_tx;

     /* SeqRxSmallerSeqExpOrBiggerEqSeqTx: Received reply from       */
     /* ELK with sequence between expected to transmitted (which     */
     /* means replies were born)                                     */
     /* range: 7:7, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD seq_rx_smaller_seq_exp_or_bigger_eq_seq_tx;

     /* FlpFifoFull: FLP request FIFO crossed threshold              */
     /* range: 8:8, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD flp_fifo_full;

     /* FlpLookupTimeout: Reply timed out                            */
     /* range: 9:9, access type: RC, default value: 0x0              */
     SOC_PB_PP_REG_FIELD flp_lookup_timeout;

     /* NifRxFifoOvf: Reply Fifo overflowed                          */
     /* range: 10:10, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD nif_rx_fifo_ovf;

     /* InvalidAddressInt: Invalid address accssesed by LPM          */
     /* range: 11:11, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD invalid_address_int;

     /* LbVectorIndexIllegalCalcInt: Use of Calc processing          */
     /* profile is illegal for the packet-format-code                */
     /* range: 12:12, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD lb_vector_index_illegal_calc_int;

     /* FecEntryAccessedInt: Requested Fec-Entry accessed            */
     /* range: 13:13, access type: RC, default value: 0x0            */
     SOC_PB_PP_REG_FIELD fec_entry_accessed_int;

  } __ATTRIBUTE_PACKED__ interrupt_reg;

  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register.                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6410 */

     /* BankA_requestCollisionIntMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_a_request_collision_int_mask;

     /* BankB_requestCollisionIntMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_b_request_collision_int_mask;

     /* BankC_requestCollisionIntMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_c_request_collision_int_mask;

     /* BankD_requestCollisionIntMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bank_d_request_collision_int_mask;

     /* LemRpfNotFecPtrIntMask: Writing 0 masks the                  */
     /* corresponding interrupt source                               */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD lem_rpf_not_fec_ptr_int_mask;

     /* NifPhysicalErrMask: Writing 0 masks the corresponding        */
     /* interrupt source                                             */
     /* range: 5:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD nif_physical_err_mask;

     /* SeqRxBigerSeqExpAndSmallerSeqTxMask: Writing 0 masks the     */
     /* corresponding interrupt source                               */
     /* range: 6:6, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD seq_rx_biger_seq_exp_and_smaller_seq_tx_mask;

     /* SeqRxSmallerSeqExpOrBiggerEqSeqTxMask: Writing 0 masks       */
     /* the corresponding interrupt source                           */
     /* range: 7:7, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD seq_rx_smaller_seq_exp_or_bigger_eq_seq_tx_mask;

     /* FlpFifoFullMask: Writing 0 masks the corresponding           */
     /* interrupt source                                             */
     /* range: 8:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD flp_fifo_full_mask;

     /* FlpLookupTimeoutMask: Writing 0 masks the corresponding      */
     /* interrupt source                                             */
     /* range: 9:9, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD flp_lookup_timeout_mask;

     /* NifRxFifoOvfMask: Writing 0 masks the corresponding          */
     /* interrupt source                                             */
     /* range: 10:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD nif_rx_fifo_ovf_mask;

     /* InvalidAddressIntMask: Writing 0 masks the corresponding     */
     /* interrupt source                                             */
     /* range: 11:11, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD invalid_address_int_mask;

     /* LbVectorIndexIllegalCalcIntMask: Writing 0 masks the         */
     /* corresponding interrupt source                               */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD lb_vector_index_illegal_calc_int_mask;

     /* FecEntryAccessedIntMask: Writing 0 masks the                 */
     /* corresponding interrupt source                               */
     /* range: 13:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD fec_entry_accessed_int_mask;

     /* IllegalBytesToRemoveValueMask: Writing 0 masks the           */
     /* corresponding interrupt source                               */
     /* range: 14:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD illegal_bytes_to_remove_value_mask;

     /* DataCenterApp: If set then for IP packets                    */
     /* outlif=\{EEP-TYPE, FEC-ENTRY. OutLifType,                     */
     /* FEC-ENTRY. OutLifLsb\}                                        */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD data_center_app;

     /* EnableOutLif_1: Outlif number 1 is in-valid - FER will       */
     /* take outlif from PMF If EnableOutLif_1is set then this       */
     /* feature is disabled                                          */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_out_lif_1;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;

  /* Sequence Tx:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6610 */

     /* SequenceTx: TX - next transmitted sequence number            */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sequence_tx;

  } __ATTRIBUTE_PACKED__ sequence_tx_reg;

  /* Sequence Tx:                                                   */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6452 - In Soc_petra-B B0, the address is 0x656c */

    /* DataCenterApp: If set then for IP packets outlif={EEP-TYPE, FEC-ENTRY. OutLifType, */
    /* FEC-ENTRY. OutLifLsb}            */
    /* range: 0:0, access type: RW, default value: 0x0             */
    /* range: 1:1, In Soc_petra-B B0            */
    SOC_PB_PP_REG_FIELD dc_app;

    /* enable_outlif_1: Outlif number 1 is in-valid - FER will take outlif from PMF */
    /* If EnableOutLif_1is set then this feature is disabled  */
    /* range: 1:1, access type: RW, default value: 0x0             */
    /* range: 0:0, In Soc_petra-B B0            */
    SOC_PB_PP_REG_FIELD enable_outlif_1;

  } __ATTRIBUTE_PACKED__ dc_app_reg;

  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* In Soc_petra-B B0, the address is 0x656c */

    /* range: 1:1, In Soc_petra-B B0            */
    SOC_PB_PP_REG_FIELD dc_app;

    /* range: 0:0, In Soc_petra-B B0            */
    SOC_PB_PP_REG_FIELD enable_outlif_1;

  } __ATTRIBUTE_PACKED__ dc_app_reg_b0;

  /* Sequence Expected:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6611 */

     /* SequenceExpected: RX - expected sequence number              */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sequence_expected;

  } __ATTRIBUTE_PACKED__ sequence_expected_reg;

  /* Last Error Sequence Rx:                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6612 */

     /* LastErrorSequenceRx: RX - last receieved sequence number     */
     /* (locked on error) This register is clear on read.            */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD last_error_sequence_rx;

  } __ATTRIBUTE_PACKED__ last_error_sequence_rx_reg;

  /* Good Replies Counter:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6613 */

     /* GoodRepliesCounter: Counts good replies arriving from        */
     /* ELK                                                          */
     /* range: 31:0, access type: CNT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD good_replies_counter;

  } __ATTRIBUTE_PACKED__ good_replies_counter_reg;

  /* Bad Replies Counter:                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6614 */

     /* BadRepliesCounter: Counts bad replies arriving from ELK      */
     /* range: 31:0, access type: CNT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD bad_replies_counter;

  } __ATTRIBUTE_PACKED__ bad_replies_counter_reg;

  /* Lookups Counter:                                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6615 */

     /* LookupsCounter: Counts lookups transmitted to ELK            */
     /* range: 31:0, access type: CNT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD lookups_counter;

  } __ATTRIBUTE_PACKED__ lookups_counter_reg;

  /* Learns Counter:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6616 */

     /* LearnsCounter: Counts lerns transmitted to ELK               */
     /* range: 31:0, access type: CNT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD learns_counter;

  } __ATTRIBUTE_PACKED__ learns_counter_reg;

  /* Nif Tx Init Credits:                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6617 */

     /* NifTxInitCredits: Initial credits for transmitter            */
     /* interface to nif (must be > 0)                               */
     /* range: 4:0, access type: RW, default value: 0x10             */
     SOC_PB_PP_REG_FIELD nif_tx_init_credits;

  } __ATTRIBUTE_PACKED__ nif_tx_init_credits_reg;

  /* Spr Dly Fraction:                                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6618 */

     /* SprDlyFraction: Tunning of shaper in 1/256 cycle units       */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD spr_dly_fraction;

  } __ATTRIBUTE_PACKED__ spr_dly_fraction_reg;

  /* Spr Dly:                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6619 */

     /* SprDly: Shaper number of cycles between two egw packets      */
     /* in transmitter                                               */
     /* range: 4:0, access type: RW, default value: 0xa              */
     SOC_PB_PP_REG_FIELD spr_dly;

  } __ATTRIBUTE_PACKED__ spr_dly_reg;

  /* Tx Channel Num:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661a */

     /* TxChannelNum: Channel number for transmitter to stamp        */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tx_channel_num;

  } __ATTRIBUTE_PACKED__ tx_channel_num_reg;

  /* Lookup Control:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661b */

     /* EnableElkLookup: Enables lookups to ELK                      */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_elk_lookup;

     /* EnableFlpReply: Enables replys to FLP                        */
     /* range: 1:1, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD enable_flp_reply;

     /* CpuLookupTrigger: User asserts to perform lookup             */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD cpu_lookup_trigger;

  } __ATTRIBUTE_PACKED__ lookup_control_reg;

  /* Cpu Lookup Key:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661c */

     /* CpuLookupKey: lookup key in ELK                              */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD cpu_lookup_key;

  } __ATTRIBUTE_PACKED__ cpu_lookup_key_reg_0;

  /* Cpu Lookup Key:                                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661d */

     /* CpuLookupKey: lookup key in ELK                              */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD cpu_lookup_key;

  } __ATTRIBUTE_PACKED__ cpu_lookup_key_reg_1;

  /* Lookup Reply:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661e */

     /* LookupReplyValid: reply is ready (FLP or CPU) This           */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD lookup_reply_valid;

     /* LookupReplyError: reply returned error                       */
     /* range: 1:1, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD lookup_reply_error;

  } __ATTRIBUTE_PACKED__ lookup_reply_reg;

  /* Lookup Reply Data:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x661f */

     /* LookupReplyData: Last Reply from ELK (cleared when           */
     /* reading LookupReply, so must read it before reading          */
     /* LookupReply)                                                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD lookup_reply_data;

  } __ATTRIBUTE_PACKED__ lookup_reply_data_reg_0;

  /* Lookup Reply Data:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6620 */

     /* LookupReplyData: Last Reply from ELK (cleared when           */
     /* reading LookupReply, so must read it before reading          */
     /* LookupReply)                                                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD lookup_reply_data;

  } __ATTRIBUTE_PACKED__ lookup_reply_data_reg_1;

  /* Lookup Reply Seq Num:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6621 */

     /* LookupReplySeqNum: Last Reply from ELK (cleared when         */
     /* reading LookupReply, so must read it before reading          */
     /* LookupReply)                                                 */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD lookup_reply_seq_num;

  } __ATTRIBUTE_PACKED__ lookup_reply_seq_num_reg;

  /* Nif Physical Err Counter:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6622 */

     /* NifPhysicalErrCounter: Counts Nif errors                     */
     /* range: 31:0, access type: CNT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD nif_physical_err_counter;

  } __ATTRIBUTE_PACKED__ nif_physical_err_counter_reg;

  /* Soc_petra C:                                      */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6623 */

    /* range: 0:0,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_mode;

    /* range: 1:1,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_lkp_record_size;

    /* range: 2:2,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_lkp_select;

    /* range: 3:3                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_lrn_record_size;

    /* range: 4:4                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_nop_record_size;

    /* range: 9:5,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_burst_size;

    /* range: 10:10,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_lkp_packing;

    /* range: 11:11,                                                 */
    SOC_PB_PP_REG_FIELD soc_petra_c_nif_credits;

  } __ATTRIBUTE_PACKED__ soc_petra_c_reg;

  /* cpu_lookup_key_msb_reg         */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6624-0x6626 */

    /* range: 31:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD cpu_lookup_key_msb;

  } __ATTRIBUTE_PACKED__ cpu_lookup_key_msb_reg[SOC_PB_PP_IHB_CPU_LOOKUP_KEY_MSB_REG_REG_MULT_NOF_REGS];



  /* Ecmp Lb Key Cfg: LB-Key Configuration                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6561 */

     /* EcmpLbKeySeed: Initial value for the LB key generation       */
     /* range: 15:0, access type: RW, default value: 0x1             */
     SOC_PB_PP_REG_FIELD ecmp_lb_key_seed;

     /* EcmpLbHashIndex: Selects one of eight options for the LB     */
     /* key generation: 4'd0 - Use polynomial 0x10491 4'd1 - Use     */
     /* polynomial 0x17111 4'd2 - Use polynomial 0x19715 4'd3 -      */
     /* Use polynomial 0x155F5 4'd4 - Use polynomial 0x13965         */
     /* 4'd5 - Use polynomial 0x1698d 4'd6 - Use polynomial          */
     /* 0x1105d 4'd7 - Use LB-Key-Packet-Data directly 4'd8 -        */
     /* Use counter incremented every packet 4'd9 - User counter     */
     /* incremented every two clocks                                 */
     /* range: 19:16, access type: RW, default value: 0x1            */
     SOC_PB_PP_REG_FIELD ecmp_lb_hash_index;

     /* EcmpLbKeyUseInPort: If set the In-PP-Port is used in the     */
     /* CRC-LB-Vector                                                */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ecmp_lb_key_use_in_port;

     /* EcmpLbKeyShift: The ECMP load balancing key is barrel        */
     /* shifted by this value.                                       */
     /* range: 24:21, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ecmp_lb_key_shift;

  } __ATTRIBUTE_PACKED__ ecmp_lb_key_cfg_reg;

  /* Trap If Accessed[0..1]: Indicates whether to trap the          */
  /* packet when a fec ptr accesses that entry. Accessed entry       */
  /* resets the bit and sets an interrupt.                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6562 */

     /* TrapIfAccessed: For each one of the 32b register:            */
     /* Writing 0: zero all 32 bits Else, set specific bits          */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD trap_if_accessed;

  } __ATTRIBUTE_PACKED__ trap_if_accessed_reg[2];

  /* Icmp Redirect Enable: Enables Redirect-ICMP check and          */
  /* Action-Profile per port                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6564 */

     /* IcmpRedirectEnable: Enables Redirect-ICMP check and          */
     /* Action-Profile per port                                      */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD icmp_redirect_enable;

  } __ATTRIBUTE_PACKED__ icmp_redirect_enable_reg_0;

  /* Icmp Redirect Enable: Enables Redirect-ICMP check and          */
  /* Action-Profile per port                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6565 */

     /* IcmpRedirectEnable: Enables Redirect-ICMP check and          */
     /* Action-Profile per port                                      */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD icmp_redirect_enable;

  } __ATTRIBUTE_PACKED__ icmp_redirect_enable_reg_1;

  /* Action Profile Uc Rpf: Action Profiles for UC RPF check        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6566 */

     /* ActionProfileUcStrictRpfFailFwd: Action-Profile for          */
     /* Unicast-RPF check failed                                     */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_uc_strict_rpf_fail_fwd;

     /* ActionProfileUcStrictRpfFailSnp: Snoop for the case          */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_uc_strict_rpf_fail_snp;

  } __ATTRIBUTE_PACKED__ action_profile_uc_rpf_reg;

  /* Action Profile Mc Rpf: Action Profiles for MC RPF check        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6567 */

     /* ActionProfileMcExplicitRpfFailFwd: Action-Profile for        */
     /* Multicast-Explicit-RPF check failed                          */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mc_explicit_rpf_fail_fwd;

     /* ActionProfileMcExplicitRpfFailSnp: Snoop for the case        */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mc_explicit_rpf_fail_snp;

     /* ActionProfileMcUseSipAsIsRpfFailFwd: Action-Profile for      */
     /* Multicast-Use-Sip-As-Is-RPF check failed                     */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_as_is_rpf_fail_fwd;

     /* ActionProfileMcUseSipAsIsRpfFailSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_as_is_rpf_fail_snp;

     /* ActionProfileMcUseSipRpfFailFwd: Action-Profile for          */
     /* Multicast-Use-Sip-RPF check failed                           */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_rpf_fail_fwd;

     /* ActionProfileMcUseSipRpfFailSnp: Snoop for the case          */
     /* above                                                        */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_rpf_fail_snp;

     /* ActionProfileMcUseSipEcmpFwd: Action-Profile for             */
     /* Multicast-Use-ECMP check failed                              */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_ecmp_fwd;

     /* ActionProfileMcUseSipEcmpSnp: Snoop for the case above       */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mc_use_sip_ecmp_snp;

  } __ATTRIBUTE_PACKED__ action_profile_mc_rpf_reg;

  /* Action Profile General: General Action Profile                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6568 */

     /* ActionProfileFacilityInvalidFwd: Action-Profile for if       */
     /* both even and odd destinations are invalid                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_facility_invalid_fwd;

     /* ActionProfileFacilityInvalidSnp: Snoop for the case          */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_facility_invalid_snp;

     /* ActionProfileFecEntryAccessedFwd: Action-Profile the         */
     /* case packets can't be sent to previously accessed entry.     */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_fec_entry_accessed_fwd;

     /* ActionProfileFecEntryAccessedSnp: Snoop for the case         */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_fec_entry_accessed_snp;

     /* ActionProfileIcmpRedirectFwd: Action-Profile for             */
     /* Redirect-ICMP packet                                         */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_icmp_redirect_fwd;

     /* ActionProfileIcmpRedirectSnp: Snoop for the case above       */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_icmp_redirect_snp;

  } __ATTRIBUTE_PACKED__ action_profile_general_reg;

  /* Dbg Last Resolved Trap: Holds the last traps that the          */
  /* packet finished with                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6569 */

     /* DbgLastResolvedCpuTrapCode: Holds the last Cpu-Trap-Code     */
     /* that a packet finished with.                                 */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_last_resolved_cpu_trap_code;

     /* DbgLastResolvedSnoopCode: Holds the last Snoop-Code that     */
     /* a packet finished with.                                      */
     /* range: 15:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_last_resolved_snoop_code;

     /* DbgLastResolvedCpuTrapCodeStrength: Holds the last           */
     /* Cpu-Trap-Code-Strength that a packet finished with.          */
     /* range: 18:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_last_resolved_cpu_trap_code_strength;

     /* DbgLastResolvedSnoopCodeStrength: Holds the last             */
     /* Snoop-Code-Strength that a packet finished with.             */
     /* range: 20:19, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_last_resolved_snoop_code_strength;

     /* DbgLastResolvedValid: Validates result - clear on read       */
     /* This register is clear on read.                              */
     /* range: 21:21, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_last_resolved_valid;

  } __ATTRIBUTE_PACKED__ dbg_last_resolved_trap_reg;

  /* Dbg Last Trap Change Destination: Holds the last traps         */
  /* that changed the destination                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x656a */

     /* DbgLastTrapChangeDestination: Holds the last                 */
     /* Cpu-Trap-Code that changed the destination. This             */
     /* register is clear on read.                                   */
     /* range: 7:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_last_trap_change_destination;

     /* DbgLastTrapChangeDestinationValid: Validates result -        */
     /* clear on read This register is clear on read.                */
     /* range: 8:8, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_last_trap_change_destination_valid;

  } __ATTRIBUTE_PACKED__ dbg_last_trap_change_destination_reg;

  /* Dbg Fer Trap:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x656b */

     /* DbgFerTrap:                                                  */
     /* range: 7:0, access type: INT, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_fer_trap;

  } __ATTRIBUTE_PACKED__ dbg_fer_trap_reg;


  /* Soc_petrac Shaper Enable:                                                  */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x6470 */

    /* range: 0:0, access type: INT, default value: 0x0             */
    SOC_PB_PP_REG_FIELD soc_petra_c_shaper_enable;

  } __ATTRIBUTE_PACKED__ soc_petra_c_shaper_enable_reg;

  /* Action Profile Sa Drop Map: Action-Profiles for the case       */
  /* \{FID, SA\} that is found and should be dropped. Selected       */
  /* by In-PP-Port. Action-Profile-SA-Drop-Index                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a0 */

     /* ActionProfileSaDrop0Fwd: Action-Profile when                 */
     /* In-PP-Port. Action-Profile-SA-Drop-Index=0                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_drop_fwd[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

     /* ActionProfileSaDrop0Snp: Snoop for the case above            */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_drop_snp[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

  } __ATTRIBUTE_PACKED__ action_profile_sa_drop_map_reg;

  /* Action Profile Sa Not Found Map: Action-Profiles for the       */
  /* case \{FID, SA\} not found. Selected by                         */
  /* In-PP-Port. Action-Profile-Sa-Not-Found-Index                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a1 */

     /* ActionProfileSaNotFound0Fwd: Action-Profile when             */
     /* In-PP-Port. Action-Profile-Sa-Not-Found-Index=0               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_not_found_fwd[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

     /* ActionProfileSaNotFound0Snp: Snoop for the case above        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_sa_not_found_snp[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

  } __ATTRIBUTE_PACKED__ action_profile_sa_not_found_map_reg;

  /* P2p Cfg: Action-Profile for Transparent P2P Service            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a2 */

     /* ActionProfileP2pMisconfigurationFwd: Action-Profile if       */
     /* lookup in L-EM not found in P2P service                      */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_p2p_misconfiguration_fwd;

     /* ActionProfileP2pMisconfigurationSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_p2p_misconfiguration_snp;

    /* ActionProfileP2pMisconfigurationSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD transparent_p2p_asd_learn_prefix;

  } __ATTRIBUTE_PACKED__ p2p_cfg_reg;

  /* Flp General Cfg: General Configuration for FLP                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a4 */

     /* ForwardingStrength: The strength of the forwarding           */
     /* destination determined by FLP                                */
     /* range: 2:0, access type: RW, default value: 0x4              */
     SOC_PB_PP_REG_FIELD forwarding_strength;

     /* EnableForwardingLookup: For each service type indicates      */
     /* whether lookup should be performed. Service type is          */
     /* encoded (by default) as follows (configurable values are     */
     /* in bold): 3'b000 - AC P2P to AC - no lookup 3'b001 - AC      */
     /* P2P to PWE - no lookup 3'b010 - AC P2P to PBB - no           */
     /* lookup 3'b011 - AC MP - lookup 3'b100 - ISID P2P - no        */
     /* lookup 3'b101 - ISID MP - lookup 3'b110 - TRILL,             */
     /* IP-Tunnel, Label-PWE-MP, Label-VRF, Label-LSP -lookup        */
     /* 3'b111 - LABEL-PWE-P2P - no lookup                           */
     /* range: 10:3, access type: RW, default value: 8'b0110_1000    */
     SOC_PB_PP_REG_FIELD enable_forwarding_lookup;

     /* TcamAccessCycle: Indicates the cycle on which the FLP        */
     /* accesses the TCAM 0: Even cycle 1: Odd cycle                 */
     /* range: 11:11, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tcam_access_cycle;

     /* ElkEnable: Enables lookups for the ELK                       */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD elk_enable;

     /* EgwFullThreshold: When number of elements in the ELK         */
     /* FIFO exceeds this value an indication is sent to the ELK     */
     /* gateway to start producing errors.                           */
     /* range: 21:13, access type: RW, default value: 0xbe           */
     SOC_PB_PP_REG_FIELD egw_full_threshold;

     /* ActionProfileElkErrorFwd: Action-Profile for the case        */
     /* ELK returns Error                                            */
     /* range: 24:22, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_elk_error_fwd;

     /* ActionProfileElkErrorSnp: Snoop for the case above           */
     /* range: 26:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_elk_error_snp;

  } __ATTRIBUTE_PACKED__ flp_general_cfg_reg;

  /* Action Profile Same Interface: Action-Profile for the          */
  /* case where the source interface equals destination             */
  /* interface                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a5 */

     /* ActionProfileSameInterfaceFwd: Action-Profile for the        */
     /* case where the source interface equals destination           */
     /* interface                                                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_same_interface_fwd;

     /* ActionProfileSameInterfaceSnp: Snoop for the case above      */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_same_interface_snp;

  } __ATTRIBUTE_PACKED__ action_profile_same_interface_reg;

  /* Action Profile Da Not Found Map: Action-Profile for the        */
  /* case where the Forwarding-Code is Ethernet and \{FID,          */
  /* DA\} is not foundSelected by                                   */
  /* In-PP-Port. Action-Profile-Da-Not-Found-Index                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a6 */

     /* ActionProfileDaNotFound0Fwd: Action-Profile when             */
     /* In-PP-Port. Action-Profile-Da-Not-Found-Index=0               */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_da_not_found_fwd[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

     /* ActionProfileDaNotFound0Snp: Snoop for the case above        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_da_not_found_snp[SOC_PB_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES];

  } __ATTRIBUTE_PACKED__ action_profile_da_not_found_map_reg;

  /* All Esadi Rbridges: Determines ESADI when Trill is the         */
  /* fwd header                                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a7 */

     /* AllEsadiRbridges: ESADI = (Inner-DA == AllEsadiRbridges)     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD all_esadi_rbridges;

  } __ATTRIBUTE_PACKED__ all_esadi_rbridges_reg[SOC_PB_PP_IHB_ALL_ESADI_RBRIDGES_REG_MULT_NOF_REGS];

  /* Trill Unknown: Action-Profile for Trill                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64a9 */

     /* ActionProfileTrillUnknownUcFwd: Action-Profile if            */
     /* Trill-UC lookup in large-EM was not found                    */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_trill_unknown_uc_fwd;

     /* ActionProfileTrillUnknownUcSnp: Snoop for the case above     */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_trill_unknown_uc_snp;

     /* ActionProfileTrillUnknownMcFwd: Action-Profile if            */
     /* Trill-MC lookup in large-EM was not found                    */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_trill_unknown_mc_fwd;

     /* ActionProfileTrillUnknownMcSnp: Snoop for the case above     */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_trill_unknown_mc_snp;

  } __ATTRIBUTE_PACKED__ trill_unknown_reg;

  /* Ipv4 Cfg: Configuration for IPv4 RPF                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64aa */

     /* RpfAllowDefaultValue: Allow default fec-pointer for RPF      */
     /* check                                                        */
     /* range: 0:0, access type: RW, default value: 0x1              */
     SOC_PB_PP_REG_FIELD rpf_allow_default_value;

     /* CompatibleMcBridgeFallback: If set then IP multicast         */
     /* forwarding for IP compatible multicast packets defaults      */
     /* to VSI default when DIP is not found in routing tables.      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD compatible_mc_bridge_fallback;

     /* ActionProfileUcLooseRpfFwd: Action profile for the case      */
     /* unicast loose RPF failed                                     */
     /* range: 4:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_uc_loose_rpf_fwd;

     /* ActionProfileUcLooseRpfSnp: Snoop for the case above         */
     /* range: 6:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_uc_loose_rpf_snp;

  } __ATTRIBUTE_PACKED__ ipv4_cfg_reg;

  /* Ipv6 Cfg: Action-Profile for IPv6                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64ab */

     /* ActionProfileDefaultUcv6Fwd: Action-Profile if               */
     /* forwarding-code is IPv6uc and lkp failed                     */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_default_ucv6_fwd;

     /* ActionProfileDefaultUcv6Snp: Snoop for the case above        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_default_ucv6_snp;

     /* ActionProfileDefaultMcv6Fwd: Action-Profile if               */
     /* forwarding-code is IPv6mc and lkp failed                     */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_default_mcv6_fwd;

     /* ActionProfileDefaultMcv6Snp: Snoop for the case above        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_default_mcv6_snp;

  } __ATTRIBUTE_PACKED__ ipv6_cfg_reg;

  /* Lsr Elsp Range Min: ELSP minimum range                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64ac */

     /* ElspRangeMin: ELSP minimum range                             */
     /* range: 19:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD elsp_range_min;

  } __ATTRIBUTE_PACKED__ lsr_elsp_range_min_reg;

  /* Lsr Elsp Range Max: ELSP maximum range                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64ad */

     /* ElspRangeMax: ELSP maximum range                             */
     /* range: 19:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD elsp_range_max;

  } __ATTRIBUTE_PACKED__ lsr_elsp_range_max_reg;

  /* Mpls Exp Map: Maps In-EXP a new EXP (8x3b)                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64ae */

     /* REG DEF CHANGE: length changed from 3 bits to 24 bits (8 vals of 3 bits each) */
     /* MplsExp: In-EXP(3b) select one of 8 EXP(3b)                  */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mpls_exp[SOC_PB_PP_IHP_MPLS_EXP_REG_NOF_FLDS];
  } __ATTRIBUTE_PACKED__ mpls_exp_map_reg;


  /* VSI and System-VSI for Transparent P2P Pwe service                         */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64af */

    /* VSI for LSR                             */
    /* range: 13:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD transparent_p2p_pwe_vsi;
    /* System VSI for LSR                             */
    /* range: 29:14, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD transparent_p2p_pwe_system_vsi;

  } __ATTRIBUTE_PACKED__ transparent_p2p_pwe_vsi_reg;


  /* VSI and System-VSI for Transparent P2P AC service      */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64a3 */

    /* VSI for LSR                             */
    /* range: 13:0, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD transparent_p2p_ac_vsi;
    /* System VSI for LSR                             */
    /* range: 29:14, access type: RW, default value: 0x0             */
    SOC_PB_PP_REG_FIELD transparent_p2p_ac_system_vsi;

  } __ATTRIBUTE_PACKED__ transparent_p2p_ac_vsi_reg;


  /* Lsr Action Profile: Action -Profiles for the case              */
  /* forwarding code is MPLS                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64b0 */

     /* ActionProfileMplsP2pNoBosFwd: Transparent-P2P and            */
     /* forwarding header is not BOS                                 */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_p2p_no_bos_fwd;

     /* ActionProfileMplsP2pNoBosSnp: Snoop for the case above       */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_p2p_no_bos_snp;

     /* ActionProfileMplsControlWordTrapFwd: Transparent-P2P and     */
     /* there is a Code-Word and first nibble = 1                    */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_control_word_trap_fwd;

     /* ActionProfileMplsControlWordTrapSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_control_word_trap_snp;

     /* ActionProfileMplsControlWordDropFwd: Transparent-P2P and     */
     /* there is a Code-Word and first nibble > 1                    */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_control_word_drop_fwd;

     /* ActionProfileMplsControlWordDropSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_control_word_drop_snp;

     /* ActionProfileMplsUnknownLabelFwd: Lookup not found           */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_unknown_label_fwd;

     /* ActionProfileMplsUnknownLabelSnp: Snoop for the case         */
     /* above                                                        */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_unknown_label_snp;

     /* ActionProfileMplsP2pMplsx4Fwd: P2P packet with 4 MPLS        */
     /* headers                                                      */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_p2p_mplsx4_fwd;

     /* ActionProfileMplsP2pMplsx4Snp: Snoop for the case above      */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mpls_p2p_mplsx4_snp;

     /* ParserPmfProfileEthOverPwe: Parser-Pmf-Profile for           */
     /* Ethernet over PWE This signal also apear at the VTT          */
     /* range: 28:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD parser_pmf_profile_eth_over_pwe;

  } __ATTRIBUTE_PACKED__ lsr_action_profile_reg;

  /* Learning Cfg: Learning configuration                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64b1 */

     /* LearnEnableMap: Maps data-processing-profile to              */
     /* learn-enable. Always enabled for Ethernet. For IP and        */
     /* MPLS will be set only for opportunistic ingress              */
     /* learning.                                                    */
     /* range: 15:0, access type: RW, default value: 0x6             */
     SOC_PB_PP_REG_FIELD learn_enable_map;

     /* IngressLearning: Enables learning for SA-lookup              */
     /* range: 16:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ingress_learning;

     /* OpportunisticLearning: Determines whether the LARGE_EM or        */
     /* the ingress PP pipe perform SA lookup. Decoded as: 1'b0      */
     /* - Pipe Learning. 1'b1 - Opportunistic Learning. This         */
     /* register must be configured to match register                */
     /* SA-Lookup-Type.                                              */
     /* range: 17:17, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD opportunistic_learning;

  } __ATTRIBUTE_PACKED__ learning_cfg_reg;

  /* Mef L2cp Peer Bitmap: Specifies if the DA is an MEF            */
  /* layer 2 control protocol service frame.                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64b2 */

     /* MefL2cpPeerBitmap: acseesed with [\{MEF-L2CP-Profile,        */
     /* DA[5:0]\}                                                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mef_l2cp_peer_bitmap;

  } __ATTRIBUTE_PACKED__ mef_l2cp_peer_bitmap_reg[SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS];

  /* Mef L2cp Drop Bitmap: Specifies if the DA is an MEF            */
  /* layer 2 control protocol service frame.                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64b6 */

     /* MefL2cpDropBitmap: acseesed with [\{MEF-L2CP-Profile,        */
     /* DA[5:0]\}                                                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mef_l2cp_drop_bitmap;

  } __ATTRIBUTE_PACKED__ mef_l2cp_drop_bitmap_reg[SOC_PB_PP_IHB_MEF_L2CP_DROP_BITMAP_REG_MULT_NOF_REGS];

  /* Ethernet Action Profiles0: Action-Profiles for                 */
  /* EthernetStrength determined by ProtocolTrap registerEach       */
  /* trap may be enabled or disabled                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64ba */

     /* ActionProfileEthMefL2cpPeerFwd: Ethernet L2CP-Peer           */
     /* bitmap                                                       */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_eth_mef_l2cp_peer_fwd;

     /* ActionProfileEthMefL2cpPeerSnp: Snoop for the case above     */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_eth_mef_l2cp_peer_snp;

     /* ActionProfileEthMefL2cpDropFwd: Ethernet L2CP-Drop           */
     /* bitmap                                                       */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_eth_mef_l2cp_drop_fwd;

     /* ActionProfileEthMefL2cpDropSnp: Snoop for the case above     */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_eth_mef_l2cp_drop_snp;

  } __ATTRIBUTE_PACKED__ ethernet_action_profiles0_reg;

  /* Ethernet Action Profiles1: Action-Profiles for                 */
  /* EthernetStrength determined by ProtocolTrap registerEach       */
  /* trap may be enabled or disabled                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64bb */

     /* ActionProfileEthFlIgmpMembershipQueryFwd: Forwarding         */
     /* layer IGMP Membership-Query                                  */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD igmp_membership_query_fwd;

     /* ActionProfileEthFlIgmpMembershipQuerySnp: Snoop for the      */
     /* case above                                                   */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD igmp_membership_query_snp;

     /* ActionProfileEthFlIgmpReportLeaveMsgFwd: Forwarding          */
     /* layer IGMP Report-Leave-Msg                                  */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD igmp_report_leave_msg_fwd;

     /* ActionProfileEthFlIgmpReportLeaveMsgSnp: Snoop for the       */
     /* case above                                                   */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD igmp_report_leave_msg_snp;

     /* ActionProfileEthFlIgmpUndefinedFwd: Forwarding layer         */
     /* IGMP Undefined                                               */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD igmp_undefined_fwd;

     /* ActionProfileEthFlIgmpUndefinedSnp: Snoop for the case       */
     /* above                                                        */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD igmp_undefined_snp;

     /* ActionProfileEthFlIcmpv6MldMcListenerQueryFwd:               */
     /* Forwarding layer MLD MC-Listener-Query                       */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_mc_listener_query_fwd;

     /* ActionProfileEthFlIcmpv6MldMcListenerQuerySnp: Snoop for     */
     /* the case above                                               */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_mc_listener_query_snp;

     /* ActionProfileEthFlIcmpv6MldReportDoneFwd: Forwarding         */
     /* layer MLD Report-Done                                        */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_report_done_fwd;

     /* ActionProfileEthFlIcmpv6MldReportDoneSnp: Snoop for the      */
     /* case above                                                   */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_report_done_snp;

     /* ActionProfileEthFlIcmpv6MldUndefinedFwd: Forwarding          */
     /* layer IGMP Undefined                                         */
     /* range: 27:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_undefined_fwd;

     /* ActionProfileEthFlIcmpv6MldUndefinedSnp: Snoop for the       */
     /* case above                                                   */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD icmpv6_mld_undefined_snp;

  } __ATTRIBUTE_PACKED__ ethernet_action_profiles1_reg;

  /* Ipv4 Action Profiles0: Action-Profiles for IPv4Strength        */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64bc */

     /* ActionProfileIpv4VersionErrorFwd: Version is different       */
     /* than 4                                                       */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_version_error_fwd;

     /* ActionProfileIpv4VersionErrorSnp: Snoop for the case         */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_version_error_snp;

     /* ActionProfileIpv4ChecksumVersionErrorFwd: Checksum           */
     /* error, Version error. This trap can be taken if at the       */
     /* forwarding ipv4 header there was checksum error or           */
     /* version error or that there is a GRE header.                 */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_checksum_version_error_fwd;

     /* ActionProfileIpv4ChecksumVersionErrorSnp: Snoop for the      */
     /* case above                                                   */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_checksum_version_error_snp;

     /* ActionProfileIpv4HeaderLengthErrorFwd: IHL is less than      */
     /* 5                                                            */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_header_length_error_fwd;

     /* ActionProfileIpv4HeaderLengthErrorSnp: Snoop for the         */
     /* case above                                                   */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_header_length_error_snp;

     /* ActionProfileIpv4TotalLengthErrorFwd: Total-Length is        */
     /* less than 20                                                 */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_total_length_error_fwd;

     /* ActionProfileIpv4TotalLengthErrorSnp: Snoop for the case     */
     /* above                                                        */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_total_length_error_snp;

     /* ActionProfileIpv4Ttl0Fwd: TTL == 0                           */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_ttl0_fwd;

     /* ActionProfileIpv4Ttl0Snp: Snoop for the case above           */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_ttl0_snp;

     /* ActionProfileIpv4HasOptionsFwd: IHL is greater than 5        */
     /* range: 27:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_has_options_fwd;

     /* ActionProfileIpv4HasOptionsSnp: Snoop for the case above     */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_has_options_snp;

  } __ATTRIBUTE_PACKED__ ipv4_action_profiles0_reg;

  /* Ipv4 Action Profiles1: Action-Profiles for IPv4Strength        */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64bd */

     /* ActionProfileIpv4Ttl1Fwd: TTL == 1                           */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_ttl1_fwd;

     /* ActionProfileIpv4Ttl1Snp: Snoop for the case above           */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_ttl1_snp;

     /* ActionProfileIpv4SipEqualDipFwd: SIP == DIP                  */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_sip_equal_dip_fwd;

     /* ActionProfileIpv4SipEqualDipSnp: Snoop for the case          */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_sip_equal_dip_snp;

     /* ActionProfileIpv4DipZeroFwd: DIP == 0                        */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_dip_zero_fwd;

     /* ActionProfileIpv4DipZeroSnp: Snoop for the case above        */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_dip_zero_snp;

     /* ActionProfileIpv4SipIsMcFwd: SIP is multicast                */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_sip_is_mc_fwd;

     /* ActionProfileIpv4SipIsMcSnp: Snoop for the case above        */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv4_sip_is_mc_snp;

  } __ATTRIBUTE_PACKED__ ipv4_action_profiles1_reg;

  /* Ipv6 Action Profiles0: Action-Profiles for IPv6Strength        */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64be */

     /* ActionProfileIpv6VersionErrorFwd: Version is different       */
     /* than 6                                                       */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_version_error_fwd;

     /* ActionProfileIpv6VersionErrorSnp: Snoop for the case         */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_version_error_snp;

     /* ActionProfileIpv6HopCount0Fwd: Hop-Count is zero             */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_hop_count0_fwd;

     /* ActionProfileIpv6HopCount0Snp: Snoop for the case above      */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_hop_count0_snp;

     /* ActionProfileIpv6HopCount1Fwd: Hop-Count is one              */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_hop_count1_fwd;

     /* ActionProfileIpv6HopCount1Snp: Snoop for the case above      */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_hop_count1_snp;

     /* ActionProfileIpv6UnspecifiedDestinationFwd: Unspecified      */
     /* destination address - Dst-IP is all zeros                    */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_unspecified_destination_fwd;

     /* ActionProfileIpv6UnspecifiedDestinationSnp: Snoop for        */
     /* the case above                                               */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_unspecified_destination_snp;

     /* ActionProfileIpv6LoopbackAddressFwd: Loopback source or      */
     /* destination address                                          */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_loopback_address_fwd;

     /* ActionProfileIpv6LoopbackAddressSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_loopback_address_snp;

     /* ActionProfileIpv6MulticastSourceFwd: Multicast source        */
     /* address - Src-IP[127:120] is equal to 8'hFF                  */
     /* range: 27:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_multicast_source_fwd;

     /* ActionProfileIpv6MulticastSourceSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_multicast_source_snp;

  } __ATTRIBUTE_PACKED__ ipv6_action_profiles0_reg;

  /* Ipv6 Action Profiles1: Action-Profiles for IPv6Strength        */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64bf */

     /* ActionProfileIpv6NextHeaderNullFwd: Hop-by-Hop header -      */
     /* Next-Header is zero                                          */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_next_header_null_fwd;

     /* ActionProfileIpv6NextHeaderNullSnp: Snoop for the case       */
     /* above                                                        */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_next_header_null_snp;

     /* ActionProfileIpv6UnspecifiedSourceFwd: Unspecified           */
     /* source address - Src-IP is all zeros                         */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_unspecified_source_fwd;

     /* ActionProfileIpv6UnspecifiedSourceSnp: Snoop for the         */
     /* case above                                                   */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_unspecified_source_snp;

     /* ActionProfileIpv6LinkLocalDestinationFwd: Link-Local         */
     /* destination address - Dst-IP[127:118] is equal to            */
     /* 10'b1111_1110_10                                             */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_link_local_destination_fwd;

     /* ActionProfileIpv6LinkLocalDestinationSnp: Snoop for the      */
     /* case above                                                   */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_link_local_destination_snp;

     /* ActionProfileIpv6SiteLocalDestinationFwd: Site-Local         */
     /* destination address - Dst-IP[127:118] is equal to            */
     /* 10'b1111_1110_11                                             */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_site_local_destination_fwd;

     /* ActionProfileIpv6SiteLocalDestinationSnp: Snoop for the      */
     /* case above                                                   */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_site_local_destination_snp;

     /* ActionProfileIpv6LinkLocalSourceFwd: Link-Local source       */
     /* address - Src-IP[127:118] is equal to 10'b1111_1110_10       */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_link_local_source_fwd;

     /* ActionProfileIpv6LinkLocalSourceSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_link_local_source_snp;

     /* ActionProfileIpv6SiteLocalSourceFwd: Site-Local source       */
     /* address - Src-IP[127:118] is equal to 10'b1111_1110_11       */
     /* range: 27:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_site_local_source_fwd;

     /* ActionProfileIpv6SiteLocalSourceSnp: Snoop for the case      */
     /* above                                                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_site_local_source_snp;

  } __ATTRIBUTE_PACKED__ ipv6_action_profiles1_reg;

  /* Ipv6 Action Profiles2: Action-Profiles for IPv6Strength        */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c0 */

     /* ActionProfileIpv6IPv4CompatibleDestinationFwd:               */
     /* IPv4-Compatible destination address - Dst-IP[127:32] is      */
     /* equal to 0                                                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_ipv4_compatible_destination_fwd;

     /* ActionProfileIpv6IPv4CompatibleDestinationSnp:               */
     /* IPv4-Compatible destination address - Dst-IP[127:32] is      */
     /* equal to 0                                                   */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_ipv4_compatible_destination_snp;

     /* ActionProfileIpv6IPv4MappedDestinationFwd: IPv4-Mapped       */
     /* destination address - Dst-IP[127:32] is equal to             */
     /* 96'h0000_FFFF_0000_0000_0000_0000                            */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_ipv4_mapped_destination_fwd;

     /* ActionProfileIpv6IPv4MappedDestinationSnp: Snoop for the     */
     /* case above                                                   */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_ipv4_mapped_destination_snp;

     /* ActionProfileIpv6MulticastDestinationFwd: Multicast          */
     /* destination - Dst-IP[127:120] is equal to 8'hFF              */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_multicast_destination_fwd;

     /* ActionProfileIpv6MulticastDestinationSnp: Snoop for the      */
     /* case above                                                   */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ipv6_multicast_destination_snp;

  } __ATTRIBUTE_PACKED__ ipv6_action_profiles2_reg;

  /* Mpls Action Profiles: Action-Profiles for MPLSStrength         */
  /* determined by ProtocolTrap registerEach trap may be            */
  /* enabled or disabled                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c1 */

     /* ActionProfileMplsTtl0Fwd: TTL == 0                           */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_ttl0_fwd;

     /* ActionProfileMplsTtl0Snp: Snoop for the case above           */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_ttl0_snp;

     /* ActionProfileMplsTtl1Fwd: TTL == 1                           */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_ttl1_fwd;

     /* ActionProfileMplsTtl1Snp: Snoop for the case above           */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_mpls_ttl1_snp;

  } __ATTRIBUTE_PACKED__ mpls_action_profiles_reg;

  /* Tcp Action Profiles: Action-Profiles for TCP                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c2 */

     /* ActionProfileTcpSnFlagsZeroFwd: Sequence number and          */
     /* flags are zero Sequence-Number and Flags (6) are both        */
     /* zero                                                         */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_tcp_sn_flags_zero_fwd;

     /* ActionProfileTcpSnFlagsZeroSnp: Snoop for the case above     */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_tcp_sn_flags_zero_snp;

     /* ActionProfileTcpSnZeroFlagsSetFwd: Sequence number zero      */
     /* and either FIN/URG or PSH are one Sequence-Number is         */
     /* zero and either Flags. FIN, Flags. URG or FLAGS. PSH are        */
     /* set                                                          */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_tcp_sn_zero_flags_set_fwd;

     /* ActionProfileTcpSnZeroFlagsSetSnp: Snoop for the case        */
     /* above                                                        */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_tcp_sn_zero_flags_set_snp;

     /* ActionProfileTcpSynFinFwd: Both Flags. SYN and Flags. FIN      */
     /* are set                                                      */
     /* range: 12:10, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_syn_fin_fwd;

     /* ActionProfileTcpSynFinSnp: Snoop for the case above          */
     /* range: 14:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_syn_fin_snp;

     /* ActionProfileTcpEqualPortsFwd: Source-Port equals            */
     /* Destination-Port                                             */
     /* range: 17:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_equal_ports_fwd;

     /* ActionProfileTcpEqualPortsSnp: Snoop for the case above      */
     /* range: 19:18, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_equal_ports_snp;

     /* ActionProfileTcpFragmentIncompleteHeaderFwd: TCP             */
     /* fragment without full TCP header - Layer-3 is IPv4 and       */
     /* IP-Header. Fragmented and IP-Header. Fragment-Offset is        */
     /* zero and (IPv4-Header. Total-Length - 4 *                     */
     /* IPv4-Header. IHL) is less than 20B                            */
     /* range: 22:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_fragment_incomplete_header_fwd;

     /* ActionProfileTcpFragmentIncompleteHeaderSnp: Snoop for       */
     /* the case above                                               */
     /* range: 24:23, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_fragment_incomplete_header_snp;

     /* ActionProfileTcpFragmentOffsetLt8Fwd: TCP fragment with      */
     /* offset less than 8 - layer-3 is IPv4 and                     */
     /* IP-Header. Fragmented and IP-Header. Fragment-Offset is        */
     /* less than 8                                                  */
     /* range: 27:25, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_fragment_offset_lt8_fwd;

     /* ActionProfileTcpFragmentOffsetLt8Snp: Snoop for the case     */
     /* above                                                        */
     /* range: 29:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_tcp_fragment_offset_lt8_snp;

  } __ATTRIBUTE_PACKED__ tcp_action_profiles_reg;

  /* Udp Action Profiles: Action-Profiles for UDP                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c3 */

     /* ActionProfileUdpEqualPortsFwd: Source-Port equals            */
     /* Destination-Port                                             */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_udp_equal_ports_fwd;

     /* ActionProfileUdpEqualPortsSnp: Snoop for the case above      */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_udp_equal_ports_snp;

  } __ATTRIBUTE_PACKED__ udp_action_profiles_reg;

  /* Icmp Action Profiles: Action-Profiles for ICMP                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c4 */

     /* ActionProfileIcmpDataGt_576Fwd: ICMP Ping with data more     */
     /* than 576 bytes - layer-3 is IPv4 and                         */
     /* (IPv4-Header. Total-Length - 4* IPv4-Header. IHL) is           */
     /* greater than 576B or layer-3 is IPv6 and                     */
     /* IPv6-Header. Payload-Length is greater than 576B              */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmp_data_gt_576_fwd;

     /* ActionProfileIcmpDataGt_576Snp: Snoop for the case above     */
     /* range: 4:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmp_data_gt_576_snp;

     /* ActionProfileIcmpFragmentedFwd: IP-Header. Fragmented         */
     /* range: 7:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmp_fragmented_fwd;

     /* ActionProfileIcmpFragmentedSnp: Snoop for the case above     */
     /* range: 9:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_icmp_fragmented_snp;

  } __ATTRIBUTE_PACKED__ icmp_action_profiles_reg;

  /* Dbg Program Selection Map:                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c5 */

     /* DbgProgramSelectionMap: For each program selected, a         */
     /* sticy bit is set when used                                   */
     /* range: 15:0, access type: INT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_program_selection_map;

  } __ATTRIBUTE_PACKED__ dbg_program_selection_map_reg;

  /* Dbg Flp Trap0:                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c6 */

     /* DbgFlpTrap0:                                                 */
     /* range: 31:0, access type: INT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_flp_trap0;

  } __ATTRIBUTE_PACKED__ dbg_flp_trap0_reg;

  /* Dbg Flp Trap1:                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c7 */

     /* DbgFlpTrap1:                                                 */
     /* range: 31:0, access type: INT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_flp_trap1;

  } __ATTRIBUTE_PACKED__ dbg_flp_trap1_reg;

  /* Dbg Flp Trap2:                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c8 */

     /* DbgFlpTrap2:                                                 */
     /* range: 12:0, access type: INT, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_flp_trap2;

  } __ATTRIBUTE_PACKED__ dbg_flp_trap2_reg;

  /* Dbg Flp Fifo:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x64c9 */

     /* FlpFifoEntryCount: Stores the highest entry count at the     */
     /* FLP Fifo                                                     */
     /* range: 8:0, access type: WMK, default value: 0x0             */
     SOC_PB_PP_REG_FIELD flp_fifo_entry_count;

     /* FlpFifoLastReadAddress: The last address accessed for        */
     /* reading at the FLP fifo                                      */
     /* range: 20:13, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD flp_fifo_last_read_address;

     /* FlpFifoLastWriteAddress: The last address accessed for       */
     /* writing at the FLP fifo                                      */
     /* range: 28:21, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD flp_fifo_last_write_address;

  } __ATTRIBUTE_PACKED__ dbg_flp_fifo_reg;

  /* Soc_petra C Chicken bit:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x656d */

    /* range: 0:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD chicken_bit_mc_use_sip_ecmp;

  } __ATTRIBUTE_PACKED__ soc_petra_c_chicken_bit_reg;

  /* Soc_petra C Cfg:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64ca */

    /* range: 0:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD extended_vrf;

    /* range: 1:1, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD elk_key;

    /* range: 2:2, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD l2_keys_use_ac;

  } __ATTRIBUTE_PACKED__ soc_petra_c_cfg_reg;

  /* Soc_petra C ACL Key select:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64cb */

    /* range: 1:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD soc_petra_c_acl_key_select;

  } __ATTRIBUTE_PACKED__ soc_petra_c_acl_key_select_reg;

  /* Soc_petra C ELK DB ID:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64cc */

    /* range: 15:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD soc_petra_c_elk_db_id;

  } __ATTRIBUTE_PACKED__ soc_petra_c_elk_db_id_reg;

  /* Soc_petra C UC VRF Config:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64cd */

    /* range: 15:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD destination;

    /* range: 16:16, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD l3_vpn_dflt_routing;

  } __ATTRIBUTE_PACKED__ soc_petra_c_dflt_uc_vrf_config_reg;

  /* Soc_petra C MC VRF Config:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64ce */

    /* range: 15:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD destination;

    /* range: 16:16, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD l3_vpn_dflt_routing;

  } __ATTRIBUTE_PACKED__ soc_petra_c_dflt_mc_vrf_config_reg;

  /* Soc_petra C Chicken Bits:                                    */
  struct
  {
    SOC_PB_PP_REG_ADDR  addr; /* 0x64cf */

    /* range: 0:0, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD chicken_bit_mldigmp_encapsulation;

    /* range: 1:1, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD chicken_bit_checksum_err;

    /* range: 2:2, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD chicken_bit_l2_cp;

    /* range: 3:3, access type: RW, default value: 0x1            */
    SOC_PB_PP_REG_FIELD chicken_bit_zero_elk_reply_if_not_found;

  } __ATTRIBUTE_PACKED__ soc_petra_c_chicken_bits_reg;

  /* Non Ethernet Meter Pointer:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6590 */

     /* NonEthernetMeterPointer: Pointer to the meter that           */
     /* counts non-Ethernet packets, Should not be configured to     */
     /* values above 9'd339                                          */
     /* range: 8:0, access type: RW, default value: 0x140            */
     SOC_PB_PP_REG_FIELD non_ethernet_meter_pointer;

  } __ATTRIBUTE_PACKED__ non_ethernet_meter_pointer_reg;

  /* Lpm Query Configuration: Depth of each sub-key                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65b0 */

     /* L2Depth: Number of key bits for level 2                      */
     /* range: 2:0, access type: RW, default value: 0x4              */
     SOC_PB_PP_REG_FIELD l2_depth;

     /* L3Depth: Number of key bits for level 3                      */
     /* range: 5:3, access type: RW, default value: 0x4              */
     SOC_PB_PP_REG_FIELD l3_depth;

     /* L4Depth: Number of key bits for level 4                      */
     /* range: 8:6, access type: RW, default value: 0x4              */
     SOC_PB_PP_REG_FIELD l4_depth;

     /* L5Depth: Number of key bits for level 5                      */
     /* range: 12:9, access type: RW, default value: 0x4             */
     SOC_PB_PP_REG_FIELD l5_depth;

     /* VrfDepth: Number of key bits for VRF                         */
     /* range: 16:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD vrf_depth;

  } __ATTRIBUTE_PACKED__ lpm_query_configuration_reg;

  /* Query Key: Debug: Quiery for the LPM - Key                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65b1 */

     /* QueryKey: Key to search for in LPM-DB - \{VRF, IP\}          */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD query_key;

  } __ATTRIBUTE_PACKED__ query_key_reg_0;

  /* Query Key: Debug: Quiery for the LPM - Key                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65b2 */

     /* QueryKey: Key to search for in LPM-DB - \{VRF, IP\}          */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD query_key;

  } __ATTRIBUTE_PACKED__ query_key_reg_1;

  /* Query Payload: Debug: Quiery for the LPM - Payload             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65b3 */

     /* QueryTrigger: Enable query with QueryKey When query is       */
     /* done - this bit is reset                                     */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD query_trigger;

     /* QueryPayload: LPM result                                     */
     /* range: 14:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD query_payload;

  } __ATTRIBUTE_PACKED__ query_payload_reg;

  /* Lpm Query Cnt: Number of queries accessed the LPM. FLP          */
  /* queries and CPU queries                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65b4 */

     /* LpmQueryCnt: Number of queries accessed the LPM. FLP         */
     /* queries and CPU queries This register is clear on read.      */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD lpm_query_cnt;

     /* LpmQueryCntOverflow: Counter overflow                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD lpm_query_cnt_overflow;

  } __ATTRIBUTE_PACKED__ lpm_query_cnt_reg;

  /* PMFGeneral Configuration1: Enable the PMF to access the        */
  /* TCAM - sets the PMF strength and includes the                  */
  /* force-program debug feature.                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6501 */

     /* EnableFirstPassBankA_Access: Enable the PMF access the       */
     /* TCAM bank-A in the first lookup.                             */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_pass_bank_access[SOC_PB_PP_REGS_TCAM_NOF_BANKS][SOC_PB_PP_REGS_TCAM_NOF_CYCLES];

     /* UseDefaultKeyA: If set, then programmable key-A will         */
     /* have default value =                                         */
     /* \{Parser-PMF-Profile,CPU-Trap-Code-Profile,Outer-Tag-QoS,In-AC,TC,Out-AC,In-PP-Port,LLVP-Incoming-Tag-Structure,Ethernet-Type-Code\};  */
     /* otherwise, defult value is 0.                                */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD use_default_key_a;

     /* UseDefaultKeyB: If set, then programmable key-B will         */
     /* have default value =                                         */
     /* \{Parser-PMF-Profile,CPU-Trap-Code-Profile,Outer-Tag-QoS,In-AC,TC,Out-AC,In-PP-Port,LLVP-Incoming-Tag-Structure,Ethernet-Type-Code\};  */
     /* otherwise, defult value is 0.                                */
     /* range: 13:13, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD use_default_key_b;

     /* FixedKeysUseAC: If set, then in AC will be used in the       */
     /* fixed keys. Affects L2-Ethernet key and L3-IPv4 key.         */
     /* range: 14:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD fixed_keys_use_ac;

     /* FixedKeysUseTCP_Control:                                     */
     /* range: 15:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD fixed_keys_use_tcp_control;

     /* PMF_Strength: Configure the PMF-Strength. According to       */
     /* this strength, the PMF can update the packet's               */
     /* Destination, TC and DP.                                      */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD pmf_strength;

     /* ForceProgram: If this bit is set, then the PMF will          */
     /* execute program-id == ForceProgramNum; otherwise, the        */
     /* program will be selected according to program-select         */
     /* parameters.                                                  */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD force_program;

     /* ForceProgramNum: If ForceProgram is set, then the PMF        */
     /* will execute program-id == ForceProgramNum; otherwise,       */
     /* it won't have any affect.                                    */
     /* range: 28:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD force_program_num;

  } __ATTRIBUTE_PACKED__ pmfgeneral_configuration1_reg;

  /* PMF CPU Trap Code Profile:                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6502 */

     /* PMF_CPU_TrapCodeProfile: A table of 256x1b, used for the     */
     /* defult key. It is accessed with the address                  */
     /* Forward-Action. CPU-Trap-Code.                                */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pmf_cpu_trap_code_profile;

  } __ATTRIBUTE_PACKED__ pmf_cpu_trap_code_profile_reg[SOC_PB_PP_IHP_PMF_CPU_TRAP_CODE_PRO_NOF_REGS];

  /* PMF Default TCAM Action:                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x650a */

     /* PMF_DefaultTCAM_Action: If the TCAM returns no-match,        */
     /* then the TCAM action will be replaced with                   */
     /* PMF_DefaultTCAM_Action                                       */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pmf_default_tcam_action;

  } __ATTRIBUTE_PACKED__ pmf_default_tcam_action_reg;

  /* Packet Header Size Range[0..2]: Used to map the packet         */
  /* header size to one of 4 ranges as                              */
  /* follows:header_size_range = header_size_in_range_0 ?           */
  /* 2'd0: header_size_in_range_1 ? 2'd1:                           */
  /* header_size_in_range_2 ? 2'd2: 2'd3.                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x650b */

     /* PktHdrSizeRangeLow: Header size range low                    */
     /* range: 6:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD pkt_hdr_size_range_low;

     /* PktHdrSizeRangeHigh: Header size range high                  */
     /* range: 14:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pkt_hdr_size_range_high;

  } __ATTRIBUTE_PACKED__ packet_header_size_range_reg[SOC_PB_PP_IHP_PMF_PKT_HDR_SZ_RNG_NOF_REGS];

  /* ECMP LB Key Select:                                            */
  /* Selected-Program. TagSelectionProfileIndex selects one of       */
  /* the following eight ECMP_LB_KeySelect, and according to        */
  /* the value, ECMP-LB-Key-Packet-Data is set.                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6511 */

     /* ECMP_LB_KeySelect0: 3'd0 : Key-A[19:0] 3'd1 :                */
     /* Key-A[51:32] 3'd2 : Key-B[19:0] 3'd3 : Key-B[51:32] 3'd4     */
     /* : TACM-PD1-Data[19:0] 3'd5 : TACM-PD2-Data[19:0] 3'd6 :      */
     /* TCAM-Sel3-Data[19:0] 3'd7 : Direct-Table-Data[19:0]          */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ecmp_lb_key_select[SOC_PB_PMF_NOF_TAG_PROFILES];

  } __ATTRIBUTE_PACKED__ ecmp_lb_key_select_reg;

  /* Dbg Eth Key:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6524 */

     /* DbgEthKey: Holds Eth key for the last packet                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_eth_key;

  } __ATTRIBUTE_PACKED__ dbg_eth_key_reg_0;

  /* Dbg Eth Key:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6525 */

     /* DbgEthKey: Holds Eth key for the last packet                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_eth_key;

  } __ATTRIBUTE_PACKED__ dbg_eth_key_reg_1;

  /* Dbg Eth Key:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6526 */

     /* DbgEthKey: Holds Eth key for the last packet                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_eth_key;

  } __ATTRIBUTE_PACKED__ dbg_eth_key_reg_2;

  /* Dbg Eth Key:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6527 */

     /* DbgEthKey: Holds Eth key for the last packet                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_eth_key;

  } __ATTRIBUTE_PACKED__ dbg_eth_key_reg_3;

  /* Dbg Eth Key:                                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6528 */

     /* DbgEthKey: Holds Eth key for the last packet                 */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_eth_key;

  } __ATTRIBUTE_PACKED__ dbg_eth_key_reg_4;

  /* Dbg IPv4 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6529 */

     /* DbgIPv4Key: Holds IPv4 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv4_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv4_key_reg_0;

  /* Dbg IPv4 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652a */

     /* DbgIPv4Key: Holds IPv4 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv4_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv4_key_reg_1;

  /* Dbg IPv4 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652b */

     /* DbgIPv4Key: Holds IPv4 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv4_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv4_key_reg_2;

  /* Dbg IPv4 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652c */

     /* DbgIPv4Key: Holds IPv4 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv4_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv4_key_reg_3;

  /* Dbg IPv4 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652d */

     /* DbgIPv4Key: Holds IPv4 key for the last packet               */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv4_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv4_key_reg_4;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652e */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_0;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x652f */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_1;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6530 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_2;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6531 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_3;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6532 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_4;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6533 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_5;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6534 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_6;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6535 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_7;

  /* Dbg IPv6 Key:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x6536 */

     /* DbgIPv6Key: Holds IPv6 key for the last packet               */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_ipv6_key;

  } __ATTRIBUTE_PACKED__ dbg_ipv6_key_reg_8;

  /* Dbg FEM[0..7]Key: Holds The 32bit Key for FEMx.                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x653a */

     /* DbgFEMKey:                                                   */
     /* range: 31:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_femkey;

  } __ATTRIBUTE_PACKED__ dbg_femkey_reg;

  /* Dbg FEM[0..7]: Holds the Program, action-valid,                */
  /* action-type and the action for FEMx.                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x653b */

     /* DbgFEMActionValid:                                           */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_femaction_valid;

     /* DbgFEMActionType:                                            */
     /* range: 7:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD dbg_femaction_type;

     /* DbgFEMAction:                                                */
     /* range: 24:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD dbg_femaction;

     /* DbgFEMProgram:                                               */
     /* range: 29:28, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD dbg_femprogram;

  } __ATTRIBUTE_PACKED__ dbg_fem_reg;

  /* Tcam Controller: Control the move operation at the tcam        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e0 */

     /* TcamCtrlTrigger: Enable move operation When operation is     */
     /* done, this bit is reset                                      */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tcam_ctrl_trigger;

     /* TcamCtrlBank: Tcam bank to perform the operation on          */
     /* range: 2:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tcam_ctrl_bank;

     /* TcamCtrlSrc: Source address to move from                     */
     /* range: 11:3, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD tcam_ctrl_src;

     /* TcamCtrlDst: Destination address to move to                  */
     /* range: 20:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tcam_ctrl_dst;

     /* TcamCtrlRange: Range of addresses to move                    */
     /* range: 29:21, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tcam_ctrl_range;

  } __ATTRIBUTE_PACKED__ tcam_controller_reg;

  /* Pmf Query Cnt: Number of queries accessed from PMF             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e1 */

     /* PmfQueryCnt: Number of queries accessed from PMF This        */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pmf_query_cnt;

     /* PmfQueryCntOverflow: Counter overflow                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD pmf_query_cnt_overflow;

  } __ATTRIBUTE_PACKED__ pmf_query_cnt_reg;

  /* Flp Query Cnt: Number of queries accessed from FLP             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e2 */

     /* FlpQueryCnt: Number of queries accessed from FLP This        */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD flp_query_cnt;

     /* FlpQueryCntOverflow: Counter overflow                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD flp_query_cnt_overflow;

  } __ATTRIBUTE_PACKED__ flp_query_cnt_reg;

  /* Egq Query Cnt: Number of queries accessed from EGQ             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e3 */

     /* EgqQueryCnt: Number of queries accessed from EGQ This        */
     /* register is clear on read.                                   */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD egq_query_cnt;

     /* EgqQueryCntOverflow: Counter overflow                        */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD egq_query_cnt_overflow;

  } __ATTRIBUTE_PACKED__ egq_query_cnt_reg;

  /* Query Cnt Enable: Enable counting queries for each             */
  /* interface per bank                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e4 */

     /* PmfQueryCntEnable: Enable counting PMF queries accessed      */
     /* to Bank A, B, C, D                                           */
     /* range: 3:0, access type: RW, default value: 0xf              */
     SOC_PB_PP_REG_FIELD pmf_query_cnt_enable;

     /* FlpQueryCntEnable: Enable counting FLP queries accessed      */
     /* to Bank A, B, C, D                                           */
     /* range: 7:4, access type: RW, default value: 0xf              */
     SOC_PB_PP_REG_FIELD flp_query_cnt_enable;

     /* EgqQueryCntEnable: Enable counting EGQ queries accessed      */
     /* to Bank A, B, C, D                                           */
     /* range: 11:8, access type: RW, default value: 0xf             */
     SOC_PB_PP_REG_FIELD egq_query_cnt_enable;

  } __ATTRIBUTE_PACKED__ query_cnt_enable_reg;

  /* Tcam Bist Wr:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e6 */

     /* Tmode: This signal indicates we are in BIST mode.            */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tmode;

     /* SmbistEn: This bit enables TCAM BIST                         */
     /* range: 4:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD smbist_en;

     /* SmbistMode: This signal is used to configure BIST into       */
     /* its running mode: 2'b00: No-Stop-On-Fail 2'b01:              */
     /* Stop-On-Fail 2'b10: Debug 2'b11: Unused                      */
     /* range: 6:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD smbist_mode;

     /* SmbistResume: This signal is used to resume BIST             */
     /* operation after a failure has occurred. Writing 1 to         */
     /* this bit in the configuration register should cause a        */
     /* pulse on this TCAM BIST signal with a length of 10           */
     /* cycles. Should be 0 by default.                              */
     /* range: 10:7, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD smbist_resume;

     /* SmbistTck: Serial clock used for shifting the debug          */
     /* data.                                                        */
     /* range: 14:11, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD smbist_tck;

     /* SmbistSe: BIST serial chain enable.                          */
     /* range: 18:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD smbist_se;

     /* SmbistSi: BIST serial chain input                            */
     /* range: 22:19, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD smbist_si;

  } __ATTRIBUTE_PACKED__ tcam_bist_wr_reg;

  /* Tcam Bist Re:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x65e7 */

     /* SmbistOn: Indicates that BIST is running.                    */
     /* range: 3:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD smbist_on;

     /* SmbistDone: Indicates that BIST is finished.                 */
     /* range: 7:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD smbist_done;

     /* SmbistFail: Indicates that BIST has failed.                  */
     /* range: 11:8, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD smbist_fail;

     /* SmbistSo: BIST serial chain output.                          */
     /* range: 15:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD smbist_so;

  } __ATTRIBUTE_PACKED__ tcam_bist_re_reg;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_REGS;
/* Block definition: EGQ  */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* Interrupt Register: EGQ interrupt register                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5800 */

     /* CudIsntOutVsiInt:                                            */
     /* range: 13:13, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD cud_isnt_out_vsi_int;

  } __ATTRIBUTE_PACKED__ interrupt_reg;

  /* Bypass System Vsi Em: If set, all lookups result will be       */
  /* 12 LSBs of the Key                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a50 */

     /* BypassSystemVsiEm: If set, all lookups result will be 12     */
     /* LSBs of the Key                                              */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD bypass_system_vsi_em;

  } __ATTRIBUTE_PACKED__ bypass_system_vsi_em_reg;

  /* Svem Lookup Arbiter Egress Lookup Counter: Counts Egress       */
  /* Lookups                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a51 */

     /* SvemLookupArbiterEgressLookupCounter: This register is       */
     /* clear on read.                                               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_lookup_arbiter_egress_lookup_counter;

     /* SvemLookupArbiterEgressLookupCounterOverflow:                */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_lookup_arbiter_egress_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_lookup_arbiter_egress_lookup_counter_reg;

  /* Svem Lookup Arbiter Large Em Lookup Counter: Counts            */
  /* Large EM Lookups                                               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a52 */

     /* SvemLookupArbiterLargeEmLookupCounter: This register is      */
     /* clear on read.                                               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_lookup_arbiter_large_em_lookup_counter;

     /* SvemLookupArbiterLargeEmLookupCounterOverflow:               */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_lookup_arbiter_large_em_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_lookup_arbiter_large_em_lookup_counter_reg;

  /* Svem Last Lookup Egress: Counts Large EM Lookups               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a53 */

     /* SvemLastLookupEgressKey:                                     */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_last_lookup_egress_key;

     /* SvemLastLookupEgressResult:                                  */
     /* range: 29:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_last_lookup_egress_result;

     /* SvemLastLookupEgressResultFound:                             */
     /* range: 30:30, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_last_lookup_egress_result_found;

  } __ATTRIBUTE_PACKED__ svem_last_lookup_egress_reg;

  /* Svem Last Lookup Large Em: Counts Large EM Lookups             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a54 */

     /* SvemLastLookupLargeEmKey:                                    */
     /* range: 15:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_last_lookup_large_em_key;

     /* SvemLastLookupLargeEmResult:                                 */
     /* range: 29:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_last_lookup_large_em_result;

     /* SvemLastLookupLargeEmResultFound:                            */
     /* range: 30:30, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_last_lookup_large_em_result_found;

  } __ATTRIBUTE_PACKED__ svem_last_lookup_large_em_reg;

  /* Svem Reset Status Register:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a5a */

     /* SvemKeytResetDone: If set, then the Key table has            */
     /* finished initialization                                      */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_keyt_reset_done;

  } __ATTRIBUTE_PACKED__ svem_reset_status_reg;

  /* Svem Management Unit Configuration Register:                   */
  /* Configuration for EM table management unit                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a60 */

     /* SvemMngmntUnitEnable: If set, then the management unit       */
     /* is enabled                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_mngmnt_unit_enable;

     /* SvemMngmntUnitActive: If set, then the management unit       */
     /* is currently active                                          */
     /* range: 4:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_mngmnt_unit_active;

  } __ATTRIBUTE_PACKED__ svem_management_unit_configuration_reg;

  /* Svem Management Unit Failure: Describes the first              */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a63 */

     /* SvemMngmntUnitFailureValid: If set, then the there is a      */
     /* management unit failure. All other fields in this            */
     /* register are valid only when this bit is set. This           */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_mngmnt_unit_failure_valid;

     /* SvemMngmntUnitFailureReason: Specifies reason for EMC        */
     /* management failure: 10 - Cam table full 09 - Table           */
     /* coherency 08 - Delete unknown key 07 - Reached max entry     */
     /* limit 06 - Inserted existing 05 - Learn request over         */
     /* static 04 - Learn over existing 03 - Change-fail non         */
     /* exist 02 - Change request over static 01 - Change            */
     /* non-exist from other 00 - Change non-exist from self         */
     /* range: 11:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_mngmnt_unit_failure_reason;

     /* SvemMngmntUnitFailureKey: Holds the Key that operation       */
     /* failed on.                                                   */
     /* range: 27:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ svem_management_unit_failure_reg;

  /* Svem Diagnostics: Diagnostics register                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a64 */

     /* SvemDiagnosticsLookup: When writing 1'b1 to this bit a       */
     /* lookup operation starts. When reading this bit as 1'b1 a     */
     /* lookup operation is ongoing.                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_diagnostics_lookup;

     /* SvemDiagnosticsRead: When writing 1'b1 to this bit a         */
     /* read operation starts. When reading this bit as 1'b1 a       */
     /* read operation is ongoing.                                   */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_diagnostics_read;

     /* SvemDiagnosticsReadAge: When writing 1'b1 to this bit a      */
     /* read-and-age operation starts. When reading this bit as      */
     /* 1'b1 a read-and-age operation is ongoing                     */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_diagnostics_read_age;

  } __ATTRIBUTE_PACKED__ svem_diagnostics_reg;

  /* Svem Diagnostics Index:                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a65 */

     /* SvemDiagnosticsIndex: Address to read from. Used             */
     /* together with read and read-and-age operations               */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_diagnostics_index;

  } __ATTRIBUTE_PACKED__ svem_diagnostics_index_reg;

  /* Svem Diagnostics Key:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a66 */

     /* SvemDiagnosticsKey: Key to lookup. Used together with        */
     /* lookup operation                                             */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_diagnostics_key;

  } __ATTRIBUTE_PACKED__ svem_diagnostics_key_reg;

  /* Svem Diagnostics Lookup Result: The result of the              */
  /* diagnostics operation                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a67 */

     /* SvemEntryFound: If set, then the entry looked up by the      */
     /* diagnostics command was found. Valid only for a lookup       */
     /* operation.                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_entry_found;

     /* SvemEntryPayload: The payload of the entry                   */
     /* range: 12:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_entry_payload;

     /* SvemEntryAgeStat: The Age status of the entry. For a         */
     /* read-and-age operation the age status before the aging       */
     /* is returned                                                  */
     /* range: 16:13, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_entry_age_stat;

  } __ATTRIBUTE_PACKED__ svem_diagnostics_lookup_result_reg;

  /* Svem Diagnostics Read Result: The result of the                */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a68 */

     /* SvemEntryValid: If set, then the index that was read         */
     /* contains a valid entry                                       */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_entry_valid;

     /* SvemEntryKey: The key of the entry read                      */
     /* range: 16:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_entry_key;

  } __ATTRIBUTE_PACKED__ svem_diagnostics_read_result_reg;

  /* Svem EMCDefrag Configuration Register: Configuration for       */
  /* EM table defragmentation machine.                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a69 */

     /* SvemDefragEnable: If set, then the defragmentation           */
     /* machine is active.                                           */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_defrag_enable;

     /* SvemDefragMode: The defragmentation mode. May be one of      */
     /* the following: 1'b0 - Periodical 1'b1 - Threshold            */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_defrag_mode;

     /* SvemDefragPeriod: The duration (in clocks) between           */
     /* defragmentation commands.                                    */
     /* range: 31:8, access type: RW, default value: 0x1000          */
     SOC_PB_PP_REG_FIELD svem_defrag_period;

  } __ATTRIBUTE_PACKED__ svem_emcdefrag_configuration_reg;

  /* Svem Requests Counter: Number of Requests processed by         */
  /* the MTM. This includes Defrag Requests that have been          */
  /* generated by the Defrag Machine                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a6a */

     /* SvemRequestsCounter: This register is clear on read.         */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_requests_counter;

     /* SvemRequestsCounterOverflow:                                 */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD svem_requests_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_requests_counter_reg;

  /* Svem Error Delete Unknown Key Counter: Number of times a       */
  /* Delete request of an Unknown Key was attempted                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a6c */

     /* SvemErrorDeleteUnknownKeyCounter: This register is clear     */
     /* on read.                                                     */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_delete_unknown_key_counter;

     /* SvemErrorDeleteUnknownKeyCounterOverflow:                    */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_delete_unknown_key_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_error_delete_unknown_key_counter_reg;

  /* Svem Error Reached Max Entry Limit Counter: Number of          */
  /* times an insertion failed because the entry limit was          */
  /* reached                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a6d */

     /* SvemErrorReachedMaxEntryLimitCounter: This register is       */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_reached_max_entry_limit_counter;

     /* SvemErrorReachedMaxEntryLimitCounterOverflow:                */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_reached_max_entry_limit_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_error_reached_max_entry_limit_counter_reg;

  /* Svem Error Cam Table Full Counter: Number of times an          */
  /* insertion failed because the CAM was full                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a6e */

     /* SvemErrorCamTableFullCounter: This register is clear on      */
     /* read.                                                        */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_cam_table_full_counter;

     /* SvemErrorCamTableFullCounterOverflow:                        */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_error_cam_table_full_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_error_cam_table_full_counter_reg;

  /* Svem Warning Inserted Existing Counter: Number of times        */
  /* an insert request of an existing entry was done.               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a6f */

     /* SvemWarningInsertedExistingCounter: This register is         */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_warning_inserted_existing_counter;

     /* SvemWarningInsertedExistingCounterOverflow:                  */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_warning_inserted_existing_counter_overflow;

  } __ATTRIBUTE_PACKED__ svem_warning_inserted_existing_counter_reg;

  /* Svem Entries Counter:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a70 */

     /* SvemEntriesCounter: Number of entries in the EM-DB           */
     /* range: 13:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD svem_entries_counter;

  } __ATTRIBUTE_PACKED__ svem_entries_counter_reg;

  /* Svem Cam Entries Counter:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x5a71 */

     /* SvemCamEntriesCounter: Number of entries in the              */
     /* auxiliary CAM table                                          */
     /* range: 5:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD svem_cam_entries_counter;

  } __ATTRIBUTE_PACKED__ svem_cam_entries_counter_reg;

  /* EHPGeneral Settings: EHP general settings.                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x588d */

     /* DisablePktFilter: If set, then disables the deny of          */
     /* packets for all filter logic                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD disable_pkt_filter;

     /* DisableSamePortFilter: If set, then disables the deny of     */
     /* packets from the same port.                                  */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD disable_same_port_filter;

     /* TmRawPortLearnDisable: PortLearnDisable                      */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tm_raw_port_learn_disable;

     /* CpuPortLearnDisable: PortLearnDisable                        */
     /* range: 3:3, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD cpu_port_learn_disable;

     /* CustomLearn: If set, enables learning when FWD_CODE is       */
     /* not Ethernet and all learning conditions are met.            */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD custom_learn;

     /* CfgDot1Ad: Indicates whether the Egress Packet Processor     */
     /* editor works in Q-in-Q mode, according to Dot1AD             */
     /* protocol. If set, configures the packet editing to work      */
     /* in Q-in-Q mode                                               */
     /* range: 5:5, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD cfg_dot1_ad;

     /* AdvaAclArpEn:                                                */
     /* range: 6:6, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ad_acl_arp_en;

     /* ErrorCodeMvr:                                                */
     /* range: 10:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD error_code_mvr;

     /* AuxiliaryDataTableMode: This register determines the         */
     /* Auxiliary data table mode. There are 2 modes: 0 -            */
     /* MODE_IS_PVLAN 1 - Split-Horizon 2 - Provider Backbone        */
     /* Bridging                                                     */
     /* range: 13:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD auxiliary_data_table_mode;

     /* IpMcTranslateVsi: If set, then IPv4/IPv6 multicast           */
     /* packet use translated VSI. This register is used in          */
     /* T-VSI resolution logic.                                      */
     /* range: 14:14, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD ip_mc_translate_vsi;

     /* NwkOffsetAdd: Network header offset from end of EPPH         */
     /* header.                                                      */
     /* range: 15:15, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD nwk_offset_add;

     /* SplitHorizonFilter:                                          */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD split_horizon_filter;

     /* DisableBounceBackEn:                                         */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD disable_bounce_back_en;

     /* BounceBackThreshold:                                         */
     /* range: 27:21, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD bounce_back_threshold;

  } __ATTRIBUTE_PACKED__ ehpgeneral_settings_reg;

  /* Deny Packet Counter: Filtered Packet counter                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x588e */

     /* DenyPktCnt: If set, then disables the deny of packets.       */
     /* range: 30:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD deny_pkt_cnt;

     /* DenyPktCntOvf: If set, then indicates that the counter       */
     /* overflowed.                                                  */
     /* range: 31:31, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD deny_pkt_cnt_ovf;

  } __ATTRIBUTE_PACKED__ deny_packet_counter_reg;

  /* Eep Tunnel Range: EEP Tunnel Range Register                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589a */

     /* EepTunnelRangeBottom: Determines the bottom value of the     */
     /* EEP tunnel range                                             */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD eep_tunnel_range_bottom;

  } __ATTRIBUTE_PACKED__ eep_tunnel_range_reg;

  /* Outlif Error Code:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589b */

     /* OutlifErrorCode:                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD outlif_error_code;

  } __ATTRIBUTE_PACKED__ outlif_error_code_reg_0;

  /* Outlif Error Code:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589c */

     /* OutlifErrorCode:                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD outlif_error_code;

  } __ATTRIBUTE_PACKED__ outlif_error_code_reg_1;

  /* Outlif Error Code:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589d */

     /* OutlifErrorCode:                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD outlif_error_code;

  } __ATTRIBUTE_PACKED__ outlif_error_code_reg_2;

  /* Trill Config:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589e */

     /* TpidProfileTrill:                                            */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tpid_profile_trill;

     /* TrillDefaultVid:                                             */
     /* range: 15:4, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD trill_default_vid;

  } __ATTRIBUTE_PACKED__ trill_config_reg;

  /* Ethernet Types Reg0: User Defined Ethernet Types               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x589f */

     /* EthType0:                                                    */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD eth_type[SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_FLDS];

  } __ATTRIBUTE_PACKED__ ethernet_types_reg[SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_REGS];

  /* Action Profile1: Action Profile Register1                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58a4 */

     /* ActionProfileNoVsiTranslation: Used when no VSI              */
     /* translation is valid                                         */
     /* range: 7:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_no_vsi_translation;

     /* ActionProfileDssStacking: Used when Dss-Stacking deny        */
     /* range: 11:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD action_profile_dss_stacking;

     /* ActionProfileLagMulticast: Used when LAG multicast deny      */
     /* range: 15:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_lag_multicast;

     /* ActionProfileExcludeSrc: Used when exclude source deny       */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_exclude_src;

     /* ActionProfileVlanMembership: Used when VLAN membership       */
     /* deny                                                         */
     /* range: 23:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_vlan_membership;

     /* ActionProfileUnacceptableFrameType: Used when acceptable     */
     /* frame type deny                                              */
     /* range: 27:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_unacceptable_frame_type;

     /* ActionProfileIllegalEep: Used when eep is higher than        */
     /* 12K                                                          */
     /* range: 31:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_illegal_eep;

  } __ATTRIBUTE_PACKED__ action_profile1_reg;

  /* Action Profile2: Action Profile Register2                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58a5 */

     /* ActionProfileSrcEqualDest: Used when source equal            */
     /* destination deny                                             */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_src_equal_dest;

     /* ActionProfileUnknownDa: Used when unknown DA deny            */
     /* range: 7:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_unknown_da;

     /* ActionProfileSplitHorizon: Used when split horizon deny      */
     /* range: 11:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD action_profile_split_horizon;

     /* ActionProfilePrivateVlan: Used when private-VLAn deny        */
     /* range: 15:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_private_vlan;

     /* ActionProfileTtlScope: Used when TTL scoping deny            */
     /* range: 19:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_ttl_scope;

     /* ActionProfileMtuViolation: Used when MTU violation deny      */
     /* range: 23:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_mtu_violation;

     /* ActionProfileTrillTtlZero: Used when TRILL TTL deny          */
     /* range: 27:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_trill_ttl_zero;

     /* ActionProfileTrillSameInterface: Used when TRILL Same        */
     /* Interface deny                                               */
     /* range: 31:28, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD action_profile_trill_same_interface;

  } __ATTRIBUTE_PACKED__ action_profile2_reg;

  /* Action Profile3: Action Profile Register3                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58a6 */

     /* ActionProfileBounceBack:                                     */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD action_profile_bounce_back;

  } __ATTRIBUTE_PACKED__ action_profile3_reg;

  /* Acceptable Frame Type Table: Acceptable frame type             */
  /* table. Used for Acceptable frame type filtering                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58a7 */

     /* AcceptableFrameTypeTable: Index[6:0] is                      */
     /* \{ethernet_tag_format[4:0],                                  */
     /* pp_port.acceptable_frame_type_profile[1:0 ]\}                */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD acceptable_frame_type_table;

  } __ATTRIBUTE_PACKED__ acceptable_frame_type_reg[SOC_PB_PP_EGQ_ACCEPTABLE_FRAME_TYPE_TABLE_REG_MULT_NOF_REGS];

  /* Private Vlan Filter: Private VLAN table. This table used        */
  /* for private VLAN filtering                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58ab */

     /* PrivateVlanFilter: The index of the table is                 */
     /* \{Src-Port-Type, Dst-Port-Type\}. The output is 1-bit,       */
     /* indicating whether to filter the packet or not.              */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD private_vlan_filter;

  } __ATTRIBUTE_PACKED__ private_vlan_filter_reg;

  /* Ttl Scope: TTL                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58ac */

     /* TtlScope: Specifies which combination of incoming and        */
     /* outgoing orientations should be filtered.                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ttl_scope;

  } __ATTRIBUTE_PACKED__ ttl_scope_reg_0;

  /* Ttl Scope: TTL                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58ad */

     /* TtlScope: Specifies which combination of incoming and        */
     /* outgoing orientations should be filtered.                    */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ttl_scope;

  } __ATTRIBUTE_PACKED__ ttl_scope_reg_1;

  /* Erpp Debug Configuration:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58b6 */

     /* DebugSelect:                                                 */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD debug_select;

     /* ErppErrorCode:                                               */
     /* range: 6:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD erpp_error_code;

  } __ATTRIBUTE_PACKED__ erpp_debug_configuration_reg;

  /* ACL General Configuration: TBD                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58b7 */

     /* EnableActionProfile: TBD                                     */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_action_profile;

     /* EnableOutTM_Port: TBD                                        */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_out_tm_port;

     /* EnableCUD_Action: TBD                                        */
     /* range: 8:8, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_cud_action;

     /* EnableTC_Action: TBD                                         */
     /* range: 12:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD enable_tc_action;

     /* EnableDP_Action: TBD                                         */
     /* range: 16:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD enable_dp_action;

     /* AclDp0: TBD                                                  */
     /* range: 21:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD acl_dp[2];

     /* HighPriorityAcl: TBD                                         */
     /* range: 24:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD high_priority_acl;

  } __ATTRIBUTE_PACKED__ acl_general_configuration_reg;

  /* ACL TCAM Access Enabler: TBD                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58b8 */

     /* EnableTCAM_BankA_Access: TBD                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD enable_tcam_bank_access[SOC_PB_PP_REGS_TCAM_NOF_BANKS];

  } __ATTRIBUTE_PACKED__ acl_tcam_access_enabler_reg;

  /* L4 Protocol Code Reg0: TBD                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x58b9 */

     /* L4ProtocolCode0: TBD                                         */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD l4_protocol_code[PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_FLDS];

  } __ATTRIBUTE_PACKED__ l4_protocol_code_reg[PP_PB_NOF_EGQ_L4_PROTOCOL_CODE_REGS];


} __ATTRIBUTE_PACKED__ SOC_PB_PP_EGQ_REGS;
/* Block definition: EPNI  */
typedef struct
{
  uint32   nof_instances; /* 1 */
  SOC_PB_PP_REG_ADDR addr;

  /* Esem Reset Status Register:                                    */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b0a */

     /* EsemKeytResetDone: If set, then the Key table has            */
     /* finished initialization                                      */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_keyt_reset_done;

  } __ATTRIBUTE_PACKED__ esem_reset_status_reg;

  /* Esem Management Unit Configuration Register:                   */
  /* Configuration for EM table management unit                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b10 */

     /* EsemMngmntUnitEnable: If set, then the management unit       */
     /* is enabled                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_enable;

     /* EsemMngmntUnitActive: If set, then the management unit       */
     /* is currently active                                          */
     /* range: 4:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_active;

  } __ATTRIBUTE_PACKED__ esem_management_unit_configuration_reg;

  /* Esem Management Unit Failure: Describes the first              */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b13 */

     /* EsemMngmntUnitFailureValid: If set, then the there is a      */
     /* management unit failure. All other fields in this            */
     /* register are valid only when this bit is set. This           */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_failure_valid;

     /* EsemMngmntUnitFailureReason: Specifies reason for EMC        */
     /* management failure: 10 - Cam table full 09 - Table           */
     /* coherency 08 - Delete unknown key 07 - Reached max entry     */
     /* limit 06 - Inserted existing 05 - Learn request over         */
     /* static 04 - Learn over existing 03 - Change-fail non         */
     /* exist 02 - Change request over static 01 - Change            */
     /* non-exist from other 00 - Change non-exist from self         */
     /* range: 11:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_failure_reason;

     /* EsemMngmntUnitFailureKey: Holds the Key that operation       */
     /* failed on.                                                   */
     /* range: 31:12, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ esem_management_unit_failure_reg_0;

  /* Esem Management Unit Failure: Describes the first              */
  /* management unit failure (since the last time this              */
  /* register was read). The register is locked after a             */
  /* failure. It is unlocked when the first part (i.e. the          */
  /* lsb) of the register is read.                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b14 */

     /* EsemMngmntUnitFailureKey: Holds the Key that operation       */
     /* failed on.                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_mngmnt_unit_failure_key;

  } __ATTRIBUTE_PACKED__ esem_management_unit_failure_reg_1;

  /* Esem Diagnostics: Diagnostics register                         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b15 */

     /* EsemDiagnosticsLookup: When writing 1'b1 to this bit a       */
     /* lookup operation starts. When reading this bit as 1'b1 a     */
     /* lookup operation is ongoing.                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_diagnostics_lookup;

     /* EsemDiagnosticsRead: When writing 1'b1 to this bit a         */
     /* read operation starts. When reading this bit as 1'b1 a       */
     /* read operation is ongoing.                                   */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_diagnostics_read;

     /* EsemDiagnosticsReadAge: When writing 1'b1 to this bit a      */
     /* read-and-age operation starts. When reading this bit as      */
     /* 1'b1 a read-and-age operation is ongoing                     */
     /* range: 2:2, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_diagnostics_read_age;

  } __ATTRIBUTE_PACKED__ esem_diagnostics_reg;

  /* Esem Diagnostics Index:                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b16 */

     /* EsemDiagnosticsIndex: Address to read from. Used             */
     /* together with read and read-and-age operations               */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_diagnostics_index;

  } __ATTRIBUTE_PACKED__ esem_diagnostics_index_reg;

  /* Esem Diagnostics Key:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b17 */

     /* EsemDiagnosticsKey: Key to lookup. Used together with        */
     /* lookup operation                                             */
     /* range: 20:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_diagnostics_key;

  } __ATTRIBUTE_PACKED__ esem_diagnostics_key_reg;

  /* Esem Diagnostics Lookup Result: The result of the              */
  /* diagnostics operation                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b18 */

     /* EsemEntryFound: If set, then the entry looked up by the      */
     /* diagnostics command was found. Valid only for a lookup       */
     /* operation.                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_entry_found;

     /* EsemEntryPayload: The payload of the entry                   */
     /* range: 15:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_entry_payload;

     /* EsemEntryAgeStat: The Age status of the entry. For a         */
     /* read-and-age operation the age status before the aging       */
     /* is returned                                                  */
     /* range: 19:16, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD esem_entry_age_stat;

  } __ATTRIBUTE_PACKED__ esem_diagnostics_lookup_result_reg;

  /* Esem Diagnostics Read Result: The result of the                */
  /* Read/ReadAge operation.                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b19 */

     /* EsemEntryValid: If set, then the index that was read         */
     /* contains a valid entry                                       */
     /* range: 0:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_entry_valid;

     /* EsemEntryKey: The key of the entry read                      */
     /* range: 21:1, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_entry_key;

  } __ATTRIBUTE_PACKED__ esem_diagnostics_read_result_reg;

  /* Esem EMCDefrag Configuration Register: Configuration for       */
  /* EM table defragmentation machine.                              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b1a */

     /* EsemDefragEnable: If set, then the defragmentation           */
     /* machine is active.                                           */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_defrag_enable;

     /* EsemDefragMode: The defragmentation mode. May be one of      */
     /* the following: 1'b0 - Periodical 1'b1 - Threshold            */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_defrag_mode;

     /* EsemDefragPeriod: The duration (in clocks) between           */
     /* defragmentation commands.                                    */
     /* range: 31:8, access type: RW, default value: 0x1000          */
     SOC_PB_PP_REG_FIELD esem_defrag_period;

  } __ATTRIBUTE_PACKED__ esem_emcdefrag_configuration_reg;

  /* Esem Requests Counter: Number of Requests processed by         */
  /* the MTM. This includes Defrag Requests that have been          */
  /* generated by the Defrag Machine                                */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b1b */

     /* EsemRequestsCounter: This register is clear on read.         */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_requests_counter;

     /* EsemRequestsCounterOverflow:                                 */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD esem_requests_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_requests_counter_reg;

  /* Esem Error Delete Unknown Key Counter: Number of times a       */
  /* Delete request of an Unknown Key was attempted                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b1d */

     /* EsemErrorDeleteUnknownKeyCounter: This register is clear     */
     /* on read.                                                     */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_delete_unknown_key_counter;

     /* EsemErrorDeleteUnknownKeyCounterOverflow:                    */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_delete_unknown_key_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_error_delete_unknown_key_counter_reg;

  /* Esem Error Reached Max Entry Limit Counter: Number of          */
  /* times an insertion failed because the entry limit was          */
  /* reached                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b1e */

     /* EsemErrorReachedMaxEntryLimitCounter: This register is       */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_reached_max_entry_limit_counter;

     /* EsemErrorReachedMaxEntryLimitCounterOverflow:                */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_reached_max_entry_limit_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_error_reached_max_entry_limit_counter_reg;

  /* Esem Error Cam Table Full Counter: Number of times an          */
  /* insertion failed because the CAM was full                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b1f */

     /* EsemErrorCamTableFullCounter: This register is clear on      */
     /* read.                                                        */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_cam_table_full_counter;

     /* EsemErrorCamTableFullCounterOverflow:                        */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_error_cam_table_full_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_error_cam_table_full_counter_reg;

  /* Esem Warning Inserted Existing Counter: Number of times        */
  /* an insert request of an existing entry was done.               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b20 */

     /* EsemWarningInsertedExistingCounter: This register is         */
     /* clear on read.                                               */
     /* range: 6:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_warning_inserted_existing_counter;

     /* EsemWarningInsertedExistingCounterOverflow:                  */
     /* range: 7:7, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_warning_inserted_existing_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_warning_inserted_existing_counter_reg;

  /* Esem Entries Counter:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b21 */

     /* EsemEntriesCounter: Number of entries in the EM-DB           */
     /* range: 13:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_entries_counter;

  } __ATTRIBUTE_PACKED__ esem_entries_counter_reg;

  /* Esem Cam Entries Counter:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b22 */

     /* EsemCamEntriesCounter: Number of entries in the              */
     /* auxiliary CAM table                                          */
     /* range: 5:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_cam_entries_counter;

  } __ATTRIBUTE_PACKED__ esem_cam_entries_counter_reg;

  /* Esem Lookup Arbiter Lookup Counter: Counts Large EM            */
  /* Lookups                                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b00 */

     /* EsemLookupArbiterLookupCounter: This register is clear       */
     /* on read.                                                     */
     /* range: 30:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_lookup_arbiter_lookup_counter;

     /* EsemLookupArbiterLookupCounterOverflow:                      */
     /* range: 31:31, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD esem_lookup_arbiter_lookup_counter_overflow;

  } __ATTRIBUTE_PACKED__ esem_lookup_arbiter_lookup_counter_reg;

  /* Esem Last Lookup: Counts Large EM Lookups                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b01 */

     /* EsemLastLookupKey:                                           */
     /* range: 20:0, access type: RO, default value: 0x0             */
     SOC_PB_PP_REG_FIELD esem_last_lookup_key;

     /* EsemLastLookupResult:                                        */
     /* range: 31:21, access type: RO, default value: 0x0            */
     SOC_PB_PP_REG_FIELD esem_last_lookup_result;

  } __ATTRIBUTE_PACKED__ esem_last_lookup_reg_0;

  /* Esem Last Lookup: Counts Large EM Lookups                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3b02 */

     /* EsemLastLookupResult:                                        */
     /* range: 3:0, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_last_lookup_result;

     /* EsemLastLookupResultFound:                                   */
     /* range: 4:4, access type: RO, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_last_lookup_result_found;

  } __ATTRIBUTE_PACKED__ esem_last_lookup_reg_1;

  /* Esem Int Reg:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a02 */

     /* EsemManagementCompleted : Asserts when reply fifo           */
     /* is not empty                                                */
     /* range: 6:6, access type: R/CL, default value: 0x0           */
     SOC_PB_PP_REG_FIELD esem_management_completed;

  } __ATTRIBUTE_PACKED__ esem_int_reg;

  /* Esem Int Reg Mask:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a12 */

     /* EsemIntRegMask: Writing 0 masks the corresponding            */
     /* interrupt source                                             */
     /* range: 6:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD esem_int_reg_mask;

  } __ATTRIBUTE_PACKED__ esem_int_reg_mask_reg;

  /* General Pp Config:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a79 */

     /* MvrEnable:                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD mvr_enable;

     /* P2pVsi: Peer to Peer VSI                                     */
     /* range: 14:1, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD p2p_vsi;

     /* TpidProfileTrill: The TPID profile after Trill header        */
     /* termination, used for parsing the Ethernet header above      */
     /* the terminated Trill header.                                 */
     /* range: 18:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD tpid_profile_trill;

     /* NwkOffsetAdd: If set then the first network header is        */
     /* four bytes after the system header stuck; else it is         */
     /* immediately after the system header stuck.                   */
     /* range: 20:20, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD nwk_offset_add;

     /* AuxiliaryDataTableMode: This register determines the         */
     /* Auxiliary data table mode. There are 3 modes: 0 -            */
     /* Private VLAN 1 - Split Horizon 2 - Provider Backbone         */
     /* Bridging                                                     */
     /* range: 25:24, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD auxiliary_data_table_mode;

  } __ATTRIBUTE_PACKED__ general_pp_config_reg;

  /* Eep Limit: The type of each entry in the encapsulation         */
  /* database is determined by its position in the database,        */
  /* i.e., by the EEP (Egress-Encapsulation-Pointer) as             */
  /* follows: If EEP is NULL, then it is invalid, Else, if        */
  /* EEP is less than or equal to EEP-Link-Layer-Limit, then        */
  /* EEP is link layer, Else, if EEP is less than or equal         */
  /* to EEP-IP-Tunnel-List, then EEP is IP Tunnel, Else EEP        */
  /* is MPLS-Tunnel (or PWE)                                        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a7a */

     /* EepLinkLayerLimit: EEP-Link-Layer-Limit is used to           */
     /* determine the type of the entry in the encapsulation         */
     /* database.                                                    */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD eep_link_layer_limit;

     /* EepIpTunnelLimit: EEP-IP-Tunnel-Limit is used to             */
     /* determine the type of the entry in the encapsulation         */
     /* database.                                                    */
     /* range: 29:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD eep_ip_tunnel_limit;

  } __ATTRIBUTE_PACKED__ eep_limit_reg;

  /* Ipv4 Exp To Tos Map:                                           */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a7b */
                           /* 0x3a7c */
                           /* 0x3a7d */
                           /* 0x3a7e */
                           /* 0x3a7f */
                           /* 0x3a80 */
                           /* 0x3a81 */
                           /* 0x3a82 */
     /* Ipv4ExpToTosMap: When the packet is PHP and the MPLS         */
     /* Command is POP-Into-IPv4-Uniform, then mapping is            */
     /* \{PP-Port. EXP-MAP-PROFILE(2),MFF. MPLS-EXP(3)\} to            */
     /* IPV4. TOS(8)                                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_exp_to_tos_map;

  } __ATTRIBUTE_PACKED__ ipv4_exp_to_tos_map_reg[PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS];


  /* Ipv6 Exp To Tc Map:                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a83 */
                           /* 0x3a84 */
                           /* 0x3a85 */
                           /* 0x3a86 */
                           /* 0x3a87 */
                           /* 0x3a88 */
                           /* 0x3a89 */
                           /* 0x3a8a */
     /* Ipv6ExpToTcMap: When the packet is PHP and the MPLS          */
     /* Command is POP-Into-IPv6-Uniform, then mapping is            */
     /* \{PP-Port. EXP-MAP-PROFILE(2),MFF. MPLS-EXP(3)\} to            */
     /* IPV6. TC(8)                                                   */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv6_exp_to_tc_map;

  } __ATTRIBUTE_PACKED__ ipv6_exp_to_tc_map_reg[PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS];


  /* Trill Config: Encapsulated Trill header configurations         */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a8c */

     /* TrillNickname: The encapsulated Trill-Header. Nickname        */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD trill_nickname;

     /* TrillOpCnt: The encapsulated Trill-Header. OpCnt              */
     /* range: 21:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD trill_op_cnt;

  } __ATTRIBUTE_PACKED__ trill_config_reg;

  /* Mpls Profile:                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a8d */

     /* MplsProfile: Mapping The MPLS push command(8) to MPLS        */
     /* profile. The MPLS profile is encoded as follows: 0 -         */
     /* Pipe 8:1 - TTL 11:9- EXP 12- CW (control word) If Pipe       */
     /* is set then, TTL-Model is Pipe; else TTL-Model is            */
     /* Uniform. If TTL-Model is Pipe, then                          */
     /* Encapsulated-MPLS. TTL = TTL; else Encapsulated-MPLS. TTL      */
     /* = In-TTL If TTL-Model is Pipe, then                          */
     /* Encapsulated-MPLS. EXP = EXP; else Encapsulated-MPLS. EXP      */
     /* = In-EXP If CW is set, then control word is added to the     */
     /* packet header. (4 Bytes)                                     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_profile;

  } __ATTRIBUTE_PACKED__ mpls_profile_reg[SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS];

  /* Mpls Exp Map Table:                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a91 */

     /* MplsExpMapTable:                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_exp_map_table;

  } __ATTRIBUTE_PACKED__ mpls_exp_map_table_reg[SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS];

  /* Ip Dscp To Exp Map:                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a93 */

     /* IpDscpToExpMap:                                              */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ip_dscp_to_exp_map;

  } __ATTRIBUTE_PACKED__ ip_dscp_to_exp_map_reg[SOC_PB_PP_EPNI_IP_DSCP_TO_EXP_MAP_REG_MULT_NOF_REGS];

  /* Ipv4 Tos:                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a99 */

     /* Ipv4Tos: If IPv4 tunnel is encapsulated, then mapping        */
     /* the Encapsulation-Entry. TosPointer(4) to                     */
     /* IPv4-Tunnel. TOS(8).                                          */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_tos;

  } __ATTRIBUTE_PACKED__ ipv4_tos_reg[SOC_PB_PP_EPNI_IPV4_TOS_REG_MULT_NOF_REGS];

  /* Ipv4 Ttl:                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3a9d */

     /* Ipv4Ttl: If IPv4 tunnel is encapsulated, then mapping is     */
     /* Encapsulation-Entry. TTLPointer(4) to IPv4. TTL(8).            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_ttl;

  } __ATTRIBUTE_PACKED__ ipv4_ttl_reg[SOC_PB_PP_EPNI_IPV4_TTL_REG_MULT_NOF_REGS];

  /* Ipv4 Sip:                                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3aa1 */

     /* Ipv4Sip: If IPv4 tunnel is encapsulated, then mapping is     */
     /* Encapsulation-Entry. SourceAddrPointer(4) to                  */
     /* IPv4. SourceAddr(32).                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ipv4_sip;

  } __ATTRIBUTE_PACKED__ ipv4_sip_reg[SOC_PB_PP_EPNI_IPV4_SIP_REG_MULT_NOF_REGS];

  /* Sa Msb: Configures the SA device for packet header             */
  /* manipulation.                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab1 */

     /* SaMsb: SA[47-6] for the constructed Ethernet link layer.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sa_msb;

  } __ATTRIBUTE_PACKED__ sa_msb_reg_0;

  /* Sa Msb: Configures the SA device for packet header             */
  /* manipulation.                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab2 */

     /* SaMsb: SA[47-6] for the constructed Ethernet link layer.     */
     /* range: 9:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD sa_msb;

  } __ATTRIBUTE_PACKED__ sa_msb_reg_1;

  /* Bsa: Backbone Mac tunnel source address configuration          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab3 */

     /* Bsa: If (SourceSystemPort < 2K) then, Ethernet-BSA is        */
     /* set to \{Bsa[47:8],Aux -Table-Bsa[7:0]\} else                */
     /* Ethernet-BSA is set to Bsa[47:0].                            */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD bsa;

  } __ATTRIBUTE_PACKED__ bsa_reg_0;

  /* Bsa: Backbone Mac tunnel source address configuration          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab4 */

     /* Bsa: If (SourceSystemPort < 2K) then, Ethernet-BSA is        */
     /* set to \{Bsa[47:8],Aux -Table-Bsa[7:0]\} else                */
     /* Ethernet-BSA is set to Bsa[47:0].                            */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD bsa;

  } __ATTRIBUTE_PACKED__ bsa_reg_1;

  /* Itag Tc Dp Map: Backbone Mac tunnel I-Tag. PCP-DEI              */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab5 */

     /* ItagTcDpMap: Mapping \{system-DP(2) and system-TC(3)\}       */
     /* to I-Tag. PCP-DEI(4).                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD itag_tc_dp_map;

  } __ATTRIBUTE_PACKED__ itag_tc_dp_map_regs[4];

  /* Itag Tc Dp Map: Backbone Mac tunnel I-Tag. PCP-DEI              */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab6 */

     /* ItagTcDpMap: Mapping \{system-DP(2) and system-TC(3)\}       */
     /* to I-Tag. PCP-DEI(4).                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD itag_tc_dp_map;

  } __ATTRIBUTE_PACKED__ itag_tc_dp_map_reg_1;

  /* Itag Tc Dp Map: Backbone Mac tunnel I-Tag. PCP-DEI              */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab7 */

     /* ItagTcDpMap: Mapping \{system-DP(2) and system-TC(3)\}       */
     /* to I-Tag. PCP-DEI(4).                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD itag_tc_dp_map;

  } __ATTRIBUTE_PACKED__ itag_tc_dp_map_reg_2;

  /* Itag Tc Dp Map: Backbone Mac tunnel I-Tag. PCP-DEI              */
  /* configuration                                                  */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab8 */

     /* ItagTcDpMap: Mapping \{system-DP(2) and system-TC(3)\}       */
     /* to I-Tag. PCP-DEI(4).                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD itag_tc_dp_map;

  } __ATTRIBUTE_PACKED__ itag_tc_dp_map_reg_3;

  /* Tpid Profile Mac In Mac: Backbone Mac tunnel                   */
  /* Tpid-Profile configuration                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ab9 */

     /* TpidProfileMacInMac: TpidProfile for the encapsulated        */
     /* MAC-IN-MAC link layer                                        */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tpid_profile_mac_in_mac;

  } __ATTRIBUTE_PACKED__ tpid_profile_mac_in_mac_reg;

  /* Tpid Profile System: TPID profile for the encapsulated         */
  /* link layer                                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3aba */

     /* TpidProfileSystem: TPID profile for the encapsulated         */
     /* link layer                                                   */
     /* range: 2:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD tpid_profile_system;

  } __ATTRIBUTE_PACKED__ tpid_profile_system_reg;

  /* Acceptable Frame Type: Mapping                                 */
  /* \{LLVP-Incoming-Tag-Format(4),PP-Port. Profile(2)\} to          */
  /* one bit AcceptableFrameType.                                   */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3abb */

     /* AcceptableFrameType: If AcceptableFrameType is clear,        */
     /* then the packet is dropped.                                  */
     /* range: 31:0, access type: RW, default value: 64'hffffffffffffffff */
     SOC_PB_PP_REG_FIELD acceptable_frame_type;

  } __ATTRIBUTE_PACKED__ acceptable_frame_type_reg[SOC_PB_PP_EPNI_ACCEPTABLE_FRAME_TYPE_REG_MULT_NOF_REGS];

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3abd */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_0;

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3abe */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_1;

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3abf */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_2;

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac0 */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_3;

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac1 */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_4;

  /* Sem Opcode Offset: Mapping \{SEM-Opcode(1),                    */
  /* Traffic-Class(3), Drop-Precedence(2)\} to SEM-Offset(3)        */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac2 */

     /* SemOpcodeOffset: The SEM-Offset(3) is added to the           */
     /* Small-EM-Result-Pointer in order to support different AC     */
     /* according to TM parameters.                                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD sem_opcode_offset;

  } __ATTRIBUTE_PACKED__ sem_opcode_offset_reg_5;

  /* Default Sem Index: The SEM-Result-Pointer is set to the        */
  /* DefaultSemIndex if the encapsulated link layer is built        */
  /* with VID taken from the Encapsulation data-base.               */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac3 */

     /* DefaultSemIndex: This is either a Trill port where we        */
     /* add the designated VLAN from the link layer or, a PBP        */
     /* where we add the B-Tag. Entry in Small-EM-Result table       */
     /* should be configured to add this VID                         */
     /* range: 13:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD default_sem_index;

  } __ATTRIBUTE_PACKED__ default_sem_index_reg;

  /* Mirror Vid Reg0: VIDs for mirroring comparison                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac4 */

     /* MirrorVid0: VID-0 for mirroring comparison                   */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mirror_vid0;

     /* MirrorVid1: VID-1 for mirroring comparison                   */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mirror_vid1;

  } __ATTRIBUTE_PACKED__ mirror_vid_reg0_reg;

  /* Mirror Vid Reg1: VIDs for mirroring comparison                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac5 */

     /* MirrorVid2: VID-2 for mirroring comparison                   */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mirror_vid2;

     /* MirrorVid3: VID-3 for mirroring comparison                   */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mirror_vid3;

  } __ATTRIBUTE_PACKED__ mirror_vid_reg1_reg;

  /* Mirror Vid Reg2: VIDs for mirroring comparison                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac6 */

     /* MirrorVid4: VID-4 for mirroring comparison                   */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mirror_vid4;

     /* MirrorVid5: VID-5 for mirroring comparison                   */
     /* range: 23:12, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD mirror_vid5;

  } __ATTRIBUTE_PACKED__ mirror_vid_reg2_reg;

  /* Mirror Vid Reg3: VIDs for mirroring comparison                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac7 */

     /* MirrorVid6: VID-6 for mirroring comparison                   */
     /* range: 11:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mirror_vid6;

  } __ATTRIBUTE_PACKED__ mirror_vid_reg3_reg;

  /* Counter Base Reg0: Counter Processor Base offset.              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac8 */

     /* CounterBaseTm: A counter base for TM mode. In this mode,     */
     /* the counter pointer is calculated as follows:                */
     /* CounterPointer = CounterBaseTm(16) +                         */
     /* \{4'd0,Egress-Multicast,System-Multicast,Out-TM-Port(7),Traffic-Class(3)\}  */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_base_tm;

     /* CounterBaseVsi: A counter base for VSI mode. In this         */
     /* mode, the counter pointer is calculated as follows:          */
     /* CounterPointer = CounterBaseVsi(16) +                        */
     /* \{2'd0,Out-Vsi(14)\}                                         */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD counter_base_vsi;

  } __ATTRIBUTE_PACKED__ counter_base_reg0_reg;

  /* Counter Base Reg1: Counter Processor Base offset.              */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ac9 */

     /* CounterBaseOutlif: A counter base for Outlif mode. In        */
     /* this mode, the counter pointer is calculated as follows:     */
     /* CounterPointer = CounterBaseOutlif(16) + Outlif(16)\}        */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_base_outlif;

  } __ATTRIBUTE_PACKED__ counter_base_reg1_reg;

  /* Counter Range Low Reg0: Counter pointer is tested to be        */
  /* in range                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3aca */

     /* CounterRangeLowTm: Lower limit for counter pointer in TM     */
     /* mode                                                         */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_range_low_tm;

     /* CounterRangeLowVsi: Lower limit for counter pointer in       */
     /* VSI mode                                                     */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD counter_range_low_vsi;

  } __ATTRIBUTE_PACKED__ counter_range_low_reg0_reg;

  /* Counter Range Low Reg1: Counter pointer is tested to be        */
  /* in range                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3acb */

     /* CounterRangeLowOutlif: Lower limit for counter pointer       */
     /* in Outlif mode                                               */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_range_low_outlif;

  } __ATTRIBUTE_PACKED__ counter_range_low_reg1_reg;

  /* Counter Range High Reg0: Counter pointer is tested to be       */
  /* in range                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3acc */

     /* CounterRangeHighTm: Upper limit for counter pointer in       */
     /* TM mode                                                      */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_range_high_tm;

     /* CounterRangeHighVsi: Upper limit for counter pointer in      */
     /* VSI mode                                                     */
     /* range: 31:16, access type: RW, default value: 0x0            */
     SOC_PB_PP_REG_FIELD counter_range_high_vsi;

  } __ATTRIBUTE_PACKED__ counter_range_high_reg0_reg;

  /* Counter Range High Reg1: Counter pointer is tested to be       */
  /* in range                                                       */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3acd */

     /* CounterRangeHighOutlif: Upper limit for counter pointer      */
     /* in Outlif mode                                               */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD counter_range_high_outlif;

  } __ATTRIBUTE_PACKED__ counter_range_high_reg1_reg;

  /* Counter Dp Map: Counter DP                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ace */

     /* CounterDpMap: Mapping System-DP(2) to Counter-DP(2).         */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD counter_dp_map;

  } __ATTRIBUTE_PACKED__ counter_dp_map_reg;

  /* Ttl Decrement Enable:                                          */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae0 */

     /* TtlDecrementEnable:                                          */
     /* range: 0:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ttl_decrement_enable;

     /* TtlTunnelDisable:                                            */
     /* range: 1:1, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ttl_tunnel_disable;

  } __ATTRIBUTE_PACKED__ ttl_decrement_enable_reg;

  /* Custom Config:                                                 */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae1 */

     /* IpProtocolCustom:                                            */
     /* range: 7:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD ip_protocol_custom;

     /* EthernetTypeCustom:                                          */
     /* range: 23:8, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD ethernet_type_custom;

  } __ATTRIBUTE_PACKED__ custom_config_reg;

  /* Mpls Control Word:                                             */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae2 */

     /* MplsControlWord:                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD mpls_control_word;

  } __ATTRIBUTE_PACKED__ mpls_control_word_reg;

  /* Pcp Dei Profile Use Tc Dp:                                     */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae3 */

     /* PcpDeiProfileUseTcDp:                                        */
     /* range: 15:0, access type: RW, default value: 0x0             */
     SOC_PB_PP_REG_FIELD pcp_dei_profile_use_tc_dp;

  } __ATTRIBUTE_PACKED__ pcp_dei_profile_use_tc_dp_reg;

  /* Count Out Of Range:                                            */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae4 */

     /* CountOutOfRange:                                             */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD count_out_of_range;

  } __ATTRIBUTE_PACKED__ count_out_of_range_reg;

  /* Etpp Debug Configuration:                                      */
  struct
  {
     SOC_PB_PP_REG_ADDR  addr; /* 0x3ae5 */

     /* DebugSelect:                                                 */
     /* range: 3:0, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD debug_select;

     /* DebugWr:                                                     */
     /* range: 4:4, access type: RW, default value: 0x0              */
     SOC_PB_PP_REG_FIELD debug_wr;

  } __ATTRIBUTE_PACKED__ etpp_debug_configuration_reg;

} __ATTRIBUTE_PACKED__ SOC_PB_PP_EPNI_REGS;
  /* Blocks definition } */

  typedef struct
  {
    SOC_PB_PP_ECI_REGS eci;
    SOC_PB_PP_OLP_REGS olp;
    SOC_PB_PP_IHP_REGS ihp;
    SOC_PB_PP_IHB_REGS ihb;
    SOC_PB_PP_EGQ_REGS egq;
    SOC_PB_PP_EPNI_REGS epni;
  } __ATTRIBUTE_PACKED__ SOC_PB_PP_REGS;

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

uint32
  soc_pb_pp_regs_get(
    SOC_SAND_OUT SOC_PB_PP_REGS  **soc_pb_pp_regs
  );

uint32
  soc_pb_pp_regs_init(void);

SOC_PB_PP_REGS*
  soc_pb_pp_regs(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_CHIP_REGS_INCLUDED__*/
#endif
