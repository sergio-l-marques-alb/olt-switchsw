
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






shr_error_e
mdb_em_test_fill_entry(
    int unit,
    int index,
    const dbal_logical_table_t * dbal_logical_table,
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
            entry->payload[uint32_counter] = 0xFFFFFFFF;
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

        SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table_id, entry_encoding, &vmv_size, &vmv_value));

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
    const dbal_logical_table_t * dbal_logical_table,
    int physical_table_idx,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry, entry_duplicate;
    int max_ext_iterations;
    int entry_index;
    int uint32_counter;
    uint32 app_id;
    shr_error_e res;
    int payload_size_int = 0;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[physical_table_idx];
    shr_error_e rv;
    mdb_em_entry_encoding_e entry_encoding;
    uint32 entry_hw_size;
    int blk;
    soc_mem_t mdb_physical_memory;
    soc_reg_t mdb_physical_status_reg = INVALIDr;
    uint32 timer_usec = 0;
    uint32 add_total_time_usecs = 0, get_total_time_usecs = 0, del_total_time_usecs = 0;
    uint32 max_entry_add_time_usecs = 0;
    uint32 total_entry_count = 0;
    uint32 mixed_ee_max_payload_idx = 0;
    uint32 mixed_ee_payload_sizes[MDB_STEP_TABLE_NOF_ENTRY_SIZES];

    

    int ext_iterator = 0, int_iterator = 0;
    int max_capacity;
    soc_field_t interrupt_field;
    soc_reg_t mdb_interrupt_reg;
    int entry_capacity_estimate = 0;
    uint32 entry_counter_before = 0;
    uint32 desc_dma_enabled;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    desc_dma_enabled = dnx_sbusdma_desc_is_enabled(unit, mdb_em_dbal_table_to_dma_enum(unit, dbal_physical_table_id));

    physical_tables[0] = dbal_physical_table_id;
    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(entry));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id, app_id, &payload_size_int));
    entry.payload_size = payload_size_int;
    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &entry.key_size));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype == MDB_EM_TYPE_LEM)
    {
        dbal_tables_e dbal_logical_table_id;

        SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, dbal_physical_table_id, app_id, &dbal_logical_table_id));

        

        if (dbal_logical_table_id == DBAL_TABLE_FWD_MACT)
        {
            entry.payload_size = entry.payload_size / 2;
        }
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &max_capacity));

    

    if (max_capacity <= dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->ovf_cam_size)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "%s does not have HW resources available to it.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    if (mode == MDB_TEST_CAPACITY_MIXED_EE)
    {

        

        mdb_em_entry_encoding_e entry_encoding_iter;
        uint32 entry_addr_bits;

        SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &entry_addr_bits));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "  Running EM test on logical table %s, stored in physical table %s\n  key_size: %d, app_id: %d, app_id_size: %d\n"),
                  dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                  entry.key_size, dbal_logical_table->app_id, dbal_logical_table->app_id_size));
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "  Test running in multiple aspect ratio mode with the following payload sizes and entry encodings:\n")));

        entry_hw_size = 0;
        for (entry_encoding_iter = MDB_EM_ENTRY_ENCODING_ONE; entry_encoding_iter <= MDB_EM_ENTRY_ENCODING_EIGHTH;
             entry_encoding_iter++)
        {
            uint8 vmv_size;

            

            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding_iter, &vmv_size));
            if (vmv_size != 0)
            {
                mdb_em_entry_encoding_e resulting_entry_encoding;
                uint32 payload_size_iter;
                uint32 entry_hw_size_iter = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                                dbal_physical_table_id)->row_width /
                    (1 << entry_encoding_iter);

                

                payload_size_iter =
                    entry_hw_size_iter + entry_addr_bits - entry.key_size - dbal_logical_table->app_id_size - vmv_size;
                payload_size_iter =
                    UTILEX_MIN(payload_size_iter,
                               dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                   dbal_physical_table_id)->max_payload_size -
                               vmv_size);

                SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                (unit, dbal_physical_table_id, entry.key_size, payload_size_iter, app_id,
                                 dbal_logical_table->app_id_size, &resulting_entry_encoding));
                if (resulting_entry_encoding != entry_encoding_iter)
                {
                    continue;
                }

                mixed_ee_payload_sizes[mixed_ee_max_payload_idx] = payload_size_iter;

                entry_hw_size += entry_hw_size_iter;
                mixed_ee_max_payload_idx++;

                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U
                          (unit,
                           "    payload_size = %d, resulting_entry_encoding (0-ONE, 1-HALF, 2-QUARTER...) = %d.\n"),
                          payload_size_iter, resulting_entry_encoding));
            }
        }

        

        if (mixed_ee_max_payload_idx >= 2)
        {
            entry_hw_size = entry_hw_size / mixed_ee_max_payload_idx;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_NONE, "%s - unable to generate different entry encodings for the table.\n",
                         dbal_physical_table_to_string(unit, dbal_physical_table_id));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, dbal_physical_table_id, entry.key_size, entry.payload_size, app_id,
                         dbal_logical_table->app_id_size, &entry_encoding));
        entry_hw_size =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);
        SHR_IF_ERR_EXIT(mdb_db_infos.em_entry_capacity_estimate.get(unit, dbal_physical_table_id, entry_encoding,
                                                                    &entry_capacity_estimate));
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "  Running EM test on logical table %s, stored in physical table %s\n  This table can hold at most %d (expected %d) of the below entries\n  key_size: %d, payload_size: %d, app_id: %d, app_id_size: %d\n"),
                  dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                  max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE / entry_hw_size, entry_capacity_estimate, entry.key_size,
                  entry.payload_size, dbal_logical_table->app_id, dbal_logical_table->app_id_size));
    }

    

    sal_memcpy(&entry_duplicate, &entry, sizeof(entry));

    if ((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE))
    {

        

        max_ext_iterations = 1;
    }
    else
    {

        

        max_ext_iterations = max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE / entry_hw_size;
    }

    SHR_IF_ERR_EXIT(mdb_em_get_physical_memory
                    (unit, dbal_physical_table_id, &mdb_physical_memory,
                     &mdb_physical_status_reg, &blk, &mdb_interrupt_reg, &interrupt_field));

    if (mdb_physical_status_reg != INVALIDr)
    {
        uint64 reg_data;
        SHR_IF_ERR_EXIT(soc_reg_get(unit, mdb_physical_status_reg, MDB_BLOCK(unit), 0, &reg_data));
        entry_counter_before = soc_reg64_field_get(unit, mdb_physical_status_reg, reg_data, ITEM_4_35f);
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Entry counter before test: %d\n"), entry_counter_before));
    }

    

    entry_index = 0;
    while (ext_iterator < max_ext_iterations)
    {
        int max_int_iterator = 0;

        

        int_iterator = 0;
        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        do
        {
            entry_index = ext_iterator + int_iterator;
            if (mode == MDB_TEST_CAPACITY_MIXED_EE)
            {
                entry.payload_size = mixed_ee_payload_sizes[entry_index % mixed_ee_max_payload_idx];
            }
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

            if (full_time == FALSE)
            {
                timer_usec = sal_time_usecs();
            }
            rv = mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0);
            if (full_time == FALSE)
            {
                uint32 add_time = sal_time_usecs() - timer_usec;

                add_total_time_usecs += add_time;
                if (add_time > max_entry_add_time_usecs)
                {
                    max_entry_add_time_usecs = add_time;
                }
            }

            if ((rv != _SHR_E_FULL) && (rv != _SHR_E_NONE))
            {
                SHR_ERR_EXIT(rv, "Failed to add entry.\n");
            }

            if (rv != _SHR_E_FULL)
            {
                total_entry_count++;
                int_iterator++;
            }
        }
        
        while (((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE)) && (rv != _SHR_E_FULL)
               && ((desc_dma_enabled == FALSE) || (int_iterator < entry_capacity_estimate)));

        max_int_iterator = int_iterator;

        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
        if (full_time == TRUE)
        {
            add_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        if ((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE))
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "  Successfully added %d entries, this represents a total utilization of %.2f%%\n  (number_of_entries * entry_hw_size / total_hw_bits)\n"),
                      max_int_iterator,
                      (100.00 * max_int_iterator * entry_hw_size / (max_capacity * MDB_DIRECT_BASIC_ENTRY_SIZE))));
        }

        

        int_iterator = 0;
        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }
        do
        {
#ifndef ADAPTER_SERVER_MODE
            entry.hitbit = 0;
#endif

            entry_index = ext_iterator + int_iterator;
            if (mode == MDB_TEST_CAPACITY_MIXED_EE)
            {
                entry.payload_size = mixed_ee_payload_sizes[entry_index % mixed_ee_max_payload_idx];
                entry_duplicate.payload_size = entry.payload_size;
            }
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry_duplicate));

            if (full_time == FALSE)
            {
                timer_usec = sal_time_usecs();
            }
            SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0));
            if (full_time == FALSE)
            {
                get_total_time_usecs += sal_time_usecs() - timer_usec;
            }
            int_iterator++;

