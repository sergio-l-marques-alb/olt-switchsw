/*
 * ! \file mdb_lpm_xpt.c Contains all of the MDB KAPS XPT callbacks provided to the KBPSDK.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/mdb.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)

#include <soc/dnx/mdb.h>
#include "mdb_internal.h"
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/dnxc_ha.h>

#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>
#include <soc/dnx/mdb_sbusdma_desc.h>

#define MDB_LPM_KEY_BUFFER_NOF_BYTES           (20)

#define MDB_LPM_RPB_FIRST_BLK_ID           1
#define MDB_LPM_RPB_LAST_BLK_ID            2
#define MDB_LPM_BB_FIRST_BLK_ID            10
#define MDB_LPM_BB_LAST_BLK_ID             25
#define MDB_LPM_BB_NOF_BLK_ID              (MDB_LPM_BB_LAST_BLK_ID - MDB_LPM_BB_FIRST_BLK_ID + 1)

#define MDB_LPM_SECOND_ADS_FIRST_BLK_ID   (42)
#define MDB_LPM_SECOND_ADS_LAST_BLK_ID    (43)
#define MDB_LPM_SECOND_BB_FIRST_BLK_ID    (44)
#define MDB_LPM_SECOND_BB_LAST_BLK_ID     (57)

#define MDB_LPM_MAX_UINT32_WIDTH     16
#define MDB_LPM_RPB_MAX_UINT32_WIDTH 6
#define MDB_LPM_AD_MAX_UINT32_WIDTH  4

#define MDB_LPM_BB_NBYTES            60
#define MDB_LPM_RPB_NBYTES           21
#define MDB_LPM_ADS_NBYTES           16
#define MDB_LPM_REG_NBYTES           4

#define MDB_LPM_RPB_CAM_BIST_CONTROL_OFFSET   0x2a
#define MDB_LPM_RPB_CAM_BIST_STATUS_OFFSET    0x2b
#define MDB_LPM_RPB_GLOBAL_CONFIG_OFFSET      0x21

#define MDB_LPM_BB_GLOBAL_CONFIG_OFFSET       0x21
#define MDB_LPM_BB_MEM_CONFIG1_OFFSET         0x23

#define KAPS2_BIG_CONFIG_REG_ADDR             0xF0

#define MDB_LPM_FIRST_RPB_DEPTH               (2048)
#define MDB_LPM_FIRST_ADS_DEPTH               (1024)
#define MDB_LPM_FIRST_BBS_DEPTH               (512)

#define KAPS_JERICHO_2_SMALL_KAPS_REVISION_REG_VALUE (0x00050000)
#define KAPS_JERICHO_2_BIG_KAPS_REVISION_REG_VALUE (0x00050001)

/* Large BB modes */
#define KAPS_LARGE_BB_WIDTH_A000_B000 0x1
#define KAPS_LARGE_BB_WIDTH_A000_B480 0x2
#define KAPS_LARGE_BB_WIDTH_A000_B960 0x4
#define KAPS_LARGE_BB_WIDTH_A480_B000 0x8
#define KAPS_LARGE_BB_WIDTH_A480_B480 0x10
#define KAPS_LARGE_BB_WIDTH_A960_B000 0x20

/*Large ADS modes*/
#define MDB_LPM_ADS_DEPTH_0           (0x0)
#define MDB_LPM_ADS_DEPTH_16K         (1024 * 16)
#define MDB_LPM_ADS_DEPTH_32K         (1024 * 32)

/* The number of bytes in a single HB entry */
#define MDB_LPM_HB_BYTES 0x2

typedef enum
{
    MDB_LPM_CMD_WRITE,
    MDB_LPM_CMD_READ,
    MDB_LPM_CMD_COMPARE,
    MDB_LPM_CMD_SEARCH,

    MDB_NOF_LPM_CMD
} mdb_lpm_cmd_e;

typedef struct
{
    struct kaps_xpt mdb_lpm_xpt;
    int unit;
} MDB_LPM_XPT;

shr_error_e
mdb_lpm_big_kaps_enabled(
    int unit,
    mdb_kaps_ip_db_id_e db_id,
    int *big_kaps_enabled)
{
    int nof_clusters;
    dbal_enum_value_field_mdb_physical_table_e mdb_table_id = (db_id == MDB_KAPS_IP_PRIVATE_DB_ID) ?
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 : DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table_id, &nof_clusters));

    if (nof_clusters > 0)
    {
        *big_kaps_enabled = TRUE;
    }
    else
    {
        *big_kaps_enabled = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

kbp_status
mdb_lpm_register_write(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(((MDB_LPM_XPT *) xpt)->unit, "%s() is not supported.\n"), FUNCTION_NAME()));
    return KBP_FATAL_TRANSPORT_ERROR;
}

kbp_status
mdb_lpm_register_read(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    int rv = KBP_OK, i, unit;
    uint32 reg_val = 0;

    unit = ((MDB_LPM_XPT *) xpt)->unit;
    if ((offset == 0) && (nbytes == 4) && (bytes != NULL))
    {
        int big_kaps_enabled = FALSE;
        int big_kaps_enabled_arr[MDB_KAPS_IP_NOF_DB];
        int db_id_iter;

        for (db_id_iter = 0; db_id_iter < MDB_KAPS_IP_NOF_DB; db_id_iter++)
        {
            rv = mdb_lpm_big_kaps_enabled(unit, db_id_iter, &big_kaps_enabled_arr[db_id_iter]);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s(), mdb_lpm_big_kaps_enabled failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            if (big_kaps_enabled_arr[db_id_iter] == TRUE)
            {
                big_kaps_enabled = TRUE;
            }
        }

        if (big_kaps_enabled == TRUE)
        {
            reg_val = KAPS_JERICHO_2_BIG_KAPS_REVISION_REG_VALUE;
        }
        else
        {
            reg_val = KAPS_JERICHO_2_SMALL_KAPS_REVISION_REG_VALUE;
        }
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s() only supports reading the KapsRevision register.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }
    for (i = 0; i < nbytes; i++)
    {
        bytes[i] = (reg_val >> ((nbytes - 1 - i) * 8)) & 0xff;
    }

    return rv;
}

kbp_status
mdb_lpm_search(
    void *xpt,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result * kaps_result)
{
    int rv = KBP_OK, unit;
    uint32 func = KAPS_FUNC3, i = 0, j, k;
    uint32 mem_array[MDB_LPM_MAX_UINT32_WIDTH];
    uint32 mem_index = MDB_LPM_RPB_MAX_UINT32_WIDTH - 2;
    bsl_severity_t severity;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s():  search_interface: %d, key: 0x"), FUNCTION_NAME(), search_interface));
        for (j = 0; j < MDB_LPM_KEY_BUFFER_NOF_BYTES; j++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X"), key[j]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
    }

    sal_memset(mem_array, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));
    for (k = 0; k < MDB_LPM_KEY_BUFFER_NOF_BYTES / 4; k++)
    {
        for (j = 0; j < 4; j++)
        {
            mem_array[mem_index] |= key[i] << (3 - j) * 8;
            i++;
        }
        mem_index--;
    }
    mem_array[MDB_LPM_RPB_MAX_UINT32_WIDTH - 1] |= (func << 6);

    rv = soc_mem_array_write(unit, KAPS_RPB_TCAM_CPU_COMMANDm, 0, KAPS_BLOCK(unit, search_interface), 0, mem_array);
    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), soc_mem_array_write failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    rv = soc_mem_array_read(unit, KAPS_RPB_TCAM_CPU_COMMANDm, 0, KAPS_BLOCK(unit, search_interface), 0, mem_array);
    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), soc_mem_array_read failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    kaps_result->match_len = mem_array[0] >> 30 | ((mem_array[1] & 0x3F) << 2);
    kaps_result->ad_value[2] = (mem_array[1] >> 2);
    kaps_result->ad_value[1] = (mem_array[1] >> 10);
    kaps_result->ad_value[0] = (mem_array[1] >> 18);


    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "result_length: %d, result:0x%02x%02x%02x\n"), kaps_result->match_len,
                     kaps_result->ad_value[0], kaps_result->ad_value[1], kaps_result->ad_value[2]));
        for (j = 0; j < MDB_LPM_MAX_UINT32_WIDTH; j++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mem_array[%d] = 0x%08X\n"), j, mem_array[j]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s()\n"), FUNCTION_NAME()));
    }

    return rv;
}

