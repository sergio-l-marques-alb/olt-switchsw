/** \file dbal_mdb_flush_access.c
 * $Id$
 *
 * Access functions used by the dbal to interact with the DMA. 
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>

#include "dbal_internal.h"
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>
#include <soc/fifodma.h>
#include <soc/dnx/dnx_fifodma.h>
#include <sal/types.h>
#include <soc/dnx/swstate/auto_generated/access/flush_access.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/** Get bit in the command bitmap */
#define FLUSH_DB_COMMAND_GET_BIT 4
#define FLUSH_MACHINE_STATUS_IDLE 0
#define FLUSH_DB_ALL_SOURCES 0x7
#define MACT_KEY_SIZE_IN_BITS 80
#define MACT_PAYLOAD_SIZE_IN_BITS 64
#define ENTRY_MASK_SIZE 8 /** Size in words */
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

    if SOC_IS_JERICHO_2_A0
        (unit)
    {
        reg = MDB_REG_212r;
    }
    else if SOC_IS_JERICHO_2_B0
        (unit)
    {
        reg = MDB_REG_221r;
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));

    /** ITEM_36_37 is the flush status field */
    flush_status = soc_reg_above_64_field32_get(unit, reg, reg_above_64_val, ITEM_36_37f);

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

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(rule_filter, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE));
    sal_memset(rule_data, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE));

    /** Run over the rules and actions and clear them one by one */
    for (action_index = 0; action_index < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; action_index++)
    {
        /*
         * MDB_LEM_FLUSH_DATAm bacame MDB_MEM_24000m 
         */
        SHR_IF_ERR_EXIT(soc_mem_write(unit, MDB_MEM_24000m, MEM_BLOCK_ALL, action_index, (void *) rule_data));

        /*
         * MDB_LEM_FLUSH_TCAMm bacame MDB_MEM_23000m 
         */
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, MDB_MEM_23000m, table_index, MEM_BLOCK_ALL, rule_index, (void *) rule_filter));

        rule_index++;

        if (rule_index == DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE)
        {
            rule_index -= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE;
            table_index = 1;
        }
    }

    flush_db_context.nof_valid_rules.set(unit, 0);

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

