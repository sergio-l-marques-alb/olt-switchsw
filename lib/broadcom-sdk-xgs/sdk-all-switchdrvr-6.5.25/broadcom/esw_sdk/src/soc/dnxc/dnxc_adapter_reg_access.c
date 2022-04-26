/** \file dnxc_adapter_reg_access.c
 * This file presents an example of replacing the registers/memories access functions. It
 * assumes using the portmod register DB bcm2801pb_a0
 */
/*
 * $Id:$
 $Copyright: (c) 2021 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

int
dnxc_tmp_workaround_func(
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
#include <appl/diag/system.h>

#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnxc/dnxc_adapter_regmem_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#endif /** BCM_ACCESS_SUPPORT */
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#include <netinet/tcp.h>
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
/** Signal ID for RECYCLE_COMMAND */
#define TX_PACKET_RECYCLE_CONTEXT_SIGNAL_ID 4
/** RECYCLE_COMMAND signal width in bits */
#define TX_PACKET_RECYCLE_CONTEXT_SIGNAL_WIDTH 6
/** Signal ID for VALID_BYTES */
#define TX_PACKET_VALID_BYTES_SIGNAL_ID 2
/** VALID_BYTES signal width in bits */
#define TX_PACKET_VALID_BYTES_SIGNAL_WIDTH 8

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
/** Opcode value for read and clear register request */
#define UPDATE_FRM_SOC_CLEAR_ON_READ_REG_REQ 146
/** Opcode value for read table request */
#define UPDATE_FRM_SOC_READ_TBL_REQ 134
/** Opcode value for read and clear table request */
#define UPDATE_FRM_SOC_CLEAR_ON_READ_TBL_REQ 147
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
#define FRM_SOC_WRAPPER_NOF_BYTES (5)

#define INIT_PACKET_SIZE 8

#define DNXC_ADAPTER_IS_BUFFER_NOT_EMPTY(__unit__, __sub_unit_id__) \
    (adapter_info[__unit__][__sub_unit_id__]->p_access_buffer_current_position > \
     adapter_info[__unit__][__sub_unit_id__]->p_access_buffer + FRM_SOC_WRAPPER_NOF_BYTES)

#define DNXC_DIE_ONE_OFFSET (0x10000000)
#define DNXC_ADAPTER_DIE_ONE_OFFSET (2058)

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
    ADAPTER_TBL_CONTROL_GET_RESPONSE_OPCODE,
    ADAPTER_MS_RUN_UNIT_TEST_REQUEST_OPCODE,
    ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_OPCODE
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
adapter_access_info_t *adapter_info[BCM_LOCAL_UNITS_MAX][DNX_DATA_MAX_ADAPTER_GENERAL_NOF_SUB_UNITS] = { {0} };

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

static int
adapter_nof_sub_units_get(
    int unit)
{
    if (SOC_IS_DNX(unit))
    {
        /** DNX cases, teh dnx data must be defined to all devices*/
        return dnx_data_adapter.general.nof_sub_units_get(unit);
    }
    else
    {
        /** DNXF - always 1*/
        return 1;
    }
}

/**
 * Map a given core to the Adapter sub unit id.
 * Relevant for units with multiple sub-units. where the higher level APIs relate ithe other sub-unit as a different core.
 */
