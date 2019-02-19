/*
 *
 * $Id:$
 *
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * This file present an example of replacing the registers\memories access functions.
 * It assumes using the portmod register DB bcm2801pb_a0
 *
 */

int tmp_workaround_func(void)
{
  return 0;
}

#ifdef CMODEL_SERVER_MODE

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_GENERAL

#ifndef VXWORKS
#include <netdb.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <bde/pli/verinet.h>
#include <errno.h>
#include <sys/socket.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <netinet/tcp.h>
#include <soc/dnx/cmodel/cmodel_reg_access.h>

#define DEFAULT_CMODEL_SERVER_PORT 6816
#define DEFAULT_CMODEL_SERVER_ADDRESS "localhost"
#define HOST_ADDRESS_MAX_LEN 40
#define MAX_PACKET_HEADER_SIZE_CMODEL 12
#define MEM_RESPONSE_HEADER_SIZE_CMODEL 10
#define TX_PACKET_SIGNAL 3
#define RX_THREAD_NOTIFY_CLOSED (-2)

/* Opcode values taken from the C model's server */
/* Set opcodes */
#define UPDATE_FRM_SOC_WRITE_REG 128
#define UPDATE_FRM_SOC_WRITE_TBL 129

/* Get opcodes (request and reply) */
#define UPDATE_FRM_SOC_READ_REG_REQ 133
#define UPDATE_FRM_SOC_READ_TBL_REQ 134
#define UPDATE_TO_SOC_READ_REG_REP 8
#define UPDATE_TO_SOC_READ_TBL_REP 9

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

typedef struct cmodel_access_info_s {

    /* Params for sending and receiving packets and signals */
    int         cmodelRxTxSocFd;
    sal_mutex_t cmodelRxTxMutex;

    /* Params for accessing the registers and memories */
    int         cmodelMemRegSocFd;
    sal_mutex_t cmodelMemRegMutex;

    /* Rx thread handle */
    sal_thread_t rx_tid;

} cmodel_access_info_t;

static cmodel_access_info_t *cmodelInfo = {0};

STATIC int cmodel_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32* data);
STATIC int cmodel_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64* data);
STATIC int cmodel_reg_above64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);
STATIC int cmodel_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data);
STATIC int cmodel_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
STATIC int cmodel_reg_above64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);
STATIC int cmodel_mem_array_read(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data);
STATIC int cmodel_mem_array_write(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data);

/* Callbacks that support C model */
soc_reg_access_t cmodel_access =  {
    cmodel_reg32_get,
    cmodel_reg64_get,
    cmodel_reg_above64_get,

    cmodel_reg32_set,
    cmodel_reg64_set,
    cmodel_reg_above64_set,

    cmodel_mem_array_read,
    cmodel_mem_array_write
};

extern char *getenv(const char*);
extern int _soc_mem_write_copyno_update(int unit, soc_mem_t mem, int *copyno, int *copyno_override);

