/**
 * \file mdb_lpm.c Contains all of the MDB KAPS (LPM) initialization and API functions.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/shrextend/shrextend_debug.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)

#include <soc/dnx/mdb.h>
#include <sal/appl/io.h>
#include "mdb_internal.h"
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

#define MDB_KAPS_ACCESS mdb_kaps_db

#define MDB_LPM_BUFF_MAX_CLEAR 1000

typedef struct
{
    struct kbp_allocator *dalloc_p;
    void *kaps_xpt_p;
    struct kbp_device *kaps_device_p;

} generic_kaps_app_data_t;

static kbp_warmboot_t Kaps_warmboot_data[SOC_MAX_NUM_DEVICES];

static FILE *Kaps_file_fp[SOC_MAX_NUM_DEVICES];

static generic_kaps_app_data_t *Lpm_app_data[SOC_MAX_NUM_DEVICES] = { NULL };

static shr_error_e
mdb_lpm_kbp_file_open(
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
        return 0;
    }

    if (device_type == KBP_DEVICE_KAPS)
    {
        file_fp = &Kaps_file_fp[unit];
    }

    if (*file_fp == NULL)
    {
        is_warmboot = SOC_WARM_BOOT(unit);

        sal_memset(prefixed_file_name, 0, sizeof(prefixed_file_name));

        stable_filename = dnx_data_dev_init.ha.stable_filename_get(unit)->val;

        /*
         * assuming stable_filename is unique for each separate run
         */
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

    Kaps_warmboot_data[unit].kbp_file_fp = Kaps_file_fp[unit];
    Kaps_warmboot_data[unit].kbp_file_read = &dnx_kbp_file_read_func;
    Kaps_warmboot_data[unit].kbp_file_write = &dnx_kbp_file_write_func;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_lpm_init_device(
    int unit)
{
    uint32 flags;
    kbp_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);

    Lpm_app_data[unit]->kaps_xpt_p = NULL;
#ifdef USE_MODEL
    DNX_KBP_TRY(kbp_sw_model_init(Lpm_app_data[unit]->dalloc_p, KBP_DEVICE_KAPS, KBP_DEVICE_DEFAULT,
                                  NULL, &(Lpm_app_data[unit]->kaps_xpt_p)));
#else
#ifndef BLACKHOLE_MODE
    SHR_IF_ERR_EXIT(mdb_lpm_xpt_init(unit, &(Lpm_app_data[unit]->kaps_xpt_p)));
