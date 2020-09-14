/** \file ctest_dnx_kaps.c
 *
 * kaps tests.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>

#include <sal/core/boot.h>

#include <shared/utilex/utilex_framework.h>

#include "../libs/kaps/include/kaps_db.h"
#include "../libs/kaps/include/kaps_default_allocator.h"
#include "../libs/kaps/include/kaps_device.h"
#include "../libs/kaps/include/kaps_key.h"
#include "../libs/kaps/include/kaps_instruction.h"
#include "../libs/kaps/include/kaps_errors.h"
#include "../libs/kaps/include/kaps_portable.h"
#include "../libs/kaps/include/kaps_ad.h"
#include "../libs/kaps/include/kaps_hb.h"
#include "../libs/kaps/include/kaps_hw_limits.h"
#include "../libs/kaps/include/kaps_xpt.h"
#include "../libs/kaps/include/kaps_sw_model.h"

#include "ctest_dnx_kaps_parser.h"

/**
 * }
 */

#define KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8 (20)
#define KAPS_DEMO_AD_WIDTH_8               (3)
#define KAPS_DEMO_AD_WIDTH_1               (20)

/* Max key length of the LPM prefixes in bits */
#define MAX_KAPS_KEY_LEN_IN_BITS    (640)

#define V4_ENTRY_PREFIX_LEN_IN_BYTES    (6)

#define MAX_PREFIX_LEN_IN_BITS             (160)
#define MAX_PREFIX_LEN_IN_BYTES            (20)

#define LPM_CAPACITY          (128 * 1024)
#define LPM_PRINT_COUNT       (100000)

typedef struct
{
    struct kaps_xpt mdb_lpm_xpt;
    int unit;
    int num_reads;
    int num_writes;
    int enable_writes;
} CTEST_KAPS_XPT;

extern kaps_status mdb_lpm_search(
    void *xpt,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result *kaps_result);

extern kaps_status mdb_lpm_register_read(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes);

extern kaps_status mdb_lpm_command(
    void *xpt,
    enum kaps_cmd cmd,
    enum kaps_func func,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * bytes);

extern kaps_status mdb_lpm_register_write(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes);

extern kaps_status mdb_lpm_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes);

extern kaps_status mdb_lpm_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes);

kaps_status
ctest_kaps_lpm_search(
    void *xpt,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result *kaps_result)
{
    return mdb_lpm_search(xpt, key, search_interface, kaps_result);
}

kaps_status
ctest_kaps_lpm_register_read(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_register_read(xpt, offset, nbytes, bytes);

    ((CTEST_KAPS_XPT *) xpt)->num_reads++;

    return rv;
}

kaps_status
ctest_kaps_lpm_command(
    void *xpt,
    enum kaps_cmd cmd,
    enum kaps_func func,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    if (((CTEST_KAPS_XPT *) xpt)->enable_writes)
    {
        rv = mdb_lpm_command(xpt, cmd, func, blk_nr, row_nr, nbytes, bytes);

        ((CTEST_KAPS_XPT *) xpt)->num_writes++;
    }

    return rv;
}

kaps_status
ctest_kaps_lpm_register_write(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_register_write(xpt, offset, nbytes, bytes);

    ((CTEST_KAPS_XPT *) xpt)->num_writes++;

    return rv;

}

kaps_status
ctest_kaps_lpm_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_hb_read(xpt, blk_nr, row_nr, bytes);

    ((CTEST_KAPS_XPT *) xpt)->num_reads++;

    return rv;
}

kaps_status
ctest_kaps_lpm_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_hb_write(xpt, blk_nr, row_nr, bytes);

    ((CTEST_KAPS_XPT *) xpt)->num_writes++;

    return rv;
}

/*
 * structs to support bcm shell command
 */
/*
 * {
 */
sh_sand_man_t dnx_kaps_default_test_man = {
    "Default Test for new KAPS code on JR2 KAPS",
    "The test adds entries to JR2 KAPS device and performs searches using adapter"
};

