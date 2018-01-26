/*
 * $Id: $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 *
 * File:       field.c
 * Purpose:    BCM56870 Field Processor functions.
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT3_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <bcm/field.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>

static const soc_reg_t efp_classid_selector_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_CLASSID_SELECTOR_PIPE0r,
      EFP_CLASSID_SELECTOR_PIPE1r,
      EFP_CLASSID_SELECTOR_PIPE2r,
      EFP_CLASSID_SELECTOR_PIPE3r,
      EFP_CLASSID_SELECTORr
};
/* supported from Trident3 */
static const soc_reg_t efp_classid_B_selector_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_CLASSID_B_SELECTOR_KEY_TYPE_PIPE0r,
      EFP_CLASSID_B_SELECTOR_KEY_TYPE_PIPE1r,
      INVALIDr,
      INVALIDr,
      EFP_CLASSID_B_SELECTOR_KEY_TYPEr
    };
static const soc_reg_t efp_classid_A_selector_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_CLASSID_SELECTOR_A_PIPE0r,
      EFP_CLASSID_SELECTOR_A_PIPE1r,
      INVALIDr,
      INVALIDr,
      EFP_CLASSID_SELECTOR_Ar
};
static const soc_reg_t efp_key4_dvp_sel_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_KEY4_DVP_SELECTOR_PIPE0r,
      EFP_KEY4_DVP_SELECTOR_PIPE1r,
      EFP_KEY4_DVP_SELECTOR_PIPE2r,
      EFP_KEY4_DVP_SELECTOR_PIPE3r,
      EFP_KEY4_DVP_SELECTORr
};
static const soc_reg_t efp_key4_mdl_sel_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_KEY4_MDL_SELECTOR_PIPE0r,
      EFP_KEY4_MDL_SELECTOR_PIPE1r,
      EFP_KEY4_MDL_SELECTOR_PIPE2r,
      EFP_KEY4_MDL_SELECTOR_PIPE3r,
      EFP_KEY4_MDL_SELECTORr
};
static const soc_reg_t efp_key8_dvp_sel_r[_FP_MAX_NUM_PIPES+1] = {
      EFP_KEY8_DVP_SELECTOR_PIPE0r,
      EFP_KEY8_DVP_SELECTOR_PIPE1r,
      EFP_KEY8_DVP_SELECTOR_PIPE2r,
      EFP_KEY8_DVP_SELECTOR_PIPE3r,
      EFP_KEY8_DVP_SELECTORr
};
static soc_reg_t outer_tpid_r[4] = {
    PARSER1_ING_OUTER_TPID_0r,
    PARSER1_ING_OUTER_TPID_1r,
    PARSER1_ING_OUTER_TPID_2r,
    PARSER1_ING_OUTER_TPID_3r
};
static soc_reg_t inner_tpid_r[4] = {
    PARSER1_ING_INNER_TPID_0r,
    PARSER1_ING_INNER_TPID_1r,
    PARSER1_ING_INNER_TPID_2r,
    PARSER1_ING_INNER_TPID_3r
};
static soc_field_t classfldtbl[][7] = {
                       {SLICE_0_F1f, SLICE_0_F2f, SLICE_0_F3f, SLICE_0_F4f, 
                                     SLICE_0_F6f, SLICE_0_F7f, SLICE_0_F8f},
                       {SLICE_1_F1f, SLICE_1_F2f, SLICE_1_F3f, SLICE_1_F4f, 
                                     SLICE_1_F6f, SLICE_1_F7f, SLICE_1_F8f},
                       {SLICE_2_F1f, SLICE_2_F2f, SLICE_2_F3f, SLICE_2_F4f, 
                                     SLICE_2_F6f, SLICE_2_F7f, SLICE_2_F8f},
                       {SLICE_3_F1f, SLICE_3_F2f, SLICE_3_F3f, SLICE_3_F4f, 
                                     SLICE_3_F6f, SLICE_3_F7f, SLICE_3_F8f}
};
static soc_field_t classIdBfldtbl[4] =
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };
static soc_field_t dvpfldtbl[4] = 
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

static soc_field_t mdlfldtbl[4] = 
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };
/*
 * Function:
 *    _field_td3_ingress_action_profiles_init
 * Purpose:
 *   Initialize action profiles based on group operational mode.
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
int
_field_td3_ingress_action_profiles_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;               /* SOC memory names.         */
    int entry_words;             /* Entry size in words.      */
    int inst;                    /* Instance iterator.        */
    int rv;                      /* Operation return status.  */
    static const soc_mem_t act_prof_mem[_FP_MAX_NUM_PIPES] =
    {
        /* Ingress Action Profile. */
        IFP_POLICY_ACTION_PROFILE_PIPE0m,
        IFP_POLICY_ACTION_PROFILE_PIPE1m
    };

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for Ingress Stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->oper_mode) {

        /* Field Groups operational in Global mode. */
        case bcmFieldGroupOperModeGlobal:

            /* Initialize action profile memory name. */
            mem = IFP_POLICY_ACTION_PROFILEm;

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
                                 IFP_POLICY_ACTION_PROFILE_PIPE0m);

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
 *     _field_td3_common_actions_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 * Returns:
 *     BCM_E_NONE
 */
