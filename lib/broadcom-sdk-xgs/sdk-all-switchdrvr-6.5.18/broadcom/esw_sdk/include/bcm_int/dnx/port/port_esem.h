/** \file port_esem.h
 * $Id$
 *
 * Internal DNX per Port ESEM management APIs
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef PORT_ESEM_H_INCLUDED
/* { */
#define PORT_ESEM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
/*
 * Defines {
 */
#define DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID     (-1)

/*
 * Define flags for esem access command exchanging.
 */

/** Allocate an ESEM cmd base on the inputs */
#define DNX_PORT_ESEM_CMD_ALLOCATE               (SAL_BIT(0))

/** Update the existing cmd, maybe add new access or update the default result profile.*/
#define DNX_PORT_ESEM_CMD_UPDATE                 (SAL_BIT(1))

/** Remove requested ESEM cmd from the exists group of ESEM commands (Must have the DNX_PORT_ESEM_CMD_UPDATE set with it) .*/
#define DNX_PORT_ESEM_CMD_REMOVE                 (SAL_BIT(2))

/** ESEM cmd can be allocated in range of 0-3 (relevant field size is 2)*/
#define DNX_PORT_ESEM_CMD_RANGE_0_3              (SAL_BIT(4))

/** ESEM cmd can be allocated in range of 0-7 (relevant field size is 3)*/
#define DNX_PORT_ESEM_CMD_RANGE_0_7              (SAL_BIT(5))

/** ESEM cmd can be allocated in range of 0-15 (relevant field size is 4)*/
#define DNX_PORT_ESEM_CMD_RANGE_0_15             (SAL_BIT(6))

/** ESEM cmd can be allocated in range of 60-63 (relevant field size is 2)*/
#define DNX_PORT_ESEM_CMD_RANGE_60_63            (SAL_BIT(7))

/** ESEM cmd can be allocated in range of 0-63 (relevant field size is 6)*/
#define DNX_PORT_ESEM_CMD_RANGE_0_63             (SAL_BIT(8))

/** Number of different prefix in ESEM command field in various tables.*/
#define DNX_ALGO_ESEM_CMD_NOF_DIFFER_PREFIX      (5)

/** Maximum prefix size of ESEM command field in various tables.*/
#define DNX_ALGO_ESEM_CMD_MAX_PREFIX_SIZE        (4)

/** Number of tags used in ESEM access command management.*/
#define DNX_ALGO_ESEM_CMD_RES_NOF_TAGS           (1 << DNX_ALGO_ESEM_CMD_MAX_PREFIX_SIZE)

/** The SVTAG ESEM access interface */
#define DNX_ALGO_ESEM_SVTAG_ESEM_CMD_IF ESEM_ACCESS_IF_2

/** }*/

/**
 * \brief
 *  Enum for ESEM access command range tag bitmap
 */
typedef enum dnx_esem_cmd_tag_range_e
{
    /*
     * The allocation can be achived.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_NONE = 0x0000,

    /*
     * The allocation must be in range of 0-3.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_0_3 = 0x0001,

    /*
     * The allocation must be in range of 0-7.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_0_7 = 0x0003,

    /*
     * The allocation must be in range of 0-15.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_0_15 = 0x000F,

    /*
     * The allocation must be in range of 60-63.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_60_63 = 0x8000,

    /*
     * The allocation must be in range of 0-63.
     */
    DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63 = 0xFFFF
} dnx_esem_cmd_tag_bmp_range_t;

/** Define possible ESEM access types */
typedef enum dnx_esem_access_type_e
{
    /** ESEM access for outer AC*/
    ESEM_ACCESS_TYPE_ETH_AC = 0,

    /** ESEM access for native AC*/
    ESEM_ACCESS_TYPE_ETH_NATIVE_AC = 1,

    /** ESEM access for forward domain network name, such as VNI/GRE-key/ISID, etc*/
    ESEM_ACCESS_TYPE_FODO_NETWORK = 2,
    ESEM_ACCESS_TYPE_VXLAN_VNI = 3,
    ESEM_ACCESS_TYPE_GRE_KEY = 4,
    ESEM_ACCESS_TYPE_MIM_ISID = 5,

    /** ESEM access for dual-homing, such as map EVPN-ESI label, IPv4 tunnel SIP, etc*/
    ESEM_ACCESS_TYPE_DUAL_HOME = 6,

    /** ESEM access for port based source address, such as B-SA in Mac-in-Mac, etc*/
    ESEM_ACCESS_TYPE_PORT_SA = 7,

    /** ESEM access for SVTAG*/
    ESEM_ACCESS_TYPE_SVTAG = 8,

    ESEM_ACCESS_TYPE_COUNT
} dnx_esem_access_type_t;

/*
 * }
 */
/*
 * Structures {
 */

/*
 * Holds all the fields of a single ESEM commands
 */
typedef struct dnx_port_esem_commands_info_s
{
    /*
     * ESEM command APP ID
     */
    dbal_enum_value_field_esem_app_db_id_e esem_app_db;
    /*
     * ESEM command access type
     */
    dnx_esem_access_type_t access_type;
    /*
     * ESEM command default result type
     */
    int esem_default_result_profile;
} dnx_port_esem_command_info_t;

/*
 * Holds all the ESEM commands that supposed to be allocated/removed together.
 * This structure is used to save several ESEM actions together to save swapping ESEM actions
 */
