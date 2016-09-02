/*! \file dbal_api.c
 *
 * System wide Logical Table Manager.
 *
 * To be used for:
 *   Access of physical table
 *   Access of pure software tables
 *   Activation of access procedutes (dispatcher) which is
 *     equivalent to 'MBCM and arad_pp_dbal' on SDK6 for JR1.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal.h"
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal_api.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>

/* 
 * this parameter holds all the DBAL SW info. this is a temporary implementation until the SW-state will be ported to Jer2
 */
static dbal_mngr_info_t dbal_mngr = { {{0}} };

/************************************************** APIs IMPLEMENTATION******************************************************************/

shr_error_e
dbal_init(
  int unit)
{
  SHR_FUNC_INIT_VARS(unit);

  if (dbal_mngr.is_intiated == 0)
  {
    SHR_IF_ERR_EXIT(dbal_fields_init(unit));
    SHR_IF_ERR_EXIT(dbal_logical_tables_init(unit));
    SHR_IF_ERR_EXIT(dbal_physical_table_init(unit));

    dbal_mngr.is_intiated = 1;
    LOG_CLI((BSL_META("DBAL init preformed \n\n")));
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_deinit(
  int unit)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_FUNC_EXIT;
}

/* Entry operations */
shr_error_e
dbal_entry_handle_take(
  int unit,
  dbal_tables_e table_id,
  uint32 * entry_handle_id)
{
  int iter;
  dbal_logical_table_t *table;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, table_id, &table));

  for (iter = 0; iter < DBAL_SW_NOF_ENTRY_HANDLES; iter++)
  {
    if (dbal_mngr.entry_handles_pool[iter].status == DBAL_ENTRY_HANDLE_STATUS_AVAILABLE)
    {
      dbal_mngr.entry_handles_pool[iter].status = DBAL_ENTRY_HANDLE_STATUS_IN_USE;
      *entry_handle_id = iter;
      dbal_mngr.entry_handles_pool[iter].table_id = table_id;
      if (table->core_mode == DBAL_CORE_ALL)
      {
        dbal_mngr.entry_handles_pool[iter].core_id = DBAL_CORE_ANY;
      }
      else
      {
        dbal_mngr.entry_handles_pool[iter].core_id = DBAL_CORE_NOT_INTIATED;
      }
      break;
    }
  }

  if (iter == DBAL_SW_NOF_ENTRY_HANDLES)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. dbal_entry_handle_take \n");
  }

  dbal_mngr.entry_handles_pool[iter].table = table;

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_release(
  int unit,
  uint32 entry_handle_id)
{
  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. handle = %d \n", entry_handle_id);
  }

  sal_memset(&(dbal_mngr.entry_handles_pool[entry_handle_id]), 0x0, sizeof(dbal_entry_handle_t));

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_info_get(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_handle_t * entry_handle)
{
  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. handle = %d \n", entry_handle_id);
  }

  (*entry_handle) = (dbal_mngr.entry_handles_pool[entry_handle_id]);

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_get(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags)
{
  dbal_entry_handle_t *entry_handle = &dbal_mngr.entry_handles_pool[entry_handle_id];
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle->num_of_fields == 0)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. no fields where set, cannot perform the action %s", table->table_name);
  }

  switch (table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      SHR_IF_ERR_EXIT(dbal_phy_table_entry_get(unit, table->physical_db_id, table->app_id, &(entry_handle->phy_entry)));
      break;

    case DBAL_ACCESS_DIRECT:
      SHR_IF_ERR_EXIT(dbal_direct_entry_get(unit, entry_handle));
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. illegal access type %d \n", table->access_type);
      break;
  }

  SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle_id, flags, "get"));
  SHR_IF_ERR_EXIT(dbal_fields_from_buffer_get(unit, entry_handle, 1));

exit:
  if (!(flags & DBAL_COMMIT_KEEP_HANDLE))
  {
    dbal_entry_handle_release(unit, entry_handle_id);
  }
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags)
{
  dbal_entry_handle_t *entry_handle = &dbal_mngr.entry_handles_pool[entry_handle_id];
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  if (entry_handle->num_of_fields == 0)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. no fields where set, cannot perform the action %s", table->table_name);
  }

  SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle_id, flags, "commit"));

  DBAL_DOCUMENTATION_MODE_PRINTS;

  if ((table->max_capacity) && (table->nof_entries + 1 > table->max_capacity))
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. logical table full %s\n", table->table_name);
  }
  switch (table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      entry_handle->phy_entry.payload_size = table->entry_payload_size;
      SHR_IF_ERR_EXIT(dbal_phy_table_entry_add(unit, table->physical_db_id, table->app_id, &(entry_handle->phy_entry)));
      break;

    case DBAL_ACCESS_DIRECT:
      SHR_IF_ERR_EXIT(dbal_direct_entry_set(unit, entry_handle));
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. illegal access type %d \n", table->access_type);
      break;
  }

  table->nof_entries++;

  if (table->sw_shadow_enabled)
  {
    dbal_entry_save_in_shadow(unit, entry_handle);
  }

