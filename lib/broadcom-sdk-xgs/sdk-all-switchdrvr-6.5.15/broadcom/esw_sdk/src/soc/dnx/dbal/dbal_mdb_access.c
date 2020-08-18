/*
 * $Id: dbal_mdb_access.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <shared/utilex/utilex_bitstream.h>

/**
 * \brief
 * Acquire the APP_ID from an MDB DBAL table, depending on whether the table is a TCAM or not.
 * \param [in] _unit - unit
 * \param [in] _entry_handle - entry handle
 * \param [in] _app_id - app_db_id
 * \return
 *      _app_id = APP_ID of the table
 */
#define DBAL_MDB_ACCESS_APP_ID_GET(_unit, _entry_handle, _app_id)                                                 \
    if ((_entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM) || (_entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT))      \
    {                                                                                                             \
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, _entry_handle->table_id, &_app_id));     \
    }                                                                                                             \
    else                                                                                                          \
    {                                                                                                             \
        _app_id = _entry_handle->table->app_id;                                                                   \
    }

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

static dbal_physical_table_actions_t mdb_empty_actions = {
    /** .entry_set */
    NULL,
    /** .entry_get */
    NULL,
    /** .entry_delete */
    NULL,
    /** .table_clear */
    NULL,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    NULL,
    /** .iterator_get_next */
    NULL,
    /** .iterator_deinit */
    NULL
};

static dbal_physical_table_actions_t mdb_em_actions = {
    /** .entry_set */
    mdb_em_entry_add,
    /** .entry_get */
    mdb_em_entry_get,
    /** .entry_delete */
    mdb_em_entry_delete,
    /** .table_clear */
    mdb_em_table_clear,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    mdb_em_iterator_init,
    /** .iterator_get_next */
    mdb_em_iterator_get_next,
    /** .iterator_deinit */
    mdb_em_iterator_deinit
};

