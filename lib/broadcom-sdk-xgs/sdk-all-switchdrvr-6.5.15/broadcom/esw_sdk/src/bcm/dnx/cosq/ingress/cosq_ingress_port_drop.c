/** \file cosq_ingress_port_drop.c
 * $Id$
 *
 * cosq ingree port drop API functions for DNX. \n
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/cosq.h>
#include <shared/gport.h>

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include <shared/utilex/utilex_framework.h>

/*
 * Defines 
 * { 
 */
#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION  _SHR_PORT_PRD_ETH_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION   _SHR_PORT_PRD_TM_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION   _SHR_PORT_PRD_IP_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION _SHR_PORT_PRD_MPLS_MAP_INDICATION

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_SET(key)  ( key |= DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_SET(key)   ( key |= DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_SET(key)   ( key |= DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_SET(key) ( key |= DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_GET(key)  ( key & DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_GET(key)   ( key & DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_GET(key)   ( key & DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_GET(key) ( key & DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_CLEAR(key)  ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_CLEAR(key)   ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_CLEAR(key)   ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_CLEAR(key) ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

#define DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL          0xF
#define DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_MAX_VAL               0xFFFF
#define DNX_COSQ_INGRESS_PORT_DROP_THRESHOLD_MAX_VAL                0x7FFF
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_SPECIAL_LABEL_MAX_VAL       0xF
#define DNX_COSQ_INGRESS_PORT_DROP_FLEX_KEY_OFFSET_KEY_MAX_VAL      0x7F
#define DNX_COSQ_INGRESS_PORT_DROP_FLEX_KEY_OFFSET_RESULT_MAX_VAL   0xFF

#define DNX_COSQ_INGRESS_PORT_DROP_TPID_MAX                                       (0xffff)

/*
 * }
 */

/**
 * \brief - Verify common paramenters for the ingress port drop 
 *        module (PRD)
 */
static int
dnx_cosq_ingress_port_drop_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /** Verify port is NIF, not including ELK ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress port drop is not supported for port of type %d", port_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for dnx_cosq_ingress_port_set
 */
static int
dnx_cosq_ingress_port_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
        case bcmCosqControlIngressPortDropTpid2:
        case bcmCosqControlIngressPortDropTpid3:
        case bcmCosqControlIngressPortDropTpid4:
            if (arg > DNX_COSQ_INGRESS_PORT_DROP_TPID_MAX || arg < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, TPID value %d is invalid", unit, arg);
            }
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            if (arg != 0 && arg != 1)   /* i.e. invalid value */
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be 0 or 1", unit, arg);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
dnx_cosq_ingress_port_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_set_verify(unit, port, type, arg));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 0, arg));
            break;
        case bcmCosqControlIngressPortDropTpid2:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 1, arg));
            break;
        case bcmCosqControlIngressPortDropTpid3:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 2, arg));
            break;
        case bcmCosqControlIngressPortDropTpid4:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 3, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreIpDscp, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreMplsExp, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreInnerTag, arg));
            break;
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreOuterTag, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_cosq_ingress_port_get
 */
static int
dnx_cosq_ingress_port_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "dnx_cosq_ingress_port_get_arg");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
dnx_cosq_ingress_port_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_get_verify(unit, port, type, arg));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 0, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid2:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 1, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid3:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 2, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid4:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 3, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreIpDscp, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreMplsExp, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreInnerTag, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreOuterTag, (uint32 *) arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify paramenters for 
 *        bcm_dnx_cosq_ingress_port_drop_control_frame_get
 */
