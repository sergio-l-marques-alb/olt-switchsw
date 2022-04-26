
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/sand/sand_mem.h>
#include "mdb_internal.h"
#include "../dbal/dbal_internal.h"
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>


    

typedef enum
{
    MDB_EM_CMD_DELETE = 320,
    MDB_EM_CMD_INSERT = 256,
    MDB_EM_CMD_INSERT_OFC = 384,
    MDB_EM_CMD_TRANSPLANT = 288,
    MDB_EM_CMD_LOOKUP = 0,
    MDB_EM_CMD_READ_BY_ADDR = 1,
    MDB_EM_CMD_DELETE_BY_ADDR = 5,
    MDB_EM_CMD_READ_HITS_A_BY_ADDR = 6,
    MDB_EM_CMD_READ_HITS_B_BY_ADDR = 7,
    MDB_EM_CMD_READ_CLEAR_HITS_A_BY_ADDR = 8,
    MDB_EM_CMD_READ_CLEAR_HITS_B_BY_ADDR = 9,
    MDB_EM_CMD_WRITE_HIT_A_BY_ADDR = 10,
    MDB_EM_CMD_WRITE_HIT_B_BY_ADDR = 11,
    MDB_EM_CMD_READ_RATIOS_BY_ADDR = 15,

    MDB_NOF_EM_CMD = 14
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
    MDB_EM_FAIL_REASON_NOT_PROCESSED = 0xa,
    MDB_EM_FAIL_REASON_HOST_STATUS_WORKING = 0xf,

    MDB_EM_NOF_FAIL_REASONS
} mdb_em_fail_reason_e;

static const char *mdb_em_fail_reason_strings[MDB_EM_NOF_FAIL_REASONS] = {
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
    "NOT_PROCESSED",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "HOST_STATUS_WORKING"
};

typedef enum
{

    

    MDB_MACT_FAIL_REASON_CLEAR = 0x0,
    MDB_MACT_FAIL_REASON_CAM_TABLE_FULL = 0x1,
    MDB_MACT_FAIL_REASON_CAM_TABLE_LOCKED = 0x2,
    MDB_MACT_FAIL_REASON_DELETE_UNKNOWN_KEY = 0x3,
    MDB_MACT_FAIL_REASON_STEPS_WATCHDOG_TRIGGER = 0x4,
    MDB_MACT_FAIL_REASON_INSERTED_EXISTING = 0x5,
    MDB_MACT_FAIL_REASON_CHANGE_FAIL_REQUEST_NON_EXIST = 0x6,
    MDB_MACT_FAIL_REASON_ECC_ERROR = 0x7,
    MDB_MACT_FAIL_REASON_INVALID_TID_FOR_OFC = 0x8,
    MDB_MACT_FAIL_REASON_BAD_STARTING_ENTRY = 0x9,
    MDB_MACT_FAIL_REASON_EXCEED_LIMIT = 0xa,
    MDB_MACT_FAIL_REASON_DELETE_NON_EXIST = 0xb,
    MDB_MACT_FAIL_REASON_LEARN_OVER_STRONGER = 0xc,
    MDB_MACT_FAIL_REASON_TRANSPLANT_OVER_STRONGER = 0xd,
    MDB_MACT_FAIL_REASON_REFRESH_OVER_STRONGER = 0xe,
    MDB_MACT_FAIL_REASON_FLUSH_DROP = 0xf,

    MDB_MACT_NOF_FAIL_REASONS
} mdb_mact_fail_reason_e;

static const char *mdb_mact_fail_reason_strings[MDB_MACT_NOF_FAIL_REASONS] = {
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





#define MDB_EM_SHADOW_HASH_NOF_RESOURCES    (10)



#define MDB_INIT_ENCODING_TYPE_INVALID      (0xFF)



#define MDB_EM_MAX_SHADOW_KEY_IN_BYTE  BITS2BYTES(DNX_DATA_MAX_MDB_PDBS_MAX_KEY_SIZE + DNX_DATA_MAX_MDB_EM_MAX_TID_SIZE)



#define MDB_EM_MAX_PAYLOAD_IN_BYTE  BITS2BYTES(DNX_DATA_MAX_MDB_PDBS_MAX_PAYLOAD_SIZE)



#define MDB_EM_MACT_STAMP_MAX 0xFF



#define MDB_EM_MACT_POLL_ATTEMPTS 10000000



#define MDB_EM_MACT_TRANSPLANT_POLL_ATTEMPTS 50000



#define MDB_EM_MACT_NOT_PROCESSED_RETRY 100000


#define MDB_EM_MACT_ENTRY_FIELD_SIZE 150


#define MDB_EM_EMP_SCAN_STATUS_POLL_RETRY 100000



#define MDB_EM_HITBIT_AGE_ATTEMPTS 100



#define MDB_EM_ENTRY_OFFSET 9



#define MDB_EM_ENTRY_TYPE_SIZE 3



#define MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS     (3)







#define MDB_EM_TABLE_MAX_SHADOW_KEY_IN_BYTE(_unit, _dbal_physical_table_id)  (BITS2BYTES(dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _dbal_physical_table_id)->max_key_size + dnx_data_mdb.em.em_info_get(_unit, _dbal_physical_table_id)->tid_size))



#define MDB_EM_TABLE_MAX_PAYLOAD_IN_BYTE(_unit, _dbal_physical_table_id)  (BITS2BYTES(dnx_data_mdb.pdbs.dbal_pdb_info_get(_unit, _dbal_physical_table_id)->max_payload_size) + 1)



#define MDB_INIT_GET_EM_EMCODING_NOF_BITS(unit, encod_req) (encod_req == MDB_EM_ENCODING_TABLE_ENTRY) ? MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS : ((encod_req == MDB_EM_ENCODING_DH_ENTRY_120) ? dnx_data_mdb.em.dh_120_entry_encoding_nof_bits_get(unit): dnx_data_mdb.em.dh_240_entry_encoding_nof_bits_get(unit))





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



sbusdma_desc_module_enable_e
mdb_em_dbal_table_to_dma_enum(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    sbusdma_desc_module_enable_e desc_module;
    dbal_enum_value_field_mdb_db_type_e db_type;
    uint8 em_sub_type;

    db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type;
    em_sub_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype;

    if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        switch (em_sub_type)
        {
            case MDB_EM_TYPE_ISEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_ISEM;
                break;
            case MDB_EM_TYPE_LEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM;
                break;
            case MDB_EM_TYPE_OEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_OEM;
                break;
            case MDB_EM_TYPE_PPMC:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_PPMC;
                break;
            case MDB_EM_TYPE_GLEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_GLEM;
                break;
            case MDB_EM_TYPE_ESEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_ESEM;
                break;
            case MDB_EM_TYPE_EXEM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_EXEM;
                break;
            case MDB_EM_TYPE_RMEP_EM:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_RMEP_EM;
                break;
            default:
                desc_module = SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES;
        }
    }
    else
    {
        desc_module = SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES;
    }

    return desc_module;
}




static shr_error_e
mdb_em_init_interface_blocks(
    int unit)
{
    dbal_physical_tables_e dbal_physical_table_iter;

    SHR_FUNC_INIT_VARS(unit);

    for (dbal_physical_table_iter = 0; dbal_physical_table_iter < DBAL_NOF_PHYSICAL_TABLES; dbal_physical_table_iter++)
    {
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_iter)->db_type ==
            DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            int blk;

            {
                blk = MDB_BLOCK(unit);
            }
            SHR_IF_ERR_EXIT(mdb_db_infos.em_interface_blk.set(unit, dbal_physical_table_iter, blk));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_init_overrides(
    int unit)
{
    soc_reg_above_64_val_t reg_val = { 0 };



    uint32 shift_msbits = 10;
    uint32 timeout_val, em_timeout_val;
    uint32 shift_clock_bits;
    uint32 core_clock_khz;
    dbal_physical_tables_e dbal_physical_table_id;
    uint8 is_standard_image = FALSE;
    uint32 block_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_CMIC_TOP_SBUS_TIMEOUTr(unit, reg_val));

    

    if (reg_val[0] == 0)
    {
        timeout_val = 0x50d6;
    }
    else
    {
        timeout_val = ((32 - shift_msbits) << shift_msbits) | (reg_val[0] >> (32 - shift_msbits));
    }

    
    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown))
    {
        em_timeout_val = timeout_val;
    }
    else
    {
        
        core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
        shift_clock_bits = 13;
        em_timeout_val = (shift_clock_bits << shift_msbits) | ((core_clock_khz / 10) >> shift_clock_bits);
    }

#if defined(BCM_DNX2_SUPPORT)
    if (SOC_REG_IS_VALID(unit, MDB_INDIRECT_COMMANDr))
    {
        uint64 reg_val_64 = 0;
        SHR_IF_ERR_EXIT(READ_MDB_INDIRECT_COMMANDr(unit, &reg_val_64));
        soc_reg64_field_set(unit, MDB_INDIRECT_COMMANDr, &reg_val_64, INDIRECT_COMMAND_TIMEOUTf, em_timeout_val);
        SHR_IF_ERR_EXIT(WRITE_MDB_INDIRECT_COMMANDr(unit, reg_val_64));
    }
#endif


    
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, MACT_INDIRECT_COMMANDr, REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, MACT_INDIRECT_COMMANDr, reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, MACT_INDIRECT_COMMANDr, REG_PORT_ANY, 0, reg_val));

#if defined(BCM_DNX2_SUPPORT)
    if (SOC_REG_IS_VALID(unit, KAPS_INDIRECT_COMMANDr))
    {
        uint32 core;
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, KAPS_INDIRECT_COMMANDr, core, 0, reg_val));
            soc_reg_field_set(unit, KAPS_INDIRECT_COMMANDr, reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, KAPS_INDIRECT_COMMANDr, core, 0, reg_val));
        }
    }
