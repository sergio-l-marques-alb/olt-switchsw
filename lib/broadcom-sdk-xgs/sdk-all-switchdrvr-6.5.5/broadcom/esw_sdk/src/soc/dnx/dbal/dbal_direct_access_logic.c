/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX
#include <shared/bsl.h>

#include "dbal.h"
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>

#define DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE         16      /*TBD verify this size */

#define DBAL_MEM_READ_LOG( memory, mem_offset, block, mem_array_offset)                                            \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Read preformed from memory %s, entry offset = %d block %d array_offset %d\n",      \
                    SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);

#define DBAL_MEM_WRITE_LOG( memory, mem_offset, block, mem_array_offset)                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Write preformed to memory %s, entry offset = %d block %d array_offset %d\n",      \
                    SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);

#define DBAL_REG_READ_LOG( reg, reg_offset, block, reg_array_offset)                                            \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Read preformed from register %s, entry offset = %d block %d array offset %d\n",      \
                    SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);

#define DBAL_REG_WRITE_LOG( reg, reg_offset, block, reg_array_offset)                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Write preformed to register %s, entry offset = %d block %d array_offset %d\n",      \
                    SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);

shr_error_e
dbal_direct_condition_check(
  int unit,
  dbal_condition_types_e condition,
  uint32 index,
  uint32 input_param,
  uint8 * is_passed)
{
  SHR_FUNC_INIT_VARS(unit);

  (*is_passed) = 0;

  switch (condition)
  {
    case DBAL_CONDITION_BIGGER_THAN:
      if (index > input_param)
      {
        (*is_passed) = 1;
      }
      break;

    case DBAL_CONDITION_LOWER_THAN:
      if (index < input_param)
      {
        (*is_passed) = 1;
      }
      break;

    case DBAL_CONDITION_IS_EQUAL_TO:
      if (index == input_param)
      {
        (*is_passed) = 1;
      }
      break;

    case DBAL_CONDITION_NONE:
      (*is_passed) = 1;
      break;

    case DBAL_CONDITION_IS_EVEN:
      if (index%2 == 0)
      {
       (*is_passed) = 1;
      }      
      break;

    case DBAL_CONDITION_IS_ODD:
      if (index%2 == 1)
      {
       (*is_passed) = 1;
      }      
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "ilegal condition value %d\n", condition);
      break;
  }

exit:
  SHR_FUNC_EXIT;

}