exit:
  if (!(flags & DBAL_COMMIT_KEEP_HANDLE))
  {
    dbal_entry_handle_release(unit, entry_handle_id);
  }
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_delete(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags)
{
  dbal_entry_handle_t *entry_handle = &dbal_mngr.entry_handles_pool[entry_handle_id];
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  DBAL_DOCUMENTATION_MODE_PRINTS;

  if (entry_handle->num_of_fields == 0)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. no fields where set, cannot perform the action %s\n", table->table_name);
  }

  if (entry_handle->phy_entry.payload_size != 0)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. cannot delete entry with value fields %s\n", table->table_name);
  }

  /*
   * TBD - put it in a function 3 type of prints get/delete/add 
   */
  SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle_id, flags, "delete"));

  if (table->nof_entries == 0)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. logical table empty %s \n", table->table_name);
  }

  switch (table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      SHR_IF_ERR_EXIT(dbal_phy_table_entry_delete
                      (unit, table->physical_db_id, table->app_id, &(entry_handle->phy_entry)));
      break;

    case DBAL_ACCESS_DIRECT:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. cannot delete entry from direct table\n");
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. , table->access_typeillegal access type %d\n", table->access_type);
      break;
  }

  table->nof_entries--;

  if (table->sw_shadow_enabled)
  {
    dbal_entry_remove_from_shadow(unit, entry_handle);
  }

exit:
  if (!(flags & DBAL_COMMIT_KEEP_HANDLE))
  {
    dbal_entry_handle_release(unit, entry_handle_id);
  }
  SHR_FUNC_EXIT;
}

/* Entry field operations */

shr_error_e
dbal_entry_array8_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 * field_val)
{
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  dbal_logical_table_t *table = entry_handle->table;
  dbal_fields_e parent_field_id = DBAL_NOF_FIELDS;
  int iter, field_index_in_table = 0;

  SHR_FUNC_INIT_VARS(unit);

  dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id);

  for (iter = 0; iter < table->num_of_fields; iter++)
  {
    if ((table->fields_info[iter].field_id == field_id) || (table->fields_info[iter].field_id == parent_field_id))
    {
      field_index_in_table = iter;
      break;
    }
  }

  if (iter == table->num_of_fields)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                 " Error. field %s not found in table %s\n", dbal_field_to_string(unit, field_id), table->table_name);
  }

  entry_handle->field_ids[entry_handle->num_of_fields] = field_id;
  entry_handle->user_output_info[entry_handle->num_of_fields].returned_pointer = field_val;
  entry_handle->user_output_info[entry_handle->num_of_fields].type = DBAL_FIELD_TYPE_ARRAY8;
  entry_handle->table_field_pos[entry_handle->num_of_fields] = field_index_in_table;
  entry_handle->num_of_fields++;

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_array32_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 * field_val)
{
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  dbal_logical_table_t *table = entry_handle->table;
  dbal_fields_e parent_field_id = DBAL_NOF_FIELDS;
  int iter, field_index_in_table = 0, is_field_updated = 0;

  SHR_FUNC_INIT_VARS(unit);

  /* TBD return error if field is not value */

  dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id);

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    if (entry_handle->field_ids[iter] == field_id || (parent_field_id != DBAL_NOF_FIELDS))
    {
      dbal_fields_e parent_field_id_to_compare = DBAL_NOF_FIELDS;
      if (parent_field_id != DBAL_NOF_FIELDS)
      {
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get
                        (unit, entry_handle->field_ids[iter], &parent_field_id_to_compare));
        if (parent_field_id_to_compare != parent_field_id)
        {
          continue;
        }
        else
        {
          entry_handle->field_ids[iter] = field_id;
        }
      }
      is_field_updated = 1;
      field_index_in_table = entry_handle->table_field_pos[iter];
      break;
    }
  }

  if (!is_field_updated)
  {
    for (iter = 0; iter < table->num_of_fields; iter++)
    {
      if ((table->fields_info[iter].field_id == field_id) || (table->fields_info[iter].field_id == parent_field_id))
      {
        field_index_in_table = iter;
        break;
      }
    }

    if (iter == table->num_of_fields)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL,
                   " Error. field %s not found in table %s\n", dbal_field_to_string(unit, field_id), table->table_name);
    }
  }

  if (!is_field_updated)
  {
    field_index_in_table = entry_handle->num_of_fields;
  }

  entry_handle->field_ids[field_index_in_table] = field_id;
  entry_handle->user_output_info[field_index_in_table].returned_pointer = field_val;
  entry_handle->user_output_info[field_index_in_table].type = DBAL_FIELD_TYPE_ARRAY32;
  entry_handle->table_field_pos[field_index_in_table] = field_index_in_table;
  if (!is_field_updated)
  {
    entry_handle->num_of_fields++;
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e dbal_entry_field8_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 * field_val);
shr_error_e dbal_entry_field16_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint16 * field_val);

