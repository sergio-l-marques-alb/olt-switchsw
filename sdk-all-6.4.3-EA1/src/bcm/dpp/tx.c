/* 
 * $Id: tx.c,v 1.27 Broadcom SDK $
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
 * File:    tx.c
 * Purpose: Implementation of bcm_petra_tx* API for dune devices
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TX

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm_int/common/tx.h>
#include <bcm/pkt.h>
#include <bcm_int/common/tx.h>

#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>

#include <soc/dpp/headers.h>
#include <bcm_int/petra_dispatch.h>



#if defined(BCM_SBUSDMA_SUPPORT) || defined(BCM_88650_A0)
sbusdma_desc_handle_t _soc_port_tx_handles[FRAGMENT_SIZE][3];
#endif

volatile uint32 _soc_tx_pending[BCM_MAX_NUM_UNITS];

/* Internal structures */
typedef struct _bcm_petra_tx_async_state_s {
    bcm_pkt_t                           *first_pkt; 
    bcm_pkt_t                           *pkt; 
    void                                *cookie;
    bcm_pkt_cb_f                        cb_f;
    struct _bcm_petra_tx_async_state_s  *next;
    sal_time_t                          start_time;
} _bcm_petra_tx_async_state_t;

typedef struct _bcm_petra_tx_state_s {
    uint8                       init_done;
    sal_mutex_t                 tx_lock;
    sal_sem_t                   tx_list_sem;
    _bcm_petra_tx_async_state_t *tx_list_head;
    _bcm_petra_tx_async_state_t *tx_list_tail;
    int                         tx_list_count;
    sal_thread_t                tid;
} _bcm_petra_tx_state_t;

/* Static variables */
static _bcm_petra_tx_state_t _bcm_petra_tx_state[BCM_MAX_NUM_UNITS];

/* Local macros */
#define DPP_TX_LOCK(unit) \
        sal_mutex_take(_bcm_petra_tx_state[unit].tx_lock, sal_mutex_FOREVER)

#define DPP_TX_UNLOCK(unit) \
        sal_mutex_give(_bcm_petra_tx_state[unit].tx_lock)

#define DPP_TX_LIST_WAIT(unit) \
        sal_sem_take(_bcm_petra_tx_state[unit].tx_list_sem, sal_sem_FOREVER)

#define DPP_TX_LIST_SIGNAL(unit) \
        sal_sem_give(_bcm_petra_tx_state[unit].tx_list_sem)

#define DPP_TX_INIT_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(_bcm_petra_tx_state[unit].init_done == 0) { return BCM_E_INIT; }

#define DPP_DEFAULT_TX_THREAD_PRI   (50)
#define DPP_TX_MAX_RETRY            (50)
#define DPP_TX_SYNC_WAIT            (50000)




void _soc_sbusdma_port_tx_cb(int unit, int status, sbusdma_desc_handle_t handle, void *data);
int bcm_arad_tx_cpu_tunnel(bcm_pkt_t *pkt, int dest_unit,  int remote_port,  uint32 flags, bcm_cpu_tunnel_mode_t mode);


int bcm_petra_tx_cpu_tunnel(bcm_pkt_t *pkt,    int dest_unit, int remote_port, uint32 flags, bcm_cpu_tunnel_mode_t mode);

#ifdef BCM_PETRA_SUPPORT
/* Function:    _bcm_petra_tx_list_dequeue
 * Purpose:     Dequeue from a unit specific TX list
 * Parameters:  unit (IN)   - unit number
 *              pp_st (OUT) - async state info
 * Returns:     BCM_E_XXX
 * Note:        Dequeues from head
 */
int _bcm_petra_tx_list_dequeue(int unit, _bcm_petra_tx_async_state_t **pp_st)
{
    _bcm_petra_tx_state_t *tx_state = &_bcm_petra_tx_state[unit];

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pp_st);
    if ((tx_state == NULL) || (tx_state->tx_list_count < 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid TX state.")));
    }
    if (tx_state->tx_list_count == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EMPTY, (_BSL_BCM_MSG("TX list count is empty, unable to Dequeue.")));
    } else if ((tx_state->tx_list_head == NULL) || 
               (tx_state->tx_list_tail == NULL)) {
        /* just an additional check...should not come here */
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "TX thread error. Invalid TX list state\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL); 
    }

    *pp_st = tx_state->tx_list_head;
    tx_state->tx_list_head = tx_state->tx_list_head->next;
    if (tx_state->tx_list_head) {
        if (tx_state->tx_list_head->next == NULL) {
            tx_state->tx_list_tail = tx_state->tx_list_head;
        } else {
            /* tail remains the same */
        }
    } else {
        tx_state->tx_list_tail = NULL;
    }
    tx_state->tx_list_count--;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_list_enqueue
 * Purpose:     Enqueue into unit specific TX list to be TX'ed asynchronously
 * Parameters:  unit (IN)   - unit number
 *              state (IN)  - async state info
 * Returns:     BCM_E_XXX
 * Notes:       Enqueues at tail
 */