STATIC
    kbp_status mdb_lpm_translate_blk_func_offset_to_mem_reg(int unit,
                                                            uint8 blk_id,
                                                            uint32 func,
                                                            uint32 offset,
                                                            soc_mem_t * mem,
                                                            soc_reg_t * reg, uint32 *array_index, int *blk)
{
    uint32 rv = KBP_OK;

    *mem = INVALIDm;
    *reg = INVALIDr;
    *array_index = 0;

    if (blk_id >= MDB_LPM_RPB_FIRST_BLK_ID && blk_id <= MDB_LPM_RPB_LAST_BLK_ID)
    {
        *blk = KAPS_BLOCK(unit, 0);
        *array_index = (blk_id - MDB_LPM_RPB_FIRST_BLK_ID);

        switch (func)
        {
            case KAPS_FUNC0:
                if (offset == MDB_LPM_RPB_CAM_BIST_CONTROL_OFFSET)
                {
                    *reg = KAPS_RPB_CAM_BIST_CONTROLr;
                }
                else if (offset == MDB_LPM_RPB_CAM_BIST_STATUS_OFFSET)
                {
                    *reg = KAPS_RPB_CAM_BIST_STATUSr;
                }
                else if (offset == MDB_LPM_RPB_GLOBAL_CONFIG_OFFSET)
                {
                    *reg = KAPS_RPB_GLOBAL_CONFIGr;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s():  unsupported RPB register offset: %d\n"),
                                               FUNCTION_NAME(), offset));
                    rv = KBP_FATAL_TRANSPORT_ERROR;
                }
                *blk = 0;
                break;

            case KAPS_FUNC1:
                *mem = KAPS_RPB_TCAM_CPU_COMMANDm;
                break;

            case KAPS_FUNC4:
                *mem = KAPS_RPB_ADSm;
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s():  RPB, unsupported func: %d\n"), FUNCTION_NAME(), func));
                rv = KBP_FATAL_TRANSPORT_ERROR;
                break;
        }
    }
    else if (blk_id >= MDB_LPM_BB_FIRST_BLK_ID && blk_id <= MDB_LPM_BB_LAST_BLK_ID)
    {
        *blk = KAPS_BLOCK(unit, 0);
        *array_index = (blk_id - MDB_LPM_BB_FIRST_BLK_ID);
        switch (func)
        {
            case KAPS_FUNC0:
                if (offset == MDB_LPM_BB_GLOBAL_CONFIG_OFFSET)
                {
                    *reg = KAPS_BB_GLOBAL_CONFIGr;
                    *blk = 0;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "%s():  unsupported BB register offset: %d\n"), FUNCTION_NAME(),
                               offset));
                    rv = KBP_FATAL_TRANSPORT_ERROR;
                }
                break;

            case KAPS_FUNC2:
            case KAPS_FUNC5:
                *mem = KAPS_BUCKET_MEMORYm;
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s():  BB, unsupported func: %d\n"), FUNCTION_NAME(), func));
                rv = KBP_FATAL_TRANSPORT_ERROR;
                break;
        }
    }
    else if (blk_id >= MDB_LPM_SECOND_ADS_FIRST_BLK_ID && blk_id <= MDB_LPM_SECOND_ADS_LAST_BLK_ID)
    {
        *array_index = (blk_id - MDB_LPM_SECOND_ADS_FIRST_BLK_ID);
        switch (func)
        {
            case KAPS_FUNC0:
                if (offset == KAPS2_BIG_CONFIG_REG_ADDR)
                {
                    *reg = INVALIDr;
                    *blk = 0;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "%s():  unsupported BB register offset: %d\n"), FUNCTION_NAME(),
                               offset));
                    rv = KBP_FATAL_TRANSPORT_ERROR;
                }
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s():  BB, unsupported func: %d\n"), FUNCTION_NAME(), func));
                rv = KBP_FATAL_TRANSPORT_ERROR;
                break;
        }
    }
    else if (blk_id >= MDB_LPM_SECOND_BB_FIRST_BLK_ID && blk_id <= MDB_LPM_SECOND_BB_LAST_BLK_ID)
    {
        *array_index = (blk_id - MDB_LPM_SECOND_BB_FIRST_BLK_ID);
        switch (func)
        {
            case KAPS_FUNC0:
                if (offset == KAPS2_BIG_CONFIG_REG_ADDR)
                {
                    *reg = INVALIDr;
                    *blk = 0;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "%s():  unsupported BB register offset: %d\n"), FUNCTION_NAME(),
                               offset));
                    rv = KBP_FATAL_TRANSPORT_ERROR;
                }
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s():  BB, unsupported func: %d\n"), FUNCTION_NAME(), func));
                rv = KBP_FATAL_TRANSPORT_ERROR;
                break;
        }
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), unrecognized blk_id = %d.\n"), FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return rv;
}

shr_error_e
mdb_lpm_is_cluster_allocated(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    uint8 *is_allocated)
{
    dbal_enum_value_field_mdb_physical_table_e assoc_mdb_physical_table_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc
                    (unit, macro_type, macro_index, cluster_index, &assoc_mdb_physical_table_id));

    if (mdb_physical_table_id == assoc_mdb_physical_table_id)
    {
        *is_allocated = TRUE;
    }
    else
    {
        *is_allocated = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_get_big_bb_fmt(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    mdb_macro_types_e macro_type,
    uint8 macro_index,
    uint32 *fmt_val)
{

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1)
    {
        if (macro_type == MDB_MACRO_A)
        {
            if (macro_index == 1)
            {
                *fmt_val = 0;
            }
            else if (macro_index == 2)
            {
                *fmt_val = 1;
            }
            else if (macro_index == 3)
            {
                *fmt_val = 2;
            }
            else if (macro_index == 4)
            {
                *fmt_val = 3;
            }
            else if (macro_index == 7)
            {
                *fmt_val = 4;
            }
            else if (macro_index == 8)
            {
                *fmt_val = 5;
            }
            else if (macro_index == 9)
            {
                *fmt_val = 6;
            }
            else if (macro_index == 11)
            {
                *fmt_val = 7;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Unrecognized combination of MDB table %d, macro type %d and macro index %d.\n",
                             mdb_physical_table, macro_type, macro_index);
            }
        }
        else if (macro_type == MDB_MACRO_B)
        {
            if (macro_index == 0)
            {
                *fmt_val = 8;
            }
            else if (macro_index == 5)
            {
                *fmt_val = 9;
            }
            else if (macro_index == 7)
            {
                *fmt_val = 10;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Unrecognized combination of MDB table %d, macro type %d and macro index %d.\n",
                             mdb_physical_table, macro_type, macro_index);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized macro type %d.\n", macro_type);
        }
    }
    else if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2)
    {
        if (macro_type == MDB_MACRO_B)
        {
            if (macro_index == 0)
            {
                *fmt_val = 0;
            }
            else if (macro_index == 1)
            {
                *fmt_val = 1;
            }
            else if (macro_index == 2)
            {
                *fmt_val = 2;
            }
            else if (macro_index == 3)
            {
                *fmt_val = 3;
            }
            else if (macro_index == 5)
            {
                *fmt_val = 4;
            }
            else if (macro_index == 7)
            {
                *fmt_val = 5;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Unrecognized combination of MDB table %d, macro type %d and macro index %d.\n",
                             mdb_physical_table, macro_type, macro_index);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized combination of MDB table %d, macro type %d.\n",
                         mdb_physical_table, macro_type);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized MDB table %d.\n", mdb_physical_table);
    }

exit:
    SHR_FUNC_EXIT;
}

kbp_status
mdb_lpm_map_bb_big_kaps_blk_id(
    int unit,
    uint8 blk_id,
    soc_mem_t * mem,
    int *blk,
    mdb_macro_types_e * macro_type,
    int *macro_index)
{
    uint32 rv = KBP_OK;

    switch (blk_id)
    {
        case MDB_LPM_SECOND_BB_FIRST_BLK_ID:
            *mem = DDHA_MACRO_0_ENTRY_BANKm;
            *blk = DDHA_BLOCK(unit, 1);

            *macro_type = MDB_MACRO_A;
            *macro_index = 1;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 1:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 2);

            *macro_type = MDB_MACRO_A;
            *macro_index = 2;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 2:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 7);

            *macro_type = MDB_MACRO_A;
            *macro_index = 3;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 3:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 3);

            *macro_type = MDB_MACRO_A;
            *macro_index = 4;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 4:
            *mem = DDHA_MACRO_1_ENTRY_BANKm;
            *blk = DDHA_BLOCK(unit, 0);

            *macro_type = MDB_MACRO_A;
            *macro_index = 7;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 5:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 4);

            *macro_type = MDB_MACRO_A;
            *macro_index = 8;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 6:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 1);

            *macro_type = MDB_MACRO_A;
            *macro_index = 9;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 7:
            *mem = DHC_MACRO_ENTRY_BANKm;
            *blk = DHC_BLOCK(unit, 5);

            *macro_type = MDB_MACRO_A;
            *macro_index = 11;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 8:
            *mem = DDHB_MACRO_0_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 0);

            *macro_type = MDB_MACRO_B;
            *macro_index = 0;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 9:
            *mem = DDHB_MACRO_1_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 0);

            *macro_type = MDB_MACRO_B;
            *macro_index = 1;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 10:
            *mem = DDHB_MACRO_1_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 2);

            *macro_type = MDB_MACRO_B;
            *macro_index = 2;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 11:
            *mem = DDHB_MACRO_1_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 1);

            *macro_type = MDB_MACRO_B;
            *macro_index = 3;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 12:
            *mem = DDHB_MACRO_0_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 2);

            *macro_type = MDB_MACRO_B;
            *macro_index = 5;
            break;

        case MDB_LPM_SECOND_BB_FIRST_BLK_ID + 13:
            *mem = DDHB_MACRO_0_ENTRY_BANKm;
            *blk = DDHB_BLOCK(unit, 3);

            *macro_type = MDB_MACRO_B;
            *macro_index = 7;
            break;

        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unsupported blk_id: %d\n"), FUNCTION_NAME(), blk_id));
            rv = KBP_FATAL_TRANSPORT_ERROR;
            break;
    }

    return rv;
}

kbp_status
mdb_lpm_map_bb_big_kaps(
    int unit,
    uint8 blk_id,
    uint32 func,
    uint32 offset,
    soc_mem_t * mem,
    uint32 *bucket_index,
    int *blk,
    int *row_in_cluster,
    soc_mem_t * mem_hitbit_a,
    soc_mem_t * mem_hitbit_b)
{
    uint32 rv = KBP_OK;
    int number_of_rows_in_cluster;

    mdb_macro_types_e macro_type;
    int macro_index;

    rv = mdb_lpm_map_bb_big_kaps_blk_id(unit, blk_id, mem, blk, &macro_type, &macro_index);
    if (rv != KBP_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s(): mdb_lpm_map_bb_big_kaps_blk_id failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if ((mem_hitbit_a != NULL) && (mem_hitbit_b != NULL))
    {
        uint32 global_macro_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, macro_type, macro_index);

        *mem_hitbit_a = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->abk_bank_a;

        *mem_hitbit_b = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->abk_bank_b;
    }

    rv = mdb_eedb_cluster_type_to_rows(unit, macro_type, &number_of_rows_in_cluster);
    if (rv != KBP_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): mdb_eedb_cluster_type_to_rows failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (rv == KBP_OK)
    {
        uint8 kaps_a_is_alloc_low, kaps_b_is_alloc_low, kaps_a_is_alloc_high, kaps_b_is_alloc_high;

        if (mdb_lpm_is_cluster_allocated(unit, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, macro_type, macro_index,
                                         (0) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                         &kaps_a_is_alloc_low) != _SHR_E_NONE
            || mdb_lpm_is_cluster_allocated(unit, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, macro_type,
                                            macro_index, (0) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                            &kaps_b_is_alloc_low) != _SHR_E_NONE
            || mdb_lpm_is_cluster_allocated(unit, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1, macro_type,
                                            macro_index, (1) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                            &kaps_a_is_alloc_high) != _SHR_E_NONE
            || mdb_lpm_is_cluster_allocated(unit, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2, macro_type,
                                            macro_index, (1) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                            &kaps_b_is_alloc_high) != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): LPM allocation query failed.\n"), FUNCTION_NAME()));
            rv = KBP_FATAL_TRANSPORT_ERROR;
        }

        if (offset < number_of_rows_in_cluster)
        {
            *row_in_cluster = offset;

            if (func == KAPS_FUNC2)
            {
                if ((kaps_a_is_alloc_low == TRUE) || (kaps_b_is_alloc_low == TRUE))
                {
                    *bucket_index = 0;
                }
                else if ((kaps_a_is_alloc_high == TRUE) || (kaps_b_is_alloc_high == TRUE))
                {
                    *bucket_index = 1;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U
                               (unit,
                                "%s(): LPM allocation query failed, unrecognized func %d, blk_id %d, when writing to big KAPS BB.\n"),
                               FUNCTION_NAME(), func, blk_id));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }
            else if (func == KAPS_FUNC10)
            {
                if (((kaps_a_is_alloc_low == TRUE) || (kaps_b_is_alloc_low == TRUE))
                    && ((kaps_a_is_alloc_high == TRUE) || (kaps_b_is_alloc_high == TRUE)))
                {
                    *bucket_index = 1;
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U
                               (unit,
                                "%s(): LPM allocation query failed, unrecognized func %d, blk_id %d, when writing to big KAPS BB.\n"),
                               FUNCTION_NAME(), func, blk_id));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit,
                            "%s(): LPM allocation query failed, unrecognized func %d when writing to big KAPS BB.\n"),
                           FUNCTION_NAME(), func));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
        else
        {
            if (((kaps_a_is_alloc_low == TRUE) || (kaps_b_is_alloc_low == TRUE))
                && ((kaps_a_is_alloc_high == TRUE) || (kaps_b_is_alloc_high == TRUE)))
            {
                *bucket_index = 1;
                *row_in_cluster = offset - number_of_rows_in_cluster;
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit,
                            "%s(): LPM allocation query failed, high offset write to blk_id %d when writing to big KAPS BB without both buckets allocated to KAPS.\n"),
                           FUNCTION_NAME(), blk_id));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            if (func != KAPS_FUNC2)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit,
                            "%s(): LPM allocation query failed, attempting to write to offset %d with func %d.\n"),
                           FUNCTION_NAME(), offset, func));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
    }

    return rv;
}

