/** \file ctest_dnx_stif_kbp_voq_db.c
 *
 * ctests for stif with kbp for voq database counting
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_kbp_stif.h"
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <bcm/stack.h>
#include <bcm/stat.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

/** sal */
#include <sal/appl/sal.h>

#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/tap.h>

#define DNX_KBP_STIF_NOF_INST_MAX (4)
#define DNX_KBP_STIF_NOF_DB_MAX (8)
#define DNX_KBP_STIF_STRING_SIZE (32)

typedef struct
{
    uint32 object_id;
    uint32 counter_set_offset;
    uint64 packets_value;
    uint64 expected_packets;
    uint64 bytes_value;
    uint64 expected_bytes;
} dnx_kbp_stif_element_ts;

typedef struct
{
    struct kbp_device *device;
    struct kbp_device *device_th[BCM_KBP_MAX_NOF_THREADS];
    struct kbp_tap_instruction *inst_th[DNX_KBP_STIF_NOF_INST_MAX];
    int nof_instructions;
    int nof_db;
    struct kbp_tap_db *db[DNX_KBP_STIF_NOF_DB_MAX];
} dnx_kbp_stif_config_ts;

dnx_kbp_stif_config_ts kbp_stif_config;

/**
* \brief - callback function, which doing the procedure required from user after ISSU/warm-boot
*   For testing, it is being called from the warm-boot test enviorment,after each successful warm-boot.
* \param [in] unit     - Device ID
* \param [in] userdata - currently not in use
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_kbp_stif_wb_test_callback(
    int unit,
    void *userdata)
{
    bcm_switch_kbp_info_t kbp_info;
    int i;
    kbp_status status;
    struct kbp_tap_instruction *new_inst_th;
    struct kbp_tap_db *new_db;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    kbp_stif_config.device = (struct kbp_device *) kbp_info.device_p;

    if (kbp_mngr_smt_enabled(unit))
    {
        for (i = 0; i < BCM_KBP_MAX_NOF_THREADS; i++)
        {
            kbp_stif_config.device_th[i] = (struct kbp_device *) kbp_info.thread_p[i];
        }
    }

    for (i = 0; i < kbp_stif_config.nof_instructions; i++)
    {
        status = kbp_tap_instruction_refresh_handle(kbp_stif_config.device, kbp_stif_config.inst_th[i], &new_inst_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_refresh_handle failed (i=%d): %s\n", i,
                         kbp_get_status_string(status));
        }
        kbp_stif_config.inst_th[i] = new_inst_th;
    }
    for (i = 0; i < kbp_stif_config.nof_db; i++)
    {
        status = kbp_tap_db_refresh_handle(kbp_stif_config.device, kbp_stif_config.db[i], &new_db);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_refresh_handle failed (i=%d): %s\n", i,
                         kbp_get_status_string(status));
        }
        kbp_stif_config.db[i] = new_db;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - dynamic ethernet ports configuration (not stif), in order to be able to do snake in the tests.
*   routine first destroy all ethernet ports, and then add new ones.
* \param [in] unit     - Device ID
* \param [in] core_id - core_id
* \param [in] nof_ports - number of ports to configure
* \param [in] port_arr - array of logical ports to add
* \param [in] speed - speed ports
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
ctest_kbp_stif_port_config(
    int unit,
    int core_id,
    int nof_ports,
    int *port_arr,
    int speed)
{
    int i;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags, current_first_phy;
    int nof_lanes, ethu_id, stif_unit_first_lane, stif_unit_nof_lanes;
    ctest_cint_argument_t cint_arguments[10];
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_dynamic_port_add_remove.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/port/cint_test_dynamic_port_add.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_all", NULL, 0),
                        "cint_dyn_port_remove_all Run Failed\n");
    current_first_phy = dnx_data_nif.phys.nof_phys_per_core_get(unit) * core_id;
    SHR_IF_ERR_EXIT(bcm_port_get(unit, 33, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(dnx_algo_phy_ethu_lane_info_get
                    (unit, interface_info.phy_port, &stif_unit_first_lane, &stif_unit_nof_lanes));
    for (i = 0; i < 10; i++)
    {
        cint_arguments[i].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[i].value.value_int32 = 0;
    }

    for (i = 0; i < nof_ports; i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, current_first_phy, &ethu_id));

        if (speed == 50000)
        {
            nof_lanes = 2;
        }
        else if (speed == 100000)
        {
            nof_lanes = (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type == imbDispatchTypeImb_cdu) ? 2 : 4;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "invalid speed (=%d)\n", speed);
        }
        /** if the phy and the stif phy on the same unit - jump to next one */
        if (((current_first_phy >= stif_unit_first_lane)
             && (current_first_phy < stif_unit_first_lane + stif_unit_nof_lanes))
            || ((current_first_phy + nof_lanes >= stif_unit_first_lane)
                && (current_first_phy + nof_lanes < stif_unit_first_lane + stif_unit_nof_lanes)))
        {
            current_first_phy = stif_unit_first_lane + stif_unit_nof_lanes;
            /** if ethu_id type was changed from cdu to clu, we need to change the nof_lanes */
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, current_first_phy, &ethu_id));
            if (speed == 100000)
            {
                nof_lanes =
                    (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type == imbDispatchTypeImb_cdu) ? 2 : 4;
            }
        }
        if (current_first_phy >= dnx_data_nif.phys.nof_phys_per_core_get(unit) * (core_id + 1))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "cannot find phy to allocate port index=%d \n", i);
        }
        cint_arguments[0].value.value_int32 = port_arr[i]; /** port_to_add */
        cint_arguments[1].value.value_int32 = 0; /** channel */
        cint_arguments[2].value.value_int32 = port_arr[i]; /** tm_port */
        cint_arguments[3].value.value_int32 = 2; /** num_priorities */
        cint_arguments[4].value.value_int32 = current_first_phy; /** first_phy */
        cint_arguments[5].value.value_int32 = BCM_PORT_IF_NIF_ETH; /** interface */
        cint_arguments[6].value.value_int32 = nof_lanes; /** num_lanes */
        cint_arguments[7].value.value_int32 = speed; /** speed kbps*/
        cint_arguments[8].value.value_int32 = 0; /** flags */
        cint_arguments[9].value.value_int32 = 0; /** offset */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "dynamic_port_add", cint_arguments, 10),
                            "dynamic_port_add Run Failed\n");
        current_first_phy += nof_lanes;
    }

