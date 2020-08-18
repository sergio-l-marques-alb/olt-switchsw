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
    {CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (predefined/oam/get)", "predefined"}
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
    {"ingress_sem_pre", "type=predefined", CTEST_POSTCOMMIT}
    ,
    {"ingress_sem_oam", "type=oam", CTEST_POSTCOMMIT}
    ,
    {"ingress_sem_get", "type=get", CTEST_POSTCOMMIT}
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
        BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_COLOR | BCM_RX_TRAP_UPDATE_ADD_VLAN |
        BCM_RX_TRAP_TRAP | BCM_RX_TRAP_BYPASS_FILTERS | BCM_RX_TRAP_LEARN_DISABLE |
        BCM_RX_TRAP_UPDATE_FORWARDING_HEADER | BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH |
        BCM_RX_TRAP_UPDATE_ECN_VALUE | BCM_RX_TRAP_UPDATE_VISIBILITY | BCM_RX_TRAP_UPDATE_VSQ;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config_p->dest_port, sal_rand() % 10);
    trap_config_p->prio = (sal_rand() % 2);
    trap_config_p->color = (sal_rand() % 2);
    trap_config_p->trap_strength = 1 + (sal_rand() % 14);
    trap_config_p->forwarding_header = (sal_rand() % 5);
    /** encap_id hold out_lif encoded as gport, encap_id2 holds raw out_lif value */
    trap_config_p->encap_id = gport_encap_id;
    trap_config_p->encap_id2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_encap_id);
    trap_config_p->meter_cmd = (sal_rand() % 5);
    trap_config_p->mapped_trap_strength = 1 + (sal_rand() % 14);
    trap_config_p->ecn_value = (sal_rand() % 7);
    trap_config_p->vsq = (sal_rand() % 255);
    trap_config_p->visibility_value = (sal_rand() % 1);
    trap_config_p->latency_flow_id_clear = (sal_rand() % 1);
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
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(trap_config_p);
    /*
     * Create the Ingress trap
     */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, trap_type, trap_id_p));
    LOG_INFO_EX(BSL_LOG_MODULE, "Created Trap: %d \n %s%s%s", *trap_id_p, EMPTY, EMPTY, EMPTY);

    ctest_dnx_rx_trap_ingress_fill(unit, lif_gport, trap_config_p);

    /*
     * Set the Ingress trap
     */
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, *trap_id_p, trap_config_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Gets the configuration of trap and compares with set configuration.
* \param [in] unit - unit ID
* \param [in] trap_id - trap id
* \param [in] trap_config_p - set trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e
ctest_dnx_rx_trap_ingress_get_and_compare(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p)
{
    bcm_rx_trap_config_t trap_config_get;
    int raw_encap_id2 = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get trap action */
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));
    /** Encap id returned by get function is encoded as gport, get raw value for compare */
    raw_encap_id2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(trap_config_get.encap_id2);

    /** Compare configuration */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->flags, &trap_config_get.flags,
                                                     sizeof(trap_config_get.flags), "flags"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->dest_port, &trap_config_get.dest_port,
                                                     sizeof(trap_config_get.dest_port), "dest_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->prio, &trap_config_get.prio, sizeof(trap_config_get.prio), "Priority (TC)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->color, &trap_config_get.color, sizeof(trap_config_get.color), "Color (DP)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->trap_strength, &trap_config_get.trap_strength,
                     sizeof(trap_config_get.trap_strength), "trap_strength"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->mapped_trap_strength, &trap_config_get.mapped_trap_strength,
                     sizeof(trap_config_get.mapped_trap_strength), "mapped_trap_strength"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->forwarding_header, &trap_config_get.forwarding_header,
                     sizeof(trap_config_get.forwarding_header), "forwarding_header"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->encap_id, &trap_config_get.encap_id,
                     sizeof(trap_config_get.encap_id), "encap_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->encap_id2, &raw_encap_id2, sizeof(trap_config_get.encap_id2), "encap_id2"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->meter_cmd, &trap_config_get.meter_cmd,
                     sizeof(trap_config_get.meter_cmd), "meter_cmd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->ecn_value, &trap_config_get.ecn_value,
                     sizeof(trap_config_get.ecn_value), "ecn_value"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->vsq, &trap_config_get.vsq, sizeof(trap_config_get.vsq), "vsq"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->visibility_value, &trap_config_get.visibility_value,
                     sizeof(trap_config_get.visibility_value), "visibility_value"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->latency_flow_id_clear, &trap_config_get.latency_flow_id_clear,
                     sizeof(trap_config_get.latency_flow_id_clear), "latency_flow_id_clear"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_len, &trap_config_get.stat_obj_config_len,
                     sizeof(trap_config_get.stat_obj_config_len), "stat_obj_config_len"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_clear_bitmap, &trap_config_get.stat_clear_bitmap,
                     sizeof(trap_config_get.stat_clear_bitmap), "stat_clear_bitmap"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_metadata_mask, &trap_config_get.stat_metadata_mask,
                     sizeof(trap_config_get.stat_metadata_mask), "stat_metadata_mask"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].counter_command_id,
                     &trap_config_get.stat_obj_config_arr[0].counter_command_id,
                     sizeof(trap_config_get.stat_obj_config_arr[0].counter_command_id), "counter_command_id0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_id,
                     &trap_config_get.stat_obj_config_arr[0].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_id), "stat_id0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_object_type,
                     &trap_config_get.stat_obj_config_arr[0].stat_object_type,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_object_type), "stat_object_type0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].is_meter,
                     &trap_config_get.stat_obj_config_arr[0].is_meter,
                     sizeof(trap_config_get.stat_obj_config_arr[0].is_meter), "is_meter0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].is_offset_by_qual_enable,
                     &trap_config_get.stat_obj_config_arr[0].is_offset_by_qual_enable,
                     sizeof(trap_config_get.stat_obj_config_arr[0].is_offset_by_qual_enable),
                     "is_offset_by_qual_enable0"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].counter_command_id,
                     &trap_config_get.stat_obj_config_arr[1].counter_command_id,
                     sizeof(trap_config_get.stat_obj_config_arr[1].counter_command_id), "counter_command_id1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_id,
                     &trap_config_get.stat_obj_config_arr[1].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_id), "stat_id1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_object_type,
                     &trap_config_get.stat_obj_config_arr[1].stat_object_type,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_object_type), "stat_object_type1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].is_meter,
                     &trap_config_get.stat_obj_config_arr[1].is_meter,
                     sizeof(trap_config_get.stat_obj_config_arr[1].is_meter), "is_meter1"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].is_offset_by_qual_enable,
                     &trap_config_get.stat_obj_config_arr[1].is_offset_by_qual_enable,
                     sizeof(trap_config_get.stat_obj_config_arr[1].is_offset_by_qual_enable),
                     "is_offset_by_qual_enable1"));

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
    int trap_id, get_trap_id;
    uint8 is_alloc;
    int trap_iter = 0, nof_traps;
    bcm_rx_trap_t *trap_type_list;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);    

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                    (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED, &trap_type_list, &nof_traps));

    for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
    {

        dnx_rx_trap_is_trap_id_allocated(unit, dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_id, &is_alloc);
        if (is_alloc == TRUE ||
            DNX_RX_TRAP_IS_DA_NOT_FOUND(dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_id) ||
            dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_id ==
            DBAL_ENUM_FVAL_INGRESS_TRAP_ID_UNKNOWN_DESTINATION)
        {
            continue;
        }
        LOG_INFO_EX(BSL_LOG_MODULE, "Trap: %s currently testing! \n %s%s%s",
                    dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_name, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_create_and_set(unit, trap_type_list[trap_iter], lif_gport,
                                                                 &trap_id, &trap_config));

        /** Get the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &get_trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);

        if (trap_id != get_trap_id)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "SET Trap-ID (%d) is not the same as GET Trap-ID (%d)\r\n", trap_id,
                         get_trap_id);
        }

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_get_and_compare(unit, trap_id, &trap_config));

        /** Destroy the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Deleted Trap: %d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS PREDEFINED TRAP CONFIGURATION TEST END.\n")));

exit:
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
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                    (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_OAM, &trap_type_list, &nof_traps));
    
    for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
    { 
        dnx_rx_trap_is_trap_id_allocated(unit, dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_id, &is_alloc);
        if (is_alloc == TRUE)
        {
            continue;
        }
    
        LOG_INFO_EX(BSL_LOG_MODULE, "Trap: %s currently testing! \n %s%s%s",
                    dnx_rx_trap_type_map[trap_type_list[trap_iter]].trap_name, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_create_and_set(unit, trap_type_list[trap_iter], lif_gport,
                                                                 &trap_id, &trap_config));
        
        /** Get the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &get_trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Gotten Trap: %d \n %s%s%s", get_trap_id, EMPTY, EMPTY, EMPTY);
        
        if (trap_id != get_trap_id)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "SET Trap-ID (%d) is not the same as GET Trap-ID (%d)\r\n", trap_id,
                         get_trap_id);
        }
        
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_ingress_get_and_compare(unit, trap_id, &trap_config));
        
        /** Destroy the Ingress trap */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        LOG_INFO_EX(BSL_LOG_MODULE, "Deleted Trap: %d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);
    
    }
    
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "INGRESS OAM TRAP CONFIGURATION TEST END.\n")));

exit:
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
    bcm_rx_trap_t trap_type;
    int trap_iter = 0, nof_traps;
    bcm_rx_trap_t *trap_type_list;
    dnx_rx_trap_class_e trap_class;
    SHR_FUNC_INIT_VARS(unit);    

    for (trap_class = DNX_RX_TRAP_CLASS_PREDEFINED ; trap_class < DNX_RX_TRAP_CLASS_NUM_OF ; trap_class++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_list
                        (unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED, &trap_type_list, &nof_traps));
        
        for (trap_iter = 0; trap_iter < nof_traps; trap_iter++)
        {
            /** Get the Ingress trap_id */
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, trap_type_list[trap_iter], &trap_id));
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
    bcm_gport_t lif_gport;
    char *test_type;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_TYPE, test_type);

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif_gport));

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_PREDEFINED) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_predefined(unit, lif_gport));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_OAM) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_oam(unit, lif_gport));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_INGRESS_OPTION_TEST_GET) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_ingress_sem_get(unit));
    }    

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_gport));

exit:
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
