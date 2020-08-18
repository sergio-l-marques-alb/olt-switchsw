/** \file mdb_ser.c
 * Contains all of the MDB access functions provided to the SER correction.
  *
  */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Includes
 * {
 */
#include <sal/appl/sal.h>
#include <shared/bsl.h>

#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include "../dbal/dbal_internal.h"
#include "mdb_internal.h"

#include <soc/sand/sand_ser_correction.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>
#include <soc/sand/sand_mem.h>
#include <soc/dnx/mdb_ser.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX

char *
mdb_physical_table_to_string(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id)
{

    static char *unknown_mdb_string = "unknown_mdb";
    static char *mdb_physical_table_strings[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] = {
        "ISEM_1",
        "INLIF_1",
        "IVSI",
        "ISEM_2",
        "INLIF_2",
        "ISEM_3",
        "INLIF_3",
        "LEM",
        "ADS_1",
        "ADS_2",
        "BIG_KAPS_BB_1",
        "BIG_KAPS_BB_2",
        "IOEM_0",
        "IOEM_1",
        "MAP",
        "FEC_1",
        "FEC_2",
        "FEC_3",
        "MC_ID",
        "GLEM_0",
        "GLEM_1",
        "EEDB_1_LL",
        "EEDB_1_DATA",
        "EEDB_2_LL",
        "EEDB_2_DATA",
        "EEDB_1_2_DATA",
        "EEDB_3_LL",
        "EEDB_3_DATA",
        "EEDB_4_LL",
        "EEDB_4_DATA",
        "EEDB_3_4_DATA",
        "EEDB_5_LL",
        "EEDB_5_DATA",
        "EEDB_6_LL",
        "EEDB_6_DATA",
        "EEDB_5_6_DATA",
        "EEDB_7_LL",
        "EEDB_7_DATA",
        "EEDB_8_LL",
        "EEDB_8_DATA",
        "EEDB_7_8_DATA",
        "EOEM_0",
        "EOEM_1",
        "ESEM",
        "EVSI",
        "EXEM_1",
        "EXEM_2",
        "EXEM_3",
        "EXEM_4",
        "RMEP"
    };

    if (mdb_phy_table_id < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
    {
        return mdb_physical_table_strings[mdb_phy_table_id];
    }
    return unknown_mdb_string;
}

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
shr_error_e
mdb_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_enum_value_field_mdb_physical_table_e * mdb_phy_table_id)
{
    int ii;
    char *mdb_table_name;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES; ii++)
    {
        mdb_table_name = mdb_physical_table_to_string(unit, ii);
        if (sal_strcasecmp(mdb_table_name, str) == 0)
        {
            *mdb_phy_table_id = (dbal_enum_value_field_mdb_physical_table_e) ii;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "mdb_physical_table_string_to_id: " "mdb phy table not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_phy_table_id_print(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id,
    int detail)
{
    dbal_enum_value_field_mdb_db_type_e db_type;
    int nof_clusters, cluster_idx;
    mdb_cluster_info_t cluster_info;
    char *phydb_names, *dbtype_names, *dbalphytb_name;
    char str_info[32];
    dbal_physical_tables_e dbal_physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    dbal_physical_table_id = mdb_direct_mdb_to_dbal[mdb_phy_table_id];
    dbalphytb_name = dbal_physical_table_to_string(unit, dbal_physical_table_id);

    phydb_names = mdb_physical_table_to_string(unit, mdb_phy_table_id);
    db_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_phy_table_id)->db_type;
    dbtype_names = mdb_get_db_type_str(db_type);
    if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM) || (db_type >= DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES))
    {
        cli_out("%-16s%-16s%-16s\n", phydb_names, dbtype_names, dbalphytb_name);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_phy_table_id, &nof_clusters));
    cli_out("%-16s%-16s%-16s%-10d\n", phydb_names, dbtype_names, dbalphytb_name, nof_clusters);
    if (detail)
    {
        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            int payload_size;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_phy_table_id, cluster_idx, &cluster_info));
            cli_out("%-16s", "");
            sal_snprintf(str_info, sizeof(str_info), "%s-%d-%d",
                         cluster_info.macro_type == MDB_MACRO_A ? "DHA" : cluster_info.macro_type ==
                         MDB_MACRO_B ? "DHB" : "EEDB", cluster_info.macro_index, cluster_info.cluster_index);
            cli_out("%-12s", str_info);
            cli_out("%-4d", cluster_info.cluster_position_in_entry);

            if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_phy_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {
                SHR_IF_ERR_EXIT(mdb_eedb_get_payload_size_from_cluster(unit, &cluster_info, (uint32 *) &payload_size));
                payload_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_size);
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_phy_table_id, &payload_size));
            }

            sal_snprintf(str_info, sizeof(str_info), "%db", payload_size);
            cli_out("%-6s", str_info);
            sal_snprintf(str_info, sizeof(str_info), "%#x-%#x", cluster_info.start_address, cluster_info.end_address);
            cli_out("%-18s\n", str_info);
        }
        if (nof_clusters != 0)
        {
            cli_out("%-16s%s\n", "", "--------------------------------------");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_macro_map_info_dump(
    int unit)
{
    soc_mem_t mem;
    int macro_idx, blk, macro_idx_max;
    mdb_macro_types_e macro_type;

    SHR_FUNC_INIT_VARS(unit);

    macro_type = MDB_MACRO_A;
    macro_idx_max = dnx_data_mdb.dh.nof_macroes_type_A_get(unit);
    cli_out("MDB_MACRO_A: macro_idx_max=%d\n", macro_idx_max);
    for (macro_idx = 0; macro_idx < macro_idx_max; macro_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, macro_type, macro_idx, &mem, NULL, NULL, &blk));
        cli_out("    %-6d%-30s%-6d\n", macro_idx, SOC_MEM_NAME(unit, mem), blk);
    }

    macro_type = MDB_MACRO_B;
    macro_idx_max = dnx_data_mdb.dh.nof_macroes_type_B_get(unit);
    cli_out("MDB_MACRO_B: macro_idx_max=%d\n", macro_idx_max);
    for (macro_idx = 0; macro_idx < macro_idx_max; macro_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, macro_type, macro_idx, &mem, NULL, NULL, &blk));
        cli_out("    %-6d%-30s%-6d\n", macro_idx, SOC_MEM_NAME(unit, mem), blk);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_phy_table_db_info_dump(
    int unit,
    int detail)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("%-16s%-16s%-16s%-10s\n", "MDB_TABLE", "Type", "DBAL_TABLE", "#Clusters");
    cli_out("----------------------------------------------------------\n");
    for (ii = 0; ii < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES; ii++)
    {
        SHR_IF_ERR_EXIT(mdb_phy_table_id_print(unit, ii, detail));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_mdb_dh_cluster_get_mem_index_update(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 bucket_idx,
    uint32 cluster_off,
    uint32 row_idx,
    uint32 *mem_index)
{
    shr_error_e rc;
    uint32 cluster_idx;
    mdb_macro_types_e macro_type;
    uint32 macro_index;
    dbal_physical_tables_e dbal_physical_table_id;
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS];
    int nof_valid_clusters;
    int entry_size;

    SHR_FUNC_INIT_VARS(unit);

    cluster_idx = bucket_idx * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) + cluster_off;

    SHR_IF_ERR_EXIT(mdb_ser_map_access_mem_to_macro_index(unit, blk, mem, &macro_type, &macro_index));

    rc = mdb_ser_map_cluster_to_dbal_phy_table(unit, macro_type, macro_index, cluster_idx, &dbal_physical_table_id);
    if (rc == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_table_id, clusters, &nof_valid_clusters));
        entry_size = clusters[0].entry_size;
    }
    else
    {
        entry_size = dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
    }

    *mem_index = row_idx & MDB_SER_ADDR_OFFSET_MASK(mem);
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                    (mem_index,
                     MDB_SER_ADDR_OFFSET_OFFSET(mem),
                     MDB_SER_ADDR_OFFSET_OFFSET(mem) +
                     (MDB_SER_ADDR_OFFSET_BITS * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) - 1));
    if (entry_size <= dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                        (mem_index,
                         MDB_SER_ADDR_OFFSET_OFFSET(mem) + (MDB_SER_ADDR_OFFSET_BITS * cluster_off),
                         MDB_SER_ADDR_OFFSET_OFFSET(mem) + (MDB_SER_ADDR_OFFSET_BITS * cluster_off) + 1));
    }
    else if (entry_size <= 2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
    {
        cluster_off = cluster_off - (cluster_off & 0x1);
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                        (mem_index,
                         MDB_SER_ADDR_OFFSET_OFFSET(mem) + (MDB_SER_ADDR_OFFSET_BITS * cluster_off),
                         MDB_SER_ADDR_OFFSET_OFFSET(mem) + (MDB_SER_ADDR_OFFSET_BITS * cluster_off) + 3));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                        (mem_index,
                         MDB_SER_ADDR_OFFSET_OFFSET(mem),
                         MDB_SER_ADDR_OFFSET_OFFSET(mem) +
                         (MDB_SER_ADDR_OFFSET_BITS * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_mdb_dh_cluster_entry_get(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 bucket_idx,
    uint32 cluster_off,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 *ecc)
{
    uint32 row_data[MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS];
    uint32 mem_index = row_idx;
    soc_field_t ecc_field[DNX_DATA_MAX_MDB_DH_NOF_BUCKET_CLUSTERS] = { ECC_0f, ECC_1f, ECC_2f, ECC_3f };
    soc_field_t data_field[DNX_DATA_MAX_MDB_DH_NOF_BUCKET_CLUSTERS] = { DATA_0f, DATA_1f, DATA_2f, DATA_3f };

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(row_data, 0x0, sizeof(row_data));

    SHR_IF_ERR_EXIT(_mdb_dh_cluster_get_mem_index_update(unit, mem, blk, bucket_idx, cluster_off, row_idx, &mem_index));
    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, bucket_idx, blk, mem_index, row_data));

    *ecc = soc_mem_field32_get(unit, mem, row_data, ecc_field[cluster_off]);
    soc_mem_field_get(unit, mem, row_data, data_field[cluster_off], entry_data);

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                                          "\n    mem=%s, blk=%d, bucket=%d, cluster=%d, row=%d, mem_index=0x%x\n"
                                          "    %-16s%08X %08X %08X %08X\n"
                                          "    %-16s%X\n\n"),
                               SOC_MEM_NAME(unit, mem), blk, bucket_idx, cluster_off, row_idx, mem_index,
                               "entry_data:", entry_data[0], entry_data[1], entry_data[2], entry_data[3],
                               "ECC:", *ecc));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_mdb_dh_cluster_entry_set(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 bucket_idx,
    uint32 cluster_off,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 ecc)
{
    uint32 control;
    uint32 row_data[MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS];
    soc_field_t ecc_field[DNX_DATA_MAX_MDB_DH_NOF_BUCKET_CLUSTERS] = { ECC_0f, ECC_1f, ECC_2f, ECC_3f };
    soc_field_t data_field[DNX_DATA_MAX_MDB_DH_NOF_BUCKET_CLUSTERS] = { DATA_0f, DATA_1f, DATA_2f, DATA_3f };
    soc_field_t command_fields[DNX_DATA_MAX_MDB_DH_NOF_BUCKET_CLUSTERS] =
        { COMMAND_0f, COMMAND_1f, COMMAND_2f, COMMAND_3f };
    uint32 mem_index;

    SHR_FUNC_INIT_VARS(unit);

    mem_index = row_idx & MDB_SER_ADDR_OFFSET_MASK(mem);

    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                    (&mem_index,
                     MDB_SER_ADDR_OFFSET_OFFSET(mem),
                     MDB_SER_ADDR_OFFSET_OFFSET(mem) +
                     MDB_SER_ADDR_OFFSET_BITS * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1));
    /** coverity[ARRAY_VS_SINGLETON:FALSE] */
    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                    (&mem_index,
                     MDB_SER_ADDR_OFFSET_OFFSET(mem) + MDB_SER_ADDR_OFFSET_BITS * cluster_off,
                     MDB_SER_ADDR_OFFSET_OFFSET(mem) + MDB_SER_ADDR_OFFSET_BITS * cluster_off + 1));

    control = MDB_SER_SET_CONTROL_BITS(MDB_CLUSTER_WRITE_INST, MDB_SER_CLUSTER_OFFSET_NONE);
    soc_mem_field_set(unit, mem, row_data, command_fields[cluster_off], &control);

    soc_mem_field32_set(unit, mem, row_data, ecc_field[cluster_off], ecc);
    soc_mem_field_set(unit, mem, row_data, data_field[cluster_off], entry_data);

    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, mem, bucket_idx, blk, mem_index, row_data));

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                                          "\n    mem=%s, blk=%d, bucket=%d, cluster=%d, row=%d, mem_index=0x%x\n"
                                          "    %-16s%08X %08X %08X %08X\n"
                                          "    %-16s%X\n\n"),
                               SOC_MEM_NAME(unit, mem), blk, bucket_idx, cluster_off, row_idx, mem_index,
                               "entry_data:", entry_data[0], entry_data[1], entry_data[2], entry_data[3], "ECC:", ecc));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_mdb_phy_mem_2_access_mem(
    int unit,
    soc_mem_t phy_mem,
    soc_mem_t * access_mem)
{
    int rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    switch (phy_mem)
    {
        case DDHA_MACRO_0_PHYSICAL_ENTRY_BANKm:
            *access_mem = DDHA_MACRO_0_ENTRY_BANKm;
            break;
        case DDHA_MACRO_1_PHYSICAL_ENTRY_BANKm:
            *access_mem = DDHA_MACRO_1_ENTRY_BANKm;
            break;
        case DDHB_MACRO_0_PHYSICAL_ENTRY_BANKm:
            *access_mem = DDHB_MACRO_0_ENTRY_BANKm;
            break;
        case DDHB_MACRO_1_PHYSICAL_ENTRY_BANKm:
            *access_mem = DDHB_MACRO_1_ENTRY_BANKm;
            break;
        case DHC_MACRO_PHYSICAL_ENTRY_BANKm:
            *access_mem = DHC_MACRO_ENTRY_BANKm;
            break;
        default:
            rc = _SHR_E_PARAM;
            break;
    }
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_mdb_ser_enable_emp_way(
    int unit,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    if (enable)
    {
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, MDB_REG_173r, REG_PORT_ANY, ITEM_0_0f, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, MDB_REG_173r, REG_PORT_ANY, ITEM_0_0f, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_macro_cluster_param_validate(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_index,
    uint32 cluster_idx,
    uint32 row_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cluster_idx >= dnx_data_mdb.dh.nof_macro_clusters_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. cluster_idx==%d is not supported.\n", cluster_idx);
    }
    if (macro_type == MDB_MACRO_A)
    {
        if (macro_index >= dnx_data_mdb.dh.nof_macroes_type_A_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_A, macro_index %d is not supported.\n",
                         macro_index);
        }

        if (row_idx >= MDB_NOF_ROWS_IN_MACRO_A_CLUSTER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_A, row_idx %d is not supported.\n", row_idx);
        }
    }
    else if (macro_type == MDB_MACRO_B)
    {
        if (macro_index >= dnx_data_mdb.dh.nof_macroes_type_B_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_B, macro_index %d is not supported.\n",
                         macro_index);
        }
        if (row_idx >= MDB_NOF_ROWS_IN_MACRO_B_CLUSTER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_B, row_idx %d is not supported.\n", row_idx);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type %d is not supported.\n", macro_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_macro_cluster_entry_get(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 *ecc)
{
    soc_mem_t mem;
    int blk;
    uint32 bucket_idx, cluster_off;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_param_validate(unit, macro_type, macro_idx, cluster_idx, row_idx));

    SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, macro_type, macro_idx, &mem, NULL, NULL, &blk));

    bucket_idx = cluster_idx / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    cluster_off = cluster_idx % dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    SHR_IF_ERR_EXIT(_mdb_dh_cluster_entry_get(unit, mem, blk, bucket_idx, cluster_off, row_idx, entry_data, ecc));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_macro_cluster_entry_set(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx,
    uint32 row_idx,
    uint32 *entry_data,
    uint32 ecc)
{
    soc_mem_t mem;
    int blk;
    uint32 bucket_idx, cluster_off;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_param_validate(unit, macro_type, macro_idx, cluster_idx, row_idx));

    SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, macro_type, macro_idx, &mem, NULL, NULL, &blk));

    bucket_idx = cluster_idx / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    cluster_off = cluster_idx % dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    SHR_IF_ERR_EXIT(_mdb_dh_cluster_entry_set(unit, mem, blk, bucket_idx, cluster_off, row_idx, entry_data, ecc));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_physical_mem_param_validate(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    if (array_idx >= dnx_data_mdb.dh.nof_macro_clusters_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. array_idx==%d is not supported.\n", array_idx);
    }
    if ((phy_mem == DDHA_MACRO_0_PHYSICAL_ENTRY_BANKm) || (phy_mem == DDHA_MACRO_1_PHYSICAL_ENTRY_BANKm))
    {
        if ((blk - SOC_MEM_BLOCK_MIN(unit, phy_mem)) >= SOC_MAX_NUM_DDHA_BLKS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. phy_mem==%s, blk %d is not supported.\n",
                         SOC_MEM_NAME(unit, phy_mem), blk);
        }

        if (entry_idx >= MDB_NOF_ROWS_IN_MACRO_A_CLUSTER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_A, entry_idx %d is not supported.\n", entry_idx);
        }
    }
    else if ((phy_mem == DDHB_MACRO_0_PHYSICAL_ENTRY_BANKm) || (phy_mem == DDHB_MACRO_1_PHYSICAL_ENTRY_BANKm))
    {
        if ((blk - SOC_MEM_BLOCK_MIN(unit, phy_mem)) >= SOC_MAX_NUM_DDHB_BLKS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. phy_mem==%s, blk %d is not supported.\n",
                         SOC_MEM_NAME(unit, phy_mem), blk);
        }
        if (entry_idx >= MDB_NOF_ROWS_IN_MACRO_B_CLUSTER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_B, entry_idx %d is not supported.\n", entry_idx);
        }
    }
    else if (phy_mem == DHC_MACRO_PHYSICAL_ENTRY_BANKm)
    {
        if ((blk - SOC_MEM_BLOCK_MIN(unit, phy_mem)) >= SOC_MAX_NUM_DHC_BLKS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. phy_mem==%s, blk %d is not supported.\n",
                         SOC_MEM_NAME(unit, phy_mem), blk);
        }
        if (entry_idx >= MDB_NOF_ROWS_IN_MACRO_A_CLUSTER)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. macro_type==MDB_MACRO_A, entry_idx %d is not supported.\n", entry_idx);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. phy_mem %s is not supported.\n", SOC_MEM_NAME(unit, phy_mem));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_physical_mem_entry_get(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data)
{
    soc_mem_t mem;
    uint32 bucket_idx, cluster_off;
    uint32 ecc, row_data[4];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_physical_mem_param_validate(unit, phy_mem, blk, array_idx, entry_idx));

    SHR_IF_ERR_EXIT(_mdb_phy_mem_2_access_mem(unit, phy_mem, &mem));

    bucket_idx = array_idx / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    cluster_off = array_idx % dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    SHR_IF_ERR_EXIT(_mdb_dh_cluster_entry_get(unit, mem, blk, bucket_idx, cluster_off, entry_idx, row_data, &ecc));

    soc_mem_field_set(unit, phy_mem, entry_data, DATAf, row_data);
    soc_mem_field32_set(unit, phy_mem, entry_data, ECCf, ecc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_physical_mem_entry_set(
    int unit,
    soc_mem_t phy_mem,
    int blk,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *entry_data)
{
    soc_mem_t mem;
    uint32 bucket_idx, cluster_off;
    uint32 ecc, row_data[4];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_physical_mem_param_validate(unit, phy_mem, blk, array_idx, entry_idx));

    SHR_IF_ERR_EXIT(_mdb_phy_mem_2_access_mem(unit, phy_mem, &mem));

    bucket_idx = array_idx / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    cluster_off = array_idx % dnx_data_mdb.dh.nof_bucket_clusters_get(unit);

    soc_mem_field_get(unit, phy_mem, entry_data, DATAf, row_data);
    ecc = soc_mem_field32_get(unit, phy_mem, entry_data, ECCf);
    SHR_IF_ERR_EXIT(_mdb_dh_cluster_entry_set(unit, mem, blk, bucket_idx, cluster_off, entry_idx, row_data, ecc));

exit:
    SHR_FUNC_EXIT;
}

