/** \file port_pp.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _PORT_PP_API_INCLUDED__
/*
 * { 
 */
#define _PORT_PP_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/port.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief Set VLAN domain on a port.
 *
 * This function is used to set both ingress and egress VLAN domain \n
 * HW tables on given pp port.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit unit gport belongs to
 *   \param [in] port port to set vlan domain on
 *   \param [in] vlan_domain required VLAN domain
 * \par DIRECT OUTPUT:
 *   \retval Error indication according to shr_error_e enum
 */
shr_error_e dnx_port_pp_vlan_domain_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_domain);

/**
 * \brief 
 *    Set DP profile on LIF for IVE outer or inner PCP mapping.
 * \param [in] unit - unit gport belongs to
 * \param [in] port - LIF to set DP profile on
 * \param [in] type - relevant type
              QOS_DP_PROFILE_OUTER
              QOS_DP_PROFILE_INNER
 * \param [in] map_id - qos map id 
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */

shr_error_e dnx_port_pp_ive_dp_profile_set(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int map_id);

/**
 * \brief 
 *     Get DP profile from LIF for IVE outer or inner PCP mapping.
 *
 * \par DIRECT INPUT:
 * \param [in] unit - unit gport belongs to
 * \param [in] port - LIF to set DP profile on
 * \param [in] type - relevant type
              QOS_DP_PROFILE_OUTER
              QOS_DP_PROFILE_INNER
 * \param [out] map_id - qos map id 
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */

shr_error_e dnx_port_pp_ive_dp_profile_get(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int *map_id);

/**
 * \brief -
 * This function is used to set ingress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_ingress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain);

/**
 * \brief -
 * This function is used to set egress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_egress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain);

/**
 * \brief -
 * This function is used to get ingress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [out] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_ingress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain);

/**
 * \brief -
 * This function is used to get egress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [out] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_egress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain);

/**
 * \brief - Get VLAN domain on a port
 *  Get both ingress and egress VLAN domains
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] vlan_domain - Vlan domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_domain_get(
    int unit,
    bcm_port_t port,
    uint32 *vlan_domain);

/**
 * \brief
 * Configure Default Port Match, called by 
 * bcm_dnx_port_match_add in case match criteria is PORT 
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] port -
 *     port - gport
 *   \param [in] match_info - bcm_dnx_port_match_add param.
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: MAC table is full
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_port_pp_egress_match_port_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

/**
 * \brief -
 * Initialize PP port module. As part of the initialization, the 
 * function configure default VLAN membership if values, called 
 * durring initialization of the device.
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -
 *     Relevant unit.
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 * \par INDIRECT OUTPUT:
 *    \retval
 *    Set default 1:1 mapping for VLAN membership according to
 *    Port x VLAN
 */
shr_error_e dnx_pp_port_init(
    int unit);

/**
 * \brief -
 * De-initialize PP port module.
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -
 *     Relevant unit.
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 */
shr_error_e dnx_pp_port_deinit(
    int unit);

/**
 * \brief - Get the VLAN Membership-IF from internal PP port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] internal_core_id - Internal Core id
 * \param [in] internal_port_pp - Internal PP port
 * \param [out] vlan_mem_if - Vlan membership interface
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  We assume that VLAN-membership-IF mapping from local port is
 *  symmetric.Because of that, it is enough to get the
 *  information from Ingress Port table.
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_membership_if_get(
    int unit,
    bcm_core_t internal_core_id,
    bcm_port_t internal_port_pp,
    uint32 *vlan_mem_if);

/**
 * \brief - Set the VLAN Membership-IF per local port.
 *  Write to INGRESS/EGRESS_PP_PORT_TABLE.
 *  We assume that VLAN-membership-IF mapping per local port is symmetric and
 *  because of that we need to set the appropriate information to Ingress and Egress PP Port tables.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port - physical port
 * \param [in] vlan_mem_if - VLAN-membership-if
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_membership_if_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_mem_if);

/**
 * \brief - Provide mapping between source system port and gport
 *
 * \param [in] unit - Unit id
 * \param [in] core_id - In core id
 * \param [in] pp_port - In pp port
 * \param [in] src_system_port - Source system port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_pp_src_system_port_set(
    int unit,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 src_system_port);

/**
 * \brief -
 *  Set the filter enablers to pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] filter_flags - Indications for filter enablers. Possible values:
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_ALL.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_unknown_filter_get
 */
shr_error_e dnx_port_pp_l2_unknown_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] filter_flags - Indications for filter enablers. Possible values:
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_ALL.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_unknown_filter_set
 */
shr_error_e dnx_port_pp_l2_unknown_filter_get(
    int unit,
    bcm_port_t port,
    int *filter_flags);

/**
 * \brief
 *   Set the PRT and LLR ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] prt_profile - PRT ptc_profile 
 * \param [in] llr_profile - LLR ptc_profile 
 
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *  
 * \see
 * dbal_enum_value_field_ingress_port_termination_profile_e
 */
shr_error_e dnx_port_ptc_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e prt_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile);

/**
 * \brief
 *   Set the LLR and VT ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] llr_profile - LLR ptc_profile 
 * \param [in] vt_profile - VT ptc_profile 
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *  
 * \see
 * dbal_enum_value_field_ingress_port_termination_profile_e, 
 * dbal_enum_value_field_vt_ptc_profile_e 
 */
shr_error_e dnx_port_pp_llr_vtt_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e vt_profile);
/**
 * \brief
 *   Get the LLR and VT ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] llr_profile_p -
 *   Pointer to dbal_enum_value_field_ingress_port_termination_profile_e. This
 *   procedure loads pointedmemory by LLR ptc_profile.
 * \param [in] vt_profile_p -
 *   Pointer to dbal_enum_value_field_ingress_port_termination_profile_e. This
 *   procedure loads pointedmemory by VT ptc_profile.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 * dbal_enum_value_field_ingress_port_termination_profile_e, 
 */
