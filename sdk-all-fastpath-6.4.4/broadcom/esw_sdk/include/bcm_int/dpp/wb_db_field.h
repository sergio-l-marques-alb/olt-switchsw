/*
 * $Id: wb_db_field.h,v 1.7 Broadcom SDK $
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
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Packet Processor devices
 *     Warm boot support
 */

#ifndef _WB_DB_FIELD_H_
#define _WB_DB_FIELD_H_ 1

/*
 *  Right now, warm boot support for field APIs on DPP devices is a work in
 *  progress.  Basically, it is unfinished (backing store space is allocated,
 *  writes to backing store are performed, but recovery is not in place).  For
 *  testing purposes, it has been included in the branches for builds and
 *  validation, to ensure that it does not cause undesirable effects as the
 *  work progresses.
 *
 *  However, since it is unfinished, it merely consumes resources and provides
 *  no particular benefit in the field.  It should therefore remain disabled,
 *  so _BCM_DPP_FIELD_WARM_BOOT_SUPPORT should be set to FALSE.
 */
#define _BCM_DPP_FIELD_WARM_BOOT_SUPPORT TRUE

#if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT

#include <bcm_int/dpp/field_int.h>
#include <soc/scache.h>

/*
 *  Alignment needs to be forced
 */

#define _BCM_DPP_WB_FIELD_ALIGN_SIZE 4
#define _BCM_DPP_WB_FIELD_ALIGNED_SIZE(_x) \
    (((_x) + (_BCM_DPP_WB_FIELD_ALIGN_SIZE - 1)) & \
     (~(_BCM_DPP_WB_FIELD_ALIGN_SIZE - 1)))

/*
 *  Versioning...
 */
#define _BCM_DPP_WB_FIELD_VERSION_1_0      SOC_SCACHE_VERSION(1, 0)
#define _BCM_DPP_WB_FIELD_VERSION_CURR     _BCM_DPP_WB_FIELD_VERSION_1_0

/* 
 * dirty bit handling
 */
#define BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_SIZE(isExternalTcam) \
    ((!isExternalTcam) ? (sizeof(_bcm_dpp_wb_1_0_field_entry_tcam_t)) : (sizeof(_bcm_dpp_wb_1_0_field_entry_ext_tcam_t)))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam) \
    ((!isExternalTcam) ? (&(wbDataIntTcam->entryCmn.entryFlags)) : (&(wbDataExtTcam->entryCmn.entryFlags)))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam) \
    ((!isExternalTcam) ? (wbDataIntTcam->entryCmn.entryFlags) : (wbDataExtTcam->entryCmn.entryFlags))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_HW_HANDLE_PTR(isExternalTcam) \
    ((!isExternalTcam) ? (&(wbDataIntTcam->entryCmn.hwHandle)) : (&(wbDataExtTcam->entryCmn.hwHandle)))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam) \
    ((!isExternalTcam) ? (wbDataIntTcam->entryCmn.hwHandle) : (wbDataExtTcam->entryCmn.hwHandle))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_GROUP_PTR(isExternalTcam) \
    ((!isExternalTcam) ? (&(wbDataIntTcam->entryCmn.entryGroup)) : (&(wbDataExtTcam->entryCmn.entryGroup)))

#define BCM_DPP_WB_FIELD_TCAM_ENTRY_GROUP(isExternalTcam) \
    ((!isExternalTcam) ? (wbDataIntTcam->entryCmn.entryGroup) : (wbDataExtTcam->entryCmn.entryGroup))

/*
 *  Backing store version 1.0 entry common data
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_cmn_s {
    uint32 entryFlags;                          /* entry flags */
    int32 entryPriority;                        /* entry priority */
    uint32 hwHandle;                            /* entry handle in hardware */
    uint32 entryGroup;   /* only need 1 byte */ /* entry group */
} _bcm_dpp_wb_1_0_field_entry_cmn_t;

/*
 *  Backing store version 1.0 external entry common data
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_cmn_ext_s {
    uint32 entryFlags;                          /* entry flags */
    uint32 hwHandle;                            /* entry handle in hardware */
    uint32 entryGroup;   /* only need 1 byte */ /* entry group */
} _bcm_dpp_wb_1_0_field_entry_cmn_ext_t;

