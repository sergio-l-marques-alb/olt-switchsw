 /*
  * $Id: $
  *
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
  * File:       field_grp.c
  * Purpose:    BCM56960 Field Processor Group management functions.
  */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <bcm_int/esw/tomahawk.h>

#include <bcm/field.h>
#include <bcm_int/esw/field.h>

int th_prio_set_with_no_free_entries = FALSE;

/* Local functions prototypes. */
STATIC int
_field_th_group_add(int unit, _field_group_add_fsm_t *fsm_ptr);
STATIC int
_field_th_lt_default_tcam_entry_install(int unit, bcm_field_entry_t lt_entry,
                                        int group_expand);
STATIC int
_field_th_ingress_group_expand_slice_install(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg, uint8 slice_number,
                                         int part_index, int group_slice_prio);
STATIC int
_field_th_ingress_group_expand_install(int unit, _field_stage_t *stage_fc,
                                       _field_group_t *fg, int entry_part,
                                       _field_lt_slice_t *lt_fs);
STATIC int
_field_th_group_ibus_list_free(int unit, _bcm_field_qual_info_t **f_qual_arr,
                               uint16 qual_count, _field_bus_info_t **bus);
STATIC int
_field_th_update_offset_src_dst_container_sel(int unit, _field_group_t *fg,
                               uint16 qual_count,
                               _bcm_field_qual_info_t **f_qual_arr);
STATIC int
_field_th_update_ext_codes_src_dst_container_sel(int unit, _field_group_t *fg, int part,
                               uint16 src_dest_cont_sel, uint16 src_dest_cont_sel_value);

STATIC int
_field_th_src_dst_container_sel_offset(int unit, _field_group_t *fg,int idx,
                               uint16 src_dst_count, _bcm_field_qual_info_t **f_qual_arr);
/*
 * Function:
 *     _field_th_group_add_initialize
 * Purpose:
 *     Perform fsm initialization & execute basic checks
 *     required before field group creation.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX.
 */
STATIC int
_field_th_group_add_initialize(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg_temp; /* Pointer to an existing Field Group.  */
    _field_stage_id_t stage; /* Field Processor stage ID.            */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Save current FSM state in previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    if (BCM_SUCCESS(_field_group_get(unit, fsm_ptr->group_id, &fg_temp))) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "FP(unit %d) Error: group=%d already exists.\n"),
             unit, fsm_ptr->group_id));
        fsm_ptr->rv = (BCM_E_EXISTS);
    }

    /* Get Field Stage control pointer. */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _field_control_get(unit, &fsm_ptr->fc);
    }

    /* Get pipeline stage from qualifiers set.  */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _bcm_field_group_stage_get(unit, &fsm_ptr->qset, &stage);
        if ((_BCM_FIELD_STAGE_INGRESS != stage) &&
            (_BCM_FIELD_STAGE_EXACTMATCH != stage)) {
            fsm_ptr->rv = BCM_E_INTERNAL;
        }
    }

    /*
     * Attempt IntraSlice Double wide Groups if stage supports it.
     * Note: TH IFP stage default comes up in IntraSlice Double wide mode.
     */
    if (BCM_SUCCESS(fsm_ptr->rv)
        && soc_feature(unit, soc_feature_field_intraslice_double_wide)
        && (fsm_ptr->fc->flags & _FP_INTRASLICE_ENABLE)
        && (stage != _BCM_FIELD_STAGE_EXACTMATCH)) {
        fsm_ptr->flags |= _BCM_FP_GROUP_ADD_INTRA_SLICE;
    }

    /* Get Field Stage Control Pointer. */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _field_stage_control_get(unit, stage, &fsm_ptr->stage_fc);
    }

    if (BCM_SUCCESS(fsm_ptr->rv)) {
        /* Verify if requested QSET is supported by the stage. */
        if (FALSE == _field_qset_is_subset(&fsm_ptr->qset,
                        &fsm_ptr->stage_fc->_field_supported_qset)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "FP(unit %d) Error: Qualifier set is not"
                 " supported by the device.\n"), unit));
            fsm_ptr->rv = (BCM_E_UNAVAIL);
        }
    }

    if (BCM_FAILURE(fsm_ptr->rv)) {
        /* Error return hence perform clean up. */
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    } else {
        /* Proceed to group allocation state. */
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_ALLOC;
    }

    /* Execute the state machine for the new state. */
    return (_field_th_group_add(unit, fsm_ptr));
}

#if 0
/*
 * Function:
 *     _field_th_group_priority_validate
 * Purpose:
 *     Validate group prioirty value, if specified priority value is NOT
 *     BCM_FIELD_GROUP_PRIO_ANY then it must be unique.
 * Parameters:
 *     unit     - (IN  BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_priority_validate(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg)
{
    int lt_idx;                  /* Logical Table iterator.    */
    _field_lt_config_t *lt_info; /* Logical Table information. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    /*
     * If Group doesn't care about priority, one of the available
     * priorities is allocated. Hence return done.
     */
    if (BCM_FIELD_GROUP_PRIO_ANY == fg->priority) {
        return (BCM_E_NONE);
    }

    /*
     * Two Field groups cannot have the same priority value as hardware
     * behavior is Indeterministic if packet matches rules in both the groups
     * that have same Group/Action priority value.
     */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        lt_info = stage_fc->lt_info[fg->instance][lt_idx];
        if ((TRUE == lt_info->valid) && (fg->priority == lt_info->priority)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Group=%d Priority=%d already in-use.\n"),
                 unit, fg->gid, fg->priority));
            return (BCM_E_PARAM);
        }
    }

    return (BCM_E_NONE);
}
#endif /* 0 */

/*
 * Function:
 *     _field_th_group_add_alloc
 * Purpose:
 *     Allocate & initialize field group structure.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX.
 */
STATIC int
_field_th_group_add_alloc(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int idx;                    /* Iterator variable.               */
    int mem_sz;                 /* Memory size to be allocated.     */
    _field_group_t *fg = NULL;  /* New group inforation structure.  */
    bcm_port_config_t pc;       /* Port configuration.              */
    int rv;                     /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Initialize port configuration structure. */
    bcm_port_config_t_init(&pc);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &pc);
    if (BCM_FAILURE(rv)) {
        /* If port configuration get fails, go to end. */
        fsm_ptr->rv = rv;
        goto bcm_error;
    }

    /* Save current state as previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Determine field group structure buffer size. */
    mem_sz = sizeof(_field_group_t);

    /* Allocate and initialize memory for Field Group. */
    _FP_XGS3_ALLOC(fg, mem_sz, "field group");
    if (NULL == fg) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error:Allocation failure for "
             "_field_group_t\n"), unit));
        /* Memory allocation failure hence proceed to clean up. */
        fsm_ptr->rv = BCM_E_MEMORY;
        goto bcm_error;
    }

    /* Initialize group structure. */
    fg->gid            = fsm_ptr->group_id;
    fg->stage_id       = fsm_ptr->stage_fc->stage_id;
    fg->qset           = fsm_ptr->qset;
    fg->aset           = fsm_ptr->aset;
    fg->pbmp           = fsm_ptr->pbmp;
    fg->priority       = fsm_ptr->priority;
    fg->hintid         = fsm_ptr->hintid;

    fg->action_res_id       = fsm_ptr->action_res_id;
    for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
        fg->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    }

    /* Set the Field Group's instance information. */
    fsm_ptr->rv = _bcm_field_th_group_instance_set(unit, fg);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    fsm_ptr->rv = _bcm_field_hints_group_count_update(unit, fg->hintid, 1);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    /* Initialize group status. */
    fsm_ptr->rv = _bcm_field_group_status_init(unit, &fg->group_status);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    /* Hint Check. */
    fsm_ptr->rv = _bcm_field_hints_group_info_update(unit, fg);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    /* Clear the group's slice extractor selectors value. */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        _FP_EXT_SELCODE_CLEAR(fg->ext_codes[idx]);
    }

    /* By default mark group as active. */
    fg->flags |= _FP_GROUP_LOOKUP_ENABLED;

    /* Update Preselector Support Flags */
    if (fsm_ptr->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET) {
       bcm_field_presel_t       presel;

       /* Preselector feature check */
       if (!soc_feature(unit, soc_feature_field_preselector_support)) {
          fsm_ptr->rv = BCM_E_UNAVAIL;
          goto bcm_error;
       }

       /* initialize the group preselector array */
       sal_memset(&fg->presel_ent_arr[0], 0x0,
            sizeof(_field_presel_entry_t *) * _FP_PRESEL_ENTRIES_MAX_PER_GROUP);

       for (presel = 0; presel < BCM_FIELD_PRESEL_SEL_MAX; presel++) {
          if (BCM_FIELD_PRESEL_TEST(fsm_ptr->preselset, presel)) {
             /* Assign the preselectors to the group presel array. */
             fsm_ptr->rv = _bcm_field_presel_group_add(unit, fg, presel);
             if (BCM_FAILURE(fsm_ptr->rv)) {
                 goto bcm_error;
             }
          }
       }

       fg->flags |= _FP_GROUP_PRESELECTOR_SUPPORT;
    }
  
    /* Set allocated poniter to FSM structure. */
    fsm_ptr->fg = fg;

    /* Update group QSET with internal qualifiers. */
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE;

    /* Execute the state machine for the new state. */
    return _field_th_group_add(unit, fsm_ptr);

bcm_error:
    if (fg != NULL) {
       sal_free(fg);
       fg = NULL;
    }
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    return _field_th_group_add(unit, fsm_ptr);
}

/*
 * Function:
 *     _field_th_group_add_qset_update
 * Purpose:
 *     Update application requested qset with internal qualifiers.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_qset_update(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg; /* Field Group structure pointer. */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    if (NULL == fsm_ptr->fg) {
        fsm_ptr->rv = (BCM_E_PARAM);
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return (_field_th_group_add(unit, fsm_ptr));
    }

    /* Get field group structure. */
    fg = fsm_ptr->fg;

    /* Save current state as the previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* All ingress devices implicitly have "bcmFieldQualifyStage" in QSETs. */
    if ((_BCM_FIELD_STAGE_INGRESS == fg->stage_id) ||
        (_BCM_FIELD_STAGE_EXACTMATCH == fg->stage_id)) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyStage);
    }

    /* Automatically include "IpType" if QSET contains Ip4 or Ip6 qualifier. */
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)
        || BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyIpType);
    }

    /* Include "LogicalTableId", if preselectors are associated to the group. */
    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
        BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyPreLogicalTableId);
    }

    /* Update Exact Match Qset. */
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchHitStatus)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset, 
                              _bcmFieldQualifyExactMatchHitStatusLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset, 
                              _bcmFieldQualifyExactMatchHitStatusLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset, 
                              _bcmFieldQualifyExactMatchHitStatusLookup0);
            BCM_FIELD_QSET_ADD(fg->qset, 
                              _bcmFieldQualifyExactMatchHitStatusLookup1);
        }
    }

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchGroupClassId)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchGroupClassIdLookup0);
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchGroupClassIdLookup1);
        }
    }

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchActionClassId)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset,
                       _bcmFieldQualifyExactMatchActionClassIdLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset,
                       _bcmFieldQualifyExactMatchActionClassIdLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchActionClassIdLookup0);
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchActionClassIdLookup1);
        }
    }


    /*
     * NOTE:
     * Since IPBM field is only optionally included in the IFP TCAM lookup key,
     * InPorts qualifier is part of the QSET only when user explicitly adds it
     * to the Group's QSET. SDK does not include it by default as its done
     * in Pre-TH devices.
     */

    /* Proceed to Extractor Mux selector selection. */
    fsm_ptr->rv = (BCM_E_NONE);
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET;

    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *     _field_th_group_extractors_init
 * Purpose:
 *     Initialize field group extractor codes.
 * Parameters:
 *     unit      - (IN)     BCM unit number.
 *     fg        - (IN/OUT) Field Group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_extractors_init(int unit, _field_group_t *fg)
{
    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_NONE);
    }

    /*
     * Initialize group extractor code flags based on group mode being
     * created.
     */
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fg->ext_codes[1].intraslice = _FP_EXT_SELCODE_DONT_USE;
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        fg->ext_codes[1].intraslice = _FP_EXT_SELCODE_DONT_USE;
        fg->ext_codes[2].intraslice = _FP_EXT_SELCODE_DONT_USE;
    }

    fg->ext_codes[0].secondary = _FP_EXT_SELCODE_DONT_USE;
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fg->ext_codes[1].secondary = _FP_EXT_SELCODE_DONT_USE;
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        fg->ext_codes[1].secondary = _FP_EXT_SELCODE_DONT_USE;
        fg->ext_codes[2].secondary = _FP_EXT_SELCODE_DONT_USE;
    }

    return (BCM_E_NONE);
}
STATIC int
_field_th_is_post_mux_qualifier(int unit, bcm_field_qualify_t qid,
                                uint8 *is_post_mux_qualifier) 
{
    if (NULL == is_post_mux_qualifier) {
        return BCM_E_PARAM;
    }

    *is_post_mux_qualifier = FALSE; 

    if ((qid == bcmFieldQualifyInPorts)
        || (qid == bcmFieldQualifyNatDstRealmId)
        || (qid == bcmFieldQualifyNatNeeded)
        || (qid == bcmFieldQualifyDrop)
        || (qid == bcmFieldQualifyDstTrunk)
        || (qid == bcmFieldQualifyDstMultipath)
        || (qid == bcmFieldQualifyDstMulticastGroup)
        || (qid == bcmFieldQualifyDstMulticastGroups)
        || (qid == bcmFieldQualifyDstL3EgressNextHops)
        || (qid == bcmFieldQualifyDstL3Egress)
        || (qid == bcmFieldQualifyDstPort)
        || (qid == bcmFieldQualifyDstMimGport)
        || (qid == bcmFieldQualifyDstNivGport)
        || (qid == bcmFieldQualifyDstVxlanGport)
        || (qid == bcmFieldQualifyDstVlanGport)
        || (qid == bcmFieldQualifyDstMplsGport)
        || (qid == bcmFieldQualifyDstWlanGport)
        || (qid == bcmFieldQualifyDstMimGports)
        || (qid == bcmFieldQualifyDstNivGports)
        || (qid == bcmFieldQualifyDstVxlanGports)
        || (qid == bcmFieldQualifyDstVlanGports)
        || (qid == bcmFieldQualifyDstMplsGports)
        || (qid == bcmFieldQualifyDstWlanGports)) {
        *is_post_mux_qualifier = TRUE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_group_flags_to_ext_mode
 * Purpose:
 *     Determine extractor slice mode based on group flags.
 * Parameters:
 *     unit     - (IN)      BCM unit number.
 *     flags    - (IN)      Group flags value.
 *     emode    - (IN/OUT)  Extractor configuration mode.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_flags_to_ext_mode(int unit, uint16 flags,
                                  _field_ext_conf_mode_t *emode)
{
    /* Input parameters check. */
    if (NULL == emode) {
        return (BCM_E_PARAM);
    }

    if (flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        *emode = _FieldExtConfMode160Bits;
    } else if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        *emode = _FieldExtConfMode320Bits;
    } else if (flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        *emode = _FieldExtConfMode480Bits;
    } else {
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_qualifier_max_chunk_size_get
 * Purpose:
 *     Get the max chunk size of qualifier.
 * Parameters:
 *     unit         - (IN) BCM unit number.
 *     qualifier    - (IN) Field Qualifier.
 *     sec_info     - (IN) Qualifier Section Info in IBUS.
 *     max_chunk_size - (OUT) Max chunk size of the qualifier.
 * Returns:
 *     BCM_E_XXX
 */
int 
_bcm_field_qualifier_max_chunk_size_get(int unit, 
                                        bcm_field_qualify_t qualifier,
                                        _field_qual_sec_info_t *sec_info,
                                        uint8 *max_chunk_size)
{
    uint32 idx = 0; /* Index Variable. */

    /* NULL parameter check. */
    if (NULL == max_chunk_size) {
        return BCM_E_PARAM;
    }

    /* Sudo qualifiers have no size. */
    if ((bcmFieldQualifyStage == qualifier)
        || (bcmFieldQualifyStageIngress == qualifier)
        || (bcmFieldQualifyStageIngressExactMatch == qualifier)
        || (bcmFieldQualifyIp4 == qualifier)
        || (bcmFieldQualifyIp6 == qualifier)
        || (bcmFieldQualifyExactMatchHitStatus == qualifier)
        || (bcmFieldQualifyExactMatchGroupClassId == qualifier)
        || (bcmFieldQualifyExactMatchActionClassId == qualifier)
        || (bcmFieldQualifyNormalizeIpAddrs == qualifier)
        || (bcmFieldQualifyNormalizeMacAddrs == qualifier)) {
        *max_chunk_size = 0;
        return BCM_E_NONE;
    }

    /* NULL parameter check. */
    if (NULL == sec_info) {
       return BCM_E_PARAM;
    }

    *max_chunk_size = 0;
    for (idx = 0; idx < sec_info->num_chunks; idx++) {
        if (*max_chunk_size < sec_info->e_params[idx].width) {
            *max_chunk_size = sec_info->e_params[idx].width;
        }
    }
 
    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_th_group_create_ace_list
 * Purpose:
 *     Create a group ACL list from user supplied qualifiers list.
 * Parameters:
 *     unit         - (IN) BCM unit number.
 *     qual_count   - (IN) Length of qualifier array.
 *     bus_info     - (IN) Input Bus Information.
 *     ace_list     - (IN/OUT) Pointer to access control list.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_sort_ace_list(int unit, 
                                uint16 qual_count, 
                                _field_bus_info_t **bus_info,
                                _field_ace_info_t **ace_list)
{
    int rv = 0;    /* Operational Status */
    uint16 idx; /* Index Variable. */
    uint16 sort_idx; /* Index Variable. */
    uint8 is_post_mux_qualifier; /* Flag to indicate post mux qualifier. */
    uint8 max_chunk_size = 0; /* Max Chunk Size of a qualifier. */
    bcm_field_qualify_t qualifier; /* Field Qualifier. */
    _field_qual_sec_info_t **qual_sec_info; /* Qualifier section info in ibus.*/
   

    /* NULL parameters check . */
    if (NULL == bus_info || NULL == ace_list) {
        return BCM_E_PARAM;
    }

    sort_idx = 0;
    qual_sec_info = NULL;
    /* Allocate ACE list memory. */
    _FP_XGS3_ALLOC(qual_sec_info, qual_count * sizeof(_field_qual_sec_info_t *), "IFP qual section info");
    if (NULL == qual_sec_info) {
        return (BCM_E_MEMORY);
    }


    /* 1. Sudo Qualifiers. */
    for (idx = 0; idx < qual_count; idx++) {
        qualifier = (*ace_list)->qual_list[idx]->qid;
        if ((bcmFieldQualifyStage == qualifier) 
            || (bcmFieldQualifyStageIngress == qualifier)
            || (bcmFieldQualifyStageIngressExactMatch == qualifier)
            || (bcmFieldQualifyIp4 == qualifier)
            || (bcmFieldQualifyIp6 == qualifier)
            || (bcmFieldQualifyExactMatchHitStatus == qualifier)
            || (bcmFieldQualifyExactMatchGroupClassId == qualifier)
            || (bcmFieldQualifyExactMatchActionClassId == qualifier)
            || (bcmFieldQualifyNormalizeIpAddrs == qualifier)
            || (bcmFieldQualifyNormalizeMacAddrs == qualifier)) {
             qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
             sort_idx++;
        }
    }
  
    /* 2. Post Mux Qualifiers. */
    for (idx = 0; idx < qual_count; idx++) {
        qualifier = (*ace_list)->qual_list[idx]->qid;
        rv = _field_th_is_post_mux_qualifier(unit, qualifier, &is_post_mux_qualifier);
        BCM_IF_ERROR_RETURN(rv);
        if (TRUE == is_post_mux_qualifier) {
            if (sort_idx == qual_count) {
                sal_free(qual_sec_info);
                goto cleanup;
            }
            qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
            sort_idx++;
       } 
    }


    /* 3. Identify Qualifiers with secondary selectors irrespective of their
     * chunks sizes. 
     */
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;
        rv = _field_th_is_post_mux_qualifier(unit, qualifier, &is_post_mux_qualifier);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (FALSE == is_post_mux_qualifier) {

            if ((bcmFieldQualifyStage == qualifier)
               || (bcmFieldQualifyStageIngress == qualifier)
               || (bcmFieldQualifyStageIngressExactMatch == qualifier)
               || (bcmFieldQualifyIp4 == qualifier)
               || (bcmFieldQualifyIp6 == qualifier)
               || (bcmFieldQualifyExactMatchHitStatus == qualifier)
               || (bcmFieldQualifyExactMatchGroupClassId == qualifier)
               || (bcmFieldQualifyExactMatchActionClassId == qualifier)
               || (bcmFieldQualifyNormalizeIpAddrs == qualifier)
               || (bcmFieldQualifyNormalizeMacAddrs == qualifier)) {
                continue;
           }

           if (_FieldExtCtrlSelDisable != (*bus_info)->qual_list[qualifier]->ctrl_sel) {
               qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
               sort_idx++;
           }
        }
    }

    /* 4. Qualifiers with max chunk size as 32.*/
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;

        rv = _field_th_is_post_mux_qualifier(unit, qualifier, 
                                             &is_post_mux_qualifier);
        if (BCM_FAILURE(rv)) {
            goto cleanup; 
        }
        if (FALSE == is_post_mux_qualifier) {
            rv = _bcm_field_qualifier_max_chunk_size_get(unit, qualifier, 
                          (*bus_info)->qual_list[qualifier], &max_chunk_size);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (32 == max_chunk_size &&
               (_FieldExtCtrlSelDisable == 
                              (*bus_info)->qual_list[qualifier]->ctrl_sel)) {

                if (sort_idx == qual_count) {
                    goto cleanup;
                }

                qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
                sort_idx++;
            }
        } 
    }

    /* 5. Qualifiers with max chunk size as 16.*/
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;

        rv = _field_th_is_post_mux_qualifier(unit, qualifier, 
                                             &is_post_mux_qualifier);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (FALSE == is_post_mux_qualifier) {
            rv = _bcm_field_qualifier_max_chunk_size_get(unit, qualifier, 
                            (*bus_info)->qual_list[qualifier], &max_chunk_size);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (16 == max_chunk_size &&
               (_FieldExtCtrlSelDisable == 
                       (*bus_info)->qual_list[qualifier]->ctrl_sel)) {

                if (sort_idx == qual_count) {
                    goto cleanup;
                }

                qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
                sort_idx++;
            }
        }
    }

    /* 6. Qualifiers with max chunk size as 8.*/
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;

        rv = _field_th_is_post_mux_qualifier(unit, qualifier, 
                                             &is_post_mux_qualifier);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (FALSE == is_post_mux_qualifier) {
            rv = _bcm_field_qualifier_max_chunk_size_get(unit, qualifier, 
                            (*bus_info)->qual_list[qualifier], &max_chunk_size);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (8 == max_chunk_size &&
               (_FieldExtCtrlSelDisable == 
                      (*bus_info)->qual_list[qualifier]->ctrl_sel)) {

                if (sort_idx == qual_count) {
                    goto cleanup;
                }

                qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
                sort_idx++;
            }
        }
    }

    /* 7. Qualifiers with max chunk size as 4.*/
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;

        rv = _field_th_is_post_mux_qualifier(unit, qualifier, 
                                             &is_post_mux_qualifier);

        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (FALSE == is_post_mux_qualifier) {
            rv = _bcm_field_qualifier_max_chunk_size_get(unit, qualifier, 
                            (*bus_info)->qual_list[qualifier], &max_chunk_size);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (4 == max_chunk_size &&
               (_FieldExtCtrlSelDisable == 
                    (*bus_info)->qual_list[qualifier]->ctrl_sel)) {

                if (sort_idx == qual_count) {
                    goto cleanup;
                }

                qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
                sort_idx++;
            }
        }
    }

    /* 8. Qualifiers with max chunk size as 2.*/
    for (idx = 0; idx < qual_count; idx++) {

        qualifier = (*ace_list)->qual_list[idx]->qid;
        rv = _field_th_is_post_mux_qualifier(unit, qualifier, 
                                             &is_post_mux_qualifier);

        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        if (FALSE == is_post_mux_qualifier) {

            rv = _bcm_field_qualifier_max_chunk_size_get(unit, qualifier, 
                            (*bus_info)->qual_list[qualifier], &max_chunk_size);

            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (2 == max_chunk_size &&
               (_FieldExtCtrlSelDisable == 
                  (*bus_info)->qual_list[qualifier]->ctrl_sel)) {

                if (sort_idx == qual_count) {
                    goto cleanup;
                }

                qual_sec_info[sort_idx] = (*ace_list)->qual_list[idx];
                sort_idx++;
            }
        }
    }

    if (qual_count != sort_idx) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    for (idx = 0; idx < qual_count; idx++) {
        (*ace_list)->qual_list[idx] = qual_sec_info[idx];
    }

cleanup:
    if (NULL != qual_sec_info) {
        sal_free(qual_sec_info);
    }

    return rv;
}
/*
 * Function:
 *     _field_th_group_create_ace_list
 * Purpose:
 *     Create a group ACL list from user supplied qualifiers list.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure pointer.
 *     f_qual_arr   - (IN)     Field qualifier array.
 *     qual_count   - (IN)     Length of qualifier array.
 *     key_size     - (IN)     Group qualifiers list key size.
 *     level        - (IN)     Extractor level.
 *     ace_list     - (IN/OUT) Pointer to access control list.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_create_ace_list(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg,
                                _bcm_field_qual_info_t **f_qual_arr,
                                uint16 qual_count, uint16 key_size,
                                int level, _field_ace_info_t **ace_list)
{
    int    idx; /* Index iterator. */
    uint32 width;
    int src_dst_cont_flags = 0; /* to keep track of SRC DST Container Flags */
    int count_src_dst_cont = 0;	/* for keeping track of postmuxers */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == f_qual_arr)
        || (NULL == ace_list)) {
        return (BCM_E_NONE);
    }

    /* Allocate ACE list memory. */
    _FP_XGS3_ALLOC(*ace_list, sizeof(_field_ace_info_t), "IFP ace info");
    if (NULL == *ace_list) {
        return (BCM_E_MEMORY);
    }

    /* Allocate memory for qualifiers in the ACE list. */
    _FP_XGS3_ALLOC((*ace_list)->qual_list,
        (qual_count * sizeof(_field_qual_sec_info_t *)), "IFP ace list");
    if (NULL == (*ace_list)->qual_list) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Allocation failure for ACE Qual list"
            " Pointers.\n"), unit));
        sal_free(*ace_list);
        return (BCM_E_MEMORY);
    }

    /* Allocate memory for qualifier section information structure. */
    for (idx = 0; idx < qual_count; idx++) {
        _FP_XGS3_ALLOC((*ace_list)->qual_list[idx],
            sizeof(_field_qual_sec_info_t), "IFP qual section");
        if (NULL == (*ace_list)->qual_list[idx]) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for ACE Qual list.\n"),
                unit));
            sal_free((*ace_list)->qual_list);
            sal_free(*ace_list);
            return (BCM_E_MEMORY);
        }
    }

    /* Update ACE key size information. */
    (*ace_list)->size = key_size;

    /* Set the Group ID. */
    (*ace_list)->gid = fg->gid;

    /* Add qualifiers to the newly created ACE list. */
    for (idx = 0; idx < qual_count; idx++) {

        /* Set the qualifier name. */
        (*ace_list)->qual_list[idx]->qid = f_qual_arr[idx]->qid;

        /* Check and initialize post muxed qualifiers settings. */
        switch ((int)(*ace_list)->qual_list[idx]->qid) {
            case (int)bcmFieldQualifyInPorts:
                (*ace_list)->flags |= (_FP_POST_MUX_IPBM);
                break;
            case (int)bcmFieldQualifyNatDstRealmId:
                (*ace_list)->flags |= (_FP_POST_MUX_NAT_DST_REALM_ID);
                break;
            case (int)bcmFieldQualifyNatNeeded:
                (*ace_list)->flags |= (_FP_POST_MUX_NAT_DST_REALM_ID);
                break;
            case (int)bcmFieldQualifyDrop:
                (*ace_list)->flags |= (_FP_POST_MUX_DROP);
                break;
            case (int)_bcmFieldQualifyExactMatchHitStatusLookup0:
                (*ace_list)->flags |= (_FP_POST_MUX_EM_HIT_STATUS_LOOKUP_0);
                break;
            case (int)_bcmFieldQualifyExactMatchHitStatusLookup1:
                (*ace_list)->flags |= (_FP_POST_MUX_EM_HIT_STATUS_LOOKUP_1);
                break;
            case (int)_bcmFieldQualifyExactMatchActionClassIdLookup0:
                (*ace_list)->flags |= (_FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_0);
                break;
            case (int)_bcmFieldQualifyExactMatchActionClassIdLookup1:
                (*ace_list)->flags |= (_FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_1);
                break;
	/* Though HW allows SrcXXXGport/s to use SRC_DST_CONTAINERS as postmuxs */
	/* commented them to efficiently use the SRC_DST_CONT for DstXXXGPorts  */
	/* SrcXXXGport/s can use SRC_CONT_A/B available only for SrcXXXports    */
	#if 0
            case (int)bcmFieldQualifySrcModPortGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case (int)bcmFieldQualifySrcMimGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case (int)bcmFieldQualifySrcNivGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case (int)bcmFieldQualifySrcVxlanGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case (int)bcmFieldQualifySrcMplsGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case (int)bcmFieldQualifySrcVlanGport:
            /* coverity[MISSING_BREAK: FALSE] */
	#endif
            case (int)bcmFieldQualifyDstTrunk:
	    case (int)bcmFieldQualifyDstPort:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DGLP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DGLP);
		    count_src_dst_cont++;
		}
		break;
	    case (int)bcmFieldQualifyDstMimGport:
	    case (int)bcmFieldQualifyDstMimGports:
	    case (int)bcmFieldQualifyDstNivGport:
	    case (int)bcmFieldQualifyDstNivGports:
	    case (int)bcmFieldQualifyDstVxlanGport:
	    case (int)bcmFieldQualifyDstVxlanGports:
	    case (int)bcmFieldQualifyDstVlanGport:
	    case (int)bcmFieldQualifyDstVlanGports:
	    case (int)bcmFieldQualifyDstMplsGport:
	    case (int)bcmFieldQualifyDstMplsGports:
	    case (int)bcmFieldQualifyDstWlanGport:
	    case (int)bcmFieldQualifyDstWlanGports:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DVPLAG)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DVPLAG);
		    count_src_dst_cont++;
		}
		break;
	    case (int)bcmFieldQualifyDstMultipath:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_ECMP1)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_ECMP1);
		    count_src_dst_cont++;
		}
		break;
	    case (int)bcmFieldQualifyDstL3Egress:
	    case (int)bcmFieldQualifyDstL3EgressNextHops:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_NEXT_HOP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_NEXT_HOP);
		    count_src_dst_cont++;
		}
		break;
	    case (int)bcmFieldQualifyDstL2MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_L2MC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_L2MC_GROUP);
		    count_src_dst_cont++;
		}
		break;
	    case (int)bcmFieldQualifyDstL3MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_IPMC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_IPMC_GROUP);
		    count_src_dst_cont++;
		}
		break;
	    default:
		;
	}

        /* Clear bits used count. */
        (*ace_list)->qual_list[idx]->bits_used = 0;

        /* Initialize qualifier key size. */
        _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(&f_qual_arr[idx]->conf_arr[0].offset,
                                           width);
        (*ace_list)->qual_list[idx]->size = width;

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: ACE[Level=%d] Gid=%d Qid=%d IPBM=%d size=%d.\n"),
            unit, level, (*ace_list)->gid, (*ace_list)->qual_list[idx]->qid,
            ((*ace_list)->flags & _FP_POST_MUX_IPBM) ? 1 : 0,
            (*ace_list)->size));
    }

    /* Total there can be 6 SRC_DST_CONT to */
    /* accomadate 6 postmux groups in Triple wide mode*/
    /* i.e., 2 per Physical slice/part */
    switch(count_src_dst_cont) {
	case 1:
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    break;
	case 2:
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1);
	    break;
	case 3:
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_B);
	    break;
	case 4:
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_B);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1_SLI_B);
	    break;
	case 5:
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_B);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1_SLI_B);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_C);
	    break;
	case 6 :
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_B);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1_SLI_B);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_0_SLI_C);
	    (*ace_list)->flags |= (_FP_POST_MUX_SRC_DST_CONT_1_SLI_C);
	    break;
	default:
	    ;
    }

    /* check the validity of postmuxers */
    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) &&
	    (((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) && count_src_dst_cont > 2)
	     || ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) && count_src_dst_cont > 4)
	     || ((fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) && count_src_dst_cont > 6))) {

	LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
			"FP(unit %d) VERB: Cannot have %d SRC_DST_CONT in "
			"Current configuration.\n"),
		    unit, count_src_dst_cont));
	return (BCM_E_CONFIG);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_ibus_copy_create
 * Purpose:
 *     Construct an input bus extractor configuration for the user supplied
 *     QSET.
 * Parameters:
 *     unit         - (IN)  BCM unit number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure pointer.
 *     f_qual_arr   - (IN)  Field qualifier array.
 *     qual_count   - (IN)  Field qualifier array length.
 *     key_size     - (IN)  Group ACL key size.
 *     buses        - (IN/OUT) Extractor input bus structure pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_ibus_copy_create(int unit, _field_stage_t *stage_fc,
                                 _field_group_t *fg,
                                 _bcm_field_qual_info_t **f_qual_arr,
                                 uint16 qual_count, uint16 key_size,
                                 _field_bus_info_t **buses)
{
    _field_qual_sec_info_t *ibus_qual_info = NULL; /* Input bus structure.  */
    _field_qual_sec_info_t *qual = NULL;   /* Field qualifier section info. */
    int idx;                                       /* Index iterator.       */
    bcm_field_qualify_t qid;                       /* Qualifier Identifier. */

    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == f_qual_arr)
        || (NULL == buses)) {
        return (BCM_E_PARAM);
    }

    /* Allocate ACE list memory. */
    _FP_XGS3_ALLOC(*buses, sizeof(_field_bus_info_t), "IFP bus info");
    if (NULL == *buses) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Allocation failure for Bus Info.\n"),
            unit));
        return (BCM_E_MEMORY);
    }

    /* Allocate input bus qualifiers section information pointers. */
    _FP_XGS3_ALLOC((*buses)->qual_list,
        (_bcmFieldQualifyCount * sizeof(_field_qual_sec_info_t *)),
         "IFP bus qual sec info ptr");
    if (NULL == (*buses)->qual_list) {
        sal_free(*buses);
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Allocation failure for Bus Qual Sec Info.\n"),
            unit));
        return (BCM_E_MEMORY);
    }

    /*
     * Append qualifiers to input bus with qualifier section information
     * details.
     */
    for (idx = 0; idx < qual_count; idx++) {

        /* Get the qualifier identifier from the requested qset list. */
        qid = f_qual_arr[idx]->qid;

        /*
         * Index using qualifier id and get the qualifier section
         * information from input bus qualifiers list.
         */
        ibus_qual_info = stage_fc->input_bus.qual_list[qid];

        /* Confirm memory is allocated for the new list element. */
        for (ibus_qual_info = stage_fc->input_bus.qual_list[qid];
             NULL != ibus_qual_info; 
             ibus_qual_info = ibus_qual_info->next) {

            /* Allocate memory for new qualifier added to the bus. */
            _FP_XGS3_ALLOC(qual, sizeof(_field_qual_sec_info_t), 
                                           "IFP qual sec info");
            
            if (NULL == qual) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: Allocation failure for Bus Qual"
                    " list.\n"), unit));
                _field_th_group_ibus_list_free(unit, f_qual_arr, qual_count, buses); 
                return BCM_E_MEMORY;
            }

            /* Copy qualifier extractor section information from input bus. */
            sal_memcpy(qual, ibus_qual_info, sizeof(_field_qual_sec_info_t));
            qual->next = NULL;
        
            if (NULL == stage_fc->input_bus.qual_list[qid]) {
                (*buses)->qual_list[qid] = qual;
                qual = NULL;
                /* Increment number of fields in the bus. */
                (*buses)->num_fields += 1;
            } else {
                qual->next = (*buses)->qual_list[qid];
                (*buses)->qual_list[qid] = qual;
                qual = NULL;
            }
 
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: IBUS[Level_0] Qid=%d\n"
                "\t\tSec_1=%d Sec_val_1=%d bus_offset=%d width=%d\n"
		"\t\tSec_2=%d Sec_val_2=%d bus_offset=%d width=%d\n"
                "\t\tSec_3=%d Sec_val_3=%d bus_offset=%d width=%d\n"
		"\t\tSec_4=%d Sec_val_4=%d\n"
                "\t\tSec_5=%d Sec_val_5=%d Qsize=%d Attr=0x%x Bits_used=%d\n"
                "\t\tNum_chunks=%d Num_Fields=%d Ibus_size=%d.\n"),
                unit, (*buses)->qual_list[qid]->qid,
                (*buses)->qual_list[qid]->e_params[0].section,
                (*buses)->qual_list[qid]->e_params[0].sec_val,
                (*buses)->qual_list[qid]->e_params[0].bus_offset,
                (*buses)->qual_list[qid]->e_params[0].width,
                (*buses)->qual_list[qid]->e_params[1].section,
                (*buses)->qual_list[qid]->e_params[1].sec_val,
                (*buses)->qual_list[qid]->e_params[1].bus_offset,
                (*buses)->qual_list[qid]->e_params[1].width,
                (*buses)->qual_list[qid]->e_params[2].section,
                (*buses)->qual_list[qid]->e_params[2].sec_val,
                (*buses)->qual_list[qid]->e_params[2].bus_offset,
                (*buses)->qual_list[qid]->e_params[2].width,
                (*buses)->qual_list[qid]->e_params[3].section,
                (*buses)->qual_list[qid]->e_params[3].sec_val,
                (*buses)->qual_list[qid]->e_params[4].section,
                (*buses)->qual_list[qid]->e_params[4].sec_val,
                (*buses)->qual_list[qid]->size,
                (*buses)->qual_list[qid]->attrib,
                (*buses)->qual_list[qid]->bits_used,
                (*buses)->qual_list[qid]->num_chunks,
                (*buses)->num_fields,
                (*buses)->size
                ));
        }  

        if (NULL == (*buses)->qual_list[qid]) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Unsupported in Ibus qual=%d.\n"),
                unit, f_qual_arr[idx]->qid));

            /* Pseudo qualifier do not need extraction, mark as not used. */
            (*buses)->qual_list[qid] = NULL;
        }
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_qual_sec_info_validate
 * Purpose:
 *     Validate the qualifier section info in case of qualifier 
 *     has multiple sections and secondary selectors.
 * Parameters:
 *     unit          - (IN)  BCM unit number.
 *     qual_sec_info - (IN/OUT)  Pointer to qualifier section info.
 *     ext_cfg       - (IN) Extractor configuration.
 *     fg            - (IN) Field Group OPerational Structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int 
