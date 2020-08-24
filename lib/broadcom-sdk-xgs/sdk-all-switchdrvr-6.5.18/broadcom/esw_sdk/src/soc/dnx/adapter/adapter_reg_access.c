/** \file adapter_reg_access.c
 * This file presents an example of replacing the registers/memories access functions. It
 * assumes using the portmod register DB bcm2801pb_a0
 */
/*
 * $Id:$
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

int
tmp_workaround_func(
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
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/swstate/auto_generated/types/adapter_types.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dcmn/error.h>

#include <netinet/tcp.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Default adapter server port */
#define DEFAULT_ADAPTER_SERVER_PORT 6816
/** Default adapter server address */
#define DEFAULT_ADAPTER_SERVER_ADDRESS "localhost"
/** Maximum host address length */
#define HOST_ADDRESS_MAX_LEN 40
/** Maximum packet header size in the adapter. */
#define MAX_PACKET_HEADER_SIZE_ADAPTER 30
/** Memory/Registers response header size in adapter */
#define MEM_RESPONSE_HEADER_SIZE_ADAPTER 13
/** Signal response header size in adapter */
#define SIGNALS_RESPONSE_LENGTH_SIZE 4
/** Constant header size for adapter signals */
#define ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE 9
/** Constant header size for adapter block names */
#define ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE 5
/** Minimum signals data size */
#define MIN_SIGNALS_DATA_SIZE 9
/** Maximum signals data size */
#define MAX_SIGNALS_DATA_SIZE 8192*2
/** Signals request header size */
#define SIGNALS_REQUEST_HEADER_SIZE 5
/** Hit indication header size */
#define ADAPTER_HIT_INDICATION_HEADER_SIZE 5
/** header size of table control get  */
#define ADAPTER_TBL_CONTROL_GET_HEADER_SIZE 5
/** Hit indication maximum packet size */
#define ADAPTER_HIT_INDICATION_MAX_PACKET_SIZE 500000
/** Signals all MS IDs */
#define SIGNALS_ALL_MS_ID -1
/** Signal ID for packet TX */
#define TX_PACKET_SIGNAL 3
/** Signal ID for PTC */
#define TX_PACKET_PTC_SIGNAL_ID 1
/** PTC signal width in bits */
#define TX_PACKET_PTC_SIGNAL_WIDTH 10
/** Signal ID for TAG_SWAP_RES */
#define TX_PACKET_TAG_SWAP_RES_SIGNAL_ID 6
/** TAG_SWAP_RES signal width in bits */
#define TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH 2
/** Signal ID for RECYCLE_COMMAND */
#define TX_PACKET_RECYCLE_COMMAND_SIGNAL_ID 4
/** RECYCLE_COMMAND signal width in bits */
#define TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH 16
/** Signal ID for VALID_BYTES */
#define TX_PACKET_VALID_BYTES_SIGNAL_ID 2
/** VALID_BYTES signal width in bits */
#define TX_PACKET_VALID_BYTES_SIGNAL_WIDTH 8

/** RX thread is about to be closed */
#define RX_THREAD_NOTIFY_CLOSED (-2)
/** Maximum size of access buffer */
#define MAX_ACCESS_BUFFER_SIZE 2048
/** Maximum size of one access entry */
#define MAX_SIZE_OF_ACCESS_ENTRY 100

/** Opcode values taken from the adapter's server */
/** Opcode value for write register */
#define UPDATE_FRM_SOC_WRITE_REG 128
/** Opcode value for write table */
#define UPDATE_FRM_SOC_WRITE_TBL 129

/** Opcode value for read register request */
#define UPDATE_FRM_SOC_READ_REG_REQ 133
/** Opcode value for read table request */
#define UPDATE_FRM_SOC_READ_TBL_REQ 134
/** Opcode value for read register reply */
#define UPDATE_TO_SOC_READ_REG_REP 8
/** Opcode value for read table reply */
#define UPDATE_TO_SOC_READ_TBL_REP 9

/** Opcode values for adapter hit indications */
/** Opcode value for start recording the hit indications  */
#define UPDATE_FRM_SOC_RCRD_ACC_ON 136
/** Opcode value for stop recording the hit indications */
#define UPDATE_FRM_SOC_RCRD_ACC_OFF 137
/** Opcode value for reading the hit bit indications */
#define UPDATE_FRM_SOC_READ_RCRD_ACC 138
/** Opcode value for reading the hit bit indications and deleting them after that */
#define UPDATE_FRM_SOC_READ_CLR_RCRD_ACC 139
/** Adapter opcode value for read record access */
#define UPDATE_TO_SOC_READ_RCRD_ACC 11
/** Opcode value for sending multiple writes together */
#define UPDATE_FRM_SOC_WRAPPER 200
#define INIT_PACKET_SIZE 8

/*
 * }
 */
const char *conversion_tbl[] = {
    "00000000",
    "00000001",
    "00000010",
    "00000011",
    "00000100",
    "00000101",
    "00000110",
    "00000111",
    "00001000",
    "00001001",
    "00001010",
    "00001011",
    "00001100",
    "00001101",
    "00001110",
    "00001111",
    "00010000",
    "00010001",
    "00010010",
    "00010011",
    "00010100",
    "00010101",
    "00010110",
    "00010111",
    "00011000",
    "00011001",
    "00011010",
    "00011011",
    "00011100",
    "00011101",
    "00011110",
    "00011111",
    "00100000",
    "00100001",
    "00100010",
    "00100011",
    "00100100",
    "00100101",
    "00100110",
    "00100111",
    "00101000",
    "00101001",
    "00101010",
    "00101011",
    "00101100",
    "00101101",
    "00101110",
    "00101111",
    "00110000",
    "00110001",
    "00110010",
    "00110011",
    "00110100",
    "00110101",
    "00110110",
    "00110111",
    "00111000",
    "00111001",
    "00111010",
    "00111011",
    "00111100",
    "00111101",
    "00111110",
    "00111111",
    "01000000",
    "01000001",
    "01000010",
    "01000011",
    "01000100",
    "01000101",
    "01000110",
    "01000111",
    "01001000",
    "01001001",
    "01001010",
    "01001011",
    "01001100",
    "01001101",
    "01001110",
    "01001111",
    "01010000",
    "01010001",
    "01010010",
    "01010011",
    "01010100",
    "01010101",
    "01010110",
    "01010111",
    "01011000",
    "01011001",
    "01011010",
    "01011011",
    "01011100",
    "01011101",
    "01011110",
    "01011111",
    "01100000",
    "01100001",
    "01100010",
    "01100011",
    "01100100",
    "01100101",
    "01100110",
    "01100111",
    "01101000",
    "01101001",
    "01101010",
    "01101011",
    "01101100",
    "01101101",
    "01101110",
    "01101111",
    "01110000",
    "01110001",
    "01110010",
    "01110011",
    "01110100",
    "01110101",
    "01110110",
    "01110111",
    "01111000",
    "01111001",
    "01111010",
    "01111011",
    "01111100",
    "01111101",
    "01111110",
    "01111111",
    "10000000",
    "10000001",
    "10000010",
    "10000011",
    "10000100",
    "10000101",
    "10000110",
    "10000111",
    "10001000",
    "10001001",
    "10001010",
    "10001011",
    "10001100",
    "10001101",
    "10001110",
    "10001111",
    "10010000",
    "10010001",
    "10010010",
    "10010011",
    "10010100",
    "10010101",
    "10010110",
    "10010111",
    "10011000",
    "10011001",
    "10011010",
    "10011011",
    "10011100",
    "10011101",
    "10011110",
    "10011111",
    "10100000",
    "10100001",
    "10100010",
    "10100011",
    "10100100",
    "10100101",
    "10100110",
    "10100111",
    "10101000",
    "10101001",
    "10101010",
    "10101011",
    "10101100",
    "10101101",
    "10101110",
    "10101111",
    "10110000",
    "10110001",
    "10110010",
    "10110011",
    "10110100",
    "10110101",
    "10110110",
    "10110111",
    "10111000",
    "10111001",
    "10111010",
    "10111011",
    "10111100",
    "10111101",
    "10111110",
    "10111111",
    "11000000",
    "11000001",
    "11000010",
    "11000011",
    "11000100",
    "11000101",
    "11000110",
    "11000111",
    "11001000",
    "11001001",
    "11001010",
    "11001011",
    "11001100",
    "11001101",
    "11001110",
    "11001111",
    "11010000",
    "11010001",
    "11010010",
    "11010011",
    "11010100",
    "11010101",
    "11010110",
    "11010111",
    "11011000",
    "11011001",
    "11011010",
    "11011011",
    "11011100",
    "11011101",
    "11011110",
    "11011111",
    "11100000",
    "11100001",
    "11100010",
    "11100011",
    "11100100",
    "11100101",
    "11100110",
    "11100111",
    "11101000",
    "11101001",
    "11101010",
    "11101011",
    "11101100",
    "11101101",
    "11101110",
    "11101111",
    "11110000",
    "11110001",
    "11110010",
    "11110011",
    "11110100",
    "11110101",
    "11110110",
    "11110111",
    "11111000",
    "11111001",
    "11111010",
    "11111011",
    "11111100",
    "11111101",
    "11111110",
    "11111111"
};