int
_field_td3_common_actions_init(int unit, _field_stage_t *stage_fc)
{
    _FP_ACTION_DECL
    uint32 offset = 0;                  /* General variable to carry offset. */
    uint32 profile_one_offset = 0;      /* Offset of PROFILE_ONE_SET. */
    uint32 input_priority_set = 0;      /* Offset of CHANGE_INPUT_PRIORITY_SET*/
    uint32 int_cn_set = 0;              /* Offset of CHANGE_INT_CN_SET. */
    uint32 cos_or_int_pri_set = 0;      /* Offset of CHANGE_COS_OR_INT_PRI_SET*/
    uint32 drop_precendence_set = 0;   /* Offset of CHANGE_DROP_PRECEDENCE_SET*/

    uint32 profile_two_offset = 0;      /* Offset of PROFILE_TWO_SET. */
    uint32 dscp_tos_set = 0;            /* Offset of CHANGE_DSCP_TOS_SET. */
    uint32 pkt_pri_set = 0;             /* Offset of CHANGE_PKT_PRI_SET. */
    uint32 ecn_set = 0;                 /* Offset of CHANGE_ECN_SET. */

    uint32 redirect_offset = 0;         /* Offset of REDIRECT_SET. */
    uint32 l3swl2change_offset = 0;     /* Offset of L3SW_CHANGE_L2_SET. */
    uint32 nat_offset = 0;              /* Offset of NAT_SET. */
    uint32 mirror_offset = 0;           /* Offset of MIRROR_SET. */
    uint32 lb_offset = 0;               /* Offset of LB_CONTROL_SET. */
    uint32 nat_override_offset = 0;     /* Offset of NAT_OVERRIDE_SET. */
    uint32 copytocpu_offset = 0;        /* Offset of COPY_TO_CPU_SET. */
    uint32 cutthrough_offset = 0;       /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32 urpf_offset = 0;             /* Offset of URPF_OVERRIDE_SET. */
    uint32 ttl_offset = 0;              /* Offset of TTL_SET. */
    uint32 cpucos_offset = 0;           /* Offset of CHANGE_CPU_COS_SET. */
    uint32 drop_offset = 0;             /* Offset of DROP_SET. */
    uint32 mirror_override_offset = 0;  /* Offset of MIRROR_OVERRIDE_SET. */
    uint32 sflow_offset = 0;            /* Offset of SFLOW_SET. */
    uint32 debug_offset = 0;            /* Offset of INSTRUMENTATION_SET */

    /* New action sets for TD3 */
#if 0
    uint32 ifp_meter_set = 0;            /* Offset of METER_SET. */
    uint32 fcoe_vsan_set = 0;            /* Offset of FCOE_VSAN_SET. */
    uint32 edit_ctrl_id_action_set = 0;  /* Offset of EDIT_CTRL_ID_ACTION_SET.*/
    uint32 ifp_counter_set = 0;          /* Offset of COUNTER_SET. */
    uint32 green_to_pid_set = 0;         /* Offset of GREEN_TO_PID_SET. */
    uint32 ignore_fcoe_zone_check_set = 0;
                             /* Offset of IGNORE_FCOE_ZONE_CHECK_SET. */
    uint32 rx_timestamp_insertion_set = 0;
                             /* Offset of RX_TIMESTAMP_INSERTION_SET. */
    uint32 tx_timestamp_insertion_set = 0; 
                             /* Offset of TX_TIMESTAMP_INSERTION_SET. */
    uint32 opaque_1_action_set = 0;      /* Offset of OPAQUE_1_ACTION_SET. */
    uint32 opaque_2_action_set = 0;      /* Offset of OPAQUE_2_ACTION_SET. */
    uint32 opaque_3_action_set = 0;      /* Offset of OPAQUE_3_ACTION_SET. */
    uint32 opaque_4_action_set = 0;      /* Offset of OPAQUE_4_ACTION_SET. */
    uint32 extraction_ctrl_id_action = 0;
                                /* Offset of EXTRACTION_CTRL_ID_ACTION. */
    uint32 loopback_profile_action_set = 0;
                              /* Offset of LOOPBACK_PROFILE_ACTION_SET. */
#endif

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        /* Offset of CHANGE_INPUT_PRIORITY_SET within the
           profile_set_1. */
        input_priority_set = 51;
        /* Offset of CHANGE_INT_CN_SET within the
           profile_set_1. */
        int_cn_set = 42;
        /* Offset of CHANGE_COS_OR_INT_PRI_SET within the
           profile_set_1. */
        cos_or_int_pri_set = 6;
        /* Offset of CHANGE_DROP_PRECEDENCE_SET within the
           profile_set_1. */
        drop_precendence_set = 0;
        profile_two_offset=56;
        /* Offset of CHANGE_DSCP_TOS_SET within the
           profile_set_2. */
        dscp_tos_set = 27;
        /* Offset of CHANGE_PKT_PRI_SET within the
           profile_set_1. */
        pkt_pri_set = 9;
        /* Offset of CHANGE_ECN_SET within the
           profile_set_2. */
        ecn_set = 0;
    }

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset + input_priority_set;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionUntaggedPacketPriorityNew,
                                      0, _FieldActionChangeInputPrioritySet,
                                      offset + 1, 4, -1, offset + 0, 1, 1);

    offset = profile_one_offset + drop_precendence_set;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 0, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 2, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 4, 2, -1);


    offset = profile_one_offset + cos_or_int_pri_set;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset , 8, -1, offset + 24, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosMapNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset , 2, -1, offset + 24, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCopy,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 24, 4, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioIntNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset , 8, -1, offset + 24, 4, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntTos,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 24, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCancel,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 24, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpUcastCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset , 4, -1, offset + 24, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpMcastCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 4, 4, -1, offset + 24, 4, 9);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 8, 8, -1, offset + 28, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosMapNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 8, 2, -1, offset + 28, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCopy,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 28, 4, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioIntNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 8, 8, -1, offset + 28, 4, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntTos,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 28, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCancel,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 28, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpUcastCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 8, 4, -1, offset + 28, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpMcastCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 12, 4, -1, offset + 28, 4, 9);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosQNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 16, 8, -1, offset + 32, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosMapNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 16, 2, -1, offset + 32, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCopy,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioIntNew,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 16, 8, -1, offset + 32, 4, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntTos,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCancel,
                                               0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpUcastCosQNew,
                                              0, _FieldActionChangeCosOrIntPriSet,
                                              offset + 16, 4, -1, offset + 32, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpMcastCosQNew,
                                              0, _FieldActionChangeCosOrIntPriSet,
                                              offset + 20, 4, -1, offset + 32, 4, 9);

    if (soc_feature(unit, soc_feature_ecn_wred)) {
    offset = profile_one_offset + int_cn_set;
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                           bcmFieldActionGpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           _FieldActionChangeIntCNSet,
                           offset + 4, 2, -1, offset + 8, 1, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                           bcmFieldActionYpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           _FieldActionChangeIntCNSet,
                           offset + 2, 2, -1, offset + 7, 1, 1);
       _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                          bcmFieldActionRpIntCongestionNotificationNew,
                          _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                          _FieldActionChangeIntCNSet,
                          offset, 2, -1, offset + 6, 1, 1);
    }
    if (soc_feature(unit, soc_feature_field_action_pfc_class)) {
       _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                          bcmFieldActionPfcClassNew,
                          0, _FieldActionChangeInputPrioritySet,
                          offset + 1, 4, -1, offset + 0, 1, 1);
    }

    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorIngress,
                                          0, _FieldActionMirrorSet,
                                          offset + 8, 4, 0, offset , 8, 0);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorEgress,
                                          0, _FieldActionMirrorSet,
                                          offset + 8, 4, 0, offset , 8, 0);

    /* IFP_LB_CONTROLS_SET(3b) */
    offset = lb_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionHgTrunkRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionTrunkRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 1, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionEcmpRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 2, 1, 1);

    /* IFP_NAT_OVERRIDE_SET(1b) */
    offset = nat_override_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionNatCancel,
                                 0, _FieldActionNatOverrideSet, offset + 0, 1, 1);
    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 0, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 3, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 3, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 5);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCutThrough,
                                 0, _FieldActionCutThrOverrideSet, offset + 0, 1, 1);

    /* IFP_URPF_OVERRIDE_SET(1b) */
    offset = urpf_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCheckUrpf,
                                 0, _FieldActionUrpfOverrideSet, offset + 0, 1, 1);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotChangeTtl,
                                 0, _FieldActionTtlOverrideSet, offset + 0, 1, 1);

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset + ecn_set;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 0, 2, -1, offset + 6, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 2, 2, -1, offset + 7, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 4, 2, -1, offset + 8, 1, 1);


    offset = profile_two_offset + pkt_pri_set;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioPktNew,
                                       0, _FieldActionChangePktPriSet,
                                       offset , 3, -1, offset + 9, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 7);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 3, 3, -1, offset + 12, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 7);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 6, 3, -1, offset + 15, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 7);


    offset = profile_two_offset + dscp_tos_set;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 13, 6, -1, offset , 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset , 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset , 2, 3);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 7, 6, -1, offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 2, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 2, 2, 3);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 19, 6, -1, offset + 4, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpTosPrecedenceNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 19, 6, -1, offset + 4, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpTosPrecedenceCopy,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 2);

    /* IFP_L3SW_CHANGE_L2_SET(33b) */
    offset = l3swl2change_offset;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionChangeL2Fields,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionChangeL2FieldsCancel,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 19, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionFabricQueue,
                                      0, _FieldActionL3SwChangeL2Set,
                     offset + 0, 2, -1,  offset + 2, 16, -1, offset + 19, 4, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                   _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,
                                   _FieldActionL3SwChangeL2Set,
                                   offset + 0, 15, -1,
                                   offset + 17, 1, 0, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                   _BCM_FIELD_ACTION_L3SWITCH_ECMP,
                                   _FieldActionL3SwChangeL2Set,
                                   offset + 0, 11, -1,
                                   offset + 17, 1, 1, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMultipathHash,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 11, 3, -1, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionL3SwitchCancel,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 19, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressClassSelect,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 0, 4, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionHiGigClassSelect,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 4, 3, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionNewClassId,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 7, 9, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionBFDSessionIdNew,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 0, 12, -1, offset + 19, 4, 9);
    /* IFP_CHANGE_CPU_COS_SET(8b) */
    offset = cpucos_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosQCpuNew,
                                        0, _FieldActionChangeCpuCosSet,
                                        offset + 0, 6, -1, offset + 6, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionServicePoolIdNew, 0,
                                                   _FieldActionChangeCpuCosSet,
                                                              offset + 2, 2, -1,
                                                              offset + 5, 1, 1,
                                                              offset + 6, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionServicePoolIdPrecedenceNew,
                                                              0, _FieldActionChangeCpuCosSet,
                                                              offset + 2, 2, -1,
                                                              offset + 0, 2, -1,
                                                              offset + 4, 2, 3,
                                                              offset + 6, 2, 2);

    /* IFP_DROP_SET(6b) */
    offset = drop_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDrop,
                                 0, _FieldActionDropSet, offset + 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropCancel,
                                 0, _FieldActionDropSet, offset + 0, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDrop,
                                 0, _FieldActionDropSet, offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropCancel,
                                 0, _FieldActionDropSet,  offset + 2, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDrop,
                                 0, _FieldActionDropSet, offset + 4, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropCancel,
                                 0, _FieldActionDropSet, offset + 4, 2, 2);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionMirrorOverride,
                                 0, _FieldActionMirrorOverrideSet, offset + 0, 1, 1);

    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngSampleEnable,
                                 0, _FieldActionSflowSet, offset + 0, 1, 1);

    /* IFP_INSTRUMENTATION_SET*/
    offset = debug_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPacketTraceEnable,
                             0,
                             _FieldActionInstrumentationSet, offset + 0, 1, 1);

    /* IFP_REDIRECT_SET(38b) */
    offset = redirect_offset;

    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                                _FieldActionRedirectSet,
                                         offset + 6, 16, -1, offset + 22 , 1, 0,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirect,
                                                 _BCM_FIELD_ACTION_REDIRECT_DVP,
                                                _FieldActionRedirectSet,
                                          offset + 6, 14, -1,
                                            offset + 3, 3, 6, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                               _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectTrunk,
                                                _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectTrunk,
                                            _BCM_FIELD_ACTION_REDIRECT_DVP,
                                            _FieldActionRedirectSet,
                                          offset + 6, 14, -1,
                                          offset + 3, 3, 6, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                     bcmFieldActionUnmodifiedPacketRedirectPort,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 0,
                                            offset + 3, 3, 1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                            _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP,
                                                _FieldActionRedirectSet,
                        offset + 6, 16, -1, offset + 3, 3, 2, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                                _BCM_FIELD_ACTION_REDIRECT_ECMP,
                                                _FieldActionRedirectSet,
                        offset + 6, 16, -1, offset + 3, 3, 3, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRedirectCancel,
                                   0, _FieldActionRedirectSet, offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectPbmp, 0,
                                          _FieldActionRedirectSet,
                       offset + 6, 10, -1, offset + 23, 2, 0, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectVlan, 0,
                                          _FieldActionRedirectSet,
                        offset + 18, 1, 0, offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectBcastPbmp, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 18, 1, 1,
                                          offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectMcast, 0,
                                           _FieldActionRedirectSet,offset + 6, 14, -1,
                                           offset + 23, 2, 2, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectIpmc, 0,
                                          _FieldActionRedirectSet,offset + 6, 14, -1,
                                          offset + 23, 2, 3, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressMask, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressPortsAdd, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 0, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionFabricEHAddOrUpdate, 0,
                                          _FieldActionRedirectSet, offset + 3, 32, -1,
                                          offset + 35, 3, -1, offset + 0, 3, 6);

    /* IFP_NAT_SET(12b) */
    offset = nat_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionNat,
                                    0, _FieldActionNatSet, offset + 11, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionNatEgressOverride,
                                    0, _FieldActionNatSet,
                                    offset + 0, 10, -1, offset + 10, 1, -1);

    /* ExactMatch Action Class Id (12b) */
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionExactMatchClassId,
                       _BCM_FIELD_ACTION_NO_IFP_SUPPORT, 0, 12, 0);

    /* TD3 New actions initialzation */
    
    /* Enable the below code after TH2 is merged with Master */
#if 0
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionHgTrunkResilientHashCancel,
                                 0, _FieldActionHgtRhActionSet, 1, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTrunkResilientHashCancel,
                                 0, _FieldActionLagRhActionSet, 1, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEcmpResilientHashCancel,
                                 0, _FieldActionEcmp1RhActionSet, 1, 1, 1);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionFibreChanZoneCheckActionCancel,
                                 0, _FieldActionFcoeVsanSet, 0, 1, 1);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionFibreChanIntVsanPri,
                                 0, _FieldActionIgnoreFCOEZoneCheckSet, 0, 1, 1, 1, 3, -1);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionFibreChanVsanId,
                                 0, _FieldActionFcoeVsanSet, 4, 12, -1);

    /* TIMESTAMP_SET(4b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsert,
                                 0, _FieldActionRxTimestampInsertionSet, 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsertCancel,
                                 0, _FieldActionRxTimestampInsertionSet, 0, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsert,
                                 0, _FieldActionTxTimestampInsertionSet, 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsertCancel,
                                 0, _FieldActionTxTimestampInsertionSet, 0, 2, 2);
    /* DLB_HGT_LAG_SET(2b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicTrunkAllEnable,
                                 0, _FieldActionHgtLagDlbActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicTrunkAllCancel,
                                 0, _FieldActionHgtLagDlbActionSet, 1, 1, 1);
#if defined(INCLUDE_L3)
    /* PROTECTION_SWITCHING_SET(1b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRecoverableDropCancel,
                                 0, _FieldActionProtectionSwitchingDropOverrideSet, 0, 1, 1);

    /* DLB_ECMP_SET(2b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpEnable,
                                 0, _FieldActionEcmpDlbActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpCancel,
                                 0, _FieldActionEcmpDlbActionSet, 1, 1, 1);

    /* DLB_ALTERNATE_PATH_CONTROL_SET(10b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgm,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmThreshold,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 7, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmBias,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 4, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmCost,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 1, 3, -1);
#endif /* INCLUDE_L3 */
#endif

/*FCOE_VSAN_SET */
/*EDIT_CTRL_ID_ACTION_SET*/
/*GREEN_TO_PID_SET*/
/*IGNORE_FCOE_ZONE_CHECK_SET*/
/*RX_TIMESTAMP_INSERTION_SET  already present for TH2*/
/*TX_TIMESTAMP_INSERTION_SET already present for TH2*/
/*OPAQUE_1_ACTION_SET*/
/*OPAQUE_2_ACTION_SET*/
/*OPAQUE_3_ACTION_SET*/
/*OPAQUE_4_ACTION_SET*/
/*EXTRACTION_CTRL_ID_ACTION  */
/*LOOPBACK_PROFILE_ACTION_SET  (only for IFP)*/

    /* All actions which have corresponding Colored(Gp/Yp/Rp) Actions needs to be
     * initialized with some default configuration(offset = 0, width = 0, value = 0).
     */
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosQNew,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                 offset + 16, 8, -1, offset + 32, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCopy,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionPrioIntNew,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                 offset + 16, 8, -1, offset + 32, 4, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntTos,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCancel,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                               offset + 32, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionUcastCosQNew,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                 offset + 16, 4, -1, offset + 32, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMcastCosQNew,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                 offset + 20, 4, -1, offset + 32, 4, 9);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet,
                                  offset + 4, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionColorIndependent,
                                 0, _FieldActionGreenToPidSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEcnNew,
                                 0, _FieldActionChangeEcnSet,
                                 offset + 4, 2, -1, offset + 8, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCopy,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 6, 3, -1, offset + 15, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktTos,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCancel,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionDscpNew,
                                 0, _FieldActionChangeDscpTosSet,
                                 offset + 19, 6, -1, offset + 4, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDscpCancel,
                                 0, _FieldActionChangeDscpTosSet,
                                 offset + 4, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCopyToCpu,
                                 0, _FieldActionCopyToCpuSet,
                                 offset + 9, 8, -1, offset + 6, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpuCancel,
                                 0, _FieldActionCopyToCpuSet,
                                 offset + 6, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuCancel,
                                 0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuReinstate,
                                 0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionTimeStampToCpu,
                                 0, _FieldActionCopyToCpuSet,
                                 offset + 9, 8, -1, offset + 6, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTimeStampToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpTimeStampToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpTimeStampToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpTimeStampToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDrop,
                                 0, _FieldActionDropSet, offset + 4, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDropCancel,
                                 0, _FieldActionDropSet, offset + 4, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntCopy,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntNew,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntTos,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntCancel,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntCopy,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntNew,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntTos,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntCancel,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntCopy,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntNew,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntTos,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntCancel,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntCopy,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntNew,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntTos,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntCancel,
                                 0, _FieldActionChangePktPriSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDot1pPreserve,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosMapNew,
                                 0, _FieldActionChangeCosOrIntPriSet,
                                 offset + 16, 2, -1, offset + 32, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDscpPreserve,
                                 0, _FieldActionChangeDscpTosSet,
                                 offset + 4, 3, 5);

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_td3_ingress_entry_policy_mem_install
 * Purpose:
 *     Install the entry actions into the policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     policy_mem   - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 * Returns:
 *     BCM_E_XXX
 */