#endif

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, DDHA_INDIRECT_COMMANDr, REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DDHA_INDIRECT_COMMANDr, reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
    for (block_iter = 0; block_iter < dnx_data_mdb.dh.nof_ddha_blocks_get(unit); block_iter++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDHA_INDIRECT_COMMANDr, block_iter, 0, reg_val));
    }

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, DDHB_INDIRECT_COMMANDr, REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DDHB_INDIRECT_COMMANDr, reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
    for (block_iter = 0; block_iter < dnx_data_mdb.dh.nof_ddhb_blocks_get(unit); block_iter++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, DDHB_INDIRECT_COMMANDr, block_iter, 0, reg_val));
    }

    if (dnx_data_mdb.dh.nof_dhc_blocks_get(unit) != 0)
    {
        SHR_IF_ERR_EXIT(READ_DHC_INDIRECT_COMMANDr(unit, REG_PORT_ANY, reg_val));
        soc_reg_field_set(unit, DHC_INDIRECT_COMMANDr, reg_val, INDIRECT_COMMAND_TIMEOUTf, timeout_val);
        for (block_iter = 0; block_iter < dnx_data_mdb.dh.nof_dhc_blocks_get(unit); block_iter++)
        {
            SHR_IF_ERR_EXIT(WRITE_DHC_INDIRECT_COMMANDr(unit, block_iter, reg_val[0]));
        }
    }


    

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    if ((is_standard_image)
        && (!(dnx_data_sbusdma_desc.global.feature_get(unit, dnx_data_sbusdma_desc_global_force_enable))))
    {
        for (dbal_physical_table_id = DBAL_PHYSICAL_TABLE_NONE; dbal_physical_table_id < DBAL_NOF_PHYSICAL_TABLES;
             dbal_physical_table_id++)
        {
            if (dnx_sbusdma_desc_is_enabled(unit, mdb_em_dbal_table_to_dma_enum(unit, dbal_physical_table_id)) == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Descriptor DMA soc property (%s) is not supported for EM tables (%s).\n",
                             spn_DMA_DESC_AGGREGATOR_ENABLE_SPECIFIC, dbal_physical_table_to_string(unit,
                                                                                                    dbal_physical_table_id));
            }
        }
    }

    SHR_IF_ERR_EXIT(mdb_em_init_interface_blocks(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_init_shadow_db(
    int unit)
{
    sw_state_htbl_init_info_t htb_init_info;
    dbal_physical_tables_e dbal_physical_table;
    SHR_FUNC_INIT_VARS(unit);

    for (dbal_physical_table = 0; dbal_physical_table < DBAL_NOF_PHYSICAL_TABLES; dbal_physical_table++)
    {
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            int capacity;
            int key_size_in_byte = MDB_EM_TABLE_MAX_SHADOW_KEY_IN_BYTE(unit, dbal_physical_table);
            int payload_size_in_byte = MDB_EM_TABLE_MAX_PAYLOAD_IN_BYTE(unit, dbal_physical_table);
            int capacity_entries;
#if (0)

            int min_entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;

#endif
            mdb_em_entry_encoding_e encoding = MDB_EM_ENTRY_ENCODING_EMPTY;

            sal_memset(&htb_init_info, 0, sizeof(htb_init_info));
            mdb_db_infos.capacity.get(unit, dbal_physical_table, &capacity);

            

            if (!MDB_EM_TABLE_USE_ALL_ASPECT_RATIOS(dbal_physical_table))
            {
                for (encoding = MDB_EM_ENTRY_ENCODING_ONE; encoding <= MDB_EM_ENTRY_ENCODING_EIGHTH; encoding++)
                {
                    uint8 vmv_size;

                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table, encoding, &vmv_size));

#if (0)

                    if (vmv_size != 0)
                    {
                        min_entry_size =
                            (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table)->row_width) /
                            (1 << encoding);
                    }

#endif
                }
            }

            

            capacity_entries = capacity;
            capacity_entries += dnx_data_mdb.em.ovf_cam_max_size_get(unit);
            htb_init_info.max_nof_elements = capacity_entries;
            htb_init_info.expected_nof_elements = capacity_entries;
            SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.create(unit, dbal_physical_table, &htb_init_info, key_size_in_byte,
                                                          payload_size_in_byte));
        }

    }

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
    uint32 key_size;
    uint32 key_size_byte;
    uint32 entry_key_size_byte;
    uint32 tid_size = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table)->tid_size;

    SHR_FUNC_INIT_VARS(unit);

    key_size_byte = MDB_EM_TABLE_MAX_SHADOW_KEY_IN_BYTE(unit, dbal_physical_table);

    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table, app_id, &key_size));

    entry_key_size_byte = BITS2BYTES(key_size);

    sal_memset(shadow_key, 0, MDB_EM_MAX_SHADOW_KEY_IN_BYTE);

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(entry->key, entry_key_size_byte, shadow_key));

    
    shadow_key[entry_key_size_byte - 1] &= (0xFF >> ((SAL_UINT8_NOF_BITS - (key_size & 0x7)) & 0x7));

    shadow_key[key_size_byte - 1] |= app_id << (SAL_UINT8_NOF_BITS - tid_size);

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
    uint8 success;
    int max_payload_size_in_byte;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_payload, 0x0, sizeof(shadow_payload));



    SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

    

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(entry->payload, BITS2BYTES(entry->payload_size), shadow_payload));

    

    max_payload_size_in_byte = MDB_EM_TABLE_MAX_PAYLOAD_IN_BYTE(unit, dbal_physical_table);
    shadow_payload[max_payload_size_in_byte - 1] = entry->payload_size;

    SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.insert(unit, dbal_physical_table, shadow_key, shadow_payload, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't add entry to MDB EM hash SW table.\n");
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
    uint8 shadow_key[MDB_EM_MAX_SHADOW_KEY_IN_BYTE];
    uint8 payload[MDB_EM_MAX_PAYLOAD_IN_BYTE];
    uint8 found_byte;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

    
    SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.find(unit, dbal_physical_table, shadow_key, payload, &found_byte));

    if (found_byte)
    {
        SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.delete(unit, dbal_physical_table, shadow_key));
    }
    else
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_shadow_extract_payload(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint8 shadow_payload[MDB_EM_MAX_PAYLOAD_IN_BYTE],
    dbal_physical_entry_t * entry)
{
    uint32 shadow_payload_size;
    uint32 payload_align[BYTES2WORDS(MDB_EM_MAX_PAYLOAD_IN_BYTE)];
    int max_payload_size_in_byte;

    SHR_FUNC_INIT_VARS(unit);

    max_payload_size_in_byte = MDB_EM_TABLE_MAX_PAYLOAD_IN_BYTE(unit, dbal_physical_table);

    

    shadow_payload_size = shadow_payload[max_payload_size_in_byte - 1];

     
    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_payload, BITS2BYTES(shadow_payload_size), payload_align));

    

    if ((entry->payload_size == 0) || (entry->payload_size < shadow_payload_size))
    {
        entry->payload_size = shadow_payload_size;
    }

    

    SHR_BITCOPY_RANGE(entry->payload, entry->payload_size - shadow_payload_size, payload_align, 0, shadow_payload_size);

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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_shadow_build_key(unit, dbal_physical_table, app_id, entry, shadow_key));

    SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.find(unit, dbal_physical_table, shadow_key, shadow_payload, &found_byte));

    if (found_byte)
    {
        SHR_IF_ERR_EXIT(mdb_em_shadow_extract_payload(unit, dbal_physical_table, app_id, shadow_payload, entry));
    }

    *found = found_byte;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_is_mact(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 payload_size,
    uint32 *is_mact)
{
    int enable_mact = TRUE;
    uint32 is_mact_temp;

    SHR_FUNC_INIT_VARS(unit);

    

#ifdef ADAPTER_SERVER_MODE
    enable_mact = FALSE;
#endif

    SHR_IF_ERR_EXIT(dbal_physical_table_app_id_is_mact(unit, dbal_physical_table_id, app_id, &is_mact_temp));

    if ((is_mact_temp == TRUE) && (enable_mact == TRUE)
        && (payload_size <= dnx_data_mdb.em.mact_max_payload_size_get(unit)))
    {
        *is_mact = TRUE;
    }
    else
    {
        *is_mact = FALSE;
    }

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
    uint32 entry_addr_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &entry_addr_bits));

    

    for (entry_encoding_index = MDB_EM_ENTRY_ENCODING_EIGHTH; entry_encoding_index >= MDB_EM_ENTRY_ENCODING_ONE;
         entry_encoding_index--)
    {
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding_index, &vmv_size));

        

        if (vmv_size == 0)
        {
            continue;
        }

        if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
        {

            

            vmv_size = 0;
        }

        
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype == MDB_EM_TYPE_LEM)
        {
            uint32 is_mact;

            SHR_IF_ERR_EXIT(mdb_em_is_mact(unit, dbal_physical_table_id, app_id, payload_size, &is_mact));

            if (((is_mact == TRUE)) && (vmv_size > dnx_data_mdb.em.mact_nof_vmv_size_get(unit)))
            {
                continue;
            }
        }

        entry_size_capacity =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding_index);

        

        if (entry_size_capacity < MDB_DIRECT_BASIC_ENTRY_SIZE)
        {
            continue;
        }

        

        if (key_size - entry_addr_bits + app_id_size + payload_size + vmv_size <= entry_size_capacity)
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
                     entry_addr_bits, app_id_size, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width);
    }

    if (entry_encoding != NULL)
    {
        *entry_encoding = entry_encoding_index;
    }

    if (payload_size + vmv_size > dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, app_id %d: payload_size(%d) + vmv_size(%d) > mdb_max_payload_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size);
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
    uint32 entry_addr_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &entry_addr_bits));

    

    for (entry_encoding_index = MDB_EM_ENTRY_ENCODING_EIGHTH; entry_encoding_index >= MDB_EM_ENTRY_ENCODING_ONE;
         entry_encoding_index--)
    {
        entry_size_capacity =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding_index);

        

        if (entry_size_capacity < MDB_DIRECT_BASIC_ENTRY_SIZE)
        {
            continue;
        }

        

        if (key_size - entry_addr_bits + app_id_size + payload_size + vmv_size <= entry_size_capacity)
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
                     entry_addr_bits, app_id_size, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width);
    }

    if (entry_encoding != NULL)
    {
        *entry_encoding = entry_encoding_index;
    }

    if (payload_size + vmv_size > dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "%s, app_id %d: payload_size(%d) + vmv_size(%d) > mdb_max_payload_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, payload_size, vmv_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_addr_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *addr_bits)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.nof_address_bits.get(unit, dbal_physical_table_id, addr_bits));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 *vmv_size,
    uint8 *vmv_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding, vmv_size));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.value.get(unit, dbal_physical_table_id, entry_encoding, vmv_value));

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
mdb_em_set_vmv_size_value(
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
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                        encoding.set(unit, dbal_physical_table_id, vmv_value_iter, entry_encoding));
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                        size.set(unit, dbal_physical_table_id, vmv_value_iter, vmv_size));
    }

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.value.set(unit, dbal_physical_table_id, entry_encoding, vmv_value));
    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.set(unit, dbal_physical_table_id, entry_encoding, vmv_size));

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
    uint32 entry_size;
    uint32 temp_total_size;
    uint32 zero_buffer = 0;
    soc_reg_above_64_val_t entry;
    uint8 vmv_size;
    uint8 vmv_value;
    uint32 entry_vmv_value;
    uint32 entry_vmv_value_full;
    uint32 addr_bits;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_ENTRY_ENCODING_ONE;
    uint8 entry_encoding_u8 = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (mem == MACT_CPU_REQUESTm)
    {
        entry_size = MDB_EM_MACT_ENTRY_FIELD_SIZE;
    }
    else
    {
        entry_size = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->entry_size;
    }

    *entry_encoding_output = MDB_EM_NOF_ENTRY_ENCODINGS;

    soc_mem_field_get(unit, mem, data, ENTRYf, entry);
    if (found != NULL)
    {
        if (mem == MACT_CPU_REQUESTm)
        {
            soc_mem_field_get(unit, mem, data, SUCCESSf, found);
        }
        else
        {
            soc_mem_field_get(unit, mem, data, FOUNDf, found);
        }
    }

    

    sal_memset(payload, 0, sizeof(uint32) * BITS2WORDS(payload_size));

    SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &addr_bits));

    

#ifdef ADAPTER_SERVER_MODE
    while (entry_encoding < MDB_EM_ENTRY_ENCODING_EMPTY)
#else
    while (((found == NULL) || (*found == TRUE)) && (entry_encoding < MDB_EM_ENTRY_ENCODING_EMPTY))
#endif
    {

        

        temp_total_size =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width >> entry_encoding;

        

        zero_buffer = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width - temp_total_size;

        SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

        

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

                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                encoding.get(unit, dbal_physical_table_id, entry_vmv_value_full, &entry_encoding_u8));
                SHR_IF_ERR_EXIT(utilex_U8_to_U32(&entry_encoding_u8, 1, entry_encoding_output));

                break;
            }
        }

        entry_encoding++;
    }

    if (found != NULL)
    {
        if (entry_encoding == MDB_EM_ENTRY_ENCODING_EMPTY)
        {
            if (*found)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unable to parse entry encoding of entry.\n");
            }
        }
