/* $Id: chip_sim_em.c,v 1.9 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SIMEM

#ifdef _MSC_VER
/*
 * In Microsoft:
 * cancel the warning of internal int to unsigned
 * The S/GET_FLD_IN_PLACE cause it
 */
#pragma warning(disable:4308)
#endif /* _MSC_VER */


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
/*#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>*/
/*#include <soc/dpp/SAND/Utils/sand_bitstream.h>*/
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#if 0
#include <soc/dpp/ARAD/arad_sim_mem.h>
#include <soc/dpp/ARAD/arad_sim_log.h>
#include <soc/dpp/ARAD/arad_sim.h>
#endif

#include <string.h>
#define CHIP_SIM_EM_BYTE_FILLER  0x0000

/* This should be replaced by the standard structure. */
CHIP_SIM_EM_BLOCK
Soc_pb_em_blocks[SOC_MAX_NUM_DEVICES][SOC_PB_EM_TABLE_ID_LAST + 1];
uint32
  chip_sim_exact_match_entry_add_unsafe(
          SOC_SAND_IN  int         unit,
          SOC_SAND_IN  uint32      offset, /* ISEM,LEM,ESEM,SVEM */
          SOC_SAND_IN  uint32      *key_ptr,
          SOC_SAND_IN  uint32     key_size,
          SOC_SAND_IN  uint32      *data_ptr,
          SOC_SAND_IN  uint32     data_size,
          SOC_SAND_OUT uint8     *success
            )
{

  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE] = {0},
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE] = {0};
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    key_id, id, ref_count=0;
  uint8
    first_appear,
    is_success;
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *success = TRUE;
  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_member_add(
    unit,
    multi_set,
    key8,
    &key_id,
    &first_appear,
    &is_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  
  if (!is_success)
  {
    *success = FALSE;
    status = ERROR;
    ARAD_DO_NOTHING_AND_EXIT;
  }

  /* Get the insertion index */
  if (first_appear &&  (key_id == SOC_SAND_HASH_TABLE_NULL))  {
      res = soc_sand_multi_set_member_lookup(
        unit,
        multi_set,
        key8,
        &key_id,
        &ref_count
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  }
  res = soc_sand_U32_to_U8(
    data_ptr,
    data_size,
    data8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.set(
            unit,
            id,
            key_id*data_nof_bytes + data_idx,
            data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_exact_match_entry_add_unsafe()",0,0);
}

uint32
chip_sim_exact_match_entry_get_unsafe(
        SOC_SAND_IN  int         unit,
        SOC_SAND_IN  uint32      offset,
        SOC_SAND_IN  uint32      *key_ptr,
        SOC_SAND_IN  uint32     key_size,
        SOC_SAND_OUT  uint32     *data_ptr,
        SOC_SAND_IN  uint32     data_size,
        SOC_SAND_OUT  uint8    *found
                                  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
   id, indx, ref_count;
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;
  sal_memset(data8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));
  sal_memset(key8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));

  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_member_lookup(
    unit,
    multi_set,
    key8,
    &indx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (indx == SOC_SAND_MULTI_SET_NULL)
  {
    *found = FALSE;
    ARAD_DO_NOTHING_AND_EXIT;
  }
  *found = TRUE;

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
            unit,
            id,
            indx*data_nof_bytes + data_idx,
            &data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  res = soc_sand_U8_to_U32(
                data8,
                data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_exact_match_entry_get_unsafe()",0,0);
}

uint32
soc_sand_exact_match_entry_get_by_index_unsafe(
  SOC_SAND_IN  int         unit,
  SOC_SAND_IN  uint32      tbl_offset,
  SOC_SAND_IN  uint32      entry_offset,
  SOC_SAND_OUT  uint32      *key_ptr,
  SOC_SAND_IN  uint32     key_size,
  SOC_SAND_OUT  uint32     *data_ptr,
  SOC_SAND_IN  uint32     data_size,
  SOC_SAND_OUT  uint8    *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
   id, ref_count;
 
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;

  status = chip_sim_em_offset_to_table_id (unit, tbl_offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_get_by_index(
    unit,
    multi_set,
    entry_offset,
    key8,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count == 0)
  {
    *found = FALSE;
    ARAD_DO_NOTHING_AND_EXIT;
  }
  *found = TRUE;

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
            unit,
            id,
            entry_offset*data_nof_bytes + data_idx,
            &data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = soc_sand_U8_to_U32(
                data8,
                data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_sand_U8_to_U32(
                key8,
                key_size,
                key_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_exact_match_entry_get_by_index_unsafe()",0,0);
}


uint32
chip_sim_exact_match_entry_remove_unsafe(
            SOC_SAND_IN  int         unit,
            SOC_SAND_IN  uint32      offset,
            SOC_SAND_IN  uint32      *key_ptr,
            SOC_SAND_IN  uint32     key_size
                                        )
{
  uint32
    id, data_indx;
  uint8
    is_last;
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;

  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_multi_set_member_remove(
                          unit,
                          multi_set,
                          key8,
                          &data_indx,
                          &is_last);

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_exact_match_entry_remove_unsafe()", 0, 0);
}

/*****************************************************
*NAME
*  chip_sim_em_init_chip_specifics_arad
*Programmer name  : Hana Gutman
*TYPE: PROC
*DATE: 15-Dec-09 19:02:39
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*  EM:
*    (1) CHIP_SIM_INDIRECT_BLOCK* indirect_blocks
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
STATIC int
chip_sim_em_init_chip_specifics_arad(
                    int unit
                    )
{
    soc_error_t rv;
    uint8 is_allocated;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(is_allocated) {
        SOC_EXIT;
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.alloc(unit, ARAD_EM_TABLE_ID_LAST + 1);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ID_LAST, INVALID_ADDRESS);
    SOCDNX_IF_ERR_EXIT(rv);


    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_LEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_LEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ISEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ISEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ISEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ISEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ESEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ESEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_RMAPEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_RMAPEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_OAMEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_OAMEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_OAMEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_OAMEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_GLEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_GLEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
/*****************************************************
*NAME
*  chip_sim_em_malloc_pb
*Programmer name  : Hana Gutman
*TYPE: PROC
*DATE: 24-Sep-02 19:02:44
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
chip_sim_em_malloc_pb(
             uint32 device_id, 
             CHIP_SIM_EM_BLOCK* em_blocks, 
             int                block_cnt
             )
{
  uint32
    status = OK;
  UINT32
    nof_addresses =0;
  int
    blk_ndx;
  CHIP_SIM_EM_BLOCK*
    block_p;
  CHIP_SIM_EM_TABLE_TYPE
    table_type = SOC_PB_EM_TABLE_ISEM;
  uint8
    is_first_loop = TRUE;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  for (blk_ndx = 0; blk_ndx < block_cnt; blk_ndx++) {
      block_p = &(em_blocks[blk_ndx]);

      if (is_first_loop == TRUE) 
      {
        is_first_loop = FALSE;
        table_type = SOC_PB_EM_TABLE_ISEM;
      } 
      else 
      {
        table_type++;
      }

    nof_addresses = block_p->end_address - block_p->start_address + 1;
      block_p->base =
        (uint8*)sal_alloc(nof_addresses*block_p->data_nof_bytes, "EM buffer");
      if (NULL == block_p->base)
      {
        /* chip_sim_log_run("failed to alloc in chip_sim_em_malloc()\r\n");*/
        status = ERROR ;
        /*chip_sim_mem_free();*/
        ARAD_DO_NOTHING_AND_EXIT;
      }
      /* multi_set_info = &(block_p->multi_set); */
      soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
      multi_set_init_info.get_entry_fun = NULL;
      multi_set_init_info.set_entry_fun = NULL;
      multi_set_init_info.max_duplications = 1;/*maximal packets to store from each content*/
      multi_set_init_info.sec_handle = 0;
      multi_set_init_info.prime_handle = device_id;
      multi_set_init_info.member_size = block_p->key_size; /* Soc_pb_em_blocks[table_type] */
      multi_set_init_info.nof_members = block_p->end_address - block_p->start_address + 1; /*maximal packets to store*/

      /* need to add return value check (currently not checked by caller)!! */
      soc_sand_multi_set_create(
        device_id,
        &multi_set_info,
        multi_set_init_info
      );
      block_p->multi_set = multi_set_info ;
      {
        /*
        * Initialize the the indirect allocated memory with
        * CHIP_SIM_INDIRECT_LONG_FILLER+.... to be marked.
        * Debuging porpuses.
        */
    #if 0
        UINT32
          byte_i;
    #endif
        sal_memset(
          block_p->base,
          0x0,
          nof_addresses*block_p->data_nof_bytes
          );
    #if 0
        for (byte_i=0; byte_i<nof_addresses*block_p->data_nof_bytes; byte_i++)
        {
          (block_p->base[i])[byte_i] = filler_i;
          if (CHIP_SIM_EM_BYTE_FILLER != 0x0)
          {
            /* if filler not zero incremant it, so every place as different value*/
            filler_i++;
            if (filler_i==0x0)
            {
              filler_i = CHIP_SIM_EM_BYTE_FILLER;
            }
          }
        }
    #endif
      }
  }


exit:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_em_malloc_arad
*Programmer name  : Hana Gutman
*TYPE: PROC
*DATE: 24-Sep-02 19:02:44
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
STATIC int
chip_sim_em_malloc_arad(
             int unit,
             int                block_cnt
             )
{
  soc_error_t rv;
  UINT32
    nof_addresses =0;
  int
    blk_ndx;
  CHIP_SIM_EM_TABLE_TYPE
    table_type = SOC_PB_EM_TABLE_ISEM;
  uint8
    is_first_loop = TRUE;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;
  UINT32 end_address;
  UINT32 start_address;
  UINT32 data_nof_bytes;
  uint8 is_allocated;

  SOCDNX_INIT_FUNC_DEFS;

  /* Check whether the sw state data is already allocated.
   * We assume that if the first one is already allocated then all of the blocks are allocated.
   */
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.is_allocated(unit, 0, &is_allocated);
  SOCDNX_IF_ERR_EXIT(rv);
  if(is_allocated) {
        SOC_EXIT;
  }

  for (blk_ndx = 0; blk_ndx < block_cnt; blk_ndx++) {
      if (is_first_loop == TRUE) 
      {
        is_first_loop = FALSE;
        table_type = SOC_PB_EM_TABLE_ISEM;
      } 
      else 
      {
        table_type++;
      }

      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.get(unit, blk_ndx, &end_address);
      SOCDNX_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.get(unit, blk_ndx, &start_address);
      SOCDNX_IF_ERR_EXIT(rv);
      nof_addresses = end_address - start_address + 1;
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, blk_ndx, &data_nof_bytes);
      SOCDNX_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.alloc(unit, blk_ndx, nof_addresses * data_nof_bytes);
      SOCDNX_IF_ERR_EXIT(rv);
      soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
      multi_set_init_info.get_entry_fun = NULL;
      multi_set_init_info.set_entry_fun = NULL;
      multi_set_init_info.max_duplications = 1;/*maximal packets to store from each content*/
      multi_set_init_info.sec_handle = 0;
      multi_set_init_info.prime_handle = unit;
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.get(unit, blk_ndx, &multi_set_init_info.member_size);
      SOCDNX_IF_ERR_EXIT(rv);
      multi_set_init_info.nof_members = end_address - start_address + 1; /*maximal packets to store*/


      /* need to add return value check (currently not checked by caller)!! */
      rv = soc_sand_multi_set_create(
        unit,
        &multi_set_info,
        multi_set_init_info
      );
      SOCDNX_SAND_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.set(unit, blk_ndx, multi_set_info);
      SOCDNX_IF_ERR_EXIT(rv);

  }


exit:
    SOCDNX_FUNC_RETURN;
}

/*****************************************************
*NAME
*  chip_sim_indirect_free
*Programmer name  :Hana Gutman
*TYPE: PROC
*DATE: 15-Dec-09 19:02:50
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    None.

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
chip_sim_em_init(int unit, SOC_SAND_DEVICE_TYPE device_type)
{
  int                block_cnt;
  soc_error_t rv;


  SOC_SAND_INTERRUPT_INIT_DEFS;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_INTERRUPTS_STOP; 

  block_cnt = 0;

  switch(device_type)
  {
  case SOC_SAND_DEV_ARAD:
    block_cnt = ARAD_EM_TABLE_ID_LAST;
    if (!SOC_WARM_BOOT(unit)) {
		    rv = chip_sim_em_init_chip_specifics_arad(unit);
		    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
		    rv = chip_sim_em_malloc_arad(unit, block_cnt);
    		SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
	  }    
    break;
  default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);     
      break;
  }


exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_init()",0,0);
  

}


uint32 
chip_sim_em_offset_to_table_id (SOC_SAND_IN   uint32      unit,
                                SOC_SAND_IN   uint32      offset,
                                SOC_SAND_OUT  uint32      *id)
{
  uint32
    block_idx;
  UINT32
    read_result_address;
  UINT32
    block_offset;
  soc_error_t rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (block_idx= 0; block_idx < ARAD_EM_TABLE_ID_LAST + 1; ++block_idx)
  {
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.get(unit, block_idx, &read_result_address);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    if(read_result_address == INVALID_ADDRESS) {
      break;
    }
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.get(unit, block_idx, &block_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    if (block_offset == offset)
    {
      *id = block_idx;
      ARAD_DO_NOTHING_AND_EXIT;
    }
  }
  *id = -1;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_offset_to_table_id()",0,0);
}

uint32
chip_sim_em_get_block(
  SOC_SAND_IN       int         unit,
  SOC_SAND_IN       uint32      tbl_offset,
  SOC_SAND_IN       uint32     key_size,
  SOC_SAND_IN       uint32     data_size,
  SOC_SAND_IN       void*      *filter_data,
  SOC_SAND_INOUT    uint32     *key_array,
  SOC_SAND_INOUT    uint32     *data_array,
  SOC_SAND_OUT      uint32     *num_entries,
  SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE  *block_range /* Get information about the traverse start index and number of allowed results. Return the new position */
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  uint8  
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set ;
  uint32    id, cnt;
  SOC_SAND_MULTI_SET_ITER   iter;
  uint32                     hash_data_ndx, ref_count;
  uint32
    status = OK ;
  uint32 tmp_key;
  SOC_SAND_IN ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA                      *tbl_data;
  uint32    eq, is_diff, max_entries_to_return;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tbl_data = (ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *)filter_data;
  cnt = 0;
  status = chip_sim_em_offset_to_table_id (unit, tbl_offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if (block_range == NULL || SOC_SAND_TBL_ITER_IS_BEGIN(&(block_range->iter))) /* Check if the traverse is in the middle */
  {
      /* Start a new traverse session */
      SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter); 
  }
  else
  {
      /* Update the iter about the last possition we were at */
      iter = (SOC_SAND_MULTI_SET_ITER)(block_range->iter);
  }
  if (block_range != NULL)
  {
      max_entries_to_return = block_range->entries_to_act;
  }
  else
  {
      /* Fetch all the entries in one block */
      max_entries_to_return = SOC_SAND_U32_MAX - 1;
  }

  res = soc_sand_multi_set_get_next(unit, multi_set, &iter, key8, &hash_data_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter) && cnt < max_entries_to_return) {

      res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
                unit,
                id,
                hash_data_ndx*data_nof_bytes + data_idx,
                &data8[data_idx]);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }


      /* Check if the payload is equal */
      res = soc_sand_U8_to_U32(
                    data8,
                    data_nof_bytes,
                    &data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
     
      eq = TRUE;
      is_diff = 0;

      if (tbl_data != NULL) {
          is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S] ^ COMPILER_64_LO(tbl_data->compare_payload_data));
          is_diff = is_diff & COMPILER_64_LO(tbl_data->compare_payload_mask);
          if (is_diff != 0) {
              eq = FALSE;
          } else {
              is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S+1] ^ COMPILER_64_HI(tbl_data->compare_payload_data));
              is_diff = is_diff & COMPILER_64_HI(tbl_data->compare_payload_mask);
              if (is_diff != 0) {
                  eq = FALSE;
              }
          }
      }


      /* Check if the key is equal */
      res = soc_sand_U8_to_U32(
                    key8,
                    key_size,
                    &key_array[cnt * ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S_LEM]);
                    /* &key_array[cnt * ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S_MAX]); */
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /* Since the key is not aligned to the entries from the SW DB, tmp_key is used to align them */
      tmp_key = (uint32)(key8[6]) | (uint32)(key8[7]<<8) | (uint32)(key8[8]<<16) | (uint32)(key8[9]<<24);
     
      is_diff = 0;

      /* is_diff =( key_array[cnt * ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S_MAX] ^ COMPILER_64_LO(tbl_data->compare_key_20_data)); */
      if (tbl_data != NULL) {
          is_diff =( tmp_key ^ (tbl_data->compare_key_20_data));
          is_diff = is_diff & (tbl_data->compare_key_20_mask);
          if (is_diff != 0) {
              eq = FALSE;
          } /* else {
              is_diff =( key_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S+1] ^ COMPILER_64_HI(tbl_data->compare_key_20_data));
              is_diff = is_diff & COMPILER_64_HI(tbl_data->compare_key_20_mask);
              if (is_diff != 0) {
                  eq = FALSE;
              }
          }*/
      }
      if (!eq) {
          goto next_loop;
      }

      cnt++;