shr_error_e
dbal_entry_field32_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 * field_val)
{
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  dbal_logical_table_t *table = entry_handle->table;
  dbal_fields_e parent_field_id = DBAL_NOF_FIELDS;
  int iter, field_index_in_table = 0, is_field_updated = 0;

  SHR_FUNC_INIT_VARS(unit);

  dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id);

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    if (entry_handle->field_ids[iter] == field_id || (parent_field_id != DBAL_NOF_FIELDS))
    {
      dbal_fields_e parent_field_id_to_compare = DBAL_NOF_FIELDS;
      if (parent_field_id != DBAL_NOF_FIELDS)
      {
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get
                        (unit, entry_handle->field_ids[iter], &parent_field_id_to_compare));
        if (parent_field_id_to_compare != parent_field_id)
        {
          continue;
        }
        else
        {
          entry_handle->field_ids[iter] = field_id;
        }
      }
      is_field_updated = 1;
      field_index_in_table = entry_handle->table_field_pos[iter];
      break;
    }
  }

  if (!is_field_updated)
  {
    for (iter = 0; iter < table->num_of_fields; iter++)
    {
      if ((table->fields_info[iter].field_id == field_id) || (table->fields_info[iter].field_id == parent_field_id))
      {
        field_index_in_table = iter;
        break;
      }
    }
    if (iter == table->num_of_fields)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL,
                   " Error. field %s not found in table %s\n", dbal_field_to_string(unit, field_id), table->table_name);
    }
  }

  if (!is_field_updated)
  {
    field_index_in_table = entry_handle->num_of_fields;
  }
  entry_handle->field_ids[field_index_in_table] = field_id;
  entry_handle->user_output_info[field_index_in_table].returned_pointer = field_val;
  entry_handle->user_output_info[field_index_in_table].type = DBAL_FIELD_TYPE_UINT32;
  entry_handle->table_field_pos[field_index_in_table] = field_index_in_table;
  if (!is_field_updated)
  {
    entry_handle->num_of_fields++;
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_field32_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val)
{
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  int iter, num_of_bits = 0;
  dbal_logical_table_t *table = entry_handle->table;
  dbal_fields_e parent_field_id = DBAL_NOF_FIELDS;
  int field_index_in_table = 0;
  int is_field_updated = 0;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id));

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    if (entry_handle->field_ids[iter] == field_id || (parent_field_id != DBAL_NOF_FIELDS))
    {
      dbal_fields_e parent_field_id_to_compare = DBAL_NOF_FIELDS;
      if (parent_field_id != DBAL_NOF_FIELDS)
      {
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get
                        (unit, entry_handle->field_ids[iter], &parent_field_id_to_compare));
        if (parent_field_id_to_compare != parent_field_id)
        {
          continue;
        }
        else
        {
          entry_handle->field_ids[iter] = field_id;
        }
      }
      is_field_updated = 1;
      num_of_bits = table->fields_info[entry_handle->table_field_pos[iter]].field_nof_bits;
      field_index_in_table = entry_handle->table_field_pos[iter];
      break;
    }
  }

  if (!is_field_updated)
  {
    for (iter = 0; iter < table->num_of_fields; iter++)
    {
      if ((table->fields_info[iter].field_id == field_id) || (table->fields_info[iter].field_id == parent_field_id))
      {
        num_of_bits = table->fields_info[iter].field_nof_bits;
        field_index_in_table = iter;
        break;
      }
    }
  }

  if (num_of_bits)
  {
    uint32 field_value_after_encode = field_val;
    if (field_val > ((1<<num_of_bits) - 1 ))
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL," Error. for field %s value exceed limit val=%d max bits allowed %d\n",
                   dbal_field_to_string(unit, field_id), field_val, num_of_bits);
    }    

    if ((field_id == DBAL_FIELD_CORE_ID) && table->core_mode == DBAL_CORE_BY_INPUT)
    {
      /*
       * in this case the core ID will be used as a seperate parameter and not part of the key 
       */
      entry_handle->core_id = field_val;
      entry_handle->table_field_pos[entry_handle->num_of_fields] = field_index_in_table;
      if (!is_field_updated)
      {
        entry_handle->field_ids[entry_handle->num_of_fields] = field_id;
        entry_handle->num_of_fields++;
      }
      SHR_EXIT();
    }

    if (dbal_fields_is_field_encoded(unit, field_id))
    {
      SHR_IF_ERR_EXIT(dbal_fields_field32_encode(unit, field_id, field_val, &field_value_after_encode));
    }
    if (table->fields_info[field_index_in_table].is_key)
    {
      utilex_bitstream_set_any_field(&(field_value_after_encode),
                                     table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                     table->fields_info[field_index_in_table].field_nof_bits,
                                     &(entry_handle->phy_entry.key[0]));
      if (!is_field_updated)
      {
        entry_handle->phy_entry.key_size += table->fields_info[field_index_in_table].field_nof_bits;
      }
    }
    else
    {
      utilex_bitstream_set_any_field(&(field_value_after_encode),
                                     table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                     table->fields_info[field_index_in_table].field_nof_bits,
                                     &(entry_handle->phy_entry.payload[0]));
      if (!is_field_updated)
      {
        uint32 full_mask = -1;
        utilex_bitstream_set_any_field(&full_mask, table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                       table->fields_info[field_index_in_table].field_nof_bits,
                                       &(entry_handle->phy_entry.p_mask[0]));
        entry_handle->phy_entry.payload_size += table->fields_info[field_index_in_table].field_nof_bits;
      }
    }

    entry_handle->table_field_pos[entry_handle->num_of_fields] = field_index_in_table;
    if (!is_field_updated)
    {
      entry_handle->field_ids[entry_handle->num_of_fields] = field_id;
      entry_handle->num_of_fields++;
    }
  }
  else
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                 " Error. field %s not found in table %s\n", dbal_field_to_string(unit, field_id), table->table_name);
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e dbal_entry_field8_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val);
shr_error_e dbal_entry_field16_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint16 field_val);

