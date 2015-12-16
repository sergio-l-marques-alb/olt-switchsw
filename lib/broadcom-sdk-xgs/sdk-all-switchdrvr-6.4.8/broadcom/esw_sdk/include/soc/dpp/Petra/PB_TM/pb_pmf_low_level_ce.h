/* $Id: soc_pb_pmf_low_level_ce.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_CE_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_CE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_CE_SUB_HEADER_0                          SOC_TMC_PMF_CE_SUB_HEADER_0
#define SOC_PB_PMF_CE_SUB_HEADER_1                          SOC_TMC_PMF_CE_SUB_HEADER_1
#define SOC_PB_PMF_CE_SUB_HEADER_2                          SOC_TMC_PMF_CE_SUB_HEADER_2
#define SOC_PB_PMF_CE_SUB_HEADER_3                          SOC_TMC_PMF_CE_SUB_HEADER_3
#define SOC_PB_PMF_CE_SUB_HEADER_4                          SOC_TMC_PMF_CE_SUB_HEADER_4
#define SOC_PB_PMF_CE_SUB_HEADER_5                          SOC_TMC_PMF_CE_SUB_HEADER_5
#define SOC_PB_PMF_CE_SUB_HEADER_FWD                        SOC_TMC_PMF_CE_SUB_HEADER_FWD
#define SOC_PB_PMF_CE_SUB_HEADER_FWD_POST                   SOC_TMC_PMF_CE_SUB_HEADER_FWD_POST
#define SOC_PB_NOF_PMF_CE_SUB_HEADERS                       SOC_TMC_NOF_PMF_CE_SUB_HEADERS
typedef SOC_TMC_PMF_CE_SUB_HEADER                           SOC_PB_PMF_CE_SUB_HEADER;

#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT                  SOC_TMC_PMF_IRPP_INFO_FIELD_TM_PORT
#define SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32                SOC_TMC_PMF_IRPP_INFO_FIELD_TM_PORT32
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE         SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC            SOC_TMC_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT               SOC_TMC_PMF_IRPP_INFO_FIELD_IN_PP_PORT
#define SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT               SOC_TMC_PMF_IRPP_INFO_FIELD_PP_CONTEXT
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4
#define SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5          SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5       SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT    SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT
#define SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE       SOC_TMC_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE       SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB  SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB
#define SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT SOC_TMC_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH      SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID    SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID      SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID      SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL
#define SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO        SOC_TMC_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO
#define SOC_PB_PMF_IRPP_INFO_FIELD_UP                       SOC_TMC_PMF_IRPP_INFO_FIELD_UP
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH           SOC_TMC_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH
#define SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE               SOC_TMC_PMF_IRPP_INFO_FIELD_SNOOP_CODE
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST               SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_DEST
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD                SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_ASD
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX                  SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_NDX
#define SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE        SOC_TMC_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE
#define SOC_PB_PMF_IRPP_INFO_FIELD_VSI                      SOC_TMC_PMF_IRPP_INFO_FIELD_VSI
#define SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI               SOC_TMC_PMF_IRPP_INFO_FIELD_SYSTEM_VSI
#define SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB       SOC_TMC_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB
#define SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG            SOC_TMC_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG
#define SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE                SOC_TMC_PMF_IRPP_INFO_FIELD_STP_STATE
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE              SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_CODE
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX        SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX
#define SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT        SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT
#define SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE  SOC_TMC_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF                   SOC_TMC_PMF_IRPP_INFO_FIELD_IN_RIF
#define SOC_PB_PMF_IRPP_INFO_FIELD_VRF                      SOC_TMC_PMF_IRPP_INFO_FIELD_VRF
#define SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC          SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC
#define SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC                  SOC_TMC_PMF_IRPP_INFO_FIELD_MY_BMAC
#define SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF            SOC_TMC_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL                   SOC_TMC_PMF_IRPP_INFO_FIELD_IN_TTL
#define SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP              SOC_TMC_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP
#define SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID        SOC_TMC_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT    SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT    SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD  SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD  SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT    SOC_TMC_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT    SOC_TMC_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT       SOC_TMC_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT        SOC_TMC_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR              SOC_TMC_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR
#define SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE        SOC_TMC_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE
#define SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE        SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE
#define SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS                   SOC_TMC_PMF_IRPP_INFO_FIELD_L4_OPS
#define SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE          SOC_TMC_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1                  SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM1
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2                  SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM2
#define SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3                  SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM3
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8                   SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_8
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16                  SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_16
#define SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32                  SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_32
#define SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32                  SOC_TMC_PMF_IRPP_INFO_FIELD_ONES_32
#define SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO               SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_INFO
#define SOC_PB_NOF_PMF_IRPP_INFO_FIELDS                     SOC_TMC_NOF_PMF_IRPP_INFO_FIELDS
typedef SOC_TMC_PMF_IRPP_INFO_FIELD                         SOC_PB_PMF_IRPP_INFO_FIELD;

typedef SOC_TMC_PMF_CE_PACKET_HEADER_INFO                      SOC_PB_PMF_CE_PACKET_HEADER_INFO;

/* $Id: soc_pb_pmf_low_level_ce.h,v 1.6 Broadcom SDK $
 *	Restrict the CE to 5 instructions per cycle temporarily
 */

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
  soc_pb_pmf_low_level_ce_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_ce_irpp_field_size_and_val_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD  irpp_field,
    SOC_SAND_OUT uint32                   *fld_size,
    SOC_SAND_OUT uint32                   *fld_map_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_ce_packet_header_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set an entry in the copy engines table, adding fields
 *   from the packet header to the PMF key. The PMF keys are
 *   constructed by a series of copy engines. Each copy
 *   engine appends field (s) from the packet headers or from
 *   the incoming IRPP information into the PMF key.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            pmf_pgm_ndx -
 *     PMF-Program-ID. Range: 0 - 31. (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Soc_petra-B) The Copy-Engine
 *     '0' set the MSB of the output vector, '1' is appended to
 *     the output vector etc.
 *   SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info -
 *     Information for the Copy-Engine to add from the packet
 *     header to the PMF key.
 * REMARKS:
 *   1. In the first lookup, only Copy Engines 0-3 update the
 *   PMF key and writes to Key A/B Program Instruction
 *   Tables.2. The get function returns a cleared 'info' if
 *   the Copy Engine is programmed to set fields from the
 *   IRPP info
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_ce_packet_header_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  );