static int
dnx_cosq_ingress_port_drop_control_frame_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(control_frame_config, _SHR_E_PARAM, "control_frame_config");

    /*
     * check control frame index is valid
     */
    if (control_frame_index >= dnx_data_nif.prd.nof_control_frames_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, control frame index %d is out of range\n", unit, control_frame_index);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see .h file 
 */
int
bcm_dnx_cosq_ingress_port_drop_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_control_frame_get_verify
                          (unit, port, flags, control_frame_index, control_frame_config));

    SHR_IF_ERR_EXIT(imb_prd_control_frame_get(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify paramenters for 
 *        bcm_dnx_cosq_ingress_port_drop_control_frame_set
 * 
 */
static int
dnx_cosq_ingress_port_drop_control_frame_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint64 mac_da_max_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_control_frame_get_verify
                    (unit, port, flags, control_frame_index, control_frame_config));

    /*
     * check ether type code and mask are valid 
     */
    if (control_frame_config->ether_type_code > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL ||
        control_frame_config->ether_type_code_mask > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d or mask %d are out of range\n", unit,
                     control_frame_config->ether_type_code, control_frame_config->ether_type_code_mask);
    }
    /*
     * check MAC DA and MAC DA mas kare valid
     */
    COMPILER_64_ZERO(mac_da_max_val);
    COMPILER_64_MASK_CREATE(mac_da_max_val, UTILEX_PP_MAC_ADDRESS_NOF_BITS, 0);
    if (control_frame_config->mac_da_val > mac_da_max_val || control_frame_config->mac_da_mask > mac_da_max_val)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, MAC DA %lu or mask %lu are out of range\n", unit,
                     (unsigned long) control_frame_config->mac_da_val,
                     (unsigned long) control_frame_config->mac_da_mask);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_control_frame_set_verify
                          (unit, port, flags, control_frame_index, control_frame_config));

    SHR_IF_ERR_EXIT(imb_prd_control_frame_set(unit, port, control_frame_index, control_frame_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_custom_ether_type_get
 */
static int
dnx_cosq_ingress_port_drop_custom_ether_type_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 ether_type_code_min, ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(ether_type_val, _SHR_E_PARAM, "ether_type_val");

    /*
     * check ether type code is valid
     */
    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    if (ether_type_code < ether_type_code_min || ether_type_code > ether_type_code_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Eth type code %d is invalid. Valid Eth type codes are 1-6\n", ether_type_code);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_custom_ether_type_get_verify
                          (unit, port, flags, ether_type_code, ether_type_val));

    SHR_IF_ERR_EXIT(imb_prd_custom_ether_type_get(unit, port, ether_type_code, ether_type_val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_custom_ether_type_set
 */
static int
dnx_cosq_ingress_port_drop_custom_ether_type_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_min, ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check ether type code is valid
     */
    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    if (ether_type_code < ether_type_code_min || ether_type_code > ether_type_code_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Eth type code %d is invalid. Valid Eth type codes are 1-6\n", ether_type_code);
    }

    /*
     * check ether type val is valid
     */
    if (ether_type_val > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type %d is out of range\n", unit, ether_type_val);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_custom_ether_type_set_verify
                          (unit, port, flags, ether_type_code, ether_type_val));

    SHR_IF_ERR_EXIT(imb_prd_custom_ether_type_set(unit, port, ether_type_code, ether_type_val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_default_priority_get
 */
static int
dnx_cosq_ingress_port_drop_default_priority_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(default_priority, _SHR_E_PARAM, "default_priority");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_default_priority_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_default_priority_get_verify(unit, port, flags, default_priority));

    SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdDefaultPriority, default_priority));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_default_priority_set
 */
static int
dnx_cosq_ingress_port_drop_default_priority_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check default priority is valid
     */
    if (default_priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, default priority %d is out of range\n", unit, default_priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_default_priority_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_default_priority_set_verify(unit, port, flags, default_priority));

    SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdDefaultPriority, default_priority));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_enable_get
 */
static int
dnx_cosq_ingress_port_drop_enable_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int *enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(enable_mode, _SHR_E_PARAM, "enable_mode");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_enable_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int *enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_enable_get_verify(unit, port, flags, enable_mode));

    SHR_IF_ERR_EXIT(imb_prd_enable_get(unit, port, enable_mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_enable_set
 */
static int
dnx_cosq_ingress_port_drop_enable_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    switch (enable_mode)
    {
        case bcmCosqIngressPortDropDisable:
        case bcmCosqIngressPortDropEnableHardStage:
        case bcmCosqIngressPortDropEnableHardAndSoftStage:
            /*
             * valid input
             */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, port %d: NIF priority drop enable mode %d is invalid\n", unit, port,
                         enable_mode);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_enable_set_verify(unit, port, flags, enable_mode));

    SHR_IF_ERR_EXIT(imb_prd_enable_set(unit, port, enable_mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_construct_get
 */
static int
dnx_cosq_ingress_port_drop_flex_key_construct_get_verify(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id, _SHR_E_PARAM, "key_id");
    SHR_NULL_CHECK(flex_key_config, _SHR_E_PARAM, "flex_key_config");

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, key_id->port));

    /*
     * check ether type code is valid
     */
    if (key_id->ether_type_code > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d is out of range\n", unit, key_id->ether_type_code);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_construct_get(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_construct_get_verify
                          (unit, key_id, flags, flex_key_config));

    SHR_IF_ERR_EXIT(imb_prd_flex_key_construct_get(unit, key_id->port, key_id->ether_type_code, flex_key_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_construct_set
 */
static int
dnx_cosq_ingress_port_drop_flex_key_construct_set_verify(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    int offset_index;
    uint32 is_configurable_ether_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_flex_key_construct_get_verify(unit, key_id, flags, flex_key_config));

    /*
     * check offset array size is valid
     */
    if (flex_key_config->array_size != BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset array size in flex_key_config should be %d\n", unit,
                     BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS);
    }
    /*
     * check offsets in array are valid
     */
    for (offset_index = 0; offset_index < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_index)
    {
        if (flex_key_config->offset_array[offset_index] > DNX_COSQ_INGRESS_PORT_DROP_FLEX_KEY_OFFSET_KEY_MAX_VAL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset %d is out of range\n", unit,
                         flex_key_config->offset_array[offset_index]);
        }
    }
    /*
     * check offset base is valid
     */
    switch (flex_key_config->offset_base)
    {
        case bcmCosqIngressPortDropFlexKeyBasePacketStart:
        case bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader:
        case bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset base %d is invalid\n", unit, flex_key_config->offset_base);
    }
    /*
     * if offset base is "end of header after eth" and the ether type code is configurable, than user must provide the ether type size
     * in all other cases, ether type size is expected to be -1
     */
    SHR_IF_ERR_EXIT(imb_prd_is_custom_ether_type_get
                    (unit, key_id->port, key_id->ether_type_code, &is_configurable_ether_type));
    if (flex_key_config->offset_base == bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader
        && is_configurable_ether_type)
    {
        if (flex_key_config->ether_type_header_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, when setting a key for configurable ether type code %d, "
                         "and offset base is set to EndOfHeaderAfterEthHeader, ether type size should be provided\n",
                         unit, key_id->ether_type_code);
        }
    }
    else if (flex_key_config->ether_type_header_size != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, ether type size should only be set for configurable ether types, when offste base is EndOfHeaderAfterEthHeader. "
                     "in all other cases it is expected to be -1\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_construct_set(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_construct_set_verify
                          (unit, key_id, flags, flex_key_config));

    SHR_IF_ERR_EXIT(imb_prd_flex_key_construct_set(unit, key_id->port, key_id->ether_type_code, flex_key_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_entry_get
 */
static int
dnx_cosq_ingress_port_drop_flex_key_entry_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(flex_key_info, _SHR_E_PARAM, "flex_key_info");

    /*
     * check TCAM entry index is valid
     */
    if (key_index >= dnx_data_nif.prd.nof_tcam_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, port %d: Flex key entry key_index %d is out of range\n", unit, port,
                     key_index);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_entry_get_verify
                          (unit, port, flags, key_index, flex_key_info));

    SHR_IF_ERR_EXIT(imb_prd_flex_key_entry_get(unit, port, key_index, flex_key_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_entry_set
 */
static int
dnx_cosq_ingress_port_drop_flex_key_entry_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    int offset_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_flex_key_entry_get_verify(unit, port, flags, key_index, flex_key_info));

    /** check entry information is valid */
    /*
     * check ether type code is valid
     */
    if (flex_key_info->ether_code.value > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL ||
        flex_key_info->ether_code.mask > DNX_COSQ_INGRESS_PORT_DROP_ETHER_TYPE_CODE_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d or mask %d is out of range\n", unit,
                     flex_key_info->ether_code.value, flex_key_info->ether_code.mask);
    }
    /*
     * check number of offsets in array 
     */
    if (flex_key_info->num_key_fields > BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, number of key fields %d is out of range\n", unit,
                     flex_key_info->num_key_fields);
    }
    /*
     * check offsets in array
     */
    for (offset_index = 0; offset_index < flex_key_info->num_key_fields; ++offset_index)
    {
        if (flex_key_info->key_fields[offset_index].value > DNX_COSQ_INGRESS_PORT_DROP_FLEX_KEY_OFFSET_RESULT_MAX_VAL ||
            flex_key_info->key_fields[offset_index].mask > DNX_COSQ_INGRESS_PORT_DROP_FLEX_KEY_OFFSET_RESULT_MAX_VAL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key_field value %d or mask %d is out of range\n", unit,
                         flex_key_info->key_fields[offset_index].value, flex_key_info->key_fields[offset_index].mask);
        }
    }
    /*
     * check priority is valid
     */
    if (flex_key_info->priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, flex_key_info->priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_entry_set_verify
                          (unit, port, flags, key_index, flex_key_info));

    SHR_IF_ERR_EXIT(imb_prd_flex_key_entry_set(unit, port, key_index, flex_key_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the key is valid and that key matches the 
 *        correct PRD map for
 *        bcm_dnx_cosq_ingress_port_drop_map_set/get APIs
 */
static int
dnx_cosq_ingress_port_drop_map_key_validate(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key)
{
    uint32 key_mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_TM_KEY_MASK;
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_IP_KEY_MASK;
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_ETH_KEY_MASK;
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_MPLS_KEY_MASK;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, invalid map %d\n", unit, map);
    }

    if (*key & ~key_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is invalid for map %d\n", unit, *key, map);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_map_get
 */
static int
dnx_cosq_ingress_port_drop_map_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");

    /*
     * check map and key are valid and key match the correct map 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_map_key_validate(unit, port, map, key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_map_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    int *priority)
{
    uint32 prd_priority;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_map_get_verify(unit, port, flags, map, &key, priority));

    *priority = 0;
    SHR_IF_ERR_EXIT(imb_prd_map_get(unit, port, map, key, &prd_priority));
    *priority = prd_priority;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_map_set
 */
static int
dnx_cosq_ingress_port_drop_map_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check map and key are valid and key match the correct map 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_map_key_validate(unit, port, map, key));

    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_map_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_map_set_verify(unit, port, flags, map, &key, priority));

    SHR_IF_ERR_EXIT(imb_prd_map_set(unit, port, map, key, priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_threshold_get
 */
static int
dnx_cosq_ingress_port_drop_threshold_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_threshold_get_verify(unit, port, flags, priority, value));

    SHR_IF_ERR_EXIT(imb_prd_threshold_get(unit, port, priority, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_threshold_set
 */
static int
dnx_cosq_ingress_port_drop_threshold_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, priority);
    }
    /*
     * Check threshold value is valid 
     */
    if (value > DNX_COSQ_INGRESS_PORT_DROP_THRESHOLD_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, threshold %d is out of range\n", unit, value);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_threshold_set_verify(unit, port, flags, priority, value));

    SHR_IF_ERR_EXIT(imb_prd_threshold_set(unit, port, priority, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_mpls_special_label_get
 */
static int
dnx_cosq_ingress_port_drop_mpls_special_label_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(label_config, _SHR_E_PARAM, "label_config");

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check label index is valid
     */
    if (label_index >= dnx_data_nif.prd.nof_mpls_special_labels_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, label index %d is invalid\n", unit, label_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_mpls_special_label_get_verify
                          (unit, port, flags, label_index, label_config));

    SHR_IF_ERR_EXIT(imb_prd_mpls_special_label_get(unit, port, label_index, label_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for 
 *        bcm_dnx_cosq_ingress_port_drop_mpls_special_label_set
 */
static int
dnx_cosq_ingress_port_drop_mpls_special_label_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_mpls_special_label_get_verify
                    (unit, port, flags, label_index, label_config));

    /*
     * check label value is valid 
     */
    if (label_config->label_value > DNX_COSQ_INGRESS_PORT_DROP_MPLS_SPECIAL_LABEL_MAX_VAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, label value %d is invalid\n", unit, label_config->label_value);
    }
    /*
     * check priority is valid
     */
    if (label_config->priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, label_config->priority);
    }
    /*
     * check is_tdm is valid
     */
    if (label_config->is_tdm > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, is_tdm %d is out of range\n", unit, label_config->is_tdm);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_mpls_special_label_set_verify
                          (unit, port, flags, label_index, label_config));

    SHR_IF_ERR_EXIT(imb_prd_mpls_special_label_set(unit, port, label_index, label_config));
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