#ifndef ADAPTER_SERVER_MODE
            if (entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, hitbit on entry is set, hitbit value 0x%x.\n", entry.hitbit);

            }
#endif

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry.payload, entry_duplicate.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));
            if (utilex_bitstream_have_one_in_range(entry.payload, 0     
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

        if (full_time == TRUE)
        {
            get_total_time_usecs += sal_time_usecs() - timer_usec;
        }

        

        if (delete_cmd == TRUE)
        {
            if (full_time == TRUE)
            {
                timer_usec = sal_time_usecs();
            }
            int_iterator = 0;
            do
            {
                entry_index = ext_iterator + int_iterator;
                if (mode == MDB_TEST_CAPACITY_MIXED_EE)
                {
                    entry.payload_size = mixed_ee_payload_sizes[entry_index % mixed_ee_max_payload_idx];
                }
                SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

                if (full_time == FALSE)
                {
                    timer_usec = sal_time_usecs();
                }
                SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, physical_tables, app_id, &entry, 0));
                if (full_time == FALSE)
                {
                    del_total_time_usecs += sal_time_usecs() - timer_usec;
                }

                int_iterator++;
            }
            while (int_iterator < max_int_iterator);

            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
            if (full_time == TRUE)
            {
                del_total_time_usecs += sal_time_usecs() - timer_usec;
            }

            

            int_iterator = 0;
            do
            {
                entry_index = ext_iterator + int_iterator;
                if (mode == MDB_TEST_CAPACITY_MIXED_EE)
                {
                    entry_duplicate.payload_size = mixed_ee_payload_sizes[entry_index % mixed_ee_max_payload_idx];
                }
                SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

                

                SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry_duplicate));
                res = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0);
                if (res != _SHR_E_NOT_FOUND)
                {
                    

                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "Test failed, entry wasn't deleted as expected, index: %d\n"),
                              entry_index));

                }
                int_iterator++;
            }
            while (int_iterator < max_int_iterator);
        }

        if ((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE))
        {
            ext_iterator++;
        }
        else
        {

            

            ext_iterator += 1 + (sal_rand() % ((max_ext_iterations / MDB_TEST_BRIEF_ENTRIES) + 1));
        }
    }

    
