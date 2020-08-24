/*
 * Copyright 2017 Broadcom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation (the "GPL").
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 (GPLv2) for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 (GPLv2) along with this source code.
 */

/*
 * This module implements a Linux PTP Clock driver for Broadcom
 * XGS switch devices.
 *
 * For a list of supported module parameters, please see below.
 *   debug: Debug level (default 0)
 *   network_transport : Transport Type (default 0 - Raw)
 *   base_dev_name: Base device name (default ptp0, ptp1, etc.)
 *
 * - All the data structures and functions work on the physical port.
 *   For array indexing purposes, we use (phy_port - 1).
 */

#include <gmodule.h> /* Must be included first */
/* Module Information */
#define MODULE_MAJOR 125
#define MODULE_NAME "linux-bcm-ptp-clock"

MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("PTP Clock Driver for Broadcom XGS Switch");
MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
#include <linux-bde.h>
#include <kcom.h>
#include <bcm-knet.h>
#include <linux/time64.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>

#include <linux/random.h>
#include <linux/seq_file.h>
#include <linux/if_vlan.h>
#include <linux/ptp_clock_kernel.h>

/* Configuration Parameters */
static int debug;
LKM_MOD_PARAM(debug, "i", int, 0);
MODULE_PARM_DESC(debug,
        "Debug level (default 0)");

static int network_transport;
LKM_MOD_PARAM(network_transport, "i", int, 0);
MODULE_PARM_DESC(network_transport,
        "Transport Type (default 2 - Raw)");

static char *base_dev_name = "ptp0";
LKM_MOD_PARAM(base_dev_name, "s", charp, 0);
MODULE_PARM_DESC(base_dev_name,
        "Base device name (default ptp0, ptp1, etc.)");

static int fw_core;
LKM_MOD_PARAM(fw_core, "i", int, 0);
MODULE_PARM_DESC(fw_core,
        "Firmware core (default 0)");

/* Debug levels */
#define DBG_LVL_VERB    0x1
#define DBG_LVL_WARN    0x2

#define DBG_VERB(_s)    do { if (debug & DBG_LVL_VERB) gprintk _s; } while (0)
#define DBG_WARN(_s)    do { if (debug & DBG_LVL_WARN) gprintk _s; } while (0)
#define DBG_ERR(_s)     do { if (1) gprintk _s; } while (0)


#ifdef LINUX_BDE_DMA_DEVICE_SUPPORT
#define DMA_DEV         device
#define DMA_ALLOC_COHERENT(d,s,h)       dma_alloc_coherent(d,s,h,GFP_ATOMIC|GFP_DMA32)
#define DMA_FREE_COHERENT(d,s,a,h)      dma_free_coherent(d,s,a,h)
#else
#define DMA_DEV         pci_dev
#define DMA_ALLOC_COHERENT(d,s,h)       pci_alloc_consistent(d,s,h)
#define DMA_FREE_COHERENT(d,s,a,h)      pci_free_consistent(d,s,a,h)
#endif

/* Type length in bytes */
#define BKSYNC_PACKLEN_U8     1
#define BKSYNC_PACKLEN_U16    2
#define BKSYNC_PACKLEN_U24    3
#define BKSYNC_PACKLEN_U32    4

#define BKSYNC_UNPACK_U8(_buf, _var)         \
    _var = *_buf++

#define BKSYNC_UNPACK_U16(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 8) |        \
                  (_buf)[1]);               \
        (_buf) += BKSYNC_PACKLEN_U16;         \
    } while (0)

#define BKSYNC_UNPACK_U24(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 16) |       \
                  ((_buf)[1] << 8)  |       \
                  (_buf)[2]);               \
        (_buf) += BKSYNC_PACKLEN_U24;         \
    } while (0)

#define BKSYNC_UNPACK_U32(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 24) |       \
                  ((_buf)[1] << 16) |       \
                  ((_buf)[2] << 8)  |       \
                  (_buf)[3]);               \
        (_buf) += BKSYNC_PACKLEN_U32;         \
    } while (0)



/* CMIC MCS-0 SCHAN Messaging registers */
/* Core0:CMC1 Core1:CMC2 */
#define CMIC_CMC_BASE                    (fw_core ? 0x33000 : 0x32000)
#define CMIC_CMC_SCHAN_MESSAGE_10r(BASE) (BASE + 0x00000034)
#define CMIC_CMC_SCHAN_MESSAGE_11r(BASE) (BASE + 0x00000038)
#define CMIC_CMC_SCHAN_MESSAGE_12r(BASE) (BASE + 0x0000003c)
#define CMIC_CMC_SCHAN_MESSAGE_13r(BASE) (BASE + 0x00000040)
#define CMIC_CMC_SCHAN_MESSAGE_14r(BASE) (BASE + 0x00000044)
#define CMIC_CMC_SCHAN_MESSAGE_15r(BASE) (BASE + 0x00000048)
#define CMIC_CMC_SCHAN_MESSAGE_16r(BASE) (BASE + 0x0000004c)
#define CMIC_CMC_SCHAN_MESSAGE_17r(BASE) (BASE + 0x00000050)
#define CMIC_CMC_SCHAN_MESSAGE_18r(BASE) (BASE + 0x00000054)
#define CMIC_CMC_SCHAN_MESSAGE_19r(BASE) (BASE + 0x00000058)
#define CMIC_CMC_SCHAN_MESSAGE_20r(BASE) (BASE + 0x0000005c)
#define CMIC_CMC_SCHAN_MESSAGE_21r(BASE) (BASE + 0x00000060)

