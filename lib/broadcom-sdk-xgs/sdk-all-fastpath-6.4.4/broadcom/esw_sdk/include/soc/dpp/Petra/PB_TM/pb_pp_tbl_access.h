/* $Id: soc_pb_pp_tbl_access.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PP_TBL_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PB_PP_TBL_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
                                                                    
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB                              (3)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MSB                              (5)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_SHIFT                            (SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MASK                             (SOC_SAND_BITS_MASK(SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MSB, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB))

#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB                          (2)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MSB                          (2)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_SHIFT                        (SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MASK                         (SOC_SAND_BITS_MASK(SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MSB, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB))

#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB                      (0)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MSB                      (1)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_SHIFT                    (SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB)
#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MASK                     (SOC_SAND_BITS_MASK(SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MSB, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB))

#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB                      (0)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MSB                      (7)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_SHIFT                    (SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK                     (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MSB, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MAX                      (SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB                  (8)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MSB                  (9)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_SHIFT                (SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MASK                 (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MSB, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB))

#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB                    (0)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MSB                    (1)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_SHIFT                  (SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK                   (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MSB, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MAX                    (SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB                    (2)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MSB                    (4)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_SHIFT                  (SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK                   (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MSB, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MAX                    (SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB               (5)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MSB               (6)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_SHIFT             (SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MASK              (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MSB, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB))

#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB                 (0)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MSB                 (0)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_SHIFT               (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK                (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MSB, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MAX                 (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB                 (1)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MSB                 (3)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_SHIFT               (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK                (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MSB, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MAX                 (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB           (4)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MSB           (5)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_SHIFT         (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK          (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MSB, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB))
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MAX           (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB             (6)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MSB             (7)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_SHIFT           (SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MASK            (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MSB, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB))

#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB     (0)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MSB     (2)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_SHIFT   (SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MASK    (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MSB, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB))

#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB       (3)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MSB       (7)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_SHIFT     (SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MASK      (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MSB, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB))

#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB      (8)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MSB      (9)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_SHIFT    (SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MASK     (SOC_SAND_BITS_MASK(SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MSB, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB))

#define SOC_PB_PP_IHP_ID                                                         (SOC_PB_IHP_ID)
#define SOC_PB_PP_IHB_ID                                                         (SOC_PB_IHB_ID)
#define SOC_PB_PP_EGQ_ID                                                         (SOC_PB_EGQ_ID)
#define SOC_PB_PP_EPNI_ID                                                        (SOC_PB_EPNI_ID)


#define SOC_PB_PP_MAX_NOF_REPS                                                   (0xfffffff) /* 7F's */
#define SOC_PB_PP_FIRST_TBL_ENTRY                                                (0)
#define SOC_PB_PP_SINGLE_OPERATION                                               (0)


#define SOC_PB_PP_IHB_TCAM_DATA_WIDTH 5

