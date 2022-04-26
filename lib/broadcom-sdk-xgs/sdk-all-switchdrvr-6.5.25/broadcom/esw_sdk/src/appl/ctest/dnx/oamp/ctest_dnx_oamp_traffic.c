/** \file ctest_dnx_oamp_traffic.c
 *
 * Tests for OAMP
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc_is */
#include <appl/diag/sand/diag_sand_framework.h>
#include <src/appl/ctest/dnx/oamp/ctest_dnx_oamp.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm/bfd.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/** Value used as a not-so-random seed to generate values   */
#define INIT_GEN_VALUE 0x5D2C85

/**
 *  Value used to generate a sequence of values that will be the
 *  same every time the test is run
 */
#define GEN_VALUE_DELTA 0xE5

/** Generalized OAMP memory types */
#define ENTRY_TYPE_QQ 0
#define ENTRY_TYPE_QS 1
#define ENTRY_TYPE_QO 2
#define ENTRY_TYPE_SS 3
#define ENTRY_TYPE_SO 4
#define ENTRY_TYPE_OO 5

/** Generalized OAMP group types */
#define GROUP_TYPE_SHORT 0
#define GROUP_TYPE_LONG 1
#define GROUP_TYPE_MAID48 2

/** Event type interrupts */
#define EVENT_TYPE_TIMEOUT 0
#define EVENT_TYPE_TIMEIN 1
#define EVENT_TYPE_RDI_SET 2
#define EVENT_TYPE_RDI_CLEAR 3
#define EVENT_TYPE_PORT_UP 4
#define EVENT_TYPE_PORT_DOWN 5
#define EVENT_TYPE_INTERFACE_UP 6
#define EVENT_TYPE_INTERFACE_DOWN 7
#define EVENT_TYPE_ALL 8

/** Currently the maximum number is limited from Gport limitation */
#define MAX_NUMBER_OF_PAIRS 200

/**
* Expected time creation per MEP.
* Could be increased in the time, since new feature will be added.
* Time changes:
*       Update when increase the time:
*         Date         |        value
*       07.03.2018              12000
*/
#define EXPECTED_CREATE_TIME_PER_MEP 12000
/**
* Since the first pair will also set the needed DBs
* the creation time will took longer.
* If a change in the actions or profiles are made,
* probably this time will need to be increased.
* Time changes:
*       Update when increase the time:
*         Date         |        value
*       07.03.2018              1900000
*/
#define EXPECTED_CREATE_TIME_FIRST_PAIR 1900000

/**
 * Global Variables
 */
static uint32 *gport_list;
static uint32 *rmeps;
static uint32 *lmeps;
static uint32 *timeout_event_count;
static uint32 *timein_event_count;
static uint32 *timein_event_state;
static uint32 starting_vlan = 100;
static uint32 number_of_gports = 0;
static int test_oam_timeout_event_counts = 0;
static int test_oam_timein_event_counts = 0;
static int test_oam_rdi_set_event_counts = 0;
static int test_oam_rdi_clear_event_counts = 0;
static int test_oam_port_state_down_event_counts = 0;
static int test_oam_port_state_up_event_counts = 0;
static int test_oam_interface_state_down_event_counts = 0;
static int test_oam_interface_state_up_event_counts = 0;
static uint8 num_levels = 8;

/**
 * Structure to keep global values of parameters
*/
typedef struct diag_dnx_oamp_traffic_test_general_s
{
    int num_entries;
    int num_updates;
    int group_id;
    int group_type;
    int memory_type;
    uint8 is_rdi;
    uint8 is_port_state_update;
    uint8 is_interface_state_update;
    uint8 with_dma;
    uint8 sematic_scale_validation;
    int first_mep_id;
    bcm_port_t port_1;
    bcm_port_t port_2;
    bcm_oam_profile_t ingress_acc_profile;
    bcm_oam_profile_t ingress_lif_profile;
    bcm_oam_profile_t egress_lif_profile;

} diag_dnx_oamp_traffic_test_general_t;

/**
 * Custom structure which hold all needed information about EP.
*/
typedef struct diag_dnx_oamp_traffic_test_ep_create_info_s
{
    int id;
    int vlan;
    int mac;
    int acc_profile_id;
    int memory_type;
    int name;
    int level;
    int counter_base_id;
    int flags;
    uint8 is_rdi;
    uint8 is_port_state_update;
    uint8 is_interface_state_update;
    bcm_gport_t gport;
    bcm_port_t port;

} diag_dnx_oamp_traffic_test_ep_create_info_t;

/**
 * \brief - Used to calculate unique event index.
 */
int
diag_dnx_oamp_test_oam_get_event_index(
    int unit,
    int16 name)
{
    int vlan, level;
    int mep_index;

    vlan = (name & 0xfffffff0) >> 4;
    level = (name & 0xe) >> 1;

    mep_index = 2 * (((vlan - starting_vlan) * num_levels) + level) + (name & 0x1);
    return mep_index;
}