void
mdb_ser_print_cluster_entry(
    int unit,
    uint32 *entry_data,
    uint32 ecc,
    char *print_msg)
{
    uint32 print_ecc;
    char tmp[(MDB_SER_ENTRY_SIZE_IN_U32 * 8) + 3];

    if (ecc <= 0xFF)
    {
        print_ecc = ecc;
    }
    else
    {
        print_ecc = soc_mem_field32_get(unit, DDHA_MACRO_0_PHYSICAL_ENTRY_BANKm, entry_data, ECCf);
        soc_mem_field32_set(unit, DDHA_MACRO_0_PHYSICAL_ENTRY_BANKm, entry_data, ECCf, 0);
    }

    _shr_format_long_integer(tmp, entry_data, MDB_SER_ENTRY_DATA_SIZE_IN_U8);
    cli_out("%sDATA=%-32s,    ECC=%02X\n", print_msg, tmp, print_ecc);

    return;
}

shr_error_e
mdb_dh_macro_cluster_chg_dump(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx)
{
    uint32 ii, row_index_max = 0;
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ecc;
    char print_msg[256];

    SHR_FUNC_INIT_VARS(unit);

    if (macro_type == MDB_MACRO_A)
    {
        row_index_max = MDB_NOF_ROWS_IN_MACRO_A_CLUSTER;
    }
    else if (macro_type == MDB_MACRO_B)
    {
        row_index_max = MDB_NOF_ROWS_IN_MACRO_B_CLUSTER;
    }

    for (ii = 0; ii < row_index_max; ii++)
    {
        sal_memset(entry_data, 0, sizeof(entry_data));
        ecc = 0;
        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_entry_get(unit, macro_type, macro_idx, cluster_idx, ii, entry_data, &ecc));
        if (utilex_bitstream_have_one_in_range(entry_data, 0, MDB_SER_ENTRY_DATA_SIZE_IN_BITS - 1))
        {
            sal_snprintf(print_msg, sizeof(print_msg), "    %s-%d, cluster=%d, index=%-6d: ",
                         macro_type == MDB_MACRO_A ? "MACRO_A" : "MACRO_B", macro_idx, cluster_idx, ii);
            mdb_ser_print_cluster_entry(unit, entry_data, ecc, print_msg);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_phy_table_cluster_chg_dump(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id)
{
    dbal_enum_value_field_mdb_db_type_e db_type;
    int nof_clusters, cluster_idx;
    mdb_cluster_info_t cluster_info;
    char *phydb_names, *dbtype_names;

    SHR_FUNC_INIT_VARS(unit);

    phydb_names = mdb_physical_table_to_string(unit, mdb_phy_table_id);
    db_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_phy_table_id)->db_type;
    dbtype_names = mdb_get_db_type_str(db_type);
    if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM) || (db_type >= DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_phy_table_id, &nof_clusters));
    cli_out("mdb physical table: %s,  db_type: %s  cluster numbers: %d\n", phydb_names, dbtype_names, nof_clusters);

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_phy_table_id, cluster_idx, &cluster_info));

        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_chg_dump
                        (unit, cluster_info.macro_type, cluster_info.macro_index, cluster_info.cluster_index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_macro_cluster_cache_check(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_idx,
    uint32 cluster_idx)
{

    uint32 ii, row_index_max = 0;
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 cache_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ecc;
    dbal_physical_tables_e dbal_id;
    dbal_enum_value_field_mdb_db_type_e db_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_ser_map_cluster_to_dbal_phy_table(unit, macro_type, macro_idx, cluster_idx, &dbal_id));

    db_type = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_id)->db_type;
    if ((db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) && (db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. cache check only support for direct/eedb/kaps dbal table.\n");
    }

    cli_out("    %s, index-%d, cluster-%d, dbal-table=%s, db_type=%s\n",
            macro_type == MDB_MACRO_A ? "MDB_MACRO_A" : macro_type == MDB_MACRO_B ? "MDB_MACRO_B" : "EEDB",
            macro_idx, cluster_idx, dbal_physical_table_to_string(unit, dbal_id), mdb_get_db_type_str(db_type));

    if (macro_type == MDB_MACRO_A)
    {
        row_index_max = MDB_NOF_ROWS_IN_MACRO_A_CLUSTER;
    }
    else if (macro_type == MDB_MACRO_B)
    {
        row_index_max = MDB_NOF_ROWS_IN_MACRO_B_CLUSTER;
    }
    for (ii = 0; ii < row_index_max; ii++)
    {
        sal_memset(entry_data, 0, sizeof(entry_data));
        sal_memset(cache_data, 0, sizeof(cache_data));
        ecc = 0;
        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_entry_get(unit, macro_type, macro_idx, cluster_idx, ii, entry_data, &ecc));

        SHR_IF_ERR_EXIT(mdb_direct_cache_get(unit, macro_type, macro_idx, cluster_idx, ii, 0, cache_data));

        if ((entry_data[0] != cache_data[0]) || (entry_data[1] != cache_data[1]) ||
            (entry_data[2] != cache_data[2]) || (entry_data[3] != cache_data[3]))
        {
            cli_out("\t%-6d: %08X %08X %08X %08X        %08X %08X %08X %08X\n", ii,
                    entry_data[0], entry_data[1], entry_data[2], entry_data[3],
                    cache_data[0], cache_data[1], cache_data[2], cache_data[3]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_phy_table_cluster_cache_check(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_phy_table_id)
{
    dbal_enum_value_field_mdb_db_type_e db_type;
    int nof_clusters, cluster_idx;
    mdb_cluster_info_t cluster_info;
    char *phydb_names, *dbtype_names;

    SHR_FUNC_INIT_VARS(unit);

    phydb_names = mdb_physical_table_to_string(unit, mdb_phy_table_id);
    db_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_phy_table_id)->db_type;
    dbtype_names = mdb_get_db_type_str(db_type);
    if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM) || (db_type >= DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_phy_table_id, &nof_clusters));

    cli_out("mdb physical table: %s,  db_type: %s  cluster numbers: %d\n", phydb_names, dbtype_names, nof_clusters);
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_phy_table_id, cluster_idx, &cluster_info));

        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_cache_check
                        (unit, cluster_info.macro_type, cluster_info.macro_index, cluster_info.cluster_index));
        cli_out("\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_map_access_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t access_mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index)
{
    mdb_macro_types_e tmp_type;
    uint32 ii, tmp_index, macro_idx_max;
    soc_mem_t mem1;
    int blk1;

    SHR_FUNC_INIT_VARS(unit);

    macro_idx_max = dnx_data_mdb.dh.nof_macroes_type_A_get(unit) + dnx_data_mdb.dh.nof_macroes_type_B_get(unit);
    for (ii = 0; ii < macro_idx_max; ii++)
    {
        if (ii < dnx_data_mdb.dh.nof_macroes_type_A_get(unit))
        {
            tmp_type = MDB_MACRO_A;
            tmp_index = ii;
        }
        else
        {
            tmp_type = MDB_MACRO_B;
            tmp_index = ii - dnx_data_mdb.dh.nof_macroes_type_A_get(unit);
        }
        SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, tmp_type, tmp_index, &mem1, NULL, NULL, &blk1));
        if ((access_mem == mem1) && (blk == blk1))
        {
            if (macro_type != NULL)
            {
                *macro_type = tmp_type;
            }
            if (macro_index != NULL)
            {
                *macro_index = tmp_index;
            }

            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. Can't found macro index for mdb access_mem %s, blk %d\n",
                 SOC_MEM_NAME(unit, access_mem), blk);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_map_mem_to_macro_index(
    int unit,
    int blk,
    soc_mem_t mem,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index)
{
    soc_mem_t access_mem;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_mdb_phy_mem_2_access_mem(unit, mem, &access_mem));
    SHR_IF_ERR_EXIT(mdb_ser_map_access_mem_to_macro_index(unit, blk, access_mem, macro_type, macro_index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_map_cluster_to_dbal_phy_table(
    int unit,
    mdb_macro_types_e macro_type,
    uint32 macro_index,
    uint32 cluster_index,
    dbal_physical_tables_e * dbal_physical_table_id)
{
    uint32 dbal_id, ii;
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS];
    int nof_valid_clusters;

    SHR_FUNC_INIT_VARS(unit);
    for (dbal_id = 1; dbal_id < DBAL_NOF_PHYSICAL_TABLES; dbal_id++)
    {
        /*
         * Only for MDB tables
         */
        if ((dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM) ||
            (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_id)->db_type == DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_id, clusters, &nof_valid_clusters));
        for (ii = 0; ii < nof_valid_clusters; ii++)
        {
            if ((clusters[ii].macro_type == macro_type) &&
                (clusters[ii].macro_idx == macro_index) && (clusters[ii].cluster_idx == cluster_index))
            {
                if (dbal_physical_table_id != NULL)
                {
                    *dbal_physical_table_id = dbal_id;
                }
                SHR_EXIT();
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Can't map macro_type %d macro_index %d, cluster_index %d to DBAL\n"),
                              macro_type, macro_index, cluster_index));
    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_ecc_1b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx)
{
    int rc;
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32], correct_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ii, eci_global_value, xor_correct_data[MDB_SER_ENTRY_SIZE_IN_U32] = { 0 };
    uint32 row_bit_width;
    uint32 ecc_field, ecc_field_prev;
    uint32 emp_way_disable = 0;
    dnxc_xor_mem_info xor_mem_info;
    int offset, correct_index, sp_index, sp_bank_size, sp_banks_num;

    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nmem=%s, blk=%d, array_idx=%d, entry_idx=%d\n\n"),
                                 SOC_MEM_NAME(unit, mem), blk, array_idx, entry_idx));

    rc = dnxc_xor_mem_info_get(unit, mem, &xor_mem_info);
    SHR_IF_ERR_EXIT_WITH_LOG(rc, "The memory %s is not XOR memory.%s%s\n", SOC_MEM_NAME(unit, mem), EMPTY, EMPTY);

    sp_banks_num = 1 << xor_mem_info.sram_banks_bits;
    sp_bank_size = SOC_MEM_ELEM_SKIP(unit, mem) / sp_banks_num;
    offset = entry_idx & ((1 << xor_mem_info.entry_used_bits) - 1);

    SHR_IF_ERR_EXIT(_mdb_ser_enable_emp_way(unit, 0));
    emp_way_disable = 1;

    /*
     * Update all SP banks value 
     */
    for (sp_index = 0; sp_index < sp_banks_num; sp_index++)
    {
        sal_memset(entry_data, 0, sizeof(entry_data));
        correct_index = offset + sp_index * sp_bank_size;

        SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, mem, blk, array_idx, correct_index, entry_data));

        sal_memcpy(correct_data, entry_data, sizeof(correct_data));
        row_bit_width = MDB_SER_ENTRY_DATA_SIZE_IN_BITS;
        ecc_field = soc_mem_field32_get(unit, mem, correct_data, ECCf);
        ecc_field_prev = ecc_field;
        rc = sand_mdb_ecc_correction(unit, row_bit_width, correct_data, &ecc_field);
        if (SHR_FAILURE(rc))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "MDB ECC 1bit correct fail at correct_index %d\n"), correct_index));
            continue;
        }
        if (ecc_field_prev != ecc_field)
        {
            soc_mem_field32_set(unit, mem, correct_data, ECCf, ecc_field);
        }

        if ((entry_data[0] != correct_data[0]) || (entry_data[1] != correct_data[1]) ||
            (entry_data[2] != correct_data[2]) || (entry_data[3] != correct_data[3]))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "MDB ECC 1bit correction at correct_index %d\n"), correct_index));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Before correction: entry: %08X %08X %08X %08X\n"),
                         entry_data[0], entry_data[1], entry_data[2], entry_data[3]));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "After correction:  entry: %08X %08X %08X %08X\n\n"),
                         correct_data[0], correct_data[1], correct_data[2], correct_data[3]));
        }
        SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, mem, blk, array_idx, correct_index, correct_data));

        for (ii = 0; ii < MDB_SER_ENTRY_SIZE_IN_U32; ii++)
        {
            xor_correct_data[ii] ^= correct_data[ii];
        }
    }

    /*
     * Update XOR banks value 
     */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "XOR bank entry update: offset=%d, xor_correct_data: %08X %08X %08X %08X\n\n"),
                 offset, xor_correct_data[0], xor_correct_data[1], xor_correct_data[2], xor_correct_data[3]));
    SHR_IF_ERR_EXIT(READ_ECI_GLOBAL_MEM_OPTIONSr(unit, &eci_global_value));
    soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));
    rc = mdb_dh_physical_mem_entry_set(unit, mem, blk, array_idx, offset, xor_correct_data);
    soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));
    SHR_IF_ERR_EXIT(rc);

