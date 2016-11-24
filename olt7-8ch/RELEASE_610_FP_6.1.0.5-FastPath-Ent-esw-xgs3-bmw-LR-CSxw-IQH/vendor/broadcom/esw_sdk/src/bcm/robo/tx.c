/*
 * $Id: tx.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        tx.c
 * Purpose:
 * Requires:
 *
 * Notes on internals:
 *    The basic tx function is bcm_tx.  In this case, a chain
 *    corresponds to a single packet.  The done chain interrupt (dv done)
 *    is used to indicate the end of packet for async transmission.
 *
 *    Arrays may be transmitted using bcm_tx_array; similarly, linked lists
 *    can be transmitted with bcm_tx_list.  In both cases, the
 *    tx_dv_info_t structure is associated with the DV.  Currently,
 *    only one call back is supported for a chain of packets.  It
 *    wouldn't be too difficult to have a per packet callback using
 *    the packet's call back member.
 */

#include <assert.h>

#include <soc/dma.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/util.h>
#include <soc/error.h>

#include <bcm_int/robo/tx.h>

#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

/* Forward declarations of static functions */
STATIC robo_dv_t *_robo_tx_dv_alloc(int unit, int pkt_count, int dcb_count,
                          bcm_pkt_cb_f call_back, void *cookie, int pkt_cb);
STATIC void _robo_tx_dv_free(int unit, robo_dv_t *dv);

STATIC int  _robo_tx_pkt_desc_add(int unit, bcm_pkt_t *pkt, robo_dv_t *dv);

STATIC int  _bcm_robo_tx_chain_send(int unit, robo_dv_t *dv);

STATIC void _bcm_robo_tx_chain_done_cb(int unit, robo_dv_t *dv);
STATIC void _bcm_robo_tx_chain_done(int unit, robo_dv_t *dv);
STATIC void _bcm_robo_tx_packet_done_cb(int unit, robo_dv_t *dv, robo_dcb_t *dcb);

STATIC void _bcm_robo_tx_callback_thread(void *param);

/* Some macros:
 * 
 *     _ON_ERROR_GOTO:  Check the return code of an action;
 *                      if error, goto the label provided.
 *     _PROCESS_ERROR:  If rv indicates an error, free the DV (if non-NULL)
 *                      and display the "err_msg" if provided.
 */

#define _ON_ERROR_GOTO(act, rv, label) if (((rv) = (act)) < 0) goto label

#define _PROCESS_ERROR(unit, rv, dv, err_msg) \
    do { \
        if ((rv) < 0) { /* Error detected */ \
            _robo_tx_dv_free(unit, dv); \
            if (err_msg) { \
                soc_cm_debug(DK_ERR, "bcm_tx: %s\n", err_msg); \
            } \
        } \
    } while (0)

/* Call back synchronization and lists */

static sal_sem_t         tx_cb_sem;
volatile static robo_dv_t    *dv_pend_first;
volatile static robo_dv_t    *dv_pend_last;

volatile static bcm_pkt_t *pkt_pend_first;
volatile static bcm_pkt_t *pkt_pend_last;

static int               tx_spl;

#define TX_LOCK          tx_spl = sal_splhi()
#define TX_UNLOCK        sal_spl(tx_spl)

#define DEFAULT_ROBO_TX_PRI 50               /* Default Thread Priority */

static uint8*            _pkt_pad_ptr;
static int               _tx_init;

volatile static int      _tx_chain_send;
volatile static int      _tx_chain_done;
volatile static int      _tx_chain_done_intr;



typedef struct _mtx_async_queue_s {
    struct _mtx_async_queue_s * next;
    int unit;
    bcm_pkt_t * pkt;
    void * cookie;
} _mtx_async_queue_t;

static _mtx_async_queue_t * _mtx_async_head;
static _mtx_async_queue_t * _mtx_async_tail;

static sal_mutex_t _mtx_async_queue_mx;
static sal_sem_t _mtx_cb_sem;
static sal_sem_t _mtx_async_tx_sem;

STATIC void _mtx_async_thread(void * param);


/*
 * Function:
 *      bcm_tx_pkt_setup
 * Purpose:
 *      Default packet setup routine for transmit
 * Parameters:
 *      unit         - Unit on which being transmitted
 *      tx_pkt       - Packet to update
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      tx_pkt->tx_pbmp should be set before calling this function
 *      Currently, the main thing this does is force the HG header
 *      on Hercules.
 */

int
bcm_robo_tx_pkt_setup(int unit, bcm_pkt_t *tx_pkt)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_tx_init
 * Purpose:
 *      Initialize BCM TX API
 * Parameters:
 *      unit - transmission unit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently, this just allocates shared memory space for the
 *      CRC append memory.  This memory is zero and used by all packets
 *      that require CRC append (allocate).
 *
 *      This pointer is only allocated once.  If allocation fails,
 *      BCM_E_MEMORY is returned.
 */

