/** \file src/bcm/dnx/dram/buffers/buffers_quarantine_mechanism.c
 *
 *
 *  This file contains implementation of functions for buffers quarantine mechanism
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */

#include <ctype.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "buffers_quarantine_mechanism.h"
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <shared/util.h>

/*
 * }
 */

/*
 * defines
 * {
 */

#define ACTIVATE 1
#define ACTION_DONE 0
#define TOKEN_MAX_SIZE 20

/** 
 * Dram deleted buffers list shouldn't be above this value.
 * If it is, then dram is probably malfunctioning.
 */
#define DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE (1000)

/*
 * }
 */

/** verify function */
static shr_error_e
dnx_dram_buffers_quarantine_read_buffer_verify(
    int unit,
    bcm_core_t core,
    uint32 *buffer,
    uint32 *buffer_is_valid,
    uint32 *quarantine_reason)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");
    SHR_NULL_CHECK(buffer_is_valid, _SHR_E_PARAM, "buffer_is_valid");
    SHR_NULL_CHECK(quarantine_reason, _SHR_E_PARAM, "quarantine_reason");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - read next buffer from quarantine fifo
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [out] buffer - read bdb
 * \param [out] buffer_is_valid - indication if buffer read from fifo is valid - when the fifo is empty, the read entry is invalid
 * \param [out] quarantine_reason - the reason that caused buffer to be quarantined
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantine_read_buffer(
    int unit,
    bcm_core_t core,
    uint32 *buffer,
    uint32 *buffer_is_valid,
    uint32 *quarantine_reason)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_dram_buffers_quarantine_read_buffer_verify
                           (unit, core, buffer, buffer_is_valid, quarantine_reason));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NEXT_QUARANTINED_BDB_IN_FIFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINED_BUFFER_IS_VALID, INST_SINGLE,
                               buffer_is_valid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINED_BDB, INST_SINGLE, buffer);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINE_REASON, INST_SINGLE, quarantine_reason);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - restore buffer read from quarantine fifo back to usage, buffer should be a valid
 *          buffer returned by dnx_dram_buffers_quarantine_read_buffer function
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - bdb to restore
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantined_buffer_restore(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** release buffer */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RELEASE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ACTIVATE, INST_SINGLE, ACTIVATE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_TO_RELEASE, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get indication that release was successful */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_RELEASE_BDB_CONTROL, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_RELEASE_ACTIVATE, ACTION_DONE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** schedule buffer to quarantine FIFO next time it is read */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_ACTIVATE, INST_SINGLE, ACTIVATE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_TO_DELETE, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_buffer_to_quarantine_done(
    int unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get indication that buffer was moved to quarantine FIFO */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_DELETE_ACTIVATE, ACTION_DONE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - update in SW the number of quarantined buffers.
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_nof_quarantined_buffers_update(
    int unit)
{
    uint32 entry_handle_id;
    uint32 deleted_bdbs_counter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** update delete buffers counter */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETED_BDB_COUNTER, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE,
                               &deleted_bdbs_counter);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE,
                                 ++deleted_bdbs_counter);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
dnx_dram_buffers_quarantine_nof_deleted_buffers_get_verify(
    int unit,
    uint32 *nof_deleted_buffers)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_deleted_buffers, _SHR_E_PARAM, "nof_deleted_buffers");

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_nof_deleted_buffers_get(
    int unit,
    uint32 *nof_deleted_buffers)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_dram_buffers_quarantine_nof_deleted_buffers_get_verify(unit, nof_deleted_buffers));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETED_BDB_COUNTER, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE, nof_deleted_buffers);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
