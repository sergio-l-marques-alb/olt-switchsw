/*
 * $Id: $
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
 * All Rights Reserved.$
 *
 * File:       field_em.c
 * Purpose:    BCM56960 Field Processor Exact Match functions.
 */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <bcm_int/esw/tomahawk.h>

#include <bcm/field.h>
#include <bcm_int/esw/field.h>

/* Local functions prototypes. */
STATIC
int _field_th_action_profiles_init(int unit, _field_stage_t *stage_fc);

STATIC
int _field_th_qos_action_profiles_init(int unit, _field_stage_t *stage_fc);

/*
 * Function:
 *  _field_th_emstage_init
 * Purpose:
 *  Initialize Exact Match Stage.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  fc       - (IN) Field control structure.
 *  stage_fc - (IN) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  Exact Match Stage in Tomahawk is almost same as IFP stage. Hence there
 *  is no need to make separate database for extractor and ibus. Instead IFP
 *  database is created first and then exact match stage pointers just use
 *  same database instead of creating it's own database.
 */
int
_field_th_emstage_init(int unit, _field_control_t *fc,
                                 _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;         /* Operation Return Status */
    _field_stage_t *stage_ing;   /* Ingress Stage Control Struct */

    /* Input parameter Check */
    if ((fc == NULL) || (stage_fc == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for exact match stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    /* Get Ingress Stage Control Structure */
    stage_ing = fc->stages;
    while (stage_ing) {
        if (stage_ing->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            break;
        }
        stage_ing = stage_ing->next;
    }

    /* Ingress stage check */
    if (NULL == stage_ing) {
        return (BCM_E_PARAM);
    }

    /* Point to Extractor Configuration array as ingress stage */
    stage_fc->ext_cfg_arr = stage_ing->ext_cfg_arr;

    /* Point to Ibus qual sec info as ingress stage */
    stage_fc->input_bus.size = stage_ing->input_bus.size;
    stage_fc->input_bus.num_fields = stage_ing->input_bus.num_fields;
    stage_fc->input_bus.qual_list = stage_ing->input_bus.qual_list;

    /* Initialise Keygen Profile */
    rv = _field_th_keygen_profiles_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: _field_th_keygen_profiles_init=%d\n"),
                  unit, rv));
        return (rv);
    }

    /* Initialise Action Profile */
    rv = _field_th_action_profiles_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: _field_th_action_profiles_init=%d\n"),
                  unit, rv));
        return (rv);
    }

    /* Initialise Qos Action Profile */
    rv = _field_th_qos_action_profiles_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_qos_action_profiles_init=%d\n"),
                 unit, rv));
        return (rv);
    }

    /* Initialize stage select codes table. */
    rv = _bcm_field_stage_fpf_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: _bcm_field_stage_fpf_init=%d\n"),
                  unit, rv));
        return (rv);
    };

    /* Initialize stage Preselector information. */
    rv = _bcm_field_th_stage_preselector_init(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _bcm_field_th_stage_preselector_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_action_profiles_init
 * Purpose:
 *   Initialize keygen program profiles based on group operational mode.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_action_profiles_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;               /* SOC memory names.         */
    int entry_words;             /* Entry size in words.      */
    int inst;                    /* Instance iterator.        */
    int rv;                      /* Operation return status.  */
    static const soc_mem_t act_prof_mem[_FP_MAX_NUM_PIPES] =
    {
        /* Exact Match Action Profile. */
        EXACT_MATCH_ACTION_PROFILE_PIPE0m,
        EXACT_MATCH_ACTION_PROFILE_PIPE1m,
        EXACT_MATCH_ACTION_PROFILE_PIPE2m,
        EXACT_MATCH_ACTION_PROFILE_PIPE3m
    };

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for Exact Match Stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->oper_mode) {

        /* Field Groups operational in Global mode. */
        case bcmFieldGroupOperModeGlobal:

            /* Initialize action profile memory name. */
            mem = EXACT_MATCH_ACTION_PROFILEm;

            /* Determine action profile entry size in number of words. */
            entry_words = soc_mem_entry_words(unit, mem);

            /* Create action program profile table. */
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    &stage_fc->action_profile[_FP_DEF_INST]);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d) Error: action profile creation failed."
                   "=%d\n"), unit, rv));
                return (rv);
            }
            break;

        /* Field Groups operational in Per-Pipe mode. */
        case bcmFieldGroupOperModePipeLocal:

            /* Determine action profiles entry size in number of words. */
            entry_words = soc_mem_entry_words(unit,
                                 EXACT_MATCH_ACTION_PROFILE_PIPE0m);

            for (inst = 0; inst < stage_fc->num_instances; inst++) {

                /* Initialize action program profile memory name. */
                mem = act_prof_mem[inst];

                /* Create action program profile table. */
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                        &stage_fc->action_profile[inst]);

                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                       "FP(unit %d) Error: action profile creation failed."
                       "=%d\n"), unit, rv));
                    return (rv);
                }
            }
            break;
 
        default:
            /*
             * Invalid Group Operational mode, should never hit this condition.
             */
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_qos_action_profiles_init
 * Purpose:
 *   Initialize qos action profiles based on group operational mode.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_qos_action_profiles_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;               /* SOC memory names.                  */
    int entry_words;             /* Entry size in words.               */
    int inst;                    /* Instance iterator.                 */
    int rv;                      /* Operation return status.           */
    static const soc_mem_t qos_action_mem[_FP_MAX_NUM_PIPES] =
    {
        /* Exact Match Qos Action Profile. */
        EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE0m,
        EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE1m,
        EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE2m,
        EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE3m
    };

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for Exact Match Stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->oper_mode) {

        /* Field Groups operational in Global mode. */
        case bcmFieldGroupOperModeGlobal:

            /* Initialize profile1 memory name. */
            mem = EXACT_MATCH_QOS_ACTIONS_PROFILEm;

            /* Determine qos action profile entry size in number of words. */
            entry_words = soc_mem_entry_words(unit, mem);

            /* Create qos action program profile table. */
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    &stage_fc->qos_action_profile[_FP_DEF_INST]);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d) Error: qos action "
                   "profile creation failed."
                   "=%d\n"), unit, rv));
                return (rv);
            }
            break;

         /* Field Groups operational in Per-Pipe mode. */
        case bcmFieldGroupOperModePipeLocal:

            /* Determine qos action profiles entry size in number of words. */
            entry_words = soc_mem_entry_words(unit,
                    EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE0m);

            for (inst = 0; inst < stage_fc->num_instances; inst++) {

                /* Initialize qos action program profile memory name. */
                mem = qos_action_mem[inst];

                /* Create qos action program profile table. */
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                         &stage_fc->qos_action_profile[inst]);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                       "FP(unit %d) Error: qos action "
                       "profile creation failed."
                       "=%d\n"), unit, rv));
                    return (rv);
                }
            }
            break;

        default:
            /*
             * Invalid Group Operational mode, should never hit this condition.
             */
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _field_th_emstage_deinit
 * Purpose:
 *   De-Initialize Exact Match Stage.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   stage_fc - (IN) Stage field control structure.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
int
_field_th_emstage_deinit(int unit, _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */

    /* Input Parameter Check. */
    if (stage_fc == NULL) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for non EM stage. */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    /* Since Exact Match Stage uses same extractor and input
     * bus database as ingress stage. hence deinit will happen
     * during ingress stage delete. Set to NULL here as we
     * are simply using ingress stage database previously.
     */

    stage_fc->ext_cfg_arr = NULL;
    stage_fc->input_bus.size = 0;
    stage_fc->input_bus.num_fields = 0;
    stage_fc->input_bus.qual_list = NULL;

    return (rv);
}

