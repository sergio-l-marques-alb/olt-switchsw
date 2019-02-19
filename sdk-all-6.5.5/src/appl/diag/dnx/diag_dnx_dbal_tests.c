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

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <bcm_int/dpp/error.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <bcm/l2.h>
#include "diag_dnx_dbal_tests.h"

/*************
 *  DEFINES  *
 *************/
#define DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE       (20)
#define DIAG_DBAL_MAX_NOF_CHILED_FIELD_PER_PARENT (4)
#define DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS      (DIAG_DBAL_MAX_NOF_CHILED_FIELD_PER_PARENT*DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS)

/*************
 *   MACROS  *
 *************/
/*
 *  setting the maximum value for N bits field
 *  UINT/INT support
 */
#define DIAG_DBAL_MAX_FIELD_VAL(field_arr,field_len,field_type)       \
{                                                                     \
  int iter_num = (field_len-1)/32;                                    \
  int residue = field_len%32;                                         \
  int iter = 0;                                                       \
  for(iter=0;iter<iter_num;iter++)                                    \
  {                                                                   \
    field_arr[iter] = 0xFFFFFFFF;                                     \
  }                                                                   \
  if(field_type == DBAL_FIELD_TYPE_INT32)                             \
  {                                                                   \
    if(residue == 0)                                                  \
      field_arr[iter_num] = 0x7FFFFFFF;                               \
    else                                                              \
      field_arr[iter_num] = ((1<<(residue-1))-1);                     \
  }                                                                   \
  else                                                                \
  {                                                                   \
    field_arr[iter_num] = 0xFFFFFFFF;                                 \
  }                                                                   \
}

/*
 *  setting the minimum value for N bits field
 *  UINT/INT support
 */
#define DIAG_DBAL_MIN_FIELD_VAL(field_arr,field_len,field_type)       \
{                                                                     \
  int iter_num = (field_len-1)/32;                                    \
  int residue = field_len%32;                                         \
  int iter = 0;                                                       \
  for(iter=0;iter<iter_num;iter++)                                    \
  {                                                                   \
    field_arr[iter] = 0x0;                                            \
  }                                                                   \
  if(field_type == DBAL_FIELD_TYPE_INT32)                             \
  {                                                                   \
    if(residue == 0)                                                  \
      field_arr[iter_num] = 0x10000000;                               \
    else                                                              \
      field_arr[iter_num] = (1<<(residue-1));                         \
  }                                                                   \
  else                                                                \
  {                                                                   \
    field_arr[iter_num] = 0x0;                                        \
  }                                                                   \
}

/*
 *  setting random value for N bits field
 */
#define DIAG_DBAL_RANDOM_FIELD_VAL(field_arr,field_len)               \
{                                                                     \
  int iter_num = (field_len+31)/32;                                   \
  int iter = 0;                                                       \
  for(iter=0;iter<iter_num;iter++)                                    \
  {                                                                   \
    field_arr[iter] = (sal_rand() + ((sal_rand()&0x4000)<<1))         \
                    + ((sal_rand() + ((sal_rand()&0x4000)<<1))<<16);  \
  }                                                                   \
}

/*************
 * TYPEDEFS  *
 *************/
