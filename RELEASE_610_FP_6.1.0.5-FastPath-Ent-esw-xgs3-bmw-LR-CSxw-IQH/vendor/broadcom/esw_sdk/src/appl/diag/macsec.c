/*
 * $Id: macsec.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * Packet Watcher
 *
 */

#include <sal/types.h>

#ifdef INCLUDE_MACSEC

#ifndef __KERNEL__
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <soc/debug.h>
#include <soc/cm.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <appl/diag/decode.h>
#include <appl/diag/dport.h>

#include <bcm/macsec.h>
#include <bcm/error.h>

#include <bcm_int/common/macsec_cmn.h>
#include <bcm_int/esw/port.h>
#include <soc/macsecphy.h>

#include <bmacsec.h>
#include <bmacsec_cli.h>

#ifdef PLISIM
#define MACSEC_SIM 1
#endif

#ifdef MACSEC_SIM

typedef struct _macsec_sim_io_res_s {
    uint32 dev_addr;
    uint32 dev_port;
    uint32 io_addr;
    int    res_size;
#define MACSEC_SIM_MAX_DATA_SIZE 16
    uint32 data[MACSEC_SIM_MAX_DATA_SIZE];
    struct _macsec_sim_io_res_s *next;
} _macsec_sim_io_res_t;

static _macsec_sim_io_res_t *io_res_bucket[16];

int macsec_sim_init()
{
    int ii;

    for (ii = 0; ii < 16; ii++) {
        io_res_bucket[ii] = NULL;
    }
    return 0;
}

void _dump_res(int bucket)
{
    _macsec_sim_io_res_t **pentry;
    int loop_cnt = 0;

    pentry = &io_res_bucket[bucket];
    printk("Dumping bucket %d\n", bucket);
    while(*pentry) {
        printk("0x%08x : %d %d %d \n", (*pentry)->io_addr, 
                                        (*pentry)->res_size,
                                        (*pentry)->dev_addr,
                                        (*pentry)->dev_port);
        pentry = &(*pentry)->next;
        if (++loop_cnt > 10000) {
            break;
        }
    }
}

#define MACSEC_SIM_RES_BUCKET(io) (((io) >> 20) & 0xf)

_macsec_sim_io_res_t* _macsec_sim_find_res(int dev_addr, int dev_port,
                                           int wordSz, uint32 io_addr)
{
    _macsec_sim_io_res_t **pentry, *res;
    int     ii, loop_cnt = 0;

    res = io_res_bucket[MACSEC_SIM_RES_BUCKET(io_addr)];

    while(res && ((res->io_addr != io_addr) || 
                  (res->dev_addr != dev_addr))) {
        res = res->next;
        if (++loop_cnt > 1000000) {
            _dump_res(MACSEC_SIM_RES_BUCKET(io_addr));
            return NULL;
        }
    }

    if (res == NULL) {
        res = sal_alloc(sizeof(_macsec_sim_io_res_t), "macsec sim res");
        for (ii = 0; ii < MACSEC_SIM_MAX_DATA_SIZE; ii++) {
            res->data[ii] = 0xbabeadda;
        }
        res->io_addr = io_addr;
        res->res_size = wordSz;
        res->dev_addr = dev_addr;
        res->next = NULL;

        pentry = &io_res_bucket[MACSEC_SIM_RES_BUCKET(io_addr)];
        res->next = *pentry;
        *pentry = res;
    }


    return res;
}

static int _macsec_sim_do_io_op(_macsec_sim_io_res_t *res, 
                                bmacsec_io_op_t op, 
                                int wordSz, void *data)
{
    int ii, read;

    read = ((op == BMACSEC_IO_TBL_RD) || 
            (op == BMACSEC_IO_REG_RD)) ? 1 : 0;

    if (read) {
        for (ii = 0; ii < wordSz; ii++) {
            *((uint32*)data + ii) = res->data[ii];
        }
    } else {
        for (ii = 0; ii < wordSz; ii++) {
            res->data[ii] = *((uint32*)data + ii);
        }
    }
#if 0
    printk("SIM %s (0x%08x): 0x", (read ? "READ" : "WRITE"), res->io_addr);
    for (ii = 0; ii < wordSz; ii++) {
        printk("%08x ", res->data[ii]);
    }
    printk("\n");
#endif
    return 0;
}