#ifndef ADAPTER_SERVER_MODE
    if ((max_entry_add_time_usecs > 10000)
        && (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype == MDB_EM_TYPE_LEM)
        && (mode == MDB_TEST_CAPACITY))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, max add time is longer than expected: %d usecs.\n",
                     max_entry_add_time_usecs);
    }
#endif 

exit:
    if (mdb_physical_status_reg != INVALIDr)
    {
        uint64 reg_data;
        uint32 counter;
        SHR_IF_ERR_CONT(soc_reg_get(unit, mdb_physical_status_reg, MDB_BLOCK(unit), 0, &reg_data));
        counter = soc_reg64_field_get(unit, mdb_physical_status_reg, reg_data, ITEM_4_35f);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "Entry counter after test: %d, int_iterator: %d, ext_iterator: %d\n"),
                  counter, int_iterator, ext_iterator));

        

        if ((delete_cmd == TRUE) && (entry_counter_before != counter))
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "Entry counter before (%d) and after (%d) test mismatch.\n"), entry_counter_before, counter));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
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
                   "In total %d entries were added, full time measurement = %d,\nentry batch = %d\navg usecs per entry batch add: %d\nmax usecs for single entry add: %d\navg usecs per entry batch SW get: %d\navg usecs per entry batch delete: %d\n"),
                  total_entry_count, full_time, entry_print_factor, add_total_time_usecs / total_entry_count_factored,
                  max_entry_add_time_usecs, get_total_time_usecs / total_entry_count_factored,
                  del_total_time_usecs / total_entry_count_factored));
    }
    else
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "In total 0 entries were added\n")));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_test_iterator(
    int unit,
    const dbal_logical_table_t * dbal_logical_table,
    int physical_table_idx)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry, entry_duplicate;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    int entry_index = 0;
    int nof_iterators = 2;
    int bitmap_length = 2048;
    uint32 bitmap[64] = { 0 };
    uint32 bitmap_verify[64] = { 0 };
    uint32 app_id;
    shr_error_e res;
    int payload_size_int = 0;
    int iterator_init = FALSE;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[physical_table_idx];
    shr_error_e rv;
    int sw_iter;
    uint8 is_end = FALSE;
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);

    

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;
    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(dbal_physical_entry_t));
    sal_memset(&entry_duplicate, 0x0, sizeof(dbal_physical_entry_t));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id, app_id, &payload_size_int));
    entry.payload_size = payload_size_int;
    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &entry.key_size));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

    do
    {
        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));

        rv = mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0);

        if ((rv != _SHR_E_FULL) && (rv != _SHR_E_NONE))
        {
            SHR_ERR_EXIT(rv, "Failed to add entry.\n");
        }

        if (rv != _SHR_E_FULL)
        {
            entry_index++;

        }
    }
    while ((entry_index < bitmap_length) && (rv != _SHR_E_FULL));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(bitmap_verify, 0, entry_index - 1));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "In total %d entries were added\n"), entry_index));

    

    for (sw_iter = 0; sw_iter < nof_iterators; sw_iter++)
    {
        sal_memset(&physical_entry_iterator, 0x0, sizeof(physical_entry_iterator));

        if (sw_iter == 0)
        {
            physical_entry_iterator.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
        }
        else
        {
            physical_entry_iterator.mdb_action_apply = 0;
        }
        SHR_IF_ERR_EXIT(mdb_em_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

        iterator_init = TRUE;

        SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                        (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));

        while (is_end == FALSE)
        {
            index = entry.key[0];
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, index, dbal_logical_table, FALSE, &entry_duplicate));
            SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0));

            

            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                            (entry.payload, entry_duplicate.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));
            if (utilex_bitstream_have_one_in_range(entry.payload, 0     
                                                   , entry.payload_size - 1))
            {

                

                SHR_ERR_EXIT(rv, "Failed. The entries were not the same.\n");
            }
            else
            {

                

                utilex_bitstream_set_bit(bitmap, index);
            }
            SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                            (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));

        }

        

        SHR_IF_ERR_EXIT(utilex_bitstream_xor(bitmap, bitmap_verify, bitmap_length / SAL_UINT32_NOF_BITS));
        res = utilex_bitstream_have_one_in_range(bitmap, 0  , bitmap_length - 1);
        if (res != 0)
        {

            

            SHR_ERR_EXIT(rv, "Iterator test failed!\n");
        }

        

        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(bitmap, 0, bitmap_length - 1));

        

        SHR_IF_ERR_EXIT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
        iterator_init = FALSE;
    }

    LOG_CLI((BSL_META("Iterator test finished!\n")));

    

    SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, app_id));

