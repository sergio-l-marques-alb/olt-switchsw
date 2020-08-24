/** \file port_sit.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef PORT_SIT_H_INCLUDED
/* { */
#define PORT_SIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_TAG_SWAP_TPID_INDEX_DOT1BR              (0)
#define DNX_TAG_SWAP_TPID_INDEX_COE                    (1)
#define DNX_TAG_SWAP_TPID_INDEX_PON                    (1)

/**
 * SIT profile 0 is reserved for default, no sit header
 * SIT profile 1 is reserved for 802.1BR service
 * E-TAG.VID: from EEDB.Dest[35:24]
 * E-TAG.PCP: from outer VLAN
 */
#define DNX_PORT_MAX_SIT_PROFILE                        (7)
#define DNX_PORT_DEFAULT_SIT_PROFILE                    (0)
#define DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE             (1)

#define DNX_SIT_TYPE_DOT1BR                             (0)
#define DNX_SIT_TYPE_DOT1Q                              (1)
#define DNX_SIT_TYPE_DOT1T                              (2)
#define DNX_SIT_TYPE_PON                                (3)

/**define configured value to select PCP_DEI*/
#define DNX_SIT_PCP_DEI_SRC_AC_OUTER_TAG                (0)
#define DNX_SIT_PCP_DEI_SRC_AC_INNER_TAG                (1)
#define DNX_SIT_PCP_DEI_SRC_AC_THIRD_TAG                (2)
#define DNX_SIT_PCP_DEI_SRC_PORT_VALUE                  (3)
#define DNX_SIT_PCP_DEI_SRC_OUTER_TAG                   (4)
#define DNX_SIT_PCP_DEI_SRC_INNER_TAG                   (5)
#define DNX_SIT_PCP_DEI_SRC_NWK_QOS_INNER_TAG           (6)
#define DNX_SIT_PCP_DEI_SRC_NWK_QOS_OUTER_TAG           (7)

#define DNX_PORT_IS_PON_PORT(flags)                     (flags & DNX_ALGO_PORT_APP_FLAG_PON)
#define DNX_PORT_IS_COE_PORT(flags)                     (flags & DNX_ALGO_PORT_APP_FLAG_COE)

/** define the port type for opeartion */
#define DNX_PORT_TYPE_PON                               0x0
#define DNX_PORT_TYPE_COE                               0x1

#define DNX_MAX_PON_SUBPORT_ID                          (3)
#define PON_SUBPORT_ID_GET(tunnel_id)                   ((tunnel_id >> 12) & 0x7)
#define PON_CHANNEL_ID_GET(tunnel_id)                   (tunnel_id & 0xfff)

/** define max vid for COE: 8bits valid */
#define DNX_MAX_COE_VID                                 (255)
#define PTC_INVALID                                     0xfff

/**
 * \brief
 *   Allocate a sit profile template according to the profile info.
 *
 * \param [in] unit - unit ID
 * \param [in] sit_profile_info - sit profile information
 * \param [out] sit_profile  - sit profile ID
 * \param [out] is_first_sit_profile_reference   - whether this is the first reference

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 */

shr_error_e dnx_port_sit_profile_alloc(
    int unit,
    dnx_sit_profile_t * sit_profile_info,
    uint8 *sit_profile,
    uint8 *is_first_sit_profile_reference);

/**
 * \brief
 *   Release a sit profile template according to the profile info.
 *
 * \param [in] unit - unit ID
 * \param [out] sit_profile  - sit profile ID
 * \param [out] is_last   - whether this is the last reference

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 */

shr_error_e dnx_port_sit_profile_free(
    int unit,
    int sit_profile,
    uint8 *is_last);

/**
 * \brief
 *   Initiate the global tag swap configuration.
 *   tag_swap_tpid0: for 802.1BR;
 *   tag_swap_tpid0: for CoE
 *   Initate default sit profile for 802.1BR
 *
 * \param [in] unit - unit ID

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 */
shr_error_e dnx_port_sit_init(
    int unit);

