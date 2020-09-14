/*
 * ctest_dnx_rx_trap_ingress_semantic.c
 *
 *  Created on: Dec 21, 2017
 *      Author: dp889757
 */
/*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include "ctest_dnx_rx_trap_ingress_semantic.h"
#include "ctest_dnx_rx_trap.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

/** 17 User defined traps were allocated to support OAMP punt packets */
#define NOF_OAM_PUNT_TRAPS 17
/*
 * }
 */

/** Ingress trap test details */
sh_sand_man_t sh_dnx_rx_trap_ingress_sem_man = {
    "Ingress traps configuration testing",
    "Create Ingress trap, then get trap id and compare values."
};

/**
 * \brief
 *   Options list for 'ingress_sem' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_ingress_sem_options[] = {
    /**         Name                               Type                 Description                 Default */
    {CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (pos, neg)", "pos"}
    ,
    {CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_MODE, SAL_FIELD_TYPE_STR, "Mode of test "
     "(predefined, oam, get)", "predefined"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for Ingress shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_ingress_sem_tests[] = {
    {"pos_pre", "type=pos mode=predefined", CTEST_POSTCOMMIT}
    ,
    {"pos_oam", "type=pos mode=oam", CTEST_POSTCOMMIT}
    ,
    {"pos_get", "type=pos mode=get", CTEST_POSTCOMMIT}
    ,
    {"neg_pre", "type=neg mode=predefined", CTEST_POSTCOMMIT}
    ,
    {"neg_ud", "type=neg mode=userdefined", CTEST_POSTCOMMIT}
    ,
    {"neg_act", "type=neg mode=actions", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

void
ctest_dnx_rx_trap_ingress_fill(
    int unit,
    bcm_gport_t gport_encap_id,
    bcm_rx_trap_config_t * trap_config_p)
{
    uint32 counter_command_id0, counter_command_id1;

    trap_config_p->flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_METER_CMD |
        BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_COLOR | BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX |
        BCM_RX_TRAP_TRAP | BCM_RX_TRAP_BYPASS_FILTERS | BCM_RX_TRAP_LEARN_DISABLE | BCM_RX_TRAP_UPDATE_ADD_VLAN |
        BCM_RX_TRAP_UPDATE_FORWARDING_HEADER | BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH |
        BCM_RX_TRAP_UPDATE_ECN_VALUE | BCM_RX_TRAP_UPDATE_VISIBILITY | BCM_RX_TRAP_UPDATE_VSQ;
    trap_config_p->flags2 = BCM_RX_TRAP_FLAGS2_UPDATE_ACL_PROFILE;
    if (sal_rand() % 1)
    {
        /** Per core configuration */
        trap_config_p->core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(trap_config_p->core_config_arr[0].dest_port, 0, sal_rand() % 10);
        /** encap_id hold out_lif encoded as gport, encap_id2 holds raw out_lif value */
        trap_config_p->core_config_arr[0].encap_id = gport_encap_id;
        trap_config_p->core_config_arr[0].encap_id2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_encap_id);

        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(trap_config_p->core_config_arr[1].dest_port, 1, sal_rand() % 10);
        /** encap_id hold out_lif encoded as gport, encap_id2 holds raw out_lif value */
        trap_config_p->core_config_arr[1].encap_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_encap_id);
        trap_config_p->core_config_arr[1].encap_id2 = gport_encap_id;
    }
    else
    {
        /** Shared by cores configuration */
        trap_config_p->core_config_arr = NULL;
        if (sal_rand() % 1)
        {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config_p->dest_port, sal_rand() % 10);
        }
        else
        {
            trap_config_p->flags |= BCM_RX_TRAP_DEST_MULTICAST;
            BCM_GPORT_MCAST_SET(trap_config_p->dest_group, sal_rand() % 10);
        }

        /** encap_id hold out_lif encoded as gport, encap_id2 holds raw out_lif value */
        trap_config_p->encap_id = gport_encap_id;
        trap_config_p->encap_id2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_encap_id);
    }

    trap_config_p->prio = (sal_rand() % 2);
    trap_config_p->color = (sal_rand() % 2);
    trap_config_p->trap_strength = 1 + (sal_rand() % 14);
    trap_config_p->forwarding_header = (sal_rand() % 5);
    trap_config_p->meter_cmd = (sal_rand() % 5);
    trap_config_p->mapped_trap_strength = 1 + (sal_rand() % 14);
    trap_config_p->ecn_value = (sal_rand() % 7);
    trap_config_p->vsq = (sal_rand() % 255);
    trap_config_p->visibility_value = (sal_rand() % 1);
    trap_config_p->latency_flow_id_clear = (sal_rand() % 1);
    trap_config_p->egress_forwarding_index = (sal_rand() % 8);
    trap_config_p->stat_obj_config_len = 2;
    counter_command_id0 = (sal_rand() % 9);
    trap_config_p->stat_obj_config_arr[0].counter_command_id = counter_command_id0;
    trap_config_p->stat_obj_config_arr[0].stat_id = (sal_rand() % 0xFFFF);
    trap_config_p->stat_obj_config_arr[0].stat_object_type = (sal_rand() % 3);
    trap_config_p->stat_obj_config_arr[0].is_offset_by_qual_enable = (sal_rand() % 1);
    trap_config_p->stat_obj_config_arr[0].is_meter = (sal_rand() % 1);
    counter_command_id1 = (sal_rand() % 9);
    trap_config_p->stat_obj_config_arr[1].counter_command_id = counter_command_id1;
    trap_config_p->stat_obj_config_arr[1].stat_id = (sal_rand() % 0xFFFF);
    trap_config_p->stat_obj_config_arr[1].stat_object_type = (sal_rand() % 3);
    trap_config_p->stat_obj_config_arr[1].is_offset_by_qual_enable = (sal_rand() % 1);
    trap_config_p->stat_obj_config_arr[1].is_meter = (sal_rand() % 1);
    /** Create random bitmap and turn off bits of stat objects configured */
    trap_config_p->stat_clear_bitmap = (sal_rand() % 0x3FF) & ~(0x1 << counter_command_id0)
        & ~(0x1 << counter_command_id1);
    trap_config_p->stat_metadata_mask = (sal_rand() % 0xFFFF);
}