exit:
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_kbp_stif_counters_compare(
    int unit,
    int nof_elements,
    dnx_kbp_stif_element_ts * element_arr)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);
    for (i = 0; i < nof_elements; i++)
    {
        if (COMPILER_64_HI(element_arr[i].expected_packets) != 0xFFFFFFFF &&
            COMPILER_64_LO(element_arr[i].expected_packets) != 0xFFFFFFFF)
        {
            if ((COMPILER_64_HI(element_arr[i].packets_value) != COMPILER_64_HI(element_arr[i].expected_packets)) ||
                (COMPILER_64_LO(element_arr[i].packets_value) != COMPILER_64_LO(element_arr[i].expected_packets)))
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Counter Verification Failed: expected {0x%X,0x%X} packets, read from kbp {0x%X,0x%X} packets. (object_id=%d, counter_set_offset=%d) \n",
                             COMPILER_64_HI(element_arr[i].expected_packets),
                             COMPILER_64_LO(element_arr[i].expected_packets),
                             COMPILER_64_HI(element_arr[i].packets_value), COMPILER_64_LO(element_arr[i].packets_value),
                             element_arr[i].object_id, element_arr[i].counter_set_offset);
            }
        }
        if (COMPILER_64_HI(element_arr[i].expected_bytes) != 0xFFFFFFFF &&
            COMPILER_64_LO(element_arr[i].expected_bytes) != 0xFFFFFFFF)
        {
            if ((COMPILER_64_HI(element_arr[i].bytes_value) != COMPILER_64_HI(element_arr[i].expected_bytes)) ||
                (COMPILER_64_LO(element_arr[i].bytes_value) != COMPILER_64_LO(element_arr[i].expected_bytes)))
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Counter Verification Failed: expected {0x%X,0x%X} bytes, read from kbp {0x%X,0x%X} bytes. (object_id=%d, counter_set_offset=%d) \n",
                             COMPILER_64_HI(element_arr[i].expected_bytes),
                             COMPILER_64_LO(element_arr[i].expected_bytes), COMPILER_64_HI(element_arr[i].bytes_value),
                             COMPILER_64_LO(element_arr[i].bytes_value), element_arr[i].object_id,
                             element_arr[i].counter_set_offset);
            }
        }

        LOG_CLI((BSL_META
                 ("counter verification PASS: object_id=%d, offset=%d, nof_packets={0x%X, 0x%X} nof_bytes={0x%X, 0x%X} expected_p {0x%X, 0x%X} expected_b {0x%X, 0x%X} \n"),
                 element_arr[i].object_id, element_arr[i].counter_set_offset,
                 COMPILER_64_HI(element_arr[i].packets_value), COMPILER_64_LO(element_arr[i].packets_value),
                 COMPILER_64_HI(element_arr[i].bytes_value), COMPILER_64_LO(element_arr[i].bytes_value),
                 COMPILER_64_HI(element_arr[i].expected_packets), COMPILER_64_LO(element_arr[i].expected_packets),
                 COMPILER_64_HI(element_arr[i].expected_bytes), COMPILER_64_LO(element_arr[i].expected_bytes)));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_read(
    int unit,
    int single_mode,
    struct kbp_device *device,
    struct kbp_tap_db *db,
    int nof_elements,
    dnx_kbp_stif_element_ts * element_arr)
{
    kbp_status status;
    int element_idx, is_complete;
    SHR_FUNC_INIT_VARS(unit);

    status = kbp_tap_db_read_initiate(db);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_read_initiate failed: %s\n", kbp_get_status_string(status));
    }
    is_complete = 0;
    do
    {
        if (single_mode == FALSE)
        {
            status = kbp_device_scrub_tap_dma_buffer(device);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n",
                             kbp_get_status_string(status));
            }
        }
        status = kbp_tap_db_is_read_complete(db, &is_complete);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_is_read_complete failed: %s\n", kbp_get_status_string(status));
        }
    }
    while (is_complete == 0);

    /*
     * Iterate through queue id, each counter within queue id, get
     * the count value.
     */
    for (element_idx = 0; element_idx < nof_elements; element_idx++)
    {
        if (single_mode == TRUE)
        {
            status = kbp_tap_db_entry_get_value(db, element_arr[element_idx].object_id,
                                                element_arr[element_idx].counter_set_offset,
                                                &element_arr[element_idx].packets_value);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_entry_get_value failed: %s\n", kbp_get_status_string(status));
            }
        }
        else
        {
            status = kbp_tap_db_entry_pair_get_value(db, element_arr[element_idx].object_id,
                                                     element_arr[element_idx].counter_set_offset,
                                                     &element_arr[element_idx].packets_value,
                                                     &element_arr[element_idx].bytes_value);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_entry_pair_get_value failed: %s\n",
                             kbp_get_status_string(status));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_stif_tap_destroy(
    int unit,
    struct kbp_tap_db **db,
    int nof_db,
    struct kbp_tap_instruction **inst_th,
    int nof_inst)
{
    int db_id, inst_id;
    int status;
    SHR_FUNC_INIT_VARS(unit);
    for (db_id = 0; db_id < nof_db; db_id++)
    {
        /** tap database destroy */
        status = kbp_tap_db_destroy(db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_destroy failed: %s\n", kbp_get_status_string(status));
        }
    }
    /** instruction destroy */
    for (inst_id = 0; inst_id < nof_inst; inst_id++)
    {
        status = kbp_tap_instruction_destroy(inst_th[inst_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_destroy failed: %s\n", kbp_get_status_string(status));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - create ingress acl context and preselection
* \param [in] unit     - Device ID
* \param [out] id      - returned context id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_kbp_stif_acl_create(
    int unit,
    bcm_field_context_t * id)
{
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    int presel_id = 10;
    bcm_field_context_t context_id = 10;
    SHR_FUNC_INIT_VARS(unit);

    /** Create context */
    bcm_field_context_info_t_init(&context_info);
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, stage, &context_info, &context_id));

    /** Create presel_entry to map relevant traffic to the context */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = stage;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    presel_entry_data.qual_data[0].qual_mask = 0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    (*id) = context_id;
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - set specifc IN-DP and generate statistics SOID0 for a cetain in port, using ACL rule
* \param [in] unit             - Device ID
* \param [in] context_id       - context_id
* \param [in] port             - in port
* \param [in] dp               - mapped DP
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_kbp_stif_acl_run(
    int unit,
    bcm_field_context_t context_id,
    int port,
    int dp)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t fe_id;
    void *dest_char;
    int action_info_pri;
    SHR_FUNC_INIT_VARS(unit);

    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    dest_char = &(fg_info.name[0]);
    switch (dp)
    {
        case BCM_FIELD_COLOR_GREEN:
            sal_strncpy(dest_char, "DpG", sizeof(fg_info.name));
            break;
        case BCM_FIELD_COLOR_YELLOW:
            sal_strncpy(dest_char, "DpY", sizeof(fg_info.name));
            break;
        case BCM_FIELD_COLOR_RED:
            sal_strncpy(dest_char, "DpR", sizeof(fg_info.name));
            break;
        case BCM_FIELD_COLOR_BLACK:
            sal_strncpy(dest_char, "DpB", sizeof(fg_info.name));
            break;
    }

    fg_info.name[sizeof(fg_info.name) - 1] = 0;

    /** Set actions */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    fg_info.action_types[1] = bcmFieldActionExtStat0;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    action_info_pri = BCM_FIELD_ACTION_PRIORITY(0, fg_id % 16);
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_info[0].priority = action_info_pri;
    attach_info.payload_info.action_info[1].priority = action_info_pri;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /** Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    BCM_GPORT_LOCAL_SET(ent_info.entry_qual[0].value[0], port);
    ent_info.entry_qual[0].mask[0] = 0x1FF;
    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[0].value[0] = dp;
    ent_info.entry_action[1].value[0] = port;
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &fe_id));
    LOG_CLI((BSL_META("Ingress Acl Entry add: fg_id=(0x%x), fe_id=(0x%x), InPort=(0x%x), dp=(0x%x)\n"),
             fg_id, fe_id, port, dp));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Creates an ACE format and ACE entry and an ePMF field group that points to the ACE entry.
*  Generate stat-id for ETPP
* \param [in] unit             - Device ID
* \param [in] stat_id        - stat_id to be set
* \param [in] profile        - profile (PP stat profile)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_kbp_stif_ace_generate_stat_id(
    int unit,
    int nof_entries,
    int *out_port,
    int *stat_id,
    int profile)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_group_info_t epmf_fg_info;
    bcm_field_group_attach_info_t epmf_attach_info;
    bcm_field_entry_info_t epmf_entry_info;
    bcm_gport_t gport;
    int i = 0, entry_idx;
    bcm_field_context_t cint_crps_context_id_epmf = 0;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    bcm_field_group_t epmf_fg_id;
    bcm_field_entry_t epmf_entry_handle;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionStat1; /**crps*/

    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

    /*
     * Create a field group in ePMF that uses the ACE entry.
     */

    bcm_field_group_info_t_init(&epmf_fg_info);
    epmf_fg_info.fg_type = bcmFieldGroupTypeTcam;
    epmf_fg_info.nof_quals = 1;
    epmf_fg_info.stage = bcmFieldStageEgress;
    epmf_fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    epmf_fg_info.nof_actions = 1;
    /*
     * The first action decides the ACE entry that will happen.
     */
    epmf_fg_info.action_types[0] = bcmFieldActionAceEntryId;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &epmf_fg_info, &epmf_fg_id));

    bcm_field_group_attach_info_t_init(&epmf_attach_info);
    epmf_attach_info.key_info.nof_quals = epmf_fg_info.nof_quals;
    epmf_attach_info.payload_info.nof_actions = epmf_fg_info.nof_actions;
    for (i = 0; i < epmf_fg_info.nof_quals; i++)
    {
        epmf_attach_info.key_info.qual_types[i] = epmf_fg_info.qual_types[i];
    }
    for (i = 0; i < epmf_fg_info.nof_actions; i++)
    {
        epmf_attach_info.payload_info.action_types[i] = epmf_fg_info.action_types[i];
        /*
         * set different priority for each call of the function. use the port to distinguish between priorities
         */
        epmf_attach_info.payload_info.action_info[i].priority = BCM_FIELD_ACTION_PRIORITY(0, out_port[0]);
    }
    epmf_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    epmf_attach_info.key_info.qual_info[0].input_arg = 0;
    epmf_attach_info.key_info.qual_info[0].offset = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, epmf_fg_id, cint_crps_context_id_epmf, &epmf_attach_info));

    for (entry_idx = 0; entry_idx < nof_entries; entry_idx++)
    {
        bcm_field_ace_entry_info_t_init(&ace_entry_info);

        ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

        ace_entry_info.entry_action[0].type = bcmFieldActionStat1;
        /** composed of command (profile) and stat_id. valid is 1.*/
        ace_entry_info.entry_action[0].value[0] = profile & 0x1F;
        ace_entry_info.entry_action[0].value[1] = stat_id[entry_idx];
        ace_entry_info.entry_action[0].value[2] = 1;

        SHR_IF_ERR_EXIT(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle));

        bcm_field_entry_info_t_init(&epmf_entry_info);
        epmf_entry_info.nof_entry_quals = epmf_fg_info.nof_quals;
        epmf_entry_info.nof_entry_actions = epmf_fg_info.nof_actions;
        for (i = 0; i < epmf_fg_info.nof_quals; i++)
        {
            epmf_entry_info.entry_qual[i].type = epmf_fg_info.qual_types[i];
        }
        for (i = 0; i < epmf_fg_info.nof_actions; i++)
        {
            epmf_entry_info.entry_action[i].type = epmf_fg_info.action_types[i];
        }
        BCM_GPORT_LOCAL_SET(gport, out_port[entry_idx]);
        epmf_entry_info.entry_qual[0].value[0] = gport;
        epmf_entry_info.entry_qual[0].mask[0] = 0x1FF;
        epmf_entry_info.entry_action[0].value[0] = ace_entry_handle;

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, epmf_fg_id, &epmf_entry_info, &epmf_entry_handle));

        LOG_CLI((BSL_META("ACE format (%d), ACE entry (%d) and ePMF FG (%d) created.\n"),
                 ace_format_id, ace_entry_handle, epmf_fg_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configuration of dnx and kbp stif test. gather statistics for voq.
 *  configuration:
 *  One device, max cores configuration, 2 ports of 200G per core, billing mode.
 *  Count Voqs per core.
 *  2 databases. one for each core.
 *  Each database holds counter_set=8, as follow:
 *  Offset-0: Forward, DP=0
 *  Offset-1: Forward, DP=1
 *  Offset-2: Forward, DP=2
 *  Offset-3: Forward, DP=3
 *  Offset-4: Drop, DP=0
 *  Offset-5: Drop, DP=1
 *  Offset-6: Drop, DP=2
 *  Offset-7: Drop, DP=3
 *  Ingress record format:
 *  Bits 0..15: queue_id.
 *  Bits 16: disposition (forward=0, drop=1)
 *  Bits 17..18: DP
 *  Bits 19..63: hole
 *  Bits 64..77: packet size
 *  Bits 78..79: record type.
 * \param [in] unit         - Number of hardware unit used
 * \param [in] args         - args
 * \param [in] sand_control - sand_control
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
#define DNX_KBP_STIF_VOQ_DB_TEST_NOF_PORTS (14)
static shr_error_e
dnx_kbp_stif_voq_db_test(
    int unit,
    char *txfir_config,
    char *link_training_enable)
{
    rhhandle_t ctest_soc_set_h = NULL;
    struct kbp_tap_record *rec_th[2];
    struct kbp_tap_db_pointer *ptr[2];
    struct kbp_tap_db_attribute *attr[2];
    struct kbp_tap_db_value *value;
    int ing_rec_size, null_rec_size, packet_size = 14;
    int attr_value, counter_offset;
    int db_id;
    int num_queues = dnx_data_ipq.queues.nof_queues_get(unit);
    int hole_len, ptr_len, attr_len, set_size = 8;
    kbp_status status;
    bcm_port_t from_port[2] = { 218, 219 };
    bcm_port_t to_port[2][DNX_KBP_STIF_VOQ_DB_TEST_NOF_PORTS] = {
        {1, 23, 26, 27, 28, 29, 30, 31, 32, 35, 36, 37, 38},
        {17, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52}
    };
    bcm_color_t dp[2] = { BCM_FIELD_COLOR_YELLOW, BCM_FIELD_COLOR_BLACK };
    rhhandle_t packet_h = NULL;
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    bcm_stat_stif_source_t source;
    bcm_stat_stif_record_format_element_t ingress_record_format_elements[4];
    int nof_elements_ingress = 4;
    dnx_kbp_stif_element_ts kbp_stif_elements[64];
    bcm_switch_kbp_info_t kbp_info;
    int kbp_thread_id;

    ctest_soc_property_t ctest_soc_property_1[] = {
        {"port_init_cl72_ext_stat0", link_training_enable},
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "80"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ucode_port_1*", NULL},
        {"ucode_port_13*", NULL},
        {"ucode_port_14*", NULL},
        {"ucode_port_15*", NULL},
        {"ucode_port_16*", NULL},
        {"ucode_port_17*", NULL},
        {"ucode_port_20*", NULL},
        {"ucode_port_1", "CGE5:core_0.1"},
        {"ucode_port_23", "XLGE2_3:core_0.23"},
        {"ucode_port_26", "XLGE2_4:core_0.26"},
        {"ucode_port_27", "XLGE2_5:core_0.27"},
        {"ucode_port_28", "XLGE2_6:core_0.28"},
        {"ucode_port_29", "XLGE2_7:core_0.29"},
        {"ucode_port_30", "XLGE2_0:core_0.30"},
        {"ucode_port_31", "CGE10:core_0.31"},
        {"ucode_port_32", "CGE11:core_0.32"},
        {"ucode_port_35", "CGE12:core_0.35"},
        {"ucode_port_36", "CGE4:core_0.36"},
        {"ucode_port_37", "CGE9:core_0.37"},
        {"ucode_port_38", "XLGE2_1:core_0.38"},
        {"serdes_tx_taps_ext_stat_lane0", txfir_config},
        {"serdes_tx_taps_ext_stat_lane1", txfir_config},
        {"serdes_tx_taps_ext_stat_lane2", txfir_config},
        {"serdes_tx_taps_ext_stat_lane3", txfir_config},
        {"ucode_port_34", NULL},
        {"port_init_speed_ext_stat2", NULL},
        {"port_init_cl72_33", link_training_enable},
        {"flexe_device_mode*", NULL},
        {NULL}
    };
    ctest_soc_property_t ctest_soc_property_2[] = {
        {"port_init_cl72_ext_stat0", link_training_enable},
        {"port_init_cl72_ext_stat2", link_training_enable},
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "80"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ucode_port_20*", NULL},
        {"ucode_port_23", "XLGE2_23:core_0.23"},
        {"ucode_port_26", "XLGE2_12:core_0.26"},
        {"ucode_port_27", "XLGE2_13:core_0.27"},
        {"ucode_port_28", "XLGE2_14:core_0.28"},
        {"ucode_port_29", "XLGE2_15:core_0.29"},
        {"ucode_port_30", "XLGE2_16:core_0.30"},
        {"ucode_port_31", "XLGE2_17:core_0.31"},
        {"ucode_port_32", "XLGE2_18:core_0.32"},
        {"ucode_port_35", "XLGE2_19:core_0.35"},
        {"ucode_port_36", "XLGE2_20:core_0.36"},
        {"ucode_port_37", "XLGE2_21:core_0.37"},
        {"ucode_port_38", "XLGE2_22:core_0.38"},
        {"ucode_port_41", "XLGE2_43:core_1.41"},
        {"ucode_port_42", "XLGE2_26:core_1.42"},
        {"ucode_port_43", "XLGE2_27:core_1.43"},
        {"ucode_port_44", "XLGE2_28:core_1.44"},
        {"ucode_port_45", "XLGE2_29:core_1.45"},
        {"ucode_port_46", "XLGE2_30:core_1.46"},
        {"ucode_port_47", "XLGE2_31:core_1.47"},
        {"ucode_port_48", "XLGE2_32:core_1.48"},
        {"ucode_port_49", "XLGE2_33:core_1.49"},
        {"ucode_port_50", "XLGE2_34:core_1.50"},
        {"ucode_port_51", "XLGE2_35:core_1.51"},
        {"ucode_port_52", "XLGE2_46:core_1.52"},
        {"serdes_tx_taps_ext_stat_lane0", txfir_config},
        {"serdes_tx_taps_ext_stat_lane1", txfir_config},
        {"serdes_tx_taps_ext_stat_lane2", txfir_config},
        {"serdes_tx_taps_ext_stat_lane3", txfir_config},
        {"serdes_tx_taps_ext_stat_lane4", txfir_config},
        {"serdes_tx_taps_ext_stat_lane5", txfir_config},
        {"serdes_tx_taps_ext_stat_lane6", txfir_config},
        {"serdes_tx_taps_ext_stat_lane7", txfir_config},
        {"port_init_cl72_33", link_training_enable},
        {"port_init_cl72_34", link_training_enable},
        {"flexe_device_mode*", NULL},
        {NULL}
    };
    ctest_soc_property_t *ctest_soc_property;
    bcm_field_context_t context_id;
    bcm_cosq_drop_reason_t drop_reasons_array[2];

    SHR_FUNC_INIT_VARS(unit);

    ctest_soc_property = (nof_cores == 1) ? ctest_soc_property_1 : ctest_soc_property_2;
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /*
     * dnx configuration
     */

    /** disable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 0));
    /** connect IngressEnqueue source to the logical port */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
    if (nof_cores > 1)
    {
        source.core = 1;
        source.src_type = bcmStatStifSourceIngressEnqueue;
        SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 34));
    }

    /** define the record format */
    /**
     *
     *  *  INGRESS
     *  record size set to 80b
     *  filled are 36b
     *  ----------------------------------------------------------------------------------------
     * |RecordType (2b)| PacketSize (14b)| Hole (45b)|DP(2b)|Disposition(1b)|queue_id(16b)
     *  ----------------------------------------------------------------------------------------
     */
    ingress_record_format_elements[0].element_type = bcmStatStifRecordElementIngressQueueNumber;
    ingress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[1].element_type = bcmStatStifRecordElementIngressDispositionIsDrop;
    ingress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[2].element_type = bcmStatStifRecordElementIngressIncomingDropPrecedence;
    ingress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[3].element_type = bcmStatStifRecordElementPacketSize;
    ingress_record_format_elements[3].mask = BCM_STAT_FULL_MASK;
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_record_format_elements));
    /** enable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 1));

    /** disable drop reasons groups 0..3, because record format contain only bit 2 */
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterGlobalResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterWredDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterOcbResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    /** set the expected drop reason in Misc group. group number 6 */
    drop_reasons_array[0] = bcmCosqDropReasonVoqWordsTotalFreeSharedReject;
    drop_reasons_array[1] = bcmCosqDropReasonDropPrecedenceLevelReject;
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterMiscDrop, drop_reasons_array, 2));

    kbp_stif_config.nof_db = nof_cores < 2 ? nof_cores : 2;
    kbp_stif_config.nof_instructions = kbp_stif_config.nof_db;
    /*
     * kbp configuration
     */
    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    kbp_stif_config.device = (struct kbp_device *) kbp_info.device_p;
    ing_rec_size = 80;
    null_rec_size = 80;

    /** Set the ingress and null record lengths */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    /** Enable counter compression since packet and byte count mode */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_COUNTER_COMPRESSION, 1);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    /** Create tap databases */
    for (db_id = 0; db_id < kbp_stif_config.nof_db; db_id++)
    {

        if (kbp_mngr_smt_enabled(unit))
        {
            kbp_thread_id = db_id % BCM_KBP_MAX_NOF_THREADS;
            /** Port-0 is mapped to Thread-0 and Port-2 is mapped to Thread-1 */
            kbp_stif_config.device_th[db_id] = (struct kbp_device *) kbp_info.thread_p[kbp_thread_id];
        }

        /** Entry type is PAIR since packet and byte count mode */
        status = kbp_tap_db_init(kbp_stif_config.device, db_id,
                                 num_queues, KBP_TAP_ENTRY_TYPE_PAIR, set_size, &kbp_stif_config.db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize pointer */
        ptr_len = dnx_data_ipq.queues.queue_id_bits_get(unit);
        status = kbp_tap_db_pointer_init(kbp_stif_config.db[db_id], &ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_pointer_add_field(ptr[db_id], "QUEUE_ID", ptr_len);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_pointer(kbp_stif_config.db[db_id], ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_set_pointer failed: %s\n", kbp_get_status_string(status));
        }
        /*
         * Add value field
         */
        status = kbp_tap_db_value_init(kbp_stif_config.db[db_id], &value);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_value_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_value_set_field(value, "PACKET_SIZE", packet_size);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_value_set_field failed: %s\n", kbp_get_status_string(status));
        }

        /** Initialize attribute, add fields. attr_len = 1 + 2 */
        attr_len = 3;
        status = kbp_tap_db_attribute_init(kbp_stif_config.db[db_id], &attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DROP_PRECEDENCE", 2);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Attribute value to counter offset mapping:
         *
         * Offset-0: Forward, DP = 0, Offset-1: Forward, DP = 1
         * Offset-2: Forward, DP = 2, Offset-3: Forward, DP = 3
         * Offset-4: Drop,    DP = 0, Offset-5: Drop, DP = 1
         * Offset-6: Drop,    DP = 2, Offset-7: Drop, DP = 3
         */
        for (attr_value = 0, counter_offset = 0; attr_value < set_size; attr_value++, counter_offset++)
        {
            status = kbp_tap_db_map_attr_value_to_entry(kbp_stif_config.db[db_id], attr_value, counter_offset);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                             kbp_get_status_string(status));
            }
        }
        /*
         * instructions, on Th-i
         * The ingress record looks as follows:
         *
         * Bits 78..79: record type
         * Bits 64..77: packet size
         * Bits 19..63: hole
         * Bits 17..18: DP
         * Bits 16: disposition (forward = 0, drop = 1)
         * Bits 0..15: queue_id.
         */
        if (kbp_mngr_smt_enabled(unit))
        {
            status = kbp_tap_instruction_init(kbp_stif_config.device_th[db_id], db_id, KBP_TAP_INSTRUCTION_TYPE_INGRESS,
                                              &kbp_stif_config.inst_th[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device_th[db_id], KBP_TAP_RECORD_TYPE_INGRESS, &rec_th[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }
        }
        else
        {
            status = kbp_tap_instruction_init(kbp_stif_config.device, db_id, KBP_TAP_INSTRUCTION_TYPE_INGRESS,
                                              &kbp_stif_config.inst_th[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_INGRESS, &rec_th[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }

        }

        hole_len = ing_rec_size - (2 + packet_size + attr_len + ptr_len);

        status = kbp_tap_record_add_field(rec_th[db_id], "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "PACKET_SIZE", packet_size, KBP_TAP_RECORD_FIELD_VALUE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "HOLE", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "DROP_PRECEDENCE", 2, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th[db_id], "QUEUE_ID", ptr_len, KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Associate record with the instruction. Add database and finally install the instruction.
         */
        status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[db_id], rec_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_add_db(kbp_stif_config.inst_th[db_id], kbp_stif_config.db[db_id], 0, num_queues);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_install(kbp_stif_config.inst_th[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
        }
    }

   /** Lock device configuration */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    /*
     * Start pumping the Stats traffic. Periodically call
     * DMA scrub API to clear dynamically evicted counters.
     */
    status = kbp_device_scrub_tap_dma_buffer(kbp_stif_config.device);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
    }

    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_create(unit, &context_id));
    sal_msleep(5000);

    /** handle traffic +  read counterd and verify for each database*/
    for (int core_id = 0; core_id < nof_cores; core_id++)
    {
        uint64 packet_count, bytes_count;
        bcm_port_mapping_info_t mapping_info;
        bcm_port_interface_info_t interface_info;
        uint32 flags;
        /** map in port to TC and DP */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_run(unit, context_id, from_port[core_id], dp[core_id]));
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port[core_id], to_port[core_id][0], 1));
        /*
         * sanke in order to get to 200G traffic rate
         */
        for (int i = 0; i < (DNX_KBP_STIF_VOQ_DB_TEST_NOF_PORTS - 1); i++)
        {
            bcm_port_loopback_set(0, to_port[core_id][i], BCM_PORT_LOOPBACK_MAC);
            bcm_port_force_forward_set(0, to_port[core_id][i], to_port[core_id][i + 1], 1);
        }
        SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port[core_id], &flags, &interface_info, &mapping_info));
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));

        /** small traffic rate */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core_id, packet_h, 72, 1,
                                                       BCM_SAT_GTF_RATE_IN_BYTES, 1000, 10000,
                                                       SAT_GRANULARITY_SET_BY_DIAG));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port[core_id], packet_h, 0));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
        LOG_CLI((BSL_META("SAT generated {0x%x,0x%x} Packets {0x%x,0x%x} Bytes\n"),
                 COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count),
                 COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count)));
        /*
         * the packet launcher send also packet from CPU COMPILER_64_ADD_32(packet_count, 1);
         * COMPILER_64_ADD_32(bytes_count, 64);
         */
        sal_msleep(5000);
        for (int j = 0; j < DNX_KBP_STIF_VOQ_DB_TEST_NOF_PORTS; j++)
        {
            /** read counters */
            SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get
                            (unit, 0, to_port[core_id][j], (int *) &kbp_stif_elements[0].object_id));

            for (int i = 0; i < set_size; i++)
            {
                /** first port in snake get the data from SAT, so need to take the packets and bytes from SAT report */
                if (j > 0)
                {
                    SHR_IF_ERR_EXIT(bcm_stat_get
                                    (unit, to_port[core_id][j - 1], snmpEtherStatsTXNoErrors, &packet_count));
                    /** bytes can be checked just for the first port, which get the data from SAT port and we control the bytes size */
                    /** all other ports add internal headers/compensation etc, and we cannot predict the size of this data in this test */
                    COMPILER_64_SET(bytes_count, 0xFFFFFFFF, 0xFFFFFFFF);
                }
                kbp_stif_elements[i].counter_set_offset = i;
                kbp_stif_elements[i].object_id = kbp_stif_elements[0].object_id;
                /** offset= {disposition(1), dp(2)} */
                /** for first db: expected offset=1, disposition=forward, dp=yellow */
                /** first port will get dp yellow, others port dp is green */
                /** for second db only first port is relevant, all packets are dropped: expected offset=7, disposition=drop, dp=black */
                if ((i == 1 && core_id == 0 && j == 0) ||
                    (i == 7 && core_id == 1 && j == 0) || (i == 0 && core_id == 0 && j > 0))
                {
                    COMPILER_64_COPY(kbp_stif_elements[i].expected_packets, packet_count);
                    COMPILER_64_COPY(kbp_stif_elements[i].expected_bytes, bytes_count);
                }
                else
                {
                    COMPILER_64_ZERO(kbp_stif_elements[i].expected_packets);
                    COMPILER_64_ZERO(kbp_stif_elements[i].expected_bytes);
                }
            }
            SHR_IF_ERR_EXIT(dnx_kbp_stif_read(unit, FALSE, kbp_stif_config.device,
                                              kbp_stif_config.db[core_id], set_size, kbp_stif_elements));

            /*
             * verify counters
             */
            SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));
        }
    }

    /** Database destroy */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_tap_destroy
                    (unit, kbp_stif_config.db, kbp_stif_config.nof_db, kbp_stif_config.inst_th,
                     kbp_stif_config.nof_instructions));