/**
 * \brief - Set flush rules and action in flush DB
 * 
 * \param [in] unit - unit id
 * \param [in] app_db_id - app_db_id used by the flush machine
 * \param [in] nof_rules - number of rules to set
 * \param [in] rules_info - list of rules to set
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

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal dma operations not supported in Error Recovery transactions!\n");
    }

    flush_db_context.flush_in_bulk.get(unit, &flush_in_bulk);

    /** Get the number of rules in the flush DB */
    flush_db_context.nof_valid_rules.get(unit, &nof_valid_rules);

    /** Make sure the flush DB is free enough for the new rules */
    if (nof_valid_rules + nof_rules > DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Flush DB is full");
    }

    /** Run over the rules and write them to the flush data and tcam tables */
    for (rule_index = 0; rule_index < nof_rules; rule_index++)
    {
        next_available_rule = nof_valid_rules + rule_index;

        /** Clear the data and filter params */
        sal_memset(rule_filter, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_RULE_SIZE));
        sal_memset(rule_data, 0, WORDS2BYTES(DNX_DATA_MAX_L2_DMA_FLUSH_DB_DATA_SIZE));

        /** Compose the LEM MDB entries for rule and mask */
        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, DBAL_PHYSICAL_TABLE_LEM, app_db_id, MDB_LEMm,
                                             rules_info[rule_index].key_rule_buf, rules_info[rule_index].key_rule_size,
                                             rules_info[rule_index].value_rule_buf,
                                             rules_info[rule_index].value_rule_size, mdb_entry));

        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, DBAL_PHYSICAL_TABLE_LEM, app_db_id, MDB_LEMm,
                                             rules_info[rule_index].key_rule_mask_buf,
                                             rules_info[rule_index].key_rule_size,
                                             rules_info[rule_index].value_rule_mask_buf,
                                             rules_info[rule_index].value_rule_size, mdb_entry_mask));

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

        
        soc_mem_field_set(unit, MDB_MEM_24000m, rule_data, ITEM_0_3f, &field_val);      /* COMMAND == ITEM_0_3 */

        
        tmp_align_buf[0] = tmp_align_buf[1] = tmp_align_buf[2] = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(rules_info[rule_index].value_action_mask_buf,
                                                       93 - rules_info[rule_index].value_rule_size,
                                                       rules_info[rule_index].value_rule_size, tmp_align_buf));

        /** PAYLOAD_MASK   */
        /** PAYLOAD_MASK == ITEM_100_195 */
        /** No need to flip mask. 0 - don't care in this memory */
        soc_mem_field_set(unit, MDB_MEM_24000m, rule_data, ITEM_4_99f, tmp_align_buf);

        /** PAYLOAD */
        /** PAYLOAD == ITEM_4_99 */
        
        tmp_align_buf[0] = tmp_align_buf[1] = tmp_align_buf[2] = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(rules_info[rule_index].value_action_buf,
                                                       93 - rules_info[rule_index].value_rule_size,
                                                       rules_info[rule_index].value_rule_size, tmp_align_buf));

        soc_mem_field_set(unit, MDB_MEM_24000m, rule_data, ITEM_100_195f, tmp_align_buf);

        SHR_IF_ERR_EXIT(soc_mem_write(unit, MDB_MEM_24000m, MEM_BLOCK_ALL, next_available_rule, (void *) rule_data));

        /***************** Set filter rules *********************/

        /** MDB_LEM_FLUSH_DATA is split to two tables. Table index should be updated. */
        if (next_available_rule >= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE)
        {
            next_available_rule -= DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES_PER_TABLE;
            table_index = 1;
        }

        field_val = 1;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_0_0f, &field_val);    /* VALID == ITEM_0_0 */

        /** ACCESSED_MASK */
        field_val = rules_info[rule_index].hit_indication_rule_mask ^ 1;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_1_1f, &field_val);    /* ACCESSED_MASK == ITEM_1_1 */

        /** APP_DB_MASK - App DB mask field is always using all 6 bits */
        field_val = 0x0;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_2_7f, &field_val);    /* APP_DB_MASK == ITEM_2_7 */

        /** MDB ENTRY MASK */
        /** Flip the mask from 0 don't care to 1 don't care */
        SHR_IF_ERR_EXIT(utilex_bitstream_not(mdb_entry_mask, ENTRY_MASK_SIZE));
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_8_264f, mdb_entry_mask);      /* ENTRY_MASK ==
                                                                                                 * ITEM_8_264 */

        /** SRC_MASK - All the 3 src bits are used */
        field_val = 7;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_265_267f, &field_val);        /* SRC_MASK ==
                                                                                                 * ITEM_265_267 */

        /** APP DB ID */
        field_val = app_db_id;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_269_274f, &field_val);        /* APP_DB_DATA ==
                                                                                                 * ITEM_269_274 */

        /** ACCESSED DATA */
        field_val = rules_info[rule_index].hit_indication_rule;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_268_268f, &field_val);        /* ACCESSED_DATA ==
                                                                                                 * ITEM_268_268 */

        /** MDB entry filter */
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_275_531f, mdb_entry); /* ENTRY_DATA == ITEM_275_531 */

        /** Select which parts are influenced by the rules (MRQ, PIPE and SCAN) */
        field_val = FLUSH_DB_ALL_SOURCES;
        soc_mem_field_set(unit, MDB_MEM_23000m, rule_filter, ITEM_532_534f, &field_val);        /* SRC_DATA ==
                                                                                                 * ITEM_532_534 */

        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, MDB_MEM_23000m, table_index, MEM_BLOCK_ALL, next_available_rule,
                                            (void *) rule_filter));
    }

    /** Update the number of valid rules */
    nof_valid_rules += nof_rules;

    flush_db_context.nof_valid_rules.set(unit, nof_valid_rules);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Rerun the age machine due to HW bug.
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

    SHR_FUNC_INIT_VARS(unit);

    /** Save the mdb banks status before disabling all of them */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, MDB_REG_181r, REG_PORT_ANY, 0, data_above_64));
    sal_memcpy(data_above_64_bck, data_above_64, sizeof(uint32) * SOC_REG_ABOVE_64_MAX_SIZE_U32);

    /** Disable all mdb banks from the influence of the age machine */
    soc_reg_above_64_field32_set(unit, MDB_REG_181r, data_above_64, ITEM_360_367f, 0xFF);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, MDB_REG_181r, REG_PORT_ANY, 0, data_above_64));

    /** Trigger age machine */
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_REG_3020r, REG_PORT_ANY, 0, 1));

    /** Wait for age machine to finish */

    /** Check if the status is Idle */
    while (age_machine_status != 0)
    {
        SHR_IF_ERR_EXIT(soc_reg64_get(unit, MDB_REG_212r, REG_PORT_ANY, 0, &data));

        /** Read age machine status */
        age_machine_status = soc_reg64_field32_get(unit, MDB_REG_212r, data, ITEM_36_37f);

        /** In case the age machine is not done, sleep 1 milisecond */
        if (age_machine_status != 0)
        {
            /** Sleep 1 milisecond */
            sal_usleep(1000);
        }
    }

    /** Enable mdb banks */
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, MDB_REG_181r, REG_PORT_ANY, 0, data_above_64_bck));

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

    if (dnx_data_l2.hw_bug.feature_get(unit, dnx_data_l2_hw_bug_age_machine_pause_after_flush))
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
    uint8 hw_accelerated)
{
    uint8 flush_in_bulk;
    int flush_in_work = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal dma operations not supported in Error Recovery transactions!\n");
    }

    flush_db_context.flush_in_bulk.get(unit, &flush_in_bulk);

    /** In bulk mode the flush machine doesn't start when it is called as part of rules set operation */
    if (bulk_commit || !flush_in_bulk)
    {
        /** Start the flush machine */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_REG_3021r, REG_PORT_ANY, 0, 1));

        /** Block the API until the flush machine is done. Block only in case there is no use of the DMA. (HW accelerated) */
        while (flush_in_work && hw_accelerated)
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

    /** In case the flush is not in bulk mode, clear the flush rules and actions */
    if (!flush_in_bulk && hw_accelerated)
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
                        (unit, entry_handle->handle_id, retrievd_entries_32, _SHR_CORE_ALL, FLUSH_MACT_ENTRY_SIZE,
                         retrievd_entries_32));
    }
    else
    {
        /** In case there are no results and the flush is done return is_end=TRUE */
        iterator_info->is_end = TRUE;

        /** Clear flush rules */
        SHR_IF_ERR_EXIT(dbal_flush_db_clear(unit));

        /** Reset flush machine trigger for the next run */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_REG_3021r, REG_PORT_ANY, 0, 0));
    }

    if (tested_entries_index > 0)
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