/* TX Timestamp FIFO Access */
#define BCM_NUM_PORTS           128

/* Service request commands to R5 */
enum {
    BCM_KSYNC_DONE=0,
    BCM_KSYNC_INIT,
    BCM_KSYNC_DEINIT,
    BCM_KSYNC_GETTIME,
    BCM_KSYNC_SETTIME,
    BCM_KSYNC_FREQCOR,
    BCM_KSYNC_PBM_UPDATE,
    BCM_KSYNC_ADJTIME,
};

/* Usage macros */
#define ONE_BILLION (1000000000)

/*
 *  Hardware specific information
 */
uint32_t sobmhrawpkts_dcb26[8] = {0x00000000, 0x00020E00, 0x00000000, 0x00000000, 0x00000000, 0x00021200, 0x00000000, 0x00000000};
uint32_t sobmhudpipv4_dcb26[8] = {0x00000000, 0x00022A00, 0x00000000, 0x00000000, 0x00000000, 0x00022E00, 0x00000000, 0x00000000};
uint32_t sobmhudpipv6_dcb26[8] = {0x00000000, 0x00022A00, 0x00000000, 0x00000000, 0x00000000, 0x00022E00, 0x00000000, 0x00000000};

uint32_t sobmhrawpkts_dcb32[8] = {0x00000000, 0x00010E00, 0x00000000, 0x00000000, 0x00000000, 0x00011200, 0x00000000, 0x00000000};
uint32_t sobmhudpipv4_dcb32[8] = {0x00000000, 0x00012A00, 0x00000000, 0x00000000, 0x00000000, 0x00012E00, 0x00000000, 0x00000000};
uint32_t sobmhudpipv6_dcb32[8] = {0x00000000, 0x00012A00, 0x00000000, 0x00000000, 0x00000000, 0x00012E00, 0x00000000, 0x00000000};

/* Driver Proc Entry root */
static struct proc_dir_entry *bksync_proc_root = NULL;

/* Shared data structures with R5 */
typedef struct _bksync_tx_ts_data_s
{
    u32 ts_valid;   /* Timestamp valid indication */
    u32 port_id;    /* Port number */
    u32 ts_seq_id;  /* Sequency Id */
    u32 ts_cnt;
    u64 timestamp;  /* Timestamp */
} bksync_tx_ts_data_t;

typedef struct _bksync_uc_linux_ipc_s
{
    u32 ksyncinit;
    u32 dev_id;
    s64 freqcorr;
    u64 portmap[BCM_NUM_PORTS/64];
    u64 ptptime;
    s64 phase_offset;
    bksync_tx_ts_data_t port_ts_data[BCM_NUM_PORTS];
} bksync_uc_linux_ipc_t;

/* Clock Private Data */
struct bksync_ptp_priv {
    struct device           dev;
    struct ptp_clock        *ptp_clock;
    struct ptp_clock_info   ptp_caps;
    struct mutex            ptp_lock;
    volatile void           *base_addr;   /* address for PCI register access */
    volatile bksync_uc_linux_ipc_t    *shared_addr; /* address for shared memory access */
    uint64_t                dma_mem;
    int                     dma_mem_size;
    struct DMA_DEV          *dma_dev;    /* Required for DMA memory control */
    u32                     pkt_rxctr[BCM_NUM_PORTS];
    u32                     pkt_txctr[BCM_NUM_PORTS];
    u32                     ts_match[BCM_NUM_PORTS];
    u32                     ts_timeout[BCM_NUM_PORTS];
    u32                     ts_discard[BCM_NUM_PORTS];
};

static struct bksync_ptp_priv *ptp_priv;
volatile bksync_uc_linux_ipc_t *linuxPTPMemory = (bksync_uc_linux_ipc_t*)(0);
static int retry_count = 10;   /* Default retry for 10 jiffies */

#if defined(CMIC_SOFT_BYTE_SWAP)

#define CMIC_SWAP32(_x)   ((((_x) & 0xff000000) >> 24) \
        | (((_x) & 0x00ff0000) >>  8) \
        | (((_x) & 0x0000ff00) <<  8) \
        | (((_x) & 0x000000ff) << 24))

#define DEV_READ32(_d, _a, _p) \
    do { \
        uint32_t _data; \
        _data = (((volatile uint32_t *)(_d)->base_addr)[(_a)/4]); \
        *(_p) = CMIC_SWAP32(_data); \
    } while(0)