#endif
#endif

    warmboot_data = &Kaps_warmboot_data[unit];

    flags = KBP_DEVICE_DEFAULT | KBP_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit))
    {
        flags |= KBP_DEVICE_SKIP_INIT;
    }

    DNX_KBP_TRY(kbp_device_init(Lpm_app_data[unit]->dalloc_p,
                                KBP_DEVICE_KAPS,
                                flags,
                                (struct kaps_xpt *) Lpm_app_data[unit]->kaps_xpt_p,
                                NULL, &Lpm_app_data[unit]->kaps_device_p));

    if (SOC_WARM_BOOT(unit))
    {
        DNX_KBP_TRY(kbp_device_restore_state
                    (Lpm_app_data[unit]->kaps_device_p, warmboot_data->kbp_file_read, warmboot_data->kbp_file_write,
                     warmboot_data->kbp_file_fp));
    }
    else
    {
        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
mdb_lpm_db_kaps_sync(
    int unit)
{
    kbp_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);

    warmboot_data = &Kaps_warmboot_data[unit];

    KBP_TRY(kbp_device_save_state_and_continue
            (Lpm_app_data[unit]->kaps_device_p, warmboot_data->kbp_file_read, warmboot_data->kbp_file_write,
             warmboot_data->kbp_file_fp));

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
    int big_kaps_enabled[MDB_KAPS_IP_NOF_DB];
    int big_kaps_enabled_global = FALSE;
    int db_id_iter;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        uint32 db_size;
        int db_size_int;
        memset(&db_handles_info, 0, sizeof(db_handles_info));

        for (db_id_iter = 0; db_id_iter < MDB_KAPS_IP_NOF_DB; db_id_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_lpm_big_kaps_enabled(unit, db_id_iter, &big_kaps_enabled[db_id_iter]));

            if (big_kaps_enabled[db_id_iter] == TRUE)
            {
                big_kaps_enabled_global = TRUE;
            }
        }

        {
            if (db_id == MDB_KAPS_IP_PRIVATE_DB_ID)
            {   /* Private DB */
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_1, &db_size_int));
            }
            else if (db_id == MDB_KAPS_IP_PUBLIC_DB_ID)
            {   /* Public DB */
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &db_size_int));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " Error: unrecognized db_id: %d\n", db_id);
            }
        }
        db_size = db_size_int;

        if ((db_size != 0) && ((big_kaps_enabled_global == FALSE) || (big_kaps_enabled[db_id] == TRUE)))
        {
            db_handles_info.is_valid = 1;
        }
        else
        {
            db_handles_info.is_valid = 0;
        }

        if (db_handles_info.is_valid == 1)
        {       /* Create the DB */
            DNX_KBP_TRY(kbp_db_init
                        (Lpm_app_data[unit]->kaps_device_p, KBP_DB_LPM, db_id, db_size, &db_handles_info.db_p));

            DNX_KBP_TRY(kbp_ad_db_init(Lpm_app_data[unit]->kaps_device_p,
                                       db_id, db_size, dnx_data_mdb.kaps.ad_width_in_bits_get(unit),
                                       &db_handles_info.ad_db_p));

            DNX_KBP_TRY(kbp_db_set_ad(db_handles_info.db_p, db_handles_info.ad_db_p));

            DNX_KBP_TRY(kbp_hb_db_init(Lpm_app_data[unit]->kaps_device_p, db_id, db_size, &db_handles_info.hb_db_p));

            DNX_KBP_TRY(kbp_db_set_hb(db_handles_info.db_p, db_handles_info.hb_db_p));
        }


        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.set(unit, db_id, &db_handles_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_id, &db_handles_info));

        if (db_handles_info.is_valid == 1)
        {
            DNX_KBP_TRY(kbp_db_refresh_handle
                        (Lpm_app_data[unit]->kaps_device_p, db_handles_info.db_p, &db_handles_info.db_p));

            DNX_KBP_TRY(kbp_ad_db_refresh_handle
                        (Lpm_app_data[unit]->kaps_device_p, db_handles_info.ad_db_p, &db_handles_info.ad_db_p));

            DNX_KBP_TRY(kbp_hb_db_refresh_handle
                        (Lpm_app_data[unit]->kaps_device_p, db_handles_info.hb_db_p, &db_handles_info.hb_db_p));
        }

        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.set(unit, db_id, &db_handles_info));
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

    char *master_key_field_names[MDB_KAPS_IP_NOF_DB] = { "PRIVATE", "PUBLIC" };

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        memset(&instruction_handles_info, 0, sizeof(instruction_handles_info));

        DNX_KBP_TRY(kbp_instruction_init(Lpm_app_data[unit]->kaps_device_p, 0 /* search_id */ ,
                                         0 /* ltr */ ,
                                         &(instruction_handles_info.inst_p)));

        for (db_idx = 0; db_idx < MDB_KAPS_IP_NOF_DB; db_idx++)
        {
            SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

            if (db_handles_info.is_valid == 1)
            {
                DNX_KBP_TRY(kbp_key_init(Lpm_app_data[unit]->kaps_device_p, &key));
                DNX_KBP_TRY(kbp_key_add_field(key,
                                              master_key_field_names[db_idx],
                                              dnx_data_mdb.kaps.key_width_in_bits_get(unit), KBP_KEY_FIELD_PREFIX));

                DNX_KBP_TRY(kbp_db_set_key(db_handles_info.db_p, key));
            }
        }

        DNX_KBP_TRY(kbp_key_init(Lpm_app_data[unit]->kaps_device_p, &master_key));

        for (db_idx = 0; db_idx < MDB_KAPS_IP_NOF_DB; db_idx++)
        {
            SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

            DNX_KBP_TRY(kbp_key_add_field(master_key,
                                          master_key_field_names[db_idx],
                                          dnx_data_mdb.kaps.key_width_in_bits_get(unit), KBP_KEY_FIELD_PREFIX));
        }

        DNX_KBP_TRY(kbp_instruction_set_key(instruction_handles_info.inst_p, master_key));

        for (db_idx = 0; db_idx < MDB_KAPS_IP_NOF_DB; db_idx++)
        {
            SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

            if (db_handles_info.is_valid == 1)
            {
                DNX_KBP_TRY(kbp_instruction_add_db(instruction_handles_info.inst_p, db_handles_info.db_p, db_idx));
            }
        }

        DNX_KBP_TRY(kbp_instruction_install(instruction_handles_info.inst_p));
    }
    else
    {
        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.
                        search_instruction_info.get(unit, MDB_LPM_INSTRUCTIONS_ID, &instruction_handles_info));

        DNX_KBP_TRY(kbp_instruction_refresh_handle
                    (Lpm_app_data[unit]->kaps_device_p, instruction_handles_info.inst_p,
                     &instruction_handles_info.inst_p));
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.
                    search_instruction_info.set(unit, MDB_LPM_INSTRUCTIONS_ID, &instruction_handles_info));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_kaps_init_db_set(
    int unit)
{
    uint32 db_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (db_idx = 0; db_idx < MDB_KAPS_IP_NOF_DB; db_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_lpm_db_init(unit, db_idx));
    }

    SHR_IF_ERR_EXIT(mdb_kaps_search_init(unit));

    DNX_KBP_TRY(kbp_device_lock(Lpm_app_data[unit]->kaps_device_p));

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

    DNX_KBP_TRY(default_allocator_create(&Lpm_app_data[unit]->dalloc_p));

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

    SHR_IF_ERR_EXIT(mdb_lpm_kbp_file_open(unit, "kaps", KBP_DEVICE_KAPS));

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
        DNX_KBP_TRY(kbp_device_destroy(Lpm_app_data[unit]->kaps_device_p));
    }
    else
    {
        DNX_KBP_TRY(default_allocator_destroy(Lpm_app_data[unit]->dalloc_p));
        DNX_KBP_TRY(default_allocator_create(&Lpm_app_data[unit]->dalloc_p));
    }

