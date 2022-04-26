/** \file kaps_verifier.c
 *
 * kaps tests.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KAPSDNX

#define TO_BRINGUP 0

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <sal/core/boot.h>

#include <shared/utilex/utilex_framework.h>

#include <soc/dnx/mdb.h>
#include <soc/dnx/mdb_internal_shared.h>

#include <soc/dnx/utils/dnx_sbusdma_desc.h>

#include "kaps_verifier.h"

#include "kaps_algo_common.h"
#include "kaps_handle.h"
#include "kaps_lpm_algo.h"
#include "kaps_key_internal.h"

#include "kaps_uda_mgr.h"
#include "kaps_cr_pool_mgr.h"
#include "kaps_verifier_fast_test_trie.h"
#include "kaps_dma.h"

#include <inttypes.h>   /* Needed for PRIu64 */

#if TO_BRINGUP
#include "kaps_xpt.h"
#endif

#define CTEST_DNX_KAPS_INPUT_XML_FILE_PATH "/tools/sand/db/kaps/"

/**
 * }
 */

uint32_t g_searchHitCnt;        /* For Debugging */
uint64_t g_coh_search_start;

static kaps_status delete_entry_from_database(
    struct xml_test *tinfo,
    struct test_db_info *db_info);

#if TO_BRINGUP
sigjmp_buf kaps_verifier_env;
#endif

kaps_status
compare_kaps_block_ser_data(
    struct kaps_device *device,
    uint32_t blk_nr,
    uint32_t total_num_rows,
    uint32_t func_num,
    uint32_t nbytes,
    uint32_t row_incr)
{
    uint32_t row_nr;
    uint8_t data_from_ser_api[KAPS_BKT_WIDTH_8], data_from_model[KAPS_BKT_WIDTH_8];
    kaps_status status;
    struct kaps_xpt *xpt = device->xpt;

    for (row_nr = 0; row_nr < total_num_rows; row_nr += row_incr)
    {
        status = kaps_device_get_ser_data(device, blk_nr, row_nr, func_num, nbytes, data_from_ser_api);

        if (status != KAPS_OK)
            return status;

        xpt->kaps_command(xpt, KAPS_CMD_READ, func_num, blk_nr, row_nr, nbytes, data_from_model);

        if (kaps_memcmp(data_from_ser_api, data_from_model, nbytes))
        {
            return KAPS_DATA_MISMATCH;
        }
    }

    return KAPS_OK;
}

kaps_status
verify_kaps_ser_data(
    struct kaps_device * device)
{
    uint32_t blk_nr, nbytes;
    uint32_t row_incr = 1;

