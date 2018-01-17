/*! \file mdb_direct.c
 *
 * Contains all of the MDB direct table access functions provided to the DBAL.
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

#include <soc/dnx/mdb.h>
#include "mdb_internal.h"
#include <soc/dnx/dbal_defines.h>
#include <soc/dnx/dbal_structures.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

#define MDB_ENTRY_BANK_SIZE_BITS 512
#define MDB_ENTRY_DATA_SIZE_BITS 128
#define MDB_ENTRY_DATA_PER_BANK  (MDB_ENTRY_BANK_SIZE_BITS/MDB_ENTRY_DATA_SIZE_BITS)

#define MDB_ENTRY_READ_INST      1
#define MDB_ENTRY_WRITE_INST     2
#define MDB_ENTRY_DATA_OFFSET    8

/* Replace with appropriate shared define */
#define MDB_BITS_IN_UINT32       32

mdb_physical_tables_e mdb_direct_dbal_to_mdb[DBAL_NOF_PHYSICAL_TABLES];

/*
 * This function returns the bit offset of the data within a bucket row
 */
static uint32 mdb_direct_table_calc_data_bit_offset(
  int unit,
  uint32 cluster_index,
  uint32 entry_index,
  uint32 entries_per_row);

/*
 * This function issues a read command and then reads the result into row_data
 */
static shr_error_e mdb_direct_table_read_entry_from_cluster(
  int unit,
  soc_mem_t mem,
  uint32 bucket_index,
  int blk,
  int offset,
  uint32 * row_data);

/*
 * This function calculates from the mem and global cluster index:
 * The blk needed to write the entry
 * The bucket_index (e.g. 0 or 1)
 * The cluster_index within the bucket (e.g. 0,1,2,3)
 */
static shr_error_e mdb_direct_table_calc_cluster_address(
  int unit,
  soc_mem_t mem,
  int cluster,
  int *blk,
  uint32 * bucket_index,
  uint32 * cluster_index);

/*
 * This function calculates from the entry_index:
 * The MACRO type (DHA/DHB)
 * The cluster in which the entry resides
 * the offset within that cluster
 */
static shr_error_e mdb_direct_table_calc_index_address(
  int unit,
  mdb_physical_tables_e physical_table,
  uint32 entry_index,
  uint32 entries_per_row,
  soc_mem_t * mem,
  int *cluster,
  int *offset);






shr_error_e
mdb_direct_table_calc_cluster_address(
  int unit,
  soc_mem_t mem,
  int cluster,
  int *blk,
  uint32 * bucket_index,
  uint32 * cluster_index)
{
  SHR_FUNC_INIT_VARS(unit);

  /*
   * Each bucket holds 4 clusters, bucket_index is 0 or 1
   */
  if (mem == DHA_ENTRY_BANKm)
  {
    *blk = DHA_BLOCK(unit, (cluster / MDB_NOF_MACRO_A_CLUSTERS));
    *cluster_index = cluster % MDB_NOF_MACRO_A_CLUSTER_IN_BUCKET;
    *bucket_index = (cluster % MDB_NOF_MACRO_A_CLUSTERS) / MDB_NOF_MACRO_A_CLUSTER_IN_BUCKET;
  }
  else if (mem == DHB_ENTRY_BANKm)
  {
    *blk = DHB_BLOCK(unit, (cluster / MDB_NOF_MACRO_B_CLUSTERS));
    *cluster_index = cluster % MDB_NOF_MACRO_B_CLUSTER_IN_BUCKET;
    *bucket_index = (cluster % MDB_NOF_MACRO_B_CLUSTERS) / MDB_NOF_MACRO_B_CLUSTER_IN_BUCKET;
  }
  else
  {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Mem %d is not supported.\n", mem);
  }

exit:
  SHR_FUNC_EXIT;
}