exit:
    if (emp_way_disable)
    {
        SHR_IF_ERR_CONT(_mdb_ser_enable_emp_way(unit, 1));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_ecc_2b_clear_entry(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx)
{
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    dnxc_xor_mem_info xor_mem_info;
    int offset, correct_index, sp_index, sp_bank_size, sp_banks_num;
    int rc;
    uint32 eci_global_value;

    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_xor_mem_info_get(unit, mem, &xor_mem_info);
    SHR_IF_ERR_EXIT_WITH_LOG(rc, "The memory %s is not XOR memory.%s%s\n", SOC_MEM_NAME(unit, mem), EMPTY, EMPTY);

    sp_banks_num = 1 << xor_mem_info.sram_banks_bits;
    sp_bank_size = SOC_MEM_ELEM_SKIP(unit, mem) / sp_banks_num;
    offset = entry_idx & ((1 << xor_mem_info.entry_used_bits) - 1);

    /*
     * Clear entry for all SP banks value 
     */
    for (sp_index = 0; sp_index < sp_banks_num; sp_index++)
    {
        sal_memset(entry_data, 0, sizeof(entry_data));
        correct_index = offset + sp_index * sp_bank_size;

        SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, mem, blk, array_idx, correct_index, entry_data));
    }

    /*
     * Clear XOR banks value 
     */
    sal_memset(entry_data, 0, sizeof(entry_data));
    SHR_IF_ERR_EXIT(READ_ECI_GLOBAL_MEM_OPTIONSr(unit, &eci_global_value));
    soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));
    rc = mdb_dh_physical_mem_entry_set(unit, mem, blk, array_idx, offset, entry_data);
    soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_ecc_2b_direct_sync_cache(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx)
{
    shr_error_e rc;
    mdb_macro_types_e macro_type;
    uint32 macro_index, cluster_index = array_idx;
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    dnxc_xor_mem_info xor_mem_info;
    int offset, correct_index, sp_index, sp_bank_size, sp_banks_num;

    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_xor_mem_info_get(unit, mem, &xor_mem_info);
    SHR_IF_ERR_EXIT_WITH_LOG(rc, "The memory %s is not XOR memory.%s%s\n", SOC_MEM_NAME(unit, mem), EMPTY, EMPTY);

    sp_banks_num = 1 << xor_mem_info.sram_banks_bits;
    sp_bank_size = SOC_MEM_ELEM_SKIP(unit, mem) / sp_banks_num;
    offset = entry_idx & ((1 << xor_mem_info.entry_used_bits) - 1);

    SHR_IF_ERR_EXIT(mdb_ser_map_mem_to_macro_index(unit, blk, mem, &macro_type, &macro_index));

    /*
     * Sync entry from cache for all SP banks value 
     */
    for (sp_index = 0; sp_index < sp_banks_num; sp_index++)
    {
        sal_memset(entry_data, 0, sizeof(entry_data));
        correct_index = offset + sp_index * sp_bank_size;

        rc = mdb_direct_cache_get(unit, macro_type, macro_index, cluster_index, correct_index, 0, entry_data);
        if ((rc == _SHR_E_NONE) || (rc == _SHR_E_NOT_FOUND))
        {
            SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, mem, blk, array_idx, correct_index, entry_data));
        }
        else
        {
            SHR_IF_ERR_EXIT(rc);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_ecc_2b_em_sync_cache(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx,
    dbal_physical_tables_e dbal_physical_table_id)
{
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS];
    int nof_valid_clusters;
    soc_mem_t tid_atr;
    soc_mem_info_t mem_info;
    int nof_app_ids;
    uint32 app_id;
    int iterator_init = FALSE;
    dbal_physical_entry_iterator_t physical_entry_iterator;
    dbal_physical_entry_t entry;
    uint8 is_end = FALSE;
    bsl_severity_t severity;
    shr_error_e rc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_table_id, clusters, &nof_valid_clusters));

    /*
     * first clear entry with ecc error 
     */
    if (clusters[0].entry_size == 240)
    {
        array_idx = array_idx - (array_idx & 0x1);
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_clear_entry(unit, blk, mem, array_idx, entry_idx));
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_clear_entry(unit, blk, mem, array_idx + 1, entry_idx));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_clear_entry(unit, blk, mem, array_idx, entry_idx));
    }

    tid_atr = dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->tid_atr_mem;
    mem_info = SOC_MEM_INFO(unit, tid_atr);
    nof_app_ids = mem_info.index_max + 1;
    physical_entry_iterator.hit_bit_flags = 0;
    physical_entry_iterator.age_flags = 0;
    for (app_id = 0; app_id < nof_app_ids; app_id++)
    {
        SHR_IF_ERR_EXIT(mdb_em_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
        iterator_init = TRUE;

        SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                        (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));
        while (is_end == FALSE)
        {
            int ii;
            SHR_GET_SEVERITY_FOR_MODULE(severity);
            if (severity >= bslSeverityInfo)
            {
                cli_out("mdb_ser_ecc_2b_em_sync_cache: dbal_physical_table is %s, app_id=%d\n",
                        dbal_physical_table_to_string(unit, dbal_physical_table_id), app_id);
                cli_out("\tKEY size=%d:        ", entry.key_size);
                for (ii = 0; ii < BITS2WORDS(entry.key_size); ii++)
                {
                    cli_out("%08X ", entry.key[ii]);
                }
                cli_out("\n");
                cli_out("\tPAYLOAD size=%d:    ", entry.payload_size);
                for (ii = 0; ii < BITS2WORDS(entry.payload_size); ii++)
                {
                    cli_out("%08X ", entry.payload[ii]);
                }
                cli_out("\n");
            }

            entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            rc = mdb_em_entry_get(unit, dbal_physical_table_id, app_id, &entry);
            if (rc == _SHR_E_NOT_FOUND)
            {
                cli_out("EM entry was not found at HW, re-write it!!!!\n");
                SHR_IF_ERR_EXIT(mdb_em_entry_add(unit, dbal_physical_table_id, app_id, &entry));
            }

            SHR_IF_ERR_EXIT(mdb_em_iterator_get_next
                            (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));
        }
    }

