/*
 * $Id: ltm.c,v 1.4 Broadcom SDK $
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
 *  FE2000 loss threshold managment 
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/sbx/g2p3/g2p3.h>

#include <bcm_int/sbx/fe2000/oam/ltm.h>

#include <shared/idxres_fl.h>

#include <bcm/error.h>


#define G2P3_NUM_LT_TABLES                     8
#define G2P3_OAM_MULTIPLIER_SINGLE_TABLE_SIZE  1024

typedef struct {
    int refCount[G2P3_NUM_LT_TABLES];
    int threshold[G2P3_NUM_LT_TABLES];

} ltm_state_t;

STATIC ltm_state_t _ltm_state[SOC_MAX_NUM_DEVICES];
#define STATE(u) _ltm_state[u]


/*
 *   Function
 *      _bcm_ltm_init
 *   Purpose
 *      Initialize the loss threshold table manager
 *   Parameters
 *       unit           = BCM device number
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
_bcm_ltm_init(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    sal_memset(&STATE(unit), 0, sizeof(ltm_state_t));
    return BCM_E_NONE;
}


/*
 *   Function
 *      _bcm_ltm_cleanup
 *   Purpose
 *      Cleanup resources associated with the loss threshold table manager
 *   Parameters
 *       unit           = BCM device number
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int _bcm_ltm_cleanup(int unit)
{
    return BCM_E_NONE;
}



/*
 *   Function
 *      _bcm_ltm_threshold_alloc
 *   Purpose
 *      Allocate  loss threshold table entry for use by loss managment oam
 *   Parameters
 *       unit           = BCM device number
 *       loss_threshold = loss threshold to allocate in 100ths of a percent
 *       id             = loss threshold table index allocated
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
_bcm_ltm_threshold_alloc(int unit, int loss_threshold, int *id)
{
    int idx, foundId = G2P3_NUM_LT_TABLES + 1;
    int rv;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* See if this threshold already exits */
    for (idx=0; idx < G2P3_NUM_LT_TABLES; idx++) {

        if (STATE(unit).threshold[idx] == loss_threshold) {
            foundId = idx;
            break;
        }

        /* record the first empty slot, in case no match for reuse */
        if ((foundId > G2P3_NUM_LT_TABLES) && 
            (STATE(unit).refCount[idx] == 0)) {
            foundId = idx;
        }
    }

    /* No match, and no empty slots */
    if (foundId >= G2P3_NUM_LT_TABLES) {
        return BCM_E_RESOURCE;
    }
    
    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Found loss threshold table idx %d refCount=%d\n"),
                 foundId, STATE(unit).refCount[foundId]));
    
    /* first time, initialize the hw table */
    if (STATE(unit).refCount[foundId] == 0) {
        soc_sbx_g2p3_oammultiplier_t ent;

        /* The oam multiplier table is indexed by the Frame Loss count,
         * and the value at each index is the minimum frames *sent* count 
         * required to remain below the loss threshold.  
         * For example: 
         *   For each index N = FrameLossCount, 
         *       table[N] = minTxCount = FrameLossCount / LossThreshold;
         */
        for (idx=0; idx < G2P3_OAM_MULTIPLIER_SINGLE_TABLE_SIZE; idx++) {
#ifndef __KERNEL__
            /* Loss Threshold (%) = (#loss / #sent) * 100 
             * #expected number of packet sent =  (#loss / loss threshold ) * 100
             *
             * SDK API inputs loss threshold as multiple of 100
             * loss_threshold is expressed in 100ths of a percent 
             * 
             * So #expected number of packet sent = (#loss/loss threshold) * 1e4
             */
            ent.frameCount = (uint32)(((idx + 1.0)/loss_threshold) * 1e4);
#endif
            rv = soc_sbx_g2p3_oammultiplier_set(unit, idx, foundId, &ent);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to write oammultipler"
                                      "[0x%x, 0x%x, 0x%x]:  %d %s\n"),
                           idx, foundId, ent.frameCount, rv, bcm_errmsg(rv)));
                return rv;
            }

            if ((idx & 0x7f) == 0) {
                LOG_DEBUG(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "oammultipler[0x%x, 0x%x] = 0x%x\n"),
                           idx, foundId, ent.frameCount));
            }
            
        }

        STATE(unit).threshold[foundId] = loss_threshold;
    }

    STATE(unit).refCount[foundId]++;
    *id = foundId;

    return BCM_E_NONE;
}



/*
 *   Function
 *      _bcm_ltm_free
 *   Purpose
 *      Return a loss threshold table index to the available pool.
 *   Parameters
 *       unit           = BCM device number
 *       id             = id to free
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
_bcm_ltm_threshold_free(int unit, int id)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (STATE(unit).refCount[id] == 0) {
        return BCM_E_BADID;
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Freeing loss threshold table idx %d refCount=%d\n"),
                 id, STATE(unit).refCount[id]));
    
    STATE(unit).refCount[id]--;

    return BCM_E_NONE;
}