static shr_error_e
ctest_dnx_rx_trap_ingress_create_and_set(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t lif_gport,
    int *trap_id_p,
    bcm_rx_trap_config_t * trap_config_p)
{
    int rv = BCM_E_NONE;
    dnx_rx_trap_map_type_t predefined_trap_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    /*
     * Create the Ingress trap
     */
    rv = bcm_rx_trap_type_create(unit, 0, trap_type, trap_id_p);
    if (rv == _SHR_E_UNAVAIL)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "");
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    LOG_INFO_EX(BSL_LOG_MODULE, "Created Trap: %d \n %s%s%s", *trap_id_p, EMPTY, EMPTY, EMPTY);

    ctest_dnx_rx_trap_ingress_fill(unit, lif_gport, trap_config_p);
    SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, trap_type, &predefined_trap_info));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Created Trap: %s \n"), predefined_trap_info.trap_name));

    if (predefined_trap_info.is_strengthless)
    {
        trap_config_p->trap_strength = 0;
        trap_config_p->snoop_strength = 0;
    }

    /*
     * Set the Ingress trap
     */
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, *trap_id_p, trap_config_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Compares two configuration of trap action.
* \param [in] unit - unit ID
* \param [in] trap_id - trap id
* \param [in] trap_config_set_p - set trap configuration
* \param [in] trap_config_get_p - get trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e
ctest_dnx_rx_trap_ingress_compare(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_set_p,
    bcm_rx_trap_config_t * trap_config_get_p)
{
    int raw_encap_id2 = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Encap id returned by get function is encoded as gport, get raw value for compare */
    if (trap_config_get_p->encap_id2 != 0)
    {
        raw_encap_id2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(trap_config_get_p->encap_id2);
    }

    /** Compare configuration */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_set_p->flags, &trap_config_get_p->flags,
                                                     sizeof(trap_config_get_p->flags), "flags"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_set_p->flags2, &trap_config_get_p->flags2,
                                                     sizeof(trap_config_get_p->flags2), "flags2"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_set_p->dest_port, &trap_config_get_p->dest_port,
                                                     sizeof(trap_config_get_p->dest_port), "dest_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->dest_group, &trap_config_get_p->dest_group,
                     sizeof(trap_config_get_p->dest_group), "dest_group"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->prio, &trap_config_get_p->prio, sizeof(trap_config_get_p->prio),
                     "Priority (TC)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->color, &trap_config_get_p->color, sizeof(trap_config_get_p->color),
                     "Color (DP)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->trap_strength, &trap_config_get_p->trap_strength,
                     sizeof(trap_config_get_p->trap_strength), "trap_strength"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->mapped_trap_strength, &trap_config_get_p->mapped_trap_strength,
                     sizeof(trap_config_get_p->mapped_trap_strength), "mapped_trap_strength"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->forwarding_header, &trap_config_get_p->forwarding_header,
                     sizeof(trap_config_get_p->forwarding_header), "forwarding_header"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->encap_id, &trap_config_get_p->encap_id,
                     sizeof(trap_config_get_p->encap_id), "encap_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->encap_id2, &raw_encap_id2, sizeof(trap_config_get_p->encap_id2),
                     "encap_id2"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->meter_cmd, &trap_config_get_p->meter_cmd,
                     sizeof(trap_config_get_p->meter_cmd), "meter_cmd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->ecn_value, &trap_config_get_p->ecn_value,
                     sizeof(trap_config_get_p->ecn_value), "ecn_value"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->vsq, &trap_config_get_p->vsq, sizeof(trap_config_get_p->vsq), "vsq"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->visibility_value, &trap_config_get_p->visibility_value,
                     sizeof(trap_config_get_p->visibility_value), "visibility_value"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->latency_flow_id_clear, &trap_config_get_p->latency_flow_id_clear,
                     sizeof(trap_config_get_p->latency_flow_id_clear), "latency_flow_id_clear"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->egress_forwarding_index, &trap_config_get_p->egress_forwarding_index,
                     sizeof(trap_config_get_p->egress_forwarding_index), "egress_forwarding_index"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_len, &trap_config_get_p->stat_obj_config_len,
                     sizeof(trap_config_get_p->stat_obj_config_len), "stat_obj_config_len"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_clear_bitmap, &trap_config_get_p->stat_clear_bitmap,
                     sizeof(trap_config_get_p->stat_clear_bitmap), "stat_clear_bitmap"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_metadata_mask, &trap_config_get_p->stat_metadata_mask,
                     sizeof(trap_config_get_p->stat_metadata_mask), "stat_metadata_mask"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[0].counter_command_id,
                     &trap_config_get_p->stat_obj_config_arr[0].counter_command_id,
                     sizeof(trap_config_get_p->stat_obj_config_arr[0].counter_command_id), "counter_command_id0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[0].stat_id,
                     &trap_config_get_p->stat_obj_config_arr[0].stat_id,
                     sizeof(trap_config_get_p->stat_obj_config_arr[0].stat_id), "stat_id0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[0].stat_object_type,
                     &trap_config_get_p->stat_obj_config_arr[0].stat_object_type,
                     sizeof(trap_config_get_p->stat_obj_config_arr[0].stat_object_type), "stat_object_type0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[0].is_meter,
                     &trap_config_get_p->stat_obj_config_arr[0].is_meter,
                     sizeof(trap_config_get_p->stat_obj_config_arr[0].is_meter), "is_meter0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[0].is_offset_by_qual_enable,
                     &trap_config_get_p->stat_obj_config_arr[0].is_offset_by_qual_enable,
                     sizeof(trap_config_get_p->stat_obj_config_arr[0].is_offset_by_qual_enable),
                     "is_offset_by_qual_enable0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[1].counter_command_id,
                     &trap_config_get_p->stat_obj_config_arr[1].counter_command_id,
                     sizeof(trap_config_get_p->stat_obj_config_arr[1].counter_command_id), "counter_command_id1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[1].stat_id,
                     &trap_config_get_p->stat_obj_config_arr[1].stat_id,
                     sizeof(trap_config_get_p->stat_obj_config_arr[1].stat_id), "stat_id1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[1].stat_object_type,
                     &trap_config_get_p->stat_obj_config_arr[1].stat_object_type,
                     sizeof(trap_config_get_p->stat_obj_config_arr[1].stat_object_type), "stat_object_type1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[1].is_meter,
                     &trap_config_get_p->stat_obj_config_arr[1].is_meter,
                     sizeof(trap_config_get_p->stat_obj_config_arr[1].is_meter), "is_meter1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_set_p->stat_obj_config_arr[1].is_offset_by_qual_enable,
                     &trap_config_get_p->stat_obj_config_arr[1].is_offset_by_qual_enable,
                     sizeof(trap_config_get_p->stat_obj_config_arr[1].is_offset_by_qual_enable),
                     "is_offset_by_qual_enable1"));

    if (trap_config_set_p->core_config_arr != NULL)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[0].dest_port,
                         &trap_config_get_p->core_config_arr[0].dest_port,
                         sizeof(trap_config_get_p->core_config_arr[0].dest_port), "core_config_arr[0].dest_port"));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[0].encap_id,
                         &trap_config_get_p->core_config_arr[0].encap_id,
                         sizeof(trap_config_get_p->core_config_arr[0].encap_id), "core_config_arr[0].encap_id"));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[0].encap_id2,
                         &trap_config_get_p->core_config_arr[0].encap_id2,
                         sizeof(trap_config_get_p->core_config_arr[0].encap_id2), "core_config_arr[0].encap_id2"));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[1].dest_port,
                         &trap_config_get_p->core_config_arr[1].dest_port,
                         sizeof(trap_config_get_p->core_config_arr[1].dest_port), "core_config_arr[1].dest_port"));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[1].encap_id,
                         &trap_config_get_p->core_config_arr[1].encap_id,
                         sizeof(trap_config_get_p->core_config_arr[1].encap_id), "core_config_arr[1].encap_id"));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &trap_config_set_p->core_config_arr[1].encap_id2,
                         &trap_config_get_p->core_config_arr[1].encap_id2,
                         sizeof(trap_config_get_p->core_config_arr[1].encap_id2), "core_config_arr[1].encap_id2"));
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get and compare was done on trap_id=%d \n %s%s%s",
                trap_id, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of Predefined Ingress traps.
 *   Sets a Ingress trap
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_gport - Gport encoding LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_predefined(
    int unit,
    bcm_gport_t lif_gport)
{
    uint8 is_alloc;
    int trap_iter = 0, nof_traps, config_iter;
    bcm_rx_trap_t *trap_type_list;
    bcm_rx_trap_config_t trap_config, trap_config_get;
    bcm_rx_trap_core_config_t *core_config_arr_p = NULL;

    int nof_ud_traps = dnx_data_trap.ingress.nof_user_defined_traps_get(unit);
    int nof_free_ud_traps = 0;
    int trap_id, get_trap_id;
    int *ud_trap_id_p = NULL;
    int rv = BCM_E_NONE;
    dnx_rx_trap_map_type_t predefined_trap_info;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_ALLOC(core_config_arr_p, sizeof(*core_config_arr_p) * dnx_data_device.general.nof_cores_get(unit),
              "core_config_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ud_trap_id_p, sizeof(*ud_trap_id_p) * nof_ud_traps, "ud_trap_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION TEST START.\n")));
    SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                    (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED, &trap_type_list, &nof_traps));

    for (config_iter = 0; config_iter < 2; config_iter++)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION TEST LOOP: %d.\n"), config_iter));
        for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get
                            (unit, trap_type_list[trap_iter], &predefined_trap_info));
            dnx_rx_trap_is_trap_id_allocated(unit, predefined_trap_info.trap_id, &is_alloc);
            if (is_alloc == TRUE ||
                DNX_RX_TRAP_IS_DA_NOT_FOUND(predefined_trap_info.trap_id) ||
                predefined_trap_info.trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_UNKNOWN_DESTINATION)
            {
                continue;
            }

            LOG_INFO_EX(BSL_LOG_MODULE, "Trap: %s currently testing! \n %s%s%s",
                        predefined_trap_info.trap_name, EMPTY, EMPTY, EMPTY);

            bcm_rx_trap_config_t_init(&trap_config);
            trap_config.core_config_arr = core_config_arr_p;

            rv = ctest_dnx_rx_trap_ingress_create_and_set(unit, trap_type_list[trap_iter], lif_gport,
                                                          &trap_id, &trap_config);
            if (rv == _SHR_E_UNAVAIL)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            /** Get the Ingress trap */
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &get_trap_id));
            LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);

            if (trap_id != get_trap_id)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "SET Trap-ID (%d) is not the same as GET Trap-ID (%d)\r\n", trap_id,
                             get_trap_id);
            }

            if (trap_config.core_config_arr_len > 0)
            {
                trap_config_get.core_config_arr = core_config_arr_p;
            }

            /** Get trap action */
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_compare(unit, trap_id, &trap_config, &trap_config_get));

            /** Destroy the Ingress trap */
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
            LOG_INFO_EX(BSL_LOG_MODULE, "Deleted Trap: %d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);

        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION Creat ALL!\n")));

    for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, trap_type_list[trap_iter], &predefined_trap_info));
        dnx_rx_trap_is_trap_id_allocated(unit, predefined_trap_info.trap_id, &is_alloc);
        if (is_alloc == TRUE ||
            DNX_RX_TRAP_IS_DA_NOT_FOUND(predefined_trap_info.trap_id) ||
            predefined_trap_info.trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_UNKNOWN_DESTINATION)
        {
            continue;
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "Trap: %s currently testing! \n %s%s%s",
                    predefined_trap_info.trap_name, EMPTY, EMPTY, EMPTY);

        rv = bcm_rx_trap_type_create(unit, 0, trap_type_list[trap_iter], &trap_id);
        if (rv == _SHR_E_UNAVAIL)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION Destroy ALL!\n")));
    for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, trap_type_list[trap_iter], &predefined_trap_info));
        dnx_rx_trap_is_trap_id_allocated(unit, predefined_trap_info.trap_id, &is_alloc);
        if (is_alloc == TRUE ||
            DNX_RX_TRAP_IS_DA_NOT_FOUND(predefined_trap_info.trap_id) ||
            predefined_trap_info.trap_id == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_UNKNOWN_DESTINATION)
        {
            continue;
        }

        /** Get the Ingress trap */
        rv = bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &get_trap_id);
        if (rv == _SHR_E_UNAVAIL)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);

        /** Destroy the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, get_trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Deleted Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION TEST END.\n")));
    for (trap_iter = 0; trap_iter < nof_ud_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_iter, &is_alloc));
        if (is_alloc == FALSE)
        {
            nof_free_ud_traps++;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS USERDEFINED TRAP CONFIGURATION Creat ALL!\n")));

    for (trap_iter = 0; trap_iter < nof_free_ud_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ud_trap_id_p[trap_iter]));
    }

    for (trap_iter = 0; trap_iter < nof_free_ud_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, ud_trap_id_p[trap_iter]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS USERDEFINED TRAP CONFIGURATION Destroy ALL!\n")));

exit:
    SHR_FREE(core_config_arr_p);
    SHR_FREE(ud_trap_id_p);
    if (trap_type_list != NULL)
    {
        sal_free(trap_type_list);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of OAM Ingress traps.
 *   Sets a Ingress trap
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_gport - Gport encoding LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_oam(
    int unit,
    bcm_gport_t lif_gport)
{
    int trap_id, get_trap_id;
    uint8 is_alloc;
    int trap_iter = 0, nof_traps;
    bcm_rx_trap_t *trap_type_list;
    bcm_rx_trap_config_t trap_config, trap_config_get;
    bcm_rx_trap_core_config_t *core_config_arr_p = NULL;
    dnx_rx_trap_map_type_t predefined_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(core_config_arr_p, sizeof(*core_config_arr_p) * dnx_data_device.general.nof_cores_get(unit),
              "core_config_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                    (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_OAM, &trap_type_list, &nof_traps));

    for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, trap_type_list[trap_iter], &predefined_trap_info));
        dnx_rx_trap_is_trap_id_allocated(unit, predefined_trap_info.trap_id, &is_alloc);
        if (is_alloc == TRUE)
        {
            continue;
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "Trap: %s currently testing! \n %s%s%s",
                    predefined_trap_info.trap_name, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_create_and_set(unit, trap_type_list[trap_iter], lif_gport,
                                                                 &trap_id, &trap_config));

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.core_config_arr = core_config_arr_p;

        /** Get the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &get_trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);

        if (trap_id != get_trap_id)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "SET Trap-ID (%d) is not the same as GET Trap-ID (%d)\r\n", trap_id, get_trap_id);
        }

        if (trap_config.core_config_arr_len > 0)
        {
            trap_config_get.core_config_arr = core_config_arr_p;
        }

        /** Get trap action */
        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_compare(unit, trap_id, &trap_config, &trap_config_get));

        /** Destroy the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Deleted Trap: %d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS OAM TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FREE(core_config_arr_p);
    if (trap_type_list != NULL)
    {
        sal_free(trap_type_list);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks following APIs:
 *   bcm_rx_trap_type_get
 *   bcm_rx_trap_type_from_id_get
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_get(
    int unit)
{
    int trap_id;
    int rv = BCM_E_NONE;
    bcm_rx_trap_t trap_type;
    int trap_iter = 0, nof_traps;
    bcm_rx_trap_t *trap_type_list;
    dnx_rx_trap_class_e trap_class;
    SHR_FUNC_INIT_VARS(unit);

    for (trap_class = DNX_RX_TRAP_CLASS_PREDEFINED; trap_class < DNX_RX_TRAP_CLASS_NUM_OF; trap_class++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                        (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED, &trap_type_list, &nof_traps));

        for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
        {
            /** Get the Ingress trap_id */
            rv = bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &trap_id);
            if (rv == _SHR_E_UNAVAIL)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);

            /** Get the trap type from trap_id */
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &trap_type));
            LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap type: %d \n %s%s%s", trap_type, EMPTY, EMPTY, EMPTY);

            if (trap_type != trap_type_list[trap_iter])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "SET Trap type (%d) is not the same as GET Trap type (%d)\r\n", trap_type,
                             trap_type_list[trap_iter]);
            }
        }

        if (trap_type_list != NULL)
        {
            sal_free(trap_type_list);
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS GET TRAP ID AND TYPE TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Negative testing for trap actions set and get
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_gport - Gport encoding LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_neg_actions(
    int unit,
    bcm_gport_t lif_gport)
{
    int trap_id;
    bcm_rx_trap_config_t trap_config, trap_config_get;
    bcm_rx_trap_core_config_t *core_config_arr_p = NULL;

    bcm_error_t rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(core_config_arr_p, sizeof(*core_config_arr_p) * dnx_data_device.general.nof_cores_get(unit),
              "core_config_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, &trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------- bcm_rx_trap_set NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Preform get after create without set with filled get struct\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_ingress_fill(unit, lif_gport, &trap_config_get);
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_compare(unit, trap_id, &trap_config, &trap_config_get));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "   Preform get after create and set with filled get struct  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.core_config_arr = core_config_arr_p;
    ctest_dnx_rx_trap_ingress_fill(unit, lif_gport, &trap_config);
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));
    ctest_dnx_rx_trap_ingress_fill(unit, lif_gport, &trap_config_get);
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_compare(unit, trap_id, &trap_config, &trap_config_get));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "       Preform get with trap_id which is not allocated      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_get(unit, 0, &trap_config_get);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying flag which is not supported          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_POLICER;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_UNAVAIL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Dest port value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 438;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Dest group value             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_DEST_MULTICAST;
    trap_config.dest_group = 589;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Encap id value             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    BCM_GPORT_SUB_TYPE_LIF_SET(trap_config.encap_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x1234);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    BCM_GPORT_SUB_TYPE_LIF_SET(trap_config.encap_id2, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x5678);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Meter command value          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_METER_CMD;
    trap_config.meter_cmd = 100;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Priority value               \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_PRIO;
    trap_config.prio = 35;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Color value                  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COLOR;
    trap_config.color = 78;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Forwaring Header value       \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config.forwarding_header = 13;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Supplying invalid EgressForwaring Index value     \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
    trap_config.egress_forwarding_index = 8;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Supplying invalid Mapped trap strength            \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH;
    trap_config.mapped_trap_strength = 16;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                 Supplying invalid ECN value                \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ECN_VALUE;
    trap_config.ecn_value = 21;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                 Supplying invalid VSQ value                \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_VSQ;
    trap_config.vsq = 572;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                 Supplying invalid Visibility value         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_VISIBILITY;
    trap_config.visibility_value = 3;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Latency flow id clear        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.latency_flow_id_clear = 5;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid trap strength clear          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.trap_strength = -1;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "            Supplying invalid Stat Obj Array length         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 3;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "         Supplying invalid Stat Obj Counter command id      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].counter_command_id = 10;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].counter_command_id = 10;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                 Supplying invalid Stat Obj id              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].stat_id = 0x111111;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].stat_id = 0x222222;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying invalid Stat Obj is offset by qualifier enable  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].is_offset_by_qual_enable = 2;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].is_offset_by_qual_enable = 2;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "               Supplying invalid Stat Obj type              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].stat_object_type = 4;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].stat_object_type = 4;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Stat Obj is meter            \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].is_meter = 2;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].is_meter = 2;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "              Supplying invalid Stat Metadata mask          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_metadata_mask = 0x11111;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "              Supplying invalid Stat Clear bitmap           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_clear_bitmap = 0x400;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Supplying invalid Core config array length        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.core_config_arr_len = 3;
    trap_config.core_config_arr = core_config_arr_p;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Dest port per core           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    core_config_arr_p[0].dest_port = 0;
    core_config_arr_p[0].encap_id = 0;
    core_config_arr_p[0].encap_id2 = 0;
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        core_config_arr_p[1].dest_port = 0;
        core_config_arr_p[1].encap_id = 0;
        core_config_arr_p[1].encap_id2 = 0;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    core_config_arr_p[0].dest_port = 876;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    core_config_arr_p[0].dest_port = 0;

    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
        trap_config.core_config_arr_len = 2;
        trap_config.core_config_arr = core_config_arr_p;
        core_config_arr_p[1].dest_port = 678;

        rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
        if (rv != BCM_E_PORT)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }

        core_config_arr_p[1].dest_port = 0;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying invalid Encap id per core            \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    BCM_GPORT_SUB_TYPE_LIF_SET(core_config_arr_p[0].encap_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x3589);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    core_config_arr_p[0].encap_id = 0;

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    BCM_GPORT_SUB_TYPE_LIF_SET(core_config_arr_p[0].encap_id2, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x1894);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    core_config_arr_p[0].encap_id2 = 0;

    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
        trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
        trap_config.core_config_arr = core_config_arr_p;
        BCM_GPORT_SUB_TYPE_LIF_SET(core_config_arr_p[1].encap_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x5489);

        rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
        if (rv != BCM_E_INTERNAL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }

        core_config_arr_p[1].encap_id = 0;

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
        trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
        trap_config.core_config_arr = core_config_arr_p;
        BCM_GPORT_SUB_TYPE_LIF_SET(core_config_arr_p[1].encap_id2, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, 0x8721);

        rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
        if (rv != BCM_E_INTERNAL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }

        core_config_arr_p[1].encap_id2 = 0;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying both dest_port and dest_group        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_DEST_MULTICAST;
    trap_config.dest_port = 200;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config.dest_group, 10);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Supplying Stat Obj Counter cmnd id both updated and cleared\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 1;
    trap_config.stat_obj_config_arr[0].counter_command_id = 7;
    trap_config.stat_clear_bitmap = 0x1 << trap_config.stat_obj_config_arr[0].counter_command_id;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stat_obj_config_len = 2;
    trap_config.stat_obj_config_arr[1].counter_command_id = 3;
    trap_config.stat_clear_bitmap = 0x1 << trap_config.stat_obj_config_arr[1].counter_command_id;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "   Suppliying dest_port per core and also shared by cores   \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    trap_config.dest_port = 200;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "   Suppliying encap_id per core and also shared by cores    \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    trap_config.encap_id = 0x22b8;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);
    trap_config.core_config_arr = core_config_arr_p;
    trap_config.encap_id2 = 0x22b8;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying Core array pointer NULL with valid array length \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.core_config_arr_len = dnx_data_device.general.nof_cores_get(unit);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying Default ingress trap ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 200;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying non-allocated trap ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    trap_id = 89;
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 200;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying trap strength to User Defined trap \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 200;
    trap_config.trap_strength = 15;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "  Supplying fwd_header action = bcmRxTrapForwardingHeaderOamBfdPdu  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_UNAVAIL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------- bcm_rx_trap_set NEGATIVE TEST END -----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