typedef enum
{
    ADAPTER_SIGNALS_REQUEST_OPCODE,
    ADAPTER_SIGNALS_RESPONSE_OPCODE,
    ADAPTER_BLOCK_NAMES_REQUEST_OPCODE,
    ADAPTER_BLOCK_NAMES_RESPONSE_OPCODE,
    ADAPTER_TBL_CONTROL_GET_REQUEST_OPCODE,
    ADAPTER_TBL_CONTROL_GET_RESPONSE_OPCODE
} adapter_opcode_e;

/**
 * \brief
 *  Context data of the adapter
 */
typedef struct
{
    /** Params used for collecting accesses */
    uint8 *p_access_buffer;
    uint8 *p_access_buffer_current_position;
    /** Determine if the writes have to be collected into one bulk write  */
    uint32 collect_adapter_accesses;

} adapter_access_info_t;

/**
 * \brief - context data of the adapter
 *
 */
adapter_access_info_t *adapter_info = { 0 };

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
 * \brief - Callbacks that support adapter. Registers using
 * soc_reg_access_func_register
 */
soc_reg_access_t adapter_access = {
    adapter_reg32_get,
    adapter_reg64_get,
    adapter_reg_above64_get,

    adapter_reg32_set,
    adapter_reg64_set,
    adapter_reg_above64_set,

    adapter_mem_array_read,
    adapter_mem_array_write
};

extern char *getenv(
    const char *);
extern int _soc_mem_write_copyno_update(
    int unit,
    soc_mem_t mem,
    int *copyno,
    int *copyno_override);

/**
 * \brief - Wait for the response from the adapter's server and read it from the socket
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] is_mem - 0 for registers, 1 for memories
 *   \param [in] data_len - length (in bytes) of the data parameter
 *   \param [in] data - value of the register/memory
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   soc_error_t
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static soc_error_t
adapter_memreg_read_response(
    int unit,
    int is_mem,
    uint32 data_len,
    uint32 *data)
{
    fd_set read_vect;
    char swapped_header[MEM_RESPONSE_HEADER_SIZE_ADAPTER];
    uint32 nw_order_int;
    uint32 rtl_address;
    int8 opcode;
    int block_id;
    int read_length = 0;
    uint32 actual_data_len;
    uint32 half_actual_data_len;
    int ii;
    char *data_char_ptr = (char *) data;
    int adapter_mem_reg_fd;

    /**Used to swap the data bytes */
    int tmp;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

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
        read_length = readn(adapter_mem_reg_fd, &(swapped_header[0]), MEM_RESPONSE_HEADER_SIZE_ADAPTER);

        /*
         * Read the length of the packet
         */
        if (read_length < MEM_RESPONSE_HEADER_SIZE_ADAPTER)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read packet length\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /*
         * Read opcode
         */
        opcode = *((int8 *) &(swapped_header[0]));

        /*
         * Read length
         */
        sal_memcpy(&nw_order_int, &swapped_header[1], sizeof(int));
        /*
         * actual_data_len is only the data length without the block id field and the RTL address field
         */
        actual_data_len = ntohl(nw_order_int) - 8;

        /*
         * Read block_id
         */
        sal_memcpy(&nw_order_int, &swapped_header[5], sizeof(int));
        block_id = ntohl(nw_order_int);

        /*
         * Read RTL address
         */
        sal_memcpy(&nw_order_int, &swapped_header[9], sizeof(int));
        rtl_address = ntohl(nw_order_int);

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("adapter_memreg_read_response: opcode=%d actual_data_len=%d block_id=%d rtl_address=%d\n"),
                  opcode, actual_data_len, block_id, rtl_address));

        read_length = readn(adapter_mem_reg_fd, data, actual_data_len);

        /*
         * Read the data field according to the length
         */
        
        if (read_length < actual_data_len)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_memreg_read_response: could not read data\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /*
         * The bytes should be swapped. The bytes are writen swapped and should be swapped again on the way from the c model's server
         */
        half_actual_data_len = actual_data_len >> 1;
        for (ii = 0; ii < half_actual_data_len; ii++)
        {
            tmp = data_char_ptr[ii];
            data_char_ptr[ii] = data_char_ptr[actual_data_len - 1 - ii];
            data_char_ptr[actual_data_len - 1 - ii] = tmp;
        }

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
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reads the signal response from the adapter server
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] signal_data - Allocated buffer for adapter signal data
 *
 * Packet response format:
 * length in bytes from opcode (32 bits) | opcode (1 byte) | MS_ID (32 bits) | nof_signals (32 bits) |
 * (signal_name_length (32 bits) | signal_name | signal_data_length in bytes (32 bits) | signal_data)*
 *
 * signal_data buffer includes:
 * nof_signals (32 bits) | (signal_name_length (32 bits) | signal_name | signal_data_length in bytes (32 bits) | signal_data)*
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] signal_data_length - Number of bytes in signal_buffer
 *   \param [out] signal_data - Allocated buffer for adapter signal data that should be freed by the user.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
