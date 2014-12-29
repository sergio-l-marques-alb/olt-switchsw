/*
 * $Id: schan.c,v 1.53 Broadcom SDK $
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
 * S-Channel (internal command bus) support
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>

#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif
#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif
#ifdef BCM_KATANA_SUPPORT
#include <soc/katana.h>
#endif
#include <soc/cmicm.h>

#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)
/* Back door into the simulation to perform direct SCHANNEL operations */
extern uint32 plibde_schan_op(int unit, schan_msg_t* msg, int, int);
#endif

#ifdef _SER_TIME_STAMP
sal_usecs_t ser_time_1;
#endif

#define IS_WRITE_ALLOWED_DURING_WB(unit) \
    (SOC_CONTROL(unit)->schan_wb_thread_id == sal_thread_self())

/*
 * S-Chanel operation names
 */

STATIC char *_soc_schan_op_names[] = {
    "UNKNOWN_OPCODE",
    "BP_WARN_STATUS",           /* 0x01 */
    "BP_DISCARD_STATUS",        /* 0x02 */
    "COS_QSTAT_NOTIFY",         /* 0x03 */
    "HOL_STAT_NOTIFY",          /* 0x04 */
    "",                         /* 0x05 */
    "",                         /* 0x06 */
    "READ_MEM_CMD",             /* 0x07 */
    "READ_MEM_ACK",             /* 0x08 */
    "WRITE_MEM_CMD",            /* 0x09 */
    "WRITE_MEM_ACK",            /* 0x0a */
    "READ_REG_CMD",             /* 0x0b */
    "READ_REG_ACK",             /* 0x0c */
    "WRITE_REG_CMD",            /* 0x0d */
    "WRITE_REG_ACK",            /* 0x0e */
    "ARL_INSERT_CMD",           /* 0x0f */
    "ARL_INSERT_DONE",          /* 0x10 */
    "ARL_DELETE_CMD",           /* 0x11 */
    "ARL_DELETE_DONE",          /* 0x12 */
    "LINKSTAT_NOTIFY",          /* 0x13 */
    "MEM_FAIL_NOTIFY",          /* 0x14 */
    "INIT_CFAP",                /* 0x15 */
    "",                         /* 0x16 */
    "ENTER_DEBUG_MODE",         /* 0x17 */
    "EXIT_DEBUG_MODE",          /* 0x18 */
    "ARL_LOOKUP_CMD",           /* 0x19 */
    "L3_INSERT_CMD",            /* 0x1a */
    "L3_INSERT_DONE",           /* 0x1b */
    "L3_DELETE_CMD",            /* 0x1c */
    "L3_DELETE_DONE",           /* 0x1d */
    "L3_LOOKUP_CMD",            /* 0x1e */
    "UNKNOWN_OPCODE",           /* 0x1f */
    "L2_LOOKUP_CMD_MSG",        /* 0x20 */
    "L2_LOOKUP_ACK_MSG",        /* 0x21 */
    "L3X2_LOOKUP_CMD_MSG",      /* 0x22 */
    "L3X2_LOOKUP_ACK_MSG",      /* 0x23 */
    "TABLE_INSERT_CMD_MSG",     /* 0x24 */
    "TABLE_INSERT_DONE_MSG",    /* 0x25 */
    "TABLE_DELETE_CMD_MSG",     /* 0x26 */
    "TABLE_DELETE_DONE_MSG",    /* 0x27 */
    "TABLE_LOOKUP_CMD_MSG",     /* 0x28 */
    "TABLE_LOOKUP_DONE_MSG",    /* 0x29 */
    "FIFO_POP_CMD_MSG",         /* 0x2a */
    "FIFO_POP_DONE_MSG",        /* 0x2b */
    "FIFO_PUSH_CMD_MSG",        /* 0x2c */
    "FIFO_PUSH_DONE_MSG",       /* 0x2d */
};

char *
soc_schan_op_name(int op)
{
    if (op < 0 || op >= COUNTOF(_soc_schan_op_names)) {
        op = 0;
    }

    return _soc_schan_op_names[op];
}

#if defined(BCM_TRIUMPH_SUPPORT)
STATIC char *_soc_schan_gen_resp_type_names[] = {
    "FOUND",          /*  0 */
    "NOT_FOUND",      /*  1 */
    "FULL",           /*  2 */
    "INSERTED",       /*  3 */
    "REPLACED",       /*  4 */
    "DELETED",        /*  5 */
    "ENTRY_IS_OLD",   /*  6 */
    "CLEARED_VALID",  /*  7 */
    "L2_FIFO_FULL",   /*  8 */
    "MAC_LIMIT_THRE", /*  9 */
    "MAC_LIMIT_DEL",  /* 10 */
    "L2_STATIC",      /* 11 */
    "UNKNOWN",        /* 12 */
    "UNKNOWN",        /* 13 */
    "UNKNOWN",        /* 14 */
    "ERROR"           /* 15 */
};

STATIC char *
soc_schan_gen_resp_type_name(int type)
{
    if (type < 0 || type >= COUNTOF(_soc_schan_gen_resp_type_names)) {
        type = 8;
    }

    return _soc_schan_gen_resp_type_names[type];
}

