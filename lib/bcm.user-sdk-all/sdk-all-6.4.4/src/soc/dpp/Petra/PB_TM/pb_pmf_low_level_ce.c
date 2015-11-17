/* $Id: pb_pmf_low_level_ce.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_pmf_low_level.c
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

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
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    pfg_ndx,
    key_ndx,
    instr_ndx;
  SOC_PB_SW_DB_PMF_CE
    sw_db_ce;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  /*
   *	Full the CE keys of invalid instructions for all Lookup-Profiles
   */
  SOC_PETRA_CLEAR(&sw_db_ce, SOC_PB_SW_DB_PMF_CE, 1);
  sw_db_ce.is_valid = FALSE;
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; ++pfg_ndx)
  {
    for (key_ndx = 0; key_ndx <= SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX; ++key_ndx)
    {
      for (instr_ndx = 0; instr_ndx <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++instr_ndx)
      {
         soc_pb_sw_db_pgm_ce_instr_set(
           unit,
           pfg_ndx,
           key_ndx,
           instr_ndx,
           &sw_db_ce
         );
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_ce_init_unsafe()", 0, 0);
}


/*********************************************************************
*     Set an entry in the copy engines table, adding fields
 *     from the packet header to the PMF key. The PMF keys are
 *     constructed by a series of copy engines. Each copy
 *     engine appends field (s) from the packet headers or from
 *     the incoming IRPP information into the PMF key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_packet_header_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;
  int32
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_ce_packet_header_entry_set_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Get-Modify-Write of the table entry (because of the valid bit)
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Set the entry data
   */
  tbl_data.inst_valid = 0x1;
  tbl_data.inst_source_select = SOC_PB_PMF_CE_PACKET_HEADER_FLD_VAL;

  switch(info->sub_header)
  {
  case SOC_PB_PMF_CE_SUB_HEADER_0:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_0_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_1:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_1_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_2:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_2_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_3:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_3_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_4:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_4_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_5:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_5_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_FWD:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_FWD_FLD_VAL;
    break;
  case SOC_PB_PMF_CE_SUB_HEADER_FWD_POST:
    fld_val = SOC_PB_PMF_CE_SUB_HEADER_FWD_POST_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 20, exit);
  }

  tbl_data.inst_header_offset_select = fld_val;

  /*
   *	Write the nibble offset in 2's complement.
   *  The offset is the info.offset MSB
   *  For example, for DA, take 1st instruction as Fwding header, 0 offset, 32 bits
   *  and 2nd instruction as Fwding header, 4-nibble offset, 16 bits
   */
  fld_val = 0;
  offset = info->offset + info->nof_bits - SOC_PB_PMF_CE_INSTRUCTION_NOF_BITS_MAX;
  if (offset >= 0)
  {
    fld_val = (offset / SOC_PB_PMF_CE_OFFSET_RESOLUTION);
  }
  else
  {
    SOC_SAND_SET_BIT(fld_val, 0x1, SOC_PB_PMF_CE_OFFSET_NIBBLE_VAL_SIZE_IN_BITS);
    fld_val += (1 << SOC_PB_PMF_CE_OFFSET_NIBBLE_VAL_SIZE_IN_BITS) - ((-offset) / SOC_PB_PMF_CE_OFFSET_RESOLUTION);
  }
  tbl_data.inst_niblle_field_offset = fld_val;
  tbl_data.inst_bit_count = info->nof_bits - 1;


  res = soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_packet_header_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_packet_header_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_CE_PACKET_HEADER_INFO, info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_packet_header_entry_set_verify()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_packet_header_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_packet_header_entry_get_verify()", pmf_pgm_ndx, pmf_key);
}