#ifdef ADAPTER_SERVER_MODE

        

        else
        {
            *found = 1;
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_vmv_calculation_by_entry(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint8 *vmv_size,
    uint8 *vmv_value)
{

    uint8 app_id_size;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_NOF_ENTRY_ENCODINGS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, dbal_physical_table_id, entry->key_size, entry->payload_size, app_id, app_id_size,
                     &entry_encoding));

    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, vmv_size, vmv_value));

    *vmv_value = *vmv_value >> (dnx_data_mdb.em.max_nof_vmv_size_get(unit) - *vmv_size);

    if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
    {

        

        *vmv_size = 0;
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
mdb_em_vmv_calculation_by_full_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 full_vmv_value,
    uint8 *vmv_value,
    uint8 *vmv_size)
{
    uint8 entry_encoding = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                    encoding.get(unit, dbal_physical_table_id, full_vmv_value, &entry_encoding));

    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, vmv_size, vmv_value));

    if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_PPMC)
    {

        

        *vmv_size = 0;
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
    mdb_em_entry_encoding_e entry_encoding,
    uint32 entry_offset,
    soc_reg_above_64_val_t data)
{
    uint32 entry_size;
    uint32 temp_total_size;
    uint32 zero_buffer = 0;
    uint8 vmv_size;
    uint8 vmv_value;
    SHR_FUNC_INIT_VARS(unit);

    if (mem == MACT_CPU_REQUESTm)
    {
        entry_size = MDB_EM_MACT_ENTRY_FIELD_SIZE;
    }
    else
    {
        entry_size = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->entry_size;
    }

    if (key_size + payload_size > entry_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s, app_id %d: key_size(%d) + payload_size(%d) > entry_size(%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id, key_size, payload_size,
                     entry_size);
    }

    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

    if (mem == MACT_CPU_REQUESTm)
    {

        

        zero_buffer = 0;
        vmv_size = dnx_data_mdb.em.max_nof_vmv_size_get(unit);
    }
    else
    {

        temp_total_size =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);

        

        zero_buffer = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width - temp_total_size;
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
        SHR_BITCOPY_RANGE(data, entry_offset + entry_size - zero_buffer - vmv_size - payload_size, payload, 0,
                          payload_size);
        SHR_BITCOPY_RANGE(data, entry_offset + entry_size - zero_buffer - vmv_size, &vmv_value_32,
                          dnx_data_mdb.em.max_nof_vmv_size_get(unit) - vmv_size, vmv_size);
    }
    SHR_BITCOPY_RANGE(data, entry_offset, key, 0, key_size);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_em_mact_prepare_command(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    mdb_em_mact_cmd_e command,
    uint32 *key,
    uint32 key_size,
    uint32 *payload,
    uint32 payload_size,
    soc_reg_above_64_val_t data,
    mdb_em_entry_encoding_e * entry_encoding)
{
    uint8 app_id_size = 0;
    uint32 entry_offset = 0;
    uint32 mact_stamp;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0x0, sizeof(soc_reg_above_64_val_t));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, dbal_physical_table_id, key_size, payload_size, app_id, app_id_size, entry_encoding));

    

    SHR_IF_ERR_EXIT(mdb_em_db.mact_stamp.get(unit, &mact_stamp));

    if (mact_stamp == MDB_EM_MACT_STAMP_MAX)
    {
        mact_stamp = 1;
    }
    else
    {
        mact_stamp++;
    }

    SHR_IF_ERR_EXIT(mdb_em_db.mact_stamp.set(unit, mact_stamp));

    soc_mem_field_set(unit, MACT_CPU_REQUESTm, data, COMMANDf, &command);
    soc_mem_field_set(unit, MACT_CPU_REQUESTm, data, APP_DBf, &app_id);
    soc_mem_field_set(unit, MACT_CPU_REQUESTm, data, STAMPf, &mact_stamp);

    SHR_IF_ERR_EXIT(mdb_em_prepare_entry
                    (unit, dbal_physical_table_id, app_id, MACT_CPU_REQUESTm, key, key_size, payload, payload_size,
                     *entry_encoding, entry_offset, data));

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
    mdb_em_entry_encoding_e * entry_encoding)
{
    uint8 app_id_size = 0;
    uint32 entry_offset = MDB_EM_ENTRY_OFFSET;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.app_id_size.get(unit, dbal_physical_table_id, app_id, &app_id_size));

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, dbal_physical_table_id, key_size, payload_size, app_id, app_id_size, entry_encoding));

    soc_mem_field_set(unit, mem, data, COMMANDf, &command);
    soc_mem_field_set(unit, mem, data, TIDf, &app_id);

    if (!dnx_data_mdb.em.feature_get(unit, dnx_data_mdb_em_entry_type_parser))
    {
        uint32 entry_type = *entry_encoding;

        soc_mem_field_set(unit, mem, data, ENTRY_TYPEf, &entry_type);
    }

    SHR_IF_ERR_EXIT(mdb_em_prepare_entry
                    (unit, dbal_physical_table_id, app_id, mem, key, key_size, payload, payload_size, *entry_encoding,
                     entry_offset, data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_mact_get_physical_memory(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    soc_mem_t * mdb_physical_memory,
    soc_reg_t * mdb_physical_status_reg,
    int *blk,
    soc_reg_t * mdb_interrupt_reg,
    soc_field_t * interrupt_field)
{
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    *mdb_physical_memory = MACT_CPU_REQUESTm;

    

    *mdb_physical_status_reg = INVALIDr;
    *mdb_interrupt_reg = MACT_INTERRUPT_REGISTERr;
    *interrupt_field = LOCAL_MACT_INTf;

    
    instance = dbal_physical_table_id - dnx_data_mdb.em.mact_dbal_id_get(unit);
    *blk = MACT_BLOCK(unit, instance);

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_physical_memory(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    soc_mem_t * mdb_physical_memory,
    soc_reg_t * mdb_physical_status_reg,
    int *blk,
    soc_reg_t * mdb_interrupt_reg,
    soc_field_t * interrupt_field)
{
    SHR_FUNC_INIT_VARS(unit);

    *mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;
    *mdb_physical_status_reg = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg;
    *mdb_interrupt_reg = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->interrupt_register;
    *interrupt_field = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->interrupt_field;
    SHR_IF_ERR_EXIT(mdb_db_infos.em_interface_blk.get(unit, dbal_physical_table_id, blk));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_entry_check_status_handle_scan_locked(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;


    uint32 return_full = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (mdb_em_age_supported_internal(unit, mdb_physical_table_id) == TRUE)
    {

        

        uint32 entry_handle_table_22;
        uint32 scan_machine_auto_en;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit,
                         dnx_data_mdb.em.mdb_emp_tables_mapping_get(unit, mdb_physical_table_id)->mdb_22_table,
                         &entry_handle_table_22));

        

        dbal_value_field32_request(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                   &scan_machine_auto_en);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_table_22, DBAL_COMMIT));

        if (scan_machine_auto_en != 0)
        {
            uint32 emp_scan_status = 1;
            uint32 poll_counter = 0;
            soc_reg_t mdb_physical_status_reg = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg;
            soc_field_t emp_scan_status_field =
                dnx_data_mdb.em.em_emp_get(unit, dbal_physical_table_id)->emp_scan_status_field;
            int instance;

            instance = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->instance;

            
            if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype != MDB_EM_TYPE_LEM)
            {
                dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_22, DBAL_COMMIT));
            }

            

            while ((emp_scan_status != 0) && (poll_counter < MDB_EM_EMP_SCAN_STATUS_POLL_RETRY))
            {
                uint64 status_reg_data;

                SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, instance, 0, &status_reg_data));
                emp_scan_status =
                    soc_reg64_field_get(unit, mdb_physical_status_reg, status_reg_data, emp_scan_status_field);

                poll_counter++;
            }

            

            if (emp_scan_status == 0)
            {
                dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];
                dbal_physical_entry_t entry_duplicate;

                sal_memset(dbal_physical_table_id_arr, 0x0, sizeof(dbal_physical_table_id_arr));

                dbal_physical_table_id_arr[0] = dbal_physical_table_id;

                

                sal_memcpy(&entry_duplicate, entry, sizeof(entry_duplicate));
                entry_duplicate.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
                SHR_IF_ERR_EXIT(mdb_em_entry_add(unit, dbal_physical_table_id_arr, app_id, &entry_duplicate, 0x0));

                

                return_full = FALSE;
            }

            
            if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype != MDB_EM_TYPE_LEM)
            {
                dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                             scan_machine_auto_en);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_22, DBAL_COMMIT));
            }
        }

        DBAL_HANDLE_FREE(unit, entry_handle_table_22);
    }

    if (return_full == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "%s is full, em_status = %d.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_mact_entry_check_status(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    mdb_em_mact_cmd_e command,
    int *retry)
{
    shr_error_e res;
    uint32 fail_reason;

    

    uint32 success;
    uint32 reply_command = MDB_NOF_EM_MACT_CMD;
    uint32 reply_stamp = 0;
    soc_reg_above_64_val_t data;
    uint32 attempt_counter = 0;
    uint32 mact_stamp;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    *retry = FALSE;

    
    instance = dbal_physical_table_id - dnx_data_mdb.em.mact_dbal_id_get(unit);

    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_transplant_no_reply))
    {

        

        if (command == MDB_EM_MACT_CMD_TRANSPLANT)
        {
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT(mdb_em_db.mact_stamp.get(unit, &mact_stamp));

    

    while (((reply_command != MDB_EM_MACT_CMD_ACK) || (reply_stamp != mact_stamp))
           && (attempt_counter < MDB_EM_MACT_POLL_ATTEMPTS))
    {
        SHR_IF_ERR_EXIT(READ_MACT_LARGE_EM_REPLYr(unit, instance, data));

        soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, COMMANDf, &reply_command);

        soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, STAMPf, &reply_stamp);

        attempt_counter++;

        
        if ((reply_command == MDB_EM_MACT_CMD_ACK) && (reply_stamp == mact_stamp))
        {
            break;
        }

        

        if ((command == MDB_EM_MACT_CMD_DELETE)
            && (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_delete_transplant_no_reply)))
        {
            soc_reg_above_64_val_t delete_counter_data;
            uint32 counter_val;

            SHR_IF_ERR_EXIT(READ_MACT_LARGE_EM_ERROR_DELETE_NON_EXIST_COUNTERr(unit, instance, delete_counter_data));

            counter_val =
                soc_reg_field_get(unit, MACT_LARGE_EM_ERROR_DELETE_NON_EXIST_COUNTERr, delete_counter_data[0],
                                  LARGE_EM_ERROR_DELETE_NON_EXIST_COUNTERf);

            if (counter_val != 0)
            {

                

                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        

        if ((command == MDB_EM_MACT_CMD_TRANSPLANT)
            && (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_delete_transplant_no_reply)))
        {
            soc_reg_above_64_val_t transplant_counter_data;
            uint32 counter_val;

            SHR_IF_ERR_EXIT(READ_MACT_LARGE_EM_ERROR_TRANSPLANT_REQUEST_OVER_STRONGER_COUNTERr
                            (unit, instance, transplant_counter_data));

            counter_val =
                soc_reg_field_get(unit, MACT_LARGE_EM_ERROR_TRANSPLANT_REQUEST_OVER_STRONGER_COUNTERr,
                                  transplant_counter_data[0], LARGE_EM_ERROR_TRANSPLANT_REQUEST_OVER_STRONGER_COUNTERf);

            if (counter_val != 0)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "%s attempted to transplant over stronger existing key in MACT.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id));
            }

            
            if ((attempt_counter >= MDB_EM_MACT_TRANSPLANT_POLL_ATTEMPTS)
                && (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_olp_always_enters_source_cpu)))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "%s MACT entry operation failed with: %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_mact_fail_reason_strings[MDB_MACT_FAIL_REASON_EXCEED_LIMIT]);
            }
        }
    }

    if (attempt_counter >= MDB_EM_MACT_POLL_ATTEMPTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MACT reply is not ready within the maximal number of polling attempts.\n");
    }

    soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, SUCCESSf, &success);

    if (success != TRUE)
    {
        soc_mem_field_get(unit, MACT_CPU_REQUESTm, data, FAIL_REASONf, &fail_reason);

        

        if (fail_reason == MDB_MACT_FAIL_REASON_ECC_ERROR)
        {
            SHR_ERR_EXIT(_SHR_E_BUSY, "%s MACT entry operation failed with: %s.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id),
                         mdb_mact_fail_reason_strings[fail_reason]);
        }
        else if (fail_reason == MDB_MACT_FAIL_REASON_CAM_TABLE_LOCKED)
        {
            SHR_IF_ERR_EXIT(mdb_em_entry_check_status_handle_scan_locked(unit, dbal_physical_table_id, app_id, entry));
        }

        

        else if ((fail_reason == MDB_MACT_FAIL_REASON_CAM_TABLE_FULL)
                 || (fail_reason == MDB_MACT_FAIL_REASON_INVALID_TID_FOR_OFC)
                 || (fail_reason == MDB_MACT_FAIL_REASON_EXCEED_LIMIT)
                 || ((fail_reason == MDB_MACT_FAIL_REASON_FLUSH_DROP)
                     && (dnx_data_mdb.
                         feature.feature_get(unit, dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit))))
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "%s MACT entry operation failed with: %s.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id),
                         mdb_mact_fail_reason_strings[fail_reason]);
        }
        else if ((fail_reason == MDB_MACT_FAIL_REASON_DELETE_UNKNOWN_KEY)
                 || (fail_reason == MDB_MACT_FAIL_REASON_DELETE_NON_EXIST))
        {

            

            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else if (fail_reason == MDB_MACT_FAIL_REASON_INSERTED_EXISTING)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "%s attempted to insert existing key to MACT.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }
        else if (fail_reason == MDB_MACT_FAIL_REASON_TRANSPLANT_OVER_STRONGER)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "%s attempted to transplant over stronger existing key in MACT.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s MACT entry operation failed with: %s, command: %d, success: %d.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id),
                         mdb_mact_fail_reason_strings[fail_reason], reply_command, success);
        }
    }

    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_transplant_no_reply))
    {

        

        if (command == MDB_EM_MACT_CMD_INSERT)
        {
            dbal_physical_entry_t entry_duplicate;
            sal_memcpy(&entry_duplicate, entry, sizeof(*entry));
            entry_duplicate.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0);
            if (res == _SHR_E_NOT_FOUND)
            {
                fail_reason = MDB_MACT_FAIL_REASON_CAM_TABLE_FULL;
                SHR_ERR_EXIT(_SHR_E_FULL, "%s MACT entry operation failed with: %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_mact_fail_reason_strings[fail_reason]);
            }
            else if (res != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(res);
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_check_status(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    soc_reg_t mdb_physical_status_reg,
    soc_reg_t mdb_interrupt_reg,
    soc_field_t interrupt_field,
    mdb_em_cmd_e command,
    int *retry)
{
    shr_error_e res;
    uint32 interrupt_value = 0;
    uint32 interrupt_reg_data;
    uint64 status_reg_data;
    mdb_em_fail_reason_e em_status;
    uint32 attempt_counter = 0;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    *retry = FALSE;

    instance = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->instance;

    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown))
    {

        

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, mdb_interrupt_reg, instance, 0, &interrupt_reg_data));
        interrupt_value = soc_reg_field_get(unit, mdb_interrupt_reg, interrupt_reg_data, interrupt_field);
        em_status = MDB_EM_FAIL_REASON_CLEAR;
    }
    else
    {

        

        interrupt_value = TRUE;
    }

    if (interrupt_value != 0)
    {

        

        int relevant_interrupt = TRUE;

        do
        {

            

            SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, instance, 0, &status_reg_data));
            em_status = soc_reg64_field_get(unit, mdb_physical_status_reg, status_reg_data, ITEM_0_3f);
            attempt_counter++;
        }
        while ((em_status == MDB_EM_FAIL_REASON_HOST_STATUS_WORKING) && (attempt_counter < MDB_EM_MACT_POLL_ATTEMPTS));

        if (attempt_counter >= MDB_EM_MACT_POLL_ATTEMPTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "EM reply is not ready within the maximal number of polling attempts.\n");
        }

        

        if (em_status == MDB_EM_FAIL_REASON_NOT_PROCESSED)
        {
            *retry = TRUE;
        }
        else if (em_status == MDB_EM_FAIL_REASON_CLEAR)
        {
            relevant_interrupt = FALSE;
        }
        else if (command == MDB_EM_CMD_INSERT)
        {
            if ((em_status == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                || (em_status == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC)
                || (em_status == MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED))
            {
                dbal_physical_entry_t entry_duplicate;

                

                sal_memcpy(&entry_duplicate, entry, sizeof(entry_duplicate));
                res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0);
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
            SHR_IF_ERR_EXIT(soc_reg_set(unit, mdb_interrupt_reg, instance, 0, interrupt_reg_data));

            if (em_status == MDB_EM_FAIL_REASON_ECC_ERROR)
            {
                SHR_ERR_EXIT(_SHR_E_BUSY, "%s entry operation failed with: %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id),
                             mdb_em_fail_reason_strings[em_status]);
            }
            else if (em_status == MDB_EM_FAIL_REASON_CAM_TABLE_LOCKED)
            {
                SHR_IF_ERR_EXIT(mdb_em_entry_check_status_handle_scan_locked
                                (unit, dbal_physical_table_id, app_id, entry));
            }
            else if ((em_status == MDB_EM_FAIL_REASON_CAM_TABLE_FULL)
                     || (em_status == MDB_EM_FAIL_REASON_INVALID_TID_FOR_OFC))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "%s is full, em_status = %d.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id), em_status);
            }
            else if (em_status == MDB_EM_FAIL_REASON_DELETE_UNKNOWN_KEY)
            {

                

                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else if (em_status == MDB_EM_FAIL_REASON_NOT_PROCESSED)
            {

                

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

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_get_min_remaining_entry_count(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *free_entry_count)
{
    mdb_em_entry_encoding_e entry_encoding;
    int capacity_estimate;

    SHR_FUNC_INIT_VARS(unit);

    

    for (entry_encoding = MDB_EM_ENTRY_ENCODING_ONE; entry_encoding < MDB_EM_ENTRY_ENCODING_EMPTY; entry_encoding++)
    {
        uint8 vmv_size, vmv_value;

        SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

        

        if (vmv_size != 0)
        {
            break;
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        capacity_estimate = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->capacity;
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.em_entry_capacity_estimate.get(unit, dbal_physical_table_id, entry_encoding,
                                                                    &capacity_estimate));
    }

    if (capacity_estimate != 0)
    {
        int hw_entry_count;

        SHR_IF_ERR_EXIT(mdb_em_hw_entry_count_get(unit, dbal_physical_table_id, &hw_entry_count));

        

        if (capacity_estimate > hw_entry_count)
        {
            *free_entry_count = capacity_estimate - hw_entry_count;
        }
        else
        {
            *free_entry_count = 0;
        }
    }
    else
    {
        *free_entry_count = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_hw_entry_count_decrease(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    soc_reg_t mdb_physical_status_reg;
    uint64 status_reg_data;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    mdb_physical_status_reg = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg;

    sal_memset(&status_reg_data, 0x0, sizeof(uint64));

    
    soc_reg64_field_set(unit, mdb_physical_status_reg, &status_reg_data, ITEM_36_36f, 0x1);

    instance = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->instance;
    SHR_IF_ERR_EXIT(soc_reg_set(unit, mdb_physical_status_reg, instance, 0, status_reg_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_hw_entry_count_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *count)
{
    soc_reg_t mdb_physical_status_reg;
    uint64 status_reg_data;
    uint64 entry_count;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    mdb_physical_status_reg = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg;

    instance = dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->instance;
    SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, instance, 0, &status_reg_data));
    entry_count = soc_reg64_field_get(unit, mdb_physical_status_reg, status_reg_data, ITEM_4_35f);
    *count = entry_count;

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
    uint8 key_size_u8;

    SHR_FUNC_INIT_VARS(unit);

    if (app_id >= DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected app_id %d.\n", app_id);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.key_size.get(unit, dbal_physical_table_id, app_id, &key_size_u8));

    *key_size = key_size_u8;

exit:
    SHR_FUNC_EXIT;
}


int
mdb_em_age_supported(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    return mdb_em_age_supported_internal(unit,
                                         dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                             dbal_physical_table_id)->logical_to_physical);
}


int
mdb_em_age_supported_internal(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    int is_age_supported_temp;

    
    if ((dnx_data_mdb.em.mdb_emp_tables_mapping_get(unit, mdb_physical_table_id)->emp_age_cfg_table != 0)
        && (mdb_em_hitbit_supported_internal(unit, mdb_physical_table_id) == TRUE))
    {
        is_age_supported_temp = TRUE;
    }
    else
    {
        is_age_supported_temp = FALSE;
    }

    return is_age_supported_temp;
}


int
mdb_em_hitbit_supported(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    return mdb_em_hitbit_supported_internal(unit,
                                            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                                dbal_physical_table_id)->logical_to_physical);
}


int
mdb_em_hitbit_supported_internal(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    int is_hitbit_supported_temp;

    {
        is_hitbit_supported_temp = TRUE;
    }

    return is_hitbit_supported_temp;
}

shr_error_e
mdb_em_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    soc_reg_above_64_val_t data;
    int blk;
    bsl_severity_t severity;
    uint32 key_size = 0;
    soc_mem_t mdb_physical_memory_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { INVALIDm };
    soc_reg_t mdb_physical_status_reg;

    

    uint32 command;
    uint32 rv;
    soc_reg_t mdb_interrupt_reg;
    soc_field_t interrupt_field;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_NOF_ENTRY_ENCODINGS;
    uint32 retry_counter = 0;
    int entry_count;

    
    int retry = TRUE;
    uint32 is_mact;

    
    uint8 is_update = FALSE;
    int tables_num = 0, table_iter;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0x0, sizeof(soc_reg_above_64_val_t));

    SHR_IF_ERR_EXIT(mdb_num_valid_tables(unit, dbal_physical_table_id_arr, &tables_num));

    SHR_IF_ERR_EXIT(mdb_em_is_mact(unit, dbal_physical_table_id_arr[0], app_id, entry->payload_size, &is_mact));

    for (table_iter = 0; table_iter < tables_num; table_iter++)
    {

        
        if ((!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_mact_transplant_no_reply)) &&
            (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY) &&
            (entry->indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_FORCE))
        {
            is_update = TRUE;
        }
        else if ((entry->indirect_commit_mode != DBAL_INDIRECT_COMMIT_MODE_NORMAL)
                 && (entry->mdb_action_apply != DBAL_MDB_ACTION_APPLY_NONE) && ((is_mact) || (table_iter == 0)))
        {
            
            shr_error_e rv_get;
            dbal_physical_entry_t get_entry;

            sal_memcpy(&get_entry, entry, sizeof(get_entry));

            
            if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) && !is_mact)
            {
                get_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
            }
            get_entry.payload_size = 0;
            get_entry.hitbit = 0;
            get_entry.age = 0;
            rv_get = mdb_em_entry_get(unit, dbal_physical_table_id_arr[table_iter], app_id, &get_entry, 0);

            
            get_entry.mdb_action_apply = entry->mdb_action_apply;
            if (rv_get == _SHR_E_NONE)
            {
                
                if (get_entry.payload_size != entry->payload_size)
                {
                    
                    if (is_mact)
                    {
                        dbal_physical_tables_e dbal_physical_table_id_arr_temp[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };

                        dbal_physical_table_id_arr_temp[0] = dbal_physical_table_id_arr[table_iter];

                        SHR_IF_ERR_EXIT(mdb_em_entry_delete
                                        (unit, dbal_physical_table_id_arr_temp, app_id, &get_entry, 0));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, dbal_physical_table_id_arr, app_id, &get_entry, 0));
                    }

                    is_update = FALSE;
                }
                else
                {
                    is_update = TRUE;
                }
            }
            else
            {
                
                if (entry->indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_UPDATE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "%s, error, update flag is set but the entry does not exist.\n",
                                 dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[0]));
                }
                else
                {
                    is_update = FALSE;
                }
            }
        }

        SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id_arr[table_iter], app_id, &key_size));

        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
        {
            if (is_mact)
            {
                SHR_IF_ERR_EXIT(mdb_em_mact_get_physical_memory
                                (unit, dbal_physical_table_id_arr[table_iter], &mdb_physical_memory_arr[table_iter],
                                 &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

                if (is_update)
                {
                    command = MDB_EM_MACT_CMD_TRANSPLANT;
                }
                else
                {
                    command = MDB_EM_MACT_CMD_INSERT;
                }

                SHR_IF_ERR_EXIT(mdb_em_mact_prepare_command
                                (unit, dbal_physical_table_id_arr[table_iter], app_id, command,
                                 entry->key, key_size, entry->payload, entry->payload_size, data, &entry_encoding));
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                                (unit, dbal_physical_table_id_arr[table_iter], &mdb_physical_memory_arr[table_iter],
                                 &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

                if (is_update)
                {
                    command = MDB_EM_CMD_TRANSPLANT;
                }
                else
                {
                    command = MDB_EM_CMD_INSERT;
                }

                SHR_IF_ERR_EXIT(mdb_em_prepare_command
                                (unit, dbal_physical_table_id_arr[table_iter], app_id, command,
                                 mdb_physical_memory_arr[table_iter], entry->key, key_size, entry->payload,
                                 entry->payload_size, data, &entry_encoding));
            }

            do
            {
                if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown))
                {

                    

                    rv = sand_fill_partial_table_with_entry(unit, mdb_physical_memory_arr[table_iter], 0, 0, blk, 0, 0,
                                                            data);
                    if (rv != _SHR_E_NONE)
                    {

                        

                        SHR_ERR_EXIT(_SHR_E_FULL, "Entry add failed in mdb_em_entry_add, %s is full.\n",
                                     dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[table_iter]));
                    }

                    if (is_mact)
                    {
                        SHR_IF_ERR_EXIT(mdb_em_mact_entry_check_status
                                        (unit, dbal_physical_table_id_arr[table_iter], app_id, entry, command, &retry));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(mdb_em_entry_check_status
                                        (unit, dbal_physical_table_id_arr[table_iter], app_id, entry,
                                         mdb_physical_status_reg, mdb_interrupt_reg, interrupt_field, command, &retry));
                    }
                }
                else
                {

                    

                    if ((!is_mact)
                        &&
                        (dnx_sbusdma_desc_is_enabled
                         (unit, mdb_em_dbal_table_to_dma_enum(unit, dbal_physical_table_id_arr[table_iter])) == TRUE))
                    {
                        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem
                                        (unit, mdb_physical_memory_arr[table_iter], 0, blk, 0, data));
                        retry = FALSE;
                    }
                    else
                    {
                        rv = soc_mem_write(unit, mdb_physical_memory_arr[table_iter], blk, 0, data);
                        if (rv != _SHR_E_NONE)
                        {

                            

                            if (rv == _SHR_E_TIMEOUT)
                            {
                                SHR_IF_ERR_EXIT(mdb_em_entry_check_status
                                                (unit, dbal_physical_table_id_arr[table_iter], app_id, entry,
                                                 mdb_physical_status_reg, mdb_interrupt_reg, interrupt_field, command,
                                                 &retry));
                            }
                            else
                            {
                                SHR_ERR_EXIT(rv, "MDB EM entry add - unexpected soc_mem_write failure: %s.\n",
                                             shrextend_errmsg_get(rv));
                            }
                        }
                        else
                        {
                            if (is_mact)
                            {

                                

                                SHR_IF_ERR_EXIT(mdb_em_mact_entry_check_status
                                                (unit, dbal_physical_table_id_arr[table_iter], app_id, entry, command,
                                                 &retry));
                            }
                            else
                            {
                                retry = FALSE;
                            }
                        }
                    }
                }

                retry_counter++;
            }
            while ((retry_counter < MDB_EM_MACT_NOT_PROCESSED_RETRY) && (retry == TRUE));

            if (retry_counter >= MDB_EM_MACT_NOT_PROCESSED_RETRY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "EM not available for processing add request.\n");
            }
        }

        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW))
        {
            SHR_IF_ERR_EXIT(mdb_em_shadow_add_key(unit, dbal_physical_table_id_arr[table_iter], app_id, entry));

            
            if (!(is_update))
            {
                if (entry_encoding == MDB_EM_NOF_ENTRY_ENCODINGS)
                {
                    
                    uint8 app_id_size = 0;

                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                    app_id_size.get(unit, dbal_physical_table_id_arr[table_iter], app_id,
                                                    &app_id_size));

                    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                    (unit, dbal_physical_table_id_arr[table_iter], key_size,
                                     entry->payload_size, app_id, app_id_size, &entry_encoding));
                }

                SHR_IF_ERR_EXIT(mdb_db_infos.
                                em_entry_count.get(unit, dbal_physical_table_id_arr[table_iter], entry_encoding,
                                                   &entry_count));
                entry_count++;
                SHR_IF_ERR_EXIT(mdb_db_infos.
                                em_entry_count.set(unit, dbal_physical_table_id_arr[table_iter], entry_encoding,
                                                   entry_count));

                
                if (entry->indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_FORCE)
                {
                    entry->indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_NORMAL;
                }
            }
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            SHR_IF_ERR_EXIT(adapter_mdb_access_em_write(unit, dbal_physical_table_id_arr[table_iter], app_id, entry));
        }
    }
