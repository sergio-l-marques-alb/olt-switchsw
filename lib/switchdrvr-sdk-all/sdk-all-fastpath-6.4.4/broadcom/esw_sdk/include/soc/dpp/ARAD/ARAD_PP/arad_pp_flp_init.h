

/* $Id: arad_pp_flp_init.h,v 1.59 Broadcom SDK $
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
 * $
*/

#ifndef __ARAD_PP_FLP_INIT_INCLUDED__
/* { */
#define __ARAD_PP_FLP_INIT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* KBR Defines */
#define ARAD_PP_FLP_32B_INST_32_ZEROS                                                                    (0xffc8)
#define ARAD_PP_FLP_32B_INST_OFFSET_ZEROS                                                                (0x7c)
#define ARAD_PP_FLP_16B_INST_16_ZEROS                                                                    (0xffc8)
#define ARAD_PP_FLP_16B_INST_OFFSET_ZEROS                                                                (0xfc)
#define ARAD_PP_FLP_32B_INST_32_ONES                                                                     (0xff88)
#define ARAD_PP_FLP_32B_INST_OFFSET_ONES                                                                 (0x78)
#define ARAD_PP_FLP_16B_INST_16_ONES                                                                     (0xff48)
#define ARAD_PP_FLP_16B_INST_OFFSET_ONES                                                                 (0xf4)
#define ARAD_PP_FLP_32B_INST_P6_IN_PORT_D                                                                (0x3f48)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_PORT_D                                                         (0x74)
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_D                                                                (SOC_IS_JERICHO(unit)? 0x7ec8: 0x7ec8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_PORT_D                                                         (0xec)
#define ARAD_PP_FLP_32B_INST_P6_PTC_D                                                                    (0x3f38)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PTC_D                                                             (0x73)
#define ARAD_PP_FLP_16B_INST_P6_PTC_D                                                                    (0x7ea8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PTC_D                                                             (0xea)
#define ARAD_PP_FLP_32B_INST_P6_IN_PORT_KEY_GEN_VAR_D                                                    (0x7f28)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_PORT_KEY_GEN_VAR_D                                             (0x72)
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D                                                    (0xfe88)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_PORT_KEY_GEN_VAR_D                                             (0xe8)
#define ARAD_PP_FLP_32B_INST_CPU2FLP_C_INTERNAL_FIELDS_DATA                                              (0xff08)
#define ARAD_PP_FLP_32B_INST_OFFSET_CPU2FLP_C_INTERNAL_FIELDS_DATA                                       (0x70)
#define ARAD_PP_FLP_16B_INST_0_CPU2FLP_C_INTERNAL_FIELDS_DATA                                            (0xfe48)
#define ARAD_PP_FLP_16B_INST_1_CPU2FLP_C_INTERNAL_FIELDS_DATA                                            (0xfe08)
#define ARAD_PP_FLP_16B_INST_OFFSET_CPU2FLP_C_INTERNAL_FIELDS_DATA                                       (0xe4)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_HEADER_SIZE_D                                                     (0x36c8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_HEADER_SIZE_D                                              (0x6c)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_HEADER_SIZE_D                                                     (0x6dc8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_HEADER_SIZE_D                                              (0xdc)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_0                (0x36b8)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_0         (0x6b)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_0                (0x6da8)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_0         (0xda)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_1                (0x36a8)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_1         (0x6a)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_1                (0x6d88)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_1         (0xd8)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_2                (0x3698)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_2         (0x69)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_2                (0x6d68)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_2         (0xd6)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_3                (0x3688)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_3         (0x68)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_3                (0x6d48)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_3         (0xd4)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_4                (0x3678)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_4         (0x67)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_4                (0x6d28)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_4         (0xd2)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_5                (0x3668)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_5         (0x66)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_5                (0x6d08)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_COMPENSATED_HEADER_OFFSET_IHP_HDR_OFFSET_BITS_5         (0xd0)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_1                                (0x5658)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_1                         (0x65)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_1                                (0xace8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_1                         (0xce)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_2                                (0xacb8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_2                         (0xcb)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_3                                (0x5628)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_3                         (0x62)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_3                                (0xac88)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_3                         (0xc8)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_4                                (0xac58)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_4                         (0xc5)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_5                                (0x55f8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_5                         (0x5f)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_5                                (0xac28)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER_D_IHP_QUAL_BITS_5                         (0xc2)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_QUALIFIER0_D                                               (0x2bf8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_QUALIFIER0_D                                        (0xbf)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_FORMAT_CODE_D                                                     (0x2dd8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_FORMAT_CODE_D                                              (0x5d)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_FORMAT_CODE_D                                                     (0x5be8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_FORMAT_CODE_D                                              (0xbe)
#define ARAD_PP_FLP_32B_INST_P6_SRC_SYSTEM_PORT_ID_D                                                     (0x7dc8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_SRC_SYSTEM_PORT_ID_D                                              (0x5c)
#define ARAD_PP_FLP_16B_INST_P6_SRC_SYSTEM_PORT_ID_D                                                     (0xfbc8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_SRC_SYSTEM_PORT_ID_D                                              (0xbc)
#define ARAD_PP_FLP_32B_INST_P6_PARSER_LEAF_CONTEXT_D                                                    (0x1da8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PARSER_LEAF_CONTEXT_D                                             (0x5a)
#define ARAD_PP_FLP_16B_INST_P6_PARSER_LEAF_CONTEXT_D                                                    (0x3b88)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PARSER_LEAF_CONTEXT_D                                             (0xb8)
#define ARAD_PP_FLP_16B_INST_P6_SERVICE_TYPE_D                                                           (0x2b78)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_SERVICE_TYPE_D                                                    (0xb7)
#define ARAD_PP_FLP_32B_INST_P6_INCOMING_TAG_STRUCTURE_D                                                 (0x1d98)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_INCOMING_TAG_STRUCTURE_D                                          (0x59)
#define ARAD_PP_FLP_16B_INST_P6_INCOMING_TAG_STRUCTURE_D                                                 (0x3b68)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_INCOMING_TAG_STRUCTURE_D                                          (0xb6)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_STRENGTH_D                                             (0x2b58)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_STRENGTH_D                                      (0xb5)
#define ARAD_PP_FLP_32B_INST_P6_FORWARDING_ACTION_DESTINATION_D                                          (0x9588)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_FORWARDING_ACTION_DESTINATION_D                                   (0x58)
#define ARAD_PP_FLP_16B_INST_0_P6_FORWARDING_ACTION_DESTINATION_D                                        (0xfb48)
#define ARAD_PP_FLP_16B_INST_1_P6_FORWARDING_ACTION_DESTINATION_D                                        (0x2b08)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_DESTINATION_D                                   (0xb4)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D                                                   (0x2af8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_TC_D                                            (0xaf)
#define ARAD_PP_FLP_32B_INST_P6_FORWARDING_ACTION_DP_D                                                   (0x0d58)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_FORWARDING_ACTION_DP_D                                            (0x55)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_DP_D                                                   (0x1ae8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_DP_D                                            (0xae)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_METER_TRAFFIC_CLASS_D                                  (0x1ad8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_METER_TRAFFIC_CLASS_D                           (0xad)
#define ARAD_PP_FLP_32B_INST_P6_FORWARDING_ACTION_CPU_TRAP_CODE_D                                        (0x3d48)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_FORWARDING_ACTION_CPU_TRAP_CODE_D                                 (0x54)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_CPU_TRAP_CODE_D                                        (0x7ac8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_CPU_TRAP_CODE_D                                 (0xac)
#define ARAD_PP_FLP_32B_INST_P6_FORWARDING_ACTION_CPU_TRAP_QUAL_D                                        (0x7d38)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_FORWARDING_ACTION_CPU_TRAP_QUAL_D                                 (0x53)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_CPU_TRAP_QUAL_D                                        (0xfaa8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_ACTION_CPU_TRAP_QUAL_D                                 (0xaa)
#define ARAD_PP_FLP_32B_INST_P6_SNOOP_CODE_D                                                             (0x3d18)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_SNOOP_CODE_D                                                      (0x51)
#define ARAD_PP_FLP_16B_INST_P6_SNOOP_CODE_D                                                             (0x7a68)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_SNOOP_CODE_D                                                      (0xa6)
#define ARAD_PP_FLP_32B_INST_P6_SNOOP_STRENGTH_D                                                         (0x0d08)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_SNOOP_STRENGTH_D                                                  (0x50)
#define ARAD_PP_FLP_16B_INST_P6_SNOOP_STRENGTH_D                                                         (0x1a48)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_SNOOP_STRENGTH_D                                                  (0xa4)
#define ARAD_PP_FLP_16B_INST_P6_LL_MIRROR_PROFILE_D                                                      (0x3a38)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_LL_MIRROR_PROFILE_D                                               (0xa3)
#define ARAD_PP_FLP_32B_INST_P6_USER_PRIORITY_D                                                          (0x14f8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_USER_PRIORITY_D                                                   (0x4f)
#define ARAD_PP_FLP_16B_INST_P6_USER_PRIORITY_D                                                          (0x2a28)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_USER_PRIORITY_D                                                   (0xa2)
#define ARAD_PP_FLP_16B_INST_P6_VSI_D                                                                    (0xfa18)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VSI_D                                                             (0xa1)
#define ARAD_PP_FLP_16B_INST_P6_VSI_PROFILE_D                                                            (0x39d8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VSI_PROFILE_D                                                     (0x9d)
#define ARAD_PP_FLP_32B_INST_P6_VSI_UNKNOWN_DA_DESTINATION_D                                             (0x94c8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VSI_UNKNOWN_DA_DESTINATION_D                                      (0x4c)
#define ARAD_PP_FLP_16B_INST_0_P6_VSI_UNKNOWN_DA_DESTINATION_D                                           (0xf9c8)
#define ARAD_PP_FLP_16B_INST_1_P6_VSI_UNKNOWN_DA_DESTINATION_D                                           (0x2988)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VSI_UNKNOWN_DA_DESTINATION_D                                      (0x9c)
#define ARAD_PP_FLP_16B_INST_P6_FID_D                                                                    (SOC_IS_JERICHO(unit)? 0xe808: 0xe978)
#define ARAD_PP_FLP_16B_INST_P6_FID_D_BITS_8                                                             (SOC_IS_JERICHO(unit)? 0x7808: 0x7978)

#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FID_D                                                             (0x97)
#define ARAD_PP_FLP_16B_INST_P6_ORIENTATION_IS_HUB_D                                                     (0x0938)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_ORIENTATION_IS_HUB_D                                              (0x93)
#define ARAD_PP_FLP_32B_INST_P6_I_SID_D                                                                  (0xbc78)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_I_SID_D                                                           (0x47)
#define ARAD_PP_FLP_16B_INST_0_P6_I_SID_D                                                                (0xf928)
#define ARAD_PP_FLP_16B_INST_1_P6_I_SID_D                                                                (0x78e8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_I_SID_D                                                           (0x92)
#define ARAD_PP_FLP_32B_INST_0_P6_VLAN_EDIT_CMD_D                                                        (0xfc48)
#define ARAD_PP_FLP_32B_INST_1_P6_VLAN_EDIT_CMD_D                                                        (0x0c08)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VLAN_EDIT_CMD_D                                                   (0x44)
#define ARAD_PP_FLP_16B_INST_0_P6_VLAN_EDIT_CMD_D                                                        (0xf8c8)
#define ARAD_PP_FLP_16B_INST_1_P6_VLAN_EDIT_CMD_D                                                        (0xf888)
#define ARAD_PP_FLP_16B_INST_2_P6_VLAN_EDIT_CMD_D                                                        (0x1848)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VLAN_EDIT_CMD_D                                                   (0x8c)
#define ARAD_PP_FLP_16B_INST_P6_TT_LEARN_ENABLE_D                                                        (0x0838)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LEARN_ENABLE_D                                                 (0x83)
#define ARAD_PP_FLP_32B_INST_0_P6_TT_LEARN_DATA_D                                                        (0xfbf8)
#define ARAD_PP_FLP_32B_INST_1_P6_TT_LEARN_DATA_D                                                        (0x3bb8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TT_LEARN_DATA_D                                                   (0x3f)
#define ARAD_PP_FLP_16B_INST_0_P6_TT_LEARN_DATA_D                                                        (0xf828)
#define ARAD_PP_FLP_16B_INST_1_P6_TT_LEARN_DATA_D                                                        (0xf7e8)
#define ARAD_PP_FLP_16B_INST_2_P6_TT_LEARN_DATA_D                                                        (0x77a8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LEARN_DATA_D                                                   (0x82)
#define ARAD_PP_FLP_32B_INST_P6_STP_STATE_D                                                              (0x0ba8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_STP_STATE_D                                                       (0x3a)
#define ARAD_PP_FLP_16B_INST_P6_STP_STATE_D                                                              (0x1788)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_STP_STATE_D                                                       (0x78)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_CODE_D                                                        (0x3778)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_CODE_D                                                 (0x77)
#define ARAD_PP_FLP_32B_INST_P6_FORWARDING_OFFSET_INDEX_D                                                (0x1398)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_FORWARDING_OFFSET_INDEX_D                                         (0x39)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_OFFSET_INDEX_D                                                (0x2768)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_OFFSET_INDEX_D                                         (0x76)
#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_OFFSET_EXTENSION_D                                            (0x1758)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_FORWARDING_OFFSET_EXTENSION_D                                     (0x75)
#define ARAD_PP_FLP_32B_INST_P6_TUNNEL_TERMINATION_CODE_D                                                (0x1b88)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TUNNEL_TERMINATION_CODE_D                                         (0x38)
#define ARAD_PP_FLP_16B_INST_P6_TUNNEL_TERMINATION_CODE_D                                                (0x3748)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TUNNEL_TERMINATION_CODE_D                                         (0x74)
#define ARAD_PP_FLP_32B_INST_P6_EEI_D                                                                    (0xbb78)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_EEI_D                                                             (0x37)
#define ARAD_PP_FLP_16B_INST_0_P6_EEI_D                                                                  (0xf728)
#define ARAD_PP_FLP_16B_INST_1_P6_EEI_D                                                                  (0x76e8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_EEI_D                                                             (0x72)
#define ARAD_PP_FLP_32B_INST_P6_OUT_LIF_D                                                                (0x7b48)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_OUT_LIF_D                                                         (0x34)
#define ARAD_PP_FLP_16B_INST_P6_OUT_LIF_D                                                                (0xf6c8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_OUT_LIF_D                                                         (0x6c)
#define ARAD_PP_FLP_32B_INST_P6_IN_LIF_D                                                                 (0x7b28)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_LIF_D                                                          (0x32)
#define ARAD_PP_FLP_16B_INST_P6_IN_LIF_D                                                                 (SOC_IS_JERICHO(unit)? 0xfaac : 0xf688)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_LIF_D                                                          (0x68)
#define ARAD_PP_FLP_32B_INST_P6_IN_LIF_PROFILE_D                                                         (0x1b08)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_LIF_PROFILE_D                                                  (0x30)
#define ARAD_PP_FLP_16B_INST_P6_IN_LIF_PROFILE_D                                                         (0x3648)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_LIF_PROFILE_D                                                  (0x64)
#define ARAD_PP_FLP_16B_INST_P6_IN_LIF_UNKNOWN_DA_PROFILE_D                                              (0x1638)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_LIF_UNKNOWN_DA_PROFILE_D                                       (0x63)
#define ARAD_PP_FLP_32B_INST_P6_IN_RIF_D                                                                 (0x5af8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_RIF_D                                                          (0x2f)
#define ARAD_PP_FLP_16B_INST_P6_IN_RIF_D                                                                 (SOC_IS_JERICHO(unit)? 0xe3e8: 0xb628)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_RIF_D                                                          (0x62)
#define ARAD_PP_FLP_16B_INST_P6_IN_RIF_UC_RPF_ENABLE_D                                                   (0x05f8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_RIF_UC_RPF_ENABLE_D                                            (0x5f)
#define ARAD_PP_FLP_32B_INST_P6_VRF_D                                                                    (0x5ad8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VRF_D                                                             (0x2d)
#define ARAD_PP_FLP_16B_INST_P6_VRF_D                                                                    (SOC_IS_JERICHO(unit)? 0xd388: 0xb5e8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VRF_D                                                             (0x5e)
#define ARAD_PP_FLP_16B_INST_P6_L3VPN_DEFAULT_ROUTING_D                                                  (0x05b8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_L3VPN_DEFAULT_ROUTING_D                                           (0x5b)
#define ARAD_PP_FLP_32B_INST_P6_PACKET_IS_COMPATIBLE_MC_D                                                (0x02b8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PACKET_IS_COMPATIBLE_MC_D                                         (0x2b)
#define ARAD_PP_FLP_16B_INST_P6_PACKET_IS_COMPATIBLE_MC_D                                                (0x05a8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PACKET_IS_COMPATIBLE_MC_D                                         (0x5a)
#define ARAD_PP_FLP_16B_INST_P6_TERMINATED_TTL_VALID_D                                                   (0x0598)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TERMINATED_TTL_VALID_D                                            (0x59)
#define ARAD_PP_FLP_32B_INST_P6_TERMINATED_TTL_D                                                         (0x3aa8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TERMINATED_TTL_D                                                  (0x2a)
#define ARAD_PP_FLP_16B_INST_P6_TERMINATED_TTL_D                                                         (0x7588)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TERMINATED_TTL_D                                                  (0x58)
#define ARAD_PP_FLP_32B_INST_P6_TERMINATED_DSCP_EXP_D                                                    (0x3a98)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TERMINATED_DSCP_EXP_D                                             (0x29)
#define ARAD_PP_FLP_16B_INST_P6_TERMINATED_DSCP_EXP_D                                                    (0x7568)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TERMINATED_DSCP_EXP_D                                             (0x56)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_IN_DSCP_EXP                                                    (0x3a88)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_IN_DSCP_EXP                                             (0x28)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_IN_DSCP_EXP                                                    (0x7548)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_IN_DSCP_EXP                                             (0x54)
#define ARAD_PP_FLP_32B_INST_P6_IN_TTL_D                                                                 (0x3a78)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_IN_TTL_D                                                          (0x27)
#define ARAD_PP_FLP_16B_INST_P6_IN_TTL_D                                                                 (0x7528)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_IN_TTL_D                                                          (0x52)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_ESADI                                                          (0x0268)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_ESADI                                                   (0x26)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_ESADI                                                          (SOC_IS_JERICHO(unit)? 0x0288: 0x0508)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_ESADI                                                   (0x50)
#define ARAD_PP_FLP_16B_INST_IPR2DSP_6EQ7_MPLS_EXP                                                       (SOC_IS_JERICHO(unit)? 0x2268: 0x24f8)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_MPLS_EXP                                                (0x4f)
#define ARAD_PP_FLP_32B_INST_IPR2DSP_6EQ7_PROGRAM_KEY_GEN_VAR                                            (0xfa58)
#define ARAD_PP_FLP_32B_INST_OFFSET_IPR2DSP_6EQ7_PROGRAM_KEY_GEN_VAR                                     (0x25)
#define ARAD_PP_FLP_16B_INST_0_IPR2DSP_6EQ7_PROGRAM_KEY_GEN_VAR                                          (0xf4e8)
#define ARAD_PP_FLP_16B_INST_1_IPR2DSP_6EQ7_PROGRAM_KEY_GEN_VAR                                          (0xf4a8)
#define ARAD_PP_FLP_16B_INST_OFFSET_IPR2DSP_6EQ7_PROGRAM_KEY_GEN_VAR                                     (0x4e)
#define ARAD_PP_FLP_32B_INST_P6_TERMINATED_PROTOCOL_D                                                    (0x0a18)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TERMINATED_PROTOCOL_D                                             (0x21)
#define ARAD_PP_FLP_16B_INST_P6_TERMINATED_PROTOCOL_D                                                    (0x1468)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TERMINATED_PROTOCOL_D                                             (0x46)
#define ARAD_PP_FLP_16B_INST_P6_COS_PROFILE_D                                                            (0x5458)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_COS_PROFILE_D                                                     (0x45)
#define ARAD_PP_FLP_32B_INST_P6_COUNTER_POINTER_A_D                                                      (0xa1f8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_COUNTER_POINTER_A_D                                               (0x1f)
#define ARAD_PP_FLP_16B_INST_0_P6_COUNTER_POINTER_A_D                                                    (0xf428)
#define ARAD_PP_FLP_16B_INST_1_P6_COUNTER_POINTER_A_D                                                    (0x43e8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_COUNTER_POINTER_A_D                                               (0x42)
#define ARAD_PP_FLP_32B_INST_P6_COUNTER_UPDATE_A_D                                                       (0x01c8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_COUNTER_UPDATE_A_D                                                (0x1c)
#define ARAD_PP_FLP_16B_INST_P6_COUNTER_UPDATE_A_D                                                       (0x03c8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_COUNTER_UPDATE_A_D                                                (0x3c)
#define ARAD_PP_FLP_32B_INST_P6_COUNTER_POINTER_B_D                                                      (0xa1b8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_COUNTER_POINTER_B_D                                               (0x1b)
#define ARAD_PP_FLP_16B_INST_0_P6_COUNTER_POINTER_B_D                                                    (0xf3a8)
#define ARAD_PP_FLP_16B_INST_1_P6_COUNTER_POINTER_B_D                                                    (0x4368)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_COUNTER_POINTER_B_D                                               (0x3a)
#define ARAD_PP_FLP_32B_INST_P6_COUNTER_UPDATE_B_D                                                       (0x0188)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_COUNTER_UPDATE_B_D                                                (0x18)
#define ARAD_PP_FLP_16B_INST_P6_COUNTER_UPDATE_B_D                                                       (0x0348)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_COUNTER_UPDATE_B_D                                                (0x34)
#define ARAD_PP_FLP_16B_INST_P6_VTT_OAM_LIF_VALID_D                                                      (0x0338)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VTT_OAM_LIF_VALID_D                                               (0x33)
#define ARAD_PP_FLP_32B_INST_P6_VTT_OAM_LIF_D                                                            (0x7978)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VTT_OAM_LIF_D                                                     (0x17)
#define ARAD_PP_FLP_16B_INST_P6_VTT_OAM_LIF_D                                                            (0xf328)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VTT_OAM_LIF_D                                                     (0x32)
#define ARAD_PP_FLP_32B_INST_P6_PROGRAM_INDEX_D                                                          (0x2158)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_PROGRAM_INDEX_D                                                   (0x15)
#define ARAD_PP_FLP_16B_INST_P6_PROGRAM_INDEX_D                                                          (0x42e8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_PROGRAM_INDEX_D                                                   (0x2e)
#define ARAD_PP_FLP_32B_INST_P6_LL_LEM_1ST_LOOKUP_FOUND_D                                                (0x0148)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_LL_LEM_1ST_LOOKUP_FOUND_D                                         (0x14)
#define ARAD_PP_FLP_16B_INST_P6_LL_LEM_1ST_LOOKUP_FOUND_D                                                (0x02c8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_LL_LEM_1ST_LOOKUP_FOUND_D                                         (0x2c)
#define ARAD_PP_FLP_32B_INST_0_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                             (0xf938)
#define ARAD_PP_FLP_32B_INST_1_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                             (0x50f8)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                        (0x13)
#define ARAD_PP_FLP_16B_INST_0_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                             (0xf2a8)
#define ARAD_PP_FLP_16B_INST_1_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                             (0xf268)
#define ARAD_PP_FLP_16B_INST_2_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                             (0xa228)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_LL_LEM_1ST_LOOKUP_RESULT_D                                        (0x2a)
#define ARAD_PP_FLP_32B_INST_0_P6_VT_ISA_KEY_D                                                           (SOC_IS_JERICHO(unit)? 0xff5c: 0xf8d8)
#define ARAD_PP_FLP_32B_INST_1_P6_VT_ISA_KEY_D                                                           (0x4098)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VT_ISA_KEY_D                                                      (0x0d)
#define ARAD_PP_FLP_16B_INST_0_P6_VT_ISA_KEY_D                                                           (0xf1e8)
#define ARAD_PP_FLP_16B_INST_1_P6_VT_ISA_KEY_D                                                           (0xf1a8)
#define ARAD_PP_FLP_16B_INST_2_P6_VT_ISA_KEY_D                                                           (0x8168)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_ISA_KEY_D                                                      (0x1e)
#define ARAD_PP_FLP_16B_INST_P6_VT_PROCESSING_PROFILE_D                                                  (0x2138)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_PROCESSING_PROFILE_D                                           (0x13)
#define ARAD_PP_FLP_32B_INST_P6_VT_LOOKUP0_FOUND_D                                                       (0x0078)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VT_LOOKUP0_FOUND_D                                                (0x07)
#define ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_FOUND_D                                                       (0x0128)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_LOOKUP0_FOUND_D                                                (0x12)
#define ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D                                                     (0xf118)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_LOOKUP0_PAYLOAD_D                                              (0x11)
#define ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP1_FOUND_D                                                       (0x00d8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_LOOKUP1_FOUND_D                                                (0x0d)
#define ARAD_PP_FLP_32B_INST_P6_VT_LOOKUP1_PAYLOAD_D                                                     (0x7848)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_VT_LOOKUP1_PAYLOAD_D                                              (0x04)
#define ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP1_PAYLOAD_D                                                     (0xf0c8)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_VT_LOOKUP1_PAYLOAD_D                                              (0x0c)
#define ARAD_PP_FLP_32B_INST_P6_TT_PROCESSING_PROFILE_D                                                  (0x1028)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TT_PROCESSING_PROFILE_D                                           (0x02)
#define ARAD_PP_FLP_16B_INST_P6_TT_PROCESSING_PROFILE_D                                                  (0x2088)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_PROCESSING_PROFILE_D                                           (0x08)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_FOUND_D                                                       (0x0078)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LOOKUP0_FOUND_D                                                (0x07)
#define ARAD_PP_FLP_32B_INST_P6_TT_LOOKUP0_PAYLOAD_D                                                     (0x7818)
#define ARAD_PP_FLP_32B_INST_OFFSET_P6_TT_LOOKUP0_PAYLOAD_D                                              (0x01)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D                                                     (SOC_IS_JERICHO(unit)? 0xfcac : 0xf068)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LOOKUP0_PAYLOAD_D                                              (0x06)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP1_FOUND_D                                                       (0x0028)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LOOKUP1_FOUND_D                                                (0x02)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP1_PAYLOAD_D                                                     (0xf018)
#define ARAD_PP_FLP_16B_INST_OFFSET_P6_TT_LOOKUP1_PAYLOAD_D                                              (0x01)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D_BITS_12                                             (0xb068)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D_BITS_9                                             (SOC_IS_JERICHO(unit)? 0x8cac : 0x8068)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP1_PAYLOAD_D_BITS_9                                              (0x8018)
#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP1_PAYLOAD_D_BITS_12                                             (0xb018)

#define ARAD_PP_FLP_ETH_KEY_OR_MASK(unit)   ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) ? 0x0 : 0x1)
/* prefix is 4 b', but only 3 MSB are written to tbl, so must be of type XXX0 (even) */
#define ARAD_PP_FLP_B_ETH_KEY_OR_MASK(unit) ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) ? ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_SA_AUTH : 0x0)
#define ARAD_PP_FLP_IPV4_KEY_OR_MASK                0x2
#define ARAD_PP_FLP_LSR_KEY_OR_MASK                 0x3
#define ARAD_PP_FLP_P2P_KEY_OR_MASK                 0x4
#define ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK       0x8 /* cover 8-15 */
#define ARAD_PP_FLP_IPV4_COMP_KEY_OR_MASK           0x6
#define ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK   0x7
#define ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK   0x5 
#define ARAD_PP_FLP_COUPLING_LSR_KEY_OR_MASK        0x9
#define ARAD_PP_FLP_TRILL_KEY_OR_MASK               0xa
#define ARAD_PP_FLP_TRILL_KEY_OR_MASK_MC            0xb

/* As of this KEY_OR_MASK, the values are app_ids and NOT prefixes. The prefixes are allocated dynamically on demand. */
#define ARAD_PP_FLP_FC_KEY_OR_MASK                  0xc
#define ARAD_PP_FLP_FC_ZONING_KEY_OR_MASK           0xd
#define ARAD_PP_FLP_FC_REMOTE_KEY_OR_MASK           0xf
#define ARAD_PP_FLP_IPMC_BIDIR_KEY_OR_MASK          0x10
#define ARAD_PP_FLP_OMAC_2_VMAC_KEY_OR_MASK         0x11
#define ARAD_PP_FLP_VMAC_KEY_OR_MASK                0x12
#define ARAD_PP_FLP_VMAC_2_OMAC_KEY_OR_MASK         0x13
#define ARAD_PP_FLP_TRILL_ADJ_KEY_OR_MASK           0x14
#define ARAD_PP_FLP_SLB_KEY_OR_MASK                 0x15
#define ARAD_PP_FLP_GLOBAL_IPV4_KEY_OR_MASK         0x18 /* prefix value will be cover 12-15 */
#define ARAD_PP_FLP_FC_N_PORT_KEY_OR_MASK           0x19
#define ARAD_PP_FLP_BFD_SINGLE_HOP_KEY_OR_MASK		0x20
#define ARAD_PP_FLP_OAM_STATISTICS_KEY_OR_MASK		0x21
#define ARAD_PP_FLP_IP6_SPOOF_STATIC_KEY_OR_MASK           0x22
#define ARAD_PP_FLP_IP6_COMPRESSION_DIP_KEY_OR_MASK        0x23
#define ARAD_PP_FLP_BFD_STATISTICS_KEY_OR_MASK		0x24
#define ARAD_PP_FLP_OAM_DOWN_UNTAGGED_STATISTICS_KEY_OR_MASK		0x25
#define ARAD_PP_FLP_OAM_SINGLE_TAG_STATISTICS_KEY_OR_MASK		0x26
#define ARAD_PP_FLP_OAM_DOUBLE_TAG_STATISTICS_KEY_OR_MASK		0x27
#define ARAD_PP_FLP_KEY_OR_MASK_LAST                0x28 /* need to update this value when adding new define */




/* FLP port profiles */
#define ARAD_PP_FLP_PORT_PROFILE_DEFAULT      (0)
#define ARAD_PP_FLP_PORT_PROFILE_EXTENDED_P2P (1)
#define ARAD_PP_FLP_PORT_PROFILE_PBP          (2)/* mac-in-mac*/
#define ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT     ARAD_PP_FLP_PORT_PROFILE_EXTENDED_P2P


#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP     (1)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV     (2)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_ALL     (0)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE    (SOC_IS_JERICHO(unit)? 0x1F: 0x3)

/* kbr defines } */

#define ARAD_PP_FLP_KEY_PROGRAM_TM                             0x00
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_ING_LEARN             0x01
#define ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_DEFAULT            0x02
#define ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_RPF                0x03
#define ARAD_PP_FLP_KEY_PROGRAM_IPV4UC                         0x04
#define ARAD_PP_FLP_KEY_PROGRAM_IPV6UC                         0x05
#define ARAD_PP_FLP_KEY_PROGRAM_P2P                            0x06
#define ARAD_PP_FLP_KEY_PROGRAM_IPV6MC                         0x07
#define ARAD_PP_FLP_KEY_PROGRAM_LSR                            0x08
#define ARAD_PP_FLP_KEY_PROGRAM_TRILL_UC                       0x0a /* Trill */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_STATIC 0x0a /* PON */
#define ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_ONE_TAG               0x0b /* Trill */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_STATIC 0x0b /* PON */
#define ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_TWO_TAGS              0x0c /* Trill */
#define ARAD_PP_FLP_KEY_PROGRAM_MAC_IN_MAC_AFTER_TERMINATIOM   0x0c
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_DHCP   0x0d /* PON */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_DHCP   0x0e /* PON */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_MAC_IN_MAC            0x0f
#define ARAD_PP_FLP_KEY_PROGRAM_TRILL_AFTER_TERMINATION        0x10 /* Trill */
#define ARAD_PP_FLP_KEY_PROGRAM_IPV4MC_WITH_RPF                0x11 /* bridging */
#define ARAD_PP_FLP_KEY_PROGRAM_IPV4COMPMC_WITH_RPF            0x12 /* routing */
#define ARAD_PP_FLP_KEY_PROGRAM_FC_REMOTE                      0x13 /* FCoE*/
#define ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT_REMOTE             0x14 /* FCoE*/
#define ARAD_PP_FLP_KEY_PROGRAM_FC                             0x15 /* FCoE*/
#define ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT                    0x16 /* FCoE*/
#define ARAD_PP_FLP_KEY_PROGRAM_COUPLING_LSR                   0x17
#define ARAD_PP_FLP_KEY_PROGRAM_BIDIR                          0x19
#define ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF                0x20
#define ARAD_PP_FLP_KEY_PROGRAM_VPLSOGRE                       0x22
#define ARAD_PP_FLP_KEY_PROGRAM_VMAC_UPSTREAM                  0x23 /* VMAC */
#define ARAD_PP_FLP_KEY_PROGRAM_VMAC_DOWNSTREAM                0x24 /* VMAC */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_PON_LOCAL_ROUTE       0x25 /* PON local route enabled */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_PON_DEFAULT_UPSTREAM  0x26 /* PON local route disabled upstream */
#define ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_PON_DEFAULT_DOWNSTREAM 0x27 /* PON local route disabled downstream */

#define ARAD_PP_FLP_KEY_PROGRAM_FC_N_PORT                      0x28 /* FCoE*/
#define ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT_N_PORT             0x29 /* FCoE*/

#define ARAD_PP_FLP_KEY_PROGRAM_GLOBAL_IPV4COMPMC_WITH_RPF     0x2a /* global routing */
#define ARAD_PP_FLP_KEY_PROGRAM_PON_ARP_DOWNSTREAM             0x2b
#define ARAD_PP_FLP_KEY_PROGRAM_PON_ARP_UPSTREAM               0x2c
#define ARAD_PP_FLP_KEY_PROGRAM_FC_TRANSIT                     0x2d /* for FC bridging similar to ethernet, but needed to enable FC-traps, learning should be disabled for these packets*/
#define ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF_2PASS          0x2e
#define ARAD_PP_FLP_KEY_PROGRAM_BFD_IPV4_SINGLE_HOP            0x2f
#define ARAD_PP_FLP_KEY_PROGRAM_BFD_IPV6_SINGLE_HOP            0x30
#define ARAD_PP_FLP_KEY_PROGRAM_OAM_STATISTICS				   0x31
#define ARAD_PP_FLP_KEY_PROGRAM_BFD_STATISTICS				   0x32
#define ARAD_PP_FLP_KEY_PROGRAM_OAM_DOWN_UNTAGGED_STATISTICS   0x33
#define ARAD_PP_FLP_KEY_PROGRAM_OAM_SINGLE_TAG_STATISTICS      0x34
#define ARAD_PP_FLP_KEY_PROGRAM_OAM_DOUBLE_TAG_STATISTICS      0x35
#define ARAD_PP_FLP_KEY_PROGRAM_BFD_MPLS_STATISTICS			   0x36
#define ARAD_PP_FLP_KEY_PROGRAM_BFD_PWE_STATISTICS			   0x37
#define ARAD_PP_FLP_KEY_PROGRAM_LAST                           (ARAD_PP_FLP_KEY_PROGRAM_BFD_PWE_STATISTICS)

#define ARAD_PP_FLP_MAP_PROG_NOT_SET                           (0xff)

/* total programs for FCoE are 7, one for trasit switch, 4 fcf and 2 for fcf n_port the last 2 are created only when NPV switch is enabled */
#define ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS                 4

/* Define an FLP Profile that is uses a signaling between FLP Programs and PMF
 * used for the FLP & PMF for */
#define ARAD_PP_FLP_PROGRAM_FWD_PROCESS_PROFILE_REPLACE_FWD_CODE    (0x01)


#define JERICHO_PP_FLP_16B_INST_P6_IN_MAPPED_PORT_D          ARAD_PP_FLP_16B_INST_P6_IN_PORT_D 

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

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

#if defined(INCLUDE_KBP) && !defined(BCM_88030)

uint32
    arad_pp_flp_elk_prog_config_max_key_size_get(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
       SOC_SAND_OUT uint32   *max_key_size_in_bits
    );

uint32
    arad_pp_flp_elk_prog_config(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
       SOC_SAND_IN  uint32  opcode,
       SOC_SAND_IN  uint32  key_size
    );

#endif

/* For TM Init */
uint32
   arad_pp_flp_prog_sel_cam_key_program_tm(
     int unit
   );
uint32
   arad_pp_flp_process_key_program_tm(
     int unit
   );



/* initialize FLP programs */
uint32
  arad_pp_flp_init(
     int unit,
     uint8 ingress_learn_enable, /* = 1*/
     uint8 ingress_learn_oppurtunistic, /* = 0 */
     uint32  sa_lookup_type /*hex 2'b10*/
   );

/* update ethernet program to perform ingress/egress learning */
uint32
  arad_pp_flp_ethernet_prog_update(
     int unit,
     uint8 learn_enable,
     uint8 ingress_learn_enable, /* = 1*/
     uint8 ingress_learn_oppurtunistic, /* = 0 */
     uint32  sa_lookup_type /*hex 2'b10*/
   );

uint32
  arad_pp_flp_ethernet_prog_learn_get(
     int unit,
     uint8 *learn_enable /* = 1*/
   );


/* update trap configuration */
uint32
  arad_pp_flp_trap_config_update(
     int unit,
     ARAD_PP_TRAP_CODE_INTERNAL trap_code_internal, 
     int trap_strength,  /* -1 for don't update */
     int snoop_strength/* -1 for don't update */
   );

uint32
  arad_pp_flp_trap_config_get(
     int unit,
     ARAD_PP_TRAP_CODE_INTERNAL trap_code_internal, 
     uint32 *trap_strength, /* null for ignore */
     uint32  *snoop_strength/* null for ignore */
   );

/* set learning for FLP program */
uint32
    arad_pp_flp_prog_learn_set(
       int unit,
       int32  prog_id,
       uint8  learn_enable
    );

/* get learning for FLP program */
uint32
  arad_pp_flp_prog_learn_get(
     int unit,
     int32  prog_id,
     uint8  *learn_enable
  );

/* 
 *  for IPMC programs wether to flood unknown packets,
 *  or to forward accroding to VRF-default destination
 */ 
uint32
   arad_pp_ipmc_not_found_proc_update(
     int unit,
     uint8  flood
   );

uint32
   arad_pp_ipmc_not_found_proc_get(
     int unit,
     uint8  *flood
   );


uint32
   arad_pp_ipv4mc_bridge_lookup_update(
     int unit,
     uint8  mode /* 0:<FID,DA>, 1:<FID,DIP>*/
   );

uint32
   arad_pp_ipv4mc_bridge_lookup_get(
     int unit,
     uint8  *mode
   );


/* Update LSR key construction */
uint32
   arad_pp_flp_key_const_lsr(
     int unit,
     uint8  in_port,
     uint8  in_rif,
     uint8  in_exp
   );

uint32
   arad_pp_flp_key_const_pwe_gre(
     int unit,     
     uint8  in_port,
     uint8  in_rif,
     uint8  in_exp
   );


uint32
   arad_pp_flp_lookups_ipv4uc_tcam_profile_set(
     int unit,
     uint32 tcam_access_profile_ndx,
     uint32 tcam_access_profile_id
   );

/* Update TRILL MC/IPV4MC / IPV6UC / IPV6MC according to usage of TCAM. */
uint32
   arad_pp_flp_lookups_TRILL_mc(
     int unit,
     uint32 is_ingress_learn,
     uint32 tcam_access_profile_id
   );
   
uint32
   arad_pp_flp_lookups_ipv4mc_with_rpf(
     int unit,
     uint8 ingress_learn_enable, /* = 1,*/
     uint8 ingress_learn_oppurtunistic /* = 0*/
   );


uint32
   arad_pp_flp_lookups_ipv4compmc_with_rpf(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_global_ipv4compmc_with_rpf(
     int    unit,
     int prog_id,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_ipv6uc(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_ipv6mc(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_oam(
     int unit,
     uint32 tcam_access_profile_id_0,
     uint32 tcam_access_profile_id_1,
     uint32 flp_key_program
   );

uint32
   arad_pp_flp_lookups_ethernet_tk_epon_uni_v6(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 ingress_learn_enable,
     uint8 ingress_learn_oppurtunistic
   );

uint32
   arad_pp_flp_lookups_ethernet_pon_default_downstream(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 ingress_learn_enable, /* = 1,*/
     uint8 ingress_learn_oppurtunistic, /* = 0*/
     int32 prog_id
   );

/* Get ingress learn mode of anti-spoofing V6*/
uint32
   arad_pp_flp_tk_epon_uni_v6_ing_learn_get(
     int unit,
     uint8 *ingress_learn_enable,
     uint8 *ingress_learn_oppurtunistic
   );


char*
  arad_pp_flp_prog_id_to_prog_name(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 prog_id
);

uint32
  arad_pp_flp_access_print_all_programs_data(
    SOC_SAND_IN  int unit
  );

/* Retrieve the FLP-Program according to the application type */
uint32 arad_pp_flp_app_to_prog_index_get(
   int unit,
   uint32 app_id,
   uint8  *prog_index
);

uint32
arad_pp_flp_fcoe_zoning_set(
     int unit,
     int enable
   );

uint32
arad_pp_flp_fcoe_vsan_mode_set(
     int unit,
     int is_vsan_from_vsi
   );

uint32
   arad_pp_flp_n_port_programs_disable(
     int unit
   );

uint32
   arad_pp_flp_npv_programs_init(
     int unit
   );

uint32
arad_pp_flp_fcoe_is_zoning_enabled(
     int unit,
     int* is_enabled
   );

uint32
arad_pp_flp_fcoe_is_vsan_from_vsi_mode(
     int unit,
     int* is_vsan_from_vsi
   );
/*********************************************************************
* NAME:
 *   arad_pp_flp_access_print_last_programs_data
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Dump last FLP program invoked.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                *last_program_id -
 *     Last FLP program invoked.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_flp_access_print_last_programs_data(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 to_print,
    SOC_SAND_OUT  int                 *prog_id
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_FLP_INIT_INCLUDED__*/
#endif