/*
 * }
 */

shr_error_e dnx_kaps_default_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

sh_sand_cmd_t dnx_kaps_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"default", dnx_kaps_default_test, NULL, NULL, &dnx_kaps_default_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/*
 * database information
 */

struct lpm_v4
{
    uint8_t pfx_data[MAX_PREFIX_LEN_IN_BYTES];
    uint32_t length;
};

enum pattern_type
{
    SEQUENTIAL_PATTERN = 0,
    RANDOM_PATTERN,
    FILE_PATTERN
};

struct kaps_demo_info
{
    struct kaps_allocator *kaps_alloc;
    struct kaps_allocator *ctest_alloc;
    void *xpt_p;
    struct kaps_device *device_p;

    /*
     * DB pointers
     */
    struct kaps_db *lpm_v4_db_p;

    /*
     * AD DB pointers
     */
    struct kaps_ad_db *lpm_v4_ad_p;

    /*
     * HB DB pointers
     */
    struct kaps_hb_db *lpm_v4_hb_p;

    /*
     * Instructions
     */
    struct kaps_instruction *lpm_v4_inst_p;     /* 1x LPM v4 search */

    /*
     * holds records
     */
    struct lpm_v4 *lpm_v4_p;

    uint32_t num_entries_inserted;

    uint32_t vrf_field_len_in_bits;
    uint32_t prefix_field_len_in_bits;
    uint32_t total_db_width_in_bits;
    uint32_t total_db_width_in_bytes;

    uint8_t *wb_memory;
    uint32_t wb_size;

    uint8_t is_blackhole_mode;
};

static kaps_status
allocate_memory_for_entries(
    struct kaps_demo_info *info,
    int num_entries_to_add)
{
    struct kaps_allocator *alloc_p = info->ctest_alloc;

    info->lpm_v4_p = alloc_p->xcalloc(alloc_p->cookie, 1, (num_entries_to_add * sizeof(struct lpm_v4)));
    if (info->lpm_v4_p == NULL)
        return KAPS_OUT_OF_MEMORY;

    return KAPS_OK;
}

static kaps_status
deallocate_memory_for_entries(
    struct kaps_demo_info *info)
{
    struct kaps_allocator *alloc_p = info->ctest_alloc;

    if (info->lpm_v4_p)
        alloc_p->xfree(alloc_p->cookie, info->lpm_v4_p);

    return KAPS_OK;
}