int
bcm_robo_tx_init(int unit) {
    if (tx_cb_sem == NULL) {
        tx_cb_sem = sal_sem_create("tx cb", sal_sem_BINARY, 0);
        if (tx_cb_sem == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* Allocate a min size pkt for padding */
    if (_pkt_pad_ptr == NULL) {
        _pkt_pad_ptr = soc_cm_salloc(unit, 64, "TX Pkt Pad");
        sal_memset(_pkt_pad_ptr, 0, 64);
    }

    if (_pkt_pad_ptr == NULL) {
        return BCM_E_MEMORY;
    }

    if (!_tx_init) {  /* Only create the thread once */
        sal_thread_t tid;

        /* Start up the tx callback handler thread */
        tid = sal_thread_create("bcmTX", SAL_THREAD_STKSZ, 
                                DEFAULT_ROBO_TX_PRI,
                                _bcm_robo_tx_callback_thread, NULL);
        if (tid < 0) {
            return BCM_E_MEMORY;
        }
    

        {
            _mtx_cb_sem = sal_sem_create("multi tx cb", 
                sal_sem_BINARY, 1);
            if (_mtx_cb_sem  == NULL) {
                return BCM_E_MEMORY;
            }

            _mtx_async_tx_sem = sal_sem_create("multi tx cb",
                    sal_sem_COUNTING, 0);
            if (_mtx_async_tx_sem  == NULL) {
                return BCM_E_MEMORY;
            }
            _mtx_async_queue_mx = 
                sal_mutex_create("multi async queue mx");
            if (_mtx_async_queue_mx  == NULL) {
                return BCM_E_MEMORY;
            }

            tid = sal_thread_create("multiAsyncTX", SAL_THREAD_STKSZ,
                                110,
                                _mtx_async_thread, NULL);
            if (tid == NULL) {
                return BCM_E_MEMORY;
            }
        }
    }
    _tx_init = TRUE;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tx
 * Purpose:
 *      Transmit a single packet
 * Parameters:
 *      unit - transmission unit
 *      pkt - The tx packet structure
 *      cookie - Callback cookie when tx done
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *  To send unicast, broadcast or multicast packets from a switch
 *  chip, one must include all the outgoing ports in the port bitmap
 *  (pkt->pi_pbmp, pkt->pi_upbmp) and include the appropriate MAC DA
 *  in the packet data.  To send different types of packet from a
 *  fabric chip, it is different and the Higig opcode
 *  (BCM_HG_OPCODE_xxx) indicates the type of packet.
 *
 *      The packet call back, if present, is used as the user
 *      "chain done" callback.  So packet callback is set to false in
 *      _robo_tx_dv_alloc.
 *
 *      If the pkt->call_back function is non-null, executes async.
 *      Otherwise, will not return until DMA completes.
 *
 *      Packet format is described by the packet flags.  In particular,
 *      if the packet has a HiGig header (as indicated in the packet's
 *      flags) it will not have a VLAN tag set up for DMA.  Care should
 *      be taken as this should only happen for Hercules (5670, 71).
 *      No provision is made for forcing the addition of both a VLAN
 * tag and a HiGig header.
 */

static int
_bcm_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    robo_dv_t *dv;
    int rv = BCM_E_NONE;
    char fmt[SOC_PBMP_FMT_LEN];
    char *err_msg = NULL;

    if (!pkt) {
        return BCM_E_PARAM;
    } else {
        if (!pkt->pkt_data || pkt->blk_count <= 0) {
            return BCM_E_PARAM;
        }
    }
    soc_cm_debug(DK_TX, "bcm_tx: pkt, u %d. len[0] %d to %s. flags 0x%x\n",
                 unit, pkt->pkt_data[0].len, SOC_PBMP_FMT(pkt->tx_pbmp, fmt),
                 pkt->flags);
    soc_cm_debug(DK_TX, "bcm_tx: dmod %d, dport %d, chan %d, op %d cos %d\n",
                 pkt->dest_mod, pkt->dest_port, pkt->dma_channel, pkt->opcode,
                 pkt->cos);

    err_msg = "Could not allocate dv/dv info";
    dv = _robo_tx_dv_alloc(unit, 1, pkt->blk_count + TX_EXTRA_DCB_COUNT,
                      pkt->call_back, cookie, pkt->call_back != NULL);
    if (!dv) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    err_msg = "Could not setup or add pkt to DV";
    _ON_ERROR_GOTO(_robo_tx_pkt_desc_add(unit, pkt, dv), rv, error);

    err_msg = "Could not send pkt";
    rv = _bcm_robo_tx_chain_send(unit, dv);

error:
    _PROCESS_ERROR(unit, rv, dv, err_msg);
    return rv;
}


static void
_mtx_tx_callback(int unit, bcm_pkt_t * pkt, void * cookie)
{
    sal_sem_t sem = * ((sal_sem_t *) cookie);
    sal_sem_give(sem);
}


STATIC int
_mtx_async_tx(int unit, bcm_pkt_t * pkt, void * cookie)
{
    _mtx_async_queue_t * item;
    
    item = soc_cm_salloc(unit, sizeof(_mtx_async_queue_t),
                 "Async packet info");
    if (item == NULL) {
        soc_cm_debug(DK_ERR, "Can't allocate packet info\n");
        return BCM_E_MEMORY;
    }

    item->unit = unit;
    item->pkt = pkt;
    item->cookie = cookie;
    item->next = NULL;

    if (sal_mutex_take(_mtx_async_queue_mx, sal_mutex_FOREVER) < 0) {
        soc_cm_debug(DK_ERR, "async tx: Can't take async TX mutex\n");
        return BCM_E_RESOURCE;
    }

    if (_mtx_async_head == NULL) {
        _mtx_async_head = item;
    } else {
        _mtx_async_tail->next = item;
    }
    _mtx_async_tail = item;
    sal_mutex_give(_mtx_async_queue_mx);

    sal_sem_give(_mtx_async_tx_sem);
    return BCM_E_NONE;
}

STATIC int
_mtx_async_queue_fetch(int * unit, bcm_pkt_t ** pkt, void ** cookie)
{
    _mtx_async_queue_t * item;

    if (sal_sem_take(_mtx_async_tx_sem, sal_sem_FOREVER) < 0) {
        soc_cm_debug(DK_ERR, "async fetch: Can't take async TX semaphore\n");
        return BCM_E_RESOURCE;
    }

    if (sal_mutex_take(_mtx_async_queue_mx, sal_mutex_FOREVER) < 0) {
        soc_cm_debug(DK_ERR, "async fetch: Can't take async TX mutex\n");
        return BCM_E_RESOURCE;
    }
    item = _mtx_async_head;
    _mtx_async_head = item->next;
    if (_mtx_async_head == NULL) {
        _mtx_async_tail = NULL;
    }

    sal_mutex_give(_mtx_async_queue_mx);

    *unit = item->unit;
    *pkt = item->pkt;
    *cookie = item->cookie;
    soc_cm_sfree(*unit, item);

    return BCM_E_NONE;
}

STATIC int
_mtx_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int pkt_cnt, pkt_cnt_tag_lo, pkt_cnt_tag_hi;
    int pkt_cnt_untag_lo, pkt_cnt_untag_hi;
    bcm_pbmp_t tx_pbmp, tx_pbmp0, tx_pbmp1;
    bcm_pbmp_t tx_upbmp, tx_upbmp0, tx_upbmp1;
    bcm_pkt_cb_f call_back;
    uint32 tmp0, tmp1;
    int rv;

    BCM_PBMP_ASSIGN(tx_pbmp, pkt->tx_pbmp);
    BCM_PBMP_ASSIGN(tx_upbmp, pkt->tx_upbmp);        

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* FE port */
        tmp0 = SOC_PBMP_WORD_GET(pkt->tx_pbmp,0) & 
            SOC_PBMP_WORD_GET(PBMP_FE_ALL(unit),0);
        /* GE port and IMP */
        tmp1 = (SOC_PBMP_WORD_GET(pkt->tx_pbmp,0) >> 24);
    } else if (SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit)){
        /* 0-23 port */
        tmp0 = SOC_PBMP_WORD_GET(pkt->tx_pbmp,0) & 0xffffff;
        /* 24 - 52 port */
        tmp1 = (SOC_PBMP_WORD_GET(pkt->tx_pbmp,1) << 8) |
                    (SOC_PBMP_WORD_GET(pkt->tx_pbmp,0)>>24);
        if (SOC_IS_ROBO5347(unit) ) {
            tmp0 = 0;
            tmp1 = SOC_PBMP_WORD_GET(pkt->tx_pbmp,0);
        }
    } else {
        tmp0 =SOC_PBMP_WORD_GET(pkt->tx_pbmp,0) ;
        tmp1 = 0;
    }

    BCM_PBMP_CLEAR(tx_pbmp0);
    BCM_PBMP_CLEAR(tx_pbmp1);
    SOC_PBMP_WORD_SET(tx_pbmp0, 0, tmp0);
    SOC_PBMP_WORD_SET(tx_pbmp1, 0, tmp1);
    
    BCM_PBMP_COUNT(tx_pbmp0, pkt_cnt_tag_lo);
    BCM_PBMP_COUNT(tx_pbmp1, pkt_cnt_tag_hi);

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
         /* FE port */
        tmp0 = SOC_PBMP_WORD_GET(pkt->tx_upbmp,0) & 
            SOC_PBMP_WORD_GET(PBMP_FE_ALL(unit),0);
        /* GE port and IMP */
        tmp1 = (SOC_PBMP_WORD_GET(pkt->tx_upbmp,0) >> 24);
    } else if (SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit)){
        /* 0-23 port */
        tmp0 = SOC_PBMP_WORD_GET(pkt->tx_upbmp,0) & 0xffffff;
        /* 24 - 52 port */
        tmp1 = (SOC_PBMP_WORD_GET(pkt->tx_upbmp,1) << 8) |
                    (SOC_PBMP_WORD_GET(pkt->tx_upbmp,0)>>24);
        if (SOC_IS_ROBO5347(unit) ) {
            tmp0 = 0;
            tmp1 = SOC_PBMP_WORD_GET(pkt->tx_upbmp,0);
        }
    } else {
        tmp0 =SOC_PBMP_WORD_GET(pkt->tx_upbmp,0) ;
        tmp1 = 0;

    }

    BCM_PBMP_CLEAR(tx_upbmp0);
    BCM_PBMP_CLEAR(tx_upbmp1);
    SOC_PBMP_WORD_SET(tx_upbmp0, 0, tmp0);
    SOC_PBMP_WORD_SET(tx_upbmp1, 0, tmp1);
   
    SOC_PBMP_AND(tx_upbmp0, tx_pbmp0);
    SOC_PBMP_AND(tx_upbmp1, tx_pbmp1);    
        
    BCM_PBMP_COUNT(tx_upbmp0, pkt_cnt_untag_lo);
    BCM_PBMP_COUNT(tx_upbmp1, pkt_cnt_untag_hi);


    call_back = pkt->call_back;
    pkt->call_back = _mtx_tx_callback;

     while ((pkt_cnt_tag_lo  > 0) ||(pkt_cnt_tag_hi > 0)) {
        /* Synchronize with the callback thread */
        sal_sem_take(_mtx_cb_sem, sal_sem_FOREVER);

        BCM_PBMP_CLEAR(pkt->tx_pbmp);
        BCM_PBMP_CLEAR(pkt->tx_upbmp);
        if (pkt_cnt_tag_lo > 0){
            /* lo bitmap */
            if (pkt_cnt_untag_lo > 0) {
                /* untag bitmap*/
                BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_upbmp0);
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_upbmp0);                
                BCM_PBMP_REMOVE(tx_pbmp0, tx_upbmp0);
                BCM_PBMP_CLEAR(tx_upbmp0);
                pkt_cnt_tag_lo -= pkt_cnt_untag_lo;
                pkt_cnt_untag_lo = 0;               
            } else {                
                /* tag bitmap only*/
                BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_pbmp0);
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_upbmp0);
                pkt_cnt_tag_lo = 0;
            }            
             pkt->dest_mod = 0;
        }else if (pkt_cnt_tag_hi > 0){
            /* hi bitmap */
            if (pkt_cnt_untag_hi > 0) {
                /* untag bitmap*/
                BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_upbmp1);
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_upbmp1);                
                BCM_PBMP_REMOVE(tx_pbmp1, tx_upbmp1);
                BCM_PBMP_CLEAR(tx_upbmp1);
                pkt_cnt_tag_hi -= pkt_cnt_untag_hi;
                pkt_cnt_untag_hi = 0;
            } else {                
                /* tag bitmap only*/
                BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_pbmp1);
                BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_upbmp1);                
                pkt_cnt_tag_hi = 0;
            }
            pkt->dest_mod = 1;
        }
        
        pkt_cnt = pkt_cnt_tag_hi + pkt_cnt_tag_lo;        
        if (pkt_cnt == 0) {
            /* For the last packet, restore the callback */
            pkt->call_back = call_back;
        }

        rv = _bcm_tx(unit, pkt, (pkt_cnt == 0)? cookie: &_mtx_cb_sem);
        if (rv < 0) {
            soc_cm_debug(DK_ERR, "_mtx_tx: %s\n", bcm_errmsg(rv));
            return rv;
        }

    }

    /* The last packet used the original callback, and _mtx_cb_mut         
     * has not been released. Do it now. */
    sal_sem_give(_mtx_cb_sem);

    /* Restore the original packet data */
    pkt->call_back = call_back;
    BCM_PBMP_ASSIGN(pkt->tx_pbmp, tx_pbmp);
    BCM_PBMP_ASSIGN(pkt->tx_upbmp, tx_upbmp);

    return BCM_E_NONE;
}