#define SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS    (4)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* {VLAN-Processing-Profile(3b), Outer-Tag(2), Outer-Tag-Is-Priority(1), Inner-Tag(2) */
#define SOC_PB_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(port_profile, out_tag_encap, out_is_prio, in_tag_encap)   \
  ((port_profile<<5)+(out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)

/* {VLAN-Processing-Profile(3b), Outer-Tag(2), Outer-Tag-Is-Priority(1), Inner-Tag(2) */
#define SOC_PB_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET(port_profile, out_tag_encap, out_is_prio, in_tag_encap)   \
  ((((out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)<<2) + port_profile)

/* SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA: \{TPID-Profile (3), Packet-Tag-Format (5)\} */
#define SOC_PB_PP_TBL_EPNI_LLVP_KEY_ENTRY_OFFSET(tpid_profile, pkt_tag_format)   \
  ((tpid_profile<<5)+(pkt_tag_format))

/* {In-PP-Port. Trap-Profile(2), DA[5:0]} */
#define SOC_PB_PP_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(reserved_mc_profile, da_mac_address_lsb) \
          ((reserved_mc_profile<<6)+da_mac_address_lsb)

/* {VID-Index(3), In-PP-Port(6)} */
#define SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(vid, port) \
          ((vid<<6)+port)

/* {Packet-Is-MEF-L2CP, LLVP-Incoming-Tag-Structure, AC-VLAN-Edit-Profile} */
#define SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(is_l2cp, tag_struct_ndx, edit_profile) \
          ((SOC_SAND_BOOL2NUM(is_l2cp)<<8)+(tag_struct_ndx<<3)+edit_profile)

#define SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(key)                                                                                                               \
          SOC_SAND_SET_FLD_IN_PLACE((key)->da_type, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_SHIFT, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MASK) |                                     \
          SOC_SAND_SET_FLD_IN_PLACE(((key)->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB) , SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_SHIFT, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MASK) |                         \
          SOC_SAND_SET_FLD_IN_PLACE((key)->default_forward_profile, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_SHIFT, SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(op_code, tos)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(tos, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_KEY_ENTRY_OFFSET(op_code, tc, dp)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(tc, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(dp, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK)

#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_KEY_ENTRY_OFFSET(op_code, outer_tag, pcp, dei)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(outer_tag, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(pcp, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(dei, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_SHIFT, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK)

#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_KEY_ENTRY_OFFSET(key_program_profile, packet_format_qual, small_em_key_profile) \
          SOC_SAND_SET_FLD_IN_PLACE(key_program_profile, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_SHIFT, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE((packet_format_qual.outer_tpid << 3) | (packet_format_qual.is_outer_prio << 2) | (packet_format_qual.inner_tpid << 0), SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_SHIFT, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(small_em_key_profile, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_SHIFT, SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MASK)

/* PCP_DEI-Map-Key =
   LLVP-Incoming-S-Tag-Exist ? {2'b11, Incoming-Tag.PCP-DEI(4)} :
   LLVP-Incoming-Tag-Exist ? {2'b10, Incoming-Tag.UP(3), 1'b0} :
   Else {1'b0, Traffic-Class(3), Drop-Precedence(2)}
   Table is Accessed by {VLAN-Edit-PCP-DEI-Profile, PCP-DEI-Map-Key}. */
#define SOC_PB_PP_PCP_DEI_MAP_KEY_STAG(pcp, dei) \
  ((0x3<<4) + (pcp<<1) + dei)
#define SOC_PB_PP_PCP_DEI_MAP_KEY_CTAG(up) \
  ((0x2<<4) + (up<<1))
#define SOC_PB_PP_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp) \
  ((tc<<2) + dp)

#define SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  ((profile_ndx<<6)+  SOC_PB_PP_PCP_DEI_MAP_KEY_STAG(pcp, dei))
#define SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up) \
  ((profile_ndx<<6)+  SOC_PB_PP_PCP_DEI_MAP_KEY_CTAG(up))
#define SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  ((profile_ndx<<6)+  SOC_PB_PP_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp))

/* Index is similar to IHP vlan edit pcp-dei map table */
#define SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei)
#define SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(profile_ndx, up) \
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up)
#define SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp)

/* {Incoming-Tag-Format (4), VLAN-Edit-Profile (4)} */
#define SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(tag_format, edit_profile) \
  ((tag_format<<4) + edit_profile)

/* {Is-IPv4(1), In-PP-Port. Cfg-TC-DP-TOS-Index(1), TOS(8)} */
#define SOC_PB_PP_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, tbl_index, tos) \
  ((is_ipv4<<9) + (tbl_index<<8) + tos)

/* {Packet-Format-Qualifier1-Ext (4), In-PP-Port. Protocol-Profile (3) */
#define SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(prtcl_ndx, profile_ndx) \
  SOC_PB_PP_FLDS_TO_BUFF_2(prtcl_ndx, 4, profile_ndx, 3)
/*SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA*/
#define SOC_PB_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_ins) \
  (oam_ins>>3)
/*SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA*/
#define SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(port_num) \
  (port_num>>3)
/*SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA*/
#define SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id) \
  ((fec_id>>4)*2 + ((fec_id%2==0)?0:1))

/*SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA*/
#define SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(is_ipv4,tos,dp) \
  ((is_ipv4<<9) + (tos<<1) + dp)

/* TC-DP-Map[CoS-Profile (4) - 1, CoS-Profile-Map-Key-lsb (10) */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_COS_PROFILE_NOF_BITS (4)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_NOF_BITS     (10)

#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_ENTRY_OFFSET(cos_profile, key_lsb) \
  SOC_PB_PP_FLDS_TO_BUFF_2(                                                   \
    (cos_profile-1), SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_COS_PROFILE_NOF_BITS,  \
    key_lsb, SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_NOF_BITS)

/* {Type = Ethernet (3'b001), 1'b0, Packet-Format-Qualifier[1]. Outer-Tag (2) UP-PCP (3), DEI (1)} */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX                   (0x2) /* 3'b001 for type + 1'b0 */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX_NOF_BITS          (4)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_OUTER_TAG_NDX_NOF_BITS   (2)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PCP_NOF_BITS             (3)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_DEI_NOF_BITS             (1)

#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(outer_tag, pcp, dei)    \
  SOC_PB_PP_FLDS_TO_BUFF_4(                                                  \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX,                     \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX_NOF_BITS,            \
    outer_tag,                                                           \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_OUTER_TAG_NDX_NOF_BITS,     \
    pcp,                                                                 \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PCP_NOF_BITS,               \
    dei,                                                                 \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_DEI_NOF_BITS);


/* {Type = IPv4|6 (2'b10|2'b11), IP-Header. TOS (8)} */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX                 (0x2)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX                 (0x3)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_PREFIX_NOF_BITS          (2)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_TOS_NOF_BITS             (8)

#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(prefix, tos)            \
  SOC_PB_PP_FLDS_TO_BUFF_2(                                                  \
    prefix,                                                              \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_PREFIX_NOF_BITS,            \
    tos,                                                                 \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_TOS_NOF_BITS);

/* {Type = MPLS (2'b01), 5'b0, Outer-Label. EXP(3)} */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX                 (0x1<<5) /* (2'b01) + 5'b0 */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX_NOF_BITS        (7)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_EXP_NOF_BITS           (3)

#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(exp)                  \
  SOC_PB_PP_FLDS_TO_BUFF_2(                                                  \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX,                   \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX_NOF_BITS,          \
    exp,                                                                 \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_EXP_NOF_BITS);

/* {Type = TC-DP (3'b000), 2'b00, Forward-Action. TC (3), Forward-Action. DP (2)} */
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_TC_NOF_BITS           (3)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_DP_NOF_BITS           (2)

#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(tc, dp)              \
  SOC_PB_PP_FLDS_TO_BUFF_2(                                                  \
    tc,                                                                  \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_TC_NOF_BITS,             \
    dp,                                                                  \
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_DP_NOF_BITS);
    

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */


typedef struct
{
  uint32 vlan_classification_profile;
  uint32 pcp_profile;
  uint32 ignore_incoming_pcp;
  uint32 tc_dp_tos_enable;
  uint32 tc_dp_tos_index;
  uint32 tc_l4_protocol_enable;
  uint32 tc_subnet_enable;
  uint32 tc_port_protocol_enable;
  uint32 default_traffic_class;
  uint32 ignore_incoming_up;
  uint32 incoming_up_map_profile;
  uint32 use_dei;
  uint32 drop_precedence_profile;
  uint32 sa_lookup_enable;
  uint32 action_profile_sa_drop_index;
  uint32 enable_sa_authentication;
  uint32 ignore_incoming_vid;
  uint32 vid_ipv4_subnet_enable;
  uint32 vid_port_protocol_enable;
  uint32 protocol_profile;
  uint32 default_initial_vid;
  uint32 non_authorized_mode_8021x;
  uint32 enable_arp_trap;
  uint32 enable_igmp_trap;
  uint32 enable_mld_trap;
  uint32 enable_dhcp_trap;
  uint32 general_trap_enable;
  uint32 default_cpu_trap_code;
  uint32 default_action_profile_fwd;
  uint32 default_action_profile_snp;
  uint32 reserved_mc_trap_profile;
} SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA;

typedef struct
{
  uint32 incoming_vid_exist;
  uint32 incoming_tag_exist;
  uint32 incoming_stag_exist;
  uint32 acceptable_frame_type_action;
  uint32 outer_comp_index;
  uint32 inner_comp_index;
} SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA;

typedef struct
{
  uint32 ll_mirror_profile;
} SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA;

typedef struct
{
  uint32 ipv4_subnet_valid;
  uint32 ipv4_subnet_tc_valid;
  uint32 ipv4_subnet_tc;
  uint32 ipv4_subnet_vid;
  uint32 ipv4_subnet_mask;
  uint32 ipv4_subnet;
} SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA;

typedef struct
{
  uint32 traffic_class_valid;
  uint32 traffic_class;
  uint32 vid_valid;
  uint32 vid;
} SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA;

typedef struct
{
  uint32 dp;
  uint32 tc;
  uint32 valid;
} SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA;

typedef struct
{
  uint32 cpu_trap_code;
  uint32 snp;
  uint32 fwd;
} SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 format_specific_data;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif;
  uint32 out_lif_valid;
  uint32 identifier;
  uint32 p2p_service;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 learn_destination;
  uint32 isid_domain;
  uint32 learn_type;
  uint32 sa_drop;
  uint32 is_dynamic;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 mpls_label;
  uint32 mpls_command;
  uint32 p2p_service;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif;
  uint32 out_lif_valid;
  uint32 tpid_profile;
  uint32 has_cw;
  uint32 p2p_service;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 asd;
  uint32 is_dynamic;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 format_specific_data;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 adjacent;
  uint32 drop_if_vid_differ;
  uint32 permit_all_ports;
  uint32 override_vid_in_tagged;
  uint32 use_vid_in_untagged;
  uint32 is_dynamic;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 vid;
  uint32 accept_untagged;
  uint32 drop_if_vid_differ;
  uint32 permit_all_ports;
  uint32 override_vid_in_tagged;
  uint32 use_vid_in_untagged;
  uint32 is_dynamic;
} SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA;

typedef struct
{
  uint32 compare_valid;
  uint32 compare_key_16_msbs_data;
  uint32 compare_key_16_msbs_mask;
  uint32 compare_payload_data[2];
  uint32 compare_payload_mask[2];
  uint32 action_drop;
  uint32 action_transplant_payload_data[2];
  uint32 action_transplant_payload_mask[2];
} SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA;

typedef struct
{
  uint32 entry_count;
  uint32 profile_pointer;
} SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA;

typedef struct
{
  uint32 limit;
  uint32 interrupt_en;
  uint32 message_en;
  uint32 is_link_layer_fid;
} SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA;

typedef struct
{
  uint32 delete_entry[2];
  uint32 create_aged_out_event[2];
  uint32 create_refresh_event[2];
} SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA;

typedef struct
{
  uint32 pp_context_offset1;
  uint32 pp_context_offset2;
  uint32 pp_context_profile;
  uint32 pp_context_value_to_use;
} SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 tpid_profile;
  uint32 outer_header_start;
} SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA;

typedef struct
{
  uint32 pp_port_packet_format_qualifier0_value;
  uint32 pp_port_pp_context_value;
} SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA;

typedef struct
{
  uint32 pp_port_fem_bit_select;
} SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 pp_port_fem_map_index;
  uint32 pp_port_fem_map_data;
} SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 pp_port_fem_field_select[SOC_PB_PORTS_FEM_PP_PORT_SIZE];
} SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_bit_select;
} SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_map_index;
  uint32 src_system_port_fem_map_data;
} SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_field_select[SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE];
} SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 pp_context_fem_bit_select;
} SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 pp_context_fem_map_index;
  uint32 pp_context_fem_map_data;
} SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 pp_context_fem_field_select[SOC_PB_PORTS_FEM_PP_CONTEXT_SIZE];
} SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 next_addr_base;
  uint32 macro_sel;
} SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA;