shr_error_e
dbal_entry_array8_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE])
{
  uint32 field_val_as_uint32[DBAL_FIELD_ARRAY_MAX_SIZE / 4];

  SHR_FUNC_INIT_VARS(unit);

  utilex_U8_to_U32(field_val, DBAL_FIELD_ARRAY_MAX_SIZE, field_val_as_uint32);

  SHR_IF_ERR_EXIT(dbal_entry_array32_set(unit, entry_handle_id, key_mode, field_id, field_val_as_uint32));

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_array32_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE / 4])
{
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  int iter, num_of_bits = 0;
  dbal_logical_table_t *table = entry_handle->table;
  int field_index_in_table = 0;
  dbal_fields_e parent_field_id = DBAL_NOF_FIELDS;
  int is_field_updated = 0;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get(unit, field_id, &parent_field_id));

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    if (entry_handle->field_ids[iter] == field_id || (parent_field_id != DBAL_NOF_FIELDS) )
    {
      dbal_fields_e parent_field_id_to_compare = DBAL_NOF_FIELDS;
      if (parent_field_id != DBAL_NOF_FIELDS)
      {
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_id_get
                        (unit, entry_handle->field_ids[iter], &parent_field_id_to_compare));
        if (parent_field_id_to_compare != parent_field_id)
        {
          continue;
        }
        else
        {
          entry_handle->field_ids[iter] = field_id;
        }
      }
      is_field_updated = 1;
      num_of_bits = table->fields_info[entry_handle->table_field_pos[iter]].field_nof_bits;
      field_index_in_table = entry_handle->table_field_pos[iter];
      break;
    }
  }

  for (iter = 0; iter < table->num_of_fields; iter++)
  {
    if ((table->fields_info[iter].field_id == field_id) || (table->fields_info[iter].field_id == parent_field_id))
    {
      num_of_bits = table->fields_info[iter].field_nof_bits;
      field_index_in_table = iter;
      break;
    }
  }

  if (num_of_bits)
  {

    if (dbal_fields_is_field_encoded(unit, field_id))
    {
      if (table->fields_info[field_index_in_table].field_nof_bits <= 32)
      {
        SHR_IF_ERR_EXIT(dbal_fields_field32_encode(unit, field_id, field_val[0], &field_val[0]));
      }
      else 
      {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. no encoding for array field %s, field is longer than 32bit", dbal_field_to_string(unit, field_id));
      }
    }

    if (table->fields_info[field_index_in_table].is_key)
    {
      utilex_bitstream_set_any_field(field_val, table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                     table->fields_info[field_index_in_table].field_nof_bits,
                                     &(entry_handle->phy_entry.key[0]));
      if (!is_field_updated)
      {
        entry_handle->phy_entry.key_size += table->fields_info[field_index_in_table].field_nof_bits;
      }
    }
    else
    {
      utilex_bitstream_set_any_field(field_val, table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                     table->fields_info[field_index_in_table].field_nof_bits,
                                     &(entry_handle->phy_entry.payload[0]));
      if (!is_field_updated)
      {
        uint32 full_mask[DBAL_FIELD_ARRAY_MAX_SIZE / 4] =
          { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
        entry_handle->phy_entry.payload_size += table->fields_info[field_index_in_table].field_nof_bits;

        utilex_bitstream_set_any_field(full_mask, table->fields_info[field_index_in_table].bits_offset_in_buffer,
                                       table->fields_info[field_index_in_table].field_nof_bits,
                                       &(entry_handle->phy_entry.p_mask[0]));
      }
    }
    entry_handle->table_field_pos[entry_handle->num_of_fields] = field_index_in_table;
    if (!is_field_updated)
    {
      entry_handle->field_ids[entry_handle->num_of_fields] = field_id;
      entry_handle->num_of_fields++;
    }
  }
  else
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. arr32 field %s not found in table\n", dbal_field_to_string(unit, field_id));
  }

exit:
  SHR_FUNC_EXIT;
}                           


shr_error_e dbal_table_iterator_init(
  int                                       unit,  
  dbal_tables_e                             table_id,
  dbal_iterator_mode_e                      mode,
  dbal_iterator_info_t* iterator_info)
{
  int iter;
  dbal_logical_table_t *table;
  int key_size = 0;

  SHR_FUNC_INIT_VARS(unit);

  sal_memset(iterator_info, 0x0, sizeof(dbal_iterator_info_t));

  iterator_info->mode     = mode;
  iterator_info->table_id = table_id;

  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, iterator_info->table_id, &table));

  if(table->access_type != DBAL_ACCESS_DIRECT)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL,"no support for table access_type different than hard logic table");
  }

  if (iterator_info->max_num_of_iterations == 0)
  {
    for (iter = 0; iter < table->num_of_fields; iter++)
    {
      if(table->fields_info[iter].is_key){                
        if (table->fields_info[iter].field_id == DBAL_FIELD_CORE_ID)
        {
          key_size += 1;
        }else
        {
          key_size += table->fields_info[iter].field_nof_bits;
          
        }
      }
    }
  }

  if(key_size > 32)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL,"no supported for key size bigger than 32 bit");
  }else
  {
    iterator_info->max_num_of_iterations = 1<<(key_size);
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e dbal_table_iterator_get_next(
  int unit,
  dbal_iterator_info_t* iterator_info)
{
  dbal_entry_handle_t *entry_handle;
  uint32 entry_handle_id;
  dbal_logical_table_t *table;

  SHR_FUNC_INIT_VARS(unit);

  /* support core_id handle according to the table type */
  /* take the handle here but execute the command from the direct access file */
  
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, iterator_info->table_id, &entry_handle_id));
  entry_handle = &dbal_mngr.entry_handles_pool[entry_handle_id];
  table = entry_handle->table;

  switch (entry_handle->table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. no supported access type %d \n", table->access_type);
      break;

    case DBAL_ACCESS_DIRECT:
      SHR_IF_ERR_EXIT(dbal_direct_entry_get_next(unit, entry_handle_id, iterator_info));
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. illegal access type %d \n", table->access_type);
      break;
  }


  SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle_id, 0, "get_next"));