int _bcm_petra_tx_list_enqueue(int unit, _bcm_petra_tx_async_state_t *state)
{
    _bcm_petra_tx_state_t       *tx_state = &_bcm_petra_tx_state[unit];
    _bcm_petra_tx_async_state_t *old_head;
    _bcm_petra_tx_async_state_t *old_tail;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(state);
    if ((tx_state == NULL) || (tx_state->tx_list_count < 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid TX state.")));
    }

    old_head = tx_state->tx_list_head;
    old_tail = tx_state->tx_list_tail;

    if (tx_state->tx_list_count == 0) {
        tx_state->tx_list_head = state;
    } else {
        if (tx_state->tx_list_tail == NULL) {
            /* just an additional check...should not come here */
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "TX thread error. Invalid TX list state\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
        }
        tx_state->tx_list_tail->next = state;
    }
    state->next = NULL;
    tx_state->tx_list_tail = state; /* insert at tail */
    tx_state->tx_list_count++;

    if (DPP_TX_LIST_SIGNAL(unit) < 0) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "TX thread signal failed. \n")));
        tx_state->tx_list_head = old_head;
        tx_state->tx_list_tail = old_tail;
        tx_state->tx_list_count--;
        BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_async_tx_ready_or_done
 * Purpose:     Checks if async TX operation is pending
 * Parameters:  unit (IN)   - Device Number
 * Returns:     0 - no pending async tx op
 *              1 - if pending
 */
int
_bcm_petra_async_tx_pending(int unit)
{

    int     pending = 1; /* initialize to pending operation */
    
#if defined(BCM_88640_A0)
    int     rv;
    uint32  regval = 0;
#endif    

    BCMDNX_INIT_FUNC_DEFS;

#if defined(BCM_88640_A0)
    if (SOC_IS_PETRAB(unit))
    {

        rv = READ_REGISTERINTERFACEPACKETCONTROLr(unit, &regval);
        if (!SOC_FAILURE(rv)) {
            pending = soc_reg_field_get(unit, REGISTERINTERFACEPACKETCONTROLr, 
                                        regval, REGIFPKTSENDDATAf);
        }
    } else 
#endif /* BCM_88640_A0 */   
#ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
        
        pending = 0;
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "Arad _bcm_petra_async_tx_pending routine\n")));
        } else
#endif /* BCM_ARAD_SUPPORT */
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not for this device type unit = %d ."), unit)); 
        }

    BCMDNX_IF_ERR_EXIT(pending);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_async_tx_op
 * Purpose:     Send an individual packet on a local unit.
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 * Returns:     BCM_E_XXX
 * Notes:       This function does a synchronous TX operation using the async
 *              tx interface. Meaning, this function blocks till HW indicates
 *              TX operation completed or timesout.
 */
