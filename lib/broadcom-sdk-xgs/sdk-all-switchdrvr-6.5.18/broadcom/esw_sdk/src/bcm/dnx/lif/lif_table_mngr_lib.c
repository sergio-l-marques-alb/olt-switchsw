/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_table_mngr_lib.c
 *
 *  The LIF Table manager module provides a general mechanism to
 *  address the complexity of In-LIF & Out-LIF allocations with
 *  the appropriate result-type format as well performing the
 *  necessary HW access and Replace operations.
 *  The module is aimed to service APIs that influence the table
 *  result-type, mainly the creation API, both in create mode
 *  and in replace mode. Other APIs that set the LIF Table's
 *  parameters are expected to avoid the LIF Table manager layer
 *  and use direct DBAL APIs, except in cases where those can
 *  alter the result-type. Those special cases require a
 *  retrieval of the LIF Table existing entry prior to applying
 *  the fields of the specific API.
 *  The module's main APIs
 *  (dnx_lif_table_mngr_allocate_local_inlif_and_set_hw &
 *  dnx_lif_table_mngr_allocate_local_outlif_and_set_hw for
 *  In-LIFs & Out-LIFs respectively) receive a DBAL handler for
 *  an already allocated LIF, filled with the table key and the
 *  relevant fields to commit.
 *  Each field that participates in the LIF tables has its own
 *  attributes in the arrays below, specifying how to treat its
 *  value either when given in the DBAL handler or not, as well
 *  as replace attributes.
 *  One of the main features of the module is the result-type
 *  selection. The module traverses the result-type formats from
 *  short to long and finds the first format that matches the
 *  supplied fields in the DBAL handler and according to their
 *  attributes.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnx/dnx_err_recovery_manager_common.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */
/*
 * Shortcuts for DBAL action ENUMS for clarity 
 *  
 *  Set action is the commit expected behavior for fields that were supplied in the handle. 
 *    SET_ACTION_NONE: Setting the field is irrelevant, action ignored 
 *    SET_ACTION_USE_VALUE: The field's value is set to the HW 
 *    SET_ACTION_DO_NOT_USE_VALUE: The field is used for result type selection, but its value
 *                                 isn't set to the HW (Another API is expected to set the value).
 *                                 The user is expected to enter the field's default value.
 *  
 *  Unset action is the commit expected behavior for fields that weren't supplied in the handle.
 *    UNSET_ACTION_NONE: Fields that shouldn't be set by the user such as result_type
 *    UNSET_ACTION_FORBIDDEN: Must be present in the handle, otherwise an error is issued.
 *    UNSET_ACTION_IGNORE: If not supplied, the field isn't set to the HW
 *    UNSET_ACTION_SET_TO_DEFAULT: If not supplied, the field is set to its default value
 */
#define SET_ACTION_NONE                  DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_NONE
#define SET_ACTION_USE_VALUE             DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_USE_VALUE
#define SET_ACTION_DO_NOT_USE_VALUE      DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_DO_NOT_USE_VALUE
#define UNSET_ACTION_NONE                DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_NONE
#define UNSET_ACTION_FORBIDDEN           DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_FORBIDDEN
#define UNSET_ACTION_IGNORE              DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_IGNORE
#define UNSET_ACTION_SET_TO_DEFAULT      DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_SET_TO_DEFAULT
#define NONE                             FALSE

/*
 * Enum definig which type of replace is done on the entry
 */
typedef enum
{
    LIF_TABLE_MNGR_REPLACE_MODE_NO_REPLACE,
    /** Following enum refers to the case where we have the same size and ll indication is valid: either both lifs need ll, don't need ll, or the old lif required ll and the new one doesn't*/
    LIF_TABLE_MNGR_REPLACE_MODE_SAME_LIF_SIZE,
    LIF_TABLE_MNGR_REPLACE_MODE_REUSE_LIF,
    LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF
} lif_table_mngr_replace_mode_e;

/*
 * Struct for default field actions in case of set and replace
 * For more details see documentation of DBAL_TABLE_LIF_TABLE_MANAGER_LIF_TABLE_FIELD_PROPERTIES_DEFAULT
 */
typedef struct
{
    uint32 field_name;
    dbal_enum_value_field_lif_table_manager_set_action_e set_action;
    dbal_enum_value_field_lif_table_manager_unset_action_e unset_action;
    uint8 value_replace_valid;
    uint8 field_existance_replace_valid;
} lif_table_manager_field_actions_default_t;

/*
 * Default field actions in case of set and replace
 */
