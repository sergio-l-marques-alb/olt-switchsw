/*
 * $Id: wb_db_trill.h,v 1.1.2.3 Broadcom SDK $
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
 * File:        wb_db_trill.h
 * Purpose:     WarmBoot - Level 2 support (TRILL Module)
 */

#ifndef _BCM_DPP_WB_DB_TRILL_H_
#define _BCM_DPP_WB_DB_TRILL_H_


/*
 * WarmBoot persistence storage  data structures. To be generic data structure declaration
 * is separate from s/w data structures. This provides for the following generic case
 *   - Not all data fields of s/w data structures need to be stored in persistence storage
 *   - Persistent storage (for optimization) could pack the various fields.
 *     NOTE: Currently no packing of instance is done in a byte. This is because the
 *           the storage footprint is not large. If requirements change this optimization
 *           can be done.
 * Warmboot framework also maintains additional memory footprint. s/w footprint and warmboot
 * footprint are different for the reasons outlined above. Ideally warmboot framework should
 * not have such a large memory footprint. This optimization should be considered if memory
 * footprint needs to be reduced.
 *  
 * In general persistance data structures declaration is common across different generations.
 * This general approach is taken where ever the foot print impact is not large on devices
 * that have less scale/instances.
 *
 * h/w and tools dependency.
 * Persistence storage is accessed by only one system. Thus endian dependencies are not there.
 * s/w data structures are declared without the "pack' attribute. Warmboot persistent
 * data structures are declared with 'pack" attribute to reduce footprint. This requires
 * that loading and storing data to those locations be done accounting different h/w (CPUs).
 * Appropriate macros are declared in this file to handle loading and storing data (uint16/uint32)
 * at non-native boundaries.
 * Tools can change across releases and thus structures will not be declared with bitwise
 * definitions.
 *
 * Exception to endian dependencies
 * It is possible that for debugging purposes the persistance
 * storage may be loaded on an evaluation system / simulator. That will bring in endian
 * dependiencies. This will require the persistence storage format to indicate the endianness
 * of data stored and compare it with current system endianness. Alternative is to have macros
 * (compile time options). This debug feature is a very low priority and not currently implemented.
 *
 * Currently there is a single handle for TRILL module.
 * In future (to handle multiple versions) if required seperate handle for each data
 * structure can be allocated. This is facilated by "_sequence_" field of
 * "SOC_SCACHE_HANDLE_SET()" macro.
 */


#include <bcm_int/dpp/trill.h>

#if defined(BCM_WARM_BOOT_SUPPORT)

#define BCM_DPP_WB_TRILL_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_TRILL_CURRENT_VERSION        BCM_DPP_WB_TRILL_VERSION_1_0


#define BCM_DPP_WB_TRILL_INFO(unit)             (_dpp_wb_trill_info_p[unit])
#define BCM_DPP_WB_TRILL_INFO_INIT(unit)        ( (_dpp_wb_trill_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_trill_info_p[unit]->init_done == TRUE) )

/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */

#define BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_TRILL_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_TRILL_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_TRILL_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_TRILL_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_TRILL_INFO(unit)->is_dirty == 1)


/*
 * runtime information
 */
typedef struct bcm_dpp_wb_trill_info_s {
    int                          init_done;
    int                          is_dirty;
    uint16                       version;
    uint8                        *scache_ptr;
    int                           size;
    /* data structures */
    uint32                        mask_set_off;
    uint32                        trill_out_ac_off;
    uint32                        last_used_id_off;
    uint32                        trill_ports_off;
} bcm_dpp_wb_trill_info_t;


typedef struct bcm_dpp_wb_trill_mask_set_s {
    int mask_set;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_trill_mask_set_t;

typedef struct bcm_dpp_wb_trill_out_ac_s {
    uint32 out_ac;
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_trill_out_ac_t;

typedef struct bcm_dpp_wb_trill_last_used_id_s {
    int last_used_id;
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_trill_last_used_id_t;



#define BCM_DPP_WB_TRILL_MAX_PORTS 200

typedef struct bcm_dpp_wb_trill_ports_s {
    int port_cnt;
    bcm_gport_t ports[BCM_DPP_WB_TRILL_MAX_PORTS];
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_trill_ports_t;

extern bcm_dpp_wb_trill_info_t   *_dpp_wb_trill_info_p[BCM_MAX_NUM_UNITS] ;

/*
 * Functions
 */
extern int
_bcm_dpp_wb_trill_state_init(int unit);

extern int
_bcm_dpp_wb_trill_state_deinit(int unit);

extern int
_bcm_dpp_wb_trill_sync(int unit);

extern int
_bcm_dpp_wb_trill_update_mask_set_state(int unit, int mask_set_value);

extern int
_bcm_dpp_wb_trill_update_ports_state(int unit, _bcm_petra_trill_port_list_t *trill_ports);

extern int
_bcm_dpp_wb_trill_update_out_ac_state(int unit, SOC_PPD_AC_ID trill_out_ac);

extern int
_bcm_dpp_wb_trill_update_last_used_id_state(int unit, int last_used_id_value);

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_TRILL_H_ */
