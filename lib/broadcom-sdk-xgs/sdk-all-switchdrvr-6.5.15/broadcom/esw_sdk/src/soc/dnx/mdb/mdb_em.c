        /*
         * ! \file mdb_em.c $Id$ Contains all of the MDB Exact Match access functions provided to the DBAL.
         */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/sand/sand_mem.h>
#include "mdb_internal.h"
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

typedef enum
{
    MDB_EM_CMD_DELETE = 320,
    MDB_EM_CMD_INSERT = 256,
    MDB_EM_CMD_INSERT_OFC = 384,
    MDB_EM_CMD_TRANSPLANT = 288,
    MDB_EM_CMD_LOOKUP = 0,
    MDB_EM_CMD_READ_BY_ADDR = 1,
    MDB_EM_CMD_READ_HITS_A_BY_ADDR = 6,
    MDB_EM_CMD_READ_HITS_B_BY_ADDR = 7,
    MDB_EM_CMD_READ_CLEAR_HITS_A_BY_ADDR = 8,
    MDB_EM_CMD_READ_CLEAR_HITS_B_BY_ADDR = 9,

    MDB_NOF_EM_CMD = 10
} mdb_em_cmd_e;

typedef enum
{
    MDB_EM_MACT_CMD_INSERT = 0,
    MDB_EM_MACT_CMD_TRANSPLANT = 1,
    MDB_EM_MACT_CMD_DELETE = 2,
    MDB_EM_MACT_CMD_REFRESH = 3,
    MDB_EM_MACT_CMD_LEARN = 4,
    MDB_EM_MACT_CMD_ACK = 5,
    MDB_EM_MACT_CMD_FLUSH_REPLY = 6,

    MDB_NOF_EM_MACT_CMD
} mdb_em_mact_cmd_e;

typedef enum
{
    MDB_EM_FAIL_REASON_CLEAR = 0x0,
    MDB_EM_FAIL_REASON_CAM_TABLE_FULL = 0x1,
    MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED = 0x2,
    MDB_EM_FAIL_REASON_DELETE_UNKNOWN_KEY = 0x3,
    MDB_EM_FAIL_REASON_STEPS_WATCHDOG_TRIGGER = 0x4,
    MDB_EM_FAIL_REASON_INSERTED_EXISTING = 0x5,
    MDB_EM_FAIL_REASON_CHANGE_FAIL_REQUEST_NON_EXIST = 0x6,
    MDB_EM_FAIL_REASON_ECC_ERROR = 0x7,
    MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC = 0x8,
    MDB_EM_FAIL_REASON_BAD_STARTING_ENTRY = 0x9,
    MDB_EM_FAIL_REASON_EXCEED_LIMIT = 0xa,
    MDB_EM_FAIL_REASON_DELETE_NON_EXIST = 0xb,
    MDB_EM_FAIL_REASON_LEARN_OVER_STRONGER = 0xc,
    MDB_EM_FAIL_REASON_TRANSPLANT_OVER_STRONGER = 0xd,
    MDB_EM_FAIL_REASON_REFRESH_OVER_STRONGER = 0xe,
    MDB_EM_FAIL_REASON_FLUSH_DROP = 0xf,

    MDB_NOF_FAIL_REASONS
} mdb_em_fail_reason_e;

static const char *mdb_em_fail_reason_strings[MDB_NOF_FAIL_REASONS] = {
    "CLEAR",
    "CAM_TABLE_FULL",
    "CAM_TABLE_LOCKED",
    "DELETE_UNKNOWN_KEY",
    "STEPS_WATCHDOG_TRIGGER",
    "INSERTED_EXISTING",
    "CHANGE_FAIL_REQUEST_NON_EXIST",
    "ECC_ERROR",
    "INVALID_TID_FOR_OFC",
    "BAD_STARTING_ENTRY",
    "EXCEED_LIMIT",
    "DELETE_NON_EXIST",
    "LEARN_OVER_STRONGER",
    "TRANSPLANT_OVER_STRONGER",
    "REFRESH_OVER_STRONGER",
    "FLUSH_DROP"
};
/*
 * }
 */

/*
 * Defines
 * {
 */
#define MDB_EM_SHADOW_HASH_NOF_RESOURCES         (10)
#define MDB_EM_NOF_BITS_8K_ADDRESS                (13)
#define MDB_EM_FORMAT_MASK_SHIFT                  (2)
#define MDB_EM_FORMAT_OFFSET_MASK                 (0x3)
#define MDB_INIT_ENCODING_TYPE_INVALID            (0xFF)
#define MDB_EM_MAX_SHADOW_KEY_IN_BYTE  BITS2BYTES(DNX_DATA_MAX_MDB_PDBS_MAX_KEY_SIZE + DNX_DATA_MAX_MDB_EM_MAX_TID_SIZE)
#define MDB_EM_MAX_PAYLOAD_IN_BYTE  BITS2BYTES(DNX_DATA_MAX_MDB_PDBS_MAX_PAYLOAD_SIZE)

#define MDB_EM_SW_ACCESS mdb_em_db

#define MDB_EM_MACT_STAMP_MAX 0xFF

#define MDB_EM_MACT_POLL_ATTEMPTS 100

#define MDB_EM_HITBIT_AGE_ATTEMPTS 100

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

const uint8 mdb_init_encoding_values_map[MDB_EM_NOF_ENCODING_REQUESTS][MDB_EM_NOF_ENCODING_TYPES] = {
    {MDB_EM_ENTRY_ENCODING_ONE, MDB_EM_ENTRY_ENCODING_HALF, MDB_EM_ENTRY_ENCODING_QUARTER, MDB_EM_ENTRY_ENCODING_EIGHTH,
     MDB_EM_ENTRY_ENCODING_EMPTY},
    {MDB_EM_DH_120_ENTRY_ENCODING_ONE, MDB_EM_DH_120_ENTRY_ENCODING_HALF, MDB_EM_DH_120_ENTRY_ENCODING_QUARTER,
     MDB_INIT_ENCODING_TYPE_INVALID, MDB_EM_DH_120_ENTRY_ENCODING_EMPTY},
    {MDB_EM_DH_240_ENTRY_ENCODING_ONE, MDB_EM_DH_240_ENTRY_ENCODING_HALF, MDB_EM_DH_240_ENTRY_ENCODING_QUARTER,
     MDB_EM_DH_240_ENTRY_ENCODING_EIGHTH, MDB_EM_DH_240_ENTRY_ENCODING_EMPTY}
};

const uint8 mdb_init_encoding_value_to_encoding_index_map[MDB_EM_NOF_ENTRY_ENCODINGS + 1] = {
    MDB_EM_ENCODING_TYPE_ONE, MDB_EM_ENCODING_TYPE_HALF, MDB_EM_ENCODING_TYPE_QUARTER, MDB_EM_ENCODING_TYPE_EIGHTH,
    MDB_EM_NOF_ENCODING_TYPES, MDB_EM_NOF_ENCODING_TYPES, MDB_EM_NOF_ENCODING_TYPES, MDB_EM_ENCODING_TYPE_EMPTY,
    MDB_EM_NOF_ENCODING_TYPES
};