/* Wait for the response from the C model's server and read it from the socket */
STATIC soc_error_t cmodel_memreg_read_response(int unit, int is_mem, uint32 data_len, uint32 *data)
{
    int rv = SOC_E_NONE;
    fd_set read_vect;
    char swapped_header[MEM_RESPONSE_HEADER_SIZE_CMODEL];
    uint32 nw_order_int;
    uint32 rtl_address;
    int8 opcode, block_id;
    int read_length = 0;
    uint32 actual_data_len;
    uint32 half_actual_data_len;
    int i;
    char *data_char_ptr = (char*)data;
    int tmp; /* Used to swap the data bytes */

    SOCDNX_INIT_FUNC_DEFS;

    FD_ZERO(&read_vect);
    FD_SET(cmodelInfo->cmodelMemRegSocFd, &read_vect);

    while (1) {
        if(select(cmodelInfo->cmodelMemRegSocFd+1,&read_vect,(fd_set*)0x0,(fd_set*)0x0,NULL)<0)
        {
            if (errno == EINTR) {
                /*
                 * Interrupted by a signal such as a GPROF alarm so
                 * restart the call
                 */
                continue;
            }
            perror("get_command: select error");
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            break;
        }
    }


    /*
     * Data ready to be read.
     */
    if( FD_ISSET(cmodelInfo->cmodelMemRegSocFd, &read_vect))
    {
        read_length = readn(cmodelInfo->cmodelMemRegSocFd, &(swapped_header[0]), MEM_RESPONSE_HEADER_SIZE_CMODEL);

        /* Read the length of the packet */
        if(read_length < MEM_RESPONSE_HEADER_SIZE_CMODEL) {
            cli_out("cmodel_read_buffer: could not read packet length\n");
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /* Read opcode */
        opcode = *((int8*)&(swapped_header[0]));

        /* Read length */
        sal_memcpy(&nw_order_int, &swapped_header[1], sizeof(int));
        actual_data_len = ntohl(nw_order_int) - 5; /* actual_data_len is only the data length without the block id field and the RTL address field */

        /* Read block_id */
        block_id = *((int8*)&(swapped_header[5]));

        /* Read RTL address */
        sal_memcpy(&nw_order_int, &swapped_header[6], sizeof(int));
        rtl_address = ntohl(nw_order_int);


        LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("cmodel_memreg_read_response: opcode=%d actual_data_len=%d block_id=%d rtl_address=%d\n"), opcode, actual_data_len, block_id, rtl_address));

        read_length = readn(cmodelInfo->cmodelMemRegSocFd, data, actual_data_len);

        /* Read the data field according to the length */
        /* if(read_length < actual_data_len || data_len < actual_data_len) { */
        if(read_length < actual_data_len) {
            cli_out("cmodel_memreg_read_response: could not read data\n");
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /* The bytes should be swapped. The bytes are writen swapped and should be swapped again on the way from the c model's server */
        half_actual_data_len = actual_data_len >> 1;
        for (i=0; i < half_actual_data_len; i++)
        {
            tmp = data_char_ptr[i];
            data_char_ptr[i] = data_char_ptr[actual_data_len - 1 - i];
            data_char_ptr[actual_data_len - 1 - i] = tmp;
        }

    }
    else
    {
        rv = SOC_E_TIMEOUT;
        SOCDNX_IF_ERR_EXIT(rv);; /* Time expire with no command */
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Send a command to the C model. This function serves read/write and memories/registers
 *
 * The message's format to receive data from the C model's server is:
 * GET:
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (1 byte) | RTL Address (4 bytes)
 * GET reply
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (1 byte) | RTL Address (4 bytes) | Data (variable length)
 *
 * SET:
 * Opcode (1 byte) | Data Length (4 bytes) | Block ID (1 byte) | RTL Address (4 bytes) | Data (variable length)
 *
 * Data Length includes only the field after it.
 */
int cmodel_memreg_access(int unit,
                                int cmic_block,
                                uint32 addr,
                                uint32 dwc_read,
                                int is_mem,
                                int is_get,
                                uint32 *data)
{
    /* char nw_order_str[sizeof(uint32)+1]; */
    uint32 nw_order_int;
    uint8 opcode;
    uint32 host_order_int;
    char packet_data[MAX_PACKET_SIZE_CMODEL];
    char *data_char_ptr;
    int write_index = 0;
    int data_len = 0;
    int rv = SOC_E_NONE;
    uint8 rounded_data_length = 0;
    uint8 padding_length = 0;
    uint8 last_dword_length = 0;  /* (padding_length + last_word_length) % 4 == 0. The 0 padding is added before the data of the last DWORD */
    int i=0;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("cmodel_memreg_access: unit=%d cmic_block=%d addr=0x%x dwc_read=%d is_mem=%d is_get=%d\n"), unit, cmic_block, addr, dwc_read, is_mem, is_get));

    sal_mutex_take(cmodelInfo->cmodelMemRegMutex, sal_mutex_FOREVER);

    /* Round up to the closest multiple of 4 */
    rounded_data_length = ((uint8)dwc_read + 3) & ~0x3;

    /* Add Opcode according to Set/Get and Table/Register */
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
        /* In case of writes to table or register the len should include the data length */
        data_len = dwc_read;
        padding_length = rounded_data_length - dwc_read;
        last_dword_length = (4 - padding_length) % 4;

        if (is_mem)
        {
            opcode = UPDATE_FRM_SOC_WRITE_TBL;
        }
        else
        {
            opcode = UPDATE_FRM_SOC_WRITE_REG;
        }
    }

    packet_data[write_index] = opcode;
    write_index += 1;

    /* Add data length ( sizeof(Block ID) + sizeof(RTL Address) + sizeof(Data) ) */
    host_order_int = 1 + 4 + data_len + padding_length;

    /* Expected result length should be added to registers and not tables */
    if (is_get && !is_mem) {
        host_order_int++; /* Add sizeof(expected data field) */
    }
    nw_order_int = ntohl(host_order_int);
    sal_memcpy(&(packet_data[write_index]), &nw_order_int, sizeof(int));
    write_index += sizeof(int);

    /* Add Block ID (1 byte) */
    packet_data[write_index] = (uint8)cmic_block;
    write_index += 1;

    /* Add RTL Address (4 bytes) */
    host_order_int = addr;
    nw_order_int = ntohl(host_order_int);
    sal_memcpy(&(packet_data[write_index]), &nw_order_int, sizeof(int));
    write_index += sizeof(int);

    /* Expected result length should be added to registers and not tables */
    /* When doing a get the packet should describe how many bytes are expected */
    if (is_get && !is_mem) {
        packet_data[write_index] = (uint8)rounded_data_length;
        write_index += 1;
    }

    /* Add data for set command */
    if (!is_get) {
        data_char_ptr = (char *)data;

        /* Make sure that the packet is not too long */
        if (MAX_PACKET_SIZE_CMODEL < rounded_data_length + write_index)
        {
            cli_out("Error: cmodel_memreg_access packet too long rounded_data_length=%d\n", rounded_data_length);
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        /* Memories/Registers are writen in a reverse order. The LSB is writen first since the padding is done on the MSB */
        for (i=0; i<rounded_data_length; i++)
        {
            packet_data[write_index+i] = data_char_ptr[(rounded_data_length-1) - i];
        }
        write_index += rounded_data_length;
    }

    /* write the packet to the socket */
    if (writen(cmodelInfo->cmodelMemRegSocFd, packet_data, write_index) != write_index)
    {
        cli_out("Error: cmodel_memreg_access data failed\n");

        rv = SOC_E_FAIL;
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* In case of get memory or register wait for the value to return */
    if(is_get)
    {
        SOCDNX_IF_ERR_EXIT(cmodel_memreg_read_response(unit, is_mem, rounded_data_length, data));
    }

exit:
    sal_mutex_give(cmodelInfo->cmodelMemRegMutex);
    SOCDNX_FUNC_RETURN;
}


/* Get register's size, address and block and then call the access function that serves the memories and registers */
STATIC int
cmodel_reg_access_handle(int unit, soc_reg_t reg, int port, int index, int is_get, soc_reg_above_64_val_t data)
{
    uint32 addr;
    int reg_size;
    int blk_id;
    uint8 acc_type;

    /* reg validity check */
    if(reg >= NUM_SOC_REG) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid register")));
        return SOC_E_INTERNAL;
    }

    addr = soc_reg_addr_get(unit, reg, port, index, SOC_REG_ADDR_OPTION_NONE,
                            &blk_id, &acc_type);
    if(addr <= 0) {
        return SOC_E_MEMORY;
    }

    reg_size = soc_reg_bytes(unit,reg);

    return cmodel_memreg_access(unit, blk_id, addr, reg_size, 0, is_get, (uint32*)data);
}

