/** \file init_pp.h
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _INIT_PP_INCLUDED__
/*
 * {
 */
#define _INIT_PP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** Forwarding additional info in case of MPLS INJECTED OAMP */
#define MPLS_INJECTED_OAMP_FAI                  5
    /** Device Scope LIF */
#define DEVICE_SCOPE_LIF_PROFILE    0
    /** System Scope LIF */
#define SYSTEM_SCOPE_LIF_PROFILE    1
    /** Disable Same Interface Filter for Lif profile */
#define DISABLE_SAME_INTERFACE_LIF_PROFILE    2

    /** Device Scope LIF in JR mode */
#define DEVICE_SCOPE_JR_MODE_LIF_PROFILE    0
    /** System Scope LIF in JR mode */
#define SYSTEM_SCOPE_JR_MODE_LIF_PROFILE    1
    /** Disable Same Interface Filter for device scope in JR mode */
#define DEVICE_SCOPE_JR_MODE_LIF_PROFILE_DISABLE    3
    /** Disable Same Interface Filter for system scope in JR mode */
#define SYSTEM_SCOPE_JR_MODE_LIF_PROFILE_DISABLE    4
/*
 * }
 */

/**
 * \brief -
 * The function is called on Init time. 
 * It invokes forward_code_map_init and configures EGRESS_FORWARD_CODE_SELECTION table
 *
 * \param [in] unit -     Relevant unit.
 * 
 * \retval Error indication according to shr_error_e enum
 *  
 */

shr_error_e dnx_general_pp_init(
    int unit);

/**
 * \brief -
 * The function is called on DeInit time. 
 *
 * \param [in] unit -     Relevant unit.
 * 
 * \retval Error indication according to shr_error_e enum
 *  
 */

shr_error_e dnx_general_pp_deinit(
    int unit);

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] current_protocol_type - Current Protocol Type value
 * \param [in] current_protocol_namespace - Current Protocol Namespace
 * \param [in] current_next_protocol - Current Next Protocol

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pp_egress_current_next_protocol_config_table_configure(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int current_next_protocol);

/**
 * \brief - Get the current_next_protocol value for
 *        DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] current_protocol_type - Current Protocol Type value
 * \param [in] current_protocol_namespace - Current Protocol Namespace
 * \param [out] current_next_protocol - Current Next Protocol

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pp_egress_current_next_protocol_config_table_get(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int *current_next_protocol);

#endif/*_INIT_PP_INCLUDED__*/
