/** \file ctest_dnx_field_utils.h
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file contains utility functions and structures needed for
 * Field Module ctests.
 *
 */

#ifndef CTEST_DNX_FIELD_UTILS_H_INCLUDED
#define CTEST_DNX_FIELD_UTILS_H_INCLUDED
/*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>

/*
 * DEFINEs
 * {
 */
/*
 * Maximum length of a string in packet_info array (dnx_field_utils_packet_info_t)
 */
#define DNX_FIELD_UTILS_STR_SIZE            64
/*
 * Number of protocols in the packet and relevant values for their
 * fields used as an array size in packet_info array (dnx_field_utils_packet_info_t)
 */
#define DNX_FIELD_UTILS_NOF_PROTOCOLS_IN_PACKET             150

/*
 * Number of qualifiers and actions we support in the basic FG utility structure.
 */
#define CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS       3
#define CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS       3
/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * This Macro is beeing used on cleanup functions
 * If an error ocurred before the cleanup, preserve it.
 * Otherwise if an error ocurred during the running of the cleanup function, return that error.
 */
#define CTEST_DNX_FIELD_UTIL_ERR(_local_func)                       \
{                                                                   \
    int _local_func_rv;                                             \
    _local_func_rv=_local_func;                                     \
    if (_func_rv==_SHR_E_NONE && _local_func_rv != _SHR_E_NONE)     \
    {                                                               \
        SHR_SET_CURRENT_ERR(_local_func_rv);                        \
    }                                                               \
}
/*
 * }
 */

/**
 * This structure holds the information about packet headers.
 */
typedef struct
{
    /*
     * Array of packet headers and relevant values for their fields,
     * which will be parsed to the utility API.
     */
    char header_info[DNX_FIELD_UTILS_NOF_PROTOCOLS_IN_PACKET][DNX_FIELD_UTILS_STR_SIZE];

} dnx_field_utils_packet_info_t;

/**
 * This structure holds the information about signals,
 * which will be verified in the traffic tests.
 */
typedef struct
{
    /*
     * Core ID, used for signal verification, to specify on which core
     * the specific signal should be verified.
     */
    int core;
    /*
     * Name of PP block, one of IRPP, ERPP, ETPP.
     * If NULL all blocks will be searched for match of the given signal.
     */
    char *block;
    /*
     * Name of stage signal comes from, If null any from
     * stages will be searched for match.
     */
    char *from;
    /*
     * Name of stage signal goes to, if null any to stage
     * will be searched for match.
     */
    char *to;
    /*
     * Full or partial name of signal requested.
     */
    char *sig_name;
    /*
     * Size of the expected_value buffer.
     */
    int size;
    /*
     * Expected signal value, should be of type uint32
     */
    uint32 expected_value;

} dnx_field_utils_signal_info_t;

/*
 * The values will be filled for qualifiers.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
typedef struct
{
    uint32 value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
} ctest_dnx_field_util_qual_values;
/*
 * The value will be filled for actions.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
typedef struct
{
    uint32 value[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
} ctest_dnx_field_util_action_values;
/*
 * This structure hold all the required information from the user to create
 * a basic TCAM FG configuration with single entry containing all(3) supplied qualifiers and actions
 * This structure is used by the following functions:
 * ctest_dnx_field_util_basic_tcam_fg_full();
 * ctest_dnx_field_util_basic_tcam_fg_add();
 * ctest_dnx_field_util_basic_context_attach();
 * ctest_dnx_field_util_basic_entry_add();
 *
 */