/*********************************************************************
*     Set an entry in the copy engines table, adding fields
 *     from the packet header to the PMF key. The PMF keys are
 *     constructed by a series of copy engines. Each copy
 *     engine appends field (s) from the packet headers or from
 *     the incoming IRPP information into the PMF key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_packet_header_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT SOC_PB_PMF_CE_PACKET_HEADER_INFO      *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_CE_PACKET_HEADER_INFO_clear(info);

  res = soc_pb_pmf_ce_packet_header_entry_get_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table entry
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the entry data
   */
  if (tbl_data.inst_source_select == SOC_PB_PMF_CE_PACKET_HEADER_FLD_VAL)
  {
    fld_val = 0;
    switch(tbl_data.inst_header_offset_select)
    {
    case SOC_PB_PMF_CE_SUB_HEADER_0_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_0;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_1_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_1;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_2_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_2;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_3_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_3;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_4_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_4;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_5_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_5;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_FWD_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_FWD;
      break;
    case SOC_PB_PMF_CE_SUB_HEADER_FWD_POST_FLD_VAL:
      fld_val = SOC_PB_PMF_CE_SUB_HEADER_FWD_POST;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 20, exit);
    }
    info->sub_header = fld_val;

    /*
     *	Get the nibble offset (in 2's complement)
     */
    fld_val = 0;
    info->nof_bits = tbl_data.inst_bit_count + 1;
    if (SOC_SAND_GET_BIT(tbl_data.inst_niblle_field_offset, 7) == 0x0)
    {
      info->offset = (tbl_data.inst_niblle_field_offset * SOC_PB_PMF_CE_OFFSET_RESOLUTION);
    }
    else
    {
      /*
       *	Negative offset value
       */
      info->offset = (SOC_SAND_GET_BITS_RANGE(tbl_data.inst_niblle_field_offset,
                                          SOC_PB_PMF_CE_OFFSET_NIBBLE_VAL_SIZE_IN_BITS-1, 0)
                      - (1<< SOC_PB_PMF_CE_OFFSET_NIBBLE_VAL_SIZE_IN_BITS))
                        * SOC_PB_PMF_CE_OFFSET_RESOLUTION;
    }
    /*
     *	Get the offset LSB
     */
    info->offset = info->offset - (info->nof_bits - SOC_PB_PMF_CE_INSTRUCTION_NOF_BITS_MAX);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_packet_header_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}


uint32
  soc_pb_pmf_ce_irpp_field_size_and_val_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD  irpp_field,
    SOC_SAND_OUT uint32                   *fld_size,
    SOC_SAND_OUT uint32                   *fld_map_val
  )
{
  uint32
    fld_val,
    fld_size_in_b;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_IRPP_FIELD_SIZE_AND_VAL_GET);

  switch(irpp_field)
  {
  case SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT32_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_UP:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_UP_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_UP_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_VSI:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VSI_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_VSI_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_VRF:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VRF_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_VRF_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32_SIZE;
    break;
  case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO:
    fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO_FLD_VAL;
    fld_size_in_b = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO_SIZE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_IRPP_FIELD_OUT_OF_RANGE_ERR, 20, exit);
  }

  *fld_size = fld_size_in_b;
  *fld_map_val = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_irpp_field_size_and_val_get()", 0, 0);
}


