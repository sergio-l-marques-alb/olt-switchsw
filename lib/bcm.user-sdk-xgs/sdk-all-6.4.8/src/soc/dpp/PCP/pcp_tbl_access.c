/* $Id: pcp_tbl_access.c,v 1.9 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_chip_tbls.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define PCP_TBL_SIZE_CHECK                                       \
  if (offset >= tbl->addr.base + tbl->addr.size)                    \
    {                                                                 \
      SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 23, exit); \
    }


#define PCP_TBL_LPM_MEM_SZIE (0x80000)





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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_tbl_access[] =
{
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_QDR_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_QDR_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD1_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD1_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD2_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD2_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_SYS_PORT_IS_MINE_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_SYS_PORT_IS_MINE_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_AGING_CFG_TABLE_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_AGING_CFG_TABLE_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FLUSH_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FLUSH_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_DSP_EVENT_TABLE_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_DSP_EVENT_TABLE_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_PR2_TCDP_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_PR2_TCDP_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MEP_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MEP_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_0_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_0_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_1_DB_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_1_DB_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_CMN_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_CMN_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_TX_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_TX_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_TX_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_TX_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_RX_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_RX_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTERS_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTERS_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTERS_TBL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTERS_TBL_SET),


  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_FIELD_IN_PLACE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_FIELD_IN_PLACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_SET_REPS_FOR_TBL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_SET_REPS_FOR_TBL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_SET_REPS_FOR_TBL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_SET_REPS_FOR_TBL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_QDR_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_QDR_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD1_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD1_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD2_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ECI_RLD2_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FID_COUNTER_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_SYS_PORT_IS_MINE_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_SYS_PORT_IS_MINE_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_AGING_CFG_TABLE_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_AGING_CFG_TABLE_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FLUSH_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_FLUSH_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_DSP_EVENT_TABLE_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_ELK_DSP_EVENT_TABLE_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_PR2_TCDP_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_PR2_TCDP_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_EXT_MEP_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MEP_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MEP_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_0_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_0_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_1_DB_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_RMEP_HASH_1_DB_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_CMN_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_CMN_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_TX_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_LMDB_TX_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_TX_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_TX_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_RX_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_DMDB_RX_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTERS_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTERS_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTERS_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTERS_TBL_SET_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_tbl_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_TBL_RANGE_OUT_OF_LIMIT_ERR,
    "PCP_TBL_RANGE_OUT_OF_LIMIT_ERR",
    "pcp table offset is out of range.\n\r "
    "offset is not in table size.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_REGS_FIELD_VAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_REGS_FIELD_VAL_OUT_OF_RANGE_ERR",
    "pcp reg val is out of range.\n\r "
    "field value is above max value.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */


STATIC uint32
  pcp_tbl_lpm_rldram_addr_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32 rld_dram ,
    SOC_SAND_IN  uint32 bank, 
    SOC_SAND_IN  uint32 bank_entry, 
    SOC_SAND_IN  uint8 exact_match_present, 
    SOC_SAND_IN  uint8 small_dram
 )
{
  uint32   addr = bank_entry;
  if(exact_match_present)
  {
   addr |= (small_dram)?(1 << 18):(1 << 19);
  }
  addr |= (bank << 20);
  addr |= (rld_dram << 23);
  return addr;
}


uint32
  pcp_field_in_place_set(
    SOC_SAND_IN  uint32           *src_data,
    SOC_SAND_IN  PCP_TBL_FIELD    *field,
    SOC_SAND_OUT uint32           *dst_data
  )
{
  uint32
    fld_lsb,
    fld_msb,
    fld_size = 0,
    last_data,
    last_data_idx,
    last_fld_max;
  uint32
    res = SOC_SAND_OK;
  PCP_REG_FIELD
    last_reg_fld;


  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_FIELD_IN_PLACE_SET);

  SOC_SAND_CHECK_NULL_INPUT(src_data);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(dst_data);

  fld_msb = field->msb;
  fld_lsb = field->lsb;

  fld_size = PCP_FLD_SIZE_BITS(fld_msb, fld_lsb);

  /*
   *	Verify no overflow, check
   *  the last significant uint32 of the src_data
   */

  last_reg_fld.lsb = 0;
  last_reg_fld.msb = (uint8)((fld_msb - fld_lsb) % SOC_SAND_REG_SIZE_BITS);
  last_fld_max = PCP_FLD_MAX(last_reg_fld);

  last_data_idx = (fld_size - 1) / SOC_SAND_REG_SIZE_BITS;
  last_data = src_data[last_data_idx];
  
  if (last_data > last_fld_max)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_OAM_REGS_FIELD_VAL_OUT_OF_RANGE_ERR, 5, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          src_data,
          fld_lsb,
          fld_size,
          dst_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_field_in_place_set()",0,0);
}