typedef struct
  {
    int nof_fields;
    int nof_combinations;
    int fields_id[DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS][DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    int fields_is_key[DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS][DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    int fields_bit_length[DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS][DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
  } diag_dbal_ltt_fields_to_check;

/*************
 * FUNCTIONS *
 *************/
/*! ****************************************************
* \brief
* check if all fields of the table have valid physical mapping
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table
*  \par DIRECT OUTPUT:
*    uint8 - \n
*      TRUE - all access are valid, FALSE - table misses physical mapping
*****************************************************/
static uint8
diag_dbal_table_is_access_valid(
  int unit,
  dbal_logical_table_t * table)
{
  int i;
  int nof_access = 0;

  for (i = 0; i < DBAL_NOF_DIRECT_ACCESS_TYPES; i++)
  {
    nof_access += table->l2p_direct_info[i].num_of_access_fields;
  }
  if (nof_access > 0)
  {
    return TRUE;
  }
  return FALSE;
}

/*! ****************************************************
* \brief
* compare two fields value, fields represented as array32. 
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table
*    \param [in] field_val - \n
*       expected values
*    \param [in] field_val_get - \n
*       retrieved values
*  \par DIRECT OUTPUT:
*    int - \n
*      0 - good, (-1) - bad
*****************************************************/
static int
compare_fields_values(
  int unit,
  dbal_logical_table_t * table,
  uint32 field_val[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE / 4],
  uint32 field_val_get[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE / 4])
{
  int i, j;
  uint32 bit, bit_get;

  for (i = 0; i < table->num_of_fields; i++)
  {
    for (j = 0; j < table->fields_info[i].field_nof_bits; j++)
    {
      bit = (field_val[i][j / 32] >> (j % 32)) & 0x1;
      bit_get = (field_val_get[i][j / 32] >> (j % 32)) & 0x1;
      if (bit != bit_get)
      {
        cli_out("Comparison error: field=%s, bit=%d, expect=%d, get=%d\n",
                dbal_field_to_string(unit, table->fields_info[i].field_id), j, bit, bit_get);
        return -1;
      }
    }
  }
  return 0;
}

/*! ****************************************************
* \brief
* print out all table's field possibole combinations
*  
*  \par DIRECT INPUT:
*    \param [in] field_combo - \n
*       combination structure
*****************************************************/
static void
print_fields_combinations(
  int unit,
  diag_dbal_ltt_fields_to_check * field_combo)
{
  int i, j;
  for (i = 0; i < field_combo->nof_combinations; i++)
  {
    cli_out("%d)\t", i);
    for (j = 0; j < field_combo->nof_fields; j++)
    {
      cli_out("field[%d]=%s ", j, dbal_field_to_string(unit, field_combo->fields_id[i][j]));
    }
    cli_out("\n");
  }
}

/*! ****************************************************
* \brief
* adding new list of fields to combinations structure. 
* this function will be called when a parent field is in the table, and will add its child fields 
*  
*  \par DIRECT INPUT:
*    \param [in] field_combo - \n
*       combination structure
*    \param [in] child_fields - \n
*       list of field to add
*    \param [in] nof_chiled_fields - \n
*       Num. of fields to add
*    \param [in] length - \n
*       field len in table
*    \param [in] is_key - \n
*       key indication of field in table
*****************************************************/
static void
diag_dbal_test_add_child_fields_to_combinations(
  diag_dbal_ltt_fields_to_check * field_combo,
  dbal_fields_e child_fields[DIAG_DBAL_MAX_NOF_CHILED_FIELD_PER_PARENT],
  int nof_chiled_fields,
  uint32 length,
  uint8 is_key)
{
  int i, j;
  int nof_combos;

  nof_combos = field_combo->nof_combinations;
  for (i = 0; i < nof_chiled_fields; i++)
  {
    for (j = 0; j < nof_combos; j++)
    {
      sal_memcpy(&field_combo->fields_id[i * nof_combos + j][0], &field_combo->fields_id[j][0],
                 sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS);
      sal_memcpy(&field_combo->fields_is_key[i * nof_combos + j][0], &field_combo->fields_is_key[j][0],
                 sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS);
      sal_memcpy(&field_combo->fields_bit_length[i * nof_combos + j][0], &field_combo->fields_bit_length[j][0],
                 sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS);
    }
  }

  for (i = 0; i < nof_chiled_fields; i++)
  {
    for (j = 0; j < nof_combos; j++)
    {
      field_combo->fields_id[i * nof_combos + j][field_combo->nof_fields] = child_fields[i];
      field_combo->fields_is_key[i * nof_combos + j][field_combo->nof_fields] = is_key;
      field_combo->fields_bit_length[i * nof_combos + j][field_combo->nof_fields] = length;
    }
  }
  field_combo->nof_fields++;
  field_combo->nof_combinations *= nof_chiled_fields;
}

cmd_result_t
diag_dbal_test_logical_table(
  int unit,
  dbal_tables_e table_id,
  int mode)
{
  int i, j, k;
  int nof_entries;
  uint32 entry_handle;
  dbal_logical_table_t *table;
  uint32 field_val[DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS]
                  [DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS]
                  [DBAL_FIELD_ARRAY_MAX_SIZE / 4] = {{{0}}};
  uint32 field_val_get[DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS]
                      [DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS]
                      [DBAL_FIELD_ARRAY_MAX_SIZE / 4] = {{{0}}};
  dbal_field_type_e field_type;
  diag_dbal_ltt_fields_to_check fields_combination;

  SHR_FUNC_INIT_VARS(unit);

  cli_out("*****diag_dbal_test_logical_table*****\n" "table=%s(%d) ,test_mode=%d\n",
          dbal_logical_table_to_string(unit, table_id), table_id, mode);

  fields_combination.nof_fields = 0;
  fields_combination.nof_combinations = 1;

  sal_srand(sal_time_usecs());

  SHR_IF_ERR_EXIT(dbal_logical_table_get(unit, table_id, &table));

  if (table->access_type != DBAL_ACCESS_DIRECT)
  {
    cli_out("Test is implemented only for hard-logic tables, mdb tables will be added soon...\n");
    return CMD_OK;
  }

  if (!diag_dbal_table_is_access_valid(unit, table))
  {
    cli_out("Table does not have a full access valid implementation\n");
    return CMD_OK;
  }

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, table_id, &entry_handle));

  /*
   * PART 1
   * add, get and compare entry with minimum/maximum field's values
   */
  for (i = 0; i < 2; i++)
  {
    if (i == 0)
    {
      cli_out("Part 1a - minimum values test\n");
    }
    else
    {
      cli_out("Part 1b - maximum values test\n");
    }

    /*
     * set 
     */
    for (j = 0; j < table->num_of_fields; j++)
    {
      dbal_fields_type_get(unit, table->fields_info[j].field_id, &field_type);
      if (i == 0)
      {
        if (table->fields_info[j].is_key)
        {
          DIAG_DBAL_MIN_FIELD_VAL(field_val[0][j], table->fields_info[j].field_nof_bits, field_type);
        }
        else
        {
          DIAG_DBAL_RANDOM_FIELD_VAL(field_val[0][j], table->fields_info[j].field_nof_bits);
        }
      }
      else
      {
        DIAG_DBAL_MAX_FIELD_VAL(field_val[0][j], table->fields_info[j].field_nof_bits, field_type);
      }
      SHR_IF_ERR_EXIT(dbal_entry_array32_set
                      (unit, entry_handle, table->fields_info[j].is_key, table->fields_info[j].field_id,
                       field_val[0][j]));
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT_KEEP_HANDLE));

    /*
     * get 
     */
    for (j = 0; j < table->num_of_fields; j++)
    {
      dbal_fields_type_get(unit, table->fields_info[j].field_id, &field_type);
      if (table->fields_info[j].is_key)
      {
        if (i == 0)
        {
          DIAG_DBAL_MIN_FIELD_VAL(field_val_get[0][j], table->fields_info[j].field_nof_bits, field_type);
        }
        else
        {
          DIAG_DBAL_MAX_FIELD_VAL(field_val_get[0][j], table->fields_info[j].field_nof_bits, field_type);
        }
        SHR_IF_ERR_EXIT(dbal_entry_array32_set
                        (unit, entry_handle, table->fields_info[j].is_key, table->fields_info[j].field_id,
                         field_val_get[0][j]));
      }
      else
      {
        SHR_IF_ERR_EXIT(dbal_entry_array32_get
                        (unit, entry_handle, table->fields_info[j].is_key, table->fields_info[j].field_id,
                         field_val_get[0][j]));
      }
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle, DBAL_COMMIT_KEEP_HANDLE));

    /*
     * compare results 
     */
    SHR_IF_ERR_EXIT(compare_fields_values(unit, table, field_val[0], field_val_get[0]));
  }

  /*
   * PART 2
   * random values set, get, traverse
   */

  /*
   * arrange all fields possible combinations 
   * The combinations are derived from Parent field encoding
   */
  for (j = 0; j < table->num_of_fields; j++)
  {
    dbal_value_encode_types_e encode_type;
    dbal_fields_encode_type_get(unit, table->fields_info[j].field_id, &encode_type);
    if (encode_type == DBAL_VALUE_ENCODE_PARENT_FIELD)
    {
      dbal_fields_e parent_field;
      int nof_childs = 0;
      dbal_fields_e child_fields[DIAG_DBAL_MAX_NOF_CHILED_FIELD_PER_PARENT];

      for (i = 0; i < DBAL_NOF_FIELDS; i++)
      {
        dbal_fields_parent_field_id_get(unit, i, &parent_field);
        if (parent_field == table->fields_info[j].field_id)
        {
          child_fields[nof_childs] = i;
          nof_childs++;
        }
      }
      diag_dbal_test_add_child_fields_to_combinations(&fields_combination, child_fields, nof_childs,
                                                      table->fields_info[j].field_nof_bits,
                                                      table->fields_info[j].is_key);
    }
    else
    {
      for (i = 0; i < fields_combination.nof_combinations; i++)
      {
        fields_combination.fields_id[i][fields_combination.nof_fields] = table->fields_info[j].field_id;
        fields_combination.fields_is_key[i][fields_combination.nof_fields] = table->fields_info[j].is_key;
        fields_combination.fields_bit_length[i][fields_combination.nof_fields] = table->fields_info[j].field_nof_bits;
      }
      fields_combination.nof_fields++;
    }
    if (fields_combination.nof_combinations > DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS)
    {
      SHR_IF_ERR_EXIT(dbal_entry_handle_release(unit, entry_handle));
      SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. table:%s(%d) exceeds the max num of fields combinations (%d)\n",
                   dbal_logical_table_to_string(unit, table_id), table_id, DIAG_DBAL_MAX_NOF_FIELD_COMBINATIONS);
    }
  }
  if (fields_combination.nof_combinations > 1)
  {
    cli_out("Checking %d fields combinations:\n", fields_combination.nof_combinations);
    print_fields_combinations(unit, &fields_combination);
  }

  /*
   * set nof_entries random values entries 
   */
  nof_entries = (fields_combination.nof_combinations > DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE) ? 
                fields_combination.nof_combinations :
				DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE;

  for (i = 0; i < nof_entries; i++)
  {
    int comb_index = i % fields_combination.nof_combinations;
    for (j = 0; j < fields_combination.nof_fields; j++)
    {
      DIAG_DBAL_RANDOM_FIELD_VAL(field_val[i][j], fields_combination.fields_bit_length[comb_index][j]);
      SHR_IF_ERR_EXIT(dbal_entry_array32_set
                      (unit, entry_handle, fields_combination.fields_is_key[comb_index][j],
                       fields_combination.fields_id[comb_index][j], field_val[i][j]));
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT_KEEP_HANDLE));
  }

  for (i = 0; i < nof_entries; i++)
  {
    /*
     * get 
     */
    int comb_index = i % fields_combination.nof_combinations;
    int nof_vals_to_copy;
    for (j = 0; j < fields_combination.nof_fields; j++)
    {
      if (fields_combination.fields_is_key[comb_index][j])
      {
        nof_vals_to_copy = (fields_combination.fields_bit_length[comb_index][j] + 31) / 32;
        for (k = 0; k < nof_vals_to_copy; k++)
        {
          field_val_get[i][j][k] = field_val[i][j][k];
        }
        SHR_IF_ERR_EXIT(dbal_entry_array32_set
                        (unit, entry_handle, fields_combination.fields_is_key[comb_index][j],
                         fields_combination.fields_id[comb_index][j], field_val_get[i][j]));
      }
      else
      {
        SHR_IF_ERR_EXIT(dbal_entry_array32_get
                        (unit, entry_handle, fields_combination.fields_is_key[comb_index][j],
                         fields_combination.fields_id[comb_index][j], field_val_get[i][j]));
      }
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle, DBAL_COMMIT_KEEP_HANDLE));

    /*
     * compare results 
     */
    if (compare_fields_values(unit, table, field_val[i], field_val_get[i]) != 0)
    {
      /*
       * if set-get are not identical:
       * check if later inserted entry override it (same key)  
       */
      uint8 found_entry = FALSE;
      for (j = i + 1; j < nof_entries; j++)
      {
        if (compare_fields_values(unit, table, field_val[j], field_val_get[i]) == 0)
        {
          cli_out("Entry found in the %d entry, expected to found in the %d, check same entry\n", j, i);
          found_entry = TRUE;
          break;
        }
      }
      if (found_entry == FALSE)
      {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error. Entry %d was not found\n", i);
      }
    }
  }
  SHR_IF_ERR_EXIT(dbal_entry_handle_release(unit, entry_handle));
exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* mact table example, runs in dbal regression  
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] l2addr
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
dbal_mact_entry_add_example(
  int unit,
  bcm_l2_addr_t * l2addr)
{
  uint32 entry_handle_id;       /* example of logical table id */

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_MACT, &entry_handle_id));

  /*
   * adding IPv4 DIP field 
   */
  /*
   * Just an example of using the mask API:
   * uint8 mac_mask[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
   * dbal_entry_array8_set_mask( unit, entry_handle_id, FIELD_IS_KEY, FIELD_L2_MAC, l2addr.mac, mac_mask);
   */

  SHR_IF_ERR_EXIT(dbal_entry_array8_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_L2_MAC, l2addr->mac));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_FID, l2addr->vid));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_FEC, l2addr->encap_id));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));

  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PORT_ID, l2addr->encap_id));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));

  SHR_IF_ERR_EXIT(dbal_entry_array32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_FEC, (uint32 *) & (l2addr->encap_id)));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* ingress vlan table example, runs in dbal regression  
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
dbal_ingress_vlan_example(
  int unit)
{
  uint32 entry_handle_id;
  int iter;
  uint8 port_mem[32] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x99, 0x66, 0x77,
    0x21, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x41, 0x42, 0x43, 0x45, 0x46, 0x47, 0x48, 0x49
  };
  uint8 port_mem_ret[32] = { 0 };

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_VLAN_PBMP, &entry_handle_id));

  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, 15));
  SHR_IF_ERR_EXIT(dbal_entry_array8_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PP_PORT_MEMBER, port_mem));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, 0));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_VLAN_PBMP, &entry_handle_id));

  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, 15));
  SHR_IF_ERR_EXIT(dbal_entry_array8_get
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PP_PORT_MEMBER, port_mem_ret));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, 0));

  SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  for (iter = 0; iter < 32; iter++)
  {
    if (port_mem[iter] != port_mem_ret[iter])
    {
      cli_out("ERROR byte %d failed to validate orig val = %x received val = %x\n", iter, port_mem[iter],
              port_mem_ret[iter]);
    }

  }

  cli_out("entry validated \n");

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* ingress port table example, runs in dbal regression  
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
dbal_ingress_port_example(
  int unit)
{
  uint32 entry_handle_id;
  uint32 vlan_domain, port_vid;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_PORT, &entry_handle_id));

  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 15));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, 0));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_DOMAIN, 3));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PORT_VID, 10));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 15));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 19));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 39));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_DOMAIN, 6));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 99));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_KEEP_HANDLE));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_DOMAIN, 6));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 47));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  if (1)
  {
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_PORT, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, 15));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, 0));
    SHR_IF_ERR_EXIT(dbal_entry_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_DOMAIN, &vlan_domain));
    SHR_IF_ERR_EXIT(dbal_entry_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PORT_VID, &port_vid));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

    if (vlan_domain != 3)
    {
      cli_out("ERROR wrong vlan domain orig val = 3 received val = %x\n", vlan_domain);
    }

    if (port_vid != 10)
    {
      cli_out("ERROR wrong port vid orig val = 10 received val = %x\n", port_vid);
    }

    cli_out("entry validated \n");
  }