exit:
    diag_sand_packet_free(unit, packet_h);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/**
 * \brief - dnx and kbp stif test.
 *  configuration:
 *  if JR2:
 *  2 ports of 200G, billing mode.
 *  if Q2A:
 *  2 ports of 100G, billing mode.
 *  if J2C:
 *  1 ports of 200G, billing mode.
 *  Ingress record format:
 *  Record type (ingress) - bits 94..95
 *  Packet Size - bits 80..93
 *  DP - bits 78..79
 *  Disposition - bit 77
 *  Drop-Reason - bit 76 (take only MSB bit from 3 bits Drop reason)
 *  SOID3 (VoQ-ID) - bits 60..75
 *  SOID2  - bits 40..59
 *  SOID1  - bits 20..39
 *  SOID0  - bits 0..19
 *  Total: 96 bits
 *
 *  Egress record format:
 *  Record type (egress) - bits 62..63
 *  Packet Size - bits 48..61
 *  Hole - bits 44..47
 *  is_MultiCast: bits 42
 *  DropReason - bits 40..41
 *  SOID1 (ACE PTR) - bits 20..39
 *  SOID0  - bits 0..19
 *  Total: 64b
 *
 *
 *  Create a database per SOID, Total 6 databases (4 ingress, 2 egress).
 *  Ingress SOID0 - acl SOID: counter_set=4 according to disposition+ Drop-Reason bit.
 *  Offset=0 - {disposition=0, Drop-Reason=0},
 *  Offset=1 - {disposition=0, Drop-Reason=1}.
 *  Offset=2 - {disposition=1, Drop-Reason=0}.
 *  Offset=3 - {disposition=1, Drop-Reason=1}.
 *      Count only packets.
 *
 *  Ingress SOID3 - queue SOID: counter_set=1.
 *      Count packets and bytes.
 *
 *  Egress SOID1 - ace ptr SOID: counter_set=8 according to DropReason+Is_multicast.
 *  Offset=0 - {DropReason=0,is_Multicast=0}
 *  Offset=1 - {DropReason=1,is_Multicast=0}
 *  Offset=2 - {DropReason=2,is_Multicast=0}
 *  Offset=3 - {DropReason=3,is_Multicast=0}
 *  Offset=4 - {DropReason=0,is_Multicast=1}
 *  Offset=5 - {DropReason=1,is_Multicast=1}
 *  Offset=6 - {DropReason=2,is_Multicast=1}
 *  Offset=7 - {DropReason=3,is_Multicast=1}
 *      Count only bytes.
 *
 * \param [in] unit         - Number of hardware unit used
 * \param [in] nof_stif_ports - number of stif ports configured
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
#define DNX_KBP_STIF_ING_EGR_TEST_NOF_DB (3)
#define DNX_KBP_STIF_ING_EGR_TEST_NOF_INST (4)

static shr_error_e
dnx_kbp_stif_ing_egr_config_example(
    int unit,
    int nof_stif_ports)
{
    uint32 nof_threads = 0;
    struct kbp_tap_record *rec_th;
    struct kbp_tap_db_value *value;
    int db_id;
    int max_qnum_mask = DNX_DATA_MAX_IPQ_QUEUES_QUEUE_ID_BITS;
    char name[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB][32] = { "ING_ACL_PTR", "ING_QUEUE_ID", "EGR_ACE_PTR" };
    int qnum_mask = dnx_data_ipq.queues.queue_id_bits_get(unit);
    int ptr_len[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB] = { 20, qnum_mask, 20 };      /* {ing-SOID0, ing-SOID3 (queues), *
                                                                                 * egr-SOID} */
    int set_size[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB] = { 4, 1, 8 };
    struct kbp_tap_db_pointer *ptr[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB];
    struct kbp_tap_db_attribute *attr[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB];
    int ing_rec_size, egr_rec_size, null_rec_size;
    int packet_size = 14;
    int attr_value, counter_offset;
    int hole_len;
    kbp_status status;
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    bcm_stat_stif_source_t source;
    int nof_elements_ingress = 7, nof_elements_egress = 5;
    bcm_switch_kbp_info_t kbp_info;
    bcm_stat_stif_record_format_element_t ingress_format_elements[7] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj1, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj2, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressQueueNumber, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressTmDropReason, 0x4},
    {bcmStatStifRecordElementIngressDispositionIsDrop, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    bcm_stat_stif_record_format_element_t egress_format_elements[5] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj1, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementEgressMetaDataPpDropReason, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementEgressMetaDataMultiCast, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    bcm_cosq_drop_reason_t drop_reasons_array[2];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * dnx configuration
     */
    /** disable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 0));
    /** connect IngressEnqueue source to the logical port */
    /** if one core device, connect ingress to one port and egress to the other port (if exist)*/
    /** if multiple cores, connect each core (ing+egr) to  one port */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
    source.src_type = bcmStatStifSourceEgressDequeue;
    if (nof_cores > 1)
    {
        SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
        source.core = 1;
        source.src_type = bcmStatStifSourceIngressEnqueue;
        SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 34));
    }
    source.src_type = bcmStatStifSourceEgressDequeue;
    /** if only one port exist, connect Egress to that port */
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, (nof_stif_ports == 1) ? 33 : 34));

    /** define the record format */
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_format_elements));
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_EGRESS, nof_elements_egress, egress_format_elements));

    /** enable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 1));

    /** disable drop reasons groups 0..3, because record format contain only bit 2 */
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterGlobalResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterWredDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterOcbResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterVoqResourcesDrop, NULL, 0));
    /** set the expected drop reason in Misc group. group number 6 */
    drop_reasons_array[0] = bcmCosqDropReasonVoqWordsTotalFreeSharedReject;
    drop_reasons_array[1] = bcmCosqDropReasonDropPrecedenceLevelReject;
    SHR_IF_ERR_EXIT(bcm_stat_group_drop_reasons_set(unit, 0, bcmStatCounterGroupFilterMiscDrop, drop_reasons_array, 2));

    /*
     * kbp configuration
     */
    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    kbp_stif_config.device = (struct kbp_device *) kbp_info.device_p;
    kbp_stif_config.nof_db = DNX_KBP_STIF_ING_EGR_TEST_NOF_DB;
    kbp_stif_config.nof_instructions = kbp_mngr_smt_enabled(unit) ? DNX_KBP_STIF_ING_EGR_TEST_NOF_INST : 2;

    ing_rec_size = 96;
    egr_rec_size = 64;
    null_rec_size = 96;

    /** Set the ingress and null record lengths */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_EGRESS_REC_SIZE, egr_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    /** Enable counter compression since packet and byte count mode */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_COUNTER_COMPRESSION, 1);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    if (kbp_mngr_smt_enabled(unit))
    {
        /** Port-0 is mapped to Thread-0  */
        kbp_stif_config.device_th[0] = (struct kbp_device *) kbp_info.thread_p[0];
        /** Port-2 is mapped to Thread-1 */
        kbp_stif_config.device_th[1] = (struct kbp_device *) kbp_info.thread_p[1];
    }

    /** Create tap databases */
    for (db_id = 0; db_id < kbp_stif_config.nof_db; db_id++)
    {
        /** Entry type is PAIR since packet and byte count mode. */
        /** configure all databases size according to max db, just to simplify the test */
        status = kbp_tap_db_init(kbp_stif_config.device, db_id, dnx_data_ipq.queues.nof_queues_get(unit),
                                 KBP_TAP_ENTRY_TYPE_PAIR, set_size[db_id], &kbp_stif_config.db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize pointer */
        status = kbp_tap_db_pointer_init(kbp_stif_config.db[db_id], &ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_pointer_add_field(ptr[db_id], name[db_id], ptr_len[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_pointer(kbp_stif_config.db[db_id], ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_set_pointer failed: %s\n", kbp_get_status_string(status));
        }
        /*
         * Add value field
         */
        status = kbp_tap_db_value_init(kbp_stif_config.db[db_id], &value);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_value_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_value_set_field(value, "PACKET_SIZE", packet_size);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_value_set_field failed: %s\n", kbp_get_status_string(status));
        }

        /** Initialize attribute*/
        status = kbp_tap_db_attribute_init(kbp_stif_config.db[db_id], &attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_init failed: %s\n", kbp_get_status_string(status));
        }
        if (!sal_strncmp(name[db_id], "ING_ACL_PTR", DNX_KBP_STIF_STRING_SIZE))
        {
            /*
             * Offset=0 - {disposition=forward, Drop-Reason=0} Offset=1 - {disposition=forward, Drop-Reason=1}
             * Offset=2 - {disposition=drop, Drop-Reason=0} Offset=3 - {disposition=drop, Drop-Reason=1}
             */
            status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_db_attribute_add_field(attr[db_id], "DROP_REASON", 1);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
        }
        if (!sal_strncmp(name[db_id], "ING_QUEUE_ID", DNX_KBP_STIF_STRING_SIZE))
        {
            status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            /*
             * QUEUE database set_size=0, all goes into offset 0
             */
            for (attr_value = 0, counter_offset = 0; attr_value <= set_size[db_id]; attr_value++)
            {
                status = kbp_tap_db_map_attr_value_to_entry(kbp_stif_config.db[db_id], attr_value, counter_offset);
                if (status != KBP_OK)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                                 kbp_get_status_string(status));
                }
            }
        }
        if (!sal_strncmp(name[db_id], "EGR_ACE_PTR", DNX_KBP_STIF_STRING_SIZE))
        {
            /*
             *  Offset=0 - {DropReason=0,is_Multicast=0}
             *  Offset=1 - {DropReason=1,is_Multicast=0}
             *  Offset=2 - {DropReason=2,is_Multicast=0}
             *  Offset=3 - {DropReason=3,is_Multicast=0}
             *  Offset=4 - {DropReason=0,is_Multicast=1}
             *  Offset=5 - {DropReason=1,is_Multicast=1}
             *  Offset=6 - {DropReason=2,is_Multicast=1}
             *  Offset=7 - {DropReason=3,is_Multicast=1}
             */
            status = kbp_tap_db_attribute_add_field(attr[db_id], "DROP_REASON", 2);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_db_attribute_add_field(attr[db_id], "MULTICAST", 1);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
            }
        }
        /** if not QUEUE db, set full spread over the counter set */
        if (sal_strncmp(name[db_id], "ING_QUEUE_ID", DNX_KBP_STIF_STRING_SIZE))
        {
            for (attr_value = 0, counter_offset = 0; attr_value < set_size[db_id]; attr_value++, counter_offset++)
            {
                status = kbp_tap_db_map_attr_value_to_entry(kbp_stif_config.db[db_id], attr_value, counter_offset);
                if (status != KBP_OK)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                                 kbp_get_status_string(status));
                }
            }
        }
    }

    /*
     * Ingress record format:
     * Record type (ingress) - bits 94..95
     * Packet Size - bits 80..93
     * DP - bits 78..79 *
     * Disposition - bit 77
     * Drop-Reason - bit 76 (take only MSB bit from 3 bits Drop reason)
     * SOID3 (VoQ-ID) - bits 60..75
     * SOID2 - bits 40..59
     * SOID1 - bits 20..39
     * SOID0 - bits 0..19
     * Total: 96 bits
     */
    /*
     * loop over device_th - 2 device thread (one per port), 4 instructions (ingress*2 threads, egress*2 threads)
     * at single port mode only one thread is neeeded and 1 instruction per ingress/egress
     */

    nof_threads = kbp_mngr_smt_enabled(unit) ? 2 : 1;

    for (int idx = 0; idx < nof_threads; idx++)
    {
        if (kbp_mngr_smt_enabled(unit))
        {
            status =
                kbp_tap_instruction_init(kbp_stif_config.device_th[idx], (idx * 2),
                                         KBP_TAP_INSTRUCTION_TYPE_INGRESS, &kbp_stif_config.inst_th[idx * 2]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device_th[idx], KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }
        }
        else
        {
            status =
                kbp_tap_instruction_init(kbp_stif_config.device, (idx * 2),
                                         KBP_TAP_INSTRUCTION_TYPE_INGRESS, &kbp_stif_config.inst_th[idx * 2]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }

        }
        hole_len = 0;
        status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "PACKET_SIZE", packet_size, KBP_TAP_RECORD_FIELD_VALUE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        hole_len = max_qnum_mask - qnum_mask;
        status = kbp_tap_record_add_field(rec_th, "HOLE_A", hole_len, KBP_TAP_RECORD_FIELD_HOLE);       /* instead of
                                                                                                         * the Queue
                                                                                                         * number align 
                                                                                                         * * * * * * *
                                                                                                         * * * * * * *
                                                                                                         * * * * * * *
                                                                                                         * * * * hole */
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }

        status = kbp_tap_record_add_field(rec_th, "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "DROP_REASON", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, name[1], ptr_len[1], KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        hole_len = 2 * ptr_len[0];
        status = kbp_tap_record_add_field(rec_th, "HOLE_B", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, name[0], ptr_len[0], KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Associate record with the instruction. Add database and finally install the instruction.
         */
        status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[idx * 2], rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_add_db(kbp_stif_config.inst_th[idx * 2], kbp_stif_config.db[0], 0,
                                            dnx_data_ipq.queues.nof_queues_get(unit));
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_add_db(kbp_stif_config.inst_th[idx * 2], kbp_stif_config.db[1], 0,
                                            dnx_data_ipq.queues.nof_queues_get(unit));
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }

        status = kbp_tap_instruction_install(kbp_stif_config.inst_th[idx * 2]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * Egress record format:
         * Record type (egress) - bits 62..63
         * Packet Size - bits 48..61
         * Hole - bits 43..47
         * is_MultiCast: bits 42
         * DropReason - bits 40..41
         * SOID1 (ACE PTR) - bits 20..39
         * SOID0 - bits 0..19
         * Total:64b
         */
        if (kbp_mngr_smt_enabled(unit))
        {
            status =
                kbp_tap_instruction_init(kbp_stif_config.device_th[idx], (idx * 2 + 1), KBP_TAP_INSTRUCTION_TYPE_EGRESS,
                                         &kbp_stif_config.inst_th[idx * 2 + 1]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device_th[idx], KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }
        }
        else
        {
            status =
                kbp_tap_instruction_init(kbp_stif_config.device, (idx * 2 + 1), KBP_TAP_INSTRUCTION_TYPE_EGRESS,
                                         &kbp_stif_config.inst_th[idx * 2 + 1]);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
            }
            status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
            }

        }
        packet_size = 14;
        hole_len = 5;
        status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "PACKET_SIZE", packet_size, KBP_TAP_RECORD_FIELD_VALUE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_A", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "MULTICAST", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, "DROP_REASON", 2, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_add_field(rec_th, name[2], ptr_len[2], KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        hole_len = ptr_len[0];
        status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_B", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[idx * 2 + 1], rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_add_db(kbp_stif_config.inst_th[idx * 2 + 1], kbp_stif_config.db[2], 0,
                                            dnx_data_ipq.queues.nof_queues_get(unit));
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_instruction_install(kbp_stif_config.inst_th[idx * 2 + 1]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_stif_ing_egr_send_and_check(
    int unit)
{
    int set_size[DNX_KBP_STIF_ING_EGR_TEST_NOF_DB] = { 4, 1, 8 };
    kbp_status status;
    bcm_port_t from_port[2] = { 218, 219 };
    bcm_port_t to_port[2] = { 1, 17 };
    bcm_color_t dp[2] = { BCM_FIELD_COLOR_YELLOW, BCM_FIELD_COLOR_BLACK };
    rhhandle_t packet_h[2] = { NULL, NULL };
    int core_id, nof_cores = dnx_data_device.general.nof_cores_get(unit);
    dnx_kbp_stif_element_ts kbp_stif_elements[64];
    uint64 packet_count[2], bytes_count[2];
    bcm_field_context_t context_id;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start pumping the Stats traffic. Periodically call
     * DMA scrub API to clear dynamically evicted counters.
     */
    for (int i = 0; i < 10; i++)
    {
        sal_msleep(1000);
        status = kbp_device_scrub_tap_dma_buffer(kbp_stif_config.device);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
        }
    }
    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_create(unit, &context_id));
    /** generate statistics */
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        int stat_pp_profile;
        bcm_stat_pp_profile_info_t pp_profile;
        /** ingress */
        /** map in port to DP and set SOID0=port */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_run(unit, context_id, from_port[core_id], dp[core_id]));
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port[core_id], to_port[core_id], 1));

        if (core_id == 0)
        {
            /** egress */
            bcm_stat_pp_profile_info_t_init(&pp_profile);
            pp_profile.counter_command_id = 7; /** external statistics command id 1 */
            SHR_IF_ERR_EXIT(bcm_stat_pp_profile_create(unit, 0,
                                                       bcmStatCounterInterfaceEgressTransmitPp, &stat_pp_profile,
                                                       &pp_profile));
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ace_generate_stat_id
                            (unit, 1, &to_port[core_id], &to_port[core_id], stat_pp_profile));
        }
    }
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port[core_id], &flags, &interface_info, &mapping_info));
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h[core_id]));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[core_id], "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h[core_id], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        /*
         * sending from SAT 80G. packet size=72 bytes. ==> 60G/8/72*(96+64 bits)=16.66G rate in the stif port
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core_id, packet_h[core_id], 72, 1,
                                                       BCM_SAT_GTF_RATE_IN_BYTES, 60000000, 10000,
                                                       SAT_GRANULARITY_SET_BY_DIAG));
    }
    status = kbp_device_scrub_tap_dma_buffer(kbp_stif_config.device);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
    }
    /** send streams */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, nof_cores));
    status = kbp_device_scrub_tap_dma_buffer(kbp_stif_config.device);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
    }
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats
                        (unit, packet_h[core_id], &bytes_count[core_id], &packet_count[core_id]));
        LOG_CLI((BSL_META("SAT generated {0x%x,0x%x} Packets {0x%x,0x%x} Bytes\n"),
                 COMPILER_64_HI(packet_count[core_id]), COMPILER_64_LO(packet_count[core_id]),
                 COMPILER_64_HI(bytes_count[core_id]), COMPILER_64_LO(bytes_count[core_id])));
    }

    for (int i = 0; i < 20; i++)
    {
        sal_msleep(1000);
        status = kbp_device_scrub_tap_dma_buffer(kbp_stif_config.device);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_scrub_tap_dma_buffer failed: %s\n", kbp_get_status_string(status));
        }
    }

    LOG_CLI((BSL_META("--- QUEUE DATABASE VERIFY --- \n")));
    /** read counters from ingress voq database (SOID3)*/
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get
                        (unit, 0, to_port[core_id], (int *) &kbp_stif_elements[0].object_id));
        kbp_stif_elements[0].counter_set_offset = 0;
        COMPILER_64_COPY(kbp_stif_elements[0].expected_packets, packet_count[core_id]);
        COMPILER_64_COPY(kbp_stif_elements[0].expected_bytes, bytes_count[core_id]);
        SHR_IF_ERR_EXIT(dnx_kbp_stif_read
                        (unit, FALSE, kbp_stif_config.device, kbp_stif_config.db[1], set_size[1], kbp_stif_elements));
        /** verify counters */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size[1], kbp_stif_elements));
    }
    LOG_CLI((BSL_META("--- INGRESS ACL DATABASE VERIFY --- \n")));

    /** read counters from ingress acl database (SOID0)*/
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        kbp_stif_elements[0].object_id = from_port[core_id];
        for (int i = 0; i < set_size[0]; i++)
        {
            kbp_stif_elements[i].counter_set_offset = i;
            kbp_stif_elements[i].object_id = kbp_stif_elements[0].object_id;
            /**
                Offset=0 - {disposition=0, Drop-Reason=0},
                Offset=1 - {disposition=0, Drop-Reason=1}.
                Offset=2 - {disposition=1, Drop-Reason=0}.
                Offset=3 - {disposition=1, Drop-Reason=1}.
            */
            if ((i == 0 && core_id == 0) || (i == 3 && core_id == 1))
            {
                COMPILER_64_COPY(kbp_stif_elements[i].expected_packets, packet_count[core_id]);
                COMPILER_64_COPY(kbp_stif_elements[i].expected_bytes, bytes_count[core_id]);
            }
            else
            {
                COMPILER_64_ZERO(kbp_stif_elements[i].expected_packets);
                COMPILER_64_ZERO(kbp_stif_elements[i].expected_bytes);
            }
        }
        SHR_IF_ERR_EXIT(dnx_kbp_stif_read(unit, FALSE, kbp_stif_config.device,
                                          kbp_stif_config.db[0], set_size[0], kbp_stif_elements));
        /** verify counters */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size[0], kbp_stif_elements));
    }

    LOG_CLI((BSL_META("--- EGRESS DATABASE VERIFY --- \n")));
    /** read counters from egress acl database (SOID1) - relevant just for core-0, because for core-1 packet are dropped at ingress */
    kbp_stif_elements[0].object_id = to_port[0];
    for (int i = 0; i < set_size[2]; i++)
    {
        kbp_stif_elements[i].counter_set_offset = i;
        kbp_stif_elements[i].object_id = kbp_stif_elements[0].object_id;
        /** Offset[i]={DropReason+Is_multicast}. */
        if (i == 0)
        {
            COMPILER_64_COPY(kbp_stif_elements[i].expected_packets, packet_count[0]);
            /** packet size is unpredicted in egress */
            COMPILER_64_SET(kbp_stif_elements[i].expected_bytes, 0xFFFFFFFF, 0xFFFFFFFF);
        }
        else
        {
            COMPILER_64_ZERO(kbp_stif_elements[i].expected_packets);
            COMPILER_64_ZERO(kbp_stif_elements[i].expected_bytes);
        }
    }
    SHR_IF_ERR_EXIT(dnx_kbp_stif_read(unit, FALSE, kbp_stif_config.device, kbp_stif_config.db[2],
                                      set_size[2], kbp_stif_elements));

    /** verify counters */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size[2], kbp_stif_elements));

