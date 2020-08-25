/** \file appl_ref_field_wa_init.c
 * $Id$
 *
 * Stacking Programmable mode application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
/** soc */
#include <soc/schanmsg.h>
#include <shared/utilex/utilex_seq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_field_wa_init.h"
#include "appl_ref_l2_init.h"

/** bcm */
#include <bcm/field.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/*
 * DEFINEs
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
* \brief
*  Configures FG type const, this is useful utility for many WA
* \param [in] unit        -  Device id
* \param [in] stage -  PMF Stage
* \param [in] context_id  -  PMF context Id
* \param [in] action_type  -  PMF action_type
* \param [in] action_value  -  PMF action_value
* \param [in] action_value  -  Name of The Field group and action
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static int
appl_dnx_field_wa_group_const_util(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_action_t action_type,
    int action_value,
    char *name)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_group_t fg_id = 0;
    bcm_field_action_t action_id = 0;
    void *dest_char;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, action_type, stage, &action_info_get_size));

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_type;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = action_value;
    action_info.size = 0;
    action_info.stage = stage;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id));

    /*
     * Create and attach Const Field group to generate constant value for action
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = stage;

    /*
     * Constant FG : number of qualifiers in key must be 0 (empty key) 
     */
    fg_info.nof_quals = 0;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_fec_dest_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.wa_fec_dest_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_j1_same_port_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.wa_j1_same_port_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_j1_learning_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.wa_j1_learning_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    
    *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_oam_layer_index_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_oam.hw_bug.feature_get(unit, dnx_data_oam_hw_bug_oam_offset_supported))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    else
    {
        if (dnx_data_field.init.wa_oam_layer_index_get(unit))
        {
            *dynamic_flags = 0;
        }
        else
        {
            *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
        }
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_trap_l4_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.wa_l4_trap_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_oam_stat_cb(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_field.init.wa_oam_stat_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    return _SHR_E_NONE;

}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_flow_id_cb(
    int unit,
    int *dynamic_flags)
{

    if (dnx_data_field.init.wa_flow_id_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }

    return _SHR_E_NONE;
}

/** see appl_ref_field_wa_init.h*/
shr_error_e
appl_dnx_field_wa_roo_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_field.init.wa_roo_get(unit)) &&
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    return _SHR_E_NONE;
}

