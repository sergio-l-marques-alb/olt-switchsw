/* $Id: pb_pmf_low_level.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_pmf_low_level.h
*
* MODULE PREFIX:  soc_pb_pp
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>

#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_fem_tag.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_pgm.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of banks per TCAM.                       */
#define  SOC_PB_PMF_TCAM_NOF_BANKS (SOC_TMC_PMF_TCAM_NOF_BANKS)

/*     Number of final TCAM lookup results.                    */
#define  SOC_PB_PMF_TCAM_NOF_LKP_RESULTS (SOC_TMC_PMF_TCAM_NOF_LKP_RESULTS)

/*     Number of FEM selected bits.                            */

/*     Maximal number of bits in the FEM output result (action
*     value).                                                 */

/*     Number of PMF Programs.                                 */
#define  SOC_PB_PMF_NOF_PGMS (SOC_TMC_PMF_NOF_PGMS)

/*     Number of cycles in the PMF process.                    */
#define  SOC_PB_PMF_NOF_CYCLES (SOC_TMC_PMF_NOF_CYCLES)

/*     Number of lookup profiles in the PMF process.           */
#define SOC_PB_PMF_LOW_LEVEL_NOF_DBS_PER_LOOKUP_PROFILE_MAX      (4)

#define SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES                       (SOC_TMC_PMF_LOW_LEVEL_NOF_DATABASES)