/**
 * \brief
 *   Set the tag swap configuration per PTC.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] enable - enable/disable
 * \param [in] tpid_sel - which tpid is used for tag swap trigger
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER - always perform tag swap
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID0_TRIGGER - perform tag swap when tpid0 matched
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID1_TRIGGER - perform tag swap when tpid1 matched
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID0_TPID1_TRIGGER - perform tag swap when tpid0-or tpid1 matched
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * It's internal module, parameter validity is guaranteed by caller
 * So no check opration here.
 *
 * \see
 * dbal_enum_value_field_sit_tag_swap_mode_e
 */

shr_error_e dnx_port_sit_tag_swap_set(
    int unit,
    bcm_gport_t port,
    int enable,
    dbal_enum_value_field_sit_tag_swap_mode_e tpid_sel);

/**
 * \brief
 *   Get the tag swap configuration per PTC.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] enable - enable/disable
 * \param [out] tpid_sel - which tpid is used for tag swap trigger
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER - always perform tag swap
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID0_TRIGGER - perform tag swap when tpid0 matched
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID1_TRIGGER - perform tag swap when tpid1 matched
 *                       DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID0_TPID1_TRIGGER - perform tag swap when tpid0-or tpid1 matched

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * It's internal module, parameter validity is guaranteed by caller
 * So no check opration here.

 *
 *
 * \see
 * dbal_enum_value_field_sit_tag_swap_mode_e
 */

shr_error_e dnx_port_sit_tag_swap_get(
    int unit,
    bcm_gport_t port,
    int *enable,
    dbal_enum_value_field_sit_tag_swap_mode_e * tpid_sel);

/**
 * \brief
 *   Set the tag swap tpid value.
 *
 * \param [in] unit - unit ID
 * \param [in] tpid_index - tpid index
 *                        DNX_TAG_SWAP_TPID_INDEX_DOT1BR: 802.1BR
 *                        DNX_TAG_SWAP_TPID_INDEX_DOT1BR: CoE
 * \param [in] tpid - tpid value

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * It's internal module, parameter validity is guaranteed by caller
 * So no check opration here.
 
 *
 * \see
 */

shr_error_e dnx_port_sit_tag_swap_tpid_set(
    int unit,
    int tpid_index,
    uint16 tpid);

/**
 * \brief
 *   Get the tag swap tpid value.
 *
 * \param [in] unit - unit ID
 * \param [in] tpid_index - tpid index
 *                        DNX_TAG_SWAP_TPID_INDEX_DOT1BR:  802.1BR
 *                        DNX_TAG_SWAP_TPID_INDEX_COE:  CoE
 * \param [out] tpid - tpid value

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * It's internal module, parameter validity is guaranteed by caller
 * So no check opration here.

 *
 * \see
 */

shr_error_e dnx_port_sit_tag_swap_tpid_get(
    int unit,
    int tpid_index,
    uint16 *tpid);

/**
 * \brief
 *   Write the sit profile into to HW table.
 *
 * \param [in] unit - unit ID
 * \param [in] sit_profile -sit profile index for HW table
 * \param [in] sit_profile_info -sit profile parameter to be write to HW 

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
  *
 * \see
 */
shr_error_e dnx_port_sit_profile_hw_set(
    int unit,
    uint8 sit_profile,
    dnx_sit_profile_t * sit_profile_info);

/**
 * \brief
 *   Get the sit profile from HW table.
 *
 * \param [in] unit - unit ID
 * \param [in] sit_profile -sit profile index for HW table
 * \param [out] sit_profile_info -sit profile parameter get from HW 

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
  *
 * \see
 */
shr_error_e dnx_port_sit_profile_hw_get(
    int unit,
    uint8 sit_profile,
    dnx_sit_profile_t * sit_profile_info);

/**
 * \brief
 *   Clear the sit profile HW table.
 *
 * \param [in] unit - unit ID
 * \param [in] sit_profile -sit profile index for HW table

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
  *
 * \see
 */