typedef struct dnx_port_esem_command_accesses_s
{
    /*
     * The NOF of requested ESEM commands to add
     */
    uint32 nof_accesses;
    /*
     * The requested ESEM commands to add information.
     */
    dnx_port_esem_command_info_t accesses[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES];

    /*
     * The NOF of ESEM interfaces to remove (this field is ignored unless the DNX_PORT_ESEM_CMD_REMOVE flag is set)
     */
    uint32 nof_interfaces_to_remove;
    /*
     * List of interfaces to remove
     */
    dnx_esem_access_if_t remove_interfaces[DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES];
} dnx_port_esem_command_accesses_t;

/*
 * }
 */

/*
 * Declarations {
 */
/**
 * \brief -
 *  Allocate and initialize esem default commands native and outer ac.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Esem cmds and default result profiles for default usage are predefined in
 *  dnx-data. Default cmds are allocated here. The corresponding tables are
 *  configured with default properties.
 *
 * \see
 *  dnx_pp_port_init
 */
shr_error_e dnx_port_esem_default_resource_init(
    int unit);

/**
 * \brief -
 *  Exchange an ESEM access command from sw algo.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port or out-lif-id in gport format.
 * \param [in] flags - ESEM access command allocation flags.
 *                     See DNX_PORT_ESEM_CMD_*
 * \param [in] prefix - In case an ARR prefix is required for the ESEM command.
 * \param [in] esem_accesses - The ESEM commands add/remove information.
 * \param [in] old_esem_cmd - The esem command used previously.
 * \param [out] new_esem_cmd - pointer to the esem command allocated.
 * \param [out] esem_cmd_data - pointer to esem command info used to allocate the new command.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  This exchange won't update hw.
 *  If flags DNX_PORT_ESEM_CMD_UPDATE is used, the exchange will
 *  inherit the old esem_cmd_data from old_esem_cmd and update it with the inputs;
 *  Else re-construct the esem_cmd_data per the inputs.
 *  If the old_esem_cmd is not one of the predefined default cmds, it will be de-allocated
 *  by decreasing its reference counter.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_exchange(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 prefix,
    dnx_port_esem_command_accesses_t * esem_accesses,
    int old_esem_cmd,
    int *new_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *is_first,
    uint8 *is_last);

/**
 * \brief -
 *  Set ESEM access command data to hardware table.
 *
 * \param [in] unit - relevant unit
 * \param [in] esem_cmd - The esem cmd id to update
 * \param [in] esem_cmd_data - ESEM access command data.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_set(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t esem_cmd_data);

/**
 * \brief -
 *  Clear the ESEM access command data from hardware table.
 *
 * \param [in] unit - relevant unit
 * \param [in] esem_cmd - The esem cmd id to update
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_clear(
    int unit,
    int esem_cmd);

/**
 * \brief -
 *  Free an ESEM access command from sw algo.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_cmd - The given ESEM access command.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_esem_cmd_exchange
 *  dnx_port_esem_cmd_data_clear
 */
shr_error_e dnx_port_esem_cmd_free(
    int unit,
    int esem_cmd,
    uint8 *is_last);

/**
 * \brief -
 *  Get the ESEM access command data according to the given command ID.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_cmd - The given ESEM access command data.
 * \param [out] esem_cmd_data - ESEM access command data.
 * \param [out] ref_count - Pointer for number of reference to the cmd, can be NULL.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_sw_get(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    int *ref_count);

/**
 * \brief -
 *  Get the ESEM access type according to ESEM default result profile.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_default_result_profile - esem default result profile
 *             Valid range is [0,15].
 * \param [out] access_type - Access type to esem table.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Get the access type to ESEM per the defaut result entry.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_access_type_get(
    int unit,
    int esem_default_result_profile,
    dnx_esem_access_type_t * access_type);

/**
 * \brief -
 *  Get the ESEM access interface.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] access_type - esem access type.
 *             See dnx_esem_access_type_t for the available types.
 * \param [in] esem_cmd_data - pointer to esem command data.
 * \param [out] esem_if - esem access interface.
 *              See dnx_esem_access_if_t for the possible interfaces.
 * \param [out] esem_entry_offset - esem entry offset relative to its EES entry.
 *              See dbal_enum_value_field_esem_offset_e for the possible values.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  There are two accesses to ESEM and must be used properly in order to yield
 *  two results. We define access_type to manage the applications that need accesses
 *  to ESEM. Access type should be determined according to applications and their
 *  match criterion. see dnx_esem_access_type_t for the available esem access types.
 *  Currently, we suppose one access for a kind of application, for example AC,
 *  is enough. So, the access type will be mapped to one esem access interface.
 *
 *  Note, the access_type may be enriched later according to the necessity.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_access_if_get(
    int unit,
    dnx_esem_access_type_t access_type,
    dnx_esem_cmd_data_t * esem_cmd_data,
    dnx_esem_access_if_t * esem_if,
    dbal_enum_value_field_esem_offset_e * esem_entry_offset);

/**
 * \brief -
 *  ADds a new commands to the "commands accesses" structure.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_app_db - The ESEM command access APP DB.
 * \param [in] access_type - ESEM access type.
 * \param [in] esem_default_result_profile -The ESEM command default result profile.
 * \param [out] esem_commands - The ESEM commands accesses structure.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_command_add(
    int unit,
    dbal_enum_value_field_esem_app_db_id_e esem_app_db,
    dnx_esem_access_type_t access_type,
    int esem_default_result_profile,
    dnx_port_esem_command_accesses_t * esem_commands);

/*
 * }
 */
#endif /* PORT_ESEM_H_INCLUDED */
