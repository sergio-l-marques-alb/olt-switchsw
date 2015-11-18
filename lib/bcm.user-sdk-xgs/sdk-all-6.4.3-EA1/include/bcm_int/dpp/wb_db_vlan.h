/*
 * $Id: wb_db_vlan.h,v 1.1 Broadcom SDK $
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
 * Module: VLAN APIs
 *
 * Purpose:
 *     VLAN API for Dune Packet Processor devices
 *     Warm boot support
 */

#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/vlan.h>

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 *  Alignment needs to be forced
 */

#define _BCM_DPP_WB_VLAN_ALIGN_SIZE 4
#define _BCM_DPP_WB_VLAN_ALIGNED_SIZE(_x) \
    (((_x) + (_BCM_DPP_WB_VLAN_ALIGN_SIZE - 1)) & \
     (~(_BCM_DPP_WB_VLAN_ALIGN_SIZE - 1)))

/*
 *  Versioning...
 */
#define _BCM_DPP_WB_VLAN_VERSION_1_0      SOC_SCACHE_VERSION(1, 0)
#define _BCM_DPP_WB_VLAN_VERSION_CURR     _BCM_DPP_WB_VLAN_VERSION_1_0

/* 
 * dirty bit handling
 */
#define BCM_DPP_WB_DEV_VLAN_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        SOC_CONTROL_UNLOCK(unit);

/*
 *  Overall descriptor for v1.0
 */
typedef struct _bcm_dpp_wb_1_0_vlan_param_s {
    /* offsets */
    uint32 offsetVlanInfo;
    uint32 offsetFidRef;
    /* limits */
    uint32 limitVlan;
    uint32 limitSharedFid;
    uint32 limitVsiLifIndex;
    uint32 limitPortsInPbmp;
} _bcm_dpp_wb_1_0_vlan_param_t;

/*
 *  VLAN information vor v1.0
 *
 *  Note the three bitmaps are stored one after another in bitmaps[]:
 *    defl_pbmp
 *    defl_ubmp
 *    vlan_bmp
 */
typedef struct _bcm_dpp_wb_1_0_vlan_info_s {
    uint32 flags;
    uint16 defl;
    uint16 count;
    uint32 bitmaps[1];
} _bcm_dpp_wb_1_0_vlan_info_t;

/*
 *  FID reference count for v1.0
 */
typedef struct _bcm_dpp_wb_1_0_vlan_fid_ref_count_s {
    uint32 fid;
    uint32 refCount;
} _bcm_dpp_wb_1_0_vlan_fid_ref_count_t;


extern uint8 wb_vlan_is_scheduled_sync[BCM_MAX_NUM_UNITS];

/*
 *  Function
 *    _bcm_dpp_vlan_fid_ref_count_save
 *  Purpose
 *    Write a single FID reference count to backing store
 *  Arguments
 *    IN unit = unit number
 *    IN index = which FID reference count to write
 *    OUT data = buffer for workspace, or NULL to write to backing store
 *    OUT size = buffer for size, or NULL to actually write
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Will write to backing store if data and size are NULL.
 *
 *    Updates value size to indicate a single entry size if size is not NULL.
 *
 *    Writes to provided space (format as backing store) if data is not NULL.
 */
extern void
_bcm_dpp_vlan_fid_ref_count_save(int unit,
                                 unsigned int index,
                                 uint8 *data,
                                 unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_vlan_info_wb_save_all
 *  Purpose
 *    Write the VLAN information to backing store
 *  Arguments
 *    IN unit = unit number
 *    OUT data = buffer for workspace, or NULL to write to backing store
 *    OUT size = buffer for size, or NULL to actually write
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Will write to backing store if data and size are NULL.
 *
 *    Updates value size to indicate a single entry size if size is not NULL.
 *
 *    Writes to provided space (format as backing store) if data is not NULL.
 */
extern void
_bcm_dpp_vlan_info_wb_save_all(int unit,
                               uint8 *data,
                               unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_vlan_all_wb_state_sync_int
 *  Purpose
 *    Sync the entire VLAN state to the warm boot buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the entire set of
 *    backing store records to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and will
 *    fill it in completely.
 *
 *    This tries to minimise writes if it can -- if the caller does not provide
 *    a buffer, it tries to allocate its own buffer.  This is to reduce
 *    overhead in writing to, and wear and tear of, the backing store.  If it
 *    can not allocate a sufficiently large buffer (which is possible, since
 *    the buffer it will need is pretty big), it simply performs the sync as
 *    individual writes.
 *
 *    Should NOT use this call except from within the VLAN module (see below).
 */
extern void
_bcm_dpp_vlan_all_wb_state_sync_int(int unit,
                                    uint8 *data,
                                    unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_vlan_all_wb_state_sync
 *  Purpose
 *    Sync the entire field state to the warm boot buffer
 *  Arguments
 *    IN unit = unit number
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Does not report error conditions for many things...
 *
 *    Use this call from anywhere outside of the VLAN module.
 */
extern int
_bcm_dpp_vlan_all_wb_state_sync(int unit);

/*
 *  Function
 *    _bcm_dpp_wb_vlan_state_init
 *  Purpose
 *    Initialise the warm boot support for field APIs
 *  Arguments
 *    IN unit = unit number
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
extern int
_bcm_dpp_wb_vlan_state_init(int unit);



#endif /* def BCM_WARM_BOOT_SUPPORT */

