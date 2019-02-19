/* $SDK/src/bcm/sbx/caladan3/wb_db_field.c */
/*
 * $Id: wb_db_field.c,v Exp $
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
 * Module: FIELD APIs
 *
 * Purpose:
 *     Warm boot support for FIELD API for Caladan3 Packet Processor devices
 */

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <shared/bitop.h>

#include <soc/sbx/caladan3/soc_sw_db.h>

#include <bcm_int/common/debug.h>
#include <sal/core/sync.h>
/*
#include <soc/sbx/caladan3/ocm.h>
#include <soc/sbx/g3p1/g3p1_tmu.h>
#include <soc/sbx/g3p1/g3p1_defs.h>

#include <bcm_int/sbx/caladan3/allocator.h>
*/
#include <shared/idxres_fl.h>
#include <bcm/field.h>

#define _SBX_CALADAN3_FIELD_H_NEEDED_ TRUE

#include <bcm_int/sbx/caladan3/field.h>
#include <soc/sbx/wb_db_cmn.h>
#include <bcm_int/sbx/caladan3/bcm_sw_db.h>
#include <bcm_int/sbx/caladan3/wb_db_field.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_BCM_FP

/*
 *  Set _BCM_CALADAN3_FIELD_WARMBOOT_WRITE_TRACKING to TRUE if you want diagnostics
 *  displayed at VERB level every time the FIELD backing store is written.
 */
#define _BCM_CALADAN3_FIELD_WARMBOOT_WRITE_TRACKING FALSE

/*
 *  Set _BCM_CALADAN3_FIELD_WARMBOOT_READ_TRACKING to TRUE if you want diagnostics
 *  displayed at VERB level every time FIELD backing store is read.
 */
#define _BCM_CALADAN3_FIELD_WARMBOOT_READ_TRACKING FALSE

/*
 *  Externs for FIELD
 */
extern int _bcm_caladan3_field_unit_data_size_get(int unit,
                                                  unsigned int *unitData_size);
extern void * _bcm_caladan3_field_glob_data_ptr_get(int unit);
extern void bcm_caladan3_g3p1_field_glob_t_size_get(unsigned int *size);

/*
 *  Locals for FIELD
 */
static bcm_caladan3_wb_field_state_scache_info_t 
        *_bcm_caladan3_wb_field_state_scache_info_p[BCM_MAX_NUM_UNITS] = { 0 };


/*  Definitions for BCM_CALADAN3_WB_FIELD_VERSION_1_0 */

#define _FIELD_CALADAN3_WB_G3P1_STAGES 2

typedef enum _bcm_caladan3_g3p1_fld_wb_entry_flags_1_0_e {
    _CALADAN3_WB_G3P1_ENTRY_VALID    = 0x00000001, /* entry is in use */
    _CALADAN3_WB_G3P1_ENTRY_IN_HW    = 0x00000004, /* entry is in hardware */
    _CALADAN3_WB_G3P1_ENTRY_COUNTER  = 0x00000010, /* entry has a counter */
    _CALADAN3_WB_G3P1_ENTRY_ACTIONS  = 0x000FFF20, /* entry actions mask */
    _CALADAN3_WB_G3P1_ENTRY_ACT_RES  = 0x00041000, /*  actions with resources */
    _CALADAN3_WB_G3P1_ENTRY_ACT_FTVL = 0x00001E00, /*  actions using FT/VLAN ID */
    _CALADAN3_WB_G3P1_ENTRY_DROP     = 0x00000100, /*  entry has Drop action */
    _CALADAN3_WB_G3P1_ENTRY_VLANNEW  = 0x00000200, /*  entry has VlanNew action */
    _CALADAN3_WB_G3P1_ENTRY_L3SWITCH = 0x00000400, /*  entry has L3Switch action */
    _CALADAN3_WB_G3P1_ENTRY_REDIRECT = 0x00000800, /*  entry has redirect action */
    _CALADAN3_WB_G3P1_ENTRY_REDIRMC  = 0x00001000, /*  entry has redirectMcast act */
    _CALADAN3_WB_G3P1_ENTRY_POLICER  = 0x00002000, /*  entry has policer act */
    _CALADAN3_WB_G3P1_ENTRY_COSQNEW  = 0x00010000, /*  entry has CosQNew action */
    _CALADAN3_WB_G3P1_ENTRY_DROPPREC = 0x00020000, /*  entry has DropPrecedence act */
    _CALADAN3_WB_G3P1_ENTRY_MIRROR   = 0x00040000, /*  entry has Mirror* action */
    _CALADAN3_WB_G3P1_ENTRY_CPYTOCPU = 0x00080000, /*  entry has CopyToCpu action */
    _CALADAN3_WB_G3P1_ENTRY_POLFLAGS = (int)0xE0000000, /* policer flags mask */
    _CALADAN3_WB_G3P1_ENTRY_TYPEPOL  = 0x20000000, /*  policer is typed */
    _CALADAN3_WB_G3P1_ENTRY_MEFCOS   = 0x40000000, /*  polcier is MEF by CoS */
    _CALADAN3_WB_G3P1_ENTRY_MEF      = (int)0x80000000  /*  policer is MEF */
} _bcm_caladan3_g3p1_fld_wb_entry_flags_1_0_t;


