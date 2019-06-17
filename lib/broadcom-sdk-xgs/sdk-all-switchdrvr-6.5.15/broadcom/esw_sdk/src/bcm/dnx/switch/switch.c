/**
 * \file switch.c
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <soc/counter.h>
#include <soc/shared/sat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnxc/drv.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#include <bcm_int/dnx/l2/l2_traverse.h>
#include <bcm_int/dnx/l2/l2_station.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/rx/rx_trap_map.h>
#include <bcm_int/dnx/l3/l3_vrrp.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <src/bcm/dnx/mpls_port/mpls_port.h>
#include <src/bcm/dnx/init/init_pp.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnx/swstate/auto_generated/access/interrupt_access.h>
#include <soc/dnxc/legacy/dnxc_mem.h>
#include <soc/dnx/synce/synce.h>
#include <soc/sand/sand_mem.h>
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/mdb_sbusdma_desc.h>
#include <bcm_int/dnx/field/field_map.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <bcm_int/dnx/cosq/cosq_latency.h>
#include <bcm_int/dnx/port/imb/imb.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif /* BCM_WARM_BOOT_API_TEST */

/*
 * }
 */

#if defined(BCM_RCPU_SUPPORT)
/*
 * {
 */
/*
 * Function:
 *      _bcm_dnx_rcpu_switch_enable_set
 * Description:
 *      Helper function to enable SCHEN, CMIC, MAC and VLAN for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to modify
 *  arg - The value to set
 * Returns:
 *      BCM_E_xxx
 */
static int
_bcm_dnx_rcpu_switch_enable_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    uint32 entry_handle_id, value;
    dbal_fields_e field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmSwitchRemoteCpuSchanEnable:
            field = DBAL_FIELD_SCHAN_EN;
            break;
        case bcmSwitchRemoteCpuMatchLocalMac:
            field = DBAL_FIELD_MAC_MATCH_EN;
            break;
        case bcmSwitchRemoteCpuCmicEnable:
            field = DBAL_FIELD_CMIC_EN;
            break;
        case bcmSwitchRemoteCpuMatchVlan:
            field = DBAL_FIELD_VLAN_MATCH_EN;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENABLE_CTRL, &entry_handle_id));

    value = (uint32) arg;
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnx_rcpu_switch_enable_get
 * Description:
 *      Helper function to get enabling status of SCHEN, CMIC, MAC and VLAN for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to get
 *  arg - The value returned
 * Returns:
 *      BCM_E_xxx
 */
static int
_bcm_dnx_rcpu_switch_enable_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    uint32 entry_handle_id, value;
    dbal_fields_e field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmSwitchRemoteCpuSchanEnable:
            field = DBAL_FIELD_SCHAN_EN;
            break;
        case bcmSwitchRemoteCpuMatchLocalMac:
            field = DBAL_FIELD_MAC_MATCH_EN;
            break;
        case bcmSwitchRemoteCpuCmicEnable:
            field = DBAL_FIELD_CMIC_EN;
            break;
        case bcmSwitchRemoteCpuMatchVlan:
            field = DBAL_FIELD_VLAN_MATCH_EN;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENABLE_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, &value));

    *arg = (int) value;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnx_rcpu_switch_vlan_tpid_sig_set
 * Description:
 *      Helper function to set VLAN, TPID ot Signature for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to modify
 *  arg - The value to set
 * Returns:
 *      BCM_E_xxx
 */
static int
_bcm_dnx_rcpu_switch_vlan_tpid_sig_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    uint32 entry_handle_id, value;
    dbal_fields_e field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmSwitchRemoteCpuVlan:
            field = DBAL_FIELD_VLAN;
            break;
        case bcmSwitchRemoteCpuTpid:
            field = DBAL_FIELD_TPID;
            break;
        case bcmSwitchRemoteCpuSignature:
            field = DBAL_FIELD_SIGNATURE;
            break;
        case bcmSwitchRemoteCpuEthertype:
            field = DBAL_FIELD_ETHERTYPE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));

    value = (arg & 0xffff);
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnx_rcpu_switch_vlan_tpid_sig_get
 * Description:
 *      Helper function to get VLAN, TPID ot Signature for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to get
 *  arg - The value returned
 * Returns:
 *      BCM_E_xxx
 */
static int
_bcm_dnx_rcpu_switch_vlan_tpid_sig_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    uint32 entry_handle_id, value;
    dbal_fields_e field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmSwitchRemoteCpuVlan:
            field = DBAL_FIELD_VLAN;
            break;
        case bcmSwitchRemoteCpuTpid:
            field = DBAL_FIELD_TPID;
            break;
        case bcmSwitchRemoteCpuSignature:
            field = DBAL_FIELD_SIGNATURE;
            break;
        case bcmSwitchRemoteCpuEthertype:
            field = DBAL_FIELD_ETHERTYPE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, &value));

    *arg = (int) value;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnx_rcpu_switch_mac_lo_set
 * Description:
 *      Set the low 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set the mac for
 *      mac_lo      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */
static int
_bcm_dnx_rcpu_switch_mac_lo_set(
    int unit,
    bcm_switch_control_t type,
    uint32 mac_lo)
{
    uint32 entry_handle_id, fieldval;
    dbal_fields_e field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Given control type select field.
     */
    switch (type)
    {
        case bcmSwitchRemoteCpuLocalMacNonOui:
            field = DBAL_FIELD_SRC_MAC_LO;
            break;
        case bcmSwitchRemoteCpuDestMacNonOui:
            field = DBAL_FIELD_DST_MAC_LO;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, &fieldval));

    fieldval = (fieldval & 0xff000000) | (mac_lo & 0xffffff);
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, fieldval);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnx_rcpu_switch_mac_hi_set
 * Description:
 *      Set the upper 24 bits of MAC address field for RCPU registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set upper MAC for
 *      mac_hi      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */
static int
_bcm_dnx_rcpu_switch_mac_hi_set(
    int unit,
    bcm_switch_control_t type,
    uint32 mac_hi)
{
    uint32 entry_handle_id, fieldval;
    dbal_fields_e field_lo, field_hi;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Given control type select register.
     */
    switch (type)
    {
        case bcmSwitchRemoteCpuLocalMacOui:
            field_lo = DBAL_FIELD_SRC_MAC_LO;
            field_hi = DBAL_FIELD_SRC_MAC_HI;
            break;
        case bcmSwitchRemoteCpuDestMacOui:
            field_lo = DBAL_FIELD_DST_MAC_LO;
            field_hi = DBAL_FIELD_DST_MAC_HI;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_lo, INST_SINGLE, &fieldval));
    fieldval = (fieldval & 0xffffff) | ((mac_hi & 0xff) << 24);
    dbal_entry_value_field32_set(unit, entry_handle_id, field_lo, INST_SINGLE, fieldval);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_hi, INST_SINGLE, &fieldval));
    fieldval = (mac_hi >> 8) & 0xffff;
    dbal_entry_value_field32_set(unit, entry_handle_id, field_hi, INST_SINGLE, fieldval);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_rcpu_switch_mac_lo_get
 * Description:
 *      Get the lower 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the lower MAC address
 * Returns:
 *      BCM_E_xxxx
 */
static int
_bcm_dnx_rcpu_switch_mac_lo_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    uint32 entry_handle_id, fieldval;
    dbal_fields_e field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Given control type select field.
     */
    switch (type)
    {
        case bcmSwitchRemoteCpuLocalMacNonOui:
            field = DBAL_FIELD_SRC_MAC_LO;
            break;
        case bcmSwitchRemoteCpuDestMacNonOui:
            field = DBAL_FIELD_DST_MAC_LO;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, &fieldval));

    *arg = (fieldval << 8) >> 8;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_rcpu_switch_mac_hi_get
 * Description:
 *      Get the upper 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the upper MAC address
 * Returns:
 *      BCM_E_xxxx
 */
static int
_bcm_dnx_rcpu_switch_mac_hi_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    uint32 entry_handle_id, fieldval;
    dbal_fields_e field_lo, field_hi, mac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Given control type select register.
     */
    switch (type)
    {
        case bcmSwitchRemoteCpuLocalMacOui:
            field_lo = DBAL_FIELD_SRC_MAC_LO;
            field_hi = DBAL_FIELD_SRC_MAC_HI;
            break;
        case bcmSwitchRemoteCpuDestMacOui:
            field_lo = DBAL_FIELD_DST_MAC_LO;
            field_hi = DBAL_FIELD_DST_MAC_HI;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type=%d\r\n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENCAP_CTRL, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_lo, INST_SINGLE, &fieldval));
    mac = fieldval >> 24;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_hi, INST_SINGLE, &fieldval));
    mac |= fieldval << 8;

    *arg = (int) mac;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif /* #if defined(BCM_RCPU_SUPPORT) */

/**
 * see bcm_int/dnx/switch/switch.h
 */