typedef struct
{
  uint32 next_addr_base;
  uint32 macro_sel;
} SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA;

typedef struct
{
  uint32 parser_pmf_profile;
  uint32 key_program_profile;
  uint32 packet_format_code;
} SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA;

typedef struct
{
  uint32 cstm_word_select;
  uint32 cstm_mask_left;
  uint32 cstm_mask_right;
  uint32 cstm_condition_select;
  uint32 cstm_comparator_mode;
  uint32 cstm_comparator_mask;
  uint32 cstm_shift_sel;
  uint32 cstm_shift_a;
  uint32 cstm_shift_b;
} SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA;

typedef struct
{
  uint32 eth_type_protocol;
  uint32 eth_sap_protocol;
} SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 ip_protocol;
} SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 cstm_protocol;
} SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 isem_type;
  uint32 isem_stamp;
  uint32 isem_key[2];
  uint32 isem_payload;
  uint32 isem_age_status;
  uint32 isem_self;
  uint32 isem_refreshed_by_dsp;
} SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA;

typedef struct
{
  uint32 vsi_or_isid;
} SOC_PB_PP_IHP_VSI_ISID_TBL_DATA;

typedef struct
{
  uint32 my_mac_lsb;
  uint32 enable_my_mac;
} SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA;

typedef struct
{
  uint32 vsi_topology_id;
} SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA;

