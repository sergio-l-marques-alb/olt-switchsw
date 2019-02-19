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
 *
 * File:        diag_dnx_dbal_xml_parsing.c
 * Purpose:     List of commands available in DNX mode
 * Requires:
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <bcm_int/dpp/error.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include "diag_dnx_dbal_tests.h"

/*************
 *  EXTERNS  *
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

/*************
 * FUNCTIONS *
 *************/
/*! ****************************************************
* \brief
* set the expected fields data for xml parsing validation test
*
*  \par DIRECT INPUT:
*    \param [in] expected_fields - \n
*      fields DB
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
diag_dbal_set_expected_fields_data(
  dbal_field_basic_info_t * expected_fields)
{
  dbal_field_basic_info_t *field;

  /*
   * IPV4 field 
   */
  field = &expected_fields[DBAL_FIELD_IPV4];
  sal_strcpy(field->name, "IPV4");
  field->max_size = 32;
  field->type = DBAL_FIELD_TYPE_IP;
  field->labels[0] = DBAL_LABEL_ALL_LABELS;

  /*
   * VRF field 
   */
  field = &expected_fields[DBAL_FIELD_VRF];
  sal_strcpy(field->name, "VRF");
  field->max_size = 17;
  field->type = DBAL_FIELD_TYPE_UINT32;
  field->labels[0] = DBAL_LABEL_ALL_LABELS;
  field->encode_type = DBAL_VALUE_ENCODE_MULTIPLE;
  field->value_input_param1 = -8;
  field->value_input_param2 = 12;

  /*
   * IPV6 field 
   */
  field = &expected_fields[DBAL_FIELD_IPV6];
  sal_strcpy(field->name, "IPV6");
  field->max_size = 128;
  field->type = DBAL_FIELD_TYPE_ARRAY8;
  field->labels[0] = DBAL_LABEL_L3;
  field->labels[1] = DBAL_LABEL_MPLS;
  field->default_value = 15;
  field->is_default_value_valid = TRUE;
  field->encode_type = DBAL_VALUE_ENCODE_PREFIX;
  field->value_input_param1 = -1;
  field->value_input_param2 = -2;
  field->field_32_encoding_func = dbal_fields_encode_prefix;
  field->field_32_decoding_func = dbal_fields_decode_prefix;

  /*
   * CORE_ID field 
   */
  field = &expected_fields[DBAL_FIELD_CORE_ID];
  sal_strcpy(field->name, "CORE_ID");
  field->max_size = 1;
  field->type = DBAL_FIELD_TYPE_BOOL;
  field->labels[0] = DBAL_LABEL_ALL_LABELS;
  field->default_value = -1;
  field->is_default_value_valid = TRUE;

  /*
   * FEC field 
   */
  field = &expected_fields[DBAL_FIELD_FEC];
  sal_strcpy(field->name, "FEC");
  field->max_size = 20;
  field->type = DBAL_FIELD_TYPE_UINT32;
  field->labels[0] = DBAL_LABEL_L2;
  field->labels[1] = DBAL_LABEL_L3;
  field->labels[2] = DBAL_LABEL_MPLS;
  field->parent_field_id = DBAL_FIELD_DESTINATION;
  field->encode_type = DBAL_VALUE_ENCODE_PREFIX;
  field->value_input_param1 = 8;
  field->value_input_param2 = 15;
  field->field_32_encoding_func = dbal_fields_encode_prefix;
  field->field_32_decoding_func = dbal_fields_decode_prefix;

  return CMD_OK;
}