/* Generate the records [entries/prefixes] for the LPM databases */
static kaps_status
generate_records_for_databases(
    struct kaps_demo_info *info,
    enum pattern_type pattern,
    char *filename,
    int num_entries_to_add)
{
    uint32_t iter, byte_nr, num_bytes;
    uint32_t pfx_gen_seq_num = 0x30000000;
    struct ctest_kaps_parse_record *file_entries = NULL;
    uint32_t num_lines_in_file = 0, num_entries_parsed_from_file = 0;
    struct ctest_kaps_parse_key *parse_key = NULL;

    if (pattern == SEQUENTIAL_PATTERN)
    {
        printf("Generating sequential prefixes \n");
    }
    else if (pattern == RANDOM_PATTERN)
    {
        printf("Generating random prefixes \n");
    }
    else if (pattern == FILE_PATTERN)
    {
        printf("Generating prefixes from file = %s\n", filename);

        ctest_kaps_parse_standard_db_file(KAPS_DB_LPM, filename,
                                          &file_entries, 1000000, &num_lines_in_file,
                                          &num_entries_parsed_from_file, &parse_key);

        if (num_entries_parsed_from_file < num_entries_to_add)
            num_entries_to_add = num_entries_parsed_from_file;
    }

    printf("\t- Generate prefixes KAPS_LPM_DB \n");
    for (iter = 0; iter < num_entries_to_add; iter++)
    {
        /*
         * All entries have VPN-ID = 1
         */
        info->lpm_v4_p[iter].pfx_data[0] = 0;
        info->lpm_v4_p[iter].pfx_data[1] = 1;

        if (pattern == SEQUENTIAL_PATTERN)
        {
            int index = info->total_db_width_in_bytes - 4;

            info->lpm_v4_p[iter].pfx_data[index] = (pfx_gen_seq_num >> 24) & 0xFF;
            info->lpm_v4_p[iter].pfx_data[index + 1] = (pfx_gen_seq_num >> 16) & 0xFF;
            info->lpm_v4_p[iter].pfx_data[index + 2] = (pfx_gen_seq_num >> 8) & 0xFF;
            info->lpm_v4_p[iter].pfx_data[index + 3] = (pfx_gen_seq_num) & 0xFF;

            info->lpm_v4_p[iter].length = info->total_db_width_in_bits;

            pfx_gen_seq_num++;
        }
        else if (pattern == RANDOM_PATTERN)
        {
            for (byte_nr = 2; byte_nr < info->total_db_width_in_bytes; byte_nr++)
            {
                info->lpm_v4_p[iter].pfx_data[byte_nr] = rand() % 256;
            }

            info->lpm_v4_p[iter].length = info->total_db_width_in_bits;
        }
        else if (pattern == FILE_PATTERN)
        {
            num_bytes = (file_entries[iter].length + 7) / 8;
            kaps_memcpy(info->lpm_v4_p[iter].pfx_data, file_entries[iter].data, num_bytes);
            info->lpm_v4_p[iter].length = file_entries[iter].length;
        }
    }

    if (pattern == FILE_PATTERN && file_entries)
    {
        kaps_sysfree(file_entries);
    }

    return KAPS_OK;
}

/* Insert/add the records [entries/prefixes] to the LPM databases */
static kaps_status
populate_records_to_databases(
    struct kaps_demo_info *info,
    uint32_t num_entries_to_add)
{
    int32_t iter, i;
    struct kaps_entry *dummy_entry;
    struct kaps_ad *ad_handle = NULL;
    struct kaps_hb *hb_handle = NULL;
    CTEST_KAPS_XPT *xpt = info->xpt_p;
    kaps_status status = KAPS_OK;

    uint32_t kaps_timers_group;

    printf("\n- Inserting prefixes KAPS_LPM_DB \n");

    if (info->is_blackhole_mode)
        xpt->enable_writes = 0;

    /*
     * Initialize the timer for measuring the update rate
     */
    utilex_ll_timer_group_allocate("kaps_lpm measurements", &kaps_timers_group);
    utilex_ll_timer_clear_all(kaps_timers_group);
    utilex_ll_timer_set("kaps_entry_add", kaps_timers_group, 0);

    for (iter = 0; iter < num_entries_to_add; iter++)
    {
        uint8_t ad_value[KAPS_DEMO_AD_WIDTH_8] = { 0 };

        /*
         * mask has value zero, all bits in the key will be matched
         */
        status = kaps_db_add_prefix
            (info->lpm_v4_db_p, info->lpm_v4_p[iter].pfx_data, info->lpm_v4_p[iter].length, &dummy_entry);

        if (status != KAPS_OK)
        {
            if (status == KAPS_DUPLICATE)
                continue;
            else
                return status;
        }

        for (i = 0; i < KAPS_DEMO_AD_WIDTH_8; ++i)
        {
            ad_value[i] = rand() % 256;
        }
        ad_value[KAPS_DEMO_AD_WIDTH_8 - 1] &= 0XF0;     /* Only first four bits in the last byte are valid for the AD */

        KAPS_STRY(kaps_ad_db_add_entry(info->lpm_v4_ad_p, ad_value, &ad_handle));
        KAPS_STRY(kaps_entry_add_ad(info->lpm_v4_db_p, dummy_entry, ad_handle));

        if (info->lpm_v4_hb_p)
        {
            KAPS_STRY(kaps_hb_db_add_entry(info->lpm_v4_hb_p, &hb_handle));
            KAPS_STRY(kaps_entry_add_hb(info->lpm_v4_db_p, dummy_entry, hb_handle));
        }

        /*
         * add the entry to the database
         */
        status = kaps_db_install(info->lpm_v4_db_p);
        if (status != KAPS_OK)
        {
            if (status == KAPS_OUT_OF_UDA || status == KAPS_OUT_OF_DBA
                || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_UIT)
            {
                KAPS_STRY(kaps_db_delete_entry(info->lpm_v4_db_p, dummy_entry));
                printf("Table is full \n");
                printf("  - Total %d prefixes inserted to LPM v4 database #0 \n", iter);
                break;
            }
            else
            {
                return status;
            }
        }

        info->num_entries_inserted++;

        if ((iter % LPM_PRINT_COUNT) == 0 && iter)
            printf("\t  -> Inserted %d prefixes to LPM v4 database #0 \n", iter);
    }

    /*
     * Stop the timer and print the time taken
     */
    utilex_ll_timer_stop(kaps_timers_group, 0);
    utilex_ll_timer_print_all(kaps_timers_group);

    /*
     * Clear the timers and free them
     */
    utilex_ll_timer_clear_all(kaps_timers_group);
    utilex_ll_timer_group_free(kaps_timers_group);

    xpt->enable_writes = 1;

    printf("  - Total %d prefixes inserted to LPM v4 database #0 \n", iter);

    return KAPS_OK;
}

