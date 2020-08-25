/*
 * $Id$ Contains all of the MDB access functions provided to the DBAL.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_INCLUDED
/*
 * {
 */
#define MDB_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>

#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>

#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/dma.h>
#include <soc/kbp/alg_kbp/include/xpt_kaps.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#endif /* defined(INCLUDE_KBP) */
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>

/*#define MDB_FPP_DUMPS*/

#define MDB_NOF_ROWS_IN_MACRO_A_CLUSTER        (16*1024)

#define MDB_NOF_ROWS_IN_MACRO_B_CLUSTER        (8*1024)

#define MDB_NOF_ROWS_IN_EEDB_BANK              (2*1024)

#define MDB_DIRECT_BASIC_ENTRY_SIZE             30
#define MDB_NOF_CLUSTER_ROW_BITS                120
#define MDB_NOF_CLUSTER_ROW_UINT32              BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS)

#define MDB_NOF_CLUSTER_ROW_BYTE                BITS2BYTES(MDB_NOF_CLUSTER_ROW_BITS)
#define MDB_MAX_NOF_CLUSTERS     100

#define MDB_LPM_INSTRUCTIONS_ID  0

#define MDB_TEST_BRIEF_FACTOR                 (1024)
#define MDB_TEST_BRIEF_ENTRIES                (128)

#define MDB_BPU_8_CLUSTERS_BIT_MASK           (0xFF)
#define MDB_BPU_6_CLUSTERS_BIT_MASK           (0x3F)
#define MDB_BPU_BUCKET_BIT_MASK               (0xF)
#define MDB_BPU_240_RIGHT_BKT_SIDE_BIT_MASK   (0x3)
#define MDB_BPU_240_LEFT_BKT_SIDE_BIT_MASK    (0xC)

#define MDB_DIRECT_INVALID_START_ADDRESS      (0xFFFFFFF)

#define MDB_PAYLOAD_SIZE_TO_PAYLOAD_SIZE_TYPE(payload_size) ((payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1)
#define MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type) ((payload_type + 1) * MDB_DIRECT_BASIC_ENTRY_SIZE)
#define MDB_ENTRY_REMAINDER_CALC(entry_index,entries_size,row_width) ((entry_index * entries_size) % row_width)
#define MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(_payload_size) ((_payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE))
/** The minimal number of UINT32 that can hold the largest entry */
#define MDB_MAX_DIRECT_PAYLOAD_SIZE_32    MDB_CEILING(SAL_UINT32_NOF_BITS,\
                                                  MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_NOF_ENUM_MDB_DIRECT_PAYLOAD_VALUES-1))

#define MDB_OUTLIF_AND_GRANULARITY_TO_PREFIX(unit, outlif, granularity) \
        ((outlif >> utilex_log2_round_up(MDB_NOF_ROWS_IN_EEDB_BANK * MDB_NOF_CLUSTER_ROW_BITS / granularity)) \
                & ((1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)) - 1))

#define MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS     (3)
#define MDB_DH_GET_GLOBAL_MACRO_INDEX(_unit,_macro_type,_macro_index) ((_macro_type == MDB_MACRO_A) ? _macro_index : dnx_data_mdb.dh.nof_macroes_type_A_get(_unit) + _macro_index)

#define MDB_INIT_GET_EM_EMCODING_NOF_BITS(unit, encod_req) (encod_req == MDB_EM_ENCODING_TABLE_ENTRY) ? MDB_EM_TABLE_ENTRY_ENCODING_NOF_BITS : ((encod_req == MDB_EM_ENCODING_DH_ENTRY_120) ? dnx_data_mdb.em.dh_120_entry_encoding_nof_bits_get(unit): dnx_data_mdb.em.dh_240_entry_encoding_nof_bits_get(unit))

#define MDB_KAPS_AD_WIDTH_IN_BYTES(unit)        BITS2BYTES(dnx_data_mdb.kaps.ad_width_in_bits_get(unit))

#define MDB_KAPS_KEY_WIDTH_IN_BYTES(unit)       BITS2BYTES(dnx_data_mdb.kaps.key_width_in_bits_get(unit))