#define REG_IF_PKT_DATA_OFFSET_END      (0x2516)
int
_bcm_petra_async_tx_op(int unit, bcm_pkt_t *pkt)
{
#if defined(BCM_88640_A0) || defined(BCM_88650_A0)
    int hdr_len,  total_len, num_fragments,fragment;
#ifdef BCM_88640_A0    

    int     fragment_len, idx_start, offset_adj;
    int      idx, num_words, bytes_to_write, offset;
    uint32  ctrl_reg, pkt_data;
  

    int rv;
#endif    
    BCMDNX_INIT_FUNC_DEFS;
    LOG_VERBOSE(BSL_LS_BCM_TX,
                (BSL_META_U(unit,
                            "_bcm_petra_async_tx_op routine START\n")));
    if (SOC_IS_PETRAB(unit)) { 
#ifdef BCM_88640_A0
            if ((pkt == NULL) || (pkt->_pkt_data.len <= 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Packet.")));
            }
            if (_bcm_petra_async_tx_pending(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Pending TX.")));
            }

            hdr_len = soc_dpp_hdr_len_get(unit, pkt->_dpp_hdr_type);
            total_len = hdr_len + pkt->_pkt_data.len;
            num_fragments = (total_len + FRAGMENT_SIZE - 1)/FRAGMENT_SIZE;
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META_U(unit,
                                    "Unit:%d sending a packet with"
                                    " %d fragments \n"), unit, num_fragments));

            offset_adj = 0;
            
            for (fragment=0; fragment < num_fragments; fragment++) {
                LOG_VERBOSE(BSL_LS_BCM_TX,
                            (BSL_META_U(unit,
                                        "Unit:%d sending "
                                        "fragment number %d: \n"), unit, fragment));
                /* fill in control reg contents */
                ctrl_reg = 0;
                if (fragment == 0) {
                    /* SOP */
                    soc_reg_field_set(unit, REGISTERINTERFACEPACKETCONTROLr, &ctrl_reg,
                                      REGIFPKTSTARTf, 1);
                }
                if (fragment == (num_fragments -1)) {
                    /* EOP */
                    soc_reg_field_set(unit, REGISTERINTERFACEPACKETCONTROLr, &ctrl_reg,
                                      REGIFPKTENDf, 1);
                    /* value programmed is num bytes - 1 */
                    fragment_len = ((total_len -1) % FRAGMENT_SIZE) + 1; 
                    soc_reg_field_set(unit, REGISTERINTERFACEPACKETCONTROLr, &ctrl_reg,
                                      REGIFPKTBEf, fragment_len - 1);
                } else {
                    fragment_len = FRAGMENT_SIZE;
                }
                /* trigger bit */
                soc_reg_field_set(unit, REGISTERINTERFACEPACKETCONTROLr, &ctrl_reg,
                                  REGIFPKTSENDDATAf, 1);

                /* write packet data and ITMH header into data registers */
                idx_start = 0;
                if ((fragment == 0) && (hdr_len > 0)) {
                    /* first fragment. push itmh base header header */
                    while (idx_start*4 < hdr_len) {
                        sal_memcpy(&pkt_data, &(pkt->_dpp_hdr[idx_start*4]), 4);

                        if ((hdr_len - (idx_start*4)) < 4) {
                            sal_memcpy((((uint8 *)&pkt_data) + (hdr_len - (idx_start*4))), pkt->_pkt_data.data, 4 - (hdr_len - (idx_start*4)));
                        }

                        rv = soc_reg32_write(unit, REG_IF_PKT_DATA_OFFSET_END - idx_start, pkt_data);
                        if (SOC_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_BCM_TX,
                                      (BSL_META_U(unit,
                                                  "Unit:%d Error(%s) writing TX packet data \n"), 
                                                  unit, bcm_errmsg(rv)));
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                
                        ++idx_start;
                    }
                   
                    offset_adj = -19;
                    
                    if (hdr_len == DPP_HDR_ITMH_SP_EXT_LEN) {
                        /* extension header is also present */
                        sal_memcpy(&pkt_data, (pkt->_dpp_hdr + DPP_HDR_ITMH_BASE_LEN),
                                   2); /* first copy 2B ext header */
                        sal_memcpy((((uint8 *)&pkt_data) + 2), pkt->_pkt_data.data, 2);
                        /* then the remaining 2B from packet data*/
                        rv = soc_reg32_write(unit, (REG_IF_PKT_DATA_OFFSET_END - 1), 
                                             pkt_data);
                        if (SOC_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_BCM_TX,
                                      (BSL_META_U(unit,
                                                  "Unit:%d Error(%s) writing TX packet data \n"), 
                                                  unit, bcm_errmsg(rv)));
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        offset_adj = -21;
                        idx_start = idx_start + 2;
                    }
                }
                num_words = (fragment_len + 3)/4;
                for (idx=idx_start; idx < num_words; idx++) {
                    pkt_data = 0;
                    offset = (fragment * FRAGMENT_SIZE) + (idx * 4) + offset_adj;
                    if (idx == (num_words -1)) {
                        /* last word. data may be less than 4 */
                        bytes_to_write = (((fragment_len - 1) % 4) + 1);
                    } else {
                        bytes_to_write = 4;
                    }
                    sal_memcpy(&pkt_data, (pkt->_pkt_data.data + offset), 
                               bytes_to_write);
                    
                    rv = soc_reg32_write(unit, (REG_IF_PKT_DATA_OFFSET_END - idx), 
                                         pkt_data);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_TX,
                                  (BSL_META_U(unit,
                                              "Unit:%d Error(%s) writing TX packet data \n"), 
                                              unit, bcm_errmsg(rv)));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                /* now trigger the TX operation */
                rv = WRITE_REGISTERINTERFACEPACKETCONTROLr(unit, ctrl_reg);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_TX,
                              (BSL_META_U(unit,
                                          "Unit:%d Error(%s) writing TX packet CTRL register \n"), 
                                          unit, bcm_errmsg(rv)));
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* wait for the trigger to be cleared or timeout */
                idx = 0;
                do {
                    idx++;
                    if (_bcm_petra_async_tx_pending(unit)) {
                        LOG_VERBOSE(BSL_LS_BCM_TX,
                                    (BSL_META_U(unit,
                                                ".")));
                        sal_usleep(DPP_TX_SYNC_WAIT);
                    } else {
                        LOG_VERBOSE(BSL_LS_BCM_TX,
                                    (BSL_META_U(unit,
                                                "Done\n")));
                        break;
                    }
                } while(idx < DPP_TX_MAX_RETRY);
                if (idx >= DPP_TX_MAX_RETRY) {
                    LOG_VERBOSE(BSL_LS_BCM_TX,
                                (BSL_META_U(unit,
                                            "TIMEOUT\n")));
                    LOG_ERROR(BSL_LS_BCM_TX,
                              (BSL_META_U(unit,
                                          "Unit:%d TX operation timed out \n"), unit));
                    BCM_RETURN_VAL_EXIT(BCM_E_TIMEOUT);
                }
            }
#endif            
        } else if   (SOC_IS_ARAD(unit)) {    
#ifdef BCM_88650_A0
                int     ret;
                LOG_VERBOSE(BSL_LS_BCM_TX,
                            (BSL_META_U(unit,
                                        "_bcm_petra_async_tx_op arad case enter \n")));
        
                if ((pkt == NULL) || (pkt->_pkt_data.len <= 0)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Packet")));
                }
                if (_bcm_petra_async_tx_pending(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Pending TX.")));
                }
                
                hdr_len = soc_dpp_hdr_len_get(unit, pkt->_dpp_hdr_type);
                total_len = hdr_len + pkt->_pkt_data.len;
                num_fragments = (total_len + FRAGMENT_SIZE - 1)/FRAGMENT_SIZE;
                LOG_VERBOSE(BSL_LS_BCM_TX,
                            (BSL_META_U(unit,
                                        "Arad: Unit:%d sending a packet with"
                                        " %d fragments \n"), unit, num_fragments));
                
        
                 for (fragment=0; fragment < num_fragments; fragment++) {
                        do {
                            ret = soc_sbusdma_desc_run(unit, _soc_port_tx_handles[0][0]);
                            if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 1000);
                            } else {
#ifdef BCM_SBUSDMA_SUPPORT
                                SOC_SBUSDMA_DM_LOCK(unit);
#endif
                                _soc_tx_pending[unit]++;
#ifdef BCM_SBUSDMA_SUPPORT
                                SOC_SBUSDMA_DM_UNLOCK(unit);
#endif
                            }

                        } while(((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) );

                 }                                
#endif    
        }