exit:
  SHR_FUNC_EXIT;
}

/*! ****************************************************
* \brief
* in lif table example, runs in dbal regression  
*  
*  \par DIRECT INPUT:
*    \param [in] unit
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
static cmd_result_t
dbal_inlif_entry_add_example(
  int unit)
{
  uint32 entry_handle_id;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));

  /*
   * adding in_lif fields 
   */
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_IN_LIF, 10000));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_GLOB_IN_LIF, 2));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_OAM_LIF_SET, 1));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_OUT_LIF, 4));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_OUT_LIF_PROFILE, 5));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_QOS_PROFILE, 6));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_EDIT_PROFILE, 7));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_EDIT_VID_1, 8));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_VLAN_EDIT_VID_2, 9));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_DESTINATION, 10));

  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  cli_out("entry added \n");

exit:
  SHR_FUNC_EXIT;
}

cmd_result_t
dbal_jer2_example(
  int unit,
  int mode)
{
  bcm_l2_addr_t l2addr;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dbal_init(unit));     /* done anyway in init */

  switch (mode)
  {
    case 0:
      cli_out("ingress vlan example\n");
      cli_out("====================\n");
      SHR_IF_ERR_EXIT(dbal_ingress_vlan_example(unit));
      cli_out("\n");
      break;

    case 1:
      cli_out("ingress port example\n");
      cli_out("====================\n");
      SHR_IF_ERR_EXIT(dbal_ingress_port_example(unit));
      cli_out("\n");
      break;
      break;

    case 4:

      cli_out("mact example\n");
      cli_out("==============\n");

      l2addr.mac[0] = 0x11;
      l2addr.mac[1] = 0x21;
      l2addr.mac[2] = 0x31;
      l2addr.mac[3] = 0x41;
      l2addr.mac[4] = 0x51;
      l2addr.mac[5] = 0x61;
      l2addr.vid = 3;
      l2addr.encap_id = 9;
      SHR_IF_ERR_EXIT(dbal_mact_entry_add_example(unit, &l2addr));
      cli_out("\n");
      break;

    case 8:
      cli_out("in-lif example\n");
      cli_out("==============\n");
      SHR_IF_ERR_EXIT(dbal_inlif_entry_add_example(unit));
      cli_out("\n");
      break;

    default:
      cli_out("ERROR input %d\n\n", mode);
      break;
  }

exit:
  SHR_FUNC_EXIT;
}