shr_error_e
mdb_em_init_timeout_overrides(
    int unit)
{
    uint32 reg_val = 0, timeout_val = 0x14 << 10 | 0xd6;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MACT_INDIRECT_COMMANDr(unit, MACT_BLOCK(unit), &reg_val));
    soc_reg_field_set(unit, MACT_INDIRECT_COMMANDr, &reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
    SHR_IF_ERR_EXIT(WRITE_MACT_INDIRECT_COMMANDr(unit, MACT_BLOCK(unit), reg_val));

    SHR_IF_ERR_EXIT(READ_MDB_INDIRECT_COMMANDr(unit, &reg_val));
    soc_reg_field_set(unit, MDB_INDIRECT_COMMANDr, &reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
    SHR_IF_ERR_EXIT(WRITE_MDB_INDIRECT_COMMANDr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Indicate if this is a MACT application.
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] dbal_physical_table -\n
 *      The physical table.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [out] is_mact -\n
 *      Will be set to 1 in case this is a MACT application and 0 otherwise.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e
mdb_em_is_mact_application(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint8 *is_mact)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_mact = 0;

    if (dbal_physical_table == DBAL_PHYSICAL_TABLE_LEM)
    {
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.is_mact_lem_app.bit_get(unit, app_id, is_mact));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_init_shadow_db(
    int unit)
{
    sw_state_htbl_init_info_t htb_init_info;
    dbal_physical_tables_e dbal_physical_table;
    int tid_id_iter;
    SHR_FUNC_INIT_VARS(unit);

    for (dbal_physical_table = 0; dbal_physical_table < DBAL_NOF_PHYSICAL_TABLES; dbal_physical_table++)
    {
        if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            int capacity;
            int key_size_in_byte =
                BITS2BYTES(dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table)->max_key_size +
                           dnx_data_mdb.em.em_info_get(unit, dbal_physical_table)->tid_size);

            sal_memset(&htb_init_info, 0, sizeof(htb_init_info));
            mdb_db_infos.capacity.get(unit, dbal_physical_table, &capacity);

            capacity += dnx_data_mdb.em.ovf_cam_max_size_get(unit);
            htb_init_info.max_nof_elements = capacity;
            htb_init_info.expected_nof_elements = capacity;
            SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                            shadow_em_db.create(unit, dbal_physical_table, &htb_init_info, key_size_in_byte,
                                                MDB_EM_MAX_PAYLOAD_IN_BYTE));
        }

    }
    for (tid_id_iter = 0; tid_id_iter < dnx_data_mdb.em.nof_lem_mact_apps_get(unit); tid_id_iter++)
    {
        const dbal_logical_table_t *dbal_logical_table;
        uint32 app_id;
        SHR_IF_ERR_EXIT(dbal_tables_table_get
                        (unit, dnx_data_mdb.em.flush_get(unit)->mact_apps_list[tid_id_iter], &dbal_logical_table));

        for (app_id = dbal_logical_table->app_id;
             app_id < dnx_data_mdb.em.em_info_get(unit, DBAL_PHYSICAL_TABLE_LEM)->tid_size;
             app_id += (1 << dbal_logical_table->app_id_size))
        {
            SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.is_mact_lem_app.bit_set(unit, app_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_application_uses_shadow(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint8 *is_shadow_supported)
{
    uint8 is_mact;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_is_mact_application(unit, dbal_physical_table, app_id, &is_mact));

    *is_shadow_supported = (is_mact
                            && entry->key_size <= dnx_data_mdb.em.flush_max_supported_key_get(unit)
                            && entry->payload_size <= dnx_data_mdb.em.flush_max_supported_payload_get(unit)) ? 0 : 1;

    *is_shadow_supported = 1;
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_shadow_build_key(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint8 *shadow_key)
{
    uint32 key_size_byte;
    uint32 entry_key_size_byte;
    uint32 tid_size = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table)->tid_size;

    SHR_FUNC_INIT_VARS(unit);

    key_size_byte = BITS2BYTES(dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table)->max_key_size + tid_size);

    entry_key_size_byte = BITS2BYTES(entry->key_size);

    sal_memset(shadow_key, 0, MDB_EM_MAX_SHADOW_KEY_IN_BYTE);

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(entry->key, entry_key_size_byte, shadow_key));

    /** Clear the excess bits in case the entry key has garbage on the MSB bits that follows the key data */
    shadow_key[entry_key_size_byte - 1] &= (0xFF >> ((SAL_UINT8_NOF_BITS - (entry->key_size & 0x7)) & 0x7));

    shadow_key[key_size_byte - 1] |= app_id << (SAL_UINT8_NOF_BITS - tid_size);

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_shadow_has_key(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 *found)
{
    uint8 found_byte;
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    uint8 payload[MDB_EM_MAX_PAYLOAD_IN_BYTE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shadow_em_db.find(unit, dbal_physical_table, shadow_key, payload, &found_byte));

    *found = found_byte;

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_shadow_add_key(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    uint8 shadow_payload[MDB_EM_MAX_PAYLOAD_IN_BYTE];
    uint32 payload_align[BYTES2WORDS(MDB_EM_MAX_PAYLOAD_IN_BYTE)];
    uint8 success;
    uint8 is_use_shadow;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(payload_align, 0x0, sizeof(payload_align));

    SHR_IF_ERR_EXIT(mdb_em_application_uses_shadow(unit, dbal_physical_table, app_id, entry, &is_use_shadow));

    if (is_use_shadow)
    {
        SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

        SHR_BITCOPY_RANGE(payload_align, BYTES2BITS(MDB_EM_MAX_PAYLOAD_IN_BYTE) - entry->payload_size, entry->payload,
                          0x0, entry->payload_size);

        SHR_IF_ERR_EXIT(utilex_U32_to_U8(payload_align, MDB_EM_MAX_PAYLOAD_IN_BYTE, shadow_payload));

        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                        shadow_em_db.insert(unit, dbal_physical_table, shadow_key, shadow_payload, &success));

        if (!success)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't add entry to MDB EM hash SW table.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_shadow_remove_key(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    uint8 is_use_shadow;
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_application_uses_shadow(unit, dbal_physical_table, app_id, entry, &is_use_shadow));

    if (is_use_shadow)
    {
        uint32 found;
        SHR_IF_ERR_EXIT(mdb_em_shadow_has_key(unit, dbal_physical_table, app_id, entry, &found));
        if (found != 0)
        {
            SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

            SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shadow_em_db.delete(unit, dbal_physical_table, shadow_key));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_shadow_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 *found)
{
    uint8 found_byte;
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    uint8 shadow_payload[MDB_EM_MAX_PAYLOAD_IN_BYTE];
    uint32 payload_align[BYTES2WORDS(MDB_EM_MAX_PAYLOAD_IN_BYTE)];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_payload, 0x0, sizeof(shadow_payload));

    SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shadow_em_db.find(unit, dbal_physical_table, shadow_key, shadow_payload, &found_byte));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_payload, MDB_EM_MAX_PAYLOAD_IN_BYTE, payload_align));

    SHR_BITCOPY_RANGE(entry->payload, 0x0, payload_align, BYTES2BITS(MDB_EM_MAX_PAYLOAD_IN_BYTE) - entry->payload_size,
                      entry->payload_size);

    *found = found_byte;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    mdb_em_entry_encoding_e * entry_encoding)
{
    int entry_encoding_index;
    int entry_size_capacity;
    uint8 vmv_size;
    int success = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (entry_encoding_index = MDB_EM_ENTRY_ENCODING_EIGHTH; entry_encoding_index >= MDB_EM_ENTRY_ENCODING_ONE;
         entry_encoding_index--)
    {
        entry_size_capacity =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding_index);

        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.
                        size.get(unit, dbal_physical_table_id, entry_encoding_index, &vmv_size));

        if ((entry_size_capacity < MDB_DIRECT_BASIC_ENTRY_SIZE) || (vmv_size == 0))
        {
            continue;
        }

        if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
        {
            vmv_size = 0;
        }

        if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_LEM)
        {
            uint32 mact_app_id, mact_ivl_app_id;
            int app_db_size;

            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &mact_app_id, &app_db_size));
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT_IVL, &mact_ivl_app_id, &app_db_size));

            if ((app_id == mact_app_id) || (app_id == mact_ivl_app_id))
            {
                if (vmv_size > dnx_data_mdb.em.mact_nof_vmv_size_get(unit))
                {
                    continue;
                }
            }
        }

        if (key_size - dnx_data_mdb.em.entry_addr_bits_get(unit) + app_id_size + payload_size + vmv_size <=
            entry_size_capacity)
        {
            success = 1;
            break;
        }
    }

    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, unable to find vmv to fulfill the formula. app_id %d: key_size(%d) - addr_bits(%d) + app_id_size(%d) + payload_size(%d) + vmv_size(%d)  > entry_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, key_size,
                     dnx_data_mdb.em.entry_addr_bits_get(unit), app_id_size, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width);
    }

    if (entry_encoding != NULL)
    {
        *entry_encoding = entry_encoding_index;
    }

    if (payload_size + vmv_size > dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, app_id %d: payload_size(%d) + vmv_size(%d) > mdb_max_payload_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_min_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    int vmv_size,
    mdb_em_entry_encoding_e * entry_encoding)
{
    int entry_encoding_index;
    int entry_size_capacity;
    int success = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (entry_encoding_index = MDB_EM_ENTRY_ENCODING_EIGHTH; entry_encoding_index >= MDB_EM_ENTRY_ENCODING_ONE;
         entry_encoding_index--)
    {
        entry_size_capacity =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding_index);

        if (entry_size_capacity < MDB_DIRECT_BASIC_ENTRY_SIZE)
        {
            continue;
        }

        if (key_size - dnx_data_mdb.em.entry_addr_bits_get(unit) + app_id_size + payload_size + vmv_size <=
            entry_size_capacity)
        {
            success = 1;
            break;
        }
    }

    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, unable to find vmv to fulfill the formula. app_id %d: key_size(%d) - addr_bits(%d) + app_id_size(%d) + payload_size(%d) + vmv_size(%d)  > entry_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, key_size,
                     dnx_data_mdb.em.entry_addr_bits_get(unit), app_id_size, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width);
    }

    if (entry_encoding != NULL)
    {
        *entry_encoding = entry_encoding_index;
    }

    if (payload_size + vmv_size > dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, app_id %d: payload_size(%d) + vmv_size(%d) > mdb_max_payload_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_key_size(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 *key_size)
{

    soc_reg_above_64_val_t data;
    soc_mem_t tid_atr = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_atr_mem;
    SHR_FUNC_INIT_VARS(unit);
    if (app_id >= dnx_data_mdb.em.max_nof_tids_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. APP ID %d is not valid.\n", app_id);
    }

    if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "Error. dbal_physical_table %d is not associated with an exact match memory.\n",
                     dbal_physical_table_id);
    }

    SHR_IF_ERR_EXIT(soc_mem_read(unit, tid_atr, SOC_BLOCK_ALL, app_id, data));

    soc_mem_field_get(unit, tid_atr, data, dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->field_5,
                      key_size);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_addr_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *addr_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    *addr_bits = dnx_data_mdb.em.entry_addr_bits_get(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 *vmv_size,
    uint8 *vmv_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding, vmv_size));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.value.get(unit, dbal_physical_table_id, entry_encoding, vmv_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_set_vmv_regs(
    int unit,
    uint8 vmv_value,
    uint8 vmv_size,
    uint8 vmv_total_nof_bits,
    uint32 encoding_value,
    uint32 encoding_nof_bits,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data)
{
    uint8 vmv_value_iter;

    SHR_FUNC_INIT_VARS(unit);

    if (vmv_encoding_data != NULL)
    {
        for (vmv_value_iter = vmv_value;
             vmv_value_iter < vmv_value + (1 << (vmv_total_nof_bits - vmv_size)); vmv_value_iter++)
        {

            SHR_BITCOPY_RANGE(vmv_encoding_data, encoding_nof_bits * vmv_value_iter,
                              &encoding_value, 0, encoding_nof_bits);

        }
    }
    if (vmv_size_data != NULL)
    {
        uint32 vmv_size_32 = vmv_size;
        uint32 vmv_init_value = vmv_value >> (dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_total_nof_bits);
        for (vmv_value_iter = vmv_init_value;
             vmv_value_iter < vmv_init_value + (1 << (vmv_total_nof_bits - vmv_size)); vmv_value_iter++)
        {

            SHR_BITCOPY_RANGE(vmv_size_data, dnx_data_mdb.em.nof_vmv_size_nof_bits_get(unit) * vmv_value_iter,
                              &vmv_size_32, 0, dnx_data_mdb.em.nof_vmv_size_nof_bits_get(unit));
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_set_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 vmv_size,
    uint8 vmv_value)
{
    uint8 vmv_value_iter;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_encoding >= MDB_EM_NOF_ENTRY_ENCODINGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid entry encoding %d.\n", entry_encoding);
    }

    for (vmv_value_iter = vmv_value;
         vmv_value_iter < vmv_value + (1 << (dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size)); vmv_value_iter++)
    {
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.encoding_map.
                        encoding.set(unit, dbal_physical_table_id, vmv_value_iter, entry_encoding));
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.encoding_map.
                        size.set(unit, dbal_physical_table_id, vmv_value_iter, vmv_size));
    }

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.value.set(unit, dbal_physical_table_id, entry_encoding, vmv_value));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.size.set(unit, dbal_physical_table_id, entry_encoding, vmv_size));

    {
        int stage, stage_index;
        for (stage = 0; stage < MDB_EM_SHIFT_VMV_STAGES; stage++)
        {
            soc_reg_t shift_vmv_reg = INVALIDr;

            switch (stage)
            {
                case MDB_EM_SHIFT_VMV_FLP:
                    shift_vmv_reg = IPPB_FLPLE_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_VTT_1:
                    shift_vmv_reg = IPPF_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_VTT_2_3:
                    shift_vmv_reg = IPPF_VTT_ST_TWO_LE_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_VTT_4_5:
                    shift_vmv_reg = IPPA_VTT_ST_TH_LE_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_IOAM:
                    shift_vmv_reg = IPPB_OAM_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_EOAM:
                    shift_vmv_reg = ETPPC_OAM_VMV_SHIFT_VALUE_MAPr;
                    break;
                case MDB_EM_SHIFT_VMV_RMEP:
                    shift_vmv_reg = INVALIDr;
                    break;
                case MDB_EM_SHIFT_VMV_ESEM_1:
                    shift_vmv_reg = ETPPC_CFG_ESEM_1_SHIFT_VALr;
                    break;
                case MDB_EM_SHIFT_VMV_ESEM_2:
                    shift_vmv_reg = ETPPC_CFG_ESEM_2_SHIFT_VALr;
                    break;
                case MDB_EM_SHIFT_VMV_ESEM_3:
                    shift_vmv_reg = ETPPC_CFG_ESEM_3_SHIFT_VALr;
                    break;
            }

            for (stage_index = 0; stage_index < dnx_data_mdb.em.shift_vmv_max_size_get(unit); stage_index++)
            {
                dbal_physical_tables_e stage_dbal_table_id;

                SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.get(unit, stage, stage_index, &stage_dbal_table_id));

                if ((stage_dbal_table_id == dbal_physical_table_id) && (shift_vmv_reg != INVALIDr))
                {
                    soc_reg_above_64_val_t data_above_64;

                    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                                    (unit, shift_vmv_reg, REG_PORT_ANY, stage_index, data_above_64));

                    SHR_IF_ERR_EXIT(mdb_em_set_vmv_regs
                                    (unit, vmv_value, vmv_size, dnx_data_mdb.em.max_nof_vmv_size_get(unit),
                                     entry_encoding, MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS, NULL, data_above_64));

                    SHR_IF_ERR_EXIT(soc_reg_above_64_set
                                    (unit, shift_vmv_reg, REG_PORT_ANY, stage_index, data_above_64));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_em_entry_extract_payload(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    soc_mem_t mem,
    soc_reg_above_64_val_t data,
    uint32 key_size,
    uint32 payload_size,
    uint32 *payload,
    uint32 *found,
    mdb_em_entry_encoding_e * entry_encoding_output)
{
    soc_field_info_t *field_info;
    uint32 entry_size;
    uint32 temp_total_size;
    uint32 zero_buffer = 0;
    soc_reg_above_64_val_t entry;
    uint8 vmv_size;
    uint8 vmv_value;
    uint32 entry_vmv_value;
    uint32 entry_vmv_value_full;
    int addr_bits;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_ENTRY_ENCODING_ONE;

    SHR_FUNC_INIT_VARS(unit);

    SOC_FIND_FIELD(ENTRYf, SOC_MEM_INFO(unit, mem).fields, SOC_MEM_INFO(unit, mem).nFields, field_info);
    entry_size = field_info->len;
    *entry_encoding_output = MDB_EM_NOF_ENTRY_ENCODINGS;

    soc_mem_field_get(unit, mem, data, ENTRYf, entry);
    if (mem == MACT_CPU_REQUESTm)
    {
        soc_mem_field_get(unit, mem, data, SUCCESSf, found);
    }
    else
    {
        soc_mem_field_get(unit, mem, data, FOUNDf, found);
    }

    sal_memset(payload, 0, sizeof(uint32) * BITS2WORDS(payload_size));

    SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &addr_bits));

    while (entry_encoding < MDB_EM_ENTRY_ENCODING_EMPTY)
    {
        temp_total_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width >> entry_encoding;
        zero_buffer = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width - temp_total_size;

        SHR_IF_ERR_EXIT(mdb_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

        if (vmv_size != 0)
        {
            entry_vmv_value = 0;
            SHR_BITCOPY_RANGE(&entry_vmv_value, dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size, entry,
                              entry_size - zero_buffer - vmv_size, vmv_size);
            entry_vmv_value_full = 0;
            SHR_BITCOPY_RANGE(&entry_vmv_value_full, 0, entry,
                              entry_size - zero_buffer - dnx_data_mdb.em.max_nof_vmv_size_get(unit),
                              dnx_data_mdb.em.max_nof_vmv_size_get(unit));

            if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
            {
                vmv_size = 0;
            }

            if ((entry_vmv_value != 0) && (entry_size - zero_buffer - vmv_size > key_size))
            {
                uint32 payload_offset = 0;
                if (entry_size - zero_buffer - vmv_size < payload_size)
                {
                    payload_offset = payload_size - (entry_size - zero_buffer - vmv_size);
                }

                SHR_BITCOPY_RANGE(payload, payload_offset, entry,
                                  entry_size + payload_offset - zero_buffer - vmv_size - payload_size,
                                  payload_size - payload_offset);

                SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.encoding_map.
                                encoding.get(unit, dbal_physical_table_id, entry_vmv_value_full,
                                             (uint8 *) entry_encoding_output));

                break;
            }
        }

        entry_encoding++;
    }

    if (entry_encoding == MDB_EM_ENTRY_ENCODING_EMPTY)
    {
        if (*found)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unable to parse entry encoding of entry.\n");
        }
    }
    else
    {
        *found = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_prepare_entry(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    soc_mem_t mem,
    uint32 *key,
    uint32 key_size,
    uint32 *payload,
    uint32 payload_size,
    soc_reg_above_64_val_t entry)
{
    soc_field_info_t *field_info;
    uint32 entry_size;
    uint32 temp_total_size;
    uint32 zero_buffer = 0;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    mdb_em_entry_encoding_e entry_encoding;
    uint8 vmv_size;
    uint8 vmv_value;
    uint8 app_id_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_physical_table == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. DBAL physical table %d is not associated with an MDB table.\n", dbal_physical_table_id);
    }

    SOC_FIND_FIELD(ENTRYf, SOC_MEM_INFO(unit, mem).fields, SOC_MEM_INFO(unit, mem).nFields, field_info);
    entry_size = field_info->len;

    if (key_size + payload_size > entry_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s, app_id %d: key_size(%d) + payload_size(%d) > entry_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, key_size, payload_size,
                     entry_size);
    }

    sal_memset(entry, 0x0, sizeof(soc_reg_above_64_val_t));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, dbal_physical_table_id, key_size, payload_size, app_id, app_id_size, &entry_encoding));

    SHR_IF_ERR_EXIT(mdb_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

    if (mem == MACT_CPU_REQUESTm)
    {
        zero_buffer = 0;
        vmv_size = dnx_data_mdb.em.max_nof_vmv_size_get(unit);
    }
    else
    {

        temp_total_size =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);
        zero_buffer = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width - temp_total_size;
    }

    if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
    {
        vmv_size = 0;
    }
    if (key_size + payload_size + zero_buffer + vmv_size > entry_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, app_id %d: key_size(%d) + payload_size(%d) + zero_buffer(%d) + vmv_size(%d) > entry_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, key_size, payload_size,
                     zero_buffer, vmv_size, entry_size);
    }

    if (payload != NULL)
    {
        uint32 vmv_value_32 = vmv_value;
        SHR_BITCOPY_RANGE(entry, entry_size - zero_buffer - vmv_size - payload_size, payload, 0, payload_size);
        SHR_BITCOPY_RANGE(entry, entry_size - zero_buffer - vmv_size, &vmv_value_32,
                          dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size, vmv_size);
    }
    SHR_BITCOPY_RANGE(entry, 0, key, 0, key_size);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_em_prepare_command(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    mdb_em_cmd_e command,
    soc_mem_t mem,
    uint32 *key,
    uint32 key_size,
    uint32 *payload,
    uint32 payload_size,
    soc_reg_above_64_val_t data,
    uint32 *stamp)
{
    soc_reg_above_64_val_t entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0x0, sizeof(soc_reg_above_64_val_t));
    sal_memset(entry, 0, sizeof(entry));

    SHR_IF_ERR_EXIT(mdb_em_prepare_entry
                    (unit, dbal_physical_table_id, app_id, mem, key, key_size, payload, payload_size, entry));

    soc_mem_field_set(unit, mem, data, ENTRYf, entry);

    if (mem == MACT_CPU_REQUESTm)
    {
        soc_reg_above_64_val_t reply_data;
        uint32 translated_cmd;
        uint32 reply_stamp;

        SHR_IF_ERR_EXIT(READ_MACT_LARGE_EM_REPLYr(unit, MACT_BLOCK(unit), reply_data));

        soc_mem_field_get(unit, MACT_CPU_REQUESTm, reply_data, STAMPf, &reply_stamp);
        reply_stamp = (reply_stamp + 1) & MDB_EM_MACT_STAMP_MAX;
        if (reply_stamp == 0)
        {
            reply_stamp++;
        }
        *stamp = reply_stamp;

        if (command == MDB_EM_CMD_INSERT)
        {
            translated_cmd = MDB_EM_MACT_CMD_INSERT;
        }
        else if (command == MDB_EM_CMD_TRANSPLANT)
        {
            translated_cmd = MDB_EM_MACT_CMD_TRANSPLANT;
        }
        else if (command == MDB_EM_CMD_DELETE)
        {
            translated_cmd = MDB_EM_MACT_CMD_DELETE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid EM command 0x%x.\n", command);
        }

        soc_mem_field_set(unit, mem, data, COMMANDf, &translated_cmd);
        soc_mem_field_set(unit, mem, data, APP_DBf, &app_id);
        soc_mem_field_set(unit, mem, data, STAMPf, &reply_stamp);
    }
    else
    {
        soc_mem_field_set(unit, mem, data, COMMANDf, &command);
        soc_mem_field_set(unit, mem, data, TIDf, &app_id);

        if (!dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_entry_type_parser))
        {
            uint32 entry_type;
            uint8 app_id_size = 0;
            SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.
                            app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

            SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                            (unit, dbal_physical_table_id, key_size, payload_size, app_id, app_id_size, &entry_type));

            soc_mem_field_set(unit, mem, data, ENTRY_TYPEf, &entry_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_physical_memory(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 payload_size,
    soc_mem_t * mdb_physical_memory,
    soc_reg_t * mdb_physical_status_reg,
    int *blk,
    soc_reg_t * mdb_interrupt_reg,
    soc_field_t * interrupt_field)
{
    int enable_mact = TRUE;
    uint8 is_mact;
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    enable_mact = FALSE;
#endif

    SHR_IF_ERR_EXIT(mdb_em_is_mact_application(unit, dbal_physical_table_id, app_id, &is_mact));

    if (is_mact && (enable_mact == TRUE) && (payload_size <= dnx_data_mdb.em.mact_max_payload_size_get(unit)))
    {
        *mdb_physical_memory = MACT_CPU_REQUESTm;
        *mdb_physical_status_reg = INVALIDr;
        *mdb_interrupt_reg = MACT_INTERRUPT_REGISTERr;
        *interrupt_field = LOCAL_MACT_INTf;
        *blk = MACT_BLOCK(unit);
    }
    else
    {
        *mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;
        *mdb_physical_status_reg = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg;
        *mdb_interrupt_reg = dnx_data_mdb.em.interrupt_register_get(unit);
        *interrupt_field = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->interrupt_field;
        *blk = MDB_BLOCK(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_entry_check_status(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    soc_reg_t mdb_physical_status_reg,
    soc_reg_t mdb_interrupt_reg,
    soc_field_t interrupt_field,
    mdb_em_cmd_e command,
    uint32 stamp)
{
    shr_error_e res;

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_physical_status_reg == INVALIDr)
    {
        uint32 fail_reason;

        uint32 success;
        uint32 reply_command = MDB_NOF_EM_MACT_CMD;
        uint32 reply_stamp = 0;
        soc_reg_above_64_val_t data;
        uint32 attempt_counter = 0;

        if (command == MDB_EM_CMD_TRANSPLANT)
        {
            SHR_EXIT();
        }

        while (((reply_command != MDB_EM_MACT_CMD_ACK) || (reply_stamp != stamp))
               && (attempt_counter < MDB_EM_MACT_POLL_ATTEMPTS))
        {
            SHR_IF_ERR_EXIT(READ_MACT_LARGE_EM_REPLYr(unit, MACT_BLOCK(unit), data));

            soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, COMMANDf, &reply_command);

            soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, STAMPf, &reply_stamp);

            attempt_counter++;
        }

        if (attempt_counter >= MDB_EM_MACT_POLL_ATTEMPTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MACT reply is not ready within the maximal number of polling attempts.\n");
        }

        soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, SUCCESSf, &success);

        if (success != TRUE)
        {
            soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, FAIL_REASONf, &fail_reason);

            if ((fail_reason == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                || (fail_reason == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC)
                || (fail_reason == MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED)
                || (fail_reason == MDB_EM_FAIL_REASON_EXCEED_LIMIT) || (fail_reason == MDB_EM_FAIL_REASON_FLUSH_DROP))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "%s MACT entry operation failed with: %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_em_fail_reason_strings[fail_reason]);
            }
            else if (fail_reason == MDB_EM_FAIL_REASON_DELETE_UNKNOWN_KEY)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else if (fail_reason == MDB_EM_FAIL_REASON_INSERTED_EXISTING)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "%s attempted to insert existing key to MACT.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s MACT entry operation failed with: %s, command: %d, success: %d.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_em_fail_reason_strings[fail_reason], reply_command, success);
            }
        }

        if (command == MDB_EM_CMD_INSERT)
        {
            dbal_physical_entry_t entry_duplicate;
            sal_memcpy(&entry_duplicate, entry, sizeof(*entry));
            entry_duplicate.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate);
            if (res == _SHR_E_NOT_FOUND)
            {
                fail_reason = MDB_EM_FAIL_REASON_CAM_TABLE_FULL;
                SHR_ERR_EXIT(_SHR_E_FULL, "%s MACT entry operation failed with: %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_em_fail_reason_strings[fail_reason]);
            }
            else if (res != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(res);
                SHR_EXIT();
            }
        }
    }
    else
    {
        uint32 interrupt_value;
        uint32 interrupt_reg_data;
        uint64 status_reg_data;
        mdb_em_fail_reason_e em_status;

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, mdb_interrupt_reg, MDB_BLOCK(unit), 0, &interrupt_reg_data));
        interrupt_value = soc_reg_field_get(unit, mdb_interrupt_reg, interrupt_reg_data, interrupt_field);

        if (interrupt_value != 0)
        {
            int relevant_interrupt = TRUE;

            SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, MDB_BLOCK(unit), 0, &status_reg_data));
            em_status = soc_reg64_field_get(unit, mdb_physical_status_reg, status_reg_data, ITEM_0_3f);

            if (command == MDB_EM_CMD_INSERT)
            {
                if ((em_status == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                    || (em_status == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC)
                    || (em_status == MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED))
                {
                    res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, entry);
                    if (res == _SHR_E_NONE)
                    {
                        relevant_interrupt = FALSE;
                    }
                }
                else if (em_status == MDB_EM_FAIL_REASON_DELETE_UNKNOWN_KEY)
                {
                    relevant_interrupt = FALSE;
                }
            }
            else if (command == MDB_EM_CMD_DELETE)
            {
                if ((em_status == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                    || (em_status == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC))
                {
                    relevant_interrupt = FALSE;
                }
            }

            if (relevant_interrupt == TRUE)
            {
                interrupt_reg_data = 0;
                soc_reg_field_set(unit, mdb_interrupt_reg, &interrupt_reg_data, interrupt_field, interrupt_value);
                SHR_IF_ERR_EXIT(soc_reg_set(unit, mdb_interrupt_reg, MDB_BLOCK(unit), 0, interrupt_reg_data));

                if ((em_status == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                    || (em_status == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC)
                    || (em_status == MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED))
                {
                    SHR_ERR_EXIT(_SHR_E_FULL, "%s is full.\n",
                                 dbal_physical_table_to_string(unit, dbal_physical_table_id));
                }
                else if (em_status == MDB_EM_FAIL_REASON_DELETE_UNKNOWN_KEY)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                    SHR_EXIT();
                }
                else if (em_status == MDB_EM_FAIL_REASON_INSERTED_EXISTING)
                {
                    SHR_ERR_EXIT(_SHR_E_EXISTS, "%s attempted to insert existing key.\n",
                                 dbal_physical_table_to_string(unit, dbal_physical_table_id));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s entry operation failed with: %s.\n",
                                 dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                 mdb_em_fail_reason_strings[em_status]);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    soc_reg_above_64_val_t data;
    int blk;
    bsl_severity_t severity;
    uint32 key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, entry->key_size);
    soc_mem_t mdb_physical_memory = INVALIDm;
    soc_reg_t mdb_physical_status_reg;
    mdb_em_cmd_e command;
    uint32 rv;
    soc_reg_t mdb_interrupt_reg;
    soc_field_t interrupt_field;
    uint32 stamp = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
        || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
    {
        SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                        (unit, dbal_physical_table_id, app_id, entry->payload_size, &mdb_physical_memory,
                         &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

#ifndef ADAPTER_SERVER_MODE
        command = entry->is_update ? (uint32) MDB_EM_CMD_TRANSPLANT : (uint32) MDB_EM_CMD_INSERT;
#else
        command = (uint32) MDB_EM_CMD_INSERT;
#endif

        SHR_IF_ERR_EXIT(mdb_em_prepare_command
                        (unit, dbal_physical_table_id, app_id, command,
                         mdb_physical_memory, entry->key, key_size, entry->payload, entry->payload_size, data, &stamp));

#ifdef MDB_FPP_DUMPS
        {
            uint32 data_offset;
            uint32 print_index;
            dbal_tables_e logical_table;

            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, dbal_physical_table_id, app_id, &logical_table));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "********************MDB-LOGIC-START********************\n")));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "//DB(%s)(%s),\n`MDBT_SUBENV.write_em_key(.em_pdb_type(%s), .key('h"),
                         dbal_logical_table_to_string(unit, logical_table), dbal_physical_table_to_string(unit,
                                                                                                          dbal_physical_table_id),
                         dbal_physical_table_to_string(unit, dbal_physical_table_id)));
            for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
            {
                print_index = BITS2WORDS(key_size) - 1 - data_offset;
                if (data_offset == 0)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%X"), entry->key[print_index]));
                }
                else
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry->key[print_index]));
                }
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "), .tid(%d), .ad('h"), app_id));
            for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size); data_offset++)
            {
                print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
                if (data_offset == 0)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%X"), entry->payload[print_index]));
                }
                else
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry->payload[print_index]));
                }
            }

            {
                mdb_em_entry_encoding_e entry_encoding;
                uint8 vmv_size = 0, vmv_value = 0;
                uint8 app_id_size = 0;

                SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.
                                app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

                SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                (unit, dbal_physical_table_id, key_size, entry->payload_size, app_id, app_id_size,
                                 &entry_encoding));

                SHR_IF_ERR_EXIT(mdb_get_vmv_size_value
                                (unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "), .payload_size(%d), .vmv_size(%d), .vmv(%d));\n"), entry->payload_size,
                             vmv_size, vmv_value));
            }

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-LOGIC-END********************\n")));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "********************MDB-PEMLA-START********************\n")));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "PhyDB(%s), AppDbId(%d), DB(%s),key(0x"),
                         dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id,
                         dbal_logical_table_to_string(unit, logical_table)));
            for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
            {
                print_index = BITS2WORDS(key_size) - 1 - data_offset;
                if (data_offset == 0)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%X"), entry->key[print_index]));
                }
                else
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry->key[print_index]));
                }
            }
            {
                mdb_em_entry_encoding_e entry_encoding;
                uint8 vmv_size = 0, vmv_value = 0, app_id_size = 0;
                uint32 vmv_value_32 = 0;
                int index;
                uint32 payload_with_vmv[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };

                SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.
                                app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

                SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                (unit, dbal_physical_table_id, key_size, entry->payload_size, app_id,
                                 app_id_size, &entry_encoding));
                SHR_IF_ERR_EXIT(mdb_get_vmv_size_value
                                (unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

                for (index = 0; index < DBAL_PHYSICAL_RES_SIZE_IN_WORDS; index++)
                {
                    payload_with_vmv[index] = entry->payload[index];
                }
                vmv_value_32 = vmv_value;
                if ((dbal_physical_table_id != DBAL_PHYSICAL_TABLE_PPMC)
                    && (dbal_physical_table_id != DBAL_PHYSICAL_TABLE_GLEM_1)
                    && (dbal_physical_table_id != DBAL_PHYSICAL_TABLE_GLEM_2))
                {
                    SHR_BITCOPY_RANGE(payload_with_vmv, entry->payload_size, &vmv_value_32,
                                      dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size, vmv_size);
                }
                else
                {
                    vmv_size = 0;
                }
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "), payload(0x")));

                for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size + vmv_size); data_offset++)
                {
                    print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
                    if (data_offset == 0)
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%X"), payload_with_vmv[print_index]));
                    }
                    else
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), payload_with_vmv[print_index]));
                    }
                }

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "), payload_size(%d)\n"), entry->payload_size + vmv_size));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-PEMLA-END********************\n")));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-START********************\n")));
#endif

        rv = sand_fill_partial_table_with_entry(unit, mdb_physical_memory, 0, 0, blk, 0, 0, data);