STATIC char *_soc_schan_gen_resp_err_names[] = {
    "NONE",            /* 0 */
    "SRAM_P_ERR",      /* 1 */
    "TCAM_SRCH_ERR",   /* 2 */
    "MULTIPLE",        /* 3 */
    "TCAM_RD_ERR",     /* 4 */
    "MULTIPLE",        /* 5 */
    "MULTIPLE",        /* 6 */
    "MULTIPLE",        /* 7 */
    "TCAM_SEQ_ERR",    /* 8 */
    "MULTIPLE",        /* 9 */
    "MULTIPLE",        /* 10 */
    "MULTIPLE",        /* 11 */
    "MULTIPLE",        /* 12 */
    "MULTIPLE",        /* 13 */
    "MULTIPLE",        /* 14 */
    "MULTIPLE",        /* 15 */
};

STATIC char *
soc_schan_gen_resp_err_name(int err)
{
    if (err < 0 || err >= COUNTOF(_soc_schan_gen_resp_err_names)) {
        err = 2;
    }

    return _soc_schan_gen_resp_err_names[err];
}

#endif /* BCM_TRIUMPH_SUPPORT */
/*
 * Resets the CMIC S-Channel interface. This is required when we sent
 * a message and did not receive a response after the poll count was
 * exceeded.
 */

STATIC void
_soc_schan_reset(int unit)
{
    uint32 val;

#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);

    if(soc_feature(unit, soc_feature_cmicm)) {
        val = soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc));
        /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
        soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), val | SC_CMCx_SCHAN_ABORT);
        SDK_CONFIG_MEMORY_BARRIER;
        soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), val);
        SDK_CONFIG_MEMORY_BARRIER;
    } else
#endif /* CMICM Support */
    {
        val = soc_pci_read(unit, CMIC_CONFIG);

        /* Toggle S-Channel abort bit in CMIC_CONFIG register */
        soc_pci_write(unit, CMIC_CONFIG, val | CC_SCHAN_ABORT);

        SDK_CONFIG_MEMORY_BARRIER;

        soc_pci_write(unit, CMIC_CONFIG, val);

        SDK_CONFIG_MEMORY_BARRIER;
    }

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }
}

void
soc_schan_header_cmd_set(int unit, schan_header_t *header, int opcode,
                         int dst_blk, int src_blk, int acc_type,
                         int data_byte_len, int dma, uint32 bank_ignore_mask)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        header->v4.opcode = opcode;
        header->v4.dst_blk = dst_blk;
        /* input argument src_blk is ignored */
        header->v4.acc_type = acc_type;
        header->v4.data_byte_len = data_byte_len;
        header->v4.dma = dma;
        header->v4.bank_ignore_mask = bank_ignore_mask;
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        header->v3.opcode = opcode;
        header->v3.dst_blk = dst_blk;
        /* input argument src_blk is ignored */
        header->v3.acc_type = acc_type;
        header->v3.data_byte_len = data_byte_len;
        header->v3.dma = dma;
        header->v3.bank_ignore_mask = bank_ignore_mask;
    } else {
        header->v2.opcode = opcode;
        header->v2.dst_blk = dst_blk;
        header->v2.src_blk = src_blk;
        /* input argument acc_type is ignored */
        header->v2.data_byte_len = data_byte_len;
        /* input argument dma is ignored */
        header->v2.bank_ignore_mask = bank_ignore_mask;
    }
}

void
soc_schan_header_cmd_get(int unit, schan_header_t *header, int *opcode,
                         int *dst_blk, int *src_blk, int *acc_type,
                         int *data_byte_len, int *dma,
                         uint32 *bank_ignore_mask)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        if (opcode != NULL) {
            *opcode = header->v4.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = header->v4.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (acc_type != NULL) {
            *acc_type = header->v4.acc_type;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v4.data_byte_len;
        }
        if (dma != NULL) {
            *dma = header->v4.dma;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v4.bank_ignore_mask;
        }
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        if (opcode != NULL) {
            *opcode = header->v3.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = header->v3.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (acc_type != NULL) {
            *acc_type = header->v3.acc_type;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v3.data_byte_len;
        }
        if (dma != NULL) {
            *dma = header->v3.dma;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v3.bank_ignore_mask;
        }
    } else {
        if (opcode != NULL) {
            *opcode = header->v2.opcode;
        }
        if (dst_blk != NULL) {
            *dst_blk = header->v2.dst_blk;
        }
        if (src_blk != NULL) {
            *src_blk = header->v2.src_blk;
        }
        if (acc_type != NULL) {
            *acc_type = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v2.data_byte_len;
        }
        if (dma != NULL) {
            *dma = 0;
        }
        if (bank_ignore_mask != NULL) {
            *bank_ignore_mask = header->v2.bank_ignore_mask;
        }
    }
}

