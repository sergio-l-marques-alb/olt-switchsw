/** \file adapter_mdb_access.c
 * This file contains the logical MDB access functions.
 *
 * $Id:$
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

int
tmp_mdb_workaround_func(
    void)
{
    return 0;
}
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX

#ifdef ADAPTER_SERVER_MODE
/*
 * {
 */
#include <netdb.h>
/*
 * }
 */

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <bde/pli/verinet.h>
#include <errno.h>
#include <sys/socket.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Opcode used for MDB logical address */
#define UPDATE_FRM_SOC_LOGICAL_ACCESS   145

/** Different MDB logical Memory types */
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_DIRECT  180
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EM   181
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EEDB 182
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_LPM  183

/** Different MDB logical commands */
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_READ          160
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE         161
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_LOOKUP        162
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE        163
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_READ_RESPONSE 164

/** Maximal buffer size to communicate with the adapter in uint8 */
#define ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8 600

/** adapter MDB logical stub error codes */
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_TABLE_ID (-1)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_BLOCK_ID (-2)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_COMMAND  (-3)
#define LOGICAL_MEM_ACCESS_ERR_ILLEGAL_KEY      (-4)
#define LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND    (-5)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_ERR      (-6)

/*
 * }
 */

/*
 * {
 */

/**
 * \brief - Copy from a uint32 buffer to a uint8 buffer in the order the adapter expects.
 * LSB byte first.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] src - the src uint32 buffer
 *   \param [in] dst - the dst uint8 buffer
 *   \param [in] nof_bytes - the number of bytes to copy
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
adapter_mdb_access_uint32_to_uint8(
    int unit,
    uint32 *src,
    uint8 *dst,
    uint32 nof_bytes)
{
    uint32 byte_counter;
    uint8 u32_u8_factor;

    SHR_FUNC_INIT_VARS(unit);

    u32_u8_factor = sizeof(uint32) / sizeof(uint8);

    for (byte_counter = 0; byte_counter < nof_bytes; byte_counter++)
    {
        /*
         * Place the MSbyte of the uint32 array in the LSbyte of the uint8 array
         */
        int uint32_index = (nof_bytes - 1 - byte_counter) / u32_u8_factor;
        int uint32_offset = (((u32_u8_factor - 1 - byte_counter) % u32_u8_factor)) * 8;
        dst[byte_counter] = (src[uint32_index] >> uint32_offset) & 0xFF;
    }

    SHR_FUNC_EXIT;
}

/**
 * Write everything to the buffer
 * This buffer configuration is shared between LPM write/delete/read
 *
 * write_index is used both for input and outputs
 */
shr_error_e
adapter_mdb_access_lpm_prep_buffer(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 nw_order_int;
    int key_size_bits;
    int cmic_blk;
    int total_length;
    int write_index;
    /*
     * Zero pad up to uint32
     */
    int padding_length;
    int padding_iter;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Always use max key size, the prefix length controls the per entry variation
     */
    key_size_bits = dnx_data_mdb.kaps.key_width_in_bits_get(unit);

    write_index = *write_index_outer;

    /*
     * Total length includes the total packet_size
     */
    total_length = sizeof(uint8) + sizeof(uint8) + sizeof(uint8) + sizeof(uint32) + sizeof(uint8) + sizeof(uint8) +
        sizeof(uint32) + (sizeof(uint8) * BITS2BYTES(key_size_bits));

    /*
     * Writes also include a payload (20bits), which is written as uint32
     */
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        total_length += sizeof(uint32);
    }

    padding_length = sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    /*
     * Write opcode
     */
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += sizeof(char);

    /*
     * Write packet length.
     */
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(uint32);

    /*
     * SW block representation to cmic HW block representation
     */
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    /*
     * Write Block ID
     */
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    /*
     * Write Memory type
     */
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_LPM;
    write_index += sizeof(char);

    /*
     * Write command
     */
    buffer[write_index] = command;
    write_index += sizeof(char);

    /*
     * Write Memory ID
     */
    buffer[write_index] = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->memory_id;
    write_index += sizeof(char);

    /*
     * Write app_id
     */
    buffer[write_index] = app_id;
    write_index += sizeof(char);

    /*
     * Write key prefix length
     */
    buffer[write_index] = entry->prefix_length;
    write_index += sizeof(char);

    /*
     * Write key size in bits
     */
    nw_order_int = htonl(key_size_bits);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    /*
     * Write key
     */
    SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8
                    (unit, entry->key, &buffer[write_index], BITS2BYTES(key_size_bits)));
    write_index += sizeof(uint8) * BITS2BYTES(key_size_bits);

    /*
     * Write payload
     */
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        nw_order_int = htonl(entry->payload[0]);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += sizeof(nw_order_int);
    }

    /*
     * Write zero padding
     */
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See H file.
 */