/*! ****************************************************
* \brief
* set the expected tables data for xml parsing validation test
*
*  \par DIRECT INPUT:
*    \param [in] expected_tables - \n
*      fields DB
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
diag_dbal_set_expected_tables_data(
  dbal_logical_table_t * expected_tables)
{
  dbal_logical_table_t *table;

  /*
   * MACT table 
   */
  table = &expected_tables[DBAL_TABLE_MACT];
  sal_strcpy(table->table_name, "MACT");
  table->access_type = DBAL_ACCESS_PHY_TABLE;
  table->num_of_fields = 4;
  table->fields_info[0].field_id = DBAL_FIELD_FID;
  table->fields_info[0].field_nof_bits = 15;
  table->fields_info[0].is_key = TRUE;
  table->fields_info[1].field_id = DBAL_FIELD_L2_MAC;
  table->fields_info[1].field_nof_bits = 48;
  table->fields_info[1].is_key = TRUE;
  table->fields_info[2].field_id = DBAL_FIELD_DESTINATION;
  table->fields_info[2].field_nof_bits = 21;
  table->fields_info[2].is_key = FALSE;
  table->fields_info[3].field_id = DBAL_FIELD_OUT_LIF;
  table->fields_info[3].field_nof_bits = 18;
  table->fields_info[3].is_key = FALSE;
  table->labels[0] = DBAL_LABEL_ALL_LABELS;
  table->core_mode = DBAL_CORE_ALL;
  table->physical_db_id = DBAL_PHYSICAL_TABLE_LEM;
  table->app_id = 15;

  /*
   * ING_VSI_INFO table 
   */
  table = &expected_tables[DBAL_TABLE_ING_VSI_INFO];
  sal_strcpy(table->table_name, "ING_VSI_INFO");
  table->access_type = DBAL_ACCESS_PHY_TABLE;
  table->num_of_fields = 4;
  table->fields_info[0].field_id = DBAL_FIELD_VSI;
  table->fields_info[0].field_nof_bits = 17;
  table->fields_info[0].is_key = TRUE;
  table->fields_info[1].field_id = DBAL_FIELD_STP_TOPOLOGY_ID;
  table->fields_info[1].field_nof_bits = 8;
  table->fields_info[1].is_key = FALSE;
  table->fields_info[2].field_id = DBAL_FIELD_MY_MAC;
  table->fields_info[2].field_nof_bits = 10;
  table->fields_info[2].is_key = FALSE;
  table->fields_info[3].field_id = DBAL_FIELD_MY_MAC_PREFIX;
  table->fields_info[3].field_nof_bits = 6;
  table->fields_info[3].is_key = FALSE;
  table->labels[0] = DBAL_LABEL_MPLS;
  table->labels[1] = DBAL_LABEL_FCOE;
  table->core_mode = DBAL_CORE_BY_INPUT;
  table->physical_db_id = DBAL_PHYSICAL_TABLE_IVSI;
  table->app_id = -1;

  /*
   * INGRESS_VLAN_PBMP table 
   */
  table = &expected_tables[DBAL_TABLE_INGRESS_VLAN_PBMP];
  sal_strcpy(table->table_name, "INGRESS_VLAN_PBMP");
  table->access_type = DBAL_ACCESS_DIRECT;
  table->num_of_fields = 3;
  table->fields_info[0].field_id = DBAL_FIELD_VLAN_ID;
  table->fields_info[0].field_nof_bits = 12;
  table->fields_info[0].is_key = TRUE;
  table->fields_info[1].field_id = DBAL_FIELD_CORE_ID;
  table->fields_info[1].field_nof_bits = 1;
  table->fields_info[1].is_key = TRUE;
  table->fields_info[2].field_id = DBAL_FIELD_PP_PORT_MEMBER;
  table->fields_info[2].field_nof_bits = 256;
  table->fields_info[2].is_key = FALSE;
  table->labels[0] = DBAL_LABEL_ALL_LABELS;
  table->core_mode = DBAL_CORE_BY_INPUT;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].num_of_access_fields = 2;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].field_id = DBAL_FIELD_PP_PORT_MEMBER;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].nof_bits = 128;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].memory = IPPA_VSI_PORT_MEMBERSHIPm;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].hw_field = VLAN_PORT_MEMBER_LINEf;

  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].field_id = DBAL_FIELD_PP_PORT_MEMBER;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].nof_bits = 128;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].offset = 128;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory = IPPA_VSI_PORT_MEMBERSHIPm;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].hw_field = VLAN_PORT_MEMBER_LINEf;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].mem_array_offset_info.encode_mode =
    DBAL_VALUE_ENCODE_HARD_VALUE;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].mem_array_offset_info.input_param = 1;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory_offset_info.encode_mode =
    DBAL_VALUE_ENCODE_DIVIDE;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory_offset_info.field_id = DBAL_FIELD_VRF;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory_offset_info.input_param = 3;

  /*
   * EGRESS_PORT table 
   */
  table = &expected_tables[DBAL_TABLE_EGRESS_PORT];
  sal_strcpy(table->table_name, "EGRESS_PORT");
  table->access_type = DBAL_ACCESS_DIRECT;
  table->num_of_fields = 4;
  table->fields_info[0].field_id = DBAL_FIELD_PP_PORT;
  table->fields_info[0].field_nof_bits = 8;
  table->fields_info[0].is_key = TRUE;
  table->fields_info[1].field_id = DBAL_FIELD_CORE_ID;
  table->fields_info[1].field_nof_bits = 15;
  table->fields_info[1].is_key = TRUE;
  table->fields_info[2].field_id = DBAL_FIELD_VLAN_DOMAIN;
  table->fields_info[2].field_nof_bits = 9;
  table->fields_info[2].is_key = FALSE;
  table->fields_info[3].field_id = DBAL_FIELD_PORT_VID;
  table->fields_info[3].field_nof_bits = 12;
  table->fields_info[3].is_key = FALSE;
  table->labels[0] = DBAL_LABEL_L2;
  table->labels[1] = DBAL_LABEL_L3;
  table->labels[2] = DBAL_LABEL_MPLS;
  table->labels[3] = DBAL_LABEL_FCOE;
  table->core_mode = DBAL_CORE_ALL;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].num_of_access_fields = 3;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].field_id = DBAL_FIELD_VLAN_DOMAIN;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].memory = ETPPA_PER_PORT_TABLEm;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].hw_field = VLAN_DOMAINf;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].condition_mode = DBAL_CONDITION_BIGGER_THAN;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].condition_input_param = 14;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].mem_array_offset_info.encode_mode =
    DBAL_VALUE_ENCODE_MULTIPLE;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].mem_array_offset_info.field_id =
    DBAL_FIELD_CORE_ID;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[0].mem_array_offset_info.input_param = 4;

  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].field_id = DBAL_FIELD_VLAN_DOMAIN;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory = ETPPA_PER_PORT_TABLEm;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].hw_field = VLAN_DOMAINf;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].condition_mode = DBAL_CONDITION_IS_EQUAL_TO;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].condition_input_param = 14;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory_offset_info.encode_mode =
    DBAL_VALUE_ENCODE_HARD_VALUE;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[1].memory_offset_info.input_param = 1;

  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].field_id = DBAL_FIELD_VLAN_DOMAIN;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].memory = ETPPA_PER_PORT_TABLEm;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].condition_mode = DBAL_CONDITION_LOWER_THAN;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].condition_input_param = 14;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].field_offset_info.encode_mode =
    DBAL_VALUE_ENCODE_PARTIAL_KEY;
  table->l2p_direct_info[DBAL_DIRECT_ACCESS_MEMORY].l2p_fields_info[2].field_offset_info.field_id = DBAL_FIELD_PP_PORT;

  return CMD_OK;
}