void
soc_schan_header_status_get(int unit, schan_header_t *header, int *opcode,
                            int *src_blk, int *data_byte_len,
                            int *err, int *ecode, int *nack)
{
    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        *opcode = header->v4.opcode;
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v4.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v4.err;
        }
        if (ecode != NULL) {
            *ecode = header->v4.ecode;
        }
        if (nack != NULL) {
            *nack = header->v4.nack;
        }
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        *opcode = header->v3.opcode;
        if (src_blk != NULL) {
            *src_blk = 0;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v3.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v3.err;
        }
        if (ecode != NULL) {
            *ecode = header->v3.ecode;
        }
        if (nack != NULL) {
            *nack = header->v3.nack;
        }
    } else {
        *opcode = header->v2.opcode;
        if (src_blk != NULL) {
            *src_blk = header->v2.src_blk;
        }
        if (data_byte_len != NULL) {
            *data_byte_len = header->v2.data_byte_len;
        }
        if (err != NULL) {
            *err = header->v2.err;
        }
        if (ecode != NULL) {
            *ecode = header->v2.ecode;
        }
        if (nack != NULL) {
            *nack = header->v2.nack;
        }
    }
}

/*
 * Dump an S-Channel message for debugging
 */

void
soc_schan_dump(int unit, schan_msg_t *msg, int dwc)
{
    char                buf[128];
    int                 i, j;

    if (soc_feature(unit, soc_feature_sbus_format_v4)) {
        LOG_CLI((BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d ACC=%d DPORT=%d OPC=%d=%s]\n"),
                 msg->header.v4.nack, msg->header.v4.bank_ignore_mask,
                 msg->header.v4.dma, msg->header.v4.ecode,
                 msg->header.v4.err, msg->header.v4.data_byte_len,
                 msg->header.v4.acc_type, msg->header.v4.dst_blk,
                 msg->header.v4.opcode,
                 soc_schan_op_name(msg->header.v4.opcode)));
    } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
        LOG_CLI((BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d ACC=%d DPORT=%d OPC=%d=%s]\n"),
                 msg->header.v3.nack, msg->header.v3.bank_ignore_mask,
                 msg->header.v3.dma, msg->header.v3.ecode,
                 msg->header.v3.err, msg->header.v3.data_byte_len,
                 msg->header.v3.acc_type, msg->header.v3.dst_blk,
                 msg->header.v3.opcode,
                 soc_schan_op_name(msg->header.v3.opcode)));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "  HDR[NACK=%d BANK=%d DMA=%d ECODE=%d ERR=%d "
                 "DLEN=%d SPORT=%d DPORT=%d OPC=%d=%s]\n"),
                 msg->header.v2.nack, msg->header.v2.bank_ignore_mask,
                 msg->header.v2.dma, msg->header.v2.ecode,
                 msg->header.v2.err, msg->header.v2.data_byte_len,
                 msg->header.v2.src_blk, msg->header.v2.dst_blk,
                 msg->header.v2.opcode,
                 soc_schan_op_name(msg->header.v2.opcode)));
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_generic_table_ops) &&
        ((msg->header.v2.opcode == TABLE_INSERT_DONE_MSG) ||
         (msg->header.v2.opcode == TABLE_DELETE_DONE_MSG) ||
         (msg->header.v2.opcode == TABLE_LOOKUP_DONE_MSG))) {
        if (soc_feature(unit, soc_feature_new_sbus_format) &&
            !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
            LOG_CLI((BSL_META_U(unit,
                                "  RSP[TYPE=%d=%s ERR_INFO=%d=%s "
                     "INDEX=0x%05x]\n"),
                     msg->genresp_v2.response.type,
                     soc_schan_gen_resp_type_name(msg->genresp_v2.response.type),
                     msg->genresp_v2.response.err_info,
                     soc_schan_gen_resp_err_name(msg->genresp_v2.response.err_info),
                     msg->genresp_v2.response.index));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "  RSP[SRC=%d TYPE=%d=%s ERR_INFO=%d=%s "
                     "INDEX=0x%05x]\n"),
                     msg->genresp.response.src,
                     msg->genresp.response.type,
                     soc_schan_gen_resp_type_name(msg->genresp.response.type),
                     msg->genresp.response.err_info,
                     soc_schan_gen_resp_err_name(msg->genresp.response.err_info),
                     msg->genresp.response.index));
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    assert(dwc <= CMIC_SCHAN_WORDS(unit));

    for (i = 0; i < dwc; i += 4) {
        buf[0] = 0;

        for (j = i; j < i + 4 && j < dwc; j++) {
            sal_sprintf(buf + sal_strlen(buf),
                        " DW[%2d]=0x%08x", j, msg->dwords[j]);
        }

        LOG_CLI((BSL_META_U(unit,
                            " %s\n"), buf));
    }
}

#define SOC_SCHAN_WB_TIME_OUT            (5000000) /*5 sec*/
int
soc_schan_override_enable(int unit)
{
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        return SOC_E_UNIT;
    }
    if (!SOC_HW_ACCESS_DISABLE(unit)) {
        return SOC_E_NONE;
    }
    if (SOC_WARM_BOOT(unit)) {
        if (sal_mutex_take(SOC_CONTROL(unit)->schan_wb_mutex,
                           SOC_SCHAN_WB_TIME_OUT)) {
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "Failed to take schan_wb_mutex.\n")));
            return SOC_E_INTERNAL;
        }
        SOC_CONTROL(unit)->schan_wb_thread_id = sal_thread_self();
    }

    return SOC_E_NONE;
}