#define SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX                    (31)
#define SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX                        (1)
#define SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX                         (7)
#define SOC_PB_PMF_LOW_LEVEL_IRPP_FIELD_MAX                     (SOC_PB_NOF_PMF_IRPP_INFO_FIELDS-1)
#define SOC_PB_PMF_LOW_LEVEL_DB_ID_NDX_MAX                      (SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES-1)
#define SOC_PB_PMF_LOW_LEVEL_KEY_FORMAT_MAX                     (SOC_PB_NOF_PMF_TCAM_KEY_SRCS-1)
#define SOC_PB_PMF_LOW_LEVEL_KEY_SRC_MAX                        (SOC_PB_NOF_PMF_DIRECT_TBL_KEY_SRCS-1)
#define SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_MAX                      (1023)
#define SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_NDX_MAX                (SOC_PB_PMF_NOF_TAG_PROFILES - 1)
#define SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_MAX                   (SOC_PB_NOF_PMF_TAG_TYPES-1)
#define SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX                    (3)
#define SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX              (15)
#define SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX               (3)
#define SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_MAX                     (SOC_PB_NOF_PMF_CE_SUB_HEADERS-1)
#define SOC_PB_PMF_LOW_LEVEL_OFFSET_MIN                         (-512)
#define SOC_PB_PMF_LOW_LEVEL_OFFSET_MAX                         (508)
#define SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MIN                       (1)
#define SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX                       (32)
#define SOC_PB_PMF_LOW_LEVEL_DB_ID_MAX                          (SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES-1)
#define SOC_PB_PMF_LOW_LEVEL_ENTRY_ID_MAX                       (16*1024-1)
#define SOC_PB_PMF_LOW_LEVEL_PRIORITY_MAX                       (16*1024-1)
#define SOC_PB_PMF_LOW_LEVEL_ID_MAX                             (7)
#define SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX                         (SOC_PB_PMF_LOW_LEVEL_NOF_FEMS-1)
#define SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX                      (1)
#define SOC_PB_PMF_LOW_LEVEL_PGM_ID_MAX                         (31)
#define SOC_PB_PMF_LOW_LEVEL_SRC_MAX                            (SOC_PB_NOF_PMF_FEM_INPUT_SRCS-1)
#define SOC_PB_PMF_LOW_LEVEL_VAL_SRC_MAX                        (SOC_PB_NOF_PMF_TAG_VAL_SRCS-1)
#define SOC_PB_PMF_LOW_LEVEL_STAT_TAG_LSB_POSITION_MAX          ((1<<5)-1)
#define SOC_PB_PMF_LOW_LEVEL_SIZE_MAX                           (SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES-1)
#define SOC_PB_PMF_LOW_LEVEL_MODE_MAX                           (SOC_PB_NOF_PMF_TCAM_FLD_L3_MODES-1)
#define SOC_PB_PMF_LOW_LEVEL_IPV4_FLD_MAX                       (SOC_PB_NOF_PMF_FLD_IPV4S-1)
#define SOC_PB_PMF_LOW_LEVEL_IPV6_FLD_MAX                       (SOC_PB_NOF_PMF_FLD_IPV6S-1)
#define SOC_PB_PMF_LOW_LEVEL_LOC_MAX                            (SOC_PB_NOF_PMF_TCAM_FLD_A_B_LOCS-1)
#define SOC_PB_PMF_LOW_LEVEL_IRPP_FLD_MAX                       (SOC_PB_NOF_PMF_IRPP_INFO_FIELDS-1)
#define SOC_PB_PMF_LOW_LEVEL_PMF_PGM_ID_MAX                     (SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX)
#define SOC_PB_PMF_LOW_LEVEL_L2_MAX                             (SOC_PB_NOF_PMF_TCAM_FLD_L2S-1)
#define SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_MIN                    (3)
#define SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_MAX                    (31)
#define SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_ID_MAX                (3)
#define SOC_PB_PMF_LOW_LEVEL_MAP_DATA_MAX                       (15)
#define SOC_PB_PMF_LOW_LEVEL_TYPE_MAX                           (SOC_PB_NOF_PMF_FEM_BIT_LOC_TYPES-1)
#define SOC_PB_PMF_LOW_LEVEL_BASE_VALUE_MAX                     (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_PFQ_MAX                            (3)
#define SOC_PB_PMF_LOW_LEVEL_SEM_13_8_NDX_MAX                   (63)
#define SOC_PB_PMF_LOW_LEVEL_FWD_MAX                            (SOC_TMC_NOF_PKT_FRWRD_TYPES-1)
#define SOC_PB_PMF_LOW_LEVEL_TTC_MAX                            (SOC_TMC_NOF_TUNNEL_TERM_CODES-1)
#define SOC_PB_PMF_LOW_LEVEL_PRSR_MAX                           (15)
#define SOC_PB_PMF_LOW_LEVEL_PORT_PMF_MAX                       (7)
#define SOC_PB_PMF_LOW_LEVEL_LLVP_MAX                           (15)
#define SOC_PB_PMF_LOW_LEVEL_PMF_PRO_MAX                        (7)
#define SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_15_8_MAX                (255)
#define SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_7_0_MAX                 (255)
#define SOC_PB_PMF_LOW_LEVEL_SEM_7_0_NDX_MAX                    (255)
#define SOC_PB_PMF_LOW_LEVEL_HEADER_TYPE_MAX                    (SOC_PB_NOF_PMF_PGM_BYTES_TO_RMV_HDRS-1)
#define SOC_PB_PMF_LOW_LEVEL_NOF_BYTES_MAX                      (31)
#define SOC_PB_PMF_LOW_LEVEL_LKP_PROFILE_ID_MAX                 (SOC_PB_PMF_NOF_LKP_PROFILE_IDS - 1)
#define SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_ID_MAX                 (7)
#define SOC_PB_PMF_LOW_LEVEL_COPY_PGM_VAR_MAX                   (255)
#define SOC_PB_PMF_LOW_LEVEL_FC_TYPE_MAX                        (SOC_PB_PORTS_NOF_FC_TYPES-1)

#define SOC_PB_PMF_LOW_LEVEL_AF_TYPE_MAX                        (SOC_PB_NOF_PMF_FEM_ACTION_TYPES-1)
#define SOC_PB_PMF_LOW_LEVEL_PGM_TYPE_MAX                       (SOC_PB_NOF_PMF_PGM_SEL_TYPES-1)
#define SOC_PB_PMF_FEM_SEL_BITS_SIZE_IN_BITS                    (SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS)
#define SOC_PB_PMF_FEM_FOR_OFFSET_MIN                           (2)
#define SOC_PB_PMF_FEM_MAP_DATA_ENCODED_BIT                     (5)
#define SOC_PB_PMF_FEM_CST_ENCODED_BIT                          (0x1F)
#define SOC_PB_PMF_FEM_CST_ENCODED_BIT_MSB                      (5)
#define SOC_PB_PMF_FEM_CST_ENCODED_BIT_LSB                      (1)
#define SOC_PB_PMF_FEM_MAP_DATA_ENCODING                        (0x8)
#define SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8_SIZE_IN_BITS   (6)
#define SOC_PB_PMF_PGM_SEL_TYPE_TTC_CODE_SIZE_IN_BITS           (4)
#define SOC_PB_PMF_PGM_SEL_PORT_PMF_SIZE_IN_BITS                (3)
#define SOC_PB_PMF_PGM_SEL_PMF_PROFILE_SIZE_IN_BITS             (3)
#define SOC_PB_PMF_LOW_LEVEL_DIRECT_TBL_VAL_MAX                 ((1<<20)-1)
#define SOC_PB_PMF_DIRECT_TBL_BITWISE_AND_VAL                   (0x3)
#define SOC_PB_PMF_CE_OFFSET_RESOLUTION                         (4)
#define SOC_PB_PMF_CE_OFFSET_NIBBLE_VAL_SIZE_IN_BITS            (7)
#define SOC_PB_PMF_TCAM_PREFIX_MSB                              (3)
#define SOC_PB_PMF_TCAM_PREFIX_SIZE_IN_BITS                     (4)
#define SOC_PB_PMF_CE_INSTRUCTION_NOF_BITS_MAX                  (32)
#define SOC_PB_PMF_LOW_LEVEL_FEM_SIZE_MAX                       (17)