/**
* \brief
*  Add entries per trap code and layer protocol+1 type
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \param [in] trap_id            - Trap id values key
* \param [in] pars_inc           -  Action of how much to increment the FWD layer
* \param [in] fwd_plus_one_type  - Forwarding+1 protocol type
* \param [in] fwd_plus_one_mask  -  Forwarding+1 mask
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_wa_oam_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info,
    int trap_id,
    int pars_inc,
    int fwd_plus_one_type,
    int fwd_plus_one_mask)
{
    bcm_field_entry_info_t entry_info;
    int ii;
    bcm_field_entry_t dummy_ent;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < 8; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = fg_info->nof_actions;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        entry_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[0].value[0] = fwd_plus_one_type;
        entry_info.entry_qual[0].mask[0] = fwd_plus_one_mask;
        entry_info.entry_qual[1].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[1].value[0] = trap_id;
        entry_info.entry_qual[1].mask[0] = 0x1FF;
        entry_info.entry_qual[2].type = bcmFieldQualifyForwardingLayerIndex;
        entry_info.entry_qual[2].value[0] = ii;
        entry_info.entry_qual[2].mask[0] = 0x7;

        entry_info.entry_action[0].type = bcmFieldActionEgressForwardingIndex;
        entry_info.entry_action[0].value[0] = (ii + pars_inc) % 8;

        entry_info.entry_action[1].type = bcmFieldActionForwardingLayerIndex;
        entry_info.entry_action[1].value[0] = (ii + pars_inc) % 8;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap LR_type %d Trap %d Index %d Action %d \n",
                     entry_info.entry_qual[0].value[0],
                     entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[2].value[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));

    }

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM WA and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_oam_layer_index_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int ii = 0;
    int nof_quals = 3;
    int nof_actions = 2;

    /*
     *
     *
     *   Trap Type                               |     Trap ID             |  Layer_FWD + 1      | Parsing_start_index
     * OAM_WA_TRAP_ETH_OAM                   224                        X                          FWD + 1
     * OAM_WA_TRAP_Y1731_O_MPLSTP        225                        X                          FWD
     * OAM_WA_TRAP_Y1731_O_PWE            226                        X                          FWD
     * OAM_WA_TRAP_BFD_O_IPV4               227                        X                          FWD + 2
     * OAM_WA_TRAP_BFD_O_MPLS              228                        X                          FWD + 2
     * OAM_WA_TRAP_BFD_O_PWE               229                     IPv4/IPv6                 FWD + 2
     * OAM_WA_TRAP_BFDCC_O_MPLSTP       230                     IPv4/IPv6                 FWD + 2
     * OAM_WA_TRAP_BFDCV_O_MPLSTP       231                     IPv4/IPv6                 FWD + 2
     * OAM_WA_TRAP_BFD_O_PWE               229                        X                          FWD
     * OAM_WA_TRAP_BFDCC_O_MPLSTP       230                        X                          FWD
     * OAM_WA_TRAP_BFDCV_O_MPLSTP       231                        X                          FWD
     * OAM_WA_TRAP_BFD_O_IPV6           232                        X                          FWD + 2
     *
     * Entrys are going to be add only for traps that need to increment the Parsing index of Forwarding
     *
     */

    /**********************************/
    /*
     * Globals for wa_oam_layer_index 
     */
    /**********************************/
    int OAM_WA_TRAP_ETH_OAM = 224;
    int OAM_WA_TRAP_BFD_O_IPV4 = 227;
    int OAM_WA_TRAP_BFD_O_MPLS = 228;
    int OAM_WA_TRAP_BFD_O_PWE = 229;
    int OAM_WA_TRAP_BFDCC_O_MPLSTP = 230;
    int OAM_WA_TRAP_BFDCV_O_MPLSTP = 231;
    int OAM_WA_TRAP_BFD_O_IPV6 = 232;

    int LayerTypeMask = 0x1F;

    bcm_field_group_t fg_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * / SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries(unit, OAM_WA_TRAP_ETH_OAM, 1, 0, 0));
     * 
     * 
     * * Set quals 
     */
    fg_info.nof_quals = nof_quals;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[2] = bcmFieldQualifyForwardingLayerIndex;

    /*
     * Set actions 
     */
    fg_info.nof_actions = nof_actions;

    fg_info.action_types[0] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_FWD_IDX_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

   /**Entries for OAM_WA_TRAP_ETH_OAM*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries(unit, fg_id, &fg_info, OAM_WA_TRAP_ETH_OAM, 1, 0, 0));

   /**Entries for OAM_WA_TRAP_BFD_O_IPV4*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries(unit, fg_id, &fg_info, OAM_WA_TRAP_BFD_O_IPV4, 2, 0, 0));

   /**Entries for OAM_WA_TRAP_BFD_O_IPV6*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries(unit, fg_id, &fg_info, OAM_WA_TRAP_BFD_O_IPV6, 2, 0, 0));

   /**Entries for OAM_WA_TRAP_BFD_O_MPLS*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries(unit, fg_id, &fg_info, OAM_WA_TRAP_BFD_O_MPLS, 2, 0, 0));

   /**Entries for OAM_WA_TRAP_BFD_O_PWE*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFD_O_PWE, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_WA_TRAP_BFD_O_PWE*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFD_O_PWE, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

   /**Entries for OAM_WA_TRAP_BFDCC_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFDCC_O_MPLSTP, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_WA_TRAP_BFDCC_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFDCC_O_MPLSTP, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

   /**Entries for OAM_WA_TRAP_BFDCV_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFDCV_O_MPLSTP, 2, bcmFieldLayerTypeIp4, LayerTypeMask));

   /**Entries for OAM_WA_TRAP_BFDCV_O_MPLSTP*/
    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_add_entries
                    (unit, fg_id, &fg_info, OAM_WA_TRAP_BFDCV_O_MPLSTP, 2, bcmFieldLayerTypeIp6, LayerTypeMask));

