/*
 * $Id: field.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 *     API for Field Processor (FP) for ROBO family and later.
 *
 *
 */


#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <sal/types.h>
#include <bcm/port.h>

#include <bcm_int/robo/field.h>
#include <soc/drv.h>


#ifdef BCM_FIELD_SUPPORT 
int robo_prio_set_with_no_free_entries = FALSE; 

#define BCM_FIELD_SHARED_ENTRIES

/*
 * Macro:
 *     ASSERT_BAD_UNIT (internal)
 * Purpose:
 *     Assert if invalid unit number used.
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in assert if invalid unit number.
 */
#define ASSERT_BAD_UNIT(unit) \
    assert(0 <= unit && unit < BCM_MAX_NUM_UNITS)

/*
 * Macro:
 *     FIELD_IS_INIT (internal)
 * Purpose:
 *     Check that the unit is valid and confirm that the field functions
 *     are initialized.
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNIT), return(BCM_E_UNAVAIL), or
 *     return(BCM_E_INIT) if fails.
 */
#define FIELD_IS_INIT(unit)                                      \
    if (!SOC_UNIT_VALID(unit)) {                                 \
        return BCM_E_UNIT;                                       \
    }                                                            \
    if (!soc_feature(unit, soc_feature_field)) {                 \
        return BCM_E_UNAVAIL;                                    \
    }                                                            \
    if (_field_control[unit] == NULL) {                          \
        FP_ERR(("FP Error: unit=%d not initialized\n", unit));   \
        return BCM_E_INIT;                                       \
    }


/*
 * Macro:
 *     FIELD_ENTRY_GET
 * Purpose:
 *     Get an entry struct.
 * Parameters:
 *     unit        - BCM device number
 *     entry       - entry ID to search for
 *     f_ent (OUT) - pointer to entry structure
 * Notes:
 *     Failure results in return(BCM_E_NOT_FOUND);
 */
#define FIELD_ENTRY_GET(unit, entry, f_ent, slice)            \
    f_ent = _robo_field_entry_find(unit, entry, slice);            \
    if (f_ent == NULL) {                                      \
        FP_ERR(("FP Error: Entry %d not found.\n", entry));   \
        return BCM_E_NOT_FOUND;                               \
    }

#ifdef BROADCOM_DEBUG
STATIC char *_robo_field_qual_name(bcm_field_qualify_t qid);
STATIC char *_robo_field_action_name(bcm_field_action_t action);
STATIC void _robo_field_slice_dump(int unit, char *prefix, _field_slice_t *fs,
                              char *suffix);

STATIC void _robo_field_range_dump(const char *pfx, _field_range_t *fr);
STATIC void _robo_field_entry_phys_dump(int unit, _field_entry_t *f_ent);
 
void _robo_field_qual_list_dump(char *prefix, _qual_info_t *qual_list,
                                  char *suffix);
STATIC void _field_meter_dump(const _field_entry_t *f_ent);
STATIC void _field_counter_dump(const _field_entry_t *f_ent);
STATIC void _robo_field_action_dump(const _field_action_t *fa);
STATIC void _robo_field_group_status_dump(const bcm_field_group_status_t *gstat);
#endif /* BROADCOM_DEBUG */
/*
 * Field control data, one per device.
 */
static _field_control_t         *_field_control[BCM_MAX_NUM_UNITS];

#ifdef BCM_FIELD_SHARED_ENTRIES
/* Sync with the entries of all group */
static _field_entry_t           *_field_shared_entries[FP_SLICE_SZ_MAX];
static int                           _field_shared_entries_free = 0;
#endif /* BCM_FIELD_SHARED_ENTRIES */


static const bcm_mac_t _bcm_field_mac_all_ones =
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


/*
 * Function:
 *     _robo_field_action_alloc
 * Purpose:
 *     Allocate and initialize an action structure.
 * Parameters:
 *     action   - Action to perform (bcmFieldActionXXX)
 *     param0   - Action parameter (use 0 if not required)
 *     param1   - Action parameter (use 0 if not required)
 *     fa (OUT) - pointer to field action structure
 * Returns:
 *     BCM_E_MEMORY - allocation failure
 *     BCM_E_NONE   - Success
 */
STATIC int
_robo_field_action_alloc(bcm_field_action_t action, uint32 param0, uint32 param1,
                    _field_action_t **fa)
{
    *fa = sal_alloc(sizeof (_field_action_t), "field_action");
    if (*fa == NULL) {
        FP_ERR(("FP Error: allocation failure for field_action\n"));
        return BCM_E_MEMORY;
    }
    sal_memset(*fa, 0, sizeof (_field_action_t));

    (*fa)->action = action;
    (*fa)->param0 = param0;
    (*fa)->param1 = param1;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_actions_conflict
 * Purpose:
 *     Determine if two actions are compatible. Incompatible actions are ones
 *     that require the same hardware fields written in different manners to 
 *     coexist.
 * Parameters:
 *     action1 - First action
 *     action2 - Second action
 * Returns:
 *     0  if no conflict exists
 *     !0 if conflict exists
 */
STATIC int
_field_actions_conflict(bcm_field_action_t act1, bcm_field_action_t act2)
{
    int act1_switch = 0, act2_switch = 0;
    
    if (act1== act2) {
        /* action duplication */
        return 1;
    }

    /* 
     * Check confilct action types 
     * 2 : switch forwarding actions
     * 3 : vlan modification actions
     * 4 : CosQ or Packet internal priority actions
     */
    switch (act1) {
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRedirect:
        case bcmFieldActionDrop:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionDropCancel:
            act1_switch = 2; /* 2: forwarding actions */
            break;
        case bcmFieldActionInnerVlanNew:
        case bcmFieldActionOuterVlanNew:
            act1_switch = 3; /* 3: vlan change actions */
            break;
        case bcmFieldActionCosQNew:
        case bcmFieldActionPrioIntNew:
            act1_switch = 4;
            break;
        default:
            break;
    }
    switch (act2) {
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRedirect:
        case bcmFieldActionDrop:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionDropCancel:
            act2_switch = 2; /* 2: forwarding actions */
            break;
        case bcmFieldActionInnerVlanNew:
        case bcmFieldActionOuterVlanNew:
            act2_switch = 3; /* 3: vlan change actions */
            break;
        case bcmFieldActionCosQNew:
        case bcmFieldActionPrioIntNew:
            act1_switch = 4;
            break;
        default:
            break;
    }
    if (act1_switch && act2_switch) {
        if (act1_switch == act2_switch) {
            return act1_switch;
        }
    }
    return 0;
}

/*
 * Function:
 *     _field_actions_conflict_allow
 * Purpose:
 *     For some special cases that even the new and old actions are
 *     determined conflicted in _field_actions_conflict(). 
 * Parameters:
 *     new_act - First action
 *     param0  - param0 of new_act
 *     param1  - param1 of new_act
 *     old_act - Second action structure
 * Returns:
 *     TRUE  if the conflict is acceptable
 *     FALSE if the conflict is not acceptable
 */
STATIC int
_field_actions_conflict_allow(int unit, bcm_field_action_t new_act, 
                        uint32 param0, uint32 param1, _field_action_t *old_act)
{
    bcm_field_action_t act1, act2;

    act1 = new_act;
    act2 = old_act->action;

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* 
         * If the actions are redirect to cpu and redirect to mirror-to port,
         * they are acceptable.
         */
        if (((act1 == bcmFieldActionRedirect) &&
            (act2 == bcmFieldActionRedirect))  ||
            ((act1 == bcmFieldActionRpRedirectPort) &&
            (act2 == bcmFieldActionRpRedirectPort)) ||
            ((act1 == bcmFieldActionGpRedirectPort) &&
            (act2 == bcmFieldActionGpRedirectPort))){
            if ((param1 == CMIC_PORT(unit)) &&
                (old_act->param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT)) {
                return TRUE;
            } else if ((param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) &&
                (old_act->param1 == CMIC_PORT(unit))) {
                return TRUE;
            } else {
                return FALSE;
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *     _field_action_isnot_supported
 *
 * Purpose:
 *     Add action performed when entry rule is matched for a packet
 *
 * Parameters:
 *     unit - BCM device number
 *     action - Action to perform (bcmFieldActionXXX)
 *
 * Returns:
 *     0  - action is supported by device
 *     1  - action is NOT supported by device
 */
STATIC int
_field_action_not_supported(int unit, bcm_field_action_t action)
{
    int                 retval = BCM_E_NONE;

    switch (action) {
        case bcmFieldActionCosQNew:
            if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit) ||
                SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;                
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionInnerVlanNew:
        case bcmFieldActionOuterVlanNew:
        case bcmFieldActionPrioPktNew:
            if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionDscpNew:
        case bcmFieldActionRpDscpNew:
            if (SOC_IS_ROBO53242(unit) ||SOC_IS_ROBO53262(unit) || 
                SOC_IS_ROBO53115(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionNewTc:
            if (SOC_IS_ROBO5395(unit) ||SOC_IS_ROBO53115(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionGpCosQNew:
            if (SOC_IS_ROBO53242(unit) ||SOC_IS_ROBO53262(unit) || 
                SOC_IS_ROBO53115(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
         case bcmFieldActionRpPrioIntNew:
            if (SOC_IS_ROBO5395(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionGpPrioIntNew:
            if (SOC_IS_ROBO5395(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionDropCancel:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionRpDropCancel:
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionTosCancel:
        case bcmFieldActionTosCopy:
        case bcmFieldActionTosNew:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionEgressMask:
        case bcmFieldActionMirrorEgress:
        case bcmFieldActionMirrorOverride:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionYpDrop:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionColorIndependent:
        case bcmFieldActionClassDestSet:
        case bcmFieldActionClassSourceSet:
        case bcmFieldActionVrfSet:
        case bcmFieldActionInnerVlanPrioNew:
        case bcmFieldActionInnerVlanDelete:
        case bcmFieldActionInnerVlanAdd:
        case bcmFieldActionInnerVlanCfiNew:
        case bcmFieldActionOuterVlanAdd:
        case bcmFieldActionOuterVlanLookup:
        case bcmFieldActionOuterVlanPrioNew:
        case bcmFieldActionOuterVlanCfiNew:
        case bcmFieldActionOuterTpidNew:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionGpSwitchToCpuReinstate:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionGpDscpPrecedenceNew:
        case bcmFieldActionGpOuterVlanPrioNew:
        case bcmFieldActionGpInnerVlanPrioNew:
        case bcmFieldActionGpOuterVlanCfiNew:
        case bcmFieldActionGpInnerVlanCfiNew:
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionGpVlanCosQNew:
        case bcmFieldActionGpPrioPktAndIntCopy:
        case bcmFieldActionGpPrioPktAndIntNew:
        case bcmFieldActionGpPrioPktAndIntTos:
        case bcmFieldActionGpPrioPktAndIntCancel:
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
        case bcmFieldActionDoNotChangeTtl:
        case bcmFieldActionDoNotCheckUrpf:
        case bcmFieldActionDoNotCheckVlan:
        case bcmFieldActionDoNotLearn:
        case bcmFieldActionIpFix:
        case bcmFieldActionIpFixCancel:
        case bcmFieldActionIncomingMplsPortSet:
            retval = BCM_E_UNAVAIL;
            break;
        case bcmFieldActionNewClassId:
        case bcmFieldActionLoopback:
        case bcmFieldActionNewReasonCode:
        case bcmFieldActionBypassStp:
        case bcmFieldActionBypassEap:
        case bcmFieldActionBypassVlan:
            if (SOC_IS_ROBO53115(unit)) {
                retval = BCM_E_NONE;
            } else {
                retval = BCM_E_UNAVAIL;
            }
            break;
        default:
            retval = BCM_E_NONE;
    }

    return retval;
}

/*
 * Function:
 *     _robo_field_entry_slice_id_set
 *
 * Purpose:
 *     Set the slice id value to the CFP TCAM to determine the slice format
 *
 * Parameters:
 *     unit - BCM device number
 *     f_ent - entry to be set
 *     sliceId - slice id value
 *
 * Returns:
 */
STATIC int
_robo_field_entry_slice_id_set(int unit, 
                            _field_entry_t *f_ent, uint32 sliceId)
{
    int retval = BCM_E_NONE;
    uint32 temp;

    /* 
     * The slice format of BCM53115 determined by slice id and L3 framing value,
     * others are determioned by slice id value
     */
    temp = sliceId & 0x7;
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_SLICE_ID, 
            &f_ent->drv_entry, &temp));

    temp = 0x7;
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_SLICE_ID, 
            &f_ent->drv_entry, &temp));

    /* For BCM53115 it need to set the L3 Framing field */
    if (SOC_IS_ROBO53115(unit)) {
        int slice = 0;
        slice = sliceId & 0x3;
        temp  = sliceId >> 2;

        /* If this is a chain slice, don't need to set L3 Framing value */
        if (slice != 0x3) {
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, 
                DRV_CFP_FIELD_L3_FRM_FORMAT, 
                    &f_ent->drv_entry, &temp));
            temp  = 3;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, 
                DRV_CFP_FIELD_L3_FRM_FORMAT, 
                    &f_ent->drv_entry, &temp));
        }
            
    }

    return retval;
}

/*
 * Function:
 *     _robo_field_control_find
 * Purpose:
 *     Lookup a _field_control_t from a unit ID.
 * Parameters:
 *     unit   BCM unit number
 */
_field_control_t *
_robo_field_control_find(int unit)
{
    ASSERT_BAD_UNIT(unit);

    return _field_control[unit];
}

/*
 * Function:
 *     _robo_field_entry_find
 * Purpose:
 *     Lookup a _field_entry_t from a unit ID and slice choice.
 * Parmeters:
 *     unit   - BCM Unit
 *     eid    - Entry ID
 *     slice  - _FP_SLICE_xxx
 * Returns:
 *     Pointer to entry struct with matching eid, or NULL if no match
 */
_field_entry_t *
_robo_field_entry_find(int unit, bcm_field_entry_t eid, int slice)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    _field_slice_t      *fs;
    int                 slice_idx;

    ASSERT_BAD_UNIT(unit);

    assert(_FP_SLICE_PRIMARY <= slice && slice <= _FP_SLICE_TERTIARY);

    fc = _robo_field_control_find(unit);
    if (fc == NULL) {
        return NULL;
    }

    /* Iterate over the linked-list of groups */
    fg = fc->groups;

    while (fg != NULL) {
        fs = &fg->slices[slice];
        for (slice_idx = 0; slice_idx < fc->tcam_sz; slice_idx++) {
            if (fs->entries[slice_idx] != NULL &&
                fs->entries[slice_idx]->eid == eid) {
                return fs->entries[slice_idx];
            }
        }
        fg = fg->next;
    }

    return NULL;
}


/*
 * Function:
 *     _robo_field_entry_prio_cmp
 * Purpose:
 *     Compare two entry priorities
 * Parameters:
 * Returns:
 *     -1 if prio_first <  prio_second
 *      0 if prio_first == prio_second
 *      1 if prio_first >  prio_second
 */
STATIC int
_robo_field_entry_prio_cmp(int prio_first, int prio_second) {
   int retval;

   if (prio_first == prio_second) {
       retval = 0;
   } else if (prio_first < prio_second) {
       retval = -1;
   } else {
       retval = 1;
   }
   FP_VVERB(("_robo_field_entry_prio_cmp (first=0x%x ? second=0x%x) retval=%d\n",
             prio_first, prio_second, retval));
   return retval;
}

/*
 * Function:
 *     _robo_field_entry_move
 * Purpose:
 *     Move an entry within a slice by "amount" indexes
 *     
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - entry to be moved
 *     amount   - number of indexes to move + or -
 * Returns:
 *     BCM_E_NONE   - Success
 */
STATIC int
_robo_field_entry_move(int unit, _field_entry_t *f_ent, int amount) {
    _field_slice_t      *fs;
    drv_cfp_entry_t     cfp_entry;
    uint32      tcam_idx_old, tcam_idx_new;
    int   tcam_idx_max;
    uint32      temp;
    _field_control_t    *fc = NULL;
    _field_counter_t    *f_cnt = NULL;

    assert(f_ent != NULL);
    fs            = f_ent->fs;
    assert(fs != NULL);

    if (amount == 0) {
        FP_WARN(("FP Warning: moving entry=%d, same slice_idx=%d(0x%x)\n",
                 f_ent->eid, f_ent->slice_idx, f_ent->slice_idx));
        return BCM_E_NONE;
    }

    fc = _robo_field_control_find(unit);
    assert(f_ent->slice_idx < fc->tcam_sz);

    tcam_idx_old = f_ent->slice_idx;
    tcam_idx_new = tcam_idx_old + amount;
    tcam_idx_max = fc->tcam_sz -1;
    assert(0 <= tcam_idx_old && tcam_idx_old <= tcam_idx_max);
    assert(0 <= tcam_idx_new && tcam_idx_new <= tcam_idx_max);
    if (robo_prio_set_with_no_free_entries == FALSE) {
        /* Move the hardware entry.*/
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_entry_read)
            (unit, tcam_idx_old, DRV_CFP_RAM_ALL, &cfp_entry));
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_entry_write)
            (unit, tcam_idx_new, DRV_CFP_RAM_ALL, &cfp_entry));
        /* Sync the counter values */
        if (f_ent->counter != NULL) {
            f_cnt = f_ent->counter;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_stat_get)
                (unit, DRV_CFP_STAT_INBAND, tcam_idx_old, &temp));
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_stat_set)
                (unit, DRV_CFP_STAT_INBAND, tcam_idx_new, temp));
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_stat_get)
                (unit, DRV_CFP_STAT_OUTBAND, tcam_idx_old, &temp));
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_stat_set)
                (unit, DRV_CFP_STAT_OUTBAND, tcam_idx_new, temp));
            f_cnt->index    = tcam_idx_new;
        }
    
        /* Clear original one */
        temp = 0;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_VALID, 
                &cfp_entry, &temp));
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_entry_write)
                (unit, tcam_idx_old, DRV_CFP_RAM_ALL, &cfp_entry));
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_stat_set)
            (unit, DRV_CFP_STAT_ALL, tcam_idx_old, temp));
    }

    /* Move the software entry to the new index */           
    if (robo_prio_set_with_no_free_entries == FALSE) {
#ifdef BCM_FIELD_SHARED_ENTRIES
        _field_shared_entries[(f_ent)->slice_idx]          = NULL;        
#endif
        (f_ent)->fs->entries[(f_ent)->slice_idx]          = NULL;

    }
#ifdef BCM_FIELD_SHARED_ENTRIES 
    _field_shared_entries[(f_ent)->slice_idx + (amount)] = (f_ent);   
#endif
    FIELD_ENTRY_SLICE_IDX_CHANGE(f_ent, amount);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _robo_field_entry_shift_down
 * Purpose:
 *     
 * Parameters:
 *     unit     - BCM device number
 * Returns:
 *     BCM_E_NONE   - Success
 */
STATIC int
_robo_field_entry_shift_down(int unit, _field_slice_t *fs, 
    uint16 slice_idx_start, uint16 next_null_idx) {
    uint16              slice_idx_empty;
    _field_control_t    *fc;

    assert(fs != NULL);
    fc = _robo_field_control_find(unit);
    assert(fc != NULL);

    slice_idx_empty = next_null_idx;

    while(slice_idx_empty > slice_idx_start) {
       /* Move the entry at the previous index to the empty index. */
#ifdef BCM_FIELD_SHARED_ENTRIES
       BCM_IF_ERROR_RETURN(
           _robo_field_entry_move(unit, _field_shared_entries[slice_idx_empty - 1], 
           1)); 
#else
        BCM_IF_ERROR_RETURN(
           _robo_field_entry_move(unit, fs->entries[slice_idx_empty - 1], 1));
#endif
       slice_idx_empty--;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _robo_field_entry_shift_up
 * Purpose:
 *     
 * Parameters:
 *     unit     - BCM device number
 * Returns:
 *     BCM_E_NONE   - Success
 */
STATIC int
_robo_field_entry_shift_up(int unit, _field_slice_t *fs, 
    uint16 slice_idx_start, uint16 prev_null_idx) {
    uint16              slice_idx_empty;
    _field_control_t    *fc;

    assert(fs != NULL);
    fc = _robo_field_control_find(unit);
    assert(fc != NULL);

    slice_idx_empty = prev_null_idx;

    while(slice_idx_empty < slice_idx_start) {
       /* Move the entry at the previous index to the empty index. */
#ifdef BCM_FIELD_SHARED_ENTRIES
       BCM_IF_ERROR_RETURN(
           _robo_field_entry_move(unit, _field_shared_entries[slice_idx_empty + 1], 
           -1)); 
#else
        BCM_IF_ERROR_RETURN(
           _robo_field_entry_move(unit, fs->entries[slice_idx_empty + 1], -1));
#endif
       slice_idx_empty++;
    }

    return BCM_E_NONE;
}




/*
 * Function:
 *     _robo_field_entry_phys_create
 *
 * Purpose:
 *     Initialize a physical entry structure.
 *
 * Parameters:
 *     unit      - BCM unit
 *     group     - Group ID
 *     entry     - Entry ID
 *     slice_idx - Entry index within slice
 *     fs        - slice that entry resides in
 *
 * Returns
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - _field_entry_t allocation failure
 */   
STATIC _field_entry_t *
_robo_field_entry_phys_create(int unit, bcm_field_entry_t entry, int prio,
                         _field_slice_t *fs)
{
    _field_control_t    *fc;
    _field_entry_t      *f_ent = NULL;

    FP_VVERB(("FP: _robo_field_entry_phys_create(entry=%d, prio=%d)\n",
             entry, prio));

    assert(fs != NULL);

    fc = _robo_field_control_find(unit);

    /* allocate and zero memory for field entry */
    f_ent = sal_alloc(sizeof (_field_entry_t), "field_entry");
    if (f_ent == NULL) {
        FP_ERR(("FP Error: allocation failure for field_entry\n"));
        return NULL;
    }
    sal_memset(f_ent, 0, sizeof (_field_entry_t));

    /* Fill in the basic fields. */
    f_ent->unit         = unit;
    f_ent->eid          = entry;
    f_ent->prio         = prio;
    f_ent->color_indep  = fc->color_indep;
    f_ent->fs           = fs;
    f_ent->dirty = _FIELD_DIRTY;

    /* Check for external TCAM slice. */
    f_ent->ext = 0;

    return f_ent;
}


/*
 * Function:
 *     _field_entry_qual_find
 *
 * Purpose:
 *     Lookup a _field_entry_t from a unit ID taking into account mode. Since
 *     double and triple-wide modes can have multiple physical entries with the
 *     same Entry ID, 
 *
 * Parmeters:
 *     unit   - BCM Unit
 *     eid    - Entry ID
 *     qual   - Qualifier used to select the correct slice
 *
 * Returns:
 *     Pointer to entry struct with matching Entry ID, or NULL if no match
 */
STATIC _field_entry_t *
_field_entry_qual_find(int unit, bcm_field_entry_t entry,
                       bcm_field_qualify_t qual) {
    _field_entry_t      *f_ent;
    _field_group_t      *fg;

    f_ent = _robo_field_entry_find(unit, entry, _FP_SLICE_PRIMARY);
    if (f_ent == NULL) {
        return f_ent;
    }
    fg = f_ent->fs->group;
    
    if (BCM_FIELD_QSET_TEST(fg->qset, qual)) {
        /* Use Primary Slice */
        return f_ent;
    }

    return NULL;
}


/*
 * Function:
 *     _robo_field_entry_tcam_idx_get (INTERNAL)
 *
 * Purpose:
 *     Get the TCAM index of an entry ID.
 *
 * Parameters:
 *     fc     - pointer to field control structure
 *     f_ent  - pointer to field entry structure
 *
 * Returns:
 *     slice number for entry
 *
 * Note:
 *    Assumes that the entry exists. Will assert if entry is not valid.
 */
int
_robo_field_entry_tcam_idx_get(_field_control_t *fc, _field_entry_t *f_ent)
{
    int                 tcam_idx = -1;

    assert(fc != NULL);
    assert(f_ent != NULL);
    assert(f_ent->slice_idx < fc->tcam_sz);

    tcam_idx = f_ent->slice_idx;

    return tcam_idx; 
}

/*
 * Function:
 *     _field_tcam_policy_clear
 *
 * Purpose:
 *     Clear a combined TCAM and POLICY_TABLE entry. This clears the actions
 *     and qualifiers from the hardware.
 *
 * Parameters:
 *     unit      - BCM device number
 *     tcam_idx  - TCAM/POLICY_TABLE entry index
 *
 * Returns:
 *     BCM_E_INTERNAL  - Memory Read failure
 *     BCM_E_UNAVAIL   - Device not supported
 *     BCM_E_NONE      - Success
 *
 * Notes:
 */
STATIC int
_field_tcam_policy_clear(int unit, int tcam_idx) {
    int                 rv = BCM_E_UNAVAIL;
    drv_cfp_entry_t     cfp_entry;
    uint32      temp;
    
    rv =  (DRV_SERVICES(unit)->cfp_entry_read)
        (unit, tcam_idx, DRV_CFP_RAM_ALL, &cfp_entry);
    if (BCM_FAILURE(rv)) {
        FP_ERR(("FP Error: Read entry index = %d fail.\n", tcam_idx));
        return rv;
    }

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_IB_NONE, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_OB_NONE, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_IB_DSCP_CANCEL, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_OB_DSCP_CANCEL, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_IB_COSQ_CANCEL, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_OB_COSQ_CANCEL, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_COSQ_CPU_CANCEL, &cfp_entry, 0, 0);
    } else if(SOC_IS_ROBO53115(unit)){
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_IB_NONE, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_OB_NONE, &cfp_entry, 0, 0);
         (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_IB_DSCP_CANCEL, &cfp_entry, 0, 0);
        (DRV_SERVICES(unit)->cfp_action_set)
           (unit, DRV_CFP_ACT_OB_DSCP_CANCEL, &cfp_entry, 0, 0);
    } else {
     (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_NONE, &cfp_entry, 0, 0);
     (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_NONE, &cfp_entry, 0, 0);
     (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_MOD_INT_PRI_CANCEL, &cfp_entry, 0, 0);
     (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_MOD_INT_PRI_CANCEL, &cfp_entry, 0, 0);
    }

    temp = 0;
    (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_VALID, &cfp_entry, &temp);
    rv =  (DRV_SERVICES(unit)->cfp_entry_write)
        (unit, tcam_idx, DRV_CFP_RAM_ACT, &cfp_entry);
    if (BCM_FAILURE(rv)) {
        FP_ERR(("FP Error: Write ACT entry index = %d fail.\n", tcam_idx));
        return rv;
    }
    rv =  (DRV_SERVICES(unit)->cfp_entry_write)
        (unit, tcam_idx, DRV_CFP_RAM_TCAM, &cfp_entry);
    if (BCM_FAILURE(rv)) {
        FP_ERR(("FP Error: Write TCAM entry index = %d fail.\n", tcam_idx));
        return rv;
    }
    return rv;
}


/*
 * Function:
 *     _field_meter_install
 *
 * Purpose:
 *     Install an FP meter into hardware.
 *
 * Parameters:
 *     unit      - BCM device number
 *     f_ent     - entry who's meter is to be installed
 *
 * Returns:
 *     BCM_E_INTERNAL  - Memory Read failure
 *     BCM_E_UNAVAIL   - Device not supported
 *     BCM_E_NONE      - Success
 *
 * Notes:
 */
STATIC int
_field_meter_install(int unit, _field_entry_t *f_ent) {
    int                 retval = BCM_E_UNAVAIL;

    retval = (DRV_SERVICES(unit)->cfp_entry_write)
        (unit, f_ent->slice_idx, DRV_CFP_RAM_METER, &f_ent->drv_entry);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: Write Meter entry index = %d fail.\n",  
            f_ent->slice_idx));
        return retval;
    }
    return retval;
}

/*
 * Function:
 *     _robo_field_tcam_policy_install
 *
 * Purpose:
 *     Install a combined TCAM and POLICY_TABLE entry. This writes the
 *     qualifiers and actions to the hardware.
 *
 * Parameters:
 *     unit      - BCM device number
 *     f_ent     - entry to be installed
 *
 * Returns:
 *     BCM_E_INTERNAL  - Memory Read failure
 *     BCM_E_UNAVAIL   - Device not supported
 *     BCM_E_NONE      - Success
 *
 * Notes:
 */
STATIC int
_robo_field_tcam_policy_install(int unit, _field_entry_t *f_ent, int tcam_idx) {
    int                 retval = BCM_E_UNAVAIL;
    uint32                 port, slice_map, temp;
    int mem_id;
    uint32 fld_id;
    int len, i;
    soc_mem_info_t  *meminfo;
    soc_field_info_t    *fieldinfo;

    assert(f_ent != NULL);

    /* If it's never been done for the slice, write the field select codes. */
    if ((soc_feature(unit, soc_feature_field_slice_enable)) && 
        (f_ent->fs->inst_flg == 0)) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            (DRV_SERVICES(unit)->cfp_control_get)
                (unit, DRV_CFP_SLICE_SELECT, port, &slice_map);
            slice_map |= (0x1 << f_ent->fs->sel_codes.fpf);
            (DRV_SERVICES(unit)->cfp_control_set)
                (unit, DRV_CFP_SLICE_SELECT, port, slice_map);
        }
    }
    f_ent->fs->inst_flg = 1;

    /* 
     * Get the value to set into each entry's valid field. 
     * The valid value is depend on chips.
     */
    if (SOC_IS_ROBO53115(unit)) {
        mem_id = CFP_TCAM_IPV4_SCm;
    } else {
        mem_id = CFP_TCAM_S0m;
    }
    fld_id = VALID_Rf;
    assert(SOC_MEM_IS_VALID(unit, mem_id));
    meminfo = &SOC_MEM_INFO(unit, mem_id);
    SOC_FIND_FIELD(fld_id, meminfo->fields,
                             meminfo->nFields, fieldinfo);
    assert(fieldinfo);
    len = fieldinfo->len;
    temp = 0;
    for (i = 0; i < len; i++) {
        temp |= (0x1 << i);
    }

    (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_VALID, &f_ent->drv_entry, &temp);
     (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, 
            DRV_CFP_FIELD_VALID, &f_ent->drv_entry, &temp);
    retval = (DRV_SERVICES(unit)->cfp_entry_write)
        (unit, tcam_idx, DRV_CFP_RAM_ACT, &f_ent->drv_entry);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: Write Act/Pol entry index = %d fail.\n",  
            f_ent->slice_idx));
        return retval;
    }
    retval = (DRV_SERVICES(unit)->cfp_entry_write)
        (unit, tcam_idx, DRV_CFP_RAM_TCAM, &f_ent->drv_entry);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: Write Tcam entry index = %d fail.\n",  
            f_ent->slice_idx));
        return retval;
    }

    return retval;
}

/*
 * Function:
 *     _robo_field_group_find
 * Purpose:
 *     Lookup a _field_group_t from a unit ID.
 * Parameters:
 *     unit - BCM unit number
 *     gid  - Group ID
 */
_field_group_t *
_robo_field_group_find(int unit, bcm_field_group_t gid)
{
    _field_control_t    *fc;
    _field_group_t      *fg = NULL;

    ASSERT_BAD_UNIT(unit);

    fc = _robo_field_control_find(unit);

    if (fc == NULL) {
        return NULL;
    }

    /* Iterate over the groups linked-list looking for a matching Group ID */
    fg = fc->groups;
    while(fg != NULL) {
        if (fg->gid == gid) {
            return fg;
        }
        fg = fg->next;
    }

    return NULL;
}

STATIC int
_field_qset_mapping(drv_cfp_entry_t *entry, bcm_field_qset_t *qset,
        int drv_qual, int bcm_qual)
{
    int wp, bp;

    if (drv_qual < 0 || drv_qual > DRV_CFP_QUAL_COUNT) {
        return BCM_E_PARAM;
    }
    if (bcm_qual < 0 || bcm_qual > bcmFieldQualifyCount) {
        return BCM_E_PARAM;
    }
    
    wp = drv_qual / 32;
    bp = drv_qual & (32-1);
    if (entry->w[wp] & (0x1 << bp)) {
        BCM_FIELD_QSET_ADD(*qset, bcm_qual);
    }

    return BCM_E_NONE;
    
}

STATIC int
_field_udf_mapping(drv_cfp_entry_t *entry, bcm_field_qset_t *qset,
        int drv_qual, int udf_id)
{
    int wp, bp;

    if (drv_qual < 0 || drv_qual > DRV_CFP_QUAL_COUNT) {
        return BCM_E_PARAM;
    }
    if (udf_id < 0 || udf_id >= BCM_FIELD_USER_NUM_UDFS) {
        return BCM_E_PARAM;
    }
    
    wp = drv_qual / 32;
    bp = drv_qual & (32-1);
    if (entry->w[wp] & (0x1 << bp)) {
        SHR_BITSET(qset->udf_map, udf_id);
    }

    return BCM_E_NONE;
    
}


STATIC void
_field_vulcan_udf_qset_mapping(int unit, bcm_field_qset_t qset, drv_cfp_entry_t *drv_entry)
{
    int i;
    for (i = 0; i < BCM_FIELD_USER_NUM_UDFS; i++) {
        /* A0 ~ A26 */
        if ((i >= 0) &&  (i < 27)) {
            if (SHR_BITGET(qset.udf_map, i)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, (DRV_CFP_QUAL_UDFA0 + i), drv_entry, 1);
            }
        }
        /* B0 ~ B26 */
        if ((i >= 27) &&  (i < 54)) {
            if (SHR_BITGET(qset.udf_map, i)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, (DRV_CFP_QUAL_UDFB0 + (i-27)), drv_entry, 1);
            }
        }
        /* C0 ~ C26 */
        if ((i >= 54) &&  (i < 81)) {
            if (SHR_BITGET(qset.udf_map, i)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, (DRV_CFP_QUAL_UDFC0 + (i-54)), drv_entry, 1);
            }
        }
        /* D0 ~ D11 */
        if ((i >= 81) &&  (i < 93)) {
            if (SHR_BITGET(qset.udf_map, i)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, (DRV_CFP_QUAL_UDFD0 + (i-81)), drv_entry, 1);
            }
        }
    }
    
}


/*
 * Function:
 *     _field_qualify_support
 *
 * Purpose:
 *     Check if these qualifier set is supported for this chip 
 *
 * Parameters:
 *     unit           - BCM unit
 *     qset           - client qualifier set
 *
 * Returns:
 *     BCM_E_UNAVAIL  - No support
 *     BCM_E_NONE     - Success
 */
int
_field_qualify_support(int unit, bcm_field_qset_t qset)
{
    int rv = BCM_E_NONE;
    int i, udf_num = 0;

    /* Get the max number of UDFS */
    rv= ((DRV_SERVICES(unit)->dev_prop_get)
            (unit, DRV_DEV_PROP_CFP_UDFS_NUM, (uint32 *) &udf_num));
    if (rv < 0) {
        udf_num = 0;
    }

    /* UDF checking */
    for (i=0;i < BCM_FIELD_USER_NUM_UDFS; i++) {
        if (SHR_BITGET(qset.udf_map, i)) {
            if (i >= udf_num) {
                return BCM_E_UNAVAIL;
            }
        }
    }

    /* Qualifier checking */
    for (i = 0; i < bcmFieldQualifyCount; i++) {
        if (BCM_FIELD_QSET_TEST(qset, i)) {
            if (SOC_IS_ROBO5348(unit) ||
                SOC_IS_ROBO5347(unit)){
                
                switch (i) {
                    case bcmFieldQualifySrcMac:
                    case bcmFieldQualifyDstMac:
                    case bcmFieldQualifySrcIp:
                    case bcmFieldQualifyDstIp:
                    case bcmFieldQualifyInPort:
                    case bcmFieldQualifyOuterVlan:
                    case bcmFieldQualifyOuterVlanId:
                    case bcmFieldQualifyOuterVlanPri:
                    case bcmFieldQualifyOuterVlanCfi:
                    case bcmFieldQualifyInnerVlan:
                    case bcmFieldQualifyInnerVlanId:
                    case bcmFieldQualifyInnerVlanCfi:
                    case bcmFieldQualifyInnerVlanPri:
                    case bcmFieldQualifyRangeCheck: /* For L4 src port less than 1024 */
                    case bcmFieldQualifyL4SrcPort:
                    case bcmFieldQualifyL4DstPort:
                    case bcmFieldQualifyEtherType:
                    case bcmFieldQualifyIpProtocol:
                    case bcmFieldQualifyDSCP: /* same as ToS */
                    case bcmFieldQualifyTtl:
                    case bcmFieldQualifyTcpControl:
                    case bcmFieldQualifyStageIngress:
                    case bcmFieldQualifySrcIpEqualDstIp:
                    case bcmFieldQualifyEqualL4Port:
                    case bcmFieldQualifyTcpSequenceZero:
                    case bcmFieldQualifyTcpHeaderSize:
                    case bcmFieldQualifyVlanFormat:
                    case bcmFieldQualifySrcPort:
                    case bcmFieldQualifyIpType:
                    case bcmFieldQualifyIp4:
                    case bcmFieldQualifyL2Format:
                    case bcmFieldQualifyPacketFormat:
                    case bcmFieldQualifyLlc:
                        break;
                    default:
                        return BCM_E_UNAVAIL;
                }
                
            } else if (SOC_IS_ROBO5395(unit)) {

                switch (i) {
                    case bcmFieldQualifySrcMac:
                    case bcmFieldQualifyDstMac:
                    case bcmFieldQualifySrcIp:
                    case bcmFieldQualifyDstIp:
                    case bcmFieldQualifyInPort:
                    case bcmFieldQualifyInPorts:
                    case bcmFieldQualifyOuterVlan:
                    case bcmFieldQualifyOuterVlanId:
                    case bcmFieldQualifyOuterVlanPri:
                    case bcmFieldQualifyOuterVlanCfi:
                    case bcmFieldQualifyInnerVlan:
                    case bcmFieldQualifyInnerVlanId:
                    case bcmFieldQualifyInnerVlanPri:
                    case bcmFieldQualifyInnerVlanCfi:
                    case bcmFieldQualifyL4SrcPort:
                    case bcmFieldQualifyL4DstPort:
                    case bcmFieldQualifyEtherType:
                    case bcmFieldQualifyIpProtocol:
                    case bcmFieldQualifyDSCP:   /* same as ToS */
                    case bcmFieldQualifyTtl:
                    case bcmFieldQualifyTcpControl:
                    case bcmFieldQualifyStageIngress:
                    case bcmFieldQualifyVlanFormat:
                    case bcmFieldQualifyIpType:
                    case bcmFieldQualifyIp4:
                    case bcmFieldQualifyL2Format:
                    case bcmFieldQualifyPacketFormat:
                    case bcmFieldQualifyLlc:
                        break;
                    default:
                        return BCM_E_UNAVAIL;
                }
                
            } else if (SOC_IS_ROBO53242(unit) ||
                SOC_IS_ROBO53262(unit)) {

                switch (i) {
                    case bcmFieldQualifySrcIp6:
                    case bcmFieldQualifySrcMac:
                    case bcmFieldQualifyDstMac:
                    case bcmFieldQualifySrcIp:
                    case bcmFieldQualifyDstIp:
                    case bcmFieldQualifyInPort:
                    case bcmFieldQualifyInPorts:
                    case bcmFieldQualifyIp6FlowLabel:
                    case bcmFieldQualifyOuterVlan:
                    case bcmFieldQualifyOuterVlanId:
                    case bcmFieldQualifyOuterVlanPri:
                    case bcmFieldQualifyOuterVlanCfi:
                    case bcmFieldQualifyInnerVlan:
                    case bcmFieldQualifyInnerVlanId:
                    case bcmFieldQualifyInnerVlanCfi:
                    case bcmFieldQualifyInnerVlanPri:
                    case bcmFieldQualifyRangeCheck: /* For L4 src port less than 1024 */
                    case bcmFieldQualifyL4SrcPort:
                    case bcmFieldQualifyL4DstPort:
                    case bcmFieldQualifyEtherType:
                    case bcmFieldQualifyIpProtocol: /* same as Ip6NextHeader */
                    case bcmFieldQualifyIpProtocolCommon:
                    case bcmFieldQualifyDSCP: /* same as ToS, Ip6TrafficClass */
                    case bcmFieldQualifyTtl: /* same as Ip6HopLimit */
                    case bcmFieldQualifyTcpControl:
                    case bcmFieldQualifyStageIngress:
                    case bcmFieldQualifySrcIpEqualDstIp:
                    case bcmFieldQualifyEqualL4Port:
                    case bcmFieldQualifyTcpSequenceZero:
                    case bcmFieldQualifyTcpHeaderSize:
                    case bcmFieldQualifyVlanFormat:
                    case bcmFieldQualifyL2Format:
                    case bcmFieldQualifyBigIcmpCheck:
                    case bcmFieldQualifyIcmpTypeCode:
                    case bcmFieldQualifyIgmpTypeMaxRespTime:
                    case bcmFieldQualifyIpType:
                    case bcmFieldQualifyIp4:
                    case bcmFieldQualifyIp6:
                    case bcmFieldQualifyPacketFormat:
                        break;
                    default:
                        return BCM_E_UNAVAIL;
                }
                
            } else if (SOC_IS_ROBO53115(unit)) {
            
                switch (i) {
                    case bcmFieldQualifySrcIp6: /* use UDF */
                    case bcmFieldQualifyDstIp6: /* use UDF */
                    case bcmFieldQualifySrcMac: /* use UDF */
                    case bcmFieldQualifyDstMac: /* use UDF */
                    case bcmFieldQualifySnap: /* use UDF */
                    case bcmFieldQualifySrcIp: /* use UDF */
                    case bcmFieldQualifyDstIp: /* use UDF */
                    case bcmFieldQualifyInPort:
                    case bcmFieldQualifyInPorts:
                    case bcmFieldQualifyIp6FlowLabel: /* use UDF */
                    case bcmFieldQualifyOuterVlan:
                    case bcmFieldQualifyOuterVlanId:
                    case bcmFieldQualifyOuterVlanPri:
                    case bcmFieldQualifyOuterVlanCfi:
                    case bcmFieldQualifyInnerVlan:
                    case bcmFieldQualifyInnerVlanId:
                    case bcmFieldQualifyInnerVlanCfi:
                    case bcmFieldQualifyInnerVlanPri:
                    case bcmFieldQualifyL4SrcPort: /* use UDF */
                    case bcmFieldQualifyL4DstPort: /* use UDF */
                    case bcmFieldQualifyEtherType: 
                    case bcmFieldQualifyIpProtocol: /* same as Ip6NextHeader */
                    case bcmFieldQualifyDSCP:   /* same as ToS, Ip6TrafficClass */
                    case bcmFieldQualifyTtl: /* same as Ip6HopLimit */
                    case bcmFieldQualifyTcpControl: /* use UDF */
                    case bcmFieldQualifyPacketFormat:
                    case bcmFieldQualifyIpType:
                    case bcmFieldQualifyStageIngress:
                    case bcmFieldQualifyIpFrag:
                    case bcmFieldQualifyL2Format:
                    case bcmFieldQualifyVlanFormat:
                    case bcmFieldQualifyIpAuth:
                    case bcmFieldQualifyClassId:
                    case bcmFieldQualifyIp4:
                    case bcmFieldQualifyIp6:
                        break;
                    default:
                        return BCM_E_UNAVAIL;
                }
                
            } else { /* other chips */
                return BCM_E_UNAVAIL;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _field_selcode_single_get
 *
 * Purpose:
 *     Calculate the FPFx select codes from a qualifier set for a single slice.
 *
 * Parameters:
 *     unit           - BCM unit
 *     qset           - client qualifier set
 *     sel            - (OUT)
 *
 * Returns:
 *     BCM_E_UNAVAIL  - No select code will satisfy qualifier set
 *     BCM_E_NONE     - Success
 */
STATIC int
_field_selcode_single_get(int unit, bcm_field_qset_t qset,  int8 *slice_id)
{
    int                 retval = BCM_E_UNAVAIL;
    drv_cfp_entry_t     drv_entry;
    uint32  temp;
    uint32 flags = 0;
    int dtag_mode = BCM_PORT_DTAG_MODE_NONE;

    retval = _field_qualify_support(unit, qset);
    if (retval != BCM_E_NONE) {
        return retval;
    }
    
    /* re-init retval value for further use.*/
    retval = BCM_E_UNAVAIL;
    
    sal_memset(&drv_entry, 0, sizeof(drv_cfp_entry_t));
    /* Translate to  driver qualifier set */
    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        if (SHR_BITGET(qset.udf_map, 0)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFA0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 1)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFA1, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 2)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFA2, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 3)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 4)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB1, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 5)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB2, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 6)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB3, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 7)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB4, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 8)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB5, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 9)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB6, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 10)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB7, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 11)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB8, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 12)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB9, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 13)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFB10, &drv_entry, 1);
        }  
        if (SHR_BITGET(qset.udf_map, 14)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFC0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 15)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFC1, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 16)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFC2, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 17)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 18)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD1, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 19)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD2, &drv_entry, 1);
        }   
        if (SHR_BITGET(qset.udf_map, 20)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD3, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 21)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD4, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 22)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD5, &drv_entry, 1);
        }   
        if (SHR_BITGET(qset.udf_map, 23)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD6, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 24)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDFD7, &drv_entry, 1);
        }                                                                 
        /* Add new user defined qualify sets of BCM53242 */
    } else if (SOC_IS_ROBO5395(unit)) {
        if (SHR_BITGET(qset.udf_map, 0)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 1)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4A, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 2)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4B, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 3)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4C, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 4)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF5A, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 5)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF5B, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 6)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF5C, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 7)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF5D, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 8)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF5E, &drv_entry, 1);
        }
    } else if (SOC_IS_ROBO53115(unit)) {
        _field_vulcan_udf_qset_mapping(unit, qset, &drv_entry);
    } else {
        if (SHR_BITGET(qset.udf_map, 0)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF0, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 1)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF2, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 2)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF3A, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 3)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF3B, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 4)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF3C, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 5)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4A, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 6)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4B, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 7)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4C, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 8)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4D, &drv_entry, 1);
        }
        if (SHR_BITGET(qset.udf_map, 9)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_UDF4E, &drv_entry, 1);
        }
    }
    
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcIp6)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_SA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstIp6)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_DA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcIp6High)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstIp6High)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcMac)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_MAC_SA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstMac)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_MAC_DA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySnap)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_SNAP_HEADER, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcIp)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_SA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstIp)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_DA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyLlc)) {
         if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)||
             SOC_IS_ROBO5395(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8022_LLC, &drv_entry, 1);
         } else {
            return retval;
         }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInPort)) {
        if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)
            ||SOC_IS_ROBO5395(unit) ||SOC_IS_ROBO53115(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SRC_PBMP, &drv_entry, 1);
        } else {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SRC_PORT, &drv_entry, 1);
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInPorts)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_SRC_PBMP, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOutPorts)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDrop)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6FlowLabel)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_FLOW_ID, &drv_entry, 1);
    }

    /* VLAN TAG */
    bcm_port_dtag_mode_get(unit, 0, &dtag_mode);

    if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
        if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlan)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_USR_VID, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_USR_PRI, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_USR_CFI, &drv_entry, 1);
        } else {
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanId)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_USR_VID, &drv_entry, 1);
            }
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanPri)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_USR_PRI, &drv_entry, 1);
            }
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanCfi)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_USR_CFI, &drv_entry, 1);
            }
        }
    } else { 
        /* Double tagging Mode */
        if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlan)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SP_VID, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SP_PRI, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SP_CFI, &drv_entry, 1);
        } else {
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanId)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_SP_VID, &drv_entry, 1);
            }
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanPri)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_SP_PRI, &drv_entry, 1);
            }
            if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlanCfi)) {
                (DRV_SERVICES(unit)->cfp_qset_set)
                    (unit, DRV_CFP_QUAL_SP_CFI, &drv_entry, 1);
            }
        }
    }

    
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerVlan)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_VID, &drv_entry, 1);
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_PRI, &drv_entry, 1);
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_CFI, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerVlanId)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_VID, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerVlanPri)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_PRI, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerVlanCfi)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_USR_CFI, &drv_entry, 1);
    }

    
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyLookupStatus)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyRangeCheck)) {
        /* robo chip only support L4 soure port < 1024 */
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L4SRC_LESS1024, &drv_entry, 1);
        /* 
         * Gnat 15843 : The qualifier,  "bcmFieldQualifyRangeCheck", 
         * represented 2 qualifiers, "bcmFieldQualifyL4SrcPort" and 
         * "bcmFieldQualifyL4DstPort", for BCM5348/BCM5347.
         */
        if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L4_SRC, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L4_DST, &drv_entry, 1);
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL4SrcPort)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L4_SRC, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL4DstPort)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L4_DST, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyEtherType)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_ETYPE, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpProtocol)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_PROTO, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDSCP) ||
        BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTos)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_TOS, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTtl)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_TTL, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6NextHeader)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_NEXT_HEADER, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6TrafficClass)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_TRAFFIC_CLASS, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6HopLimit)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP6_HOP_LIMIT, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcModid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcPortTgid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcPort)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_SRC_PORT, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcTrunk)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstModid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstPortTgid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstPort)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstTrunk)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTcpControl)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_TCP_FLAG, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyPacketRes)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyPacketFormat)) {
        if (SOC_IS_ROBO53115(unit)||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L2_FRM_FORMAT, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L3_FRM_FORMAT, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SPTAG, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_1QTAG, &drv_entry, 1);
         } else if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)||
             SOC_IS_ROBO5395(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8023_OR_EII, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8022_LLC, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8022_SNAP, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_SPTAG, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_1QTAG, &drv_entry, 1);
         } else {
            return retval;
         }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcClassL2)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcMacGroup)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcClassL3)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcClassField)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstClassL2)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstClassL2)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstClassL3)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDstClassField)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyPortClass)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpInfo)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpProtocolCommon)) {
        if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L4_FRM_FORMAT, &drv_entry, 1);
        } else {
            return retval;
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyMHOpcode)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpFlags)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerTpid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterTpid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpType)){
        if (SOC_IS_ROBO53115(unit)||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {

            (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L3_FRM_FORMAT, &drv_entry, 1);
        } else if (SOC_IS_ROBO5347(unit)||
                   SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5395(unit)){
            (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IPV4, &drv_entry, 1);
        } else {
            return retval;
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDecap)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyHiGig)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyHiGigProxy)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStage)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageIngressSecurity)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageIngressQoS)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageLookup)) {    
         return retval;    
    }    
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageEgress)) {    
         return retval;    
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageEgressSecurity)) {    
         return retval;    
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyStageExternal)) {    
         return retval;    
    }

    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifySrcIpEqualDstIp)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_SAME, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyEqualL4Port)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L4_SAME, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTcpSequenceZero)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_TCP_SEQ_ZERO, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTcpHeaderSize)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_TCP_HDR_LEN, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInterfaceClassL2)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInterfaceClassL3)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL3IntfGroup)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInterfaceClassPort)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL3Routable)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpFrag)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_FRGA, &drv_entry, 1);
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_NON_FIRST_FRGA, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyVrf)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOutPort)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp4)) {
        if (SOC_IS_ROBO53115(unit)||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L3_FRM_FORMAT, &drv_entry, 1);
        } else if (SOC_IS_ROBO5347(unit)||
                   SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5395(unit)){
            (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IPV4, &drv_entry, 1);
        } else {
            return retval;
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6)) {
        if (SOC_IS_ROBO53115(unit)||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_L3_FRM_FORMAT, &drv_entry, 1);
        } else {
            return retval;
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2Format)) {
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53242(unit) ||
            SOC_IS_ROBO53262(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_L2_FRM_FORMAT, &drv_entry, 1);
        } else if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) ||
            SOC_IS_ROBO5395(unit)) {
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8023_OR_EII, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8022_LLC, &drv_entry, 1);
            (DRV_SERVICES(unit)->cfp_qset_set)
                (unit, DRV_CFP_QUAL_8022_SNAP, &drv_entry, 1);
        } else {
            return retval;
        }
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyVlanFormat)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_1QTAG, &drv_entry, 1);
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_SPTAG, &drv_entry, 1);
    }

    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyExtensionHeaderType)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyExtensionHeaderSubCode)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL4Ports)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyMirrorCopy)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyTunnelTerminated)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyMplsTerminated)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerSrcIp)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerDstIp)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerSrcIp6)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerDstIp6)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerSrcIp6High)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerDstIp6High)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerTtl)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerIpProtocol)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerIpFrag)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyVlanTranslationHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyForwardingVlanValid)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIngressStpState)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2SrcHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2SrcStatic)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2DestHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2StationMove)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL2CacheHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL3SrcHostHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL3DestHostHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyL3DestRouteHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpmcStarGroupHit)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyDosAttack)) {
        return retval;
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIpAuth)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_IP_AUTH, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyClassId)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_CLASS_ID, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyBigIcmpCheck)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_BIG_ICMP_CHECK, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIcmpTypeCode)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_ICMPIGMP_TYPECODE, &drv_entry, 1);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIgmpTypeMaxRespTime)) {
        (DRV_SERVICES(unit)->cfp_qset_set)
            (unit, DRV_CFP_QUAL_ICMPIGMP_TYPECODE, &drv_entry, 1);
    }
    
    retval = ((DRV_SERVICES(unit)->cfp_slice_id_select)
        (unit, &drv_entry, &temp, flags));

    if (retval == BCM_E_NONE) {
        *slice_id = temp;
    } else {

        /* Using UDFs to substitute the qualify */
        if (SOC_IS_ROBO53115(unit)){
            drv_cfp_entry_t temp_drv_entry;
            drv_cfp_qual_udf_info_t qual_udf_info;
            int i, j, k;
            uint32 qual_diff;
            _field_control_t    *fc;


            fc = _robo_field_control_find(unit);
            flags = DRV_CFP_QUAL_REPLACE_BY_UDF;

            /* If the qualify is not support in a slice,*/
            /* using UDF to achieve it */
            retval = ((DRV_SERVICES(unit)->cfp_slice_id_select)
                (unit, &drv_entry, &temp, flags));
            if (retval == BCM_E_NONE) {
                /* find the qualify which will be replaced by UDF */
                /* Get the old qualify set of this slice */
                retval = (DRV_SERVICES(unit)->cfp_slice_to_qset)
                    (unit, temp, &temp_drv_entry);
                if (retval < 0) {
                    return retval;
                }

                 for (i = 0; i < (DRV_CFP_QUAL_COUNT / 32) + 1; i++) {
                    /* Qualifys that need support by UDF */
                    qual_diff  = drv_entry.w[i] & ~(temp_drv_entry.w[i]);
                    if (qual_diff) {
                        for (j=0; j < 32; j++) {
                            if (qual_diff & (0x1 << j)) {
                                sal_memset(&qual_udf_info, 0, 
                                    sizeof(drv_cfp_qual_udf_info_t));
                                retval = (DRV_SERVICES(unit)-> \
                                    cfp_sub_qual_by_udf)( unit, 
                                    1, temp, (i * 32 + j), &qual_udf_info);
                                if (retval < 0) {
                                    return retval;
                                }
                                
                                for (k=0; k < qual_udf_info.udf_num; k++) {
                                    fc->udf[qual_udf_info.udf_index[k]].\
                                        valid = 1;
                                    fc->udf[qual_udf_info.udf_index[k]].\
                                        use_count = 0;
                                    fc->udf[qual_udf_info.udf_index[k]].\
                                        slice_id = temp;
                                }
                                
                            }
                        }
                    }
                }
                

                *slice_id = temp;

                /* Need handle failed case */
            }

        }
        
        
    }
    
   return retval;
}

/*
 * Function:
 *     _field_selcode_get
 *
 * Purpose:
 *     Calculate the FPFx select codes from a qualifier set and group mode.
 *
 * Parameters:
 *     unit           - BCM unit
 *     qset           - client qualifier set
 *     fg             - Group structure
 *
 * Returns:
 *     BCM_E_PARAM    - mode unknown
 *     BCM_E_RESOURCE - No select code will satisfy qualifier set
 *     BCM_E_NONE     - Success
 *
 * Notes:
 *     Calling function is responsible for ensuring appropriate slices
 *     are available.
 */
STATIC int
_field_selcode_get(int unit, bcm_field_qset_t qset, _field_group_t *fg) {
    int                 retval = BCM_E_UNAVAIL;

    assert(fg != NULL);

    switch(fg->mode) {
        case bcmFieldGroupModeSingle:
        case bcmFieldGroupModeAuto:
            BCM_FIELD_QSET_INIT(fg->slices[0].qset);
            retval = _field_selcode_single_get(unit, qset,
                                               &fg->slices[0].sel_codes.fpf);
            if (BCM_FAILURE(retval)) {
                FP_ERR(("FP Error: Failure in _field_selcode_single_get()\n"));
            }
            sal_memcpy(&fg->slices[0].qset, &qset, sizeof(bcm_field_qset_t));
            break;
        case bcmFieldGroupModeDouble:
        case bcmFieldGroupModeTriple:
            return retval;

        default: 
            return BCM_E_PARAM;
    }

    return retval;
}



/*
 * Function:
 *     _field_selcode_to_qset
 * 
 * Purpose:
 *     Get the qset that represents all the qualifiers supported by
 *     the given field selection codes.
 *
 * Parameters:
 *     unit     - BCM device number
 *     selcodes - Field Selection codes
 *     qset     - (OUT) Field qualifier set
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_field_selcode_to_qset(int unit, _field_sel_t selcodes, bcm_field_qset_t *qset)
{
    int     rv = BCM_E_NONE;
    uint32  slice_id;
    drv_cfp_entry_t drv_entry;
    int dtag_mode = BCM_PORT_DTAG_MODE_NONE;

    /* Clear the Qset to start with an empty one. */
    BCM_FIELD_QSET_INIT(*qset);

    sal_memset(&drv_entry, 0, sizeof(drv_cfp_entry_t));
    slice_id = selcodes.fpf;
    
     rv = (DRV_SERVICES(unit)->cfp_slice_to_qset)
        (unit, slice_id, &drv_entry);

    bcm_port_dtag_mode_get(unit, 0, &dtag_mode);
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_SRC_PORT, bcmFieldQualifySrcPort));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_MAC_DA, bcmFieldQualifyDstMac));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_MAC_SA, bcmFieldQualifySrcMac));
    if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_VID, bcmFieldQualifyOuterVlanId));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_PRI, bcmFieldQualifyOuterVlanPri));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_CFI, bcmFieldQualifyOuterVlanCfi));
    } else {
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_SP_VID, bcmFieldQualifyOuterVlanId));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_SP_PRI, bcmFieldQualifyOuterVlanPri));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_SP_CFI, bcmFieldQualifyOuterVlanCfi));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_VID, bcmFieldQualifyInnerVlanId));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_PRI, bcmFieldQualifyInnerVlanPri));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_USR_CFI, bcmFieldQualifyInnerVlanCfi));
    }
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_ETYPE, bcmFieldQualifyEtherType));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_DA, bcmFieldQualifyDstIp));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_SA, bcmFieldQualifySrcIp));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_SAME, bcmFieldQualifySrcIpEqualDstIp));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_L4_DST, bcmFieldQualifyL4DstPort));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_L4_SRC, bcmFieldQualifyL4SrcPort));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_L4SRC_LESS1024, bcmFieldQualifyRangeCheck));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_L4_SAME, bcmFieldQualifyEqualL4Port));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_TCP_SEQ_ZERO, bcmFieldQualifyTcpSequenceZero));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_TCP_HDR_LEN, bcmFieldQualifyTcpHeaderSize));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_TCP_FLAG, bcmFieldQualifyTcpControl));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_PROTO, bcmFieldQualifyIpProtocol));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_TOS, bcmFieldQualifyDSCP));
    BCM_IF_ERROR_RETURN(
    _field_qset_mapping(&drv_entry, qset, 
        DRV_CFP_QUAL_IP_TTL, bcmFieldQualifyTtl));

    /* Add BCM53242 new qualify sets here*/
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {    
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF0, 0));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF2, 1));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF3A, 2));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF3B, 3));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF3C, 4));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4A, 5));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4B, 6));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4C, 7));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4D, 8));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4E, 9));
    } else if (SOC_IS_ROBO5395(unit)) {
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF0, 0));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4A, 1));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4B, 2));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF4C, 3));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF5A, 4));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF5B, 5));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF5C, 6));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF5D, 7));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDF5E, 8));
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* Add BCM53242 new qualify sets */
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA0, 0));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA1, 1));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA2, 2));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB0, 3));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB1, 4));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB2, 5));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB3, 6));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB4, 7));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB5, 8));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB6, 9));
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB7, 10));            
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB8, 11));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB9, 12));                          
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB10, 13));            
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC0, 14));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC1, 15));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC2, 16));            
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD0, 17));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD1, 18));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD2, 19));            
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD3, 20));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD4, 21));                                      
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD5, 22));            
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD6, 23));  
        BCM_IF_ERROR_RETURN(
        _field_udf_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD7, 24));                                        
    } else if (SOC_IS_ROBO53115(unit)) {
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA0, 0));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA1, 1));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA2, 2));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA3, 3));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA4, 4));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA5, 5));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA6, 6));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA7, 7));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA8, 8));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA9, 9));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA10, 10));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA11, 11));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA12, 12));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA13, 13));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA14, 14));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA15, 15));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA16, 16));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA17, 17));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA18, 18));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA19, 19));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA20, 20));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA21, 21));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA22, 22));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA23, 23));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA24, 24));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA25, 25));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFA26, 26));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB0, 27));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB1, 28));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB2, 29));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB3, 30));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB4, 31));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB5, 32));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB6, 33));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB7, 34));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB8, 35));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB9, 36));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB10, 37));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB11, 38));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB12, 39));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB13, 40));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB14, 41));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB15, 42));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB16, 43));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB17, 44));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB18, 45));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB19, 46));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB0, 47));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB21, 48));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB22, 49));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB23, 50));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB24, 51));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB25, 52));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFB26, 53));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC0, 54));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC1, 55));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC2, 56));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC3, 57));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC4, 58));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC5, 59));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC6, 60));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC7, 61));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC8, 62));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC9, 63));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC10, 64));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC11, 65));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC12, 66));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC13, 67));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC14, 68));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC15, 69));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC16, 70));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC17, 71));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC18, 72));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC19, 73));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC20, 74));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC21, 75));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC22, 76));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC23, 77));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC24, 78));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC25, 79));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFC26, 80));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD0, 81));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD1, 82));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD2, 83));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD3, 84));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD4, 85));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD5, 86));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD6, 87));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD7, 88));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD8, 89));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD9, 90));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD10, 91));
        BCM_IF_ERROR_RETURN(
        _field_qset_mapping(&drv_entry, qset, 
            DRV_CFP_QUAL_UDFD11, 92));
        
    }

    return rv;
}


/* 
 * Function:
 *     _robo_field_group_id_generate
 *
 * Purpose:
 *     Find an unused Group ID.
 *
 * Parameters:
 *     unit  - BCM device number
 *     qset - qualify set
 *     group - (OUT) new Group ID
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - null pointer to group
 */
STATIC int
_robo_field_group_id_generate(int unit, bcm_field_qset_t qset, 
        bcm_field_group_t *group, int pri) {

#ifdef SLICE_SUPPORT_MULTI_GROUPS
    _field_control_t    *fc;
    _field_group_t      *fg = NULL;
    int8        slice_id;
    int         rv = BCM_E_NONE;
    bcm_field_qset_t    qset_union;
#endif
    
     if (group == NULL) {
        FP_ERR(("FP Error: group == NULL\n"));
        return BCM_E_PARAM;
    }

#ifdef SLICE_SUPPORT_MULTI_GROUPS
    /* 
     * If pri == BCM_FIELD_GROUP_PRIO_ANY,
     * search the existed groupes.
     * If the group can support the input qset, return this exieted group id.
     */
    if (pri ==BCM_FIELD_GROUP_PRIO_ANY) {     
        /* Check if group(slice id) already created */
        fc = _robo_field_control_find(unit);

        if (fc == NULL) {
            return BCM_E_INTERNAL;
        }

        rv = _field_selcode_single_get(unit, qset, &slice_id);
        if (BCM_FAILURE(rv)) {
            FP_ERR(("FP Error: new Qset won't work.\n"));
            return rv;
        }

        fg = fc->groups;
        while(fg != NULL) {
            if (fg->slices[0].sel_codes.fpf == slice_id) {
                *group = fg->gid;
                sal_memset(&qset_union, 0, sizeof(bcm_field_qset_t));
                /* merge qset to this group */
                _robo_field_qset_union(&fg->slices[0].qset,
                      &qset, &qset_union);
                sal_memcpy(&fg->qset, &qset_union, sizeof(bcm_field_qset_t));
                sal_memcpy(&fg->slices[0].qset, &qset_union, 
                    sizeof(bcm_field_qset_t));
                rv = BCM_E_EXISTS;
                return rv;
            }
            fg = fg->next;
        }
    }

#endif


    *group = _FP_GROUP_ID_BASE;
    while( _robo_field_group_find(unit, *group) != NULL) {
        *group += 1;
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *     _robo_field_group_status_init
 * Purpose:
 *     Fill in the initial fields of a group status struct.
 * Parameters:
 * Returns:
 *     nothing
 */
STATIC void
_robo_field_group_status_init(int slice_indexes, bcm_field_group_status_t *fg_stat) {
    assert(fg_stat != NULL);
    fg_stat->prio_min       = BCM_FIELD_ENTRY_PRIO_HIGHEST;
    fg_stat->prio_max       = 0;

    fg_stat->entries_total  = slice_indexes;
    fg_stat->entries_free   = slice_indexes;
    fg_stat->counters_total  = slice_indexes;
    fg_stat->counters_free   = slice_indexes;
    fg_stat->meters_total  = slice_indexes;
    fg_stat->meters_free   = slice_indexes;
}


/*
 * Function:
 *     _field_qset_union
 *
 * Purpose:
 *     make a union of two qsets
 *
 * Parameters:
 *     qset1      - source 1
 *     qset2      - source 2
 *     qset_union - (OUT) result of union operation
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
int
_robo_field_qset_union(const bcm_field_qset_t *qset1,
                  const bcm_field_qset_t *qset2,
                  bcm_field_qset_t *qset_union)
{
    int                    idx;

    assert(qset_union != NULL);

    /* Perform the union of the qualifier bitmap. */
    for (idx = 0; idx < _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); idx++) {
        qset_union->w[idx] = qset1->w[idx] | qset2->w[idx];
    }

    /* Perform the union of the qualifier udfs. */
    for (idx = 0; idx < _SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS); idx++) {
        qset_union->udf_map[idx] = qset1->udf_map[idx] | qset2->udf_map[idx];
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_qset_is_subset
 * Purpose:
 *     Determine if qset one is a subset of qset two.
 * Parameters:
 *     qset_1
 *     qset_2
 * Returns:
 *     1 - if 'qset_1' is a subset of 'qset_2'
 *     0 - if 'qset_1' is NOT a subset of 'qset_2'
 */
int
_robo_field_qset_is_subset(const bcm_field_qset_t *qset_1,
                      const bcm_field_qset_t *qset_2)
{
    int                 idx;

    for (idx = 0; idx < _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); idx++) {
        if ((qset_1->w[idx] | qset_2->w[idx]) & ~qset_2->w[idx]) {
            return 0;
        }
    }
    return 1;
}


/*
 * Function:
 *     _field_qual_info_get
 * Purpose:
 *     Retrieve memory field access information for the designated qualifier.
 *     Only previously set qualifiers are considered.
 * Parameters:
 *     unit     - BCM Unit
 *     qi (OUT) - Qual Info (offset & width)
 *     f_ent    - physical entry struct
 *     qid      - Qualifier to look up
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - Qualifier not in group's Qset
 *
 *     If successful, returns pointer (using qi) to qualifier entry
 *     information.
 */
int
_robo_field_qual_info_get(int unit, _qual_info_t **qi,
                     _field_entry_t *f_ent, bcm_field_qualify_t qid)
{
    assert(qi        != NULL);
    assert(f_ent     != NULL);
    assert(f_ent->fs != NULL);

    if (!BCM_FIELD_QSET_TEST(f_ent->fs->qset, qid)) {
        /* Only qualifiers that were explicitly requested may be used */
        FP_ERR(("FP Error: qual=%s not in group=%d Qset\n",
                _robo_field_qual_name(qid), f_ent->fs->group->gid));
        return BCM_E_PARAM;
    }

    *qi = f_ent->fs->qual_list;

    while ((*qi != NULL) && ((*qi)->qid != qid)) {
        *qi = (*qi)->next;
    };

    assert((*qi) != NULL);

    FP_VERB(("FP: QualInfo={qid=%s}\n",
             _robo_field_qual_name((*qi)->qid)));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_qual_list_destroy (INTERNAL)
 *
 * Purpose:
 *     Frees the memory for a qualifier linked list.
 *
 * Returns:
 *     None
 */
void
_robo_field_qual_list_destroy(_qual_info_t **f_qual_p)
{
    _qual_info_t  *f_qual, *f_qual_old;

    f_qual = *f_qual_p;

    while (f_qual != NULL) {
        f_qual_old = f_qual;
        f_qual     = f_qual->next;
        sal_memset(f_qual_old, 0, sizeof(_qual_info_t));
        sal_free(f_qual_old);
    }
    *f_qual_p = NULL; /* Null out old list pointer. */
}


/*
 * Function:
 *     _robo_field_qual_value32_set
 * Purpose:
 *     Set a qualifier field with the designated data and mask
 *     (<=32-bit) values.
 */
int
_robo_field_qual_value32_set(int unit, bcm_field_qualify_t qual, 
        _field_entry_t *f_ent, uint32 data, uint32 mask)
{
    uint32  fld_data, fld_mask;
    FP_VERB(("FP: setting qual values data=0x%x, mask=0x%x\n", data, mask));
    fld_data = data;
    fld_mask = mask;
    FP_VERB(("_field_qual_value32_set : data= 0x%x, mask =0x%x\n",
        data, mask));
    return _robo_field_qual_value_set(unit, qual, f_ent, &fld_data, &fld_mask);
}



/*
 * Function:
 *     _field_qual_value_set
 *
 * Purpose:
 *     Set a qualifier field from the designated data and mask arrays.
 *
 * Parameters:
 *     
 *     f_ent - tcam.f0, f1, f2, f3 or ext are outputs.
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - data or mask too big for field
 */
int
_robo_field_qual_value_set(int unit, bcm_field_qualify_t qual, _field_entry_t *f_ent,
                      uint32 *p_data, uint32 *p_mask)
{
    int     rv = BCM_E_NONE;
    uint32      fld_index = 0;
    uint32      temp;
    int    dtag_mode = BCM_PORT_DTAG_MODE_NONE;


    assert(f_ent  != NULL);
    assert(p_data != NULL);
    assert(p_mask != NULL);

    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) || 
        SOC_IS_ROBO5395(unit)) {
        bcm_port_dtag_mode_get(unit, 0, &dtag_mode);
        switch(qual) {
            case bcmFieldQualifySrcMac:
                fld_index = DRV_CFP_FIELD_MAC_SA;
                break;
            case bcmFieldQualifyDstMac:
                fld_index = DRV_CFP_FIELD_MAC_DA;
                break;
            case bcmFieldQualifySrcIp:
                fld_index = DRV_CFP_FIELD_IP_SA;
                break;
            case bcmFieldQualifyDstIp:
                fld_index = DRV_CFP_FIELD_IP_DA;
                break;
            case bcmFieldQualifyOuterVlanId:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_VID;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_VID;
                }
                break;
            case bcmFieldQualifyOuterVlanCfi:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_CFI;
                } else {
                    return BCM_E_INTERNAL;
                }
                break;
            case bcmFieldQualifyOuterVlanPri:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_PRI;
                } else {
                    return BCM_E_INTERNAL;
                }
                break;
            case bcmFieldQualifyInnerVlanId:
                fld_index = DRV_CFP_FIELD_USR_VID;
                break;
            case bcmFieldQualifyInnerVlanCfi:
                fld_index = DRV_CFP_FIELD_USR_CFI;
                break;
            case bcmFieldQualifyInnerVlanPri:
                fld_index = DRV_CFP_FIELD_USR_PRI;
                break;

            case bcmFieldQualifyRangeCheck:
                fld_index = DRV_CFP_FIELD_L4SRC_LESS1024;
                break;
            case bcmFieldQualifyL4SrcPort:
                fld_index = DRV_CFP_FIELD_L4SRC;
                break;
            case bcmFieldQualifyL4DstPort:
                fld_index = DRV_CFP_FIELD_L4DST;
                break;
            case bcmFieldQualifyEtherType:
                fld_index = DRV_CFP_FIELD_ETYPE;
                break;
            case bcmFieldQualifyIpProtocol:
                fld_index = DRV_CFP_FIELD_IP_PROTO;
                break;

            case bcmFieldQualifyDSCP:
                fld_index = DRV_CFP_FIELD_IP_TOS;
                break;
            case bcmFieldQualifyTtl:
                fld_index = DRV_CFP_FIELD_IP_TTL;
                break;
            case bcmFieldQualifySrcPortTgid:
            case bcmFieldQualifySrcPort: 
                fld_index = DRV_CFP_FIELD_SRC_PORT;
                break;
            /* Add for BCM5395 */
            case bcmFieldQualifyInPorts:
                fld_index = DRV_CFP_FIELD_IN_PBMP;
                break;
            case bcmFieldQualifyTcpControl:
                fld_index = DRV_CFP_FIELD_TCP_FLAG;
                break;
            case bcmFieldQualifySrcIpEqualDstIp:
                fld_index = DRV_CFP_FIELD_SAME_IP;
                break;
            case bcmFieldQualifyEqualL4Port:
                fld_index = DRV_CFP_FIELD_SAME_L4PORT;
                break;
            case bcmFieldQualifyTcpSequenceZero:
                fld_index = DRV_CFP_FIELD_TCP_SEQ_ZERO;
                break;
            case bcmFieldQualifyTcpHeaderSize:
                fld_index = DRV_CFP_FIELD_TCP_HDR_LEN;
                break;
            case bcmFieldQualifyIpType:
                fld_index = DRV_CFP_FIELD_IPV4_VALID;
                break;
            default:
                rv = BCM_E_INTERNAL;
                return rv;
        }
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        bcm_port_dtag_mode_get(unit, 0, &dtag_mode);
        switch(qual) {
            case bcmFieldQualifySrcMac:
                fld_index = DRV_CFP_FIELD_MAC_SA;
                break;
            case bcmFieldQualifyDstMac:
                fld_index = DRV_CFP_FIELD_MAC_DA;
                break;
            case bcmFieldQualifySrcIp:
                fld_index = DRV_CFP_FIELD_IP_SA;
                break;
            case bcmFieldQualifyDstIp:
                fld_index = DRV_CFP_FIELD_IP_DA;
                break;
            case bcmFieldQualifyOuterVlanId:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_VID;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_VID;
                }
                break;
            case bcmFieldQualifyOuterVlanCfi:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_CFI;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_CFI;
                }
                break;
            case bcmFieldQualifyOuterVlanPri:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_PRI;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_PRI;
                }
                break;
            case bcmFieldQualifyInnerVlanId:
                fld_index = DRV_CFP_FIELD_USR_VID;
                break;
            case bcmFieldQualifyInnerVlanCfi:
                fld_index = DRV_CFP_FIELD_USR_CFI;
                break;
            case bcmFieldQualifyInnerVlanPri:
                fld_index = DRV_CFP_FIELD_USR_PRI;
                break;

        case bcmFieldQualifyRangeCheck:
            fld_index = DRV_CFP_FIELD_L4SRC_LESS1024;
            break;
        case bcmFieldQualifyL4SrcPort:
            fld_index = DRV_CFP_FIELD_L4SRC;
            break;
        case bcmFieldQualifyL4DstPort:
            fld_index = DRV_CFP_FIELD_L4DST;
            break;
        case bcmFieldQualifyEtherType:
            fld_index = DRV_CFP_FIELD_ETYPE;
            break;
        case bcmFieldQualifyIpProtocol:
            fld_index = DRV_CFP_FIELD_IP_PROTO;
            break;
        case bcmFieldQualifyDSCP:
            fld_index = DRV_CFP_FIELD_IP_TOS;
            break;
        case bcmFieldQualifyTtl:
            fld_index = DRV_CFP_FIELD_IP_TTL;
            break;
        case bcmFieldQualifyTcpControl:
            fld_index = DRV_CFP_FIELD_TCP_FLAG;
            break;
        case bcmFieldQualifySrcIpEqualDstIp:
            fld_index = DRV_CFP_FIELD_SAME_IP;
            break;
        case bcmFieldQualifyEqualL4Port:
            fld_index = DRV_CFP_FIELD_SAME_L4PORT;
            break;
        case bcmFieldQualifyTcpSequenceZero:
            fld_index = DRV_CFP_FIELD_TCP_SEQ_ZERO;
            break;
        case bcmFieldQualifyTcpHeaderSize:
            fld_index = DRV_CFP_FIELD_TCP_HDR_LEN;
            break;
            case bcmFieldQualifyIpType:
                fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
                break;
            case bcmFieldQualifyInPorts:
                fld_index = DRV_CFP_FIELD_IN_PBMP;
                break;
            case bcmFieldQualifyIpProtocolCommon:
                fld_index = DRV_CFP_FIELD_L4_FRM_FORMAT;
                break;
            case bcmFieldQualifyIp6FlowLabel:
                fld_index = DRV_CFP_FIELD_IP6_FLOW_ID;
                break;
            case bcmFieldQualifySrcIp6:
                fld_index = DRV_CFP_FIELD_IP6_SA;
                break;
            case bcmFieldQualifyL2Format:
                fld_index = DRV_CFP_FIELD_L2_FRM_FORMAT;
                break;
            case bcmFieldQualifyBigIcmpCheck:
                fld_index = DRV_CFP_FIELD_BIG_ICMP_CHECK;
                break;
            case bcmFieldQualifyIcmpTypeCode:
            case bcmFieldQualifyIgmpTypeMaxRespTime:
                fld_index = DRV_CFP_FIELD_ICMPIGMP_TYPECODE;
                break;
            /* Add specific Qualifies for BCM53242 below */
        default:
            rv = BCM_E_INTERNAL;
            return rv;
        }
    } else if (SOC_IS_ROBO53115(unit)) {
        bcm_port_dtag_mode_get(unit, 0, &dtag_mode);
        switch (qual) {
            case bcmFieldQualifyInPorts:
                fld_index = DRV_CFP_FIELD_IN_PBMP;
                break;
            case bcmFieldQualifyOuterVlanId:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_VID;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_VID;
                }
                break;
            case bcmFieldQualifyOuterVlanPri:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_PRI;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_PRI;
                }
                break;
            case bcmFieldQualifyOuterVlanCfi:
                if (dtag_mode == BCM_PORT_DTAG_MODE_NONE) {
                    fld_index = DRV_CFP_FIELD_USR_CFI;
                } else {
                    fld_index = DRV_CFP_FIELD_SP_CFI;
                }
                break;
            case bcmFieldQualifyInnerVlanId:
                fld_index = DRV_CFP_FIELD_USR_VID;
                break;
            case bcmFieldQualifyInnerVlanPri:
                fld_index = DRV_CFP_FIELD_USR_PRI;
                break;
            case bcmFieldQualifyInnerVlanCfi:
                fld_index = DRV_CFP_FIELD_USR_CFI;
                break;
            case bcmFieldQualifyL2Format:
                fld_index = DRV_CFP_FIELD_L2_FRM_FORMAT;
                break;
            case bcmFieldQualifyIpType:
                fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
                break;
            case bcmFieldQualifyIpProtocol:
                fld_index = DRV_CFP_FIELD_IP_PROTO;
                break;
            case bcmFieldQualifyDSCP:
                fld_index = DRV_CFP_FIELD_IP_TOS;
                break;
            case bcmFieldQualifyTtl:
                fld_index = DRV_CFP_FIELD_IP_TTL;
                break;
            case bcmFieldQualifyEtherType:
                fld_index = DRV_CFP_FIELD_ETYPE;
                break;

            /* Add qualify which can be support by UDFs */
            case bcmFieldQualifySrcMac:
                fld_index = DRV_CFP_FIELD_MAC_SA;
                break;
            case bcmFieldQualifyDstMac:
                fld_index = DRV_CFP_FIELD_MAC_DA;
                break;
            case bcmFieldQualifySrcIp:
                fld_index = DRV_CFP_FIELD_IP_SA;
                break;
            case bcmFieldQualifyDstIp:
                fld_index = DRV_CFP_FIELD_IP_DA;
                break;
            case bcmFieldQualifySrcIp6:
                fld_index = DRV_CFP_FIELD_IP6_SA;
                break;
            case bcmFieldQualifyDstIp6:
                fld_index = DRV_CFP_FIELD_IP6_DA;
                break;
            case bcmFieldQualifyL4SrcPort:
                fld_index = DRV_CFP_FIELD_L4SRC;
                break;
            case bcmFieldQualifyL4DstPort:
                fld_index = DRV_CFP_FIELD_L4DST;
                break;
            case bcmFieldQualifySnap:
                fld_index = DRV_CFP_FIELD_SNAP_HEADER;
                break;
            case bcmFieldQualifyLlc:
                fld_index = DRV_CFP_FIELD_LLC_HEADER;
                break;
            case bcmFieldQualifyTcpControl:
                fld_index = DRV_CFP_FIELD_TCP_FLAG;
                break;
            case bcmFieldQualifyIp6FlowLabel:
                fld_index = DRV_CFP_FIELD_IP6_FLOW_ID;
                break;
            case bcmFieldQualifyIpAuth:
                fld_index = DRV_CFP_FIELD_IP_AUTH;
                break;

            default:
                rv = BCM_E_INTERNAL;
                return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }


    FP_VERB(("_field_qual_value_set : fld_index = 0x%x\n", fld_index));
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, fld_index, 
            &f_ent->drv_entry, p_data));
    
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
            &f_ent->drv_entry, p_mask)); 

    /*
     * For BCM5348/5347, automatically enable
     * IPv4 valid/TCPUDP valid/TCP frame qualifies
     * by checking if related fields enabled.
     */
    if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)||SOC_IS_ROBO5395(unit)) {
    /* Set IPv4_Valid bit if qualifer is */
    /* DstIP, SrcIP, SrcIPEqualDstIP */
    switch (qual) {
        case bcmFieldQualifyDstIp:
        case bcmFieldQualifySrcIp:
        case bcmFieldQualifySrcIpEqualDstIp:
            fld_index = DRV_CFP_FIELD_IPV4_VALID;
            temp = 1;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, fld_index,
                    &f_ent->drv_entry, &temp));

            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                    &f_ent->drv_entry, &temp));
            break;
        default:
            break;
    }
     /* Set TCP_UDP_Valid bit if qualifer is */
    /* L4DstPort, L4SrcPort, EqualL4Port */
     /* Set TCP_FRAME bit if qualifer is */
    /* TCP_Seq_zero, TCP_Control, TCP_header_Size */
    switch (qual) {
        case bcmFieldQualifyL4DstPort:
        case bcmFieldQualifyL4SrcPort:
        case bcmFieldQualifyEqualL4Port:
        case bcmFieldQualifyRangeCheck: /* Only for L4 srcPort less than 1024 */  
            if (!SOC_IS_ROBO5395(unit)) {
                    fld_index = DRV_CFP_FIELD_TCPUDP_VALID;
                    temp = 1;
                    BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->cfp_field_set)
                        (unit, DRV_CFP_RAM_TCAM, fld_index,
                            &f_ent->drv_entry, &temp));
        
                    BCM_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->cfp_field_set)
                        (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                        &f_ent->drv_entry, &temp));
                }

            break;
        case bcmFieldQualifyTcpSequenceZero:
        case bcmFieldQualifyTcpControl:
        case bcmFieldQualifyTcpHeaderSize:
            fld_index = DRV_CFP_FIELD_TCP_FRAME;
            temp = 1;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, fld_index,
                    &f_ent->drv_entry, &temp));

            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                    &f_ent->drv_entry, &temp));

            break;
        default : 
            break;
    }
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* Set L3_FRAME_FORMAT IPv4 or IPv6 */
        switch (qual) {
            case bcmFieldQualifyDstIp:
            case bcmFieldQualifySrcIp:
            case bcmFieldQualifyDSCP:
            case bcmFieldQualifyIpProtocol:
            case bcmFieldQualifyTtl:
            case bcmFieldQualifySrcIpEqualDstIp:
                fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
                temp = FP_BCM53242_L3_FRM_FMT_IP4;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index,
                        &f_ent->drv_entry, &temp));
    
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                        &f_ent->drv_entry, &temp));
                break;
            case bcmFieldQualifyIp6FlowLabel:
            case bcmFieldQualifySrcIp6:
                fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
                temp = FP_BCM53242_L3_FRM_FMT_IP6;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index,
                        &f_ent->drv_entry, &temp));
    
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                        &f_ent->drv_entry, &temp));
                break;
            default:
                break;
    }

        /* Set L4_FRAME_FORMAT TCP, UDP or ICMPIGMP */
        switch (qual) {
            case bcmFieldQualifyIcmpTypeCode:
            case bcmFieldQualifyIgmpTypeMaxRespTime:
            case bcmFieldQualifyBigIcmpCheck:
                fld_index = DRV_CFP_FIELD_L4_FRM_FORMAT;
                temp = FP_BCM53242_L4_FRM_FMT_ICMPIGMP;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index,
                        &f_ent->drv_entry, &temp));
    
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                        &f_ent->drv_entry, &temp));
    
                break;

            case bcmFieldQualifyTcpSequenceZero:
            case bcmFieldQualifyTcpControl:
            case bcmFieldQualifyTcpHeaderSize:
                fld_index = DRV_CFP_FIELD_L4_FRM_FORMAT;
                temp = FP_BCM53242_L4_FRM_FMT_TCP;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index,
                        &f_ent->drv_entry, &temp));
    
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                        &f_ent->drv_entry, &temp));
    
                break;
            default : 
                break;
        }
    }
  FP_VERB(("_field_qual_value_set : data= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_data[0], f_ent->drv_entry.tcam_data[1], 
        f_ent->drv_entry.tcam_data[2], f_ent->drv_entry.tcam_data[3], 
        f_ent->drv_entry.tcam_data[4], f_ent->drv_entry.tcam_data[5]));
  FP_VERB(("_field_qual_value_set : mask= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_mask[0], f_ent->drv_entry.tcam_mask[1], 
        f_ent->drv_entry.tcam_mask[2], f_ent->drv_entry.tcam_mask[3], 
        f_ent->drv_entry.tcam_mask[4], f_ent->drv_entry.tcam_mask[5]));
    
    f_ent->dirty = _FIELD_DIRTY;
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_udf_value_set
 *
 * Purpose:
 *     Set a UDF qualifier field from the designated data and mask arrays.
 *
 * Parameters:
 *     
 *     f_ent - tcam.f0, f1, f2, f3 or ext are outputs.
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - data or mask too big for field
 */
int
_robo_field_udf_value_set(int unit, bcm_field_udf_t udf_idx, _field_entry_t *f_ent,
                      uint32 *p_data, uint32 *p_mask)
{
    int     rv = BCM_E_NONE;
    uint32      fld_index = 0;
    uint32      temp;


    assert(f_ent  != NULL);
    assert(p_data != NULL);
    assert(p_mask != NULL);

    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) || 
        SOC_IS_ROBO5395(unit)) {
        switch(udf_idx) {
            case 0:
                fld_index = DRV_CFP_FIELD_UDF0;
                break;
            case 1:
                fld_index = DRV_CFP_FIELD_UDF2;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF4A;
                }
                break;
            case 2:
                fld_index = DRV_CFP_FIELD_UDF3A;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF4B;
                }
                break;
            case 3:
                fld_index = DRV_CFP_FIELD_UDF3B;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF4C;
                }
                break;
            case 4:
                fld_index = DRV_CFP_FIELD_UDF3C;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF5A;
                }
                break;
            case 5:
                fld_index = DRV_CFP_FIELD_UDF4A;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF5B;
                }
                break;
            case 6:
                fld_index = DRV_CFP_FIELD_UDF4B;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF5C;
                }
                break;
            case 7:
                fld_index = DRV_CFP_FIELD_UDF4C;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF5D;
                }
                break;
            case 8:
                fld_index = DRV_CFP_FIELD_UDF4D;
                if (SOC_IS_ROBO5395(unit)){
                    fld_index = DRV_CFP_FIELD_UDF5E;
                }
                break;
            case 9:
                fld_index = DRV_CFP_FIELD_UDF4E;
                break;
            default:
                rv = BCM_E_INTERNAL;
                return rv;
        }
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        switch(udf_idx) {
            case 0:
                fld_index = DRV_CFP_FIELD_UDFA0;
                break;
            case 1:
                fld_index = DRV_CFP_FIELD_UDFA1;
                break;
            case 2:
                fld_index = DRV_CFP_FIELD_UDFA2;
                break;
            case 3:
                fld_index = DRV_CFP_FIELD_UDFB0;
                break;
            case 4:
                fld_index = DRV_CFP_FIELD_UDFB1;
                break;
            case 5:
                fld_index = DRV_CFP_FIELD_UDFB2;
                break;
            case 6:
                fld_index = DRV_CFP_FIELD_UDFB3;
                break;
            case 7:
                fld_index = DRV_CFP_FIELD_UDFB4;
                break;
            case 8:
                fld_index = DRV_CFP_FIELD_UDFB5;
                break;
            case 9:
                fld_index = DRV_CFP_FIELD_UDFB6;
                break;
            case 10:
                fld_index = DRV_CFP_FIELD_UDFB7;
                break;
            case 11:
                fld_index = DRV_CFP_FIELD_UDFB8;
                break;
            case 12:
                fld_index = DRV_CFP_FIELD_UDFB9;
                break;
            case 13:
                fld_index = DRV_CFP_FIELD_UDFB10;
                break;
            case 14:
                fld_index = DRV_CFP_FIELD_UDFC0;
                break;
            case 15:
                fld_index = DRV_CFP_FIELD_UDFC1;
                break;
            case 16:
                fld_index = DRV_CFP_FIELD_UDFC2;
                break;
            case 17:
                fld_index = DRV_CFP_FIELD_UDFD0;
                break;
            case 18:
                fld_index = DRV_CFP_FIELD_UDFD1;
                break;
            case 19:
                fld_index = DRV_CFP_FIELD_UDFD2;
                break;
            case 20:
                fld_index = DRV_CFP_FIELD_UDFD3;
                break;
            case 21:
                fld_index = DRV_CFP_FIELD_UDFD4;
                break;
            case 22:
                fld_index = DRV_CFP_FIELD_UDFD5;
                break;
            case 23:
                fld_index = DRV_CFP_FIELD_UDFD6;
                break;
            case 24:
                fld_index = DRV_CFP_FIELD_UDFD7;
                break;
        default:
            rv = BCM_E_INTERNAL;
            return rv;
        }
    } else if (SOC_IS_ROBO53115(unit)) {
        switch (udf_idx) {
            case 0:
            case 9:
            case 18:
                fld_index = DRV_CFP_FIELD_UDFA0;
                break;
            case 1:
            case 10:
            case 19:
                fld_index = DRV_CFP_FIELD_UDFA1;
                break;
            case 2:
            case 11:
            case 20:
                fld_index = DRV_CFP_FIELD_UDFA2;
                break;
            case 3:
            case 12:
            case 21:
                fld_index = DRV_CFP_FIELD_UDFA3;
                break;
            case 4:
            case 13:
            case 22:
                fld_index = DRV_CFP_FIELD_UDFA4;
                break;
            case 5:
            case 14:
            case 23:
                fld_index = DRV_CFP_FIELD_UDFA5;
                break;
            case 6:
            case 15:
            case 24:
                fld_index = DRV_CFP_FIELD_UDFA6;
                break;
            case 7:
            case 16:
            case 25:
                fld_index = DRV_CFP_FIELD_UDFA7;
                break;
            case 8:
            case 17:
            case 26:
                fld_index = DRV_CFP_FIELD_UDFA8;
                break;
                
            case 27:
            case 36:
            case 45:
                fld_index = DRV_CFP_FIELD_UDFB0;
                break;
            case 28:
            case 37:
            case 46:
                fld_index = DRV_CFP_FIELD_UDFB1;
                break;
            case 29:
            case 38:
            case 47:
                fld_index = DRV_CFP_FIELD_UDFB2;
                break;
            case 30:
            case 39:
            case 48:
                fld_index = DRV_CFP_FIELD_UDFB3;
                break;
            case 31:
            case 40:
            case 49:
                fld_index = DRV_CFP_FIELD_UDFB4;
                break;
            case 32:
            case 41:
            case 50:
                fld_index = DRV_CFP_FIELD_UDFB5;
                break;
            case 33:
            case 42:
            case 51:
                fld_index = DRV_CFP_FIELD_UDFB6;
                break;
            case 34:
            case 43:
            case 52:
                fld_index = DRV_CFP_FIELD_UDFB7;
                break;
            case 35:
            case 44:
            case 53:
                fld_index = DRV_CFP_FIELD_UDFB8;
                break;
                
            case 54:
            case 63:
            case 72:
                fld_index = DRV_CFP_FIELD_UDFC0;
                break;
            case 55:
            case 64:
            case 73:
                fld_index = DRV_CFP_FIELD_UDFC1;
                break;
            case 56:
            case 65:
            case 74:
                fld_index = DRV_CFP_FIELD_UDFC2;
                break;
            case 57:
            case 66:
            case 75:
                fld_index = DRV_CFP_FIELD_UDFC3;
                break;
            case 58:
            case 67:
            case 76:
                fld_index = DRV_CFP_FIELD_UDFC4;
                break;
            case 59:
            case 68:
            case 77:
                fld_index = DRV_CFP_FIELD_UDFC5;
                break;
            case 60:
            case 69:
            case 78:
                fld_index = DRV_CFP_FIELD_UDFC6;
                break;
            case 61:
            case 70:
            case 79:
                fld_index = DRV_CFP_FIELD_UDFC7;
                break;
            case 62:
            case 71:
            case 80:
                fld_index = DRV_CFP_FIELD_UDFC8;
                break;
                
            case 81:
                fld_index = DRV_CFP_FIELD_UDFD0;
                break;
            case 82:
                fld_index = DRV_CFP_FIELD_UDFD1;
                break;
            case 83:
                fld_index = DRV_CFP_FIELD_UDFD2;
                break;
            case 84:
                fld_index = DRV_CFP_FIELD_UDFD3;
                break;
            case 85:
                fld_index = DRV_CFP_FIELD_UDFD4;
                break;
            case 86:
                fld_index = DRV_CFP_FIELD_UDFD5;
                break;
            case 87:
                fld_index = DRV_CFP_FIELD_UDFD6;
                break;
            case 88:
                fld_index = DRV_CFP_FIELD_UDFD7;
                break;
            case 89:
                fld_index = DRV_CFP_FIELD_UDFD8;
                break;
            case 90:
                fld_index = DRV_CFP_FIELD_UDFD9;
                break;
            case 91:
                fld_index = DRV_CFP_FIELD_UDFD10;
                break;
            case 92:
                fld_index = DRV_CFP_FIELD_UDFD11;
                break;

            default:
                rv = BCM_E_INTERNAL;
                return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }


    FP_VERB(("_field_qual_value_set : fld_index = 0x%x\n", fld_index));
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, fld_index, 
            &f_ent->drv_entry, p_data));
    
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
            &f_ent->drv_entry, p_mask));
    
    /*
     * Enable related UDF valid bits.
     */
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
    if (udf_idx < 10) {
            switch (udf_idx) {
                case 0:
                    fld_index = DRV_CFP_FIELD_UDF0_VALID;
                    break;
                case 1:
                    fld_index = DRV_CFP_FIELD_UDF2_VALID;
                    break;
                case 2:
                    fld_index = DRV_CFP_FIELD_UDF3A_VALID;
                    break;
                case 3:
                    fld_index = DRV_CFP_FIELD_UDF3B_VALID;
                    break;
                case 4:
                    fld_index = DRV_CFP_FIELD_UDF3C_VALID;
                    break;
                case 5:
                    fld_index = DRV_CFP_FIELD_UDF4A_VALID;
                    break;
                case 6:
                    fld_index = DRV_CFP_FIELD_UDF4B_VALID;
                    break;
                case 7:
                    fld_index = DRV_CFP_FIELD_UDF4C_VALID;
                    break;
                case 8:
                    fld_index = DRV_CFP_FIELD_UDF4D_VALID;
                    break;
                case 9:
                    fld_index = DRV_CFP_FIELD_UDF4E_VALID;
                    break;
                default:
                    break;
            }

            temp = 1;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, fld_index, 
                    &f_ent->drv_entry, &temp));
            
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
                    &f_ent->drv_entry, &temp));
        }
    }
    if (SOC_IS_ROBO5395(unit)) {
        if (udf_idx < 9) {
                switch (udf_idx) {
                    case 0:
                        fld_index = DRV_CFP_FIELD_UDF0_VALID;
                        break;
                    case 1:
                        fld_index = DRV_CFP_FIELD_UDF4A_VALID;
                        break;
                    case 2:
                        fld_index = DRV_CFP_FIELD_UDF4B_VALID;
                        break;
                    case 3:
                        fld_index = DRV_CFP_FIELD_UDF4C_VALID;
                        break;
                    case 4:
                        fld_index = DRV_CFP_FIELD_UDF5A_VALID;
                        break;
                    case 5:
                        fld_index = DRV_CFP_FIELD_UDF5B_VALID;
                        break;
                    case 6:
                        fld_index = DRV_CFP_FIELD_UDF5C_VALID;
                        break;
                    case 7:
                        fld_index = DRV_CFP_FIELD_UDF5D_VALID;
                        break;
                    case 8:
                        fld_index = DRV_CFP_FIELD_UDF5E_VALID;
                        break;
                    default:
                        break;
                }
    
                temp = 1;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index, 
                        &f_ent->drv_entry, &temp));
                
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
                        &f_ent->drv_entry, &temp));
        }
    }

    /*
     * Enable related UDF valid bits of BCM53242 extended UDFs.
     * (bcmFieldQualifyUserDefined10~24)
     */
    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* Add specific code for BCM53242 here */
        if (udf_idx < 25) {
                switch (udf_idx) {
                    case 0:
                        fld_index = DRV_CFP_FIELD_UDFA0_VALID;
                        break;
                    case 1:
                        fld_index = DRV_CFP_FIELD_UDFA1_VALID;
                        break;
                    case 2:
                        fld_index = DRV_CFP_FIELD_UDFA2_VALID;
                        break;
                    case 3:
                        fld_index = DRV_CFP_FIELD_UDFB0_VALID;
                        break;
                    case 4:
                        fld_index = DRV_CFP_FIELD_UDFB1_VALID;
                        break;
                    case 5:
                        fld_index = DRV_CFP_FIELD_UDFB2_VALID;
                        break;
                    case 6:
                        fld_index = DRV_CFP_FIELD_UDFB3_VALID;
                        break;
                    case 7:
                        fld_index = DRV_CFP_FIELD_UDFB4_VALID;
                        break;
                    case 8:
                        fld_index = DRV_CFP_FIELD_UDFB5_VALID;
                        break;
                    case 9:
                        fld_index = DRV_CFP_FIELD_UDFB6_VALID;
                        break;
                    case 10:
                        fld_index = DRV_CFP_FIELD_UDFB7_VALID;
                        break;
                    case 11:
                        fld_index = DRV_CFP_FIELD_UDFB8_VALID;
                        break;
                    case 12:
                        fld_index = DRV_CFP_FIELD_UDFB9_VALID;
                        break;
                    case 13:
                        fld_index = DRV_CFP_FIELD_UDFB10_VALID;
                        break;
                    case 14:
                        fld_index = DRV_CFP_FIELD_UDFC0_VALID;
                        break;
                    case 15:
                        fld_index = DRV_CFP_FIELD_UDFC1_VALID;
                        break;
                    case 16:
                        fld_index = DRV_CFP_FIELD_UDFC2_VALID;
                        break;
                    case 17:
                        fld_index = DRV_CFP_FIELD_UDFD0_VALID;
                        break;
                    case 18:
                        fld_index = DRV_CFP_FIELD_UDFD1_VALID;
                        break;
                    case 19:
                        fld_index = DRV_CFP_FIELD_UDFD2_VALID;
                        break;
                    case 20:
                        fld_index = DRV_CFP_FIELD_UDFD3_VALID;
                        break;
                    case 21:
                        fld_index = DRV_CFP_FIELD_UDFD4_VALID;
                        break;
                    case 22:
                        fld_index = DRV_CFP_FIELD_UDFD5_VALID;
                        break;
                    case 23:
                        fld_index = DRV_CFP_FIELD_UDFD6_VALID;
                        break;
                    case 24:
                        fld_index = DRV_CFP_FIELD_UDFD7_VALID;
                        break;
                    default:
                        break;
                }
    
                temp = 1;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index, 
                        &f_ent->drv_entry, &temp));
                
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
                        &f_ent->drv_entry, &temp));
        }
    }
    
    if (SOC_IS_ROBO53115(unit)) {
        if (udf_idx < 93) {
                switch (udf_idx) {
                    case 0:
                    case 9:
                    case 18:
                        fld_index = DRV_CFP_FIELD_UDFA0_VALID;
                        break;
                    case 1:
                    case 10:
                    case 19:
                        fld_index = DRV_CFP_FIELD_UDFA1_VALID;
                        break;
                    case 2:
                    case 11:
                    case 20:
                        fld_index = DRV_CFP_FIELD_UDFA2_VALID;
                        break;
                    case 3:
                    case 12:
                    case 21:
                        fld_index = DRV_CFP_FIELD_UDFA3_VALID;
                        break;
                    case 4:
                    case 13:
                    case 22:
                        fld_index = DRV_CFP_FIELD_UDFA4_VALID;
                        break;
                    case 5:
                    case 14:
                    case 23:
                        fld_index = DRV_CFP_FIELD_UDFA5_VALID;
                        break;
                    case 6:
                    case 15:
                    case 24:
                        fld_index = DRV_CFP_FIELD_UDFA6_VALID;
                        break;
                    case 7:
                    case 16:
                    case 25:
                        fld_index = DRV_CFP_FIELD_UDFA7_VALID;
                        break;
                    case 8:
                    case 17:
                    case 26:
                        fld_index = DRV_CFP_FIELD_UDFA8_VALID;
                        break;
                        
                    case 27:
                    case 36:
                    case 45:
                        fld_index = DRV_CFP_FIELD_UDFB0_VALID;
                        break;
                    case 28:
                    case 37:
                    case 46:
                        fld_index = DRV_CFP_FIELD_UDFB1_VALID;
                        break;
                    case 29:
                    case 38:
                    case 47:
                        fld_index = DRV_CFP_FIELD_UDFB2_VALID;
                        break;
                    case 30:
                    case 39:
                    case 48:
                        fld_index = DRV_CFP_FIELD_UDFB3_VALID;
                        break;
                    case 31:
                    case 40:
                    case 49:
                        fld_index = DRV_CFP_FIELD_UDFB4_VALID;
                        break;
                    case 32:
                    case 41:
                    case 50:
                        fld_index = DRV_CFP_FIELD_UDFB5_VALID;
                        break;
                    case 33:
                    case 42:
                    case 51:
                        fld_index = DRV_CFP_FIELD_UDFB6_VALID;
                        break;
                    case 34:
                    case 43:
                    case 52:
                        fld_index = DRV_CFP_FIELD_UDFB7_VALID;
                        break;
                    case 35:
                    case 44:
                    case 53:
                        fld_index = DRV_CFP_FIELD_UDFB8_VALID;
                        break;
                        
                    case 54:
                    case 63:
                    case 72:
                        fld_index = DRV_CFP_FIELD_UDFC0_VALID;
                        break;
                    case 55:
                    case 64:
                    case 73:
                        fld_index = DRV_CFP_FIELD_UDFC1_VALID;
                        break;
                    case 56:
                    case 65:
                    case 74:
                        fld_index = DRV_CFP_FIELD_UDFC2_VALID;
                        break;
                    case 57:
                    case 66:
                    case 75:
                        fld_index = DRV_CFP_FIELD_UDFC3_VALID;
                        break;
                    case 58:
                    case 67:
                    case 76:
                        fld_index = DRV_CFP_FIELD_UDFC4_VALID;
                        break;
                    case 59:
                    case 68:
                    case 77:
                        fld_index = DRV_CFP_FIELD_UDFC5_VALID;
                        break;
                    case 60:
                    case 69:
                    case 78:
                        fld_index = DRV_CFP_FIELD_UDFC6_VALID;
                        break;
                    case 61:
                    case 70:
                    case 79:
                        fld_index = DRV_CFP_FIELD_UDFC7_VALID;
                        break;
                    case 62:
                    case 71:
                    case 80:
                        fld_index = DRV_CFP_FIELD_UDFC8_VALID;
                        break;
                        
                    case 81:
                        fld_index = DRV_CFP_FIELD_UDFD0_VALID;
                        break;
                    case 82:
                        fld_index = DRV_CFP_FIELD_UDFD1_VALID;
                        break;
                    case 83:
                        fld_index = DRV_CFP_FIELD_UDFD2_VALID;
                        break;
                    case 84:
                        fld_index = DRV_CFP_FIELD_UDFD3_VALID;
                        break;
                    case 85:
                        fld_index = DRV_CFP_FIELD_UDFD4_VALID;
                        break;
                    case 86:
                        fld_index = DRV_CFP_FIELD_UDFD5_VALID;
                        break;
                    case 87:
                        fld_index = DRV_CFP_FIELD_UDFD6_VALID;
                        break;
                    case 88:
                        fld_index = DRV_CFP_FIELD_UDFD7_VALID;
                        break;
                    case 89:
                        fld_index = DRV_CFP_FIELD_UDFD8_VALID;
                        break;
                    case 90:
                        fld_index = DRV_CFP_FIELD_UDFD9_VALID;
                        break;
                    case 91:
                        fld_index = DRV_CFP_FIELD_UDFD10_VALID;
                        break;
                    case 92:
                        fld_index = DRV_CFP_FIELD_UDFD11_VALID;
                        break;
                    default:
                        break;
                }
    
                temp = 1;
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM, fld_index, 
                        &f_ent->drv_entry, &temp));
                
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->cfp_field_set)
                    (unit, DRV_CFP_RAM_TCAM_MASK, fld_index, 
                        &f_ent->drv_entry, &temp));
        }
    }

  FP_VERB(("_field_udf_value_set : data= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_data[0], f_ent->drv_entry.tcam_data[1], 
        f_ent->drv_entry.tcam_data[2], f_ent->drv_entry.tcam_data[3], 
        f_ent->drv_entry.tcam_data[4], f_ent->drv_entry.tcam_data[5]));
  FP_VERB(("_field_udf_value_set : mask= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_mask[0], f_ent->drv_entry.tcam_mask[1], 
        f_ent->drv_entry.tcam_mask[2], f_ent->drv_entry.tcam_mask[3], 
        f_ent->drv_entry.tcam_mask[4], f_ent->drv_entry.tcam_mask[5]));
    
    f_ent->dirty = _FIELD_DIRTY;
    
    return BCM_E_NONE;
}


/*
 * Function:
 *     _field_group_prio_make
 *
 * Purpose:
 *     Generate an arbitrary priority choice if none is given.
 *
 * Returns:
 *     BCM_E_RESOURCE - no group available
 *     BCM_E_NONE     - Success
 */
STATIC int
_field_group_prio_make(_field_control_t *fc, int *pri)
{
    int                 prior;

    assert(fc != NULL);

    for (prior = 0; prior < fc->field_status.group_total; prior++) {
        if (fc->slices[prior].group == NULL) {
            *pri = prior;
            return BCM_E_NONE;
        }
    }

    FP_ERR(("FP Error: No Group available.\n"));
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     _robo_field_slice_clear
 *
 * Purpose:
 *     Reset the fields in a slice. Note that the entries list must be
 *     empty before calling this. Also, this does NOT deallocate the memory for
 *     the slice itself. Normally, this is used when a group no longer needs
 *     ownership of a slice so the slice gets returned to the pool of available
 *     slices.
 *
 * Paramters:
 *     unit - BCM device number
 *     fs   - Link to physical slice structure to be cleared
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_BUSY      - Entries still in slice, can't clear slice
 */
STATIC int
_robo_field_slice_clear(int unit, _field_control_t *fc, _field_slice_t *fs)
{
    uint16              slice_idx;

    assert(fc != NULL);
    assert(fs != NULL);

    for (slice_idx = 0; slice_idx < fc->tcam_sz; slice_idx++) {
        if (fs->entries[slice_idx] != NULL) {
            FP_ERR(("FP Error: Entries still in slice=%d.\n", fs->slice_numb));
            return BCM_E_BUSY;
        }
    }

    if (SOC_IS_ROBO53115(unit)) {
        int i;
        int retval;
        _field_group_t *fg;
        int another_slice_use = 0;

        fg = fc->groups;
        while (fg != NULL) {
            if ((fg->slices->sel_codes.fpf == fs->sel_codes.fpf) &&
                (fg->gid != fs->group->gid)) {
                another_slice_use =1;
                break;
            }
            fg = fg->next;
        }

        if (another_slice_use == 0) {
        /* Check if this qualify can be substitute by UDFs*/
        retval = (DRV_SERVICES(unit)->cfp_sub_qual_by_udf)(
            unit, 0, fs->sel_codes.fpf, 0, NULL);
        if (retval < 0) {
            return retval;
        }
        /* Recover the UDF translate by qual */
        for (i = 0; i < BCM_FIELD_USER_NUM_UDFS_MAX; i++) {
            if ((fc->udf[i].valid) && (fc->udf[i].slice_id == fc->udf[i].slice_id)) {
                fc->udf[i].valid = 0;

            }
        }
        }
    }

    fs->group = NULL; /* slice no longer belongs to any group */
    BCM_FIELD_QSET_INIT(fs->qset);
    _FIELD_SELCODE_CLEAR(unit, fs->sel_codes);

    /* No need to clear select codes */

    /* Free slice's qualifier info list */
    /* _field_qual_list_destroy(&fs->qual_list);*/
    fs->qual_list = NULL;

    return BCM_E_NONE;
}


/*
 * Function: _field_group_init
 *     
 * Purpose:
 *
 * Parameters:
 *     unit          - BCM device number.
 *     group         - Group ID
 *     mode          - Group mode (single, double, triple or Auto-wide)
 *     slice_base    - Primary slice number for group
 *     qset_p        - pointer to group's qualifier set
 *     slice_indexes -
 *     fg            - (OUT) initialized group structure
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - No select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */

STATIC int
_field_group_init(int unit, bcm_field_group_t group,
                  bcm_field_group_mode_t mode, int slice_base,
                  bcm_field_qset_t *qset_p, int slice_indexes,
                  _field_group_t *fg)
{

    int                 retval = BCM_E_NONE;
    _field_control_t    *fc = NULL;

    fc = _robo_field_control_find(unit);
    assert(fc != NULL);


    /* Check if this slice is used */
    if (fc->slices[slice_base].group != NULL) {
        return BCM_E_CONFIG;
    }
    if (_field_shared_entries_free == 0) {
        return BCM_E_RESOURCE;
    }
    
    sal_memset(fg, 0, sizeof (_field_group_t));

    fg->unit       = unit;
    fg->gid        = group;
    fg->mode       = mode;
    fg->slices     = &fc->slices[slice_base];
#ifdef LVL7_FIXUP
    fg->priority     = slice_base;
#endif
    sal_memcpy(&(fg->qset), qset_p, sizeof (bcm_field_qset_t));

    _FIELD_SELCODE_CLEAR(unit, fg->slices[0].sel_codes);
    BCM_FIELD_QSET_INIT(fg->slices[0].qset);
    retval = _field_selcode_get(unit, fg->qset, fg);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: Failure in _field_selcode_get()\n"));
        return retval;
    }
    fg->slices[0].inst_flg = 0;
    fg->slices[0].group = fg;

    _robo_field_group_status_init(slice_indexes, &fg->group_status);

    /* Write group parameters to hardware. */
    /* Check Me */
    /* Add field type map checking */
    /* BCM_IF_ERROR_RETURN(fc->functions.fp_group_install(unit, fg)); */

    return retval;
}

/*
 * Function:
 *     _field_status_init
 * Purpose:
 *     Initialize field status info for device.
 *
 * Parameters:
 *     unit         - BCM device number
 *     fc           - field control info for device
 *     field_status - (OUT) Status for field module
 *
 * Returns:
 *     none
 */
STATIC void
_field_status_init(int unit, _field_control_t *fc,
                   bcm_field_status_t *field_status) {
    assert(fc           != NULL);
    assert(field_status != NULL);

    /* initialize field_status */
    field_status->group_total = 0;
    field_status->group_total = fc->tcam_slices;
    field_status->group_free  = field_status->group_total;
    if (soc_feature(unit, soc_feature_field_slice_enable)) {
        field_status->flags |= BCM_FIELD_STAT_SLICE_EN;
    }
}

/*
 * Function:
 *     _field_slice_array_init
 *
 * Purpose:
 *     Initialize field slice array within the field control struct.
 *
 * Parameters:
 *     fc - field control info for device
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Allocation failure
 */
STATIC int
_field_slice_array_init(_field_control_t *fc) {
    int                slice;

    assert(fc != NULL);

    /* Allocate slice array within _field_control */
    fc->slices = sal_alloc(sizeof(struct _field_slice_s)
                           * fc->field_status.group_total, "slices info");
    if (fc->slices == NULL) {
        FP_ERR(("FP Error: Allocation failure for Slices Info\n"));
        return BCM_E_MEMORY;
    }

    /* Initialize the slice array. */
    sal_memset(fc->slices, 0, sizeof(_field_slice_t) * fc->tcam_slices);
    for (slice = 0; slice < fc->tcam_slices; slice++) {
        BCM_FIELD_QSET_INIT(fc->slices[slice].qset);
        fc->slices[slice].slice_numb = slice;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_status_init
 * Purpose:
 *     Initialize tcam related info in Field Control
 *
 * Parameters:
 *     unit  - BCM device number
 *     fc    - (OUT) field control info for device
 *
 * Returns:
 *     none
 */
STATIC void
_robo_field_tcam_info_init(int unit, _field_control_t *fc) {
    uint32  entry_size;
    uint32  tcam_size;
    assert(fc != NULL);

    DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_TCAM_SIZE, &tcam_size);
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit) || SOC_IS_ROBO53115(unit)) {
        entry_size = soc_property_get(unit, spn_BCM_FIELD_ENTRY_SZ, 
            tcam_size);
        /* Need to define new for 53115 FPGA broad */
        fc->tcam_sz = entry_size;
        fc->tcam_slices = 16; /* 4 group, since chips' max slice map is 4 */
        fc->tcam_slice_sz            = fc->tcam_sz /fc->tcam_slices;
    }
}

/*
 * Function: 
 *     _robo_field_udf_usecount_increment
 *
 * Purpose:
 *     Increment the use counts for any UDFs used
 *
 * Parameters:
 *     fc - unit's field control struct
 *     fg - group's metadata struct
 *
 * Returns:
 *     BCM_E_RESOURCE - Invalid UDF
 *     BCM_E_NONE     - Success
 *
 */
STATIC int
_robo_field_udf_usecount_increment(_field_control_t *fc, _field_group_t *fg) {
    int                 idx;

    for (idx = 0; idx < BCM_FIELD_USER_NUM_UDFS; idx++) {
        if (SHR_BITGET(fg->qset.udf_map, idx)) {
            fc->udf[idx].use_count++;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _robo_field_entry_phys_destroy
 *
 * Purpose:
 *     Destroy a physical entry from a slice. Note that this does not remove
 *     the entry from the hardware.
 *
 * Parameters:
 *     unit      - BCM device number
 *     fs        - slice that entry resides in
 *     entry     - Entry ID to remove
 *
 * Returns:
 *     BCM_E_NONE - Success
 */   
STATIC int
_robo_field_entry_phys_destroy(int unit, _field_entry_t *f_ent)
{
    _field_slice_t      *fs;
    _field_action_t     *fa;

    fs = f_ent->fs;
    assert(fs    != NULL);
    assert(f_ent != NULL);

    /* remove entry's counter if it exists */
    if (f_ent->counter != NULL) {
        BCM_IF_ERROR_RETURN(bcm_field_counter_destroy(unit, f_ent->eid));
    }

    /* Remove all actions from entry. */
    fa = f_ent->actions;
    while (fa != NULL) {
        BCM_IF_ERROR_RETURN(bcm_field_action_remove(unit, f_ent->eid, fa->action));
        fa = f_ent->actions;
    }

    /* remove entry's meter if it exists */
    if (f_ent->meter != NULL) {
        BCM_IF_ERROR_RETURN(bcm_field_meter_destroy(unit, f_ent->eid));
    }

    /* Remove entry from slice's entry array. */
    fs->entries[f_ent->slice_idx] = NULL;
#ifdef BCM_FIELD_SHARED_ENTRIES
    _field_shared_entries[f_ent->slice_idx] = NULL;
#endif /* BCM_FIELD_SHARED_ENTRIES */

    /* okay to free entry */
    sal_free(f_ent);

    return BCM_E_NONE;
}

/* Function: _field_robo_reqd_prio_set_move
 *
 * Purpose: Checks if the entry needs to be moved due to prio set
 *
 * Parameters: 
 *   unit - 
 *   f_ent - Entry whose priority is being set
 *   prio - The new priority for f_ent
 *
 * Returns:
 *   TRUE/FALSE
 */
int
_field_robo_reqd_prio_set_move(int unit, _field_entry_t *f_ent, int prio)
{
    _field_control_t    *fc = NULL;

    int i, flag; /* flag denotes is we are before OR after f_ent */

    fc = _robo_field_control_find(unit);
    flag = -1; /* We are before f_ent */
    for (i = 0; i < fc->tcam_sz; i++) {
        if (_field_shared_entries[i] == f_ent) {
            flag = 1; /* Now, we are after f_ent */
            continue;
        }
        if (_field_shared_entries[i] == NULL) {
            continue;
        }
        if (flag == -1) {
            if (_robo_field_entry_prio_cmp(_field_shared_entries[i]->prio, prio) < 0) {
        /* 
                 * An entry before f_ent has lower priority than prio
                 *     Movement is required 
                 */
                return TRUE; 
            }
        } else {
            if (_robo_field_entry_prio_cmp(_field_shared_entries[i]->prio, prio) > 0) {
        /* 
                 * An entry after f_ent has higher priority than prio
                 *     Movement is required 
                 */
                return TRUE;
            }
        }
    }
    return FALSE; /* f_ent with new prio is in proper location */
}

/*
 * Function:
 *  _robo_field_sw_counter_get
 * Description:
 *  Service routine used to retrieve 64-bit software accumulated counters.
 * Parameters:
 *   unit     - (IN) BCM device number. 
 *   fc       - (IN) Field control structure. 
 *   stage_fc - (IN) Stage field control structure.
 *   idx      - (IN) Counter hw index. 
 *   value    - (OUT) Counter index 
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_robo_field_sw_counter_get(int unit, _field_counter_t *f_cnt, 
            int counter_num, uint64 *value)
{
    uint32                      temp; /* HW 32 bit counter value.*/
    int rv = BCM_E_NONE;
    uint32      diff;
    uint64      diff64;
    
    /* Input parameters check. */
    if ((NULL == value) || (NULL == f_cnt)) { 
        return (BCM_E_PARAM);
    }
    
    COMPILER_64_ZERO(*value);

    /* Get counters from Hw. */
    if (counter_num == 0) {
        rv = (DRV_SERVICES(unit)->cfp_stat_get)
            (unit, DRV_CFP_STAT_INBAND, f_cnt->index, &temp);
        if (rv < 0) {
            return rv;
        }
    
        if (f_cnt->_field_x32_counters.last_hw_value_0 <= temp) {
            diff = temp - f_cnt->_field_x32_counters.last_hw_value_0;
        } else {
            diff = (0xffffffff) - 
                (f_cnt->_field_x32_counters.last_hw_value_0 - temp) + 1;
        }
    
        COMPILER_64_SET(diff64, 0, diff);
        COMPILER_64_ADD_64(
            f_cnt->_field_x32_counters.accumulated_counter_0, diff64);
    
        f_cnt->_field_x32_counters.last_hw_value_0 = temp;
        COMPILER_64_OR(*value, f_cnt->_field_x32_counters.accumulated_counter_0);
    } else {
        rv = (DRV_SERVICES(unit)->cfp_stat_get)
            (unit, DRV_CFP_STAT_OUTBAND, f_cnt->index, &temp);

        if (rv < 0) {
            return rv;
        }
    
        if (f_cnt->_field_x32_counters.last_hw_value_1 <= temp) {
            diff = temp - f_cnt->_field_x32_counters.last_hw_value_1;
        } else {
            diff = (0xffffffff) - 
                (f_cnt->_field_x32_counters.last_hw_value_1 - temp) + 1;
        }

        COMPILER_64_SET(diff64, 0, diff);
        COMPILER_64_ADD_64(
            f_cnt->_field_x32_counters.accumulated_counter_1, diff64);

        f_cnt->_field_x32_counters.last_hw_value_1 = temp;
        COMPILER_64_OR(*value, f_cnt->_field_x32_counters.accumulated_counter_1);
    }

    return (BCM_E_NONE);
}



/*
 * Function:
 *  _robo_field_sw_counter_set
 * Description:
 *  Service routine used to retrieve 64-bit software accumulated counters.
 * Parameters:
 *   unit     - (IN) BCM device number. 
 *   f_cnt - (IN) field counter structure.
 *   value    - (OUT) Counter index 
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *   accumulated_counter is set to value, and
 *   last_hw_value is set in appropriate number of bits
 */
STATIC int
_robo_field_sw_counter_set(int unit, _field_counter_t *f_cnt, int idx, uint64 value)
{
    
    /* Input parameters check. */
    if (NULL == f_cnt) { 
        return (BCM_E_PARAM);
    }

    if (idx == 0) {
        COMPILER_64_ZERO(f_cnt->_field_x32_counters.accumulated_counter_0);
        COMPILER_64_OR(f_cnt->_field_x32_counters.accumulated_counter_0, value);
        COMPILER_64_TO_32_LO(f_cnt->_field_x32_counters.last_hw_value_0, value);
    } else {
        COMPILER_64_ZERO(f_cnt->_field_x32_counters.accumulated_counter_1);
        COMPILER_64_OR(f_cnt->_field_x32_counters.accumulated_counter_1, value);
        COMPILER_64_TO_32_LO(f_cnt->_field_x32_counters.last_hw_value_1, value);
    }
    
    return (BCM_E_NONE);
}


STATIC int
_field_tcam_sync_entry(int unit, _field_control_t *fc, uint32 index)
{
    int retval = BCM_E_NONE;
    drv_cfp_entry_t temp_entry;
    _field_entry_t      *f_ent;
    uint32              temp;

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    f_ent = _field_shared_entries[index];
    
    sal_memset(&temp_entry, 0, sizeof(drv_cfp_entry_t));
    /* Check if this entry is valid or invalid */
    if (f_ent) {
        /* Valid */
        /* Ignore the dirty entries */
        if (f_ent->dirty == _FIELD_CLEAN) {
            retval = (DRV_SERVICES(unit)->cfp_entry_read)
                (unit, index, DRV_CFP_RAM_TCAM, &temp_entry);
            if (BCM_FAILURE(retval)) {
                sal_mutex_give(fc->fc_lock);
                return retval;
            }
            /* 
             Since the Valid field of TCAM Mask always present as zero,
             We need set this field before comparison.
             */
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                temp = 0xf;
            } else {
                temp = 0x3;
            }
            (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_VALID, 
                &temp_entry, &temp);
            if ((sal_memcmp(&temp_entry.tcam_data, 
                &f_ent->drv_entry.tcam_data, sizeof(temp_entry.tcam_data)) !=0 ) ||
                (sal_memcmp(&temp_entry.tcam_mask, 
                &f_ent->drv_entry.tcam_mask, sizeof(temp_entry.tcam_mask)))) {
                FP_VERB(("%s, DIFF : index = %d\n",__FUNCTION__, index));
                FP_VERB(("data : old = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, \
                    0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                    f_ent->drv_entry.tcam_data[0], f_ent->drv_entry.tcam_data[1], 
                    f_ent->drv_entry.tcam_data[2], f_ent->drv_entry.tcam_data[3],
                    f_ent->drv_entry.tcam_data[4], f_ent->drv_entry.tcam_data[5],
                    f_ent->drv_entry.tcam_data[6], f_ent->drv_entry.tcam_data[7],
                    f_ent->drv_entry.tcam_data[8], f_ent->drv_entry.tcam_data[9],
                    f_ent->drv_entry.tcam_data[10], f_ent->drv_entry.tcam_data[11]));
                FP_VERB(("data : new = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, \
                    0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                    temp_entry.tcam_data[0], temp_entry.tcam_data[1], 
                    temp_entry.tcam_data[2], temp_entry.tcam_data[3],
                    temp_entry.tcam_data[4], temp_entry.tcam_data[5],
                    temp_entry.tcam_data[6], temp_entry.tcam_data[7],
                    temp_entry.tcam_data[8], temp_entry.tcam_data[9],
                    temp_entry.tcam_data[10], temp_entry.tcam_data[11]));
                FP_VERB(("mask : old = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, \
                    0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                    f_ent->drv_entry.tcam_mask[0], f_ent->drv_entry.tcam_mask[1], 
                    f_ent->drv_entry.tcam_mask[2], f_ent->drv_entry.tcam_mask[3],
                    f_ent->drv_entry.tcam_mask[4], f_ent->drv_entry.tcam_mask[5],
                    f_ent->drv_entry.tcam_mask[6], f_ent->drv_entry.tcam_mask[7],
                    f_ent->drv_entry.tcam_mask[8], f_ent->drv_entry.tcam_mask[9],
                    f_ent->drv_entry.tcam_mask[10], f_ent->drv_entry.tcam_mask[11]));
                FP_VERB(("mask : new = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, \
                    0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                    temp_entry.tcam_mask[0], temp_entry.tcam_mask[1], 
                    temp_entry.tcam_mask[2], temp_entry.tcam_mask[3],
                    temp_entry.tcam_mask[4], temp_entry.tcam_mask[5],
                    temp_entry.tcam_mask[6], temp_entry.tcam_mask[7],
                    temp_entry.tcam_mask[8], temp_entry.tcam_mask[9],
                    temp_entry.tcam_mask[10], temp_entry.tcam_mask[11]));
                /* write the software copy to chip */
                retval = (DRV_SERVICES(unit)->cfp_entry_write)
                    (unit, index, DRV_CFP_RAM_TCAM, &f_ent->drv_entry);
            }
        }
    } else {
        /* Invalid entry only need to check the valid bits */
        retval = (DRV_SERVICES(unit)->cfp_entry_write)
            (unit, index, DRV_CFP_RAM_TCAM_INVALID, &temp_entry);
        if (BCM_FAILURE(retval)) {
            sal_mutex_give(fc->fc_lock);
            return retval;
        }
    }
        
    sal_mutex_give(fc->fc_lock);

    return retval;
}


/*
 * Function:
 *      soc_robo_counter_thread
 * Purpose:
 *      Master counter collection and accumulation thread.
 * Parameters:
 *      unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *      Nothing, does not return.
 */
STATIC void
_field_tcam_thread(void *unit_vp)
{
    int             unit = PTR_TO_INT(unit_vp);
    int             rv = BCM_E_NONE;
    int             interval;
    _field_control_t        *fc;
    sal_usecs_t     stime, etime;
    int         chunk_index, chunk_size;
    uint32         i;


    fc = _robo_field_control_find(unit);
    if (fc == NULL) {
        return;
    }
    /*
     * There's a race condition since the PID is used as a
     * signal of whether the counter thread is running.  We sleep
     * here to make sure it gets initialized properly in SOC_CONTROL
     * by the thread_create return.
     */

    sal_sleep(1);

    /*
     * Create a semaphore used to time the trigger scans.
     */

    fc->tcam_notify = sal_sem_create("tcam notify", sal_sem_BINARY, 0);

    if (fc->tcam_notify == NULL) {
        FP_ERR(("_field_tcam_thread: semaphore init failed\n"));
        rv = BCM_E_INTERNAL;
        goto done;
    }

    chunk_index = 0;
    chunk_size =  fc->tcam_slice_sz;

    while ((interval = fc->tcam_interval) != 0) {

        FP_ERR(("_field_tcam_thread: Process %d-%d\n",
             chunk_index, chunk_index + chunk_size - 1));

        stime = sal_time_usecs();

        for (i = chunk_index; i < (chunk_index + chunk_size); i++) {
            /* sync entry */
            _field_tcam_sync_entry(unit, fc, i);
        }
        /*
         * Implement the sleep using a semaphore timeout so if the task
         * is requested to exit, it can do so immediately.
         */

        etime = sal_time_usecs();
        FP_VERB(("_field_tcam_thread: unit=%d: done in %d usec\n",
                 unit,
                 SAL_USECS_SUB(etime, stime)));
        
        if ((chunk_index += chunk_size) >= fc->tcam_sz) {
            chunk_index = 0;
        }

        sal_sem_take(fc->tcam_notify, interval / fc->tcam_slices);
    }

 done:
    if (rv < 0) {
        FP_ERR(("_field_tcam_thread: Operation failed; exiting\n"));
    }


    FP_VERB(("_field_tcam_thread: exiting\n"));

    if (fc->tcam_notify) {
        sal_sem_destroy(fc->tcam_notify);
    }
    fc->tcam_pid = SAL_THREAD_ERROR;
    fc->tcam_interval = 0;

    sal_thread_exit(0);
}


int 
_robo_field_thread_stop(int unit)
{
    _field_control_t    *fc;
    soc_timeout_t   to;

    FIELD_IS_INIT(unit);
    FP_VVERB(("FP: _robo_field_thread_stop(%d)\n", unit));

    fc = _field_control[unit];

    /* Stop TCAM protection thread */ 
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53242(unit) || 
        SOC_IS_ROBO53262(unit)) {
        fc->tcam_interval = 0;
        if (fc->tcam_pid != SAL_THREAD_ERROR) {
            if (fc->tcam_notify) {
                sal_sem_give(fc->tcam_notify);
            }
            /* Give thread a few seconds to wake up and exit */
            soc_timeout_init(&to, BCM_ROBO_FIELD_TCAM_THREAD_INTERVAL, 0);

            while (fc->tcam_pid != SAL_THREAD_ERROR) {
                if (soc_timeout_check(&to)) {
                    FP_ERR(("_robo_field_thread_stop: thread did not exit\n"));
                    fc->tcam_pid = SAL_THREAD_ERROR;
                    break;
                }

                sal_usleep(10000);
            }
        }
    }

    return BCM_E_NONE;
}


#ifdef BROADCOM_DEBUG
/*
 * Function:
 *     _robo_field_qual_name
 * Purpose:
 *     Translate a Qualifier enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_qualify_e. (ex.bcmFieldQualifyInPorts)
 * Returns:
 *     Text name of indicated qualifier enum value.
 */
STATIC char *
_robo_field_qual_name(bcm_field_qualify_t qid)
{
    /* Text names of the enumerated qualifier IDs. */
    static char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;

    return (qid >= bcmFieldQualifyCount ? "??" : qual_text[qid]);
}

/*
 * Function:
 *     _robo_field_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
STATIC char *
_robo_field_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    static char *action_text[] = BCM_FIELD_ACTION_STRINGS;
    assert(COUNTOF(action_text)     == bcmFieldActionCount);

    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

/*
 * Function:
 *     _robo_field_group_mode_name
 * Purpose:
 *     Return text name of indicated group mode enum value.
 */
STATIC char *
_robo_field_group_mode_name(bcm_field_group_mode_t mode)
{
    static char *mode_text[bcmFieldGroupModeCount] =
                 BCM_FIELD_GROUP_MODE_STRINGS;

    return (mode >= bcmFieldGroupModeCount ? "??" : mode_text[mode]);
}

#endif /* BROADCOM_DEBUG */

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *     bcm_robo_field_group_flush
 * Purpose:
 *     Flush the entries belonging to a given group; Removes
 *     hw entries from the slice and destroys sw instances;
 *     Destroys group sw object as well
 * Parameters:
 *     unit   - BCM device number
 *     group  - Field Group ID
 * Returns:
 *     BCM_E_PARAM
 *     BCM_E_NONE
 */
int
bcm_robo_field_group_flush(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}


#else

int
bcm_robo_field_group_flush(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

#endif /* BCM_WARM_BOOT_SUPPORT */



/*
 * Function: bcm_robo_field_action_add
 *
 * Purpose:
 *     Add action performed when entry rule is matched for a packet
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - entry ID
 *     action - Action to perform (bcmFieldActionXXX)
 *     param0 - Action parameter (use 0 if not required)
 *     param1 - Action parameter (use 0 if not required)
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_MEMORY    - Allocation failure
 *     BCM_E_RESOURCE  - Conflicting actions exist in entry.
 *     BCM_E_RESOURCE  - Counter not previously created for entry.
 *     BCM_E_PARAM     - param0 or param1 out of range for action
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_action_add(int unit,
             bcm_field_entry_t entry,
             bcm_field_action_t action,
             uint32 param0,
             uint32 param1)
{
    _field_entry_t      *f_ent;
    _field_action_t     *fa = NULL;
    int                 retval;
    uint32          temp;
    uint32          explicit_dest;

    FP_VVERB(
      ("FP: bcm_field_action_add(unit=%d, entry=%d, action=%s, p0=%d, p1=%d)\n",
       unit, entry, _robo_field_action_name(action), param0, param1));
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Check for existing actions that conflict with the new action. */    
    fa = f_ent->actions;
    explicit_dest = 0;
    while(fa != NULL) {
        if (_field_actions_conflict(action, fa->action)) {
            if (!_field_actions_conflict_allow(unit, action, param0, param1, fa)) {
                FP_ERR((
                 "FP Error: action=%d conflicts with existing action in entry=%d\n",
                    action, entry ));
                return BCM_E_CONFIG;
            } else {
                /* 
                 * Set the flag to represent rediret to both CPU and mirror-to port
                 * is configurable.
                 */
                explicit_dest = 1;
            }
        }
        fa = fa->next;
    }

    /* Confirm that action is supported by device. */
    if (_field_action_not_supported(unit, action)) {
        FP_ERR(("FP Error: action=%d not supported on unit=%d\n", action,
                unit));
        return BCM_E_UNAVAIL;
    }

    /* For Redirect-to-port action, range check the port value.  */
    if ((action == bcmFieldActionRedirect || 
         action == bcmFieldActionRpRedirectPort ||
         action == bcmFieldActionGpRedirectPort ||
         action == bcmFieldActionMirrorIngress || 
         action == bcmFieldActionRpMirrorIngress ||
         action == bcmFieldActionGpMirrorIngress) &&
        !SOC_PORT_VALID_RANGE(unit, (bcm_port_t)param1 & ~(1 << 6))) {
        FP_ERR(("FP ERROR: param1=%d out of range for Redirect.\n", param1));
        /*
         * Special parameter value to represent actions rediret or mirror-to
         * of BCM53242. Although it's not a valid port number.
         */
        if (!(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) 
            || !((param1 == FP_ACT_CHANGE_FWD_ALL_PORTS) ||
                  (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT))) {
            return BCM_E_PARAM;
        }
    }

    /* 
     * For meter configuration actions, confirm meter exists and set
     * its mode value to param0. Param1 is not used. Don't create an action
     * struct.
     */
    /* Check for this support by bcm5348 or not */
    if (action == bcmFieldActionMeterConfig) {
        if (f_ent->meter == NULL) {
            FP_ERR(("FP Error: No meter in entry=%d\n", entry));
            return BCM_E_RESOURCE;
        }
        /* mode = 4 & 5 are undefined */
        if (param0 == 4 || param0 == 5 || param0 > 7) {
            FP_ERR(("FP Error: invalid meter mode=%d\n", param0));
            return BCM_E_PARAM;
        }

        /* Unsupport meter mode for robo chips */
        if (param0 == 2 || param0 == 3 || param0 == 6 || param0 == 7) {
            FP_ERR(("FP Error: unsupport meter mode=%d\n", param0));
            return BCM_E_UNAVAIL;
        }
        f_ent->mode_mtr        = param0;
        /* f_ent->meter->inst_flg = 0; */
        f_ent->dirty        = _FIELD_DIRTY;
        return BCM_E_NONE;
    }

    if (action == bcmFieldActionUpdateCounter) {
        if (f_ent->meter == NULL) {
            if (param0 == BCM_FIELD_COUNTER_MODE_NO_YES) {
                /* 
                 * The bcm53242/bcm53262 count the packets in 
                 * Inband counter by default.
                 */
                if ((!SOC_IS_ROBO53242(unit)) && (!SOC_IS_ROBO53262(unit))) {
                    return BCM_E_UNAVAIL;
                }
            } else if (param0 == BCM_FIELD_COUNTER_MODE_YES_NO) {
                /* 
                 * The bcm5395 bcm5348/5347 bcm53115 counter packets in 
                 * Outband counter by default.
                 */
                if ((!SOC_IS_ROBO5348(unit)) && (!SOC_IS_ROBO5347(unit)) &&
                    (!SOC_IS_ROBO5395(unit)) && (!SOC_IS_ROBO53115(unit))) {
                    return BCM_E_UNAVAIL;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
        } else {
            if (param0 != BCM_FIELD_COUNTER_MODE_RED_NOTRED) {
                return BCM_E_UNAVAIL;
            }
        }
    }

    /* NewClassID action can't be add in Slice 0 for BCM53115 */
    if ((action == bcmFieldActionNewClassId) &&
        (SOC_IS_ROBO53115(unit))) {
        _field_group_t      *fg;
        
        fg = _robo_field_group_find(unit, f_ent->gid);
        if (fg == NULL) {
            FP_ERR(("FP Error: group=%d not found in unit=%d.\n", f_ent->gid, unit));
            return BCM_E_NOT_FOUND;
        }
        /* 
         * The value of fpf is the combination of L3 framing and
         * slice id value for BCM53115 */
        if ((fg->slices->sel_codes.fpf & 0x3) == 0) {
            FP_ERR(
            ("FP Error: bcmFieldActionNewCalssId can't add to this entry\n"));
            return BCM_E_UNAVAIL;
        }
    }

    switch (action) {
        case bcmFieldActionPrioIntNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_MOD_INT_PRI, 
                &f_ent->drv_entry, param0, 0);
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_MOD_INT_PRI, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionPrioPktNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_PCP_NEW, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionCopyToCpu:
            /* should use symbol */
            temp = CMIC_PORT(unit);           
             /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, 0x1 << temp, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, 0x1 << temp, 0);
            } else if (SOC_IS_ROBO53242(unit) ||
                       SOC_IS_ROBO53262(unit)) {
                /* Check if copy to mirror-to port is configured */
                fa = f_ent->actions;
                explicit_dest = 0;
                while(fa != NULL) {
                    if ((fa->action == bcmFieldActionMirrorIngress) &&
                        (fa->param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT)) {
                        /* 
                         * Copy to mirror-to port is already configured. 
                         * Flag to represent copy to both cpu and mirror-to port 
                         * is acceptable .
                         */
                        explicit_dest = 1;
                        break;
                    }
                    fa = fa->next;
                }
                
                if (explicit_dest) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, temp, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, temp, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, temp, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, temp, 0);
            }
            break;
        case bcmFieldActionMirrorIngress:           
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit) ||
                       SOC_IS_ROBO53262(unit)) {
                /* Check if copy to mirror-to port is configured */
                explicit_dest = 0;
                if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    fa = f_ent->actions;
                    while(fa != NULL) {
                        if (fa->action == bcmFieldActionCopyToCpu) {
                            /* 
                             * Copy to cpu is already configured. 
                             * Flag to represent copy to both cpu and mirror-to port 
                             * is acceptable .
                             */
                            explicit_dest = 1;
                            break;
                        }
                        fa = fa->next;
                    }
                }
                
                if (explicit_dest) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, param1, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, param1, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionRedirect:
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_REDIRECT, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_REDIRECT, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit)||
                       SOC_IS_ROBO53262(unit)) {
                if (explicit_dest) {
                    /* Redirect to both cpu and mirror-to port */
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) { 
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, param1, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_REDIRECT, 
                    &f_ent->drv_entry, param1, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_REDIRECT, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionRedirectPbmp:
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_REDIRECT, 
                    &f_ent->drv_entry, param0, 0);
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_REDIRECT, 
                    &f_ent->drv_entry, param0, 0);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldActionDrop:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_DROP, 
                &f_ent->drv_entry, 0, 0);
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_DROP, 
                &f_ent->drv_entry, 0, 0);
            break;
        case bcmFieldActionDscpNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_DSCP_NEW, 
                &f_ent->drv_entry, param0, 0);
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_DSCP_NEW, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionCosQNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_COSQ_NEW, 
                &f_ent->drv_entry, param0, 0);
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_COSQ_NEW, 
                &f_ent->drv_entry, param0, 0);
                                            
            break;
        case bcmFieldActionCosQCpuNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_COSQ_CPU_NEW, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionRpCopyToCpu:
            temp = CMIC_PORT(unit);
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, 0x1 << temp, 0);
            } else if (SOC_IS_ROBO53242(unit) ||
                       SOC_IS_ROBO53262(unit)) {
                /* Check if copy to mirror-to port is configured */
                fa = f_ent->actions;
                explicit_dest = 0;
                while(fa != NULL) {
                    if ((fa->action == bcmFieldActionRpMirrorIngress) &&
                        (fa->param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT)) {
                        /* 
                         * Copy to mirror-to port is already configured. 
                         * Flag to represent copy to both cpu and mirror-to port 
                         * is acceptable .
                         */
                        explicit_dest = 1;
                        break;
                    }
                    fa = fa->next;
                }
                
                if (explicit_dest) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, temp, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, temp, 0);
            }            
            break;
        case bcmFieldActionRpDrop:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_DROP, 
                &f_ent->drv_entry, 0, 0);
            break;
        case bcmFieldActionRpDscpNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_DSCP_NEW, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionGpCopyToCpu:
            temp = CMIC_PORT(unit);
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, 0x1 << temp, 0);
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, temp, 0);
            }            
            break;
        case bcmFieldActionGpDrop:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_DROP, 
                &f_ent->drv_entry, 0, 0);
            break;
        case bcmFieldActionGpDscpNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_DSCP_NEW, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionRpPrioIntNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_OB_MOD_INT_PRI, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionGpPrioIntNew:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_IB_MOD_INT_PRI, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionInnerVlanNew:
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                /*
                 * param0 carries the value of new vid.
                 * param1 carries the desired port bitmap.
                 */
                if (param1 == 0) {
                    temp = SOC_PBMP_WORD_GET(PBMP_E_ALL(unit), 0);
                } else {
                    temp = param1;
                }
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_CHANGE_CVID, 
                    &f_ent->drv_entry, param0, temp));
            } else {
                /*
                 * param0 carries the value of new vid.
                 */
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_CHANGE_CVID, 
                    &f_ent->drv_entry, param0, 0));
            }
            break;
        case bcmFieldActionOuterVlanNew:
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                /*
                 * param0 carries the value of new vid.
                 * param1 carries the desired port bitmap.
                 */
                if (param1 == 0) {
                    temp = SOC_PBMP_WORD_GET(PBMP_E_ALL(unit), 0);
                } else {
                    temp = param1;
                }
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_CHANGE_SPVID, 
                    &f_ent->drv_entry, param0, temp));
            } else {
                /*
                 * param0 carries the value of new vid.
                 */
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_CHANGE_SPVID, 
                    &f_ent->drv_entry, param0, 0));
            }
            break;
        case bcmFieldActionNewTc:
             (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_CHANGE_TC, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionNewClassId:
             (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_CLASSFICATION_ID, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionLoopback:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_LOOPBACK, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionNewReasonCode:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_REASON_CODE, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionBypassStp:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_STP_BYPASS, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionBypassEap:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_EAP_BYPASS, 
                &f_ent->drv_entry, param0, 0);
            break;
        case bcmFieldActionBypassVlan:
            (DRV_SERVICES(unit)->cfp_action_set)
                (unit, DRV_CFP_ACT_VLAN_BYPASS, 
                &f_ent->drv_entry, param0, 0);
            break;    
        case bcmFieldActionRpRedirectPort:
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_REDIRECT, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit)||
                       SOC_IS_ROBO53262(unit)) {
                if (explicit_dest) {
                    /* Redirect to both cpu and mirror-to port */
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) { 
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_REDIRECT, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_REDIRECT, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionRpMirrorIngress:           
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit) ||
                       SOC_IS_ROBO53262(unit)) {
                /* Check if copy to mirror-to port is configured */
                explicit_dest = 0;
                if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    fa = f_ent->actions;
                    while(fa != NULL) {
                        /* 
                         * Copy to cpu is already configured. 
                         * Flag to represent copy to both cpu and mirror-to port 
                         * is acceptable .
                         */
                        if (fa->action == bcmFieldActionRpCopyToCpu) {
                            explicit_dest = 1;
                            break;
                        }
                        fa = fa->next;
                    }
                }
                
                if (explicit_dest) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_APPEND, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_APPEND, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionGpRedirectPort:
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_REDIRECT, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit)||
                       SOC_IS_ROBO53262(unit)) {
                if (explicit_dest) {
                    /* Redirect to both cpu and mirror-to port */
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) { 
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_REDIRECT, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_REDIRECT, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionGpMirrorIngress:           
            /*
             * If the port field is bitmap format,
             * translate the port number to bitmap format
             */
            if (SOC_IS_ROBO5395(unit) ||
                SOC_IS_ROBO53115(unit)) {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, 0x1 << param1, 0);
            } else if (SOC_IS_ROBO53242(unit) ||
                       SOC_IS_ROBO53262(unit)) {
                /* Check if copy to mirror-to port is configured */
                explicit_dest = 0;
                if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    fa = f_ent->actions;
                    while(fa != NULL) {
                        /* 
                         * Copy to cpu is already configured. 
                         * Flag to represent copy to both cpu and mirror-to port 
                         * is acceptable .
                         */
                        if (fa->action == bcmFieldActionGpCopyToCpu) {
                            explicit_dest = 1;
                            break;
                        }
                        fa = fa->next;
                    }
                }
                
                if (explicit_dest) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_CPU;
                    temp |= DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else if (param1 == FP_ACT_CHANGE_FWD_MIRROT_TO_PORT) {
                    temp = DRV_FIELD_ACT_CHANGE_FWD_MIRROR_CAPTURE;
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, 0, temp);
                } else {
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_APPEND, 
                        &f_ent->drv_entry, param1, 0);
                }
            } else {
                (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_APPEND, 
                    &f_ent->drv_entry, param1, 0);
            }
            break;
        case bcmFieldActionUpdateCounter:
            /* Do nothing with action ram */
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    /*
     * It's okay to allocate the action and add it to entry's linked-list.
     */
    retval = _robo_field_action_alloc(action, param0, param1, &fa);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: failure in _robo_field_action_alloc()\n"));
        return retval;
    }

    assert(fa != NULL);
    fa->inst_flg = 0; /* mark new action as not yet installed */

    /* Add action to front of entry's linked-list. */
    fa->next = f_ent->actions;
    f_ent->actions  = fa;
    f_ent->dirty = _FIELD_DIRTY;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_action_get
 *
 * Purpose:
 *     Get parameters associated with an entry action
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry ID
 *     action - Action to perform (bcmFieldActionXXX)
 *     param0 - (OUT) Action parameter
 *     param1 - (OUT) Action parameter
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - paramX is NULL
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_action_get(int unit,
             bcm_field_entry_t entry,
             bcm_field_action_t action,
             uint32 *param0,
             uint32 *param1)
{
    _field_entry_t      *f_ent;
    _field_action_t     *fa;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Find matching action in the entry */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        if (fa->action == action) {
            break;
        }
    }

    if (fa == NULL) {
        FP_ERR(("FP Error: action not in entry=%d\n", entry));
        return BCM_E_NOT_FOUND;
    }

    if (param0 == NULL || param1 == NULL) {
        FP_ERR(("FP Error: param0 == NULL || param1 == NULL\n"));
        return BCM_E_PARAM;
    }

    *param0 = fa->param0;
    *param1 = fa->param1;

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_action_remove
 *
 * Purpose:
 *     Remove an action performed when entry rule is matched for a packet.
 *
 * Parameters:
 *     unit   - BCM device number
 *     entry  - Entry ID
 *     action - Action to remove (bcmFieldActionXXX)
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - Action out of valid range.
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_action_remove(int unit,
                bcm_field_entry_t entry,
                bcm_field_action_t action)
{
    _field_entry_t      *f_ent;
    _field_action_t     *fa;
    _field_action_t     *fa_prev = NULL;

    FP_VVERB(("FP: bcm_field_action_remove(unit=%d, entry=%d, action=%s)\n",
               unit, entry, _robo_field_action_name(action)));
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    if (action < 0 || bcmFieldActionCount <= action) {
        FP_ERR(("FP Error: unknown action=%d\n"));
        return BCM_E_PARAM;
    }


    /* Find the action in the entry */
    fa = f_ent->actions; /* start at head */
    while (fa != NULL) {
        if (fa->action == action) { /* found match, destroy action */
            switch (action) {
                case bcmFieldActionPrioIntNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_MOD_INT_PRI_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_MOD_INT_PRI_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionRpPrioIntNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_MOD_INT_PRI_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionGpPrioIntNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_MOD_INT_PRI_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionPrioPktNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_PCP_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionCopyToCpu:
                case bcmFieldActionDrop:
                case bcmFieldActionRedirectPbmp:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionRedirect:
                case bcmFieldActionMirrorIngress:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionRpMirrorIngress:
                case bcmFieldActionRpRedirectPort:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionGpMirrorIngress:
                case bcmFieldActionGpRedirectPort:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionDscpNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_DSCP_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_DSCP_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionCosQNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_COSQ_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_COSQ_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionCosQCpuNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_COSQ_CPU_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionRpDrop:
                case bcmFieldActionRpCopyToCpu:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_OB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionGpDrop:
                case bcmFieldActionGpCopyToCpu:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_IB_NONE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionInnerVlanNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_CHANGE_CVID_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionOuterVlanNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_CHANGE_SPVID_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionNewClassId:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_CLASSFICATION_ID, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionLoopback:    
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_LOOPBACK,
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionNewReasonCode:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_REASON_CODE, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionBypassStp:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_STP_BYPASS, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionBypassEap:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_EAP_BYPASS, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionBypassVlan:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_VLAN_BYPASS, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionNewTc:
                    (DRV_SERVICES(unit)->cfp_action_set)
                        (unit, DRV_CFP_ACT_CHANGE_TC_CANCEL, 
                        &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionRpDscpNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_OB_DSCP_CANCEL, 
                    &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionGpDscpNew:
                    (DRV_SERVICES(unit)->cfp_action_set)
                    (unit, DRV_CFP_ACT_IB_DSCP_CANCEL, 
                    &f_ent->drv_entry, 0, 0);
                    break;
                case bcmFieldActionUpdateCounter:
                    /* Do nothing with action ram */
                    break;
                default:
                    return BCM_E_UNAVAIL;
            }
            if (fa_prev != NULL) {
                fa_prev->next = fa->next;
            } else { /* matched head of list */
                f_ent->actions = fa->next;
            }
            /* okay to free entry */
            sal_free(fa);
            f_ent->dirty = _FIELD_DIRTY;
            return BCM_E_NONE;
        }
        fa_prev = fa;
        fa      = fa->next;
    }

    FP_ERR(("FP Error: no matching action=%d found\n"));
    return BCM_E_NOT_FOUND; /* no matching action found */
}

/*
 * Function: bcm_robo_field_action_delete
 *
 * Purpose:
 *     Delete an action performed when entry rule is matched for a packet.
 *
 * Parameters:
 *     unit   - BCM device number
 *     entry  - Entry ID
 *     action - Action to remove (bcmFieldActionXXX)
 *     param0 - Action parameter (use 0 if not required)
 *     param1 - Action parameter (use 0 if not required)
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - Action out of valid range.
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_action_delete(int unit,
                bcm_field_entry_t entry,
                bcm_field_action_t action, 
                uint32 param0, uint32 param1)
{
   return bcm_robo_field_action_remove(unit, entry, action);
}

/*
 * Function: bcm_field_action_remove_all
 *
 * Purpose:
 *     Remove all actions from an entry rule.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_action_remove_all(int unit,
                                bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_action_t     *fa;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* start at the head of the actions list and burn them up */
    while (f_ent->actions != NULL) {
        fa = f_ent->actions;
        f_ent->actions = fa->next;
        sal_free(fa);
        f_ent->dirty = _FIELD_DIRTY;
    }
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_MOD_INT_PRI_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_MOD_INT_PRI_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_NONE, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_NONE, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_DSCP_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_DSCP_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_COSQ_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_COSQ_CANCEL,
        &f_ent->drv_entry, 0, 0);                    
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_COSQ_CPU_CANCEL, 
        &f_ent->drv_entry, 0, 0);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_field_action_mac_add
 * Purpose:
 *      Add an action to a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry Id.
 *      action - (IN) Field action id.
 *      mac - (IN) Field action parameter.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_action_mac_add(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_action_mac_get
 * Purpose:
 *      Retrieve the parameters for an action previously added to a
 *      field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry Id.
 *      action - (IN) Field action id.
 *      mac - (IN) Field action argument.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_action_mac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_mac_t *mac)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_group_traverse
 * Purpose:
 *      Traverse all the fp groups in the system, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      callback - (IN) A pointer to the callback function to call for each fp group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_robo_field_group_traverse(int unit, bcm_field_group_traverse_cb callback,
                              void *user_data)
{
    return BCM_E_UNAVAIL; 
}


int
bcm_robo_field_action_ports_add(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t pbmp)
{
    _field_entry_t *f_ent;
    _field_action_t *fa;

    
    if (action != bcmFieldActionRedirectPbmp) {
        FP_ERR(("Incorrect action parameter\n"));
        return (BCM_E_UNAVAIL);
    }

    if (!(SOC_IS_ROBO5395(unit) ||
        SOC_IS_ROBO53115(unit))) {
        return BCM_E_UNAVAIL;
    }

    /* Action is always in PRIMARY_SLICE  */
    FIELD_IS_INIT(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Check for existing actions that conflict with the new action. */ 
    fa = f_ent->actions;
    while (fa != NULL) {
        if (_field_actions_conflict(action, fa->action)) {
            FP_ERR(("FP Error: action=%d conflicts with existing"
                    "action in entry=%d\n", action, entry));
            return BCM_E_CONFIG;
        }
        fa = fa->next;
    }

    /* Create a new action entry */
    fa = sal_alloc(sizeof (_field_action_t), "field_action");
    if (fa == NULL) {
        FP_ERR(("FP Error: allocation failure for field_action\n"));
        return (BCM_E_MEMORY);
    }
    sal_memset(fa, 0, sizeof (_field_action_t));

    fa->action = action;
    fa->param0 = SOC_PBMP_WORD_GET(pbmp, 0);
    fa->param1 = 0;

    if (f_ent->meter == NULL) {
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_OB_REDIRECT, 
        &f_ent->drv_entry, fa->param0, 0);
    }
    (DRV_SERVICES(unit)->cfp_action_set)
        (unit, DRV_CFP_ACT_IB_REDIRECT, 
        &f_ent->drv_entry, fa->param0, 0);

    /* Add action to front of entry's linked-list. */
    fa->next = f_ent->actions;
    f_ent->actions  = fa;
    f_ent->dirty    = _FIELD_DIRTY;

    return (BCM_E_NONE);
}

int
bcm_robo_field_action_ports_get(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t *pbmp)
{
    _field_entry_t *f_ent;
    _field_action_t *fa;

    if (action != bcmFieldActionRedirectPbmp) {
        FP_ERR(("Incorrect action parameter\n"));
        return (BCM_E_UNAVAIL);
    }

    FIELD_IS_INIT(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Find matching action in the entry */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        if (fa->action == action) {
            break;
        }
    }

    if (fa == NULL) {
        FP_ERR(("FP Error: action not in entry=%d\n", entry));
        return (BCM_E_NOT_FOUND);
    }

    if (pbmp == NULL) {
        FP_ERR(("FP Error: pbmp == NULL\n"));
        return (BCM_E_PARAM);
    }
    
    BCM_PBMP_CLEAR(*pbmp);

    SOC_PBMP_WORD_SET(*pbmp, 0, fa->param0);
    return (BCM_E_NONE);
}

/* Section: Field Statistics */

/*
 * Function: 
 *    bcm_robo_field_stat_create
 *
 * Description:
 *       Create statistics collection entity.
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      group    - (IN) Field group id. 
 *      nstat    - (IN) Number of elements in stat array.
 *      stat_arr - (IN) Collected statistics descriptors array.
 *      stat_id  - (OUT) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_stat_create(int unit, bcm_field_group_t group, int nstat, 
                          bcm_field_stat_t *stat_arr, int *stat_id) 
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *    bcm_robo_field_stat_destroy
 *
 * Description:
 *       Destroy statistics collection entity.
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      stat_id  - (IN) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_stat_destroy(int unit, int stat_id)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *     bcm_robo_field_stat_config_get
 *
 * Description:
 *      Get number of different statistics associated with statistics
 *      collection entity.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      stat_id   - (IN) Statistics entity id.
 *      stat_size - (OUT) Number of collercted statistics
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_stat_size(int unit, int stat_id, int *stat_size)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *     bcm_robo_field_stat_config_get
 *
 * Description:
 *      Get enabled statistics for specific collection entity.
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      stat_id  - (IN) Statistics entity id.
 *      nstat    - (IN) Number of elements in stat array.
 *      stat_arr - (OUT) Collected statistics descriptors array.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_stat_config_get(int unit, int stat_id, int nstat, 
                              bcm_field_stat_t *stat_arr)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_stat_get
 *
 * Description:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      stat_id   - (IN) Statistics entity id.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_stat_get(int unit, int stat_id, bcm_field_stat_t stat, 
                       uint64 *value)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_stat_get32
 *
 * Description:
 *      Get lower 32 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      stat_id   - (IN) Statistics entity id.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_robo_field_stat_get32(int unit, int stat_id, 
                             bcm_field_stat_t stat, uint32 *value)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_stat_multi_get
 *
 * Description:
 *      Get 64 bit counter values for multiple statistic types.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      stat_id   - (IN) Statistics entity id.
 *      nstat     - (IN) Number of elements in stat array.
 *      stat_arr  - (IN) Collected statistics descriptors array.
 *      value_arr - (OUT) Collected counters values.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_robo_field_stat_multi_get(int unit, int stat_id, int nstat, 
                                 bcm_field_stat_t *stat_arr, uint64 *value_arr)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_stat_multi_get32
 *
 * Description:
 *      Get lower 32 bit counter values for multiple statistic types.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      stat_id   - (IN) Statistics entity id.
 *      nstat     - (IN) Number of elements in stat array.
 *      stat_arr  - (IN) Collected statistics descriptors array.
 *      value_arr - (OUT) Collected counters values.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_robo_field_stat_multi_get32(int unit, int stat_id, int nstat, 
                                   bcm_field_stat_t *stat_arr, 
                                   uint32 *value_arr)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_entry_stat_attach
 *
 * Description:
 *       Attach statistics entity to Field Processor entry.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      entry     - (IN) Field entry id. 
 *      stat_id   - (IN) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_robo_field_entry_stat_attach(int unit, bcm_field_entry_t entry, 
                                    int stat_id)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_entry_stat_detach
 *
 * Description:
 *       Detach statistics entity to Field Processor entry.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      entry     - (IN) Field entry id. 
 *      stat_id   - (IN) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_robo_field_entry_stat_detach(int unit, bcm_field_entry_t entry,
                                    int stat_id)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: 
 *      bcm_robo_field_entry_stat_get
 *
 * Description:
 *      Get statistics entity attached to Field Processor entry.  
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      entry     - (IN) Field entry id. 
 *      stat_id   - (IN) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_field_entry_stat_get(int unit, bcm_field_entry_t entry, int *stat_id)
{
    return (BCM_E_UNAVAIL);
}


/*
 * Function: bcm_robo_field_counter_create
 *
 * Description:
 *      Create a field counter pair within an entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry not found in unit
 *      BCM_E_EXISTS    - Entry already has counter.
 *      BCM_E_RESOURCE  - No counters left.
 *      BCM_E_MEMORY    - Counter allocation error
 *      BCM_E_NONE      - Success
 */
int 
bcm_robo_field_counter_create(int unit,
                 bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_counter_t    *f_cnt = NULL;
    int             rv = BCM_E_NONE;
    uint32          temp;
    uint64          val;
    _field_group_t      *fg;

    
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    FP_VVERB(("BEGIN bcm_robo_field_counter_create(unit=%d, entry=%d\n", unit,
              entry));

    fg = _robo_field_group_find(unit, f_ent->gid);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found in unit=%d.\n", f_ent->gid, unit));
        return BCM_E_NOT_FOUND;
    }

    if (f_ent->counter != NULL) {
        FP_ERR(("FP Error: Counter already exists in entry=%d.\n", entry));
        return BCM_E_EXISTS;
    }

    /* Allocate a counter struct and add it to entry. */
    f_cnt = sal_alloc(sizeof (_field_counter_t), "field_counter");
    if (f_cnt == NULL) {
        FP_ERR(("FP Error: allocation failure for field_counter\n"));
        return BCM_E_MEMORY;
    }

    f_cnt->index    = f_ent->slice_idx;
    f_cnt->entries  = 1;                          /* 1 ref. to this counter */
    f_ent->counter  = f_cnt;
    f_ent->mode_ctr = BCM_FIELD_COUNTER_MODE_DEFAULT;
    f_ent->dirty = _FIELD_DIRTY;

    /* Reset the counter value */
    temp = 0;
    COMPILER_64_SET(val, 0, temp);
    rv = bcm_field_counter_set(unit, entry, 0, val);
    if (rv < 0) {
        FP_ERR(("FP Error: Fail to reset the counter 0 value in entry=%d.\n", entry));
        return BCM_E_INTERNAL;
    }
    rv = bcm_field_counter_set(unit, entry, 1, val);
    if (rv < 0) {
        FP_ERR(("FP Error: Fail to reset the counter 1 value in entry=%d.\n", entry));
        return BCM_E_INTERNAL;
    }

    fg->group_status.counters_free--;

    return rv;
}

/*
 * Function: bcm_robo_field_counter_destroy
 *
 * Description:
 *     Remove a counter pair from an entry. If no other entries reference
 *     the counter, it is deallocated.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry not found.
 *     BCM_E_EMPTY     - No source counter exists.
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     Counter pair becomes available for reallocation if it
 *     is not associated with any other entry rule.
 */
int 
bcm_robo_field_counter_destroy(int unit,
                  bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_group_t      *fg;

    FP_VVERB(("FP: bcm_field_counter_destroy(unit=%d, entry=%d)\n", unit,
             entry));
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    fg = _robo_field_group_find(unit, f_ent->gid);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found in unit=%d.\n", f_ent->gid, unit));
        return BCM_E_NOT_FOUND;
    }

    if (f_ent->counter == NULL) {
        FP_ERR(("FP Error: no counter in entry=%d.\n", entry));
        return BCM_E_EMPTY;
    }

    f_ent->counter->entries--;

    sal_free(f_ent->counter);
    f_ent->counter  = NULL;

    fg->group_status.counters_free++;
    
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_counter_get
 *
 * Description:
 *      Get a counter value.
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *      counter_num - Choice of out-profile (counter_num=1) or in-profile (counter_num=0)
 *                    of the counter pair.
 *      valp - (OUT) Pointer to counter value
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry ID not found in unit.
 *      BCM_E_EMPTY     - No counter created for entry.
 *      BCM_E_PARAM     - counter_num does not match counter index.
 *      BCM_E_PARAM     - valp pointing to NULL.
 *      BCM_E_XXX       - From memory read operations.
 *      BCM_E_UNAVAIL   - Feature unavailable on unit.
 *      BCM_E_NONE      - Success
 */
int
bcm_robo_field_counter_get(int unit,
              bcm_field_entry_t entry,
              int counter_num,
              uint64 *valp)
{
    int     rv = BCM_E_NONE;
    _field_entry_t      *f_ent;
    _field_counter_t    *f_cnt = NULL;
    
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);
    FP_VVERB(("bcm_robo_field_counter_get(unit=%d, entry=%d\n", unit,
              entry));
    
    if (counter_num < 0 || 1 < counter_num) {
        FP_ERR(("FP Error: counter_num != 0 || counter_num != 1\n"));
        return (BCM_E_PARAM);
    }
    if (f_ent->counter == NULL) {
        FP_ERR(("FP Error: Counter didn't exists in entry=%d.\n", entry));
        return BCM_E_EMPTY;
    }
    f_cnt = f_ent->counter;

    rv = _robo_field_sw_counter_get(unit, f_cnt, counter_num, valp);
    
    return rv;
}

int 
bcm_robo_field_counter_get32(int unit,
                bcm_field_entry_t entry,
                int counter_num,
                uint32 *valp)
{
    uint64 count_64;
    int rv;
    rv = bcm_field_counter_get(unit, entry, counter_num, &count_64);
    if (BCM_SUCCESS(rv)) {
        *valp = COMPILER_64_LO(count_64);
    } 
    return rv;
}

/*
 * Function: bcm_robo_field_counter_set
 *
 * Description:
 *      Set a counter value.
 *
 * Parameters:
 *      unit        - BCM device number
 *      entry       - Entry ID
 *      counter_num - Choice of out-profile (counter_num=1) or in-profile (counter_num=0)
 *                    of the counter pair.
 *      val - Packet/byte counter value
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry ID not found in unit.
 *      BCM_E_EMPTY     - No counter created for entry.
 *      BCM_E_PARAM     - counter_num does not match counter index.
 *      BCM_E_XXX       - From memory write operations.
 *      BCM_E_UNAVAIL   - Feature unavailable on unit.
 *      BCM_E_NONE      - Success
 */
int 
bcm_robo_field_counter_set(int unit,
              bcm_field_entry_t entry,
              int counter_num,
              uint64 val)
{
    int     rv = BCM_E_NONE;
    _field_entry_t      *f_ent;
    _field_counter_t    *f_cnt = NULL;
    uint32  temp;
    
    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    FP_VVERB(("bcm_robo_field_counter_set(unit=%d, entry=%d\n", unit,
              entry));

    if (f_ent->counter == NULL) {
        FP_ERR(("FP Error: Counter didn't exists in entry=%d.\n", entry));
        return BCM_E_EMPTY;
    }
    f_cnt = f_ent->counter;

    if (counter_num < 0 || 1 < counter_num) {
        FP_ERR(("FP Error: counter_num != 0 || counter_num != 1\n"));
        return (BCM_E_PARAM);
    }

    /* Preserve total value and calculate hw counter value. */
    rv = _robo_field_sw_counter_set(unit, f_cnt, counter_num, val);

    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    
    temp = COMPILER_64_LO(val);
    if (counter_num == 0) {
        rv = (DRV_SERVICES(unit)->cfp_stat_set)
            (unit, DRV_CFP_STAT_INBAND, f_cnt->index, temp);
    } else {
        rv = (DRV_SERVICES(unit)->cfp_stat_set)
            (unit, DRV_CFP_STAT_OUTBAND, f_cnt->index, temp);
    }
    
    return rv;
}

/*
 * Function: bcm_robo_field_counter_set32
 *
 * Description:
 *      Set a 32-bit counter value.
 *
 * Parameters:
 *      unit        - BCM device number
 *      entry       - Entry ID
 *      counter_num - Choice of upper (counter_num=1) or lower (counter_num=0)
 *                    of the counter pair.
 *      count       - Packet/byte counter value to set
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry ID not found in unit.
 *      BCM_E_EMPTY     - No counter created for entry.
 *      BCM_E_PARAM     - counter_num does not match counter index.
 *      BCM_E_XXX       - From memory write operations.
 *      BCM_E_UNAVAIL   - Feature unavailable on unit.
 *      BCM_E_NONE      - Success
 */
int
bcm_robo_field_counter_set32(int unit, bcm_field_entry_t entry, int counter_num,
                            uint32 count)
{
    uint64 count_64;

    COMPILER_64_SET(count_64, 0, count);

    return bcm_field_counter_set(unit, entry, counter_num, count_64);
}

/*
 * Function: bcm_robo_field_counter_share
 *
 * Description:
 *      Share the counter pair from one entry with another entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      src_entry - Entry ID that has counter pair
 *      dst_entry - Entry ID that will get counter pair
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized
 *      BCM_E_NOT_FOUND - Either source or destination entry not found
 *      BCM_E_RESOURCE  - Source and destination are not in the same group.
 *      BCM_E_EXISTS    - Destination entry already has counter.
 *      BCM_E_EMPTY     - No source counter exists.
 *      BCM_E_NONE      - Success
 *
 * Notes:
 *     The src_entry and dst_entry should belong to the same field group
 */
int 
bcm_robo_field_counter_share(int unit,
                bcm_field_entry_t src_entry,
                bcm_field_entry_t dst_entry)
{
    return BCM_E_UNAVAIL;
}



/*
 * Function: bcm_robo_field_detach
 *
 * Purpose:
 *     Free resources associated with field module
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_INIT - BCM Unit not initialized.
 *     BCM_E_XXX  - Error code from bcm_field_group_destroy() or
 *                  bcm_field_entry_destroy_all().
 *     BCM_E_NONE - Success
 */
int 
bcm_robo_field_detach(int unit)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    bcm_port_t  port;

    FIELD_IS_INIT(unit);
    FP_VVERB(("FP: bcm_field_detach(%d)\n", unit));

    fc = _field_control[unit];

    _robo_field_thread_stop(unit);

    /* Destroy all entries in unit. */
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy_all(unit));

    /* Destroy all groups in unit. */
    fg = fc->groups;
    while (fg != NULL) {
        BCM_IF_ERROR_RETURN
            (bcm_field_group_destroy(unit, fg->gid));
        fg = fc->groups;
    }

    /* Disable CFP */
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        (DRV_SERVICES(unit)->cfp_control_set)
            (unit, DRV_CFP_ENABLE, port, 0);
    }
    /* Clear HW TABLE */
    (DRV_SERVICES(unit)->cfp_control_set)(unit, DRV_CFP_TCAM_RESET, 0, 0);

    /* destroy all ranges */
    while (fc->ranges != NULL) {
        BCM_IF_ERROR_RETURN(bcm_field_range_destroy(unit, fc->ranges->rid));
    }

    /* Destroy a Unit's slices */
    sal_free(fc->slices);

    if (fc->fc_lock != NULL) {
        sal_mutex_destroy(fc->fc_lock);
    }

    sal_free(fc);

    _field_control[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_entry_copy
 *
 * Purpose:
 *     Create a copy of an existing entry. The new entry will be a member of
 *     the same group as the source entry.
 *
 * Parameters:
 *     unit      - BCM device number
 *     src_entry - Entry to copy
 *     dst_entry - (OUT) New entry
 *
 * Returns:
 *     BCM_E_INIT        BCM Unit not initialized
 *     BCM_E_NOT_FOUND   Source entry not found
 *     BCM_E_INTERNAL    No group exists for source entry ID.
 *     BCM_E_PARAM       dst_entry pointing to NULL
 *     BCM_E_RESOURCE    No destination entry available
 *     BCM_E_XXX         Error from bcm_field_entry_copy_id()
 */
int 
bcm_robo_field_entry_copy(int unit,
             bcm_field_entry_t src_entry,
             bcm_field_entry_t *dst_entry)
{
    _field_entry_t      *f_ent_src;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, src_entry, f_ent_src, _FP_SLICE_PRIMARY);

    if (dst_entry == NULL) {
        FP_ERR(("FP Error: dst_entry == NULL\n"));
        return BCM_E_PARAM;
    }

    /* Generate a destination Entry ID.  */
    *dst_entry = src_entry + 1;
    while (_robo_field_entry_find(unit, *dst_entry, _FP_SLICE_PRIMARY) != NULL) {
        *dst_entry += 1;
        assert(*dst_entry < _FP_ENTRY_ID_BASE + 0x1000000);
    }

    return bcm_field_entry_copy_id(unit, src_entry, *dst_entry);
}

/*
 * Function: bcm_robo_field_entry_copy_id
 *
 * Purpose:
 *     Create a copy of an existing entry with a requested ID
 *
 * Parameters:
 *     unit      - BCM device number
 *     src_entry - Source entry to copy
 *     dst_entry - Destination entry for copy
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Source Entry ID not found
 *     BCM_E_XXX       - Error code from bcm_field_entry_create_id()
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_entry_copy_id(int unit,
             bcm_field_entry_t src_entry,
             bcm_field_entry_t dst_entry)
{
    uint32              kbits_sec, kbits_burst;
    _field_entry_t      *f_ent_src, *f_ent_dst;
    _field_action_t     *fa_src;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, src_entry, f_ent_src, _FP_SLICE_PRIMARY);

    BCM_IF_ERROR_RETURN
        (bcm_field_entry_create_id(unit, f_ent_src->fs->group->gid, dst_entry));

    FIELD_ENTRY_GET(unit, dst_entry, f_ent_dst, _FP_SLICE_PRIMARY);

    /* sal_memcpy(&f_ent_dst->tcam, &f_ent_src->tcam, sizeof(_field_tcam_t)); */
    sal_memcpy(&f_ent_dst->drv_entry, &f_ent_src->drv_entry, 
        sizeof(drv_cfp_entry_t));

    /* Copy counter, if it exists. */
    if (f_ent_src->counter) {
        BCM_IF_ERROR_RETURN(
            bcm_field_counter_create(unit, dst_entry));
        f_ent_dst->mode_ctr = f_ent_src->mode_ctr;
    } else {
        f_ent_dst->counter = NULL;
    }

    /* Copy meter, if it exists. */
    if (f_ent_src->meter == NULL) {
        f_ent_dst->meter = NULL;
    } else {
        BCM_IF_ERROR_RETURN(bcm_field_meter_create(unit, dst_entry));

        BCM_IF_ERROR_RETURN(
            bcm_field_meter_get(unit, src_entry, BCM_FIELD_METER_COMMITTED,
                                &kbits_sec, &kbits_burst));
        BCM_IF_ERROR_RETURN(
            bcm_field_meter_set(unit, dst_entry, BCM_FIELD_METER_COMMITTED,
                                kbits_sec, kbits_burst));
        f_ent_dst->mode_mtr = f_ent_src->mode_mtr;
    }

    /* Copy source entry's action linked list.  */
    for (fa_src = f_ent_src->actions; fa_src != NULL; fa_src = fa_src->next) {
        BCM_IF_ERROR_RETURN(
            bcm_field_action_add(unit, dst_entry, fa_src->action, 
                                 fa_src->param0, fa_src->param1));
    }


    f_ent_dst->dirty = _FIELD_DIRTY;

    /* Set the destination entry's priority to the same as the source's. */
    return bcm_field_entry_prio_set(unit, dst_entry, f_ent_src->prio);
}

/*
 * Function: bcm_robo_field_entry_create
 *
 * Purpose:
 *     Create a blank entry based on a group. Automatically generate an entry
 *     ID.
 *
 * Parameters:
 *     unit  - BCM device number
 *     group - Field group ID
 *     entry - (OUT) New entry
 *
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 *     BCM_E_PARAM       *entry was NULL
 *     BCM_E_RESOURCE    No unused entries available
 *     BCM_E_XXX         Error from bcm_field_entry_create_id
 *
 * See Also:
 * bcm_field_entry_create_id
 */
int 
bcm_robo_field_entry_create(int unit,
               bcm_field_group_t group,
               bcm_field_entry_t *entry)
{
    _field_group_t      *fg;

    FIELD_IS_INIT(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found in unit=%d.\n", group, unit));
        return BCM_E_NOT_FOUND;
    }

    if (fg->group_status.entries_free == 0) {
        FP_ERR(("FP Error: No entries free in group.\n"));
        return BCM_E_RESOURCE;
    }
#ifdef BCM_FIELD_SHARED_ENTRIES
    if (_field_shared_entries_free == 0) {
        FP_ERR(("FP Error: No entries free in field.\n"));
        return BCM_E_RESOURCE;
    }
#endif    

    if (entry == NULL) {
        FP_ERR(("FP Error: entry == NULL.\n"));
        return BCM_E_PARAM;
    }

    /* Generate an entry ID.  */
    *entry = _FP_ENTRY_ID_BASE;
    while (_robo_field_entry_find(unit, *entry, _FP_SLICE_PRIMARY) != NULL) {
        *entry += 1;
        assert(*entry < _FP_ENTRY_ID_BASE + 0x1000000);
    }

    return bcm_field_entry_create_id(unit, group, *entry);
}

/*
 * Function: bcm_robo_field_entry_create_id
 *
 * Purpose:
 *     Create a blank entry group based on a group;
 *     allows selection of a specific slot in a slice
 *
 * Parameters:
 *     unit - BCM device number
 *     group - Field group ID
 *     entry - Requested entry ID; must be in the range prio_min through
 *             prio_max as returned by bcm_field_group_status_get().
 * Returns:
 *     BCM_E_INIT      - unit not initialized
 *     BCM_E_EXISTS    - Entry ID already in use
 *     BCM_E_NOT_FOUND - Group ID not found in unit
 *     BCM_E_MEMORY    - allocation failure
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_entry_create_id(int unit,
               bcm_field_group_t group,
               bcm_field_entry_t entry)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    _field_entry_t      *f_ent;
    uint32          temp;
    int     idx;

    FP_VVERB(("FP: bcm_robo_field_entry_create_id(group=%d, entry=%d)\n",
             group, entry));

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    /* Confirm that 'entry' is not already used on unit */
    if( _robo_field_entry_find(unit, entry, _FP_SLICE_PRIMARY) != NULL) {
        FP_ERR(("FP Error: entry=%d already exists.\n", entry));
        return BCM_E_EXISTS;
    }

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found on unit=%d.\n", group, unit));
        return BCM_E_NOT_FOUND;
    }

    if (fg->group_status.entries_free <= 0) {
        FP_ERR(("FP Error: No entries free in group.\n"));
        return BCM_E_RESOURCE;
    }
#ifdef BCM_FIELD_SHARED_ENTRIES
    if (_field_shared_entries_free == 0) {
        FP_ERR(("FP Error: No entries free in field.\n"));
        return BCM_E_RESOURCE;
    }
#endif        

    /* Create entry in primary slice. */
    f_ent = _robo_field_entry_phys_create(unit, entry,
                                     BCM_FIELD_ENTRY_PRIO_LOWEST,
                                     &fg->slices[0]);
    if (f_ent == NULL) {
        return BCM_E_MEMORY;
    }
    
#ifdef LVL7_FIXUP
    /* Find unused entry in slice. */
    for (idx=(fg->priority*16) ; idx < fc->tcam_sz; idx++) {
        if (NULL == _field_shared_entries[idx]) {
            _field_shared_entries[idx] = f_ent;
            f_ent->slice_idx = idx;
            fg->slices[0].entries[f_ent->slice_idx] = f_ent;
            break;
        }
    }
#else
    /* Find unused entry in slice. */
    for (idx = 0; idx < fc->tcam_sz; idx++) {
        if (NULL == _field_shared_entries[idx]) {
            _field_shared_entries[idx] = f_ent;
            f_ent->slice_idx = idx;
            fg->slices[0].entries[f_ent->slice_idx] = f_ent;
            break;
        }
    }
#endif
    
    f_ent->gid = group;
    /* Set the slice id to tcam */
    temp = fg->slices[0].sel_codes.fpf;
    BCM_IF_ERROR_RETURN(
        _robo_field_entry_slice_id_set(unit, f_ent, temp)); 

    /* Move entry to the default position. */
    if (fg->group_status.entries_free - 1 < fg->group_status.entries_total) {
        BCM_IF_ERROR_RETURN(
            bcm_field_entry_prio_set(unit, entry, BCM_FIELD_ENTRY_PRIO_DEFAULT));
    }

    fg->group_status.entries_free--;
#ifdef BCM_FIELD_SHARED_ENTRIES
    _field_shared_entries_free--;
#endif /* BCM_FIELD_SHARED_ENTRIES */    
    assert(0 <= fg->group_status.entries_free);

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
        BCM_IF_ERROR_RETURN(
            bcm_field_qualify_IpType(unit, entry, bcmFieldIpTypeIpv4Any));
    }
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
        BCM_IF_ERROR_RETURN(
            bcm_field_qualify_IpType(unit, entry, bcmFieldIpTypeIpv6));
    }



    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_group_create_mode
 *     
 * Purpose:
 *
 * Parameters:
 *     unit - BCM device number.
 *     port - Port number
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - (OUT) New field group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - No select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_group_create_mode(int unit, 
                    bcm_field_qset_t qset,
                    int pri,
                    bcm_field_group_mode_t mode,
                    bcm_field_group_t *group)
{
    int rv;
    FIELD_IS_INIT(unit);

    rv = _robo_field_group_id_generate(unit, qset, group, pri);
    if ( rv == BCM_E_EXISTS) {
        return BCM_E_NONE;
    }

    if (BCM_FAILURE(rv)) {
        FP_ERR(("FP Error: new group won't create.\n"));
        return rv;
    }

    return bcm_field_group_create_mode_id(unit, qset, pri, mode, *group);
}

/*
 * Function: bcm_robo_field_group_create_mode_id
 *     
 * Purpose:
 *
 * Parameters:
 *     unit - BCM device number.
 *     port - Port number
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_group_create_mode_id(int unit,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t group)
{
    _field_control_t    *fc = NULL;
    _field_group_t      *fg = NULL;
    int                 slice_indexes;
    int                 retval;
    int                 qual_stage_only;
    bcm_field_qualify_t qual;
    int     idx;
#ifdef DEBUG
    int                 empty_qset;
#endif /* DEBUG */

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);    

#ifdef DEBUG
    if (BCM_DEBUG_CHECK(BCM_DBG_FP | BCM_DBG_VERBOSE)) {
        soc_cm_print("bcm_field_group_create_mode_id(gid=%d, ", group);
        _field_qset_dump("qset=", qset, ")\n");
    }
    /* Warn if using empty Qset. */
    empty_qset = 1;
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if(BCM_FIELD_QSET_TEST(qset, qual)) {
            empty_qset = 0;
            break;
        }
    }

    if(empty_qset != 0) {
        FP_WARN(("FP Warning: Creating group with empty Qset\n"));
    }

#endif /* DEBUG */

    /* Return BCM_E_UNAVAIL if the qset only includes "StageIngress */
    qual_stage_only = 1;
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if(BCM_FIELD_QSET_TEST(qset, qual)) {
            if (qual != bcmFieldQualifyStageIngress) {
                qual_stage_only = 0;
                break;
            }
        }
    }
    /* Check if the qset includes any UDFs */
    if (qual_stage_only){
        for (idx = 0; idx < BCM_FIELD_USER_NUM_UDFS; idx++) {
            if (SHR_BITGET(qset.udf_map, idx)) {
                qual_stage_only = 0;
                break;
            }
        }
    }
    if(qual_stage_only != 0) {
        FP_ERR(("FP Warning: Creating group with only StageIngress in Qset\n"));
        return BCM_E_UNAVAIL;
    }

    /*
     * PacketFormat qualifiers was deprecated & replaced 
     * with VlanFormat , IpType.
     */
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyPacketFormat)) {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2Format);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat);
    }
    /*
     * Automatically include IpType 
     * if the qset contains PacketFormat/Ip4/Ip6.
     */
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp4)||
        BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyIp6)||
        BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyPacketFormat)) {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
    }

    /*
     * Vlan qualifiers ared extended to Id/Cfi/Pri
     */
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyOuterVlan)) {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanPri);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanCfi);
    }
    if (BCM_FIELD_QSET_TEST(qset, bcmFieldQualifyInnerVlan)) {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlanId);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlanPri);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlanCfi);
    }

    slice_indexes = fc->tcam_slice_sz;
    assert(0 < slice_indexes);

    /* If not specified, generate a priority. */
    if (pri == BCM_FIELD_GROUP_PRIO_ANY) {
        BCM_IF_ERROR_RETURN(_field_group_prio_make(fc, &pri));
    } else if (pri < 0 || fc->tcam_slices <= pri ) {
        FP_ERR(("FP Error: pri=%d out-of-range.\n", pri));
        return BCM_E_PARAM;
    }

    /* Group IDs must be unique within a unit. */
    if (_robo_field_group_find(unit, group) != NULL) {
        FP_ERR(("FP Error: group=%d already exists.\n", group));
        return BCM_E_EXISTS;
    }

    /* Allocate & initialize memory for field group. */
    fg = sal_alloc(sizeof (_field_group_t), "field_group");
    if (fg == NULL) {
        FP_ERR(("FP Error: Allocation failure for field_group\n"));
        return BCM_E_MEMORY;
    }

    retval = _field_group_init(unit, group, mode, pri, &qset, slice_indexes, 
                               fg);
    if (BCM_FAILURE(retval)) {
        sal_free(fg);
        FP_ERR(("FP Error: Failure in _field_group_init()\n"));
        return retval;
    }
    
    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    /* Increment the use counts for any UDFs used */
    retval = _robo_field_udf_usecount_increment(fc, fg);
    if (BCM_FAILURE(retval)) {
        sal_mutex_give(fc->fc_lock);
        sal_free(fg);
        return retval;
    }

    /* Insert new field group into head of Unit's groups list */
    fg->next = fc->groups;
    fc->groups = fg;
    fc->field_status.group_free--;
    assert(fc->field_status.group_free >= 0);
    sal_mutex_give(fc->fc_lock);

    BCM_IF_ERROR_RETURN(
        bcm_field_group_enable_set(unit, group, TRUE));

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_entry_destroy
 *
 * Purpose:
 *     Destroy an entry.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_XXX       - From bcm_field_counter_destroy() or
 *                       bcm_field_meter_destroy()
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_entry_destroy(int unit,
                bcm_field_entry_t entry)
{
    _field_group_t      *fg;
    _field_entry_t      *f_ent;

    FP_VVERB(("FP: bcm_field_entry_destroy(unit=%d, entry=%d)\n", unit, entry));
    FIELD_IS_INIT(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    fg = _robo_field_group_find(unit, f_ent->gid);
    assert(fg != NULL);

    BCM_IF_ERROR_RETURN(
        bcm_field_entry_remove(unit, entry));

    /* Destroy physical entry in primary slice */
    BCM_IF_ERROR_RETURN(
        _robo_field_entry_phys_destroy(unit, f_ent));

    /* Increment group's count of free entries. */
    fg->group_status.entries_free++;
#ifdef BCM_FIELD_SHARED_ENTRIES
    _field_shared_entries_free++;
#endif /* BCM_FIELD_SHARED_ENTRIES */
    assert(fg->group_status.entries_free <= fg->group_status.entries_total);

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_entry_destroy_all
 *
 * Purpose:
 *     Destroy all entries on a unit. It iterates over all slices in a unit.
 *     For each slice, If entries exist, it calls bcm-field_entry_destroy()
 *     using the Entry ID.
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_XXX       - Error from bcm_field_entry_destroy()
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_entry_destroy_all(int unit)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    _field_entry_t      **entries;
    uint16              slice_idx;

    FP_VVERB(("FP: bcm_field_entry_destroy_all(unit=%d)\n", unit));
    FIELD_IS_INIT(unit);
    fc = _field_control[unit];
    assert(fc != NULL);

    /* Scan unit's groups freeing each entry list. */
    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        entries   = fg->slices[0].entries;
        slice_idx = fc->tcam_sz;
        do {
            slice_idx--;
            if (entries[slice_idx] != NULL) {
                BCM_IF_ERROR_RETURN(
                    bcm_field_entry_destroy(unit, entries[slice_idx]->eid));
            }
        } while(slice_idx != 0);
    }

    return BCM_E_NONE;
}


/* bcm_field_entry_dump not dispatchable */

/*
 * Function: bcm_robo_field_entry_install
 *
 * Purpose:
 *     Install a entry into the hardware tables.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry to install
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found on unit.
 *     BCM_E_XXX       - Error from _field_XX_tcam_policy_install()
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     Qualifications should be made and actions should be added
 *     prior to installing the entry.
 */
int 
bcm_robo_field_entry_install(int unit,
                bcm_field_entry_t entry)
{
    int                 retval = BCM_E_UNAVAIL;
    int                 tcam_idx;
    _field_control_t    *fc;
    _field_entry_t      *f_ent;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    tcam_idx = _robo_field_entry_tcam_idx_get(fc, f_ent);

    /* Clear the TCAM and Policy entry */
    retval = _field_tcam_policy_clear(unit, tcam_idx);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: Failed to clear tcam and policy tables.\n"));
        sal_mutex_give(fc->fc_lock);
        return retval;
    }

    /* Install entry's meter, if it exists. */
    if (f_ent->meter != NULL) {
        retval = _field_meter_install(unit, f_ent);
        if (BCM_FAILURE(retval)) {
            FP_ERR(("FP Error: FB failed to install meter.\n"));
            sal_mutex_give(fc->fc_lock);
            return retval;
        }
        f_ent->meter->inst_flg = 1;
    }

    /* Install the TCAM entry along with any needed policies (actions).
     * Note that this includes installing any counters. */
    retval = _robo_field_tcam_policy_install(unit, f_ent, tcam_idx);
    FP_VERB(
   ("bcm_robo_field_entry_install : data= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_data[0], f_ent->drv_entry.tcam_data[1], 
        f_ent->drv_entry.tcam_data[2], f_ent->drv_entry.tcam_data[3], 
        f_ent->drv_entry.tcam_data[4], f_ent->drv_entry.tcam_data[5]));
    FP_VERB(
   ("bcm_robo_field_entry_install : mask= 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        f_ent->drv_entry.tcam_mask[0], f_ent->drv_entry.tcam_mask[1], 
        f_ent->drv_entry.tcam_mask[2], f_ent->drv_entry.tcam_mask[3], 
        f_ent->drv_entry.tcam_mask[4], f_ent->drv_entry.tcam_mask[5]));
    

    sal_mutex_give(fc->fc_lock);

    if (BCM_SUCCESS(retval)) {
        f_ent->dirty = _FIELD_CLEAN;
    } else {
        FP_ERR(("FP Error: failed to install entry.\n"));
    }

    return retval;
}

/*     
 * Function:
 *      bcm_field_entry_policer_attach
 * Purpose:
 *      Attach a policer to a field entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      entry_id   - (IN) Field entry ID.
 *      level      - (IN) Policer level.
 *      policer_id - (IN) Policer ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */ 
int
bcm_robo_field_entry_policer_attach(int unit, bcm_field_entry_t entry_id,
                                    int level, bcm_policer_t policer_id)
{
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *      bcm_field_entry_policer_detach
 * Purpose:
 *      Detach a policer from a field entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      entry_id - (IN) Field entry ID.
 *      level    - (IN) Policer level.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_field_entry_policer_detach(int unit, bcm_field_entry_t entry_id,
                                    int level)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_field_entry_policer_detach_all
 * Purpose:
 *      Detach all policers from a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry_id - (IN) Field entry ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_field_entry_policer_detach_all(int unit, bcm_field_entry_t entry_id)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_field_entry_policer_get
 * Purpose:
 *      Get the policer(s) attached to a field entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      entry_id   - (IN) Field entry ID.
 *      level      - (IN) Policer level.
 *      policer_id - (OUT) Policer ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_field_entry_policer_get(int unit, bcm_field_entry_t entry_id,
                                int level, bcm_policer_t *policer_id)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_entry_prio_get
 *
 * Purpose:
 *     Gets the priority within the group of the entry.
 *
 * Parameters:
 *     unit   - BCM device number
 *     entry  - Field entry to operate on
 *     prio   - (OUT) priority of entry
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_PARAM      - prio pointing to NULL
 *     BCM_E_NOT_FOUND  - Entry ID not found on unit
 */
int
bcm_robo_field_entry_prio_get(int unit, bcm_field_entry_t entry, int *prio)
{
    _field_entry_t      *f_ent;

    FIELD_IS_INIT(unit);

    if (prio == NULL ) {
        FP_ERR(("FP Error: prio==NULL\n"));
        return BCM_E_PARAM;
    }

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    *prio = f_ent->prio;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_entry_prio_set
 *
 * Purpose:
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Field entry to operate on
 *
 * Returns:
 *     BCM_E_NONE       Success
 */
int
bcm_robo_field_entry_prio_set(int unit, bcm_field_entry_t entry, int prio)
{
    uint16              slice_idx_target = 0, temp;
    uint16              slice_idx_old;
    _field_control_t    *fc = NULL;
    _field_group_t      *fg = NULL;
    _field_slice_t      *fs = NULL;
    _field_entry_t      *f_ent_pri = NULL;/* Entry in primary slice   */
    int    prev_null_idx = 0, next_null_idx = 0;
    int    prev_null_found = 0, next_null_found = 0;
    int    shift_up_amount = 0, shift_down_amount = 0;
    int intr_level = 0, rv;
    int flag_no_free_entries = FALSE;
    int hit = 0;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent_pri, _FP_SLICE_PRIMARY);

    /* If the priority isn't changing, just return.*/
    if (f_ent_pri->prio == prio) {
        return BCM_E_NONE;
    }

    fg = f_ent_pri->fs->group;

    robo_prio_set_with_no_free_entries = FALSE; /* Global variable */

    if (_field_robo_reqd_prio_set_move(unit, f_ent_pri, prio) == FALSE) {
        goto end;
    }
    
#ifdef BCM_FIELD_SHARED_ENTRIES
    if ((fg->group_status.entries_free <= 0) || 
        (_field_shared_entries_free <= 0)) {
#else /* BCM_FIELD_SHARED_ENTRIES*/
    if (fg->group_status.entries_free <= 0) {
#endif /* BCM_FIELD_SHARED_ENTRIES */
        if ((f_ent_pri->dirty == _FIELD_DIRTY)) {
            /*
             * As there are no free entries in any of the slices belonging to
             * this group, and this entry is NOT installed,
             *     fake that it does not exist.
             */
            intr_level = sal_splhi();
#ifdef BCM_FIELD_SHARED_ENTRIES
            _field_shared_entries[f_ent_pri->slice_idx] = NULL;
#endif
            f_ent_pri->fs->entries[f_ent_pri->slice_idx] = NULL;
            flag_no_free_entries = TRUE;
        } else {
            return BCM_E_CONFIG;
        }
    }

    slice_idx_old = f_ent_pri->slice_idx;
    assert(slice_idx_old < fc->tcam_sz);
    fs = f_ent_pri->fs;

    /* Find the target slice index. That is the one with the highest index
     * for this priority class. */
    for (slice_idx_target = 0;
         slice_idx_target < fc->tcam_sz;
         slice_idx_target++){
#ifdef BCM_FIELD_SHARED_ENTRIES
        if (_field_shared_entries[slice_idx_target] == NULL) {
#else
        if (fs->entries[slice_idx_target] == NULL) {
#endif            
            FP_VVERB(("Found an empty slice_idx=%d\n", slice_idx_target));
            /* Find the previous free entry */
            prev_null_found = 1;
            prev_null_idx = slice_idx_target;
        } else {
#ifdef BCM_FIELD_SHARED_ENTRIES        
            if (_field_shared_entries[slice_idx_target] == f_ent_pri) {
#else 
            if (fs->entries[slice_idx_target] == f_ent_pri) {
#endif
                /* Skip the entry itself */
                continue;
            }
            
#ifdef BCM_FIELD_SHARED_ENTRIES
            if (_robo_field_entry_prio_cmp(prio, 
                _field_shared_entries[slice_idx_target]->prio)
#else
            if (_robo_field_entry_prio_cmp(prio, fs->entries[slice_idx_target]->prio)
#endif            
                > 0) {
                hit = 1;
                FP_VVERB(("Found target slice_idx=%d\n", slice_idx_target));
                break;
            }
        }
        if (slice_idx_target == (fc->tcam_sz - 1)) {
            /* last entry in the slice */
            break;
        }
    }

    /* Find next free entry */
    temp = slice_idx_target;
    for (; temp < fc->tcam_sz; temp++) {
#ifdef BCM_FIELD_SHARED_ENTRIES
        if (_field_shared_entries[temp] == NULL) {
#else        
        if (fs->entries[temp] == NULL) {
#endif            
            next_null_found = 1;
            next_null_idx = temp;
            break;
        }
    }

    /* 
     * Put the entry back, 
     *     in case there is a context switch, AND
     *     another thread calls entry_create
     */
    if (flag_no_free_entries == TRUE) {
#ifdef BCM_FIELD_SHARED_ENTRIES
        _field_shared_entries[f_ent_pri->slice_idx] = f_ent_pri;
#endif
        f_ent_pri->fs->entries[f_ent_pri->slice_idx] = f_ent_pri;
        sal_spl(intr_level);
    }
    

    if ((!prev_null_found) && (!next_null_found)) {
        return BCM_E_CONFIG;
    }

    if (prev_null_found) {
        shift_up_amount = slice_idx_target - prev_null_idx;
    } else {
        shift_up_amount = fc->tcam_sz;
    }
    
    if (next_null_found) {
        shift_down_amount = next_null_idx - slice_idx_target;
    } else {
        shift_down_amount = fc->tcam_sz;
    }

    /* Move the entry at the target index to target_index+1. This may
     * mean shifting more entries down to make room. In other words,
     * shift the target index and any that follow it down 1 as far as the
     * next empty index.
     */
#ifdef BCM_FIELD_SHARED_ENTRIES
    if (_field_shared_entries[slice_idx_target] != NULL) {
#else
    if (fs->entries[slice_idx_target] != NULL) {
#endif
        if (shift_down_amount == fc->tcam_sz) {
            /* 
             * from slice_idx_target to max tcam size index are all full.
             * Can't move any existed entries down.
             * Can only set to the prev_null_idx(shift up). 
             */
            if (slice_idx_target == (fc->tcam_sz-1)) {
                if (hit) {
                    /*
                     * If ("hit" && "target idx is the lastest entry id").
                     * The original latest entry can't move down any further,
                     * the only way is to move the previous one up and insert the
                     * new rule to the previous(latest - 1) entry.
                     */
                    slice_idx_target -= 1;
                }
                BCM_IF_ERROR_RETURN(
                    _robo_field_entry_shift_up(unit, &fg->slices[0], 
                    slice_idx_target, prev_null_idx));
            } else {
                slice_idx_target -= 1;
#ifdef BCM_FIELD_SHARED_ENTRIES
                if (_field_shared_entries[slice_idx_target] != NULL) {
#else
                if (fs->entries[slice_idx_target] != NULL) {
#endif
                    /* If the previous entries are not null, need to move them up. */
                    BCM_IF_ERROR_RETURN(
                        _robo_field_entry_shift_up(unit, &fg->slices[0], 
                        slice_idx_target, prev_null_idx));
                }
            }
        } else if (shift_down_amount < shift_up_amount) {
            BCM_IF_ERROR_RETURN(
                _robo_field_entry_shift_down(unit, &fg->slices[0], 
                slice_idx_target, next_null_idx));
        } else {
            /* 
             * If the direction is up, the target entry 
             * will be the previous entry of original one. 
             */
            slice_idx_target = slice_idx_target - 1;
            BCM_IF_ERROR_RETURN(
                _robo_field_entry_shift_up(unit, &fg->slices[0], 
                slice_idx_target, prev_null_idx));
        }
    }

    /* Move the entry from its old slice index to the target slice index. */
    if (slice_idx_target != f_ent_pri->slice_idx) {
        if (flag_no_free_entries) {
            robo_prio_set_with_no_free_entries = TRUE;
        }
        rv = _robo_field_entry_move(unit, f_ent_pri,
                              slice_idx_target - f_ent_pri->slice_idx);
        if (BCM_FAILURE(rv)) {
            robo_prio_set_with_no_free_entries = FALSE; 
            return rv;
        }

        /* in case _robo_field_entry_move is called from some other function */
        robo_prio_set_with_no_free_entries = FALSE; 
    }


end:
    /* Assign the requested priority to the entry. */
    f_ent_pri->prio = prio;

    if (fg->group_status.prio_max < prio) {
        fg->group_status.prio_max = prio;
    }
    if (0 <= prio &&
        (prio < fg->group_status.prio_min || fg->group_status.prio_min < 0)) {
        fg->group_status.prio_min = prio;
    }

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_entry_reinstall
 *
 * Purpose:
 *     Re-install a entry into the hardware tables.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry to install
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found on unit
 *     BCM_E_UNAVAIL   - Feature not implemented.
 *     BCM_E_XXX       - From _bcm_XXX_install() calls
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     Reinstallation may only be used to change the actions for
 *     an installed entry without having to remove and re-add the
 *     entry.
 */
int 
bcm_robo_field_entry_reinstall(int unit,
                bcm_field_entry_t entry)
{
    _field_control_t    *fc;
    _field_entry_t      *f_ent;
    int                 retval = BCM_E_UNAVAIL;
    int                 tcam_idx;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);
    tcam_idx = _robo_field_entry_tcam_idx_get(fc, f_ent);

    /* If it exists and is changed, install the entry's meter. */
    if (f_ent->meter != NULL && f_ent->meter->inst_flg == 0) {
        retval = _field_meter_install(unit, f_ent);

        if (BCM_FAILURE(retval)) {
            FP_ERR(("FP Error: failed to install meter\n"));
            sal_mutex_give(fc->fc_lock);
            return retval;
        }
        f_ent->meter->inst_flg = 1;
    }

    if (f_ent->dirty == _FIELD_DIRTY) {
        /* Write the TCAM and Policy entry */
        retval = _robo_field_tcam_policy_install(unit, f_ent, tcam_idx);

        if (BCM_FAILURE(retval)) {
            FP_ERR(("FP Error: FB failed to install entry\n"));
        } else {
            f_ent->dirty = _FIELD_CLEAN;
        }
    } else {
        retval = BCM_E_NONE;
    }

    sal_mutex_give(fc->fc_lock);

    return retval;
}


/*
 * Function: bcm_robo_field_entry_remove
 *
 * Purpose:
 *     Remove an entry from the hardware tables.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry to remove
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found on unit
 *     BCM_E_XXX
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     This does not destroy the entry; it uninstalls it from
 *     any hardware tables.
 *     Destroy a entry using bcm_field_entry_destroy.
 */

int
bcm_robo_field_entry_remove(int unit,
                bcm_field_entry_t entry)
{
    _field_control_t    *fc;
    _field_entry_t      *f_ent;
    int                 retval = BCM_E_NONE;
    uint32              temp;

    FP_VVERB(("FP: bcm_field_entry_remove(%d, %d)\n", unit, entry));
    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    if (f_ent->dirty == _FIELD_CLEAN) {
        retval = (DRV_SERVICES(unit)->cfp_entry_read)
            (unit, f_ent->slice_idx, DRV_CFP_RAM_TCAM, &f_ent->drv_entry);
        if (BCM_FAILURE(retval)) {
            sal_mutex_give(fc->fc_lock);
            return retval;
        }
        temp = 0;
        (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_VALID, 
                &f_ent->drv_entry, &temp);
        retval = (DRV_SERVICES(unit)->cfp_entry_write)
            (unit, f_ent->slice_idx, DRV_CFP_RAM_TCAM, &f_ent->drv_entry);
        if (BCM_FAILURE(retval)) {
            sal_mutex_give(fc->fc_lock);
            return retval;
        }
        f_ent->dirty = _FIELD_DIRTY;
    }

    sal_mutex_give(fc->fc_lock);

    return retval;
}

/*
 * Function: bcm_robo_field_group_create
 *
 * Purpose:
 *     Create a field group based on the field group selector flags.
 *
 * Parameters:
 *     unit - BCM device number.
 *     port - Port number
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *     group - (OUT) New field group ID
 *
 * Returns:
 *     BCM_E_INIT     - BCM unit not initialized
 *     BCM_E_XXX      - Error code from _field_group_prio_make()
 *     BCM_E_PARAM    - pri out of range (0-15 for FB & ER) or group == NULL
 *     BCM_E_RESOURCE - no select codes will satisfy qualifier set
 *     BCM_E_NONE     - Success
 *
 * Notes:
 *      Allocates a hardware slice at the requested priority or better.
 *      Higher numerical value for priority has better priority for
 *      conflict resolution when there is a search hit on multiple slices.
 */

int 
bcm_robo_field_group_create(int unit,
               bcm_field_qset_t qset,
               int pri,
               bcm_field_group_t *group)
{
    int     rv;
    
    FIELD_IS_INIT(unit);

    rv = _robo_field_group_id_generate(unit, qset, group, pri);
    if ( rv == BCM_E_EXISTS) {
        return BCM_E_NONE;
    }

    if (BCM_FAILURE(rv)) {
        FP_ERR(("FP Error: new group won't create.\n"));
        return rv;
    }

    return bcm_field_group_create_mode_id(unit, qset, pri,
                                          bcmFieldGroupModeDefault, *group);
}


/*
 * Function: bcm_robo_field_group_create_id
 *     
 * Purpose:
 *     Create a field group based on the field group selector flags
 *     with a requested ID
 *
 * Parameters:
 *     unit - BCM device number.
 *     port - Port number
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    group - Requested field group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - No unused group/slices left
 *     BCM_E_PARAM     - priority out of range (0-15 for FB & ER)
 *     BCM_E_EXISTS    - group with that id already exists on this unit.
 *     BCM_E_MEMORY    - Group memory allocation failure
 *     BCM_E_XXX       - Error code from _field_group_prio_make().
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_group_create_id(int unit,
                  bcm_field_qset_t qset,
                  int pri,
                  bcm_field_group_t group)
{
    FIELD_IS_INIT(unit);

    return bcm_field_group_create_mode_id(unit, qset, pri,
                                          bcmFieldGroupModeDefault, group);
}

/*
 * Function: bcm_robo_field_group_destroy
 *    
 * Purpose:
 *     Delete a field group
 *     
 * Parameters:
 *     unit - BCM device number
 *     port - Port number
 *     group - Field group
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found in unit
 *     BCM_E_BUSY      - Entries not destroyed yet
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *      All entries that uses this group should have been destroyed
 *      before calling this routine.
 *      Operation will fail if entries exist that uses this temptlate
 */
int 
bcm_robo_field_group_destroy(int unit,
                bcm_field_group_t group)
{
    _field_control_t    *fc;
    _field_group_t      *fg,                /* Group structure to free up   */
                        *fg_prev = NULL;    /* Previous node in groups list */
    int                 idx;
    uint32              udf_num;
    int                 rv;

    FP_VVERB(("FP: bcm_field_group_destroy(unit=%d, group=%d)\n", unit, group));
    /* Find and validate field control. */
    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    /* Search the field control for the Group ID. */
    fg = fc->groups;
    while(fg != NULL) {
        if (fg->gid == group) {
            break;
        }
        fg_prev = fg;
        fg      = fg->next;
    }
    if (fg == NULL) {
        FP_ERR(("FP Error: Group=%d not found in unit=%d.\n", group, unit));
        return BCM_E_NOT_FOUND;
    }

    /* Entries must be freed first (see note above). */
    if (fg->group_status.entries_free != fg->group_status.entries_total) {
        FP_ERR(("FP Error: %d entries still in group=%d.\n", 
                fg->group_status.entries_total - fg->group_status.entries_free,
                group));
        return BCM_E_BUSY;
    }

    /* Clear primary slice used by this group. */
    BCM_IF_ERROR_RETURN(_robo_field_slice_clear(unit, fc, &fg->slices[0]));

    /* okay, go ahead and free the group */
    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    /* Decrement the use counts for any UDFs used by the group */
    rv = DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num);
    if (BCM_FAILURE(rv)){
        sal_mutex_give(fc->fc_lock);
        return rv;
    }
    
    for (idx = 0; idx < udf_num; idx++) {
        if (SHR_BITGET(fg->qset.udf_map, idx))  {
            if (fc->udf[idx].use_count > 0) {
                fc->udf[idx].use_count--;
            }
        }
    }

    /* Remove this group from device's linked-list of groups. */
    if (fg_prev == NULL) { /* Group is at head of list. */
        fc->groups = fg->next;
    } else {
        fg_prev->next = fg->next;
    }

    sal_free(fg);
    fc->field_status.group_free++;
    assert(fc->field_status.group_free <= fc->field_status.group_total);
    sal_mutex_give(fc->fc_lock);

    bcm_field_group_enable_set(unit, group, FALSE);

    return BCM_E_NONE;
}



/*
 * Function: bcm_robo_field_group_get
 *    
 * Purpose:
 *     Get the qualifier set for a previously created field group
 *     
 * Parameters:
 *     unit  - BCM device number
 *     port  - Port number
 *     group - Field group ID
 *     qset  - (OUT) Field qualifier set
 *     
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized.
 *     BCM_E_NOT_FOUND - Group ID not found in this unit
 *     BCM_E_PARAM     - qset is NULL
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_group_get(int unit,
            bcm_field_group_t group,
            bcm_field_qset_t *qset)
{
    _field_group_t      *fg;

    FIELD_IS_INIT(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (qset == NULL) {
        FP_ERR(("FP Error: qset == NULL\n"));
        return BCM_E_PARAM;
    }

    *qset = fg->qset;

    return BCM_E_NONE;
}

int
bcm_robo_field_group_lookup_disable(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_lookup_enable(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function: bcm_robo_field_group_get
 *
 * Purpose:
 *     Return the mode of a Group ID. This is its single, double or triple-wide
 *     state. Mode specified the number of slices allocated to the group.
 *
 * Parameters:
 *     unit  - BCM device number
 *     group - Field group ID
 *     mode  - (OUT) single, double, triple or auto mode
 *     
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found for in this unit
 *     BCM_E_PARAM     - mode pointing to NULL
 *     BCM_E_NONE      - Success
 *     
 */
int
bcm_robo_field_group_mode_get(int unit,
                             bcm_field_group_t group,
                             bcm_field_group_mode_t *mode)
{
    _field_group_t      *fg;

    FIELD_IS_INIT(unit);

    if (mode == NULL) {
        FP_ERR(("FP Error: mode=>NULL\n"));
        return BCM_E_PARAM;
    }

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found on unit=%d\n", group, unit));
        return BCM_E_NOT_FOUND;
    }

    *mode = fg->mode;

    return BCM_E_NONE;
}

int 
bcm_robo_field_group_range_get(int unit,
                  bcm_field_group_t group,
                  uint32 flags,
                  bcm_l4_port_t *min,
                  bcm_l4_port_t *max)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_group_range_set(int unit,
                  bcm_field_group_t group,
                  uint32 flags,
                  bcm_l4_port_t min,
                  bcm_l4_port_t max)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function: bcm_robo_field_group_set
 *    
 * Purpose:
 *     Update a previously created field group based on the field
 *     group selector flags
 *     
 * Parameters:
 *     unit  - BCM device number
 *     port  - Port number
 *     group - Field group ID
 *     qset  - Field qualifier set
 *     
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found for in this unit
 *     BCM_E_RESOURCE  - No select code can satisfy qualifier set
 *     BCM_E_NONE      - Success
 *     
 * Notes:
 *     If no entry exist that use this group then updates are always
 *     permitted.
 *     If entries exist that use this group then updates are permitted
 *     only if it can be satisfied with the current selection of
 *     (fpf0, fpf1, fpf2, fpf3) field selector encodings.
 */
int 
bcm_robo_field_group_set(int unit,
            bcm_field_group_t group,
            bcm_field_qset_t qset)
{
    bcm_field_qset_t        qset_pri;
    bcm_field_qset_t        qset_old;
    _field_control_t        *fc = NULL;
    _field_group_t          *fg = NULL;
    int                     retval;
    int                     re_create;
    _field_slice_t          *fs_pri = NULL;
    bcm_field_group_mode_t  mode;
    uint8                   slice;
    int     empty_qset, qual;

    FP_VVERB(("BEGIN bcm_field_group_set(unit=%d, group=%d)\n", unit, group));

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        return BCM_E_NOT_FOUND;
    }

    fs_pri           = &fg->slices[0];
    fs_pri->inst_flg = 0 ;

    BCM_FIELD_QSET_INIT(qset_pri);

    /* Check NULL qualifier set */
    empty_qset = 1;
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if(BCM_FIELD_QSET_TEST(qset, qual)) {
            empty_qset = 0;
            break;
        }
    }
    if (empty_qset) {
        return BCM_E_UNAVAIL;
    }

    /* If no entries have been added to group try requested qset.  */
    if (fg->group_status.entries_free == fg->group_status.entries_total) {
        FP_VERB(("FP: bcm_field_group_set() with no entries\n"));

        /* Get the unit's lock */
        sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

        /* Remember the group's current Qset, mode, and base slice. */
        sal_memset(&qset_old, 0, sizeof(bcm_field_qset_t));
        sal_memcpy(&qset_old, &fg->qset, sizeof(bcm_field_qset_t));
        slice = fg->slices[0].slice_numb;
        mode  = fg->mode;

        /* Destroy the old group. */
        retval = bcm_field_group_destroy(unit, group);
        if (BCM_FAILURE(retval)) {
            sal_mutex_give(fc->fc_lock);
            return retval;
        }

        /* Try to re-create the group with the requested Qset. */
        retval = bcm_field_group_create_mode_id(unit, qset, slice, mode, group);

        /* On failure, re-create the old group. */
        if (BCM_FAILURE(retval)) {
            FP_ERR(("FP Error: new Qset won't work on group=%d.\n", group));
            re_create = bcm_field_group_create_mode_id(unit, qset_old, slice,
                                                       mode, group);
            if (BCM_FAILURE(re_create)) { /* Should never fail. */
                sal_mutex_give(fc->fc_lock);
                return BCM_E_INTERNAL;
            }
        }

        /* Release the unit's lock */
        sal_mutex_give(fc->fc_lock);
        return retval;
    }

    /*
     * Handle cases where entries have been previously set in group.
     */

    /* If the requested Qset is supported by the current select codes, 
     * change the group's qset.
     */
    BCM_IF_ERROR_RETURN
        (_field_selcode_to_qset(unit, fs_pri->sel_codes, &qset_pri));

    /* qset_full represents total Qset that current select codes support. */
    if (_robo_field_qset_is_subset(&qset, &qset_pri)) {
        _robo_field_qset_union(&fg->qset, &qset, &fg->qset);
        return BCM_E_NONE;
    } 

    /* otherwise, no select codes can support */
    return BCM_E_RESOURCE;
}

int
bcm_robo_field_group_compress(int unit, bcm_field_group_t group)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_group_priority_set(int unit, bcm_field_group_t group,
                                 int priority)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_group_priority_get(int unit, bcm_field_group_t group,
                                 int *priority)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function: bcm_robo_field_group_status_get
 *
 * Purpose:
 *     Get the number of used and available entries, counters, and
 *     meters for a field group.
 *
 * Paramters:
 *     unit - BCM device number
 *     group - Field group ID
 *     status - (OUT) Status structure
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit has not been intialized
 *     BCM_E_NOT_FOUND - Group ID not found on unit.
 *     BCM_E_PARAM     - *status is NULL
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_group_status_get(int unit,
                   bcm_field_group_t group,
                   bcm_field_group_status_t *status)
{
    /* Since Robo chip didn't abled to configure the slice format
     * Show we keep this feature ???
     */
    _field_group_t      *fg;

    FIELD_IS_INIT(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: Group=%d not found in unit=%d.\n", group, unit));
        return BCM_E_NOT_FOUND;
    }

    if (status == NULL) {
        return BCM_E_PARAM;
    }

    *status = fg->group_status;

    return BCM_E_NONE;
}

int
bcm_robo_field_group_port_create_mode(int unit, bcm_port_t port,
                                         bcm_field_qset_t qset, int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_port_create_mode_id(int unit, bcm_port_t port,
                                        bcm_field_qset_t qset, int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_group_ports_create_mode(int unit, bcm_pbmp_t pbmp,
                                      bcm_field_qset_t qset, int pri,
                                      bcm_field_group_mode_t mode,
                                      bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_ports_create_mode_id(int unit, bcm_pbmp_t pbmp,
                                         bcm_field_qset_t qset, int pri,
                                         bcm_field_group_mode_t mode,
                                         bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_control_get
 *
 * Purpose:
 *     Get control status info.
 *
 * Parameters:
 *     unit     - BCM device number
 *     control  - Control element to get
 *     status   - (OUT) Status of field element
 *
 * Returns:
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_PARAM   - *state pointing to NULL
 *     BCM_E_NONE    - Success
 */
int
bcm_robo_field_control_get(int unit, bcm_field_control_t control, uint32 *state)
{
    _field_control_t    *fc;

    if (state == NULL) {
        return BCM_E_PARAM;
    }

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    switch (control) {
        case bcmFieldControlStage:
            *state = fc->stage;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_control_set
 *
 * Purpose:
 *     Set control status.
 *
 * Parameters:
 *     unit     - BCM device number
 *     control  - Control element to set
 *     status   - Status for field module
 *
 * Returns:
 *     BCM_E_NONE    - Success
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_PARAM   - Flag state not valid on device
 */
int
bcm_robo_field_control_set(int unit, bcm_field_control_t control, uint32 state)
{
    _field_control_t    *fc;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    switch (control) {
        case bcmFieldControlStage:
                fc->stage = state;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_init
 *
 * Purpose:
 *     Initialize field module.
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - Field Processor not on device.
 *     BCM_E_MEMORY  - Allocation failure
 *     BCM_E_XXX     - Error code from bcm_XX_field_init()
 *     BCM_E_NONE    - Success
 */
int 
bcm_robo_field_init(int unit)
{
    int                 udf_idx, retval = BCM_E_UNAVAIL;
    _field_control_t    *fc = NULL;
    uint32              udf_num;

    /* Make sure the Unit can support this module. */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_field)) {
        FP_ERR(("FP Error: No Field Processor available Unit=%d\n", unit));
        return BCM_E_UNAVAIL;
    }

    assert(BCM_FIELD_QUALIFY_MAX        >= bcmFieldQualifyCount);

    /* Detatch first if it's been previously initialized. */
    if (_field_control[unit] != NULL) {
        bcm_field_detach(unit);
    }

    /* Allocate a bcm_field_control */
    fc = sal_alloc(sizeof (_field_control_t), "field_control");
    if (fc == NULL) {
        FP_ERR(("FP Error: Allocation failure for Field Control\n"));
        return BCM_E_MEMORY;
    }
    sal_memset(fc, 0, sizeof (_field_control_t));

    fc->fc_lock = sal_mutex_create("field_control.lock");
    if (fc->fc_lock == NULL) {
        sal_free(fc);
        FP_ERR(("FP Error: Allocation failure for Field Control Lock\n"));
        return BCM_E_MEMORY;
    }
    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    _robo_field_tcam_info_init(unit, fc);
    _field_status_init(unit, fc, &fc->field_status);

    FP_VERB(("FP: tcam_sz=%d, tcam_slices=%d, tcam_slice_sz=%d\n",
             fc->tcam_sz, fc->tcam_slices, fc->tcam_slice_sz));

    fc->stage       = bcmFieldStageDefault;

    /* Assign the virtual UDFs to the underlying H/W */
    retval = DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num);
    if (BCM_FAILURE(retval)) {
        sal_mutex_give(fc->fc_lock);
        sal_mutex_destroy(fc->fc_lock);
        sal_free(fc);
        return retval;
    }
    
    for (udf_idx = 0; udf_idx < udf_num; udf_idx++) {
        fc->udf[udf_idx].udf_num = udf_idx;
    }

    retval = _field_slice_array_init(fc);
    if (BCM_FAILURE(retval)) {
        sal_mutex_give(fc->fc_lock);
        sal_mutex_destroy(fc->fc_lock);
        sal_free(fc);
        return retval;
    }

#ifdef BCM_FIELD_SHARED_ENTRIES
    _field_shared_entries_free = fc->tcam_sz;
#endif /* BCM_FIELD_SHARED_ENTRIES */


    if ((retval = (DRV_SERVICES(unit)->cfp_init)(unit)) < 0) {
            sal_mutex_give(fc->fc_lock);
            sal_mutex_destroy(fc->fc_lock);
            sal_free(fc);
            return retval;
    } 
    _field_control[unit] = fc;
    assert (_field_control[unit] != NULL);
    sal_mutex_give(fc->fc_lock);


    fc->tcam_pid = SAL_THREAD_ERROR;
    fc->tcam_notify = NULL;
    fc->tcam_interval = BCM_ROBO_FIELD_TCAM_THREAD_INTERVAL; /* 60 sec */
    /* Create thread for 65nm TCAM protection thread */
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53242(unit) || 
        SOC_IS_ROBO53262(unit)) {
        fc->tcam_pid = sal_thread_create("TCAM",
                     SAL_THREAD_STKSZ,
                     BCM_ROBO_FIELD_TCAM_THREAD_PRI,
                     _field_tcam_thread,
                     INT_TO_PTR(unit));
        if (fc->tcam_pid == SAL_THREAD_ERROR) {
            FP_ERR(("FP Error: Thread create failed\n"));
            return (BCM_E_MEMORY);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_meter_create
 *
 * Description:
 *      Create a meter group and associate it with the entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry ID not found on unit
 *      BCM_E_EXISTS    - Meter already created for entry
 *      BCM_E_RESOURCE  - No meters left.
 *      BCM_E_MEMORY    - Allocation failure
 *      BCM_E_NONE      - Success
 */
int 
bcm_robo_field_meter_create(int unit,
               bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_meter_t      *fm;
    _field_group_t      *fg;

    FIELD_IS_INIT(unit);

    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    fg = _robo_field_group_find(unit, f_ent->gid);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found in unit=%d.\n", f_ent->gid, unit));
        return BCM_E_NOT_FOUND;
    }

    if (f_ent->meter != NULL) {
        FP_ERR(("FP Error: entry=%d already has a meter\n", entry));
        return BCM_E_EXISTS;
    }

    fm = sal_alloc(sizeof (_field_meter_t), "field_meter");
    if (fm == NULL) {
        FP_ERR(("FP Error: allocation failure for field_meter\n"));
        return BCM_E_MEMORY;
    }
    sal_memset(fm, 0, sizeof (_field_meter_t));

    fm->index       = f_ent->slice_idx;
    fm->entries     = 1;
    fm->inst_flg    = 0;
    f_ent->mode_mtr = BCM_FIELD_METER_MODE_FLOW;
    f_ent->meter    = fm;

    fg->group_status.meters_free--;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_meter_destroy
 *
 * Description:
 *     Delete a meter associated with a entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry not found.
 *     BCM_E_EMPTY     - No entry meter exists.
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     The meter is only deallocated if no other entries are using it.
 */
int 
bcm_robo_field_meter_destroy(int unit,
                bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_group_t      *fg;
    int retval = BCM_E_NONE;
    _field_control_t    *fc;

    FP_VVERB(("FP: bcm_field_meter_destroy(unit=%d, entry=%d)\n", unit, entry));
    FIELD_IS_INIT(unit);

    fc = _robo_field_control_find(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);
    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    fg = _robo_field_group_find(unit, f_ent->gid);
    if (fg == NULL) {
        FP_ERR(("FP Error: group=%d not found in unit=%d.\n", f_ent->gid, unit));
        return BCM_E_NOT_FOUND;
    }
    
    if (f_ent->meter == NULL) {
        FP_ERR(("FP Error: no meter in entry=%d.\n", entry));
        return BCM_E_EMPTY;
    }

    /* Also clear the meter in hw */
    (DRV_SERVICES(unit)->cfp_meter_set)
        (unit, &f_ent->drv_entry, 0, 0);
    retval = _field_meter_install(unit, f_ent);
    if (BCM_FAILURE(retval)) {
        FP_ERR(("FP Error: FB failed to install meter.\n"));
        sal_mutex_give(fc->fc_lock);
        return retval;
    }
    f_ent->meter->inst_flg = 1;
    

    f_ent->meter->entries--;
    sal_free(f_ent->meter);
    f_ent->meter = NULL;
    f_ent->dirty = _FIELD_DIRTY;

    fg->group_status.meters_free++;
    
    sal_mutex_give(fc->fc_lock);
    
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_meter_get
 *
 * Description:
 *     Get the metering parameter.
 *
 * Parameters:
 *      unit        - BCM device number
 *      entry       - Entry ID
 *      meter_num   - Choice of peak or committed meter values.
 *      kbits_sec   - (OUT) Data rate setting of meter (1000/sec)
 *      kbits_burst - (OUT) Max burst setting of meter (1000)
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found on unit.
 *     BCM_E_EMPTY     - No meter in entry
 *     BCM_E_PARAM     - 'kbits_sec' or 'kbits_burst' pointing to NULL
 *                       or bad 'meter_num' not selecting single meter.
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_meter_get(int unit,
            bcm_field_entry_t entry,
            int meter_num,
            uint32 *kbits_sec,
            uint32 *kbits_burst)
{
    _field_entry_t      *f_ent;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    if (f_ent->meter == NULL) {
        FP_ERR(("FP Error: entry=%d has no meter.\n", entry));
        return BCM_E_EMPTY;
    }

    if (kbits_sec == NULL || kbits_burst == NULL) {
        FP_ERR(("kbits_sec == NULL || kbits_burst == NULL\n"));
        return BCM_E_PARAM;
    }

    if (meter_num == BCM_FIELD_METER_PEAK) {
        *kbits_sec   = f_ent->meter->peak_kbits_sec;
        *kbits_burst = f_ent->meter->peak_kbits_burst;
        return BCM_E_NONE;
    } else if (meter_num == BCM_FIELD_METER_COMMITTED) {
        *kbits_sec   = f_ent->meter->commit_kbits_sec;
        *kbits_burst = f_ent->meter->commit_kbits_burst;
        return BCM_E_NONE;
    } else {
        FP_ERR(("FP Error: bad meter_num.\n"));
        return BCM_E_PARAM;
    }
}

/*
 * Function: bcm_robo_field_meter_set
 *
 * Description:
 *     Set the metering parameter.
 *
 * Parameters:
 *     unit        - BCM device number
 *     entry       - Entry ID
 *     meter_num   - Entry in meter group (BCM_FIELD_METER_xxx)
 *     kbits_sec   - Data rate to associate with meter (1000/sec)
 *     kbits_burst - Max burst to associate with meter (1000)
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized.
 *     BCM_E_NOT_FOUND - Entry not found.
 *     BCM_E_EMPTY     - No entry meter exists.
 *     BCM_E_PARAM     - Bad value for 'meter_num'
 *     BCM_E_NONE      - Success
 */
int 
bcm_robo_field_meter_set(int unit,
            bcm_field_entry_t entry,
            int meter_num,
            uint32 kbits_sec,
            uint32 kbits_burst)
{
    _field_entry_t      *f_ent;
    _field_meter_t      *meter;
    uint8               flag = 0;
    uint32              max_meter_burst = BCM_FIELD_METER_KBITS_BURST_MAX;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Confirm that entry has a meter associated with it. */
    if (f_ent->meter == NULL) {
        FP_ERR(("FP Error: no meter in entry=%d.\n", entry));
        return BCM_E_EMPTY;
    }

    /* Range check refresh rate (kbits_sec). */
    if (kbits_sec > BCM_FIELD_METER_KBITS_RATE_MAX) {
        FP_WARN(("FP Warning: excess rate=0x%x > rate_max=0x%x.\n",
                kbits_sec, BCM_FIELD_METER_KBITS_RATE_MAX));
        kbits_sec = BCM_FIELD_METER_KBITS_RATE_MAX;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        max_meter_burst = BCM_53242_FIELD_METER_KBITS_BURST_MAX;
    } else {
        max_meter_burst = BCM_FIELD_METER_KBITS_BURST_MAX;
    }
    /* Range check bucket size (kbits_burst). */
    if (kbits_burst > max_meter_burst) {
        FP_WARN(("FP Warning: excess burst=0x%x > burst_max=0x%x.\n",
                kbits_burst, max_meter_burst));
        kbits_burst = max_meter_burst;
    }

    meter = f_ent->meter;

    if (meter_num & BCM_FIELD_METER_COMMITTED) {
        meter->commit_kbits_sec   = kbits_sec;
        meter->commit_kbits_burst = kbits_burst;
        flag = 1;
        (DRV_SERVICES(unit)->cfp_meter_set)
            (unit, &f_ent->drv_entry, kbits_sec, kbits_burst);
    }

    if (flag == 0) {
        FP_ERR(("FP Error: bad meter_num=%d.\n", meter_num));
        return BCM_E_UNAVAIL;
    } 

    meter->inst_flg = 0; /* mark meter as not installed */

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_meter_share
 *
 * Description:
 *     Share the meter pair from one entry with a second entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      src_entry - Entry ID that has meter group
 *      dst_entry - Entry ID that will get meter group
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Source or destination entry not found on unit.
 *      BCM_E_PARAM     - Entries not from same slice.
 *      BCM_E_EXISTS    - Destination entry already has meter.
 *      BCM_E_NONE      - Success
 *
 * Notes:
 *     The src_entry and dst_entry should belong to the field group
 */
int 
bcm_robo_field_meter_share(int unit,
              bcm_field_entry_t src_entry,
              bcm_field_entry_t dst_entry)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_qualify_clear
 *
 * Purpose:
 *     Remove all field qualifications from a filter entry
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Field entry to operate on
 *
 * Returns:
 *     BCM_E_INIT       BCM Unit not initialized.
 *     BCM_E_NOT_FOUND  Entry ID not found in unit.
 *     BCM_E_NONE       Success
 */
int 
bcm_robo_field_qualify_clear(int unit,
                bcm_field_entry_t entry)
{
    _field_entry_t      *f_ent;
    _field_group_t      *fg;
    uint32 temp;

    FIELD_IS_INIT(unit);
    FIELD_ENTRY_GET(unit, entry, f_ent, _FP_SLICE_PRIMARY);

    /* Clear f_ent->tcam(mask fields initialized to "don't cares" */
    sal_memset(&f_ent->drv_entry.tcam_data[0], 0, 
        sizeof(f_ent->drv_entry.tcam_data));
    sal_memset(&f_ent->drv_entry.tcam_mask[0], 0, 
        sizeof(f_ent->drv_entry.tcam_mask));

    /* 
     * The slice ID  should not be reset.
     * Without this value, the slice format is unknown.
     */
    fg = _robo_field_group_find(unit, f_ent->gid);
    if (fg != NULL) {
        temp = fg->slices[0].sel_codes.fpf;
        BCM_IF_ERROR_RETURN(
            _robo_field_entry_slice_id_set(unit, f_ent, temp));
         
        f_ent->dirty = _FIELD_DIRTY;
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;

}

int 
bcm_robo_field_qualify_DSCP(int unit, bcm_field_entry_t entry,
               uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyDSCP);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyDSCP, f_ent, data, mask);

    return rv;
}

int
bcm_robo_field_qualify_Decap(int unit, bcm_field_entry_t entry,
                            bcm_field_decap_t decap)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyForwardingVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Vpn_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyVpn
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vpn_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vpn_t *data, 
    bcm_vpn_t *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanId
 * Purpose:
 *      Set match criteria for bcmFieildQualifyForwardingVlanId
 *      qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_Vpn
 * Purpose:
 *      Set match criteria for bcmFieildQualifyVpn
 *      qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vpn(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vpn_t data, 
    bcm_vpn_t mask)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_Drop(int unit, bcm_field_entry_t entry,
                                     uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_DstIp(int unit, bcm_field_entry_t entry,
                bcm_ip_t data, bcm_ip_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyDstIp);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyDstIp, f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_DstIp6(int unit, bcm_field_entry_t entry,
                 bcm_ip6_t data, bcm_ip6_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data_field[4], mask_field[4];

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyDstIp6);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }
    SAL_IP6_ADDR_TO_UINT32(data, data_field);
    SAL_IP6_ADDR_TO_UINT32(mask, mask_field);

    rv = _robo_field_qual_value_set(unit, bcmFieldQualifyDstIp6, f_ent, 
        data_field, mask_field);
    return rv;
}

int 
bcm_robo_field_qualify_DstIp6High(int unit, bcm_field_entry_t entry,
                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_DstMac(int unit, bcm_field_entry_t entry,
                 bcm_mac_t data, bcm_mac_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      mac_field[2], mask_field[2];

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyDstMac);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(mac_field, 0, 8);
    sal_memset(mask_field, 0, 8);
    SAL_MAC_ADDR_TO_UINT32(data, mac_field);
    SAL_MAC_ADDR_TO_UINT32(mask, mask_field);

    rv = _robo_field_qual_value_set(unit, bcmFieldQualifyDstMac, f_ent, 
        mac_field, mask_field);

    return rv;
}

int 
bcm_robo_field_qualify_DstModid(int unit, bcm_field_entry_t entry,
                   bcm_module_t data, bcm_module_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_robo_field_qualify_DstPortTgid(int unit, bcm_field_entry_t entry,
                  bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_EtherType(int unit, bcm_field_entry_t entry,
                uint16 data, uint16 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyEtherType);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyEtherType, 
                     f_ent, data, mask);

    return rv;
}

/*
 * Function:
 *      bcm_robo_field_qualifier_delete
 * Purpose:
 *      Remove match criteria from a field processor entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual_id - (IN) BCM field qualifier id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualifier_delete(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_qualify_t qual_id)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}

int
bcm_robo_field_qualify_InPort(int unit, bcm_field_entry_t entry,
                             bcm_port_t data, bcm_port_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    if (0 == SOC_PORT_VALID(unit, data)) {
        return (BCM_E_PARAM);
    }

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInPort);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }


    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
        uint32 mask_val;
        bcm_pbmp_t pbmp;
        
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_PORT_ADD(pbmp, data);
        mask_val = ~(SOC_PBMP_WORD_GET(pbmp, 0));
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInPorts, 
                                 f_ent, 
                                 0,
                                 mask_val);
    } else {
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifySrcPort, f_ent, 
            data, mask);
    }
    return rv;
}

int
bcm_robo_field_qualify_OutPort(int unit, bcm_field_entry_t entry,
                               bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_InPorts(int unit, bcm_field_entry_t entry,
                  bcm_pbmp_t data, bcm_pbmp_t mask)
{
     _field_entry_t      *f_ent;
    int                 rv;
    uint32 data_val = 0, mask_val = 0;
    
    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInPorts);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
        data_val = 0;
        mask_val = ~(SOC_PBMP_WORD_GET(data, 0));
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInPorts, 
                                 f_ent, 
                                 0,
                                 mask_val);
    } else {
    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInPorts, 
                             f_ent, 
                             SOC_PBMP_WORD_GET(data, 0),
                            SOC_PBMP_WORD_GET(mask, 0));
    }
    return rv;
}

int 
bcm_robo_field_qualify_Ip6FlowLabel(int unit, bcm_field_entry_t entry,
                   uint32 data, uint32 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIp6FlowLabel);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIp6FlowLabel, f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_Ip6HopLimit(int unit, bcm_field_entry_t entry,
                  uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint8    new_data, new_mask;
    uint32      fld_index = 0;
    uint32      temp;


    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find
                (unit, entry, bcmFieldQualifyIp6HopLimit);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* Since BCM53115 and BCM53242 only have some values to set. */
    /* The selections are 0, 1, 255 and others */
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit)) {
        new_mask = 3;
        switch (data) {
            case 0:
                new_data = 0;
                break;
            case 1:
                new_data = 1;
                break;
            case 255:
                new_data = 3;
                break;
            default:
                new_data = 2;
        }
        rv = _robo_field_qual_value32_set
                (unit, bcmFieldQualifyIp6HopLimit, 
                 f_ent, new_data, new_mask);
    } else {
        rv = _robo_field_qual_value32_set
                (unit, bcmFieldQualifyIp6HopLimit, 
                 f_ent, data, mask);
    }
    if (rv < 0) {
        return rv;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* 
         * Since the Ip6HopLimit qualifier id is equal to 
         * Ttl, the L3_FRM_FORMAT field will be programmed 
         * as IP4 at _robo_field_qual_value32_set().
         *
         * The L3_FRM_FORMAT has to be configured to IP6
         * after set qualifier value of Ip6HopLimit.
         */
        fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
        temp = FP_BCM53242_L3_FRM_FMT_IP6;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM, fld_index,
                &f_ent->drv_entry, &temp));

        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                &f_ent->drv_entry, &temp));
    }


    return rv;
}

int 
bcm_robo_field_qualify_Ip6NextHeader(int unit, bcm_field_entry_t entry,
                    uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      fld_index = 0;
    uint32      temp;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find
                (unit, entry, bcmFieldQualifyIp6NextHeader);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set
            (unit, bcmFieldQualifyIp6NextHeader, 
             f_ent, data, mask);
    if (rv < 0) {
        return rv;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* 
         * Since the Ip6NextHeader qualifier id is equal to 
         * IpProtocol, the L3_FRM_FORMAT field will be programmed 
         * as IP4 at _robo_field_qual_value32_set().
         *
         * The L3_FRM_FORMAT has to be configured to IP6
         * after set qualifier value of Ip6NextHeader.
         */
        fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
        temp = FP_BCM53242_L3_FRM_FMT_IP6;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM, fld_index,
                &f_ent->drv_entry, &temp));

        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                &f_ent->drv_entry, &temp));
    }


    return rv;
}

int 
bcm_robo_field_qualify_Ip6TrafficClass(int unit, bcm_field_entry_t entry,
                      uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      fld_index = 0;
    uint32      temp;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find
                (unit, entry, bcmFieldQualifyIp6TrafficClass);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set
            (unit, bcmFieldQualifyIp6TrafficClass, 
             f_ent, data, mask);
    if (rv < 0) {
        return rv;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* 
         * Since the Ip6TrafficClass qualifier id is equal to 
         * DSCP, the L3_FRM_FORMAT field will be programmed 
         * as IP4 at _robo_field_qual_value32_set().
         *
         * The L3_FRM_FORMAT has to be configured to IP6
         * after set qualifier value of Ip6TrafficClass.
         */
        fld_index = DRV_CFP_FIELD_L3_FRM_FORMAT;
        temp = FP_BCM53242_L3_FRM_FMT_IP6;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM, fld_index,
                &f_ent->drv_entry, &temp));
    
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM_MASK, fld_index,
                &f_ent->drv_entry, &temp));
    }

    return rv;
}

int 
bcm_robo_field_qualify_IpFlags(int unit, bcm_field_entry_t entry,
                  uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_robo_field_qualify_IpProtocol(int unit, bcm_field_entry_t entry,
                 uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIpProtocol);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIpProtocol, 
                               f_ent, data, mask);

    return rv;
}

int
bcm_robo_field_qualify_ForwardingType(int unit, bcm_field_entry_t entry,
                             bcm_field_ForwardingType_t data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_IpType(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t data)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32              new_data = 0, new_mask= 0;
    
    FIELD_IS_INIT(unit);

    if (data >= bcmFieldIpTypeCount) {
        return (BCM_E_PARAM);
    }

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIpType);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        switch(data) {
            case bcmFieldIpTypeIpv4Any:
                new_data = FP_BCM53242_L3_FRM_FMT_IP4;
                break;
            case bcmFieldIpTypeIpv6:
                new_data = FP_BCM53242_L3_FRM_FMT_IP6;
                break;
            case bcmFieldIpTypeNonIp:
                new_data = FP_BCM53242_L3_FRM_FMT_OTHERS;
                break;
            default:
    return BCM_E_UNAVAIL;
                break;
        }
        new_mask = FP_BCM53242_L3_FRM_FMT_MASK;
    } else if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) || 
               SOC_IS_ROBO5395(unit)) {
        switch (data) {
            case bcmFieldIpTypeIpv4Any:
                /* 
                 * For BCM5348/5347, 
                 * only DRV_CFP_FIELD_IPV4_VALID (1 bit)supported.
                 */
                new_data = 0x1;
                new_mask = 0x1;
                break;
            default:
                return BCM_E_UNAVAIL;
                break;
        }
    } else if (SOC_IS_ROBO53115(unit)) {
        switch (data) {
            case bcmFieldIpTypeIpv4Any:
                new_data = FP_BCM53115_L3_FRM_FMT_IP4;
                break;
            case bcmFieldIpTypeIpv6:
                new_data = FP_BCM53115_L3_FRM_FMT_IP6;
                break;
            case bcmFieldIpTypeNonIp:
                new_data = FP_BCM53115_L3_FRM_FMT_NON_IP;
                break;
            default:
                return BCM_E_UNAVAIL;
                break;
        }
        new_mask = FP_BCM53115_L3_FRM_FMT_MASK;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIpType, 
                               f_ent, new_data, new_mask);

    return rv;
}

int
bcm_robo_field_qualify_L3IntfGroup(int unit, bcm_field_entry_t entry,
                                  bcm_if_group_t data, bcm_if_group_t mask)
{
    return BCM_E_UNAVAIL;
}



int 
bcm_robo_field_qualify_L4DstPort(int unit, bcm_field_entry_t entry,
                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyL4DstPort);
    if (f_ent == NULL) {
        /* 
         * Gnat 15843 : The qualifier,  "bcmFieldQualifyRangeCheck", 
         * represented 2 qualifiers, "bcmFieldQualifyL4SrcPort" and 
         * "bcmFieldQualifyL4DstPort", for BCM5348/BCM5347.
         */
        if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
            f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyRangeCheck);
            if (f_ent == NULL) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyL4DstPort, 
                                 f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_L4SrcPort(int unit, bcm_field_entry_t entry,
                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyL4SrcPort);
    if (f_ent == NULL) {
        /* 
         * Gnat 15843 : The qualifier,  "bcmFieldQualifyRangeCheck", 
         * represented 2 qualifiers, "bcmFieldQualifyL4SrcPort" and 
         * "bcmFieldQualifyL4DstPort", for BCM5348/BCM5347.
         */
        if (SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5348(unit)) {
            f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyRangeCheck);
            if (f_ent == NULL) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyL4SrcPort, 
                                  f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_LookupStatus(int unit, bcm_field_entry_t entry,
                   uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_MHOpcode(int unit, bcm_field_entry_t entry,
                   uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanId
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanId
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlanId);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyOuterVlanId, 
                             f_ent, data, mask);

    return rv;
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanPri
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanPri
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanPri(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlanPri);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyOuterVlanPri, 
                             f_ent, data, mask);

    return rv;
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanCfi
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanCfi
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanCfi(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlanCfi);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyOuterVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyOuterVlanCfi, 
                             f_ent, data, mask);

    return rv;
}



int 
bcm_robo_field_qualify_OuterVlan(int unit, bcm_field_entry_t entry,
                  bcm_vlan_t data, bcm_vlan_t mask)
{
    int rv; /* Operation return status. */

    rv = bcm_robo_field_qualify_OuterVlanId(unit, entry, 
                                           (data & 0xfff), 
                                           (mask & 0xfff));
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_robo_field_qualify_OuterVlanCfi(unit, entry, 
                                            ((data >> 12) & 0x1),
                                            ((mask >> 12) & 0x1));
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_robo_field_qualify_OuterVlanPri(unit, entry,
                                            (data >> 13),
                                            (mask >> 13));
    return (rv);
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanId
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanId
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlanId);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInnerVlanId, 
                             f_ent, data, mask);

    return rv;
}


/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanPri
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanPri
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanPri(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlanPri);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInnerVlanPri, 
                             f_ent, data, mask);

    return rv;
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanCfi
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanCfi
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanCfi(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlanCfi);
    if (f_ent == NULL) {
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyInnerVlan);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyInnerVlanCfi, 
                             f_ent, data, mask);

    return rv;
}


int 
bcm_robo_field_qualify_InnerVlan(int unit, bcm_field_entry_t entry,
                 bcm_vlan_t data, bcm_vlan_t mask)
{
    int rv; /* Operation return status. */

    rv = bcm_robo_field_qualify_InnerVlanId(unit, entry, 
                                           (data & 0xfff), 
                                           (mask & 0xfff));
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_robo_field_qualify_InnerVlanCfi(unit, entry, 
                                            ((data >> 12) & 0x1),
                                            ((mask >> 12) & 0x1));
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_robo_field_qualify_InnerVlanPri(unit, entry,
                                            (data >> 13),
                                            (mask >> 13));
    return (rv);
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanPri_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanPri
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanPri_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanCfi_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanCfi_get
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanCfi_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanPri_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanPri
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanPri_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanCfi_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanCfi_get
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanCfi_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL; 
}

int 
bcm_robo_field_qualify_PacketFormat(int unit, bcm_field_entry_t entry,
                   uint16 data, uint16 mask)
{
    _field_entry_t      *f_ent;
    int                 rv = BCM_E_NONE;
    bcm_field_IpType_t  iptype;
    bcm_field_L2Format_t l2format;
    uint8               data_802_3;
    uint32              new_data = 0, new_mask = 0;
    
    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyPacketFormat);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53115(unit)) {
        if (mask & BCM_FIELD_PKT_FMT_IP_MASK) {
            if (data & BCM_FIELD_PKT_FMT_IPV4) {
                new_data = FP_BCM53115_L3_FRM_FMT_IP4;
            } else if (data & BCM_FIELD_PKT_FMT_IPV6) {
                new_data = FP_BCM53115_L3_FRM_FMT_IP6;
            } else {
                new_data = FP_BCM53115_L3_FRM_FMT_NON_IP;
            }     
        
            new_mask = FP_BCM53115_L3_FRM_FMT_MASK;
        } else {
            new_data = 0;
            new_mask = 0;
        }

        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIpType, 
                               f_ent, new_data, new_mask);      
        if (rv < 0) {
            return rv;
        }      

        if (mask & BCM_FIELD_PKT_FMT_L2_MASK) {
            data_802_3 = data & BCM_FIELD_PKT_FMT_L2_MASK;
            switch (data_802_3) {
                case BCM_FIELD_PKT_FMT_L2_ETH_II:
                    new_data = 0x0;
                    new_mask = 0x3;
                    break;
                case BCM_FIELD_PKT_FMT_L2_SNAP:
                    /* include SNAP_Public and SNAP_Private */
                    new_data = 0x1;
                    new_mask = 0x1; 
                    break;
                case BCM_FIELD_PKT_FMT_L2_LLC:
                case BCM_FIELD_PKT_FMT_L2_802_3:
                    new_data = 0x2;
                    new_mask = 0x3;
                    break;
                default:
                    rv = BCM_E_UNAVAIL;
                    return rv;
            }
        } else {
            new_data = 0;
            new_mask = 0;
        }      

        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyL2Format, 
                               f_ent, new_data, new_mask);     
        if (rv < 0) {
            return rv;
        }
       
        new_data = (data & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
        new_mask = (mask & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
       
        rv = bcm_field_qualify_VlanFormat(unit, entry, new_data, new_mask);
        
        return rv;
    }

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* If including IPv4 or IPv6, set L3_FRAME_FORMAT */
        new_data = (data & BCM_FIELD_PKT_FMT_IP_MASK);
        new_mask = (mask & BCM_FIELD_PKT_FMT_IP_MASK);
        /* Set iptype default value */
        iptype = bcmFieldIpTypeCount;
        if (new_data && new_mask) {
            if ((new_data & new_mask) == BCM_FIELD_PKT_FMT_IPV4) {
                iptype = bcmFieldIpTypeIpv4Any;
            } else if ((new_data & new_mask) ==  BCM_FIELD_PKT_FMT_IPV6) {
                iptype = bcmFieldIpTypeIpv6;
            }
        
            rv = bcm_field_qualify_IpType(unit, entry, iptype);
            if (rv < 0) {
                return rv;
            }
        }
        /* If including EtherII or SNAP, set L2_FRAME_FORMAT */
        new_data = (data & BCM_FIELD_PKT_FMT_L2_MASK);
        new_mask = (mask & BCM_FIELD_PKT_FMT_L2_MASK);
        switch (new_data & new_mask) {
            case BCM_FIELD_PKT_FMT_L2_ETH_II:
                if (new_mask) {
                    l2format = bcmFieldL2FormatEthII; 
                } else {
                    l2format = bcmFieldL2FormatAny; 
                }
                break;
            case BCM_FIELD_PKT_FMT_L2_SNAP:
                l2format = bcmFieldL2FormatSnap;
                break;
            default:
                /* Use bcmFieldL2FormatLlc to represent other L2 frame types */
                l2format = bcmFieldL2FormatLlc;
                break;
        }
        
        rv = bcm_field_qualify_L2Format(unit, entry, l2format);
        if (rv < 0) {
            return rv;
        }

        new_data = (data & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
        new_mask = (mask & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
    
        rv = bcm_field_qualify_VlanFormat(unit, entry, new_data, new_mask);

        return rv;
    }

    if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)||SOC_IS_ROBO5395(unit)){
        /* If including IPv4 or IPv6, set L3_FRAME_FORMAT */
        new_data = (data & BCM_FIELD_PKT_FMT_IP_MASK);
        new_mask = (mask & BCM_FIELD_PKT_FMT_IP_MASK);
        /* Set iptype default value */
        iptype = bcmFieldIpTypeCount;
        if (new_data && new_mask) {
            if ((new_data & new_mask) == BCM_FIELD_PKT_FMT_IPV4) {
                iptype = bcmFieldIpTypeIpv4Any;
            }
            rv = bcm_field_qualify_IpType(unit, entry, iptype);
            if (rv < 0) {
                return rv;
            }
        }

        /* If including EtherII or SNAP, set L2_FRAME_FORMAT */
        new_data = (data & BCM_FIELD_PKT_FMT_L2_MASK);
        new_mask = (mask & BCM_FIELD_PKT_FMT_L2_MASK);
        switch (new_data & new_mask) {
            case BCM_FIELD_PKT_FMT_L2_ETH_II:
                if (new_mask) {
                    l2format = bcmFieldL2FormatEthII; 
                } else {
                    l2format = bcmFieldL2FormatAny; 
                }
                break;
            case BCM_FIELD_PKT_FMT_L2_802_3:
                l2format = bcmFieldL2Format802dot3;
                break;
            case BCM_FIELD_PKT_FMT_L2_SNAP:
                l2format = bcmFieldL2FormatSnap;
                break;
            case BCM_FIELD_PKT_FMT_L2_LLC:
                l2format = bcmFieldL2FormatLlc;
                break;
            default:
                FP_ERR(("FP Error: Invalid l2 format =%d\n",(new_data & new_mask)));
                return (BCM_E_INTERNAL);
                break;
        }
        
        rv = bcm_field_qualify_L2Format(unit, entry, l2format);
        if (rv < 0) {
            return rv;
        }

        new_data = (data & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
        new_mask = (mask & BCM_FIELD_PKT_FMT_TAGGED_MASK) >> 2;
    
       rv = bcm_field_qualify_VlanFormat(unit, entry, new_data, new_mask);

        return rv;
    }
    return rv;
}

int 
bcm_robo_field_qualify_L2Format(int unit, bcm_field_entry_t entry,
                                bcm_field_L2Format_t type)
{
    _field_entry_t      *f_ent;
    int                 rv = BCM_E_NONE;
    uint32              data = 0, mask = 0;
    uint32              field_type = 0;
    
    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyL2Format);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53115(unit)) {
        switch (type) {
          case bcmFieldL2FormatAny:
              data = 0x0;
              mask = 0x0;
              break; 
          case bcmFieldL2FormatEthII:
              data = 0x0;
              mask = 0x3;
              break;
          case bcmFieldL2FormatSnap:
              data = 0x1;
              mask = 0x1; 
              break;
          case bcmFieldL2FormatLlc:
          case bcmFieldL2Format802dot3:
              data = 0x2;
              mask = 0x3;
              break;
          default:
              FP_ERR(("FP Error: %d not supported on unit=%d\n", type, unit));
              return (BCM_E_UNAVAIL);
              break;
        }
        rv = _robo_field_qual_value32_set
            (unit, bcmFieldQualifyL2Format, f_ent, data, mask);
    } else if (SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit)) {
        switch (type) {
          case bcmFieldL2FormatAny:
              data = 0x0;
              mask = 0x0;
              break; 
          case bcmFieldL2FormatEthII:
              data = 0x1;
              mask = 0x3;
              break;
          case bcmFieldL2FormatSnap:
              data = 0x2;
              mask = 0x3; 
              break;
          default:
              data = 0x0;
              mask = 0x3; 
              break;
        }
        rv = _robo_field_qual_value32_set
            (unit, bcmFieldQualifyL2Format, f_ent, data, mask);
    } else if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)||
        SOC_IS_ROBO5395(unit)) {
        if (type != bcmFieldL2FormatAny) {
            switch(type) {
                case bcmFieldL2Format802dot3:
                case bcmFieldL2FormatEthII:
                case bcmFieldL2FormatLlc:
                    field_type = DRV_CFP_FIELD_EII_OR_8023;
                    break;
                case bcmFieldL2FormatSnap:
                    field_type = DRV_CFP_FIELD_IEEE_SNAP;
                    break;
                default:
                    FP_ERR(("FP Error: %d not supported on unit=%d\n", type, unit));
                    return (BCM_E_UNAVAIL);
                    break;
    }
            data = 1;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, field_type, 
                    &f_ent->drv_entry, &data));

            mask = 1;
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, field_type, 
                    &f_ent->drv_entry, &mask));
        }
    }
    
    return rv;
}

int 
bcm_robo_field_qualify_VlanFormat(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32 new_data, new_mask;
    int    dtag_mode = BCM_PORT_DTAG_MODE_NONE;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyVlanFormat);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = bcm_port_dtag_mode_get(unit, 0, &dtag_mode);
    if (rv < 0) {
        return rv;
    }
    
    if (dtag_mode ==  BCM_PORT_DTAG_MODE_NONE) {
        /* Check 1q VLAN tag */
        if (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
            new_data = (data & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED);
            new_mask = (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED);
            if (SOC_IS_ROBO53115(unit)) {
                if (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO) {
                     new_mask |= 0x2;
                     if (!(data & 
                        BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO)) {
                        new_data |= 0x2;
                     }
                }
            }
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_1QTAGGED, 
                    &f_ent->drv_entry, &new_data));
            
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_1QTAGGED, 
                    &f_ent->drv_entry, &new_mask));
        }
    } else {
        if (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
            new_data = (data & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED);
            new_mask = (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED);
            if (SOC_IS_ROBO53115(unit)) {
                if (mask & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO) {
                     new_mask |= 0x2;
                     if (!(data & 
                        BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO)) {
                        new_data |= 0x2;
                     }
                }
            }
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_SPTAGGED, 
                    &f_ent->drv_entry, &new_data));
            
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_SPTAGGED, 
                    &f_ent->drv_entry, &new_mask));
        }
        if (mask & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
            new_data = (data & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) >> 1;
            new_mask = (mask & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) >> 1;
            if (SOC_IS_ROBO53115(unit)) {
                if (mask & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO) {
                     new_mask |= 0x2;
                     if (!(data & 
                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO)) {
                        new_data |= 0x2;
                     }
                }
            }
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_1QTAGGED, 
                    &f_ent->drv_entry, &new_data));
            
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_1QTAGGED, 
                    &f_ent->drv_entry, &new_mask));
        }
    }

    return rv;
}

int 
bcm_robo_field_qualify_RangeCheck(int unit, bcm_field_entry_t entry,
                 bcm_field_range_t range, int invert)
{
    _field_entry_t      *f_ent;
    int                 rv;
    _field_control_t *fc;
    _field_range_t      *fr;
    int                 hw_index;
    uint32  fld_data = 0, fld_mask = 0;
    uint32 flags = 0, range_type = 0;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyRangeCheck);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        /* 
         * Check the hw_index of this range is 4 or not.
         * If the hw_index is 4 then set the L4SRC_LESS1024 qualifier,
         * otherwise configure the range checker field.
         */
        hw_index = -1;
        fc = _robo_field_control_find(unit);
        
        for (fr = fc->ranges; fr != NULL; fr = fr->next) {
            if (fr->rid == range) {
                hw_index = fr->hw_index;
                flags = fr->flags;
                break;
            }
        }

        if (hw_index < 0) {
            FP_ERR(("FP Error: Ranger not created yet.\n"));
            return BCM_E_PARAM;
        }

        if (hw_index > 9) {
            return BCM_E_INTERNAL;
        }

        if (hw_index == 9) {
            if (invert < 0) {
                fld_mask = 0x0;
            } else {
                fld_mask = 0x1;
                if (invert) {
                    fld_data = 0x0;
                } else {
                    fld_data = 0x1;
                }
            }
            
            rv = _robo_field_qual_value32_set(unit, 
                bcmFieldQualifyRangeCheck, f_ent, fld_data, fld_mask);
        } else {

            if (flags & BCM_FIELD_RANGE_OUTER_VLAN) {
                hw_index -= 4;
                range_type = DRV_CFP_FIELD_VLAN_RANGE;
            } else {
                range_type = DRV_CFP_FIELD_L4_PORT_RANGE;
            }

            if (invert < 0) {
                fld_mask &= ~(1 << hw_index);

            } else {
                fld_mask |= (1 << hw_index);
        
                if (invert) {
                    fld_data &= ~(1 << hw_index);
                } else {
                    fld_data |= (1 << hw_index);
                }
            }

            rv = (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM, range_type, 
                &f_ent->drv_entry, &fld_data);
    
            rv = (DRV_SERVICES(unit)->cfp_field_set)
                (unit, DRV_CFP_RAM_TCAM_MASK, range_type, 
                &f_ent->drv_entry, &fld_mask);
        }
        
        
    } else {
        if (invert < 0) {
            fld_mask = 0x0;
        } else {
            fld_mask = 0x1;
    
            if (invert) {
                fld_data = 0x0;
            } else {
                fld_data = 0x1;
            }
        }

        /* BCM5348/BCM5347 */
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyRangeCheck, 
                                 f_ent, fld_data, fld_mask);
    }

    return rv;
}

int 
bcm_robo_field_qualify_SrcIp(int unit, bcm_field_entry_t entry,
                bcm_ip_t data, bcm_ip_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcIp);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifySrcIp, f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_SrcIp6(int unit, bcm_field_entry_t entry,
                 bcm_ip6_t data, bcm_ip6_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data_field[4], mask_field[4];

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcIp6);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    SAL_IP6_ADDR_TO_UINT32(data, data_field);
    SAL_IP6_ADDR_TO_UINT32(mask, mask_field);

    rv = _robo_field_qual_value_set(unit, bcmFieldQualifySrcIp6, f_ent, 
        data_field, mask_field);

    return rv;
}

int
bcm_robo_field_qualify_SrcIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_robo_field_qualify_SrcMac(int unit, bcm_field_entry_t entry,
                 bcm_mac_t data, bcm_mac_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32  mac_field[2], mask_field[2];

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcMac);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(mac_field, 0, 8);
    sal_memset(mask_field, 0, 8);
    SAL_MAC_ADDR_TO_UINT32(data, mac_field);
    SAL_MAC_ADDR_TO_UINT32(mask, mask_field);
    rv = _robo_field_qual_value_set(unit, bcmFieldQualifySrcMac, f_ent, 
        mac_field, mask_field);

    return rv;
}

/*
 * Function: bcm_robo_field_qualify_SrcMacGroup
 *
 * Purpose:
 *     Add Src MAC group qualification data and mask to an entry.
 *
 * Parameters:
 *     unit            - BCM unit number
 *     entry           - Entry ID
 *     data            - Src MAC Group (a.k.a. MAC Block index)
 *     mask            - mask for the above data
 * Returns:
 *     BCM_E_INIT      - Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found in unit.
 *     BCM_E_PARAM     - Wrong parameter
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_qualify_SrcMacGroup(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InterfaceClassL2(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InterfaceClassL3(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InterfaceClassPort(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassL2(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassL3(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassField(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassL2(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassL3(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassField(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}



int 
bcm_robo_field_qualify_SrcModid(int unit, bcm_field_entry_t entry,
                   bcm_module_t data, bcm_module_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcPort);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifySrcPort, f_ent, 
        data_port, mask_port);

    return rv;
}

int 
bcm_robo_field_qualify_SrcPortTgid(int unit, bcm_field_entry_t entry,
                  bcm_port_t data, bcm_port_t mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcPortTgid);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifySrcPortTgid, 
                                 f_ent, data, mask);

    return rv;
}


int
bcm_robo_field_qualify_SrcTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_TcpControl(int unit, bcm_field_entry_t entry,
                 uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyTcpControl);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTcpControl, 
                                 f_ent, data, mask);

    return rv;
}

int 
bcm_robo_field_qualify_Ttl(int unit, bcm_field_entry_t entry,
              uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv = BCM_E_NONE;
    uint8 new_data, new_mask;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyTtl);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* Since BCM53115 and BCM53242 only have some values to set. */
    /* The selections are 0, 1, 255 and others */
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit)) {
        new_mask = 3;
        switch (data) {
            case 0:
                new_data = 0;
                break;
            case 1:
                new_data = 1;
                break;
            case 255:
                new_data = 3;
                break;
            default:
                new_data = 2;
        }
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTtl, 
            f_ent, new_data, new_mask);
    } else {
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTtl, 
            f_ent, data, mask);
    }

    return rv;
}

int
bcm_robo_field_qualify_IpInfo(int unit, bcm_field_entry_t entry,
                 uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_qualify_PacketRes(int unit, bcm_field_entry_t entry,
                uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_UserDefined(int unit, bcm_field_entry_t entry,
          bcm_field_udf_t udf_id, uint8 *data, uint8 *mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      udf_data, udf_mask;

    FIELD_IS_INIT(unit);

    if (BCM_FIELD_USER_NUM_UDFS <= udf_id ||
        data == NULL ||
        mask == NULL) {
        FP_ERR(("FP Error: udf_id=%d out-of-range or "
                "data==NULL || mask==NULL\n", udf_id));
        return (BCM_E_PARAM);
    }
    
    f_ent = _robo_field_entry_find(unit, entry, _FP_SLICE_PRIMARY);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    
    udf_data = (*data << 24) | (*(data + 1) << 16) | 
        (*(data + 2) << 8) |(*(data + 3));
    udf_mask = (*mask << 24) | (*(mask + 1) << 16) | 
        (*(mask + 2) << 8) |(*(mask + 3));

    rv = _robo_field_udf_value_set(unit, 
        udf_id, f_ent, &udf_data, &udf_mask);

    return rv;
}

int
bcm_robo_field_qualify_IpProtocolCommon(int unit, bcm_field_entry_t entry,
                                       bcm_field_IpProtocolCommon_t protocol)
{
    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        _field_entry_t      *f_ent;
        uint32 data, mask;
        int    rv;
        
        FIELD_IS_INIT(unit);
    
        if (protocol >= bcmFieldIpProtocolCommonCount) {
            return (BCM_E_PARAM);
        }
    
        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIpProtocolCommon);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }
    
        switch (protocol) {
            case bcmFieldIpProtocolCommonTcp:
                data = FP_BCM53242_L4_FRM_FMT_TCP;
                break;
            case bcmFieldIpProtocolCommonUdp:
                data = FP_BCM53242_L4_FRM_FMT_UDP;
                break;
            case bcmFieldIpProtocolCommonIgmp:
            case bcmFieldIpProtocolCommonIcmp:
                data = FP_BCM53242_L4_FRM_FMT_ICMPIGMP;
                break;
            case bcmFieldIpProtocolCommonIp6Icmp:
            case bcmFieldIpProtocolCommonTcpUdp:
                return BCM_E_UNAVAIL;
                break;
            default:
                data = FP_BCM53242_L4_FRM_FMT_OTHERS;
                break;
        }
        mask = FP_BCM53242_L4_FRM_FMT_MASK;
    
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIpProtocolCommon, 
                                   f_ent, data, mask);
    
        return rv;
    }
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_Snap(int unit, bcm_field_entry_t entry,
                            bcm_field_snap_header_t data, 
                            bcm_field_snap_header_t mask)
{
    if (SOC_IS_ROBO53115(unit)) {
        _field_entry_t      *f_ent;
        int     rv;
        uint32              ref_data[2], ref_mask[2];

        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySnap);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }

        ref_data[1] = (data.org_code & 0x00ff0000) >> 16;
        ref_data[0] = (data.org_code & 0x0000ffff) << 16 | data.type;
 
        ref_mask[1] = (mask.org_code & 0x00ff0000) >> 16;
        ref_mask[0] = (mask.org_code & 0x0000ffff) << 16 | mask.type;

        rv = _robo_field_qual_value_set(unit, bcmFieldQualifySnap, 
            f_ent, ref_data, ref_mask);

        return rv;
    }  
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_Llc(int unit, bcm_field_entry_t entry,
                           bcm_field_llc_header_t data, 
                           bcm_field_llc_header_t mask)
{
    if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)||
        SOC_IS_ROBO5395(unit)) {
        _field_entry_t      *f_ent;
        int     rv = BCM_E_NONE;
        uint32 ref_data, ref_mask;

        f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyLlc);
        if (f_ent == NULL) {
            return BCM_E_NOT_FOUND;
        }

        /* Set LLC header */
        ref_data = (data.dsap << 16) | (data.ssap << 8) | data.control; 
        ref_mask = (mask.dsap << 16) | (mask.ssap << 8) | mask.control; 
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_control_set)
            (unit, DRV_CFP_LLC_ENCAP, 0, ref_data & ref_mask));

        /* Qualify LLC packet format */
        ref_data = 1;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_IEEE_LLC, 
                &f_ent->drv_entry, &ref_data));
        
        ref_mask = 1;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_field_set)
            (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_IEEE_LLC, 
                &f_ent->drv_entry, &ref_mask));

        return rv;
    }  
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_OuterTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_PortClass(int unit, bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IpAuth(int unit, bcm_field_entry_t entry,
                 uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIpAuth);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIpAuth, 
                                 f_ent, data, mask);

    return rv;
}




/*
 * Function: bcm_robo_field_range_create
 *    
 * Purpose:
 *     Allocate a range checker and set its parameters.
 *
 * Parameters:
 *     unit   - BCM device number
 *     range  - (OUT) Range check ID, will not be zero
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */

int
bcm_robo_field_range_create(int unit, bcm_field_range_t *range,
               uint32 flags, bcm_l4_port_t min, bcm_l4_port_t max)
{
    _field_control_t    *fc;
    int                 retval;

    FP_VVERB(("BEGIN bcm_robo_field_range_create(unit=%d, range->0x%x, ", unit,
              range));
    FP_VVERB(("flags=0x%08x, min=0x%x, max=0x%x)\n", unit, range, flags, min,
               max));

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    for (;;) {
        retval = bcm_field_range_create_id(unit, fc->range_id, flags,
                                               min, max);

        if (retval != BCM_E_EXISTS) {
            break;
        }

        if (++fc->range_id == 0) {
            fc->range_id = 1;
        }
    }

    if (BCM_SUCCESS(retval)) {
        *range = fc->range_id;

        if (++fc->range_id == 0) {
            fc->range_id = 1;
        }
    }

    sal_mutex_give(fc->fc_lock);

    return retval;
}

/*
 * Function: bcm_robo_field_range_group_create
 *    
 * Purpose:
 *     Allocate a range checker and set its parameters.
 *
 * Parameters:
 *     unit   - BCM device number
 *     range  - (OUT) Range check ID, will not be zero
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *     group  - L3 interface group number
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid L3 interface group number
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */

int
bcm_robo_field_range_group_create(int unit,
                                 bcm_field_range_t *range,
                                 uint32 flags,
                                 bcm_l4_port_t min,
                                 bcm_l4_port_t max,
                                 bcm_if_group_t group)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_range_create_id
 *    
 * Purpose:
 *     Allocate a range checker and set its parameters.
 *
 * Parameters:
 *     unit   - BCM device number
 *     range  - Range check ID to use
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_EXISTS    - Range ID already in use
 *     BCM_E_RESOURCE  - Hardware range checkers all in use
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_range_create_id(int unit, bcm_field_range_t range,
              uint32 flags, bcm_l4_port_t min, bcm_l4_port_t max)
{
    _field_control_t    *fc;
    _field_range_t      *fr;
    int                 hw_index  = -1; /* Free/matching rang index.*/
    uint32            ranger_num = 0; 
    uint32            hw_used = 0;     /* Used indexes map.        */
    uint32            hw_used_vlan = 0, hw_used_l4port = 0;
    int    rv = BCM_E_NONE;
    uint32 type = 0;

    FP_VVERB(("BEGIN bcm_robo_field_range_create_id(unit=%d, range=%d, ", unit,
              range));
    FP_VVERB(("flags=0x%08x, min=0x%x, max=0x%x)\n", flags, min, max));

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    /* Flags checking */
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
    /* Determine the ranger for bcm5348/5347 only (src port 0~1023) */
        if (flags & BCM_FIELD_RANGE_SRCPORT && 
                flags & BCM_FIELD_RANGE_DSTPORT) {
            FP_ERR(("FP Error: Can't select both source and destination.\n"));
            return (BCM_E_PARAM);
        }

        if(flags & ~BCM_FIELD_RANGE_SRCPORT) {
            FP_ERR(("FP Error: Can't select destination port.\n"));
            return BCM_E_UNAVAIL;
        }
        if (min != 0 || max != 1023) {
            return BCM_E_UNAVAIL;
        }
    } else if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
        /* BCM5395 didn't have field ranger */
        return BCM_E_UNAVAIL;
    }  else if (SOC_IS_ROBO53242(unit) || 
        SOC_IS_ROBO53262(unit)) {
        if ((flags & BCM_FIELD_RANGE_INVERT)) {
            return BCM_E_UNAVAIL;
        }
        if (flags & BCM_FIELD_RANGE_SRCPORT && 
            flags & BCM_FIELD_RANGE_DSTPORT) {
            FP_ERR(("FP Error: Can't select both source and destination.\n"));
            return (BCM_E_PARAM);
        }

        /*
         * Although the BCM53242/BCM53262 has 4 L4 port range checkers,
         * it also has the qualifier, L4SRCLESS1024(L4 src port less than 1024).
         * So we define hw_index of this qualifier is 4(5th) as its index.
         */
         if (min == 0 && max == 1023) {
            hw_index = 9;
        }
    }

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    /* Search existing ranges */
    for (fr = fc->ranges; fr != NULL; fr = fr->next) {
        if (fr->rid == range) {
            sal_mutex_give(fc->fc_lock);
            FP_ERR(("FP Error: range_id=%d already exists.\n", range));
            return (BCM_E_EXISTS);
        }

        /* Build maps of hardware in-use indexes. */
        hw_used |= (1 << fr->hw_index);

        /* Found an exisiting match so use it. */
        if (flags == fr->flags && min == fr->min && max == fr->max) {
            hw_index = fr->hw_index;
        }

        if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
            if (fr->flags & BCM_FIELD_RANGE_OUTER_VLAN) {
                hw_used_vlan ++;
            }

            if ((fr->flags & BCM_FIELD_RANGE_SRCPORT) || 
               (fr->flags & BCM_FIELD_RANGE_DSTPORT)) {
                hw_used_l4port ++;
            }
        }
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        if (flags & BCM_FIELD_RANGE_OUTER_VLAN) { 
            if (hw_used_vlan >= 4) {
                sal_mutex_give(fc->fc_lock);
                FP_ERR(("FP Error: No hardware range checkers left.\n", range));
                return (BCM_E_RESOURCE);
            }
        }

        if ((flags & BCM_FIELD_RANGE_SRCPORT) || 
           (flags & BCM_FIELD_RANGE_DSTPORT)) {
            if (hw_used_l4port >= 4) {
                sal_mutex_give(fc->fc_lock);
                FP_ERR(("FP Error: No hardware range checkers left.\n", range));
                return (BCM_E_RESOURCE);
            }
        }
    }

    /* get ranger num */
    if (SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit) ||
        SOC_IS_ROBO5348(unit) ||
        SOC_IS_ROBO5347(unit)) {
        rv= (DRV_SERVICES(unit)->dev_prop_get
            (unit, DRV_DEV_PROP_CFP_RNG_NUM, &ranger_num));
        if (rv < 0) {
            sal_mutex_give(fc->fc_lock);
            return rv;
        }
    }

    /* If no match found, allocate a new hardware index. */
    if (hw_index < 0) {
        int start_idx = 0;
        if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
            if (flags & BCM_FIELD_RANGE_OUTER_VLAN) { 
                start_idx = 4;
            }
        }
        for (hw_index = start_idx; hw_index < ranger_num+start_idx; hw_index++) {
            /* Found an unused FB style range checker */
            if ((hw_used & (1 << hw_index)) == 0) {
                break;
            }
        }

        /* No hardware indexes left. */
        if (hw_index == (ranger_num+start_idx)) {
            sal_mutex_give(fc->fc_lock);
            FP_ERR(("FP Error: No hardware range checkers left.\n", range));
            return (BCM_E_RESOURCE);
        }
    }
    
    /* Create a new range entry for the list */
    if ((fr = sal_alloc(sizeof (*fr), "fp_range")) == NULL) {
        sal_mutex_give(fc->fc_lock);
        return BCM_E_MEMORY;
    }

    fr->flags    = flags;
    fr->rid      = range;
    fr->min      = min;
    fr->max      = max;
    fr->hw_index = hw_index;

    if ((SOC_IS_ROBO53242(unit) ||
        SOC_IS_ROBO53262(unit)) && (hw_index != 9)) {
        uint32 idx;

        /* parameter checking */
        rv = (DRV_SERVICES(unit)->cfp_ranger
            (unit, flags, min, max));
        if (rv < 0) {
            sal_mutex_give(fc->fc_lock);
            sal_free(fr);
            return rv;
        }
        if (flags & BCM_FIELD_RANGE_SRCPORT) {
            type = DRV_CFP_RANGE_SRCPORT;
            idx= hw_index;
        } else if (flags & BCM_FIELD_RANGE_DSTPORT){
            type = DRV_CFP_RANGE_DSTPORT;
            idx= hw_index;
        } else {
            type = DRV_CFP_RANGE_VLAN;
            idx = hw_index -4;
        }

        /* ranger _set */
        rv = (DRV_SERVICES(unit)->cfp_range_set
            (unit, type, idx, min, max));
        if (rv < 0) {
            sal_mutex_give(fc->fc_lock);
            sal_free(fr);
            return rv;
        }
    }
    
    /* Add to list of range checkers. */
    fr->next = fc->ranges;
    fc->ranges = fr;

    sal_mutex_give(fc->fc_lock);

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_range_group_create_id
 *    
 * Purpose:
 *     Allocate an interface group range checker and set its parameters.
 *
 * Parameters:
 *     unit   - BCM device number
 *     range  - Range check ID to use
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *     group  - L3 interface group number
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_EXISTS    - Range ID already in use
 *     BCM_E_RESOURCE  - Hardware range checkers all in use
 *     BCM_E_PARAM     - Invalid L3 interface group number
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */

int
bcm_robo_field_range_group_create_id(int unit,
                                    bcm_field_range_t range,
                                    uint32 flags,
                                    bcm_l4_port_t min,
                                    bcm_l4_port_t max,
                                    bcm_if_group_t group)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function: bcm_robo_field_range_destroy
 *
 * Purpose:
 *     Deallocate a range
 *
 * Parameters:
 *     unit  - BCM device number
 *     range  - Range check ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Invalid range ID
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_range_destroy(int unit, bcm_field_range_t range)
{
    _field_control_t    *fc;
    _field_range_t      *fr, *fr_prev; 

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    /* Find matching entry */
    fr_prev = NULL;
    for (fr = fc->ranges; fr != NULL; fr = fr->next) {
        if (fr->rid == range) {
            break;
        }
        fr_prev = fr;
    }

    if (fr == NULL) {
        sal_mutex_give(fc->fc_lock);
        FP_ERR(("FP Error: range=%d not found in unit=%d.\n", range, unit));
        return BCM_E_NOT_FOUND;
    }


    /* Remove from list and free */
    if (fr_prev == NULL) {
        fc->ranges = fr->next;
    } else {
        fr_prev->next = fr->next;
    }

    sal_mutex_give(fc->fc_lock);

    sal_free(fr);

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_range_get
 *    
 * Purpose:
 *     Get the TCP/UDP port for a range
 *
 * Parameters:
 *     unit  - BCM device number
 *     range  - Range check ID
 *     flags - (OUT) Current range checker flags
 *     min   - (OUT) Lower bounds of range to be checked
 *     max   - (OUT) Upper bounds of range to be checked
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Invalid range ID
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_range_get(int unit, bcm_field_range_t range,
            uint32 *flags, bcm_l4_port_t *min, bcm_l4_port_t *max)
{
    _field_control_t    *fc;
    _field_range_t      *fr;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    for (fr = fc->ranges; fr != NULL; fr = fr->next) {
        if (fr->rid == range) {
            break;
        }
    }

    if (fr == NULL) {
        sal_mutex_give(fc->fc_lock);
        FP_ERR(("FP Error: range=%d not found in unit=%d.\n", range, unit));
        return BCM_E_NOT_FOUND;
    }

    if (flags != NULL) {
        *flags = fr->flags;
    }

    if (min != NULL) {
        *min = fr->min;
    }

    if (max != NULL) {
        *max = fr->max;
    }

    sal_mutex_give(fc->fc_lock);

    return BCM_E_NONE;
}


/* bcm_field_show not dispatchable */

/*
 * Function: bcm_robo_field_status_get
 *
 * Purpose:
 *     Retrieve status information
 *
 * Parameters:
 *     unit   - BCM device number
 *     status - (OUT) Status for field module
 *
 * Returns:
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_PARAM   - status pointing to NULL
 *     BCM_E_NONE    - Success
 */
int 
bcm_robo_field_status_get(int unit, bcm_field_status_t *status)
{
     _field_control_t    *fc;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    if (status == NULL) {
       FP_ERR(("FP Error: 'status' == NULL\n"));
        return BCM_E_PARAM;
    }

    *status = fc->field_status;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_udf_spec_set
 *
 * Purpose:
 *     Define a user-defined (1-byte or 4 -byte) field 
 *
 * Parameters:
 *     unit      - BCM device number
 *     udf_spec  - Pointer to a user-defined field (UDF) definition structure
 *     flags     - One or more of BCM_FIELD_USER_* flags
 *     offset    - Word Offset (0..79)
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid flag or out-of-range offset
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int 
bcm_robo_field_udf_spec_set(int unit,
               bcm_field_udf_spec_t *udf_spec,
               uint32 flags,
               uint32 offset)
{
    uint32 test_flag = 0, offset_max;
    uint16 dev_id= 0;
    uint8 rev_id = 0;

    FIELD_IS_INIT(unit);

    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
        (unit, DRV_DEV_PROP_CFP_UDFS_OFFSET_MAX, &offset_max));
    
    if ((NULL == udf_spec) || (offset > offset_max)) {
        return (BCM_E_PARAM); 
    }
    
    if (flags & ~BCM_FIELD_USER_OFFSET_BASE_MASK) {
        FP_ERR(("FP Error: flags = 0x%x didn't support by UDF\n", flags));
        return BCM_E_PARAM;
    }

    /* select available udf field */
    udf_spec->offset[0] = offset;
    /* The offset_base is restored is offset[1] */
    /* Check type of UDF offset of BCM53115/53242/53262. Other chips don't care the parameter. */
    if (SOC_IS_ROBO53115(unit)) {
        test_flag = BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME | \
                    BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR | \
                    BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR;

        if (!(flags & test_flag)) {
            return BCM_E_PARAM;
        }
    } else if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        test_flag = BCM_FIELD_USER_OFFSET_BASE_END_OF_TAG | \
                    BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR | \
                    BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR;

        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (((dev_id == BCM53242_DEVICE_ID) && (rev_id == BCM53242_B0_REV_ID)) || 
            ((dev_id == BCM53262_DEVICE_ID) && (rev_id == BCM53262_B0_REV_ID)) || 
            ((dev_id == BCM53242_DEVICE_ID) && (rev_id == BCM53242_B1_REV_ID)) || 
            ((dev_id == BCM53262_DEVICE_ID) && (rev_id == BCM53262_B1_REV_ID))) {
            test_flag |= BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME;
        }

        if (!(flags & test_flag)) {
            return BCM_E_PARAM;
        }
    }

    udf_spec->offset[1] = flags >> 16;
    return BCM_E_NONE; 
}

int 
bcm_robo_field_udf_spec_get(int unit,
               bcm_field_udf_spec_t *udf_spec,
               uint32 *flags,
               uint32 *offset)
{
    FIELD_IS_INIT(unit);

    /* select available udf field */
    *offset = udf_spec->offset[0];
    *flags = udf_spec->offset[1] << 16;

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_qset_add_udf
 *
 * Purpose:
 *     Add a UDF definition to a qset 
 *
 * Parameters:
 *     unit     - BCM device number
 *     qset     - Pointer to a qset structure
 *     udf_id   - UDF ID to add to the qset
 *
 * Returns:
 *     BCM_E_INIT     - BCM unit not initialized 
 *     BCM_E_RESOURCE - Cannot add UDF (all UDFs used)
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */
int 
bcm_robo_field_qset_add_udf(int unit,
               bcm_field_qset_t *qset,
               bcm_field_udf_t udf_id)

{
    _field_control_t *fc;
    uint32           udf_num;

    FIELD_IS_INIT(unit);
    fc = _robo_field_control_find(unit);

    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num));

    if ( (qset == NULL) || 
         (udf_id >= udf_num) ||
         (!fc->udf[udf_id].valid) ) {
        return BCM_E_NOT_FOUND;
    }

    SHR_BITSET(qset->udf_map, udf_id); /* in use */

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_udf_create
 *
 * Purpose:
 *     Create a virtual UDF identifier, based on the requested udf_spec.
 *     On success, returns a udf_id.
 *
 * Parameters:
 *     unit      - BCM device number
 *     udf_spec  - Pointer to a user-defined field (UDF) definition structure
 *     udf_id    - Pointer to a UDF ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - NULL pointer parameter
 *     BCM_E_RESOURCE  - Cannot create new UDF (full)
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_udf_create(int unit,
                         bcm_field_udf_spec_t *udf_spec,
                         bcm_field_udf_t *udf_id)
{
    _field_control_t    *fc;
    bcm_field_udf_t     tuid, free_udf_id;
    int                 rv;
    uint32              udf_num;

    FIELD_IS_INIT(unit);

    if (udf_spec == NULL || udf_id == NULL) {
        return BCM_E_PARAM;
    }

    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    /* First check for any currently configured UDFs to share */
    free_udf_id = BCM_FIELD_USER_NUM_UDFS;
    rv = DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num);
    if (BCM_FAILURE(rv)) {
        sal_mutex_give(fc->fc_lock);
        return rv;
    }
    
    for (tuid = 0; tuid < udf_num; tuid++) {
        if (!fc->udf[tuid].valid) {
            if (BCM_FIELD_USER_NUM_UDFS == free_udf_id) {
                free_udf_id = tuid;
            }
            continue;
        }
        rv = bcm_field_udf_create_id(unit, udf_spec, tuid);
        if (BCM_SUCCESS(rv)) {
            sal_mutex_give(fc->fc_lock);
            *udf_id = tuid;
            return BCM_E_NONE;
        }
        else if (rv != BCM_E_EXISTS) {
            sal_mutex_give(fc->fc_lock);
            return rv;
        }
    }

    /* No UDFs to share; available UDF? */
    if (BCM_FIELD_USER_NUM_UDFS == free_udf_id) {
        rv = BCM_E_RESOURCE;
    }
    else {
        *udf_id = free_udf_id;
        rv = bcm_field_udf_create_id(unit, udf_spec, *udf_id);
    }

    sal_mutex_give(fc->fc_lock);
    return rv;
}

/*
 * Function: bcm_robo_field_udf_create_id
 * 
 * Purpose:
 *     Configure a virtual UDF, using the designated udf_id.
 *
 * Parameters:
 *     unit      - BCM device number
 *     udf_spec  - Pointer to a user-defined field (UDF) definition structure
 *     udf_id    - UDF ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid udf_id or NULL pointer parameter
 *     BCM_E_EXISTS    - Requested UDF is already configured 
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_udf_create_id(int unit,
                            bcm_field_udf_spec_t *udf_spec,
                            bcm_field_udf_t udf_id)
{
    _field_control_t    *fc;
    int                 rv = BCM_E_UNAVAIL;
    bcm_port_t  port;
    uint32      udf_num;
    uint32  offset, offset1, offset_base;

    FIELD_IS_INIT(unit);

    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num));
    if ( (udf_spec == NULL) ||
         (udf_id >= udf_num) ) {
        return BCM_E_PARAM;
    }

    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    if (fc->udf[udf_id].valid) {
        bcm_field_udf_spec_t current_udf_spec;
        sal_memset(&current_udf_spec, 0, sizeof(bcm_field_udf_spec_t));
        rv = (DRV_SERVICES(unit)->cfp_udf_get)
            (unit, 0, udf_id, &offset, &offset_base);
        current_udf_spec.offset[0] = offset;
        offset = 0;
        switch(offset_base) {
            case DRV_CFP_UDF_OFFSET_BASE_START_OF_FRAME:
                offset |= BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME;
                break;
            case DRV_CFP_UDF_OFFSET_BASE_END_OF_TAG:
                offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_TAG;
                break;
            case DRV_CFP_UDF_OFFSET_BASE_END_OF_L2_HDR:
                offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR;
                break;
            case DRV_CFP_UDF_OFFSET_BASE_END_OF_L3_HDR:
                offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR;
                break;
        }

        current_udf_spec.offset[1] = offset >> 16;

        sal_mutex_give(fc->fc_lock);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        else if (!sal_memcmp(udf_spec, &current_udf_spec, 
                             sizeof(bcm_field_udf_spec_t))) {
            /* UDF already configured as requested */
            return BCM_E_NONE;
        }
        else {
            /* UDF already configured, but differently */
            return BCM_E_EXISTS;
        }
    }

    /* UDF is currently unused; go ahead and take it */
    rv = BCM_E_UNAVAIL;
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        offset = udf_spec->offset[0];
        offset1 = udf_spec->offset[1] << 16;
        if (offset1 & BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME) {
                offset_base = DRV_CFP_UDF_OFFSET_BASE_START_OF_FRAME;
        } else if (offset1 & BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR) {
            offset_base = DRV_CFP_UDF_OFFSET_BASE_END_OF_L2_HDR;
        } else if (offset1 & BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR) {
            offset_base = DRV_CFP_UDF_OFFSET_BASE_END_OF_L3_HDR;
        } else if (offset1 & BCM_FIELD_USER_OFFSET_BASE_END_OF_TAG) {
                offset_base = DRV_CFP_UDF_OFFSET_BASE_END_OF_TAG;
        } else {
            /* Assign the default UDF offset base flags */
            offset_base = DRV_CFP_UDF_OFFSET_BASE_START_OF_FRAME;
            /* 
             * Since the BCM53242/BCM53262 A0 didn't support the offset base 
             * from the start of frame, it need to be changed for those chips.
             */
            if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
                offset_base = DRV_CFP_UDF_OFFSET_BASE_END_OF_TAG;
            }
        }

        rv = (DRV_SERVICES(unit)->cfp_udf_set)
            (unit, port, udf_id, offset, offset_base);
    }

    if (BCM_FAILURE(rv)) {
        sal_mutex_give(fc->fc_lock);
        return rv;
    }

    fc->udf[udf_id].valid = 1;
    fc->udf[udf_id].use_count = 0;
 
    sal_mutex_give(fc->fc_lock);

    return BCM_E_NONE;
}


/*
 * Function: bcm_robo_field_udf_ethertype_set
 *
 * Purpose:
 *     Set the UDF Ethertype match value.
 *
 * Parameters:
 *     unit      - BCM device number
 *     index     - 0->7, Ethertype match index
 *     frametype - BCM_PORT_FRAMETYPE_xxx, 0 indicates disable entry
 *     ethertype - Ethertype value to match
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid index or frametype parameter
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_udf_ethertype_set(int unit, int index,
                                bcm_port_frametype_t frametype,
                                bcm_port_ethertype_t ethertype) 
{
    return BCM_E_UNAVAIL;
}
/*
 * Function: bcm_robo_field_udf_ethertype_get
 *
 * Purpose:
 *     Get the UDF Ethertype match fields.
 *
 * Parameters:
 *     unit      - BCM device number
 *     index     - 0->7, Ethertype match index
 *     frametype - (OUT) BCM_PORT_FRAMETYPE_xxx
 *     ethertype - (OUT) Ethertype value to match
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid index or NULL pointer parameter
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_udf_ethertype_get(int unit, int index,
                                bcm_port_frametype_t *frametype,
                                bcm_port_ethertype_t *ethertype)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_udf_ipprotocol_set
 *
 * Purpose:
 *     Set the UDF IP Protocol match values.
 *
 * Parameters:
 *     unit      - BCM device number
 *     index     - Either 0 or 1, which of the two IpProtocol matches
 *     flags     - BCM_FIELD_USER_IP4/6 flags, 0 means clear entry
 *     proto     - protocol value to match
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid udf_id or NULL pointer parameter
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_udf_ipprotocol_set(int unit, int index, uint32 flags, uint8 proto)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_udf_ipprotocol_get
 *
 * Purpose:
 *     Get the state of the UDF IP Protocol match fields.
 *
 * Parameters:
 *     unit      - BCM device number
 *     index     - Either 0 or 1, which of the two IpProtocol matches
 *     flags     - (OUT) BCM_FIELD_USER_IP4/6 flags, 0 means match disabled 
 *     proto     - (OUT) protocol value to match
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid index or NULL pointer parameter
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_udf_ipprotocol_get(int unit, int index, uint32 *flags,
                                 uint8 *proto)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_udf_get
 *
 * Purpose:
 *     Retrieve the specified virtual UDF settings, returning them to the
 *     designated udf_spec.
 *
 * Parameters:
 *     unit      - BCM device number
 *     udf_spec  - Pointer to a user-defined field (UDF) definition structure
 *     udf_id    - UDF ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid udf_id or NULL pointer parameter
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */
int
bcm_robo_field_udf_get(int unit,
                      bcm_field_udf_spec_t *udf_spec,
                      bcm_field_udf_t udf_id)
{
    _field_control_t    *fc;
    int                 rv = BCM_E_UNAVAIL;
    uint32              udf_num;
    uint32      offset, offset_base;

    FIELD_IS_INIT(unit);

    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num));
    if ( (udf_spec == NULL) ||
         (udf_id >= udf_num) ) {
        return BCM_E_PARAM;
    }

    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    if (!fc->udf[udf_id].valid) {
        sal_mutex_give(fc->fc_lock);
        FP_ERR(("FP Error: udf=%d not configured in unit=%d.\n", udf_id, unit));
        return BCM_E_NOT_FOUND;
    }

    rv = (DRV_SERVICES(unit)->cfp_udf_get)
        (unit, 0, udf_id, &offset, &offset_base);
    udf_spec->offset[0] = offset;
    offset = 0;
    switch(offset_base) {
        case DRV_CFP_UDF_OFFSET_BASE_START_OF_FRAME:
            offset |= BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME;
            break;
        case DRV_CFP_UDF_OFFSET_BASE_END_OF_TAG:
            offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_TAG;
            break;
        case DRV_CFP_UDF_OFFSET_BASE_END_OF_L2_HDR:
            offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR;
            break;
        case DRV_CFP_UDF_OFFSET_BASE_END_OF_L3_HDR:
            offset |= BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR;
            break;
    }
    udf_spec->offset[1] = offset >> 16;
    sal_mutex_give(fc->fc_lock);

    return rv;
}


/*
 * Function: bcm_robo_field_udf_destroy
 *
 * Purpose:
 *     Destroy the specified virtual UDF, removing it from hardware.
 *     
 * Parameters:
 *     unit      - BCM device number
 *     udf_id    - UDF ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid udf_id
 *     BCM_E_EXISTS    - Requested UDF is being used
 *     BCM_E_UNAVAIL   - Feature unavailable on unit.
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */

int
bcm_robo_field_udf_destroy(int unit,
                          bcm_field_udf_t udf_id)
{
    _field_control_t     *fc;
    uint32               udf_num;

    FIELD_IS_INIT(unit);

    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num));
    if (udf_id >= udf_num) {
        FP_ERR(("FP Error: udf=%d out-of-range.\n", udf_id));
        return BCM_E_PARAM;
    }

    fc = _robo_field_control_find(unit);

    sal_mutex_take(fc->fc_lock, sal_mutex_FOREVER);

    if (!fc->udf[udf_id].valid) {
        /* UDF already destroyed */
        sal_mutex_give(fc->fc_lock);
        FP_ERR(("FP Error: udf=%d not configured in unit=%d.\n", udf_id, unit));
        return BCM_E_PARAM;
    }
    if (fc->udf[udf_id].use_count) {
        /* UDF still in use; do not destroy */
        sal_mutex_give(fc->fc_lock);
        return BCM_E_EXISTS;
    }

    fc->udf[udf_id].valid = 0;

    sal_mutex_give(fc->fc_lock);

    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_group_enable_set
 *    
 * Purpose:
 *     Enable/disable packet lookup on a group.
 *
 * Parameters:
 *     unit   - BCM device number
 *     enable - lookup enable!=0/disable==0 state of group
 *     group  - Field group ID
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_UNAVAIL   - BCM device does not have enable/disable feature
 */
int
bcm_robo_field_group_enable_set(int unit, bcm_field_group_t group, int enable)
{
    _field_group_t      *fg;
    bcm_port_t  port;
    uint32  temp;

    FIELD_IS_INIT(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: Group ID=%d not found.\n", group));
        return BCM_E_NOT_FOUND;
    }
    
    if(soc_feature(unit, soc_feature_field_slice_enable)) {

        /* Enable/disable slice. */
        port = 0;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_control_get)
                (unit, DRV_CFP_SLICE_SELECT, port, &temp));
        if (enable) {
            temp |= (0x1 << fg->slices[0].sel_codes.fpf);
        } else {
            temp &= ~(0x1 << fg->slices[0].sel_codes.fpf);
        }
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            BCM_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->cfp_control_set)(unit, 
                DRV_CFP_SLICE_SELECT, port, temp));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_field_group_enable_get
 *    
 * Purpose:
 *     Get the lookup enable/disable state of a group
 *
 * Parameters:
 *     unit   - BCM device number
 *     group  - Field group ID
 *     enable - (OUT) lookup enable/disable state of group
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found on unit
 *     BCM_E_UNAVAIL   - BCM device does not have enable/disable feature
 */
int
bcm_robo_field_group_enable_get(int unit, bcm_field_group_t group, int *enable)
{
    _field_group_t      *fg;
    uint32  temp;

    FIELD_IS_INIT(unit);

    fg = _robo_field_group_find(unit, group);
    if (fg == NULL) {
        FP_ERR(("FP Error: Group ID=%d not found.\n", group));
        return BCM_E_NOT_FOUND;
    }
        
    if(soc_feature(unit, soc_feature_field_slice_enable)) {
        
        temp = 0;
        BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->cfp_control_get)
                (unit, DRV_CFP_SLICE_SELECT, 0, &temp));
        if (temp & (0x1 << fg->slices[0].sel_codes.fpf)) {
            *enable = TRUE;
        } else {
            *enable = FALSE;
        }
    }
    return BCM_E_NONE;
}

int
bcm_robo_field_qualify_HiGig(int unit, bcm_field_entry_t entry,
                            uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstHiGig
 * Purpose:
 *      Qualify on HiGig destination packets.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstHiGig(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstHiGig_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstHiGig
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstHiGig_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}


int
bcm_robo_field_qualify_Stage(int unit, bcm_field_entry_t entry,
                            bcm_field_stage_t stage)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_SrcIpEqualDstIp(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifySrcIpEqualDstIp);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (flag) {
        data = 1;;
    } else {
        data = 0;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifySrcIpEqualDstIp, 
                                 f_ent, data, 1);

    return rv;
}

int 
bcm_robo_field_qualify_EqualL4Port(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyEqualL4Port);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (flag) {
        data = 1;;
    } else {
        data = 0;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyEqualL4Port, 
                                 f_ent, data, 1);

    return rv;
}

int 
bcm_robo_field_qualify_TcpSequenceZero(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyTcpSequenceZero);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (flag) {
        data = 1;;
    } else {
        data = 0;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTcpSequenceZero, 
                                 f_ent, data, 1);

    return rv;
}

int
bcm_robo_field_qualify_TcpHeaderSize(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint8 data_val, mask_val, tmp;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyTcpHeaderSize);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        if (mask) {
            data_val = 1;
            mask_val = 1;
            /* Set desired TCP header size */
            tmp = data & mask;
            rv = (DRV_SERVICES(unit)->cfp_range_set
                    (unit, DRV_CFP_RANGE_TCP_HEADER_LEN, 0, tmp, 0));
            if (rv < 0) {
                return rv;
            }
        } else {
            data_val = 0;
            mask_val = 0;
        }
    } else {
        data_val = data;
        mask_val = mask;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTcpHeaderSize, 
                                 f_ent, data_val, mask_val);

    return rv;
}

int 
bcm_robo_field_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                                  bcm_field_IpFrag_t frag_info)
{
     _field_entry_t      *f_ent;
    int                 rv = BCM_E_NONE;
    uint32 frag_data, frag_mask, non_first_data, non_first_mask;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIpFrag);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    switch (frag_info) {
        case bcmFieldIpFragNon:
            frag_data = 0;
            frag_mask = 1;
            non_first_mask = 0;
            break;
        case bcmFieldIpFragFirst:
            frag_data = 1;
            frag_mask = 1;
            non_first_data = 0;
            non_first_mask = 1;
            break;
        case bcmFieldIpFragNonOrFirst:
            frag_mask = 0;
            non_first_data = 0;
            non_first_mask = 1;
            break;
        case bcmFieldIpFragNotFirst:
            frag_data = 1;
            frag_mask = 1;
            non_first_data = 1;
            non_first_mask = 1;
            break;
        default:
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_IP_FRAG, 
            &f_ent->drv_entry, &frag_data));
    
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_IP_FRAG, 
            &f_ent->drv_entry, &frag_mask));

    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM, DRV_CFP_FIELD_IP_NON_FIRST_FRAG, 
            &f_ent->drv_entry, &non_first_data));
    
    BCM_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->cfp_field_set)
        (unit, DRV_CFP_RAM_TCAM_MASK, DRV_CFP_FIELD_IP_NON_FIRST_FRAG, 
            &f_ent->drv_entry, &non_first_mask));

    return rv;
    
}

int 
bcm_robo_field_qualify_L3Routable(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{    
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_qualify_Tos(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyTos);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyTos, f_ent, data, mask);

    return rv;
}
int bcm_robo_field_qualify_LookupClass0(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_qualify_Vrf(int unit, bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_qualify_ExtensionHeaderType(int unit, 
                                               bcm_field_entry_t entry,
                                               uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_L4Ports
 * Description : qualify on the 4 bytes after L3 header.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 4 bytes after L3 header are present. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_L4Ports(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_MirrorCopy
 * Description : qualify on the mirrored packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not Mirrored/Mirrored packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_MirrorCopy(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}


/* Function    : bcm_robo_field_qualify_TunnelTerminated
 * Description : qualify on the tunnel terminated packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not Tunneled/Tunnel Terminated packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_TunnelTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_MplsTerminated
 * Description : qualify on the mpls terminated packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not mpls terminated/mpls Terminated packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_MplsTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}


int bcm_robo_field_qualify_ExtensionHeaderSubCode(int unit, 
                                                  bcm_field_entry_t entry,
                                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_OutPorts(int unit, bcm_field_entry_t entry,
                               bcm_pbmp_t data, bcm_pbmp_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerSrcIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerSrcIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerSrcIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDSCP(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerIpProtocol(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}


int bcm_robo_field_qualify_InnerIpFrag(int unit, bcm_field_entry_t entry,
                                  bcm_field_IpFrag_t frag_info)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerTtl(int unit, bcm_field_entry_t entry,
                          uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}


int 
bcm_robo_field_qualify_DosAttack(int unit, bcm_field_entry_t entry, 
                                uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IpmcStarGroupHit(int unit, bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3DestRouteHit(int unit, bcm_field_entry_t entry, 
                                     uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3DestHostHit(int unit, bcm_field_entry_t entry, 
                                     uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3SrcHostHit(int unit, bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2CacheHit(int unit, bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2StationMove(int unit, bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2DestHit(int unit, bcm_field_entry_t entry, 
                                uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2SrcStatic(int unit, bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2SrcHit(int unit, bcm_field_entry_t entry, 
                               uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IngressStpState(int unit,bcm_field_entry_t entry, 
                                      uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_ForwardingVlanValid(int unit, bcm_field_entry_t entry, 
                                          uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_VlanTranslationHit(int unit, bcm_field_entry_t entry, 
                                         uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_BigIcmpCheck(int unit, bcm_field_entry_t entry,
                                         uint32 flag, uint32 size)
{
    _field_entry_t      *f_ent;
    int                 rv;
    uint32      data;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyBigIcmpCheck);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit)|| SOC_IS_ROBO53262(unit)) {
        if (flag) {
            data = 1;
            /* icmp size set */
            rv = (DRV_SERVICES(unit)->cfp_range_set
                    (unit, DRV_CFP_RANGE_BIG_ICMP, 0, size, 0));
            if (rv < 0) {
                return rv;
            }
        } else {
            data = 0;
        }
    
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyBigIcmpCheck, 
                                     f_ent, data, 1);

    } else {
        rv = BCM_E_UNAVAIL;
    }
    return rv;
}

int 
bcm_robo_field_qualify_IcmpTypeCode(int unit, bcm_field_entry_t entry,
                                         uint16 data,uint16 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIcmpTypeCode);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit)|| SOC_IS_ROBO53262(unit)) {
   
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIcmpTypeCode, 
                                     f_ent, data, mask);

    } else {
        rv = BCM_E_UNAVAIL;
    }
    return rv;
}

int 
bcm_robo_field_qualify_IgmpTypeMaxRespTime(int unit, bcm_field_entry_t entry,
                                         uint16 data,uint16 mask)
{
    _field_entry_t      *f_ent;
    int                 rv;

    FIELD_IS_INIT(unit);

    f_ent = _field_entry_qual_find(unit, entry, bcmFieldQualifyIgmpTypeMaxRespTime);
    if (f_ent == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IS_ROBO53242(unit)|| SOC_IS_ROBO53262(unit)) {
   
        rv = _robo_field_qual_value32_set(unit, bcmFieldQualifyIgmpTypeMaxRespTime, 
                                     f_ent, data, mask);

    } else {
        rv = BCM_E_UNAVAIL;
    }
    return rv;
}

int
bcm_robo_field_qualify_InnerL4DstPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerL4SrcPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIpType(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t type)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIpProtocolCommon(int unit, bcm_field_entry_t entry,
                                       bcm_field_IpProtocolCommon_t protocol)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIp6FlowLabel(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_DstL3Egress(int unit, 
                                   bcm_field_entry_t entry, 
                                   bcm_if_t if_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMulticastGroup(int unit, 
                                 bcm_field_entry_t entry, 
                                 bcm_gport_t group)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_SrcMplsGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMplsGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_SrcMimGport(int unit, 
                              bcm_field_entry_t entry, 
                              bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMimGport(int unit, 
                              bcm_field_entry_t entry, 
                              bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_SrcWlanGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstWlanGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_Loopback(int unit, 
                               bcm_field_entry_t entry, 
                               uint8 data, 
                               uint8 mask)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_LoopbackType(int unit, 
                                   bcm_field_entry_t entry, 
                                   bcm_field_LoopbackType_t loopback_type)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_TunnelType(int unit, 
                                 bcm_field_entry_t entry, 
                                 bcm_field_TunnelType_t tunnel_type)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_LoopbackType_get
 * Purpose:
 *      Get loopback type field qualification from a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      loopback_type - (OUT) Loopback type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_LoopbackType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_LoopbackType_t *loopback_type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TunnelType_get
 * Purpose:
 *      Get tunnel type field qualification from a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      tunnel_type - (OUT) Tunnel type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TunnelType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_TunnelType_t *tunnel_type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstL3Egress_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstL3Egress
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      if_id - (OUT) L3 forwarding object.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstL3Egress_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_if_t *if_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMulticastGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMulticastGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      group - (OUT) Multicast group id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMulticastGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *group)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMplsGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMplsGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMimGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mim_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMimGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mim_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcWlanGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstWlanGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Loopback_get
 * Purpose:
 *      Get loopback field qualification from  a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      data - (OUT) Data to qualify with.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Loopback_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OutPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOutPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OutPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InPorts_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InPorts_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_pbmp_t *data, 
    bcm_pbmp_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OutPorts_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOutPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OutPorts_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_pbmp_t *data, 
    bcm_pbmp_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Drop_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDrop
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Drop_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcModid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcModid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data, 
    bcm_module_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcPortTgid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcPortTgid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcPortTgid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data_modid, 
    bcm_module_t *mask_modid, 
    bcm_port_t *data_port, 
    bcm_port_t *mask_port)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcTrunk_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_trunk_t *data, 
    bcm_trunk_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstModid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstModid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstModid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data, 
    bcm_module_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstPortTgid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstPortTgid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstPortTgid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data_modid, 
    bcm_module_t *mask_modid, 
    bcm_port_t *data_port, 
    bcm_port_t *mask_port)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstTrunk_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_trunk_t *data, 
    bcm_trunk_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerL4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerL4SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerL4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerL4DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlan_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOuterVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlan_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlan_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlan_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_EtherType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyEtherType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_EtherType_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_EqualL4Port_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyEqualL4Port
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_EqualL4Port_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpProtocol_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_IpInfo_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpInfo
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpInfo_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PacketRes_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPacketRes
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PacketRes_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DSCP_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DSCP_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpFlags_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpFlags
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpFlags_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpControl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpControl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpControl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpSequenceZero_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpSequenceZero
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier match flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpSequenceZero_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpHeaderSize_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpHeaderSize
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpHeaderSize_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ttl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ttl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_RangeCheck_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyRangeCheck
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      max_count - (IN) Max entries to fill.
 *      range - (OUT) Range checkers array.
 *      invert - (OUT) Range checkers invert array.
 *      count - (OUT) Number of filled range checkers.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_RangeCheck_get(
    int unit, 
    bcm_field_entry_t entry, 
    int max_count, 
    bcm_field_range_t *range, 
    int *invert, 
    int *count)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIpEqualDstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIpEqualDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier match flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIpEqualDstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6NextHeader_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6NextHeader
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6NextHeader_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6TrafficClass_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6TrafficClass
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6TrafficClass_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIp6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIp6FlowLabel_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6FlowLabel_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6HopLimit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6HopLimit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6HopLimit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMac_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMac
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_mac_t *data, 
    bcm_mac_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMac_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMac
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_mac_t *data, 
    bcm_mac_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PacketFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPacketFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PacketFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMacGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMacGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMacGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Inner ip header ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_ForwardingType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2Format_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2Format
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match l2 format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2Format_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_L2Format_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_VlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_VlanFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MHOpcode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMHOpcode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MHOpcode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_UserDefined_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyUserDefined
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      udf_id - (IN) Udf spec id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_UserDefined_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_udf_t udf_id, 
    uint8 data[BCM_FIELD_USER_FIELD_SIZE], 
    uint8 mask[BCM_FIELD_USER_FIELD_SIZE])
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_HiGig_get
 * Purpose:
 *      Qualify on HiGig packets.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_HiGig_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassField_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassField_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassField_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassField_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3IntfGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3IntfGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3IntfGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_if_group_t *data, 
    bcm_if_group_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier protocol encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpProtocolCommon_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpProtocolCommon_t *protocol)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier inner ip protocol encodnig.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpProtocolCommon_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpProtocolCommon_t *protocol)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Snap_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySnap
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Snap_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_snap_header_t *data, 
    bcm_field_snap_header_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Llc_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyLlc
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Llc_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_llc_header_t *data, 
    bcm_field_llc_header_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerTpid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerTpid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *tpid)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterTpid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOuterTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterTpid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *tpid)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PortClass_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPortClass
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PortClass_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3Routable_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3Routable
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3Routable_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Qualifier ip framentation encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpFrag_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpFrag_t *frag_info)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_LookupClass0_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyLookupClass0
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_LookupClass0_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Vrf_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVrf
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vrf_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ExtensionHeaderType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyExtensionHeaderType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ExtensionHeaderType_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ExtensionHeaderSubCode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyExtensionHeaderSubCode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ExtensionHeaderSubCode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4Ports_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4Ports
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4Ports_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MirrorCopy_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMirrorCopy
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MirrorCopy_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TunnelTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTunnelTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TunnelTerminated_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MplsTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMplsTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MplsTerminated_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerTtl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerTtl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDSCP_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDSCP_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpProtocol_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Inner ip header fragmentation info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpFrag_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpFrag_t *frag_info)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DosAttack_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDosAttack
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DosAttack_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpmcStarGroupHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpmcStarGroupHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpmcStarGroupHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3DestRouteHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3DestRouteHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3DestRouteHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3DestHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3DestHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3DestHostHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3SrcHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3SrcHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3SrcHostHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2CacheHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2CacheHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2CacheHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2StationMove_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2StationMove
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2StationMove_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2DestHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2DestHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2DestHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2SrcStatic_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2SrcStatic
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2SrcStatic_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2SrcHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2SrcHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2SrcHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IngressStpState_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIngressStpState
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IngressStpState_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanValid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingVlanValid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanValid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_VlanTranslationHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVlanTranslationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_VlanTranslationHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpAuth_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpAuth
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpAuth_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_BigIcmpCheck_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyBigIcmpCheck
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Flag.
 *      size - (OUT) Size.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_BigIcmpCheck_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag, 
    uint32 *size)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IcmpTypeCode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIcmpTypeCode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IcmpTypeCode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IgmpTypeMaxRespTime_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIgmpTypeMaxRespTime
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IgmpTypeMaxRespTime_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_TranslatedVlanFormat
 * Purpose:
 *      Set match criteria for bcmFieildQualifyTranslatedVlanFormat
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TranslatedVlanFormat(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IntPriority
 * Purpose:
 *      Set match criteria for bcmFieildQualifyIntPriority
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IntPriority(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_Color
 * Purpose:
 *      Set match criteria for bcmFieildQualifyColor
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      color - (IN) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Color(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 color)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_SrcModPortGport
 * Purpose:
 *      Set match criteria for bcmFieildQualifySrcModPortGport
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match gport.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModPortGport(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t data)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_TranslatedVlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTranslatedVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TranslatedVlanFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IntPriority_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIntPriority
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IntPriority_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Color_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyColor
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      color - (OUT) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Color_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *color)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_SrcModPortGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcModPortGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match gport.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModPortGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *data)
{
    return BCM_E_UNAVAIL; 
}

/* Function: bcm_robo_field_group_wlan_create_mode
 *     
 * Purpose:
 *     Create a wlan field group with a mode (single, double, etc.).
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range,
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - (OUT) field Group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_group_wlan_create_mode(int unit, bcm_field_qset_t qset, int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL; 
}

/* Function: bcm_robo_field_group_wlan_create_mode_id
 *     
 * Purpose:
 *     Create a wlan field group with a mode (single, double, etc.).
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range,
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - (OUT) field Group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_group_wlan_create_mode_id(int unit, bcm_field_qset_t qset, int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t group)
{
    return BCM_E_UNAVAIL; 
}

#ifdef BROADCOM_DEBUG

/* Section: Field Debug */

/*
 * Function: bcm_field_show
 *
 * Purpose:
 *     Show current S/W state if compiled in debug mode.
 *
 * Parameters:
 *     unit - BCM device number
 *     pfx - Character string to prefix output lines
 *
 * Returns:
 *     Nothing.
 */


int
bcm_robo_field_show(int unit, const char *pfx)
{
    int                 idx;
    _field_control_t    *fc;
    _field_group_t      *fg;
    _field_range_t      *fr;
    uint32              udf_num;

    if (_field_control[unit] == NULL) {
        FP_SHOW(("%s: BCM.%d: not initialized!\n", pfx, unit));
        return BCM_E_INIT;
    }
    fc = _field_control[unit];

    FP_SHOW(("%s:\tunit %d:", pfx, unit));
    FP_SHOW((" field_status={group_total=%d, group_free=%d}\n",
                 fc->field_status.group_total, fc->field_status.group_free));

    FP_SHOW(("%s:\t      :tcam_sz=%d(%#x),", pfx, fc->tcam_sz, fc->tcam_sz));
    FP_SHOW((" tcam_slices=%d,", fc->tcam_slices));
    FP_SHOW((" tcam_slice_sz=%d(%#x),", fc->tcam_slice_sz, fc->tcam_slice_sz));
    FP_SHOW((" tcam_ext_numb=%d,", fc->tcam_ext_numb));
    FP_SHOW(("\n"));

    /* Print the any defined UDFs. */
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num));
    for (idx = 0; idx < udf_num; idx++) {
        if (fc->udf[idx].valid) {
        FP_SHOW(("%s:\tudf %d: use_count=%d, udf_num=UserDefined%d\n",
                     pfx, idx, fc->udf[idx].use_count,idx));
        }
    }

    /* Display any range checkers defined. */
    for (fr = fc->ranges; fr != NULL; fr = fr->next) {
        _robo_field_range_dump(pfx, fr);
    }

    /* Print the groups, along with their entries. */
    for (fg = fc->groups; fg != NULL ; fg = fg->next) {
        bcm_field_group_dump(unit, fg->gid);
    }

    return BCM_E_NONE;
}

/*
 * Function: bcm_field_entry_dump
 *
 * Purpose:
 *     Show contents of a field entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry to dump
 *
 * Returns:
 *     Nothing.
 */
int
bcm_robo_field_entry_dump(int unit, bcm_field_entry_t entry)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    _field_entry_t      *f_ent;
    _field_action_t     *fa;

    fc = _field_control[unit];
    if (fc == NULL) {
        FP_SHOW(("unit %d not initialized\n", unit));
        return BCM_E_INIT;
    }

    f_ent = _robo_field_entry_find(unit, entry, _FP_SLICE_PRIMARY);
    if (f_ent == NULL) {
        FP_SHOW(("unit %d entry %d not initialized\n", unit, entry));
        return BCM_E_INIT;
    }

    fg = f_ent->fs->group;
    /* Dump the primary entry. */
    _robo_field_entry_phys_dump(unit, f_ent);

    /* Display action data */
    fa = f_ent->actions;
    while (fa != NULL) {
        FP_SHOW(("         action="));
        _robo_field_action_dump(fa);
        FP_SHOW(("\n"));
        fa = fa->next;
    }

    FP_SHOW(("         counter="));
    _field_counter_dump(f_ent);
    FP_SHOW((",\n"));
    FP_SHOW(("         meter="));
    _field_meter_dump(f_ent);
    FP_SHOW(("\n"));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _robo_field_range_dump
 *
 * Purpose:
 *     Show contents of a range checker
 *
 * Parameters:
 *     fr - range checker structure pointer
 *
 * Returns:
 *     Nothing.
 */
STATIC void
_robo_field_range_dump(const char *pfx, _field_range_t *fr)
{
    FP_SHOW(("%s: Range ID=%d, flag=%#x, min=%d, max=%d, hw_index=%d(%#x), ",
                 pfx, fr->rid, fr->flags, fr->min, fr->max, fr->hw_index,
                 fr->hw_index));
}

/*
 * Function:
 *     _robo_field_entry_phys_dump
 *
 * Purpose:
 *     Show contents of a physical entry structure
 *
 * Parameters:
 *     unit  - BCM device number
 *     f_ent - Physical entry to dump
 *
 * Returns:
 *     Nothing.
 */
STATIC void
_robo_field_entry_phys_dump(int unit, _field_entry_t *f_ent)
{
    _field_group_t      *fg;
    int         idx;

    fg = f_ent->fs->group;

    FP_SHOW(("EID %3d: gid=%d\n", f_ent->eid, f_ent->fs->group->gid));
    FP_SHOW(("         slice_idx=%#x, prio=%#x, ", f_ent->slice_idx,
                 f_ent->prio));
    if (f_ent->dirty == _FIELD_CLEAN) {
        FP_SHOW(("Installed\n"));
    } else {
        FP_SHOW(("Not installed\n"));
    }

    FP_SHOW(("          "));
    FP_SHOW((" KEY=0x"));
    for (idx = 5; idx >=0; idx--) {
        FP_SHOW(("%08x ", f_ent->drv_entry.tcam_data[idx]));
  
    }

    FP_SHOW(("\n           "));
    FP_SHOW(("MASK=0x"));
    for (idx = 5; idx >= 0; idx--) {
        FP_SHOW(("%08x ", f_ent->drv_entry.tcam_mask[idx]));
  
    }
    
    FP_SHOW(("         tcam:          color_indep=%d, ", f_ent->color_indep));
    FP_SHOW(("\n         "));
}

/*
 * Function: bcm_field_group_dump
 *
 * Purpose:
 *     Show contents of a field group.
 *
 * Parameters:
 *     unit  - BCM device number
 *     group - Field group to dump
 *
 * Returns:
 *     Nothing.
 */

int
bcm_robo_field_group_dump(int unit, bcm_field_group_t group)
{
    _field_control_t    *fc;
    _field_group_t      *fg;
    int                 enable;
    int                 idx;

    if (!SOC_UNIT_VALID(unit) ||
        !soc_feature(unit, soc_feature_field) ||
        _field_control[unit] == NULL) {
        FP_SHOW(("FP: unit %d not initialized\n", unit));
        return BCM_E_INIT;
    }

    fc = _robo_field_control_find(unit);
    assert(fc != NULL);

    fg = _robo_field_group_find(unit, group);

    if (fg == NULL) {
        FP_SHOW(("GID %d: not initialized\n", group));
        return BCM_E_INIT;
    }

    FP_SHOW(("GID %3d: unit=%d, gid=%d, mode=%s, ", group,
                 fg->unit, fg->gid, _robo_field_group_mode_name(fg->mode)));
    bcm_field_group_enable_get(unit, group, &enable);
    if (enable) {
        FP_SHOW(("lookup=Enabled,\n"));
    } else {
        FP_SHOW(("lookup=Disabled,\n"));
    }
    _robo_field_qset_dump("         qset=", fg->qset, ",\n");

    /* Print the primary slice data. */
    FP_SHOW(("         slice_pri="));
    _robo_field_slice_dump(unit, "                    ", &fg->slices[0], "\n");

    FP_SHOW(("         group_status="));
    _robo_field_group_status_dump(&fg->group_status);
    FP_SHOW(("\n"));

    /* Print group's entries */
    for (idx = 0; idx < fc->tcam_sz; idx++) {
        if (fg->slices[0].entries[idx] != NULL) {
            if (fg->slices[0].entries[idx]->gid == group) {
                bcm_field_entry_dump(unit, fg->slices[0].entries[idx]->eid);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _robo_field_selcode_dump
 * Purpose:
 *     Output a set of field selects code.
 */
void
_robo_field_selcode_dump(int unit, char *prefix, _field_sel_t sel_codes,
                    char *suffix)
{
    FP_SHOW(("%s{", (prefix == NULL) ? "" : prefix));
    FP_SHOW(("FPF=%2d", sel_codes.fpf));


    FP_SHOW(("}%s", (suffix == NULL) ? "" : suffix));
}

/*
 * Function:
 *     _robo_field_slice_dump
 * Purpose:
 *     Output a slice worth of data, including any entries in the slice.
 */
STATIC void 
_robo_field_slice_dump(int unit, char *prefix, _field_slice_t *fs, char *suffix)
{
    int                 first_print;
    _qual_info_t        *qi = NULL;

    FP_SHOW(("{slice_numb=%d, ", fs->slice_numb));
    _robo_field_selcode_dump(unit, "tcam_config=", fs->sel_codes, ", ");

    if (fs->group != NULL) {
        FP_SHOW(("gid=%d,\n", fs->group->gid));
    } else {
        FP_SHOW(("gid=NONE,\n"));
    }

    FP_SHOW(("%sqset=", prefix));
    _robo_field_qset_dump("", fs->qset, "");
    FP_SHOW((",\n"));

    qi = fs->qual_list;
    FP_SHOW(("%squal_list={", prefix));
    first_print = 1;

    /* Output the qualifier info list*/
    while (qi != NULL) {
        FP_SHOW(("%s%s", 
                     (first_print ? "" : "->"), 
                     _robo_field_qual_name(qi->qid)));
        first_print = 0;
        qi = qi->next;
    }
    FP_SHOW(("}},%s", (suffix == NULL) ? "" : suffix));
}

/*
 * Function:
 *     _robo_field_qual_list_dump
 * Purpose:
 *     Output qualiers set in 'qset'.
 */
 void
_robo_field_qual_list_dump(char *prefix, _qual_info_t *qual_list, char *suffix)
{
    _qual_info_t        *qi;
    int                 first_print = 0;

    FP_SHOW(("%s{", (prefix == NULL) ? "" : prefix));
    qi = qual_list;
    first_print = 1;

    /* Output the qualifier info list*/
    while (qi != NULL) {
        FP_SHOW(("%s%s", 
                     (first_print ? "" : "->"), 
                     _robo_field_qual_name(qi->qid)));
        first_print = 0;
        qi = qi->next;
    }

    FP_SHOW(("}%s", (suffix == NULL) ? "" : suffix));
}

/*
 * Function:
 *     _field_qset_dump
 * Purpose:
 *     Output qualiers set in 'qset'.
 */
void
_robo_field_qset_dump(char *prefix, bcm_field_qset_t qset, char *suffix)
{
    bcm_field_qualify_t qual;
    int                 idx;
    int first_qual = 1, first_udf_id = 1;
    uint32              udf_num;
    int                 unit = 0;

    if (prefix == NULL) {
        prefix = "";
    }
    if (suffix == NULL) {
        suffix = "";
    }

    FP_SHOW(("%s{", prefix));
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if (BCM_FIELD_QSET_TEST(qset, qual)) {
            FP_SHOW(("%s%s", (first_qual ? "" : ", "), 
                         _robo_field_qual_name(qual)));
            first_qual = 0;
        }
    }

    DRV_SERVICES(unit)->dev_prop_get
                    (unit, DRV_DEV_PROP_CFP_UDFS_NUM, &udf_num);
    for (idx = 0; idx < udf_num; idx++) {
        if (!SHR_BITGET(qset.udf_map, idx)) {
            continue;
        }
        FP_SHOW(("%s%d", (first_udf_id ? " : udf_id={" : ", "), idx));
        first_udf_id = 0;
    }
    if (first_udf_id == 0) {
        FP_SHOW(("}"));
    }

    FP_SHOW(("}%s", suffix));
}

/*
 * Function:
 *     _field_qset_debug
 * Purpose:
 *     Output qualier set in 'qset' for debug mode only.
 */
void
_robo_field_qset_debug(bcm_field_qset_t qset)
{
    bcm_field_qualify_t qual;
    int first_qual = 1;

    FP_VERB(("{"));
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if (BCM_FIELD_QSET_TEST(qset, qual)) {
            FP_VERB(("%s%s", (first_qual ? "" : ", "), 
                         _robo_field_qual_name(qual)));
            first_qual = 0;
        }
    }
    FP_VERB(("}"));
}

/*
 * Function:
 *     _field_counter_dump
 * Purpose:
 *     Output fields in a _field_counter_s struct.
 */
STATIC void
_field_counter_dump(const _field_entry_t *f_ent)
{
    _field_counter_t    *f_cnt;

    assert(f_ent != NULL);
    f_cnt = f_ent->counter;

    if (f_cnt == NULL) {
        FP_SHOW(("NULL"));
    } else {
        FP_SHOW(("{idx=%d, mode=%#x, entries=%d}", f_cnt->index,
                     f_ent->mode_ctr, f_cnt->entries));
    }
}

/*
 * Function:
 *     _field_meter_dump
 * Purpose:

 *     Output meter data for given entry.
 */
STATIC void
_field_meter_dump(const _field_entry_t *f_ent)
{
    _field_meter_t      *fm;

    assert(f_ent != NULL);
    fm = f_ent->meter; 

    if (fm == NULL) {
        FP_SHOW(("NULL,"));
    } else {
        FP_SHOW(("{"));
        FP_SHOW(("peak_kbits_sec=%#x, peak_kbits_burst=%#x,",
                     fm->peak_kbits_sec, fm->peak_kbits_burst));
        FP_SHOW((" commit_kbits_sec=%#x, commit_kbits_burst=%#x, ",
                     fm->commit_kbits_sec, fm->commit_kbits_burst));
        FP_SHOW((" mode=%#x, entries=%d}",
                     f_ent->mode_mtr, fm->entries));
    }
}

/*
 * Function:
 *     _robo_field_action_dump
 * Purpose:
 *     Output fields in a _field_action_s struct.
 */
STATIC void
_robo_field_action_dump(const _field_action_t *fa)
{
    if (fa == NULL) {
        FP_SHOW(("NULL"));
    } else {
        FP_SHOW(("{act=%s, param0=%d(%#x), param1=%d(%#x)},",
                     _robo_field_action_name(fa->action), fa->param0, fa->param0,
                     fa->param1, fa->param1));
    }
}

/*
 * Function:
 *     _robo_field_group_status_dump
 * Purpose:
 *     Output the fields in a bcm_field_group_status_s struct.
 */
STATIC void
_robo_field_group_status_dump(const bcm_field_group_status_t *gstat)
{
    FP_SHOW(("{prio_min=%d,",       gstat->prio_min));
    FP_SHOW((" prio_max=%d,",       gstat->prio_max));
    FP_SHOW((" entries_total=%d,",  gstat->entries_total));
    FP_SHOW((" entries_free=%d,",   gstat->entries_free));
    FP_SHOW(("\n                      "));
    FP_SHOW((" counters_total=%d,", gstat->counters_total));
    FP_SHOW((" counters_free=%d,",  gstat->counters_free));
    FP_SHOW((" meters_total=%d,",   gstat->meters_total));
    FP_SHOW((" meters_free=%d}",    gstat->meters_free));
}

#endif /* BROADCOM_DEBUG */
int bcm_robo_field_resync(int unit)
{
    return BCM_E_UNAVAIL;
}
#else  /* !BCM_FIELD_SUPPORT */



int
bcm_robo_field_init(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_detach(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_flush(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_status_get(int unit, bcm_field_status_t *status)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_control_get(int unit, bcm_field_control_t control, uint32 *state)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_control_set(int unit, bcm_field_control_t control, uint32 state)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_create(int unit,
                           bcm_field_qset_t qset,
                           int pri,
                           bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_create_id(int unit,
                              bcm_field_qset_t qset,
                              int pri,
                              bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_create_mode(int unit,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_create_mode_id(int unit,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_mode_get(int unit,
                         bcm_field_group_t group,
                         bcm_field_group_mode_t *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_set(int unit,
                        bcm_field_group_t group,
                        bcm_field_qset_t qset)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_get(int unit,
                        bcm_field_group_t group,
                        bcm_field_qset_t *qset)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_destroy(int unit,
                            bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_compress(int unit, bcm_field_group_t group)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_group_priority_set(int unit, bcm_field_group_t group,
                                 int priority)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_group_priority_get(int unit, bcm_field_group_t group,
                                 int *priority)
{
    return (BCM_E_UNAVAIL);
}


int
bcm_robo_field_group_status_get(int unit,
                               bcm_field_group_t group,
                               bcm_field_group_status_t *status)
{
    return BCM_E_UNAVAIL;
}

    int
bcm_robo_field_group_enable_set(int unit, bcm_field_group_t group, int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_enable_get(int unit, bcm_field_group_t group, int *enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_range_create_id(int unit,
                              bcm_field_range_t range,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_range_group_create_id(int unit,
                                    bcm_field_range_t range,
                                    uint32 flags,
                                    bcm_l4_port_t min,
                                    bcm_l4_port_t max,
                                    bcm_if_group_t group)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_range_create(int unit,
                           bcm_field_range_t *range,
                           uint32 flags,
                           bcm_l4_port_t min,
                           bcm_l4_port_t max)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function: bcm_robo_field_range_group_create
 *    
 * Purpose:
 *     Allocate a range checker and set its parameters.
 *
 * Parameters:
 *     unit   - BCM device number
 *     range  - (OUT) Range check ID, will not be zero
 *     flags  - One or more of BCM_FIELD_RANGE_* flags
 *     min    - Lower bounds of range to be checked, inclusive
 *     max    - Upper bounds of range to be checked, inclusive
 *     group  - L3 interface group number
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_PARAM     - Invalid L3 interface group number
 *     BCM_E_NONE      - Success
 *     BCM_E_XXX
 */

int
bcm_robo_field_range_group_create(int unit,
                                 bcm_field_range_t *range,
                                 uint32 flags,
                                 bcm_l4_port_t min,
                                 bcm_l4_port_t max,
                                 bcm_if_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_range_get(int unit,
                        bcm_field_range_t range,
                        uint32 *flags,
                        bcm_l4_port_t *min,
                        bcm_l4_port_t *max)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_range_destroy(int unit,
                            bcm_field_range_t range)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_range_set(int unit,
                              bcm_field_group_t group,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_range_get(int unit,
                              bcm_field_group_t group,
                              uint32 *flags,
                              bcm_l4_port_t *min,
                              bcm_l4_port_t *max)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_udf_spec_set(int unit,
                           bcm_field_udf_spec_t *udf_spec,
                           uint32 flags,
                           uint32 offset)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_udf_spec_get(int unit,
                           bcm_field_udf_spec_t *udf_spec,
                           uint32 *flags,
                           uint32 *offset)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_create(int unit,
                         bcm_field_udf_spec_t *udf_spec,
                         bcm_field_udf_t *udf_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_ethertype_set(int unit, int index, 
                                bcm_port_frametype_t frametype,
                                bcm_port_ethertype_t ethertype) 
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_ethertype_get(int unit, int index,
                                bcm_port_frametype_t *frametype,
                                bcm_port_ethertype_t *ethertype) 
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_ipprotocol_set(int unit, int index, uint32 flags, uint8 proto)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_ipprotocol_get(int unit, int index, uint32 *flags,
                                 uint8 *proto)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_create_id(int unit,
                            bcm_field_udf_spec_t *udf_spec,
                            bcm_field_udf_t udf_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_get(int unit,
                      bcm_field_udf_spec_t *udf_spec,
                      bcm_field_udf_t udf_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_udf_destroy(int unit,
                          bcm_field_udf_t udf_id)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qset_add_udf(int unit,
                           bcm_field_qset_t *qset,
                           bcm_field_udf_t udf_id)

{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_entry_create(int unit,
                           bcm_field_group_t group,
                           bcm_field_entry_t *entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_create_id(int unit,
                              bcm_field_group_t group,
                              bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_destroy(int unit,
                            bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_destroy_all(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_copy(int unit,
                         bcm_field_entry_t src_entry,
                         bcm_field_entry_t *dst_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_copy_id(int unit,
                            bcm_field_entry_t src_entry,
                            bcm_field_entry_t dst_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_install(int unit,
                            bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_reinstall(int unit,
                              bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_remove(int unit,
                           bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_entry_policer_attach(int unit, bcm_field_entry_t entry_id, 
                                   int level, bcm_policer_t policer_id)
{
    return BCM_E_UNAVAIL; 
}

int 
bcm_robo_field_entry_policer_detach(int unit, bcm_field_entry_t entry_id, 
                                   int level)
{
    return BCM_E_UNAVAIL; 
}

int 
bcm_robo_field_entry_policer_detach_all(int unit, bcm_field_entry_t entry_id)
{
    return BCM_E_UNAVAIL; 
}

int 
bcm_robo_field_entry_policer_get(int unit, bcm_field_entry_t entry_id, 
                                int level, bcm_policer_t *policer_id)
{
    return BCM_E_UNAVAIL; 
}

int
bcm_robo_field_entry_prio_get(int unit, bcm_field_entry_t entry, int *prio)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_prio_set(int unit, bcm_field_entry_t entry, int prio)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_clear(int unit, bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InPort(int unit, bcm_field_entry_t entry,
                             bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_OutPort(int unit, bcm_field_entry_t entry,
                             bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_qualify_InPorts(int unit, bcm_field_entry_t entry,
                              bcm_pbmp_t data, bcm_pbmp_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Drop(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcModid(int unit, bcm_field_entry_t entry,
                               bcm_module_t data, bcm_module_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcPortTgid(int unit, bcm_field_entry_t entry,
                                  bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstModid(int unit, bcm_field_entry_t entry,
                               bcm_module_t data, bcm_module_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstPortTgid(int unit, bcm_field_entry_t entry,
                                  bcm_port_t data, bcm_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_L4SrcPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_L4DstPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_OuterVlan(int unit, bcm_field_entry_t entry,
                                bcm_vlan_t data, bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerVlan(int unit, bcm_field_entry_t entry,
                                bcm_vlan_t data, bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_EtherType(int unit, bcm_field_entry_t entry,
                                uint16 data, uint16 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_IpProtocol(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_LookupStatus(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_IpInfo(int unit, bcm_field_entry_t entry,
                             uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_PacketRes(int unit, bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DSCP(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_IpFlags(int unit, bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_TcpControl(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Ttl(int unit, bcm_field_entry_t entry,
                          uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_RangeCheck(int unit, bcm_field_entry_t entry,
                                 int range, int result)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_L2Format(int unit, bcm_field_entry_t entry,
                                bcm_field_L2Format_t type)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_VlanFormat(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_DstIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Ip6NextHeader(int unit, bcm_field_entry_t entry,
                                    uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Ip6TrafficClass(int unit, bcm_field_entry_t entry,
                                      uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Ip6FlowLabel(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Ip6HopLimit(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcMac(int unit, bcm_field_entry_t entry,
                             bcm_mac_t data, bcm_mac_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_SrcMacGroup(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_qualify_InterfaceClassL2(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InterfaceClassL3(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InterfaceClassPort(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassL2(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassL3(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_SrcClassField(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassL2(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassL3(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstClassField(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_DstMac(int unit, bcm_field_entry_t entry,
                             bcm_mac_t data, bcm_mac_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_PacketFormat(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_ForwardingType(int unit, bcm_field_entry_t entry,
                             bcm_field_ForwardingType_t data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_IpType(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_MHOpcode(int unit, bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_UserDefined(int unit, bcm_field_entry_t entry,
                                  bcm_field_udf_t udf_id,
                                  uint8 *data, uint8 *mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Decap(int unit, bcm_field_entry_t entry,
                            bcm_field_decap_t decap)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_HiGig(int unit, bcm_field_entry_t entry,
                            uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstHiGig
 * Purpose:
 *      Qualify on HiGig destination packets.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstHiGig(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstHiGig_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstHiGig
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstHiGig_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}


int
bcm_robo_field_qualify_L3IntfGroup(int unit, bcm_field_entry_t entry,
                                  bcm_if_group_t data, bcm_if_group_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_Stage(int unit, bcm_field_entry_t entry,
                            bcm_field_stage_t stage)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_SrcIpEqualDstIp(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_EqualL4Port(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_TcpSequenceZero(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_TcpHeaderSize(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_field_qualify_IpProtocolCommon(int unit, bcm_field_entry_t entry,
                                       bcm_field_IpProtocolCommon_t protocol)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_Snap(int unit, bcm_field_entry_t entry,
                            bcm_field_snap_header_t data, 
                            bcm_field_snap_header_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_Llc(int unit, bcm_field_entry_t entry,
                           bcm_field_llc_header_t data, 
                           bcm_field_llc_header_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_OuterTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_PortClass(int unit, bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}




int
bcm_robo_field_action_add(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 param0,
                         uint32 param1)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_action_get(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 *param0,
                         uint32 *param1)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_action_remove(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_action_t action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_action_remove_all(int unit,
                                bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_action_ports_add(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_action_ports_get(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t *pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_action_delete(int unit, bcm_field_entry_t entry,
                       bcm_field_action_t action,
                       uint32 param0, uint32 param1)
{
   return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_counter_create(int unit,
                             bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_counter_share(int unit,
                            bcm_field_entry_t src_entry,
                            bcm_field_entry_t dst_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_counter_destroy(int unit,
                              bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_counter_set(int unit,
                          bcm_field_entry_t entry,
                          int counter_num,
                          uint64 val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_counter_get(int unit,
                          bcm_field_entry_t entry,
                          int counter_num,
                          uint64 *valp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_counter_get32(int unit,
                            bcm_field_entry_t entry,
                            int counter_num,
                            uint32 *valp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_meter_create(int unit,
                           bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_meter_share(int unit,
                          bcm_field_entry_t src_entry,
                          bcm_field_entry_t dst_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_meter_destroy(int unit,
                            bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_meter_set(int unit,
                        bcm_field_entry_t entry,
                        int meter_num,
                        uint32 kbits_sec,
                        uint32 kbits_burst)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_meter_get(int unit,
                        bcm_field_entry_t entry,
                        int meter_num,
                        uint32 *kbits_sec,
                        uint32 *kbits_burst)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_qualify_OutPorts(int unit, bcm_field_entry_t entry,
                               bcm_pbmp_t data, bcm_pbmp_t mask)
{
    return (BCM_E_UNAVAIL);
}
int bcm_robo_field_qualify_LookupClass0(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_qualify_Vrf(int unit, bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_qualify_ExtensionHeaderType(int unit, 
                                               bcm_field_entry_t entry,
                                               uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_L4Ports
 * Description : qualify on the 4 bytes after L3 header.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 4 bytes after L3 header are present. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_L4Ports(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_MirrorCopy
 * Description : qualify on the mirrored packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not Mirrored/Mirrored packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_MirrorCopy(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}


/* Function    : bcm_robo_field_qualify_TunnelTerminated
 * Description : qualify on the tunnel terminated packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not Tunneled/Tunnel Terminated packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_TunnelTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

/* Function    : bcm_robo_field_qualify_MplsTerminated
 * Description : qualify on the mpls terminated packets only.
 * Parameters  : (IN) unit   BCM device number
 *               (IN) entry  Field entry to qualify
 *               (IN) data   0/1 Not mpls terminated/mpls Terminated packets. 
 *               (IN) mask   data mask.
 * Returns     : BCM_E_XXX
 */
int bcm_robo_field_qualify_MplsTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_qualify_ExtensionHeaderSubCode(int unit, 
                                                  bcm_field_entry_t entry,
                                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                                  bcm_field_IpFrag_t frag_info)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_field_qualify_L3Routable(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{    
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_qualify_Tos(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_group_port_create_mode(int unit, bcm_port_t port,
                                         bcm_field_qset_t qset, int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_port_create_mode_id(int unit, bcm_port_t port,
                                        bcm_field_qset_t qset, int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_ports_create_mode(int unit, bcm_pbmp_t pbmp,
                                      bcm_field_qset_t qset, int pri,
                                      bcm_field_group_mode_t mode,
                                      bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_ports_create_mode_id(int unit, bcm_pbmp_t pbmp,
                                         bcm_field_qset_t qset, int pri,
                                         bcm_field_group_mode_t mode,
                                         bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_field_counter_set32(int unit, bcm_field_entry_t entry, int counter_num,
                            uint32 count)
{
    return BCM_E_UNAVAIL;
}
int bcm_robo_field_resync(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_show(int unit, const char *pfx)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_entry_dump(int unit, bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_group_dump(int unit, bcm_field_group_t group)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerSrcIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerSrcIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerSrcIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDstIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerDSCP(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerIpProtocol(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}


int bcm_robo_field_qualify_InnerIpFrag(int unit, bcm_field_entry_t entry,
                                  bcm_field_IpFrag_t frag_info)
{
     return (BCM_E_UNAVAIL);
}

int
bcm_robo_field_qualify_InnerTtl(int unit, bcm_field_entry_t entry,
                          uint8 data, uint8 mask)
{
     return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_DosAttack(int unit, bcm_field_entry_t entry, 
                                uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IpmcStarGroupHit(int unit, bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3DestRouteHit(int unit, bcm_field_entry_t entry, 
                                      uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3DestHostHit(int unit, bcm_field_entry_t entry, 
                                     uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L3SrcHostHit(int unit, bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2CacheHit(int unit, bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2StationMove(int unit, bcm_field_entry_t entry, 
                                    uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2DestHit(int unit, bcm_field_entry_t entry, 
                                uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2SrcStatic(int unit, bcm_field_entry_t entry, 
                                  uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_L2SrcHit(int unit, bcm_field_entry_t entry, 
                               uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IngressStpState(int unit,bcm_field_entry_t entry, 
                                      uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_ForwardingVlanValid(int unit, bcm_field_entry_t entry, 
                                          uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_VlanTranslationHit(int unit, bcm_field_entry_t entry, 
                                         uint8 data, uint8 mask) 
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IpAuth(int unit, bcm_field_entry_t entry,
                 uint8 data, uint8 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_BigIcmpCheck(int unit, bcm_field_entry_t entry,
                                         uint32 flag, uint32 size)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IcmpTypeCode(int unit, bcm_field_entry_t entry,
                                         uint16 data,uint16 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_IgmpTypeMaxRespTime(int unit, bcm_field_entry_t entry,
                                         uint16 data,uint16 mask)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_entry_stat_attach(int unit, bcm_field_entry_t entry, 
                                    int stat_id)
{
    return (BCM_E_UNAVAIL);
}

int bcm_robo_field_entry_stat_detach(int unit, bcm_field_entry_t entry,
                                    int stat_id)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_entry_stat_get(int unit, bcm_field_entry_t entry, int *stat_id)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_InnerL4DstPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerL4SrcPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIpType(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t type)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIpProtocolCommon(int unit, bcm_field_entry_t entry,
                                       bcm_field_IpProtocolCommon_t protocol)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_field_qualify_InnerIp6FlowLabel(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return (BCM_E_UNAVAIL);
}

int 
bcm_robo_field_qualify_DstL3Egress(int unit, 
                                   bcm_field_entry_t entry, 
                                   bcm_if_t if_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMulticastGroup(int unit, 
                                         bcm_field_entry_t entry, 
                                         bcm_gport_t group)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_SrcMplsGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMplsGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_SrcMimGport(int unit, 
                              bcm_field_entry_t entry, 
                              bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstMimGport(int unit, 
                              bcm_field_entry_t entry, 
                              bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL; 
}



int 
bcm_robo_field_qualify_SrcWlanGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_DstWlanGport(int unit, 
                               bcm_field_entry_t entry, 
                               bcm_gport_t wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}


int 
bcm_robo_field_qualify_Loopback(int unit, 
                               bcm_field_entry_t entry, 
                               uint8 data, 
                               uint8 mask)
{
    return BCM_E_UNAVAIL; 
}



int 
bcm_robo_field_qualify_LoopbackType(int unit, 
                                   bcm_field_entry_t entry, 
                                   bcm_field_LoopbackType_t loopback_type)
{
    return BCM_E_UNAVAIL; 
}

int 
bcm_robo_field_qualify_TunnelType(int unit, 
                                 bcm_field_entry_t entry, 
                                 bcm_field_TunnelType_t tunnel_type)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_LoopbackType_get
 * Purpose:
 *      Get loopback type field qualification from a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      loopback_type - (OUT) Loopback type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_LoopbackType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_LoopbackType_t *loopback_type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TunnelType_get
 * Purpose:
 *      Get tunnel type field qualification from a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      tunnel_type - (OUT) Tunnel type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TunnelType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_TunnelType_t *tunnel_type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstL3Egress_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstL3Egress
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      if_id - (OUT) L3 forwarding object.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstL3Egress_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_if_t *if_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMulticastGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMulticastGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      group - (OUT) Multicast group id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMulticastGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *group)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMplsGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMplsGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMplsGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mpls_port_id - (OUT) Mpls port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMplsGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mpls_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMimGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mim_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMimGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMimGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      mim_port_id - (OUT) Mim port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMimGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *mim_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcWlanGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstWlanGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstWlanGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      wlan_port_id - (OUT) Wlan port id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstWlanGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *wlan_port_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Loopback_get
 * Purpose:
 *      Get loopback field qualification from  a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry id.
 *      data - (OUT) Data to qualify with.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Loopback_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OutPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOutPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OutPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InPorts_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InPorts_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_pbmp_t *data, 
    bcm_pbmp_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OutPorts_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOutPorts
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OutPorts_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_pbmp_t *data, 
    bcm_pbmp_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Drop_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDrop
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Drop_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcModid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcModid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data, 
    bcm_module_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcPortTgid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcPortTgid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcPortTgid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data_modid, 
    bcm_module_t *mask_modid, 
    bcm_port_t *data_port, 
    bcm_port_t *mask_port)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcTrunk_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_trunk_t *data, 
    bcm_trunk_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstModid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstModid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstModid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data, 
    bcm_module_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstPortTgid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstPortTgid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstPortTgid_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_port_t *data, 
    bcm_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data_modid - (OUT) Qualifier module id.
 *      mask_modid - (OUT) Qualifier module id mask.
 *      data_port - (OUT) Qualifier port id.
 *      mask_port - (OUT) Qualifier port id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_module_t *data_modid, 
    bcm_module_t *mask_modid, 
    bcm_port_t *data_port, 
    bcm_port_t *mask_port)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstTrunk_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstTrunk
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstTrunk_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_trunk_t *data, 
    bcm_trunk_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerL4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerL4SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerL4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerL4DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4SrcPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4SrcPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4SrcPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4DstPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4DstPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4DstPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_l4_port_t *data, 
    bcm_l4_port_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlan_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOuterVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlan_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlan_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerVlan
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlan_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_EtherType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyEtherType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_EtherType_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_EqualL4Port_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyEqualL4Port
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_EqualL4Port_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpProtocol_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_IpInfo_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpInfo
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpInfo_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PacketRes_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPacketRes
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PacketRes_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DSCP_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DSCP_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpFlags_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpFlags
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpFlags_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpControl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpControl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpControl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpSequenceZero_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpSequenceZero
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier match flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpSequenceZero_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TcpHeaderSize_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTcpHeaderSize
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TcpHeaderSize_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ttl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ttl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_RangeCheck_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyRangeCheck
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      max_count - (IN) Max entries to fill.
 *      range - (OUT) Range checkers array.
 *      invert - (OUT) Range checkers invert array.
 *      count - (OUT) Number of filled range checkers.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_RangeCheck_get(
    int unit, 
    bcm_field_entry_t entry, 
    int max_count, 
    bcm_field_range_t *range, 
    int *invert, 
    int *count)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcIpEqualDstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcIpEqualDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Qualifier match flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcIpEqualDstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6NextHeader_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6NextHeader
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6NextHeader_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6TrafficClass_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6TrafficClass
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6TrafficClass_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIp6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIp6FlowLabel_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6FlowLabel_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6FlowLabel
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6FlowLabel_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Ip6HopLimit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIp6HopLimit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Ip6HopLimit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMac_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMac
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_mac_t *data, 
    bcm_mac_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstMac_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstMac
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstMac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_mac_t *data, 
    bcm_mac_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PacketFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPacketFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PacketFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcMacGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcMacGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcMacGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Inner ip header ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_ForwardingType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match ip type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpType_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpType_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2Format_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2Format
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      type - (OUT) Qualifier match l2 format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2Format_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_L2Format_t *type)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_VlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_VlanFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MHOpcode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMHOpcode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MHOpcode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_UserDefined_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyUserDefined
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      udf_id - (IN) Udf spec id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_UserDefined_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_udf_t udf_id, 
    uint8 data[BCM_FIELD_USER_FIELD_SIZE], 
    uint8 mask[BCM_FIELD_USER_FIELD_SIZE])
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_HiGig_get
 * Purpose:
 *      Qualify on HiGig packets.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_HiGig_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassPort_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassPort
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassPort_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InterfaceClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInterfaceClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InterfaceClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_SrcClassField_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcClassField_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassL2_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassL2
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassL2_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassL3_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassL3
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassL3_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DstClassField_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDstClassField
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DstClassField_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3IntfGroup_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3IntfGroup
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3IntfGroup_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_if_group_t *data, 
    bcm_if_group_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier protocol encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpProtocolCommon_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpProtocolCommon_t *protocol)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpProtocolCommon_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpProtocolCommon
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      protocol - (OUT) Qualifier inner ip protocol encodnig.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpProtocolCommon_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpProtocolCommon_t *protocol)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Snap_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySnap
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Snap_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_snap_header_t *data, 
    bcm_field_snap_header_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Llc_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyLlc
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Llc_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_llc_header_t *data, 
    bcm_field_llc_header_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerTpid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerTpid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *tpid)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterTpid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyOuterTpid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      tpid - (OUT) Qualifier tpid.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterTpid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *tpid)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_PortClass_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyPortClass
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_PortClass_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3Routable_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3Routable
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3Routable_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Qualifier ip framentation encoding.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpFrag_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpFrag_t *frag_info)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_LookupClass0_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyLookupClass0
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_LookupClass0_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Vrf_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVrf
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vrf_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *data, 
    uint32 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ExtensionHeaderType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyExtensionHeaderType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ExtensionHeaderType_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ExtensionHeaderSubCode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyExtensionHeaderSubCode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ExtensionHeaderSubCode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L4Ports_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL4Ports
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L4Ports_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MirrorCopy_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMirrorCopy
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MirrorCopy_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TunnelTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTunnelTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TunnelTerminated_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_MplsTerminated_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMplsTerminated
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_MplsTerminated_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip_t *data, 
    bcm_ip_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp6_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp6
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp6_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerSrcIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerSrcIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerSrcIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDstIp6High_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDstIp6High
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDstIp6High_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_ip6_t *data, 
    bcm_ip6_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerTtl_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerTtl
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerTtl_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerDSCP_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerDSCP
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerDSCP_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpProtocol_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpProtocol
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpProtocol_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerIpFrag_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyInnerIpFrag
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      frag_info - (OUT) Inner ip header fragmentation info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerIpFrag_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_IpFrag_t *frag_info)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_DosAttack_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyDosAttack
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_DosAttack_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpmcStarGroupHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpmcStarGroupHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpmcStarGroupHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3DestRouteHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3DestRouteHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3DestRouteHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3DestHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3DestHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3DestHostHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L3SrcHostHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL3SrcHostHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L3SrcHostHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2CacheHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2CacheHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2CacheHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2StationMove_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2StationMove
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2StationMove_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2DestHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2DestHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2DestHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2SrcStatic_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2SrcStatic
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2SrcStatic_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_L2SrcHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyL2SrcHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_L2SrcHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IngressStpState_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIngressStpState
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IngressStpState_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanValid_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingVlanValid
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanValid_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_VlanTranslationHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVlanTranslationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_VlanTranslationHit_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IpAuth_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIpAuth
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IpAuth_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_BigIcmpCheck_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyBigIcmpCheck
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      flag - (OUT) Flag.
 *      size - (OUT) Size.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_BigIcmpCheck_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint32 *flag, 
    uint32 *size)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IcmpTypeCode_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIcmpTypeCode
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IcmpTypeCode_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IgmpTypeMaxRespTime_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIgmpTypeMaxRespTime
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IgmpTypeMaxRespTime_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint16 *data, 
    uint16 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualifier_delete
 * Purpose:
 *      Remove match criteria from a field processor entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual_id - (IN) BCM field qualifier id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualifier_delete(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_qualify_t qual_id)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TranslatedVlanFormat
 * Purpose:
 *      Set match criteria for bcmFieildQualifyTranslatedVlanFormat
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TranslatedVlanFormat(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IntPriority
 * Purpose:
 *      Set match criteria for bcmFieildQualifyIntPriority
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IntPriority(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_Color
 * Purpose:
 *      Set match criteria for bcmFieildQualifyColor
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      color - (IN) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Color(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 color)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_SrcModPortGport
 * Purpose:
 *      Set match criteria for bcmFieildQualifySrcModPortGport
 *                     qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match gport.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModPortGport(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t data)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_TranslatedVlanFormat_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyTranslatedVlanFormat
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_TranslatedVlanFormat_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_IntPriority_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyIntPriority
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_IntPriority_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Color_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyColor
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      color - (OUT) Qualifier match color.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Color_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *color)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_SrcModPortGport_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifySrcModPortGport
 *                     qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match gport.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_SrcModPortGport_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_gport_t *data)
{
    return BCM_E_UNAVAIL; 
}


/* Function: bcm_robo_field_group_wlan_create_mode
 *     
 * Purpose:
 *     Create a wlan field group with a mode (single, double, etc.).
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range,
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - (OUT) field Group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_group_wlan_create_mode(int unit, bcm_field_qset_t qset, int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t *group)
{
    return BCM_E_UNAVAIL; 
}

/* Function: bcm_robo_field_group_wlan_create_mode_id
 *     
 * Purpose:
 *     Create a wlan field group with a mode (single, double, etc.).
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range,
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - (OUT) field Group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_robo_field_group_wlan_create_mode_id(int unit, bcm_field_qset_t qset, int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t group)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanId
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanId
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanPri
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanPri
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanPri(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanCfi
 * Purpose:
 *       Set match criteria for bcmFieildQualifyOuterVlanCfi
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanCfi(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanId
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanId
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanPri
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanPri
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanPri(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanCfi
 * Purpose:
 *       Set match criteria for bcmFieildQualifyInnerVlanCfi
 *                       qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanCfi(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 data, 
    uint8 mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanPri_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanPri
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanPri_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_OuterVlanCfi_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyOuterVlanCfi_get
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_OuterVlanCfi_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanPri_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanPri
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanPri_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_InnerVlanCfi_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyInnerVlanCfi_get
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_InnerVlanCfi_get(
    int unit, 
    bcm_field_entry_t entry, 
    uint8 *data, 
    uint8 *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_action_mac_add
 * Purpose:
 *      Add an action to a field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry Id.
 *      action - (IN) Field action id.
 *      mac - (IN) Field action parameter.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_action_mac_add(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_action_mac_get
 * Purpose:
 *      Retrieve the parameters for an action previously added to a
 *      field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field entry Id.
 *      action - (IN) Field action id.
 *      mac - (IN) Field action argument.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_action_mac_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_mac_t *mac)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_group_traverse
 * Purpose:
 *      Traverse all the fp groups in the system, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      callback - (IN) A pointer to the callback function to call for each fp group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_robo_field_group_traverse(int unit, bcm_field_group_traverse_cb callback,
                              void *user_data)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanId_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyForwardingVlanId
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanId_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t *data, 
    bcm_vlan_t *mask)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_robo_field_qualify_Vpn_get
 * Purpose:
 *       Get match criteria for bcmFieildQualifyVpn
 *                       qualifier from the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (OUT) Qualifier match data.
 *      mask - (OUT) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vpn_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vpn_t *data, 
    bcm_vpn_t *mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_ForwardingVlanId
 * Purpose:
 *      Set match criteria for bcmFieildQualifyForwardingVlanId
 *      qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_ForwardingVlanId(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vlan_t data, 
    bcm_vlan_t mask)
{
    return BCM_E_UNAVAIL; 
}


/*
 * Function:
 *      bcm_robo_field_qualify_Vpn
 * Purpose:
 *      Set match criteria for bcmFieildQualifyVpn
 *      qualifier in the field entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data - (IN) Qualifier match data.
 *      mask - (IN) Qualifier match data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_field_qualify_Vpn(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_vpn_t data, 
    bcm_vpn_t mask)
{
    return BCM_E_UNAVAIL; 
}


#endif  /* !BCM_FIELD_SUPPORT */
