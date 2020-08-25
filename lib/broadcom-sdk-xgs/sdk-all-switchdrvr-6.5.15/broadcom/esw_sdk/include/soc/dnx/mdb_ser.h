/** \file mdb_ser.h
 * Contains all of the MDB access functions provided to the SER correction.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_SER_H_INCLUDED
/*
 * {
 */
#define MDB_SER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/mdb.h>

#define MDB_SER_ENTRY_SIZE_IN_BITS          128
#define MDB_SER_ENTRY_DATA_SIZE_IN_BITS     120
#define MDB_SER_ENTRY_ECC_SIZE_IN_BITS      8
#define MDB_SER_ENTRY_DATA_SIZE_IN_U8       15
#define MDB_SER_ENTRY_SIZE_IN_U8            16
#define MDB_SER_ENTRY_SIZE_IN_U32           4

#define MDB_SER_ADDR_OFFSET_OFFSET(_mem)    (((_mem == DDHB_MACRO_1_ENTRY_BANKm) || (_mem == DDHB_MACRO_0_ENTRY_BANKm)) ? 13 : 14)
#define MDB_SER_ADDR_OFFSET_BITS            (2)
#define MDB_SER_ADDR_OFFSET_MASK(_mem)      ((1 << MDB_SER_ADDR_OFFSET_OFFSET(_mem)) - 1)

#define MDB_SER_CLUSTER_OFFSET_NONE         0

#define MDB_SER_OFFSET_OFFSET           3
#define MDB_SER_COMMAND_OFFSET          0

#define MDB_SER_SET_CONTROL_BITS(command, offset) (((command&0x7) << MDB_SER_COMMAND_OFFSET) | ((offset&0x1F) << MDB_SER_OFFSET_OFFSET))

void mdb_ser_print_cluster_entry(
    int unit,
    uint32 *entry_data,
    uint32 ecc,
    char *print_msg);

shr_error_e mdb_phy_table_db_info_dump(
    int unit,
    int detail);

shr_error_e mdb_macro_map_info_dump(
    int unit);

shr_error_e mdb_dh_macro_cluster_entry_get(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 *ecc);

shr_error_e mdb_dh_macro_cluster_entry_set(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 ecc);

shr_error_e mdb_dh_physical_mem_entry_get(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data);

shr_error_e mdb_dh_physical_mem_entry_set(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data);

shr_error_e mdb_dh_macro_cluster_chg_dump(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx);

shr_error_e mdb_phy_table_cluster_chg_dump(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

shr_error_e mdb_ser_ecc_1b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx);

shr_error_e mdb_ser_ecc_2b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx);

char *mdb_get_db_type_str(
    dbal_enum_value_field_mdb_db_type_e mdb_db_type);

char *mdb_physical_table_to_string(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

/**
 * \brief
 *   get mdb physical table id by name of
 * \param [in] unit - The unit number.
 * \param [in] str - mdb physical table name.
 * \param [out] mdb_phy_table_id - mdb physical table id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e mdb_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_enum_value_field_mdb_physical_table_e * mdb_phy_table_id);

shr_error_e mdb_ser_map_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index);

shr_error_e mdb_ser_map_access_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t access_mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index);
shr_error_e mdb_ser_map_cluster_to_dbal_phy_table(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_index,
    uint32 cluster_index,
    dbal_physical_tables_e * dbal_physical_table_id);

shr_error_e mdb_dh_macro_cluster_cache_check(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx);

shr_error_e mdb_phy_table_cluster_cache_check(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id);

/*
 * }
 */
#endif /* !MDB_SER_H_INCLUDED */