int
_field_td3_ingress_entry_policy_mem_install(int unit, _field_entry_t *f_ent,
                                           soc_mem_t policy_mem, int tcam_idx)
{
    int               rv=BCM_E_NONE;
                                       /* Operation return status.            */
    int               policy_idx;      /* Policy index in the Policy table.   */
    uint32            abuf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                       /* Action Profile Data Buffer. */
    _field_group_t *fg = NULL;         /* Field Group Control.        */
    ifp_policy_table_entry_t ibuf_nar; /* Narrow Entry Buffer.      */
    ifp_policy_table_wide_entry_t ibuf_wide;    /* Wide Entry Buffer.        */
    uint32 *bufp = NULL;               /* Hardware Buffer Ptr.      */
    uint32 ap_idx;                     /* Action profile Index*/
    uint8 entry_part;                  /* Entry part number */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }
    fg = f_ent->group;
    /* Get Hardware Buffer Pointer and memory identifier. */
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
           !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        bufp = (uint32 *)&ibuf_nar;
    } else {
        bufp = (uint32 *)&ibuf_wide;
    }

    /* Initialize exact match entries. */
    sal_memcpy(bufp, soc_mem_entry_null(unit, policy_mem),
                soc_mem_entry_words(unit, policy_mem) * sizeof(uint32));

    /* (Intra Slice) Double Wide/ Triple wide modes,
     * IFP_TCAM_WIDE have 256 entries
     * but IFP_POLICY_TABLE will have 512 entries.
     */
    policy_idx = tcam_idx;
    _bcm_field_th_entry_flags_to_tcam_part(unit, f_ent->flags,
                                   fg->flags, &entry_part);
    ap_idx = fg->action_profile_idx[entry_part];
    if (ap_idx != -1) {
        /* Set Action Profile Data. */
        rv = _field_th_entry_action_profile_data_set(unit,
                                            f_ent,
                                            &ap_idx, policy_mem,
                                            abuf);
        BCM_IF_ERROR_RETURN(rv);
        /* Write the POLICY Table */
        /* Set Action Data in entry Buffer. */
        soc_mem_field32_set(unit, policy_mem,
                            bufp,
                            DATA_TYPEf, ap_idx);
        soc_mem_field_set(unit, policy_mem,
                            bufp,
                            POLICY_DATAf, abuf);

        rv = soc_th_ifp_mem_write(unit, policy_mem,
                             MEM_BLOCK_ALL, policy_idx, bufp);
    }
    return (rv);
}
/*
 * Function:
 *    _field_td3_presel_qualifiers_init
 *
 * Purpose:
 *    Initialize Field Ingress Stage Preselector Qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_td3_presel_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /*
     * All the qualifiers offset mentioned are the actual individual HW field
     * offset in IFP_LOGICAL_TABLE_SELECT Memory which are starting from bit
     * position 2. "KEY" overlap field in the memory is used during write,
     * hence to accomidate in the field, the offsets of the qualifiers has to
     * be substracted by 2.
     */
    static int8 key_base_off = 2;
    int8 em_base_shift = 0;
    int8 em_rsrvd_shift = 0;
    _FP_QUAL_DECL;


    /* Exact match stage qualifier offset are not same as in IFP presel.
     * Hence during init here we need to take care of offset position
     * accordingly.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        em_base_shift = 12;
        em_rsrvd_shift = 5;
    } 

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_presel_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
        "IFP Preselector qualifiers");
    if (stage_fc->f_presel_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    /* Preselector qualifiers. */
#if 0
    /* SOURCE_CLASS - 32 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifySourceClass,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        1 - key_base_off, 32);
#endif
    /* EXACT_MATCH_LOGICAL_TABLE_ID_0 - 5 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, 
        _bcmFieldQualifyExactMatchGroupClassIdLookup0,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT, 
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        34 - key_base_off, 5);
    /* EXACT_MATCH_LOGICAL_TABLE_ID_1 - 5 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, 
        _bcmFieldQualifyExactMatchGroupClassIdLookup1,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT
        , _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        39 - key_base_off, 5);
    /* RESERVED_0 - 1 */
    /* HG_LOOKUP - 1 */
    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
         44 - key_base_off - em_base_shift + em_rsrvd_shift, 1);
    /* CPU_PKT_PROFILE_KEY_CONTROL - 1 */
#if 0
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualify,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        45 - key_base_off - em_base_shift, 1);
#endif
    /* MIRROR - 1 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        46 - key_base_off - em_base_shift, 1);
    /* HG - 1 */
    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        47 - key_base_off - em_base_shift, 1);
    /* DROP - 1 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        47 - key_base_off - em_base_shift, 1);
    /* PKT_RESOLUTION - 6 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        48 - key_base_off - em_base_shift, 6);
    /* FORWARDING_TYPE - 4 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        54 - key_base_off - em_base_shift, 4);
    /* LOOKUP_STATUS_VECTOR - 19 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+0 - key_base_off - em_base_shift, 3);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+3 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+4 - key_base_off - em_base_shift, 2);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStpState,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+4 - key_base_off - em_base_shift, 2);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+6 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+7 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+8 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+9 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
         _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+10 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+10 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+11 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+12 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+13 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+14 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+15 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+16 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+16 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+16 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+16 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+17 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+17 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+17 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+17 - key_base_off - em_base_shift, 1);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        58+18 - key_base_off - em_base_shift, 1);
    /* SVP_VALID - 1 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        82 - key_base_off - em_base_shift, 1);
    /* MY_STATION_HIT - 1 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        84 - key_base_off - em_base_shift, 1);
   /* TUNNEL_TYPE_LOOPBACK_TYPE - 5 */
    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        80 - key_base_off - em_base_shift, 5);
    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        80 - key_base_off - em_base_shift, 5);
    /* LOOPBACK - 1 */
    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        84 - key_base_off - em_base_shift, 1);
    /* L4_VALID - 1 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        86 - key_base_off - em_base_shift, 1);
    /* L3_TYPE - 5 */
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        87 - key_base_off - em_base_shift, 5);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMixedSrcClassId,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        2 - key_base_off, 32);
    /* RESERVED - 6 */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td3_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_td3_action_params_check(int unit,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
    if (NULL == f_ent ||
        NULL == fa) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id) {
        return BCM_E_INTERNAL;
    }

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
           switch (fa->action) {
              case bcmFieldActionVlanActions:
                  if (f_ent->vlan_action_set == NULL) {
                     return BCM_E_INTERNAL;
                  }
                  BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit,
                                              f_ent->vlan_action_set));
                  return BCM_E_NONE;
                  break;
              default:
                  break;
           }
           break;
        default:
            ;
    }

    return _bcm_field_th_action_params_check(unit, f_ent, fa);
}


/*
 * Function:
 *     _bcm_field_td3_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_action_support_check(int unit,
                                    _field_entry_t *f_ent,
                                    bcm_field_action_t action,
                                    int *result)
{
    if ((NULL == result) || (NULL == f_ent)) {
        return BCM_E_PARAM;
    }

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
             switch (action) {
                  case bcmFieldActionVlanActions:
                  case bcmFieldActionPacketTraceEnable:
                       *result = TRUE;
                       return BCM_E_NONE;
                  case bcmFieldActionOuterVlanCfiNew:
                  case bcmFieldActionOuterVlanAdd:
                  case bcmFieldActionOuterVlanNew:
                  case bcmFieldActionOuterVlanPrioNew:
                  case bcmFieldActionOuterVlanPrioCopyInner:
                  case bcmFieldActionOuterVlanCfiCopyInner:
                  case bcmFieldActionOuterVlanLookup:
                  case bcmFieldActionInnerVlanCfiNew:
                  case bcmFieldActionInnerVlanAdd:
                  case bcmFieldActionInnerVlanNew:
                  case bcmFieldActionInnerVlanPrioNew:
                  case bcmFieldActionInnerVlanPrioCopyOuter:
                  case bcmFieldActionInnerVlanCfiCopyOuter:
                  case bcmFieldActionPrioPktNew:
                  case bcmFieldActionPrioPktAndIntNew:
                       *result = FALSE;
                       return BCM_E_NONE;
                  default:
                       break;
             }
             break;
        default:
             break;
    }

    return _bcm_field_th_action_support_check(unit, f_ent, action, result);
}

/*
 * Function:
 *     _bcm_field_td3_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_td3_action_get(int             unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          int             tcam_idx,
                          _field_action_t *fa,
                          uint32          *buf
                          )
{
    bcm_vlan_action_set_t *actions;
    
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
        case bcmFieldActionVlanActions:
            if ((f_ent->vlan_action_set == NULL) ||
                (fa->hw_index == _FP_INVALID_INDEX)) {
               return BCM_E_CONFIG;
            }
            actions = f_ent->vlan_action_set;
            PolicySet(unit, mem, buf, NEW_INNER_CFIf, actions->new_inner_cfi);
            PolicySet(unit, mem, buf, NEW_INNER_DOT1Pf, actions->new_inner_pkt_prio);
            PolicySet(unit, mem, buf, NEW_OUTER_CFIf, actions->new_outer_cfi);
            PolicySet(unit, mem, buf, NEW_OUTER_DOT1Pf, actions->priority);
            PolicySet(unit, mem, buf, NEW_INNER_VLANf, actions->new_inner_vlan);
            PolicySet(unit, mem, buf, NEW_OUTER_VLANf, actions->new_outer_vlan);
            PolicySet(unit, mem, buf, TAG_ACTION_PROFILE_PTRf, fa->hw_index);
            break;
        case bcmFieldActionPacketTraceEnable:
            PolicySet(unit, mem, buf, INSTRUMENTATION_TRIGGERS_ENABLEf, 0x1);
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_td3_functions_init
 *
 * Purpose:
 *    Set up functions pointers
 *
 * Parameters:
 *    functions - (IN/OUT) Pointer to device specific field module utility
 *                         routines.
 *
 * Returns:
 *     Nothing
 */
void
_bcm_field_td3_functions_init(_field_funct_t *functions)
{
    (void) _bcm_field_th_functions_init(functions);

    functions->fp_action_support_check =
                                            _bcm_field_td3_action_support_check;
    functions->fp_action_params_check  =
                                             _bcm_field_td3_action_params_check;
    functions->fp_vlan_actions_profile_hw_alloc =
                                            _bcm_field_td3_vlan_action_hw_alloc;
    functions->fp_vlan_actions_profile_hw_free =
                                             _bcm_field_td3_vlan_action_hw_free;
    functions->fp_entry_stat_extended_attach =
                                  _bcm_field_td2plus_entry_stat_extended_attach;
    functions->fp_entry_stat_extended_get =
                                     _bcm_field_td2plus_entry_stat_extended_get;
}

/*
 * Function to allocate HW resource for ING_VLAN_TAG_ACTION_PROFILE_TABLE
 * for the given VLAN action set.
 */ 