typedef struct _bcm_caladan3_g3p1_field_wb_entry_data_tgt_1_0_s {
    bcm_module_t module;                            /* target module */
    bcm_port_t port;                                /* target port */
} _bcm_caladan3_g3p1_field_wb_entry_data_tgt_1_0_t;

typedef union _bcm_caladan3_g3p1_field_wb_entry_data_1_0_u {
    _bcm_caladan3_g3p1_field_wb_entry_data_tgt_1_0_t target;   /* target module/port */
    uint32 ftHandle;                                /* FT entry handle */
    uint32 mcHandle;                                /* MC group handle */
    bcm_vlan_t VSI;                                 /* VSI */
} _bcm_caladan3_g3p1_field_wb_entry_data_1_0_t;

#define _FIELD_CALADAN3_WB_G3P1_RANGES_PER_ENTRY_1_0 2

typedef struct _bcm_caladan3_g3p1_field_wb_entry_1_0_s {
    int16 priority;                                 /* priority */
    _bcm_caladan3_g3p1_field_wb_entry_data_1_0_t ftvlData;     /* FT / VLAN data */
    bcm_policer_t policer;                          /* policer */
    bcm_field_range_t range[_FIELD_CALADAN3_WB_G3P1_RANGES_PER_ENTRY_1_0]; /* ranges */
    _bcm_caladan3_g3p1_fld_wb_entry_flags_1_0_t entryFlags;    /* internal flags */
    _bcm_caladan3_field_group_index group;              /* group */
    _bcm_caladan3_field_entry_index prevEntry;          /* prev entry (group) */
    _bcm_caladan3_field_entry_index nextEntry;          /* next entry (group) */
    _bcm_caladan3_field_entry_index prevEntryRb;        /* next entry (rulebase) */
    _bcm_caladan3_field_entry_index nextEntryRb;        /* next entry (rulebase) */
} _bcm_caladan3_g3p1_field_wb_entry_1_0_t;




typedef struct _bcm_caladan3_g3p1_field_wb_group_1_0_s {
    bcm_field_qset_t qset;                        /* qualifier set bitmap */
    int priority;                                 /* priority */
    _bcm_caladan3_field_count entries;                /* number of entries */
    _bcm_caladan3_field_count counters;               /* number of counters */
    _bcm_caladan3_field_entry_index entryHead;        /* first entry ID */
    _bcm_caladan3_field_entry_index entryTail;        /* last entry ID */
    _bcm_caladan3_field_group_index nextGroup;        /* next group ID */
    _bcm_caladan3_field_group_index prevGroup;        /* previous group ID */
    uint8 rulebase;                               /* which rulebase */
} _bcm_caladan3_g3p1_field_wb_group_1_0_t;



typedef struct _bcm_caladan3_g3p1_field_wb_rulebase_1_0_s {
    bcm_pbmp_t ports;                                        /* pbmp */
    bcm_field_qset_t qset;                                   /* qualifiers */
    SHR_BITDCL action[_SHR_BITDCLSIZE(bcmFieldActionCount)]; /* actions */
    _bcm_caladan3_field_count rules;                             /* curr rules */
    _bcm_caladan3_field_count rulesMax;                          /* max rules */
    _bcm_caladan3_field_count entries;                           /* curr entries */
    _bcm_caladan3_field_count entriesMax;                        /* max entries */
    _bcm_caladan3_field_entry_index entryHead;                   /* first entry */
    _bcm_caladan3_field_group_index groupHead;                   /* first group */
} _bcm_caladan3_g3p1_field_wb_rulebase_1_0_t;