int
soc_schan_override_disable(int unit)
{
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        return SOC_E_UNIT;
    }
    if (SOC_CONTROL(unit)->schan_wb_thread_id == sal_thread_self()) {
        SOC_CONTROL(unit)->schan_wb_thread_id = SAL_THREAD_ERROR;
        if (sal_mutex_give(SOC_CONTROL(unit)->schan_wb_mutex)) {
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "Failed to release schan_wb_mutex.\n")));
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

/*
 * soc_schan_op
 *
 * Writes a message of dwc_write DWORDs from msg to the S-Channel, waits
 * for the operation to complete, then reads dwc_read DWORDs from the
 * channel into msg.  If there is no return data, use dwc_read = 0.
 *
 * On platforms where it is appropriate, the S-Channel is locked during
 * the operation to prevent multiple tasks trying to use the S-Channel
 * simultaneously.
 *
 * Return value is negative on error, 0 on success.
 *
 * If intr is true, this routine goes to sleep until an S-Channel
 * completion interrupt wakes it up.  Otherwise, it polls for the done
 * bit.  NOTE: if schanIntrEnb is false, intr is always overridden to 0.
 *
 * Polling is more CPU efficient for most operations since they complete
 * much faster than the interrupt processing time would take.  However,
 * due to the chip design, some operations such as ARL insert and delete
 * may have unbounded response time.  In this case, the interrupt should
 * be used.  The worst case we have seen is 1 millisec when the switch
 * is passing max traffic with random addresses at min packet size.
 *
 * Ensure that a SOC_CONTROL(unit)->schan_op function pointer is defined for this
 * unit, and dispatch to that if possible.
 */
int
soc_schan_op(int unit,
             schan_msg_t *msg,
             int dwc_write, int dwc_read,
             int intr)
{
    if (SOC_CONTROL(unit)->schan_op == NULL) {
        LOG_BSL_FATAL(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "SOC_SCHAN_OP() function is undefined\n")));
        return SOC_E_FAIL;
    } else {
        return SOC_CONTROL(unit)->schan_op(unit, msg, dwc_write, dwc_read, intr);
    }
}

#ifdef BCM_CMICM_SUPPORT
int
fschan_wait_idle(int unit)
{
    int cmc = SOC_PCI_CMC(unit);
    soc_timeout_t to;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);
    do {
        if (0==(soc_pci_read(unit, CMIC_CMCx_FSCHAN_STATUS_OFFSET(cmc)) & FSCHAN_BUSY)) {
            return SOC_E_NONE;
        }
    } while (!soc_timeout_check(&to));
    return SOC_E_TIMEOUT;
}
#endif

/*
 * _soc_schan_check_hw_access_disabled
 *
 * Called by soc_schan_op_* functions.  If hardware access is disabled, this
 * should return TRUE and set *rv to an error code that will be passed up to the
 * caller.  Otherwise, return FALSE and do not alter *rv.
 */
STATIC int
_soc_schan_check_hw_access_disabled(int unit, schan_msg_t *msg, int *rv)
{
    if (SOC_HW_ACCESS_DISABLE(unit)) {
        switch (msg->header.v2.opcode) {
        case WRITE_MEMORY_CMD_MSG:
            *rv = SOC_E_NONE;
            return TRUE;
        case WRITE_REGISTER_CMD_MSG:
            msg->header.v2.opcode = WRITE_REGISTER_ACK_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case ARL_INSERT_CMD_MSG:
            msg->header.v2.opcode = ARL_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case ARL_DELETE_CMD_MSG:
            msg->header.v2.opcode = ARL_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case L3_INSERT_CMD_MSG:
            msg->header.v2.opcode = L3_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case L3_DELETE_CMD_MSG:
            msg->header.v2.opcode = L3_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case INIT_CFAP_MSG:
            *rv = SOC_E_NONE;
            return TRUE;
        case READ_REGISTER_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                sal_memset(msg->bytes, 0,
                           sizeof(uint32) * CMIC_SCHAN_WORDS_ALLOC);
                msg->header.v2.opcode = READ_REGISTER_ACK_MSG;
                *rv = SOC_E_NONE;
                return TRUE;
            }
            break;
        case READ_MEMORY_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                sal_memset(msg->bytes, 0,
                           sizeof(uint32) * CMIC_SCHAN_WORDS_ALLOC);
                msg->header.v2.opcode = READ_MEMORY_ACK_MSG;
                *rv = SOC_E_NONE;
                return TRUE;
            }
            break;
        case L2_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                msg->header.v2.opcode = L2_LOOKUP_ACK_MSG;
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case L3X2_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                msg->header.v2.opcode = L3X2_LOOKUP_ACK_MSG;
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case ARL_LOOKUP_CMD_MSG:
        case L3_LOOKUP_CMD_MSG:
            if (SOC_IS_DETACHING(unit)) {
                *rv = SOC_E_NOT_FOUND;
                return TRUE;
            }
            break;
        case TABLE_INSERT_CMD_MSG:
            msg->header.v2.opcode = TABLE_INSERT_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case TABLE_DELETE_CMD_MSG:
            msg->header.v2.opcode = TABLE_DELETE_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case FIFO_POP_CMD_MSG:
            msg->header.v2.opcode = FIFO_POP_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        case FIFO_PUSH_CMD_MSG:
            msg->header.v2.opcode = FIFO_PUSH_DONE_MSG;
            *rv = SOC_E_NONE;
            return TRUE;
        default:
            break;
        }
    }

    return FALSE;
}