shr_error_e
dbal_direct_offset_calculate(
  int unit,
  dbal_entry_handle_t * entry_handle,
  uint32 index,
  dbal_value_encode_info_t * encode_info,
  uint32 * calc_index)
{
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  (*calc_index) = 0;

  /*
   * in this case we use specific field to be the key of the entry 
   */
  if (encode_info->field_id != DBAL_FIELD_EMPTY)
  {

    uint32 field_val = 0, is_found = 0;
    SHR_IF_ERR_EXIT(dbal_specific_field_from_buffer_get
                    (unit, entry_handle, encode_info->field_id, &field_val, &is_found));
    if (is_found)
    {
      index = field_val;
    }
    else
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "ilegal field %s not exists in table %s\n",
                   dbal_field_to_string(unit, encode_info->field_id), table->table_name);
    }
  }

  switch (encode_info->encode_mode)
  {
    case DBAL_VALUE_ENCODE_NONE:
    case DBAL_VALUE_ENCODE_PARTIAL_KEY:
      (*calc_index) = index;
      break;

    case DBAL_VALUE_ENCODE_MODULO:
      (*calc_index) = index % encode_info->input_param;
      break;

    case DBAL_VALUE_ENCODE_DIVIDE:
      (*calc_index) = index / encode_info->input_param;
      break;

    case DBAL_VALUE_ENCODE_MULTIPLE:
      (*calc_index) = index * encode_info->input_param;
      break;

    case DBAL_VALUE_ENCODE_BOOL:
      if (index != 0)
      {
        (*calc_index) = 1;
      }
      break;

    case DBAL_VALUE_ENCODE_SUBTRACT:
      (*calc_index) = index - encode_info->input_param;
      break;

    case DBAL_VALUE_ENCODE_HARD_VALUE:
    (*calc_index) = encode_info->input_param;
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "ilegal encode mode %d\n", encode_info->encode_mode);
      break;
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_direct_memory_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int access_number)
{
  int iter;
  soc_mem_t last_memory_used = 0;
  int last_mem_entry_offset = 0;
  int last_mem_array_offset_used = 0;
  uint32 mem_entry_offset = 0, field_offset = 0;
  uint8 is_conditaion_pass = 0;
  uint32 data[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE] = { 0 };
  uint32 key = entry_handle->phy_entry.key[0];
  uint32 orig_field_offset = 0;
  uint32 core_id;
  dbal_logical_table_t *table = entry_handle->table;
  dbal_direct_l2p_info_t *l2p_direct_info = &(table->l2p_direct_info[access_number]);
  int block = MEM_BLOCK_ANY;

  SHR_FUNC_INIT_VARS(unit);

  core_id = DBAL_CORE_ANY;
  if (table->core_mode == DBAL_CORE_BY_INPUT)
  {
    if (entry_handle->core_id != DBAL_CORE_NOT_INTIATED)
    {
      core_id = entry_handle->core_id;
    }
  }

  for (iter = 0; iter < l2p_direct_info->num_of_access_fields; iter++)
  {
    SHR_IF_ERR_EXIT(dbal_direct_condition_check
                    (unit, l2p_direct_info->l2p_fields_info[iter].condition_mode, key,
                     l2p_direct_info->l2p_fields_info[iter].condition_input_param, &is_conditaion_pass));
    if (is_conditaion_pass)
    {
      orig_field_offset = key;
      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_field_offset = 0;
      }

      /*
       * mem entry offset calculate
       */
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, key, &(l2p_direct_info->l2p_fields_info[iter].memory_offset_info),
                       &mem_entry_offset));

      /*
       * field offset calculate
       */
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_field_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].field_offset_info), &field_offset));
      /*
       * TBD validate that indexes recieved are legal 
       */

      if (last_memory_used == 0)
      {
        last_memory_used = l2p_direct_info->l2p_fields_info[iter].memory;
        last_mem_array_offset_used = l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param;
        last_mem_entry_offset = mem_entry_offset;

        if (core_id != DBAL_CORE_ANY)
        {
          block = SOC_MEM_BLOCK_MIN(unit, last_memory_used) + core_id;
        }
        else
        {
          block = MEM_BLOCK_ANY;
        }
        sal_memset(data, 0x0, DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4);

        soc_mem_array_read(unit, last_memory_used, last_mem_array_offset_used, block, mem_entry_offset, data);

        DBAL_MEM_READ_LOG(last_memory_used, mem_entry_offset, block, last_mem_array_offset_used);
      }
      else
      {
        if (last_memory_used != l2p_direct_info->l2p_fields_info[iter].memory
            || (last_mem_entry_offset != mem_entry_offset)
            || (last_mem_array_offset_used != l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param))
        {
          int block = MEM_BLOCK_ANY;
          last_memory_used = l2p_direct_info->l2p_fields_info[iter].memory;
          last_mem_entry_offset = mem_entry_offset;
          last_mem_array_offset_used = l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param;
          sal_memset(data, 0x0, DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4);
          block = MEM_BLOCK_ANY;

          if (core_id != DBAL_CORE_ANY)
          {
            block = SOC_MEM_BLOCK_MIN(unit, last_memory_used) + core_id;
          }
          soc_mem_array_read(unit, last_memory_used, last_mem_array_offset_used, block, mem_entry_offset, data);

          DBAL_MEM_READ_LOG(last_memory_used, mem_entry_offset, block, last_mem_array_offset_used);
        }
      }

      {
        uint32 field_val[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE];
        sal_memset(field_val, 0x0, DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4);

        soc_mem_field_get(unit, l2p_direct_info->l2p_fields_info[iter].memory, data,
                          l2p_direct_info->l2p_fields_info[iter].hw_field, field_val);

        LOG_INFO_EX(BSL_LOG_MODULE, "read field %s from memory, value received= 0x%x %s%s\n",
                    SOC_FIELD_NAME(unit, l2p_direct_info->l2p_fields_info[iter].hw_field), field_val[0], EMPTY, EMPTY);

        /*
         * this means that the field has offset, need to handle it
         */
        if (field_offset != 0)
        {
          uint32 tmp_field_val[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE];
          sal_memset(tmp_field_val, 0x0, DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4);
          SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                          (field_val, field_offset,
                           table->fields_info[l2p_direct_info->l2p_fields_info[iter].field_pos_in_table].field_nof_bits,
                           tmp_field_val));

          LOG_INFO_EX(BSL_LOG_MODULE, "Taking part of the field orig val %x, offset =%d, received val = %x%s\n",
                      tmp_field_val[0],
                      table->fields_info[l2p_direct_info->l2p_fields_info[iter].field_pos_in_table].
                      bits_offset_in_buffer, tmp_field_val[0], EMPTY);
          sal_memcpy(field_val, tmp_field_val,
                     DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4 );
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (field_val, l2p_direct_info->l2p_fields_info[iter].offset,
                         l2p_direct_info->l2p_fields_info[iter].nof_bits, entry_handle->phy_entry.payload));
      }
    }
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_direct_memory_set(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int access_number)
{
  int iter;
  soc_mem_t last_memory_used = 0;
  int last_mem_offset_used = 0;
  int last_mem_array_offset_used = 0;
  uint32 mem_offset = 0, field_offset = 0;
  uint8 is_conditaion_pass = 0;
  uint32 field_data[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE] = { 0 };
  uint32 data[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE] = { 0 };
  uint32 key = entry_handle->phy_entry.key[0];
  uint32 orig_field_offset = 0;
  uint32 core_id;
  dbal_logical_table_t *table = entry_handle->table;
  dbal_direct_l2p_info_t *l2p_direct_info = &(table->l2p_direct_info[access_number]);
  int block = MEM_BLOCK_ANY;

  SHR_FUNC_INIT_VARS(unit);

  core_id = DBAL_CORE_ANY;
  if (table->core_mode == DBAL_CORE_BY_INPUT)
  {
    if (entry_handle->core_id != DBAL_CORE_NOT_INTIATED)
    {
      core_id = entry_handle->core_id;
    }
  }

  /*
   * TBD optimization: if all fields in memory was set preform write without read 
   */
  for (iter = 0; iter < l2p_direct_info->num_of_access_fields; iter++)
  {
    sal_memset(field_data, 0x0, DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4);
    SHR_IF_ERR_EXIT(dbal_direct_condition_check
                    (unit, l2p_direct_info->l2p_fields_info[iter].condition_mode, key,
                     l2p_direct_info->l2p_fields_info[iter].condition_input_param, &is_conditaion_pass));

    if (is_conditaion_pass)
    {
      orig_field_offset = key;
      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_field_offset = 0;
      }

      /*
       * calculate the memory entry offset 
       */
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, key, &(l2p_direct_info->l2p_fields_info[iter].memory_offset_info),
                       &mem_offset));

      /*
       * calculate the memory field offset 
       */
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_field_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].field_offset_info), &field_offset));
      /*
       * if the current memory is the same memory has the last no need to write yet
       */

      if ((last_memory_used != l2p_direct_info->l2p_fields_info[iter].memory) || (last_mem_offset_used != mem_offset)
          || (last_mem_array_offset_used != l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param))
      {
        /*
         * not the first memory, need to preform mem write to the prev memory 
         */
        if (last_memory_used != 0)
        {
          block = MEM_BLOCK_ANY;
          if (core_id != DBAL_CORE_ANY)
          {
            block = SOC_MEM_BLOCK_MIN(unit, last_memory_used) + core_id;
          }

          soc_mem_array_write(unit, last_memory_used, last_mem_array_offset_used, block, last_mem_offset_used, data);

          DBAL_MEM_WRITE_LOG(last_memory_used, mem_offset, block, last_mem_array_offset_used);
        }

        sal_memset(data, 0x0, 4*DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE);
        block = MEM_BLOCK_ANY;
        if (core_id != DBAL_CORE_ANY)
        {
          block = SOC_MEM_BLOCK_MIN(unit, l2p_direct_info->l2p_fields_info[iter].memory) + core_id;
        }

        soc_mem_array_read(unit, l2p_direct_info->l2p_fields_info[iter].memory,
                           l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param, block, mem_offset,
                           data);
        /*
         * updating the parameters of the current memory 
         */
        last_memory_used = l2p_direct_info->l2p_fields_info[iter].memory;
        last_mem_offset_used = mem_offset;
        last_mem_array_offset_used = l2p_direct_info->l2p_fields_info[iter].mem_array_offset_info.input_param;

        DBAL_MEM_READ_LOG(last_memory_used, mem_offset, block, last_mem_array_offset_used);
      }

      /*
       * this part takes the field value from the buffer, taking only the reqiured bits 
       */
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                      (entry_handle->phy_entry.payload, l2p_direct_info->l2p_fields_info[iter].offset,
                       l2p_direct_info->l2p_fields_info[iter].nof_bits, field_data));

      /*
       * handling offset in field when field offset exists 
       */
      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode != DBAL_VALUE_ENCODE_NONE)
      {
        uint32 tmp_field_data[DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE] = { 0 };

        soc_mem_field_get(unit, l2p_direct_info->l2p_fields_info[iter].memory, data,
                          l2p_direct_info->l2p_fields_info[iter].hw_field, tmp_field_data);

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (field_data, field_offset,
                         table->fields_info[l2p_direct_info->l2p_fields_info[iter].field_pos_in_table].field_nof_bits,
                         tmp_field_data));

        sal_memcpy(field_data, tmp_field_data,
                   DBAL_DIRECT_ACCESS_MAX_MEMORY_LINE_SIZE*4 );
      }

      soc_mem_field_set(unit, l2p_direct_info->l2p_fields_info[iter].memory, data,
                        l2p_direct_info->l2p_fields_info[iter].hw_field, field_data);

      LOG_INFO_EX(BSL_LOG_MODULE, "setting field %-20s, field offset %d, value 0x%x (%u) \n",
                  SOC_FIELD_NAME(unit, l2p_direct_info->l2p_fields_info[iter].hw_field), field_offset, field_data[0],
                  field_data[0]);
    }
  }

  if (last_memory_used != 0)
  {
    block = MEM_BLOCK_ANY;
    if (core_id != DBAL_CORE_ANY)
    {
      block = SOC_MEM_BLOCK_MIN(unit, last_memory_used) + core_id;
    }

    soc_mem_array_write(unit, last_memory_used, last_mem_array_offset_used, block, last_mem_offset_used, data);
    DBAL_MEM_WRITE_LOG(last_memory_used, mem_offset, block, last_mem_array_offset_used);
  }