typedef struct _bcm_caladan3_g3p1_field_wb_glob_1_0s {
    _bcm_caladan3_g3p1_field_wb_rulebase_1_0_t rulebase[_FIELD_CALADAN3_WB_G3P1_STAGES];/* rbs */
    _bcm_caladan3_g3p1_field_wb_group_1_0_t *group;            /* ptr to group array */
    _bcm_caladan3_g3p1_field_wb_entry_1_0_t *entry;            /* ptr to entries array */
    _bcm_caladan3_field_range_t *range;                 /* ptr to ranges array */
    shr_idxres_list_handle_t rangeFree;             /* range free list */
    int unit;                                       /* BCM layer unit ID */
    _bcm_caladan3_field_count groupFreeCount;           /* free groups */
    _bcm_caladan3_field_count groupTotal;               /* total groups */
    _bcm_caladan3_field_count entryFreeCount;           /* free entries */
    _bcm_caladan3_field_count entryTotal;               /* total entries */
    _bcm_caladan3_field_count rangeTotal;               /* total ranges */
    _bcm_caladan3_field_group_index groupFreeHead;      /* head of free groups */
    _bcm_caladan3_field_entry_index entryFreeHead;      /* head of free entries */
    bcm_policer_t dropPolicer;                      /* policer for drop actn */
    uint8 uMaxSupportedStages;
} _bcm_caladan3_g3p1_field_wb_glob_1_0_t;









/*
 *  Defines for FIELD
 */
#define SBX_SCACHE_INFO_PTR(unit) \
                (_bcm_caladan3_wb_field_state_scache_info_p[unit])
#define SBX_SCACHE_INFO_CHECK(unit) \
      ((_bcm_caladan3_wb_field_state_scache_info_p[unit]) != NULL \
      && (_bcm_caladan3_wb_field_state_scache_info_p[unit]->init_done == TRUE))




/*
 *  Warmboot APIs implementation for FIELD
 */
STATIC int
_bcm_caladan3_wb_field_state_scache_alloc (int unit)
{

    BCM_INIT_FUNC_DEFS;
    BCM_ALLOC (_bcm_caladan3_wb_field_state_scache_info_p[unit], 
                sizeof (bcm_caladan3_wb_field_state_scache_info_t),
                "Scache for FIELD warm boot");

exit:
    BCM_FUNC_RETURN;

}

STATIC void
_bcm_caladan3_wb_field_state_scache_free (int unit)
{

    BCM_INIT_FUNC_DEFS;
    BCM_FREE (_bcm_caladan3_wb_field_state_scache_info_p[unit]);

    BCM_FUNC_RETURN_VOID;
}


STATIC int
_bcm_caladan3_wb_field_state_layout_init (int           unit,
                                          int           version,
                                          unsigned int *scache_len)
{

    unsigned int unitData_size = 0;
    unsigned int size = 0;
    BCM_INIT_FUNC_DEFS;
    if(SBX_SCACHE_INFO_PTR(unit) == NULL)
    {
        LOG_ERROR (BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                               "Warm boot not initialized for unit %d \n"),
                    unit));
        BCM_EXIT;
    }

    switch(version)
    {
        case BCM_CALADAN3_WB_FIELD_VERSION_1_0:
            /* Initialize scache info */
            *scache_len = 0;
            SBX_SCACHE_INFO_PTR(unit)->version = version;

            /* Layout for scache length and offset calculation */

            bcm_caladan3_g3p1_field_glob_t_size_get(&size);
            assert(size == sizeof(_bcm_caladan3_g3p1_field_wb_glob_1_0_t));
            SBX_WB_DB_LAYOUT_INIT(_bcm_caladan3_g3p1_field_wb_glob_1_0_t, 1, _sbx_caladan3_field[unit].data);

            SBX_WB_DB_LAYOUT_INIT(unsigned int, 1, unitData_size);
            _bcm_caladan3_field_unit_data_size_get(unit, &unitData_size);
            
            *scache_len +=  unitData_size;

            /* Update scache length */
            SBX_SCACHE_INFO_PTR(unit)->scache_len = *scache_len;
            break;
        default:
            _rv = BCM_E_INTERNAL;
            BCM_IF_ERR_EXIT (_rv);
            break;
    }

exit:
    BCM_FUNC_RETURN;
}

