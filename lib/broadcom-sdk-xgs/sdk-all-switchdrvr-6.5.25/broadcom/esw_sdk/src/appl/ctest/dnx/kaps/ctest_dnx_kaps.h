/*
 * ! \file ctest_dnx_kaps.h contains declarations for KAPS Ctest
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_KAPS_H_INCLUDED
#define CTEST_DNX_KAPS_H_INCLUDED

#include "kaps_db.h"
#include "kaps_default_allocator.h"
#include "kaps_device.h"
#include "kaps_key.h"
#include "kaps_instruction.h"
#include "kaps_errors.h"
#include "kaps_portable.h"
#include "kaps_ad.h"
#include "kaps_hb.h"
#include "kaps_hw_limits.h"
#include "kaps_xpt.h"
#include "kaps_sw_model.h"

#define KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8 (20)
#define KAPS_DEMO_AD_WIDTH_8               (3)
#define KAPS_DEMO_AD_WIDTH_1               (20)

/* Max key length of the LPM prefixes in bits */
#define MAX_KAPS_KEY_LEN_IN_BITS    (640)

#define MAX_PREFIX_LEN_IN_BITS             (160)
#define MAX_PREFIX_LEN_IN_BYTES            (20)

#define LPM_CAPACITY          (128 * 1024)
#define LPM_PRINT_COUNT       (100000)

#define MAX_KAPS_CTEST_STRING_LEN (512)

/*
 * database information
 */

struct lpm_record
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
    struct kaps_db *lpm_db_p;

    /*
     * AD DB pointers
     */
    struct kaps_ad_db *lpm_ad_db_p;

    /*
     * HB DB pointers
     */
    struct kaps_hb_db *lpm_hb_db_p;

    /*
     * Instructions
     */
    struct kaps_instruction *lpm_inst_p;

    /*
     * holds records
     */
    struct lpm_record *lpm_records_p;

    uint32 num_entries_inserted;

    uint32 vrf_field_len_in_bits;
    uint32 prefix_field_len_in_bits;
    uint32 total_db_width_in_bits;
    uint32 total_db_width_in_bytes;

    uint8_t *wb_memory;
    uint32_t wb_size;

    uint8 is_blackhole_mode;

    uint8 connect_to_sw_model;

    uint8 is_device_locked;

    uint32 sw_model_dev_id;

    uint32 sw_model_sub_type;

    uint32 sw_model_profile;
};

typedef struct
{
    struct kaps_xpt mdb_lpm_xpt;
    int unit;
    int num_reads;
    int num_writes;
    struct kaps_demo_info *info;
} CTEST_KAPS_MDB_XPT;

#endif
