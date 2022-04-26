
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/mdb.h>
#include <sal/appl/io.h>
#include "mdb_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

 

#define MDB_LPM_BUFF_MAX_CLEAR 10

#ifndef KAPS_WARMBOOT_TYPE_DEFINED
#define KAPS_WARMBOOT_TYPE_DEFINED


typedef struct kaps_warmboot_s
{
    FILE *kaps_file_fp;
    kaps_device_issu_read_fn kaps_file_read;
    kaps_device_issu_write_fn kaps_file_write;
} kaps_warmboot_t;
#endif 



typedef struct
{
    struct kaps_allocator *dalloc_p;
    void *kaps_xpt_p;
    struct kaps_device *kaps_device_p;

} generic_kaps_app_data_t;

typedef struct kaps_key *mdb_kaps_key_t_p;

static kaps_warmboot_t Kaps_warmboot_data[SOC_MAX_NUM_DEVICES];

static FILE *Kaps_file_fp[SOC_MAX_NUM_DEVICES];

static generic_kaps_app_data_t *Lpm_app_data[SOC_MAX_NUM_DEVICES] = { NULL };


const int mdb_lpm_ad_size_enum_to_bits[MDB_NOF_LPM_AD_SIZES] = {
    20, 
};



#define DNX_KAPS_INDIRECT_COMMIT_MODE_CHECK_AND_UPDATE(_db_entry_, _mode_) \
do { \
     \
    if ((_db_entry_) == NULL) \
    { \
         \
        if ((_mode_) == DBAL_INDIRECT_COMMIT_MODE_UPDATE) \
        { \
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The update flag was used on an entry that doesn't exists.\n"); \
        } \
        (_mode_) = DBAL_INDIRECT_COMMIT_MODE_NORMAL; \
    } \
    else \
    { \
         \
        if ((_mode_) == DBAL_INDIRECT_COMMIT_MODE_NORMAL) \
        { \
            SHR_ERR_EXIT(_SHR_E_EXISTS, "This entry already exists, to modify it the update flag should be used.\n"); \
        } \
        (_mode_) = DBAL_INDIRECT_COMMIT_MODE_UPDATE; \
    } \
} while (0)


void
mdb_lpm_kaps_buffer_dbal_to_kaps(
    const uint32 nof_bits,
    const uint32 nof_valid_bits,
    const uint32 *data_in,
    uint8 *data_out)
{
    const uint32 nof_bytes = BITS2BYTES(nof_bits);
    const uint32 nof_valid_bytes = BITS2BYTES(nof_valid_bits);
    int byte_index = nof_valid_bytes;
    int byte_counter = nof_bytes - nof_valid_bytes;
    int uint32_index = byte_counter >> 2;
    const uint32 align_bits = BYTES2BITS(nof_bytes) - nof_bits;

    while (byte_index--)
    {
        
        data_out[byte_index] = (data_in[uint32_index] >> (((nof_bytes - byte_index - 1) & 0x3) << 3));
        if ((++byte_counter & 0x3) == 0)
        {
            uint32_index++;
        }
    }

    
    if (align_bits != 0)
    {
        uint8 data_remainder = 0x00;
        byte_index = nof_valid_bytes;
        while (byte_index--)
        {
            uint8 tmp_data_out = (data_out[byte_index] << align_bits) | data_remainder;
            data_remainder = data_out[byte_index] >> (8 - align_bits);
            data_out[byte_index] = tmp_data_out;
        }
    }
}


void
mdb_lpm_kaps_buffer_kaps_to_dbal(
    uint32 nof_bits,
    uint8 *data_in,
    uint32 *data_out)
{
    uint32 nof_bytes = BITS2BYTES(nof_bits);
    uint32 nof_uint32 = BITS2WORDS(nof_bits);
    int32 byte_index;
    int32 uint32_index;
    int byte_counter = 0;
    uint8 align_bits = BYTES2BITS(nof_bytes) - nof_bits;
    uint32 tmp_data_out;

    byte_index = nof_bytes;
    uint32_index = 0;
    data_out[uint32_index] = 0x00000000;

    while (byte_index--)
    {
        int data_offset = (((nof_bytes - byte_index - 1) % 4) * 8);
        byte_counter++;
        tmp_data_out = data_in[byte_index] << (data_offset);
        data_out[uint32_index] |= tmp_data_out;

        if ((byte_counter) % 4 == 0)
        {
            uint32_index++;
            data_out[uint32_index] = 0x0;
        }
    }

    
    if (align_bits != 0)
    {
        uint32 data_remainder = 0x00000000;
        uint32_index = nof_uint32;
        while (uint32_index--)
        {
            tmp_data_out = (data_out[uint32_index] >> align_bits) | data_remainder;
            data_remainder = (data_out[uint32_index] << (32 - align_bits));
            data_out[uint32_index] = tmp_data_out;
        }
    }
}


shr_error_e
mdb_lpm_cache_big_bb_get(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(mdb_lpm_xpt_cache_big_bb_get
                    (unit, Lpm_app_data[unit]->kaps_device_p, macro_type, macro_index, cluster_index, row_index,
                     dst_offset_bits, dst_array));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_lpm_cache_tcam_get(
    int unit,
    int db_id,
    int instance_id,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(mdb_lpm_xpt_cache_tcam_get
                    (unit, Lpm_app_data[unit]->kaps_device_p, db_id, instance_id, row_index,
                     dst_offset_bits, dst_array));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_kaps_entry_lpm_get(
    int unit,
    mdb_kaps_db_t_p db_p,
    mdb_kaps_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 prefix_length,
    uint32 *payload_size,
    uint16 *kaps_hitbit)
{
    kaps_status res;
    struct kaps_entry *db_entry = NULL;
    struct kaps_ad *ad_entry = NULL;
    struct kaps_hb *hb_entry = NULL;
    struct kaps_entry_info entry_info;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    uint32 payload_size_in_bytes;

    SHR_FUNC_INIT_VARS(unit);

    mdb_lpm_kaps_buffer_dbal_to_kaps(key_size_in_bits, key_size_in_bits, key, data);

    
    kaps_db_get_prefix_handle(db_p, data, prefix_length, &db_entry);
    if (db_entry == NULL)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    res = kaps_entry_get_info(db_p, db_entry, &entry_info);
    if (res == KAPS_PENDING_ENTRY)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if (entry_info.active == 0)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    *payload_size = mdb_lpm_ad_size_enum_to_bits[MDB_LPM_AD_SIZE_20];
    payload_size_in_bytes = BITS2BYTES(*payload_size);

    DNX_KAPS_TRY(kaps_entry_get_ad(db_p, db_entry, &ad_entry));
    if (ad_entry == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_entry_get_ad() Failed\n");
    }

    
    if (*kaps_hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
    {
        
        DNX_KAPS_TRY(kaps_entry_get_hb(db_p, db_entry, &hb_entry));
    }

    sal_memset(asso_data, 0x0, sizeof(asso_data[0]) * payload_size_in_bytes);

    
    DNX_KAPS_TRY(kaps_ad_db_get(entry_info.ad_db, ad_entry, asso_data));

    mdb_lpm_kaps_buffer_kaps_to_dbal(*payload_size, asso_data, payload);

    if ((hb_entry != NULL) && (*kaps_hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION))
    {
        uint8 clear;
        uint32 hitbit_val = 0;
        if (*kaps_hitbit & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
        {
            clear = 1;
        }
        else
        {
            clear = 0;
        }
        DNX_KAPS_TRY(kaps_hb_entry_get_bit_value(kaps_hb_db_p, hb_entry, &hitbit_val, clear));

        
        if (hitbit_val != 0)
        {
            *kaps_hitbit |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED;
        }
        else
        {
            *kaps_hitbit &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_kaps_entry_lpm_add(
    int unit,
    mdb_kaps_db_t_p db_p,
    mdb_kaps_ad_db_t_p ad_db_p,
    mdb_kaps_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    dbal_indirect_commit_mode_e * indirect_commit_mode,
    uint8 kaps_hitbit)
{
    kaps_status res;
    struct kaps_entry *db_entry = NULL;
    struct kaps_ad *ad_entry = NULL;
    struct kaps_hb *hb_entry = NULL;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    uint8 add_entry = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    mdb_lpm_kaps_buffer_dbal_to_kaps(dnx_data_mdb.kaps.key_width_in_bits_get(unit), key_size_in_bits, key, data);
    mdb_lpm_kaps_buffer_dbal_to_kaps(payload_size_in_bits, payload_size_in_bits, payload, asso_data);

    
    kaps_db_get_prefix_handle(db_p, data, prefix_length, &db_entry);

    
    {
        DNX_KAPS_INDIRECT_COMMIT_MODE_CHECK_AND_UPDATE(db_entry, *indirect_commit_mode);
    }

    if ((*indirect_commit_mode) != DBAL_INDIRECT_COMMIT_MODE_NORMAL)
    {
        
        DNX_KAPS_TRY(kaps_entry_get_ad(db_p, db_entry, &ad_entry));
        DNX_KAPS_TRY(kaps_ad_db_update_entry(ad_db_p, ad_entry, asso_data));
        add_entry = FALSE;
    }

    if (add_entry)
    {
        
        DNX_KAPS_TRY(kaps_db_add_prefix(db_p, data, prefix_length, &db_entry));
        res = kaps_ad_db_add_entry(ad_db_p, asso_data, &ad_entry);

        if (DNX_KAPS_IS_ERROR(res))
        {
            
            kaps_db_delete_entry(db_p, db_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_ad_db_add_entry Failed\n");
        }

        res = kaps_entry_add_ad(db_p, db_entry, ad_entry);
        if (DNX_KAPS_IS_ERROR(res))
        {
            
            kaps_db_delete_entry(db_p, db_entry);
            kaps_ad_db_delete_entry(ad_db_p, ad_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_entry_add_ad Failed\n");
        }

        if (kaps_hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
        {
            res = kaps_hb_db_add_entry(kaps_hb_db_p, &hb_entry);
            if (DNX_KAPS_IS_ERROR(res))
            {
                
                kaps_db_delete_entry(db_p, db_entry);
                kaps_ad_db_delete_entry(ad_db_p, ad_entry);
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_hb_db_add_entry Failed\n");
            }
            res = kaps_entry_add_hb(db_p, db_entry, hb_entry);
            if (DNX_KAPS_IS_ERROR(res))
            {
                
                kaps_db_delete_entry(db_p, db_entry);
                kaps_ad_db_delete_entry(ad_db_p, ad_entry);
                kaps_hb_db_delete_entry(kaps_hb_db_p, hb_entry);
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_entry_add_hb Failed\n");
            }
        }

        res = kaps_db_install(db_p);
        if (DNX_KAPS_IS_ERROR(res))
        {
            
            kaps_db_delete_entry(db_p, db_entry);
            kaps_ad_db_delete_entry(ad_db_p, ad_entry);

            if (kaps_hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
            {
                kaps_hb_db_delete_entry(kaps_hb_db_p, hb_entry);
            }

            if (kaps_dnx_error_translation(res) == _SHR_E_FULL)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, " Error: Table is full, %s\n", kaps_get_status_string(res));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_db_install failed with: %s.\n",
                             kaps_get_status_string(res));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_kaps_entry_lpm_delete(
    int unit,
    mdb_kaps_db_t_p db_p,
    mdb_kaps_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 prefix_length)
{
    struct kaps_entry *db_entry = NULL;
    mdb_kaps_ad_db_t_p ad_db_p;
    struct kaps_ad *ad_entry = NULL;
    struct kaps_hb *hb_entry = NULL;
    struct kaps_entry_info entry_info;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };
    kaps_status kaps_rv;

    SHR_FUNC_INIT_VARS(unit);

    
    mdb_lpm_kaps_buffer_dbal_to_kaps(dnx_data_mdb.kaps.key_width_in_bits_get(unit), key_size_in_bits, key, data);
    kaps_rv = kaps_db_get_prefix_handle(db_p, data, prefix_length, &db_entry);

    if ((kaps_rv != KAPS_OK) || (db_entry == NULL))
    {
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found.");
        }
    }
    
    DNX_KAPS_TRY(kaps_entry_get_info(db_p, db_entry, &entry_info));
    ad_db_p = entry_info.ad_db;

    
    DNX_KAPS_TRY(kaps_entry_get_ad(db_p, db_entry, &ad_entry));
    if (ad_entry == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kaps_entry_get_ad() Failed\n");
    }

    
    if (kaps_hb_db_p != NULL)
    {
        DNX_KAPS_TRY(kaps_entry_get_hb(db_p, db_entry, &hb_entry));
    }

    DNX_KAPS_TRY(kaps_db_delete_entry(db_p, db_entry));
    if (hb_entry != NULL)
    {
        DNX_KAPS_TRY(kaps_hb_db_delete_entry(kaps_hb_db_p, hb_entry));
    }

    DNX_KAPS_TRY(kaps_ad_db_delete_entry(ad_db_p, ad_entry));

exit:
    SHR_FUNC_EXIT;
}



int
mdb_lpm_kaps_file_read_func(
    void *handle,
    uint8 *buffer,
    uint32 size,
    uint32 offset)
{
    size_t result;

    if (!handle)
    {
        return SOC_E_FAIL;
    }

    if (0 != fseek(handle, offset, SEEK_SET))
    {
        return SOC_E_FAIL;
    }

    result = fread(buffer, 1, size, handle);
    if (result < size)
    {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}



int
mdb_lpm_kaps_file_write_func(
    void *handle,
    uint8 *buffer,
    uint32 size,
    uint32 offset)
{
    size_t result;

    if (!handle)
    {
        return SOC_E_UNIT;
    }

    if (0 != fseek(handle, offset, SEEK_SET))
    {
        return SOC_E_FAIL;
    }

    result = sal_fwrite(buffer, 1, size, handle);
    if (result != size)
    {
        return SOC_E_MEMORY;
    }
    sal_fflush(handle);

    return SOC_E_NONE;
}

static shr_error_e
mdb_lpm_kaps_file_open(
    int unit,
    char *filename,
    int device_type)
{
    int is_warmboot;
    char prefixed_file_name[SOC_PROPERTY_NAME_MAX + 256];
    char *stable_filename = NULL;

    FILE **file_fp = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == filename)
    {
        return _SHR_E_NONE;
    }

    if (device_type == KAPS_DEVICE_KAPS)
    {
        file_fp = &Kaps_file_fp[unit];
    }
    else
    {
        return _SHR_E_NONE;
    }

    if (*file_fp == NULL)
    {
        is_warmboot = SOC_WARM_BOOT(unit);

        sal_memset(prefixed_file_name, 0, sizeof(prefixed_file_name));

        stable_filename = dnx_data_dev_init.ha.stable_filename_get(unit)->val;

        

        
        if (NULL != stable_filename)
        {
            sal_strncat_s(prefixed_file_name, stable_filename, sizeof(prefixed_file_name));

            sal_strncat_s(prefixed_file_name, "_", sizeof(prefixed_file_name));
        }
        sal_strncat_s(prefixed_file_name, filename, sizeof(prefixed_file_name));

        if ((*file_fp = sal_fopen(prefixed_file_name, is_warmboot != 0 ? "r+" : "w+")) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error:  sal_fopen() Failed\n");
        }
    }

    Kaps_warmboot_data[unit].kaps_file_fp = Kaps_file_fp[unit];
    Kaps_warmboot_data[unit].kaps_file_read = &mdb_lpm_kaps_file_read_func;
    Kaps_warmboot_data[unit].kaps_file_write = &mdb_lpm_kaps_file_write_func;

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_lpm_init_device(
    int unit)
{
    uint32 flags;
    kaps_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);

    Lpm_app_data[unit]->kaps_xpt_p = NULL;
#ifdef USE_MODEL

    

    DNX_KAPS_TRY(kaps_sw_model_init(Lpm_app_data[unit]->dalloc_p, KAPS_DEVICE_KAPS, KAPS_DEVICE_DEFAULT,
                                    NULL, &(Lpm_app_data[unit]->kaps_xpt_p)));
#else
#ifndef BLACKHOLE_MODE
    SHR_IF_ERR_EXIT(mdb_lpm_xpt_init(unit, &(Lpm_app_data[unit]->kaps_xpt_p)));
#endif
#endif

    warmboot_data = &Kaps_warmboot_data[unit];

    flags = KAPS_DEVICE_DEFAULT | KAPS_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit))
    {
        flags |= KAPS_DEVICE_SKIP_INIT;
    }

    DNX_KAPS_TRY(kaps_device_init(Lpm_app_data[unit]->dalloc_p,
                                  KAPS_DEVICE_KAPS,
                                  flags,
                                  (struct kaps_xpt *) Lpm_app_data[unit]->kaps_xpt_p,
                                  NULL, &Lpm_app_data[unit]->kaps_device_p));

    if (SOC_WARM_BOOT(unit))
    {
        DNX_KAPS_TRY(kaps_device_restore_state
                     (Lpm_app_data[unit]->kaps_device_p, warmboot_data->kaps_file_read, warmboot_data->kaps_file_write,
                      warmboot_data->kaps_file_fp));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_kaps_db.init(unit));

        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.alloc(unit));
    }

exit:
    SHR_FUNC_EXIT;
}



int
mdb_lpm_db_kaps_sync(
    int unit)
{
    kaps_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);

    warmboot_data = &Kaps_warmboot_data[unit];

    KAPS_TRY(kaps_device_save_state_and_continue
             (Lpm_app_data[unit]->kaps_device_p, warmboot_data->kaps_file_read, warmboot_data->kaps_file_write,
              warmboot_data->kaps_file_fp));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_lpm_db_init(
    int unit,
    uint32 db_id)
{
    mdb_kaps_db_handles_t db_handles_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        uint32 db_size;
        int db_size_int;
        sal_memset(&db_handles_info, 0, sizeof(db_handles_info));

        

        {
            if (db_id < dnx_data_mdb.kaps.nof_dbs_get(unit))
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.
                                capacity.get(unit, dnx_data_mdb.kaps.db_info_get(unit, db_id)->dbal_id, &db_size_int));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Error: unrecognized db_id: %d\n", db_id);
            }
        }
        db_size = db_size_int;

        if (db_size != 0)
        {
            db_handles_info.is_valid = 1;
        }
        else
        {
            db_handles_info.is_valid = 0;
        }

        if (db_handles_info.is_valid == 1)
        {
            int result_type_iter;
            int nof_result_types;


            
            DNX_KAPS_TRY(kaps_db_init
                         (Lpm_app_data[unit]->kaps_device_p, KAPS_DB_LPM, db_id, db_size, &db_handles_info.db_p));

            
            nof_result_types = dnx_data_mdb.kaps.db_info_get(unit, db_id)->nof_result_types;

            for (result_type_iter = 0; result_type_iter < nof_result_types; result_type_iter++)
            {
                mdb_lpm_ad_size_e ad_size;

                {
                    ad_size = MDB_LPM_AD_SIZE_20;
                }

                if (dnx_data_mdb.kaps.db_supported_ad_sizes_get(unit, db_id, ad_size)->is_supported)
                {

                    
                    DNX_KAPS_TRY(kaps_ad_db_init(Lpm_app_data[unit]->kaps_device_p,
                                                 DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES * db_id + result_type_iter,
                                                 db_size, mdb_lpm_ad_size_enum_to_bits[ad_size],
                                                 &db_handles_info.ad_db_p_arr[result_type_iter]));

                    DNX_KAPS_TRY(kaps_db_set_ad(db_handles_info.db_p, db_handles_info.ad_db_p_arr[result_type_iter]));

                    db_handles_info.ad_db_size_arr[result_type_iter] = ad_size;
                }
            }

            
            if (dnx_data_mdb.kaps.feature_get(unit, dnx_data_mdb_kaps_hitbit_support))
            {
                DNX_KAPS_TRY(kaps_hb_db_init
                             (Lpm_app_data[unit]->kaps_device_p, db_id, db_size, &db_handles_info.hb_db_p));

                DNX_KAPS_TRY(kaps_db_set_hb(db_handles_info.db_p, db_handles_info.hb_db_p));
            }
        }

        

        

        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.set(unit, db_id, &db_handles_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_id, &db_handles_info));

        if (db_handles_info.is_valid == 1)
        {
            int result_type_iter;
            int nof_result_types;

            DNX_KAPS_TRY(kaps_db_refresh_handle
                         (Lpm_app_data[unit]->kaps_device_p, db_handles_info.db_p, &db_handles_info.db_p));

            
            nof_result_types = dnx_data_mdb.kaps.db_info_get(unit, db_id)->nof_result_types;;
            for (result_type_iter = 0; result_type_iter < nof_result_types; result_type_iter++)
            {
                if (db_handles_info.ad_db_p_arr[result_type_iter] != NULL)
                {
                    DNX_KAPS_TRY(kaps_ad_db_refresh_handle
                                 (Lpm_app_data[unit]->kaps_device_p, db_handles_info.ad_db_p_arr[result_type_iter],
                                  &db_handles_info.ad_db_p_arr[result_type_iter]));
                }
            }

            if (dnx_data_mdb.kaps.feature_get(unit, dnx_data_mdb_kaps_hitbit_support))
            {
                DNX_KAPS_TRY(kaps_hb_db_refresh_handle
                             (Lpm_app_data[unit]->kaps_device_p, db_handles_info.hb_db_p, &db_handles_info.hb_db_p));
            }
        }

        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.set(unit, db_id, &db_handles_info));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    }

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_kaps_search_init(
    int unit)
{
    uint32 db_idx;

    mdb_kaps_key_t_p master_key;
    mdb_kaps_key_t_p key;

    mdb_kaps_db_handles_t db_handles_info;

    mdb_kaps_instruction_handles_t instruction_handles_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        int nof_dbs = dnx_data_mdb.kaps.nof_dbs_get(unit);

        sal_memset(&instruction_handles_info, 0, sizeof(instruction_handles_info));

        

        DNX_KAPS_TRY(kaps_instruction_init(Lpm_app_data[unit]->kaps_device_p, 0  ,
                                           0  ,
                                           &(instruction_handles_info.inst_p)));

        
        for (db_idx = 0; db_idx < nof_dbs; db_idx++)
        {
            SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

            if (db_handles_info.is_valid == 1)
            {
                DNX_KAPS_TRY(kaps_key_init(Lpm_app_data[unit]->kaps_device_p, &key));

                

                DNX_KAPS_TRY(kaps_key_add_field(key,
                                                dnx_data_mdb.kaps.db_info_get(unit, db_idx)->name,
                                                dnx_data_mdb.kaps.key_width_in_bits_get(unit), KAPS_KEY_FIELD_PREFIX));

                DNX_KAPS_TRY(kaps_db_set_key(db_handles_info.db_p, key));
            }
        }

        

        DNX_KAPS_TRY(kaps_key_init(Lpm_app_data[unit]->kaps_device_p, &master_key));

        
        for (db_idx = 0; db_idx < nof_dbs; db_idx++)
        {
            SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

            

            DNX_KAPS_TRY(kaps_key_add_field(master_key,
                                            dnx_data_mdb.kaps.db_info_get(unit, db_idx)->name,
                                            dnx_data_mdb.kaps.key_width_in_bits_get(unit), KAPS_KEY_FIELD_PREFIX));
        }

        DNX_KAPS_TRY(kaps_instruction_set_key(instruction_handles_info.inst_p, master_key));

        for (db_idx = 0; db_idx < nof_dbs; db_idx++)
        {
            SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

            if (db_handles_info.is_valid == 1)
            {
                DNX_KAPS_TRY(kaps_instruction_add_db(instruction_handles_info.inst_p, db_handles_info.db_p, db_idx));
            }
        }

        DNX_KAPS_TRY(kaps_instruction_install(instruction_handles_info.inst_p));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_kaps_db.search_instruction_info.get(unit, &instruction_handles_info));

        DNX_KAPS_TRY(kaps_instruction_refresh_handle
                     (Lpm_app_data[unit]->kaps_device_p, instruction_handles_info.inst_p,
                      &instruction_handles_info.inst_p));
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_EXIT(mdb_kaps_db.search_instruction_info.set(unit, &instruction_handles_info));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_kaps_init_db_set(
    int unit)
{
    uint32 db_idx;
    int nof_dbs;

    SHR_FUNC_INIT_VARS(unit);

    nof_dbs = dnx_data_mdb.kaps.nof_dbs_get(unit);

    
    nof_dbs = MDB_LPM_DBS_IN_PAIR;

    
    for (db_idx = 0; db_idx < nof_dbs; db_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_lpm_db_init(unit, db_idx));
    }

    

    SHR_IF_ERR_EXIT(mdb_kaps_search_init(unit));

    DNX_KAPS_TRY(kaps_device_lock(Lpm_app_data[unit]->kaps_device_p));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_lpm_init_app(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (Lpm_app_data[unit] == NULL)
    {
        SHR_ALLOC_SET_ZERO(Lpm_app_data[unit], sizeof(generic_kaps_app_data_t), "lpm_app_data[unit]", "%s%s%s\r\n",
                           EMPTY, EMPTY, EMPTY);
        if (Lpm_app_data[unit] == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, " Error:  SHR_ALLOC for lpm_app_data Failed\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, " Error: kaps is already initialized.\n");
    }

    

    DNX_KAPS_TRY(kaps_default_allocator_create(&Lpm_app_data[unit]->dalloc_p));

    

    SHR_IF_ERR_EXIT(mdb_lpm_init_device(unit));

    

    SHR_IF_ERR_EXIT(mdb_kaps_init_db_set(unit));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(mdb_lpm_kaps_file_open(unit, "kaps", KAPS_DEVICE_KAPS));

    SHR_IF_ERR_EXIT(mdb_lpm_init_app(unit));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
mdb_lpm_deinit_device(
    int unit,
    int no_sync_flag)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!no_sync_flag)
    {
        DNX_KAPS_TRY(kaps_device_destroy(Lpm_app_data[unit]->kaps_device_p));
    }
    else
    {
        DNX_KAPS_TRY(kaps_default_allocator_destroy(Lpm_app_data[unit]->dalloc_p));
        DNX_KAPS_TRY(kaps_default_allocator_create(&Lpm_app_data[unit]->dalloc_p));
    }

#ifdef USE_MODEL

    

    if (!no_sync_flag)
    {
        DNX_KAPS_TRY(kaps_sw_model_destroy(Lpm_app_data[unit]->kaps_xpt_p));
    }
#else
#ifndef BLACKHOLE_MODE
    mdb_lpm_xpt_deinit(unit, Lpm_app_data[unit]->kaps_xpt_p);
#endif
#endif

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_deinit(
    int unit,
    int no_sync_flag)
{
    SHR_FUNC_INIT_VARS(unit);

    if (NULL == Lpm_app_data[unit])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in mdb_lpm_deinit_app(): kaps not initialized.\n");
    }

    SHR_IF_ERR_EXIT(mdb_lpm_deinit_device(unit, no_sync_flag));

    

    DNX_KAPS_TRY(kaps_default_allocator_destroy(Lpm_app_data[unit]->dalloc_p));

    SHR_FREE(Lpm_app_data[unit]);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_get_ip_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *ipv4_capacity,
    int *ipv6_capacity)
{
    int total_nof_bb_rows = 0;

    

    int ipv4_capacity_per_row_mul = 370;
    int ipv4_capacity_per_row_div = 32;
    int ipv6_capacity_per_row_mul = 185;
    int ipv6_capacity_per_row_div = 32;
    int ipv4_capacity_per_row_small_kaps = 8;
    int ipv6_capacity_per_row_small_kaps = 4;
    int capacity;
    int big_kaps_enabled;
    uint8 db_idx;
    uint32 kaps_hier;

    SHR_FUNC_INIT_VARS(unit);

    
#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        *ipv4_capacity = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->capacity;
        *ipv6_capacity = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->capacity;
        SHR_EXIT();
    }
#endif

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_lpm_xpt_big_kaps_enabled(unit, db_idx, &big_kaps_enabled));

    {
        
        mdb_kaps_db_handles_t db_handles_info;

        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));
        if (db_handles_info.is_valid)
        {
            DNX_KAPS_TRY(kaps_db_get_property(db_handles_info.db_p, KAPS_PROP_NUM_LEVELS_IN_DB, &kaps_hier));
            if (!
                (((kaps_hier == 3) && (big_kaps_enabled == TRUE)) || ((kaps_hier == 2) && (big_kaps_enabled == FALSE))))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "%s: KAPS SDK hierarchy (%d) not aligned to SDK big KAPS enabled (%d)\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id), kaps_hier, big_kaps_enabled);
            }
        }
    }

    if (big_kaps_enabled == FALSE)
    {

        

        int nof_small_bbs;

        SHR_IF_ERR_EXIT(mdb_lpm_xpt_get_nof_small_bbs(unit, dbal_physical_table_id, &nof_small_bbs));

        total_nof_bb_rows = nof_small_bbs * dnx_data_mdb.kaps.db_info_get(unit, db_idx)->nof_rows_in_small_bb;

        *ipv4_capacity = total_nof_bb_rows * ipv4_capacity_per_row_small_kaps;
        *ipv6_capacity = total_nof_bb_rows * ipv6_capacity_per_row_small_kaps;
    }
    else
    {

        

        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &capacity));
        total_nof_bb_rows = capacity / dnx_data_mdb.kaps.max_prefix_in_bb_row_get(unit);

        

        *ipv4_capacity = total_nof_bb_rows * ipv4_capacity_per_row_mul / ipv4_capacity_per_row_div;
        *ipv6_capacity = total_nof_bb_rows * ipv6_capacity_per_row_mul / ipv6_capacity_per_row_div;
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_get_current_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *num_entries,
    int *num_entries_iter,
    int *capacity_estimate)
{
    mdb_kaps_db_handles_t db_handles_info;
    uint8 db_idx;
    struct kaps_db_stats stats;
    int num_entries_iter_tmp = -1;
    struct kaps_entry_iter *mdb_lpm_iter;
    struct kaps_entry *kpb_e;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

    
    if (db_handles_info.is_valid != 1)
    {
        *num_entries = 0;
        *num_entries_iter = 0;
        *capacity_estimate = 0;
    }
    else
    {
        DNX_KAPS_TRY(kaps_db_stats(db_handles_info.db_p, &stats));

        DNX_KAPS_TRY(kaps_db_entry_iter_init(db_handles_info.db_p, &mdb_lpm_iter));

        do
        {
            num_entries_iter_tmp++;
            DNX_KAPS_TRY(kaps_db_entry_iter_next(db_handles_info.db_p, mdb_lpm_iter, &kpb_e));
        }
        while (kpb_e != NULL);

        DNX_KAPS_TRY(kaps_db_entry_iter_destroy(db_handles_info.db_p, mdb_lpm_iter));

        *num_entries = stats.num_entries;
        *num_entries_iter = num_entries_iter_tmp;
        *capacity_estimate = stats.capacity_estimate;
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_dbal_to_db(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint8 *db_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type ==
            DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
        {
            *db_idx = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Error: unrecognized LPM physical_table.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    mdb_kaps_db_handles_t db_handles_info;
    mdb_kaps_ad_db_t_p ad_db_p = NULL;
    mdb_lpm_ad_size_e ad_size = 0;
    uint8 db_idx;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);


    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    {
        ad_size = db_handles_info.ad_db_size_arr[0];
        ad_db_p = db_handles_info.ad_db_p_arr[0];
    }

    {
        SHR_IF_ERR_EXIT(mdb_lpm_kaps_entry_lpm_add(unit,
                                                   db_handles_info.db_p,
                                                   ad_db_p,
                                                   db_handles_info.hb_db_p,
                                                   entry->key,
                                                   entry->key_size,
                                                   entry->payload,
                                                   mdb_lpm_ad_size_enum_to_bits[ad_size], entry->prefix_length,
                                                   &(entry->indirect_commit_mode), entry->hitbit));
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        {
            SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_write(unit, dbal_physical_table_id, app_id, entry));
        }
    }
#endif

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        dbal_tables_e logical_table;
        int print_index, data_offset;

        SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, dbal_physical_table_id, app_id, &logical_table));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-PEMLA-START********************\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "PhyDB(%s), AppDbId(%d), DB(%s), Prefix(%d), key(0x"),
                                     dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                     app_id, dbal_logical_table_to_string(unit, logical_table), entry->prefix_length));
        for (data_offset = 0; data_offset < BITS2WORDS(entry->key_size); data_offset++)
        {
            print_index = BITS2WORDS(entry->key_size) - 1 - data_offset;
            if (data_offset == 0)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%X"), entry->key[print_index]));
            }
            else
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry->key[print_index]));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "), payload(0x")));

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

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "), payload_size(%d)\n"), entry->payload_size));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-PEMLA-END********************\n")));

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "prefix_length: %d. dbal_physical_table: %s, app_id: %d, payload_size: %d.\n"),
                     entry->prefix_length, dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id,
                     entry->payload_size));
        if (ad_size < MDB_NOF_LPM_AD_SIZES)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "ad_size: %d.\n"), mdb_lpm_ad_size_enum_to_bits[ad_size]));
        }
        for (data_offset = 0; data_offset < MDB_KAPS_KEY_WIDTH_IN_UINT32(unit); data_offset++)
        {
            print_index = MDB_KAPS_KEY_WIDTH_IN_UINT32(unit) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size); data_offset++)
        {
            print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->payload[%d]: %08X.\n"), print_index, entry->payload[print_index]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_add: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    int res = _SHR_E_NONE;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        {
            res = adapter_mdb_access_lpm_read(unit, dbal_physical_table_id, app_id, entry);
        }
    }
    else
