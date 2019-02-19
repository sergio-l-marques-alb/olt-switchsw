/* 
 * $Id: mem.c,v 1.8 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * SOC memory access implementation for DPP
 */

#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_MEM

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/cmic.h>

/* this struct contains info about registers that provide indirect memory 
 * access for each block. The offsets and widths may differ for each block.
 */
typedef struct soc_dpp_mem_access_info_s {
    int     block;                  /* block number */
    int     block_offset;           /* block address offset */
    int     wr_reg_offset;          /* indirect_command_wr_data reg offset 
                                       within the block */
    int     rd_reg_offset;          /* indirect_command_rd_data reg offset 
                                       within the block */
    int     wr_rd_width;            /* bitwidth of indirect_command_rd/wr_data 
                                       reg offsets */
    int     cmd_offset;             /* indirect_command reg offset */
    int     cmd_addr_offset;        /* indirect_command_address offset */
    int     cmd_data_incr_offset;   /* indirect_cmd_data_increment offset */
} soc_dpp_mem_access_info_t;

CONST soc_dpp_mem_access_info_t soc_petrab_mem_access_info [] = {
    {SOC_BLK_CFC,   0x4600, 0x20,   0x30,   44,     0x40,   0x41,   0x42},
    {SOC_BLK_DPI,   0x0c00, 0x20,   0x30,   56,     0x40,   0x41,   0x0},
    {SOC_BLK_DRC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_ECI,   0x0,    0x40,   0x50,   36,     0x60,   0x61,   0x62},
    {SOC_BLK_EGQ,   0x5800, 0x20,   0x30,   256,    0x40,   0x41,   0x42},
    {SOC_BLK_EPNI,  0x3a00, 0x20,   0x30,   256,    0x40,   0x41,   0x42},
    {SOC_BLK_FCR,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_FCT,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MMU,   0x0a00, 0x20,   0x30,   512,    0x40,   0x41,   0x42},
    {SOC_BLK_FDR,   0x0,    0x0,    0x0,     0,     0x0,    0x0,    0x0},
    {SOC_BLK_FDT,   0x2c00, 0x0,    0x30,   244,    0x40,   0x41,   0x0},
    {SOC_BLK_MESH_TOPOLOGY,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_IDR,   0x2800, 0x20,   0x30,   245,    0x40,   0x41,   0x42},
    {SOC_BLK_IHB,   0x6400, 0x20,   0x30,   309,    0x40,   0x41,   0x42},
    {SOC_BLK_IHP,   0x6000, 0x20,   0x30,   309,    0x40,   0x41,   0x42},
    {SOC_BLK_IPS,   0x0200, 0x20,   0x30,   64,     0x40,   0x41,   0x42},
    {SOC_BLK_IPT,   0x0800, 0x20,   0x30,   512,    0x40,   0x41,   0x42},
    {SOC_BLK_IQM,   0x0400, 0x20,   0x30,   180,    0x40,   0x41,   0x42},
    {SOC_BLK_IRE,   0x2400, 0x20,   0x30,   128,    0x40,   0x41,   0x42},
    {SOC_BLK_IRR,   0x2a00, 0x20,   0x30,   0,      0x40,   0x41,   0x42},
    {SOC_BLK_MAC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MBU,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MCC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_NBI,   0x4a00, 0x20,   0x30,   128,    0x40,   0x41,   0x42},
    {SOC_BLK_NIF,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_OLP,   0x4400, 0x20,   0x30,   32,     0x40,   0x41,   0x42},
    {SOC_BLK_QDR,   0x0600, 0x20,   0x30,   32,     0x40,   0x41,   0x42},
    {SOC_BLK_RTP,   0x3600, 0x20,   0x30,   72,     0x40,   0x41,   0x42},
    {SOC_BLK_SCH,   0x4200, 0x20,   0x30,   32,     0x40,   0x41,   0x42}
};

#define SOC_PETRAB_MEM_ACCESS_INFO_LEN (sizeof(soc_petrab_mem_access_info)/ \
                                        sizeof(soc_dpp_mem_access_info_t))