/*
 * Function:
 *  _field_th_em_validate_view
 * Purpose:
 *   Validate Exact Match View.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   key_size - (IN) Qualifier Key Size.
 *   fg       - (IN/OUT) field group structure.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
int
_field_th_em_validate_view(int unit, uint16 key_size, _field_group_t *fg)
{
    int rv = BCM_E_NONE;                /* Operational Status.   */
    bcm_field_aset_t aset;              /* Action Set.           */
    uint16 aset_key_size = 0;           /* Action Set Size.      */
    uint8 aset_size[_FieldEmActionSetCount] = {0};
                                        /* Action Per Set Size.  */
    uint8 aset_cnt[_FieldEmActionSetCount] = {0};
                                        /* Action Per Set Count. */
    uint8 aset_cum[_FieldEmActionSetCount] = {0};
                                        /* Action Per Set
                                           Cumulative Count.     */
    int i,a_idx;                        /* Action Iterator.      */

    /* Input Parameter Check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Retreive Action Set */
    aset = fg->aset;

    /* Reserve 1 Bit always as this info is
     * not available during group creation.
     */
    aset_size[_FieldEmActionGreenToPidSet] +=
                            EM_ACTION_GREEN_TO_PID_SET_SIZE;
    aset_cnt[_FieldEmActionGreenToPidSet]++;

    /* Calculate Action Size. */
    for (a_idx = 0; a_idx < bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(aset, a_idx)) {
            switch (a_idx) {
                case bcmFieldActionPolicerGroup:
                    aset_size[_FieldEmActionMeterSet] +=
                                      EM_ACTION_METER_SET_SIZE;
                    aset_cnt[_FieldEmActionMeterSet]++;
                    break;
                case bcmFieldActionMirrorOverride:
                    aset_size[_FieldEmActionMirrorOverrideSet] +=
                                      EM_ACTION_MIRROR_OVERRIDE_SET_SIZE;
                    aset_cnt[_FieldEmActionMirrorOverrideSet]++;
                    break;
                case bcmFieldActionNatCancel:
                    aset_size[_FieldEmActionNatOverrideSet] +=
                                      EM_ACTION_NAT_OVERRIDE_SET_SIZE;
                    aset_cnt[_FieldEmActionNatOverrideSet]++;
                    break;
                case bcmFieldActionIngSampleEnable:
                    aset_size[_FieldEmActionSflowSet] +=
                                      EM_ACTION_SFLOW_SET_SIZE;
                    aset_cnt[_FieldEmActionSflowSet]++;
                    break;
                    /* Cut Through Override */
                case bcmFieldActionDoNotCheckUrpf:
                    aset_size[_FieldEmActionUrpfOverrideSet] +=
                                      EM_ACTION_CUT_THR_OVERRIDE_SET_SIZE;
                    aset_cnt[_FieldEmActionUrpfOverrideSet]++;
                    break;
                case bcmFieldActionDoNotChangeTtl:
                    aset_size[_FieldEmActionTtlOverrideSet] +=
                                      EM_ACTION_TTL_OVERRIDE_SET_SIZE;
                    aset_cnt[_FieldEmActionTtlOverrideSet]++;
                    break;
                case bcmFieldActionDynamicHgTrunkCancel:
                case bcmFieldActionTrunkLoadBalanceCancel:
                case bcmFieldActionEcmpLoadBalanceCancel:
                    aset_size[_FieldEmActionLbControlSet] +=
                                      EM_ACTION_LB_CONTROL_SET_SIZE;
                    aset_cnt[_FieldEmActionLbControlSet]++;
                    break;
                case bcmFieldActionDrop:
                case bcmFieldActionGpDrop:
                case bcmFieldActionYpDrop:
                case bcmFieldActionRpDrop:
                case bcmFieldActionDropCancel:
                case bcmFieldActionGpDropCancel:
                case bcmFieldActionYpDropCancel:
                case bcmFieldActionRpDropCancel:
                    aset_size[_FieldEmActionDropSet] +=
                                      EM_ACTION_DROP_SET_SIZE;
                    aset_cnt[_FieldEmActionDropSet]++;
                    break;
                case bcmFieldActionCosQCpuNew:
                case bcmFieldActionServicePoolIdNew:
                    aset_size[_FieldEmActionChangeCpuCosSet] +=
                                      EM_ACTION_CHANGE_CPU_COS_SET_SIZE;
                    aset_cnt[_FieldEmActionChangeCpuCosSet]++;
                    break;
                case bcmFieldActionMirrorIngress:
                case bcmFieldActionMirrorEgress:
                    aset_size[_FieldEmActionMirrorSet] +=
                                      EM_ACTION_MIRROR_SET_SIZE;
                    aset_cnt[_FieldEmActionMirrorSet]++;
                    break;
                case bcmFieldActionNat:
                case bcmFieldActionNatEgressOverride:
                    aset_size[_FieldEmActionNatSet] +=
                                      EM_ACTION_NAT_SET_SIZE;
                    aset_cnt[_FieldEmActionNatSet]++;
                    break;
                case bcmFieldActionCopyToCpu:
                case bcmFieldActionGpCopyToCpu:
                case bcmFieldActionYpCopyToCpu:
                case bcmFieldActionRpCopyToCpu:
                case bcmFieldActionTimeStampToCpu:
                case bcmFieldActionGpTimeStampToCpu:
                case bcmFieldActionYpTimeStampToCpu:
                case bcmFieldActionRpTimeStampToCpu:
                case bcmFieldActionCopyToCpuCancel:
                case bcmFieldActionGpCopyToCpuCancel:
                case bcmFieldActionYpCopyToCpuCancel:
                case bcmFieldActionRpCopyToCpuCancel:
                case bcmFieldActionSwitchToCpuCancel:
                case bcmFieldActionGpSwitchToCpuCancel:
                case bcmFieldActionYpSwitchToCpuCancel:
                case bcmFieldActionRpSwitchToCpuCancel:
                case bcmFieldActionSwitchToCpuReinstate:
                case bcmFieldActionGpSwitchToCpuReinstate:
                case bcmFieldActionYpSwitchToCpuReinstate:
                case bcmFieldActionRpSwitchToCpuReinstate:
                case bcmFieldActionTimeStampToCpuCancel:
                case bcmFieldActionGpTimeStampToCpuCancel:
                case bcmFieldActionYpTimeStampToCpuCancel:
                case bcmFieldActionRpTimeStampToCpuCancel:
                    aset_size[_FieldEmActionCopyToCpuSet] +=
                                      EM_ACTION_COPY_TO_CPU_SET_SIZE;
                    aset_cnt[_FieldEmActionCopyToCpuSet]++;
                    break;
                case bcmFieldActionEgressClassSelect:
                case bcmFieldActionHiGigClassSelect:
                case bcmFieldActionNewClassId:
                case bcmFieldActionMultipathHash:
                case bcmFieldActionAddClassTag:
                case bcmFieldActionFabricQueue:
                case bcmFieldActionL3Switch:
                case bcmFieldActionDstMacNew:
                case bcmFieldActionSrcMacNew:
                case bcmFieldActionOuterVlanNew:
                case bcmFieldActionVnTagNew:
                case bcmFieldActionVnTagDelete:
                case bcmFieldActionEtagNew:
                case bcmFieldActionEtagDelete:
                case bcmFieldActionL3SwitchCancel:
                case bcmFieldActionBFDSessionIdNew:
                case bcmFieldActionChangeL2FieldsCancel:
                case bcmFieldActionChangeL2Fields:
                    aset_size[_FieldEmActionL3SwChangeL2Set] +=
                                      EM_ACTION_L3SW_CHANGE_L2_SET_SIZE;
                    aset_cnt[_FieldEmActionL3SwChangeL2Set]++;
                    break;
                case bcmFieldActionRedirectTrunk:
                case bcmFieldActionRedirect:
                case bcmFieldActionUnmodifiedPacketRedirectPort:
                case bcmFieldActionRedirectEgrNextHop:
                case bcmFieldActionRedirectCancel:
                case bcmFieldActionRedirectPbmp:
                case bcmFieldActionRedirectVlan:
                case bcmFieldActionRedirectBcastPbmp:
                case bcmFieldActionRedirectMcast:
                case bcmFieldActionRedirectIpmc:
                case bcmFieldActionEgressMask:
                case bcmFieldActionEgressPortsAdd:
                    aset_size[_FieldEmActionEmRedirectSet] +=
                                      EM_ACTION_EM_REDIRECT_SET_SIZE;
                    aset_cnt[_FieldEmActionEmRedirectSet]++;
                    break;
                case bcmFieldActionStatGroup:
                    aset_size[_FieldEmActionCounterSet] +=
                                      EM_ACTION_COUNTER_SET_SIZE;
                    aset_cnt[_FieldEmActionCounterSet]++;
                    break;
                default:
                    break;
            }
        }
    }

    /* Find action cumulative size. */
    for (i=0; i<_FieldEmActionSetCount; i++) {
        if (aset_cnt[i]) {
            aset_cum[i] = aset_size[i]/aset_cnt[i];
        }
    }

    /* Calculate action key size. */
    for (i=0; i<_FieldEmActionSetCount; i++) {
        aset_key_size += aset_cum[i];
    }

    /* Determine view of group
     * based on key and action size
     * View A(210b) = 160b key + 42b policy + 8b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 18 bit action data
     * View B(210b) = 128b key + 74b policy + 8b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 50 bit action data
     * View C(420b) = 320b key + 84b policy + 16b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 60 bit action data
     */
    if (key_size <= 128) {
        if (aset_key_size <= 18) {
            fg->em_mode = _FieldExactMatchMode160;
        } else if (18 < aset_key_size && aset_key_size <=50) {
            fg->em_mode = _FieldExactMatchMode128;
        } else if (50 < aset_key_size && aset_key_size <=60) {
            fg->em_mode = _FieldExactMatchMode320;
        } else {
            return (BCM_E_CONFIG);
        }
    } else if (128 < key_size && key_size <= 160) {
        if (aset_key_size <= 18) {
            fg->em_mode = _FieldExactMatchMode160;
        } else if (18 < aset_key_size && aset_key_size <=60) {
            fg->em_mode = _FieldExactMatchMode320;
        } else {
            return (BCM_E_CONFIG);
        }
    } else if (160 < key_size && key_size <= 320) {
        if (aset_key_size <= 60) {
            fg->em_mode = _FieldExactMatchMode320;
        } else {
            return (BCM_E_CONFIG);
        }
    } else {
        return (BCM_E_CONFIG);
    }

    /* Validate with group flag also */
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        if (fg->em_mode == _FieldExactMatchMode320) {
            return (BCM_E_CONFIG);
        }
    }

    /* If Double Slice Request */
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fg->em_mode = _FieldExactMatchMode320;
    }

    return (rv);
}