STATIC void
_mtx_async_thread(void * param)
{
    int unit;
    bcm_pkt_t * pkt;
    void * cookie;
    int rv;

    COMPILER_REFERENCE(param);

    while(1) {
        if ((rv = _mtx_async_queue_fetch(&unit, &pkt, &cookie)) < 0) {
            soc_cm_debug(DK_ERR, "Async TX: fetch: %s\n", bcm_errmsg(rv));
            break;
        }
        if ((rv = _mtx_tx(unit, pkt, cookie)) < 0) {
            soc_cm_debug(DK_ERR, "Async TX: tx: %s\n", bcm_errmsg(rv));
            break;
        }
    }
}


/*
 * Function:
 *      bcm_tx
 * Purpose:
 *      Wrapper for _bcm_tx to work around bcm5348 for
 *      transmitting to a bitmap more than 32-bit.
 * Parameters:
 *      unit - transmission unit
 *      pkt - The tx packet structure
 *      cookie - Callback cookie when tx done
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Bcm5348 transmitting bitmap >32-bit.To emulate this
 *      functionality, bcm_tx iterates over the requested bitmaps. 
 *      The callback, if applicable, shall be called only once.
 */

int
bcm_robo_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int rv = BCM_E_NONE;
    int pkt_cnt;
    BCM_PBMP_COUNT(pkt->tx_pbmp, pkt_cnt);
    if ((pkt_cnt < 1) || (BCM_PKT_TX_ETHER(pkt))) {
        return _bcm_tx(unit, pkt, cookie);
    } else if (pkt->call_back == NULL) {
        return _mtx_tx(unit, pkt, cookie);
    } else {
        return _mtx_async_tx(unit, pkt, cookie);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tx_array
 * Purpose:
 *      Transmit an array of packets
 * Parameters:
 *      unit - transmission unit
 *      pkt - array of pointers to packets to transmit
 *      count - Number of packets in list
 *      all_done_cb - Callback function (if non-NULL) when all pkts trx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If all_done_cb is non-NULL, the packets are sent asynchronously
 *      (the routine returns before all the pkts are sent)
 *
 *      The "packet callback" will be set according to the value
 *      in each packet's "call_back" member, so that must be initialized
 *      for all packets in the chain.
 *
 *      If any packet requires a callback, the packet-done callback is
 *      enabled for all packets in the chain.
 *
 *      This routine does not support tunnelling to a remote CPU for
 *      forwarding.
 *
 *      CURRENTLY:  The TX parameters, src mod, src port, PFM
 *      and internal priority, are determined by the first packet in
 *      the array.  This may change to break the array into subchains
 *      when differences are detected.
 */

int
bcm_robo_tx_array(int unit, bcm_pkt_t **pkt, int count, bcm_pkt_cb_f all_done_cb,
             void *cookie)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_tx_list
 * Purpose:
 *      Transmit a linked list of packets
 * Parameters:
 *      unit - transmission unit
 *      pkt - Pointer to linked list of packets
 *      all_done_cb - Callback function (if non-NULL) when all pkts trx'd
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If callback is non-NULL, the packets are sent asynchronously
 *      (the routine returns before all the pkts are sent)
 *
 *      The "packet callback" will be set according to the value
 *      in each packet's "call_back" member, so that must be initialized
 *      for all packets in the chain.
 *
 *      The "next" member of the packet is used for the linked list.
 *      CAREFUL:  The internal _next member is not used for this.
 *
 *      This routine does not support tunnelling to a remote CPU for
 *      forwarding.
 *
 *      The TX parameters, src mod, src port, PFM and internal priority,
 *      are currently determined by the first packet in the list.
 */

int
bcm_robo_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, void *cookie)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      _bcm_robo_tx_chain_send
 * Purpose:
 *      Send out a chain of one or more packets
 */