exit:
    SHR_FUNC_EXIT;
}

/*
* When hitting TCP UDP traps in FLP they might be False Positive since the HW do not validate that the next protocol of IP is indeed TCP/UDP.
* Hence we add a validation in iPMF3 (Workaround) that in case the trap was hit but the next protocol is not TCP/UDP we disable the trap.

TCP SEQ and flags are zero                                   bcmRxTrapTcpSnFlagsZero
TCP SEQ is zero and either FIN/URG or PSH are one            bcmRxTrapTcpSnZeroFlagsSet
TCP SYN and FIN are set                                      bcmRxTrapTcpSynFin
TCP Source port equals destination port                      bcmRxTrapTcpEqualPorts
TCP fragment without full TCP header                         bcmRxTrapTcpFragmentIncompleteHeader
TCP fragment with offset less than 8                         bcmRxTrapTcpFragmentOffsetLt8
UDP Source port equals destination port                      bcmRxTrapUdpEqualPorts
*/

/**
* \brief
*  Add entries:
*  When +1 protocol match the hit trap - nothing should happen
*  When +1 protocol do no match the hit trap - should preform some defualt trap that has no actions configured
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \param [in] action_do_nothing  - The do nothing action
* \param [in] trap_do_nothing    - The do nothing trap
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_wa_trap_l4_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info,
    bcm_field_action_t action_do_nothing,
    bcm_gport_t trap_do_nothing)
{
    bcm_field_entry_info_t entry_info;
    int ii;
    int trap_code = 0;
    int tcp_udp_nof_traps = 7;
    bcm_field_entry_t dummy_ent;

    bcm_rx_trap_t field_wa_trap_tcp_udp_types[tcp_udp_nof_traps];

    SHR_FUNC_INIT_VARS(unit);
    field_wa_trap_tcp_udp_types[0] = bcmRxTrapTcpSnFlagsZero;
    field_wa_trap_tcp_udp_types[1] = bcmRxTrapTcpSnZeroFlagsSet;
    field_wa_trap_tcp_udp_types[2] = bcmRxTrapTcpSynFin;
    field_wa_trap_tcp_udp_types[3] = bcmRxTrapTcpEqualPorts;
    field_wa_trap_tcp_udp_types[4] = bcmRxTrapTcpFragmentIncompleteHeader;
    field_wa_trap_tcp_udp_types[5] = bcmRxTrapTcpFragmentOffsetLt8;
    field_wa_trap_tcp_udp_types[6] = bcmRxTrapUdpEqualPorts;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries that match +1 protocol and should not change the behavior %s%s%s%s\n",
                 EMPTY, EMPTY, EMPTY, EMPTY);

    for (ii = 0; ii < tcp_udp_nof_traps; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = 1;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, field_wa_trap_tcp_udp_types[ii], &trap_code));

        entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[0].value[0] = trap_code;
        entry_info.entry_qual[0].mask[0] = 0x1FF;
        entry_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeTcp;
        entry_info.entry_qual[1].mask[0] = 0x1F;
        if (field_wa_trap_tcp_udp_types[ii] == bcmRxTrapUdpEqualPorts)
        {
            entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeUdp;
        }

        entry_info.entry_action[0].type = action_do_nothing;
        entry_info.entry_action[0].value[0] = 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap %d LR_type %d LR_Mask %d Action %d \n",
                     entry_info.entry_qual[0].value[0], entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[1].mask[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries that DO NOT match +1 protocol should not change the behavior %s%s%s%s\n",
                 EMPTY, EMPTY, EMPTY, EMPTY);
    for (ii = 0; ii < tcp_udp_nof_traps; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = 1;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, field_wa_trap_tcp_udp_types[ii], &trap_code));

        entry_info.entry_qual[0].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[0].value[0] = trap_code;
        entry_info.entry_qual[0].mask[0] = 0x1FF;
        entry_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeTcp;
        entry_info.entry_qual[1].mask[0] = 0x0;
        if (field_wa_trap_tcp_udp_types[ii] == bcmRxTrapUdpEqualPorts)
        {
            entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeUdp;
        }

        entry_info.entry_action[0].type = bcmFieldActionTrap;
        entry_info.entry_action[0].value[0] = trap_do_nothing;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals Trap %d LR_type %d LR_Mask %d Action %d \n",
                     entry_info.entry_qual[0].value[0], entry_info.entry_qual[1].value[0],
                     entry_info.entry_qual[1].mask[0], entry_info.entry_action[0].value[0]);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM WA and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_trap_l4_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_attach_info_t attach_info;
    bcm_rx_trap_config_t trap_config;
    int trap_id;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int ii = 0;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;

    bcm_field_action_t action_do_nothing;

    bcm_gport_t trap_do_nothing = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 31;
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF1;

    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "L4_TRAP_Do_Nothing", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_do_nothing));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * Set quals 
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_types[1] = action_do_nothing;

   /**For Void action there is no reason to put Valid bit, only waste bit (since not performed by FES)*/
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "L4_TRAP_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
   /**Would like to take */
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

   /**Max Strength*/
    BCM_GPORT_TRAP_SET(trap_do_nothing, trap_id, 15, 7);

   /**Entries */
    SHR_IF_ERR_EXIT(appl_ref_field_wa_trap_l4_add_entries(unit, fg_id, &fg_info, action_do_nothing, trap_do_nothing));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add entries per stat_lm_index to +1