#define SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS_SIZE_IN_BITS              (72)
#define SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS             (144)
#define SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS_SIZE_IN_BITS             (288)


#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(pph, ingress_shape) ((pph) + ((ingress_shape) << 1))
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_REGULAR             SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(FALSE, FALSE)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_PPH_PRESENT         SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(TRUE,  FALSE)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_IS                  SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(FALSE, TRUE)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_IS_PPH_PRESENT      SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(TRUE,  TRUE)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_IS_ON(pmf_profile)  ((((pmf_profile) & 0x2) != 0) ? TRUE : FALSE)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_PP                     (0xf)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_RAW                    (7)
#define SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_FTMH                   (8)

#define SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_ETH               (0) /* In fact, from 0 to 4 according to the PFG (FP) */
#define SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW               (5)
#define SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM                (6)
#define SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING          (7)


/* Tag-Profile = Lookup-Profile */
#define SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_RAW                  (SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW)
#define SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_TM                   (SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM)
#define SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_STACKING             (SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING)

#define SOC_PB_PMF_FEM_PGM_FOR_TM                               (0)
#define SOC_PB_PMF_FEM_PGM_FOR_STACK                            (1)
#define SOC_PB_PMF_FEM_PGM_FOR_ETH                              (2)
#define SOC_PB_PMF_FEM_PGM_FOR_RAW                              (3)

#define SOC_PB_PMF_CE_PACKET_HEADER_FLD_VAL                     (0x0)
#define SOC_PB_PMF_CE_IRPP_INFO_FLD_VAL                         (0x1)

#define SOC_PB_PMF_FEM_ACTION_TYPE_DEST_FLD_VAL                 (0)
#define SOC_PB_PMF_FEM_ACTION_TYPE_DP_FLD_VAL                   (1)
#define SOC_PB_PMF_FEM_ACTION_TYPE_TC_FLD_VAL                   (2)
#define SOC_PB_PMF_FEM_ACTION_TYPE_TRAP_FLD_VAL                 (3)
#define SOC_PB_PMF_FEM_ACTION_TYPE_SNP_FLD_VAL                  (4)
#define SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR_FLD_VAL               (5)
#define SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS_FLD_VAL              (6)
#define SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC_FLD_VAL              (7)
#define SOC_PB_PMF_FEM_ACTION_TYPE_IS_FLD_VAL                   (8)
#define SOC_PB_PMF_FEM_ACTION_TYPE_METER_FLD_VAL                (9)
#define SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER_FLD_VAL              (10)
#define SOC_PB_PMF_FEM_ACTION_TYPE_STAT_FLD_VAL                 (11)
#define SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA_FLD_VAL        (13)
#define SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM_FLD_VAL         (12)
#define SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF_FLD_VAL               (14)
#define SOC_PB_PMF_FEM_ACTION_TYPE_NOP_FLD_VAL                  (15)

#define SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8_TBL_NDX         (0)
#define SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0_TBL_NDX          (1)
#define SOC_PB_PMF_PGM_SEL_TYPE_SEM_NDX_7_0_TBL_NDX             (2)
#define SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8_TBL_NDX        (3)
#define SOC_PB_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE_TBL_NDX         (4)
#define SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO_TBL_NDX          (6)
#define SOC_PB_PMF_PGM_SEL_TYPE_LOOKUPS_TBL_NDX                 (5)
#define SOC_PB_PMF_PGM_SEL_TYPE_LLVP_PFC_TBL_NDX                (7)