typedef struct
{
  uint32 fid_class[4];
} SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA;

typedef struct
{
  uint32 profile_index[4];
} SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA;

typedef struct
{
  uint32 bvd_topology_id;
} SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA;

typedef struct
{
  uint32 fid_class[4];
} SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA;

typedef struct
{
  uint32 profile_index[4];
} SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA;

typedef struct
{
  uint32 fid;
} SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA;

typedef struct
{
  uint32 vlan_range_upper_limit;
  uint32 vlan_range_lower_limit;
} SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA;

typedef struct
{
  uint32 range_valid0;
  uint32 range_valid1;
} SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA;

typedef struct
{
  uint32 designated_vlan;
} SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA;

typedef struct
{
  uint32 vlan_port_member_line[2];
} SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA;

typedef struct
{
  uint32 designated_vlan_index;
  uint32 vlan_domain;
  uint32 default_sem_base;
  uint32 default_sem_opcode;
  uint32 port_is_pbp;
  uint32 small_em_key_profile;
  uint32 learn_ac;
} SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 sem_result_table[2];
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA;

typedef struct
{
  uint32 out_lif;
  uint32 vlan_edit_vid;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 mef_l2_cp_profile;
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
  uint32 cos_profile;
  uint32 destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA;

typedef struct
{
  uint32 out_lif_or_vc_label;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 mef_l2_cp_profile;
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
  uint32 cos_profile;
  uint32 destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA;

typedef struct
{
  uint32 isid;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 mef_l2_cp_profile;
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
  uint32 cos_profile;
  uint32 destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 tt_learn_enable;
  uint32 orientation_is_hub;
  uint32 vlan_edit_vid;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 mef_l2_cp_profile;
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
  uint32 cos_profile;
  uint32 learn_destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 isidor_out_lifor_vc_label;
  uint32 tpid_profile_index;
  uint32 cos_profile;
  uint32 destination;
  uint32 service_type_lsb;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 destination_valid;
  uint32 orientation_is_hub;
  uint32 tt_learn_enable;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 destination;
  uint32 service_type_lsb;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 tt_learn_enable;
  uint32 tpid_profile;
  uint32 service_type;
  uint32 learn_destination;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA;

typedef struct
{
  uint32 in_rif;
  uint32 in_rif_valid;
  uint32 cos_profile;
  uint32 service_type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 model_is_pipe;
  uint32 destination_valid;
  uint32 tpid_profile;
  uint32 has_cw;
  uint32 valid;
  uint32 isidor_out_lifor_vc_label;
  uint32 destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 orientation_is_hub;
  uint32 tt_learn_enable;
  uint32 tpid_profile;
  uint32 has_cw;
  uint32 valid;
  uint32 learn_asd;
  uint32 learn_destination;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA;

typedef struct
{
  uint32 in_rif;
  uint32 in_rif_valid;
  uint32 model_is_pipe;
  uint32 valid;
  uint32 cos_profile;
  uint32 service_type;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA;

typedef struct
{
  uint32 in_rif;
  uint32 in_rif_valid;
  uint32 model_is_pipe;
  uint32 valid;
  uint32 cos_profile;
  uint32 service_type;
  uint32 type;
} SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA;

typedef struct
{
  uint32 ipv6_sem_offset;
  uint32 ipv6_add_offset_to_base;
  uint32 ipv6_opcode_valid;
  uint32 ipv4_sem_offset;
  uint32 ipv4_add_offset_to_base;
  uint32 ipv4_opcode_valid;
} SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 tc_dp_sem_offset;
  uint32 tc_dp_add_offset_to_base;
  uint32 tc_dp_opcode_valid;
} SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 pcp_dei_sem_offset;
  uint32 pcp_dei_add_offset_to_base;
  uint32 pcp_dei_opcode_valid;
} SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 per_port_stp_state[4];
} SOC_PB_PP_IHP_STP_TABLE_TBL_DATA;

typedef struct
{
  uint32 vrid_my_mac_map;
} SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA;

typedef struct
{
  uint32 mpls_label_range_encountered;
} SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA;

typedef struct
{
  uint32 mpls_tunnel_termination_valid;
} SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA;

typedef struct
{
  uint32 ip_over_mpls_exp_mapping;
} SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 incoming_tag_structure;
  uint32 pcp_dei_profile;
  uint32 ivec;
} SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA;

typedef struct
{
  uint32 llvp_prog_sel;
} SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA;

typedef struct
{
  uint32 key_inst0;
  uint32 key_inst1;
} SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA;

typedef struct
{
  uint32 key_inst2;
  uint32 key_inst3;
  uint32 and_mask;
  uint32 or_mask;
  uint32 sem_lookup_enable;
} SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA;

typedef struct
{
  uint32 key_inst0;
  uint32 key_inst1;
  uint32 key_inst2;
  uint32 key_inst3;
  uint32 and_mask;
  uint32 or_mask;
  uint32 sem_lookup_enable;
} SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA;

typedef struct
{
  uint32 key_inst0;
  uint32 key_inst1;
  uint32 and_mask;
  uint32 or_mask;
} SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA;

typedef struct
{
  uint32 ivec;
} SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA;

typedef struct
{
  uint32 pcp_dei;
} SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA;

typedef struct
{
  uint32 pbb_cfm_max_level;
  uint32 pbb_cfm_trap_valid;
} SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA;

typedef struct
{
  uint32 sem_result_accessed;
} SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA;

typedef struct
{
  uint32 vrf;
  uint32 uc_rpf_enable;
  uint32 enable_routing_mc;
  uint32 enable_routing_uc;
  uint32 enable_routing_mpls;
  uint32 in_rif_cos_profile;
} SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA;

typedef struct
{
  uint32 traffic_class;
  uint32 drop_precedence;
} SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA;

typedef struct
{
  uint32 physical_port_mine0;
  uint32 physical_port_mine1;
  uint32 physical_port_mine2;
  uint32 physical_port_mine3;
} SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA;

typedef struct
{
  uint32 lag_lb_key_count;
  uint32 lag_lb_key_start;
  uint32 lb_profile;
  uint32 ecmp_lb_key_count;
  uint32 lb_bos_search;
  uint32 lb_include_bos_hdr;
} SOC_PB_PP_IHB_PINFO_FER_TBL_DATA;

typedef struct
{
  uint32 ecmp_group_size;
} SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA;

typedef struct
{
  uint32 lb_vector_index3;
  uint32 lb_vector_index1;
  uint32 lb_vector_index5;
  uint32 lb_vector_index4;
  uint32 lb_vector_index2;
} SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA;

typedef struct
{
  uint32 chunk_size;
  uint32 chunk_bitmap[2];
} SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA;

typedef struct
{
  uint32 protection_pointer;
  uint32 ecmp_group_size_index;
} SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA;

typedef struct
{
  uint32 fec_entry[2];
} SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA;

typedef struct
{
  uint32 destination;
} SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif_or_dist_tree_nick;
  uint32 is_out_lif;
} SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif_lsb;
  uint32 uc_rpf_mode;
  uint32 mc_rpf_mode;
  uint32 rif;
  uint32 out_lif_type;
} SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif;
  uint32 out_vsi_lsb;
} SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA;