/* *INDENT-OFF* */
static const lif_table_manager_field_actions_default_t lif_table_manager_field_actions_default[] = {
    /**FIELD_ID,                                      set_action,              unset_action, value_replace_valid, field_existance_replace_valid*/
    /** Result type field is not relevant because is not set by the user */
    {DBAL_FIELD_RESULT_TYPE,                   SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           TRUE,  NONE},
    {DBAL_FIELD_RAW_DATA_ARR,                  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_RAW_DATA,                      SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_DUMMY_FIELD1,                  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_DUMMY_FIELD2,                  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_DUMMY_FIELD3,                  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_HW_ENTRY_TYPE,                 SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_NONE,           NONE,  NONE},
    /** These fields are set by the value given by the user. */
    /** In case user didn't specify value, a default value is used */
    /** This is the default usage */
    {DBAL_FIELD_ENCAP_QOS_MODEL,               SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_FORWARD_QOS_MODEL,             SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_TERMINATION_REMARK_PROFILE,    SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_PROTECTION_POINTER,            SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_PROTECTION_PATH,               SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_IN_LIF_PROFILE,                SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_ERROR_ESTIMATE,                SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** RCH table fields */
    {DBAL_FIELD_FWD_CODE,                      SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_ETHERTYPE,                     SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SYSTEM_PORT_AGGREGATE,         SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_CONTROL_LIF,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_GLOBAL_IN_LIF,                 SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_ZERO_PADDING_RCH,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** End of RCH table fields */
    /** IPFIX_PSAMP table fields */
    {DBAL_FIELD_OBSERVATION_DOMAIN,            SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_TEMPLATE_ID,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** End of ETPS_IPFIX_PSAMP table fields */
    /** ETPS_SRV6 table fields */
    {DBAL_FIELD_ENCAP_1_QOS_MODEL,             SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SRH_ROUTING_TYPE,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SRH_SEGMENT_LEFT,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SRH_LAST_ENTRY,                SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SRH_FLAGS,                     SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SRH_TAG,                       SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_CS_VAR,                        SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** End of ETPS_SRV6 table fields */
    {DBAL_FIELD_TERMINATION_DEPTH,             SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,         SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, SET_ACTION_USE_VALUE,    UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, SET_ACTION_USE_VALUE,UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_PROPAGATION_PROF,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_TERMINATION_TYPE,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_L2_MAC,                        SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_TTL_DECREMENT_DISABLE,         SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_TTL_MODE,                      SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LIF_AHP_UDP,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LIF_AHP_VXLAN_VXLAN_GPE,       SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LIF_AHP_MPLS,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_LIF_AHP_GRE_GENEVE,            SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SERVICE_TYPE,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, FALSE, FALSE},
    {DBAL_FIELD_OUT_LIF_PROFILE,               SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_EGRESS_LLVP_PROFILE,           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_NEXT_OUTLIF_POINTER,           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_FODO_ASSIGNMENT_MODE,          SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_ECN_MAPPING_PROFILE,           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_NEXT_PARSER_CONTEXT,           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, FALSE, NONE},
    {DBAL_FIELD_DUAL_HOMING,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_IPV4_HEADER_DF_FLAG ,          SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_IPV4_HEADER_RESERVED_FLAG,     SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_IPV4_DIP,                      SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_HW_ID,                         SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_ERSPANV2_HEADER_INDEX_17_19,   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_DATA_ENTRY_RESERVED,           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_DIP6,                          SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  FALSE},
    {DBAL_FIELD_SFLOW_DATA_EMPTY,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SID,                           SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_SWITCH_ID,                     SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_NWK_QOS_IDX,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_TTL_PIPE_PROFILE,              SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_EEDB_LL_EXTRA_DATA,            SET_ACTION_USE_VALUE,        UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** These fields are set by the value given by the user. */
    /** In case user didn't specify value, result type will not contain it */
    /** Use in case the existence of the field changes the lif functionality */
    {DBAL_FIELD_SOURCE_IDX,                    SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    {DBAL_FIELD_VSI,                           SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    {DBAL_FIELD_MPLS_LABEL,                    SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    {DBAL_FIELD_MPLS_LABEL_2,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  TRUE},
    {DBAL_FIELD_VRF,                           SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  TRUE},
    {DBAL_FIELD_SUB_TYPE_FORMAT,               SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    {DBAL_FIELD_ECID_OR_PON_TAG,               SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    {DBAL_FIELD_EGRESS_LAST_LAYER,             SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      FALSE, FALSE},
    {DBAL_FIELD_GLOB_IN_LIF,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_FORBIDDEN,      TRUE,  FALSE},
    /** These fields are set by the value given by the user. */
    /** In case user didn't specify value, a default value is used the first time the lif is configured, */
    /** but in case of replace the existing value will be kept. */
    /** Use in case the field can be both set in the lif api and outside (we are trying to avoid such cases as much as possible) */
    {DBAL_FIELD_ESEM_COMMAND,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_IGNORE,         TRUE,  FALSE},
    {DBAL_FIELD_LEARN_ENABLE,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_IGNORE,         TRUE,  TRUE},
    {DBAL_FIELD_REMARK_PROFILE,                SET_ACTION_USE_VALUE,        UNSET_ACTION_IGNORE,         TRUE,  TRUE},
    {DBAL_FIELD_QOS_PROFILE,                   SET_ACTION_USE_VALUE,        UNSET_ACTION_IGNORE,         TRUE,  TRUE},
    /** These fields are NOT set by the value given by the user. */
    /** In case user didn't specify value, a default value is used to reset the field. */
    /** Use in case the field is set by a separate api but indication of existence is given by the lif api */
    {DBAL_FIELD_STAT_OBJECT_ID,                SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    {DBAL_FIELD_STAT_OBJECT_CMD,               SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE,  TRUE},
    /** These fields are NOT set by the value given by the user. */
    /** In case user didn't specify value, a default value is used the first time the lif is configured, */
    /** but in case of replace the existing value will be kept. */
    /** Use in case the field is set by a separate api and lif api is not aware of it */
    {DBAL_FIELD_ACTION_PROFILE_IDX,            SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_ESEM_NAME_SPACE,               SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_GLOB_OUT_LIF,                  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         TRUE,  TRUE},
    {DBAL_FIELD_EEI,                           SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_OAM_LIF_SET,                   SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_DESTINATION,                   SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_VLAN_EDIT_PROFILE,             SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_MTU_PROFILE,                   SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_VLAN_EDIT_VID_1,               SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_ACTION_PROFILE,                SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  FALSE},
    {DBAL_FIELD_VLAN_EDIT_VID_2,               SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_LIF_GENERIC_DATA_0,            SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_LIF_GENERIC_DATA_1,            SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,     SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,         NONE,  TRUE},
    {DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE,     SET_ACTION_USE_VALUE,        UNSET_ACTION_IGNORE,         NONE,  FALSE},
    /** This is a superfield, not used as is */
    {DBAL_FIELD_FODO,                          SET_ACTION_NONE,             UNSET_ACTION_NONE,           NONE,  NONE},
    {DBAL_FIELD_DEFAULT_FODO,                  SET_ACTION_USE_VALUE,        UNSET_ACTION_NONE,           NONE,  NONE},
    /** ETC */
};
/* *INDENT-ON* */

/*
 * Struct for per table field actions in case of set and replace.
 * For more details see documentation of DBAL_TABLE_LIF_TABLE_MANAGER_LIF_TABLE_FIELD_PROPERTIES_PER_TABLE
 */
typedef struct
{
    dbal_tables_e dbal_table_id;
    uint32 field_name;
    dbal_enum_value_field_lif_table_manager_set_action_e set_action;
    dbal_enum_value_field_lif_table_manager_unset_action_e unset_action;
    uint8 value_replace_valid;
    uint8 field_existance_replace_valid;
} lif_table_manager_field_actions_per_table_t;

/*
 * Per table field actions in case of set and replace, in case not defaults.
 * If values are default, they will be taken from lif_table_manager_field_actions_default
 */
/* *INDENT-OFF* */
static const lif_table_manager_field_actions_per_table_t lif_table_manager_field_actions_per_table[] = {
    /**   dbal_table_id,             FIELD_ID,               set_action,              unset_action, value_replace_valid, field_existance_replace_valid*/
    {DBAL_TABLE_EEDB_EVPN,           DBAL_FIELD_OUT_LIF_PROFILE, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE,  FALSE,FALSE},
    {DBAL_TABLE_EEDB_RIF_BASIC,      DBAL_FIELD_STAT_OBJECT_ID,  SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, FALSE},
    {DBAL_TABLE_EEDB_RIF_BASIC,      DBAL_FIELD_STAT_OBJECT_CMD, SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, FALSE},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL,    DBAL_FIELD_VRF,             SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, FALSE},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL,    DBAL_FIELD_STAT_OBJECT_ID,  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL,    DBAL_FIELD_STAT_OBJECT_CMD, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL,    DBAL_FIELD_RAW_DATA,        SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, FALSE},

    {DBAL_TABLE_EEDB_IPV6_TUNNEL,    DBAL_FIELD_VRF,             SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, FALSE},
    {DBAL_TABLE_EEDB_IPV6_TUNNEL,    DBAL_FIELD_STAT_OBJECT_ID,  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_EEDB_IPV6_TUNNEL,    DBAL_FIELD_STAT_OBJECT_CMD, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_EEDB_RSPAN,          DBAL_FIELD_ESEM_NAME_SPACE, SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_ERSPAN,         DBAL_FIELD_RAW_DATA,        SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    
    {DBAL_TABLE_EEDB_ARP,            DBAL_FIELD_STAT_OBJECT_ID, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, NONE,  FALSE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_VRF,             SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT,
                                                                 SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, TRUE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_IN_LIF_PROFILE,  SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, FALSE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_STAT_OBJECT_ID,  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_STAT_OBJECT_CMD, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, TRUE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_PROPAGATION_PROF, SET_ACTION_USE_VALUE,  UNSET_ACTION_IGNORE, TRUE,  TRUE},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_ECN_MAPPING_PROFILE, SET_ACTION_USE_VALUE,  UNSET_ACTION_IGNORE, TRUE,  TRUE},

    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_PROTECTION_POINTER,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,  SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
                                     DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,  SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_EEI,             SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_SERVICE_TYPE,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
                                     DBAL_FIELD_SERVICE_TYPE,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT,
                                     DBAL_FIELD_SERVICE_TYPE,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_VSI,             SET_ACTION_USE_VALUE, UNSET_ACTION_FORBIDDEN,      TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
                                     DBAL_FIELD_VSI,             SET_ACTION_USE_VALUE, UNSET_ACTION_FORBIDDEN,      TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT,
                                     DBAL_FIELD_VSI,             SET_ACTION_USE_VALUE, UNSET_ACTION_FORBIDDEN, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_GLOB_OUT_LIF,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
                                     DBAL_FIELD_GLOB_OUT_LIF,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT,
                                     DBAL_FIELD_GLOB_OUT_LIF,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_IN_AC_INFO_DB,       DBAL_FIELD_DESTINATION,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE,         TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
                                     DBAL_FIELD_DESTINATION,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE,         TRUE, TRUE},
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT,
                                     DBAL_FIELD_DESTINATION,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE,         TRUE, TRUE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_FODO,            SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_FODO_ASSIGNMENT_MODE,  SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, FALSE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_IN_LIF_PROFILE,  SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, FALSE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_STAT_OBJECT_ID,  SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_STAT_OBJECT_CMD, SET_ACTION_DO_NOT_USE_VALUE, UNSET_ACTION_IGNORE, FALSE, FALSE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_PROPAGATION_PROF, SET_ACTION_USE_VALUE,  UNSET_ACTION_IGNORE, TRUE,  TRUE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, SET_ACTION_USE_VALUE,  UNSET_ACTION_IGNORE, TRUE,  TRUE},
    {DBAL_TABLE_IN_LIF_FORMAT_L2TP,  DBAL_FIELD_DESTINATION,     SET_ACTION_USE_VALUE,  UNSET_ACTION_IGNORE, TRUE, TRUE},
    {DBAL_TABLE_EEDB_PPPOE,          DBAL_FIELD_RAW_DATA,        SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_PPPOE,          DBAL_FIELD_NEXT_OUTLIF_POINTER, SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_L2TP,           DBAL_FIELD_RAW_DATA,        SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_L2TP,           DBAL_FIELD_NEXT_OUTLIF_POINTER, SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_DATA_ENTRY,     DBAL_FIELD_LI_CONTENT_ID,   SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_EEDB_DATA_ENTRY,     DBAL_FIELD_RAW_DATA_SIZE,   SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_PROTECTION_POINTER,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,  SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_EEI,             SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_SERVICE_TYPE,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_VSI,             SET_ACTION_USE_VALUE, UNSET_ACTION_FORBIDDEN,      TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_GLOB_OUT_LIF,    SET_ACTION_USE_VALUE, UNSET_ACTION_SET_TO_DEFAULT, TRUE, TRUE},
    {DBAL_TABLE_PON_DTC_CLASSIFICATION, 
                                     DBAL_FIELD_DESTINATION,     SET_ACTION_USE_VALUE, UNSET_ACTION_IGNORE,         TRUE, TRUE},
    /** ETC */
};
/* *INDENT-ON* */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
/*
 * Declaration of extern functions
 * {
 */
extern shr_error_e dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id);

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief - Return for each table and field what action to perform in case of setting & unsetting the field.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] field_id - relevant field id
 *   \param [out] field_set_action - action to perform if field is set
 *   \param [out] field_unset_action - action to perform if field is unset
 *   \param [out] field_value_replace_valid - can value of this field be replaced?
 *   \param [out] field_existance_replace_valid - can this field be replaced from valid to invalid?
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_field_actions_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 field_id,
    dbal_enum_value_field_lif_table_manager_set_action_e * field_set_action,
    dbal_enum_value_field_lif_table_manager_unset_action_e * field_unset_action,
    uint8 *field_value_replace_valid,
    uint8 *field_existance_replace_valid)
{
    uint32 field_index;
    uint32 nof_entries_per_table =
        sizeof(lif_table_manager_field_actions_per_table) / sizeof(lif_table_manager_field_actions_per_table_t);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Find the right entry in the actions array
     */
    for (field_index = 0; field_index < nof_entries_per_table; field_index++)
    {
        if ((lif_table_manager_field_actions_per_table[field_index].field_name == field_id) &&
            (lif_table_manager_field_actions_per_table[field_index].dbal_table_id == dbal_table_id))
        {
            /*
             * Found the right entry, read actions
             */
            if (field_set_action)
            {
                *field_set_action = lif_table_manager_field_actions_per_table[field_index].set_action;
            }
            if (field_unset_action)
            {
                *field_unset_action = lif_table_manager_field_actions_per_table[field_index].unset_action;
            }
            if (field_value_replace_valid)
            {
                *field_value_replace_valid = lif_table_manager_field_actions_per_table[field_index].value_replace_valid;
            }
            if (field_existance_replace_valid)
            {
                *field_existance_replace_valid =
                    lif_table_manager_field_actions_per_table[field_index].field_existance_replace_valid;
            }
            break;
        }
    }
    if (field_index == nof_entries_per_table)
    {
        /*
         * No relevant tries in the per table map, look in the default array
         */
        uint32 nof_entries =
            sizeof(lif_table_manager_field_actions_default) / sizeof(lif_table_manager_field_actions_default_t);
        /*
         * Find the right entry in the actions array
         */
        for (field_index = 0; field_index < nof_entries; field_index++)
        {
            if (lif_table_manager_field_actions_default[field_index].field_name == field_id)
            {
                /*
                 * Found the right entry, read actions
                 */
                if (field_set_action)
                {
                    *field_set_action = lif_table_manager_field_actions_default[field_index].set_action;
                }
                if (field_unset_action)
                {
                    *field_unset_action = lif_table_manager_field_actions_default[field_index].unset_action;
                }
                if (field_value_replace_valid)
                {
                    *field_value_replace_valid =
                        lif_table_manager_field_actions_default[field_index].value_replace_valid;
                }
                if (field_existance_replace_valid)
                {
                    *field_existance_replace_valid =
                        lif_table_manager_field_actions_default[field_index].field_existance_replace_valid;
                }
                break;
            }
        }
        if (field_index == nof_entries)
        {
            /*
             * Field wasn't found in any of the tables
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't find actions for field: %s", dbal_field_to_string(unit, field_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return for each table and field what action to perform in case of setting & unsetting the field.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] field_id - relevant field id
 *   \param [out] field_set_action - action to perform if field is set
 *   \param [out] field_unset_action - action to perform if field is unset
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
shr_error_e
dnx_lif_table_mngr_field_set_actions_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 field_id,
    dbal_enum_value_field_lif_table_manager_set_action_e * field_set_action,
    dbal_enum_value_field_lif_table_manager_unset_action_e * field_unset_action)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Find the set actions in the actions array
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_actions_get
                    (unit, dbal_table_id, field_id, field_set_action, field_unset_action, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - For each table and field return if replacement is valid
 * set/unset/replace
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] field_id - relevant field id
 *   \param [out] field_value_replace_valid - can value of this field be replaced?
 *   \param [out] field_existance_replace_valid - can this field be replaced from valid to invalid?
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_field_replace_actions_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 field_id,
    uint8 *field_value_replace_valid,
    uint8 *field_existance_replace_valid)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Find the replace actions in the actions array
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_actions_get
                    (unit, dbal_table_id, field_id, NULL, NULL, field_value_replace_valid,
                     field_existance_replace_valid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init forbidden actions per table SW state.
 * This table is used by algo lif table manager to when deciding on result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_forbidden_fields_per_table_init(
    int unit)
{
    dbal_physical_tables_e physical_table_id;
    uint32 dbal_table_id;
    CONST dbal_logical_table_t *table;
    uint32 *field_forbidden_ids_bitmap_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC(field_forbidden_ids_bitmap_p, sizeof(*field_forbidden_ids_bitmap_p) * _SHR_BITDCLSIZE
              (dnx_data_lif.lif_table_manager.max_fields_per_table_get(unit)),
              "field_forbidden_ids_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Loop on all DBAL tables
     */
    for (dbal_table_id = 1; dbal_table_id < DBAL_NOF_TABLES; dbal_table_id++)
    {
        /** Get table information */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (table->nof_physical_tables > 0)
        {
            uint32 forbidden_fields_per_table_handle_id;

            sal_memset(field_forbidden_ids_bitmap_p, 0,
                       sizeof(*field_forbidden_ids_bitmap_p) *
                       _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_table_get(unit)));

            /** Get table info */
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

            /*
             * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
             */
            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                           &physical_table_id));

            /*
             * If the table is LIF table (in LIF or EEDB), add it into the DBs
             */
            if (physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_1 || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_2
                || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_3 || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_4
                || physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1 || physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)

            {
                int field_idx;
                /** Loop on all the fields and fill the table */

                for (field_idx = 0; field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields;
                     field_idx++)
                {
                    dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
                    dbal_enum_value_field_lif_table_manager_unset_action_e field_unset_action;
                    dbal_fields_e field_id =
                        table->multi_res_info[table->nof_result_types].results_info[field_idx].field_id;

                    /** Get instructions on what to do with this field */
                    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                                    (unit, dbal_table_id, field_id, &field_set_action, &field_unset_action));

                    if (field_unset_action == UNSET_ACTION_FORBIDDEN)
                    {
                        /*
                         * Action is forbidden, set in the corresponding field index in the bitmap
                         */
                        SHR_BITSET(field_forbidden_ids_bitmap_p, field_idx);
                    }
                }
                /*
                 * Write FORBIDDEN action fields bitmap per dbal table
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_FORDIDDEN_FIELDS_PER_TABLE,
                                                  &forbidden_fields_per_table_handle_id));

                dbal_entry_key_field32_set(unit, forbidden_fields_per_table_handle_id, DBAL_FIELD_DBAL_TABLE,
                                           dbal_table_id);
                dbal_entry_value_field_arr32_set(unit, forbidden_fields_per_table_handle_id,
                                                 DBAL_FIELD_FORBIDDEN_FIELDS_BMP, INST_SINGLE,
                                                 field_forbidden_ids_bitmap_p);

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, forbidden_fields_per_table_handle_id, DBAL_COMMIT));
                DBAL_HANDLE_FREE(unit, forbidden_fields_per_table_handle_id);
            }
        }
    }

exit:
    SHR_FREE(field_forbidden_ids_bitmap_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init per lif table forbidden action fields
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_forbidden_fields_per_table_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility to set lif table keys according to ingress/egress and dbal table id indications
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] result_type - result type, if -1 ignore
 *   \param [in] local_lif_id - local lif id key
 *   \param [in] lif_table_entry_handle_id - handle id to set key on
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_lif_table_key_fields(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    int local_lif_id,
    uint32 lif_table_entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
        {
            dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_OUT_RIF, local_lif_id);
        }
        else
        {
            dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif_id);
        }
    }

    if (result_type != -1)
    {
        dbal_entry_value_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
/**
 * \brief - this API is used to update the rollback / comparison journals.
 * HW LIF tables have special case that the entries in this table depends on LIF SW table. The result type of the entry
 * is saved in SW table, so it means that the tables should be syncronized. When Lif table manager update an entry both
 * tables become out of sync. In this case DBAL doesn't know how to get the entry (size of entry is out of sync)
 *
 * For this reason when working with error recovery, the DBAL cannot perform get and cannot keep the entry in the journal,
 * so journal is updated outside of DBAL.
 */

static shr_error_e
dnx_lif_table_mngr_journal_update(
    int unit,
    int local_lif_id,
    int is_inlif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    uint8 is_new_local_lif)
{
    /** in case error recovery enabled, need to perform get to the entry and update the journal. if there is no entry in
     *  HW, need to update the journal with delete entry */
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    dbal_entry_handle_t *get_handle_ptr = NULL;
    dbal_tables_e dbal_table_id;
    uint32 existing_result_type;
    uint32 lif_table_entry_handle_id;
    int outlif_phase_dummy;
    int rv = 0;
    uint8 is_region_end_required = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
    {
        /*
         * error recovery is disabled, no need to proceed
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_dbal_access_region_start(unit));
    is_region_end_required = TRUE;

    if (!is_inlif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif_id,
                                                        &dbal_table_id,
                                                        &existing_result_type, &outlif_phase_dummy, NULL, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id, &existing_result_type, NULL));
    }

    /*
     * check if DBAL entry exists and journal accordingly
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /*
     * set key of handle before getting
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_inlif, dbal_table_id, existing_result_type, local_lif_id,
                     lif_table_entry_handle_id));

    /*
     * if we don't find any dbal entry we journal CLEAR, otherwise journal ADD
     */
    rv = dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS);
    dbal_entry_handle_get_internal(unit, lif_table_entry_handle_id, &entry_handle_ptr);

    if (rv != _SHR_E_NOT_FOUND && !is_new_local_lif)
    {
        get_handle_ptr = entry_handle_ptr;
    }

    dnx_dbal_journal_log_add(unit, get_handle_ptr, entry_handle_ptr, 0);

exit:
    if (is_region_end_required)
    {
        dnx_err_recovery_common_dbal_access_region_end(unit);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

/**
 * \brief - Add new entry into lif table, copy contents from existing lif
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] src_result_type - result type of the original entry
 *   \param [in] src_local_lif_id - local lif to copy from
 *   \param [in] dst_local_lif_id - new local lif to create, copy contents of src_local_lif_id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_copy_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 src_result_type,
    int src_local_lif_id,
    int dst_local_lif_id)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /*
     * Get source lif entry
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, src_result_type, src_local_lif_id,
                     lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Change to new key
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, -1, dst_local_lif_id, lif_table_entry_handle_id));
    /*
     * Update fields and write back to lif table
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, lif_table_entry_handle_id));
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace the result type of a local inlif
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] new_result_type - Chosen result type
 *   \param [in] existing_result_type - Current result type in case of replace
 *   \param [in] local_lif_id - Allocated local lif id
 *   \param [in] entry_reuse - Indication in case of reusing the same local lif for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_replace_result_type_inlif(
    int unit,
    int core_id,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 new_result_type,
    uint32 existing_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.core_id = core_id;
    inlif_info.local_inlif = local_lif_id;
    inlif_info.fixed_entry_size = lif_info->entry_size_to_allocate;
    /*
     * Replace existing result type with new one
     */
    inlif_info.old_result_type = existing_result_type;
    inlif_info.dbal_result_type = new_result_type;
    if (!entry_reuse)
    {
        /*
         * If can't reuse entry, then a new one was reserved
         */
        inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE;
    }
    inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace the result type of a local outlif
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] new_result_type - Chosen result type
 *   \param [in] existing_result_type - Current result type in case of replace
 *   \param [in] local_lif_id - Allocated local lif id
 *   \param [in] entry_reuse - Indication in case of reusing the same local lif for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_replace_result_type_outlif(
    int unit,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_outlif_info_t * lif_info,
    uint32 new_result_type,
    uint32 existing_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * RIF has special allocation procedure
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_lif_table_mngr_replace_result_type_outlif cannot be called with RIF entry");
    }
    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = dbal_table_id;
    outlif_info.outlif_phase = lif_info->outlif_phase;
    outlif_info.local_outlif = local_lif_id;
    outlif_info.fixed_entry_size = lif_info->entry_size_to_allocate;
    /*
     * Replace existing result type with new one
     */
    outlif_info.alternative_result_type = existing_result_type;
    outlif_info.dbal_result_type = new_result_type;

    if (!entry_reuse)
    {
        /*
         * If can't reuse entry, then a new one was reserved
         */
        outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE;
    }
    /*
     * Now replace entry.
     */
    outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new entry into lif table, according to given handle id fields
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] current_result_type - existing entry result type, relevant if entry_reuse is 1
 *   \param [in] local_lif_id - new entry local lif
 *   \param [in] entry_reuse - reuse existing entry for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    void *lif_info,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    uint32 current_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    uint32 lif_table_entry_handle_id;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, entry_reuse ? current_result_type : result_type,
                     local_lif_id, lif_table_entry_handle_id));

    if (entry_reuse)
    {
        /*
         * If replacing result type on same entry get the entry first
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));
        /*
         * In case of a different result type - need to change the current one on the dbal handle
         */
        if (result_type != current_result_type)
        {
            SHR_IF_ERR_EXIT(dbal_entry_result_type_update
                            (unit, lif_table_entry_handle_id, result_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE));
        }
        if (is_ingress)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_inlif
                            (unit, core_id, dbal_table_id, (lif_table_mngr_inlif_info_t *) lif_info, result_type,
                             current_result_type, local_lif_id, entry_reuse));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_outlif
                            (unit, dbal_table_id, (lif_table_mngr_outlif_info_t *) lif_info,
                             result_type, current_result_type, local_lif_id, entry_reuse));
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all the fields and fill the table */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
        dbal_enum_value_field_lif_table_manager_unset_action_e field_unset_action;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        /** Get instructions on what to do with this field */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                        (unit, dbal_table_id, field_id, &field_set_action, &field_unset_action));

        /** Check if field exists on entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** Field exists: Check if action is to copy the field value from the superset handle. Otherwise ignore */
            if (field_set_action == SET_ACTION_USE_VALUE)
            {
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
        else
        {
            /** Field does not exist: Check the action. If forbidden return error. */
            /** Else set to default or ignore according to action */
            if (field_unset_action == UNSET_ACTION_FORBIDDEN)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Forbidden field found on result type. field: %s",
                             dbal_field_to_string(unit, field_id));
            }
            if (field_unset_action == UNSET_ACTION_SET_TO_DEFAULT)
            {
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
            }
        }

    }

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace existing entry in lif table with new result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] existing_result_type - result type of the current entry
 *   \param [in] new_result_type - new entry result type
 *   \param [in] local_lif_id - new entry local lif
 *   \param [in] entry_reuse - reuse existing entry for new result type

 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_hw_replace_result_type(
    int unit,
    int core_id,
    uint8 is_ingress,
    void *lif_info,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 existing_result_type,
    uint32 new_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    uint32 lif_table_entry_handle_id_existing, lif_table_entry_handle_id_new;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** We need two handles - one to read existing values and one to fill new entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_existing));
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_new));
    /** Set keys for the two handles */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, existing_result_type, local_lif_id,
                     lif_table_entry_handle_id_existing));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, new_result_type, local_lif_id,
                     lif_table_entry_handle_id_new));

    /*
     * In case we replace result type of an entry we must fill all the fields.
     * We will copy all fields that are mutual for the two result types and fill with defaults all the other values.
     */
    /** Get all fields from existing entry */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id_existing, DBAL_GET_ALL_FIELDS));
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_inlif
                        (unit, core_id, dbal_table_id, (lif_table_mngr_inlif_info_t *) lif_info, new_result_type,
                         existing_result_type, local_lif_id, entry_reuse));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_outlif
                        (unit, dbal_table_id, (lif_table_mngr_outlif_info_t *) lif_info,
                         new_result_type, existing_result_type, local_lif_id, entry_reuse));
    }

    /*
     * For each field in the new result type:
     *    1. If input entry_handle_id should set it: write new value according to user input
     *    2. else if should be rewritten by default: write default value
     *    3. If neither done:
     *       3.1 If present in the old result type: fill it from there
     *       3.2 else fill default
     */
    for (field_idx = 0; field_idx < table->multi_res_info[new_result_type].nof_result_fields; field_idx++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint8 is_field_set = FALSE;

        dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
        dbal_enum_value_field_lif_table_manager_unset_action_e field_unset_action;
        dbal_fields_e field_id = table->multi_res_info[new_result_type].results_info[field_idx].field_id;
        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            /** Only field that should be skipped is result type */
            continue;
        }

        /** Get instructions on what to do with this field */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                        (unit, dbal_table_id, field_id, &field_set_action, &field_unset_action));

        /** Check if field exists on entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** Field exists: Check if action is to copy the field value from the superset handle. Otherwise ignore */
            if (field_set_action == SET_ACTION_USE_VALUE)
            {
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE, field_val);
                is_field_set = TRUE;
            }
        }
        else
        {
            /** Field does not exist: Check the action. If forbidden return error. */
            /** Else set to default or ignore according to action */
            if (field_unset_action == UNSET_ACTION_FORBIDDEN)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Forbidden field found on result type");
            }
            if (field_unset_action == UNSET_ACTION_SET_TO_DEFAULT)
            {
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
                is_field_set = TRUE;

            }
        }
        /** Field that were not set must get value - either from old table or default */
        if (!is_field_set)
        {
            /** Check if field exists on old hw entry */
            rv = dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id_existing, field_id,
                                                         INST_SINGLE, field_val);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                /** Copy old HW value to new entry */
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE, field_val);
            }
            else
            {
                /** Set value from default */
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
            }
        }
    }
    /** Now we have all the values in the new entry set and we can write new entry to HW */
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit
                                               (unit, lif_table_entry_handle_id_new,
                                                DBAL_COMMIT | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get dbal handle id fields and table specific info from HW lif table entry
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] result_type - new entry result type
 *   \param [in] outlif_phase - Lif mngr outlif phase of the current lif, invalid for inlif
 *   \param [in] lif_table_entry_handle_id - DBAL entry handle of the hw lif table
 *   \param [in] field_ids_bitmap - bitmap containing sw state result for valid fields in this entry
 *   \param [out] entry_handle_id - DBAL entry handle. The entry will be set to type DBAL_SUPERSET_RESULT_TYPE
 *                  and contain all the fields that are set in HW
 *   \param [out] table_specific_flags - additional information per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_get_table_fields_and_specific_flags(
    int unit,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    uint32 outlif_phase,
    uint32 lif_table_entry_handle_id,
    uint32 *field_ids_bitmap,
    uint32 entry_handle_id,
    uint32 *table_specific_flags)
{
    int field_idx;
    CONST dbal_logical_table_t *table;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(field_ids_bitmap, _SHR_E_INTERNAL, "field_ids_bitmap");
    SHR_NULL_CHECK(table_specific_flags, _SHR_E_INTERNAL, "table_specific_flags");

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all the fields and fill lif info */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id != DBAL_FIELD_RESULT_TYPE)
        {
            /** Check in SW state if this field is enabled */
            if (SHR_BITGET(field_ids_bitmap, field_idx))
            {
                /*
                 * Field enabled - copy value from HW to the superset handle
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id, field_id,
                                                                        INST_SINGLE, field_val));
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill
                    (unit, dbal_table_id, result_type, outlif_phase, table_specific_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_get_outlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    uint32 lif_table_entry_handle_id, sw_field_indicator_entry_handle_id;
    uint32 result_type;
    dbal_tables_e dbal_table_id;
    uint32 *field_ids_bitmap_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    SHR_ALLOC(field_ids_bitmap_p,
              sizeof(*field_ids_bitmap_p) *
              _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit)),
              "field_ids_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    sal_memset(lif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif_id,
                                                    &dbal_table_id,
                                                    &result_type, &lif_info->outlif_phase,
                                                    &lif_info->entry_size_to_allocate, NULL));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** This SW table stores the valid fields for each lif */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_table_fields_and_specific_flags
                    (unit, local_lif_id, dbal_table_id, result_type, lif_info->outlif_phase, lif_table_entry_handle_id,
                     field_ids_bitmap_p, entry_handle_id, &(lif_info->table_specific_flags)));

exit:
    SHR_FREE(field_ids_bitmap_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_get_inlif_info(
    int unit,
    int core_id,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    uint32 lif_table_entry_handle_id, sw_field_indicator_entry_handle_id;
    uint32 result_type;
    dbal_tables_e dbal_table_id;
    uint32 *field_ids_bitmap_p = NULL;

    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    SHR_ALLOC(field_ids_bitmap_p,
              sizeof(*field_ids_bitmap_p) *
              _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit)),
              "field_ids_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    sal_memset(lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    /*
     * Take DBAL table ID from handle and get the physical table id from table id
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                    (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id, &result_type,
                     &lif_info->entry_size_to_allocate));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, TRUE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** This SW table stores the valid fields for each lif */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_SBC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_DPC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_table_fields_and_specific_flags
                    (unit, local_lif_id, dbal_table_id, result_type, -1, lif_table_entry_handle_id,
                     field_ids_bitmap_p, entry_handle_id, &(lif_info->table_specific_flags)));