STATIC int
cmodel_mem_access_handle(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, int is_read, void *entry_data)
{
    uint32 data_byte_len, addr;
    int cmic_blk, blk;
    uint8 acc_type;
    int copyno_override = 0;
    int entry_dw;
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if(mem >= NUM_SOC_REG) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid memory")));
        return SOC_E_INTERNAL;
    }

    entry_dw = soc_mem_entry_words(unit, mem);
    data_byte_len = entry_dw * sizeof (uint32);


    _soc_mem_write_copyno_update(unit, mem, &copyno, &copyno_override);

    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {
        if (copyno_override) {
            blk = copyno = copyno_override;
        } else if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }

        cmic_blk = SOC_BLOCK2SCH(unit, blk);

        addr = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);

        rv = cmodel_memreg_access(unit, cmic_blk, addr, data_byte_len, 1, is_read, (uint32*)entry_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
cmodel_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32* data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    rv = cmodel_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0) {
        return rv;
    }

    data[0] = data_a64[0];

    return SOC_E_NONE;
}

STATIC int
cmodel_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64* data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    rv = cmodel_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0) {
        return rv;
    }

    COMPILER_64_SET(*data, data_a64[1], data_a64[0]);

    return SOC_E_NONE;
}

STATIC int
cmodel_reg_above64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    return cmodel_reg_access_handle(unit, reg, port, index, 1, data);
}