#endif
    {
        uint8 db_idx;
        mdb_kaps_db_handles_t db_handles_info;

        SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

        SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

        if (db_handles_info.is_valid != 1)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }

        res = mdb_lpm_kaps_entry_lpm_get(unit,
                                         db_handles_info.db_p,
                                         db_handles_info.hb_db_p,
                                         entry->key,
                                         dnx_data_mdb.kaps.key_width_in_bits_get(unit),
                                         entry->payload, entry->prefix_length, &entry->payload_size, &entry->hitbit);
    }

    if (res == _SHR_E_NOT_FOUND)
    {
        

        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_get: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "prefix_length: %d. dbal_physical_table: %s, app_id: %d, payload_size: %d.\n"),
                     entry->prefix_length, dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id,
                     entry->payload_size));
        for (data_offset = 0; data_offset < MDB_KAPS_KEY_WIDTH_IN_UINT32(unit); data_offset++)
        {
            print_index = MDB_KAPS_KEY_WIDTH_IN_UINT32(unit) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(entry->payload_size); data_offset++)
        {
            print_index = BITS2WORDS(entry->payload_size) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->payload[%d]: %08X.\n"), print_index, entry->payload[print_index]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_get: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    mdb_kaps_db_handles_t db_handles_info;
    uint8 db_idx;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);


    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    {
        SHR_IF_ERR_EXIT(mdb_lpm_kaps_entry_lpm_delete(unit,
                                                      db_handles_info.db_p,
                                                      db_handles_info.hb_db_p,
                                                      entry->key, dnx_data_mdb.kaps.key_width_in_bits_get(unit),
                                                      entry->prefix_length));
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        {
            SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_delete(unit, dbal_physical_table_id, app_id, entry));
        }
    }