/*********************************************************************
*     Set an entry in the copy engines table, adding fields
 *     from the IRPP information to the PMF key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD            irpp_field
  )
{
  uint32
    fld_val,
    fld_size_in_b,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_IRPP_INFO_ENTRY_SET_UNSAFE);

  res = soc_pb_pmf_ce_irpp_info_entry_set_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx,
          nof_bits,
          irpp_field
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get-Modify-Write of the table entry (because of the valid bit)
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Set the entry data
   */
  tbl_data.inst_valid = 0x1;
  tbl_data.inst_source_select = SOC_PB_PMF_CE_IRPP_INFO_FLD_VAL;
  tbl_data.inst_header_offset_select = 0;

  /*
   *	Get the field size
   */
  fld_val = 0;
  fld_size_in_b = 0;

  res = soc_pb_pmf_ce_irpp_field_size_and_val_get(
          unit,
          irpp_field,
          &fld_size_in_b,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  tbl_data.inst_niblle_field_offset = fld_val;

  /*
   *	Make the number of bits to copy configurable
   */
  if (nof_bits != 0)
  {
    fld_size_in_b = nof_bits - 1;
  }
  tbl_data.inst_bit_count = fld_size_in_b;


  res = soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_irpp_info_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_irpp_info_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD            irpp_field
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_IRPP_INFO_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(nof_bits, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_OUT_OF_RANGE_ERR, 35, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(irpp_field, SOC_PB_PMF_LOW_LEVEL_IRPP_FIELD_MAX, SOC_PB_PMF_LOW_LEVEL_IRPP_FIELD_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_irpp_info_entry_set_verify()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_irpp_info_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_IRPP_INFO_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_irpp_info_entry_get_verify()", pmf_pgm_ndx, pmf_key);
}

/*********************************************************************
*     Set an entry in the copy engines table, adding fields
 *     from the IRPP information to the PMF key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_irpp_info_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT uint32                             *nof_bits,
    SOC_SAND_OUT SOC_PB_PMF_IRPP_INFO_FIELD               *irpp_field
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_IRPP_INFO_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(irpp_field);

  res = soc_pb_pmf_ce_irpp_info_entry_get_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table entry
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the entry data
   */
  if (tbl_data.inst_source_select == SOC_PB_PMF_CE_IRPP_INFO_FLD_VAL)
  {
    fld_val = 0;
    switch(tbl_data.inst_niblle_field_offset)
    {
    case SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_PP_PORT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PP_CONTEXT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_UP_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_UP;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SNOOP_CODE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_DEST;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_ASD;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_NDX;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_VSI_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VSI;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SYSTEM_VSI;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_STP_STATE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_CODE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_RIF;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_VRF_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_VRF;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_MY_BMAC;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_TTL;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_L4_OPS;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM1;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM2;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_CUSTOM3;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_FLD_VAL:
      switch(tbl_data.inst_bit_count)
      {
      case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8_SIZE:
        fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_8;
        break;
      case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16_SIZE:
        fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16;
        break;
      case SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32_SIZE:
      default:
        fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32;
        break;
      }
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32;
      break;
    case SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO_FLD_VAL:
      fld_val = SOC_PB_PMF_IRPP_INFO_FIELD_LEARN_INFO;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_IRPP_FIELD_OUT_OF_RANGE_ERR, 20, exit);
    }
    *nof_bits = tbl_data.inst_bit_count + 1;
    *irpp_field = fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_irpp_info_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}

/*********************************************************************
*     Set an entry in the copy engines table to be invalid.
 *     Invalid entries are bypassed, and do not affect the PMF
 *     key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_nop_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_not_valid
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_NOP_ENTRY_SET_UNSAFE);

  res = soc_pb_pmf_ce_nop_entry_set_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx,
          is_ce_not_valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Get-Modify-Write of the table entry
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_ce_not_valid == TRUE)
  {
    tbl_data.inst_valid = FALSE;
  }
  else
  {
    tbl_data.inst_valid = TRUE;
  }

  res = soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_nop_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_nop_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_not_valid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_NOP_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_nop_entry_set_verify()", pmf_pgm_ndx, pmf_key);
}

uint32
  soc_pb_pmf_ce_nop_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_NOP_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_nop_entry_get_verify()", pmf_pgm_ndx, pmf_key);
}

/*********************************************************************
*     Set an entry in the copy engines table to be invalid.
 *     Invalid entries are bypassed, and do not affect the PMF
 *     key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_ce_nop_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_OUT uint8                            *is_ce_not_valid,
    SOC_SAND_OUT uint32                             *loc
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_CE_NOP_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_ce_not_valid);
  SOC_SAND_CHECK_NULL_INPUT(loc);

  res = soc_pb_pmf_ce_nop_entry_get_verify(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          ce_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table entry
   */
  res = soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
          unit,
          pmf_key,
          pmf_pgm_ndx,
          ce_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (tbl_data.inst_valid == FALSE)
  {
    *is_ce_not_valid = TRUE;
  }
  else
  {
    *is_ce_not_valid = FALSE;
    *loc = (tbl_data.inst_source_select == 1) ? 0 : 1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_ce_nop_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_header, SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_MAX, SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 10, exit);
  if ((info->offset > SOC_PB_PMF_LOW_LEVEL_OFFSET_MAX) || (info->offset < SOC_PB_PMF_LOW_LEVEL_OFFSET_MIN))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_OFFSET_OUT_OF_RANGE_ERR, 11, exit);
  }
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MIN, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_MAX, SOC_PB_PMF_LOW_LEVEL_NOF_BITS_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_CE_PACKET_HEADER_INFO_verify()",0,0);
}

void
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  SOC_PB_PMF_CE_SUB_HEADER enum_val
  )
{
  return SOC_TMC_PMF_CE_SUB_HEADER_to_string(enum_val);
}


const char*
  SOC_PB_PMF_IRPP_INFO_FIELD_to_string(
    SOC_SAND_IN  SOC_PB_PMF_IRPP_INFO_FIELD enum_val
  )
{
  return SOC_TMC_PMF_IRPP_INFO_FIELD_to_string(enum_val);
}


void
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