int _bcm_field_td3_vlan_action_hw_alloc(int unit, _field_entry_t *f_ent)
{
    int               rv;
    uint32            profile_idx;
    _field_action_t  *fa;
     
    if (f_ent == NULL) {
       return BCM_E_PARAM;
    }

    /* Extract action info from the entry structure. */
    for (fa = f_ent->actions;
         ((fa != NULL) && (_FP_ACTION_VALID & fa->flags));
         fa = fa->next) {
        /* Allocate mtp destination only for mirroring actions */
        if (bcmFieldActionVlanActions != fa->action) {
            continue;
        }

        if(_FP_ACTION_MODIFY & fa->flags) {
            fa->old_index = fa->hw_index;
            fa->hw_index = _FP_INVALID_INDEX;
        }

        if (_FP_INVALID_INDEX == fa->hw_index) {
            rv = _bcm_trx_vlan_action_profile_entry_add(unit,
                                      f_ent->vlan_action_set,
                                               &profile_idx);
            if ((BCM_E_RESOURCE == rv) &&
               (_FP_INVALID_INDEX != fa->old_index)) {
               int ref_count = 0;

               /*
                * Retrieve the reference count and if ref_count <= 1,
                * delete the profile and add new profile.
                */ 
               rv = _bcm_trx_vlan_action_profile_ref_count_get(unit,
                                         fa->old_index, &ref_count);
               if (BCM_FAILURE(rv)) {
                  return rv;
               }

                if (ref_count > 1) {
                    return (BCM_E_RESOURCE);
                }

               /*
                * Destroy old profile ONLY if it is not used by other entries.
                */
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                     fa->old_index);
                if (BCM_FAILURE(rv)) {
                   return rv;
                }

                /* Destroy old profile is no longer required. */
                fa->old_index = _FP_INVALID_INDEX;

                /* Reallocate profile for new action. */
                rv = _bcm_trx_vlan_action_profile_entry_add(unit,
                                          f_ent->vlan_action_set,
                                                   &profile_idx);
                if (BCM_FAILURE(rv)) {
                   return rv;
                }
            }
            fa->hw_index = profile_idx;
        }
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function to free HW resource allocated for ING_VLAN_TAG_ACTION_PROFILE_TABLE
 * for the given VLAN action set.
 */ 
int
_bcm_field_td3_vlan_action_hw_free(int unit, _field_entry_t *f_ent, uint32 flags)
{
    _field_action_t *fa;  /* Field action descriptor. */
    int rv = BCM_E_NONE;  /* Operation return status. */

    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return (BCM_E_NONE);
    }

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionVlanActions:
              if ((flags & _FP_ACTION_RESOURCE_FREE) &&
                  (_FP_INVALID_INDEX != fa->hw_index)) {
                  rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                        fa->hw_index);
                  BCM_IF_ERROR_RETURN(rv);
                  fa->hw_index = _FP_INVALID_INDEX;
              }

              if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) &&
                  (_FP_INVALID_INDEX != fa->old_index)) {
                  rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                        fa->old_index);
                  BCM_IF_ERROR_RETURN(rv);
                  fa->old_index = _FP_INVALID_INDEX;
              }
              break;
          default:
              break;
        }
    }
    return (rv);
}

/*
 * Update bcm_vlan_action_set_t structure from HW for a given profile index
 * and FP Policy memory.
 * Note: Policy entry must contain the policy mem data.
 */  
int _bcm_field_td3_vlan_action_profile_entry_get(int unit,
                                                 soc_mem_t policy_mem,
                                                 uint32 *policy_entry,
                                                 uint32 profile_index,
                                                 bcm_vlan_action_set_t *actions)
{
    if (NULL == policy_entry) {
       return BCM_E_PARAM;
    }

    if (actions == NULL) {
       _FP_XGS3_ALLOC(actions, sizeof(bcm_vlan_action_set_t),
                      "FP Entry WB VLAN Action set");
       if (actions == NULL) {
          return BCM_E_MEMORY;
       }
    }

    _bcm_trx_vlan_action_profile_entry_get(unit, actions, profile_index);

    actions->new_inner_pkt_prio = PolicyGet(unit, policy_mem, policy_entry, NEW_INNER_DOT1Pf);
    actions->new_outer_cfi = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_CFIf);
    actions->priority = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_DOT1Pf);
    actions->new_inner_vlan = PolicyGet(unit, policy_mem, policy_entry, NEW_INNER_VLANf);
    actions->new_outer_vlan = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_VLANf);

    /* 
     * COVERITY
     *  The allocated actions memory is assigned to f_ent->vlan_action_set
     *  It will be free when destroying it (bcm_field_entry_destroy)
     */
    /* coverity[leaked_storage] */
    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_td3_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_field_td3_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
         "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

   /* EFP_KEY4(L2 Key) Qualifiers */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 180, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 183, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               194, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     194, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 12
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 222, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 241, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 243, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 245, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 1,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 2,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 3,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 4,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 6,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 7,
		               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207, 15);

    /* EFP_KEY1(IPv4 Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 57, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 65, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 97, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 129, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 139, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 179, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               180, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               188, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 1,
                               0,
                               180, 8, /* DST_PORT */
                               188, 8, /* DST_MODID */
                               0, 0,
                               0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 1,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 2,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 243, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 247, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 1,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 2,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 3,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 4,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 6,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 7,
		                       214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    /* EFP_KEY2 (IPv6 Singlewide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               9, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               73, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 1);
    
#if 0
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrDestPortF2, 0,
                               188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrDestPortF2, 0,
                               196, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrDestPortF2, 1,
                               0,
                               188, 7, /* DST_PORT */
                               196, 8, /* DST_MODID */
                               0, 0,
                               0);
#else
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 188, 8);
#endif

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 1,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 2,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               196, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 224, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 243, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 247, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 1,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 2,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 3,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 4,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 6,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 7,
		                       209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);

    /* EFP_KEY3 (IPv6 Double Wide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 57, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 185, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 201, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 234, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 236, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 1,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 2,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 3,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 4,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 6,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 7,
		                       219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);

    /* FCOE EFP_KEY5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTFabricId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 12, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTValid,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 21, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 29, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 37, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 69, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 93, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 117, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 125, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 127, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 139, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 147, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 162, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 167, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               168, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               176, 8
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 1,
                               0,
                               176, 8, /* DST_PORT */
                               168, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 184, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 199, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 229, 8);


    /*EFP_KEY6 (HiGiG Key)  Qualifiers*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstMulticast, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstMulticastGroupId, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigTrafficClass, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 144, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstModuleGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 136, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstPortGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstModPortGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcModuleGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 120, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcPortGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 112, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcModPortGport, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 112, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLoadBalanceID, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 104, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigColor, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 102, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigExtendedHeaderPresent,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 101, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIntCongestionNotification, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 99, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMirrorOrSwitchPkt,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 88, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressTagged, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 91, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstTrunk, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 95, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstTrunkId, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 92, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressL3SwitchPkt, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 85, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLabelType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 86, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMplsPkt,  
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 84, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLabel, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigReplicationId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigPortFilteringMode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcTrunk,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 45, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigPreserveFlags, 
                 _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                _bcmFieldDevSelDisable, 0, 
                0,
                44, 1, 
                43, 1, 
                0, 0,
                0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressClassificationTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 80, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigEgressMcast,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 95, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigForwardingType, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 90, 5);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVni, 
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 80, 10, 
                 36, 4, 
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMulticastIndex,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpReplicationId, 
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 64, 16, 
                 40, 3, 
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 48, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigDoNotFlags, 
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 46, 1, 
                 45, 1, 
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpLagFailoverPacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 44, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProtectionSwitchingStatus,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 43, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMirrorToVp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 47, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpPreserveFlags, 
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 35, 1, 
                 34, 1, 
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineClassificationTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 80, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineDeferredFlags, 
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 79, 1, 
                 76, 1, 
                 72, 1,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineVxltStatus,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 77, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePktPriNew,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 73, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineDscpNew,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 66, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePreserveDscp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 47, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePreserveDot1p,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineSrcType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 43, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode, 
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 40, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 162, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 170, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 178, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 1,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 214, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 216, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 1,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 2,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 3,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 4,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 6,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 7,
		                       199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);

    /* EFP_KEY7(Loopback Key) qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackQueue,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 116, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 111, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackSrcGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 94, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPktIsVisible,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 93, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackCpuMasqueradePktProfile,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 90, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackTrafficClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackPacketProcessingPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 129, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 138, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 146, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 154, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 158, 2);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 1,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 188, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 190, 1);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 1,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 2,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 3,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 4,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 6,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 7,
		                       173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    /* EFP_KEY8(Bytes After L2 Key) Qualifiers */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL2PayLoad,
                    _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 
                    _bcmFieldSliceSelDisable, 0, 59, 144);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 17, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 23, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 40, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 41, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 49, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 50, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 1,
                               203, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                     _bcmFieldSliceSelEgrClassF8, 2,
                     0,
                     203, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231, 14);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 247, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 249, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 1,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 2,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 3,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 4,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 6,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 7,
		                       216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_td3_egress_selcodes_install
 *
 * Purpose:
 *     Writes the field egress secondary select codes.
 *
 * Parameters:
 *     unit          - (IN) BCM device number
 *     fg            - (IN) Installed group structure.
 *     slice_numb    - (IN) Slice number to set mode for.
 *     selcode_index - (IN) Index to group slecodes.
 *
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */

int
_field_td3_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   int            selcode_idx
                                   )
{
    int             inst;
    uint8           idx; /* Index to EFP keys. */
    _field_sel_t    *sel;
    _field_stage_t  *stage_fc;
    soc_reg_t efp_classid_sel_r;
    uint64 reg_val;

    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    /* Max number of slices on Tomahawk for EFP is 4(slice 0 - slice 3) */
    if (slice_num >= 4)  {
        return BCM_E_INTERNAL;
    }

    sel = &fg->sel_codes[selcode_idx];
    if (sel == NULL) {
       return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                                                 &stage_fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
       inst = _FP_MAX_NUM_PIPES;
    } else {
       inst = fg->instance;
    }
    efp_classid_sel_r = efp_classid_A_selector_r[inst];
    idx = 0;    
    /* Class Id Selector codes for different EFP keys (KEY1 - KEY8) */
    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f1_sel
                                                   ));
    }
    if (sel->egr_classId_B_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f1_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }
    idx++;
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f2_sel
                                                   ));
    }
    if (sel->egr_classId_B_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f2_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }
    idx++;
    if (sel->egr_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f3_sel
                                                   ));
    }
    if (sel->egr_classId_B_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f3_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f4_sel
                                                   ));
    }
    if (sel->egr_classId_B_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f4_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f6_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f6_sel
                                                   ));
    }
    if (sel->egr_classId_B_f6_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f6_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f7_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f7_sel
                                                   ));
    }
    if (sel->egr_classId_B_f7_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f7_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f8_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f8_sel
                                                   ));
    }
    if (sel->egr_classId_B_f8_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      &reg_val
                                      ));
        soc_reg64_field32_set(unit,
                          efp_classid_B_selector_r[inst],
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f8_sel);
        BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      efp_classid_B_selector_r[inst],
                                      REG_PORT_ANY,
                                      idx,
                                      reg_val));
    }

    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_key4_dvp_sel_r[inst],
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key4_dvp_sel
                                                  ));
    }
    /* SELECTOR CODES for EFP_KEY8_DVP_SELECTOR register */
    if (sel->egr_key8_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_key8_dvp_sel_r[inst],
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key8_dvp_sel
                                                  ));
    }

    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register */
    if (sel->egr_key4_mdl_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_key4_mdl_sel_r[inst],
                                                   REG_PORT_ANY,
                                                   mdlfldtbl[slice_num],
                                                   sel->egr_key4_mdl_sel
                                                  ));
    }

    return (BCM_E_NONE);
}
/*    _field_td3_stage_quals_ibus_map_init
 * Purpose:
 *    Initialize Field Ingress Stage Qualifiers to their extractors mapping.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field stage control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_INTERNAL - Invalid CAP Stage ID.
 *    BCM_E_NONE - Success.
 */