kbp_status
mdb_lpm_write_command_big_kaps(
    int unit,
    uint8 blk_id,
    uint32 cmd,
    uint32 func,
    uint32 offset,
    uint32 nbytes,
    uint8 *bytes)
{
    uint32 i = 0, k;
    int rv = KBP_OK;
    dbal_physical_entry_t data_entry;
    uint32 row_data[MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS];
    uint32 mem_index;

    uint32 *mem_dest;

    sal_memset(row_data, 0, (MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS) * sizeof(uint32));
    sal_memset(&data_entry, 0x0, sizeof(data_entry));

    if ((blk_id == MDB_LPM_SECOND_ADS_FIRST_BLK_ID) || (blk_id == MDB_LPM_SECOND_ADS_LAST_BLK_ID))
    {
         /*ADS*/ mem_index = MDB_LPM_AD_MAX_UINT32_WIDTH - 1;
        mem_dest = data_entry.payload;
    }
    else
    {
        mem_index = MDB_LPM_MAX_UINT32_WIDTH - 2;
        mem_dest = row_data;
    }

    i = 0;
    if (nbytes % 4 > 0)
    {
        for (i = 0; i < nbytes % 4; i++)
        {
            mem_dest[mem_index] |= bytes[i] << (nbytes % 4 - 1 - i) * 8;
        }
        mem_index--;
    }
    for (k = 0; k < nbytes / 4; k++)
    {
        mem_dest[mem_index] = bytes[i] << 24 | bytes[i + 1] << 16 | bytes[i + 2] << 8 | bytes[i + 3];
        i += 4;
        mem_index--;
    }

    if ((blk_id == MDB_LPM_SECOND_ADS_FIRST_BLK_ID) || (blk_id == MDB_LPM_SECOND_ADS_LAST_BLK_ID))
    {
            /*
             * Use the direct table interface to write to the MDB clusters
             */
            dbal_physical_tables_e dbal_physical_table_id;

        if (func != KAPS_FUNC16)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), unrecognized func %d when writing to big KAPS ADS.\n"), FUNCTION_NAME(),
                       func));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        dbal_physical_table_id =
            (blk_id == MDB_LPM_SECOND_ADS_FIRST_BLK_ID) ? DBAL_PHYSICAL_TABLE_KAPS_1 : DBAL_PHYSICAL_TABLE_KAPS_2;

        data_entry.key[0] = offset;
        data_entry.payload_size = MDB_NOF_CLUSTER_ROW_BITS;
        utilex_bitstream_set_bit_range(data_entry.p_mask, 0, MDB_NOF_CLUSTER_ROW_BITS);

        rv = mdb_direct_table_entry_add(unit, dbal_physical_table_id, 0 /* app_id */ , &data_entry);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_direct_table_entry_add failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }
    else if ((blk_id >= MDB_LPM_SECOND_BB_FIRST_BLK_ID) && (blk_id <= MDB_LPM_SECOND_BB_LAST_BLK_ID))
    {
        uint32 mdb_cmd = MDB_CLUSTER_WRITE_INST;
        soc_mem_t mem;
        uint32 bucket_index;
        int blk, row_in_cluster;

        if (func == KAPS_FUNC0)
        {
            return rv;
        }

        rv = mdb_lpm_map_bb_big_kaps(unit, blk_id, func, offset, &mem, &bucket_index, &blk, &row_in_cluster, NULL,
                                     NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), mdb_lpm_map_bb_big_kaps failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_0f, &mdb_cmd);
        soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_1f, &mdb_cmd);
        soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_2f, &mdb_cmd);
        soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_3f, &mdb_cmd);

        if (mdb_sbusdma_desc_is_enabled(unit) == TRUE)
        {
            rv = mdb_sbusdma_desc_add_mem(unit, mem, bucket_index, blk, row_in_cluster, row_data);
        }
        else
        {
            rv = soc_mem_array_write(unit, mem, bucket_index, blk, row_in_cluster, row_data);
        }
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), write to memory failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), unexpected blk_id %d.\n"), FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return KBP_OK;
}

kbp_status
mdb_lpm_write_command_small_kaps(
    int unit,
    uint8 blk_id,
    uint32 cmd,
    uint32 func,
    uint32 offset,
    uint32 nbytes,
    uint8 *bytes)
{
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 reg_val;
    uint32 array_index;
    uint32 mem_array[MDB_LPM_MAX_UINT32_WIDTH];
    uint32 mem_index = 0;
    uint32 i = 0, k;
    int blk;
    uint32 core;
    int rv = KBP_OK;
    bsl_severity_t severity;

    SHR_GET_SEVERITY_FOR_MODULE(severity);

    rv = mdb_lpm_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk);
    if (rv != KBP_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s(), mdb_lpm_translate_blk_func_offset_to_mem_reg failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): mem: %s, reg: %s, array_index: %d. \n"),
                                     FUNCTION_NAME(), SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit, reg), array_index));
    }

    if (mem != INVALIDm)
    {
        sal_memset(mem_array, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));

        if (mem == KAPS_RPB_TCAM_CPU_COMMANDm)
        {
            mem_index = MDB_LPM_RPB_MAX_UINT32_WIDTH - 1;
        }
        else if (mem == KAPS_RPB_ADSm)
        {
            mem_index = MDB_LPM_AD_MAX_UINT32_WIDTH - 1;
        }
        else if (mem == KAPS_BUCKET_MEMORYm)
        {
            mem_index = MDB_LPM_MAX_UINT32_WIDTH - 2;
        }

        i = 0;
        if (nbytes % 4 > 0)
        {
            for (i = 0; i < nbytes % 4; i++)
            {
                mem_array[mem_index] |= bytes[i] << (nbytes % 4 - 1 - i) * 8;
            }
            mem_index--;
        }
        for (k = 0; k < nbytes / 4; k++)
        {
            mem_array[mem_index] = bytes[i] << 24 | bytes[i + 1] << 16 | bytes[i + 2] << 8 | bytes[i + 3];
            i += 4;
            mem_index--;
        }

        if (mem == KAPS_RPB_TCAM_CPU_COMMANDm)
        {
            uint32 mem_field;

            mem_field = 0x1;
            soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_array, RPB_TCAM_CPU_COMMAND_INST_Af, &mem_field);
            soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_array, RPB_TCAM_CPU_COMMAND_INST_Bf, &mem_field);
        }

#ifdef MDB_FPP_DUMPS
        {
            int row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "********************MDB-LOGIC-START********************\n")));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "//DB(KAPS)(KAPS)\n")));

            if (mem == KAPS_RPB_TCAM_CPU_COMMANDm)
            {
                int intf, intf_idx;;
                if (blk_id == MDB_LPM_RPB_FIRST_BLK_ID)
                {
                    intf = 0;
                }
                else
                {
                    intf = 2;
                }
                for (intf_idx = 0; intf_idx < 2; intf_idx++)
                {
                    if (offset % 2 == 0)
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U
                                                     (unit,
                                                      "`KAPS_DFT(0).kaps.kaps_tcam_logical_wrapper_wrapper_4.bist_interface_%d.kaps_tcam_logical_wrapper.tcam_%s.i_tcam_brcm.camarray_key[(%d/2)%s] = {160'h"),
                                                     intf + intf_idx, offset / 2 >= 512 ? "odd" : "even", offset,
                                                     offset / 2 >= 512 ? "-512" : ""));

                        for (row_offset = MDB_LPM_RPB_MAX_UINT32_WIDTH - 2; row_offset >= 0; row_offset--)
                        {
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                        (BSL_META_U(unit, "%08x"),
                                         mem_array[row_offset + 1] << 28 || mem_array[row_offset] >> 4));
                        }

                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, ", 4'h0 /*rpb_search_lsn*/}; // key;\n")));
                    }
                    else
                    {
                        uint32 valid_bits = 0;
                        soc_mem_field_get(unit, KAPS_RPB_TCAM_CPU_CMDm, mem_array, RPB_TCAM_CPU_CMD_DINVf, &valid_bits);
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U
                                                     (unit,
                                                      "`KAPS_DFT(0).kaps.kaps_tcam_logical_wrapper_wrapper_4.bist_interface_%d.kaps_tcam_logical_wrapper.tcam_%s.i_tcam_brcm.camarray_msk[(%d/2)%s] = {160'h"),
                                                     intf + intf_idx, offset / 2 >= 512 ? "odd" : "even", offset,
                                                     offset / 2 >= 512 ? "-512" : ""));

                        for (row_offset = MDB_LPM_RPB_MAX_UINT32_WIDTH - 2; row_offset >= 0; row_offset--)
                        {
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                        (BSL_META_U(unit, "%08x"),
                                         mem_array[row_offset + 1] << 28 || mem_array[row_offset] >> 4));
                        }

                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, ", 4'h0 /*rpb_search_lsn*/}; // mask;\n")));

                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U
                                                     (unit,
                                                      "`KAPS_DFT(0).kaps.kaps_tcam_logical_wrapper_wrapper_4.bist_interface_%d.kaps_tcam_logical_wrapper.tcam_%s.i_tcam_brcm.camarray_vld[(%d/2)%s] = 2'd%d;\n"),
                                                     intf + intf_idx, offset / 2 >= 512 ? "odd" : "even", offset,
                                                     offset / 2 >= 512 ? "-512" : "", valid_bits));
                    }
                }
            }
            else if (mem == KAPS_RPB_ADSm)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U
                                             (unit,
                                              "`KAPS_REGFILE(0).rpb_ads_mem[%d].bd_write_entry(.index(%d), .data(128'h"),
                                             array_index, offset));

                for (row_offset = MDB_LPM_AD_MAX_UINT32_WIDTH - 1; row_offset >= 0; row_offset--)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), mem_array[row_offset]));
                }

                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "));\n")));
            }
            else if (mem == KAPS_BUCKET_MEMORYm)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U
                                             (unit,
                                              "`KAPS_REGFILE(0).bucket_memory_mem[%d].bd_write_entry(.index(%d), .data(480'h"),
                                             array_index, offset));

                for (row_offset = MDB_LPM_MAX_UINT32_WIDTH - 2; row_offset >= 0; row_offset--)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), mem_array[row_offset]));
                }

                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "));\n")));

            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-LOGIC-END********************\n")));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-START********************\n")));