#endif /*#if defined(BCM_88640_A0) || defined(BCM_88650_A0)*/

    BCM_EXIT;
exit:
BCMDNX_FUNC_RETURN;
}

void _soc_sbusdma_port_tx_cb(int unit, int status, sbusdma_desc_handle_t handle, 
                              void *data)
{

}

/* Function:    _bcm_petra_tx_thread
 * Purpose:     Handle Asynchronous packet tx operations
 * Parameters:  cookie (IN) - unit number
 * Returns:     none
 */
STATIC void
_bcm_petra_tx_thread(void *cookie)
{
    int                         unit = PTR_TO_INT(cookie);
    int                         rv, retry_count;
    _bcm_petra_tx_async_state_t *state;
    char                        thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t	            thread;

    BCMDNX_INIT_FUNC_DEFS;
    while (1) {
        /* wait for list non-empty signal */
        if (DPP_TX_LIST_WAIT(unit) < 0) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "TX thread error waiting on list. Thread Exiting\n")));
            break;
        }

        DPP_TX_LOCK(unit);
        rv = BCM_E_NONE;
        while (BCM_SUCCESS(rv)) {
            /* process the whole list */
            state = NULL;
            rv = _bcm_petra_tx_list_dequeue(unit, &state);
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_EMPTY) {
                    LOG_ERROR(BSL_LS_BCM_TX,
                              (BSL_META_U(unit,
                                          "Unit:%d TX thread error (%s) dequeuing TX "
                                          "request\n"), unit, bcm_errmsg(rv)));
                }
                break;
            }
            if (state == NULL) {
                /* Should not hit...just an additional check */
                LOG_ERROR(BSL_LS_BCM_TX,
                          (BSL_META_U(unit,
                                      "TX thread error. Dequeued invalid TX request\n")));
                rv = BCM_E_INTERNAL;
                break;
            }
            retry_count = 0;
            do {
                if (_bcm_petra_async_tx_pending(unit)) {
                    retry_count++;
                    rv = BCM_E_RESOURCE;
                    sal_usleep(DPP_TX_SYNC_WAIT);
                } else {
                    /* ready to send */
                    if (SOC_IS_PETRAB(unit)) {
                        rv = _bcm_petra_async_tx_op(unit, state->pkt); 
                    }
                    if (rv == BCM_E_RESOURCE) {
                        retry_count++; /* retry only if BCM_E_RESOURCE */
                    } else {
                        break;
                    }
                }
            } while (retry_count < DPP_TX_MAX_RETRY);

            if (BCM_SUCCESS(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_TX,
                            (BSL_META_U(unit,
                                        "TX'ed pkt"
                                        "pkt:%p and cookie=%p \n"), 
                             state->pkt, state->cookie));
                if (state->cb_f) {
                    /* Sent successfully and this is the last packet in the 
                     * list/array. call the registered callback function 
                     */
                    state->cb_f(unit, state->first_pkt, state->cookie);
                }
            } else {
                LOG_ERROR(BSL_LS_BCM_TX,
                          (BSL_META_U(unit,
                                      "Error (%s) TX'ing pkt:%p\n"), bcm_errmsg(rv), 
                                      state->pkt));
            }
           BCM_FREE(state);
        }
        DPP_TX_UNLOCK(unit);
    }

    /* Some error happened. */
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    LOG_ERROR(BSL_LS_BCM_TX,
              (BSL_META_U(unit,
                          "AbnormalThreadExit:%s, unit %d\n"), thread_name, unit));

    BCMDNX_FUNC_RETURN_VOID;
}

