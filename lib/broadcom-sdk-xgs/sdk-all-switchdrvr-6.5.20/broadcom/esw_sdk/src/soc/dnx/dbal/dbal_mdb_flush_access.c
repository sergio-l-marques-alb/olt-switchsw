/** \file dbal_mdb_flush_access.c
 * $Id$
 *
 * Access functions used by the dbal to interact with the DMA.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>
#include <soc/drv.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>
#include <soc/fifodma.h>
#include <soc/dnx/dnx_fifodma.h>
#include <sal/types.h>
#include <soc/dnx/swstate/auto_generated/access/flush_access.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/mdb.h>
#include <src/soc/dnx/flush/flush_internal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

/** Get bit in the command bitmap */
#define FLUSH_DB_COMMAND_GET_BIT 4
#define FLUSH_DB_ALL_SOURCES 0x7
#define FLUSH_DB_PIPE_SOURCE 0x1
#define MACT_KEY_SIZE_IN_BITS 80
#define MACT_PAYLOAD_SIZE_IN_BITS 64
#define PAYLOAD_MASK_SIZE 3 /** Size in words */
#define FLUSH_MACT_ENTRY_SIZE 144
#define MACT_CPU_REQUEST_COMMAND_OFFSET_BIT 156
#define MACT_CPU_REQUEST_COMMAND_NOF_BITS 3
#define MACT_CPU_REQUEST_COMMAND_FLUSH 6