STATIC int
_soc_schan_op_arad_sanity_check(int unit, schan_msg_t *msg, int dwc_write,
                                int dwc_read)
{
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit) &&
        msg->header.v3.opcode != READ_REGISTER_CMD_MSG &&
        msg->header.v3.opcode != READ_MEMORY_CMD_MSG &&
        ((SOC_WARM_BOOT(unit) && !IS_WRITE_ALLOWED_DURING_WB(unit)) || SOC_IS_DETACHING(unit)))
    {
        LOG_BSL_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "soc_schan_op: ERROR, trying to access HW during deinit or warm reboot\n")));
        return SOC_E_FAIL;
    }
#endif

    return SOC_E_NONE;
}

/*
 * _soc_schan_op_sanity_check
 *
 * First, check that writing is allowed:
 *    If (device is ARAD) and (opcode is not a READ command) and (override flag is off)
 *    Then: writing is not allowed.
 *
 * Then, assert sanity checks.
 * Finally, log the message that schan access is beginning
 *
 * Returns TRUE if the caller should abort the call and return *rv up the stack.
 * Returns FALSE if the caller should continue as normal.
 */
STATIC int
_soc_schan_op_sanity_check(int unit, schan_msg_t *msg, int dwc_write,
                           int dwc_read, int *rv)
{
    *rv = _soc_schan_op_arad_sanity_check(unit, msg, dwc_write, dwc_read);
    if (SOC_FAILURE(*rv)) {
        return TRUE;
    }

    assert(! sal_int_context());
    assert(dwc_write <= CMIC_SCHAN_WORDS(unit));
    assert(dwc_read <= CMIC_SCHAN_WORDS(unit));

    if (_soc_schan_check_hw_access_disabled(unit, msg, rv) == TRUE) {
        return TRUE;
    }

    if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "S-CHANNEL %s: (unit %d)\n"),
                 soc_schan_op_name(msg->header.v2.opcode), unit));
        soc_schan_dump(unit, msg, dwc_write);
    }

    return FALSE;
}

#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)
/*
 * soc_schan_op_wrapper_plibde
 *
 * When SOC_CONTROL(unit)->schan_op() is called with plisim enabled, it's
 * dispatched here.
 */
STATIC int
_soc_schan_op_wrapper_plibde(int unit, schan_msg_t *msg, int dwc_write,
                            int dwc_read, int intr)
{
    int rv;

    /* Back door into the simulation to perform direct SCHANNEL operations */
    if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                            "S-CHANNEL %s: (unit %d)\n"),
                 soc_schan_op_name(msg->header.v2.opcode), unit));
        soc_schan_dump(unit, msg, dwc_write);
    }

    rv = plibde_schan_op(unit, msg, dwc_write, dwc_read);

    if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        soc_schan_dump(unit, msg, dwc_read);
    }

    return rv;
}
#endif

#ifdef INCLUDE_RCPU
/*
 * soc_schan_op_wrapper_rcpu
 *
 * When SOC_CONTROL(unit)->schan_op() is called for RCPU, it's
 * dispatched here.  If a rcpu schan callback is registered, it should be
 * called.
 */
STATIC int
_soc_schan_op_wrapper_rcpu(int unit, schan_msg_t *msg, int dwc_write,
                          int dwc_read, int intr)
{
    if (SOC_CONTROL(unit)->soc_rcpu_schan_op == NULL) {
        LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "No soc_rcpu_schan_op callback defined\n")));
        return SOC_E_FAIL;
    }

    return SOC_CONTROL(unit)->soc_rcpu_schan_op(unit, msg, dwc_write, dwc_read);
}
#endif /* INCLUDE_RCPU */

/*
 * _soc_schan_cmicm_intr_wait
 *
 * Called by schan_op_* functions.  On CMIC-M chips with schan interrupts
 * enabled, wait on a schan interrupt.  When schan interrupts are disabled,
 * _soc_schan_cmicm_poll_wait is called instead.
 */
STATIC int
_soc_schan_cmicm_intr_wait(int unit)
{
#ifdef BCM_CMICM_SUPPORT

    int rv = SOC_E_NONE;
    int cmc = SOC_PCI_CMC(unit);

    soc_cmicm_intr0_enable(unit, IRQ_CMCx_SCH_OP_DONE);
    if (sal_sem_take(SOC_CONTROL(unit)->schanIntr,
                     SOC_CONTROL(unit)->schanTimeout) != 0) {
        rv = SOC_E_TIMEOUT;
    } else {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt received\n")));

        if (SOC_CONTROL(unit)->schan_result & SC_CMCx_MSG_NAK) {
            rv = SOC_E_FAIL;
        }
        if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
            if (SOC_CONTROL(unit)->schan_result & SC_CMCx_MSG_TIMEOUT_TST) {
                rv = SOC_E_TIMEOUT;
            }
        }

        if (soc_feature(unit, soc_feature_schan_err_check)) {
            uint32 schan_err, err_bit = 0;

            schan_err = soc_pci_read(unit, CMIC_CMCx_SCHAN_ERR_OFFSET(cmc));
            err_bit = soc_reg_field_get(unit, CMIC_CMC0_SCHAN_ERRr, schan_err, ERRBITf);

            if (err_bit) {
                rv = SOC_E_FAIL;
                LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                    "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
            }
        }
    }

    soc_cmicm_intr0_disable(unit, IRQ_CMCx_SCH_OP_DONE);

    return rv;

