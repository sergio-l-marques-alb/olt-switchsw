/*! \file mdb_em.c
 *
 * Contains all of the MDB Exact Match access functions provided to the DBAL.
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
#include <soc/dnx/dbal_defines.h>
#include <soc/dnx/dbal_structures.h>
#include "mdb_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

typedef enum
{
  MDB_EM_CMD_DELETE,
  MDB_EM_CMD_INSERT,
  MDB_EM_CMD_REFRESH,
  MDB_EM_CMD_LEARN,
  MDB_EM_CMD_DEFRAG,
  MDB_EM_CMD_ACK,
  MDB_EM_CMD_MOVE,
  MDB_EM_CMD_LOOKUP,

  MDB_NOF_EM_CMD
} mdb_em_cmd_e;

#define MDB_EM_CMD_OFFSET     0
#define MDB_EM_CMD_MASK       0x7
#define MDB_EM_APP_OFFSET     3
#define MDB_EM_APP_MASK       0x3F
#define MDB_EM_ENTRY_OFFSET   9

soc_mem_t mdb_em_dbal_to_mdb[DBAL_NOF_PHYSICAL_TABLES];

shr_error_e
mdb_em_entry_add(
  int unit,
  dbal_physical_tables_e dbal_physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  soc_reg_above_64_val_t data;
  int blk = MDB_BLOCK(unit);
  int is_masked;

  SHR_FUNC_INIT_VARS(unit);

  sal_memset(data, 0x0, sizeof(data));

  if (mdb_em_dbal_to_mdb[dbal_physical_table] == INVALIDm)
  {
    SHR_ERR_EXIT(_SHR_E_BADID,
                 "Error. dbal_physical_table %d is not associated with an exact match memory.\n", dbal_physical_table);
  }

  SHR_IF_ERR_EXIT(mdb_direct_is_payload_masked(unit, entry, &is_masked));

  /*
   * The command is located at MDB_EM_CMD_OFFSET, the app id at MDB_EM_APP_OFFSET, entry key at MDB_EM_ENTRY_OFFSET, entry payload at MDB_EM_ENTRY_OFFSET + key_size
   */
  data[0] = MDB_EM_CMD_INSERT;
  data[0] |= (app_id & MDB_EM_APP_MASK) << MDB_EM_APP_OFFSET;
  SHR_BITCOPY_RANGE(data, MDB_EM_ENTRY_OFFSET, entry->key, 0 /*src_offset */ , entry->key_size);

  if (is_masked == 1)
  {
    /*
     * if the payload is partially masked we will need to read the entry, zero the valid payload bits and use OR to copy over only the valid bits
     */
    dbal_physical_entry_t get_entry;

    sal_memcpy(&get_entry, &entry, sizeof(dbal_physical_entry_t));

    SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table, app_id, &get_entry));

    /*
     * zero the valid payload bits
     */
    SHR_BITREMOVE_RANGE(get_entry.payload, entry->p_mask, 0, entry->payload_size, get_entry.payload);

    /*
     * Use OR to insert the masked payload
     */
    SHR_BITOR_RANGE(get_entry.payload, entry->payload, 0, entry->payload_size, get_entry.payload);

    SHR_BITCOPY_RANGE(data, MDB_EM_ENTRY_OFFSET + entry->key_size, get_entry.payload, 0 /*src_offset */ ,
                      entry->payload_size);
  }
  else
  {
    /*
     * If the payload is fully valid, we can simply copy the payload as is to the appropriate offset
     */
    SHR_BITCOPY_RANGE(data, MDB_EM_ENTRY_OFFSET + entry->key_size, entry->payload, 0 /*src_offset */ ,
                      entry->payload_size);
  }

  SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_em_dbal_to_mdb[dbal_physical_table], blk, 0 /*index */ , data));

#ifdef MDB_DEBUG_PRINTS
  {
    uint32 data_offset;
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_add: start\n")));
    LOG_CLI((BSL_META_U(0, "entry->key_size: %d. entry->payload_size: %d, physical_table: %d, app_id: %d.\n"),
             entry->key_size, entry->payload_size, dbal_physical_table, app_id));
    for (data_offset = 0; data_offset < sizeof(entry->key) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->key: %d: %08X.\n"), data_offset, entry->key[data_offset]));
    }
    for (data_offset = 0; data_offset < sizeof(entry->payload) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->payload: %d: %08X.\n"), data_offset, entry->payload[data_offset]));
    }
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_add: end\n")));
  }
