/*
 * $Id: oam.h,v 1.0 Broadcom SDK $
 *
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
 */

#ifndef __BCM_INT_FP_OAM_H__
#define __BCM_INT_FP_OAM_H__

#include "soc/shared/ccm.h"
#include "soc/shared/ccm_msg.h"

/* Direction + isLocal + Group ID + Endpoint Name + Level + Gport + 2VIDs */
#define _FP_OAM_HASH_KEY_SIZE ( sizeof(uint8)                   +   \
                                sizeof(uint8)                   +   \
                                sizeof(bcm_oam_group_t)         +   \
                                sizeof(uint16)                  +   \
                                sizeof(int)                     +   \
                                sizeof(bcm_gport_t)             +   \
                                sizeof(bcm_vlan_t)              +   \
                                sizeof(bcm_vlan_t) )

typedef uint8 _bcm_fp_oam_hash_key_t[_FP_OAM_HASH_KEY_SIZE];



/*
 * Typedef:
 *     _bcm_fp_oam_group_data_t
 * Purpose:
 *     Group information.
 */
typedef struct _bcm_fp_oam_group_data_s
{
    int                 in_use; /* Group status */
    ccm_group_data_t    group_data; /* Group data stores on UK */
} _bcm_fp_oam_group_data_t;


/*
 * Typedef:
 *     _bcm_fp_oam_hash_data_t
 * Purpose:
 *     Endpoint hash table data structure.
 */
typedef struct _bcm_fp_oam_mep_data_s
{
    int in_use;                         /* Endpoint status */
    ccm_mep_data_t        mep_data;/* CCM Data Stored on UK */
} _bcm_fp_oam_mep_data_t;


/*
 * Typedef:
 *     _bcm_fp_oam_control_t
 * Purpose:
 *     OAM module control structure. One structure for each XGS device.
 */
typedef struct _bcm_fp_oam_control_s {

    int                         init;           /* TRUE if OAM module has */
                                                /* been initialized */
    int                         unit;           /* oc unit number */
    sal_mutex_t                 oc_lock;        /* Protection mutex */

    uint32                      group_count;    /* Total groups count */
    uint32                      mep_count;      /* Total mep count */

    /* Group Data */
    shr_idxres_list_handle_t    group_pool;     /* Group indices pool */
    _bcm_fp_oam_group_data_t    *group_data;    /* Group Data */

    /* LMEP Data */
    shr_idxres_list_handle_t    mep_pool;       /* LMEP indices pool */
    _bcm_fp_oam_mep_data_t      *mep_data;      /* LMEP Data */
    shr_htb_hash_table_t        mep_htbl;       /* LMEP+RMEP hash table */

    /* CCM Event handling data elements */
    uint32                      event_mask;     /* Event mask */
    _bcm_oam_event_handler_t    *event_handler_list_p; /* Event handlers LL */
    int                         event_handler_cnt[bcmOAMEventCount];
    sal_thread_t                event_thread_id; /* Event thread Prio */

    /* CCM UC Messaging data elements */
    int                         ccm_uc_num;     /* UC Num for CCM App */
    uint8                       *dma_buffer;    /* DMA Buffer for F/w Msging */
    uint32                      dma_buffer_len; /* Max DMA Msg size */
    int                         ccm_ukernel_ready; /* UKERNEL ready or not */

} _bcm_fp_oam_control_t;


#endif /* !__BCM_INT_FP_OAM_H__ */
