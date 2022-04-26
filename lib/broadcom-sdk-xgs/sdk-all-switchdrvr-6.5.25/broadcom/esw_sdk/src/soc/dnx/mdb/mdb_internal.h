
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_INTERNAL_INCLUDED

#define MDB_H_INTERNAL_INCLUDED


#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/mdb_internal_shared.h>
#include <libs/kaps/include/kaps_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>







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



#define MDB_EM_LPM_ADAPTER_SIZE                      (4096)



#define MDB_LPM_KAPS_CFG_SMALL_KAPS_ENABLE 100


#define MDB_LPM_BB_MAX_UINT32_WIDTH  (BYTES2WORDS(DNX_DATA_MAX_MDB_KAPS_BB_BYTE_WIDTH))
#define MDB_LPM_RPB_MAX_UINT32_WIDTH (BYTES2WORDS(DNX_DATA_MAX_MDB_KAPS_RPB_BYTE_WIDTH))
#define MDB_LPM_AD_MAX_UINT32_WIDTH  (BYTES2WORDS(DNX_DATA_MAX_MDB_KAPS_ADS_BYTE_WIDTH))


typedef struct
{
    struct kaps_xpt mdb_lpm_xpt;
    int unit;
} MDB_LPM_XPT;



#define MDB_TEST_BRIEF_FACTOR                 (1024)



#define MDB_TEST_BRIEF_ENTRIES                (128)









#define MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,macro_type)  (dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits)
#define MDB_DIRECT_ADDR_OFFSET_BITS                     (2)
#define MDB_DIRECT_ADDR_OFFSET_MASK(unit,macro_type)    ((1 << MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,macro_type)) - 1)



#ifndef ADAPTER_SERVER_MODE
#define MDB_EEDB_ACCESS_UNLOCK(_unit,_expr)                                                             \
do                                                                                                       \
{                                                                                                        \
    int _macro_rv = 0;                                                                                   \
    int _attempt_counter = 0;                                                                            \
    int _enable_feature = dnx_data_mdb.feature.feature_get(_unit, dnx_data_mdb_feature_eedb_bank_traffic_lock); \
    uint32 error_check_en = FALSE;                                                                       \
    uint32 timeout_check_en = FALSE;                                                                     \
    if (_enable_feature)                                                                                 \
    {                                                                                                    \
        error_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_err_check);                             \
        SOC_FEATURE_CLEAR(unit, soc_feature_schan_err_check);                                            \
        timeout_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_hw_timeout);                          \
        SOC_FEATURE_CLEAR(unit, soc_feature_schan_hw_timeout);                                           \
        SHR_IF_ERR_EXIT(dnx_ire_packet_start(_unit));                                                    \
    }                                                                                                    \
    do                                                                                                   \
    {                                                                                                    \
        _macro_rv = _expr;                                                                               \
        _attempt_counter++;                                                                              \
    } while (_enable_feature && (_attempt_counter < 10));                                                \
    if (_enable_feature)                                                                                 \
    {                                                                                                    \
        if (error_check_en)                                                                              \
        {                                                                                                \
            SOC_FEATURE_SET(unit, soc_feature_schan_err_check);                                          \
        }                                                                                                \
        if (timeout_check_en)                                                                            \
        {                                                                                                \
            SOC_FEATURE_SET(unit, soc_feature_schan_hw_timeout);                                         \
        }                                                                                                \
        SHR_IF_ERR_EXIT(dnx_ire_packet_end(_unit));                                                      \
    } else                                                                                               \
    {                                                                                                    \
        SHR_IF_ERR_EXIT(_macro_rv);                                                                      \
    }                                                                                                    \
} while (0);
#else
#define MDB_EEDB_ACCESS_UNLOCK(_unit,_expr)                                                             \
do                                                                                                       \
{                                                                                                        \
    SHR_IF_ERR_EXIT(_expr);                                                                              \
} while (0);
#endif 







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










extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);






shr_error_e mdb_direct_address_mapping_verify(
    int unit,
    int physical_address_start,
    int physical_address_end);



shr_error_e mdb_direct_cache_init(
    int unit);



shr_error_e mdb_direct_cache_alloc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index);



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



shr_error_e mdb_direct_calc_macro_mem_blk(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    soc_mem_t * mem_entry,
    soc_reg_t * overflow_reg,
    soc_mem_t mem_hitbits[MDB_NOF_ABK_BANKS],
    int *blk);


shr_error_e mdb_em_get_physical_memory(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    soc_mem_t * mdb_physical_memory,
    soc_reg_t * mdb_physical_status_reg,
    int *blk,
    soc_reg_t * mdb_interrupt_reg,
    soc_field_t * interrupt_field);

sbusdma_desc_module_enable_e mdb_em_dbal_table_to_dma_enum(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);


shr_error_e mdb_parse_xml_direct_physical_map(
    int unit);



shr_error_e mdb_lpm_dbal_to_db(
    int unit,
    dbal_physical_tables_e physical_table,
    uint8 *db_idx);



shr_error_e mdb_lpm_cache_big_bb_get(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);


shr_error_e mdb_lpm_cache_tcam_get(
    int unit,
    int db_id,
    int instance_id,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);



shr_error_e mdb_lpm_xpt_get_nof_small_bbs(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *nof_small_bbs);


void mdb_lpm_kaps_buffer_dbal_to_kaps(
    const uint32 nof_bits,
    const uint32 nof_valid_bits,
    const uint32 *data_in,
    uint8 *data_out);


void mdb_lpm_kaps_buffer_kaps_to_dbal(
    uint32 nof_bits,
    uint8 *data_in,
    uint32 *data_out);



shr_error_e mdb_lpm_xpt_init(
    int unit,
    void **xpt);



shr_error_e mdb_lpm_xpt_deinit(
    int unit,
    void *xpt);



shr_error_e mdb_lpm_xpt_cache_big_bb_get(
    int unit,
    struct kaps_device *device,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);


shr_error_e mdb_lpm_xpt_cache_tcam_get(
    int unit,
    struct kaps_device *device,
    int db_id,
    int instance_id,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array);



shr_error_e mdb_lpm_xpt_dump_hw(
    int unit,
    uint8 small_kaps_db_idx,
    mdb_physical_table_e big_kaps_ads,
    mdb_physical_table_e big_kaps_bb);




shr_error_e mdb_eedb_calc_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *capacity);



shr_error_e mdb_get_dh_block(
    int unit,
    uint32 global_macro_index,
    int *blk);


shr_error_e mdb_arm_image_load_default(
    int unit);



shr_error_e mdb_num_valid_tables(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    int *tables_num);

#ifdef BCM_DNX2_SUPPORT


shr_error_e mdb_parse_xml_update_fec_hierarchy_address(
    int unit);
#endif 



#endif 