* \param [in] unit               - Device ID
* \param [in] fg_id              - FG ID
* \param [in] fg_info            - FG info
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_ref_field_wa_oam_wa_add_entries(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info)
{
    bcm_field_entry_info_t entry_info;
    int ii;
    int OAM_WA_STAT_LM_SIZE = 3;
    bcm_field_entry_t dummy_ent;

    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Add entries%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);

    for (ii = 0; ii < OAM_WA_STAT_LM_SIZE + 1; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = fg_info->nof_actions;
        entry_info.nof_entry_quals = fg_info->nof_quals;

        entry_info.entry_qual[0].type = fg_info->qual_types[0];
        entry_info.entry_qual[0].value[0] = ii;
        entry_info.entry_qual[0].mask[0] = 0x3;

        entry_info.entry_action[0].type = fg_info->action_types[0];
        if (ii == OAM_WA_STAT_LM_SIZE)
        {
            entry_info.entry_action[0].value[0] = 0;
        }
        else
        {
            entry_info.entry_action[0].value[0] = (ii + 1);
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Quals stat_LM_id %d  Action stat_LM_id %d %s%s\n",
                     entry_info.entry_qual[0].value[0], entry_info.entry_action[0].value[0], EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &dummy_ent));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM WA and attach it to given as param context