static dbal_physical_table_actions_t mdb_direct_actions = {
    /** .entry_set */
    mdb_direct_table_entry_add,
    /** .entry_get */
    mdb_direct_table_entry_get,
    /** .entry_delete */
    mdb_direct_table_entry_delete,
    /** .table_clear */
    mdb_direct_table_clear,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    mdb_direct_table_iterator_init,
    /** .iterator_get_next */
    mdb_direct_table_iterator_get_next,
    /** .iterator_deinit */
    mdb_direct_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_eedb_actions = {
    /** .entry_set */
    mdb_eedb_table_entry_add,
    /** .entry_get */
    mdb_eedb_table_entry_get,
    /** .entry_delete */
    mdb_eedb_table_entry_delete,
    /** .table_clear */
    mdb_eedb_table_clear,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    mdb_eedb_table_iterator_init,
    /** .iterator_get_next */
    mdb_eedb_table_iterator_get_next,
    /** .iterator_deinit */
    mdb_eedb_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_tcam_actions = {
    /** .entry_set */
    dnx_field_tcam_access_entry_add,
    /** .entry_get */
    dnx_field_tcam_access_entry_get,
    /** .entry_delete */
    dnx_field_tcam_access_entry_delete,
    /** .table_clear */
    NULL,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    dnx_field_tcam_access_iterator_init,
    /** .iterator_get_next */
    dnx_field_tcam_access_iterator_get_next,
    /** .iterator_deinit */
    dnx_field_tcam_access_iterator_deinit
};

static dbal_physical_table_actions_t mdb_lpm_actions = {
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    /** .entry_set */
    mdb_lpm_entry_add,
    /** .entry_get */
    mdb_lpm_entry_get,
    /** .entry_delete */
    mdb_lpm_entry_delete,
    /** .table_clear */
    mdb_lpm_table_clear,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    mdb_lpm_iterator_init,
    /** .iterator_get_next */
    mdb_lpm_iterator_get_next,
    /** .iterator_deinit */
    mdb_lpm_iterator_deinit
#else
    /** .entry_set */
    NULL,
    /** .entry_get */
    NULL,
    /** .entry_delete */
    NULL,
    /** .table_clear */
    NULL,
    /** .table_default_values_set */
    NULL,
    /** .table_init */
    NULL,
    /** .table_deinit */
    NULL,
    /** .iterator_init */
    NULL,
    /** .iterator_get_next */
    NULL,
    /** .iterator_deinit */
    NULL
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
};

static dbal_physical_mngr_info_t physical_table_mngr = {
    {
     {
            /** .physical_name */
      "MDB TABLE NONE",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_NONE,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_empty_actions},
     {
            /** .physical_name */
      "MDB TABLE TCAM",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_TCAM,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_tcam_actions},
     {
            /** .physical_name */
      "MDB TABLE LPM PRIVATE",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_KAPS_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_lpm_actions},
     {
            /** .physical_name */
      "MDB TABLE LPM PUBLIC",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_KAPS_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_lpm_actions},
     {
            /** .physical_name */
      "MDB TABLE ISEM 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_ISEM_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE INLIF 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_INLIF_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE IVSI",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_IVSI,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE ISEM 2",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_ISEM_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE ISEM 3",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_ISEM_3,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE INLIF 2",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_INLIF_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE INLIF 3",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_INLIF_3,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE LEM",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_LEM,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE IOEM 0",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_IOEM_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE IOEM 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_IOEM_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE MAP",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_MAP,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE FEC 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_FEC_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE FEC 2",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_FEC_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE FEC 3",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_FEC_3,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE MC ID",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_PPMC,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE GLEM 0",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_GLEM_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE GLEM 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_GLEM_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 2",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 3",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_3,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 4",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_4,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 5",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_5,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 6",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_6,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 7",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_7,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EEDB 8",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EEDB_8,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_eedb_actions},
     {
            /** .physical_name */
      "MDB TABLE EOEM 0",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EOEM_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EOEM 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EOEM_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_DPC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE ESEM",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_ESEM,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EVSI",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_EVSI,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_direct_actions},
     {
            /** .physical_name */
      "MDB TABLE EXEM 1",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_SEXEM_1,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EXEM 2",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_SEXEM_2,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EXEM 3",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_SEXEM_3,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE EXEM 4",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_LEXEM,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE RMEP",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_RMEP_EM,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_em_actions},
     {
            /** .physical_name */
      "MDB TABLE KBP",
            /** .physical_db_type */
      DBAL_PHYSICAL_TABLE_KBP,
            /** .physical_core_mode */
      DBAL_CORE_MODE_SBC,
            /** .nof_entries */
      0,
            /** .table_actions */
      &mdb_empty_actions}
     }
};

static shr_error_e
dbal_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_entry_t * entry,
    uint32 app_id,
    uint8 print_only_key,
    char *action)
{
    int ii, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry->key_size);
    int payload_size_in_words = BITS2WORDS(entry->payload_size);
    dbal_physical_table_def_t *PhysicalTable;
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strcasecmp(action, "get from")) || (!sal_strcasecmp(action, "get next from"))
                || (!sal_strcasecmp(action, "Access ID get")))
            {
                logger_action = 1;
            }
            else if (!sal_strcasecmp(action, "delete from"))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);

            /** Physical table */
            SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_table_id, &PhysicalTable));
            LOG_CLI((BSL_META("Entry %s Physical Table: %s\n"), action, PhysicalTable->physical_name));

            LOG_CLI((BSL_META("Acccess ID %d\n"), entry->entry_hw_id));
            /** Print key and key_mask */
            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("Key(%3d bits): 0x"), entry->key_size));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->key[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            LOG_CLI((BSL_META("Key Mask     : 0x")));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->k_mask[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            if (entry->prefix_length != 0)
            {
                /*
                 * Relevant for LPM tables only
                 */
                LOG_CLI((BSL_META("Prefix length: %u\n"), entry->prefix_length));
            }

            if (!print_only_key)
            {
                /** Print payload and payload */
                LOG_CLI((BSL_META("Payload(%3d bits): 0x"), entry->payload_size));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->payload[ii]));
                }
                LOG_CLI((BSL_META("\n")));
                LOG_CLI((BSL_META("Payload Mask     : 0x")));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->p_mask[ii]));
                }
                LOG_CLI((BSL_META("\n")));
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_init(
    int unit)
{
    int table_idx;
    dbal_physical_table_def_t *PhysicalTable;
    dbal_logical_table_t *table;
    /** This array is used for efficiency reason to update above 1k dbal logical tabled without accessing the SW-state at each update */
    int table_capacity[DBAL_NOF_PHYSICAL_TABLES];
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(mdb_pre_init_step(unit));
    }

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table_idx, &PhysicalTable));

        if (physical_table_mngr.physical_tables[table_idx].physical_db_type != table_idx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_init %d\n", table_idx);
        }

        if ((table_idx == DBAL_PHYSICAL_TABLE_TCAM) || (table_idx == DBAL_PHYSICAL_TABLE_KBP))
        {
            table_capacity[table_idx] = 0;
            if (!SOC_WARM_BOOT(unit))
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, 0));
            }
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, table_idx, &table_capacity[table_idx]));

        if (!SOC_WARM_BOOT(unit))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, table_capacity[table_idx]));
        }

        if (physical_table_mngr.physical_tables[table_idx].table_actions->table_init)
        {
            SHR_IF_ERR_EXIT(physical_table_mngr.physical_tables[table_idx].table_actions->table_init(unit));
        }
    }

    for (table_idx = 0; table_idx < DBAL_NOF_TABLES; table_idx++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_idx, &table));

        if ((table->access_method != DBAL_ACCESS_METHOD_MDB) ||
            (table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT))
        {
            continue;
        }
        table->max_capacity = table_capacity[table->physical_db_id[0]];
    }

    if (!SOC_WARM_BOOT(unit))
    {
        /**init the flush machine (this is the flush of the LEM that is a part of the MDB*/
        SHR_IF_ERR_EXIT(dbal_flush_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_deinit(
    int unit)
{
    int table_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        if (physical_table_mngr.physical_tables[table_idx].table_actions->table_deinit)
        {
            SHR_IF_ERR_EXIT(physical_table_mngr.physical_tables[table_idx].table_actions->table_deinit(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_physical_table_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_def_t ** physical_table)
{
    SHR_FUNC_INIT_VARS(unit);

    if (physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_get %d\n", physical_table_id);
    }
    *physical_table = &(physical_table_mngr.physical_tables[physical_table_id]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size)
{
    int payload_size;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API can not called with table which is not MDB table. table %s.\n",
                     table->table_name);
    }

    if (res_type_idx < table->nof_result_types)
    {
        payload_size = table->multi_res_info[res_type_idx].entry_payload_size;
        if (table->result_type_mapped_to_sw)
        {
            payload_size -= table->multi_res_info[res_type_idx].results_info[0].field_nof_bits;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                     res_type_idx, table->table_name, table->nof_result_types);
    }

    *p_size = payload_size;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_eedb_link_list_indication_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list)
{
    uint8 found_next_ptr = FALSE;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (res_type_idx < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type cannot have negative value at that point (%d)\n", res_type_idx);
    }

    if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && dbal_physical_table_is_eedb(table->physical_db_id[0]))
    {
        if (res_type_idx < table->nof_result_types)
        {
            int num_of_fields = table->multi_res_info[res_type_idx].nof_result_fields;
            dbal_field_types_defs_e field_type;

            SHR_IF_ERR_EXIT(dbal_fields_field_type_get
                            (unit, table->multi_res_info[res_type_idx].results_info[num_of_fields - 1].field_id,
                             &field_type));
            if ((field_type == DBAL_FIELD_TYPE_DEF_EEDB_LL_NEXT_POINTER))
            {
                found_next_ptr = TRUE;
            }
            else
            {
                found_next_ptr = FALSE;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                         res_type_idx, table->table_name, table->nof_result_types);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API can not called with table which is not EEDB table. table %s.\n",
                     table->table_name);
    }
    *has_link_list = found_next_ptr;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_sizes_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    int *key_size,
    int *max_pld_size)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, phy_db_id, app_db_id, &dbal_table_id));

    if (dbal_table_id != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (table->result_type_mapped_to_sw)
        {
            *max_pld_size = table->max_payload_size - table->multi_res_info[0].results_info[0].field_nof_bits;
        }
        else
        {
            *max_pld_size = table->max_payload_size;
        }
        *key_size = (table->key_size - table->core_id_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id)
{
    int ii, jj, num_of_tables;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    num_of_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    if (phy_db_id < DBAL_NOF_PHYSICAL_TABLES)
    {
        if (app_db_id < DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE)
        {
            for (ii = 0; ii < num_of_tables; ii++)
            {
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
                if (table->access_method != DBAL_ACCESS_METHOD_MDB)
                {
                    continue;
                }
                if ((sal_strstr(table->table_name, "EXAMPLE") != NULL))
                {
                    continue;
                }
                for (jj = 0; jj < table->nof_physical_tables; jj++)
                {
                    if (table->physical_db_id[jj] == phy_db_id)
                    {
                        if (phy_db_id == DBAL_PHYSICAL_TABLE_TCAM)
                        {
                            if ((table->app_id == app_db_id) ||
                                ((table->app_id + 1 == app_db_id) && (table->key_size > 160)))
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                        else
                        {
                            uint32 app_id_1 = 0;
                            uint32 app_id_2 = 0;

                            app_id_1 = table->app_id & ((1 << table->app_id_size) - 1);
                            app_id_2 = app_db_id & ((1 << table->app_id_size) - 1);

                            if (app_id_1 == app_id_2)
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal AppDbId %d, Max Value is %d\n", app_db_id,
                         DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE - 1);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal phyDB %d, DBAL_NOF_PHYSICAL_TABLES=%d\n", phy_db_id,
                     DBAL_NOF_PHYSICAL_TABLES);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_eedb_payload_data_size_etps_format_get(
    int unit,
    uint32 etps_format,
    int *data_payload_size)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
        if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            continue;
        }
        if (!dbal_physical_table_is_eedb(table->physical_db_id[0]))
        {
            continue;
        }
        if (etps_format >= 0x30)
        {
            /** All etps which are larger than 48, are saved for data entries. all of size 120 bits */
            *data_payload_size = 120;
            SHR_EXIT();
        }
        for (jj = 0; jj < table->nof_result_types; jj++)
        {
            if ((table->multi_res_info[jj].result_type_hw_value[0] == etps_format) &&
                (!table->result_type_mapped_to_sw) && (!table->multi_res_info[jj].is_disabled))
            {
                uint8 has_ll;
                int payload_size;
                SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get(unit, ii, jj, &has_ll));
                if (has_ll)
                {
                    payload_size = table->multi_res_info[jj].entry_payload_size - MDB_DIRECT_BASIC_ENTRY_SIZE;
                }
                else
                {
                    payload_size = table->multi_res_info[jj].entry_payload_size;
                }
                *data_payload_size = payload_size;
                SHR_EXIT();
            }
        }
    }
    *data_payload_size = -1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_id_and_result_type_from_etps_format_get(
    int unit,
    uint32 etps_format,
    dbal_tables_e * table_id,
    int *result_type)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
        if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            continue;
        }
        if ((!dbal_physical_table_is_eedb(table->physical_db_id[0]))
            && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_ESEM))
        {
            continue;
        }
        for (jj = 0; jj < table->nof_result_types; jj++)
        {
            if ((table->multi_res_info[jj].result_type_hw_value[0] == etps_format) &&
                (!table->result_type_mapped_to_sw))
            {
                *table_id = ii;
                *result_type = jj;
                SHR_EXIT();
            }
        }
    }
    *table_id = DBAL_NOF_TABLES;
    *result_type = 0;

exit:
    SHR_FUNC_EXIT;
}

uint8
dbal_physical_table_is_fec(
    dbal_physical_tables_e physical_table_id)
{
    if ((physical_table_id >= DBAL_PHYSICAL_TABLE_FEC_1) && (physical_table_id <= DBAL_PHYSICAL_TABLE_FEC_3))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_physical_table_is_in_lif(
    dbal_physical_tables_e physical_table_id)
{
    if ((physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1) ||
        ((physical_table_id >= DBAL_PHYSICAL_TABLE_INLIF_2) && (physical_table_id <= DBAL_PHYSICAL_TABLE_INLIF_3)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_physical_table_is_eedb(
    dbal_physical_tables_e physical_table_id)
{
    if ((physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1) && (physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_logical_table_is_out_lif_allocator_eedb(
    CONST dbal_logical_table_t * table)
{
    if ((table->allocator_field_id == DBAL_FIELD_OUT_LIF) && dbal_physical_table_is_eedb(table->physical_db_id[0]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_logical_table_is_out_rif_allocator(
    CONST dbal_logical_table_t * table)
{
    if ((table->allocator_field_id == DBAL_FIELD_OUT_RIF) && dbal_physical_table_is_eedb(table->physical_db_id[0]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
static shr_error_e
dbal_mdb_mact_dynamic_entry_check(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 strength_value;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    if ((entry_handle->table_id == DBAL_TABLE_FWD_MACT) || (entry_handle->table_id == DBAL_TABLE_FWD_MACT_IVL))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, DBAL_FIELD_MAC_STRENGTH, 0, entry_handle->cur_res_type, 0,
                         &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, DBAL_FIELD_MAC_STRENGTH, entry_handle->phy_entry.payload,
                         &strength_value));
        if (strength_value <= DBAL_ENUM_FVAL_MAC_STRENGTH_1)
        {
            /** Entry is dynamic, set flag to update HW only */
            if (entry_handle->phy_entry.mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)
            {
                entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_NONE;
            }
            else if (entry_handle->phy_entry.mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
            {
                entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief
 * this function resolve the result type of an entry, before it gets the full entry.
 * If no error occur, the cur_res_type on entry handle will be update.
 * If cannot find match result type, _SHR_E_NOT_FOUND is returned
 */
shr_error_e
dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw)
{
    int jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_internal_disable_set(unit);
    if (resolve_from_sw != table->result_type_mapped_to_sw)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB RT resolution, mismatch with result from SW flag. table = %s\n",
                     table->table_name);
    }

    while (curr_res_type < table->nof_result_types)
    {
        if (resolve_from_sw)
        {
            SHR_SET_CURRENT_ERR(dbal_sw_res_type_resolution(unit, entry_handle));
            SHR_EXIT();
        }
        else
        {
            /**
             * skip disabled result types   */
            if (entry_handle->table->multi_res_info[curr_res_type].is_disabled)
            {
                curr_res_type++;
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                       DBAL_FIELD_RESULT_TYPE, 0, curr_res_type, 0, &table_field_info));

            SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, DBAL_FIELD_RESULT_TYPE,
                                                       entry_handle->phy_entry.payload, field_value));

            for (jj = 0; jj < entry_handle->table->multi_res_info[curr_res_type].result_type_nof_hw_values; jj++)
            {
                if (entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[jj] == field_value[0])
                {
                    entry_handle->cur_res_type = curr_res_type;
                    SHR_EXIT();
                }
            }
            curr_res_type++;
        }
    }
    if (curr_res_type == table->nof_result_types)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_def_t *PhysicalTable;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (table->result_type_mapped_to_sw)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        sal_memset(&iterator_info->mdb_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

        if (PhysicalTable->table_actions->iterator_init == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
        }
        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(PhysicalTable->table_actions->iterator_init(unit, physical_db_id, app_id,
                                                                    &iterator_info->mdb_iterator));

        if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            dbal_table_field_info_t table_field_info = { 0 };

            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, entry_handle->table_id, table->allocator_field_id, 1, entry_handle->cur_res_type, 0,
                             &table_field_info));
            /**keeping the max entry size */
            iterator_info->max_num_of_iterations = table_field_info.max_value;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief fill the flush shadow with all the information of the rules and actions
 */
static shr_error_e
dbal_iterator_rules_shadow_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 actions_bitmap)
{
    int ii, rule_counter = 0;
    int nof_result_types, nof_flush_rules;
    uint8 has_rt_rule = FALSE;
    int result_type_rule_value = 0;
    dbal_flush_shadow_info_t *flush_shadow = NULL;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    nof_result_types = entry_handle->table->nof_result_types;
    nof_flush_rules = iterator_info->mdb_iterator.nof_flush_rules;

    /** Allocate the rules shadow buffers for flash machine */
    SHR_ALLOC_SET_ZERO(flush_shadow, sizeof(dbal_flush_shadow_info_t) * nof_flush_rules,
                       "Flush shadow allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY, EMPTY);

    /** Check if there is a rule on result type */
    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (!has_rt_rule)
            {
                has_rt_rule = TRUE;
                result_type_rule_value = iterator_info->val_rules_info[ii].value[0];
                if (iterator_info->mdb_iterator.nof_flush_rules != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found result type rule, but more than a single flush rule.\n");
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found two result type rules.\n");
            }
        }
    }

    /**
     * loop over all flush machine rules, and create per line its info:
     *     - key rules buffer + mask
     *     - value rules buffer + mask
     *     - hit indication rules
     *     - actions
     *     - action bitmap
     */
    for (ii = 0; ii < nof_result_types; ii++)
    {
        int jj;
        uint8 field_not_exists = FALSE;
        uint32 build_rules_entry_handle_id = 0;
        dbal_access_condition_info_t *key_rules, *value_rules;
        dbal_iterator_actions_info_t *actions;
        dbal_entry_handle_t *build_rules_entry_handle;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &build_rules_entry_handle_id));

        key_rules = iterator_info->key_rules_info;
        value_rules = iterator_info->val_rules_info;
        actions = iterator_info->actions_info;

        /** set key rules */
        for (jj = 0; jj < iterator_info->nof_key_rules; jj++)
        {
            dbal_entry_key_field_set(unit, build_rules_entry_handle_id, key_rules[jj].field_id, key_rules[jj].value,
                                     key_rules[jj].mask, DBAL_POINTER_TYPE_ARR_UINT32);
        }

        /**
         * set value rules:
         * if result_type rule exists, set it first
         */
        if (!has_rt_rule)
        {
            uint32 rt_value[1] = { 0 };
            rt_value[0] = ii;
            if (!utilex_bitstream_test_bit(&iterator_info->mdb_iterator.result_types_in_flush_bitmap, ii))
            {
                continue;
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
        }
        else if (ii != result_type_rule_value)
        {
            continue;
        }

        for (jj = 0; jj < iterator_info->nof_val_rules; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            /**
             * Check if the field exists in the current result type. set the rule only in case it exists.
             */
            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, value_rules[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                field_not_exists = TRUE;
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, value_rules[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, value_rules[jj].value, value_rules[jj].mask);
        }

        if (field_not_exists)
        {
            continue;
        }

        flush_shadow[rule_counter].key_rule_size = entry_handle->table->key_size;
        flush_shadow[rule_counter].value_rule_size = entry_handle->table->multi_res_info[ii].entry_payload_size;
        /**
         * Copy result and key rule from the temporary handle to the flash machine shadow
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, build_rules_entry_handle_id, &build_rules_entry_handle));
        sal_memcpy(flush_shadow[rule_counter].key_rule_buf, build_rules_entry_handle->phy_entry.key,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
        sal_memcpy(flush_shadow[rule_counter].key_rule_mask_buf, build_rules_entry_handle->phy_entry.k_mask,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload,
                                                       entry_handle->table->multi_res_info[ii].zero_padding,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask,
                                                       entry_handle->table->multi_res_info[ii].zero_padding,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_mask_buf));

        /**
         * Create action buffers
         * Similar to value rules, check first that the field exists in the current
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, entry_handle->table_id, build_rules_entry_handle_id));
        for (jj = 0; jj < iterator_info->nof_actions; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            if (actions[jj].field_id == DBAL_FIELD_RESULT_TYPE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal action for field RESULT_TYPE\n");
            }
            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, actions[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            if (jj == 0)
            {
                uint32 rt_value[1] = { 0 };
                rt_value[0] = ii;
                dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                           0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, actions[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, actions[jj].value, actions[jj].mask);
        }

        /**
         * Copy result action from the temporary handle to the flash machine shadow
         */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload,
                                                       entry_handle->table->multi_res_info[ii].zero_padding,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask,
                                                       entry_handle->table->multi_res_info[ii].zero_padding,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_mask_buf));

        /** set hit bit rule */
        if (iterator_info->hit_bit_rule_valid)
        {
            flush_shadow[rule_counter].hit_indication_rule_mask = TRUE;
            flush_shadow[rule_counter].hit_indication_rule = iterator_info->hit_bit_rule_is_hit;
        }

        /** Set action bitmap */
        flush_shadow[rule_counter].action_types_btmp = actions_bitmap;
        flush_shadow[rule_counter].value_action_size = entry_handle->table->max_payload_size;
        rule_counter++;
    }
    SHR_IF_ERR_EXIT(dbal_flush_rules_set(unit, entry_handle->table->app_id, rule_counter, flush_shadow));

exit:
    SHR_FREE(flush_shadow);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 *  \brief check if a given set of rules and action will mapped to flush machine or DBAL (SW actions)
 */
static shr_error_e
dbal_mdb_iterator_is_flush_machine(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii, result_type_value_in_rule = 0;
    uint8 has_rt_rule = FALSE, use_flush = TRUE;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    mdb_iterator_info = &iterator_info->mdb_iterator;

    mdb_iterator_info->result_types_in_flush_bitmap = 0;
    mdb_iterator_info->nof_flush_rules = 0;

    
    if ((entry_handle->table_id != DBAL_TABLE_FWD_MACT) && (entry_handle->table_id != DBAL_TABLE_FWD_MACT_IVL))
    {
        use_flush = FALSE;
    }

    /** flush support rules of type EQUAL_TO only*/
    for (ii = 0; (ii < iterator_info->nof_key_rules) && use_flush; ii++)
    {
        if (iterator_info->key_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            use_flush = FALSE;
        }
    }

    /** flush support rules of type EQUAL_TO only*/
    for (ii = 0; (ii < iterator_info->nof_val_rules) && use_flush; ii++)
    {
        if (iterator_info->val_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            use_flush = FALSE;
        }
    }

    /** FLUSH can not retrieve hit bit */
    if (iterator_info->hit_bit_action_get)
    {
        use_flush = FALSE;
    }

    for (ii = 0; ii < entry_handle->table->nof_result_types; ii++)
    {
        if(use_flush)
        {
            mdb_em_entry_encoding_e entry_encoding;

            SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding(unit,
                                                      entry_handle->table->physical_db_id[0],
                                                      entry_handle->table->key_size,
                                                      entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                      entry_handle->table->app_id,
                                                      entry_handle->table->app_id_size,
                                                      &entry_encoding));

            if(entry_encoding != MDB_EM_ENTRY_ENCODING_ONE)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_flush_bitmap, ii));
                mdb_iterator_info->nof_flush_rules++;
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
                mdb_iterator_info->nof_non_flush_rules++;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
            mdb_iterator_info->nof_non_flush_rules++;
        }
    }

    /**
     * Check if there is a Result type rule:
     * If there is, verify it doesn't have two RESULT TYPE rules
     */
    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (has_rt_rule == FALSE)
            {
                /** indicate that there is a result type rule */
                has_rt_rule = TRUE;

                /** result type rule value */
                result_type_value_in_rule = iterator_info->val_rules_info[ii].value[0];

                if (use_flush
                    && utilex_bitstream_test_bit(&mdb_iterator_info->result_types_in_flush_bitmap,
                                                 result_type_value_in_rule))
                {
                    mdb_iterator_info->nof_flush_rules = 1;
                    mdb_iterator_info->result_types_in_flush_bitmap = (1 << ii);
                    mdb_iterator_info->nof_non_flush_rules = 0;
                    mdb_iterator_info->result_types_in_non_flush_bitmap = 0;
                }
                else
                {
                    mdb_iterator_info->nof_flush_rules = 0;
                    mdb_iterator_info->result_types_in_flush_bitmap = 0;
                    mdb_iterator_info->nof_non_flush_rules = 1;
                    mdb_iterator_info->result_types_in_non_flush_bitmap = (1 << ii);
                }
            }
            else
            {
                /** We cannot support two result type rules */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Two Result type rules with different values cannot be set. val1=%d, val2=%d",
                             result_type_value_in_rule, iterator_info->val_rules_info[ii].value[0]);
            }
        }
    }

    if (mdb_iterator_info->nof_flush_rules == 0)
    {
        use_flush = FALSE;
    }

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    mdb_iterator_info->iterate_in_flush_machine = use_flush;
#else
    mdb_iterator_info->iterate_in_flush_machine = FALSE;
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_get_next(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    dbal_iterator_info_t * iterator_info)
{
    uint8 entry_found = FALSE;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    dbal_physical_table_def_t *PhysicalTable;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    while (!entry_found && !iterator_info->is_end)
    {
        /** case that the result type is mapped to SW using SW iterator to find the next entry */
        if (table->result_type_mapped_to_sw)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_next(unit, entry_handle));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
                entry_found = TRUE;
                if (table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    iterator_info->physical_db_index = entry_handle->core_id;
                }
            }
        }
        else if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            /** case that the there is a allocator field, using resource manager to find the next entry */
            int key_bigger_then_capacity = 0;
            entry_found = TRUE;
            if (iterator_info->used_first_key)
            {
                entry_handle->phy_entry.key[0]++;
                if (entry_handle->phy_entry.key[0] > iterator_info->max_num_of_iterations)
                {
                    key_bigger_then_capacity = 1;
                    entry_found = FALSE;
                }
            }
            iterator_info->used_first_key = 1;

            if (!key_bigger_then_capacity)
            {
                if ((dbal_logical_table_is_out_lif_allocator_eedb(table)) ||
                    (dbal_logical_table_is_out_rif_allocator(table)) ||
                    (dbal_physical_table_is_in_lif(table->physical_db_id[0])))
                {
                    entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
                }
                SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
            }

            if ((is_valid_entry == DBAL_KEY_IS_VALID) && (entry_found))
            {
                int rv = dbal_mdb_table_entry_get(unit, entry_handle);
                if (!rv)
                {
                    if (sal_memcmp
                        (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0)
                    {
                        /**if the entry is default treat it as not found */
                        entry_found = FALSE;
                    }
                    else
                    {
                        entry_found = TRUE;
                    }
                }
                else
                {
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        /**ID allocated but entry was not added */
                        entry_found = FALSE;
                    }
                    else
                    {
                        /**any other error code need to return... */
                        SHR_ERR_EXIT(rv, "internal error in entry get");
                    }
                }
            }
            else
            {
                iterator_info->is_end = 1;
            }
        }
        else
        {
            /** using HW iterator */
            if (PhysicalTable->table_actions->iterator_get_next == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
            }
            if (!table->has_result_type)
            {
                entry_handle->cur_res_type = 0;
            }

            SHR_IF_ERR_EXIT(PhysicalTable->
                            table_actions->iterator_get_next(unit, physical_db_id, app_id, &iterator_info->mdb_iterator,
                                                             &entry_handle->phy_entry, &iterator_info->is_end));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                if (table->has_result_type)
                {
                    int rv;
                    /** Result type is resolved now, validate it related to table */
                    rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
                    if (rv == _SHR_E_NONE)
                    {
                        entry_found = TRUE;
                    }
                    else if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
                else
                {
                    entry_found = TRUE;
                }
            }
        }
    }

    if (table->table_type == DBAL_TABLE_TYPE_TCAM)
    {
        entry_handle->access_id_set = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_table_entry_get_next_inner(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint32 is_action_get;
    uint32 perform_action;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    mdb_iterator_info = &iterator_info->mdb_iterator;
    phy_entry = &entry_handle->phy_entry;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    is_action_get = iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET;
    perform_action = iterator_info->actions_bitmap & ~DBAL_ITER_ACTION_GET;

    if (!iterator_info->used_first_key)
    {
        iterator_info->mdb_iterator.hit_bit_flags = entry_handle->phy_entry.hitbit;
        SHR_IF_ERR_EXIT(dbal_mdb_iterator_is_flush_machine(unit, entry_handle));
        if (mdb_iterator_info->iterate_in_flush_machine)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_rules_shadow_set(unit, entry_handle, iterator_info->actions_bitmap));
            SHR_IF_ERR_EXIT(dbal_flush_start(unit, FALSE, (uint8) !is_action_get));

            if (!is_action_get)
            {
                /** Call flush end to clean up the flush machine after the flush is done */
                SHR_IF_ERR_EXIT(dbal_flush_end(unit));
            }
        }
    }

    if (is_action_get && mdb_iterator_info->iterate_in_flush_machine && !mdb_iterator_info->start_non_flush_iteration)
    {
        SHR_IF_ERR_EXIT(dbal_flush_entry_get(unit, entry_handle));
        if (!iterator_info->is_end)
        {
            entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
            /** check if it is a MACT dynamic entry, in case it is, do not perform action in SW shadow */
            SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle));