exit:
    SHR_FREE(core_config_arr_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Negative testing for trap actions set and get
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_gport - Gport encoding LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_neg_predefined(
    int unit)
{
    int trap_id;
    bcm_error_t rv = BCM_E_NONE;
    int *null_pointer = NULL;
    char *trap_name = "";
    dnx_rx_trap_map_type_t predefined_trap_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_create() NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Create flags different than 0 and WITH_ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_REPLACE, bcmRxTrapTerminatedIpv4HasOptions, &trap_id);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Create flags  WITH_ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapTerminatedIpv4HasOptions, &trap_id);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Create trap type out of range \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, (bcmRxTrapCount + 5), &trap_id);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Create same trap twice \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapTerminatedIpv4HasOptions, &trap_id));

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapTerminatedIpv4HasOptions, &trap_id);
    if (rv != BCM_E_EXISTS)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_EXISTS! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Null Pointer \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapMplsUnexpectedBos, null_pointer);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "-------------- bcm_rx_trap_type_create() NEGATIVE TEST END -----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_get() NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Null Pointer \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapMplsUnexpectedBos, null_pointer);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Ingress Userdefined trap type             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_get() NEGATIVE TEST END ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_destroy() NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Non-created trap \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, bcmRxTrapMplsUnexpectedBos, &predefined_trap_info));
    rv = bcm_rx_trap_type_destroy(unit, predefined_trap_info.trap_id);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_destroy should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   Out of range trap id \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_destroy(unit, 603);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_destroy should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    rv = BCM_E_NONE;

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_destroy() NEGATIVE TEST END ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                  dnx_rx_trap_type_to_name invalid trap type \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = dnx_rx_trap_type_to_name(unit, (bcmRxTrapCount + 5), trap_name);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_type_to_name should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "      dnx_rx_trap_type_to_name unavailable trap type        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = dnx_rx_trap_type_to_name(unit, bcmRxTrapReserved, trap_name);
    if (rv != BCM_E_UNAVAIL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_type_to_name should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------- INGRESS SEMANTIC NEGATIVE TEST END ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Negative testing for trap actions set and get
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_gport - Gport encoding LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem_neg_userdefined(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int nof_ud_traps = dnx_data_trap.ingress.nof_user_defined_traps_get(unit);
    int trap_id = 370;
    int trap_iter;
    int *ud_trap_id_p = NULL;
    dnx_rx_trap_map_type_t predefined_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_ALLOC(ud_trap_id_p, sizeof(*ud_trap_id_p) * nof_ud_traps, "ud_trap_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_create() NEGATIVE TEST START ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Create WITH_ID same trap twice \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id));

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_EXISTS)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_EXISTS! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Create 1 more than max UD traps \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    for (trap_iter = 0; trap_iter < (nof_ud_traps - 1 - NOF_OAM_PUNT_TRAPS); trap_iter++)
    {
        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ud_trap_id_p[trap_iter]);
        if (rv != BCM_E_NONE && trap_iter < (nof_ud_traps - 3 - NOF_OAM_PUNT_TRAPS))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_FULL! . Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " Create UD trap_type WITH_ID of pre-defined trap. \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(dnx_rx_trap_predefined_map_info_get(unit, bcmRxTrapMplsUnexpectedBos, &predefined_trap_info));
    trap_id = predefined_trap_info.trap_id;

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "------------- bcm_rx_trap_type_create() NEGATIVE TEST END ----------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

exit:
    SHR_FREE(ud_trap_id_p);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function checks the configuration of Ingress traps.
 *
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_ingress_sem(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t lif_gport = 0;
    char *test_type, *test_mode;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_TYPE, test_type);
    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_MODE, test_mode);

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif_gport));

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_POSITIVE) == 0)
    {
        if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_PREDEFINED) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_predefined(unit, lif_gport));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_OAM) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_oam(unit, lif_gport));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_GET) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_get(unit));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Mode indicated is unsupported\n Supported modes (mode=): %s, %s, %s\n ",
                         CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_PREDEFINED, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_OAM,
                         CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_GET);
        }
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_NEGATIVE) == 0)
    {
        if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_PREDEFINED) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_neg_predefined(unit));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_USERDEFINED) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_neg_userdefined(unit));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_MODE_ACTIONS) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_neg_actions(unit, lif_gport));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test type indicated is unsupported\n Supported types (type=): %s, %s\n ",
                     CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_POSITIVE, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_NEGATIVE);
    }

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_gport));

exit:
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_gport));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_ingress_sem_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