dnx_dram_buffers_quarantine_nof_free_buffers_get_verify(
    int unit,
    uint32 *nof_free_buffers)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_free_buffers, _SHR_E_PARAM, "nof_free_buffers");

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_nof_free_buffers_get(
    int unit,
    uint32 *nof_free_buffers)
{
    uint32 entry_handle_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_dram_buffers_quarantine_nof_free_buffers_get_verify(unit, nof_free_buffers));
    *nof_free_buffers = 0;

    /** prevent dqm from using buffers and release all the buffers currently in the DQM back to the BDM */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DQM_CONTROL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DQM_CONTROL, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** read number of free buffer in every core and sum them up */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        uint32 free_buffers;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FREE_BDB_COUNTER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FREE_BDB_COUNTER, INST_SINGLE, &free_buffers);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *nof_free_buffers += free_buffers;
    }

    /** restore DQM ability to use buffers */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DQM_CONTROL, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_buffer_flush_all(
    int unit)
{
    int core;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        uint32 buffer;
        uint32 buffer_is_valid;
        uint32 quarantine_reason;
        do
        {
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_read_buffer
                            (unit, core, &buffer, &buffer_is_valid, &quarantine_reason));
            if (buffer_is_valid)
            {
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_restore(unit, core, buffer));
            }
        }
        while (buffer_is_valid);
    }

exit:
    SHR_FUNC_EXIT;
}

/** check buffer index is valid for device and number of enabled drams */
shr_error_e
dnx_dram_buffers_buffer_verify(
    int unit,
    uint32 global_buffer_index,
    uint32 enabled_drams_bitmap)
{
    int max_nof_bdbs_per_core = dnx_data_dram.buffers.nof_bdbs_get(unit);
    int max_nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    int max_nof_bdbs_per_fpc_bank = max_nof_bdbs_per_core / dnx_data_dram.buffers.nof_fpc_banks_get(unit);
    int nof_bdbs_per_bank;
    int nof_enabled_drams;
    SHR_FUNC_INIT_VARS(unit);

    shr_bitop_range_count(&enabled_drams_bitmap, 0, max_nof_drams, &nof_enabled_drams);

    nof_bdbs_per_bank = (max_nof_bdbs_per_fpc_bank * nof_enabled_drams) / max_nof_drams;

    if ((global_buffer_index >= max_nof_bdbs_per_core)
        || (global_buffer_index % max_nof_bdbs_per_fpc_bank) >= nof_bdbs_per_bank)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Received buffer index (%d) is invalid.\n", global_buffer_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/** set delete reasons for quarantine mechanism, this will cause a buffer to reach the quarantine FIFO if its
 * reason is set */
static shr_error_e
dnx_dram_buffers_quarantine_delete_reasons_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONFIGS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_LAST_BUFF_CRC_EN, INST_SINGLE, 1);
    if (dnx_data_dram.buffers.feature_get(unit, dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_TDU_ERR_EN, INST_SINGLE, 1);
    }
    if (dnx_data_dram.buffers.feature_get(unit, dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_TDU_CORRECTED_ERR_EN, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_ECC_ERR_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_CRC_ERR_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_PKTSIZE_ERR_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get whether a buffer exists in BDB_ERRORS_COUNTER table.
 *          BDB_ERRORS_COUNTER is a hash table, so it doesn't have an entry per buffer.
 *          It has entries only for buffers we explicitly put into it.
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - buffer
 * \param [out] is_buffer_exists - true/false is the buffer exists/not_exists in table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantine_is_buffer_exists_in_table(
    int unit,
    bcm_core_t core,
    uint32 buffer,
    int *is_buffer_exists)
{
    uint32 entry_handle_id;
    uint32 dummy;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, &dummy);
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *is_buffer_exists = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        *is_buffer_exists = TRUE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of errors that a buffer triggered
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - buffer
 * \param [out] nof_errors - number of errors for core,buffer
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantine_nof_error_get(
    int unit,
    bcm_core_t core,
    uint32 buffer,
    uint32 *nof_errors)
{
    uint32 entry_handle_id;
    int is_buffer_exists;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If the buffer doesn't exists in the hash table it means is doesn't have any errors until now.
     * We need to check this, otherwise 'dbal_entry_get' will return an error in case the entry doesn't
     * exists.
     */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_is_buffer_exists_in_table(unit, core, buffer, &is_buffer_exists));
    if (is_buffer_exists == FALSE)
    {
        *nof_errors = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, nof_errors);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set number of errors that a buffer triggered
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - buffer
 * \param [in] nof_errors - number of errors for core,buffer
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantine_nof_error_set(
    int unit,
    bcm_core_t core,
    uint32 buffer,
    uint32 nof_errors)
{
    uint32 entry_handle_id;
    int is_buffer_exists;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Check if buffer exists in the hash table */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_is_buffer_exists_in_table(unit, core, buffer, &is_buffer_exists));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, nof_errors);

    /*
     * Since BDB_ERRORS_COUNTER is a hash table, the first 'set' to a certain buffer should be 'committed' and subsequent 'sets' should be 'updated'.
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit
                    (unit, entry_handle_id, (is_buffer_exists == FALSE) ? DBAL_COMMIT : DBAL_COMMIT_UPDATE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** read a token from file handle */
/** the deleted buffer file supposed to have only digits with spaces between 2 numbers */
static shr_error_e
dnx_dram_buffers_quarantine_token_read(
    int unit,
    FILE * p_file,
    uint32 *val,
    uint8 *token_found)
{
    int index = 0;
    char token[TOKEN_MAX_SIZE] = { 0 };
    char c_from_file;

    SHR_FUNC_INIT_VARS(unit);
    *token_found = FALSE;

    for (index = 0; index < TOKEN_MAX_SIZE; index++)
    {
        c_from_file = (char) sal_fgetc(p_file);
        if (sal_feof(p_file))
        {
            break;
        }
        if (c_from_file == ' ' || c_from_file == '\n')
        {
            *token_found = TRUE;
            break;
        }
        /*
         * Tokens are unsigned integers in decimal base (see write to file in
         * dnx_dram_buffers_quarantine_buffer_to_file_write) 
         */
        if (c_from_file < '0' || c_from_file > '9')
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal token read from file. Token's hex value is %x\n", c_from_file);
        }

        token[index] = c_from_file;
    }

    if (index == TOKEN_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "token is too big\n");
    }
    if (*token_found)
    {
        *val = _shr_ctoi(token);
    }
    else
    {
        *val = -1;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Delete indicated buffer (BDB pointer) from the pool of usable buffers.
 * \param [in] unit -
 *   Unit HW id
 * \param [in] core -
 *   bcm_core_t. The core to which the buffer is attached. Must be valid.
 * \param [in] buffer_identifier -
 *   uint32. Identifier of the buffer to delete. Must be smaller than
 *   dnx_data_dram.buffers.nof_bdbs
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This procedure does nothing unless the 'is_delete_bdb_supported'
 *     feature is TRUE.
 *   * This procedure should only be called for buffers which were found
 *     to have failed 'dnx_data_dram.buffers.allowed_errors_get' times and,
 *     so, they are useless.
 * \see
 *   * dnx_dram_buffers_quarantine_init
 */
static shr_error_e
dnx_dram_buffers_initiated_deletion(
    int unit,
    bcm_core_t core,
    uint32 buffer_identifier)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        /*
         * Verify received buffer and core are in range,
         * If they are not, it might be a sign of a corrupted file
         */
        uint32 delete_ptr_failed;

        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_identifier,
                                                       dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap));
        DNXCMN_CORE_VALIDATE(unit, core, FALSE);
        /*
         * This is the delete action itself.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DELETE_BDB_POINTER,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PTR_TO_DELETE, INST_SINGLE, buffer_identifier,
                                         GEN_DBAL_FIELD_LAST_MARK));
        /*
         * Here we should wait 10 clock cycles but this is taken care of by the
         * (relatively slow) speed of the software.
         * Now, verify the delete action has not failed.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_DELETE_BDB_POINTER,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_DELETE_PTR_FAILED, INST_SINGLE,
                                         &delete_ptr_failed, GEN_DBAL_FIELD_LAST_MARK));
        if (delete_ptr_failed)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n" "HW indicates failure to delete buffer %d on core %d.\r\n", buffer_identifier, core);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Open all gates ('enable') to start deletion ('pre') of bdbs (dram buffers)
 *   which were marked as faulty (due to multiple failures).
 *   OR
 *   Close all gates ('disable') after deletion ('post') of bdbs (dram buffers)
 *   which were marked as faulty (due to multiple failures).
 * \param [in] unit -
 *   Unit HW id
 * \param [in] enable -
 *   int. Boolean. If non-zero (TRUE) the 'enable' operation is required
 *   ('pre deletion').
 *   If zero (FALSE) then 'disable' operation is required ('post deletion').
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * This procedure does nothing unless the 'is_delete_bdb_supported'
 *     feature is TRUE.
 * \see
 *   * dnx_dram_buffers_quarantine_init
 */