exit:
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        if (packet_h[core_id] != NULL)
        {
            diag_sand_packet_free(unit, packet_h[core_id]);
        }
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_stif_ing_egr_two_ports_test(
    int unit,
    char *txfir_config,
    char *link_training_enable)
{
    rhhandle_t ctest_soc_set_h = NULL;

    ctest_soc_property_t ctest_soc_property[] = {
        {"port_init_cl72_ext_stat0", link_training_enable},
        {"port_init_cl72_ext_stat2", link_training_enable},
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "96"},
        {"stat_if_report_size_egress", "64"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"serdes_tx_taps_ext_stat", txfir_config},
        {"port_init_cl72_33", link_training_enable},
        {"port_init_cl72_34", link_training_enable},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ucode_port_20*", NULL},
        {NULL}
    };
    SHR_FUNC_INIT_VARS(unit);
    /** init soc properties */
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /** configure DNX and KBP stif */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_config_example(unit, 2));

   /** Lock device configuration */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_send_and_check(unit));

    /** Database destroy */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_tap_destroy
                    (unit, kbp_stif_config.db, kbp_stif_config.nof_db, kbp_stif_config.inst_th,
                     kbp_stif_config.nof_instructions));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/**
 * \brief - dnx and kbp stif test.
 *  configuration:
 *  One JR2.
 *  1 port of 200G core 0, billing mode.
 *  count only packets
 *  Ingress record format:
 *  Record type (ingress) - bits 94..95
 *  Packet Size - bits 80..93
 *  Hole - bits 40..79
 *  DP - bits 38..39
 *  Drop-Reason - bit 37 (take only MSB bit from 3 bits Drop reason)
 *  Disposition - bit 36
 *  SOID3 (VoQ-ID) - bits 20..35
 *  SOID0  - bits 0..19
 *  Total: 96 bits
 *
 *  Egress record format:
 *  Egress record format:
 *  Record type (egress) - bits 78..79
 *  Packet Size - bits 64..77
 *  Hole - bits 60..63
 *  disposition - bits 60 >> dummy. just to have some attribute in the record
 *  SOID2 - bits 40..59
 *  SOID1 (ACE PTR) - bits 20..39
 *  SOID0 - bits 0..19
 *  Total: 80b
 *
 *
 *
 *  Ingress database - queue SOID: counter_set=1.
 *      Count packets
 *
 *  Egress database - ace ptr SOID: counter_set=1.
 *      Count packets
 *
 * \param [in] unit         - Number of hardware unit used
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
#define DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB (2)
#define DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS (14)
static shr_error_e
dnx_kbp_stif_ing_egr_one_port_test(
    int unit)
{
    rhhandle_t ctest_soc_set_h = NULL;
    struct kbp_tap_record *rec_th;
    int db_id;
    bcm_field_context_t context_id;
    char name[DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB][DNX_KBP_STIF_STRING_SIZE] = { "ING_QUEUE_ID", "EGR_SOID0" };
    int ptr_len[DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB] = { 16, 20 };     /* {queues, other SOID} */
    int set_size = 1;
    struct kbp_tap_db_pointer *ptr[DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB];
    struct kbp_tap_db_attribute *attr[DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB];
    int ing_rec_size, egr_rec_size, null_rec_size;
    int packet_size = 14;
    int attr_value, counter_offset;
    int hole_len;
    kbp_status status;
    bcm_port_t from_port = 218;
    bcm_port_t to_port[DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS] =
        { 1, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32, 35, 36, 37 };
    bcm_color_t dp = BCM_FIELD_COLOR_GREEN;
    rhhandle_t packet_h = NULL;
    bcm_stat_stif_source_t source;
    int nof_elements_ingress = 6, nof_elements_egress = 4;
    dnx_kbp_stif_element_ts kbp_stif_elements[64];
    bcm_switch_kbp_info_t kbp_info;
    int stat_pp_profile;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_stif_record_format_element_t ingress_format_elements[6] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressQueueNumber, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressTmDropReason, 0x4},
    {bcmStatStifRecordElementIngressDropPrecedenceMeterResolved, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressDispositionIsDrop, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    bcm_stat_stif_record_format_element_t egress_format_elements[4] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj1, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj2, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    uint64 packet_count, bytes_count;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    dnx_data_port_static_add_ext_stat_speed_t ext_stat_speed;
    ctest_soc_property_t *ctest_soc_property;
    ctest_soc_property_t ctest_soc_property_q2a[] = {
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "64"},
        {"stat_if_report_size_egress", "80"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ucode_port_20*", NULL},
        {"ucode_port_34*", NULL},
        {"port_init_speed_ext_stat2*", NULL},
        {NULL}
    };
    ctest_soc_property_t ctest_soc_property_jr2[] = {
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "96"},
        {"stat_if_report_size_egress", "80"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"ucode_port_20*", NULL},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ucode_port_34*", NULL},
        {"port_init_speed_ext_stat2*", NULL},
        {NULL}
    };
    int sat_packet_size = 72;
    int sat_rate;
    SHR_FUNC_INIT_VARS(unit);

    /** sat rate calculation is to get to max record per second in the device. */
    /** since device generate I+E records per packet, max records is according to the max packet rate, */
    /** which is according to the core clock. In addition need to consider the snake size and the sat packet size */
    /** finally, we round it to Mbps resolution */
    sat_rate =
        0.99 * (dnx_data_device.general.core_clock_khz_get(unit) / DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS) *
        sat_packet_size * 8 / 1000000 * 1000000;

    if (soc_is(unit, Q2A_DEVICE))
    {
        ctest_soc_property = ctest_soc_property_q2a;
    }
    else
    {
        ctest_soc_property = ctest_soc_property_jr2;
    }
    /** init soc properties */
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    /** if the sat rate is more than 40G, we will allocate 100G port, on the safe side. */
    SHR_IF_ERR_EXIT(ctest_kbp_stif_port_config
                    (unit, 0, DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS, to_port,
                     (sat_rate > 40000000 ? 100000 : 50000)));

    /*
     * dnx configuration
     */
    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ext_stat_speed_read(0, 0, &ext_stat_speed));
    /** disable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 0));
    /** connect IngressEnqueue source to the logical port */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
    source.src_type = bcmStatStifSourceEgressDequeue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));

    /** define the record format */
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_format_elements));
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_EGRESS, nof_elements_egress, egress_format_elements));

    /** enable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 1));

    /*
     * kbp configuration
     */
    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    kbp_stif_config.device = (struct kbp_device *) kbp_info.device_p;
    kbp_stif_config.nof_db = DNX_KBP_STIF_ING_EGR_ONE_PORT_NOF_DB;
    kbp_stif_config.nof_instructions = 2;

    /** we are using one port in this test, For q2a, it can't carry ing+egr records. so we reduce the ing record size */
    ing_rec_size = soc_is(unit, Q2A_DEVICE) ? 64 : 96;
    egr_rec_size = 80;
    null_rec_size = soc_is(unit, Q2A_DEVICE) ? 64 : 96;;

    /** Set the ingress and null record lengths */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_EGRESS_REC_SIZE, egr_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    if (kbp_mngr_smt_enabled(unit))
    {
        /** Port-0 is mapped to Thread-0  */
        kbp_stif_config.device_th[0] = (struct kbp_device *) kbp_info.thread_p[0];
    }

    /** Create tap databases */
    for (db_id = 0; db_id < kbp_stif_config.nof_db; db_id++)
    {
        /** Entry type is PAIR since packet and byte count mode */
        status = kbp_tap_db_init(kbp_stif_config.device, db_id, dnx_data_ipq.queues.nof_queues_get(unit),
                                 KBP_TAP_ENTRY_TYPE_SINGLE, set_size, &kbp_stif_config.db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize pointer */
        status = kbp_tap_db_pointer_init(kbp_stif_config.db[db_id], &ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_pointer_add_field(ptr[db_id], name[db_id], ptr_len[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_pointer(kbp_stif_config.db[db_id], ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_set_pointer failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize attribute*/
        status = kbp_tap_db_attribute_init(kbp_stif_config.db[db_id], &attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * database set_size=1, all goes into offset 0
         */
        for (attr_value = 0, counter_offset = 0; attr_value <= set_size; attr_value++)
        {
            status = kbp_tap_db_map_attr_value_to_entry(kbp_stif_config.db[db_id], attr_value, counter_offset);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                             kbp_get_status_string(status));
            }
        }

    }

    /*
     * Ingress record format:
     *  Record type (ingress) - bits 94..95
     *  Packet Size - bits 80..93
     *  Hole - bits 40..79
     *  DP - bits 38..39
     *  Drop-Reason - bit 37 (take only MSB bit from 3 bits Drop reason)
     *  Disposition - bit 36
     *  SOID3 (VoQ-ID) - bits 20..35
     *  SOID0  - bits 0..19
     *  Total: 96 bits
     */
    if (kbp_mngr_smt_enabled(unit))
    {
        status = kbp_tap_instruction_init(kbp_stif_config.device_th[0], 1, KBP_TAP_INSTRUCTION_TYPE_INGRESS,
                                          &kbp_stif_config.inst_th[0]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device_th[0], KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    else
    {
        status = kbp_tap_instruction_init(kbp_stif_config.device, 1, KBP_TAP_INSTRUCTION_TYPE_INGRESS,
                                          &kbp_stif_config.inst_th[0]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    packet_size = 14;
    status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    /** instead of DP and drop-reason and Hole and packet size */
    hole_len = soc_is(unit, Q2A_DEVICE) ? (packet_size + 11) : (packet_size + 43);

    status = kbp_tap_record_add_field(rec_th, "HOLE_A", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, name[0], ptr_len[0], KBP_TAP_RECORD_FIELD_POINTER);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    hole_len = 20;      /* instead of 3 SOIDs */
    status = kbp_tap_record_add_field(rec_th, "HOLE_B", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }

    /*
     * Associate record with the instruction. Add database and finally install the instruction.
     */
    status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[0], rec_th);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_instruction_add_db
        (kbp_stif_config.inst_th[0], kbp_stif_config.db[0], 0, dnx_data_ipq.queues.nof_queues_get(unit));
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_instruction_install(kbp_stif_config.inst_th[0]);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
    }

    /*
     * Egress record format:
     * Record type (egress) - bits 78..79
     * Packet Size - bits 64..77
     * Hole - bits 60..63
     * disposition - bits 60 >> dummy. just to have some attribute in the record
     * SOID2 - bits 40..59
     * SOID1 (ACE PTR) - bits 20..39
     * SOID0 - bits 0..19
     * Total: 80b
     */
    if (kbp_mngr_smt_enabled(unit))
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device_th[0], 2, KBP_TAP_INSTRUCTION_TYPE_EGRESS, &kbp_stif_config.inst_th[1]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device_th[0], KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    else
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device, 2, KBP_TAP_INSTRUCTION_TYPE_EGRESS, &kbp_stif_config.inst_th[1]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    packet_size = 14;
    status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    /** hole is the packet size + 4 bits hole */
    status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_PACKET_SIZE", (packet_size + 4), KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    hole_len = 19; /** instead of 1 SOIDs-dispositon */
    status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_A", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, name[1], ptr_len[1], KBP_TAP_RECORD_FIELD_POINTER);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    hole_len = 20; /** instead of 1 SOID */
    status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_B", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }

    /*
     * Associate record with the instruction. Add database and finally install the instruction.
     */
    status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[1], rec_th);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_instruction_add_db(kbp_stif_config.inst_th[1], kbp_stif_config.db[1], 0,
                                        dnx_data_ipq.queues.nof_queues_get(unit));
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
    }

    status = kbp_tap_instruction_install(kbp_stif_config.inst_th[1]);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
    }

   /** Lock device configuration */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    sal_msleep(3000);

    /** generate statistics */
    /** ingress */
    /** map in port to DP and set SOID0=port */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_create(unit, &context_id));
    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_run(unit, context_id, from_port, dp));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port, to_port[0], 1));
    /*
     * snake in order to get to close to 200G traffic rate
     */
    for (int i = 0; i < (DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS - 1); i++)
    {
        bcm_port_loopback_set(0, to_port[i], BCM_PORT_LOOPBACK_MAC);
        bcm_port_force_forward_set(0, to_port[i], to_port[i + 1], 1);
    }
    /** egress */
    bcm_stat_pp_profile_info_t_init(&pp_profile);
    pp_profile.counter_command_id = 7; /** external statistics command id 1 */
    SHR_IF_ERR_EXIT(bcm_stat_pp_profile_create(unit, 0,
                                               bcmStatCounterInterfaceEgressTransmitPp, &stat_pp_profile, &pp_profile));
    /** generate egress ace ptr for all ports  */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_ace_generate_stat_id
                    (unit, DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS, to_port, to_port, stat_pp_profile));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port, &flags, &interface_info, &mapping_info));
    /** send packets */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    /*
     * sending from SAT 41G. packet size=72 bytes. snake of 14 ports ==> 41G/8/72=71.11M packets. 71.11*14=~995packets.
     * ==>995M ingress recordPerSeconds + 995M Egress recordPerSeconds.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, 0, packet_h, sat_packet_size, 1,
                                                   BCM_SAT_GTF_RATE_IN_BYTES, sat_rate, 10000,
                                                   SAT_GRANULARITY_SET_BY_DIAG));

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port, packet_h, 0));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
    LOG_CLI((BSL_META("SAT generated {0x%x,0x%x} Packets {0x%x,0x%x} Bytes\n"),
             COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count),
             COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count)));

    LOG_CLI((BSL_META("--- ING QUEUES DATABASE VERIFY --- \n")));
    COMPILER_64_COPY(kbp_stif_elements[0].expected_packets, packet_count);

    sal_msleep(2000);

    /** read counters from ingress voq database (SOID3)*/
    for (int j = 0; j < DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS; j++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get
                        (unit, 0, to_port[j], (int *) &kbp_stif_elements[0].object_id));

        SHR_IF_ERR_EXIT(bcm_stat_get
                        (unit, to_port[j], snmpEtherStatsTXNoErrors, &kbp_stif_elements[0].expected_packets));

        COMPILER_64_SET(kbp_stif_elements[0].expected_bytes, 0xFFFFFFFF, 0xFFFFFFFF);
        kbp_stif_elements[0].counter_set_offset = 0;
        SHR_IF_ERR_EXIT(dnx_kbp_stif_read
                        (unit, TRUE, kbp_stif_config.device, kbp_stif_config.db[0], set_size, kbp_stif_elements));
        /** verify counters */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));
    }

    LOG_CLI((BSL_META("--- EGRESS DATABASE VERIFY --- \n")));

    /** read counters from egress acl database (SOID1) */
    COMPILER_64_COPY(kbp_stif_elements[0].expected_packets, packet_count);
    COMPILER_64_SET(kbp_stif_elements[0].expected_bytes, 0xFFFFFFFF, 0xFFFFFFFF);
    kbp_stif_elements[0].counter_set_offset = 0;
    kbp_stif_elements[0].object_id = to_port[0];
    SHR_IF_ERR_EXIT(dnx_kbp_stif_read
                    (unit, TRUE, kbp_stif_config.device, kbp_stif_config.db[1], set_size, kbp_stif_elements));
    /** verify counters */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));

    /** Database destroy */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_tap_destroy
                    (unit, kbp_stif_config.db, kbp_stif_config.nof_db, kbp_stif_config.inst_th,
                     kbp_stif_config.nof_instructions));