/**
 * \brief - Reset the event counters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_type - EVENT_TYPE_TIMEOUT to reset timeout event counters,
 *                          EVENT_TYPE_TIMEIN to reset timein event counters,
 *                          EVENT_TYPE_RDI_SET to reset rdi set event counters,
 *                          EVENT_TYPE_RDI_CLEAR to reset rdi clear event counters,
 *                          EVENT_TYPE_PORT_UP to reset port state up event counters,
 *                          EVENT_TYPE_PORT_DOWN to reset port state down event counters,
 *                          EVENT_TYPE_INTERFACE_UP to reset interface state up event counters,
 *                          EVENT_TYPE_INTERFACE_DOWN to reset interface state down event counters,
 *                          EVENT_TYPE_ALL to reset all event counters,
 * \param [in/out] num_entries - Length of the event array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_reset_event_counters(
    int unit,
    int event_type,
    int num_entries)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);
    switch (event_type)
    {
        /**
        * Reset timeout event counters
        */
        case EVENT_TYPE_TIMEOUT:
        {
            test_oam_timeout_event_counts = 0;
            for (i = 0; i < (2 * num_entries); i++)
            {
                timein_event_count[i] = 0;
            }
            break;
        }
        /**
        * Reset timein event counters
        */
        case EVENT_TYPE_TIMEIN:
        {
            test_oam_timein_event_counts = 0;
            for (i = 0; i < (2 * num_entries); i++)
            {
                timeout_event_count[i] = 0;
            }
            break;
        }
        /**
        * Reset rdi event counters
        */
        case EVENT_TYPE_RDI_SET:
        {
            test_oam_rdi_set_event_counts = 0;
            break;
        }
        /**
        * Reset rdi event counters
        */
        case EVENT_TYPE_RDI_CLEAR:
        {
            test_oam_rdi_clear_event_counts = 0;
            break;
        }
        /**
        * Reset port state up event counters
        */
        case EVENT_TYPE_PORT_UP:
        {
            test_oam_port_state_up_event_counts = 0;
            break;
        }
        /**
        * Reset port state down event counters
        */
        case EVENT_TYPE_PORT_DOWN:
        {
            test_oam_port_state_down_event_counts = 0;
            break;
        }
        /**
        * Reset interface state up event counters
        */
        case EVENT_TYPE_INTERFACE_UP:
        {
            test_oam_interface_state_up_event_counts = 0;
            break;
        }
        /**
        * Reset interface state down event counters
        */
        case EVENT_TYPE_INTERFACE_DOWN:
        {
            test_oam_interface_state_down_event_counts = 0;
            break;
        }
        default:
        /**
        * Reset all event counters
        */
        {
            test_oam_timein_event_counts = 0;
            test_oam_timeout_event_counts = 0;
            test_oam_rdi_set_event_counts = 0;
            test_oam_rdi_clear_event_counts = 0;
            test_oam_port_state_up_event_counts = 0;
            test_oam_port_state_down_event_counts = 0;
            test_oam_interface_state_up_event_counts = 0;
            test_oam_interface_state_down_event_counts = 0;
            for (i = 0; i < (2 * num_entries); i++)
            {
                timeout_event_count[i] = 0;
                timein_event_count[i] = 0;
            }
            break;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the period of the remote MEP.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - Structure of the MEP.
 * \param [in/out] event_index - Event ID in the event array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_updateRMEP(
    int unit,
    int index,
    int *event_index)
{
    int multiplier;
    int rmep_id;
    bcm_oam_endpoint_info_t rmep;
    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_endpoint_info_t_init(&rmep);

    rmep_id = rmeps[index];
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, rmep_id, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

    *event_index = diag_dnx_oamp_test_oam_get_event_index(unit, rmep.name);

    /**
     * multiplier is used for calculating period for RMEP.
     * Expect to be between 25 and 35. Formula is:
     * (sal_rand() % (max_multiplier - min_multiplier + 1)) + min_multiplier
     */
    if (dnx_data_oam.oamp.rmep_db_non_zero_lifetime_units_limitation_get(unit))
    {
        /**
         * Because of hardware limitations, JR2 A0
         * has a lower maximum supported value
         * of RMEP CCM period
         */
        multiplier = (sal_rand() % 6) + 12;
    }
    else
    {
        multiplier = (sal_rand() % 11) + 25;
    }
    rmep.ccm_period = (BCM_OAM_ENDPOINT_CCM_PERIOD_100MS * multiplier) / 10;
    rmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

    LOG_CLI((BSL_META("Update period of Remote Endpoint [%d] to %d\n"), rmep_id, rmep.ccm_period));

    /**
     * Update mep configuration of the second remote MEP in the pair.
     */
    rmep_id = rmeps[index + 1];
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, rmep_id, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

    rmep.ccm_period = (BCM_OAM_ENDPOINT_CCM_PERIOD_100MS * multiplier) / 10;
    rmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

    LOG_CLI((BSL_META("Update period of Remote Endpoint [%d] to %d\n"), rmep_id, rmep.ccm_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set RDI on RX.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - MEP index in rmeps array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_update_rmep_rdi_on_rx_set(
    int unit,
    int mep_index)
{
    int index;
    int rmep_id;
    bcm_oam_endpoint_info_t rmep;
    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < 2; ++index)
    {
        bcm_oam_endpoint_info_t_init(&rmep);

        rmep_id = rmeps[mep_index + index];
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, rmep_id, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

        rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_REMOTE;
        rmep.flags2 |= BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI;
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &rmep), "Test Failed! bcm_oam_endpoint_get failed.");

        LOG_CLI((BSL_META("Set RDI on RX for Remote Endpoint [%d]\n"), rmep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Disable RDI from RX.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - MEP index in lmeps array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_lmep_rdi_from_rx_disable(
    int unit,
    int mep_index)
{
    int index;
    int lmep_id;
    bcm_oam_endpoint_info_t lmep;
    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < 2; ++index)
    {
        bcm_oam_endpoint_info_t_init(&lmep);

        lmep_id = lmeps[mep_index + index];
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, lmep_id, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

        lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_REPLACE;
        lmep.flags2 |= BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE;
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

        LOG_CLI((BSL_META("Disable RDI from RX for Local Endpoint [%d]\n"), lmep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the period of the local MEP.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - Structure of the MEP.
 * \param [in/out] event_index - Event ID in the event array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_updateLMEP(
    int unit,
    int index,
    int *event_index)
{
    int lmep_id;
    bcm_oam_endpoint_info_t lmep;
    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_endpoint_info_t_init(&lmep);

    lmep_id = lmeps[index];
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, lmep_id, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

    *event_index = diag_dnx_oamp_test_oam_get_event_index(unit, lmep.name);

    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "Test Failed! bcm_oam_endpoint_create failed.");

    LOG_CLI((BSL_META("Update period of Endpoint [%d] to %d event_index %d\n"), lmep_id, lmep.ccm_period,
             *event_index));

    /**
     * Update mep configuration of the second MEP in the pair.
     */
    lmep_id = lmeps[index + 1];

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, lmep_id, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "Test Failed! bcm_oam_endpoint_create failed.");

    LOG_CLI((BSL_META("Update period of Endpoint [%d] to %d\n"), lmep_id, lmep.ccm_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the port state of the local MEPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_index - lmeps array index of first MEP in pair.
 * \param [in] port_state - 0: port blocked, 1: port up.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_update_port_state(
    int unit,
    int mep_index,
    uint8 port_state)
{
    int index;
    int lmep_id;
    bcm_oam_endpoint_info_t lmep;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Update configuration of endpoints in the pair.
     */
    for (index = 0; index < 2; ++index)
    {
        bcm_oam_endpoint_info_t_init(&lmep);

        lmep_id = lmeps[mep_index + index];
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, lmep_id, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

        lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
        lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        lmep.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        if (port_state == 0)
        {
            /** Port state blocked */
            lmep.port_state = BCM_OAM_PORT_TLV_BLOCKED;
        }
        else
        {
            /** Port state up */
            lmep.port_state = BCM_OAM_PORT_TLV_UP;
        }

        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "Test Failed! bcm_oam_endpoint_create failed.");

        LOG_CLI((BSL_META("Update port state of Endpoint [%d]"), lmep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the interface state of the local MEPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_index - lmeps array index of first MEP in pair.
 * \param [in] interface_state - 0: interface down, 1: interface up.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_update_interface_state(
    int unit,
    int mep_index,
    uint8 interface_state)
{
    int index;
    int lmep_id;
    bcm_oam_endpoint_info_t lmep;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Update configuration of endpoints in the pair.
     */
    for (index = 0; index < 2; ++index)
    {
        bcm_oam_endpoint_info_t_init(&lmep);

        lmep_id = lmeps[mep_index + index];
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, lmep_id, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

        lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
        lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        lmep.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        if (interface_state == 0)
        {
            /** Interface state blocked */
            lmep.interface_state = BCM_OAM_INTERFACE_TLV_DOWN;
        }
        else
        {
            /** Interface state up */
            lmep.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }

        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "Test Failed! bcm_oam_endpoint_create failed.");

        LOG_CLI((BSL_META("Update interface state of Endpoint [%d]"), lmep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create local and remote MEPs
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in/out] ep1 - Structure of the MEP.
 * \param [in/out] ep2 - Structure of the MEP.
 * \param [in] mep_index - An index in MEP array.
 * \param [in] group - Group ID of the group where the EP will reside.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_create_endpoint_with_rmep(
    int unit,
    diag_dnx_oamp_traffic_test_ep_create_info_t * ep1,
    diag_dnx_oamp_traffic_test_ep_create_info_t * ep2,
    int mep_index,
    int group)
{
    bcm_oam_endpoint_info_t lmep = { 0 };
    bcm_oam_endpoint_info_t rmep = { 0 };
    int sampling_ratio_ind;
    bcm_mac_t mac_mep = { 0x00, 0x00, 0x00, 0x01, 0x02, 0xff };

    SHR_FUNC_INIT_VARS(unit);

    sampling_ratio_ind = sal_rand() % 9;

    bcm_oam_endpoint_info_t_init(&lmep);
    bcm_oam_endpoint_info_t_init(&rmep);

    lmep.type = bcmOAMEndpointTypeEthernet;
    lmep.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    lmep.id = ep1->id;
    lmep.group = group;
    lmep.name = ep1->name;
    lmep.level = ep1->level;
    lmep.vlan = ep1->vlan;
    BCM_GPORT_VLAN_PORT_ID_SET(lmep.gport, ep1->gport);
    BCM_GPORT_SYSTEM_PORT_ID_SET(lmep.tx_gport, ep1->port);
    lmep.outer_tpid = 0x8100;
    sal_memcpy(lmep.src_mac_address, &mac_mep, sizeof(mac_mep));
    lmep.src_mac_address[5] = ep1->mac;
    lmep.sampling_ratio = sampling_ratio_ind;
    lmep.acc_profile_id = ep1->acc_profile_id;
    lmep.endpoint_memory_type = ep1->memory_type;
    lmep.flags = ep1->flags;
    /** In case of port state update test */
    if (ep1->is_port_state_update)
    {
        lmep.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        lmep.port_state = BCM_OAM_PORT_TLV_UP;
    }
    /** In case of interface state update test */
    if (ep1->is_interface_state_update)
    {
        lmep.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        lmep.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    /**
     * Create MEP
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &lmep), "LMEP creation failed");
    lmeps[mep_index] = lmep.id;

    LOG_CLI((BSL_META("Local MEP with ID[%d] | MDL[%d] | Name[%d] | Vlan[%d] was created on LIF[%d]\n"),
             lmep.id, lmep.level, lmep.name, lmep.vlan, ep1->gport));

    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    /** In case of RDI test */
    if (ep2->is_rdi)
    {
        rmep.flags2 |= BCM_OAM_ENDPOINT2_RDI_ON_LOC | BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR;
    }
    /** In case of port state update test */
    if (ep1->is_port_state_update)
    {
        rmep.port_state = BCM_OAM_PORT_TLV_UP;
    }
    /** In case of interface state update test */
    if (ep1->is_interface_state_update)
    {
        rmep.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    rmep.id = lmep.id;
    rmep.local_id = lmep.id;
    rmep.name = ep2->name;
    rmep.loc_clear_threshold = 1;

    /**
     * Create RMEP
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, &rmep), "RMEP creation failed");
    rmeps[mep_index] = rmep.id;

    LOG_CLI((BSL_META("Remote MEP with ID[%d] Name[%d] was created on MEP[%d]\n"), rmep.id, rmep.name, lmep.id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set callback to calculate event counts.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Not used in this scope.
 * \param [in] event_type - Event type, could be:
 *                              Timeout,
 *                              Timein,
 *                              RDI.
 * \param [in] group - Not used in this scope.
 * \param [in] endpoint - MEP id, used to calculate EP index.
 * \param [in] user_data - Additional user data if it's needed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_cb(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
{

    bcm_oam_endpoint_info_t lmep;
    int mep_index;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Flags %d, Event type %d Group %d,Endpoint 0x%x .\n"), flags, event_type, group, endpoint));

    /**
    * Get endpoint information.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, endpoint, &lmep), "Test Failed! bcm_oam_endpoint_get failed.");

    /**
    * Calculate EP index, based on MEP information.
    */
    mep_index = diag_dnx_oamp_test_oam_get_event_index(unit, lmep.name);

    if (event_type == bcmOAMEventEndpointCCMTimeout)
    {
        /** Increase the event count per EP Index */
        ++timeout_event_count[mep_index];
        /** Increase the global event count */
        ++test_oam_timeout_event_counts;
        /**
         * In case of second timeout event raise an error.
         * In case of timeout set the state to "1".
        */
        if (timein_event_state[mep_index] == 1)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "TimeOut Error, mep_index: %d\n", mep_index);
        }
        timein_event_state[mep_index] = 1;
    }
    else if (event_type == bcmOAMEventEndpointCCMTimein)
    {
        /** Increase the event count per EP Index */
        timein_event_count[mep_index]++;
        /** Increase the global event count */
        test_oam_timein_event_counts++;
        /**
         * In case of timein set the state to "0".
        */
        if (timein_event_state[mep_index] == 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "TimeIn Error, mep_index: %d\n", mep_index);
        }
        timein_event_state[mep_index] = 0;
    }
    else if (event_type == bcmOAMEventEndpointRemote)
    {
        test_oam_rdi_set_event_counts++;
    }
    else if (event_type == bcmOAMEventEndpointRemoteUp)
    {
        test_oam_rdi_clear_event_counts++;
    }
    else if (event_type == bcmOAMEventEndpointPortDown)
    {
        test_oam_port_state_down_event_counts++;
    }
    else if (event_type == bcmOAMEventEndpointPortUp)
    {
        test_oam_port_state_up_event_counts++;
    }
    else if (event_type == bcmOAMEventEndpointInterfaceDown)
    {
        test_oam_interface_state_down_event_counts++;
    }
    else if (event_type == bcmOAMEventEndpointInterfaceUp)
    {
        test_oam_interface_state_up_event_counts++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register all types of events that are going to be tested.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_register_events(
    int unit)
{
    bcm_oam_event_types_t timeout_event, timein_event, rdi_event, port_interface_event;
    SHR_FUNC_INIT_VARS(unit);

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timeout_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timein_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(rdi_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timeout_event, diag_dnx_oamp_test_oam_cb, (void *) 1),
                        "Test failed! Register of timeout event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oamp_test_oam_cb, (void *) 2),
                        "Test failed! Register of timein event failed.");

    BCM_OAM_EVENT_TYPE_SET(rdi_event, bcmOAMEventEndpointRemote);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, rdi_event, diag_dnx_oamp_test_oam_cb, (void *) 3),
                        "Test failed! Register of rdi set event failed.");

    BCM_OAM_EVENT_TYPE_SET(rdi_event, bcmOAMEventEndpointRemoteUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, rdi_event, diag_dnx_oamp_test_oam_cb, (void *) 4),
                        "Test failed! Register of rdi clear event failed.");

    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, port_interface_event, diag_dnx_oamp_test_oam_cb, (void *) 5),
                        "Test failed! Register of port event failed.");

    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, port_interface_event, diag_dnx_oamp_test_oam_cb, (void *) 6),
                        "Test failed! Register of port event failed.");

    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, port_interface_event, diag_dnx_oamp_test_oam_cb, (void *) 7),
                        "Test failed! Register of interface event failed.");

    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, port_interface_event, diag_dnx_oamp_test_oam_cb, (void *) 8),
                        "Test failed! Register of interface event failed.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set unique name based on level and vlan
 */
static int
diag_dnx_oamp_test_oam_vlan_level__to_name(
    int vlan,
    int level)
{
    return vlan + level;
}

/**
 * \brief - Validate that the raised events count is higher than
 *          exact number of events.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_type - EVENT_TYPE_TIMEOUT for timeout events,
 *                          EVENT_TYPE_TIMEIN for timein events,
 *                          EVENT_TYPE_RDI_SET to reset rdi set event counters,
 *                          EVENT_TYPE_RDI_CLEAR to reset rdi clear event counters,
 *                          EVENT_TYPE_PORT_UP to reset port state up event counters,
 *                          EVENT_TYPE_PORT_DOWN to reset port state down event counters,
 *                          EVENT_TYPE_INTERFACE_UP to reset interface state up event counters,
 *                          EVENT_TYPE_INTERFACE_DOWN to reset interface state down event counters
 * \param [in] expected_value - number if expected events.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_check_non_expected_event_interupts(
    int unit,
    int event_type,
    int expected_value)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Wait a bit to raise the interrupts */
    sal_msleep(10);
    switch (event_type)
    {
        case EVENT_TYPE_TIMEOUT:
            if (test_oam_timeout_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEOUT events, expected %d actual %d\n",
                             expected_value, test_oam_timeout_event_counts);
            }
            break;
        case EVENT_TYPE_TIMEIN:
            if (test_oam_timein_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEIN events, expected %d actual %d\n",
                             expected_value, test_oam_timein_event_counts);
            }
            break;
        case EVENT_TYPE_RDI_SET:
            if (test_oam_rdi_set_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected RDI set events, expected %d actual %d\n",
                             expected_value, test_oam_rdi_set_event_counts);
            }
            break;
        case EVENT_TYPE_RDI_CLEAR:
            if (test_oam_rdi_clear_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected RDI clear events, expected %d actual %d\n",
                             expected_value, test_oam_rdi_clear_event_counts);
            }
            break;
        case EVENT_TYPE_PORT_UP:
            if (test_oam_port_state_up_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected port up events, expected %d actual %d\n",
                             expected_value, test_oam_port_state_up_event_counts);
            }
            break;
        case EVENT_TYPE_PORT_DOWN:
            if (test_oam_port_state_down_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected port down events, expected %d actual %d\n",
                             expected_value, test_oam_port_state_down_event_counts);
            }
            break;
        case EVENT_TYPE_INTERFACE_UP:
            if (test_oam_interface_state_up_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected interface up events, expected %d actual %d\n",
                             expected_value, test_oam_interface_state_up_event_counts);
            }
            break;
        case EVENT_TYPE_INTERFACE_DOWN:
            if (test_oam_interface_state_down_event_counts > expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected interface down events, expected %d actual %d\n",
                             expected_value, test_oam_interface_state_down_event_counts);
            }
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Not supported event type!\n");
    }

    LOG_CLI((BSL_META("Event validation pass, non unexpected interrupts were raised!\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validate that the raised events count is lower than
 *          exact number of events.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_type - EVENT_TYPE_TIMEOUT for timeout events,
                            EVENT_TYPE_TIMEIN for timein events,
 *                          EVENT_TYPE_RDI_SET to reset rdi set event counters,
 *                          EVENT_TYPE_RDI_CLEAR to reset rdi clear event counters,
 *                          EVENT_TYPE_PORT_UP to reset port state up event counters,
 *                          EVENT_TYPE_PORT_DOWN to reset port state down event counters,
 *                          EVENT_TYPE_INTERFACE_UP to reset interface state up event counters,
 *                          EVENT_TYPE_INTERFACE_DOWN to reset interface state down event counters
 * \param [in] expected_value - number if expected events.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_check_expected_event_interupts(
    int unit,
    int event_type,
    int expected_value)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Wait a bit to raise the interrupts*/
    sal_msleep(10);
    switch (event_type)
    {
        case EVENT_TYPE_TIMEOUT:
            if (test_oam_timeout_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEOUT events, expected %d actual %d\n",
                             expected_value, test_oam_timeout_event_counts);
            }
            break;
        case EVENT_TYPE_TIMEIN:
            if (test_oam_timein_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEIN events, expected %d actual %d\n",
                             expected_value, test_oam_timein_event_counts);
            }
            break;
        case EVENT_TYPE_RDI_SET:
            if (test_oam_rdi_set_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected RDI set events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_rdi_set_event_counts);
            }
            break;
        case EVENT_TYPE_RDI_CLEAR:
            if (test_oam_rdi_clear_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected RDI clear events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_rdi_clear_event_counts);
            }
            break;
        case EVENT_TYPE_PORT_UP:
            if (test_oam_port_state_up_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected port up events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_port_state_up_event_counts);
            }
            break;
        case EVENT_TYPE_PORT_DOWN:
            if (test_oam_port_state_down_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected port down events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_port_state_down_event_counts);
            }
            break;
        case EVENT_TYPE_INTERFACE_UP:
            if (test_oam_interface_state_up_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected interface up events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_interface_state_up_event_counts);
            }
            break;
        case EVENT_TYPE_INTERFACE_DOWN:
            if (test_oam_interface_state_down_event_counts < expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed! Unexpected interface down events, expected %d expected_value actual %d\n",
                             expected_value, test_oam_interface_state_down_event_counts);
            }
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Not supported event type!\n");
    }

    LOG_CLI((BSL_META("Event validation pass, expected interrupts were raised!\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function is used to enable and disable
 *           loop-back connection of both ports.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] port_1 - Port on which loopback action will be set.
 * \param [in] port_2 - Port on which loopback action will be set.
 * \param [in] action - LOOP_BACK_ENABLE to enable loopback,
 *                      LOOP_BACK_DISABLE to disable the loopback.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_test_loopback_on_off(
    int unit,
    bcm_port_t port_1,
    bcm_port_t port_2,
    int action)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(ADAPTER_SERVER_MODE)
    SHR_CLI_EXIT_IF_ERR(bcm_port_loopback_set(unit, port_1, action),
                        "Test failed! LoopBack of port = %d action = %d failed\n", port_1, action);

    SHR_CLI_EXIT_IF_ERR(bcm_port_loopback_set(unit, port_2, action),
                        "Test failed! LoopBack of port = %d  action = %d failed\n", port_2, action);
exit:
#endif
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates inLIF.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] port - Port of the inLIF.
 * \param [in] vlan - VLAN of the inLIF.
 * \param [in/out] vlan_port_id - inLIF index.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_create_vlan_gport(
    int unit,
    bcm_port_t port,
    int vlan,
    int *vlan_port_id)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Create In-LIF */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.match_vlan = vlan;
    vlan_port.port = port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed. Could not create In-LIF\n");

    LOG_CLI((BSL_META("GPORT with ID[%d] was created.\n"), vlan_port.vlan_port_id));

    *vlan_port_id = vlan_port.vlan_port_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Switches the MEP_id in the given index,
            in the lmeps and rmeps arrays,
            with the last MEP pair.
 *
 * \param [in] index - Entry index.
 * \param [in] length - Length of the array.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void
diag_dnx_oamp_test_oam_switch_endpoints(
    int index,
    int length)
{
    int temp;

    temp = lmeps[index];
    lmeps[index] = lmeps[length - 2];
    lmeps[length - 2] = temp;

    temp = lmeps[index + 1];
    lmeps[index + 1] = lmeps[length - 1];
    lmeps[length - 1] = temp;

    temp = rmeps[index];
    rmeps[index] = rmeps[length - 2];
    rmeps[length - 2] = temp;

    temp = rmeps[index + 1];
    rmeps[index + 1] = rmeps[length - 1];
    rmeps[length - 1] = temp;

}

/**
 * \brief - Reset time variables.
 *
 * \param [in/out] time_param - Used to store time variables.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void
diag_dnx_oamp_traffic_test_time_calc_t_init(
    diag_dnx_oamp_traffic_test_time_calc_t * time_param)
{
    time_param->start_time = 0;
    time_param->end_time = 0;
    time_param->interaction = 0;
    time_param->actual_create_time = 0;
    time_param->expected_max_create_time = 0;
}

/**
 * \brief - Calculate current and maximum time.
 *
 * \param [in/out] time_param - Used to store time variables.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void
diag_dnx_oamp_traffic_test_time_calculation(
    diag_dnx_oamp_traffic_test_time_calc_t * time_param)
{
    /**
    * The first pair of MEPs were created.
    * The needed time is more than the rest,
    * because the DB and profiles must be configured for,
    * the first time.
    */
    if (time_param->interaction == 0)
    {
        time_param->expected_max_create_time += EXPECTED_CREATE_TIME_FIRST_PAIR;
    }
    time_param->actual_create_time += (time_param->end_time - time_param->start_time);
    time_param->interaction += 2;
    time_param->expected_max_create_time += (EXPECTED_CREATE_TIME_PER_MEP * 2);
}