_field_th_qual_sec_info_validate(int unit, 
                                 _field_qual_sec_info_t *qual_sec_info,
                                 _field_ext_cfg_t *ext_cfg, 
                                 _field_group_t *fg)
{
    int part; /* Extractor part number. */
    int level; /* Extractor level. */

    /* InPut Parameter Validation. */
    if (NULL == qual_sec_info || NULL == ext_cfg || NULL == fg) {
        return BCM_E_PARAM;
    }

    /* extarctor id is encoded as below in which  bit18 - bit27 are for "level",
     * bit28 - bit31 are for part number 
     */
    level = (ext_cfg->ext_id >> 18) & 0xf;
    part = (ext_cfg->ext_id >> 28) & 0x3;

    /* Validation is required at level 1. */
    if (1 != level) {
        return BCM_E_NONE;
    }

    /* Check qualifiers chunk will fit in this part of group or not. */
    switch (qual_sec_info->ctrl_sel) {
        case _FieldExtCtrlClassIdContASel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                fg->ext_codes[part].class_id_cont_a_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].class_id_cont_a_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlClassIdContBSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].class_id_cont_b_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].class_id_cont_b_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlClassIdContCSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                fg->ext_codes[part].class_id_cont_c_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].class_id_cont_c_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlClassIdContDSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].class_id_cont_d_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].class_id_cont_d_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlTtlFnSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].ttl_fn_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                 fg->ext_codes[part].ttl_fn_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlTosFnSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].tos_fn_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                 fg->ext_codes[part].tos_fn_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlTcpFnSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].tcp_fn_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                 fg->ext_codes[part].tcp_fn_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
       case _FieldExtCtrlSrcDestCont0Sel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                fg->ext_codes[part].src_dest_cont_0_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].src_dest_cont_0_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlSrcDestCont1Sel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].src_dest_cont_1_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].src_dest_cont_1_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
       case _FieldExtCtrlSrcContASel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                fg->ext_codes[part].src_cont_a_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].src_cont_a_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        case _FieldExtCtrlSrcContBSel:
            if ((_FP_EXT_SELCODE_DONT_CARE !=
                 fg->ext_codes[part].src_cont_b_sel) &&
                (qual_sec_info->ctrl_sel_val !=
                fg->ext_codes[part].src_cont_b_sel)) {
                return (BCM_E_RESOURCE);
            }
            break;
        default:
            break;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     _field_th_group_qual_extract
 * Purpose:
 *     Determine group qualifier's extractor hierarchy configuration.
 * Parameters:
 *     unit      - (IN)     BCM unit number.
 *     qual_sec  - (IN)     Qualifier Secction Info.
 *     ext_cfg   - (IN)     Extractor configuration for a given level.
 *     ace_qual  - (IN)     Pointer to group qualifiers list.
 *     iter      - (IN)     Qualifier extraction iterator instance value.
 *     ext_info  - (IN/OUT) Pointer to new extractor information structure.
 *     ff        - (IN/OUT) Matching field found status.
 *     chunk_idx - (IN/OUT) Qualifier chunk ID.
 *     inflation - (IN/OUT) Inflation status in bits.
 *     fg        - (in/OUT) Field group structure pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_qual_extract(int unit, _field_qual_sec_info_t **qual_sec,
                             _field_ext_cfg_t **ext_cfg,
                             _field_qual_sec_info_t **ace_qual, int iter,
                             int level, _field_ext_info_t **ext_info, int *ff,
                             int *chunk_idx, int *inflation,
                             _field_group_t *fg)
{
    int delta;                          /* Extracted bits count.              */
    int fill_bits = 0;                  /* Total extracted bits count.        */
    bcm_field_qualify_t qid;            /* Qualifier identifier.              */
    int p, l, g, e_num;                 /* part, level, gran & ext number.    */

    /* Input parameters check. */
    if ((NULL == qual_sec) || (NULL == ext_cfg) || (NULL == ace_qual)
        || (NULL == ext_info) || (NULL == ff) || (NULL == chunk_idx)
        || (NULL == inflation) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    /* Confirm field found has been cleared. */
    if (0 != *ff) {
        return (BCM_E_PARAM);
    }

    /* Skip the extractor section if it's already in use. */
    if ((*ext_cfg)->in_use) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "\tVerb: Skip Ext_Id=0x%x as it's In_Use=%d.\n"),
            (*ext_cfg)->ext_id, (*ext_cfg)->in_use));
        return (BCM_E_NONE);
    }

    /* Get Qualifier Identifier from ACE qual list. */
    qid = (*ace_qual)->qid;

    /*
     * We have extractor configuration (IN, OUT Sections), Qualifier
     * ID from ACE list and qualifier information [Section & Section Value]
     * from input bus.
     * Now check if qualifer that's being extracted has valid info in input
     * bus. If valid, confirm qualifier ID matches QID in input list and
     * Check current extractor IN section matches Qualifier section value
     *  in input bus.
     */

    /* Validate the Qualifier section information. */
    BCM_IF_ERROR_RETURN(_field_th_qual_sec_info_validate
                                            (unit, *qual_sec, *ext_cfg, fg));

    if ((NULL != *qual_sec)
        && (qid == (*qual_sec)->qid)
        && ((*qual_sec)->e_params[*chunk_idx].section
            == (*ext_cfg)->in_sec)) {

        /*
         * Verify if qualifer's bits used count is less than total size of
         * the qualifier in input bus. If its lesser, then proceed with
         * extraction of these bits.
         */
        if ((*qual_sec)->bits_used < (*qual_sec)->size) {
            /* Extract using selective extractors when 'iter == 0'. */
            if ((iter == 0) && ((*qual_sec)->size - (*qual_sec)->bits_used)
                 < (*ext_cfg)->gran) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Verb: Not in selective Diff=%d Gran=%d.\n"),
                    unit, ((*qual_sec)->size - (*qual_sec)->bits_used),
                    (*ext_cfg)->gran));
                return (BCM_E_NONE);
            }

            /* Check if qualifier falls in passthru section. */
            if ((*ext_cfg)->flags & _FP_EXT_ATTR_PASS_THRU) {
                /*
                 * All bits will pass through to next level no extraction
                 * required. Use entire qualifier width for fill bits
                 * calculation.
                 */
                delta = (*qual_sec)->e_params[*chunk_idx].width;
            } else {
                /* Extract size of "gran" no. of bits. */
                delta = (*ext_cfg)->gran;
            }

            /*
             * Get extractor section's current fill bits count.
             */
            fill_bits = (*ext_info)->sections[(*ext_cfg)->out_sec]->fill_bits;

            /*
             * Update fill bits count with new bits (delta) that's being
             * extracted.
             */
            fill_bits += delta;

            /*
             * Check if total fill bits count exceeds the current extractor's
             * drain bit capacity.
             */
            if (fill_bits >
                (*ext_info)->sections[(*ext_cfg)->out_sec]->drain_bits) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: No free bits in Extractor: Out_sec=%d"
                    " Drained_bits=%d.\n"), (*ext_cfg)->out_sec,
                    (*ext_info)->sections[(*ext_cfg)->out_sec]->drain_bits));

                /* Extractor does not have capacity, return resource error. */
                return (BCM_E_RESOURCE);
            }

            /*
             * Extractor has capacity. Update fill bits count in the selected
             * extractor section.
             */
            (*ext_info)->sections[(*ext_cfg)->out_sec]->fill_bits = fill_bits;

            /* Mark extractor in use. */
            (*ext_cfg)->in_use = 1;

            /* Set attached qualifier pointer for the extractor. */
            (*ext_cfg)->attached_qual = *qual_sec;

            /* Update section selector code value. */
            (*qual_sec)->e_params[*chunk_idx].sec_val
                = ((*qual_sec)->e_params[*chunk_idx].bus_offset
                    + (*qual_sec)->e_params[*chunk_idx].extracted)
                        / (*ext_cfg)->gran;

            /* Calculate inflation. */
            *inflation = ((*ext_cfg)->gran
                            - (*qual_sec)->e_params[*chunk_idx].width);

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "\tVerb: Qual_Extracted - [Level=%d] In_Sec=%d Chunk=%d\n"
                "\t\tsec=%d sec_val=%d width=%d extracted=%d b_off=%d\n"
                "\t\te_gran=%d\n"),
                level, (*ext_cfg)->in_sec, *chunk_idx,
                (*qual_sec)->e_params[*chunk_idx].section,
                (*qual_sec)->e_params[*chunk_idx].sec_val,
                (*qual_sec)->e_params[*chunk_idx].width,
                (*qual_sec)->e_params[*chunk_idx].extracted,
                (*qual_sec)->e_params[*chunk_idx].bus_offset,
                (*ext_cfg)->gran
                ));

            /*
             * Update section and section value for the extracted qualifier
             * chunk in ACE qual list.
             */
            (*ace_qual)->e_params[(*ace_qual)->num_chunks].section =
                (*qual_sec)->e_params[*chunk_idx].section;

            (*ace_qual)->e_params[(*ace_qual)->num_chunks].sec_val =
                (*qual_sec)->e_params[*chunk_idx].sec_val;

            /*
             * Set extractor section and it's select code value in group's
             * extractor codes array.
             */
            _FP_EXT_ID_PARSE((*ext_cfg)->ext_id, p, l, g, e_num);

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "\tVerb: Enum=%d Level=%d p=%d g=%d\n"), e_num, l, p, g));
            switch (g) {
                case 32:
                    if ((_FieldKeygenExtSelL2E16 == (*ext_cfg)->out_sec)
                        || (_FieldKeygenExtSelL2AE16 == (*ext_cfg)->out_sec)) {
                        fg->ext_codes[p].l1_e32_sel[e_num] =
                            (*qual_sec)->e_params[*chunk_idx].sec_val;
                    } else if (_FieldKeygenExtSelL2BE16
                                == (*ext_cfg)->out_sec) {
                        fg->ext_codes[p].l1_e32_sel[e_num] =
                            (*qual_sec)->e_params[*chunk_idx].sec_val;
                    } else if (_FieldKeygenExtSelL2CE16
                                == (*ext_cfg)->out_sec) {
                        fg->ext_codes[p].l1_e32_sel[e_num] =
                            (*qual_sec)->e_params[*chunk_idx].sec_val;
                    } else {
                        return (BCM_E_INTERNAL);
                    }
                    break;
                case 16:
                    if (l == 1) {
                        if ((_FieldKeygenExtSelL2E16 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL2AE16 == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l1_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2BE16 ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2CE16
                                    == (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    } else if (l == 2) {
                        if ((_FieldKeygenExtSelL3E16 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL3AE16 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL3E4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL3AE4 == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l2_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if ((_FieldKeygenExtSelL3BE16 ==
                                    (*ext_cfg)->out_sec)
                                    || (_FieldKeygenExtSelL3BE4 ==
                                        (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l2_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if ((_FieldKeygenExtSelL3CE16 ==
                                    (*ext_cfg)->out_sec) ||
                                    (_FieldKeygenExtSelL3CE4 ==
                                    (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l2_e16_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 8:
                    if (l == 1) {
                        if ((_FieldKeygenExtSelL2E4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL2AE4 == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l1_e8_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2BE4 ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e8_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2CE4
                                    == (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e8_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 4:
                    if (l == 1) {
                        if ((_FieldKeygenExtSelL2E4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL2AE4 == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l1_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2BE4 ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2CE4
                                    == (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    } else if (l == 3) {
                        if ((_FieldKeygenExtSelL4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL4A == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l3_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4B ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4C ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e4_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 2:
                    if (l == 1) {
                        if ((_FieldKeygenExtSelL2E4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL2AE4 == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l1_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2BE4 ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL2CE4
                                    == (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l1_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    } else if (l == 3) {
                        if ((_FieldKeygenExtSelL4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL4A == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l3_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4B ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4C ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e2_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 1:
                    if (l == 3) {
                        if ((_FieldKeygenExtSelL4 == (*ext_cfg)->out_sec) ||
                            (_FieldKeygenExtSelL4A == (*ext_cfg)->out_sec)) {
                            fg->ext_codes[p].l3_e1_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4B ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e1_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else if (_FieldKeygenExtSelL4C ==
                                    (*ext_cfg)->out_sec) {
                            fg->ext_codes[p].l3_e1_sel[e_num] =
                                (*qual_sec)->e_params[*chunk_idx].sec_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            /* Setup Pre-Mux extractor selectors. */
            if ((bcmFieldQualifySrcMac == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstMac == (*ext_cfg)->attached_qual->qid)) {
                fg->ext_codes[p].normalize_mac_sel =
                    BCM_FIELD_QSET_TEST(fg->qset,
                        bcmFieldQualifyNormalizeMacAddrs) ? 1 : 0;
            }
            if ((bcmFieldQualifySrcIp == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIpClass ==
                                         (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIpClassMsbNibble ==
                                         (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIpClassLower ==
                                         (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIpClassUpper ==
                                         (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIpClass ==
                                            (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIpClassMsbNibble ==
                                            (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIpClassLower ==
                                            (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIpClassUpper ==
                                            (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6 == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6High ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6Low ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6Class ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6ClassMsbNibble ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6ClassLower ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifySrcIp6ClassUpper ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6 == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6High ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6Low ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6Class ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6ClassMsbNibble ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6ClassLower ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyDstIp6ClassUpper ==
                                             (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyL4SrcPort == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyL4SrcPortClass ==
                                                (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyL4DstPort == (*ext_cfg)->attached_qual->qid)
                || (bcmFieldQualifyL4DstPortClass ==
                                                (*ext_cfg)->attached_qual->qid)
                ) {
                fg->ext_codes[p].normalize_l3_l4_sel =
                    BCM_FIELD_QSET_TEST(fg->qset,
                        bcmFieldQualifyNormalizeIpAddrs) ? 1 : 0;
            }
            switch (g) {
                case 32:
                    if ((l == 1) && (_FieldExtCtrlSelDisable
                        != (*ext_cfg)->attached_qual->ctrl_sel)) {
                        if (_FieldExtCtrlAuxTagASel ==
                            (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_a_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlAuxTagBSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_b_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 16:
                    if ((l == 1) && (_FieldExtCtrlSelDisable
                        != (*ext_cfg)->attached_qual->ctrl_sel)) {
                        if (_FieldExtCtrlClassIdContASel ==
                            (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].class_id_cont_a_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlClassIdContBSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].class_id_cont_b_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlClassIdContCSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].class_id_cont_c_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlClassIdContDSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].class_id_cont_d_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlSrcContBSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].src_cont_b_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlSrcContASel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].src_cont_a_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlSrcDestCont0Sel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].src_dest_cont_0_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlSrcDestCont1Sel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].src_dest_cont_1_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlAuxTagASel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_a_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlAuxTagBSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_b_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlAuxTagCSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_c_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlAuxTagDSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].aux_tag_d_sel =
                                        (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 8:
                    if ((l == 1) && (_FieldExtCtrlSelDisable
                        != (*ext_cfg)->attached_qual->ctrl_sel)) {
                        if (_FieldExtCtrlTosFnSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].tos_fn_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else if (_FieldExtCtrlTtlFnSel ==
                                    (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].ttl_fn_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                case 4:
                    if ((l == 1) && (_FieldExtCtrlSelDisable
                        != (*ext_cfg)->attached_qual->ctrl_sel)) {
                        if (_FieldExtCtrlTcpFnSel ==
                                (*ext_cfg)->attached_qual->ctrl_sel) {
                            fg->ext_codes[p].tcp_fn_sel =
                                (*ext_cfg)->attached_qual->ctrl_sel_val;
                        } else {
                            return (BCM_E_INTERNAL);
                        }
                    }
                    break;
                default:
                    ;
            }

            /*
             * Update number of chunks for the qualifier in current level in ACE
             * list.
             */
            (*ace_qual)->num_chunks += 1;

            /* Set field found flag. */
            *ff = 1;

            return (BCM_E_NONE);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_copy_extractors_list
 * Purpose:
 *     Create a copy of the extractor hierarchy configuration based on a
 *     selected extractor slice mode.
 * Parameters:
 *     unit     - (IN)      BCM unit number.
 *     stage_fc - (IN)      Stage Field control structure.
 *     emode    - (IN)      Extractor mode.
 *     ext_info - (IN/OUT)  Pointer to new extractor information structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_copy_extractors_list(int unit, _field_stage_t *stage_fc,
                                     _field_ext_conf_mode_t emode,
                                     _field_ext_info_t **ext_info)
{
    _field_ext_info_t *orig_ext_info = NULL; /* Extractor info original list. */
    int alloc_sz;                            /* Memory allocation size.       */
    int idx;                                 /* Iterator Index.               */
    int level;                               /* Extractor hierarcy level.     */
    int num_sec;                             /* Number of Sections.           */
    _field_keygen_ext_sel_t ext_sel;         /* Extractor selector type.      */
    _field_ext_cfg_t *ext_cfg;               /* Extractor configuration.      */
    _field_ext_sections_t *org_ext_secs;     /* Original extractor sections.  */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == ext_info)) {
        return (BCM_E_PARAM);
    }

    /* Get extractor information for the selected extractor mode. */
    orig_ext_info = stage_fc->ext_cfg_arr[emode];
    /* Confirm original extractor info list is initialized. */
    if (NULL == orig_ext_info) {
        return (BCM_E_INTERNAL);
    }

    /* Allocate extractor info memory. */
    _FP_XGS3_ALLOC(*ext_info, sizeof(_field_ext_info_t), "IFP ext info");
    if (NULL == *ext_info) {
        return (BCM_E_MEMORY);
    }

    /* Set the extractor mode. */
    (*ext_info)->mode = emode;

    /* Copy the extractor hierarchy section list for the selected mode. */
    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {
        alloc_sz = orig_ext_info->conf_size[level];

        /* Confirm extractor configuration has a valid size. */
        if (0 == alloc_sz) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Error null Ext Config List - Level=%d"
                " config_sz=%d.\n"), unit, level,
                orig_ext_info->conf_size[level]));
            sal_free(*ext_info);
            return (BCM_E_INTERNAL);
        }

        /* Allocate memory for extractor configurations in input bus list. */
        _FP_XGS3_ALLOC((*ext_info)->ext_cfg[level],
            (alloc_sz * sizeof(_field_ext_cfg_t)), "IFP ext config");
        if (NULL == (*ext_info)->ext_cfg[level]) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for extractor"
                " config.\n"), unit));
            for (idx = (level - 1); idx >= 0; idx--) {
                ext_cfg = (*ext_info)->ext_cfg[idx];
                if (NULL != ext_cfg) {
                    sal_free(ext_cfg);
                }
            }
            sal_free(*ext_info);
            *ext_info = NULL;
            return (BCM_E_MEMORY);
        }

        /* Copy extractor configuration for current level. */
        sal_memcpy((*ext_info)->ext_cfg[level], orig_ext_info->ext_cfg[level],
            alloc_sz * sizeof(_field_ext_cfg_t));

        /* Copy extractor configuration size information. */
        (*ext_info)->conf_size[level] = orig_ext_info->conf_size[level];
    }

    /* Copy extractor sections information for the selected HW slice mode. */
    if ((NULL != orig_ext_info->sections) && (orig_ext_info->num_sec > 0)) {

        /* Initialize number of sections information in new extractor info. */
        (*ext_info)->num_sec = orig_ext_info->num_sec;

        /* Allocate stage extractor sections pointers. */
        _FP_XGS3_ALLOC((*ext_info)->sections,
            (_FieldKeygenExtSelCount * sizeof(_field_ext_sections_t *)),
            "Field Ext sections");
        if (NULL == (*ext_info)->sections) {

            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for extractor"
                " sections pointer.\n"), unit));

            /* Free previously allocated extractor configuration memeory. */
            for (idx = 1; idx < _FP_EXT_LEVEL_COUNT; idx++) {
                ext_cfg = (*ext_info)->ext_cfg[idx];
                if (NULL != ext_cfg) {
                    sal_free(ext_cfg);
                }
            }

            /* Free extractor information pointer for the given mode. */
            sal_free(*ext_info);
            *ext_info = NULL;
            return (BCM_E_MEMORY);
        }

        /*
         * Get number of extractor sections initialized for current extractor
         * mode from the new extractor information structure.
         */
        num_sec = (*ext_info)->num_sec;

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Num_Sec=%d.\n"), unit, num_sec));

        /* Iterate over valid extractors and copy their configuration. */
        for (ext_sel = _FieldKeygenExtSelL1E32;
             ext_sel < _FieldKeygenExtSelCount; ext_sel++) {

            org_ext_secs = orig_ext_info->sections[ext_sel];
            if (NULL != org_ext_secs) {

                /* Allocate memory for extractor sections. */
                _FP_XGS3_ALLOC((*ext_info)->sections[ext_sel],
                    sizeof(_field_ext_sections_t), "IFP Ext section");
                if (NULL == (*ext_info)->sections[ext_sel]) {

                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: Allocation failure for extractor"
                        " section=%d.\n"), unit, ext_sel));

                    /* Free sections pointers list memory.*/
                    if (NULL != (*ext_info)->sections) {
                        sal_free((*ext_info)->sections);
                    }

                    /* Free extractors configuration memory. */
                    for (idx = 1; idx < _FP_EXT_LEVEL_COUNT; idx++) {
                        ext_cfg = (*ext_info)->ext_cfg[idx];
                        if (NULL != ext_cfg) {
                            sal_free(ext_cfg);
                        }
                    }

                    /* Free extractor information pointer for the given mode. */
                    sal_free(*ext_info);
                    *ext_info = NULL;
                    return (BCM_E_MEMORY);
                }

                /* Copy extractor information. */
                sal_memcpy((*ext_info)->sections[ext_sel], org_ext_secs,
                    sizeof(_field_ext_sections_t));

                /* Keep track of how many sections are yet to be copied. */
                num_sec--;
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Verb: Ext_sec=%d fill=%d drain=%d"
                    " Rem_Sec=%d.\n"), unit, ext_sel,
                    (*ext_info)->sections[ext_sel]->fill_bits,
                    (*ext_info)->sections[ext_sel]->drain_bits, num_sec));
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_new_ibus_create
 * Purpose:
 *     Create new input bus for a higher level section based on previous level's
 *     output sections of the extractor configurations.
 * Parameters:
 *     unit         - (IN)      BCM unit number.
 *     stage_fc     - (IN)      Stage Field control structure.
 *     ext_cfg      - (IN)      Pointer to a extractor configuration.
 *     buses        - (IN/OUT)  New input bus section pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_new_ibus_create(int unit, _field_stage_t *stage_fc,
                          _field_ext_info_t *ext_info,
                          _field_ext_cfg_t *ext_cfg, int level,
                          _field_bus_info_t **buses)
{
    int alloc_sz;               /* Memory allocation size.  */
    bcm_field_qualify_t qid;    /* Qualifier identifier.    */
    int chunk;               /* Qualifier chunk number.  */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == ext_cfg) || (NULL == buses)) {
        return (BCM_E_PARAM);
    }

    /* Confirm the extractor has a valid qualifier attached to it. */
    if (NULL == ext_cfg->attached_qual) {
        return (BCM_E_INTERNAL);
    }

    /*
     * Allocate memory for the new input bus and qualifiers pointer list first
     * time this function is calle.
     */
    if (NULL == *buses) {
        /* Allocate ACE list memory. */
        _FP_XGS3_ALLOC(*buses, sizeof(_field_bus_info_t), "IFP ibus info");
        if (NULL == *buses) {
            return (BCM_E_MEMORY);
        }

        alloc_sz = _bcmFieldQualifyCount * sizeof(_field_qual_sec_info_t *);
        /* Allocate memory for qualifiers in the ACE list. */
        _FP_XGS3_ALLOC((*buses)->qual_list, alloc_sz, "IFP ibus qual list");
        if (NULL == (*buses)->qual_list) {
            sal_free(*buses);
            return (BCM_E_MEMORY);
        }
    }


    /* Get the qualifier identifer. */
    qid = ext_cfg->attached_qual->qid;
    chunk = ext_cfg->chunk_extracted;
    /* Check if the qualifier has already been added to input bus. */
    if (NULL == (*buses)->qual_list[qid]) {
        _FP_XGS3_ALLOC((*buses)->qual_list[qid], sizeof(_field_qual_sec_info_t),
            "IFP ibus qual section");
        if (NULL == (*buses)->qual_list[qid]) {
            sal_free((*buses)->qual_list);
            sal_free(*buses);
            return (BCM_E_MEMORY);
        }

        (*buses)->qual_list[qid]->qid = ext_cfg->attached_qual->qid;
        (*buses)->qual_list[qid]->e_params[chunk].section = ext_cfg->out_sec;
        (*buses)->qual_list[qid]->size = ext_cfg->attached_qual->size;
        (*buses)->qual_list[qid]->attrib = ext_cfg->attached_qual->attrib;
        (*buses)->qual_list[qid]->bits_used = 0;
        (*buses)->qual_list[qid]->num_chunks += 1;
        (*buses)->qual_list[qid]->in_use = 1;
        (*buses)->size += ext_cfg->gran;
        (*buses)->num_fields += 1;
        (*buses)->qual_list[qid]->e_params[chunk].bus_offset = 
                                                            ext_cfg->bus_offset;
        /*
         * Check and update width and bus offset for L2 extractor passed
         * through bits.
         */
        if (ext_cfg->flags & _FP_EXT_ATTR_PASS_THRU) {
            (*buses)->qual_list[qid]->e_params[chunk].width =
                    ext_cfg->attached_qual->e_params[chunk].width;
            (*buses)->qual_list[qid]->e_params[chunk].bus_offset +=
                    ext_cfg->attached_qual->e_params[chunk].bus_offset;
        } else {
            (*buses)->qual_list[qid]->e_params[chunk].width = ext_cfg->gran;
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "\tVerb: [Level=%d] Chunk=0 Qid=%d qsize=%d Sec=%d\n"
            "\t\tsec_val=%d swidth=%d bus_offset=%d bsize=%d num_fld=%d\n"
            "\t\tnchk=%d.\n"),
            level,
            (*buses)->qual_list[qid]->qid,
            (*buses)->qual_list[qid]->size,
            (*buses)->qual_list[qid]->e_params[chunk].section,
            (*buses)->qual_list[qid]->e_params[chunk].sec_val,
            (*buses)->qual_list[qid]->e_params[chunk].width,
            (*buses)->qual_list[qid]->e_params[chunk].bus_offset,
            (*buses)->size,
            (*buses)->num_fields,
            (*buses)->qual_list[qid]->num_chunks));
    } else {
        (*buses)->qual_list[qid]->e_params[chunk].section = ext_cfg->out_sec;
        (*buses)->qual_list[qid]->num_chunks += 1;
        (*buses)->size += ext_cfg->gran;
        (*buses)->qual_list[qid]->e_params[chunk].bus_offset =
                                                        ext_cfg->bus_offset;
        /*
         * Check and update width and bus offset for L2 extractor passed
         * through bits.
         */
        if (ext_cfg->flags & _FP_EXT_ATTR_PASS_THRU) {
            (*buses)->qual_list[qid]->e_params[chunk].width =
                    ext_cfg->attached_qual->e_params[chunk].width;
            (*buses)->qual_list[qid]->e_params[chunk].bus_offset +=
                    ext_cfg->attached_qual->e_params[chunk].bus_offset;
        } else {
            (*buses)->qual_list[qid]->e_params[chunk].width = ext_cfg->gran;
        }
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "\tVerb: [Level=%d] Chunk=%d Qid=%d qsize=%d Sec=%d\n"
            "\t\tsec_val=%d swidth=%d bus_offset=%d bsize=%d num_fld=%d\n"
            "\t\tnchk=%d.\n"),
            level,
            chunk,
            (*buses)->qual_list[qid]->qid,
            (*buses)->qual_list[qid]->size,
            (*buses)->qual_list[qid]->e_params[chunk].section,
            (*buses)->qual_list[qid]->e_params[chunk].sec_val,
            (*buses)->qual_list[qid]->e_params[chunk].width,
            (*buses)->qual_list[qid]->e_params[chunk].bus_offset,
            (*buses)->size,
            (*buses)->num_fields,
            (*buses)->qual_list[qid]->num_chunks));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_qual_offsets_set
 * Purpose:
 *     Set group qualifiers offset values.
 * Parameters:
 *     unit         - (IN) BCM unit number.
 *     level        - (IN) Extractor level.
 *     f_qual_arr   - (IN) User supplied field qualifier array.
 *     qual_count   - (IN) Length of user supplied qualifier array.
 *     obus         - (IN) Level 3 Extractor output.
 *     fg           - (IN/OUT) Field group structure pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_qual_offsets_set(int unit, int level,
                                 _bcm_field_qual_info_t **f_qual_arr,
                                 uint16 qual_count, _field_bus_info_t **obus,
                                 _field_group_t *fg)
{
    int chunk_idx;                             /* Index iterator.             */
    int qual_idx;                              /* Qualifier index iterator.   */
    int parts_count = -1;                      /* Parts count.                */
    int part = 0;                              /* Part number.                */
    int arr_sz;                                /* Array size.                 */
    uint8 *size[_FP_MAX_ENTRY_WIDTH] = {NULL}; /* Entry part array size.      */
    uint16 *qid_arr;                           /* Qualifier ID array pointer. */
    _bcm_field_group_qual_t *q_arr;            /* Qualifier array pointer.    */
    _bcm_field_qual_offset_t *offset_arr;      /* Qual offset array pointer.  */
    _field_qual_sec_info_t *bus_qual;          /* Qual section info pointer.  */
    int idx;                                   /* Index iterator.             */
    uint8 *tmp;                                /* Temporary pointer variable. */
    uint16 qid;                                /* Qualifier ID.               */
    _bcm_field_qual_conf_t *conf = NULL;       /* Qualifier configuration.    */
    int arr_idx;                               /* Array index.                */
    int container_sel = 0;		       /* For selecting container_0/1 */
    int rv = 0;				       /* Operational Status	      */
    int count_postmux =0;		       /* To keep track of Postmuxes  */
    int src_dst_cont_flags = 0;		       /* Flags for src dst containers*/
    int is_postmux =0;			       /* Flag to determine postmux   */
    int dglp_p =0;			       /* Part info for DGLP postmux  */
    int dvplag_p =0;			       /* Part info for DVPLAG postmux*/
    int ecmp1_p =0;			       /* Part info for ECMP1 postmux */
    int next_hop_p =0;			       /* Part info for NxtHop postmux*/
    int ipmc_p =0;			       /* Part info for IPMC postmux  */
    int l2mc_p =0;			       /* Part info for L2MC postmux  */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == f_qual_arr) || (NULL == obus)) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        fg->flags, &parts_count));

    /* Allocate group slice parts qualifier array size pointers. */
    for (part = 0; part < parts_count; part++) {
        _FP_XGS3_ALLOC(size[part], sizeof(uint8) * qual_count, "q_arr size");
        if (NULL == size[part]) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Size var memory allocation failure.\n"),
                    unit));
            /* Deallocate memory already allocated. */
            for (idx = part - 1; idx >= 0; idx--) {
                tmp = size[idx];
                if (NULL != tmp) {
                    sal_free(tmp);
                    /* Mark unused. */
                    size[idx] = NULL;
                }
            }
            return (BCM_E_MEMORY);
        }
    }

    /*
     * Iterate over qualifiers list and get slice parts qualifier list size.
     */
    for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
        /* Get the qualifier ID. */
        qid = f_qual_arr[qual_idx]->qid;
	is_postmux = 1;

        /* Check and update size for Post Muxed qualifiers. */
	switch (qid) {
            case bcmFieldQualifyInPorts:
            case bcmFieldQualifyNatDstRealmId:
	    case bcmFieldQualifyNatNeeded:
	    case bcmFieldQualifyDrop:
        case _bcmFieldQualifyExactMatchHitStatusLookup0:
        case _bcmFieldQualifyExactMatchHitStatusLookup1:
        case _bcmFieldQualifyExactMatchActionClassIdLookup0:
        case _bcmFieldQualifyExactMatchActionClassIdLookup1:
		if (size[0][qual_idx] == 0) {
		    size[0][qual_idx] += 1;
		}
		break;
	/* Though HW allows SrcXXXGport/s to use SRC_DST_CONTAINERS as postmuxs */
	/* commented them to efficiently use the SRC_DST_CONT for DstXXXGPorts  */
	/* SrcXXXGport/s can use SRC_CONT_A/B available only for SrcXXXports    */
	#if 0
            case bcmFieldQualifySrcModPortGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldQualifySrcMimGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldQualifySrcNivGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldQualifySrcVxlanGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldQualifySrcMplsGport:
            /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldQualifySrcVlanGport:
            /* coverity[MISSING_BREAK: FALSE] */
	#endif
	/* update partition for Src_dst_container post muxes */
	    case bcmFieldQualifyDstTrunk:
	    case bcmFieldQualifyDstPort:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DGLP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DGLP);
		    dglp_p=count_postmux/2;
		    count_postmux++;
		}
		if (size[dglp_p][qual_idx] == 0) {
		    size[dglp_p][qual_idx] += 1;
		}
		break;
	    case bcmFieldQualifyDstMimGport:
	    case bcmFieldQualifyDstMimGports:
	    case bcmFieldQualifyDstNivGport:
	    case bcmFieldQualifyDstNivGports:
	    case bcmFieldQualifyDstVxlanGport:
	    case bcmFieldQualifyDstVxlanGports:
	    case bcmFieldQualifyDstVlanGport:
	    case bcmFieldQualifyDstVlanGports:
	    case bcmFieldQualifyDstMplsGport:
	    case bcmFieldQualifyDstMplsGports:
	    case bcmFieldQualifyDstWlanGport:
	    case bcmFieldQualifyDstWlanGports:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DVPLAG)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DVPLAG);
		    dvplag_p=count_postmux/2;
		    count_postmux++;
		}
		if (size[dvplag_p][qual_idx] == 0) {
		    size[dvplag_p][qual_idx] += 1;
		}
		break;
	    case bcmFieldQualifyDstMultipath:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_ECMP1)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_ECMP1);
		    ecmp1_p=count_postmux/2;
		    count_postmux++;
		}
		if (size[ecmp1_p][qual_idx] == 0) {
		    size[ecmp1_p][qual_idx] += 1;
		}
		break;
	    case bcmFieldQualifyDstL3Egress:
	    case bcmFieldQualifyDstL3EgressNextHops:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_NEXT_HOP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_NEXT_HOP);
		    next_hop_p =count_postmux/2;
		    count_postmux++;
		}
		if (size[next_hop_p][qual_idx] == 0) {
		    size[next_hop_p][qual_idx] += 1;
		}
		break;
	    case bcmFieldQualifyDstL2MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_L2MC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_L2MC_GROUP);
		    l2mc_p =count_postmux/2;
		    count_postmux++;
		}
		if (size[l2mc_p][qual_idx] == 0) {
		    size[l2mc_p][qual_idx] += 1;
		}
		break;
	    case bcmFieldQualifyDstL3MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_IPMC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_IPMC_GROUP);
		    ipmc_p =count_postmux/2;
		    count_postmux++;
		}
		if (size[ipmc_p][qual_idx] == 0) {
		    size[ipmc_p][qual_idx] += 1;
		}
		break;
	    default:
		is_postmux = 0;
	}
	if(is_postmux == 1) {
	    continue;
	}

        if (NULL == *obus) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: qid=%d.\n"), unit, qid));
            continue;
        }

        /* Get qualifier pointer in output bus. */
        bus_qual = (*obus)->qual_list[qid];
        if (NULL == bus_qual) {
            continue;
        }

        /* Iterate over qualifier chunks. */
        for (chunk_idx = 0; chunk_idx < bus_qual->num_chunks; chunk_idx++) {
            switch (bus_qual->e_params[chunk_idx].section) {
                case _FieldKeygenExtSelL4:
                    if (size[0][qual_idx] == 0) {
                        size[0][qual_idx] += 1;
                    }
                    break;
                case _FieldKeygenExtSelL4A:
                    if (size[0][qual_idx] == 0) {
                        size[0][qual_idx] += 1;
                    }
                    break;
                case _FieldKeygenExtSelL4B:
                    if (size[1][qual_idx] == 0) {
                        size[1][qual_idx] += 1;
                    }
                    break;
                case _FieldKeygenExtSelL4C:
                    if (size[2][qual_idx] == 0) {
                        size[2][qual_idx] += 1;
                    }
                    break;
                default:
                    /* Invalid L3 extractor section. */
                    return (BCM_E_INTERNAL);
            }
        }
    }


    /* Iterate over entry parts. */
    for (part = 0; part < parts_count; part++) {
        offset_arr = NULL;
        qid_arr = NULL;

        /* Get qualifiers array pointer. */
        q_arr = &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part]);

        /* Initialize array size. */
        arr_sz = 0;

        /* Calculate array size for group slice's part. */
        for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
            arr_sz += size[part][qual_idx];
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Part=%d Arr_sz=%d.\n"), unit, part, arr_sz));

        if (arr_sz > 0) {
            /* Allocate Qualifier ID array memory. */
            _FP_XGS3_ALLOC(qid_arr, (arr_sz * sizeof(uint16)), "Group qual id");
            if (NULL == qid_arr) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: Group Qual ID memory allocation"
                    " failure.\n"), unit));
                /* Deallocate memory allocated to store array size. */
                for (idx = 0; idx < parts_count; idx++) {
                    tmp = size[idx];
                    if (NULL != tmp) {
                        sal_free(tmp);
                        /* Mark unused. */
                        size[idx] = NULL;
                    }
                }
                return (BCM_E_MEMORY);
            }

            /* Allocate qualifier offset array memory. */
            _FP_XGS3_ALLOC(offset_arr,
                (arr_sz * sizeof(_bcm_field_qual_offset_t)),
                 "Group qual offset");
            if (NULL == offset_arr) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: Group qual offsets memory allocation"
                    " failure.\n"), unit));

                /* Deallocate qualifier identifier memory. */
                sal_free(qid_arr);

                /* Deallocate memory allocated to store array size. */
                for (idx = 0; idx < parts_count; idx++) {
                    tmp = size[idx];
                    if (NULL != tmp) {
                        sal_free(tmp);
                        /* Mark unused. */
                        size[idx] = NULL;
                    }
                }
                return (BCM_E_MEMORY);
            }
        }

        /* Initialize qualifier entry part pointers. */
        q_arr->qid_arr = qid_arr;
        q_arr->offset_arr = offset_arr;

        /* Initialize array size. */
        q_arr->size = arr_sz;

        if (q_arr->size > 0) {
            arr_idx = 0;
            for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {

                /* Skip qualifier if it's not in current part. */
                if (0 == size[part][qual_idx]) {
                    continue;
                }

                qid = f_qual_arr[qual_idx]->qid;

                /* Check and setup offsets for post muxed qualifiers. */
                if ((qid == bcmFieldQualifyInPorts)
                    || (qid == bcmFieldQualifyNatDstRealmId)
                    || (qid == bcmFieldQualifyNatNeeded)
                    || (qid == bcmFieldQualifyDrop)
		/* Though HW allows SrcXXXGport/s to use SRC_DST_CONTAINERS as postmuxs */
		/* commented them to efficiently use the SRC_DST_CONT for DstXXXGPorts  */
		/* SrcXXXGport/s can use SRC_CONT_A/B available only for SrcXXXports    */
/*                    || (qid == bcmFieldQualifySrcModPortGport)
                    || (qid == bcmFieldQualifySrcMimGport)
                    || (qid == bcmFieldQualifySrcNivGport)
                    || (qid == bcmFieldQualifySrcVxlanGport)
                    || (qid == bcmFieldQualifySrcMplsGport)
                    || (qid == bcmFieldQualifySrcVlanGport)*/
                    || (qid == bcmFieldQualifyDstTrunk)
                    || (qid == bcmFieldQualifyDstMultipath)
                    || (qid == bcmFieldQualifyDstL2MulticastGroup)
                    || (qid == bcmFieldQualifyDstL3MulticastGroup)
                    || (qid == bcmFieldQualifyDstL3EgressNextHops)
                    || (qid == bcmFieldQualifyDstL3Egress)
                    || (qid == bcmFieldQualifyDstPort)
                    || (qid == bcmFieldQualifyDstMimGport)
                    || (qid == bcmFieldQualifyDstNivGport)
                    || (qid == bcmFieldQualifyDstVxlanGport)
                    || (qid == bcmFieldQualifyDstVlanGport)
                    || (qid == bcmFieldQualifyDstMplsGport)
                    || (qid == bcmFieldQualifyDstWlanGport)
                    || (qid == bcmFieldQualifyDstMimGports)
                    || (qid == bcmFieldQualifyDstNivGports)
                    || (qid == bcmFieldQualifyDstVxlanGports)
                    || (qid == bcmFieldQualifyDstVlanGports)
                    || (qid == bcmFieldQualifyDstMplsGports)
                    || (qid == bcmFieldQualifyDstWlanGports)
                    || (qid == _bcmFieldQualifyExactMatchHitStatusLookup0)
                    || (qid == _bcmFieldQualifyExactMatchHitStatusLookup1)
                    || (qid == _bcmFieldQualifyExactMatchActionClassIdLookup0)
                    || (qid == _bcmFieldQualifyExactMatchActionClassIdLookup1)) {

                    /* Initialize post mux qualifier details. */
                    q_arr->qid_arr[arr_idx] = (uint16) qid;
                    q_arr->offset_arr[arr_idx].field = KEYf;

                    /* Get qualifier configuration pointer. */
                    conf = f_qual_arr[qual_idx]->conf_arr;
                    q_arr->offset_arr[arr_idx].num_offsets =
                                            conf->offset.num_offsets;
                    LOG_INFO(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: qid=%d num_off=%d off0=%d off1=%d.\n"),
                         unit, qid,
                         q_arr->offset_arr[arr_idx].num_offsets,
                         conf->offset.offset[0],
                         conf->offset.offset[1]));
                    for (idx = 0;
                         idx < q_arr->offset_arr[arr_idx].num_offsets;
                         idx++) {
                        q_arr->offset_arr[arr_idx].offset[idx] =
                                                conf->offset.offset[idx];
                        q_arr->offset_arr[arr_idx].width[idx] =
                                                conf->offset.width[idx];
                    }
                    arr_idx++;
                    continue;
                }

                if (NULL == *obus) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: qid=%d.\n"), unit, qid));
                    continue;
                }

                bus_qual = (*obus)->qual_list[qid];
                /* Confirm bus qualifier information is valid. */
                if (NULL == bus_qual) {
                    continue;
                }

                /* Validate the Max supported qual offsets */
                if (bus_qual->num_chunks >= _BCM_FIELD_QUAL_OFFSET_MAX) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: Update Max Offsets for Qid=%d "
                        "to %d.\n"), unit, bus_qual->qid, bus_qual->num_chunks
                        ));

                    /* Free qualifier identifier memory. */
                    sal_free(qid_arr);

                    /* Free offset array memory. */
                    sal_free(offset_arr);

                    /* Mark qualifier array and offset array as unused. */
                    q_arr->qid_arr = NULL;
                    q_arr->offset_arr = NULL;
                    q_arr->size = 0;

                    /* Free memory allocated for group part sizes. */
                    for (part = 0; part < parts_count; part++) {
                        if (NULL != size[part]) {
                            sal_free(size[part]);
                        }
                    }
                    return (BCM_E_INTERNAL);
                }

                /* Initialize Qualifier ID value. */
                q_arr->qid_arr[arr_idx] = (uint16) bus_qual->qid;

                /* Initialize qualifier HW field info. */
                q_arr->offset_arr[arr_idx].field = KEYf;

                /* Increment qualifier chunks/offsets count. */
                q_arr->offset_arr[arr_idx].num_offsets =
                                                bus_qual->num_chunks;

                for (chunk_idx = 0; chunk_idx < bus_qual->num_chunks;
                     chunk_idx++) {
                    /*
                     * Skip chunks that are not relevant to the qualifier
                     * part.
                     */
                    if (((part == 0)
                        && ((_FieldKeygenExtSelL4
                            != bus_qual->e_params[chunk_idx].section)
                        && (_FieldKeygenExtSelL4A !=
                            bus_qual->e_params[chunk_idx].section)))
                        || ((part == 1) && (_FieldKeygenExtSelL4B
                            != bus_qual->e_params[chunk_idx].section))
                        || ((part == 2) && (_FieldKeygenExtSelL4C
                            != bus_qual->e_params[chunk_idx].section))) {
                        continue;
                    }

                    /* Set qualifier chunk offset position in the final key. */
                    q_arr->offset_arr[arr_idx].offset[chunk_idx] =
                                    bus_qual->e_params[chunk_idx].bus_offset;
                    q_arr->offset_arr[arr_idx].width[chunk_idx] =
                                    bus_qual->e_params[chunk_idx].width;

                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Qid[Chk_No=%d]=%d Qid_Arr_Idx=%d Offset=%d"
                        " Width=%d.\n"),
                        chunk_idx, q_arr->qid_arr[arr_idx], arr_idx,
                        q_arr->offset_arr[arr_idx].offset[chunk_idx],
                        q_arr->offset_arr[arr_idx].width[chunk_idx]));
                }

                /* Increment array index/size. */
                arr_idx++;
            }
        }
    }

    /*
     * Free memory allocated for group's slice parts qualifier array size
     * pointers.
     */
    for (part = 0; part < parts_count; part++) {
        tmp = size[part];
        if (NULL != tmp) {
            sal_free(tmp);
            size[part] = NULL;
        }
    }

    /* Set up post mux extractor selector values. */
    for (part = 0; part < parts_count; part++) {
        /* Get qualifiers array pointer. */
        q_arr = &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part]);
        for (qual_idx = 0; qual_idx < q_arr->size; qual_idx++) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Qid=%d Part=%d\n"),
                unit, q_arr->qid_arr[qual_idx], part));
            switch (q_arr->qid_arr[qual_idx]) {
                case bcmFieldQualifyInPorts:
                    fg->ext_codes[part].ipbm_present = 1;
                    break;
                case bcmFieldQualifyNatDstRealmId:
                    fg->ext_codes[part].pmux_sel[2] = 1;
                    break;
                case bcmFieldQualifyNatNeeded:
                    fg->ext_codes[part].pmux_sel[1] = 1;
                    break;
                case bcmFieldQualifyDrop:
                    fg->ext_codes[part].pmux_sel[0] = 1;
                    break;
                case _bcmFieldQualifyExactMatchHitStatusLookup0:
                case _bcmFieldQualifyExactMatchHitStatusLookup1:
                    fg->ext_codes[part].pmux_sel[3] = 1;
                    break;
                case _bcmFieldQualifyExactMatchActionClassIdLookup0:
                    fg->ext_codes[part].pmux_sel[8] = 1;
                    fg->ext_codes[part].pmux_sel[9] = 1;
                    fg->ext_codes[part].pmux_sel[10] = 1;
                    break;
                case _bcmFieldQualifyExactMatchActionClassIdLookup1:
                    fg->ext_codes[part].pmux_sel[5] = 1;
                    fg->ext_codes[part].pmux_sel[6] = 1;
                    fg->ext_codes[part].pmux_sel[7] = 1;
                    break;
		/* Though HW allows SrcXXXGport/s to use SRC_DST_CONTAINERS as postmuxs */
		/* commented them to efficiently use the SRC_DST_CONT for DstXXXGPorts  */
		/* SrcXXXGport/s can use SRC_CONT_A/B available only for SrcXXXports    */
		#if 0
                case bcmFieldQualifySrcModPortGport:
                    fg->ext_codes[part].pmux_sel[11] = 1;
                    fg->ext_codes[part].pmux_sel[4] = 1;
                    fg->ext_codes[part].src_dest_cont_1_sel = 1;
                    break;
                case bcmFieldQualifySrcMimGport:
                /* coverity[MISSING_BREAK: FALSE] */
                case bcmFieldQualifySrcNivGport:
                /* coverity[MISSING_BREAK: FALSE] */
                case bcmFieldQualifySrcVxlanGport:
                /* coverity[MISSING_BREAK: FALSE] */
                case bcmFieldQualifySrcMplsGport:
                /* coverity[MISSING_BREAK: FALSE] */
                case bcmFieldQualifySrcVlanGport:
                    fg->ext_codes[part].pmux_sel[11] = 1;
                    fg->ext_codes[part].pmux_sel[4] = 1;
                    fg->ext_codes[part].src_dest_cont_1_sel = 2;
                    break;
		#endif
		case bcmFieldQualifyDstTrunk:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstPort:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_DGLP)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_DGLP);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 4);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		case bcmFieldQualifyDstMimGport:
		case bcmFieldQualifyDstMimGports:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstNivGport:
		case bcmFieldQualifyDstNivGports:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstVxlanGport:
		case bcmFieldQualifyDstVxlanGports:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstVlanGport:
		case bcmFieldQualifyDstVlanGports:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstMplsGport:
		case bcmFieldQualifyDstMplsGports:
		case bcmFieldQualifyDstWlanGport:
		case bcmFieldQualifyDstWlanGports:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_DVPLAG)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_DVPLAG);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 5);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		case bcmFieldQualifyDstMultipath:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_ECMP1)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_ECMP1);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 7);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		case bcmFieldQualifyDstL2MulticastGroup:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_L2MC_GROUP)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_L2MC_GROUP);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 11);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		case bcmFieldQualifyDstL3MulticastGroup:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_IPMC_GROUP)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_IPMC_GROUP);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 10);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		case bcmFieldQualifyDstL3EgressNextHops:
		    /* coverity[MISSING_BREAK: FALSE] */
		case bcmFieldQualifyDstL3Egress:
		    if ((src_dst_cont_flags & _FP_SRC_DST_CONT_NEXT_HOP)) {
			src_dst_cont_flags &= ~(_FP_SRC_DST_CONT_NEXT_HOP);
			rv = _field_th_update_ext_codes_src_dst_container_sel(unit, fg, part, container_sel, 9);
			if (!BCM_FAILURE(rv)) {
			    container_sel++;
			}
		    }
		    break;
		default:
		    ;
	    }
            for (chunk_idx = 0;
                 chunk_idx < q_arr->offset_arr[qual_idx].num_offsets;
                 chunk_idx++) {

                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: Offset=%d Width=%d\n"),
                    q_arr->offset_arr[qual_idx].offset[chunk_idx],
                    q_arr->offset_arr[qual_idx].width[chunk_idx]));
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_extractors_list_free
 * Purpose:
 *     Group extractors list free function.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     ext_info     - (IN/OUT) Field extractors information structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_extractors_list_free(int unit, _field_ext_info_t **ext_info)
{
    int ext;                            /* Extractor sections iterator. */
    int level;                          /* Extractor level iterator.    */
    _field_ext_cfg_t *ext_cfg;          /* Extractor configuration.     */
    _field_ext_sections_t *section;     /* Extractor section pointer.   */

    /* Input parameters check. */
    if (NULL == ext_info) {
        return (BCM_E_PARAM);
    }

    /* Nothing to do, return success. */
    if (NULL == *ext_info) {
        return (BCM_E_NONE);
    }

    /* Free extractor sections memory. */
    for (ext = _FieldKeygenExtSelL1E32; ext < _FieldKeygenExtSelCount; ext++) {
        section = (*ext_info)->sections[ext];
        if (NULL != section) {
            sal_free(section);
            section = NULL;
        }
    }

    /* Free sections pointers memory. */
    if (NULL != (*ext_info)->sections) {
        sal_free((*ext_info)->sections);
        (*ext_info)->sections = NULL;
    }

    /* Free extractors configuration memory for all levels. */
    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {
        ext_cfg = (*ext_info)->ext_cfg[level];
        if (NULL != ext_cfg) {
            sal_free(ext_cfg);
            ext_cfg = NULL;
        }
    }

    /* Free extractors information memory allocated for the group. */
    sal_free(*ext_info);

    /* Mark extractor information pointer as unused. */
    *ext_info = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_ace_list_free
 * Purpose:
 *     Field group access control list free function.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     qual_count   - (IN)     Qualifiers count.
 *     ace_list     - (IN/OUT) Field group access control list structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_ace_list_free(int unit, uint16 qual_count,
                              _field_ace_info_t **ace_list)
{
    uint16 qual;                        /* Qualifier sections iterator. */
    _field_qual_sec_info_t *qual_sec;   /* Qualifier section.           */

    /* Input parameter check. */
    if (NULL == ace_list) {
        return (BCM_E_PARAM);
    }

    /* Nothing to do, then return success. */
    if (NULL == *ace_list) {
        return (BCM_E_NONE);
    }

    /* Free qualifiers extractor section info memory. */
    for (qual = 0; qual < qual_count; qual++) {
        qual_sec = (*ace_list)->qual_list[qual];
        if (NULL != qual_sec) {
            sal_free(qual_sec);
        }
    }

    /* Free qualifiers extractor section info pointers memory. */
    if (NULL != (*ace_list)->qual_list) {
        sal_free((*ace_list)->qual_list);
    }

    /* Free list memory. */
    if (NULL != *ace_list) {
        sal_free(*ace_list);
        *ace_list = NULL;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_ibus_list_free
 * Purpose:
 *     Field group qualifiers input bus list free function.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     f_qual_arr   - (IN)     Group qualifiers information structure.
 *     qual_count   - (IN)     Qualifiers count.
 *     bus          - (IN/OUT) Input bus information structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_ibus_list_free(int unit, _bcm_field_qual_info_t **f_qual_arr,
                               uint16 qual_count, _field_bus_info_t **bus)
{
    _field_qual_sec_info_t *qual_sec;   /* Qualifier section information.   */
    _field_qual_sec_info_t *qual;       /* Qualifier section information.   */
    int idx;                            /* Index iterator.                  */
    bcm_field_qualify_t qid;            /* Qualifier identifier.            */

    /* Input parameter check. */
    if ((NULL == bus) || (NULL == f_qual_arr)) {
        return (BCM_E_PARAM);
    }

    /* Nothing to do, return success. */
    if (NULL == *bus) {
        return (BCM_E_NONE);
    }

    /* Free bus qualifiers sections memory. */
    for (idx = 0; idx < qual_count; idx++) {
        qid = f_qual_arr[idx]->qid;
        qual_sec = (*bus)->qual_list[qid];
        while (NULL != qual_sec) {
            qual = qual_sec;
            qual_sec = qual_sec->next;
            sal_free(qual);
        }
    }

    /* Free bus qualifiers sections pointers memory. */
    if (NULL != (*bus)->qual_list) {
        sal_free((*bus)->qual_list);
    }

    /* Free bus instance memory. */
    if (NULL != *bus) {
        sal_free(*bus);
        *bus = NULL;
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_level1_bus_length_validate
 * Purpose:
 *     Validate the Level1 bus length.
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     ext_cfg  - (IN) Extractor configuration for a given level.
 *     ext_info - (IN) Pointer to new extractor information structure.
 *     fg       - (IN) Field group structure pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_level1_bus_length_validate(int unit,
                                     _field_ext_cfg_t *ext_cfg,
                                     _field_ext_info_t *ext_info,
                                     _field_group_t *fg)
{
    _field_keygen_ext_sel_t s16; /* Extractor selector type. */
    _field_keygen_ext_sel_t s4;  /* Extractor selector type. */


    /* Parameters Check */
    if (NULL == ext_cfg || NULL == ext_info || NULL == fg) {  
        return BCM_E_PARAM;
    }

  
 
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
        !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {

        /* If group is in Single Wide Mode. */

        s16 = _FieldKeygenExtSelL2E16;
        s4 = _FieldKeygenExtSelL2E4;

        if ((ext_info->sections[s16]->fill_bits +
             ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 80) {
             return BCM_E_FULL;
        }

    } else if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ||
               (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ||
               (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {

        /* If group is in IntrasliceDouble/Double/Triple Wide mode. */

        if (_FieldKeygenExtSelL2E16 == ext_cfg->out_sec ||
            _FieldKeygenExtSelL2E4 == ext_cfg->out_sec) {
             
            /* Level1 bus section is in part0 (only part exist in this mode) and
             * group mode is IntrasliceDouble Wide mode.
             */

            s16 = _FieldKeygenExtSelL2E16;
            s4 = _FieldKeygenExtSelL2E4;

            if (ext_info->sections[s16]->fill_bits > 64 &&
                !BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
                if (((ext_info->sections[s16]->fill_bits - 64) + 
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }   
            } else if(ext_info->sections[s16]->fill_bits > 32 &&
                      BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
                if (((ext_info->sections[s16]->fill_bits - 32) + 
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                } 
            } else if (s4 == ext_cfg->out_sec) {
                if ((ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }  
            }

        } else if (_FieldKeygenExtSelL2AE16 == ext_cfg->out_sec ||
                   _FieldKeygenExtSelL2AE4 == ext_cfg->out_sec) {

            /* Level1 bus section is in part0 and group mode is 
             * Double/Triple Wide mode.
             */
            s16 = _FieldKeygenExtSelL2AE16;
            s4 = _FieldKeygenExtSelL2AE4;

            if (ext_info->sections[s16]->fill_bits > 64 &&
                !BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
                if (((ext_info->sections[s16]->fill_bits - 64) +
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            } else if(ext_info->sections[s16]->fill_bits > 32 &&
                      BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
                if (((ext_info->sections[s16]->fill_bits - 32) +
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            } else if (s4 == ext_cfg->out_sec) {
                if ((ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            }

        } else if (_FieldKeygenExtSelL2BE16 == ext_cfg->out_sec ||
                   _FieldKeygenExtSelL2BE4 == ext_cfg->out_sec) {

            /* Level1 bus section is in part1 and group mode is 
             * Double/Triple Wide mode.
             */

            s16 = _FieldKeygenExtSelL2BE16;
            s4 = _FieldKeygenExtSelL2BE4;

            if (ext_info->sections[s16]->fill_bits > 64) {
                if (((ext_info->sections[s16]->fill_bits - 64) +
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            } else if (s4 == ext_cfg->out_sec) {
                if ((ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            }

        } else if (_FieldKeygenExtSelL2CE16 == ext_cfg->out_sec ||
                    _FieldKeygenExtSelL2E4 == ext_cfg->out_sec) {

            /* Level1 bus section is in part2 and group mode is Triple */

            s16 = _FieldKeygenExtSelL2CE16;
            s4 = _FieldKeygenExtSelL2CE4;

            if (ext_info->sections[s16]->fill_bits > 64) {
                if (((ext_info->sections[s16]->fill_bits - 64) +
                    ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            } else if (s4 == ext_cfg->out_sec) {
                if ((ext_info->sections[s4]->fill_bits + ext_cfg->gran) > 96) {
                    return BCM_E_FULL;
                }
            }
        }
    } else {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_qual_extractor_release
 * Purpose:
 *     Clear the extractors allocated for a given qualifier.
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     qual     - (IN) Field Qualifier.
 *     level    - (IN) Extractor Level.
 *     ext_info - (IN/OUT) extractor info of all levels.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_th_qual_extractor_release(int unit, bcm_field_qualify_t qual,
                                     int level, _field_ext_info_t **ext_info) {

    int ext_idx;                      /* Extractor index.         */
    _field_ext_cfg_t *ext_cfg = NULL; /* Extractor configuration. */

    /* Parameters Check. */  
    if (NULL == ext_info) {
        return BCM_E_PARAM;
    }

    /* Go throup all extarctors of a given level. */
    for (ext_idx = 0; ext_idx < (*ext_info)->conf_size[level]; ext_idx++) {
        /* Get the extractor configuration. */      
        ext_cfg = (*ext_info)->ext_cfg[level] + ext_idx;
        if (NULL!= ext_cfg->attached_qual) {
            if (qual == ext_cfg->attached_qual->qid && ext_cfg->in_use) {
                ext_cfg->in_use = 0;
                ext_cfg->attached_qual = NULL;
                ext_cfg->chunk_extracted = -1;
            }
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_th_group_process_qset
 * Purpose:
 *     Process user supplied QSET and determine the extractor selector values.
 * Parameters:
 *     unit         - (IN) BCM unit number.
 *     stage_fc     - (IN) Stage Field control structure.
 *     fg           - (IN) Field group structure pointer.
 *     f_qual_arr   - (IN) User supplied field qualifier array.
 *     qual_count   - (IN) Length of user supplied qualifier array.
 *     key_size     - (IN) Key size of the supplied QSET.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_process_qset(int unit, _field_stage_t *stage_fc,
                             _field_group_t *fg,
                             _bcm_field_qual_info_t **f_qual_arr,
                             uint16 qual_count, uint16 key_size)
{
    _field_ace_info_t *ace_list[_FP_EXT_LEVEL_COUNT] = {0}; /* Access list.   */
    _field_bus_info_t *buses[_FP_EXT_LEVEL_COUNT] = {0}; /* Input buses.      */
    _field_bus_info_t *pbus;                    /* Input bus pointer.         */
    _field_ext_info_t *ext_info = NULL;         /* Extractor info structure.  */
    _field_qual_sec_info_t *ibus_qual = NULL;   /* Input bus section info.    */
    _field_qual_sec_info_t *ace_qual = NULL;    /* ACL section information.   */
    _field_ext_cfg_t *ext_cfg = NULL;           /* Extractor configuration.   */
    _field_ext_conf_mode_t emode;               /* Extractor mode.            */
    uint32 l1_p0_size = 0;                      /* Level1 Part 0 size.        */
    uint32 l1_p1_size = 0;                      /* Level1 Part 1 size.        */
    uint32 l1_p2_size = 0;                      /* Level1 Part 2 size.        */
    uint8 duplicate = 0;                        /* Duplicate field.           */
    int level;                                  /* Extractor hierarchy level. */
    int idx;                                    /* Index iterator.            */
    int ff;                                     /* Field found.               */
    int sec_idx;                                /* Section iterator.          */
    int inflation = 0;                          /* Inflation in bits.         */
    int size;                                   /* Size in bits.              */
    int rv;                                     /* Operation return status.   */
    int chunk_idx;                              /* Qualifier Chunk index.     */
    int p, l, g, e_num;                 /* part, level, gran & ext number.    */
    int iter = 0;                               /* Extraction iterations.     */
    int chunk_extracted = -1;           
 
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == f_qual_arr)) {
        return (BCM_E_PARAM);
    }

    /* Get extractor mode based on the group flags value. */
    BCM_IF_ERROR_RETURN(_field_th_group_flags_to_ext_mode(unit, fg->flags,
        &emode));

    /* Validate group mode against the actual keysize. */
    if (((emode == _FieldExtConfMode160Bits) && (key_size > 160))
        || ((emode == _FieldExtConfMode320Bits) && (key_size > 320))
        || ((emode == _FieldExtConfMode480Bits) && (key_size > 480))) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Invalid Extractor Mode=%d.\n"),
            unit, emode));
        return (BCM_E_CONFIG);
    }

    /* Validate Group qset and aset here for exact match group 
     * Group flag can be single/double for EM, check if group
     * qset and aset are possible in flag, if false return error 
     * Also update em_mode in group here.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN(
                _field_th_em_validate_view(unit, key_size, fg));
    }

    /*
     * When key size is greater than 80 bits, group must be created in Intra
     * Slice double wide mode.
     * Applicable only for stage ingress.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        if ((key_size > 80)
             && ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
             && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: Upgrade group to "
                        "Intra slice Gid=%d.\n"),
                        unit, fg->gid));
            return (BCM_E_CONFIG);
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) Verb: Extractor mode=%d.\n"), unit, emode));

    /* Create the extractors list for the selected extractor mode. */
    BCM_IF_ERROR_RETURN(_field_th_group_copy_extractors_list(unit, stage_fc,
        emode, &ext_info));

    /* Create Group QSET list for supported extractor levels. */
    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {
        rv = _field_th_group_create_ace_list(unit, stage_fc, fg,
                f_qual_arr, qual_count, key_size, level, &ace_list[level]);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_group_create_ace_list=%d.\n"),
                unit, rv));
            goto cleanup;
        }
    }

    rv = _field_th_update_offset_src_dst_container_sel(unit, fg, qual_count,
                                f_qual_arr);
    /* Build Level 0 input bus for qualifiers in Group's QSET. */
    rv = _field_th_group_ibus_copy_create(unit, stage_fc, fg, f_qual_arr,
            qual_count, key_size, &buses[0]);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_group_ibus_copy_create=%d.\n"),
            unit, rv));
        goto cleanup;
    }

    rv =  _field_th_group_sort_ace_list(unit, qual_count, &buses[0], &ace_list[1]);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < qual_count; idx++) {
        
        /* Get the Level 1 qualifier information. */
        ace_qual = ace_list[1]->qual_list[idx];
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Start Level=%d extraction.size %d\n"), unit, level,ace_qual->size));
        /* Skip post muxed qualifiers. */
        if ((bcmFieldQualifyInPorts == ace_qual->qid)
            || (bcmFieldQualifyNatDstRealmId == ace_qual->qid)
            || (bcmFieldQualifyNatNeeded == ace_qual->qid)
            || (bcmFieldQualifyDrop == ace_qual->qid)
            || ((int)_bcmFieldQualifyExactMatchHitStatusLookup0 == (int)ace_qual->qid)
            || ((int)_bcmFieldQualifyExactMatchHitStatusLookup1 == (int)ace_qual->qid)
            || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup0 == (int)ace_qual->qid)
            || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup1 == (int)ace_qual->qid)) {
            /* All post muxer qualifiers are initialized in partition zero and 
             * will be pushed to fixed offsets in the final key.So these 
             * qualifiers will be skipped during group create. So total size of 
             * these post mux qualifiers needs to consider while assigning extarctors 
             * in level1 partition zero.
             */
		l1_p0_size += ace_qual->size;
	}
    }
    /* check the ace_list flags for SRC_DST_CONT and */
    /* update the size of partition accordingly */
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_0) {
	l1_p0_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_1) {
	l1_p0_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_0_SLI_B) {
	l1_p1_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_1_SLI_B) {
	l1_p1_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_0_SLI_C) {
	l1_p2_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }
    if(ace_list[1]->flags & _FP_POST_MUX_SRC_DST_CONT_1_SLI_C) {
	l1_p2_size += _FP_SRC_DST_CONT_QUAL_SIZE;
    }

    /* Build extractor hierarchy for the group's qset per extraction level. */
    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Start Level=%d extraction.\n"), unit, level));

        /* Iterate over list of qualifiers that need extraction. */
        for (idx = 0; idx < qual_count; idx++) {

              

            /* Get the qualifier information. */
            ace_qual = ace_list[level]->qual_list[idx];

            /* Skip post muxed qualifiers. */
            if ((bcmFieldQualifyInPorts == ace_qual->qid)
                || (bcmFieldQualifyNatDstRealmId == ace_qual->qid)
                || (bcmFieldQualifyNatNeeded == ace_qual->qid)
                || (bcmFieldQualifyDrop == ace_qual->qid)
	    /* Though HW allows SrcXXXGport/s to use SRC_DST_CONTAINERS as postmuxs */
	    /* commented them to efficiently use the SRC_DST_CONT for DstXXXGPorts  */
	    /* SrcXXXGport/s can use SRC_CONT_A/B available only for SrcXXXports    */
/*                || (bcmFieldQualifySrcModPortGport == ace_qual->qid)
                || (bcmFieldQualifySrcMimGport == ace_qual->qid)
                || (bcmFieldQualifySrcNivGport == ace_qual->qid)
                || (bcmFieldQualifySrcVxlanGport == ace_qual->qid)
                || (bcmFieldQualifySrcMplsGport == ace_qual->qid)
                || (bcmFieldQualifySrcVlanGport == ace_qual->qid) */
                || (bcmFieldQualifyDstTrunk == ace_qual->qid)
                || (bcmFieldQualifyDstMultipath == ace_qual->qid)
                || (bcmFieldQualifyDstL2MulticastGroup == ace_qual->qid)
                || (bcmFieldQualifyDstL3MulticastGroup == ace_qual->qid)
                || (bcmFieldQualifyDstL3EgressNextHops == ace_qual->qid)
                || (bcmFieldQualifyDstL3Egress == ace_qual->qid)
                || (bcmFieldQualifyDstPort == ace_qual->qid)
                || (bcmFieldQualifyDstMimGport == ace_qual->qid)
                || (bcmFieldQualifyDstNivGport == ace_qual->qid)
                || (bcmFieldQualifyDstVxlanGport == ace_qual->qid)
                || (bcmFieldQualifyDstVlanGport == ace_qual->qid)
                || (bcmFieldQualifyDstMplsGport == ace_qual->qid)
                || (bcmFieldQualifyDstWlanGport == ace_qual->qid)
                || (bcmFieldQualifyDstMimGports == ace_qual->qid)
                || (bcmFieldQualifyDstNivGports == ace_qual->qid)
                || (bcmFieldQualifyDstVxlanGports == ace_qual->qid)
                || (bcmFieldQualifyDstVlanGports == ace_qual->qid)
                || (bcmFieldQualifyDstMplsGports == ace_qual->qid)
                || (bcmFieldQualifyDstWlanGports == ace_qual->qid)
                || ((int)_bcmFieldQualifyExactMatchHitStatusLookup0 == (int)ace_qual->qid)
                || ((int)_bcmFieldQualifyExactMatchHitStatusLookup1 == (int)ace_qual->qid)
                || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup0 == (int)ace_qual->qid)
                || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup1 == (int)ace_qual->qid)) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: Skip Post Muxed Qid[Level=%d]=%d.\n"),
                    level, ace_qual->qid));
                continue;
            }

            /*
             * Skip extraction for pseudo qualifiers as these are not actual
             * fields in hardware.
             */
            if ((bcmFieldQualifyStage == ace_qual->qid)
                || (bcmFieldQualifyStageIngress == ace_qual->qid)
                || (bcmFieldQualifyStageIngressExactMatch == ace_qual->qid)
                || (bcmFieldQualifyIp4 == ace_qual->qid)
                || (bcmFieldQualifyIp6 == ace_qual->qid)
                || (bcmFieldQualifyExactMatchHitStatus == ace_qual->qid)
                || (bcmFieldQualifyExactMatchGroupClassId == ace_qual->qid)
                || (bcmFieldQualifyExactMatchActionClassId == ace_qual->qid)) {
                continue;
            }

            /*
             * If qualifier bits used has a valid value then this qualifier has
             * been extracted, skip it.
             */
            if (ace_qual->bits_used > 0) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: Skip Qid[Level=%d]=%d Bits_used([=%d] > 0).\n"),
                     level, ace_qual->qid, ace_qual->bits_used));
                continue;
            }

            /* Clear duplicate and fields found settings. */
            duplicate = 0;
            ff = 0;

            /* Initialize the chunk id. */
            chunk_idx = 0;

            /* Get qualifier information from input bus. */
            ibus_qual = (buses[level - 1])->qual_list[ace_qual->qid];
            if (NULL == ibus_qual) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: Pseudo Qualifier - Qid=%d Level=%d.\n"),
                    ace_qual->qid, level));
                continue;
            }


            for (ibus_qual = (buses[level - 1])->qual_list[ace_qual->qid]; 
                 NULL != ibus_qual; ibus_qual = ibus_qual->next) {
                /*
                 * Check if the qualifier has already been extracted and mark it
                 * as duplicate if all bits have been extracted.
                 */
                if ((ibus_qual->qid == ace_qual->qid)
                    && (ibus_qual->bits_used >= ibus_qual->size)) {
                    /* Set duplicate status. */
                    duplicate = 1;

                    /* Update duplicate status in qualifier attributes. */
                    if (0 == (ace_qual->attrib & _FP_EXT_ATTR_DUPLICATE)) {
                        ace_qual->attrib |= _FP_EXT_ATTR_DUPLICATE;
                    }

                    /* Update extracted bits count in the qualifier. */
                    ace_qual->bits_used = ibus_qual->bits_used;
                }

                /*
                 * If duplicate, move to next qualifer to be extracted in the
                 * ACL list.
                 */

                if (duplicate) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Duplicate Qid=%d.\n"), ibus_qual->qid));
                    continue;
                }

                chunk_extracted = 0;
                for (iter = 0; iter < 2; iter++) {
                /* Iterate over extractors available at a given level. */
                for (sec_idx = 0; 
                     sec_idx < ext_info->conf_size[level]; sec_idx++) {

                    /* Get the extractor configuration. */
                    ext_cfg = ext_info->ext_cfg[level] + sec_idx;

                    _FP_EXT_ID_PARSE(ext_cfg->ext_id, p, l, g, e_num);

                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Next_Extractor - [Level=%d P=%d] G=%d"
                        " E_No=%d\n \t\tIn_Sec=%d Out_Sec=%d flags=0x%x"
                        " In_Use=%d\n"),
                        l, p, g, e_num, ext_cfg->in_sec, ext_cfg->out_sec,
                        ext_cfg->flags, ext_cfg->in_use));

                    /*
                     * Check if current extractor is available in 80bit mode.
                     * If ACL key size is <= 80 and this extractor is not
                     * available for this width, then skip it.
                     */
                    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) { 
                        if ((ace_list[level]->size <= 80)
                             && (ext_cfg->flags & _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT)) {
                              LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                         "\tVerb: ACE Size=%d Sec=%d Not in 80b Key\n"),
                                         ace_list[level]->size, sec_idx));
                            continue;
                        }
                    }

                    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                        if (fg->em_mode == _FieldExactMatchMode128) {
                            if (ext_cfg->flags & _FP_EXT_ATTR_NOT_WITH_EM_MODE_128) {
                                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "\tVerb: ACE Size=%d Sec=%d Not in 128 Mode View\n"),
                                     ace_list[level]->size, sec_idx));
                                continue;
                            }
                        }
                    }

                    /*
                     * Check and skip the extractor if InPorts qualifier is
                     * present in QSET and this extractor is not available for
                     * this configuration.
                     */
                    if ((ace_list[level]->flags & _FP_POST_MUX_IPBM)
                         && (ext_cfg->flags & _FP_EXT_ATTR_NOT_WITH_IPBM)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "\tVerb: ACE Skip Sec=%d due to IPBM in Key.\n"),
                            sec_idx));
                        continue;
                    }

                    /* Check and skip unavailable extractors. */
                    if (((ace_list[level]->flags
                        & _FP_POST_MUX_NAT_DST_REALM_ID) && (ext_cfg->flags
                        & _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID))
                        || ((ace_list[level]->flags & _FP_POST_MUX_NAT_NEEDED)
                            && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED))
                        || ((ace_list[level]->flags 
                                & _FP_POST_MUX_EM_HIT_STATUS_LOOKUP_0)
                            && (ext_cfg->flags
                                & _FP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0))
                        || ((ace_list[level]->flags 
                                & _FP_POST_MUX_EM_HIT_STATUS_LOOKUP_1)
                            && (ext_cfg->flags
                                & _FP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1))
                        || ((ace_list[level]->flags 
                                & _FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_0)
                            && (ext_cfg->flags
                                & _FP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_0))
                        || ((ace_list[level]->flags 
                                & _FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_1)
                            && (ext_cfg->flags
                                & _FP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_1))
                        || ((ace_list[level]->flags & _FP_POST_MUX_DROP)
                            && (ext_cfg->flags & _FP_EXT_ATTR_NOT_WITH_DROP))
			            || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_1_SLI_B) && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B))
                        || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_0_SLI_B)
                            && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B))
			            || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_1_SLI_C) && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C))
                        || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_0_SLI_C)
                            && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C))
			            || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_1) && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1))
                        || ((ace_list[level]->flags
                            & _FP_POST_MUX_SRC_DST_CONT_0)
                            && (ext_cfg->flags
                            & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0))) {
			continue;
		    }
		    /*check and skip src_dst_container_0/1 for 80-bit configuration*/
		    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
			!(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
		    {
			/*80 bit mode*/
			if (((ace_list[level]->flags
                                & _FP_POST_MUX_SRC_DST_CONT_1) && (ext_cfg->flags
                                & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_80))
                            || ((ace_list[level]->flags
				& _FP_POST_MUX_SRC_DST_CONT_0) && (ext_cfg->flags
				& _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_80))) {
			continue;
                        }
		    }

                    if (1 == level) {
                        rv = _field_th_level1_bus_length_validate(unit, ext_cfg,
                                                                  ext_info, fg);
                        if (BCM_E_FULL == rv) {
                            continue;
                        } 

                        BCM_IF_ERROR_RETURN(rv);
                    }
 
                    /*
                     * Check if all bits have been extracted for current
                     * qualifier. If yes, move on to next qualifier in the
                     * Group's QSET list.
                     */
                    if (ace_qual->bits_used >= ace_qual->size) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "\tVerb: ACE bits_used=%d >= size=%d\n"),
                            ace_qual->bits_used, ace_qual->size));
                        break;
                    }

                    /*
                     * Check if total size extarcted at level1 partition 0 is
                     * greater than the max key size.All post muxer qualifiers
                     * are initialized in partition 0 and need to consider the
                     * size of these qualifiers while assigning extarctors at
                     * level1 partition 0.
                     */
                    if ((160 < (l1_p0_size + ext_cfg->gran))
                        && 1 == l && 0 == p) {
                        continue;
                    }
                    if ((160 < (l1_p1_size + ext_cfg->gran))
                        && 1 == l && 1 == p) {
                        continue;
                    }
                    if ((160 < (l1_p2_size + ext_cfg->gran))
                        && 1 == l && 2 == p) {
                        continue;
                    }

                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Attempt to extract now...Qid=%d bits_used=%d"
                        " size=%d n_chunks=%d.\n"),               
                        ace_qual->qid, ace_qual->bits_used, ace_qual->size,
                        ace_qual->num_chunks));

                    /*
                     * Extract qualifier bits using selected extractor
                     * section.
                     */
                    rv = _field_th_group_qual_extract(unit, &ibus_qual,
                            &ext_cfg, &ace_qual, iter, level, &ext_info, &ff,
                            &chunk_idx, &inflation, fg);
                    if (BCM_FAILURE(rv) && (BCM_E_RESOURCE != rv)) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                            "FP(unit %d) Error: "
                            "_field_th_group_qual_extract=%d.\n"), unit, rv));
                        goto cleanup;
                    }

                    /* Matching extractor found for the qualifier. */
                    if (1 == ff) {
                        /* Update size extracted at partition zero level1 */
                        if (1 == l && 0 == p) {
                            l1_p0_size += ext_cfg->gran;
                        }

                        /* Update size extracted at partition 1 level1 */
                        if (1 == l && 1 == p) {
                            l1_p1_size += ext_cfg->gran;
                        }

                        /* Update size extracted at partition 2 level1 */
                        if (1 == l && 2 == p) {
                            l1_p2_size += ext_cfg->gran;
                        }
                        /*
                         * Determine number of bits extracted by the matched
                         * extractor.
                         */
                        if (ext_cfg->flags & _FP_EXT_ATTR_PASS_THRU) {
                            size =
                              ext_cfg->attached_qual->e_params[chunk_idx].width;
                            inflation = 0;
                        } else {
                            /*
                             * Use the minimum size among the qualifier size in
                             * input bus, extractor granularity and qualifier
                             * size in stage ingress QSET.
                             */
                            if ((ext_cfg->attached_qual->size < ext_cfg->gran)
                                && (ext_cfg->attached_qual->size
                                    < ace_qual->size)) {
                                size = ext_cfg->attached_qual->size;
                            } else if ((ext_cfg->gran
                                < ext_cfg->attached_qual->size)
                                && ext_cfg->gran < ace_qual->size) {
                                size = ext_cfg->gran;
                            } else {
                                size = ace_qual->size;
                            }

                            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "\tVerb: Qid=%d Inflation=%d.\n"),
                                ace_qual->qid, inflation));
                        }

                        /* Update bits used count for the qualifier. */
                        ace_qual->bits_used += size;
                        
                        /* Update extracted bits count in the attached 
                         * qualifier. */
                        ext_cfg->attached_qual->bits_used += size;

                        ext_cfg->attached_qual->e_params[chunk_idx].extracted
                              += size;

                        if (ext_cfg->attached_qual->e_params[chunk_idx].extracted
                            == ext_cfg->attached_qual->e_params[chunk_idx].width) {
                            chunk_idx += 1;
                        }
                        ff = 0;
                        ext_cfg->chunk_extracted = chunk_extracted;
                        chunk_extracted++;
                    }
                }
                }
                /*
                 * Check if all bits have been extracted for current qualifier.
                 * If yes, move on to next qualifier in the Group's QSET list.
                 */
                if (ace_qual->bits_used >= ace_qual->size) {

                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: ACE bits_used=%d >= size=%d\n"),
                        unit, ace_qual->bits_used, ace_qual->size));
                    break;

                } else {

                    ace_qual->bits_used = 0;
                    chunk_idx = 0;
                    ff = 0;
                    duplicate = 0;

                    rv = _bcm_field_th_qual_extractor_release(unit, 
                                                              ace_qual->qid, 
                                                              level, &ext_info);
                    BCM_IF_ERROR_RETURN(rv);

                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: Try Next Configuration of qid=%d\n"),
                           unit, ace_qual->qid));
                }
                }
                /*
                * If bits used is not equal to qualifier size, there are more
                * bits to extract and the qualifier is not fully extracted.
                * Continue extraction.
                */
                if (ace_qual->bits_used < ace_qual->size) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Not fully extracted Qid=%d.\n"),
                        ace_qual->qid));
                    rv = BCM_E_RESOURCE;
                    goto cleanup;
                }

                /* If qualifier size is zero then it's an error. */
                if (0 == ace_qual->size) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Extractor for Qid=%d Not Found.\n"),
                        ace_qual->qid));
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }

                /*
                 * If bits used is equal to qualifier size, then all bits have
                 * been extracted.
                 */
                if (ace_qual->bits_used == ace_qual->size) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "\tVerb: Fully extracted Qid=%d bits_used=%d"
                        " size=%d.\n\n"), ace_qual->qid, ace_qual->bits_used,
                        ace_qual->size));
                }
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Now construct output bus.\n"), unit));

        /* Now create the output bus, which is input bus for next level. */
        for (sec_idx = 0; sec_idx < ext_info->conf_size[level]; sec_idx++) {
            /*
             * Iterate over available extractors at this level and check which
             * extractors have valid qualifiers attached to them.
             */
            ext_cfg = ext_info->ext_cfg[level] + sec_idx;
            if (NULL != ext_cfg->attached_qual) {

                /*
                 * Create new input bus for next level using extractors
                 * Out section information in the current level.
                 */
                rv = _field_th_new_ibus_create(unit, stage_fc, ext_info,
                        ext_cfg, level, &buses[level]);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: _field_th_new_ibus_create=%d.\n"),
                        unit, rv));
                    goto cleanup;
                }
            }
        }
    }

    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {
        for (idx = 0; idx < qual_count; idx++) {
            /* Get the qualifier information. */
            ace_qual = ace_list[level]->qual_list[idx];
            for (chunk_idx = 0; chunk_idx < ace_qual->num_chunks; chunk_idx++) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\tVerb: LEvel %d Qid=%d Sec=%d Sec_val=%d bus_offset=%d width=%d.\n"),
			level,
                    ace_qual->qid,
                    ace_qual->e_params[chunk_idx].section,
                    ace_qual->e_params[chunk_idx].sec_val,
                    ace_qual->e_params[chunk_idx].bus_offset,
                    ace_qual->e_params[chunk_idx].width));
            }
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,              
        "FP(unit %d) Verb: Final output bus - Level 4.\n"), unit));

    for (idx = 0; idx < qual_count; idx++) {
        pbus = buses[3];
        if (NULL != pbus) {
            ibus_qual = buses[3]->qual_list[f_qual_arr[idx]->qid];
            if (NULL == ibus_qual) {
                continue;
            }

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "\tVerb: Qid=%d Size=%d No.Chunks=%d\n"),
                ibus_qual->qid,
                ibus_qual->size,
                ibus_qual->num_chunks));

            for (chunk_idx = 0; chunk_idx < ibus_qual->num_chunks;
                 chunk_idx++) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "\t\tVerb: Chunk_No=%d Bus_offset=%d Width=%d Sec=%d.\n"),
                     chunk_idx,
                     ibus_qual->e_params[chunk_idx].bus_offset,
                     ibus_qual->e_params[chunk_idx].width,
                     ibus_qual->e_params[chunk_idx].section));
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        /* Set qualifier offsets in group qual array. */
        rv = _field_th_group_qual_offsets_set(unit, level, f_qual_arr, qual_count,
                &buses[3], fg);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_group_qual_offsets_set=%d.\n"),
                unit, rv));
        }
    }