exit:

    diag_sand_packet_free(unit, packet_h);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/**
 * \brief - dnx and kbp stif test.
 *  configuration:
 *  One JR2.
 *  1 port of 200G core 0, billing mode.
 *
 *  Ingress record format:
 *  Record type (ingress) - bits 94..95
 *  Packet Size - bits 80..93
 *  DP - bits 78..79
 *  Drop-Reason - bit 77 (take only MSB bit from 3 bits Drop reason)
 *  Disposition - bit 76
 *  SOID3 (VoQ-ID) - bits 60..75
 *  SOID2  - bits 40..59
 *  SOID1  - bits 20..39
 *  SOID0  - bits 0..19
 *  Total: 96 bits
 *
 *  Egress record format:
 *  Record type (egress) - bits 70..71
 *  Packet Size - bits 56..69
 *  Hole  -  bits 40..55
 *  SOID1  - bits 20..39
 *  SOID0  - bits 0..19
 *  Total: 72b
 *
 *
 *
 *  Ingress database - queue SOID: counter_set=1.
 *      Count packets and bytes.
 *  + 3 other dummy databases
 *  Egress database - ace ptr SOID: counter_set=1.
 *      Count packets and bytes..
 *  + 3 other dummy databases
 * \param [in] unit         - Number of hardware unit used
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
#define DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB (6)
#define DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS (14)
static shr_error_e
dnx_kbp_stif_ing_egr_max_dbs_one_port_test(
    int unit)
{
    rhhandle_t ctest_soc_set_h = NULL;
    struct kbp_tap_record *rec_th;
    bcm_field_context_t context_id;
    int db_id;
    char name[DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB][DNX_KBP_STIF_STRING_SIZE] =
        { "ING_SOID0", "ING_SOID1", "ING_SOID2", "ING_QUEUE_ID", "EGR_SOID0", "EGR_ACE_PTR" };
    int max_qnum_mask = DNX_DATA_MAX_IPQ_QUEUES_QUEUE_ID_BITS;
    int qnum_mask = dnx_data_ipq.queues.queue_id_bits_get(unit);
    int ptr_len[DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB] = { 20, 20, 20, qnum_mask, 20, 19 };      /* {queues, *
                                                                                                         * other *
                                                                                                         * SOID} */
    int set_size = 1;
    struct kbp_tap_db_pointer *ptr[DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB];
    struct kbp_tap_db_attribute *attr[DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB];
    int ing_rec_size, egr_rec_size, null_rec_size;
    int packet_size = 14;
    int attr_value, counter_offset;
    int hole_len;
    kbp_status status;
    bcm_port_t from_port = 218;
    bcm_port_t to_port[DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS] =
        { 1, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32, 35, 36, 37 };
    bcm_color_t dp = BCM_FIELD_COLOR_GREEN;
    rhhandle_t packet_h = NULL;
    bcm_stat_stif_source_t source;
    int nof_elements_ingress = 7, nof_elements_egress = 3;
    dnx_kbp_stif_element_ts kbp_stif_elements[64];
    bcm_switch_kbp_info_t kbp_info;
    int stat_pp_profile;
    bcm_stat_pp_profile_info_t pp_profile;
    bcm_stat_stif_record_format_element_t ingress_format_elements[7] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj1, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj2, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressQueueNumber, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementIngressTmDropReason, 0x4},
    {bcmStatStifRecordElementIngressDispositionIsDrop, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    bcm_stat_stif_record_format_element_t egress_format_elements[3] =
        { {bcmStatStifRecordElementObj0, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementObj1, BCM_STAT_FULL_MASK},
    {bcmStatStifRecordElementPacketSize, BCM_STAT_FULL_MASK}
    };
    uint64 packet_count, bytes_count;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    ctest_soc_property_t ctest_soc_property[] = {
        {"stat_if_enable", "1"},
        {"stat_if_report_mode", "BILLING"},
        {"stat_if_report_size_ingress", "96"},
        {"stat_if_report_size_egress", "72"},
        {"stat_if_idle_reports_present", "1"},
        {"appl_enable_l2", "0"},
        {"ucode_port_20*", NULL},
        {"ucode_port_5*", NULL},
        {"ucode_port_6*", NULL},
        {"ext_tcam_dev_type", "BCM52321"},
        {NULL}
    };
    int sat_packet_size = 72;
    int sat_rate; /** bps */
    SHR_FUNC_INIT_VARS(unit);

    sat_rate =
        0.99 * (dnx_data_device.general.core_clock_khz_get(unit) / DNX_KBP_STIF_ING_EGR_ONE_PORT_TEST_NOF_PORTS) *
        sat_packet_size * 8 / 1000000 * 1000000;

    /** init soc properties */
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    /** if the sat rate is more than 40G, we will allocate 100G port, on the safe side. */
    SHR_IF_ERR_EXIT(ctest_kbp_stif_port_config
                    (unit, 0, DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS, to_port,
                     (sat_rate > 40000000 ? 100000 : 50000)));

    /*
     * dnx configuration
     */
    /** disable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 0));
    /** connect IngressEnqueue source to the logical port */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));
    source.src_type = bcmStatStifSourceEgressDequeue;
    SHR_IF_ERR_EXIT(bcm_stat_stif_source_mapping_set(unit, 0, source, 33));

    /** define the record format */
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_format_elements));
    SHR_IF_ERR_EXIT(bcm_stat_stif_record_format_set
                    (unit, BCM_STAT_EGRESS, nof_elements_egress, egress_format_elements));

    /** enable traffic */
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, 0, 1));

    /*
     * kbp configuration
     */
    SHR_IF_ERR_EXIT(bcm_switch_kbp_info_get(unit, 0, &kbp_info));
    kbp_stif_config.device = (struct kbp_device *) kbp_info.device_p;
    kbp_stif_config.nof_db = DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB;
    kbp_stif_config.nof_instructions = 2;

    ing_rec_size = 96;
    egr_rec_size = 72;
    null_rec_size = 96;

    /** Set the ingress and null record lengths */
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_device_set_property(kbp_stif_config.device, KBP_DEVICE_PROP_TAP_EGRESS_REC_SIZE, egr_rec_size);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_device_set_property failed: %s\n", kbp_get_status_string(status));
    }

    if (kbp_mngr_smt_enabled(unit))
    {
        kbp_stif_config.device_th[0] = (struct kbp_device *) kbp_info.thread_p[0];
    }

    /** Create tap databases */
    for (db_id = 0; db_id < kbp_stif_config.nof_db; db_id++)
    {
        /** Entry type is PAIR since packet and byte count mode */
        status = kbp_tap_db_init(kbp_stif_config.device, db_id, dnx_data_ipq.queues.nof_queues_get(unit),
                                 KBP_TAP_ENTRY_TYPE_SINGLE, set_size, &kbp_stif_config.db[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_init failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize pointer */
        status = kbp_tap_db_pointer_init(kbp_stif_config.db[db_id], &ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_pointer_add_field(ptr[db_id], name[db_id], ptr_len[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_pointer_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_pointer(kbp_stif_config.db[db_id], ptr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_set_pointer failed: %s\n", kbp_get_status_string(status));
        }
        /** Initialize attribute*/
        status = kbp_tap_db_attribute_init(kbp_stif_config.db[db_id], &attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_attribute_add_field(attr[db_id], "DISPOSITION", 1);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_db_set_attribute(kbp_stif_config.db[db_id], attr[db_id]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_attribute_add_field failed: %s\n", kbp_get_status_string(status));
        }

        /*
         * database set_size=1, all goes into offset 0
         */
        for (attr_value = 0, counter_offset = 0; attr_value <= set_size; attr_value++)
        {
            status = kbp_tap_db_map_attr_value_to_entry(kbp_stif_config.db[db_id], attr_value, counter_offset);
            if (status != KBP_OK)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_db_map_attr_value_to_entry failed: %s\n",
                             kbp_get_status_string(status));
            }
        }
    }

    /*
     * Ingress record format:
     * Record type (ingress) - bits 94..95
     * Packet Size - bits 80..93
     * DP - bits 78..79
     * Drop-Reason - bit 77 (take only MSB bit from 3 bits Drop reason)
     * Disposition - bit 76
     * SOID3 (VoQ-ID) - bits 60..74/75/76/77
     * SOID2 - bits 40..59
     * SOID1 - bits 20..39
     * SOID0 - bits 0..19
     * Total: 96 bits
     */
    if (kbp_mngr_smt_enabled(unit))
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device_th[0], 1, KBP_TAP_INSTRUCTION_TYPE_INGRESS, &kbp_stif_config.inst_th[0]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device_th[0], KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    else
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device, 1, KBP_TAP_INSTRUCTION_TYPE_INGRESS, &kbp_stif_config.inst_th[0]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_INGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    packet_size = 14;
    status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    hole_len = 1 + packet_size + (max_qnum_mask - qnum_mask); /** instead of drop-reason, packet size and the Queue number align hole*/
    status = kbp_tap_record_add_field(rec_th, "HOLE_A", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    for (db_id = 3; db_id >= 0; db_id--)
    {
        status = kbp_tap_record_add_field(rec_th, name[db_id], ptr_len[db_id], KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
    }

    /*
     * Associate record with the instruction. Add database and finally install the instruction.
     */
    status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[0], rec_th);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
    }
    for (db_id = 0; db_id < 4; db_id++)
    {
        status = kbp_tap_instruction_add_db
            (kbp_stif_config.inst_th[0], kbp_stif_config.db[db_id], 0, dnx_data_ipq.queues.nof_queues_get(unit));
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
    }
    status = kbp_tap_instruction_install(kbp_stif_config.inst_th[0]);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
    }

    /*
     * Egress record format:
     *  Record type (egress) - bits 70..71
     *  Packet Size - bits 56..69
     *  Hole  -  bits 40..55
     *  disposition - 39
     *  SOID1  - bits 20..38
     *  SOID0  - bits 0..19
     *  Total: 72b
     */
    if (kbp_mngr_smt_enabled(unit))
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device_th[0], 2, KBP_TAP_INSTRUCTION_TYPE_EGRESS, &kbp_stif_config.inst_th[1]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device_th[0], KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }
    }
    else
    {
        status = kbp_tap_instruction_init
            (kbp_stif_config.device, 2, KBP_TAP_INSTRUCTION_TYPE_EGRESS, &kbp_stif_config.inst_th[1]);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_init failed: %s\n", kbp_get_status_string(status));
        }
        status = kbp_tap_record_init(kbp_stif_config.device, KBP_TAP_RECORD_TYPE_EGRESS, &rec_th);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_init failed: %s\n", kbp_get_status_string(status));
        }

    }
    status = kbp_tap_record_add_field(rec_th, "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    packet_size = 14;
    /** hole is the packet size +16 bits hole */
    status = kbp_tap_record_add_field(rec_th, "EGR_HOLE_PACKET_SIZE", (packet_size + 16), KBP_TAP_RECORD_FIELD_HOLE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    status = kbp_tap_record_add_field(rec_th, "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
    }
    for (db_id = (DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB - 1); db_id >=
         (DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB - 2); db_id--)
    {
        status = kbp_tap_record_add_field(rec_th, name[db_id], ptr_len[db_id], KBP_TAP_RECORD_FIELD_POINTER);
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_record_add_field failed: %s\n", kbp_get_status_string(status));
        }
    }

    /*
     * Associate record with the instruction. Add database and finally install the instruction.
     */
    status = kbp_tap_instruction_set_record(kbp_stif_config.inst_th[1], rec_th);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_set_record failed: %s\n", kbp_get_status_string(status));
    }
    for (db_id = 4; db_id < DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_NOF_DB; db_id++)
    {
        status = kbp_tap_instruction_add_db
            (kbp_stif_config.inst_th[1], kbp_stif_config.db[db_id], 0, dnx_data_ipq.queues.nof_queues_get(unit));
        if (status != KBP_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_add_db failed: %s\n", kbp_get_status_string(status));
        }
    }
    status = kbp_tap_instruction_install(kbp_stif_config.inst_th[1]);
    if (status != KBP_OK)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "kbp_tap_instruction_install failed: %s\n", kbp_get_status_string(status));
    }

   /** Lock device configuration */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    sal_msleep(3000);

    /** generate statistics */
    /** ingress */
    /** map in port to DP and set SOID0=port */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_create(unit, &context_id));
    SHR_IF_ERR_EXIT(dnx_kbp_stif_acl_run(unit, context_id, from_port, dp));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port, to_port[0], 1));
    /*
     * snake in order to get to close to 200G traffic rate
     */
    for (int i = 0; i < (DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS - 1); i++)
    {
        bcm_port_loopback_set(0, to_port[i], BCM_PORT_LOOPBACK_MAC);
        bcm_port_force_forward_set(0, to_port[i], to_port[i + 1], 1);
    }
    /** egress */
    bcm_stat_pp_profile_info_t_init(&pp_profile);
    pp_profile.counter_command_id = 7; /** external statistics command id 1 */
    SHR_IF_ERR_EXIT(bcm_stat_pp_profile_create(unit, 0,
                                               bcmStatCounterInterfaceEgressTransmitPp, &stat_pp_profile, &pp_profile));
    SHR_IF_ERR_EXIT(dnx_kbp_stif_ace_generate_stat_id
                    (unit, DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS, to_port, to_port, stat_pp_profile));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port, &flags, &interface_info, &mapping_info));
    /** send packets */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    /*
     * sending from SAT 41G. packet size=72 bytes. snake of 14 ports ==> 41G/8/72=71.11M packets. 71.11*14=~995packets.
     * ==>995M ingress recordPerSeconds + 995M Egress recordPerSeconds.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, 0, packet_h, sat_packet_size, 1,
                                                   BCM_SAT_GTF_RATE_IN_BYTES, sat_rate, 10000,
                                                   SAT_GRANULARITY_SET_BY_DIAG));

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port, packet_h, 0));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
    LOG_CLI((BSL_META("SAT generated {0x%x,0x%x} Packets {0x%x,0x%x} Bytes\n"),
             COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count),
             COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count)));

    sal_msleep(4000);
    LOG_CLI((BSL_META("--- ING QUEUES DATABASE VERIFY --- \n")));
    COMPILER_64_COPY(kbp_stif_elements[0].expected_packets, packet_count);

    /** read counters from ingress voq database (SOID3)*/
    for (int j = 0; j < DNX_KBP_STIF_ING_EGR_MAX_DBS_ONE_PORT_TEST_NOF_PORTS; j++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get
                        (unit, 0, to_port[j], (int *) &kbp_stif_elements[0].object_id));

        SHR_IF_ERR_EXIT(bcm_stat_get
                        (unit, to_port[j], snmpEtherStatsTXNoErrors, &kbp_stif_elements[0].expected_packets));

        COMPILER_64_SET(kbp_stif_elements[0].expected_bytes, 0xFFFFFFFF, 0xFFFFFFFF);
        kbp_stif_elements[0].counter_set_offset = 0;
        SHR_IF_ERR_EXIT(dnx_kbp_stif_read
                        (unit, TRUE, kbp_stif_config.device, kbp_stif_config.db[3], set_size, kbp_stif_elements));
        /** verify counters */
        SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));
    }

    LOG_CLI((BSL_META("--- EGRESS DATABASE VERIFY --- \n")));

    /** read counters from egress acl database (SOID1) */
    COMPILER_64_COPY(kbp_stif_elements[0].expected_packets, packet_count);
    COMPILER_64_SET(kbp_stif_elements[0].expected_bytes, 0xFFFFFFFF, 0xFFFFFFFF);
    kbp_stif_elements[0].counter_set_offset = 0;
    kbp_stif_elements[0].object_id = to_port[0];
    SHR_IF_ERR_EXIT(dnx_kbp_stif_read
                    (unit, TRUE, kbp_stif_config.device, kbp_stif_config.db[5], set_size, kbp_stif_elements));
    /** verify counters */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_counters_compare(unit, set_size, kbp_stif_elements));

    /** Database destroy */
    SHR_IF_ERR_EXIT(dnx_kbp_stif_tap_destroy
                    (unit, kbp_stif_config.db, kbp_stif_config.nof_db, kbp_stif_config.inst_th,
                     kbp_stif_config.nof_instructions));