/**
 * \brief - Create basic OAM environment.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in/out] global_parameters - control parameter for the environment.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_environment_init(
    int unit,
    diag_dnx_oamp_traffic_test_general_t * global_parameters)
{
    uint8 profile_id = 0x1;
    uint32 profile_flags = 0;
    bcm_oam_group_info_t group_info;
    uint8 group_name_short[] = { 0x1, 0x3, 0x2, 0xd, 0xe };
    uint8 group_name_long[] =
        { 0x01, 0x20, 0x0d, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xab, 0xcd, 0x01, 0x02, 0x03, 0x04, 0x05 };
    uint8 group_name_maid48[] =
        { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11,
        0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
        0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30
    };

    SHR_FUNC_INIT_VARS(unit);

    /** Enable the loopback in case of traffic validation only */
    if (!global_parameters->sematic_scale_validation)
    {
        /**
        * Add loop-back connection to both ports.
        */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                            (unit, global_parameters->port_1, global_parameters->port_2, LOOP_BACK_ENABLE),
                            "Test failed! LoopBack enable failed!\n");
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                            (unit, global_parameters->port_2, global_parameters->port_2, LOOP_BACK_ENABLE),
                            "Test failed! LoopBack enable failed!\n");
    }
    /**
    * Classify both ports.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_port_class_set(unit, global_parameters->port_1, bcmPortClassId, global_parameters->port_1);,
                        "Test failed! Class set of port = %d failed\n", global_parameters->port_1);
    SHR_CLI_EXIT_IF_ERR(bcm_port_class_set(unit, global_parameters->port_2, bcmPortClassId, global_parameters->port_2);,
                        "Test failed! Class set of port = %d failed\n", global_parameters->port_2);
    /**
    * Define all type tested events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_register_events(unit), "Test failed! Event registration failed\n");

    /**
    * Reset event counters.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_reset_event_counters
                        (unit, EVENT_TYPE_ALL, global_parameters->num_entries), "Test failed! Events are not reset!\n");

    /**
    * Create group with name based of group type parameter.
    */
    bcm_oam_group_info_t_init(&group_info);
    if (global_parameters->group_type == GROUP_TYPE_LONG)
    {
        /*
         * ICC group.
         */
        sal_memcpy(group_info.name, &group_name_long, sizeof(group_name_long));
    }
    else if (global_parameters->group_type == GROUP_TYPE_MAID48)
    {
        /*
         * MAID48 group.
         */
        group_info.id = 6;
        group_info.group_name_index = 0x100a4;
        group_info.flags = BCM_OAM_GROUP_WITH_ID | BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        sal_memcpy(group_info.name, &group_name_maid48, sizeof(group_name_maid48));
    }
    else
    {
        /*
         * Short group.
         * Choose as default.
         */
        sal_memcpy(group_info.name, &group_name_short, sizeof(group_name_short));
    }

    SHR_CLI_EXIT_IF_ERR(bcm_oam_group_create(unit, &group_info), "Test failed.  Could not create group.\n");
    global_parameters->group_id = group_info.id;

    /**
    * Get ingress Acc profile.
    * Since the test is oriented to OAMP, use existing profile.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, profile_id, bcmOAMProfileIngressAcceleratedEndpoint,
                                                       &profile_flags, &global_parameters->ingress_acc_profile),
                        "Test failed. Could not get ingress acc profile.\n");
    /**
    * Get ingress LIF profile.
    * Since the test is oriented to OAMP, use existing profile.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, profile_id, bcmOAMProfileIngressLIF,
                                                       &profile_flags, &global_parameters->ingress_lif_profile),
                        "Test failed. Could not get ingress profile.\n");
    /**
    * Get ingress Acc profile.
    * Since the test is oriented to OAMP, use existing profile.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, profile_id, bcmOAMProfileEgressLIF,
                                                       &profile_flags, &global_parameters->egress_lif_profile),
                        "Test failed. Could not get egress profile.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Perform basic validation checks.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] global_parameters - test parameter which will be validated.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_validity_check(
    int unit,
    diag_dnx_oamp_traffic_test_general_t global_parameters)
{
    int nof_max_endpoints;
    int threshold_value;
    SHR_FUNC_INIT_VARS(unit);

    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    threshold_value = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    if (threshold_value == 0)
    {
        if (global_parameters.num_entries > 8192)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Test failed! Max number of MEP pairs is 8192, actual: %d. Aborting test.\n",
                         global_parameters.num_entries);
        }
    }
    else
    {
        if (global_parameters.num_entries > ((threshold_value + ((nof_max_endpoints - threshold_value) / 4)) / 2))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Test failed! Max number of MEP pairs is %d, actual: %d. Aborting test.\n",
                         ((threshold_value + ((nof_max_endpoints - threshold_value) / 4)) / 2),
                         global_parameters.num_entries);
        }
    }

    /** It's not possible to update more MEPs than actual MEPs */
    if (global_parameters.num_updates > global_parameters.num_entries)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Test failed! Illegal number of Updates. Aborting test.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Choose memory type of the MEPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] memory_type - Shows the memory type of both MEPs.
 * \param [in] mep_1 - EP structure of the first MEP.
 * \param [in] mep_2 - EP structure of the second MEP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_oam_set_memory_type(
    int unit,
    uint8 memory_type,
    diag_dnx_oamp_traffic_test_ep_create_info_t * mep_1,
    diag_dnx_oamp_traffic_test_ep_create_info_t * mep_2)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (memory_type)
    {
        case ENTRY_TYPE_QQ:
            mep_1->memory_type = bcmOamEndpointMemoryTypeShortEntry;
            mep_2->memory_type = bcmOamEndpointMemoryTypeShortEntry;
            break;
        case ENTRY_TYPE_QS:
            mep_1->memory_type = bcmOamEndpointMemoryTypeShortEntry;
            mep_2->memory_type = bcmOamEndpointMemoryTypeSelfContained;
            break;
        case ENTRY_TYPE_QO:
            mep_1->memory_type = bcmOamEndpointMemoryTypeShortEntry;
            mep_2->memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
            break;
        case ENTRY_TYPE_SS:
            mep_1->memory_type = bcmOamEndpointMemoryTypeSelfContained;
            mep_2->memory_type = bcmOamEndpointMemoryTypeSelfContained;
            break;
        case ENTRY_TYPE_SO:
            mep_1->memory_type = bcmOamEndpointMemoryTypeSelfContained;
            mep_2->memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
            break;
        case ENTRY_TYPE_OO:
            mep_1->memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
            mep_2->memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Not supported memory type!\n");
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - This function tests OAMP with traffic.
 *          Creates different pairs of MEPs and validate event changes.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_traffic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_dnx_oamp_traffic_test_general_t global_parameters = { 0 };
    diag_dnx_oamp_traffic_test_time_calc_t time_parameters = { 0 };
    diag_dnx_oamp_traffic_test_ep_create_info_t ep1 = { 0 };
    diag_dnx_oamp_traffic_test_ep_create_info_t ep2 = { 0 };
    int vlan_port_id, vlan_port_id2;
    int endpoint = 0, update = 0;
    int event_index = 0, index = 0, exp_event_cnt = 0, event_type = 0;
    uint32 vlan;
    uint8 level;
    int mep_id = 0;
    rhhandle_t ctest_soc_set_h = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Port on which will reside the MEPs */
    SH_SAND_GET_INT32("Port", global_parameters.port_1);
    /** Port on which will reside the MEPs */
    SH_SAND_GET_INT32("Port2", global_parameters.port_2);
    /** Number pairs of MEPs */
    SH_SAND_GET_INT32("Num_ENTries", global_parameters.num_entries);
    /** Number of updated that will be performed during the run of the test */
    SH_SAND_GET_INT32("Num_UPDates", global_parameters.num_updates);
    /** Set the group type of the MEP */
    SH_SAND_GET_INT32("GRouP_Type", global_parameters.group_type);
    /** Set the memory type of the MEP */
    SH_SAND_GET_INT32("MEMory_Type", global_parameters.memory_type);
    /** Test RDI events */
    SH_SAND_GET_BOOL("is_RDI", global_parameters.is_rdi);
    /** Test port state update events */
    SH_SAND_GET_BOOL("is_Port_state_UPdate", global_parameters.is_port_state_update);
    /** Test interface state update events */
    SH_SAND_GET_BOOL("is_InterFace_state_UPdate", global_parameters.is_interface_state_update);
    /** Test interrupt with DMA mechanism */
    SH_SAND_GET_BOOL("with_dma", global_parameters.with_dma);
    /** Test the scale only */
    SH_SAND_GET_INT32("SEMantic_run_ONly", global_parameters.sematic_scale_validation);
    SH_SAND_GET_INT32("with_ID", global_parameters.first_mep_id);

    LOG_CLI((BSL_META("Test with %d entries\n"), global_parameters.num_entries));
    LOG_CLI((BSL_META("Test with updates = %d\n"), global_parameters.num_updates));
    LOG_CLI((BSL_META("Test on port = %d\n"), global_parameters.port_1));
    LOG_CLI((BSL_META("Test on port = %d\n"), global_parameters.port_2));

#if defined(ADAPTER_SERVER_MODE)
    /** In case of scale run skip this test */
    if (global_parameters.sematic_scale_validation)
    {
        LOG_CLI((BSL_META("The adapter has mdb limitation, aborting the run..\n")));
        SHR_FUNC_EXIT;
    }
#endif

    if (global_parameters.with_dma)
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"oamp_fifo_dma_event_interface_enable", "1"}
            ,
            {"oamp_fifo_dma_event_interface_timeout", "0"}
            ,
            {"oamp_fifo_dma_event_interface_buffer_size", "8192"}
            ,
            {"oamp_fifo_dma_event_interface_threshold", "1"}
            ,
            {NULL}
        };

        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