static shr_error_e
adapter_sub_unit_id_from_core_get(
    int unit,
    int core,
    int *sub_unit_id)
{
    int index;
    int nof_sub_units = 0;
    int swapped_core = dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get(unit);
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    nof_sub_units = adapter_nof_sub_units_get(unit);

    for (index = 0; index < nof_sub_units; index++)
    {
        if (core == (index * nof_sub_units + swapped_core))
        {
            *sub_unit_id = index;
            found = TRUE;
        }
    }

    if (!found)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

static soc_error_t
adapter_mem_block_id_swap(
    int unit,
    soc_mem_t mem,
    int in_block_id,
    int *out_block_id)
{
    int core_idx_swap = dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get(unit);
    int num_of_blocks;
    int block_type;
    SHR_FUNC_INIT_VARS(unit);

    *out_block_id = in_block_id;
    /** if the block is COPYNO_ALL or core_swap is zero - no need to swap anything */
    if ((core_idx_swap == 0) || (in_block_id == COPYNO_ALL))
    {
        SHR_EXIT();
    }
    /** if the mem is part of ignore list, no need to swap anything */
    block_type = SOC_BLOCK_TYPE(unit, in_block_id);
    if (dnx_data_adapter.reg_mem_access.swap_core_ignore_map_get(unit, block_type)->ignore == 1)
    {
        SHR_EXIT();
    }

    num_of_blocks = (SOC_MEM_BLOCK_MAX(unit, mem) - SOC_MEM_BLOCK_MIN(unit, mem) + 1);
    if (num_of_blocks != dnx_data_device.general.nof_cores_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "num_of_blocks=%d, nof_cores=%d, cannot swap cores for mem=%d\n",
                     num_of_blocks, dnx_data_device.general.nof_cores_get(unit), mem);
    }
    else
    {
        /** if block_id belong to core-0, modify it to core_idx_swap*/
        if (in_block_id - SOC_MEM_BLOCK_MIN(unit, mem) == 0)
        {
            *out_block_id = SOC_MEM_BLOCK_MIN(unit, mem) + core_idx_swap;
        }
        /** if block_id belong to core_idx_swap, modify it to core zero */
        if (in_block_id - SOC_MEM_BLOCK_MIN(unit, mem) == core_idx_swap)
        {
            *out_block_id = SOC_MEM_BLOCK_MIN(unit, mem);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static soc_error_t
adapter_reg_block_idx_swap(
    int unit,
    soc_mem_t reg,
    int port_in,
    int *port_out)
{
    int core_idx_swap = dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get(unit);
    int num_of_blocks;
    int block_type;
    SHR_FUNC_INIT_VARS(unit);

    *port_out = port_in;
    /** if the port indicates ALL, or core_swap is zero - no need to swap anything */
    if ((core_idx_swap == 0) || (port_in == REG_PORT_ANY || port_in == SOC_CORE_ALL))
    {
        SHR_EXIT();
    }
    /** if the reg is part of ignore list, no need to swap anything */
    block_type = SOC_BLOCK_TYPE(unit, SOC_REG_BLOCK_FIRST_INSTANCES(unit, reg));
    if (dnx_data_adapter.reg_mem_access.swap_core_ignore_map_get(unit, block_type)->ignore == 1)
    {
        SHR_EXIT();
    }

    num_of_blocks = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
    if (num_of_blocks != dnx_data_device.general.nof_cores_get(unit))
    {
        /*
         * LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "num_of_blocks=%d, nof_cores=%d, cannot swap cores for
         * reg=%d\n"), num_of_blocks, dnx_data_device.general.nof_cores_get(unit), reg));
         */
        SHR_ERR_EXIT(_SHR_E_FAIL, "num_of_blocks=%d, nof_cores=%d, cannot swap cores for reg=%d\n",
                     num_of_blocks, dnx_data_device.general.nof_cores_get(unit), reg);
    }
    else
    {
        /** if port_in belong to core-0, modify it to core_idx_swap*/
        if (port_in == 0)
        {
            *port_out = port_in + core_idx_swap;
        }
        /** if port_in belong to swap_core, modify it to core_0 */
        if (port_in == core_idx_swap)
        {
            *port_out = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /* BCM_DNX_SUPPORT */

/**
 * \brief - Wait for the response from the adapter's server and read it from the socket
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] sub_unit_id - sub_unit_id
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
    int sub_unit_id,
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

    /** Used to swap the data bytes */
    int tmp;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

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

        /** before copy the data into the data_char_ptr buffer, verify that actual_data_len is not bigger then the buffer size */
        if ((actual_data_len > WORDS2BYTES(SOC_REG_ABOVE_64_MAX_SIZE_U32) && is_mem == 0) ||
            ((actual_data_len > SOC_MAX_MEM_BYTES) && (is_mem == 1)))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U
                                       (unit, "adapter_memreg_read_response: actual_data_len(=%d) above max allowed\n"),
                                       actual_data_len));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        read_length = readn(adapter_mem_reg_fd, data, actual_data_len);

        /*
         * Read the data field according to the length
         */
        /*
         * make sure the length of the response is the one expected: if(read_length < actual_data_len || data_len < actual_data_len) {
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
    int core,
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
    int sub_unit_id_for_core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_sub_unit_id_from_core_get(unit, core, &sub_unit_id_for_core));

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id_for_core, &adapter_sdk_interface_fd);

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
    int core,
    uint32 ms_id,
    uint32 *signal_data_length,
    char **signal_data)
{
    uint32 nw_order_int;
    int write_index = 0;
    uint32 host_order_int;
    char buffer[SIGNALS_REQUEST_HEADER_SIZE];
    int adapter_sdk_interface_fd;
    int sub_unit_id_for_core = -1;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = adapter_sub_unit_id_from_core_get(unit, core, &sub_unit_id_for_core);
    if (rv == _SHR_E_EMPTY)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id_for_core, &adapter_sdk_interface_fd);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_signals_write_request: unit=%d ms_id=%d \n"), unit, ms_id));

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, sub_unit_id_for_core, sal_mutex_FOREVER);

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
    SHR_IF_ERR_EXIT(adapter_signals_read_response(unit, core, signal_data_length, signal_data));

exit:
    if (sub_unit_id_for_core != -1)
    {
        adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, sub_unit_id_for_core);
    }
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
    int sub_unit_id,
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

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id, &adapter_sdk_interface_fd);

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
 * \param [in] sub_unit_id - sub unit index.
 *
 * \return status - 0 no error, otherwise error occured
 */
uint32
adapter_init_rx_tx_socket(
    int unit,
    int sub_unit_id)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    char packet_data[INIT_PACKET_SIZE];
    int adapter_rx_tx_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_rx_tx_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);

    /** Reserved two byte = 0 */
    packet_data[index_position++] = 0;
    packet_data[index_position++] = 0;

    /** Connection request version */
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        packet_data[index_position++] = dnx_data_adapter.general.lib_ver_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        packet_data[index_position++] = dnxf_data_device.general.adapter_lib_ver_get(unit);
#endif
    }

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
        adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id);
        SHR_ERR_EXIT(_SHR_E_PORT, "Error: adapter_init_rx_tx_socket failed\n");
    }

    adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read the block response from the adapter server
 *
 * Packet response format:
 *      length in bytes (4B)
 *      opcode (1B)
 *      MS name length (4B)
 *      unit test ID (4B)
 *      unit test result (4B)
 *      response message length (4B)
 *      MS name (MS name length + 1 bytes)
 *      response message (response message length + 1 bytes)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] ms_name - The name of the MS
 *   \param [in] unit_test_id - The ID of the MS unit test
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \param [out] unit_test_result - the result of the unit test execution - PASS/FAIL
 *   \param [out] message - response message from the unit test execution (optional)
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
adapter_ms_run_unit_test_read_response(
    int unit,
    int sub_unit_id,
    char *ms_name,
    uint32 unit_test_id,
    uint32 *unit_test_result,
    char message[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE])
{
    fd_set read_vect;
    int packet_size;
    int8 opcode;
    uint32 long_val;
    uint32 response_unit_test_id = -1;
    uint32 response_message_length;
    uint32 ms_name_length = 0;
    char response_ms_name[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE] = { 0 };
    int buffer_offset = 0;
    char buffer[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE] = { 0 };
    int read_constant_header = 0;
    int adapter_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id, &adapter_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_fd, &read_vect);

    while (1)
    {
        if (select(adapter_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
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
    if (FD_ISSET(adapter_fd, &read_vect))
    {
        read_constant_header = readn(adapter_fd, buffer, ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE);
        /** Read the length of the packet */
        if (read_constant_header < ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer could not read the constant header\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read packet_size */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if ((packet_size < ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE)
            || (packet_size >= ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_HEADER_SIZE))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter unit_test invalid packet size %d\n"), packet_size));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read opcode */
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter unit_test wrong opcode\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        /** Read the MS name length */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        ms_name_length = long_val;
        buffer_offset += sizeof(ms_name_length);

        /** Read the unit test ID */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        response_unit_test_id = long_val;
        buffer_offset += sizeof(unit_test_id);

        /** Read unit test result */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        *unit_test_result = long_val;
        buffer_offset += sizeof(uint32);

        /** Read the response message length */
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        response_message_length = long_val;
        buffer_offset += sizeof(response_message_length);

        /** Read the MS name */
        if (ms_name_length)
        {
            readn(adapter_fd, response_ms_name, ms_name_length);
        }

        /** Read the response message */
        if (response_message_length)
        {
            readn(adapter_fd, message, response_message_length);
        }
        else
        {
            message[0] = '\0';
        }

        /** Validate the response MS name and unit test ID match the requested */
        if (sal_strncmp(ms_name, response_ms_name, ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "The response MS name does not match the requested; Request \"%s\"; Response \"%s\"\n",
                         ms_name, response_ms_name);
        }
        if (unit_test_id != response_unit_test_id)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "The response unit test ID does not match the requested; Request \"%d\"; Response \"%d\"\n",
                         unit_test_id, response_unit_test_id);
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
 * \brief - Execute a module simulation unit test from the control plane according the
 *          MS ID and Unit test ID
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
int
adapter_ms_run_unit_test(
    int unit,
    char *ms_name,
    uint32 unit_test_id,
    uint32 *unit_test_result,
    char message[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE])
{
    uint32 nw_order_int;
    int write_index = 0;
    uint32 host_order_int;
    char buffer[ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_HEADER_SIZE];
    int adapter_fd;
    int base_sub_unit_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, base_sub_unit_id, &adapter_fd);

    LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("adapter_ms_run_unit_test_write_request: unit=%d ms_name=%s unit_test_id=%d\n"), unit, ms_name,
              unit_test_id));

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);

    /** Add opcode (1B) to the buffer */
    buffer[write_index] = ADAPTER_MS_RUN_UNIT_TEST_REQUEST_OPCODE;
    write_index += sizeof(char);

    /** Add unit test ID (4B) */
    host_order_int = unit_test_id;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(&(buffer[write_index]), &nw_order_int, sizeof(unit_test_id));
    write_index += sizeof(unit_test_id);

    /** Add ms_name length (4B) */
    host_order_int = sal_strnlen(ms_name, ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_MS_NAME_SIZE) + 1;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(&(buffer[write_index]), &nw_order_int, sizeof(uint32));
    write_index += sizeof(uint32);

    /** Add ms_name */
    sal_memcpy(&(buffer[write_index]), ms_name, host_order_int);
    write_index += host_order_int;

    /*
     * write the packet to the socket
     */
    if (writen(adapter_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_ms_run_unit_test_write_request data failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    /** Fetch the unit test result from the adapter */
    SHR_IF_ERR_EXIT(adapter_ms_run_unit_test_read_response
                    (unit, base_sub_unit_id, ms_name, unit_test_id, unit_test_result, message));

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, base_sub_unit_id);
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
    /** Assuming all sub units are the same adapter, we canread info from base sub unit id*/
    int base_sub_unit_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, base_sub_unit_id, &adapter_sdk_interface_fd);

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
    SHR_IF_ERR_EXIT(adapter_blocks_read_response(unit, base_sub_unit_id, block_names_length, block_names));

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, base_sub_unit_id);
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
    int sub_unit_id,
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

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

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

        if (packet_size > ADAPTER_HIT_INDICATION_MAX_PACKET_SIZE)
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
    int base_sub_unit_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, base_sub_unit_id, &adapter_mem_reg_fd);

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
    adapter_read_hit_bits_response(unit, base_sub_unit_id, record_length, recorded_data);

exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, base_sub_unit_id);
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
    int sub_unit_id,
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

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id, &adapter_sdk_interface_fd);

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
    int base_sub_unit_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    block_name_length = sal_strnlen(blk_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    table_name_length = sal_strnlen(tbl_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, base_sub_unit_id, &adapter_sdk_interface_fd);

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
    control_get_table_data_response(unit, base_sub_unit_id, tbl_entry_data_allocated_size, tbl_entry_size,
                                    tbl_entry_data);

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, base_sub_unit_id);
    SHR_FUNC_EXIT;
}

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
int
adapter_disable_collect_accesses(
    int unit,
    int sub_unit_id)
{
    SHR_FUNC_INIT_VARS(unit);

    adapter_info[unit][sub_unit_id]->collect_adapter_accesses = 0;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Determine if collection of writes has to be done or not.
 *
 * \param [in] unit - Unit id
 * \param [in] sub_unit_id - sub unit index
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
    int sub_unit_id,
    int is_get,
    int is_mem,
    int extra_bytes,
    uint8 *do_collect)
{

    SHR_FUNC_INIT_VARS(unit);

    *do_collect = 0;

    if ((adapter_info[unit][sub_unit_id]->collect_adapter_accesses == 1)
        && !is_get
        && ((adapter_info[unit][sub_unit_id]->p_access_buffer_current_position + extra_bytes -
             adapter_info[unit][sub_unit_id]->p_access_buffer) < MAX_ACCESS_BUFFER_SIZE - MAX_SIZE_OF_ACCESS_ENTRY))
    {
        *do_collect = 1;
    }

    SHR_EXIT();

exit:
    SHR_VOID_FUNC_EXIT;
}

