/*
 * $Id: wb_db_mirror.h,v 1.1 Broadcom SDK $
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
 * File:        wb_db_mirror.h
 * Purpose:     WarmBoot - Level 2 support (MIRROR Module)
 */

#ifndef _BCM_DPP_WB_DB_MIRROR_H_
#define _BCM_DPP_WB_DB_MIRROR_H_


/*
 * WarmBoot persistence storage  data structures. To be generic data structure declaration
 * is seperate from s/w data structures. This provides for the following generic case
 *   - Not all data fields of s/w data structures need to be stored in peristence storage
 *   - Persistence storage (for optimization) could pack the variuos fields.
 *     NOTE: Currently no packing of instance is done in a byte. This is because the
 *           the storage footprint is not large. If requirements change this optimization
 *           can be done.
 * Warmboot framework also maintains additional memory footprint. s/w footprint and warmboot
 * footprint are different for the reasons outlined above. Ideally warmbot framework should
 * not have such a large memory footprint. This optimization should be considered if memory
 * footprint needs to be reduced.
 *  
 * In general persistance data structures declaration is common across different generations.
 * This general approach is taken where ever the foot print impact is not large on devices
 * that have less scale/instances.
 *
 * h/w and tools dependency.
 * Persistence storage is accessed by only one system. Thus endian dependencies are not there.
 * s/w data structures are declared without the "pack' attribute. Warmboot persistence
 * data structures are declared with 'pack" attribute to reduce footprint. This requires
 * that loading and storing data to those locations be done accounting different h/w (CPUs).
 * Appropriate macros are declared in this file to handle loading and storing data (uint16/uint32)
 * at non-native boundaries.
 * Tools can change across releases and thus structures will not be declared with bitwise
 * definitions.
 *
 * Exception to endian dependencies
 * It is possible that for debugging purposes the persistance
 * storage my be loaded on an evaluation system / simulator. That will bring in endian
 * dependiencies. This will require the persistence storage format to indicate the endianness
 * of data stored and compare it with current system endianness. Alternative is to have macros
 * (compile time options). This debug feature is a very low priority and not currently implemented.
 *
 * Currently there is a single handle for MIRROR module.
 * In future (to handle multiple versions) if required seperate handle for each data
 * structure can be allocated. This is facilated by "_sequence_" field of
 * "SOC_SCACHE_HANDLE_SET()" macro.
 */


#include <bcm_int/dpp/mirror.h>

#if defined(BCM_WARM_BOOT_SUPPORT)

#define BCM_DPP_WB_MIRROR_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_MIRROR_CURRENT_VERSION        BCM_DPP_WB_MIRROR_VERSION_1_0


#define BCM_DPP_WB_MIRROR_INFO(unit)             (_dpp_wb_mirror_info_p[unit])
#define BCM_DPP_WB_MIRROR_INFO_INIT(unit)        ( (_dpp_wb_mirror_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_mirror_info_p[unit]->init_done == TRUE) )

/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */

#define BCM_DPP_WB_DEV_MIRROR_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_MIRROR_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_MIRROR_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_MIRROR_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_MIRROR_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_MIRROR_INFO(unit)->is_dirty == 1)


/*
 * runtime information
 */
typedef struct bcm_dpp_wb_mirror_info_s {
    int            init_done;
    int            is_dirty;

    uint16         version;
    uint8         *scache_ptr;
    int            size;

    int            refCount_off;
#ifdef BCM_PETRAB_SUPPORT
    int            mirror_off;
#endif
} bcm_dpp_wb_mirror_info_t;


extern bcm_dpp_wb_mirror_info_t   *_dpp_wb_mirror_info_p[BCM_MAX_NUM_UNITS];

/*
 * Functions
 */
extern int
_bcm_dpp_wb_mirror_state_init(int unit);

extern int
_bcm_dpp_wb_mirror_state_deinit(int unit);

extern int
_bcm_dpp_wb_mirror_sync(int unit);

extern int
_bcm_dpp_wb_mirror_update_refCount_state(int unit, int index);

extern int
_bcm_dpp_wb_mirror_update_mirror_mode(int unit);

extern int
_bcm_dpp_wb_mirror_update_port_profile(int unit, int port);


#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_MIRROR_H_ */