#if !defined(ADAPTER_SERVER_MODE)
    /** In case of scanner limitation skip this test */
    if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_scanner_limitation))
    {
        LOG_CLI((BSL_META("The device has scanner_limitation, aborting the run..\n")));
        SHR_FUNC_EXIT;
    }
#endif

    /** Allocate memory for test variables */
    rmeps = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "rmeps_db");
    lmeps = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "lmeps_db");
    gport_list = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "gport_list");
    timein_event_count = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "timein_event_count");
    timeout_event_count = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "timeout_event_count");
    timein_event_state = sal_alloc((global_parameters.num_entries + 1) * 2 * 4, "timein_event_state");
    /** Init variables */
    diag_dnx_oamp_traffic_test_time_calc_t_init(&time_parameters);
    number_of_gports = 0;

    /** Perform basic validity checks */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_validity_check(unit, global_parameters),
                        "{1}\t Test failed! Init of the environment has failed! \n");

    /**
    * Configure OAM environment.
    * Set LoopBack on both ports.
    * Get profiles.
    * Create OAM group(SHOR, LONG or MAID48).
    * Register events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_environment_init(unit, &global_parameters),
                        "{2}\t Test failed! Init of the environment has failed! \n");

    if (global_parameters.first_mep_id != -1)
    {
        mep_id = global_parameters.first_mep_id;
    }
    /**
    * Loop "N" number of times based on number of entries.
    * In each iteration create 2 MEPs with one RMEP per MEP.
    * In each four iterations create 2 inLIFs, 1 per 8 MEPs.
    */
    for (endpoint = 0; endpoint < global_parameters.num_entries; ++endpoint)
    {
        /** Create unique vlan per each pair of MEPs */
        vlan = starting_vlan + endpoint / (num_levels);
        /** Create unique level per each pair of MEPs per inLIF */
        level = endpoint % num_levels;

        /**
        * Create new set of inLIFs for every 8 pairs of MEPs.
        * Set LIF profile for each inLIF.
        */
        if (level == 0)
        {
            /**
            * Create inLIF per 8 MEPs.
            * Store the value in GPORT list for further usage.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_create_vlan_gport
                                (unit, global_parameters.port_1, vlan, &vlan_port_id),
                                "{3}\t Test failed! InLIF creation failed \n");
            gport_list[number_of_gports] = vlan_port_id;
            number_of_gports++;

            /**
            * Create inLIF per 8 MEPs.
            * Store the value in GPORT list for further usage.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_create_vlan_gport
                                (unit, global_parameters.port_2, vlan, &vlan_port_id2),
                                "{4}\t Test failed! InLIF creation failed \n");
            gport_list[number_of_gports] = vlan_port_id2;
            number_of_gports++;

            /** Bind lif with profile */
            SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set(unit, 0, vlan_port_id, global_parameters.ingress_lif_profile,
                                                        global_parameters.egress_lif_profile),
                                "{5}\t Test failed. Could not bind profiles to lif\n");

            /** Bind lif with profile */
            SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set(unit, 0, vlan_port_id2, global_parameters.ingress_lif_profile,
                                                        global_parameters.egress_lif_profile),
                                "{6}\t Test failed. Could not bind profiles to lif\n");

            LOG_CLI((BSL_META("Profile set of gports [DONE]\n")));
        }

        /**
        * Fill custom structure for MEP creation
        */
        ep1.name = diag_dnx_oamp_test_oam_vlan_level__to_name(vlan, level);
        ep1.gport = vlan_port_id2;
        ep1.vlan = vlan;
        ep1.level = level;
        ep1.port = global_parameters.port_1;
        if (global_parameters.first_mep_id != -1)
        {
            ep1.id = mep_id * 4;
            ep1.flags = BCM_OAM_ENDPOINT_WITH_ID;
        }

        /** Use unique MAC per MEP */
        ep1.mac = vlan;
        ep1.is_rdi = global_parameters.is_rdi;
        ep1.is_port_state_update = global_parameters.is_port_state_update;
        ep1.is_interface_state_update = global_parameters.is_interface_state_update;
        ep1.acc_profile_id = global_parameters.ingress_acc_profile;
        mep_id++;
        /**
        * Fill custom structure for MEP creation
        */
        ep2.name = diag_dnx_oamp_test_oam_vlan_level__to_name(vlan, level);
        ep2.gport = vlan_port_id;
        ep2.vlan = vlan;
        ep2.port = global_parameters.port_2;
        ep2.level = level;
        if (global_parameters.first_mep_id != -1)
        {
            ep2.id = mep_id * 4;
            ep2.flags = BCM_OAM_ENDPOINT_WITH_ID;
        }
        /** Use unique MAC per MEP */
        ep2.mac = vlan + 1;
        ep2.is_rdi = global_parameters.is_rdi;
        ep2.is_port_state_update = global_parameters.is_port_state_update;
        ep2.is_interface_state_update = global_parameters.is_interface_state_update;
        ep2.acc_profile_id = global_parameters.ingress_acc_profile;
        mep_id++;
        /** Set memory type of the MEPs */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_set_memory_type(unit, global_parameters.memory_type, &ep1, &ep2),
                            "{7}\t Test failed! Test failed. Could not set memory type!\n");

        /**
        * Start the timer.
        */
        time_parameters.start_time = sal_time_usecs();

        /** Create connected endpoints on port1 and port2 */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_create_endpoint_with_rmep
                            (unit, &ep2, &ep1, (2 * endpoint), global_parameters.group_id),
                            "{8}\t Test failed! Endpoint creation failed \n");

        /** Create connected endpoints on port1 and port2 */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_create_endpoint_with_rmep
                            (unit, &ep1, &ep2, (2 * endpoint + 1), global_parameters.group_id),
                            "{9}\t Test failed! Endpoint creation failed \n");

        /**
        * Stop timer when both MEP with RMEPs are created.
        */
        time_parameters.end_time = sal_time_usecs();

        /**
        * Calculate current time interval for pair creation.
        * Add it to the actual maximum time.
        * Increase the expected time with 2 iterations.
        */
        diag_dnx_oamp_traffic_test_time_calculation(&time_parameters);
    }

    /**
    * Validate that the number of entries is equal to number of time calculations.
    */
    if (time_parameters.interaction != (global_parameters.num_entries * 2))
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "{10}\t Actual calculated MEPs[%d] is different than the actual MEPs[%d]\n",
                     time_parameters.actual_create_time, time_parameters.expected_max_create_time);
    }