/*
 * Function:
 *  _field_th_em_group_lookup_get
 * Purpose:
 *  Get Lookup information for exact match group
 *  with given priority.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  priority - (IN) Exact Match Group Priority.
 *  lookup   - (OUT) Exact Match Lookup Information.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This function is applicable only for exact match
 *  group.
 */
int
_field_th_em_group_lookup_get(int unit,
                              int priority,
                              uint8 *lookup)
{
    int rv = BCM_E_NONE;        /* Operational Status.        */
    _field_control_t *fc;       /* Unit FP control structure. */
    _field_group_t *fg = NULL;  /* Group information.         */

    /* Input Parameter Check. */
    if (lookup == NULL) {
        return (BCM_E_PARAM);
    }

    /* Get unit FP control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Iterate over the groups linked-list looking for a matching
     * group Id. */
    fg = fc->groups;
    while (fg != NULL) {
        if ((fg->priority == priority) &&
            (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH)) {
            break;
        }
        fg = fg->next;
    }

    /* Exact Match Group Check. */
    if (fg == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Exact Match Group with priority %d "
            "does not exist.\n"),
            unit,priority));
        return (BCM_E_NOT_FOUND);
    }

    /* Slice Check. */
    if (fg->slices == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Return Lookup Information. */
    if (fg->slices->slice_number == 0) {
        *lookup = BCM_FIELD_EXACT_MATCH_LOOKUP_0;
    } else if (fg->slices->slice_number == 1) {
        *lookup = BCM_FIELD_EXACT_MATCH_LOOKUP_1;
    } else {
        rv = BCM_E_INTERNAL;
    }

    return (rv);
}

/*
 * Function:
 *  _field_th_em_ltid_based_groupid_get
 * Purpose:
 *  Get group id information for exact match group
 *  with given logical table id.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  ltid     - (IN) Exact Match Logical Table Id.
 *  group    - (OUT) Exact Match Group Id.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This function is only applicable for exact match
 *  group.
 */
int
_field_th_em_ltid_based_groupid_get(int unit, int ltid,
                                              bcm_field_group_t *group)
{
    int rv = BCM_E_NONE;         /* Operational Status.        */
    _field_control_t *fc;        /* Unit FP control structure. */
    _field_group_t *fg = NULL;   /* Group information.         */

    /* Input Parameter Check */
    if (group == NULL) {
        return (BCM_E_PARAM);
    }

    /* Get unit FP control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Iterate over the groups linked-list looking for a matching Group ID */
    fg = fc->groups;
    while (fg != NULL) {
        if ((fg->lt_id == ltid) &&
            (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH)) {
            break;
        }
        fg = fg->next;
    }

    /* Exact Match Group Check. */
    if (fg == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Exact Match Group with ltid %d "
            "does not exist.\n"),
             unit,ltid));
        return (BCM_E_NOT_FOUND);
    }

    /* Return Group Information. */
    *group = fg->gid;

    return (rv);
}

/*
 * Function:
 *  _field_th_em_group_priority_hintbased_qset_update
 * Purpose:
 *  Update Field Group Qset based on Group Priority.
 * Parameters:
 *  unit       - (IN) BCM device number.
 *  hint_entry - (IN) - Field Hint Structure.
 *  fg         - (OUT) field group structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 */
int
_field_th_em_group_priority_hintbased_qset_update(int unit,
                                                  _field_group_t *fg,
                                                  bcm_field_hint_t *hint_entry)
{
    int rv = BCM_E_NONE;        /* Operational Status. */
    uint8 lkup = 0;             /* Exact Match Lookup. */

    /* Input Parameter Check. */
    if ((fg == NULL) || (hint_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Get Exact Match with provided priority. */
    rv = _field_th_em_group_lookup_get(unit, hint_entry->priority, &lkup);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set Exact Match Hit Status. */
    if (hint_entry->qual == bcmFieldQualifyExactMatchHitStatus) {
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchHitStatus)) {
            if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_0) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchHitStatusLookup0);
            } else if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_1) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchHitStatusLookup1);
            } else {
                rv = BCM_E_INTERNAL;
            }
        }
    } else if (hint_entry->qual == bcmFieldQualifyExactMatchGroupClassId) {
        /* Set Exact Match Group Class Id. */
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchGroupClassId)) {
            if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_0) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup0);
            } else if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_1) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup1);
            } else {
                rv = BCM_E_INTERNAL;
            }
        }
    } else if (hint_entry->qual == bcmFieldQualifyExactMatchActionClassId) {
        /* Set Exact Match Action Class Id. */
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchActionClassId)) {
            if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_0) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchActionClassIdLookup0);
            } else if (lkup == BCM_FIELD_EXACT_MATCH_LOOKUP_1) {
                BCM_FIELD_QSET_ADD(fg->qset,
                        _bcmFieldQualifyExactMatchActionClassIdLookup1);
            } else {
                rv = BCM_E_INTERNAL;
            }
        }
    } else {
        rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *  _field_th_exactmatch_slice_validate
 * Purpose:
 *   Validate Exact Match Slice Allocation.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   stage_fc - (IN) Stage Control structure.
 *   fg       - (IN) field group structure.
 *   slice_id - (IN) Slice Identifier.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
int
_field_th_exactmatch_slice_validate(int unit,
                                    _field_stage_t *stage_fc,
                                    _field_group_t *fg, int slice_id)
{
    int rv = BCM_E_NONE;                   /* Operational Status   */
    _field_group_t *fg_ptr = NULL;         /* Field Group Pointer. */
    _field_lt_slice_t *lt_slice = NULL;    /* Lt Slice Pointer.    */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* In Exact Match Single and Double Wide
     * Groups can share slices.
     */
    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) && (1 == (slice_id % 3))) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Verb: slices not available for DoubleWide"
                    " exact match group.\n"), unit));
        return (BCM_E_CONFIG);
    }

    /* Retrieve the existing groups associated with the Slice */
    rv = _bcm_field_th_slice_group_get_next(unit, fg->instance,
                                            fg->stage_id, slice_id,
                                            &fg_ptr, &fg_ptr);
    if (BCM_SUCCESS(rv)) {
        /* Group with prioirty ANY, can't share
         * slice with any other group's. */
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

        if ((fg_ptr->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Group creation failed, Already default group exists"
                     " with the same priority[%d].\n\r"),
                     fg_ptr->priority));
            return (BCM_E_PARAM);
        }

        if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Group creation failed, can't create a group with the"
                     " priority"
                     " same as existing preselector group priority[%d]\n\r"),
                     fg_ptr->priority));
            return (BCM_E_PARAM);
        }

        lt_slice = fg_ptr->lt_slices;

    } else if (rv == BCM_E_NOT_FOUND) {
        /* Nothing to do. */
        rv = BCM_E_NONE;
    } else {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_field_th_group_lt_slice_validate(unit, stage_fc, fg,
                                                          slice_id, lt_slice));

    return (rv);
}