* \param [in] unit        - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_oam_stat_init(
    int unit)
{

    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_attach_info_t attach_info;
    int ii = 0;
    void *dest_char;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals 
     */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStatOamLM;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatOamLM;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "WA_OAM__stat_lm", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(appl_ref_field_wa_oam_wa_add_entries(unit, fg_id, &fg_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Since FER had a HW but of not  setting the dest type of trap correctly (i.e. setting dest instead of updating trap code)
* we need iPMF3 WA which will do simple case of dest = dest and let hw handle the case dest type trap.
*
*  iPMF3 DE with dest = dest
* \param [in] unit        - Device id
* \param [in] Context_id        - Context to attach the FEC_WA Field group to.
* \param [in] add_df        - If TRUE a FEC WA field group will be add otherwise only attached to context
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
appl_dnx_field_wa_fec_dest_on_context(
    int unit,
    bcm_field_context_t context_id,
    int add_fg)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = dnx_data_field.group.id_fec_wa_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (add_fg == TRUE)
    {
        bcm_field_action_info_t action_info;
        bcm_field_action_t fec_dest_action = 0;
        void *dest_char;
        /** Create destination action to ignore destination qualifier which is 0*/
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionForward;
        action_info.prefix_size = 11;
        action_info.prefix_value = 0;
        action_info.size = 21;
        action_info.stage = bcmFieldStageIngressPMF3;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "WA_FEC_Dst_w_o_qual", sizeof(action_info.name));

        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &fec_dest_action));

        bcm_field_group_info_t_init(&fg_info);
        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.nof_quals = 1;
        fg_info.stage = bcmFieldStageIngressPMF3;

        fg_info.qual_types[0] = bcmFieldQualifyDstPort;

        fg_info.nof_actions = 1;
        fg_info.action_with_valid_bit[0] = FALSE;

        /** Order of actions should be the same as order of qualifiers*/
        fg_info.action_types[0] = fec_dest_action;

        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "WA_FEC_dest", sizeof(fg_info.name));

        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    }
    else
    {
        bcm_field_group_info_t_init(&fg_info);
        SHR_IF_ERR_EXIT(bcm_field_group_info_get(unit, fg_id, &fg_info));
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    /**Destination is taken from meta data*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Since FER had a HW but of not  setting the dest type of trap correctly (i.e. setting dest instead of updating trap code)
* we need iPMF3 WA which will do simple case of dest = dest and let hw handle the case dest type trap.
*
*  iPMF3 DE with dest = dest
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_fec_dest_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_field_wa_fec_dest_on_context(unit, context_id, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* ERPP Fwd-Context Selection
*   (FAI==0) -->  FWD-Context = ETH  =>  this is the problem
* ERPP Ethernet filters are executed, packet may be trapped
* Fix traps in PMF
*    PMF Context Selection by: PPH_Present(1b) == 0
*       No lookup
*       Actions:
*           Forward-Action-Profile = NORMAL (0) => revert ERPP filters
*           ACE_Context_Value[1:0] = J_Mode_FWD_Code_Override (via ACE-Pointer) => ensure correct context selection in ETPP
*    ETPP Fwd-Code Selection
*       (ACE_Context_Value[1:0] == J_Mode_FWD_Code_Override) --> FWD-Context = TM
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_j1_same_port_init(
    int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SAME_P_ACE", sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextForwardingCodeOverride;

    SHR_IF_ERR_EXIT(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_SAME_P_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

    SHR_IF_ERR_EXIT(appl_dnx_field_wa_group_const_util(unit, bcmFieldStageEgress, context_id, bcmFieldActionAceEntryId,
                                                       ace_entry_handle, "J1_SAME_P_CNST_ACE"));

    SHR_IF_ERR_EXIT(appl_dnx_field_wa_group_const_util(unit, bcmFieldStageEgress, context_id, bcmFieldActionTrap,
                                                       0, "J1_SAME_P_CNST_TRAP"));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * lowest priority presel
     * */
    p_id.presel_id = 126;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPphPresent;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = 0;
    p_data.qual_data[0].qual_mask = 0x1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* Egress parser always assume that FLOW extension present
* Hence need to create PMF WA to set the valid bit for flow ext to TRUE
* Since PMF is the one to generate the Flow id we set the action to be flow_id=0 flow_profile=0 and flow_valid=1
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
appl_dnx_field_wa_flow_id_init(
    int unit)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_context_get(unit);
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t flow_action = 0;
    bcm_field_action_info_t action_info_predefined;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_action_info_get
                    (unit, bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF1, &action_info_predefined));

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionLatencyFlowId;
    action_info.prefix_size = action_info_predefined.size;
    /**Valid bit is the lsb*/
    action_info.prefix_value = 1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "WA_Flow_valid_bit", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &flow_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = flow_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "WA_Flow_FG", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /**Make the action to be lowest priority*/
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_wa_roo_init(
    int unit)
{

    /*
     *  * Context "Routing, ROO"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      out_lif1_range (!=0x0)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = OutLIF[0] (18 lsb)
     *      OutLIF[0] = OutLIF[1]
     *Context " Routing, Not ROO"
     *   Context_Selection:
     *      Fwd layerType = IPv4/IPv6
     *      out_lif1_range (==0x0)
     *   Actions: Direct_Extraction
     *      fwd_domain_id = OutLIF[0] (18 lsb)
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id_with_roo = 0;
    bcm_field_group_t fg_id_w_o_roo = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    void *dest_char;
    bcm_field_context_t context_id_with_roo;
    bcm_field_context_t context_id_w_o_roo;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_range_info_t range_info;
    int range_zero = 0, range_non_zero = 1;
    SHR_FUNC_INIT_VARS(unit);

    /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcmFieldQualifyVrf, bcmFieldStageIngressPMF3, &qual_info_get));

    qual_info.size = qual_info_get.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_G_OUT_LIF_LS", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyOutVPort1;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionVSwitchNew;
    fg_info.action_types[1] = bcmFieldActionOutVport0;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_ROO_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_with_roo));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_ROO_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_with_roo));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of GLOB_OUT_LIF_0*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyOutVPort0, bcmFieldStageIngressPMF3, &qual_info_get));

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_with_roo, context_id_with_roo, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * Presel of WITH_OUT roo MUST BE Higher priority (i.e. lower number)
     * Since with_out roo check if the LIF range == 0
     * If the context was not hit there it means Lif Range != 0
     * */
    p_id.presel_id = 120;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_with_roo;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = 121;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /*******************************************************
     * With Out Roo
     * ****************************************************
     */
    fg_info.nof_actions = 1;
    fg_info.nof_quals = 1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_o_ROO_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_w_o_roo));

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_ROO_w_o_ROO_CTX", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &context_id_w_o_roo));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_w_o_roo, context_id_w_o_roo, &attach_info));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
     * Presel of WITH_OUT roo MUST BE Higher priority (i.e. lower number)
     * Since with_out roo check if the LIF range == 0
     * If the context was not hit there it means Lif Range != 0
     * */
    p_id.presel_id = 119;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id_w_o_roo;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyVPortRangeCheck;
    /**The check the range of out_lif_1 we set arg to 1*/
    p_data.qual_data[1].qual_arg = 1;
    /**Assuming profile 0 is configured for no-range*/
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    p_id.presel_id = 118;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /**Configure outlif_ranges*/
    bcm_field_range_info_t_init(&range_info);
    range_zero = 0;
    range_non_zero = 1;
    range_info.max_val = 0;
    range_info.min_val = 0;
    range_info.range_type = bcmFieldRangeTypeOutVport;
    SHR_IF_ERR_EXIT(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF3, range_zero, &range_info));

    range_info.max_val = (1 << qual_info.size) - 1;
    SHR_IF_ERR_EXIT(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF3, range_non_zero, &range_info));

    {
        /**Add FEC WA on ROO context's*/
        bcm_field_group_t fg_fec_id = dnx_data_field.group.id_fec_wa_get(unit);
        int add_fec_fg_needed = FALSE;
        int rv = BCM_E_NONE;

        rv = bcm_field_group_info_get(unit, fg_fec_id, &fg_info);
        if (rv != BCM_E_NONE)
        {
            add_fec_fg_needed = TRUE;
        }
        SHR_IF_ERR_EXIT(appl_dnx_field_wa_fec_dest_on_context(unit, context_id_with_roo, add_fec_fg_needed));
        SHR_IF_ERR_EXIT(appl_dnx_field_wa_fec_dest_on_context(unit, context_id_w_o_roo, FALSE));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
appl_dnx_field_j1_learning_wa_init(
    int unit)
{

    /*
     * j1_learning_wa
     * 1. Context selection OLP port + TM forwarding type (ITMH) -> create a context with compare in iPMF1
     * 2. 2 user defined qualifiers :
     *     1. Learn-info format payload[39:40] - 2 bits
     *     2. Type of destination : payload[16:18] - 3 bits
     * 3. Build a compare key in iPMF1 with structure:
     *    Bits [0:8]: in-port
     *    Bits [9:10]: Learn-info format (user_qual1)
     * 4. Use Direct Extraction in iPMF2 with the following key:
     *     1. Compare result equal bit [bit 3]
     *     2. Type of destination : (user_qual2) [bits 0:2]
     * 5. Use FEM action with condition of the 4 bits of the key.
     * 6. In case 4 bits condition equal to
     *   1001, 1010, 1011, 1100,1101   [values of 9,10,11,12,13] perform action redirect with some destination port (const)
     * 7. Additional configuration for second-Pass (packets that are coming in 2nd pass from recycle port):
     *    context selection based on Recycle port.
     *    Constant FG : no key, Action performed: Redirect to destination port.
     */

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_info_t fg_info;
    int ii = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t cmp_qual_id, const_qual_id, learn_foramt_qual_id, fwd_type_qual_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t redirect_action_id;
    bcm_field_action_priority_t action_priority;
    void *dest_char;
    int op_offset;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_context_t context_id, second_pass_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_gport_t olp_port, gport_rcy, gport_dest;
    uint32 recycle_port = 202;
    bcm_gport_t destination_port = 203;
    uint32 olp_port_class_id = 1;
    uint32 rcycle_port_class_id = 2;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Create user defined qual for 1bit result of the desired compare operand.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_1bitCmpRes", sizeof(qual_info.name));
    qual_info.size = 1;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cmp_qual_id));

    /**
     * Create user defined qual for zero constatnt qualifier (2 bits)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_zeroConst", sizeof(qual_info.name));
    qual_info.size = 2;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &const_qual_id));

    /**
     * Create user defined qual for  Learning format in OLP payload (bits 39:40)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_Format", sizeof(qual_info.name));
    qual_info.size = 2;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &learn_foramt_qual_id));

    /**
     * Create user defined qual for forwarding type in OLP payload (bits 18:16)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Leran_Fwd_Type", sizeof(qual_info.name));
    qual_info.size = 3;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &fwd_type_qual_id));

    /**
     * Create user defined action for  Learning format in OLP payload (bits 39:40)
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_redirect", sizeof(action_info.name));
    action_info.size = 4;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &redirect_action_id));

    /*
     * Get the OLP port number
     */
    /**SHR_IF_ERR_EXIT(appl_dnx_olp_port_get(unit, &olp_port));*/
    BCM_GPORT_LOCAL_SET(olp_port, 240);
    BCM_GPORT_LOCAL_SET(gport_rcy, recycle_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport_dest, destination_port);

    /*
     * create a context in iPMF1 stage with compare mode 
     */
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_CTX", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    /***
	 * creare the context selection based on OLP port profile
     * */
    p_id.presel_id = 125;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = olp_port_class_id;
    p_data.qual_data[1].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, olp_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, olp_port_class_id));

        /**
	* Init the compare_info
	*/
    bcm_field_context_compare_info_t_init(&compare_info);
        /**
	* Fill the compare info
	* In the example L4 Src port is being compared to L4 Dst port using initial keys F and G of IPMF1
	*/
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = learn_foramt_qual_id;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 39;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = const_qual_id;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    compare_info.second_key_info.qual_info[0].input_arg = 0;
    compare_info.second_key_info.qual_info[0].offset = 0;

        /**
	* Create the context compare mode.
	*/
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));

    /*
     * create a direct Extraction Field Group in iPMF2 
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = fwd_type_qual_id;
    fg_info.qual_types[1] = cmp_qual_id;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = redirect_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_FG", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;

    for (ii = 9; ii < 14; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionRedirect;
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = ((gport_rcy >> ii) & 0x1);
    }

    action_priority = BCM_FIELD_ACTION_POSITION(1, 7);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

    /**
     * Retrieve the desired result offset of the qualifier.
     */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get));

    /**
     * Get the compare opernad offset.
     */
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandEqual, &op_offset));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 18;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = op_offset + qual_info_get.offset;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /*
     * create a context in iPMF1 stage for 2nd pass 
     */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "J1_Learn_2Pass_CTX", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF1, &context_info, &second_pass_context_id));

    /***
	 * creare the context selection based on OLP port profile
     * */
    p_id.presel_id = 124;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = second_pass_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = rcycle_port_class_id;
    p_data.qual_data[0].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    SHR_IF_ERR_EXIT(bcm_port_class_set
                    (unit, gport_rcy, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcycle_port_class_id));

    /*
     * Create a constant Field Group to redirect all packets ingressing from recyle port to destination port
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_wa_group_const_util
                    (unit, bcmFieldStageIngressPMF1, second_pass_context_id, bcmFieldActionRedirect, gport_dest,
                     "J1_Learn_2Pass_FG"));

exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */
