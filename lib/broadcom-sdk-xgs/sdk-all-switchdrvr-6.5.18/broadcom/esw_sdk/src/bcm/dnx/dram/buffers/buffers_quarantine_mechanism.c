/** \file src/bcm/dnx/dram/buffers/buffers_quarantine_mechanism.c
 *
 *
 *  This file contains implementation of functions for buffers quarantine mechanism
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
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

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "buffers_quarantine_mechanism.h"
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <sal/appl/io.h>
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
    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_read_buffer_verify
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
 * \brief - delete buffer read from quarantine fifo forever (until reboot), buffer should be a valid
 *          buffer returned by dnx_dram_buffers_quarantine_read_buffer function, this deletion works
 *          only if the buffer was read from the quarantine FIFO
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - buffer to delete
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
dnx_dram_buffers_quarantined_buffer_delete(
    int unit,
    bcm_core_t core,
    uint32 buffer)
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

    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_nof_deleted_buffers_get_verify(unit, nof_deleted_buffers));
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
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_nof_free_buffers_get_verify(unit, nof_free_buffers));
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
    for (int core = 0; core < nof_cores; ++core)
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
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    for (int core = 0; core < nof_cores; ++core)
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

/** check buffer is within valid range */
static shr_error_e
dnx_dram_buffers_buffer_verify(
    int unit,
    uint32 buffer)
{
    uint32 nof_bdbs = dnx_data_dram.buffers.nof_bdbs_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    if (buffer >= nof_bdbs)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Received buffer (%d) is out of range. Should be smaller than %d\n", buffer, nof_bdbs);
    }
exit:
    SHR_FUNC_EXIT;
}

/** check core is within valid range */
static shr_error_e
dnx_dram_buffers_core_verify(
    int unit,
    uint32 core)
{
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    if (core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Received core (%d) is out of range. Should be smaller than %d\n", core, nof_cores);
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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, nof_errors);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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

    SHR_FUNC_INIT_VARS(unit);
    *token_found = FALSE;
    while ((token[index] = (char) sal_fgetc(p_file)) > 0)
    {
        if (!_shr_isxdigit(token[index]))
        {
            if (index > 0)
            {
                *token_found = TRUE;
            }
            break;
        }

        if (index == (TOKEN_MAX_SIZE - 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "token is too big\n");
        }
        ++index;
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
 *   Also, update corresponding counters.
 * \param [in] unit -
 *   Unit HW id
 * \param [in] core -
 *   bcm_core_t. The core to which the buffer is attached. Must be
 *   smaller than dnx_data_device.general.nof_cores
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

        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_identifier));
        SHR_IF_ERR_EXIT(dnx_dram_buffers_core_verify(unit, core));
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
        /*
         * Update internal SW counter for the number of deleted buffers (BDB pointrs)
         */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer_identifier));
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
    uint32 nof_cores;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
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
            for (core = 0; core < nof_cores; core++)
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
    FILE *p_file = NULL;
    uint32 buffer_to_mark_for_deleting;
    uint32 core;
    uint8 value_found;
    uint8 mutex_is_mine = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** if no drams are available, get out */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
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
        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_to_mark_for_deleting));
        SHR_IF_ERR_EXIT(dnx_dram_buffers_core_verify(unit, core));
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
            uint32 nof_errors;
            /** get number of errors of buffer */
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_get(unit, core, buffer, &nof_errors));
            /** if nof_errors == threshold, the buffer was already written to file - so just need to delete it */
            if (nof_errors == buffer_delete_threshold)
            {
                /** delete buffer */
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer));
                continue;
            }

            /** add current error to counter */
            ++nof_errors;
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_set(unit, core, buffer, nof_errors));

            /** if new nof_errors == threshold, the buffer needs to be written to file and to be deleted */
            if (nof_errors == buffer_delete_threshold)
            {
                /** delete buffer */
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer));
                /** write buffer to file */
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_to_file_write(unit, core, buffer));
            }
            else
            {
                /** restore buffer */
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_restore(unit, core, buffer));
            }
        }
    }
    while (buffer_is_valid);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(
    int unit)
{
    uint32 buffers_array[DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE];
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    int size;
    int core = 0;
    int index = 0;
    uint8 is_dram_db_inited = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** If no drams are available, get out */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        SHR_EXIT();
    }

    /** If dram is not yet initialized, get out */
    SHR_IF_ERR_EXIT(dnx_dram_db.is_init(unit, &is_dram_db_inited));
    if (!is_dram_db_inited)
    {
        SHR_EXIT();
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, TRUE));

        /** iterate on cores */
        for (core = 0; core < nof_cores; ++core)
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