#endif
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            if (mdb_sbusdma_desc_is_enabled(unit) == TRUE)
            {
                rv = mdb_sbusdma_desc_add_mem(unit, mem, array_index, KAPS_BLOCK(unit, core), offset, mem_array);
            }
            else
            {
                rv = soc_mem_array_write(unit, mem, array_index, KAPS_BLOCK(unit, core), offset, mem_array);
            }
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), write to memory failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
#ifdef MDB_FPP_DUMPS
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-END********************\n")));
#endif

    }
    else if (reg != INVALIDr)
    {
        /*
         * Assuming only registers up to 32 bits
         */
        reg_val = (uint32) bytes[0] << 24 | (uint32) bytes[1] << 16 | (uint32) bytes[2] << 8 | (uint32) bytes[3];

        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            rv = soc_reg32_set(unit, reg, blk + core, array_index, reg_val);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), soc_reg32_set failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }

    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), both mem and reg are invalid.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return KBP_OK;
}

kbp_status
mdb_lpm_write_command(
    void *xpt,
    uint8 blk_id,
    uint32 cmd,
    uint32 func,
    uint32 offset,
    uint32 nbytes,
    uint8 *bytes)
{
    int rv = KBP_OK, i, unit = ((MDB_LPM_XPT *) xpt)->unit;

    bsl_severity_t severity;

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() start\n nbytes: %d, bytes: 0x"), FUNCTION_NAME(), nbytes));
        for (i = 0; i < nbytes; i++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[i]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s():  blk_id: %d, cmd: %d, func: %d, offset: %d, nbytes: %d\n"),
                                     FUNCTION_NAME(), blk_id, cmd, func, offset, nbytes));
    }

    if ((blk_id >= MDB_LPM_RPB_FIRST_BLK_ID) && (blk_id <= MDB_LPM_BB_LAST_BLK_ID))
    {
        rv = mdb_lpm_write_command_small_kaps(unit, blk_id, cmd, func, offset, nbytes, bytes);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_lpm_write_command_small_kaps failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }
    else if ((blk_id >= MDB_LPM_SECOND_ADS_FIRST_BLK_ID) && (blk_id <= MDB_LPM_SECOND_BB_LAST_BLK_ID))
    {
        rv = mdb_lpm_write_command_big_kaps(unit, blk_id, cmd, func, offset, nbytes, bytes);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_lpm_write_command_big_kaps failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), unrecognized blk_id %d.\n"), FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return KBP_OK;
}

kbp_status
mdb_lpm_big_kaps_ads_depth(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    uint32 *depth)
{
    int rv = KBP_OK;
    int cluster_idx, nof_clusters, end_address = 0;
    uint32 depth_temp = 0;

    rv = mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters);
    if (rv != KBP_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s(), mdb_db_infos.db.number_of_clusters.get failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        rv = mdb_db_infos.db.clusters_info.end_address.get(unit, mdb_physical_table, cluster_idx, &end_address);
        if (rv != KBP_OK)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_db_infos.db.clusters_info.end_address.get failed.\n"),
                       FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        if (end_address > depth_temp)
        {
            depth_temp = end_address;
        }
    }

    if ((depth_temp != MDB_LPM_ADS_DEPTH_0) && (depth_temp != MDB_LPM_ADS_DEPTH_16K)
        && (depth_temp != MDB_LPM_ADS_DEPTH_32K))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s(), unexpected ADS depth %d for MDB table %d.\n"), FUNCTION_NAME(), depth_temp,
                   mdb_physical_table));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    *depth = depth_temp;

    return rv;
}