#define SOC_PETRAB_INDIRECT_CMD_TYPE_READ       (0x80000000)
#define SOC_PETRAB_INDIRECT_CMD_TYPE_WRITE_MASK (0x7fffffff)
#define SOC_PETRAB_INDIRECT_CMD_TRIGGER         (0x1)
#define SOC_PETRAB_INDIRECT_CMD_TIMEOUT         (10000000)  /* 10 seconds */
int
_soc_dpp_indirect_mem_access_info_get(int unit, soc_mem_t mem, 
                                CONST soc_dpp_mem_access_info_t **pp_mem_access_info)
{
    int             idx;
    soc_block_t     blk;
    
    if (pp_mem_access_info == NULL) {
        return SOC_E_PARAM;
    }

    blk = SOC_MEM_BLOCK_ANY(unit, mem);
    if ((blk <= 0) || (blk > SOC_MAX_NUM_BLKS)) {
        return SOC_E_INTERNAL;
    }
    for (idx = 0; idx < SOC_PETRAB_MEM_ACCESS_INFO_LEN; idx++) {
        if (soc_petrab_mem_access_info[idx].block == 
            (SOC_BLOCK_INFO(unit, blk).type)) {
            if (soc_petrab_mem_access_info[idx].wr_rd_width == 0) {
                return SOC_E_PARAM;
            }
            *pp_mem_access_info = &soc_petrab_mem_access_info[idx];
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

int
_soc_dpp_indirect_mem_access_trigger(int unit, int trig_offset)
{
    int     rv;
    uint32          data;
    sal_usecs_t     cur_time, start_time;

    /* Read-Modify-Write */
    rv = soc_reg32_read(unit, trig_offset, &data);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access failed(%s). "
                             "Unable to trigger op\n"), soc_errmsg(rv)));
        return rv;
    }
    if (data & SOC_PETRAB_INDIRECT_CMD_TRIGGER) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access aborted. "
                             "trigger already set.\n")));
        return SOC_E_BUSY;
    }

    data |= SOC_PETRAB_INDIRECT_CMD_TRIGGER;
    rv = soc_reg32_write(unit, trig_offset, data);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access failed(%s). "
                             "Unable to trigger op\n"), soc_errmsg(rv)));
        return rv;
    }

    /* wait for completion */
    start_time = sal_time_usecs();
    do {
        rv = soc_reg32_read(unit, trig_offset, &data);
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_MEM,
                     (BSL_META_U(unit,
                                 "Indirect memory access failed(%s). "
                                 "Unable to read trigger status\n"), soc_errmsg(rv)));
            return rv;
        }
        if (data & SOC_PETRAB_INDIRECT_CMD_TRIGGER) {
           LOG_VERBOSE(BSL_LS_SOC_MEM,
                       (BSL_META_U(unit,
                                   "Waiting for trigger operation"
                                   " to finish \n")));
            sal_usleep(50);
        } else {
            return SOC_E_NONE;
        }
        cur_time = sal_time_usecs();
    } while ((cur_time - start_time) < SOC_PETRAB_INDIRECT_CMD_TIMEOUT);

    return SOC_E_TIMEOUT;
}