shr_error_e
dnx_switch_network_group_id_verify(
    int unit,
    int is_ingress,
    bcm_switch_network_group_t network_group_id)
{

    uint32 nof_network_group_id;

    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress == TRUE)
    {
        nof_network_group_id = dnx_data_lif.in_lif.nof_in_lif_incoming_orientation_get(unit);
    }
    else
    {
        nof_network_group_id = dnx_data_lif.out_lif.nof_out_lif_outgoing_orientation_get(unit);
    }

    if ((network_group_id < 0) || (network_group_id >= nof_network_group_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error - network_group_id = %d, is out of rang [0:%d], is_ingress = %d.\n",
                     network_group_id, (nof_network_group_id - 1), is_ingress);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get the trap context port profile and the switch header type of out port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] trap_context_port_profile - Trap context port profile
 * \param [out] switch_header_type - Switch port header type
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
dnx_switch_header_type_out_get(
    int unit,
    bcm_port_t port,
    uint32 *trap_context_port_profile,
    int *switch_header_type)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 prp_context_port_profile;
    
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                               trap_context_port_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                               &prp_context_port_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (*trap_context_port_profile)
    {
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL:
        {
            *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
            break;
        }
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_RAW_PORT:
        {
            *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
            break;
        }
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_CPU_PORT:
        {
            if (prp_context_port_profile == DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_KEEP_ORIG_ENC_ON_TRAPPED)
            {
                *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU;
            }
            else
            {
                *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_CPU;
            }
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid trap_context_port_profile=%d \n", *trap_context_port_profile);
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the prt profile and the switch header type of in port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] prt_profile - Prt profile
 * \param [out] switch_header_type - Switch port header type
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
dnx_switch_header_type_in_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e * prt_profile,
    int *switch_header_type)
{
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;
    uint32 in_switch_header_type;
    uint32 out_switch_header_type;
    bcm_trunk_t trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TRUNK(port))
    {
        trunk_id = BCM_GPORT_TRUNK_GET(port);
        SHR_IF_ERR_EXIT(dnx_trunk_header_types_get(unit, trunk_id, &in_switch_header_type, &out_switch_header_type));
        *switch_header_type = in_switch_header_type;
    }
    else
    {
        /** Get the Prt ptc profile */
        SHR_IF_ERR_EXIT(dnx_port_ptc_profile_get(unit, port, prt_profile, &llr_profile));

        switch (*prt_profile)
        {
            case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET:
            {
                *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
                break;
            }
            case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2:
            case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1:
            {
                if (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), port))
                {
                    if (*prt_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1)
                    {
                        *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE;
                    }
                    else
                    {
                        *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
                    }
                }
                else
                {
                    if (*prt_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1)
                    {
                      *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE;
                    }
                    else
                    {
                      *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
                    }
                }
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid prt_profile =%d \n", *prt_profile);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the prt profile and the switch header type of port for direction both (in,out)
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] prt_profile - Prt profile
 * \param [out] switch_header_type - Switch port header type
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
dnx_switch_header_type_both_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e * prt_profile,
    int *switch_header_type)
{
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;
    uint32 in_switch_header_type;
    uint32 out_switch_header_type;
    bcm_trunk_t trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    *prt_profile = 0;
    if (BCM_GPORT_IS_TRUNK(port))
    {
        trunk_id = BCM_GPORT_TRUNK_GET(port);
        SHR_IF_ERR_EXIT(dnx_trunk_header_types_get(unit, trunk_id, &in_switch_header_type, &out_switch_header_type));
        if ((in_switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
            && (out_switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH))
        {
            *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Direction BOTH (in and out) API is only avaiable for ETH header type trunk gports. Other types are non-directional. Please use bcm_switch_control_indexed_port_get API with direction IN or OUT instead. \n");
        }
    }
    else
    {
        /** Get the Prt ptc profile */
        SHR_IF_ERR_EXIT(dnx_port_ptc_profile_get(unit, port, prt_profile, &llr_profile));

        if (*prt_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET)
        {
            *switch_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Direction BOTH (in and out) API is only avaiable for ETH header type ports. Other types are non-directional. Please use bcm_switch_control_indexed_port_get API with direction IN or OUT instead. \n");
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function used in dnx_switch_header_type_set_verify and dnx_switch_header_type_get_verify
 *   Used for verifying the direction and the used port
 */
static shr_error_e
dnx_switch_port_direction_verify(
    int unit,
    bcm_port_t port,
    int direction,
    int switch_header_type)
{
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(port))
    {
        /** Get the port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    }

    if ((direction != DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN) && (direction != DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT)
        && (direction != DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Direction %d is not supported! It can be only BOTH (0), IN (1) or OUT (2)!",
                     direction);
    }

    if (!BCM_GPORT_IS_TRUNK(port))
    {
        if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN) && (!DNX_ALGO_PORT_TYPE_IS_ING_PP(unit, port_type)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is not ingress port! Direction IN can be used only for ingress ports!",
                         port);
        }

        if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT) && (!DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_type)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is not egress port! Direction OUT can be used only for egress ports!",
                         port);
        }

        if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH)
            && ((!DNX_ALGO_PORT_TYPE_IS_ING_PP(unit, port_type)) || (!DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_type))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is not supported on both IN and OUT direction!", port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_switch_header_type_get
 */
static shr_error_e
dnx_switch_header_type_get_verify(
    int unit,
    bcm_port_t port,
    int direction,
    int *switch_header_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(switch_header_type, _SHR_E_PARAM, "switch_header_type");
    SHR_IF_ERR_EXIT(dnx_switch_port_direction_verify(unit, port, direction, *switch_header_type));

exit:
    SHR_FUNC_EXIT;
}

/** See .h for reference */
int
dnx_switch_header_type_get(
    int unit,
    bcm_port_t port,
    int direction,
    int *switch_header_type)
{
    dbal_enum_value_field_port_termination_ptc_profile_e prt_profile;
    uint32 trap_context_port_profile;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        port = gport_info.local_port;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_switch_header_type_get_verify(unit, port, direction, switch_header_type));

    switch (direction)
    {
        case DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN:
        {
            SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, port, &prt_profile, switch_header_type));
            break;
        }
        case DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT:
        {
            SHR_IF_ERR_EXIT(dnx_switch_header_type_out_get(unit, port, &trap_context_port_profile, switch_header_type));
            break;
        }
        case DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH:
        {
            SHR_IF_ERR_EXIT(dnx_switch_header_type_both_get(unit, port, &prt_profile, switch_header_type));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid direction =%d! It can be both (0), in (1) or out (2). \n", direction);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve Port-attributes according to key
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] key - Key index and key type
 * \param [out] value - Value that should be retrieved
 *
 * bcmSwitchPortHeaderType:
 * key.index can be 0, 1 and 2. 0 means both directions (in and out), 1 means 'in' and 2 means 'out'.
 * value indicates the port header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_control_indexed_port_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t * value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key.type)
    {
        case bcmSwitchPortHeaderType:
        {
            SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port, key.index, &value->value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d is not supported!", key.type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API gets info for port-specific and device-wide operating
*   modes
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] port - port
*   \param [in] type - switch control type
*   \param [out] arg  - value
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *
* \see
*   * None
*/
int
bcm_dnx_switch_control_port_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int *arg)
{
    int core;
    uint32 pp_port;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;

    SHR_FUNC_INIT_VARS(unit);

    value.value = 0;
    key.type = type;
    key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH;

    /** local to pp port + core */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &core, &pp_port));

    switch (type)
    {
        case bcmSwitchMeterAdjust:
        {
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_ingress_port_compensation_delta_get(unit, port, arg));
            break;
        }
        case bcmSwitchPrependTagEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_prepend_tag_get(unit, port, arg));
            break;
        }
        case bcmSwitchTagPcpDeiSrc:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_tag_pcp_dei_src_get(unit, port, arg));
            break;
        }
        case bcmSwitchPortHeaderType:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_get(unit, port, key, &value));
            *arg = value.value;
            break;
        }
        case bcmSwitchBfdMyDipDestination:
        {
            SHR_IF_ERR_EXIT(dnx_bfd_my_dip_destination_get(unit, (uint32 *) &value.value));
            *arg = value.value;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Switch control not supported!, %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_switch_header_type_set
 */
static shr_error_e
dnx_switch_header_type_set_verify(
    int unit,
    bcm_port_t port,
    int direction,
    int switch_header_type)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t local_port;
    dnx_algo_port_type_e port_type;
    int prd_enable_mode;
    int system_headers_mode = 0;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if ((switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_CPU)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_RAW)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU)
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Switch header type (%d) is not supported for system_headers_mode=%d!",
                     switch_header_type, system_headers_mode);
    }

    SHR_IF_ERR_EXIT(dnx_switch_port_direction_verify(unit, port, direction, switch_header_type));

    
    if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN) && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Switch header type (%d) for direction IN is not supported for system_headers_mode=%d!",
                     switch_header_type, system_headers_mode);
    }

    if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT) && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_CPU)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_RAW)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Switch header type (%d) for direction OUT is not supported!", switch_header_type);
    }

    
    if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH)
        && (switch_header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Switch header type (%d) for direction BOTH is not supported!",
                     switch_header_type);
    }

    /** Verify PRD is disabled for NIF ports */
    
    if (!SOC_IS_J2C(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));

        BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, local_port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE, FALSE))
            {
                SHR_IF_ERR_EXIT(imb_prd_enable_get(unit, local_port, &prd_enable_mode));
                if (prd_enable_mode != bcmCosqIngressPortDropDisable)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Switch header type cannot be configured if PRD is enabled on local port %d.\nPlease disable PRD.\n",
                                 local_port);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure general Port-attributes according to key and value
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] direction - Port direction
 * \param [in] switch_header_type - Header type that should be set
 *
 * Port direction can be 0, 1 and 2. 0 means both directions (in and out), 1 means 'in' and 2 means 'out'.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_switch_header_type_set(
    int unit,
    bcm_port_t port,
    int direction,
    int switch_header_type)
{

    bcm_port_t logical_port;
    uint32 header_mode;
    bcm_trunk_t trunk_id;
    uint32 in_switch_header_type;
    uint32 out_switch_header_type;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        logical_port = gport_info.local_port;
        port = gport_info.local_port;

    }

    SHR_INVOKE_VERIFY_DNX(dnx_switch_header_type_set_verify(unit, port, direction, switch_header_type));
    logical_port = port;
    trunk_id = 0;
    if (!BCM_GPORT_IS_TRUNK(port))
    {
        /** Get the port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    }
    if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
    {
        header_mode = DNX_ALGO_PORT_HEADER_MODE_ETH;
    }
    else if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW)
    {
        header_mode = DNX_ALGO_PORT_HEADER_MODE_MPLS_RAW;
    }
    else if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
             || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
    {
        header_mode = DNX_ALGO_PORT_HEADER_MODE_INJECTED_2_PP;
    }
    else
    {
        header_mode = DNX_ALGO_PORT_HEADER_MODE_NON_PP;
    }
    if (BCM_GPORT_IS_TRUNK(port))
    {
        trunk_id = BCM_GPORT_TRUNK_GET(port);
    }

    if (direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH || direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN)
    {
        if (!BCM_GPORT_IS_TRUNK(port) && (port_type != DNX_ALGO_PORT_TYPE_RCY_MIRROR))
        {
            /*
             * Port Ethernet bitmap is considered only for incoming port
             * RCY_MIRROR ports are internal ports and should not be added to Ethernet bitmap
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_eth_bitmap_set(unit, logical_port, header_mode));
        }
        if (port_type != DNX_ALGO_PORT_TYPE_RCY_MIRROR)
        {
            /*
             * Configure Ingress Ethernet properties
             * For mirror ports no Ethernet properties should be configured
             */
            SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_set(unit, port, switch_header_type));
        }
        if (!BCM_GPORT_IS_TRUNK(port))
        {
            /** Configure Port Termination PTC Profile according to the switch header type */
            SHR_IF_ERR_EXIT(dnx_port_pp_prt_ptc_profile_set(unit, port, switch_header_type));
        }

        if (BCM_GPORT_IS_TRUNK(port))
        {
            SHR_IF_ERR_EXIT(dnx_trunk_header_types_get
                            (unit, trunk_id, &in_switch_header_type, &out_switch_header_type));
            in_switch_header_type = switch_header_type;
            SHR_IF_ERR_EXIT(dnx_trunk_header_types_set(unit, trunk_id, in_switch_header_type, out_switch_header_type));
        }
    }
    /** Configure the outgoing port */
    if ((direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT || direction == DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH)
        && DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_type))
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_egress_set(unit, port, switch_header_type));

        if (BCM_GPORT_IS_TRUNK(port))
        {
            SHR_IF_ERR_EXIT(dnx_trunk_header_types_get
                            (unit, trunk_id, &in_switch_header_type, &out_switch_header_type));
            out_switch_header_type = switch_header_type;
            SHR_IF_ERR_EXIT(dnx_trunk_header_types_set(unit, trunk_id, in_switch_header_type, out_switch_header_type));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure general Port-attributes according to key and value
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] key - Key index and key type.
 * \param [in] value - Value that should be set
 * bcmSwitchPortHeaderType:
 * key.index can be 0, 1 and 2. 0 means both directions (in and out), 1 means 'in' and 2 means 'out'.
 * value indicates the port header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_control_indexed_port_set(
    int unit,
    bcm_port_t port,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key.type)
    {
        case bcmSwitchPortHeaderType:
        {
            SHR_IF_ERR_EXIT(dnx_switch_header_type_set(unit, port, key.index, value.value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d is not supported!", key.type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*   API gets info for port-specific and device-wide operating
*   modes
*
* Use cases:
*
* 1.
* Description
* Parameters:
* - type -
* - arg -
*
* 130.
* Meter - set ingress compensation delta value per port
* Parameters:
* - type - bcmSwitchMeterAdjust
* - arg -  delta value in range (-64) - (+63)
*
* 739.
* Sets TM port header type.
* Parameters:
* -type - bcmSwitchPortHeaderType
* -arg - BCM_SWITCH_PORT_HEADER_TYPE_ETH, BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP, BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW
*
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] port - port
*   \param [in] type - switch control type
*   \param [in] arg  - value
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *
* \see
*   * None
*/
int
bcm_dnx_switch_control_port_set(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int arg)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    SHR_FUNC_INIT_VARS(unit);

    key.type = type;
    key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH;
    value.value = arg;

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmSwitchMeterAdjust:
        {
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_ingress_port_compensation_delta_set(unit, port, arg));
            break;
        }
        case bcmSwitchPrependTagEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_prepend_tag_set(unit, port, arg));
            break;
        }
        case bcmSwitchTagPcpDeiSrc:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_tag_pcp_dei_src_set(unit, port, arg));
            break;
        }
        case bcmSwitchPortHeaderType:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, port, key, value));
            break;
        }
        case bcmSwitchBfdMyDipDestination:
        {
            SHR_IF_ERR_EXIT(dnx_bfd_my_dip_destination_set(unit, value.value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Switch control not supported!, %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * performs a full sync
 */

static int
_bcm_dnx_switch_control_sync(
    int unit,
    int arg)
{
    int rv = _SHR_E_NONE;
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)

    rv = mdb_lpm_db_kaps_sync(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = kbp_mngr_wb_sync(unit);
    SHR_IF_ERR_EXIT(rv);

#endif

    rv = bcm_common_sat_wb_sync(unit, 0);
    SHR_IF_ERR_EXIT(rv);

#if defined(BCM_WARM_BOOT_SUPPORT) && defined (BCM_SAT_SUPPORT)
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SAT_ENABLE, 0))
    {
        rv = soc_sat_wb_sync(unit, 0);
        SHR_IF_ERR_EXIT(rv);
    }
#endif

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm))
    {
        soc_cmic_uc_msg_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_NOHALT);
    }