kbp_status
mdb_lpm_read_command(
    void *xpt,
    uint32 blk_id,
    uint32 cmd,
    uint32 func,
    uint32 offset,
    uint32 nbytes,
    uint8 *bytes)
{
    int rv = KBP_OK, unit = ((MDB_LPM_XPT *) xpt)->unit;
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 reg_val;
    uint32 array_index;
    uint32 mem_array[MDB_LPM_MAX_UINT32_WIDTH];
    uint32 mem_index = 0;
    uint32 i = 0, j, k;
    int blk;
    bsl_severity_t severity;

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() start\n nbytes: %d, bytes: 0x"), FUNCTION_NAME(), nbytes));
        for (j = 0; j < nbytes; j++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[j]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s():  blk_id: %d, cmd: %d, func: %d, offset: %d, nbytes: %d\n"),
                                     FUNCTION_NAME(), blk_id, cmd, func, offset, nbytes));
    }

    rv = mdb_lpm_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk);
    if (rv != KBP_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s(), mdb_lpm_translate_blk_func_offset_to_mem_reg failed.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): mem: %s, reg: %s, array_index: %d. \n"),
                                     FUNCTION_NAME(), SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit, reg), array_index));
    }

    if (mem != INVALIDm)
    {
        int core;
        sal_memset(mem_array, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));

        if (mem == KAPS_RPB_TCAM_CPU_COMMANDm)
        {
            int core = 0;
            uint32 mem_field;
            mem_field = 0x1;
            soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_array, RPB_TCAM_CPU_COMMAND_INST_Af, &mem_field);

            for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
            {
                if (mdb_sbusdma_desc_is_enabled(unit) == TRUE)
                {
                    rv = mdb_sbusdma_desc_add_mem(unit, mem, array_index, KAPS_BLOCK(unit, core), offset, mem_array);
                }
                else
                {
                    rv = soc_mem_array_write(unit, mem, array_index, KAPS_BLOCK(unit, core), offset, mem_array);
                }
                if (rv != _SHR_E_NONE)
                {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), write to memory failed.\n"), FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }
        }

        sal_memset(mem_array, 0, sizeof(mem_array));
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            uint32 mem_array_temp[MDB_LPM_MAX_UINT32_WIDTH];

            sal_memset(mem_array_temp, 0, sizeof(mem_array_temp));

            rv = soc_mem_array_read(unit, mem, array_index, KAPS_BLOCK(unit, core), offset, mem_array_temp);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), soc_mem_array_read failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            if (core == 0)
            {
                sal_memcpy(mem_array, mem_array_temp, sizeof(mem_array));
            }
        }

        if (mem == KAPS_RPB_TCAM_CPU_COMMANDm)
        {
            mem_index = MDB_LPM_RPB_MAX_UINT32_WIDTH - 1;
        }
        else if (mem == KAPS_RPB_ADSm)
        {
            mem_index = MDB_LPM_AD_MAX_UINT32_WIDTH - 1;
        }
        else if ((mem == KAPS_BUCKET_MEMORYm) || (mem == KAPS_BBS_BUCKET_MEMORYm))
        {
            mem_index = MDB_LPM_MAX_UINT32_WIDTH - 2;
        }

        i = 0;
        if (nbytes % 4 > 0)
        {
            for (i = 0; i < nbytes % 4; i++)
            {
                bytes[i] = mem_array[mem_index] >> ((nbytes % 4 - 1 - i) * 8);
            }
            mem_index--;
        }
        for (k = 0; k < nbytes / 4; k++)
        {
            for (j = 0; j < 4; j++)
            {
                bytes[i] = mem_array[mem_index] >> (3 - j) * 8;
                i++;
            }
            mem_index--;
        }

    }
    else if (reg != INVALIDr)
    {
        /*
         * Assuming only registers up to 32 bits
         */
        rv = soc_reg32_get(unit, reg, blk, array_index, &reg_val);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), soc_reg32_get failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
        bytes[0] = reg_val >> 24;
        bytes[1] = reg_val >> 16;
        bytes[2] = reg_val >> 8;
        bytes[3] = reg_val;

    }
    else if (blk_id >= MDB_LPM_SECOND_ADS_FIRST_BLK_ID && blk_id <= MDB_LPM_SECOND_ADS_LAST_BLK_ID
             && func == KAPS_FUNC0)
    {
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table = (blk_id == MDB_LPM_SECOND_ADS_FIRST_BLK_ID) ?
            DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 : DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;

        rv = mdb_lpm_big_kaps_ads_depth(unit, mdb_physical_table, &reg_val);
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_lpm_big_kaps_ads_depth failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        bytes[0] = reg_val >> 24;
        bytes[1] = reg_val >> 16;
        bytes[2] = reg_val >> 8;
        bytes[3] = reg_val;
    }
    else if (blk_id >= MDB_LPM_SECOND_BB_FIRST_BLK_ID && blk_id <= MDB_LPM_SECOND_BB_LAST_BLK_ID && func == KAPS_FUNC0)
    {
        if (!SOC_WARM_BOOT(unit)) {
            soc_mem_t mem;
            int blk;
            mdb_macro_types_e macro_type;
            int macro_index;
            dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;
            int kaps_allocation[DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 -
                                DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 + 1] = { 0 };

            rv = mdb_lpm_map_bb_big_kaps_blk_id(unit, blk_id, &mem, &blk, &macro_type, &macro_index);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s(), mdb_lpm_map_bb_big_kaps_blk_id failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            for (mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
                 mdb_physical_table_id <= DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2; mdb_physical_table_id++)
            {
                int bucket_index;
                for (bucket_index = 0;
                     bucket_index <
                     dnx_data_mdb.dh.nof_macro_clusters_get(unit) / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
                     bucket_index++)
                {
                    uint8 is_allocated;

                    rv = mdb_lpm_is_cluster_allocated(unit, mdb_physical_table_id, macro_type, macro_index,
                                                      bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                                      &is_allocated);
                    if (rv != _SHR_E_NONE)
                    {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "%s(), mdb_lpm_is_cluster_allocated failed.\n"), FUNCTION_NAME()));
                        return KBP_FATAL_TRANSPORT_ERROR;
                    }

                    kaps_allocation[mdb_physical_table_id - DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1] +=
                        is_allocated;
                }
            }

            if ((kaps_allocation[0] != 0) && (kaps_allocation[1] != 0))
            {
                if ((kaps_allocation[0] != 1) || (kaps_allocation[1] != 1))
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U
                               (unit,
                                "%s(), unexpected allocation of blk_id %d. Allocated asymmetrically to both KAPS_A and KAPS_B.\n"),
                               FUNCTION_NAME(), blk_id));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
                reg_val = KAPS_LARGE_BB_WIDTH_A480_B480;
            }
            else if ((kaps_allocation[0] == 0) && (kaps_allocation[1] == 0))
            {
                reg_val = KAPS_LARGE_BB_WIDTH_A000_B000;
            }
            else
            {
                dbal_enum_value_field_mdb_physical_table_e mdb_physical_table;
                uint32 ads_depth;
                int allocation;
                uint32 regular_alloc, wide_alloc;

                if ((kaps_allocation[0] != 0))
                {
                    mdb_physical_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
                    allocation = kaps_allocation[0];
                    regular_alloc = KAPS_LARGE_BB_WIDTH_A480_B000;
                    wide_alloc = KAPS_LARGE_BB_WIDTH_A960_B000;
                }
                else
                {
                    mdb_physical_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
                    allocation = kaps_allocation[1];
                    regular_alloc = KAPS_LARGE_BB_WIDTH_A000_B480;
                    wide_alloc = KAPS_LARGE_BB_WIDTH_A000_B960;
                }

                rv = mdb_lpm_big_kaps_ads_depth(unit, mdb_physical_table, &ads_depth);
                if (rv != _SHR_E_NONE)
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "%s(), mdb_lpm_big_kaps_ads_depth failed.\n"), FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }

                if (ads_depth == MDB_LPM_ADS_DEPTH_32K)
                {
                    if (macro_type == MDB_MACRO_A)
                    {
                        if (allocation == 1)
                        {
                            reg_val = regular_alloc;
                        }
                        else if (allocation == 2)
                        {
                            reg_val = wide_alloc;
                        }
                        else
                        {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit, "%s(), unexpected alloc %d for blk_id %d.\n"), FUNCTION_NAME(),
                                       allocation, blk_id));
                            return KBP_FATAL_TRANSPORT_ERROR;
                        }
                    }
                    else if (macro_type == MDB_MACRO_B)
                    {
                        if (allocation == 2)
                        {
                            reg_val = regular_alloc;
                        }
                        else
                        {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit, "%s(), unexpected alloc %d for blk_id %d.\n"), FUNCTION_NAME(),
                                       allocation, blk_id));
                            return KBP_FATAL_TRANSPORT_ERROR;
                        }
                    }
                    else
                    {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "%s(), unexpected macro_type %d for blk_id %d.\n"), FUNCTION_NAME(),
                                   macro_type, blk_id));
                        return KBP_FATAL_TRANSPORT_ERROR;
                    }
                }
                else if (ads_depth == MDB_LPM_ADS_DEPTH_16K)
                {
                    if (macro_type == MDB_MACRO_A)
                    {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "%s(), unexpected macro_type %d for ads_depth %d and blk_id %d.\n"),
                                   FUNCTION_NAME(), macro_type, ads_depth, blk_id));
                        return KBP_FATAL_TRANSPORT_ERROR;

                    }
                    else if (macro_type == MDB_MACRO_B)
                    {
                        if (allocation == 1)
                        {
                            reg_val = regular_alloc;
                        }
                        else if (allocation == 2)
                        {
                            reg_val = wide_alloc;
                        }
                        else
                        {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit, "%s(), unexpected alloc %d for blk_id %d.\n"), FUNCTION_NAME(),
                                       allocation, blk_id));
                            return KBP_FATAL_TRANSPORT_ERROR;
                        }
                    }
                    else
                    {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "%s(), unexpected macro_type %d for blk_id %d.\n"), FUNCTION_NAME(),
                                   macro_type, blk_id));
                        return KBP_FATAL_TRANSPORT_ERROR;
                    }
                }
                else
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "%s(), unexpected allocation %d and ads_depth %d for blk_id %d.\n"),
                               FUNCTION_NAME(), allocation, ads_depth, blk_id));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }

            rv = mdb_kaps_db.big_kaps_large_bb_mode.set(unit, blk_id - MDB_LPM_SECOND_BB_FIRST_BLK_ID, reg_val);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s(), mdb_kaps_db.big_kaps_large_bb_mode.set failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        } else {
            rv = mdb_kaps_db.big_kaps_large_bb_mode.get(unit, blk_id - MDB_LPM_SECOND_BB_FIRST_BLK_ID, &reg_val);
            if (rv != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "%s(), mdb_kaps_db.big_kaps_large_bb_mode.get failed.\n"), FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }

        bytes[0] = reg_val >> 24;
        bytes[1] = reg_val >> 16;
        bytes[2] = reg_val >> 8;
        bytes[3] = reg_val;
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(), both mem and reg are invalid.\n"), FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return KBP_OK;
}