STATIC int
_bcm_robo_tx_chain_send(int unit, robo_dv_t *dv)
{
    ++_tx_chain_send;
    if (dv->dv_done_packet) { /* Send async */
        soc_cm_debug(DK_TX, "bcm_tx: async send\n");
        SOC_IF_ERROR_RETURN(soc_robo_dma_start(unit, dv));
    } else { /* Send sync */
        soc_cm_debug(DK_TX, "bcm_tx: sync send\n");
        SOC_IF_ERROR_RETURN(soc_robo_dma_wait(unit, dv));
            _bcm_robo_tx_chain_done_cb(unit, dv);
    }

    return BCM_E_NONE;
}               
    
/****************************************************************
 *
 * Map a destination MAC address to port bitmaps.  Uses
 * the dest_mac passed as a parameter.  Sets the
 * bitmaps in the pkt structure.
 *
 ****************************************************************/

/*
 * Function:
 *      bcm_tx_pkt_l2_map
 * Purpose:
 *      Resolve the packet's L2 destination and update the necessary
 *      fields of the packet.
 * Parameters:
 *      unit - Transmit unit
 *      pkt - Pointer to pkt being transmitted
 *      dest_mac - use for L2 lookup
 *      vid - use for L2 lookup
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      
 */

int
bcm_robo_tx_pkt_l2_map(int unit, bcm_pkt_t *pkt, bcm_mac_t dest_mac, int vid)
{
    return BCM_E_UNAVAIL;
}               
    
/****************************************************************
 *
 * Functions to setup DVs and DCBs.
 *    _robo_tx_dv_alloc:         Allocate and init a TX DV
 *    _robo_tx_pkt_desc_add:     Add all the descriptors for a packet to a DV
 *
 * Minor functions:
 *    _robo_dcb_flags_get:      Calculate the DCB flags.
 *    _robo_get_mac_vlan_ptrs:  Determine DMA pointers for src_mac and
 *                              vlan.  This is in case the src_mac is not
 *                              in the same pkt block as the dest_mac; or
 *                              the packet is untagged.  Also sets the
 *                              byte and block offsets for data that follows.
 * 
 ****************************************************************/



/*
 * Function:
 *      _robo_tx_dv_alloc
 * Purpose:
 *      Allocate a DV, a dv_info structure and initialize.
 * Parameters:
 *      unit - Robo unit number for allocation
 *      dcb_count - The number of DCBs to provide
 *      pkt - Pointer to packet structure to provide to tx_info structure
 *      call_back - Chain done callback function
 *      cookie - User cookie used for chain done and packet callbacks.
 *      per_pkt_cb - Do we need to callback on each packet complete?
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Always sets the dv_done_chain callback; sets packet done
 *      call back according to per_pkt_cb parameter.
 */

STATIC robo_dv_t *
_robo_tx_dv_alloc(int unit, int pkt_count, int dcb_count, 
             bcm_pkt_cb_f chain_done_cb, void *cookie, int per_pkt_cb)
{
    robo_dv_t *dv;
    tx_dv_info_t *dv_info;

    if (pkt_count > SOC_DV_PKTS_MAX) {
        soc_cm_debug(DK_ERR, "TX array:  Cannot TX more than %d pkts. "
                     "Attempting %d.\n", SOC_DV_PKTS_MAX, pkt_count);
        return NULL;
    }

    if ((dv = soc_robo_dma_dv_alloc(unit, DV_TX, dcb_count)) == NULL) {
        return NULL;
    }
    
    if ((dv_info = sal_alloc(sizeof(tx_dv_info_t), "tx_dv")) == NULL) {
        soc_robo_dma_dv_free(unit, dv);
        return NULL;
    }
    sal_memset(dv_info, 0, sizeof(tx_dv_info_t));
    TX_INFO_SET(dv, dv_info);
    TX_INFO(dv)->cookie = cookie;
    TX_INFO(dv)->chain_done_cb = chain_done_cb;
    dv->dv_done_chain = _bcm_robo_tx_chain_done_cb;
    if (per_pkt_cb) {
        dv->dv_done_packet = _bcm_robo_tx_packet_done_cb;
    }

    return dv;
}               
                        
/*
 * Function:
 *      _robo_tx_dv_free
 * Purpose:
 *      Free a DV and associated dv_info structure
 * Parameters:
 *      unit - Robo unit number for allocation
 *      dv - The DV to free
 * Returns:
 *      None
 */

STATIC void
_robo_tx_dv_free(int unit, robo_dv_t *dv)
{
    if (dv) {
        if (TX_INFO(dv)) {
            sal_free(TX_INFO(dv));
        }
        soc_robo_dma_dv_free(unit, dv);
    }
}               
                       
/* Set up the DV for bcm_tx */
STATIC INLINE uint32
_robo_dcb_flags_get(int unit, bcm_pkt_t *pkt, robo_dv_t *dv)
{
    uint32 dcb_flags = 0;

    dcb_flags |= SOC_DMA_COS(pkt->cos);
    /* This level does not handle CRC append, but marks for HW to generate */
    if (pkt->flags & (BCM_TX_CRC_APPEND | BCM_TX_CRC_REGEN)) {
        dcb_flags |= SOC_DMA_CRC_REGEN;
    }

    return dcb_flags;
}

/*
 * Based on packet settings, get pointers to vlan, and src mac
 * Update the "current" block and byte offset
 */