typedef struct
{
  uint32 fec_entry_accessed;
} SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA;

typedef struct
{
  uint32 path_select[8];
} SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA;

typedef struct
{
  uint32 destination_valid;
} SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA;

typedef struct
{
  uint32 fwd_act_trap;
  uint32 fwd_act_control;
  uint32 fwd_act_destination;
  uint32 fwd_act_destination_valid;
  uint32 fwd_act_destination_add_vsi;
  uint32 fwd_act_destination_add_vsi_shift;
  uint32 fwd_act_traffic_class;
  uint32 fwd_act_traffic_class_valid;
  uint32 fwd_act_drop_precedence;
  uint32 fwd_act_drop_precedence_valid;
  uint32 fwd_act_meter_pointer;
  uint32 fwd_act_meter_pointer_selector;
  uint32 fwd_act_counter_pointer;
  uint32 fwd_act_counter_pointer_selector;
  uint32 fwd_act_drop_precedence_meter_command;
  uint32 fwd_act_drop_precedence_meter_command_valid;
  uint32 fwd_act_fwd_offset_index;
  uint32 fwd_act_fwd_offset_index_valid;
  uint32 fwd_act_ethernet_policer_pointer;
  uint32 fwd_act_ethernet_policer_pointer_valid;
  uint32 fwd_act_learn_disable;
} SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 snoop_action;
} SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA;

typedef struct
{
  uint32 action_profile_sa_not_found_index;
  uint32 learn_enable;
  uint32 action_profile_sa_drop_index;
  uint32 enable_unicast_same_interface_filter;
  uint32 program_translation_profile;
  uint32 sa_lookup_enable;
  uint32 action_profile_da_not_found_index;
  uint32 transparent_p2p_service_enable;
} SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA;

typedef struct
{
  uint32 program11;
  uint32 program10;
  uint32 program01;
  uint32 program00;
} SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA;