uint32
mdb_direct_table_calc_data_bit_offset(
  int unit,
  uint32 cluster_index,
  uint32 entry_index,
  uint32 entries_per_row)
{
  SHR_FUNC_INIT_VARS(unit);

  /*
   * The data offset is:
   * Each row is 512 bits, which 128 bits we intend to write (cluster_index * MDB_ENTRY_DATA_SIZE_BITS)
   * Within the 128 bits, where are the 120 data bits located (MDB_ENTRY_DATA_OFFSET)
   * Within the 120bits, if we have more than one entry per row, which entry we write
   */
  return (cluster_index * MDB_ENTRY_DATA_SIZE_BITS) + MDB_ENTRY_DATA_OFFSET +
    (entry_index % entries_per_row) * MDB_NOF_MACRO_ROW_BITS / entries_per_row;

  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_read_entry_from_cluster(
  int unit,
  soc_mem_t mem,
  uint32 bucket_index,
  int blk,
  int offset,
  uint32 * row_data)
{
  int data_traverse;

  SHR_FUNC_INIT_VARS(unit);

  /*
   * The command is 128bits, first three bits are the command, next 5 bits are the format and the next 120 are the entry.
   */

  /*
   * First write a read command and then read
   */
  for (data_traverse = 0;
       data_traverse < MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32;
       data_traverse += MDB_ENTRY_DATA_SIZE_BITS / MDB_BITS_IN_UINT32)
  {
    row_data[data_traverse] = MDB_ENTRY_READ_INST;
  }
  SHR_IF_ERR_EXIT(soc_mem_array_write(unit, mem, bucket_index, blk, offset, row_data));

  SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, bucket_index, blk, offset, row_data));

#ifdef MDB_DEBUG_PRINTS
  LOG_CLI((BSL_META_U(0, "mdb_direct_table_read_entry_from_cluster:\n")));
  LOG_CLI((BSL_META_U(0, "Mem: %s. bucket_index: %d. blk: %d. offset: %d.\n"),
           SOC_MEM_NAME(unit, mem), bucket_index, blk, offset));
  for (data_traverse = 0; data_traverse < MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32; data_traverse++)
  {
    if (data_traverse % MDB_ENTRY_DATA_PER_BANK == 0)
    {
      LOG_CLI((BSL_META_U(0, "\n")));
    }
    LOG_CLI((BSL_META_U(0, "row_data %d: %08X. "), data_traverse, row_data[data_traverse]));
  }
  LOG_CLI((BSL_META_U(0, "\n")));
