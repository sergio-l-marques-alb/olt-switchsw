
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_INIT_INTERNAL_INCLUDED

#define MDB_H_INIT_INTERNAL_INCLUDED



#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

#include <soc/dnx/mdb.h>
#include <soc/dnx/mdb_internal_shared.h>






#define MDB_LPM_NOF_FMT_BITS      (4)


#define MDB_LPM_MIN_FMT           (1)


#define MDB_NOF_DIRECT_FMT        (32)


#define MDB_BPU_MODE_BITS                       (3)
#define MDB_BPU_MODE_LPM                        (0)
#define MDB_BPU_MODE_TCAM                       (1)
#define MDB_BPU_MODE_EM_120                     (2)
#define MDB_BPU_MODE_EM_240                     (3)
#define MDB_BPU_MODE_DIRECT_120                 (4)
#define MDB_BPU_MODE_DIRECT_240                 (5)






#define MDB_DIRECT_ROW_SIZE_TO_DIVIDER(_row_size) ((_row_size / MDB_NOF_CLUSTER_ROW_BITS) - 1)



#define MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(_payload_size) ((_payload_size / MDB_DIRECT_BASIC_ENTRY_SIZE))





















shr_error_e mdb_dh_set_bpu_setting_macro(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx);


shr_error_e mdb_dh_init(
    int unit);


shr_error_e mdb_step_table_init(
    int unit);



shr_error_e mdb_dh_init_update_em_bank_sizes(
    int unit,
    mdb_physical_table_e mdb_physical_table,
    dh_em_bank_info * em_bank_info);



shr_error_e mdb_dh_init_table(
    int unit,
    mdb_physical_table_e mdb_physical_table_id);


shr_error_e mdb_dh_init_table_em_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);



shr_error_e mdb_dh_init_update_if_cluster_offset(
    int unit,
    int cluster_flags,
    mdb_cluster_info_t * cluster_info_p,
    mdb_physical_table_e mdb_physical_table);


shr_error_e mdb_init_dh_add_em_format(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e dbal_physical_table_id);


shr_error_e mdb_init_eedb_deinit_bank(
    int unit,
    int bank_idx);


shr_error_e mdb_init_eedb_phase(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index,
    soc_reg_above_64_val_t type_bank_select,
    soc_reg_above_64_val_t phase_bank_select,
    soc_reg_above_64_val_t bank_id);


shr_error_e mdb_init_em_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table);


shr_error_e mdb_init_lpm_xpt_inner(
    int unit);


#endif 