#define DEV_WRITE32(_d, _a, _v) \
    do { \
        uint32_t _data = CMIC_SWAP32(_v); \
        ((volatile uint32_t *)(_d)->base_addr)[(_a)/4] = (_data); \
    } while(0)

#else

#define DEV_READ32(_d, _a, _p) \
    do { \
        *(_p) = (((volatile uint32_t *)(_d)->base_addr)[(_a)/4]); \
    } while(0)

#define DEV_WRITE32(_d, _a, _v) \
    do { \
        ((volatile uint32_t *)(_d)->base_addr)[(_a)/4] = (_v); \
    } while(0)
#endif  /* defined(CMIC_SOFT_BYTE_SWAP) */

static void
ptp_sleep(int jiffies)
{
#if 0
    int usec = (jiffies * 1000);
    usleep_range(usec,usec+1);
#endif

    wait_queue_head_t wq;
    init_waitqueue_head(&wq);

    wait_event_timeout(wq, 0, jiffies);

}

/**
 * bksync_ptp_adjfreq
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ppb: frequency correction value
 *
 * Description: this function will set the frequency correction
 */
static int bksync_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);
    int ret = -1;
    int retry_cnt = retry_count;
    u32 cmd_status = BCM_KSYNC_FREQCOR;

    mutex_lock(&priv->ptp_lock);
    priv->shared_addr->freqcorr  = (s64)ppb;
    priv->shared_addr->ksyncinit = BCM_KSYNC_FREQCOR;
    do {
        cmd_status = priv->shared_addr->ksyncinit;
        if (cmd_status == BCM_KSYNC_DONE) {
            ret = 0;
            break;
        }
        ptp_sleep(1);
        retry_cnt--;
    } while (retry_cnt);
    mutex_unlock(&priv->ptp_lock);

    if (retry_cnt == 0) {
        DBG_ERR(("Timeout on response from R5 to adjfreq\n"));
    }

    return ret;
}

/**
 * bksync_ptp_adjtime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @delta: desired change in nanoseconds
 *
 * Description: this function will shift/adjust the hardware clock time.
 */
static int bksync_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
    int retry_cnt = retry_count;
    u32 cmd_status = BCM_KSYNC_ADJTIME;
    int ret = -1;
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);
    mutex_lock(&priv->ptp_lock);

    /* Command to R5 for the update */
    ptp_priv->shared_addr->phase_offset = delta;
    ptp_priv->shared_addr->ksyncinit = BCM_KSYNC_ADJTIME;
    do {
        cmd_status = priv->shared_addr->ksyncinit;
        if (cmd_status == BCM_KSYNC_DONE) {
            ret = 0;
            break;
        }
        ptp_sleep(1);
        retry_cnt--;
    } while (retry_cnt);
    mutex_unlock(&priv->ptp_lock);

    if (retry_cnt == 0) {
        DBG_ERR(("Timeout on response from R5 to adjtime\n"));
    }

    return ret;
}

/**
 * bksync_ptp_gettime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: pointer to hold time/result
 *
 * Description: this function will read the current time from the
 * hardware clock and store it in @ts.
 */
static int bksync_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);
    int retry_cnt = retry_count;
    int ret = -1;
    u32 cmd_status = BCM_KSYNC_GETTIME;
    s64 reftime = 0;

    mutex_lock(&priv->ptp_lock);
    priv->shared_addr->ksyncinit = BCM_KSYNC_GETTIME;
    do {
        cmd_status = priv->shared_addr->ksyncinit;
        if (cmd_status == BCM_KSYNC_DONE) {
            reftime = priv->shared_addr->ptptime;
            ret = 0;
            break;
        }
        ptp_sleep(1);
        retry_cnt--;
    } while (retry_cnt);
    mutex_unlock(&priv->ptp_lock);

    if(retry_cnt == 0) {
        DBG_ERR(("Timeout on response from R5 to gettime\n"));
    }

    *ts = ns_to_timespec64(reftime);
    return ret;
}


/**
 * bksync_ptp_settime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: time value to set
 *
 * Description: this function will set the current time on the
 * hardware clock.
 */
static int bksync_ptp_settime(struct ptp_clock_info *ptp,
        const struct timespec64 *ts)
{
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);
    s64 reftime;
    int ret = -1;
    int retry_cnt = retry_count;
    u32 cmd_status = BCM_KSYNC_SETTIME;

    reftime = timespec64_to_ns(ts);

    mutex_lock(&priv->ptp_lock);
    priv->shared_addr->ptptime   = reftime;
    priv->shared_addr->phase_offset = 0;
    priv->shared_addr->ksyncinit = BCM_KSYNC_SETTIME;
    do {
        cmd_status = priv->shared_addr->ksyncinit;
        if (cmd_status == BCM_KSYNC_DONE) {
            ret = 0;
            break;
        }
        ptp_sleep(1);
        retry_cnt--;
    } while (retry_cnt);

    mutex_unlock(&priv->ptp_lock);

    return ret;
}

static int bksync_ptp_enable(struct ptp_clock_info *ptp,
        struct ptp_clock_request *rq, int on)
{
    return 0;
}