static kaps_status
verify_search_results(
    uint32_t res_port,
    struct kaps_entry *entry,
    struct kaps_complete_search_result *cmp_rslt,
    uint8_t * ad_value)
{
    /*
     * compare the two search results
     */

    if (kaps_memcmp(ad_value, cmp_rslt->assoc_data[res_port], KAPS_DEMO_AD_WIDTH_8) != 0)
    {
        uint32_t i;

        printf("\n\n\t- Error !! Mismatch between AD returned in hardware and software for the search\n\n");

        printf("Expected: ");
        for (i = 0; i < KAPS_DEMO_AD_WIDTH_8; ++i)
        {
            printf("%02x ", ad_value[i]);
        }
        printf("\n");

        printf("Got:");
        for (i = 0; i < KAPS_DEMO_AD_WIDTH_8; ++i)
        {
            printf("%02x ", cmp_rslt->assoc_data[res_port][i]);
        }
        printf("\n");

        return KAPS_MISMATCH;
    }

    return KAPS_OK;
}

/* Search the records [entries/prefixes] which are added toLPM databases */
static kaps_status
perform_search_on_databases(
    struct kaps_demo_info *info)
{
    uint32_t iter;
    uint8_t ad_value[KAPS_DEMO_AD_WIDTH_8] = { 0 };

    uint8_t master_key[MAX_KAPS_KEY_LEN_IN_BITS];
    uint8_t search_key[KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8];
    int32_t index0 = 0, prio = 0;
    struct kaps_entry *lpm_v4_entry = NULL;
    struct kaps_ad *lpm_v4_ad_handle = NULL;
    struct kaps_complete_search_result cmp_rslt;