/** Don't test the timing on adapter */
#if !defined(ADAPTER_SERVER_MODE)
    /**
    * Validate that actual time is lower than the expected.
    */
    if (time_parameters.actual_create_time > time_parameters.expected_max_create_time)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "{11}\t Actual creation time:%d is higher than expected:%d\n",
                     time_parameters.actual_create_time, time_parameters.expected_max_create_time);
    }
#endif

    if (global_parameters.sematic_scale_validation)
    {
        /** Destroy all MEPs */
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy_all(unit, global_parameters.group_id),
                            "{37}\t Test failed! bcm_oam_endpoint_destroy_all failed!\n");
        SHR_CLI_EXIT(_SHR_E_NONE, "*********************** SCALE TEST PASS !!! ***********************\n");
    }
    /** Wait 3 second to check for unexpected interrupts */
    sal_sleep(3);

    /**
    * Check that the no unexpected events were raised.
    */
    exp_event_cnt = 0;
    for (event_type = EVENT_TYPE_TIMEOUT; event_type < EVENT_TYPE_ALL; ++event_type)
    {
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_non_expected_event_interupts
                            (unit, event_type, exp_event_cnt),
                            "{12}\t Test failed! Unexpected number of events raised!\n");
    }

    /**
    * Disable the loop-back to raise timeout events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_DISABLE),
                        "{13}\t Test failed! LoopBack disable failed!\n");

    /** Wait 3 second to check for unexpected interrupts */
    sal_sleep(3);

    /**
    * Check that the correct amount of timeout events were raised.
    */
    exp_event_cnt = (endpoint * 2);
