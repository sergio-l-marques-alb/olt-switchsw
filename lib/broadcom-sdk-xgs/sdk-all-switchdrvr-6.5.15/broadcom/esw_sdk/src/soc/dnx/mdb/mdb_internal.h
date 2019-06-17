/*
 * ! \file mdb.h $Id$ Contains all of the internal MDB defines and globals. MDB Terminology Cluster - This is the
 * basic memory unit (each cluster is used for a single type of data base), has 16k/8k (MACROA/B) of 120b rows. Bucket
 * - A row of 4 clusters in indexes inside the MACRO 0-3 or 4-7. Macro - Contain 2 buckets (8 clusters). The MDB
 * consists of 12 Macro-A and 8 Macro-B The Macro-As are spread evenly between a single memory DHC_MACRO_ENTRY_BANK and
 * twelve blocks DHC0/../11. The Macro-Bs are spread evenly between two memories DDHB_MACRO_0/1_ENTRY_BANK and four
 * blocks DDHB0/1/2/3. Each Macro consists of 2 buckets, 4 clusters per bucket Macro-A cluster is 16k rows, Macro-B
 * cluster is 8k rows
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_INTERNAL_INCLUDED
/*
 * {
 */
#define MDB_H_INTERNAL_INCLUDED

/*
 * Include files.
 * {
 */
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <soc/dnx/mdb.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define MDB_ENTRY_DATA_SIZE_IN_BITS            128
#define MDB_ENTRY_DATA_SIZE_IN_UINT32          BITS2WORDS(MDB_ENTRY_DATA_SIZE_IN_BITS)

#define MDB_ENTRY_DATA_TOTAL_SIZE_IN_BITS      (MDB_ENTRY_DATA_SIZE_IN_BITS + 8)
#define MDB_ENTRY_BANK_SIZE_IN_BITS            544
#if (MDB_ENTRY_BANK_SIZE_IN_BITS % SAL_UINT32_NOF_BITS) != 0
#error "MDB_ENTRY_BANK_SIZE_IN_BITS assumes to be divided by SAL_UINT32_NOF_BITS"
#endif
#define MDB_ENTRY_BANK_SIZE_UINT32             (MDB_ENTRY_BANK_SIZE_IN_BITS/SAL_UINT32_NOF_BITS)
#if (MDB_ENTRY_DATA_SIZE_IN_BITS % SAL_UINT32_NOF_BITS) != 0
#error "MDB_ENTRY_DATA_SIZE_IN_BITS assumes to be divided by SAL_UINT32_NOF_BITS"
#endif
#define MDB_ENTRY_DATA_SIZE_UINT32             BITS2WORDS(MDB_ENTRY_DATA_SIZE_IN_BITS)
#define MDB_ENTRY_DATA_PER_BANK                (MDB_ENTRY_BANK_SIZE_IN_BITS/MDB_ENTRY_DATA_SIZE_IN_BITS)

#define MDB_EM_ADAPTER_SIZE                      (4096)

#define MDB_KAPS_BIG_BB_WIDTH_A000_B480     (1 << 0)
#define MDB_KAPS_BIG_BB_WIDTH_A000_B960     (1 << 1)
#define MDB_KAPS_BIG_BB_WIDTH_A480_B000     (1 << 2)
#define MDB_KAPS_BIG_BB_WIDTH_A480_B480     (1 << 3)
#define MDB_KAPS_BIG_BB_WIDTH_A960_B000     (1 << 4)
#define MDB_KAPS_BIG_ADS_DEPTH_16K          (1 << 5)
#define MDB_KAPS_BIG_ADS_DEPTH_32K          (1 << 6)

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define MDB_CEILING(x,y) (((x) -1 + (y))/(x))

/*
 * }
 */

/*
 * ENUMs
 * {
 */