exit:
  dbal_entry_handle_release(unit, entry_handle_id);
  SHR_FUNC_EXIT;
}


shr_error_e dbal_table_iterator_destroy(
  int unit,
  dbal_iterator_info_t* iterator_info)
{
  SHR_FUNC_INIT_VARS(unit);

  sal_memset(iterator_info, 0x0, sizeof(dbal_iterator_info_t));

/*exit:*/
  SHR_FUNC_EXIT;
}

shr_error_e dbal_table_iterator_info_dump(
  int unit,
  dbal_iterator_info_t* iterator_info)
{
  
	dbal_logical_table_t *table;
  int iter;
	
	SHR_FUNC_INIT_VARS(unit);
	
	SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, iterator_info->table_id, &table));
	
	LOG_CLI((BSL_META("dumping iterator information\n")));
	LOG_CLI((BSL_META("----------------------------\n")));
	LOG_CLI((BSL_META("table %s num 0f fields = %d entry index %d is end %d num of iterations %d\n"), table->table_name, iterator_info->num_of_fields, iterator_info->entry_index, iterator_info->is_end, iterator_info->max_num_of_iterations));
	LOG_CLI((BSL_META("requested fields information \n")));
  for (iter = 0; iter < iterator_info->num_of_fields; iter++)
  {
    LOG_CLI((BSL_META("name %s pointer %p \n"), dbal_field_to_string(unit,iterator_info->requested_fields_info[iter].field_id), iterator_info->requested_fields_info[iter].field_val));
  }
  LOG_CLI((BSL_META("\n")));

exit:
  SHR_FUNC_EXIT;
}





















/**************************************************INTERNAL FUNCTIONS******************************************************************/
shr_error_e
dbal_action_prints(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags,
  char *src_str)
{
  int bsl_severity;
  dbal_entry_handle_t *entry_handle = &dbal_mngr.entry_handles_pool[entry_handle_id];


  SHR_FUNC_INIT_VARS(unit);

  SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

  if (bsl_severity >= bslSeverityInfo)
  {
    LOG_CLI((BSL_META("\n**********************************************************************\n")));
    LOG_CLI((BSL_META("action %s handle ID %d, table %s flags %s \n"), src_str, 
             entry_handle_id, entry_handle->table->table_name, dbal_action_flags_to_string(unit, flags)));

    LOG_CLI((BSL_META("Entry Fields:\n")));

    SHR_IF_ERR_EXIT(dbal_entry_print(unit, entry_handle_id, 0));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_entries_print_from_shadow(
  int unit,
  dbal_tables_e table_id,
  uint8 format,
  uint32 max_entries)
{
  /*
   * printing entries from shadow
   */
  uint32 entry_handle_id;
  dbal_entry_handle_t *entry_handle;
  dbal_logical_table_t *table;
  int shadow_num_of_entries = DBAL_PHYSICAL_TABLE_SHADOW_SIZE;
  int iter, fields_iter;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, table_id, &entry_handle_id));
  entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);

  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, table_id, &table));

  if (table->nof_entries == 0)
  {
    SHR_EXIT();
  }
  LOG_CLI((BSL_META("\nPrinting Entries for table %s\n"), table->table_name));
  LOG_CLI((BSL_META("========================================================================\n\n")));

  if (table->access_type == DBAL_ACCESS_DIRECT)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. this diagnostic not supported for hard logic tables only MDB\n");
  }

  if (!table->sw_shadow_enabled)
  {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. shadowing is not enabled for table %s\n", table->table_name);
  }

  for (iter = 0; iter < shadow_num_of_entries; iter++)
  {
    if (table->entries_shadow[iter].k_mask[0] == 1)
    {
      sal_memcpy(&entry_handle->phy_entry, &(table->entries_shadow[iter]), sizeof(dbal_physical_entry_t));
      entry_handle->num_of_fields = table->num_of_fields;
      for (fields_iter = 0; fields_iter < entry_handle->num_of_fields; fields_iter++)
      {
        entry_handle->field_ids[fields_iter] = table->fields_info[fields_iter].field_id;
        entry_handle->table_field_pos[fields_iter] = fields_iter;
      }
      SHR_IF_ERR_EXIT(dbal_entry_print(unit, entry_handle_id, 0));
    }
  }

  LOG_CLI((BSL_META("\n\n")));