adapter_signals_read_response(
    int unit,
    uint32 *signal_data_length,
    char **signal_data)
{
    fd_set read_vect;
    int packet_size;
    int8 opcode;
    uint32 ms_id;
    int read_constant_header = 0;
    char buffer[ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE];
    char *signal_data_dyn;
    int buffer_offset = 0;
    uint32 long_val;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * Interrupted by a signal such as a GPROF alarm so restart the call
                 */
                continue;
            }
            perror("get_command: select error");
            SHR_ERR_EXIT(_SHR_E_FAIL, "select error\n");
        }
        else
        {
            break;
        }
    }

    /*
     * Data ready to be read.
     */
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        /*
         * Read the constant header of the packet
         */
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE);

        if (read_constant_header < ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read packet size */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if ((packet_size < MIN_SIGNALS_DATA_SIZE) || (packet_size > MAX_SIGNALS_DATA_SIZE))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter signal invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read opcode */
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        /** Check opcode */
        if (opcode != ADAPTER_SIGNALS_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Wrong opcode in signals response message\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read ms_id */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        ms_id = long_val;
        buffer_offset += sizeof(ms_id);

        /*
         * The signal_data_length includes nof_signals (32 bits) | (signal_name_length (32 bits) | signal_name |
         * signal_data_length in bytes (32 bits) | signal_data)*
         */
        *signal_data_length = packet_size - buffer_offset + 4;
        signal_data_dyn = sal_alloc(*signal_data_length, "adapter signals buffer");
        readn(adapter_sdk_interface_fd, signal_data_dyn, *signal_data_length);
        *signal_data = signal_data_dyn;

    }
    else
    {
        /*
         * Time expire with no command
         */
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Timeout on signal request\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get signals from the adapter server
 *
 * Packet request format: | opcode (1 byte) | MS_ID (32 bits)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] ms_id - Block id
 *   \param [in] signal_data - Dynamically allocated buffer inside the adapter_get_signal for adapter signal data. It should be
 *          freed using sal_free().
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] signal_data_length - Number of bytes in signal_buffer
 *   \param [out] signal_data - Dynamically allocated buffer for adapter signal data
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
shr_error_e
adapter_get_signal(
    int unit,
    uint32 ms_id,
    uint32 *signal_data_length,
    char **signal_data)
{
    uint32 nw_order_int;
    int write_index = 0;
    uint32 host_order_int;
    char buffer[SIGNALS_REQUEST_HEADER_SIZE];
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_signals_write_request: unit=%d ms_id=%d \n"), unit, ms_id));

    adapter_context_db.adapter_sdk_interface_mutex.take(unit, sal_mutex_FOREVER);

    /** Add opcode (1B) to the buffer */
    buffer[write_index] = ADAPTER_SIGNALS_REQUEST_OPCODE;
    write_index += sizeof(char);

    /** Add MS ID (4 B) */
    host_order_int = ms_id;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(&(buffer[write_index]), &nw_order_int, sizeof(ms_id));
    write_index += sizeof(ms_id);

    /*
     * write the packet to the socket
     */
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_signals_write_request data failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
    /** Fetch the signals from the adapter */
    SHR_IF_ERR_EXIT(adapter_signals_read_response(unit, signal_data_length, signal_data));

exit:
    adapter_context_db.adapter_sdk_interface_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read the block response from the adapter server
 *
 * Packet response format:
 * length in bytes from opcode (32 bits) | opcode (1 byte) | nof_blocks (32 bits)| (MS_ID (32 bits) |
 * ms_name_length | ms_name)*
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] block_names - Dynamically allocated buffer for adapter block names and freed by the caller.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] block_names_length - Returned adapter block names length
 *   \param [out] block_names - Returned adapter block names
 * block_names format: nof_blocks (32 bits)| (MS_ID (32 bits) | ms_name_length (32 bits) | ms_name)*
 * All the numbers in block_names arrive in NW order and should be converted using ntohl.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
adapter_blocks_read_response(
    int unit,
    int *block_names_length,
    char **block_names)
{
    fd_set read_vect;
    int packet_size;
    int8 opcode;
    uint32 long_val;
    int buffer_offset = 0;
    char buffer[ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE];
    char *block_names_dyn;
    int read_constant_header = 0;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * Interrupted by a signal such as a GPROF alarm so restart the call
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
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE);
        /** Read the length of the packet */
        if (read_constant_header < ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read packet_size */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if ((packet_size <= MIN_SIGNALS_DATA_SIZE) || (packet_size > MAX_SIGNALS_DATA_SIZE))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter signal invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read opcode */
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != ADAPTER_BLOCK_NAMES_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter block_names wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** block_names_length includes nof_blocks (32 bits)| (MS_ID (32 bits) | ms_name_length (32 bits) | ms_name)* */
        *block_names_length = packet_size - buffer_offset + 4;
        block_names_dyn = sal_alloc(*block_names_length, "adapter block names buffer");
        readn(adapter_sdk_interface_fd, block_names_dyn, *block_names_length);
        *block_names = block_names_dyn;
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
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init the Rx Tx socket with SDK configuration. 
 * | Reserved (two bytes)  MUST be 0 | Con. Req. Ver. (one byte) | Con. Requester Type (one byte)  | Sync number (four bytes) |  
 *
 * \param [in] unit - unit id. 
 *
 * \return status - 0 no error, otherwise error occured
 */
uint32
adapter_init_rx_tx_socket(
    int unit)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    char packet_data[INIT_PACKET_SIZE];
    int adapter_rx_tx_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_rx_tx_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_rx_tx_fd.get(unit, &adapter_rx_tx_fd);

    /** Reserved two byte = 0 */
    packet_data[index_position++] = 0;
    packet_data[index_position++] = 0;

    /** Connection request version */
    packet_data[index_position++] = dnx_data_adapter.general.lib_ver_get(unit);

    /** Connection request type (CPU==2)   */
    packet_data[index_position++] = 2;

    /** Sync number (should be -1) */
    nw_order_int = htonl(-1);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /*
     * write to socket
     */
    if (writen(adapter_rx_tx_fd, packet_data, index_position) != index_position)
    {
        adapter_context_db.adapter_rx_tx_mutex.give(unit);
        SHR_ERR_EXIT(_SHR_E_PORT, "Error: adapter_init_rx_tx_socket failed\n");
    }

    adapter_context_db.adapter_rx_tx_mutex.give(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adapter get block names
 *
 * Packet request format: opcode (1B)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] block_names_length -  Length of the adapter block names
 *   \param [in] block_names - Dynamicallly allocated buffer inside adapter_get_block_names for adapter block names. It should be
 *          freed using sal_free().
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
int
adapter_get_block_names(
    int unit,
    int *block_names_length,
    char **block_names)
{
    int write_index = 0;
    char buffer[SIGNALS_REQUEST_HEADER_SIZE];
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_sdk_interface_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    /** Add opcode (1B) to the buffer */
    buffer[write_index] = ADAPTER_BLOCK_NAMES_REQUEST_OPCODE;
    write_index += sizeof(char);

    /** write the packet to the socket */
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_get_block_names writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
    /** Read the block names from the adapter */
    adapter_blocks_read_response(unit, block_names_length, block_names);

exit:
    adapter_context_db.adapter_sdk_interface_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read the hit bit indications with/or without deleting them after that
 *
 * Response packet format: opcode(1B) | length (4B) | (opcode(1B) | blk_id(4B) | rtl_addr(4B) | offset(4B))*
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] record_length - Number of bytes in the hit bits indication buffer
 *   \param [in] recorded_data - Allocated buffer for hit bit indications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] record_length - Number of bytes in the hit bits indication buffer
 *   \param [out] recorded_data - Dynamically allocated buffer for hit bit indications. It should be freed by the user.
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
adapter_read_hit_bits_response(
    int unit,
    uint32 *record_length,
    char **recorded_data)
{
    fd_set read_vect;
    uint32 packet_size;
    char opcode;
    uint32 long_val;
    int buffer_offset = 0;
    uint8 buffer[ADAPTER_HIT_INDICATION_HEADER_SIZE];
    char *record_dyn;
    int read_constant_header = 0;
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * Interrupted by a signal such as a GPROF alarm so restart the call
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
        /** Read the constant header of the packet */
        read_constant_header = readn(adapter_mem_reg_fd, buffer, ADAPTER_HIT_INDICATION_HEADER_SIZE);

        if (read_constant_header < ADAPTER_HIT_INDICATION_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "adapter_read_hit_indication_record: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read opcode */
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != UPDATE_TO_SOC_READ_RCRD_ACC)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter read_record wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read packet_size */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if (packet_size < 0 || packet_size > ADAPTER_HIT_INDICATION_MAX_PACKET_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter hit indication invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** record_length includes (opcode(1B), blk_id(4B), rtl_addr(4B), offset(4B))* */
        *record_length = packet_size;
        record_dyn = sal_alloc(*record_length, "adapter hit indications buffer");
        readn(adapter_mem_reg_fd, record_dyn, *record_length);
        *recorded_data = record_dyn;
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
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read the hit bit indications in adapter
 *
 * Packet request format: | opcode(1B) | length (4B - 0s)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] read_and_delete - Clear on read parameter. '1' means read and delete, '0' means read without deleting.
 *
 *   \param [in] record_length - Number of bytes in the hit bits indication buffer
 *   \param [in] recorded_data - Dynamically allocated buffer inside adapter_read_hit_bits for hit bit indications. It
 *          should be freed by the user.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] record_length - Number of bytes in the hit bits indication buffer
 *   \param [out] recorded_data - Allocated buffer for hit bit indications
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
adapter_read_hit_bits(
    int unit,
    int read_and_delete,
    uint32 *record_length,
    char **recorded_data)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_HIT_INDICATION_HEADER_SIZE];
    uint32 length = 0;
    uint32 nw_order_int;
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_mem_reg_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    if (read_and_delete == 1)
    {
        /** Add opcode (1B) to the buffer for read and clear access record */
        buffer[write_index] = UPDATE_FRM_SOC_READ_CLR_RCRD_ACC;
    }
    else
    {
        /** Add opcode (1B) to the buffer for read access record without deleting it */
        buffer[write_index] = UPDATE_FRM_SOC_READ_RCRD_ACC;
    }

    write_index += sizeof(char);

    /** Add length (4B) that is 0  */
    nw_order_int = htonl(length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(length));
    write_index += sizeof(length);

    /** Write the packet to the socket */
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_read_record: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    /** Read the recorded hit indications from the adapter */
    adapter_read_hit_bits_response(unit, record_length, recorded_data);