#ifdef MDB_FPP_DUMPS
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-END********************\n")));
#endif
        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "sand_fill_partial_table_with_entry failed in mdb_em_entry_add, %s is full.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }

        SHR_IF_ERR_EXIT(mdb_entry_check_status
                        (unit, dbal_physical_table_id, app_id, entry, mdb_physical_status_reg, mdb_interrupt_reg,
                         interrupt_field, command, stamp));
    }

    if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
        || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW))
    {
        SHR_IF_ERR_EXIT(mdb_em_shadow_add_key(unit, dbal_physical_table_id, app_id, entry));
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if ((severity >= bslSeverityVerbose) && (mdb_physical_memory != INVALIDm))
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                     key_size, entry->payload_size, dbal_physical_table_to_string(unit,
                                                                                  dbal_physical_table_id), app_id));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->mdb_action_apply: %d.\n"), entry->mdb_action_apply));
        for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
        {
            print_index = BITS2WORDS(key_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size); data_offset++)
        {
            print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->payload[%d]: %08X.\n"), print_index, entry->payload[print_index]));
        }
        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Written to memory: %s.\n"), SOC_MEM_NAME(unit, mdb_physical_memory)));
            for (data_offset = 0; data_offset < BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes);
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Written data[%d]: %08X.\n"), print_index, data[print_index]));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_add: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    soc_reg_above_64_val_t write_data, read_data;
    int blk = MDB_BLOCK(unit);
    bsl_severity_t severity;
    uint32 found = 0;
    uint32 key_size;
    soc_mem_t mdb_physical_memory = INVALIDm;
    uint32 stamp = 0;
    uint32 retrieve_hitbit_or_age = ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
                                     || (entry->age & DBAL_PHYSICAL_KEY_AGE_GET));
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_ENTRY_ENCODING_ONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, entry->key_size);

    mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;

    if (mdb_physical_memory == INVALIDm)
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "Error. dbal_physical_table %s is not associated with an exact match memory.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    
    if (SOC_IS_JERICHO_2_B0(unit) || SOC_IS_J2C(unit))
    {
        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW) && !retrieve_hitbit_or_age)
        {
            SHR_IF_ERR_EXIT(mdb_em_shadow_get(unit, dbal_physical_table_id, app_id, entry, &found));
        }
    }
    else
    {
        if (((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
             || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)) && !retrieve_hitbit_or_age)
        {
            SHR_IF_ERR_EXIT(mdb_em_shadow_get(unit, dbal_physical_table_id, app_id, entry, &found));
        }
    }

    if (((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY) ||
         ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) && (!found))) || retrieve_hitbit_or_age)
    {
        SHR_IF_ERR_EXIT(mdb_em_prepare_command
                        (unit, dbal_physical_table_id, app_id, (uint32) MDB_EM_CMD_LOOKUP,
                         mdb_physical_memory, entry->key, key_size, NULL, entry->payload_size, write_data, &stamp));

        SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, write_data));

        SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, read_data));
        SHR_IF_ERR_EXIT(mdb_em_entry_extract_payload
                        (unit, dbal_physical_table_id, mdb_physical_memory,
                         read_data, key_size, entry->payload_size, entry->payload, &found, &entry_encoding));
    }