typedef enum
{
    MDB_EM_DH_120_ENTRY_ENCODING_ONE = 3,
    MDB_EM_DH_120_ENTRY_ENCODING_HALF = 2,
    MDB_EM_DH_120_ENTRY_ENCODING_QUARTER = 1,
    MDB_EM_DH_120_ENTRY_ENCODING_EMPTY = 0,
    MDB_EM_DH_120_NOF_ENTRY_ENCODINGS
} mdb_em_dh_120_entry_encoding_e;
typedef enum
{
    MDB_EM_DH_240_ENTRY_ENCODING_ONE = 4,
    MDB_EM_DH_240_ENTRY_ENCODING_HALF = 3,
    MDB_EM_DH_240_ENTRY_ENCODING_QUARTER = 2,
    MDB_EM_DH_240_ENTRY_ENCODING_EIGHTH = 1,
    MDB_EM_DH_240_ENTRY_ENCODING_EMPTY = 0,
    MDB_EM_DH_240_NOF_ENTRY_ENCODINGS
} mdb_em_dh_240_entry_encoding_e;
typedef enum
{
    MDB_CLUSTER_ACCESS_INST = 0,
    MDB_CLUSTER_READ_INST = 1,
    MDB_CLUSTER_WRITE_INST = 2
} mdb_cluster_instructions_e;

typedef enum
{
    MDB_DIRECT_REMAINDER_TYPE_0B,
    MDB_DIRECT_REMAINDER_TYPE_30B,
    MDB_DIRECT_REMAINDER_TYPE_60B,
    MDB_DIRECT_REMAINDER_TYPE_90B,
    MDB_DIRECT_REMAINDER_TYPE_120B,
    MDB_DIRECT_REMAINDER_TYPE_150B,
    MDB_DIRECT_REMAINDER_TYPE_180B,
    MDB_DIRECT_REMAINDER_TYPE_210B,
    MDB_NOF_DIRECT_REMAINDER_TYPES
} mdb_direct_remainder_types_e;

typedef enum
{
    MDB_DIRECT_ROW_WIDTH_TYPE_120B,
    MDB_DIRECT_ROW_WIDTH_TYPE_240B,
    MDB_NOF_DIRECT_ROW_WIDTH_TYPES
} mdb_direct_row_width_types_e;
typedef enum
{
    MDB_DIRECT_ENTRY_HIT_BIT_INDEX_1 = 0,
    MDB_DIRECT_ENTRY_HIT_BIT_INDEX_2 = 1,
    MDB_DIRECT_ENTRY_NOF_HIT_BIT_INDEXS = 2
} mdb_direct_entry_hit_bit_index_e;

typedef enum
{
    MDB_ABK_BANK_A = 0,
    MDB_ABK_BANK_B = 1,
    MDB_NOF_ABK_BANKS = 2
} mdb_abk_banks_e;

/*
 * }
 */

/*
 * Structures
 * {
 */

/*
 * }
 */
/*
 * Globals
 * {
 */

/*
 * }
 */
/*
 * Externs
 * {
 */

/*
 * }
 */

/*
 * Function headers
 * {
 */

shr_error_e mdb_direct_cache_set(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 src_offset_bits,
    uint32 *src_array);

shr_error_e mdb_direct_cache_get(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);

shr_error_e mdb_direct_get_cluster_index(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address,
    int *cluster_idx);

shr_error_e mdb_direct_calc_macro_mem_blk(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    soc_mem_t * mem_entry,
    soc_reg_t * overflow_reg,
    soc_mem_t mem_hitbits[MDB_NOF_ABK_BANKS],
    int *blk);

shr_error_e mdb_parse_xml_direct_physical_map(
    int unit);


shr_error_e mdb_lpm_xpt_init(
    int unit,
    void **xpt);

shr_error_e mdb_lpm_xpt_deinit(
    int unit,
    void *xpt);

shr_error_e mdb_lpm_xpt_dump_hw(
    int unit,
    mdb_kaps_ip_db_id_e small_kaps_db,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_ads,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_bb);

shr_error_e mdb_lpm_uint8_to_uint32(
    int unit,
    int nof_bytes,
    uint8 *uint8_data,
    uint32 *uint32_data);

shr_error_e mdb_lpm_uint32_to_uint8(
    int unit,
    int nof_bytes,
    uint32 *uint32_data,
    uint8 *uint8_data);

shr_error_e mdb_lpm_big_kaps_enabled(
    int unit,
    mdb_kaps_ip_db_id_e db_id,
    int *big_kaps_enabled);

shr_error_e mdb_eedb_calc_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *capacity);

shr_error_e mdb_dh_add_em_format(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_init_get_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id);

shr_error_e mdb_get_dh_block(
    int unit,
    uint32 global_macro_index,
    int *blk);


/*
 * }
 */
#endif /* !MDB_H_INTERNAL_INCLUDED */