#endif /* BCM_CMICM_SUPPORT */

    /*
     * Take counter processor's dma fifos lock. mainly to not allow changing the state while main thread is syncing
     */
    rv = dnx_crps_fifo_state_lock_take(unit);
    SHR_IF_ERR_EXIT(rv);
    lock_taken = TRUE;
    /*
     * commit the scache to persistent storage (all scache buffers)
     */
    rv = soc_scache_commit(unit);
    SHR_IF_ERR_EXIT(rv);

    /*
     * Mark scache as clean
     */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 0;
    SOC_CONTROL_UNLOCK(unit);

    SHR_IF_ERR_EXIT(rv);

exit:
    if (lock_taken)
    {
        /*
         * Give counter processor's dma fifos lock.
         */
        rv = dnx_crps_fifo_state_lock_give(unit);
        SHR_IF_ERR_EXIT(rv);
    }
#ifdef BCM_WARM_BOOT_API_TEST
    /* don't perform wb reset test on switch control API that was called for wb sync */
    dnxc_wb_test_mode_skip_wb_sequence(unit);
#endif /* BCM_WARM_BOOT_API_TEST */
    SHR_FUNC_EXIT;
}

#define DNX_SWITCH_CONTROL_SYNC_STATE (0x1) /** flag which indicates to sync the sw state */
#define DNX_SWITCH_CONTROL_SYNC_CRPS_DISABLE (0x2) /** flag which indicates to disable/stop CRPS activities that may write into the sw state */
#define DNX_SWITCH_CONTROL_SYNC_COUNTER_DISABLE (0x3) /** flag which indicates to disable/stop CRPS activities that may write into the sw state */

#endif /* BCM_WARM_BOOT_SUPPORT */

/* Manual sync + stop eviction is made by API bcm_switch_control_set
 * (type=bcmSwitchControlSync, arg= BCM_SWITCH_CONTROL_SYNC_FULL)
 * this call should be made immediately before exiting the driver
 * the flag DNX_SWITCH_CONTROL_SYNC_CRPS_DISABLE is included in BCM_SWITCH_CONTROL_SYNC_FULL
 * so in this case, the eviction will be stopped before the sync is made
 */