kbp_status
mdb_lpm_command(
    void *xpt,
    enum kaps_cmd cmd,
    enum kaps_func func,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * bytes)
{
    int rv = KBP_OK;
    int unit;
    int byte_idx;
    bsl_severity_t severity;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    if ((nbytes != MDB_LPM_BB_NBYTES) && (nbytes != MDB_LPM_RPB_NBYTES) && (nbytes != MDB_LPM_ADS_NBYTES)
        && (nbytes != MDB_LPM_REG_NBYTES))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s():  Unexpected nbytes: %d\n"), FUNCTION_NAME(), nbytes));
    }

    if ((dnxc_ha_is_access_disabled(unit, DNXC_HA_ALLOW_SCHAN) == TRUE) && (cmd == KAPS_CMD_WRITE))
    {
        return KBP_OK;
    }

    switch (cmd)
    {
        case KAPS_CMD_READ:
            rv = mdb_lpm_read_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
            break;

        case KAPS_CMD_WRITE:
            rv = mdb_lpm_write_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
            break;

        case KAPS_CMD_EXTENDED:
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s():  IBC interface disabled, redundant command: %d\n"), FUNCTION_NAME(),
                       cmd));
            break;

        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s():  unsupported cmd: %d\n"), FUNCTION_NAME(), cmd));
            rv = KBP_FATAL_TRANSPORT_ERROR;
            break;
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() start: cmd: %d, func: %d, blk_nr: %d, row_nr: %d\n"),
                                     FUNCTION_NAME(), cmd, func, blk_nr, row_nr));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() nbytes: %d, bytes: 0x"), FUNCTION_NAME(), nbytes));
        for (byte_idx = 0; byte_idx < nbytes; byte_idx++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[byte_idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return rv;
}

shr_error_e
mdb_lpm_init_xpt_overrides(
    int unit)
{
    soc_reg_above_64_val_t kaps_rpb_global_config;
    mdb_kaps_ip_db_id_e db_id;
    uint8 bytes[DNX_DATA_MAX_MDB_KAPS_RPB_BYTE_WIDTH];
    uint8 blk_id;
    MDB_LPM_XPT xpt;
    soc_field_t rpb_field;
    int bb_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(bytes, 0, sizeof(bytes));

    sal_memset(kaps_rpb_global_config, 0, sizeof(kaps_rpb_global_config));

    sal_memset(&xpt, 0, sizeof(xpt));
    xpt.unit = unit;

    rpb_field = dnx_data_mdb.kaps.rpb_field_get(unit);

    for (blk_id = 0; blk_id <= MDB_LPM_RPB_LAST_BLK_ID - MDB_LPM_RPB_FIRST_BLK_ID; blk_id++)
    {
        int core;
        uint32 mem_array[MDB_LPM_MAX_UINT32_WIDTH];
        uint32 mem_field;

        sal_memset(mem_array, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));

        mem_field = 0x1;
        soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_array, RPB_TCAM_CPU_COMMAND_INST_Af, &mem_field);
        soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_array, RPB_TCAM_CPU_COMMAND_INST_Bf, &mem_field);

        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            uint32 mem_array_temp[MDB_LPM_MAX_UINT32_WIDTH];

            soc_mem_array_write(unit, KAPS_RPB_TCAM_CPU_COMMANDm, blk_id, KAPS_BLOCK(unit, core), 0, mem_array);
            soc_mem_array_read(unit, KAPS_RPB_TCAM_CPU_COMMANDm, blk_id, KAPS_BLOCK(unit, core), 0, mem_array_temp);

            soc_mem_array_write(unit, KAPS_RPB_TCAM_CPU_COMMANDm, blk_id, KAPS_BLOCK(unit, core), 1024, mem_array);
            soc_mem_array_read(unit, KAPS_RPB_TCAM_CPU_COMMANDm, blk_id, KAPS_BLOCK(unit, core), 1024, mem_array_temp);
        }
    }

    soc_reg_field_set(unit, KAPS_RPB_GLOBAL_CONFIGr, kaps_rpb_global_config, rpb_field, 0x1);

    for (db_id = MDB_KAPS_IP_PRIVATE_DB_ID; db_id < MDB_KAPS_IP_NOF_DB; db_id++)
    {
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            int hitbit_row_idx;
            int hitbit_row_idx_max = dnx_data_mdb.kaps.nof_rows_in_small_rbp_hitbits_get(unit);

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, KAPS_RPB_GLOBAL_CONFIGr, core, db_id, kaps_rpb_global_config[0]));

            for (hitbit_row_idx = 0; hitbit_row_idx < hitbit_row_idx_max; hitbit_row_idx++)
            {
                uint32 hitbit_row = 0;

                SHR_IF_ERR_EXIT(soc_mem_array_read
                                (unit, KAPS_TCAM_HIT_INDICATIONm, db_id, KAPS_BLOCK(unit, core), hitbit_row_idx,
                                 &hitbit_row));
            }
        }
    }

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        for (bb_id = 0; bb_id < MDB_LPM_BB_NOF_BLK_ID; bb_id++)
        {
            {
                int hitbit_row_idx;
                int hitbit_row_idx_max = dnx_data_mdb.kaps.nof_rows_in_small_bb_get(unit);

                for (hitbit_row_idx = 0; hitbit_row_idx < hitbit_row_idx_max; hitbit_row_idx++)
                {
                    uint32 hitbit_row = 0;

                    SHR_IF_ERR_EXIT(soc_mem_array_read
                                    (unit, KAPS_RPB_TCAM_HIT_INDICATIONm, bb_id, KAPS_BLOCK(unit, core), hitbit_row_idx,
                                     &hitbit_row));
                }
            }
        }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_xpt_parse_ads(
    int unit,
    uint32 *raw_ads)
{

    uint32 field;

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_BDATAf, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " BDATA:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_BLENGTHf, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " BLENGTH:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_ROFSf, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " ROFS:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_0f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " FM_0:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_1f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 1:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_2f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 2:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_3f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 3:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_4f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 4:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_5f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 5:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_6f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 6:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_7f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 7:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_8f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 8:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_9f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 9:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_10f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 10:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_11f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 11:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_12f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 12:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_13f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 13:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_14f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 14:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_FM_15f, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " 15:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_KSHIFTf, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " KSHIFT:0x%x"), field));

    soc_mem_field_get(unit, KAPS_RPB_ADSm, raw_ads, RPB_ADS_LBUCKETf, &field);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, " LBUCKET:0x%x"), field));

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_get_nof_small_bbs(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *nof_small_bbs)
{
    MDB_LPM_XPT xpt;
    int rv = KBP_OK;
    int small_bb_counter = 0;
    mdb_kaps_ip_db_id_e db_idx;
    int blk_id;

    SHR_FUNC_INIT_VARS(unit);

    xpt.unit = unit;

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &db_idx));

    for (blk_id = MDB_LPM_BB_FIRST_BLK_ID; blk_id <= MDB_LPM_BB_LAST_BLK_ID; blk_id++)
    {
        uint8 bytes[DNX_DATA_MAX_MDB_KAPS_BB_BYTE_WIDTH];
        uint32 bb_global_config;
        uint32 rpb_sel;

        rv = mdb_lpm_read_command(&xpt, blk_id, KAPS_CMD_READ, KAPS_FUNC0, MDB_LPM_BB_GLOBAL_CONFIG_OFFSET,
                                  sizeof(uint32), bytes);
        if (rv != KBP_OK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mdb_lpm_read_command failed with blk_id %d, offset %d.\n", blk_id,
                         MDB_LPM_BB_GLOBAL_CONFIG_OFFSET);
        }

        bb_global_config = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];

        rpb_sel = soc_reg_field_get(unit, KAPS_BB_GLOBAL_CONFIGr, bb_global_config, BB_RPB_SEL_Nf);

        if (rpb_sel == db_idx)
        {
            small_bb_counter++;
        }
    }

    *nof_small_bbs = small_bb_counter;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_xpt_dump_hw(
    int unit,
    mdb_kaps_ip_db_id_e small_kaps_db,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_ads,
    dbal_enum_value_field_mdb_physical_table_e big_kaps_bb)
{
    int rv = KBP_OK;
    uint32 blk_id;
    MDB_LPM_XPT xpt;
    uint32 offset;
    int cluster_idx;
    int nof_clusters;
    SHR_FUNC_INIT_VARS(unit);

    xpt.unit = unit;

    if (small_kaps_db == MDB_KAPS_IP_PRIVATE_DB_ID)
    {
        blk_id = MDB_LPM_RPB_FIRST_BLK_ID;
    }
    else
    {
        blk_id = MDB_LPM_RPB_LAST_BLK_ID;
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Small KAPS RPB:\n")));

    for (offset = 0; offset < MDB_LPM_FIRST_RPB_DEPTH; offset++)
    {
        uint8 bytes[DNX_DATA_MAX_MDB_KAPS_RPB_BYTE_WIDTH];
        uint32 words[MDB_LPM_MAX_UINT32_WIDTH];
        int byte_index;
#ifndef ADAPTER_SERVER_MODE
        uint32 valid_bits = 0;
#endif

        rv = mdb_lpm_read_command(&xpt, blk_id, KAPS_CMD_READ, KAPS_FUNC1, offset,
                                  dnx_data_mdb.kaps.rpb_byte_width_get(unit), bytes);
        if (rv != KBP_OK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mdb_lpm_read_command failed with blk_id %d, offset %d.\n", blk_id, offset);
        }

        SHR_IF_ERR_EXIT(mdb_lpm_uint8_to_uint32(unit, dnx_data_mdb.kaps.rpb_byte_width_get(unit), bytes, words));

#ifdef ADAPTER_SERVER_MODE
        if (utilex_bitstream_have_one_in_range(words, 0, BYTES2BITS(dnx_data_mdb.kaps.rpb_byte_width_get(unit)) - 1))
#else
        SHR_BITCOPY_RANGE(&valid_bits, 0, words, 0, dnx_data_mdb.kaps.rpb_valid_bits_get(unit));
        SHR_BITCOPY_RANGE(words, 0, words, dnx_data_mdb.kaps.rpb_valid_bits_get(unit),
                          BYTES2BITS(dnx_data_mdb.kaps.rpb_byte_width_get(unit)));

        if (valid_bits != 0)
#endif
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Row %d: 0x"), offset));
            for (byte_index = 0; byte_index < dnx_data_mdb.kaps.rpb_byte_width_get(unit); byte_index++)
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%02x"), bytes[byte_index]));
            }
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nSmall KAPS ADS:\n")));
    for (offset = 0; offset < MDB_LPM_FIRST_ADS_DEPTH; offset++)
    {
        uint8 bytes[DNX_DATA_MAX_MDB_KAPS_ADS_BYTE_WIDTH];
        uint32 words[MDB_LPM_MAX_UINT32_WIDTH];
        int byte_index;

        rv = mdb_lpm_read_command(&xpt, blk_id, KAPS_CMD_READ, KAPS_FUNC4, offset,
                                  dnx_data_mdb.kaps.ads_byte_width_get(unit), bytes);
        if (rv != KBP_OK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mdb_lpm_read_command failed with blk_id %d, offset %d.\n", blk_id, offset);
        }

        SHR_IF_ERR_EXIT(mdb_lpm_uint8_to_uint32(unit, dnx_data_mdb.kaps.ads_byte_width_get(unit), bytes, words));

        if (utilex_bitstream_have_one_in_range
            (words, 0 /* start_place */ , BYTES2BITS(dnx_data_mdb.kaps.ads_byte_width_get(unit)) - 1))
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "  Row %d: 0x"), offset));
            for (byte_index = 0; byte_index < dnx_data_mdb.kaps.ads_byte_width_get(unit); byte_index++)
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%02x"), bytes[byte_index]));
            }
            SHR_IF_ERR_EXIT(mdb_lpm_xpt_parse_ads(unit, words));
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nSmall KAPS BBS:\n")));
    for (blk_id = MDB_LPM_BB_FIRST_BLK_ID; blk_id <= MDB_LPM_BB_LAST_BLK_ID; blk_id++)
    {
        uint8 bytes[DNX_DATA_MAX_MDB_KAPS_BB_BYTE_WIDTH];
        uint32 bb_global_config;
        uint32 rpb_sel;

        rv = mdb_lpm_read_command(&xpt, blk_id, KAPS_CMD_READ, KAPS_FUNC0, MDB_LPM_BB_GLOBAL_CONFIG_OFFSET,
                                  sizeof(uint32), bytes);
        if (rv != KBP_OK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mdb_lpm_read_command failed with blk_id %d, offset %d.\n", blk_id,
                         MDB_LPM_BB_GLOBAL_CONFIG_OFFSET);
        }

        bb_global_config = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];

        rpb_sel = soc_reg_field_get(unit, KAPS_BB_GLOBAL_CONFIGr, bb_global_config, BB_RPB_SEL_Nf);

        if (rpb_sel == small_kaps_db)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "  Small KAPS BB %d:\n"), blk_id - MDB_LPM_BB_FIRST_BLK_ID));

            for (offset = 0; offset < MDB_LPM_FIRST_BBS_DEPTH; offset++)
            {
                int byte_index;
                uint32 words[MDB_LPM_MAX_UINT32_WIDTH];

                rv = mdb_lpm_read_command(&xpt, blk_id, KAPS_CMD_READ, KAPS_FUNC2, offset,
                                          dnx_data_mdb.kaps.bb_byte_width_get(unit), bytes);
                if (rv != KBP_OK)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "mdb_lpm_read_command failed with blk_id %d, offset %d.\n", blk_id,
                                 offset);
                }

                SHR_IF_ERR_EXIT(mdb_lpm_uint8_to_uint32(unit, dnx_data_mdb.kaps.bb_byte_width_get(unit), bytes, words));

                if (utilex_bitstream_have_one_in_range
                    (words, 0 /* start_place */ , BYTES2BITS(dnx_data_mdb.kaps.bb_byte_width_get(unit)) - 1))
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "    Row %d: 0x"), offset));
                    for (byte_index = 0; byte_index < dnx_data_mdb.kaps.bb_byte_width_get(unit); byte_index++)
                    {
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%02x"), bytes[byte_index]));
                    }
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                }
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nBig KAPS ADS:\n")));
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, big_kaps_ads, &nof_clusters));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        mdb_cluster_info_t cluster_info;
        dbal_physical_entry_t data_entry;
        dbal_physical_tables_e dbal_physical_table_id;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, big_kaps_ads, cluster_idx, &cluster_info));

        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "macro_type %d, macro_idx %d, cluster_idx %d:\n"),
                                  cluster_info.macro_type, cluster_info.macro_index, cluster_info.cluster_index));

        dbal_physical_table_id =
            (big_kaps_ads ==
             DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1) ? DBAL_PHYSICAL_TABLE_KAPS_1 : DBAL_PHYSICAL_TABLE_KAPS_2;

        data_entry.payload_size = MDB_NOF_CLUSTER_ROW_BITS;
        utilex_bitstream_set_bit_range(data_entry.p_mask, 0, MDB_NOF_CLUSTER_ROW_BITS);

        for (offset = cluster_info.start_address; offset < cluster_info.end_address; offset++)
        {
            data_entry.key[0] = offset;

            SHR_IF_ERR_EXIT(mdb_direct_table_entry_get(unit, dbal_physical_table_id, 0 /* app_id */ , &data_entry));
            if (utilex_bitstream_have_one_in_range
                (data_entry.payload, 0 /* start_place */ , MDB_NOF_CLUSTER_ROW_BITS - 1))
            {
                int uint32_idx;

                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "    Row %d: 0x"), offset));
                for (uint32_idx = 0; uint32_idx < BYTES2WORDS(dnx_data_mdb.kaps.ads_byte_width_get(unit)); uint32_idx++)
                {
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit, "%08x"),
                              data_entry.payload[BYTES2WORDS(dnx_data_mdb.kaps.ads_byte_width_get(unit)) - 1 -
                                                 uint32_idx]));
                }
                SHR_IF_ERR_EXIT(mdb_lpm_xpt_parse_ads(unit, data_entry.payload));
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
            }
        }
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, big_kaps_bb, &nof_clusters));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nBig KAPS BBs:\n")));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        mdb_cluster_info_t cluster_info;
        uint32 mdb_cmd = MDB_CLUSTER_READ_INST;
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, big_kaps_bb, cluster_idx, &cluster_info));
        if (cluster_info.cluster_index % dnx_data_mdb.dh.nof_bucket_clusters_get(unit) == 0)
        {
            uint32 row_data[MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS];
            soc_mem_t mem;
            int blk;
            int cluster_rows;
            uint32 bucket_index = cluster_info.cluster_index / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);

            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "macro_type %d, macro_idx %d, cluster_idx %d:\n"),
                                      cluster_info.macro_type, cluster_info.macro_index, cluster_info.cluster_index));

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, cluster_info.macro_type, &cluster_rows));

            SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk
                            (unit, cluster_info.macro_type, cluster_info.macro_index, &mem, NULL, NULL, &blk));

            for (offset = 0; offset < cluster_rows; offset++)
            {
                soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_0f, &mdb_cmd);
                soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_1f, &mdb_cmd);
                soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_2f, &mdb_cmd);
                soc_mem_field_set(unit, DDHB_MACRO_0_ENTRY_BANKm, row_data, COMMAND_3f, &mdb_cmd);

                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, bucket_index, blk, offset, row_data));

                if (utilex_bitstream_have_one_in_range
                    (row_data, 0 /* start_place */ , BYTES2BITS(dnx_data_mdb.kaps.bb_byte_width_get(unit)) - 1))
                {
                    int uint32_idx;

                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "    Row %d: 0x"), offset));
                    for (uint32_idx = 0; uint32_idx < BYTES2WORDS(dnx_data_mdb.kaps.bb_byte_width_get(unit));
                         uint32_idx++)
                    {
                        LOG_INFO(BSL_LOG_MODULE,
                                 (BSL_META_U(unit, "%08x"),
                                  row_data[BYTES2WORDS(dnx_data_mdb.kaps.bb_byte_width_get(unit)) - 1 - uint32_idx]));
                    }
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_xpt_deinit(
    int unit,
    void *xpt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(xpt);

    SHR_FUNC_EXIT;
}