typedef struct
{
    /*
     * Context for testing - If the field is not explicitly set
     *  by the user, DNX_FIELD_CONTEXT_ID_DEFAULT will be set on init.
     */
    dnx_field_context_t context;
    /*
     * PMF stage identifier
     */
    dnx_field_stage_e field_stage;
    /*
     * BCM Qualifiers for the Field Group(Key)
     */
    bcm_field_qualify_t bcm_quals[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];
    /**
    * Indicate how to read the relevant qualifier from PBUS
    * The information is aligned to the qualifier types array above
    * Number of elements set by qual_types array
    */
    dnx_field_qual_attach_info_t qual_info[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];
    /*
     * BCM Qualifier Values, every qual_values member contains an array of 4 Value fields(accommodating large values)
     */
    ctest_dnx_field_util_qual_values qual_values[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];
    /*
     *BCM  Actions for the Field Group
     */
    bcm_field_action_t bcm_actions[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];
    /*
     * BCM Action Values
     */
    ctest_dnx_field_util_action_values action_values[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

} ctest_dnx_field_util_basic_tcam_fg_t;

/*
 * The values will be filled for qualifiers.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
typedef struct
{
    uint32 value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
} ctest_bcm_field_util_qual_values;
/*
 * The value will be filled for actions.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
typedef struct
{
    uint32 value[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
} ctest_bcm_field_util_action_values;
typedef struct
{
    /**
     * Field Group ID is input and output parameter.
     * If the field is left empty, it will be initialized to -1 at init and used as output
     * for by 'bcm_field_group_create()'.
     */
    bcm_field_group_t fg_id;

    /**
     * Context for testing - If the field is not explicitly set
     *  by the user, DNX_FIELD_CONTEXT_ID_DEFAULT will be set on init.
     */
    bcm_field_context_t context;

    /**
     * PMF stage identifier
     */
    bcm_field_stage_t stage;

    /**
     * BCM Qualifiers for the Field Group(Key)
     */
    bcm_field_qualify_t bcm_qual[CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
   * Indicate how to read the relevant qualifier from PBUS
   * The information is aligned to the qualifier types array above
   * Number of elements set by qual_types array
   */
    bcm_field_qualify_attach_info_t qual_info[CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     * BCM Qualifier Values, every qual_values member contains an array of 4 Value fields(accommodating large values)
     */
    ctest_bcm_field_util_qual_values qual_values[CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     *BCM  Actions for the Field Group .
     */
    bcm_field_action_t bcm_actions[CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     * BCM Action Values
     */
    ctest_bcm_field_util_action_values action_values[CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

} ctest_bcm_field_util_basic_tcam_fg_t;

/**
 * \brief
 *  This function performs full clean-up of a basic PMF configuration.
 *  It detaches all contexts from all field groups.
 *  It deletes all field groups, contexts and user defined qualifiers and actions.
 * \param [in] unit             - Device ID
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_dnx_field_util_clean(
    int unit);

/**
 * \brief
 *  Initializes the ctest_dnx_field_util_basic_tcam_fg_t structure to default valid values.
 *  The structure is initialized and ready for a default valid configuration.
 *  The qualifiers and actions are loaded from arrays at the top of the file.
 *
 * \param [in]  unit                    - Device ID
 * \param [in]  tcam_fg_util_p           - Structure holding all relevant info for creating a full configuration,
 *                                        ready for traffic testing of the set qualifiers and actions supplied.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_dnx_field_util_basic_tcam_fg_t_init(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p);

/**
 * \brief
 *  The function takes a custom ctest structure containing the context and PMF stage of the desired
 *  PMF configuration. It creates a field group in the required stage and attaches it to the supplied(if not Default)
 *  context . The BCM qualifiers and actions are translated to DNX encoded and used throughout the function.
 *  The values supplied are used to configure a TCAM entry containing all supplied qualifiers and actions.
 *
 * \param [in]  unit                    - Device ID
 * \param [in]  tcam_fg_util_p          - Structure holding all relevant info for creating a full configuration,
 *                                        ready for traffic testing of the set qualifiers and actions supplied.
 * \param [out]  fg_id                  - Field Group ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_dnx_field_util_basic_tcam_fg_full(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    dnx_field_group_t * fg_id);

/**
 * \brief
 *  Initializes the ctest_bcm_field_util_basic_tcam_fg_t structure to default valid values.
 *  The structure is initialized and ready for a default valid configuration.
 *  The qualifiers and actions are loaded from arrays at the top of the file.
 *
 * \param [in]  unit                    - Device ID
 * \param [in/out]  tcam_fg_util_p          - Structure holding all relevant info for creating a full configuration,
 *                                        ready for traffic testing of the set qualifiers and actions supplied.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_bcm_field_util_basic_tcam_fg_t_init(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p);

/**
 * \brief
 *  The function takes a custom ctest structure containing the context and PMF stage of the desired
 *  PMF configuration in BCM level. It creates a field group in the required stage and attaches it to the supplied(if not Default)
 *  context. The values supplied are used to configure a TCAM entry containing all supplied qualifiers and actions.
 *
 * \param [in]  unit                    - Device ID
 * \param [in/out]  tcam_fg_util_p      - Structure holding all relevant info for creating a full configuration,
 *                                        ready for traffic testing of the set qualifiers and actions supplied.
 *                                        Returns Context and Field Group IDs if not set explicitly before calling the function.
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_bcm_field_util_basic_tcam_fg_full(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p);

/**
 * \brief
 *  Configuring and sending a single packet.
 * \param [in] unit   - 
 *  Device ID
 * \param [in] src_port -
 *  The source fort to sent the packet from.
 * \param [in] packet_p -
 *  A pointer to one-dimensional array of type "dnx_field_utils_packet_info_t", which contains information about
 *  the packets, which will be transmitted. Each element is related to a different packet.
 *  If we want to send a packet like PTCHoETHoVLANoIPv4 ( and we want to specify
 *  any field of one of the protocols, for example: we want to set VID=15, which is located inside
 *  the VLAN protocol. To change the VID we should use "VLAN.VID=15". If we don't change any field
 *  in the protocols, there are default values for fields, which are taken from "NetworkStructures.xml".
 *  It is mandatory to put an empty string at the end of every packet array!!!
 *  The array should have following structure:
 *
 *      dnx_field_utils_packet_info_t packet_info[nof_packets = 5] =
 *      {
 *          { { "PTCH_2", "ETH", "VLAN", "IPv4", "IPv4.SIP", "0.0.0.2", "IPv4.DIP", "0.0.0.1", "" } },
 *          { { "PTCH_2", "ETH", "VLAN", "VLAN.VID", "15", "IPv4", "" } },
 *          { { "PTCH_2", "ETH", "ETH.DA", "0:0:0:0:0:1", "ETH.SA", "0:0:0:0:1:2", "VLAN", "IPv4", ""} },
 *          { { "PTCH_2", "ETH", "VLAN", "IPv4", "UDP", "UDP.Src_Port", "0x1900", ""} },
 *          { { "PTCH_2", "ETH", "VLAN", "IPv4", "TCP", "TCP.Dst_Port", "0x2100", ""} }
 *      }
 *
 *      How to call the utility API, using the above array:
 *
 *      SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, &(packet_info[packet_index])));
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_dnx_field_utils_packet_tx(
    int unit,
    bcm_port_t src_port,
    dnx_field_utils_packet_info_t * packet_p);

#endif /* CTEST_DNX_FIELD_UTILS_H_INCLUDED */