    printf("\n- Searching prefixes KAPS_LPM_DB \n");
    for (iter = 0; iter < info->num_entries_inserted; iter++)
    {
        kaps_memset(&cmp_rslt, 0, sizeof(struct kaps_complete_search_result));
        kaps_memset(master_key, 0, sizeof(master_key));
        kaps_memset(search_key, 0, sizeof(search_key));
        kaps_memset(ad_value, 0, sizeof(ad_value));

        /*
         * generate the master key for result port 0 [159:0]
         */
        kaps_memcpy(&master_key[0 * KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8], info->lpm_v4_p[iter].pfx_data, 10);

        /*
         * generate the search key for s/w search [159:0]
         */
        kaps_memcpy(search_key, info->lpm_v4_p[iter].pfx_data, info->total_db_width_in_bytes);

        KAPS_TRY(kaps_db_search(info->lpm_v4_db_p, search_key, &lpm_v4_entry, &index0, &prio));
        if (lpm_v4_entry)
        {
            KAPS_TRY(kaps_entry_get_ad(info->lpm_v4_db_p, lpm_v4_entry, &lpm_v4_ad_handle));
            KAPS_TRY(kaps_ad_db_get(info->lpm_v4_ad_p, lpm_v4_ad_handle, ad_value));
        }

        /*
         * search in the device
         */
        KAPS_TRY(kaps_instruction_search(info->lpm_v4_inst_p, master_key, 0, &cmp_rslt));
        KAPS_TRY(verify_search_results(0, lpm_v4_entry, &cmp_rslt, ad_value));

        if ((iter % LPM_PRINT_COUNT) == 0 && iter)
            printf("\t  -> Searched %d prefixes from LPM v4 database #0 \n", iter);
    }
    printf("  - Total %d prefixes searched from LPM v4 database #0 \n\n", iter);

    return KAPS_OK;
}

static kaps_status
create_databases(
    struct kaps_demo_info *info,
    int is_hb_present)
{
    /*
     * Create the LPM v4 database
     */
    printf("\t- Create LPM v4 database #0 \n");
    printf("\t\t Key Layout: 48b -> table_id(8b) + vpn_id(8b) + sip(32b) \n");
    KAPS_TRY(kaps_db_init(info->device_p, KAPS_DB_LPM, 0, LPM_CAPACITY, &info->lpm_v4_db_p));
    KAPS_TRY(kaps_ad_db_init(info->device_p, 0, LPM_CAPACITY, 20, &info->lpm_v4_ad_p));
    KAPS_TRY(kaps_db_set_ad(info->lpm_v4_db_p, info->lpm_v4_ad_p));

    if (is_hb_present)
    {
        KAPS_TRY(kaps_hb_db_init(info->device_p, 0, LPM_CAPACITY, &info->lpm_v4_hb_p));
        KAPS_TRY(kaps_db_set_hb(info->lpm_v4_db_p, info->lpm_v4_hb_p));
    }

    return KAPS_OK;
}

static kaps_status
create_key_and_inst_databases(
    struct kaps_demo_info *info)
{
    struct kaps_key *lpm_v4_key;
    struct kaps_key *lpm_v4_mkey;

    /*
     * Create the LPM v4 key
     */
    printf("\n\t- Create LPM v4 key for database #0 \n");
    KAPS_TRY(kaps_key_init(info->device_p, &lpm_v4_key));

    /*
     * Key format: 48b -> VRF_id(16b) + sip(32b)
     */
    printf("\t\t- Add the key fields to the v4 database key \n");

    if (info->vrf_field_len_in_bits)
        KAPS_TRY(kaps_key_add_field(lpm_v4_key, "VRF_ID", info->vrf_field_len_in_bits, KAPS_KEY_FIELD_EM));

    KAPS_TRY(kaps_key_add_field(lpm_v4_key, "PREFIX", info->prefix_field_len_in_bits, KAPS_KEY_FIELD_PREFIX));

    /*
     * Add the key to the database
     */
    printf("\t\t- Add key to LPM v4 database #0 \n");
    KAPS_TRY(kaps_db_set_key(info->lpm_v4_db_p, lpm_v4_key));

    /*
     * Create LPM v4 DB instruction
     */
    printf("\t\t- Create instruction with id 0, for LPM v4 database #0 \n");
    KAPS_TRY(kaps_instruction_init(info->device_p, 0, 0, &info->lpm_v4_inst_p));

    /*
     * Add the master key details
     */
    KAPS_TRY(kaps_key_init(info->device_p, &lpm_v4_mkey));