#endif

exit:
    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if ((severity >= bslSeverityVerbose) && (mdb_physical_memory_arr[0] != INVALIDm))
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, tables_num: %d, app_id: %d.\n"),
                     key_size, entry->payload_size, tables_num, app_id));
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "DBAL table %d: %s.\n"), table_iter,
                         dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[table_iter])));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "SW: %s. HW: %s.\n"),
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)) ? "TRUE" : "FALSE",
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY)) ? "TRUE" : "FALSE"));

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
            for (table_iter = 0; table_iter < tables_num; table_iter++)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Written to memory: %s.\n"),
                             SOC_MEM_NAME(unit, mdb_physical_memory_arr[table_iter])));
            }
            for (data_offset = 0; data_offset < BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory_arr[0]).bytes);
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory_arr[0]).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Written data[%d]: %08X.\n"), print_index, data[print_index]));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_add: end\n")));
    }
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_dma_mode_set(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint8 enable,
    uint8 *dma_mode_changed)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown) &&
        dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_ecc_simple_command_deadlock))
    {
        uint32 entry_handle_mdb_15_table;
        uint32 current_dma_mode;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_15_info_get(unit, dbal_physical_table_id)->dbal_table,
                         &entry_handle_mdb_15_table));

        dbal_value_field32_request(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_8, INST_SINGLE,
                                   &current_dma_mode);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_15_table, DBAL_COMMIT));

        

        if (current_dma_mode != enable)
        {
            if (dma_mode_changed != NULL)
            {
                *dma_mode_changed = TRUE;
            }

            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_8, INST_SINGLE, enable);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_15_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_15_table);
        }
    }
    else
    {
        if (dma_mode_changed != NULL)
        {
            *dma_mode_changed = FALSE;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    soc_reg_above_64_val_t write_data, read_data;
    int blk;
    bsl_severity_t severity;
    uint32 found = 0;
    uint32 key_size = 0;
    soc_mem_t mdb_physical_memory = INVALIDm;
    uint32 retrieve_hitbit_or_age = 0;
    uint8 dma_mode_changed = FALSE;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_ENTRY_ENCODING_ONE;
    int retry = FALSE;
    uint32 rv;
    uint8 hw_access = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(write_data, 0x0, sizeof(soc_reg_above_64_val_t));

    SHR_IF_ERR_EXIT(mdb_db_infos.em_interface_blk.get(unit, dbal_physical_table_id, &blk));

#ifdef ADAPTER_SERVER_MODE
    
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit) &&
        (dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->memory_id !=
         MDB_ADAPTER_MEMORY_ID_INVALID)
        && (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype != MDB_EM_TYPE_GLEM))
    {
        rv = adapter_mdb_access_em_read(unit, dbal_physical_table_id, app_id, entry);
        SHR_SET_CURRENT_ERR(rv);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        key_size = entry->key_size;
    }
    else