#if defined(ADAPTER_SERVER_MODE)
    exp_event_cnt = 0;
#endif
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts(unit, EVENT_TYPE_TIMEOUT, exp_event_cnt),
                        "{14}\t Test failed! Not enough timeout events were raised!\n");

    /**
    * Enable the loop-back to restore the connection.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_ENABLE),
                        "{15}\t Test failed! LoopBack disable failed!\n");

    /** Wait 3 second to check for unexpected interrupts */
    sal_sleep(3);

    /**
    * Check that the correct amount of timein events were raised.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts(unit, EVENT_TYPE_TIMEIN, exp_event_cnt),
                        "{16}\t Test failed! Not enough timein events were raised!\n");

    /**
    * Update test start here, add more info.
    * Perform two validation.
    * One per pair and one global
    */
    for (update = 1; update <= global_parameters.num_updates; ++update)
    {
        /** Reset all event counter before next validation */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_reset_event_counters
                            (unit, EVENT_TYPE_ALL, global_parameters.num_entries),
                            "{17}\t Test failed! Events are not reseted \n");
        /**
        * Reduce the index per each iteration.
        * In that way we can't hit the same entries.
        */
        index = (sal_rand() % (global_parameters.num_entries - update + 1));

        if (global_parameters.is_rdi)
        {
            /**
             * Update local MEP RDI by copying the RDI indication
             * from the last CCM packet received.
             */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_update_rmep_rdi_on_rx_set(unit, (2 * index)),
                                "{18}\t Test failed! Local MEP update failed!\n");
        }

        /**
        * Update the period of the MEPs.
        * After the update the period of the MEP will be higher than the
        * period of the RMEP, so timeout events will be raised.
        */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_updateLMEP(unit, (2 * index), &event_index),
                            "{19}\t Test failed! Local MEP update failed!\n");

        if (global_parameters.is_port_state_update)
        {
            /**
            * Update the port state of the MEPs to blocked.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_update_port_state(unit, (2 * index), 0),
                                "{20}\t Test failed! Local MEP port state update failed!\n");
        }

        if (global_parameters.is_interface_state_update)
        {
            /**
            * Update the interface state of the MEPs to down.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_update_interface_state(unit, (2 * index), 0),
                                "{21}\t Test failed! Local MEP interface state update failed!\n");
        }

        /**
        * Validate that timeout events are raised and the connection is lost.
        * We will have a lot of timein-timeout events, since the connection is restored and lost.
        * We need to validate that at least a single timeout is raised per pair.
        */
        sal_sleep(3);

        if (global_parameters.is_rdi)
        {
            /**
             * Set RDI bit on outgoing packets
             * from LOC indication of peer endpoint.
             */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_lmep_rdi_from_rx_disable(unit, (2 * index)),
                                "{22}\t Test failed! Local MEP update failed!\n");
        }

        exp_event_cnt = 2;
