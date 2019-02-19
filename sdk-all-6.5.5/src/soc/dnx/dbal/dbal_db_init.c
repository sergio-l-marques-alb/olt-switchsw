
/*! \file dbal_db_init.c
 *
 * Main functions for init the dbal fields and logical tables DB
 *
 */
/*
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

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "dbal_db_init.h"
#include "dbal_string_mgmt.h"

/*************
 * EXTERNS   *
 *************/
extern shr_error_e dbal_fields_encode_prefix(
  int unit,
  uint32 field_val,
  uint32 prefix_val,
  uint32 prefix_length,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_decode_prefix(
  int unit,
  uint32 field_val,
  uint32 prefix_length,
  uint32 unused,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_encode_bool(
  int unit,
  uint32 field_val,
  uint32 suffix_val,
  uint32 suffix_offset,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_decode_bool(
  int unit,
  uint32 field_val,
  uint32 prefix_length,
  uint32 unused,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_encode_suffix(
  int unit,
  uint32 field_val,
  uint32 suffix_val,
  uint32 suffix_offset,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_decode_suffix(
  int unit,
  uint32 field_val,
  uint32 unused,
  uint32 suffix_offset,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_encode_subtract(
  int unit,
  uint32 field_val,
  uint32 subtract_val,
  uint32 unused,
  uint32 * field_val_out);
extern shr_error_e dbal_fields_decode_subtract(
  int unit,
  uint32 field_val,
  uint32 subtracted_val,
  uint32 unused,
  uint32 * field_val_out);

/*************
 *  GLOBALS  *
 *************/
static char *label_delimiters = "\n\t ";

/*************
 * FUNCTIONS *
 *************/
shr_error_e
dbal_db_init_fields_set_default(
  int unit,
  dbal_field_basic_info_t * field_info)
{
  int i;

  dbal_field_basic_info_t field_default =
    { "", 0, DBAL_NOF_FIELD_TYPES, {DBAL_LABEL_NONE}, DBAL_NOF_FIELDS, 0, 0, DBAL_VALUE_ENCODE_NONE, 0, 0, NULL, NULL };

  SHR_FUNC_INIT_VARS(unit);

  /*
   * Initialize all fields
   */
  for (i = 0; i < DBAL_NOF_FIELDS; i++)
  {
    field_info[i] = field_default;
  }

  SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_logical_tables_set_default(
  int unit,
  dbal_logical_table_t * table_info)
{
  int i;

  dbal_logical_table_t table_default = { "", {DBAL_LABEL_NONE}, 0, 0, DBAL_CORE_NONE, 0, 0,
  DBAL_NOF_ACCESS_TYPES, DBAL_PHYSICAL_TABLE_NONE, 0, 0, 0,
  {{DBAL_FIELD_EMPTY, 0, 0, 0, 0, 0, 0, 0}},
  0, {{0}}, NULL, NULL
  };

  SHR_FUNC_INIT_VARS(unit);

  /*
   * Initialize all tables
   */
  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    table_info[i] = table_default;
  }

  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* The function fill the field parameters according to the xml read params 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_params - \n
*      a single field parametes read from xml
*    \param [out] field_info - \n
*      dbal fields info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
static shr_error_e
dbal_db_init_field_add(
  int unit,
  field_db_struct_t * field_params,
  dbal_field_basic_info_t * field_info)
{

  dbal_field_basic_info_t *field_entry;
  dbal_fields_e field_id;
  int label_index;
  char *label_token;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * find the field id according to its name
   */
  SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_params->name, &field_id));
  field_entry = field_info + field_id;

  /*
   * Name - Printing Name
   */
  sal_strcpy(field_entry->name, field_params->name);

  /*
   * Size
   */
  field_entry->max_size = field_params->size;

  /*
   * Type
   */
  SHR_IF_ERR_EXIT(dbal_field_type_string_to_id(unit, field_params->type, &field_entry->type));

  /*
   * Labels
   */
  label_index = 0;
  label_token = sal_strtok(field_params->labels, label_delimiters);
  while (label_token != NULL)
  {
    SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_token, &field_entry->labels[label_index]));
    if (field_entry->labels[label_index] == DBAL_LABEL_ALL_LABELS)
    {
      field_entry->labels[0] = DBAL_LABEL_ALL_LABELS;
      for (label_index = 1; label_index < DBAL_NOF_LABEL_TYPES; label_index++)
      {
        field_entry->labels[label_index] = DBAL_LABEL_NONE;
      }
      break;
    }
    else
    {
      label_index++;
      label_token = sal_strtok(NULL, label_delimiters);
    }
  }

  /*
   * Parent
   */
  if (field_params->parent[0] != '\0')
  {
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_params->parent, &field_entry->parent_field_id));
  }

  /*
   * Default Value
   */
  field_entry->default_value = field_params->default_val;
  field_entry->is_default_value_valid = field_params->default_val_valid;

  /*
   * Encoding
   */
  if (field_params->encode_type[0] != '\0')
  {
    SHR_IF_ERR_EXIT(dbal_encode_type_string_to_id(unit, field_params->encode_type, &field_entry->encode_type));
  }
  field_entry->value_input_param1 = field_params->encode_param1;
  field_entry->value_input_param2 = field_params->encode_param2;

  switch (field_entry->encode_type)
  {
    case DBAL_VALUE_ENCODE_BOOL:
      field_entry->field_32_encoding_func = dbal_fields_encode_bool;
      field_entry->field_32_decoding_func = dbal_fields_decode_bool;
      break;
    case DBAL_VALUE_ENCODE_PREFIX:
      field_entry->field_32_encoding_func = dbal_fields_encode_prefix;
      field_entry->field_32_decoding_func = dbal_fields_decode_prefix;
      break;
    case DBAL_VALUE_ENCODE_SUFFIX:
      field_entry->field_32_encoding_func = dbal_fields_encode_suffix;
      field_entry->field_32_decoding_func = dbal_fields_decode_suffix;
      break;
    case DBAL_VALUE_ENCODE_SUBTRACT:
      field_entry->field_32_encoding_func = dbal_fields_encode_subtract;
      field_entry->field_32_decoding_func = dbal_fields_decode_subtract;
      break;
    case DBAL_VALUE_ENCODE_MODULO:
    case DBAL_VALUE_ENCODE_DIVIDE:
    case DBAL_VALUE_ENCODE_MULTIPLE:
    case DBAL_VALUE_ENCODE_PARENT_FIELD:
    case DBAL_VALUE_ENCODE_PARTIAL_KEY:
    case DBAL_VALUE_ENCODE_NONE:
    case DBAL_NOF_VALUE_ENCODE_TYPES:
    case DBAL_VALUE_ENCODE_HARD_VALUE:

      field_entry->field_32_encoding_func = NULL;
      field_entry->field_32_decoding_func = NULL;
      break;
  }

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* The function fill the mdb table parameters according to the xml read params 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_params - \n
*      a single logical table parametes read from xml
*    \param [out] table_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
static shr_error_e
dbal_db_init_mdb_logical_table_add(
  int unit,
  table_db_struct_t * table_params,
  dbal_logical_table_t * table_info)
{

  dbal_logical_table_t *table_entry;
  dbal_tables_e table_id;
  dbal_fields_e field_id;
  int i, label_index;
  char *label_token;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * find the field id according to its name
   */
  SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_params->name, &table_id));

  table_entry = table_info + table_id;

  /*
   * Name - Printing Name
   */
  sal_strcpy(table_entry->table_name, table_params->name);

  /*
   * Labels
   */
  label_index = 0;
  label_token = sal_strtok(table_params->labels, label_delimiters);
  while (label_token != NULL)
  {
    SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_token, &table_entry->labels[label_index]));
    if (table_entry->labels[label_index] == DBAL_LABEL_ALL_LABELS)
    {
      table_entry->labels[0] = DBAL_LABEL_ALL_LABELS;
      for (label_index = 1; label_index < DBAL_NOF_LABEL_TYPES; label_index++)
      {
        table_entry->labels[label_index] = DBAL_LABEL_NONE;
      }
      break;
    }
    else
    {
      label_index++;
      label_token = sal_strtok(NULL, label_delimiters);
    }
  }

  /*
   * Core Mode
   */
  SHR_IF_ERR_EXIT(dbal_core_mode_string_to_id(unit, table_params->core_mode, &table_entry->core_mode));

  /*
   * Key
   */
  for (i = 0; i < table_params->nof_key_fields; i++)
  {
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params->key_fields[i].name, &field_id));
    table_entry->fields_info[i].field_id = field_id;
    table_entry->fields_info[i].field_nof_bits = table_params->key_fields[i].size;
    table_entry->fields_info[i].is_key = TRUE;
  }

  /*
   * Result
   */
  for (i = 0; i < table_params->nof_res_fields; i++)
  {
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params->result_fields[i].name, &field_id));
    table_entry->fields_info[i + table_params->nof_key_fields].field_id = field_id;
    table_entry->fields_info[i + table_params->nof_key_fields].field_nof_bits = table_params->result_fields[i].size;
    table_entry->fields_info[i + table_params->nof_key_fields].is_key = FALSE;
  }

  /*
   * Total Num of Fields
   */
  table_entry->num_of_fields = table_params->nof_key_fields + table_params->nof_res_fields;

  /*
   * phy DB 
   */
  table_entry->access_type = DBAL_ACCESS_PHY_TABLE;
  table_entry->app_id = table_params->app_db_id;
  SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, table_params->phy_db, &table_entry->physical_db_id));

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* The function fill the hard logic table parameters according to the xml read params 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_params - \n
*      a single logical table parametes read from xml
*    \param [out] table_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
static shr_error_e
dbal_db_init_hard_logic_logical_table_add(
  int unit,
  table_db_struct_t * table_params,
  dbal_logical_table_t * table_info)
{

  dbal_logical_table_t *table_entry;
  dbal_tables_e table_id;
  dbal_fields_e field_id;
  dbal_direct_l2p_field_info_t *access_entry;
  int i, label_index;
  char *label_token;

  dbal_direct_access_types_e access_type;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * find the field id according to its name
   */
  SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_params->name, &table_id));
  table_entry = table_info + table_id;

  /*
   * Name - Printing Name
   */
  sal_strcpy(table_entry->table_name, table_params->name);

  /*
   * Labels
   */
  label_index = 0;
  label_token = sal_strtok(table_params->labels, label_delimiters);
  while (label_token != NULL)
  {
    SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_token, &table_entry->labels[label_index]));
    if (table_entry->labels[label_index] == DBAL_LABEL_ALL_LABELS)
    {
      table_entry->labels[0] = DBAL_LABEL_ALL_LABELS;
      for (label_index = 1; label_index < DBAL_NOF_LABEL_TYPES; label_index++)
      {
        table_entry->labels[label_index] = DBAL_LABEL_NONE;
      }
      break;
    }
    else
    {
      label_index++;
      label_token = sal_strtok(NULL, label_delimiters);
    }
  }

  /*
   * Core Mode
   */
  SHR_IF_ERR_EXIT(dbal_core_mode_string_to_id(unit, table_params->core_mode, &table_entry->core_mode));

  /*
   * Key
   */
  for (i = 0; i < table_params->nof_key_fields; i++)
  {
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params->key_fields[i].name, &field_id));
    table_entry->fields_info[i].field_id = field_id;
    table_entry->fields_info[i].field_nof_bits = table_params->key_fields[i].size;
    table_entry->fields_info[i].is_key = TRUE;
  }

  /*
   * Result
   */
  for (i = 0; i < table_params->nof_res_fields; i++)
  {
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params->result_fields[i].name, &field_id));
    table_entry->fields_info[i + table_params->nof_key_fields].field_id = field_id;
    table_entry->fields_info[i + table_params->nof_key_fields].field_nof_bits = table_params->result_fields[i].size;
    table_entry->fields_info[i + table_params->nof_key_fields].is_key = FALSE;
  }

  /*
   * Total Num of Fields
   */
  table_entry->num_of_fields = table_params->nof_key_fields + table_params->nof_res_fields;

  /*
   * Exclusive properties of hard_logic tables 
   */
  table_entry->access_type = DBAL_ACCESS_DIRECT;

  /*
   * access layer
   */
  for (i = 0; i < table_params->nof_access; i++)
  {
    /*
     * access type (memory or register)
     */
    if (table_params->access[i].access_is_memory)
    {
      access_type = DBAL_DIRECT_ACCESS_MEMORY;
    }
    else
    {
      access_type = DBAL_DIRECT_ACCESS_REGISTER;
    }

    /*
     * In case of None memory, it means that the access block is missing 
     */
    if (sal_strcmp(table_params->access[i].access_name, "NONE") == 0)
    {
      continue;
    }

    access_entry =
      table_entry->l2p_direct_info[access_type].l2p_fields_info +
      table_entry->l2p_direct_info[access_type].num_of_access_fields;
    table_entry->l2p_direct_info[access_type].num_of_access_fields++;

    /*
     * the associated dbal field
     */
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params->access[i].access_field_name, &access_entry->field_id));

    access_entry->nof_bits = table_params->access[i].access_size;
    access_entry->offset = table_params->access[i].access_offset;

    /*
     * HW parameters
     */
    if (access_type == DBAL_DIRECT_ACCESS_MEMORY)
    {
    SHR_IF_ERR_EXIT(dbal_hw_entity_string_to_id(unit, table_params->access[i].access_name, &access_entry->memory))}
    else
    {
    SHR_IF_ERR_EXIT(dbal_hw_entity_string_to_id(unit, table_params->access[i].access_name, &access_entry->reg[0]))}
    if (table_params->access[i].hw_field[0] != '\0')
    {
    SHR_IF_ERR_EXIT(dbal_hw_entity_string_to_id(unit, table_params->access[i].hw_field, &access_entry->hw_field))}

    /*
     * condition
     */
    if (table_params->access[i].access_condition_type[0] != '\0')
    {
      SHR_IF_ERR_EXIT(dbal_condition_string_to_id
                      (unit, table_params->access[i].access_condition_type, &access_entry->condition_mode));
    }
    access_entry->condition_input_param = table_params->access[i].access_condition_value;

    /*
     * offsets (Array, Entry,Data)
     */
    if (table_params->access[i].array_offset_type[0] != '\0')
    {
      SHR_IF_ERR_EXIT(dbal_encode_type_string_to_id
                      (unit, table_params->access[i].array_offset_type,
                       &access_entry->mem_array_offset_info.encode_mode));

      access_entry->mem_array_offset_info.input_param = table_params->access[i].array_offset_value;

      if (table_params->access[i].array_offset_field[0] != '\0')
      {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id
                        (unit, table_params->access[i].array_offset_field,
                         &access_entry->mem_array_offset_info.field_id));
      }
    }

    if (table_params->access[i].entry_offset_type[0] != '\0')
    {
      SHR_IF_ERR_EXIT(dbal_encode_type_string_to_id
                      (unit, table_params->access[i].entry_offset_type, &access_entry->memory_offset_info.encode_mode));

      access_entry->memory_offset_info.input_param = table_params->access[i].entry_offset_value;

      if (table_params->access[i].entry_offset_field[0] != '\0')
      {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id
                        (unit, table_params->access[i].entry_offset_field, &access_entry->memory_offset_info.field_id));
      }
    }

    if (table_params->access[i].data_offset_type[0] != '\0')
    {
      SHR_IF_ERR_EXIT(dbal_encode_type_string_to_id
                      (unit, table_params->access[i].data_offset_type, &access_entry->field_offset_info.encode_mode));

      access_entry->field_offset_info.input_param = table_params->access[i].data_offset_value;

      if (table_params->access[i].data_offset_field[0] != '\0')
      {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id
                        (unit, table_params->access[i].data_offset_field, &access_entry->field_offset_info.field_id));
      }
    }
  }
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_fields(
  int unit,
  int flags,
  dbal_field_basic_info_t * field_info)
{

  int res;
  void *curTop, *cur;

  field_db_struct_t cur_field_param;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * Read xml DB
   */
  if ((flags & DBAL_INIT_FLAGS_VALIDATION) == 0)
  {
    if ((curTop = dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_fields_definition", "FieldDbCatalog", 0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal fields xml file\n");
    }
  }
  else
  {
    if ((curTop =
         dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_fields_validation_test", "FieldDbCatalog", 0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal fields xml file\n");
    }
  }

  /*
   * Run all over XML fields and add it to the fields table
   */
  RHDATA_ITERATOR(cur, curTop, "Field")
  {
    sal_memset(&cur_field_param, 0x0, sizeof(field_db_struct_t));

    RHDATA_GET_STR_STOP(cur, "Name", cur_field_param.name);
    RHDATA_GET_INT_STOP(cur, "Size", cur_field_param.size);
    RHDATA_GET_STR_STOP(cur, "Type", cur_field_param.type);
    RHCONTENT_GET_STR_STOP(cur, "FieldLabels", cur_field_param.labels);

    RHCHDATA_GET_STR_DEF(cur, "Parent", "Name", cur_field_param.parent, "");

    if (dbx_xml_child_get_content_int(cur, "DefaultValue", &cur_field_param.default_val) != NULL)
    {
      cur_field_param.default_val_valid = TRUE;
    }

    RHCHDATA_GET_STR_DEF(cur, "Encoding", "EncodeType", cur_field_param.encode_type, "");
    RHCHDATA_GET_INT_DEF(cur, "Encoding", "Param1", cur_field_param.encode_param1, 0);
    RHCHDATA_GET_INT_DEF(cur, "Encoding", "Param2", cur_field_param.encode_param2, 0);

    SHR_IF_ERR_EXIT(dbal_db_init_field_add(unit, &cur_field_param, field_info));
  }

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* The function fill the mdb table parameters according to the xml read params 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] interface_node - \n
*      node pointer to interface read from xml
*    \param [in] table_params - \n
*      a single logical table parametes parsed from xml
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*  \par INDIRECT OUTPUT:
*    \param [out] table_param - \n
*      a single logical table parametes parsed from xml
*****************************************************/
static shr_error_e
dbal_db_init_logical_table_read_interface(
  int unit,
  void *interface_node,
  table_db_struct_t * table_param)
{
  int res, counter;
  void *curKey, *curResult, *curSub;

  SHR_FUNC_INIT_VARS(unit);

  RHDATA_GET_STR_STOP(interface_node, "Type", table_param->type);

  counter = 0;
  curKey = dbx_xml_child_get_first(interface_node, "Key");
  RHDATA_ITERATOR(curSub, curKey, "Field")
  {
    RHDATA_GET_STR_STOP(curSub, "Name", table_param->key_fields[counter].name);
    RHDATA_GET_INT_DEF(curSub, "Size", table_param->key_fields[counter].size, 0);
    RHDATA_GET_INT_DEF(curSub, "Offset", table_param->key_fields[counter].offset, 0);
    counter++;
  }
  table_param->nof_key_fields = counter;

  counter = 0;
  curResult = dbx_xml_child_get_first(interface_node, "Result");
  RHDATA_ITERATOR(curSub, curResult, "Field")
  {
    RHDATA_GET_STR_STOP(curSub, "Name", table_param->result_fields[counter].name);
    RHDATA_GET_INT_DEF(curSub, "Size", table_param->result_fields[counter].size, 0);
    RHDATA_GET_INT_DEF(curSub, "Offset", table_param->result_fields[counter].offset, 0);
    counter++;
  }
  table_param->nof_res_fields = counter;

  RHCONTENT_GET_STR_STOP(interface_node, "TableLabels", table_param->labels);

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* The function fill the mdb table parameters according to the xml read params 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_node - \n
*      node pointer to interface read from xml
*    \param [in] access_params - \n
*      a single logical table access parametes parsed from xml
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*  \par INDIRECT OUTPUT:
*    \param [out] table_param - \n
*      a single logical table parametes parsed from xml
*****************************************************/
static shr_error_e
dbal_db_init_logical_table_read_access_field(
  int unit,
  void *field_node,
  table_db_access_params_struct_t * access_params)
{
  int res;
  void *memory_node;

  SHR_FUNC_INIT_VARS(unit);

  RHDATA_GET_STR_STOP(field_node, "Name", access_params->access_field_name);

  RHDATA_GET_INT_DEF(field_node, "Size", access_params->access_size, 0);
  RHDATA_GET_INT_DEF(field_node, "Offset", access_params->access_offset, 0);

  RHCHDATA_GET_STR_DEF(field_node, "Condition", "Type", access_params->access_condition_type, "");
  RHCHDATA_GET_INT_DEF(field_node, "Condition", "Value", access_params->access_condition_value, 0);

  memory_node = dbx_xml_child_get_first(field_node, "Register");
  if (memory_node == NULL)
  {
    memory_node = dbx_xml_child_get_first(field_node, "Memory");
    access_params->access_is_memory = TRUE;
  }

  RHDATA_GET_STR_STOP(memory_node, "Name", access_params->access_name);

  RHCHDATA_GET_STR_DEF(memory_node, "ArrayOffset", "EncodeType", access_params->array_offset_type, "");
  RHCHDATA_GET_STR_DEF(memory_node, "ArrayOffset", "Field", access_params->array_offset_field, "");
  RHCHDATA_GET_INT_DEF(memory_node, "ArrayOffset", "Value", access_params->array_offset_value, 0);

  RHCHDATA_GET_STR_DEF(memory_node, "EntryOffset", "EncodeType", access_params->entry_offset_type, "");
  RHCHDATA_GET_STR_DEF(memory_node, "EntryOffset", "Field", access_params->entry_offset_field, "");
  RHCHDATA_GET_INT_DEF(memory_node, "EntryOffset", "Value", access_params->entry_offset_value, 0);

  RHCHDATA_GET_STR_DEF(memory_node, "DataOffset", "EncodeType", access_params->data_offset_type, "");
  RHCHDATA_GET_STR_DEF(memory_node, "DataOffset", "Field", access_params->data_offset_field, "");
  RHCHDATA_GET_INT_DEF(memory_node, "DataOffset", "Value", access_params->data_offset_value, 0);

  RHCHDATA_GET_STR_DEF(memory_node, "HwField", "Name", access_params->hw_field, "");
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_mdb_logical_tables(
  int unit,
  int flags,
  dbal_logical_table_t * table_info)
{
  int res;
  void *curTop, *cur, *curInterface, *curPhy;
  table_db_struct_t cur_table_param;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * Read xml DB
   */
  if ((flags & DBAL_INIT_FLAGS_VALIDATION) == 0)
  {
    if ((curTop =
         dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_logical_tables_mdb_definition", "AppDbCatalog", 0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal mdb logical tables xml file\n");
    }
  }
  else
  {
    if ((curTop =
         dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_logical_tables_mdb_validation_test", "AppDbCatalog",
                              0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal mdb logical tables xml file\n");
    }
  }

  /*
   * Run all over XML fields and add it to the fields table
   */
  RHDATA_ITERATOR(cur, curTop, "AppDB")
  {
    sal_memset(&cur_table_param, 0x0, sizeof(table_db_struct_t));
    RHDATA_GET_STR_STOP(cur, "Name", cur_table_param.name);

    curInterface = dbx_xml_child_get_first(cur, "AppDbInterface");
    dbal_db_init_logical_table_read_interface(unit, curInterface, &cur_table_param);

    curPhy = dbx_xml_child_get_first(cur, "AppToPhyDbMapping");
    RHCHDATA_GET_STR_STOP(curPhy, "CoreMode", "Mode", cur_table_param.core_mode);
    RHCONTENT_GET_STR_STOP(curPhy, "PhyDb", cur_table_param.phy_db);
    RHCONTENT_GET_INT_STOP(curPhy, "AppDbId", &cur_table_param.app_db_id);

    SHR_IF_ERR_EXIT(dbal_db_init_mdb_logical_table_add(unit, &cur_table_param, table_info));
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_hard_logic_logical_tables(
  int unit,
  int flags,
  dbal_logical_table_t * table_info)
{

  int res, access_counter;
  void *curTop, *cur, *curInterface, *curPhy, *curAccess, *curAccessField;

  table_db_struct_t cur_table_param;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * Read xml DB
   */
  if ((flags & DBAL_INIT_FLAGS_VALIDATION) == 0)
  {
    if ((curTop =
         dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_logical_tables_hard_logic_definition", "AppDbCatalog",
                              0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal hard-logic logical tables xml file\n");
    }
  }
  else
  {
    if ((curTop =
         dbx_file_get_xml_top(SOC_CHIP_STRING(unit), "dbal_logical_tables_hard_logic_validation_test", "AppDbCatalog",
                              0)) == NULL)
    {
      SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. cannot find dbal hard-logic logical tables xml file\n");
    }
  }

  /*
   * Run all over XML fields and add it to the fields table
   */
  RHDATA_ITERATOR(cur, curTop, "AppDB")
  {
    sal_memset(&cur_table_param, 0x0, sizeof(table_db_struct_t));
    RHDATA_GET_STR_STOP(cur, "Name", cur_table_param.name);

    curInterface = dbx_xml_child_get_first(cur, "AppDbInterface");
    dbal_db_init_logical_table_read_interface(unit, curInterface, &cur_table_param);

    curPhy = dbx_xml_child_get_first(cur, "AppToPhyDbMapping");
    RHCHDATA_GET_STR_STOP(curPhy, "CoreMode", "Mode", cur_table_param.core_mode);

    access_counter = 0;
    curAccess = dbx_xml_child_get_first(curPhy, "Access");
    RHDATA_ITERATOR(curAccessField, curAccess, "Field")
    {
      dbal_db_init_logical_table_read_access_field(unit, curAccessField, &cur_table_param.access[access_counter]);
      access_counter++;
    }
    cur_table_param.nof_access = access_counter;
    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_logical_table_add(unit, &cur_table_param, table_info));
  }

exit:
  SHR_FUNC_EXIT;
}