STATIC int
cmodel_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    soc_reg_above_64_val_t data_a64;

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[0] = data;

    return cmodel_reg_access_handle(unit, reg, port, index, 0, data_a64);
}

STATIC int
cmodel_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    soc_reg_above_64_val_t data_a64;

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[1] = COMPILER_64_HI(data);
    data_a64[0] = COMPILER_64_LO(data);

    return cmodel_reg_access_handle(unit, reg, port, index, 0, data_a64);
}

STATIC int
cmodel_reg_above64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    return cmodel_reg_access_handle(unit, reg, port, index, 0, data);
}

STATIC int
cmodel_mem_array_read(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data)
{
    return cmodel_mem_access_handle(unit, mem, array_index, copyno, index, 1, entry_data);
}

STATIC int
cmodel_mem_array_write(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data)
{
    return cmodel_mem_access_handle(unit, mem, array_index, copyno, index, 0, entry_data);
}

/* Allocate the sockets for the cmodel.
 * is_regs = 1 allocates the socket for the registers/memories. is_regs = 0 is used for packets.
 */
int _init_cmodel_sockets(int unit, int is_regs)
{
   /* struct sockaddr_in cli_addr; */
   /*  socklen_t cli_addr_size; */
    struct sockaddr_in srv_addr;
#ifndef VXWORKS
    struct hostent *hostentPtr = NULL;
#endif
    char *s = NULL;
    char *cmodelHost = NULL;
    char tmp[80];
    int cmodelHostPort;
    int optval = 1;
    int socFd;
    int rv = SOC_E_NONE;

SOCDNX_INIT_FUNC_DEFS;

    /* Allocate mutex to control the socket's handling */
    if (is_regs)
    {
        cmodelInfo->cmodelMemRegMutex = sal_mutex_create("C model registers socket mutex");

        s = getenv("CMODEL_MEMORY_PORT");

        if (!s)
        {
            rv = SOC_E_INIT;
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        cmodelInfo->cmodelRxTxMutex = sal_mutex_create("C model RxTx socket mutex");

        /* Setup C model's server port */
        s = getenv("CMODEL_PACKET_PORT");
        if (!s)
        {
            rv = SOC_E_INIT;
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    cmodelHost = getenv("SOC_TARGET_SERVER");

    /* Get C model's server name */
    /* Setup target host */
    if (!cmodelHost)
    {
        snprintf(tmp, sizeof(tmp), "SOC_TARGET_SERVER%d", 0/*devNo*/);
        cmodelHost = getenv(tmp);
    }

    /* Allocate a socket to the C model's server. This socket will be used for the RxTx packet transmission */
    /* Wait until the C model's server is up */
    cmodelHostPort = atoi(s);

    memset((void *)&srv_addr, 0, sizeof(srv_addr));



#ifdef VXWORKS
    if (!isdigit((unsigned)*cmodelHost))
    {
        cli_out("cmodel_reg_access_init: can't resolve host names in vxworks\n");
    }

    srv_addr.sin_addr.s_addr = inet_addr(cmodelHost);
#else
    hostentPtr = gethostbyname(cmodelHost);
    if(hostentPtr == NULL) {
     /*   cli_out("cmodel_reg_access_init: hostname lookup failed for host [%s].\n", cmodelHost);
        perror("gethostbyname"); */
        rv = SOC_E_INIT;
        SOCDNX_IF_ERR_EXIT(rv);
    }
    memcpy(&srv_addr.sin_addr,hostentPtr->h_addr,4);
#endif

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(cmodelHostPort);

    if (is_regs)
    {
        if ((cmodelInfo->cmodelMemRegSocFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
        /*    perror("server: can't open stream socket"); */
            rv = SOC_E_INIT;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        socFd = cmodelInfo->cmodelMemRegSocFd;
    }
    else
    {
        if ((cmodelInfo->cmodelRxTxSocFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
        /*    perror("server: can't open stream socket"); */
            rv = SOC_E_INIT;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        socFd = cmodelInfo->cmodelRxTxSocFd;
    }

    /* configure the socket to send the data immidiately */
    if (setsockopt(socFd, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval)) < 0 ) {
        rv = SOC_E_INIT;
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (connect(socFd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
    {
        rv = SOC_E_INIT;
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* Init sockets to the c model. One socket for registers and memories and the other for RxTx */
soc_error_t cmodel_reg_access_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (!cmodelInfo) {

        /* Allocate the static context parameter */
        cmodelInfo = sal_alloc(sizeof(cmodel_access_info_t), "cmodel access_info");

        if(!cmodelInfo) {
            return SOC_E_MEMORY;
        }

        sal_memset(cmodelInfo, 0, sizeof(cmodel_access_info_t));

        SOCDNX_IF_ERR_EXIT(_init_cmodel_sockets(unit, 1));

        SOCDNX_IF_ERR_EXIT(_init_cmodel_sockets(unit, 0));
    }

    /* Register callbacks for memory access */
    SOCDNX_IF_ERR_EXIT(soc_reg_access_func_register(unit, &cmodel_access));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Close the sockets and mutex to the c model's server */
int cmodel_reg_access_deinit(int unit)
{
    if(!cmodelInfo) {
        return SOC_E_INIT;
    }

    /* Close the socket to the C model's server */
    if (cmodelInfo->cmodelRxTxSocFd >= 0)
    {
        close(cmodelInfo->cmodelRxTxSocFd);
        cmodelInfo->cmodelRxTxSocFd = -1;
    }
    /* Free the mutex that control's the socket to the C model's server */
    if (cmodelInfo->cmodelRxTxMutex)
    {
        sal_mutex_destroy(cmodelInfo->cmodelRxTxMutex);
    }

    if (cmodelInfo->cmodelMemRegSocFd >= 0)
    {
        close(cmodelInfo->cmodelMemRegSocFd);
        cmodelInfo->cmodelMemRegSocFd = -1;
    }
    /* Free the mutex that control's the socket to the C model's server */
    if (cmodelInfo->cmodelMemRegMutex)
    {
        sal_mutex_destroy(cmodelInfo->cmodelMemRegMutex);
    }

    sal_free(cmodelInfo);
    cmodelInfo = NULL;

    return SOC_E_NONE;
}


/*
 * Reads a Tx buffer from the C model's server. Assumes that the buffer's length is a multiple of 4
 * Parameters:
 *  sockfd -socket file descriptor.
 *  Packet format: length (4 bytes), block ID (4 bytes), NOF signals (4 bytes), [signal ID (4 bytes), signal length (4 bytes), data]
 */
uint32 cmodel_read_buffer(int unit, cmodel_ms_id_e *ms_id, uint32* nof_signals, uint32 *src_port, int *len, unsigned char *buf)
{
    fd_set read_vect;
    char swapped_header[MAX_PACKET_HEADER_SIZE_CMODEL];
    uint32 packet_length;
    long long_val;
    int offset = 0;
    int nfds = 0;
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    assert(cmodelInfo->cmodelRxTxSocFd);

    LOG_VERBOSE(BSL_LS_SYS_VERINET,
                (BSL_META("cmodel_read_buffer: sockfd=%d\n"), cmodelInfo->cmodelRxTxSocFd));

    /* Setup bitmap for read notification ... */
    FD_ZERO(&read_vect);

    /* Add Cmodel Rx-Tx socket to selected fds */
    FD_SET(cmodelInfo->cmodelRxTxSocFd, &read_vect);

    /* Add read end of pipe to selected fds */
    FD_SET(pipe_fds[0], &read_vect);

    /* Set maximum fd */
    nfds = (cmodelInfo->cmodelRxTxSocFd > pipe_fds[0]) ? cmodelInfo->cmodelRxTxSocFd+1 : pipe_fds[0]+1;

    /* Listen to two files:
         - Cmodel Rx-Tx socket (for incoming packets)
         - Read end of pipe     (for thread exit notification)
       Once a file contains information to be read, we read it and process
       accordingly - handle packet or exit thread */
    while ((rv = select(nfds,&read_vect,(fd_set*)0x0,(fd_set*)0x0,NULL)) == -1 && errno == EINTR) {
        continue;
    }

    if (rv < 0) {
        perror("get_command: select error");
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Thread is about to be closed */
    if (FD_ISSET(pipe_fds[0], &read_vect)) {
        return RX_THREAD_NOTIFY_CLOSED;
    }

    /*
     * Data ready to be read.
     */
    if(FD_ISSET(cmodelInfo->cmodelRxTxSocFd, &read_vect))
    {
        /* Read the length of the packet */
        if(readn(cmodelInfo->cmodelRxTxSocFd, &(swapped_header[0]), MAX_PACKET_HEADER_SIZE_CMODEL) < MAX_PACKET_HEADER_SIZE_CMODEL) {
            cli_out("cmodel_read_buffer: could not read packet length\n");
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        }

        long_val = ntohl(*(uint32*)&(swapped_header[offset]));
        packet_length = long_val;
        offset += sizeof(uint32);

        long_val = ntohl(*(uint32*)&(swapped_header[offset]));
        *ms_id = long_val;
        offset += sizeof(uint32);

        long_val = ntohl(*(uint32*)&(swapped_header[offset]));
        *nof_signals = long_val;
        offset += sizeof(uint32);

        LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("cmodel_read_buffer: packet_length=%d ms_id=%d nof_signals=%d\n"), packet_length, *ms_id, *nof_signals));

        /* len includes the signal ID, signal length and the data length */
        *len = packet_length-2*sizeof(uint32);

        /* Read the Rx packet */
        if(readn(cmodelInfo->cmodelRxTxSocFd, buf, *len) < *len) {
            cli_out("cmodel_read_buffer: could not read packet\n");
            rv = SOC_E_FAIL;
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Client call: send packet to the cmodel's server
 *  unit - not in use
 *  ms_id - ID of the block in the C model
 *  src_port - not in use
 *  len - buf length in bytes
 *  not_signals - number of signals in buf
 *  Packet format: | length in bytes from ms_id(32 bits) | ms_id (32 bits) | nof_signals (32 bits) | SIGNALS (Signal id 32 bits | data length in bytes 32 bits | data )*
 */
uint32 cmodel_send_buffer(int unit, cmodel_ms_id_e ms_id, uint32 src_port, int len, unsigned char *buf, int nof_signals)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    uint32 buf_len_in_bits = len * 8;
    int index;
    char packet_data[MAX_PACKET_SIZE_CMODEL];

    LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("cmodel_send_buffer: src_port=%d len=0x%x\n"), src_port, len));

    sal_mutex_take(cmodelInfo->cmodelRxTxMutex, sal_mutex_FOREVER);

    /* Add packet's size. Not including the size field in the size */
    nw_order_int = ntohl(buf_len_in_bits + 4*sizeof(uint32)); /* Add the ms_id size and nof_signals size to the buffer's length. Add the signal ID and signal's length as well. */
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /* Add MS ID */
    nw_order_int = ntohl(ms_id);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /* Add number of signals */
    nw_order_int = ntohl(nof_signals);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /* Add signal ID */
    nw_order_int = ntohl(TX_PACKET_SIGNAL);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /* Add signal length in bytes. Each bit consumes a byte in the packet */
    nw_order_int = ntohl(buf_len_in_bits);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    /* Add buffer */
    /* Convert the buf to characters of '0' and '1' */
    for (index=0; index < len; index++)
    {
        sal_memcpy(&(packet_data[index_position + index*8]), conversion_tbl[(int)buf[index]], 8);
    }
    index_position += buf_len_in_bits;

    /* write the packet to the socket */
    if (writen(cmodelInfo->cmodelRxTxSocFd,packet_data, index_position) != index_position)
    {
        cli_out("Error: cmodel_send_packet data failed\n");
        sal_mutex_give(cmodelInfo->cmodelRxTxMutex);
        return -1;
    }

    sal_mutex_give(cmodelInfo->cmodelRxTxMutex);

    return 0;
}

#endif /* CMODEL_SERVER_MODE */