#endif
        }
        else
        {
            /** Call flush end to clean up the flush machine */
            SHR_IF_ERR_EXIT(dbal_flush_end(unit));
        }
    }

    /*
     * If there is no get action:
     * it doesn't matter if flush ran or not.
     * it finished its work, and we need to continue to sw iterator
     */
    if ((iterator_info->is_end && mdb_iterator_info->iterate_in_flush_machine) ||
        !is_action_get || !mdb_iterator_info->iterate_in_flush_machine || mdb_iterator_info->start_non_flush_iteration)
    {
        uint32 result_types_to_find = 0xFFFFFFFF;

        if (!mdb_iterator_info->start_non_flush_iteration)
        {
            iterator_info->is_end = FALSE;
        }

        mdb_iterator_info->start_non_flush_iteration = TRUE;
        /** In case there were no flush rules or there is already a result type rule - No need to add non flush rules for result type*/
        if (mdb_iterator_info->iterate_in_flush_machine)
        {
            result_types_to_find = mdb_iterator_info->result_types_in_non_flush_bitmap;
        }

        SHR_IF_ERR_EXIT(dbal_mdb_access_get_next(unit, app_id, table->physical_db_id[iterator_info->physical_db_index],
                                                 entry_handle, phy_entry, iterator_info));
        if (!iterator_info->is_end)
        {
            dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
            if (table->core_mode == DBAL_CORE_MODE_DPC)
            {
                entry_handle->core_id = iterator_info->physical_db_index;
            }
            SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
            if (is_valid_entry == DBAL_KEY_IS_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB iterator found invalid entry\n");
            }
            else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
            {
                iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                SHR_EXIT();
            }
            else if (!utilex_bitstream_test_bit(&result_types_to_find, entry_handle->cur_res_type))
            {
                if (!is_action_get && perform_action)
                {
                    entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
                    /** check if it is a MACT dynamic entry, in case it is, do not perform action in SW shadow */
                    SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle));
#endif
                }
                else
                {
                    iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                }
                SHR_EXIT();
            }
            iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
            SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                      table->physical_db_id[iterator_info->physical_db_index],
                                                      &entry_handle->phy_entry, app_id, FALSE, "get next from"));
        }
        else
        {
            if ((table->nof_physical_tables > 0) && (!table->result_type_mapped_to_sw) &&
                (iterator_info->physical_db_index < (table->nof_physical_tables - 1)))
            {
                SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_deinit(unit, entry_handle));
                iterator_info->physical_db_index++;
                iterator_info->is_end = FALSE;
                SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_init(unit, entry_handle));
                SHR_IF_ERR_EXIT(dbal_mdb_access_get_next(unit, app_id,
                                                         table->physical_db_id[iterator_info->physical_db_index],
                                                         entry_handle, phy_entry, iterator_info));
            }
            if (!iterator_info->is_end)
            {
                dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
                if (table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    entry_handle->core_id = iterator_info->physical_db_index;
                }
                SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
                if (is_valid_entry == DBAL_KEY_IS_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB iterator found invalid entry\n");
                }
                else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
                {
                    iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                    SHR_EXIT();
                }
                else if (!utilex_bitstream_test_bit(&result_types_to_find, entry_handle->cur_res_type))
                {
                    if (!is_action_get && perform_action)
                    {
                        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
                        /** check if it is a MACT dynamic entry, in case it is, do not perform action in SW shadow */
                        SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle));
#endif
                    }
                    else
                    {
                        iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                    }
                    SHR_EXIT();
                }
                iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
                entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
                SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                          table->physical_db_id[iterator_info->physical_db_index],
                                                          &entry_handle->phy_entry, app_id, FALSE, "get next from"));
            }
            else
            {
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while (iterator_info->mdb_iterator.continue_calling_mdb_get_next)
    {
        iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_mdb_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_def_t *PhysicalTable;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (entry_handle->table->result_type_mapped_to_sw)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

        if (PhysicalTable->table_actions->iterator_deinit == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
        }

        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(PhysicalTable->table_actions->iterator_deinit(unit, physical_db_id, app_id,
                                                                      &iterator_info->mdb_iterator));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_get(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry)
{
    int rv;
    dbal_physical_table_def_t *PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->entry_get == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (entry_handle->table->result_type_mapped_to_sw)
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", entry_handle->table->table_name);
        }
        /** Don't need to get the result type, already updated in result type resolution*/
        phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
    }

    /*
     * In case action_apply flag is not SW_SHADOW,
     * check the global flag for skipping shadow and having the result be read from HW
     */
    {
        dbal_mdb_action_apply_type_e orig_access_apply_type = phy_entry->mdb_action_apply;

        if (phy_entry->mdb_action_apply != DBAL_MDB_ACTION_APPLY_SW_SHADOW)
        {
            uint8 skip_shasow = 0;
            SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.get(unit, &skip_shasow));
            if (skip_shasow)
            {
                phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            }
        }

        rv = PhysicalTable->table_actions->entry_get(unit, physical_db_id, app_id, &entry_handle->phy_entry);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        phy_entry->mdb_action_apply = orig_access_apply_type;
    }

    if (entry_handle->table->has_result_type)
    {
        if (entry_handle->table->result_type_mapped_to_sw)
        {
            dbal_table_field_info_t *table_field_info;
             /**incase that the entry has result type in SW need to update the result type value in the entry buffer this must be
              * after entry_get because entry get is overriding the buffer */
            table_field_info = &(entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[0]);

            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (&entry_handle->table->multi_res_info[entry_handle->cur_res_type].result_type_hw_value[0],
                             table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                             entry_handle->phy_entry.payload));
        }
        else
        {
            if (dbal_physical_table_is_eedb(physical_db_id))
            {
                if (entry_handle->table->max_payload_size != phy_entry->payload_size)
                {
                    int offset;
                    uint32 payload_copy[5] = { 0 };
                    offset = entry_handle->table->max_payload_size - phy_entry->payload_size;
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                    (phy_entry->payload, 0, phy_entry->payload_size, payload_copy));
                    sal_memset(phy_entry->payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
                    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                    (payload_copy, offset, phy_entry->payload_size, phy_entry->payload));
                }
            }

            /** Result type is resolved now, validate it related to table */
            rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int core_id;
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;

    SHR_FUNC_INIT_VARS(unit);
    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;

    phy_entry->payload_size = entry_handle->table->max_payload_size;
    phy_entry->key_size = entry_handle->table->key_size;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    if (dbal_logical_table_is_out_rif_allocator(table) || dbal_logical_table_is_out_lif_allocator_eedb(table))
    {
        SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, &phy_entry->eedb_ll));
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "handle result type not initialized table %s\n",
                         entry_handle->table->table_name);
        }
        phy_entry->payload_size = DBAL_RES_INFO.entry_payload_size;
    }

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            int rv;
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }

            rv = dbal_mdb_access_get(unit, app_id, table->physical_db_id[core_id], entry_handle, phy_entry);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            break;
        }
    }

    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, table->physical_db_id[core_id], &entry_handle->phy_entry, app_id, FALSE,
                     "get from"));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_delete(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry)
{
    dbal_physical_table_def_t *PhysicalTable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->entry_delete == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (entry_handle->table->result_type_mapped_to_sw)
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
        }
        phy_entry->payload_size -= entry_handle->table->multi_res_info[0].results_info[0].field_nof_bits;
    }
    SHR_IF_ERR_EXIT(PhysicalTable->table_actions->entry_delete(unit, physical_db_id, app_id, phy_entry));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint32 payload_size_to_delete;
    SHR_FUNC_INIT_VARS(unit);
    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;

    if (dbal_logical_table_is_out_rif_allocator(table) || dbal_logical_table_is_out_lif_allocator_eedb(table))
    {
        SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, &phy_entry->eedb_ll));
        payload_size_to_delete = DBAL_RES_INFO.entry_payload_size;
    }
    else if (dbal_physical_table_is_in_lif(table->physical_db_id[0]))
    {
        payload_size_to_delete = DBAL_RES_INFO.entry_payload_size;
    }
    else
    {
        payload_size_to_delete = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
        /*
         * code for MC_ID only.
         * As the MC_ID entry's VMV is managed by PMF, and not by MDB access, the VMV (which is RESULT_TYPE for MC_ID) need
         * to be set on buffer
         */
        if (table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_PPMC)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
            if (table->multi_res_info[entry_handle->cur_res_type].zero_padding > 0)
            {
                uint32 result_type_hw_value[1] = { 0 };
                result_type_hw_value[0] = table->multi_res_info[entry_handle->cur_res_type].result_type_hw_value[0];
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(result_type_hw_value,
                                                               table->multi_res_info[entry_handle->
                                                                                     cur_res_type].results_info[0].
                                                               bits_offset_in_buffer -
                                                               table->multi_res_info[entry_handle->
                                                                                     cur_res_type].zero_padding,
                                                               table->multi_res_info[entry_handle->
                                                                                     cur_res_type].results_info[0].
                                                               field_nof_bits, phy_entry->payload));
                /*
                 * Move the payload from max_payload_size alignment to the appropriate vmv alignment
                 */
                payload_size_to_delete = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
                SHR_BITCOPY_RANGE(phy_entry->payload, 0, phy_entry->payload,
                                  entry_handle->table->max_payload_size - payload_size_to_delete,
                                  payload_size_to_delete);
            }
        }
    }

    phy_entry->key_size = table->key_size;
    phy_entry->payload_size = payload_size_to_delete;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    {
        int core_id;
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                      table->physical_db_id[core_id], phy_entry, app_id, TRUE,
                                                      "delete from"));
            SHR_IF_ERR_EXIT(dbal_mdb_access_delete(unit, app_id, table->physical_db_id[core_id], entry_handle,
                                                   phy_entry));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_write(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry)
{
    dbal_physical_table_def_t *PhysicalTable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->entry_add == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (entry_handle->table->result_type_mapped_to_sw)
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_add(unit, entry_handle));
        }
    }
    SHR_IF_ERR_EXIT(PhysicalTable->table_actions->entry_add(unit, physical_db_id, app_id, phy_entry));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    dbal_physical_entry_t phy_entry_normalized;
    uint32 app_id;
    SHR_FUNC_INIT_VARS(unit);
    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
    phy_entry = &entry_handle->phy_entry;
    phy_entry->is_update = entry_handle->is_entry_update;
    phy_entry->payload_size = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
    phy_entry->payload_offset = table->multi_res_info[entry_handle->cur_res_type].zero_padding;
    phy_entry->key_size = table->key_size;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    /** check if it is a MACT dynamic entry, in case it is, do not perform action in SW shadow */
    SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle));