exit:
    adapter_context_db.adapter_mem_reg_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Handle the get table data response
 * Packet format: opcode (1 byte) | data size in bytes (4 bytes) | data |
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
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
int
control_get_table_data_response(
    int unit,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data)
{
    fd_set read_vect;
    int data_size;
    int8 opcode;
    int read_constant_header = 0;
    char buffer[ADAPTER_TBL_CONTROL_GET_HEADER_SIZE];
    int buffer_offset = 0;
    uint32 long_val;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * Interrupted by a signal such as a GPROF alarm so restart the call
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
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        /*
         * Read the constant header of the packet
         */
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_TBL_CONTROL_GET_HEADER_SIZE);

        if (read_constant_header < ADAPTER_TBL_CONTROL_GET_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                  "control_get_table_data_response: could not read the constant header\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read opcode */
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != ADAPTER_TBL_CONTROL_GET_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter control_get_table_data_response wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read data size */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        data_size = long_val;
        buffer_offset += sizeof(data_size);

        if (data_size < 0 || data_size > tbl_entry_data_allocated_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter control_get_table_data_response invalid pkt size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read table data */
        readn(adapter_sdk_interface_fd, (void *) tbl_entry_data, data_size);
        *tbl_entry_size = data_size;
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
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get table data from the control plane according the
 *         table name, block name and entry index
 *
 * Packet request format: opcode (1 byte) | packet size (4 bytes) | block name length (4 bytes) | block name
 *                     table name  length (4 bytes) | table name | address (4 bytes)
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
int
control_get_table_data(
    int unit,
    char *tbl_name,
    char *blk_name,
    int entry_index,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data)
{
    int block_name_length;
    int table_name_length;
    int write_index = 0;
    char buffer[500];
    uint32 nw_order_int;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    block_name_length = sal_strlen(blk_name);
    table_name_length = sal_strlen(tbl_name);

    adapter_context_db.adapter_sdk_interface_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);

    buffer[write_index] = ADAPTER_TBL_CONTROL_GET_REQUEST_OPCODE;
    write_index += sizeof(char);

    /*
     * Write packet length. Names length + 2*name length size + sizeof(address) 
     */
    nw_order_int = htonl(block_name_length + table_name_length + 3 * sizeof(uint32));
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(uint32);

    /*
     * Write block name length 
     */
    nw_order_int = htonl(block_name_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    /*
     * Write block name 
     */
    sal_memcpy(&buffer[write_index], blk_name, block_name_length);
    write_index += block_name_length;

    /*
     * Write table name length 
     */
    nw_order_int = htonl(table_name_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    /*
     * Write table name 
     */
    sal_memcpy(&buffer[write_index], tbl_name, table_name_length);
    write_index += table_name_length;

    nw_order_int = htonl(entry_index);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    /** Write the packet to the socket */
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_read_record: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    /** Read the recorded hit indications from the adapter */
    control_get_table_data_response(unit, tbl_entry_data_allocated_size, tbl_entry_size, tbl_entry_data);

exit:
    adapter_context_db.adapter_sdk_interface_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Disable the collecting of writes into one bulk write
 *
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
int
adapter_disable_collect_accesses(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    adapter_info->collect_adapter_accesses = 0;

    SHR_FUNC_EXIT;
}

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
int
adapter_dnx_init_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    adapter_disable_collect_accesses(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Determine if collection of writes has to be done or not.
 *
 * \param [in] unit - Unit id
 * \param [in] is_get - 0 for set, 1 for get
 * \param [in] is_mem - 0 for registers, 1 for memories
 * \param [out] do_collect - Determine if collecting of accesses has to be done. '1' means collect, '0' means do not collect and
 *        write to the socket
 *
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void
adapter_collect_accesses(
    int unit,
    int is_get,
    int is_mem,
    uint8 *do_collect)
{

    SHR_FUNC_INIT_VARS(unit);

    *do_collect = 0;

    if (adapter_info->collect_adapter_accesses == 1 && !is_get
        && adapter_info->p_access_buffer_current_position - adapter_info->p_access_buffer <
        MAX_ACCESS_BUFFER_SIZE - MAX_SIZE_OF_ACCESS_ENTRY)
    {
        *do_collect = 1;
    }

    SHR_EXIT();

exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief - Send a command to the adapter. This function serves
 * read/write and memories/registers
 *
 * The format of the packet that is written to the socket is:
 * Opcode = 200 (1B) | overall_length (4B) | (opcode (1B) | data_length (4B) | Block ID (4 byte) | RTL Address (4 bytes)
 * | Data (variable length))* 
 * overall_length includes only the fields after it. data_length - data length in bytes 
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
int
adapter_memreg_access(
    int unit,
    int cmic_block,
    uint32 addr,
    uint32 data_length,
    int is_mem,
    int is_get,
    uint32 *data)
{
    uint32 nw_order_int;
    uint8 opcode;
    uint32 host_order_int;
    char *data_char_ptr;
    int data_len = 0;
    uint8 rounded_data_length = 0;
    uint8 padding_length = 0;
    int overall_length = 0;
    /** (padding_length + last_word_length) % 4 == 0. The 0 padding is added before the data of the last DWORD */
    /*
     * uint8 last_dword_length = 0; 
     */
    int ii = 0;
    uint8 do_collect;
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_get)
    {
        LOG_INFO(BSL_LS_SOC_SCHAN,
                 (BSL_META("adapter_memreg_access: unit=%d cmic_block=%d addr=0x%x data_length=%d is_mem=%d data=0x"),
                  unit, cmic_block, addr, data_length, is_mem));
        while (ii < ((data_length - 1) / 4 + 1))
        {
            LOG_INFO(BSL_LS_SOC_SCHAN, (BSL_META("%X "), data[ii]));
            ii++;
        }
        LOG_INFO(BSL_LS_SOC_SCHAN, (BSL_META("\n")));
    }

    adapter_context_db.adapter_mem_reg_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    /** Round up to the closest multiple of 4 */
    rounded_data_length = ((uint8) data_length + 3) & ~0x3;

    /** Add Opcode according to Set/Get and Table/Register */
    if (is_get)
    {
        if (is_mem)
        {
            opcode = UPDATE_FRM_SOC_READ_TBL_REQ;
        }
        else
        {
            opcode = UPDATE_FRM_SOC_READ_REG_REQ;
        }
    }
    else
    {
        /** In case of writes to table or register the length should include the data length */
        data_len = data_length;
        padding_length = rounded_data_length - data_length;
        /*
         * last_dword_length = (4 - padding_length) % 4; 
         */

        if (is_mem)
        {
            opcode = UPDATE_FRM_SOC_WRITE_TBL;
        }
        else
        {
            opcode = UPDATE_FRM_SOC_WRITE_REG;
        }
    }

    /** Add opcode */
    *adapter_info->p_access_buffer_current_position = opcode;
    adapter_info->p_access_buffer_current_position += sizeof(opcode);

    /** Add data length (sizeof(Block ID) + sizeof(RTL Address) + sizeof(Data)) */
    host_order_int = 4 + 4 + data_len + padding_length;

    /** Expected result length should be added to registers and not tables */
    if (is_get && !is_mem)
    {
        /** Add sizeof(expected data field) */
        host_order_int++;
    }

    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Add Block ID (4 B) */
    host_order_int = cmic_block;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Add RTL Address (4 B) */
    host_order_int = addr;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Expected result length should be added to registers and not tables */
    /** When doing a get the packet should describe how many bytes are expected */
    if (is_get && !is_mem)
    {
        *adapter_info->p_access_buffer_current_position = (uint8) rounded_data_length;
        adapter_info->p_access_buffer_current_position += sizeof(uint8);
    }

    /** Add data for set command */
    if (!is_get)
    {
        /** Add data for set command */
        data_char_ptr = (char *) data;

        /** Memories/Registers are writen in a reverse order. The LSB is writen first since the padding is done on the MSB */
        for (ii = 0; ii < rounded_data_length; ii++)
        {
            *(adapter_info->p_access_buffer_current_position + ii) = data_char_ptr[(rounded_data_length - 1) - ii];
        }
        adapter_info->p_access_buffer_current_position += rounded_data_length;
    }

    adapter_collect_accesses(unit, is_get, is_mem, &do_collect);

    if (do_collect == 0)
    {
        /** Add opcode for the whole transaction */
        *adapter_info->p_access_buffer = UPDATE_FRM_SOC_WRAPPER;

        /** Calculate the overall_length. Subtract opcode(1B) and overall_length field (4B) */
        overall_length = adapter_info->p_access_buffer_current_position - adapter_info->p_access_buffer - 1 - 4;

        /** Add the overall length (4B)  */
        nw_order_int = htonl(overall_length);
        sal_memcpy((adapter_info->p_access_buffer + 1), &nw_order_int, sizeof(overall_length));

        /*
         * Write the packet to the socket
         * The whole packet size is overall_length + 1B (opcode) + 4B (overall_length field)
         */
        if (writen(adapter_mem_reg_fd, adapter_info->p_access_buffer, (overall_length + 1 + 4)) !=
            (overall_length + 1 + 4))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_memreg_access data failed\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /*
         * The current position in the buffer is moved by 5 in order to be reserved memory in the beginning of the buffer for
         * opcode = 200 (1B) and overall_length field (4B)
         */
        adapter_info->p_access_buffer_current_position = adapter_info->p_access_buffer + 5;

        /** In case of get memory or register wait for the value to return */
        if (is_get)
        {
            SHR_IF_ERR_EXIT(adapter_memreg_read_response(unit, is_mem, rounded_data_length, data));
        }

    }

exit:
    adapter_context_db.adapter_mem_reg_mutex.give(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - check the validity of the memory attributes
 *
 * \param [in] unit - unit id
 * \param [in] mem - memory id
 * \param [in] array_index - index of the array in case of a memory array
 * \param [in] copyno - memory block id
 * \param [in] index - line number
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
adapter_mem_validity_check(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index)
{
    int entry_num_max;

    SHR_FUNC_INIT_VARS(unit);

    if (!soc_mem_is_valid(unit, mem))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory is not valid.\n");
    }

    if (copyno != MEM_BLOCK_ALL && copyno != SOC_CORE_ALL && !SOC_MEM_BLOCK_VALID(unit, mem, copyno))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory block is not valid.\n");
    }

    entry_num_max = soc_mem_index_max(unit, mem);

    if (index < 0 || index > entry_num_max)
    {
        
#if 1
        char *mem_name;
        char mem_prefix[12];
        mem_name = soc_mem_name[mem];

        if (sal_strlen(mem_name) >= 11)
        {
            sal_strncpy(mem_prefix, mem_name, 11);
            mem_prefix[11] = 0;
        }

        if (sal_strcmp(mem_prefix, "OAMP_MEP_DB"))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's index is invalid.\n");
        }
#else
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's index is invalid.\n");
#endif
    }

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        if (maip && (array_index < maip->first_array_index || array_index >= maip->first_array_index + maip->numels))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's array index is invalid.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check the validity of the register attributes
 *
 * \param [in] unit - unit id
 * \param [in] reg - reg id
 * \param [in] index - register's index when using an array
 *
 * \return
 *   shr_error_e - return SOC_E_MEMORY when the parameters are not valid
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
adapter_reg_validity_check(
    int unit,
    soc_reg_t reg,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_REG_IS_VALID(unit, reg))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Register is not invalid.\n");
    }

    if (SOC_REG_IS_ARRAY(unit, reg)
        && (index < SOC_REG_INFO(unit, reg).first_array_index
            || index >= SOC_REG_NUMELS(unit, reg) + SOC_REG_INFO(unit, reg).first_array_index))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Register's array index is invalid.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get register's size, address and block and then call
 * the access function that serves the memories and registers
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] is_get - 0 for set, 1 for get
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
static int
adapter_reg_access_handle(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    int is_get,
    soc_reg_above_64_val_t data)
{
    int reg_size;
    int rv;
    soc_reg_access_info_t access_info;
    int block;
    int cmic_block;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_reg_validity_check(unit, reg, index));

    if ((rv =
         soc_reg_xaddr_get(unit, reg, port, index, is_get ? SOC_REG_ADDR_OPTION_NONE : SOC_REG_ADDR_OPTION_WRITE,
                           &access_info)) != SOC_E_NONE)
    {
        return rv;
    }

    reg_size = soc_reg_bytes(unit, reg);

    for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block)
    {
        cmic_block = access_info.blk_list[block];
        /*
         * Workaround to support memories and registers reset. Writing to a cmic register is not supported. * This
         * workaround allows writing to a specific cmic register that is resetting the registers and * memories to
         * their default values. 
         */
        if (reg == DMU_CRU_RESETr)
        {
            cmic_block = 7;
        }
        rv = adapter_memreg_access(unit, cmic_block, access_info.offset, reg_size, 0, is_get, (uint32 *) data);
        if (is_get)
        {
            break;
        }
    }

    return rv;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - handler function for memory/register read/write
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] mem - memory ID
 *   \param [in] array_index - index of the memory in the array
 *   \param [in] copyno - core ID
 *   \param [in] index - index of the reg/memory in the array
 *   \param [in] is_read - 0 for write. 1 for read
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
static int
adapter_mem_access_handle(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    int is_read,
    void *entry_data)
{
    uint32 data_byte_len, addr;
    int cmic_blk, blk;
    uint8 acc_type;
    int entry_dw;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_mem_validity_check(unit, mem, array_index, copyno, index));

    entry_dw = soc_mem_entry_words(unit, mem);
    data_byte_len = entry_dw * sizeof(uint32);
    /*
     * If we are writing to all block instances, use COPYNO_ALL to loop over the instances 
     */
    if (copyno == SOC_CORE_ALL || SOC_BLOCK_IS_BROADCAST(unit, copyno))
    {
        copyno = COPYNO_ALL;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {
        if (copyno != COPYNO_ALL && copyno != blk)
        {
            continue;
        }

        /*
         * SW block representation to cmic HW block representation
         */
        cmic_blk = SOC_BLOCK2SCH(unit, blk);

        addr = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);

        rv = adapter_memreg_access(unit, cmic_blk, addr, data_byte_len, 1, is_read, (uint32 *) entry_data);
        SHR_IF_ERR_EXIT(rv);

        /*
         * In case of read operation, return after the first one
         */
        if (is_read)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register get 32 bits
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] reg - register ID
 *   \param [in] port - in port
 *   \param [in] index - index of the reg/memory in the array
 *   \param [out] data - value of the register/memory
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
int
adapter_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;
    /*
     * Make sure output container is initialized since down, along the call stack,
     * it is (a) loaded, but not in full, (b) it is used on unloaded locations
     */
    sal_memset(data_a64, 0, sizeof(data_a64));
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0)
    {
        return rv;
    }

    data[0] = data_a64[0];

    return _SHR_E_NONE;
}

