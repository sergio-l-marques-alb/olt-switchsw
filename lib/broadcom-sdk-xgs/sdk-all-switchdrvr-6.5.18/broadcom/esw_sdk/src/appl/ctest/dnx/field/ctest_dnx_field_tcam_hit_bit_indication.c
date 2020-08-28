/**
 * $Id: ctest_dnx_field_tcam_hit_bit_indication.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * \file ctest_dnx_field_tcam_hit_bit_indication.c
 *
 * TCAM HIT BIT INDICATION testing.
 *
 */
/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include "ctest_dnx_field_tcam_hit_bit_indication.h"
#include "ctest_dnx_field_utils.h"
#include <soc/sand/sand_signals.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'tcam_hit' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_tcam_hit_bit_indication_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'tcam_hit' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_tcam_hit_bit_indication_tests[] = {
    {"HIT_BIT", "", CTEST_POSTCOMMIT},
    {NULL}
};

/**
 *  tcam_hit shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_tcam_hit_bit_indication_man = {
    .brief =
        "Field TCAM HIT BIT INDICATION tests",
    .full =
        "This Ctest presents hit bit indication functionality with DBAL APIs.\n"
        "Creating field group, attaching it to the default context, adding an entry\n"
        "and sending traffic.\n",
    .synopsis =
        ""
};

/**
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and
 * to match on the qualifier.
 * Expected value isn't taken from here, and is merely initialized.
 */
static dnx_field_utils_signal_info_t Ctest_tcam_hit_bit_indication_sig_info = { 0, "IRPP", "IPMF1", "FER", "TC", 3, 6 };

/** The packet to be sent. */
static dnx_field_utils_packet_info_t Ctest_tcam_hit_bit_indication_packet_info = {
    {"PTCH_2", "PTCH_2.PP_SSP", "27", "ETH1", "IPv4", "IPv4.SIP", "0.0.0.2", "IPv4.DIP", "0.0.0.1", ""}
};

/**
 * \brief
 *  This function is creating TCAM FG (attaching it
 *  to the default context). Adding an entry to the FG. All config is done
 *  on IPMF1 stage.
 *
 * \param [in] unit - Device ID
 * \param [out] fg_id_p - Created Field Group id
 * \param [out] entry_handle_p - Created Entry Handle
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_tcam_hit_bit_indication_config(
    int unit,
    bcm_field_group_t * fg_id_p,
    bcm_field_entry_t * entry_handle_p)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    sal_strncpy_s((char *) fg_info.name, "TCAM_HIT_BIT_INDICATION", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.priority = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = bcmFieldQualifyTpid;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x8100;
    entry_info.entry_qual[0].mask[0] = 0xFFFF;

    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 6;

    /** Create the field group. */
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was created. \n%s%s%s", *fg_id_p, EMPTY, EMPTY, EMPTY);

    /** Attach the created field group to the context. */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, *fg_id_p, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info));

    LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was attached to Context ID %d. \n%s%s", *fg_id_p, BCM_FIELD_CONTEXT_ID_DEFAULT, EMPTY, EMPTY);

    /** Add an entry to the created field group. */
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, *fg_id_p, &entry_info, entry_handle_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "Entry ID 0x%X (%d) was added to Field Group ID %d. \n%s", *entry_handle_p, *entry_handle_p, *fg_id_p, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This functions tests the TCAM HIT BIT INDICATION using DBAL functions.
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_tcam_hit_bit_indication_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_entry_t entry_handle;
    bcm_field_group_t fg_id;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags = 0;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    uint32 hit_bit = FALSE;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnx_field_tcam_hit_bit_indication_config(unit, &fg_id, &entry_handle));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, 200, &flags, &interface_info, &mapping_info));
    sal_snprintf(Ctest_tcam_hit_bit_indication_packet_info.header_info[2], DNX_FIELD_UTILS_STR_SIZE, "%d", mapping_info.pp_port);
    SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 200, &Ctest_tcam_hit_bit_indication_packet_info));
    SHR_IF_ERR_EXIT(sand_signal_verify(unit, mapping_info.core, Ctest_tcam_hit_bit_indication_sig_info.block,
                                       Ctest_tcam_hit_bit_indication_sig_info.from, Ctest_tcam_hit_bit_indication_sig_info.to,
                                       Ctest_tcam_hit_bit_indication_sig_info.sig_name, &(Ctest_tcam_hit_bit_indication_sig_info.expected_value),
                                       Ctest_tcam_hit_bit_indication_sig_info.size, NULL, NULL, 0));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (hit_bit == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Entry HIT BIT indication is FALSE, when it should be 0x%x! FG ID %d and ENTRY ID %d.\n",
                     hit_bit, fg_id, entry_handle);
    }

    hitbit_flags |= DBAL_ENTRY_ATTR_HIT_CLEAR;
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (hit_bit == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Entry HIT BIT indication is FALSE, when it should be 0x%x! FG ID %d and ENTRY ID %d.\n",
                     hit_bit, fg_id, entry_handle);
    }

    hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hit_bit));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (hit_bit != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Entry HIT BIT indication is 0x%x, when it should be FALSE! FG ID %d and ENTRY ID %d.\n",
                     hit_bit, fg_id, entry_handle);
    }

    SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id, NULL, entry_handle));
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the TCAM HIT BIT INDICATION ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field tcam_hit"
 */
shr_error_e
sh_dnx_field_tcam_hit_bit_indication_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (!SOC_IS_JERICHO2_ONLY(unit))
    {
        LOG_CLI_EX("\r\n" "HIT BIT is not supported for J2C and above devices!!%s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_tcam_hit_bit_indication_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