    /*
     * Master Key
     */
    if (info->vrf_field_len_in_bits)
        KAPS_TRY(kaps_key_add_field(lpm_v4_mkey, "VRF_ID", info->vrf_field_len_in_bits, KAPS_KEY_FIELD_EM));

    KAPS_TRY(kaps_key_add_field(lpm_v4_mkey, "PREFIX", info->prefix_field_len_in_bits, KAPS_KEY_FIELD_PREFIX));

    KAPS_TRY(kaps_key_add_field(lpm_v4_mkey, "ZERO_FILL", 160 - info->total_db_width_in_bits, KAPS_KEY_FIELD_EM));

    printf("\t\t- Master key added to the instruction #0 \n");
    KAPS_TRY(kaps_instruction_set_key(info->lpm_v4_inst_p, lpm_v4_mkey));

    /*
     * Add the keys to the LPM v4 inst, Set the KPU and Result Port for the LPM v4 database
     */
    KAPS_TRY(kaps_instruction_add_db(info->lpm_v4_inst_p, info->lpm_v4_db_p, 0));
    KAPS_TRY(kaps_instruction_install(info->lpm_v4_inst_p));
    printf("\n");
    KAPS_TRY(kaps_instruction_print(info->lpm_v4_inst_p, stdout));

    printf("- Created and installed all instructions\n");

    return KAPS_OK;
}

/* Warmboot callback functions for write */
static int32_t
wb_nv_write(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    uint8_t *tmp;
    struct kaps_demo_info *info = (struct kaps_demo_info *) handle;
    struct kaps_allocator *alloc = info->ctest_alloc;

    if (info->wb_memory)
    {
        if ((offset + size) > info->wb_size)
        {
            /*
             * Realloc NV memory
             */
            tmp = (uint8_t *) alloc->xcalloc(alloc->cookie, info->wb_size * 2, sizeof(uint8_t));
            if (!tmp)
            {
                printf("NV memory alloc failed\n");
                kaps_sassert(0);
            }

            kaps_memcpy(tmp, info->wb_memory, info->wb_size);

            alloc->xfree(alloc->cookie, info->wb_memory);

            info->wb_memory = tmp;
            info->wb_size *= 2;
        }

        kaps_memcpy(&info->wb_memory[offset], buffer, size);
    }

    return 0;
}

/* Warmboot callback functions for read */
static int32_t
wb_nv_read(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    struct kaps_demo_info *info = (struct kaps_demo_info *) handle;

    if (info->wb_memory)
    {
        if ((offset + size) > info->wb_size)
            return -1;

        kaps_memcpy(buffer, &info->wb_memory[offset], size);
    }

    return 0;
}

static kaps_status
perform_warmboot(
    struct kaps_demo_info *info)
{
    struct kaps_allocator *alloc = info->ctest_alloc;

    printf("\n- Performing warmboot \n");

    info->wb_size = 80 * 1024 * 1024;
    info->wb_memory = alloc->xcalloc(alloc->cookie, info->wb_size, sizeof(uint8_t));

    if (!info->wb_memory)
    {
        kaps_printf("Warmboot memory allocation failed \n");
        info->wb_size = 0;
        return KAPS_OUT_OF_MEMORY;
    }

    /*
     * Save the software state in warmboot memory
     */
    KAPS_TRY(kaps_device_save_state(info->device_p, wb_nv_read, wb_nv_write, (void *) info));

    /*
     * Destroy the KAPS device
     */
    KAPS_TRY(kaps_device_destroy(info->device_p));

    /*
     * Re-initialize the KAPS device.
     */
    KAPS_TRY(kaps_device_init
             (info->kaps_alloc, KAPS_DEVICE_KAPS, KAPS_DEVICE_SKIP_INIT, info->xpt_p, NULL, &info->device_p));

    /*
     * Restore the software state from the warmboot memory
     */
    KAPS_TRY(kaps_device_restore_state(info->device_p, wb_nv_read, wb_nv_write, (void *) info));