int
bcm_caladan3_wb_field_state_sync (int unit)
{
    int version   = 0;
    uint8 *scache_ptr_orig = NULL;
    unsigned int unitData_size = 0;
    _bcm_caladan3_g3p1_field_wb_glob_1_0_t *glob_data_ptr = NULL;

    BCM_INIT_FUNC_DEFS;
    if(SBX_SCACHE_INFO_CHECK(unit) != TRUE) {
        LOG_ERROR (BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                           "Warm boot scache not initialized for unit %d \n"),
                    unit));
        BCM_EXIT;
    }

    if(SBX_SCACHE_INFO_PTR(unit) == NULL) {
        LOG_ERROR (BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                               "Warm boot not initialized for unit %d \n"),
                    unit));
        BCM_EXIT;
    }

    version = SBX_SCACHE_INFO_PTR(unit)->version;
    scache_ptr_orig = SBX_SCACHE_INFO_PTR(unit)->scache_ptr;

    switch(version)
    {
        case BCM_CALADAN3_WB_FIELD_VERSION_1_0:
            glob_data_ptr = (_bcm_caladan3_g3p1_field_wb_glob_1_0_t *)
                                   _bcm_caladan3_field_glob_data_ptr_get(unit);

            
            
            SBX_WB_DB_SYNC_VARIABLE(_bcm_caladan3_g3p1_field_wb_glob_1_0_t,
                                                            1, *glob_data_ptr);
            _bcm_caladan3_field_unit_data_size_get(unit, &unitData_size);
            SBX_WB_DB_SYNC_VARIABLE(unsigned int, 1, unitData_size);
            SBX_WB_DB_SYNC_MEMORY(int8, unitData_size, glob_data_ptr->group);

            /* Restore the scache ptr to original */
            SBX_SCACHE_INFO_PTR(unit)->scache_ptr = scache_ptr_orig;

            break;


        default:
            _rv = BCM_E_INTERNAL;
            BCM_IF_ERR_EXIT (_rv);
            break;
    }

exit:
    BCM_FUNC_RETURN;
}

STATIC int
_bcm_caladan3_wb_field_state_restore (int unit)
{
    int version      = 0;
    _bcm_caladan3_g3p1_field_wb_glob_1_0_t glob_data_res;
    _bcm_caladan3_g3p1_field_wb_glob_1_0_t *glob_ptr;
    unsigned int unitData_size = 0, size = 0;

    BCM_INIT_FUNC_DEFS;

    if(SBX_SCACHE_INFO_PTR(unit) == NULL)
    {
        LOG_ERROR (BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                               "Warm boot not initialized for unit %d \n"),
                    unit));
        BCM_EXIT;
    }
    version = SBX_SCACHE_INFO_PTR(unit)->version;

    switch(version)
    {
        case BCM_CALADAN3_WB_FIELD_VERSION_1_0:
            /* Restore state from scache */
            glob_ptr = (_bcm_caladan3_g3p1_field_wb_glob_1_0_t *)
                                    _bcm_caladan3_field_glob_data_ptr_get(unit);
            bcm_caladan3_g3p1_field_glob_t_size_get(&size);
            assert (size == sizeof(_bcm_caladan3_g3p1_field_wb_glob_1_0_t));

            SBX_WB_DB_RESTORE_VARIABLE(_bcm_caladan3_g3p1_field_wb_glob_1_0_t, 1, glob_data_res);

            glob_ptr->rangeFree      = glob_data_res.rangeFree;
            glob_ptr->groupFreeCount = glob_data_res.groupFreeCount;
            glob_ptr->groupTotal     = glob_data_res.groupTotal;
            glob_ptr->entryFreeCount = glob_data_res.entryFreeCount;
            glob_ptr->entryTotal     = glob_data_res.entryTotal;
            glob_ptr->groupFreeHead  = glob_data_res.groupFreeHead;
            glob_ptr->dropPolicer    = glob_data_res.dropPolicer;
            glob_ptr->uMaxSupportedStages = glob_data_res.uMaxSupportedStages;
            
            LOG_VERBOSE (BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                    "WB_FIELD: maxSupportedStage: %d\n"),
                          glob_ptr->uMaxSupportedStages));

            
            SBX_WB_DB_RESTORE_VARIABLE(unsigned int, 1, unitData_size);
            SBX_WB_DB_RESTORE_MEMORY(int8, unitData_size, glob_ptr->group);
            
            break;


        default:
            _rv = BCM_E_INTERNAL;
            BCM_IF_ERR_EXIT (_rv);
            break;
    }

exit:
    BCM_FUNC_RETURN;
}