shr_error_e dnx_port_pp_llr_vtt_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_ingress_port_termination_profile_e * llr_profile_p,
    dbal_enum_value_field_ingress_port_termination_profile_e * vt_profile_p);

/**
 * \brief
 *   Get the PRT and LLR ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] prt_profile - PRT ptc_profile
 * \param [out] llr_profile - LLR ptc_profile
 
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_ptc_profile_set
 */
shr_error_e dnx_port_ptc_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e * prt_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e * llr_profile);

/**
 * \brief -
 *  Set the action profile for sa-drop to port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] action_flags - Action flags for SA drop. Possible values:
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_NONE,
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_DROP,
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_TRAP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_sa_drop_action_profile_get
 */
shr_error_e dnx_port_pp_l2_sa_drop_action_profile_set(
    int unit,
    bcm_port_t port,
    int action_flags);

/**
 * \brief -
 *  Get the action profile for sa-drop from port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] action_flags - Action flags for SA drop. Possible values:
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_NONE,
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_DROP,
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_TRAP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_sa_drop_action_profile_set
 */
shr_error_e dnx_port_pp_l2_sa_drop_action_profile_get(
    int unit,
    bcm_port_t port,
    int *action_flags);

/**
 * \brief - Configure Ingress Ethernet properties
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *  In accordance with it the ethernet properties will be enabled
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_initial_eth_properties_set(
    int unit,
    bcm_port_t port,
    uint32 header_type);

/**
 * \brief - Configure Trap Context Port Profile according to switch header type
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] switch_header_type - Switch header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_egress_set(
    int unit,
    bcm_port_t port,
    int switch_header_type);

/**
 * \brief - Configure Port Termination PTC Profile according to the header type
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port number
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_prt_ptc_profile_set(
    int unit,
    bcm_port_t port,
    uint32 header_type);

/**
 * \brief - Configure Port Termination PTC Profile according to the header type based on gport_info
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_info - gport info
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * this function is used in places where algo_gpm cannot return all needed
 *     info to make this configuration and additional actions are required. one such places is lag.
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_prt_ptc_profile_internal_set(
    int unit,
    dnx_algo_gpm_gport_phy_info_t * gport_info,
    uint32 header_type);

/**
 * \brief -
 *  Set the flooding destinations for the unknown packets.
 *
 * \param [in] unit - The unit ID.
 * \param [in] port_flood_profile - Port flooding profile.
 * \param [in] lif_flood_profile - InLIF flooding profile.
 * \param [in] default_frwrd_dst - Forwarding destinations for unknwon packets.
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  port_flood_profile = lif_flood_profile = 0 is used as system default
 *  configurations in which VSI.default-forward-destination is used.
 *  In case of port flooding, port_flood_profile = 1 and lif_flood_profile = 0.
 *  In case of LIF flooding, port_flood_profile = 0/1 and lif_flood_profile = input.
 *
 * \see
 *  bcm_dnx_port_flood_group_set.
 */
shr_error_e dnx_port_lif_flood_profile_action_set(
    int unit,
    int port_flood_profile,
    int lif_flood_profile,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES]);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] filter_flags - Indications for filter enablers. 
 *                Possible values:   TRUE - Logical Same interface filter is enabled.
 *                Possible values:   FALSE - Logical Same interface filter is disabled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_lif_same_interface_disable_set(
    int unit,
    bcm_port_t port,
    int filter_flags);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] filter_flags - Indications for filter enablers. 
 *                Possible values:   TRUE - Logical Same interface filter is enabled.
 *                Possible values:   FALSE - Logical Same interface filter is disabled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_lif_same_interface_disable_get(
    int unit,
    bcm_port_t port,
    int *filter_flags);

    /**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port id.
 * \param [in] in_lif_same_interface_mode - Set the LIF profile mode.
 *                Possible values:   0 - Device Scope LIF.
 *                Possible values:   1 - System Scope LIF.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_in_lif_same_interface_set(
    int unit,
    bcm_port_t gport,
    int in_lif_same_interface_mode);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port id.
 * \param [out] in_lif_same_interface_mode - Get the LIF Mode.
 *                Possible values:   0 - Device scope LIF.
 *                Possible values:   1 - System Scope LIF.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_in_lif_same_interface_get(
    int unit,
    bcm_port_t gport,
    int *in_lif_same_interface_mode);

/**
 * \brief - Configure PORT_VID in INGRESS_PP_PORT table
 *
 * \param [in] unit - Relevant unit
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core id
 * \param [in] vid - VLAN id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_untagged_vlan_set_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id,
    bcm_vlan_t vid);

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a physical port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] core_id - Core ID the port resides in.
 * \param [in] pp_port - Physical port ID in the core.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_learn_set(
    int unit,
    int core_id,
    int pp_port,
    uint32 flags);

/**
 * \brief - Update the tm_port of trunk member with the pp properties
 *    of trunk`s pp port
 *
 * \param [in] unit - Unit number
 * \param [in] trunk_id - Trunk id
 * \param [in] core - Core id
 * \param [in] tm_port - TM port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_pp_port_update(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 core,
    uint32 tm_port);

/**
 * \brief - Configure parsing context per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] parsing_context - Parsing context
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_parsing_context_set(
    int unit,
    bcm_port_t port,
    uint32 parsing_context);
/*
 * }
 */
#endif /**_PORT_PP_API_INCLUDED__*/