/* Function:    bcm_petra_tx_init
 * Purpose:     Initialize internal data structures asynchronous transmit.
 * Parameters:  unit (IN)   - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_init(int unit)
{
    _bcm_petra_tx_state_t       *tx_state;
    _bcm_petra_tx_async_state_t *state;
    int                         rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
       if (SOC_IS_ARAD(unit)) {
         return (bcm_common_tx_init(unit));
     } else 
#endif
     { 
    BCM_DPP_UNIT_CHECK(unit);

    if (!BCM_IS_LOCAL(unit)) {
        /* Do nothing. TX tunnel setup is done using bcm_tx_cpu_tunnel* API */
        BCM_EXIT;
    }

    tx_state = &_bcm_petra_tx_state[unit];

    if (!tx_state->init_done) {
        /* not initialized before */
        tx_state->tx_lock = sal_mutex_create("tx mutex");
        if(tx_state->tx_lock == NULL) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "pkt tx mutex create failed. \n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }

        tx_state->tx_list_sem = sal_sem_create("tx sem", sal_sem_BINARY, 0);
        if(tx_state->tx_list_sem == NULL) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "pkt list semaphore create failed. \n")));
            sal_mutex_destroy(tx_state->tx_lock);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }

        tx_state->tid = sal_thread_create("bcmTx", 
                                          SAL_THREAD_STKSZ,
                                          soc_property_get(unit, 
                                                    spn_BCM_TX_THREAD_PRI,
                                                    DPP_DEFAULT_TX_THREAD_PRI),
                                          _bcm_petra_tx_thread,
                                          INT_TO_PTR(unit));
        if ((tx_state->tid == NULL) || (tx_state->tid == SAL_THREAD_ERROR)) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "Unit:%d BCM TX init error. TX thread create failed\n"),
                                  unit));
            sal_mutex_destroy(tx_state->tx_lock);
            sal_sem_destroy(tx_state->tx_list_sem);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }

        tx_state->tx_list_head = NULL;
        tx_state->tx_list_tail = NULL;
        tx_state->tx_list_count = 0;
        tx_state->init_done = 1;
    } else {
        /* re-init. Flush the TX list. */
        DPP_TX_LOCK(unit);
        while (tx_state->tx_list_count > 0) {
            state = NULL;
            rv = _bcm_petra_tx_list_dequeue(unit, &state);
            if (BCM_FAILURE(rv) || (state == NULL)) {
                break;
            }
            /* Its upto the user to free bcm_pkt_t structures */
           BCM_FREE(state);
        }
        DPP_TX_UNLOCK(unit);
    }

    BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_list_age
 * Purpose:     Age out entries in TX list
 * Parameters:  uint (IN) - unit number
 * Returns:     none
 */
