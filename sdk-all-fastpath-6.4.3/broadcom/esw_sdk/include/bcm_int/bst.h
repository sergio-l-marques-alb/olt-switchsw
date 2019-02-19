/*
 * $Id: bst.h,v 1.6 Broadcom SDK $
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
 */

#ifndef _BCM_BST_H
#define _BCM_BST_H

#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/cosq.h>

#define _BCM_MAX_PIPES      4

typedef enum {
    _bcmResourceDevice  = 0,
    _bcmResourceEgrPool,
    _bcmResourceEgrMCastPool,
    _bcmResourceIngPool,
    _bcmResourcePortPool,
    _bcmResourcePriGroupShared,
    _bcmResourcePriGroupHeadroom,
    _bcmResourceUcast,
    _bcmResourceMcast,
    _bcmResourceMaxCount
} _bcm_bst_resource_t;

typedef enum _bcm_bst_cb_ret_e {
    _BCM_BST_RV_OK  = 0,
    _BCM_BST_RV_RETRY,
    _BCM_BST_RV_ERROR
} _bcm_bst_cb_ret_t;

typedef enum _bcm_bst_snapshot_mode_e {
    _BCM_BST_SNAPSHOT_THDO,
    _BCM_BST_SNAPSHOT_THDI,
    _BCM_BST_SNAPSHOT_CFAP,
    _BCM_BST_SNAPSHOT_COUNT
} _bcm_bst_snapshot_mode_t;

/*
 * _bcm_bst_device_index_resolve callback is used if device
 * bst API uses the common implementation. This callback queries
 * the device implementation to resolve the gport, cosq to
 * bst resource, along with the index ranges. also device implementation
 * can return a reason code retry to call tresolve again if the
 * indexes are not contiguous or to map multiple resources.
 */
typedef _bcm_bst_cb_ret_t (*_bcm_bst_device_index_resolve)(int unit,
                                         bcm_gport_t gport, bcm_cos_queue_t cosq,
                                         bcm_bst_stat_id_t bid,
                                         int *pipe, int *from_hw_index, 
                                         int *to_hw_index, int *rcb_data, 
                                         void **user_data, int *bcm_rv);

typedef int (*_bcm_bst_device_index_map)(int unit,
            bcm_bst_stat_id_t bid, int port, int index, bcm_gport_t *gport, bcm_cos_t *cosq);

typedef struct _bcm_bst_device_handlers_s {
    _bcm_bst_device_index_resolve resolve_index;
    _bcm_bst_device_index_map     reverse_resolve_index;
} _bcm_bst_device_handlers_t;

extern int _bcm_bst_attach(int unit, _bcm_bst_device_handlers_t *cbs);

extern int _bcm_bst_detach(int unit);

extern int
_bcm_bst_cmn_profile_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile);

extern int
_bcm_bst_cmn_profile_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile);

extern int _bcm_bst_cmn_control_set(int unit, bcm_switch_control_t type, int arg);

extern int _bcm_bst_cmn_control_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_bst_cmn_stat_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_bst_stat_id_t bid, uint32 options, uint64 *pvalue);

extern int _bcm_bst_cmn_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int 
_bcm_bst_cmn_stat_multi_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                            uint32 options, int max_values,
                            bcm_bst_stat_id_t *id_list, uint64 *pvalues);
extern int 
_bcm_bst_cmn_stat_clear(int unit, bcm_gport_t port, bcm_cos_queue_t cosq, 
                        bcm_bst_stat_id_t bid);

typedef enum _bcm_bst_op_e {
    _bcmBstOpGet = 0,
    _bcmBstOpSet
} _bcm_bst_op_t;

typedef struct _bcm_bst_resource_info_s {
    int         valid;
    uint32      flags;
    soc_pbmp_t  pbmp;
    soc_mem_t   stat_mem[_BCM_MAX_PIPES];
    soc_reg_t   stat_reg[_BCM_MAX_PIPES];
    soc_field_t stat_field;

    int         threshold_adj;
    int         threshold_gran; /* Granularity of cells */
    soc_mem_t   threshold_mem[_BCM_MAX_PIPES];
    soc_mem_t   threshold_reg[_BCM_MAX_PIPES];
    soc_field_t threshold_field;

    soc_mem_t   profile_mem;
    soc_mem_t   profile_reg;
    soc_field_t profile_field;

    soc_profile_reg_t  iprofile_r;
    soc_profile_reg_t  *profile_r;
    soc_profile_mem_t  iprofile_m;
    soc_profile_mem_t  *profile_m;

    int                 index_min;
    int                 index_max;

    uint32              *p_stat;
    uint32              *p_threshold;
    int                 num_instance; /* total instances of this resource */
    int                 num_stat_pp;
} _bcm_bst_resource_info_t;

typedef int (*_bcm_bst_byte_to_cell)(int unit, uint32 byte);

