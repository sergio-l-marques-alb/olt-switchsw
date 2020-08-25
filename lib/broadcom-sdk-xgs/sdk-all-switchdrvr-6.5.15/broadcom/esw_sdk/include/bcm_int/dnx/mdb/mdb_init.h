/*
 * ! \file mdb_init.h $Id$ Contains all the MDB initialize files includes.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_INIT_INCLUDED
/*
 * {
 */
#define MDB_H_INIT_INCLUDED

/*
 * Include files.
 * {
 */
#include <shared/swstate/sw_state.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/mdb.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define MDB_LPM_MAX_FMT           (11)
#define MDB_LPM_NOF_FMT_BITS      (4)
#define MDB_LPM_MIN_FMT           (1)
#define MDB_NOF_DIRECT_FMT        (32)


/*When in 960bits mode, randomly choose one of the BPUs to be ODD*/
#define MDB_BPU_SIZE_960_ODD                    (13)
#define MDB_BPU_SIZE_960                        (12)
#define MDB_BPU_SIZE_480                        (4)
#define MDB_BPU_SIZE_240x2                      (3)
#define MDB_BPU_SIZE_120x2_240                  (1)
#define MDB_BPU_SIZE_240_120x2                  (2)
#define MDB_BPU_SIZE_120x4                      (0)

#define MDB_BPU_MODE_BITS                       (3)
#define MDB_BPU_MODE_LPM                        (0)
#define MDB_BPU_MODE_TCAM                       (1)
#define MDB_BPU_MODE_EM_120                     (2)
#define MDB_BPU_MODE_EM_240                     (3)
#define MDB_BPU_MODE_DIRECT_120                 (4)
#define MDB_BPU_MODE_DIRECT_240                 (5)

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define MDB_INIT_EM_IS_EMP(mdb_physical_table) ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM) || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3) || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4))
#define MDB_DIRECT_ROW_SIZE_TO_DIVIDER(_row_size) ((_row_size / MDB_NOF_CLUSTER_ROW_BITS) - 1)
/*
 * }
 */

/*
 * ENUMs
 * {
 */

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
 * Function headers
 * {
 */
shr_error_e mdb_init_add_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);
shr_error_e mdb_init_delete_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS]);
shr_error_e mdb_dh_init(
    int unit);

shr_error_e mdb_step_table_init(
    int unit);

shr_error_e mdb_dh_init_table(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id);

shr_error_e mdb_dh_init_table_em_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_em_vmv_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

shr_error_e mdb_init_update_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);
/*
 * }
 */
#endif /* !MDB_H_INIT_INCLUDED */