STATIC INLINE void
_robo_get_mac_vlan_ptrs(int unit, robo_dv_t *dv, bcm_pkt_t *pkt, uint8 **src_mac,
                   uint8 **vlan_ptr, int *block_offset, int *byte_offset)
{
    uint16  in_tag_tpid = 0, dev_tpid = ENET_DEFAULT_TPID;
    int     dt_mode = BCM_PORT_DTAG_MODE_NONE;
    
    /* check if Double tag is enabled or supported */
    if (bcm_port_dtag_mode_get(unit, CMIC_PORT(unit), &dt_mode)) {
        dt_mode = BCM_PORT_DTAG_MODE_NONE;
    } else {
        if (dt_mode != BCM_PORT_DTAG_MODE_NONE) {   /* DT_mode enabled */
            /* assign the TPID : 
             *  - if DT_mode is enabled, set the device TPID as SP-TPID.
             *  - else the device TPID will be 1Q TPID always(0x8100)
             */
            if (bcm_port_tpid_get(unit, CMIC_PORT(unit), &dev_tpid)){
                dev_tpid = ENET_DEFAULT_TPID;
            }
        }
    }
    
    /* Assume everything is in block 0 */
    *src_mac = &pkt->pkt_data[0].data[sizeof(bcm_mac_t)];
    *block_offset = 0;

    if (BCM_PKT_NO_VLAN_TAG(pkt)) { /* Get VLAN from _vtag pkt member */
        *byte_offset = 2 * sizeof(bcm_mac_t);
    /*
        sal_memcpy(SOC_DV_VLAN_TAG(dv), pkt->_vtag, sizeof(uint32));
        *vlan_ptr = SOC_DV_VLAN_TAG(dv);
    */
        *vlan_ptr = NULL;

        if (pkt->pkt_data[0].len < 2 * sizeof(bcm_mac_t)) {
            /* Src MAC in block 1 */
            *src_mac = pkt->pkt_data[1].data;
            *block_offset = 1;
            *byte_offset = sizeof(bcm_mac_t);
        }
    } else { /* Packet has VLAN tag */
        
        *byte_offset = 2 * sizeof(bcm_mac_t) + ENET_TAG_SIZE;
        *vlan_ptr = &pkt->pkt_data[0].data[2 * sizeof(bcm_mac_t)];
        in_tag_tpid = *(uint16 *)(*vlan_ptr);

        if (pkt->pkt_data[0].len < 2 * sizeof(bcm_mac_t)) {
            /* Src MAC in block 1; assume VLAN there too at first */
            *src_mac = pkt->pkt_data[1].data;
            *vlan_ptr = &pkt->pkt_data[1].data[sizeof(bcm_mac_t)];
            in_tag_tpid = *(uint16 *)(*vlan_ptr);
            *block_offset = 1;
            *byte_offset = sizeof(bcm_mac_t) + ENET_TAG_SIZE;
            if (((*vlan_ptr)[0] != (dev_tpid >> 8)) ||
                    ((*vlan_ptr)[1] != (dev_tpid & 0xf))) {
                *vlan_ptr = NULL;
                *byte_offset -= ENET_TAG_SIZE;
            }

            if (pkt->pkt_data[1].len < sizeof(bcm_mac_t) + ENET_TAG_SIZE) {
                /* Oops, VLAN in block 2 */
                *vlan_ptr = pkt->pkt_data[2].data;
                in_tag_tpid = *(uint16 *)(*vlan_ptr);
                *block_offset = 2;
                *byte_offset = ENET_TAG_SIZE;
                if (((*vlan_ptr)[0] != (dev_tpid >> 8)) ||
                        ((*vlan_ptr)[1] != (dev_tpid & 0xf))) {
                    *vlan_ptr = NULL;
                    *byte_offset -= ENET_TAG_SIZE;
                }
            }
        } else if (pkt->pkt_data[0].len <
                   2 * sizeof(bcm_mac_t) + sizeof(uint32)) {
            /* VLAN in block 2 */
            *block_offset = 1;
            *byte_offset = ENET_TAG_SIZE;
            *vlan_ptr = pkt->pkt_data[1].data;
            in_tag_tpid = *(uint16 *)(*vlan_ptr);
            if (((*vlan_ptr)[0] != (dev_tpid >> 8)) ||
                    ((*vlan_ptr)[1] != (dev_tpid & 0xf))) {
                *vlan_ptr = NULL;
                *byte_offset -= ENET_TAG_SIZE;
            }

        } else if (((*vlan_ptr)[0] != (dev_tpid >> 8)) ||
                   ((*vlan_ptr)[1] != (dev_tpid & 0xf))) {
            *vlan_ptr = NULL;
            *byte_offset -= ENET_TAG_SIZE;
        }
    }
}				
    

/*
 * Function:
 *      _robo_tx_pkt_desc_add
 * Purpose:
 *      Add all descriptors to a DV for a given packet.
 * Parameters:
 *      unit - Roboswitch device ID
 *      pkt - Pointer to packet (cookie) placed in info structure
 *      dv - DCB vector to update
 *      dcb_flags - Added directly to DCB
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Uses tx info pkt_count member to get packet index; then advances
 *      the pkt_count member.
 *
 *      See orion/doc/pkt.txt for info about restrictions on MACs,
 *      and the VLAN not crossing block boundaries
 *
 *      The index of the packet in the DV is always 0 for TX.
 * 
 * Devices : Special behavior listed.
 *  1. bcm53115 : Tag/Untag enforcement is bypast when iDT_mode enabled.
 *      - in iDT_mode, all tag/untag action will be handled by EVR table.
 *      a. CPU->NNI : The SP-Tag will always be tagged when iDT_mode.
 *      b. CPU->UNI : The SP-Tag will always be untagged when iDT_mode.
 */