static int
dnx_switch_control_sync_handle(
    int unit,
    int arg)
{
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (arg & DNX_SWITCH_CONTROL_SYNC_CRPS_DISABLE)
    {
        /** disable the eviction from crps engine, read all counters and disconnect the DMA_FIFO and bg thread. */
        /** After this operation, crps counters will not update the sw state and sync can be made */
        rv = dnx_crps_eviction_disable_wrapper(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    if (arg & DNX_SWITCH_CONTROL_SYNC_COUNTER_DISABLE)
    {
        /** disable the eviction from crps engine, read all counters and disconnect the DMA_FIFO and bg thread. */
        /** After this operation, crps counters will not update the sw state and sync can be made */
        rv = soc_counter_stop(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    if (!SOC_WARM_BOOT(unit))
    {
        rv = _bcm_dnx_switch_control_sync(unit, arg);
        SHR_IF_ERR_EXIT(rv);
    }
#else
    rv = _SHR_E_UNAVAIL;
    SHR_IF_ERR_EXIT(rv);
#endif

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configures traverse mode of the MDB-LEM. Move from different bulk states and normal traverse. In bulk traverse the rules are set and then the
 *        flush starts. In normal mode the flush machine is running after each rule that is set.
 */
static shr_error_e
dnx_switch_traverse_mode_set(
    int unit,
    int flush_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flush_mode)
    {
        case bcmSwitchTableUpdateNormal:
        {
            SHR_IF_ERR_EXIT(dbal_bulk_mode_set(unit, DBAL_ACCESS_METHOD_MDB, DBAL_BULK_MODULE_MDB_LEM_FLUSH, 0));
            break;
        }
        case bcmSwitchTableUpdateRuleAdd:
        {
            SHR_IF_ERR_EXIT(dbal_bulk_mode_set(unit, DBAL_ACCESS_METHOD_MDB, DBAL_BULK_MODULE_MDB_LEM_FLUSH, 1));
            break;
        }
        case bcmSwitchTableUpdateRuleClear:
        {
            SHR_IF_ERR_EXIT(dbal_bulk_clear(unit, DBAL_ACCESS_METHOD_MDB, DBAL_BULK_MODULE_MDB_LEM_FLUSH));
            break;
        }
        case bcmSwitchTableUpdateRuleCommit:
        {
            SHR_IF_ERR_EXIT(dbal_bulk_commit(unit, DBAL_ACCESS_METHOD_MDB, DBAL_BULK_MODULE_MDB_LEM_FLUSH));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flush mode");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Sync function for KBP (External TCAM)
* \param [in] unit        - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None.
*/
static int
dnx_switch_control_external_tcam_sync_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Apply the ACL configuration
     */
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_set(unit));
    /*
     * Sync (Lock) the External Tcam (kbp) device
     */
    SHR_IF_ERR_EXIT(kbp_mngr_sync(unit));

exit:
    SHR_FUNC_EXIT;
}
#if defined(INCLUDE_KBP)
/**
* \brief
*  Init/De-init function for KBP (External TCAM)
* \param [in] unit        - Device Id
* \param [in] arg         - Init or De-init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None.
*/
static int
dnx_switch_control_external_tcam_init_deinit_handle(
    int unit,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (arg)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_init(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_kbp_deinit(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
#endif
/**
 * \brief - Set routed learning mode for supported applications according to input flags
 *
 * \param [in] unit - unit Id
 * \param [in] arg - input flags (BCM_SWITCH_CONTROL_L3_LEARN_XXX)
 *
 * \return
 *   shr_error_e - Error type
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_switch_control_routed_learn_set(
    int unit,
    int arg)
{
    uint32 entry_handle_id;
    char field_name[DBAL_MAX_STRING_LENGTH];
    char **field_name_tokens = NULL;
    unsigned int num_of_tokens;
    uint8 learn_enable;
    dbal_enum_value_field_ingress_opportunistic_learning_e opportunistic_learning;
    int learn_mode;
    int nof_fields;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Save router learning value (bitmap) in a SW table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARNING_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTED_LEARNING, INST_SINGLE, arg);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Routed learning is controlled by PEMLA_FWD1_CONTEXT_LEARN_ALLOWED table.
     * We are deciding which of it's fields should be set according to the field names.
     * Assumptions:
     * 1. all fields names include 3 sub-string separated by "___"
     * 2. 1st sub-string is always "FWD1"
     * 3. 2nd sub-string indicated the application type, e.g MPLS, IPV4, FCOE etc.
     * 4. 3rd sub-string indicates specific characteristics. in IPvX related fields, UC/MC indication exists in the sub-string.
     *
     * Going over all the field names and deciding according to the input flags which should be set.
     */
    /** Get table info (in order to go over the fields names) */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_FWD1_CONTEXT_LEARN_ALLOWED, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, DBAL_TABLE_PEMLA_FWD1_CONTEXT_LEARN_ALLOWED, &table));

    nof_fields = table->multi_res_info[0].nof_result_fields;

    for (int ii = 0; ii < nof_fields; ii++)
    {
        learn_enable = 0;
        sal_memset(field_name, 0, sizeof(field_name));

        /** Get sub-strings separated by "___" */
        sal_strncpy(field_name, dbal_field_to_string(unit, table->multi_res_info[0].results_info[ii].field_id),
                    sal_strlen(dbal_field_to_string(unit, table->multi_res_info[0].results_info[ii].field_id)));
        field_name_tokens = utilex_str_split(field_name, "___", 3, &num_of_tokens);
        /** validating assumption #1 */
        SHR_VAL_VERIFY(num_of_tokens, 3, _SHR_E_INTERNAL, "Unexpected field name structure");

        if (arg & BCM_SWITCH_CONTROL_L3_LEARN_IPV4_UC)
        {
            if (sal_strncmp("IPV4", field_name_tokens[1], sal_strlen(field_name_tokens[1])) == 0)
            {
                if (sal_strstr(field_name_tokens[2], "_UC") != NULL)
                {
                    learn_enable = 1;
                }
            }
        }
        if ((arg & BCM_SWITCH_CONTROL_L3_LEARN_IPV4_MC) && (learn_enable == 0))
        {
            if (sal_strncmp("IPV4", field_name_tokens[1], sal_strlen(field_name_tokens[1])) == 0)
            {
                if (sal_strstr(field_name_tokens[2], "MC_") != NULL)
                {
                    learn_enable = 1;
                }
            }
        }
        if ((arg & BCM_SWITCH_CONTROL_L3_LEARN_IPV6_UC) && (learn_enable == 0))
        {
            if (sal_strncmp("IPV6", field_name_tokens[1], sal_strlen(field_name_tokens[1])) == 0)
            {
                if (sal_strstr(field_name_tokens[2], "_UC") != NULL)
                {
                    learn_enable = 1;
                }
            }
        }
        if ((arg & BCM_SWITCH_CONTROL_L3_LEARN_IPV6_MC) && (learn_enable == 0))
        {
            if (sal_strncmp("IPV6", field_name_tokens[1], sal_strlen(field_name_tokens[1])) == 0)
            {
                if (sal_strstr(field_name_tokens[2], "MC_") != NULL)
                {
                    learn_enable = 1;
                }
            }
        }
        if ((arg & BCM_SWITCH_CONTROL_L3_LEARN_MPLS) && (learn_enable == 0))
        {
            if (sal_strncmp("MPLS", field_name_tokens[1], sal_strlen(field_name_tokens[1])) == 0)
            {
                learn_enable = 1;
            }
        }

        dbal_entry_value_field8_set(unit, entry_handle_id, table->multi_res_info[0].results_info[ii].field_id,
                                    INST_SINGLE, learn_enable);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Ingress learning mode must be set to opportunistic in 2 cases:
     * 1. Routed learning is enabled
     * 2. Egress learning mode is set
     */
    SHR_IF_ERR_EXIT(dnx_l2_learn_mode_get(unit, &learn_mode));

    /** check above conditions */
    if ((learn_mode & (BCM_L2_INGRESS_DIST | BCM_L2_INGRESS_CENT)) && (arg == BCM_SWITCH_CONTROL_L3_LEARN_NONE))
    {
        opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_PIPE_LEARNING;
    }
    else
    {
        opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING;
    }

    /*
     * Update the opportunistic learning (pipe or opportunistic)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OPPORTUNISTIC_LEARNING, INST_SINGLE,
                                 opportunistic_learning);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    if (field_name_tokens != NULL)
    {
        utilex_str_split_free(field_name_tokens, 3);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h for reference */
shr_error_e
dnx_switch_control_routed_learn_get(
    int unit,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 routed_learning;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** The value is saved in a SW table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARNING_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ROUTED_LEARNING, INST_SINGLE, &routed_learning);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *arg = routed_learning;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Receive the ID of the configured trap for a particular program in a given stage.
 * \param [in] unit - unit ID
 * \param [in] table_id - FWD1 or FWD2 table ID (stage)
 * \param [in] field_id - ID of the value field (program)
 * \param [out] trap_id - The trap_id found as result of the value field
 * \return
 *   shr_error_e - Error type
 */
shr_error_e
dnx_switch_fwd_not_found_trap_get_one(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int *trap_id)
{
    uint32 trap_dest;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, &trap_dest);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *trap_id = trap_dest;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the default forwarding destination for all programs in FWD1 and FWD2 to be the given trap ID
 * \param [in] unit - unit ID
 * \param [in] trap_id - the ID of the trap type (bcm_rx_trap_t) which will be configured as destination.
 * \return
 *   shr_error_e - Error type
 */
static shr_error_e
dnx_switch_fwd_not_found_trap_set(
    int unit,
    int trap_id)
{
    uint32 entry_handle_id;
    uint32 trap_dest;
    int idx;
    int trap_strength = dnx_data_trap.strength.default_trap_strength_get(unit);
    bcm_gport_t trap_gport;
    dbal_fields_e fwd1_fields[] =
        { /**DBAL_FIELD_FWD1___BIER___MPLS_FORWARDING,*/ DBAL_FIELD_FWD1___BIER___TI_FORWARDING,
        DBAL_FIELD_FWD1___FCOE___FCF_NO_VFT, DBAL_FIELD_FWD1___FCOE___FCF_NPV_NO_VFT,
        DBAL_FIELD_FWD1___FCOE___FCF_NPV_VFT, DBAL_FIELD_FWD1___FCOE___FCF_VFT, DBAL_FIELD_FWD1___FCOE___TRANSIT,
        DBAL_FIELD_FWD1___FCOE___TRANSIT_FIP, DBAL_FIELD_FWD1___MPLS___FWD, DBAL_FIELD_FWD1___MPLS___FWD_COUPLED,
        DBAL_FIELD_FWD1___MPLS___FWD_PER_INTERFACE
    };
    dbal_fields_e fwd2_fields[] = {
        DBAL_FIELD_FWD2___FCOE___FCF, DBAL_FIELD_FWD2___FCOE___FCF_FIP,
        DBAL_FIELD_FWD2___IPV4___MC_PRIVATE_WO_BF_WO_F2B,
        DBAL_FIELD_FWD2___IPV4___PRIVATE_UC, DBAL_FIELD_FWD2___IPV6___MC_PRIVATE_WO_BF_WO_F2B,
        DBAL_FIELD_FWD2___IPV6___PRIVATE_UC, DBAL_FIELD_FWD2___SRV6___PRIVATE_UC
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, 0);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, trap_gport, &trap_dest));
    /** Iterate over the result fields of FWD1 table and set the trap ID as destination */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_FWD1_DEFAULT_FORWARD_RESULT, &entry_handle_id));
    for (idx = 0; idx < sizeof(fwd1_fields) / sizeof(fwd1_fields[0]); idx++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, fwd1_fields[idx], INST_SINGLE, trap_dest);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Iterate over the result fields of the FWD2 table and set the trap ID as destination. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_FWD2_DEFAULT_FORWARD_RESULT, entry_handle_id));
    for (idx = 0; idx < sizeof(fwd2_fields) / sizeof(fwd2_fields[0]); idx++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, fwd2_fields[idx], INST_SINGLE, trap_dest);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive the default forwarding destination
 *  which has been set using dnx_switch_fwd_not_found_trap_set
 *  for all programs in FWD1 and FWD2.
 * \param [in] unit - unit ID
 * \param [out] trap_id - the ID of the trap which is set.
 * \return
 *   shr_error_e - Error type
 */
static shr_error_e
dnx_switch_fwd_not_found_trap_get(
    int unit,
    int *trap_id)
{
    uint32 entry_handle_id;
    uint32 trap_dest;
    uint32 trap_id_temp;
    uint32 trap_id_prev = bcmRxTrapDefault;
    int idx;
    bcm_gport_t trap_gport;
    dbal_fields_e fwd1_fields[] =
        { /** DBAL_FIELD_FWD1___BIER___MPLS_FORWARDING, */ DBAL_FIELD_FWD1___BIER___TI_FORWARDING,
        DBAL_FIELD_FWD1___FCOE___FCF_NO_VFT, DBAL_FIELD_FWD1___FCOE___FCF_NPV_NO_VFT,
        DBAL_FIELD_FWD1___FCOE___FCF_NPV_VFT, DBAL_FIELD_FWD1___FCOE___FCF_VFT, DBAL_FIELD_FWD1___FCOE___TRANSIT,
        DBAL_FIELD_FWD1___FCOE___TRANSIT_FIP, DBAL_FIELD_FWD1___MPLS___FWD, DBAL_FIELD_FWD1___MPLS___FWD_COUPLED,
        DBAL_FIELD_FWD1___MPLS___FWD_PER_INTERFACE
    };
    dbal_fields_e fwd2_fields[] = {
        DBAL_FIELD_FWD2___FCOE___FCF,
        DBAL_FIELD_FWD2___FCOE___FCF_FIP, DBAL_FIELD_FWD2___IPV4___MC_PRIVATE_WO_BF_WO_F2B,
        DBAL_FIELD_FWD2___IPV4___PRIVATE_UC, DBAL_FIELD_FWD2___IPV6___MC_PRIVATE_WO_BF_WO_F2B,
        DBAL_FIELD_FWD2___IPV6___PRIVATE_UC, DBAL_FIELD_FWD2___SRV6___PRIVATE_UC
    };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_FWD1_DEFAULT_FORWARD_RESULT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** Go over all fields in FWD1 and if any value is different than the expected one, return it. */
    for (idx = 0; idx < sizeof(fwd1_fields) / sizeof(fwd1_fields[0]); idx++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, fwd1_fields[idx], INST_SINGLE, &trap_dest));
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, trap_dest, &trap_gport));
        trap_id_temp = BCM_GPORT_TRAP_GET_ID(trap_gport);
        if (idx == 0)
        {
            trap_id_prev = trap_id_temp;
        }
        else if (trap_id_prev != trap_id_temp)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Expected the same default destination for all FWD1 fields, found %d and %d\n", trap_id_prev,
                         trap_id_temp);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_FWD2_DEFAULT_FORWARD_RESULT, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** Go over all fields in FWD2 and if any value is different than the expected one, return it. */
    for (idx = 0; idx < sizeof(fwd2_fields) / sizeof(fwd2_fields[0]); idx++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, fwd2_fields[idx], INST_SINGLE, &trap_dest));
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, trap_dest, &trap_gport));
        trap_id_temp = BCM_GPORT_TRAP_GET_ID(trap_gport);
        if (trap_id_prev != trap_id_temp)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Expected the same default destination for all FWD2 fields, found %d and %d\n", trap_id_prev,
                         trap_id_temp);
        }
    }
    *trap_id = trap_id_prev;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure port-specific and device-wide operating modes.
 *
 * Use cases:
 *
 * 1.
 * Description
 * Parameters:
 * - type -
 * - arg -
 *
 * 373. Scan all MAC addresses and decrement age-status in case that aging is disabled and age machine is not still working.
 * Parameters:
 * - type - bcmSwitchL2AgeScan
 *
 * 728.
 * Meter - enable disable fairness mechanism (adding random number to the bucket level) for all
 *         meter databases (ingress/egress/global) and for CBL and EBL buckets.
 * Parameters:
 * - type - bcmSwitchPolicerFairness
 * - arg -  TRUE/FALSE to enable/disable the feature
 *
 * 780.
 * Enable/disable the limit check when the CPU is adding a MACT entry.
 * Parameters:
 * - type - bcmSwitchL2LearnLimitToCpu
 * - arg -  '1' means limits are checked. '0' means limits are not forced for CPU add MACT entry.
 *
 * \param [in] unit - unit id
 * \param [in] type - configuration option taken from bcm_switch_control_t
 * \param [in] arg - argument for the configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    int rv = BCM_E_NONE;
    dnx_rx_trap_protocol_my_arp_ndp_config_t my_arp_ndp_config;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmSwitchECMPHashConfig:
        case bcmSwitchECMPSecondHierHashConfig:
        case bcmSwitchECMPThirdHierHashConfig:
        case bcmSwitchTrunkHashConfig:
        case bcmSwitchNwkHashConfig:
        {
            int index;
            dnx_switch_lb_tcam_key_t tcam_key;
            dnx_switch_lb_tcam_result_t tcam_result;
            dnx_switch_lb_valid_t tcam_valid;
            uint32 physical_client;

            index = 0;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_get(unit, index, &tcam_key, &tcam_result, &tcam_valid));
            /*
             * Convert logical client to physical so it can be loaded into tcam
             */
            SHR_IF_ERR_EXIT(dnx_switch_lb_logical_client_to_physical
                            (unit, (bcm_switch_control_t) type, &physical_client));
            tcam_result.client_x_crc_16_sel[physical_client] = (dnx_switch_lb_client_crc_sel_t) (arg);
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_set(unit, index, &tcam_key, &tcam_result, tcam_valid));
            break;
        }
        case bcmSwitchMplsStack0HashSeed:
        {
            dnx_switch_lb_general_seeds_t general_seeds;

            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
            general_seeds.mpls_stack_0 = (uint32) (arg);
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_set(unit, &general_seeds));
            break;
        }
        case bcmSwitchMplsStack1HashSeed:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "'bcmSwitchMplsStack1HashSeed' (%d) is, currently, unsupported", type);
            break;
        }
        case bcmSwitchParserHashSeed:
        {
            dnx_switch_lb_general_seeds_t general_seeds;

            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
            general_seeds.parser = (uint32) (arg);
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_set(unit, &general_seeds));
            break;
        }
        case bcmSwitchTraverseMode:
        {
            SHR_IF_ERR_EXIT(dnx_switch_traverse_mode_set(unit, arg));
            break;
        }
        case bcmSwitchL2LearnMode:
        {
            rv = dnx_l2_learn_mode_set(unit, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL2AgeScan:
        {
            rv = dnx_l2_age_scan_set(unit);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL2LearnLimitToCpu:
        {
            rv = dnx_l2_learn_limit_to_cpu_set(unit, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL2StaticMoveToCpu:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmSwitchL2StaticMoveToCpu is not supported.\n");
        }
        case bcmSwitchEtagEthertype:
        {
            uint16 etag_tpid = 0;
            if (arg > 0xFFFF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid tpid value = 0x%x, bigger than the 0xFFFF.\n", arg);
            }
            etag_tpid = (uint16) arg;
            rv = dnx_port_sit_tag_swap_tpid_set(unit, DNX_TAG_SWAP_TPID_INDEX_DOT1BR, etag_tpid);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL3UrpfDefaultRoute:
        {
            int is_uc = 1;
            if (arg != 0 && arg != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control bcmSwitchL3UrpfDefaultRoute. Supported values are 0(off) and 1(on).\n",
                             arg);
            }
            rv = dnx_l3_rpf_default_route_set(unit, is_uc, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchMeterMef10dot3Enable:
        {
            /** default mode - MEF 10.3 enable - user can disable it and
             *  work in mode 10.2 for ingress/egress */
            if (arg != 0 && arg != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control bcmSwitchMeterMef10dot3Enable. Supported values are 0(disable) and 1(enable).\n",
                             arg);
            }
            rv = dnx_policer_mgmt_mef_10_3_set(unit, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL3McRpfDefaultRoute:
        {
            int is_uc = 0;
            if (arg != 0 && arg != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control bcmSwitchL3McRpfDefaultRoute. Supported values are 0(off) and 1(on).\n",
                             arg);
            }
            rv = dnx_l3_rpf_default_route_set(unit, is_uc, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchMplsPWControlWord:
        {
            if ((!dnx_data_mpls.general.mpls_control_word_supported_get(unit)) && (arg != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value 0x%x provided to switch control bcmSwitchMplsPWControlWord. Supported value is 0.\n",
                             arg);
            }
            SHR_IF_ERR_EXIT(dnx_mpls_cw_set(unit, arg));
            break;
        }
        /************************************************************************/
            /*
             * RCPU
             */
        /************************************************************************/
#if defined(BCM_RCPU_SUPPORT)
        case bcmSwitchRemoteCpuSchanEnable:
        case bcmSwitchRemoteCpuMatchLocalMac:
        case bcmSwitchRemoteCpuCmicEnable:
        case bcmSwitchRemoteCpuMatchVlan:
        {
            if ((arg != 0x0) && (arg != 0x1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control type=%d. Supported values are 0(disable) and 1(enable).\n",
                             arg, type);
            }
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_enable_set(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuLocalMacOui:
        case bcmSwitchRemoteCpuDestMacOui:
        {
            if ((arg < 0x0) || (arg > 0xffffff))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control type=%d. Supported values are 0~0xffffff.\n",
                             arg, type);
            }
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_mac_hi_set(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuLocalMacNonOui:
        case bcmSwitchRemoteCpuDestMacNonOui:
        {
            if ((arg < 0x0) || (arg > 0xffffff))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control type=%d. Supported values are 0~0xffffff.\n",
                             arg, type);
            }
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_mac_lo_set(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuEthertype:
        case bcmSwitchRemoteCpuVlan:
        case bcmSwitchRemoteCpuTpid:
        case bcmSwitchRemoteCpuSignature:
        {
            if ((arg < 0x0) || (arg > 0xffff))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid value %d provided to switch control type=%d. Supported values are 0~0xffff.\n",
                             arg, type);
            }
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_vlan_tpid_sig_set(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
#endif /* #if defined(BCM_RCPU_SUPPORT) */

        case bcmSwitchWarmBoot:
        case bcmSwitchStableSelect:
        case bcmSwitchStableSize:
        case bcmSwitchStableUsed:
            /*
             * all should be configured through config.bcm
             */
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Unsupported switch control");
        case bcmSwitchCrashRecoveryMode:
        case bcmSwitchCrTransactionStart:
        case bcmSwitchCrCommit:
        case bcmSwitchCrLastTransactionStatus:
        case bcmSwitchCrCouldNotRecover:
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Feature unavailable: crash recovery\n");
        case bcmSwitchControlAutoSync:
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Feature unavailable: autosync\n");
        case bcmSwitchControlSync:
        {
            SHR_IF_ERR_EXIT(dnx_switch_control_sync_handle(unit, arg));
            break;
        }

        case bcmSwitchMcastTrunkIngressCommit:
        {
            SHR_IF_ERR_EXIT(dnx_trunk_flip_active_configuration_selector(unit));
            break;
        }

        case bcmSwitchMcastTrunkEgressCommit:
        {
            SHR_IF_ERR_EXIT(dnx_trunk_active_configuration_to_stand_by_configuration_copy(unit));
            break;
        }

        case bcmSwitchPolicerFairness:
        {
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_fairness_set(unit, arg));
            break;
        }
        case bcmSwitchCacheTableUpdateAll:
        {
            SHR_IF_ERR_EXIT(soc_sand_cache_table_update_all(unit));
            break;
        }

        case bcmSwitchL2StationExtendedMode:
        {
            SHR_IF_ERR_EXIT(dnx_l2_station_multi_my_mac_exem_mode_set(unit, arg));
            break;
        }
        case bcmSwitchMplsPipeTunnelLabelExpSet:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL,
                         "This configuration is redundant in current device. EXP in PIPE mode can be both SET and COPY according to mpls api flags configuration\n");
            break;
        }
        case bcmSwitchSynchronousPortClockSource:
            SHR_IF_ERR_EXIT(soc_dnx_synce_port_set(unit, 0, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceBkup:
            SHR_IF_ERR_EXIT(soc_dnx_synce_port_set(unit, 1, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceDivCtrl:
            SHR_IF_ERR_EXIT(soc_dnx_synce_divider_set(unit, 0, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
            SHR_IF_ERR_EXIT(soc_dnx_synce_divider_set(unit, 1, arg));
            break;
        /** Protocol traps additional switch control configuration */
        case bcmSwitchArpMyIp1:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            my_arp_ndp_config.ip_1 = arg;
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_set(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            break;
        case bcmSwitchArpMyIp2:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            my_arp_ndp_config.ip_2 = arg;
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_set(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            break;
        case bcmSwitchArpIgnoreDa:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            my_arp_ndp_config.ignore_da = arg;
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_set(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            break;
        case bcmSwitchNdpMyIp1:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            my_arp_ndp_config.ip_1 = arg;
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_set(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            break;
        case bcmSwitchNdpMyIp2:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            my_arp_ndp_config.ip_2 = arg;
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_set(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            break;
        case bcmSwitchIcmpIgnoreDa:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_icmp_ignore_da_hw_set(unit, arg));
            break;
        case bcmSwitchExternalTcamSync:
            SHR_IF_ERR_EXIT(dnx_switch_control_external_tcam_sync_handle(unit));
            break;
#if defined(INCLUDE_KBP)
        case bcmSwitchExternalTcamInit:
            SHR_IF_ERR_EXIT(dnx_switch_control_external_tcam_init_deinit_handle(unit, arg));
            break;
#endif
        case bcmSwitchL3UrpfMode:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmSwitchL3UrpfMode is not supported in DNX devices - global RPF mode is no longer supported, instead use per RIF option.\n");
            break;
        }
        case bcmSwitchL3McastL2:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmSwitchL3McastL2 is not supported in DNX devices - global L3McastL2 mode can no longer be configured, instead use per RIF option.\n");
            break;
        }
        case bcmSwitchDescCommit:
        {
            SHR_IF_ERR_EXIT(mdb_sbusdma_desc_wait_done(unit));
            break;
        }
        case bcmSwitchIngressRateLimitMpps:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_packet_rate_limit_set(unit, arg));
            break;
        }
        case bcmSwitchL3RoutedLearn:
        {
            SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_set(unit, arg));
            break;
        }
        case bcmSwitchEndToEndLatencyPerDestinationPort:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_per_port_set(unit, arg));
            break;
        }
        case bcmSwitchForwardLookupNotFoundTrap:
        {
            SHR_IF_ERR_EXIT(dnx_switch_fwd_not_found_trap_set(unit, arg));
            break;
        }
        case bcmSwitchFtmhEtherType:
        {
            /*
             * Set Ethertype for outgoing packets with FTMH forwarding header
             */
            SHR_IF_ERR_EXIT(dnx_init_pp_egress_current_next_protocol_config_table_configure(unit,
                                                                                            DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FTMH,
                                                                                            DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
                                                                                            arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This BCM switch control type (%d) is not supported! \n", type);
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_switch_control_get
 */
static shr_error_e
dnx_switch_control_get_verify(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the traverse mode of the flush machine. Normal or aggregate.
 *
 * \param [in] unit - unit id
 * \param [in] flush_mode - traverse mode bcmSwitchTableUpdateRuleAdd for aggregate or bcmSwitchTableUpdateNormal.
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
dnx_switch_traverse_mode_get(
    int unit,
    int *flush_mode)
{
    int collect_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_bulk_mode_get(unit, DBAL_ACCESS_METHOD_MDB, DBAL_BULK_MODULE_MDB_LEM_FLUSH, &collect_mode));

    if (collect_mode)
    {
        *flush_mode = bcmSwitchTableUpdateRuleAdd;
    }
    else
    {
        *flush_mode = bcmSwitchTableUpdateNormal;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port-specific and device-wide operating modes.
 *
 * Use cases:
 *
 * 1.
 * Description
 * Parameters:
 * - type -
 * - arg -
 *
 * 728.
 * Meter - enable disable fairness mechanism (adding random number to the bucket level) for all
 *         meter databases (ingress/egress/global) and for CBL and EBL buckets.
 * Parameters:
 * - type - bcmSwitchPolicerFairness
 * - arg -  TRUE/FALSE to enable/disable the feature
 *
 * 780.
 * Return whether the limits on MACT are forced on CPU MACT additions.
 * Parameters:
 * - type - bcmSwitchL2LearnLimitToCpu
 * - arg -  '1' means limits are checked. '0' means limits are not forced for CPU add MACT entry.
 *
 * \param [in] unit - Relevant unit
 * \param [in] type - Configuration option taken from bcm_switch_control_t
 * \param [out] arg - Argument for the configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    int rv = BCM_E_NONE;
    dnx_rx_trap_protocol_my_arp_ndp_config_t my_arp_ndp_config;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 flags;
#endif
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_control_get_verify(unit, type, arg));

    switch (type)
    {
        case bcmSwitchECMPHashConfig:
        case bcmSwitchECMPSecondHierHashConfig:
        case bcmSwitchECMPThirdHierHashConfig:
        case bcmSwitchTrunkHashConfig:
        case bcmSwitchNwkHashConfig:
        {
            int index;
            dnx_switch_lb_tcam_key_t tcam_key;
            dnx_switch_lb_tcam_result_t tcam_result;
            dnx_switch_lb_valid_t tcam_valid;
            uint32 physical_client;

            index = 0;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_get(unit, index, &tcam_key, &tcam_result, &tcam_valid));
            /*
             * Convert logical client to physical so it can be read from tcam
             */
            SHR_IF_ERR_EXIT(dnx_switch_lb_logical_client_to_physical
                            (unit, (bcm_switch_control_t) type, &physical_client));
            *arg = (int) (tcam_result.client_x_crc_16_sel[physical_client]);
            break;
        }
        case bcmSwitchMplsStack0HashSeed:
        {
            dnx_switch_lb_general_seeds_t general_seeds;

            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
            *arg = (int) (general_seeds.mpls_stack_0);
            break;
        }
        case bcmSwitchMplsStack1HashSeed:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "'bcmSwitchMplsStack1HashSeed' (%d) is, currently, unsupported", type);
            break;
        }
        case bcmSwitchParserHashSeed:
        {
            dnx_switch_lb_general_seeds_t general_seeds;

            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
            *arg = (int) (general_seeds.parser);
            break;
        }
        case bcmSwitchTraverseMode:
        {
            SHR_IF_ERR_EXIT(dnx_switch_traverse_mode_get(unit, arg));
            break;
        }
        case bcmSwitchCrashRecoveryMode:
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Feature unavailable: crash recovery\n");
        case bcmSwitchControlAutoSync:
            rv = BCM_E_UNAVAIL;
            break;
        case bcmSwitchControlSync:
            break;
#ifdef BCM_WARM_BOOT_SUPPORT
        case bcmSwitchStableSelect:
            rv = soc_stable_get(unit, arg, &flags);
            break;
        case bcmSwitchStableSize:
            rv = soc_stable_size_get(unit, arg);
            break;
        case bcmSwitchStableUsed:
            rv = soc_stable_used_get(unit, arg);
            break;
        case bcmSwitchWarmBoot:
            (*arg) = SOC_WARM_BOOT(unit);
            rv = _SHR_E_NONE;
            break;
#else /* BCM_WARM_BOOT_SUPPORT */
        case bcmSwitchStableSelect:
        case bcmSwitchStableSize:
        case bcmSwitchStableUsed:
        case bcmSwitchWarmBoot:
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Feature unavailable: warmboot\n");
#endif
        case bcmSwitchL2LearnMode:
        {
            SHR_IF_ERR_EXIT(dnx_l2_learn_mode_get(unit, arg));
            break;
        }
        case bcmSwitchL2LearnLimitToCpu:
        {
            rv = dnx_l2_learn_limit_to_cpu_get(unit, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchL2StaticMoveToCpu:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmSwitchL2StaticMoveToCpu is not supported.\n");
        }
        case bcmSwitchL3UrpfDefaultRoute:
        {
            int is_uc = 1;
            rv = dnx_l3_rpf_default_route_get(unit, is_uc, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchMeterMef10dot3Enable:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_mef_10_3_get(unit, arg));
            break;
        case bcmSwitchL3McRpfDefaultRoute:
        {
            int is_uc = 0;
            rv = dnx_l3_rpf_default_route_get(unit, is_uc, arg);
            SHR_IF_ERR_EXIT(rv);
            break;
        }
        case bcmSwitchMplsPWControlWord:
        {
            SHR_IF_ERR_EXIT(dnx_mpls_cw_get(unit, arg));
            break;
        }
        /************************************************************************/
            /*
             * RCPU
             */
        /************************************************************************/
#if defined(BCM_RCPU_SUPPORT)
        case bcmSwitchRemoteCpuSchanEnable:
        case bcmSwitchRemoteCpuMatchLocalMac:
        case bcmSwitchRemoteCpuCmicEnable:
        case bcmSwitchRemoteCpuMatchVlan:
        {
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_enable_get(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuLocalMacOui:
        case bcmSwitchRemoteCpuDestMacOui:
        {
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_mac_hi_get(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuLocalMacNonOui:
        case bcmSwitchRemoteCpuDestMacNonOui:
        {
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_mac_lo_get(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
        case bcmSwitchRemoteCpuEthertype:
        case bcmSwitchRemoteCpuVlan:
        case bcmSwitchRemoteCpuTpid:
        case bcmSwitchRemoteCpuSignature:
        {
            if (soc_feature(unit, soc_feature_rcpu_1))
            {
                rv = _bcm_dnx_rcpu_switch_vlan_tpid_sig_get(unit, type, arg);
                SHR_IF_ERR_EXIT(rv);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Feature unavailable: soc_feature_rcpu_1\r\n");
            }
            break;
        }
#endif /* #if defined(BCM_RCPU_SUPPORT) */

        case bcmSwitchMcastTrunkIngressCommit:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Feature unsupported for get\n");
            break;
        }

        case bcmSwitchMcastTrunkEgressCommit:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Feature unsupported for get\n");
            break;
        }

        case bcmSwitchPolicerFairness:
        {
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_fairness_get(unit, arg));
            break;
        }

        case bcmSwitchL2StationExtendedMode:
        {
            SHR_IF_ERR_EXIT(dnx_l2_station_multi_my_mac_exem_mode_get(unit, arg));
            break;
        }
        case bcmSwitchMplsPipeTunnelLabelExpSet:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL,
                         "This configuration is redundant in current device. EXP in PIPE mode can be both SET and COPY according to mpls api flags configuration\n");
            break;
        }
        case bcmSwitchSynchronousPortClockSource:
            SHR_IF_ERR_EXIT(soc_dnx_synce_port_get(unit, 0, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceBkup:
            SHR_IF_ERR_EXIT(soc_dnx_synce_port_get(unit, 1, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceDivCtrl:
            SHR_IF_ERR_EXIT(soc_dnx_synce_divider_get(unit, 0, arg));
            break;
        case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
            SHR_IF_ERR_EXIT(soc_dnx_synce_divider_get(unit, 1, arg));
            break;
        /** Protocol traps additional switch control configuration */
        case bcmSwitchArpMyIp1:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            *arg = my_arp_ndp_config.ip_1;
            break;
        case bcmSwitchArpMyIp2:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            *arg = my_arp_ndp_config.ip_2;
            break;
        case bcmSwitchArpIgnoreDa:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapArpMyIp, &my_arp_ndp_config));
            *arg = my_arp_ndp_config.ignore_da;
            break;
        case bcmSwitchNdpMyIp1:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            *arg = my_arp_ndp_config.ip_1;
            break;
        case bcmSwitchNdpMyIp2:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_my_arp_ndp_get(unit, bcmRxTrapNdpMyIp, &my_arp_ndp_config));
            *arg = my_arp_ndp_config.ip_2;
            break;
        case bcmSwitchIcmpIgnoreDa:
            SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_icmp_ignore_da_hw_get(unit, arg));
            break;
        case bcmSwitchL3UrpfMode:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmSwitchL3UrpfMode is not supported in DNX devices - global RPF mode is no longer supported, instead use per RIF option.\n");
            break;
        }
        case bcmSwitchL3McastL2:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmSwitchL3McastL2 is not supported in DNX devices - global L3McastL2 mode can no longer be configured, instead use per RIF option.\n");
            break;
        }
        case bcmSwitchEtagEthertype:
            SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_tpid_get(unit, DNX_TAG_SWAP_TPID_INDEX_DOT1BR, (uint16 *) arg));
            break;
        case bcmSwitchIngressRateLimitMpps:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_packet_rate_limit_get(unit, arg));
            break;
        }
        case bcmSwitchL3RoutedLearn:
        {
            SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, arg));
            break;
        }
        case bcmSwitchEndToEndLatencyPerDestinationPort:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_per_port_get(unit, arg));
            break;
        }
        case bcmSwitchForwardLookupNotFoundTrap:
        {
            SHR_IF_ERR_EXIT(dnx_switch_fwd_not_found_trap_get(unit, arg));
            break;
        }
        case bcmSwitchFtmhEtherType:
        {
            /*
             * Set Ethertype for outgoing packets with FTMH forwarding header
             */
            SHR_IF_ERR_EXIT(dnx_init_pp_egress_current_next_protocol_config_table_get(unit,
                                                                                      DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FTMH,
                                                                                      DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
                                                                                      arg));
            break;
        }

        case bcmSwitchExternalTcamSync:
        {
            dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

            /** check if bcm_switch_control_set() was called - return 1 if it was, 0 if not*/
            *arg = 0;
            SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
            if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
            {
                *arg = 1;
            }
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This BCM switch control type (%d) is not supported! \n", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_dnx_switch_network_group_config_set
*/
shr_error_e
dnx_switch_network_group_config_set_verify(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify config pointer is not NULL:
     */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /*
     * Verify source_network_group_id range:
     */
    SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, source_network_group_id));

    /*
     * Verify dest_network_group_id range:
     */
    SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, config->dest_network_group_id));

    /*
     * Verify that no flag other then BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE is set:
     */
    if (config->config_flags & (~BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error - config_flags = 0x%08X, Only BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE (0x%08X) is supported!!!\n",
                     config->config_flags, BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set Orientation-based filtering (Split-Horizon) between two orientations.
 *
 * \param [in] unit - Relevant unit
 * \param [in] source_network_group_id - incoming orientation id.
 * \param [in] config -pointer to structure containing
 *      dest_network_group_id - outgoing orientation id.
 *      config_flags - configuration flags.
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
bcm_dnx_switch_network_group_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t * config)
{
    uint32 entry_handle_id;
    uint32 filter_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_network_group_config_set_verify(unit, source_network_group_id, config));

    filter_value = (_SHR_IS_FLAG_SET(config->config_flags, BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE)) ? 1 : 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SPLIT_HORIZON_FILTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_ORIENTATION, source_network_group_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, config->dest_network_group_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_FILTER_ENABLE, INST_SINGLE,
                                 filter_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_dnx_switch_network_group_config_get
*/
shr_error_e
dnx_switch_network_group_config_get_verify(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify config pointer is not NULL:
     */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /*
     * Verify source_network_group_id range:
     */
    SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, source_network_group_id));

    /*
     * Verify dest_network_group_id range:
     */
    SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, config->dest_network_group_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the Orientation-based filtering (Split-Horizon) between two orientations.
 *
 * \param [in] unit - Relevant unit
 * \param [in] source_network_group_id - incoming orientation id.
 * \param [in,out] config -pointer to structure containing -
 *      [in] dest_network_group_id - outgoing orientation id.
 *      [out] config_flags - configuration flags.
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
bcm_dnx_switch_network_group_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t * config)
{
    uint32 entry_handle_id;
    uint32 filter_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_network_group_config_get_verify(unit, source_network_group_id, config));

    /*
     * Get Filter value from HW:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SPLIT_HORIZON_FILTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_ORIENTATION, source_network_group_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, config->dest_network_group_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_FILTER_ENABLE, INST_SINGLE,
                                        &filter_value);

    if (filter_value == 1)
    {
        config->config_flags |= BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    }
    else
    {
        config->config_flags &= ~BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
int
bcm_dnx_switch_rov_get(
    int unit,
    uint32 flags,
    uint32 *rov)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rov, _SHR_E_PARAM, "rov");

    SHR_IF_ERR_EXIT(soc_dnxc_avs_value_get(unit, rov));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API gets temperature info for PVT monitors
*   modes
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] temperature_max - Maximal number of acceptable temperature sensors
*   \param [out] temperature_array - Pointer to array to be loaded by this procedure. Each
*        element contains current temperature and peak temperature
*   \param [out] temperature_count  - the number of sensors (and, therefore, the number of valid
*        elements on temperature_array).
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *
* \see
*   * None
*/
int
bcm_dnx_switch_temperature_monitor_get(
    int unit,
    int temperature_max,
    bcm_switch_temperature_monitor_t * temperature_array,
    int *temperature_count)
{
    int nof_pvt_mon;
    int pvt_base, pvt_factor;
    uint32 entry_handle_id;
    uint16 current[DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS];
    uint16 peak[DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS];
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(temperature_array, _SHR_E_PARAM, "temperature_array");
    SHR_NULL_CHECK(temperature_count, _SHR_E_PARAM, "temperature_count");

    nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    pvt_base = dnx_data_pvt.general.pvt_base_get(unit);
    pvt_factor = dnx_data_pvt.general.pvt_factor_get(unit);

    if (temperature_max < nof_pvt_mon)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Array size should be equal to or larger than %d.\n", nof_pvt_mon);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_TEMPERATURE_MONITOR, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_THERMAL_DATA, i, &current[i]);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_PEAK_THERMAL_DATA, i, &peak[i]);
    }
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    for (i = 0; i < nof_pvt_mon; i++)
    {
        /*
         * curr [0.1 C] = (pvt_base - curr * pvt_factor)/10000
         */
        temperature_array[i].curr = (pvt_base - current[i] * pvt_factor) / 10000;
        /*
         * peak [0.1 C] = (pvt_base - curr * peak)/10000
         */
        temperature_array[i].peak = (pvt_base - peak[i] * pvt_factor) / 10000;
    }

    *temperature_count = nof_pvt_mon;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Get l3 protocol group for multiple my mac (vrrp)
 *
 * \param [in] unit - Relevant unit
 * \param [out] group_members - combination (flags) of l3 protocols (can be either input or output).
 * \param [out] group_id - group ID (can be either input or output)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_l3_protocol_group_get(
    int unit,
    uint32 *group_members,
    bcm_l3_protocol_group_id_t * group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_get(unit, group_members, group_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Set l3 protocol group for multiple my mac (vrrp)
 *
 * \param [in] unit - Relevant unit
 * \param [in] group_members - combination (flags) of l3 protocols.
 * \param [in] group_id - group ID to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_l3_protocol_group_set(
    int unit,
    uint32 group_members,
    bcm_l3_protocol_group_id_t group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_l3_vrrp_protocol_group_set(unit, group_members, group_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/***************************************************************/
/***************************************************************/
#define DNX_SWITCH_LOCK_TAKE
#define DNX_SWITCH_LOCK_RELEASE
/*
 * Switch Event Function
 *
 */

/*
 * Function:
 *    bcm_dnx_switch_event_register
 * Description:
 *    Registers a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *    cb          - The desired call back function to register for critical events.
 *    userdata    - Pointer to any user data to carry on.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 *
 *    Several call back functions could be registered, they all will be called upon
 *    critical event. If registered callback is called it is adviced to log the
 *    information and reset the chip.
 *    Same call back function with different userdata is allowed to be registered.
 */
int
bcm_dnx_switch_event_register(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SWITCH_LOCK_TAKE;
    rc = soc_event_register(unit, (soc_event_cb_t) cb, userdata);
    SHR_IF_ERR_EXIT(rc);

exit:
    DNX_SWITCH_LOCK_RELEASE;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *    bcm_dnx_switch_event_unregister
 * Description:
 *    Unregisters a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *    cb          - The desired call back function to unregister for critical events.
 *    userdata    - Pointer to any user data associated with a call back function
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 *
 *  If userdata = NULL then all matched call back functions will be unregistered,
 */
int
bcm_dnx_switch_event_unregister(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SWITCH_LOCK_TAKE;
    rc = soc_event_unregister(unit, (soc_event_cb_t) cb, userdata);
    SHR_IF_ERR_EXIT(rc);

exit:
    DNX_SWITCH_LOCK_RELEASE;
    SHR_FUNC_EXIT;
}

static int
dnx_switcht_event_blk_all_intr_enable_set(
    int unit,
    int intr_id,
    soc_interrupt_db_t *interrupts,
    int enable)
{
    int rc = _SHR_E_NONE;
    int blk, is_valid, intr_port, is_enable;
    soc_block_info_t *bi;
    uint32  old_intr_status[DNX_SWITCH_INTR_BLOCK_WIDTH];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * backup old intr enable status 
     */
    sal_memset(old_intr_status, 0, sizeof(old_intr_status));
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        if (!SOC_INFO(unit).block_valid[blk])
        {
            continue;
        }
        bi = &(SOC_BLOCK_INFO(unit, blk));
        SHR_IF_ERR_EXIT(soc_interrupt_is_valid(unit, bi, &(interrupts[intr_id]), &is_valid));
        if (is_valid)
        {
            intr_port = soc_interrupt_get_intr_port_from_index(unit, intr_id, bi->number);
            SHR_IF_ERR_EXIT(soc_interrupt_is_enabled(unit, intr_port, &(interrupts[intr_id]), &is_enable));
            if (is_enable)
            {
                SHR_BITSET(old_intr_status, blk);
            }
        }
    }

    /*
     * Set per interrupt 
     */
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        if (!SOC_INFO(unit).block_valid[blk])
        {
            continue;
        }
        bi = &(SOC_BLOCK_INFO(unit, blk));
        rc = soc_interrupt_is_valid(unit, bi, &(interrupts[intr_id]), &is_valid);
        SHR_IF_ERR_EXIT(rc);
        if (is_valid)
        {
            intr_port = soc_interrupt_get_intr_port_from_index(unit, intr_id, bi->number);
            rc = soc_interrupt_is_enabled(unit, intr_port, &(interrupts[intr_id]), &is_enable);
            SHR_IF_ERR_EXIT(rc);
            if (enable)
            {
                rc = soc_interrupt_enable(unit, intr_port, &(interrupts[intr_id]));
            }
            else
            {
                rc = soc_interrupt_disable(unit, intr_port, &(interrupts[intr_id]));
            }
            SHR_IF_ERR_EXIT(rc);
        }
    }

exit:
    if (rc != _SHR_E_NONE)
    {
        /*
         * error recorvery with saved intr status 
         */
        for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
        {
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }
            bi = &(SOC_BLOCK_INFO(unit, blk));
            rc = soc_interrupt_is_valid(unit, bi, &(interrupts[intr_id]), &is_valid);
            if ((rc == _SHR_E_NONE) && (is_valid))
            {
                intr_port = soc_interrupt_get_intr_port_from_index(unit, intr_id, bi->number);
                if (SHR_BITGET(old_intr_status, blk))
                {
                    rc = soc_interrupt_enable(unit, intr_port, &(interrupts[intr_id]));
                }
                else
                {
                    rc = soc_interrupt_disable(unit, intr_port, &(interrupts[intr_id]));
                }
            }
        }
    }
    SHR_FUNC_EXIT;
}

static int
dnx_switch_event_intr_sw_state_storms_set(
    int unit,
    soc_interrupt_db_t * interrupts,
    int event_id,
    bcm_switch_event_control_action_t action,
    uint32 value)
{
    int inter, nof_interrupts;
    int start_intr, end_intr;
    int blk;
    soc_block_info_t *bi = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * opt in for Error Recovery
    */
    DNX_ERR_RECOVERY_START(unit);
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    if (event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
    {
        start_intr = 0;
        end_intr = nof_interrupts - 1;
    }
    else
    {
        if ((event_id >= nof_interrupts) || (event_id < 0))
        {
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
        start_intr = event_id;
        end_intr = event_id;
    }

    /*
     * update sw_state 
     */
    if (action == bcmSwitchEventStormNominal)
    {
        if (event_id != BCM_SWITCH_EVENT_CONTROL_ALL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nominal storm detection parameter (event_id)");
        }
        /*
         * Warm boot buffer updating
         */
        SHR_IF_ERR_EXIT(intr_db.storm_nominal.set(unit, value));
        SOC_SWITCH_EVENT_NOMINAL_STORM(unit) = value;
    }
    else
    {
        for (inter = start_intr; inter <= end_intr; inter++)
        {
            if (action == bcmSwitchEventStormTimedCount)
            {
                SHR_IF_ERR_EXIT(intr_db.storm_timed_count.set(unit, inter, value));
            }
            else if (action == bcmSwitchEventStormTimedPeriod)
            {
                SHR_IF_ERR_EXIT(intr_db.storm_timed_period.set(unit, inter, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
        }
    }

    /*
     * clear storm time status at intr DB 
     */
    for (inter = start_intr; inter <= end_intr; inter++)
    {
        if (!SOC_REG_IS_VALID(unit, interrupts[inter].reg))
        {
            continue;
        }

        SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit, interrupts[inter].reg).block)
        {
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }

            bi = &(SOC_BLOCK_INFO(unit, blk));
            switch (action)
            {
                case bcmSwitchEventStormTimedCount:
                {
                    (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                    break;
                }
                case bcmSwitchEventStormTimedPeriod:
                {
                    (interrupts[inter].storm_detection_start_time)[bi->number] = 0;
                    break;
                }
                case bcmSwitchEventStormNominal:
                {
                    (interrupts[inter].storm_nominal_count)[bi->number] = 0x0;
                    (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                    break;
                }
                default:
                {
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
                }
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static int
dnx_switch_event_flags_value_update(
    int unit,
    bcm_switch_event_control_action_t action,
    uint32 value,
    uint32 *intr_flags)
{
    uint32 flags = *intr_flags;

    SHR_FUNC_INIT_VARS(unit);

    switch (action)
    {
        case bcmSwitchEventLog:
        {
            if (value == 0)
            {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            }
            else
            {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            }
            break;
        }
        case bcmSwitchEventCorrActOverride:
        {
            /*
             * Value - 0 : Only bcm callback will be called for this interrupt. 
             * Value - 1 : Only user callback will be called for this interrupt. At this mode 
             *             BCM driver will only print the interrupt information for logging.
             * Value - 2 : User call back will be called immediately after bcm callback.
             */
            if (value == 2)
            {
                flags |= SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
            }
            else
            {
                flags &= ~SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
            }
            if (value == 0)
            {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            }
            else
            {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            }
            break;
        }
        case bcmSwitchEventPriority:
        {
            if (value > SOC_INTERRUPT_DB_FLAGS_PRIORITY_MAX_VAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority value is out af range");
            }
            SHR_BITCOPY_RANGE(&flags, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB, &value, 0,
                              SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN);
            break;
        }
        case bcmSwitchEventUnmaskAndClearDisable:
        {
            if (value == 0)
            {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            }
            else
            {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            }
            break;
        }
        case bcmSwitchEventForceUnmask:
        {
            if (value == 0)
            {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            }
            else
            {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            }
            break;
        }
        default:
        {
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
    *intr_flags = flags;

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_switch_event_intr_sw_state_flags_set(
    int unit,
    int event_id,
    bcm_switch_event_control_action_t action,
    uint32 value)
{
    int inter, nof_interrupts;
    int start_intr, end_intr;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    if (event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
    {
        start_intr = 0;
        end_intr = nof_interrupts - 1;
    }
    else
    {
        if ((event_id >= nof_interrupts) || (event_id < 0))
        {
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
        start_intr = event_id;
        end_intr = event_id;
    }

    for (inter = start_intr; inter <= end_intr; inter++)
    {
        SHR_IF_ERR_EXIT(soc_interrupt_flags_get(unit, inter, &flags));
        SHR_IF_ERR_EXIT(dnx_switch_event_flags_value_update(unit, action, value, &flags));
        SHR_IF_ERR_EXIT(intr_db.flags.set(unit, inter, flags));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *    bcm_dnx_switch_event_control_set
 * Description:
 *    Set event control
 * Parameters:
 *    unit        - Device unit number
 *    type        - Event action.
 *    value       - Event value
 * Returns:
 *      _SHR_E_xxx
 */
int
bcm_dnx_switch_event_control_set(
    int unit,
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type,
    uint32 value)
{
    int rc = _SHR_E_NONE, nof_interrupts;
    soc_interrupt_db_t *interrupts;
    int inter = 0;
    soc_block_info_t *bi;
    int bi_index = 0, inf_index = type.index, port;
    int is_valid;
    soc_block_types_t block_types;

    SHR_FUNC_INIT_VARS(unit);
    DNX_SWITCH_LOCK_TAKE;

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "supports only interrupts event");
    }

    if (SOC_CONTROL(unit)->interrupts_info == NULL)
	{
		SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
	}
    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    /*
     * verify interrupt id
     */
    rc = soc_nof_interrupts(unit, &nof_interrupts);
    SHR_IF_ERR_EXIT(rc);

    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL)
    {
        if (type.event_id >= nof_interrupts || type.event_id < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid interrupt");
        }

        /*
         * verify block instance
         */
        if (!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid interrupt register for the device");
        }
        /*
         * Verify block index
         */
        if (type.index != BCM_CORE_ALL)
        {
            block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;
            rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
            SHR_IF_ERR_EXIT(rc);
            if (!is_valid)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported block instance");
            }

            inf_index = soc_interrupt_get_block_index_from_port(unit, type.event_id, type.index);
            if (inf_index < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported block instance");
            }
        }
    }

    /*
     * switch case for all
     */
    switch (type.action)
    {
        case bcmSwitchEventMask:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                rc = soc_dnxc_interrupt_all_enable_set(unit, (value ? 0 : 1));
                SHR_IF_ERR_EXIT(rc);
            }
            else
            {
                /*
                 * Set per interrupt
                 */
                if (type.index == BCM_CORE_ALL)
                {
                    if (value)
                    {
                        rc = dnx_switcht_event_blk_all_intr_enable_set(unit, type.event_id, interrupts, 0);
                    }
                    else
                    {
                        rc = dnx_switcht_event_blk_all_intr_enable_set(unit, type.event_id, interrupts, 1);
                    }
                }
                else
                {
                    if (value)
                    {
                        rc = soc_interrupt_disable(unit, type.index, &(interrupts[type.event_id]));
                    }
                    else
                    {
                        rc = soc_interrupt_enable(unit, type.index, &(interrupts[type.event_id]));
                    }
                }
                SHR_IF_ERR_EXIT(rc);
            }
            break;
        }
        case bcmSwitchEventForce:
        {
            /*
             * Set/clear per interrupt
             */
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                for (inter = 0; inter < nof_interrupts; inter++)
                {
                    for (bi_index = 0; SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++)
                    {
                        if (!SOC_INFO(unit).block_valid[bi_index])
                        {
                            continue;
                        }
                        bi = &(SOC_BLOCK_INFO(unit, bi_index));
                        rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                        SHR_IF_ERR_EXIT(rc);
                        if (is_valid)
                        {
                            port = soc_interrupt_get_intr_port_from_index(unit, inter, bi->number);
                            rc = soc_interrupt_force(unit, port, &(interrupts[inter]), 1 - value);
                            SHR_IF_ERR_EXIT(rc);
                        }
                    }
                }
            }
            else
            {
                rc = soc_interrupt_force(unit, type.index, &(interrupts[type.event_id]), 1 - value);
                SHR_IF_ERR_EXIT(rc);
            }
            break;
        }
        case bcmSwitchEventClear:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                if (value)
                {
                    rc = soc_interrupt_clear_all(unit);
                    SHR_IF_ERR_EXIT(rc);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid event clear parameter");
                }
            }
            else
            {
                /*
                 * Set per interrupt
                 */
                if (value)
                {
                    if (NULL == interrupts[type.event_id].interrupt_clear)
                    {
                        if (interrupts[type.event_id].vector_id == 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                                         "Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id");
                        }
                        else
                        {
                            LOG_WARN(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                                                                    "Warning: call to interrupt clear for vector pointer, nothing done\n")));
                        }
                    }
                    else
                    {
                        rc = interrupts[type.event_id].interrupt_clear(unit, type.index, type.event_id);
                        SHR_IF_ERR_EXIT(rc);
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid event clear parameter");
                }
            }
            break;
        }
        case bcmSwitchEventRead:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported event control");
            break;
        }
        case bcmSwitchEventStat:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                if (value)
                {
                    for (inter = 0; inter < nof_interrupts; inter++)
                    {
                        for (bi_index = 0; SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++)
                        {
                            if (!SOC_INFO(unit).block_valid[bi_index])
                            {
                                continue;
                            }

                            bi = &(SOC_BLOCK_INFO(unit, bi_index));
                            rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                            SHR_IF_ERR_EXIT(rc);

                            if (is_valid)
                            {
                                (interrupts[inter].statistics_count)[bi->number] = 0x0;
                            }
                        }
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid statistics parameter value");
                }
            }
            else
            {
                (interrupts[type.event_id].statistics_count)[inf_index] = 0x0;
            }
            break;
        }

        case bcmSwitchEventStormTimedCount:
        case bcmSwitchEventStormTimedPeriod:
        case bcmSwitchEventStormNominal:
        {
            rc = dnx_switch_event_intr_sw_state_storms_set(unit, interrupts, type.event_id, type.action, value);
            SHR_IF_ERR_EXIT(rc);
            break;
        }

        case bcmSwitchEventLog:
        case bcmSwitchEventCorrActOverride:
        case bcmSwitchEventPriority:
        case bcmSwitchEventUnmaskAndClearDisable:
        case bcmSwitchEventForceUnmask:
        {
            rc = dnx_switch_event_intr_sw_state_flags_set(unit, type.event_id, type.action, value);
            SHR_IF_ERR_EXIT(rc);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control");
            break;
        }
    }

exit:
    DNX_SWITCH_LOCK_RELEASE;
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *    bcm_dnx_switch_event_control_get
 * Description:
 *    Get event control
 * Parameters:
 *    unit        - Device unit number
 *  type        - Event action.
 *  value       - Event value
 * Returns:
 *      _SHR_E_xxx
 */
int
bcm_dnx_switch_event_control_get(
    int unit,
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type,
    uint32 *value)
{
    int rc = _SHR_E_NONE, nof_interrupts;
    soc_interrupt_db_t *interrupts;
    int inter_get;
    int is_enable;
    soc_block_types_t block_types;
    int is_valid;
    int inf_index = type.index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    DNX_SWITCH_LOCK_TAKE;

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "supports only interrupts event");
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    SHR_IF_ERR_EXIT(rc);

    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL)
    {
        if (type.event_id >= nof_interrupts || type.event_id < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid interrupt");
        }

        /*
         * verify block instance
         */
        if (!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid interrupt register for the device");
        }

        block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;

        if (!SOC_BLOCK_IN_LIST(block_types, SOC_BLK_CLP) && !SOC_BLOCK_IN_LIST(block_types, SOC_BLK_XLP))
        {
            rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
            SHR_IF_ERR_EXIT(rc);
            if (!is_valid)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported block instance");
            }
        }

        inf_index = soc_interrupt_get_block_index_from_port(unit, type.event_id, type.index);
        if (inf_index < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported block instance");
        }
    }

    switch (type.action)
    {
        case bcmSwitchEventMask:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                rc = soc_interrupt_is_all_mask(unit, (int *) value);
                SHR_IF_ERR_EXIT(rc);
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_is_enabled(unit, type.index, &(interrupts[type.event_id]), &is_enable);
                SHR_IF_ERR_EXIT(rc);
                *value = (is_enable == 0);
            }
            break;
        }
        case bcmSwitchEventForce:
        {
            /*
             * Set/clear per interrupt
             */
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid <controll all> event parameter for force option");
            }
            else
            {
                rc = soc_interrupt_force_get(unit, type.index, &(interrupts[type.event_id]), &is_enable);
                SHR_IF_ERR_EXIT(rc);
                *value = is_enable;
            }
            break;
        }
        case bcmSwitchEventClear:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                rc = soc_interrupt_is_all_clear(unit, (int *) value);
                SHR_IF_ERR_EXIT(rc);
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
                SHR_IF_ERR_EXIT(rc);
                *value = (inter_get == 0);
            }
            break;
        }
        case bcmSwitchEventRead:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid read parameter (event_id)");
            }
            else
            {
                {
                    /*
                     * Get per interrupt
                     */
                    rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
                    SHR_IF_ERR_EXIT(rc);
                    *value = (inter_get == 1);
                }
            }
            break;
        }
        case bcmSwitchEventStormTimedPeriod:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Storm Timed Period parameter (event_id)");
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_storm_timed_period_get(unit, type.event_id, value);
                SHR_IF_ERR_EXIT(rc);
            }
            break;
        }
        case bcmSwitchEventStormTimedCount:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Storm Timed Count parameter (event_id)");
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_storm_timed_count_get(unit, type.event_id, value);
                SHR_IF_ERR_EXIT(rc);
            }
            break;
        }
        case bcmSwitchEventStormNominal:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                *value = SOC_SWITCH_EVENT_NOMINAL_STORM(unit);
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nominal storm parameter (event_id)");
            }
            break;
        }
        case bcmSwitchEventStat:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Statistics Control parameter (event_id)");
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                *value = (interrupts[type.event_id].statistics_count)[inf_index];
            }
            break;
        }
        case bcmSwitchEventLog:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Log Control parameter (event_id)");
            }
            else
            {
                uint32 flags;
                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
                SHR_IF_ERR_EXIT(rc);
                *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE) != 0) ? 0x1 : 0x0;
            }
            break;
        }
        case bcmSwitchEventCorrActOverride:
        {
            /*
             * Value - 0 : Only bcm callback will be called for this interrupt.
             * Value - 1 : Only user callback will be called for this interrupt. At this mode BCM driver will only print the interrupt information for logging.
             * Value - 2 : User call back will be called immediately after bcm callback.
             */
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid CorrActOveride Control parameter (event_id)");
            }
            else
            {
                /*
                 * Get per interrupt
                 */
                uint32 flags;

                rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
                SHR_IF_ERR_EXIT(rc);

                if (flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB)
                {
                    *value = 0x2;
                }
                else
                {
                    *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE) != 0) ? 0x1 : 0x0;
                }
            }
            break;
        }
        case bcmSwitchEventPriority:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Event Priority Control parameter (event_id)");
            }
            else
            {
                uint32 flags;

                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
                SHR_IF_ERR_EXIT(rc);
                *value = ((flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB);
            }
            break;
        }
        case bcmSwitchEventUnmaskAndClearDisable:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid UnmaskAndClearDisable Control parameter (event_id)");
            }
            else
            {
                uint32 flags;

                /*
                 * Get per interrupt
                 */
                rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
                SHR_IF_ERR_EXIT(rc);
                *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS) != 0) ? 0x1 : 0x0;
            }
            break;
        }
        case bcmSwitchEventForceUnmask:
        {
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ForceUnmask Control parameter (event_id)");
            }
            else
            {
                uint32 flags;
                /*
                 * Get per interrupt
                 */

                rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
                SHR_IF_ERR_EXIT(rc);

                *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS) != 0) ? 0x1 : 0x0;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control");
            break;
        }
    }

