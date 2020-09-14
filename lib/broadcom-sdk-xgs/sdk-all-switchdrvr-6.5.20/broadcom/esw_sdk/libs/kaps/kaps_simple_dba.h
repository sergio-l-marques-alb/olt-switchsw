/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#ifndef __KAPS_SIMPLE_DBA_H
#define __KAPS_SIMPLE_DBA_H

#include "kaps_bitmap.h"
#include "kaps_device.h"
#include "kaps_fibmgr.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_db;

#define MAX_NUM_RPT_ENTRIES_IN_POOL 256

    struct kaps_simple_dba_range_for_pref_len
    {
        int32_t first_row;
        int32_t last_row;
        int32_t prefix_len;
        struct kaps_simple_dba_range_for_pref_len *next;
        struct kaps_simple_dba_range_for_pref_len *prev;
    };

    struct kaps_simple_dba_node
    {
        struct kaps_simple_dba_range_for_pref_len *chunks;
        struct kaps_fast_bitmap used;
        uint8_t we_are_in_pre_insert_mode;
        uint8_t pre_inserted_entries_not_placed_yet;
        uint8_t alloced;
        void (
    *entry_move_callback) (
    void *,
    void *,
    uint32_t,
    uint32_t);
    };

    struct kaps_simple_dba
    {
        struct kaps_device *device;
        struct kaps_allocator *alloc;
        struct kaps_fast_bitmap free;
        struct kaps_simple_dba_node nodes[MAX_NUM_RPT_ENTRIES_IN_POOL];
        void **pfx_bundles;

        uint32_t pool_start_loc;
        uint32_t pool_size;
        uint32_t free_entries;
        uint32_t dba_no;
        uint32_t is_clone;
    };

    enum kaps_dba_trigger_compression_mode
    {
        DBA_NO_TRIGGER_COMPRESSION = 0,/**< No DBA compression */
        DBA_TRIGGER_COMPRESSION_1 = 1, /**< 40b - 40b Compression Config of AB */
        DBA_TRIGGER_COMPRESSION_2 = 2, /**< 24b - 24b - 32b Compression Config of AB */
    };

    struct kaps_fib_dba
    {
        struct kaps_simple_dba kaps_simple_dba;

        kaps_fib_tbl_mgr *m_fib_tbl_mgr;

        struct kaps_ab_info *m_ab_info;

        uint32_t pool_index;

        void *defaultRptEntry_p;

        enum kaps_dba_trigger_compression_mode mode;

        uint8_t m_start_offset_1;

        uint8_t m_num_bytes;

        uint8_t m_is_delete_op;

        uint8_t m_is_cur_active;

        uint8_t m_curDbId;

        uint8_t m_is_extra_byte_added;

        uint32_t cb_result;

        uint32_t cb_reason;
    };

    kaps_status kaps_simple_dba_find_place_for_entry(
    struct kaps_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len,
    int32_t * result);

    void kaps_simple_dba_place_entry_at_loc(
    struct kaps_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len,
    uint32_t pos);

/** Incrementally builds histogram of prefix lengths (last_row of
    apm_range_for_pref_len holds number of prefixes of that length). */
    void kaps_simple_dba_pre_insert_entry(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len);

/** Function to call between last call to pre_insert_entry() and first
    call to apm_find_place_for_pre_inserted_entry(). */
    void kaps_simple_dba_convert_ranges_from_pre_insert(
    struct kaps_simple_dba *m,
    int32_t group_id);

/** Shuffle-less find_place based on the histogram. */
    int32_t kaps_simple_dba_find_place_for_pre_inserted_entry(
    struct kaps_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len);

    void kaps_simple_dba_free_entry(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len,
    uint32_t position);

    void kaps_simple_dba_replace_entry(
    struct kaps_simple_dba *m,
    void *handle,
    int32_t group_id,
    uint32_t pos);

    void *kaps_simple_dba_get_entry(
    struct kaps_simple_dba *m,
    uint32_t pos);

    struct kaps_simple_dba *kaps_simple_dba_init(
    struct kaps_device *device,
    uint32_t pool_start,
    uint32_t pool_size,
    uint32_t dba_no,
    uint32_t is_clone);

    void kaps_simple_dba_destroy(
    struct kaps_simple_dba *m);

    kaps_status kaps_kaps_simple_dba_init_node(
    struct kaps_simple_dba *m,
    int32_t group_id,
    void (*entry_move_callback) (void *,
                                 void *,
                                 uint32_t,
                                 uint32_t));

    void kaps_simple_dba_free_node(
    struct kaps_simple_dba *m,
    int32_t group_id);

    uint32_t kaps_simple_dba_verify(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t(*get_prio_length) (void *));

    void *kaps_simple_dba_search(
    struct kaps_simple_dba *m,
    int32_t group_id,
    struct kaps_db *db,
    uint8_t * key,
    int32_t(*compare_fn) (struct kaps_db * db,
                          uint8_t * key,
                          void *user_data));

    void kaps_simple_dba_repair_entry(
    struct kaps_simple_dba *m,
    uint32_t row_nr,
    uint32_t * found);

#ifdef __cplusplus
}
#endif

#endif