    for (blk_nr = device->dba_offset; blk_nr <= device->dba_offset + 1; blk_nr++)
    {
        nbytes = 2 * (KAPS_RPB_WIDTH_8 + 1);

        KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr,
                                              2 * device->hw_res->num_rows_in_rpb[blk_nr - device->dba_offset],
                                              KAPS_FUNC1, nbytes, row_incr));

    }

    for (blk_nr = device->dba_offset; blk_nr <= device->dba_offset + 1; blk_nr++)
    {
        nbytes = KAPS_ADS_WIDTH_8;

        KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr,
                                              device->hw_res->num_rows_in_rpb[blk_nr - device->dba_offset],
                                              KAPS_FUNC4, nbytes, row_incr));

    }

    for (blk_nr = KAPS2_SMALL_BB_START; blk_nr <= KAPS2_SMALL_BB_END; ++blk_nr)
    {
        nbytes = KAPS_BKT_WIDTH_8;

        KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr, device->hw_res->num_rows_in_small_bb[0],
                                              KAPS_FUNC2, nbytes, row_incr));

    }

    if (device->silicon_sub_type != KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
    {
        for (blk_nr = KAPS2_ADS2_BLOCK_START; blk_nr <= KAPS2_ADS2_BLOCK_END; ++blk_nr)
        {
            uint32_t total_num_ads2_rows = device->hw_res->ads2_depth[blk_nr - KAPS2_ADS2_BLOCK_START];

            nbytes = KAPS_ADS_WIDTH_8;

            KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr, total_num_ads2_rows, KAPS_FUNC16, nbytes, row_incr));

        }
    }

    if (device->silicon_sub_type != KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
    {
        for (blk_nr = KAPS2_LARGE_BB_START; blk_nr <= KAPS2_LARGE_BB_END; ++blk_nr)
        {
            uint32_t sw_bb_nr = blk_nr - KAPS2_LARGE_BB_START;
            uint32_t total_num_bb_rows;

            sw_bb_nr *= 2;

            if (device->map->bb_map[0][sw_bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A000_B000)
                continue;

            nbytes = KAPS_BKT_WIDTH_8;

            total_num_bb_rows = device->map->bb_map[0][sw_bb_nr].bb_num_rows;

            KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr, total_num_bb_rows, KAPS_FUNC2, nbytes, row_incr));

            if (device->map->bb_map[0][sw_bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A960_B000
                || device->map->bb_map[0][sw_bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A000_B960
                || device->map->bb_map[0][sw_bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A480_B480)
            {
                KAPS_STRY(compare_kaps_block_ser_data(device, blk_nr, total_num_bb_rows,
                                                      KAPS_FUNC10, nbytes, row_incr));

            }
        }
    }

    return KAPS_OK;

}

kaps_status
utils_print_key(
    const uint8_t * key,
    uint32_t key_width_8,
    uint32_t inhex)
{
    if (inhex)
    {
        int i;

        for (i = 0; i < key_width_8; i++)
        {
            if (i && ((i % 20) == 0))
                kaps_printf("\n");
            if ((i % 2) == 0)
                kaps_printf(" ");
            kaps_printf("%02x", key[i]);
        }
        return KAPS_OK;
    }
    else
    {
        int i, nbits;
        char *bits[] = { "0", "1", "*" };
        uint8_t mask[KAPS_HW_MAX_SEARCH_KEY_WIDTH_1 / KAPS_BITS_IN_BYTE];

        nbits = key_width_8 * 8;
        kaps_memset(mask, 0, key_width_8);
        for (i = 0; i < nbits; i++)
        {
            if (i && ((i % 80) == 0))
                kaps_printf("\n");
            if (i % 4 == 0)
                kaps_printf(" ");
            kaps_printf("%s", bits[kaps_array_get_bit(key, mask, i)]);
        }
        return KAPS_OK;
    }
}

kaps_status
utils_print_bits(
    uint32_t nbits,
    const uint8_t * data,
    const uint8_t * mask,
    FILE * fp)
{
    int i;
    char *bits[] = { "0", "1", "*" };

    for (i = 0; i < nbits; i++)
    {
        if (i && ((i % 80) == 0))
            kaps_printf("\n");
        if (i % 4 == 0)
            kaps_printf(" ");
        kaps_fprintf(fp, "%s", bits[kaps_array_get_bit(data, mask, i)]);
    }
    kaps_fprintf(fp, "\n");
    return KAPS_OK;
}

kaps_status
print_html_sw_state_and_device_dump(
    struct xml_test * tinfo)
{
    if (tinfo->flag_output_dir)
    {
        char buf[256];
        FILE *fp;

        print_html_output_file(tinfo, PRINT_AFTER);

        if (tinfo->print_dump)
        {
            print_sw_state(tinfo);
            if (kaps_snprintf(buf, sizeof(buf), "%s/test_out.xml", tinfo->flag_output_dir) < 0)
            {
                kaps_sassert(0);
            }
            fp = kaps_fopen(buf, "w");
            if (fp == NULL)
            {
                kaps_sassert(0);
            }
            KAPS_VERIFIER_ALLOC_ERR(tinfo, (kaps_device_dump(tinfo->device, 1, fp)));
            kaps_fclose(fp);
        }
    }

    return KAPS_OK;
}


void
get_kaps_device_types(
    uint32_t id,
    uint32_t sub_type)
{
    switch (id)
    {
        case KAPS_DEFAULT_DEVICE_ID:
            kaps_printf("  < KAPS DEFAULT DEVICE >  ");
            break;
        case KAPS_QUMRAN_DEVICE_ID:
            kaps_printf("  < KAPS QUMRAN DEVICE [QAX] >  ");
            break;
        case KAPS_JERICHO_PLUS_DEVICE_ID:
            kaps_printf("  < KAPS JERICHO_PLUS DEVICE :  ");
            switch (sub_type)
            {
                case KAPS_JERICHO_PLUS_SUB_TYPE_FM0:
                    kaps_printf("sub_type: JP_FM0 >  ");
                    break;
                case KAPS_JERICHO_PLUS_SUB_TYPE_FM4:
                    kaps_printf("sub_type: JP_FM4 >  ");
                    break;
                case KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0:
                    kaps_printf("sub_type: JP_MODE_FM0 >  ");
                    break;
                case KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4:
                    kaps_printf("sub_type: JP_MODE_FM4 >  ");
                    break;
            }
            break;
        case KAPS_QUX_DEVICE_ID:
            kaps_printf("  < KAPS QUX DEVICE >  ");
            break;
        case KAPS_JERICHO_2_DEVICE_ID:
            kaps_printf("  < KAPS JERICHO_2 DEVICE >  ");
            switch (sub_type)
            {
                case KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL:
                    kaps_printf("Sub Type: JR2_TWO_LEVEL >  ");
                    break;
                case KAPS_JERICHO_2_SUB_TYPE_THREE_LEVEL:
                    kaps_printf("Sub Type: JR2_THREE_LEVEL >  ");
                    break;

                case KAPS_JERICHO_2_SUB_TYPE_Q2A_THREE_LEVEL:
                    kaps_printf("Sub Type: JR2 Q2A THREE_LEVEL> ");
                    break;

                case KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL:
                    kaps_printf("Sub Type: JR2 PLUS THREE_LEVEL> ");
                    break;
            }
            break;
    }   /* switch */
    kaps_printf("\n");
}

/* Warmboot callback functions for file write/read */
int32_t
kaps_verifier_wb_nv_write(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    uint32_t i;
    int32_t ret;
    uint8_t *tmp;
    struct xml_test *tinfo = (struct xml_test *) handle;

    /*
     * Heap Memory Implementation 
     */
    if (tinfo->issu_nv_memory)
    {
        /*
         * Realloc NV 
         */
        if ((offset + size) > tinfo->nv_size)
        {
            tmp = (uint8_t *) kaps_sysmalloc(tinfo->nv_size * 2);
            if (!tmp)
            {
                kaps_printf("NV Memory alloc failed\n");
                kaps_sassert(0);
            }
            for (i = 0; i < tinfo->nv_size; i++)
                tmp[i] = tinfo->issu_nv_memory[i];
            kaps_sysfree(tinfo->issu_nv_memory);
            tinfo->issu_nv_memory = tmp;
            tinfo->nv_size = tinfo->nv_size * 2;
        }
        for (i = 0; i < size; i++)
            tinfo->issu_nv_memory[offset + i] = buffer[i];
    }   /* File Implementation */
    else
    {

        /*
         * position the file pointer to the offset from beginning of the file 
         */
        (void) kaps_fseek(tinfo->issu_dump_file, offset, SEEK_SET);

        for (i = 0; i < size; i++)
        {
            ret = sal_fputc(buffer[i], tinfo->issu_dump_file);
            if (ret == EOF)
                return -1;
        }
    }
    tinfo->num_file_ops++;

    if (offset > tinfo->peak_dump_size)
        tinfo->peak_dump_size = offset;

    return 0;
}

int32_t
kaps_verifier_wb_nv_read(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    uint32_t i;
    int32_t ret;
    struct xml_test *tinfo = (struct xml_test *) handle;

    if (tinfo->issu_nv_memory)
    {
        if ((offset + size) > tinfo->nv_size)
            return -1;
        for (i = 0; i < size; i++)
            buffer[i] = tinfo->issu_nv_memory[offset + i];
    }
    else
    {

        /*
         * position the file pointer to the offset from beginning of the file 
         */
        (void) kaps_fseek(tinfo->issu_dump_file, offset, SEEK_SET);

        for (i = 0; i < size; i++)
        {
            ret = sal_fgetc(tinfo->issu_dump_file);
            if (ret == EOF)
                return -1;
            else
                buffer[i] = (uint8_t) ret;
        }
    }
    tinfo->num_file_ops++;

    if (offset > tinfo->peak_dump_size)
        tinfo->peak_dump_size = offset;

    return 0;
}

void
db_full_handler(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_db_stats *stats)
{
    struct NlmFibStats fib_stats;
    struct kaps_db *db = db_info->parse_info->db.db;
    double uda_percent_used;
    double upper_capacity, lower_capacity;

    (void) fib_stats;
    (void) db;
    (void) uda_percent_used;

    if (!tinfo->strict_capacity_checking)
        return;

    if (!db_info->parse_info->est_capacity_per_mb)
        return;

    /*
     * Compare capacity against est_min_capacity
     */
    lower_capacity = db_info->parse_info->est_capacity_per_mb * (100 - tinfo->capacity_tolerance) / 100.0;
    lower_capacity = lower_capacity * db->common_info->total_bb_size_in_bits;
    lower_capacity = lower_capacity / (1024 * 1024);

    upper_capacity = db_info->parse_info->est_capacity_per_mb * (100 + tinfo->capacity_tolerance) / 100.0;
    upper_capacity = upper_capacity * db->common_info->total_bb_size_in_bits;
    upper_capacity = upper_capacity / (1024 * 1024);

    if (stats->num_entries < lower_capacity)
    {
        kaps_printf
            ("CAPACITY_ERROR: Capacity of the DB %d is %d which is less than lower capacity of %lf \n",
             db_info->parse_info->id, stats->num_entries, lower_capacity);

        tinfo->capacity_problem_present = 1;
    }
    else if (stats->num_entries > upper_capacity)
    {
        kaps_printf
            ("CAPACITY_ERROR: Capacity of the DB %d is %d which is more than upper capacity of %lf \n",
             db_info->parse_info->id, stats->num_entries, upper_capacity);

        tinfo->capacity_problem_present = 1;
    }

}

/**
 * Fill the AD array in device specific manner with
 * random associated data
 */
static void
fill_ad(
    struct xml_test *tinfo,
    uint16_t ad_width_8,
    uint32_t ad_width_1,
    struct kaps_verifier_ad_info *ad_info)
{
    int32_t i;
    uint32_t num_extra_bits_in_last_byte, num_bits_to_mask;
    uint8_t mask_value;

    for (i = 0; i < ad_width_8; i++)
    {
        ad_info->ad_data[i] = ((kaps_random_r(tinfo->seedp)) % 0xFE) + 1;       /* Asso-data must be non-zeros */
    }

    if (tinfo->flag_device == KAPS_DEVICE_KAPS)
    {
        num_extra_bits_in_last_byte = ad_width_1 % 8;

        num_bits_to_mask = 0;
        if (num_extra_bits_in_last_byte)
        {
            num_bits_to_mask = 8 - num_extra_bits_in_last_byte;

            mask_value = 0xFF << num_bits_to_mask;

            ad_info->ad_data[ad_width_8 - 1] &= mask_value;
        }
    }
}

/**
 * Allocate an instance of associated data used
 * by the test harness.
 */
static struct kaps_verifier_ad_info *
alloc_ad(
    struct xml_test *tinfo,
    struct kaps_parse_record *e,
    uint16_t ad_width_8,
    uint32_t ad_width_1)
{
    struct kaps_verifier_ad_info *tmp;

    if (tinfo->ad_free_list)
    {
        tmp = (struct kaps_verifier_ad_info *) tinfo->ad_free_list;
        tinfo->ad_free_list = tinfo->ad_free_list->next;
        kaps_memset(tmp, 0, sizeof(*tmp));

        if (e->user_ad_data == NULL)
            fill_ad(tinfo, ad_width_8, ad_width_1, tmp);
        else
            kaps_memcpy(tmp->ad_data, e->user_ad_data, ad_width_8);

        return tmp;
    }

    tmp = kaps_syscalloc(1, sizeof(*tmp));
    if (!tmp)
        return tmp;

    if (e->user_ad_data == NULL)
        fill_ad(tinfo, ad_width_8, ad_width_1, tmp);
    else
        kaps_memcpy(tmp->ad_data, e->user_ad_data, ad_width_8);

    return tmp;
}

/**
 * Recycle AD structure
 */
static void
free_ad(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_verifier_ad_info *ad_info)
{
    struct kaps_verifier_free_list *f_node = (struct kaps_verifier_free_list *) ad_info;

    if (ad_info == db_info->zero_size_ad_info)
    {
        return;
    }

    f_node->next = tinfo->ad_free_list;
    tinfo->ad_free_list = f_node;
}

/**
 * Recycle all AD structures in the free list
 *
 */
static void
free_all_ad(
    struct xml_test *tinfo)
{
    while (tinfo->ad_free_list)
    {
        struct kaps_verifier_ad_info *tmp = (struct kaps_verifier_ad_info *) tinfo->ad_free_list;

        tinfo->ad_free_list = tinfo->ad_free_list->next;
        kaps_sysfree(tmp);
    }
}

static void
kaps_verifier_coherency_index_change_callback(
    void *handle,
    struct kaps_db *db,
    struct kaps_entry *e,
    int32_t old_index,
    int32_t new_index)
{
    uint32_t ix_max = 1024 * 1024, ix_min = 0;
    struct xml_test *tinfo;
    struct test_db_info *test_db;
    uint32_t magic_num = 0;

    tinfo = (struct xml_test *) handle;
    test_db = get_test_info(tinfo, db);

    kaps_sassert(test_db->ix_callback_num == magic_num);
    kaps_sassert(test_db->callback_enabled);

    if (tinfo->do_not_perform_xpt_searches)
        return;

    if (tinfo->looking_for_pio == 0)
    {
        struct kaps_parse_record *add = tinfo->pio_entries;

        while (add)
        {
            struct kaps_parse_record *next = add->next_entry;

            add->next_entry = NULL;
            add = next;
        }
        tinfo->pio_entries = NULL;
    }

    if (tinfo->flag_coherency)
    {
        tinfo->looking_for_pio = 1;
        tinfo->xpt_db = test_db;
        if (!tinfo->pio_entries)
        {
            tinfo->pio_entries = test_db->e_to_parse_record[(uintptr_t) e];
        }
        else
        {
            struct kaps_parse_record *add = NULL;

            kaps_sassert(test_db->e_to_parse_record_size > (uintptr_t) e);
            add = test_db->e_to_parse_record[(uintptr_t) e];
            if (add->next_entry != NULL)
                kaps_sassert(add->num_ranges != 0);
            kaps_sassert(add->e == e);
            add->next_entry = tinfo->pio_entries;
            tinfo->pio_entries = add;
        }
    }

    if (db->device->type == KAPS_DEVICE_KAPS)
    {
        /*
         * Allow further processing within this function although AD database is present. Do not return
         */
    }
    else
    {
        /*
         * For Non-KAPS devices, simply return
         */
        if (db->common_info->ad_info.ad)
            return;
    }

    ix_min = test_db->ix_lo;
    ix_max = test_db->ix_hi;

    if (new_index != -1)
    {
        if (new_index < ix_min || new_index > ix_max)
        {
            kaps_printf("Error : New Index :%u returned is out of range (%u - %u) DBid: %d \n\n ", new_index, ix_min,
                        ix_max, db->tid);
            exit(1);
        }
    }

    if (old_index != -1)
    {
        if (old_index < ix_min || old_index > ix_max)
        {
            kaps_printf("Error : Old Index :%u returned is out of range (%u - %u) DBid: %d \n\n ", old_index, ix_min,
                        ix_max, db->tid);
            exit(1);
        }
    }

    if (new_index == -1)
    {
        test_db->ix_is_free[old_index - ix_min] = 1;
        return;
    }

    if (test_db->ix_is_free[new_index - ix_min] != 1)
    {
        kaps_printf("Error : New Index Location %u is not free, DBid :%d \n\n", new_index, db->tid);
    }

    if (old_index == -1)
    {
        test_db->ix_to_entry[new_index - ix_min] = e;
        test_db->ix_is_free[new_index - ix_min] = 0;
    }
    else if (old_index != -1 && new_index != -1)
    {
        test_db->ix_to_entry[new_index - ix_min] = e;
        if (!test_db->ix_is_free[old_index - ix_min])
            test_db->ix_is_free[new_index - ix_min] = 0;
        test_db->ix_is_free[old_index - ix_min] = 1;
        return;
    }
}

static kaps_status
realloc_e_parse_record_array(
    struct test_db_info *db_info)
{
    uint32_t i, new_size = db_info->e_to_parse_record_size * 1.5;
    struct kaps_parse_record **tmp = kaps_syscalloc(new_size, sizeof(struct kaps_parse_record *));

    if (!tmp)
        return KAPS_OUT_OF_MEMORY;

    for (i = 0; i < db_info->e_to_parse_record_size; i++)
    {
        tmp[i] = db_info->e_to_parse_record[i];
    }
    kaps_sysfree(db_info->e_to_parse_record);
    db_info->e_to_parse_record_size = new_size;
    db_info->e_to_parse_record = tmp;
    return KAPS_OK;
}

/**
 * Allocate memory for index changed callbacks
 */
static kaps_status
allocate_memory_to_ix_dbs(
    struct xml_test *tinfo)
{
    int32_t iter, num_db;
    uint32_t ix_range = 0;
    struct test_db_info *tmp = NULL;

    num_db = tinfo->num_databases;
    for (iter = 0; iter < num_db; iter++)
    {
        uint32_t lo, hi;

        lo = hi = 0;
        tinfo->db_info_array[iter].need_to_free_ix = 1;

        if (tinfo->db_info_array[iter].container_db)
        {
            if (tmp && tmp->parse_info == tinfo->db_info_array[iter].container_db)
            {
                tinfo->db_info_array[iter].ix_to_entry = tmp->ix_to_entry;
                tinfo->db_info_array[iter].ix_is_free = tmp->ix_is_free;
                tinfo->db_info_array[iter].ix_lo = tmp->ix_lo;
                tinfo->db_info_array[iter].ix_hi = tmp->ix_hi;
                tinfo->db_info_array[iter].need_to_free_ix = 0;
                continue;
            }
        }
        else
        {
            tmp = &tinfo->db_info_array[iter];
        }

        lo = 0;
        hi = 10 * 1024 * 1024;

        tinfo->db_info_array[iter].ix_lo = lo;
        tinfo->db_info_array[iter].ix_hi = hi;
        ix_range = hi - lo + 1;

        if (tinfo->db_info_array[iter].parse_info->index_range_min != 0
            || tinfo->db_info_array[iter].parse_info->index_range_max != 0)
        {
            /*
             * Only when user has specified a valid index range, verify we
             * are honoring it
             */
            if (lo != tinfo->db_info_array[iter].parse_info->index_range_min
                || hi != tinfo->db_info_array[iter].parse_info->index_range_max)
                kaps_printf("Index range specified %d:%d does not match returned value %d:%d\n",
                            tinfo->db_info_array[iter].parse_info->index_range_min,
                            tinfo->db_info_array[iter].parse_info->index_range_max, lo, hi);
        }

        if (!tinfo->db_info_array[iter].callback_enabled)
        {
            continue;
        }

        tinfo->db_info_array[iter].ix_to_entry = kaps_syscalloc(ix_range, sizeof(struct kaps_entry *));

        if (tinfo->db_info_array[iter].ix_to_entry == NULL)
            return KAPS_OUT_OF_MEMORY;

        tinfo->db_info_array[iter].ix_is_free = kaps_syscalloc(ix_range, sizeof(uint8_t));

        if (tinfo->db_info_array[iter].ix_is_free == NULL)
            return KAPS_OUT_OF_MEMORY;

        kaps_memset(tinfo->db_info_array[iter].ix_is_free, 1, ix_range);
    }

    return KAPS_OK;
}

/**
 * Free memory allocated for index changed callbacks
 */
static void
free_ix_dbs(
    struct xml_test *tinfo)
{
    int32_t iter, num_db;

    num_db = tinfo->num_databases;
    for (iter = 0; iter < num_db; iter++)
    {
        if (tinfo->db_info_array[iter].need_to_free_ix)
        {
            if (tinfo->db_info_array[iter].ix_to_entry)
                kaps_sysfree(tinfo->db_info_array[iter].ix_to_entry);
            if (tinfo->db_info_array[iter].ix_is_free)
                kaps_sysfree(tinfo->db_info_array[iter].ix_is_free);
        }
        else
        {
            tinfo->db_info_array[iter].ix_to_entry = NULL;
            tinfo->db_info_array[iter].ix_is_free = NULL;
        }
    }
}

/**
 * Print heap usage statistics
 */
static void
xml_print_heap_info(
    struct xml_test *tinfo,
    struct kaps_allocator *default_allocator)
{
    struct kaps_default_allocator_stats stats;

    kaps_default_allocator_get_stats(default_allocator, &stats);

    /*
     * The correct portable way to print uint64_t is to use PRIu64. Otherwise garbage values were being printed on 32b
     * platforms when printing uint64_t values 
     */
    kaps_printf("HEAPS     nallocs:%" PRIu64 ", nfrees:%" PRIu64 ", peak bytes:%" PRIu64
                ", peak:%0.2fMB, cumulative:%0.2fMB\n", stats.nallocs, stats.nfrees, stats.peak_bytes,
                ((float) stats.peak_bytes) / (1024.0 * 1024.0), ((float) stats.cumulative_bytes) / (1024.0 * 1024.0));
}

static void
print_lpm_stats(
    struct xml_test *tinfo,
    struct kaps_db *db,
    struct test_db_info *db_info)
{
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
    struct NlmFibStats fib_stats;
    struct kaps_db_stats user_stats;

    if (db->type != KAPS_DB_LPM)
        return;

    (void) kaps_db_stats(db, &user_stats);
    (void) kaps_lpm_db_get_fib_stats(lpm_db, &fib_stats);

    kaps_lpm_db_print_fib_stats(db, &fib_stats, &user_stats);

    if (db_info)
    {
        if (user_stats.capacity_estimate > db_info->max_capacity_estimate)
            db_info->max_capacity_estimate = user_stats.capacity_estimate;

        if (user_stats.capacity_estimate < db_info->min_capacity_estimate)
            db_info->min_capacity_estimate = user_stats.capacity_estimate;

        if (db_info->min_capacity_estimate < 0xFFFFFFFF && db_info->max_capacity_estimate > 0)
        {
            kaps_printf("CAP_ESTIMATE : ");

            if (db_info->min_capacity_estimate < 0xFFFFFFFF)
                kaps_printf("Minimum estimate = %d ", db_info->min_capacity_estimate);

            if (db_info->max_capacity_estimate > 0)
                kaps_printf(", Maximum estimate = %d, ", db_info->max_capacity_estimate);

            kaps_printf("Most recent estimate = %d\n", db_info->most_recent_capacity_estimate);

            if (tinfo->verify_db_stats && db_info->least_full_point)
            {
                double min_estimate_percent_error, max_estimate_percent_error;
                double most_recent_estimate_percent_error;

                min_estimate_percent_error =
                    (abs((int) (db_info->min_capacity_estimate - db_info->least_full_point)) * 100.0) /
                    db_info->least_full_point;
                max_estimate_percent_error =
                    (abs((int) (db_info->max_capacity_estimate - db_info->least_full_point)) * 100.0) /
                    db_info->least_full_point;
                most_recent_estimate_percent_error =
                    (abs((int) (db_info->most_recent_capacity_estimate - db_info->least_full_point)) * 100.0) /
                    db_info->least_full_point;

                kaps_printf("Minimum estimate percent error = %lf\n", min_estimate_percent_error);

                kaps_printf("Maximum estimate percent error = %lf\n", max_estimate_percent_error);

                kaps_printf("Most recent estimate percent error = %lf\n", most_recent_estimate_percent_error);

                /*
                 * We are keeping the allowed min estimate error and max estimate error upto 30%. This may look high
                 * The reason we are doing is that as entries are added, the granularity of entries can go on
                 * decreasing. So we can end up with more entries than what we initially estimated. So for instance in
                 * the sequential pattern, the initial min_estimate percent error is upto 22%.
                 */

                if (min_estimate_percent_error > 30)
                {
                    kaps_printf("DB_STATS_ERROR: Percentage Error in Min estimate is above permitted value \n");
                    tinfo->db_stats_problem_present = 1;
                }

                if (max_estimate_percent_error > 30)
                {
                    kaps_printf("DB_STATS_ERROR: Percentage Error in Max estimate is above permitted value \n");
                    tinfo->db_stats_problem_present = 1;
                }

                if (most_recent_estimate_percent_error > 10)
                {
                    kaps_printf("DB_STATS_ERROR: Percentage Error in Most recent estimate is above permitted value\n");
                    kaps_printf("DB_STATS_ERROR: Check if device is underutilized`\n");
                    tinfo->db_stats_problem_present = 1;
                }

            }
        }

    }
    kaps_printf("\n");
}

/**
 * Print test statistics
 */
static kaps_status
xml_print_stats(
    struct xml_test *tinfo,
    struct xml_parse_info *info,
    int32_t is_first)
{
    uint32_t lowest_capacity = -1;
    struct test_db_info *db_info = NULL;
    struct xml_parse_info *tab, *tmp;

    if (info->type != KAPS_DB_LPM)
        return KAPS_OK;

    tab = info;
    db_info = get_test_info_for_xml_db(tinfo, tab);
    kaps_printf("\n DB_INFO: %s - %d File: %s, entries:%d, width:%d",
                kaps_device_db_name(db_info->parse_info->db.db), info->id,
                info->inputs == NULL ? info->db.db->description : info->inputs[0], db_info->nentries, tab->width_1);

    for (tmp = tab; tmp; tmp = tmp->next_tab)
    {
        db_info = get_test_info_for_xml_db(tinfo, tmp);
        if (db_info->least_full_point < lowest_capacity)
            lowest_capacity = db_info->least_full_point;
    }

    if (lowest_capacity != -1)
        kaps_printf(" low capacity:%u\n", lowest_capacity);
    else
        kaps_printf(" low capacity:not hit\n");

    db_info = get_test_info_for_xml_db(tinfo, tab);
    kaps_printf("%sCore Loop PIO Stats : ", info->next_tab ? " |     " : " ");

    kaps_printf("ix_callbacks = %u, num_piowrs = %u, num_piords = %u, num_blk_ops = %u, total_ops = %u\n",
                db_info->parse_info->db.db->common_info->pio_stats.num_ix_cbs,
                db_info->parse_info->db.db->common_info->pio_stats.num_of_piowrs,
                db_info->parse_info->db.db->common_info->pio_stats.num_of_piords,
                db_info->parse_info->db.db->common_info->pio_stats.num_blk_ops,
                (db_info->parse_info->db.db->common_info->pio_stats.num_of_piowrs +
                 db_info->parse_info->db.db->common_info->pio_stats.num_blk_ops));

    kaps_printf
        ("%sadds:%u deletes:%u heavy_deletes:%u installs:%u searches:%u heavy_searches:%u iter:%u misc:%u ad_up:%u db_full:%u ix_full:%u\n",
         info->next_tab ? " |     " : " ", db_info->nadds, db_info->ndeletes, db_info->n_heavydeletes,
         db_info->ninstalls, db_info->nsearches, db_info->n_heavysearches, db_info->niter, db_info->nmisc,
         db_info->nad_updates, db_info->db_full, db_info->ix_full);

    tinfo->overall_srch_count += (db_info->nsearches + db_info->n_heavysearches);       /* main db */

    tab = info->next_tab;
    for (; tab; tab = tab->next_tab)
    {
        db_info = get_test_info_for_xml_db(tinfo, tab);

        kaps_printf(" |  \n |-- TBL_ID:%d File: %s, entries:%u, width:%u\n",
                    tab->id,
                    (db_info->parse_info->inputs ==
                     NULL ? db_info->parse_info->db.db->description : db_info->parse_info->inputs[0]),
                    db_info->nentries, tab->width_1);

        kaps_printf
            (" |     adds:%u deletes:%u heavy_deletes:%u installs:%u searches:%u heavy_searches:%u iter:%u misc:%u ad_up:%u db_full:%u ix_full:%u\n",
             db_info->nadds, db_info->ndeletes, db_info->n_heavydeletes, db_info->ninstalls, db_info->nsearches,
             db_info->n_heavysearches, db_info->niter, db_info->nmisc, db_info->nad_updates, db_info->db_full,
             db_info->ix_full);
        tinfo->overall_srch_count += (db_info->nsearches + db_info->n_heavysearches);   /* table db */
    }

    db_info = get_test_info_for_xml_db(tinfo, info);

    kaps_printf("\n");

    /*
     * print verbose stats about the algorithms
     */
    kaps_printf("#DB Statistics\n");
    kaps_printf("#    Index range %d - %d\n", db_info->ix_lo, db_info->ix_hi);

    if (info->type == KAPS_DB_LPM || info->type == KAPS_DB_EM)
    {
        struct kaps_db *tmp = NULL, *db = info->db.db;
        if (db->parent)
            db = db->parent;

        for (tmp = db; tmp; tmp = tmp->next_tab)
        {
            if (tmp->is_clone)
                continue;
            db_info = get_test_info(tinfo, tmp);
            print_lpm_stats(tinfo, tmp, db_info);
        }
    }

    kaps_printf("\n");

    sal_fflush(stdout);
    return KAPS_OK;
}

void
print_html_output_file(
    struct xml_test *tinfo,
    enum print_html_file at)
{
    char *html_name;
    FILE *output;
    int32_t len;

    if (!tinfo->flag_output_dir)
        return;

    if (at == PRINT_BEFORE)
    {
        len =
            sal_strnlen(tinfo->flag_output_dir,
                        SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen("summary.html",
                                                                                    SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
            + 10;
    }
    else
    {
        len =
            sal_strnlen(tinfo->flag_output_dir,
                        SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen("summary_after.html",
                                                                                    SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
            + 10;
    }

    html_name = kaps_sysmalloc(len);
    if (!html_name)
    {
        kaps_printf("Could not allocate memory for printing HTML output\n");
        exit(1);
    }

    if (at == PRINT_BEFORE)
    {
        kaps_snprintf(html_name, len, "%s/summary.html", tinfo->flag_output_dir);
    }
    else
    {
        kaps_snprintf(html_name, len, "%s/summary_after.html", tinfo->flag_output_dir);
    }

    output = kaps_open_file(html_name);
    if (!output)
    {
        kaps_printf("Cannot open file for writing: %s\n", html_name);
        exit(1);
    }

    kaps_device_print_html(tinfo->device, output);

    kaps_fclose(output);
    kaps_sysfree(html_name);
}

void
print_sw_state(
    struct xml_test *tinfo)
{
    char *html_name;
    FILE *output;
    int32_t len;

    if (!tinfo->flag_output_dir)
        return;

    len =
        sal_strnlen(tinfo->flag_output_dir, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen("sw_state.log",
                                                                                                        SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
        + 10;

    html_name = kaps_sysmalloc(len);
    if (!html_name)
    {
        kaps_printf("Could not allocate memory for printing sw_state output\n");
        exit(1);
    }

    kaps_snprintf(html_name, len, "%s/sw_state.log", tinfo->flag_output_dir);

    output = kaps_open_file(html_name);
    if (!output)
    {
        kaps_printf("Cannot open file for writing: %s\n", html_name);
        exit(1);
    }

    kaps_device_print_sw_state(tinfo->device, output);

    kaps_fclose(output);
    kaps_sysfree(html_name);
}

static void
reopen_all_steps_output_file(
    struct xml_test *tinfo)
{
    if (tinfo->flag_silent_steps)
    {
        tinfo->scenario->all_steps_file = NULL;
        return;
    }

    if (!tinfo->flag_output_dir)
        return;

    tinfo->scenario->all_steps_file = kaps_fopen(tinfo->scenario->all_step_file_name, "a");
    if (!tinfo->scenario->all_steps_file)
    {
        kaps_printf("Cannot open file for writing: %s\n", tinfo->scenario->all_step_file_name);
        kaps_sassert(0);
    }
}

static void
close_all_steps_output_file(
    struct xml_test *tinfo)
{
    if (!tinfo->flag_output_dir)
        return;
    if (!tinfo->scenario->all_steps_file)
        return;
    kaps_fclose(tinfo->scenario->all_steps_file);
}

static void
init_all_steps_output_file_operations(
    struct xml_test *tinfo)
{
    int32_t len;

    if (!tinfo->flag_output_dir)
        return;

    if (tinfo->flag_silent_steps)
    {
        tinfo->scenario->all_steps_file = NULL;
        return;
    }

    len =
        sal_strnlen(tinfo->flag_output_dir,
                    SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen("all_steps.html",
                                                                                SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
        + 50;

    tinfo->scenario->all_step_file_name = kaps_sysmalloc(len);
    if (!tinfo->scenario->all_step_file_name)
    {
        kaps_printf("Could not allocate memory for printing HTML output\n");
        exit(1);
    }

    kaps_snprintf(tinfo->scenario->all_step_file_name, len, "%s/all_steps.html", tinfo->flag_output_dir);
    tinfo->scenario->all_steps_file = kaps_fopen(tinfo->scenario->all_step_file_name, "w+");
    if (!tinfo->scenario->all_steps_file)
    {
        kaps_printf("Cannot open file for writing: %s\n", tinfo->scenario->all_step_file_name);
        kaps_sassert(0);
    }

    print_dynamic_core_loop_scenario(tinfo->scenario->all_steps_file, tinfo->scenario);

    kaps_fprintf(tinfo->scenario->all_steps_file, "<h3> Summary </h3>\n");
    kaps_fprintf(tinfo->scenario->all_steps_file, "<iframe src=\"summary.html\" width=\"80%\" height=\"900\">\n");
    kaps_fprintf(tinfo->scenario->all_steps_file, "      <p>Your browser does not support iframes.</p>\n");
    kaps_fprintf(tinfo->scenario->all_steps_file, "</iframe>\n");

    close_all_steps_output_file(tinfo);
}

static void
finish_all_steps_output_file_operations(
    struct xml_test *tinfo)
{
    if (!tinfo->flag_output_dir)
        return;

    if (tinfo->flag_silent_steps)
    {
        tinfo->scenario->all_steps_file = NULL;
        return;
    }

    reopen_all_steps_output_file(tinfo);

    kaps_printf("dynamic core loop, finished execution of all steps\n");
    kaps_printf("****\n");
    kaps_fprintf(tinfo->scenario->all_steps_file, "<br><br><h4>Finished executing all steps<br><br></h4>");

    if (tinfo->scenario->all_steps_file)
        kaps_fclose(tinfo->scenario->all_steps_file);
    kaps_sysfree(tinfo->scenario->all_step_file_name);
}

static void
print_html_step_summery(
    struct xml_test *tinfo,
    int32_t total_step_counter,
    int32_t actual_step_no)
{
    char *html_name;
    FILE *output;
    int32_t len;

    if (!tinfo->flag_output_dir)
        return;

    if (tinfo->flag_silent_steps)
    {
        tinfo->scenario->all_steps_file = NULL;
        return;
    }

    len =
        sal_strnlen(tinfo->flag_output_dir,
                    SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen("steps/step_000_00.html",
                                                                                SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
        + 50;

    html_name = kaps_sysmalloc(len);
    if (!html_name)
    {
        kaps_printf("Could not allocate memory for printing HTML output\n");
        exit(1);
    }

    kaps_snprintf(html_name, len, "%s/steps/step_%03d_%02d.html",
                  tinfo->flag_output_dir, total_step_counter, actual_step_no);

    output = kaps_open_file(html_name);
    if (!output)
    {
        kaps_printf("Cannot open file for writing: %s\n", html_name);
        exit(1);
    }

    tinfo->device->hw_res->disable_header_print = 1;
    kaps_device_print_html(tinfo->device, output);
    kaps_fclose(output);
    kaps_sysfree(html_name);
    tinfo->device->hw_res->disable_header_print = 0;

    kaps_fprintf(tinfo->scenario->all_steps_file, "<br><br>");
    kaps_fprintf(tinfo->scenario->all_steps_file,
                 "<iframe src=\"steps/step_%03d_%02d.html\" width=\"70%\" height=\"900\" align=\"center\">\n",
                 total_step_counter, actual_step_no);
    kaps_fprintf(tinfo->scenario->all_steps_file, "      <p>Your browser does not support iframes.</p>\n");
    kaps_fprintf(tinfo->scenario->all_steps_file, "</iframe>\n");
    sal_fflush(tinfo->scenario->all_steps_file);
}

void
distribute_ops_among_all_databases(
    struct xml_test *tinfo)
{
    uint32_t total_capacity = 0, count = 0, iter = 0;
    struct xml_parse_info *tmp = tinfo->xml_parse_info;

    for (tmp = tinfo->xml_parse_info; tmp; tmp = tmp->next)
    {
        struct test_db_info *db_info = NULL;
        uint32_t capacity = 0;

        if (tmp->type == KAPS_DB_AD || tmp->type == KAPS_DB_COUNTER || tmp->type == KAPS_DB_TAP
            || tmp->type == KAPS_DB_DMA)
            continue;

        db_info = get_test_info_for_xml_db(tinfo, tmp);
        if (db_info == NULL)
        {
            return;
        }
        capacity = db_info->parse_info->db.db->common_info->capacity;

        db_info->user_cap_min = db_info->parse_info->user_cap_min;
        db_info->user_cap_max = db_info->parse_info->user_cap_max;
        db_info->user_ops_min = db_info->parse_info->user_ops_min;
        db_info->user_ops_max = db_info->parse_info->user_ops_max;

        if (capacity == 0)
        {
            if (tmp->parse_upto)
                capacity = tmp->parse_upto;
            else
                capacity = db_info->nentries;
        }

        total_capacity += capacity;
    }

    if (total_capacity == 0)
        return;

    count = 0;
    for (iter = 0; iter < 10000;)
    {
        float conversion_helper;
        uint32_t factor;
        int32_t total = 0;
        struct xml_parse_info *tab, *tmp = tinfo->db_info_array[count].parse_info;
        uint32_t capacity = tmp->db.db->common_info->capacity;

        if (tmp->db.db->common_info->dummy_db)
        {
            count++;
            if (count >= tinfo->num_databases)
                count = 0;
            continue;
        }

        if (capacity == 0)
        {
            if (tmp->parse_upto)
                capacity = tmp->parse_upto;
            else
                capacity = tinfo->db_info_array[count].nentries;
        }

        kaps_sassert(capacity);
        kaps_sassert(!tinfo->db_info_array[count].container_db);

        conversion_helper = (capacity * 10000.00f) / total_capacity;
        factor = (uint32_t) conversion_helper;
        for (tab = tmp; tab; tab = tab->next_tab)
        {
            struct xml_parse_info *ctab;

            total++;
            for (ctab = tab->clone; ctab; ctab = ctab->next)
                total++;
        }
        if (factor == 0)
            factor = total;
        while (factor && iter < 10000)
        {
            tinfo->ops_distr[iter] = count + (iter % total);
            factor--;
            iter++;
        }
        count = count + total;
        if (count >= tinfo->num_databases)
            count = 0;
    }
}

/**
 * Create the flat database, instruction and core loop operations
 * information.
 */
static kaps_status
create_test_info(
    struct xml_test *tinfo,
    struct xml_parse_info *xml_parse_info,
    struct xml_inst_info *xml_inst_info)
{
    struct xml_parse_info *tmp;
    struct xml_inst_info *tinst;
    int32_t count = 0, num_ad = 0;
    uint32_t total_capacity;
    uint32_t iter;
    uint32_t num_thread0_inst = 0, num_thread1_inst = 0;

    static enum test_operation operations_array_with_ad[] =
    { TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_INSTALL, TEST_INSTALL,
        TEST_DELETE, TEST_DELETE, TEST_HEAVY_SEARCH_DELETE,
        TEST_SEARCH, TEST_SEARCH, TEST_INSTALL,
        TEST_ITER,
        TEST_ENTRY,
        TEST_UPDATE_AD
    };

    static enum test_operation operations_array_no_ad[] =
    { TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_ADD, TEST_ADD, TEST_ADD, TEST_ADD,
        TEST_INSTALL, TEST_INSTALL,
        TEST_DELETE, TEST_DELETE, TEST_HEAVY_SEARCH_DELETE,
        TEST_SEARCH, TEST_SEARCH,
        TEST_ITER,
        TEST_ENTRY
    };


    tinfo->seedp = &tinfo->flag_seed;
    for (tmp = xml_parse_info; tmp; tmp = tmp->next)
    {
        struct xml_parse_info *tab;

        if (tmp->type == KAPS_DB_AD)
        {
            num_ad++;
            continue;
        }

        for (tab = tmp; tab; tab = tab->next_tab)
        {
            struct xml_parse_info *ctab;

            if (!tab->db_key)
                continue;
            count++;
            for (ctab = tab->clone; ctab; ctab = ctab->next)
                count++;
        }
    }

    kaps_printf("\n");

    if (count == 0)
    {
        return KAPS_OK;
    }

    tinfo->num_databases = count;

    tinfo->db_info_array = kaps_syscalloc(count, sizeof(struct test_db_info));
    if (!tinfo->db_info_array)
        return KAPS_OUT_OF_MEMORY;

    total_capacity = 0;
    count = 0;
    for (tmp = xml_parse_info; tmp; tmp = tmp->next)
    {
        struct xml_parse_info *tab;

        if (tmp->type == KAPS_DB_AD || tmp->type == KAPS_DB_COUNTER || tmp->type == KAPS_DB_TAP
            || tmp->type == KAPS_DB_DMA)
            continue;

        total_capacity += tmp->db.db->common_info->capacity;

        for (tab = tmp; tab; tab = tab->next_tab)
        {
            struct xml_parse_info *ctab;
            struct test_db_info *c_parent;

            kaps_sassert(tab->db_key);
            tinfo->db_info_array[count].parse_info = tab;
            tinfo->db_info_array[count].ninstructions = tab->ninstructions;
            tinfo->db_info_array[count].ad_db_info = tmp->ad_info;
            tinfo->db_info_array[count].least_full_point = -1;
            tinfo->db_info_array[count].is_brimmed = 0;
            tinfo->db_info_array[count].seedp = tinfo->seedp;
            tinfo->db_info_array[count].gmask = NULL;
            tinfo->db_info_array[count].max_capacity_estimate = 0;
            tinfo->db_info_array[count].min_capacity_estimate = 0xFFFFFFFF;
            tinfo->db_info_array[count].most_recent_capacity_estimate = 0;

            tinfo->db_info_array[count].parse_info->db.db->handle = &tinfo->db_info_array[count];
            if (tab != tmp)
                tinfo->db_info_array[count].container_db = tmp;
            c_parent = &tinfo->db_info_array[count];
            count++;

            for (ctab = tab->clone; ctab; ctab = ctab->next)
            {
                tinfo->db_info_array[count].parse_info = ctab;
                tinfo->db_info_array[count].ninstructions = ctab->ninstructions;
                tinfo->db_info_array[count].ad_db_info = tmp->ad_info;
                tinfo->db_info_array[count].least_full_point = -1;
                tinfo->db_info_array[count].container_db = tmp;
                tinfo->db_info_array[count].clone_parent = c_parent;
                tinfo->db_info_array[count].seedp = tinfo->seedp;
                tinfo->db_info_array[count].gmask = NULL;
                tinfo->db_info_array[count].max_capacity_estimate = 0;
                tinfo->db_info_array[count].min_capacity_estimate = 0xFFFFFFFF;
                tinfo->db_info_array[count].most_recent_capacity_estimate = 0;
                tinfo->db_info_array[count].parse_info->db.db->handle = &tinfo->db_info_array[count];
                count++;
            }
        }
    }

    count = 0;
    for (iter = 0; iter < 10000; iter++)
    {
        float conversion_helper;
        uint32_t factor;
        int32_t total = 0;
        struct xml_parse_info *tab, *tmp = tinfo->db_info_array[count].parse_info;

        kaps_sassert(!tinfo->db_info_array[count].container_db);
        conversion_helper =
            (tinfo->db_info_array[count].parse_info->db.db->common_info->capacity * 10000.00f) / total_capacity;
        factor = (uint32_t) conversion_helper;
        for (tab = tmp; tab; tab = tab->next_tab)
        {
            struct xml_parse_info *ctab;

            total++;
            for (ctab = tab->clone; ctab; ctab = ctab->next)
                total++;
        }
        if (factor == 0)
            factor = total;
        while (factor && iter < 10000)
        {
            tinfo->ops_distr[iter] = count + (iter % total);
            factor--;
            iter++;
        }
        count = count + total;
        if (count >= tinfo->num_databases)
            count = 0;
    }
    count = 0;
    for (tinst = xml_inst_info; tinst; tinst = tinst->next)
        count++;
    tinfo->num_instructions = count;

    tinfo->instruction_info_array = kaps_syscalloc(count, sizeof(struct test_instr_info));
    if (!tinfo->instruction_info_array)
        return KAPS_OUT_OF_MEMORY;

    count = 0;
    for (tinst = xml_inst_info; tinst; tinst = tinst->next)
    {
        tinfo->instruction_info_array[count].info = tinst;
        tinfo->instruction_info_array[count].seedp = tinfo->seedp;
        count++;
        if (tinst->thread_id)
            num_thread1_inst++;
        else
            num_thread0_inst++;
    }

    if (num_ad)
    {
        {
            tinfo->num_operations = sizeof(operations_array_with_ad) / sizeof(*operations_array_with_ad);
            tinfo->operations_array = operations_array_with_ad;
        }
    }
    else
    {
        tinfo->num_operations = sizeof(operations_array_no_ad) / sizeof(*operations_array_no_ad);
        tinfo->operations_array = operations_array_no_ad;
    }

    for (count = 0; count < tinfo->num_databases; count++)
    {
        uint32_t i, j = 0;
        struct test_db_info *db_info = &tinfo->db_info_array[count];

        if (tinfo->global_est_min_capacity != -1)
        {
            db_info->parse_info->est_min_capacity = tinfo->global_est_min_capacity;
        }

        if (db_info->parse_info->ninstructions == 0)    /* allocate memory for the DB which is participating in the
                                                         * search */
            continue;

        db_info->db_inst_info = kaps_syscalloc(db_info->parse_info->ninstructions, sizeof(uint32_t));
        for (i = 0; i < tinfo->num_instructions; i++)
        {
            struct test_instr_info *inst = &tinfo->instruction_info_array[i];
            uint32_t nsearches = 0;

            for (nsearches = 0; nsearches < inst->info->num_searches; nsearches++)
            {
                if (inst->info->desc[nsearches].db_info == db_info->parse_info)
                {
                    db_info->db_inst_info[j] = i;
                    j++;
                }
            }
        }
        kaps_sassert(j == db_info->ninstructions);
    }

    return KAPS_OK;
}

kaps_status
compare_db_and_xml_keys(
    struct kaps_parse_key * xml_key,
    struct kaps_parse_key * db_key)
{
    if (xml_key == NULL && db_key == NULL)
        return KAPS_INVALID_ARGUMENT;

    while (xml_key && db_key)
    {
        if (!sal_strncmp(db_key->fname, "PRIORITY", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
            || !sal_strncmp(db_key->fname, "AD", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            db_key = db_key->next;
            continue;
        }
        if (xml_key->width != db_key->width)
            break;
        xml_key = xml_key->next;
        db_key = db_key->next;
    }

    if (!xml_key && !db_key)
        return KAPS_OK;
    else
        return KAPS_INTERNAL_ERROR;
}

/**
 * Parse input files
 */
static kaps_status
parse_datasets(
    struct xml_test *tinfo)
{
    int32_t i;
    kaps_status status;

    /*
     * Parse or generate the entries array for the ACL
     * LPM and EM databases
     */
    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct test_db_info *db_info = &tinfo->db_info_array[i];
        uint32_t ndefault_entries = 0, iter = 0, index = 0;
        struct xml_parse_default_entries *tmp = db_info->parse_info->default_entries;

        if (db_info->clone_parent)
        {
            /*
             * use all the information from the parent
             */
            db_info->entries = db_info->clone_parent->entries;
            db_info->nentries = db_info->clone_parent->nentries;
            db_info->file_key = db_info->clone_parent->file_key;
            continue;
        }

        for (; tmp; tmp = tmp->next)
            ndefault_entries += tmp->nentries;

        if (db_info->parse_info->num_inputs == 0)
        {       /* now for all ACL can have single prefix field and have random gen */
            uint32_t parse_capacity, pattern;
            char *ttemp;

            kaps_db_get_property(db_info->parse_info->db.db, KAPS_PROP_DESCRIPTION, &ttemp);
            if (ttemp == NULL)
                return KAPS_PARSE_ERROR;
            (void) sscanf(ttemp, "%u", &pattern);

            parse_capacity = db_info->parse_info->db.db->common_info->capacity;

            if (db_info->parse_info->table_mix_percent && db_info->parse_info->table_mix_percent < 100)
            {
                if (db_info->parse_info->db.db->common_info->capacity > 100)
                    parse_capacity =
                        (db_info->parse_info->db.db->common_info->capacity / 100) *
                        db_info->parse_info->table_mix_percent;
            }

            if (db_info->parse_info->parse_upto > 0)
                parse_capacity = db_info->parse_info->parse_upto;

            if (tinfo->nentries_to_add && (parse_capacity > tinfo->nentries_to_add))
            {
                parse_capacity = tinfo->nentries_to_add;
            }

            if (db_info->parse_info->db.db->common_info->dummy_db && parse_capacity == 0)
                parse_capacity = 10;

            if (tinfo->flag_gen_unique == 0)
            {
                KAPS_TRY(kaps_parse_generate_prefixes(pattern, db_info->parse_info->width_1,
                                                      parse_capacity + ndefault_entries,
                                                      &db_info->entries, &db_info->nentries));
            }
            else
            {
                KAPS_TRY(kaps_parse_generate_unique_prefixes(tinfo->flag_device, pattern,
                                                             db_info->parse_info->width_1,
                                                             parse_capacity + ndefault_entries,
                                                             &db_info->entries, &db_info->nentries));
            }

            tmp = db_info->parse_info->default_entries;

            for (; tmp; tmp = tmp->next)
            {
                kaps_sassert(db_info->parse_info->type == KAPS_DB_LPM);

                for (iter = 0; iter < tmp->nentries; iter++)
                {
                    struct kaps_parse_record *rec = &tmp->entries[iter];

                    db_info->entries[index].priority = rec->priority;
                    db_info->entries[index].length = rec->length;
                    kaps_memcpy(db_info->entries[index].data, rec->data,
                                db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE);
                    kaps_memcpy(db_info->entries[index].mask, rec->mask,
                                db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE);
                    index++;
                }
            }

            kaps_sassert(index == ndefault_entries);
            db_info->file_key = db_info->parse_info->db_key;
        }
        else
        {
            int32_t parse_capacity;
            uint32_t ad_width = 0;

            kaps_sassert(db_info->parse_info->num_inputs != 0);

            parse_capacity = db_info->parse_info->db.db->common_info->capacity;

            if (db_info->parse_info->table_mix_percent && db_info->parse_info->table_mix_percent < 100)
            {
                if (db_info->parse_info->db.db->common_info->capacity > 100)
                    parse_capacity =
                        (db_info->parse_info->db.db->common_info->capacity / 100) *
                        db_info->parse_info->table_mix_percent;
            }

            if (db_info->parse_info->parse_upto > 0)
                parse_capacity = db_info->parse_info->parse_upto;

            if (tinfo->nentries_to_add && (parse_capacity > tinfo->nentries_to_add))
            {
                parse_capacity = tinfo->nentries_to_add;
            }

            if (tinfo->nentries_to_add && (parse_capacity == 0))
            {
                parse_capacity = tinfo->nentries_to_add;
            }

            status =
                parse_standard_db_file(db_info->parse_info->type, db_info->parse_info->inputs[0], &db_info->entries,
                                       parse_capacity, ndefault_entries, &ad_width, &db_info->nact_entries,
                                       &db_info->nentries, &db_info->file_key);

            if (status != KAPS_OK)
            {
                if (status == KAPS_OUT_OF_MEMORY)
                    kaps_printf("Too big data in file %s, ran out of memory when parsing it.\n",
                                db_info->parse_info->inputs[0]);
                else
                    kaps_printf("Error in file %s\n", db_info->parse_info->inputs[0]);
                return status;
            }

            if (ad_width)
            {
                /*
                 * XML AD WIDTH should match with dataset ad width 
                 */
                if (db_info->ad_db_info->ad_width_8 != ad_width / 8)
                {
                    kaps_printf("Error in File %s  AD width should be same in XML and dataset file \n",
                                db_info->parse_info->inputs[0]);
                    return KAPS_INTERNAL_ERROR;
                }

                if (db_info->ad_db_info->next)
                {
                    kaps_printf("Error in File %s  Multiple AD width parsing is not yet supported\n",
                                db_info->parse_info->inputs[0]);
                    return KAPS_INTERNAL_ERROR;
                }
            }

            tmp = db_info->parse_info->default_entries;

            for (; tmp; tmp = tmp->next)
            {
                for (iter = 0; iter < tmp->nentries; iter++)
                {
                    struct kaps_parse_record *rec = &tmp->entries[iter];

                    db_info->entries[index].priority = rec->priority;
                    db_info->entries[index].length = rec->length;
                    kaps_memcpy(db_info->entries[index].data, rec->data,
                                db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE);
                    kaps_memcpy(db_info->entries[index].mask, rec->mask,
                                db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE);
                    index++;
                }
            }

            kaps_sassert(index == ndefault_entries);

            status = compare_db_and_xml_keys(db_info->parse_info->db_key, db_info->file_key);

            if (status != KAPS_OK)
            {
                kaps_printf("Miss_Match in xml_key & dataset key for file %s\n", db_info->parse_info->inputs[0]);
                return status;
            }

        }
    }

    return KAPS_OK;
}

/**
 * Create independent fast trie based search verification structures
 */

static kaps_status
create_search_trees(
    struct xml_test *tinfo)
{
    /*
     * Build the independent fast test trie for verification
     * in the inner loop
     */
    int32_t i;

    tinfo->flag_use_pivot = 1;

    if (tinfo->flag_defer_deletes)
        tinfo->flag_use_pivot = 0;      /* Here we are using visited with 2 so it is difficult to maintain the changes
                                         * but it is possible */

    if (tinfo->flag_coherency || tinfo->flag_random == 0)
        tinfo->flag_use_pivot = 0;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct kaps_parse_key *tmp;
        int32_t j, key_width = 0;
        struct test_db_info *db_info = &tinfo->db_info_array[i];
        uint32_t ndefault_entries = 0, index = 0;
        struct xml_parse_default_entries *default_entries = db_info->parse_info->default_entries;

        for (; default_entries; default_entries = default_entries->next)
            ndefault_entries += default_entries->nentries;

        for (tmp = db_info->parse_info->db_key; tmp; tmp = tmp->next)
        {
            key_width += tmp->width;
            if (tmp->type == KAPS_KEY_FIELD_HOLE)
            {
                db_info->has_explicit_bmr = 1;
            }
        }

        if (db_info->has_explicit_bmr)
        {
            db_info->gmask = kaps_syscalloc(key_width / 8, sizeof(uint8_t));

            if (!db_info->gmask)
                return KAPS_OUT_OF_MEMORY;

            key_width = 0;
            for (tmp = db_info->parse_info->db_key; tmp; tmp = tmp->next)
            {
                uint32_t offset = key_width / 8;
                uint32_t width = tmp->width / 8;

                key_width += tmp->width;
                if (tmp->type == KAPS_KEY_FIELD_HOLE)
                {
                    kaps_memset(&db_info->gmask[offset], 0xff, width);
                }
            }
        }

        if (db_info->clone_parent)
        {
            if (db_info->has_explicit_bmr == 0)
                db_info->fast_trie = db_info->clone_parent->fast_trie;
            else
            {
                KAPS_TRY(fast_test_trie_build(db_info->entries, db_info->nentries,
                                              db_info->parse_info->type, key_width / KAPS_BITS_IN_BYTE,
                                              db_info->gmask, &db_info->fast_trie));
            }
            db_info->indices = db_info->clone_parent->indices;
            continue;
        }

        if (tinfo->flag_use_pivot)
        {
            db_info->indices = kaps_syscalloc(db_info->nentries, sizeof(uint32_t));
            if (db_info->indices == NULL)
                return KAPS_OUT_OF_MEMORY;
        }

        if (tinfo->maintain_e_to_parse_record_table)
        {
            db_info->e_to_parse_record_size = db_info->nentries + 9;
            db_info->e_to_parse_record =
                kaps_syscalloc(db_info->e_to_parse_record_size, sizeof(struct kaps_parse_record *));
            if (db_info->e_to_parse_record == NULL)
                return KAPS_OUT_OF_MEMORY;
        }

        /*
         * Construct the priority for default entries 
         */
        if (db_info->parse_info->default_entries)
        {
            uint32_t iter = 0;
            struct xml_parse_default_entries *tmp = db_info->parse_info->default_entries;

            for (; tmp; tmp = tmp->next)
            {
                for (iter = 0; iter < tmp->nentries; iter++)
                {
                    struct kaps_parse_record *rec = &tmp->entries[iter];

                    rec->priority = 255 - rec->length;
                    rec->priority |= tmp->priority << 8;
                    db_info->entries[index].visited = 0;
                    db_info->entries[index].status = FAST_TRIE_INACTIVE;
                    db_info->entries[index].e = NULL;
                    db_info->entries[index].ad_data = NULL;
                    db_info->entries[index].priority = rec->priority;
                    db_info->entries[index].length = rec->length;

                    if (db_info->indices)
                        db_info->indices[index] = index;
                    index++;
                }
            }

            /*
             * As we have copied the default entries release the memory in xml parse 
             */
            tmp = db_info->parse_info->default_entries;
            while (tmp)
            {
                struct xml_parse_default_entries *next = tmp->next;

                kaps_parse_destroy(tmp->entries);
                kaps_sysfree(tmp);
                tmp = next;
            }
        }

        for (j = index; j < db_info->nentries; j++)
        {
            db_info->entries[j].visited = 0;
            db_info->entries[j].status = FAST_TRIE_INACTIVE;
            db_info->entries[j].e = NULL;
            db_info->entries[j].ad_data = NULL;
            db_info->entries[j].next = NULL;

            if (db_info->indices)
                db_info->indices[j] = j;
        }

        db_info->end_index_default_entries = ndefault_entries;

        db_info->pivot = db_info->nentries;

        if (!tinfo->flag_num_iterations && !tinfo->flag_random && !tinfo->flag_coherency)
            continue;

        KAPS_TRY(fast_test_trie_build(db_info->entries, db_info->nentries,
                                      db_info->parse_info->type, key_width / KAPS_BITS_IN_BYTE,
                                      NULL, &db_info->fast_trie));
    }

    return KAPS_OK;
}

/**
 *
 * Shift specific database keys into the master search key, only
 * if the master key segments have not already been populated
 * into
 */

static void
shift_db_key_fields_into_search_key(
    struct kaps_parse_key *master_key,
    struct kaps_parse_key *db_key,
    uint8_t * db_record,
    uint8_t * search_key,
    uint8_t * tuple_visited_map)
{
    struct kaps_parse_key *db_key_field;
    uint32_t db_key_offset_8;

    db_key_field = db_key;
    db_key_offset_8 = 0;
    while (db_key_field)
    {
        uint32_t found = 0, index = 0;
        uint32_t master_key_offset_8 = 0;
        struct kaps_parse_key *master_key_field = master_key;

        if (db_key_field->type == KAPS_KEY_FIELD_HOLE)
        {
            db_key_offset_8 += (db_key_field->width / KAPS_BITS_IN_BYTE);
            db_key_field = db_key_field->next;
            continue;
        }

        while (master_key_field)
        {
            if (sal_strncmp(master_key_field->fname, db_key_field->fname, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
                == 0)
            {
                found = 1;
                break;
            }
            master_key_offset_8 += (master_key_field->width / KAPS_BITS_IN_BYTE);
            master_key_field = master_key_field->next;
            index++;
        }

        kaps_sassert(found == 1);

        /*
         * Only if the master key field tuple has
         * not already been visited, we will copy our
         * database key contents onto it
         */
        if (tuple_visited_map[index] == 0)
        {
            kaps_memcpy(&search_key[master_key_offset_8],
                        &db_record[db_key_offset_8], db_key_field->width / KAPS_BITS_IN_BYTE);
            tuple_visited_map[index] = 1;
        }

        db_key_offset_8 += db_key_field->width / KAPS_BITS_IN_BYTE;
        db_key_field = db_key_field->next;
    }
}

/**
 *
 * From the master search key extract the field elements
 * if they have already been populated by another
 * database. If they have not been populated we will
 * assume this database will take control of those
 * master key segments
 *
 */
static void
copy_db_key_fields_from_search_key(
    struct kaps_parse_key *master_key,
    struct kaps_parse_key *db_key,
    uint8_t * db_record,
    uint8_t * search_key,
    uint8_t * tuple_visited_map)
{
    struct kaps_parse_key *db_key_field;
    uint32_t db_key_offset_8;

    db_key_field = db_key;
    db_key_offset_8 = 0;
    while (db_key_field)
    {
        uint32_t found = 0, index = 0;
        uint32_t master_key_offset_8 = 0;
        struct kaps_parse_key *master_key_field = master_key;

        if (db_key_field->type == KAPS_KEY_FIELD_HOLE)
        {
            db_key_offset_8 += (db_key_field->width / KAPS_BITS_IN_BYTE);
            db_key_field = db_key_field->next;
            continue;
        }

        while (master_key_field)
        {
            if (sal_strncmp(master_key_field->fname, db_key_field->fname, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
                == 0)
            {
                found = 1;
                break;
            }
            master_key_offset_8 += (master_key_field->width / KAPS_BITS_IN_BYTE);
            master_key_field = master_key_field->next;
            index++;
        }

        kaps_sassert(found == 1);

        /*
         * Only if the master key field tuple has
         * not already been visited, we will copy our
         * database key contents onto it
         */
        if (tuple_visited_map[index] == 1)
        {
            kaps_memcpy(&db_record[db_key_offset_8],
                        &search_key[master_key_offset_8], db_key_field->width / KAPS_BITS_IN_BYTE);
        }

        db_key_offset_8 += db_key_field->width / KAPS_BITS_IN_BYTE;
        db_key_field = db_key_field->next;
    }
}

/**
 * From the master search key extract the field elements of the key
 */
static void
get_db_key_from_master_key(
    struct kaps_parse_key *master_key,
    struct kaps_parse_key *db_key,
    uint8_t * mskey_data,
    uint8_t * dbkey_data)
{
    struct kaps_parse_key *db_key_field;
    uint32_t db_key_offset_8;

    db_key_field = db_key;
    db_key_offset_8 = 0;
    while (db_key_field)
    {
        uint32_t found = 0, index = 0;
        uint32_t master_key_offset_8 = 0;
        struct kaps_parse_key *master_key_field = master_key;

        if (db_key_field->type == KAPS_KEY_FIELD_HOLE)
        {
            db_key_offset_8 += (db_key_field->width / KAPS_BITS_IN_BYTE);
            db_key_field = db_key_field->next;
            continue;
        }

        while (master_key_field)
        {
            if (sal_strncmp(master_key_field->fname, db_key_field->fname, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
                == 0)
            {
                found = 1;
                kaps_printf("(%s, %d, %s) ", db_key_field->fname, db_key_field->width,
                            kaps_key_get_type_internal(db_key_field->type));
                break;
            }
            master_key_offset_8 += (master_key_field->width / KAPS_BITS_IN_BYTE);
            master_key_field = master_key_field->next;
            index++;
        }

        kaps_sassert(found == 1);

        /*
         * Only if the master key field tuple has exists in the db key
         * we will copy master key contents to the database key
         */
        kaps_memcpy(&dbkey_data[db_key_offset_8],
                    &mskey_data[master_key_offset_8], db_key_field->width / KAPS_BITS_IN_BYTE);

        db_key_offset_8 += db_key_field->width / KAPS_BITS_IN_BYTE;
        db_key_field = db_key_field->next;
    }
    kaps_printf("\n");
}

/**
 * Compare the AD information returned from the device to
 * stored AD information in the harness.
*/
kaps_status
compare_ad_entries(
    struct xml_test *tinfo,
    struct test_db_info *cur_db,
    int32_t i,
    struct kaps_verifier_ad_info *ad_info,
    struct kaps_complete_search_result *cmp_result,
    uint32_t priority,
    uint32_t print_error_msg,
    struct kaps_instruction *instruction)
{
    uint32_t ad_len = 0;
    uint32_t iter;
    uint32_t ad_start_byte;
    uint32_t uda_mb;
    uint8_t *ad_data = ad_info->ad_data;
    struct xml_parse_info *parse_info;
    uint8_t mask_value;
    uint32_t num_extra_bits_in_last_byte, num_bits_to_mask;

    if (cur_db->container_db)
        parse_info = cur_db->container_db;
    else
        parse_info = cur_db->parse_info;

    if (cmp_result->resp_type[i] == KAPS_INDEX_AND_NO_AD && parse_info->ad_info)
    {
        KAPS_TRY(kaps_ad_db_get_resource(ad_info->parse_ad_info->ad_db, KAPS_HW_RESOURCE_UDA, &uda_mb));
        /*
         * If AD is In-Place uda_mb will be 0, should not throw an error as response type will be KAPS_INDEX_AND_NO_AD) 
         */
        if (uda_mb)
        {
            /*
             * No AD in response, whereas we are expecting 
             */
            kaps_printf(" Expecting AD, however, response says no AD\n");
            return KAPS_MISMATCH;
        }
        else
        {
            return KAPS_OK;
        }
    }

    if (!parse_info->ad_info && cmp_result->resp_type[i] != KAPS_INDEX_AND_NO_AD)
    {
        /*
         * Not expecting AD and we got it 
         */
        kaps_printf(" Not expecting AD, however, response says there is AD\n");
        return KAPS_MISMATCH;
    }

    if (!parse_info->ad_info)
        return KAPS_OK;

    if (cur_db->parse_info->db.db->device->type == KAPS_DEVICE_KAPS)
    {
        ad_start_byte = 0;
        /*
         * KAPS has only 20-bit AD data. This has to be represented in 3 bytes which is 24 bits. Currently the KAPS XPT 
         * is filling up some garbage data in the last 4 bits. So we are masking out the last 4 bits since further down 
         * we are doing a byte wise compare
         */
        ad_len = ad_info->parse_ad_info->ad_width_8;

        num_extra_bits_in_last_byte = ad_info->parse_ad_info->ad_db->user_width_1 % 8;

        num_bits_to_mask = 0;
        if (num_extra_bits_in_last_byte)
        {
            num_bits_to_mask = 8 - num_extra_bits_in_last_byte;

            mask_value = 0xFF << num_bits_to_mask;

            cmp_result->assoc_data[i][ad_len - 1] &= mask_value;        /* mask out the last 4 bits in the last byte of 
                                                                         * the AD */
        }
    }
    else
    {
        /*
         * : Handling non 32b aligned AD sizes 
         */
        ad_len = ad_info->parse_ad_info->ad_align_8;
        ad_start_byte = ad_info->parse_ad_info->ad_offset_8;

    }

    for (iter = 0; iter < ad_len; iter++)
    {
        uint32_t j;

        if (cmp_result->assoc_data[i][iter + ad_start_byte] != ad_data[iter])
        {
            if (!print_error_msg)
                return KAPS_MISMATCH;
            kaps_printf("\n\n !!! Error: AD Mismatch !!!\n");
            kaps_printf(" Got: \n");

            for (j = 0; j < ad_len; j++)
            {
                kaps_printf("%02x ", cmp_result->assoc_data[i][j + ad_start_byte]);
            }

            kaps_printf("\n Instead of: \n");
            for (j = 0; j < ad_len; j++)
            {
                kaps_printf("%02x ", ad_data[j]);
            }
            kaps_printf("\n");

            return KAPS_MISMATCH;
        }
    }

    return KAPS_OK;
}

struct test_db_info *
get_test_info(
    struct xml_test *tinfo,
    struct kaps_db *db)
{
    int32_t i;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        if (tinfo->db_info_array[i].parse_info->db.db == db)
            return &tinfo->db_info_array[i];
    }
    kaps_sassert(0);
    return NULL;
}

struct test_db_info *
get_test_info_for_xml_db(
    struct xml_test *tinfo,
    struct xml_parse_info *db)
{
    int32_t i;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        if (tinfo->db_info_array[i].parse_info == db)
            return &tinfo->db_info_array[i];
    }
    kaps_sassert(0);
    return NULL;
}

/**
 * Calculate hit index based on device type
 */

static uint32_t
compute_hit_index(
    enum kaps_device_type device_type,
    uint32_t index,
    uint8_t dev_id)
{
    uint32_t hit_index = index;

    return hit_index;
}

/**
 * For a database participating in a search create a search
 * key and depending on the shared tuples in the master search
 * simulate the KPU and construct the final database key and
 * perform a search and gather expected hit index and results
 */

void
gather_search_expected_data(
    struct xml_test *tinfo,
    struct test_instr_info *inst_info,
    struct kaps_verifier_search_info *search_rec,
    uint8_t * master_key,
    uint8_t * master_key_visited_map,
    uint32_t force_search_key)
{
    kaps_status status;
    struct kaps_parse_record *match;
    uint32_t key_width_8;
    struct test_db_info *cur_db;
    struct kaps_db *cp_db;
    uint32_t i;
    static uint32_t gather_search_hit_cnt = 0;

    gather_search_hit_cnt++;

    cur_db = search_rec->db_info;
    cp_db = cur_db->parse_info->db.db;
    key_width_8 = cur_db->parse_info->width_1 / KAPS_BITS_IN_BYTE;

    search_rec->cur_instr = inst_info;

    if (gather_search_hit_cnt == 1 && force_search_key)
    {
        /*
         * Hardcode the key that should be searched
         */
        search_rec->key[0] = 0;
        search_rec->key[1] = 1;
        search_rec->key[2] = 0;

        for (i = 3; i < key_width_8; ++i)
            search_rec->key[i] = 255;
    }
    else if (tinfo->force_misses)
    {
        /*
         * We will attempt a MISS by randomizing the key. But note that we can't always guarantee a MISS
         */
        for (i = 0; i < key_width_8; ++i)
            search_rec->key[i] = kaps_random_r(tinfo->seedp) % 256;
    }
    else
    {
        /*
         * Generate a random key that is expected to hit the
         * record under consideration. This does not mean eventually
         * we may match a different entry in the database due
         * to priority.
         */

        status = kaps_parse_generate_random_valid_key(search_rec->selected_rec, search_rec->key, key_width_8);

        kaps_sassert(status == KAPS_OK);
    }

    /*
     * Master search key elements are typically shared, if another database
     * has shifted its values into the master key tuples, at this point
     * we will overwrite our key generated above with the fields
     * from the master search key. This means the expected hit above may
     * change
     */

    copy_db_key_fields_from_search_key(inst_info->info->master_key, cur_db->parse_info->db_key,
                                       search_rec->key, master_key, master_key_visited_map);

    match = NULL;
    status = fast_test_trie_search(cur_db->fast_trie, search_rec->key, key_width_8, cur_db->gmask, 0, &match);

    kaps_sassert(status == KAPS_OK);
    search_rec->trie_match = match;

    /*
     * Perform a control plane search also, NOTE: When coherency
     * is enabled, we cannot directly match the control plane
     * search with the fast trie search
     */
    status = kaps_db_search(cp_db, search_rec->key,
                            &search_rec->cp_entry, &search_rec->cp_index, &search_rec->priority);
    kaps_sassert(status == KAPS_OK);

    if (search_rec->cp_entry == NULL)
    {
        kaps_sassert(match == NULL);
    }
    else
    {
        kaps_sassert(match != NULL);
        if (cur_db->parse_info->type == KAPS_DB_LPM)
        {
            kaps_sassert(match->e == search_rec->cp_entry);
        }
    }

    if (cur_db->callback_enabled)
    {
        struct test_db_info *callback_info_db;

        if (cur_db->clone_parent)
            callback_info_db = cur_db->clone_parent;
        else
            callback_info_db = cur_db;

        if (search_rec->cp_entry
            && (callback_info_db->ix_to_entry[search_rec->cp_index - callback_info_db->ix_lo] != search_rec->cp_entry))
        {
            kaps_printf("Error : Index Data Stored does not match SW search result \n\n");
            kaps_sassert(0);
        }
    }

    if (match && tinfo->flag_device != KAPS_DEVICE_KAPS)
    {
        status = kaps_entry_get_index(cur_db->parse_info->db.db,
                                      search_rec->cp_entry, &search_rec->num_indices, &search_rec->indices);
        kaps_sassert(status == KAPS_OK);
    }

    if (tinfo->flag_device == KAPS_DEVICE_INVALID)
    {
        int32_t i, srch_port;

        for (i = 0; i < tinfo->device->max_num_searches; i++)
        {
            if (inst_info->info->desc[i].db_info == cur_db->parse_info)
            {
                srch_port = inst_info->info->desc[i].result_id;
                kaps_memcpy(&master_key[srch_port * KAPS_SEARCH_INTERFACE_WIDTH_8],
                            search_rec->key, cur_db->parse_info->width_1 / KAPS_BITS_IN_BYTE);
            }
        }
        return;
    }

    shift_db_key_fields_into_search_key(inst_info->info->master_key,
                                        cur_db->parse_info->db_key,
                                        search_rec->key, master_key, master_key_visited_map);
}

static kaps_status
compare_entries(
    struct xml_test *tinfo,
    struct kaps_complete_search_result *cmp_result,
    struct kaps_verifier_search_info search_data[],
    struct test_instr_info *inst_info,
    uint8_t * search_key,
    int32_t skip_free_index)
{
    int32_t i;
    uint8_t extract_key[KAPS_HW_MAX_SEARCH_KEY_WIDTH_8] = { 0 };

    for (i = 0; i < KAPS_INSTRUCTION_MAX_RESULTS; i++)
    {
        struct test_db_info *cur_db;
        int32_t error = 0, hw_result = -1, has_index = 1;

        if (cmp_result->hit_or_miss[i])
        {
            hw_result = compute_hit_index(tinfo->device->type, cmp_result->hit_index[i], cmp_result->hit_dev_id[i]);
            if (search_data[i].trie_match)
                search_data[i].trie_match->hit = 1;
        }

        if (cmp_result->resp_type[i] > KAPS_INDEX_AND_256B_AD)
            has_index = 0;

        cur_db = search_data[i].db_info;

        if (!cur_db)
            continue;

        if (has_index)
        {
            /*
             * Kaps produces no hit index, check this only for
             * other device types
             */
            if (hw_result == -1 && search_data[i].num_indices == 0)
            {
                /*
                 * No expected match
                 */
                if (search_data[i].trie_match)
                    kaps_sassert(search_data[i].trie_match->status == FAST_TRIE_INACTIVE);
                kaps_sassert(cmp_result->hit_or_miss[i] == KAPS_MISS);
                error = 0;
            }
            else if (hw_result == -1 && search_data[i].num_indices > 0)
            {
                int32_t j;

                kaps_printf("\n----------------------------------------------------------------------\n");
                kaps_instruction_print(inst_info->info->instruction, stdout);
                kaps_printf
                    ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search returns miss where as expected SW hit indices ",
                     inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                for (j = 0; j < search_data[i].num_indices; j++)
                    kaps_printf("%x ", search_data[i].indices[j]);
                error = 1;
                tinfo->failed_search_id = i;
            }
            else if (hw_result != -1 && search_data[i].num_indices == 0)
            {
                kaps_printf("\n----------------------------------------------------------------------\n");
                kaps_instruction_print(inst_info->info->instruction, stdout);
                kaps_printf
                    ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search result is %x, where as not expecting SW hit ",
                     inst_info->info->instruction->id, inst_info->info->instruction->ltr, i, hw_result);
                kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                error = 1;
                tinfo->failed_search_id = i;
            }
            else
            {
                /*
                 * HW hit index must match one of the SW indices 
                 */
                int32_t j, found = 0;

                for (j = 0; j < search_data[i].num_indices; j++)
                {
                    if (hw_result == search_data[i].indices[j])
                    {
                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    /*
                     * If there are duplicate entries with same priority, then the result from HW/model might mismatch
                     * with the single result returned by kaps_db_search. Check if this is the case by generating all
                     * results with fast_test_trie_search and checking if any of them matches the result returned by
                     * HW/model 
                     */

                    uint32_t k = 0, iter = 0;
                    struct kaps_parse_record *possible_hits[FTT_MAX_POSSIBLE_MATCHES] = { 0 };

                    fast_test_trie_search(search_data[i].db_info->fast_trie, search_data[i].key,
                                          search_data[i].db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE,
                                          search_data[i].db_info->gmask, 1, possible_hits);
                    for (k = 0; k < FTT_MAX_POSSIBLE_MATCHES; k++)
                    {
                        int32_t *indices, nindices;

                        if (possible_hits[k] == NULL)
                            break;
                        kaps_entry_get_index(search_data[i].db_info->parse_info->db.db,
                                             possible_hits[k]->e, &nindices, &indices);
                        for (iter = 0; iter < nindices; iter++)
                        {
                            if (indices[iter] == hw_result)
                            {
                                error = 0;
                                found = 1;
                                /*
                                 * kaps_printf("matched Entry = %u\n", ((struct kaps_acl_entry_mp *)
                                 * search_data[i].db_info->parse_info->db.db->common_info->user_hdl_to_entry[(uint32_t)(uint64_t)(possible_hits[k]->e)])->id); 
                                 */
                                break;
                            }
                        }
                        if (iter != nindices)
                            break;
                    }
                    if (search_data[i].db_info->callback_enabled)
                    {
                        if (search_data[i].db_info->ix_to_entry[hw_result - search_data[i].db_info->ix_lo] ==
                            search_data[i].trie_match->e)
                        {
                            error = 0;
                            found = 1;
                        }
                    }
                    /*
                     * End of duplicate check 
                     */
                }

                if (!found)
                {
                    kaps_printf("\n----------------------------------------------------------------------\n");
                    kaps_instruction_print(inst_info->info->instruction, stdout);
                    kaps_printf
                        ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search result is %x, does not match SW hit indices ",
                         inst_info->info->instruction->id, inst_info->info->instruction->ltr, i, hw_result);
                    kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                    for (j = 0; j < search_data[i].num_indices; j++)
                        kaps_printf("%x ", search_data[i].indices[j]);
                    error = 1;
                    tinfo->failed_search_id = i;
                }
            }
        }

        if (tinfo->flag_device != KAPS_DEVICE_KAPS && has_index)
        {
            if (!error && ((cmp_result->hit_or_miss[i] && search_data[i].num_indices == 0)
                           || ((cmp_result->hit_or_miss[i] == KAPS_MISS) && search_data[i].num_indices > 0)))
            {
                if (cmp_result->hit_or_miss[i] && search_data[i].num_indices == 0)
                {
                    kaps_printf("\n----------------------------------------------------------------------\n");
                    kaps_instruction_print(inst_info->info->instruction, stdout);
                    kaps_printf
                        ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search result hit, where as not expecting SW hit ",
                         inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                    kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                    error = 1;
                    tinfo->failed_search_id = i;
                }
                else if ((cmp_result->hit_or_miss[i] == KAPS_MISS) && search_data[i].num_indices > 0)
                {
                    kaps_printf("\n----------------------------------------------------------------------\n");
                    kaps_instruction_print(inst_info->info->instruction, stdout);
                    kaps_printf
                        ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search returns miss where as got SW hit",
                         inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                    kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                    error = 1;
                    tinfo->failed_search_id = i;
                }
            }
        }

        if (tinfo->flag_device == KAPS_DEVICE_KAPS)
        {
            if (search_data[i].trie_match && cmp_result->hit_or_miss[i] == KAPS_MISS)
            {
                kaps_instruction_print(inst_info->info->instruction, stdout);
                kaps_printf
                    ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search returns miss whereas expected SW hit indices ",
                     inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                error = 1;
                tinfo->failed_search_id = i;
            }
            else if (cmp_result->hit_or_miss[i] == KAPS_HIT && !search_data[i].trie_match)
            {
                kaps_instruction_print(inst_info->info->instruction, stdout);
                kaps_printf
                    ("  Error: Inst ID: %d, Inst LTR: %d Result %d, HW Search returns hit whereas not expecting SW hit ",
                     inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                error = 1;
                tinfo->failed_search_id = i;
            }
        }

        if (error)
        {
            kaps_printf("\nMaster key: \n");
            utils_print_key(search_key, inst_info->info->master_key_width_1 / KAPS_BITS_IN_BYTE, 0);

            kaps_printf("\nExtract key: ");
            kaps_memset(extract_key, 0, sizeof(extract_key));
            get_db_key_from_master_key(inst_info->info->master_key, cur_db->parse_info->db_key, search_key,
                                       extract_key);
            /*
             * this will print the key extracted from the master key, but db_key and this are same 
             */
            /*
             * utils_print_key(extract_key, cur_db->parse_info->width_1 / KAPS_BITS_IN_BYTE, 0);
             */
            kaps_printf("\n%s", kaps_device_db_name(cur_db->parse_info->db.db));

            kaps_printf("%d:  Key: \n", cur_db->parse_info->id);
            utils_print_key(search_data[i].key, cur_db->parse_info->width_1 / KAPS_BITS_IN_BYTE, 0);

            kaps_printf("\nSelect entry: %p \n", search_data[i].selected_rec);
            utils_print_bits(cur_db->parse_info->width_1,
                             search_data[i].selected_rec->data, search_data[i].selected_rec->mask, stdout);
            kaps_printf("\n");
            if (search_data[i].trie_match)
            {
                kaps_printf("Match entry: %p \n", search_data[i].trie_match);
                utils_print_bits(cur_db->parse_info->width_1,
                                 search_data[i].trie_match->data, search_data[i].trie_match->mask, stdout);
            }
            else
            {
                kaps_printf("Match entry: NULL");
            }
            kaps_printf("\n----------------------------------------------------------------------\n");
            inst_info->nfail++;
        }
        else if (cmp_result->hit_or_miss[i] || !has_index)
        {
            kaps_status status;
            if (search_data[i].trie_match)
            {
                struct kaps_verifier_ad_info *ad_info
                    = (struct kaps_verifier_ad_info *) search_data[i].trie_match->ad_data;

                status =
                    compare_ad_entries(tinfo, cur_db, i, ad_info, cmp_result, search_data[i].priority, 1,
                                       inst_info->info->instruction);

                if (status != KAPS_OK)
                {
                    error = 1;
                    inst_info->nfail++;
                    kaps_printf("Search count when the failure happened = %d\n", g_searchHitCnt);
                    kaps_printf("\n -- AD mis-match for Inst ID: %d, Inst LTR: %d Result port: %d",
                                inst_info->info->instruction->id, inst_info->info->instruction->ltr, i);
                }
            }
        }

        if (search_data[i].num_indices && !skip_free_index)
            kaps_entry_free_index_array(cur_db->parse_info->db.db, search_data[i].indices);

        /*
         * return on first failure found; either in index or asso-data 
         */
        if (error)
        {
            if (!tinfo->searches_in_progress)
            {
                if (tinfo->flag_random == 0)
                {
                    kaps_sassert(tinfo->target_db);
                    kaps_printf("\n - Search Failed for %d entry\n", tinfo->target_db->nsearches);
                }
                else
                {
                    kaps_printf("\n - Search Failed for %d Iteration\n", tinfo->num_iterations);
                }
            }
            return KAPS_MISMATCH;
        }

    }
    return KAPS_OK;
}

/**
 *
 * Perform an instruction search. generate an arbitrary master key
 * by creating specific matches/mismatches for individual databases
 * and concatenating them together
 */
static kaps_status
perform_search(
    struct xml_test *tinfo,
    struct test_instr_info *inst_info)
{
    int32_t i, target_db = -1;
    uint8_t search_key[KAPS_HW_MAX_SEARCH_KEY_WIDTH_8];
    struct kaps_complete_search_result cmp_result;
    struct kaps_verifier_search_info search_data[KAPS_INSTRUCTION_MAX_RESULTS];
    struct kaps_verifier_resolution resolution[KAPS_INSTRUCTION_MAX_RESULTS];
    uint8_t master_key_visited_map[100];
    uint32_t cb_addr = inst_info->info->context_address;
    kaps_status status;
    uint32_t hits_in_nX_srch = 0;

    g_searchHitCnt++;
    if (g_searchHitCnt == 4)
    {
        int x = 0;
        ++x;    /* For debugging */
    }

    if (tinfo->xpt_type == KAPS_VERIFIER_MDB_XPT && tinfo->does_xpt_support_search)
    {
        dnx_sbusdma_desc_wait_done(tinfo->unit);
    }

    if (tinfo->target_db)
    {
        uint32_t i;
        struct test_db_info *db_info = tinfo->target_db;

        for (i = 0; i < inst_info->info->num_searches; i++)
        {
            if (inst_info->info->desc[i].db_info == db_info->parse_info)
            {
                target_db = i;
                inst_info->target_db_id = target_db;
                break;
            }
        }

        if (tinfo->flag_random == 0)
        {
            /*
             * Must be a duplicate entry in sequential core loop we can skip this 
             */
            if (db_info->entries[db_info->nsearches].visited == 0)
            {
                /*
                 * Incrementing the counter to fetch the next entry 
                 */
                db_info->nsearches++;
                return KAPS_OK;
            }
        }
    }
    else
    {
        target_db = kaps_random_r(tinfo->seedp) % inst_info->info->num_searches;
    }

    kaps_sassert(target_db != -1);
    kaps_memset(search_key, 0, sizeof(search_key));
    kaps_memset(search_data, 0, sizeof(search_data));
    kaps_memset(master_key_visited_map, 0, sizeof(master_key_visited_map));
    kaps_memset(resolution, 0, sizeof(resolution));

    inst_info->nsearches++;

    /*
     * First generate a valid key for the database under target, and
     * shift into the master key. For the remaining databases, we will
     * depend on the master key overlap to decide whether they will
     * hit or miss
     */

    {
        struct test_db_info *cur_db;
        uint8_t res_id;
        struct kaps_parse_record *add;

        res_id = inst_info->info->desc[target_db].result_id;
        cur_db = get_test_info_for_xml_db(tinfo, inst_info->info->desc[target_db].db_info);
        kaps_sassert(cur_db != NULL);

        if (tinfo->flag_random)
        {
            add = get_next_entry(cur_db, 1);
            if (add == NULL)
            {
                /*
                 * No valid entry in database, lookup invalid entry we will expect a miss 
                 */
                add = get_next_entry(cur_db, 0);
                if (!tinfo->flag_defer_deletes)
                {
                    kaps_sassert(add != NULL);
                }
                else
                {
                    if (add == NULL)
                        add = get_next_entry(cur_db, 0xf);      /* select random entry for lookup */
                    kaps_sassert(add != NULL);
                }
            }
        }
        else
        {
            add = &cur_db->entries[cur_db->nsearches];
            kaps_sassert(add);
        }

        if (tinfo->target_rec)
        {
            add = tinfo->target_rec;
        }

        cur_db->nsearches++;
        search_data[res_id].db_info = cur_db;
        search_data[res_id].selected_rec = add;

        gather_search_expected_data(tinfo, inst_info, &search_data[res_id], search_key, master_key_visited_map, 0);

        if (search_data[res_id].trie_match)
        {
            resolution[res_id].search_info[resolution[res_id].count] = search_data[res_id];
            resolution[res_id].count++;
        }
        inst_info->target_db_id = res_id;
    }

    for (i = 0; i < inst_info->info->num_searches; i++)
    {
        struct test_db_info *cur_db;
        uint32_t random_num = kaps_random_r(tinfo->seedp);
        uint8_t res_id;
        uint32_t factor = 0;
        struct kaps_parse_record *add = NULL;

        if (i == target_db)
            continue;

        res_id = inst_info->info->desc[i].result_id;
        cur_db = get_test_info_for_xml_db(tinfo, inst_info->info->desc[i].db_info);

        factor = cur_db->nentries;
        kaps_sassert(cur_db != NULL);
        if (tinfo->flag_random)
            cur_db->nsearches++;

        search_data[res_id].db_info = cur_db;

        if (tinfo->flag_random == 0)
        {
            /*
             * for sequential loop: ~99% we target the Hits 
             */
            if (cur_db->clone_parent)
                factor = cur_db->clone_parent->nadds;
            else
                factor = cur_db->nadds;

            if (factor == 0)
                factor = cur_db->nentries;

            add = &cur_db->entries[random_num % factor];
        }
        else
        {
            /*
             * for random loop: will make sure more than 80% we target the Hits 
             */
            {
                add = get_next_entry(cur_db, 1);
                if (add == NULL)
                {
                    /*
                     * No valid entry in database, lookup invalid entry we will expect a miss 
                     */
                    add = get_next_entry(cur_db, 0);
                    if (!tinfo->flag_defer_deletes)
                    {
                        kaps_sassert(add != NULL);
                    }
                    else
                    {
                        if (add == NULL)
                            add = get_next_entry(cur_db, 0xf);  /* select random entry for lookup */
                        kaps_sassert(add != NULL);
                    }
                }
            }
        }

        kaps_sassert(add != NULL);

        search_data[res_id].selected_rec = add;
        search_data[res_id].trie_match = NULL;
        search_data[res_id].indices = NULL;
        search_data[res_id].num_indices = 0;
        gather_search_expected_data(tinfo, inst_info, &search_data[res_id], search_key, master_key_visited_map, 0);

        if (search_data[res_id].trie_match)
        {
            resolution[res_id].search_info[resolution[res_id].count] = search_data[res_id];
            resolution[res_id].count++;
        }
    }

    if (!tinfo->does_xpt_support_search)
    {
        /*
         * black hole mode 
         */
        for (i = 0; i < KAPS_INSTRUCTION_MAX_RESULTS; ++i)
        {
            if (search_data[i].num_indices)
            {
                kaps_entry_free_index_array(search_data[i].db_info->parse_info->db.db, search_data[i].indices);
            }
        }

        return KAPS_OK;
    }

    if (tinfo->searches_in_progress)
    {
        struct kaps_verifier_search_record *search_rec = NULL;
        int32_t max_searches_possible = 1024;

        search_rec = &tinfo->expected_results[tinfo->cur_num_searches % max_searches_possible];
        search_rec->inst_info = inst_info;

        if (tinfo->flag_random == 0)
            search_rec->iteration_number = tinfo->target_db->nsearches;
        else
            search_rec->iteration_number = tinfo->num_iterations;

        kaps_memcpy(search_rec->key, search_key, sizeof(search_key));

        kaps_memcpy(search_rec->exp_results.search_info, search_data, sizeof(search_data));

        tinfo->cur_num_searches++;
        return KAPS_OK;
    }

    if (inst_info->info->master_key)
    {
        struct kaps_parse_key *tmp;
        uint32_t cur_offset = 0;

        /*
         * Honour the overlay fields in the master key 
         */
        for (tmp = inst_info->info->master_key; tmp; tmp = tmp->next)
        {
            if (tmp->overlay_field)
            {
                kaps_memcpy(&search_key[tmp->offset / KAPS_BITS_IN_BYTE], &search_key[cur_offset / KAPS_BITS_IN_BYTE],
                            (tmp->width / KAPS_BITS_IN_BYTE));
                kaps_memmove(&search_key[cur_offset / KAPS_BITS_IN_BYTE],
                             &search_key[(cur_offset / KAPS_BITS_IN_BYTE) + (tmp->width / KAPS_BITS_IN_BYTE)],
                             sizeof(search_key) - ((cur_offset / KAPS_BITS_IN_BYTE) +
                                                   (tmp->width / KAPS_BITS_IN_BYTE)));
                continue;
            }
            cur_offset += tmp->width;
        }
    }

    KAPS_TRY(kaps_instruction_search(inst_info->info->instruction, search_key, cb_addr, &cmp_result));

    hits_in_nX_srch = 0;
    for (i = 0; i < inst_info->info->num_searches; i++)
    {
        uint32_t result_port = inst_info->info->desc[i].result_id;

        if (cmp_result.hit_or_miss[result_port])
        {
            inst_info->dbs_hit[result_port] += 1;
            hits_in_nX_srch += 1;
        }
        else
        {
            inst_info->dbs_mis[result_port] += 1;
        }
    }
    inst_info->total_hits[hits_in_nX_srch] += 1;

    status = compare_entries(tinfo, &cmp_result, search_data, inst_info, search_key, 0);

    if (status != KAPS_OK)
    {

        return status;
    }

    return KAPS_OK;

}

static kaps_status
select_instr_and_perform_search(
    struct xml_test *tinfo)
{
    int32_t index;
    struct test_instr_info *inst_info;

    if (tinfo->flag_random)
    {
        index = kaps_random_r(tinfo->seedp) % tinfo->num_instructions;
        inst_info = &tinfo->instruction_info_array[index];
    }
    else
    {
        inst_info = tinfo->target_inst;
    }

    return perform_search(tinfo, inst_info);
}

/**
 *
 * Pick a random database
*/

static struct test_db_info *
pick_database(
    struct xml_test *tinfo)
{
    int32_t random_no = kaps_random_r(tinfo->seedp) % 10000;
    int32_t index = tinfo->ops_distr[random_no];
    struct test_db_info *db_info = &tinfo->db_info_array[index];

    kaps_sassert(db_info->parse_info->db.db->common_info->dummy_db == 0);

    /*
     * If this is a clone, pick the parent, as we do not
     * yet add entries to the clone
     */
    if (db_info->clone_parent)
        return db_info->clone_parent;
    return db_info;
}

/**
 * Pick a random database with on chip AD
 */
static struct test_db_info *
pick_database_with_ad(
    struct xml_test *tinfo)
{
    int32_t i, random_no = kaps_random_r(tinfo->seedp) % 10000;
    int32_t index = tinfo->ops_distr[random_no];
    struct test_db_info *db_info = &tinfo->db_info_array[index];

    if (db_info->ad_db_info)
    {
        if (db_info->clone_parent)
            return db_info->clone_parent;
        return db_info;
    }

    for (i = index + 1; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];
        if (db_info->ad_db_info)
        {
            if (db_info->clone_parent)
                return db_info->clone_parent;
            return db_info;
        }
    }

    for (i = 0; i < index; i++)
    {
        db_info = &tinfo->db_info_array[i];
        if (db_info->ad_db_info)
        {
            if (db_info->clone_parent)
                return db_info->clone_parent;
            return db_info;
        }
    }

    kaps_sassert(0);
    return NULL;
}

/**
 * get the next random entry that can be added to the database
*/

struct kaps_parse_record *
get_next_entry(
    struct test_db_info *db_info,
    uint8_t visited)
{
    if (visited == 0xf)
    {
        struct kaps_parse_record *add = NULL;
        int32_t index = kaps_random_r(db_info->seedp) % db_info->nentries;

        add = &db_info->entries[index];
        return add;
    }

    if (db_info->indices)
    {
        if (visited == 0)
        {
            int32_t index;

            if (db_info->clone_parent)
                db_info = db_info->clone_parent;

            if (db_info->pivot == 0)
                return NULL;

            index = kaps_random_r(db_info->seedp) % db_info->pivot;

            kaps_sassert(db_info->entries[db_info->indices[index]].visited == 0);
            db_info->index = index;
            return &db_info->entries[db_info->indices[index]];
        }
        else if (visited == 1)
        {
            int32_t index = 0;

            if (db_info->clone_parent)
                db_info = db_info->clone_parent;

            if (db_info->pivot == db_info->nentries)
                return NULL;

            index = kaps_random_r(db_info->seedp) % (db_info->nentries - db_info->pivot) + db_info->pivot;

            if (index == db_info->nentries)
                index--;

            kaps_sassert(db_info->entries[db_info->indices[index]].visited);
            db_info->index = index;
            return &db_info->entries[db_info->indices[index]];
        }
        else
        {
            kaps_sassert(0);
        }
    }
    else
    {
        int32_t index = kaps_random_r(db_info->seedp) % db_info->nentries;
        struct kaps_parse_record *add = NULL;

        if (db_info->entries[index].visited != visited || db_info->entries[index].pending_del)
        {
            int32_t j;

            for (j = index; j < db_info->nentries; j++)
            {
                if (db_info->entries[j].visited == visited && !db_info->entries[index].pending_del)
                {
                    add = &db_info->entries[j];
                    break;
                }
            }
            if (!add)
            {
                for (j = 0; j < index; j++)
                {
                    if (db_info->entries[j].visited == visited && !db_info->entries[index].pending_del)
                    {
                        add = &db_info->entries[j];
                        break;
                    }
                }
            }
        }
        else
        {
            add = &db_info->entries[index];
        }
        return add;
    }
    return NULL;
}

/**
 * get the ad_info
 */

static struct kaps_parse_ad_info *
get_ad_for_db(
    struct xml_test *tinfo,
    struct test_db_info *db_info)
{

    if (db_info->ad_db_info->next == NULL)
    {
        return db_info->ad_db_info;
    }
    else
    {
        if (db_info->ad_db_info->ad_percent != -1)
        {
            struct kaps_parse_ad_info *ad_info = db_info->ad_db_info;
            int32_t random_no = 0;

            random_no = kaps_random_r(tinfo->seedp) % 100;
            for (; ad_info; ad_info = ad_info->next)
            {
                if (random_no >= ad_info->ad_percent_index_start && random_no <= ad_info->ad_percent_index_end)
                    break;
            }
            kaps_sassert(ad_info);
            return ad_info;
        }
        else
        {
            uint16_t count = 0;
            uint16_t randno = kaps_random_r(db_info->seedp) % db_info->parse_info->nad_dbs;
            struct kaps_parse_ad_info *tmp = db_info->ad_db_info;

            while (tmp)
            {
                if (count == randno)
                    break;
                tmp = tmp->next;
                count++;
            }

            kaps_sassert(tmp);

            if (tinfo->flag_random == 0)
            {
                if (tmp->ad_db->ad_list.count >= tmp->capacity && db_info->ad_db_info->next)
                {
                    struct kaps_parse_ad_info *tmp1 = db_info->ad_db_info;

                    while (tmp1 && tmp1->ad_db->ad_list.count >= tmp1->capacity)
                    {
                        tmp1 = tmp1->next;
                    }
                    if (tmp1)
                        return tmp1;
                }
            }
            return tmp;
        }
    }
}

kaps_status
add_ad_to_entry(
    struct xml_test * tinfo,
    struct test_db_info * db_info,
    struct kaps_parse_record * add,
    struct kaps_entry * e)
{
    struct kaps_verifier_ad_info *ad_info = NULL;
    struct kaps_parse_ad_info *ad_db_info;
    uint32_t ad_capacity = 0;
    struct kaps_db *db = db_info->parse_info->db.db;
    kaps_status status = KAPS_OK;

    ad_db_info = db_info->ad_db_info;
    while (ad_db_info)
    {
        ad_capacity += ad_db_info->capacity;
        ad_db_info = ad_db_info->next;
    }

    ad_db_info = get_ad_for_db(tinfo, db_info);

    if (ad_db_info->ad_db->user_width_1 == 0)
    {
        ad_info = db_info->zero_size_ad_info;
        KAPS_TRY(kaps_entry_add_ad(db, e, ad_info->ad));
        ad_info->refcount++;
    }
    else if (db_info->parse_info->db.db->common_info->capacity > ad_capacity)
    {
        uint32_t ratio = db_info->parse_info->db.db->common_info->capacity / ad_db_info->capacity;
        uint32_t refcount = 0;
        struct kaps_parse_record *add_ad = NULL;

        if (ratio == 0)
            ratio = 1;

        if (tinfo->flag_random)
        {
            uint32_t rand = kaps_random_r(tinfo->seedp) % (ratio + 1);

            if (!rand)
            {
                add_ad = get_next_entry(db_info, 1);
            }
        }
        else
        {
            add_ad = &db_info->entries[db_info->nadds % ad_capacity];
        }

        if (add_ad && add_ad->ad_data)
        {
            refcount = ((struct kaps_verifier_ad_info *) (add_ad->ad_data))->refcount;
            if (refcount < ratio)
            {
                ad_info = (struct kaps_verifier_ad_info *) add_ad->ad_data;
                KAPS_TRY(kaps_entry_add_ad(db, e, ad_info->ad));
                ad_info->refcount++;
            }
        }
    }

    if (ad_info == NULL)
    {
        {
            ad_info = alloc_ad(tinfo, add, ad_db_info->ad_width_8, ad_db_info->ad_db->user_width_1);
            if (ad_info == NULL)
                return KAPS_OUT_OF_MEMORY;

            status = kaps_ad_db_add_entry(ad_db_info->ad_db, ad_info->ad_data, &ad_info->ad);

            if (status == KAPS_OUT_OF_UDA)
            {
                struct kaps_db_stats stats;

                if (tinfo->flag_random == 0)
                    tinfo->n_status = status;
                status = kaps_db_stats(db_info->parse_info->db.db, &stats);
                if (status != KAPS_OK)
                {
                    free_ad(tinfo, db_info, ad_info);
                    return status;
                }

                if (stats.num_entries < db_info->least_full_point)
                {
                    db_info->least_full_point = stats.num_entries;
                }
                db_info->db_full++;
                db_full_handler(tinfo, db_info, &stats);

                status = kaps_db_delete_entry(db, e);
                if (status != KAPS_OK)
                {
                    free_ad(tinfo, db_info, ad_info);
                    return status;
                }
                add->e = NULL;
                db_info->is_brimmed = 1;
                free_ad(tinfo, db_info, ad_info);
                return KAPS_OUT_OF_UDA;
            }
        }

        if (status == KAPS_OK)
            status = kaps_entry_add_ad(db, e, ad_info->ad);

        if (status != KAPS_OK)
        {
            free_ad(tinfo, db_info, ad_info);
            return status;
        }
        ad_info->parse_ad_info = ad_db_info;
        ad_info->refcount++;
    }
    add->ad_data = ad_info;
    return KAPS_OK;
}

static kaps_status
add_hb_to_entry(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_parse_record *add,
    struct kaps_entry *e)
{
    struct kaps_hb *hb_handle = NULL;
    uint32_t ratio = db_info->parse_info->db.db->common_info->capacity / db_info->parse_info->hb_db_info->capacity;
    uint32_t rand = kaps_random_r(tinfo->seedp) % (ratio + 1);
    struct kaps_db *db = db_info->parse_info->db.db;
    kaps_status status = KAPS_OK;

    if (tinfo->flag_random == 0)
    {
        if (db_info->parse_info->hb_db_info->hb_db->hb_list.count >= db_info->parse_info->hb_db_info->hb_db->capacity)
            return KAPS_OK;
    }

    if (!rand)
    {
        KAPS_TRY(kaps_hb_db_add_entry(db_info->parse_info->hb_db_info->hb_db, &hb_handle));
        status = kaps_entry_add_hb(db, e, hb_handle);

        if (status == KAPS_OUT_OF_UDA)
        {
            struct kaps_db_stats stats;
            struct kaps_verifier_ad_info *ad_info = add->ad_data;

            if (tinfo->flag_random == 0)
                tinfo->n_status = status;
            status = kaps_db_stats(db_info->parse_info->db.db, &stats);

            if (status != KAPS_OK)
            {
                KAPS_TRY(kaps_hb_db_delete_entry(db_info->parse_info->hb_db_info->hb_db, hb_handle));
                if (ad_info->refcount > 0)
                    ad_info->refcount--;
                if (!ad_info->refcount)
                {
                    if (ad_info != db_info->zero_size_ad_info)
                    {
                        KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                        free_ad(tinfo, db_info, ad_info);
                    }
                }
                add->ad_data = NULL;

                free_ad(tinfo, db_info, ad_info);
                return status;
            }

            if (stats.num_entries < db_info->least_full_point)
            {
                db_info->least_full_point = stats.num_entries;
            }
            db_info->db_full++;

            db_full_handler(tinfo, db_info, &stats);

            status = kaps_db_delete_entry(db, e);
            if (status != KAPS_OK)
            {
                KAPS_TRY(kaps_hb_db_delete_entry(db_info->parse_info->hb_db_info->hb_db, hb_handle));
                if (ad_info->refcount > 0)
                    ad_info->refcount--;
                if (!ad_info->refcount)
                {
                    if (ad_info != db_info->zero_size_ad_info)
                    {
                        KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                        free_ad(tinfo, db_info, ad_info);
                    }
                }
                add->ad_data = NULL;
                return status;
            }
            add->e = NULL;
            db_info->is_brimmed = 1;
            free_ad(tinfo, db_info, ad_info);
            return KAPS_OUT_OF_UDA;
        }
        else if (status != KAPS_OK)
        {
            return status;
        }

        add->hb_e = hb_handle;
    }
    return KAPS_OK;
}

/**
 *
 * Randomly select a database from the set of databases we have
 * and add a single entry to it.
*/

static kaps_status
add_entry_to_database_internal(
    struct xml_test *tinfo,
    struct test_db_info *db_info)
{
    struct kaps_parse_record *add;
    struct kaps_entry *e = NULL;
    kaps_status status = KAPS_OK;
    struct kaps_db *db, *db1 = NULL;
    uint32_t index = 0;
    uint32_t entry_location = 0;

    if (tinfo->flag_random && !db_info->parse_info->incremental && !tinfo->adds_in_progress)
    {
        return KAPS_OK;
    }

    if (tinfo->flag_random == 0 || tinfo->adds_in_progress || tinfo->force_seq_ins)
    {
        add = &db_info->entries[db_info->nadds];
        index = db_info->nadds;
        entry_location = index + 1;
    }
    else
    {
        add = get_next_entry(db_info, 0);
        index = db_info->index;
        if (db_info->indices)
        {
            entry_location = db_info->indices[index] + 1;
        }
    }

    if (!add)
    {
        /*
         * added all entries, simply return
         */
        return KAPS_OK;
    }

    if (add->in_search_loop)
        return KAPS_OK;

    kaps_sassert(add->next == NULL);
    db = db_info->parse_info->db.db;

    if (tinfo->num_databases > 1)
        db1 = tinfo->db_info_array[1].parse_info->db.db;

    if (entry_location)
    {
        struct kaps_db *tmp = db;
        if (tmp->parent)
            tmp = tmp->parent;

        if (tmp->has_tables
            || tinfo->flag_defer_deletes
            || (!(tinfo->flag_random == 0 || tinfo->flag_random == 1))
            || !db_info->parse_info->incremental || db_info->end_index_default_entries)
        {
            entry_location = 0;
        }

        if (add->cust_add_or_del)
            entry_location = 0;
    }

    if (db_info->parse_info->type == KAPS_DB_LPM)
    {
        if (entry_location)
        {
            status = kaps_db_add_prefix_with_index(db, add->data, add->length, entry_location);
            kaps_sassert(status != KAPS_DB_WB_LOCATION_BUSY);
            if (status == KAPS_OK)
                e = (struct kaps_entry *) ((uintptr_t) entry_location);
        }
        else if (add->cust_add_or_del)
        {
            struct kaps_db *tmp_db = db;

            struct test_db_info *tmp_db_info = db_info;
            if (add->cust_db_id == 1)
                tmp_db_info = &tinfo->db_info_array[1];

            if (add->cust_db_id == 1)
                tmp_db = db1;

            if (add->cust_add_or_del == KAPS_CUSTOM_OPERATION_ADD)
            {
                status = kaps_db_add_prefix(tmp_db, add->data, add->length, &e);
            }
            else if (add->cust_add_or_del == KAPS_CUSTOM_OPERATION_DEL)
            {
                KAPS_TRY(kaps_db_get_prefix_handle(tmp_db, add->data, add->length, &e));
                tmp_db_info->exp_delete = tmp_db_info->e_to_parse_record[(uintptr_t) e];
                KAPS_TRY(delete_entry_from_database(tinfo, tmp_db_info));
                tmp_db_info->exp_delete = NULL;
                db_info->nadds++;       /* nadds should be incremented even though it is a delete */
                return KAPS_OK;
            }
            else if (add->cust_add_or_del == KAPS_CUSTOM_OPERATION_UPD)
            {
                struct kaps_parse_record *cur_parse_record;
                struct kaps_verifier_ad_info *ad_info;

                KAPS_TRY(kaps_db_get_prefix_handle(tmp_db, add->data, add->length, &e));

                cur_parse_record = tmp_db_info->e_to_parse_record[(uintptr_t) e];

                ad_info = (struct kaps_verifier_ad_info *) cur_parse_record->ad_data;

                if (ad_info->parse_ad_info->ad_db->user_width_1 == 0)
                    return KAPS_OK;

                kaps_memcpy(ad_info->ad_data, add->user_ad_data, ad_info->parse_ad_info->ad_width_8);

                kaps_ad_db_update_entry(ad_info->parse_ad_info->ad_db, ad_info->ad, ad_info->ad_data);

                db_info->nadds++;
                return KAPS_OK;
            }
        }
        else
        {
            status = kaps_db_add_prefix(db, add->data, add->length, &e);
        }

        /*
         * Both LPM and LPM mapped to ACL flag immediate duplicate on addition of entry 
         */
        if (status == KAPS_DUPLICATE)
        {
            db_info->nduplicates++;
            db_info->nadds++;
            return KAPS_OK;
        }

    }

    if (status == KAPS_DB_FULL)
        return KAPS_OK;

    if (status != KAPS_OK)
    {
        if (e != NULL)
            KAPS_TRY(kaps_db_delete_entry(db, e));
        return status;
    }

    if (db_info->ad_db_info)
    {
        struct test_db_info *tmp_db_info = db_info;
        if (add->cust_db_id == 1)
            tmp_db_info = &tinfo->db_info_array[1];

        status = add_ad_to_entry(tinfo, tmp_db_info, add, e);

        if (status == KAPS_OUT_OF_UDA)
        {
            if (tinfo->flag_random == 2)
                return KAPS_OUT_OF_UDA;
            return KAPS_OK;
        }
        else if (status != KAPS_OK)
            return status;
    }

    /*
     * ADD HB 
     */
    if (db_info->parse_info->hb_db_info)
    {
        status = add_hb_to_entry(tinfo, db_info, add, e);
        if (status == KAPS_OUT_OF_UDA)
        {
            if (tinfo->flag_random == 2)
                return KAPS_OUT_OF_UDA;
            return KAPS_OK;
        }
        else if (status != KAPS_OK)
            return status;
    }

    if (tinfo->maintain_e_to_parse_record_table)
    {
        struct test_db_info *tmp_db_info = db_info;
        if (add->cust_db_id == 1)
            tmp_db_info = &tinfo->db_info_array[1];

        if ((uintptr_t) e >= tmp_db_info->e_to_parse_record_size)
            KAPS_TRY(realloc_e_parse_record_array(tmp_db_info));

        tmp_db_info->e_to_parse_record[(uintptr_t) e] = add;
    }

    if ((!tinfo->flag_random || tinfo->flag_random == 8) && !tinfo->adds_in_progress)
    {
        /*
         * LPM is incremental in sequential/dynamic loop, else in random loop its batch 
         */
        struct kaps_db *tmp_db = db;

        if (add->cust_db_id == 1)
            tmp_db = db1;

        add->e = e;

        status = kaps_db_install(tmp_db);
        if (tinfo->flag_crash_recovery)
        {
            tinfo->can_crash = 0;
            KAPS_TRY(kaps_device_end_transaction(tinfo->device));
            tinfo->can_crash = 1;
            KAPS_TRY(kaps_device_start_transaction(tinfo->device));
        }
        kaps_sassert(status != KAPS_DUPLICATE);
        if (status == KAPS_OUT_OF_DBA
            || status == KAPS_OUT_OF_UDA
            || status == KAPS_OUT_OF_INDEX
            || status == KAPS_OUT_OF_UIT || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_AD)
        {

            /*
             * Database is full, not really an error
             */
            struct kaps_db_stats stats;

            if (tinfo->flag_random == 0)
                tinfo->n_status = status;

            KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
            if (stats.num_entries < db_info->least_full_point)
            {
                db_info->least_full_point = stats.num_entries;
            }
            db_info->db_full++;
            db_full_handler(tinfo, db_info, &stats);

            if (status == KAPS_OUT_OF_INDEX)
            {
                db_info->ix_full++;
            }
            db_info->is_brimmed = 1;

            KAPS_TRY(kaps_db_delete_entry(db, e));

            if (db_info->ad_db_info)
            {
                struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) add->ad_data;
                if (ad_info->refcount > 0)
                    ad_info->refcount--;
                if (!ad_info->refcount)
                {
                    if (ad_info != db_info->zero_size_ad_info)
                    {
                        KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                        free_ad(tinfo, db_info, ad_info);
                    }
                }
                add->ad_data = NULL;
            }

            if (db_info->parse_info->hb_db_info && add->hb_e)
            {
                KAPS_TRY(kaps_hb_db_delete_entry(db_info->parse_info->hb_db_info->hb_db, add->hb_e));
                add->hb_e = NULL;
            }

            add->e = NULL;

            if (tinfo->flag_random == 2)
                return status;

            return KAPS_OK;
        }

        db_info->ninstalls++;
        /*
         * Only for NetRoute, mark the entry immediately as
         * active. This breaks, coherency, however, removes
         * the dependency on callbacks.
         */
        add->status = FAST_TRIE_ACTIVE;
    }

    if (status != KAPS_OK)
        return status;

    add->e = e;
    add->visited = 1;
    db_info->nadds++;

    if (tinfo->flag_use_pivot)
    {
        uint8_t need_swap = 1;

        db_info->index = index;
        if (tinfo->flag_random == 0)
        {
            if (db_info->nadds > db_info->pivot)
            {
                db_info->indices[db_info->pivot - 1] = db_info->indices[db_info->index];
                if (db_info->pivot)
                    db_info->pivot--;
                need_swap = 0;
            }
        }

        if (need_swap)
        {
            kaps_sassert(db_info->indices != NULL);
            SWAP(db_info->indices[db_info->index], db_info->indices[db_info->pivot - 1]);
            if (db_info->pivot)
                db_info->pivot--;
        }
    }

    /*
     * Chain the entry as pending an add. This will allow
     * us to mark the entry as active when we invoke the
     * install call. This eliminates the need for a callback
     */
    if ((tinfo->flag_random != 0 && tinfo->flag_random != 8) || tinfo->adds_in_progress)
    {
        kaps_sassert(add->next == NULL);
        add->next = db_info->pending;
        db_info->pending = add;
        add->status = FAST_TRIE_PENDING;
    }

    return KAPS_OK;
}

static kaps_status
add_entry_to_database(
    struct xml_test *tinfo,
    struct test_db_info *db_info)
{
    kaps_status status;

    if (tinfo->flag_coherency)
        tinfo->looking_for_pio = 0;

    status = add_entry_to_database_internal(tinfo, db_info);

    return status;
}

static kaps_status
select_db_and_add_entry_to_itself(
    struct xml_test *tinfo)
{
    struct test_db_info *db_info = NULL;

    if (tinfo->target_db)
        db_info = tinfo->target_db;
    else
        db_info = pick_database(tinfo);

    return add_entry_to_database(tinfo, db_info);
}

/**
 * Randomly select a database from the set of databases we have
 * and delete an entry from it
 */
static kaps_status
delete_entry_from_database(
    struct xml_test *tinfo,
    struct test_db_info *db_info)
{
    struct kaps_parse_record *add = NULL;
    struct kaps_db *db = NULL;

    if (db_info->is_brimmed && tinfo->flag_defer_deletes && db_info->pending)
    {
        add = db_info->pending; /* heavy delete have pending_list install lead to DB_FULL so we pick up, pending list
                                 * to immediate delete and later random active entry to make have space */
    }
    else if (tinfo->flag_random)
    {
        add = get_next_entry(db_info, 1);
    }
    else if (db_info->exp_delete)
    {
        add = db_info->exp_delete;
    }
    else
    {
        add = &db_info->entries[db_info->ndeletes];
        /*
         * For duplicate entries 
         */
        if (add->visited == 0)
        {
            db_info->ndeletes++;
            return KAPS_OK;
        }
    }

    if (!add)
    {
        /*
         * No entries to delete, simply return
         */
        return KAPS_OK;
    }

    if (add->in_search_loop)
    {
        return KAPS_OK;
    }

    db = db_info->parse_info->db.db;

    /*
     * check the incoming entry already present in the delete_pending 
     */
    if (add->pending_del)
    {
        return KAPS_OK;
    }
    tinfo->can_crash = 0;
    tinfo->last_delete_rec = add;
    tinfo->xpt_db = db_info;

    if (tinfo->flag_coherency)
        add->visited = 2;

    tinfo->can_crash = 1;
    KAPS_TRY(kaps_db_delete_entry(db, add->e));
    add->counter_copy = 0;
    tinfo->can_crash = 0;

    if (tinfo->maintain_e_to_parse_record_table)
    {
        db_info->e_to_parse_record[(uintptr_t) add->e] = NULL;
    }

    db_info->ndeletes++;

    if (tinfo->flag_defer_deletes && add->status != FAST_TRIE_PENDING)
    {
        /*
         * 1. Add this entry to new list say delete list like pending list
         * 2. What if the entry is pending entry ? remove from add pending list and add to delete pending list
         * 3. if active make sure to add for pending list
         */

        kaps_sassert(add->next == NULL);
        add->next = db_info->delete_pending;
        db_info->delete_pending = add;
        add->pending_del = 1;
    }
    else
    {
        KAPS_TRY(clean_up_delete_entry(tinfo, db_info, add));
    }

    return KAPS_OK;
}

kaps_status
clean_up_delete_entry(
    struct xml_test * tinfo,
    struct test_db_info * db_info,
    struct kaps_parse_record * add)
{
    add->e = NULL;
    add->visited = 0;

    if (tinfo->flag_use_pivot)
    {
        SWAP(db_info->indices[db_info->index], db_info->indices[db_info->pivot]) db_info->pivot++;
        if (db_info->pivot > db_info->nentries)
            kaps_sassert(0);
    }

    /*
     * Delete AD Entry if refcount is zero 
     */
    if (add->ad_data != NULL)
    {
        struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) add->ad_data;

        if (ad_info->refcount > 0)
            ad_info->refcount--;

        if (!ad_info->refcount)
        {
            if (ad_info != db_info->zero_size_ad_info)
            {
                KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                free_ad(tinfo, db_info, ad_info);
            }
        }
        add->ad_data = NULL;
    }

    /*
     * Delete HB Entry 
     */
    if (db_info->parse_info->hb_db_info && add->hb_e)
    {
        KAPS_TRY(kaps_hb_db_delete_entry(db_info->parse_info->hb_db_info->hb_db, add->hb_e));
    }

    if (add->status == FAST_TRIE_PENDING)
    {
        /*
         * This entry is sitting in the pending list of the database
         * remove it from there also.
         */
        struct kaps_parse_record *tmp, *prev = NULL;

        for (tmp = db_info->pending; tmp; tmp = tmp->next)
        {
            if (tmp == add)
            {
                if (prev == NULL)
                    db_info->pending = tmp->next;
                else
                    prev->next = tmp->next;
                break;
            }
            prev = tmp;
        }
        add->next = NULL;
    }

    add->status = FAST_TRIE_INACTIVE;
    add->hit = 0;
    add->counter = 0;
    add->hb_e = NULL;
    tinfo->last_delete_rec = NULL;
    tinfo->xpt_db = NULL;

    tinfo->can_crash = 1;

    /*
     * If database population is less than 90% of least full point database is no longer considered to be brimmed. 
     */
    if (tinfo->flag_random == 0)
    {
        db_info->is_brimmed = 0;
    }
    else if (db_info->is_brimmed)
    {
        if (db_info->ndeletes <= db_info->nadds)
        {
            if ((db_info->nadds - db_info->ndeletes) < ((9 * db_info->least_full_point) / 10))
            {
                db_info->is_brimmed = 0;
            }
        }
    }

    return KAPS_OK;
}

static kaps_status
select_db_and_delete_entry_from_itself(
    struct xml_test *tinfo)
{
    struct test_db_info *db_info = NULL;

    if (tinfo->flag_random)
    {
        db_info = pick_database(tinfo);
    }
    else
    {
        db_info = tinfo->target_db;
    }
    return delete_entry_from_database(tinfo, db_info);
}

/**
 * Randomly select a database from the set of databases we have
 * and perform heavy searches and deletes
 */
static kaps_status
perform_heavy_search_and_delete(
    struct xml_test *tinfo)
{
    kaps_status status;

    struct test_db_info *db_info;
    struct kaps_db_stats stats;
    uint32_t iter, delete_iterations, search_iterations;
    int32_t i, target_db = -1;
    struct test_instr_info *inst_info;

    db_info = pick_database(tinfo);

    /*
     * In case of heavy search and delete 30 -100 % of entries present in the target db will be searched & deleted 
     */
    if ((db_info->is_brimmed == 0) && (!tinfo->issu_in_progress))
        return KAPS_OK;

    KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
    delete_iterations = (kaps_random_r(tinfo->seedp) % 8) + 3;
    delete_iterations *= (stats.num_entries / 10);

    search_iterations = (kaps_random_r(tinfo->seedp) % 8) + 3;
    search_iterations *= (stats.num_entries / 10);

    /*
     * If heavy search is triggered from ISSU ops dont delete entries and search all 
     */
    if (tinfo->issu_in_progress)
    {
        search_iterations = stats.num_entries;
        delete_iterations = 0;
    }
    /*
     * Find an instruction which searches the target db 
     */

    for (iter = 0; iter < tinfo->num_instructions; iter++)
    {
        inst_info = &tinfo->instruction_info_array[iter];
        for (i = 0; i < inst_info->info->num_searches; i++)
        {
            if (db_info == get_test_info_for_xml_db(tinfo, inst_info->info->desc[i].db_info))
            {
                target_db = i;
                break;
            }
        }
        if (target_db != -1)
            break;
    }

    /*
     * No instruction found with the target db, don't perform searches 
     */
    if (target_db == -1)
        return KAPS_OK;

    db_info->n_heavysearches++;
    tinfo->target_db = db_info;

    for (iter = 0; iter < search_iterations; iter++)
    {
        KAPS_STRY(perform_search(tinfo, inst_info));
    }

    tinfo->target_db = NULL;

    db_info->n_heavydeletes++;
    for (iter = 0; iter < delete_iterations; iter++)
    {
        KAPS_STRY(delete_entry_from_database(tinfo, db_info));
    }

    if (tinfo->flag_defer_deletes)
    {
        status = perform_install_on_db(tinfo, db_info);
        if (status == KAPS_OUT_OF_DBA || status == KAPS_OUT_OF_UDA || status == KAPS_OUT_OF_INDEX
            || status == KAPS_OUT_OF_UIT || status == KAPS_OUT_OF_AD || status == KAPS_EXHAUSTED_PCM_RESOURCE)
        {
            return KAPS_OK;
        }
        else
        {
            return status;
        }
    }

    return KAPS_OK;
}

kaps_status
perform_install_on_db(
    struct xml_test * tinfo,
    struct test_db_info * db_info)
{
    kaps_status status;

    status = kaps_db_install(db_info->parse_info->db.db);
    if (tinfo->flag_crash_recovery)
    {
        tinfo->can_crash = 0;
        KAPS_TRY(kaps_device_end_transaction(tinfo->device));
        tinfo->can_crash = 1;
        KAPS_TRY(kaps_device_start_transaction(tinfo->device));
    }
    if (status == KAPS_OUT_OF_DBA
        || status == KAPS_OUT_OF_UDA
        || status == KAPS_OUT_OF_INDEX
        || status == KAPS_OUT_OF_UIT || status == KAPS_OUT_OF_AD || status == KAPS_EXHAUSTED_PCM_RESOURCE)
    {
        /*
         * Database is full, not really an error.
         * Record this capacity point
         */
        struct kaps_db_stats stats;
        KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
        if (stats.num_entries < db_info->least_full_point)
        {
            db_info->least_full_point = stats.num_entries;
        }
        db_info->db_full++;
        db_full_handler(tinfo, db_info, &stats);

        if (status == KAPS_OUT_OF_INDEX)
        {
            db_info->ix_full++;
        }
        db_info->is_brimmed = 1;

        return KAPS_OK;
    }

    if (status == KAPS_OK)
    {
        struct xml_parse_info *tab;

        db_info->ninstalls++;

        /*
         * Install, installs entries across all the tables
         * in the database. The loop below in turn activates
         * all the entries that are part of the database.
         */

        if (db_info->container_db)
            tab = db_info->container_db;
        else
            tab = db_info->parse_info;

        if (!tab->db_key)
            tab = tab->next_tab;

        for (; tab; tab = tab->next_tab)
        {
            struct kaps_parse_record *add;
            struct test_db_info *tmp = get_test_info_for_xml_db(tinfo, tab);

            add = tmp->pending;
            while (add)
            {
                struct kaps_parse_record *next = add->next;

                add->next = NULL;
                kaps_sassert(add->status == FAST_TRIE_PENDING);
                add->status = FAST_TRIE_ACTIVE;
                add = next;
            }
            tmp->pending = NULL;

            add = tmp->delete_pending;
            while (add)
            {
                struct kaps_parse_record *next = add->next;

                add->next = NULL;
                kaps_sassert(add->status == FAST_TRIE_ACTIVE || add->status == FAST_TRIE_PENDING);
                KAPS_TRY(clean_up_delete_entry(tinfo, tmp, add));
                add->pending_del = 0;
                add = next;
            }
            tmp->delete_pending = NULL;
        }
    }

    return status;
}

/**
 * Randomly select a database from the set of databases we have
 * and invoke install on it
*/

static kaps_status
perform_install(
    struct xml_test *tinfo)
{
    kaps_status status;
    struct test_db_info *db_info;

    if (tinfo->target_db)
        db_info = tinfo->target_db;
    else
        db_info = pick_database(tinfo);

    if (db_info->is_search_loop)
        return KAPS_OK;

    if (!db_info->parse_info->incremental && !tinfo->adds_in_progress)
        kaps_sassert(db_info->pending == NULL);

    status = kaps_db_install(db_info->parse_info->db.db);
    if (tinfo->flag_crash_recovery)
    {
        tinfo->can_crash = 0;
        KAPS_TRY(kaps_device_end_transaction(tinfo->device));
        tinfo->can_crash = 1;
        KAPS_TRY(kaps_device_start_transaction(tinfo->device));
    }
    if (status == KAPS_OUT_OF_DBA
        || status == KAPS_OUT_OF_UDA
        || status == KAPS_OUT_OF_UIT
        || status == KAPS_OUT_OF_INDEX || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_AD)
    {
        /*
         * Database is full, not really an error.
         * Record this capacity point
         */
        struct kaps_db_stats stats;

        KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
        if (stats.num_entries < db_info->least_full_point)
        {
            db_info->least_full_point = stats.num_entries;
        }
        db_info->db_full++;
        db_full_handler(tinfo, db_info, &stats);

        if (status == KAPS_OUT_OF_INDEX)
        {
            db_info->ix_full++;
        }
        db_info->is_brimmed = 1;

        return KAPS_OK;
    }

    if (status == KAPS_OK)
    {
        struct xml_parse_info *tab;

        tinfo->can_crash = 0;
        db_info->ninstalls++;

        /*
         * Install, installs entries across all the tables
         * in the database. The loop below in turn activates
         * all the entries that are part of the database.
         */

        if (db_info->container_db)
            tab = db_info->container_db;
        else
            tab = db_info->parse_info;

        for (; tab; tab = tab->next_tab)
        {
            struct kaps_parse_record *add;
            struct test_db_info *tmp = get_test_info_for_xml_db(tinfo, tab);

            add = tmp->pending;
            while (add)
            {
                struct kaps_parse_record *next = add->next;

                add->next = NULL;
                kaps_sassert(add->status == FAST_TRIE_PENDING);
                add->status = FAST_TRIE_ACTIVE;
                add = next;
            }
            tmp->pending = NULL;

            add = tmp->delete_pending;
            while (add)
            {
                struct kaps_parse_record *next = add->next;

                add->next = NULL;
                kaps_sassert(add->status == FAST_TRIE_ACTIVE || add->status == FAST_TRIE_PENDING);
                KAPS_TRY(clean_up_delete_entry(tinfo, tmp, add));
                add->pending_del = 0;
                add = next;
            }
            tmp->delete_pending = NULL;
        }
        tinfo->can_crash = 1;
    }
    return status;
}

/**
 * Randomly pick a database from the set of databases we have
 * and iterate entries in it
 */

static kaps_status
perform_entry_iter(
    struct xml_test *tinfo)
{
    kaps_status status = KAPS_OK;
    struct test_db_info *db_info;
    struct kaps_entry_iter *iter;
    struct kaps_db *db;
    int32_t count = 0, max;
    struct kaps_db_stats stats;

    db_info = pick_database(tinfo);

    db = db_info->parse_info->db.db;
    KAPS_STRY(kaps_db_entry_iter_init(db, &iter));
    KAPS_TRY(kaps_db_stats(db, &stats));

    /*
     * Iterate up to 10% of the entries, as iterating millions
     * of entries like netroute case can be very expensive.
     */

    max = stats.num_entries / 10;
    if (max > 10000)
        max = 10000;

    do
    {
        struct kaps_entry *e;

        status = kaps_db_entry_iter_next(db, iter, &e);
        if (status != KAPS_OK)
            goto iter_error;
        if (e == NULL)
            break;
        
        count++;
        if (count > max)
            break;
    }
    while (1);

    KAPS_TRY(kaps_db_entry_iter_destroy(db, iter));
    db_info->niter++;
    return KAPS_OK;

iter_error:
    KAPS_TRY(kaps_db_entry_iter_destroy(db, iter));
    return status;
}

static kaps_status
validate_hb_entries_jr1(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    uint32_t get_aged_entries)
{
    uint32_t i;
    uint32_t num_exp_entries = 0;
    struct kaps_device *device = tinfo->device;
    struct kaps_aged_entry_iter *iter;
    struct kaps_entry *entry;
    struct kaps_hb *hb;
    uint32_t idle_count;
    uint32_t bit_value;

    if (db_info->parse_info->hb_db_info == NULL)
        return KAPS_OK;

    db_info->ncalls_timer++;
    KAPS_TRY(kaps_hb_db_timer(db_info->parse_info->hb_db_info->hb_db));

    for (i = 0; i < db_info->nentries; i++)
    {
        struct kaps_parse_record *rec = &db_info->entries[i];

        if (rec->visited == 0)
            continue;

        if (rec->hb_e == NULL)
        {
            rec->hit = 0;
            rec->counter = 0;
            continue;
        }

        if (rec->hit == 0)
        {
            /*
             * The entry was not searched. So increment the idle count
             */
            rec->counter++;
        }
        else
        {
            /*
             * If the entry has been searched and it has a hit bit, make sure num idles is 0
             */
            if (rec->hb_e)
            {
                /*
                 * Qumran does not have IPT (RPB) hit bits in the hardware. So we can't do a strict checking of the hit 
                 * bits since Lmpsofar hits will not show up in the hardware
                 */
                if (device->type == KAPS_DEVICE_KAPS && device->id != KAPS_QUMRAN_DEVICE_ID)
                {
                    kaps_hb_entry_get_idle_count(db_info->parse_info->hb_db_info->hb_db, rec->hb_e, &idle_count);

                    kaps_sassert(idle_count == 0);
                }
            }
            rec->hit = 0;
            rec->counter = 0;
        }

        if (rec->counter >= db_info->parse_info->hb_db_info->age)
        {
            num_exp_entries++;
        }
    }

    if (get_aged_entries)
    {
        struct kaps_entry *cp_entries[10000] = { 0 };
        uint32_t nentries = 0;

        KAPS_STRY(kaps_db_get_aged_entries(db_info->parse_info->hb_db_info->hb_db, 10000, &nentries, cp_entries));
    }

    kaps_hb_db_aged_entry_iter_init(db_info->parse_info->hb_db_info->hb_db, &iter);

    do
    {
        KAPS_STRY(kaps_hb_db_aged_entry_iter_next(db_info->parse_info->hb_db_info->hb_db, iter, &entry));

        if (entry == NULL)
            break;

        KAPS_STRY(kaps_entry_get_hb(db_info->parse_info->db.db, entry, &hb));

        /*
         * The idle count of an aged entry should be greater than 0
         */
        KAPS_STRY(kaps_hb_entry_get_idle_count(db_info->parse_info->hb_db_info->hb_db, hb, &idle_count));
        kaps_sassert(idle_count >= db_info->parse_info->hb_db_info->age);

        /*
         * Explicitly set the idle count of the entry as 0
         */
        KAPS_STRY(kaps_hb_entry_set_idle_count(db_info->parse_info->hb_db_info->hb_db, hb, 0));

        /*
         * When we read the idle count now, it should be 0
         */
        KAPS_STRY(kaps_hb_entry_get_idle_count(db_info->parse_info->hb_db_info->hb_db, hb, &idle_count));
        kaps_sassert(idle_count == 0);

        KAPS_STRY(kaps_hb_entry_get_bit_value(db_info->parse_info->hb_db_info->hb_db, hb, &bit_value, 1));
        if (tinfo->device->xpt)
        {
            /*
             * Check the bit value only if it is not blackhole mode
             */
            kaps_sassert(bit_value == 0);
        }
    }
    while (1);

    kaps_hb_db_aged_entry_iter_destroy(db_info->parse_info->hb_db_info->hb_db, iter);

    return KAPS_OK;
}

static kaps_status
validate_hb_entries_no_hb_machine(
    struct xml_test *tinfo,
    struct test_db_info *db_info)
{
    struct kaps_entry_iter *iter;
    struct kaps_entry *entry;
    struct kaps_hb *hb;
    uint32_t bit_value;
    uint32_t loop_cnt;

    if (db_info->parse_info->hb_db_info == NULL)
        return KAPS_OK;

    db_info->ncalls_timer++;

    kaps_db_entry_iter_init(db_info->parse_info->db.db, &iter);

    loop_cnt = 0;

    do
    {
        loop_cnt++;

        KAPS_STRY(kaps_db_entry_iter_next(db_info->parse_info->db.db, iter, &entry));

        if (entry == NULL)
            break;

        KAPS_STRY(kaps_entry_get_hb(db_info->parse_info->db.db, entry, &hb));

        if (hb)
        {
            KAPS_STRY(kaps_hb_entry_get_bit_value(db_info->parse_info->hb_db_info->hb_db, hb, &bit_value, 1));
        }
    }
    while (1);

    kaps_db_entry_iter_destroy(db_info->parse_info->db.db, iter);

    return KAPS_OK;
}

static kaps_status
validate_hb_entries(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    uint32_t get_aged_entries)
{
    if (tinfo->device->id < KAPS_JERICHO_2_DEVICE_ID)
    {
        KAPS_STRY(validate_hb_entries_jr1(tinfo, db_info, get_aged_entries));
    }
    else
    {
        KAPS_STRY(validate_hb_entries_no_hb_machine(tinfo, db_info));
    }

    return KAPS_OK;
}

/**
  * Randomly pick a database from the set of databases we have
 * and perform a set of misc operations that include
 */
static kaps_status
perform_entry_ops(
    struct xml_test *tinfo)
{
    struct test_db_info *db_info;
    struct kaps_parse_record *add;
    uint32_t priority = -1;
    int32_t nindices, *indices, i;
    struct kaps_ad *ad;
    struct kaps_hb *hb;
    struct kaps_entry_info e_info;
    kaps_status status = KAPS_OK;

    db_info = pick_database(tinfo);
    add = get_next_entry(db_info, 1);

    if (!add)
    {
        /*
         * No active entries
         */
        return KAPS_OK;
    }

    if (add->in_search_loop)
        return KAPS_OK;

    KAPS_TRY(kaps_entry_get_priority(db_info->parse_info->db.db, add->e, &priority));
    if (db_info->parse_info->type == KAPS_DB_EM)
    {
        /*
         * Priority should always be zero
         */
        if (priority != 0)
        {
            kaps_printf("++++ Error priority returned by EM is not zero\n");
            return KAPS_INTERNAL_ERROR;
        }
    }
    else if (db_info->parse_info->type == KAPS_DB_LPM)
    {
        /*
         * Priority is the prefix length
         */
        if (priority != add->length)
        {
            kaps_printf("++++ Prefix length %d returned by API "
                        "does not match our prefix length %d\n", priority, add->length);
            return KAPS_INTERNAL_ERROR;
        }
    }

    if (tinfo->flag_device != KAPS_DEVICE_KAPS)
    {
        KAPS_TRY(kaps_entry_get_index(db_info->parse_info->db.db, add->e, &nindices, &indices));
        
        if (nindices > 0)
        {
            KAPS_TRY(kaps_entry_free_index_array(db_info->parse_info->db.db, indices));
        }
    }

    /*
     * get the AD for the entry and compare
     */

    if (db_info->ad_db_info)
    {
        struct kaps_verifier_ad_info *ad_info;
        struct kaps_entry_info e_info;
        uint8_t i, value[KAPS_HW_MAX_UDA_WIDTH_8];

        KAPS_TRY(kaps_device_set_property(tinfo->device, KAPS_DEVICE_PROP_READ_TYPE, 0));
        status = kaps_entry_get_info(db_info->parse_info->db.db, add->e, &e_info);
        if (status == KAPS_PENDING_ENTRY)
        {
            kaps_sassert(add->status != FAST_TRIE_ACTIVE);
        }
        else if (status != KAPS_OK)
        {
            return status;
        }

        KAPS_TRY(kaps_entry_get_ad(db_info->parse_info->db.db, add->e, &ad));
        if (ad == NULL)
        {
            /*
             * problem 
             */
            kaps_printf("++++ Entry returned NULL AD, not expected\n");
            return KAPS_INTERNAL_ERROR;
        }
        ad_info = (struct kaps_verifier_ad_info *) add->ad_data;

        if (status != KAPS_PENDING_ENTRY)
        {
            kaps_sassert(ad_info->ad == e_info.ad_handle);
            kaps_sassert(ad_info->parse_ad_info->ad_db == e_info.ad_db);
        }

        if (ad_info->ad != ad)
        {
            kaps_printf("++++ AD entry handles do not match\n");
            return KAPS_INTERNAL_ERROR;
        }

        status = kaps_ad_db_get(ad_info->parse_ad_info->ad_db, ad, value);
        if (status == KAPS_PENDING_ENTRY)
            /*
             * wont read from device for pending entries 
             */ ;
        else if (status == KAPS_OK)
        {
            for (i = 0; i < ad_info->parse_ad_info->ad_align_8; i++)
            {
                if (value[i] != ad_info->ad_data[i])
                {
                    kaps_printf("++++ AD value Mis-Match: Got: ");
                    {
                        uint32_t x = 0;

                        for (x = 0; x < ad_info->parse_ad_info->ad_align_8; x++)
                            kaps_printf("%.2x ", value[x]);
                        kaps_printf(" Exp: ");
                        for (x = 0; x < ad_info->parse_ad_info->ad_align_8; x++)
                            kaps_printf("%.2x ", ad_info->ad_data[x]);
                    }
                    kaps_printf("\n");
                    return KAPS_INTERNAL_ERROR;
                }
            }
        }
        else
            return status;

        KAPS_TRY(kaps_device_set_property(tinfo->device, KAPS_DEVICE_PROP_READ_TYPE, 0));
    }

    /** Get the hit bit information and compare */
    if (db_info->parse_info->hb_db_info)
    {
        KAPS_STRY(kaps_entry_get_hb(db_info->parse_info->db.db, add->e, &hb));
        if (add->hb_e != hb)
        {
            kaps_printf("Mismatch in hb entry returned from kaps_entry_get_hb \n");
            return KAPS_INTERNAL_ERROR;
        }
    }

    /*
     * For LPM databases, retrieve the handle from prefix and ensure
     * it is valid
     */
    if (db_info->parse_info->type == KAPS_DB_LPM && add->status == FAST_TRIE_ACTIVE)
    {
        struct kaps_entry *hdl;

        KAPS_TRY(kaps_db_get_prefix_handle(db_info->parse_info->db.db, add->data, add->length, &hdl));
        kaps_sassert(hdl == add->e);
    }

    /*
     * For DMA databases, performs some adds and deletes
     * as C-Model support is not there yet.
     */
    if (db_info->parse_info->type == KAPS_DB_DMA)
    {
        uint8_t value[16];
        uint32_t offset;
        uint32_t iter;

        for (iter = 0; iter < 100; iter++)
        {
            offset = kaps_random_r(tinfo->seedp) % 4096;
            kaps_memset(value, (kaps_random_r(tinfo->seedp) % 256), 16);
            KAPS_TRY(kaps_dma_db_add_entry(db_info->parse_info->db.dma_db, value, offset));
        }

        for (iter = 0; iter < 100; iter++)
        {
            offset = kaps_random_r(tinfo->seedp) % 4096;
            KAPS_TRY(kaps_dma_db_delete_entry(db_info->parse_info->db.dma_db, offset));
        }
    }

    /*
     * Get the entry information and cross check
     */

    status = kaps_entry_get_info(db_info->parse_info->db.db, add->e, &e_info);
    if (status == KAPS_PENDING_ENTRY)
    {
        kaps_sassert(add->status != FAST_TRIE_ACTIVE);
    }
    else if (status != KAPS_OK)
    {
        return status;
    }

    KAPS_TRY(kaps_device_set_property(tinfo->device, KAPS_DEVICE_PROP_READ_TYPE, 0));
    if (status == KAPS_OK)
    {
        kaps_sassert((db_info->parse_info->width_1 / KAPS_BITS_IN_BYTE) == e_info.width_8);
        kaps_sassert(add->num_ranges == e_info.nranges);

        if (db_info->ad_db_info)
        {
            struct kaps_verifier_ad_info *ad_info;

            ad_info = (struct kaps_verifier_ad_info *) add->ad_data;
            kaps_sassert(ad_info->ad == e_info.ad_handle);
        }
        else
        {
            kaps_sassert(e_info.ad_handle == NULL);
        }

        if (add->status == FAST_TRIE_ACTIVE)
            kaps_sassert(e_info.active == 1);
        if (db_info->parse_info->type == KAPS_DB_LPM)
        {
            int32_t nbytes = (add->length + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;

            kaps_sassert(add->length == e_info.prio_len);
            for (i = 0; i < nbytes; i++)
            {
                kaps_sassert((add->data[i] & ~add->mask[i]) == (e_info.data[i] & ~e_info.mask[i]));
            }
        }
    }

    if (db_info->parse_info->hb_db_info)
    {
        /*
         * If the searches are done outside this function, then we may end up never calling validate_hb_entries. To
         * overcome this problem, we update the db_info->ncalls_timer to catch up with the number of searches
         */
        if (db_info->nsearches / 10000 > (db_info->ncalls_timer + 1))
        {
            db_info->ncalls_timer = (db_info->nsearches / 10000) - 1;
        }

        if ((db_info->nsearches / 10000) == (db_info->ncalls_timer + 1))
            KAPS_TRY(validate_hb_entries(tinfo, db_info, ((kaps_random_r(tinfo->seedp) % 5) == 0)));
    }

    db_info->nmisc++;
    return KAPS_OK;
}

/**
 * Randomly pick a database from the set of databases we have
 * and update AD of a random entry from it
 */

static kaps_status
perform_update_ad(
    struct xml_test *tinfo)
{
    struct test_db_info *db_info;
    struct kaps_parse_record *add;
    struct kaps_verifier_ad_info *ad_info;

    db_info = pick_database_with_ad(tinfo);
    add = get_next_entry(db_info, 1);

    if (!add)
    {
        /*
         * No active entries
         */
        return KAPS_OK;
    }

    if (add->in_search_loop)
        return KAPS_OK;

    /*
     * Update existing AD data
     */

    kaps_sassert(db_info->ad_db_info);
    kaps_sassert(add->ad_data);
    ad_info = (struct kaps_verifier_ad_info *) add->ad_data;
    fill_ad(tinfo, ad_info->parse_ad_info->ad_width_8, db_info->ad_db_info->ad_db->user_width_1, ad_info);

    if (ad_info->parse_ad_info->ad_db->user_width_1 == 0)
        return KAPS_OK;
    KAPS_TRY(kaps_ad_db_update_entry(ad_info->parse_ad_info->ad_db, ad_info->ad, ad_info->ad_data));
    db_info->nad_updates++;

    return KAPS_OK;
}

void
reconstruct_indices(
    struct test_db_info *db_info)
{
    uint32_t i;

    if (db_info->parse_info->db.db->is_clone)
        return;
    for (i = 0; i < db_info->pivot; i++)
    {
        if (db_info->entries[db_info->indices[i]].visited == 1)
        {
            uint32_t skip = 1;
            uint8_t req_swap = 1;
            while ((db_info->pivot >= skip) && db_info->entries[db_info->indices[db_info->pivot - skip]].visited == 1)
            {
                if ((db_info->pivot - skip) == i)
                {
                    db_info->pivot = i;
                    req_swap = 0;
                    break;
                }
                skip++;
            }
            if (db_info->pivot >= skip && req_swap)
            {
                db_info->pivot = db_info->pivot - skip;
            SWAP(db_info->indices[db_info->pivot], db_info->indices[i])}
        }
    }

    for (i = db_info->pivot; i < db_info->nentries; i++)
    {
        if (db_info->entries[db_info->indices[i]].visited == 0)
        {
            SWAP(db_info->indices[db_info->pivot], db_info->indices[i]) db_info->pivot++;
        }
    }

    for (i = 0; i < db_info->pivot; i++)
    {
        if (db_info->entries[db_info->indices[i]].visited == 1)
            kaps_sassert(0);
    }

    for (i = db_info->pivot; i < db_info->nentries; i++)
    {
        if (db_info->entries[db_info->indices[i]].visited == 0)
            kaps_sassert(0);
    }
}

static kaps_status
refresh_the_handles(
    struct xml_test *tinfo)
{
    uint32_t i, j;
    struct test_db_info *db_info = NULL;
    struct test_instr_info *instr_info = NULL;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct kaps_device *db_device = NULL;

        db_info = &tinfo->db_info_array[i];
        if (db_info->parse_info->bc_dbs)
        {
            struct kaps_device *device = tinfo->device;
            int32_t flag = 0;

            for (j = 0; j < tinfo->num_bc_devices; j++)
            {
                if (db_info->parse_info->bc_dbs[j])
                {
                    if (!flag)
                    {
                        KAPS_TRY(kaps_db_refresh_handle(device, db_info->parse_info->db.db,
                                                        &db_info->parse_info->db.db));
                        db_info->parse_info->db.db->handle = db_info;
                        db_device = device;
                        flag = 1;
                        db_info->parse_info->bc_dbs[j] = db_info->parse_info->db.db;
                    }
                    else
                    {
                        db_info->parse_info->bc_dbs[j] =
                            kaps_db_get_bc_db_on_device(device, db_info->parse_info->db.db);
                    }
                }
                device = device->next_bc_device;
            }
        }
        else
        {
            KAPS_TRY(kaps_db_refresh_handle(tinfo->device, db_info->parse_info->db.db, &db_info->parse_info->db.db));
            db_info->parse_info->db.db->handle = db_info;
            db_device = tinfo->device;
        }

        if (db_info->ad_db_info && !db_info->container_db)
        {
            struct kaps_parse_ad_info *tmp = db_info->ad_db_info;

            while (tmp)
            {
                kaps_sassert(db_device != NULL);
                KAPS_TRY(kaps_ad_db_refresh_handle(db_device, tmp->ad_db, &tmp->ad_db));
                tmp = tmp->next;
            }
        }

        if (db_info->parse_info->hb_db_info && !db_info->container_db)
        {
            KAPS_TRY(kaps_hb_db_refresh_handle(db_device, db_info->parse_info->hb_db_info->hb_db,
                                               &db_info->parse_info->hb_db_info->hb_db));
        }
    }

    for (i = 0; i < tinfo->num_instructions; i++)
    {
        struct kaps_device *device;

        instr_info = &tinfo->instruction_info_array[i];
        device = tinfo->device;
        for (j = 0; j < tinfo->num_bc_devices; j++)
        {
            if (device->bc_id == instr_info->info->bc_dev_no)
                break;
            device = device->next_bc_device;
        }
        KAPS_TRY(kaps_instruction_refresh_handle(device,
                                                 instr_info->info->instruction, &instr_info->info->instruction));
    }

    return KAPS_OK;
}

static kaps_status
clear_hits_during_warmboot(
    struct xml_test *tinfo)
{
    uint32_t i, iter;
    struct test_db_info *db_info;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct kaps_parse_record *rec;

        db_info = &tinfo->db_info_array[i];

        if (db_info->parse_info->db.db->is_clone)
            continue;

        for (iter = 0; iter < db_info->nentries; iter++)
        {
            rec = &db_info->entries[iter];

            if (rec)
            {
                rec->hit = 0;
            }
        }
    }

    return KAPS_OK;
}

/**
 * Perform Warmboot operations on device
 * Warmboot Save, Device destroy and Warmboot Restore
 */

static kaps_status
perform_warmboot_operations(
    struct xml_test *tinfo)
{
    struct test_db_info *tmp = NULL, *db_info = NULL;
    struct kaps_parse_record *add = NULL;
    struct kaps_entry *e = NULL;
    struct kaps_timeval tv;
    uint32_t start_time, end_time, time_taken;
    kaps_status status = KAPS_OK;
    int32_t i;
    uint32_t rand, iter;
    struct kaps_allocator *alloc = tinfo->device->alloc;
    struct kaps_device *bc_devices[KAPS_MAX_BROADCAST_DEVICES];
    void *bc_xpts[KAPS_MAX_BROADCAST_DEVICES];
    static uint32_t dbg_count = 0;

    kaps_sassert(tinfo->flag_warmboot);

    clear_hits_during_warmboot(tinfo);

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct kaps_db_stats stats;

        db_info = &tinfo->db_info_array[i];
        KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
        db_info->num_entries_present = stats.num_entries;
    }

    tinfo->issu_in_progress = 1;
    tinfo->num_file_ops = 0;

    /*
     * By default we will be using heap memory, if we want to dump to a file change it 
     */
    if (1)
    {
        uint32_t wb_mem = 160;  /* O3S */

        tinfo->issu_nv_memory = (uint8_t *) kaps_sysmalloc(wb_mem * 1024 * 1024);
        if (!tinfo->issu_nv_memory)
        {
            kaps_printf("NV Memory alloc failed\n");
            return KAPS_OUT_OF_MEMORY;
        }
        tinfo->nv_size = wb_mem * 1024 * 1024;
    }
    else
    {
        if (tinfo->flag_output_dir)
        {
            char op_filename[100];

            sal_strlcpy(op_filename, tinfo->flag_output_dir, sizeof(op_filename));
            sal_strncat_s(op_filename, ".dump", sizeof(op_filename));
            tinfo->issu_dump_file = kaps_fopen(op_filename, "w+");
        }
        else
        {
            tinfo->issu_dump_file = kaps_fopen("kaps_verifier_wb.dump", "w+");
        }
    }

    kaps_gettimeofday(&tv);
    start_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;

    bc_devices[0] = tinfo->device;
    bc_xpts[0] = bc_devices[0]->xpt;
    for (i = 1; i < tinfo->num_bc_devices; i++)
    {
        bc_devices[i] = bc_devices[i - 1]->next_bc_device;
        bc_xpts[i] = bc_devices[i]->xpt;
    }

    dbg_count++;

    if (dbg_count & 1)
        KAPS_TRY(kaps_device_save_state_and_continue(tinfo->device, kaps_verifier_wb_nv_read,
                                                     kaps_verifier_wb_nv_write, (void *) tinfo));
    else
        KAPS_TRY(kaps_device_save_state(tinfo->device, kaps_verifier_wb_nv_read,
                                        kaps_verifier_wb_nv_write, (void *) tinfo));

    kaps_gettimeofday(&tv);
    end_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
    time_taken = end_time - start_time;

    if (time_taken > tinfo->peak_issu_save_time)
        tinfo->peak_issu_save_time = time_taken;

    if (tinfo->num_file_ops > tinfo->peak_file_ops)
        tinfo->peak_file_ops = tinfo->num_file_ops;

    KAPS_TRY(kaps_device_destroy(tinfo->device));

    for (i = 0; i < tinfo->num_bc_devices; i++)
    {
        struct kaps_device *device = NULL;

        KAPS_STRY(kaps_device_init(alloc, tinfo->flag_device,
                                   tinfo->device_init_flags | KAPS_DEVICE_SKIP_INIT,
                                   bc_xpts[i], tinfo->config, &device));

        if (i == 0)
            tinfo->device = device;
    }

    KAPS_TRY(kaps_device_set_property(tinfo->device, (enum kaps_device_properties) KAPS_DEVICE_PROP_ADV_PRINT, 2));

    tinfo->num_file_ops = 0;

    kaps_gettimeofday(&tv);
    start_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;

    KAPS_TRY(kaps_device_restore_state(tinfo->device, kaps_verifier_wb_nv_read,
                                       kaps_verifier_wb_nv_write, (void *) tinfo));
    kaps_gettimeofday(&tv);
    end_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
    time_taken = end_time - start_time;

    KAPS_TRY(refresh_the_handles(tinfo));

    if (time_taken > tinfo->peak_issu_restore_time)
        tinfo->peak_issu_restore_time = time_taken;

    if (tinfo->num_file_ops > tinfo->peak_file_ops)
        tinfo->peak_file_ops = tinfo->num_file_ops;

    /*
     * 1 out of 200 WB ops reconcile will trigger 
     */
    rand = kaps_random_r(tinfo->seedp) % 200;

    /*
     * Start the reconcile phase here
     */
    if (tinfo->block_warmboot_reconcile == 0 && (!rand || (tinfo->flag_warmboot == 1)))
    {
        uint32_t count = 0;

        KAPS_TRY(kaps_device_reconcile_start(tinfo->device));

        for (i = 0; i < tinfo->num_databases; i++)
        {
            struct kaps_parse_record *rec;
            uint32_t set_used_iterations;

            db_info = &tinfo->db_info_array[i];

            if (db_info->parse_info->db.db->is_clone)
                continue;

            set_used_iterations = (kaps_random_r(tinfo->seedp) % 2) + 9;
            set_used_iterations *= (db_info->num_entries_present / 10);

            for (iter = 0; iter < db_info->nentries; iter++)
            {
                rec = &db_info->entries[iter];
                /*
                 * Mark all entries as inactive initially in the reconcile phase
                 */
                if (rec->status == FAST_TRIE_ACTIVE || rec->status == FAST_TRIE_PENDING)
                {
                    if (rec->status == FAST_TRIE_PENDING)
                    {
                        /*
                         * This entry is sitting in the pending list of the database
                         * remove it from there also.
                         */
                        struct kaps_parse_record *tmp, *prev = NULL;
                        for (tmp = db_info->pending; tmp; tmp = tmp->next)
                        {
                            if (tmp == rec)
                            {
                                if (prev == NULL)
                                    db_info->pending = tmp->next;
                                else
                                    prev->next = tmp->next;
                                break;
                            }
                            prev = tmp;
                        }
                        rec->next = NULL;
                    }
                    else
                    {
                        rec->status = FAST_TRIE_INACTIVE;
                    }
                    db_info->ndeletes++;
                }
            }
            /*
             * Mark some of the active entries as used
             */
            for (iter = 0; iter < db_info->nentries; iter++)
            {
                rec = &db_info->entries[iter];
                if (!rec->visited)
                    continue;
                if (count == set_used_iterations)
                    break;
                KAPS_TRY(kaps_entry_set_used(db_info->parse_info->db.db, rec->e));
                db_info->ndeletes--;

                if (rec->status == FAST_TRIE_PENDING)
                {
                    rec->visited = 0;
                    if (tinfo->flag_use_pivot)
                    {
                        SWAP(db_info->indices[db_info->index], db_info->indices[db_info->pivot]) db_info->pivot++;
                    }
                }
                else
                {
                    rec->status = FAST_TRIE_ACTIVE;
                }
                count++;
            }
            /*
             * Update the visited flag for the entries not being set as used
             */
            for (iter = 0; iter < db_info->nentries; iter++)
            {
                rec = &db_info->entries[iter];
                if (rec->status == FAST_TRIE_INACTIVE)
                {
                    rec->e = NULL;
                    rec->hb_e = NULL;
                    rec->visited = 0;
                    if (rec->ad_data && rec != add)
                    {
                        struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) (rec->ad_data);

                        ad_info->refcount--;
                        if (ad_info->refcount == 0)
                            free_ad(tinfo, db_info, ad_info);
                        rec->ad_data = NULL;
                    }
                }
                else if (rec->status == FAST_TRIE_PENDING)
                {
                    if (rec->visited == 0)
                    {
                        rec->next = db_info->pending;
                        db_info->pending = rec;
                    }
                    else
                    {
                        rec->e = NULL;
                        rec->hb_e = NULL;
                        rec->status = FAST_TRIE_INACTIVE;
                        if (rec->ad_data && rec != add)
                        {
                            struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) (rec->ad_data);

                            ad_info->refcount--;
                            if (ad_info->refcount == 0)
                                free_ad(tinfo, db_info, ad_info);
                            rec->ad_data = NULL;
                        }
                    }
                    rec->visited = !rec->visited;
                }
            }
            if (db_info->delete_pending)
            {
                struct kaps_parse_record *prev = NULL;

                add = db_info->delete_pending;
                while (add)
                {
                    struct kaps_parse_record *next = add->next;

                    if (add->status == FAST_TRIE_INACTIVE)
                    {
                        if (add == db_info->delete_pending)
                            db_info->delete_pending = next;
                        else
                        {
                            kaps_sassert(prev != NULL);
                            prev->next = add->next;
                        }
                        add->next = NULL;
                        add->pending_del = 0;
                    }
                    else
                        prev = add;
                    add = next;
                }
            }
        }
        /*
         * End of Reconcile Phase 
         */
        KAPS_TRY(kaps_device_reconcile_end(tinfo->device));
    }

    KAPS_TRY(kaps_device_lock(tinfo->device));

    for (i = 0; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];

        /*
         * If database population is less than 90% of least full point database is no longer considered to be brimmed. 
         */
        if (db_info->is_brimmed)
        {
            struct kaps_db_stats stats;

            KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &stats));
            if (stats.num_entries < ((9 * db_info->least_full_point) / 10))
            {
                db_info->is_brimmed = 0;
            }
        }
    }

    /*
     * If we added an entry before save we should install it now 
     */
    if (add)
    {
        kaps_sassert(add->next == NULL);
        add->next = tmp->pending;
        tmp->pending = add;
        add->status = FAST_TRIE_PENDING;

        status = perform_install_on_db(tinfo, tmp);
        if (status == KAPS_OUT_OF_DBA
            || status == KAPS_OUT_OF_UDA
            || status == KAPS_OUT_OF_INDEX
            || status == KAPS_OUT_OF_UIT || status == KAPS_OUT_OF_AD || status == KAPS_EXHAUSTED_PCM_RESOURCE)
        {
            if (tmp->parse_info->type == KAPS_DB_LPM)
            {
                KAPS_TRY(kaps_db_delete_entry(tmp->parse_info->db.db, e));
                if (tmp->ad_db_info)
                {
                    struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) add->ad_data;

                    if (ad_info->refcount > 0)
                        ad_info->refcount--;
                    if (!ad_info->refcount)
                    {
                        if (ad_info != db_info->zero_size_ad_info)
                        {
                            KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                            free_ad(tinfo, db_info, ad_info);
                        }
                    }
                    add->ad_data = NULL;
                }

                /*
                 * Delete HB Entry 
                 */
                if (db_info->parse_info->hb_db_info && add->hb_e)
                {
                    KAPS_TRY(kaps_hb_db_delete_entry(db_info->parse_info->hb_db_info->hb_db, add->hb_e));
                    add->hb_e = NULL;
                }
                add->e = NULL;
                add->status = FAST_TRIE_INACTIVE;
                add->ad_data = NULL;
                tmp->pending = NULL;
            }
            else
            {
                add->visited = 1;
                add->e = e;
            }
        }
        else if (status == KAPS_OK)
        {
            add->e = e;
            add->visited = 1;
            add->status = FAST_TRIE_ACTIVE;
            db_info->nadds++;
        }
        else
        {
            return status;
        }
    }
    /*
     * By default we will be using heap memory, if we want to dump to a file change it 
     */
    if (1)
    {
        kaps_sysfree(tinfo->issu_nv_memory);
        tinfo->issu_nv_memory = NULL;
        tinfo->nv_size = 0;
    }
    else
    {
        kaps_fclose(tinfo->issu_dump_file);
    }

    /*
     * Disabling for now as this will impact the regression time Will enable after some testing to decide the
     * probability 
     */
    if (0)
        KAPS_TRY(perform_heavy_search_and_delete(tinfo));

    tinfo->num_issu++;
    tinfo->issu_in_progress = 0;

    if (tinfo->flag_use_pivot)
    {
        for (i = 0; i < tinfo->num_databases; i++)
            reconstruct_indices(&tinfo->db_info_array[i]);
    }
    return KAPS_OK;
}

static kaps_status
populate_the_database(
    struct xml_test *tinfo)
{
    struct xml_parse_info *parent = NULL, *tab = NULL;
    uint32_t j, count = 0, ntables = 0, total = 0;
    uint32_t nadds = 0, ndups = 0;
    struct test_db_info *db_info = tinfo->target_db;
    struct test_db_info **table_info = NULL;
    struct kaps_timeval tv;
    uint32_t capacity = 0, start_time, end_time, time_taken = 0;
    kaps_status status;
    struct kaps_db_stats stats;

    if (db_info == NULL)
    {
        return KAPS_INTERNAL_ERROR;
    }
    parent = db_info->parse_info;
    capacity = parent->db.db->common_info->capacity;

    if (db_info->parse_info->parse_upto > 0)
        capacity = db_info->parse_info->parse_upto;

    if (!capacity)
        capacity = -1;

    for (tab = parent; tab; tab = tab->next_tab)
    {
        struct xml_parse_info *ctab = NULL;
        total++;
        ntables++;
        for (ctab = tab->clone; ctab; ctab = ctab->next)
            total++;
    }

    table_info = kaps_syscalloc(ntables, sizeof(struct test_db_info *));

    if (table_info == NULL)
        return KAPS_OUT_OF_MEMORY;

    count = 0;

    for (tab = parent; tab; tab = tab->next_tab)
    {
        table_info[count] = get_test_info_for_xml_db(tinfo, tab);
        count++;
    }
    kaps_sassert(count == ntables);

    count = nadds = 0;
    kaps_gettimeofday(&tv);
    start_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;

    while (1)
    {
        struct test_db_info *cur_db = NULL;

        if (table_info[0]->parse_info->table_mix_percent)
        {
            int32_t i, random_no = 0;

            random_no = kaps_random_r(tinfo->seedp) % 100;
            for (i = 0; i < ntables; i++)
            {
                if (random_no >= table_info[i]->parse_info->tbl_mix_percent_low &&
                    random_no <= table_info[i]->parse_info->tbl_mix_percent_high)
                    break;
            }
            cur_db = table_info[i];
        }
        else
        {
            cur_db = table_info[count];
        }

        tinfo->target_db = cur_db;

        if (cur_db->is_brimmed || nadds >= capacity ||
            cur_db->nadds >= cur_db->nentries || nadds >= tinfo->nentries_to_add)
            break;

        status = add_entry_to_database(tinfo, cur_db);

        if (status == KAPS_OUT_OF_AD)
        {
            status = KAPS_OK;
        }

        if (status != KAPS_OK)
        {
            kaps_sysfree(table_info);
            return status;
        }

        count++;
        if (count >= ntables)
            count = 0;
        nadds++;

        if (nadds % 10000 == 0)
        {
            status = kaps_db_stats(cur_db->parse_info->db.db, &stats);
            if (status != KAPS_OK)
            {
                kaps_sysfree(table_info);
                return status;
            }

            if (stats.capacity_estimate > cur_db->max_capacity_estimate)
                cur_db->max_capacity_estimate = stats.capacity_estimate;

            if (stats.capacity_estimate < cur_db->min_capacity_estimate)
                cur_db->min_capacity_estimate = stats.capacity_estimate;

            cur_db->most_recent_capacity_estimate = stats.capacity_estimate;
        }
    }

    kaps_gettimeofday(&tv);
    end_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
    time_taken = end_time - start_time;

    if (time_taken == 0)
        time_taken = 1;

    for (j = 0; j < ntables; j++)
    {
        ndups += table_info[j]->nduplicates;
    }

    for (tab = parent; tab; tab = tab->next_tab)
    {
        struct test_db_info *db = get_test_info_for_xml_db(tinfo, tab);

        if (db == NULL)
        {
            status = KAPS_INTERNAL_ERROR;
            return status;
        }
        if (tab == parent)
        {
            kaps_printf(" - Total adds for %s-%d : %d",
                        kaps_device_db_name(tab->db.db), tab->db.db->tid, (db->nadds - db->nduplicates));

            kaps_printf("    ops/sec : %llu", (1000000ULL * (uint64_t) (nadds - ndups)) / time_taken);
            if (tinfo->n_status != KAPS_OK)
            {
                kaps_printf(" Reason: %s", kaps_get_status_string(tinfo->n_status));
                tinfo->n_status = KAPS_OK;
            }

        }
        else
        {
            kaps_printf("       - Table Total adds for %s-%d : %d",
                        kaps_device_db_name(tab->db.db), tab->db.db->tid, (db->nadds - db->nduplicates));
        }
        {
            float value = db->nadds - db->nduplicates;

            if ((db->nadds - db->nduplicates) < (1024 * 1024))
                kaps_printf(" [ %.3fK ]", (value / (1024.0)));
            else
                kaps_printf(" [ %.3fM ]", (value / (1024.0 * 1024)));

            if (db->user_cap_min || db->user_cap_max || db->user_ops_min || db->user_ops_max)
            {
                kaps_printf("\n EYE_SCAN (Exp v/s Got):  Capacity:: min-max(%d %d)(%d)    Ops:: min-max(%u %u) (%u) \n",
                            db->user_cap_min, db->user_cap_max,
                            (db->nadds - db->nduplicates),
                            db->user_ops_min, db->user_ops_max, (1000000ULL * (uint64_t) (nadds - ndups)) / time_taken);
            }
        }
        sal_fflush(stdout);
        {
            /*
             * prints for KAPS capacity matrix parsing 
             */
            kaps_printf("\nINCRE: cap: %d max_cap: 0 ops/s: %llu heap:0.0MB pio:0 shuffles:0\n",
                        (db->nadds - db->nduplicates), (1000000ULL * (uint64_t) (nadds - ndups)) / time_taken);

        }
        kaps_printf("\n");
    }
    kaps_sysfree(table_info);
    return KAPS_OK;
}

kaps_status
preadd_the_default_entries(
    struct xml_test * tinfo)
{
    uint32_t iter = 0, i, print = 0;
    struct test_db_info *db_info = NULL;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct kaps_db *db = NULL;
        struct kaps_entry *e = NULL;

        db_info = &tinfo->db_info_array[i];

        db = db_info->parse_info->db.db;

        if (db_info->end_index_default_entries != 0)
        {
            /*
             * kaps_sassert(db_info->parse_info->type == KAPS_DB_LPM);
             */

            if (!print)
            {
                kaps_printf("\n - Adding the Default Entries to Databases\n");
                print = 1;
            }

            for (iter = 0; iter < db_info->end_index_default_entries; iter++)
            {
                struct kaps_parse_record *add = &db_info->entries[iter];
                if (db_info->parse_info->type == KAPS_DB_LPM)
                {
                    KAPS_TRY(kaps_db_add_prefix(db, add->data, add->length, &e));
                    KAPS_TRY(kaps_entry_set_property(db, e, KAPS_ENTRY_PROP_META_PRIORITY, add->priority >> 8));
                }

                if (db_info->ad_db_info)
                {
                    KAPS_TRY(add_ad_to_entry(tinfo, db_info, add, e));
                }

                KAPS_TRY(kaps_db_install(db));

                add->e = e;
                add->visited = 1;
                add->status = FAST_TRIE_ACTIVE;
                db_info->nadds++;

                if (tinfo->flag_use_pivot)
                {
                    db_info->index = iter;
                    SWAP(db_info->indices[db_info->index], db_info->indices[db_info->pivot - 1]);
                    if (db_info->pivot)
                        db_info->pivot--;
                }
            }
            kaps_printf(" - Total adds of default entries for %s-%d : %d\n", kaps_device_db_name(db),
                        db->tid, (db_info->nadds - db_info->nduplicates));
        }
    }

    return KAPS_OK;
}

kaps_status
preadd_the_entries_for_batch_mode(
    struct xml_test * tinfo)
{
    struct xml_parse_info *tmp = NULL, *xml_parse_info = tinfo->xml_parse_info;
    uint8_t print = 0;

    tinfo->adds_in_progress = 1;
    for (tmp = xml_parse_info; tmp; tmp = tmp->next)
    {
        struct test_db_info *db_info = NULL;
        if (tmp->type == KAPS_DB_AD || tmp->type == KAPS_DB_COUNTER || tmp->type == KAPS_DB_TAP || tmp->incremental)
            continue;

        if (!print)
        {
            kaps_printf("\n - Adding the Entries to Databases in Batch Mode\n");
            print = 1;
        }
        db_info = get_test_info_for_xml_db(tinfo, tmp);
        tinfo->target_db = db_info;
        KAPS_STRY(populate_the_database(tinfo));
    }

    tinfo->target_db = NULL;
    tinfo->adds_in_progress = 0;
    return KAPS_OK;
}

static kaps_status
search_all_active_entries(
    struct xml_test *tinfo)
{
    uint32_t i;

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct test_db_info *db_info = &tinfo->db_info_array[i];
        uint32_t iter = 0;

        tinfo->target_db = db_info;
        for (iter = 0; iter < db_info->nentries; iter++)
        {
            if (db_info->entries[iter].status == FAST_TRIE_ACTIVE)
            {
                tinfo->target_rec = &db_info->entries[iter];
                KAPS_TRY(perform_search(tinfo, &tinfo->instruction_info_array[db_info->db_inst_info[0]]));
            }
        }
    }
    tinfo->target_rec = NULL;
    tinfo->target_db = NULL;
    return KAPS_OK;
}

