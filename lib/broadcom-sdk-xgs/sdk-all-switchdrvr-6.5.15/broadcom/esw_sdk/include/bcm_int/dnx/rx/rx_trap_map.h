/** 
 *  \file bcm_int/dnx/rx/rx_trap_map.h
 * $Id$
 * 
 * Internal DNX RX APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef RX_TRAP_MAP_H_INCLUDED
/* { */
#define RX_TRAP_MAP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>

#define DNX_RX_TRAP_MAP_MAX_NOF_DEFAULT_TRAPS 3

/* 
 * Types 
 * { 
 */

/**
 * \brief struct containing mapping info for trap_type.
 */
typedef struct
{
    int valid;
    uint32 trap_id;
    dnx_rx_trap_block_e trap_block;
    dnx_rx_trap_class_e trap_class;
    char *trap_name;
    char *description;
    uint32 flags;
} dnx_rx_trap_map_type_t;

/**
 * \brief struct containing user-defined traps information.
 */
typedef struct
{
    /** User-defined trap ids range is [lower_bound_trap_id, upper_bound_trap_id] */
    int lower_bound_trap_id;
    int upper_bound_trap_id;
    char *block_name;
    int default_trap_id_list[DNX_RX_TRAP_MAP_MAX_NOF_DEFAULT_TRAPS];
} dnx_rx_trap_map_ud_t;

/**
 * \brief struct containing Ingress user-defined traps mapping.
 */
typedef struct
{
    int valid;
    int alloc_index;
} dnx_rx_trap_map_ingress_ud_t;

/**
 * \brief struct containing ETPP profile dbal fields.
 */
typedef struct
{
    /** recycle packet priority field */
    dbal_fields_e cfg_rcy_prio;

    /** recycle packet crop field */
    dbal_fields_e cfg_rcy_crop;

    /** recycle packet append orifinal FTMH field */
    dbal_fields_e cfg_rcy_append_orig;

    /** recycle command field */
    dbal_fields_e cfg_rcy_cmd;

    /** CPU trap code field */
    dbal_fields_e cfg_cpu_trap_code;
} dnx_rx_trap_map_etpp_profile_dbal_fields_t;

/**
 * \brief struct containing statistical object overwrite dbal fields.
 */
typedef struct
{
    /** overwrite field */
    dbal_fields_e overwrite;

    /** index to overwrite field */
    dbal_fields_e index;

    /** value field */
    dbal_fields_e value;

    /** offset by qualifier field */
    dbal_fields_e offset_by_qual;

    /** type field */
    dbal_fields_e type;

    /** is meter field */
    dbal_fields_e is_meter;
} dnx_rx_trap_map_stat_obj_dbal_fields_t;

/*
* } Types
*/

/*
 * Globals
 * {
 */

extern const dnx_rx_trap_map_type_t dnx_rx_trap_type_map[bcmRxTrapCount];
extern const dnx_rx_trap_map_type_t dnx_rx_app_trap_type_map[bcmRxTrapCount];
extern const dnx_rx_trap_map_ud_t dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_NUM_OF];
extern const dnx_rx_trap_map_etpp_profile_dbal_fields_t
    dnx_rx_trap_map_etpp_ud_profile[DNX_RX_TRAP_ETPP_FWD_ACT_NOF_PROFILES];
extern const dnx_rx_trap_map_etpp_profile_dbal_fields_t
    dnx_rx_trap_map_etpp_oam_profile[DNX_RX_TRAP_ETPP_OAM_NOF_TRAPS];
extern const dnx_rx_trap_map_stat_obj_dbal_fields_t dnx_rx_trap_map_stat_obj[DNX_RX_TRAP_NOF_STAT_OBJS_TO_OW];
extern const dnx_rx_trap_map_ingress_ud_t dnx_rx_trap_map_ingress_ud[DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES];
extern const dbal_enum_value_field_ingress_trap_id_e dnx_rx_trap_map_ingress_ud_alloc[DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES];

/**
 * \brief - check if ingress trap is user defined
 */
#define DNX_RX_TRAP_IS_INGRESS_USER_DEFINED(trap_id)  (dnx_rx_trap_map_ingress_ud[trap_id].valid)

/**
 * \brief - check if ERPP trap is user defined
 */
#define DNX_RX_TRAP_IS_ERPP_USER_DEFINED(trap_id) ((trap_id >= dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_ERPP].lower_bound_trap_id) || (trap_id <= dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_ERPP].upper_bound_trap_id))

/**
 * \brief - check if ETPP trap is user defined
 */
#define DNX_RX_TRAP_IS_ETPP_USER_DEFINED(trap_id) ((trap_id >= dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_ETPP].lower_bound_trap_id) || (trap_id <= dnx_rx_ud_trap_map[DNX_RX_TRAP_BLOCK_ETPP].upper_bound_trap_id))

/*
* } Globals
*/

/**
* \brief
*  Retrun list of trap map info for supported per block and class 
* \param [in] unit            - Device ID
* \param [in] trap_block      - Stage to get trap ID for
* \param [in] trap_class      -  Trap class to get Id of
* \param [out] trap_map_list_p  - List of trap map info
* \param [out] nof_traps_p   - number of traps in the list
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_map_list(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_class_e trap_class,
    dnx_rx_trap_map_type_t ** trap_map_list_p,
    int *nof_traps_p);

/**
* \brief
*  Retrun list of application trap map info for supported per block 
* \param [in] unit            - Device ID
* \param [in] trap_block      - Stage to get trap ID for
* \param [out] trap_map_list_p  - List of trap map info
* \param [out] nof_traps_p   - number of traps in the list
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_app_map_list(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_type_t ** trap_map_list_p,
    int *nof_traps_p);

/**
* \brief
*  Retrun trap type of application trap
* \param [in] unit            - Device ID
* \param [in] trap_block      - trap block
* \param [in] trap_id  - trap_id
* \param [out] trap_type_p   - bcmRxTrap type
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_app_type_from_id_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    uint32 trap_id,
    bcm_rx_trap_t * trap_type_p);

#endif/*_RX_TRAP_MAP_API_INCLUDED__*/