/*
 * Function:
 *  _field_th_keygen_em_profile_entry_pack
 * Purpose:
 *  Build a LT keygen exact match program profile
 *  table entry.
 * Parameters:
 *  unit         - (IN)     BCM device number.
 *  stage_fc     - (IN)     Stage field control structure.
 *  fg           - (IN)     Field group structure.
 *  part         - (IN)     Entry part number.
 *  mem          - (IN)     Memory name identifier.
 *  prof_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *  BCM_E_XXX
 */
int
_field_th_keygen_em_profile_entry_pack(int unit, _field_stage_t *stage_fc,
                                   _field_group_t *fg, int part, soc_mem_t mem,
                        exact_match_key_gen_program_profile_entry_t *prof_entry)
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
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e32_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_32_sel[idx],
                    fg->ext_codes[part].l1_e32_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_16_sel[idx],
                    fg->ext_codes[part].l1_e16_sel[idx]);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e8_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_8_sel[idx],
                    fg->ext_codes[part].l1_e8_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_4_sel[idx],
                    fg->ext_codes[part].l1_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_2_sel[idx],
                    fg->ext_codes[part].l1_e2_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l2_16_sel[idx],
                    fg->ext_codes[part].l2_e16_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 21; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l3_4_sel[idx],
                    fg->ext_codes[part].l3_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l3_2_sel[idx],
                    fg->ext_codes[part].l3_e2_sel[idx]);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e1_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l3_1_sel[idx],
                    fg->ext_codes[part].l3_e1_sel[idx]);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _field_th_keygen_em_profile_entry_pack
 * Purpose:
 *  Build a LT keygen exact match program profile
 *  mask table entry.
 * Parameters:
 *  unit         - (IN)     BCM device number.
 *  stage_fc     - (IN)     Stage field control structure.
 *  fg           - (IN)     Field group structure.
 *  part         - (IN)     Entry part number.
 *  mem          - (IN)     Memory name identifier.
 *  prof_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *  BCM_E_XXX
 */