exit:
  SHR_FUNC_EXIT;

}

shr_error_e
dbal_direct_register_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int access_number)
{
  int iter;
  soc_reg_t last_register_used = 0;
  int last_reg_offset_used = 0;
  uint32 reg_offset = 0, field_offset = 0;
  uint8 is_conditaion_pass = 0;
  uint32 data = 0;
  uint32 key = entry_handle->phy_entry.key[0];
  uint32 orig_field_offset = 0, orig_reg_offset = 0;
  uint32 core_id;
  int block = REG_PORT_ANY;
  dbal_logical_table_t *table = entry_handle->table;
  dbal_direct_l2p_info_t *l2p_direct_info = &table->l2p_direct_info[access_number];
  /*
   * TBD if encoding type is bool number of bits should be 1 
   */

  SHR_FUNC_INIT_VARS(unit);

  core_id = DBAL_CORE_ANY;
  if (table->core_mode == DBAL_CORE_BY_INPUT)
  {
    if (entry_handle->core_id != DBAL_CORE_NOT_INTIATED)
    {
      core_id = entry_handle->core_id;
    }
  }

  if (core_id != DBAL_CORE_ANY)
  {
    block = core_id;
  }

  for (iter = 0; iter < l2p_direct_info->num_of_access_fields; iter++)
  {
    SHR_IF_ERR_EXIT(dbal_direct_condition_check
                    (unit, l2p_direct_info->l2p_fields_info[iter].condition_mode, key,
                     l2p_direct_info->l2p_fields_info[iter].condition_input_param, &is_conditaion_pass));
    if (is_conditaion_pass)
    {
      orig_reg_offset = key;
      orig_field_offset = key;
      if (l2p_direct_info->l2p_fields_info[iter].memory_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_reg_offset = 0;
      }
      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_field_offset = 0;
      }

      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_reg_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].memory_offset_info), &reg_offset));
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_field_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].field_offset_info), &field_offset));
      /*
       * TBD validate that indexes recieved are legal 
       */

      if (last_register_used == 0 || (last_reg_offset_used != reg_offset))
      {
        last_register_used = l2p_direct_info->l2p_fields_info[iter].reg[reg_offset];
        data = 0;
        soc_reg32_get(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], core_id, 0, &data);
        DBAL_REG_READ_LOG(last_register_used, reg_offset, block, 0);
      }
      else
      {
        if (last_register_used != l2p_direct_info->l2p_fields_info[iter].reg[reg_offset])
        {
          last_register_used = l2p_direct_info->l2p_fields_info[iter].reg[reg_offset];
          last_reg_offset_used = reg_offset;
          data = 0;
          soc_reg32_get(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], core_id, 0, &data);
          DBAL_REG_READ_LOG(last_register_used, reg_offset, block, 0);
        }
      }
      {
        uint32 field_val = 0;

        field_val =
          soc_reg_field_get(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], data,
                            l2p_direct_info->l2p_fields_info[iter].hw_field);
        LOG_INFO_EX(BSL_LOG_MODULE, "read field %s from register, value received= 0x%x %s%s\n",
                    SOC_FIELD_NAME(unit, l2p_direct_info->l2p_fields_info[iter].hw_field), field_val, EMPTY, EMPTY);
        if (field_offset != 0)
        {
          uint32 tmp_field_val = 0;
          SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                          (&field_val, field_offset,
                           (uint32) (table->fields_info[l2p_direct_info->l2p_fields_info[iter].field_pos_in_table].
                                     field_nof_bits), &tmp_field_val));
          field_val = tmp_field_val;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&field_val, (uint32) (l2p_direct_info->l2p_fields_info[iter].offset),
                         (uint32) (l2p_direct_info->l2p_fields_info[iter].nof_bits),
                         (uint32 *) (entry_handle->phy_entry.payload)));

      }
      /*
       * TBD support reg index 2 possiblities (1) when array of registers (2) register index 
       */
      /*
       * TBD support offset in field for bitmap operations and multiple fields with same name 
       */
    }
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_direct_register_set(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int access_number)
{
  int iter;
  soc_reg_t last_register_used = 0;
  int last_reg_offset_used = 0;
  uint32 reg_offset = 0, field_offset = 0;
  uint8 is_conditaion_pass = 0;
  uint32 field_data = 0;
  uint32 data = 0;
  uint32 key = entry_handle->phy_entry.key[0];
  uint32 orig_field_offset = 0, orig_reg_offset = 0;
  uint32 core_id;
  int block = REG_PORT_ANY;
  dbal_logical_table_t *table = entry_handle->table;
  dbal_direct_l2p_info_t *l2p_direct_info = &table->l2p_direct_info[access_number];
  /*
   * TBD if encoding type is bool number of bits should be 1 
   */

  SHR_FUNC_INIT_VARS(unit);

  core_id = DBAL_CORE_ANY;
  if (table->core_mode == DBAL_CORE_BY_INPUT)
  {
    if (entry_handle->core_id != DBAL_CORE_NOT_INTIATED)
    {
      core_id = entry_handle->core_id;
    }
  }

  if (core_id != DBAL_CORE_ANY)
  {
    block = core_id;
  }

  for (iter = 0; iter < l2p_direct_info->num_of_access_fields; iter++)
  {
    field_data = 0;
    SHR_IF_ERR_EXIT(dbal_direct_condition_check
                    (unit, l2p_direct_info->l2p_fields_info[iter].condition_mode, key,
                     l2p_direct_info->l2p_fields_info[iter].condition_input_param, &is_conditaion_pass));
    if (is_conditaion_pass)
    {
      orig_reg_offset = key;
      orig_field_offset = key;
      if (l2p_direct_info->l2p_fields_info[iter].memory_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_reg_offset = 0;
      }
      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode == DBAL_VALUE_ENCODE_NONE)
      {
        orig_field_offset = 0;
      }

      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_reg_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].memory_offset_info), &reg_offset));
      SHR_IF_ERR_EXIT(dbal_direct_offset_calculate
                      (unit, entry_handle, orig_field_offset,
                       &(l2p_direct_info->l2p_fields_info[iter].field_offset_info), &field_offset));
      /*
       * TBD validate that indexes recieved are legal 
       */

      if (((last_register_used != l2p_direct_info->l2p_fields_info[iter].reg[reg_offset]))
          || (last_reg_offset_used != reg_offset))
      {
        if (last_register_used != 0)
        {
          soc_reg32_set(unit, last_register_used, block, last_reg_offset_used, data);
          DBAL_MEM_WRITE_LOG(last_register_used, last_reg_offset_used, block, 0);
        }

        data = 0;
        soc_reg32_get(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], block, 0, &data);
        DBAL_REG_READ_LOG(last_register_used, reg_offset, block, 0);
      }

      /*
       * handling partial SW field use 
       */
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                      (entry_handle->phy_entry.payload, l2p_direct_info->l2p_fields_info[iter].offset,
                       l2p_direct_info->l2p_fields_info[iter].nof_bits, &field_data));
      /*
       * TBD support reg index 2 possiblities (1) when array of registers (2) register index 
       * if register has no field we can access directly to an offset in the register. 
       */

      if (l2p_direct_info->l2p_fields_info[iter].field_offset_info.encode_mode != DBAL_VALUE_ENCODE_NONE)
      {
        uint32 field_val = 0;
        field_val =
          soc_reg_field_get(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], data,
                            l2p_direct_info->l2p_fields_info[iter].hw_field);
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&field_data, field_offset,
                         table->fields_info[l2p_direct_info->l2p_fields_info[iter].field_pos_in_table].field_nof_bits,
                         &field_val));
        field_data = field_val;
      }
      soc_reg_field_set(unit, l2p_direct_info->l2p_fields_info[iter].reg[reg_offset], &data,
                        l2p_direct_info->l2p_fields_info[iter].hw_field, field_data);
      last_register_used = l2p_direct_info->l2p_fields_info[iter].reg[reg_offset];
      last_reg_offset_used = reg_offset;
      LOG_INFO_EX(BSL_LOG_MODULE, "setting field %-20s, field offset %d, value 0x%x (%u) \n",
                  SOC_FIELD_NAME(unit, l2p_direct_info->l2p_fields_info[iter].hw_field), field_offset, field_data,
                  field_data);
    }
  }

  if (last_register_used != 0)
  {
    soc_reg32_set(unit, last_register_used, block, last_reg_offset_used, data);
    DBAL_MEM_WRITE_LOG(last_register_used, last_reg_offset_used, block, 0);
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_direct_entry_get(
  int unit,
  dbal_entry_handle_t * entry_handle)
{
  int iter;
  uint8 has_configuration = FALSE;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle->phy_entry.key_size > 32)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "key is bigger than uint32 %d missing implementation\n",
                 entry_handle->phy_entry.key_size);
  }

  for (iter = 0; iter < DBAL_NOF_DIRECT_ACCESS_TYPES; iter++)
  {
    if (table->l2p_direct_info[iter].num_of_access_fields != 0)
    {
      has_configuration = TRUE;
      switch (iter)
      {
        case DBAL_DIRECT_ACCESS_MEMORY:
          SHR_IF_ERR_EXIT(dbal_direct_memory_get(unit, entry_handle, iter));
          break;
        case DBAL_DIRECT_ACCESS_REGISTER:
          SHR_IF_ERR_EXIT(dbal_direct_register_get(unit, entry_handle, iter));
          break;
        case DBAL_DIRECT_ACCESS_PEMLA:
          continue; 
      }
    }
  }

  if (!has_configuration)
  {
    LOG_CLI((BSL_META("Missing configuration for logical 2 physical in table %s for direct access \n"),
             table->table_name));
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_direct_entry_set(
  int unit,
  dbal_entry_handle_t * entry_handle)
{
  int iter;
  uint8 has_configuration = FALSE;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle->phy_entry.key_size > 32)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "key is bigger than uint32 %d missing implementation\n",
                 entry_handle->phy_entry.key_size);
  }

  for (iter = 0; iter < DBAL_NOF_DIRECT_ACCESS_TYPES; iter++)
  {
    if (table->l2p_direct_info[iter].num_of_access_fields != 0)
    {
      has_configuration = TRUE;
      switch (iter)
      {
        case DBAL_DIRECT_ACCESS_MEMORY:
          SHR_IF_ERR_EXIT(dbal_direct_memory_set(unit, entry_handle, iter));
          break;
        case DBAL_DIRECT_ACCESS_REGISTER:
          SHR_IF_ERR_EXIT(dbal_direct_register_set(unit, entry_handle, iter));
          break;
        case DBAL_DIRECT_ACCESS_PEMLA:
          continue; 
      }
    }
  }

  if (!has_configuration)
  {
    LOG_CLI((BSL_META("Missing configuration for logical 2 physical in table %s for direct access \n"),
             table->table_name));
  }