int
bcm_caladan3_wb_field_state_init (int unit)
{
    int    flags   = SOC_CALADAN3_SCACHE_DEFAULT;
    int    exists  = 0;
    uint16 version = BCM_CALADAN3_WB_FIELD_VERSION_CURR;
    uint16 recovered_version   = 0;
    uint8 *scache_ptr          = NULL;
    unsigned int scache_len    = 0;
    soc_scache_handle_t handle = 0;

    BCM_INIT_FUNC_DEFS;
    if(SBX_SCACHE_INFO_PTR(unit)) {
        _bcm_caladan3_wb_field_state_scache_free (unit);
    }

    BCM_IF_ERR_EXIT (_bcm_caladan3_wb_field_state_scache_alloc (unit));
    if (SBX_SCACHE_INFO_PTR(unit) == NULL) {
        LOG_ERROR (BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                               "Warm boot not initialized for unit %d \n"),
                    unit));
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET (handle, unit, BCM_MODULE_FIELD, 0);

    if (SOC_WARM_BOOT (unit)) {
        /* WARM BOOT */
        /* fetch the existing warm boot space */
        _rv = bcm_caladan3_scache_ptr_get (unit,
                                           handle,
                                           socScacheRetrieve,
                                           flags,
                                          &scache_len,
                                          &scache_ptr,
                                           version,
                                          &recovered_version,
                                          &exists);
    
        if (BCM_FAILURE(_rv)) {
            LOG_ERROR (BSL_LS_BCM_FP,
                       (BSL_META_U(unit,
                                   "unable to get current warm boot state for"
                                   " unit %d FIELD instance: %d (%s)\n"),
                        unit, _rv, _SHR_ERRMSG (_rv)));
            BCM_EXIT;
        }

        LOG_VERBOSE (BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                "unit %d loading FIELD state\n"),
                      unit));
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr = scache_ptr;

        BCM_IF_ERR_EXIT (_bcm_caladan3_wb_field_state_layout_init (unit,
                                                  version, &scache_len));

        if(scache_len != SBX_SCACHE_INFO_PTR(unit)->scache_len) {
            LOG_ERROR (BSL_LS_BCM_FP,
                       (BSL_META_U(unit,
                                   "Scache length %d is not same as "
                                   "stored length %d\n"),
                       scache_len, SBX_SCACHE_INFO_PTR(unit)->scache_len));
        }
        BCM_IF_ERR_EXIT (_bcm_caladan3_wb_field_state_restore (unit));
        if (version != recovered_version) {
            /* set up layout for the preferred version */
            BCM_IF_ERR_EXIT (
                      _bcm_caladan3_wb_field_state_layout_init (unit,
                                                 version, &scache_len));
            LOG_VERBOSE (BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d reallocate %d bytes warm boot "
                                     "backing store space\n"),
                          unit, scache_len));

             /* reallocate the warm boot space */
            _rv = bcm_caladan3_scache_ptr_get (unit,
                                               handle,
                                               socScacheRealloc,
                                               flags,
                                              &scache_len,
                                              &scache_ptr,
                                               version,
                                              &recovered_version,
                                              &exists);
            if (BCM_FAILURE(_rv)) {
                LOG_ERROR (BSL_LS_BCM_FP,
                           (BSL_META_U(unit,
                                     "unable to reallocate %d bytes warm boot "
                                "space for unit %d FIELD instance: %d (%s)\n"),
                           scache_len, unit, _rv, _SHR_ERRMSG (_rv)));
                BCM_EXIT;
            }
        }		/* if (version != recovered_version) */

        SBX_SCACHE_INFO_PTR(unit)->scache_ptr = scache_ptr;
        _bcm_caladan3_wb_field_state_scache_info_p[unit]->init_done = TRUE;

    } else {
        /* COLD BOOT */
        /* set up layout for the preferred version */
        BCM_IF_ERR_EXIT (_bcm_caladan3_wb_field_state_layout_init (unit, 
                                                     version, &scache_len));

        /* set up backing store space */
        LOG_VERBOSE (BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "unit %d FIELD: allocate %d bytes warm boot "
                                 "backing store space\n"),
                      unit, scache_len));
        _rv = bcm_caladan3_scache_ptr_get (unit,
                                           handle,
                                           socScacheCreate,
                                           flags,
                                          &scache_len,
                                          &scache_ptr,
                                           version,
                                          &recovered_version,
                                          &exists);
        if (BCM_FAILURE(_rv)) {
            LOG_ERROR (BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                  "unable to allocate %d bytes warm boot space"
                                  " for unit %d field instance: %d (%s)\n"),
                         scache_len, unit, _rv, _SHR_ERRMSG (_rv)));
            BCM_EXIT;
        }
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr = scache_ptr;
        _bcm_caladan3_wb_field_state_scache_info_p[unit]->init_done = TRUE;
    }

exit:
    BCM_FUNC_RETURN;
}

#endif /* def BCM_WARM_BOOT_SUPPORT */