#ifndef ADAPTER_SERVER_MODE
    if (found && retrieve_hitbit_or_age)
    {
        uint32 way, prefix, address;
        uint32 way_post, prefix_post, address_post;
        uint32 attempt_counter = 0;

        /*
         * Retrieve the way, prefix, address
         */
        soc_mem_field_get(unit, mdb_physical_memory, read_data, WAY_INDEXf, &way);
        soc_mem_field_get(unit, mdb_physical_memory, read_data, PREFIX_NUMf, &prefix);
        soc_mem_field_get(unit, mdb_physical_memory, read_data, ADDRESSf, &address);

        do
        {
            attempt_counter++;
            /*
             * Retrieve the age
             */
            if (entry->age & DBAL_PHYSICAL_KEY_AGE_GET)
            {
                soc_mem_t em_age_mem = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->age_mem;
                soc_mem_t em_age_ovf_cam_mem =
                    dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->age_ovf_cam_mem;
                uint32 ovf_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
                uint32 ovf_pos;
                uint32 entry_handle_mdb_24_table;
                uint32 age_cnt_em_row_width;
                uint32 age_cnt_em_entry_width;
                uint32 age_cnt_em_base_width;
                uint32 bank_start_ptr;
                uint32 age_cnts_ratio;
                uint32 age_entry_address;
                soc_reg_above_64_val_t age_mem_row;
                uint32 em_entry_age_cnts;

                dbal_enum_value_field_mdb_physical_table_e mdb_physical_table =
                    dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

                ovf_pos =
                    dnx_data_mdb.dh.dh_info_get(unit,
                                                mdb_physical_table)->table_macro_interface_mapping[ovf_pos_in_array];

                if ((em_age_mem == INVALIDm) || (em_age_ovf_cam_mem == INVALIDm))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "MDB EM table %s does not support aging.\n",
                                 dbal_physical_table_to_string(unit, dbal_physical_table_id));
                }

                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit,
                                 dnx_data_mdb.em.mdb_24_info_get(unit, mdb_physical_table)->dbal_table,
                                 &entry_handle_mdb_24_table));

                dbal_value_field32_request(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_1, way,
                                           &bank_start_ptr);
                dbal_value_field32_request(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                                           &age_cnt_em_row_width);

                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_24_table, DBAL_COMMIT));

                if (way == ovf_pos)
                {
                    age_cnts_ratio = dnx_data_mdb.em.age_ovfcam_row_size_bits_get(unit) / age_cnt_em_row_width;

                    age_entry_address = address / age_cnts_ratio;

                    SHR_IF_ERR_EXIT(soc_mem_read(unit, em_age_ovf_cam_mem, blk, age_entry_address, age_mem_row));

                    em_entry_age_cnts = 0;
                    SHR_BITCOPY_RANGE(&em_entry_age_cnts, 0x0, age_mem_row,
                                      (age_entry_address % age_cnts_ratio) * age_cnt_em_row_width,
                                      age_cnt_em_row_width);
                }
                else
                {

                    age_cnts_ratio = dnx_data_mdb.em.age_row_size_bits_get(unit) / age_cnt_em_row_width;

                    age_entry_address = (bank_start_ptr * 8 + address) / age_cnts_ratio;

                    SHR_IF_ERR_EXIT(soc_mem_read(unit, em_age_mem, blk, age_entry_address, age_mem_row));

                    em_entry_age_cnts = 0;
                    SHR_BITCOPY_RANGE(&em_entry_age_cnts, 0x0, age_mem_row,
                                      (address % age_cnts_ratio) * age_cnt_em_row_width, age_cnt_em_row_width);
                }

                age_cnt_em_base_width =
                    age_cnt_em_row_width / (dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->row_width /
                                            MDB_DIRECT_BASIC_ENTRY_SIZE);
                age_cnt_em_entry_width = age_cnt_em_row_width / (1 << entry_encoding);
                age_cnt_em_entry_width =
                    age_cnt_em_entry_width >
                    dnx_data_mdb.em.age_entry_size_bits_get(unit) ? dnx_data_mdb.
                    em.age_entry_size_bits_get(unit) : age_cnt_em_entry_width;

                entry->age = 0;
                SHR_BITCOPY_RANGE(&entry->age, 0x0, &em_entry_age_cnts,
                                  ((prefix + 1) * age_cnt_em_base_width) - age_cnt_em_entry_width,
                                  age_cnt_em_entry_width);
            }

            /*
             * Retrieve the hitbit
             */
            if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
            {
                soc_reg_above_64_val_t hitbit_data;
                soc_reg_above_64_val_t hitbit_result;
                uint32 command, hits_a, hits_b;

                sal_memcpy(hitbit_data, read_data, sizeof(read_data));

                /*
                 * Issue a READ-HITS-A-BY-ADDR
                 */
                if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
                {
                    command = MDB_EM_CMD_READ_CLEAR_HITS_A_BY_ADDR;
                }
                else
                {
                    command = MDB_EM_CMD_READ_HITS_A_BY_ADDR;
                }
                soc_mem_field_set(unit, mdb_physical_memory, hitbit_data, COMMANDf, &command);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, hitbit_data));

                /*
                 * Retrieve the HIT for A
                 */
                SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, hitbit_result));
                soc_mem_field_get(unit, mdb_physical_memory, hitbit_result, HITSf, &hits_a);

                /*
                 * Use the prefix to retrieve the correct hitbit
                 */
                if (((hits_a >> prefix) & 0x1) == 1)
                {
                    entry->hitbit |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
                }
                else
                {
                    entry->hitbit &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
                }

                /*
                 * Issue a READ-HITS-B-BY-ADDR
                 */
                if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
                {
                    command = MDB_EM_CMD_READ_CLEAR_HITS_B_BY_ADDR;
                }
                else
                {
                    command = MDB_EM_CMD_READ_HITS_B_BY_ADDR;
                }
                soc_mem_field_set(unit, mdb_physical_memory, hitbit_data, COMMANDf, &command);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, hitbit_data));

                /*
                 * Retrieve the HIT for B
                 */
                SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, hitbit_result));
                soc_mem_field_get(unit, mdb_physical_memory, hitbit_result, HITSf, &hits_b);

                /*
                 * Use the prefix to retrieve the correct hitbit
                 */
                if (((hits_b >> prefix) & 0x1) == 1)
                {
                    entry->hitbit |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B;
                }
                else
                {
                    entry->hitbit &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B;
                }
            }

            SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, write_data));
            SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, read_data));

            soc_mem_field_get(unit, mdb_physical_memory, read_data, WAY_INDEXf, &way_post);
            soc_mem_field_get(unit, mdb_physical_memory, read_data, PREFIX_NUMf, &prefix_post);
            soc_mem_field_get(unit, mdb_physical_memory, read_data, ADDRESSf, &address_post);
        }
        while (((way_post != way) || (prefix_post != prefix) || (address_post != address))
               && (attempt_counter < MDB_EM_HITBIT_AGE_ATTEMPTS));

        if (attempt_counter >= MDB_EM_HITBIT_AGE_ATTEMPTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "EM age or hitbit retrieval was unsuccessful within the maximal number of attempts.\n");
        }
    }