#if defined(ADAPTER_SERVER_MODE)
        exp_event_cnt = 0;
#endif
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                            (unit, EVENT_TYPE_TIMEOUT, exp_event_cnt),
                            "{23}\t Test failed! Unexpected number of timeout events raised!\n");
        if (global_parameters.is_rdi)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_RDI_SET, exp_event_cnt),
                                "{24}\t Test failed! Unexpected number of RDI set events raised!\n");
        }
        if (global_parameters.is_port_state_update)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_PORT_DOWN, exp_event_cnt),
                                "{25}\t Test failed! Unexpected number of port down events raised!\n");
        }
        if (global_parameters.is_interface_state_update)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_INTERFACE_DOWN, exp_event_cnt),
                                "{26}\t Test failed! Unexpected number of interface down events raised!\n");
        }

        /**
        * Increase the expected RMEP period and validate the timein.
        * After the update the RMEP period will be higher than the
        * period of the MEP and the connection will be restored.
        */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_updateRMEP(unit, (2 * index), &event_index),
                            "{27}\t test_oam_updateLMEP failed!\n");

        if (global_parameters.is_port_state_update)
        {
            /**
            * Update the port state of the MEPs to up.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_update_port_state(unit, (2 * index), 1),
                                "{28}\t Test failed! Local MEP port state update failed!\n");
        }

        if (global_parameters.is_interface_state_update)
        {
            /**
            * Update the interface state of the MEPs to up.
            */
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_update_interface_state(unit, (2 * index), 1),
                                "{29}\t Test failed! Local MEP interface state update failed!\n");
        }

        /**
        * Validate that timein events are raised and the connection is restored.
        * We will have a lot of timein-timeout events, since the connection is restored and lost.
        * We need to validate that at least a single timein is raised per pair.
        */
        sal_sleep(3);

        exp_event_cnt = 2;
#if defined(ADAPTER_SERVER_MODE)
        exp_event_cnt = 0;