int
_soc_dpp_indirect_mem_access(int unit, soc_mem_t mem, int copyno, int index, 
                             void *entry_data, int mem_op)
{
    soc_mem_info_t              meminfo = SOC_MEM_INFO(unit, mem);
    CONST soc_dpp_mem_access_info_t   *p_acc_info;
    int                         words_to_access, words_accessed;
    int                         rv;
    uint32                      data, tbl_entry_addr;
    uint32                      addr_reg_offset, data_buf_offset, trig_offset;

    rv = _soc_dpp_indirect_mem_access_info_get(unit, mem, &p_acc_info);
    if (SOC_FAILURE(rv)) {
        return SOC_E_PARAM; /* no indirect access support for given memory */
    }

    /* calculate entry address within block */
    words_to_access = BYTES2WORDS(meminfo.bytes);
    tbl_entry_addr = (meminfo.base & 0xffffff) + (words_to_access * index);
    switch (mem_op) {
    case MEM_TABLE_READ:
        tbl_entry_addr |= SOC_PETRAB_INDIRECT_CMD_TYPE_READ;

        /* register-offset that indicates where the data is read from */
        data_buf_offset = (p_acc_info->block_offset + 
                            p_acc_info->rd_reg_offset);
        break;
    case MEM_TABLE_WRITE:
        tbl_entry_addr &= SOC_PETRAB_INDIRECT_CMD_TYPE_WRITE_MASK;

        /* register-offset that indicates where to write data */
        data_buf_offset = (p_acc_info->block_offset + 
                           p_acc_info->wr_reg_offset);
        break;
    default:
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Invalid operation specified\n")));
        return SOC_E_PARAM;
    }
    /* register-offset that indicates which address to rd/write */
    addr_reg_offset = (p_acc_info->block_offset + 
                       p_acc_info->cmd_addr_offset);

    /* register-offset of the trigger register for the block */
    trig_offset = (p_acc_info->block_offset + 
                   p_acc_info->cmd_offset);

    words_accessed = 0;
    while (words_accessed < words_to_access) {
        if (mem_op == MEM_TABLE_READ) {
            /* write the address to be read */
            data = tbl_entry_addr + words_accessed;
            rv = soc_reg32_write(unit, addr_reg_offset, data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to set read address\n"), soc_errmsg(rv)));
                return rv;
            }

            /* trigger read op */
            rv = _soc_dpp_indirect_mem_access_trigger(unit, trig_offset);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Trigger operation failed.\n"), soc_errmsg(rv)));
                return rv;
            }

            /* read result */
            rv = soc_reg32_read(unit, data_buf_offset, &data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to read value\n"), soc_errmsg(rv)));
                return rv;
            }
            sal_memcpy((((char *)entry_data) + (words_accessed * sizeof(data))), 
                       &data,
                       sizeof(data));
        } else {
            /* MEM_TABLE_WRITE */
            /* write the value of the entry */
            sal_memcpy(&data,
                       (((char *)entry_data) + (words_accessed * sizeof(data))),
                       sizeof(data));
            rv = soc_reg32_write(unit, data_buf_offset, data);
           LOG_VERBOSE(BSL_LS_SOC_MEM,
                       (BSL_META_U(unit,
                                   "Writing data:0x%x to memory "
                                   "address 0x%x\n"),data, data_buf_offset));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to write value\n"), soc_errmsg(rv)));
                return rv;
            }

            /* program the address to be written to */
            data = tbl_entry_addr + words_accessed;
            rv = soc_reg32_write(unit, addr_reg_offset, data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to set write address\n"), soc_errmsg(rv)));
                return rv;
            }

            /* trigger write op */
            rv = _soc_dpp_indirect_mem_access_trigger(unit, trig_offset);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Trigger operation failed.\n"), soc_errmsg(rv)));
                return rv;
            }
        }
        words_accessed++;
    }

    return rv;
}

int
_soc_dpp_mem_access_valid(int unit, soc_mem_t mem, int copyno, int index)
{
    if (!soc_mem_is_valid(unit, mem)) {
        return SOC_E_MEMORY;
    }
    if (copyno == MEM_BLOCK_ANY) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "invalid block %d for memory %s\n"),
                             copyno, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }
    if (!((index >= 0) && (index <= soc_mem_index_max(unit, mem)))) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "invalid index %d for memory %s\n"),
                             index, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_dpp_mem_read(int unit, soc_mem_t mem, int copyno, int index, void *entry_data)
{
    int rv;

    if (entry_data == NULL) {
        return SOC_E_PARAM;
    }

    rv = _soc_dpp_mem_access_valid(unit, mem, copyno, index);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s failed: Invalid inputs\n"), FUNCTION_NAME()));
        return rv;
    }

    rv = _soc_dpp_indirect_mem_access(unit, mem, copyno, index, entry_data, 
                                      MEM_TABLE_READ);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "soc_mem_read operation failed to read from "
                             "index %d memory %s \n"), index, SOC_MEM_NAME(unit, mem)));
    }

    return rv;
}