#define MDB_KAPS_KEY_WIDTH_IN_UINT32(unit)      BITS2WORDS(dnx_data_mdb.kaps.key_width_in_bits_get(unit))
#define MDB_EM_KEY_MIN_OR_ABOVE(unit,key)       UTILEX_MAX(key,dnx_data_mdb.em.min_key_size_get(unit))

#define MDB_EEDB_POINTER_FORMAT_17BIT_RES_1k     (0)
#define MDB_EEDB_POINTER_FORMAT_18BIT_RES_1k     (1)
#define MDB_EEDB_POINTER_FORMAT_19BIT_RES_2k     (2)
#define MDB_EEDB_POINTER_FORMAT_20BIT_RES_4k     (3)

#define MDB_NOF_PHASES_PER_EEDB_BANK                    (3)
#define MDB_NOF_PHASES_PER_EEDB_BANK_BITS               (2)
#define MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS     (1)
#define MDB_PHASE_BANK_SELECT_DEFAULT                   (0)

typedef struct kbp_key *mdb_kaps_key_t_p;
typedef struct kbp_dma_db *mdb_kaps_dma_db_t_p;

/*
 * ENUMs
 * {
 */
typedef enum
{
    MDB_INIT_CLUSTER_ADD,
    MDB_INIT_CLUSTER_DELETE,

    MDB_NOF_INIT_CLUSTER_OPERATIONS
} mdb_init_cluster_func_e;
typedef enum
{
    MDB_KAPS_KEY_PREFIX_IPV4_UC = 0,
    MDB_KAPS_KEY_PREFIX_IPV4_MC = 2,
    MDB_KAPS_KEY_PREFIX_IPV6_UC = 3,
    MDB_KAPS_KEY_PREFIX_IPV6_MC = 4,
    MDB_KAPS_KEY_PREFIX_FCOE = 6,
    MDB_NOF_KAPS_KEY_PREFIX
} mdb_kaps_key_prefix_e;

typedef enum
{
    MDB_EEDB_MEM_TYPE_PHY,
    MDB_EEDB_MEM_TYPE_LL,

    MDB_NOF_EEDB_MEM_TYPES
} mdb_eedb_mem_type_e;

typedef enum
{
    MDB_EM_ENTRY_ENCODING_ONE = 0,
    MDB_EM_ENTRY_ENCODING_HALF = 1,
    MDB_EM_ENTRY_ENCODING_QUARTER = 2,
    MDB_EM_ENTRY_ENCODING_EIGHTH = 3,
    MDB_EM_ENTRY_ENCODING_EMPTY = 7,
    MDB_EM_NOF_ENTRY_ENCODINGS
} mdb_em_entry_encoding_e;

typedef enum
{
    MDB_EM_ENCODING_TYPE_ONE,
    MDB_EM_ENCODING_TYPE_HALF,
    MDB_EM_ENCODING_TYPE_QUARTER,
    MDB_EM_ENCODING_TYPE_EIGHTH,
    MDB_EM_ENCODING_TYPE_EMPTY,
    MDB_EM_NOF_ENCODING_TYPES
} mdb_em_encoding_types_e;
typedef enum
{
    MDB_EM_ENCODING_TABLE_ENTRY,
    MDB_EM_ENCODING_DH_ENTRY_120,
    MDB_EM_ENCODING_DH_ENTRY_240,
    MDB_EM_NOF_ENCODING_REQUESTS
} mdb_em_encoding_request_e;

typedef enum
{
    MDB_TEST_BRIEF,
    MDB_TEST_FULL,
    MDB_TEST_CAPACITY,

    MDB_NOF_TEST_MODES
} mdb_test_mode_e;

typedef enum
{
    MDB_BALANCED_PROFILE,
    MDB_BALANCED_EXEM_PROFILE,
    MDB_L2_XL_PROFILE,
    MDB_L3_XL_PROFILE,
    MDB_EXT_KBP_PROFILE,
    MDB_EXEM_DEBUG_PROFILE,
    MDB_CUSTOM_PROFILE,
    MDB_NOF_PROFILES
} mdb_profiles_e;

typedef enum
{

    MDB_EM_HIT_BIT_MODE_A,
    MDB_EM_HIT_BIT_MODE_B,
    MDB_EM_HIT_BIT_MODE_A_OR_B
} mdb_em_hit_bit_modes_e;

/*
 * }
 */