exit:
  SHR_FUNC_EXIT;
}


shr_error_e 
dbal_direct_entry_get_next(
  int unit,
  uint32 entry_handle_id,
  dbal_iterator_info_t* iterator_info)
{
  dbal_logical_table_t *table;
  uint32 key_val;
  int iter;
  int entry_found = 0;
  int is_value_field_exists = 0;
  dbal_table_field_info_t table_field_info;
  uint32 key_left = 0;
  uint32 mask = 0;
  dbal_entry_handle_t entry_handle;

  SHR_FUNC_INIT_VARS(unit);

  /* support core_id handle according to the table type */
  /* take the handle here but execute the command from the direct access file */

  SHR_IF_ERR_EXIT(dbal_entry_handle_info_get(unit,entry_handle_id, &entry_handle));
  table = entry_handle.table;

  while ((!entry_found) && (iterator_info->entry_index < iterator_info->max_num_of_iterations))
  {
    /* stting all key fields */
    key_left = iterator_info->entry_index;
    for (iter = 0; iter < table->num_of_fields; iter++)
    {
      if(table->fields_info[iter].is_key){
        if ((table->fields_info[iter].field_id == DBAL_FIELD_CORE_ID) && (table->core_mode == DBAL_CORE_BY_INPUT))
        {
          /* if bigger tha iterator_info->max_num_of_iterations/1 use code 1 (depend on the table core mode)*/
          if (iterator_info->entry_index > (iterator_info->max_num_of_iterations/2))
          {
            SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, table->fields_info[iter].field_id, 1));
          }else
          {
            SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, table->fields_info[iter].field_id, 0));
          }
        }
        else
        {
          mask = (1<<(table->fields_info[iter].field_nof_bits)) -1;
          key_val = key_left & mask;
          key_left = key_left >> table->fields_info[iter].field_nof_bits;                        
          SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, table->fields_info[iter].field_id, key_val));
          
        }
      }
      else
      {
        is_value_field_exists = 1;
      }
    }

    if (!is_value_field_exists)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL,"no value fields exists in input");
    }

    /* getting the requested fields */
    for (iter = 0; iter < iterator_info->num_of_fields; iter++)
    {
      SHR_IF_ERR_EXIT(dbal_table_field_info_get(unit,iterator_info->table_id, iterator_info->requested_fields_info[iter].field_id, &table_field_info));

      if(table_field_info.is_key)
      {          
        if (table_field_info.field_id == DBAL_FIELD_CORE_ID)
        {
          *(iterator_info->requested_fields_info[iter].field_val) = 0;
        }
        else
        {
          *(iterator_info->requested_fields_info[iter].field_val) = iterator_info->entry_index;
        }
      }
      else
      {
        SHR_IF_ERR_EXIT(dbal_entry_array32_get(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, 
                                         iterator_info->requested_fields_info[iter].field_id, 
                                         iterator_info->requested_fields_info[iter].field_val));
      }        
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit,entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));

    /* if the entry is not equals to the default entry it is returned */
    for (iter = 0; iter < iterator_info->num_of_fields; iter++)
    {
      dbal_table_field_info_get(unit,iterator_info->table_id, iterator_info->requested_fields_info[iter].field_id, &table_field_info);
      if (!table_field_info.is_key)
      {
        if(sal_memcmp(iterator_info->requested_fields_info[iter].field_val, 0x0, table_field_info.field_nof_bits/8))
        {
          entry_found = 1;
          iterator_info->entry_number++;
          continue;
        }        
      }
    }    

    iterator_info->entry_index++;
  }  
    

  /* we need to validate that this is the last entry possible, temp code -> run 100 entries */
  if(iterator_info->entry_index >= iterator_info->max_num_of_iterations)
  {
    iterator_info->is_end = 1;
  }

exit:
  SHR_FUNC_EXIT;
}