static shr_error_e
dnx_dram_buffers_bdbs_pre_post_deletion_enable(
    int unit,
    int enable)
{
    uint32 enable_enqueue;
    uint32 enable_allocate;
    uint32 release_all_bdbs;
    uint32 move_all_ptr_to_mem;
    int poll_total_count, poll_counter;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        if (enable != FALSE)
        {
            uint32 nof_bdbs;
            int matched;
            uint32 nof_free_buffers;

            enable_enqueue = 0;
            enable_allocate = 0;
            release_all_bdbs = 1;
            move_all_ptr_to_mem = 1;
            /*
             * Stop allocation and enqueuing of BDBs.
             * Release all BDBs from their curently allocated positions and put
             * them in original pool.
             * Verify all BDBs were released.
             * Move all BDBs to 'memory'.
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDB_CONFIG,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, enable_allocate,
                                             GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_EN, INST_SINGLE, enable_enqueue,
                                             GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE,
                                             release_all_bdbs, GEN_DBAL_FIELD_LAST_MARK));
            poll_total_count = NUM_POLL_TOTAL_COUNT;
            SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_bdbs_per_core_get(unit, &nof_bdbs));
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                matched = FALSE;
                for (poll_counter = 0; poll_counter < poll_total_count; poll_counter++)
                {
                    /*
                     * Just to be on the safe side...
                     */
                    nof_free_buffers = 0;
                    SHR_IF_ERR_EXIT
                        (dnx_dbal_gen_get(unit, DBAL_TABLE_FREE_BDB_COUNTER,
                                          1, 1,
                                          GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                          GEN_DBAL_FIELD32, DBAL_FIELD_FREE_BDB_COUNTER, INST_SINGLE, &nof_free_buffers,
                                          GEN_DBAL_FIELD_LAST_MARK));
                    if (nof_free_buffers == nof_bdbs)
                    {
                        matched = TRUE;
                        break;
                    }
                }
                if (matched == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "Number of free buffers on core %d after %d poll cycles is %d (Should be %d)\r\n",
                                 core, poll_counter, nof_free_buffers, nof_bdbs);
                }
            }
            /*
             * At this point, all BDBs are 'released'.
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDM_INIT,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_MOVE_ALL_PTR_TO_MEM, INST_SINGLE,
                                             move_all_ptr_to_mem, GEN_DBAL_FIELD_LAST_MARK));
        }
        else
        {
            enable_enqueue = 1;
            enable_allocate = 1;
            release_all_bdbs = 0;
            move_all_ptr_to_mem = 0;
            /*
             * Move all BDBs from 'memory' to original storage.
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDM_INIT,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_MOVE_ALL_PTR_TO_MEM, INST_SINGLE,
                                             move_all_ptr_to_mem, GEN_DBAL_FIELD_LAST_MARK));
            /*
             * Return all BDBs from their originally allocated positions.
             * Restart allocation and enqueuing of BDBs.
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE,
                                             release_all_bdbs, GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                            (unit, DBAL_TABLE_BDB_CONFIG, 0, 1, GEN_DBAL_FIELD32, DBAL_FIELD_BDB_ALLOCATE_EN,
                             INST_SINGLE, enable_allocate, GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                            (unit, DBAL_TABLE_DQM_CONTROL, 0, 1, GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_EN, INST_SINGLE,
                             enable_enqueue, GEN_DBAL_FIELD_LAST_MARK));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_quarantine_init(
    int unit)
{
    char *deleted_buffers_file;
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);
    uint32 enabled_drams_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    FILE *p_file = NULL;
    uint32 buffer_to_mark_for_deleting;
    uint32 core;
    uint8 value_found;
    uint8 mutex_is_mine = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** if no drams are available, get out */
    if (enabled_drams_bitmap == 0)
    {
        SHR_EXIT();
    }

    /** if the threshold is 0, the quarantine mechanism is not used */
    if (buffer_delete_threshold == 0)
    {
        SHR_EXIT();
    }

    /** set delete reasons */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_delete_reasons_set(unit));

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.take(unit, sal_mutex_FOREVER));
    mutex_is_mine = 1;

    /** take file path from dnx data */
    deleted_buffers_file = dnx_data_dram.buffers.deleted_buffers_info_get(unit)->deleted_buffers_file;

    /** open file with read mode */
    p_file = sal_fopen(deleted_buffers_file, "r");
    if (p_file == NULL)
    {
        /** file doesn't exist - no buffers to mark for deletion during init */
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, TRUE));
    /*
     * read from file next buffer to mark for deletion -
     * format should be "buffer_0 core buffer_1 core ... buffer_n core"
     */
    for (;;)
    {
        /** read token from file */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_token_read
                        (unit, p_file, &buffer_to_mark_for_deleting, &value_found));
        if (value_found == FALSE)
        {
            break;
        }
        /** read token from file */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_token_read(unit, p_file, &core, &value_found));
        if (value_found == FALSE)
        {
            break;
        }
        /*
         * Verify received buffer and core are in range,
         * If they are not, it might be a sign of a corrupted file
         */

        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_to_mark_for_deleting, enabled_drams_bitmap));
        DNXCMN_CORE_VALIDATE(unit, (int) core, FALSE);
        /*
         * Set number of errors in db to be delete threshold - this will cause a buffer deletion on first error.
         * Note that if buffer can be deleted by HW (See dnx_dram_buffers_initiated_deletion() below)
         * then there will be no 'first error' and the procedure below only remains to update
         * HW counter (for diagnostics/statistics).
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_set
                        (unit, core, buffer_to_mark_for_deleting, buffer_delete_threshold));
        /*
         * Delete buffer by HW operation (if this feature is enabled).
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_initiated_deletion(unit, core, buffer_to_mark_for_deleting));
        /*
         * Update internal SW counter for the number of deleted buffers (BDB pointers)
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_quarantined_buffers_update(unit));
    }
    SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, FALSE));

    

exit:
    /** close file if was opened */
    if (p_file != NULL)
    {
        sal_fclose(p_file);
    }
    /** give mutex */
    if (mutex_is_mine)
    {
        SHR_IF_ERR_CONT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dram_buffers_quarantine_buffer_to_file_write(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    FILE *p_file = NULL;
    char *deleted_buffers_file;
    SHR_FUNC_INIT_VARS(unit);

    /** take mutex */
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.take(unit, sal_mutex_FOREVER));

    /** take file path from dnx data */
    deleted_buffers_file = dnx_data_dram.buffers.deleted_buffers_info_get(unit)->deleted_buffers_file;

    /** open file with append mode */
    p_file = sal_fopen(deleted_buffers_file, "a");
    if (p_file == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to open file\n");
    }

    /** write to file buffer and core */
    if (sal_fprintf(p_file, "%u %u ", buffer, core) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to write to file\n");
    }

exit:
    /** close file if was opened */
    if (p_file != NULL)
    {
        sal_fclose(p_file);
    }
    /** give mutex */
    SHR_IF_ERR_CONT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update number of errors for the BDB, and delete it if necessary.
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - bdb
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * The buffer is always moved to quarantine fifo by HW. If we reached
 *     the threshold then we just need to update SW DB, otherwise we need
 *     to move the buffer out of the HW quarantine fifo.
 * \see
 *   * None
 */
static shr_error_e
dnx_dram_buffers_quarantine_buffer_nof_errors_update(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);
    uint32 nof_errors;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of errors of buffer */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_get(unit, core, buffer, &nof_errors));

    /** if nof_errors == threshold, the buffer was already written to file - so just need to update SW counter of deleted buffers */
    if (nof_errors == buffer_delete_threshold)
    {
        /*
         * Update internal SW counter for the number of deleted buffers (BDB pointers)
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_quarantined_buffers_update(unit));
        SHR_EXIT();
    }

    /** add current error to counter */
    ++nof_errors;
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_set(unit, core, buffer, nof_errors));

    /*
     * The HW always moves the problematic buffer to quarantine fifo.
     * In case new nof_errors == threshold, the buffer needs to be written to file and to
     * update SW counter of deleted buffers.
     * In case new nof_errors < threshold, we need to move the buffer out of the quarantine
     * fifo and back to usage.
     */
    if (nof_errors == buffer_delete_threshold)
    {
        /*
         * Update internal SW counter for the number of deleted buffers (BDB pointers)
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_quarantined_buffers_update(unit));
        /** write buffer to file */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_to_file_write(unit, core, buffer));
    }
    else
    {
        /** restore buffer */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_restore(unit, core, buffer));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dram_buffers_quarantine_fifo_not_empty_interrupt_handler(
    int unit,
    bcm_core_t core)
{
    uint32 buffer;
    uint32 buffer_is_valid;
    uint32 quarantine_reason;
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /** if the threshold is 0, the quarantine mechanism is not used */
    if (buffer_delete_threshold == 0)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%s(): Buffer Quarantine Mechanism is not active\n"), FUNCTION_NAME()));
        SHR_EXIT();
    }

    /** as long as FIFO is not empty */
    do
    {
        /** read next in FIFO */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_read_buffer
                        (unit, core, &buffer, &buffer_is_valid, &quarantine_reason));
        /** if valid */
        if (buffer_is_valid)
        {
            /** Update nof errors and delete the buffer if needed */
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_nof_errors_update(unit, core, buffer));
        }
    }
    while (buffer_is_valid);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_dram_buffers_quarantine_ecc_error_interrupt_handler(
    int unit,
    uint32 hbc_block)
{
    uint32 entry_handle_id;
    uint32 hbm_index = hbc_block / dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 channel[1] = { hbc_block / dnx_data_dram.general_info.nof_channels_get(unit) };
    uint32 bank[1] = { 0 };
    uint32 row[1] = { 0 };
    uint32 column[1] = { 0 };
    uint32 logical_ptr[1] = { 0 };
    uint32 logical_buffer[1] = { 0 };
    uint32 bdb_index = 0;
    uint32 data_source_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Compute logical buffer from physical buffer
     */

    /*
     * Get physical buffer 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_LAST_ECC_ERROR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_BANK, INST_SINGLE, bank);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_ROW, INST_SINGLE, row);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_COLUMN, INST_SINGLE, column);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Convert physical buffer to logical_ptr */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0x3)
    {
        logical_ptr[0] |= SHR_IS_BITSET(row, 7) << 0;
        logical_ptr[0] |= SHR_IS_BITSET(row, 8) << 1;
        logical_ptr[0] |= SHR_IS_BITSET(row, 9) << 2;
        logical_ptr[0] |= SHR_IS_BITSET(row, 10) << 3;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 8) ^ SHR_IS_BITSET(bank, 2)) << 4;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 7) ^ hbm_index ^ SHR_IS_BITSET(column, 4)) << 5;
        logical_ptr[0] |= SHR_IS_BITSET(column, 5) << 6;
        logical_ptr[0] |= SHR_IS_BITSET(row, 0) << 7;
        logical_ptr[0] |= SHR_IS_BITSET(row, 1) << 8;
        logical_ptr[0] |= SHR_IS_BITSET(row, 2) << 9;
        logical_ptr[0] |= SHR_IS_BITSET(row, 3) << 10;
        logical_ptr[0] |= SHR_IS_BITSET(row, 4) << 11;
        logical_ptr[0] |= SHR_IS_BITSET(row, 5) << 12;
        logical_ptr[0] |= SHR_IS_BITSET(row, 6) << 13;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 10) ^ SHR_IS_BITSET(channel, 2)) << 14;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 10) ^ SHR_IS_BITSET(bank, 1)) << 15;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 9) ^ SHR_IS_BITSET(bank, 0)) << 16;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 7) ^ SHR_IS_BITSET(bank, 3)) << 17;
        logical_ptr[0] |= SHR_IS_BITSET(row, 11) << 18;
        logical_ptr[0] |= SHR_IS_BITSET(row, 12) << 19;
        logical_ptr[0] |= SHR_IS_BITSET(row, 13) << 20;
        logical_ptr[0] |= SHR_IS_BITSET(column, 3) << 21;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 8) ^ SHR_IS_BITSET(channel, 0)) << 22;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 9) ^ SHR_IS_BITSET(channel, 1)) << 23;
        logical_ptr[0] |= SHR_IS_BITSET(column, 4) << 24;
    }

    /** Extract logical_buffer from logical_ptr */
    SHR_BITCOPY_RANGE(logical_buffer, 0, logical_ptr, 4, 20);;
    /** Get BDB from logical buffer */
    bdb_index = logical_buffer[0] / 8;
    /** Get data_source_id from logical_ptr */
    data_source_id = SHR_IS_BITSET(logical_ptr, 24);

    /** Update nof errors for this BDB and delete it if needed */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_nof_errors_update(unit, data_source_id, bdb_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(
    int unit)
{
    uint32 buffers_array[DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE];
    int size;
    int core = 0;
    int index = 0;
    int is_dram_initialized = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** If no drams are available, get out */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        SHR_EXIT();
    }

    /** If dram is not yet initialized, get out */
    SHR_IF_ERR_EXIT(dnx_dram_db.is_initialized.get(unit, &is_dram_initialized));
    if (!is_dram_initialized)
    {
        SHR_EXIT();
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, TRUE));

        /** iterate on cores */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            /** clear array */
            sal_memset(buffers_array, 0, sizeof(buffers_array));
            /** get status for deleted or quarantined buffers according to input */
            SHR_IF_ERR_EXIT(dnx_dram_buffers_status_get
                            (unit, core, TRUE, buffers_array, DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE, &size));

            /** print overflow warning if buffers_array was too small */
            if (size > DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE)
            {
                /** print overflow warning */
                LOG_CLI((BSL_META("Too many buffers were deleted on core %d, deleting %d out of %d that were found\n"),
                         core, DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE, size));
                /** use MAX_SIZE instead of size when iterating array */
                size = DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE;
            }

            /** add buffers to deleted buffers list */
            for (index = 0; index < size; ++index)
            {
                /*
                 * Delete buffer by HW operation
                 */
                SHR_IF_ERR_EXIT(dnx_dram_buffers_initiated_deletion(unit, core, buffers_array[index]));
            }
        }

        SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}