exit:
    SHR_FREE(field_ids_bitmap_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Clear HW Inlif table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - relevant for inlif tables
 *   \param [in] is_ingress - set if ingress dbal table is to be cleared
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] local_lif_id - Allocated local lif id
 *   \param [in] result_type - Chosen result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_lif_clear_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate local lif according to table id and result type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] result_type - Chosen result type
 *   \param [in] existing_result_type - Current result type in case of replace
 *   \param [in] is_next_pointer_valid - Indication that next pointer field was requested
 *   \param [out] local_lif_id - Allocated local lif id
 *   \param [out] entry_reuse - Indication in case of replace if same local lif can be used
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_allocate_outlif(
    int unit,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_outlif_info_t * lif_info,
    uint32 result_type,
    uint32 existing_result_type,
    uint8 is_next_pointer_valid,
    int *local_lif_id,
    int *entry_reuse)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * RIF has special allocation procedure
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_rif_allocate(unit, *local_lif_id, result_type, lif_info->entry_size_to_allocate));
    }
    else
    {
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
        outlif_info.dbal_table_id = dbal_table_id;
        outlif_info.outlif_phase = lif_info->outlif_phase;
        outlif_info.dbal_result_type = result_type;
        outlif_info.fixed_entry_size = lif_info->entry_size_to_allocate;
        if (is_next_pointer_valid)
        {
            outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST;
        }
        else
        {
            outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST;
        }
        if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
        {
            /*
             * New local Lif
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

            *local_lif_id = outlif_info.local_outlif;
        }
        else
        {
            /*
             * Replace existing result type with new one
             */
            outlif_info.alternative_result_type = existing_result_type;
            outlif_info.local_outlif = *local_lif_id;
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_can_be_reused(unit, &outlif_info, entry_reuse));
            /*
             * If can reuse entry no need to allocate new entry,
             * Else reserve a new one.
             */
            if (*entry_reuse == FALSE)
            {
                outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE;
                SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

                *local_lif_id = outlif_info.local_outlif;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate local lif according to table id and result type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] result_type - Chosen result type
 *   \param [in] existing_result_type - Current result type in case of replace
 *   \param [out] local_lif_id - Allocated local lif id
 *   \param [out] entry_reuse - Indication in case of replace if same local lif can be used
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_allocate_inlif(
    int unit,
    int core_id,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 result_type,
    uint32 existing_result_type,
    int *local_lif_id,
    int *entry_reuse)
{
    lif_mngr_local_inlif_info_t inlif_info;
    int smaller_outlif;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.core_id = core_id;
    inlif_info.dbal_result_type = result_type;
    inlif_info.fixed_entry_size = lif_info->entry_size_to_allocate;

    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        /*
         * New local Lif
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));

        *local_lif_id = inlif_info.local_inlif;
    }
    else
    {
        /*
         * Replace existing result type with new one
         */
        inlif_info.old_result_type = existing_result_type;
        inlif_info.local_inlif = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_can_be_reused(unit, &inlif_info, entry_reuse, &smaller_outlif));
        /*
         * If can reuse entry no need to allocate new entry,
         * Else reserve a new one.
         */
        if (*entry_reuse == FALSE)
        {
            inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE;
            if (smaller_outlif)
            {
                inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER;
            }
            SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));

            *local_lif_id = inlif_info.local_inlif;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SW state table that contains information
 *   on the valid fields for each lif.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_lif_table_mngr_valid_fields_clear_by_out_lif(
    int unit,
    int local_lif_id)
{
    uint32 sw_field_indicator_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    /*
     * Clear entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SW state table that contains information
 *   on the valid fields for each lif.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] core_id - core id
 *   \param [in] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_lif_table_mngr_valid_fields_clear_by_in_lif(
    int unit,
    dbal_physical_tables_e physical_table_id,
    int core_id,
    int local_lif_id)
{
    uint32 sw_field_indicator_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get handle
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_SBC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_DPC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    /*
     * Clear entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill SW state table that contains information
 *   on the valid fields for each lif.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] is_replace - indication to replace existing entry
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_store_valid_fields(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    dbal_physical_tables_e physical_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    int local_lif_id,
    uint8 is_replace)
{
    int field_idx, sub_field_idx, superset_result_type_idx;
    CONST dbal_logical_table_t *table;
    uint32 sw_field_indicator_entry_handle_id;
    uint32 *field_ids_bitmap_p = NULL;
    shr_error_e rv;
    dbal_table_field_info_t table_field_info;
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(field_ids_bitmap_p,
                       sizeof(*field_ids_bitmap_p) *
                       _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit)),
                       "field_ids_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Add only relevant entries
     */
    if (is_ingress == FALSE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_SBC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_DPC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** Get table info */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** This field is required add to SW state */
            SHR_BITSET(field_ids_bitmap_p, field_idx);
        }
        else
        {
            /** This field is not required */
            SHR_BITCLR(field_ids_bitmap_p, field_idx);
        }

        /**field not found. Check if it has child-fields, and see if they were set on the handle*/
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table_id, field_id, FALSE,
                                                       result_type, 0, &table_field_info));

            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));
            /**Iterate on child fields*/
            if (field_info->nof_child_fields != 0)
            {
                for (sub_field_idx = 0; sub_field_idx < field_info->nof_child_fields; sub_field_idx++)
                {
                    /** Check if the field is required, if it was set in the superset result type.. */
                    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                 field_info->sub_field_info[sub_field_idx].sub_field_id,
                                                                 INST_SINGLE, field_val);
                    /*
                     * if the field is found, need to find the index in the bitmap according to the superset 
                     */
                    if (rv == _SHR_E_NONE)
                    {
                        for (superset_result_type_idx = 0;
                             superset_result_type_idx <
                             table->multi_res_info[table->nof_result_types].nof_result_fields;
                             superset_result_type_idx++)
                        {
                            field_id =
                                table->multi_res_info[table->nof_result_types].
                                results_info[superset_result_type_idx].field_id;
                            if (field_id == field_info->sub_field_info[sub_field_idx].sub_field_id)
                            {
                                /** This field is required add to SW state */
                                SHR_BITSET(field_ids_bitmap_p, superset_result_type_idx);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    dbal_entry_value_field_arr32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                     field_ids_bitmap_p);
    /** Valid for both new entry and update, so existing entry should be overwritten */
    if (is_replace)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));
    }