#ifndef ADAPTER_SERVER_MODE
kbp_status
mdb_lpm_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    int rv = KBP_OK, unit;
    shr_error_e shr_err;
    uint32 data = 0;
    bsl_severity_t severity;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    memset(bytes, 0, MDB_LPM_HB_BYTES * sizeof(uint8_t));

    if ((blk_nr >= MDB_LPM_BB_FIRST_BLK_ID) && (blk_nr <= MDB_LPM_BB_LAST_BLK_ID))
    {
        int core;
        uint32 data_core;


        shr_err =
            mdb_db_infos.mdb_cluster_infos.small_kaps_bb_hitbit.get(unit, blk_nr - MDB_LPM_BB_FIRST_BLK_ID, row_nr,
                                                                    &data);
        if (shr_err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "%s() failed to get hitbit row from swstate, error code: %d \n"), FUNCTION_NAME(),
                     shr_err));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        /*
         * Read from both cores and OR the resulting hitbits
         */
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            READ_KAPS_RPB_TCAM_HIT_INDICATIONm(unit, blk_nr - MDB_LPM_BB_FIRST_BLK_ID, KAPS_BLOCK(unit, core), row_nr,
                                               &data_core);
            data |= data_core;
        }

        shr_err =
            mdb_db_infos.mdb_cluster_infos.small_kaps_bb_hitbit.set(unit, blk_nr - MDB_LPM_BB_FIRST_BLK_ID, row_nr,
                                                                    data);
        if (shr_err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "%s() failed to set hitbit row to swstate, error code: %d \n"), FUNCTION_NAME(),
                     shr_err));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        bytes[0] = (data >> BYTES2BITS(1)) & 0xFF;
        bytes[1] = data & 0xFF;
    }
    else if ((blk_nr >= MDB_LPM_SECOND_BB_FIRST_BLK_ID) && (blk_nr <= MDB_LPM_SECOND_BB_LAST_BLK_ID))
    {
        soc_mem_t mem, mem_hitbit_a, mem_hitbit_b;
        int blk;
        uint32 bucket_index;
        int row_in_cluster;
        uint32 blk_nr_mode;
        uint32 double_hitbit_mode;

        soc_reg_above_64_val_t hitbit_row;
        uint32 nof_buckets;
        uint32 bytes_offset;

        rv = mdb_lpm_map_bb_big_kaps(unit, blk_nr, KAPS_FUNC2, row_nr, &mem, &bucket_index, &blk,
                                     &row_in_cluster, &mem_hitbit_a, &mem_hitbit_b);
        if (rv != KBP_OK)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): mdb_lpm_map_bb_big_kaps failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        shr_err = mdb_kaps_db.big_kaps_large_bb_mode.get(unit, blk_nr - MDB_LPM_SECOND_BB_FIRST_BLK_ID, &blk_nr_mode);
        if (shr_err != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_kaps_db.big_kaps_large_bb_mode.get failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        if ((blk_nr_mode == KAPS_LARGE_BB_WIDTH_A960_B000) || (blk_nr_mode == KAPS_LARGE_BB_WIDTH_A480_B480)
            || (blk_nr_mode == KAPS_LARGE_BB_WIDTH_A000_B960))
        {
            double_hitbit_mode = 1;
            bucket_index = 0;
            nof_buckets = dnx_data_mdb.dh.nof_buckets_in_macro_get(unit);
        }
        else
        {
            double_hitbit_mode = 0;
            nof_buckets = bucket_index + 1;
        }

        for (; bucket_index < nof_buckets; bucket_index++)
        {
            sal_memset(hitbit_row, 0, sizeof(hitbit_row));
            data = 0;
            shr_err = soc_mem_array_write(unit, mem_hitbit_a, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to write hitbit A to memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            shr_err = soc_mem_array_write(unit, mem_hitbit_b, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to write hitbit B to memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }


            shr_err = soc_mem_array_read(unit, mem_hitbit_a, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to read hitbit A from memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
            data |= hitbit_row[0];

            shr_err = soc_mem_array_read(unit, mem_hitbit_b, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to read hitbit B from memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
            data |= hitbit_row[0];

            bytes_offset = double_hitbit_mode * (nof_buckets - bucket_index - 1) * dnx_data_mdb.kaps.nof_bytes_in_hitbit_row_get(unit);
            bytes[bytes_offset] = (data >> BYTES2BITS(1)) & 0xFF;
            bytes[bytes_offset + 1] = data & 0xFF;
        }
    }
    else
    {
        /*
         * The KBPSDK only utilizes the last stage hitbit
         */
        LOG_CLI((BSL_META_U(unit, "%s() unrecognized hitbit blk_nr: %d \n"), FUNCTION_NAME(), blk_nr));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 i = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s() start\n blk_nr: %d, row_nr: %d, bytes: 0x"), FUNCTION_NAME(), blk_nr,
                     row_nr));
        for (i = 0; i < MDB_LPM_HB_BYTES; i++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[i]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "data: 0x%08X\n"), data));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return rv;
}

kbp_status
mdb_lpm_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    int rv = KBP_OK, unit;
    bsl_severity_t severity;
    uint32 data = 0;
    shr_error_e shr_err;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    if ((blk_nr >= MDB_LPM_BB_FIRST_BLK_ID) && (blk_nr <= MDB_LPM_BB_LAST_BLK_ID))
    {
        data = (bytes[0] << BYTES2BITS(1)) | bytes[1];
        rv = mdb_lpm_hb_read(xpt, blk_nr, row_nr, bytes);
        if (rv != KBP_OK)
        {
            LOG_CLI((BSL_META_U(unit, "%s() failed to perform mdb_lpm_hb_read\n"), FUNCTION_NAME()));
            return rv;
        }

        shr_err =
            mdb_db_infos.mdb_cluster_infos.small_kaps_bb_hitbit.set(unit, blk_nr - MDB_LPM_BB_FIRST_BLK_ID, row_nr,
                                                                    data);
        if (shr_err != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "%s() failed to set hitbit row to swstate, error code: %d \n"), FUNCTION_NAME(),
                     shr_err));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }
    else if ((blk_nr >= MDB_LPM_SECOND_BB_FIRST_BLK_ID) && (blk_nr <= MDB_LPM_SECOND_BB_LAST_BLK_ID))
    {
        soc_mem_t mem, mem_hitbit_a, mem_hitbit_b;
        int blk;
        uint32 bucket_index;
        int row_in_cluster;
        uint32 blk_nr_mode;
        uint32 double_hitbit_mode;

        soc_reg_above_64_val_t hitbit_row;
        uint32 nof_buckets;
        uint32 bytes_offset;

        sal_memset(hitbit_row, 0, sizeof(hitbit_row));

        rv = mdb_lpm_map_bb_big_kaps(unit, blk_nr, KAPS_FUNC2, row_nr, &mem, &bucket_index, &blk,
                                     &row_in_cluster, &mem_hitbit_a, &mem_hitbit_b);
        if (rv != KBP_OK)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): mdb_lpm_map_bb_big_kaps failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        shr_err = mdb_kaps_db.big_kaps_large_bb_mode.get(unit, blk_nr - MDB_LPM_SECOND_BB_FIRST_BLK_ID, &blk_nr_mode);
        if (shr_err != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s(), mdb_kaps_db.big_kaps_large_bb_mode.get failed.\n"), FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }

        if ((blk_nr_mode == KAPS_LARGE_BB_WIDTH_A960_B000) || (blk_nr_mode == KAPS_LARGE_BB_WIDTH_A480_B480)
            || (blk_nr_mode == KAPS_LARGE_BB_WIDTH_A000_B960))
        {
            double_hitbit_mode = 1;
            bucket_index = 0;
            nof_buckets = dnx_data_mdb.dh.nof_buckets_in_macro_get(unit);
        }
        else
        {
            double_hitbit_mode = 0;
            nof_buckets = bucket_index + 1;
        }


        for (; bucket_index < nof_buckets; bucket_index++)
        {
            bytes_offset = double_hitbit_mode * (nof_buckets - bucket_index - 1) * dnx_data_mdb.kaps.nof_bytes_in_hitbit_row_get(unit);
            data = (bytes[bytes_offset] << BYTES2BITS(1)) | bytes[bytes_offset + 1];
            hitbit_row[0] = data | 0xFFFF0000;
            shr_err = soc_mem_array_write(unit, mem_hitbit_a, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to write hitbit A to memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            shr_err = soc_mem_array_write(unit, mem_hitbit_b, bucket_index, blk, row_in_cluster, hitbit_row);
            if (shr_err != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META_U(unit, "%s() failed to write hitbit B to memory, error code: %d \n"),
                         FUNCTION_NAME(), shr_err));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
    }
    else
    {
        /*
         * The KBPSDK only utilizes the last stage hitbit
         */
        LOG_CLI((BSL_META_U(unit, "%s() unrecognized hitbit blk_nr: %d \n"), FUNCTION_NAME(), blk_nr));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 i = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s() start\n blk_nr: %d, row_nr: %d, bytes: 0x"), FUNCTION_NAME(), blk_nr,
                     row_nr));
        for (i = 0; i < MDB_LPM_HB_BYTES; i++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[i]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "data: 0x%08X\n"), data));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return rv;
}
#else /* #ifndef ADAPTER_SERVER_MODE */
kbp_status
mdb_lpm_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    int rv = KBP_OK, unit;
    uint32 data = 0;
    bsl_severity_t severity;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    bytes[0] = 0;
    bytes[1] = 0;

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 i = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s() start\n blk_nr: %d, row_nr: %d, bytes: 0x"), FUNCTION_NAME(), blk_nr,
                     row_nr));
        for (i = 0; i < MDB_LPM_HB_BYTES; i++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[i]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "data: 0x%08X\n"), data));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return rv;
}

kbp_status
mdb_lpm_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    int rv = KBP_OK, unit;
    uint32 data = 0;
    bsl_severity_t severity;

    unit = ((MDB_LPM_XPT *) xpt)->unit;

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        uint32 i = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s() start\n blk_nr: %d, row_nr: %d, bytes: 0x"), FUNCTION_NAME(), blk_nr,
                     row_nr));
        for (i = 0; i < MDB_LPM_HB_BYTES; i++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%02X "), bytes[i]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "data: 0x%08X\n"), data));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s() end\n"), FUNCTION_NAME()));
    }

    return rv;
}
#endif /* #ifndef ADAPTER_SERVER_MODE */

shr_error_e
mdb_lpm_xpt_init(
    int unit,
    void **xpt)
{
    MDB_LPM_XPT *xpt_p;
    int db_id_iter;
    int big_kaps_enabled;
    int small_kaps_enabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(*xpt, sizeof(MDB_LPM_XPT), "kaps_xpt", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (*xpt == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, " Error:  SHR_ALLOC for xpt Failed\n");
    }

    xpt_p = (MDB_LPM_XPT *) * xpt;

    xpt_p->mdb_lpm_xpt.device_type = KBP_DEVICE_KAPS;

    xpt_p->mdb_lpm_xpt.kaps_search = mdb_lpm_search;
    xpt_p->mdb_lpm_xpt.kaps_register_read = mdb_lpm_register_read;
    xpt_p->mdb_lpm_xpt.kaps_command = mdb_lpm_command;
    xpt_p->mdb_lpm_xpt.kaps_register_write = mdb_lpm_register_write;

    xpt_p->mdb_lpm_xpt.kaps_hb_read = mdb_lpm_hb_read;
    xpt_p->mdb_lpm_xpt.kaps_hb_write = mdb_lpm_hb_write;

    xpt_p->unit = unit;

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(mdb_lpm_init_xpt_overrides(unit));
    }


    for (db_id_iter = 0; db_id_iter < MDB_KAPS_IP_NOF_DB; db_id_iter++)
    {
        SHR_IF_ERR_EXIT(mdb_lpm_big_kaps_enabled(unit, db_id_iter, &big_kaps_enabled));

        if (big_kaps_enabled == FALSE)
        {
            small_kaps_enabled = TRUE;
        }
    }

    if (small_kaps_enabled == TRUE)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.small_kaps_bb_hitbit.alloc(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_lpm_entry_search(
    int unit,
    int core,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    uint32 mem_cmd[MDB_LPM_MAX_UINT32_WIDTH], mem_reply[MDB_LPM_MAX_UINT32_WIDTH];
    bsl_severity_t severity;
    mdb_kaps_ip_db_id_e mdb_db_idx;
    uint32 mem_field;
    int prefix_length;
    int big_kaps_enabled;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_lpm_dbal_to_db(unit, dbal_physical_table_id, &mdb_db_idx));

    sal_memset(mem_cmd, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));
    sal_memset(mem_reply, 0, MDB_LPM_MAX_UINT32_WIDTH * sizeof(uint32));

    SHR_BITCOPY_RANGE(mem_cmd, dnx_data_mdb.kaps.key_lsn_bits_get(unit), entry->key, 0,
                      dnx_data_mdb.kaps.key_width_in_bits_get(unit));

    mem_field = 0x1;
    SHR_IF_ERR_EXIT(mdb_lpm_big_kaps_enabled(unit, mdb_db_idx, &big_kaps_enabled));
    if (big_kaps_enabled == TRUE)
    {
        soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_CMDm, mem_cmd, RPB_TCAM_CPU_CMD_EKSf, &mem_field);
    }
    mem_field = 0x1;
    soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_CMDm, mem_cmd, RPB_TCAM_CPU_CMD_KSf, &mem_field);
    mem_field = 0x1;
    soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_CMDm, mem_cmd, RPB_TCAM_CPU_CMD_CEf, &mem_field);
    mem_field = 0x1;
    soc_mem_field_set(unit, KAPS_RPB_TCAM_CPU_COMMANDm, mem_cmd, RPB_TCAM_CPU_COMMAND_INST_Af, &mem_field);

    SHR_IF_ERR_EXIT(WRITE_KAPS_RPB_TCAM_CPU_COMMANDm(unit, mdb_db_idx, KAPS_BLOCK(unit, core), 0, mem_cmd));
    SHR_IF_ERR_EXIT(READ_KAPS_RPB_TCAM_CPU_COMMANDm(unit, mdb_db_idx, KAPS_BLOCK(unit, core), 0, mem_reply));

    if (big_kaps_enabled == FALSE)
    {

        entry->payload[0] = (mem_reply[1]) & ((1 << dnx_data_mdb.kaps.ad_width_in_bits_get(unit)) - 1);
        prefix_length = (int) mem_reply[0] >> 24;
        entry->prefix_length = prefix_length;
    }
    else
    {
        soc_reg_t result_core0_db0_reg = MDB_REG_2002r,
            result_core0_db1_reg = MDB_REG_2016r,
            result_core1_db0_reg = MDB_REG_2005r, result_core1_db1_reg = MDB_REG_2019r, result_reg;

        soc_field_t payload_field = ITEM_0_19f, prefix_length_field = ITEM_20_27f, result_found_field =
            ITEM_28_28f, result_error_field = ITEM_29_29f;

        uint32 reg_val;
        uint32 found, error;

        sal_memset(&reg_val, 0x0, sizeof(reg_val));

        if (core == 0)
        {
            if (mdb_db_idx == MDB_KAPS_IP_PRIVATE_DB_ID)
            {
                result_reg = result_core0_db0_reg;
            }
            else
            {
                result_reg = result_core0_db1_reg;
            }
        }
        else
        {
            if (mdb_db_idx == MDB_KAPS_IP_PRIVATE_DB_ID)
            {
                result_reg = result_core1_db0_reg;
            }
            else
            {
                result_reg = result_core1_db1_reg;
            }
        }

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, result_reg, MDB_BLOCK(unit), 0, &reg_val));

        found = soc_reg_field_get(unit, result_reg, reg_val, result_found_field);
        error = soc_reg_field_get(unit, result_reg, reg_val, result_error_field);

        if (error != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " Search interface has returned error indication. found: %d, prefix length: %d, payload: 0x%x\n",
                         found, soc_reg_field_get(unit, result_reg, reg_val, prefix_length_field),
                         soc_reg_field_get(unit, result_reg, reg_val, payload_field));
        }

        if (found != 0)
        {
            entry->payload[0] = soc_reg_field_get(unit, result_reg, reg_val, payload_field);
            entry->prefix_length = soc_reg_field_get(unit, result_reg, reg_val, prefix_length_field);
        }
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int print_index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_search: start\n")));
        for (print_index = MDB_LPM_RPB_MAX_UINT32_WIDTH - 1; print_index >= 0; print_index--)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mem_cmd[%d]: %08X.\n"), print_index, mem_cmd[print_index]));
        }
        for (print_index = MDB_LPM_RPB_MAX_UINT32_WIDTH - 1; print_index >= 0; print_index--)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "mem_reply[%d]: %08X.\n"), print_index, mem_reply[print_index]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_lpm_entry_search: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