#endif

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_get: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->mdb_action_apply: %d.\n"), entry->mdb_action_apply));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                     key_size, entry->payload_size, dbal_physical_table_to_string(unit,
                                                                                  dbal_physical_table_id), app_id));
        if (stamp != 0)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Read from memory: %s.\n"), SOC_MEM_NAME(unit, mdb_physical_memory)));
            for (data_offset = 0; data_offset < BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes);
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "write_data[%d]: %08X.\n"), print_index, write_data[print_index]));
            }
            for (data_offset = 0; data_offset < BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes);
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "read_data[%d]: %08X.\n"), print_index, read_data[print_index]));
            }
        }
        for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
        {
            print_index = BITS2WORDS(key_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size); data_offset++)
        {
            print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->payload[%d]: %08X.\n"), print_index, entry->payload[print_index]));
        }
        if (retrieve_hitbit_or_age)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->hitbit: 0x%x.\n"), entry->hitbit));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->age: 0x%x.\n"), entry->age));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_get: end\n")));
    }

    if (found == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    soc_reg_above_64_val_t write_data;
    int blk;
    bsl_severity_t severity;
    uint32 key_size;
    soc_mem_t mdb_physical_memory = INVALIDm;
    soc_reg_t mdb_physical_status_reg;
    soc_field_t interrupt_field;
    soc_reg_t mdb_interrupt_reg;
    uint32 stamp = 0;

    SHR_FUNC_INIT_VARS(unit);

    key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, entry->key_size);

    if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
        || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
    {
        SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                        (unit, dbal_physical_table_id, app_id, entry->payload_size, &mdb_physical_memory,
                         &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

        if (mdb_physical_memory == INVALIDm)
        {
            SHR_ERR_EXIT(_SHR_E_BADID,
                         "Error. dbal_physical_table %d is not associated with an exact match register.\n",
                         dbal_physical_table_id);
        }

        SHR_IF_ERR_EXIT(mdb_em_prepare_command
                        (unit, dbal_physical_table_id, app_id, (uint32) MDB_EM_CMD_DELETE,
                         mdb_physical_memory, entry->key, key_size, entry->payload, entry->payload_size, write_data,
                         &stamp));

        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, mdb_physical_memory, 0, 0, blk, 0, 0, write_data));

        SHR_IF_ERR_EXIT(mdb_entry_check_status
                        (unit, dbal_physical_table_id, app_id, entry, mdb_physical_status_reg, mdb_interrupt_reg,
                         interrupt_field, MDB_EM_CMD_DELETE, stamp));
    }

    if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
        || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW))
    {
        SHR_IF_ERR_EXIT(mdb_em_shadow_remove_key(unit, dbal_physical_table_id, app_id, entry));
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if ((severity >= bslSeverityVerbose) && (mdb_physical_memory != INVALIDm))
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_delete: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->mdb_action_apply: %d.\n"), entry->mdb_action_apply));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                     key_size, entry->payload_size, dbal_physical_table_to_string(unit,
                                                                                  dbal_physical_table_id), app_id));
        for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
        {
            print_index = BITS2WORDS(key_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Deleted from memory: %s.\n"), SOC_MEM_NAME(unit, mdb_physical_memory)));
        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
        {
            for (data_offset = 0; data_offset < (BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes));
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Written data[%d]: %08X.\n"), print_index, write_data[print_index]));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_delete: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    dbal_tables_e dbal_logical_table_id = DBAL_TABLE_EMPTY;
    const dbal_logical_table_t *dbal_logical_table;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HASH_TABLE_ITER_SET_BEGIN(&(physical_entry_iterator->mdb_em_htb_iter));

    SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, dbal_physical_table_id, app_id, &dbal_logical_table_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_logical_table_id, &dbal_logical_table));

    if (dbal_logical_table_id != DBAL_TABLE_EMPTY)
    {
        key_size = dbal_logical_table->key_size;
        if (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC)
        {
            key_size -= dbal_logical_table->core_id_nof_bits;
        }
        physical_entry_iterator->mdb_em_key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, key_size);

        physical_entry_iterator->mdb_em_payload_size = dbal_logical_table->max_payload_size;
        if (dbal_logical_table->result_type_mapped_to_sw)
        {
            physical_entry_iterator->mdb_em_payload_size -=
                dbal_logical_table->multi_res_info[0].results_info[0].field_nof_bits;
        }
    }
    else
    {
        physical_entry_iterator->mdb_em_key_size = 0;
        physical_entry_iterator->mdb_em_payload_size = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    uint8 entry_was_found = FALSE;
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    uint8 shadow_payload[MDB_EM_MAX_PAYLOAD_IN_BYTE];
    uint32 key_size_byte, app_id_offset;
    SHR_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_em_key_size == 0)
    {
        *is_end = TRUE;
        SHR_EXIT();
    }

    sal_memset(entry, 0x0, sizeof(*entry));
    sal_memset(shadow_payload, 0x0, sizeof(shadow_payload));

    key_size_byte =
        BITS2BYTES(dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_key_size +
                   dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_size);
    app_id_offset = SAL_UINT8_NOF_BITS - dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_size;

    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&(physical_entry_iterator->mdb_em_htb_iter)) && entry_was_found == FALSE)
    {
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                        shadow_em_db.get_next(unit, dbal_physical_table_id, &(physical_entry_iterator->mdb_em_htb_iter),
                                              shadow_key, shadow_payload));

        if (!SW_STATE_HASH_TABLE_ITER_IS_END(&(physical_entry_iterator->mdb_em_htb_iter))
            && (shadow_key[key_size_byte - 1] >> app_id_offset) == app_id)
        {
            entry->key_size = physical_entry_iterator->mdb_em_key_size;
            entry->payload_size = physical_entry_iterator->mdb_em_payload_size;
            entry->hitbit = physical_entry_iterator->hit_bit_flags;
            entry->age = physical_entry_iterator->age_flags;
            SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_key, BITS2BYTES(entry->key_size), entry->key));
            /*
             * In case there is a hit bit or age request the HW must be accessed.
             */
            if ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION) || (entry->age & DBAL_PHYSICAL_KEY_AGE_GET))
            {
                SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, entry));
            }
            else
            {
                uint32 payload_align[BYTES2WORDS(MDB_EM_MAX_PAYLOAD_IN_BYTE)];
                shadow_key[key_size_byte - 1] &=
                    0xFF >> dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_size;

                SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_payload, MDB_EM_MAX_PAYLOAD_IN_BYTE, payload_align));
                SHR_BITCOPY_RANGE(entry->payload, 0x0, payload_align,
                                  BYTES2BITS(MDB_EM_MAX_PAYLOAD_IN_BYTE) - entry->payload_size, entry->payload_size);
            }
            entry_was_found = TRUE;
        }
    }

    *is_end = (SW_STATE_HASH_TABLE_ITER_IS_END(&(physical_entry_iterator->mdb_em_htb_iter))
               && (entry_was_found == FALSE)) ? TRUE : FALSE;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    physical_entry_iterator->mdb_em_htb_iter = UTILEX_U32_MAX;

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id)
{
    uint8 is_end = FALSE;
    uint32 entries_to_delete_itr, valid_entries_count = 0;
    dbal_physical_entry_t *entries = NULL;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    dbal_physical_entry_t empty_entry;

    SHR_FUNC_INIT_VARS(unit);

    entries =
        (dbal_physical_entry_t *) sal_alloc(sizeof(dbal_physical_entry_t) * MDB_EM_SHADOW_HASH_NOF_RESOURCES,
                                            "MDB EM entries clear");
    if (entries == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate memory for entry buffer.\n");
    }

    sal_memset(&empty_entry, 0, sizeof(dbal_physical_entry_t));
    SHR_IF_ERR_EXIT(dbal_tables_sizes_by_mdb_phy_get(unit, dbal_physical_table_id, app_id,
                                                     (int *) (&empty_entry.key_size),
                                                     (int *) (&empty_entry.payload_size)));

    empty_entry.key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, empty_entry.key_size);
    while (is_end == FALSE)
    {
        valid_entries_count = 0;
        SHR_IF_ERR_EXIT(mdb_em_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
        while ((is_end == FALSE) & (valid_entries_count < MDB_EM_SHADOW_HASH_NOF_RESOURCES))
        {
            sal_memcpy(&entries[valid_entries_count], &empty_entry, sizeof(dbal_physical_entry_t));
            SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                            (unit, dbal_physical_table_id, app_id, &physical_entry_iterator,
                             &entries[valid_entries_count], &is_end));
            if (is_end == FALSE)
            {
                valid_entries_count++;
            }
        }

        SHR_IF_ERR_EXIT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
        for (entries_to_delete_itr = 0; entries_to_delete_itr < valid_entries_count; entries_to_delete_itr++)
        {
            mdb_em_entry_delete(unit, dbal_physical_table_id, app_id, &entries[entries_to_delete_itr]);
        }
    }