#endif
    {

#if !defined(PLISIM) && !defined(ADAPTER_SERVER_MODE)
        retrieve_hitbit_or_age = ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
                                  || (entry->age & DBAL_PHYSICAL_KEY_AGE_GET));
#endif

        if ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION) &&
            (mdb_em_hitbit_supported(unit, dbal_physical_table_id) != TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "%s does not support hitbits in current MDB profile.\n", dbal_physical_table_to_string(unit,
                                                                                                                dbal_physical_table_id));
        }

        if ((entry->age & DBAL_PHYSICAL_KEY_AGE_GET) && (mdb_em_age_supported(unit, dbal_physical_table_id) != TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "%s does not support age bits in current MDB profile.\n", dbal_physical_table_to_string(unit,
                                                                                                                 dbal_physical_table_id));
        }

        SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &key_size));

        

        mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;

        if (mdb_physical_memory == INVALIDm)
        {
            SHR_ERR_EXIT(_SHR_E_BADID,
                         "Error. dbal_physical_table %s is not associated with an exact match memory.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }

        

        if (((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
             || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)) && !retrieve_hitbit_or_age)
        {
            SHR_IF_ERR_EXIT(mdb_em_shadow_get(unit, dbal_physical_table_id, app_id, entry, &found));
        }

        if (((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY) ||
             ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) && (!found))) || retrieve_hitbit_or_age)
        {

            hw_access = TRUE;
            SHR_IF_ERR_EXIT(mdb_em_prepare_command
                            (unit, dbal_physical_table_id, app_id, (uint32) MDB_EM_CMD_LOOKUP,
                             mdb_physical_memory, entry->key, key_size, NULL, entry->payload_size, write_data,
                             &entry_encoding));

            

            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(mdb_em_dma_mode_set
                                                       (unit, dbal_physical_table_id, FALSE, &dma_mode_changed));

            rv = soc_mem_write(unit, mdb_physical_memory, blk, 0, write_data);

            

            if ((rv != _SHR_E_NONE) && (rv != _SHR_E_TIMEOUT))
            {
                SHR_ERR_EXIT(rv, "MDB EM entry get - unexpected soc_mem_write failure: %s.\n",
                             shrextend_errmsg_get(rv));
            }

            SHR_IF_ERR_EXIT(mdb_em_entry_check_status
                            (unit, dbal_physical_table_id, app_id, entry,
                             dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->status_reg,
                             dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->interrupt_register,
                             dnx_data_mdb.em.em_interrupt_get(unit, dbal_physical_table_id)->interrupt_field,
                             MDB_EM_CMD_LOOKUP, &retry));

            

            SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, read_data));
            SHR_IF_ERR_EXIT(mdb_em_entry_extract_payload
                            (unit, dbal_physical_table_id, mdb_physical_memory,
                             read_data, key_size, entry->payload_size, entry->payload, &found, &entry_encoding));
        }

        if (found && retrieve_hitbit_or_age)
        {
             
            uint32 way, prefix, address;
            uint32 way_post, prefix_post, address_post;
            uint32 attempt_counter = 0;

            
            soc_mem_field_get(unit, mdb_physical_memory, read_data, WAY_INDEXf, &way);
            soc_mem_field_get(unit, mdb_physical_memory, read_data, PREFIX_NUMf, &prefix);
            soc_mem_field_get(unit, mdb_physical_memory, read_data, ADDRESSf, &address);

            

            do
            {
                attempt_counter++;
                
                if (entry->age & DBAL_PHYSICAL_KEY_AGE_GET)
                {
                    soc_mem_t em_age_mem = dnx_data_mdb.em.em_emp_get(unit, dbal_physical_table_id)->age_mem;
                    soc_mem_t em_age_ovf_cam_mem =
                        dnx_data_mdb.em.em_emp_get(unit, dbal_physical_table_id)->age_ovf_cam_mem;

                    

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
                    uint32 is_mact;
                    uint8 max_nof_age_entry_bits;
                    mdb_physical_table_e mdb_physical_table =
                        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

                    SHR_IF_ERR_EXIT(mdb_em_is_mact
                                    (unit, dbal_physical_table_id, app_id, entry->payload_size, &is_mact));

                    ovf_pos =
                        dnx_data_mdb.dh.dh_info_get(unit,
                                                    mdb_physical_table)->table_macro_interface_mapping
                        [ovf_pos_in_array];

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
                        uint32 mem_array_index = 0;

                        

                        age_cnts_ratio = dnx_data_mdb.em.age_row_size_bits_get(unit) / age_cnt_em_row_width;

                        

                        age_entry_address = (bank_start_ptr * 8 + address) / age_cnts_ratio;


                        SHR_IF_ERR_EXIT(soc_mem_array_read
                                        (unit, em_age_mem, mem_array_index, blk, age_entry_address, age_mem_row));

                        

                        em_entry_age_cnts = 0;
                        SHR_BITCOPY_RANGE(&em_entry_age_cnts, 0x0, age_mem_row,
                                          (address % age_cnts_ratio) * age_cnt_em_row_width, age_cnt_em_row_width);
                    }

                    
                    max_nof_age_entry_bits =
                        dnx_data_mdb.em.em_aging_info_get(unit, dbal_physical_table_id)->max_nof_age_entry_bits;

                    age_cnt_em_base_width =
                        age_cnt_em_row_width /
                        (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width /
                         MDB_DIRECT_BASIC_ENTRY_SIZE);
                    age_cnt_em_entry_width = age_cnt_em_row_width / (1 << entry_encoding);
                    age_cnt_em_entry_width =
                        (age_cnt_em_entry_width >
                         max_nof_age_entry_bits) ? max_nof_age_entry_bits : age_cnt_em_entry_width;

                    

                    entry->age = 0;
                    SHR_BITCOPY_RANGE(&entry->age, 0x0, &em_entry_age_cnts,
                                      ((prefix + 1) * age_cnt_em_base_width) - age_cnt_em_entry_width,
                                      age_cnt_em_entry_width - dnx_data_mdb.em.em_aging_info_get(unit,
                                                                                                 dbal_physical_table_id)->rbd_size);
                }

                
                if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
                {
                    soc_reg_above_64_val_t hitbit_data;
                    soc_reg_above_64_val_t hitbit_result;
                    uint32 command, hits_a, hits_b;

                    sal_memcpy(hitbit_data, read_data, sizeof(read_data));

                    
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
                    
                    SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, hitbit_result));
                    soc_mem_field_get(unit, mdb_physical_memory, hitbit_result, HITSf, &hits_a);

                    
                    if (((hits_a >> prefix) & 0x1) == 1)
                    {
                        entry->hitbit |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
                    }
                    else
                    {
                        entry->hitbit &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
                    }

                    
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

                    
                    SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, hitbit_result));
                    soc_mem_field_get(unit, mdb_physical_memory, hitbit_result, HITSf, &hits_b);

                    
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

        if (found == 0)
        {

            

            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

exit:
    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_get: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                     key_size, entry->payload_size, dbal_physical_table_to_string(unit,
                                                                                  dbal_physical_table_id), app_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "SW: %s. HW: %s.\n"),
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)) ? "TRUE" : "FALSE",
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY)) ? "TRUE" : "FALSE"));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->mdb_action_apply: %d.\n"), entry->mdb_action_apply));

        

        if (hw_access == TRUE)
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

    

    if (dma_mode_changed == TRUE)
    {
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_CONT(mdb_em_dma_mode_set(unit, dbal_physical_table_id, TRUE, NULL));
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_em_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    soc_reg_above_64_val_t write_data;
    int blk;
    bsl_severity_t severity;
    uint32 key_size;
    soc_mem_t mdb_physical_memory_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { INVALIDm };
    soc_reg_t mdb_physical_status_reg;

    

    uint32 command;
    soc_field_t interrupt_field;
    soc_reg_t mdb_interrupt_reg;
    mdb_em_entry_encoding_e entry_encoding = MDB_EM_NOF_ENTRY_ENCODINGS;
    int entry_count;
    uint32 retry_counter = 0;

    

    int retry = TRUE;
    uint32 is_mact;
    int tables_num = 0, table_iter;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(write_data, 0x0, sizeof(soc_reg_above_64_val_t));

    SHR_IF_ERR_EXIT(mdb_num_valid_tables(unit, dbal_physical_table_id_arr, &tables_num));

    for (table_iter = 0; table_iter < tables_num; table_iter++)
    {
        SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id_arr[table_iter], app_id, &key_size));

        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
        {
            SHR_IF_ERR_EXIT(mdb_em_is_mact
                            (unit, dbal_physical_table_id_arr[table_iter], app_id, entry->payload_size, &is_mact));

            if (is_mact)
            {
                SHR_IF_ERR_EXIT(mdb_em_mact_get_physical_memory
                                (unit, dbal_physical_table_id_arr[table_iter], &mdb_physical_memory_arr[table_iter],
                                 &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

                command = MDB_EM_MACT_CMD_DELETE;

                SHR_IF_ERR_EXIT(mdb_em_mact_prepare_command
                                (unit, dbal_physical_table_id_arr[table_iter], app_id, command,
                                 entry->key, key_size, entry->payload, entry->payload_size, write_data,
                                 &entry_encoding));
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                                (unit, dbal_physical_table_id_arr[table_iter], &mdb_physical_memory_arr[table_iter],
                                 &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

                command = MDB_EM_CMD_DELETE;

                SHR_IF_ERR_EXIT(mdb_em_prepare_command
                                (unit, dbal_physical_table_id_arr[table_iter], app_id, command,
                                 mdb_physical_memory_arr[table_iter], entry->key, key_size, entry->payload,
                                 entry->payload_size, write_data, &entry_encoding));
            }

            do
            {
                if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown))
                {

                    

                    SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry
                                    (unit, mdb_physical_memory_arr[table_iter], 0, 0, blk, 0, 0, write_data));

                    if (is_mact)
                    {
                        SHR_IF_ERR_EXIT(mdb_em_mact_entry_check_status
                                        (unit, dbal_physical_table_id_arr[table_iter], app_id, entry, command, &retry));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(mdb_em_entry_check_status
                                        (unit, dbal_physical_table_id_arr[table_iter], app_id, entry,
                                         mdb_physical_status_reg, mdb_interrupt_reg, interrupt_field, command, &retry));
                    }
                }
                else
                {

                    

                    if ((!is_mact)
                        &&
                        (dnx_sbusdma_desc_is_enabled
                         (unit, mdb_em_dbal_table_to_dma_enum(unit, dbal_physical_table_id_arr[table_iter])) == TRUE))
                    {
                        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem
                                        (unit, mdb_physical_memory_arr[table_iter], 0, blk, 0, write_data));
                        retry = FALSE;
                    }
                    else
                    {
                        shr_error_e rv;

                        rv = soc_mem_write(unit, mdb_physical_memory_arr[table_iter], blk, 0, write_data);
                        if (rv != _SHR_E_NONE)
                        {

                            

                            if (rv == _SHR_E_TIMEOUT)
                            {
                                SHR_IF_ERR_EXIT(mdb_em_entry_check_status
                                                (unit, dbal_physical_table_id_arr[table_iter], app_id, entry,
                                                 mdb_physical_status_reg, mdb_interrupt_reg, interrupt_field,
                                                 MDB_EM_CMD_DELETE, &retry));
                            }
                            else
                            {
                                SHR_ERR_EXIT(rv, "MDB EM entry delete - unexpected soc_mem_write failure: %s.\n",
                                             shrextend_errmsg_get(rv));
                            }
                        }
                        else
                        {
                            if (is_mact)
                            {

                                

                                SHR_IF_ERR_EXIT(mdb_em_mact_entry_check_status
                                                (unit, dbal_physical_table_id_arr[table_iter], app_id, entry, command,
                                                 &retry));
                            }
                            else
                            {
                                retry = FALSE;
                            }
                        }
                    }
                }

                retry_counter++;
            }
            while ((retry_counter < MDB_EM_MACT_NOT_PROCESSED_RETRY) && (retry == TRUE));

            if (retry_counter >= MDB_EM_MACT_NOT_PROCESSED_RETRY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "EM not available for processing delete request.\n");
            }

        }

        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW))
        {
            shr_error_e rv;

            rv = mdb_em_shadow_remove_key(unit, dbal_physical_table_id_arr[table_iter], app_id, entry);

            if (rv == _SHR_E_NONE)
            {
                

                if (entry_encoding == MDB_EM_NOF_ENTRY_ENCODINGS)
                {

                    

                    uint8 app_id_size = 0;

                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                    app_id_size.get(unit, dbal_physical_table_id_arr[table_iter], app_id,
                                                    &app_id_size));

                    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                    (unit, dbal_physical_table_id_arr[table_iter], entry->key_size,
                                     entry->payload_size, app_id, app_id_size, &entry_encoding));
                }
                SHR_IF_ERR_EXIT(mdb_db_infos.
                                em_entry_count.get(unit, dbal_physical_table_id_arr[table_iter], entry_encoding,
                                                   &entry_count));
                entry_count--;
                SHR_IF_ERR_EXIT(mdb_db_infos.
                                em_entry_count.set(unit, dbal_physical_table_id_arr[table_iter], entry_encoding,
                                                   entry_count));
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "mdb_em_shadow_remove_key failed unexpectedly.\n");
            }
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id_arr[0])->db_subtype == MDB_EM_TYPE_GLEM)
        {
            SHR_IF_ERR_EXIT(adapter_mdb_access_em_delete(unit, dbal_physical_table_id_arr[0], app_id, entry));
        }
        else
        {
            for (table_iter = 0; table_iter < tables_num; table_iter++)
            {
                SHR_IF_ERR_EXIT(adapter_mdb_access_em_delete
                                (unit, dbal_physical_table_id_arr[table_iter], app_id, entry));
            }
        }
    }