cleanup:

    /* Free bus list memory. */
    for (level = 0; level < _FP_EXT_LEVEL_COUNT; level++) {
        _field_th_group_ibus_list_free(unit, f_qual_arr, qual_count,
            &buses[level]);
    }

    /* Free access control lists memory. */
    for (level = 1; level < _FP_EXT_LEVEL_COUNT; level++) {
        _field_th_group_ace_list_free(unit, qual_count, &ace_list[level]);
    }

    /* Free memory allocated for extractors information. */
    _field_th_group_extractors_list_free(unit, &ext_info);

    return (rv);
}

/*
 * Function:
 *     _field_th_group_update_keysize
 * Purpose:
 *     Calcualte the real key size of the user supplied QSET based on width of
 *     the qualifiers in the input bus.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     stage_fc     - (IN)     Stage Field control structure.
 *     f_qual_arr   - (IN)     Pointer to user supplied field qualifier array.
 *     qual_count   - (IN)     Qualifier array length.
 *     rkey_size    - (IN/OUT) Real key size of the supplied QSET.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_update_keysize(int unit, _field_stage_t *stage_fc,
                               _bcm_field_qual_info_t **f_qual_arr,
                               uint16 qual_count, uint16 *rkey_size)
{
    _field_qual_sec_info_t **qual_list; /* Qualifiers list. */
    int idx;                            /* Index iterator.  */
    bcm_field_qualify_t qid;            /* Qualifier ID.    */
    static char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
    int src_dst_cont_flags = 0;		/* Src Dst Container Flags */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_qual_arr) || (NULL == rkey_size)) {
        return (BCM_E_PARAM);
    }

    qual_list = stage_fc->input_bus.qual_list;
    /* Confirm input bus qualifiers array list is initialized. */
    if (NULL == qual_list) {
        return (BCM_E_INTERNAL);
    }

    /* Iterate over list of qualifiers */
    for (idx = 0; idx < qual_count; idx++) {
        /* Get the Qualifier ID. */
        qid = f_qual_arr[idx]->qid;

        /*
         * Skip pseudo qualifiers that are not supported in input bus.
         *  - bcmFieldQualifyStage
         *  - bcmFieldQualifyStageIngress
         */
	if (NULL != qual_list[qid]) {
	    /* Get the real key size of this qualifier. */
	    switch(qid) {
		case bcmFieldQualifyDstTrunk:
		case bcmFieldQualifyDstPort:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DGLP)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_DGLP);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		case bcmFieldQualifyDstMimGport:
		case bcmFieldQualifyDstMimGports:
		case bcmFieldQualifyDstNivGport:
		case bcmFieldQualifyDstNivGports:
		case bcmFieldQualifyDstVxlanGport:
		case bcmFieldQualifyDstVxlanGports:
		case bcmFieldQualifyDstVlanGport:
		case bcmFieldQualifyDstVlanGports:
		case bcmFieldQualifyDstMplsGport:
		case bcmFieldQualifyDstMplsGports:
		case bcmFieldQualifyDstWlanGport:
		case bcmFieldQualifyDstWlanGports:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DVPLAG)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_DVPLAG);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		case bcmFieldQualifyDstMultipath:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_ECMP1)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_ECMP1);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		case bcmFieldQualifyDstL3Egress:
		case bcmFieldQualifyDstL3EgressNextHops:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_NEXT_HOP)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_NEXT_HOP);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		case bcmFieldQualifyDstL2MulticastGroup:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_L2MC_GROUP)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_L2MC_GROUP);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		case bcmFieldQualifyDstL3MulticastGroup:
		    if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_IPMC_GROUP)) {
			src_dst_cont_flags |= (_FP_SRC_DST_CONT_IPMC_GROUP);
			*rkey_size += qual_list[qid]->size;
		    }
		    break;
		default:
		    *rkey_size += qual_list[qid]->size;
		    break;
	    }
	    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
			    "FP(unit %d) Verb: qid=%d size=%d rkey_size=%d\n"),
			unit, qid, qual_list[qid]->size, *rkey_size));
	} else {
	    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
			    "FP(unit %d) Verb: Skip pseudo qualifier=%s\n"),
			unit, qual_text[qid]));
	}
    }

    /* For group creation, a valid QSET must be supplied on TH. */
    if (0 == *rkey_size) {
        return (BCM_E_CONFIG);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_ext_code_get
 * Purpose:
 *     Finds extractor select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN)     BCM unit number.
 *     stage_fc  - (IN)     Stage Field control structure.
 *     qset_req  - (IN)     Client qualifier set.
 *     fg        - (IN/OUT) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_ext_code_get(int unit, _field_stage_t *stage_fc,
                               bcm_field_qset_t *qset_req, _field_group_t *fg)
{
    int rv = BCM_E_NONE;                    /* Return status.           */
    _bcm_field_qual_info_t **f_qual_arr;    /* Qualifiers information.  */
    uint16 qual_count;                      /* Qualifier count.         */
    uint16 rkey_size = 0;                   /* ACL real key size.       */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == qset_req || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Initialize group extractor codes based on group flags. */
    rv = _field_th_group_extractors_init(unit, fg);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Get requested qualifiers information from IFP qualifiers
     * list - (Width of the qualifier).
     */
    f_qual_arr = NULL;
    rv = _bcm_field_qualifiers_info_get(unit, stage_fc, qset_req,
                &f_qual_arr, &qual_count);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Calcuate key size for supplied QSET based on the qualifiers width in the
     * input bus.
     */
    rv = _field_th_group_update_keysize(unit, stage_fc, f_qual_arr, qual_count,
            &rkey_size);
    if (BCM_FAILURE(rv)) {
        sal_free(f_qual_arr);
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: QSET keysize calculation.\n"), unit));
        return (rv);
    }

    /*
     * Process group's QSET and determine the extractor select codes for
     * logical table's slices.
     */
    rv = _field_th_group_process_qset(unit, stage_fc, fg, f_qual_arr,
            qual_count, rkey_size);
    if (BCM_FAILURE(rv)) {
        sal_free(f_qual_arr);
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Processing group QSET (rkey_size=%d).\n"),
            unit, rkey_size));
        return (rv);
    }

    /* Deallocate qualifiers information array memory. */
    sal_free(f_qual_arr);
    return (rv);
}