exit:
    DNX_SWITCH_LOCK_RELEASE;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Maps the Forwarding type(key.index) which will to the compressed forwarding type(value.value).
* \par DIRECT INPUT:
*   \param [in] unit   - unit Id
*   \param [in] key    - The key.index to be mapped(compressed)
*   \param [out] value - The value that the key was mapped to.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_switch_control_mtu_get(
    int unit,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t * value)
{

    uint32 entry_handle_id;
    uint32 dnx_layer_type;
    uint8 get_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_MTU_LAYER_TYPE_MAP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_convert_forwarding_type(unit, BCM_CORE_ALL, (uint32 *) &key.index, &dnx_layer_type));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_LAYER_TYPE, dnx_layer_type);

    /**
     * Set the Compressed Value
     */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_COMPRESSED, INST_SINGLE, &get_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    value->value = get_val;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See switch.h
 */
int
bcm_dnx_switch_control_indexed_get(
    int unit,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t * value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    switch (key.type)
    {
        case bcmSwitchLinkLayerMtuFilter:
        {
            SHR_IF_ERR_EXIT(dnx_switch_control_mtu_get(unit, key, value));
            break;
        }
        case bcmSwitchHashSeed:
        {
            dnx_switch_lb_crc_seed_t crc_seed;

            crc_seed.lb_crc_function_index = (uint16) key.index;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_get(unit, &crc_seed));
            value->value = (uint32) (crc_seed.lb_crc_seed);
            break;
        }
        case bcmSwitchLayerRecordModeSelection:
        {
            uint32 physical_client;
            dnx_switch_lb_lsms_crc_select_t lsms_crc_select;
            /*
             * Convert logical client to physical so it can be read from tcam
             */
            SHR_IF_ERR_EXIT(dnx_switch_lb_logical_client_to_physical
                            (unit, (bcm_switch_control_t) key.index, &physical_client));
            lsms_crc_select.lb_client = (uint8) physical_client;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_get(unit, &lsms_crc_select));
            value->value = (uint32) (lsms_crc_select.lb_selection_bit_map);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type: %d\r\n", key.type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Maps the Forwarding type(key.index) which will to the compressed forwarding type(value.value).
* \par DIRECT INPUT:
*   \param [in] unit  - unit Id
*   \param [in] key   - The key.index to be mapped(compressed)
*   \param [in] value - The value that the key will be mapped to.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_switch_control_mtu_set(
    int unit,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t value)
{
    uint32 entry_handle_id;
    uint32 dnx_layer_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_MTU_LAYER_TYPE_MAP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_convert_forwarding_type(unit, BCM_CORE_ALL, (uint32 *) &key.index, &dnx_layer_type));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_LAYER_TYPE, dnx_layer_type);

    /*
     * Set the Compressed Value
     */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_COMPRESSED, INST_SINGLE, value.value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See switch.h
 */