exit:

    diag_sand_packet_free(unit, packet_h);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_id;
    char *link_training_enable = NULL;
    char txfir_str[RHNAME_MAX_SIZE];
    dnx_data_port_static_add_ext_stat_speed_t ext_stat_speed;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("id", test_id);
    SH_SAND_GET_STR("link_training_enable", link_training_enable);

    /** callback registeration for warmboot test */
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_test_callback_register(unit, 0, dnx_kbp_stif_wb_test_callback, NULL);
#endif
    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ext_stat_speed_read(0, 0, &ext_stat_speed));

    /** configuration example, hopefully fit to all devices running this test */
    sal_strncpy(txfir_str, (((ext_stat_speed.speed == 100000) && (ext_stat_speed.nof_lanes == 4)) ?
                            "nrz:-10:87:-2:0:0:0" : "pam4:-24:140:0:2:0:0"), RHNAME_MAX_SIZE);
    switch (test_id)
    {
        case 0:
            SHR_IF_ERR_EXIT(dnx_kbp_stif_voq_db_test(unit, txfir_str, link_training_enable));
            break;
        case 1:
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_two_ports_test(unit, txfir_str, link_training_enable));
            break;
        case 2:
            if (!sal_memcmp(link_training_enable, "0", 2))
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "only link_training_enable=FALSE. Not supported for test id=%d \n", test_id);
            }
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_one_port_test(unit));
            break;
        case 3:

            if (!sal_memcmp(link_training_enable, "0", 2))
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "only link_training_enable=FALSE. Not supported for test id=%d \n", test_id);
            }
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_max_dbs_one_port_test(unit));
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "test ID=%d is not supported: \n", test_id);
    }
    LOG_CLI((BSL_META("TEST PASS!!! \n")));