uint32
  pcp_field_in_place_get(
    SOC_SAND_IN  uint32           *src_data,
    SOC_SAND_IN  PCP_TBL_FIELD   *field,
    SOC_SAND_OUT uint32           *dst_data
  )
{
  uint32
    fld_lsb,
    fld_msb,
    fld_size = 0;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_FIELD_IN_PLACE_GET);

  SOC_SAND_CHECK_NULL_INPUT(src_data);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(dst_data);

  fld_msb = field->msb;
  fld_lsb = field->lsb;

  fld_size = PCP_FLD_SIZE_BITS(fld_msb, fld_lsb);

  res = soc_sand_bitstream_get_any_field(
          src_data,
          fld_lsb,
          fld_size,
          dst_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_field_in_place_get()",0,0);
}


uint32
  pcp_eci_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  PCP_REGS
    *regs = pcp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_SET_REPS_FOR_TBL_UNSAFE);

  res = pcp_write_fld_unsafe(
          unit,
          &(regs->eci.indirect_command_reg.indirect_command_count),
          PCP_DEFAULT_INSTANCE,
          nof_reps + 1 /*Hardware will decrease by 1 in setting trigger to 1*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

uint32
  pcp_elk_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  PCP_REGS
    *regs = pcp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_SET_REPS_FOR_TBL_UNSAFE);

  res = pcp_write_fld_unsafe(
          unit,
          &(regs->elk.indirect_command_reg.indirect_command_count),
          PCP_DEFAULT_INSTANCE,
          nof_reps + 1 /*Hardware will decrease by 1 in setting trigger to 1*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

uint32
  pcp_oam_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  PCP_REGS
    *regs = pcp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_SET_REPS_FOR_TBL_UNSAFE);

  res = pcp_write_fld_unsafe(
          unit,
          &(regs->oam.indirect_command_reg.indirect_command_count),
          PCP_DEFAULT_INSTANCE,
          nof_reps + 1 /*Hardware will decrease by 1 in setting trigger to 1*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

uint32
  pcp_sts_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  PCP_REGS
    *regs = pcp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_SET_REPS_FOR_TBL_UNSAFE);

  res = pcp_write_fld_unsafe(
          unit,
          &(regs->sts.indirect_command_reg.indirect_command_count),
          PCP_DEFAULT_INSTANCE,
          nof_reps + 1 /*Hardware will decrease by 1 in setting trigger to 1*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_set_reps_for_tbl_unsafe()", nof_reps, 0);
}
uint32
  pcp_eci_qdr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_QDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_QDR_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_QDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_QDR_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_QDR_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ECI_QDR_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.qdr_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_QDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->qdr_data),
          &(tbl_data->qdr_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_qdr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_eci_qdr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_QDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_QDR_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_QDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_QDR_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_QDR_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.qdr_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->qdr_data),
          &(tbl->qdr_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_QDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_qdr_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_eci_rld1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD1_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD1_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ECI_RLD1_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld1_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rld1_data),
          (tbl_data->rld1_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_eci_rld1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD1_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD1_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld1_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->rld1_data),
          &(tbl->rld1_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_eci_rld2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD2_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD2_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ECI_RLD2_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld2_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rld2_data),
          (tbl_data->rld2_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_eci_rld2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD2_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD2_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld2_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->rld2_data),
          &(tbl->rld2_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld2_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  pcp_eci_rld_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD1_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD1_TBL
    *tbl;
  PCP_ECI_RLD2_TBL
    *tbl2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD1_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld1_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->rld1_data),
          &(tbl->rld1_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* rldram 0, first bank */
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  /* rldram 0, second bank */
  offset += 0x100000;
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  /* rldram 1, first bank */
  tbl2 = &(tables->eci.rld2_tbl);
  offset =  tbl2->addr.base + entry_offset;
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  /* rldram 1, second bank */
  offset += 0x100000;
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld2_tbl_set_unsafe()", entry_offset, 0);
}