#endif

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_calc_index_address(
  int unit,
  mdb_physical_tables_e physical_table,
  uint32 entry_index,
  uint32 entries_per_row,
  soc_mem_t * mem,
  int *cluster,
  int *offset)
{
  uint32 cluster_index;
  uint32 macro_index;

  SHR_FUNC_INIT_VARS(unit);

  if (entry_index < mdb_db_infos[physical_table].macro_a_clusters_num * MDB_NOF_MACRO_A_ROWS_CLUSTER * entries_per_row)
  {
    /*
     * Write to MACRO_A
     */
    *mem = DHA_ENTRY_BANKm;

    macro_index = entry_index;
    cluster_index = macro_index / (MDB_NOF_MACRO_A_ROWS_CLUSTER * entries_per_row);
    *offset = (macro_index - (cluster_index * (MDB_NOF_MACRO_A_ROWS_CLUSTER * entries_per_row))) / entries_per_row;
    *cluster = mdb_db_infos[physical_table].macro_a_clusters_addr[cluster_index];
  }
  else if (entry_index <
           (((mdb_db_infos[physical_table].macro_a_clusters_num * MDB_NOF_MACRO_A_ROWS_CLUSTER) * entries_per_row) +
            ((mdb_db_infos[physical_table].macro_b_clusters_num * MDB_NOF_MACRO_B_ROWS_CLUSTER) * entries_per_row)))
  {
    /*
     * Write to MACRO_B
     */
    *mem = DHB_ENTRY_BANKm;

    /*
     * The index minus all of the indices stored in MACRO A
     */
    macro_index =
      (entry_index -
       ((mdb_db_infos[physical_table].macro_a_clusters_num * MDB_NOF_MACRO_A_ROWS_CLUSTER) * entries_per_row));
    cluster_index = macro_index / (MDB_NOF_MACRO_B_ROWS_CLUSTER * entries_per_row);
    *offset = (macro_index - (cluster_index * (MDB_NOF_MACRO_B_ROWS_CLUSTER * entries_per_row))) / entries_per_row;
    *cluster = mdb_db_infos[physical_table].macro_b_clusters_addr[cluster_index];
  }
  else
  {
    /*
     * Return error
     */
    SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error. Index %d out of range in physical table %d.\n", entry_index, physical_table);
  }

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_calc_entries_per_row(
  int unit,
  dbal_physical_table_def_t * dbal_physical_table,
  uint32 * entries_per_row)
{
  uint32 entries_per_row_calc;

  SHR_FUNC_INIT_VARS(unit);

  entries_per_row_calc = MDB_NOF_MACRO_ROW_BITS / dbal_physical_table->max_entry_size;

  /*
   * We only support 4/2/1 entries per row
   */
  if (entries_per_row_calc >= 4)
  {
    entries_per_row_calc = 4;
  }
  else if (entries_per_row_calc >= 2)
  {
    entries_per_row_calc = 2;
  }
  else if (entries_per_row_calc == 0)
  {
    /*
     * Entries over 120bit currently aren't supported
     */
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Error. Currently the MDB does not support entries over 120bit.\n");
  }

  *entries_per_row = entries_per_row_calc;

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_is_payload_masked(
  int unit,
  dbal_physical_entry_t * entry,
  int *is_masked)
{

  int valid_payload_bits;

  SHR_FUNC_INIT_VARS(unit);

  shr_bitop_range_count(entry->p_mask, 0 /*first */ , entry->payload_size, &valid_payload_bits);

  if (valid_payload_bits == entry->payload_size)
  {
    *is_masked = 0;
  }
  else
  {
    *is_masked = 1;
  }

  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_add(
  int unit,
  dbal_physical_tables_e dbal_physical_table_id,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  dbal_physical_table_def_t *dbal_physical_table;
  soc_mem_t mem;
  uint32 row_data[MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32];
  uint32 entries_per_row;
  uint32 data_offset;
  uint32 entry_index = entry->key[0];
  uint32 bucket_index;
  uint32 cluster_index;
  uint32 row_data_index;
  int is_masked;
  int cluster, offset;
  int blk;

  SHR_FUNC_INIT_VARS(unit);

  if (mdb_direct_dbal_to_mdb[dbal_physical_table_id] == MDB_NOF_PHYSICAL_TABLES)
  {
    SHR_ERR_EXIT(_SHR_E_PARAM,
                 "Error. DBAL physical table %d is not associated with an MDB direct table.\n", dbal_physical_table_id);
  }

  SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_entries_per_row(unit, dbal_physical_table, &entries_per_row));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_index_address
                  (unit, mdb_direct_dbal_to_mdb[dbal_physical_table_id], entry_index, entries_per_row, &mem, &cluster,
                   &offset));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_cluster_address(unit, mem, cluster, &blk, &bucket_index, &cluster_index));
  row_data_index = cluster_index * (MDB_ENTRY_DATA_SIZE_BITS / MDB_BITS_IN_UINT32);

  data_offset = mdb_direct_table_calc_data_bit_offset(unit, cluster_index, entry_index, entries_per_row);

  SHR_IF_ERR_EXIT(mdb_direct_is_payload_masked(unit, entry, &is_masked));

  if (is_masked == 1)
  {
    /*
     * if the payload is partially masked we will need to read the entry, zero the valid payload bits and use OR to copy over only the valid bits
     */
    uint32 payload[(DBAL_FIELD_ARRAY_MAX_SIZE * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS) / 4];

    /*
     * Read the entry from the HW
     */
    SHR_IF_ERR_EXIT(mdb_direct_table_read_entry_from_cluster(unit, mem, bucket_index, blk, offset, row_data));
    SHR_BITCOPY_RANGE(payload, 0 /*src_offset */ , row_data, data_offset, entry->payload_size);

    /*
     * zero the valid payload bits
     */
    SHR_BITREMOVE_RANGE(payload, entry->p_mask, 0, entry->payload_size, payload);

    /*
     * Use OR to insert the masked payload
     */
    SHR_BITOR_RANGE(payload, entry->payload, 0, entry->payload_size, payload);

    SHR_BITCOPY_RANGE(row_data, data_offset, payload, 0 /*src_offset */ , entry->payload_size);
  }
  else
  {
    /*
     * If the payload is fully valid, we can simply copy the payload as is to the appropriate offset
     */
    if (entries_per_row > 1)
    {
      /*
       * If we have more than one entry per row , we will need to read-modify-write
       */
      SHR_IF_ERR_EXIT(mdb_direct_table_read_entry_from_cluster(unit, mem, bucket_index, blk, offset, row_data));
    }
    else
    {
      sal_memset(row_data, 0x0, (MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32) * sizeof(row_data[0]));
    }

    SHR_BITCOPY_RANGE(row_data, data_offset, entry->payload, 0 /*src_offset */ , entry->payload_size);
  }

  row_data[row_data_index] |= MDB_ENTRY_WRITE_INST;
  SHR_IF_ERR_EXIT(soc_mem_array_write(unit, mem, bucket_index, blk, offset, row_data));

#ifdef MDB_DEBUG_PRINTS
  LOG_CLI((BSL_META_U(0, "mdb_direct_table_entry_add: start\n")));
  LOG_CLI((BSL_META_U(0, "entry.key: %d. entry.payload_size: %d, physical_table: %d.\n"),
           entry->key[0], entry->payload_size, dbal_physical_table_id));
  LOG_CLI((BSL_META_U(0, "Mem: %s. bucket_index: %d. blk: %d. offset: %d. data_index: %d.\n"),
           SOC_MEM_NAME(unit, mem), bucket_index, blk, offset, cluster_index));
  for (cluster_index = 0; cluster_index < MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32; cluster_index++)
  {
    if (cluster_index % MDB_ENTRY_DATA_PER_BANK == 0)
    {
      LOG_CLI((BSL_META_U(0, "\n")));
    }
    LOG_CLI((BSL_META_U(0, "entry_data %d: %08X. "), cluster_index, row_data[cluster_index]));
  }
  LOG_CLI((BSL_META_U(0, "\n")));
  for (data_offset = 0; data_offset < (entry->payload_size / MDB_BITS_IN_UINT32 + 1); data_offset++)
  {
    LOG_CLI((BSL_META_U(0, "entry.payload %d: %08X.\n"), data_offset, entry->payload[data_offset]));
  }
  LOG_CLI((BSL_META_U(0, "mdb_direct_table_entry_add: end\n")));
#endif

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_get(
  int unit,
  dbal_physical_tables_e dbal_physical_table_id,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  dbal_physical_table_def_t *dbal_physical_table;
  soc_mem_t mem;
  uint32 row_data[MDB_ENTRY_BANK_SIZE_BITS / MDB_BITS_IN_UINT32];
  uint32 entries_per_row;
  uint32 data_offset;
  uint32 entry_index = entry->key[0];
  uint32 bucket_index;
  uint32 cluster_index;
  int offset;
  int blk;
  int cluster;

  SHR_FUNC_INIT_VARS(unit);

  if (mdb_direct_dbal_to_mdb[dbal_physical_table_id] == MDB_NOF_PHYSICAL_TABLES)
  {
    SHR_ERR_EXIT(_SHR_E_PARAM,
                 "Error. DBAL physical table %d is not associated with an MDB direct table.\n", dbal_physical_table_id);
  }

  SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_entries_per_row(unit, dbal_physical_table, &entries_per_row));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_index_address
                  (unit, mdb_direct_dbal_to_mdb[dbal_physical_table_id], entry_index, entries_per_row, &mem, &cluster,
                   &offset));

  SHR_IF_ERR_EXIT(mdb_direct_table_calc_cluster_address(unit, mem, cluster, &blk, &bucket_index, &cluster_index));

  SHR_IF_ERR_EXIT(mdb_direct_table_read_entry_from_cluster(unit, mem, bucket_index, blk, offset, row_data));

  data_offset = mdb_direct_table_calc_data_bit_offset(unit, cluster_index, entry_index, entries_per_row);

  SHR_BITCOPY_RANGE(entry->payload, 0 /*src_offset */ , row_data, data_offset, entry->payload_size);

#ifdef MDB_DEBUG_PRINTS
  LOG_CLI((BSL_META_U(0, "mdb_direct_table_entry_get: start\n")));
  LOG_CLI((BSL_META_U(0, "entry.key: %d. entry.payload_size: %d, physical_table: %d.\n"),
           entry->key[0], entry->payload_size, dbal_physical_table_id));
  for (data_offset = 0; data_offset < (entry->payload_size / MDB_BITS_IN_UINT32 + 1); data_offset++)
  {
    LOG_CLI((BSL_META_U(0, "entry.payload %d: %08X.\n"), data_offset, entry->payload[data_offset]));
  }
  LOG_CLI((BSL_META_U(0, "mdb_direct_table_entry_get: end\n")));
#endif

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_delete(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_ERR_EXIT(_SHR_E_UNAVAIL,
               "Error. Direct tables do not support delete, set the entry to the desired value instead.\n");

exit:
  SHR_FUNC_EXIT;
}
