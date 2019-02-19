/*
 * $Id: pcid.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        pcid.h
 * Purpose:     Basic include file for pcid
 */

#ifndef _PCID_PCID_H
#define _PCID_PCID_H

#include <unistd.h>
#include <stdlib.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <bde/pli/verinet.h>

#include <sal/core/thread.h>

#include <sys/types.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/dcb.h>

#define PCIM_SIZE_MAX   0x2000
#define PCIC_SIZE       0x48

    /* Return values for process request */
#define PR_REQUEST_HANDLED 1
#define PR_NO_REQUEST 2
#define PR_ALL_DONE 0
#define PR_ERROR -1

#define PACKET_CHECK_INTERVAL	500000
#define COUNTER_ACT_INTERVAL	1000000

#define PKT_SIZE_MAX	(10 * 1024 + 8)

#define CPU_MAX_PACKET_QUEUE 100
#define CPU_MAX_PACKET_SIZE 1600

/*
 * SOC internal register/memory simulation
 */

#define SOC_DATUM_HASH_SIZE	131
#define SOC_HASH_DATUM(addr)	((addr) % SOC_DATUM_HASH_SIZE)

typedef enum bcm_sim_msg_e {
    BCM_SIM_INVALID,
    BCM_SIM_ACTIVATE,
    BCM_SIM_DEACTIVATE,
    BCM_SIM_KEEP_ALIVE,
    BCM_SIM_DONE,
    BCM_SIM_TX_PKT,
    BCM_SIM_FROM_CPU,
    BCM_SIM_TO_CPU,
    BCM_SIM_SBUS_MSG_FROM_CPU,
    BCM_SIM_SBUS_MSG_TO_CPU,
    BCM_PKT_OVER_SOCKET
} bcm_sim_msg_t;

typedef struct soc_datum_s soc_datum_t;

/* Keep the data part of the following structures the same */
typedef struct sim_data_s {
    uint32 data[SOC_MAX_MEM_WORDS];
} sim_data_t;

struct soc_datum_s {
    soc_datum_t     *next;
    uint32          addr;
    uint32          data[SOC_MAX_MEM_WORDS];
};

typedef struct packet_s {
    uint8           data[PKT_SIZE_MAX];
    int             length;
    int             consum;
    uint32          dcbd[8]; 
    struct packet_s *next;
} packet_t;

typedef struct pcid_info_s pcid_info_t;

typedef void (*_pcid_tx_cb_f)(pcid_info_t *pcid_info, dcb_t *dcb, int chan);
typedef void (*_pcid_ioctl_f)(pcid_info_t *pcid_info, uint32 *args);
typedef int (*_pcid_schan_f)(pcid_info_t *pcid_info, int unit);
typedef int (*_pcid_reset_f)(pcid_info_t *pcid_info, int unit);

struct pcid_info_s {
    int             opt_port;
    int             opt_verbose;
    int             opt_pli_verbose;
    int             opt_debug;
    int             opt_noreset;
    int             opt_pktloop;
    int             opt_pktfile;
    int             opt_counter;
    int             opt_gbp_mb;
    int             opt_gbp_wid;
    int             opt_gbp_banks;
    int             opt_override_devid;
    int             opt_override_revid;
    soc_chip_types  opt_chip;
    char            *opt_chip_name;
    int             opt_i2crom;
    FILE            *i2crom_fp;
    int             opt_exit;
    uint32          pcim[PCIM_SIZE_MAX / 4];
    uint32          pcic[PCIC_SIZE / 4];
    soc_datum_t     *reg_ht[SOC_DATUM_HASH_SIZE];
    soc_datum_t     *mem_ht[SOC_DATUM_HASH_SIZE];
    int             sockfd;
    int             newsockfd;
    netconnection_t *client;
    int             unit;
    int             pkt_count;
    packet_t        *pkt_list;
    sal_mutex_t     pkt_mutex; 
    _pcid_tx_cb_f   tx_cb;
    _pcid_ioctl_f   ioctl;
     sal_sem_t      cpu_sem;
    void            *cookie;    /* To be used by applications */
    int             pkt_sockfd;
    int             pkt_newsockfd;
    int             pkt_opt_port;
    void            *pkt_cookie;    /* To be used by applications */
    soc_cm_init_t   init_data;
    _pcid_schan_f   schan_cb;
    _pcid_reset_f   reset_cb;
};

typedef struct cpu_packet_desc_s {
    int             length;
    pbmp_t          port_bitmap;
    pbmp_t          untagged_port_bitmap;
    pbmp_t          l3_port_bitmap;
    int             crc_regen;
    int             pkt_num;
    int             priority;
    unsigned char   data[CPU_MAX_PACKET_SIZE];
    unsigned int    dest_bitmap;
    uint32          opcode;
    uint32          header_type;
    uint32          dst_modid;
    uint32          dst_portid;
    uint32          src_modid;
    uint32          src_tgid;
} cpu_packet_desc_t;

void           *pcid_thread(void *cookie);
void            pcid_shutdown(pcid_info_t *);

/* in util.c */
extern int pcid_x2i(int digit);
extern void pcid_type_name(char *buffer, uint32 type);

extern void pcid_test_pkt_handler(pcid_info_t *info, dcb_t *dcb, int chan);
extern void pcid_schan_cb_set(pcid_info_t *pcid_info, _pcid_schan_f f);
extern void pcid_reset_cb_set(pcid_info_t *pcid_info, _pcid_reset_f f);

#define NEEDS_ARL_INIT(pcid_info) ((   \
    SOC_IS_STRATA(pcid_info->unit)))

#endif  /* _PCID_PCID_H */