/*! ****************************************************
* \brief
* compare the expected fields DB and the parsed fields DB
* compare all field's structure elements (field by field)
* called only if memory compare has failed
*
*  \par DIRECT INPUT:
*    \param [in] expected_field - \n
*      fields DB - expected
*    \param [in] parsed_field - \n
*      fields DB - parsed
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
diag_dbal_compare_field_data(
  dbal_field_basic_info_t expected_field,
  dbal_field_basic_info_t parsed_field)
{
  int i;

  if (sal_strcmp(expected_field.name, parsed_field.name) != 0)
  {
    cli_out("Error - Unexpected name. Parsed:%s ,Expected:%s \n", parsed_field.name, expected_field.name);
    return CMD_FAIL;
  }
  if (expected_field.max_size != parsed_field.max_size)
  {
    cli_out("Error - Unexpected max_size. Parsed=%d ,Expected=%d \n", parsed_field.max_size, expected_field.max_size);
    return CMD_FAIL;
  }
  for (i = 0; i < DBAL_NOF_LABEL_TYPES; i++)
  {
    if (expected_field.labels[i] != parsed_field.labels[i])
    {
      cli_out("Error - Unexpected labels[%i]. Parsed=%d ,Expected=%d \n", i, parsed_field.labels[i],
              expected_field.labels[i]);
      return CMD_FAIL;
    }
  }
  if (expected_field.type != parsed_field.type)
  {
    cli_out("Error - Unexpected type. Parsed=%d ,Expected=%d \n", parsed_field.type, expected_field.type);
    return CMD_FAIL;
  }
  if (expected_field.parent_field_id != parsed_field.parent_field_id)
  {
    cli_out("Error - Unexpected parent_field_id. Parsed=%d ,Expected=%d \n", parsed_field.parent_field_id,
            expected_field.parent_field_id);
    return CMD_FAIL;
  }
  if (expected_field.default_value != parsed_field.default_value)
  {
    cli_out("Error - Unexpected default_value. Parsed=%d ,Expected=%d \n", parsed_field.default_value,
            expected_field.default_value);
    return CMD_FAIL;
  }
  if (expected_field.is_default_value_valid != parsed_field.is_default_value_valid)
  {
    cli_out("Error - Unexpected is_default_value_valid. Parsed=%d ,Expected=%d \n", parsed_field.is_default_value_valid,
            expected_field.is_default_value_valid);
    return CMD_FAIL;
  }
  if (expected_field.encode_type != parsed_field.encode_type)
  {
    cli_out("Error - Unexpected encode_type. Parsed=%d ,Expected=%d \n", parsed_field.encode_type,
            expected_field.encode_type);
    return CMD_FAIL;
  }
  if (expected_field.value_input_param1 != parsed_field.value_input_param1)
  {
    cli_out("Error - Unexpected value_input_param1. Parsed=%d ,Expected=%d \n", parsed_field.value_input_param1,
            expected_field.value_input_param1);
    return CMD_FAIL;
  }
  if (expected_field.value_input_param2 != parsed_field.value_input_param2)
  {
    cli_out("Error - Unexpected value_input_param2. Parsed=%d ,Expected=%d \n", parsed_field.value_input_param2,
            expected_field.value_input_param2);
    return CMD_FAIL;
  }
  if (expected_field.field_32_decoding_func != parsed_field.field_32_decoding_func)
  {
    cli_out("Error - Unexpected field_32_decoding_func. Parsed=%p ,Expected=%p \n", parsed_field.field_32_decoding_func,
            expected_field.field_32_decoding_func);
    return CMD_FAIL;
  }
  if (expected_field.field_32_encoding_func != parsed_field.field_32_encoding_func)
  {
    cli_out("Error - Unexpected field_32_encoding_func. Parsed=%p ,Expected=%p \n", parsed_field.field_32_encoding_func,
            expected_field.field_32_encoding_func);
    return CMD_FAIL;
  }

  return CMD_OK;
}

/*! ****************************************************
* \brief
* compare the expected tables DB and the parsed tables DB
* compare all table's structure elements (field by field)
* called only if memory compare has failed
*
*  \par DIRECT INPUT:
*    \param [in] expected_table - \n
*      tables DB - expected
*    \param [in] expected_table - \n
*      tables DB - parsed
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
diag_dbal_compare_table_data(
  dbal_logical_table_t expected_table,
  dbal_logical_table_t parsed_table)
{
  int i, j;
  dbal_direct_l2p_info_t *access_expected, *access_parsed;
  if (sal_strcmp(expected_table.table_name, parsed_table.table_name) != 0)
  {
    cli_out("Error - Unexpected table_name. Parsed:%s ,Expected:%s \n", parsed_table.table_name,
            expected_table.table_name);
    return CMD_FAIL;
  }
  for (i = 0; i < DBAL_NOF_LABEL_TYPES; i++)
  {
    if (expected_table.labels[i] != parsed_table.labels[i])
    {
      cli_out("Error - Unexpected labels[%d]. Parsed=%d ,Expected=%d \n", i, parsed_table.labels[i],
              expected_table.labels[i]);
      return CMD_FAIL;
    }
  }
  if (expected_table.access_type != parsed_table.access_type)
  {
    cli_out("Error - Unexpected access_type. Parsed=%d ,Expected=%d \n", parsed_table.access_type,
            expected_table.access_type);
    return CMD_FAIL;
  }
  if (expected_table.num_of_fields != parsed_table.num_of_fields)
  {
    cli_out("Error - Unexpected num_of_fields. Parsed=%d ,Expected=%d \n", parsed_table.num_of_fields,
            expected_table.num_of_fields);
    return CMD_FAIL;
  }
  for (i = 0; i < DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS; i++)
  {
    if (expected_table.fields_info[i].field_nof_bits != parsed_table.fields_info[i].field_nof_bits)
    {
      cli_out("Error - Unexpected fields_info[%d].field_nof_bits. Parsed=%d ,Expected=%d \n", i,
              parsed_table.fields_info[i].field_nof_bits, expected_table.fields_info[i].field_nof_bits);
      return CMD_FAIL;
    }
    if (expected_table.fields_info[i].field_id != parsed_table.fields_info[i].field_id)
    {
      cli_out("Error - Unexpected fields_info[%d].field_id. Parsed=%d ,Expected=%d \n", i,
              parsed_table.fields_info[i].field_id, expected_table.fields_info[i].field_id);
      return CMD_FAIL;
    }
    if (expected_table.fields_info[i].is_key != parsed_table.fields_info[i].is_key)
    {
      cli_out("Error - Unexpected fields_info[%d].is_key. Parsed=%d ,Expected=%d \n", i,
              parsed_table.fields_info[i].is_key, expected_table.fields_info[i].is_key);
      return CMD_FAIL;
    }
  }
  if (expected_table.core_mode != parsed_table.core_mode)
  {
    cli_out("Error - Unexpected core_mode. Parsed=%d ,Expected=%d \n", parsed_table.core_mode,
            expected_table.core_mode);
    return CMD_FAIL;
  }
  if (expected_table.physical_db_id != parsed_table.physical_db_id)
  {
    cli_out("Error - Unexpected physical_db_id. Parsed=%d ,Expected=%d \n", parsed_table.physical_db_id,
            expected_table.physical_db_id);
    return CMD_FAIL;
  }
  if (expected_table.app_id != parsed_table.app_id)
  {
    cli_out("Error - Unexpected app_id. Parsed=%d ,Expected=%d \n", parsed_table.app_id, expected_table.app_id);
    return CMD_FAIL;
  }

  for (j = 0; j < DBAL_NOF_DIRECT_ACCESS_TYPES; j++)
  {
    access_expected = &expected_table.l2p_direct_info[j];
    access_parsed = &parsed_table.l2p_direct_info[j];

    if (access_expected->num_of_access_fields != access_parsed->num_of_access_fields)
    {
      cli_out("Error - Unexpected access type=%d, nof fields. Parsed=%d ,Expected=%d \n", j,
              access_parsed->num_of_access_fields, access_expected->num_of_access_fields);
      return CMD_FAIL;
    }
    for (i = 0; i < DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS; i++)
    {
      if (access_expected->l2p_fields_info[i].condition_mode != access_parsed->l2p_fields_info[i].condition_mode)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],condition_mode. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].condition_mode, access_expected->l2p_fields_info[i].condition_mode);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].condition_input_param !=
          access_parsed->l2p_fields_info[i].condition_input_param)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],condition_input_param. Parsed=%d ,Expected=%d \n",
                j, i, access_parsed->l2p_fields_info[i].condition_input_param,
                access_expected->l2p_fields_info[i].condition_input_param);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].field_id != access_parsed->l2p_fields_info[i].field_id)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],field_id. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].field_id, access_expected->l2p_fields_info[i].field_id);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].hw_field != access_parsed->l2p_fields_info[i].hw_field)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],hw_field. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].hw_field, access_expected->l2p_fields_info[i].hw_field);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].memory != access_parsed->l2p_fields_info[i].memory)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],memory. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].memory, access_expected->l2p_fields_info[i].memory);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].reg[0] != access_parsed->l2p_fields_info[i].reg[0])
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],reg[0]. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].reg[0], access_expected->l2p_fields_info[i].reg[0]);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].nof_bits != access_parsed->l2p_fields_info[i].nof_bits)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],nof_bits. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].nof_bits, access_expected->l2p_fields_info[i].nof_bits);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].offset != access_parsed->l2p_fields_info[i].offset)
      {
        cli_out("Error - Unexpected access type=%d, field_index[%d],offset. Parsed=%d ,Expected=%d \n", j, i,
                access_parsed->l2p_fields_info[i].offset, access_expected->l2p_fields_info[i].offset);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].mem_array_offset_info.encode_mode !=
          access_parsed->l2p_fields_info[i].mem_array_offset_info.encode_mode)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],mem_array_offset_info.encode_mode. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].mem_array_offset_info.encode_mode,
           access_expected->l2p_fields_info[i].mem_array_offset_info.encode_mode);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].mem_array_offset_info.field_id !=
          access_parsed->l2p_fields_info[i].mem_array_offset_info.field_id)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],mem_array_offset_info.field_id. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].mem_array_offset_info.field_id,
           access_expected->l2p_fields_info[i].mem_array_offset_info.field_id);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].mem_array_offset_info.input_param !=
          access_parsed->l2p_fields_info[i].mem_array_offset_info.input_param)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],mem_array_offset_info.input_param. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].mem_array_offset_info.input_param,
           access_expected->l2p_fields_info[i].mem_array_offset_info.input_param);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].memory_offset_info.encode_mode !=
          access_parsed->l2p_fields_info[i].memory_offset_info.encode_mode)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],memory_offset_info.encode_mode. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].memory_offset_info.encode_mode,
           access_expected->l2p_fields_info[i].memory_offset_info.encode_mode);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].memory_offset_info.field_id !=
          access_parsed->l2p_fields_info[i].memory_offset_info.field_id)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],memory_offset_info.field_id. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].memory_offset_info.field_id,
           access_expected->l2p_fields_info[i].memory_offset_info.field_id);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].memory_offset_info.input_param !=
          access_parsed->l2p_fields_info[i].memory_offset_info.input_param)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],memory_offset_info.input_param. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].memory_offset_info.input_param,
           access_expected->l2p_fields_info[i].memory_offset_info.input_param);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].field_offset_info.encode_mode !=
          access_parsed->l2p_fields_info[i].field_offset_info.encode_mode)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],field_offset_info.encode_mode. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].field_offset_info.encode_mode,
           access_expected->l2p_fields_info[i].field_offset_info.encode_mode);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].field_offset_info.field_id !=
          access_parsed->l2p_fields_info[i].field_offset_info.field_id)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],field_offset_info.field_id. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].field_offset_info.field_id,
           access_expected->l2p_fields_info[i].field_offset_info.field_id);
        return CMD_FAIL;
      }
      if (access_expected->l2p_fields_info[i].field_offset_info.input_param !=
          access_parsed->l2p_fields_info[i].field_offset_info.input_param)
      {
        cli_out
          ("Error - Unexpected access type=%d, field_index[%d],field_offset_info.input_param. Parsed=%d ,Expected=%d \n",
           j, i, access_parsed->l2p_fields_info[i].field_offset_info.input_param,
           access_expected->l2p_fields_info[i].field_offset_info.input_param);
        return CMD_FAIL;
      }
    }
  }
  return CMD_OK;
}

cmd_result_t
diag_dbal_test_xml_parsing(
  int unit)
{
  int i;
  int res = CMD_OK;

  dbal_field_basic_info_t fields_data[DBAL_NOF_FIELDS];
  dbal_field_basic_info_t fields_parsed_info[DBAL_NOF_FIELDS];

  dbal_logical_tables_info_t tables_data;
  dbal_logical_tables_info_t tables_parsed_info;

  /*
   * Fields - setting hard-coded values
   */
  res = dbal_db_init_fields_set_default(unit, fields_data);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_fields_set_default, res=%d\n", res);
    return res;
  }

  res = diag_dbal_set_expected_fields_data(fields_data);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_set_expected_fields_data, res=%d\n", res);
    return res;
  }

  /*
   * Fields reading
   */
  res = dbal_db_init_fields_set_default(unit, fields_parsed_info);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_fields_set_default, res=%d\n", res);
    return res;
  }

  res = dbal_db_init_fields(unit, DBAL_INIT_FLAGS_VALIDATION, fields_parsed_info);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_fields, res=%d\n", res);
    return res;
  }

  /*
   * Fields comparison
   */
  for (i = 0; i < DBAL_NOF_FIELDS; i++)
  {
    if (sal_memcmp(&fields_data[i], &fields_parsed_info[i], sizeof(dbal_field_basic_info_t)) != 0)
    {
      res = diag_dbal_compare_field_data(fields_data[i], fields_parsed_info[i]);
      if (res != CMD_OK)
      {
        cli_out("Error. dbal_db_compare_field_data, field=%d, res=%d\n", i, res);
        return res;
      }
      else
      {
        cli_out("Error. cmd_dbal_xml_parsing_validation sal_memcmp failed but fields are identical, field=%d, \n", i);
        return CMD_FAIL;
      }
    }
  }
  cli_out("XML Parsing Validation: Fields - PASSED\n");

  /*
   * Tables - setting hard-coded values
   */
  res = dbal_db_init_logical_tables_set_default(unit, tables_data.logical_tables);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_logical_tables_set_default, res=%d\n", res);
    return res;
  }

  res = diag_dbal_set_expected_tables_data(tables_data.logical_tables);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_set_expected_tables_data, res=%d\n", res);
    return res;
  }

  /*
   * Tables reading
   */
  res = dbal_db_init_logical_tables_set_default(unit, tables_parsed_info.logical_tables);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_logical_tables_set_default, res=%d\n", res);
    return res;
  }

  res = dbal_db_init_mdb_logical_tables(unit, DBAL_INIT_FLAGS_VALIDATION, tables_parsed_info.logical_tables);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_mdb_logical_tables, res=%d\n", res);
    return res;
  }

  res = dbal_db_init_hard_logic_logical_tables(unit, DBAL_INIT_FLAGS_VALIDATION, tables_parsed_info.logical_tables);
  if (res != CMD_OK)
  {
    cli_out("Error. dbal_db_init_hard_logic_logical_tables, res=%d\n", res);
    return res;
  }

  /*
   * Tables comparison
   */
  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    if (sal_memcmp(&tables_data.logical_tables[i], &tables_parsed_info.logical_tables[i], sizeof(dbal_logical_table_t))
        != 0)
    {
      res = diag_dbal_compare_table_data(tables_data.logical_tables[i], tables_parsed_info.logical_tables[i]);
      if (res != CMD_OK)
      {
        cli_out("Error. dbal_db_compare_table_data, table=%d, res=%d\n", i, res);
        return res;
      }
      else
      {
        cli_out("Error. cmd_dbal_xml_parsing_validation sal_memcmp failed but tables are identical, table=%d, \n", i);
        return CMD_FAIL;
      }
    }
  }
  cli_out("XML Parsing Validation: Tables - PASSED\n");

  return res;
}
