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

/**************************************************DB hardcoded Initialization ******************************************************************/

#define ENCODE_NONE                         {DBAL_VALUE_ENCODE_NONE,0, DBAL_FIELD_EMPTY}

static dbal_logical_tables_info_t logical_tables_info = { };

void
dbal_table_info_print_all_table_init_params(
  int unit)
{

  int i, j, k;
  dbal_logical_table_t *table;

  table = logical_tables_info.logical_tables;

  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    if ((table->table_name != '\0')&&(sal_strcmp(table->table_name,"") != 0))
    {
      LOG_CLI((BSL_META("name:%s\t, core_mode:%d, access_type:%d\n"), table->table_name, table->core_mode,
               table->access_type));
      LOG_CLI((BSL_META("physical_db_id:%d, app_id:%d, nof_fields:%d\n"), table->physical_db_id, table->app_id,
               table->num_of_fields));

      LOG_CLI((BSL_META("Fields info:\n")));
      for (j = 0; j < table->num_of_fields; j++)
      {
        LOG_CLI((BSL_META("field=%d, field_nof_bits=%d, is_key=%d\n"), table->fields_info[j].field_id,
                 table->fields_info[j].field_nof_bits, table->fields_info[j].is_key));
      }

      LOG_CLI((BSL_META("access info:\n")));
      for (j = 0; j < DBAL_NOF_ACCESS_TYPES; j++)
      {
        LOG_CLI((BSL_META("\ttype=%d,num_of_access_fields=%d\n"), j,table->l2p_direct_info[j].num_of_access_fields));
        for (k = 0; k < table->l2p_direct_info[j].num_of_access_fields; k++)
        {
          LOG_CLI((BSL_META("\t\tfield=%d,nofbits=%d,offset=%d,condition=%d,condparam=%d\n"),
                   table->l2p_direct_info[j].l2p_fields_info[k].field_id,
                   table->l2p_direct_info[j].l2p_fields_info[k].nof_bits,
                   table->l2p_direct_info[j].l2p_fields_info[k].offset,
                   table->l2p_direct_info[j].l2p_fields_info[k].condition_mode,
                   table->l2p_direct_info[j].l2p_fields_info[k].condition_input_param));
          LOG_CLI((BSL_META("\t\tmemory=%d,register=%d,array_index=%d,hw_field=%d\n"),
                   table->l2p_direct_info[j].l2p_fields_info[k].memory,
                   table->l2p_direct_info[j].l2p_fields_info[k].reg[0],
                   table->l2p_direct_info[j].l2p_fields_info[k].mem_array_offset_info.input_param,
                   table->l2p_direct_info[j].l2p_fields_info[k].hw_field));
          LOG_CLI((BSL_META("\t\toffset1: encode=%d,param=%d,field=%d\n"),
                   table->l2p_direct_info[j].l2p_fields_info[k].memory_offset_info.encode_mode,
                   table->l2p_direct_info[j].l2p_fields_info[k].memory_offset_info.input_param,
                   table->l2p_direct_info[j].l2p_fields_info[k].memory_offset_info.field_id));
          LOG_CLI((BSL_META("\t\toffset2: encode=%d,param=%d,field=%d\n"),
                   table->l2p_direct_info[j].l2p_fields_info[k].field_offset_info.encode_mode,
                   table->l2p_direct_info[j].l2p_fields_info[k].field_offset_info.input_param,
                   table->l2p_direct_info[j].l2p_fields_info[k].field_offset_info.field_id));
        }
      }
    }
    table++;
  }
}