exit:
    if (iterator_init == TRUE)
    {
        SHR_IF_ERR_CONT(mdb_em_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_ser_ecc_2b_interrupt_data_correct(
    int unit,
    int blk,
    soc_mem_t mem,
    uint32 array_idx,
    uint32 entry_idx)
{
    shr_error_e rc;
    mdb_macro_types_e macro_type;
    uint32 macro_index;
    dbal_physical_tables_e dbal_physical_table_id;
    uint32 cluster_index = array_idx;

    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nmem=%s, blk=%d, array_idx=%d, entry_idx=%d\n\n"),
                                 SOC_MEM_NAME(unit, mem), blk, array_idx, entry_idx));

    SHR_IF_ERR_EXIT(mdb_ser_map_mem_to_macro_index(unit, blk, mem, &macro_type, &macro_index));

    rc = mdb_ser_map_cluster_to_dbal_phy_table(unit, macro_type, macro_index, cluster_index, &dbal_physical_table_id);
    if (rc != _SHR_E_NONE)
    {
        /*
         * cluster was not allocated to any dbal table, just clear this entry 
         */
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_clear_entry(unit, blk, mem, array_idx, entry_idx));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(mdb_ser_map_cluster_to_dbal_phy_table
                    (unit, macro_type, macro_index, cluster_index, &dbal_physical_table_id));

    if ((dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) ||
        (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
    {
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_direct_sync_cache(unit, blk, mem, array_idx, entry_idx));
    }
    else if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        /*
         * cache for mdb type kaps was not ready yet. Include Big ADs and Big BBs
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: ECC 2bit entry was allocated Kaps dbal table.\n");
    }
    else if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_IF_ERR_EXIT(mdb_ser_ecc_2b_em_sync_cache(unit, blk, mem, array_idx, entry_idx, dbal_physical_table_id));
    }
    else if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: ECC 2bit entry was allocated TCAM dbal table.\n");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: ECC 2bit entry was allocated invalid dbal table.\n");
    }

exit:
    SHR_FUNC_EXIT;
}
