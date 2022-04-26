/*
 * ! \file kaps_verifier_parse.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __KAPS_VERIFIER_PARSE_H_INCLUDED
#define __KAPS_VERIFIER_PARSE_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <sal/core/boot.h>

#include <shared/utilex/utilex_framework.h>

#include "kaps_db.h"
#include "kaps_key.h"
#include "kaps_instruction.h"
#include "kaps_hw_limits.h"

#define IPv6_MAX_LEN (128)
#define IPv4_MAX_LEN (32)

/**
 *
 * @file kaps_verifier_parse.h
 *
 * KBP file parsing and printing utilities.
 *
 * @addtogroup KAPS_PARSE_APIS
 * @{
 */

/**
 * @brief Parsed range information (generally useful for ACLs).
 */
struct kaps_parse_range
{
    uint16 offset_8; /**< The byte offset to the start of range
                          in the range-modified key. In other
                          cases, such as ACL parser, it is defined
                          by the module. */
    uint16 lo;       /**< Low end of the range used for making >= comparisons. */
    uint16 hi;       /**< High end of range used for making <= comparisons. */
};

/**
 * @brief Custom operation for KAPS testing
 */
enum kaps_custom_operation
{
    KAPS_CUSTOM_OPERATION_ADD = 1,  /**< Custom ADD operation*/
    KAPS_CUSTOM_OPERATION_DEL = 2,  /**< Custom DEL operation*/
    KAPS_CUSTOM_OPERATION_UPD = 3   /**< Custom Update AD operation*/
};

/**
 * @brief A single KAPS parse record.
 */

struct kaps_parse_record
{
    struct kaps_entry *e;                /**< Testing use. */
    struct kaps_parse_record *next;      /**< Testing use. */
    struct kaps_parse_record *next_entry;      /**< Testing use. */
    struct kaps_parse_range ranges[KAPS_HW_MAX_RANGE_COMPARES];  /**< User-specified ranges. */
    struct kaps_hb *hb_e;                /**< HitBit Entry */
    uint64 counter_copy;              /**< Counter copy */
    uint8 *data;                      /**< Data bits. */
    uint8 *mask;                      /**< 1 == don't care. */
    void *ad_data;                      /**< Testing use. */
    uint8 *user_ad_data;              /**< AD data from dataset file */
    uint32 priority:22;               /**< Line number for ACL, or inverse prefix length for LPM. */
    uint32 length:8;                  /**< Prefix length, for LPM only. */
    uint32 visited:2;                 /**< Used for bookkeeping. */
    uint32 num_ranges:3;              /**< Number of active range records. */
    uint32 status:3;                  /**< Testing use. */
    uint32 hit:1;                     /**< Testing use */
    uint32 pending_del:1;             /**< Testing use */
    uint32 in_search_loop:1;          /**< Testing use */
    uint32 old_visited_status:2;      /**< Testing use */
    uint32 counter:20;                /**< Testing use */
    uint8_t cust_add_or_del;          /**< custom scenario, have add/del/upd : ADD:1, DEL:2, UPD:3 */
    uint32_t cust_db_id;              /**< DB Id to which the record belongs for custom scenario */
};

/**
 * @brief Parsed key format.
 */

struct kaps_parse_key
{
    char *fname;                        /**< String name for the key. */
    uint32 width;                     /**< Width of key in bits. */
    uint32 offset;
    uint32 overlay_field;
    enum kaps_key_field_type type;       /**< Type of key field. */
    struct kaps_parse_key *next;         /**< Linked list of key fields. */
    char buf[];                         /**<< Memory for string name. */
};

/**
 *@breif parsed AD info.
 */

struct kaps_parse_ad_info
{
    struct kaps_ad_db *ad_db;            /**< Valid AD Database pointer */
    void *zerob_ad_handle;       /**< 0b ad_handle which we associate for every entry */
    int32 ad_percent;                /**< percentage of distribution */
    int32 ad_percent_index_start;            /**< low value of distribution */
    int32 ad_percent_index_end;           /**< high value of distribution */
    struct kaps_parse_ad_info *next;     /**< Linked List of AD Databases */
    void **dangling_ad;                 /**< List of Dangling AD entries */
    int32 ad_list_count;              /**< Stats for number of Dangling AD entries */
    uint32 capacity;                  /**< Capacity of the AD Database */
    uint32 ad_width_8:8;              /**< AD Database width in Bytes, aligned */
    uint32 ad_offset_8:8;             /**< AD offset, if non multiple of 32b */
    uint32 ad_resp:8;                 /**< AD Response type */
    uint32 ad_align_8:8;              /**< AD Database width in Bytes, non aligned */
};