exit:
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_test_callback_unregister(unit);
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_config(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_id;
    uint32 nof_stif_ports;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("id", test_id);
    SH_SAND_GET_UINT32("number", nof_stif_ports);

    /** callback registeration for warmboot test */
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_test_callback_register(unit, 0, dnx_kbp_stif_wb_test_callback, NULL);
#endif
    switch (test_id)
    {
        case 1:
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_config_example(unit, nof_stif_ports));
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "test ID=%d is not supported: \n", test_id);
    }
    LOG_CLI((BSL_META("TEST PASS!!! \n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_verify(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int test_id;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("id", test_id);
    switch (test_id)
    {
        case 1:
            SHR_IF_ERR_EXIT(dnx_kbp_stif_ing_egr_send_and_check(unit));
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "test ID=%d is not supported: \n", test_id);
    }
    LOG_CLI((BSL_META("TEST PASS!!! \n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_stif_destroy(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_stif_tap_destroy
                    (unit, kbp_stif_config.db, kbp_stif_config.nof_db, kbp_stif_config.inst_th,
                     kbp_stif_config.nof_instructions));

    LOG_CLI((BSL_META("TEST PASS!!! \n")));

exit:
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_test_callback_unregister(unit);
#endif
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_kbp_stif_test_list_options[] = {
    {"id", SAL_FIELD_TYPE_UINT32, "test id", "0"},
    {"link_training_enable", SAL_FIELD_TYPE_STR, "Enable link training", "1"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_stif_test_man = {
    .brief = "test kbp and dnx devices for statistics gathering using statistic interface",
    .full =
        "Test-0 configuration: ingress voq database per core. record_size=80. counter_set_size=8 (dp*disposition). packets and bytes \n"
        "Test-1 configuration: ingress(96 bits) and egress(64 bits) records , total 3 databases, 2 ports \n",
    "Test-2 configuration: ingress(96 bits) and egress(80 bits), 1 stif port 200Gps, one ingress database, one egress database. single counter mode (packets counters only) \n",
    "Test-3 configuration: ingress record 96 bits and egress record 72bits, 1 stif port 200Gps, four ingress databases, four egress databases. single counter mode (packets counters only)\n",
    .synopsis = NULL,
    .examples = "ctest kbp stif test id=0"
};

static sh_sand_option_t dnx_kbp_stif_config_list_options[] = {
    {"id", SAL_FIELD_TYPE_UINT32, "test id", "1"},
    {"number", SAL_FIELD_TYPE_UINT32, "number of stif ports", "2"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_stif_config_man = {
    .brief =
        "call only the STIF DNX API configurations and the KBP STAT API configurations. Do not run the entrire test. assuming all Soc properties was configured already",
    .full =
        "Test-1 configuration: ingress(96 bits) and egress(64 bits) records , total 3 databases, by default 2 ports \n",
    .synopsis = NULL,
    .examples = "ctest kbp stif config id=1"
};

static sh_sand_option_t dnx_kbp_stif_verify_list_options[] = {
    {"id", SAL_FIELD_TYPE_UINT32, "test id", "1"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_stif_verify_man = {
    .brief =
        "call only the send packets and check steps of a test. Do not run the entrire test. assuming the configuration already was made",
    .full = NULL,
    .synopsis = NULL,
    .examples = "ctest kbp stif verify id=1"
};

static sh_sand_option_t dnx_kbp_stif_destroy_list_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_kbp_stif_destroy_man = {
    .brief = "destroy kbp tap databases and instructions that are created",
    .full = NULL,
    .synopsis = NULL,
    .examples = "ctest kbp stif destroy"
};

/**
 * \brief DNX STIF-KBP ctests * List of the supported ctests, pointer to command function and
 * command usage function. This is the entry point for STIF-KBP
 * ctest commands  */
sh_sand_cmd_t sh_dnx_kbp_stif_test_cmds[] = {
    {"test", dnx_kbp_stif_test, NULL, dnx_kbp_stif_test_list_options, &dnx_kbp_stif_test_man, NULL, NULL,
     SH_CMD_SKIP_EXEC,.label = "kbp"},
    {"config", dnx_kbp_stif_config, NULL, dnx_kbp_stif_config_list_options, &dnx_kbp_stif_config_man, NULL, NULL,
     SH_CMD_SKIP_EXEC,.label = "kbp"},
    {"verify", dnx_kbp_stif_verify, NULL, dnx_kbp_stif_verify_list_options, &dnx_kbp_stif_verify_man, NULL, NULL,
     SH_CMD_SKIP_EXEC,.label = "kbp"},
    {"destroy", dnx_kbp_stif_destroy, NULL, dnx_kbp_stif_destroy_list_options, &dnx_kbp_stif_destroy_man, NULL, NULL,
     SH_CMD_SKIP_EXEC,.label = "kbp"},
    {NULL}
};