int
_bcm_petra_tx_list_age(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_petra_tx_list_age is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_tunnel
 * Purpose:     Tunnel a packet to be transmited to remote unit
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 *              cookie (IN) - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_tunnel(int unit, bcm_pkt_t *pkt, void *cookie)
{
    bcm_cpu_tunnel_mode_t mode = BCM_CPU_TUNNEL_PACKET; BCMDNX_INIT_FUNC_DEFS;
                                                        /* default */
    if (SOC_IS_PETRAB(unit)) {
        if (pkt->call_back != NULL && cookie != NULL) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "Unit:%d bcm_petra_tx ERROR. Cookie non-NULL on async tunnel "
                                  "call\n"), unit));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
    }

    if (pkt->flags & BCM_TX_BEST_EFFORT) {
        mode = BCM_CPU_TUNNEL_PACKET_BEST_EFFORT;
    } else if (pkt->flags & BCM_TX_RELIABLE) {
        mode = BCM_CPU_TUNNEL_PACKET_RELIABLE;
    }
    if (SOC_IS_PETRAB(unit)) {
#ifdef BCM_RPC_SUPPORT
        BCMDNX_IF_ERR_EXIT(bcm_tx_cpu_tunnel(pkt, unit, 0, BCM_CPU_TUNNEL_F_PBMP, mode));
#endif
    } 
#ifdef BCM_ARAD_SUPPORT

    else if (SOC_IS_ARAD(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_arad_tx_cpu_tunnel(pkt, unit, 0, BCM_CPU_TUNNEL_F_PBMP, mode));
    }
#endif    
exit:
    BCMDNX_FUNC_RETURN;
}




int
bcm_arad_tx_cpu_tunnel(bcm_pkt_t *pkt,
                  int dest_unit,
                  int remote_port,
                  uint32 flags,
                  bcm_cpu_tunnel_mode_t mode)
{
#ifdef BCM_SBUSDMA_SUPPORT       
            soc_sbusdma_desc_ctrl_t ctrl;
            soc_sbusdma_desc_cfg_t cfg;
            int rv = BCM_E_NONE;
            BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
            ctrl.flags = 0;
            ctrl.cfg_count = 1;
            ctrl.buff = pkt->pkt_data; /*    soc->counter_buf64+(port*soc->counter_perport); */
            ctrl.cb = _soc_sbusdma_port_tx_cb;
            cfg.blk = pkt->blk_count ;
            cfg.addr = (uint32)remote_port; /*ing_addr + port*/;
            cfg.width = 1;
            cfg.count = pkt->blk_count;
            cfg.addr_shift = 8;
            rv = (soc_sbusdma_desc_create(dest_unit, &ctrl, &cfg,&_soc_port_tx_handles[0][0]));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TX,
                          (BSL_META("Error(%s) in soc_sbusdma_desc_create\n"), bcm_errmsg(rv)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META("handle ip: %d\n"), _soc_port_tx_handles[0][0]));
            _bcm_petra_async_tx_op(dest_unit, pkt);
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META("_bcm_petra_async_tx_op exit\n")));
#else
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
#endif
    BCM_EXIT; /*INIT;*/
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_local
 * Purpose:     Send packets on a local unit.
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - ptr to tx packet structure
 *              count (IN)      - number of packets to send
 *              all_done_cb (IN)- callback function when tx is done
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_local_array(int unit, bcm_pkt_t **pkt, int count, 
                          bcm_pkt_cb_f all_done_cb, void *cookie)
{
    int                         rv = BCM_E_NONE;
    bcm_pkt_t                   *tx_pkt, *first_pkt;
    int                         retry_count, pkt_idx;
    _bcm_petra_tx_async_state_t *state = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    if(count < 0) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Invalid number of packets to send: %d\n"), count));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    if (!SOC_IS_ARAD(unit)) {
    tx_pkt = *pkt;
    first_pkt = *pkt;
    pkt_idx = 1;

    if (all_done_cb) {
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "TX'ing asynchronously"
                                "Pushing packets to TX list to be sent by TX thread.\n")));
        /* push all individual packets into TX list */
        DPP_TX_LOCK(unit);
        while (tx_pkt) {
            BCMDNX_ALLOC(state, sizeof(_bcm_petra_tx_async_state_t), "Async state");
            if (state == NULL) {
                rv = BCM_E_MEMORY;
                break;
            }
            sal_memset(state, 0, sizeof(_bcm_petra_tx_async_state_t));
            state->pkt = tx_pkt;
            state->start_time = sal_time_usecs();
            rv = _bcm_petra_tx_list_enqueue(unit, state);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TX,
                          (BSL_META_U(unit,
                                      "Insert to TX list Failed (%s)\n"), bcm_errmsg(rv)));
               BCM_FREE(state);
                break;
            }
            /* get next packet */
            if (count == 0) {
                /* count == 0 means list */
                tx_pkt = tx_pkt->next;
            } else {
                /* count non-zero means array */
                if (pkt_idx == count) {
                    tx_pkt = NULL; /* no more packets */
                } else {
                    tx_pkt = *(pkt + pkt_idx);
                    pkt_idx++;
                }
            }
            if (tx_pkt == NULL) {
                /* This is the last packet in the list/array. Store the info
                 * for callback to be made 
                 */
                state->cb_f = all_done_cb;
                state->cookie = cookie;
                state->first_pkt = first_pkt;
            }
        }
        DPP_TX_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "TX failed (%s)\n"), bcm_errmsg(rv)));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META_U(unit,
                                    "Successfully pushed"
                                    "all packets into TX list to be handled by TX Thread\n")));
        }
    } else {
        /* Send synchronously */
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "Unit:%d TX'ing synchronously \n"), unit));
        DPP_TX_LOCK(unit);
        while (tx_pkt) {
            retry_count = 0;
            do {
                if (_bcm_petra_async_tx_pending(unit)) {
                    retry_count++;
                    rv = BCM_E_RESOURCE;
                    sal_usleep(DPP_TX_SYNC_WAIT);
                } else {
                    /* ready to send */
                    rv = _bcm_petra_async_tx_op(unit, tx_pkt);
                    if (rv == BCM_E_RESOURCE) {
                        retry_count++; /* retry only if BCM_E_RESOURCE */
                    } else {
                        break;
                    }
                }
            } while (retry_count < DPP_TX_MAX_RETRY);
    
            /* get next packet */
            if (count == 0) {
                /* count == 0 means list */
                tx_pkt = tx_pkt->next;
            } else {
                /* count non-zero means array */
                if (pkt_idx == count) {
                    tx_pkt = NULL; /* no more packets */
                } else {
                    tx_pkt = *(pkt + pkt_idx);
                    pkt_idx++;
                }
            }
        }
        DPP_TX_UNLOCK(unit);
    
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "Unit:%d TX failed (%s)\n"), unit, bcm_errmsg(rv)));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META_U(unit,
                                    "Unit:%d TX successful \n"), unit));
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
    } else { /* for Arad */
        BCMDNX_IF_ERR_EXIT(bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie));
    } 
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* Function:    bcm_petra_tx
 * Purpose:     Transmit a packet
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 *              cookie (IN) - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int bcm_arad_tx(int unit, bcm_pkt_t *pkt, void *cookie);