/*
 * Function:
 *     _field_th_ext_code_assign
 *
 * Purpose:
 *     Calculate the extractor select codes from a qualifier set and group mode.
 *
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     qset           - (IN) Client qualifier set.
 *     selcode_clear  - (IN) Clear the selcodes
 *     fg             - (IN/OUT) Group info structure.
 *
 * Returns:
 *     BCM_E_PARAM    - mode unknown
 *     BCM_E_RESOURCE - No select code will satisfy qualifier set
 *     BCM_E_NONE     - Success
 *
 * Notes:
 *     Calling function is responsible for ensuring appropriate slices
 *     are available.
 *     selcode_clear will be 0 (don't clear) when this function is called
 *         from bcm_field_group_set
 */
STATIC int
_field_th_ext_code_assign(int unit,
                          bcm_field_qset_t qset,
                          int selcode_clear,
                          _field_group_t *fg)
{
    _field_control_t *fc;       /* Field control structure.         */
    _field_stage_t *stage_fc;   /* Stage Field control structure.   */
    int idx;                    /* slice index iterator.            */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure for this device. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /*
     * Clear group extractor selector codes.
     */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        if (selcode_clear) {
            _FP_EXT_SELCODE_CLEAR(fg->ext_codes[idx]);
        }
    }

    /* Get keygen extractor codes for group qset. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_ext_code_get(unit, stage_fc, &qset,
        fg));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_extractor_codes_get
 * Purpose:
 *     Calculate the extractor select codes from a qualifier set and group mode.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_extractor_codes_get(int unit,
                                        _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg;     /* Field Group pointer.     */
    int orig_group_flags;   /* Original Group flags.    */

    /* Input parameters check. */
    if (NULL == fsm_ptr || NULL == fsm_ptr->fg) {
        return (BCM_E_PARAM);
    }

    /* Check if requested mode is supported by device. */
    switch (fsm_ptr->mode) {
        case bcmFieldGroupModeQuad:
        case bcmFieldGroupModeDirect:
        case bcmFieldGroupModeDirectExtraction:
        case bcmFieldGroupModeHashing:
        case bcmFieldGroupModeExactMatch:
            /* Mode not supported by device. */
            fsm_ptr->rv = (BCM_E_RESOURCE);
            break;
        case bcmFieldGroupModeTriple: /* 480 bit mode. */
            /* Check Triple wide mode group support in device feature list. */
            if (0 == soc_feature(unit, soc_feature_field_ingress_triple_wide)) {
                fsm_ptr->rv = (BCM_E_RESOURCE);
            } else {
                if (fsm_ptr->stage_fc->stage_id
                                            == _BCM_FIELD_STAGE_EXACTMATCH) {
                    fsm_ptr->rv = (BCM_E_CONFIG);
                }
            }
            break;
        case bcmFieldGroupModeDouble: /* 320 bit mode. */
            /* Check if device supports Double wide mode. */
            if (0 == soc_feature(unit, soc_feature_field_wide)) {
                fsm_ptr->rv = (BCM_E_RESOURCE);
            }
            break;
        case bcmFieldGroupModeSingle: /* 80 bit mode. */
            if (BCM_FIELD_QSET_TEST(fsm_ptr->qset, bcmFieldQualifyInPorts)) {
                fsm_ptr->rv = (BCM_E_CONFIG);
            }
            break;
        default:
            break;
    }

    /* If mode check returned failure, proceed to cleanup state. */
    if (BCM_FAILURE(fsm_ptr->rv)) {
        fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return (_field_th_group_add(unit, fsm_ptr));
    }

    /* Group pointer initialization. */
    fg = fsm_ptr->fg;

    fsm_ptr->rv = BCM_E_RESOURCE;
    orig_group_flags = fg->flags;

    if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {

        if (bcmFieldGroupModeAuto == fsm_ptr->mode) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group mode auto.\n")));

            /* For Auto mode, try Single -> Double -> Triple modes. */
            /* Single wide Non-Intra slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY) &&
                    !BCM_FIELD_QSET_TEST(fsm_ptr->qset, bcmFieldQualifyInPorts)) {
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Trying single...\n")));
                /* 80bit mode. */
                fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE);
                fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
            }

            /* Single wide intra slice selection. */
            if ((BCM_FAILURE(fsm_ptr->rv)
                        && (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE))) {
                if (soc_feature(unit, soc_feature_field_intraslice_double_wide)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying intra...\n")));
                    /* 160bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                            | _FP_GROUP_INTRASLICE_DOUBLEWIDE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
                }
            }

            /* Double wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying double...\n")));
                    /* 320bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_DOUBLE_SLICE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
                }
            }

            /* Triple wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying triple...\n")));
                    /* 480bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_TRIPLE_SLICE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
                }
            }
        } else {
            switch (fsm_ptr->mode) {
                case bcmFieldGroupModeSingle:
                    /* Single wide non intra slice selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP,
                                (BSL_META_U(unit, "Trying single...\n")));
                        fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                fg);
                    }
                    break;
                case bcmFieldGroupModeDouble:
                    /* Double wide intra slice selection. */
                    if ((soc_feature(unit, soc_feature_field_intraslice_double_wide)
                                && (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE))) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying intra...\n")));
                        fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                | _FP_GROUP_INTRASLICE_DOUBLEWIDE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                fg);
                    }
                    /* Double wide inter slice selection. */
                    if ((BCM_FAILURE(fsm_ptr->rv))
                            && (0 == (fsm_ptr->flags
                                    & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY))) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying double...\n")));
                        fg->flags = (orig_group_flags |
                                _FP_GROUP_SPAN_DOUBLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                fg);
                    }
                    break;
                case bcmFieldGroupModeTriple:
                    /* Triple wide inter slice selection. */
                    if (0 == (fsm_ptr->flags
                                & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying triple...\n")));
                        fg->flags = (orig_group_flags |
                                _FP_GROUP_SPAN_TRIPLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                fg);
                    }
                    break;
                default:
                    fsm_ptr->rv = (BCM_E_PARAM);
            }
        }
    } else if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {

        if (bcmFieldGroupModeAuto == fsm_ptr->mode) {

            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group mode auto.\n")));

            /* Single wide intra slice selection. */
            if ((0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY))) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Trying single ...\n")));
                /* 160bit/128Bit mode. */
                fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                             | _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
                fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
            }

            /* Double wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying double...\n")));
                    /* 320bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_DOUBLE_SLICE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1, fg);
                }
            }
        } else {
            switch (fsm_ptr->mode) {
                case bcmFieldGroupModeSingle:
                    /* Single Wide selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying intra...\n")));
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                                     | _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                                                                fg);
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                    }
                    break;
                case bcmFieldGroupModeDouble:
                    /* Double wide inter slice selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying double ..\n")));
                        fg->flags = (orig_group_flags | _FP_GROUP_SPAN_DOUBLE_SLICE); 
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, fg->qset, 1,
                                fg);
                    }
                    break;
                default:
                    fsm_ptr->rv = (BCM_E_PARAM);
            }
        }
    } else {
        return (BCM_E_INTERNAL);
    }

    if (BCM_FAILURE(fsm_ptr->rv)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit, "No success so far.\n")));

        /* No luck so far - try different alternative qset if possible. */
        if ((BCM_E_RESOURCE == fsm_ptr->rv)
             && (_BCM_FP_GROUP_ADD_STATE_QSET_UPDATE
                 == fsm_ptr->fsm_state_prev)) {
            fsm_ptr->rv = (BCM_E_NONE);
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE;
        }
    } else {

        /* Insert group into units group list. */
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Configuration found...\n")));
            fsm_ptr->rv = _bcm_field_group_linked_list_insert(unit, fsm_ptr);
        }

        if (BCM_SUCCESS(fsm_ptr->rv)) {
            /* Success proceed to slice allocation. */
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;
        } else {
            /* Failure proceed to cleanup. */
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    }

    /* Set current state to be previous state. */
    fsm_ptr->fsm_state_prev = _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET;

    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *     _field_th_group_add_qset_alternate
 * Purpose:
 *     Currently no alternate QSET identified for TH IFP qualifiers.
 *     Proceed to the next state in the state machine.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_qset_alternate(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Set current state to be previous state. */
    fsm_ptr->fsm_state_prev =  fsm_ptr->fsm_state;

    /* Alternate QSET is not available, proceed to end. */
    fsm_ptr->rv = (BCM_E_RESOURCE);
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;

    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *     _field_th_group_lt_slice_validate
 * Purpose:
 *     Validate if the slice has enought free entries.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     stage_fc      - (IN) Stage field control structure pointer.
 *     fg            - (IN) Field group structure pointer.
 *     slice_id      - (IN) Physical slice number.
 *     lt_fs         - (IN) Reference to LT Slice structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_th_group_lt_slice_validate(int unit,
                                  _field_stage_t *stage_fc,
                                  _field_group_t *fg,
                                  int slice_id,
                                  _field_lt_slice_t *lt_fs)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       int            id;                  /* iterator.           */
       uint8          presel_ent_ct = 0;   /* Presel Entry count. */
       uint8          first_entry_flg = 0;
       uint8          atomicity_entry = 0;

       /* calculated the presel entries to be created */
       for (id = 0; id < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; id++) {
           if (fg->presel_ent_arr[id] != NULL) {
              presel_ent_ct++;
           }
       }

       if (lt_fs == NULL) {
          lt_fs = stage_fc->lt_slices[fg->instance] + slice_id;
          first_entry_flg = 1; 
       } 

       /*
        * To provide SW atomicity, an entry in the slice has to be kept
        * free to move other entries in the slice during priority configuration.
        * Hence, free entries has to be reduced to 1 during validation.
        */ 
       if (lt_fs->slice_flags & _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT) {
          if (lt_fs->free_count == 0) {
             return BCM_E_INTERNAL;  
          }
          atomicity_entry = 1; 
       }       
   
       if ((lt_fs->free_count - atomicity_entry) < presel_ent_ct) {
          if (first_entry_flg == 1) {
             LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "No free entries left in the LT slice[%d] to create [%d]"
                " Preselector entries.\n\r"),
                slice_id, presel_ent_ct));
             return (BCM_E_CONFIG);
          } else {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Can not create the requested %d Presel entries"
                " for the given group's priority[%d].\n\r"
                "Only %d free entries left in the LT slice[%d]\n\r"),
                presel_ent_ct, fg->priority,
               (lt_fs->free_count - atomicity_entry), slice_id));
             return (BCM_E_RESOURCE);
          }
       }
    } else {
       int             part;            /* Part iterator */
       int             parts_count = 0; /* Entry Parts */
       _field_slice_t  *fs;             /* Field slice structure pointer.*/
 
       /* Get number of entry parts based on the field group flags. */
       BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                          &parts_count));

       /* Validate slice LT_MAP status. */
       for (part = 0; part < parts_count; part++) {
          fs = stage_fc->slices[fg->instance] + slice_id + part;
          if (0 != fs->lt_map) {
              LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Verb: Slice=%d in-use.\n"),
                   unit, (slice_id + part)));
              return (BCM_E_CONFIG);
          }
       }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_slice_groups_get_next
 * Purpose:
 *     Retrieve the next valid group pointer associated with the given slice.
 * Parameters:
 *     unit       - (IN   BCM device number.
 *     instance   - (IN)  Pipe to which group belongs to.
 *     stage_id   - (IN)  Stage (IFP/VFP/EFP) to which group belongs to.
 *     slice_id   - (IN)  Field slice id.
 *     curr_group - (IN)  Reference to current group index associated to slice.
 *     next_group - (OUT) Reference to next available group index associated to
 *                        the given slice.
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     "*curr_group == NULL" to retrieve the first group associated with the
 *      given slice.
 */
int
_bcm_field_th_slice_group_get_next(int unit,
                                   int instance,
                                   _field_stage_id_t stage_id, 
                                   int slice_id,
                                   _field_group_t **curr_group,
                                   _field_group_t **next_group)
{
    _field_group_t    *fg = NULL;
    _field_slice_t    *fs = NULL;

    if (curr_group == NULL || next_group == NULL) {
       return BCM_E_PARAM;
    }

    if (*curr_group == NULL) {
       _field_control_t  *fc;     /* Field Control Structure */

       /* Get device Field control structure. */
       BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
       fg = fc->groups;
    } else {
       fg = (*curr_group)->next;
    }
 
    *next_group = NULL;
    for (; fg != NULL; fg = fg->next) {
        if ((instance != fg->instance) || (stage_id != fg->stage_id)) {
            continue;
        }

        for (fs = fg->slices; fs != NULL; fs = fs->next) {
            if (fs->slice_number == slice_id) {
               *next_group = fg;
               return BCM_E_NONE;
            }
        } 
    }
    
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _bcm_field_th_group_add_slice_validate
 * Purpose:
 *     Validate a candidate slice if it fits for a group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     stage_fc      - (IN) State machine tracking structure.
 *     fg            - (IN) Field group structure.
 *     slice_id      - (IN) Candidate slice id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_add_slice_validate(int unit,
                                       _field_stage_t *stage_fc,
                                       _field_group_t *fg,
                                       int slice_id)
{
    int                rv;                                /* Return status. */
    _field_slice_t     *fs_part[_FP_MAX_ENTRY_WIDTH] = {NULL}; /* Field slice
                                                                  structure
                                                                  pointer. */
    _field_group_t     *fg_ptr = NULL;
    _field_lt_slice_t  *lt_slice = NULL;

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Validate slice for exact match group. */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        return _field_th_exactmatch_slice_validate(unit, stage_fc,
                                                     fg, slice_id);
    }

    /*
     * If the slice belongs to an auto-expanded group and it is not the first
     * slice of that group, cannot use it.
     */
    if (stage_fc->slices[fg->instance][slice_id].prev != NULL) {
        return (BCM_E_CONFIG);
    }

    /*
     * Skip the slice if group is Intraslice and slice is not Intraslice
     * capable.
     */
    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
        && !(stage_fc->slices[fg->instance][slice_id].slice_flags
            & _BCM_FIELD_SLICE_INTRASLICE_CAPABLE)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for IntraSliceDouble wide-mode.\n"),
             unit, slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Requested 80bit single wide group mode. Check if slice is already
     * operating in 160bit mode and return error if that's true.
     */
    fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
    if (((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
            && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
         && (fs_part[0]->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Single wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Three TCAMs are grouped to give a memory macro of size "160b * 256 * 3".
     * Triple wide group is created using slices within the same memory macro.
     */
    if ((fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) && (slice_id % 3 != 0)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Triple wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Double wide group is created using consecutive slices within a memory
     * macro. But consecutive slices across a memory macro boundary cannot be
     * paried to create a double wide group. Skip slices 2, 5, 8 and 11 from
     * allocating for Double Wide Group base slice.
     */
    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) && (2 == (slice_id % 3))) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Double wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * For slices 1, 4, 7 and 10 check if previous slice is configured for Double
     * wide mode. If TRUE, then skip current slice for Double wide groups.
     */
    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) && (1 == (slice_id % 3))) {
        fs_part[0] = stage_fc->slices[fg->instance] + (slice_id - 1);
        if (fs_part[0]->group_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: slice=%d is secondary slice of DoubleWide"
                " group.\n"), unit, slice_id));
            return (BCM_E_CONFIG);
        }

        /*
         * If previous slice is unused, then current slice will be used as
         * secondary slice for the double wide group, hence skip it.
         */
        if (0 == (fs_part[0]->group_flags & _FP_GROUP_SPAN_MASK)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: slice=%d accounted for DoubleWide"
                " group.\n"), unit, slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /*
     * Validate slice current mode for double wide group's allocation
     * eligibility.
     */
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        fs_part[1] = stage_fc->slices[fg->instance] + (slice_id + 1);
        if (((fs_part[0]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fs_part[0]->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
            || ((fs_part[1]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fs_part[1]->group_flags
                     & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            /*
             * HW Limitation: Single wide mode slices (80-bit wide) in use
             * cannot be paired with another slice.
             */
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: bad slice=%d for Double wide-mode.\n"),
                 unit, slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /* Validate slice for triple wide mode use. */
    if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        fs_part[1] = stage_fc->slices[fg->instance] + slice_id + 1;
        fs_part[2] = stage_fc->slices[fg->instance] + slice_id + 2;
        if (((fs_part[0]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fs_part[0]->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
            || ((fs_part[1]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                 && !(fs_part[1]->group_flags
                      & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
            || ((fs_part[2]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                 && !(fs_part[2]->group_flags
                      & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            /*
             * HW Limitation: Single wide mode slices (80-bit wide) in use
             * cannot be paired with another slice.
             */
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: bad slice=%d for Triple-mode.\n"), unit,
                 slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /* Initialize group auto expansion support status. */
    if (soc_feature(unit, soc_feature_field_virtual_slice_group)) {
        if (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION) {
            fg->flags |= _FP_GROUP_SELECT_AUTO_EXPANSION;
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Group Auto Expansion status=%d.\n"),
                 unit, (fg->flags & _FP_GROUP_SELECT_AUTO_EXPANSION) ? 1: 0));
        }
    }

    /* Retrieve the existing groups associated with the Slice */
    rv = _bcm_field_th_slice_group_get_next(unit, fg->instance, 
                                            fg->stage_id, slice_id, 
                                            &fg_ptr, &fg_ptr);

    if (BCM_SUCCESS(rv)) {

        int mode;
     
        /* In case of auto-expansion, go for a next slice for the same group. */
        if (fg == fg_ptr) {
           return (BCM_E_CONFIG);
        }

        /* Group with prioirty ANY, can't share slice with any other group's. */
        if (BCM_FIELD_GROUP_PRIO_ANY == fg->priority) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group with prio ANY"
                            "can't share slice with any other groups.\n\r")));
            return BCM_E_CONFIG;
        }

        /* Compare the group priority with the existing group in the slice. */
        if (fg->priority != fg_ptr->priority) {
           LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Slice[%d] contains other groups with different priority.\n\r"),
              slice_id));
           return (BCM_E_CONFIG);
        }

        /* Allow two groups without preselectors and with same prioirty for 
         *  backward comaptability. If two groups have same prioirty, that 
         *  means prioirty b/w these two groups doesnot matter to the user.
         */
        if ((fg_ptr->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0 &&
            (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
            return BCM_E_CONFIG;  
        }
        
       /*
        * Groups can't share the slice, if the existing group
        * doesn't support preselector
        */
        if ((fg_ptr->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Group creation failed, Already default group exists"
             " with the same priority[%d].\n\r"),
              fg_ptr->priority));
           return (BCM_E_PARAM);
        }

        if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Group creation failed, can't create a group with the priority"
            " same as existing preselector group priority[%d]\n\r"),
              fg_ptr->priority));
           return (BCM_E_PARAM);
        }

        /* Compare the group modes */
        mode = _FP_GROUP_SPAN_SINGLE_SLICE | _FP_GROUP_SPAN_DOUBLE_SLICE |
               _FP_GROUP_SPAN_TRIPLE_SLICE | _FP_GROUP_INTRASLICE_DOUBLEWIDE;

        if ((fg->flags & mode) != (fg_ptr->flags & mode)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Group creation failed, already existing group with the same"
             " priority[%d] is in different mode. Can't create same priority"
             " groups with different modes.\n\r"),
              slice_id));
           return (BCM_E_PARAM);
        }

        /* Update the LT Slice to which the group's
         * preselector entries are to be created. */
        lt_slice = fg_ptr->lt_slices;
    } else if (rv == BCM_E_NOT_FOUND) {
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        if (fs_part[0]->group_flags != 0x0) {
            /*
             *  If group_flags is not NULL, the slice must belongs to some
             *  other group and should be a part of double-wide or Triple-wide
             *  for the previous slice.
             */
            return BCM_E_CONFIG;
        }
    } else {
        return rv;
    }

    /*
     * To share slices with more than one field group, mutually exclusive
     * pre-selection rules must be setup in LT select TCAM. In legacy mode,
     * only a default pre-selection rule is installed per-slice. Hence slices
     * cannot be shared by multiple field groups. Validate slice LT mapping to
     * confirm it's available for new group. For wide mode group, all slices
     * should be available for group create to succeed.
     */
    BCM_IF_ERROR_RETURN(_field_th_group_lt_slice_validate(unit, stage_fc, fg,
                                                          slice_id, lt_slice));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_slice_mode_set
 * Purpose:
 *     Program group's slice mode in hardware.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure.
 *     slice_num    - (IN) Slice for which mode is to be set in HW.
 *     fg           - (IN) Field group structure pointer.
 *     clear        - (IN) Reset to hardware default value.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_slice_mode_set(int unit, _field_stage_t *stage_fc,
                                 uint8 slice_num, _field_group_t *fg,
                                 int clear)
{
    uint8 mode;                          /* Slice mode: 0-Narrow, 1-Wide.  */
    uint32 ifp_config;                   /* Register SW entry buffer.      */
    static soc_reg_t ifp_config_regs[] = /* Per-Pipe IFP config registers. */
        {
            IFP_CONFIG_PIPE0r,
            IFP_CONFIG_PIPE1r,
            IFP_CONFIG_PIPE2r,
            IFP_CONFIG_PIPE3r
        };

    /* Input Parameter Check */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for exact match stage. */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    if (0 == clear) {
        /* Determine the slice mode based on group flags. */
        mode = (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 0 : 1;
    } else {
        /* HW reset default is "1 - Wide" mode. */
        mode = 1;
    }


    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Get slice configuration from hardware. */
            BCM_IF_ERROR_RETURN(READ_IFP_CONFIGr(unit, slice_num, &ifp_config));

            /* Set slice mode value in register buffer. */
            soc_reg_field_set(unit, IFP_CONFIGr, &ifp_config, IFP_SLICE_MODEf,
                mode);

            /* Write register value to hardware. */
            BCM_IF_ERROR_RETURN(WRITE_IFP_CONFIGr(unit, slice_num, ifp_config));
            break;

        case bcmFieldGroupOperModePipeLocal:
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                ifp_config_regs[fg->instance], REG_PORT_ANY, slice_num,
                &ifp_config));

            /* Set slice mode value in register buffer. */
            soc_reg_field_set(unit, ifp_config_regs[fg->instance], &ifp_config,
                IFP_SLICE_MODEf, mode);

            BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                ifp_config_regs[fg->instance], REG_PORT_ANY, slice_num,
                ifp_config));
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_dirty
 * Purpose:
 *     Check if logical table entry was modified after last installation
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     lt_f_ent - (IN)  Primary entry pointer.
 *     dirty    - (OUT) Entry dirty flag status.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_dirty(int unit, _field_lt_entry_t *lt_f_ent, int *dirty)
{
    int parts_count = 0;    /* Entry TCAM parts count.  */
    int idx;                /* Entry parts iterator.    */

    /* Input parameters check. */
    if (NULL == lt_f_ent || NULL == dirty) {
        return (BCM_E_PARAM);
    }

    /* Get number of TCAM parts based on group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    for (idx = 0; idx < parts_count; idx++) {
        if (lt_f_ent[idx].flags & _FP_ENTRY_DIRTY) {
            break;
        }
    }

    /* Update dirty flags status based on part index value. */
    *dirty = (idx < parts_count) ? TRUE : FALSE;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_lt_entry_t_compare
 * Purpose:
 *      Compare logical table entry id in _field_lt_entry_t structure.
 * Parameters:
 *      a - (IN) first entry to compare.
 *      b - (IN) second entry to compare with.
 * Returns:
 *      a<=>b
 */
STATIC int
_field_lt_entry_t_compare(void *a, void *b)
{
    _field_lt_entry_t **first; /* First compared entry. */
    _field_lt_entry_t **second; /* Second compared entry. */

    first = (_field_lt_entry_t **)a;
    second = (_field_lt_entry_t **)b;

    if ((*first)->eid < (*second)->eid) {
        return (-1);
    } else if ((*first)->eid > (*second)->eid) {
        return (1);
    }
    return (0);
}

/*
 * Function:
 *     _field_th_entry_flags_to_tcam_part
 * Purpose:
 *     Each logical table field entry contains up to 3 TCAM entries. This
 *     routine maps sw entry flags to tcam entry (0-2).
 *     Note this is not a physical address in tcam.
 *     Single: 0
 *     Single & Intraslice Double: 0
 *     Paired: 0, 1
 *     Triple: 0, 1, 2
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     entry_flags - (IN)  Entry flags.
 *     group_flags - (IN)  Entry group flags.
 *     entry_part  - (OUT) Entry part (0-2)
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_INTERNAL   - Error
 */
int
_bcm_field_th_entry_flags_to_tcam_part(int unit, uint32 entry_flags,
                                       uint32 group_flags,
                                       uint8 *entry_part)
{
    /* Input parameter check. */
    if (NULL == entry_part) {
        return (BCM_E_PARAM);
    }

    if (entry_flags & _FP_ENTRY_PRIMARY) {
        *entry_part = 0;
    } else if (entry_flags & _FP_ENTRY_SECONDARY) {
        *entry_part = 1;
    } else if (entry_flags & _FP_ENTRY_TERTIARY) {
        *entry_part = 2;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_get
 * Purpose:
 *     Lookup a LT entry in Group's LT entries array for a given unit ID,
 *     Entry ID and entry flags.
 * Parmeters:
 *     unit     - (IN)  BCM device number.
 *     lt_eid   - (IN)  Logical table Entry ID.
 *     flags    - (IN)  Entry flags to match.
 *     lt_ent_p - (OUT) Entry lookup result.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_get(int unit, bcm_field_entry_t lt_eid, uint32 flags,
                       _field_lt_entry_t **lt_ent_p)
{
    _field_lt_entry_t target; /* LT entry lookup variable. */
    _field_lt_entry_t *lt_f_ent; /* LT entry lookup pointer. */
    _field_control_t *fc; /* Device Field control structure. */
    _field_group_t *fg; /* Field group structure. */
    uint8 entry_part = 0; /* Wide entry part number. */
    int idx; /* LT entry index. */

    /* Input parameters check. */
    if (NULL == lt_ent_p) {
        return (BCM_E_PARAM);
    }

    /* Initialize target logical table entry. */
    target.eid = lt_eid;
    lt_f_ent = &target;

    /* Get device Field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get field groups handle. */
    fg = fc->groups;

    while (NULL != fg) {
        /* Skip empty groups. */
        if (NULL == fg->lt_entry_arr) {
            fg = fg->next;
            continue;
        }

        /* Search in group entry array list.*/
        idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                    sizeof(_field_lt_entry_t *), (void *)&lt_f_ent,
                    _field_lt_entry_t_compare);
        if (idx >= 0) {
            (void) _bcm_field_th_entry_flags_to_tcam_part(unit, flags,
                                   fg->flags, &entry_part);
            *lt_ent_p = fg->lt_entry_arr[idx] + entry_part;
            return (BCM_E_NONE);
        }
        fg = fg->next;
    }

    /* LT entry with lt_eid not found. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _field_th_lt_entry_get_by_id
 * Purpose:
 *     Lookup a complete Logical table FP rule(entry) from a unit & rule id
 *     choice.
 * Parmeters:
 *     unit         - (IN) BCM device number.
 *     lt_eid       - (IN) LT Entry id.
 *     lt_ent_arr   - (OUT) Entry lookup result array.
 *                          (Array of pointers to primary/secondary/teriary ...
 *                           parts of entry).
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_get_by_id(int unit, bcm_field_entry_t lt_eid,
                             _field_lt_entry_t **lt_ent_arr)
{
    int rv; /* Operation return status. */

    /* Input parameter check. */
    if (NULL == lt_ent_arr) {
        return (BCM_E_PARAM);
    }

    /* Initialize entry pointer array*/
    *lt_ent_arr = NULL;

    rv = _field_th_lt_entry_get(unit, lt_eid, _FP_ENTRY_PRIMARY, lt_ent_arr);
    return (rv);
}

/*
 * Function:
 *     _field_th_lt_entry_tcam_idx_get
 * Purpose:
 *     Get the TCAM index of an entry ID.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     lt_f_ent     - (IN) Pointer to field logical table entry structure.
 *     lt_fs        - (IN) Pointer to field logical table slice structure.
 *     tcam_idx     - (OUT) Entry tcam index.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_tcam_idx_get(int unit, _field_lt_entry_t *lt_f_ent,
                                _field_lt_slice_t *lt_fs, int *tcam_idx)
{
    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == tcam_idx || (NULL == lt_fs))) {
        return (BCM_E_PARAM);
    }

    /* Requested entry structure sanity check. */
    if ((NULL == lt_f_ent->group) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Calculate the TCAM index. */
    *tcam_idx = lt_fs->start_tcam_idx + lt_f_ent->index;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_tcam_mem_get
 * Purpose:
 *     Get logica table memory identifier based on stage group operational mode.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     lt_f_ent     - (IN)     Pointer to field logical table entry structure.
 *     lt_tcam_mem  - (IN/OUT) LT memory identifer pointer.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_tcam_mem_get(int unit, _field_stage_t *stage_fc,
                          _field_lt_entry_t *lt_f_ent,
                          soc_mem_t *lt_tcam_mem)
{
    /* Per-Pipe IFP Logical Table TCAM view. */
    static const soc_mem_t lt_ifp_tcam[] =
    {
        IFP_LOGICAL_TABLE_SELECT_PIPE0m,
        IFP_LOGICAL_TABLE_SELECT_PIPE1m,
        IFP_LOGICAL_TABLE_SELECT_PIPE2m,
        IFP_LOGICAL_TABLE_SELECT_PIPE3m
    };
    
    /* Per-Pipe Exact Match Logical Table TCAM view. */
    static const soc_mem_t lt_em_tcam[] =
    {
        EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE2m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE3m
    };
    
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent) || (NULL == lt_tcam_mem)) {
        return (BCM_E_PARAM);
    }

    /* Set LT selection memory identifier based on stage group oper mode. */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = IFP_LOGICAL_TABLE_SELECTm;
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = lt_ifp_tcam[lt_f_ent->group->instance];
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = lt_em_tcam[lt_f_ent->group->instance];
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_tcam_entry_get
 * Purpose:
 *     Read an entry installed in hardware into SW entry buffer.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     fg       - (IN)  Pointer to field group structure.
 *     lt_f_ent - (OUT) Pointer to field logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_tcam_entry_get(int unit, _field_group_t *fg,
                            _field_lt_entry_t *lt_f_ent)
{
    uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry buffer.    */
    _field_stage_t *stage_fc;     /* Stage field control structure. */
    int tcam_idx;                 /* TCAM index.                    */
    static soc_mem_t lt_tcam_mem; /* LT selection TCAM memory ID.   */
    int rv;                       /* Operation return status.       */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    /* Check if LT TCAM entry is already present in SW. */
    if (NULL != lt_f_ent->tcam.key) {
        return (BCM_E_NONE);
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get key and data field based on stage */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Get key field size in logical table entry. */
        lt_f_ent->tcam.key_size = WORDS2BYTES(soc_mem_field_length(unit,
                    IFP_LOGICAL_TABLE_SELECTm, KEYf));

        /* Get data field size in logical table entry. */
        lt_f_ent->tcam.data_size = WORDS2BYTES(soc_mem_field_length(unit,
                    IFP_LOGICAL_TABLE_SELECTm, DATAf));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        /* Get key field size in logical table entry. */
        lt_f_ent->tcam.key_size = WORDS2BYTES(soc_mem_field_length(unit,
                    EXACT_MATCH_LOGICAL_TABLE_SELECTm, KEYf));

        /* Get data field size in logical table entry. */
        lt_f_ent->tcam.data_size = WORDS2BYTES(soc_mem_field_length(unit,
                    EXACT_MATCH_LOGICAL_TABLE_SELECTm, DATAf));
    } else {
        return (BCM_E_PARAM);
    }

    /* Allocate memory to store entry Key info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.key, lt_f_ent->tcam.key_size,
                    "LT Key Alloc.");
    if (NULL == lt_f_ent->tcam.key) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Allocate memory to store entry Mask info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.mask, lt_f_ent->tcam.key_size,
                    "LT Mask Alloc.");
    if (NULL == lt_f_ent->tcam.mask) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Allocate memory to store entry Data info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.data, lt_f_ent->tcam.data_size,
                    "LT Data Alloc.");
    if (NULL == lt_f_ent->tcam.data) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Get entry TCAM index. */
    rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_ent, lt_f_ent->lt_fs,
                                         &tcam_idx);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Get logical table memory name based on the group operational mode. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

    /* Read entry part from hardware. */
    rv = soc_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx,
            tcam_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Get entry Key value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, KEYf,
        lt_f_ent->tcam.key);

    /* Get entry Mask value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, MASKf,
        lt_f_ent->tcam.mask);

    /* Get entry Data value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, DATAf,
        lt_f_ent->tcam.data);

    return (BCM_E_NONE);

error:
    if (lt_f_ent->tcam.key) {
        sal_free(lt_f_ent->tcam.key);
        lt_f_ent->tcam.key = NULL;
    }

    if (lt_f_ent->tcam.mask) {
        sal_free(lt_f_ent->tcam.mask);
        lt_f_ent->tcam.mask = NULL;
    }

    if (lt_f_ent->tcam.data) {
        sal_free(lt_f_ent->tcam.data);
        lt_f_ent->tcam.data = NULL;
    }

    return (rv);
}

/*
 * Function:
 *     _field_th_lt_default_entry_install
 * Purpose:
 *     Install a logical table entry in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Field logical table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_install(int unit, bcm_field_entry_t lt_entry)
{
    _field_control_t *fc;           /* Field control structure. */
    _field_lt_entry_t *lt_f_ent;    /* LT field entry.          */
    int rv;                         /* Operation return status. */
    int dirty;                      /* Field entry dirty flag.  */

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Lookup using entry ID and get the SW entry copy. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
        _FP_ENTRY_PRIMARY, &lt_f_ent));

    /* Check if entry has been modified since last install in hardware. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_dirty(unit, lt_f_ent, &dirty));

    if (dirty) {
        /* Entry has changed, install in hardware. */
        rv = _field_th_lt_default_tcam_entry_install(unit, lt_entry, 0);
    } else {
        /* No change and nothing to do, return success. */
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *     _field_th_group_lt_entry_add
 * Purpose:
 *     Given stage, group instance, slice and entry offset in slice, calculate
 *     TCAM index.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     fg       - (IN)  Field group structure.
 *     lt_f_ent - (OUT) Field logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_lt_entry_add(int unit, _field_group_t *fg,
                             _field_lt_entry_t *lt_f_ent)
{
    _field_lt_entry_t **lt_f_ent_arr;   /* Field group LT entry array. */
    int mem_sz;                         /* Allocation memory size. */
    int idx;                            /* Entry insertion index. */
    int tmp;                            /* Temporary iterator. */

    /* Input parameters check. */
    if (NULL == fg || NULL == lt_f_ent) {
        return (BCM_E_PARAM);
    }

    /* Verify if entry is already present in the group. */
    if (NULL != fg->lt_entry_arr) {
        idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                    sizeof(_field_lt_entry_t *), &lt_f_ent,
                    _field_lt_entry_t_compare);
        if (idx >= 0) {
            return (BCM_E_NONE);
        }
    } else {
        idx = -1; /* Insert into 0th location. */
    }

    /* Check if group has enough room for new entry. */
    if ((fg->lt_grp_status.entry_count + 1)
        > (fg->lt_ent_blk_cnt * _FP_GROUP_ENTRY_ARR_BLOCK)) {
        mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->lt_ent_blk_cnt + 1)
                    * sizeof(_field_lt_entry_t *);
        lt_f_ent_arr = NULL;
        _FP_XGS3_ALLOC(lt_f_ent_arr, mem_sz, "field group LT entries array");
        if (NULL == lt_f_ent_arr) {
            return (BCM_E_MEMORY);
        }

        /* Copy original array to newly allocated one. */
        if (NULL != fg->lt_entry_arr) {
            mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->lt_ent_blk_cnt)
                        * sizeof(_field_lt_entry_t *);
            sal_memcpy(lt_f_ent_arr, fg->lt_entry_arr, mem_sz);
            sal_free(fg->lt_entry_arr);
        }

        fg->lt_entry_arr = lt_f_ent_arr;
        fg->lt_ent_blk_cnt++;
    }

    /* Check entry block exists to add the entry. */
    if (NULL != fg->lt_entry_arr) {
        /*
         * Make room for entry insertion.
         */
        idx = (((-1) * idx) - 1);
        tmp = fg->lt_grp_status.entry_count - 1;
        while (tmp >= idx) {
            fg->lt_entry_arr[tmp + 1] = fg->lt_entry_arr[tmp];
            tmp--;
        }
        fg->lt_entry_arr[idx] = lt_f_ent;
        fg->lt_grp_status.entry_count++;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_slice_offset_to_tcam_index
 * Purpose:
 *     Given stage, group instance, slice and entry offset in slice, calculate
 *     TCAM index.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field entry structure.
 *     slice    - (IN) Entry slice number.
 *     slice_idx -(IN) Entry offset in the slice.
 *     tcam_idx - (OUT) Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_slice_offset_to_tcam_index(int unit, _field_stage_t *stage_fc,
                                            int instance, int slice,
                                            int slice_idx, int *tcam_idx)
{
    _field_lt_slice_t *lt_fs; /* Logical table slice number. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == tcam_idx)) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check. */
    if (instance < 0 || instance >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Get logical table slice structure. */
    lt_fs = stage_fc->lt_slices[instance] + slice;

    /*
     * Compute TCAM index using the slice start TCAM index and supplied
     * slice offset index.
     */
    *tcam_idx = lt_fs->start_tcam_idx + slice_idx;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_tcam_idx_to_slice_offset
 * Purpose:
 *     Given stage, group instance and tcam_index, calculate slice and entry
 *     offset in the slice.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure.
 *     instance     - (IN) Field Group instance.
 *     tcam_idx     - (IN) Entry tcam index.
 *     slice        - (OUT) Entry slice number.
 *     slice_idx    - (OUT) Entry offset in the slice.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_tcam_idx_to_slice_offset(int unit, _field_stage_t *stage_fc,
                                          int instance, int tcam_idx,
                                          int *slice, int *slice_idx)
{
    _field_lt_slice_t *lt_fs;   /* Field Slice number.  */
    int idx;                    /* Slice iterator.      */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == slice_idx) || (NULL == slice)
        || (instance < 0 || instance > stage_fc->num_instances)) {
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < stage_fc->tcam_slices; idx++) {
        lt_fs = stage_fc->lt_slices[instance] + idx;
        if (tcam_idx < (lt_fs->start_tcam_idx + lt_fs->entry_count)) {
            *slice = idx;
            *slice_idx = tcam_idx - lt_fs->start_tcam_idx;
            break;
        }
    }

    /* TCAM index sanity check. */
    if (idx == stage_fc->tcam_slices) {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_part_tcam_idx_get
 *
 * Purpose:
 *     Given primary entry slice/idx calculate
 *     secondary/tertiary slice/index.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     f_ent   - (IN) Field entry pointer.
 *     idx_pri - (IN) Primary entry tcam index.
 *     ent_part    - (IN) Entry part id.
 *     idx_out - (OUT) Entry part tcam index.
 * Returns
 *     BCM_E_XXX
 *
 */
int
_bcm_field_th_lt_entry_part_tcam_idx_get(int unit, _field_group_t *fg,
                                         uint32 idx_pri, uint8 ent_part,
                                         int *idx_out)
{
    uint8 slice_num;            /* Primary entry slice number.      */
    int pri_slice = -1;          /* Prmary entry slice number.       */
    int pri_index = -1;          /* Primary entry slice index.       */
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    _field_lt_slice_t *lt_fs;   /* Logical table slice structure.   */
    int rv;                     /* Operation return status.         */


    /* Input parameters check. */
    if (NULL == fg || NULL == idx_out) {
        return (BCM_E_PARAM);
    }

    /* Primary entry index. */
    if (0 == ent_part) {
        *idx_out = idx_pri;
        return (BCM_E_NONE);
    }

    /* Get Stage Field Control Structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /*
     * Get primary part entry slice number and slice offset given entry TCAM
     * index.
     */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_idx_to_slice_offset(unit,
        stage_fc, fg->instance, idx_pri, &pri_slice, &pri_index));

    /* Get entry part to slice number 0, 1, 2 */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(ent_part,
        fg->flags, &slice_num));

    /* Get slice descriptor structure. */
    lt_fs = stage_fc->lt_slices[fg->instance] + (pri_slice + slice_num);

    /* Get the TCAM index given slice number and slice entry offset. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
            fg->instance, lt_fs->slice_number, pri_index, idx_out);

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_default_entry_phys_create
 * Purpose:
 *    Create a physical entry in a logical table slice.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN) Stage field control structure.
 *    lt_ent    - (IN) Logical table entry identifier.
 *    prio      - (IN) Logical table entry priority.
 *    lt_fs     - (IN) Logical table slice structure.
 *    fg        - (IN) Field group structure.
 *    ent_p     - (OUT) Allocated & initialized entry.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_phys_create(int unit, _field_stage_t *stage_fc,
                                   bcm_field_entry_t lt_ent, int prio,
                                   _field_lt_slice_t *lt_fs, _field_group_t *fg,
                                   _field_lt_entry_t **ent_p)
{
    _field_control_t *fc;   /* Field control structure.              */
    int idx;                /* Slice entries iteration index.        */
    int parts_count = -1;   /* Number of entry parts.                */
    int part_index;         /* Entry parts iterator.                 */
    int mem_sz;             /* Memory allocation size.               */
    _field_lt_entry_t *lt_f_ent = NULL; /* LT field entry structure. */
    int rv;                 /* Operation return status.              */
    int pri_tcam_idx = -1;  /* Primary entry TCAM index.             */
    int slice_num = -1;     /* Slice number.                         */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_fs) || (NULL == fg)
         || (NULL == ent_p)) {
        return (BCM_E_PARAM);
    }

    /* Check if free entries are available in the slice. */
    if (0 == lt_fs->free_count) {
        return (BCM_E_RESOURCE);
    }

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get number of entry parts based on group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Calcuate buffer size required for SW entry. */
    mem_sz = sizeof(_field_lt_entry_t) * parts_count;

    /* Allocate and zero memory for LT selection entry. */
    _FP_XGS3_ALLOC(lt_f_ent, mem_sz, "LT field entry");
    if (NULL == lt_f_ent) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: allocation failure for LT entry\n"), unit));
        return (BCM_E_MEMORY);
    }

    /* Get index for primary entry. */
    for (idx = 0; idx < lt_fs->entry_count; idx++) {
        if (NULL == lt_fs->entries[idx]) {
            lt_f_ent->index = idx;
            break;
        }
    }

    /* Get entry TCAM index given slice number and entry offset in slice. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
            fg->instance, lt_fs->slice_number, lt_f_ent->index, &pri_tcam_idx);
    if (BCM_FAILURE(rv)) {
        sal_free(lt_f_ent);
        return (rv);
    }

    /* Fill entry primitives. */
    for (idx = 0; idx < parts_count; idx++) {
        lt_f_ent[idx].eid = lt_ent;
        lt_f_ent[idx].prio = prio;
        lt_f_ent[idx].group = fg;

        /* Get entry part flags. */
        rv = _bcm_field_th_tcam_part_to_entry_flags(unit, idx, fg->flags,
                &lt_f_ent[idx].flags);
        if (BCM_FAILURE(rv)) {
            sal_free(lt_f_ent);
            return rv;
        }

        /* Given primary entry TCAM index calculate entry part tcam index. */
        if (0 != idx) {
            rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, lt_f_ent->group,
                                                pri_tcam_idx, idx, &part_index);
            if (BCM_FAILURE(rv)) {
                sal_free(lt_f_ent);
                return (rv);
            }

            /*
             * Given entry part TCAM index, determine the slice number and slice
             * offset index.
             */
            rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                    fg->instance, part_index, &slice_num,
                    (int *)&lt_f_ent[idx].index);
            if (BCM_FAILURE(rv)) {
                sal_free(lt_f_ent);
                return (rv);
            }
            lt_f_ent[idx].lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;
        } else {
            /* Set entry slice. */
            lt_f_ent[idx].lt_fs = lt_fs;
        }

        /* Decrement slice free entry count for primary entries. */
        lt_f_ent[idx].lt_fs->free_count--;

        /* Assign entry to a slice. */
        lt_f_ent[idx].lt_fs->entries[lt_f_ent[idx].index] = lt_f_ent + idx;

        /* Mark entry dirty. */
        lt_f_ent[idx].flags |= _FP_ENTRY_DIRTY;
    }

    /* Insert the entry into group entry array. */
    rv = _field_th_group_lt_entry_add(unit, fg, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        sal_free(lt_f_ent);
        return (rv);
    }

    /* Return allocated/filled entry structure to the caller. */
    *ent_p = lt_f_ent;
    return (rv);
}

/*
 * Function:
 *    _field_th_group_lt_entry_delete
 * Purpose:
 *    Delete an entry from group LT entries array.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    fg       - (IN) Field group structure.
 *    lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_lt_entry_delete(int unit, _field_group_t *fg,
                                _field_lt_entry_t *lt_f_ent)
{
    int idx; /* Entry deletion index.   */
    int tmp; /* Temporary iterator.     */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Ensure group entry array is valid. */
    if (NULL == fg->lt_entry_arr) {
        return (BCM_E_INTERNAL);
    }

    /* Confirm entry is present in the group. */
    idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                sizeof(_field_lt_entry_t *), &lt_f_ent,
                _field_lt_entry_t_compare);
    if (idx < 0) {
        return (BCM_E_NOT_FOUND);
    }

    /* Store array index where the entry was found. */
    tmp = idx;
    /*
     * Re-Pack the entries array to clear the last pointer due to entry deletion.
     */
    while (tmp < fg->lt_grp_status.entry_count - 1) {
        fg->lt_entry_arr[tmp] = fg->lt_entry_arr[tmp + 1];
        tmp++;
    }

    /* Decrement total valid entry count. */
    fg->lt_grp_status.entry_count--;

    /* Clear entry address stored in the last array index. */
    fg->lt_entry_arr[fg->lt_grp_status.entry_count] = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_lt_default_entry_phys_destroy
 * Purpose:
 *    Destroy a physical entry from a logical table slice.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_phys_destroy(int unit, _field_lt_entry_t *lt_f_ent)
{
    _field_control_t *fc;       /* Device field control structure. */
    _field_lt_slice_t *lt_fs;   /* Logical table slice structure.  */
    _field_group_t *fg;         /* Field group structure.          */
    int parts_count = -1;       /* Entry parts count.              */
    int idx;                    /* Entry iterator index.           */
    uint8 slice_number;         /* Slice number.                   */
    int rv;                     /* Operation return status.        */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get entry slice information. */
    lt_fs = lt_f_ent->lt_fs;

    /* Get the group to which the entry belongs. */
    fg = lt_f_ent->group;

    /* Get entry parts count. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = 0; idx < parts_count; idx++) {

        /* Free entry TCAM key buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.key) {
            sal_free(lt_f_ent[idx].tcam.key);
        }

        /* Free entry TCAM mask buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.mask) {
            sal_free(lt_f_ent[idx].tcam.mask);
        }

        /* Free entry Data mask buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.data) {
            sal_free(lt_f_ent[idx].tcam.data);
        }

        /* Get entry part slice number. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        /* Remove entry pointer from the slice. */
        lt_fs[slice_number].entries[lt_f_ent[idx].index] = NULL;

        /* Increment slice unused entry count. */
        if (0 == (lt_f_ent[idx].flags & _FP_ENTRY_SECOND_HALF)) {
            lt_fs[slice_number].free_count++;
        }
    }

    /* Remove the entry from group entry array. */
    rv = _field_th_group_lt_entry_delete(unit, fg, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Group entry delete Eid=%d.\n"),
             unit, lt_f_ent->eid));
    }

    /* Free entry memory. */
    sal_free(lt_f_ent);
    return (rv);
}

/*
 * Function:
 *    _field_th_lt_tcam_data_mem_get
 * Purpose:
 *    Get logical table data memory identifier based on stage group operational
 *    mode.
 * Parameters:
 *    unit          - (IN) BCM device number.
 *    stage_fc      - (IN) Stage field control structure.
 *    fg            - (IN) Field group control structure.
 *    lt_tcam_mem   - (IN) Logical table data view identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_tcam_data_mem_get(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg,
                                soc_mem_t *lt_tcam_mem)
{
    /* Per-Pipe Logical Table TCAM DATA views. */
    static const soc_mem_t lt_ifp_tcam_data_mem[] =
    {
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m
    };
    static const soc_mem_t lt_em_tcam_data_mem[] =
    {
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
        EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m
    };

    /* Input parameters check. */
    if (NULL == lt_tcam_mem) {
        return (BCM_E_PARAM);
    }

    /*
     * Assign LT selection TCAM memory identifier based on
     * stage's group operational mode.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = lt_ifp_tcam_data_mem[fg->instance];
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = lt_em_tcam_data_mem[fg->instance];
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_data_value_set
 * Purpose:
 *     Set LT entry DATA portion of an entry.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     stage_fc    - (IN) Stage field control structure.
 *     fg          - (IN) Field group structure.
 *     index       - (IN) Entry part index.
 *     lt_data_mem - (IN) LT Data Memory.
 *     data        - (IN/OUT) Reference to LT Data to be configured.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_data_value_set(int unit, _field_stage_t *stage_fc,
                                      _field_group_t *fg, int index,
                                      soc_mem_t lt_data_mem, uint32 *data)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == data)) {
        return (BCM_E_PARAM);
    }

    /* Group must have a valid KeyGen index, else return error. */
    if (_FP_EXT_SELCODE_DONT_CARE == fg->ext_codes[index].keygen_index) {
        return (BCM_E_INTERNAL);
    }

    /* Check and set keygen index value. */
    soc_mem_field32_set(unit, lt_data_mem, data,
        KEY_GEN_PROGRAM_PROFILE_INDEXf, fg->ext_codes[index].keygen_index);

    /* Check and set IP Normalize control. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].normalize_l3_l4_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            NORMALIZE_L3_L4f, fg->ext_codes[index].normalize_l3_l4_sel);
    }

    /* Check and set MAC Normalize control. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].normalize_mac_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            NORMALIZE_L2f, fg->ext_codes[index].normalize_mac_sel);
    }

    /* Check and set AUX TAG A selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_A_SELf, fg->ext_codes[index].aux_tag_a_sel);
    }

    /* Check and set AUX TAG B selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_B_SELf, fg->ext_codes[index].aux_tag_b_sel);
    }

    /* Check and set AUX TAG C selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_c_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_C_SELf, fg->ext_codes[index].aux_tag_c_sel);
    }

    /* Check and set AUX TAG D selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_d_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_D_SELf, fg->ext_codes[index].aux_tag_d_sel);
    }

    /* Check and set TCP function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].tcp_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TCP_FN_SELf, fg->ext_codes[index].tcp_fn_sel);
    }

    /* Check and set TOS function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].tos_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TOS_FN_SELf, fg->ext_codes[index].tos_fn_sel);
    }

    /* Check and set TTL function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].ttl_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TTL_FN_SELf, fg->ext_codes[index].ttl_fn_sel);
    }

    /* Check and set Class ID container A selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_A_SELf,
            fg->ext_codes[index].class_id_cont_a_sel);
    }

    /* Check and set Class ID container B selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_B_SELf,
            fg->ext_codes[index].class_id_cont_b_sel);
    }

    /* Check and set Class ID container C selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_c_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_C_SELf,
            fg->ext_codes[index].class_id_cont_c_sel);
    }

    /* Check and set Class ID container D selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_d_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_D_SELf,
            fg->ext_codes[index].class_id_cont_d_sel);
    }

    /* Check and set Source Container A selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_cont_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_CONTAINER_A_SELf, fg->ext_codes[index].src_cont_a_sel);
    }

    /* Check and set Source Container B selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_cont_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_CONTAINER_B_SELf, fg->ext_codes[index].src_cont_b_sel);
    }

    /* Check and set Src Dest Container 0 selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_dest_cont_0_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_DST_CONTAINER_0_SELf, fg->ext_codes[index].src_dest_cont_0_sel);
    }

    /* Check and set Src Dest Container 1 selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_dest_cont_1_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_DST_CONTAINER_1_SELf, fg->ext_codes[index].src_dest_cont_1_sel);
    }

    /* Check and set IPBM selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].ipbm_present) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            IPBM_PRESENTf, fg->ext_codes[index].ipbm_present);
    }

    /* Set Logical Table ID value. */
    if (-1 != fg->lt_id) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            LOGICAL_TABLE_IDf, (fg->lt_id));
    }

    /* Enable lookup for this logical table in the slice. */
    soc_mem_field32_set(unit, lt_data_mem, data, ENABLEf,
        (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 1 : 0);

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Switch on entry part. */
        switch (index) {
            case 0:
                if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                        || (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {
                    /* Multiwide mode first slice. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 1);
                } else {
                    /* Independent slice. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 0);
                }
                break;
            case 1:
                if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
                    /* Multiwide mode second slice of THREE. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 3);
                } else {
                    /* Multiwide mode second slice of TWO. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 2);
                }
                break;
            case 2:
                /* Multiwide mode third slice. */
                soc_mem_field32_set(unit, lt_data_mem, data,
                        MULTIWIDE_MODEf, 4);
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        soc_mem_field32_set(unit, lt_data_mem, data,
                                         KEY_TYPEf, fg->em_mode);
    } else {
        /* Invalid Stage Id */
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_default_data_value_set
 * Purpose:
 *     Set LT entry DATA portion of an entry.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Stage field control structure.
 *     fg       - (IN)     Field group structure.
 *     index    - (IN)     Entry part index.
 *     lt_f_ent - (IN/OUT) Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_data_value_set(int unit, _field_stage_t *stage_fc,
                                          _field_group_t *fg, int index,
                                          _field_lt_entry_t *lt_f_ent)
{
    soc_mem_t lt_tcam_mem; /* LT selection TCAM data view memory name. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Group must have a valid KeyGen index, else return error. */
    if (_FP_EXT_SELCODE_DONT_CARE == fg->ext_codes[index].keygen_index) {
        return (BCM_E_INTERNAL);
    }

    /* Get LT select TCAM memory identifier. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_data_mem_get(unit, stage_fc, fg,
        &lt_tcam_mem));

    /* Allocate entry part key, mask & data buffer. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_entry_get(unit, fg, lt_f_ent));

    /* Configure TCAM data values */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_data_value_set(unit, stage_fc,
                                  fg, index, lt_tcam_mem, lt_f_ent->tcam.data));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profiles_mem_get
 * Purpose:
 *     Get LT keygen program profile memory names.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Stage field control structure.
 *     fg       - (IN)     Field group structure.
 *     mem_arr  - (IN/OUT) Pointer to memory name array.
 *     arr_len  - (IN)     Memory name array length.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profiles_mem_get(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, soc_mem_t *mem_arr,
                                  int arr_len)
{
    int idx; /* Array index iterator. */
    /* Per-Pipe KeyGen program profile table identifiers. */
    static const soc_mem_t ifp_pipe_mem[_FP_MAX_NUM_PIPES][2] =
    {
        {
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m
        },
        {
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE1m
        },
        {
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE2m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE2m
        },
        {
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE3m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE3m
        }
    };
    static const soc_mem_t em_pipe_mem[_FP_MAX_NUM_PIPES][2] =
    {
        {
           EXACT_MATCH_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
           EXACT_MATCH_KEY_GEN_MASK_PIPE0m
        },
        {
            EXACT_MATCH_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
            EXACT_MATCH_KEY_GEN_MASK_PIPE1m
        },
        {
            EXACT_MATCH_KEY_GEN_PROGRAM_PROFILE_PIPE2m,
            EXACT_MATCH_KEY_GEN_MASK_PIPE2m
        },
        {
            EXACT_MATCH_KEY_GEN_PROGRAM_PROFILE_PIPE3m,
            EXACT_MATCH_KEY_GEN_MASK_PIPE3m
        }
    };
    static const soc_mem_t ifp_global_mem[2] =
    {
        IFP_KEY_GEN_PROGRAM_PROFILEm,
        IFP_KEY_GEN_PROGRAM_PROFILE2m
    };
    static const soc_mem_t em_global_mem[2] = 
    {
        EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm,
        EXACT_MATCH_KEY_GEN_MASKm
    };

    
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == mem_arr)) {
        return (BCM_E_PARAM);
    }

    /*
     * Assign keygen memory identifier name based on stage group oper mode.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = ifp_global_mem[idx];
                }
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = em_global_mem[idx];
                }
            } else {
                return (BCM_E_PARAM);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = ifp_pipe_mem[fg->instance][idx];
                } 
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = em_pipe_mem[fg->instance][idx];
                }
            } else {
                return (BCM_E_PARAM);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profile2_entry_pack
 * Purpose:
 *     Build a LT keygen program profile2 table entry.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part         - (IN)     Entry part number.
 *     mem          - (IN)     Memory name identifier.
 *     prof2_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profile2_entry_pack(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg, int part, soc_mem_t mem,
                            ifp_key_gen_program_profile2_entry_t *prof2_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t pmux_sel[] = /* Post mux selectors. */
        {
            POST_EXTRACTOR_MUX_0_SELf,
            POST_EXTRACTOR_MUX_1_SELf,
            POST_EXTRACTOR_MUX_2_SELf,
            POST_EXTRACTOR_MUX_3_SELf,
            POST_EXTRACTOR_MUX_4_SELf,
            POST_EXTRACTOR_MUX_5_SELf,
            POST_EXTRACTOR_MUX_6_SELf,
            POST_EXTRACTOR_MUX_7_SELf,
            POST_EXTRACTOR_MUX_8_SELf,
            POST_EXTRACTOR_MUX_9_SELf,
            POST_EXTRACTOR_MUX_10_SELf,
            POST_EXTRACTOR_MUX_11_SELf,
            POST_EXTRACTOR_MUX_12_SELf,
            POST_EXTRACTOR_MUX_13_SELf,
            POST_EXTRACTOR_MUX_14_SELf
        };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof2_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set Post MUX extractor selcode values. */
    for (idx = 0; idx < 15; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].pmux_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof2_entry, pmux_sel[idx],
                fg->ext_codes[part].pmux_sel[idx]);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profile1_entry_pack
 * Purpose:
 *     Build a LT keygen program profile1 table entry.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part         - (IN)     Entry part number.
 *     mem          - (IN)     Memory name identifier.
 *     prof1_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profile1_entry_pack(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg, int part, soc_mem_t mem,
                               ifp_key_gen_program_profile_entry_t *prof1_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t l1_32_sel[] = /* 32 bit extractors. */
        {
            L1_E32_SEL_0f,
            L1_E32_SEL_1f,
            L1_E32_SEL_2f,
            L1_E32_SEL_3f
        };
    static const soc_field_t l1_16_sel[] = /* 16 bit extractors. */
        {
            L1_E16_SEL_0f,
            L1_E16_SEL_1f,
            L1_E16_SEL_2f,
            L1_E16_SEL_3f,
            L1_E16_SEL_4f,
            L1_E16_SEL_5f,
            L1_E16_SEL_6f
        };
    static const soc_field_t l1_8_sel[] = /* 8 bit extractors. */
        {
            L1_E8_SEL_0f,
            L1_E8_SEL_1f,
            L1_E8_SEL_2f,
            L1_E8_SEL_3f,
            L1_E8_SEL_4f,
            L1_E8_SEL_5f,
            L1_E8_SEL_6f
        };
    static const soc_field_t l1_4_sel[] = /* 4 bit extractors. */
        {
            L1_E4_SEL_0f,
            L1_E4_SEL_1f,
            L1_E4_SEL_2f,
            L1_E4_SEL_3f,
            L1_E4_SEL_4f,
            L1_E4_SEL_5f,
            L1_E4_SEL_6f,
            L1_E4_SEL_7f
        };
    static const soc_field_t l1_2_sel[] = /* 2 bit extractors. */
        {
            L1_E2_SEL_0f,
            L1_E2_SEL_1f,
            L1_E2_SEL_2f,
            L1_E2_SEL_3f,
            L1_E2_SEL_4f,
            L1_E2_SEL_5f,
            L1_E2_SEL_6f,
            L1_E2_SEL_7f
        };
    static const soc_field_t l2_16_sel[] = /* 16 bit extractors. */
        {
            L2_E16_SEL_0f,
            L2_E16_SEL_1f,
            L2_E16_SEL_2f,
            L2_E16_SEL_3f,
            L2_E16_SEL_4f,
            L2_E16_SEL_5f,
            L2_E16_SEL_6f,
            L2_E16_SEL_7f,
            L2_E16_SEL_8f,
            L2_E16_SEL_9f
        };
    static const soc_field_t l3_4_sel[] = /* 4 bit extractors. */
        {
            L3_E4_SEL_0f,
            L3_E4_SEL_1f,
            L3_E4_SEL_2f,
            L3_E4_SEL_3f,
            L3_E4_SEL_4f,
            L3_E4_SEL_5f,
            L3_E4_SEL_6f,
            L3_E4_SEL_7f,
            L3_E4_SEL_8f,
            L3_E4_SEL_9f,
            L3_E4_SEL_10f,
            L3_E4_SEL_11f,
            L3_E4_SEL_12f,
            L3_E4_SEL_13f,
            L3_E4_SEL_14f,
            L3_E4_SEL_15f,
            L3_E4_SEL_16f,
            L3_E4_SEL_17f,
            L3_E4_SEL_18f,
            L3_E4_SEL_19f,
            L3_E4_SEL_20f
        };
    static const soc_field_t l3_2_sel[] = /* 2 bit extractors. */
        {
            L3_E2_SEL_0f,
            L3_E2_SEL_1f,
            L3_E2_SEL_2f,
            L3_E2_SEL_3f,
            L3_E2_SEL_4f
        };
    static const soc_field_t l3_1_sel[] = /* 1 bit extractors. */
        {
            L3_E1_SEL_0f,
            L3_E1_SEL_1f
        };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof1_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e32_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_32_sel[idx],
                fg->ext_codes[part].l1_e32_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_16_sel[idx],
                fg->ext_codes[part].l1_e16_sel[idx]);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e8_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_8_sel[idx],
                fg->ext_codes[part].l1_e8_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_4_sel[idx],
                fg->ext_codes[part].l1_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_2_sel[idx],
                fg->ext_codes[part].l1_e2_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_16_sel[idx],
                fg->ext_codes[part].l2_e16_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 21; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_4_sel[idx],
                fg->ext_codes[part].l3_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_2_sel[idx],
                fg->ext_codes[part].l3_e2_sel[idx]);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e1_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_1_sel[idx],
                fg->ext_codes[part].l3_e1_sel[idx]);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_group_keygen_profiles_index_alloc
 * Purpose:
 *     Install LT keygen program profile table entries in hardware.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part_index   - (IN)     Entry part number.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_keygen_profiles_index_alloc(int unit,
                                           _field_stage_t *stage_fc,
                                           _field_group_t *fg, int part_index)
{
    ifp_key_gen_program_profile_entry_t ifp_prof1_entry;  
                                     /* IFP Keygen profile1. */
    ifp_key_gen_program_profile2_entry_t ifp_prof2_entry; 
                                     /* IFP Keygen profile2. */
    exact_match_key_gen_program_profile_entry_t em_prof_entry;
                                     /* Exact Match Profile. */
    exact_match_key_gen_mask_entry_t em_prof_mask_entry;
                                     /* Exact Match Mask Profile. */
    void *entries[2];                /* Entries array.   */
    soc_mem_t mem[2];                /* Keygen profile memory identifier. */
    int rv;                          /* Operation return status.          */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Get keygen program profiles memory name identifer. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_profiles_mem_get(unit, stage_fc, fg,
                    mem, COUNTOF(mem)));

        /* Clear keygen profile entry. */
        sal_memcpy(&ifp_prof1_entry, soc_mem_entry_null(unit, mem[0]),
                soc_mem_entry_words(unit, mem[0]) * sizeof(uint32));

        /* Clear keygen profile2 entry. */
        sal_memcpy(&ifp_prof2_entry, soc_mem_entry_null(unit, mem[1]),
                soc_mem_entry_words(unit, mem[1]) * sizeof(uint32));

        /* Construct keygen profile1 table entry. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_profile1_entry_pack(unit, stage_fc, fg,
                    part_index, mem[0], &ifp_prof1_entry));

        /* Construct keygen profile2 table entry. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_profile2_entry_pack(unit, stage_fc, fg,
                    part_index, mem[1], &ifp_prof2_entry));

        /* Initialize entries array pointers. */
        entries[0] = &ifp_prof1_entry;
        entries[1] = &ifp_prof2_entry;

        /* Add entries to profile tables in hardware. */
        rv = soc_profile_mem_add(unit,
                &stage_fc->keygen_profile[fg->instance].profile,
                entries, 1, &fg->ext_codes[part_index].keygen_index);
    
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {

        BCM_IF_ERROR_RETURN(_field_th_keygen_profiles_mem_get(unit, stage_fc, fg,
                    mem, COUNTOF(mem)));

        /* Clear keygen profile entry. */
        sal_memcpy(&em_prof_entry, soc_mem_entry_null(unit, mem[0]),
                soc_mem_entry_words(unit, mem[0]) * sizeof(uint32));

        sal_memcpy(&em_prof_mask_entry, soc_mem_entry_null(unit, mem[1]),
                soc_mem_entry_words(unit, mem[1]) * sizeof(uint32));

        /* Construct keygen profile table entry. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_em_profile_entry_pack(unit, stage_fc, fg,
                    part_index, mem[0], &em_prof_entry));

        /* Construct keygen mask entry */
        BCM_IF_ERROR_RETURN(_field_th_keygen_em_profile_mask_entry_pack(unit, stage_fc, fg,
                    part_index, mem[1], &em_prof_mask_entry));

        /* Initialize entries array pointers. */
        entries[0] = &em_prof_entry;
        entries[1] = &em_prof_mask_entry;

        /* Add entries to profile tables in hardware. */
        rv = soc_profile_mem_add(unit,
                &stage_fc->keygen_profile[fg->instance].profile,
                entries, 1, &fg->ext_codes[part_index].keygen_index);

    } else {
        /* Invalid Stage */
        rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_entry_default_rule_init
 * Purpose:
 *    Create a default rule for the group in logical table selection TCAM with
 *    appropriate key, mask and data values.
 * Parameters:
 *     unit     - (IN)      BCM device number.
 *     stage_fc - (IN)      Stage field control structure.
 *     lt_f_ent - (IN/OUT)  Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_rule_init(int unit, _field_stage_t *stage_fc,
                                     _field_lt_entry_t *lt_f_ent)
{
    _field_group_t *fg;   /* Field group structure.       */
    int parts_count = -1; /* Number of field entry parts. */
    int idx;              /* Entry parts iterator.        */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get entry's group structure. */
    fg = lt_f_ent->group;

    /* Get number of entry parts using group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Install entry parts in hardware. */
    for (idx = 0; idx < parts_count; idx++) {
        /*
         * Install keygen program profile table entries for the group in
         * hardware.
         */
        BCM_IF_ERROR_RETURN(_bcm_field_th_group_keygen_profiles_index_alloc(
                                                  unit, stage_fc, fg, idx));
        /*
         * Build LT selection TCAM default entry.
         */
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_default_data_value_set(unit,
                            stage_fc, fg, idx, lt_f_ent + idx));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_default_tcam_entry_install
 * Purpose:
 *     Install a logical table entry in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Field logical table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_tcam_entry_install(int unit, bcm_field_entry_t lt_entry,
                                        int group_expand)
{
    _field_stage_t *stage_fc;           /* Stage Field Control structure.  */
    int tcam_idx[_FP_MAX_ENTRY_WIDTH];  /* Entry TCAM index.               */
    int parts_count = 0;                /* Field entry parts count.        */
    _field_lt_entry_t *lt_f_ent;        /* Field LT entry pointer.         */
    _field_lt_slice_t *lt_part_fs;      /* Field LT entry pointer.         */
    _field_control_t *fc;               /* Field control pointer.          */
    int idx;                            /* Index iterator.                 */
    int rv = BCM_E_NONE;                /* Operation return status.        */
    soc_mem_t lt_tcam_mem;              /* TCAM memory ID.                 */
    uint8          valid;               /* Entry Valid bit.                */

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit , &fc));

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN
         (_field_th_lt_entry_get_by_id(unit, lt_entry, &lt_f_ent));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
        lt_f_ent->group->stage_id, &stage_fc));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    for (idx = parts_count -1; idx >= 0; idx--) {
        lt_part_fs = (lt_f_ent + idx)->lt_fs;

        if (group_expand == 1) {
           /* move to last slice. */
           for (;lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
               if (lt_part_fs->next == NULL) {
                  break;
               }
           }
        }
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_tcam_idx_get(unit,
            lt_f_ent + idx, lt_part_fs, tcam_idx + idx));
    }

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

    valid = (lt_f_ent->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 1 : 0;

    /* Install all parts of the entry in hardware. */
    for (idx = parts_count - 1; idx >= 0; idx--) {
       /*
        * Maximum index value for entry in this TCAM is limited to
        * No. of slices * No. of LT_IDs/slice (12 * 32 == 384).
        */
        if ((tcam_idx[idx] < soc_mem_index_min(unit, lt_tcam_mem)) ||
            (tcam_idx[idx] > (stage_fc->lt_tcam_sz - 1) )) {
           return (BCM_E_INTERNAL);
        }

 
        rv = _bcm_field_th_lt_entry_hw_install(unit, lt_tcam_mem, tcam_idx[idx],
                                               (lt_f_ent + idx)->tcam.key,
                                               (lt_f_ent + idx)->tcam.mask,
                                               (lt_f_ent + idx)->tcam.data,
                                               valid);
        if (BCM_FAILURE(rv)) {
           return (BCM_E_INTERNAL);
        }
        lt_f_ent[idx].flags &= ~_FP_ENTRY_DIRTY;
        lt_f_ent[idx].flags |= _FP_ENTRY_INSTALLED;
        lt_f_ent[idx].flags |= _FP_ENTRY_ENABLED;
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_default_entry_create
 * Purpose:
 *    Create an entry in the slice/s reserved in logical table selection TCAM
 *    for the field group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 *     lt_entry     - (OUT) Logical Table field entry identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_create(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, _field_lt_slice_t *lt_fs,
                                  bcm_field_entry_t *lt_entry)
{
    _field_control_t *fc;        /* Field control structure.    */
    _field_lt_entry_t *lt_f_ent; /* LT field entry pointer.     */
    int rv;                      /* Operation return status.    */

    /* Input parameter check. */
    if ((NULL == lt_entry) || (NULL == stage_fc) || (NULL == fg)
         || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get device field control handle. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Generate an LT entry ID. */
    fc->last_allocated_lt_eid++;
    while (BCM_SUCCESS(_field_th_lt_entry_get(unit, fc->last_allocated_lt_eid,
                _FP_ENTRY_PRIMARY, &lt_f_ent))) {
        fc->last_allocated_lt_eid++;
        if (_FP_ID_MAX == fc->last_allocated_lt_eid) {
            /* Initialize base LT Entry ID*/
            fc->last_allocated_lt_eid = _FP_ID_BASE;
        }
    }

    /* Update allocated entry ID to the caller. */
    *lt_entry = fc->last_allocated_lt_eid;

    /* Create entry in primary slice. */
    rv = _field_th_lt_default_entry_phys_create(unit, stage_fc, *lt_entry,
            BCM_FIELD_ENTRY_PRIO_LOWEST, lt_fs, fg, &lt_f_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set up default LT selection rule based on group's keygen codes. */
    rv = _field_th_lt_entry_default_rule_init(unit, stage_fc, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        /* Destroy resources allocated for the entry. */
        _field_th_lt_default_entry_phys_destroy(unit, lt_f_ent);
        return (rv);
    }
    return (rv);
}

/*
 * Function:
 *    _field_th_ingress_selcodes_install
 * Purpose:
 *    Install Logical table selection rule and TCAM key generation rules in
 *    hardware.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_selcodes_install(int unit, _field_stage_t *stage_fc,
                                   _field_group_t *fg, _field_lt_slice_t *lt_fs)
{
    int   rv;            /* Operation return status.    */
    int   idx;           /* iteration index.            */
    int   inst;          /* Instance.                   */
    _field_control_t *fc; /* Field Control Structure.   */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       _field_presel_entry_t  *f_presel;

       /*
        * Create Presel Entries in the LT slice associated with the group
        */
       for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
           if (fg->presel_ent_arr[idx] != NULL) {
              f_presel = fg->presel_ent_arr[idx];
           } else {
              continue;
           }
          
           /* Allocate the presel entries in the LT slice for this group. */
           rv = _bcm_field_th_lt_entry_config_set(unit, stage_fc, fg, lt_fs,
                                                  f_presel);
           BCM_IF_ERROR_RETURN(rv);

           /* Install the LT Entry in HW. */
           rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel);
           BCM_IF_ERROR_RETURN(rv);
 
           /* Check if the group is expanded across slices. */
           if (f_presel->lt_fs->next != NULL) {
              _field_lt_slice_t     *lt_part_fs;
              _field_presel_entry_t *f_presel_p;
              int                   p, parts_count = -1;

              /* Get number of entry parts. */
              rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                        &parts_count);
              BCM_IF_ERROR_RETURN(rv);

              f_presel_p = f_presel;
              for (p = 0; (p < parts_count && f_presel_p != NULL); p++) {
                  lt_part_fs = f_presel_p->lt_fs->next;
                  for (;lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
                     rv = _field_th_ingress_group_expand_install(unit, stage_fc,
                                                             fg, p, lt_part_fs);
                     BCM_IF_ERROR_RETURN(rv);
                  }
              } 
           }
       }
    } else {
       bcm_field_entry_t lt_entry; /* LT entry ID. */

       /* Create a default LT entry for this group. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_create(unit, stage_fc, fg,
                                                             lt_fs, &lt_entry));
       /* Store LT Entry ID in group LT information structure. */
       if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
           for (inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
               fc->lt_info[inst][fg->lt_id]->lt_entry = lt_entry;
           }
       } else {
           fc->lt_info[fg->instance][fg->lt_id]->lt_entry = lt_entry;
       }

       /* Install the LT entry in hardware. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_install(unit, lt_entry));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_ltmap_unused_resource_get
 * Purpose:
 *     Get unused LT IDs and LT action priorities information.
 * Parameters:
 *     unit                 - (IN) BCM device number.
 *     stage_fc             - (IN) Field stage control structure.
 *     ltmap                - (IN) Logical table configuration structure.
 *     unused_ltids         - (OUT) Unused Logical Table IDs.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_ltmap_unused_resource_get(int unit,
                                 _field_stage_t *stage_fc,
                                 _field_group_t *fg,
                                 uint32 *unused_ltids)
{
    int lt_idx;                 /* Logical table iterator. */
    uint32 used_ltids = 0;      /* Used Logical Table IDs. */
    int inst;                   /* Instance.               */
    _field_control_t   *fc;     /* Field Control Strucutre.*/

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == unused_ltids)) {
        return (BCM_E_PARAM);
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Based on stage oper mode, derive used ltid */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for (inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            /* Iterate over all supported LT IDs. */
            for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
                /* Skip unused LT IDs. */
                if (FALSE == fc->lt_info[inst][lt_idx]->valid) {
                    continue;
                }
                /* Update bit value in the used bitmaps. */
                used_ltids |= (1 << lt_idx);
            }
        }
    } else {
        /* Iterate over all supported LT IDs. */
        for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
            /* Skip unused LT IDs. */
            if (FALSE == fc->lt_info[fg->instance][lt_idx]->valid) {
                continue;
            }
            /* Update bit value in the used bitmaps. */
            used_ltids |= (1 << lt_idx);
        }
    }

    /* Unused caller requested information. */
    *unused_ltids = ~(used_ltids);
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ltid_alloc
 * Purpose:
 *    Allocate a logical table identifier from unused logical table ID pool.
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    max_ltid          - (IN) Logical Table ID maximum value possible.
 *    lt_unused_bmap    - (IN) Unused Logical Table IDs.
 *    lt_id             - (OUT) Logical Table Identifier.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_ltid_alloc(int unit, int max_ltid, uint32 *lt_unused_bmap, int *lt_id)
{
    int lt_idx; /* Logical table iterator. */

    /* Input parameters check. */
    if (NULL == lt_unused_bmap || NULL == lt_id) {
        return (BCM_E_PARAM);
    }

    /* If all LT_IDs have been assigned, return resource error. */
    if (0 == *lt_unused_bmap) {
        return (BCM_E_RESOURCE);
    }

    /* Iterate over unused LT_IDs bit array. */
    for (lt_idx = 0; lt_idx < max_ltid; lt_idx++) {
        if (*lt_unused_bmap & (1 << lt_idx)) {
            break;
        }
    }

    /* Mark selected LT_ID as used. */
    *lt_unused_bmap &= ~(1 << lt_idx);

    /* Update LT_ID value in caller argument. */
    *lt_id = lt_idx;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_prio_cmp
 * Purpose:
 *     Compare two group priorities linked to logical tables.
 * Parameters:
 *     a - (IN) First logical table group priority.
 *     b - (IN) Second logical table group priority.
 * Returns:
 *     -1 if prio_first <  prio_second
 *     0 if prio_first == prio_second
 *     1 if prio_first >  prio_second
 */
STATIC int
_field_th_lt_prio_cmp(void *a, void *b)
{
    _field_lt_config_t *first;  /* First Logical table configuration.   */
    _field_lt_config_t *second; /* Second Logical table configuration.  */

    first = (_field_lt_config_t *)a;
    second = (_field_lt_config_t *)b;

    if (first->priority < second->priority) {
        return (-1);
    } else if (first->priority > second->priority) {
        return (1);
    }

    return (0);
}

/*
 * Function:
 *    _field_th_lt_priority_alloc
 * Purpose:
 *    Allocate logical table action priority for the group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *    fg        - (IN) State machine tracking structure.
 *    lt_id     - (IN) Logical Table Identifier.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_priority_alloc(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg, int lt_id)
{
    int lt_idx;                                 /* Logical table iterator.   */
    _field_lt_config_t lt_info[_FP_MAX_NUM_LT]; /* LT information.           */
    int action_prio = (_FP_MAX_NUM_LT - 1);     /* LT action priority value. */
    int idx;                                    /* Iterator.                 */
    _field_control_t *fc;                       /* Field Control Strucutre.  */
    int inst;                                   /* Instance.                 */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for (inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            /* Get LT information for the group. */
            for (idx = 0; idx < _FP_MAX_NUM_LT; idx++) {
                sal_memcpy(&lt_info[idx], fc->lt_info[inst][idx],
                        sizeof(_field_lt_config_t));
            }

            /*
             * Initialize LT information for the new logical table ID, for it to be
             * in the correct location after the sort.
             */
            lt_info[lt_id].priority = fg->priority;
            lt_info[lt_id].lt_action_pri = 0;
            lt_info[lt_id].valid = TRUE;

            for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d): Verb-B4-Sort: lt_id(%d): lt_grp_prio(%d): "
                   "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                    unit, lt_info[lt_idx].lt_id,
                    lt_info[lt_idx].priority, lt_info[lt_idx].lt_action_pri,
                    lt_info[lt_idx].valid, lt_info[lt_idx].flags));
            }

            /* Sort the logical tables array based on group priority value. */
            _shr_sort(lt_info, COUNTOF(lt_info), sizeof(_field_lt_config_t),
                    _field_th_lt_prio_cmp);

            for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d): Verb-Aft-Sort: lt_id(%d): lt_grp_prio(%d): "
                   "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                    unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
                    lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
                    lt_info[lt_idx].flags));
            }

            /*
             * Assign LT action priority based on the position of LT ID in the sorted
             * array.
             */
            action_prio = (_FP_MAX_NUM_LT - 1); 
            for (idx = (_FP_MAX_NUM_LT - 1); idx >= 0; idx--) {
                if (FALSE == lt_info[idx].valid) {
                    continue;
                }
                lt_info[idx].lt_action_pri = action_prio--;
            }

            for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d): Verb-Aft-PrioAssign: lt_id(%d): lt_grp_prio(%d): "
                   "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                    unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
                    lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
                    lt_info[lt_idx].flags));
            }

            for (idx = 0; idx < _FP_MAX_NUM_LT; idx++) {
                if (FALSE == lt_info[idx].valid) {
                    continue;
                }
                lt_idx = lt_info[idx].lt_id;
                fc->lt_info[inst][lt_idx]->lt_action_pri =
                    lt_info[idx].lt_action_pri;
            }
        }
    } else {
        /* Get LT information for the group. */
        for (idx = 0; idx < _FP_MAX_NUM_LT; idx++) {
            sal_memcpy(&lt_info[idx], fc->lt_info[fg->instance][idx],
                    sizeof(_field_lt_config_t));
        }

        /*
         * Initialize LT information for the new logical table ID, for it to be
         * in the correct location after the sort.
         */
        lt_info[lt_id].priority = fg->priority;
        lt_info[lt_id].lt_action_pri = 0;
        lt_info[lt_id].valid = TRUE;

        for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d): Verb-B4-Sort: lt_id(%d): lt_grp_prio(%d): "
               "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                unit, lt_info[lt_idx].lt_id,
                lt_info[lt_idx].priority, lt_info[lt_idx].lt_action_pri,
                lt_info[lt_idx].valid, lt_info[lt_idx].flags));
        }

        /* Sort the logical tables array based on group priority value. */
        _shr_sort(lt_info, COUNTOF(lt_info), sizeof(_field_lt_config_t),
                _field_th_lt_prio_cmp);

        for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d): Verb-Aft-Sort: lt_id(%d): lt_grp_prio(%d): "
               "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
                lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
                lt_info[lt_idx].flags));
        }

        /*
         * Assign LT action priority based on the position of LT ID in the sorted
         * array.
         */
        for (idx = (_FP_MAX_NUM_LT - 1); idx >= 0; idx--) {
            if (FALSE == lt_info[idx].valid) {
                continue;
            }
            lt_info[idx].lt_action_pri = action_prio--;
        }

        for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d): Verb-Aft-PrioAssign: lt_id(%d): lt_grp_prio(%d): "
               "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
                unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
                lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
                lt_info[lt_idx].flags));
        }

        for (idx = 0; idx < _FP_MAX_NUM_LT; idx++) {
            if (FALSE == lt_info[idx].valid) {
                continue;
            }
            lt_idx = lt_info[idx].lt_id;
            fc->lt_info[fg->instance][lt_idx]->lt_action_pri =
                lt_info[idx].lt_action_pri;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_group_ltmap_alloc
 * Purpose:
 *    Allocate a Logitcal Table ID for the field group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *    fg        - (IN/OUT) Field group structure pointer.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_ltmap_alloc(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg)
{
    uint32 unused_ltids;                        /* Unused LT IDs.             */
    int lt_id;                                  /* Logical Table Identifier.  */
    _field_control_t *fc;                       /* Field Control Strucutre.   */
    int inst;                                   /* Instance.                  */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get unused LT IDs and LT action priorites. */
    BCM_IF_ERROR_RETURN(_field_ltmap_unused_resource_get(unit, stage_fc,
                                                         fg, &unused_ltids));

    /* Allocate a LT ID from unused LT ID pool. */
    BCM_IF_ERROR_RETURN(_field_th_ltid_alloc(unit, stage_fc->num_logical_tables,
                                                     &unused_ltids, &lt_id));

    /* Allocate LT action priority. */
    BCM_IF_ERROR_RETURN(_field_th_lt_priority_alloc(unit, stage_fc, fg, lt_id));

    /* Set LT ID info in database. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for (inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            fc->lt_info[inst][lt_id]->valid = TRUE;
            fc->lt_info[inst][lt_id]->priority = fg->priority;
            fc->lt_info[inst][lt_id]->flags = fg->flags;
        }
    } else {
        fc->lt_info[fg->instance][lt_id]->valid = TRUE;
        fc->lt_info[fg->instance][lt_id]->priority = fg->priority;
        fc->lt_info[fg->instance][lt_id]->flags = fg->flags;
    }

    /* Get group's LT info structure pointer. */
    fg->lt_id = lt_id;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_group_install
 * Purpose:
 *    Auxiliary routine used to install field group in hardware.
 * Parameters:
 *     unit         - (IN)      BCM device number.
 *     stage_fc     - (IN)      Installed group structure.
 *     fg           - (IN/OUT)  Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_install(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg)
{
    _field_lt_slice_t *lt_fs;   /* LT Slice pointer.                */
    _field_slice_t *fs;         /* Field slice structure pointer.   */
    int parts_count = -1;       /* Number of entry parts.           */
    int part;                   /* Entry parts iterator.            */
    uint8 slice_number;         /* LT Slice number.                 */
    int lt_part_prio = _FP_MAX_LT_PART_PRIO;    
                                /* LT part priority value.          */
    int inst;                   /* Instance.                        */
    _field_control_t *fc;       /* Field control structure.         */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == fg->lt_slices)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Allocate a Logical Table ID for the group. */
    BCM_IF_ERROR_RETURN(_field_th_group_ltmap_alloc(unit, stage_fc, fg));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Iterate over entry parts and program hardware for each slice. */
    for (part = 0; part < parts_count; part++) {

        /* Get slice number to which the entry part belongs to. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(part,
            fg->flags, &slice_number));

        /* Get LT entry part slice control structure. */
        lt_fs = stage_fc->lt_slices[fg->instance] +
                fg->slices->slice_number + slice_number;

        /* Set slice mode in hardware based on Group mode/flags. */
        BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
            lt_fs->slice_number, fg, 0));
        /*
         * Initialize slice free entries count value to number of
         * entries in the slice.
         */
        if (0 == fg->slices[0].lt_map) {
            lt_fs->free_count = lt_fs->entry_count;
        }

        /* Get IFP TCAM slice information. */
        fs = fg->slices + part;

        /* Update slice LT_ID mapping in group's slice. */
        fs->lt_map |= (1 << fg->lt_id);

        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            for(inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
                fc->lt_info[inst][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
                fc->lt_info[inst][fg->lt_id]->lt_part_pri[fs->slice_number] = lt_part_prio;
            }
        } else {
            fc->lt_info[fg->instance][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
            fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = lt_part_prio;
        }
    }

    /* Install TCAM keygen program configuration in hardware for each slice. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_selcodes_install(unit, stage_fc, fg,
                                                           fg->lt_slices));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_group_install
 * Purpose:
 *     Install group slice mode in hardware and update slice parameters based on
 *     the group mode.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_group_install(int unit,
                                  _field_group_add_fsm_t *fsm_ptr)
{
    _field_slice_t *fs;     /* Slice pointer.           */
    _field_group_t *fg;     /* Field Group Pointer.     */
    int parts_count = -1;   /* Number of entry parts.   */
    int idx;                /* Slice iterator.          */
    uint32 entry_flags;     /* Entry flags.             */
    uint8 slice_number;     /* Slice number.            */
    int slice_init;         /* Slice SW init needed.    */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Get field group pointer. */
    fg = fsm_ptr->fg;

    /* Check if this is the first group installed in slice. */
    slice_init = (0 == fg->slices[0].lt_map) ? TRUE : FALSE;

    /* Write group parameters to hardware. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_group_install(unit,
                                     fsm_ptr->stage_fc, fg));

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = parts_count - 1; idx >=0; idx--) {
        /* Get entry flags. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_entry_flags(unit, idx,
            fg->flags, &entry_flags));

        /* Get slice number for given entry part. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        /* Get slice pointer. */
        fs = fg->slices + slice_number;

        if (!(entry_flags & _FP_ENTRY_SECOND_HALF)) {
            if (slice_init) {
                /*
                 * Initialize slice free entries count value based on group
                 * flags.
                 */
                if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
                    fs->free_count = fs->entry_count;
                } else {
                    fs->entry_count >>= 1;
                    fs->free_count = fs->entry_count;
                    fs->start_tcam_idx >>= 1;
                }

                /* Initialize slice group flags. */
                fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_group_add_slice_allocate
 * Purpose:
 *    Allocate slice for a new group.
 * Parameters:
 *    unit     - (IN)     BCM device number.
 *    fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_add_slice_allocate(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t     *fg;           /* Field Group structure pointer. */
    _field_stage_t     *stage_fc;     /* Field Stage control pointer.   */
    int                slice;         /* Slice iterator.                */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Initialize field group pointer. */
    fg = fsm_ptr->fg;

    /* Initialize field stage pointer. */
    stage_fc = fsm_ptr->stage_fc;

    slice = -1;
    /* Reserve a slice for this new Field Group. */
    for (slice = 0; slice < stage_fc->tcam_slices; slice++ ) {
        fsm_ptr->rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                             slice);
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "==> %s(): SLICE[%d] allocated for group[%d].\n\r"),
            __func__, slice, fg->gid));
            break;
        }

        if ((BCM_E_PARAM == fsm_ptr->rv) || (BCM_E_RESOURCE == fsm_ptr->rv)) {
            break;
        }
    }

    /* Check if allocation was successful. */
    if (slice == stage_fc->tcam_slices) {
        fsm_ptr->rv = (BCM_E_RESOURCE);
    }

    if (BCM_FAILURE(fsm_ptr->rv)) {
        if (fsm_ptr->fsm_state_prev != _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS) {
            fsm_ptr->rv = (BCM_E_NONE);
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS;
        } else {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    } else {
        /* Update Group slices pointer. */
        fg->slices = stage_fc->slices[fg->instance] + slice;

        /* Update Group LT slices pointer. */
        fg->lt_slices = stage_fc->lt_slices[fg->instance] + slice;

        /*
         * Install key generation extractor codes and LT selection rule for the
         * group in hardware.
         */
        fsm_ptr->rv = _field_th_group_add_group_install(unit, fsm_ptr);
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP;
        } else {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    }

    /* Set current state as the previous state. */
    fsm_ptr->fsm_state_prev = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;
    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *    _bcm_field_th_entry_tcam_parts_count
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    group_flags - (IN)  Entry group flags.
 *    part_count  - (OUT) Entry parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_entry_tcam_parts_count(int unit, uint32 group_flags,
                                     int *part_count)
{
    /* Input parameters check. */
    if (NULL == part_count) {
        return (BCM_E_PARAM);
    }

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /* 
         * Part value is same for Narrow and Wide TCAMs
         * (IFP_TCAM and IFP_TCAM_WIDE)
         * (group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
         */
        *part_count = 1;
    } else if (group_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        /* IFP_TCAM_WIDE view only. */
        *part_count = 2;
    } else if (group_flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        /* IFP_TCAM_WIDE view only. */
        *part_count = 3;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_slice_offset_to_tcam_idx
 * Purpose:
 *     Given stage, slice number and entry index in the slice, calculate TCAM
 *     index.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field entry structure.
 *     instance - (IN) Field processor pipe instance.
 *     slice    - (IN) Entry slice number.
 *     slice_idx -(IN) Entry offset in the slice.
 *     tcam_idx - (OUT) Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_slice_offset_to_tcam_idx(int unit, _field_stage_t *stage_fc,
                                       int instance, int slice, int slice_idx,
                                       int *tcam_idx)
{
    _field_slice_t *fs; /* Field slice structure pointer. */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == tcam_idx) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check. */
    if (instance < 0 || instance >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Get the slice pointer. */
    fs = stage_fc->slices[instance] + slice;

    /* Given stage slice number and slice index, calcuate TCAM index. */
    *tcam_idx = fs->start_tcam_idx + slice_idx;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_tcam_idx_to_slice_offset
 *
 * Purpose:
 *     Given stage and tcam_index,
 *     calculate slice and entry offset in the
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field stage structure.
 *     f_ent    - (IN) Reference to field entry structure.
 *     tcam_idx - (IN) Entry tcam index.
 *     slice    - (OUT) Entry slice number.
 *     slice_idx -(OUT) Entry offset in the slice.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_tcam_idx_to_slice_offset(int unit, _field_stage_t *stage_fc,
                                       _field_entry_t *f_ent, int tcam_idx,
                                       int *slice, int *slice_idx)
{
    _field_slice_t  *fs;                  /* Field Slice number.  */
    int             idx;                  /* Slice iterator.      */
    int             max_entries_in_slice; /* Max entries supported in a slice.*/

   /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == slice_idx) || (NULL == slice)
        || (NULL == f_ent)) {
        return (BCM_E_PARAM);
    }

    if (!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE) ||
        (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        max_entries_in_slice = 256;
    } else {
        max_entries_in_slice = 512;
    }

    for (idx = 0; idx < stage_fc->tcam_slices; idx++) {
        fs = stage_fc->slices[f_ent->group->instance] + idx;

        if (tcam_idx < ((idx * max_entries_in_slice) + max_entries_in_slice)) {
            *slice = idx;
            *slice_idx = tcam_idx - fs->start_tcam_idx;
            break;
        }
    }

    /* TCAM index sanity check. */
    if (idx == stage_fc->tcam_slices) {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_tcam_part_to_slice_number
 * Purpose:
 *     Each field entry contains up to 3 TCAM entries. This routine maps tcam
 *     entry (0-2) to a slice number
 * Parameters:
 *     entry_part   - (IN)  Entry Part number. 
 *     group_flags  - (IN)  Field Group flags.
 *     slice_number - (OUT) Slice number (0-2)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_tcam_part_to_slice_number(int entry_part, uint32 group_flags,
                                        uint8 *slice_number)
{
    /* Input parameters check. */
    if (NULL == slice_number) {
        return (BCM_E_PARAM);
    }

    switch (entry_part) {
        case 0:
            *slice_number = 0;
            break;
        case 1:
            *slice_number = 1;
            break;
        case 2:
            *slice_number = 2;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_entry_part_tcam_idx_get
 *
 * Purpose:
 *     Given primary entry slice/idx calculate secondary/tertiary slice/index.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry pointer.
 *     idx_pri  - (IN) Primary entry tcam index.
 *     ent_part - (IN) Entry part id.
 *     idx_out  - (OUT) Entry part tcam index.
 * Returns
 *     BCM_E_XXX
 */
int
_bcm_field_th_entry_part_tcam_idx_get(int unit, _field_entry_t *f_ent,
                                      uint32 idx_pri, uint8 entry_part,
                                      int *idx_out)
{
    uint8 slice_num = 0;        /* Primary entry slice number.              */
    int pri_slice = 0;          /* Primary slice number.                    */
    int pri_index = 0;          /* Primary entry.                           */
    int part_index;             /* Entry part slice index.                  */
    _field_stage_t *stage_fc;   /* Field stage control structure pointer.   */
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_group_t *fg;         /* Field Group structure.                   */
    int rv;                     /* Operation return status.                 */

    /* Input parameters check. */
    if (NULL == f_ent || NULL == idx_out) {
        return (BCM_E_PARAM);
    }

    /* Primary entry index. */
    if (0 == entry_part) {
        *idx_out = idx_pri;
        return (BCM_E_NONE);
    }

    fg = f_ent->group;

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get primary part slice and entry index. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
        f_ent, idx_pri, &pri_slice, &pri_index));

    /* Get entry part slice number 0, 1, 2. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(entry_part,
        fg->flags, &slice_num));

    fs = stage_fc->slices[fg->instance] + (pri_slice + slice_num);

    part_index = pri_index;

    /*
     * Given slice number & entry's offset index in slice, calculcate TCAM
     * index.
     */
    rv = _bcm_field_th_slice_offset_to_tcam_idx(unit, stage_fc, fg->instance,
            fs->slice_number, part_index, idx_out);
    return (rv);
}

/*
 * Function:
 *     _field_th_entry_slice_idx_change
 * Purpose:
 *     Move the software entry to a new slice index.
 * Parmeters:
 *     unit         - (IN)      BCM device number.
 *     f_ent        - (IN/OUT)  Field entry to be moved.
 *     parts_count  - (IN)      Number of entry parts count.
 *     tcam_idx_new - (IN)      Entry new TCAM index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_entry_slice_idx_change(int unit, _field_entry_t *f_ent,
                                 int parts_count, int *tcam_idx_new)
{
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_group_t *fg;         /* Field Group structure pointer.           */
    _field_stage_t *stage_fc;   /* Stage Field control structure pointer.   */
    int new_slice_num = 0;      /* Entry new slice number.                  */
    int new_slice_idx = 0;      /* Entry's new offset in the slice.         */
    int part;                   /* Entry parts iterator.                    */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_ent->group) || (NULL == f_ent->fs)) {
        return (BCM_E_PARAM);
    }

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
        &stage_fc));

    /* Get entry group information. */
    fg = f_ent->group;
    for (part = 0; part < parts_count; part++) {
        /* Get slice control structure. */
        fs = f_ent[part].fs;

        if (FALSE == th_prio_set_with_no_free_entries) {
            fs->entries[f_ent[part].slice_idx] = NULL;
        }

        /*
         * Calculate entry new slice number and offset in slice from TCAM index
         * value.
         */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
            f_ent, tcam_idx_new[part], &new_slice_num, &new_slice_idx));

        /* If entry moves across the slice, update free counter.*/
        if (new_slice_num != f_ent[part].fs->slice_number
            && (0 == (f_ent[part].flags & _FP_ENTRY_SECOND_HALF))) {
            fs->free_count++;
            stage_fc->slices[fg->instance][new_slice_num].free_count--;
        }

        /* Update entry structure. */
        stage_fc->slices[fg->instance][new_slice_num].entries[new_slice_idx] =
            f_ent + part;
        f_ent[part].fs = &stage_fc->slices[fg->instance][new_slice_num];
        f_ent[part].slice_idx = new_slice_idx;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_entry_move
 * Purpose:
 *     Move an entry within a slice by "amount" indexes
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - entry to be moved
 *     amount   - number of indexes to move + or -
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_field_th_entry_move(int unit, _field_entry_t *f_ent, int amount)
{
    int tcam_idx_old[_FP_MAX_ENTRY_WIDTH] = {0}; /* Original entry TCAM
                                                    index.          */
    int tcam_idx_new[_FP_MAX_ENTRY_WIDTH] = {0}; /* New destination TCAM
                                                    index.          */
    _field_control_t *fc;       /* Field control structure pointer. */
    _field_stage_t *stage_fc;   /* Field stage structure pointer.   */
    _field_group_t *fg;         /* Field Group structure pointer.   */
    /* _field_slice_t *fs; */   /* Field slice structure pointer.   */
    int parts_count = 0;        /* Number of parts per-entry.       */
    int idx;                    /* Entry parts iterator.            */
    int new_offset;             /* New entry offset in slice.       */
    int new_slice_num = -1;     /* New slice number.                */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_ent->fs) || (NULL == f_ent->group)) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) vverb: BEGIN _field_th_entry_move(entry=%d, amount=%d)\n"),
         unit, f_ent->eid, amount));

    fg = f_ent->group;

    if (0 == amount) {
        LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) warn:  moving entry=%d, same slice_idx=%d(%#x)\n"),
             unit, f_ent->eid, f_ent->slice_idx, f_ent->slice_idx));
        return (BCM_E_NONE);
    }

    /* Get Field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get Stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get number of parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = 0; idx < parts_count; idx++) {
        /* Calculate entry parts OLD TCAM indices. */
        BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_idx_get(unit, f_ent + idx,
            &tcam_idx_old[idx]));

        /* Calculate entry part tcam offset. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_entry_part_tcam_idx_get(unit, f_ent,
            tcam_idx_old[0] + amount, idx, &tcam_idx_new[idx]));
    }

    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
        f_ent, tcam_idx_new[0], &new_slice_num, &new_offset));

    if ((new_slice_num != f_ent->fs->slice_number)
         && (f_ent->flags & _FP_ENTRY_INSTALLED)) {
        /* fs = stage_fc->slices[fg->instance] + new_slice_num; */

        /* Move the entry in hardware if it's already installed in HW. */
        if (f_ent->flags & _FP_ENTRY_INSTALLED) {
            BCM_IF_ERROR_RETURN(fc->functions.fp_entry_move(unit, f_ent,
                parts_count, tcam_idx_old, tcam_idx_new));
        }
    }
    /* Move the SW entry to the new slice index. */
    BCM_IF_ERROR_RETURN(_field_th_entry_slice_idx_change(unit, f_ent,
        parts_count, tcam_idx_new));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_slice_is_empty
 * Purpose:
 *     Report if a slice has any entries.
 * Parameters:
 *     fc       - (IN)  Field control structure.
 *     fs       - (IN)  Slice control structure.
 *     empty    - (OUT) True - slice is empty/False otherwise.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_slice_is_empty(int unit, _field_slice_t *fs, uint8 *empty)
{
    /* Input parameters check. */
    if (NULL == fs || NULL == empty) {
        return (BCM_E_PARAM);
    }

    /* Check if total entries count is equal to the free count. */
    if (fs->entry_count == fs->free_count) {
        /* Slice has not valid group entries. */
        *empty = TRUE;
    } else {
        /* Slice has atleast one valid entry. */
        *empty = FALSE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_slice_clear
 *
 * Purpose:
 *    Reset the fields in a slice. Note that the entries list must be
 *    empty before calling this. Also, this does NOT deallocate the memory for
 *    the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit  - (IN)     BCM device number
 *    fg    - (IN)     Field group structure.
 *    fs    - (IN/OUT) Link to physical slice structure to be cleared
 *
 * Returns:
 *    BCM_E_NONE    - Success
 *    BCM_E_BUSY    - Entries still in slice, can't clear slice
 */
STATIC int
_field_th_ingress_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    _field_control_t *fc;       /* Field control structure.         */
    uint32 entry_idx;           /* Slice entries iterator.          */
    bcm_port_t port;            /* Port iterator.                   */
    int inst;                   /* Instance.                        */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == fs)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fs->stage_id,
        &stage_fc));

    for (entry_idx = 0; entry_idx < fs->entry_count; entry_idx++) {
        if ((NULL != fs->entries[entry_idx])
             && (fs->entries[entry_idx]->group->gid == fg->gid)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Entries still in slice=%d.\n"),
                 unit, fs->slice_number));
            return (BCM_E_BUSY);
        }
    }

    /* Remove ports from slice's port bitmap. */
    BCM_PBMP_ITER(fg->pbmp, port) {
        BCM_PBMP_PORT_REMOVE(fs->pbmp, port);
    }

    /* Clear Group's LT_ID bit in slice LT map. */
    fs->lt_map &= ~(1 << fg->lt_id);

    /* Remove slice from LT ID partition MAP. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for(inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            fc->lt_info[inst][fg->lt_id]->lt_part_map &= ~(1 << fs->slice_number);
            /* Set slice partition priority to hardware reset default value. */
            fc->lt_info[inst][fg->lt_id]->lt_part_pri[fs->slice_number] = 0;
        }
    } else {
        fc->lt_info[fg->instance][fg->lt_id]->lt_part_map &= ~(1 << fs->slice_number);
        fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = 0;
    }

    /*  Initialize entries per-slice count. */
    fs->entry_count = stage_fc->tcam_sz / stage_fc->tcam_slices;

    /* Initialize free entries count. */
    fs->free_count = fs->entry_count;

    /* Initialize start TCAM index. */
    fs->start_tcam_idx = fs->slice_number * fs->entry_count;

    /* Clear group flags settings. */
    fs->group_flags = 0;

    return (BCM_E_NONE);
}
/*
 * Function:
 *    _bcm_field_th_ingress_slice_clear
 *
 * Purpose:
 *    Reset the fields in a slice. Note that the entries list must be
 *    empty before calling this. Also, this does NOT deallocate the memory for
 *    the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit  - (IN)     BCM device number
 *    fg    - (IN)     Field group structure.
 *    fs    - (IN/OUT) Link to physical slice structure to be cleared
 *
 * Returns:
 *    BCM_E_NONE    - Success
 *    BCM_E_BUSY    - Entries still in slice, can't clear slice
 */
int
_bcm_field_th_ingress_slice_clear(int unit, 
                                  _field_group_t *fg, 
                                  _field_slice_t *fs)
{
    return _field_th_ingress_slice_clear(unit, fg, fs);
}

/*
 * Function:
 *    _bcm_field_th_lt_default_slice_clear
 *
 * Purpose:
 *    Reset the default entries in a slice. Note that the entries list must be
 *    empty before calling this. Also, this does NOT deallocate the memory for
 *    the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit      - (IN) BCM device number
 *    fg        - (IN) Field group structure.
 *    slice_num - (IN) Slice to be reset.
 *
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
_bcm_field_th_lt_default_slice_clear(int unit, _field_group_t *fg,
                                     int slice_num)
{
    _field_lt_entry_t    *lt_f_entry;
    _field_lt_entry_t    *lt_f_entry_p = NULL;
    int                   rv;
    int                   tcam_idx;
    int                   part, parts = -1; 
    _field_stage_t        *stage_fc;
    _field_lt_slice_t     *lt_fs, *lt_part_fs = NULL;
    soc_mem_t             lt_tcam_mem;
    uint32                entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /*Entry buffer*/
    _field_control_t *fc;
    bcm_field_entry_t lt_entry;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Find the LT Slice to be cleared. */
    lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;


    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
    } else {
        lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;
    }
    
    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit,
                        lt_entry, &lt_f_entry));
    if (lt_f_entry->lt_fs == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Get entry parts count. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                             &parts));
    for (part = 0; part < parts; part++) {
        lt_f_entry_p = lt_f_entry + part;
        lt_part_fs = lt_f_entry_p->lt_fs;
        for (;lt_part_fs != NULL;lt_part_fs=lt_part_fs->next) {
            if (lt_part_fs == lt_fs) {
                lt_part_fs->free_count++;
                lt_part_fs->entries[lt_f_entry_p->index] = NULL;
                if (lt_part_fs->free_count == lt_part_fs->entry_count) {
                    if (lt_part_fs->prev != NULL) {
                       lt_part_fs->prev->next = lt_part_fs->next;
                    } else {
                       lt_f_entry_p->lt_fs = lt_part_fs->next;
                       if (fg->lt_slices == lt_fs) {
                          fg->lt_slices = fg->lt_slices->next;
                       }
                    }
                    if (lt_part_fs->next != NULL) {
                       lt_part_fs->next->prev = lt_part_fs->prev;
                    }
                } else {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Error: Unable to detach the slice:%d as slice is not"
                     " empty, free_count:%d entry_count:%d\n\r"),
                      lt_part_fs->slice_number, lt_part_fs->free_count,
                      lt_part_fs->entry_count));
                    return BCM_E_INTERNAL;
                }
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "%s(): DETACHED SLICE:%d lt_part_fs:%p\n\r\n\r"),
                        __func__,lt_part_fs->slice_number, lt_part_fs));
                break;
            }
        }

        if (lt_part_fs != NULL) {
           break;
        }
    }

    if (lt_part_fs == NULL) {
       return BCM_E_INTERNAL;
    }

    rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_entry_p, lt_fs,
                                          &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);
 
    /* Get TCAM view to be used for programming the hardware */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc,
                                       lt_f_entry, &lt_tcam_mem));

    /* Clear the entry in hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                       tcam_idx, entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_th_lt_slice_clear
 *
 * Purpose:
 *    Reset the Preselector entries in a slice. Note that the entries list must
 *    be empty before calling this. Also, this does NOT deallocate the memory
 *    for the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit      - (IN) BCM device number
 *    fg        - (IN) Field group structure.
 *    slice_num - (IN) Slice to be reset.
 *
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
_bcm_field_th_lt_slice_clear(int unit, _field_group_t *fg, int slice_num)
{
    int                   rv, idx;
    int                   tcam_idx;
    int                   part, parts = -1; 
    _field_stage_t        *stage_fc;
    _field_lt_slice_t     *lt_fs;
    _field_lt_slice_t     *lt_part_fs = NULL;
    _field_presel_entry_t *f_presel,
                          *f_presel_p;
    soc_mem_t             lt_tcam_mem;
    uint32                entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /*Entry buffer*/

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Clear the slice for default groups */
    if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
       return _bcm_field_th_lt_default_slice_clear(unit, fg, slice_num); 
    }
 
    if (fg->presel_ent_arr[0] == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Find the LT Slice to be cleared. */
    lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;

    /* Get entry parts count. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                             &parts));

    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        if (fg->presel_ent_arr[idx] != NULL) {
           f_presel = fg->presel_ent_arr[idx];
        } else {
           continue;
        }

        f_presel_p = f_presel;
        for (part = 0; (part < parts && f_presel_p != NULL); part++) {
            lt_part_fs = f_presel_p->lt_fs;
            
            for (; lt_part_fs != NULL; lt_part_fs=lt_part_fs->next) {
                if (lt_part_fs == lt_fs) {
                   lt_part_fs->free_count++;
                   lt_part_fs->p_entries[f_presel->hw_index] = NULL;
                   if (lt_part_fs->free_count == lt_part_fs->entry_count) {
                      if (lt_part_fs->prev != NULL) {
                         lt_part_fs->prev->next = lt_part_fs->next;
                      } else {
                         f_presel_p->lt_fs = lt_part_fs->next;
                         if (fg->lt_slices == lt_fs) {
                            fg->lt_slices = lt_part_fs->next;
                         }
                      }

                      if (lt_part_fs->next != NULL) {
                         lt_part_fs->next->prev = lt_part_fs->prev;
                      }
                   } else {
                      LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "----> DELETED PRESEL:%d GROUP:%d SLICE:%d PART:%d lt_part_fs:%p\n\r\n\r"),
                        f_presel_p->presel_id, fg->gid,
                      lt_part_fs->slice_number, part, lt_part_fs));
                      break;
                   }
                   LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "====> DELETED PRESEL:%d GROUP:%d DETACHED SLICE:%d PART:%d lt_part_fs:%p\n\r\n\r"),
                        f_presel_p->presel_id, fg->gid, lt_part_fs->slice_number, part, lt_part_fs));
                   break;
                }
            }
            if (lt_part_fs != NULL) {
               break;
            }
            f_presel_p = f_presel_p->next;
        }

        if (lt_part_fs == NULL) {
           return BCM_E_INTERNAL;
        }

        rv = _bcm_field_presel_entry_tcam_idx_get(unit, f_presel_p, lt_fs,
                                                  &tcam_idx);
        BCM_IF_ERROR_RETURN(rv);
    
        /* Get TCAM view to be used for programming the hardware */
        if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                            f_presel->group->instance,
                                           _BCM_FIELD_MEM_TYPE_IFP_LT,
                              _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                 &lt_tcam_mem, NULL));
        } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
            rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, 
                                            f_presel->group->instance,
                                             _BCM_FIELD_MEM_TYPE_EM_LT,
                                         _BCM_FIELD_MEM_VIEW_TYPE_TCAM,
                                                  &lt_tcam_mem, NULL));
        } else {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(rv);

        /* Clear the entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                          tcam_idx, entry));

    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_th_group_free_unused_slices
 * Purpose:
 *     Unallocate group unused slices.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     stage_fc       - (IN) Stage field control structure.
 *     fg             - (IN) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_free_unused_slices(int unit,
                                       _field_stage_t *stage_fc,
                                       _field_group_t *fg)
{
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_slice_t *fs_next;    /* Field slice structure pointer.           */
    _field_slice_t *fs_ptr;     /* Field slice structure pointer.           */
    uint8 empty;                /* Slice is empty flag.                     */
    int slice;                  /* Group slices iterator.                   */
    int count;                  /* Group slices span count.                 */
    _field_group_t *fg_next = NULL;
    _field_group_t *fg_list[32];
    int group_ct = 0, ct;
    
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (NULL == fg->slices) {
        return (BCM_E_INTERNAL);
    }

    fs = fg->slices;
    /* Don't free the slice if it is the last slice in the group */
    if (fs->next == NULL) {
       return BCM_E_NONE;
    }

    while (NULL != fs) {
       fs_next = fs->next;
       BCM_IF_ERROR_RETURN(_field_th_slice_is_empty(unit, fs, &empty));
       
       if (empty) {
           BCM_IF_ERROR_RETURN(_bcm_field_group_slice_count_get(fs->group_flags,
               &count));

           for (slice = 0; slice < count; slice++) {
                /* Remove slice from Group's slices linked list. */
                fs_ptr = fs + slice;
                      
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "%s(): SLICE TO BE detached SLICE:%d fs_ptr:%p fg_slice:%p"
                     " fg_slice_num:%d\n\r\n\r"), __func__,
                     fs_ptr->slice_number, fs_ptr, fg->slices,
                     (fg->slices!=NULL)?fg->slices->slice_number:-1));

                do {
                   /* Retrieve the existing groups associated with the Slice */
                   (void)_bcm_field_th_slice_group_get_next(unit,
                                                    fg->instance, fg->stage_id,
                                                    fs->slice_number,
                                                    &fg_next, &fg_next);
                   if (fg_next != NULL) {
                      /* Reset the partition priority for the slice. */
                      BCM_IF_ERROR_RETURN(_bcm_field_th_lt_part_prio_reset(unit,
                                                    stage_fc, fg_next, fs_ptr));
                      /* Clear unused LT slice settings. */
                      BCM_IF_ERROR_RETURN(_bcm_field_th_lt_slice_clear(unit,
                                            fg_next, fs_ptr->slice_number));
                      /* Clear unused slice settings. */
                      BCM_IF_ERROR_RETURN(_field_th_ingress_slice_clear(unit,
                                                           fg_next, fs_ptr));
                      fg_list[group_ct] = fg_next;
                      group_ct++; 
                   }
                } while(fg_next != NULL);
           }
          
           /* Adjust Group slices. */
           for (fs_ptr = fg->slices; fs_ptr != NULL;
               fs_ptr = fs_ptr->next) {
               if (fs_ptr == fs) {
                  if (NULL == fs_ptr->prev) {
                     /* Adjust the base slice for all groups in the slice */
                     for (ct = 0; ct < group_ct; ct++) {
                        fg_list[ct]->slices = fs_ptr->next;
                     }   
                  } else {
                     fs_ptr->prev->next = fs_ptr->next;
                  }
                   
                  if (NULL != fs_ptr->next) {
                     fs_ptr->next->prev = fs_ptr->prev;
                  }
                  fs_ptr->next = fs_ptr->prev = NULL;
               }
           }
       }
       fs = fs_next;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Field group structure
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_compress(int unit, _field_group_t *fg, _field_stage_t *stage_fc)
{
    _field_slice_t *fs;     /* Field slice structure pointer.   */
    _field_slice_t *efs;    /* Slice that contains free slots.  */
    int eidx;               /* Empty slot index.                */
    int idx;                /* Slice entry iterator.            */
    int tmp_idx1;           /* Slice entry index 1.             */
    int tmp_idx2;           /* Slice entry index 2.             */
    int slice_sz;           /* Number of entries in a slice.    */
    int rv;                 /* Operation return status.         */

    /* Get group slice pointer. */
    fs = fg->slices;

    efs = NULL;
    eidx = -1;

    while (NULL != fs) {
        _BCM_FIELD_ENTRIES_IN_SLICE(unit, fg, fs, slice_sz);
        for (idx = 0; idx < slice_sz; idx++) {
            /* Find an empty slot. */
            if (NULL == fs->entries[idx]) {
                if (NULL == efs) {
                    efs = fs;
                    eidx = idx;
                }
                continue;
            }

            if (NULL != efs) {
                /*
                 * Given Stage, Slice number and Entry offset in the slice,
                 * calculate the TCAM entry indices.
                 */
                BCM_IF_ERROR_RETURN(_bcm_field_th_slice_offset_to_tcam_idx(unit,
                    stage_fc, fg->instance, efs->slice_number, eidx, &tmp_idx1));

                BCM_IF_ERROR_RETURN(_bcm_field_th_slice_offset_to_tcam_idx(unit,
                    stage_fc, fg->instance, fs->slice_number, idx, &tmp_idx2));

                /* Move the entry in hardware. */
                BCM_IF_ERROR_RETURN(_field_th_entry_move(unit, fs->entries[idx],
                    (tmp_idx1 - tmp_idx2)));
                fs = efs;
                idx = eidx;
                efs = NULL;
                eidx = -1;
                _BCM_FIELD_ENTRIES_IN_SLICE(unit, fg, fs, slice_sz);
            }
        }
        fs = fs->next;
    }
    rv = _bcm_field_th_group_free_unused_slices(unit, stage_fc, fg);
    return (rv);
}

/*
 * Function:
 *     _field_th_stage_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Field group structure
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_stage_group_compress(int unit,
                               _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    _field_group_t *fg;     /* Field Group structure pointer.   */
    int rv = BCM_E_NONE;    /* Operation return status.         */

    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        /* Skit other stages. */
        if (fg->stage_id != stage_fc->stage_id) {
            continue;
        }

        /* Skit new groups. */
        if (NULL == fg->slices) {
            continue;
        }

        /* Ignore Non-Expanded Groups. */
        if (NULL == fg->slices->next) {
            continue;
        }

        /*
         * Best effort to compress groups that occupy more than one physical
         * table.
         */
        rv = _field_th_group_compress(unit, fg, stage_fc);
    }
    return (rv);
}

/*
 * Function:
 *     _field_th_group_add_cam_compress
 * Purpose:
 *     Compress other groups to free room for inserted group.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_cam_compress(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Set current stage to be previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Compress expanded groups. */
    BCM_IF_ERROR_RETURN(_field_th_stage_group_compress(unit, fsm_ptr->fc,
        fsm_ptr->stage_fc));

    /* Retry allocating slice/s for the new field group. */
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;

    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *     _bcm_field_th_tcam_part_to_entry_flags
 * Purpose:
 *     Each field entry contains up to 3 TCAM
 *     entries. This routine maps tcam entry (0-2)
 *     to SW entry flags.
 *     Single: 0 (IFP_TCAM view)
 *     Single & Intraslice Double: 0 (IFP_TCAM_WIDE view)
 *     Paired: 0, 1 (IFP_TCAM_WIDE view)
 *     Triple: 0, 1, 2 (IFP_TCAM_WIDE view)
 *     Retrieve the part of the group entry resides in.
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     entry_flags - (IN)  Entry flags.
 *     group_flags - (IN)  Entry group flags.
 *     entry_part  - (OUT) Entry part (0-2)
 * Returns:
 *     BCM_E_NONE       - Success.
 *     BCM_E_INTERNAL   - Invalid entry part.
 */
int
_bcm_field_th_tcam_part_to_entry_flags(int unit, int entry_part,
                                       uint32 group_flags,
                                       uint32 *entry_flags)
{
    /* Input parameters check. */
    if (NULL == entry_flags) {
        return (BCM_E_PARAM);
    }

    /* Map entry part count to entry flags. */
    switch (entry_part) {
        case 0:
            *entry_flags = _FP_ENTRY_PRIMARY;
            break;
        case 1:
            *entry_flags = _FP_ENTRY_SECONDARY;
            break;
        case 2:
            *entry_flags = _FP_ENTRY_TERTIARY;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_group_ltmap_delete
 * Purpose:
 *    Delete Logical Table's association with group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    fg        - (IN/OUT) Field group structure pointer.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_ltmap_delete(int unit, _field_group_t *fg)
{
    _field_control_t *fc;       /* Field Control Strucutre.       */
    int inst;                   /* Instance.                      */
    _field_stage_t *stage_fc;   /* Field stage control structure. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Check if group has a valid logical table information pointer. */
    if (fg->lt_id == -1) {
        return (BCM_E_NONE);
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    
    /* Get stage control information. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, 
                                                 fg->stage_id, &stage_fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for (inst = 0; inst < _FP_MAX_NUM_PIPES; inst++) {
            fc->lt_info[inst][fg->lt_id]->valid = FALSE;
            fc->lt_info[inst][fg->lt_id]->priority = 0;
            fc->lt_info[inst][fg->lt_id]->flags = 0;
            fc->lt_info[inst][fg->lt_id]->lt_action_pri = 0; 
            fc->lt_info[inst][fg->lt_id]->lt_entry = 0;
        }
    } else {
        fc->lt_info[fg->instance][fg->lt_id]->valid = FALSE;
        fc->lt_info[fg->instance][fg->lt_id]->priority = 0;
        fc->lt_info[fg->instance][fg->lt_id]->flags = 0;
        fc->lt_info[fg->instance][fg->lt_id]->lt_action_pri = 0;
        fc->lt_info[fg->instance][fg->lt_id]->lt_entry = 0;
    }

    /* Clear Group's LT_ID information. */
    fg->lt_id = -1;
    
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_lt_entry_tcam_remove
 * Purpose:
 *     Clears a LT selection TCAM entry for a given index from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_f_ent - (IN) Logical Table entry structure.
 *     tcam_idx - (IN) Entry TCAM index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_lt_entry_tcam_remove(int unit, _field_lt_entry_t *lt_f_ent,
                            int tcam_idx)
{
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    soc_mem_t lt_tcam_mem;      /* LT TCAM memory name identifier.  */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
        lt_f_ent->lt_fs->stage_id, &stage_fc));

    /* Get LT TCAM memory name. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

    /* Validate supplied TCAM index. */
    if (tcam_idx < soc_mem_index_min(unit, lt_tcam_mem)
        || tcam_idx > soc_mem_index_max(unit, lt_tcam_mem)) {
        return (BCM_E_PARAM);
    }

    /* Clear TCAM entry in hardware at the supplied TCAM index. */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
        tcam_idx, soc_mem_entry_null(unit, lt_tcam_mem)));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_default_entry_remove
 * Purpose:
 *     Remove a logical table entry from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Logical Table entry identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_remove(int unit, bcm_field_entry_t lt_entry)
{
    _field_lt_entry_t *lt_f_ent; /* LT entry structure pointer. */
    int parts_count = -1;        /* Entry parts count.          */
    int part;                    /* Entry part.                 */
    int tcam_idx;                /* LT TCAM entry index.        */

    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
        _FP_ENTRY_PRIMARY, &lt_f_ent));

    if (!(lt_f_ent->flags & _FP_ENTRY_INSTALLED)) {
        /* Entry not installed in hardware to remove. */
        return (BCM_E_NONE);
    }

    /* Get number of entry parts using group flags. . */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    /* Iterate over entry parts and remove the entry from HW. */
    for (part = 0; part < parts_count; part++) {
        /* Get LT TCAM entry details from hardware. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_entry_get(unit, lt_f_ent->group,
            lt_f_ent + part));

        /* Given entry part number, get the entry TCAM index. */
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_tcam_idx_get(unit,
            lt_f_ent + part, (lt_f_ent + part)->lt_fs, &tcam_idx));

        /* Clear the entry from hardware. */
        BCM_IF_ERROR_RETURN(_field_lt_entry_tcam_remove(unit, lt_f_ent + part,
            tcam_idx));

        lt_f_ent[part].flags |= _FP_ENTRY_DIRTY;
        lt_f_ent[part].flags &= ~(_FP_ENTRY_INSTALLED);
        lt_f_ent[part].flags &= ~(_FP_ENTRY_ENABLED);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_default_rule_deinit
 * Purpose:
 *     Deinit logical table default rule from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_rule_deinit(int unit, _field_stage_t *stage_fc,
                                       _field_lt_entry_t *lt_f_ent)
{
    int parts_count = 0; /* Entry parts count.               */
    int part;            /* Entry part iterator.             */
    _field_group_t *fg;  /* Field group structure pointer.   */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get field group structure pointer. */
    fg = lt_f_ent->group;

    /* Get number of entry parts using group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Uninstall entry parts keygen program profile entries from hardware. */
    for (part = 0; part < parts_count; part++) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
            &stage_fc->keygen_profile[fg->instance].profile,
            fg->ext_codes[part].keygen_index));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_selcodes_uninstall
 * Purpose:
 *     Uninstall group's key generation program from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field Group control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_selcodes_uninstall(int unit, _field_stage_t *stage_fc,
                                     _field_group_t *fg)
{
    _field_control_t *fc;         /* Field Control Strucutre.  */
    
    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       int                     part, parts_ct = 0;
       int                     idx;
       _field_presel_entry_t  *f_presel;
       

       /* Get number of entry parts using group flags. */
       BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                                &parts_ct));

       /*
        * Detach Presel Entries from the LT slice associated with the group
        */
       for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
           if (fg->presel_ent_arr[idx] != NULL) {
              f_presel = fg->presel_ent_arr[idx];
           } else {
              continue;
           }

           /* Uninstall entry parts keygen program profile
            * entries from hardware. */
           for (part = 0; part < parts_ct; part++) {
               BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                      &stage_fc->keygen_profile[fg->instance].profile,
                      fg->ext_codes[part].keygen_index));
           }

           /* Remove Presel entries from hardware. */
           BCM_IF_ERROR_RETURN(_bcm_field_th_presel_entry_hw_remove
                                                   (unit, f_presel));

           /* Destroy resources allocated for the entry. */
           BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_phys_destroy
                                                   (unit, f_presel));
       }
    } else {
       _field_lt_entry_t *lt_f_ent; /* LT field entry structure pointer.  */
       bcm_field_entry_t lt_entry;  /* LT selection Entry ID.             */

        /* Get Entry ID Group's LT information structure. */
       if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
           lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
       } else {
           lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry; 
       }

       /* Get entry LT entry structure pointer. */
       BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
                           _FP_ENTRY_PRIMARY, &lt_f_ent));

       /* Remove LT entry from hardware. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_remove(unit, lt_entry));

       /* Delete keygen profiles used by the group. */
       BCM_IF_ERROR_RETURN(_field_th_lt_entry_default_rule_deinit(unit,
                                                     stage_fc, lt_f_ent));

       /* Destroy resources allocated for the entry. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_phys_destroy(
                                                     unit, lt_f_ent));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_group_uninstall
 * Purpose:
 *     Uninstall group from all slices.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_uninstall(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg)
{
    _field_control_t *fc;       /* Field control structure.     */
    _field_slice_t *fs;         /* Field slice structure.       */
    _field_slice_t *temp_fs;    /* Expanded slices iterator.    */
    int parts_count = 0;        /* Number of entry parts.       */
    uint8 slice_number;         /* Slice iterator.              */
    int idx;                    /* Slice iterator.              */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }
    /* Get Field Control structure pointer. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /*
     * Uninstall TCAM keygen program configuration from hardware for each
     * slice.
     */
    BCM_IF_ERROR_RETURN(_field_th_ingress_selcodes_uninstall(unit, stage_fc,
        fg));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = parts_count - 1; idx >= 0; idx--) {
        /* Get Slice ID from TCAM entry part number. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        fs = fg->slices + slice_number;

        /* Clear slice mode in hardware and reset to HW default. */
        BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
            fs->slice_number, fg, 1));

        while (NULL != fs) {
            /* Clear slice if in-use. */
            BCM_IF_ERROR_RETURN(_field_th_ingress_slice_clear(unit, fg, fs));
            temp_fs = fs->next;
            if (NULL != fs->prev) {
                fs->prev->next = NULL;
                fs->prev = NULL;
            }
            fs = temp_fs;
        }
    }

    /*  Free LT_ID association with this group. */
    BCM_IF_ERROR_RETURN(_field_th_group_ltmap_delete(unit, fg));

    /* Install LT part mapping to hardware. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_ingress_logical_table_map_write(unit,
                                                             stage_fc, fg));

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_group_lt_prio_update
 * Purpose:
 *    Update a group's logical table action priority value.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fg       - (IN) Field group.
 *     priority - (IN) New field group priority value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_lt_prio_update(int unit, _field_group_t *fg,
                                   int priority)
{
    _field_stage_t *stage_fc;   /* Field stage control.         */
    int lt_id;                  /* Logical Table Identifier.    */
    _field_group_t     *fg_ptr = NULL;
    int inst;                   /* Instance.                    */
    _field_control_t *fc;       /* Field control structure.     */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Return success if there is no change in group's priority value. */
    if (fg->priority == priority) {
        return (BCM_E_NONE);
    }

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->slices == NULL) {
        return BCM_E_INTERNAL;
    }

    do {
        /* Retrieve the existing groups associated with the Slice */
        (void)_bcm_field_th_slice_group_get_next(unit, fg->instance,
                                                 fg->stage_id,
                                                 fg->slices->slice_number,
                                                 &fg_ptr, &fg_ptr);
        if ((fg_ptr != NULL) && (fg_ptr != fg)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: Can not set the priority, group's Slice has more"
                " than one group.\n\r")));
           return BCM_E_CONFIG;
        }
    } while(fg_ptr != NULL); 


    /* Get current Logical Table ID value assigned to the Group. */
    lt_id = fg->lt_id;

    /* Update group's priority value. */
    fg->priority = priority;

    /* Allocate LT action priority. */
    BCM_IF_ERROR_RETURN(_field_th_lt_priority_alloc(unit, stage_fc, fg,
        lt_id));

    /* Update priority value in Group's logical table information structure. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for (inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            fc->lt_info[inst][lt_id]->priority = fg->priority;
        }
    } else {
        fc->lt_info[fg->instance][lt_id]->priority = fg->priority;
    }

    /* Install LT part mapping to hardware. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_ingress_logical_table_map_write(unit,
                                                             stage_fc, fg));

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_group_deinit
 * Purpose:
 *    Destroy field group structure.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     fg     - (IN) Allocated group structure.
 *
 * Returns:
 *     BCM_E_NONE      - Success
 */
int
_bcm_field_th_group_deinit(int unit, _field_group_t *fg)
{
    _field_control_t *fc;       /* Field control structure.       */
    int idx;                    /* Qualifiers iteration index.    */
    _field_stage_t *stage_fc;   /* Stage field control structure. */

    /* Nothing to do as group is already uninstalled. */
    if (NULL == fg) {
        return (BCM_E_NONE);
    }

    /* Get Field Control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Uninstall group from every slice. */
    if (NULL != fg->slices) {
        _field_th_ingress_group_uninstall(unit, stage_fc, fg);
    }

    /* Deallocate group qualifiers list. */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        /* If qualifier set was updated, free original qualifiers arrary. */
        BCM_IF_ERROR_RETURN(_bcm_field_group_qualifiers_free(fg, idx));
    }

    /* Decrement the use counts for any UDFs used by the group */
    for (idx = 0; idx < BCM_FIELD_USER_NUM_UDFS; idx++) {
        if (SHR_BITGET(fg->qset.udf_map, idx))  {
            if (fc->udf[idx].use_count > 0) {
                fc->udf[idx].use_count--;
            }
        }
    }

    /* Deallocate group entry array if any. */
    if (NULL != fg->entry_arr) {
        sal_free(fg->entry_arr);
    }

    /* Deallocate group LT entry array if any. */
    if (NULL != fg->lt_entry_arr) {
        sal_free(fg->lt_entry_arr);
    }

    /* Remove group from unit's group list. */
    BCM_IF_ERROR_RETURN(_bcm_field_group_linked_list_remove(unit, fg));

    /* Deallocate group memory. */
    sal_free(fg);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_end
 * Purpose:
 *     Group add state machine last/clean up state.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_end(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Deallocate Field Group memory on failure. */
    if (fsm_ptr->stage_fc != NULL) {
        if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            if (BCM_FAILURE(fsm_ptr->rv)
                    || BCM_FAILURE(fsm_ptr->rv =
                        _field_group_default_aset_set(unit, fsm_ptr->fg))) {
                _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
            }
        } else if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
            if (BCM_FAILURE(fsm_ptr->rv)) {
                _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
            }
        } else {
            return (BCM_E_PARAM);
        }
    } else {
        if (BCM_FAILURE(fsm_ptr->rv)) {
            _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
        }
    }

    return (fsm_ptr->rv);
}

/*
 * Function:
 *    _field_th_group_add_adjust_lt_map
 * Purpose:
 *    Update slices logical table mapping and priority in hardware.
 * Parameters:
 *    unit     - (IN)     BCM device number.
 *    fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_add_adjust_lt_map(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameter check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Set current state to be the previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Install LT part mapping to hardware. */
    fsm_ptr->rv = _bcm_field_th_ingress_logical_table_map_write(unit,
                                      fsm_ptr->stage_fc, fsm_ptr->fg);
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    }

    return (_field_th_group_add(unit, fsm_ptr));
}

/*
 * Function:
 *    _field_th_group_add
 * Purpose:
 *    Create a field processor group
 * Parameters:
 *    unit      - (IN)     BCM device number.
 *    fsm_ptr   - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_PARAM       - Null state machine structure pointer.
 *    BCM_E_UNAVAIL     - Unsupported group create option specified.
 *    BCM_E_RESOURCE    - Requested HW resources unavailable
 *                        or Specified QSET cannot be accomodated with available
 *                        hardware resources.
 *    BCM_E_MEMORY      - Memory allocation failure.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_th_group_add(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int rv = BCM_E_INTERNAL; /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    switch (fsm_ptr->fsm_state) {
        case _BCM_FP_GROUP_ADD_STATE_START:
            rv = _field_th_group_add_initialize(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_ALLOC:
            rv = _field_th_group_add_alloc(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE:
            rv = _field_th_group_add_qset_update(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET:
            rv = _field_th_group_add_extractor_codes_get(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE:
            rv = _field_th_group_add_qset_alternate(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE:
            rv = _field_th_group_add_slice_allocate(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS:
            rv = _field_th_group_add_cam_compress(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP:
            rv = _field_th_group_add_adjust_lt_map(unit, fsm_ptr);
            break;
        case _BCM_FP_GROUP_ADD_STATE_END:
            rv = _field_th_group_add_end(unit, fsm_ptr);
            break;
        default:
            /* Not a valid switch-case option. */
            return rv;
    }

    return (rv);
}

