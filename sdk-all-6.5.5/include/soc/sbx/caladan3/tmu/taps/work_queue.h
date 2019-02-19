/*
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
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: work_queue.h,v 1.12 Broadcom SDK $
 *
 * TAPS work queue library defines/interfaces
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SBX_CALADN3_TAPS_WORK_QUEUE_H_
#define _SBX_CALADN3_TAPS_WORK_QUEUE_H_

#include <soc/sbx/sbDq.h>

typedef enum taps_work_type_e_s {
    TAPS_TCAM_WORK,
    TAPS_SBUCKET_WORK,
    TAPS_DBUCKET_DATA_WORK,
    TAPS_DBUCKET_WORK,
    TAPS_TCAM_PROPAGATION_WORK,
    TAPS_SBUCKET_PROPAGATION_WORK,
    TAPS_REDISTRIBUTE_STAGE1_WORK,
    TAPS_REDISTRIBUTE_STAGE2_WORK,
    TAPS_REDISTRIBUTE_STAGE3_WORK,
    TAPS_WORK_TYPE_MAX
} taps_work_type_e_t;

#define _TAPS_VALID_WORK_TYPE_(type) \
    (type >= TAPS_TCAM_WORK && type < TAPS_WORK_TYPE_MAX)

typedef struct taps_wgroup_s {
    dq_t  work_group_list_node;
    unsigned int wgroup; /* group id */
    unsigned int force_work_type_enable; /* force work type enable */
    taps_work_type_e_t forced_work_type; /* when force_work_type_enable is TRUE, always use this work type */
    dq_t  work_list[TAPS_WORK_TYPE_MAX]; /* work item list */
    uint8 host_share;
} taps_wgroup_t, *taps_wgroup_handle_t;

#define _TAPS_MAX_WGROUP_ (1) /* increase when batching */
typedef struct taps_wq_s {
    dq_t  work_group_list;
} taps_wq_t, *taps_wq_handle_t;

#define _WQ_DEQUEUE_DEFAULT_ (0) /* from head */
#define _WQ_DEQUEUE_TAIL_ (1)

/*
 *
 * Function:
 *   taps_work_queue_init
 * Purpose:
 *   Init work queue
 * Parameters
 *   (IN)  unit              : unit number of the device
 *   (OUT) p_wq              : return the handle of work queue object
 * Returns
 *   SOC_E_NONE - successfully created a work queue
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_queue_init(int unit, taps_wq_handle_t *p_wq);

/*
 *
 * Function:
 *   taps_work_queue_destroy
 * Purpose:
 *   Destroy work queue
 * Parameters
 *   (IN)  unit            : unit number of the device
 *   (OUT) wq              : handle of work queue object
 * Returns
 *   SOC_E_NONE - successfully created a work queue
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_queue_destroy(int unit, taps_wq_handle_t wq);

/*
 *
 * Function:
 *   taps_work_group_create
 * Purpose:
 *   creates a taps work group
 * Parameters
 *   (IN) unit         : unit number of the device
 *   (IN) work_queue   : handle returned by taps_work_queue_init
 *   (IN) work_group   : work group id
 * Returns
 *   SOC_E_NONE - successfully enqueued work payload object
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_group_create(int unit, 
                                  taps_wq_handle_t work_queue, 
                                  unsigned int wgroup,
                                  taps_wgroup_handle_t *work_group_handle);

/*
 *
 * Function:
 *   taps_work_group_destroy
 * Purpose:
 *   destroy a taps work group
 * Parameters
 *   (IN) unit         : unit number of the device
 *   (IN) work_queue   : handle returned by taps_work_queue_init
 *   (IN) work_group   : work group id
 * Returns
 *   SOC_E_NONE - successfully enqueued work payload object
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_group_destroy(int unit, 
                                   taps_wgroup_handle_t wghdl);

/*
 *
 * Function:
 *   taps_work_enqueue
 * Purpose:
 *   Enqueue work into work queue
 * Parameters
 *   (IN) unit         : unit number of the device
 *   (IN) work_group   : work group handle
 *   (IN) type         : work type to be enqueued
 *   (IN) work_item    : work item
 * Returns
 *   SOC_E_NONE - successfully enqueued work payload object
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_enqueue(int unit, 
                             taps_wgroup_handle_t work_group,
                             taps_work_type_e_t type,
                             dq_p_t work_item);
/*
 *
 * Function:
 *   taps_work_dequeue
 * Purpose:
 *   Dequeue work queue. Return NULL indicate work_queue is empty
 * Parameters
 *   (IN) unit         : unit number of the device
 *   (IN) work_group   : work group handle
 *   (IN) type         : work type to be enqueued
 *   (OUT)work_item    : work item dequeued
 * Returns
 *   SOC_E_NONE - successfully dequeued work payload object
 *   SOC_E_EMPTY- work queue empty
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_dequeue(int unit, 
                             taps_wgroup_handle_t work_group,
                             taps_work_type_e_t type,
                             dq_p_t *work_item,
                             /* used to dequeue last enqueued items comes handy with cleanup */
                             uint8 dq_tail /* 0 - default, 1 - pull out last enqueued item */ );

/*
 *
 * Function:
 *   taps_work_queue_iter_first
 * Returns
 *   SOC_E_NONE - if work available
 *   SOC_E_EMPTY- work queue empty
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_queue_iter_first(int unit, 
                                      taps_wgroup_handle_t work_group,
                                      taps_work_type_e_t type,
                                      dq_p_t *work_item);

/*
 *
 * Function:
 *   taps_work_queue_iter_get_next
 * Returns
 *   SOC_E_NONE - if work available
 *   SOC_E_EMPTY- work queue empty
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_queue_iter_get_next(int unit, 
                                         taps_wgroup_handle_t work_group,
                                         taps_work_type_e_t type,
                                         dq_p_t *work_item);
/*
 *
 * Function:
 *   taps_work_queue_stats
 * Returns
 *   SOC_E_NONE - if work available
 *   SOC_E_EMPTY- work queue empty
 *   SOC_E_* as appropriate otherwise
 */
extern int taps_work_queue_stats(int unit, 
                                 taps_wgroup_handle_t work_group,
                                 taps_work_type_e_t type);

#endif /* _SBX_CALADN3_TAPS_WORK_QUEUE_H_ */