exit:
    if (entries != NULL)
    {
        sal_free(entries);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_test_fill_entry(
    int unit,
    int index,
    CONST dbal_logical_table_t * dbal_logical_table,
    int fill_payload,
    dbal_physical_entry_t * entry)
{
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[0];
    uint32 app_id = dbal_logical_table->app_id;

    SHR_FUNC_INIT_VARS(unit);

    entry->key[0] = index;

    if (fill_payload == TRUE)
    {
        uint32 uint32_counter;

        for (uint32_counter = 0;
             uint32_counter < ((entry->payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS); uint32_counter++)
        {
            entry->payload[uint32_counter] = 0xFFFFFFFF - index;
        }
    }
    else
    {
        sal_memset(entry->payload, 0x0, sizeof(entry->payload));
    }

    if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
    {
        uint32 vmv_value_32;
        mdb_em_entry_encoding_e entry_encoding;
        uint8 vmv_size;
        uint8 vmv_value;

        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, dbal_physical_table_id, entry->key_size, entry->payload_size, app_id,
                         dbal_logical_table->app_id_size, &entry_encoding));

        SHR_IF_ERR_EXIT(mdb_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

        vmv_value_32 = vmv_value;

        SHR_BITCOPY_RANGE(entry->payload, entry->payload_size - vmv_size, &vmv_value_32,
                          dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size, vmv_size);
    }

    if ((((entry->payload_size + SAL_UINT32_NOF_BITS -
           1) / SAL_UINT32_NOF_BITS) * SAL_UINT32_NOF_BITS) != entry->payload_size)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                        (entry->payload, entry->payload_size,
                         (((entry->payload_size + SAL_UINT32_NOF_BITS -
                            1) / SAL_UINT32_NOF_BITS) * SAL_UINT32_NOF_BITS) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_test_table(
    int unit,
    CONST dbal_logical_table_t * dbal_logical_table,
    mdb_test_mode_e mode,
    int delete_cmd)
{
    dbal_physical_entry_t entry, entry_duplicate;
    int max_ext_iterations;
    int entry_index;
    int uint32_counter;
    uint32 app_id;
    shr_error_e res;
    uint32 key_size;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[0];
    shr_error_e rv;
    mdb_em_entry_encoding_e entry_encoding;
    uint32 entry_hw_size;
    int blk;
    soc_mem_t mdb_physical_memory;
    soc_reg_t mdb_physical_status_reg = INVALIDr;
    uint32 total_time_usecs = 0;
    uint32 total_entry_count = 0;

    int ext_iterator = 0, int_iterator = 0;
    int max_capacity;
    soc_field_t interrupt_field;
    soc_reg_t mdb_interrupt_reg;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(entry));
    if (dbal_logical_table->result_type_mapped_to_sw)
    {
        entry.payload_size =
            dbal_logical_table->max_payload_size - dbal_logical_table->multi_res_info[0].results_info[0].field_nof_bits;
    }
    else
    {
        entry.payload_size = dbal_logical_table->max_payload_size;
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

    key_size = dbal_logical_table->key_size;
    if (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC)
    {
        key_size -= dbal_logical_table->core_id_nof_bits;
    }
    key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, key_size);
    entry.key_size = key_size;

    if (dbal_logical_table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_LEM)
    {
        dbal_tables_e dbal_logical_table_id;

        SHR_FUNC_INIT_VARS(unit);

        SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get
                        (unit, dbal_logical_table->physical_db_id[0], dbal_logical_table->app_id,
                         &dbal_logical_table_id));

        if ((dbal_logical_table_id == DBAL_TABLE_FWD_MACT) || (dbal_logical_table_id == DBAL_TABLE_FWD_MACT_IVL))
        {
            entry.payload_size = dbal_logical_table->multi_res_info[0].entry_payload_size;
        }
    }

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, dbal_physical_table_id, entry.key_size, entry.payload_size, app_id,
                     dbal_logical_table->app_id_size, &entry_encoding));
    entry_hw_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &max_capacity));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "  Running EM test on logical table %s, stored in physical table %s\n  This table can hold at most %d of the below entries\n  key_size: %d, payload_size: %d, app_id: %d, app_id_size: %d\n"),
              dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
              max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE / entry_hw_size, key_size, entry.payload_size,
              dbal_logical_table->app_id, dbal_logical_table->app_id_size));

    sal_memcpy(&entry_duplicate, &entry, sizeof(entry));

    if (mode == MDB_TEST_CAPACITY)
    {
        max_ext_iterations = 1;
    }
    else
    {
        max_ext_iterations = max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE / entry_hw_size;
    }

    if (max_capacity == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "%s does not have HW resources available to it.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                    (unit, dbal_physical_table_id, app_id, entry.payload_size, &mdb_physical_memory,
                     &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

    if (mdb_physical_status_reg != INVALIDr)
    {
        uint64 reg_data;
        uint32 counter;
        SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, MDB_BLOCK(unit), 0, &reg_data));
        counter = soc_reg64_field_get(unit, mdb_physical_status_reg, reg_data, ITEM_4_35f);
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Entry counter before test: %d\n"), counter));
    }

    entry_index = 0;
    while (ext_iterator < max_ext_iterations)
    {
        int max_int_iterator = 0;

        int_iterator = 0;
        do
        {
            uint32 timer_usec;
            entry_index = ext_iterator + int_iterator;
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

            timer_usec = sal_time_usecs();
            rv = mdb_em_entry_add(unit, dbal_physical_table_id, app_id, &entry);
            total_time_usecs += sal_time_usecs() - timer_usec;

            if ((rv != _SHR_E_FULL) && (rv != _SHR_E_NONE))
            {
                SHR_ERR_EXIT(rv, "Failed to add entry.\n");
            }
            total_entry_count++;
            int_iterator++;
        }
        while ((mode == MDB_TEST_CAPACITY) && (rv != _SHR_E_FULL));

        max_int_iterator = int_iterator - 1;

        if (mode == MDB_TEST_CAPACITY)
        {

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "  Successfully added %d entries, this represents a total utilization of %.2f%%\n  (number_of_entries * entry_hw_size / total_hw_bits)\n"),
                      max_int_iterator,
                      (100.00 * max_int_iterator * entry_hw_size / (max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE))));
        }

        int_iterator = 0;
        do
        {
#ifndef ADAPTER_SERVER_MODE
            entry.hitbit = 0;
#endif

            entry_index = ext_iterator + int_iterator;
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry_duplicate));

            SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate));
            int_iterator++;