#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START_FLD_VAL           (0)
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST_FLD_VAL             (1)
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING_FLD_VAL          (2)
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING_FLD_VAL     (3)


#define SOC_PB_PMF_TAG_VAL_SRC_A_29_0_FLD_VAL                   (0)
#define SOC_PB_PMF_TAG_VAL_SRC_A_61_32_FLD_VAL                  (1)
#define SOC_PB_PMF_TAG_VAL_SRC_B_29_0_FLD_VAL                   (2)
#define SOC_PB_PMF_TAG_VAL_SRC_B_61_32_FLD_VAL                  (3)
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_0_FLD_VAL                   (4)
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_1_FLD_VAL                   (5)
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_2_FLD_VAL                   (6)
#define SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL_FLD_VAL                  (7)

#define SOC_PB_PMF_FEM_INPUT_SRC_A_31_0_FLD_VAL                 (5)
#define SOC_PB_PMF_FEM_INPUT_SRC_A_47_16_FLD_VAL                (6)
#define SOC_PB_PMF_FEM_INPUT_SRC_A_63_32_FLD_VAL                (7)
#define SOC_PB_PMF_FEM_INPUT_SRC_A_79_48_FLD_VAL                (8)
#define SOC_PB_PMF_FEM_INPUT_SRC_A_95_64_FLD_VAL                (9)
#define SOC_PB_PMF_FEM_INPUT_SRC_B_31_0_FLD_VAL                 (10)
#define SOC_PB_PMF_FEM_INPUT_SRC_B_47_16_FLD_VAL                (11)
#define SOC_PB_PMF_FEM_INPUT_SRC_B_63_32_FLD_VAL                (12)
#define SOC_PB_PMF_FEM_INPUT_SRC_B_79_48_FLD_VAL                (13)
#define SOC_PB_PMF_FEM_INPUT_SRC_B_95_64_FLD_VAL                (14)
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0_FLD_VAL                 (0)
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1_FLD_VAL                 (1)
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2_FLD_VAL                 (2)
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3_FLD_VAL                 (3)
#define SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL_FLD_VAL                (4)
#define SOC_PB_PMF_FEM_INPUT_SRC_NOP_FLD_VAL                    (15)

#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0_FLD_VAL             (0)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10_FLD_VAL           (1)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20_FLD_VAL           (2)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30_FLD_VAL           (3)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0_FLD_VAL             (4)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10_FLD_VAL           (5)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20_FLD_VAL           (6)
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30_FLD_VAL           (7)


#define SOC_PB_PMF_CE_SUB_HEADER_0_FLD_VAL                      (0)
#define SOC_PB_PMF_CE_SUB_HEADER_1_FLD_VAL                      (1)
#define SOC_PB_PMF_CE_SUB_HEADER_2_FLD_VAL                      (2)
#define SOC_PB_PMF_CE_SUB_HEADER_3_FLD_VAL                      (3)
#define SOC_PB_PMF_CE_SUB_HEADER_4_FLD_VAL                      (4)
#define SOC_PB_PMF_CE_SUB_HEADER_5_FLD_VAL                      (5)
#define SOC_PB_PMF_CE_SUB_HEADER_FWD_FLD_VAL                    (6)
#define SOC_PB_PMF_CE_SUB_HEADER_FWD_POST_FLD_VAL               (7)