static kaps_status
recover_from_crash(
    struct xml_test *tinfo)
{
    struct kaps_allocator *alloc;
    enum kaps_restore_status cr_status;
    uint32_t i;
    kaps_status st = KAPS_OK;

    tinfo->num_crash_recovery++;
    tinfo->can_crash = 0;

    if (1)
    {
        struct kaps_device *device = tinfo->device;
        struct kaps_allocator *alloc_p = device->alloc;
        uint8_t *dummy = (uint8_t *) kaps_sysmalloc(tinfo->nv_size);

        kaps_memcpy(dummy, tinfo->issu_nv_memory, tinfo->nv_size);

        device->issu_in_progress = 1;
        if (device->smt)
            device->smt->issu_in_progress = 1;

        tinfo->do_not_update_index_change_callbacks = 1;

        st = kaps_device_destroy(tinfo->device);
        if (st != KAPS_OK)
        {
            kaps_sysfree(dummy);
            return st;
        }
        tinfo->do_not_update_index_change_callbacks = 0;
        kaps_memcpy(tinfo->issu_nv_memory, dummy, tinfo->nv_size);
        kaps_sysfree(dummy);
        KAPS_TRY(kaps_default_allocator_destroy(alloc_p));
    }
    KAPS_TRY(kaps_default_allocator_create(&alloc));
    KAPS_TRY(kaps_device_init(alloc, tinfo->flag_device, tinfo->device_init_flags | KAPS_DEVICE_SKIP_INIT,
                              tinfo->xpt, tinfo->config, &tinfo->device));