/* structure describing a PTP hardware clock */
static struct ptp_clock_info bksync_ptp_caps = {
    .owner = THIS_MODULE,
    .name = "bksync_ptp_clock",
    .max_adj = 200000,
    .n_alarm = 0,
    .n_ext_ts = 0,
    .n_per_out = 0, /* will be overwritten in bksync_ptp_register */
    .n_pins = 0,
    .pps = 0,
    .adjfreq = bksync_ptp_adjfreq,
    .adjtime = bksync_ptp_adjtime,
    .gettime64 = bksync_ptp_gettime,
    .settime64 = bksync_ptp_settime,
    .enable = bksync_ptp_enable,
};

/**
 * bksync_ptp_hw_tstamp_enable
 *
 * @dev_no: device number
 * @port: port number
 *
 * Description: this is a callback function to enable the timestamping on
 * a given port
 */
int bksync_ptp_hw_tstamp_enable(int dev_no, int port)
{
    uint64_t portmap = 0;
    int map = 0;
    int ret = 0;

    DBG_VERB(("Enable timestamp on a given port:%d\n", port));
    if (port <= 0) {
        DBG_ERR(("Error enabling timestamp on port:%d\n", port));
        ret = -1;
        goto exit;
    }

    port -= 1;
    map = port/64; port = port%64;

    /* Update the shared structure member */
    portmap = ptp_priv->shared_addr->portmap[map];
    portmap |= (uint64_t)0x1 << port;
    ptp_priv->shared_addr->portmap[map] = portmap;
    /* Command to R5 for the update */
    ptp_priv->shared_addr->ksyncinit=BCM_KSYNC_PBM_UPDATE;

exit:
    return ret;
}

/**
 * bksync_ptp_hw_tstamp_disable
 *
 * @dev_no: device number
 * @port: port number
 *
 * Description: this is a callback function to disable the timestamping on
 * a given port
 */
int bksync_ptp_hw_tstamp_disable(int dev_no, int port)
{
    uint64_t portmap = 0;
    int map = 0;
    int ret = 0;

    DBG_VERB(("Disable timestamp on a given port:%d\n", port));
    if (port <= 0) {
        DBG_ERR(("Error disabling timestamp on port:%d\n", port));
        ret = -1;
        goto exit;
    }

    port -= 1;
    map = port/64;  port = port%64;

    /* Update the shared structure member */
    portmap = ptp_priv->shared_addr->portmap[map];
    portmap &= ~((uint64_t)0x1 << port);
    ptp_priv->shared_addr->portmap[map]= portmap;

    /* Command to R5 for the update */
    ptp_priv->shared_addr->ksyncinit = BCM_KSYNC_PBM_UPDATE;

exit:
    return ret;
}

/**
 * bksync_ptp_hw_tstamp_tx_time_get
 *
 * @dev_no: device number
 * @port: port number
 * @pkt: packet address
 * @ts: timestamp to be retrieved
 *
 * Description: this is a callback function to retrieve the timestamp on
 * a given port
 */
int bksync_ptp_hw_tstamp_tx_time_get(int dev_no, int port, uint8_t *pkt, uint64_t *ts)
{
    /* Get Timestamp from R5 or CLMAC */
    uint32_t ts_valid = 0;
    uint32_t seq_id = 0;
    uint32_t pktseq_id = 0;
    uint64_t timestamp = 0;
    uint16_t tpid = 0;
    int retry_cnt = retry_count;
    int pkt_offset;

    if (!ptp_priv || !pkt || !ts || port < 1 || port > 255)
        return -1;
    *ts = 0;

    switch(network_transport)
    {
        case 2:
            pkt_offset = 0x2c;
            break;
        case 4:
            pkt_offset = 0x48;
            break;
        case 6:
            pkt_offset = 0x2c;
            break;
        default:
            pkt_offset = 0x2c;
            break;
    }

    /* Need to check VLAN tag if packet is tagged */
    tpid = (*(pkt + 12) << 8) | *(pkt+13);
    if (tpid == 0x8100) {
        pkt_offset += 4;
    }

    pktseq_id = *((uint16_t *)(pkt + pkt_offset));


    port -= 1;
    /* Fetch the TX timestamp from shadow memory */
    do {
        ts_valid = ptp_priv->shared_addr->port_ts_data[port].ts_valid;
        if (ts_valid) {
            /* Read the entry */
            /*port_id   = ptp_priv->shared_addr->port_ts_data[port].port_id;*/
            seq_id    = ptp_priv->shared_addr->port_ts_data[port].ts_seq_id;
            timestamp = ptp_priv->shared_addr->port_ts_data[port].timestamp;

            /* Clear the shadow memory to get next entry */
            ptp_priv->shared_addr->port_ts_data[port].timestamp = 0;
            ptp_priv->shared_addr->port_ts_data[port].port_id = 0;
            ptp_priv->shared_addr->port_ts_data[port].ts_seq_id = 0;
            ptp_priv->shared_addr->port_ts_data[port].ts_valid = 0;

            if (seq_id == pktseq_id) {
                *ts = timestamp;
                ptp_priv->ts_match[port] += 1;
                break;
            } else {
                DBG_ERR(("discard timestamp on port %d Skb_SeqID %d FW_SeqId %d\n", port, pktseq_id, seq_id));
                ptp_priv->ts_discard[port] += 1;
                continue;
            }
        }
        ptp_sleep(1);
        retry_cnt--;
    } while(retry_cnt);

    ptp_priv->pkt_txctr[port] += 1;
    if (retry_cnt == 0) {
        ptp_priv->ts_timeout[port] += 1;
        DBG_ERR(("FW Response timeout: Tx TS on phy port:%d\n", port));
    }

    return 0;
}