STATIC int
_robo_tx_pkt_desc_add(int unit, bcm_pkt_t *pkt, robo_dv_t *dv)
{
    int byte_offset = 0;
    int pkt_len = 0;  /* Length calculation for byte padding */
    int tmp_len;
    int min_len = 60; /* Min length pkt before padded */
    uint8 *vlan_ptr;
    uint8 *src_mac;
    int block_offset = 0;
    int i;
    bcm_vlan_t default_vid;
    uint32 dcb_flags = 0;
    uint32 crc;
    uint32 tag_value;
    uint8 *pp;

    /* Calculate the DCB flags for this packet. */
    dcb_flags = _robo_dcb_flags_get(unit, pkt, dv);

    /* Just set up the packet as is and return */
    if (pkt->flags & BCM_TX_FAST_PATH) {
        for (i = 0; i < pkt->blk_count; i++) {
            SOC_IF_ERROR_RETURN(soc_robo_dma_desc_add(dv,
                (sal_vaddr_t) pkt->pkt_data[i].data, pkt->pkt_data[i].len));
        }

        return BCM_E_NONE;
    }

    if (pkt->pkt_data[0].len < sizeof(bcm_mac_t)) {
        return BCM_E_PARAM;
    }

    /* Get pointers to srcmac and vlan; check if bad block count */
    _robo_get_mac_vlan_ptrs(unit, dv, pkt, &src_mac, &vlan_ptr, &block_offset,
                       &byte_offset);
    if (block_offset >= pkt->blk_count) {
        return BCM_E_PARAM;
    }
    if (byte_offset >= pkt->pkt_data[block_offset].len) {
        byte_offset = 0;
        block_offset++;
    }

    /* Set up pointer to the packet in TX info structure. */
    TX_INFO_PKT_ADD(dv, pkt);

    /* Fill brcm proprietary tag */
    if (BCM_PBMP_IS_NULL(pkt->tx_pbmp)) {
    /* no specified port, force h/w to search l2 table for forwarding */
        dv->dv_opcode = BRCM_OP_UCAST;
        if (soc_feature(unit, soc_feature_tag_enforcement)){
            soc_cm_debug(DK_TX, "TAG_NO_ENFORCEMENT \n");
            if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {                
                dv->dv_te_53242 = TAG_NO_ENFORCEMENT;
            } else if (SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
                dv->dv_te_53115 = TAG_NO_ENFORCEMENT;
            } else {
                dv->dv_te = TAG_NO_ENFORCEMENT;
            }
        }
    } else {
        uint32 pbmp;
        SOC_PBMP_PORT_REMOVE(pkt->tx_pbmp, CMIC_PORT(unit));
        pbmp = SOC_PBMP_WORD_GET( pkt->tx_pbmp,0);

        /*  Use the largest range (dv_dst_pbmp 29-bit) in BRCM tag 
          *  for general usage.
          *  Chips which have another meaningful bits covered in the range
          *  need to handle those meaninful bits later in case these bits were
          *  override here.
          */
        dv->dv_dst_pbmp = pbmp;
        
        if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {           
            if (pkt->dest_mod == 0) {
                dv->dv_opcode = BRCM_OP_MULTI_PORT_LS;
            } else if (pkt->dest_mod == 1) {
                dv->dv_opcode = BRCM_OP_MULTI_PORT_IMP_HS;
            }
        } else if (SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
            if (pkt->dest_mod == 0) {
                dv->dv_opcode = BRCM_OP_MCAST;
            } else if (pkt->dest_mod == 1) {
                dv->dv_opcode = BRCM_OP_MULTI_PORT_24_52;
            }
        } else {
            if (pkt->dest_mod == 0) {              
                dv->dv_opcode = BRCM_OP_MULTI_PORT;           
            } else if (pkt->dest_mod == 1) {
                dv->dv_opcode = BRCM_OP_MULTI_PORT_24_52; 
            }
        }

        /* Handel tag enforcement/traffic class queue bits */
        if (soc_feature(unit, soc_feature_tag_enforcement)){
            
            if (SOC_IS_ROBO5395(unit)) {
                if ((pkt->flags & BCM_TX_TIME_STAMP_REPORT) ||
                    (pkt->flags & BCM_PKT_F_TIMESYNC)){
                    dv->dv_ts = 1;
                }
                dv->dv_tc = pkt->prio_int;
            } else if (SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
                if ((pkt->flags & BCM_TX_TIME_STAMP_REPORT) ||
                    (pkt->flags & BCM_PKT_F_TIMESYNC)) {
                    dv->dv_ts_53115 = 1;
                }
                dv->dv_tc_53115 = pkt->prio_int;
            } else {
                dv->dv_tc_53242 = pkt->prio_int;
            }

            if (SOC_PBMP_IS_NULL(pkt->tx_upbmp) && (vlan_ptr !=NULL)) {
                soc_cm_debug(DK_TX, "TAG_ENFORCEMENT \n");
                if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {                     
                    dv->dv_te_53242 = TAG_ENFORCEMENT;
                } else if (SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
                    dv->dv_te_53115 = TAG_ENFORCEMENT;
                } else {
                    dv->dv_te = TAG_ENFORCEMENT;
                }
            } else {
                soc_cm_debug(DK_TX, "UNTAG_ENFORCEMENT \n");
                if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {                     
                    dv->dv_te_53242 = UNTAG_ENFORCEMENT;
                } else if (SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
                    dv->dv_te_53115 = UNTAG_ENFORCEMENT;
                } else {
                    dv->dv_te = UNTAG_ENFORCEMENT;
                }
            }
        }

    }

    /* Destination MAC */
    ENET_COPY_MACADDR(pkt->pkt_data[0].data, SOC_ROBO_DV_DEST_MAC(dv));

    /* Source MAC */
    ENET_COPY_MACADDR(src_mac, SOC_ROBO_DV_SRC_MAC(dv));

    /* BRCM type/tag */
    tmp_len = 2*ENET_MAC_SIZE;
    pkt_len = 2*ENET_MAC_SIZE;

    /* No BRCM type (2 bytes) for BCM53115, BCM53118 */
    if ((!SOC_IS_ROBO53115(unit)) && (!SOC_IS_ROBO53118(unit))) {
        SOC_ROBO_DV_BRCM_TAG(dv)[0] = ENET_DEFAULT_BRCMID >> 8;
        SOC_ROBO_DV_BRCM_TAG(dv)[1] = ENET_DEFAULT_BRCMID & 0xff;
    }
    tag_value = bcm_htonl(dv->dv_brcm_tag);

    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
        pp = SOC_ROBO_DV_BRCM_TAG(dv);
    } else {
        pp = SOC_ROBO_DV_BRCM_TAG(dv) + 2;
    }
    sal_memcpy(pp, (uint8 *)&tag_value, 4);

    /* BRCM Tag size = 4 bytes(ENET_BRCM_TAG_SIZE - 2) for BCM53115, BCM53118 */
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
        tmp_len += ENET_BRCM_SHORT_TAG_SIZE;
    } else {
        tmp_len += ENET_BRCM_TAG_SIZE;
    }

    /* VLAN tag */
    if (vlan_ptr) {
        bcm_pbmp_t tmp_pbmp;

        BCM_PBMP_ASSIGN(tmp_pbmp, pkt->tx_pbmp);
        BCM_PBMP_AND(tmp_pbmp, pkt->tx_upbmp);

        if (soc_feature(unit, soc_feature_tag_enforcement)){     
            /* for chips with soc_feature_tag_enforcement  
                packet must have tag */
            if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                sal_memcpy(SOC_ROBO_DV_VLAN_TAG_53115(dv), vlan_ptr, ENET_TAG_SIZE);
            } else {
                sal_memcpy(SOC_ROBO_DV_VLAN_TAG(dv), vlan_ptr, ENET_TAG_SIZE);
            }
            tmp_len += ENET_TAG_SIZE;
            pkt_len += ENET_TAG_SIZE;
            min_len += ENET_TAG_SIZE;
        } else {
            if (BCM_PBMP_IS_NULL(pkt->tx_pbmp) || BCM_PBMP_IS_NULL(tmp_pbmp)) {
                /* No BRCM type (2 bytes) for BCM53115, BCM53118 : 
                  * shift SOC_ROBO_DV_VLAN_TAG started memory location
                  */
                if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                    sal_memcpy(SOC_ROBO_DV_VLAN_TAG_53115(dv), vlan_ptr, ENET_TAG_SIZE);
                } else {
                    sal_memcpy(SOC_ROBO_DV_VLAN_TAG(dv), vlan_ptr, ENET_TAG_SIZE);
                }
                tmp_len += ENET_TAG_SIZE;
                pkt_len += ENET_TAG_SIZE;
                min_len += ENET_TAG_SIZE;
            }
        }
    } else {
        if (soc_feature(unit, soc_feature_tag_enforcement)){     
            uint16  cpu_tpid = 0;
            int     dt_mode = 0;
            bcm_port_t  cpu;
            
            /* check double tagging mode first */
            cpu = CMIC_PORT(unit);
            cpu_tpid = ENET_DEFAULT_TPID;
            
            if ( bcm_port_dtag_mode_get(unit, cpu, &dt_mode) == BCM_E_NONE){
                if (dt_mode != BCM_PORT_DTAG_MODE_NONE){ /* DT_mode enabled */
                    if (bcm_port_tpid_get(unit, cpu, &cpu_tpid)){
                        cpu_tpid = ENET_DEFAULT_TPID;
                    }
                }
            }
            /* for chips with soc_feature_tag_enforcement  
                packet must have tag */
            bcm_vlan_default_get(unit, &default_vid);
            /* No BRCM type (2 bytes) for BCM53115 : 
              * shift SOC_ROBO_DV_VLAN_TAG started memory location
              */
            if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                SOC_ROBO_DV_VLAN_TAG_53115(dv)[0] = cpu_tpid >> 8;
                SOC_ROBO_DV_VLAN_TAG_53115(dv)[1] = cpu_tpid & 0xff;
                SOC_ROBO_DV_VLAN_TAG_53115(dv)[2] = default_vid >> 8;
                SOC_ROBO_DV_VLAN_TAG_53115(dv)[3] = default_vid & 0xff;

            } else {
                SOC_ROBO_DV_VLAN_TAG(dv)[0] = cpu_tpid >> 8;
                SOC_ROBO_DV_VLAN_TAG(dv)[1] = cpu_tpid & 0xff;
                SOC_ROBO_DV_VLAN_TAG(dv)[2] = default_vid >> 8;
                SOC_ROBO_DV_VLAN_TAG(dv)[3] = default_vid & 0xff;
            }
            tmp_len += ENET_TAG_SIZE;
            pkt_len += ENET_TAG_SIZE;
            min_len += ENET_TAG_SIZE;
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_robo_dma_desc_add(dv, (sal_vaddr_t)dv->dv_dmabufhdr, tmp_len));

    /*
     * If byte offset indicates we're not at the end of a packet's data
     * block, add a DCB for the rest of the current block.
     */
    if (byte_offset != 0) {
        if ((block_offset == (pkt->blk_count - 1)) &&
        ((pkt->flags & BCM_TX_CRC_FLD) == BCM_TX_CRC_REGEN)) {
            tmp_len = pkt->pkt_data[block_offset].len -
              sizeof(uint32) - byte_offset;
    } else {
            tmp_len = pkt->pkt_data[block_offset].len - byte_offset;
    }
        SOC_IF_ERROR_RETURN(soc_robo_dma_desc_add(dv,
            (sal_vaddr_t) &pkt->pkt_data[block_offset].data[byte_offset],
        tmp_len));
        block_offset++;
        pkt_len += tmp_len;
    }

    /* Add DCBs for the remainder of the blocks. */
    for (i = block_offset; i < pkt->blk_count; i++) {
        if ((i == (pkt->blk_count - 1)) &&
        ((pkt->flags & BCM_TX_CRC_FLD) == BCM_TX_CRC_REGEN)) {
        /* strip out the CRC in packet */
            tmp_len = pkt->pkt_data[i].len - sizeof(uint32);
    } else {
            tmp_len = pkt->pkt_data[i].len;
    }

        SOC_IF_ERROR_RETURN(soc_robo_dma_desc_add(dv,
            (sal_vaddr_t) pkt->pkt_data[i].data, tmp_len));
        pkt_len += tmp_len;
    }

    /* Pad runt packets */
    if ((pkt_len < min_len) && !(pkt->flags & BCM_TX_NO_PAD)) {
        SOC_IF_ERROR_RETURN(soc_robo_dma_desc_add(dv,
               (sal_vaddr_t) _pkt_pad_ptr, min_len - pkt_len));
    pkt_len = min_len;
    }

    /* There is no inner CRC in original frame data for BCM53115, BCM53118 */
    if ((!SOC_IS_ROBO53115(unit)) && (!SOC_IS_ROBO53118(unit))) {
        /* Calculate Inner CRC */
        crc = cal_crc32(dv, 1);
#ifdef BE_HOST
        crc = _shr_swap32(crc);
#endif
        sal_memcpy((uint8 *)dv->dv_dmabufcrc, &crc, sizeof(uint32));
        pkt_len += sizeof(uint32);
#if 0
        /* Override previous value due to update actual len */
        dv->dv_len = pkt_len;
        pp = SOC_ROBO_DV_BRCM_TAG(dv) + 2;
        tag_value = bcm_htonl(dv->dv_brcm_tag);
        sal_memcpy(pp, (uint8 *)&tag_value, 4);
#endif
        /* Add Inner CRC */
        SOC_IF_ERROR_RETURN
            (soc_robo_dma_desc_add(dv, (sal_vaddr_t)dv->dv_dmabufcrc, sizeof(uint32)));

#if 0
        /* Calculate Outer CRC */
        crc = cal_crc32(dv, 0);
        sal_memcpy((uint8 *)dv->dv_dmabufcrc + sizeof(uint32),
               &crc, sizeof(uint32));

        /* Adjust length of last added desc to include the Outer CRC */
        dv->dv_dcb[dv->dv_vcnt-1].len += sizeof(uint32);
        dv->dv_length += sizeof(uint32);
#endif
    }

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Functions to handle callbacks:
 *
 *   _bcm_robo_tx_callback_thread: The non-interrupt thread that manages
 *                                 packet done completions.
 *   _bcm_robo_tx_chain_done_cb:   The interrupt handler callback for chain
 *                                 done.  Adds dv-s to pending list.
 *   _bcm_robo_tx_chain_done:      Handles the dv_done in non-interrupt
 *                                 context.  Makes user level call back and
 *                                 frees the DV.
 *   _bcm_robo_tx_packet_done_cb:  Interrupt handler for packet done.
 *                                 Adds pkt to callback list if needed.
 *
 ****************************************************************/