int
_field_td3_stage_quals_ibus_map_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_SEC_INFO_DECL;
    /* L0 BUS extractor sections offset:
     *    - 32bit extractor = 0
     *    - 16bit extractor = 608
     *    - 8bit extractor  = 1120
     *    - 4bit extractor  = 1280
     *    - 2bit extractor  = 1400
     */
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocate stage qualifiers extraction mapping array. */
    _FP_XGS3_ALLOC(stage_fc->input_bus.qual_list,
        (_bcmFieldQualifyCount * sizeof(_field_qual_sec_info_t *)),
         "IFP qualifiers");
    if (NULL == stage_fc->input_bus.qual_list) {
        return (BCM_E_MEMORY);
    }

    /* Post MUX qualifiers initialization. */
    /* Not Intializing the SGPP,SGLP and SVP's as post mux qualifiers
	as these can be part of premuxer's and can use SRC_Container_A/B selectors*/
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDevicePortBitmap,
        _FieldKeygenExtSelDisable, 0, 36, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySystemPortBitmap,
        _FieldKeygenExtSelDisable, 0, 36, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySourceGportBitmap,
        _FieldKeygenExtSelDisable, 0, 36, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatDstRealmId,
        _FieldKeygenExtSelDisable, 0, 2, 157);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatNeeded,
        _FieldKeygenExtSelDisable, 0, 1, 158);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDrop,
        _FieldKeygenExtSelDisable, 0, 1, 159);
    /* Exact Match Chaining Post Mux Qualifiers. */
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchActionClassIdLookup0,
        _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0,
        4, 4, 4, 120, 124, 128);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchActionClassIdLookup1,
        _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0,
        4, 4, 4, 132, 136, 140);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, 
        _bcmFieldQualifyExactMatchHitStatusLookup0,
        _FieldKeygenExtSelDisable, 0,
        1, 154);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchHitStatusLookup1,
        _FieldKeygenExtSelDisable, 0,
        1, 155);

    /*Initailizing the offset values to 0 for _FieldExtCtrlSrcDestCont1Sel
	and _FieldExtCtrlSrcDestCont0Sel. Appropirate offsets will be updated during
	group creation depending on 80bit mode or 160bit mode*/

	/*SRC_DST_CONTAINER_0_SEL*/
	/*DGLP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
        _FieldExtCtrlSrcDestCont0Sel, 4, _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0, 16,  1, 0, 0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstPort, _FieldExtCtrlSrcDestCont0Sel, 4,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*DVP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*ECMP 1*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipath, _FieldExtCtrlSrcDestCont0Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipathOverlay, _FieldExtCtrlSrcDestCont0Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    /* ECMP2 */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipathUnderlay, _FieldExtCtrlSrcDestCont0Sel, 8,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*NEXT_HOP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3EgressNextHops, _FieldExtCtrlSrcDestCont0Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3Egress, _FieldExtCtrlSrcDestCont0Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3MulticastGroup, _FieldExtCtrlSrcDestCont0Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*L2MC_GROUP :L2MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL2MulticastGroup, _FieldExtCtrlSrcDestCont0Sel, 11,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*SRC_DST_CONTAINER_1_SEL*/
	/*DGLP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
        _FieldExtCtrlSrcDestCont1Sel, 4, _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0, 16,  1, 0, 0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstPort, _FieldExtCtrlSrcDestCont1Sel, 4,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*DVP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*ECMP 1*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipath, _FieldExtCtrlSrcDestCont1Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipathOverlay, _FieldExtCtrlSrcDestCont1Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    /* ECMP 2 */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipathUnderlay, _FieldExtCtrlSrcDestCont1Sel, 8,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3EgressNextHops, _FieldExtCtrlSrcDestCont1Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3Egress, _FieldExtCtrlSrcDestCont1Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3MulticastGroup, _FieldExtCtrlSrcDestCont1Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*L2MC_GROUP :L2MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL2MulticastGroup, _FieldExtCtrlSrcDestCont1Sel, 11,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);

    /*
     *  L2 qualifiers.
     */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyISid,
        _FieldKeygenExtSelL1E32, 4, 32, 128);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
        _FieldKeygenExtSelL1E32, 3, _FieldKeygenExtSelL1E16, 31, 32, 16, 96,
        496);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
        _FieldKeygenExtSelL1E32, 2, _FieldKeygenExtSelL1E16, 30, 32, 16, 64,
        480);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyL2PayloadFirstEightBytes,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 544,
        576);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                                       _FieldKeygenExtSelL1E16, 25, 16, 400);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                                       _FieldKeygenExtSelL1E16, 25, 12, 400);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                                    _FieldKeygenExtSelL1E16, 25, 12, 1, 400);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                                    _FieldKeygenExtSelL1E16, 25, 13, 3, 400);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                    _FieldKeygenExtSelL1E8, 4, _FieldKeygenExtSelL1E4, 5,
                                                           8, 4, 32, 20);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                             _FieldKeygenExtSelL1E16, 26, 16, 416);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                             _FieldKeygenExtSelL1E16, 26, 12, 416);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                                _FieldKeygenExtSelL1E16, 26, 12, 1, 416);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                                _FieldKeygenExtSelL1E16, 26, 13, 3, 416);

    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
        _FieldKeygenExtSelL1E8, 3, _FieldKeygenExtSelL1E4, 4, 8, 4, 24,
        16);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
        _FieldKeygenExtSelL1E16, 29, 16, 464);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
        _FieldKeygenExtSelL1E2, 12, 1, 24);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
        _FieldKeygenExtSelL1E2, 12, 1, 1, 24);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
        _FieldKeygenExtSelL1E2, 18, 2, 36);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
        _FieldKeygenExtSelL1E2, 19, 2, 38);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
        _FieldKeygenExtSelL1E2, 20, 2, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
        _FieldKeygenExtSelL1E2, 21, 2, 42);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
        _FieldKeygenExtSelL1E2, 22, 2, 44);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
        _FieldKeygenExtSelL1E4, 10, 3, 40);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
        _FieldKeygenExtSelL1E4, 10, 3, 1, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
        _FieldKeygenExtSelL1E4, 11, 2, 44);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyStpState,
        _FieldKeygenExtSelL1E4, 11, 2, 44);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
        _FieldKeygenExtSelL1E4, 11, 2, 1, 44);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
        _FieldKeygenExtSelL1E4, 11, 3, 1, 44);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
        _FieldKeygenExtSelL1E4, 12, 1, 48);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
        _FieldKeygenExtSelL1E4, 12, 1, 1, 48);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
        _FieldKeygenExtSelL1E4, 12, 2, 1, 48);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
        _FieldKeygenExtSelL1E4, 12, 2, 1, 48);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
        _FieldKeygenExtSelL1E4, 12, 3, 1, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
        _FieldKeygenExtSelL1E4, 13, 4, 52);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
        _FieldKeygenExtSelL1E4, 13, 1, 1, 52);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
        _FieldKeygenExtSelL1E4, 13, 2, 1, 52);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
        _FieldKeygenExtSelL1E4, 13, 3, 1, 52);
    /* L1E4, 15 for Lookup status vector 5 */
    /* L1E4, 16 for FLOW_1_HDR_VALID_CHK_RES_FOR_KEYGEN */
    /* L1E4, 17 for FLOW_2_HDR_VALID_CHK_RES_FOR_KEYGEN */
    /* NV GRE */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2GreVsid,
                                    _FieldKeygenExtSelL1E32, 4, 24, 128);
    /* MPLS. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelAction, _FieldKeygenExtSelL1E4,
        29, 4, 116);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabel, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabel, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
        _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
        _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelId, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E32, 0, 12, 20, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelId, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 12, 20, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
        _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        12, 4, 0, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
        _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        12, 4, 0, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelTtl, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E32, 0, 8, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelTtl, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 8, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelTtl, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E16, 2, 8, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelTtl, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelExp, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E32, 0, 9, 3, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelExp, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 9, 3, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelExp, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E16, 2, 9, 3, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelExp, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4, 9, 3, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelBos, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E32, 0, 8, 1, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelBos, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 8, 1, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelBos, _FieldExtCtrlAuxTagBSel, 3,
        _FieldKeygenExtSelL1E16, 2, 8, 1, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelBos, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4, 8, 1, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsControlWord, _FieldExtCtrlAuxTagBSel, 4,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsControlWord, _FieldExtCtrlAuxTagASel, 4,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
        _FieldExtCtrlAuxTagBSel, 4,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
        _FieldExtCtrlAuxTagASel, 4,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    /*L2 Payload for non-ip packets 32 bytes */
    _FP_QUAL_EIGHT_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2PayLoad,
        _FieldKeygenExtSelL1E32, 11, _FieldKeygenExtSelL1E32, 12,
        _FieldKeygenExtSelL1E32, 13, _FieldKeygenExtSelL1E32, 14,
        _FieldKeygenExtSelL1E32, 15, _FieldKeygenExtSelL1E32, 16,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18,
        32, 32, 32, 32, 32, 32, 32, 32, 352, 384, 416, 448, 480, 512, 544, 576); 
    /*
     *  L3 IPv6 qualifiers.
     */
    _FP_QUAL_FOUR_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
        _FieldKeygenExtSelL1E32, 11, _FieldKeygenExtSelL1E32, 12,
        _FieldKeygenExtSelL1E32, 13, _FieldKeygenExtSelL1E32, 14, 32, 32, 32,
        32, 352, 384, 416, 448);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Low,
        _FieldKeygenExtSelL1E32, 11, _FieldKeygenExtSelL1E32, 12, 32, 32, 352,
        384);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
        _FieldKeygenExtSelL1E32, 13, _FieldKeygenExtSelL1E32, 14, 32, 32, 416,
        448);
    _FP_QUAL_FOUR_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
        _FieldKeygenExtSelL1E32, 15, _FieldKeygenExtSelL1E32, 16,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 32,
        32, 480, 512, 544, 576);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Low,
        _FieldKeygenExtSelL1E32, 15, _FieldKeygenExtSelL1E32, 16, 32, 32, 480,
        512);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 544,
        576);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
        _FieldKeygenExtSelL1E8, 14, 8, 112);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
        _FieldKeygenExtSelL1E8, 15, 8, 120);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyIp6FlowLabel, _FieldExtCtrlAuxTagBSel, 9,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyIp6FlowLabel, _FieldExtCtrlAuxTagASel, 9,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
            _FieldExtCtrlAuxTagASel, 9,
            _FieldKeygenExtSelL1E16, 4,
            _FieldKeygenExtSelL1E16, 5,
            16, 16, 64, 80);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
            _FieldExtCtrlAuxTagBSel, 9,
            _FieldKeygenExtSelL1E16, 2,
            _FieldKeygenExtSelL1E16, 3,
            16, 16, 32, 48);
    /*
     *  L3 IPv4 qualifiers.
     */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
        _FieldKeygenExtSelL1E32, 11, 32, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
        _FieldKeygenExtSelL1E32, 15, 32, 480);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTos, _FieldExtCtrlTosFnSel, 2,
            _FieldKeygenExtSelL1E8, 11, 8, 88);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtl, _FieldExtCtrlTtlFnSel, 2,
            _FieldKeygenExtSelL1E8, 12, 8, 96);
    /* bcmFieldQualifyIpProtocol == bcmFieldQualifyIp6NextHeader. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
            _FieldKeygenExtSelL1E8, 13, 8, 104);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
            _FieldExtCtrlTcpFnSel, 2,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_TWO_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyIpType,
        _FieldKeygenExtSelL1E4, 1, _FieldKeygenExtSelL1E2, 3, 0, 4, 0, 1, 4,
        6);
    /* Bitpos_33 maps to this qualifier in LO bus. */