    KAPS_TRY(kaps_device_set_property(tinfo->device, KAPS_DEVICE_PROP_CRASH_RECOVERY, 0, (void *) tinfo->issu_nv_memory,
                                      tinfo->nv_size));

    KAPS_TRY(kaps_device_restore_state(tinfo->device, NULL, NULL, NULL));
    KAPS_TRY(kaps_device_query_restore_status(tinfo->device, &cr_status));
    KAPS_TRY(kaps_device_clear_restore_status(tinfo->device));
    KAPS_TRY(refresh_the_handles(tinfo));
    KAPS_TRY(kaps_device_lock(tinfo->device));

    for (i = 0; i < tinfo->num_databases; i++)
    {
        struct test_db_info *db_info = &tinfo->db_info_array[i];
        struct kaps_parse_record *add = db_info->pending;
        uint32_t reconstruct_pivot = 0;

        while (add)
        {
            struct kaps_parse_record *next = add->next;

            reconstruct_pivot = 1;
            add->next = NULL;
            kaps_sassert(add->status == FAST_TRIE_PENDING);
            add->status = FAST_TRIE_INACTIVE;
            add->e = NULL;
            add->hb_e = NULL;
            kaps_sassert(add->visited == 1);
            add->visited = 0;
            add->ad_data = NULL;
            add = next;
        }
        db_info->pending = NULL;
        /*
         * If defer deletes is set, we need to revert the pending delete entries 
         */
        if (tinfo->flag_defer_deletes)
        {
            add = db_info->delete_pending;
            while (add)
            {
                struct kaps_parse_record *next = add->next;

                reconstruct_pivot = 1;
                add->next = NULL;
                add->status = FAST_TRIE_ACTIVE;
                add->visited = 1;
                add->pending_del = 0;
                add = next;
            }
            db_info->delete_pending = NULL;
        }
        if (reconstruct_pivot && tinfo->flag_use_pivot)
            reconstruct_indices(db_info);
    }
    /*
     * If any delete entry to replay 
     */
    if (tinfo->last_delete_rec && !tinfo->flag_defer_deletes)
    {
        KAPS_TRY(kaps_db_delete_entry(tinfo->xpt_db->parse_info->db.db, tinfo->last_delete_rec->e));
        tinfo->last_delete_rec->e = NULL;
        tinfo->last_delete_rec->status = FAST_TRIE_INACTIVE;
        tinfo->last_delete_rec->ad_data = NULL;
        tinfo->last_delete_rec->visited = 0;
    }