/*
 * Function:
 *      _bcm_robo_tx_callback_thread
 * Purpose:
 *      Non-interrupt tx callback context
 * Parameters:
 *      param - ignored
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently assumes that the packet done interrupt will
 *      be handled before (or at same time) as the corresponding
 *      chain done interrupt.  This only matters when there's
 *      a per-packet callback since that refers to the cookie
 *      in the dv.
 */

STATIC void
_bcm_robo_tx_callback_thread(void *param)
{
    robo_dv_t *dv_list, *dv, *dv_next;
    bcm_pkt_t *pkt_list, *pkt, *pkt_next;

    COMPILER_REFERENCE(param);

    while (1) {
        if (sal_sem_take(tx_cb_sem, sal_sem_FOREVER) < 0) {
            soc_cm_debug(DK_ERR, "TX callback thread error\n");
            break;
        }

        TX_LOCK; /* Grab the lists from the interrupt handler */
        dv_list = (robo_dv_t *)dv_pend_first;
        dv_pend_first = dv_pend_last = NULL;
        pkt_list = (bcm_pkt_t *)pkt_pend_first;
        pkt_pend_first = pkt_pend_last = NULL;
        TX_UNLOCK;

        /*
         * Handle the per pkt callbacks first (which use DVs),
         * then the DVs.  See notes above
         */
        pkt = pkt_list;
        while (pkt) {
            pkt_next = pkt->_next;
            (pkt->call_back)(pkt->unit, pkt,
                             TX_INFO((robo_dv_t *)(pkt->_dv))->cookie);
            pkt = pkt_next;
        }

        dv = dv_list;
        while (dv) {
            /* Chain done may deallocate dv */
            dv_next = TX_DV_NEXT(dv);
            _bcm_robo_tx_chain_done(dv->dv_unit, dv);
            dv = dv_next;
        }
    }
}