#if 0
    /*removed from td3 reg file */
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
        _FieldKeygenExtSelL1E2, 4, 1, 1, 8);
#endif
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIcmpError,
        _FieldKeygenExtSelL1E2, 4, 1, 8);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
        _FieldKeygenExtSelL1E2, 13, 1, 1, 26);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
        _FieldKeygenExtSelL1E2, 15, 2, 30);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
        _FieldKeygenExtSelL1E2, 16, 2, 32);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpFragNonOrFirst,
        _FieldKeygenExtSelL1E2, 6, 2, 12);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyColor,
        _FieldKeygenExtSelL1E2, 23, 2, 46);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyGenericAssociatedChannelLabelValid,
        _FieldKeygenExtSelL1E2, 24, 1, 48);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyRouterAlertLabelValid,
        _FieldKeygenExtSelL1E2, 24, 1, 1, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
        _FieldKeygenExtSelL1E2, 25, 2, 50);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
        _FieldKeygenExtSelL1E2, 21, 2, 52);

    /* L4 qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
        _FieldKeygenExtSelL1E16, 27, 16, 432);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
        _FieldKeygenExtSelL1E16, 28, 16, 448);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
        _FieldKeygenExtSelL1E16, 28, 16, 448);

    /*
     *  FCoE qualifiers.
     */
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
        _FieldKeygenExtSelL1E32, 11, 8, 24, 352);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
        _FieldKeygenExtSelL1E32, 11, 3, 3, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
        _FieldKeygenExtSelL1E32, 11, 3, 352);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
        _FieldKeygenExtSelL1E32, 12, 8, 24, 384);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
        _FieldKeygenExtSelL1E32, 12, 8, 384);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
        _FieldKeygenExtSelL1E32, 15, 8, 24, 480);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
        _FieldKeygenExtSelL1E32, 15, 8, 480);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
        _FieldKeygenExtSelL1E8, 11, 8, 88);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
        _FieldKeygenExtSelL1E8, 12, 8, 96);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyFibreChanVFTPri, _FieldExtCtrlAuxTagDSel, 7,
        _FieldKeygenExtSelL1E16, 0, 12, 3, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyFibreChanVFTFabricId, _FieldExtCtrlAuxTagDSel, 7,
        _FieldKeygenExtSelL1E16, 0, 12, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyFibreChanVFTPri, _FieldExtCtrlAuxTagCSel, 7,
        _FieldKeygenExtSelL1E16, 1, 12, 3, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyFibreChanVFTFabricId, _FieldExtCtrlAuxTagCSel, 7,
        _FieldKeygenExtSelL1E16, 1, 12, 16);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
        _FieldKeygenExtSelL1E8, 13, 8, 104);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
        _FieldKeygenExtSelL1E8, 15, 8, 120);

    /* Switch - Port and Interface qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInPort,
        _FieldKeygenExtSelL1E8, 0, 8, 0);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVpn,
        _FieldKeygenExtSelL1E8, 1, _FieldKeygenExtSelL1E4, 0,
        _FieldKeygenExtSelL1E2, 1, 8, 4, 2, 8, 0, 2);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVrf,
        _FieldKeygenExtSelL1E8, 2, _FieldKeygenExtSelL1E4, 32, 8, 4, 16,
        128);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContASel, 0,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContASel, 1,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContASel, 2,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContASel, 3,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContASel, 4,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContASel, 5,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceLookupClassPort, _FieldExtCtrlClassIdContASel, 6,
        _FieldKeygenExtSelL1E16, 15, 8, 8, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceIngressKeySelectClassPort, _FieldExtCtrlClassIdContASel, 6,
        _FieldKeygenExtSelL1E16, 15, 8, 240);
#if 0 
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceIngressKeySelectClassPort, _FieldExtCtrlClassIdContASel, 6,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
#endif
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContASel, 7,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
#if 0 
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContASel, 7,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
#endif

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContBSel, 0,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContBSel, 1,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContBSel, 2,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContBSel, 3,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContBSel, 4,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContBSel, 5,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceLookupClassPort, _FieldExtCtrlClassIdContBSel, 6,
        _FieldKeygenExtSelL1E16, 14, 8, 8, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceIngressKeySelectClassPort, _FieldExtCtrlClassIdContBSel, 6,
        _FieldKeygenExtSelL1E16, 14, 8, 224);
#if 0 
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceIngressKeySelectClassPort, _FieldExtCtrlClassIdContBSel, 6,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
#endif
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContBSel, 7,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
#if 0 
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContBSel, 7,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
#endif
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyGroupClass,
        _FieldKeygenExtSelL1E8, 5, 6, 2, 40);
    /*Container_A_SEL*/
    /*SGPP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModPortGport,
            _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModPortGports,
            _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcPort,
            _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModuleGport,
            _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            8, 8, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcTrunkMemberGport,
            _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 1, 1, 160, 4);
    /*SGPP -*/
    /*SGLP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcTrunk,
            _FieldExtCtrlSrcContASel, 1, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 1, 1, 160, 4);
    /*SGLP -*/
    /* SVP */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMplsGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMplsGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMimGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMimGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcWlanGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcWlanGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVlanGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVlanGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVxlanGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVxlanGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcNivGport,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcNivGports,
            _FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 1, 1, 160, 4);
    /*SVP - */
    /*Container_B_SEL*/
    /*SGPP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModPortGport,
            _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2, 
            0, 16, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModPortGports,
            _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcPort,
            _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcModuleGport,
            _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            8, 8, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcTrunkMemberGport,
            _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 0, 1, 176, 4);

    /*SGPP -*/
    /*SGLP +*/

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcTrunk,
            _FieldExtCtrlSrcContBSel, 1, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 16, 0, 1, 176, 4);

    /*SGLP -*/
    /* SVP */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMplsGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMplsGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMimGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcMimGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcWlanGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcWlanGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVlanGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVlanGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVxlanGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcVxlanGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcNivGport,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
            bcmFieldQualifySrcNivGports,
            _FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
            _FieldKeygenExtSelL1E2, 2,
            0, 14, 0, 1, 176, 4);

    /*SVP - */

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContCSel, 0,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 1,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContCSel, 2,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 3,
        _FieldKeygenExtSelL1E16, 13, 16, 208);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContDSel, 0,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 1,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContDSel, 2,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 3,
        _FieldKeygenExtSelL1E16, 12, 16, 192);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
        _FieldKeygenExtSelL1E2, 8, 1, 16);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyDestVirtualPortValid,
        _FieldKeygenExtSelL1E2, 8, 1, 1, 16);
 
    /* HINTS Required: Acutal qualifier size is more than the 
     * available section size */
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContASel, 0,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContASel, 1,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContASel, 2,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContASel, 3,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContASel, 4,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContASel, 5,
        _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
                    bcmFieldQualifyInterfaceIngressKeySelectClassPort, 
                                      _FieldExtCtrlClassIdContASel, 6,
                                     _FieldKeygenExtSelL1E4, 9, 4, 36);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContASel, 7,
        _FieldKeygenExtSelL1E4, 9, 4, 36);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContASel, 0,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContASel, 1,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContASel, 2,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContASel, 3,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContASel, 4,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContASel, 5,
        _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
                    bcmFieldQualifyInterfaceIngressKeySelectClassPort, 
                                      _FieldExtCtrlClassIdContASel, 6,
                                    _FieldKeygenExtSelL1E8, 9, 8, 72);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
             bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContASel, 7,
                                     _FieldKeygenExtSelL1E8, 9, 8, 72);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContBSel, 0,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContBSel, 1,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContBSel, 2,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContBSel, 3,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContBSel, 4,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContBSel, 5,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceIngressKeySelectClassPort, _FieldExtCtrlClassIdContBSel, 6,
        _FieldKeygenExtSelL1E4, 8, 4, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContBSel, 7,
        _FieldKeygenExtSelL1E4, 8, 4, 32);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContBSel, 0,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContBSel, 1,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContBSel, 2,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContBSel, 3,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContBSel, 4,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContBSel, 5,
        _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
                    bcmFieldQualifyInterfaceIngressKeySelectClassPort,
                                      _FieldExtCtrlClassIdContBSel, 6,
                                    _FieldKeygenExtSelL1E8, 8, 8, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
             bcmFieldQualifyUdfClass, _FieldExtCtrlClassIdContBSel, 7,
                                     _FieldKeygenExtSelL1E8, 8, 8, 64);

     _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContCSel, 0,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 1,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContCSel, 2,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 3,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContCSel, 4,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 5,
        _FieldKeygenExtSelL1E4, 7, 4, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 6,
        _FieldKeygenExtSelL1E4, 7, 4, 28);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContCSel, 0,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 1,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContCSel, 2,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 3,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContCSel, 4,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 5,
        _FieldKeygenExtSelL1E8, 7, 8, 56);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 6,
        _FieldKeygenExtSelL1E8, 7, 8, 56);

    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContCSel, 4,
        _FieldKeygenExtSelL1E16, 13, 0, 8, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 5,
        _FieldKeygenExtSelL1E16, 13, 0, 8, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 6,
        _FieldKeygenExtSelL1E16, 13, 0, 8, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContCSel, 4,
        _FieldKeygenExtSelL1E16, 13, 8, 8, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContCSel, 5,
        _FieldKeygenExtSelL1E16, 13, 8, 8, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContCSel, 6,
        _FieldKeygenExtSelL1E16, 13, 8, 8, 208);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContDSel, 0,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 1,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContDSel, 2,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 3,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContDSel, 4,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 5,
        _FieldKeygenExtSelL1E4, 6, 4, 24);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 6,
        _FieldKeygenExtSelL1E4, 6, 4, 24);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContDSel, 0,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 1,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContDSel, 2,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 3,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContDSel, 4,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 5,
        _FieldKeygenExtSelL1E8, 6, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 6,
        _FieldKeygenExtSelL1E8, 6, 8, 48);

    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContDSel, 4,
        _FieldKeygenExtSelL1E16, 12, 0, 8, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 5,
        _FieldKeygenExtSelL1E16, 12, 0, 8, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 6,
        _FieldKeygenExtSelL1E16, 12, 0, 8, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContDSel, 4,
        _FieldKeygenExtSelL1E16, 12, 8, 8, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContDSel, 5,
        _FieldKeygenExtSelL1E16, 12, 8, 8, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContDSel, 6,
        _FieldKeygenExtSelL1E16, 12, 8, 8, 192);

    /* Meta data qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
        _FieldKeygenExtSelL1E16, 17, 16, 272);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVrf,
        _FieldKeygenExtSelL1E16, 17, 12, 272);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVpn,
        _FieldKeygenExtSelL1E16, 17, 16, 272);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
        _FieldKeygenExtSelL1E16, 18, 13, 288);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
        _FieldKeygenExtSelL1E16, 23, _FieldKeygenExtSelL1E16, 24, 16, 16, 368,
        384);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
        _FieldKeygenExtSelL1E8, 10, 6, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyPacketLength, _FieldExtCtrlAuxTagDSel, 2,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyPacketLength, _FieldExtCtrlAuxTagCSel, 2,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
        _FieldKeygenExtSelL1E4, 14, 0, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
        _FieldKeygenExtSelL1E4, 14, 0, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
        _FieldKeygenExtSelL1E4, 14, 0, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
        _FieldKeygenExtSelL1E4, 14, 0, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
        _FieldKeygenExtSelL1E4, 14, 1, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
        _FieldKeygenExtSelL1E4, 14, 1, 1, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
        _FieldKeygenExtSelL1E4, 14, 1, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
        _FieldKeygenExtSelL1E4, 14, 1, 56);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
        _FieldKeygenExtSelL1E4, 14, 2, 1, 56);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
        _FieldKeygenExtSelL1E4, 18, 4, 72);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
        _FieldKeygenExtSelL1E4, 30, 4, 120);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
        _FieldKeygenExtSelL1E4, 31, 4, 124);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
        _FieldKeygenExtSelL1E2, 5, 1, 1, 10);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyCpuVisibilityPacket,
        _FieldKeygenExtSelL1E2, 5, 1, 10);

    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4Ports, _FieldExtCtrlAuxTagDSel, 0,
        _FieldKeygenExtSelL1E16, 0, 1, 1, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4Ports, _FieldExtCtrlAuxTagCSel, 0,
        _FieldKeygenExtSelL1E16, 1, 1, 1, 16);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
        _FieldKeygenExtSelL1E2, 6, 1, 1, 12);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
        _FieldKeygenExtSelL1E2, 17, 1, 34);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
        _FieldKeygenExtSelL1E2, 17, 1, 1, 34);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatSrcRealmId,
        _FieldKeygenExtSelL1E2, 0, 2, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyIntCongestionNotification,
        _FieldKeygenExtSelL1E2, 7, 2, 14);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
        _FieldKeygenExtSelL1E2, 14, 1, 1, 28);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
        _FieldKeygenExtSelL1E2, 14, 1, 28);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagDSel, 3,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagDSel, 4,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagDSel, 5,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagDSel, 6,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagCSel, 3,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagCSel, 4,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagCSel, 5,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagCSel, 6,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagBSel, 5,
        _FieldKeygenExtSelL1E16, 3, 16, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagBSel, 5,
        _FieldKeygenExtSelL1E16, 2, 16, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagBSel, 6,
        _FieldKeygenExtSelL1E16, 3, 16, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagBSel, 6,
        _FieldKeygenExtSelL1E16, 2, 16, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagASel, 5,
        _FieldKeygenExtSelL1E16, 5, 16, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagASel, 5,
        _FieldKeygenExtSelL1E16, 4, 16, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagASel, 6,
        _FieldKeygenExtSelL1E16, 5, 16, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagASel, 6,
        _FieldKeygenExtSelL1E16, 4, 16, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagBSel, 5,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagBSel, 6,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagASel, 5,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagASel, 6,
        _FieldKeygenExtSelL1E32, 1, 32, 32);

    /* VXLAN qualifiers. */
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanNetworkId, _FieldExtCtrlAuxTagBSel, 7,
        _FieldKeygenExtSelL1E32, 0, 24, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanNetworkId, _FieldExtCtrlAuxTagASel, 7,
        _FieldKeygenExtSelL1E32, 1, 24, 32);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
        _FieldKeygenExtSelL1E32, 4, 24, 128);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
        _FieldExtCtrlAuxTagBSel, 7,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 8, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
        _FieldExtCtrlAuxTagASel, 7,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 8, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanFlags, _FieldExtCtrlAuxTagBSel, 7,
        _FieldKeygenExtSelL1E32, 0, 24, 8, 0);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
        _FieldKeygenExtSelL1E32, 4, 24, 8, 128);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanFlags, _FieldExtCtrlAuxTagASel, 7,
        _FieldKeygenExtSelL1E32, 1, 24, 8, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
        _FieldExtCtrlAuxTagBSel, 7,
        _FieldKeygenExtSelL1E16, 3,
        8, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
        _FieldExtCtrlAuxTagASel, 7,
        _FieldKeygenExtSelL1E16, 5,
        8, 8, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVnTag, _FieldExtCtrlAuxTagBSel, 0,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVnTag, _FieldExtCtrlAuxTagASel, 0,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
        _FieldExtCtrlAuxTagBSel, 0,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
        _FieldExtCtrlAuxTagASel, 0,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyETag, _FieldExtCtrlAuxTagBSel, 0,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyETag, _FieldExtCtrlAuxTagASel, 0,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyETag,
        _FieldExtCtrlAuxTagBSel, 0,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyETag,
        _FieldExtCtrlAuxTagASel, 0,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyCnTag, _FieldExtCtrlAuxTagBSel, 1,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyCnTag, _FieldExtCtrlAuxTagASel, 1,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
        _FieldExtCtrlAuxTagBSel, 1,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
        _FieldExtCtrlAuxTagASel, 1,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyFabricQueueTag, _FieldExtCtrlAuxTagBSel, 2,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyFabricQueueTag, _FieldExtCtrlAuxTagASel, 2,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
        _FieldExtCtrlAuxTagBSel, 2,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
        _FieldExtCtrlAuxTagASel, 2,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanHeaderBits8_31, _FieldExtCtrlAuxTagBSel, 8,
        _FieldKeygenExtSelL1E32, 0, 24, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanHeaderBits8_31, _FieldExtCtrlAuxTagASel, 8,
        _FieldKeygenExtSelL1E32, 1, 24, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
        _FieldExtCtrlAuxTagBSel, 8,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 8, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
        _FieldExtCtrlAuxTagASel, 8,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 8, 64, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanHeaderBits56_63, _FieldExtCtrlAuxTagBSel, 8,
        _FieldKeygenExtSelL1E32, 0, 24, 8, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyVxlanHeaderBits56_63, _FieldExtCtrlAuxTagASel, 8,
        _FieldKeygenExtSelL1E32, 1, 24, 8, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
        _FieldExtCtrlAuxTagBSel, 8,
        _FieldKeygenExtSelL1E16, 3,
        8, 8, 48);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
        _FieldExtCtrlAuxTagASel, 8,
        _FieldKeygenExtSelL1E16, 5,
        8, 8, 80);

    /* Class Qualifiers */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxID,
            _FieldKeygenExtSelL1E16, 27, 16, 432);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxID,
            _FieldKeygenExtSelL1E16, 28, 16, 448);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4DstPortClass,
            _FieldKeygenExtSelL1E4, 19, 4, 76);
    /* removed from Td3 reg file */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxIDClass,
            _FieldKeygenExtSelL1E4, 19, 4, 76);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPortClass,
            _FieldKeygenExtSelL1E4, 20, 4, 80);
    /* removed from Td3 reg file */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxIDClass,
            _FieldKeygenExtSelL1E4, 20, 4, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyEtherTypeClass,
            _FieldKeygenExtSelL1E4, 21, 4, 84);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolClass,
            _FieldKeygenExtSelL1E4, 22, 4, 88);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassMsbNibble,
            _FieldKeygenExtSelL1E4, 23, 4, 92);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassMsbNibble,
            _FieldKeygenExtSelL1E4, 23, 4, 92);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassMsbNibble,
            _FieldKeygenExtSelL1E4, 23, 4, 92);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassMsbNibble,
            _FieldKeygenExtSelL1E4, 24, 4, 96);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassMsbNibble,
            _FieldKeygenExtSelL1E4, 24, 4, 96);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassMsbNibble,
            _FieldKeygenExtSelL1E4, 24, 4, 96);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClass,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 23, 16, 16, 4, 336, 352, 92);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Class,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 23, 16, 16, 4, 336, 352, 92);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClass,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 23, 16, 16, 4, 336, 352, 92);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClass,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 24, 16, 16, 4, 304, 320, 96);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Class,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 24, 16, 16, 4, 304, 320, 96);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClass,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 24, 16, 16, 4, 304, 320, 96);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
            _FieldExtCtrlTcpFnSel, 0,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
            _FieldExtCtrlTcpFnSel, 1,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTosClassZero, _FieldExtCtrlTosFnSel, 0,
            _FieldKeygenExtSelL1E8, 11, 8, 88);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTosClassOne, _FieldExtCtrlTosFnSel, 1,
            _FieldKeygenExtSelL1E8, 11, 8, 88);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtlClassZero, _FieldExtCtrlTtlFnSel, 0,
            _FieldKeygenExtSelL1E8, 12, 8, 96);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtlClassOne, _FieldExtCtrlTtlFnSel, 1,
            _FieldKeygenExtSelL1E8, 12, 8, 96);

    /* UDF1 and UDF2 qualifiers. */
    
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData0,
        _FieldKeygenExtSelL1E16, 9, 16, 144);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData1,
        _FieldKeygenExtSelL1E16, 8, 16, 128);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData2,
        _FieldKeygenExtSelL1E32, 10, 32, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData3,
        _FieldKeygenExtSelL1E32, 9, 32, 288);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData4,
        _FieldKeygenExtSelL1E32, 8, 32, 256);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData5,
        _FieldKeygenExtSelL1E16, 7, 16, 112);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData6,
        _FieldKeygenExtSelL1E16, 6, 16, 96);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData7,
        _FieldKeygenExtSelL1E32, 7, 32, 224);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData8,
        _FieldKeygenExtSelL1E32, 6, 32, 192);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData9,
        _FieldKeygenExtSelL1E32, 5, 32, 160);

    /* BFD qualifiers. */
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyBfdYourDiscriminator, _FieldExtCtrlAuxTagBSel, 10,
        _FieldKeygenExtSelL1E32, 0, 32, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyBfdYourDiscriminator, _FieldExtCtrlAuxTagASel, 10,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyBfdYourDiscriminator,
        _FieldExtCtrlAuxTagBSel, 10,
        _FieldKeygenExtSelL1E16, 2,
        _FieldKeygenExtSelL1E16, 3,
        16, 16, 32, 48);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyBfdYourDiscriminator,
        _FieldExtCtrlAuxTagASel, 10,
        _FieldKeygenExtSelL1E16, 4,
        _FieldKeygenExtSelL1E16, 5,
        16, 16, 64, 80);

    /* Exact Match Chaining Qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchGroupClassIdLookup0,
        _FieldKeygenExtSelL1E16, 16,
        5, 256);
    _FP_QUAL_SEC_INFO_OFFSET_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchGroupClassIdLookup1,
        _FieldKeygenExtSelL1E16, 16,
        8, 5, 256);
     
    /* Preselector Logical Table ID Qualifier. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyPreLogicalTableId,
        _FieldKeygenExtSelL1E8, 5, 5, 40);

    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4SrcPortClass, _FieldExtCtrlAuxTagDSel, 0,
        _FieldKeygenExtSelL1E16, 0, 6, 4, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4SrcPortClass, _FieldExtCtrlAuxTagCSel, 0,
        _FieldKeygenExtSelL1E16, 1, 6, 4, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4DstPortClass, _FieldExtCtrlAuxTagDSel, 0,
        _FieldKeygenExtSelL1E16, 0, 2, 4, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyL4DstPortClass, _FieldExtCtrlAuxTagCSel, 0,
        _FieldKeygenExtSelL1E16, 1, 2, 4, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyIpProtocolClass, _FieldExtCtrlAuxTagDSel, 0,
        _FieldKeygenExtSelL1E16, 0, 10, 4, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyIpProtocolClass, _FieldExtCtrlAuxTagCSel, 0,
        _FieldKeygenExtSelL1E16, 1, 10, 4, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyIpFragNonOrFirst, _FieldExtCtrlAuxTagCSel, 0,
        _FieldKeygenExtSelL1E16, 1, 0, 1, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_OFFSET_ADD(unit, stage_fc,
        bcmFieldQualifyIpFragNonOrFirst, _FieldExtCtrlAuxTagDSel, 0,
        _FieldKeygenExtSelL1E16, 0, 0, 1, 0);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_td3_default_policer_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number.
 *     stage_fc  - (IN)Stage control structure.
 *     level     - (IN)Policer level.
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
static int
_field_td3_default_policer_set(int unit, _field_stage_t *stage_fc,
                               int level, _field_entry_t *f_ent, uint32 *buf)
{
    _bcm_field_action_offset_t  meter_info_offset; /* Meter Set for basic Meter info. */

    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }

      sal_memset(&meter_info_offset, 0x0, sizeof(meter_info_offset));

      /* METER_PAIR_MODE */
       meter_info_offset.offset[1] = 1;
       meter_info_offset.width[1]  = 3;
       meter_info_offset.value[1]  = 0;

       /* METER_PAIR_MODE_MODIFIER */
       meter_info_offset.offset[2] = 0; 
       meter_info_offset.width[2]  = 1;
       meter_info_offset.value[2]  = 1;

       ACTION_SET(unit, f_ent, buf, &meter_info_offset);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_td3_ingress_policer_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number.
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
int
_field_td3_ingress_policer_action_set(int unit, _field_entry_t *f_ent,
                                     uint32 *buf)
{
    _field_entry_policer_t      *f_ent_pl;  /* Field entry policer descriptor.*/
    _field_stage_t              *stage_fc;  /* Stage field control structure. */
    _field_policer_t            *f_pl;      /* Field policer descriptor.      */
    int                         idx;        /* Policers levels iterator.      */
    uint32                      meter_pair_mode = BCM_FIELD_METER_MODE_DEFAULT;
                                            /* Hw meter usage bits.      */
    int                         meter_pair_idx;  /* Meter pair index.         */
    int                         rv;              /* Operation return status.  */
    _bcm_field_action_offset_t  meter_offset;    /* Meter Set for ODD_EVEN    */
    _bcm_field_action_offset_t  meter_info_offset;/* Meter Set for basic info */
    int                         instance;    /*  Current Pipe instance        */
    _field_stage_id_t      stage_id;  /* Stage id used for metering.          */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    stage_id = (_BCM_FIELD_STAGE_EXACTMATCH == f_ent->group->stage_id) ? \
               _BCM_FIELD_STAGE_INGRESS : f_ent->group->stage_id;

    /* Get stage control structure. */
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the current pipe instance from group */
    instance = f_ent->group->instance;

    for (idx = 0; idx < _FP_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = f_ent->policer + idx;

        if ((idx > 0)) {
            /* Trident device does not support Level1 policers */
            break;
        }

        if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
            /* Install preserve the color policer. */
            rv = _field_td3_default_policer_set(unit, stage_fc, idx,
                                                f_ent, buf);
            BCM_IF_ERROR_RETURN(rv);
            continue;
        }

        /* Get policer config. */
        rv = _bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl);
        BCM_IF_ERROR_RETURN(rv);

        if (0 == f_pl->level) {
            /* Get hw encoding for meter mode. */
            rv = _bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode);
            BCM_IF_ERROR_RETURN(rv);
            /* If level 0 policer is Modified trTcm ->
             * meter sharing mode is dual.
             */
        }


           /* Pair index is (Pool number * Pairs in Pool + Pair number) */
            meter_pair_idx
                = (f_pl->pool_index
                    * (stage_fc->meter_pool[instance][f_pl->pool_index]->num_meter_pairs))
                    + (f_pl->hw_index);

                 sal_memset(&meter_info_offset, 0x0, sizeof(meter_info_offset));
                 sal_memset(&meter_offset, 0x0, sizeof(meter_offset));

                 /* METER_PAIR_INDEX */
                 meter_info_offset.offset[0] = 4; 
                 meter_info_offset.width[0]  = 12;
                 meter_info_offset.value[0]  = meter_pair_idx;

                 /* METER_PAIR_MODE */
                 meter_info_offset.offset[1] = 1;
                 meter_info_offset.width[1]  = 3;
                 meter_info_offset.value[1]  = meter_pair_mode;

                 /*
                 * Flow mode is the only one that cares about the test and
                 * update bits.
                 */
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                    /* Excess meter - even index. */

                    /* METER_TEST_ODD */
                    meter_offset.offset[0] = 19;
                    meter_offset.width[0]  = 1;
                    meter_offset.value[0]  = 0;

                     /* METER_TEST_EVEN */
                    meter_offset.offset[1] = 17;
                    meter_offset.width[1]  = 1;
                    meter_offset.value[1]  = 1;

                    /* METER_UPDATE_ODD */
                    meter_offset.offset[2] = 18;
                    meter_offset.width[2]  = 1;
                    meter_offset.value[2]  = 0;

                    /* METER_UPDATE_EVEN */
                    meter_offset.offset[3] = 16;
                    meter_offset.width[3]  = 1;
                    meter_offset.value[3]  = 1;

                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                    /* Committed meter - odd index. */

                    /* METER_TEST_ODD */
                    meter_offset.offset[0] = 19;
                    meter_offset.width[0]  = 1;
                    meter_offset.value[0]  = 1;

                     /* METER_TEST_EVEN */
                    meter_offset.offset[1] = 17;
                    meter_offset.width[1]  = 1;
                    meter_offset.value[1]  = 0;

                    /* METER_UPDATE_ODD */
                    meter_offset.offset[2] = 18;
                    meter_offset.width[2]  = 1;
                    meter_offset.value[2]  = 1;

                    /* METER_UPDATE_EVEN */
                    meter_offset.offset[3] = 16;
                    meter_offset.width[3]  = 1;
                    meter_offset.value[3]  = 0;

                }

                if ((f_pl->cfg.mode == bcmPolicerModePassThrough) ||
                    (f_pl->cfg.mode == bcmPolicerModeSrTcmModified)) {

                    /* METER_PAIR_MODE_MODIFIER */
                    meter_info_offset.offset[2] = 0;
                    meter_info_offset.width[2]  = 1;
                    meter_info_offset.value[2]  = 1;
                }

                ACTION_SET(unit, f_ent, buf, &meter_offset);
                ACTION_SET(unit, f_ent, buf, &meter_info_offset);
    }

   return BCM_E_NONE;
}
/*
 * Function: _bcm_field_td3_ForwardingType_set
 *
 * Purpose:
 *     Add qualification data to an entry for IP Type.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     stage_id - (IN) Stage ID. 
 *     entry    - (IN) Entry ID.
 *     type     - (IN) Forwarding type.
 *     data     - (OUT) Data.
 *     mask     - (OUT) Mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask)
{

    if (NULL == data || NULL == mask) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
           switch (type) {
            case bcmFieldForwardingTypeAny:
                *data = 0;
                *mask = 0;
                break;
            case bcmFieldForwardingTypeL2:
                *data = 0;
                *mask = 0xE;
                break;
            case bcmFieldForwardingTypeL2Independent:
                *data = 0;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Shared:
                *data = 1;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Vpn:
                *data = 2;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2VpnDirect:
/* PTP_VFI */
                *data = 7;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3Direct:
                *data = 9;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3:
                *data = 8;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeMpls:
                *data = 10;
                *mask = 0xF;
                break;
            default:
               return (BCM_E_PARAM);
          }
          break;
      default:
          return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_td3_forwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stage_id - (IN) Stage ID. 
 *      entry    - (IN) BCM field entry id.
 *      type     - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td3_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type)
{
    if (NULL == type) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
          if ((0 == data) && (mask == 0)) {
              *type = bcmFieldForwardingTypeAny;
          } else if ((0 == data) && (mask == 0xE)) {
              *type = bcmFieldForwardingTypeL2;
          } else if ((0 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Independent;
          } else if ((1 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Shared;
          } else if ((2 == data) && (mask == 0xF)) {
              *type =  bcmFieldForwardingTypeL2Vpn;
          } else if ((7 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2VpnDirect;
          } else if ((9 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3Direct;
          } else if ((8 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3;
          } else if ((10 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeMpls;
          } else {
              return (BCM_E_INTERNAL);
          }
           break;
       default:
           return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}
/* Function    : _bcm_field_td3_tpid_hw_encode
 * Description : Encode tpid value to hw specific value.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) tpid   TPID to encode
 *               (OUT) hw_code HW code.
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_tpid_hw_encode(int unit, uint16 tpid, uint16 outer_tpid,
                                                     uint32 *hw_code)
{
    int rv = BCM_E_NONE;  /* Operation return status. */
    soc_reg_t *tpid_r;
    uint32 reg_val;
    uint16 tpid_value;
    int idx;

    if (outer_tpid == 1) {
	tpid_r = outer_tpid_r;
    } else {
	tpid_r = inner_tpid_r;
    }
    for (idx=0; idx<4; idx++) {
	BCM_IF_ERROR_RETURN(soc_reg32_get(unit, tpid_r[idx],
                                             REG_PORT_ANY, 0,
                                             &reg_val));
        tpid_value = soc_reg_field_get(unit, tpid_r[idx],
                                             reg_val, TPIDf);
	if (tpid == tpid_value) {
	    *hw_code = idx;
	    break;
	}
    }
    if (idx == 4) {
    /* None of the TPID REG values matched with
     * the user qualified value. Return error */
      rv = (BCM_E_UNAVAIL);
    }
    return rv;
}
/* Function    : _field_tpid_hw_decode
 * Description : Encode tpid value to hw specific value.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) hw_code HW code.
 *               (OUT) tpid  decoded TPID
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_tpid_hw_decode(int unit, uint32 hw_code, uint16 outer_tpid,
                                                           uint16 *tpid)
{
    soc_reg_t *tpid_r;
    uint32 reg_val;

    if (outer_tpid == 1) {
	tpid_r = outer_tpid_r;
    } else {
	tpid_r = inner_tpid_r;
    }
    if (hw_code >= 4) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, tpid_r[hw_code],
                                      REG_PORT_ANY, 0,
                                      &reg_val));
    *tpid = soc_reg_field_get(unit, tpid_r[hw_code],
                              reg_val, TPIDf);
    return (BCM_E_NONE);
}

/* Function    : _field_td3_drop_vector_mask_init
 * Description : To initialize drop vector mask for IFP and EM stages.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) stage.
 *               (OUT) Drop vector in Hw set to default values
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_drop_vector_mask_init(int unit)
{
    uint32 drop_vector_mask[3];
    uint32 clear_bit = 0;
    uint32 entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    /* Initialize IFP_EM DROP VECTOR MASK.
    * CANCUN will use/program the mask provided at index 1.
    * IFP_EM_DROP_VECTOR_MASK_PROFILE_0 is for
    * IFP_LOGICAL_TABLE_SELECT and IFP_EM_DROP_VECTOR_MASK_PROFILE_1
    * for IFP_TCAM. FP has to program the correc default mask.
    * As per h/w limitation, PFM, PVLAN_VP_EFILTER,
    * MC_PBM_ZERO, MULTICAST_INDEX_ERROR are NA for logical tcam.
    */

     /* drop vector mask is 96 bits width. ie., 3 uint32 values*/
     sal_memset(drop_vector_mask, -1, sizeof(uint32) * 3);

     /* clear the entry */
     sal_memset(entry_buf, 0x0, sizeof(entry_buf));
    /* IFP_EM_DROP_VECTOR_MASK_PROFILE_1 for IFP_TCAM */
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       entry_buf, IFP_EM_KEY_MASK_0f, drop_vector_mask);
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       entry_buf, IFP_EM_KEY_MASK_1f, drop_vector_mask);
     /* MEM write */
     BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                       IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       MEM_BLOCK_ANY, 1, entry_buf));
     /* clear the entry for writing profile 0*/
     sal_memset(entry_buf, 0x0, sizeof(entry_buf));
     /* PFMf */
     _bcm_field_th_val_set(drop_vector_mask, &clear_bit, 80, 1);
     /* PVLAN_VP_EFILTERf */
     _bcm_field_th_val_set(drop_vector_mask, &clear_bit, 66, 1);
     /* MC_PBM_ZEROf */
     _bcm_field_th_val_set(drop_vector_mask, &clear_bit, 81, 1);
     /* MULTICAST_INDEX_ERRORf */
     _bcm_field_th_val_set(drop_vector_mask, &clear_bit, 50, 1);
     /* IFP_EM_DROP_VECTOR_MASK_PROFILE_0 is for IFP_LOGICAL_TABLE_SELECT */
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_0m,
                       entry_buf, IFP_EM_LOGICAL_TABLE_SELECT_KEY_MASKf,
                       drop_vector_mask);
     /* MEM write */
     BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                       IFP_EM_DROP_VECTOR_MASK_PROFILE_0m,
                       MEM_BLOCK_ANY, 1, entry_buf));
     return (BCM_E_NONE);
}

#endif