/*
 *  Backing store version 1.0 TCAM entry data (per action)
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_tcam_action_s {
    bcm_field_action_t bcmType;                 /* BCM action type */
    uint32 bcmParam0;                           /* action param0 */
    uint32 bcmParam1;                           /* action param1 */
} _bcm_dpp_wb_1_0_field_entry_tcam_action_t;

/*
 *  Backing store version 1.0 view of a TCAM entry
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_tcam_s {
    _bcm_dpp_wb_1_0_field_entry_cmn_t         entryCmn;
    _bcm_dpp_wb_1_0_field_entry_tcam_action_t tcamAction[1];
} _bcm_dpp_wb_1_0_field_entry_tcam_t;

/*
 *  Backing store version 1.0 view of an external TCAM entry
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_ext_tcam_s {
    _bcm_dpp_wb_1_0_field_entry_cmn_ext_t   entryCmn;
} _bcm_dpp_wb_1_0_field_entry_ext_tcam_t;

/*
 *  Backing store version 1.0 Direct Extraction entry data (per action)
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_dir_ext_action_s {
    bcm_field_action_t bcmType;                 /* BCM action type */
} _bcm_dpp_wb_1_0_field_entry_dir_ext_action_t;

/*
 *  Backing store version 1.0 view of a Direct Extraction entry
 */
typedef struct _bcm_dpp_wb_1_0_field_entry_dir_ext_s {
    _bcm_dpp_wb_1_0_field_entry_cmn_t            entryCmn;
    _bcm_dpp_wb_1_0_field_entry_dir_ext_action_t dirExtAction[1];
} _bcm_dpp_wb_1_0_field_entry_dir_ext_t;

/*
 *  Backing store version 1.0 group data
 */
typedef struct _bcm_dpp_wb_1_0_field_group_s {
    uint32 groupFlags;                          /* group flags */
    uint32 hwHandle;                            /* group handle in hardware */
    uint32 grpMode;                             /* group mode */
    uint32 grpStage;                            /* which hardware stage */
    uint32 predefKey;                           /* which predefined key */
    uint32 oaset[1];                            /* original (caller's) aset */
} _bcm_dpp_wb_1_0_field_group_t;

/*
 *  Backing store version 1.0 stage data
 *
 *  The ipfgs array contains the presel_set for each of the four usual 'basic'
 *  types for implied preselectors, in order: Ethernet, IPv4, IPv6, MPLS.
 */
typedef struct _bcm_dpp_wb_1_0_field_stage_s {
    uint32 ipfgs[1];                            /* implied PFGs by type */
} _bcm_dpp_wb_1_0_field_stage_t;

/*
 *  Backing store version 1.0 preselector (all) data
 *
 *  The preselData array contains preselector sets for the common four basic
 *  frame types, then reference counts for the individual preselectors (with
 *  each biased by 1 to indicate in use, so 0 indicates not in use, 1
 *  indicates in use with 0 references, 2 is in use with one reference, &c).
 */
typedef struct _bcm_dpp_wb_1_0_field_presel_all_s {
    uint32 preselRefcounts[_BCM_DPP_PRESEL_NOF_PORT_PROFILES*_bcmDppFieldProfileTypeCount]; /* In/Out Port RefCounts */
    uint32 preselData[1];                       /* bitmaps, refcounts, &c */
} _bcm_dpp_wb_1_0_field_presel_all_t;

/*
 *  Backing store version 1.0 data field (all) data
 *
 *  The dataFieldRefs array contains flags and reference counts for the data
 *  fields.  The flags are in the upper bits (bit 31 indicates length is in
 *  bits, bit 30 indicates offset is in bits), and the reference count in the
 *  lower 30 bits and is biased by 1 to indicate in use, so zero indicates not
 *  in use, 1 indicates in use with 0 references, 2 in use with 1 ref, &c.
 */
typedef struct _bcm_dpp_wb_1_0_field_datafield_all_s {
    uint32 dataFieldRefs[1];                    /* reference counts & flags */
} _bcm_dpp_wb_1_0_field_datafield_all_t;

/*
 *  Backing store version 1.0 single range type
 *
 *  There will be one of these for each range type; they will be serially
 *  placed in the warm boot backing store.
 *
 *  The rangeData array contains a bitmap indicating which ranges are in use.
 */
typedef struct _bcm_dpp_wb_1_0_field_range_all_s {
    uint32 qualifyType;                         /* bcmFieldQualify* for range */
    uint32 wb_rangeLimit;                       /* number of this range type */
    uint32 rangeData[1];                        /* range in use bits */
} _bcm_dpp_wb_1_0_field_range_all_t;

/*
 *  Backing store version 1.0 unit data
 */
typedef struct _bcm_dpp_wb_1_0_field_unit_s {
    uint32 unitFlags;                           /* unit flags */
} _bcm_dpp_wb_1_0_field_unit_t;

/*
 *  Backing store version 1.0 format information
 */
typedef struct _bcm_dpp_wb_1_0_field_param_s {
    /* offsets */
    uint32 offsetUnit;                          /* offset to unit data */
    uint32 offsetRange;                         /* offset to range data */
    uint32 offsetDataField;                     /* offset to data field data */
    uint32 offsetPresel;                        /* offset to preselector data */
    uint32 offsetStage;                         /* offset to stage data */
    uint32 offsetGroup;                         /* offset to group data */
    uint32 offsetTcamEntry;                     /* offset to TCAM ent data */
    uint32 offsetExtTcamEntry;                  /* offset to external TCAM ent data */
    uint32 offsetDirExtEntry;                   /* offset to Dir Ext ent data */
    uint32 totalSize;                           /* offset beyond all data */
    /* limits */
    uint32 bcmFieldQualifyCount;                /* bcmFieldQualifyCount */
    uint32 bcmFieldActionCount;                 /* bcmFieldActionCount */
    uint32 tcamBcmActionLimit;                  /* TCAM entry BCM action lim */
    uint32 ppdActionLimit;                      /* PPD layer aciton limit */
    uint32 ppdActionCount;                      /* high value for PPD action */
    uint32 ppdQualLimit;                        /* PPD layer qualifier limit */
    uint32 ppdQualCount;                        /* high value for PPD qual */
    uint32 preselLimit;                         /* preselector limit */
    uint32 dataFieldLimit;                      /* data field limit */
    uint32 tcamEntryLimit;                      /* TCAM entry limit */
    uint32 extTcamEntryLimit;                   /* external TCAM entry limit */
    uint32 dirextEntryLimit;                    /* Dirext Extraction ent lim */
    uint32 groupLimit;                          /* group limit */
    uint32 stageLimit;                          /* stage limit */
    uint32 rangeLimit;                          /* range of one type limit */
    uint32 rangeTypes;                          /* range type count */
} _bcm_dpp_wb_1_0_field_param_t;


extern uint8 wb_field_is_scheduled_sync[BCM_MAX_NUM_UNITS]; /*not autosync*/

/*
 *  Function
 *    _bcm_dpp_field_tcam_entry_wb_save
 *  Purpose
 *    Save a TCAM entry's information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN entry = which entry to save
 *    IN isExternalTcam = is entry in external TCAM
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of a single TCAM entry
 *    backing store record to the unsigned int pointed to by size (ignoring the
 *    entry value).
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    If entryFlags does not indicate IN_HW, this will zero the data instead.
 *    This is because we are not keeping track of any entry not in hardware
 *    across a warm boot and there have been concerns about being able to
 *    compress the backing store (since we are doing random writes to it, it is
 *    obvious that this compression must be done across the entire backing
 *    store rather than as part of the write, but that is a different matter).
 */