static int macsec_sim_inited = 0;

int macsec_sim_io_handler(bcm_macsec_dev_addr_t devaddr,/* device addr*/
                          int dev_port,        /* dev port index  */
                          bmacsec_io_op_t op,  /* I/O operation   */  
                          uint32 io_addr,      /* I/O address     */
                          int wordSz,         /* Word size       */
                          int num_entry,
                          uint32 *data)
{
    _macsec_sim_io_res_t *res;

    if (macsec_sim_inited == 0) {
        macsec_sim_init();
        macsec_sim_inited = 1;
    }

    res = _macsec_sim_find_res(devaddr, dev_port, wordSz, io_addr);

    if (res == NULL) {
        return -1;
    }
    _macsec_sim_do_io_op(res, op, wordSz, data);
#if 0
    bmacsec_mmi1_io(devaddr, dev_port, op, io_addr, wordSz, data);
#endif
    return 0;
}

static int _macsec_sim_port_create(void)
{
    int ii, jj, p;
    bcm_macsec_port_config_t pCfg;

    for (ii = 0; ii < 1; ii++) {
        for (jj = 0; jj < 2; jj++) {
            p = (ii*8) + jj + 1;
            bmacsec_port_create(p,
                                BMACSEC_CORE_OCTAL_GIG,
                                0x1d, jj, macsec_sim_io_handler);
            printk("SIM: hacking port %d to be MACSEC enabled.", 
                    (ii*8) + jj + 1);
            sal_memset(&pCfg, 0, sizeof(pCfg));
            pCfg.flags = (BMACSEC_PORT_ENABLE 
                          | BMACSEC_PORT_SINGLE_CHANNEL
                          | BMACSEC_PORT_INGRESS_STORE_FORWARD
                          | BMACSEC_PORT_INGRESS_SECTAG_MISSING_FORWARD
                          | BMACSEC_PORT_INGRESS_SECTAG_INVALID_FORWARD
                          | BMACSEC_PORT_INGRESS_SECTAG_PRESERVE
                          | BMACSEC_PORT_INGRESS_SECTAG_REPLACE_VLANID
                          | BMACSEC_PORT_EGRESS_SECTAG_MISSING_UNCONTROLLED);
            pCfg.max_frame = 1518;
            pCfg.replay_window_size = 100;
            pCfg.ingress_sectag_offset = 10;
            pCfg.encap_dst_mac[5] = 0x05;
            pCfg.encap_src_mac[5] = 0x4;
            pCfg.encap_ethertype = 0xabab;

            pCfg.egress_flow_match_set[0] = 
            pCfg.egress_flow_match_set[1] = BMACSEC_FLOW_MATCH_MACSA |
                BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;

            pCfg.egress_flow_match_set[2] = BMACSEC_FLOW_MATCH_MACSA |
                BMACSEC_FLOW_MATCH_ETHTYPE;

            pCfg.egress_flow_match_set[3] = BMACSEC_FLOW_MATCH_ETHTYPE;

            pCfg.ingress_flow_match_set[0] = 
            pCfg.ingress_flow_match_set[1] = BMACSEC_FLOW_MATCH_MACSA |
                BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;

            pCfg.ingress_flow_match_set[2] = BMACSEC_FLOW_MATCH_MACSA |
                BMACSEC_FLOW_MATCH_ETHTYPE;

            pCfg.ingress_flow_match_set[3] = BMACSEC_FLOW_MATCH_ETHTYPE;


            bcm_macsec_port_config_set(0, p, &pCfg);
        }
    }
    return 0;
}

static int chanId_db[2048];
static int assocId_db[10000];

/*
 * Function: 	sh_macsec
 * Purpose:	MACSec comands
 * Parameters:	u - SOC unit #
 * Returns:	CMD_OK/CMD_FAIL/
 */