#ifdef USE_MODEL
    if (!no_sync_flag)
    {
        DNX_KBP_TRY(kbp_sw_model_destroy(Lpm_app_data[unit]->kaps_xpt_p));
    }
#else
#ifndef BLACKHOLE_MODE
    mdb_lpm_xpt_deinit(unit, Lpm_app_data[unit]->kaps_xpt_p);
#endif
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_deinit_app(
    int unit,
    int no_sync_flag)
{
    SHR_FUNC_INIT_VARS(unit);

    if (NULL == Lpm_app_data[unit])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in mdb_lpm_deinit_app(): kaps not initialized.\n");
    }

    SHR_IF_ERR_EXIT(mdb_lpm_deinit_device(unit, no_sync_flag));

    DNX_KBP_TRY(default_allocator_destroy(Lpm_app_data[unit]->dalloc_p));

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

    SHR_FUNC_INIT_VARS(unit);

    if ((dbal_physical_table_id != DBAL_PHYSICAL_TABLE_KAPS_1)
        && (dbal_physical_table_id != DBAL_PHYSICAL_TABLE_KAPS_2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized LPM physical_table (%s).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &capacity));
    total_nof_bb_rows = capacity / dnx_data_mdb.kaps.max_prefix_in_bb_row_get(unit);

    if (capacity <=
        dnx_data_mdb.kaps.max_prefix_in_bb_row_get(unit) * dnx_data_mdb.kaps.nof_rows_in_small_bb_get(unit) *
        dnx_data_mdb.kaps.nof_small_bbs_get(unit))
    {
        *ipv4_capacity = total_nof_bb_rows * ipv4_capacity_per_row_small_kaps;
        *ipv6_capacity = total_nof_bb_rows * ipv6_capacity_per_row_small_kaps;
    }
    else
    {
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
    mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;
    struct kbp_db_stats stats;
    int num_entries_iter_tmp = -1;
    struct kbp_entry_iter *mdb_lpm_iter;
    struct kbp_entry *kpb_e;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

    /** If the physical table has not been allocated, return indication that capacity is 0. */
    if (db_handles_info.is_valid != 1)
    {
        *num_entries = 0;
        *num_entries_iter = 0;
        *capacity_estimate = 0;
    }
    else
    {
        DNX_KBP_TRY(kbp_db_stats(db_handles_info.db_p, &stats));

        DNX_KBP_TRY(kbp_db_entry_iter_init(db_handles_info.db_p, &mdb_lpm_iter));

        do
        {
            num_entries_iter_tmp++;
            DNX_KBP_TRY(kbp_db_entry_iter_next(db_handles_info.db_p, mdb_lpm_iter, &kpb_e));
        }
        while (kpb_e != NULL);

        DNX_KBP_TRY(kbp_db_entry_iter_destroy(db_handles_info.db_p, mdb_lpm_iter));

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
    dbal_physical_tables_e physical_table,
    mdb_kaps_ip_db_id_e * db_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if (physical_table == DBAL_PHYSICAL_TABLE_KAPS_1)
        {
            *db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;
        }
        else if (physical_table == DBAL_PHYSICAL_TABLE_KAPS_2)
        {
            *db_idx = MDB_KAPS_IP_PUBLIC_DB_ID;
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
mdb_lpm_uint32_to_uint8(
    int unit,
    int nof_bytes,
    uint32 *uint32_data,
    uint8 *uint8_data)
{
    int uint32_index;

    SHR_FUNC_INIT_VARS(unit);
    {
        if ((nof_bytes == MDB_KAPS_KEY_WIDTH_IN_BYTES(unit)) || (nof_bytes == MDB_KAPS_KEY_WIDTH_IN_BYTES(unit)))
        {
            int uint32_max = BYTES2WORDS(nof_bytes);
            for (uint32_index = 0; uint32_index < uint32_max; uint32_index++)
            {
                uint8_data[(uint32_index * 4) + 0] = (uint32_data[uint32_max - 1 - uint32_index] >> 24) & 0xFF;
                uint8_data[(uint32_index * 4) + 1] = (uint32_data[uint32_max - 1 - uint32_index] >> 16) & 0xFF;
                uint8_data[(uint32_index * 4) + 2] = (uint32_data[uint32_max - 1 - uint32_index] >> 8) & 0xFF;
                uint8_data[(uint32_index * 4) + 3] = (uint32_data[uint32_max - 1 - uint32_index]) & 0xFF;
            }
        }
        else if (nof_bytes == MDB_KAPS_AD_WIDTH_IN_BYTES(unit))
        {
            uint8_data[0] = (uint32_data[0] >> 12) & 0xFF;
            uint8_data[1] = (uint32_data[0] >> 4) & 0xFF;
            uint8_data[2] = (uint32_data[0] << 4) & 0xFF;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Error:  unexpected nof_bytes to mdb_lpm_uint32_to_uint8.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_uint8_to_uint32(
    int unit,
    int nof_bytes,
    uint8 *uint8_data,
    uint32 *uint32_data)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if ((nof_bytes == MDB_KAPS_KEY_WIDTH_IN_BYTES(unit))
            || (nof_bytes == dnx_data_mdb.kaps.ads_byte_width_get(unit))
            || (nof_bytes == dnx_data_mdb.kaps.rpb_byte_width_get(unit))
            || (nof_bytes == dnx_data_mdb.kaps.bb_byte_width_get(unit)))
        {
            int uint32_index, uint32_start_index;
            int uint32_max = BYTES2WORDS(nof_bytes);
            int uint8_offset = 0;
            if (nof_bytes % WORDS2BYTES(1) != 0)
            {
                uint32_data[uint32_max - 1] = 0;
                for (uint8_offset = 0; uint8_offset < nof_bytes % WORDS2BYTES(1); uint8_offset++)
                {
                    uint32_data[uint32_max - 1] = uint32_data[uint32_max - 1] << 8;
                    uint32_data[uint32_max - 1] |= uint8_data[uint8_offset];
                }
                uint32_start_index = 1;
            }
            else
            {
                uint32_start_index = 0;
            }

            for (uint32_index = uint32_start_index; uint32_index < uint32_max; uint32_index++)
            {
                uint32_data[uint32_max - 1 - uint32_index] = (uint8_data[uint8_offset + 0] << 24) |
                    (uint8_data[uint8_offset + 1] << 16) |
                    (uint8_data[uint8_offset + 2] << 8) | uint8_data[uint8_offset + 3];
                uint8_offset += 4;
            }
        }
        else if (nof_bytes == MDB_KAPS_AD_WIDTH_IN_BYTES(unit))
        {
            uint32_data[0] = uint8_data[0] << 12 | uint8_data[1] << 4 | uint8_data[2] >> 4;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Error:  unexpected nof_bytes to mdb_lpm_uint32_to_uint8.\n");
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
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; /** currently only the first physical table is considered */
    mdb_kaps_db_handles_t db_handles_info;
    mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_add(unit,
                                          db_handles_info.db_p,
                                          db_handles_info.ad_db_p,
                                          db_handles_info.hb_db_p,
                                          entry->key,
                                          dnx_data_mdb.kaps.key_width_in_bits_get(unit),
                                          entry->payload,
                                          dnx_data_mdb.kaps.ad_width_in_bits_get(unit), entry->prefix_length,
                                          &(entry->indirect_commit_mode), entry->hitbit, 0, NULL));

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_write(unit, dbal_physical_table_id, app_id, entry));
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
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "prefix_length: %d. dbal_physical_table: %s, app_id: %d.\n"),
                                     entry->prefix_length, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                     app_id));
        for (data_offset = 0; data_offset < MDB_KAPS_KEY_WIDTH_IN_UINT32(unit); data_offset++)
        {
            print_index = MDB_KAPS_KEY_WIDTH_IN_UINT32(unit) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(dnx_data_mdb.kaps.ad_width_in_bits_get(unit)); data_offset++)
        {
            print_index = BITS2WORDS(dnx_data_mdb.kaps.ad_width_in_bits_get(unit)) - 1 - data_offset;
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
    int res;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub))
    {
        res = adapter_mdb_access_lpm_read(unit, dbal_physical_table_id, app_id, entry);
    }
    else
#endif
    {
        mdb_kaps_db_handles_t db_handles_info;
        mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;

        SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

        SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

        if (db_handles_info.is_valid != 1)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }

        res = dnx_kbp_entry_lpm_get(unit,
                                    db_handles_info.db_p,
                                    db_handles_info.ad_db_p,
                                    db_handles_info.hb_db_p,
                                    entry->key,
                                    dnx_data_mdb.kaps.key_width_in_bits_get(unit),
                                    entry->payload, dnx_data_mdb.kaps.ad_width_in_bits_get(unit), entry->prefix_length,
                                    &entry->hitbit, NULL, NULL);
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
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "prefix_length: %d. dbal_physical_table: %s, app_id: %d.\n"),
                                     entry->prefix_length, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                     app_id));
        for (data_offset = 0; data_offset < MDB_KAPS_KEY_WIDTH_IN_UINT32(unit); data_offset++)
        {
            print_index = MDB_KAPS_KEY_WIDTH_IN_UINT32(unit) - 1 - data_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry->key[%d]: %08X.\n"), print_index, entry->key[print_index]));
        }
        for (data_offset = 0; data_offset < BITS2WORDS(dnx_data_mdb.kaps.ad_width_in_bits_get(unit)); data_offset++)
        {
            print_index = BITS2WORDS(dnx_data_mdb.kaps.ad_width_in_bits_get(unit)) - 1 - data_offset;
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
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; /** currently only the first physical table is considered */
    mdb_kaps_db_handles_t db_handles_info;
    mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;
    bsl_severity_t severity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

    if (db_handles_info.is_valid != 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, " Error: Physical table %s not allocated\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_delete(unit,
                                             db_handles_info.db_p,
                                             db_handles_info.ad_db_p,
                                             db_handles_info.hb_db_p,
                                             entry->key, dnx_data_mdb.kaps.key_width_in_bits_get(unit),
                                             entry->prefix_length, 0, NULL, NULL));

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_delete(unit, dbal_physical_table_id, app_id, entry));
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
    mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));
    if (db_handles_info.is_valid != 1)
    {
        physical_entry_iterator->mdb_lpm_db_p = NULL;
        physical_entry_iterator->mdb_lpm_ad_db_p = NULL;
        physical_entry_iterator->mdb_lpm_iter = NULL;
    }
    else
    {
        physical_entry_iterator->mdb_lpm_db_p = db_handles_info.db_p;
        physical_entry_iterator->mdb_lpm_ad_db_p = db_handles_info.ad_db_p;

        DNX_KBP_TRY(kbp_db_entry_iter_init
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
    struct kbp_entry *kpb_e = NULL;
    struct kbp_entry_info kpb_e_info;

    SHR_FUNC_INIT_VARS(unit);

    if (physical_entry_iterator->mdb_lpm_db_p != NULL)
    {
        DNX_KBP_TRY(kbp_db_entry_iter_next
                    (physical_entry_iterator->mdb_lpm_db_p, physical_entry_iterator->mdb_lpm_iter, &kpb_e));
    }

    if (kpb_e == NULL)
    {
        *is_end = TRUE;
    }
    else
    {
        DNX_KBP_TRY(kbp_entry_get_info(physical_entry_iterator->mdb_lpm_db_p, kpb_e, &kpb_e_info));

        while ((app_id != MDB_APP_ID_ITER_ALL)
               && ((kpb_e_info.data[0] >> (SAL_UINT8_NOF_BITS - dnx_data_mdb.kaps.key_prefix_length_get(unit))) !=
                   app_id))
        {
            DNX_KBP_TRY(kbp_db_entry_iter_next
                        (physical_entry_iterator->mdb_lpm_db_p, physical_entry_iterator->mdb_lpm_iter, &kpb_e));

            if (kpb_e == NULL)
            {
                *is_end = TRUE;
                break;
            }
            else
            {
                DNX_KBP_TRY(kbp_entry_get_info(physical_entry_iterator->mdb_lpm_db_p, kpb_e, &kpb_e_info));
            }
        }

        if (kpb_e != NULL)
        {
            if (kpb_e_info.ad_handle != NULL)
            {
                uint8 ad_8[BITS2BYTES(DNX_DATA_MAX_MDB_KAPS_AD_WIDTH_IN_BITS)];

                DNX_KBP_TRY(kbp_ad_db_get(physical_entry_iterator->mdb_lpm_ad_db_p, kpb_e_info.ad_handle, ad_8));

                sal_memset(entry, 0x0, sizeof(*entry));

                entry->key_size = dnx_data_mdb.kaps.key_width_in_bits_get(unit);
                SHR_IF_ERR_EXIT(mdb_lpm_uint8_to_uint32
                                (unit, MDB_KAPS_KEY_WIDTH_IN_BYTES(unit), kpb_e_info.data, entry->key));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                (entry->k_mask, dnx_data_mdb.kaps.key_width_in_bits_get(unit) - kpb_e_info.prio_len,
                                 dnx_data_mdb.kaps.key_width_in_bits_get(unit) - 1));
                entry->prefix_length = kpb_e_info.prio_len;

                entry->payload_size = dnx_data_mdb.kaps.ad_width_in_bits_get(unit);
                SHR_IF_ERR_EXIT(mdb_lpm_uint8_to_uint32(unit, MDB_KAPS_AD_WIDTH_IN_BYTES(unit), ad_8, entry->payload));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                (entry->p_mask, 0, dnx_data_mdb.kaps.ad_width_in_bits_get(unit) - 1));

                if (physical_entry_iterator->hit_bit_flags & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
                {
                    entry->hitbit = physical_entry_iterator->hit_bit_flags;
                    SHR_IF_ERR_EXIT(mdb_lpm_entry_get(unit, dbal_physical_table_id, app_id, entry, 0));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBPSDK entry AD handle is null.\n");
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
        DNX_KBP_TRY(kbp_db_entry_iter_destroy
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
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; /** currently only the first physical table is considered */
    uint8 is_end = FALSE;
    int entries_count, current_entry;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    dbal_physical_entry_t entries[MDB_LPM_BUFF_MAX_CLEAR];
    mdb_kaps_db_handles_t db_handles_info;
    mdb_kaps_ip_db_id_e db_idx = MDB_KAPS_IP_PRIVATE_DB_ID;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    SHR_IF_ERR_EXIT(MDB_KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info));

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
mdb_lpm_dump_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    mdb_kaps_ip_db_id_e small_kaps_db;
    dbal_enum_value_field_mdb_physical_table_e big_kaps_ads, big_kaps_bb;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_KAPS_1)
    {
        small_kaps_db = MDB_KAPS_IP_PRIVATE_DB_ID;
        big_kaps_ads = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
        big_kaps_bb = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
    }
    else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_KAPS_2)
    {
        small_kaps_db = MDB_KAPS_IP_PUBLIC_DB_ID;
        big_kaps_ads = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
        big_kaps_bb = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "%s is not associated with an LPM MDB table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(mdb_lpm_xpt_dump_hw(unit, small_kaps_db, big_kaps_ads, big_kaps_bb));

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
    entry->key[4] = 0 << 26 /* app_id */  | (vrf & 0x3FFF) << 12 | dip >> 20;
    entry->key[3] = dip << 12;
    entry->prefix_length = prefix_length;
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
        entry->payload[0] = payload & ((1 << dnx_data_mdb.kaps.ad_width_in_bits_get(unit)) - 1);
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
    dbal_physical_table_def_t *dbal_physical_table;
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

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));

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
                             (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                              1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range
                (entry_duplicate.payload, 0 /* start_place */ , dnx_data_mdb.kaps.ad_width_in_bits_get(unit) - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate.payload, entry.payload,
                                 (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                                  1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
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

            SHR_IF_ERR_EXIT(mdb_lpm_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate_search.payload, entry.payload,
                             (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                              1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range(entry_duplicate_search.payload, 0 /* start_place */ ,
                                                   dnx_data_mdb.kaps.ad_width_in_bits_get(unit) - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate_search.payload, entry.payload,
                                 (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                                  1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
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
                             (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                              1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range
                (entry_duplicate.payload, 0 /* start_place */ , dnx_data_mdb.kaps.ad_width_in_bits_get(unit) - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate.payload, entry.payload,
                                 (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                                  1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
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

            SHR_IF_ERR_EXIT(mdb_lpm_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry_duplicate_search.payload, entry.payload,
                             (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                              1) / SAL_UINT32_NOF_BITS));

            if (utilex_bitstream_have_one_in_range(entry_duplicate_search.payload, 0 /* start_place */ ,
                                                   dnx_data_mdb.kaps.ad_width_in_bits_get(unit) - 1))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                (entry_duplicate_search.payload, entry.payload,
                                 (dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS -
                                  1) / SAL_UINT32_NOF_BITS));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
                     1; uint32_counter >= 0; uint32_counter--)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                }
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                for (uint32_counter =
                     ((dnx_data_mdb.kaps.ad_width_in_bits_get(unit) + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS) -
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
                SHR_IF_ERR_EXIT(mdb_lpm_entry_search(unit, 0, dbal_physical_table_id, 0, &entry_duplicate_search));

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

        if ((mode == MDB_TEST_CAPACITY))
        {
            ext_iterator++;
        }
        else
        {
            ext_iterator += sal_rand() % (max_ext_iterations / MDB_TEST_BRIEF_ENTRIES);
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "In total %d entries were added, full time measurement = %d,\navg usecs per entry add: %d\navg usecs per entry update + HW search (if full): %d\navg usecs per entry get: %d\navg usecs per entry delete: %d\n"),
              total_entry_count, full_time, add_total_time_usecs / total_entry_count,
              update_total_time_usecs / total_entry_count, get_total_time_usecs / total_entry_count,
              del_total_time_usecs / total_entry_count));

exit:
    SHR_FUNC_EXIT;
}

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