int
_field_th_keygen_em_profile_mask_entry_pack(int unit,
                                _field_stage_t *stage_fc,
                                _field_group_t *fg,
                                int part, soc_mem_t mem,
                                exact_match_key_gen_mask_entry_t *prof_entry)
{
    _bcm_field_group_qual_t *q_arr;     /* Group Qualifier Array.    */
    _bcm_field_qual_offset_t *offset;   /* Qualifier Offset Pointer. */
    uint32 *fn_data;                    /* Data Buffer Pointer.      */
    uint32 value;                       /* Mask Value.               */
    int i,j;                            /* Iterators.                */

    /* Input Parameter Check. */
    if ((NULL == stage_fc) ||
        (NULL == fg) ||
        (NULL == prof_entry)) {
        return (BCM_E_PARAM);
    }

    /* Retreive Buffer data pointer and qualifier array. */
    fn_data = (uint32 *)prof_entry;
    q_arr = &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part]);

    /* Set Mask entry. */
    if (q_arr != NULL) {
        if (q_arr->size > 0) {
            for(i=0; i < q_arr->size; i++) {
                offset = q_arr->offset_arr + i;
                for (j=0; j < offset->num_offsets; j++) {
                    if(offset->width[j] > 0) {
                        value = (1 << offset->width[j]) - 1;
                        BCM_IF_ERROR_RETURN(
                           _bcm_field_th_val_set(
                                        fn_data, &value, offset->offset[j],
                                        offset->width[j]));
                    }
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _field_th_em_qos_profile_actions_alloc
 * Purpose:
 *   Allocate Qos Actions Profile for Exact Match Entry.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   f_ent    - (IN) Field Entry.
 *   qp_idx   - (OUT) Qos Actions Profile Index.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
STATIC int
_field_th_em_qos_profile_actions_alloc(int unit,
                                       _field_entry_t *f_ent,
                                       uint32 *qp_idx)
{
    int rv = BCM_E_NONE;                /* Operational Status.               */
    _field_action_t *fa = NULL;         /* Field Action Structure.           */
    _field_stage_t *stage_fc = NULL;    /* Field Stage Control.              */
    exact_match_qos_actions_profile_entry_t qos_prof_entry;
                                        /* Qos Actions Profile Entry Buffer. */
    void *entries[1];                   /* Profile Entry.                    */
    _field_group_t *fg = NULL;          /* Field Group Control.              */
    soc_mem_t mem;                      /* Memory Identifier.                */
    uint32 *bufp;                       /* Hardware Buffer Ptr.              */

    /* Input Parameter Check */
    if ((NULL == f_ent) ||
        (NULL == f_ent->group) ||
        (NULL == qp_idx)) {
        return (BCM_E_PARAM);
    }

    /* Retreive Field Entry Group */
    fg = f_ent->group;

    /* Retreive Stage Control */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 fg->stage_id,
                                                 &stage_fc));

    /* Assign Qos Actions Profile Memory */
    mem = EXACT_MATCH_QOS_ACTIONS_PROFILEm;

    /* Initialize Qos Profile Actions Entry */
    sal_memcpy(&qos_prof_entry, soc_mem_entry_null(unit, mem),
            soc_mem_entry_words(unit, mem) * sizeof(uint32));

    /* Assign Buffer Pointer */
    bufp = (uint32 *)&qos_prof_entry;

    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        if (_FP_ACTION_VALID & fa->flags) {
            switch (fa->action) {
                case bcmFieldActionCosQNew:
                case bcmFieldActionGpCosQNew:
                case bcmFieldActionYpCosQNew:
                case bcmFieldActionRpCosQNew:
                case bcmFieldActionCosMapNew:
                case bcmFieldActionGpCosMapNew:
                case bcmFieldActionYpCosMapNew:
                case bcmFieldActionRpCosMapNew:
                case bcmFieldActionUcastCosQNew:
                case bcmFieldActionGpUcastCosQNew:
                case bcmFieldActionYpUcastCosQNew:
                case bcmFieldActionRpUcastCosQNew:
                case bcmFieldActionMcastCosQNew:
                case bcmFieldActionGpMcastCosQNew:
                case bcmFieldActionYpMcastCosQNew:
                case bcmFieldActionRpMcastCosQNew:
                case bcmFieldActionPrioIntNew:
                case bcmFieldActionGpPrioIntNew:
                case bcmFieldActionYpPrioIntNew:
                case bcmFieldActionRpPrioIntNew:
                case bcmFieldActionDropPrecedence:
                case bcmFieldActionGpDropPrecedence:
                case bcmFieldActionYpDropPrecedence:
                case bcmFieldActionRpDropPrecedence:
                case bcmFieldActionPrioIntCopy:
                case bcmFieldActionPrioIntTos:
                case bcmFieldActionPrioIntCancel:
                case bcmFieldActionGpPrioIntCopy:
                case bcmFieldActionGpPrioIntTos:
                case bcmFieldActionGpPrioIntCancel:
                case bcmFieldActionYpPrioIntCopy:
                case bcmFieldActionYpPrioIntTos:
                case bcmFieldActionYpPrioIntCancel:
                case bcmFieldActionRpPrioIntCopy:
                case bcmFieldActionRpPrioIntTos:
                case bcmFieldActionRpPrioIntCancel:
                case bcmFieldActionGpIntCongestionNotificationNew:
                case bcmFieldActionYpIntCongestionNotificationNew:
                case bcmFieldActionRpIntCongestionNotificationNew:
                case bcmFieldActionPfcClassNew: 
                    rv = _bcm_field_th_profile1_action_set(unit, 
                                                           f_ent, fa, 
                                                           bufp);
                    BCM_IF_ERROR_RETURN(rv);
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionEcnNew:
                case bcmFieldActionGpEcnNew:
                case bcmFieldActionYpEcnNew:
                case bcmFieldActionRpEcnNew:
                case bcmFieldActionPrioPktNew:
                case bcmFieldActionGpPrioPktNew:
                case bcmFieldActionYpPrioPktNew:
                case bcmFieldActionRpPrioPktNew:
                case bcmFieldActionGpTosPrecedenceNew:
                case bcmFieldActionDscpNew:
                case bcmFieldActionGpDscpNew:
                case bcmFieldActionYpDscpNew:
                case bcmFieldActionRpDscpNew:
                case bcmFieldActionPrioPktCopy:
                case bcmFieldActionGpPrioPktCopy:
                case bcmFieldActionYpPrioPktCopy:
                case bcmFieldActionRpPrioPktCopy:
                case bcmFieldActionPrioPktTos:
                case bcmFieldActionGpPrioPktTos:
                case bcmFieldActionYpPrioPktTos:
                case bcmFieldActionRpPrioPktTos:
                case bcmFieldActionPrioPktCancel:
                case bcmFieldActionGpPrioPktCancel:
                case bcmFieldActionYpPrioPktCancel:
                case bcmFieldActionRpPrioPktCancel:
                case bcmFieldActionDot1pPreserve:
                case bcmFieldActionGpDot1pPreserve:
                case bcmFieldActionYpDot1pPreserve:
                case bcmFieldActionRpDot1pPreserve:
                case bcmFieldActionDscpCancel:
                case bcmFieldActionGpDscpCancel:
                case bcmFieldActionYpDscpCancel:
                case bcmFieldActionRpDscpCancel:
                case bcmFieldActionGpTosPrecedenceCopy:
                case bcmFieldActionDscpPreserve:
                case bcmFieldActionGpDscpPreserve:
                case bcmFieldActionYpDscpPreserve:
                case bcmFieldActionRpDscpPreserve:
                    rv = _bcm_field_th_profile2_action_set(unit, 
                                                           f_ent, fa, 
                                                           bufp);
                    BCM_IF_ERROR_RETURN(rv);
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                default :
                    /* Do nothing */
                    break;
            }
        }
    }

    /* Set Profile Entry. */
    entries[0] = bufp;

    /* Add entry to Qos Actions Profile Tables In Hardware */
    rv = soc_profile_mem_add(unit,
                             &stage_fc->qos_action_profile[fg->instance],
                             entries, 1, qp_idx);
    return rv;
}

/*
 * Function:
 *  _field_th_em_profile_actions_alloc
 * Purpose:
 *   Allocate Actions Profile for Exact Match Entry.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   f_ent    - (IN) Field Entry.
 *   abuf     - (OUT) Action Data Buffer.
 *   ap_idx   - (OUT) Actions Profile Index.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
STATIC int
_field_th_em_profile_actions_alloc(int unit,
                                   _field_entry_t *f_ent,
                                   uint32 *abuf,
                                   uint32 *ap_idx)
{
    int rv = BCM_E_NONE;              /* Operational Status.          */
    soc_mem_t mem;                    /* Memory Identifier.           */
    exact_match_action_profile_entry_t act_prof_entry;
                                      /* Action Profile Table Buffer. */
    _field_em_action_set_t acts_buf[_FieldEmActionSetCount];
    _field_action_t *fa = NULL;       /* Field Action Structure.      */
    _field_em_action_set_t *acts;     /* Action Set Data.             */
    _field_stage_t *stage_fc = NULL;  /* Field Stage Control.         */
    _field_group_t *fg = NULL;        /* Field Group Control.         */
    _field_action_set_type_t idx;     /* Exact Match Action Set
                                       * Iterator.                    */
    uint32 act_offset = 0;            /* Exact Match Action Set
                                       * Offset.                      */
    void *entries[1];                 /* Profile Entry.               */
    uint32 meterbuf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                      /* Action Profile Data Buffer. */

    /* Input Parameter Check */
    if ((NULL == f_ent) ||
        (NULL == abuf) ||
        (NULL == ap_idx) ||
        (NULL == f_ent->group)) {
        return (BCM_E_PARAM);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Retreive Stage Control */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 fg->stage_id,
                                                 &stage_fc));

    /* Assign Exact Match Action Profile Memory */
    mem = EXACT_MATCH_ACTION_PROFILEm;

    /* Initialise variables */
    sal_memset(acts_buf, 0, sizeof(acts_buf));
    sal_memcpy(&act_prof_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));

    /* Set Entry Meter Information.. */
    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionPolicerGroup)) {
        acts = &acts_buf[_FieldEmActionMeterSet];
        rv = _field_th_ingress_policer_action_set(unit, f_ent, meterbuf);
        BCM_IF_ERROR_RETURN(rv);
        rv = _bcm_field_th_val_get(meterbuf, &acts->data, 265, 18);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, mem, &act_prof_entry,
                            METER_SET_ENABLEf, 1);
        acts->valid = 1;
    }

    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        if (_FP_ACTION_VALID & fa->flags) {
            switch (fa->action) {
                case bcmFieldActionMirrorOverride:
                    acts = &acts_buf[_FieldEmActionMirrorOverrideSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        MIRROR_OVERRIDE_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionNatCancel:
                    acts = &acts_buf[_FieldEmActionNatOverrideSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        NAT_OVERRIDE_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionIngSampleEnable:
                    acts = &acts_buf[_FieldEmActionSflowSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        SFLOW_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                    /* Cut Through Override */
                case bcmFieldActionDoNotCheckUrpf:
                    acts = &acts_buf[_FieldEmActionUrpfOverrideSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        URPF_OVERRIDE_SET_ENABLEf, 1);
                    acts->valid = 1;
                    break;
                case bcmFieldActionDoNotChangeTtl:
                    acts = &acts_buf[_FieldEmActionTtlOverrideSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        TTL_OVERRIDE_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionTrunkLoadBalanceCancel:
                case bcmFieldActionDynamicHgTrunkCancel:
                case bcmFieldActionEcmpLoadBalanceCancel:
                    acts = &acts_buf[_FieldEmActionLbControlSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        LB_CONTROLS_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionMirrorIngress:
                case bcmFieldActionMirrorEgress:
                    acts = &acts_buf[_FieldEmActionMirrorSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        MIRROR_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionNat:
                case bcmFieldActionNatEgressOverride:
                    acts = &acts_buf[_FieldEmActionNatSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        NAT_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionDrop:
                case bcmFieldActionGpDrop:
                case bcmFieldActionYpDrop:
                case bcmFieldActionRpDrop:
                case bcmFieldActionDropCancel:
                case bcmFieldActionGpDropCancel:
                case bcmFieldActionYpDropCancel:
                case bcmFieldActionRpDropCancel:
                    acts = &acts_buf[_FieldEmActionDropSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        DROP_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionCosQCpuNew:
                case bcmFieldActionServicePoolIdNew:
                    acts = &acts_buf[_FieldEmActionChangeCpuCosSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        CHANGE_CPU_COS_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionCopyToCpu:
                case bcmFieldActionGpCopyToCpu:
                case bcmFieldActionYpCopyToCpu:
                case bcmFieldActionRpCopyToCpu:
                case bcmFieldActionTimeStampToCpu:
                case bcmFieldActionGpTimeStampToCpu:
                case bcmFieldActionYpTimeStampToCpu:
                case bcmFieldActionRpTimeStampToCpu:
                case bcmFieldActionCopyToCpuCancel:
                case bcmFieldActionGpCopyToCpuCancel:
                case bcmFieldActionYpCopyToCpuCancel:
                case bcmFieldActionRpCopyToCpuCancel:
                case bcmFieldActionSwitchToCpuCancel:
                case bcmFieldActionGpSwitchToCpuCancel:
                case bcmFieldActionYpSwitchToCpuCancel:
                case bcmFieldActionRpSwitchToCpuCancel:
                case bcmFieldActionSwitchToCpuReinstate:
                case bcmFieldActionGpSwitchToCpuReinstate:
                case bcmFieldActionYpSwitchToCpuReinstate:
                case bcmFieldActionRpSwitchToCpuReinstate:
                case bcmFieldActionTimeStampToCpuCancel:
                case bcmFieldActionGpTimeStampToCpuCancel:
                case bcmFieldActionYpTimeStampToCpuCancel:
                case bcmFieldActionRpTimeStampToCpuCancel:
                    acts = &acts_buf[_FieldEmActionCopyToCpuSet];
                    rv = _bcm_field_th_misc_action_set(unit, f_ent,
                                                       fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        COPY_TO_CPU_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionEgressClassSelect:
                case bcmFieldActionHiGigClassSelect:
                case bcmFieldActionNewClassId:
                case bcmFieldActionMultipathHash:
                case bcmFieldActionAddClassTag:
                case bcmFieldActionFabricQueue:
                case bcmFieldActionL3Switch:
                case bcmFieldActionDstMacNew:
                case bcmFieldActionSrcMacNew:
                case bcmFieldActionOuterVlanNew:
                case bcmFieldActionVnTagNew:
                case bcmFieldActionVnTagDelete:
                case bcmFieldActionEtagNew:
                case bcmFieldActionEtagDelete:
                case bcmFieldActionL3SwitchCancel:
                case bcmFieldActionBFDSessionIdNew:
                case bcmFieldActionChangeL2FieldsCancel:
                case bcmFieldActionChangeL2Fields:
                    acts = &acts_buf[_FieldEmActionL3SwChangeL2Set];
                    rv = _bcm_field_th_l3swl2change_action_set(unit, f_ent,
                                                               fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        L3SW_CHANGE_L2_SET_ENABLEf, 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                case bcmFieldActionRedirectTrunk:
                case bcmFieldActionRedirect:
                case bcmFieldActionUnmodifiedPacketRedirectPort:
                case bcmFieldActionRedirectEgrNextHop:
                case bcmFieldActionRedirectCancel:
                case bcmFieldActionRedirectPbmp:
                case bcmFieldActionRedirectVlan:
                case bcmFieldActionRedirectBcastPbmp:
                case bcmFieldActionRedirectMcast:
                case bcmFieldActionRedirectIpmc:
                case bcmFieldActionEgressMask:
                case bcmFieldActionEgressPortsAdd:
                    acts = &acts_buf[_FieldEmActionEmRedirectSet];
                    rv = _bcm_field_th_redirect_action_set(unit, f_ent,
                                                           fa, &acts->data);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, &act_prof_entry,
                                        EXACT_MATCH_REDIRECT_SET_ENABLEf , 1);
                    acts->valid = 1;
                    fa->flags &= ~_FP_ACTION_DIRTY;
                    break;
                default:
                    break;
            }
        }
    }

    /* Now Populate Action Data Buffer.
     * Sequence here for action set is hardware dependant
     */
    idx = _FieldEmActionMeterSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                    EM_ACTION_METER_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_METER_SET_SIZE;
    }
    idx = _FieldEmActionGreenToPidSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                             EM_ACTION_GREEN_TO_PID_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_GREEN_TO_PID_SET_SIZE;
    }
    idx = _FieldEmActionMirrorOverrideSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                          EM_ACTION_MIRROR_OVERRIDE_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_MIRROR_OVERRIDE_SET_SIZE;
    }
    idx = _FieldEmActionNatOverrideSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                             EM_ACTION_NAT_OVERRIDE_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_NAT_OVERRIDE_SET_SIZE;
    }
    idx = _FieldEmActionSflowSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                    EM_ACTION_SFLOW_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_SFLOW_SET_SIZE;
    }
    idx = _FieldEmActionCutThrOverrideSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                        EM_ACTION_CUT_THR_OVERRIDE_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_CUT_THR_OVERRIDE_SET_SIZE;
    }
    idx = _FieldEmActionUrpfOverrideSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                            EM_ACTION_URPF_OVERRIDE_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_URPF_OVERRIDE_SET_SIZE;
    }
    idx = _FieldEmActionTtlOverrideSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                             EM_ACTION_TTL_OVERRIDE_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_TTL_OVERRIDE_SET_SIZE;
    }
    idx = _FieldEmActionLbControlSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                   act_offset,
                               EM_ACTION_LB_CONTROL_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_LB_CONTROL_SET_SIZE;
    }
    idx = _FieldEmActionDropSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                     EM_ACTION_DROP_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_DROP_SET_SIZE;
    }
    idx = _FieldEmActionChangeCpuCosSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                           EM_ACTION_CHANGE_CPU_COS_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_CHANGE_CPU_COS_SET_SIZE;
    }
    idx = _FieldEmActionMirrorSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                   EM_ACTION_MIRROR_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_MIRROR_SET_SIZE;
    }
    idx = _FieldEmActionNatSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                      EM_ACTION_NAT_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_NAT_SET_SIZE;
    }
    idx = _FieldEmActionCopyToCpuSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                              EM_ACTION_COPY_TO_CPU_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_COPY_TO_CPU_SET_SIZE;
    }
    idx = _FieldEmActionL3SwChangeL2Set;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                           EM_ACTION_L3SW_CHANGE_L2_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_L3SW_CHANGE_L2_SET_SIZE;
    }
    idx = _FieldEmActionEmRedirectSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                              EM_ACTION_EM_REDIRECT_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_EM_REDIRECT_SET_SIZE;
    }
    idx = _FieldEmActionCounterSet;
    if (acts_buf[idx].valid) {
        rv = _bcm_field_th_val_set(abuf, &acts_buf[idx].data,
                                                  act_offset,
                                  EM_ACTION_COUNTER_SET_SIZE);
        BCM_IF_ERROR_RETURN(rv);
        act_offset += EM_ACTION_COUNTER_SET_SIZE;
    }

    /* Set Profile Entry. */
    entries[0] = &act_prof_entry;

    /* Add Entries To Profile Tables In Hardware. */
    rv = soc_profile_mem_add(unit,
                             &stage_fc->action_profile[fg->instance],
                             entries, 1, ap_idx);
    return (rv);
}