int opt_del = 1;
int opt_create = 1;
static int _macsec_test1(void)
{
    int rv = CMD_OK;
    bcm_macsec_secure_chan_t chan;
    int chanId, ii, jj, kk, port, assocId;
    bcm_macsec_secure_assoc_t assoc;
    bcm_macsec_flow_match_t flow;
    bcm_macsec_flow_action_t action;
    int     flowId, ing_chanId, egr_chanId;
    uint8   mac1[6], mac2[6];
    uint8   mac1_mask[6], mac2_mask[6];
    int     egr_cfl[32];
    int     egr_ucfl[32];
    int     ing_cfl[32];
    int     ing_ucfl[32];

    sal_memset(chanId_db, 0, sizeof(int)*2048);

    mac1[0] = 0x00;
    mac1[1] = 0x10;
    mac1[2] = 0x20;
    mac1[3] = 0x30;
    mac1[4] = 0x40;
    mac1[5] = 0x50;

    mac1_mask[0] = 0xff; 
    mac1_mask[1] = 0xff; 
    mac1_mask[2] = 0xff; 
    mac1_mask[3] = 0xff; 
    mac1_mask[4] = 0xff; 
    mac1_mask[5] = 0xff; 

    mac2_mask[0] = 0xff; 
    mac2_mask[1] = 0xff; 
    mac2_mask[2] = 0xff; 
    mac2_mask[3] = 0xff; 
    mac2_mask[4] = 0xff; 
    mac2_mask[5] = 0xff; 

    mac2[0] = 0x10;
    mac2[1] = 0x11;
    mac2[2] = 0x12;
    mac2[3] = 0x13;
    mac2[4] = 0x14;
    mac2[5] = 0x15;
    for (ii = 0; ii < 8; ii++) {
        chan.sci[ii] = 0x55 + ii;
    }


    for (ii = 0; ii < 1; ii++) {
        printk("Create Ingress channel\n");
        chan.flags = BCM_MACSEC_SECURE_CONFIDENTIAL     |
                     BCM_MACSEC_SECURE_SECTAG_TX_SCI    |
                     BMACSEC_SECURE_CHAN_INGRESS;
        port = 1;
        chanId = 100;
        rv = bcm_macsec_secure_chan_create(0, port, 
                                    BCM_MACSEC_SECURE_WITH_ID, &chan, &chanId);
        if (rv < 0) {
            printk("**** Failed to create Ingress secure channel port=%d\n", port);
        } else {
            printk("Ingress Secure channel created with ID : %d port=%d\n", chanId, port);
        }
        ing_chanId = chanId;

        for (kk = 0; kk < 4; kk++) {
            assocId = -1;
            sal_memset(&assoc, 0, sizeof(bcm_macsec_secure_assoc_t));
            assoc.an = kk;
            assoc.flags = BMACSEC_ASSOC_ENCAP_REDIRECT;
            assoc.flags |= BMACSEC_ASSOC_ACTIVE;
            assoc.secure_offset = 10;
            assoc.crypto = BMACSEC_CRYPTO_AES_128_GCM;
            for (jj = 0; jj < 16; jj++) {
                assoc.aes128_gcm.key[jj] = kk + 1;
            }
            rv = bcm_macsec_secure_assoc_create(0, port, 0, ing_chanId, &assoc, &assocId);
            if (rv < 0) {
                printk("ASSOC creation Failed\n");
            } else {
                printk("ASSOC creation OK (ID = %d) port=%d\n", assocId, port);
            }
            assocId_db[(ing_chanId * 4) + kk] = assocId;
        }

        printk("Create Egress channel\n");
        chan.flags = BCM_MACSEC_SECURE_CONFIDENTIAL     |
                     BCM_MACSEC_SECURE_SECTAG_TX_SCI;
        port = 1;
        chanId = 200;
        rv = bcm_macsec_secure_chan_create(0, port, 
                                    BCM_MACSEC_SECURE_WITH_ID, &chan, &chanId);
        if (rv < 0) {
            printk("**** Failed to create Egress secure channel port=%d\n", port);
        } else {
            printk("Egress Secure channel created with ID : %d port=%d\n", chanId, port);
        }

        egr_chanId = chanId;

        for (kk = 0; kk < 4; kk++) {
            assocId = -1;
            sal_memset(&assoc, 0, sizeof(bcm_macsec_secure_assoc_t));
            assoc.an = kk;
            assoc.flags = BMACSEC_ASSOC_ENCAP_REDIRECT;
            assoc.flags |= BMACSEC_ASSOC_ACTIVE;
            assoc.secure_offset = 10;
            assoc.crypto = BMACSEC_CRYPTO_AES_128_GCM;
            for (jj = 0; jj < 16; jj++) {
                assoc.aes128_gcm.key[jj] = kk + 1;
            }
            rv = bcm_macsec_secure_assoc_create(0, port, 0, egr_chanId, &assoc, &assocId);
            if (rv < 0) {
                printk("ASSOC creation Failed\n");
            } else {
                printk("ASSOC creation OK (ID = %d) port=%d\n", assocId, port);
            }
            assocId_db[(egr_chanId * 4) + kk] = assocId;
        }
#if 1
        printk("Creating controlled egress Flows\n");
        for (kk = 0; kk < 8; kk++) {
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;
            flow.direction = BCM_MACSEC_DIR_EGRESS;
            flow.pkt_format = BMACSEC_PACKET_FORMAT_ETH_II;
            flow.priority = 10 - kk;
            action.sectag_offset = 0;
            sal_memcpy(flow.dst_mac, mac1, 6);
            sal_memcpy(flow.dst_mac_mask, mac1_mask, 6);
            sal_memcpy(flow.src_mac, mac2, 6);
            sal_memcpy(flow.src_mac_mask, mac1_mask, 6);
            flow.ethertype = 0xdada;
            action.flags = BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD;
            action.chanId = egr_chanId;
            flowId = -1;
            if (bcm_macsec_flow_create(0, 1, 0, &flow, &action, &flowId) < 0) {
                printk("Flow create failed\n");
            } else {
                printk("Flow create OK (ID = %d)\n", flowId);
                egr_cfl[kk] = flowId;
            }
        }

    /* Add few uncontrolled flows */
        printk("Creating Uncontrolled egress Flows\n");
        for (kk = 0; kk < 5; kk++) {
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;
            flow.direction = BCM_MACSEC_DIR_EGRESS;
            flow.pkt_format = BMACSEC_PACKET_FORMAT_ETH_II;
            flow.priority = kk;
            action.sectag_offset = 0;
            printk("Creating uncontrolled flow with sa, da\n");
            sal_memcpy(flow.dst_mac, mac1, 6);
            sal_memcpy(flow.dst_mac_mask, mac1_mask, 6);
            sal_memcpy(flow.src_mac, mac2, 6);
            sal_memcpy(flow.src_mac_mask, mac1_mask, 6);
            flow.ethertype = 0xdada;
            action.flags = BCM_MACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD;
            action.chanId = -1;
            flowId = 1000 + kk;
            if (bcm_macsec_flow_create(0, 1, 0, &flow, &action, &flowId) < 0) {
                printk("Flow create failed\n");
            } else {
                printk("Flow create OK (ID = %d)\n", flowId);
                egr_ucfl[kk] = flowId;
            }
        }

        printk("Updating controlled egress Flows, making them uncontrlled\n");
        for (kk = 0; kk < 5; kk++) {
            rv = bcm_macsec_flow_get(0, port, egr_cfl[kk], &flow, &action);
            if (rv < 0) {
                printk("Failed to find the flow with ID %d\n", egr_cfl[kk]);
                continue;
            }
            /* update the flow */
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA;
            flow.priority = kk;
            action.flags = BCM_MACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD;
            action.sectag_offset = 0;
            printk("Updating flow %d\n", egr_cfl[kk]);
            if (bcm_macsec_flow_create(0, 1, BCM_MACSEC_SECURE_REPLACE,
                                       &flow, &action, &egr_cfl[kk]) < 0) {
                printk("Flow update failed\n");
            } else {
                printk("Flow update OK (ID = %d)\n", egr_cfl[kk]);
            }
        }
        printk("Updating Uncontrolled egress Flows, making them controlled\n");
        for (kk = 0; kk < 5; kk++) {
            rv = bcm_macsec_flow_get(0, port, egr_cfl[kk], &flow, &action);
            if (rv < 0) {
                printk("Failed to find the flow with ID %d\n", egr_cfl[kk]);
                continue;
            }
            /* update the flow */
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA;
            flow.priority = kk;
            action.sectag_offset = 0;
            action.chanId = egr_chanId;
            action.flags = BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD;
            printk("Updating flow %d\n", egr_cfl[kk]);
            if (bcm_macsec_flow_create(0, 1, BCM_MACSEC_SECURE_REPLACE,
                                       &flow, &action, &egr_cfl[kk]) < 0) {
                printk("Flow update failed\n");
            } else {
                printk("Flow update OK (ID = %d)\n", egr_cfl[kk]);
            }
        }
#else
        egr_chanId = 0;
        egr_cfl[0] = egr_ucfl[0] = 0;
#endif
        printk("Creating controlled Ingress Flows\n");
        for (kk = 0; kk < 8; kk++) {
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;
            flow.direction = BCM_MACSEC_DIR_INGRESS;
            flow.pkt_format = BMACSEC_PACKET_FORMAT_ETH_II;
            flow.priority = 10 - kk;
            action.sectag_offset = 0;
            sal_memcpy(flow.dst_mac, mac1, 6);
            sal_memcpy(flow.dst_mac_mask, mac1_mask, 6);
            sal_memcpy(flow.src_mac, mac2, 6);
            sal_memcpy(flow.src_mac_mask, mac1_mask, 6);
            flow.ethertype = 0xdada;
            action.flags = BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD;
            action.chanId = ing_chanId;
            flowId = -1;
            if (bcm_macsec_flow_create(0, 1, 0, &flow, &action, &flowId) < 0) {
                printk("Flow create failed\n");
            } else {
                printk("Flow create OK (ID = %d)\n", flowId);
                ing_cfl[kk] = flowId;
            }
        }

    /* Add few uncontrolled flows */
        printk("Creating Uncontrolled ingress Flows\n");
        for (kk = 0; kk < 5; kk++) {
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA |
                BMACSEC_FLOW_MATCH_ETHTYPE;
            flow.direction = BCM_MACSEC_DIR_INGRESS;
            flow.pkt_format = BMACSEC_PACKET_FORMAT_ETH_II;
            flow.priority = kk;
            action.sectag_offset = 0;
            printk("Creating uncontrolled flow with sa, da\n");
            sal_memcpy(flow.dst_mac, mac1, 6);
            sal_memcpy(flow.dst_mac_mask, mac1_mask, 6);
            sal_memcpy(flow.src_mac, mac2, 6);
            sal_memcpy(flow.src_mac_mask, mac1_mask, 6);
            flow.ethertype = 0xdada;
            action.flags = BCM_MACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD;
            action.chanId = 10;
            flowId = 1000 + kk;
            if (bcm_macsec_flow_create(0, 1, 0, &flow, &action, &flowId) < 0) {
                printk("Flow create failed\n");
            } else {
                printk("Flow create OK (ID = %d)\n", flowId);
                ing_ucfl[kk] = flowId;
            }
        }

        printk("Updating controlled Ingress Flows, making them uncontrlled\n");
        for (kk = 0; kk < 5; kk++) {
            rv = bcm_macsec_flow_get(0, port, ing_cfl[kk], &flow, &action);
            if (rv < 0) {
                printk("Failed to find the flow with ID %d\n", ing_cfl[kk]);
                continue;
            }
            /* update the flow */
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA;
            flow.priority = kk;
            action.flags = BCM_MACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD;
            action.sectag_offset = 0;
            printk("Updating flow %d\n", ing_cfl[kk]);
            if (bcm_macsec_flow_create(0, 1, BCM_MACSEC_SECURE_REPLACE,
                                       &flow, &action, &ing_cfl[kk]) < 0) {
                printk("Flow update failed\n");
            } else {
                printk("Flow update OK (ID = %d)\n", ing_cfl[kk]);
            }
        }
        printk("Updating Uncontrolled Ingress Flows, making them controlled\n");
        for (kk = 0; kk < 5; kk++) {
            rv = bcm_macsec_flow_get(0, port, ing_cfl[kk], &flow, &action);
            if (rv < 0) {
                printk("Failed to find the flow with ID %d\n", ing_cfl[kk]);
                continue;
            }
            /* update the flow */
            flow.flags = BMACSEC_FLOW_MATCH_MACSA | BMACSEC_FLOW_MATCH_MACDA;
            flow.priority = kk;
            action.sectag_offset = 0;
            action.chanId = ing_chanId;
            action.flags = BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD;
            printk("Updating flow %d\n", ing_cfl[kk]);
            if (bcm_macsec_flow_create(0, 1, BCM_MACSEC_SECURE_REPLACE,
                                       &flow, &action, &ing_cfl[kk]) < 0) {
                printk("Flow update failed\n");
            } else {
                printk("Flow update OK (ID = %d)\n", ing_cfl[kk]);
            }
        }
    }

#if 0
    printk("Testing channel destroy\n");
    for (ii = 0; ii < 1; ii++) {
        port = (ii % 16) + 1;
        rv = bcm_macsec_secure_chan_destroy(0, port, chanId_db[ii]);
        if (rv < 0) {
            printk("**** Failed to destroy secure channel (%d)\n", chanId_db[ii]);
        } else {
            printk("Secure channel destroyed with ID : %d\n", chanId_db[ii]);
            chanId_db[ii] = -1;
        }
    }
#endif

    return CMD_OK;
}