/**
 * Parse a text file holding a KBP database in any of the known formats.
 * The passed-in options determine the nature of the
 * result array returned. For LPM file types, the entry priority
 * is computed as the valid prefix length. For ACLs, it is order
 * of entry in the file. If the random flag is set, the order of parsed
 * entries is randomized and the original priorities are retained.
 *
 * @param db_type ACL, LPM, or EM database types.
 * @param randomize If set to a nonzero value, will randomize the input array.
 * @param fname Name of database file.
 * @param req_num_entries used to parse the entries requested, If zero we will parse the whole dataset file.
 * @param result Used to return an array of parsed records. The memory must be freed by the caller.
 * @param num_entries Size of the array of parsed records.
 * @param key The layout of the entry fields in the file.
 *
 * It is the responsibility of the caller to free up the memory pointed
 * to by the result array
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_parse_db_file(
    enum kaps_db_type db_type,
    int32 randomize,
    const char *fname,
    uint32 req_num_entries,
    uint32 num_default_entries,
    struct kaps_parse_record **result,
    uint32 *num_entries,
    struct kaps_parse_key **key);

/**
 * Randomizes the order of entries in the input set of entries
 *
 * @param randomize If set to a nonzero value, will randomize the input array.
 * @param result Used to return an array of parsed records after randomization. The memory must be freed by the caller.
 * @param num_entries Size of the array of parsed records.
 *
 * It is the responsibility of the caller to free up the memory pointed
 * to by the result array. The memory of input array is freed
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status randomize_entries(
    int32 randomize,
    struct kaps_parse_record **result,
    uint32 *num_entries);

/**
 * Frees up the memory allocated for the entries.
 *
 * @param entries The entry array returned by kaps_parse_db_file().
 *
 */
void kaps_parse_destroy(
    struct kaps_parse_record *entries);

/**
 * Prints the key header.
 *
 * @param key Valid key layout.
 */

void kaps_parse_print_key(
    struct kaps_parse_key *key);

/**
 * Destroys the parse key.
 *
 * @param key Valid key layout.
 */

void kaps_parse_destroy_key(
    struct kaps_parse_key *key);

/**
 * Produces a text string with standard representation of the ACL entry.
 *
 * @param key The layout of the entry.
 * @param entry The entry to be printed.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_parse_print_record(
    struct kaps_parse_key *key,
    const struct kaps_parse_record *entry);

/**
 * Generates a single key that matches the ACL entry.
 *
 * @param e Points to the record holding the entry.
 * @param key Points to the memory region where the key should be generated.
 * @param width_8 Specifies the width of the entry.
 *
 */
kaps_status kaps_parse_generate_random_valid_key(
    const struct kaps_parse_record *e,
    uint8 *key,
    uint32 width_8);

enum db_priotiry_type
{
    TEST_PRIO_SINGLE,            /**< All entries has same priority */
    TEST_PRIO_INCREMENT,         /**< Entries with incremental priority */
    TEST_PRIO_DECREMENT,         /**< Entries with decremental priority */
    TEST_PRIO_RANDOM,            /**< Entries with random priority */
    TEST_PRIO_HOLES_1,           /**< Entries where n-1 with same prio, last one < prio */
    TEST_PRIO_HOLES_2,           /**< Entries where n-1 with same prio, last one > prio */
    TEST_PRIO_INCREMENT2,        /**< Entries with incremental priority, start from bigger value */
};

struct manually_placed_resources
{
    uint8 rpt_bmp[2]; /**< 16b RPT bitmap */
    uint8 dba_bmp[32]; /**< 256b DBA bitmap */
    uint8 num_dba_dt; /**< Number of DBA based decision trees */
    uint8 num_sram_dt; /**< Number of SRAM based decision tress */
    uint8 max_lsn_size[4]; /**< Maximum LSN size of each DT */
    uint8 udm_bmp[2][8]; /**< 64b UDM bitmap */
};

/**
 * @brief Configuration options used for setting a budget for the database resources.
 */
struct kaps_parse_db_conf
{
    struct manually_placed_resources user_specified; /**< User Specified manual resources */
    enum kaps_device_type flag_device; /**< Device type. */
    int32 num_ab;                  /**< Number of ABs to use. */
    int32 uda_mb;                  /**< Number of megabits of UDA to use. */
    int32 max_lsn;                 /**< Max number bricks in LSN. */
    int32 sram_pwr_bgt;            /**< NetACL SRAM power budget. */
    int16 range_units;             /**< Number of MCOR units. */
    int16 ad_width;                /**< -1 if no AD, or width of AD in bits. */
    int16 algorithmic;             /**< Set to algorithmic/nonalgorithmic mode. */
    int16 pc;                      /**< Power-control level. */
    int16 up;                      /**< Update rate/capacity tradeoff. */
    enum kaps_db_type db_type;        /**< Database type. */
    int32 flag_incremental;        /**< Populate entries incrementally if set to 1. */
    uint32 flag_shuffle_stats:1;   /**< calculate PIOWR stats*/
    uint32 flag_sw_srchs:1;        /**< do sw searches for LPM stats*/
    uint32 index_range:1;          /**< if index range specified */
    uint32 suppress_ix_cb:1;       /**< suppress ix_callback for update rates */
    uint32 force_to_sram:1;        /**< force the entries to sram */
    uint32 rebuild_tree:1;         /**< to avoid re-building */
    uint32 reduced_ix_cb:1;        /**< set prop reduced ix callbacks for LPM */
    uint32 manual_placed_resources:1; /**< set the prop for the manually placed resource */
    uint32 manual_placed_pwr_bgt:1;   /**< set the prop for the manually placed pwr bgt */
    int32 upd_level;               /**< update rate level */
    uint32 index_min;              /**< min index */
    uint32 index_max;              /**< max index */
    uint32 num_duplicates;         /**< Num of duplicates in the dataset */
    enum db_priotiry_type ptype;     /**< Test Type */
    enum kaps_status status;          /**< Return status with reason code*/
};