/* given rld-dram, bank, entry in bank, and part of data, calculate the address and write required part of the data */
uint32
  pcp_eci_rld_helper_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              rld_ndx,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA *tbl_data,/* data in lsbs */
    SOC_SAND_IN  uint32              side /*1-left, 2-right, 3-both */
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD2_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD2_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld2_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->rld1_data),
          &(tbl->rld2_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld_helper_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  pcp_eci_rld3_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD2_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD3_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD2_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ECI_RLD1_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->eci.rld3_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rld3_data),
          (tbl_data->rld1_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld3_tbl_get_unsafe()", entry_offset, 0);
}



uint32
  pcp_eci_rld3_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ECI_RLD1_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ECI_RLD3_TBL
    *tbl3;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD1_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ECI_RLD1_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl3 = &(tables->eci.rld3_tbl);

  offset =  tbl3->addr.base + entry_offset;

  if (offset >= tbl3->addr.base + tbl3->addr.size)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 23, exit); 
  }


  res = pcp_field_in_place_set(
          (tbl_data->rld1_data),
          &(tbl3->rld3_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* rldram 3, first bank */
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  /* rldram 3, second bank */
  offset += 0x100000;
  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID,
          PCP_ECI_RLD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld3_tbl_set_unsafe()", entry_offset, 0);
}





uint32
  pcp_eci_rld_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              mem_ndx,      /* LPM bank */
    SOC_SAND_IN  uint32              entry_offset, /* relative to the bank, consider entry size is 36 and not 72 */
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA *tbl_data, /* includes lpm data, size is 36 bits*/
    SOC_SAND_IN  uint8             full/*0-only 36 lsb is valid, 1-entry in include full line*/
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 cur_bank;
  PCP_SW_DB_LPM_INFO
    lpm_hw_info;
  uint8
    small_dram, /* small/large rldram */
    em_present; /* exact match in banks */
  uint32 
    mems_in_bank,
    data_side; /* right to high banks */
  uint32
    bank_size,
    addr,
    entry_in_bank;
  PCP_ECI_RLD1_TBL_DATA  
    tbl_old_data,
    tbl_new_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_SET_UNSAFE);

  res = pcp_sw_db_ipv4_lpm_hw_info_get(
          unit,
          &lpm_hw_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (lpm_hw_info.lkup_mode == PCP_MGMT_ELK_LKP_MODE_EM)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }

  em_present = (lpm_hw_info.lkup_mode == PCP_MGMT_ELK_LKP_MODE_BOTH)?1:0;  /* _isexact_match */
  small_dram = (lpm_hw_info.dram_size == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288)?1:0; /* is_small */


  /* bank size is 2M or 1M according to rldram size */
  if (small_dram)
  {
    bank_size = 0x100000;
  }
  else
  {
    bank_size = 0x200000;
  }
  /* if also for ELK usage then half of it for LPM */
  if (em_present)
  {
    bank_size /= 2;
  }

  /* if memory 0, then can be in bank 0 or 6 according to entry msb*/
  if (mem_ndx == 0)
  {
    if (entry_offset < bank_size)
    {
      cur_bank = 0;
      entry_in_bank = entry_offset;
    }
    else if (entry_offset < 2*bank_size)
    {
      cur_bank = 6;
      entry_in_bank = entry_offset - bank_size;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 7, exit);
    }
  }
  else /* otherwise , for other banks */
  {
    /* how many mems in bank */
    mems_in_bank = (uint32)(bank_size / PCP_TBL_LPM_MEM_SZIE);
    /* cur bank according to memory index */
    cur_bank = 1 + (uint32)((mem_ndx-1)/mems_in_bank);
    if (cur_bank >= 3)
    {
      /* shouldnot be this call */
      SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
    }
    cur_bank *= 2;

    /* check bank <= 2 */
    if (cur_bank > 7)
    {
      SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
    }
    /* entry in bank = (mem_ndx-1)%mems_in_bank * size of mem (0.5 Mega )  */
    entry_in_bank = ((mem_ndx-1)%mems_in_bank) * PCP_TBL_LPM_MEM_SZIE + entry_offset;
  }

  data_side = entry_in_bank  % 2; /* data set in msb (1) or lsb (0) */
  entry_in_bank /= 2; /* entry in 72b bank */

  /* build address */
  addr = pcp_tbl_lpm_rldram_addr_get(
            unit,
            0,  /* rldram, as done in rldram write */
            cur_bank, /* bank */
            entry_in_bank, /* entry in 72b bank */
            em_present,  /* _isexact_match */
            small_dram /* is_small */
         );

 /* read full, line entry divide by 2, if needed */
  if(!full)
  {
    /* update correct half */
    res = pcp_eci_rld1_tbl_get_unsafe(
            unit,
            addr,
            &tbl_old_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = soc_sand_bitstream_set_any_field(
            tbl_data->rld1_data,
            data_side*36,
            36,
            tbl_old_data.rld1_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

    tbl_new_data = tbl_old_data;
  }
  else
  {
    tbl_new_data = *tbl_data;
  }

  /* write full entry, also to peer rldram x bank  (+1) */
  res = pcp_eci_rld_tbl_set_unsafe(
          unit,
          addr,
          &tbl_new_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_rld_lpm_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  pcp_eci_lpm4_tbl_set_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32             mem_ndx,      /* not in use */
    SOC_SAND_IN  uint32             entry_offset, /* relative to the bank, consider entry size is 36 and not 72 */
    SOC_SAND_IN  uint32             tbl_data[3], /* includes lpm data, size is 36 bits*/
    SOC_SAND_IN  uint32             flags/*0-only 36 lsb is valid, 1-entry in include full line*/
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 cur_bank = 3;
  PCP_SW_DB_LPM_INFO
    lpm_hw_info;
  uint8
    small_dram, /* small/large rldram */
    em_present, /* exact match in banks */
    full = FALSE;
  uint32 
    data_side; /* right to high banks */
  uint32
    bank_size,
    addr,
    entry_in_bank = entry_offset;
  PCP_ECI_RLD1_TBL_DATA  
    tbl_old_data,
    tbl_new_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ECI_RLD2_TBL_SET_UNSAFE);

  res = pcp_sw_db_ipv4_lpm_hw_info_get(
          unit,
          &lpm_hw_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (lpm_hw_info.lkup_mode == PCP_MGMT_ELK_LKP_MODE_EM)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }

  em_present = (lpm_hw_info.lkup_mode == PCP_MGMT_ELK_LKP_MODE_BOTH)?1:0;  /* _isexact_match */
  small_dram = (lpm_hw_info.dram_size == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288)?1:0; /* is_small */


  /* bank size is 2M or 1M according to rldram size */
  if (small_dram)
  {
    bank_size = 0x100000;
  }
  else
  {
    bank_size = 0x200000;
  }
  /* if also for ELK usage then half of it for LPM */
  if (em_present)
  {
    bank_size /= 2;
  }

  data_side = entry_in_bank  % 2; /* data set in msb (1) or lsb (0) */
  entry_in_bank /= 2; /* entry in 72b bank */

  /* build address */
  addr = pcp_tbl_lpm_rldram_addr_get(
            unit,
            0,  /* rldram, as done in rldram write */
            cur_bank, /* bank */
            entry_in_bank, /* entry in 72b bank */
            em_present,  /* _isexact_match */
            small_dram /* is_small */
         );

 /* read full, line entry divide by 2, if needed */
  if(!full)
  {
    /* update correct half */
    res = pcp_eci_rld3_tbl_get_unsafe(
            unit,
            addr,
            &tbl_old_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = soc_sand_bitstream_set_any_field(
            tbl_data,
            data_side*36,
            36,
            tbl_old_data.rld1_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

    tbl_new_data = tbl_old_data;
  }
  else
  {
    PCP_COPY(&tbl_new_data,tbl_data, PCP_ECI_RLD1_TBL_DATA, 1);
  }

  /* write full entry, also to peer rldram x bank  */
  res = pcp_eci_rld3_tbl_set_unsafe(
          unit,
          addr,
          &tbl_new_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_eci_lpm4_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  pcp_elk_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.fid_counter_profile_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->limit),
          &(tbl_data->limit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->interrupt_en),
          &(tbl_data->interrupt_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->limit_en),
          &(tbl_data->limit_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->is_ll_fid),
          &(tbl_data->is_ll_fid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_fid_counter_profile_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.fid_counter_profile_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->limit),
          &(tbl->limit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->interrupt_en),
          &(tbl->interrupt_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->limit_en),
          &(tbl->limit_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->is_ll_fid),
          &(tbl->is_ll_fid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_fid_counter_profile_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FID_COUNTER_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_FID_COUNTER_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.fid_counter_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->entry_count),
          &(tbl_data->entry_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->profile_pointer),
          &(tbl_data->profile_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_fid_counter_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FID_COUNTER_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FID_COUNTER_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.fid_counter_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->entry_count),
          &(tbl->entry_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->profile_pointer),
          &(tbl->profile_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_fid_counter_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_sys_port_is_mine_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_SYS_PORT_IS_MINE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_SYS_PORT_IS_MINE_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.sys_port_is_mine_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->port_is_mine),
          &(tbl_data->port_is_mine)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_sys_port_is_mine_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_sys_port_is_mine_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_SYS_PORT_IS_MINE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_SYS_PORT_IS_MINE_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.sys_port_is_mine_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->port_is_mine),
          &(tbl->port_is_mine),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_sys_port_is_mine_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_aging_cfg_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_AGING_CFG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_AGING_CFG_TABLE_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_AGING_CFG_TABLE_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.aging_cfg_table_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_delete_entry[FALSE]),
          &(tbl_data->aging_cfg_info_delete_entry[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_create_aged_out_event[FALSE]),
          &(tbl_data->aging_cfg_info_create_aged_out_event[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_create_refresh_event[FALSE]),
          &(tbl_data->aging_cfg_info_create_refresh_event[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_delete_entry[TRUE]),
          &(tbl_data->aging_cfg_info_delete_entry[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_create_aged_out_event[TRUE]),
          &(tbl_data->aging_cfg_info_create_aged_out_event[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->aging_cfg_info_create_refresh_event[TRUE]),
          &(tbl_data->aging_cfg_info_create_refresh_event[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_aging_cfg_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_aging_cfg_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_AGING_CFG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_AGING_CFG_TABLE_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.aging_cfg_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_delete_entry[FALSE]),
          &(tbl->aging_cfg_info_delete_entry[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_create_aged_out_event[FALSE]),
          &(tbl->aging_cfg_info_create_aged_out_event[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_create_refresh_event[FALSE]),
          &(tbl->aging_cfg_info_create_refresh_event[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_delete_entry[TRUE]),
          &(tbl->aging_cfg_info_delete_entry[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_create_aged_out_event[TRUE]),
          &(tbl->aging_cfg_info_create_aged_out_event[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->aging_cfg_info_create_refresh_event[TRUE]),
          &(tbl->aging_cfg_info_create_refresh_event[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);



  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_aging_cfg_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FLUSH_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FLUSH_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_FLUSH_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.flush_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->compare_valid),
          &(tbl_data->compare_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->key_fid),
          &(tbl_data->key_fid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->key_mask),
          &(tbl_data->key_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->payload),
          (tbl_data->payload)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->payload_mask),
          (tbl_data->payload_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->drop),
          &(tbl_data->drop)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->new_payload),
          (tbl_data->new_payload)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->new_payload_mask),
          (tbl_data->new_payload_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_flush_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_FLUSH_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_FLUSH_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.flush_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->compare_valid),
          &(tbl->compare_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->key_fid),
          &(tbl->key_fid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->key_mask),
          &(tbl->key_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          (tbl_data->payload),
          &(tbl->payload),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = pcp_field_in_place_set(
          (tbl_data->payload_mask),
          &(tbl->payload_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->drop),
          &(tbl->drop),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = pcp_field_in_place_set(
          (tbl_data->new_payload),
          &(tbl->new_payload),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = pcp_field_in_place_set(
          (tbl_data->new_payload_mask),
          &(tbl->new_payload_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_flush_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_dsp_event_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_DSP_EVENT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_DSP_EVENT_TABLE_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_ELK_DSP_EVENT_TABLE_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.dsp_event_table_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->to_lpbck),
          &(tbl_data->to_lpbck)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->to_fifo2),
          &(tbl_data->to_fifo2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->to_fifo1),
          &(tbl_data->to_fifo1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_dsp_event_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_elk_dsp_event_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_ELK_DSP_EVENT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ELK_DSP_EVENT_TABLE_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->elk.dsp_event_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->to_lpbck),
          &(tbl->to_lpbck),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->to_fifo2),
          &(tbl->to_fifo2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->to_fifo1),
          &(tbl->to_fifo1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

   res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ELK_ID,
          PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_elk_dsp_event_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_pr2_tcdp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_PR2_TCDP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_PR2_TCDP_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_PR2_TCDP_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.pr2_tcdp_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tc),
          &(tbl_data->tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dp),
          &(tbl_data->dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_pr2_tcdp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_pr2_tcdp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_PR2_TCDP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_PR2_TCDP_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.pr2_tcdp_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->tc),
          &(tbl->tc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->dp),
          &(tbl->dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_pr2_tcdp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_ext_mep_index_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_EXT_MEP_INDEX_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.ext_mep_index_db_tbl);

  /* extended table: belong to oam but implemented in eci qdr*/
  offset = tables->eci.qdr_tbl.addr.base + tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID, /* extended table: belong to oam but implemented in eci qdr*/
          PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->mep_db_ptr),
          &(tbl_data->mep_db_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->accelerate),
          &(tbl_data->accelerate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_ext_mep_index_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_ext_mep_index_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_EXT_MEP_INDEX_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.ext_mep_index_db_tbl);

  /* extended table: belong to oam but implemented in eci qdr*/
  offset = tables->eci.qdr_tbl.addr.base + tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->mep_db_ptr),
          &(tbl->mep_db_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->accelerate),
          &(tbl->accelerate),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_ECI_ID, /* extended table: belong to oam but implemented in eci qdr*/
          PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_ext_mep_index_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_ext_mep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    local_entry_offset = 0,
    offset  = 0,
    data_offset = 0,
    tmp_data[PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE],
    data[PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_EXT_MEP_DB_TBL
    *tbl;
  uint16
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tmp_data, uint32, PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_EXT_MEP_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.ext_mep_db_tbl);

  /* special table: 
   * memory width bits = tables->eci.qdr_tbl.addr.width_bits = 18, 
   * data = tbl->addr.width_bits = 72. 
   * every entry is in jump of 4 offsets
   */
  local_entry_offset = 4 * entry_offset;

  /* extended table: belong to oam but implemented in eci qdr */
  offset = tables->eci.qdr_tbl.addr.base + tbl->addr.base + local_entry_offset;

  if (offset >= tables->eci.qdr_tbl.addr.base + tables->eci.qdr_tbl.addr.size)
  { 
    SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 23, exit);
  }

  for (i=0 ; i < (tbl->addr.width_bits / tables->eci.qdr_tbl.addr.width_bits) ; i++)
  {
    res = soc_sand_tbl_read_unsafe(
            unit,
            tmp_data,
            offset + i,
            sizeof(data),
            PCP_ECI_ID, /* extended table: belong to oam but implemented in eci qdr */
            PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_sand_bitstream_set_any_field(tmp_data, data_offset, tables->eci.qdr_tbl.addr.width_bits, data);
    data_offset += tables->eci.qdr_tbl.addr.width_bits;
  }

  res = pcp_field_in_place_get(
          data,
          &(tbl->out_ac),
          &(tbl_data->out_ac)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->mepid),
          &(tbl_data->mepid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->vid_vld),
          &(tbl_data->vid_vld)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->vid),
          &(tbl_data->vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ptc_header),
          &(tbl_data->ptc_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_ext_mep_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_ext_mep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    local_entry_offset = 0,
    offset  = 0,
    data_offset = 0,
    tmp_data[PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE],
    data[PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_EXT_MEP_DB_TBL
    *tbl;
  uint16
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_EXT_MEP_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tmp_data, uint32, PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.ext_mep_db_tbl);

  /* special table: 
   * memory width bits = tables->eci.qdr_tbl.addr.width_bits = 18, 
   * data = tbl->addr.width_bits = 72. 
   * every entry is in jump of 4 offsets
   */
  local_entry_offset = 4 * entry_offset;

  /* extended table: belong to oam but implemented in eci qdr*/
  offset = tables->eci.qdr_tbl.addr.base + tbl->addr.base + local_entry_offset;
  
  if (offset >= tables->eci.qdr_tbl.addr.base + tables->eci.qdr_tbl.addr.size)
  { 
    SOC_SAND_SET_ERROR_CODE(PCP_TBL_RANGE_OUT_OF_LIMIT_ERR, 23, exit);
  }

  res = pcp_field_in_place_set(
          &(tbl_data->out_ac),
          &(tbl->out_ac),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->mepid),
          &(tbl->mepid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->vid_vld),
          &(tbl->vid_vld),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->vid),
          &(tbl->vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->ptc_header),
          &(tbl->ptc_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  for (i=0 ; i < (tbl->addr.width_bits / tables->eci.qdr_tbl.addr.width_bits) ; i++)
  {
    soc_sand_bitstream_get_any_field(data, data_offset, tables->eci.qdr_tbl.addr.width_bits, tmp_data);
    data_offset += tables->eci.qdr_tbl.addr.width_bits;

    res = soc_sand_tbl_write_unsafe(
            unit,
            tmp_data,
            offset + i,
            sizeof(data),
            PCP_ECI_ID, /* extended table: belong to oam but implemented in eci qdr*/
            PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_ext_mep_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_mep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_MEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_MEP_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MEP_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_MEP_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_MEP_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.mep_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_MEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ccm_interval),
          &(tbl_data->ccm_interval)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->maid),
          &(tbl_data->maid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ccm_tx_start),
          &(tbl_data->ccm_tx_start)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_enable),
          &(tbl_data->dm_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_db_index),
          &(tbl_data->dm_db_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lm_enable),
          &(tbl_data->lm_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lmm_enable),
          &(tbl_data->lmm_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lmr_enable),
          &(tbl_data->lmr_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lm_db_index),
          &(tbl_data->lm_db_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ccm_pr),
          &(tbl_data->ccm_pr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->up1down0),
          &(tbl_data->up1down0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->sa_lsb),
          &(tbl_data->sa_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rdi_indicator),
          &(tbl_data->rdi_indicator)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->md_level),
          &(tbl_data->md_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mep_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_mep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_MEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_MEP_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MEP_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_MEP_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.mep_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->ccm_interval),
          &(tbl->ccm_interval),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->maid),
          &(tbl->maid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->ccm_tx_start),
          &(tbl->ccm_tx_start),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->dm_enable),
          &(tbl->dm_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->dm_db_index),
          &(tbl->dm_db_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->lm_enable),
          &(tbl->lm_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->lmm_enable),
          &(tbl->lmm_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->lmr_enable),
          &(tbl->lmr_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);


  res = pcp_field_in_place_set(
          &(tbl_data->lm_db_index),
          &(tbl->lm_db_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->ccm_pr),
          &(tbl->ccm_pr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->up1down0),
          &(tbl->up1down0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->sa_lsb),
          &(tbl->sa_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rdi_indicator),
          &(tbl->rdi_indicator),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->md_level),
          &(tbl->md_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_MEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mep_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_RMEP_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ccm_interval),
          &(tbl_data->ccm_interval)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->ccm_defect),
          &(tbl_data->ccm_defect)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rcvd_rdi),
          &(tbl_data->rcvd_rdi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->ccm_interval),
          &(tbl->ccm_interval),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->ccm_defect),
          &(tbl->ccm_defect),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rcvd_rdi),
          &(tbl->rcvd_rdi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_HASH_0_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_0_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_RMEP_HASH_0_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_hash_0_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rmep_db_ptr0),
          &(tbl_data->rmep_db_ptr0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->verifier0),
          &(tbl_data->verifier0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->valid_hash0),
          &(tbl_data->valid_hash0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_hash_0_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_HASH_0_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_0_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_hash_0_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->rmep_db_ptr0),
          &(tbl->rmep_db_ptr0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->verifier0),
          &(tbl->verifier0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->valid_hash0),
          &(tbl->valid_hash0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_hash_0_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_HASH_1_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_1_DB_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_RMEP_HASH_1_DB_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_hash_1_db_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rmep_db_ptr1),
          &(tbl_data->rmep_db_ptr1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->verifier1),
          &(tbl_data->verifier1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->valid_hash1),
          &(tbl_data->valid_hash1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_hash_1_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_RMEP_HASH_1_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_RMEP_HASH_1_DB_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.rmep_hash_1_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->rmep_db_ptr1),
          &(tbl->rmep_db_ptr1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->verifier1),
          &(tbl->verifier1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->valid_hash1),
          &(tbl->valid_hash1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_rmep_hash_1_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_lmdb_cmn_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_LMDB_CMN_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_CMN_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_LMDB_CMN_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.lmdb_cmn_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tx_fcf_c),
          &(tbl_data->tx_fcf_c)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tx_fcf_p),
          &(tbl_data->tx_fcf_p)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rx_fcb_peer_c),
          &(tbl_data->rx_fcb_peer_c)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rx_fcb_peer_p),
          &(tbl_data->rx_fcb_peer_p)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rx_fcb_c),
          &(tbl_data->rx_fcb_c)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rx_fcb_p),
          &(tbl_data->rx_fcb_p)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tx_fcb_peer_c),
          &(tbl_data->tx_fcb_peer_c)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tx_fcb_peer_p),
          &(tbl_data->tx_fcb_peer_p)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->rx_fcb_indx_msb),
          &(tbl_data->rx_fcb_indx_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->tx_fcf_indx_msb),
          &(tbl_data->tx_fcf_indx_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lmm_da),
          (tbl_data->lmm_da)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_lmdb_cmn_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_lmdb_cmn_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_LMDB_CMN_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_CMN_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.lmdb_cmn_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->tx_fcf_c),
          &(tbl->tx_fcf_c),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->tx_fcf_p),
          &(tbl->tx_fcf_p),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rx_fcb_peer_c),
          &(tbl->rx_fcb_peer_c),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rx_fcb_peer_p),
          &(tbl->rx_fcb_peer_p),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rx_fcb_c),
          &(tbl->rx_fcb_c),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rx_fcb_p),
          &(tbl->rx_fcb_p),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->tx_fcb_peer_c),
          &(tbl->tx_fcb_peer_c),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->tx_fcb_peer_p),
          &(tbl->tx_fcb_peer_p),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->rx_fcb_indx_msb),
          &(tbl->rx_fcb_indx_msb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->tx_fcf_indx_msb),
          &(tbl->tx_fcf_indx_msb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = pcp_field_in_place_set(
          (tbl_data->lmm_da),
          &(tbl->lmm_da),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_lmdb_cmn_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_lmdb_tx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_LMDB_TX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_TX_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_LMDB_TX_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.lmdb_tx_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lmm_interval),
          &(tbl_data->lmm_interval)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->lm_pr),
          &(tbl_data->lm_pr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_lmdb_tx_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_lmdb_tx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_LMDB_TX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_LMDB_TX_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.lmdb_tx_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->lmm_interval),
          &(tbl->lmm_interval),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->lm_pr),
          &(tbl->lm_pr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_lmdb_tx_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_dmdb_tx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_DMDB_TX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_TX_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_DMDB_TX_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.dmdb_tx_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_interval),
          &(tbl_data->dm_interval)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_da),
          (tbl_data->dm_da)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_pr),
          &(tbl_data->dm_pr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_dmdb_tx_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_dmdb_tx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_DMDB_TX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_TX_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.dmdb_tx_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->dm_interval),
          &(tbl->dm_interval),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          (tbl_data->dm_da),
          &(tbl->dm_da),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->dm_pr),
          &(tbl->dm_pr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_dmdb_tx_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_dmdb_rx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_DMDB_RX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_RX_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_OAM_DMDB_RX_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.dmdb_rx_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_delay),
          &(tbl_data->dm_delay)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->dm_delay_valid),
          &(tbl_data->dm_delay_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_dmdb_rx_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_oam_dmdb_rx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_OAM_DMDB_RX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_DMDB_RX_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->oam.dmdb_rx_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          &(tbl_data->dm_delay),
          &(tbl->dm_delay),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          &(tbl_data->dm_delay_valid),
          &(tbl->dm_delay_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_OAM_ID,
          PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_dmdb_rx_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_sts_ing_counters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_STS_ING_COUNTERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTERS_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_STS_ING_COUNTERS_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->sts.ing_counters_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_STS_ID,
          PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->packets),
          (tbl_data->packets)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->octets),
          (tbl_data->octets)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_ing_counters_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_sts_ing_counters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_STS_ING_COUNTERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTERS_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->sts.ing_counters_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->packets),
          &(tbl->packets),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_set(
          (tbl_data->octets),
          &(tbl->octets),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_STS_ID,
          PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_ing_counters_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  pcp_sts_egr_counters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_STS_EGR_COUNTERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTERS_TBL_GET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE);
  PCP_CLEAR(tbl_data, PCP_STS_EGR_COUNTERS_TBL_DATA, 1);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->sts.egr_counters_tbl);

  offset = tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_STS_ID,
          PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_field_in_place_get(
          data,
          &(tbl->packets),
          (tbl_data->packets)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_egr_counters_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  pcp_sts_egr_counters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE];
  PCP_TBLS
    *tables = NULL;
  PCP_STS_EGR_COUNTERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTERS_TBL_SET_UNSAFE);

  PCP_CLEAR(data, uint32, PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE);

  res = pcp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->sts.egr_counters_tbl);

  offset =  tbl->addr.base + entry_offset;

  PCP_TBL_SIZE_CHECK;

  res = pcp_field_in_place_set(
          (tbl_data->packets),
          &(tbl->packets),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          PCP_STS_ID,
          PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_egr_counters_tbl_set_unsafe()", entry_offset, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_tbl_access_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_tbl_access;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_tbl_access_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_tbl_access;
}


/* } */

                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