#endif /* MACSEC_SIM */

char sh_macsec_usage[] =
    "Call macsec help for more information.\n"
    "\n";


/*
 * This variable stores the current BCM unit.
 */
static int _macsec_bcm_unit = -1;

static int
macsec_port_sym_parse(char *pname, bmacsec_dev_addr_t *dev_addr, 
                      int *dev_port, bmacsec_port_t *pid)
{
    bcm_port_config_t   pcfg;
    pbmp_t              pbm;
    soc_port_t          p, dport;
    int                 unit = _macsec_bcm_unit;

    if (unit < 0) {
        return -1;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm)) {
        return -1;
    }

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        if (sal_strcasecmp(pname, BCM_PORT_NAME(unit, p)) == 0) {
            /*
             * Fill device address and device port info.
             */
#if 0 
            *dev_addr = (dport - 1)/8;;
            *dev_port = (dport - 1) % 8;
            *pid = SOC_MACSEC_PORTID(unit, dport);
            
#else
            *dev_addr = soc_property_port_get(unit, p, 
                                        spn_MACSEC_DEVICE_ADDRESS, -1);
            *dev_port = soc_property_port_get(unit, p, 
                                     spn_MACSEC_DEVICE_PORT, -1);
            *pid = SOC_MACSEC_PORTID(unit, p);
#endif
            return 0;
        }
    }

    return -1;
}