    /*
     * Refresh the AD DB pointers
     */
    if (info->lpm_v4_ad_p)
        KAPS_TRY(kaps_ad_db_refresh_handle(info->device_p, info->lpm_v4_ad_p, &info->lpm_v4_ad_p));

    /*
     * Refresh the HB DB pointers
     */
    if (info->lpm_v4_hb_p)
        KAPS_TRY(kaps_hb_db_refresh_handle(info->device_p, info->lpm_v4_hb_p, &info->lpm_v4_hb_p));

    /*
     * Refresh the LPM DB pointers
     */
    if (info->lpm_v4_db_p)
        KAPS_TRY(kaps_db_refresh_handle(info->device_p, info->lpm_v4_db_p, &info->lpm_v4_db_p));

    /*
     * Refresh the instruction handles
     */
    if (info->lpm_v4_inst_p)
        KAPS_TRY(kaps_instruction_refresh_handle(info->device_p, info->lpm_v4_inst_p, &info->lpm_v4_inst_p));

    /*
     * Lock the KAPS device
     */
    KAPS_TRY(kaps_device_lock(info->device_p));

    /*
     * Release the warm boot memory
     */
    if (info->wb_memory)
    {
        alloc->xfree(info->ctest_alloc->cookie, info->wb_memory);
    }

    printf("\n- Warmboot is complete \n\n");

    return KAPS_OK;
}

static kaps_status
kaps_workflow_fn(
    void *xpt)
{
    struct kaps_demo_info info = { 0 };
    struct kaps_default_allocator_stats alloc_stats;
    enum pattern_type pattern = SEQUENTIAL_PATTERN;
    char *filename = "rrc00_ipv4_only.txt";
    int num_entries_to_add = 10000;
    int is_hb_present = 1;
    int do_warmboot = 1;
    int do_search = 1;
    int is_blackhole_mode = 0;
    int vrf_field_len_in_bits = 16;
    int prefix_field_len_in_bits = 32;

    printf("\n -------------------------------------------------------------------");
    printf("\n     Scenario: KAPS demo application using the LPM module");
    printf("\n ------------------------------------------------------------------- \n\n");

    /*
     * Create the default allocator
     */
    printf("- Create default allocator \n");
    KAPS_TRY(kaps_default_allocator_create(&info.kaps_alloc));
    KAPS_TRY(kaps_default_allocator_create(&info.ctest_alloc));

    /*
     * Assign the XPT
     */
    info.xpt_p = xpt;

    info.is_blackhole_mode = is_blackhole_mode;
    if (is_blackhole_mode)
        do_search = 0;

    info.vrf_field_len_in_bits = vrf_field_len_in_bits;
    info.prefix_field_len_in_bits = prefix_field_len_in_bits;
    info.total_db_width_in_bits = vrf_field_len_in_bits + prefix_field_len_in_bits;
    info.total_db_width_in_bytes = (info.total_db_width_in_bits + 7) / 8;

    /*
     * Initialize the device
     */
    printf("- Initialize the KBP device \n\n");
    KAPS_TRY(kaps_device_init
             (info.kaps_alloc, KAPS_DEVICE_KAPS, KAPS_DEVICE_DEFAULT, info.xpt_p, NULL, &info.device_p));

    /*
     * Allocate memory for storing the entries for LPM databases
     */
    printf("- Allocate memory for storing the database entries/prefixes \n");
    KAPS_TRY(allocate_memory_for_entries(&info, num_entries_to_add));

    /*
     * Create the LPM databases as required
     */
    printf("- Create Databases \n");
    KAPS_TRY(create_databases(&info, is_hb_present));

    printf("- Create keys and instructions for databases \n");
    KAPS_TRY(create_key_and_inst_databases(&info));

    printf("\n- Lock the device \n");
    KAPS_TRY(kaps_device_lock(info.device_p));

    /*
     * Generate records for all the databases
     */

    printf("- Generate the records for the databases \n");
    KAPS_TRY(generate_records_for_databases(&info, pattern, filename, num_entries_to_add));

    /*
     * Insert the record to the databases
     */
    printf("- Populate the databases \n");
    KAPS_TRY(populate_records_to_databases(&info, num_entries_to_add));

    /*
     * Perform all the warmboot related operations
     */
    if (do_warmboot)
        KAPS_TRY(perform_warmboot(&info));

    /*
     * Search the inserted prefixes in database
     * Do not perform searches in simulation mode
     */
    if (!SAL_BOOT_PLISIM && do_search)
    {
        KAPS_TRY(perform_search_on_databases(&info));
    }

    kaps_default_allocator_get_stats(info.kaps_alloc, &alloc_stats);
    printf("- Heap Peak Usage = %0.2fMB\n", ((float) alloc_stats.peak_bytes) / (1024.0 * 1024.0));

    printf("- Total number of writes = %d\n", ((CTEST_KAPS_XPT *) info.xpt_p)->num_writes);
    printf("- Total number of reads = %d\n\n", ((CTEST_KAPS_XPT *) info.xpt_p)->num_reads);
    /*
     * Free memory for stored entries for LPM or both databases
     */
    printf("- De-allocate memory for the database entries/prefixes stored ");
    KAPS_TRY(deallocate_memory_for_entries(&info));

    /*
     * Destroy the device handle
     */
    printf("\n- Destroy the device \n");
    KAPS_TRY(kaps_device_destroy(info.device_p));

    /*
     * Destroy the default allocator
     */
    printf("- Destroy the default allocator \n");
    KAPS_TRY(kaps_default_allocator_destroy(info.kaps_alloc));
    KAPS_TRY(kaps_default_allocator_destroy(info.ctest_alloc));

    printf("\n ----------------------------------------------------------");
    printf("\n --> KAPS demo application using the LPM module done <--");
    printf("\n --> Overall Result: PASSED <--");
    printf("\n ---------------------------------------------------------- \n\n");

    return KAPS_OK;
}