exit:
  dbal_entry_handle_release(unit, entry_handle_id);
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_print(
  int unit,
  uint32 entry_handle_id,
  uint8 format)
{
  int iter;
  dbal_entry_handle_t *entry_handle = &(dbal_mngr.entry_handles_pool[entry_handle_id]);
  dbal_logical_table_t *table = entry_handle->table;
  dbal_table_field_info_t field_info = { 0 };

  SHR_FUNC_INIT_VARS(unit);

  if (1)        /* buffer is printed with low severity */
  {
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nKey: ")));
    for (iter = 0; iter < 10; iter++)
    {
      LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%x:"), entry_handle->phy_entry.key[iter]));
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\tPayload: ")));
    for (iter = 0; iter < 10; iter++)
    {
      LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%x:"), entry_handle->phy_entry.payload[iter]));
    }
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\tPayload mask: ")));
    for (iter = 0; iter < 10; iter++)
    {
      LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%x:"), entry_handle->phy_entry.p_mask[iter]));
    }
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
  }

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    SHR_IF_ERR_EXIT(dbal_table_field_info_get
                    (unit, entry_handle->table_id, entry_handle->field_ids[iter], &field_info));

    if ((entry_handle->field_ids[iter] == DBAL_FIELD_CORE_ID) && table->core_mode == DBAL_CORE_BY_INPUT)
    {
      /*
       * in this case the core ID will be used as a seperate parameter and not part of the key 
       */
      LOG_CLI((BSL_META("%s "), dbal_field_to_string(unit, entry_handle->field_ids[iter])));
      LOG_CLI((BSL_META("%d"), entry_handle->core_id));
    }
    else
    {

      if (table->fields_info[entry_handle->table_field_pos[iter]].is_key)
      {
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_print
                        (unit, entry_handle->field_ids[iter], entry_handle->phy_entry.key,
                         entry_handle->phy_entry.k_mask, field_info.field_nof_bits, field_info.bits_offset_in_buffer));
      }
      else
      {
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_print
                        (unit, entry_handle->field_ids[iter], entry_handle->phy_entry.payload,
                         entry_handle->phy_entry.p_mask, field_info.field_nof_bits, field_info.bits_offset_in_buffer));
      }
    }
    if (iter < entry_handle->num_of_fields - 1)
    {
      LOG_CLI((BSL_META(", ")));
    }
  }

  LOG_CLI((BSL_META("\n")));

exit:
  SHR_FUNC_EXIT;
}

/****************************************INTERNAL FUNCTIONS******************************************************/