shr_error_e
adapter_access_transaction_wrapper_set(
    int unit,
    int sub_unit_id,
    int adapter_mem_reg_fd)
{
    int overall_length;
    uint32 nw_order_int;
    SHR_FUNC_INIT_VARS(unit);

    /** Add opcode for the whole transaction */
    *adapter_info[unit][sub_unit_id]->p_access_buffer = UPDATE_FRM_SOC_WRAPPER;

    /** Calculate the overall_length. Subtract opcode(1B) and overall_length field (4B) */
    overall_length =
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position -
        adapter_info[unit][sub_unit_id]->p_access_buffer - FRM_SOC_WRAPPER_NOF_BYTES;

    /** Add the overall length (4B)  */
    nw_order_int = htonl(overall_length);
    sal_memcpy((adapter_info[unit][sub_unit_id]->p_access_buffer + 1), &nw_order_int, sizeof(overall_length));

    /*
     * Write the packet to the socket
     * The whole packet size is overall_length + 1B (opcode) + 4B (overall_length field)
     */
    if (writen(adapter_mem_reg_fd, adapter_info[unit][sub_unit_id]->p_access_buffer, (overall_length + 1 + 4)) !=
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
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position =
        adapter_info[unit][sub_unit_id]->p_access_buffer + FRM_SOC_WRAPPER_NOF_BYTES;

exit:
    SHR_FUNC_EXIT;
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
    int is_clear_on_read,
    uint32 *data)
{
    uint32 nw_order_int;
    uint8 opcode;
    uint32 host_order_int;
    char *data_char_ptr;
    int data_len = 0;
    uint32 rounded_data_length = 0;
    uint8 padding_length = 0;
    /** (padding_length + last_word_length) % 4 == 0. The 0 padding is added before the data of the last DWORD */
    /*
     * uint8 last_dword_length = 0;
     */
    int ii = 0;
    uint8 do_collect;
    int adapter_mem_reg_fd;
    /** it will be calculated base on each inputs layer (for example: core_id) */
    int sub_unit_id = 0;
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);

    /** The adapter die offset is different from the actual die offset */
    /** Once the cmic_block belong to the second die )according to the offset), convert it to the adapter offset */
    if (cmic_block & DNXC_DIE_ONE_OFFSET)
    {
        
        cmic_block = cmic_block & ~DNXC_DIE_ONE_OFFSET;
    }

    /** as temp solution, do not access if the sub_unit_id mutes was not created yet. */
    /** Remove it once sub_unit_id is dynamicly calc or input parameter */
    adapter_context_db.params.adapter_mem_reg_mutex.is_created(unit, sub_unit_id, &is_created);
    if (is_created == FALSE)
    {
        LOG_DEBUG(BSL_LS_SOC_SCHAN, (BSL_META("mutex for sub_unit_id=%d was not created, exit!\n"), is_created));
        SHR_EXIT();
    }

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

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

    /** Round up to the closest multiple of 4 */
    rounded_data_length = (data_length + 3) & ~0x3;

    if (!is_get)
    {
        /** if new data cross the max buffer size, first sent the buffer wo the new data */
        adapter_collect_accesses(unit, sub_unit_id, is_get, is_mem, rounded_data_length, &do_collect);
        if ((do_collect == 0) && DNXC_ADAPTER_IS_BUFFER_NOT_EMPTY(unit, sub_unit_id))
        {
            SHR_IF_ERR_EXIT(adapter_access_transaction_wrapper_set(unit, sub_unit_id, adapter_mem_reg_fd));
        }
    }

    /** Add Opcode according to Set/Get and Table/Register */
    if (is_get)
    {
        if (is_mem)
        {
            if (is_clear_on_read)
            {
                opcode = UPDATE_FRM_SOC_CLEAR_ON_READ_TBL_REQ;
            }
            else
            {
                opcode = UPDATE_FRM_SOC_READ_TBL_REQ;
            }

        }
        else
        {
            if (is_clear_on_read)
            {
                opcode = UPDATE_FRM_SOC_CLEAR_ON_READ_REG_REQ;
            }
            else
            {
                opcode = UPDATE_FRM_SOC_READ_REG_REQ;
            }

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
    *adapter_info[unit][sub_unit_id]->p_access_buffer_current_position = opcode;
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(opcode);

    /** Add data length (sizeof(Block ID) + sizeof(RTL Address) + sizeof(Data)) */
    host_order_int = 4 + 4 + data_len + padding_length;

    /** Expected result length should be added to registers and not tables */
    if (is_get && !is_mem)
    {
        /** Add sizeof(expected data field) */
        host_order_int++;
    }

    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Add Block ID (4 B) */
    host_order_int = cmic_block;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Add RTL Address (4 B) */
    host_order_int = addr;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    /** Expected result length should be added to registers and not tables */
    /** When doing a get the packet should describe how many bytes are expected */
    if (is_get && !is_mem)
    {
        if (rounded_data_length >= utilex_power_of_2(SAL_UINT8_NOF_BITS))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit, "Error: rounded_data_length=%d. above uint8 max size, (cmic_block=%d, addr=%d)\n"),
                       rounded_data_length, cmic_block, addr));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        *adapter_info[unit][sub_unit_id]->p_access_buffer_current_position = (uint8) rounded_data_length;
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(uint8);
    }

    /** Add data for set command */
    if (!is_get)
    {
        /** Add data for set command */
        data_char_ptr = (char *) data;

        /** Memories/Registers are writen in a reverse order. The LSB is writen first since the padding is done on the MSB */
        for (ii = 0; ii < rounded_data_length; ii++)
        {
            *(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position + ii) =
                data_char_ptr[(rounded_data_length - 1) - ii];
        }
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += rounded_data_length;
    }

    adapter_collect_accesses(unit, sub_unit_id, is_get, is_mem, 0, &do_collect);

    if (do_collect == 0)
    {
        SHR_IF_ERR_EXIT(adapter_access_transaction_wrapper_set(unit, sub_unit_id, adapter_mem_reg_fd));

        /** In case of get memory or register wait for the value to return */
        if (is_get)
        {
            SHR_IF_ERR_EXIT(adapter_memreg_read_response(unit, sub_unit_id, is_mem, rounded_data_length, data));
        }

    }

exit:
    if (is_created == TRUE)
    {
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
    }
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

#if !defined(SOC_NO_NAMES)
/* { */
    if (index < 0 || index > entry_num_max)
    {
        char *mem_name;
        char mem_prefix[12];
        mem_name = soc_mem_name[mem];

        if (sal_strnlen(mem_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) >= 11)
        {
            sal_strncpy(mem_prefix, mem_name, 11);
            mem_prefix[11] = 0;
        }

        if (sal_strncmp(mem_prefix, "OAMP_MEP_DB", sizeof(mem_prefix)))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's index is invalid.\n");
        }
    }
/* } */
#endif

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
    int is_clear_on_read = FALSE;
    int reg_index;
    const dnxc_data_table_info_t *table_info;
    int swap_port = port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_reg_validity_check(unit, reg, index));
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(adapter_reg_block_idx_swap(unit, reg, port, &swap_port));
    }
#endif
    if ((rv =
         soc_reg_xaddr_get(unit, reg, swap_port, index, is_get ? SOC_REG_ADDR_OPTION_NONE : SOC_REG_ADDR_OPTION_WRITE,
                           &access_info)) != SOC_E_NONE)
    {
        return rv;
    }
    reg_size = soc_reg_bytes(unit, reg);

    for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block)
    {
        cmic_block = access_info.blk_list[block];
        if (reg == DMU_CRU_RESETr)
        {
            if (SOC_IS_DNX(unit))
            {
#ifdef BCM_DNX_SUPPORT
                
                cmic_block = dnx_data_adapter.reg_mem_access.cmic_block_index_get(unit);
#endif
            }
            else
            {
#ifdef BCM_DNXF_SUPPORT
                cmic_block = 7;
#endif
            }
        }
#ifdef BCM_DNX_SUPPORT
        if ((is_get == TRUE) && (SOC_IS_DNX(unit)))
        {
            /** check if the reg in the clear on read list */
            table_info = dnx_data_adapter.reg_mem_access.clear_on_read_regs_info_get(unit);
            for (reg_index = 0; reg_index < table_info->key_size[0]; reg_index++)
            {
                if (dnx_data_adapter.reg_mem_access.clear_on_read_regs_get(unit, reg_index)->reg == reg)
                {
                    break;
                }
            }
            if ((reg_index <
                 table_info->key_size[0]) /** || (SOC_REG_INFO(unit, reg).flags1 & SOC_REG_FLAG_INIT_ON_READ) */ )
            {
                is_clear_on_read = TRUE;
            }
        }
#endif
        rv = adapter_memreg_access
            (unit, cmic_block, access_info.offset, reg_size, 0, is_get, is_clear_on_read, (uint32 *) data);
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
    const dnxc_data_table_info_t *table_info;
    int is_clear_on_read = FALSE, mem_index;
    int swap_copyno = copyno;
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
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(adapter_mem_block_id_swap(unit, mem, copyno, &swap_copyno));
    }
#endif

    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {
        if (swap_copyno != COPYNO_ALL && swap_copyno != blk)
        {
            continue;
        }

        /*
         * SW block representation to cmic HW block representation
         */
        cmic_blk = SOC_BLOCK2SCH(unit, blk);
#ifdef BCM_DNX_SUPPORT
        if ((is_read == TRUE) && (SOC_IS_DNX(unit)))
        {
            /** check if the mem in the clear on read list */
            table_info = dnx_data_adapter.reg_mem_access.clear_on_read_mems_info_get(unit);
            for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
            {
                if (dnx_data_adapter.reg_mem_access.clear_on_read_mems_get(unit, mem_index)->mem == mem)
                {
                    break;
                }
            }
            if (mem_index < table_info->key_size[0])
            {
                is_clear_on_read = TRUE;
            }
        }
#endif
        addr = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        rv = adapter_memreg_access(unit, cmic_blk, addr, data_byte_len, 1, is_read, is_clear_on_read,
                                   (uint32 *) entry_data);
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
 *   \param [in] sub_unit_id - sub unit index
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
    int sub_unit_id,
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
    int u_index;
    int nof_sub_units = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** assuming system with multiple sub-units and multiple units is not allowed */
    /** therefore, env variable index is determine just by one of them */

    nof_sub_units = adapter_nof_sub_units_get(unit);

    u_index = (nof_sub_units > 1) ? sub_unit_id : unit;
    /*
     * Allocate mutex to control the socket's handling
     */
    switch (socket_target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.params.adapter_rx_tx_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_PACKET_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_REGS:
            adapter_context_db.params.adapter_mem_reg_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_MEMORY_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.params.adapter_sdk_interface_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_SDK_INTERFACE_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_EXTERNAL_EVENTS_PORT_%d", u_index);
            s = getenv(tmp);
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
        sal_snprintf(tmp, sizeof(tmp), "SOC_TARGET_SERVER%d", u_index);
        adapter_host = getenv(tmp);
    }

    /*
     * Allocate a socket to the adapter's server. This socket will be used for the RxTx packet transmission
     */
    /** Wait until the adapter's server is up */
    adapter_host_port = sal_atoi(s);

    sal_memset((void *) &srv_addr, 0, sizeof(srv_addr));

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
    sal_memcpy(&srv_addr.sin_addr, hostent_ptr->h_addr, 4);

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
            adapter_context_db.params.adapter_rx_tx_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_REGS:
            adapter_context_db.params.adapter_mem_reg_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.params.adapter_sdk_interface_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_fd.set(unit, sub_unit_id, soc_fd);
            adapter_context_db.params.adapter_server_address.set(unit, sub_unit_id, srv_addr.sin_addr.s_addr);
            adapter_context_db.params.adapter_external_events_port.set(unit, sub_unit_id, srv_addr.sin_port);
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
 *   \param [in] unit - Unit-ID *
 *   \param [in] sub_unit_id - sub unit index
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
    int unit,
    int sub_unit_id)
{
    int adapter_mem_reg_fd;
    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

    if (!adapter_info[unit][sub_unit_id])
    {

        /*
         * Allocate the static context parameter
         */
        adapter_info[unit][sub_unit_id] = sal_alloc(sizeof(adapter_access_info_t), "adapter access_info");

        if (!adapter_info[unit][sub_unit_id])
        {
            return _SHR_E_MEMORY;
        }

        sal_memset(adapter_info[unit][sub_unit_id], 0, sizeof(adapter_access_info_t));

        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_RX_TX));
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_REGS));
        adapter_sockets_init(unit, sub_unit_id, ADAPTER_SIGNALS);
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_EVENTS));
    }

    if (NULL == adapter_info[unit][sub_unit_id]->p_access_buffer)
    {
        adapter_info[unit][sub_unit_id]->p_access_buffer =
            sal_alloc(sizeof(char) * MAX_ACCESS_BUFFER_SIZE, "adapter access buffer");
    }
    /*
     * The current position in the buffer is moved by 5 in order to be reserved memory in the beginning of the buffer for
     * opcode = 200 (1B) and overall_length field (4B)
     */
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position =
        adapter_info[unit][sub_unit_id]->p_access_buffer + 5;

    /** Gets the value of do_collect_enable from the dnx_data */
    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        adapter_info[unit][sub_unit_id]->collect_adapter_accesses =
            dnx_data_adapter.reg_mem_access.do_collect_enable_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        /*
         * Feature is not relevant for DNXF
         */
        adapter_info[unit][sub_unit_id]->collect_adapter_accesses = 0;
#endif
    }
    /*
     * Register callbacks for memory access - do it for sub_unit 0 only
     */
    if (sub_unit_id == 0)
    {
        SHR_IF_ERR_EXIT(soc_reg_access_func_register(unit, &adapter_access));
    }
    /*
     * Reset the tables and return the registers to their default value
     */
    if (!sw_state_is_warm_boot(unit))
    {
        adapter_reg32_set(unit, DMU_CRU_RESETr, REG_PORT_ANY, 0, 1);
    }

    SHR_IF_ERR_EXIT(adapter_init_rx_tx_socket(unit, sub_unit_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Close the sockets and mutex to the c model's server
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
int
adapter_reg_access_deinit(
    int unit,
    int sub_unit_id)
{
    int adapter_rx_tx_fd;
    int adapter_mem_reg_fd;
    int adapter_sdk_interface_fd;
    int adapter_external_events_fd;

    if (!adapter_info[unit][sub_unit_id])
    {
        return _SHR_E_INIT;
    }

    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id, &adapter_sdk_interface_fd);
    adapter_context_db.params.adapter_external_events_fd.get(unit, sub_unit_id, &adapter_external_events_fd);

    /*
     * Close the socket to the adapter's server
     */
    if (adapter_rx_tx_fd >= 0)
    {
        close(adapter_rx_tx_fd);
        adapter_context_db.params.adapter_rx_tx_fd.set(unit, sub_unit_id, -1);
    }

    if (adapter_mem_reg_fd >= 0)
    {
        close(adapter_mem_reg_fd);
        adapter_context_db.params.adapter_mem_reg_fd.set(unit, sub_unit_id, -1);
    }

    /*
     * Close the signals socket to the adapter's server
     */
    if (adapter_sdk_interface_fd >= 0)
    {
        close(adapter_sdk_interface_fd);
        adapter_context_db.params.adapter_sdk_interface_fd.set(unit, sub_unit_id, -1);
    }

    /*
     * Close the external event socket to the adapter's server
     */
    if (adapter_external_events_fd >= 0)
    {
        close(adapter_external_events_fd);
        adapter_context_db.params.adapter_external_events_fd.set(unit, sub_unit_id, -1);
    }

    sal_free(adapter_info[unit][sub_unit_id]->p_access_buffer);
    sal_free(adapter_info[unit][sub_unit_id]);
    adapter_info[unit][sub_unit_id] = NULL;
    /*
     * sw state module deinit is done automatically at device deinit
     */
    return _SHR_E_NONE;
}

/**
 * \brief - This function sends messages
 *        (packets/signals/events) to the desired adapter socket.
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
    uint8 *message_data = NULL;
    struct sockaddr_in srv_addr;
    socklen_t serverlen;
    int adapter_ver = 0;
    uint32 server_ip, server_port;

    
    int base_sub_unit_id = 0;

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_write_buffer_to_socket: len=0x%x\n"), len));

    message_data = sal_alloc(sizeof(*message_data) * MAX_PACKET_SIZE_ADAPTER, "message_data buffer");

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        adapter_ver = dnxf_data_device.general.adapter_lib_ver_get(unit);;
#endif
    }
    /*
     * Skip length (will be updated later)
     */
    index_position += sizeof(int);

    /*
     * Take mutex base on target
     */
    switch (target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.params.adapter_rx_tx_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);
            adapter_context_db.params.adapter_rx_tx_fd.get(unit, base_sub_unit_id, &adapter_fd);
            /** Device ID dest was added in version 4.0 */
            /** due to adapter problem, ADAPTER_EVENTS cannot work in version 4 or 5, even if defined */
            if (adapter_ver >= 4)
            {
                int device_id = -1;
                nw_order_int = htonl(device_id);
                sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
                index_position += sizeof(int);
            }
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_mutex.take(unit, base_sub_unit_id, sal_mutex_FOREVER);
            adapter_context_db.params.adapter_external_events_fd.get(unit, base_sub_unit_id, &adapter_fd);
            break;
        default:
            break;
    }

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

        /** Add src channel from version 3.0 */
        if (adapter_ver >= 3)
        {
            nw_order_int = htonl(0);
            sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
            index_position += sizeof(int);
        }
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
            adapter_context_db.params.adapter_rx_tx_mutex.give(unit, base_sub_unit_id);
            break;
        case ADAPTER_EVENTS:
            /*
             * Get the socket server data
             */
            serverlen = sizeof(srv_addr);
            sal_memset((void *) &srv_addr, 0, serverlen);
            srv_addr.sin_family = AF_INET;

            adapter_context_db.params.adapter_server_address.get(unit, base_sub_unit_id, &server_ip);
            adapter_context_db.params.adapter_external_events_port.get(unit, base_sub_unit_id, &server_port);
            srv_addr.sin_addr.s_addr = server_ip;
            srv_addr.sin_port = server_port;

            rv = (sendto(adapter_fd, message_data, index_position, 0, (struct sockaddr *) &srv_addr, serverlen) !=
                  index_position);

            adapter_context_db.params.adapter_external_events_mutex.give(unit, base_sub_unit_id);
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
    sal_free(message_data);

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
    uint32 src_channel,
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
    char *packet_data = NULL;
    char signal_data[MAX_SIGNAL_SIZE];
    int adapter_rx_tx_fd;
    int adapter_ver = 0;
    uint32 port_info = 0;
    int nif_or_cpu_port = 0;
    int core;
    int sub_unit_id_for_core;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_send_buffer: src_port=%d len=0x%x\n"), src_port, len));

    packet_data = sal_alloc(sizeof(*packet_data) * MAX_PACKET_SIZE_ADAPTER, "packet_data buffer");

    /*
     * Adapter might have a sub units instances.
     * Derive the sub_unit as:
     * src_port->core->sub_unit
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));
    SHR_IF_ERR_EXIT(adapter_sub_unit_id_from_core_get(unit, core, &sub_unit_id_for_core));

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        adapter_ver = packet_data[index_position++] = dnxf_data_device.general.adapter_lib_ver_get(unit);;
#endif
    }
    adapter_context_db.params.adapter_rx_tx_mutex.take(unit, sub_unit_id_for_core, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id_for_core, &adapter_rx_tx_fd);

    /*
     * Leave space for packet_size_index_position
     */
    packet_size_index_postion = index_position;
    index_position += sizeof(int);

    /** Device ID dest was added in version 4.0 */
    if (adapter_ver >= 4)
    {
        int device_id = -1;
        nw_order_int = htonl(device_id);
        sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    /** Add MS ID */
    nw_order_int = htonl(ms_id);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Get the CPU port numbers */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT

        if (BCM_PBMP_MEMBER(port_config.cpu, src_port))
        {
            /** Add port type CPU */
            port_info = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
            nif_or_cpu_port = src_port;
        }
        else
        {
            /** Ethernet type port */
            port_info = dnx_data_egr_queuing.params.egr_if_nif_base_get(unit);
            if (dnx_algo_port_nif_interface_id_get(unit, src_port, 0, &core, &nif_or_cpu_port) != 0)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Error: adapter_send_packet dnx_algo_port_nif_interface_id_get failed\n")));
                adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id_for_core);
                return -1;
            }
        }
#endif
    }
    nw_order_int = htonl(port_info);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Add nif port */
    nw_order_int = htonl(nif_or_cpu_port);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /** Add src channel from version 3.0 */
    if (adapter_ver >= 3)
    {
        nw_order_int = htonl(src_channel);
        sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    /** Leave space fot number of signals */
    nof_signals_index_position = index_position;
    index_position += sizeof(int);

    /** Add TAG_SWAP_RES signal */
    sal_memset(signal_data, 0, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH);
    adapter_send_buffer_add_signal(TX_PACKET_TAG_SWAP_RES_SIGNAL_ID, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH, signal_data,
                                   &index_position, packet_data);
    nof_signals++;

    if (!dnx_data_headers.
        system_headers.feature_get(unit, dnx_data_headers_system_headers_system_headers_ibch1_supported))
    {
        /** Add RECYCLE_COMMAND signal */
        sal_memset(signal_data, 0, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH);
        adapter_send_buffer_add_signal(TX_PACKET_RECYCLE_COMMAND_SIGNAL_ID, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH,
                                       signal_data, &index_position, packet_data);
        nof_signals++;
    }
    else
    {
        /** Add RECYCLE_CONTEXT signal */
        sal_memset(signal_data, 0, TX_PACKET_RECYCLE_CONTEXT_SIGNAL_WIDTH);
        adapter_send_buffer_add_signal(TX_PACKET_RECYCLE_CONTEXT_SIGNAL_ID, TX_PACKET_RECYCLE_CONTEXT_SIGNAL_WIDTH,
                                       signal_data, &index_position, packet_data);
        nof_signals++;
    }
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
        adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id_for_core);
        return -1;
    }

    adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id_for_core);