#endif

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 data_offset;
        uint32 print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_delete: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "prefix_length: %d. dbal_physical_table: %s, app_id: %d.\n"),
                                     entry->prefix_length, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                     app_id));
        for (data_offset = 0; data_offset < MDB_KAPS_KEY_WIDTH_IN_UINT32(unit); data_offset++)
        {
            print_index = MDB_KAPS_KEY_WIDTH_IN_UINT32(unit) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_delete: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    mdb_kaps_db_handles_t db_handles_info;
    uint8 db_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));
    if (db_handles_info.is_valid != 1)
    {
        physical_entry_iterator->mdb_lpm_db_p = NULL;
        physical_entry_iterator->mdb_lpm_iter = NULL;
    }
    else
    {
        physical_entry_iterator->mdb_lpm_db_p = db_handles_info.db_p;

        DNX_KAPS_TRY(kaps_db_entry_iter_init
                     (physical_entry_iterator->mdb_lpm_db_p, &physical_entry_iterator->mdb_lpm_iter));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    struct kaps_entry *kpb_e = NULL;
    struct kaps_entry_info kpb_e_info;

    SHR_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_lpm_db_p != NULL)
    {
        DNX_KAPS_TRY(kaps_db_entry_iter_next
                     (physical_entry_iterator->mdb_lpm_db_p, physical_entry_iterator->mdb_lpm_iter, &kpb_e));
    }

    if (kpb_e == NULL)
    {
        *is_end = TRUE;
    }
    else
    {
        DNX_KAPS_TRY(kaps_entry_get_info(physical_entry_iterator->mdb_lpm_db_p, kpb_e, &kpb_e_info));

        

        while ((app_id != MDB_APP_ID_ITER_ALL)
               && ((kpb_e_info.data[0] >> (SAL_UINT8_NOF_BITS - dnx_data_mdb.kaps.key_prefix_length_get(unit))) !=
                   app_id))
        {
            DNX_KAPS_TRY(kaps_db_entry_iter_next
                         (physical_entry_iterator->mdb_lpm_db_p, physical_entry_iterator->mdb_lpm_iter, &kpb_e));

            if (kpb_e == NULL)
            {
                *is_end = TRUE;
                break;
            }
            else
            {
                DNX_KAPS_TRY(kaps_entry_get_info(physical_entry_iterator->mdb_lpm_db_p, kpb_e, &kpb_e_info));
            }
        }

        if (kpb_e != NULL)
        {
            if (kpb_e_info.ad_handle != NULL)
            {
                uint8 ad_8[BITS2BYTES(DNX_DATA_MAX_MDB_PDBS_MAX_PAYLOAD_SIZE)];

                DNX_KAPS_TRY(kaps_ad_db_get(kpb_e_info.ad_db, kpb_e_info.ad_handle, ad_8));

                sal_memset(entry, 0x0, sizeof(*entry));

                

                entry->key_size = dnx_data_mdb.kaps.key_width_in_bits_get(unit);
                mdb_lpm_kaps_buffer_kaps_to_dbal(dnx_data_mdb.kaps.key_width_in_bits_get(unit), kpb_e_info.data,
                                                 entry->key);
                entry->prefix_length = kpb_e_info.prio_len;

                
                
                entry->payload_size = mdb_lpm_ad_size_enum_to_bits[MDB_LPM_AD_SIZE_20];

                 
                mdb_lpm_kaps_buffer_kaps_to_dbal(entry->payload_size, ad_8, entry->payload);

                

                if (physical_entry_iterator->hit_bit_flags & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
                {
                    entry->hitbit = physical_entry_iterator->hit_bit_flags;
                    SHR_IF_ERR_EXIT(mdb_lpm_entry_get(unit, dbal_physical_table_id, app_id, entry, 0));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KAPS entry AD handle is null.\n");
                *is_end = TRUE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_lpm_db_p != NULL)
    {
        DNX_KAPS_TRY(kaps_db_entry_iter_destroy
                     (physical_entry_iterator->mdb_lpm_db_p, physical_entry_iterator->mdb_lpm_iter));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_table_clear(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    uint8 is_end = FALSE;
    int entries_count, current_entry;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    dbal_physical_entry_t entries[MDB_LPM_BUFF_MAX_CLEAR];
    mdb_kaps_db_handles_t db_handles_info;
    uint8 db_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {

        

        is_end = TRUE;
    }

    while (is_end == FALSE)
    {
        sal_memset(&physical_entry_iterator, 0, sizeof(physical_entry_iterator));
        SHR_IF_ERR_EXIT(mdb_lpm_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

        entries_count = 0;
        while (is_end == FALSE && entries_count < MDB_LPM_BUFF_MAX_CLEAR)
        {
            SHR_IF_ERR_EXIT(mdb_lpm_iterator_get_next
                            (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entries[entries_count],
                             &is_end));

            

            if (is_end == FALSE)
            {
                entries_count++;
            }
        }

        SHR_IF_ERR_EXIT(mdb_lpm_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

        

        for (current_entry = 0; current_entry < entries_count; current_entry++)
        {
            SHR_IF_ERR_EXIT(mdb_lpm_entry_delete(unit, physical_tables, app_id, &entries[current_entry], 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_lpm_table_verify(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    uint8 is_end = FALSE;
    uint8 verify_failed = FALSE;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    dbal_physical_entry_t entry;
    mdb_kaps_db_handles_t db_handles_info;
    uint8 db_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(mdb_kaps_db.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {

        

        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));

    sal_memset(&physical_entry_iterator, 0, sizeof(physical_entry_iterator));
    SHR_IF_ERR_EXIT(mdb_lpm_iterator_init(unit, dbal_physical_table_id, MDB_APP_ID_ITER_ALL, &physical_entry_iterator));

    while (is_end == FALSE)
    {
        SHR_IF_ERR_EXIT(mdb_lpm_iterator_get_next
                        (unit, dbal_physical_table_id, MDB_APP_ID_ITER_ALL, &physical_entry_iterator, &entry, &is_end));

        

        if (is_end == FALSE)
        {
            dbal_physical_entry_t entry_duplicate_search;
            int uint32_counter;

            

            sal_memcpy(&entry_duplicate_search, &entry, sizeof(entry_duplicate_search));
            sal_memset(entry_duplicate_search.payload, 0x0, sizeof(entry_duplicate_search.payload));

            SHR_IF_ERR_EXIT(mdb_lpm_xpt_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate_search.payload, entry.payload,
                             (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range(entry_duplicate_search.payload, 0  ,
                                                   entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate_search.payload, entry.payload,
                                 (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nEntry with mismatched HW and SW payload:\n")));
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry key:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.key_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.key[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "key prefix length %d\n"), entry.prefix_length));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "SW shadow payload:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "HW search payload:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "%08X"), entry_duplicate_search.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                verify_failed = TRUE;
            }
        }
    }

    SHR_IF_ERR_EXIT(mdb_lpm_iterator_deinit
                    (unit, dbal_physical_table_id, MDB_APP_ID_ITER_ALL, &physical_entry_iterator));

    if (verify_failed == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Verify failed, HW search payload is not equal to SW shadow payload.\n");
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_dump_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    uint8 small_kaps_db_idx;
    mdb_physical_table_e big_kaps_ads, big_kaps_bb;
    int table_id_shift = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "%s is not associated with an LPM MDB table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    small_kaps_db_idx = table_id_shift;
    big_kaps_ads = MDB_PHYSICAL_TABLE_ADS_1 + table_id_shift;
    big_kaps_bb = MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 + table_id_shift;

    
    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));

    SHR_IF_ERR_EXIT(mdb_lpm_xpt_dump_hw(unit, small_kaps_db_idx, big_kaps_ads, big_kaps_bb));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_test_fill_entry(
    int unit,
    int index,
    int fill_payload,
    uint8 is_update,
    dbal_physical_entry_t * entry)
{
    uint32 dip;
    uint32 vrf = 5;
    int prefix_length = 52;
    uint32 payload;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(*entry));

    dip = index;
    entry->key[4] = 0 << 26   | (vrf & 0x3FFF) << 12 | dip >> 20;
    entry->key[3] = dip << 12;
    entry->key_size = 54;
    entry->prefix_length = prefix_length;
    entry->payload_size = mdb_lpm_ad_size_enum_to_bits[MDB_LPM_AD_SIZE_20];
    entry->indirect_commit_mode = is_update ? DBAL_INDIRECT_COMMIT_MODE_UPDATE : DBAL_INDIRECT_COMMIT_MODE_NORMAL;

    if (is_update == TRUE)
    {
        payload = (index + 1) * 2;
    }
    else
    {
        payload = index + 1;
    }

    if (fill_payload == TRUE)
    {
        entry->payload[0] = payload & ((1 << entry->payload_size) - 1);
    }

    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry, entry_duplicate;
#ifndef ADAPTER_SERVER_MODE
    dbal_physical_entry_t entry_duplicate_search;
#endif
    int max_ext_iterations;
    int uint32_counter;
    shr_error_e res;
    shr_error_e rv;

    

    int ext_iterator = 0, int_iterator = 0;
    int max_capacity;
    uint32 add_total_time_usecs = 0, get_total_time_usecs = 0, del_total_time_usecs = 0, update_total_time_usecs = 0;
    uint32 total_entry_count = 0;

    SHR_FUNC_INIT_VARS(unit);


    sal_memset(&entry, 0x0, sizeof(entry));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &max_capacity));

    if (max_capacity == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "%s does not have HW resources available to it.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;

    if (mode == MDB_TEST_CAPACITY)
    {

        

        max_ext_iterations = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &max_ext_iterations));
    }

    

    ext_iterator = 0;
    while (ext_iterator < max_ext_iterations)
    {
        int entry_index;
        int max_int_iterator = 0;
        uint32 timer_usec = 0;

        

        int_iterator = 0;
        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        do
        {
            entry_index = ext_iterator + int_iterator;
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, FALSE, &entry));

            if (full_time == FALSE)
            {
                timer_usec = sal_time_usecs();
            }
            rv = mdb_lpm_entry_add(unit, physical_tables, 0, &entry, 0);
            if (full_time == FALSE)
            {
                add_total_time_usecs += sal_time_usecs() - timer_usec;
            }

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
                       "  Successfully added %d entries with prefix length of 52bits, absolute maximum possible is %d\n"),
                      max_int_iterator, max_capacity));
        }

        

        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
        if (full_time == TRUE)
        {
            add_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        

        int_iterator = 0;
        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        do
        {
            entry_index = ext_iterator + int_iterator;

            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, FALSE, &entry));
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, FALSE, &entry_duplicate));

            if (full_time == FALSE)
            {
                timer_usec = sal_time_usecs();
            }
            SHR_IF_ERR_EXIT(mdb_lpm_entry_get(unit, dbal_physical_table_id, 0, &entry_duplicate, 0));
            if (full_time == FALSE)
            {
                get_total_time_usecs += sal_time_usecs() - timer_usec;
            }

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate.payload, entry.payload,
                             (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range
                (entry_duplicate.payload, 0  , entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate.payload, entry.payload,
                                 (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry_duplicate.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, read data is not equal to written data.\n");
            }

#ifndef ADAPTER_SERVER_MODE

            

            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, FALSE, &entry));
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, FALSE, &entry_duplicate_search));

            SHR_IF_ERR_EXIT(mdb_lpm_xpt_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate_search.payload, entry.payload,
                             (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range(entry_duplicate_search.payload, 0  ,
                                                   entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate_search.payload, entry.payload,
                                 (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "%08X"), entry_duplicate_search.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Test failed, read search debug interface data is not equal to written data.\n");
            }
#endif
            int_iterator++;
        }
        while (int_iterator < max_int_iterator);

        if (full_time == TRUE)
        {
            get_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        

        int_iterator = 0;
        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        do
        {
            entry_index = ext_iterator + int_iterator;
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, TRUE, &entry));

            if (full_time == FALSE)
            {
                timer_usec = sal_time_usecs();
            }
            rv = mdb_lpm_entry_add(unit, physical_tables, 0, &entry, 0);
            if (full_time == FALSE)
            {
                update_total_time_usecs += sal_time_usecs() - timer_usec;
            }

            if ((rv != _SHR_E_FULL) && (rv != _SHR_E_NONE))
            {
                SHR_ERR_EXIT(rv, "Failed to update entry.\n");
            }
            int_iterator++;
        }
        while (int_iterator < max_int_iterator);

        

        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
        if (full_time == TRUE)
        {
            update_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        

        int_iterator = 0;
        do
        {
            entry_index = ext_iterator + int_iterator;

            entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, TRUE, &entry));
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, TRUE, &entry_duplicate));

            SHR_IF_ERR_EXIT(mdb_lpm_entry_get(unit, dbal_physical_table_id, 0, &entry_duplicate, 0));

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate.payload, entry.payload,
                             (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range
                (entry_duplicate.payload, 0  , entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate.payload, entry.payload,
                                 (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry_duplicate.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                SHR_ERR_EXIT(_SHR_E_FAIL, "Test update failed, read data is not equal to written data.\n");
            }

#ifndef ADAPTER_SERVER_MODE

            

            entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, TRUE, TRUE, &entry));
            SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, TRUE, &entry_duplicate_search));

            SHR_IF_ERR_EXIT(mdb_lpm_xpt_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate_search.payload, entry.payload,
                             (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range(entry_duplicate_search.payload, 0  ,
                                                   entry.payload_size - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate_search.payload, entry.payload,
                                 (entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((entry.payload_size + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "%08X"), entry_duplicate_search.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Test update failed, read search debug interface data is not equal to written data.\n");
            }
#endif
            int_iterator++;
        }
        while (int_iterator < max_int_iterator);

        

        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        if (delete_cmd == TRUE)
        {
            int_iterator = 0;
            do
            {
                entry_index = ext_iterator + int_iterator;
                SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, FALSE, &entry_duplicate));
                if (full_time == FALSE)
                {
                    timer_usec = sal_time_usecs();
                }
                SHR_IF_ERR_EXIT(mdb_lpm_entry_delete(unit, physical_tables, 0, &entry_duplicate, 0));
                if (full_time == FALSE)
                {
                    del_total_time_usecs += sal_time_usecs() - timer_usec;
                }

                int_iterator++;
            }
            while (int_iterator < max_int_iterator);
        }
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
        if (full_time == TRUE)
        {
            del_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        

        if (delete_cmd == TRUE)
        {
            int_iterator = 0;
            do
            {
                entry_index = ext_iterator + int_iterator;
                SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, FALSE, &entry_duplicate));
                res = mdb_lpm_entry_get(unit, dbal_physical_table_id, 0, &entry_duplicate, 0);
                if (res != _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, read data after delete should return NOT_FOUND.\n");
                }

#ifndef ADAPTER_SERVER_MODE

                

                SHR_IF_ERR_EXIT(mdb_lpm_test_fill_entry(unit, entry_index, FALSE, FALSE, &entry_duplicate_search));
                SHR_IF_ERR_EXIT(mdb_lpm_xpt_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

                if (entry_duplicate_search.payload[0] != 0x0)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Test failed, read search debug interface data after delete should be all 0, instead it is 0x%x.\n",
                                 entry_duplicate_search.payload[0]);
                }
#endif
                int_iterator++;
            }
            while (int_iterator < max_int_iterator);
        }

        if (mode == MDB_TEST_CAPACITY)
        {
            ext_iterator++;
        }
        else
        {
            ext_iterator += 1 + (sal_rand() % (max_ext_iterations / MDB_TEST_BRIEF_ENTRIES));
        }
    }

    if (total_entry_count != 0)
    {
        
        int entry_print_factor = 10;
        int total_entry_count_factored = total_entry_count / entry_print_factor;

        total_entry_count_factored = total_entry_count_factored ? total_entry_count_factored : 1;

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "In total %d entries were added, full time measurement = %d,\nentry batch = %d\navg usecs per entry batch add: %d\navg usecs per entry batch update + HW search (if full): %d\navg usecs per entry batch get: %d\navg usecs per entry batch delete: %d\n"),
                  total_entry_count, full_time, entry_print_factor, add_total_time_usecs / total_entry_count_factored,
                  update_total_time_usecs / total_entry_count_factored,
                  get_total_time_usecs / total_entry_count_factored,
                  del_total_time_usecs / total_entry_count_factored));
    }
    else
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "In total 0 entries were added\n")));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_lpm_ser_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    int core;
    uint8 db_idx;
    uint32 nof_tcam_rows;
    int tcam_row;
    int ecc_nof_bits = 16;
    sal_usecs_t scan_usecs;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];

    

    int sleep_factor = 4;

    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(physical_tables, 0x0, sizeof(physical_tables));
    physical_tables[0] = dbal_physical_table_id;
    SHR_IF_ERR_EXIT(mdb_lpm_table_clear(unit, physical_tables, MDB_APP_ID_ITER_ALL));

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));
    nof_tcam_rows = dnx_data_mdb.kaps.db_info_get(unit, db_idx)->nof_rows_in_rpb;

    

    scan_usecs =
        (dnx_data_intr.ser.kaps_tcam_scan_get(unit)->scan_window * nof_tcam_rows /
         dnx_data_device.general.core_clock_khz_get(unit)) * 1000;

    

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        uint32 ecc_data_orig, ecc_data_mod, ecc_data_read;

        for (tcam_row = 0; tcam_row < nof_tcam_rows; tcam_row++)
        {
            

            SHR_IF_ERR_EXIT(soc_mem_array_read
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_orig));

            ecc_data_mod = ecc_data_orig ^ (1 << (sal_rand() % ecc_nof_bits));
            SHR_IF_ERR_EXIT(soc_mem_array_write
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_mod));

            sal_usleep(sleep_factor * scan_usecs);
            SHR_IF_ERR_EXIT(soc_mem_array_read
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_read));

            if (ecc_data_orig != ecc_data_read)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "TCAM 1bit scan test failed, db_idx = %d, core = %d, tcam_row = %d, ecc_data_orig = 0x%x, ecc_data_mod = 0x%x, ecc_data_read = 0x%x.\n",
                             db_idx, core, tcam_row, ecc_data_orig, ecc_data_mod, ecc_data_read);
            }
        }

        {
            
            uint32 first_bit, second_bit;

            tcam_row = sal_rand() % nof_tcam_rows;
            SHR_IF_ERR_EXIT(soc_mem_array_read
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_orig));

            first_bit = 1 << (sal_rand() % ecc_nof_bits);
            do
            {
                second_bit = 1 << (sal_rand() % ecc_nof_bits);
            }
            while (second_bit == first_bit);

            ecc_data_mod = ecc_data_orig ^ (first_bit | second_bit);
            SHR_IF_ERR_EXIT(soc_mem_array_write
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_mod));

            sal_usleep(100 * sleep_factor * scan_usecs);
            SHR_IF_ERR_EXIT(soc_mem_array_read
                            (unit, KAPS_TCAM_ECC_MEMORYm, db_idx, KAPS_BLOCK(unit, core), tcam_row, &ecc_data_read));

            if (ecc_data_read != 0x0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "TCAM 2bit scan test failed, db_idx = %d, core = %d, tcam_row = %d, ecc_data_orig = 0x%x, ecc_data_mod = 0x%x, ecc_data_read = 0x%x.\n",
                             db_idx, core, tcam_row, ecc_data_orig, ecc_data_mod, ecc_data_read);
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "KAPS TCAM 1bit SER test completed successfully!\n")));

exit:
    SHR_FUNC_EXIT;
}