#else
    return SOC_E_FAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * _soc_schan_cmicm_poll_wait
 *
 * Called by schan_op_* functions.  On CMIC-M chips with schan interrupts
 * disabled, wait on a schan response via polling.  When schan interrupts are
 * enabled, _soc_schan_cmicm_intr_wait is called instead.
 */
STATIC int
_soc_schan_cmicm_poll_wait(int unit, schan_msg_t *msg)
{
#if defined(BCM_CMICM_SUPPORT)

    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanCtrl;
    int cmc = SOC_PCI_CMC(unit);

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);

    while (((schanCtrl = soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc))) &
            SC_CMCx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    }

    if (rv == SOC_E_NONE) {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
    }

    if (schanCtrl & SC_CMCx_MSG_NAK) {
        schan_msg_t resp_msg;

        /* This is still the input opcode */
        resp_msg.dwords[1] =
            soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, 1));

        rv = SOC_E_FAIL;
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  NAK received from SCHAN.\n")));

        switch (msg->header.v2.opcode) {
            case TABLE_INSERT_CMD_MSG:
            case TABLE_DELETE_CMD_MSG:
            case TABLE_LOOKUP_CMD_MSG:
                if (resp_msg.genresp.response.type != SCHAN_GEN_RESP_TYPE_ERROR) {
                    /* Not a parity error */
                    break;
                }
                /* Else fallthru */
            case READ_MEMORY_CMD_MSG:
            case READ_REGISTER_CMD_MSG:
#if defined(BCM_XGS_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
            {
                int dst_blk, opcode;
                soc_schan_header_cmd_get(unit, &msg->header, &opcode, &dst_blk,
                    NULL, NULL, NULL, NULL, NULL);

                (void)soc_ser_mem_nack(INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address),
                    INT_TO_PTR(dst_blk),
                    opcode == READ_REGISTER_CMD_MSG ? INT_TO_PTR(1) : 0, 0);
            }
#endif
            default:
                break;
        }
    }
    if ((schanCtrl & SC_CMCx_MSG_SER_CHECK_FAIL) &&
        soc_feature(unit, soc_feature_ser_parity)){
        rv = SOC_E_FAIL;
        LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "  SER Parity Check Error.\n")));

#if defined(BCM_XGS_SUPPORT)
        sal_dpc(soc_ser_fail, INT_TO_PTR(unit),
                INT_TO_PTR(msg->gencmd.address), 0, 0, 0);
#endif
    }

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_CMCx_MSG_TIMEOUT_TST) {
            rv = SOC_E_TIMEOUT;
        }
    }

    if (soc_feature(unit, soc_feature_schan_err_check)) {
        uint32 schan_err, err_bit = 0;

        schan_err = soc_pci_read(unit, CMIC_CMCx_SCHAN_ERR_OFFSET(cmc));
        err_bit = soc_reg_field_get(unit, CMIC_CMC0_SCHAN_ERRr, schan_err, ERRBITf);

        if (err_bit) {
            rv = SOC_E_FAIL;
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
        }
    }
    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), SC_CMCx_MSG_CLR);

    return rv;

#else
    return SOC_E_FAIL;
#endif /* BCM_CMICM_SUPPORT */
}


/*
 * _soc_schan_no_cmicm_dpp_err_check
 *
 * On DPP and DFE devices (without CMIC-M) but with the schan_err_check feature,
 * exercise the feature and check for schan errors.
 */
STATIC int
_soc_schan_no_cmicm_dpp_err_check(int unit)
{
    int rv = SOC_E_NONE;

#if defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
    if(soc_feature(unit, soc_feature_schan_err_check)) {
        uint32 schan_err, err_bit = 0;

        schan_err = soc_pci_read(unit, CMIC_SCHAN_ERR);
        err_bit = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, schan_err, ERRBITf);

        if (err_bit) {
            soc_pci_write(unit, CMIC_SCHAN_ERR, 0);
            rv = SOC_E_FAIL;
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  ERRBIT received in CMIC_SCHAN_ERR.\n")));
        }
    }
#endif /* BCM_DPP_SUPPORT, BCM_DFE_SUPPORT*/

    return rv;
}

/*
 * _soc_schan_no_cmicm_intr_wait
 *
 * Called by schan_op_* functions.  On chips without CMIC-M using schan
 * interrupts, wait on a schan interrupt.  When schan interrupts are disabled,
 * _soc_schan_no_cmicm_poll_wait is called instead.
 */