int
soc_dpp_mem_write(int unit, soc_mem_t mem, int copyno, int index, void *entry_data)
{
    int rv;

    if (entry_data == NULL) {
        return SOC_E_PARAM;
    }

    rv = _soc_dpp_mem_access_valid(unit, mem, copyno, index);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s failed: Invalid inputs\n"), FUNCTION_NAME()));
        return rv;
    }

    rv = _soc_dpp_indirect_mem_access(unit, mem, copyno, index, entry_data, 
                                      MEM_TABLE_WRITE);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s operation failed to write to "
                             "index %d memory %s \n"), FUNCTION_NAME(), index, 
                  SOC_MEM_NAME(unit, mem)));
    }

    return rv;
}


/*
 * Function:
 *      soc_mem_generic_insert
 * Purpose:
 *      Insert an entry
 * Parameters:
 *      unit - unit #
 *      entry - entry to insert
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_EXISTS - existing entry was replaced
 *      SOC_E_FULL - full
 *      SOC_E_BUSY - modfifo full
 * Notes:
 *      Uses hardware insertion; sends an INSERT message over the
 *      S-Channel.
 */
#ifdef BCM_DNX_P3_SUPPORT
int
soc_dpp_p3_mem_insert(int unit, soc_mem_t mem, int copyno,
                       void *entry)
{
    schan_msg_t schan_msg;
    int         rv;
    int         entry_dw = 0; 
    uint8       at;
    int         type, err_info, index, allow_intr=0;
    int         src_blk, dst_blk, acc_type, data_byte_len;
    int         opcode, nack;

    assert(SOC_MEM_IS_VALID(unit, mem));
    assert(soc_attached(unit));

    if (copyno == MEM_BLOCK_ANY) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
        LOG_WARN(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "soc_mem_generic_insert: invalid block %d for memory %s\n"),
                  copyno, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Insert table[%s]"), SOC_MEM_NAME(unit, mem)));
        if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityVerbose, unit)) {
            soc_mem_entry_dump(unit, mem, entry);
        }
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "\n")));
    }

    entry_dw = soc_mem_entry_words(unit, mem);


    schan_msg_clear(&schan_msg);

    acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));    
    dst_blk = SOC_BLOCK2SCH(unit, copyno);
    data_byte_len = entry_dw * 4;

    schan_msg.gencmd.address = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);

    soc_schan_header_cmd_set(unit, &schan_msg.header, TABLE_INSERT_CMD_MSG,
                dst_blk, src_blk, acc_type, data_byte_len, 0, 0);

    /* Fill in packet data */
    sal_memcpy(schan_msg.gencmd.data, entry, entry_dw * 4);

    if (SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /*
     * Execute S-Channel "table insert" command packet consisting of
     * (header word + address word + entry_dw), and read back
     * (header word + response word + entry_dw) data words.
     */
    if (2 + entry_dw > CMIC_SCHAN_WORDS(unit)) {
       LOG_WARN(BSL_LS_SOC_MEM,
                (BSL_META_U(unit,
                            "soc_mem_generic_insert: assert will fail for memory %s\n"),
                 SOC_MEM_NAME(unit, mem)));
    }
    rv = soc_schan_op(unit, &schan_msg, entry_dw + 2, entry_dw + 2, allow_intr);

    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
        NULL, NULL, &nack);
    if (opcode != TABLE_DELETE_DONE_MSG) {
        LOG_ERROR(BSL_LS_SOC_MEM,
                  (BSL_META_U(unit,
                              "soc_mem_generic_insert: "
                              "invalid S-Channel reply, expected TABLE_INSERT_DONE_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, 1);
        return SOC_E_INTERNAL;
    }

    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
        type = schan_msg.genresp_v2.response.type;
        err_info = schan_msg.genresp_v2.response.err_info;
        index = schan_msg.genresp_v2.response.index;
    } else {
        type = schan_msg.genresp.response.type;
        err_info = schan_msg.genresp.response.err_info;
        index = schan_msg.genresp.response.index;
    }
    if ((nack != 0) || (rv == SOC_E_FAIL)) {
        if (type == SCHAN_GEN_RESP_TYPE_FULL) {
            LOG_INFO(BSL_LS_SOC_MEM,
                     (BSL_META_U(unit,
                                 "Insert table[%s]: hash bucket full\n"),
                                 SOC_MEM_NAME(unit, mem)));
            rv = SOC_E_FULL;
        } else if (type == SCHAN_GEN_RESP_TYPE_ERROR) {

            if (err_info == SCHAN_GEN_RESP_ERROR_BUSY) {
                LOG_INFO(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Insert table[%s]: Modfifo full\n"),
                                     SOC_MEM_NAME(unit, mem)));
                rv = SOC_E_BUSY;
            } else if (err_info == SCHAN_GEN_RESP_ERROR_PARITY) {
                LOG_ERROR(BSL_LS_SOC_MEM,
                          (BSL_META_U(unit,
                                      "Insert table[%s]: Parity Error Index %d\n"),
                           SOC_MEM_NAME(unit, mem), index));
                rv = SOC_E_INTERNAL;
            }
        } else {
            rv = SOC_E_FAIL;
        }
    } 
    return rv;
}