int
bcm_petra_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    

    if (pkt == NULL) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Invalid packet structure (NULL)\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

            /*return _bcm_petra_tx_tunnel(unit, pkt, cookie); */

    if (!SOC_IS_ARAD(unit)) {
        DPP_TX_INIT_CHECK(unit);
    rv = bcm_petra_tx_pkt_setup(unit,pkt);
    if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "TX failed in pkt_setup(%s)\n"), bcm_errmsg(rv)));
    }
    }
#ifdef BCM_ARAD_SUPPORT
    if ((!BCM_IS_LOCAL(unit)) || (SOC_IS_ARAD(unit)))  {
        /*return _bcm_petra_tx_tunnel(unit, pkt, cookie); */
        BCMDNX_IF_ERR_EXIT(bcm_common_tx(unit, pkt, cookie)); 
    } else 
#endif    
    {

    BCMDNX_IF_ERR_EXIT(_bcm_petra_tx_local_array(unit, &pkt, 1, pkt->call_back, cookie));
    }
exit:
    BCMDNX_FUNC_RETURN;
}




/* Function:    bcm_petra_tx_array
 * Purpose:     Transmit an array of packets
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - array of tx packet structures
 *              count (IN)      - number of packets to be sent
 *              all_done_cb (IN)- callback when all packets are sent
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_array(int unit, bcm_pkt_t **pkt, int count, 
                   bcm_pkt_cb_f all_done_cb, void *cookie)
{
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_NULL_CHECK(pkt);
    if(count <= 0){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("count must be positive")));
    }
    if(!BCM_IS_LOCAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unit must be local")));
    }
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit))  {
        BCMDNX_IF_ERR_EXIT( bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie));
    } else 
#endif
    {
        DPP_TX_INIT_CHECK(unit);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_tx_local_array(unit, pkt, count, all_done_cb, cookie));
    }
exit:
                                                                    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_tx_list
 * Purpose:     Transmit a list of packets
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - list of tx packet structures
 *              all_done_cb (IN)- callback when all packets are sent
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, 
                  void *cookie)
{
    bcm_pkt_t *pkt_ptr[1];
    BCMDNX_INIT_FUNC_DEFS;
    
    *pkt_ptr = pkt;

    BCMDNX_NULL_CHECK(pkt);
    if(!BCM_IS_LOCAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unit must be local")));
    }
#ifdef BCM_ARAD_SUPPORT

    if (SOC_IS_ARAD(unit))  {
        BCMDNX_IF_ERR_EXIT( bcm_common_tx_list(unit, pkt, all_done_cb, cookie));
    } else 
#endif
    {

        DPP_TX_INIT_CHECK(unit);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_tx_local_array(unit, pkt_ptr, 0, all_done_cb, cookie));
    }
exit:
                                                                 BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_pkt_header_setup
 * Purpose:     Configure header for the packet depending on CPU port config
 * Parameters:  unit (IN)   - Device Number
 *              pkt (IN)    - TX packet structure
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_pkt_header_setup(int unit, bcm_pkt_t *pkt)
{
    soc_dpp_itmh_t  itmh;
    uint32          prio_val, dp, fwd_type;

    BCMDNX_INIT_FUNC_DEFS;
    sal_memset(&itmh, 0, sizeof(soc_dpp_itmh_t));

    /* The following fields are not configurable using bcm_pkt_t fields. 
     * If they need to be configured, a header needs to be built directly
     * using soc_dpp_hdr_* functions and _dpp_hdr and _dpp_hdr_type should
     * be set appropriately
     * 
     *  version
     *  pph_present
     *  out_mirr_dis
     *  in_mirr_dis
     *  snoop_cmd
     *  exclude_src 
     */
    prio_val = (pkt->flags & BCM_TX_PRIO_INT) ? pkt->prio_int : pkt->cos;
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_traffic_class, prio_val);

    switch (pkt->color) {
    case bcmColorGreen:
        dp = 0;
        break;
    case bcmColorYellow:
        dp = 1;
        break;
    case bcmColorRed:
        dp = 2;
        break;
    case bcmColorBlack:
        dp =3;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Unit:%d invalid color (%d) specified. Could not map to a "
                              "valid drop-precedence\n"), unit, pkt->color));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_dp, dp);

    if (pkt->flow_id > 0) {
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_UNICAST_FLOW;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_flow_id, pkt->flow_id);
    } else if (pkt->multicast_group > 0) {
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_SYSTEM_MULTICAST;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_multicast_id, 
                               pkt->multicast_group);
    } else {
        /* default to UC direct */
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_UNICAST_DIRECT;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_dest_sys_port, 
                               pkt->src_gport);
    }
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_fwd_type, fwd_type);
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_pph_present, 1);
    sal_memcpy(&pkt->_dpp_hdr, &itmh.base.raw.bytes[0], 
               DPP_HDR_ITMH_BASE_LEN);

    {
        
        uint8 pph[15] =
            {0x00,0x00,0x80,0x00,0x10,
             0x40,0x00,0x00,0x00,0x00,
             0x00,0x00,0x00,0x00,0x63};

#if 0
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "PPH: 0x%02x-%02x-%02x-%02x-%02x \n"),
                     pph[0], pph[1], pph[2], pph[3], pph[4]));
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "       %02x-%02x-%02x-%02x-%02x \n"),
                     pph[5], pph[6], pph[7], pph[8], pph[9]));
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "       %02x-%02x-%02x-%02x-%02x \n"),
                     pph[10], pph[11], pph[12], pph[13], pph[14]));