int
bcm_dnx_switch_control_indexed_set(
    int unit,
    bcm_switch_control_key_t key,
    bcm_switch_control_info_t value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_SWITCH_LOCK_TAKE;

    switch (key.type)
    {
        case bcmSwitchLinkLayerMtuFilter:
        {
            if(value.value >= DNX_RX_MTU_PORT_COMPRESSED_LAYER_TYPE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The value supplied is too big %d! "
                                "Compressed Forward Layer Type must be between 1-6. \n",
                                value.value);

            }
            SHR_IF_ERR_EXIT(dnx_switch_control_mtu_set(unit, key, value));
            break;
        }
        case bcmSwitchHashSeed:
        {
            dnx_switch_lb_crc_seed_t crc_seed;
            uint32 max_crc_seed;

            crc_seed.lb_crc_function_index = (uint16) key.index;
            /*
             * Since we are shifting to 16 bits here, we need to also verify here, and not within
             * the called procedure.
             */
            max_crc_seed = dnx_data_switch.load_balancing.nof_seeds_per_crc_function_get(unit);
            if (value.value >= max_crc_seed)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "Seed of crc function on input (%d) is larger than max allowed (%d).\r\n"
                             "  This is illegal. Quit.\r\n", value.value, max_crc_seed - 1);
            }
            crc_seed.lb_crc_seed = (uint16) (value.value);
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_set(unit, &crc_seed));
            break;
        }
        case bcmSwitchLayerRecordModeSelection:
        {
            uint32 physical_client;
            dnx_switch_lb_lsms_crc_select_t lsms_crc_select;

            /*
             * Convert logical client to physical so it can be loaded into tcam
             */
            SHR_IF_ERR_EXIT(dnx_switch_lb_logical_client_to_physical
                            (unit, (bcm_switch_control_t) key.index, &physical_client));
            lsms_crc_select.lb_client = (uint8) physical_client;
            lsms_crc_select.lb_selection_bit_map = (uint16) (value.value);
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_set(unit, &lsms_crc_select));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type: %d\r\n", key.type);
            break;
        }
    }