/*
 * Function:
 *      soc_dpp_p3_mem_lkup
 * Purpose:
 *      Send a lookup message over the S-Channel and receive the response.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      key - entry to look up
 *      result - entry to receive entire found entry
 *      index_ptr (OUT) - If found, receives table index where found
 * Returns:
 *      SOC_E_INTERNAL if retries exceeded or other internal error
 *      SOC_E_NOT_FOUND if the entry is not found.
 *      SOC_E_NONE (0) on success (entry found):
 */

int
soc_dpp_p3_mem_lkup(int unit, soc_mem_t mem, int copyno, 
                       void *key_result)
{
    schan_msg_t schan_msg;
    int         rv, entry_dw, allow_intr=0;
    uint8       at;
    int         type, index, index_valid;
    uint32      *data;
    int *index_ptr = NULL;/* not needed currently*/
    int         src_blk, dst_blk, acc_type, data_byte_len;
    int         opcode, nack;
        

    assert(SOC_MEM_IS_VALID(unit, mem));
    assert(soc_attached(unit));

    if(key_result == NULL){
        LOG_WARN(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "key_result is NULL\n")));
        return SOC_E_PARAM;
    }

    if (copyno == MEM_BLOCK_ANY) {
        /* coverity[overrun-local : FALSE] */
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
        LOG_WARN(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "soc_mem_generic_lookup: invalid block %d for memory %s\n"),
                  copyno, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    schan_msg_clear(&schan_msg);

    acc_type  = SOC_MEM_ACC_TYPE(unit, mem);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    dst_blk = SOC_BLOCK2SCH(unit, copyno);
    data_byte_len = entry_dw * 4;

    schan_msg.gencmd.address = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);

    soc_schan_header_cmd_set(unit, &schan_msg.header, TABLE_LOOKUP_CMD_MSG,
            dst_blk, src_blk, acc_type, data_byte_len, 0, 0);

    /* Fill in entry data */
    sal_memcpy(schan_msg.gencmd.data, key_result, entry_dw * 4);

    if (SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /*
     * Execute S-Channel "lookup insert" command packet consisting of
     * (header word + address word + entry_dw), and read back
     * (header word + response word + entry_dw) data words.
     */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 2, entry_dw + 2, allow_intr);

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, &nack);
    if (opcode != TABLE_LOOKUP_DONE_MSG) {
        LOG_ERROR(BSL_LS_SOC_MEM,
                  (BSL_META_U(unit,
                              "soc_mem_generic_lookup: "
                              "invalid S-Channel reply, expected TABLE_LOOKUP_DONE_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, entry_dw + 2);
        return SOC_E_INTERNAL;
    }

    if (soc_feature(unit, soc_feature_new_sbus_format) &&
        !soc_feature(unit, soc_feature_new_sbus_old_resp)) {
        type = schan_msg.genresp_v2.response.type;
        index = schan_msg.genresp_v2.response.index;
        data = schan_msg.genresp_v2.data;
    } else {
        type = schan_msg.genresp.response.type;
        index = schan_msg.genresp.response.index;
        data = schan_msg.genresp.data;
    }

    if ((nack != 0) || (rv == SOC_E_FAIL)) {
        if (index_ptr) {
            *index_ptr = -1;
        }
        if (type == SCHAN_GEN_RESP_TYPE_NOT_FOUND) {
            rv = SOC_E_NOT_FOUND;

        } else {
			rv = SOC_E_FAIL;
        }
    } else {
        if (key_result != NULL) {
            sal_memcpy(key_result, data, entry_dw * sizeof(uint32));
        }
        if (index_ptr != NULL) {
            *index_ptr = index;
        }
        index_valid = (index >= 0 &&
                           index <= soc_mem_index_max(unit, mem));
        if (!index_valid) {
            LOG_ERROR(BSL_LS_SOC_MEM,
                      (BSL_META_U(unit,
                                  "soc_mem_generic_lookup: "
                                  "invalid index %d for memory %s\n"),
                       index, SOC_MEM_NAME(unit, mem)));
            rv = SOC_E_INTERNAL;
        }
    }
    if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Lookup table[%s]:"), SOC_MEM_NAME(unit, mem)));
        if (bsl_check(bslLayerSoc, bslSourceMem, bslSeverityVerbose, unit)) {
            soc_mem_entry_dump(unit, mem, data);
        }
        if (SOC_FAILURE(rv)) {
            if (type == SCHAN_GEN_RESP_TYPE_NOT_FOUND) {
                LOG_INFO(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     " Not found\n")));
            } else {
                LOG_INFO(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     " Fail\n")));
            }
        } else {
            LOG_INFO(BSL_LS_SOC_MEM,
                     (BSL_META_U(unit,
                                 " (index=%d)\n"), index));
        }
    }

    return rv;
}