/**************************************************DBAL Initialization ******************************************************************/
shr_error_e
dbal_logical_tables_init(
  int unit)
{
  int i, j, k;
  int bits_offset_in_key = 0, bits_offset_in_payload = 0;
  int is_core_mode_valid = 0;

  dbal_logical_table_t *table = NULL;

  SHR_FUNC_INIT_VARS(unit);
  SHR_IF_ERR_EXIT(dbal_db_init_logical_tables_set_default(unit, logical_tables_info.logical_tables));
  SHR_IF_ERR_EXIT(dbal_db_init_mdb_logical_tables(unit, DBAL_INIT_FLAGS_NONE, logical_tables_info.logical_tables));
  SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_logical_tables(unit, DBAL_INIT_FLAGS_NONE, logical_tables_info.logical_tables));

  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    /*
     * TBD init each table
     * update shadow if needed
     */

    table = &logical_tables_info.logical_tables[i];
    if (table->is_table_initiated == 1)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. init alredy preformed on table %s\n", table->table_name);
    }

    is_core_mode_valid = 0;
    bits_offset_in_key = 0;
    bits_offset_in_payload = 0;

    if (table->sw_shadow_enabled == 1)
    {
      int shadow_table_size = 4096;
      if (table->access_type != DBAL_ACCESS_DIRECT)
      {
        shadow_table_size = DBAL_PHYSICAL_TABLE_SHADOW_SIZE;
      }
      table->entries_shadow =
        sal_alloc(shadow_table_size * sizeof(dbal_physical_entry_t), "logical_table_init table shadow");

      if (!table->entries_shadow)
      {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. table alloc failed table %d \n", i);
      }
      sal_memset(table->entries_shadow, 0x0, shadow_table_size * sizeof(dbal_physical_entry_t));
    }

    if (table->core_mode == DBAL_CORE_ALL)
    {
      is_core_mode_valid = 1;
    }
    for (j = 0; j < table->num_of_fields; j++)
    {

      if (table->fields_info[j].field_id == DBAL_FIELD_EMPTY)
      {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. invalid field ID in table %s \n", table->table_name);
      }

      if (dbal_fields_is_field_encoded(unit, table->fields_info[j].field_id))
      {
        table->fields_info[j].is_field_encoded = 1;
      }

      if (table->fields_info[j].field_nof_bits == 0)
      {
        uint32 field_size = 0;
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, table->fields_info[j].field_id, &field_size));
        table->fields_info[j].field_nof_bits = field_size;
      }

      if (!table->fields_info[j].is_key)
      {
        table->entry_payload_size += table->fields_info[j].field_nof_bits;
      }

      if ((table->core_mode == DBAL_CORE_BY_INPUT) && (table->fields_info[j].is_key)
          && (table->fields_info[j].field_id == DBAL_FIELD_CORE_ID))
      {
        is_core_mode_valid = 1; /* if the table is configured by core, the core field must be present in the key */
      }
      else
      {

        if (table->fields_info[j].is_key)
        {
          table->fields_info[j].bits_offset_in_buffer = bits_offset_in_key;
          bits_offset_in_key += table->fields_info[j].field_nof_bits;

        }
        else
        {
          table->fields_info[j].bits_offset_in_buffer = bits_offset_in_payload;
          bits_offset_in_payload += table->fields_info[j].field_nof_bits;
        }
      }
    }
    if (table->access_type == DBAL_ACCESS_DIRECT)
    {
      int field_index_in_table = 0;
      for (k = 0; k < DBAL_NOF_ACCESS_TYPES; k++)
      {
        for (j = 0; j < table->l2p_direct_info[k].num_of_access_fields; j++)
        {
          {

            /*
             * look for the corresponding field in the table and set position 
             */
            for (field_index_in_table = 0; field_index_in_table < table->num_of_fields; field_index_in_table++)
            {
              if (table->fields_info[field_index_in_table].field_id ==
                  table->l2p_direct_info[k].l2p_fields_info[j].field_id)
              {
                break;
              }
            }
            if (field_index_in_table == table->num_of_fields)
            {
              SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. field %d in L2P not found in table %s\n",
                           table->l2p_direct_info[k].l2p_fields_info[j].field_id, table->table_name);
            }

            table->l2p_direct_info[k].l2p_fields_info[j].field_pos_in_table = field_index_in_table;

            if (table->l2p_direct_info[k].l2p_fields_info[j].nof_bits == 0)
            {
              table->l2p_direct_info[k].l2p_fields_info[j].nof_bits =
                table->fields_info[field_index_in_table].field_nof_bits;
            }

            table->l2p_direct_info[k].l2p_fields_info[j].offset +=
              table->fields_info[field_index_in_table].bits_offset_in_buffer;
          }
          /*
           * TBD, in direct 
           * validate that all the fields are results
           * validate that all fields from the same memory are packed
           * validate that all fields in the logical 2 phy exists in the table fields
           * validate that all memory index mode aof the same memory are the same or handle this issue if needed 
           */
        }
      }
    }
    if (is_core_mode_valid != 1)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. core mode is by_core but field core_id not exists in table %s\n",
                   table->table_name);
    }
    table->is_table_initiated = 1;
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_table_get(
  int unit,
  dbal_tables_e table_id,
  dbal_logical_table_t ** table)
{
  (*table) = &logical_tables_info.logical_tables[table_id];
  return 0;
}