/**
 * \brief - Returns if the flash machine is running
 *
 * \param [in] unit - unit id
 * \param [in] flush_in_work - 1 if the flush is in work. 0 otherwise.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_flush_in_work(
    int unit,
    int *flush_in_work)
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_t reg = MDB_REG_212r;
    uint32 flush_status;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_A0(unit))
    {
        reg = MDB_REG_212r;
    }
    else if (SOC_IS_JERICHO2_B(unit))
    {
        reg = MDB_REG_221r;
    }
    else if (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit))
    {
        reg = MDB_REG_215r;
    }
    else if (SOC_IS_J2P(unit))
    {
        reg = MDB_REG_217r;
    }

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));

    if (SOC_IS_J2P(unit))
    {
      /** ITEM_37_38 is the flush status field */
        flush_status = soc_reg_above_64_field32_get(unit, reg, reg_above_64_val, ITEM_37_38f);

    }
    else
    {
      /** ITEM_36_37 is the flush status field */
        flush_status = soc_reg_above_64_field32_get(unit, reg, reg_above_64_val, ITEM_36_37f);
    }

    if (flush_status == FLUSH_MACHINE_STATUS_IDLE)
    {
        *flush_in_work = 0;
    }
    else
    {
        *flush_in_work = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_clean_reply_fifo(
    int unit)
{
    int entries_count = 0;
    uint8 *retrievd_entries = NULL;
    uint32 entry_size;
    int flush_channel;

    SHR_FUNC_INIT_VARS(unit);

    /** Get DMA channel ID */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &flush_channel));
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_resume(unit, flush_channel));

    entries_count = 1;

    while (entries_count)
    {
        /** Read an entry from the DMA's host memory */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_num_entries_get
                        (unit, flush_channel, &entries_count, &retrievd_entries, &entry_size));

        if (entries_count)
        {
            /** Release the memory of the used DMA entry */
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_set_entries_read(unit, flush_channel, entries_count));
        }
    }

    /** Pause the DMA */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_pause(unit, flush_channel));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear flush DB rules and actions
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_flush_db_clear(
    int unit)
{
    int rule_index = 0;
    int action_index = 0;
    uint32 rule_filter[DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE];
    uint32 rule_data[DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE];
    uint32 table_index = 0;
    soc_mem_t flush_data_mem, flush_tcam_mem;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(rule_filter, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE));
    sal_memset(rule_data, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE));

    flush_tcam_mem = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->table_name;
    flush_data_mem = dnx_data_l2.age_and_flush_machine.data_rules_get(unit)->table_name;

    /** Run over the rules and actions and clear them one by one */
    for (action_index = 0; action_index < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; action_index++)
    {
        /*
         * MDB_LEM_FLUSH_DATAm bacame MDB_MEM_24000m
         */
        SHR_IF_ERR_EXIT(soc_mem_write(unit, flush_data_mem, MEM_BLOCK_ALL, action_index, (void *) rule_data));

        /*
         * MDB_LEM_FLUSH_TCAMm bacame MDB_MEM_23000m
         */
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, flush_tcam_mem, table_index, MEM_BLOCK_ALL, rule_index, (void *) rule_filter));

        rule_index++;

        if (rule_index == DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE)
        {
            rule_index -= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE;
            table_index = 1;
        }
    }

    flush_db_context.nof_valid_rules.set(unit, 0);

    /**
     *  Clear reply FIFO from ACKs that might fill the FIFO.
     *  When having a rule for delete and continue learning the reply FIFO might be filled with rejection ACKs due to the rule.
     *  Clean these ACKs.
     **/
    SHR_IF_ERR_EXIT(dnx_clean_reply_fifo(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init the flush DB context
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_flush_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize the default flush mode to be normal (not bulk) */
    flush_db_context.init(unit);
    flush_db_context.flush_in_bulk.set(unit, 0);
   /** SHR_IF_ERR_EXIT(dbal_flush_db_clear(unit)); */

    /** Reset the flush machine trigger for the next run */
    /*
     * SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_REG_3021r, REG_PORT_ANY, 0, 0));
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_flush_vmv_rule_set(
    int unit,
    mdb_em_entry_encoding_e entry_encoding,
    uint8 vmv_size,
    soc_reg_above_64_val_t mdb_entry,
    soc_reg_above_64_val_t mdb_entry_mask)
{
    int entry_encoding_index = 0;
    soc_field_info_t *field_info;
    uint32 lem_entry_size;

    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve the MDB_LEM entry size */
    SOC_FIND_FIELD(ENTRYf, SOC_MEM_INFO(unit, MDB_LEMm).fields, SOC_MEM_INFO(unit, MDB_LEMm).nFields, field_info);
    lem_entry_size = field_info->len;

    for (entry_encoding_index = entry_encoding; entry_encoding_index >= MDB_EM_ENTRY_ENCODING_ONE;
         entry_encoding_index--)
    {
        int vmv_aligned_offset;
        int entry_size =
            dnx_data_mdb.pdbs.pdb_info_get(unit, DBAL_PHYSICAL_TABLE_LEM)->row_width / (1 << entry_encoding_index);

        vmv_aligned_offset =
            lem_entry_size - dnx_data_mdb.pdbs.pdb_info_get(unit, DBAL_PHYSICAL_TABLE_LEM)->row_width + entry_size;

        if (entry_encoding_index == entry_encoding)
        {
            /** for the current entry encoding, need to set VMV mask bits to all CARE (1's), according to vmv_size */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (mdb_entry_mask, vmv_aligned_offset - vmv_size, vmv_aligned_offset - 1));
        }
        else
        {
            /*
             * for all bigger entry types, need to set:
             * 1. VMV mask bits to all CARE (1's) - according to max vmv sizem, 6bits
             * 2. VMV values to 0 - according to max vmv sizem, 6bits
             */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (mdb_entry_mask, vmv_aligned_offset - dnx_data_mdb.em.max_nof_vmv_size_get(unit),
                             vmv_aligned_offset - 1));
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (mdb_entry, vmv_aligned_offset - dnx_data_mdb.em.max_nof_vmv_size_get(unit),
                             vmv_aligned_offset - 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see h file
 */
shr_error_e
dbal_flush_rules_set(
    int unit,
    int app_db_id,
    int nof_rules,
    dbal_flush_shadow_info_t * rules_info)
{
    /** Number of valid rules in the flush DB */
    uint32 nof_valid_rules;
    uint32 rule_index;
    uint32 table_index = 0;
    uint32 field_val;
    uint32 next_available_rule;
    uint32 rule_filter[DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE];
    uint32 rule_data[DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE];
    soc_reg_above_64_val_t mdb_entry;
    soc_reg_above_64_val_t mdb_entry_mask;
    uint8 flush_in_bulk;
    uint32 tmp_align_buf[3];
    uint8 first_get_command = 0;
    int flush_channel;
    uint8 app_db_id_size = 0;
    soc_mem_t flush_data_mem, flush_tcam_mem;
    int flush_in_work = 0;
    soc_field_info_t *field_info;
    uint32 lem_entry_size;
    bsl_severity_t severity;
    soc_field_t valid_field;
    soc_field_t entry_mask_field;
    soc_field_t entry_filter_field;
    soc_field_t src_mask_field;
    soc_field_t src_filter_field;
    soc_field_t appdb_id_mask_field;
    soc_field_t appdb_id_filter_field;
    soc_field_t accessed_mask_field;
    soc_field_t accessed_filter_field;
    soc_field_t command_field;
    soc_field_t payload_mask_field;
    soc_field_t payload_field;
    int flush_wait_count = 800;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_flush_in_work(unit, &flush_in_work));

    /** Waiting 800 milisecond to make sure the flush is not in work */
    while (flush_in_work && flush_wait_count)
    {
        /** Sleep 1 milisecond */
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(dbal_flush_in_work(unit, &flush_in_work));
        flush_wait_count--;
    }

    /** Make sure the flush is not in work */
    if (flush_in_work)
    {
        SHR_ERR_EXIT(_SHR_E_BUSY, "Flush rules can not be set before the previous run is done.\n");
    }

    if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal dma operations not supported in Error Recovery transactions!\n");
    }

    valid_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->valid;
    entry_mask_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->entry_mask;
    entry_filter_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->entry_filter;
    src_mask_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->src_mask;
    src_filter_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->src_filter;
    appdb_id_mask_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->appdb_id_mask;
    appdb_id_filter_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->appdb_id_filter;
    accessed_mask_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->accessed_mask;
    accessed_filter_field = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->accessed_filter;
    command_field = dnx_data_l2.age_and_flush_machine.data_rules_get(unit)->command;
    payload_mask_field = dnx_data_l2.age_and_flush_machine.data_rules_get(unit)->payload_mask;
    payload_field = dnx_data_l2.age_and_flush_machine.data_rules_get(unit)->payload;

    flush_db_context.flush_in_bulk.get(unit, &flush_in_bulk);

    flush_tcam_mem = dnx_data_l2.age_and_flush_machine.filter_rules_get(unit)->table_name;
    flush_data_mem = dnx_data_l2.age_and_flush_machine.data_rules_get(unit)->table_name;

    /** Get the number of rules in the flush DB */
    flush_db_context.nof_valid_rules.get(unit, &nof_valid_rules);

    /** Make sure the flush DB is free enough for the new rules */
    if (nof_valid_rules + nof_rules > DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Flush DB is full");
    }

    /** Retrieve the app_db_id size to calculate the entry encoding */
    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.app_id_size.get(unit, DBAL_PHYSICAL_TABLE_LEM, app_db_id, &app_db_id_size));

    /** Retrieve the MDB_LEM entry size */
    SOC_FIND_FIELD(ENTRYf, SOC_MEM_INFO(unit, MDB_LEMm).fields, SOC_MEM_INFO(unit, MDB_LEMm).nFields, field_info);
    lem_entry_size = field_info->len;

    /** Run over the rules and write them to the flush data and tcam tables */
    for (rule_index = 0; rule_index < nof_rules; rule_index++)
    {
        uint8 vmv_size = 0;
        mdb_em_entry_encoding_e entry_encoding;

        next_available_rule = nof_valid_rules + rule_index;

        /** Clear the data and filter params */
        sal_memset(rule_filter, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE));
        sal_memset(rule_data, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE));

        /** Calculate the entry encoding */
        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, DBAL_PHYSICAL_TABLE_LEM, rules_info[rule_index].key_rule_size,
                         rules_info[rule_index].value_rule_size, app_db_id, app_db_id_size, &entry_encoding));

        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, DBAL_PHYSICAL_TABLE_LEM, entry_encoding, &vmv_size));

        /** Compose the LEM MDB entries for rule and mask */
        sal_memset(mdb_entry, 0x0, sizeof(mdb_entry));
        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, DBAL_PHYSICAL_TABLE_LEM, app_db_id, MDB_LEMm,
                                             rules_info[rule_index].key_rule_buf, rules_info[rule_index].key_rule_size,
                                             rules_info[rule_index].value_rule_buf,
                                             rules_info[rule_index].value_rule_size, entry_encoding, 0, mdb_entry));

        sal_memset(mdb_entry_mask, 0x0, sizeof(mdb_entry_mask));
        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, DBAL_PHYSICAL_TABLE_LEM, app_db_id, MDB_LEMm,
                                             rules_info[rule_index].key_rule_mask_buf,
                                             rules_info[rule_index].key_rule_size,
                                             rules_info[rule_index].value_rule_mask_buf,
                                             rules_info[rule_index].value_rule_size, entry_encoding, 0,
                                             mdb_entry_mask));

        SHR_IF_ERR_EXIT(dbal_flush_vmv_rule_set(unit, entry_encoding, vmv_size, mdb_entry, mdb_entry_mask));

        SHR_GET_SEVERITY_FOR_MODULE(severity);
        if ((severity >= bslSeverityVerbose))
        {
            uint32 data_offset;
            uint32 print_index;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_flush_rule_add: start\n")));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                                         rules_info[rule_index].key_rule_size, rules_info[rule_index].value_rule_size,
                                         dbal_physical_table_to_string(unit, DBAL_PHYSICAL_TABLE_LEM), app_db_id));
            for (data_offset = 0; data_offset < BITS2WORDS(rules_info[rule_index].key_rule_size); data_offset++)
            {
                print_index = BITS2WORDS(rules_info[rule_index].key_rule_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "rule_value->key[%d]: %08X.\n"), print_index,
                             rules_info[rule_index].key_rule_buf[print_index]));
            }
            for (data_offset = 0; data_offset < BITS2WORDS(rules_info[rule_index].value_rule_size); data_offset++)
            {
                print_index = BITS2WORDS(rules_info[rule_index].value_rule_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "rule_value->payload[%d]: %08X.\n"), print_index,
                             rules_info[rule_index].value_rule_buf[print_index]));
            }

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "key_size: %d. entry->payload_size: %d, physical_table: %s, app_id: %d.\n"),
                                         rules_info[rule_index].key_rule_size, rules_info[rule_index].value_rule_size,
                                         dbal_physical_table_to_string(unit, DBAL_PHYSICAL_TABLE_LEM), app_db_id));
            for (data_offset = 0; data_offset < BITS2WORDS(rules_info[rule_index].key_rule_size); data_offset++)
            {
                print_index = BITS2WORDS(rules_info[rule_index].key_rule_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "rule_mask->key[%d]: %08X.\n"), print_index,
                             rules_info[rule_index].key_rule_mask_buf[print_index]));
            }
            for (data_offset = 0; data_offset < BITS2WORDS(rules_info[rule_index].value_rule_size); data_offset++)
            {
                print_index = BITS2WORDS(rules_info[rule_index].value_rule_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "rule_mask->payload[%d]: %08X.\n"), print_index,
                             rules_info[rule_index].value_rule_mask_buf[print_index]));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Written to rules buffer, Written data: 0x")));
            for (data_offset = 0; data_offset < BITS2WORDS(lem_entry_size); data_offset++)
            {
                print_index = BITS2WORDS(lem_entry_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), mdb_entry[print_index]));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nWritten to rules buffer, Written mask: 0x")));
            for (data_offset = 0; data_offset < BITS2WORDS(lem_entry_size); data_offset++)
            {
                print_index = BITS2WORDS(lem_entry_size) - 1 - data_offset;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), mdb_entry_mask[print_index]));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nmdb_flush_rule_add: end\n")));
        }

        if (rules_info[rule_index].action_types_btmp & FLUSH_DB_COMMAND_GET_BIT)
        {
            if (flush_in_bulk)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Bulk mode must be used with no callbacks and no SW additional editing");
            }

            /** Resume the DMA of the flush machine if a get command is used */
            if (!first_get_command)
            {
                first_get_command = 1;

                /** Resume the flush DMA */
                SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &flush_channel));
                SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_resume(unit, flush_channel));
            }
        }

        /***************** Set action rules *********************/
        /** Set action type */
        field_val = rules_info[rule_index].action_types_btmp;

        soc_mem_field_set(unit, flush_data_mem, rule_data, command_field, &field_val);

        tmp_align_buf[0] = tmp_align_buf[1] = tmp_align_buf[2] = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(rules_info[rule_index].value_action_mask_buf,
                                                       96 - vmv_size - rules_info[rule_index].value_rule_size,
                                                       rules_info[rule_index].value_rule_size, tmp_align_buf));

        /** PAYLOAD_MASK   */
        /** No need to flip mask. 0 - don't care in this memory */
        soc_mem_field_set(unit, flush_data_mem, rule_data, payload_mask_field, tmp_align_buf);

        /** PAYLOAD */
        tmp_align_buf[0] = tmp_align_buf[1] = tmp_align_buf[2] = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(rules_info[rule_index].value_action_buf,
                                                       96 - vmv_size - rules_info[rule_index].value_rule_size,
                                                       rules_info[rule_index].value_rule_size, tmp_align_buf));

        soc_mem_field_set(unit, flush_data_mem, rule_data, payload_field, tmp_align_buf);

        SHR_IF_ERR_EXIT(soc_mem_write(unit, flush_data_mem, MEM_BLOCK_ALL, next_available_rule, (void *) rule_data));

        /***************** Set filter rules *********************/

        /** MDB_LEM_FLUSH_DATA is split to two tables. Table index should be updated. */
        if (next_available_rule >= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE)
        {
            next_available_rule -= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE;
            table_index = 1;
        }

        field_val = 1;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, valid_field, &field_val);

        /** ACCESSED_MASK */
        field_val = rules_info[rule_index].hit_indication_rule_mask ^ 1;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, accessed_mask_field, &field_val);

        /** APP_DB_MASK - App DB mask field is always using all 6 bits */
        field_val = 0x0;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, appdb_id_mask_field, &field_val);

        /** MDB ENTRY MASK */
        /** Flip the mask from 0 don't care to 1 don't care */
        SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(mdb_entry_mask, 0, lem_entry_size - 1));
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, entry_mask_field, mdb_entry_mask);

        /** SRC_MASK - All the 3 src bits are used */
        field_val = 7;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, src_mask_field, &field_val);

        /** APP DB ID */
        field_val = app_db_id;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, appdb_id_filter_field, &field_val);

        /** ACCESSED DATA */
        field_val = rules_info[rule_index].hit_indication_rule;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, accessed_filter_field, &field_val);

        /** MDB entry filter */
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, entry_filter_field, mdb_entry);

        /** Select which parts are influenced by the rules (MRQ, PIPE and SCAN) */
        field_val = FLUSH_DB_ALL_SOURCES;
        soc_mem_field_set(unit, flush_tcam_mem, rule_filter, src_filter_field, &field_val);

        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, flush_tcam_mem, table_index, MEM_BLOCK_ALL, next_available_rule,
                                            (void *) rule_filter));
    }

    /** Update the number of valid rules */
    nof_valid_rules += nof_rules;

    flush_db_context.nof_valid_rules.set(unit, nof_valid_rules);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Rerun the age machine.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_unpause_age_machine(
    int unit)
{
    uint64 data;
    soc_reg_above_64_val_t data_above_64;
    soc_reg_above_64_val_t data_above_64_bck;
    uint32 age_machine_status = 1; /** Start as not in Idle */
    soc_reg_t lem_emp_age_reg, lem_emp_age_scan_pulse_reg, lem_em_status_reg;
    soc_field_t disable_age_field, lem_emp_scan_status_field;

    SHR_FUNC_INIT_VARS(unit);

    lem_emp_age_reg = dnx_data_l2.age_and_flush_machine.age_get(unit)->age_config;
    lem_emp_age_scan_pulse_reg = dnx_data_l2.age_and_flush_machine.age_get(unit)->scan_pulse;
    lem_em_status_reg = dnx_data_mdb.em.em_info_get(unit, DBAL_PHYSICAL_TABLE_LEM)->status_reg;
    disable_age_field = dnx_data_l2.age_and_flush_machine.age_get(unit)->disable_aging;
    lem_emp_scan_status_field = dnx_data_mdb.em.em_info_get(unit, DBAL_PHYSICAL_TABLE_LEM)->emp_scan_status_field;

    /** Save the mdb banks status before disabling all of them */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, lem_emp_age_reg, REG_PORT_ANY, 0, data_above_64));
    sal_memcpy(data_above_64_bck, data_above_64, sizeof(uint32) * SOC_REG_ABOVE_64_MAX_SIZE_U32);

    /** Disable all mdb banks from the influence of the age machine */
    soc_reg_above_64_field32_set(unit, lem_emp_age_reg, data_above_64, disable_age_field, 0xFF);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, lem_emp_age_reg, REG_PORT_ANY, 0, data_above_64));

    /** Trigger age machine */
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, lem_emp_age_scan_pulse_reg, REG_PORT_ANY, 0, 1));

    /** Wait for age machine to finish */

    /** Check if the status is Idle */
    while (age_machine_status != 0)
    {
        SHR_IF_ERR_EXIT(soc_reg64_get(unit, lem_em_status_reg, REG_PORT_ANY, 0, &data));

        /** Read age machine status */
        age_machine_status = soc_reg64_field32_get(unit, lem_em_status_reg, data, lem_emp_scan_status_field);

        /** In case the age machine is not done, sleep 1 milisecond */
        if (age_machine_status != 0)
        {
            /** Sleep 1 milisecond */
            sal_usleep(1000);
        }
    }

    /** Enable mdb banks */
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, lem_emp_age_reg, REG_PORT_ANY, 0, data_above_64_bck));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Do needed actions after the flush machine is done.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_flush_end(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_machine_pause_after_flush))
    {
        SHR_IF_ERR_EXIT(dnx_l2_unpause_age_machine(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flush_start(
    int unit,
    uint8 bulk_commit,
    uint8 hw_accelerated,
    uint8 non_blocking)
{
    uint8 flush_in_bulk;
    int flush_in_work = 0;
    soc_reg_t mdb_flush_reg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_flush_in_work(unit, &flush_in_work));

    /** Make sure the flush is not in work */
    if (flush_in_work)
    {
        SHR_ERR_EXIT(_SHR_E_BUSY, "Flush machine can not be started before the previous run is done.\n");
    }
    flush_in_work = 1;

    if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal dma operations not supported in Error Recovery transactions!\n");
    }

    flush_db_context.flush_in_bulk.get(unit, &flush_in_bulk);

    mdb_flush_reg = dnx_data_l2.age_and_flush_machine.flush_get(unit)->flush_pulse;

    /** In bulk mode the flush machine doesn't start when it is called as part of rules set operation */
    if (bulk_commit || !flush_in_bulk)
    {
        /** Start the flush machine */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, mdb_flush_reg, REG_PORT_ANY, 0, 1));

        /** Block the API until the flush machine is done. Block only in case there is no use of the DMA (HW accelerated) and non-blocking=0   */
        while (flush_in_work && hw_accelerated && !non_blocking)
        {
            SHR_IF_ERR_EXIT(dbal_flush_in_work(unit, &flush_in_work));

            /** In case the flush is not done, sleep 1 milisecond */
            if (flush_in_work)
            {
                /** Sleep 1 milisecond */
                sal_usleep(1000);
            }
        }
    }

    /** In case the flush is not in bulk mode, clear the flush rules and actions. Do not clear in non-blocking mode. */
    if (!flush_in_bulk && hw_accelerated && !non_blocking)
    {
        SHR_IF_ERR_EXIT(dbal_flush_db_clear(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flush_mode_collect(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    flush_db_context.flush_in_bulk.set(unit, 1);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flush_mode_get(
    int unit,
    int *collect_mode)
{

    SHR_FUNC_INIT_VARS(unit);

    flush_db_context.flush_in_bulk.get(unit, (uint8 *) collect_mode);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flush_mode_normal(
    int unit)
{

    uint32 nof_valid_rules = 0;

    SHR_FUNC_INIT_VARS(unit);

    flush_db_context.nof_valid_rules.get(unit, &nof_valid_rules);

    if (nof_valid_rules)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot change the traverse mode, there are valid rules in flush\n");
    }

    flush_db_context.flush_in_bulk.set(unit, 0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flush_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int dma_channel_id;
    int entries_count = 0;
    uint8 *retrievd_entries = NULL;
    uint32 *retrievd_entries_32 = NULL;
    uint32 entry_size;
    int flush_in_work = 1;
    uint32 command_type = (uint32) -1;/** init to invalid value */
    int tested_entries_index = 0;
    int found_valid_entry = 0;
    int flush_channel;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    iterator_info->used_first_key = TRUE;

    /** Get DMA channel ID */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &dma_channel_id));

    /** Continue looking for results until the flush is done or a result is available */
    while (flush_in_work && (found_valid_entry == 0))
    {
        /** Read an entry from the DMA's host memory */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_num_entries_get
                        (unit, dma_channel_id, &entries_count, &retrievd_entries, &entry_size));

        retrievd_entries_32 = (uint32 *) retrievd_entries;

        /** Filter out the entries that are not coming from the flush machine according to the command field */
        while (!found_valid_entry && (entries_count - tested_entries_index > 0))
        {
            tested_entries_index++;

            /** Check the command type */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(retrievd_entries_32, MACT_CPU_REQUEST_COMMAND_OFFSET_BIT,
                                                           MACT_CPU_REQUEST_COMMAND_NOF_BITS, &command_type));

            /** If the entry is not a flush entry, move to the next entry in the queue */
            if (command_type == MACT_CPU_REQUEST_COMMAND_FLUSH)
            {
                found_valid_entry = 1;
            }
            else
            {
                retrievd_entries_32 = (uint32 *) ((uint8 *) retrievd_entries_32 + entry_size);
            }
        }

        /** In case there are no entries check if the flush is still working */
        if (found_valid_entry == 0)
        {
            SHR_IF_ERR_EXIT(dbal_flush_in_work(unit, &flush_in_work));

            if (flush_in_work)
            {
                /** Sleep 1 milisecond */
                sal_usleep(1000);
            }
        }
    }

    /** Make sure that no entry was received between the last check of the DMA's memory and the flush machine's status */
    if (entries_count == 0)
    {
        /** Read an entry from the DMA's host memory */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_num_entries_get
                        (unit, dma_channel_id, &entries_count, &retrievd_entries, &entry_size));
        retrievd_entries_32 = (uint32 *) retrievd_entries;

        /** Filter out the entries that are not coming from the flush machine according to the command field */
        while (!found_valid_entry && (entries_count - tested_entries_index > 0))
        {
            tested_entries_index++;

            /** Check the command type */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(retrievd_entries_32, MACT_CPU_REQUEST_COMMAND_OFFSET_BIT,
                                                           MACT_CPU_REQUEST_COMMAND_NOF_BITS, &command_type));

            /** If the entry is not a flush entry, move to the next entry in the queue */
            if (command_type == MACT_CPU_REQUEST_COMMAND_FLUSH)
            {
                found_valid_entry = 1;
            }
            else
            {
                retrievd_entries_32 = (uint32 *) ((uint8 *) retrievd_entries_32 + entry_size);
            }
        }
    }

    if (found_valid_entry)
    {
        /**
         * Fill the DBAL handle with the fetched entry
         */
        /** Key is aligned to the MSB. Payload is right after the key */

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                        (unit, entry_handle->handle_id, retrievd_entries_32, NULL, _SHR_CORE_ALL, FLUSH_MACT_ENTRY_SIZE,
                         retrievd_entries_32));
    }
    else
    {
        /** In case there are no results and the flush is done return is_end=TRUE */
        iterator_info->is_end = TRUE;

        /** Clear flush rules */
        SHR_IF_ERR_EXIT(dbal_flush_db_clear(unit));
    }

    /** In case of no valid entry dbal_flush_db_clear is releasing the entries from the DMA's memory */
    if (tested_entries_index > 0 && found_valid_entry)
    {
        /** Release the memory of the used DMA entry */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_set_entries_read(unit, dma_channel_id, tested_entries_index));
    }

    /** Pause the DMA of the flush machine */
    if (iterator_info->is_end == TRUE)
    {
        /** Pause the flush DMA */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &flush_channel));
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_pause(unit, flush_channel));
    }

exit:
    SHR_FUNC_EXIT;
}