uint32
  soc_pb_pmf_ce_packet_header_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  );

uint32
  soc_pb_pmf_ce_packet_header_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_ce_packet_header_entry_set_unsafe" API.
 *     Refer to "soc_pb_pmf_ce_packet_header_entry_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pmf_ce_packet_header_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_ce_irpp_info_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set an entry in the copy engines table, adding fields
 *   from the IRPP information to the PMF key.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            pmf_pgm_ndx -
 *     PMF-Program-ID. Range: 0 - 31. (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Soc_petra-B) The Copy-Engine
 *     '0' set the MSB of the output vector, '1' is appended to
 *     the output vector etc.
 *   SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD            irpp_field -
 *     Field of the IRPP info to extract to the Copy-Engine.
 * REMARKS:
 *   1. In the first lookup, only Copy Engines 0-3 update the
 *   PMF key and writes to Key A/B Program Instruction
 *   Tables.2. When extracting a single field, the CE removes
 *   the irrelevant bits (due to 4 bits alignment).3. The get
 *   function returns a cleared 'info' if the Copy Engine is
 *   programmed to set fields from the packet sub-headers
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD            irpp_field
  );

uint32
  soc_pb_pmf_ce_irpp_info_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD            irpp_field
  );

uint32
  soc_pb_pmf_ce_irpp_info_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_ce_irpp_info_entry_set_unsafe" API.
 *     Refer to "soc_pb_pmf_ce_irpp_info_entry_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pmf_ce_irpp_info_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT uint32                             *nof_bits,
    SOC_SAND_OUT SOC_PB_PMF_IRPP_INFO_FIELD               *irpp_field
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_ce_nop_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set an entry in the copy engines table to be invalid.
 *   Invalid entries are bypassed, and do not affect the PMF
 *   key.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            pmf_pgm_ndx -
 *     PMF-Program-ID. Range: 0 - 31. (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Soc_petra-B) The Copy-Engine
 *     '0' set the MSB of the output vector, '1' is appended to
 *     the output vector etc.
 *   SOC_SAND_IN  uint8                            is_ce_not_valid -
 *     If TRUE, then the Copy Engine does not perform any
 *     operation. Otherwise, the Copy Engine performs the
 *     operation as set by the Copy Engine APIs.
 * REMARKS:
 *   1. In the first lookup, only Copy Engines 0-3 update the
 *   PMF key and writes to Key A/B Program Instruction
 *   Tables.2. To validate the entry, the user should call
 *   pmf_ce_packet_header_entry_set() or
 *   pmf_ce_irpp_info_entry_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_ce_nop_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_not_valid
  );

uint32
  soc_pb_pmf_ce_nop_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_not_valid
  );

uint32
  soc_pb_pmf_ce_nop_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_ce_nop_entry_set_unsafe" API.
 *     Refer to "soc_pb_pmf_ce_nop_entry_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_ce_nop_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT uint8                            *is_ce_not_valid,
    SOC_SAND_OUT uint32                             *loc
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_low_level_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_pmf_low_level module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pmf_low_level_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pmf_low_level_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_pmf_low_level module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pmf_low_level_get_errs_ptr(void);

uint32
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  );

void
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  SOC_PB_PMF_CE_SUB_HEADER enum_val
  );

const char*
  SOC_PB_PMF_IRPP_INFO_FIELD_to_string(
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD enum_val
  );

void
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_CE_INCLUDED__*/
#endif