#ifndef ADAPTER_SERVER_MODE
            if (entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, hitbit on entry is set, hitbit value 0x%x.\n", entry.hitbit);

            }
#endif

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry.payload, entry_duplicate.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));
            if (utilex_bitstream_have_one_in_range(entry.payload, 0     /* start_place */
                                                   , entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry.payload, entry_duplicate.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter = ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) - 1;
                     uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter = ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) - 1;
                     uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry_duplicate.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_index));
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, read data is not equal to written data.\n");
            }

        }
        while (int_iterator < max_int_iterator);

        if (delete_cmd == TRUE)
        {
            int_iterator = 0;
            do
            {
                entry_index = ext_iterator + int_iterator;
                SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

                SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, dbal_physical_table_id, app_id, &entry));

                SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry_duplicate));
                res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate);
                if (res != _SHR_E_NOT_FOUND)
                {
                    /*
                     * SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, entry wasn't deleted as expected .\n");
                     */

                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "Test failed, entry wasn't deleted as expected, index: %d\n"),
                              entry_index));

                }
                int_iterator++;
            }
            while (int_iterator < max_int_iterator);
        }

        if ((mode == MDB_TEST_FULL) || (mode == MDB_TEST_CAPACITY))
        {
            ext_iterator++;
        }
        else
        {
            ext_iterator += 1 + (sal_rand() % ((max_ext_iterations / MDB_TEST_BRIEF_ENTRIES) + 1));
        }
    }

exit:
    if (mdb_physical_status_reg != INVALIDr)
    {
        uint64 reg_data;
        uint32 counter;
        SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, MDB_BLOCK(unit), 0, &reg_data));
        counter = soc_reg64_field_get(unit, mdb_physical_status_reg, reg_data, ITEM_4_35f);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "Entry counter after test: %d, int_iterator: %d, ext_iterator: %d\n"),
                  counter, int_iterator, ext_iterator));
    }
    if (total_entry_count == 0)
    {
        total_entry_count = 1;
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "In total %d entries were added, avg usecs per entry add: %d\n"), total_entry_count,
              total_time_usecs / total_entry_count));

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd)
{
    dbal_tables_e dbal_logical_table_id;
    CONST dbal_logical_table_t *dbal_logical_table = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    for (dbal_logical_table_id = 0; dbal_logical_table_id < DBAL_NOF_TABLES; dbal_logical_table_id++)
    {
        if (dbal_logical_table_id == DBAL_TABLE_FWD_MACT)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_logical_table_id, &dbal_logical_table));
        if (dbal_logical_table->physical_db_id[0] == dbal_physical_table_id)
        {
            break;
        }
    }

    if (dbal_logical_table_id == DBAL_NOF_TABLES)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "DBAL physical table %d is not associated with a DBAL logical table, skipping table test.\n"),
                  dbal_physical_table_id));
    }
    else
    {
        int enable_mact = TRUE;

#ifdef ADAPTER_SERVER_MODE
        enable_mact = FALSE;
#endif

        SHR_IF_ERR_EXIT(mdb_em_test_table(unit, dbal_logical_table, mode, delete_cmd));

        if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_LEM) && (enable_mact == TRUE))
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, DBAL_TABLE_FWD_MACT, &dbal_logical_table));
            SHR_IF_ERR_EXIT(mdb_em_test_table(unit, dbal_logical_table, mode, delete_cmd));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_dynamic_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode)
{
    dbal_logical_table_t dbal_logical_table;
    uint8 vmv_size, vmv_value;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    sal_memset(&dbal_logical_table, 0x0, sizeof(dbal_logical_table));

    dbal_logical_table.access_method = DBAL_ACCESS_METHOD_MDB;
    dbal_logical_table.app_id = (1 << dnx_data_mdb.em.min_nof_app_id_bits_get(unit)) - 2;
    dbal_logical_table.app_id_size = dnx_data_mdb.em.min_nof_app_id_bits_get(unit);
    dbal_logical_table.core_id_nof_bits = 0;
    dbal_logical_table.core_mode = DBAL_CORE_MODE_SBC;
    dbal_logical_table.key_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_key_size;
    SHR_IF_ERR_EXIT(mdb_get_capacity(unit, dbal_physical_table_id, &dbal_logical_table.max_capacity));
    dbal_logical_table.max_payload_size =
        dnx_data_mdb.pdbs.pdb_info_get(unit,
                                       dbal_physical_table_id)->max_payload_size -
        dnx_data_mdb.em.max_tid_size_get(unit) - dnx_data_mdb.em.max_nof_vmv_size_get(unit);
    dbal_logical_table.nof_physical_tables = 1;
    dbal_logical_table.physical_db_id[0] = dbal_physical_table_id;
    sal_sprintf(dbal_logical_table.table_name, "DYNAMIC_%s_app_%d",
                dbal_physical_table_to_string(unit, dbal_physical_table_id), dbal_logical_table.app_id);
    dbal_logical_table.table_type = DBAL_TABLE_TYPE_EM;

    SHR_IF_ERR_EXIT(mdb_init_logical_table(unit, DBAL_TABLE_FWD_MACT, &dbal_logical_table));

    SHR_IF_ERR_EXIT(mdb_get_vmv_size_value
                    (unit, dbal_physical_table_id, MDB_EM_ENTRY_ENCODING_ONE, &vmv_size, &vmv_value));
    if (vmv_value == 0)
    {
        vmv_size = dnx_data_mdb.em.max_nof_vmv_size_get(unit);
        vmv_value = 0x1;

        SHR_IF_ERR_EXIT(mdb_set_vmv_size_value
                        (unit, dbal_physical_table_id, MDB_EM_ENTRY_ENCODING_ONE, vmv_size, vmv_value));
    }

    SHR_IF_ERR_EXIT(mdb_em_test_table(unit, &dbal_logical_table, mode, TRUE));

exit:
    SHR_FUNC_EXIT;
}

void
mdb_em_get_lfsr_and_spn_sizes(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e physical_db_id,
    uint32 *spn_size_index,
    uint32 *lfsr_size_index)
{

    *spn_size_index = 0;
    while (((*spn_size_index) < dnx_data_mdb.em.spn_get(unit, physical_db_id)->nof_spn_size - 1)
           && (dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[(*spn_size_index) + 1] <= key_size))
    {
        (*spn_size_index)++;
    }

    *lfsr_size_index = 0;
    while (((*lfsr_size_index) < dnx_data_mdb.em.nof_lfsr_sizes_get(unit) - 1)
           && (dnx_data_mdb.em.lfsr_get(unit, (*lfsr_size_index) + 1)->lfsr_size <= key_size))
    {
        (*lfsr_size_index)++;
    }
}