extern void
_bcm_dpp_field_tcam_entry_wb_save(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  uint8 isExternalTcam,
                                  uint8 *data,
                                  unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_entry_wb_save
 *  Purpose
 *    Save a Direct Extraction entry's information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN entry = which entry to save
 *    OUT data = where to put the data
 *    OUT size = where to put entry size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of a single Direct
 *    Extraction entry backing store record to the unsigned int pointed to by
 *    size (ignoring the entry value).
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    If entryFlags does not indicate IN_HW, this will zero the data instead.
 *    This is because we are not keeping track of any entry not in hardware
 *    across a warm boot and there have been concerns about being able to
 *    compress the backing store (since we are doing random writes to it, it is
 *    obvious that this compression must be done across the entire backing
 *    store rather than as part of the write, but that is a different matter).
 */
extern void
_bcm_dpp_field_dir_ext_entry_wb_save(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     uint8 *data,
                                     unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_group_wb_save
 *  Purpose
 *    Save a group's information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN group = which group to save
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of a single group backing
 *    store record to the unsigned int pointed to by size (ignoring the group
 *    value).
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    If groupFlags does not indicate IN_HW, this will zero the data instead.
 *    This is because we are not keeping track of any group not in hardware
 *    across a warm boot and there have been concerns about being able to
 *    compress the backing store (since we are doing random writes to it, it is
 *    obvious that this compression must be done across the entire backing
 *    store rather than as part of the write, but that is a different matter).
 */
extern void
_bcm_dpp_field_group_wb_save(_bcm_dpp_field_info_t *unitData,
                             _bcm_dpp_field_grp_idx_t group,
                             uint8 *data,
                             unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_stage_wb_save
 *  Purpose
 *    Save a stage's information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN stage = which stage to save
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of a single stage backing
 *    store record to the unsigned int pointed to by size (ignoring the stage
 *    value).
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    At this point, there is nothing to save per stage.
 */
extern void
_bcm_dpp_field_stage_wb_save(_bcm_dpp_field_info_t *unitData,
                             _bcm_dpp_field_stage_idx_t stage,
                             uint8 *data,
                             unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_presel_all_wb_save
 *  Purpose
 *    Save the unit's preselector information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the preselector backing
 *    store record to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    The preselector information is pretty small, so this just writes it all
 *    at once rather than taking a preselector argument.  Also, in addition to
 *    the data for a single preselector it might have to update the implied
 *    preselector information, so it might have needed to do two very small
 *    writes instead of one slightly larger one (and usually the overhead of
 *    two very small writes is much less than one small write).
 *
 *    Reference counts written are zero if the preselector is not in use (not
 *    allocated) and actual reference count plus one if it is allocated.  This
 *    way, the allocation flag and reference count are represented at once.
 *
 *    An eventual change in model for this feature will probably necessitate an
 *    update to the backing store format, but that change would come after full
 *    multi-stage support has been started (more than just ingress/egress --
 *    later DPP hardware supports programmable processing of this form in
 *    several additional stages.  It will likely look more like the range
 *    handling does now.
 */
extern void
_bcm_dpp_field_presel_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                  uint8 *data,
                                  unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_datafield_all_wb_save
 *  Purpose
 *    Save the unit's data field information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the data field backing
 *    store record to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    The data field information is pretty small, so this just writes it all at
 *    once rather than taking a data field argument.
 *
 *    Reference counts written are zero if the data field is not in use (not
 *    allocated) and actual reference count plus one if it is allocated.  This
 *    way, the allocation flag and reference count are represented at once.
 *    Also, we encode the length as bits flag as the high bit of the reference
 *    count, and the offset as bits flag as the next-to-high bit of the
 *    reference count, in order to compact the tables further.
 */
extern void
_bcm_dpp_field_datafield_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                     uint8 *data,
                                     unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_range_all_wb_save
 *  Purpose
 *    Save all range information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the range backing store
 *    record to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    If groupFlags does not indicate IN_HW, zeroes the backing store data
 *    instead. This is because we are not keeping track of any group not in
 *    hardware across a warm boot and there have been concerns about being able
 *    to compress the backing store (since we are doing random writes to it, it
 *    is obvious that this compression must be done across the entire backing
 *    store rather than as part of the write, but that is a different matter).
 */
extern void
_bcm_dpp_field_range_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                 uint8 *data,
                                 unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_unit_wb_save
 *  Purpose
 *    Save the unit's information to the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the unit backing
 *    store record to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    This always has something to write, since it applies to the whole unit,
 *    but there is not much to write, since subset writes are done for each
 *    invidivual part of the field APIs.
 */
extern void
_bcm_dpp_field_unit_wb_save(_bcm_dpp_field_info_t *unitData,
                            uint8 *data,
                            unsigned int *size);

/*
 *  Function
 *    _bcm_dpp_field_all_wb_state_sync
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
 */
extern int
_bcm_dpp_field_all_wb_state_sync(int unit);

/*
 *  Function
 *    _bcm_dpp_wb_field_state_init
 *  Purpose
 *    Initialise the warm boot support for field APIs
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
extern int
_bcm_dpp_wb_field_state_init(_bcm_dpp_field_info_t *unitData);

/*
 *  Function
 *    _bcm_dpp_wb_field_state_dump
 *  Purpose
 *    Dump the warm boot state for field APIs
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN prefix = prefix for display
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
extern void
_bcm_dpp_wb_field_state_dump(_bcm_dpp_field_info_t *unitData,
                             const char *prefix);

#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _BCM_DPP_FIELD_WARM_BOOT_SUPPORT */
#endif /* ndef _WB_DB_FIELD_H_ */

