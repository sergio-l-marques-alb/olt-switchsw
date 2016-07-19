/* $Id: pb_pmf_low_level_fem_tag.h,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_fem_tag.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PB_PMF_FEM_INPUT_SRC_A_31_0                     SOC_TMC_PMF_FEM_INPUT_SRC_A_31_0
#define SOC_PB_PMF_FEM_INPUT_SRC_A_47_16                    SOC_TMC_PMF_FEM_INPUT_SRC_A_47_16
#define SOC_PB_PMF_FEM_INPUT_SRC_A_63_32                    SOC_TMC_PMF_FEM_INPUT_SRC_A_63_32
#define SOC_PB_PMF_FEM_INPUT_SRC_A_79_48                    SOC_TMC_PMF_FEM_INPUT_SRC_A_79_48
#define SOC_PB_PMF_FEM_INPUT_SRC_A_95_64                    SOC_TMC_PMF_FEM_INPUT_SRC_A_95_64
#define SOC_PB_PMF_FEM_INPUT_SRC_B_31_0                     SOC_TMC_PMF_FEM_INPUT_SRC_B_31_0
#define SOC_PB_PMF_FEM_INPUT_SRC_B_47_16                    SOC_TMC_PMF_FEM_INPUT_SRC_B_47_16
#define SOC_PB_PMF_FEM_INPUT_SRC_B_63_32                    SOC_TMC_PMF_FEM_INPUT_SRC_B_63_32
#define SOC_PB_PMF_FEM_INPUT_SRC_B_79_48                    SOC_TMC_PMF_FEM_INPUT_SRC_B_79_48
#define SOC_PB_PMF_FEM_INPUT_SRC_B_95_64                    SOC_TMC_PMF_FEM_INPUT_SRC_B_95_64
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0                     SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_0
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1                     SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_1
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2                     SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_2
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3                     SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_3
#define SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL                    SOC_TMC_PMF_FEM_INPUT_SRC_DIR_TBL
#define SOC_PB_PMF_FEM_INPUT_SRC_TCAM                       SOC_TMC_PMF_FEM_INPUT_TCAM
#define SOC_PB_PMF_FEM_INPUT_SRC_NOP                        SOC_TMC_PMF_FEM_INPUT_SRC_NOP
#define SOC_PB_NOF_PMF_FEM_INPUT_SRCS                       SOC_TMC_NOF_PMF_FEM_INPUT_SRCS
typedef SOC_TMC_PMF_FEM_INPUT_SRC                              SOC_PB_PMF_FEM_INPUT_SRC;

#define SOC_PB_PMF_TAG_TYPE_STAT_TAG                        SOC_TMC_PMF_TAG_TYPE_STAT_TAG
#define SOC_PB_PMF_TAG_TYPE_LAG_LB_KEY                      SOC_TMC_PMF_TAG_TYPE_LAG_LB_KEY
#define SOC_PB_PMF_TAG_TYPE_ECMP_LB_KEY                     SOC_TMC_PMF_TAG_TYPE_ECMP_LB_KEY
#define SOC_PB_PMF_TAG_TYPE_STACK_RT_HIST                   SOC_TMC_PMF_TAG_TYPE_STACK_RT_HIST
#define SOC_PB_NOF_PMF_TAG_TYPES                            SOC_TMC_NOF_PMF_TAG_TYPES
typedef SOC_TMC_PMF_TAG_TYPE                                   SOC_PB_PMF_TAG_TYPE;

#define SOC_PB_PMF_TAG_VAL_SRC_A_29_0                       SOC_TMC_PMF_TAG_VAL_SRC_A_29_0
#define SOC_PB_PMF_TAG_VAL_SRC_A_61_32                      SOC_TMC_PMF_TAG_VAL_SRC_A_61_32
#define SOC_PB_PMF_TAG_VAL_SRC_B_29_0                       SOC_TMC_PMF_TAG_VAL_SRC_B_29_0
#define SOC_PB_PMF_TAG_VAL_SRC_B_61_32                      SOC_TMC_PMF_TAG_VAL_SRC_B_61_32
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_0                       SOC_TMC_PMF_TAG_VAL_SRC_TCAM_0
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_1                       SOC_TMC_PMF_TAG_VAL_SRC_TCAM_1
#define SOC_PB_PMF_TAG_VAL_SRC_TCAM_2                       SOC_TMC_PMF_TAG_VAL_SRC_TCAM_2
#define SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL                      SOC_TMC_PMF_TAG_VAL_SRC_DIR_TBL
#define SOC_PB_NOF_PMF_TAG_VAL_SRCS                         SOC_TMC_NOF_PMF_TAG_VAL_SRCS
typedef SOC_TMC_PMF_TAG_VAL_SRC                                SOC_PB_PMF_TAG_VAL_SRC;


#define SOC_PB_PMF_FEM_ACTION_TYPE_DEST                     SOC_TMC_PMF_FEM_ACTION_TYPE_DEST
#define SOC_PB_PMF_FEM_ACTION_TYPE_DP                       SOC_TMC_PMF_FEM_ACTION_TYPE_DP
#define SOC_PB_PMF_FEM_ACTION_TYPE_TC                       SOC_TMC_PMF_FEM_ACTION_TYPE_TC
#define SOC_PB_PMF_FEM_ACTION_TYPE_TRAP                      SOC_TMC_PMF_FEM_ACTION_TYPE_TRAP
#define SOC_PB_PMF_FEM_ACTION_TYPE_SNP                      SOC_TMC_PMF_FEM_ACTION_TYPE_SNP
#define SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR                   SOC_TMC_PMF_FEM_ACTION_TYPE_MIRROR
#define SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS                  SOC_TMC_PMF_FEM_ACTION_TYPE_MIR_DIS
#define SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC                  SOC_TMC_PMF_FEM_ACTION_TYPE_EXC_SRC
#define SOC_PB_PMF_FEM_ACTION_TYPE_IS                       SOC_TMC_PMF_FEM_ACTION_TYPE_IS
#define SOC_PB_PMF_FEM_ACTION_TYPE_METER                    SOC_TMC_PMF_FEM_ACTION_TYPE_METER
#define SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER                  SOC_TMC_PMF_FEM_ACTION_TYPE_COUNTER
#define SOC_PB_PMF_FEM_ACTION_TYPE_STAT                     SOC_TMC_PMF_FEM_ACTION_TYPE_STAT
#define SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF                   SOC_TMC_PMF_FEM_ACTION_TYPE_OUTLIF
#define SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA            SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA
#define SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM             SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM
#define SOC_PB_PMF_FEM_ACTION_TYPE_NOP                      SOC_TMC_PMF_FEM_ACTION_TYPE_NOP
#define SOC_PB_NOF_PMF_FEM_ACTION_TYPES                     SOC_TMC_NOF_PMF_FEM_ACTION_TYPES
typedef SOC_TMC_PMF_FEM_ACTION_TYPE                            SOC_PB_PMF_FEM_ACTION_TYPE;

#define SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST                     SOC_TMC_PMF_FEM_BIT_LOC_TYPE_CST
#define SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY                     SOC_TMC_PMF_FEM_BIT_LOC_TYPE_KEY
#define SOC_PB_PMF_FEM_BIT_LOC_TYPE_MAP_DATA                SOC_TMC_PMF_FEM_BIT_LOC_TYPE_MAP_DATA
#define SOC_PB_NOF_PMF_FEM_BIT_LOC_TYPES                    SOC_TMC_NOF_PMF_FEM_BIT_LOC_TYPES
typedef SOC_TMC_PMF_FEM_BIT_LOC_TYPE                           SOC_PB_PMF_FEM_BIT_LOC_TYPE;

typedef SOC_TMC_PMF_FEM_INPUT_INFO                             SOC_PB_PMF_FEM_INPUT_INFO;
typedef SOC_TMC_PMF_TAG_SRC_INFO                               SOC_PB_PMF_TAG_SRC_INFO;
typedef SOC_TMC_PMF_FEM_NDX                                    SOC_PB_PMF_FEM_NDX;
typedef SOC_TMC_PMF_FEM_SELECTED_BITS_INFO                     SOC_PB_PMF_FEM_SELECTED_BITS_INFO;
typedef SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO                 SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO;
typedef SOC_TMC_PMF_FEM_BIT_LOC                                SOC_PB_PMF_FEM_BIT_LOC;
typedef SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO                     SOC_PB_PMF_FEM_ACTION_FORMAT_INFO;

#define SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT (0)
#define SOC_PB_PMF_FEM_ACTION_DEFAULT_DEST_1        (1)
#define SOC_PB_PMF_FEM_FTMH_ACTION_FORMAT_NDX       (1)
#define SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX        (2)
#define SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3         (3)


#define SOC_PB_PMF_LOW_LEVEL_NOF_FEMS               (8)

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

uint32
  soc_pb_pmf_low_level_fem_tag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_fem_output_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX   *fem_ndx,
    SOC_SAND_OUT uint32            *output_size_in_bits
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_db_fem_input_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Select the input for each Field Extraction MACRO. The FEM
 *   processes the PCL results, and extracts possible actions
 *   to be applied on the packet. INPUT SOC_SAND_IN
 *   SOC_PPD_PMF_LKP_PROFILE *lkp_profile_ndx - Lookup-Profile
 *   information (id and cycle). SOC_SAND_IN uint32 *fem_ndx -
 *   FEM (Field Extraction Macro) Index. Range: 0 - 7.
 *   (Soc_petra-B) SOC_SAND_IN SOC_PPD_PMF_FEM_INPUT_INFO *info - FEM
 *   input parameters: the FEM-Program-Id and the FEM-Input
 *   source. RETURNS OK or Error indicationREMARKS None.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx -
 *     SOC_SAND_IN SOC_PPD_PMF_LKP_PROFILE *lkp_profile_ndx
 *   SOC_SAND_IN  uint32                            fem_ndx -
 *     SOC_SAND_IN uint32 fem_ndx
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO             *info -
 *     FUNCTION Select the input for each Field Extraction
 *     MACRO. The FEM processes the PCL results, and extracts
 *     possible actions to be applied on the packet. INPUT
 *     SOC_SAND_IN SOC_PPD_PMF_LKP_PROFILE *lkp_profile_ndx -
 *     Lookup-Profile information (id and cycle). SOC_SAND_IN
 *     uint32 *fem_ndx - FEM (Field Extraction Macro) Index.
 *     Range: 0 - 7. (Soc_petra-B) SOC_SAND_IN SOC_PPD_PMF_FEM_INPUT_INFO
 *     *info - FEM input parameters: the FEM-Program-Id and the
 *     FEM-Input source.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_db_fem_input_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO             *info
  );

uint32
  soc_pb_pmf_db_fem_input_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO             *info
  );

uint32
  soc_pb_pmf_db_fem_input_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_db_fem_input_set_unsafe" API.
 *     Refer to "soc_pb_pmf_db_fem_input_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_db_fem_input_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_INPUT_INFO             *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_db_tag_select_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Tag value source per Tag profile. The TAG source
 *   designates the data location to take the Tag value from,
 *   and where only the lsb bits are used (the number of bits
 *   used depends on the Tag-Type).
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            tag_profile_ndx -
 *     Tag-Profile Index. Set per PMF-Program-Id. Range: 0 - 7.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx -
 *     Tag type for which the Tag source is selected.
 *   SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO               *tag_src -
 *     Tag source parameters: its location and the number of
 *     bits to consider.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_db_tag_select_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  );

uint32
  soc_pb_pmf_db_tag_select_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  );

uint32
  soc_pb_pmf_db_tag_select_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_db_tag_select_set_unsafe" API.
 *     Refer to "soc_pb_pmf_db_tag_select_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_db_tag_select_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_fem_select_bits_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the location of 4 bits from the FEM-key that select
 *   the performed action format for this key (configure the
 *   Select-4-bits table).
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx -
 *     FEM Index.
 *   SOC_SAND_IN  uint32                            fem_pgm_ndx -
 *     FEM-Program-Id. Is set with the FEM input source. Range:
 *     0 - 3. (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info -
 *     Bits to select from the FEM-Key.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_fem_select_bits_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  );

uint32
  soc_pb_pmf_fem_select_bits_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  );

uint32
  soc_pb_pmf_fem_select_bits_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_fem_select_bits_set_unsafe" API.
 *     Refer to "soc_pb_pmf_fem_select_bits_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_fem_select_bits_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_fem_action_format_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the location of 4 bits from the FEM-key that select
 *   the performed action format for this key (configure the
 *   Select-4-bits table).
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx -
 *     FEM Index.
 *   SOC_SAND_IN  uint32                            fem_pgm_ndx -
 *     FEM-Program-Id. Is set with the FEM input source. Range:
 *     0 - 3. (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            selected_bits_ndx -
 *     Value of the selected-bits with
 *     soc_ppd_pmf_fem_select_bits_set() API. Range: 0 - 15.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info -
 *     Action-format-Id and the Map-Data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_map_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  soc_pb_pmf_fem_action_format_map_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  soc_pb_pmf_fem_action_format_map_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_fem_action_format_map_set_unsafe" API.
 *     Refer to "soc_pb_pmf_fem_action_format_map_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_map_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_fem_action_format_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the format of the each action done by the
 *   Field Extraction Macro. Each FEM can perform up to four
 *   different actions.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx -
 *     FEM Index.
 *   SOC_SAND_IN  uint32                            action_fomat_ndx -
 *     Action-Format-Id. Range: 0 - 3. (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info -
 *     Parameters of the Action Format: its type, its
 *     base-value and the field extraction location.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  );

uint32
  soc_pb_pmf_fem_action_format_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  );

uint32
  soc_pb_pmf_fem_action_format_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_fem_action_format_set_unsafe" API.
 *     Refer to "soc_pb_pmf_fem_action_format_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  );


uint32
  SOC_PB_PMF_FEM_INPUT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO *info
  );

uint32
  SOC_PB_PMF_TAG_SRC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO *info
  );

uint32
  SOC_PB_PMF_FEM_NDX_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX *info
  );

uint32
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  );

uint32
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  SOC_PB_PMF_FEM_BIT_LOC_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC *info
  );

uint32
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  );

void
  SOC_PB_PMF_FEM_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_INPUT_INFO *info
  );

void
  SOC_PB_PMF_TAG_SRC_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TAG_SRC_INFO *info
  );



void
  SOC_PB_PMF_FEM_NDX_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_NDX *info
  );

void
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  SOC_PB_PMF_FEM_BIT_LOC_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_BIT_LOC *info
  );

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1
const char*
  SOC_PB_PMF_FEM_INPUT_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_SRC enum_val
  );

const char*
  SOC_PB_PMF_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE enum_val
  );

const char*
  SOC_PB_PMF_TAG_VAL_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TAG_VAL_SRC enum_val
  );

const char*
  SOC_PB_PMF_FEM_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_TYPE enum_val
  );

const char*
  SOC_PB_PMF_FEM_BIT_LOC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC_TYPE enum_val
  );

void
  SOC_PB_PMF_FEM_INPUT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO *info
  );

void
  SOC_PB_PMF_TAG_SRC_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO *info
  );



void
  SOC_PB_PMF_FEM_NDX_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX *info
  );

void
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  SOC_PB_PMF_FEM_BIT_LOC_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC *info
  );

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__*/
#endif