typedef enum
{
    MDB_EEDB_TABLE_A,
    MDB_EEDB_TABLE_B,
    MDB_EEDB_TABLE_C,
    MDB_EEDB_TABLE_D,
    MDB_NOF_EEDB_TABLES
} mdb_eedb_table_e;

typedef struct mdb_cluster_alloc_info
{
    dbal_physical_tables_e dbal_physical_table_id;

    mdb_eedb_mem_type_e mdb_eedb_type;

    mdb_macro_types_e macro_type;

    uint8 macro_idx;

    uint8 cluster_idx;

    int macro_nof_rows;

    uint8 macro_row_width;
    int entry_size;
        dbal_enum_value_field_mdb_eedb_phase_e
        phase_alloc_logical_entries[(MDB_NOF_ROWS_IN_MACRO_A_CLUSTER / MDB_NOF_ROWS_IN_EEDB_BANK)];

    int logical_start_address;
} mdb_cluster_alloc_info_t;

extern const char cmd_mdb_usage[];

extern const dbal_physical_tables_e mdb_direct_mdb_to_dbal[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES];

shr_error_e mdb_pre_init_step(
    int unit);

shr_error_e mdb_get_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    int *capacity);

shr_error_e mdb_em_get_addr_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *addr_bits);

shr_error_e mdb_em_get_min_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    int vmv_size,
    mdb_em_entry_encoding_e * entry_encoding);
shr_error_e mdb_eedb_translate(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    mdb_macro_types_e macro_type,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id);
shr_error_e mdb_eedb_cluster_type_to_rows(
    int unit,
    mdb_macro_types_e macro_type,
    int *cluster_rows);

shr_error_e mdb_eedb_init_phase(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index,
    soc_reg_above_64_val_t type_bank_select,
    soc_reg_above_64_val_t phase_bank_select,
    soc_reg_above_64_val_t bank_id);

shr_error_e mdb_eedb_init_ll_db_atr(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index);

shr_error_e mdb_init_set_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);
shr_error_e mdb_eedb_init(
    int unit);

shr_error_e mdb_get_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 *vmv_size,
    uint8 *vmv_value);

shr_error_e mdb_set_vmv_size_value(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 vmv_size,
    uint8 vmv_value);

shr_error_e mdb_em_get_key_size(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    uint32 *key_size);
shr_error_e mdb_direct_table_get_direct_payload_type(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    dbal_enum_value_field_direct_payload_sizes_e * payload_type);
shr_error_e mdb_direct_table_get_basic_size(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int *basic_size);

shr_error_e mdb_direct_table_get_basic_size_using_logical_mdb_table(
    int unit,
    dbal_physical_tables_e logic_table_id,
    int *basic_size);
shr_error_e mdb_verify_macro_cluster_allocation(
    int unit,
    uint32 cluster_allocation);

shr_error_e mdb_em_set_vmv_regs(
    int unit,
    uint8 vmv_value,
    uint8 vmv_size,
    uint8 vmv_total_nof_bits,
    uint32 encoding_value,
    uint32 encoding_nof_bits,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data);

shr_error_e mdb_direct_table_get_row_width(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *row_width);

