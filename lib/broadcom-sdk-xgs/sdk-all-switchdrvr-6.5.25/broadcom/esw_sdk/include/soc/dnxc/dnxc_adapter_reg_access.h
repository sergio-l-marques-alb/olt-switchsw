/** \file dnxc_adapter_reg_access.h
 * This file handles the communication with the adapter's server
 */
/*
 * $Id:adapter_reg_access.h,v 1.312 Broadcom SDK $                                                           $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

#ifndef DNXC_ADAPTER_REG_ACCESS_H_INCLUDED
/* { */
#define DNXC_ADAPTER_REG_ACCESS_H_INCLUDED

#ifdef ADAPTER_SERVER_MODE

/*
 * Include files
 * {
 */

#include <include/soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

/** Maximum packet size in adapter in bits calculated based on CDMAC_JUMBO_MAXSZ plus additional buffer for signals */
#define MAX_PACKET_SIZE_ADAPTER (16360*8 + 100)

#define MAX_SIGNAL_SIZE 256
/** Maximum number of packets that can be processed in parallel */
#define MAX_PACKETS_IN_PARALLEL 10
/** Adapter opcode value for reading from register */
#define UPDATE_TO_SOC_REG_READ 0
/** Adapter opcode value for reading from memory */
#define UPDATE_TO_SOC_MEM_READ 1
/** Adapter opcode value for writing to register */
#define UPDATE_TO_SOC_REG_WRITE 4
/** Adapter opcode value for writing to memory */
#define UPDATE_TO_SOC_MEM_WRITE 5

/** Constant response header: length in bytes (4B) | opcode (1B) | MS name length (4B) | unit test ID (4B) | unit test result (4B) | response message length (4B) */
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE (21)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE (65)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE (257)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_HEADER_SIZE \
    (ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE + ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE + ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE)

/** Constant request header: opcode(1B) | unit_test_id (4B) | ms_name_length (4B) */
#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_CONSTANT_HEADER_SIZE (9)
#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_MS_NAME_SIZE (65)
#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_HEADER_SIZE (ADAPTER_MS_RUN_UNIT_TEST_REQUEST_CONSTANT_HEADER_SIZE + ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_MS_NAME_SIZE)
/*
 * }
 */

/*
 * Enum of the MS IDs in the adapter
 */
typedef enum
{
    /** Loopback module id in adapter */
    ADAPTER_MS_ID_LOOPBACK = -3,
    ADAPTER_MS_ID_FIRST_MS = -1,
    /** Module id for terminating the adapter`s server */
    ADAPTER_MS_ID_TERMINATE_SERVER = -5
} adapter_ms_id_e;

typedef enum
{
    ADAPTER_RX_TX,
    ADAPTER_REGS,
    ADAPTER_SIGNALS,
    ADAPTER_EVENTS
} socket_target_e;

extern int pipe_fds[2];

/**
 * \brief - Register get 32 bits
 *
 * DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the arrayindex of the reg/memory in the arrayindex of the reg/memory in the array
 *   \param [in] data - value of the register/memory value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);
/**
 * \brief - Register get 64 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the arrayindex of the reg/memory in the arrayindex of the reg/memory in the array
 *   \param [in] data - value of the register/memory value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data);
/**
 * \brief - Register get above 64 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg_above64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);
/**
 * \brief - Register set 32 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);
/**
 * \brief - Register set 64 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data);
/**
 * \brief - Register set above 64 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg_above64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);
/**
 * \brief - Read the data of a memory array
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] mem - memory ID
 *   \param [in] array_index - index of the memory in the array
 *   \param [in] copyno - core ID
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] entry_data - data of the memory's entry
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);
/**
 * \brief - Write data to a memory array
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] mem - memory ID
 *   \param [in] array_index - index of the memory in the array
 *   \param [in] copyno - core ID
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] entry_data - data of the memory's entry
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);

/**
 * \brief - Get signals from the adapter server
 *
 * Packet request format: | opcode (1 byte) | MS_ID (32 bits)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] ms_id - Block id
 *   \param [in,out] signal_data - Allocated buffer for adapter signal data
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] signal_data_length - Number of bytes in signal_buffer
 *   See signal_data above - Allocated buffer for adapter signal data
 *   signal_data format: nof_signals (32 bits) | (signal_name_length (32 bits) | signal_name | signal_data_length in
 *   bytes (32 bits) | signal_data)*
 *   All the numbers in signal_data arrive in NW order and should be converted using ntohl.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e adapter_get_signal(
    int unit,
    int core,
    uint32 ms_id,
    uint32 *signal_data_length,
    char **signal_data);

/**
 * \brief - This function sends messages
 *        (packets/signals/events) to the desired adapter socket.
 *
 * \param [in] unit - unit id.
 * \param [in] ms_id - Block id.
 * \param [in] target - The socket to use (ADAPTER_
 *        RX_TX/REGS/SIGNALS/EVENTS).
 * \param [in] len - Buffer length in bytes.
 * \param [in] num_of_signals - number of signals (relevant only
 *        for ADAPTER_SIGNALS target).
 * \param [in] buf - The message to be sent (data only). Format
 *        is ASCII - Each bit (0/1) is represented as a char - 1
 *        byte.
 *
 * \return int - 0 no error, otherwise error occured
 */