STATIC int
_soc_schan_no_cmicm_intr_wait(int unit)
{
    int rv = SOC_E_NONE;

    soc_intr_enable(unit, IRQ_SCH_MSG_DONE);

    if (sal_sem_take(SOC_CONTROL(unit)->schanIntr,
                     SOC_CONTROL(unit)->schanTimeout) != 0) {
        rv = SOC_E_TIMEOUT;
    } else {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt received\n")));

        if (SOC_CONTROL(unit)->schan_result & SC_MSG_NAK_TST) {
            rv = SOC_E_FAIL;
        }
        if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
            if (SOC_CONTROL(unit)->schan_result & SC_MSG_TIMEOUT_TST) {
                rv = SOC_E_TIMEOUT;
            }
        }

        SOC_IF_ERROR_RETURN(_soc_schan_no_cmicm_dpp_err_check(unit));
    }

    soc_intr_disable(unit, IRQ_SCH_MSG_DONE);

    return rv;
}

/*
 * _soc_schan_no_cmicm_tr2_check_ser_nack
 *
 * On Triumph2 devices, enable special processing in response to schan NACK
 * responses.  For table insert/lookup/delete, if a parity error appears,
 * send a NACK.  For memory/register reads, send a NACK regardless.
 */
STATIC int
_soc_schan_no_cmicm_tr2_check_ser_nack(int unit, schan_msg_t *msg)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    schan_msg_t resp_msg;

    /* This is still the input opcode */
    resp_msg.dwords[1] =
        soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, 1));

    switch (msg->header.v2.opcode) {
        case TABLE_INSERT_CMD_MSG:
        case TABLE_DELETE_CMD_MSG:
        case TABLE_LOOKUP_CMD_MSG:
            if (SCHAN_GEN_RESP_TYPE_ERROR !=
                resp_msg.genresp.response.type) {
                /* Not a parity error */
                break;
            }
            /* Else fallthru */
        case READ_MEMORY_CMD_MSG:
        case READ_REGISTER_CMD_MSG:
            {
                int dst_blk, opcode;
                soc_schan_header_cmd_get(unit, &msg->header, &opcode, &dst_blk,
                    NULL, NULL, NULL, NULL, NULL);

                (void)soc_ser_mem_nack(INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address),
                    INT_TO_PTR(dst_blk),
                    opcode == READ_REGISTER_CMD_MSG ? INT_TO_PTR(1) : 0, 0);
            }
            break;
        default:
            break;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return SOC_E_NONE;
}

/*
 * _soc_schan_no_cmicm_check_ser_parity
 *
 * On TD/TR2 devices without cmicm but with ser parity support, check for a ser
 * parity error.
 */
STATIC int
_soc_schan_no_cmicm_check_ser_parity(int unit, uint32 *schanCtrl, schan_msg_t *msg)
{
    int rv = SOC_E_NONE;

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if ((*schanCtrl & SC_MSG_SER_CHECK_FAIL_TST) &&
        soc_feature(unit, soc_feature_ser_parity)) {
            rv = SOC_E_FAIL;
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "  SER Parity Check Error.\n")));
            sal_dpc(soc_ser_fail, INT_TO_PTR(unit),
                    INT_TO_PTR(msg->gencmd.address), 0, 0, 0);
    }
#endif /* BCM_TRIUMPH2_SUPPORT, BCM_TRIDENT_SUPPORT */

    return rv;
}

/*
 * _soc_schan_no_cmicm_poll_wait
 *
 * Called by schan_op_* functions.  On chips without CMIC-M and with schan
 * interrupts disabled, wait on a schan response via polling.  When schan
 * interrupts are enabled, _soc_schan_no_cmicm_intr_wait is called instead.
 */
STATIC int
_soc_schan_no_cmicm_poll_wait(int unit, schan_msg_t *msg)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanCtrl;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);

    while (((schanCtrl = soc_pci_read(unit, CMIC_SCHAN_CTRL)) &
            SC_MSG_DONE_TST) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

    if (rv == SOC_E_NONE) {
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
    }

    if (schanCtrl & SC_MSG_NAK_TST) {
        rv = SOC_E_FAIL;
#ifdef _SER_TIME_STAMP
        ser_time_1 = sal_time_usecs();
#endif
        LOG_BSL_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  NAK received from SCHAN.\n")));

        SOC_IF_ERROR_RETURN(_soc_schan_no_cmicm_tr2_check_ser_nack(unit, msg));
    }

    SOC_IF_ERROR_RETURN(_soc_schan_no_cmicm_check_ser_parity(unit, &schanCtrl, msg));

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_MSG_TIMEOUT_TST) {
            rv = SOC_E_TIMEOUT;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_schan_no_cmicm_dpp_err_check(unit));
    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_DONE_CLR);

    return rv;
}

#if defined(BCM_TRIDENT2_SUPPORT) && defined(BCM_CMICM_SUPPORT)
/*
 * soc_schan_op_td2
 *
 * When soc_schan_op() is called on Trident2, it is dispatched here
 */