int
soc_dpp_p3_mem_delete(int unit, soc_mem_t mem, int copyno, 
                       void *key)
{
    schan_msg_t schan_msg;
    int         rv, entry_dw, allow_intr=0;
    uint8       at;
    int         src_blk, dst_blk, acc_type, data_byte_len;
    int         opcode, nack;

    assert(SOC_MEM_IS_VALID(unit, mem));
    assert(soc_attached(unit));

    if (copyno == MEM_BLOCK_ANY) {
        /* coverity[overrun-local : FALSE] */
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
        LOG_WARN(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "soc_mem_generic_lookup: invalid block %d for memory %s\n"),
                  copyno, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    schan_msg_clear(&schan_msg);

    acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    dst_blk = SOC_BLOCK2SCH(unit, copyno);
    data_byte_len = entry_dw * 4;

    schan_msg.gencmd.address = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);

    soc_schan_header_cmd_set(unit, &schan_msg.header, TABLE_DELETE_CMD_MSG,
                         dst_blk, src_blk, acc_type, data_byte_len, 0, 0); 

    /* Fill in entry data */
    sal_memcpy(schan_msg.gencmd.data, key, entry_dw * 4);

    if (SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /*
     * Execute S-Channel "lookup insert" command packet consisting of
     * (header word + address word + entry_dw), and read back
     * (header word + response word + entry_dw) data words.
     */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 2, entry_dw + 2, allow_intr);
    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, &nack);
    if (opcode != TABLE_DELETE_DONE_MSG) {
        LOG_ERROR(BSL_LS_SOC_MEM,
                  (BSL_META_U(unit,
                              "soc_mem_generic_delete: "
                              "invalid S-Channel reply, expected TABLE_LOOKUP_DELETE_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, entry_dw + 2);
        return SOC_E_INTERNAL;
    }

    return rv;
}

#endif /*BCM_DNX_P3_SUPPORT*/