/*
 * Function:
 *  _field_th_em_entry_data_set
 * Purpose:
 *   Set Exact Match Policy Data.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   f_ent    - (IN) Field Entry.
 *   entbuf   - (OUT) Entry Policy Data Buffer.
 *   qp_idx   - (OUT) Qos Actions Profile Index.
 *   ap_idx   - (OUT) Action Profile Index.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
STATIC int
_field_th_em_entry_data_set(int unit,
                            _field_entry_t *f_ent,
                            uint32 *entbuf,
                            uint32 *qp_idx,
                            uint32 *ap_idx)
{
    uint32 abuf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Action Profile Data Buffer. */
    int rv = BCM_E_NONE;             /* Operation Return Status.    */
    _field_stage_t *stage_fc;        /* Field Stage Control.        */
    _field_group_t *fg;              /* Field Group Control.        */

    /* Input Parameters Check. */
    if ((NULL == f_ent) ||
        (NULL == f_ent->group) ||
        (NULL == entbuf) ||
        (NULL == qp_idx) ||
        (NULL == ap_idx)) {
        return (BCM_E_PARAM);
    }

    /* Retreive Field Group. */
    fg = f_ent->group;

    /* Return for non EM Stage. */
    if (fg->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    /* Retreive Stage Control. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 fg->stage_id, &stage_fc));

    /* Set Qos Profile Index. */
    rv = _field_th_em_qos_profile_actions_alloc(unit,
                                                f_ent,
                                                qp_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Set Action Profile Index. */
    rv = _field_th_em_profile_actions_alloc(unit,
                                            f_ent,
                                            abuf,
                                            ap_idx);
    if (BCM_FAILURE(rv)) {
        soc_profile_mem_delete(unit,
                               &stage_fc->qos_action_profile[fg->instance],
                               *qp_idx);
        return (rv);
    }

    /* Set Action Data in entry Buffer. */
    if (fg->em_mode == _FieldExactMatchMode128) {
        soc_mem_field32_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE128__QOS_PROFILE_IDf, *qp_idx);
        soc_mem_field32_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE128__ACTION_PROFILE_IDf, *ap_idx);
        soc_mem_field_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE128__ACTION_DATAf, abuf);
    } else if (fg->em_mode == _FieldExactMatchMode160) {
        soc_mem_field32_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE160__QOS_PROFILE_IDf, *qp_idx);
        soc_mem_field32_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE160__ACTION_PROFILE_IDf, *ap_idx);
        soc_mem_field_set(unit, EXACT_MATCH_2m,
                            entbuf,
                            MODE160__ACTION_DATAf, abuf);
    } else if (fg->em_mode == _FieldExactMatchMode320) {
        soc_mem_field32_set(unit, EXACT_MATCH_4m,
                            entbuf,
                            MODE320__QOS_PROFILE_IDf, *qp_idx);
        soc_mem_field32_set(unit, EXACT_MATCH_4m,
                            entbuf,
                            MODE320__ACTION_PROFILE_IDf, *ap_idx);
        soc_mem_field_set(unit, EXACT_MATCH_4m,
                            entbuf,
                            MODE320__ACTION_DATAf, abuf);
    } else {
        /* Invalid Exact Match Mode. */
        return (BCM_E_PARAM);
    }

    return rv;
}