typedef struct
{
  uint32 key_a_instruction0;
  uint32 key_a_instruction1;
  uint32 key_b_instruction0;
  uint32 key_b_instruction1;
  uint32 key_b_instruction2;
  uint32 key_b_instruction3;
  uint32 elk_lkp_valid;
  uint32 lem_1st_lkp_valid;
  uint32 lem_1st_lkp_key_select;
  uint32 lem_1st_lkp_key_type;
  uint32 lem_1st_lkp_and_value;
  uint32 lem_1st_lkp_or_value;
  uint32 lem_2nd_lkp_valid;
  uint32 lem_2nd_lkp_key_select;
  uint32 lem_2nd_lkp_and_value;
  uint32 lem_2nd_lkp_or_value;
  uint32 lpm_1st_lkp_valid;
  uint32 lpm_1st_lkp_and_value;
  uint32 lpm_1st_lkp_or_value;
  uint32 lpm_2nd_lkp_valid;
  uint32 lpm_2nd_lkp_and_value;
  uint32 lpm_2nd_lkp_or_value;
  uint32 tcam_key_size;
  uint32 tcam_bank_valid;
  uint32 tcam_and_value;
  uint32 tcam_or_value;
  uint32 data_processing_profile;
} SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA;

typedef struct
{
  uint32 cpu_trap_code_lsb;
  uint32 snp;
  uint32 fwd;
} SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 l3vpn_default_routing;
} SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA;

typedef struct
{
  uint32 header_profile_learn_disable;
  uint32 header_profile_always_add_pph_learn_ext;
  uint32 header_profile_st_vsq_ptr_tc_mode;
  uint32 header_profile_build_pph;
  uint32 header_profile_ftmh_pph_present;
  uint32 header_profile_build_ftmh; /* Taken from TM */
} SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA;

typedef struct
{
  uint32 snp_act_snp_sampling_probability;
} SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 mrr_act_mrr_sampling_probability;
} SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 lpm;
} SOC_PB_PP_IHB_LPM_TBL_DATA;


typedef struct
{
  uint32 pinfo_pmf_key_gen_var;
} SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA;

typedef struct
{
  uint32 program_selection_map;
} SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA;

typedef struct
{
  uint32 first_pass_key_profile_index;
  uint32 second_pass_key_profile_index;
  uint32 tag_selection_profile_index;
  uint32 bytes_to_remove_header;
  uint32 bytes_to_remove_offset;
  uint32 system_header_profile;
  uint32 copy_program_variable;
} SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA;

typedef struct
{
  uint32 inst_valid;
  uint32 inst_source_select;
  uint32 inst_header_offset_select;
  uint32 inst_niblle_field_offset;
  uint32 inst_bit_count;
} SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 inst0_valid;
  uint32 inst0_source_select;
  uint32 inst0_header_offset_select;
  uint32 inst0_niblle_field_offset;
  uint32 inst0_bit_count;
  uint32 inst1_valid;
  uint32 inst1_source_select;
  uint32 inst1_header_offset_select;
  uint32 inst1_niblle_field_offset;
  uint32 inst1_bit_count;
} SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 bank_key_select[SOC_PB_TCAM_NOF_BANKS];
  uint32 bank_db_id_and_value[SOC_PB_TCAM_NOF_BANKS];
  uint32 bank_db_id_or_value[SOC_PB_TCAM_NOF_BANKS];
  uint32 tcam_pd1_members;
  uint32 tcam_pd2_members;
  uint32 tcam_sel3_member;
  uint32 tcam_sel4_member;
} SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 direct_action_table;
} SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 mask[SOC_PB_PP_IHB_TCAM_DATA_WIDTH];
  uint32 value[SOC_PB_PP_IHB_TCAM_DATA_WIDTH];
  uint32 valid;
} SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA;

typedef struct
{
  uint32 found;
  uint32 address;
} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_COMPARE_DATA;

typedef struct
{
  uint32 vbe;   /* whether to write/read vbi.*/
  uint32 mskdatae; /* mskdatae  to read/write the data/mask or not. */
  uint32 wr; /* set for Write*/
  uint32 rd; /* set for Read*/
  uint32 cmp; /* set for compare */
  uint32 flush; /* set for clear all entries */
  uint32 val[SOC_PB_PP_IHB_TCAM_DATA_WIDTH]; /* value for write/compare */
  uint32 vbi; /* valid or not valid entry. */
  uint32 cfg; /* 1 --> 144, 2 --> 72 */
  uint32 hqsel; /* 144 --> 1111, 72 --> 0011 for first, 1100 for second)*/
  uint32 mask[SOC_PB_PP_IHB_TCAM_DATA_WIDTH]; /* mask */
} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_TBL_SET_DATA;

typedef struct
{
  uint32 vbo;  /*   is it valid entry! output for read.*/
  uint32 dataout[SOC_PB_PP_IHB_TCAM_DATA_WIDTH];  /*value of the entry*/
  uint32 hadr;  /*address offset in the bank. in sub words offset.*/
  uint32 hit;  /*found or not. for compare.*/
  uint32 mhit;  /*always zero.*/
  uint32 mask[SOC_PB_PP_IHB_TCAM_DATA_WIDTH];  /*mask*/
} __ATTRIBUTE_PACKED__ SOC_PB_PP_IHB_TCAM_TBL_GET_DATA;

typedef struct
{
  uint32 low;
  uint32 high;
} SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA;

