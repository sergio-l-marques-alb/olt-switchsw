
/*
 * $Id: $
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
 * this module is used to store and acquire all of the device lock's, semaphores, mutexes etc. 
 */
#ifndef _SW_STATE_SYNC_DB_H
#define _SW_STATE_SYNC_DB_H

#include <sal/core/sync.h>
#include <bcm/types.h>

typedef struct sw_state_dpp_sync_db_counter_s {
    sal_thread_t    background;    /* unit background thread */
    sal_sem_t       bgSem;         /* unit background waiting sem */
    sal_mutex_t     cacheLock;     /* cache access locking */
} sw_state_dpp_sync_db_counter_t;

typedef struct sw_state_dpp_sync_db_s{

    /* 
     * bcm locks
     */
    sal_mutex_t stg_lock;
    sal_mutex_t policer_lock;
    sal_mutex_t mirror_lock;
    sal_mutex_t l3_lock;
    sal_mutex_t l2_lock;
    sal_mutex_t trunk_lock;
    sw_state_dpp_sync_db_counter_t counter;
    sal_mutex_t unitLock;  /* unit lock */ /* Moved from bcm_dpp_field_info_OLD_t */
    /* 
     * soc locks
     */
    /* add dpp's soc mutexes and semaphores here */

    /* 
     * shared locks
     */
    /* add shared mutexes and semaphores here */
    sal_mutex_t *htb_locks; 

} sw_state_dpp_sync_db_t;

typedef struct sw_state_sync_db_s {
    sw_state_dpp_sync_db_t  dpp;
} sw_state_sync_db_t;

extern sw_state_sync_db_t sw_state_sync_db[BCM_MAX_NUM_UNITS];

#endif /*_SW_STATE_SYNC_DB_H*/