#endif

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_get(
  int unit,
  dbal_physical_tables_e dbal_physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  soc_reg_above_64_val_t write_data, read_data;
  int blk = MDB_BLOCK(unit);

  SHR_FUNC_INIT_VARS(unit);

  sal_memset(write_data, 0x0, sizeof(write_data));

  if (mdb_em_dbal_to_mdb[dbal_physical_table] == INVALIDm)
  {
    SHR_ERR_EXIT(_SHR_E_BADID,
                 "Error. dbal_physical_table %d is not associated with an exact match memory.\n", dbal_physical_table);
  }

  /*
   * Issue a lookup command
   * The command is located at MDB_EM_CMD_OFFSET, the app id at MDB_EM_APP_OFFSET, entry key at MDB_EM_ENTRY_OFFSET, entry payload at MDB_EM_ENTRY_OFFSET + key_size
   */
  write_data[0] = MDB_EM_CMD_LOOKUP;
  write_data[0] |= (app_id & MDB_EM_APP_MASK) << MDB_EM_APP_OFFSET;
  SHR_BITCOPY_RANGE(write_data, MDB_EM_ENTRY_OFFSET, entry->key, 0 /*src_offset */ , entry->key_size);
  SHR_BITCOPY_RANGE(write_data, MDB_EM_ENTRY_OFFSET + entry->key_size, entry->payload, 0 /*src_offset */ ,
                    entry->payload_size);
  SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_em_dbal_to_mdb[dbal_physical_table], blk, 0 /*index */ , write_data));

  /*
   * Read the lookup response
   */
  SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_em_dbal_to_mdb[dbal_physical_table], blk, 0 /*index */ , read_data));

  SHR_BITCOPY_RANGE(entry->payload, 0 /*dst_offset */ , read_data, MDB_EM_ENTRY_OFFSET + entry->key_size,
                    entry->payload_size);

#ifdef MDB_DEBUG_PRINTS
  {
    uint32 data_offset;
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_add: start\n")));
    LOG_CLI((BSL_META_U(0, "entry->key_size: %d. entry->payload_size: %d, physical_table: %d, app_id: %d.\n"),
             entry->key_size, entry->payload_size, dbal_physical_table, app_id));
    for (data_offset = 0; data_offset < sizeof(entry->key) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->key: %d: %08X.\n"), data_offset, entry->key[data_offset]));
    }
    for (data_offset = 0; data_offset < sizeof(entry->payload) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->payload: %d: %08X.\n"), data_offset, entry->payload[data_offset]));
    }
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_add: end\n")));
  }
#endif

exit:
  SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_delete(
  int unit,
  dbal_physical_tables_e dbal_physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry)
{
  soc_reg_above_64_val_t data;
  int blk = MDB_BLOCK(unit);

  SHR_FUNC_INIT_VARS(unit);

  sal_memset(data, 0x0, sizeof(data));

  if (mdb_em_dbal_to_mdb[dbal_physical_table] == INVALIDm)
  {
    SHR_ERR_EXIT(_SHR_E_BADID,
                 "Error. dbal_physical_table %d is not associated with an exact match register.\n",
                 dbal_physical_table);
  }

  /*
   * The command is located at MDB_EM_CMD_OFFSET, the app id at MDB_EM_APP_OFFSET, entry key at MDB_EM_ENTRY_OFFSET, entry payload at MDB_EM_ENTRY_OFFSET + key_size
   */
  data[0] = MDB_EM_CMD_DELETE;
  data[0] |= (app_id & MDB_EM_APP_MASK) << MDB_EM_APP_OFFSET;
  SHR_BITCOPY_RANGE(data, MDB_EM_ENTRY_OFFSET, entry->key, 0 /*src_offset */ , entry->key_size);

  /*
   * soc_mem_field_set(unit, mdb_em_dbal_to_mdb[dbal_physical_table], data, COMMANDf, MDB_EM_CMD_DELETE);
   * soc_mem_field_set(unit, mdb_em_dbal_to_mdb[dbal_physical_table], data, APPLICATION_IDf, 0);
   * soc_mem_field_set(unit, mdb_em_dbal_to_mdb[dbal_physical_table], data, ENTRYf, entry->key);
   */

  SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_em_dbal_to_mdb[dbal_physical_table], blk, 0 /*index */ , data));

#ifdef MDB_DEBUG_PRINTS
  {
    uint32 data_offset;
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_delete: start\n")));
    LOG_CLI((BSL_META_U(0, "entry->key_size: %d. entry->payload_size: %d, physical_table: %d, app_id: %d.\n"),
             entry->key_size, entry->payload_size, dbal_physical_table, app_id));
    for (data_offset = 0; data_offset < sizeof(entry->key) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->key: %d: %08X.\n"), data_offset, entry->key[data_offset]));
    }
    for (data_offset = 0; data_offset < sizeof(entry->payload) / sizeof(uint32); data_offset++)
    {
      LOG_CLI((BSL_META_U(0, "entry->payload: %d: %08X.\n"), data_offset, entry->payload[data_offset]));
    }
    LOG_CLI((BSL_META_U(0, "mdb_em_entry_delete: end\n")));
  }
#endif

exit:
  SHR_FUNC_EXIT;
}
