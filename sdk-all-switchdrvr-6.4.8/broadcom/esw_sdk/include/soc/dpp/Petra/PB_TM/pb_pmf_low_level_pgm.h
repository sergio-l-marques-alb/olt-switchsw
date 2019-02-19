/* $Id: soc_pb_pmf_low_level_pgm.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_PGM_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_PGM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



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


#define SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8             SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8
#define SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0              SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0
#define SOC_PB_PMF_PGM_SEL_TYPE_SEM_NDX_7_0                 SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0
#define SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8            SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8
#define SOC_PB_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE             SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE
#define SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO              SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO
#define SOC_PB_PMF_PGM_SEL_TYPE_LOOKUPS                     SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS
#define SOC_PB_PMF_PGM_SEL_TYPE_LLVP_PFC                    SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC
#define SOC_PB_NOF_PMF_PGM_SEL_TYPES                        SOC_TMC_NOF_PMF_PGM_SEL_TYPES
typedef SOC_TMC_PMF_PGM_SEL_TYPE                               SOC_PB_PMF_PGM_SEL_TYPE;

#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START               SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_START
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST                 SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_1ST
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING              SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_FWDING
#define SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING         SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING
#define SOC_PB_NOF_PMF_PGM_BYTES_TO_RMV_HDRS                SOC_TMC_NOF_PMF_PGM_BYTES_TO_RMV_HDRS
typedef SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR                       SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR;

typedef SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM                        SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM;
typedef SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC                        SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC;
typedef SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF                       SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF;
typedef SOC_TMC_PMF_PGM_SEL_VAL_LKP                            SOC_PB_PMF_PGM_SEL_VAL_LKP;
typedef SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC                       SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC;
typedef SOC_TMC_PMF_PGM_SEL_VAL                                SOC_PB_PMF_PGM_SEL_VAL;
typedef SOC_TMC_PMF_PGM_SELECTION_ENTRY                        SOC_PB_PMF_PGM_SELECTION_ENTRY;
typedef SOC_TMC_PMF_PGM_VALIDITY_INFO                          SOC_PB_PMF_PGM_VALIDITY_INFO;
typedef SOC_TMC_PMF_PGM_BYTES_TO_RMV                           SOC_PB_PMF_PGM_BYTES_TO_RMV;
typedef SOC_TMC_PMF_PGM_INFO                                   SOC_PB_PMF_PGM_INFO;

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
  soc_pb_pmf_low_level_pgm_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_IN  uint32                          pfg_ndx,
   SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  );

/* $Id: soc_pb_pmf_low_level_pgm.h,v 1.5 Broadcom SDK $
 *  PMF Port Profile management functions
 */
uint32
  soc_pb_pmf_low_level_pgm_port_profile_encode(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          pfg_ndx,
    SOC_SAND_IN  uint8                          is_only_port_pmf_profile,
    SOC_SAND_OUT uint32                          *encoded_pp_info
  );

uint32
  soc_pb_pmf_low_level_pgm_port_profile_decode(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           encoded_pp_info,
    SOC_SAND_IN  uint8                          is_only_port_pmf_profile,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_OUT  uint32                        *pfg_ndx
  );

uint32
  soc_pb_pmf_low_level_pgm_port_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_OUT uint32                          *internal_profile
  );

uint32
  soc_pb_pmf_low_level_pgm_port_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  );


/*
 *  System Header Profile management functions
 */
uint32
  soc_pb_pmf_low_level_pgm_header_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pmf_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_OUT uint32                          *internal_profile
  );

uint32
  soc_pb_pmf_low_level_pgm_header_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  );

uint32
  soc_pb_pmf_low_level_pgm_header_profile_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PORTS_FC_TYPE                   fc_type,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE             header_type,
    SOC_SAND_IN  uint8                          tm_pph_present,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile
  );

uint32
  soc_pb_pmf_low_level_pgm_header_type_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_OUT SOC_PB_PORTS_FC_TYPE                  *fc_type,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE            *header_type
  );

uint32
  soc_pb_pmf_low_level_pgm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


/*********************************************************************
* NAME:
 *   soc_pb_pmf_pgm_selection_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set which programs are relevant per value of a program
 *   selection parameter.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx -
 *     Program selection parameter. For a specific packet, a
 *     program can be selected only if its value in all the
 *     program selection parameters indicates this program as
 *     relevant.
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO          *info -
 *     Indicates which programs are valid for this entry.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_selection_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  );

uint32
  soc_pb_pmf_pgm_selection_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  );

uint32
  soc_pb_pmf_pgm_selection_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_pgm_selection_entry_set_unsafe" API.
 *     Refer to "soc_pb_pmf_pgm_selection_entry_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_selection_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_pgm_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Program properties, e.g. Lookup-Profile-IDs. The
 *   Copy Engine instructions are set via the PMF Copy Engine
 *   APIs.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            pmf_pgm_ndx -
 *     PMF-Program-ID. Range: 0 - 31. (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_PGM_INFO                   *info -
 *     The program properties, except the ones related to the
 *     Copy Engine set in the Copy Engine APIs.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO                     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

uint32
  soc_pb_pmf_pgm_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO                     *info
  );

uint32
  soc_pb_pmf_pgm_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_pgm_set_unsafe" API.
 *     Refer to "soc_pb_pmf_pgm_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_pmf_pgm_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_INFO                   *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_LKP_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  );

uint32
  SOC_PB_PMF_PGM_SEL_VAL_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL *info
  );

uint32
  SOC_PB_PMF_PGM_SELECTION_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  );

uint32
  SOC_PB_PMF_PGM_VALIDITY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO *info
  );

uint32
  SOC_PB_PMF_PGM_BYTES_TO_RMV_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  );

uint32
  SOC_PB_PMF_PGM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_LKP_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_clear(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_TYPE sel_type,
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL *info
  );

void
  SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  );

void
  SOC_PB_PMF_PGM_VALIDITY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_VALIDITY_INFO *info
  );

void
  SOC_PB_PMF_PGM_BYTES_TO_RMV_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  );

void
  SOC_PB_PMF_PGM_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1


const char*
  SOC_PB_PMF_PGM_SEL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_TYPE enum_val
  );

const char*
  SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR enum_val
  );


void
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_LKP_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  );

void
  SOC_PB_PMF_PGM_SEL_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL *info
  );

void
  SOC_PB_PMF_PGM_SELECTION_ENTRY_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  );

void
  SOC_PB_PMF_PGM_VALIDITY_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO *info
  );

void
  SOC_PB_PMF_PGM_BYTES_TO_RMV_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  );

void
  SOC_PB_PMF_PGM_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_PGM_INCLUDED__*/
#endif