/*
 * Function:
 *   _field_th_em_entry_install
 * Purpose:
 *   Install Exact Match Entry.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   f_ent    - (IN) Field Entry.
 *   entbuf   - (OUT) Entry Policy Data Buffer.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
int
_field_th_em_entry_install(int unit, _field_entry_t *f_ent)
{
    int rv = BCM_E_NONE;             /* Operational Status.       */
    int parts_count = 0;             /* Field Entry Parts Count.  */
    int e_part = 0;                  /* Entry Part Iterator.      */
    _field_group_t *fg;              /* Field Group Control.      */
    _field_stage_t *stage_fc;        /* Field Stage Control.      */
    exact_match_2_entry_t ebuf_nar;  /* Narrow Entry Buffer.      */
    exact_match_4_entry_t ebuf_wide; /* Wide Entry Buffer.        */
    soc_mem_t mem;                   /* Memory Identifier.        */
    uint32 tbuf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Temp Buffer One.          */
    uint32 tbuftemp[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Temp Buffer Two.          */
    uint32 *bufp;                    /* Hardware Buffer Ptr.      */
    _field_entry_t *f_ent_part;      /* Field Entry Part.         */
    _field_entry_t *f_ent_temp;      /* Field Entry Part Temp.    */
    int part_idx;                    /* Entry Part Index.         */
    uint32 qp_idx = 0;               /* Qos Actions Profile Index.*/
    uint32 ap_idx = 0;               /* Actions Profile Index.    */
    uint32 param0 = 0;               /* Action Param 0.           */
    uint32 param1 = 0;               /* Action Param 1.     .     */
    static soc_mem_t em_entry_nar_mem[_FP_MAX_NUM_PIPES] = {
        EXACT_MATCH_2_PIPE0m,
        EXACT_MATCH_2_PIPE1m,
        EXACT_MATCH_2_PIPE2m,
        EXACT_MATCH_2_PIPE3m,
    };
    static soc_mem_t em_entry_wide_mem[_FP_MAX_NUM_PIPES] = {
        EXACT_MATCH_4_PIPE0m,
        EXACT_MATCH_4_PIPE1m,
        EXACT_MATCH_4_PIPE2m,
        EXACT_MATCH_4_PIPE3m,
    };

    /* Input Parameters Check. */
    if ((NULL == f_ent) ||
        (NULL == f_ent->group) ||
        (NULL == f_ent->fs)) {
        return (BCM_E_PARAM);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Retreive Stage Control. */
    BCM_IF_ERROR_RETURN(
            _field_stage_control_get(unit, fg->stage_id, &stage_fc));

    /* Do nothing for non EM Stage. */
    if (fg->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    /* Get Hardware Buffer Pointer and memory identifier. */
    if ((fg->em_mode == _FieldExactMatchMode128) ||
        (fg->em_mode == _FieldExactMatchMode160)) {
        bufp = (uint32 *)&ebuf_nar;
        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem = EXACT_MATCH_2m;
        } else {
            mem = em_entry_nar_mem[fg->instance];
        }
    } else {
        bufp = (uint32 *)&ebuf_wide;
        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem = EXACT_MATCH_4m;
        } else {
            mem = em_entry_wide_mem[fg->instance];
        }
    }

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
            fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Key If Not Installed */
    if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
        for (part_idx = 0; part_idx < parts_count; part_idx++) {
            f_ent_temp = f_ent + part_idx;
            BCM_IF_ERROR_RETURN(
                    _bcm_field_qual_tcam_key_mask_get(unit, f_ent_temp));
        }
    }

    /* Initialize exact match entries. */
    sal_memcpy(bufp, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));

    /* Validate Parts and EM View. */
    if ((((fg->em_mode == _FieldExactMatchMode128) ||
          (fg->em_mode == _FieldExactMatchMode160)) &&
          (parts_count != 1)) ||
         ((fg->em_mode == _FieldExactMatchMode320) &&
          (parts_count != 2))) {
        return (BCM_E_INTERNAL);
    }

    /* Set Action Buffer. */
    rv = _field_th_em_entry_data_set(unit, f_ent, bufp, &qp_idx, &ap_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Action Class Id. */
    rv = bcm_esw_field_action_get(unit, f_ent->eid, bcmFieldActionExactMatchClassId, &param0, &param1);
    if ((BCM_FAILURE(rv)) && (BCM_E_NOT_FOUND != rv)) {
        return(rv);
    }

    /* Set Exact Match Entry */
    if (fg->em_mode == _FieldExactMatchMode128) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE128__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 101, 27);
        soc_mem_field_set(unit, mem, bufp, MODE128__KEY_1_ONLYf, tbuf);
        soc_mem_field32_set(unit, mem, bufp, MODE128__EXACT_MATCH_CLASS_IDf, param0);

    } else if (fg->em_mode == _FieldExactMatchMode160) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE160__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 101, 59);
        soc_mem_field_set(unit, mem, bufp, MODE160__KEY_1_ONLYf, tbuf);
        soc_mem_field32_set(unit, mem, bufp, MODE160__EXACT_MATCH_CLASS_IDf, param0);

    } else if (fg->em_mode == _FieldExactMatchMode320) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_2f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_3f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_2f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_3f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key, tbuf, 101, 59);

        /* Key From Second Entry Part. */
        f_ent_part = f_ent + 1;
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key, tbuftemp, 0, 42);
        rv = _bcm_field_th_val_set(tbuf, tbuftemp, 59, 42);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_1_ONLYf, tbuf);;
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key, tbuf, 42, 101);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_2_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key, tbuf, 143, 17);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_3_ONLYf, tbuf);

        soc_mem_field32_set(unit, mem, bufp, MODE320__EXACT_MATCH_CLASS_IDf, param0);
    } else {
        /* Invalid Exact Match Mode. */
        return (BCM_E_PARAM);
    }

    /* Write Entry In Hardware. */
    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, bufp);

    if (BCM_FAILURE(rv)) {
        soc_profile_mem_delete(unit,
                               &stage_fc->qos_action_profile[fg->instance],
                               qp_idx);
        soc_profile_mem_delete(unit,
                               &stage_fc->action_profile[fg->instance],
                               ap_idx);
        return (rv);
    }

    /* Copy into Hw Key. */
    for (e_part = 0; e_part < parts_count; e_part++) {

        f_ent_part = f_ent + e_part;
        if(f_ent_part->tcam.key_hw == NULL) {
            _FP_XGS3_ALLOC(f_ent_part->tcam.key_hw,
                f_ent_part->tcam.key_size, "EM_TCAM Key Alloc.");
        }

        sal_memcpy(f_ent_part->tcam.key_hw,
                f_ent_part->tcam.key, f_ent_part->tcam.key_size);

        /* Set Entry Flags. */
        f_ent_part->flags &= ~_FP_ENTRY_DIRTY;
        f_ent_part->flags |= _FP_ENTRY_INSTALLED;
        f_ent_part->flags |= _FP_ENTRY_ENABLED;
    }

    /* Increment Hardware Count. */
    f_ent->fs->hw_ent_count++;

    return rv;
}