exit:
    if (iterator_init == TRUE)
    {
        SHR_IF_ERR_CONT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_test_hitbit_basic(
    int unit,
    const dbal_logical_table_t * dbal_logical_table,
    int physical_table_idx)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry, entry_duplicate;
    int entry_index = 0;
    uint32 app_id, entry_hit = 0;
    int payload_size_int = 0;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[physical_table_idx];
    int index;
    int write_hitbit = 0, read_hitbit = 1;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;
    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(dbal_physical_entry_t));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id, app_id, &payload_size_int));
    entry.payload_size = payload_size_int;
    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &entry.key_size));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "  Running EM test on logical table %s, stored in physical table %s\n  key_size: %d, app_id: %d, app_id_size: %d\n"),
              dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
              entry.key_size, dbal_logical_table->app_id, dbal_logical_table->app_id_size));

    SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
    SHR_IF_ERR_EXIT(mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0));

    entry_index = entry.key[0];
    
    sal_memcpy(&entry_duplicate, &entry, sizeof(dbal_physical_entry_t));
    entry_hit = entry.hitbit;

    
    for (index = write_hitbit; index < 2; index++)
    {
        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry));
        if (index == write_hitbit)
        {
            entry.hitbit = entry_hit | DBAL_PHYSICAL_KEY_HITBIT_WRITE;
        }
        else
        {
            entry.hitbit = entry_hit | DBAL_PHYSICAL_KEY_HITBIT_GET;
        }
        SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry, 0));

        if (index == read_hitbit
            && ((entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A) == 0
                || (entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B) == 0))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, hitbit on entry is not set, hitbit value 0x%x.\n", entry.hitbit);
        }
    }
    
    SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, physical_tables, app_id, &entry, 0));
    SHR_IF_ERR_EXIT(mdb_em_entry_add(unit, physical_tables, app_id, &entry_duplicate, 0));

    SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry_duplicate));
    entry_duplicate.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_GET;
    SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry_duplicate, 0));
    if ((entry_duplicate.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A) != 0
        || (entry_duplicate.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, hitbit on entry is set, hitbit value 0x%x.\n", entry_duplicate.hitbit);
    }
    

    SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, app_id));