exit:
    sal_free(packet_data);
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
    SHR_IF_ERR_EXIT(adapter_send_buffer(unit, ADAPTER_MS_ID_TERMINATE_SERVER, 0, 0, 0, NULL, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - trigger access transaction, to upload into adapter all buffered transaction.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sub_unit_id - sub unit
 *
 * \return
 *   uint32
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
adapter_access_transaction_trigger(
    int unit,
    int sub_unit_id)
{
    int mutex_taken = 0;
    int adapter_mem_reg_fd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(adapter_info[unit][sub_unit_id]->p_access_buffer, _SHR_E_INIT, "p_access_buffer");
    SHR_NULL_CHECK(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, _SHR_E_INIT,
                   "p_access_buffer_current_position");

    /** current position is intiate to the access_buffer + FRM_SOC_WRAPPER_NOF_BYTES. */
    /** if current position is above it, it means something in the buffer, and need to trigger it */
    if (DNXC_ADAPTER_IS_BUFFER_NOT_EMPTY(unit, sub_unit_id))
    {
        adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);
        adapter_context_db.params.adapter_mem_reg_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
        mutex_taken = TRUE;
        SHR_IF_ERR_EXIT(adapter_access_transaction_wrapper_set(unit, sub_unit_id, adapter_mem_reg_fd));
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
        mutex_taken = FALSE;
    }

exit:
    if (mutex_taken == TRUE)
    {
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
    }
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
    int sub_unit_id;
    int nof_sub_units = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_sub_units = adapter_nof_sub_units_get(unit);

    for (sub_unit_id = 0; sub_unit_id < nof_sub_units; sub_unit_id++)
    {
        adapter_disable_collect_accesses(unit, sub_unit_id);
        SHR_IF_ERR_EXIT(adapter_access_transaction_trigger(unit, sub_unit_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

shr_error_e
dnxc_init_adapter_reg_access_init(
    int unit)
{
    sal_thread_t *mem_scan_pid_p;
    sal_usecs_t **mem_scan_interval_p;
    int sub_unit_id;
    int nof_sub_units = 0;
    int nof_units;

    SHR_FUNC_INIT_VARS(unit);

    nof_units = soc_cm_get_num_devices();

    nof_sub_units = adapter_nof_sub_units_get(unit);

    /** Verify that in case of multiple units, no unit has multiple sub-units */
    if (nof_units > 1)
    {
        if (nof_sub_units > 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d has %d subunits. "
                         "adapter assumes only units or sub-units is larger than 1", unit, nof_sub_units);
        }
    }

    /** lock the adapter init step - because socket connection use the same address for all units and can't do it in parallel */
    if (dnx_adapter_init_lock != NULL)
    {
        /** take lock */
        if (0 != sal_mutex_take(dnx_adapter_init_lock, 2000000))
        {
            SHR_ERR_EXIT(BCM_E_TIMEOUT, "unable to take dnx_adapter_init_lock mutex");
        }
    }

    adapter_context_db.init(unit);

    for (sub_unit_id = 0; sub_unit_id < nof_sub_units; sub_unit_id++)
    {
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, MEM_SCAN_PID, (void **) &mem_scan_pid_p));
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, MEM_SCAN_INTERVAL, (void **) &mem_scan_interval_p));
        /*
         * do not enable memscan task, otherwise tr 7 will segmentation fail
         */
        *mem_scan_pid_p = SAL_THREAD_ERROR;
        *mem_scan_interval_p = 0;

        SHR_IF_ERR_EXIT(adapter_reg_access_init(unit, sub_unit_id));
    }
exit:
    if (dnx_adapter_init_lock != NULL)
    {
        if (0 != sal_mutex_give(dnx_adapter_init_lock))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "unable to give dnx_adapter_init_lock mutex \n")));
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_init_adapter_reg_access_deinit(
    int unit)
{
    int sub_unit_id;
    int nof_sub_units = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_sub_units = adapter_nof_sub_units_get(unit);

    for (sub_unit_id = 0; sub_unit_id < nof_sub_units; sub_unit_id++)
    {
        SHR_IF_ERR_EXIT(adapter_reg_access_deinit(unit, sub_unit_id));
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT
#ifdef BCM_DNX_SUPPORT
static soc_error_t
dnxc_adapter_regmem_inst_swap(
    int unit,
    const access_device_block_t * block,
    access_regmem_id_t regmem,
    access_block_instance_num_t in_inst,
    access_block_instance_num_t * out_inst)
{
    int core_idx_swap = dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get(unit);
    int num_of_instances = block->nof_instances;

    SHR_FUNC_INIT_VARS(unit);

    *out_inst = in_inst;
    /** if the instance is ACCESS_ALL_BLOCK_INSTANCES or core_swap is zero - no need to swap anything */
    if ((core_idx_swap == 0) || (in_inst == ACCESS_ALL_BLOCK_INSTANCES))
    {
        SHR_EXIT();
    }
    /** if the block is part of ignore list, no need to swap anything */
    if (dnx_data_adapter.reg_mem_access.swap_core_ignore_map_by_global_block_ids_enum_get
        (unit, block->block_type)->ignore == 1)
    {
        SHR_EXIT();
    }

    if (num_of_instances != dnx_data_device.general.nof_cores_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "num_of_instances=%d, nof_cores=%d, cannot swap cores for regmem=%d\n",
                     num_of_instances, dnx_data_device.general.nof_cores_get(unit), regmem);
    }
    else
    {
        /** if inst_id belong to core-0, modify it to core_idx_swap*/
        if (in_inst == 0)
        {
            *out_inst = core_idx_swap;
        }
        /** if block_id belong to core_idx_swap, modify it to core zero */
        if (in_inst == core_idx_swap)
        {
            *out_inst = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - check the validity of the memory/register attributes
 *
 * \param [in] unit - unit id
 * \param [in] flags - access flags
 * \param [in] regmem - register/memory id
 * \param [in] inst - instance id
 * \param [in] array_index - array index. zero if no array
 * \param [in] mem_index - memory index in case of memory. otherwise, zero
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
dnxc_adapter_regmem_validity_check(
    int unit,
    uint32 flags,
    access_regmem_id_t regmem,
    access_block_instance_num_t inst,
    uint16 array_index,
    uint32 mem_index,
    int is_read)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_IS_NEW_ACCESS_INITIALIZED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "new access is not active.\n");
    }

    if (is_read == FALSE)
    {
        /*
         * Return error if HW modify is not allowed (e.g. during detach, wb)
         */
        if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR, It is not currently allowed to modify HW\n");
        }

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * Return error if HW modify is not an err recovery approved access
         */
        if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
        {

            SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid error recovery hw access\n");
        }
#endif
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check the validity of the memory/register attributes
 *
 * \param [in] unit - unit id
 * \param [in] flags - access flags
 * \param [in] regmem - register/memory id
 * \param [in] inst - instance id
 * \param [in] array_index - array index. zero if no array
 * \param [in] mem_index - memory index in case of memory. otherwise, zero
 * \param [inout] entry_data - data in to write, data out to read
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
dnxc_adapter_regmem_access_handle(
    const int unit,
    uint32 flags,
    access_regmem_id_t regmem,
    access_block_instance_num_t inst,
    uint16 array_index,
    uint32 mem_index,
    uint32 *entry_data)
{
    int is_mem = FALSE, is_read = TRUE;
    uint32 data_byte_len;
    access_block_instance_num_t swap_inst = inst;
    access_runtime_info_t *runtime_info = SOC_CONTROL(unit)->device_access_runtime_info;        /* Access information
                                                                                                 * for the device */
    access_local_regmem_id_t local_regmem;
    access_regmem_sbus_info_t sbus_access_info;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info;
    const access_device_block_t *local_block;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, FLAG_ACCESS_IS_WRITE))
    {
        is_read = FALSE;
    }
    SHR_IF_ERR_EXIT(dnxc_adapter_regmem_validity_check(unit, flags, regmem, inst, array_index, mem_index, is_read));

    SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, regmem, &local_regmem));
    rm_info = device_info->local_regs + local_regmem;
    local_block = device_info->blocks + rm_info->local_block_id;        /* The local block of the regmem */

    if (_SHR_IS_FLAG_SET(rm_info->flags, ACCESS_REGMEM_FLAG_MEMORY))
    {
        is_mem = TRUE;
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_adapter_regmem_inst_swap(unit, local_block, regmem, inst, &swap_inst));
    }
#endif

    SHR_IF_ERR_EXIT(access_get_regmem_model_access_info
                    (unit, flags, regmem, swap_inst, array_index, mem_index, &sbus_access_info));
    data_byte_len = sbus_access_info.size_in_uint32s * sizeof(uint32);

    /** assuming the caller, allocated entry_data in the size of the entry. */
    if (is_read == TRUE)
    {
        sal_memset(entry_data, 0x0, data_byte_len);
    }

    /** DMU_CRU_RESETr special case */
    if (rDMU_CRU_RESET == regmem)
    {
        if (SOC_IS_DNX(unit))
        {
#ifdef BCM_DNX_SUPPORT
            
            sbus_access_info.block_ids[0] = dnx_data_adapter.reg_mem_access.cmic_block_index_get(unit);
#endif
        }
        else
        {
#ifdef BCM_DNXF_SUPPORT
            sbus_access_info.block_ids[0] = 7;
#endif
        }
    }

    /** access all instances required */
    for (int operation_idx = 0; operation_idx < sbus_access_info.nof_operations; operation_idx++)
    {
        SHR_IF_ERR_EXIT(adapter_memreg_access(unit, sbus_access_info.block_ids[operation_idx], sbus_access_info.address,
                                              data_byte_len, is_mem, is_read, FALSE, (uint32 *) entry_data));

        /** In case of read operation, return after the first one */
        if (is_read)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;

}
#endif /** BCM_ACCESS_SUPPORT */
/*
 * }
 */
/** ADAPTER_SERVER_MODE */
#endif