    if (tinfo->flag_crash_recovery > 2)
        KAPS_TRY(search_all_active_entries(tinfo));
    return KAPS_OK;
}

/**
 * Run the core test loop
 */

static kaps_status
run_random_core_loop(
    struct xml_test *tinfo)
{
    int32_t index = 0;
    int32_t progress = tinfo->flag_num_iterations / PROGRESS_BAR_FACTOR;
    kaps_status status = KAPS_OK;
    int32_t previous_percent = -1, current_percent;

    if (progress == 0)
        progress = tinfo->flag_num_iterations;

    if (tinfo->num_operations)
    {
        while (1)
        {
            enum test_operation next_op;

            index = kaps_random_r(tinfo->seedp) % tinfo->num_operations;
            next_op = tinfo->operations_array[index];

            current_percent = tinfo->num_iterations * 100 / tinfo->flag_num_iterations;

            if (tinfo->flag_warmboot == 2)
            {
                /*
                 * We want warmboot to be triggered 100 times with -w2 option. So when the percentage of operations
                 * performed changes (say from 1% to 2%), we will trigger warmboot 
                 */
                if (previous_percent != current_percent)
                    next_op = TEST_WARMBOOT;
            }
            else if (tinfo->flag_crash_recovery == 2)
            {
                if (previous_percent != current_percent)
                    next_op = TEST_CRASH_RECVRY;
            }
            else if (tinfo->flag_ISSU == 2)
            {
                if ((kaps_random_r(tinfo->seedp) % (tinfo->num_operations * 1000)) == 99)
                    next_op = TEST_ISSU;
            }

            switch (next_op)
            {
                case TEST_PIO:
                    kaps_sassert(tinfo->flag_coherency);
                    if (tinfo->device->type == KAPS_DEVICE_KAPS)
                    {
                        /*
                         * PMR-TO-DO implement this function later
                         */
                        /*
                         * status = kaps_verifier_kaps_xpt_write_pio(tinfo, 1);
                         */
                    }
                    break;
                case TEST_ADD:
                    status = select_db_and_add_entry_to_itself(tinfo);
                    break;
                case TEST_DELETE:
                    status = select_db_and_delete_entry_from_itself(tinfo);
                    break;
                case TEST_HEAVY_SEARCH_DELETE:
                    status = perform_heavy_search_and_delete(tinfo);
                    break;
                case TEST_SEARCH:
                    tinfo->can_crash = 0;
                    status = select_instr_and_perform_search(tinfo);
                    tinfo->can_crash = 1;
                    break;
                case TEST_INSTALL:
                    status = perform_install(tinfo);
                    break;
                case TEST_ITER:
                    tinfo->can_crash = 0;
                    status = perform_entry_iter(tinfo);
                    tinfo->can_crash = 1;
                    break;
                case TEST_ENTRY:
                    tinfo->can_crash = 0;
                    status = perform_entry_ops(tinfo);
                    tinfo->can_crash = 1;
                    break;
                case TEST_UPDATE_AD:
                    tinfo->can_crash = 0;
                    status = perform_update_ad(tinfo);
                    tinfo->can_crash = 1;
                    break;
                case TEST_WARMBOOT:
                    status = perform_warmboot_operations(tinfo);
                    break;
                case TEST_CRASH_RECVRY:
                    status = recover_from_crash(tinfo);
                    break;

                default:
                    break;
            }
            if (status != KAPS_OK)
            {
                if (status == KAPS_DB_FULL)
                {
                    status = KAPS_OK;   /* Need to handle in module (RK), as of now kaps_verifier handling it */
                    return status;
                }
                else
                {
                    if (tinfo->allocator && !tinfo->allocator->check_status)
                        return status;
                    else if (status != KAPS_OUT_OF_MEMORY)
                        kaps_printf("Expecting error code as out of memory\n");

                    if (status == KAPS_OUT_OF_MEMORY)
                        return status;
                }
            }

            if (tinfo->search_all_active
                && (tinfo->search_all_active == -1 || tinfo->search_all_active == tinfo->num_iterations))
            {
                KAPS_STRY(search_all_active_entries(tinfo));
            }

            tinfo->num_iterations++;

            if ((tinfo->num_iterations % progress) == 0)
            {
                int32_t i, ratio = tinfo->num_iterations / progress;

                /*
                 * For every test case we will be triggering warmboot at 20 and 70 percent of the core loop 
                 */
                if ((tinfo->flag_warmboot == 1) && (ratio == 2 || ratio == 7))
                {
                    status = perform_warmboot_operations(tinfo);
                    if (status != KAPS_OK)
                    {
                        {
                            if (!tinfo->allocator->check_status)
                                return status;
                            else if (status != KAPS_OUT_OF_MEMORY)
                                kaps_printf("Expecting error code as out of memory\n");

                            if (status == KAPS_OUT_OF_MEMORY)
                                return status;
                        }
                    }
                }
                /*
                 * For every test case we will be triggering crash recovery at 30 and 80 percent of the core loop 
                 */
                if ((tinfo->flag_crash_recovery == 1) && (ratio == 3 || ratio == 8))
                {
                    /*
                     * FILE *fp = NULL, *fp1 = NULL;
                     * 
                     * fp = kaps_fopen("before_crash", "w"); fp1 = kaps_fopen("after_crash", "w");
                     * 
                     * if (!fp || !fp1) return 1;
                     * 
                     * kaps_device_print_html(tinfo->device, fp); kaps_fclose(fp); 
                     */

                    status = recover_from_crash(tinfo);
                    if (status != KAPS_OK)
                    {
                        {
                            if (!tinfo->allocator->check_status)
                                return status;
                            else if (status != KAPS_OUT_OF_MEMORY)
                                kaps_printf("Expecting error code as out of memory\n");
                            if (status == KAPS_OUT_OF_MEMORY)
                                return status;
                        }
                    }

                    /*
                     * kaps_device_print_html(tinfo->device, fp1); kaps_fclose(fp1); 
                     */
                }

                kaps_printf("\r%3d%% [", ratio * 10);
                for (i = 0; i < ratio; i++)
                    kaps_printf("=");
                for (i = ratio; i < PROGRESS_BAR_FACTOR; i++)
                    kaps_printf(" ");
                kaps_printf("]");
                if (tinfo->flag_warmboot)
                {
                    kaps_printf(" Successfully completed %d Warmboot Operations ", tinfo->num_issu);

                }
                else if (tinfo->flag_crash_recovery)
                    kaps_printf(" Successfully completed %d Crash Recovery Operations ", tinfo->num_crash_recovery);
                else if (tinfo->flag_ISSU)
                    kaps_printf(" Successfully completed %d ISSU Operations ", tinfo->num_issu_ops);

                sal_fflush(stdout);

            }

            if (status != KAPS_OK)
            {
                {
                    if (!tinfo->allocator->check_status)
                        return status;
                    else if (status != KAPS_OUT_OF_MEMORY)
                        kaps_printf("Expecting error code as out of memory\n");
                    if (status == KAPS_OUT_OF_MEMORY)
                        return status;
                }
            }

            previous_percent = current_percent;

            if (tinfo->num_iterations == tinfo->flag_num_iterations)
                break;

        }
    }