typedef struct
{
  uint32 out_pp_port;
  uint32 mtu;
  uint32 dst_system_port_id;
  uint32 lb_key_max;
  uint32 lb_key_min;
  uint32 is_stacking_port;
  uint32 peer_tm_domain_id;
  uint32 port_type;
  uint32 cnm_intrcpt_fc_vec_index_5_0;
  uint32 cnm_intrcpt_fc_vec_index_12_6;
  uint32 cnm_intrcpt_fc_vec_index_13_13;
  uint32 cnm_intrcpt_en;
  uint32 ad_count_out_port_flag;
} SOC_PB_PP_EGQ_PPCT_TBL_DATA;

typedef struct
{
  uint32 pvlan_port_type;
  uint32 orientation_is_hub;
  uint32 eei_type;
  uint32 unknown_bc_da_action_filter;
  uint32 unknown_mc_da_action_filter;
  uint32 unknown_uc_da_action_filter;
  uint32 enable_src_equal_dst_filter;
  uint32 acceptable_frame_type_profile;
  uint32 egress_vsi_filtering_enable;
  uint32 disable_filtering;
  uint32 acl_profile;
  uint32 acl_data;
  uint32 disable_learning;
  uint32 port_is_pbp;
} SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA;

typedef struct
{
  uint32 tpid_profile_link;
  uint32 edit_command_outer_vid_source;
  uint32 edit_command_outer_pcp_dei_source;
  uint32 edit_command_inner_vid_source;
  uint32 edit_command_inner_pcp_dei_source;
  uint32 edit_command_bytes_to_remove;
  uint32 outer_tpid_ndx;
  uint32 inner_tpid_ndx;
} SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA;

typedef struct
{
  uint32 vsi_membership[2];
} SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA;

typedef struct
{
  uint32 ttl_scope;
} SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA;

typedef struct
{
  uint32 auxtable;
} SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA;

typedef struct
{
  uint32 eep_orientation;
} SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA;

typedef struct
{
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
} SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA;

typedef struct
{
  /* TBL DEF CHANGE: Replaced single 33b field with actual fields */
  uint32 otm_valid;
  uint32 otm;
  uint32 discard;
  uint32 tc;
  uint32 tc_valid;
  uint32 dp;
  uint32 dp_valid;
  uint32 cud;
  uint32 cud_valid;
} SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA;

typedef struct
{
  /* TBL DEF CHANGE: Added mpls2 fields*/
  uint32 next_eep;
  uint32 next_vsi_lsb;
  /* Two fields below are valid if MPLS1-Command is push-0 through push-7 */
  uint32 mpls2_label;
  uint32 mpls2_command;
  /* Else, two fields below are valid if MPLS1-Command is pop-into-Eth */
  uint32 has_cw;
  uint32 tpid_profile;
  /* Else, upper fields above are not valid */
  uint32 mpls1_label;
  uint32 mpls1_command;
} SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA;

typedef struct
{
  uint32 next_eep;
  uint32 next_vsi_lsb;
  uint32 gre_enable;
  uint32 ipv4_tos_index;
  uint32 ipv4_ttl_index;
  uint32 ipv4_src_index;
  uint32 ipv4_dst;
} SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA;

typedef struct
{
  uint32 vid;
  uint32 vid_valid;
  uint32 dest_mac[2];
} SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA;

typedef struct
{
  uint32 entry[3];
} SOC_PB_PP_EPNI_ENCAPSULATION_DB_BANK2_TBL_DATA;

typedef struct
{
  uint32 entry[2];
} SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA;

typedef struct
{
  uint32 egress_stp_state[2];
} SOC_PB_PP_EPNI_STP_TBL_DATA;

typedef struct
{
  uint32 pcp_dei_profile;
  uint32 vid[2];
  uint32 vlan_edit_profile;
} SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA;

typedef struct
{
  uint32 pcp_enc_table;
} SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA;

typedef struct
{
  uint32 acceptable_frame_type_profile;
  uint32 port_is_pbp;
  uint32 egress_stp_filter_enable;
  uint32 egress_vsi_filter_enable;
  uint32 disable_filter;
  uint32 default_sem_result_0;
  uint32 default_sem_result_14_1;
  uint32 exp_map_profile;
  uint32 c_tpid_index;
  uint32 s_tpid_index;
  uint32 tag_profile;
  uint32 vlan_domain;
  uint32 eei_type;
  uint32 cep_c_vlan_edit;
  uint32 llvp_profile;
  uint32 mpls_ethertype_select;
  uint32 count_mode;
  uint32 count_enable;
  uint32 counter_compensation;
} SOC_PB_PP_EPNI_PP_PCT_TBL_DATA;

typedef struct
{
  uint32 llvp_c_tag_offset;
  uint32 llvp_packet_has_c_tag;
  uint32 llvp_packet_has_up;
  uint32 llvp_packet_has_pcp_dei;
  uint32 llvp_incoming_tag_format;
} SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA;

typedef struct
{
  uint32 outer_tpid_index;
  uint32 inner_tpid_index;
  uint32 outer_vid_source;
  uint32 inner_vid_source;
  uint32 outer_pcp_dei_source;
  uint32 inner_pcp_dei_source;
  uint32 tags_to_remove;
} SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA;

typedef struct
{
  uint32 program_index;
  uint32 new_header_size;
  uint32 add_network_header;
  uint32 remove_network_header;
  uint32 system_header_size;
} SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA;

typedef struct
{
  uint32 tpid_profile_link;
  uint32 edit_command_outer_vid_source;
  uint32 edit_command_outer_pcp_dei_source;
  uint32 edit_command_inner_vid_source;
  uint32 edit_command_inner_pcp_dei_source;
  uint32 edit_command_bytes_to_remove;
  uint32 outer_tpid_ndx;
  uint32 inner_tpid_ndx;
} SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA;