exit:
    SHR_FREE(field_ids_bitmap_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse table and change all pointer to given lif to the new value. 
 *  Skip tables that aren't LIF tables
 *  Used by dnx_lif_table_mngr_update_isem_pointers,
 *          dnx_lif_table_mngr_update_tcam_pointers,
 *          dnx_lif_table_mngr_update_eedb_next_pointer
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_id - table to traverse
 *   \param [in] is_ingress - ingress indication
 *   \param [in] is_default - default lif indication
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_pointers_single_table(
    int unit,
    dbal_tables_e table_id,
    uint8 is_ingress,
    uint8 is_default,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    uint32 entry_handle_id;
    uint32 field_id, result_type_idx;
    CONST dbal_logical_table_t *table_attr;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    field_id = is_default ? DBAL_FIELD_DEFAULT_LIF : is_ingress ? DBAL_FIELD_IN_LIF : DBAL_FIELD_NEXT_OUTLIF_POINTER;

    /*
     * Verify It's a LIF Table by checking that the expected field exists in the table.
     * Check all the result type until the field is found. 
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table_attr));
    for (result_type_idx = 0; result_type_idx < table_attr->nof_result_types; result_type_idx++)
    {
        if (dbal_tables_field_info_get_no_err(unit, table_id, field_id, 0, result_type_idx, 0, &field_info) ==
            _SHR_E_NONE)
        {
            break;
        }
    }

    /*
     * If the expected LIF field wasn't found in any of the result types, skip the table
     */
    if (result_type_idx == table_attr->nof_result_types)
    {
        SHR_EXIT();
    }

    /*
     * Traverse by rule over the current table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, field_id, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &old_local_lif_id, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                    (unit, entry_handle_id, DBAL_ITER_ACTION_UPDATE, field_id, INST_SINGLE, &new_local_lif_id, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse ISEM and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_isem_pointers(
    int unit,
    dbal_physical_tables_e physical_table_id,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    int rv;
    dbal_tables_e table_id;
    dbal_physical_tables_e match_physical_table_id;
    uint32 entry_handle_id;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    /*
     * Traverse the ISEM table, move all entries from old lif to new lif
     */
    match_physical_table_id =
        (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1) ? DBAL_PHYSICAL_TABLE_ISEM_1 : DBAL_PHYSICAL_TABLE_ISEM_2;

    /*
     * Special processing for the PON DTC ISEM1 -> INLIF2 
     */
    if (physical_table_id != DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, old_local_lif_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
            if ((result_type ==
                 DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED)
                || (result_type ==
                    DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED)
                || (result_type == DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN)
                || (result_type ==
                    DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN))
            {
                match_physical_table_id = DBAL_PHYSICAL_TABLE_ISEM_1;
            }
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, match_physical_table_id,
                     DBAL_TABLE_TYPE_EM, &table_id));
    while (table_id != DBAL_TABLE_EMPTY)
    {
        /*
         * Traverse by rule over the current table
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                        (unit, table_id, TRUE, FALSE, old_local_lif_id, new_local_lif_id));

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, match_physical_table_id,
                         DBAL_TABLE_TYPE_EM, &table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse TCAM and change all pointers to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_tcam_pointers(
    int unit,
    dbal_physical_tables_e physical_table_id,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    int table_idx;
    
    int table_id_tunnels_nof = 4;
    dbal_tables_e table_id_tunnels[] =
        { DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS, DBAL_TABLE_IPV4_TT_TCAM_BASIC, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS,
        DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
    };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Traverse the TCAM table, move all entries from old lif to new lif
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        /*
         * For inlif1 traverse only AC TCAM
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                        (unit, DBAL_TABLE_IN_AC_TCAM_DB, TRUE, FALSE, old_local_lif_id, new_local_lif_id));
    }
    else
    {
        /*
         * For inlif2 traverse all tunnel TCAMs
         */
        for (table_idx = 0; table_idx < table_id_tunnels_nof; table_idx++)
        {
            /*
             * Traverse by rule over the current table
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                            (unit, table_id_tunnels[table_idx], TRUE, FALSE, old_local_lif_id, new_local_lif_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse DEFAULT_IN_LIF table and change all pointers to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_default_lif_pointers(
    int unit,
    dbal_physical_tables_e physical_table_id,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only relevant when lif is AC
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        /*
         * Traverse the DEFAULT_IN_LIF table table, move all entries from old lif to new lif
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                        (unit, DBAL_TABLE_INGRESS_PP_PORT, TRUE, TRUE, old_local_lif_id, new_local_lif_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Traverse ISEM and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] physical_table_id - physical table id (inlif1/2/3)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_termination_match_information(
    int unit,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    int old_local_lif_id,
    int new_local_lif_id)
{
    uint32 entry_handle_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Check according to physical table which SW state to access DPC or SBC
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, old_local_lif_id);
    /*
     * Not all lif store match information, replace only if exists
     */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NONE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, new_local_lif_id);
        SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, old_local_lif_id);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse EEDB and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_eedb_next_pointer(
    int unit,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Traverse the EEDB table, move all pointers from old lif to new lif
     */
    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_EEDB, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_DIRECT, &table_id));
    while (table_id != DBAL_TABLE_EMPTY)
    {
        /*
         * Traverse by rule over the current table
         */

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                        (unit, table_id, FALSE, FALSE, old_local_lif_id, new_local_lif_id));

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_EEDB, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_DIRECT, &table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify replace is valid by checking each field on the handle
 *  and comparing to existing entry
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - new handle id provided by the user
 *   \param [in] entry_handle_id_existing - handle id to the lif table manager struct existing in the HW
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_replace_validity_verify(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_handle_id_existing)
{
    dbal_tables_e dbal_table_id;
    int field_idx;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /*
     * Verify replace is valid by checking each field on the handle
     */
    for (field_idx = 0; field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields; field_idx++)
    {
        uint8 field_value_replace_valid, field_existance_replace_valid;
        dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
        dbal_enum_value_field_lif_table_manager_unset_action_e field_unset_action;
        uint8 is_field_exist_on_current, is_field_exist_on_new;
        dbal_fields_e field_id = table->multi_res_info[table->nof_result_types].results_info[field_idx].field_id;
        /*
         * First check that field replacement is valid.
         * Also check if value of the field is interesting otherwise skip the check
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_replace_actions_get
                        (unit, dbal_table_id, field_id, &field_value_replace_valid, &field_existance_replace_valid));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                        (unit, dbal_table_id, field_id, &field_set_action, &field_unset_action));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id_existing, field_id, &is_field_exist_on_current));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, field_id, &is_field_exist_on_new));

        if (!field_existance_replace_valid)
        {
            /*
             * Existence replace is invalid, verify that user is not trying to replace the existence of this field
             */
            if ((!is_field_exist_on_current && is_field_exist_on_new)
                || (is_field_exist_on_current && !is_field_exist_on_new))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Field %s cannot be replaced from valid to invalid or vice versa\n",
                             dbal_field_to_string(unit, field_id));
            }
        }
        if (!field_value_replace_valid && (field_set_action == DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_USE_VALUE))
        {
            /*
             * Value replace is invalid, verify that user is not trying to replace the value of this field
             */
            if (is_field_exist_on_current && is_field_exist_on_new)
            {
                uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                uint32 field_val_existing[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                uint8 is_equal;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id_existing, field_id, INST_SINGLE, field_val_existing));

                utilex_bitstream_compare(field_val, field_val_existing, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS, &is_equal);
                if (!is_equal)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Field value %s cannot be replaced\n", dbal_field_to_string(unit, field_id));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_print_debug_info(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 is_replace,
    uint32 result_type,
    uint32 old_result_type)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Debug information on the chosen result type
     */
    if (!is_replace)
    {
        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "Successfully found a result type: %s %s%s%s\r\n",
                       dbal_result_type_to_string(unit, dbal_table_id, result_type), EMPTY, EMPTY, EMPTY);
    }
    else if (old_result_type != result_type)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Successfully found a new result type. Replacing old result type %s with new result type %s %s%s\r\n",
                    dbal_result_type_to_string(unit, dbal_table_id, old_result_type),
                    dbal_result_type_to_string(unit, dbal_table_id, result_type), EMPTY, EMPTY);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_allocate_local_outlif_and_set_hw_verify(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && ((lif_info->global_lif == 0) || (lif_info->global_lif == DNX_ALGO_GPM_LIF_INVALID)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lif must be allocated when calling dnx_lif_table_mngr_allocate_local_outlif_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set");
    }

    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        lif_table_mngr_outlif_info_t lif_info_existing;
        uint32 entry_handle_id_existing;
        dbal_tables_e dbal_table_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
        /*
         * Get existing handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_existing));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, *local_lif_id, entry_handle_id_existing, &lif_info_existing));
        /*
         * Verify out phase didn't change
         */
        if (lif_info_existing.outlif_phase != lif_info->outlif_phase)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "outlif phase cannot be replaced. Old value: %d, new value: %d \n",
                         lif_info_existing.outlif_phase, lif_info->outlif_phase);
        }
        /*
         * Verify replace is valid by checking each field on the handle
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_validity_verify(unit, entry_handle_id, entry_handle_id_existing));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    uint32 result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0;
    lif_table_mngr_replace_mode_e replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_NO_REPLACE;
    int entry_reuse;
    uint8 next_pointer_required_by_user;
    uint8 existing_result_type_has_ll;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw_verify
                          (unit, entry_handle_id, local_lif_id, lif_info));

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /*
     * Decide on smallest result type
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type
                    (unit, lif_info->table_specific_flags, lif_info->outlif_phase, entry_handle_id, &result_type));
    /*
     * If user was requesting to allocate fixed size,
     */
    if (lif_info->entry_size_to_allocate != 0)
    {
        CONST dbal_logical_table_t *table;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (lif_info->entry_size_to_allocate < table->multi_res_info[result_type].entry_payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not sufficient for smallest result type %d\n",
                         lif_info->entry_size_to_allocate, result_type);
        }
    }
    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, *local_lif_id,
                                                        NULL, &existing_result_type, NULL, NULL,
                                                        &existing_result_type_has_ll));
    }

    /*
     * Check if user is asking for next pointer, this info is required by lif allocation manager
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &next_pointer_required_by_user));
    /*
     * Lif allocation
     */
    if ((!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE) || (existing_result_type != result_type)
         || (existing_result_type_has_ll != next_pointer_required_by_user)))
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_outlif
                        (unit, dbal_table_id, lif_info, result_type, existing_result_type,
                         next_pointer_required_by_user, local_lif_id, &entry_reuse));
        if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
        {
            if (entry_reuse)
            {
                if (existing_result_type != result_type)
                {
                    replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_REUSE_LIF;
                }
                /**Can re-use lif, no need to update the lif mngr sw tables*/
                else
                {
                    replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_SAME_LIF_SIZE;
                }
            }
            /**entry_reuse == False, need to allocate a new lif*/
            else
            {
                replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF;
            }
        }
    }
    else
    {
        /** Get here if replacing an entry with the same size and ll indication, entry can be reused*/
        replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_SAME_LIF_SIZE;
    }

    /*
     * indications for new local lif that was allocated and new local lif with replace.
     * These indications will be useful later on
     */
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
                        || (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_store_valid_fields
                    (unit, 0, FALSE, dbal_table_id, -1, entry_handle_id, result_type, *local_lif_id,
                     !is_new_local_lif));

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update(unit, *local_lif_id, 0, 0, 0, is_new_local_lif));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    /*
     * set HW
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, existing_result_type, old_local_lif_id,
                         *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw
                        (unit, _SHR_CORE_ALL, FALSE, (void *) lif_info, dbal_table_id, entry_handle_id, result_type,
                         existing_result_type, *local_lif_id, (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_REUSE_LIF)));
    }

    /*
     * In case of replacing with new local lif: Change all EEDB pointers to point on the new lif
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_eedb_next_pointer
                        (unit, (uint32) old_local_lif_id, (uint32) *local_lif_id));
    }

    /*
     * Write to GLEM if required
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;

        if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
        {
            /*
             * In case of replacing with new local lif: Update GLM and remove all mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, LIF_LIB_GLEM_KEEP_OLD_VALUE));
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif));
        }
        else
        {
            /*
             * New lif: add new GLEM entry
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id));
        }

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif, &local_lif_mapping_info));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw_replace_result_type
                        (unit, _SHR_CORE_ALL, FALSE, (void *) lif_info, dbal_table_id,
                         entry_handle_id, existing_result_type, result_type, *local_lif_id, FALSE));
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, _SHR_CORE_ALL, FALSE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local old Lif clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, old_local_lif_id));

        /*
         * LIF table valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, old_local_lif_id));
    }
    /*
     * Print debug info
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, (replace_mode != LIF_TABLE_MNGR_REPLACE_MODE_NO_REPLACE), result_type,
                     existing_result_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_allocate_local_inlif_and_set_hw_verify(
    int unit,
    int core_id,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && ((lif_info->global_lif == 0) || (lif_info->global_lif == DNX_ALGO_GPM_LIF_INVALID)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lif must be allocated when calling dnx_lif_table_mngr_allocate_local_inlif_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set\n");
    }
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        lif_table_mngr_inlif_info_t lif_info_existing;
        uint32 entry_handle_id_existing;
        dbal_tables_e dbal_table_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
        /*
         * Get existing handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_existing));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, core_id, *local_lif_id, entry_handle_id_existing, &lif_info_existing));
        /*
         * Verify replace is valid by checking each field on the handle
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_validity_verify(unit, entry_handle_id, entry_handle_id_existing));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_allocate_local_inlif_and_set_hw(
    int unit,
    int core_id,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    uint32 result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0;
    dbal_physical_tables_e physical_table_id;
    lif_table_mngr_replace_mode_e replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_NO_REPLACE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw_verify
                          (unit, core_id, entry_handle_id, local_lif_id, lif_info));

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
    /*
     * Decide on smallest result type
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type
                    (unit, lif_info->table_specific_flags, 0, entry_handle_id, &result_type));
    /*
     * If user was requesting to allocate fixed size,
     */
    if (lif_info->entry_size_to_allocate != 0)
    {
        CONST dbal_logical_table_t *table;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (lif_info->entry_size_to_allocate < table->multi_res_info[result_type].entry_payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not sufficient for smallest result type %d\n",
                         lif_info->entry_size_to_allocate, result_type);
        }
    }
    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        dbal_tables_e dbal_table_id_dummy;

        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, *local_lif_id, core_id, physical_table_id, &dbal_table_id_dummy, &existing_result_type,
                         NULL));
    }

    /*
     * Lif allocation
     */
    if ((!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE) || (existing_result_type != result_type)))
    {
        int entry_reuse;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_inlif
                        (unit, core_id, dbal_table_id, lif_info, result_type, existing_result_type, local_lif_id,
                         &entry_reuse));
        if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
        {
            replace_mode = entry_reuse ? LIF_TABLE_MNGR_REPLACE_MODE_REUSE_LIF : LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF;
        }
    }
    else
    {
        /** Get here is replacing entry with same result type, entry can be reused */
        replace_mode = LIF_TABLE_MNGR_REPLACE_MODE_SAME_LIF_SIZE;
    }

    /*
     * indications for new local lif that was allocated and new local lif with replace.
     * These indications will be useful later on
     */
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
                        || (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_store_valid_fields
                    (unit, core_id, TRUE, dbal_table_id, physical_table_id, entry_handle_id, result_type,
                     *local_lif_id, !is_new_local_lif));

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update
                    (unit, *local_lif_id, 1, core_id, physical_table_id, is_new_local_lif));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    /*
     * set HW
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        /*
         * First copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, core_id, TRUE, dbal_table_id, existing_result_type, old_local_lif_id, *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw
                        (unit, core_id, TRUE, (void *) lif_info, dbal_table_id, entry_handle_id, result_type,
                         existing_result_type, *local_lif_id, (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_REUSE_LIF)));
    }

    /*
     * In case of replacing with new local lif: Change all ISEM entries to point on the new lif
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_isem_pointers
                        (unit, physical_table_id, (uint32) old_local_lif_id, (uint32) *local_lif_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_tcam_pointers
                        (unit, physical_table_id, (uint32) old_local_lif_id, (uint32) *local_lif_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_default_lif_pointers
                        (unit, physical_table_id, (uint32) old_local_lif_id, (uint32) *local_lif_id));
    }

    /*
     * Create SW global->local mapping
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;
        local_lif_mapping_info.core_id = core_id;
        local_lif_mapping_info.phy_table = physical_table_id;

        if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
        {
            /*
             * In case of replacing with new local lif: Remove mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif));
        }

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif, &local_lif_mapping_info));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (replace_mode == LIF_TABLE_MNGR_REPLACE_MODE_NEW_LIF)
    {
        lif_mngr_local_inlif_info_t old_local_lif_info;
        sal_memset(&old_local_lif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw_replace_result_type
                        (unit, core_id, TRUE, (void *) lif_info, dbal_table_id,
                         entry_handle_id, existing_result_type, result_type, *local_lif_id, FALSE));
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, core_id, TRUE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local old Lif clear
         */
        old_local_lif_info.dbal_table_id = dbal_table_id;
        old_local_lif_info.dbal_result_type = existing_result_type;
        old_local_lif_info.core_id = core_id;
        old_local_lif_info.local_inlif = old_local_lif_id;

        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &old_local_lif_info));

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_termination_match_information
                        (unit, core_id, physical_table_id, old_local_lif_id, *local_lif_id));
        /*
         * old LIF valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                        (unit, physical_table_id, core_id, old_local_lif_id));
    }
    /*
     * Print debug info
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, _SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE),
                     result_type, existing_result_type));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_outlif_info_clear(
    int unit,
    int local_lif_id,
    uint32 flags)
{
    dbal_tables_e dbal_table_id;
    uint32 result_type;
    int outlif_phase_dummy;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif_id,
                                                    &dbal_table_id, &result_type, &outlif_phase_dummy, NULL, NULL));

    /*
     * clear HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                    (unit, _SHR_CORE_ALL, FALSE, local_lif_id, dbal_table_id, result_type));

    /*
     * clear SW
     */

    /*
     * Clear VALID FIELDS SW state
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, local_lif_id));

    /*
     * Local Lif clear
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        /*
         * RIF has special handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_rif_free(unit, local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, local_lif_id));
    }

    /*
     * Clear GLEM if required
     */
    if (!_SHR_IS_FLAG_SET(flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING))
    {
        int global_lif;
        lif_mapping_local_lif_info_t local_lif_info;
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_lif_id;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif));

        /** Update the  GLEM. **/
        SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, global_lif));

        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_inlif_info_clear(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 flags)
{
    uint32 result_type;
    lif_mngr_local_inlif_info_t inlif_info;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                    (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id, &result_type, NULL));

    /*
     * clear HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw(unit, core_id, TRUE, local_lif_id, dbal_table_id, result_type));

    /*
     * clear SW
     */

    /*
     * Clear VALID FIELDS SW state
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif(unit, physical_table_id, core_id, local_lif_id));

    /*
     * Local Lif clear
     */
    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));
    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.dbal_result_type = result_type;
    inlif_info.core_id = core_id;
    inlif_info.local_inlif = local_lif_id;

    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &inlif_info));

    /*
     * Clear global to local mapping if required
     */
    if (!_SHR_IS_FLAG_SET(flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING))
    {
        int global_lif;
        lif_mapping_local_lif_info_t local_lif_info;
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_lif_id;
        local_lif_info.core_id = core_id;
        local_lif_info.phy_table = physical_table_id;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif));

        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_is_field_exist_on_dbal_handle(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle)
{
    shr_error_e rv;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Try to read value, if succeeded return found */
    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_field_on_handle = (rv == _SHR_E_NONE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle,
    uint32 *field_val)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Try to read value, if succeeded return found */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_field_on_handle = (rv == _SHR_E_NONE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_is_valid_field(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint8 is_ingress,
    dbal_fields_e field_id,
    uint8 *is_valid)
{
    uint32 sw_field_indicator_entry_handle_id;
    uint32 result_type;
    lif_mngr_outlif_phase_e outlif_phase_dummy;
    uint32 *field_ids_bitmap_p = NULL;
    int field_idx;
    CONST dbal_logical_table_t *table;
    dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
    dbal_tables_e dbal_table_id_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC(field_ids_bitmap_p,
              sizeof(*field_ids_bitmap_p) *
              _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit)),
              "field_ids_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    if (is_ingress)
    {
        /** Get physical table id from dbal table id */
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id_get, &result_type, NULL));
        if (dbal_table_id != dbal_table_id_get)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " There was an issue with dbal_table_id parameter, please check the input values.\n");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif_id, &dbal_table_id_get,
                                                        &result_type, &outlif_phase_dummy, NULL, NULL));
        if (dbal_table_id != dbal_table_id_get)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         " There was an issue with dbal_table_id parameter, please check the input values.\n");
        }
    }

    /** This SW table stores the valid fields for each lif */
    if (is_ingress == FALSE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_SBC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_DPC_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    *is_valid = FALSE;
    /** Loop over the fields and find the relevant field to check */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id_curr = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id == field_id_curr)
        {
            /** Check in SW state if this field is enabled and exit */
            *is_valid = SHR_BITGET(field_ids_bitmap_p, field_idx) ? TRUE : FALSE;
            break;
        }
    }

exit:
    SHR_FREE(field_ids_bitmap_p);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