#endif

        sal_memcpy(&pkt->_dpp_hdr[DPP_HDR_ITMH_BASE_LEN], &pph[0], 15);
    }

    pkt->_dpp_hdr_type = DPP_HDR_itmh_base;
    
    BCM_EXIT;
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_tx_pkt_setup
 * Purpose:     Configure the tx packet structure for TX
 * Parameters:  unit (IN)   - Device Number
 *              pkt (IN)    - TX packet structure
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_pkt_setup(int unit, bcm_pkt_t *pkt)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
  

    BCMDNX_NULL_CHECK(pkt);
    if (!BCM_IS_LOCAL(unit)) {
        /* nothing to do for non-local units */
        BCM_EXIT;
    }
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit))  {
        BCMDNX_IF_ERR_EXIT( bcm_common_tx_pkt_setup(unit,pkt));
    } else 
#endif
    {
    DPP_TX_INIT_CHECK(unit);
    if (pkt->_dpp_hdr_type == DPP_HDR_none) {
        /* packet header not already configured */
        rv = _bcm_petra_tx_pkt_header_setup(unit, pkt);
    }

    BCMDNX_IF_ERR_EXIT(rv);
     }
exit:
    BCMDNX_FUNC_RETURN;
}

#if defined(BROADCOM_DEBUG)
/* Function:    bcm_petra_tx_show
 * Purpose:     Display info about tx state
 * Parameters:  unit (IN)   - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_show(int unit)
{

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit))  {
        BCMDNX_IF_ERR_EXIT( bcm_common_tx_show(unit));
    } else 
#endif    
    {
    DPP_TX_INIT_CHECK(unit);

    /* Nothing to display for now. */
    
    BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BROADCOM_DEBUG */

#endif /* BCM_PETRA_SUPPORT */