typedef int (*_bcm_bst_cell_to_byte)(int unit, uint32 cell);

typedef int (*_bcm_bst_control_set)(int unit, bcm_switch_control_t type,
                                    int arg);
typedef int (*_bcm_bst_control_get)(int unit, bcm_switch_control_t type,
                                    int *arg);

typedef int (*_bcm_bst_pre_sync_cb)(int unit, bcm_bst_stat_id_t bid,
             int *sync_val);
typedef int (*_bcm_bst_post_sync_cb)(int unit, bcm_bst_stat_id_t bid,
             int sync_val);

typedef int (*_bcm_bst_intr_to_resources_cb)(int unit, uint32 *flags);
typedef int (*_bcm_bst_hw_trigger_set)(int unit, bcm_bst_stat_id_t bid);
typedef int (*_bcm_bst_hw_trigger_reset)(int unit, bcm_bst_stat_id_t bid);
typedef int (*_bcm_bst_hw_intr_cb)(int unit);
typedef int (*_bcm_bst_hw_stat_snapshot)(int unit, bcm_bst_stat_id_t bid,
                                         int port, int index);
typedef int (*_bcm_bst_hw_stat_clear_t)(int unit,
                                        _bcm_bst_resource_info_t *resInfo,
                                        bcm_bst_stat_id_t bid, int port,
                                        int index);

#define _BCM_BST_SYNC_F_ENABLE          0x1
#define _BCM_BST_SYNC_F_THREAD_VALID    0x2

typedef struct _bcm_bst_cmn_unit_info_s {
    uint32                          max_bytes;
    _bcm_bst_byte_to_cell           to_cell;
    _bcm_bst_cell_to_byte           to_byte;
    _bcm_bst_control_set            control_set;
    _bcm_bst_control_get            control_get;
    _bcm_bst_resource_info_t        resource_tbl[bcmBstStatIdMaxCount];
    _bcm_bst_device_handlers_t      handlers;
    _bcm_bst_pre_sync_cb            pre_sync;
    _bcm_bst_post_sync_cb           post_sync;
    _bcm_bst_intr_to_resources_cb   intr_to_resources;
    _bcm_bst_hw_trigger_set         hw_trigger_set;
    _bcm_bst_hw_trigger_reset       hw_trigger_reset;
    _bcm_bst_hw_intr_cb             hw_intr_cb;
    _bcm_bst_hw_stat_snapshot       hw_stat_snapshot;
    _bcm_bst_hw_stat_clear_t        hw_stat_clear;
    sal_sem_t                       bst_trigger;
    int                             sync_flags;
    sal_thread_t                    bst_thread_id;
    int                             interval;
    sal_mutex_t                     bst_reslock;
    int                             mode;
    /*
     * mode = 0, 1 SHARED and 8 HEADROOM
     * mode = 1, 8 SHARED and 1 HEADROOM 
     */
    int                             track_mode;
    int                             snapshot_mode;
} _bcm_bst_cmn_unit_info_t;

extern _bcm_bst_cmn_unit_info_t *_bcm_bst_unit_info[BCM_MAX_NUM_UNITS];

#define _BCM_UNIT_BST_INFO(u) _bcm_bst_unit_info[(u)]

#define _BCM_BST_RESOURCE(u,r) (&((_bcm_bst_unit_info[(u)])->resource_tbl[(r)]))

#define _BCM_BST_RESOURCE_VALID(pr)     (pr)->valid

#define _BCM_BST_STAT_THRESHOLD_INFO_INIT(pe)        \
    do {                                             \
        int i = 0;                                   \
        for (i = 0; i < _BCM_MAX_PIPES; i++) {       \
            (pe)->stat_mem[i] = (INVALIDm);          \
            (pe)->stat_reg[i] = (INVALIDr);          \
            (pe)->threshold_mem[i] = (INVALIDm);     \
            (pe)->threshold_reg[i] = (INVALIDr);     \
        }                                            \
        (pe)->stat_field = (INVALIDf);               \
        (pe)->threshold_field = (INVALIDf);          \
        (pe)->threshold_gran = (1);                  \
    } while(0);

#define _BCM_BST_STAT_INFO(pe,smem,sreg,sf)     \
            (pe)->stat_mem[0] = (smem);         \
            (pe)->stat_reg[0] = (sreg);         \
            (pe)->stat_mem[1] = INVALIDm;       \
            (pe)->stat_reg[1] = INVALIDr;       \
            (pe)->stat_field = (sf);

#define _BCM_BST_STAT_INFO2(pe,smem1,smem2,sreg1,sreg2,sf)     \
            (pe)->stat_mem[0] = (smem1);         \
            (pe)->stat_reg[0] = (sreg1);         \
            (pe)->stat_mem[1] = (smem2);         \
            (pe)->stat_reg[1] = (sreg2);         \
            (pe)->stat_field = (sf);