    kaps_printf("\n");
    return KAPS_OK;
}

int8_t
is_step_target_reached(
    struct xml_test * tinfo,
    struct test_step_target_expression * pTarget_expression)
{
    int8_t target_element_no;
    int8_t target_element_satisfied = 0;
    int8_t db_id;
    struct kaps_db_stats db_stats;
    uint32_t print_detailed_stats;

    print_detailed_stats = 1;

    for (target_element_no = 0; target_element_no < pTarget_expression->num_target_element; target_element_no++)
    {
        target_element_satisfied = 0;
        switch (pTarget_expression->target_elements[target_element_no].target_type)
        {
            case STEP_TARGET_ITER:
                if (tinfo->num_step_iterations ==
                    pTarget_expression->target_elements[target_element_no].element_iter.iter_count)
                {
                    target_element_satisfied = 1;
                    if ((pTarget_expression->num_target_element > 1)
                        && (pTarget_expression->expression_type == LOGICAL_OR))
                    {
                        kaps_printf("target iter_count reached, num_step_iterations: %d\n", tinfo->num_step_iterations);
                        kaps_fprintf(tinfo->scenario->all_steps_file,
                                     "target iter_count reached, num_step_iterations: %d<br>\n",
                                     tinfo->num_step_iterations);
                    }
                }
                else if ((pTarget_expression->target_elements[target_element_no].element_iter.iter_count > 10) &&
                         (tinfo->num_step_iterations %
                          (pTarget_expression->target_elements[target_element_no].element_iter.iter_count / 10) == 0))
                {
                    kaps_printf("iteration in progress: %d%%\n",
                                tinfo->num_step_iterations /
                                (pTarget_expression->target_elements[target_element_no].element_iter.iter_count / 10) *
                                10);
                }
                break;
            case STEP_TARGET_DB_FULL:
                db_id = pTarget_expression->target_elements[target_element_no].element_num_entries.db_id;
                if (tinfo->db_info_array[db_id].db_full ||
                    (tinfo->db_info_array[db_id].nadds - tinfo->db_info_array[db_id].ndeletes >=
                     tinfo->db_info_array[db_id].nentries - 1))
                {
                    target_element_satisfied = 1;
                    if ((pTarget_expression->num_target_element == 1)
                        || (pTarget_expression->expression_type == LOGICAL_OR))
                    {
                        KAPS_TRY(kaps_db_stats(tinfo->db_info_array[db_id].parse_info->db.db, &db_stats));

                        kaps_printf("target db_full reached, db_id: %d, nadds: %d, ndeletes: %d, capacity: %d\n", db_id,
                                    tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                    (db_stats.num_entries + db_stats.range_expansion));
                        kaps_fprintf(tinfo->scenario->all_steps_file,
                                     "target db_full reached, db_id: %d, nadds: %d, ndeletes: %d, capacity: %d<br>\n",
                                     db_id, tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                     (db_stats.num_entries + db_stats.range_expansion));

                        if (print_detailed_stats)
                        {
                            print_lpm_stats(tinfo, tinfo->db_info_array[db_id].parse_info->db.db,
                                            &tinfo->db_info_array[db_id]);
                        }

                        kaps_printf("____________________________________________\n\n\n\n\n");
                    }
                }
                else if (tinfo->db_info_array[db_id].nadds % 100000 == 0)
                {
                    kaps_printf("db_full: addition in progress: db_id: %d, num_adds: %d, num_delets: %d\n", db_id,
                                tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes);
                }
                break;
            case STEP_TARGET_NUM_ENTRIES:
                db_id = pTarget_expression->target_elements[target_element_no].element_num_entries.db_id;
                if ((tinfo->db_info_array[db_id].nadds - tinfo->db_info_array[db_id].ndeletes) ==
                    pTarget_expression->target_elements[target_element_no].element_num_entries.num_entries)
                {
                    target_element_satisfied = 1;
                    if ((pTarget_expression->num_target_element == 1)
                        || (pTarget_expression->expression_type == LOGICAL_OR))
                    {
                        KAPS_TRY(kaps_db_stats(tinfo->db_info_array[db_id].parse_info->db.db, &db_stats));

                        kaps_printf("target num_entries reached, db_id: %d, nadds: %d, ndeletes: %d, capacity: %d\n",
                                    db_id, tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                    (db_stats.num_entries + db_stats.range_expansion));
                        kaps_fprintf(tinfo->scenario->all_steps_file,
                                     "target num_entries reached, db_id: %d, nadds: %d, ndeletes: %d, capacity: %d<br>\n",
                                     db_id, tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                     (db_stats.num_entries + db_stats.range_expansion));
                        if (print_detailed_stats)
                        {
                            print_lpm_stats(tinfo, tinfo->db_info_array[db_id].parse_info->db.db,
                                            &tinfo->db_info_array[db_id]);
                        }

                        kaps_printf("____________________________________________\n\n\n\n\n");
                    }
                }
                else if ((pTarget_expression->target_elements[target_element_no].element_num_entries.num_entries > 10)
                         && ((tinfo->db_info_array[db_id].nadds - tinfo->db_info_array[db_id].ndeletes) %
                             (pTarget_expression->target_elements[target_element_no].element_num_entries.num_entries /
                              10)) == 0)
                {
                    kaps_printf("target_num: addition in progress: db_id: %d, num_adds: %d, num_delets: %d\n", db_id,
                                tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes);
                }
                break;
            case STEP_TARGET_DELETE_ENTRIES:
                db_id = pTarget_expression->target_elements[target_element_no].element_num_entries.db_id;
                if (tinfo->db_info_array[db_id].ndeletes >=
                    pTarget_expression->target_elements[target_element_no].element_delete_entries.target_ndeletes)
                {
                    target_element_satisfied = 1;
                    if ((pTarget_expression->num_target_element == 1)
                        || (pTarget_expression->expression_type == LOGICAL_OR))
                    {
                        KAPS_TRY(kaps_db_stats(tinfo->db_info_array[db_id].parse_info->db.db, &db_stats));

                        kaps_printf("target num_entries reached, db_id: %d, nadds: %d, ndeletes: %d, capacity:%d\n",
                                    db_id, tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                    (db_stats.num_entries + db_stats.range_expansion));
                        kaps_fprintf(tinfo->scenario->all_steps_file,
                                     "target num_entries reached, db_id: %d, nadds: %d, ndeletes: %d, capacity:%d<br>\n",
                                     db_id, tinfo->db_info_array[db_id].nadds, tinfo->db_info_array[db_id].ndeletes,
                                     (db_stats.num_entries + db_stats.range_expansion));

                        if (print_detailed_stats)
                        {
                            print_lpm_stats(tinfo, tinfo->db_info_array[db_id].parse_info->db.db,
                                            &tinfo->db_info_array[db_id]);
                        }

                        kaps_printf("____________________________________________\n\n\n\n\n");
                    }
                }
                else if ((pTarget_expression->target_elements[target_element_no].
                          element_delete_entries.target_ndeletes > 10)
                         && (tinfo->db_info_array[db_id].ndeletes %
                             (pTarget_expression->target_elements[target_element_no].
                              element_delete_entries.target_ndeletes / 10) == 0))
                {
                    kaps_printf("deletion in progress: db_id: %d, num_deletes: %d, target deletes: %d\n", db_id,
                                tinfo->db_info_array[db_id].ndeletes,
                                pTarget_expression->target_elements[target_element_no].
                                element_delete_entries.target_ndeletes);
                }
                break;
            case STEP_TARGET_REPEAT_STEPS:
                target_element_satisfied = 1;
                break;
        }
        if (pTarget_expression->num_target_element == 1)
            return target_element_satisfied;
        else if (pTarget_expression->expression_type == LOGICAL_OR && target_element_satisfied)
            return 1;
        else if (pTarget_expression->expression_type == LOGICAL_AND && (!target_element_satisfied))
            return 0;
    }
    return target_element_satisfied;
}

static kaps_status
Step_PreProcess(
    struct xml_test *tinfo,
    struct test_step *test_step)
{
    int i, j, k;

    for (i = 0; i < test_step->target_expression.num_target_element; i++)
    {
        if (test_step->target_expression.target_elements[i].target_type == STEP_TARGET_DELETE_ENTRIES)
        {
            int8_t db_id = test_step->target_expression.target_elements[i].element_delete_entries.db_id;
            int8_t delete_percentage =
                test_step->target_expression.target_elements[i].element_delete_entries.delete_percentage;

            tinfo->db_info_array[db_id].db_full = 0;
            test_step->target_expression.target_elements[i].element_delete_entries.target_ndeletes =
                tinfo->db_info_array[db_id].ndeletes +
                (tinfo->db_info_array[db_id].nadds
                 - tinfo->db_info_array[db_id].nduplicates
                 - tinfo->db_info_array[db_id].ndeletes) * delete_percentage / 100;
        }
        else if (test_step->target_expression.target_elements[i].target_type == STEP_TARGET_REPEAT_STEPS)
        {
            if (test_step->target_expression.target_elements[i].element_repeat_steps.num_repeat > 0)
            {
                test_step->target_expression.target_elements[i].element_repeat_steps.num_repeat--;
                tinfo->scenario->curr_executing_step_no =
                    test_step->target_expression.target_elements[i].element_repeat_steps.step_no - 1;
            }
        }
        for (j = 0; j < test_step->operation_list.num_operation; j++)
        {
            test_step->operation_list.step_operation[j].is_whole_op_failed = 0;
            for (k = 0; k < test_step->operation_list.step_operation[j].num_ids; k++)
            {
                test_step->operation_list.step_operation[j].is_sub_op_failed[k] = 0;
            }
        }
    }
    return KAPS_OK;
}

static kaps_status
run_dynamic_core_loop(
    struct xml_test *tinfo)
{
    int32_t step_no;
    int32_t total_step_counter = 0;
    int8_t i, db_id, instr_id;
    kaps_status status = KAPS_OK;

    if (!tinfo->flag_silent_steps)
    {
        print_dynamic_core_loop_scenario(stdout, tinfo->scenario);
    }
    init_all_steps_output_file_operations(tinfo);

    kaps_printf("****\n");
    kaps_printf("dynamic core loop, total no of steps to execute: %d\n", tinfo->scenario->num_test_steps);