enum {
    bxconCustomEncapVersionInvalid = 0,
    bxconCustomEncapVersionOne  = 1,

    bxconCustomEncapVersionCurrent = bxconCustomEncapVersionOne,
    bxconCustomEncapVersionReserved = 255 /* last */
} bxconCustomEncapVersion;

enum {
    bxconCustomEncapOpcodeInvalid = 0,
    bxconCustomEncapOpcodePtpRx = 1,
    bxconCustomEncapOpcodeReserved = 255 /* last */
} bxconCustomEncapOpcode;

enum {
    bxconCustomEncapPtpRxTlvInvalid = 0,
    bxconCustomEncapPtpRxTlvPtpRxTime = 1,
    bxconCustomEncapPtpRxTlvReserved = 255 /* last */
} bxconCustomEncapPtpRxTlvType;

void
bksync_dump_pkt(uint8_t *data, int size)
{
    int idx;
    char str[128];

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            sprintf(str, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            sprintf(&str[strlen(str)], "- ");
        }
        sprintf(&str[strlen(str)], "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            sprintf(&str[strlen(str)], "\n");
            gprintk(str);
        }
    }
    if ((idx & 0xf) != 0) {
        sprintf(&str[strlen(str)], "\n");
        gprintk(str);
    }
}


static inline int
bksync_pkt_custom_encap_ptprx_get(uint8_t *pkt, uint64_t *ing_ptptime)
{
    uint8_t  *custom_hdr;
    uint8_t   id[4];
    uint8_t   ver, opc;
    uint8_t   nh_type, nh_rsvd;
    uint16_t  len, tot_len;
    uint16_t  nh_len;
    uint32_t  seq_id = 0;
    uint32_t  ptp_rx_time[2];
    uint64_t  u64_ptp_rx_time = 0;

    custom_hdr = pkt;

    BKSYNC_UNPACK_U8(custom_hdr, id[0]);
    BKSYNC_UNPACK_U8(custom_hdr, id[1]);
    BKSYNC_UNPACK_U8(custom_hdr, id[2]);
    BKSYNC_UNPACK_U8(custom_hdr, id[3]);
    if (!((id[0] == 'B') && (id[1] == 'C') && (id[2] == 'M') && (id[3] == 'C'))) {
        /* invalid signature */
        return -1;
    }

    BKSYNC_UNPACK_U8(custom_hdr, ver);
    switch (ver) {
        case bxconCustomEncapVersionCurrent:
            break;
        default:
            gprintk("invalid ver\n");
            return -1;
    }

    BKSYNC_UNPACK_U8(custom_hdr, opc);
    switch (opc) {
        case bxconCustomEncapOpcodePtpRx:
            break;
        default:
            gprintk("invalid opcode\n");
            return -1;
    }


    BKSYNC_UNPACK_U16(custom_hdr, len);
    BKSYNC_UNPACK_U32(custom_hdr, seq_id);
    tot_len = len;

    /* remaining length of custom encap */
    len = len - (custom_hdr - pkt);


    /* process tlv */
    while (len > 0) {
        BKSYNC_UNPACK_U8(custom_hdr, nh_type);
        BKSYNC_UNPACK_U8(custom_hdr, nh_rsvd);
        BKSYNC_UNPACK_U16(custom_hdr, nh_len);
        len = len - (nh_len);
        if (nh_rsvd != 0x0) {
            continue; /* invalid tlv */
        }

        switch (nh_type) {
            case bxconCustomEncapPtpRxTlvPtpRxTime:
                BKSYNC_UNPACK_U32(custom_hdr, ptp_rx_time[0]);
                BKSYNC_UNPACK_U32(custom_hdr, ptp_rx_time[1]);
                u64_ptp_rx_time = ((uint64_t)ptp_rx_time[1] << 32) | (uint64_t)ptp_rx_time[0];
                *ing_ptptime = u64_ptp_rx_time;
                break;
            default:
                custom_hdr += nh_len;
                break;
        }
    }

#if 0
if (!(seq_id % 100)) {
    gprintk("****** seq_id = %d ptp time = 0x%llx\n", seq_id, u64_ptp_rx_time);
    bksync_dump_pkt(pkt, tot_len);
}
#endif

    DBG_VERB(("Custom encap header: ver=%d opcode=%d seq_id=0x%x\n", ver, opc, seq_id));

    return (tot_len);
}