/* getting the value of fields with type uint8/16/32/IP */
shr_error_e
dbal_specific_field_from_buffer_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  dbal_fields_e field_id,
  uint32 * field_val_returned,
  uint32 * is_found)
{
  dbal_table_field_info_t field_info = { 0 };
  uint32 field_val, field_value_after_encode;
  int iter;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * TBD when doing get should not receive value it means that we need to use the valid bit
   */
  SHR_IF_ERR_EXIT(dbal_table_field_info_get(unit, entry_handle->table_id, field_id, &field_info));

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    if (entry_handle->field_ids[iter] == field_id)
    {
      break;
    }
  }

  if (iter == entry_handle->num_of_fields)
  {
    *is_found = 0;
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                 " Error. field %s not exists in buffer %s ", dbal_field_to_string(unit, field_id), table->table_name);
  }

  if (field_id == DBAL_FIELD_CORE_ID)
  {
    (*field_val_returned) = entry_handle->core_id;
  }
  else
  {
    if(field_info.field_nof_bits > 32)
    {
      SHR_ERR_EXIT(_SHR_E_INTERNAL,
                 " Error. field %s bigger than 32bit ", dbal_field_to_string(unit, field_id));
    }

    if (!table->fields_info[entry_handle->table_field_pos[iter]].is_key)
    {
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                      (entry_handle->phy_entry.payload, field_info.bits_offset_in_buffer, field_info.field_nof_bits,
                       &field_val));
      if (dbal_fields_is_field_encoded(unit, field_id))
      {
        SHR_IF_ERR_EXIT(dbal_fields_field32_decode
                        (unit, entry_handle->field_ids[iter], field_val, &field_value_after_encode));
        (*field_val_returned) = field_value_after_encode;
      }
      else
      {
        (*field_val_returned) = field_val;
      }
    }
    else
    {
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                      (entry_handle->phy_entry.key, field_info.bits_offset_in_buffer, field_info.field_nof_bits,
                       &field_val));
      if (dbal_fields_is_field_encoded(unit, field_id))
      {
        SHR_IF_ERR_EXIT(dbal_fields_field32_decode
                        (unit, entry_handle->field_ids[iter], field_val, &field_value_after_encode));
        (*field_val_returned) = field_value_after_encode;
      }
      else
      {
        (*field_val_returned) = field_val;
      }
    }
  }

  *is_found = 1;

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_from_buffer_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int is_only_payload_fields /*TBD not supported yet only payload field are given */ )
{
  int iter;
  dbal_table_field_info_t field_info = { 0 };
  uint32 field_val, field_value_after_encode;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  for (iter = 0; iter < entry_handle->num_of_fields; iter++)
  {
    SHR_IF_ERR_EXIT(dbal_table_field_info_get
                    (unit, entry_handle->table_id, entry_handle->field_ids[iter], &field_info));

    switch (entry_handle->user_output_info[iter].type)
    {

      case DBAL_FIELD_TYPE_UINT32:
      case DBAL_FIELD_TYPE_INT32:
      {
        if (!table->fields_info[entry_handle->table_field_pos[iter]].is_key)
        {
          SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                          (entry_handle->phy_entry.payload, field_info.bits_offset_in_buffer, field_info.field_nof_bits,
                           &field_val));
          if (dbal_fields_is_field_encoded(unit, entry_handle->field_ids[iter]))
          {
            SHR_IF_ERR_EXIT(dbal_fields_field32_decode
                            (unit, entry_handle->field_ids[iter], field_val, &field_value_after_encode));
            (*(uint32 *) (entry_handle->user_output_info[iter].returned_pointer)) = field_value_after_encode;
          }
          else
          {
            (*(uint32 *) (entry_handle->user_output_info[iter].returned_pointer)) = field_val;
          }
        }
      }
        break;

      case DBAL_FIELD_TYPE_ARRAY32:
      {
        uint32 field_array32_val[DBAL_FIELD_ARRAY_MAX_SIZE];
        if (!table->fields_info[entry_handle->table_field_pos[iter]].is_key)
        {
          SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                          (entry_handle->phy_entry.payload, field_info.bits_offset_in_buffer, field_info.field_nof_bits,
                           field_array32_val));
          if (dbal_fields_is_field_encoded(unit, entry_handle->field_ids[iter]))
          {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " Error. no decoding func for array field %s",
                         dbal_field_to_string(unit, entry_handle->field_ids[iter]));
          }
          else
          {
            sal_memcpy((uint32 *) entry_handle->user_output_info[iter].returned_pointer, field_array32_val,
                       4 * DBAL_FIELD_ARRAY_MAX_SIZE);
          }
        }

      }
        break;

      case DBAL_FIELD_TYPE_ARRAY8:
      {
        uint32 field_array32_val[DBAL_FIELD_ARRAY_MAX_SIZE];
        if (!table->fields_info[entry_handle->table_field_pos[iter]].is_key)
        {
          SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                          (entry_handle->phy_entry.payload, field_info.bits_offset_in_buffer, field_info.field_nof_bits,
                           field_array32_val));
          if (dbal_fields_is_field_encoded(unit, entry_handle->field_ids[iter]))
          {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " Error. no decoding func for array field %s\n",
                         dbal_field_to_string(unit, entry_handle->field_ids[iter]));
          }
          else
          {
            utilex_U32_to_U8(field_array32_val, DBAL_FIELD_ARRAY_MAX_SIZE,
                             ((uint8 *) entry_handle->user_output_info[iter].returned_pointer));
          }
        }
      }
        break;

      case DBAL_FIELD_TYPE_BOOL:       /* this field type is not used when using the get field functionality */
      case DBAL_FIELD_TYPE_IP:
      case DBAL_FIELD_TYPE_NONE:
      case DBAL_FIELD_TYPE_BITMAP:
        if (!table->fields_info[entry_handle->table_field_pos[iter]].is_key)
        {
          SHR_ERR_EXIT(_SHR_E_INTERNAL,
                       " Error. wrong type %s %d \n",
                       dbal_field_type_to_string(unit, entry_handle->user_output_info[iter].type),
                       entry_handle->user_output_info[iter].type);
        }
        break;

      default:
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     " Error. wrong type %s %d\n",
                     dbal_field_type_to_string(unit, entry_handle->user_output_info[iter].type),
                     entry_handle->user_output_info[iter].type);
        break;
    }
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_from_buffer_print(
  int unit,
  dbal_fields_e field_id,
  uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE],
  uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE],
  int size,
  int offset)
{
  uint32 val_as_uint32;
  uint32 decoded_field_value;
  /*
   * uint16 val_as_uint16;
   * uint8  val_as_uint8;
   */
  int iter;
  dbal_field_basic_info_t *field_info;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_fields_field_info_get(unit, field_id, &field_info));

  LOG_CLI((BSL_META("%s "), field_info->name));

  switch (field_info->type)
  {
    case DBAL_FIELD_TYPE_INT32:
    case DBAL_FIELD_TYPE_UINT32:      
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, size,&val_as_uint32));
      if (dbal_fields_is_field_encoded(unit, field_id))
      {
        SHR_IF_ERR_EXIT(dbal_fields_field32_decode(unit, field_id, val_as_uint32, &decoded_field_value));
        LOG_CLI((BSL_META("encoded 0x%x (%d) decoded 0x%x (%d)"), val_as_uint32, val_as_uint32, decoded_field_value,
                 decoded_field_value));
      }
      else
      {
        LOG_CLI((BSL_META("0x%x (%d)"), val_as_uint32, val_as_uint32));
      }
      break;

    case DBAL_FIELD_TYPE_IP:
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, size,&val_as_uint32));
      LOG_CLI((BSL_META(" %d.%d.%d.%d (0x%x)"), ((val_as_uint32) & 0xff), ((val_as_uint32 >> 8) & 0xff),
               ((val_as_uint32 >> 16) & 0xff), ((val_as_uint32 >> 24) & 0xff), val_as_uint32));
      break;

    case DBAL_FIELD_TYPE_BITMAP:
      for (iter = 0; iter < size; iter++)
      {
        LOG_CLI((BSL_META("bitmap: ")));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, 1,&val_as_uint32));        
        offset = offset + 1;
        LOG_CLI((BSL_META("%d"), val_as_uint32));
      }
      break;

    case DBAL_FIELD_TYPE_BOOL:
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, size,&val_as_uint32));
      if (val_as_uint32 == 0)
      {
        LOG_CLI((BSL_META("False")));
      }
      else
      {
        LOG_CLI((BSL_META("True")));
      }
      break;

    case DBAL_FIELD_TYPE_ARRAY8:
      for (iter = 0; iter < (size / 8) - 1; iter++)
      {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, 8,&val_as_uint32));        
        offset = offset + 8;
        LOG_CLI((BSL_META("%x:"), val_as_uint32));
      }
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, 8,&val_as_uint32));        
      LOG_CLI((BSL_META("%x "), val_as_uint32));
      break;

     case DBAL_FIELD_TYPE_ARRAY32:
      for (iter = 0; iter < (size / 32) - 1; iter++)
      {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, 32,&val_as_uint32));
        offset = offset + 32;
        LOG_CLI((BSL_META("%x:"), val_as_uint32));
      }
      SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(field_val, offset, 32,&val_as_uint32));
      LOG_CLI((BSL_META("%x "), val_as_uint32));
      break;

    default:
      break;
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_save_in_shadow(
  int unit,
  dbal_entry_handle_t * entry_handle)
{
  int entry_index = 0;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  switch (table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      /*
       * TBD this should be done by link-list or other SW, maybe import from jer1 currently implemented with small array 
       */

      for (entry_index = 0; entry_index < DBAL_PHYSICAL_TABLE_SHADOW_SIZE; entry_index++)
      {
        if (table->entries_shadow[entry_index].k_mask[0] == 0)
        {
          sal_memcpy(&(table->entries_shadow[entry_index]), &(entry_handle->phy_entry), sizeof(dbal_physical_entry_t));
          break;
        }
      }
      break;

    case DBAL_ACCESS_DIRECT:
      if (0)
      {
        if (entry_handle->phy_entry.key_size > 32)
        {
          SHR_ERR_EXIT(_SHR_E_INTERNAL,
                       " Error. key size longer than 32 bit is not supported key siz %d\n",
                       entry_handle->phy_entry.key_size);
        }
        entry_index = entry_handle->phy_entry.key[0];
        sal_memcpy(&(table->entries_shadow[entry_index]), &(entry_handle->phy_entry), sizeof(dbal_physical_entry_t));
      }
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. illegal access type %d \n", table->access_type);
      break;
  }

  table->entries_shadow[entry_index].k_mask[0] = 1;

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_remove_from_shadow(
  int unit,
  dbal_entry_handle_t * entry_handle)
{
  int entry_index = 0;
  dbal_logical_table_t *table = entry_handle->table;

  SHR_FUNC_INIT_VARS(unit);

  switch (table->access_type)
  {
    case DBAL_ACCESS_PHY_TABLE:
      /*
       * TBD this should be done by link-list or other SW, maybe import from jer1 currently implemented with small array 
       */

      for (entry_index = 0; entry_index < DBAL_PHYSICAL_TABLE_SHADOW_SIZE; entry_index++)
      {
        if (!sal_memcmp(&table->entries_shadow[entry_index], &(entry_handle->phy_entry), sizeof(dbal_physical_entry_t)))
        {
          sal_memset(&table->entries_shadow[entry_index], 0x0, sizeof(dbal_physical_entry_t));
          break;

        }
      }
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. entry not found\n");
      break;

    case DBAL_ACCESS_DIRECT:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. delete entry to direct \n");
      break;

    default:
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. illegal access type %d \n", table->access_type);
      break;
  }

  table->entries_shadow[entry_index].k_mask[0] = 1;

exit:
  SHR_FUNC_EXIT;
}

int
dbal_work_mode_is_only_sw(
  int unit)
{
  return dbal_mngr.work_mode & DBAL_WORK_MODE_SW_ONLY;
}

shr_error_e
dbal_sw_work_mode_set(
  int unit,
  int enable)
{
  SHR_FUNC_INIT_VARS(unit);

  if (enable)
  {
    dbal_mngr.work_mode = dbal_mngr.work_mode | DBAL_WORK_MODE_SW_ONLY;
    LOG_CLI((BSL_META("only SW work mode Enabled\n")));
  }
  else
  {
    dbal_mngr.work_mode &= ~(DBAL_WORK_MODE_SW_ONLY);
    LOG_CLI((BSL_META("only SW work mode Disabled\n")));
  }

  SHR_FUNC_EXIT;
}

shr_error_e
dbal_log_severity_get(
  int unit,
  bsl_severity_t * severity)
{

  SHR_FUNC_INIT_VARS(unit);

  SHR_GET_SEVERITY_FOR_MODULE(*severity);

  SHR_FUNC_EXIT;
}

shr_error_e
dbal_log_severity_set(
  int unit,
  int severity)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_SET_SEVERITY_FOR_MODULE(severity);

  SHR_FUNC_EXIT;
}

shr_error_e
dbal_handles_info_dump(
  int unit,
  int handle_id)
{
  int iter;
  int free_handle_counter = 0;

  SHR_FUNC_INIT_VARS(unit);

  for (iter = 0; iter < DBAL_SW_NOF_ENTRY_HANDLES; iter++)
  {

    if (dbal_mngr.entry_handles_pool[iter].status != DBAL_ENTRY_HANDLE_STATUS_AVAILABLE)
    {
      LOG_CLI((BSL_META("Handle %d is been used by table %s\n"), iter,
               dbal_mngr.entry_handles_pool[iter].table->table_name));
    }
    else
    {
      free_handle_counter++;
    }
  }

  LOG_CLI((BSL_META("Total available handles %d of %d handles \n"), free_handle_counter, DBAL_SW_NOF_ENTRY_HANDLES));

  if (handle_id != -1)
  {
    /*
     * TBD print specific handle info 
     */
  }

  SHR_FUNC_EXIT;
}