shr_error_e
mdb_dh_add_em_format(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e dbal_physical_table_id)
{
    int bank_iter;
    int blk;
    uint32 cluster_pos;
    uint32 nof_clusters;
    uint32 global_macro_index;
    uint8 format_count = 0;
    soc_mem_t fmt_atr_mem;
    em_key_format_info key_format_info;
    int nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.format_count.get(unit, dbal_physical_table_id, &format_count));

    if (format_count == dnx_data_mdb.em.nof_formats_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "%s has more different key sizes than the number of EM formats (%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id),
                     dnx_data_mdb.em.nof_formats_get(unit));
    }
    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                    key_to_format_map.set(unit, dbal_physical_table_id, key_size, format_count));

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.format_count.set(unit, dbal_physical_table_id, format_count + 1));

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.get(unit, dbal_physical_table_id, &key_format_info));

    nof_clusters = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width / MDB_NOF_CLUSTER_ROW_BITS;

    nof_banks = dnx_data_mdb.pdbs.max_nof_interface_dhs_get(unit);

    for (bank_iter = 0; bank_iter < nof_banks; bank_iter++)
    {

        if (key_format_info.em_bank_info[bank_iter].bank_size_used_8k > 0)
        {
            uint32 cluster_index = 0;
            uint32 bucket_size_log2 = utilex_log2_round_up(key_format_info.em_bank_info[bank_iter].bank_size_used_8k);

            while (cluster_index < dnx_data_mdb.dh.nof_macro_clusters_get(unit))
            {

                if (utilex_bitstream_test_bit(&key_format_info.em_bank_info[bank_iter].cluster_mapping, cluster_index))
                {
                    uint32 mask = 0;
                    uint32 compare_size = key_size - MDB_EM_NOF_BITS_8K_ADDRESS - bucket_size_log2;

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                    (&mask,
                                     (compare_size % dnx_data_mdb.em.format_granularity_get(unit)) +
                                     MDB_EM_FORMAT_MASK_SHIFT, SAL_UINT32_NOF_BITS - 1));

                    for (cluster_pos = 0; cluster_pos < nof_clusters; cluster_pos++)
                    {

                        uint32 offset;
                        uint32 offset_shift = (cluster_pos == 0) ? 0 : MDB_EM_FORMAT_MASK_SHIFT;

                        uint32 format_res;

                        offset =
                            ((compare_size /
                              dnx_data_mdb.
                              em.format_granularity_get(unit)) >> offset_shift) & MDB_EM_FORMAT_OFFSET_MASK;

                        global_macro_index =
                            MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, key_format_info.em_bank_info[bank_iter].macro_type,
                                                          key_format_info.em_bank_info[bank_iter].macro_index);

                        SHR_IF_ERR_EXIT(mdb_get_dh_block(unit, global_macro_index, &blk));

                        fmt_atr_mem = dnx_data_mdb.dh.cluster_fmt_atr_info_get(unit, global_macro_index)->fmt_atr_mem;

                        format_res = mask | offset;

                        SHR_IF_ERR_EXIT(soc_mem_array_write
                                        (unit, fmt_atr_mem, cluster_index + cluster_pos, blk, format_count,
                                         &format_res));

                    }
                    cluster_index += nof_clusters;

                }
                else
                {
                    cluster_index++;
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table)
{

    int ii, loop_limit;
    uint32 key_size;
    uint32 compare_size;
    SHR_FUNC_INIT_VARS(unit);

    key_size = dbal_logical_table->key_size;
    if (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC)
    {
        key_size -= dbal_logical_table->core_id_nof_bits;
    }
    key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, key_size);

    compare_size = key_size + dbal_logical_table->app_id_size;

    if (dbal_logical_table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. dbal_physical_table %s is not associated with MDB.\n",
                     dbal_physical_table_to_string(unit, dbal_logical_table->physical_db_id[0]));
    }

    loop_limit =
        (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC) ? (dnx_data_device.general.nof_cores_get(unit) - 1) : 0;

    for (ii = 0; ii <= loop_limit; ii++)
    {

        soc_reg_above_64_val_t fmt_data;
        soc_reg_above_64_val_t age_profile_data;
        soc_reg_above_64_val_t ovf_enable_reg_data, ovf_enable_field_data;
        dbal_physical_tables_e physical_db_id = dbal_logical_table->physical_db_id[ii];
        soc_mem_t tid_atr = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->tid_atr_mem;
        soc_reg_t ovf_enable_reg;
        soc_reg_t emp_age_profile_reg;
        soc_field_t ovf_enable_field;
        int em_address_size;
        uint32 app_id;
        uint32 nof_app_id_bits = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->tid_size;
        uint32 enable_spn = TRUE;
        uint32 age_profile = 0;
        uint32 age_profile_size_in_bits = dnx_data_mdb.em.aging_profiles_size_in_bits_get(unit);
        uint32 spn_size_index;
        uint32 lfsr_size_index;
        uint32 em_format;
        uint8 is_mact;

        if (nof_app_id_bits < dbal_logical_table->app_id_size)
        {
            LOG_CLI((BSL_META
                     ("%s has app_id_size of %d, while the nof_app_id_bits of %s is %d.\n"),
                     dbal_logical_table->table_name,
                     dbal_logical_table->app_id_size,
                     dbal_physical_table_to_string(unit, physical_db_id), nof_app_id_bits));
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                        key_to_format_map.get(unit, physical_db_id, compare_size, &em_format));

        if (em_format == MDB_INVALID_EM_FORMAT_VALUE)
        {
            SHR_IF_ERR_EXIT(mdb_dh_add_em_format(unit, compare_size, physical_db_id));
            SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                            key_to_format_map.get(unit, physical_db_id, compare_size, &em_format));
        }

        SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, physical_db_id, &em_address_size));

        SHR_IF_ERR_EXIT(mdb_em_is_mact_application(unit, physical_db_id, dbal_logical_table->app_id, &is_mact));
        if (dbal_logical_table->app_id >= (1 << dbal_logical_table->app_id_size))
        {
            SHR_ERR_EXIT(_SHR_E_BADID,
                         "%s has the APP ID value of %d, while its app_id_size is %d and the nof_app_id_bits of %s is %d.\n",
                         dbal_logical_table->table_name, dbal_logical_table->app_id,
                         dbal_logical_table->app_id_size, dbal_physical_table_to_string(unit, physical_db_id),
                         nof_app_id_bits);
        }

        emp_age_profile_reg = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->emp_age_profiles;

        if (emp_age_profile_reg != INVALIDr)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, emp_age_profile_reg, REG_PORT_ANY, 0, age_profile_data));
            age_profile = is_mact;
        }

        for (app_id = dbal_logical_table->app_id;
             app_id < (1 << nof_app_id_bits); app_id += (1 << dbal_logical_table->app_id_size))
        {
            uint32 existing_key_size;

            SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.
                            app_id_size.set(unit, physical_db_id, app_id, dbal_logical_table->app_id_size));

            SHR_IF_ERR_EXIT(soc_mem_read(unit, tid_atr, SOC_BLOCK_ALL, app_id, fmt_data));

            soc_mem_field_get(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_5,
                              &existing_key_size);

            if (existing_key_size != 0)
            {

                int search_db_index;
                int search_app_id;
                const dbal_logical_table_t *search_dbal_logical_table;

                for (search_db_index = 0; search_db_index < DBAL_NOF_TABLES; search_db_index++)
                {
                    if (search_db_index != dbal_table)
                    {
                        int jj;
                        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, search_db_index, &search_dbal_logical_table));
                        for (jj = 0; jj < search_dbal_logical_table->nof_physical_tables; jj++)
                        {
                            if ((search_dbal_logical_table->maturity_level != DBAL_MATURITY_LOW)
                                && (search_dbal_logical_table->mdb_image_type != DBAL_MDB_IMG_STD_1_NOT_ACTIVE))
                            {
                                if (search_dbal_logical_table->physical_db_id[jj] == physical_db_id)
                                {
                                    for (search_app_id = search_dbal_logical_table->app_id;
                                         search_app_id < (1 << nof_app_id_bits);
                                         search_app_id += (1 << search_dbal_logical_table->app_id_size))
                                    {
                                        if (app_id == search_app_id)
                                        {
                                            LOG_CLI((BSL_META
                                                     ("%s: APP ID %d is already configured by %s for physical table %s.\n"),
                                                     dbal_logical_table->table_name, app_id,
                                                     search_dbal_logical_table->table_name,
                                                     dbal_physical_table_to_string(unit, physical_db_id)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            mdb_em_get_lfsr_and_spn_sizes(unit, key_size, physical_db_id, &spn_size_index, &lfsr_size_index);

            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_0,
                              &em_format);
            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_1,
                              &lfsr_size_index);
            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_2,
                              &spn_size_index);
            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_3,
                              &enable_spn);
            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_4,
                              (uint32 *) (&(dbal_logical_table->app_id_size)));
            soc_mem_field_set(unit, tid_atr, fmt_data, dnx_data_mdb.em.em_info_get(unit, physical_db_id)->field_5,
                              &key_size);

            SHR_IF_ERR_EXIT(soc_mem_write(unit, tid_atr, SOC_BLOCK_ALL, app_id, fmt_data));

            if (emp_age_profile_reg != INVALIDr)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field(age_profile_data,
                                                           app_id * age_profile_size_in_bits,
                                                           age_profile_size_in_bits, age_profile));
            }

        }

        if (emp_age_profile_reg != INVALIDr)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, emp_age_profile_reg, REG_PORT_ANY, 0, age_profile_data));
        }

        if (!(is_mact && SOC_IS_JERICHO_2_A0(unit)))
        {
            ovf_enable_reg = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->ovf_enable_reg;
            ovf_enable_field = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->ovf_enable_field;

            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ovf_enable_reg, REG_PORT_ANY, 0, ovf_enable_reg_data));

            soc_reg_above_64_field_get(unit, ovf_enable_reg, ovf_enable_reg_data, ovf_enable_field,
                                       ovf_enable_field_data);

            for (app_id = dbal_logical_table->app_id; app_id < (1 << nof_app_id_bits);
                 app_id += (1 << dbal_logical_table->app_id_size))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(ovf_enable_field_data, app_id));
            }

            soc_reg_above_64_field_set(unit, ovf_enable_reg, ovf_enable_reg_data, ovf_enable_field,
                                       ovf_enable_field_data);

            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ovf_enable_reg, REG_PORT_ANY, 0, ovf_enable_reg_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_calc_vmv_regs(
    int unit,
    mdb_em_encoding_request_e encoding_request,
    dbal_physical_tables_e dbal_table_id,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data)
{
    uint8 entry_encoding = 0;
    uint32 encoding_nof_bits;
    uint8 vmv_value;
    uint32 encoding_value;
    uint32 encoding_index;
    uint32 vmv_mask;
    uint8 vmv_size;
    uint8 vmv_value_init;
    int is_esem_shift = FALSE;
    int vmv_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    encoding_nof_bits = MDB_INIT_GET_EM_EMCODING_NOF_BITS(unit, encoding_request);

    if ((dbal_table_id == DBAL_PHYSICAL_TABLE_ESEM) && (vmv_encoding_data == NULL))
    {
        is_esem_shift = TRUE;
    }

    for (entry_encoding = 0; entry_encoding < MDB_EM_NOF_ENCODING_TYPES; entry_encoding++)
    {
        encoding_value = mdb_init_encoding_values_map[encoding_request][entry_encoding];
        encoding_index = mdb_init_encoding_values_map[MDB_EM_ENCODING_TABLE_ENTRY][entry_encoding];

        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.size.get(unit, dbal_table_id, encoding_index, &vmv_size));

        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.value.get(unit, dbal_table_id, encoding_index, &vmv_value_init));

        if (vmv_size != 0)
        {
            vmv_nof_bits = dnx_data_mdb.em.max_nof_vmv_size_get(unit);

            if (is_esem_shift == TRUE)
            {
                vmv_nof_bits = UTILEX_MAX(dnx_data_mdb.em.esem_nof_vmv_size_get(unit), vmv_size);

                vmv_value_init >>= (dnx_data_mdb.em.max_nof_vmv_size_get(unit) -
                                    dnx_data_mdb.em.esem_nof_vmv_size_get(unit));
            }

            SHR_IF_ERR_EXIT(mdb_em_set_vmv_regs
                            (unit, vmv_value_init, vmv_size, vmv_nof_bits, encoding_value, encoding_nof_bits,
                             vmv_encoding_data, vmv_size_data));
        }
    }

    vmv_nof_bits =
        (is_esem_shift ==
         TRUE) ? dnx_data_mdb.em.esem_nof_vmv_size_get(unit) : dnx_data_mdb.em.max_nof_vmv_size_get(unit);

    vmv_mask = UTILEX_BITS_MASK(vmv_nof_bits - 1, 0);

    for (vmv_value = 0; vmv_value < (1 << dnx_data_mdb.em.max_nof_vmv_size_get(unit)); vmv_value++)
    {

        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.encoding_map.
                        encoding.get(unit, dbal_table_id, vmv_value, &entry_encoding));
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.vmv_info.encoding_map.size.get(unit, dbal_table_id, vmv_value, &vmv_size));

        encoding_index = mdb_init_encoding_value_to_encoding_index_map[entry_encoding];
        if ((encoding_index < MDB_EM_ENCODING_TYPE_EMPTY) && (((vmv_mask >> vmv_size) & vmv_value) == 0))
        {
            encoding_value = mdb_init_encoding_values_map[encoding_request][entry_encoding];

            SHR_IF_ERR_EXIT(mdb_em_set_vmv_regs
                            (unit, vmv_value, vmv_size, vmv_nof_bits, encoding_value, encoding_nof_bits,
                             vmv_encoding_data, vmv_size_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_init_shift_vmv_conf(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_FLP, 0, DBAL_PHYSICAL_TABLE_LEM));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_FLP, 1, DBAL_PHYSICAL_TABLE_SEXEM_3));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_FLP, 2, DBAL_PHYSICAL_TABLE_SEXEM_2));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_1, 0, DBAL_PHYSICAL_TABLE_SEXEM_1));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_1, 1, DBAL_PHYSICAL_TABLE_ISEM_1));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_1, 2, DBAL_PHYSICAL_TABLE_ISEM_1));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_2_3, 0, DBAL_PHYSICAL_TABLE_SEXEM_1));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_2_3, 1, DBAL_PHYSICAL_TABLE_ISEM_2));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_2_3, 2, DBAL_PHYSICAL_TABLE_ISEM_2));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_4_5, 0, DBAL_PHYSICAL_TABLE_SEXEM_3));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_4_5, 1, DBAL_PHYSICAL_TABLE_ISEM_2));
    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_VTT_4_5, 2, DBAL_PHYSICAL_TABLE_ISEM_2));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_IOAM, 0, DBAL_PHYSICAL_TABLE_IOEM_1));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_EOAM, 0, DBAL_PHYSICAL_TABLE_EOEM_1));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.
                    shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_RMEP, 0, DBAL_PHYSICAL_TABLE_RMEP_EM));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_ESEM_1, 0, DBAL_PHYSICAL_TABLE_ESEM));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_ESEM_2, 0, DBAL_PHYSICAL_TABLE_ESEM));

    SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.set(unit, MDB_EM_SHIFT_VMV_ESEM_3, 0, DBAL_PHYSICAL_TABLE_ESEM));

exit:
    SHR_FUNC_EXIT;

}
