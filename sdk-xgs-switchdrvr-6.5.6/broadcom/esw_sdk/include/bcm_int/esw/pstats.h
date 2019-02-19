/*
 * $Id: pstats.h,v 1.0 Broadcom SDK $
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

#ifndef _BCM_PSTATS_H
#define _BCM_PSTATS_H

#if defined(INCLUDE_PSTATS)
#include <soc/defs.h>
#include <soc/types.h>

typedef int (*_bcm_pstats_session_create_f)(int, int, int,
                                            bcm_pstats_session_element_t*,
                                            bcm_pstats_session_id_t*);
typedef int (*_bcm_pstats_session_destroy_f)(int, bcm_pstats_session_id_t);
typedef int (*_bcm_pstats_session_get_f)(int, bcm_pstats_session_id_t,
                                         int,
                                         bcm_pstats_session_element_t*,
                                         int*);
typedef int (*_bcm_pstats_data_sync_f)(int);
typedef int (*_bcm_pstats_session_data_get_f)(int, bcm_pstats_session_id_t,
                                              int,
                                              bcm_pstats_timestamp_t*,
                                              int,
                                              bcm_pstats_data_t*, int*);
typedef int (*_bcm_pstats_session_data_clear_f)(int, bcm_pstats_session_id_t);
typedef int (*_bcm_pstats_session_traverse_f)(int, bcm_pstats_session_traverse_cb,
                                              void*);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
typedef void (*_bcm_pstats_sw_dump_f)(int);
#endif

typedef struct _bcm_pstats_unit_driver_s {
    sal_mutex_t                             pstats_lock;
    _bcm_pstats_session_create_f            session_create;
    _bcm_pstats_session_destroy_f           session_destroy;
    _bcm_pstats_session_get_f               session_get;
    _bcm_pstats_data_sync_f                 data_sync;
    _bcm_pstats_session_data_get_f          session_data_get;
    _bcm_pstats_session_data_clear_f        session_data_clear;
    _bcm_pstats_session_traverse_f          session_traverse;

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    _bcm_pstats_sw_dump_f                   pstats_sw_dump;
#endif
} _bcm_pstats_unit_driver_t;

extern _bcm_pstats_unit_driver_t *_bcm_pstats_unit_driver[BCM_MAX_NUM_UNITS];
#define _BCM_PSTATS_UNIT_DRIVER(u) _bcm_pstats_unit_driver[(u)]

#define PSTATS_LOCK(u)\
        do {\
            if (_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock){\
                sal_mutex_take(_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock,\
                               sal_mutex_FOREVER);\
            }\
        }while(0)
#define PSTATS_UNLOCK(u)\
        do {\
            if (_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock){\
                sal_mutex_give(_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock);\
            }\
        }while(0)

extern int _bcm_pstats_init(int unit);
extern void _bcm_pstats_deinit(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_pstats_sw_dump(int unit);
#endif

#endif /* INCLUDE_PSTATS */
#endif /* _BCM_PSTATS_H */