/**
 *
 * Dynamically generates the LPM entries for the requested pattern.
 *
 * @param pattern The requested test pattern such as 801, 901, some times duplicate prefixes may be present ....
 * @param length The maximum length of the prefix.
 * @param num_requested Number of prefixes required.
 * @param result Array for returning the parse records populated with prefixes.
 * @param num_entries The number of actual prefixes created.
 *
 * It is the responsibility of the caller to free up the memory pointed
 * to by the result array and the key.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_parse_generate_prefixes(
    uint32 pattern,
    uint32 length,
    uint32 num_requested,
    struct kaps_parse_record **result,
    uint32 *num_entries);

/**
 *
 * Dynamically generates the LPM entries for the requested pattern, where hash table is
 * used to check for the duplicate prefixes, this will make sure no duplicates are
 * generated.
 *
 * @param device_type KBP device type
 * @param pattern The requested test pattern such as 801, 901, some times duplicate prefixes may be present ....
 * @param length The maximum length of the prefix.
 * @param num_requested Number of prefixes required.
 * @param result Array for returning the parse records populated with prefixes.
 * @param num_entries The number of actual prefixes created.
 *
 * It is the responsibility of the caller to free up the memory pointed
 * to by the result array and the key.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_parse_generate_unique_prefixes(
    enum kaps_device_type dev_type,
    uint32 pattern,
    uint32 length,
    uint32 num_requested,
    struct kaps_parse_record **result,
    uint32 *num_entries);

/**
 *
 * API parses the char array and fill the kaps_parse_record structure in the specified format
 *
 * @param db_type database type
 * @param buf char array to be parsed
 * @param key key structure of the database
 * @param entry structure to be filled
 * @param key_width_1 width of the database
 * @param acl_priority priority to  assign for this entry
 * @param format1 indicates lpm type of format
 * @param format2 indicates acl type of format
 * @param ad_width width of the AD
 * @param em_with_mask EM field parses data as well mask too (if not set only data)
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_parse_entry(
    enum kaps_db_type db_type,
    const char *buf,
    struct kaps_parse_key *key,
    struct kaps_parse_record *entry,
    uint32 key_width_1,
    uint32 acl_priority,
    int32 format1,
    int32 format2,
    uint32 ad_width,
    uint32 em_with_mask);

/**
 * Parse a text file holding a KBP database in any of the known formats.
 * The passed-in options determine the nature of the
 * result array returned. For LPM file types, the entry priority
 * is computed as the valid prefix length. For ACLs, it is order
 * of entry in the file.
 *
 * @param db_type ACL, LPM, or EM database types.
 * @param fname Name of database file.
 * @param result Used to return an array of parsed records. The memory must be freed by the caller.
 * @param req_num_entries used to parse the entries requested, If zero we will parse the whole dataset file.
 * @param ndefault_entries number of default entries specified in the xml file
 * @param ad_width AD width of the database if dataset has AD
 * @param num_lines Actual number of lines in the file
 * @param num_entries Size of the array of parsed records.
 * @param key The layout of the entry fields in the file.
 *
 * It is the responsibility of the caller to free up the memory pointed
 * to by the result array
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status parse_standard_db_file(
    enum kaps_db_type db_type,
    const char *fname,
    struct kaps_parse_record * *result,
    uint32 req_num_entries,
    uint32 ndefault_entries,
    uint32 *ad_width,
    uint32 *num_lines,
    uint32 *num_entries,
    struct kaps_parse_key * *key);

/**
 * Evaluate a KBP parsed database by showing its distribution & other metrics.
 * Current plan is to parse LPM DBs only!
 *
 * @param db_type ACL, LPM, or EM database types.
 * @param fname Name of database file.
 * @param entries The actual DB entries to be evaluated.
 * @param num_entries Size of the array of parsed records.
 * @param key_len The length of the entry fields (LPM + VRF + ...) in the file.
 * @param histogram The histogram of the DB prefixes.
 * @param max_prefix_len The maximum prefix length of this DB.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status eval_lpm_db(
    enum kaps_db_type db_type,
    const char *fname,
    struct kaps_parse_record *entries,
    uint32 num_entries,
    uint32 key_len,
    uint32 **histogram,
    uint32 *max_prefix_len);

/**
 * @}
 */

#endif /* __KAPS_VERIFIER_PARSE_H_INCLUDED */