#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_FLD_VAL                     (0)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16_FLD_VAL                    (0)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32_FLD_VAL                    (0)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32_FLD_VAL                    (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT_FLD_VAL                 (10)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT_FLD_VAL									  (12)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32_FLD_VAL								  (12)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE_FLD_VAL           (14)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0_FLD_VAL            (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1_FLD_VAL            (18)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2_FLD_VAL            (20)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3_FLD_VAL            (22)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4_FLD_VAL            (24)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5_FLD_VAL            (26)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_FLD_VAL         (28)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_FLD_VAL    (27)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT_FLD_VAL      (29)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0_FLD_VAL         (30)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1_FLD_VAL         (32)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2_FLD_VAL         (34)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3_FLD_VAL         (36)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4_FLD_VAL         (38)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5_FLD_VAL         (40)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC_FLD_VAL              (44)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT_FLD_VAL                 (45)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH_FLD_VAL        (46)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID_FLD_VAL      (51)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID_FLD_VAL        (52)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID_FLD_VAL        (53)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL_FLD_VAL   (57)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE_FLD_VAL   (59)
#define SOC_PB_PMF_IRPP_INFO_FIELD_UP_FLD_VAL                         (60)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH_FLD_VAL             (61)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE_FLD_VAL                 (63)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE_FLD_VAL          (64)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX_FLD_VAL                    (68)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VSI_FLD_VAL                        (72)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI_FLD_VAL                 (76)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB_FLD_VAL         (77)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG_FLD_VAL              (83)
#define SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE_FLD_VAL                  (84)
#define SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC_FLD_VAL                    (85)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT_FLD_VAL          (86)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX_FLD_VAL          (87)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE_FLD_VAL                (88)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE_FLD_VAL    (89)
#define SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF_FLD_VAL              (96)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF_FLD_VAL                     (99)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VRF_FLD_VAL                        (101)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC_FLD_VAL            (102)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL_FLD_VAL                     (104)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP_FLD_VAL                (106)
#define SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID_FLD_VAL          (110)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO_FLD_VAL                 (137)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST_FLD_VAL                 (141)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD_FLD_VAL                  (147)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT_FLD_VAL          (158)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD_FLD_VAL  (165)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST_FLD_VAL (169)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD_FLD_VAL  (176)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST_FLD_VAL (180)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT_FLD_VAL      (185)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT_FLD_VAL      (190)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT_FLD_VAL      (194)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT_FLD_VAL      (198)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT_FLD_VAL         (203)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT_FLD_VAL   (204)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE_FLD_VAL         (205)
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE_FLD_VAL            (206)
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR_FLD_VAL                (214)
#define SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE_FLD_VAL          (216)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE_FLD_VAL          (217)
#define SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS_FLD_VAL                     (223)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1_FLD_VAL                    (231)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2_FLD_VAL                    (234)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO_FLD_VAL          (238)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3_FLD_VAL                    (239)