/**
 * bksync_ptp_hw_tstamp_rx_time_upscale
 *
 * @dev_no: device number
 * @ts: timestamp to be retrieved
 *
 * Description: this is a callback function to retrieve 64b equivalent of
 *   rx timestamp
 */
int bksync_ptp_hw_tstamp_rx_time_upscale(int dev_no, int port, struct sk_buff *skb, uint64_t *ts)
{
    int ret = 0;
    int custom_encap_len = 0;

    /* parse custom encap header in pkt for ptp rxtime */
    custom_encap_len = bksync_pkt_custom_encap_ptprx_get((skb->data), ts);

    /* Remove the custom encap header from pkt */
    if (custom_encap_len > 0) {
        skb_pull(skb, custom_encap_len);
    }

    port -= 1;
    ptp_priv->pkt_rxctr[port] += 1;

    return ret;
}

int bksync_ptp_hw_tstamp_tx_meta_get(int dev_no, struct sk_buff *skb,
                                  uint32_t **md)
{
    uint16_t tpid = 0;
    int offset = 0;
    /* Need to check VLAN tag if packet is tagged */
    tpid = (skb->data[12] << 8) | skb->data[13];
    if (tpid == 0x8100) {
        offset = 4;
    }
    switch(network_transport)
    {
        case 2: /* IEEE 802.3 */
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                *md = &sobmhrawpkts_dcb32[offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                *md = &sobmhrawpkts_dcb26[offset];
            }
            break;
        case 4: /* UDP IPv4   */
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                *md = &sobmhudpipv4_dcb32[offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                *md = &sobmhudpipv4_dcb26[offset];
            }
            break;
        case 6: /* UDP IPv6   */
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                *md = &sobmhudpipv6_dcb32[offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                *md = &sobmhudpipv6_dcb26[offset];
            }
            break;
        default:
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                *md = &sobmhudpipv4_dcb32[offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                *md = &sobmhudpipv4_dcb26[offset];
            }
            break;
    }
    return 0;
}


int bksync_ptp_hw_tstamp_ptp_clock_index_get(int dev_no)
{
    if (!ptp_priv)
        return -1;
    return 0;
}

static void bksync_ptp_init(struct ptp_clock_info *ptp)
{
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);

    mutex_lock(&priv->ptp_lock);
    /* Set current phase_offset as ZERO */
    priv->shared_addr->phase_offset = 0;
    priv->shared_addr->ksyncinit = BCM_KSYNC_INIT;
    mutex_unlock(&priv->ptp_lock);
}

static void bksync_ptp_deinit(struct ptp_clock_info *ptp)
{
    struct bksync_ptp_priv *priv =
        container_of(ptp, struct bksync_ptp_priv, ptp_caps);

    mutex_lock(&priv->ptp_lock);
    priv->shared_addr->ksyncinit = BCM_KSYNC_DEINIT;
    mutex_unlock(&priv->ptp_lock);
}

/*
 * Device Debug Statistics Proc Entry
 */
/**
* This function is called at the beginning of a sequence.
* ie, when:
*    - the /proc/bcm/ksync/stats file is read (first time)
*   - after the function stop (end of sequence)
*
*/
static void *bksync_proc_seq_start(struct seq_file *s, loff_t *pos)
{
   static int port = 0;
   /* beginning a new sequence ? */
   if ( *pos == 0 )
   {
        seq_printf(s, "Port Bitmap : %08llx%08llx\n",
                      (uint64_t)(ptp_priv->shared_addr->portmap[1]),
                      (uint64_t)(ptp_priv->shared_addr->portmap[0]));
        seq_printf(s,"%4s| %9s| %9s| %9s| %9s| %9s| %9s|\n",
                     "Port", "RxCounter", "TxCounter", "TSTimeout", "TSRead", "TSMatch", "TSDiscard");
        port = 0;
        return &port;
   }
   /* End of the sequence, return NULL */
   *pos = 0;
   return NULL;
}

/**
* This function is called after the beginning of a sequence.
* It's called untill the return is NULL (this ends the sequence).
*
*/
static void *bksync_proc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    int *tmp_v = (int *)v;
    (*tmp_v)++;
    (*pos)++;
    /* Iterate all the ports */
    if ((*tmp_v) == BCM_NUM_PORTS)
    {
        *tmp_v = 0;
        return NULL;
    } else
        return v;
}
/**
* This function is called at the end of a sequence
*
*/
static void bksync_proc_seq_stop(struct seq_file *s, void *v)
{
    /* nothing to do, we use a static value in bksync_proc_seq_start() */
}