typedef struct
{
  uint32 ip_exp_map;
} SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA;

typedef struct
{
  uint32 etpp_debug;
} SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA;

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
  soc_pb_pp_tbl_access_init_unsafe(
    SOC_SAND_IN  int             unit
  );

uint32
  soc_pb_pp_reps_for_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

uint32
  soc_pb_pp_ihp_port_mine_table_physical_port_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_port_mine_table_physical_port_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_port_protocol_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tos_2_cos_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tos_2_cos_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_reserved_mc_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_reserved_mc_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tm_port_pp_context_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tm_port_pp_context_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_info_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_info_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_values_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_values_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_program1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_program1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_program2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_program2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_packet_format_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_packet_format_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_isem_management_request_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_isem_management_request_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_ISID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_topology_id_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_topology_id_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_da_not_found_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_da_not_found_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_topology_id_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_topology_id_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_fid_class_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_fid_class_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_da_not_found_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_bvd_da_not_found_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_stp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_stp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_mpls_label_range_encountered_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_llvp_prog_sel_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pbb_cfm_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_pbb_cfm_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_in_rif_config_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_in_rif_config_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_FER_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_ecmp_group_size_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_ecmp_group_size_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lb_pfc_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_general_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_ip_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_ip_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_default_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_default_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_path_select_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_path_select_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_destination_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_destination_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_snoop_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_translation_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_translation_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_unknown_da_action_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_unknown_da_action_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_vrf_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_vrf_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_header_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_header_profile_tbl2stream_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL  *tbl,
    SOC_SAND_OUT uint32  *data
  );

uint32
  soc_pb_pp_ihb_header_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_snp_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_snp_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_mrr_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_mrr_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lpm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LPM_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LPM_TBL_DATA  *tbl_data
  );

/* $Id: soc_pb_pp_tbl_access.h,v 1.6 Broadcom SDK $
 * Read indirect table tcam_bank_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 *
 * entry_offset is a relative offset within a bank
 */
uint32
  soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA  *tbl_data
  );

/*
 *	Generic access functions
 */

uint32
  soc_pb_pp_ihb_program_selection_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              map_tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              map_tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

/*
 *	End of generic functions
 */

uint32
  soc_pb_pp_ihb_program_selection_map0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map3_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map3_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map4_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map4_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map5_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map5_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map6_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map6_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map7_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_selection_map7_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA  *tbl_data
  );

/*
 *	Generic access function
 */

uint32
  soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pmf_key_ndx,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  uint32              ce_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pmf_key_ndx,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  uint32              ce_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

/*
 *	End of generic access functions
 */

uint32
  soc_pb_pp_ihb_key_a_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint32              tbl_ndx,
   SOC_SAND_OUT SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_key_a_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
   SOC_SAND_IN  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_key_b_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_key_b_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  );

/*
 *	Generic function to access the TCAM PMF table
 */
uint32
  soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              db_profile_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
    );

uint32
  soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              db_profile_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 *	End of generic function
 */

uint32
  soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_direct_action_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_direct_action_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_tcam_bank_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_TBL_GET_DATA  *tbl_data
  );

/*
 * Write indirect table tcam_bank_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 *
 * entry_offset is a relative offset within a bank
 */
uint32
  soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA  *tbl_data
  );

/*
 * Read indirect table tcam_action_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 *
 * entry_offset is a relative offset within a bank
 */
uint32
  soc_pb_pp_ihb_tcam_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_ihb_tcam_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_PPCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_PPCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_vsi_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_vsi_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ttl_scope_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_ttl_scope_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_aux_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_aux_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_eep_orientation_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_eep_orientation_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_cfm_trap_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_cfm_trap_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_action_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_egq_action_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_tx_tag_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_tx_tag_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_stp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_STP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_stp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_STP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_small_em_result_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_small_em_result_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_pcp_dei_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_pcp_dei_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_pp_pct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PP_PCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_pp_pct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_llvp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_llvp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_program_vars_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_program_vars_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_etpp_debug_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_pp_epni_etpp_debug_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA  *tbl_data
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_tbl_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_tbl_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_tbl_access_get_procs_ptr(void);
/*
 * Read to Tcam Banks from block IHB,
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_OUT  SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA    *data
  );

/*
 * Write to Tcam Banks from block IHB,
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_IN   SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA     *data
  );


/*
 * Set indirect table Tcam Bank A from block IHB,
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_valid_bit_tbl_set_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_IN  uint8                           valid
  );

/*
 * Get indirect table Tcam Bank A from block IHB,
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_valid_bit_tbl_get_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_OUT  uint8                           *valid
  );

/*
 * Flush all entries of the TCAM bank
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_flush_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx
  );

/*
 * compare command by the CPU of the TCAM bank entries.
 * doesn't take semaphore.
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_compare_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA     *compare_data,
    SOC_SAND_OUT  SOC_PB_PP_IHB_TCAM_COMPARE_DATA      *found_data
  );

/* } */

/*********************************************************************
* NAME:
 *   soc_pb_pp_tbl_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_tbl_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_tbl_access_get_errs_ptr(void);

/* } */
                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
/* } __SOC_PB_PP_TBL_ACCESS_H_INCLUDED__*/
#endif