#define _BCM_BST_STAT_INFO_N(pe,smem,sreg,sf,n) \
            (pe)->stat_mem[n] = (smem);         \
            (pe)->stat_reg[n] = (sreg);         \
            (pe)->stat_field = (sf);

#define _BCM_BST_THRESHOLD_INFO(pe,pmem,preg,pf,gran)       \
            (pe)->threshold_mem[0] = (pmem);                \
            (pe)->threshold_reg[0] = (preg);                \
            (pe)->threshold_mem[1] = INVALIDm;              \
            (pe)->threshold_reg[1] = INVALIDr;              \
            (pe)->threshold_field = (pf);                   \
            (pe)->threshold_gran = (gran);

#define _BCM_BST_THRESHOLD_INFO2(pe,pmem1,pmem2,preg1,preg2,pf,gran) \
            (pe)->threshold_mem[0] = (pmem1);                        \
            (pe)->threshold_reg[0] = (preg1);                        \
            (pe)->threshold_mem[1] = (pmem2);                        \
            (pe)->threshold_reg[1] = (preg2);                        \
            (pe)->threshold_field = (pf);                            \
            (pe)->threshold_gran = (gran);

#define _BCM_BST_THRESHOLD_INFO_N(pe,pmem,preg,pf,gran,n) \
            (pe)->threshold_mem[n] = (pmem);         \
            (pe)->threshold_reg[n] = (preg);         \
            (pe)->threshold_gran = (gran);           \
            (pe)->threshold_field = (pf);

#define _BCM_BST_PROFILE_INFO(pe,pmem,preg,pf)       \
            (pe)->profile_mem = (pmem);              \
            (pe)->profile_reg = (preg);              \
            (pe)->profile_field = (pf);

#define _BCM_BST_CMN_RES_F_PROFILEID    0x01
#define _BCM_BST_CMN_RES_F_PIPED        0x02
/* Following flags for Resource Classification (Dev/Ing/Egr) */
#define _BCM_BST_CMN_RES_F_RES_DEV      0x04
#define _BCM_BST_CMN_RES_F_RES_ING      0x08
#define _BCM_BST_CMN_RES_F_RES_EGR      0x10

/* Following flags for Resource Classification (Dev/Ing/Egr) */
#define _BCM_BST_CMN_RES_F_RES_DEV      0x04
#define _BCM_BST_CMN_RES_F_RES_ING      0x08
#define _BCM_BST_CMN_RES_F_RES_EGR      0x10

#define _BCM_BST_RESOURCE_FLAG_SET(pe,rflags)        \
            (pe)->flags = (rflags);                  \
            (pe)->valid = 1;

#define _BCM_BST_RESOURCE_STAT_INIT(pres, num, rname)                   \
    if ((pres)->p_stat == NULL) {                                       \
            (pres)->p_stat = sal_alloc((num)*sizeof(uint32), (rname));  \
    }                                                                   \
    (pres)->num_stat_pp = (num);

#define _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, num, rname)                  \
    if ((pres)->p_threshold == NULL) {                                      \
            (pres)->p_threshold = sal_alloc((num)*sizeof(uint32), (rname)); \
    } 

#define _BCM_BST_RESOURCE_PBMP_SET(pe,pbm)      (pe)->pbmp = (pbm)

#define _BCM_BST_RESOURCE_MIN       bcmBstStatIdDevice
#define _BCM_BST_RESOURCE_MAX       bcmBstStatIdMaxCount

#define _BCM_BST_RESOURCE_ITER(bi, bid)             \
        for (bid = _BCM_BST_RESOURCE_MIN;           \
             bid < _BCM_BST_RESOURCE_MAX; bid++)

#define _BCM_BST_SYNC_ENABLED(bi)   \
        (((bi)->sync_flags & _BCM_BST_SYNC_F_ENABLE) ? 1 : 0)

#define _BCM_BST_SYNC_THREAD_VALID(bi)   \
        (((bi)->sync_flags & _BCM_BST_SYNC_F_THREAD_VALID) ? 1 : 0)


extern int _bcm_bst_sync_thread_start(int unit);
extern int _bcm_bst_sync_thread_stop(int unit);
extern int _bst_intr_enable_set(int unit, int enable);
extern int _bcm_bst_hw_event_cb(int unit);
extern int _bst_intr_status_reset(int unit);
extern int _bcm_bst_sync_hw_trigger(int unit, bcm_bst_stat_id_t bid,
                                    int port, int index);
extern int _bcm_bst_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                     int port, int index);
extern int _bcm_bst_hw_stat_clear(int unit,
                                  _bcm_bst_resource_info_t *resInfo,
                                  bcm_bst_stat_id_t bid, int port, int index);

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int bcm_bst_th_init(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */

#endif /* _BCM_BST_H */