shr_error_e
adapter_mdb_access_lpm_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    /*
     * Write to the buffer
     */
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                     &write_index, buffer));

    /** Write the packet to the socket */
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

exit:
    adapter_context_db.adapter_mem_reg_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * See H file.
 */
shr_error_e
adapter_mdb_access_lpm_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    /*
     * Write to the buffer
     */
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                     &write_index, buffer));

    /** Write the packet to the socket */
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

exit:
    adapter_context_db.adapter_mem_reg_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * See H file.
 */
shr_error_e
adapter_mdb_access_lpm_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;
    fd_set read_vect;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    /*
     * Write to the buffer
     */
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                     &write_index, buffer));

    /** Write the packet to the socket */
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    /*
     * Wait for the response
     */
    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * Interrupted by a signal such as a GPROF alarm so
                 * restart the call
                 */
                continue;
            }
            perror("get_command: select error");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            break;
        }
    }

    /*
     * Data ready to be read.
     */
    if (FD_ISSET(adapter_mem_reg_fd, &read_vect))
    {
        int read_length = 0;
        uint32 nw_order_int;
        uint8 key_size_u8;
        uint32 expected_response_size;
        int8 err_id;

        sal_memset(buffer, 0x0, sizeof(buffer));

        /*
         * Always use max key size, the prefix length controls the per entry variation
         */
        key_size_u8 = BITS2BYTES(dnx_data_mdb.kaps.key_width_in_bits_get(unit));

        /*
         * expected_response_size includes the same as lpm_write, just with error at the end:
         * ... |
         * key size (uint32)  | key ~~~  | payload (uint32) |Error (uint8) |
         *
         * Also includes a header of 5 bytes: opcode UPDATE_FRM_SOC_LOGICAL_ACCESS and additional fields
         *
         */
        expected_response_size = 5 * sizeof(uint8) +
            sizeof(uint32) + sizeof(uint8) + sizeof(uint8) + sizeof(uint8) + sizeof(uint8) + sizeof(uint8) +
            sizeof(uint32) + (sizeof(uint8) * key_size_u8) + sizeof(uint32) + sizeof(uint8);

        read_length = readn(adapter_mem_reg_fd, &(buffer[0]), expected_response_size);

        /*
         * Read the length of the packet
         */
        if (read_length < expected_response_size)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "adapter_read_buffer: could not read response packet up to err\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /*
         * Read Error (uint8)
         */
        err_id = (int8) buffer[expected_response_size - sizeof(uint8)];

        if (err_id != 0)
        {
            if (err_id == LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND)
            {
                /*
                 * Exit without error print
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter LPM read returned an error (%d)\n"), err_id));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
            SHR_EXIT();
        }

        /*
         * Read payload (uint32)
         */
        sal_memcpy(&nw_order_int, &buffer[expected_response_size - sizeof(uint8) - sizeof(uint32)], sizeof(uint32));
        entry->payload[0] = ntohl(nw_order_int);
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        /*
         * Time expire with no command
         */
        SHR_EXIT();
    }

exit:
    adapter_context_db.adapter_mem_reg_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/** ADAPTER_SERVER_MODE */
#endif