/*
 * Function:
 *  _field_th_em_entry_remove
 * Purpose:
 *  Remove Exact Match Entry from hardware.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  f_ent    - (IN) Field Entry.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 *   Install and Enabled Flags will
 *   be cleared by calling function.
 */
int
_field_th_em_entry_remove(int unit, _field_entry_t *f_ent)
{
    int rv = BCM_E_NONE;             /* Operational Status.       */
    int parts_count = 0;             /* Field entry parts count.  */
    int e_part = 0;                  /* Entry Part Iterator.      */
    _field_control_t *fc;            /* Field Control Structure.  */
    _field_group_t *fg;              /* Field Group Control.      */
    _field_stage_t *stage_fc;        /* Field Stage Control.      */
    exact_match_2_entry_t ebuf_nar;  /* Narrow Entry Buffer.      */
    exact_match_4_entry_t ebuf_wide; /* Wide Entry Buffer.        */
    soc_mem_t mem;                   /* Memory Identifier.        */
    uint32 tbuf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Temp Buffer One.          */
    uint32 tbuftemp[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Temp Buffer Two.          */
    int index;                       /* Entry Index.              */
    uint32 *bufp;                    /* Hardware Buffer Ptr.      */
    _field_entry_t *f_ent_part;      /* Field Entry Part.         */
    uint32 qp_idx = 0;               /* Qos Actions Profile Index.*/
    uint32 ap_idx = 0;               /* Action Profile Index.     */
    static soc_mem_t em_entry_nar_mem[_FP_MAX_NUM_PIPES] = {
        EXACT_MATCH_2_PIPE0m,
        EXACT_MATCH_2_PIPE1m,
        EXACT_MATCH_2_PIPE2m,
        EXACT_MATCH_2_PIPE3m,
    };
    static soc_mem_t em_entry_wide_mem[_FP_MAX_NUM_PIPES] = {
        EXACT_MATCH_4_PIPE0m,
        EXACT_MATCH_4_PIPE1m,
        EXACT_MATCH_4_PIPE2m,
        EXACT_MATCH_4_PIPE3m,
    };

    /* Input Parameters Check. */
    if ((NULL == f_ent) ||
        (NULL == f_ent->group) ||
        (NULL == f_ent->fs)) {
        return (BCM_E_PARAM);
    }

    /* If secondary part return none as uninstall
     * has happened in primary part already.
     */
    if (f_ent->flags & _FP_ENTRY_SECONDARY) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Retreive Field Group. */
    fg = f_ent->group;

    /* Retreive Stage Control. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 fg->stage_id, &stage_fc));

    /* Do nothing for non EM Stage. */
    if (fg->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        return (BCM_E_NONE);
    }

    /* Check if HW key is present to remove entry. */
    if (NULL == f_ent->tcam.key_hw) {
        return (BCM_E_PARAM);
    }

    /* Get Hardware Buffer Pointer and memory identifier. */
    if ((fg->em_mode == _FieldExactMatchMode128) ||
        (fg->em_mode == _FieldExactMatchMode160)) {
        bufp = (uint32 *)&ebuf_nar;
        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem = EXACT_MATCH_2m;
        } else {
            mem = em_entry_nar_mem[fg->instance];
        }
    } else {
        bufp = (uint32 *)&ebuf_wide;
        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem = EXACT_MATCH_4m;
        } else {
            mem = em_entry_wide_mem[fg->instance];
        }
    }

    /* Initialize exact match entries. */
    sal_memcpy(bufp, soc_mem_entry_null(unit, mem),
            soc_mem_entry_words(unit, mem) * sizeof(uint32));

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                           fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    /* Validate Parts and EM View. */
    if ((((fg->em_mode == _FieldExactMatchMode128) ||
          (fg->em_mode == _FieldExactMatchMode160)) &&
          (parts_count != 1)) ||
         ((fg->em_mode == _FieldExactMatchMode320) &&
          (parts_count != 2))) {
        return (BCM_E_INTERNAL);
    }

    /* Set Exact Match Entry Search Key. */
    if (fg->em_mode == _FieldExactMatchMode128) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE128__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 101, 27);
        soc_mem_field_set(unit, mem, bufp, MODE128__KEY_1_ONLYf, tbuf);

    } else if (fg->em_mode == _FieldExactMatchMode160) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE160__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 101, 59);
        soc_mem_field_set(unit, mem, bufp, MODE160__KEY_1_ONLYf, tbuf);

    } else if (fg->em_mode == _FieldExactMatchMode320) {

        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_0f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_1f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_2f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, KEY_TYPE_3f, fg->em_mode);
        soc_mem_field32_set(unit, mem, bufp, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_1f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_2f, 1);
        soc_mem_field32_set(unit, mem, bufp, VALID_3f, 1);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 0, 101);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_0_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent->tcam.key_hw, tbuf, 101, 59);

        /* Key From Second Entry Part. */
        f_ent_part = f_ent + 1;
        if (f_ent_part->tcam.key_hw == NULL) {
            return (BCM_E_PARAM);
        }
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key_hw, tbuftemp, 0, 42);
        rv = _bcm_field_th_val_set(tbuf, tbuftemp, 59, 42);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_1_ONLYf, tbuf);;
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key_hw, tbuf, 42, 101);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_2_ONLYf, tbuf);
        rv = _bcm_field_th_val_get(f_ent_part->tcam.key_hw, tbuf, 143, 17);
        soc_mem_field_set(unit, mem, bufp, MODE320__KEY_3_ONLYf, tbuf);

    } else {
        /* Invalid Exact Match Mode. */
        return (BCM_E_PARAM);
    }

    /* Search Entry First Before Delete. */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, bufp, bufp, 0);
    if (BCM_FAILURE(rv)) {
        if (fc->init == TRUE) {
            return (rv);
        } else {
            /* If init is false it means field detach is happening.
             * which might be due to rc or fp init.
             * since soc init will clear all memories hence
             * exact match memory will be cleared and search will
             * not be a success. Since memory is cleared before itself
             * so delete call should return success in such case.
             */
            rv = BCM_E_NONE;
            return rv;
        }
    }

    /* Get Qos and Action Profile Index Allocated From Entry. */
    if (fg->em_mode == _FieldExactMatchMode128) {
        qp_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE128__QOS_PROFILE_IDf);
        ap_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE128__ACTION_PROFILE_IDf);
    } else if (fg->em_mode == _FieldExactMatchMode160) {
        qp_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE160__QOS_PROFILE_IDf);
        ap_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE160__ACTION_PROFILE_IDf);
    } else if (fg->em_mode == _FieldExactMatchMode320) {
        qp_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE320__QOS_PROFILE_IDf);
        ap_idx = soc_mem_field32_get(unit, mem,
                                     bufp, MODE320__ACTION_PROFILE_IDf);
    } else {
        /* Invalid Exact Mode Mode. */
        return (BCM_E_PARAM);
    }

    /* Delete Entry From HW if found. */
    BCM_IF_ERROR_RETURN(
            soc_mem_delete(unit, mem, MEM_BLOCK_ALL, bufp));

    /* Delete Entry Profile Memories for actions. */
    soc_profile_mem_delete(unit,
                           &stage_fc->qos_action_profile[fg->instance],
                           qp_idx);
    soc_profile_mem_delete(unit,
                           &stage_fc->action_profile[fg->instance],
                           ap_idx);

    /* Free Hw Entry Copy. */
    for (e_part = 0; e_part < parts_count; e_part++) {
        f_ent_part = f_ent + e_part;
        sal_free(f_ent_part->tcam.key_hw);
        f_ent_part->tcam.key_hw = NULL;

        /* Set Entry Flags. */
        f_ent_part->flags |= _FP_ENTRY_DIRTY;
        f_ent_part->flags &= ~_FP_ENTRY_INSTALLED;
        f_ent_part->flags &= ~_FP_ENTRY_ENABLED;
    }

    /* Decrement Hw Slice Entry Count. */
    f_ent->fs->hw_ent_count--;

    return rv;
}

#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
int _th_field_not_empty;
#endif /* !BCM_TOMAHAWK_SUPPORT && !BCM_FIELD_SUPPORT */