shr_error_e
initialize_xpt(
    int unit,
    void **xpt)
{
    CTEST_KAPS_XPT *xpt_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(*xpt, sizeof(CTEST_KAPS_XPT), "kaps_xpt", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (*xpt == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, " Error:  SHR_ALLOC for xpt Failed\n");
    }

    xpt_p = (CTEST_KAPS_XPT *) * xpt;

    xpt_p->mdb_lpm_xpt.device_type = KAPS_DEVICE_KAPS;

    xpt_p->mdb_lpm_xpt.kaps_search = ctest_kaps_lpm_search;
    xpt_p->mdb_lpm_xpt.kaps_register_read = ctest_kaps_lpm_register_read;
    xpt_p->mdb_lpm_xpt.kaps_command = ctest_kaps_lpm_command;
    xpt_p->mdb_lpm_xpt.kaps_register_write = ctest_kaps_lpm_register_write;
    xpt_p->mdb_lpm_xpt.kaps_hb_read = ctest_kaps_lpm_hb_read;
    xpt_p->mdb_lpm_xpt.kaps_hb_write = ctest_kaps_lpm_hb_write;

    xpt_p->unit = unit;
    xpt_p->num_writes = 0;
    xpt_p->num_reads = 0;
    xpt_p->enable_writes = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kaps_default_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    kaps_status status = KAPS_OK;
    void *xpt = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the XPT
     */
    SHR_IF_ERR_EXIT(initialize_xpt(unit, &xpt));

    /*
     * Running the test for J2 KAPS
     */
    status = kaps_workflow_fn(xpt);

    if (status != KAPS_OK)
        SHR_ERR_EXIT(_SHR_E_FAIL, "KAPS test failed with status = %d\n", status);

    printf("Test passed\n");

exit:
    SHR_FUNC_EXIT;
}