exit:
    DNX_SWITCH_LOCK_RELEASE;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  re-init the dram, after power down and power up was made.
*
* \param [in] unit  - unit Id
* \param [in] flags - currently not in use
*
* \return
*   shr_error_e
*
* \remark
*   should be called part of temperature monitor procedure.
* \see
*   * None
*/
int
bcm_dnx_switch_dram_init(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dram_reinit(unit, flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - dram power down callback registration.
 *
 * \param [in] unit - unit number
 * \param [in] flags - NONE
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_dram_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dram_power_down_cb_register(unit, flags, callback, userdata));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - dram power down callback un-registration.
 *
 * \param [in] unit - unit number
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_dram_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dram_power_down_cb_unregister(unit, callback, userdata));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - power dowm dram procedure when temperature exceed the "powe down" threshold.
 *
 * \param [in] unit - unit number
 * \param [in] flags - API flags (currently NONE)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This functionality should be used when Soc property: dram_temperature_monitor_enable=FALSE
 * \see
 *   * None
 */
int
bcm_dnx_switch_dram_power_down(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dram_power_down(unit, flags));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable or disable traffic into DRAM. once the temperature exceed a threshold, user should disable traffic to DRAM.
 *   When temperature reduce below threshold, it should enable it back.
 *
 * \param [in] unit - unit number
 * \param [in] flags - API flags (currently NONE)
 * \param [in] enable - true for enable traffic into DRAM, false for disable
 * \return
 *   shr_error_e
 *
 * \remark
 *   This functionality should be used when Soc property: dram_temperature_monitor_enable=FALSE
 * \see
 *   * None
 */