/*
 * Function:
 *    _bcm_field_th_group_add
 * Purpose:
 *    Create a field processor group
 * Parameters:
 *    unit - (IN)     BCM device number.
 *    gc   - (IN/OUT) Group configuration structure pointer.
 * Returns:
 *    BCM_E_PARAM - Null group configuration structure pointer.
 *    BCM_E_UNAVAIL - Unsupported group create option specified.
 *    BCM_E_RESOURCE - Requested HW resources unavailable
 *                     or Specified QSET cannot be accomodated with available
 *                     hardware resources.
 *    BCM_E_MEMORY - Memory allocation failure.
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_group_add(int unit,
                        bcm_field_group_config_t *gc)
{
    int                    rv = BCM_E_INTERNAL; /* Operation return status. */
    bcm_port_config_t      pc;       /* Device port configuration structure. */
    _field_stage_id_t      stage;    /* FP stage information. */
    _field_group_add_fsm_t fsm;      /* Group creation state machine.    */
    _field_control_t       *fc;

    /* Input parameter check. */
    if (NULL == gc) {
        return (BCM_E_PARAM);
    }

    /* Get unit FP control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /*
     * Check and return error when SMALL or LARGE slice request flag is set
     * for TH device.
     */
    if ((0 == soc_feature(unit, soc_feature_field_ingress_two_slice_types))
        && (gc->flags & BCM_FIELD_GROUP_CREATE_SMALL
            || gc->flags & BCM_FIELD_GROUP_CREATE_LARGE)) {
        return (BCM_E_UNAVAIL);
    }


    /* Get the FP stage in which new group must be created. */
    BCM_IF_ERROR_RETURN(_bcm_field_group_stage_get(unit, &gc->qset, &stage));

    if (!(gc->flags & BCM_FIELD_GROUP_CREATE_WITH_ID)) {
        /* qset and pri are validated in bcm_field_group_create_mode_id(). */
        BCM_IF_ERROR_RETURN(_bcm_field_group_id_generate(unit, &gc->group));
    }

    /* Get device port configuration. */
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pc));

    /* Initialize group creation tracking structure. */
    sal_memset(&fsm, 0, sizeof(_field_group_add_fsm_t));

    /* Initialize group creation tracking structure. */
    fsm.fsm_state = _BCM_FP_GROUP_ADD_STATE_START;
    fsm.priority = gc->priority;
    fsm.group_id = gc->group;
    fsm.qset = gc->qset;
    fsm.aset = gc->aset;
    fsm.rv = BCM_E_NONE;
    fsm.action_res_id = gc->action_res_id;
    fsm.hintid = gc->hintid;

    if (fsm.hintid != 0) {
        if (fsm.hintid >= _FP_HINT_ID_MAX) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "HintId is out of allowed range\r\n")));
            rv = BCM_E_PARAM;
            return (rv);
        }
        if (_FP_HINTID_BMP_TEST(fc->hintid_bmp, fsm.hintid) == 0) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Trying to attach hintid which is"
                                " not created\r\n")));
            rv = BCM_E_NOT_FOUND;
            return (rv);
        }
        if ((BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageEgress)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageLookup)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageExternal)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageIngressExactMatch)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageClass)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageClassExactMatch))) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,"HintId is supported only on"
                                " StageIngress \r\n")));
            rv = BCM_E_PARAM;
            return (rv);
        }
    }

    /* Apply group member port configuration. */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_PORT) {
        BCM_PBMP_ASSIGN(fsm.pbmp, gc->ports);
    } else {
        BCM_PBMP_ASSIGN(fsm.pbmp, pc.all);
    }

    /* Apply group mode configuration */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_MODE) {
        fsm.mode = gc->mode;
    } else {
        fsm.mode = bcmFieldGroupModeDefault;
    }

    /* Validate and Apply group Preselset configuration */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET) {
       fsm.preselset = gc->preselset;
       fsm.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    }
 
    switch (stage) {
        case _BCM_FIELD_STAGE_LOOKUP:
        case _BCM_FIELD_STAGE_EGRESS:
            /*
             * COVERITY
             *
             * Callee Mirror module function using most of the stack.
             */
            /* coverity[stack_use_overflow : FALSE] */
            /*  Add FP group. */
            rv = _bcm_field_group_add(unit, &fsm);
            break;
        case _BCM_FIELD_STAGE_EXACTMATCH:
        case _BCM_FIELD_STAGE_INGRESS:
            rv = _field_th_group_add(unit, &fsm);
            break;
        case _BCM_FIELD_STAGE_CLASS:
            rv = _bcm_field_th_class_group_add(unit, &fsm, gc->aset);
            break;
        default:
            /* Invalid FP stage. */
            return (BCM_E_PARAM);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (rv);
}

/*
 * Function: _bcm_field_th_find_empty_slice
 *
 * Purpose:
 *     Get a new slice for an existing group if available.
 *     Used for auto-expansion of groups
 *     Currently: supported in Raptor, Firebolt2.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     fg     - (IN) field group
 *     fs_ptr - (IN/OUT) _field_slice_t for the PRIMARY if there is one available.
 *
 * Returns:
 *     BCM_E_xxx
 *
 */
int
_bcm_field_th_find_empty_slice(int unit, _field_group_t *fg,
                               _field_slice_t **fs_ptr)
{
    _field_control_t *fc;          /* Field control structure. */
    _field_stage_t   *stage_fc;    /* Field stage control.     */
    _field_slice_t   *fs;          /* Field slice pointer.     */
    uint8 slice_num;               /* Slice iterator.          */
    int parts_count = 0;           /* Number of entry parts.   */
    uint32 entry_flags;            /* Field entry part flags.  */
    int rv;                        /* Operation return status. */
    int lt_part_prio;              /* LT Partition priority.   */
    int part_index;
    uint8 old_physical_slice;      /* Last slice in group.     */
    uint8 new_physical_slice;      /* Allocated slice.         */
    _field_group_t  *curr_fg, *next_fg;

    /* Get field control structure. */
    rv = _field_control_get(unit, &fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    if (0 == (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION)) {
        return (BCM_E_RESOURCE);
    }

    fs = &fg->slices[0];
    while (fs->next != NULL) {
        fs = fs->next;
    }
    old_physical_slice = fs->slice_number;

    /*
     * Find an empty slice
     * Validate the ports and mode
     */
    for (slice_num = 0; slice_num < stage_fc->tcam_slices; slice_num++) {
        if (slice_num == old_physical_slice) {
           continue;
        }

        rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                    slice_num);
        if (BCM_SUCCESS(rv)) {
            uint8 empty;

            fs = stage_fc->slices[fg->instance] + slice_num;
            BCM_IF_ERROR_RETURN(_field_th_slice_is_empty(unit, fs, &empty));
            if (empty == TRUE) {
               break;
            }
        }
    }

    if (slice_num == stage_fc->tcam_slices) {
        /* No free slice; attempt to compress existing auto expanded groups. */
        /*
         * COVERITY
         *
         * Callee Mirror module function using most of the stack.
         */
        /* coverity[stack_use_overflow : FALSE] */
        rv = _field_th_stage_group_compress(unit, fc, stage_fc);
        BCM_IF_ERROR_RETURN(rv);

        /* Retry slice allocation. */
        for (slice_num = 0; slice_num < stage_fc->tcam_slices; slice_num++) {
            rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                        slice_num);
            if (BCM_SUCCESS(rv)) {
                break;
            }
        }
        
        if (slice_num == stage_fc->tcam_slices)
        {
           /* No free slice */
           return (BCM_E_RESOURCE);
        }
    }

    new_physical_slice = slice_num;

    /* Get number of entry parts for the group. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    /* Retrieve the partition priority value for wide mode group's slices.*/
    rv = _bcm_field_th_lt_part_prio_value_get(unit, fg, &lt_part_prio);
    BCM_IF_ERROR_RETURN(rv);

    /* Set up the new physical slice parameters in Software */
    for(part_index = 0; part_index < parts_count; part_index++) {
        /* Get entry flags. */
        rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part_index, fg->flags,
                                                    &entry_flags);
        BCM_IF_ERROR_RETURN(rv);

        /* Get slice id for entry part */
        rv = _bcm_field_th_tcam_part_to_slice_number(part_index, fg->flags,
                                                     &slice_num);
        BCM_IF_ERROR_RETURN(rv);

        /* Get slice pointer. */
        fs = stage_fc->slices[fg->instance] + new_physical_slice
                + slice_num;

        /* Enable slice. */
        if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
            /*
             * Initialize slice free entries count value based on group
             * flags.
             */
            if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
                fs->free_count = fs->entry_count;
            } else {
                fs->entry_count >>= 1;
                fs->free_count = fs->entry_count;
                fs->start_tcam_idx >>= 1;
            }

            /* Set group flags in in slice.*/
            fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

            /* Add slice to slices linked list . */
            stage_fc->slices[fg->instance][old_physical_slice + slice_num].next = fs;
            fs->prev =
             &stage_fc->slices[fg->instance][old_physical_slice + slice_num];
        }

        curr_fg = NULL;
        next_fg = NULL;
        do {
           rv = _bcm_field_th_slice_group_get_next(unit, fg->instance,
                                                   fg->stage_id,
                                                   old_physical_slice,
                                                   &curr_fg, &next_fg);
           if (BCM_FAILURE(rv)) {
              break;
           }

          /*
           * Install TCAM keygen program configuration in hardware for all
           * the groups for each slice.
           */
           rv = _field_th_ingress_group_expand_slice_install(unit, stage_fc,
                                           next_fg,
                                           new_physical_slice + slice_num,
                                           part_index, lt_part_prio);
           BCM_IF_ERROR_RETURN(rv);
           curr_fg = next_fg;
        } while (next_fg != NULL);
    }

    *fs_ptr = stage_fc->slices[fg->instance] + new_physical_slice;
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_th_group_enable_set
 *
 * Purpose:
 *     To enable/diable the field group operational status.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     group  - (IN) field group
 *     enable - (IN) enable (or) disbale flag.
 *
 * Returns:
 *     BCM_E_xxx
 *
 */
int
_bcm_field_th_group_enable_set(int unit, bcm_field_group_t group, uint32 enable)
{
    int rv;                             /* operational status */
    int tcam_idx[_FP_MAX_ENTRY_WIDTH];  /* Entry TCAM index.  */
    int parts_count = -1;               /* Parts count.       */
    uint32 idx;                         /* Temporary pointer variable. */
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Entry buffer. */
    _field_group_t *fg;           /* Field Group Structure. */
    soc_mem_t lt_tcam_mem;        /* TCAM memory ID. */
    bcm_field_entry_t lt_entry;   /* Logical table entry. */
    _field_lt_entry_t *lt_f_ent;  /* Field LT entry pointer. */
    _field_stage_t *stage_fc;     /* Stage Field control structure. */
    _field_control_t *fc;         /* Field Control Strucutre.       */

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get the group control pointer. */
    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    /* Get the stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            fg->stage_id, &stage_fc));

    /* Assign groups logical table entry. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
    } else {
        lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;
    }

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit, lt_entry,
        &lt_f_ent));

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, lt_f_ent->group->flags,
            &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count; idx++) {
        rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_ent + idx,
                (lt_f_ent + idx)->lt_fs, tcam_idx + idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    rv = _field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent, &lt_tcam_mem);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count; idx++)  {
        /*
         * Maximum index value for entry in this TCAM is limited to
         * No. of slices * No. of LT_IDs/slice (12 * 32 == 384).
         */
        if ((tcam_idx[idx] < soc_mem_index_min(unit, lt_tcam_mem))
            || (tcam_idx[idx] > (stage_fc->lt_tcam_sz - 1) )) {
            return (BCM_E_PARAM);
        }

        /* Clear entry buffer. */
        sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

        /* Read entry into SW buffer. */
        rv = soc_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx[idx],
                entry);
        BCM_IF_ERROR_RETURN(rv);

        soc_mem_field32_set(unit, lt_tcam_mem, entry, VALIDf, enable ? 1 : 0);

        /* Install entry in hardware. */
        rv = soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL, tcam_idx[idx],
                entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Enable/Disable group flags. */
    if (enable) {
        fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
    } else {
        fg->flags &= ~_FP_GROUP_LOOKUP_ENABLED;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_th_group_status_calc
 * Purpose:
 *    Fill in the values of a group status structure.
 * Parameters:
 *    unit - (IN)     BCM device number.
 *    fg   - (IN/OUT) Group to update the status structure.
 * Returns:
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_group_status_calc(int unit, _field_group_t *fg)
{
    bcm_field_group_status_t *status;   /* Group status structure pointer. */
    _field_stage_t *stage_fc;           /* Stage field control structure.  */
    _field_slice_t *fs;                 /* Field slice control structure.  */
    int count = 0;                      /* Generic purposes counter.       */
    int rv;                             /* Operation return status.        */
    int slice_index;                    /* Hardware slice index.           */
    uint8 single_wide = 0;              /* Single wide group indicator.    */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get field group stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, fg->stage_id, &stage_fc));

    /* Get field group status structure pointer. */
    status = &fg->group_status;
    if (NULL == status) {
        return (BCM_E_INTERNAL);
    }

    /* Get group primary slice structure pointer. */
    fs = &fg->slices[0];

    /* Get field group intraslice double wide status. */
    single_wide = ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) ? 1 : 0;
    status->entries_total = 0;
    /* Calculate total entries count in slices allocated for the group. */
    while (NULL != fs) {
        status->entries_total += fs->entry_count;
        fs = fs->next;
    }

    /* Calculate total free entries count in slices allocated for the group. */
    status->entries_free = 0;
    fs = &fg->slices[0];
    while (NULL != fs) {
        /* Get free entries count in the slice. */
        _bcm_field_entries_free_get(unit, fs, fg, &count);
        status->entries_free += count;
        fs = fs->next;
    }

    /*
     * Calculate total counters and meters count, free counters and meters
     * count.
     */
    fs = fg->slices;
    status->counters_total = _bcm_field_counters_total_get(stage_fc, fs);
    status->counters_free = _bcm_field_counter_free_get(stage_fc, fs);
    status->meters_total = _bcm_field_meters_total_get(stage_fc, fg->instance,
                                fs);
    status->meters_free = _bcm_field_meter_free_get(stage_fc, fg->instance,
                                fs);

    /*
     * Calculate total entries count and total free entries count for the group
     * if it expands to other available slices.
     */
    if (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION) {
        for (slice_index = 0; slice_index < stage_fc->tcam_slices;
             ++slice_index) {
            rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                    slice_index);
            if (BCM_SUCCESS(rv)) {
                fs = stage_fc->slices[fg->instance] + slice_index;
                if (single_wide) {
                    status->entries_total += fs->entry_count;
                    status->entries_free += fs->free_count;
                } else {
                    status->entries_total += (fs->entry_count / 2);
                    status->entries_free += (fs->free_count / 2);
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_default_group_expand_install
 * Purpose:
 *    Allocate and Install LT Entries in a group expanded slice.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Reference to Field Stage structure.
 *    fg         - (IN) Reference to Field Group structure.
 *    entry_part - (IN) Slice Entry Part.
 *    lt_fs      - (IN) Reference to LT expanded slice.
 * Returns:
 *    BCM_E_xxx.
 */
int
_field_th_ingress_default_group_expand_install(int unit,
                                               _field_stage_t *stage_fc,
                                               _field_group_t *fg,
                                               int entry_part,
                                               _field_lt_slice_t *lt_fs)
{
    int rv; 
    int parts_count = -1, part_index = -1;
    int slice_num = -1, part, pri_tcam_idx = 0;
    uint8 slice_attached = 0;
    _field_lt_slice_t     *lt_part_fs = NULL;
    _field_lt_entry_t     *lt_f_entry;
    _field_lt_entry_t     *lt_f_entry_p = NULL;
    _field_control_t *fc;
    bcm_field_entry_t lt_entry;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) != 0) {
       return BCM_E_INTERNAL;
    }
  
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "%s(): fg:%p fg->gid:%d lt_fs:%p slice_number:%d entry_part:%d\n\r"),
        __func__, fg, fg->gid, lt_fs, lt_fs->slice_number, entry_part));


    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
         lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
    } else {
         lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry; 
    }

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit,
                      lt_entry, &lt_f_entry));

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                              &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (part = 0; part < parts_count; part++) {
       lt_f_entry_p = lt_f_entry + part;
       if (part == entry_part) {
          break;
       }
    }

    if (lt_f_entry_p == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Get entry part flags. */
    rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part, fg->flags,
                                                   &(lt_f_entry_p->flags));
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    /* Given primary entry TCAM index calculate entry part tcam index. */
    if (0 != part) {
        /* Get entry TCAM index given slice number and entry offset in slice. */
        rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
                                      fg->instance, lt_fs->slice_number,
                                      lt_f_entry->index, &pri_tcam_idx);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, fg,
                                                      pri_tcam_idx,
                                                      part, &part_index);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

       /*
        * Given entry part TCAM index, determine the slice number and slice
        * offset index.
        */
        rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                       fg->instance, part_index, &slice_num,
                       (int *)&lt_f_entry_p->index);
        if (BCM_FAILURE(rv)) {
           return rv;
        }
    }

    /* Check if the given lt slice is already associated to the entry. */
    lt_part_fs = lt_f_entry_p->lt_fs;
    for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
        if (lt_part_fs == lt_fs) {
           slice_attached = 1;
           break;
        }
    }
       
    if (slice_attached == 0) {
       /* Attach the expanded slice for the part entry. */
       lt_part_fs = lt_f_entry_p->lt_fs;
       for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
          if (lt_part_fs->next == NULL) {
             lt_part_fs->next = lt_fs;
             lt_part_fs->next->next = NULL;
             lt_part_fs->next->prev = lt_part_fs;
             break;
          } 
       }
    }

    /* Mark entry dirty. */
    lt_f_entry_p->flags |= _FP_ENTRY_DIRTY;
   
    /* Update LT Slices. */ 
    /* Decrement slice free entry count for primary entries. */
    lt_fs->free_count--;
    /* Assign entry to a slice. */
    lt_fs->entries[lt_f_entry_p->index] = lt_f_entry_p;

    for (part = 0; part < parts_count; part++) {
       lt_f_entry_p = lt_f_entry + part;

       if (entry_part == part) {
          break;
       }
    }
    
    /* Install the LT Entry in HW. */
    rv = _field_th_lt_default_tcam_entry_install(unit, lt_f_entry_p->eid, 1);

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "default_group_expand_install():"
     "lt_f_entry:%p eid:%d lt_fs:%p slice_num:%d slice_idx:%d\n\r"),
     lt_f_entry, lt_f_entry->eid, lt_fs,lt_fs->slice_number,lt_f_entry->index));
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));
    
    return rv;

}

/*
 * Function:
 *    _field_th_ingress_group_expand_install
 * Purpose:
 *    Allocate and Install Presel Entries in a group expanded slice.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Reference to Field Stage structure.
 *    fg         - (IN) Reference to Field Group structure.
 *    entry_part - (IN) Slice Entry Part.
 *    lt_fs      - (IN) Reference to LT expanded slice.
 * Returns:
 *    BCM_E_xxx.
 */
STATIC int
_field_th_ingress_group_expand_install(int unit, _field_stage_t *stage_fc,
                                       _field_group_t *fg, int entry_part,
                                       _field_lt_slice_t *lt_fs)
{
    int rv, idx;
    int parts_count = -1, part_index = -1;
    int slice_num = -1, part, pri_tcam_idx = 0;
    uint8 slice_attached = 0;
    _field_lt_slice_t     *lt_part_fs;
    _field_presel_entry_t *f_presel_p;
    _field_presel_entry_t *f_presel;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "%s(): fg:%p fg->gid:%d lt_fs:%p slice_number:%d entry_part:%d\n\r"),
        __func__, fg, fg->gid, lt_fs, lt_fs->slice_number, entry_part));

    f_presel = fg->presel_ent_arr[0];
    if (f_presel == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                              &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    f_presel_p = f_presel;
    for (part = 0; (part < parts_count && f_presel_p != NULL); part++) {
       if (part == entry_part) {
          break;
       }
       f_presel_p = f_presel_p->next;
    }

    if (f_presel_p == NULL || part >= parts_count) {
       return BCM_E_INTERNAL;
    }

    /* Get entry part flags. */
    rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part, fg->flags,
                                                &(f_presel_p->flags));
    BCM_IF_ERROR_RETURN(rv);

    /* Given primary entry TCAM index calculate entry part tcam index. */
    if (0 != part) {
        /* Get entry TCAM index given slice number and entry offset in slice. */
        rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
                                      fg->instance, lt_fs->slice_number,
                                     f_presel->hw_index, &pri_tcam_idx);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, fg,
                                                      pri_tcam_idx,
                                                      part, &part_index);
        BCM_IF_ERROR_RETURN(rv);

       /*
        * Given entry part TCAM index, determine the slice number and slice
        * offset index.
        */
        rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                       fg->instance, part_index, &slice_num,
                       (int *)&f_presel_p->hw_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Check if the given lt slice is already associated to the presel entry. */
    lt_part_fs = f_presel_p->lt_fs;
    for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
        if (lt_part_fs == lt_fs) {
           slice_attached = 1;
           break;
        }
    }
    
    if (slice_attached == 0) {
       /* Attach the expanded slice for the part entry. */
       lt_part_fs = f_presel_p->lt_fs;
       for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
          if (lt_part_fs->next == NULL) {
             lt_part_fs->next = lt_fs;
             lt_part_fs->next->next = NULL;
             lt_part_fs->next->prev = lt_part_fs;
             break;
          } 
       }
    }

    if (lt_part_fs == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Mark entry dirty. */
    f_presel_p->flags |= _FP_ENTRY_DIRTY;

    /* Update Presel LT Slices. */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        if (fg->presel_ent_arr[idx] != NULL) {
           f_presel = fg->presel_ent_arr[idx];
        } else {
           continue;
        }
  
        f_presel_p = f_presel;
        for (part = 0; (part < parts_count && f_presel_p != NULL); part++) {
           if (entry_part == part) {
              break;
           }
           f_presel_p = f_presel_p->next;
        }

        if (f_presel_p == NULL || part == parts_count) {
           return BCM_E_INTERNAL;
        }
 
        lt_part_fs = f_presel_p->lt_fs;
        for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
           /* Goto the last slice. */
           if (lt_part_fs->next == NULL) {
              /* Decrement slice free entry count for primary entries. */
              lt_part_fs->free_count--;
              /* Assign entry to a slice. */
              lt_part_fs->p_entries[f_presel_p->hw_index] = f_presel_p;
              break;
           }
        }

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "group_expand_install():"
              "f_presel_p:%p presel:%d lt_fs:%p slice_num:%d slice_idx:%d\n\r"),
              f_presel_p,f_presel_p->presel_id,lt_fs,lt_fs->slice_number,
              f_presel_p->hw_index));
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));

        /* Install the LT Entry in HW. */
        rv = _bcm_field_th_lt_entry_install(unit, f_presel_p, lt_fs);
        BCM_IF_ERROR_RETURN(rv);
    }


    return BCM_E_NONE;
}



/*
 * Function:
 *    _field_th_ingress_group_expand_slice_install
 * Purpose:
 *    Auxiliary routine used to install field group in hardware
 *    for a given slice.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     stage_fc      - (IN) Installed group structure.
 *     fg            - (IN/OUT)  Field group structure.
 *     slice_number  - (IN) Slice number group installed in.
 *     part_index    - (IN) Entry Part Index.
 *     group_slice_prio - (IN) Group's Priority for the particular slice.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_expand_slice_install(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg, uint8 slice_number,
                                         int part_index, int group_slice_prio)
{
    int rv;
    _field_lt_slice_t *lt_fs;   /* LT Slice pointer.                */
    _field_slice_t *fs;         /* Field slice structure pointer.   */
    int inst;
    _field_control_t          *fc;           /* Field control structure. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == fg->lt_slices)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get LT entry part slice control structure. */
    lt_fs = stage_fc->lt_slices[fg->instance] + slice_number;

    /* Set slice mode in hardware based on Group mode/flags. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
        lt_fs->slice_number, fg, 0));
    /*
     * Initialize slice free entries count value to number of
     * entries in the slice.
     */
    if (0 == fg->slices[0].lt_map) {
        lt_fs->free_count = lt_fs->entry_count;
    }

    /* Get IFP TCAM slice information. */
    fs = stage_fc->slices[fg->instance] + slice_number;

    /* Update slice LT_ID mapping in group's slice. */
    fs->lt_map |= (1 << fg->lt_id);

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        for(inst = 0 ; inst < _FP_MAX_NUM_PIPES; inst++) {
            fc->lt_info[inst][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
            fc->lt_info[inst][fg->lt_id]->lt_part_pri[fs->slice_number] = group_slice_prio;
        }
    } else {
        fc->lt_info[fg->instance][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
        fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = group_slice_prio;
    }

    /* Install TCAM keygen program configuration in hardware for each slice. */
    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       rv = _field_th_ingress_group_expand_install(unit, stage_fc, fg,
                                                   part_index, lt_fs);
    } else {
       /* Default case. */
       rv = _field_th_ingress_default_group_expand_install(unit, stage_fc, fg,
                                                           part_index, lt_fs);
    }
 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error: Group slice auto-expand failed for"
             " group:%d part:%d slice:%d\n\r"),
             fg->gid, part_index, lt_fs->slice_number));
        return rv;
    }

    /* Set the partition Priority. */
    rv =_bcm_field_th_ingress_lt_partition_prio_write(unit, stage_fc, fg,
                                       fg->lt_id, slice_number);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error: LT Partition Set failed for"
             " group:%d lt_id:%d slice:%d val:%d\n\r"),
             fg->gid, fg->lt_id, slice_number, group_slice_prio));
    }

    return (rv);
}

/*
 * Function:
 *     _field_th_update_offset_src_dst_container_sel
 * Purpose:
 * 	To update the proper bus offsets for postmuxes using src_dst_container_sel_0/1
 * Parameters:
 *     unit         - (IN) 	BCM unit number.
 *     fg           - (IN) 	Field group structure pointer.
 *     f_qual_arr   - (IN)      Field qualifier array.
 *     qual_count   - (IN)      Length of qualifier array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_update_offset_src_dst_container_sel(int unit, _field_group_t *fg,
                               uint16 qual_count,
                               _bcm_field_qual_info_t **f_qual_arr)
{
    int qid;			/* Qualifier ID   */
    int idx;			/* Index iterator.*/
    int src_dst_cont_flags = 0;	/* Src Dst Container Flags */
    int src_dst_cont_count = 0; /* Src Dst Cont count */
    int dglp_off_0 = 0;		/* DGLP offset 0 */
    int dglp_off_1 = 0;		/* DGLP offset 1 */
    int dvplag_off_0 = 0;	/* DVPLAG offset 0 */
    int dvplag_off_1 = 0;	/* DVPLAG offset 1 */
    int ecmp1_off_0 = 0;	/* ECMP1 offset 0 */
    int ecmp1_off_1 = 0;	/* ECMP1 offset 1 */
    int next_hop_off_0 = 0;	/* NEXT_HOP offset 0 */
    int next_hop_off_1 = 0;	/* NEXT_HOP offset 1 */
    int ipmc_off_0 = 0;		/* IPMC offset 0 */
    int ipmc_off_1 = 0;		/* IPMC offset 1 */
    int l2mc_off_0 = 0;		/* L2MC offset 0 */
    int l2mc_off_1 = 0;		/* L2MC offset 1 */

    /* update f_qual_arr with proper offsets for post mux qualifiers using src_dst_cont_0/1*/
    for (idx = 0; idx < qual_count; idx++) {
	/* Set the qualifier name. */
	qid = f_qual_arr[idx]->qid;
	switch(qid) {
	    case bcmFieldQualifyDstTrunk:
	    case bcmFieldQualifyDstPort:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DGLP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DGLP);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    dglp_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    dglp_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {

		    f_qual_arr[idx]->conf_arr->offset.offset[0] = dglp_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = dglp_off_1;
		}
		break;
	    case bcmFieldQualifyDstMimGport:
	    case bcmFieldQualifyDstMimGports:
	    case bcmFieldQualifyDstNivGport:
	    case bcmFieldQualifyDstNivGports:
	    case bcmFieldQualifyDstVxlanGport:
	    case bcmFieldQualifyDstVxlanGports:
	    case bcmFieldQualifyDstVlanGport:
	    case bcmFieldQualifyDstVlanGports:
	    case bcmFieldQualifyDstMplsGport:
	    case bcmFieldQualifyDstMplsGports:
	    case bcmFieldQualifyDstWlanGport:
	    case bcmFieldQualifyDstWlanGports:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_DVPLAG)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_DVPLAG);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    dvplag_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    dvplag_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {

		    f_qual_arr[idx]->conf_arr->offset.offset[0] = dvplag_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = dvplag_off_1;
		}
		break;
	    case bcmFieldQualifyDstMultipath:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_ECMP1)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_ECMP1);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    ecmp1_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    ecmp1_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {

		    f_qual_arr[idx]->conf_arr->offset.offset[0] = ecmp1_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = ecmp1_off_1;
		}
		break;
	    case bcmFieldQualifyDstL3Egress:
	    case bcmFieldQualifyDstL3EgressNextHops:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_NEXT_HOP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_NEXT_HOP);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    next_hop_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    next_hop_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {

		    f_qual_arr[idx]->conf_arr->offset.offset[0] = next_hop_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = next_hop_off_1;
		}
		break;
	    case bcmFieldQualifyDstL2MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_L2MC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_L2MC_GROUP);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    l2mc_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    l2mc_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {
		    f_qual_arr[idx]->conf_arr->offset.offset[0] = l2mc_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = l2mc_off_1;
		}
		break;
	    case bcmFieldQualifyDstL3MulticastGroup:
		if (!(src_dst_cont_flags & _FP_SRC_DST_CONT_IPMC_GROUP)) {
		    src_dst_cont_flags |= (_FP_SRC_DST_CONT_IPMC_GROUP);
		    _field_th_src_dst_container_sel_offset(unit, fg, idx,
			    src_dst_cont_count, f_qual_arr);
		    src_dst_cont_count++;
		    ipmc_off_0 = f_qual_arr[idx]->conf_arr->offset.offset[0];
		    ipmc_off_1 = f_qual_arr[idx]->conf_arr->offset.offset[1];
		} else {
		    f_qual_arr[idx]->conf_arr->offset.offset[0] = ipmc_off_0;
		    f_qual_arr[idx]->conf_arr->offset.offset[1] = ipmc_off_1;
		}
		break;
	    default:
		;
	}
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_src_dst_container_sel_offset
 * Purpose:
 * 	To update the proper bus offsets for postmuxes using src_dst_container_sel_0/1
 * Parameters:
 *     unit         - (IN) 	BCM unit number.
 *     fg           - (IN) 	Field group structure pointer.
 *     f_qual_arr   - (IN)      Field qualifier array.
 *     qual_count   - (IN)      Length of qualifier array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_src_dst_container_sel_offset(int unit, _field_group_t *fg, int idx,
					uint16 src_dst_cont_count,
					_bcm_field_qual_info_t **f_qual_arr)
{
    /*check the flags for Intraslice or span single slice modes*/
    if((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
	&& !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
	{
	  /*change the offset for 80bit mode for src_dst_container_1 and 0*/
	    if(src_dst_cont_count == 0){
		f_qual_arr[idx]->conf_arr->offset.offset[0] = 88;
		f_qual_arr[idx]->conf_arr->offset.offset[1] = 152;
	    }
	    else {
		/*count is 1. so set the container 1 offsets*/
		f_qual_arr[idx]->conf_arr->offset.offset[0] = 104;
		f_qual_arr[idx]->conf_arr->offset.offset[1] = 153;
	    }
	}
	else {
	    /*160 bit mode offsets*/
	    if((src_dst_cont_count%2) == 0){
		f_qual_arr[idx]->conf_arr->offset.offset[0] = 48;
		f_qual_arr[idx]->conf_arr->offset.offset[1] = 152;
	    }
	    else {
		/*count is 1. so set the container 1 offsets*/
		f_qual_arr[idx]->conf_arr->offset.offset[0] = 64;
		f_qual_arr[idx]->conf_arr->offset.offset[1] = 153;
	    }
	}
    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_th_update_ext_codes_src_dst_container_sel
 * Purpose:
 * 	To update the proper extractor codes for postmuxes using src_dst_container_sel_0/1
 * Parameters:
 *     unit         		- (IN) 		BCM unit number.
 *     fg           		- (IN/OUT) 	Field group structure pointer.
 *     src_dest_cont_sel   	- (IN)      	Container_selector 0/1
 *     src_dest_cont_sel_value   - (IN)      	Value for container_sel_val
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_update_ext_codes_src_dst_container_sel(int unit, _field_group_t *fg, int part,
                               uint16 src_dest_cont_sel, uint16 src_dest_cont_sel_value)
{
    if (src_dest_cont_sel >= 6) {
	/*only 2 selectors exists 0 and 1*/
	return BCM_E_INTERNAL;
    }
    if ((part == 0 && src_dest_cont_sel >= 2)
	|| (part == 1 && src_dest_cont_sel >= 4)) {
	/* skip this return -1;*/
	return BCM_E_FAIL;
	}

    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: PostMux count=%d Current Src_dst_cont (0/1) =%d.\n"),
                 unit, src_dest_cont_sel, (src_dest_cont_sel%2)));
    fg->ext_codes[part].pmux_sel[4] = 1;
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
	!(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
    {
	/*80 bit mode*/
	if(src_dest_cont_sel == 0)
	{
	    /*use container_0 for the first postmux qualifier in qset*/
	    fg->ext_codes[part].pmux_sel[12] = 1;
	    fg->ext_codes[part].src_dest_cont_0_sel = src_dest_cont_sel_value;
	}
	else /* if (src_dest_cont_sel == 1)*/
	{
	    fg->ext_codes[part].pmux_sel[11] = 1;
	    fg->ext_codes[part].src_dest_cont_1_sel = src_dest_cont_sel_value;
	}
    } else {
	/*160bit mode*/
        if((src_dest_cont_sel%2) == 0)
	{
	    /*use container_0 for the first postmux qualifier in qset*/
	    fg->ext_codes[part].pmux_sel[14] = 1;
	    fg->ext_codes[part].src_dest_cont_0_sel = src_dest_cont_sel_value;
	}
	else /* if (src_dest_cont_sel == 1)*/
	{
	    fg->ext_codes[part].pmux_sel[13] = 1;
	    fg->ext_codes[part].src_dest_cont_1_sel = src_dest_cont_sel_value;
	}
    }
    return BCM_E_NONE;

}
 
/*
 * Function:
 *     _bcm_field_th_group_instance_set
 *
 * Purpose:
 *     Set the group instance based on the group's operational mode
 *     and group's pbmp.
 *
 * Parameters:
 *     unit       -  (IN)  BCM device number.
 *     fg         -  (IN)  Reference to Field Group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_instance_set(int unit, _field_group_t *fg)
{
    int                rv;                 
    int                pipe;      /* Device pipe iterator.    */
    bcm_port_config_t  pc;        /* Port configuration.      */
    _field_stage_t    *stage_fc;  /* Stage control structure. */
    bcm_pbmp_t         t_pbmp;    /* Pipe Port bitmap.        */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Initialize port configuration structure. */
    bcm_port_config_t_init(&pc);

    /* Get device port configuration. */
    rv = bcm_esw_port_config_get(unit, &pc);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

   /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

   /*
    * Based on the Stage Group Operational mode, set the Field Group's instance
    * information.
    */
    if (bcmFieldGroupOperModeGlobal == stage_fc->oper_mode) {

      /*
       * Validate user supplied IPBM value.
       * For global mode, all ports must be part of IPBM value, if supplied.
       */
       if (BCM_PBMP_NEQ(pc.all, fg->pbmp)) {
           /* If port configuration get fails, go to end. */
           return BCM_E_PARAM;
       }

       /* For group global operational mode, use the default instance.  */
       fg->instance = _FP_DEF_INST;
    } else {
      /*
       * For PipeLocal group oper mode, derive the instance
       * information from group's PBM value.
       */
       for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
           BCM_PBMP_ASSIGN(t_pbmp, PBMP_PIPE(unit, pipe));
           BCM_PBMP_REMOVE(t_pbmp, PBMP_LB(unit));
          /*
           * User must use bcm_port_config_get API to retrieve Pipe PBMP value
           * and pass this value in Group's pbmp parameter when creating
           * PipeLocal groups.
           */
           if (BCM_PBMP_EQ(t_pbmp, fg->pbmp)) {
              /*
               * Pipe PBM matches user supplied PBM, update field group
               * instance.
               */
               fg->instance = pipe;
               break;
           }
       }

      /*
       * Portbit map value supplied by the user not matching any of the pipe's
       * port bitmap mask value, hence return input paramter error.
       */
       if (NUM_PIPE(unit) == pipe) {
          return BCM_E_PARAM;
       }
    }

    return (BCM_E_NONE);
}

#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
int _th_field_not_empty;
#endif /* !BCM_TOMAHAWK_SUPPORT && !BCM_FIELD_SUPPORT */