exit:

    SHR_FUNC_EXIT;
}



shr_error_e
mdb_em_test_hitbit_capacity(
    int unit,
    const dbal_logical_table_t * dbal_logical_table,
    int physical_table_idx)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry;
    int entry_index = 0, entry_ind;
    uint32 app_id;
    int payload_size_int = 0;
    shr_error_e rv;
    int capacity;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[physical_table_idx];
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;
    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(dbal_physical_entry_t));

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id, app_id, &payload_size_int));
    entry.payload_size = payload_size_int;
    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &entry.key_size));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "  Running EM test on logical table %s, stored in physical table %s\n  key_size: %d, app_id: %d, app_id_size: %d\n"),
              dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
              entry.key_size, dbal_logical_table->app_id, dbal_logical_table->app_id_size));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, dbal_physical_table_id, &capacity));
    entry_index = 0;
    
    for (entry_index = 0; entry_index < 1000; entry_index++)
    {
        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
        SHR_IF_ERR_EXIT(mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0));
        entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_WRITE;
        SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry, 0));
    }
    entry.hitbit &= ~DBAL_PHYSICAL_KEY_HITBIT_WRITE;

    
    for (entry_ind = 1000; entry_ind < capacity / 2; entry_ind++)
    {
        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_ind, dbal_logical_table, TRUE, &entry));
        rv = mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_FULL);
        if (rv == _SHR_E_FULL)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Successfully added %d entries.\n"), entry_ind));
            break;
        }
    }
    
    for (entry_index = 0; entry_index < 1000; entry_index++)
    {
        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, FALSE, &entry));
        entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_GET;
        SHR_IF_ERR_EXIT(mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry, 0));
        if ((entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A) == 0
            || (entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, hitbit on entry is not set, hitbit value 0x%x., entry_index %d\n",
                         entry.hitbit, entry_index);
        }
    }
    
    SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, app_id));

exit:

    SHR_FUNC_EXIT;
}