/**
* This function is called for each "step" of a sequence
*
*/
static int bksync_proc_seq_show(struct seq_file *s, void *v)
{
    int port = *((int *)v);
    if (ptp_priv->pkt_rxctr[port] || ptp_priv->pkt_txctr[port] ||
        ptp_priv->ts_discard[port] || ptp_priv->ts_timeout[port] ||
        ptp_priv->shared_addr->port_ts_data[port].ts_cnt || ptp_priv->ts_match[port]) {
            seq_printf(s, "%4d| %9d| %9d| %9d| %9d| %9d| %9d| %s\n", (port + 1),
                ptp_priv->pkt_rxctr[port],
                ptp_priv->pkt_txctr[port],
                ptp_priv->ts_timeout[port],
                ptp_priv->shared_addr->port_ts_data[port].ts_cnt,
                ptp_priv->ts_match[port],
                ptp_priv->ts_discard[port],
                ptp_priv->pkt_txctr[port] != ptp_priv->ts_match[port] ? "***":"");
    }
    return 0;
}

/**
* seq_operations for bsync_proc_*** entries
*
*/
static struct seq_operations bksync_proc_seq_ops = {
    .start = bksync_proc_seq_start,
    .next  = bksync_proc_seq_next,
    .stop  = bksync_proc_seq_stop,
    .show  = bksync_proc_seq_show
};
static int bksync_proc_txts_open(struct inode * inode, struct file * file)
{
    return seq_open(file, &bksync_proc_seq_ops);
}

static ssize_t
bksync_proc_txts_write(struct file *file, const char *buf,
                      size_t count, loff_t *loff)
{
    char debug_str[40];
    char *ptr;
    int port;

    if (copy_from_user(debug_str, buf, count)) {
        return -EFAULT;
    }

    if ((ptr = strstr(debug_str, "clear")) != NULL) {
        for (port = 0; port < BCM_NUM_PORTS; port++) {
            ptp_priv->pkt_rxctr[port] = 0;
            ptp_priv->pkt_txctr[port] = 0;
            ptp_priv->ts_discard[port] = 0;
            ptp_priv->ts_timeout[port] = 0;
            ptp_priv->ts_match[port] = 0;
            ptp_priv->shared_addr->port_ts_data[port].ts_cnt = 0;
        }
    } else {
        gprintk("Warning: unknown input\n");
    }

    return count;
}

struct file_operations bksync_proc_txts_file_ops = {
    owner:      THIS_MODULE,
    open:       bksync_proc_txts_open,
    read:       seq_read,
    llseek:     seq_lseek,
    write:      bksync_proc_txts_write,
    release:    seq_release,
};

static int
bksync_proc_init(void)
{
    struct proc_dir_entry *entry;

    PROC_CREATE(entry, "stats", 0666, bksync_proc_root, &bksync_proc_txts_file_ops);
    if (entry == NULL) {
        return -1;
    }
    return 0;
}

static int
bksync_proc_cleanup(void)
{
    remove_proc_entry("stats", bksync_proc_root);
    return 0;
}


/**
 * bksync_ptp_register
 * @priv: driver private structure
 * Description: this function will register the ptp clock driver
 * to kernel. It also does some house keeping work.
 */
static int bksync_ptp_register(void)
{
    int err = -ENODEV;
    dma_addr_t dma_mem = 0;

    /* Support on core-0 or core-1 */
    if (fw_core < 0 || fw_core > 1) {
        goto exit;
    }

    /* default transport is raw, ieee 802.3 */
    switch (network_transport) {
        case 2: /* IEEE 802.3 */
        case 4: /* UDP IPv4   */
        case 6: /* UDP IPv6   */
            break;
        default:
            network_transport = 2;
    }

    ptp_priv = kzalloc(sizeof(*ptp_priv), GFP_KERNEL);
    if (!ptp_priv) {
        err = -ENOMEM;
        goto exit;
    }

    /* Reset memory */
    memset(ptp_priv, 0, sizeof(*ptp_priv));

    err = -ENODEV;

    /* Initialize the Base address for CMIC and shared Memory access */
    ptp_priv->base_addr = lkbde_get_dev_virt(0);
    ptp_priv->dma_dev = lkbde_get_dma_dev(0);
    ptp_priv->dma_mem_size = 16384;/*sizeof(bksync_uc_linux_ipc_t);*/
    DBG_ERR(("Allocate shared memory with R5\n"));
    ptp_priv->shared_addr = DMA_ALLOC_COHERENT(ptp_priv->dma_dev,
                                       ptp_priv->dma_mem_size,
                                       &dma_mem);
    if (ptp_priv->shared_addr != NULL) {
        /* Reset memory */
        memset((void *)ptp_priv->shared_addr, 0, ptp_priv->dma_mem_size);

        ptp_priv->dma_mem = (uint64_t)dma_mem;
        DBG_ERR(("Shared memory allocation (%d bytes) successful at 0x%016lx.\n",
                ptp_priv->dma_mem_size, (long unsigned int)ptp_priv->dma_mem));
#ifdef __LITTLE_ENDIAN
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_12r(CMIC_CMC_BASE), 0);
#else
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_12r(CMIC_CMC_BASE), 1);
#endif
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_10r(CMIC_CMC_BASE),
                    (ptp_priv->dma_mem & 0xffffffff));
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_11r(CMIC_CMC_BASE),
                    (ptp_priv->dma_mem >> 32) & 0xffffffff);
    }

    if (debug & DBG_LVL_VERB)
        printk(KERN_EMERG"%s %p:%p\n",__FUNCTION__,
               ptp_priv->base_addr,(void *)ptp_priv->shared_addr);

    ptp_priv->ptp_caps = bksync_ptp_caps;

    mutex_init(&(ptp_priv->ptp_lock));
    bksync_ptp_init(&(ptp_priv->ptp_caps));

    /* Register ptp clock driver with bksync_ptp_caps */
    ptp_priv->ptp_clock = ptp_clock_register(&ptp_priv->ptp_caps,
            NULL);

    if (IS_ERR(ptp_priv->ptp_clock)) {
        ptp_priv->ptp_clock = NULL;
    } else if (ptp_priv->ptp_clock) {
        err = 0;

        /* Register BCM-KNET HW Timestamp Callback Functions */
        bkn_hw_tstamp_enable_cb_register(bksync_ptp_hw_tstamp_enable);
        bkn_hw_tstamp_disable_cb_register(bksync_ptp_hw_tstamp_disable);
        bkn_hw_tstamp_tx_time_get_cb_register(bksync_ptp_hw_tstamp_tx_time_get);
        bkn_hw_tstamp_tx_meta_get_cb_register(bksync_ptp_hw_tstamp_tx_meta_get);
        bkn_hw_tstamp_rx_time_upscale_cb_register(bksync_ptp_hw_tstamp_rx_time_upscale);
        bkn_hw_tstamp_ptp_clock_index_cb_register(bksync_ptp_hw_tstamp_ptp_clock_index_get);
    }

     /* Initialize proc files */
     bksync_proc_root = proc_mkdir("bcm/ksync", NULL);
     bksync_proc_init();