STATIC int
_soc_schan_op_td2(int unit, schan_msg_t *msg, int dwc_write,
                 int dwc_read, int intr)
{
    int i, rv;
    int cmc = SOC_PCI_CMC(unit);

    if (_soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

    SCHAN_LOCK(unit);

    do {
        rv = SOC_E_NONE;

        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), msg->dwords[i]);
        }

        /* Tell CMIC to start */
        soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), SC_CMCx_MSG_START);

        /* Wait for completion using either the interrupt or polling method */
        if (intr && SOC_CONTROL(unit)->schanIntrEnb) {
            rv = _soc_schan_cmicm_intr_wait(unit);
        } else {
            rv = _soc_schan_cmicm_poll_wait(unit, msg);
        }

        if (rv == SOC_E_TIMEOUT) {
            LOG_BSL_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                                "soc_schan_op: operation attempt timed out\n")));
            SOC_CONTROL(unit)->stat.err_sc_tmo++;
            _soc_schan_reset(unit);
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
        }

        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

        SOC_CONTROL(unit)->stat.schan_op++;

    } while (0);

    SCHAN_UNLOCK(unit);

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
        }
    }

    return rv;
}
#endif /* BCM_TRIDENT2_SUPPORT && BCM_CMICM_SUPPORT */

/*
 * _soc_schan_timeout_check
 *
 * After a schan response is received, enable special processing for timeout
 * errors.  Sometimes timeout should be ignored.
 */
STATIC int
_soc_schan_timeout_check(int unit, int *rv, schan_msg_t *msg)
{
    if (*rv != SOC_E_TIMEOUT) {
        return FALSE;
    }

    if (SOC_IS_TRIUMPH3(unit) && msg->header.v3.opcode == WRITE_REGISTER_CMD_MSG) {
        if ((msg->writecmd.address == 0x02029700) &&
            (msg->writecmd.header.v3.dst_blk >= 0x8  && msg->writecmd.header.v3.dst_blk <= 0xf)) {
            *rv = SOC_E_NONE;
        }
    } else {
        LOG_BSL_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation attempt timed out\n")));
        SOC_CONTROL(unit)->stat.err_sc_tmo++;
        _soc_schan_reset(unit);
        return TRUE;
    }

    return FALSE;
}

/*
 * _soc_schan_op_cmicm
 *
 * On devices that support CMIC-M, this is the basic schan operation function
 */
STATIC int
_soc_schan_op_cmicm(int unit, schan_msg_t *msg, int dwc_write,
                   int dwc_read, int intr)
{
#ifdef BCM_CMICM_SUPPORT

    int i, rv;
    int cmc = SOC_PCI_CMC(unit);

    if (_soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

    SCHAN_LOCK(unit);

    do {
        rv = SOC_E_NONE;

        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), msg->dwords[i]);
        }

        /* Tell CMIC to start */
        soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), SC_CMCx_MSG_START);

        /* Wait for completion using either the interrupt or polling method */
        if (intr && SOC_CONTROL(unit)->schanIntrEnb) {
            rv = _soc_schan_cmicm_intr_wait(unit);
        } else {
            rv = _soc_schan_cmicm_poll_wait(unit, msg);
        }

        if (_soc_schan_timeout_check(unit, &rv, msg) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
        }

        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

        SOC_CONTROL(unit)->stat.schan_op++;

    } while (0);

    SCHAN_UNLOCK(unit);

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
        }
    }

    return rv;

#else
    return SOC_E_FAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * _soc_schan_op_no_cmicm
 *
 * On devices that do not support CMIC-M, this is the basic logic for schan
 * operations.
 */
STATIC int
_soc_schan_op_no_cmicm(int unit, schan_msg_t *msg, int dwc_write,
                      int dwc_read, int intr)
{
    int i, rv;

    if (_soc_schan_op_sanity_check(unit, msg, dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

    SCHAN_LOCK(unit);

    do {
        rv = SOC_E_NONE;

        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_SCHAN_MESSAGE(unit, i), msg->dwords[i]);
        }

        soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_START_SET);

        /* Wait for completion using either the interrupt or polling method */
        if (intr && SOC_CONTROL(unit)->schanIntrEnb) {
            rv = _soc_schan_no_cmicm_intr_wait(unit);
        } else {
            rv = _soc_schan_no_cmicm_poll_wait(unit, msg);
        }

        if (_soc_schan_timeout_check(unit, &rv, msg) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] = soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, i));
        }

        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

        SOC_CONTROL(unit)->stat.schan_op++;

    } while (0);

    SCHAN_UNLOCK(unit);

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_BSL_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_BSL_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
        }
    }

    return rv;
}

/*
 * soc_find_schan_op
 *
 * Called at the end of soc_attach(), this should return a pointer to the
 * correct soc_schan_op implementation function
 */
schan_op_func_t soc_find_schan_op(int unit)
{
#if defined(PLISIM) && defined(PLISIM_DIRECT_SCHAN)
    return _soc_schan_op_wrapper_plibde;
#endif

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_ONLY(unit) || SOC_IS_RCPU_UNIT(unit)) {
        return _soc_schan_op_wrapper_rcpu;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2(unit)) {
        return _soc_schan_op_td2;
    }
#endif

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        return _soc_schan_op_cmicm;
    }
#endif

    return _soc_schan_op_no_cmicm;
}