/* Table operations */
shr_error_e dbal_table_clear(
  int unit,
  dbal_tables_e table_id,
  uint8 mode);

shr_error_e
dbal_table_field_info_get(
  int unit,
  dbal_tables_e table_id,
  dbal_fields_e field_id,
  dbal_table_field_info_t * field_info)
{
  int i;
  dbal_logical_table_t *table;
  dbal_fields_e parent_field_id;

  SHR_FUNC_INIT_VARS(unit);

  (*field_info).field_id = DBAL_FIELD_EMPTY;
  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, table_id, &table));
  SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id));

  for (i = 0; i < table->num_of_fields; i++)
  {
    if ((table->fields_info[i].field_id == field_id) || (table->fields_info[i].field_id == parent_field_id))
    {
      (*field_info) = table->fields_info[i];
      break;
    }
  }

  if ((*field_info).field_id == DBAL_FIELD_EMPTY)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. field %s not found in table %-20s \n", dbal_field_to_string(unit, field_id),
                 table->table_name);
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_info_print(
  int unit,
  dbal_tables_e table_id)
{
  int i, is_label_printed = 0;
  char *true_string = "KEY    ";
  char *false_string = "PAYLOAD";
  dbal_logical_table_t *table;
  int key_size = 0, payload_size = 0;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, table_id, &table));
  if (table->is_table_initiated == 0)
  {
    LOG_INFO_EX(BSL_LOG_MODULE, "\n table is not initiated %s %s%s%s", table->table_name, EMPTY, EMPTY, EMPTY);
    SHR_EXIT();
  }
  LOG_CLI((BSL_META("Printing info for logical table %-20s\n"), table->table_name));
  LOG_CLI((BSL_META("==============================================\n")));
  /*
   * TBD dump max capacity, min index, sw_shadow_enabled 
   */
  LOG_CLI((BSL_META("Access type: %-10s "), dbal_access_type_to_string(unit, table->access_type)));
  if (table->access_type == DBAL_ACCESS_PHY_TABLE)
  {
    LOG_CLI((BSL_META("Name: %-20s\n"), dbal_physical_table_to_string(unit, table->physical_db_id)));
  }
  else
  {
    LOG_CLI((BSL_META("\n")));
  }
  LOG_CLI((BSL_META("Table Labels: ")));
  for (i = 0; i < DBAL_NOF_LABEL_TYPES; i++)
  {
    if (table->labels[i] != DBAL_LABEL_NONE)
    {
      if (is_label_printed == 1)
      {
        LOG_CLI((BSL_META(", ")));
      }
      LOG_CLI((BSL_META("%s"), dbal_label_to_string(unit, table->labels[i])));
      is_label_printed = 1;
    }
  }
  LOG_CLI((BSL_META("\n")));
  LOG_CLI((BSL_META("Core mode: %s\n"), dbal_core_mode_to_string(unit, table->core_mode)));
  LOG_CLI((BSL_META("Max capacity: %d, Shadow %s, "), table->max_capacity,
           table->sw_shadow_enabled ? "ENABELD" : "DISABELD"));
  LOG_CLI((BSL_META("Entries added: %d\n"), table->nof_entries));
  LOG_CLI((BSL_META("Table fields\n")));
  LOG_CLI((BSL_META("------------\n")));

  LOG_CLI((BSL_META("\tField Name        |Size|buffer offset|Type\n")));
  LOG_CLI((BSL_META("\t------------------------------------------\n")));
  for (i = 0; i < table->num_of_fields; i++)
  {

    LOG_CLI((BSL_META("\t%-18s|"), dbal_field_to_string(unit, table->fields_info[i].field_id)));
    LOG_CLI((BSL_META("%-4d|"), table->fields_info[i].field_nof_bits));
    LOG_CLI((BSL_META("%-13d|"), table->fields_info[i].bits_offset_in_buffer));
    LOG_CLI((BSL_META("%s\n"), table->fields_info[i].is_key ? true_string : false_string));
    if (table->fields_info[i].is_key)
    {
      key_size += table->fields_info[i].field_nof_bits;
    }
    else
    {
      payload_size += table->fields_info[i].field_nof_bits;
    }
  }

  LOG_CLI((BSL_META("\n\tTotal key size %d, Total payload size %d\n\n"), key_size, payload_size));

  if (table->access_type == DBAL_ACCESS_DIRECT)
  {
    char *memory_name;
    char *reg_name;
    char *field_name;
    int j;
    LOG_CLI((BSL_META("\tDirect access logical to phisical info:\n")));
    LOG_CLI((BSL_META
             ("\t  Field name        |bit size|offset|Mapped to Mem/Reg                 |HW field                  |array| mem offset   |field offset  |Condition\n")));
    LOG_CLI((BSL_META
             ("\t  ----------------------------------------------------------------------------------------------------------------------------------------------\n")));
    for (i = 0; i < DBAL_NOF_ACCESS_TYPES; i++)
    {
      for (j = 0; j < table->l2p_direct_info[i].num_of_access_fields; j++)
      {
        switch (i)
        {
          case DBAL_DIRECT_ACCESS_MEMORY:
            memory_name = SOC_MEM_NAME(unit, table->l2p_direct_info[i].l2p_fields_info[j].memory);
            field_name = SOC_FIELD_NAME(unit, table->l2p_direct_info[i].l2p_fields_info[j].hw_field);
            /*
             * TBD handle memory/register 
             */
          LOG_CLI((BSL_META("\t  %-18s|%-8d|%-6d|%-30smem |%-26s|%-5d|%-12s %d|%-12s %d|"),
                     dbal_field_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].field_id),
                     table->l2p_direct_info[i].l2p_fields_info[j].nof_bits,
                   table->l2p_direct_info[i].l2p_fields_info[j].offset, memory_name, field_name, table->l2p_direct_info[i].l2p_fields_info[j].mem_array_offset_info.input_param,
                   dbal_encode_type_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].mem_array_offset_info.encode_mode), table->l2p_direct_info[i].l2p_fields_info[j].mem_array_offset_info.input_param,
                    dbal_encode_type_to_string(unit,table->l2p_direct_info[i].l2p_fields_info[j].field_offset_info.encode_mode), table->l2p_direct_info[i].l2p_fields_info[j].field_offset_info.input_param));
            if (table->l2p_direct_info[i].l2p_fields_info[j].condition_mode == DBAL_CONDITION_NONE)
            {
              LOG_CLI((BSL_META("\n")));
            }
            else
            {
              LOG_CLI((BSL_META("%-10s %d\n"),
                       dbal_condition_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].condition_mode),
                       table->l2p_direct_info[i].l2p_fields_info[j].condition_input_param));
            }
            break;
          case DBAL_DIRECT_ACCESS_REGISTER:
            reg_name = SOC_REG_NAME(unit, table->l2p_direct_info[i].l2p_fields_info[j].reg[0]);
            field_name = SOC_FIELD_NAME(unit, table->l2p_direct_info[i].l2p_fields_info[j].hw_field);
            /*
             * TBD handle memory/register 
             */
            LOG_CLI((BSL_META("\t  %-18s|%-8d|%-6d|%-30sreg|%-26s|0    |%-12s %d|%-12s %d|"),
                     dbal_field_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].field_id),
                     table->l2p_direct_info[i].l2p_fields_info[j].nof_bits,
                     table->l2p_direct_info[i].l2p_fields_info[j].offset, reg_name, field_name, 
                     dbal_encode_type_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].mem_array_offset_info.encode_mode), table->l2p_direct_info[i].l2p_fields_info[j].mem_array_offset_info.input_param,
                      dbal_encode_type_to_string(unit,table->l2p_direct_info[i].l2p_fields_info[j].field_offset_info.encode_mode), table->l2p_direct_info[i].l2p_fields_info[j].field_offset_info.input_param));
            if (table->l2p_direct_info[i].l2p_fields_info[j].condition_mode == DBAL_CONDITION_NONE)
            {
            LOG_CLI((BSL_META("none      \n")));
            }
            else
            {
              LOG_CLI((BSL_META("%-10s %d\n"),
                       dbal_condition_to_string(unit, table->l2p_direct_info[i].l2p_fields_info[j].condition_mode),
                       table->l2p_direct_info[i].l2p_fields_info[j].condition_input_param));
            }
            break;
          case DBAL_DIRECT_ACCESS_PEMLA:
            continue;
        }
      }
    }
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_max_entry_size_get(
  int unit,
  dbal_physical_tables_e physical_table,
  int *max_entry_size)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  (*max_entry_size) = 0;

  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    if ((logical_tables_info.logical_tables[i].physical_db_id == physical_table)
        && (logical_tables_info.logical_tables[i].access_type == DBAL_ACCESS_PHY_TABLE))
    {
      if ((*max_entry_size) < logical_tables_info.logical_tables[i].entry_payload_size)
      {
        (*max_entry_size) = logical_tables_info.logical_tables[i].entry_payload_size;
      }
    }
  }

  SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_dump_all(
  int unit,
  dbal_labels_e label,
  int mode /*1=full */ )
{

  int i, j;
  dbal_logical_table_t *table;

  SHR_FUNC_INIT_VARS(unit);

  if (label == DBAL_LABEL_NONE)
  {
    LOG_CLI((BSL_META("\n\n LOGICAL TABLES\n")));
    LOG_CLI((BSL_META(" ==============\n\n")));
  }
  else
  {
    LOG_CLI((BSL_META("\n\n Dumping all tables related to label %s\n\n"), dbal_label_to_string(unit, label)));
    LOG_CLI((BSL_META(" =============================================\n\n")));
  }

  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, i, &table));
    if (label == DBAL_LABEL_NONE)
    {
      if (mode)
      {
        SHR_IF_ERR_EXIT(dbal_table_info_print(unit, i));
        LOG_CLI((BSL_META("\n\n")));
      }
      else
      {
        LOG_CLI((BSL_META("\t%s (%d)\n"), table->table_name, i));
      }
    }
    else
    {
      for (j = 0; j < DBAL_NOF_LABEL_TYPES; j++)
        if ((table->labels[j] == label) || (table->labels[j] == DBAL_NOF_LABEL_TYPES))
        {
          if (mode)
          {
            SHR_IF_ERR_EXIT(dbal_table_info_print(unit, i));
            LOG_CLI((BSL_META("\n\n")));
          }
          else
          {
            LOG_CLI((BSL_META("\t%s (%d)\n"), table->table_name, i));
          }
          break;
        }
    }
  }

exit:
  SHR_FUNC_EXIT;
}