    tinfo->scenario->curr_executing_step_no = 1;
    while (status == KAPS_OK && tinfo->scenario->curr_executing_step_no <= tinfo->scenario->num_test_steps)
    {
        step_no = tinfo->scenario->curr_executing_step_no - 1;

        reopen_all_steps_output_file(tinfo);
        kaps_printf("\n\nexecuting step: %d\n", step_no + 1);
        if (!tinfo->flag_silent_steps)
        {
            KAPS_STRY(print_dynamic_core_loop_step(stdout, tinfo->scenario, step_no));
        }
        kaps_fprintf(tinfo->scenario->all_steps_file, "<h3><br><br><br>Step %03d_%02d</h3>\n", total_step_counter + 1,
                     step_no + 1);
        kaps_fprintf(tinfo->scenario->all_steps_file, "<h4>\n", total_step_counter + 1, step_no + 1);
        KAPS_STRY(print_dynamic_core_loop_step(tinfo->scenario->all_steps_file, tinfo->scenario, step_no));
        kaps_fprintf(tinfo->scenario->all_steps_file, "</h4>\n");

        if (tinfo->scenario->test_steps[step_no].step_type == STEP_SPECIAL)
        {
            int32_t op_num;

            for (op_num = 0; op_num < tinfo->scenario->test_steps[step_no].special_step.num_operations; op_num++)
            {
                switch (tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].op_type)
                {
                    case SPECIAL_STEP_OPERATION_TYPE_WARMBOOT:
                        if (tinfo->flag_warmboot != 0)
                        {
                            kaps_printf("warmboot\n");
                            KAPS_STRY(perform_warmboot_operations(tinfo));
                        }
                        break;
                    case SPECIAL_STEP_OPERATION_TYPE_CRASH_RECOVERY:
                        if (tinfo->flag_crash_recovery != 0)
                        {
                            kaps_printf("dynamic crash recovery\n");
                            tinfo->can_crash = 1;
                            KAPS_STRY(recover_from_crash(tinfo));
                            tinfo->can_crash = 0;
                        }
                        break;
                    case SPECIAL_STEP_OPERATION_TYPE_INSTALL:
                        for (i = 0;
                             i <
                             tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].op_data.no_of_db;
                             i++)
                        {
                            db_id =
                                tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].
                                op_data.db_ids[i];
                            status = perform_install_on_db(tinfo, &tinfo->db_info_array[db_id]);
                            if (status != KAPS_OK)
                            {
                                if (tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].
                                    op_data.db_full[i] == 0)
                                {
                                    kaps_printf
                                        ("\t dynamic core loop, step no: %d, op_num: %d, db_idx: %d failed, kaps_status: %s\n",
                                         step_no + 1, op_num, i, kaps_get_status_string(status));
                                    kaps_fprintf(tinfo->scenario->all_steps_file,
                                                 "     dynamic core loop, step no: %d, op_num: %d, db_idx: %d failed, kaps_status: %s<br>",
                                                 step_no + 1, op_num, i, kaps_get_status_string(status));
                                    tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].
                                        op_data.db_full[i] = 1;

                                    tinfo->scenario->test_steps[step_no].special_step.step_operations[op_num].
                                        op_data.is_full_op_fail = 1;
                                    for (i = 0;
                                         i <
                                         tinfo->scenario->test_steps[step_no].special_step.
                                         step_operations[op_num].op_data.no_of_db; i++)
                                    {
                                        if (tinfo->scenario->test_steps[step_no].special_step.
                                            step_operations[op_num].op_data.db_full[i] == 0)
                                        {
                                            tinfo->scenario->test_steps[step_no].special_step.
                                                step_operations[op_num].op_data.is_full_op_fail = 0;
                                        }
                                    }
                                }

                                if ((status == KAPS_OUT_OF_DBA) || (status == KAPS_OUT_OF_UIT) ||
                                    (status == KAPS_OUT_OF_UDA) || (status == KAPS_EXHAUSTED_PCM_RESOURCE) ||
                                    (status == KAPS_OUT_OF_AD) || (status == KAPS_OUT_OF_INDEX))
                                {
                                    int32_t i;

                                    for (i = 0;
                                         i <
                                         tinfo->scenario->test_steps[step_no].special_step.
                                         step_operations[op_num].op_data.no_of_db; i++)
                                    {
                                        if (tinfo->scenario->test_steps[step_no].special_step.
                                            step_operations[op_num].op_data.is_full_op_fail == 0)
                                        {
                                            kaps_printf("\t\t continuing...\n");
                                            kaps_fprintf(tinfo->scenario->all_steps_file, "continuing...<br>");
                                            status = KAPS_OK;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
        else
        {
            Step_PreProcess(tinfo, &tinfo->scenario->test_steps[step_no]);
            tinfo->num_step_iterations = 0;
            while (status == KAPS_OK
                   && is_step_target_reached(tinfo, &tinfo->scenario->test_steps[step_no].target_expression) == 0)
            {
                int32_t index, op_num;

                index = kaps_random_r(tinfo->seedp) % tinfo->scenario->test_steps[step_no].operation_list.total_bias;

                op_num = 0;
                while (index >= tinfo->scenario->test_steps[step_no].operation_list.step_operation[op_num].bias)
                {
                    index -= tinfo->scenario->test_steps[step_no].operation_list.step_operation[op_num].bias;
                    op_num++;
                }

                if (tinfo->scenario->test_steps[step_no].operation_list.step_operation[op_num].is_whole_op_failed == 0)
                {
                    if (tinfo->scenario->test_steps[step_no].operation_list.step_operation[op_num].operation_type ==
                        TEST_WARMBOOT)
                    {
                        status = perform_warmboot_operations(tinfo);
                        if (status != KAPS_OK)
                        {
                            tinfo->scenario->test_steps[step_no].operation_list.
                                step_operation[op_num].is_whole_op_failed = 1;
                        }
                    }
                    else
                    {
                        for (i = 0;
                             i < tinfo->scenario->test_steps[step_no].operation_list.step_operation[op_num].num_ids;
                             i++)
                        {
                            if (tinfo->scenario->test_steps[step_no].operation_list.
                                step_operation[op_num].is_sub_op_failed[i] == 0)
                            {
                                db_id =
                                    tinfo->scenario->test_steps[step_no].operation_list.
                                    step_operation[op_num].db_instr_id[i];

                                switch (tinfo->scenario->test_steps[step_no].operation_list.
                                        step_operation[op_num].operation_type)
                                {
                                    case TEST_ADD:
                                        status = add_entry_to_database(tinfo, &tinfo->db_info_array[db_id]);
                                        break;
                                    case TEST_DELETE:
                                        status = delete_entry_from_database(tinfo, &tinfo->db_info_array[db_id]);
                                        break;
                                    case TEST_SEARCH:
                                        instr_id = db_id;
                                        status = perform_search(tinfo, &tinfo->instruction_info_array[instr_id]);
                                        break;
                                    case TEST_INSTALL:
                                        status = perform_install_on_db(tinfo, &tinfo->db_info_array[db_id]);
                                        break;
                                    case TEST_ADD_AND_INSTALL:
                                        status = add_entry_to_database(tinfo, &tinfo->db_info_array[db_id]);
                                        if (status == KAPS_OK)
                                        {
                                            status = perform_install_on_db(tinfo, &tinfo->db_info_array[db_id]);
                                        }
                                        break;
                                    default:
                                        kaps_sassert(0);
                                }

                                if (status != KAPS_OK)
                                {
                                    if (tinfo->scenario->test_steps[step_no].operation_list.
                                        step_operation[op_num].is_sub_op_failed[i] == 0)
                                    {
                                        kaps_printf
                                            ("\t dynamic core loop, step no: %d, op_num: %d, db_idx: %d failed, kaps_status: %s\n",
                                             step_no + 1, op_num, i, kaps_get_status_string(status));
                                        kaps_fprintf(tinfo->scenario->all_steps_file,
                                                     "     dynamic core loop, step no: %d, op_num: %d, db_idx: %d failed, kaps_status: %s<br>",
                                                     step_no + 1, op_num, i, kaps_get_status_string(status));
                                        tinfo->scenario->test_steps[step_no].operation_list.
                                            step_operation[op_num].is_sub_op_failed[i] = 1;

                                        tinfo->scenario->test_steps[step_no].operation_list.
                                            step_operation[op_num].is_whole_op_failed = 1;
                                        for (i = 0;
                                             i <
                                             tinfo->scenario->test_steps[step_no].operation_list.
                                             step_operation[op_num].num_ids; i++)
                                        {
                                            if (tinfo->scenario->test_steps[step_no].
                                                operation_list.step_operation[op_num].is_sub_op_failed[i] == 0)
                                            {
                                                tinfo->scenario->test_steps[step_no].
                                                    operation_list.step_operation[op_num].is_whole_op_failed = 0;
                                            }
                                        }
                                    }

                                    if ((status == KAPS_OUT_OF_DBA) || (status == KAPS_OUT_OF_UIT) ||
                                        (status == KAPS_OUT_OF_UDA) || (status == KAPS_EXHAUSTED_PCM_RESOURCE) ||
                                        (status == KAPS_OUT_OF_AD) || (status == KAPS_OUT_OF_INDEX))
                                    {
                                        int32_t i;

                                        status = KAPS_OK;

                                        for (i = 0;
                                             i < tinfo->scenario->test_steps[step_no].operation_list.num_operation; i++)
                                        {
                                            if (tinfo->scenario->test_steps[step_no].operation_list.
                                                step_operation[i].is_whole_op_failed == 0)
                                            {
                                                kaps_printf("\t\t continuing...\n");
                                                kaps_fprintf(tinfo->scenario->all_steps_file, "continuing...<br>");
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                tinfo->num_step_iterations++;
            }
        }
        print_html_step_summery(tinfo, total_step_counter + 1, step_no + 1);
        close_all_steps_output_file(tinfo);
        tinfo->scenario->curr_executing_step_no++;
        total_step_counter++;
    }

    finish_all_steps_output_file_operations(tinfo);

    /*
     * handle if DB FULL 
     */
    if ((status == KAPS_OUT_OF_DBA) || (status == KAPS_OUT_OF_UIT) || (status == KAPS_OUT_OF_UDA) ||
        (status == KAPS_EXHAUSTED_PCM_RESOURCE) || (status == KAPS_OUT_OF_AD) || (status == KAPS_OUT_OF_INDEX))
        status = KAPS_OK;

    /*
     * print stats for ref 
     */
    if (1)
    {
        uint32_t iter = 0;
        struct kaps_db_stats db_stats;

        kaps_printf("\n----------------------------------------------------------------------------");
        kaps_printf("\n-- Dynamic core loop databases stats");
        kaps_printf("\n ID  Entries (added:range_expansion)\n");
        for (iter = 0; iter < tinfo->num_databases; iter++)
        {
            if (tinfo->db_info_array[iter].clone_parent == NULL)
            {
                KAPS_TRY(kaps_db_stats(tinfo->db_info_array[iter].parse_info->db.db, &db_stats));
                kaps_printf("\n[%d]  %d  (%d %d)", iter,
                            (db_stats.num_entries + db_stats.range_expansion), db_stats.num_entries,
                            db_stats.range_expansion);
            }
        }
        kaps_printf("\n\n Dynamic Capacity: ");
        for (iter = 0; iter < tinfo->num_databases; iter++)
        {
            if (tinfo->db_info_array[iter].clone_parent == NULL)
            {
                KAPS_TRY(kaps_db_stats(tinfo->db_info_array[iter].parse_info->db.db, &db_stats));
                kaps_printf(" %d ", (db_stats.num_entries + db_stats.range_expansion));
            }
        }
        kaps_printf("\n----------------------------------------------------------------------------\n");
    }

    return status;
}

static kaps_status
custom_add_entry(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_parse_record *add)
{
    kaps_status status = KAPS_OK;
    struct kaps_db *db = db_info->parse_info->db.db;
    struct kaps_entry *e = NULL;

    if (db_info->parse_info->type == KAPS_DB_LPM)
    {
        status = kaps_db_add_prefix(db, add->data, add->length, &e);
        if (status == KAPS_OK)
        {
            add->e = e;
        }

        if (status == KAPS_OK && db_info->ad_db_info)
            status = add_ad_to_entry(tinfo, db_info, add, e);

    }

    return status;
}

static int32_t
get_next_index(
    struct xml_test *tinfo,
    int32_t cur_idx,
    uint32_t end_idx,
    int32_t step)
{
    static uint32_t init = 0;
    uint32_t size, i, idx1, idx2;
    int32_t tmp_val;
    static int32_t *rand_list, iter_idx = 0, iter_end = 0;

    if (step == 0)
    {
        if (init == 0)
        {
            kaps_sassert(cur_idx <= end_idx);
            size = end_idx - cur_idx;
            if (size == 0)
                return -1;
            rand_list = kaps_sysmalloc(size * sizeof(int32_t));
            for (i = 0; i < size; i++)
            {
                rand_list[i] = cur_idx + i + 1;
            }

            for (i = 0; i < size * 4; i++)
            {
                idx1 = kaps_random_r(tinfo->seedp) % size;
                idx2 = kaps_random_r(tinfo->seedp) % size;
                tmp_val = rand_list[idx1];
                rand_list[idx1] = rand_list[idx2];
                rand_list[idx2] = tmp_val;
            }
            iter_idx = 0;
            iter_end = size - 1;
            init = 1;
        }
        if (iter_idx > iter_end)
        {
            iter_idx = iter_end = 0;
            init = 0;
            return -1;
        }

        cur_idx = rand_list[iter_idx];
        iter_idx++;
    }
    else
    {
        cur_idx += step;

        if (step < 0)
        {
            if (cur_idx < end_idx)
                return -1;
        }
        else
        {
            if (cur_idx > end_idx)
                return -1;
        }
    }
    return cur_idx;
}

/* pass ops as 0 to record the start time */
static uint32_t
custom_get_ops_per_second(
    uint32_t ops)
{
    struct kaps_timeval tv = { 0 };
    static uint32_t start_time = 0, end_time = 0, time_taken = 0;
    uint32_t ops_sec = 0;

    if (ops == 0)
    {
        kaps_gettimeofday(&tv);
        start_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
        end_time = 0;
        time_taken = 0;
    }
    else
    {
        kaps_gettimeofday(&tv);
        end_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
        time_taken = end_time - start_time;
        if (time_taken == 0)
        {
            time_taken = 1;
        }

        ops_sec = (1000000ULL * (uint64_t) ops) / time_taken;
    }

    return ops_sec;
}

static kaps_status
custom_delete_entry(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_parse_record *entry)
{
    struct kaps_db *db = db_info->parse_info->db.db;
    kaps_status status = KAPS_OK;

    status = kaps_db_delete_entry(db, entry->e);
    if (status != KAPS_OK)
        kaps_printf("Error in deleting entry. Error = %s\n", kaps_get_status_string(status));

    clean_up_delete_entry(tinfo, db_info, entry);
    return status;
}

static kaps_status
custom_delete_entries(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    uint32_t start_idx,
    uint32_t end_idx,
    int32_t step)
{
    kaps_status status = KAPS_OK;
    struct kaps_db *db = db_info->parse_info->db.db;
    int32_t index;
    struct kaps_parse_record *add;

    if (start_idx > (db_info->nentries - 1) && end_idx > (db_info->nentries - 1))
        return status;

    if (end_idx > (db_info->nentries - 1))
        end_idx = db_info->nentries - 1;

    if (start_idx > (db_info->nentries - 1))
        start_idx = db_info->nentries - 1;

    index = start_idx;

    while (index != -1)
    {
        add = &db_info->entries[index];
        status = kaps_db_delete_entry(db, add->e);
        if (status != KAPS_OK)
            kaps_printf("Error in deleting entry index %d Error = %s\n", index, kaps_get_status_string(status));

        clean_up_delete_entry(tinfo, db_info, add);
        index = get_next_index(tinfo, index, end_idx, step);
    }
    return KAPS_OK;
}

static kaps_status
custom_delete_entries_one_db(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    uint32_t percent,
    uint32_t * ops_sec)
{

    struct kaps_db *db;
    struct kaps_db_stats db_stats;
    uint32_t entries_to_delete, total_deletes = 0;

    custom_get_ops_per_second(0);

    db = db_info->parse_info->db.db;
    kaps_sassert(((db_info->parse_info->type == KAPS_DB_LPM) && db_info->clone_parent == NULL));

    KAPS_TRY(kaps_db_stats(db, &db_stats));
    entries_to_delete = db_stats.num_entries * percent / 100;
    total_deletes += entries_to_delete;
    if (entries_to_delete)
        custom_delete_entries(tinfo, db_info, db_stats.num_entries - entries_to_delete, db_stats.num_entries - 1, 1);

    *ops_sec = custom_get_ops_per_second(total_deletes);

    return KAPS_OK;
}

static kaps_status
custom_delete_entries_db_by_db(
    struct xml_test *tinfo,
    uint32_t percent,
    uint32_t * ops_sec)
{
    struct test_db_info *db_info;
    struct kaps_db *db;
    struct kaps_db_stats db_stats;
    uint32_t i, entries_to_delete, total_deletes = 0;

    custom_get_ops_per_second(0);

    for (i = 0; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];
        db = db_info->parse_info->db.db;
        if ((db_info->parse_info->type == KAPS_DB_LPM) && db_info->clone_parent == NULL)
        {

            KAPS_TRY(kaps_db_stats(db, &db_stats));
            entries_to_delete = db_stats.num_entries * percent / 100;
            total_deletes += entries_to_delete;
            if (entries_to_delete)
                custom_delete_entries(tinfo, db_info, db_stats.num_entries - entries_to_delete,
                                      db_stats.num_entries - 1, 1);
            /*
             * KAPS_TRY(kaps_db_stats(db, &db_stats));
             */
        }
    }
    *ops_sec = custom_get_ops_per_second(total_deletes);

    return KAPS_OK;
}

/* del_order   0 - 0->n,   1 - n->0,  2 - random  */
/* del_mode 0 = 1 biy 1,  1 - capacity ratio */
static kaps_status
custom_delete_entries_roundrobin(
    struct xml_test *tinfo,
    uint32_t percent,
    uint32_t del_mode,
    uint32_t del_order,
    uint32_t * ops_sec)
{
    struct test_db_info *db_info;
    struct kaps_db_stats db_stats;
    uint32_t i, j, entries_to_delete[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t entries_deleted[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint8_t db_flag[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t db_idx[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t db_del_cnt[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t total_entries = 0, total_entries_to_delete = 0;
    uint32_t completed_db = 0;
    struct kaps_parse_record *entry;
    kaps_status status = KAPS_OK;

    kaps_sassert(tinfo->num_databases < KAPS_VERIFIER_MAX_DB_COUNT);

    if (del_mode == 1)
    {
        for (i = 0; i < tinfo->num_databases; i++)
        {
            db_info = &tinfo->db_info_array[i];
            if ((db_info->parse_info->type == KAPS_DB_LPM) && db_info->clone_parent == NULL)
            {

                total_entries += db_info->nentries;
            }
        }
    }

    /*
     * skip non lpm,acl databses 
     */
    for (i = 0; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];
        if ((db_info->parse_info->type != KAPS_DB_LPM) || db_info->clone_parent != NULL)
            db_flag[i] = 1;
        else
        {
            (void) kaps_db_stats(db_info->parse_info->db.db, &db_stats);

            entries_to_delete[i] = db_stats.num_entries * percent / 100;
            if (entries_to_delete[i] == 0)
            {
                db_flag[i] = 1;
                db_del_cnt[i] = 0;
                continue;
            }

            if (del_order == 0)
                db_idx[i] = db_stats.num_entries - entries_to_delete[i];
            else if (del_order == 1)
                db_idx[i] = db_stats.num_entries - 1;
            else
            {
                /*
                 * handle random mode here 
                 */
            }

            total_entries_to_delete += entries_to_delete[i];
            if (del_mode == 0)
            {
                db_del_cnt[i] = 1;
            }
            else
            {
                db_del_cnt[i] = db_info->nentries * 100 / total_entries;
                if (db_del_cnt[i] == 0)
                    db_del_cnt[i] = 1;
            }
        }
    }

    custom_get_ops_per_second(0);

    while (1)
    {
        completed_db = 0;
        for (i = 0; i < tinfo->num_databases; i++)
        {

            if (db_flag[i])
            {   /* Skip deletion if the db is already deleted for given % */
                completed_db++;
                continue;
            }

            db_info = &tinfo->db_info_array[i];

            for (j = 0; j < db_del_cnt[i]; j++)
            {
                if (entries_deleted[i] >= entries_to_delete[i])
                {       /* all entries already deleted */
                    db_flag[i] = 1;
                    completed_db++;
                    break;
                }

                entry = &db_info->entries[db_idx[i]];
                if (del_order == 0)
                    db_idx[i]++;
                else if (del_order == 1)
                    db_idx[i]--;
                else
                {
                    /*
                     * handle random mode 
                     */
                }

                status = custom_delete_entry(tinfo, db_info, entry);
                if (status != KAPS_OK)
                    kaps_printf("Error Deleteing Entry \n");
                entries_deleted[i]++;
            }

            if (db_flag[i])
                continue;

            if (entries_deleted[i] >= entries_to_delete[i])
            {   /* all entries already deleted */
                db_flag[i] = 1;
                completed_db++;
                continue;
            }
        }

        if (completed_db == tinfo->num_databases)
            break;
    }

    *ops_sec = custom_get_ops_per_second(total_entries_to_delete);

    return KAPS_OK;
}

static kaps_status
custom_add_entries_roundrobin_1(
    struct xml_test *tinfo,
    uint32_t * ops_sec,
    struct kaps_verifier_dbwise_table *dbwise,
    uint32_t del_db_idx)
{
    /*
     * Add Entries to all DBs in a round robin way, one at a time, till the dbs reach thier capacity/parse-only, or we
     * reach the unable to add entry condition is reached for the db. Print capacity
     */
    uint8_t db_flag[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t db_idx[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint32_t db_add_cnt[KAPS_VERIFIER_MAX_DB_COUNT] = { 0 };
    uint8_t completed_db = 0;
    uint32_t i, j;
    struct test_db_info *db_info;
    kaps_status status = KAPS_OK;
    struct kaps_parse_record *add;
    struct kaps_db_stats db_stats;
    static uint32_t dbg_count = 0;
    uint32_t total_entries = 0, total_adds = 0;

    /*
     * configurable param 
     */
    uint32_t add_mode = 0;      /* 0 - 1 at a time, 1 - percent based on capacity */

    if (tinfo->cm_add_mode)
        add_mode = tinfo->cm_add_mode - 1;

    kaps_sassert(tinfo->num_databases < KAPS_VERIFIER_MAX_DB_COUNT);

    if (add_mode == 1)
    {
        for (i = 0; i < tinfo->num_databases; i++)
        {
            db_info = &tinfo->db_info_array[i];
            if ((db_info->parse_info->type == KAPS_DB_LPM) && db_info->clone_parent == NULL)
            {

                total_entries += db_info->nentries;
            }
        }
    }

    /*
     * skip non lpm,acl databses. Also skip clone DBs 
     */
    for (i = 0; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];
        if ((db_info->parse_info->type != KAPS_DB_LPM) || db_info->clone_parent != NULL)
        {
            db_flag[i] = 1;

        }
        else
        {

            if (dbwise)
            {
                for (j = 0; j < dbwise->dbwise_db_count; j++)
                {
                    if (db_info == dbwise->db_info[j])
                    {
                        if (j == del_db_idx)
                        {
                            db_flag[i] = 1;     /* Do not add entries to the db where we are reducing now */
                        }
                        break;
                    }
                }

                if (j == dbwise->dbwise_db_count)
                    db_flag[i] = 1;     /* This db is not participating in dbwise add-remove test */
            }

            if (db_flag[i] == 0)
            {
                KAPS_TRY(kaps_db_stats(db_info->parse_info->db.db, &db_stats));
                db_idx[i] = db_stats.num_entries;
                if (add_mode == 0)
                {
                    db_add_cnt[i] = 1;
                }
                else
                {
                    db_add_cnt[i] = db_info->nentries * 100 / total_entries;
                    if (db_add_cnt[i] == 0)
                        db_add_cnt[i] = 1;
                }
            }
            /*
             * kaps_printf("db = %d add_count = %d\n", i, db_add_cnt[i]);
             */
        }
    }

    custom_get_ops_per_second(0);

    while (1)
    {
        completed_db = 0;
        for (i = 0; i < tinfo->num_databases; i++)
        {

            if (db_flag[i])
            {   /* Skip addition if the db is already full */
                completed_db++;
                continue;
            }

            db_info = &tinfo->db_info_array[i];

            for (j = 0; j < db_add_cnt[i]; j++)
            {
                if (db_idx[i] >= db_info->nentries)
                {       /* all entries already added */
                    db_flag[i] = 1;
                    completed_db++;
                    break;
                }

                add = &db_info->entries[db_idx[i]];
                db_idx[i]++;
                dbg_count++;
                status = custom_add_entry(tinfo, db_info, add);
                if (status == KAPS_OK)
                    status = kaps_db_install(db_info->parse_info->db.db);
                if (status != KAPS_OK)
                {

                    if (add->e
                        && (status == KAPS_OUT_OF_DBA
                            || status == KAPS_OUT_OF_UDA
                            || status == KAPS_OUT_OF_INDEX
                            || status == KAPS_OUT_OF_UIT
                            || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_AD))
                    {

                        KAPS_TRY(kaps_db_delete_entry(db_info->parse_info->db.db, add->e));

                        if (db_info->ad_db_info)
                        {
                            struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) add->ad_data;
                            if (ad_info->refcount > 0)
                                ad_info->refcount--;
                            if (!ad_info->refcount)
                            {
                                if (ad_info != db_info->zero_size_ad_info)
                                {
                                    KAPS_TRY(kaps_ad_db_delete_entry(ad_info->parse_ad_info->ad_db, ad_info->ad));
                                    free_ad(tinfo, db_info, ad_info);
                                }
                            }
                            add->ad_data = NULL;
                        }
                    }
                    db_idx[i]--;
                    db_flag[i] = 1;
                    completed_db++;
                    break;
                }
                else
                {
                    total_adds++;
                    add->status = FAST_TRIE_ACTIVE;
                }
            }
            if (db_flag[i])
                continue;

            if (db_idx[i] >= db_info->nentries)
            {   /* all entries already added */
                db_flag[i] = 1;
                completed_db++;
                continue;
            }
        }
        if (completed_db == tinfo->num_databases)
            break;
    }

    *ops_sec = custom_get_ops_per_second(total_adds);

    return status;
}

void
capacity_table_init(
    struct xml_test *tinfo,
    struct kaps_verifier_db_capacity_table *tbl)
{
    struct test_db_info *db_info;
    uint32_t i, width;

    kaps_memset(tbl, 0, sizeof(struct kaps_verifier_db_capacity_table));
    for (i = 0; i < tinfo->num_databases; i++)
    {
        db_info = &tinfo->db_info_array[i];
        if (db_info->parse_info->type == KAPS_DB_LPM)
        {
            tbl->type[tbl->num_db] = db_info->parse_info->type;
            width = db_info->parse_info->width_1;
            if (width <= 80)
                width = 80;
            else if (width <= 160)
                width = 160;
            else if (width <= 320)
                width = 320;
            else if (width <= 480)
                width = 480;
            else
                width = 640;
            tbl->db_width[tbl->num_db] = width;
            tbl->db_info[tbl->num_db] = db_info;
            tbl->db_id[tbl->num_db] = db_info->parse_info->id;
            tbl->num_db++;
        }
    }
    tbl->row = kaps_sysmalloc(sizeof(struct kaps_verifier_db_capacity_row) * tinfo->cm_iter);
    tbl->num_total_rows = tinfo->cm_iter;
    tbl->num_filled_rows = 0;
    tbl->tinfo = tinfo;
}

void
capacity_table_add(
    struct kaps_verifier_db_capacity_table *tbl,
    char *description,
    uint32_t add_ops)
{
    uint32_t i;
    struct kaps_db_stats db_stats;
    struct kaps_default_allocator_stats stats;

    if (tbl->num_filled_rows == tbl->num_total_rows)
    {
        kaps_printf("Reached max number of capacity points.\n");
        exit(0);
    }

    sal_strlcpy(tbl->row[tbl->num_filled_rows].row_label, description,
                sizeof(tbl->row[tbl->num_filled_rows].row_label));
    for (i = 0; i < tbl->num_db; i++)
    {
        (void) (kaps_db_stats(tbl->db_info[i]->parse_info->db.db, &db_stats));
        tbl->row[tbl->num_filled_rows].data[i] = db_stats.num_entries;
    }
    tbl->row[tbl->num_filled_rows].update_rate[0] = add_ops;

    kaps_default_allocator_get_stats(tbl->tinfo->alloc, &stats);
    tbl->row[tbl->num_filled_rows].heap_size = stats.peak_bytes / 1024 / 1024;

    tbl->num_filled_rows++;
}

void
capacity_table_update_delops(
    struct kaps_verifier_db_capacity_table *tbl,
    uint32_t ops)
{
    if (tbl->num_filled_rows == 0)
    {
        kaps_printf("table update_delops called before table_add.\n");
        exit(0);
    }

    tbl->row[tbl->num_filled_rows - 1].update_rate[1] = ops;
}

void
capacity_table_print(
    struct kaps_verifier_db_capacity_table *tbl)
{
    uint32_t i, j, start_idx;
    char div_line[512] = { 0 }, *dlp;
    uint32_t n_print_db = 0, n_remain_db = tbl->num_db;

    kaps_printf("\n");

    while (1)
    {
        if (n_remain_db == 0)
            break;

        if (n_remain_db <= 8)
            n_print_db = n_remain_db;
        else
            n_print_db = 8;

        n_remain_db -= n_print_db;

        kaps_memset(&div_line[0], 0, 512);

        dlp = &div_line[0];
        kaps_printf("\n\n|          |");
        sal_strlcpy(dlp, "|----------|", sizeof(div_line));
        dlp += 12;
        start_idx = tbl->num_db - n_remain_db - n_print_db;
        for (j = start_idx; j < (start_idx + n_print_db); j++)
        {
            if (tbl->type[j] == KAPS_DB_LPM)
                kaps_printf(" LPM %3u   |", tbl->db_width[j]);
            else
                kaps_printf(" ACL %3u   |", tbl->db_width[j]);
            sal_strlcpy(dlp, "-----------|", sizeof(div_line));
            dlp += 12;
        }

        /*
         * Print the DB numbers 
         */
        kaps_printf("\n|          |");
        for (j = start_idx; j < (start_idx + n_print_db); j++)
        {
            kaps_printf(" DB  %3u   |", tbl->db_id[j]);
        }

        if (n_remain_db == 0)
        {

            kaps_printf("  Add ops/sec |  Del Ops/sec |  Heap in MB |\n");
            sal_strlcpy(dlp, "--------------|--------------|-------------|", sizeof(div_line));
            dlp += 30;
            kaps_printf("%s\n", &div_line[0]);

            for (i = 0; i < tbl->num_filled_rows; i++)
            {
                kaps_printf("|%10s", tbl->row[i].row_label);
                for (j = start_idx; j < (start_idx + n_print_db); j++)
                {
                    kaps_printf("|%10d ", tbl->row[i].data[j]);
                }
                kaps_printf("| %12d | %13d| %12d", tbl->row[i].update_rate[0], tbl->row[i].update_rate[1],
                            tbl->row[i].heap_size);
                kaps_printf("|");
                if (tbl->row[i].update_rate[0] < tbl->add_update_rate_limit_1)
                    kaps_printf("+");
                if (tbl->row[i].update_rate[0] < tbl->add_update_rate_limit_2)
                    kaps_printf("+");

                if (tbl->row[i].update_rate[1] < tbl->del_update_rate_limit_1)
                    kaps_printf("-");
                if (tbl->row[i].update_rate[1] < tbl->del_update_rate_limit_2)
                    kaps_printf("-");

                kaps_printf("\n");

            }
        }
        else
        {

            kaps_printf("\n");
            kaps_printf("%s\n", &div_line[0]);

            for (i = 0; i < tbl->num_filled_rows; i++)
            {
                kaps_printf("|%10s", tbl->row[i].row_label);
                for (j = start_idx; j < (start_idx + n_print_db); j++)
                {
                    kaps_printf("|%10d ", tbl->row[i].data[j]);
                }
                kaps_printf("|\n");
            }

        }
        kaps_printf("%s\n", &div_line[0]);
    }
}

void
capacity_table_dtor(
    struct kaps_verifier_db_capacity_table *tbl)
{
    kaps_sysfree(tbl->row);
}

void
capacity_table_compute_limits(
    struct xml_test *tinfo,
    struct kaps_verifier_db_capacity_table *tbl)
{
    uint32_t i;
    uint32_t min_update_rate_add = 0xFFFFFFFF, max_update_rate_add = 0;
    uint32_t min_update_rate_del = 0xFFFFFFFF, max_update_rate_del = 0;
    const uint32_t update_rate_outlier_tolerance = 60;  /* upto 20% of the entries can have upto 60% deviation from max 
                                                         * update rate */
    uint32_t min_heap = 0xFFFFFFFF, max_heap = 0;

    for (i = 0; i < tbl->num_filled_rows; i++)
    {

        if (i > 0)
        {
            /*
             * skip the update rate of first iteration. On Analysis it was found that in places with many mallocs the
             * system time taken for the first iteration is higher the nthe other iterations. Hence skipping the first
             * iteration in the check to avoid the OS issue 
             */

            if (min_update_rate_add > tbl->row[i].update_rate[0])
                min_update_rate_add = tbl->row[i].update_rate[0];

            if (max_update_rate_add < tbl->row[i].update_rate[0])
                max_update_rate_add = tbl->row[i].update_rate[0];

            if (min_update_rate_del > tbl->row[i].update_rate[1])
                min_update_rate_del = tbl->row[i].update_rate[1];

            if (max_update_rate_del < tbl->row[i].update_rate[1])
                max_update_rate_del = tbl->row[i].update_rate[1];
        }
    }

    for (i = 0; i < tbl->num_filled_rows; i++)
    {

        if (i > 0)
        {
            /*
             * skip the update rate of first iteration. On Analysis it was found that in places with many mallocs the
             * system time taken for the first iteration is higher the nthe other iterations. Hence skipping the first
             * iteration in the check to avoid the OS issue 
             */

            if (min_update_rate_add > tbl->row[i].update_rate[0])
                min_update_rate_add = tbl->row[i].update_rate[0];

            if (max_update_rate_add < tbl->row[i].update_rate[0])
                max_update_rate_add = tbl->row[i].update_rate[0];

            if (min_update_rate_del > tbl->row[i].update_rate[1])
                min_update_rate_del = tbl->row[i].update_rate[1];

            if (max_update_rate_del < tbl->row[i].update_rate[1])
                max_update_rate_del = tbl->row[i].update_rate[1];

        }

        if (min_heap > tbl->row[i].heap_size)
            min_heap = tbl->row[i].heap_size;

        if (max_heap < tbl->row[i].heap_size)
            max_heap = tbl->row[i].heap_size;

    }

    /*
     * calculate the threshold limits 
     */
    tbl->add_update_rate_limit_1 = max_update_rate_add * (100 - tinfo->cm_update_tolerance) / 100;
    tbl->add_update_rate_limit_2 = max_update_rate_add * (100 - update_rate_outlier_tolerance) / 100;
    tbl->del_update_rate_limit_1 = max_update_rate_del * (100 - tinfo->cm_update_tolerance) / 100;
    tbl->del_update_rate_limit_2 = max_update_rate_del * (100 - update_rate_outlier_tolerance) / 100;

}

uint32_t
custom_verify_capacity_table(
    struct xml_test *tinfo,
    struct kaps_verifier_db_capacity_table *tbl)
{
    uint32_t i, j;
    uint32_t updadd_1_cnt = 0, updadd_2_cnt = 0, upddel_1_cnt = 0, upddel_2_cnt = 0;
    uint32_t min_capacity[KAPS_VERIFIER_MAX_DB_COUNT], max_capacity[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t ret_value = 1, desc_size = 0;

    kaps_memset(tinfo->cm_error_description, 0, 1024);

    for (i = 0; i < KAPS_VERIFIER_MAX_DB_COUNT; i++)
    {
        min_capacity[i] = 0xFFFFFFFF;
        max_capacity[i] = 0;
    }

    for (i = 0; i < tbl->num_filled_rows; i++)
    {

        if (i > 0)
        {
            if (tbl->row[i].update_rate[0] >= tbl->add_update_rate_limit_1)
                updadd_1_cnt++;
            else if (tbl->row[i].update_rate[0] >= tbl->add_update_rate_limit_2)
                updadd_2_cnt++;
            else
            {
            }

            if (tbl->row[i].update_rate[1] >= tbl->del_update_rate_limit_1)
                upddel_1_cnt++;
            else if (tbl->row[i].update_rate[1] >= tbl->del_update_rate_limit_2)
                upddel_2_cnt++;
            else
            {
            }

            for (j = 0; j < tbl->num_db; j++)
            {
                if (min_capacity[j] > tbl->row[i].data[j])
                    min_capacity[j] = tbl->row[i].data[j];

                if (max_capacity[j] < tbl->row[i].data[j])
                    max_capacity[j] = tbl->row[i].data[j];

            }

        }

    }
    kaps_printf("up_add_limit1 = %u up_add_limit2 = %u up_del_limit1 = %u up_del_limit2 = %u\n",
                tbl->add_update_rate_limit_1, tbl->add_update_rate_limit_2, tbl->del_update_rate_limit_1,
                tbl->del_update_rate_limit_2);
    kaps_printf("up_add_1 = %u up_add_2 = %u up_del_1 = %u up_del_2 = %u\n", updadd_1_cnt, updadd_2_cnt, upddel_1_cnt,
                upddel_2_cnt);
    if ((updadd_1_cnt + updadd_2_cnt) != (tbl->num_filled_rows - 1))
    {   /* some upd rate was below add_update_rate_limit_2 */
        desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
        if (desc_size < 700)
        {
            kaps_sprintf(tinfo->cm_error_description + desc_size, "entries(++) exceed add upd outlier limit.\\n");
            tinfo->is_custom_error_description = 1;
        }
        ret_value = 0;

    }
    else if ((updadd_1_cnt * 100 / (tbl->num_filled_rows - 1)) < 80)
    {

        desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
        if (desc_size < 700)
        {
            kaps_sprintf(tinfo->cm_error_description + desc_size, ">20%% entries(+) exceed add upd limit.\\n");
            tinfo->is_custom_error_description = 1;
        }
        ret_value = 0;

    }
    else
    {
    }

    if ((upddel_1_cnt + upddel_2_cnt) != (tbl->num_filled_rows - 1))
    {   /* some upd rate was below del_update_rate_limit_2 */
        desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
        if (desc_size < 700)
        {
            kaps_sprintf(tinfo->cm_error_description + desc_size, "entries(--) exceed del upd outlier limit.\\n");
            tinfo->is_custom_error_description = 1;
        }
        ret_value = 0;

    }
    else if ((upddel_1_cnt * 100 / (tbl->num_filled_rows - 1)) < 80)
    {
        desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
        if (desc_size < 700)
        {
            kaps_sprintf(tinfo->cm_error_description + desc_size, ">20%% entries(-)exceed del upd limit.\\n");
            tinfo->is_custom_error_description = 1;
        }
        ret_value = 0;
    }

    for (j = 0; j < tbl->num_db; j++)
    {
        if (max_capacity[j]
            && ((max_capacity[j] - min_capacity[j]) * 100 / max_capacity[j]) > tinfo->cm_capacity_tolerance)
        {
            desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
            if (desc_size < 700)
            {
                kaps_sprintf(tinfo->cm_error_description + desc_size, "DB%u Capacity - %u%% (%u%%)\\n", tbl->db_id[j],
                             ((max_capacity[j] - min_capacity[j]) * 100 / max_capacity[j]),
                             tinfo->cm_capacity_tolerance);
                tinfo->is_custom_error_description = 1;
            }
            ret_value = 0;
        }
    }

    if (tbl->max_heap && ((tbl->max_heap - tbl->min_heap) * 100 / tbl->max_heap) > tinfo->cm_memory_tolerance)
    {
        desc_size = sal_strnlen(tinfo->cm_error_description, sizeof(tinfo->cm_error_description));
        if (desc_size < 700)
        {
            kaps_sprintf(tinfo->cm_error_description + desc_size, "Heap Memory - %u%% (%u%%)\\n",
                         ((tbl->max_heap - tbl->min_heap) * 100 / tbl->max_heap), tinfo->cm_memory_tolerance);
            tinfo->is_custom_error_description = 1;
        }
        ret_value = 0;
    }

    return ret_value;
}

kaps_status
custom_perform_warmboot_operation(
    struct xml_test * tinfo)
{
    tinfo->block_warmboot_reconcile = 1;
    KAPS_STRY(perform_warmboot_operations(tinfo));
    tinfo->block_warmboot_reconcile = 0;

    return KAPS_OK;
}

void
cm_compose_custom_description(
    struct xml_test *tinfo,
    struct kaps_verifier_db_capacity_table *tbl)
{
    uint32_t j = 0, idx = 0;

    tinfo->cm_description[0] = '\0';
    /*
     * First add the ad info 
     */
    for (j = 0; j < tbl->num_db; j++)
    {
        idx = sal_strnlen(tinfo->cm_description, sizeof(tinfo->cm_error_description));
        if (tbl->type[j] == KAPS_DB_LPM)
            kaps_sprintf(&tinfo->cm_description[idx], "LPM_%u ", tbl->db_width[j]);
        else
            kaps_sprintf(&tinfo->cm_description[idx], "ACL_%u ", tbl->db_width[j]);
    }
}

static kaps_status
run_custom_core_loop(
    struct xml_test *tinfo)
{
    uint32_t iter = 0;
    uint32_t mode = tinfo->custom_mode;
    kaps_status status = KAPS_OK;

    (void) status;

    kaps_printf("\n***************\n  Custom Core Loop\n");

    /*
     * Some cleanup 
     */
    tinfo->flag_use_pivot = 0;

    if (mode == 11)
    {
        /*
         * params 
         */
        /*
         * num_iter - number of iterations 
         */
        /*
         * percent - percent of entries to delete per iteration 
         */
        /*
         * dump_log - device dump needed after each iteration? 
         */

        uint32_t num_iters = tinfo->cm_iter;
        uint32_t percent = tinfo->cm_percent;
        uint32_t dump_html = tinfo->cm_dump_html;
        uint32_t del_mode = tinfo->cm_del_mode;
        uint32_t del_order = tinfo->cm_del_order;
        char desc[2048];
        FILE *fp1;
        struct kaps_verifier_db_capacity_table tbl;
        uint32_t ops_sec = 0;
        char *scenario_desc;
        uint32_t warmboot_iter_add = 0, warmboot_iter_del = 0;
        struct kaps_verifier_dbwise_table dbwise;
        uint32_t dbwise_del_idx = 0;

        kaps_memset(&dbwise, 0, sizeof(struct kaps_verifier_dbwise_table));

        capacity_table_init(tinfo, &tbl);

        kaps_device_get_property(tinfo->device, KAPS_DEVICE_PROP_DESCRIPTION, &scenario_desc);
        if (scenario_desc && sal_strncmp(scenario_desc, "[custom]", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {

            tinfo->is_custom_description = 1;
            cm_compose_custom_description(tinfo, &tbl);
            kaps_printf(" - Custom Description: %s\n", tinfo->cm_description);
        }

        if (tinfo->flag_warmboot)
        {
            warmboot_iter_add = kaps_random() % num_iters;
            warmboot_iter_del = kaps_random() % num_iters;
        }

        dbwise_del_idx = dbwise.dbwise_db_count - 1;

        for (iter = 0; iter < num_iters; iter++)
        {
            kaps_printf("Adding Entries ...  ");
            sal_fflush(stdout);

            /*
             * in the first iteration use regular add to add to all dbs 
             */
            if (tinfo->cm_dbwise_add_remove && iter > 0)
            {
                custom_add_entries_roundrobin_1(tinfo, &ops_sec, &dbwise, dbwise_del_idx);

            }
            else
                custom_add_entries_roundrobin_1(tinfo, &ops_sec, NULL, 0);
            kaps_printf("(ops/sec %u)\n", ops_sec);

            kaps_sprintf(desc, "iter%04d", iter);
            capacity_table_add(&tbl, desc, ops_sec);

            if (dump_html == 1 && iter == (num_iters - 1))
            {

                kaps_snprintf(desc, 2048, "%s/summary_after.html", tinfo->flag_output_dir);
                fp1 = kaps_fopen(desc, "w");
                if (fp1)
                {
                    kaps_device_print_html(tinfo->device, fp1);
                    kaps_fclose(fp1);
                }
            }
            else if (dump_html == 2)
            {
                kaps_snprintf(desc, 2048, "%s/iter%04d.html", tinfo->flag_output_dir, iter);
                fp1 = kaps_fopen(desc, "w");
                if (fp1)
                {
                    kaps_device_print_html(tinfo->device, fp1);
                    kaps_fclose(fp1);
                }
            }

            tinfo->pass = 1;

            if (tinfo->flag_warmboot && iter == warmboot_iter_add)
            {
                kaps_printf("Performing Warmboot ...");
                sal_fflush(stdout);
                KAPS_STRY(custom_perform_warmboot_operation(tinfo));
                kaps_printf("Done\n");
            }

            kaps_printf("Deleting Entries ...");
            sal_fflush(stdout);

            if (tinfo->cm_dbwise_add_remove)
            {
                dbwise_del_idx = (dbwise_del_idx + 1) % dbwise.dbwise_db_count;
                custom_delete_entries_one_db(tinfo, dbwise.db_info[dbwise_del_idx],
                                             dbwise.dbwise_db_percent[dbwise_del_idx], &ops_sec);
            }
            else
            {
                if (del_mode == 2)
                    custom_delete_entries_db_by_db(tinfo, percent, &ops_sec);
                else
                    custom_delete_entries_roundrobin(tinfo, percent, del_mode, del_order, &ops_sec);
            }

            kaps_printf("(ops/sec %u)\n", ops_sec);
            capacity_table_update_delops(&tbl, ops_sec);

            if (tinfo->flag_warmboot && iter == warmboot_iter_del)
            {
                kaps_printf("Performing Warmboot ...");
                sal_fflush(stdout);
                KAPS_STRY(custom_perform_warmboot_operation(tinfo));
                kaps_printf("Done\n");
            }
        }

        capacity_table_compute_limits(tinfo, &tbl);
        capacity_table_print(&tbl);

        if (status == KAPS_OK)
        {
            uint32_t p_result = 0;
            if (tinfo->cm_dbwise_add_remove == 0)
                p_result = custom_verify_capacity_table(tinfo, &tbl);
            else
                p_result = 1;

            if (p_result == 0)
            {
                tinfo->pass = 0;
                kaps_assert((tinfo->is_custom_error_description == 1), "Custom verify discrepancy");
            }
        }

        capacity_table_dtor(&tbl);

    }
    return KAPS_OK;
}

static kaps_status
search_the_database(
    struct xml_test *tinfo,
    struct test_db_info *db,
    char *str)
{
    uint32_t to_search = (tinfo->flag_num_iterations > db->nadds) ? db->nadds : tinfo->flag_num_iterations;
    uint32_t progress = to_search / PROGRESS_BAR_FACTOR;
    struct kaps_verifier_search_record *search_rec = tinfo->expected_results;
    uint32_t max_searches_possible = 1024;
    kaps_status status = KAPS_OK;

    tinfo->cur_num_searches = 0;
    tinfo->num_iterations = 0;

    if (search_rec)
        kaps_memset(search_rec, 0, max_searches_possible * sizeof(struct kaps_verifier_search_record));

    if (db->clone_parent)
        to_search =
            (tinfo->flag_num_iterations >
             db->clone_parent->nadds) ? db->clone_parent->nadds : tinfo->flag_num_iterations;

    if (progress == 0)
        progress = to_search;

    tinfo->target_db = db;

    if (db->ninstructions)
    {
        uint32_t instno = 0;

        while (db->nsearches < to_search)
        {

            if (instno >= db->ninstructions)
                instno = 0;

            tinfo->target_inst = &tinfo->instruction_info_array[db->db_inst_info[instno]];

            tinfo->num_iterations++;
            status = perform_search(tinfo, tinfo->target_inst);

            if (tinfo->flag_fail_on_continue && status == KAPS_MISMATCH)
            {
                status = KAPS_OK;
            }
            else if (status != KAPS_OK)
            {
                return status;
            }

            if (!(db->nsearches % progress) || (db->nsearches == to_search))
            {
                if (tinfo->print_num_searches)
                {
                    kaps_printf(" %s %s-%d : %d\r", str,
                                kaps_device_db_name(db->parse_info->db.db), db->parse_info->db.db->tid, db->nsearches);
                    sal_fflush(stdout);
                }
                if (db->nsearches == to_search)
                    KAPS_TRY(validate_hb_entries(tinfo, db, 1));
                else
                    KAPS_TRY(validate_hb_entries(tinfo, db, 0));
            }

            instno++;
        }
        if (tinfo->print_num_searches)
            kaps_printf("\n");
    }

    return KAPS_OK;
}

kaps_status
do_sequential_search(
    struct xml_test * tinfo)
{
    /*
     * make sure the searches are fine initially 
     */
    uint32_t count = 0, db_no = 0;
    kaps_status status = KAPS_OK;

    db_no = 0;
    if (tinfo->flag_num_iterations)
    {
        kaps_printf("\n - Searching the databases\n");
        while (db_no < tinfo->num_databases)
        {
            struct test_db_info *parent_test_db = &tinfo->db_info_array[db_no];
            struct xml_parse_info *parent = NULL, *tab = NULL;
            uint32_t total = 0;

            kaps_sassert(parent_test_db->container_db == NULL);
            parent = parent_test_db->parse_info;

            for (tab = parent; tab; tab = tab->next_tab)
            {
                struct test_db_info *db = get_test_info_for_xml_db(tinfo, tab);
                struct xml_parse_info *ctab = NULL;

                if (db == NULL)
                {
                    status = KAPS_INTERNAL_ERROR;
                    return status;
                }
                db->nsearches = 0;
                if (tab == parent)
                    KAPS_STRY(search_the_database(tinfo, db, " - Total Searches for "));
                else
                    KAPS_STRY(search_the_database(tinfo, db, "       - Table Total Searches for "));
                total++;
                for (ctab = tab->clone; ctab; ctab = ctab->next)
                {
                    struct test_db_info *db = get_test_info_for_xml_db(tinfo, ctab);

                    db->nsearches = 0;
                    if (tab == parent)
                        KAPS_STRY(search_the_database(tinfo, db, "       - Clone Total Searches for "));
                    else
                        KAPS_STRY(search_the_database(tinfo, db, "              - Clone Total Searches for "));
                    total++;
                }
            }
            db_no += total;

        }
    }
    count++;

    return status;
}

static kaps_status
run_sequential_core_loop(
    struct xml_test *tinfo)
{
    uint32_t print = 0, db_no = 0;
    kaps_status status = KAPS_OK;

    /*
     * If we are having multiple dbs in the add-del input dataset file, then the first dataset itself is sufficient
     * since it will have the combined operations of both databases and we will not be parsing the second dataset. In
     * this case set this variable to 1
     */
    uint32_t is_add_del_ops_of_multiple_dbs = tinfo->is_add_del_ops_of_multiple_dbs;

    /*
     * Adding Entries to the Database 
     */
    db_no = 0;
    while (db_no < tinfo->num_databases)
    {
        uint32_t total = 0;
        struct xml_parse_info *parent = NULL, *tab = NULL;
        struct test_db_info *db_info = &tinfo->db_info_array[db_no];

        if (db_info->parse_info->db.db->common_info->dummy_db)
        {
            db_no++;
            continue;
        }

        kaps_sassert(db_info->container_db == NULL);
        tinfo->n_status = KAPS_OK;

        tinfo->target_db = db_info;
        parent = db_info->parse_info;

        for (tab = parent; tab; tab = tab->next_tab)
        {
            struct xml_parse_info *ctab = NULL;
            total++;
            for (ctab = tab->clone; ctab; ctab = ctab->next)
                total++;
        }

        if (!db_info->parse_info->incremental)
        {
            db_no += total;
            continue;
        }

        if (!print)
        {
            kaps_printf("\n - Populating the databases\n");
            print = 1;
        }

        KAPS_STRY(populate_the_database(tinfo));
        db_no += total;

        if (is_add_del_ops_of_multiple_dbs)
            break;
    }
    tinfo->n_status = KAPS_OK;

    KAPS_TRY(print_html_sw_state_and_device_dump(tinfo));

#if 0
    if (tinfo->flag_warmboot)
    {
        kaps_printf("\n WB oper after populating dbs\n");
        KAPS_STRY(perform_warmboot_operations(tinfo));
    }
#endif

    if (tinfo->xpt_type == KAPS_VERIFIER_MDB_XPT && tinfo->does_xpt_support_search)
    {
        dnx_sbusdma_desc_wait_done(tinfo->unit);
    }

    KAPS_STRY(do_sequential_search(tinfo));

    if (tinfo->device->id == KAPS_JERICHO_2_DEVICE_ID && tinfo->xpt_type == KAPS_VERIFIER_KAPS_MODEL_XPT)
    {
        kaps_printf(" - Verifying the KAPS SER Data \n");
        status = verify_kaps_ser_data(tinfo->device);

        if (status != KAPS_OK)
            kaps_assert(0, "ERROR verifying the KAPS SER Data \n");
    }

    if (tinfo->flag_verbose)
    {
        uint32_t is_first = 1;
        struct xml_parse_info *tmp = tinfo->xml_parse_info;

        while (tmp)
        {
            xml_print_stats(tinfo, tmp, is_first);
            tmp = tmp->next;
            is_first = 0;
        }
    }


    if (tinfo->flag_skip_delete_all == 0)
    {
        /*
         * delete the entries from the databases 
         */
        kaps_printf("\n - Deleting the entries in databases\n");
        db_no = 0;
        while (db_no < tinfo->num_databases)
        {
            struct test_db_info *db_info = &tinfo->db_info_array[db_no];
            struct xml_parse_info *tab = NULL, *parent = db_info->parse_info;
            struct kaps_timeval tv;
            uint32_t start_time, end_time, time_taken;
            uint32_t ndeletes = 0, total = 0;

            for (tab = parent; tab; tab = tab->next_tab)
            {
                struct xml_parse_info *ctab = NULL;

                total++;
                for (ctab = tab->clone; ctab; ctab = ctab->next)
                    total++;
            }

            kaps_gettimeofday(&tv);
            start_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
            for (tab = parent; tab; tab = tab->next_tab)
            {

                tinfo->target_db = get_test_info_for_xml_db(tinfo, tab);
                while (tinfo->target_db->ndeletes < tinfo->target_db->nadds)
                {
                    KAPS_TRY(select_db_and_delete_entry_from_itself(tinfo));
                }
                ndeletes += tinfo->target_db->ndeletes;
            }
            kaps_gettimeofday(&tv);
            end_time = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
            time_taken = end_time - start_time;
            if (time_taken == 0)
            {
                time_taken = 1;
            }

            kaps_printf(" - Total deletes for %s-%2d: %6d    ", kaps_device_db_name(db_info->parse_info->db.db),
                        db_info->parse_info->db.db->tid, ndeletes);
            kaps_printf("ops/sec : %llu \n", (1000000ULL * (uint64_t) ndeletes) / time_taken);
            db_no += total;
        }
    }
    else
        kaps_printf("Skipping Delete all entries due to -skip_delete_all option \n");

    kaps_printf("\n");

#if 0
    if (tinfo->flag_warmboot)
        kaps_printf("\n WB oper after deleting all db entries\n");
    KAPS_STRY(perform_warmboot_operations(tinfo));
}
#endif

return status;
}

#if TO_BRINGUP
void
kaps_verifier_sig_handler(
    int signo)
{
    siglongjmp(kaps_verifier_env, -1);
}

void *
kaps_verifier_create_crash(
    void *arg)
{
    while (1)
    {
        struct xml_test *tinfo = (struct xml_test *) arg;

        sleep(5);

        if (tinfo->can_crash)
        {
            tinfo->can_crash = 0;
            pthread_kill(tinfo->parent_thread_id, SIGUSR2);
        }
    }
    return NULL;
}
#endif

static kaps_status
run_test_loop(
    struct xml_test *tinfo)
{
    if (tinfo->flag_inner_loop == 0)
        return KAPS_OK;

    if (tinfo->flag_random == 1)
    {
        if (tinfo->flag_num_iterations == 0)
            return KAPS_OK;
    }

    if (tinfo->flag_crash_recovery)
    {
        KAPS_TRY(kaps_device_start_transaction(tinfo->device));
    }

    KAPS_TRY(allocate_memory_to_ix_dbs(tinfo));

    /*
     * Create the fast trie for search verification
     */
    KAPS_TRY(create_search_trees(tinfo));

    if (tinfo->flag_verbose)
    {
        switch (tinfo->flag_random)
        {
            case 0:
                kaps_printf(" - Starting sequential core loop ");
                break;
            case 1:
                kaps_printf(" - Starting random core loop ");
                break;
            case 2:
                kaps_printf(" - Starting dynamic testing core loop ");
                break;
            case 9:
                kaps_printf(" - Starting hybrid testing core loop ");
                break;
            default:
                break;
        }
    }

    /*
     * Start coherency testing by holding back PIOs
     */
    if (tinfo->flag_coherency)
    {

        if (tinfo->flag_verbose)
            kaps_printf("Coherency enabled\n");
    }
    else
    {
        if (tinfo->flag_verbose)
            kaps_printf("Coherency disabled\n");
    }

    tinfo->can_crash = 0;

#if TO_BRINGUP
    if (tinfo->flag_crash_recovery > 2)
    {
        int err = 0;
        pthread_t tid;
        struct sigaction sighandle;

        tinfo->parent_thread_id = pthread_self();
        sighandle.sa_handler = kaps_verifier_sig_handler;
        sigemptyset(&sighandle.sa_mask);
        sighandle.sa_flags = 0;
        sigaction(SIGUSR2, &sighandle, NULL);

        err = pthread_create(&tid, NULL, &kaps_verifier_create_crash, tinfo);
        if (err != 0)
            kaps_printf("\nNot able to create the thread\n");

        if (sigsetjmp(kaps_verifier_env, 1) != 0)
            KAPS_TRY(recover_from_crash(tinfo));
    }
#endif

    /*
     * as of now we cant pass WB and CR flags simultaneously, either of the operations is performed once we have
     * support, we will enabled both, and randomly do any operation 
     */
    if (tinfo->flag_warmboot)
    {
        kaps_printf("  -> First Warmboot Operation [After device_lock; Where the DB are not populated] \n");
        KAPS_STRY(perform_warmboot_operations(tinfo));
    }
    else if (tinfo->flag_crash_recovery)
    {
        kaps_printf("  ->First Crash Recovery Operation  [After device_lock; Where the DB are not populated] \n");
        KAPS_STRY(recover_from_crash(tinfo));
    }

    KAPS_STRY(preadd_the_default_entries(tinfo));
    KAPS_STRY(preadd_the_entries_for_batch_mode(tinfo));

    tinfo->can_crash = 1;
    if (tinfo->flag_random == 0)
    {
        KAPS_STRY(run_sequential_core_loop(tinfo));
    }
    else if (tinfo->flag_random == 1)
    {
        KAPS_STRY(run_random_core_loop(tinfo));
    }
    else if (tinfo->flag_random == 2)
    {
        KAPS_STRY(run_dynamic_core_loop(tinfo));
    }
    else if (tinfo->flag_random == 3)
    {
        KAPS_STRY(run_custom_core_loop(tinfo));
    }

    kaps_printf(" - Core loop ended\n");

    if (tinfo->flag_crash_recovery)
    {
        KAPS_TRY(kaps_device_end_transaction(tinfo->device));
    }

    return KAPS_OK;
}

kaps_status
prepare_xpt(
    struct xml_test * tinfo,
    uint32_t flags,
    struct kaps_allocator * model_alloc,
    uint32_t verbose,
    uint32_t dev_no,
    void **xpt_p,
    kaps_nlm_allocator * al)
{
    void *xpt = NULL, *t_xpt = NULL;

    if (tinfo->xpt_type == KAPS_VERIFIER_MDB_XPT)
    {
        /*
         * We have to use DNX Simulation Model since KAPS Model is not available.
         * Searches are not supported by DNX Simulation Model
         */
        if (SAL_BOOT_PLISIM)
        {
            tinfo->does_xpt_support_search = 0;
        }
    }

    if (tinfo->xpt_type == KAPS_VERIFIER_BLACKHOLE_XPT)
    {
        /*
         * black hole 
         */
        if (verbose)
            kaps_printf("Blackhole\n");
        xpt = NULL;
        tinfo->flag_use_pivot = 1;
        tinfo->does_xpt_support_search = 0;
    }
    else if (tinfo->xpt_type == KAPS_VERIFIER_KAPS_MODEL_XPT)
    {
        /*
         * C-Model 
         */
        struct kaps_sw_model_kaps_config *config = NULL;

        if (verbose)
            kaps_printf("C-Model\n");

        tinfo->flag_use_pivot = 1;

        if (tinfo->flag_device == KAPS_DEVICE_KAPS)
        {
            if (verbose)
                kaps_printf(" - Device Type: KAPS_DEVICE_KAPS    ");

            config = tinfo->mdl_config;
            if (config && verbose)
                get_kaps_device_types(config->id, config->sub_type);
        }

        /*
         * Since KAPS C-Model is not available, use blackhole mode
         */
        tinfo->xpt_type = KAPS_VERIFIER_BLACKHOLE_XPT;
        tinfo->does_xpt_support_search = 0;

    }
    else if (tinfo->xpt_type == KAPS_VERIFIER_MDB_XPT)
    {
        tinfo->mdb_xpt.mdb_lpm_xpt.device_type = KAPS_DEVICE_KAPS;

        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_search = mdb_lpm_xpt_search;
        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_register_read = mdb_lpm_xpt_register_read;
        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_command = mdb_lpm_xpt_command;
        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_register_write = mdb_lpm_xpt_register_write;
        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_hb_read = mdb_lpm_xpt_hb_read;
        tinfo->mdb_xpt.mdb_lpm_xpt.kaps_hb_write = mdb_lpm_xpt_hb_write;

        xpt = &tinfo->mdb_xpt;

        if (SAL_BOOT_PLISIM)
        {
            tinfo->does_xpt_support_search = 0;
        }
        else
        {
            tinfo->does_xpt_support_search = 1;
        }
    }
    else
    {
        kaps_printf("Unknown\n");
        return KAPS_INTERNAL_ERROR;
    }

    tinfo->does_xpt_support_replication = 0;
    if (tinfo->xpt_type == KAPS_VERIFIER_MDB_XPT && !SAL_BOOT_PLISIM)
    {
        tinfo->does_xpt_support_replication = 1;
    }

    if (tinfo->flag_coherency)
    {
#if TO_BRINGUP
        if (tinfo->flag_device == KAPS_DEVICE_KAPS)
        {
            KAPS_TRY(kaps_verifier_kaps_xpt_init(tinfo, xpt, &t_xpt));
        }
#endif
    }
    else
    {
        t_xpt = xpt;
    }
    *xpt_p = t_xpt;
    return KAPS_OK;

}

kaps_status
kaps_verifier_device_set_property(
    struct xml_test * tinfo)
{
    kaps_status status = KAPS_OK;
    struct xml_parse_info *xml_parse_info = NULL;
    struct xml_inst_info *xml_inst_info = NULL;
    char *scenario_desc;

    tinfo->pass = 0;

    if (tinfo->config)
    {

        KAPS_VERIFIER_ALLOC_MEM_ERR(tinfo, (kaps_device_init(tinfo->alloc, tinfo->flag_device, tinfo->flags,
                                                             tinfo->xpt_pp[0], tinfo->config, &tinfo->device)));

    }
    else
    {

        KAPS_VERIFIER_ALLOC_MEM_ERR(tinfo, (kaps_device_init(tinfo->alloc, tinfo->flag_device, tinfo->flags,
                                                             tinfo->xpt_pp[0], NULL, &tinfo->device)));

    }

    KAPS_TRY(kaps_device_set_property(tinfo->device, (enum kaps_device_properties) KAPS_DEVICE_PROP_ADV_PRINT, 2));

    if (tinfo->flag_crash_recovery)
    {
        if (1)
        {
            tinfo->nv_size = 320 * 1024 * 1024;
            tinfo->issu_nv_memory = (uint8_t *) kaps_sysmalloc(tinfo->nv_size);
            if (!tinfo->issu_nv_memory)
            {
                kaps_printf("NV Memory alloc failed\n");
                return KAPS_OUT_OF_MEMORY;
            }
        }
        else
        {
            if (tinfo->flag_output_dir)
            {
                char op_filename[100];

                sal_strlcpy(op_filename, tinfo->flag_output_dir, sizeof(op_filename));
                sal_strncat_s(op_filename, ".dump", sizeof(op_filename));
                tinfo->issu_dump_file = kaps_fopen(op_filename, "w+");
            }
            else
            {
                tinfo->issu_dump_file = kaps_fopen("kaps_verifier_crash_wb.dump", "w+");
            }
        }
        KAPS_TRY(kaps_device_set_property
                 (tinfo->device, KAPS_DEVICE_PROP_CRASH_RECOVERY, 1, (void *) tinfo->issu_nv_memory, tinfo->nv_size));
    }

    tinfo->device_init_flags = tinfo->flags;
    if (tinfo->flag_random == 2)
    {
        status = xml_parse_dynamic_core_loop_scenario(tinfo->test_step_config_file_name, &tinfo->scenario);
        kaps_sassert(status == KAPS_OK);
    }
    status = xml_parse_scenarios(tinfo->input_xml_file_name, tinfo->device,
                                 tinfo->does_xpt_support_replication, &xml_parse_info, &xml_inst_info);

    tinfo->xml_parse_info = xml_parse_info;
    tinfo->xml_inst_info = xml_inst_info;

    if (status != KAPS_OK)
    {
        kaps_printf(" - Failed in XML parse scenario\n");
        kaps_printf(" - Result: XML parse scenario returned %s\n", kaps_get_status_string(status));
        tinfo->flag_error_status = 1;
        return status;
    }

    kaps_printf(" - XML Parse: OK\n");

    /*
     * Create a flat list of databases and instructions for easy access during testing
     */
    KAPS_VERIFIER_ALLOC_ERR(tinfo, (create_test_info(tinfo, tinfo->xml_parse_info, tinfo->xml_inst_info)));

    if (tinfo->device->type == KAPS_DEVICE_KAPS)
    {
        int32_t i;

        for (i = 0; i < tinfo->num_databases; i++)
        {
            struct test_db_info *db_info = &tinfo->db_info_array[i];

            /*
             * Set to all DB, if flag enabled [for the ACL (lpm_map_acl) set the prop DEFER_DELETES] 
             */
            if (tinfo->flag_defer_deletes)
            {
                status = kaps_db_set_property(db_info->parse_info->db.db, KAPS_PROP_DEFER_DELETES, 1);
                if (status != KAPS_OK)
                {
                    kaps_printf(" - Failed to set db property for deferring the deletes \n");
                    tinfo->flag_error_status = 1;
                    return status;
                }
                else
                    kaps_printf(" --- Setting DB property for deferring the deletes DB[%d]\n", i);
            }
            else
            {   /* just read if any DB set the prop during parse print it */
                uint32_t is_defer_del = 0;

                status = kaps_db_get_property(db_info->parse_info->db.db, KAPS_PROP_DEFER_DELETES, &is_defer_del);
                if (status != KAPS_OK)
                {
                    kaps_printf(" - Failed to get db property for deferring the deletes \n");
                    tinfo->flag_error_status = 1;
                    return status;
                }
                if (is_defer_del)
                {
                    kaps_printf(" --- Setting DB property for deferring the deletes DB[%d]\n", i);
                }
            }
        }
    }

    /*
     * Install our own callback function for the databases
     * that do not have AD stored on the chip. At some point we potentially
     * will install callbacks even for these databases to ensure
     * we can manage counters externally. For KAPS, never enable this.
     */
    {
        int32_t i;
        uint8_t last_callback_num = 0;

        for (i = 0; i < tinfo->num_databases; i++)
        {
            struct test_db_info *db_info = &tinfo->db_info_array[i];

            if (db_info->ad_db_info && tinfo->device->type != KAPS_DEVICE_KAPS)
                continue;

            if (!tinfo->flag_coherency)
                continue;

            if (db_info->container_db)
            {
                db_info->ix_callback_num = last_callback_num;
                db_info->callback_enabled = 1;
                continue;
            }

            if (tinfo->flag_coherency)
            {
                status = kaps_db_set_property(db_info->parse_info->db.db,
                                              KAPS_PROP_INDEX_CALLBACK,
                                              kaps_verifier_coherency_index_change_callback, tinfo);
                if (status != KAPS_OK)
                {
                    kaps_printf(" - Failed to install index callback\n");
                    break;
                }
                db_info->callback_enabled = 1;
                db_info->ix_callback_num = 0;
                last_callback_num = 0;
                continue;
            }

        }
    }

    kaps_printf("\n - Log Output Dir: %s\n", tinfo->flag_output_dir ? tinfo->flag_output_dir : "None");

    if (status == KAPS_OK)
    {
        if (tinfo->flag_device_dump_out && tinfo->flag_output_dir)
        {
            char buf[512] = { 0, };
            FILE *dev_file;
            if (kaps_snprintf(buf, sizeof(buf), "%s/before_dev_lock.xml", tinfo->flag_output_dir) < 0)
            {
                kaps_sassert(0);
            }
            dev_file = kaps_open_file(buf);

            if (dev_file != NULL)
            {
                (void) kaps_device_dump(tinfo->device, 0, dev_file);
                kaps_fclose(dev_file);
            }
            else
            {
                kaps_sassert(0);
            }
        }

        status = kaps_device_lock(tinfo->device);

        if (tinfo->flag_device_dump_out && tinfo->flag_output_dir)
        {
            char buf[512] = { 0, };
            FILE *dev_file;
            if (kaps_snprintf(buf, sizeof(buf), "%s/after_dev_lock.xml", tinfo->flag_output_dir) < 0)
            {
                kaps_sassert(0);
            }
            dev_file = kaps_open_file(buf);
            if (dev_file != NULL)
            {
                (void) kaps_device_dump(tinfo->device, 0, dev_file);
                kaps_fclose(dev_file);
            }
            else
            {
                kaps_sassert(0);
            }
        }
    }

    kaps_device_get_property(tinfo->device, KAPS_DEVICE_PROP_DESCRIPTION, &scenario_desc);

    kaps_printf(" - Device Lock: %s ", kaps_get_status_string(status));

    kaps_printf("\n");
    kaps_printf(" - Description (DNX KAPS): %s\n", scenario_desc ? scenario_desc : "None");

    print_html_output_file(tinfo, PRINT_BEFORE);
    print_sw_state(tinfo);

    if (status != KAPS_OK)
    {
        tinfo->flag_error_status = 1;
        return status;
    }


    /*
     * Read in the input datasets, or generate the datasets on the fly 
     */
    KAPS_VERIFIER_ALLOC_ERR(tinfo, (parse_datasets(tinfo)));

    distribute_ops_among_all_databases(tinfo);

    sal_fflush(stdout);

    return status;
}

static kaps_status
kaps_verifier_print_device_info(
    struct xml_test *tinfo)
{
    struct xml_parse_info *tmp;
    kaps_status status = KAPS_OK;
    uint32_t is_first;

    tmp = tinfo->xml_parse_info;

    if (tinfo->flag_random)
    {
        is_first = 1;
        while (tmp)
        {
            xml_print_stats(tinfo, tmp, is_first);
            tmp = tmp->next;
            is_first = 0;
        }
    }

    if (tinfo->flag_coherency)
    {
        kaps_printf("PIO OPERATIONS: %u\n", tinfo->num_pio);
    }

    if (tinfo->num_issu)
    {
        kaps_printf("Warmboot OPERATIONS: %u\n", tinfo->num_issu);
        kaps_printf("Warmboot Peak NV Size:%3.2f MB\n", (((float) tinfo->peak_dump_size) / (1024 * 1024)));
        kaps_printf("Warmboot Peak NV Ops: %u\n", tinfo->peak_file_ops);
        kaps_printf("Warmboot Peak Save Time: %3.2f secs\n", (((float) tinfo->peak_issu_save_time) / (1000000)));
        kaps_printf("Warmboot Peak Restore Time: %3.2f secs\n", (((float) tinfo->peak_issu_restore_time) / (1000000)));
    }

    if (tinfo->flag_crash_recovery)
    {
        kaps_cr_pool_mgr_get_stats(tinfo->device);
    }

    {
        int32_t i;

        for (i = 0; i < tinfo->num_instructions; i++)
        {
            if (tinfo->instruction_info_array[i].nfail)
            {
                tinfo->flag_error_status = 1;
                return status;
            }
        }
    }

    if (tinfo->flag_expected_fail == 0)
    {
        tinfo->pass = 1;
    }

    return status;
}

/**
 * Main step of processing the input XML file. Multiple
 * actions are taken within this function which spans
 * the complete kaps_verifier functionality
 */
static kaps_status
process_file(
    struct xml_test *tinfo)
{
    char *fname = tinfo->input_xml_file_name;

    struct kaps_allocator *alloc = NULL, *model_alloc = NULL;
    kaps_nlm_allocator al;
    kaps_status status = KAPS_OK;
    void *t_xpt = NULL;
    uint32_t flags, i, swap = 0;

    flags = KAPS_DEVICE_DEFAULT;

    if (tinfo->flag_warmboot)
        flags |= KAPS_DEVICE_ISSU;

    KAPS_TRY(kaps_default_allocator_create(&alloc));

    if (tinfo->flag_seed == -1)
    {
        uint32_t stime;
        struct kaps_timeval tv;

        kaps_gettimeofday(&tv);
        stime = tv.tv_sec * 1000000 + tv.tv_ns / 1000;
        kaps_srandom(stime);
        tinfo->flag_seed_reuse = stime;
    }
    else
    {
        tinfo->flag_seed_reuse = tinfo->flag_seed;
        kaps_srandom(tinfo->flag_seed);
    }

    /*
     * Print the XML file, we can take out storing the xml file backup in nightly regression 
     */
    if (tinfo->flag_verbose == 2)
    {
        FILE *fptr;
        int c;

        fptr = kaps_fopen(fname, "r");
        if (fptr == NULL)
        {
            kaps_printf("Cannot open file %s \n", fname);
            goto alloc_error;
        }
        kaps_printf("\n\n _____________________________________________________________________\n");
        kaps_printf("\n xml file name: %s   (can used for recreate)\n\n", fname);

        /*
         * Read contents from file and print on console 
         */
        c = sal_fgetc(fptr);
        while (c != EOF)
        {
            kaps_fprintf(stdout, "%c", c);
            c = sal_fgetc(fptr);
        }
        kaps_printf("\n\n _____________________________________________________________________\n\n\n");
        kaps_fclose(fptr);
    }

    kaps_printf("\n----------------------------------------------------------\n");
    kaps_printf(" - Input: %s\n", fname);

    kaps_printf(" - Transport: ");

    switch (tinfo->flag_device)
    {

        case KAPS_DEVICE_KAPS:
            kaps_printf("[Device_Type: KAPS] ");
            break;
        default:
            kaps_printf("[Device_Type: Unknown] ");
            break;
    }

    status = kaps_default_allocator_create(&model_alloc);
    if (status != KAPS_OK)
        goto alloc_error;

    status = prepare_xpt(tinfo, flags, model_alloc, 1, 0, &t_xpt, &al);
    if (status != KAPS_OK)
        goto alloc_error;

    tinfo->xpt = t_xpt;
    tinfo->xpt_pp[0] = t_xpt;

    for (i = 1; i < tinfo->num_bc_devices; i++)
    {
        status = prepare_xpt(tinfo, flags, model_alloc, 0, i, &t_xpt, &al);
        if (status != KAPS_OK)
            goto alloc_error;
        tinfo->xpt_pp[i] = t_xpt;
    }

    tinfo->seedp = &tinfo->flag_seed;
    tinfo->model_alloc = model_alloc;
    tinfo->alloc = alloc;
    tinfo->al = &al;
    tinfo->flags = flags;

    /*
     *  Initialize the device and set the device related properties and values.
     */
    status = kaps_verifier_device_set_property(tinfo);
    if (status != KAPS_OK)
    {
        if (tinfo->flag_expected_fail != 0)
            goto alloc_error;
        else if (tinfo->flag_error_status == 1)
        {
            goto error;
        }
        else if (tinfo->flag_error_status == 2)
            goto alloc_error;
        else
            return status;
    }

    if (tinfo->flag_skip_delete_all == 2)
    {
        tinfo->flag_skip_delete_all = 0;
        if (kaps_random_r(tinfo->seedp) % 3 == 2)
        {
            tinfo->flag_skip_delete_all = 1;
        }
    }

    tinfo->flag_skip_delete_all = 0;

    /*
     * Run the core test loop
     */
    status = run_test_loop(tinfo);

    if (status != KAPS_OK)
    {
        kaps_printf("Failed: %s\n", kaps_get_status_string(status));
        if (tinfo->flag_ISSU)
        {
            if (tinfo->flag_error_status == 2)
                goto alloc_error;
            else
                goto error;
        }
        else
            goto error;
    }

    kaps_verifier_print_device_info(tinfo);

    if (status != KAPS_OK)
    {
        goto error;
    }

error:

    if (tinfo->flag_random)
        if (tinfo->flag_random != 3)    /* skip this to prevent overwriting of summary after in case of custom core
                                         * loop */
            KAPS_TRY(print_html_sw_state_and_device_dump(tinfo));

    tinfo->pio_entries = NULL;
    tinfo->do_not_perform_xpt_searches = 1;


    if (!tinfo->flag_skip_delete_all)
    {
        if (tinfo->db_info_array)
        {
            int32_t i, iter;

            for (i = (tinfo->num_databases - 1); i >= 0; i--)
            {
                kaps_status status = 0;

                if (!tinfo->db_info_array[i].clone_parent
                    && tinfo->db_info_array[i].parse_info->db.db->common_info->defer_deletes_to_install)
                {

                    status = kaps_db_install(tinfo->db_info_array[i].parse_info->db.db);
                    if (status != KAPS_OK)
                        kaps_printf("++++ DB install (defer_deletes_to_install) failed, before delete ALL \n");
                    kaps_printf("   --- last install for DB before deleteAll for deferred deletes \n");
                }
            }

            for (i = (tinfo->num_databases - 1); i >= 0; i--)
            {
                kaps_status status;

                if (tinfo->db_info_array[i].clone_parent)
                {
                    if (tinfo->db_info_array[i].db_inst_info)
                        kaps_sysfree(tinfo->db_info_array[i].db_inst_info);
                    if (tinfo->db_info_array[i].gmask && tinfo->db_info_array[i].fast_trie)
                    {
                        kaps_sysfree(tinfo->db_info_array[i].gmask);
                        fast_test_trie_destroy(tinfo->db_info_array[i].fast_trie);
                    }
                    continue;
                }

                {

                    status = kaps_db_delete_all_entries(tinfo->db_info_array[i].parse_info->db.db);
                    if (status != KAPS_OK && status != KAPS_DEVICE_UNLOCKED)
                        kaps_printf("++++ Delete all entries failed\n");
                    if (tinfo->db_info_array[i].container_db == NULL)
                    {
                        if (tinfo->db_info_array[i].ad_db_info)
                        {
                            struct kaps_parse_ad_info *tmp = tinfo->db_info_array[i].ad_db_info;

                            while (tmp)
                            {
                                if (swap == 0)
                                {
                                    status = kaps_ad_db_delete_all_entries(tmp->ad_db);
                                    swap = 1;
                                }
                                else
                                {
                                    status = kaps_ad_db_delete_unused_entries(tmp->ad_db);
                                    swap = 0;
                                }
                                if (status != KAPS_OK)
                                    kaps_printf("++++ AD DB delete all entries failed\n");
                                tmp = tmp->next;
                            }
                        }

                        if (tinfo->num_bc_devices > 1)
                        {
                            for (iter = 0; iter < tinfo->num_bc_devices; iter++)
                            {
                                if (tinfo->db_info_array[i].parse_info->bc_dbs
                                    && tinfo->db_info_array[i].parse_info->bc_dbs[iter]
                                    && tinfo->db_info_array[i].parse_info->db.db !=
                                    tinfo->db_info_array[i].parse_info->bc_dbs[iter])
                                    status = kaps_db_destroy(tinfo->db_info_array[i].parse_info->bc_dbs[iter]);
                                if (status != KAPS_OK)
                                    kaps_printf("++++ Broadcast DB destroy failed\n");
                            }
                        }

                        status = kaps_db_destroy(tinfo->db_info_array[i].parse_info->db.db);
                        if (status != KAPS_OK)
                            kaps_printf("++++ DB destroy failed\n");

                        if (tinfo->db_info_array[i].ad_db_info)
                        {
                            struct kaps_parse_ad_info *tmp = tinfo->db_info_array[i].ad_db_info;

                            while (tmp)
                            {
                                status = kaps_ad_db_destroy(tmp->ad_db);
                                if (status != KAPS_OK)
                                    kaps_printf("++++ AD DB destroy failed\n");
                                tmp = tmp->next;
                            }
                        }
                        if (tinfo->db_info_array[i].parse_info->hb_db_info)
                        {
                            status = kaps_hb_db_destroy(tinfo->db_info_array[i].parse_info->hb_db_info->hb_db);
                            if (status != KAPS_OK)
                                kaps_printf("++++ HB DB destroy failed\n");
                        }
                    }
                }

                if (tinfo->db_info_array[i].fast_trie)
                    fast_test_trie_destroy(tinfo->db_info_array[i].fast_trie);

                if (tinfo->db_info_array[i].ad_db_info && tinfo->flag_inner_loop)
                {
                    struct test_db_info *db_info = &tinfo->db_info_array[i];

                    for (iter = 0; iter < db_info->nentries; iter++)
                    {
                        struct kaps_parse_record *rec = &db_info->entries[iter];
                        struct kaps_verifier_ad_info *ad_info = (struct kaps_verifier_ad_info *) rec->ad_data;

                        if (rec->visited)
                        {
                            if (ad_info && ad_info->refcount)
                                ad_info->refcount--;
                            if (rec->ad_data && !ad_info->refcount)
                            {
                                if (rec->ad_data == tinfo->db_info_array[i].zero_size_ad_info)
                                {
                                    tinfo->db_info_array[i].zero_size_ad_info = NULL;
                                }

                                kaps_sysfree(rec->ad_data);
                                rec->ad_data = NULL;
                            }
                        }
                    }
                }
                kaps_parse_destroy(tinfo->db_info_array[i].entries);
                if (tinfo->db_info_array[i].db_inst_info)
                    kaps_sysfree(tinfo->db_info_array[i].db_inst_info);
                if (tinfo->db_info_array[i].file_key != tinfo->db_info_array[i].parse_info->db_key)
                {
                    kaps_parse_destroy_key(tinfo->db_info_array[i].file_key);
                }
                if (tinfo->flag_use_pivot)
                {
                    kaps_sysfree(tinfo->db_info_array[i].indices);
                }
                if (tinfo->db_info_array[i].e_to_parse_record)
                {
                    kaps_sysfree(tinfo->db_info_array[i].e_to_parse_record);
                }

                if (tinfo->db_info_array[i].zero_size_ad_info)
                {
                    kaps_sysfree(tinfo->db_info_array[i].zero_size_ad_info);
                }
            }
            free_ix_dbs(tinfo);
            kaps_sysfree(tinfo->db_info_array);
        }

        if (tinfo->instruction_info_array)
        {
            int32_t i;

            for (i = 0; i < tinfo->num_instructions; i++)
            {
                kaps_status status;

                status = kaps_instruction_destroy(tinfo->instruction_info_array[i].info->instruction);
                if (status != KAPS_OK)
                    kaps_printf("++++ Instruction destroy failed\n");
            }
            kaps_sysfree(tinfo->instruction_info_array);
        }

        free_all_ad(tinfo);
        tinfo->num_databases = 0;
        tinfo->num_instructions = 0;
        tinfo->db_info_array = NULL;
        tinfo->instruction_info_array = NULL;

        xml_parse_destroy(tinfo->xml_parse_info, tinfo->xml_inst_info);

        {
            struct kaps_device *tmp = tinfo->device->next_bc_device;

            while (tmp)
            {
                struct kaps_device *tmp_next_device = tmp->next_bc_device;


                tmp = tmp_next_device;
            }
        }
    }

    KAPS_TRY(kaps_device_destroy(tinfo->device));

    tinfo->can_crash = 0;

    if (tinfo->flag_coherency == 0)
    {

    }

alloc_error:

    if (tinfo->flag_verbose)
        xml_print_heap_info(tinfo, alloc);

    kaps_printf("\n Total __SEARCHES: %d \n\n", tinfo->overall_srch_count);

    (void) kaps_default_allocator_destroy(alloc);

    if (model_alloc)
        (void) kaps_default_allocator_destroy(model_alloc);

    if (!tinfo->pass)
    {
        if (tinfo->is_custom_error_description)
        {
            kaps_printf("FAIL_REASON: %s\n", tinfo->cm_error_description);
            kaps_printf("RESULT: FAIL\n");
        }
        else if (tinfo->flag_expected_fail
                 &&
                 (!sal_strncmp
                  (tinfo->flag_expected_fail, kaps_get_status_string(status),
                   SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
        {
            kaps_printf("RESULT: EXPECTED FAIL\n");
            kaps_printf("----------------------------------------------------------\n");
        }
        else
        {
            kaps_printf("RESULT: FAIL\n");
            kaps_printf("----------------------------------------------------------\n");
        }
    }
    else
    {
        if (tinfo->capacity_problem_present)
        {
            kaps_printf("PROBLEM WITH CAPACITY\n");
            kaps_printf("RESULT: FAIL\n");
            kaps_printf("----------------------------------------------------------\n");
        }
        else if (tinfo->db_stats_problem_present)
        {
            kaps_printf("PROBLEM WITH DB_STATS\n");
            kaps_printf("RESULT: FAIL\n");
            kaps_printf("----------------------------------------------------------\n");
        }
        else
        {
            kaps_printf("RESULT: PASS\n");
            kaps_printf("----------------------------------------------------------\n");
        }
    }

    if (tinfo->vector_fp)
        kaps_fclose(tinfo->vector_fp);

    if (tinfo->flag_crash_recovery)
    {
        if (1)
        {
            kaps_sysfree(tinfo->issu_nv_memory);
            tinfo->issu_nv_memory = NULL;
            tinfo->nv_size = 0;
        }
        else
        {
            kaps_fclose(tinfo->issu_dump_file);
        }
    }

    return status;
}

void
print_date_and_time(
    void)
{
#if TO_BRINGUP
    struct kaps_timeval tv;
    time_t now_time;
    struct tm *nowtm;
    char tmp_buf[64];

    kaps_gettimeofday(&tv);
    now_time = tv.tv_sec;
    nowtm = localtime(&now_time);
    strftime(tmp_buf, sizeof tmp_buf, "%d-%b-%Y %H:%M:%S", nowtm);
    kaps_printf("%s\n", tmp_buf);
#endif
}

#if TO_BRINGUP
static void
hdl(
    int sig,
    siginfo_t * siginfo,
    void *context)
{
    kaps_printf("\nRESULT: Timeout\n");
    kaps_printf("\n - Test End Time: ");
    print_date_and_time();
    sal_fflush(stdout);
    exit(0);
}
#endif

/**
 * Main entry point
 * @retval 0 on success
 * @retval non zero on error
 */
shr_error_e
verifier_start(
    int unit,
    struct kaps_verifier_input_params *input_params_ptr)
{
    kaps_status cur_status = KAPS_OK;
    struct xml_test *test_info;
    struct kaps_sw_model_kaps_config mdl_config;  /**< C-model Config */

    SHR_FUNC_INIT_VARS(unit);

#if TO_BRINGUP
    kaps_printf("\nMachine details:\n");
    system("cat /proc/cpuinfo | grep \"model name\\|MHz\" | head -2; cat /proc/meminfo | grep MemTotal");
#endif

    kaps_printf("\n");
    kaps_printf("------------------------------------------\n");
    kaps_printf("         %s                               \n", kaps_device_get_sdk_version());
    kaps_printf("------------------------------------------\n\n");

    kaps_printf(" - Test Start Time: ");
    print_date_and_time();
    test_info = kaps_syscalloc(1, sizeof(*test_info));
    if (!test_info)
    {
        kaps_printf("Out of memory\n");
        exit(1);
    }

#if TO_BRINGUP
    {
        struct sigaction act;

        /*
         * Set up signal handler 
         */
        kaps_memset(&act, '\0', sizeof(act));

        act.sa_sigaction = &hdl;

        /*
         * The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. 
         */
        act.sa_flags = SA_SIGINFO;

        if (sigaction(SIGTERM, &act, NULL) < 0)
        {
            perror("sigaction");
            return 1;
        }
    }
#endif

    test_info->unit = unit;

    /*
     * Default flags 
     */
    test_info->capacity_tolerance = 10;
    test_info->strict_capacity_checking = 1;

    test_info->baseline_accuracy = 100;
    test_info->overall_srch_count = 0;
    /*
     * test_info->device_file_name = NULL; 
     */
    test_info->device_file_name[0] = 0;
    test_info->target_db = NULL;

    test_info->flag_device = KAPS_DEVICE_KAPS;
    test_info->flag_num_iterations = 10000;
    test_info->flag_seed = 1234;
    test_info->xpt_type = KAPS_VERIFIER_KAPS_MODEL_XPT;
    test_info->flag_coherency = 0;
    test_info->flag_inner_loop = 1;
    test_info->flag_warmboot = 0;
    test_info->flag_random = 1;

    test_info->flag_gen_unique = 0;
    test_info->flag_verbose = 1;

    test_info->flag_crash_recovery = 0;
    test_info->flag_defer_deletes = 0;
    test_info->flag_ISSU = 0;
    test_info->flag_error_status = 0;

    test_info->flag_silent_steps = 1;

    test_info->n_status = KAPS_OK;
    test_info->num_bc_devices = 1;
    test_info->cur_num_searches = 0;
    test_info->global_est_min_capacity = -1;

    test_info->print_num_searches = 1;

    test_info->flag_fail_on_continue = 0;
    test_info->flag_device_dump_out = 0;

    test_info->nentries_to_add = -1;
    test_info->cm_capacity_tolerance = 5;
    test_info->cm_update_tolerance = 20;
    test_info->cm_memory_tolerance = 5;
    test_info->cm_percent = 100;
    test_info->cm_add_mode = 0;
    test_info->cm_del_mode = 0;
    test_info->cm_del_order = 1;
    test_info->cm_iter = 10;

    test_info->ch_initial_num_entries = 5000;

    test_info->flag_skip_delete_all = 2;

    test_info->maintain_e_to_parse_record_table = 1;

    kaps_memset(&mdl_config, 0, sizeof(struct kaps_sw_model_kaps_config));

    /*
     * Join the $SDK path with XML Path and the device config file name to get the full path of the device
     * configuration XML file
     */
    if (input_params_ptr->device_xml && input_params_ptr->device_xml[0] != 0)
    {
        SHR_IF_ERR_EXIT(dbx_file_get_sdk_path(unit,
                                              input_params_ptr->device_xml,
                                              CTEST_DNX_KAPS_INPUT_XML_FILE_PATH, test_info->device_file_name, TRUE));
    }

    /*
     * Join the $SDK path with XML Path and the input test XML file name to get the full path of the input test XML
     * file 
     */
    SHR_IF_ERR_EXIT(dbx_file_get_sdk_path(unit,
                                          input_params_ptr->input_xml,
                                          CTEST_DNX_KAPS_INPUT_XML_FILE_PATH, test_info->input_xml_file_name, TRUE));

    test_info->flag_random = input_params_ptr->coreloop_type;
    test_info->flag_warmboot = input_params_ptr->warmboot_type;
    test_info->xpt_type = input_params_ptr->xpt_type;
    test_info->flag_num_iterations = input_params_ptr->num_iterations;
    test_info->cmodel_device_type = input_params_ptr->sw_model_dev_id;
    test_info->cmodel_sub_type = input_params_ptr->sw_model_sub_type;
    test_info->cmodel_profile = input_params_ptr->sw_model_profile;

    test_info->vector_file_name = input_params_ptr->vector_file_name;

    if (test_info->flag_device == KAPS_DEVICE_UNKNOWN)
    {
        kaps_printf("Device type needs to be specified\n");
    }

    if (test_info->vector_file_name[0])
    {
        test_info->vector_fp = kaps_open_file(test_info->vector_file_name);
        if (test_info->vector_fp == NULL)
        {
            kaps_printf("Could not open vector file: %s\n", test_info->vector_file_name);
        }
    }

    if (test_info->flag_device == KAPS_DEVICE_KAPS)
    {
        mdl_config.id = 1;
        mdl_config.profile = 0;
    }

    if (sal_strncmp(test_info->device_file_name, "", sizeof(test_info->device_file_name)))
    {
        cur_status = xml_parse_device_config_file(test_info->device_file_name, test_info->flag_device, &mdl_config);
        if (cur_status != KAPS_OK)
        {
            kaps_printf("FAILED: %s\n", kaps_get_status_string(cur_status));
            exit(1);
        }
    }
    else
    {
        mdl_config.id = input_params_ptr->sw_model_dev_id;
        mdl_config.sub_type = input_params_ptr->sw_model_sub_type;
        mdl_config.profile = input_params_ptr->sw_model_profile;
    }

    /*
     * default will be over written if the device config file provided 
     */
    test_info->mdl_config = &mdl_config;

    cur_status = process_file(test_info);
    if (cur_status != KAPS_OK)
    {
        kaps_printf("FAILED: %s\n", kaps_get_status_string(cur_status));
        kaps_printf("\n - Test End Time: ");
        print_date_and_time();
        kaps_printf("\n");
        exit(1);
    }

    if (test_info->flag_output_dir)
        kaps_sysfree(test_info->flag_output_dir);

    if (test_info->test_step_config_file_name)
        kaps_sysfree(test_info->test_step_config_file_name);
    if (test_info->flag_expected_fail)
        kaps_sysfree(test_info->flag_expected_fail);

    kaps_printf("\n - Test End Time: ");
    print_date_and_time();
    kaps_printf("\n");

    if (test_info->capacity_problem_present)
    {
        kaps_sysfree(test_info);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Capacity is not between lower and upper capacity\n");
    }

    kaps_sysfree(test_info);

exit:
    SHR_FUNC_EXIT;
}

 /**
  * Main entry point for KAPS-test
  */
shr_error_e
kaps_verifier_main(
    int unit,
    sh_sand_control_t * sand_control)
{
    struct kaps_verifier_input_params input_params;
    struct kaps_verifier_input_params *input_params_ptr = &input_params;
    uint32_t ptr_size = sizeof(input_params_ptr);

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("device_xml", input_params_ptr->device_xml);
    SH_SAND_GET_STR("input_xml", input_params_ptr->input_xml);
    SH_SAND_GET_ENUM("coreloop_type", input_params_ptr->coreloop_type);
    SH_SAND_GET_ENUM("warmboot_type", input_params_ptr->warmboot_type);
    SH_SAND_GET_ENUM("xpt_type", input_params_ptr->xpt_type);
    SH_SAND_GET_UINT32("iterations", input_params_ptr->num_iterations);
    SH_SAND_GET_UINT32("sw_model_dev_id", input_params_ptr->sw_model_dev_id);
    SH_SAND_GET_UINT32("sw_model_sub_type", input_params_ptr->sw_model_sub_type);
    SH_SAND_GET_UINT32("sw_model_profile", input_params_ptr->sw_model_profile);
    SH_SAND_GET_STR("vector_file", input_params_ptr->vector_file_name);

    LOG_CLI((BSL_META("at kaps_verifier_main\n")));

    LOG_CLI((BSL_META("device_xml: %s\n"), input_params_ptr->device_xml));
    LOG_CLI((BSL_META("input_xml: %s\n"), input_params_ptr->input_xml));
    LOG_CLI((BSL_META("coreloop_type: %u\n"), input_params_ptr->coreloop_type));
    LOG_CLI((BSL_META("warmboot_type: %u\n"), input_params_ptr->warmboot_type));
    LOG_CLI((BSL_META("xpt_type: %u\n"), input_params_ptr->xpt_type));
    LOG_CLI((BSL_META("iterations: %u\n"), input_params_ptr->num_iterations));
    LOG_CLI((BSL_META("vector file name: %s\n"), input_params_ptr->vector_file_name));
    LOG_CLI((BSL_META("Size of Pointer = %u bytes\n"), ptr_size));

    if ((sal_strncmp(input_params_ptr->device_xml, "", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        && (sal_strncmp(input_params_ptr->input_xml, "", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0))
    {
        LOG_CLI((BSL_META("No input files provided, so nothing to do. Exiting...\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(verifier_start(unit, input_params_ptr));

exit:
    SHR_FUNC_EXIT;
}