shr_error_e dnx_port_sit_profile_hw_clear(
    int unit,
    uint8 sit_profile);

/**
 * \brief
 *   Configure the per PP port SIT parameter.
 *
 * \param [in] unit - unit ID
 * \param [in] port - pp port
 * \param [in] sit_profile -sit profile ID
 * \param [in] sit_pcp   -port default pcp
 * \param [in] sit_dei -port default dei
 * \param [in] sit_vid -port default sit vid

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * For input sit parameter, NULL pointer means this field need not 
 * update, case of  4 NULL pointers is thought as invalid.
 * \see
 */

shr_error_e dnx_port_sit_egress_port_set(
    int unit,
    bcm_gport_t port,
    uint8 *sit_profile,
    uint8 *sit_pcp,
    uint8 *sit_dei,
    uint16 *sit_vid);

/**
 * \brief
 *   Retreive the per PP port SIT parameter from HW table
 *
 * \param [in] unit - unit ID
 * \param [in] port - pp port
 * \param [out] sit_profile -sit profile ID
 * \param [out] sit_pcp   -port default pcp
 * \param [out] sit_dei -port default dei
 * \param [out] sit_vid -port default sit vid

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * For input sit parameter, NULL pointer means this field need not 
 * retreived, case of  4 NULL pointers is thought as invalid.
 * \see
 */

shr_error_e dnx_port_sit_egress_port_get(
    int unit,
    bcm_gport_t port,
    uint8 *sit_profile,
    uint8 *sit_pcp,
    uint8 *sit_dei,
    uint16 *sit_vid);

/**
 * \brief
 *   Check if SIT profile in used.
 *
 * \param [in] unit - unit ID
 * \param [in] sit_profile - SIT profile to check
 * \param [out] is_used - 0: unused, 1-used
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */

shr_error_e dnx_port_sit_profile_is_used(
    int unit,
    uint8 sit_profile,
    uint8 *is_used);

/**
 * \brief
 *   Get SIT profile for sit type according to input parameter.
 *
 * \param [in] unit - unit ID
 * \param [in] type - SIT type
 * \param [in] para - input para for sit
 * \param [out] sit_profile - return sit profile.
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */
shr_error_e dnx_port_sit_get_sit_profile(
    int unit,
    int type,
    void *para,
    uint8 *sit_profile);

/**
 * \brief
 *   Get PCP_DEI source.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [out] value - return PCP_DEI source.
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */
shr_error_e dnx_port_sit_tag_pcp_dei_src_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief
 *   Set PCP_DEI source.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [in] value - PCP_DEI source. Supported source: \n
 *      0 - DNX_SIT_PCP_DEI_SRC_AC_OUTER_TAG \n
 *      1 - DNX_SIT_PCP_DEI_SRC_AC_INNER_TAG \n
 *      2 - DNX_SIT_PCP_DEI_SRC_AC_THIRD_TAG \n
 *      3 - DNX_SIT_PCP_DEI_SRC_PORT_VALUE \n
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 */
shr_error_e dnx_port_sit_tag_pcp_dei_src_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief
 *   Get prepend Tag setting.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [out] value - return PCP_DEI source.
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */
shr_error_e dnx_port_sit_prepend_tag_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief
 *   Enable/Disable prepend tag for port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [in] value - 0: enable, 1: disables.
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */
shr_error_e dnx_port_sit_prepend_tag_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief
 *   Set port type flag.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - COE port.
 * \param [in] flags    - flag for port type. \n
 *                          - DNX_PORT_FLAGS_COE_PORT: COE port type \n
 *                          - DNX_PORT_FLAGS_PON_PORT: PON port type \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_port_flag_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief
 *   Get port type flag.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - COE port.
 * \param [out] flags    - flag for port type. \n
 *                          - DNX_PORT_FLAGS_COE_PORT: COE port type \n
 *                          - DNX_PORT_FLAGS_PON_PORT: PON port type \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */

shr_error_e dnx_port_sit_port_flag_get(
    int unit,
    bcm_port_t port,
    uint32 *flags);

/**
 * \brief
 *   Enable/Disable COE port.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - COE port.
 * \param [in] value    - value for operation. \n
 *                          - 1: enable \n
 *                          - 0: disable \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_coe_enable_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief
 *   Get COE port status.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - COE port.
 * \param [out] value   - value of COE status. \n
 *                          - 1: enable \n
 *                          - 0: disable \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_coe_enable_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief
 *   Enable/Disable PON port.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - PON port.
 * \param [in] value    - value for operation. \n
 *                          - 1: enable \n
 *                          - 0: disable \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_pon_enable_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief
 *   Get PON port status.
 *
 * \param [in] unit     - device unit number.
 * \param [in] port     - PON port.
 * \param [out] value   - value of PON status. \n
 *                          - 1: enable \n
 *                          - 0: disable \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_pon_enable_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief
 *   Set port mapping infomation.
 *
 * \param [in] unit     - device unit number.
 * \param [in] type     - mapping type. \n
 *                          - bcmPortExtenderMappingTypePonTunnel (used for PON)\n
 *                          - bcmPortExtenderMappingTypePortVlan (used for COE)\n
 * \param [in] mapping_info - information for mapping. \n
 *                          - pp_port: pp_port mapping to. \n
 *                          - tunnel_id: PON tunnel mapping to, used for PON. \n
 *                          - phy_port: first channel under same interface. \n
 *                          - vlan: VID of CoE port, both for ingress mapping and egress encapsulation. \n
 *                          - user_define_value: un-used.  \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_application_mapping_info_set(
    int unit,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info);

/**
 * \brief
 *   Get port mapping infomation.
 *
 * \param [in] unit     - device unit number.
 * \param [in] type     - mapping type. \n
 *                          - bcmPortExtenderMappingTypePonTunnel \n
 *                          - bcmPortExtenderMappingTypePortVlan \n
 * \param [in,out] mapping_info - get information for mapping. \n
 *                          - pp_port[out]: pp_port mapping to. \n
 *                          - tunnel_id[out]: PON tunnel mapping to, used for PON. \n
 *                          - phy_port[in]: first channel under same interface. \n
 *                          - vlan[in]: VID of CoE port. \n
 *                          - user_define_value: un-used.  \n
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *
 * \see
 *   * None
 */

shr_error_e dnx_port_sit_application_mapping_info_get(
    int unit,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info);

/**
 * \brief
 *   Set sw table for virtural port TCAM management.
 *
 * \param [in] unit - unit ID
 * \param [in] access_id - access id for TCAM entry
 * \param [in] is_use  - if use of TCAM entry
 * \param [in] priority - priority of TCAM entry, useless for COE
 * \param [in] pp_port - Result.pp_port of TCAM entry
 * \param [in] sys_port - Result.sys_port of TCAM entry, useless for COE
 * \param [in] key - Key of TCAM entry
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_virtual_port_tcam_sw_set(
    int unit,
    uint32 access_id,
    uint8 is_use,
    uint16 priority,
    uint16 pp_port,
    uint16 sys_port,
    uint32 key);

/**
 * \brief
 *   Allocate a virtual port TCAM access id according to Key and mask.
 *
 * \param [in] unit - unit ID
 * \param [in] key - key for TCAM entry
 * \param [in] mask - mask for TCAM entry
 * \param [out] entry_access_id  - access id of TCAM entry
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   * mask is used for priority.
 *   * mask is not support at present.
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_virtual_port_access_id_alloc(
    int unit,
    uint32 key,
    uint32 mask,
    uint32 *entry_access_id);

/**
 * \brief
 *   Get virtural port TCAM access id according to port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - local port
 * \param [out] entry_access_id  - hold access id of TCAM entry
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_sit_virtual_port_access_id_get_by_port(
    int unit,
    bcm_port_t port,
    uint32 *entry_access_id);
/* } */
/* } */
#endif /* PORT_SIT_H_INCLUDED */