exit:
    return err;
}

static int bksync_ptp_remove(void)
{
    if (!ptp_priv)
        return 0;

    bksync_proc_cleanup();
    remove_proc_entry("bcm/ksync", NULL);

    /* UnRegister BCM-KNET HW Timestamp Callback Functions */
    bkn_hw_tstamp_enable_cb_unregister(bksync_ptp_hw_tstamp_enable);
    bkn_hw_tstamp_disable_cb_unregister(bksync_ptp_hw_tstamp_disable);
    bkn_hw_tstamp_tx_time_get_cb_unregister(bksync_ptp_hw_tstamp_tx_time_get);
    bkn_hw_tstamp_tx_meta_get_cb_unregister(bksync_ptp_hw_tstamp_tx_meta_get);
    bkn_hw_tstamp_rx_time_upscale_cb_unregister(bksync_ptp_hw_tstamp_rx_time_upscale);
    bkn_hw_tstamp_ptp_clock_index_cb_unregister(bksync_ptp_hw_tstamp_ptp_clock_index_get);

    DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_10r(CMIC_CMC_BASE), 0);
    DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_11r(CMIC_CMC_BASE), 0);

    /* Deinitialize the PTP */
    bksync_ptp_deinit(&(ptp_priv->ptp_caps));

    if (ptp_priv->shared_addr != NULL) {
        DMA_FREE_COHERENT(ptp_priv->dma_dev, ptp_priv->dma_mem_size,
                          (void *)ptp_priv->shared_addr, (dma_addr_t)ptp_priv->dma_mem);
        ptp_priv->shared_addr = NULL;
    }
    DBG_ERR(("Free R5 memory\n"));

    /* Unregister the bcm ptp clock driver */
    ptp_clock_unregister(ptp_priv->ptp_clock);

    /* Free Memory */
    kfree(ptp_priv);

    return 0;
}
#endif


/*
 * Generic module functions
 */

/*
 * Function: _pprint
 *
 * Purpose:
 *    Print proc filesystem information.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
    static int
_pprint(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    /* put some goodies here */
    pprintf("Broadcom BCM PTP Hardware Clock Module\n");
#else
    pprintf("Broadcom BCM PTP Hardware Clock Module not supported\n");
#endif
    return 0;
}

/*
 * Function: _init
 *
 * Purpose:
 *    Module initialization.
 *    Attached SOC all devices and optionally initializes these.
 * Parameters:
 *    None
 * Returns:
 *    0 on success, otherwise -1
 */
    static int
_init(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    bksync_ptp_register();
    return 0;
#else
    return -1;
#endif
}

/*
 * Function: _cleanup
 *
 * Purpose:
 *    Module cleanup function
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
    static int
_cleanup(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    bksync_ptp_remove();
    return 0;
#else
    return -1;
#endif
}

static gmodule_t _gmodule = {
name: MODULE_NAME,
      major: MODULE_MAJOR,
      init: _init,
      cleanup: _cleanup,
      pprint: _pprint,
      ioctl: NULL,
      open: NULL,
      close: NULL,
};

    gmodule_t*
gmodule_get(void)
{
    EXPORT_NO_SYMBOLS;
    return &_gmodule;
}