/**
 * \brief - Register get 64 bits
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
int
adapter_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    sal_memset(data_a64, 0, sizeof(data_a64));
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0)
    {
        return rv;
    }

    COMPILER_64_SET(*data, data_a64[1], data_a64[0]);

    return _SHR_E_NONE;
}

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
int
adapter_reg_above64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    int rv;
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data);
    return rv;
}

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
int
adapter_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data)
{
    int rv;
    soc_reg_above_64_val_t data_a64;

    /*
     * Return error if HW modify is not allowed (e.g. during detach, wb) 
     */
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg32_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    /*
     * Return error if HW modify is not an err recovery approved access
     */
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg32_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[0] = data;

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data_a64);
    return rv;
}

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
int
adapter_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data)
{
    int rv;
    soc_reg_above_64_val_t data_a64;

    /*
     * Return error if HW modify is not allowed (e.g. during detach, wb) 
     */
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg64_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    /*
     * Return error if HW modify is not an err recovery approved access
     */
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg64_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[1] = COMPILER_64_HI(data);
    data_a64[0] = COMPILER_64_LO(data);

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data_a64);
    return rv;
}

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
int
adapter_reg_above64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    int rv;

    /*
     * Return error if HW modify is not allowed (e.g. during detach, wb) 
     */
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg_above64_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    /*
     * Return error if HW modify is not an err recovery approved access
     */
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg_above64_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data);
    return rv;
}

/**
 * \brief - handler for memory array read
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
int
adapter_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data)
{
    int rv;
    rv = adapter_mem_access_handle(unit, mem, array_index, copyno, index, 1, entry_data);
    return rv;
}

/**
 * \brief - handler for memory array write
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
int
adapter_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data)
{
    int rv;

    /*
     * Return error if HW modify is not allowed (e.g. during detach, wb) 
     */
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_mem_array_write: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    /*
     * Return error if HW modify is not an err recovery approved access
     */
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_mem_array_write: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    rv = adapter_mem_access_handle(unit, mem, array_index, copyno, index, 0, entry_data);
    return rv;
}

/**
 * \brief - Allocate the sockets for the adapter.
 * is_regs = 1 allocates the socket for the registers/memories.
 * is_regs = 0 is used for packets.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] is_regs - 0 for tx socket. 1 for memories and
 *          registers.
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
int
adapter_sockets_init(
    int unit,
    socket_target_e socket_target)
{
    /*
     * struct sockaddr_in cli_addr;
     */
    /*
     * socklen_t cli_addr_size;
     */
    struct hostent *hostent_ptr = NULL;
    char *s = NULL;
    char *default_udp_port = "6817";
    char *adapter_host = NULL;
    char tmp[80];
    int adapter_host_port;
    int optval = 1;
    int soc_fd;
    struct sockaddr_in srv_addr;
    int socket_type = SOCK_STREAM;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate mutex to control the socket's handling
     */
    switch (socket_target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.adapter_rx_tx_mutex.create(unit);
            s = getenv("CMODEL_PACKET_PORT");
            break;
        case ADAPTER_REGS:
            adapter_context_db.adapter_mem_reg_mutex.create(unit);
            s = getenv("CMODEL_MEMORY_PORT");
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.adapter_sdk_interface_mutex.create(unit);
            s = getenv("CMODEL_SDK_INTERFACE_PORT");
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.adapter_external_events_mutex.create(unit);
            s = getenv("CMODEL_EXTERNAL_EVENTS_PORT");
            if (!s)
            {
                /** Environment var missing. Using default value. */
                s = default_udp_port;
            }
            socket_type = SOCK_DGRAM;
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_sockets_init failed\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
    }

    if (!s)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }

    /*
     * Setup adapter's server port
     */

    adapter_host = getenv("SOC_TARGET_SERVER");

    /*
     * Get adapter's server name
     */
    /** Setup target host */
    if (!adapter_host)
    {
        snprintf(tmp, sizeof(tmp), "SOC_TARGET_SERVER%d", 0 /* devNo */ );
        adapter_host = getenv(tmp);
    }

    /*
     * Allocate a socket to the adapter's server. This socket will be used for the RxTx packet transmission
     */
    /** Wait until the adapter's server is up */
    adapter_host_port = atoi(s);

    memset((void *) &srv_addr, 0, sizeof(srv_addr));

    hostent_ptr = gethostbyname(adapter_host);
    if (hostent_ptr == NULL)
    {
        /*
         * cli_out("adapter_reg_access_init: hostname lookup failed for host [%s].\n", adapter_host);
         * perror("gethostbyname");
         */
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }
    memcpy(&srv_addr.sin_addr, hostent_ptr->h_addr, 4);

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(adapter_host_port);

    soc_fd = socket(AF_INET, socket_type, 0);
    if (soc_fd < 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }

    switch (socket_target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.adapter_rx_tx_fd.set(unit, soc_fd);
            break;
        case ADAPTER_REGS:
            adapter_context_db.adapter_mem_reg_fd.set(unit, soc_fd);
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.adapter_sdk_interface_fd.set(unit, soc_fd);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.adapter_external_events_fd.set(unit, soc_fd);
            adapter_context_db.adapter_server_address.set(unit, srv_addr.sin_addr.s_addr);
            adapter_context_db.adapter_external_events_port.set(unit, srv_addr.sin_port);
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_sockets_init failed\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
    }

    /*
     * UDP is connectionless. Connect only for TCP sockets 
     */
    if (socket_type == SOCK_STREAM)
    {
        /*
         * configure the socket to send the data immidiately
         */
        if (setsockopt(soc_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(optval)) < 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INIT);
            SHR_EXIT();
        }

        if (connect(soc_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) < 0)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INIT,
                                     "Failed connecting the Memory (1), Tx (0) or Signals (2) (%d) socket. Port (%d) adapter IP %s\r\n",
                                     socket_target, adapter_host_port, adapter_host);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init sockets to the c model.
 * One socket for registers and memories and the other for RxTx
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   soc_error_t
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
soc_error_t
adapter_reg_access_init(
    int unit)
{
    int adapter_mem_reg_fd;
    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.init(unit);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);

    if (!adapter_info)
    {

        /*
         * Allocate the static context parameter
         */
        adapter_info = sal_alloc(sizeof(adapter_access_info_t), "adapter access_info");

        if (!adapter_info)
        {
            return _SHR_E_MEMORY;
        }

        sal_memset(adapter_info, 0, sizeof(adapter_access_info_t));

        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, ADAPTER_RX_TX));
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, ADAPTER_REGS));
        adapter_sockets_init(unit, ADAPTER_SIGNALS);
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, ADAPTER_EVENTS));
    }

    if (NULL == adapter_info->p_access_buffer)
    {
        adapter_info->p_access_buffer = sal_alloc(sizeof(char) * MAX_ACCESS_BUFFER_SIZE, "adapter access buffer");
    }
    /*
     * The current position in the buffer is moved by 5 in order to be reserved memory in the beginning of the buffer for
     * opcode = 200 (1B) and overall_length field (4B)
     */
    adapter_info->p_access_buffer_current_position = adapter_info->p_access_buffer + 5;

    /** Gets the value of do_collect_enable from the dnx_data */
    adapter_info->collect_adapter_accesses = dnx_data_adapter.reg_mem_access.do_collect_enable_get(unit);
    /*
     * Register callbacks for memory access
     */
    SHR_IF_ERR_EXIT(soc_reg_access_func_register(unit, &adapter_access));

    /*
     * Reset the tables and return the registers to their default value
     */
    if (!sw_state_is_warm_boot(unit))
    {
        adapter_reg32_set(0, DMU_CRU_RESETr, 200, 0, 1);
    }

    SHR_IF_ERR_EXIT(adapter_init_rx_tx_socket(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Close the sockets and mutex to the c model's server
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
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
int
adapter_reg_access_deinit(
    int unit)
{
    int adapter_rx_tx_fd;
    int adapter_mem_reg_fd;
    int adapter_sdk_interface_fd;
    int adapter_external_events_fd;

    if (!adapter_info)
    {
        return _SHR_E_INIT;
    }

    adapter_context_db.adapter_rx_tx_fd.get(unit, &adapter_rx_tx_fd);
    adapter_context_db.adapter_mem_reg_fd.get(unit, &adapter_mem_reg_fd);
    adapter_context_db.adapter_sdk_interface_fd.get(unit, &adapter_sdk_interface_fd);
    adapter_context_db.adapter_external_events_fd.get(unit, &adapter_external_events_fd);

    /*
     * Close the socket to the adapter's server
     */
    if (adapter_rx_tx_fd >= 0)
    {
        close(adapter_rx_tx_fd);
        adapter_context_db.adapter_rx_tx_fd.set(unit, -1);
    }

    if (adapter_mem_reg_fd >= 0)
    {
        close(adapter_mem_reg_fd);
        adapter_context_db.adapter_mem_reg_fd.set(unit, -1);
    }

    /*
     * Close the signals socket to the adapter's server
     */
    if (adapter_sdk_interface_fd >= 0)
    {
        close(adapter_sdk_interface_fd);
        adapter_context_db.adapter_sdk_interface_fd.set(unit, -1);
    }

    /*
     * Close the external event socket to the adapter's server
     */
    if (adapter_external_events_fd >= 0)
    {
        close(adapter_external_events_fd);
        adapter_context_db.adapter_external_events_fd.set(unit, -1);
    }

    sal_free(adapter_info->p_access_buffer);
    sal_free(adapter_info);
    adapter_info = NULL;
    /*
     * sw state module deinit is done automatically at device deinit 
     */
    return _SHR_E_NONE;
}

/**
 * \brief - Reads a Tx buffer from the adapter's server.
 * Assumes that the buffer's length is a multiple of 4
 * Parameters: sockfd -socket file descriptor. Packet format:
 * length (4 bytes), block ID (4 bytes), NOF signals (4 bytes),
 * [signal ID (4 bytes), signal length (4 bytes), data]
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] nof_signals - number of signals
 *   \param [in] src_port - source port
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
uint32
adapter_read_buffer(
    int unit,
    adapter_ms_id_e * ms_id,
    uint32 *nof_signals,
    uint32 *src_port,
    int *len,
    unsigned char *buf)
{
    fd_set read_vect;
    char swapped_header[MAX_PACKET_HEADER_SIZE_ADAPTER];
    uint32 packet_length;
    long long_val;
    int offset = 0;
    int nfds = 0;
    int rv = _SHR_E_NONE;
    /*
     * int out_port; 
     */
    int adapter_rx_tx_fd;
    int constant_header_size;
    int adapter_ver;

    SHR_FUNC_INIT_VARS(unit);

    constant_header_size = dnx_data_adapter.rx.constant_header_size_get(unit);
    adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
    adapter_context_db.adapter_rx_tx_fd.get(unit, &adapter_rx_tx_fd);
    assert(adapter_rx_tx_fd);

    LOG_VERBOSE(BSL_LS_SYS_VERINET, (BSL_META("adapter_read_buffer: sockfd=%d\n"), adapter_rx_tx_fd));

    /** Setup bitmap for read notification*/
    FD_ZERO(&read_vect);

    /** Add Adapter Rx-Tx socket to selected fds*/
    FD_SET(adapter_rx_tx_fd, &read_vect);

    /** Add read end of pipe to selected fds*/
    FD_SET(pipe_fds[0], &read_vect);

    /** Set maximum fd */
    nfds = (adapter_rx_tx_fd > pipe_fds[0]) ? adapter_rx_tx_fd + 1 : pipe_fds[0] + 1;

    /*
     * Listen to two files:
     * - Adapter Rx-Tx socket (for incoming packets)
     * - Read end of pipe     (for thread exit notification)
     * Once a file contains information to be read, we read it and process
     * accordingly - handle packet or exit thread
     */
    while ((rv = select(nfds, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL)) == -1 && errno == EINTR)
    {
        continue;
    }

    if (rv < 0)
    {
        perror("get_command: select error");
        SHR_IF_ERR_EXIT(rv);
    }

    /** Thread is about to be closed */
    if (FD_ISSET(pipe_fds[0], &read_vect))
    {
        return RX_THREAD_NOTIFY_CLOSED;
    }

    /** Data ready to be read.*/
    if (FD_ISSET(adapter_rx_tx_fd, &read_vect))
    {
        /** Read the length of the packet*/
        if (readn(adapter_rx_tx_fd, &(swapped_header[0]), constant_header_size) < constant_header_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter server disconnected\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_DISABLED);
            SHR_EXIT();
        }

        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        packet_length = long_val;
        offset += sizeof(uint32);

        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *ms_id = long_val;
        offset += sizeof(uint32);

        /** out port was added in version 2.0 */
        if (adapter_ver >= 2)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            /*
             * out_port = long_val; 
             */
            offset += sizeof(uint32);
        }

        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *nof_signals = long_val;
        offset += sizeof(uint32);

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("adapter_read_buffer: packet_length=%d ms_id=%d nof_signals=%d\n"), packet_length, *ms_id,
                  *nof_signals));

        /*
         * packet_length is equal to the packet's size minus the length field size (4 bytes) 
         * The constant header was read already so the last part of the packet is left to be read.
         */
        *len = (packet_length + 4) - constant_header_size;

        /*
         * Read the Rx packet
         */
        if (readn(adapter_rx_tx_fd, buf, *len) < *len)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read packet\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sends messages
 *        (packets/signals/events) to the desired CModel socket.
 *
 * \param [in] unit - unit id.
 * \param [in] ms_id - Block id.
 * \param [in] target - The socket to use (ADAPTER_
 *        RX_TX/EVENTS).
 * \param [in] len - Buffer length in bytes.
 * \param [in] num_of_signals - number of signals (relevant only
 *        for ADAPTER_SIGNALS target, ignored otherwise).
 * \param [in] buf - The message to be sent (data only). Format
 *        is ASCII - Each bit (0/1) is represented as a char - 1
 *        byte.
 *
 * \return int - 0 no error, otherwise error occured
 */
int
adapter_write_buffer_to_socket(
    int unit,
    adapter_ms_id_e ms_id,
    socket_target_e target,
    int len,
    int num_of_signals,
    uint8 *buf)
{
    int rv = 0;
    uint32 nw_order_int;
    uint32 index_position = 0;
    int adapter_fd;
    uint8 message_data[MAX_PACKET_SIZE_ADAPTER];
    struct sockaddr_in srv_addr;
    socklen_t serverlen;
    uint32 server_ip, server_port;

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_write_buffer_to_socket: len=0x%x\n"), len));

    /*
     * Take mutex base on target 
     */
    switch (target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.adapter_rx_tx_mutex.take(unit, sal_mutex_FOREVER);
            adapter_context_db.adapter_rx_tx_fd.get(unit, &adapter_fd);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.adapter_external_events_mutex.take(unit, sal_mutex_FOREVER);
            adapter_context_db.adapter_external_events_fd.get(unit, &adapter_fd);
            break;
        default:
            break;
    }

    /*
     * Skip length (will be updated later) 
     */
    index_position += sizeof(int);

    /** Add MS ID */
    nw_order_int = htonl(ms_id);
    sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    if (target == ADAPTER_RX_TX)
    {
        /*
         * Add Src port type (=CPU)
         */
        nw_order_int = htonl(0);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);

        /*
         * Add source port. Use 0 hardcoded
         */
        nw_order_int = htonl(0);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);

        /*
         * Target RX_TX expects the number of signals to appear right after the MSID
         */
        nw_order_int = htonl(num_of_signals);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    /** Add buffer */
    sal_memcpy(&(message_data[index_position]), buf, len);
    index_position += len;

    /*
     * The length field (first 4 bytes of the message) indicates the number of bytes that follows it.
     */
    nw_order_int = htonl(index_position - sizeof(uint32));
    sal_memcpy(message_data, &nw_order_int, sizeof(int));

    /*
     * Write the data to the socket and free mutex base on target 
     */
    switch (target)
    {
        case ADAPTER_RX_TX:
            rv = (writen(adapter_fd, message_data, index_position) != index_position);
            adapter_context_db.adapter_rx_tx_mutex.give(unit);
            break;
        case ADAPTER_EVENTS:
            /*
             * Get the socket server data 
             */
            serverlen = sizeof(srv_addr);
            memset((void *) &srv_addr, 0, serverlen);
            srv_addr.sin_family = AF_INET;

            adapter_context_db.adapter_server_address.get(unit, &server_ip);
            adapter_context_db.adapter_external_events_port.get(unit, &server_port);
            srv_addr.sin_addr.s_addr = server_ip;
            srv_addr.sin_port = server_port;

            rv = (sendto(adapter_fd, message_data, index_position, 0, (struct sockaddr *) &srv_addr, serverlen) !=
                  index_position);

            adapter_context_db.adapter_external_events_mutex.give(unit);
            break;
        default:
            break;
    }

    if (rv)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_write_buffer_to_socket failed\n")));
    }

    /*
     * Wait for packet to get passed 
     */
    sal_usleep(5000000);

    return 0;
}

/**
 * \brief - Add a signal to a packet data.
 * 
 * \param [in] signal_id - signal id to add
 * \param [in] signal_width - signal width to add
 * \param [in] signal_value - signal value to add 
 * \param [in,out] index_position - points to the offset in the packet's buffer
 * \param [in,out] packet_data - packet to update
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void
adapter_send_buffer_add_signal(
    uint32 signal_id,
    uint32 signal_width,
    char *signal_value,
    uint32 *index_position,
    char *packet_data)
{
    uint32 nw_order_int;

    nw_order_int = htonl(signal_id);
    sal_memcpy(&(packet_data[*index_position]), &nw_order_int, sizeof(int));
    *index_position += sizeof(int);

    nw_order_int = htonl(signal_width);
    sal_memcpy(&(packet_data[*index_position]), &nw_order_int, sizeof(int));
    *index_position += sizeof(int);

    sal_memcpy(&(packet_data[*index_position]), signal_value, signal_width);
    *index_position += signal_width;
}

/**
 * \brief -  Client call: send packet to the adapter's server
 * unit - not in use
 * ms_id - ID of the block in the adapter
 * src_port - not in use
 * len - buf length in bytes
 * nof_signals - number of signals in buf
 * Packet format: | length in bytes from ms_id(32 bits) | ms_id (32 bits) | src_port (32 bits) nof_signals (32 bits) | 
 *                                  SIGNALS (Signal id 32 bits | data length in bytes 32 bits | data )*
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] src_port - source port
 *   \param [in] len - length of the buffer parameter
 *   \param [in] buf - buffer with the tx packet's data
 *   \param [in] nof_signals - number of signals including the header signal but not including the constant signals that are
 *          added in this function
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
uint32
adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    int len,
    unsigned char *buf,
    int nof_signals)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    uint32 nof_signals_index_position;
    uint32 packet_size_index_postion;
    uint32 buf_len_in_bits = len * 8;
    int index;
    char packet_data[MAX_PACKET_SIZE_ADAPTER];
    char signal_data[MAX_SIGNAL_SIZE];
    int adapter_rx_tx_fd;
    uint32 port_info;
    int nif_or_cpu_port;
    int core;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_send_buffer: src_port=%d len=0x%x\n"), src_port, len));

    adapter_context_db.adapter_rx_tx_mutex.take(unit, sal_mutex_FOREVER);
    adapter_context_db.adapter_rx_tx_fd.get(unit, &adapter_rx_tx_fd);

    /*
     * Leave space for packet_size_index_position
     */
    packet_size_index_postion = index_position;
    index_position += sizeof(int);

    /** Add MS ID */
    nw_order_int = htonl(ms_id);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Get the CPU port numbers */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    if (BCM_PBMP_MEMBER(port_config.cpu, src_port))
    {
        /** Add port type CPU */
        port_info = 0;
        nif_or_cpu_port = src_port;
    }
    else
    {
        /** Ethernet type port */
        port_info = 10;

        if (dnx_algo_port_nif_interface_id_get(unit, src_port, 0, &core, &nif_or_cpu_port) != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error: adapter_send_packet dnx_algo_port_nif_interface_id_get failed\n")));
            adapter_context_db.adapter_rx_tx_mutex.give(unit);
            return -1;
        }
    }

    nw_order_int = htonl(port_info);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Add nif port */
    nw_order_int = htonl(nif_or_cpu_port);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Leave space fot number of signals */
    nof_signals_index_position = index_position;
    index_position += sizeof(int);

    /** Add TAG_SWAP_RES signal */
    sal_memset(signal_data, 0, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH);
    adapter_send_buffer_add_signal(TX_PACKET_TAG_SWAP_RES_SIGNAL_ID, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH, signal_data,
                                   &index_position, packet_data);
    nof_signals++;

    /** Add RECYCLE_COMMAND signal */
    sal_memset(signal_data, 0, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH);
    adapter_send_buffer_add_signal(TX_PACKET_RECYCLE_COMMAND_SIGNAL_ID, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH,
                                   signal_data, &index_position, packet_data);
    nof_signals++;

    /** Add VALID_BYTES signal*/
    sal_memcpy(&(signal_data[0]), conversion_tbl[0], 8);
    adapter_send_buffer_add_signal(TX_PACKET_VALID_BYTES_SIGNAL_ID, TX_PACKET_VALID_BYTES_SIGNAL_WIDTH, signal_data,
                                   &index_position, packet_data);
    nof_signals++;

    /** Add header signal ID */
    nw_order_int = htonl(TX_PACKET_SIGNAL);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Add signal length in bytes. Each bit consumes a byte in the packet */
    nw_order_int = htonl(buf_len_in_bits);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Add bufferf */
    /*
     * Convert the buf to characters of '0' and '1'
     */
    for (index = 0; index < len; index++)
    {
        sal_memcpy(&(packet_data[index_position + index * 8]), conversion_tbl[(int) buf[index]], 8);
    }
    index_position += buf_len_in_bits;

    /** Write the nof_signals to the packet */
    nw_order_int = htonl(nof_signals); /** +1 is for the PTC signal which is always 0 */
    sal_memcpy(&(packet_data[nof_signals_index_position]), &nw_order_int, sizeof(int));

    /** Write the packet_size to the packet */
    /** Add packet's size. Not including the size field in the size */
    /*
     * Add the ms_id size and nof_signals size to the buffer's length.
     * Add the signal ID and signal's length as well.
     */
    nw_order_int = htonl(index_position - sizeof(uint32));
    sal_memcpy(&(packet_data[packet_size_index_postion]), &nw_order_int, sizeof(int));

    /*
     * write the packet to the socket
     */
    if (writen(adapter_rx_tx_fd, packet_data, index_position) != index_position)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_send_packet data failed\n")));
        adapter_context_db.adapter_rx_tx_mutex.give(unit);
        return -1;
    }

    adapter_context_db.adapter_rx_tx_mutex.give(unit);

exit:
    SHR_FUNC_EXIT;
}

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
uint32
adapter_terminate_server(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /** src_port is not used and that`s why it is 0  */
    SHR_IF_ERR_EXIT(adapter_send_buffer(unit, ADAPTER_MS_ID_TERMINATE_SERVER, 0, 0, NULL, 0));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/** ADAPTER_SERVER_MODE */
#endif