int
bcm_dnx_switch_dram_traffic_enable_set(
    int unit,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dram_traffic_enable_set(unit, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
bcm_dnx_switch_thermo_sensor_read_verify(
    int unit,
    bcm_switch_thermo_sensor_type_t sensor_type,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sensor_data, _SHR_E_PARAM, "sensor_data");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - read from HW the thermo sensors
 *
 * \param [in] unit - unit number
 * \param [in] sensor_type - Thermo sensors enum. use bcmSwitchThermoSensorDram for reading dram temp
 * \param [in] interface_id - interface is
 * \param [out] sensor_data - output data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_switch_thermo_sensor_read(
    int unit,
    bcm_switch_thermo_sensor_type_t sensor_type,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_switch_thermo_sensor_read_verify(unit, sensor_type, interface_id, sensor_data));
    switch (sensor_type)
    {
        case bcmSwitchThermoSensorDram:
            SHR_IF_ERR_EXIT(dnx_dram_thermo_sensor_read(unit, interface_id, sensor_data));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported sensor_type");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_kbp_info_get_verify(
    int unit, 
    bcm_core_t core, 
    bcm_switch_kbp_info_t *kbp_info)
{
    SHR_FUNC_INIT_VARS(unit);    
    SHR_NULL_CHECK(kbp_info, _SHR_E_PARAM, "kbp_info");

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - get from DNX driver the KBP device pointer
 *
 * \param [in] unit - unit number
 * \param [in] core - not in used
 * \param [out] kbp_info - pointer to kbp device_pointer
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int 
bcm_dnx_switch_kbp_info_get(
    int unit, 
    bcm_core_t core, 
    bcm_switch_kbp_info_t *kbp_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_switch_kbp_info_get_verify(unit, core, kbp_info));
#if defined(INCLUDE_KBP)
    SHR_IF_ERR_EXIT(dnx_kbp_info_get(unit, kbp_info));
#else
    kbp_info->device_p = NULL;
#endif

exit:
    SHR_FUNC_EXIT;
}

/* at the end of file */
#undef BSL_LOG_MODULE