#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_SIZE                     (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16_SIZE                    (15)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32_SIZE                    (31)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32_SIZE                    (31)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT_SIZE                 (6)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT_SIZE									   (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32_SIZE								   (31)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE_SIZE           (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5_SIZE            (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_SIZE         (6)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_SIZE    (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT_SIZE      (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0_SIZE         (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1_SIZE         (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2_SIZE         (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3_SIZE         (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4_SIZE         (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5_SIZE         (7)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC_SIZE              (13)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT_SIZE                 (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH_SIZE        (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID_SIZE      (19)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID_SIZE        (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID_SIZE        (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE_SIZE   (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL_SIZE   (14)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE_SIZE         (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_SIZE         (6)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_SIZE    (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT_SIZE   (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO_SIZE          (0)
#define SOC_PB_PMF_IRPP_INFO_FIELD_UP_SIZE                         (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH_SIZE             (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE_SIZE                 (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST_SIZE                 (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD_SIZE                  (24)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX_SIZE                    (14)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE_SIZE          (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VSI_SIZE                        (14)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI_SIZE                 (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB_SIZE         (1)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG_SIZE              (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE_SIZE                  (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE_SIZE                (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX_SIZE          (3)
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT_SIZE          (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE_SIZE    (4)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF_SIZE                     (12)
#define SOC_PB_PMF_IRPP_INFO_FIELD_VRF_SIZE                        (9)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC_SIZE            (1)
#define SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC_SIZE                    (1)
#define SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF_SIZE              (25)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL_SIZE                     (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP_SIZE                (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID_SIZE          (15)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT_SIZE      (17)
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT_SIZE      (17)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST_SIZE (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD_SIZE  (26)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST_SIZE (16)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD_SIZE  (26)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT_SIZE      (15)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT_SIZE      (15)
#define SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT_SIZE         (17)
#define SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT_SIZE          (42)
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR_SIZE                (32)
#define SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE_SIZE          (8)
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE_SIZE          (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS_SIZE                     (24)
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE_SIZE            (2)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1_SIZE                    (32)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2_SIZE                    (10)
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3_SIZE                    (19)
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO_SIZE                 (107)

#define SOC_PB_PMF_TCAM_KEY_SRC_UNDEF_FLD_VAL                   (0)
#define SOC_PB_PMF_TCAM_KEY_SRC_L2_FLD_VAL                      (1)
#define SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4_FLD_VAL                 (2)
#define SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6_FLD_VAL                 (3)
#define SOC_PB_PMF_TCAM_KEY_SRC_B_A_FLD_VAL                     (7)
#define SOC_PB_PMF_TCAM_KEY_SRC_A_71_0_FLD_VAL                  (8)
#define SOC_PB_PMF_TCAM_KEY_SRC_A_103_32_FLD_VAL                (9)
#define SOC_PB_PMF_TCAM_KEY_SRC_A_143_0_FLD_VAL                 (10)
#define SOC_PB_PMF_TCAM_KEY_SRC_A_175_32_FLD_VAL                (11)
#define SOC_PB_PMF_TCAM_KEY_SRC_B_71_0_FLD_VAL                  (12)
#define SOC_PB_PMF_TCAM_KEY_SRC_B_103_32_FLD_VAL                (13)
#define SOC_PB_PMF_TCAM_KEY_SRC_B_143_0_FLD_VAL                 (14)
#define SOC_PB_PMF_TCAM_KEY_SRC_B_175_32_FLD_VAL                (15)

#define SOC_PB_PMF_TCAM_FLD_L2_LLVP_LSB 											  (138)
#define SOC_PB_PMF_TCAM_FLD_L2_STAG_LSB                         (122)
#define SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC_LSB                   (106)
#define SOC_PB_PMF_TCAM_FLD_L2_SA_LSB                           (58)
#define SOC_PB_PMF_TCAM_FLD_L2_DA_LSB                           (10)
#define SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE_LSB                    (6)
#define SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT_LSB                   (0)

#define SOC_PB_PMF_TCAM_FLD_L2_LLVP_SIZE 											  (4)
#define SOC_PB_PMF_TCAM_FLD_L2_STAG_SIZE                        (16)
#define SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC_SIZE                  (16)
#define SOC_PB_PMF_TCAM_FLD_L2_SA_SIZE                          (48)
#define SOC_PB_PMF_TCAM_FLD_L2_DA_SIZE                          (48)
#define SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE_SIZE                   (4)
#define SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT_SIZE                  (6)

#define SOC_PB_PMF_FLD_IPV4_L4OPS_LSB                           (136)
#define SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL_LSB                      (132)
#define SOC_PB_PMF_FLD_IPV4_DF_LSB                              (131)
#define SOC_PB_PMF_FLD_IPV4_MF_LSB                              (130)
#define SOC_PB_PMF_FLD_IPV4_FRAG_NON_0_LSB                      (129)
#define SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG_LSB                      (128)
#define SOC_PB_PMF_FLD_IPV4_SIP_LSB                             (96)
#define SOC_PB_PMF_FLD_IPV4_DIP_LSB                             (64)
#define SOC_PB_PMF_FLD_IPV4_L4OPS_OPT_LSB                       (32)
#define SOC_PB_PMF_FLD_IPV4_SRC_PORT_LSB                        (48)
#define SOC_PB_PMF_FLD_IPV4_DEST_PORT_LSB                       (32)
#define SOC_PB_PMF_FLD_IPV4_TOS_LSB                             (24)
#define SOC_PB_PMF_FLD_IPV4_TCP_CTL_LSB                         (18)
#define SOC_PB_PMF_FLD_IPV4_IN_AC_VRF_LSB                       (0)
#define SOC_PB_PMF_FLD_IPV4_IN_PP_PORT_LSB                      (12)
#define SOC_PB_PMF_FLD_IPV4_IN_VID_LSB                          (0)

#define SOC_PB_PMF_FLD_IPV4_L4OPS_SIZE                          (7)
#define SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL_SIZE                     (4)
#define SOC_PB_PMF_FLD_IPV4_DF_SIZE                             (1)
#define SOC_PB_PMF_FLD_IPV4_MF_SIZE                             (1)
#define SOC_PB_PMF_FLD_IPV4_FRAG_NON_0_SIZE                     (1)
#define SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG_SIZE                     (1)
#define SOC_PB_PMF_FLD_IPV4_SIP_SIZE                            (32)
#define SOC_PB_PMF_FLD_IPV4_DIP_SIZE                            (32)
#define SOC_PB_PMF_FLD_IPV4_L4OPS_OPT_SIZE                      (16)
#define SOC_PB_PMF_FLD_IPV4_SRC_PORT_SIZE                       (16)
#define SOC_PB_PMF_FLD_IPV4_DEST_PORT_SIZE                      (16)
#define SOC_PB_PMF_FLD_IPV4_TOS_SIZE                            (8)
#define SOC_PB_PMF_FLD_IPV4_TCP_CTL_SIZE                        (6)
#define SOC_PB_PMF_FLD_IPV4_IN_AC_VRF_SIZE                      (16)
#define SOC_PB_PMF_FLD_IPV4_IN_PP_PORT_SIZE                     (6)
#define SOC_PB_PMF_FLD_IPV4_IN_VID_SIZE                         (12)

#define SOC_PB_PMF_FLD_IPV6_L4OPS_LSB                           (280)
#define SOC_PB_PMF_FLD_IPV6_SIP_HIGH_LSB                        (152 + 64)
#define SOC_PB_PMF_FLD_IPV6_SIP_LOW_LSB                         (152)
#define SOC_PB_PMF_FLD_IPV6_DIP_HIGH_LSB                        (24 + 64)
#define SOC_PB_PMF_FLD_IPV6_DIP_LOW_LSB                         (24)
#define SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL_LSB                      (20)
#define SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL_LSB                 (14)
#define SOC_PB_PMF_FLD_IPV6_IN_AC_VRF_LSB                       (0)

#define SOC_PB_PMF_FLD_IPV6_L4OPS_SIZE                          (8)
#define SOC_PB_PMF_FLD_IPV6_SIP_HIGH_SIZE                       (64)
#define SOC_PB_PMF_FLD_IPV6_SIP_LOW_SIZE                        (64)
#define SOC_PB_PMF_FLD_IPV6_DIP_HIGH_SIZE                       (64)
#define SOC_PB_PMF_FLD_IPV6_DIP_LOW_SIZE                        (64)
#define SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL_SIZE                     (4)
#define SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL_SIZE                (6)
#define SOC_PB_PMF_FLD_IPV6_IN_AC_VRF_SIZE                      (14)

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

typedef enum
{
 /*
  *  Selector PD1.
  */
  SOC_PB_PMF_BANK_SEL_PD1 = 0,
  /*
   *  Selector PD2.
   */
  SOC_PB_PMF_BANK_SEL_PD2 = 1,
  /*
   *  Selector Sel3.
   */
  SOC_PB_PMF_BANK_SEL_SEL3 = 2,
  /*
   *  Selector Sel4.
   */
  SOC_PB_PMF_BANK_SEL_SEL4 = 3,
  /*
   *  Number of types in SOC_PB_PMF_BANK_SEL
   */
  SOC_PB_NOF_PMF_BANK_SELS = 4
}SOC_PB_PMF_BANK_SEL;

typedef enum
{
 /*
  *  Key-A.
  */
  SOC_PB_PMF_TCAM_KEY_TYPE_A = 0,
 /*
  *  Key-B.
  */
  SOC_PB_PMF_TCAM_KEY_TYPE_B = 1,
 /*
  *  Key-A and Key-B.
  */
  SOC_PB_PMF_TCAM_KEY_TYPE_A_B = 2,
  /*
   *  Number of types in SOC_PB_PMF_TCAM_KEY_TYPE
   */
   SOC_PB_NOF_PMF_TCAM_KEY_TYPES = 3
}SOC_PB_PMF_TCAM_KEY_TYPE;

typedef enum
{
 /*
  *  Key-A.
  */
  SOC_PB_PMF_TCAM_KEY_NDX_A = 0,
 /*
  *  Key-B.
  */
  SOC_PB_PMF_TCAM_KEY_NDX_B = 1,
  /*
   *  Number of types in SOC_PB_PMF_TCAM_KEY_NDX
   */
   SOC_PB_NOF_PMF_TCAM_KEY_NDXS = 2
}SOC_PB_PMF_TCAM_KEY_NDX;

/* } */

uint32
  soc_pb_pmf_low_level_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_low_level_tm_init_set_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_low_level_tm_pgm_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

uint32
  soc_pb_pmf_low_level_stack_init_set_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_low_level_stack_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

uint32
  soc_pb_pmf_low_level_raw_pgm_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

uint32
  soc_pb_pmf_low_level_raw_pgm_dt_disable(
    SOC_SAND_IN  int                 unit
  );

uint32
  soc_pb_pmf_low_level_eth_pgm_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 pfg_ndx,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_INCLUDED__*/
#endif

