/* $Id: arad_pmf_low_level_ce.h,v 1.14 Broadcom SDK $
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

#ifndef __ARAD_PMF_LOW_LEVEL_CE_INCLUDED__
/* { */
#define __ARAD_PMF_LOW_LEVEL_CE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


typedef SOC_TMC_PMF_CE_PACKET_HEADER_INFO                      ARAD_PMF_CE_PACKET_HEADER_INFO;

/*
 *	Restrict the CE to 5 instructions per cycle temporarily
 */
#define ARAD_PMF_ETH_CE_NOF_INSTR_ZONES  (5)

/* 2 groups per LSB, each one of 8 instructions (4*32b and 4*16b)*/
#define ARAD_PMF_CE_NOF_GROUP_PER_LSB    (2)

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

/*
 * Useful function to get: 
 * - if the field with these bits can be inserted in a Copy Engine 
 * - how many bits it will take 
 */
uint32
  arad_pmf_ce_internal_field_offset_qual_find(
	  SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_IN  uint32		                   nof_bits,
	  SOC_SAND_IN  uint32						   ce_offset,
	  SOC_SAND_IN  uint8		                   is_32b_ce,
      SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD         irpp_field_in,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT  ARAD_PMF_IRPP_INFO_FIELD           *irpp_field,
	  SOC_SAND_OUT  uint32		                   *qual_lsb,
	  SOC_SAND_OUT  uint32		                   *qual_nof_bits
 );


/*
 * Get how many bits it will take in the CE for header field
 */
uint32
  arad_pmf_ce_nof_real_bits_compute_header(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info, /* Offset and number of bits */
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_IN  uint8		                   is_32b_ce,
	  SOC_SAND_OUT uint32						   *nof_bits_for_ce /* Can exceed what the Copy Engine tolerates */
  );

uint32
  arad_pmf_ce_internal_field_offset_compute(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD        irpp_field,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_IN  uint32		                   nof_bits,
      SOC_SAND_IN  uint32		                   qual_lsb,
      SOC_SAND_IN  uint32                             lost_bits, /* mandatory lost bits */
      SOC_SAND_IN  uint8		                   is_32b_ce,
      SOC_SAND_OUT uint8                           *is_found,
      SOC_SAND_OUT uint32						   *ce_offset,
      SOC_SAND_OUT uint32						   *nof_bits_for_ce,
      SOC_SAND_OUT uint32                          *hw_buffer_jericho /* Valid from Jericho: LSB or MSB buffer for HW configuration only */
  );


uint8
  arad_pmf_low_level_ce_is_32b_ce(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  );

uint8
  arad_pmf_low_level_ce_is_second_group(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  );

uint32
  arad_pmf_ce_header_info_find(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD            header_field,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT ARAD_PMF_CE_HEADER_QUAL_INFO	      *qual_info
  );

/*
 * Get if this field is an internal qualifier and its number of bits
 */
uint32
  arad_pmf_ce_internal_field_info_find(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD            irpp_field,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT ARAD_PMF_CE_IRPP_QUALIFIER_INFO     *qual_info
  );

soc_field_t
  arad_pmf_ce_valid_fld_group_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint32                    ce_group_ndx,
    SOC_SAND_IN  uint32                    pmf_key
  );

soc_mem_t
  arad_pmf_ce_table_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint8                     is_ce_in_msb,
    SOC_SAND_IN  uint8                     is_second_lookup,
    SOC_SAND_IN  uint8                     ce_ndx
  );


uint32
  arad_pmf_low_level_ce_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pmf_ce_entry_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     pmf_pgm_ndx,
    SOC_SAND_IN  uint32                     pmf_key,
    SOC_SAND_IN  uint32                     ce_ndx,
    SOC_SAND_IN  uint8                      is_ce_in_msb,
    SOC_SAND_IN  uint8                      is_second_lookup
  );


/*********************************************************************
* NAME:
 *   arad_pmf_ce_packet_header_entry_set_unsafe
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
 *     PMF-Program-ID. Range: 0 - 31. (Arad-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Arad-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Arad-B) The Copy-Engine
 *     '0' set the MSB of the output vector, '1' is appended to
 *     the output vector etc.
 *   SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info -
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
  arad_pmf_ce_packet_header_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pmf_ce_packet_header_entry_set_unsafe" API.
 *     Refer to "arad_pmf_ce_packet_header_entry_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  arad_pmf_ce_packet_header_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  );

/*********************************************************************
* NAME:
 *   arad_pmf_ce_internal_info_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set an entry in the copy engines table, adding fields
 *   from the IRPP information to the PMF key.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            pmf_pgm_ndx -
 *     PMF-Program-ID. Range: 0 - 31. (Arad-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Arad-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Arad-B) The Copy-Engine
 *     '0' set the MSB of the output vector, '1' is appended to
 *     the output vector etc.
 *   SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD            irpp_field -
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
  arad_pmf_ce_internal_info_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
	SOC_SAND_IN  uint32                             qual_lsb,
    SOC_SAND_IN  uint32                             lost_bits, /* mandatory lost bits */
	SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD            irpp_field
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pmf_ce_internal_info_entry_set_unsafe" API.
 *     Refer to "arad_pmf_ce_internal_info_entry_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pmf_ce_internal_info_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD         irpp_field_in,
	SOC_SAND_OUT  uint32                            *qual_lsb,
	SOC_SAND_OUT uint32                             *nof_bits,
    SOC_SAND_OUT ARAD_PMF_IRPP_INFO_FIELD             *irpp_field
  );

/*********************************************************************
* NAME:
 *   arad_pmf_ce_nop_entry_set_unsafe
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
 *     PMF-Program-ID. Range: 0 - 31. (Arad-B)
 *   SOC_SAND_IN  uint32                            pmf_key -
 *     PMF-Key-ID (output of the Copy Engine). Range: 0 - 1.
 *     (Arad-B)
 *   SOC_SAND_IN  uint32                            ce_ndx -
 *     Copy-Engine-ID. Range: 0 - 7. (Arad-B) The Copy-Engine
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
  arad_pmf_ce_nop_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
	SOC_SAND_IN  uint8                            is_ce_not_valid
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pmf_ce_nop_entry_set_unsafe" API.
 *     Refer to "arad_pmf_ce_nop_entry_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pmf_ce_nop_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
	SOC_SAND_OUT uint8                            *is_ce_not_valid,
    SOC_SAND_OUT uint32                             *loc
  );

uint32
  ARAD_PMF_CE_PACKET_HEADER_INFO_verify(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info,
    SOC_SAND_IN  uint32						ce_ndx
  );

void
  ARAD_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  ARAD_PMF_CE_SUB_HEADER enum_val
  );

const char*
  ARAD_PMF_IRPP_INFO_FIELD_to_string(
    SOC_SAND_IN  ARAD_PMF_IRPP_INFO_FIELD enum_val
  );

void
  ARAD_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info
  );

#endif /* ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PMF_LOW_LEVEL_CE_INCLUDED__*/
#endif