shr_error_e mdb_direct_table_entry_add(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_direct_table_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_direct_table_entry_delete(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_direct_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_direct_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_direct_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

shr_error_e mdb_direct_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_direct_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id);

shr_error_e mdb_em_entry_add(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_em_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_em_entry_delete(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_em_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd);

shr_error_e mdb_em_dynamic_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_em_prepare_entry(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    soc_mem_t mem,
    uint32 *key,
    uint32 key_size,
    uint32 *payload,
    uint32 payload_size,
    soc_reg_above_64_val_t entry);

shr_error_e mdb_em_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_em_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

shr_error_e mdb_em_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_em_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id);

shr_error_e mdb_em_init_shadow_db(
    int unit);

shr_error_e mdb_em_init_timeout_overrides(
    int unit);

#if defined(INCLUDE_KBP)

shr_error_e mdb_lpm_init(
    int unit);

int mdb_lpm_db_kaps_sync(
    int unit);

shr_error_e mdb_lpm_entry_add(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_entry_search(
    int unit,
    int core,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_entry_delete(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int delete_cmd);

shr_error_e mdb_lpm_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_lpm_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

shr_error_e mdb_lpm_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_lpm_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id);

shr_error_e mdb_lpm_dump_hw(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

uint32 mdb_lpm_calculate_prefix_length(
    uint32 *payload_mask);

shr_error_e mdb_lpm_get_current_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *num_entries,
    int *num_entries_iter,
    int *capacity_estimate);

shr_error_e mdb_lpm_dbal_to_db(
    int unit,
    dbal_physical_tables_e physical_table,
    mdb_kaps_ip_db_id_e * db_idx);

shr_error_e mdb_lpm_get_nof_small_bbs(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *nof_small_bbs);

shr_error_e mdb_lpm_get_big_bb_fmt(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    mdb_macro_types_e macro_type,
    uint8 macro_index,
    uint32 *fmt_val);

shr_error_e mdb_lpm_prefix_len_to_mask(
    int unit,
    int prefix_length,
    dbal_physical_entry_t * entry);

shr_error_e mdb_lpm_deinit_app(
    int unit,
    int no_sync_flag);

shr_error_e mdb_lpm_big_kaps_enabled(
    int unit,
    mdb_kaps_ip_db_id_e db_id,
    int *big_kaps_enabled);

shr_error_e mdb_lpm_xpt_init_inner(
    int unit);

#endif /* defined(INCLUDE_KBP) */

shr_error_e mdb_eedb_table_entry_add(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_eedb_table_entry_get(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_eedb_table_entry_delete(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

shr_error_e mdb_eedb_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_eedb_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_eedb_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

shr_error_e mdb_eedb_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e mdb_eedb_table_clear(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id);
mdb_profiles_e mdb_eedb_get_payload_size_from_cluster(
    int unit,
    mdb_cluster_info_t * cluster_info_p,
    uint32 *payload_size);

shr_error_e mdb_eedb_table_data_granularity_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *address_granularity);

shr_error_e mdb_eedb_table_nof_entries_per_cluster_type_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_macro_types_e macro_type,
    uint32 *nof_entries);

shr_error_e mdb_eedb_entry_to_phase(
    int unit,
    uint32 entry_index,
    dbal_physical_tables_e * dbal_physical_table_id);

shr_error_e mdb_init_get_table_resources(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS],
    int *nof_valid_clusters);

shr_error_e mdb_remove_bpu_setup_setting(
    int unit,
    uint32 current_allocation,
    uint32 global_macro_index);

shr_error_e mdb_em_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table);
shr_error_e mdb_eedb_dynamic_bank_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);
shr_error_e mdb_eedb_dynamic_cluster_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode);

shr_error_e mdb_get_macro_allocation(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_table_id,
    int macro_idx,
    mdb_macro_types_e macro_type,
    uint32 *macro_cluster_allocation);

shr_error_e mdb_eedb_phase_bank_to_index(
    int unit,
    int eedb_bank_index,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    uint32 *phase_bank_index);

shr_error_e mdb_init_em_entry_encoding(
    int unit);

shr_error_e mdb_em_get_entry_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 key_size,
    uint32 payload_size,
    uint32 app_id,
    int app_id_size,
    mdb_em_entry_encoding_e * entry_encoding);


void mdb_shift_vmv_reg_fetch(
    int stage,
    dbal_tables_e * table_name,
    dbal_fields_e * field_name,
    dbal_fields_e * key_name);

shr_error_e mdb_em_init_shift_vmv_conf(
    int unit);

shr_error_e mdb_parse_xml_em_vmv(
    int unit);

void mdb_init_em_set_b_access_interface_regs(
    );
shr_error_e mdb_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table);

shr_error_e mdb_eedb_deinit_bank(
    int unit,
    int bank_idx);

shr_error_e mdb_direct_get_payload_type(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    dbal_enum_value_field_mdb_direct_payload_e * payload_type);
shr_error_e mdb_em_calc_vmv_regs(
    int unit,
    mdb_em_encoding_request_e encoding_request,
    dbal_physical_tables_e dbal_table_id,
    soc_reg_above_64_val_t vmv_encoding_data,
    soc_reg_above_64_val_t vmv_size_data);

shr_error_e mdb_eedb_hitbit_init(
    int unit);

int dnx_mdb_init(
    int unit);

int dnx_mdb_deinit(
    int unit);



/*
 * }
 */
#endif /* !MDB_H_INCLUDED */