#endif
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                            (unit, EVENT_TYPE_TIMEIN, exp_event_cnt),
                            "{30}\t Test failed! Unexpected number of timein events raised!\n");
        if (global_parameters.is_rdi)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_RDI_CLEAR, exp_event_cnt),
                                "{31}\t Test failed! Unexpected number of RDI clear events raised!\n");
        }
        if (global_parameters.is_port_state_update)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_PORT_UP, exp_event_cnt),
                                "{32}\t Test failed! Unexpected number of port up events raised!\n");
        }
        if (global_parameters.is_interface_state_update)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_expected_event_interupts
                                (unit, EVENT_TYPE_INTERFACE_UP, exp_event_cnt),
                                "{33}\t Test failed! Unexpected number of interface up events raised!\n");
        }

        /** Reset all event counter before next validation */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_reset_event_counters
                            (unit, EVENT_TYPE_ALL, global_parameters.num_entries),
                            "{34}\t Test failed! Events are not reseted \n");

        /**
        * Wait 3 second to check for unexpected interrupts.
        * Since the connection has been restored, we don't expect more events.
        */
        sal_sleep(3);

        /**
        * Check that the no unexpected events were raised.
        */
        exp_event_cnt = 0;
        for (event_type = EVENT_TYPE_TIMEOUT; event_type < EVENT_TYPE_ALL; ++event_type)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_oam_check_non_expected_event_interupts
                                (unit, event_type, exp_event_cnt),
                                "{35}\t Test failed! Unexpected number of events raised!\n");
        }

        /**
        * Move MEPs indexes in the end and proceed with next pair.
        */
        diag_dnx_oamp_test_oam_switch_endpoints((2 * index), (2 * (global_parameters.num_entries - update + 1)));
    }

    /**
    * Clean up.
    */
    LOG_CLI((BSL_META("*********************** TEST PASS !!! ***********************\n")));

    /** Destroy all MEPs */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy_all(unit, global_parameters.group_id),
                        "{37}\t Test failed! bcm_oam_endpoint_destroy_all failed!\n");
exit:

    /** Disable the loop-back. */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_DISABLE),
                        "{36}\t Test failed! LoopBack disable failed!\n");

    /** Wait a bit to avoid warnings during the MEPs' destroy */
    sal_sleep(1);

    /** Free allocated memory */
    sal_free(timein_event_count);
    sal_free(timeout_event_count);
    sal_free(rmeps);
    sal_free(lmeps);
    sal_free(gport_list);
    sal_free(timein_event_state);

    /** End of the test */
    LOG_CLI((BSL_META("*********************** TEST END !!! ***********************\n")));
    SHR_FUNC_EXIT;
}

/** Supported endpoint types   */
static sh_sand_enum_t diag_dnx_oamp_memory_all_type_enum_table[] = {
    {"Quarter_Quarter", ENTRY_TYPE_QQ},
    {"Quarter_Self", ENTRY_TYPE_QS},
    {"Quarter_Offloaded", ENTRY_TYPE_QO},
    {"Self_Self", ENTRY_TYPE_SS},
    {"Self_Offloaded", ENTRY_TYPE_SO},
    {"Offloaded_Offloaded", ENTRY_TYPE_OO},
    {NULL}
};

/** Supported endpoint types   */
static sh_sand_enum_t diag_dnx_oamp_group_all_type_enum_table[] = {
    {"SHORT", GROUP_TYPE_SHORT},
    {"LONG", GROUP_TYPE_LONG},
    {"MAID48", GROUP_TYPE_MAID48},
    {NULL}
};

/** Supported endpoint types   */
sh_sand_enum_t diag_dnx_oamp_traffic_test_type_enum_table[] = {
    {"TRAFFIC", TEST_TYPE_TRAFFIC},
    {"SEMANTIC", TEST_TYPE_SEMANTIC},
    {NULL}
};
/** Test arguments   */
sh_sand_option_t diag_dnx_oamp_traffic_test_options[] = {
    {"Port", SAL_FIELD_TYPE_INT32, "Number of entries", "13"},
    {"Port2", SAL_FIELD_TYPE_INT32, "Type of entry", "14"},
    {"Num_ENTries", SAL_FIELD_TYPE_INT32, "OAMP flags", "20"},
    {"MEMory_Type", SAL_FIELD_TYPE_INT32, "OAMP flags", "Self_Self", (void *) diag_dnx_oamp_memory_all_type_enum_table},
    {"Num_UPDates", SAL_FIELD_TYPE_INT32, "Number of entries", "10"},
    {"GRouP_Type", SAL_FIELD_TYPE_INT32, "Type of entry", "SHORT", (void *) diag_dnx_oamp_group_all_type_enum_table},
    {"is_RDI", SAL_FIELD_TYPE_BOOL, "RDI test", "NO"},
    {"is_Port_state_UPdate", SAL_FIELD_TYPE_BOOL, "Port state update test", "NO"},
    {"is_InterFace_state_UPdate", SAL_FIELD_TYPE_BOOL, "Interface state update test", "NO"},
    {"with_DMA", SAL_FIELD_TYPE_BOOL, "Use DMA for interrupt", "NO"},
    {"SEMantic_run_ONly", SAL_FIELD_TYPE_INT32, "Configure the mep only, without traffic.", "TRAFFIC",
     (void *) diag_dnx_oamp_traffic_test_type_enum_table},
    {"with_ID", SAL_FIELD_TYPE_INT32, "Configure the mep with ID, choose the starting ID.", "-1"},
    {NULL}
};

/** Test manual   */
sh_sand_man_t diag_dnx_oamp_traffic_test_man = {
    /** Brief */
    "Semantic test of basic OAMP MEP APIs",
    /** Full */
    "Create OAMP MEP entry.  Get entry and compare."
        " Modify some entries.  Get entries and compare."
        " Delete half the entries individually." " Delete the rest of the entries.",
    /** Synopsis   */
    "ctest oamp traffic [Port=<value> Port2=<value> Num_ENTries=<value> Num_UPDates=<value> GRouP_Type=<value> is_RDI=<value> is_Port_state_UPdate=<value> is_InterFace_state_UPdate=<value>] is_DMA=<value> SEMantic_run_ONly=<TRAFFIC/SEMANTIC> with_ID=<value>",
    /** Example   */
    "ctest oamp traffic Port=13 Port2=14 Num_ENTries=1 Num_UPDates=1 GRouP_Type=SHORT MEMory_Type=Self_Self",
};

/** Automatic test list (ctest oamp run)   */
sh_sand_invoke_t diag_dnx_oamp_traffic_tests[] = {
    {"eps_20_oam_update_10", "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=10 GRouP_Type=SHORT MEMory_Type=Self_Self"},
    {"eps_20_oam_update_20_self_offloaded",
     "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=20 GRouP_Type=LONG MEMory_Type=Self_Offloaded"},
    {"48_byte_maid_test", "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=20 GRouP_Type=MAID48 MEMory_Type=Self_Self"},
    {"rdi_test", "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=20 GRouP_Type=SHORT MEMory_Type=Self_Self is_RDI=YES"},
    {"port_state_test",
     "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=20 GRouP_Type=SHORT MEMory_Type=Self_Self is_Port_state_UPdate=YES"},
    {"interface_state_test",
     "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=20 GRouP_Type=SHORT MEMory_Type=Self_Self is_InterFace_state_UPdate=YES"},
    {"scale_test", "Port=13 Port2=14 Num_ENTries=1000 Num_UPDates=0 GRouP_Type=SHORT MEMory_Type=Self_Self"},
    {"scale_test_offloaded",
     "Port=13 Port2=14 Num_ENTries=1900 GRouP_Type=SHORT MEMory_Type=Offloaded_Offloaded SEMantic_run_ONly=1"},
    {"dma_test", "Port=13 Port2=14 Num_ENTries=20 Num_UPDates=10 GRouP_Type=SHORT MEMory_Type=Self_Self with_dma=YES"},
    {NULL}
};