#endif

exit:
    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if ((severity >= bslSeverityVerbose) && (mdb_physical_memory_arr[0] != INVALIDm))
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_delete: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry->mdb_action_apply: %d.\n"), entry->mdb_action_apply));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "key_size: %d. entry->payload_size: %d, tables_num: %d, app_id: %d.\n"),
                     key_size, entry->payload_size, tables_num, app_id));
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "DBAL table %d: %s.\n"), table_iter,
                         dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[table_iter])));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "SW: %s. HW: %s.\n"),
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)) ? "TRUE" : "FALSE",
                     ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                      || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY)) ? "TRUE" : "FALSE"));

        for (data_offset = 0; data_offset < BITS2WORDS(key_size); data_offset++)
        {
            print_index = BITS2WORDS(key_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Deleted from memory: %s.\n"),
                         SOC_MEM_NAME(unit, mdb_physical_memory_arr[table_iter])));
        }
        if ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_HW_ONLY))
        {
            for (data_offset = 0; data_offset < (BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory_arr[0]).bytes));
                 data_offset++)
            {
                print_index = BYTES2WORDS(SOC_MEM_INFO(unit, mdb_physical_memory_arr[0]).bytes) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Written data[%d]: %08X.\n"), print_index, write_data[print_index]));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_em_entry_delete: end\n")));
    }
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_delete_by_addr(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int cluster_addr)
{

    int blk;
    mdb_physical_table_e assoc_mdb_physical_table_id;
    int nof_clusters;
    int cluster_idx;
    int cluster_offset = 0;
    dbal_physical_tables_e dbal_physical_table_id;
    soc_mem_t mdb_physical_memory = INVALIDm;
    soc_reg_above_64_val_t write_data;
    uint32 field_val;
    int global_macro_index;

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc
                    (unit, macro_type, macro_index, cluster_index, &assoc_mdb_physical_table_id));
    dbal_physical_table_id = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, assoc_mdb_physical_table_id)->physical_to_logical;

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "The provided macro_type(%d), macro_index(%d), cluster_index(%d) are associated with %s which is not an MDB EM table.\n",
                     macro_type, macro_index, cluster_index, dbal_physical_table_to_string(unit,
                                                                                           dbal_physical_table_id));
    }

    

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, assoc_mdb_physical_table_id, &nof_clusters));

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        mdb_cluster_info_t cluster_info;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        clusters_info.get(unit, assoc_mdb_physical_table_id, cluster_idx, &cluster_info));

        if ((cluster_info.macro_type == macro_type) && (cluster_info.macro_index == macro_index)
            && (cluster_info.cluster_index == cluster_index))
        {
            cluster_offset = cluster_info.start_address;
            break;
        }
    }

    if (cluster_idx >= nof_clusters)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unable to find the cluster info of macro_type(%d), macro_index(%d), cluster_index(%d) which is associated with %s.\n",
                     macro_type, macro_index, cluster_index, dbal_physical_table_to_string(unit,
                                                                                           dbal_physical_table_id));
    }

    mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;
    SHR_IF_ERR_EXIT(mdb_db_infos.em_interface_blk.get(unit, dbal_physical_table_id, &blk));

    sal_memset(write_data, 0x0, sizeof(soc_reg_above_64_val_t));

    

    global_macro_index = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->global_start_index + macro_index;
    field_val =
        dnx_data_mdb.dh.dh_info_get(unit,
                                    assoc_mdb_physical_table_id)->table_macro_interface_mapping[global_macro_index];
    soc_mem_field_set(unit, mdb_physical_memory, write_data, WAY_INDEXf, &field_val);

    field_val =
        (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, assoc_mdb_physical_table_id)->row_width /
         MDB_DIRECT_BASIC_ENTRY_SIZE) - 1;
    soc_mem_field_set(unit, mdb_physical_memory, write_data, PREFIX_NUMf, &field_val);

    field_val = cluster_addr + cluster_offset;
    soc_mem_field_set(unit, mdb_physical_memory, write_data, ADDRESSf, &field_val);

    field_val = MDB_EM_ENTRY_ENCODING_ONE;
    soc_mem_field_set(unit, mdb_physical_memory, write_data, ENTRY_TYPEf, &field_val);

    field_val = MDB_EM_CMD_DELETE_BY_ADDR;
    soc_mem_field_set(unit, mdb_physical_memory, write_data, COMMANDf, &field_val);

    SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, write_data));

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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_action_apply != DBAL_MDB_ACTION_APPLY_HW_ONLY)
    {

        

        SW_STATE_HASH_TABLE_ITER_SET_BEGIN(&(physical_entry_iterator->mdb_em_htb_iter));
    }
    else
    {


        mdb_physical_table_e mdb_physical_table_id =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
        uint32 entry_handle_mdb_15_table, entry_handle_mdb_16_table, entry_handle_mdb_23_table;
        uint32 row_width;

        row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->row_width;

        physical_entry_iterator->mdb_em_ratios =
            (1 << ((row_width / MDB_DIRECT_BASIC_ENTRY_SIZE) * MDB_EM_ENTRY_TYPE_SIZE)) - 1;
        physical_entry_iterator->mdb_entry_index = SAL_UINT32_MAX;
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(mdb_em_dma_mode_set(unit, dbal_physical_table_id, FALSE, NULL));

        

        if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_15_info_get(unit, dbal_physical_table_id)->dbal_table,
                             &entry_handle_mdb_15_table));
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_15_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_15_table);
        }

        

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_16_info_get(unit, mdb_physical_table_id)->dbal_table,
                         &entry_handle_mdb_16_table));

        dbal_value_field32_request(unit, entry_handle_mdb_16_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                   &(physical_entry_iterator->mdb_em_emc_bank_enable));

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_16_table, DBAL_COMMIT));

        dbal_entry_value_field32_set(unit, entry_handle_mdb_16_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0x0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_16_table, DBAL_COMMIT));

        

        if (mdb_em_age_supported_internal(unit, mdb_physical_table_id) == TRUE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_23_info_get(unit,
                                                             mdb_physical_table_id)->dbal_table,
                             &entry_handle_mdb_23_table));

            dbal_value_field32_request(unit, entry_handle_mdb_23_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                       &(physical_entry_iterator->mdb_em_emp_bank_enable));

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_23_table, DBAL_COMMIT));

            dbal_entry_value_field32_set(unit, entry_handle_mdb_23_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0x0);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_23_table, DBAL_COMMIT));
        }
    }

    

    if (app_id != MDB_APP_ID_ITER_ALL)
    {
        int payload_size = 0;
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id, app_id, &payload_size));

        SHR_IF_ERR_EXIT(mdb_em_get_key_size
                        (unit, dbal_physical_table_id, app_id, &physical_entry_iterator->mdb_em_key_size));
        physical_entry_iterator->mdb_em_payload_size = payload_size;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_get_way_index(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int mdb_cluster_idx,
    uint32 *way_index)
{
    mdb_macro_types_e macro_type;
    uint8 macro_idx;
    uint8 cluster_idx;
    int nof_clusters;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    

    if (mdb_cluster_idx < nof_clusters)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        macro_type.get(unit, mdb_physical_table_id, mdb_cluster_idx, &macro_type));

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        macro_index.get(unit, mdb_physical_table_id, mdb_cluster_idx, &macro_idx));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        cluster_index.get(unit, mdb_physical_table_id, mdb_cluster_idx, &cluster_idx));

        SHR_IF_ERR_EXIT(mdb_dh_em_way_index_get
                        (unit, mdb_physical_table_id, macro_type, macro_idx, cluster_idx, way_index));
    }
    else
    {
        macro_type = MDB_NOF_MACRO_TYPES;
        macro_idx = 0;
        cluster_idx = 0;

        SHR_IF_ERR_EXIT(mdb_dh_em_way_index_get
                        (unit, mdb_physical_table_id, macro_type, macro_idx, cluster_idx, way_index));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_iterator_get_next_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    int nof_clusters;
    int row_width;
    uint32 max_prefix_num;
    uint32 invalid_ratio_val;
    soc_mem_t mdb_physical_memory = INVALIDm;
    int blk;
    int entry_was_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(*entry));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
    row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->row_width;
    mdb_physical_memory = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->em_interface;
    SHR_IF_ERR_EXIT(mdb_db_infos.em_interface_blk.get(unit, dbal_physical_table_id, &blk));

    

    max_prefix_num = row_width / MDB_DIRECT_BASIC_ENTRY_SIZE;
    invalid_ratio_val = (1 << (max_prefix_num * MDB_EM_ENTRY_TYPE_SIZE)) - 1;

    

    while ((entry_was_found == FALSE) && (*is_end == FALSE))
    {

        

        while ((physical_entry_iterator->mdb_em_ratios == invalid_ratio_val) && (*is_end == FALSE))
        {
            int max_cluster_idx;

            

            if (physical_entry_iterator->mdb_cluster_index < nof_clusters)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.end_address.get(unit, mdb_physical_table_id,
                                                                              physical_entry_iterator->mdb_cluster_index,
                                                                              &max_cluster_idx));
            }
            else
            {
                max_cluster_idx = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->ovf_cam_size;
            }

            

            if (physical_entry_iterator->mdb_entry_index == SAL_UINT32_MAX)
            {
                if (physical_entry_iterator->mdb_cluster_index < nof_clusters)
                {
                    int start_address;

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.start_address.get(unit, mdb_physical_table_id,
                                                                                    physical_entry_iterator->mdb_cluster_index,
                                                                                    &start_address));
                    physical_entry_iterator->mdb_entry_index = start_address;
                }
                else
                {

                    

                    physical_entry_iterator->mdb_entry_index = 0;
                }
                SHR_IF_ERR_EXIT(mdb_em_get_way_index
                                (unit, mdb_physical_table_id, physical_entry_iterator->mdb_cluster_index,
                                 &physical_entry_iterator->mdb_em_way_index));
            }
            else
            {
                physical_entry_iterator->mdb_entry_index++;
            }

            

            while ((physical_entry_iterator->mdb_entry_index < max_cluster_idx)
                   && (physical_entry_iterator->mdb_em_ratios == invalid_ratio_val))
            {
                soc_reg_above_64_val_t read_ratios_data, read_ratios_results, read_ratios_entry;
                uint32 command = MDB_EM_CMD_READ_RATIOS_BY_ADDR;

                

                sal_memset(read_ratios_data, 0x0, sizeof(read_ratios_data));

                soc_mem_field_set(unit, mdb_physical_memory, read_ratios_data, COMMANDf, &command);
                soc_mem_field_set(unit, mdb_physical_memory, read_ratios_data, ADDRESSf,
                                  &physical_entry_iterator->mdb_entry_index);
                soc_mem_field_set(unit, mdb_physical_memory, read_ratios_data, WAY_INDEXf,
                                  &physical_entry_iterator->mdb_em_way_index);

                SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, read_ratios_data));

                

                SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, read_ratios_results));
                soc_mem_field_get(unit, mdb_physical_memory, read_ratios_results, ENTRYf, read_ratios_entry);

                physical_entry_iterator->mdb_em_ratios = read_ratios_entry[0];

                

                if (physical_entry_iterator->mdb_em_ratios == invalid_ratio_val)
                {
                    physical_entry_iterator->mdb_entry_index++;
                }
            }

            

            if (physical_entry_iterator->mdb_entry_index == max_cluster_idx)
            {

                

                if (physical_entry_iterator->mdb_cluster_index < nof_clusters)
                {
                    physical_entry_iterator->mdb_cluster_index += row_width / MDB_NOF_CLUSTER_ROW_BITS;
                    physical_entry_iterator->mdb_entry_index = SAL_UINT32_MAX;
                }
                else
                {

                    

                    *is_end = TRUE;
                }
            }
        }

        

        if (*is_end != TRUE)
        {
            uint32 entry_type = MDB_EM_ENTRY_ENCODING_EMPTY;
            uint32 mdb_em_ratios_mask, mdb_em_ratios_mask_size;
            uint32 prefix_num = max_prefix_num - 1;
            soc_reg_above_64_val_t read_entry_data, read_entry_reply, read_entry_field;
            uint32 command = MDB_EM_CMD_READ_BY_ADDR;
            int payload_size_int;
            uint32 app_id_extracted;
            uint8 app_id_size;
            mdb_em_entry_encoding_e entry_encoding_output;

            

            while (entry_type == MDB_EM_ENTRY_ENCODING_EMPTY)
            {
                SHR_BITCOPY_RANGE(&entry_type, 0, &physical_entry_iterator->mdb_em_ratios,
                                  prefix_num * MDB_EM_ENTRY_TYPE_SIZE, MDB_EM_ENTRY_TYPE_SIZE);

                if ((prefix_num == 0) || (entry_type != MDB_EM_ENTRY_ENCODING_EMPTY))
                {
                    break;
                }
                prefix_num--;
            }

            if (entry_type == MDB_EM_ENTRY_ENCODING_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected mdb_em_ratios: 0x%x.\n",
                             physical_entry_iterator->mdb_em_ratios);
            }

            

            mdb_em_ratios_mask_size = max_prefix_num / (1 << entry_type);
            mdb_em_ratios_mask = (1 << (MDB_EM_ENTRY_TYPE_SIZE * mdb_em_ratios_mask_size)) - 1;
            physical_entry_iterator->mdb_em_ratios |=
                mdb_em_ratios_mask << ((prefix_num + 1 - mdb_em_ratios_mask_size) * MDB_EM_ENTRY_TYPE_SIZE);

            

            sal_memset(read_entry_data, 0x0, sizeof(read_entry_data));

            soc_mem_field_set(unit, mdb_physical_memory, read_entry_data, COMMANDf, &command);
            soc_mem_field_set(unit, mdb_physical_memory, read_entry_data, ADDRESSf,
                              &physical_entry_iterator->mdb_entry_index);
            soc_mem_field_set(unit, mdb_physical_memory, read_entry_data, WAY_INDEXf,
                              &physical_entry_iterator->mdb_em_way_index);
            soc_mem_field_set(unit, mdb_physical_memory, read_entry_data, PREFIX_NUMf, &prefix_num);
            soc_mem_field_set(unit, mdb_physical_memory, read_entry_data, ENTRY_TYPEf, &entry_type);

            SHR_IF_ERR_EXIT(soc_mem_write(unit, mdb_physical_memory, blk, 0, read_entry_data));

            

            SHR_IF_ERR_EXIT(soc_mem_read(unit, mdb_physical_memory, blk, 0, read_entry_reply));

            

            soc_mem_field_get(unit, mdb_physical_memory, read_entry_reply, TIDf, &app_id_extracted);
            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                            app_id_size.get(unit, dbal_physical_table_id, app_id_extracted, &app_id_size));
            app_id_extracted &= (1 << app_id_size) - 1;

            if ((app_id != MDB_APP_ID_ITER_ALL) && (app_id_extracted != app_id))
            {
                continue;
            }
            else
            {
                entry_was_found = TRUE;
            }

            physical_entry_iterator->app_id = app_id_extracted;

            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                            max_payload_size.get(unit, dbal_physical_table_id, app_id_extracted, &payload_size_int));
            entry->payload_size = payload_size_int;
            SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id_extracted, &entry->key_size));

            

            soc_mem_field_get(unit, mdb_physical_memory, read_entry_reply, ENTRYf, read_entry_field);

            sal_memset(entry->key, 0x0, sizeof(entry->key));
            SHR_BITCOPY_RANGE(entry->key, 0, read_entry_field, 0, entry->key_size);

            sal_memset(entry->payload, 0x0, sizeof(entry->payload));
            SHR_IF_ERR_EXIT(mdb_em_entry_extract_payload
                            (unit, dbal_physical_table_id, mdb_physical_memory, read_entry_reply, entry->key_size,
                             entry->payload_size, entry->payload, NULL, &entry_encoding_output));

            if (entry_encoding_output != entry_type)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Entry type from read_ratios (%d) is different from type after extract payload (%d).\n",
                             entry_type, entry_encoding_output);
            }

            

            entry->hitbit = physical_entry_iterator->hit_bit_flags;
            entry->age = physical_entry_iterator->age_flags;
            entry->mdb_action_apply = physical_entry_iterator->mdb_action_apply;

            if ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION) || (entry->age & DBAL_PHYSICAL_KEY_AGE_GET)
                || (entry->mdb_action_apply & DBAL_MDB_ACTION_APPLY_HW_ONLY))
            {
                SHR_IF_ERR_EXIT(mdb_em_entry_get
                                (unit, dbal_physical_table_id, physical_entry_iterator->app_id, entry, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_iterator_get_next_sw(
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

    

    if ((app_id != MDB_APP_ID_ITER_ALL) && (physical_entry_iterator->mdb_em_key_size == 0))
    {
        *is_end = TRUE;
        SHR_EXIT();
    }

    sal_memset(entry, 0x0, sizeof(*entry));
    sal_memset(shadow_payload, 0x0, sizeof(shadow_payload));

    key_size_byte = MDB_EM_TABLE_MAX_SHADOW_KEY_IN_BYTE(unit, dbal_physical_table_id);
    app_id_offset = SAL_UINT8_NOF_BITS - dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_size;

    

    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&(physical_entry_iterator->mdb_em_htb_iter)) && entry_was_found == FALSE)
    {
        uint32 app_id_sw;
        SHR_IF_ERR_EXIT(mdb_em_db.
                        shadow_em_db.get_next(unit, dbal_physical_table_id, &(physical_entry_iterator->mdb_em_htb_iter),
                                              shadow_key, shadow_payload));
         
        app_id_sw = shadow_key[key_size_byte - 1] >> app_id_offset;

        if ((!SW_STATE_HASH_TABLE_ITER_IS_END(&(physical_entry_iterator->mdb_em_htb_iter)))
            && ((app_id == MDB_APP_ID_ITER_ALL) || (app_id_sw == app_id)))
        {

            

            if (app_id == MDB_APP_ID_ITER_ALL)
            {
                int payload_size_int;
                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                max_payload_size.get(unit, dbal_physical_table_id, app_id_sw, &payload_size_int));

                SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id_sw, &entry->key_size));
                entry->payload_size = payload_size_int;
            }
            else
            {
                entry->key_size = physical_entry_iterator->mdb_em_key_size;
                entry->payload_size = physical_entry_iterator->mdb_em_payload_size;
            }
            physical_entry_iterator->app_id = app_id_sw;
            entry->hitbit = physical_entry_iterator->hit_bit_flags;
            entry->age = physical_entry_iterator->age_flags;
            entry->mdb_action_apply = physical_entry_iterator->mdb_action_apply;

            

            shadow_key[key_size_byte - 1] &=
                0xFF >> dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_size;
            SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_key, BITS2BYTES(entry->key_size), entry->key));

            
            if ((entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION) || (entry->age & DBAL_PHYSICAL_KEY_AGE_GET)
                || (entry->mdb_action_apply & DBAL_MDB_ACTION_APPLY_HW_ONLY))
            {
                SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id_sw, entry, 0));
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_em_shadow_extract_payload
                                (unit, dbal_physical_table_id, app_id_sw, shadow_payload, entry));
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
mdb_em_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    SHR_FUNC_INIT_VARS(unit);

    

    if (physical_entry_iterator->mdb_action_apply != DBAL_MDB_ACTION_APPLY_HW_ONLY)
    {
        SHR_IF_ERR_EXIT(mdb_em_iterator_get_next_sw
                        (unit, dbal_physical_table_id, app_id, physical_entry_iterator, entry, is_end));
    }
#ifndef ADAPTER_SERVER_MODE
    else
    {
        SHR_IF_ERR_EXIT(mdb_em_iterator_get_next_hw
                        (unit, dbal_physical_table_id, app_id, physical_entry_iterator, entry, is_end));
    }
#endif

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
    DBAL_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_action_apply != DBAL_MDB_ACTION_APPLY_HW_ONLY)
    {

        

        physical_entry_iterator->mdb_em_htb_iter = UTILEX_U32_MAX;
    }
    else
    {

        

        mdb_physical_table_e mdb_physical_table_id =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
        uint32 entry_handle_mdb_15_table, entry_handle_mdb_16_table, entry_handle_mdb_23_table;

        physical_entry_iterator->mdb_em_ratios = SAL_UINT32_MAX;
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(mdb_em_dma_mode_set(unit, dbal_physical_table_id, TRUE, NULL));

        

        if (!dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_15_info_get(unit, dbal_physical_table_id)->dbal_table,
                             &entry_handle_mdb_15_table));
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_15_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_15_table);
        }

        

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_16_info_get(unit, mdb_physical_table_id)->dbal_table,
                         &entry_handle_mdb_16_table));

        dbal_entry_value_field32_set(unit, entry_handle_mdb_16_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                     physical_entry_iterator->mdb_em_emc_bank_enable);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_16_table, DBAL_COMMIT));

        

        if (mdb_em_age_supported_internal(unit, mdb_physical_table_id) == TRUE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_23_info_get(unit,
                                                             mdb_physical_table_id)->dbal_table,
                             &entry_handle_mdb_23_table));

            dbal_entry_value_field32_set(unit, entry_handle_mdb_23_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         physical_entry_iterator->mdb_em_emp_bank_enable);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_23_table, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_table_clear(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    dbal_physical_entry_t *entries = NULL;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    uint8 is_end = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    if (app_id == MDB_APP_ID_ITER_ALL)
    {
        dbal_physical_entry_t entry;
        int tables_num = 0, table_iter;

        sal_memset(&physical_entry_iterator, 0, sizeof(physical_entry_iterator));
        physical_entry_iterator.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
        SHR_IF_ERR_EXIT(mdb_em_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

        while (is_end == FALSE)
        {

            
            physical_entry_iterator.mdb_em_payload_size = 0;
            SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                            (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));

            
            if (is_end == FALSE)
            {
                entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
                SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, physical_tables, physical_entry_iterator.app_id, &entry, 0));
            }
        }

        SHR_IF_ERR_EXIT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

        
        SHR_IF_ERR_EXIT(mdb_num_valid_tables(unit, physical_tables, &tables_num));
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_em_db.shadow_em_db.delete_all(unit, physical_tables[table_iter]));
        }
    }
    else
    {
        
        uint32 app_id_array[MDB_EM_SHADOW_HASH_NOF_RESOURCES];
        uint32 entries_to_delete_itr, valid_entries_count = 0;

        entries =
            (dbal_physical_entry_t *) sal_alloc(sizeof(dbal_physical_entry_t) * MDB_EM_SHADOW_HASH_NOF_RESOURCES,
                                                "MDB EM entries clear");
        if (entries == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate memory for entry buffer.\n");
        }

        
        while (is_end == FALSE)
        {
            valid_entries_count = 0;

            sal_memset(&physical_entry_iterator, 0, sizeof(physical_entry_iterator));
            SHR_IF_ERR_EXIT(mdb_em_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

            
            physical_entry_iterator.mdb_em_payload_size = 0;
            while ((is_end == FALSE) & (valid_entries_count < MDB_EM_SHADOW_HASH_NOF_RESOURCES))
            {

                
                sal_memset(&entries[valid_entries_count], 0x0, sizeof(dbal_physical_entry_t));
                SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                                (unit, dbal_physical_table_id, app_id, &physical_entry_iterator,
                                 &entries[valid_entries_count], &is_end));
                app_id_array[valid_entries_count] = physical_entry_iterator.app_id;

                
                if (is_end == FALSE)
                {
                    valid_entries_count++;
                }
            }

            SHR_IF_ERR_EXIT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

            
            for (entries_to_delete_itr = 0; entries_to_delete_itr < valid_entries_count; entries_to_delete_itr++)
            {
                SHR_IF_ERR_EXIT(mdb_em_entry_delete
                                (unit, physical_tables, app_id_array[entries_to_delete_itr],
                                 &entries[entries_to_delete_itr], 0));
            }
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
mdb_em_calc_vmv_regs(
    int unit,
    mdb_em_encoding_request_e encoding_request,
    mdb_em_encoding_request_e encoding_nof_bits_request,
    dbal_physical_tables_e dbal_table_id,
    uint8 esem_cmd_indication,
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

    encoding_nof_bits = MDB_INIT_GET_EM_EMCODING_NOF_BITS(unit, encoding_nof_bits_request);

    

    if (esem_cmd_indication && (vmv_encoding_data == NULL))
    {
        is_esem_shift = TRUE;
    }

    for (entry_encoding = 0; entry_encoding < MDB_EM_NOF_ENCODING_TYPES; entry_encoding++)
    {
        encoding_value = mdb_init_encoding_values_map[encoding_request][entry_encoding];
        encoding_index = mdb_init_encoding_values_map[MDB_EM_ENCODING_TABLE_ENTRY][entry_encoding];

        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_table_id, encoding_index, &vmv_size));

        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.value.get(unit, dbal_table_id, encoding_index, &vmv_value_init));

        

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

        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.encoding.get(unit, dbal_table_id, vmv_value, &entry_encoding));
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.size.get(unit, dbal_table_id, vmv_value, &vmv_size));

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