shr_error_e
mdb_em_test_single_hash_packing(
    int unit,
    const dbal_logical_table_t * dbal_logical_table,
    int physical_table_idx)
{

    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry;
    dbal_physical_tables_e dbal_physical_table_id = dbal_logical_table->physical_db_id[physical_table_idx];
    mdb_em_entry_encoding_e entry_encoding;
    uint32 app_id;
    uint32 payload_size_quarter, payload_size_half;
    uint32 entry_addr_bits;
    uint32 entry_hw_size;
    uint8 vmv_size;
    int entry_index;

    
    int delete_entry_increment_arr[] = { 4 };
    int delete_entry_increment_num = 1;
    int delete_entry_increment_iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;
    app_id = dbal_logical_table->app_id;
    sal_memset(&entry, 0x0, sizeof(entry));

    
    SHR_IF_ERR_EXIT(mdb_em_get_key_size(unit, dbal_physical_table_id, app_id, &entry.key_size));

    SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_physical_table_id, &entry_addr_bits));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "  Running EM test on logical table %s, stored in physical table %s\n  key_size: %d, app_id: %d, app_id_size: %d\n"),
              dbal_logical_table->table_name, dbal_physical_table_to_string(unit, dbal_physical_table_id),
              entry.key_size, dbal_logical_table->app_id, dbal_logical_table->app_id_size));

    
    entry_encoding = MDB_EM_ENTRY_ENCODING_QUARTER;

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding, &vmv_size));
    entry_hw_size =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);

    payload_size_quarter =
        entry_hw_size + entry_addr_bits - entry.key_size - dbal_logical_table->app_id_size - vmv_size;
    payload_size_quarter =
        UTILEX_MIN(payload_size_quarter,
                   dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size - vmv_size);

    
    entry_encoding = MDB_EM_ENTRY_ENCODING_HALF;

    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, dbal_physical_table_id, entry_encoding, &vmv_size));
    entry_hw_size =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / (1 << entry_encoding);

    payload_size_half = entry_hw_size + entry_addr_bits - entry.key_size - dbal_logical_table->app_id_size - vmv_size;
    payload_size_half =
        UTILEX_MIN(payload_size_half,
                   dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size - vmv_size);

    for (delete_entry_increment_iter = 0; delete_entry_increment_iter < delete_entry_increment_num;
         delete_entry_increment_iter++)
    {
        int nof_added_entries = 0, nof_deleted_entries = 0;
        shr_error_e rv = _SHR_E_NONE;

        
        entry.payload_size = payload_size_quarter;

        
        for (rv = _SHR_E_NONE, entry_index = 0; rv != _SHR_E_FULL; entry_index++)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
            rv = mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0);
        }
        nof_added_entries = entry_index - 1;
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Successfully added %d entries(QUARTER).\n"), nof_added_entries));

        
        for (entry_index = 0; entry_index < nof_added_entries;)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry(unit, entry_index, dbal_logical_table, TRUE, &entry));
            SHR_IF_ERR_EXIT(mdb_em_entry_delete(unit, physical_tables, app_id, &entry, 0));
            entry_index += delete_entry_increment_arr[delete_entry_increment_iter];
            nof_deleted_entries++;
        }
        sal_printf("Successfully deleted %d entries(QUARTER)!\n", nof_deleted_entries);

        
        entry.payload_size = payload_size_half;

        
        for (rv = _SHR_E_NONE, entry_index = 0; rv != _SHR_E_FULL; entry_index++)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_fill_entry
                            (unit, nof_added_entries + entry_index, dbal_logical_table, TRUE, &entry));
            rv = mdb_em_entry_add(unit, physical_tables, app_id, &entry, 0);
        }
        sal_printf("Successfully added %d entries(HALF)!\n", entry_index);

        
        SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, MDB_APP_ID_ITER_ALL));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