/*
 * Function:
 *      _bcm_robo_tx_chain_done_cb
 * Purpose:
 *      Interrupt handler callback to schedule chain done processing
 * Parameters:
 *      unit - SOC unit #
 *      dv - pointer to dv that has completed.
 * Returns:
 *      Nothing
 * Notes:
 *      All the handler does is put the DV on a queue and
 *      check if the handler thread needs to be awakened.
 *
 *      This function is called in INTERRUPT CONTEXT
 */

STATIC void
_bcm_robo_tx_chain_done_cb(int unit, robo_dv_t *dv)
{
    ++_tx_chain_done_intr;
    dv->dv_unit = unit;
    TX_DV_NEXT_SET(dv, NULL);
    if (dv_pend_last) { /* Queue is non-empty */
        TX_DV_NEXT_SET(dv_pend_last, dv);
        dv_pend_last = dv;
    } else { /* Empty queue; init first and last */
        dv_pend_first = dv;
        dv_pend_last = dv;
    }

    sal_sem_give(tx_cb_sem);
}               
                             
/*
 * Function:
 *      _bcm_robo_tx_chain_done
 * Purpose:
 *      Process the completion of a TX DMA chain.
 * Parameters:
 *      unit - SOC unit number.
 *      dv - pointer to completed DMA chain.
 * Returns:
 *      Nothing.
 * Notes:
 */

STATIC void
_bcm_robo_tx_chain_done(int unit, robo_dv_t *dv)
{
    bcm_pkt_cb_f  callback;
    volatile bcm_pkt_t *pkt;
    void *cookie;

    assert(dv != NULL);

    ++_tx_chain_done;
    /*
     * Operation complete; call user's completion callback routine, if any.
     */

    callback = TX_INFO(dv)->chain_done_cb;
    if (callback) {
        pkt = TX_INFO(dv)->pkt[0];
        cookie = TX_INFO(dv)->cookie;
        callback(unit, (bcm_pkt_t *)pkt, cookie);
    }

    _robo_tx_dv_free(unit, dv);
}
 
/*
 * Function:
 *      _bcm_robo_tx_packet_done_cb
 * Purpose:
 *      Packet done interrupt callback
 * Parameters:
 *      unit - SOC unit number.
 *      dv - pointer to completed DMA chain.
 * Returns:
 *      Nothing.
 * Notes:
 *      Just checks to see if the packet's callback is set.  If
 *      it is, the packet is added to the packet pending list
 *      and the tx thread is woken up.
 *
 *      This will only be set up as a call back if some packet in the
 *      chain requires a callback.
 */

STATIC void
_bcm_robo_tx_packet_done_cb(int unit, robo_dv_t *dv, robo_dcb_t *dcb)
{
    bcm_pkt_t *pkt;

    COMPILER_REFERENCE(dcb);
    assert(dv);
    assert(TX_INFO(dv));
    assert(TX_INFO(dv)->pkt_count > TX_INFO(dv)->pkt_done_cnt);

    pkt = (bcm_pkt_t *)TX_INFO_CUR_PKT(dv);
    pkt->_dv = dv;
    pkt->unit = unit;
    pkt->_next = NULL;

    /*
     * If callback present, add to list
     */

    if (pkt->call_back) {
        /* Assumes interrupt context */
        if (pkt_pend_last) { /* Queue is non-empty */
            pkt_pend_last->_next = pkt;
            pkt_pend_last = pkt;
        } else { /* Empty queue; init first and last */
            pkt_pend_first = pkt;
            pkt_pend_last = pkt;
        }

        sal_sem_give(tx_cb_sem);
    }

    TX_INFO_PKT_MARK_DONE(dv);
}               
    
/****************************************************************
 *
 * TX DV info dump routine.
 *
 ****************************************************************/

#if defined(BROADCOM_DEBUG)
/*
 * Function:
 *      bcm_tx_show
 * Purpose:
 *      Display info about tx state
 * Parameters:
 *      unit - mostly ignored
 * Returns:
 *      None
 * Notes:
 */

int
bcm_robo_tx_show(int unit)
{
    soc_cm_print("TX state:  chain_send %d. chain_done %d. "
                 "chain_done_intr %d\n", 
                 _tx_chain_send,
                 _tx_chain_done,
                 _tx_chain_done_intr);
    soc_cm_print("           pkt_pend_first %p. pkt_pend_last %p.\n",
                 (void *)pkt_pend_first,
                 (void *)pkt_pend_last);
    soc_cm_print("           dv_pend_first %p. dv_pend_last %p.\n",
                 (void *)dv_pend_first,
                 (void *)dv_pend_last);

    return BCM_E_NONE;
}               
    
/*
 * Function:
 *      bcm_tx_dv_dump
 * Purpose:
 *      Display info about a DV that is setup for TX.
 * Parameters:
 *      dv - The DV to show info about
 * Returns:
 *      None
 * Notes:
 *      Mainly, dumps the tx_dv_info_t structure; then calls soc_robo_dma_dump_dv
 */

void
bcm_robo_tx_dv_dump(void *dv_p)
{
    tx_dv_info_t *dv_info;
    robo_dv_t *dv = (robo_dv_t *)dv_p;

    dv_info = TX_INFO(dv);
    if (dv_info != NULL) {
        soc_cm_print("TX DV info:\n    DV %p. pkt count %d. done count %d.\n",
                     (void *)dv, dv_info->pkt_count, dv_info->pkt_done_cnt);
        soc_cm_print("    cookie %p. cb %p\n", (void *)dv_info->cookie,
                     (void *)dv_info->chain_done_cb);
    } else {
        soc_cm_print("TX DV info is NULL\n");
    }
    soc_robo_dma_dump_dv(dv->dv_unit, "", dv);
}
 
#endif