/*
 * Function: 	sh_macsec
 * Purpose:	MACSec comands
 * Parameters:	u - SOC unit #
 * Returns:	CMD_OK/CMD_FAIL/
 */
cmd_result_t
sh_macsec(int u, args_t *a)
{
    int rv = CMD_USAGE;
    bmacsec_cli_cfg_t bmacsec_cli_cfg;
    char *arg;
    int num_a;
    static int macsec_cli_registered = 0;

    _macsec_bcm_unit = u;

    if(!macsec_cli_registered) {
        bmacsec_cli_cfg.port_sym_parse = macsec_port_sym_parse;
#ifdef MACSEC_SIM
        bmacsec_cli_cfg.dev_io_f = macsec_sim_io_handler;
#else
        bmacsec_cli_cfg.dev_io_f = bmacsec_io_mmi;
#endif
        rv = bmacsec_cli_register(&bmacsec_cli_cfg);
        if(rv < 0) {
            printk("Failed to register MACSEC CLI\n");
            return CMD_FAIL;
        }
   
        macsec_cli_registered = 1;
    }

    arg = ARG_GET(a);
#ifdef MACSEC_SIM
    if (arg != NULL && sal_strcasecmp(arg, "pc") == 0) {
        _macsec_test1();
        return CMD_OK;
    }

    if (arg != NULL && sal_strcasecmp(arg, "sim_init") == 0) {
        _macsec_sim_port_create();
        return CMD_OK;
    }
#endif
    num_a = a->a_argc - 1;
    num_a = (num_a < 0) ? 0 : num_a;
    rv = bmacsec_handle_cmd(num_a, &a->a_argv[1]);
    a->a_arg = a->a_argc; /* Consume all args */
    _macsec_bcm_unit = -1;
    return rv;
}

#endif /* INCLUDE_MACSEC */