mdb_em_test_table_clear(
    int unit,
    const dbal_logical_table_t ** dbal_logical_tables,
    uint32 nof_dbal_logical_tables,
    dbal_physical_tables_e dbal_physical_table_id)
{
    uint32 dbal_logical_table_iter;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry, entry_duplicate;
    int entry_index = 1;
    int payload_size_int = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    physical_tables[0] = dbal_physical_table_id;

    sal_memset(&entry, 0x0, sizeof(dbal_physical_entry_t));

    
    for (dbal_logical_table_iter = 0; dbal_logical_table_iter < nof_dbal_logical_tables; dbal_logical_table_iter++)
    {
        sal_memset(&entry, 0x0, sizeof(dbal_physical_entry_t));
        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id,
                                                                dbal_logical_tables[dbal_logical_table_iter]->app_id,
                                                                &payload_size_int));
        entry.payload_size = payload_size_int;
        SHR_IF_ERR_EXIT(mdb_em_get_key_size
                        (unit, dbal_physical_table_id, dbal_logical_tables[dbal_logical_table_iter]->app_id,
                         &entry.key_size));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));

        SHR_IF_ERR_EXIT(mdb_em_test_fill_entry
                        (unit, entry_index, dbal_logical_tables[dbal_logical_table_iter], TRUE, &entry));

        SHR_IF_ERR_EXIT(mdb_em_entry_add
                        (unit, physical_tables, dbal_logical_tables[dbal_logical_table_iter]->app_id, &entry, 0));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "Added one entry in logical table %s, stored in physical table %s, app_id: %d\n"),
                  dbal_logical_tables[dbal_logical_table_iter]->table_name, dbal_physical_table_to_string(unit,
                                                                                                          dbal_physical_table_id),
                  dbal_logical_tables[dbal_logical_table_iter]->app_id));
    }

    
    dbal_logical_table_iter = 0;
    SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, dbal_logical_tables[dbal_logical_table_iter]->app_id));

    
    dbal_logical_table_iter++;
    sal_memcpy(&entry_duplicate, &entry, sizeof(entry_duplicate));
    SHR_IF_ERR_EXIT(mdb_em_test_fill_entry
                    (unit, entry_index, dbal_logical_tables[dbal_logical_table_iter], FALSE, &entry_duplicate));

    entry_duplicate.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
    SHR_IF_ERR_EXIT(mdb_em_entry_get
                    (unit, dbal_physical_table_id, dbal_logical_tables[dbal_logical_table_iter]->app_id,
                     &entry_duplicate, 0));

    
    SHR_IF_ERR_EXIT(utilex_bitstream_xor(entry_duplicate.payload, entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));

    if (utilex_bitstream_have_one_in_range
        (entry_duplicate.payload, 0  , entry_duplicate.payload_size - 1))
    {
        
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed. The entries were not the same.\n");
    }

    
    SHR_IF_ERR_EXIT(mdb_em_test_fill_entry
                    (unit, entry_index, dbal_logical_tables[dbal_logical_table_iter], FALSE, &entry_duplicate));

    entry_duplicate.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
    SHR_IF_ERR_EXIT(mdb_em_entry_get
                    (unit, dbal_physical_table_id, dbal_logical_tables[dbal_logical_table_iter]->app_id,
                     &entry_duplicate, 0));

    SHR_IF_ERR_EXIT(utilex_bitstream_xor(entry_duplicate.payload, entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_WORDS));

    if (utilex_bitstream_have_one_in_range
        (entry_duplicate.payload, 0  , entry_duplicate.payload_size - 1))
    {
        
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed. The entries were not the same.\n");
    }

    SHR_IF_ERR_EXIT(mdb_em_table_clear(unit, physical_tables, MDB_APP_ID_ITER_ALL));

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_em_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    mdb_test_mode_e mode,
    int delete_cmd,
    int full_time)
{
    uint32 dbal_logical_table_iter = 0;
    dbal_tables_e dbal_logical_table_id;
    CONST dbal_logical_table_t *dbal_logical_tables[MDB_EM_TEST_MAX_NOF_DBAL_LOGICAL_TABLES] = { NULL, NULL };

    

    uint32 mixed_ee_min_key_size = SAL_UINT32_MAX;
    dbal_tables_e mixed_ee_dbal_logical_table_id = DBAL_NOF_TABLES;

    

    int physical_table_iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%s is not associated with an MDB EM table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    

    for (dbal_logical_table_id = 0; dbal_logical_table_id < DBAL_NOF_TABLES; dbal_logical_table_id++)
    {
        int found = FALSE;

        SHR_IF_ERR_EXIT(dbal_tables_table_get
                        (unit, dbal_logical_table_id, &dbal_logical_tables[dbal_logical_table_iter]));
        for (physical_table_iter = 0;
             physical_table_iter < dbal_logical_tables[dbal_logical_table_iter]->nof_physical_tables;
             physical_table_iter++)
        {
            if (dbal_logical_tables[dbal_logical_table_iter]->physical_db_id[physical_table_iter] ==
                dbal_physical_table_id)
            {
                int payload_size_int = 0;
                uint32 key_size;

                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.get(unit, dbal_physical_table_id,
                                                                        dbal_logical_tables
                                                                        [dbal_logical_table_iter]->app_id,
                                                                        &payload_size_int));

                if ((payload_size_int == 0)
                    || (payload_size_int >
                        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->max_payload_size))
                {

                    

                    continue;
                }

                

                if ((app_id != MDB_APP_ID_ITER_ALL) && (app_id != dbal_logical_tables[dbal_logical_table_iter]->app_id))
                {
                    continue;
                }

                if (mode == MDB_TEST_CAPACITY_MIXED_EE || mode == MDB_TEST_HASH_PACKING)
                {

                    

                    SHR_IF_ERR_EXIT(mdb_em_get_key_size
                                    (unit, dbal_physical_table_id, dbal_logical_tables[dbal_logical_table_iter]->app_id,
                                     &key_size));

                    if (key_size < mixed_ee_min_key_size)
                    {
                        mixed_ee_min_key_size = key_size;
                        mixed_ee_dbal_logical_table_id = dbal_logical_table_id;
                    }
                }
                else if ((mode == MDB_TEST_TABLE_CLEAR) &&
                         ((dbal_logical_table_iter == 0) ||
                          (dbal_logical_tables[dbal_logical_table_iter - 1]->app_id ==
                           dbal_logical_tables[dbal_logical_table_iter]->app_id)))
                {
                    
                    if (dbal_logical_table_iter == 0)
                    {
                        dbal_logical_table_iter++;
                    }
                    break;
                }
                else
                {
                    found = TRUE;
                    break;
                }
            }
        }

        if (found == TRUE)
        {
            break;
        }
    }

    if (mode == MDB_TEST_CAPACITY_MIXED_EE || mode == MDB_TEST_HASH_PACKING)
    {
        dbal_logical_table_id = mixed_ee_dbal_logical_table_id;
        SHR_IF_ERR_EXIT(dbal_tables_table_get
                        (unit, dbal_logical_table_id, &dbal_logical_tables[dbal_logical_table_iter]));
        for (physical_table_iter = 0;
             physical_table_iter < dbal_logical_tables[dbal_logical_table_iter]->nof_physical_tables;
             physical_table_iter++)
        {
            if (dbal_logical_tables[dbal_logical_table_iter]->physical_db_id[physical_table_iter] ==
                dbal_physical_table_id)
            {
                break;
            }
        }
    }

    if (dbal_logical_table_id == DBAL_NOF_TABLES)
    {
        if (mode == MDB_TEST_CAPACITY_MIXED_EE)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "DBAL physical table %d is not associated with a DBAL logical table that supports mixed entry encoding, skipping table test.\n"),
                      dbal_physical_table_id));
        }
        else if (mode == MDB_TEST_TABLE_CLEAR)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "DBAL physical table %d is not associated with at least two DBAL logical tables with different app ids, skipping table test.\n"),
                      dbal_physical_table_id));
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "DBAL physical table %d is not associated with a DBAL logical table, skipping table test.\n"),
                      dbal_physical_table_id));
        }
    }
    else
    {
        dbal_tables_e dbal_logical_table_id_iter = DBAL_TABLE_EMPTY;

        

        while (TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                            (unit, dbal_logical_table_id_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB,
                             dbal_physical_table_id, DBAL_TABLE_TYPE_NONE, &dbal_logical_table_id_iter));

            if (dbal_logical_table_id_iter != DBAL_TABLE_EMPTY)
            {
                SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_logical_table_id_iter));
            }
            else
            {
                break;
            }
        }
        if (mode == MDB_TEST_ITERATOR)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_iterator
                            (unit, dbal_logical_tables[dbal_logical_table_iter], physical_table_iter));
        }
        else if (mode == MDB_TEST_HASH_PACKING)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_single_hash_packing
                            (unit, dbal_logical_tables[dbal_logical_table_iter], physical_table_iter));
        }
        else if (mode == MDB_TEST_TABLE_CLEAR)
        {
            SHR_IF_ERR_EXIT(mdb_em_test_table_clear
                            (unit, dbal_logical_tables, dbal_logical_table_iter + 1, dbal_physical_table_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_em_test_table
                            (unit, dbal_logical_tables[dbal_logical_table_iter], physical_table_iter, mode, delete_cmd,
                             full_time));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