int adapter_write_buffer_to_socket(
    int unit,
    adapter_ms_id_e ms_id,
    socket_target_e target,
    int len,
    int num_of_signals,
    uint8 *buf);

/**
 * \brief - Read the hit bit indications in adapter
 *
 * Packet request format: |opcode(1B) | length (32 bits - 0s)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] read_and_delete - Parameter that tells if to delete the hit bit indication record after reading it or
 *          just to read it. '1' means read and delete, '0' means read without deleting it.
 *   \param [in,out] record_length - Number of bytes in the hit bits indication buffer
 *   \param [in,out] recorded_data - Dynamically allocated buffer inside adapter_read_hit_bits for hit bit indications. It
 *          should be freed by the user.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   See record_length - Number of bytes in the hit bits indication buffer
 *   See recorded_data above - Allocated buffer for hit bit indications
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_read_hit_bits(
    int unit,
    int read_and_delete,
    uint32 *record_length,
    char **recorded_data);

/**
 * \brief - Get table data from the control plane according the
 *         table name, block name and entry index
 *
 * Packet request format: | opcode(1B) | length (4B - 0s)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] tbl_name - table name as a string
 *   \param [in] blk_name - block name as a string
 *   \param [in] entry_index - index of the entry in the table
 *   \param [in] tbl_entry_data_allocated_size - number of bytes allocated in tbl_entry_data
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] tbl_entry_size - Number of bytes in tbl_entry_data. Filled by control_get_table_data
 *   \param [out] tbl_entry_data - Buffer allocated by the callder for the table entry results
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int control_get_table_data(
    int unit,
    char *tbl_name,
    char *blk_name,
    int entry_index,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data);

/**
 * \brief - Execute a module simulation unit test from the control plane according the
 *          MS name and Unit test ID
 *
 * Packet request format: | opcode(1B) | unit_test_id (4B) | ms_name_length (4B) | ms_name (ms_name_length bytes)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] ms_name - The name of the MS
 *   \param [in] unit_test_id - unit_test_id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] unit_test_result - the result of the unit test execution - PASS/FAIL
 *   \param [out] message - response message from the unit test execution (optional)
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_ms_run_unit_test(
    int unit,
    char *ms_name,
    uint32 unit_test_id,
    uint32 *unit_test_result,
    char message[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE]);

/**
 * \brief - Read the block response from the adapter server
 *
 * Packet response format:
 * length in bytes from opcode (32 bits) | opcode (1 byte) | nof_blocks (32 bits)| (MS_ID (32 bits) |
 * ms_name_length | ms_name)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in,out] block_names - Allocated buffer for adapter block names
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] block_names_length - Returned adapter block names length
 *   See block_names above - Returned adapter block names
 * block_names format: nof_blocks (32 bits)| (MS_ID (32 bits) | ms_name_length (32 bits) | ms_name)*
 * All the numbers in block_names arrive in NW order and should be converted using ntohl.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_get_block_names(
    int unit,
    int *block_names_length,
    char **block_names);

/**
 * \brief - Register access init
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] sub_unit_id - sub unit index
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg_access_init(
    int unit,
    int sub_unit_id);
/**
 * \brief - Register access deinit
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] sub_unit_id - sub unit index
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_reg_access_deinit(
    int unit,
    int sub_unit_id);

/**
 * \brief - Disable the collecting of writes into one bulk write
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] sub_unit_id - sub unit index
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   uint8
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_disable_collect_accesses(
    int unit,
    int sub_unit_id);
/**
 * \brief - Executed when the initialization of the device is done
 *  Disable the collecting of writes into one bulk write
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   uint8
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_dnx_init_done(
    int unit);
/**
 * \brief - Reads a Tx buffer from the adapter's server.
 * Assumes that the buffer's length is a multiple of 4
 * Parameters: sockfd -socket file descriptor. Packet format:
 * length (4 bytes), block ID (4 bytes), NOF signals (4 bytes),
 * [signal ID (4 bytes), signal length (4 bytes), data]
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] sub_unit_id - sub unit id
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] nof_signals - number of signals
 *   \param [in] ifc_type - port type. (CPU/NIF/OLP/etc..)
 *   \param [in] port_channel - channel of the port
 *   \param [in] port_first_lane - port_first_lane that sent out the packet from sim to cpu, meaning adapter outgoing port.
 *                                 relevant if the ifc_type is NIF
  *   \param [in] len - length of the buffer parameter
 *   \param [in] buf - buffer of the rx packet
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   uint32
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 adapter_read_buffer(
    int unit,
    int sub_unit_id,
    adapter_ms_id_e * ms_id,
    uint32 *nof_signals,
    int *ifc_type,
    uint32 *port_channel,
    uint32 *port_first_lane,
    int *len,
    unsigned char *buf);
/**
 * \brief -  Client call: send packet to the adapter's server
 * unit - not in use
 * ms_id - ID of the block in the adapter
 * src_port - not in use
 * len - buf length in bytes
 * not_signals - number of signals in buf
 * Packet format: | length in bytes from ms_id(32 bits) | ms_id (32 bits) | nof_signals (32 bits) | SIGNALS (Signal id 32 bits | data length in bytes 32 bits | data )*
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] src_port - source port
 *   \param [in] src_channel - source channel
 *   \param [in] len - length of the buffer parameter
 *   \param [in] buf - buffer with the tx packet's data
 *   \param [in] nof_signals - number of signals
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   uint32
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    uint32 src_channel,
    int len,
    unsigned char *buf,
    int nof_signals);
/**
 * \brief - Send a command to the adapter. This function serves
 * read/write and memories/registers
 *
 * The format of the packet that is written to the socket is:
 * Opcode = 200 (1B)| overall_length (4B) | (opcode (1B) | data_length (4B) | Block ID (4 byte) | RTL Address (4 bytes)
 * | Data (variable length))*
 * overall_length includes only the fields after it. data_length - data length in bytes.
 *
 * The message's format to receive data from the adapter's server is:
 * GET:
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (4 byte) | RTL Address (4 bytes)
 * GET reply
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (4 byte) |
 * RTL Address (4 bytes) | Data (variable length)
 *
 * SET:
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (4 byte) |
 * RTL Address (4 bytes) | Data (variable length)
 *
 * Data Length includes only the field after it.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] cmic_block - block ID of the reg/memory
 *   \param [in] addr - addr of the reg/memory
 *   \param [in] data_length - length (in bytes) of the data param
 *   \param [in] is_mem - 0 for registers, 1 for memories
 *   \param [in] is_get - 0 for set, 1 for get
 *   \param [in] is_clear_on_read - clear on read if true
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int adapter_memreg_access(
    int unit,
    int cmic_block,
    uint32 addr,
    uint32 data_length,
    int is_mem,
    int is_get,
    int is_clear_on_read,
    uint32 *data);

/**
 * \brief - Terminate adapter`s server
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   uint32
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 adapter_terminate_server(
    int unit);

/**
 * \brief - wrapper for adapter_reg_access_init
 *
 */
shr_error_e dnxc_init_adapter_reg_access_init(
    int unit);

/**
 * \brief - wrapper for adapter_reg_access_deinit
 *
 */
shr_error_e dnxc_init_adapter_reg_access_deinit(
    int unit);

#endif

/**DNXC_ADAPTER_REG_ACCESS_H_INCLUDED*/
/* } */
#endif
