/*
 * $Id: range.h$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        range.h
 * Purpose:     Internal Range module data structure definitions for the
 *              BCM library.
 *
 */

#ifndef _BCM_INT_RANGE_H
#define _BCM_INT_RANGE_H

#include <bcm/range.h>

#include <soc/mem.h>
#include <bcm/types.h>
#include <bcm_int/esw/field.h>

/*
 * Typedef:
 *     _range_t
 * Purpose:
 *     Internal management of Range Checkers.
 *     
 *     rtype   : Range Checker type - UDF/Outer Vlan/L4 Src Port/L4 Dst Port/
 *               IP Pkt length
 *     rid     : Range Checker ID.
 *     min     : Lower range to match (inclusive) and mandatory for all range 
 *               checker types.
 *     max     : Upper range to match (inclusive) and mandatory for all range
 *               checker types.
 *     hw_index: Hardware index currently being used by this range checker.
 *
 *     The fields offset/width/udf_id are applicable only when the Range 
 *     Checker Type is ****bcmRangeTypeUdf****.
 *
 *     udf_id : This is required to know UDF chunk used by this UDF can be
 *              used range checking.
 *     offset : This is the offset(in bits) for mask to apply before range 
 *              check comparison with min and max values.
 *     width  : This is the width(in bits) from offset specified for mask 
 *              to apply before range check comparison with min and max 
 *              values.
 *     
 *     ports  : This is a PBMP to specify for which pipe of XGS pipeline user
 *              wants to create this range checker. 
 *              User can specify a specific pipe or all pipes. User needs to 
 *              call bcm_port_config_get API to get "per pipe"/"all pipe" PBMP. 
 *              This parameter is mandatory for all range checker types.
 *      
 *     pipe_instance : Pipe instance
 */
typedef struct _range_s {
    bcm_range_type_t    rtype;
    bcm_range_t         rid;
    uint32              min;
    uint32              max;
    int                 hw_index;
    uint8               offset;
    uint8               width;
    bcm_udf_id_t        udf_id;
    bcm_pbmp_t          ports;
    int                 pipe_instance;
    struct _range_s     *next;
} _range_t;


typedef struct bcmi_xgs5_range_ctrl_s {
    soc_mem_t           range_mem;        /* Tcam memory   */
    sal_mutex_t         range_mutex;     /* Range lock    */
    uint32              flags;           /* misc. info    */
    uint8               num_ranges;      /* Number of Ranges existing in the system */
    uint8               max_ranges;      /* Restrict maximum ranges */
    uint32              hw_bmap;         /* HW bitmap     */
    struct _range_s     *ranges;         /* List of all ranges allocated.*/
    int                 range_oper_mode; /* Range module Oper mode. */
    uint8              range_used_by_module;   /* Range used by module:
                                                   0 - used by none
                                                   1 - used by Range module
                                                   2 - used by field module */
}bcmi_xgs5_range_ctrl_t;

/* Range Module Initialised state */
extern int _bcm_xgs5_range_init[BCM_MAX_NUM_UNITS];

extern bcmi_xgs5_range_ctrl_t *range_control[BCM_MAX_NUM_UNITS];

#define RANGE_INIT_CHECK(unit) \
    do {                                    \
        if (range_control[unit] == NULL) {  \
            return BCM_E_INIT;              \
        }                                   \
    } while(0)             


#define _RANGE_ID_BASE (0)
#define _RANGE_ID_MAX  (32 * (SOC_MAX_NUM_PIPES) - 1)

/*
 * Macro: RANGE_CTRL
 * Purpose: Range control
 */
#define RANGE_CTRL(u) range_control[u]

/*
 * Macro:   RANGE_LOCK 
 * Purpose: Lock to take the Range Mutex
 */

#define RANGE_LOCK(u)   sal_mutex_take((RANGE_CTRL(u)->range_mutex), sal_mutex_FOREVER);

/*
 * Macro:   RANGE_UNLOCK
 * Purpose: Release the Range Mutex
 */

#define RANGE_UNLOCK(u) sal_mutex_give(RANGE_CTRL(u)->range_mutex);

/* Flags for range used by module */
#define RANGE_USED_BY_FIELD_MODULE  0
#define RANGE_USED_BY_RANGE_MODULE  1

/* Extern function declarations */
extern int bcmi_xgs5_range_init(int unit);
extern int bcmi_xgs5_range_detach(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_range_scache_sync(int unit);
extern int bcmi_xgs5_range_wb_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int bcm_esw_range_init(int unit);
extern int bcm_esw_range_detach(int unit);

extern int bcm_range_id_generate(int unit, bcm_range_t rid);

extern int bcmi_xgs5_range_create(int unit, int flags, bcm_range_config_t *range_config);
extern int bcmi_xgs5_range_get(int unit, bcm_range_config_t *range_config);
extern int bcmi_xgs5_range_destroy(int unit, bcm_range_t rid);

extern int bcmi_xgs5_range_node_get(int unit, bcm_range_config_t *range_config);
extern int bcmi_range_check_set(int unit, _range_t *range, int hw_index, int enable, uint16 udf_mask); 

extern int bcmi_xgs5_range_oper_mode_set(int unit, bcm_range_oper_mode_t oper_mode);
extern int bcmi_xgs5_range_oper_mode_get(int unit, bcm_range_oper_mode_t *oper_mode);

extern int bcmi_xgs5_range_traverse(int unit, bcm_range_traverse_cb callback, void *user_data);

extern int bcmi_xgs5_range_get_hw_index(int unit, bcm_range_t rid, int *hw_index);

extern int bcmi_xgs5_range_validate_port_config(int unit, bcm_range_oper_mode_t oper_mode, bcm_pbmp_t ports, int *instance);
extern int bcmi_xgs5_range_validate_field_stage_oper_mode(int unit, bcm_field_group_oper_mode_t grp_oper_mode);
extern int bcmi_xgs5_range_validate_field_group_instance(int unit, bcm_range_t range, int instance);

extern int bcmi_xgs5_range_RangeCheckersAPIType_set (int unit, int api_type);
extern int bcmi_xgs5_range_RangeCheckersAPIType_get (int unit, int *api_type);
#endif