#endif

    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        phy_entry->payload_size = table->max_payload_size;
    }
    else
    {
        /** payload_offset represents the shift in the payload and payload mask from lsb */
        if (phy_entry->payload_offset != 0)
        {
            uint32 payload_copy[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = {
                0
            };
            phy_entry_normalized = *phy_entry;
            sal_memset(&phy_entry_normalized.payload[0], 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
            sal_memset(&phy_entry_normalized.p_mask[0], 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (&phy_entry->payload[0], phy_entry->payload_offset, phy_entry->payload_size, payload_copy));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (payload_copy, 0, phy_entry->payload_size, phy_entry_normalized.payload));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (&phy_entry->p_mask[0], phy_entry->payload_offset, phy_entry->payload_size, payload_copy));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (payload_copy, 0, phy_entry->payload_size, phy_entry_normalized.p_mask));
            phy_entry = &phy_entry_normalized;
        }
    }
    if (entry_handle->table->result_type_mapped_to_sw)
    {
        phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
    }

    if (dbal_physical_table_is_eedb(table->physical_db_id[0]))
    {
        SHR_IF_ERR_EXIT(dbal_tables_eedb_link_list_indication_get
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, &phy_entry->eedb_ll));
    }

    {
        int core_id;
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                      table->physical_db_id[core_id], phy_entry, app_id, FALSE,
                                                      "add to"));
            SHR_IF_ERR_EXIT(dbal_mdb_access_write(unit, app_id, table->physical_db_id[core_id], entry_handle,
                                                  phy_entry));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_table_clear(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_def_t *PhysicalTable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->table_clear == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (entry_handle->table->result_type_mapped_to_sw)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_clear(unit, entry_handle));
    }
    SHR_IF_ERR_EXIT(PhysicalTable->table_actions->table_clear(unit, physical_db_id, app_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_access_table_clear_by_iter(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 iter_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &iter_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, iter_handle_id, DBAL_ITER_ACTION_DELETE));
    SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, iter_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table = entry_handle->table;
    uint32 app_id;
    SHR_FUNC_INIT_VARS(unit);

    /**there is an iterator field for the table use it */
    if ((table->table_type != DBAL_TABLE_TYPE_LPM) && (table->access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear_by_iter(unit, entry_handle));
    }
    else
    {
        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
        {
            int core_id;
            DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
            {
                if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear
                                (unit, app_id, table->physical_db_id[core_id], entry_handle));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 tcam_handler_id;
    uint32 access_id;
    int rv;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);
    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM &&
        entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for TCAM\n");
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, entry_handle->table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, entry_handle->table->physical_db_id[0], &entry_handle->phy_entry,
                     tcam_handler_id, TRUE, "Access ID get"));
    rv = dnx_field_tcam_access_key_to_access_id
        (unit, core, tcam_handler_id, entry_handle->phy_entry.key, entry_handle->phy_entry.k_mask, &access_id);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    entry_handle->phy_entry.entry_hw_id = (int) access_id;
exit:
    SHR_FUNC_EXIT;
}