next_loop:
      res = soc_sand_multi_set_get_next(unit, multi_set, &iter, key8, &hash_data_ndx, &ref_count);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }
  *num_entries = cnt;

  /* Update the block_range about the current state of the traverse */
  if (block_range != NULL) 
  {
      if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) 
      {
          SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
      }
      else
      {
          /* The traverse is in the middle */
          block_range->iter = (uint32)iter;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LEM_ACCESS_PAYLOAD_verify()",0,0);
  
}

uint32
  chip_sim_em_match_rule_mac(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *match) 
{
    uint8   eq;
    int     i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *match = FALSE;

    if (mac_key->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        ARAD_DO_NOTHING_AND_EXIT;
    }
    if (rule->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /* Match mac and mask*/
    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++) {
         eq = ((mac_key->key_val.mac.mac.address[i] ^ rule->key_rule.mac.mac.address[i]) &  rule->key_rule.mac.mac_mask.address[i]);
         if (eq != 0 ) {
             ARAD_DO_NOTHING_AND_EXIT;
         }
    }
    eq = (rule->key_rule.mac.fid ^  mac_key->key_val.mac.fid) & rule->key_rule.mac.fid_mask;
    if (eq != 0 ) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    if (rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) {
        if (rule->value_rule.val.aging_info.is_dynamic != mac_entry_value->aging_info.is_dynamic) {
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }
    if ((rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) 
        && (rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE))
    {
        if (rule->value_rule.val.frwrd_info.forward_decision.dest_id != mac_entry_value->frwrd_info.forward_decision.dest_id) {
            ARAD_DO_NOTHING_AND_EXIT;
        }
        if (rule->value_rule.val.frwrd_info.forward_decision.type != mac_entry_value->frwrd_info.forward_decision.type) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

    }
    *match = TRUE;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_match_rule_mac()",0,0);
  
}

