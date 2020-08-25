/** \file vlan_port.c
 *
 *  VLAN port procedures for DNX. Allows creation of
 *  VLAN-Port(AC) entities.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <shared/util.h>

#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/port/port_match.h>
#include <soc/dnx/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_qos_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define ETHERTYPE_IPV4 (0x0800)
#define ETHERTYPE_IPV6 (0x86dd)
#define ETHERTYPE_MPLS (0x8847)
#define ETHERTYPE_ARP  (0x0806)
#define ETHERTYPE_INITIALIZATION  (0)
/**
 * \brief MPLS L2-VPNs may have native VLAN port. Check whether the gport is an MPLS L2-VPN type.
 */
#define DNX_GPORT_IS_MPLS_L2VPN(_gport_) (BCM_GPORT_IS_MPLS_PORT(_gport_) || BCM_GPORT_IS_TUNNEL(_gport_))
/*
 * }
 */

/** Prefix value that is added to the destination field when building the expected payload in the VTT */
#define LIF_LEARN_INFO_OPTIMIZED_PREFIX 12
#define IN_AC_TCAM_DB_PCP_MASK (0xE)

/**
 * \brief
 *   Holds information for the entry to be inserted to the in
 *   lif table
 *
 */
typedef struct
{
    /**
     * \brief
     *  Dbal table id
     */
    dbal_tables_e dbal_table_id;
    /**
     * \brief
     *   Result type of the given table
     */
    int result_type;
    /**
     * \brief
     *   Global lif of the entry
     */
    int global_lif;
    /**
     * \brief
     *   vsi of the LIF
     */
    bcm_vlan_t vsi;
    /**
     * \brief
     *   fodo assignment mode of the LIF
     */
    int fodo_assignment_mode;
    /**
     * \brief
     *   in lif profile
     */
    int in_lif_profile;
    /**
     * \brief
     *   vlan_domain
     */
    uint32 vlan_domain;
    /**
     * \brief
     *   learn_enable
     */
    uint32 learn_enable;
    /**
     * \brief
     *   learn_context
     */
    dbal_enum_value_field_learn_payload_context_e learn_context;
    /**
     * \brief
     *   learn_info
     */
    uint32 learn_info;
    /**
     * \brief
     *   protection_pointer
     */
    int protection_pointer;
    /**
     * \brief
     *   protection_path
     */
    int protection_path;
    /**
     * \brief
     *   flush_group
     */
    uint32 flush_group;
} dnx_ingress_ac_table_info_t;

/**
 * \brief
 *   Holds information for ingress table ids (outlif) and the
 *   accompanying result types
 */
typedef struct
{
    /**
     * \brief
     *  Dbal table id
     */
    dbal_tables_e dbal_table_id;
    /**
     * \brief
     *   Result type of the given table
     */
    int result_type;
    /**
     * \brief
     *   EEDB phase of the given table
     */
    int outlif_phase;
    /**
     * \brief
     *   Local lif of the next EEDB entry, pointed from this entry.
     */
    int next_outlif;
    /**
     * \brief
     *   out lif profile.
     */
    int out_lif_profile;
    /**
     * \brief
     *   vlan_domain
     */
    uint32 vlan_domain;
    /**
     * \brief
     *   protection_pointer
     */
    int protection_pointer;
    /**
     * \brief
     *   protection_path
     */
    int protection_path;
    /**
     * \brief
     *   is last ethernet layer indication
     *   (similar to is native)
     */
    int last_eth_layer;
    /**
     * nwk_qos_index
     */
    int nwk_qos_idx;
    /**
     * egress qos model
     */
    int qos_model;
} dnx_egress_ac_table_info_t;

/**
 * \brief
 *   Enumeration for the various AC applications:
 *   SERVICE - Symmetrical LIF that enables learning
 *   VLAN_TRANSLATION - Performs Egress/Ingress VLAN edit only
 *   OPTIMIZATION - Aggregates VLANs on a port to one In-LIF
 */
typedef enum
{
    INVALID_AC_APPLICATION_TYPE = -1,
    FIRST_AC_APPLICATION_TYPE = 0,
    /**
     *  Symmetrical LIF that enables learning
     */
    SERVICE_AC_APPLICATION_TYPE = FIRST_AC_APPLICATION_TYPE,
    /**
     *  Performs Egress/Ingress VLAN edit only
     */
    VLAN_TRANSLATION_AC_APPLICATION_TYPE,
    /**
     *  Aggregates VLANs on a port to one In-LIF
     */
    OPTIMIZATION_AC_APPLICATION_TYPE,
    NUM_AC_APPLICATION_TYPE_E
} ac_application_type_e;

/**
 * \brief - function returns whether a given VLAN-Port gport is allocated
 */
static shr_error_e
dnx_vlan_port_is_allocated(
    int unit,
    bcm_gport_t gport,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = FALSE;

    /*
     * Use VLAN-Port sub type for each case, checking its algo "is_allocaed:
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
    {
        /*
         * Handle Ingress Virtual Native Vlan port
         */
        int sw_handle;

        sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(gport);

        SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.is_allocated(unit, sw_handle, is_allocated));

    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        /*
         * Handle Egress Virtual Vlan ports: ESEM Native or non-Native:
         */
        int esem_handle;

        esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(gport);

        SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.is_allocated(unit, esem_handle, is_allocated));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Egress ESEM Default:
         */
        int esem_handle;
        /*
         * Decode the esem_handle from the virtual gport
         */
        esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport);

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.is_allocated(unit, esem_handle, is_allocated));
    }
    else
    {
        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;

        if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
        {
            /*
             * Ingress only:
             */
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        }
        else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)
        {
            /*
             * Egress Only
             */
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
        else
        {
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF;
        }

        /*
         * Check local LIFs allocation using DNX Algo Gport Management:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &gport_hw_resources));

        /*
         * If we got here, the LIF was allocated.
         */
        *is_allocated = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create/find
 * The function verifies flags field - for correct ingress/egress only setting.
 */
static shr_error_e
dnx_vlan_port_direction_flags_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Verify that both flags BCM_VLAN_PORT_CREATE_INGRESS_ONLY BCM_VLAN_PORT_CREATE_EGRESS_ONLY are not set
     */
    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, Both BCM_VLAN_PORT_CREATE_INGRESS_ONLY and BCM_VLAN_PORT_CREATE_EGRESS_ONLY are set! flags = 0x%08X\n",
                     vlan_port->flags);
    }
    else if ((is_egress == TRUE) && (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, Unavailable criteria BCM_VLAN_PORT_MATCH_PORT (%d) in egress, flags = 0x%08X\n",
                     vlan_port->criteria, vlan_port->flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create/find
 * The function verifies that criteria field is supported and
 * it is also verifies the proper match fields.
 */
static shr_error_e
dnx_vlan_port_criteria_verify(
    int unit,
    int is_match_none_valid,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Check supported criterias
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
            if (is_match_none_valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. criteria = BCM_VLAN_PORT_MATCH_NONE (%d) is not supported! flags = 0x%08X\n",
                             vlan_port->criteria, vlan_port->flags);
            }
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            break;

        case BCM_VLAN_PORT_MATCH_PORT:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            /*
             * Match port and port X CVLAN, port X VLAN X PCP, port X VLAN X VLAN X PCP are not supported for Ingress Native AC !
             */
            if ((is_ingress == TRUE) && (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)))
            {

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! Ingress Native unsupported criteria. flags = 0x%08X, criteria = %d\n"
                             "Only BCM_VLAN_PORT_MATCH_NONE(%d),BCM_VLAN_PORT_MATCH_PORT_VLAN(%d) and "
                             "BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED(%d) are supported for Ingress Native.\n",
                             vlan_port->flags, vlan_port->criteria, BCM_VLAN_PORT_MATCH_NONE,
                             BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED);
            }
            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:

            if (is_ingress == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) but flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is not set! flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) but flag BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) is not set! flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) is not supported for native! (flag BCM_VLAN_PORT_NATIVE (0x%08X) is set!) flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_NATIVE, vlan_port->flags);
            }

            break;

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:

            if (is_ingress == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  BCM_VLAN_PORT_MATCH_PORT_UNTAGGED (%d) is ingress configuration! (flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is set!) flags = 0x%08X\n",
                             BCM_VLAN_PORT_MATCH_PORT_UNTAGGED, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  BCM_VLAN_PORT_MATCH_PORT_UNTAGGED (%d) is not supported for native! (flag BCM_VLAN_PORT_NATIVE (0x%08X) is set!) flags = 0x%08X\n",
                             BCM_VLAN_PORT_MATCH_PORT_UNTAGGED, BCM_VLAN_PORT_NATIVE, vlan_port->flags);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported criteria (%d)!\n"
                         "Only BCM_VLAN_PORT_MATCH_NONE(%d), BCM_VLAN_PORT_MATCH_PORT(%d), "
                         "and BCM_VLAN_PORT_MATCH_PORT_VLAN(%d), BCM_VLAN_PORT_MATCH_PORT_CVLAN(%d), "
                         "and BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN(%d), BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED(%d), BCM_VLAN_PORT_MATCH_NAMESPACE_VSI(%d), "
                         "BCM_VLAN_PORT_MATCH_NAMESPACE_PORT(%d) and BCM_VLAN_PORT_MATCH_PORT_UNTAGGED (%d) are supported\n",
                         vlan_port->criteria, BCM_VLAN_PORT_MATCH_NONE, BCM_VLAN_PORT_MATCH_PORT,
                         BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_CVLAN,
                         BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED,
                         BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                         BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, BCM_VLAN_PORT_MATCH_PORT_UNTAGGED);
            break;
    }

    /*
     * Verify match criteria:
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
            /*
             * Verify match_vlan is in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:

            /*
             * Verify match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_inner_vlan);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            /*
             * Verify match_pcp match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_PCP_CHK_ID(unit, vlan_port->match_pcp);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            /*
             * Verify match_pcp match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_inner_vlan);
            BCM_DNX_PCP_CHK_ID(unit, vlan_port->match_pcp);
            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Verify Namespace range:
             */
            uint32 network_domain_max_val;

            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, DBAL_FIELD_NETWORK_DOMAIN, FALSE,
                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI, 0,
                             (int *) &network_domain_max_val));

            if (vlan_port->match_class_id > network_domain_max_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) but match_class_id = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, vlan_port->match_class_id, network_domain_max_val);
            }

            /*
             * Verify VSI range:
             */
            if (!DNX_VSI_VALID(unit, vlan_port->vsi))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) but vsi = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, vlan_port->vsi,
                             (dnx_data_l2.vsi.nof_vsis_get(unit) - 1));
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            uint32 namespace_max_val;

            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, DBAL_FIELD_NAMESPACE, FALSE,
                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT, 0,
                             (int *) &namespace_max_val));

            if (vlan_port->match_class_id > namespace_max_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) but match_class_id = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->match_class_id, namespace_max_val);
            }

            /*
             * Verify port:
             *   - should be encoded as gport system port.
             */
            if (!BCM_GPORT_IS_SYSTEM_PORT(vlan_port->port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) but port = 0x%08X is not SYSTEM_PORT (%d << %d)!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->port, _SHR_GPORT_TYPE_SYSTEM_PORT,
                             _SHR_GPORT_TYPE_SHIFT);
            }

            break;
        }
        default:
            /*
             *Type of supported criterias has been checked before.
             */
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_verify(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    int is_virtual_ac)
{
    int is_ingress, is_egress, fec, fec_id, ing_table_capacity, eg_table_capacity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Verify direction flags BCM_VLAN_PORT_CREATE_INGRESS_ONLY / BCM_VLAN_PORT_CREATE_EGRESS_ONLY
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_direction_flags_verify(unit, vlan_port));

    /*
     * Check supported criterias and Verify match criteria.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_criteria_verify(unit, TRUE, vlan_port));

    /*
     * Verify that legacy FEC information wasn't provided.
     * In DNX FEC allocation moved to a unified API - bcm_l3_egress_Create().
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CASCADED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_CASCADED flag is not supported."
                     "FEC creation is done in bcm_l3_egress_create() API.\n");
    }

    /** Verify L2 protection information */
    SHR_IF_ERR_EXIT(dnx_failover_l2_protection_info_verify(unit, vlan_port, NULL));

    /**ingress qos*/
    if ((vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid) ||
        (vlan_port->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid) ||
        (vlan_port->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "does not support ingress qos model now!");
    }
    /**egress qos*/
    if ((vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelInitial) ||
        (vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "does not support bcmQosEgressModelInitial and bcmQosEgressModelPipeMyNameSpace!");
    }
    if ((vlan_port->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (vlan_port->pkt_pri != 0 || vlan_port->pkt_cfi != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "pkt_pri and pkt_cfi must be 0 if egress qos model is neither PipeMyNameSpace nor PipeNextNameSpace");
    }
    /*
     * Verify next pointer (tunnel_id):
     */
    if (vlan_port->tunnel_id != 0)
    {
        /*
         * next outlif pointer (tunnel_id) is relevant only for egress non-virtual native AC is configured
         */
        if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == FALSE) ||
            (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == TRUE) ||
            (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, tunnel_id = 0x%08X can be configured only for non-virtual native AC egress vlan ports!\n"
                         "BCM_VLAN_PORT_NATIVE (0x%08X) flag should be set, BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) should not be set.\n"
                         "flags = 0x%08X\n",
                         vlan_port->tunnel_id, BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
        }

        /*
         * next outlif pointer (tunnel_id) can only be PWE or TUNNEL gport
         */
        if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->tunnel_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, creating Egress non-virtual Native LIF but tunnel_id = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                         vlan_port->tunnel_id, vlan_port->flags);
        }
        
        if (!BCM_GPORT_IS_MPLS_PORT(vlan_port->tunnel_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Egress non-virtual Native LIF may only have PWE GPort as 'tunnel_id' value (given: 0x%08x)\n",
                         vlan_port->tunnel_id);
        }
    }

    /*
     * BCM_VLAN_PORT_VLAN_TRANSLATION flag verification:
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
    {

        if ((is_ingress == TRUE) && (is_egress == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_VLAN_TRANSLATION can't be used for both ingress and egress in the same call!\n",
                         vlan_port->flags);
        }

        if (is_ingress == TRUE)
        {
            if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, flags = 0x%08X! BCM_VLAN_PORT_VLAN_TRANSLATION flag is applicable for ingress only with BCM_VLAN_PORT_NATIVE flag set.\n",
                             vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) && vlan_port->ingress_network_group_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "vlan_port.ingress_network_group_id(%d) must be 0 for ingress VLAN with BCM_VLAN_PORT_VLAN_TRANSLATION and BCM_VLAN_PORT_NATIVE set.\n",
                             vlan_port->ingress_network_group_id);
            }
        }
        /*
         * If we got here, is_egress is set ...
         *
         * For Egress VLAN translation (Native and non-native) there are two options:
         *   ESEM-Default: criteria must be NONE
         *   ESM: criteria must be port-vlan
         */
        else
        {
            /*
             * Check for ESEM-Default:
             *  - criteria - must be "none"
             *  - vsi - must be 0
             *  - port - must be 0
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT flag is applicable only with criteria BCM_VLAN_PORT_MATCH_NONE (=%d). criteria = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_MATCH_NONE, vlan_port->criteria);
                }

                if (vlan_port->vsi != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT (0x%08X) flag is applicable only with vsi =0! vsi = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_DEFAULT, vlan_port->vsi);
                }

                if (vlan_port->port != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT (0x%08X) flag is applicable only with port =0! port = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_DEFAULT, vlan_port->port);
                }
            }
            else
            {
                /*
                 * Check for ESEM:
                 *  - criteria
                 *  - vsi
                 */
                if ((vlan_port->criteria != BCM_VLAN_PORT_MATCH_PORT_VLAN)
                    && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                    && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_PORT))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM: applicable only with criteria BCM_VLAN_PORT_MATCH_PORT_VLAN (=%d)"
                                 " or BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) or BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) . criteria = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                                 BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->criteria);
                }

                /*
                 * ESEM out lif:
                 * Need to check that VSI is in range (as the VSI is one of the keys to the ESEM table):
                 * Note:
                 * VSI is valid only for ESEM and ESEM-namesapce-vsi, not valid for ESEM-namesapce-port.
                 */
                if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN ||
                    vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                {
                    if (!DNX_VSI_VALID(unit, vlan_port->vsi) && vlan_port->vsi != BCM_VLAN_ALL)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "vsi = %d is not a valid vsi. Valid range is [0:%d], or 0x%04X for all vsis.\n",
                                     vlan_port->vsi, (dnx_data_l2.vsi.nof_vsis_get(unit) - 1), BCM_VLAN_ALL);
                    }
                }
            }
        }

    }

    /*
     * Verify that each LIF application type is configured coherently across the VLAN-Port structure
     */
    if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
    {
        /*
         * AC Optimization is relevant for INGRESS Only, check that BCM_VLAN_PORT_CREATE_INGRESS_ONLY is set
         */
        if (is_egress == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "AC Optimization is relevant for INGRESS Only,"
                         "Error! BCM_VLAN_PORT_VSI_BASE_VID flag is for ingress only, need to set BCM_VLAN_PORT_CREATE_INGRESS_ONLY."
                         "flags = 0x%08X\n", vlan_port->flags);
        }

        /*
         * AC Optimization cannot be P2P
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "AC Optimization cannot be P2P!!"
                         "Error! BCM_VLAN_PORT_VSI_BASE_VID (0x%08X) cannot be set together with BCM_VLAN_PORT_CROSS_CONNECT (0x%08X)."
                         "flags = 0x%08X\n", BCM_VLAN_PORT_VSI_BASE_VID, BCM_VLAN_PORT_CROSS_CONNECT, vlan_port->flags);
        }

        if (((vlan_port->vsi % 4096) == 0) &&
            ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NONE) || (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT)))
        {

            /*
             * Verify that BCM_VLAN_PORT_VLAN_TRANSLATION is not set:
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_VLAN_PORT_VLAN_TRANSLATION flag cannot be set with BCM_VLAN_PORT_VSI_BASE_VID.\n");
            }

            /*
             * Verify that a flush group isn't configured, as a learning context for Optimized AC 
             * with a flush group isn't defined. 
             */
            if (vlan_port->group)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flush group can't be supported by an Optimized AC.\n");
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When BCM_VLAN_PORT_VSI_BASE_VID is set, vsi must be modulo 4096 and criteria must be NONE or MATCH_PORT.\n");

        }
    }
    else if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
    {
        /*
         * Verify that group parameter is in the correct range:
         */
        SHR_MAX_VERIFY(vlan_port->group, SAL_UPTO_BIT(dnx_data_l2.dma.flush_group_size_get(unit)), _SHR_E_PARAM,
                       "group exceeded allowed range\n");
    }
    else
    {
        /*
         * Verify that for a VLAN-Translation LIF, the vsi isn't reset as this
         * is a Symmetric LIF attribute.
         */
        if ((is_ingress == TRUE) && (vlan_port->vsi == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_VLAN_TRANSLATION flag cannot be set in Ingress when vsi = 0.\n");
        }

        /*
         * AC VLAN Translation LIF
         */

        if (is_egress == TRUE)
        {
            if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, BCM_VLAN_PORT_VLAN_TRANSLATION is not set !!! AC VLAN Translation (vsi !=0 and BCM_VLAN_PORT_VSI_BASE_VID is not set) .\n");

            }
        }

        /*
         * MC Group is a learning attribute, therefore not applicable for a VLAN Translation
         */
        if (vlan_port->failover_mc_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "There's no learning inforamtion for the LIF, failover_mc_group(%u) should be reset\n",
                         vlan_port->failover_mc_group);
        }
    }

    /*
     * Verify ingress_network_group_id range:
     */
    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, vlan_port->ingress_network_group_id));
    }

    /*
     * Verify egress_network_group_id range:
     */
    if (is_egress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, vlan_port->egress_network_group_id));
    }

    /*
     * Verify ingress wide data flag:
     * Only applicable for:
     *  - ingress.
     *  - non-native (AC_MP or AC_P2P)
     *  - native (AC_MP -not P2P and not virtual)
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
    {
        if (is_ingress == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) is only valid for symmetric or ingress-only LIF! flags = 0x%08X\n",
                         BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) flag is not valid for Cross-Connect native LIF! flags = 0x%08X\n",
                             BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) flag is not valid for virtual native LIF! flags = 0x%08X\n",
                             BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
            }
        }
    }

    /*
     * Verify protection input parameters
     * In case of FEC Protection verify the FEC gport
     */
    if (vlan_port->failover_port_id)
    {
        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(fec, vlan_port->failover_port_id);
        if (fec == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Invalid Gport for FEC protection, failover_port_id = 0x%08X\n",
                         vlan_port->failover_port_id);
        }
        else
        {
            /*
             * FEC is encoded with Type=DNX_FAILOVER_TYPE_FEC.
             * Need to get only the FEC ID in order to verify it is in range
             */
            DNX_FAILOVER_ID_GET(fec_id, fec);
            SHR_RANGE_VERIFY(fec_id, 0, dnx_data_failover.path_select.fec_size_get(unit) - 1, _SHR_E_PARAM,
                             "FEC failover-id out of bound.\n");
        }

        /*
         * At the FEC protection case the failover MC group should not be set
         */
        if (vlan_port->failover_mc_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "At the FEC protection case the failover MC group should not be set. failover_mc_group(%u)\n",
                         vlan_port->failover_mc_group);
        }
    }
    else
    {
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
        {
            if (is_virtual_ac == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Non-LIF protection is not allowed\n");
            }
        }
    }

    /*
     * Split Horizon
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NETWORK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_NETWORK flag is not supported."
                     "Orientation is configured through network_group field.\n");
    }

    /*
     * BCM_VLAN_PORT_WITH_ID verification:
     * Check it is vlan-port.
     * Check the sub-type.
     * Also, check that LIF ID is in range.
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        uint32 low_global_lif_id_bound;
        uint32 high_global_lif_id_bound;
        uint32 global_lif_id_value;

        /*
         * check it is a VLAN-PORT
         */
        if (!BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "LIF type is not VLAN-PORT (%d), check the bits starting at %d with mask 0x%x\n",
                         vlan_port->flags, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_VLAN_PORT, _SHR_GPORT_TYPE_SHIFT,
                         _SHR_GPORT_TYPE_MASK);
        }

        /*
         * for AC LIF, check sub-values
         */
        if (BCM_GPORT_SUB_TYPE_IS_LIF(vlan_port->vlan_port_id))
        {
            global_lif_id_value = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
            low_global_lif_id_bound = dnx_data_l3.rif.nof_rifs_get(unit);
            high_global_lif_id_bound =
                low_global_lif_id_bound + dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit);

            switch (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id))
            {
                case BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY:
                {
                    if (is_egress)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates INGRESS ONLY LIF but the flag BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08X) is not set!!!\n",
                                     vlan_port->flags, vlan_port->vlan_port_id, BCM_VLAN_PORT_CREATE_INGRESS_ONLY);
                    }
                    break;
                }
                case BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY:
                {

                    if (is_ingress)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates EGRESS ONLY LIF but the flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is not set!!!\n",
                                     vlan_port->flags, vlan_port->vlan_port_id, BCM_VLAN_PORT_CREATE_EGRESS_ONLY);
                    }
                    break;
                }
                case 0:
                {

                    if ((is_ingress == FALSE) || (is_egress == FALSE))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates symmetric LIF but the flags indicates non-symmetric\n",
                                     vlan_port->flags, vlan_port->vlan_port_id);
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X\n",
                                 vlan_port->flags, vlan_port->vlan_port_id);
                }
            }
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
        {
            /*
             * Ingress Virtual Native Vlan port
             */
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_2, &ing_table_capacity));

            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = 0;
            high_global_lif_id_bound = low_global_lif_id_bound + ing_table_capacity;
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port->vlan_port_id))
        {
            /*
             * Egress Virtual Vlan ports (ESEM Native or non-Native)
             */
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_2, &ing_table_capacity));
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_ESEM, &eg_table_capacity));

            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = ing_table_capacity;
            high_global_lif_id_bound = low_global_lif_id_bound + eg_table_capacity;
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
        {
            int default_ac_val, default_native_val, default_dual_homing_val;

            /*
             * Egress ESEM Default
             */
            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = 0;
            high_global_lif_id_bound = dnx_data_esem.default_result_profile.nof_profiles_get(unit);

            
            default_ac_val = dnx_data_esem.default_result_profile.default_ac_get(unit);
            default_native_val = dnx_data_esem.default_result_profile.default_native_get(unit);
            default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);

            if ((global_lif_id_value == default_ac_val) ||
                (global_lif_id_value == default_native_val) || (global_lif_id_value == default_dual_homing_val))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Error, flag BCM_VLAN_PORT_WITH_ID is set for a reserved Virtual Egress Default entry (%d)\n",
                             global_lif_id_value);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "LIF subtype is unknown, subtype = 0x%08X, check the bits starting at %d with mask 0x%x\n",
                         vlan_port->flags, vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_GET(vlan_port->vlan_port_id),
                         _SHR_GPORT_SUB_TYPE_SHIFT, _SHR_GPORT_SUB_TYPE_MASK);
        }

        /*
         * Verify LIF ID is in range
         */
        if ((global_lif_id_value >= high_global_lif_id_bound) || (global_lif_id_value < low_global_lif_id_bound))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "Global LIF ID = 0x%08X is out of range [0x%08X:0x%08X]\n",
                         vlan_port->flags, vlan_port->vlan_port_id, global_lif_id_value, low_global_lif_id_bound,
                         high_global_lif_id_bound - 1);
        }
    }

    /*
     * BCM_VLAN_PORT_ENCAP_WITH_ID verification:
     * 1. Can only be set for Egress LIF.
     * 2. Can't be set for Egress Virtual LIF (because no global LIF is allocated)
     * 3. If BCM_VLAN_PORT_WITH_ID is used, check that both global_lif values are the same.
     * 4. Check encap_id range
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ENCAP_WITH_ID))
    {
        uint32 high_global_l2_gport_lifs_bound;
        if (is_egress == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_ENCAP_WITH_ID can only be set for Egress LIF! flags = 0x%08X\n",
                         vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)
            || _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_ENCAP_WITH_ID can't be set for Egress Virtual LIF! flags = 0x%08X\n",
                         vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            int vlan_port_id_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
            int encap_id_lif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
            if (vlan_port_id_lif != encap_id_lif)
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, both flags BCM_VLAN_PORT_WITH_ID and BCM_VLAN_PORT_ENCAP_WITH_ID are set but their global LIFs value do not match! flags = 0x%08X, vlan_port_id = 0x%08X, encap_id = 0x%08X\n",
                             vlan_port->flags, vlan_port->vlan_port_id, vlan_port->encap_id);
        }

        high_global_l2_gport_lifs_bound = dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit);
        if ((vlan_port->encap_id >= high_global_l2_gport_lifs_bound) || (vlan_port->encap_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, encap_id = 0x%08X is out of range [0:0x%08X]!\n", vlan_port->encap_id,
                         high_global_l2_gport_lifs_bound - 1);
        }
    }

    /*
     * Single side symmetric allocation
     * Invalid with: WITH_ID, REPLACE and without NATIVE
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
    {
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_ALLOC_SYMMETRIC flag is not supported "
                         "without BCM_VLAN_PORT_NATIVE\n");
        }
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_VLAN_PORT_ALLOC_SYMMETRIC and BCM_VLAN_PORT_REPLACE cannot be used together");
        }
    }

    
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        /*
         * For non-native, port is irrelevant (=0) when:
         *   - criteria is NONE + ingress only + vsi is not 0 (so the appl type is vlan_translation thus no learning and the port is irrelevant)
         *   - EEDB only
         *   - ESEM Namespace-vsi
         *   - ESEM Default
         */
        if (((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NONE)
             && (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY)) && (vlan_port->vsi != 0))
            || (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY)
                && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
            || (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
            || (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT)))
        {
            if (vlan_port->port != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error!!! port = 0x%08X is irrelevant for this case! flags = 0x%08X, criteria = %d\n",
                             vlan_port->port, vlan_port->flags, vlan_port->criteria);
            }
        }
        else
        {
            dnx_algo_gpm_gport_phy_info_t gport_info;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        }
    }
    else
    {
        /*
         * Ingress Native LIF:
         *  - if the criteria is not NONE, the port has to be PWE gport
         */
        if (is_ingress == TRUE)
        {
            if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, creating Ingress Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X, criteria = %d\n",
                                 vlan_port->port, vlan_port->flags, vlan_port->criteria);
                }
            }
        }
        else if (is_egress == TRUE)
        {
            /*
             * Egress Native LIF:
             *  - non-virtual - verify the tunnel_id (is already done above).
             *  - ESEM virtual - verify the port is PWE LIF
             *  - ESEM-Default - verify the port is 0 (already done above).
             */
            if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == TRUE) &&
                (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT) == FALSE))
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, creating Egress virtual Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                                 vlan_port->port, vlan_port->flags);
                }
            }
        }
    }

    /*
     * EEDB only: 
     * Verify criteria:
     *  - native - criteria is meaningless (since we "get" the native out-lif from PWE ingress lif)
     *  - non native -verify that the criteria is NONE
     * Verify vsi is 0!
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) &&
        (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
    {
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, creating Egress Only non-native LIF at EEDB, criteria must be BCM_VLAN_PORT_MATCH_NONE (%d)! flags = 0x%08X, criteria = %d\n",
                             BCM_VLAN_PORT_MATCH_NONE, vlan_port->flags, vlan_port->criteria);
            }
        }

        if (vlan_port->vsi != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, creating Egress Only LIF at EEDB, vsi (%d) is irrelevant and must be0! flags = 0x%08X\n",
                         vlan_port->vsi, vlan_port->flags);
        }

    }

    
    if (vlan_port->vsi == 0)
    {
        int is_symmetric = is_ingress && is_egress;
        int is_esem_default = is_egress && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT)
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
        int is_egress_only_non_virtual = (!is_symmetric) && is_egress
            && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION));
        int is_ingress_p2p_non_native = is_ingress
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT)
            && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE));
        int is_ingress_native = is_ingress && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
        int is_egress_esem_namespace_port = (!is_symmetric) && is_egress
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) &&
            (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_PORT);

        if (((!is_symmetric) && (ll_app_type != OPTIMIZATION_AC_APPLICATION_TYPE) && (!is_esem_default)
             && (!is_egress_only_non_virtual) && (!is_ingress_p2p_non_native) && (!is_egress_esem_namespace_port))
            || is_ingress_native)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, vsi = 0 for wrong settings!!! flags = 0x%08X\n", vlan_port->flags);
        }
    }

    /**Validate ingress stat_pp flag*/
    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE)) && (!is_ingress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, BCM_VLAN_PORT_STAT_INGRESS_ENABLE is for ingress mode");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");
    /*
     * For Find using lookup:
     */
    if (vlan_port->vlan_port_id == 0)
    {
        /*
         * Verify direction falgs BCM_VLAN_PORT_CREATE_INGRESS_ONLY / BCM_VLAN_PORT_CREATE_EGRESS_ONLY
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_direction_flags_verify(unit, vlan_port));
        /*
         * Check supported criterias and verify match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_criteria_verify(unit, FALSE, vlan_port));
        /*
         * For Egress only, only ESEM is supported:
         *   - EEDB - can't find (the key is OUT_LIF).
         *   - ESEM-Default - can't find (the key is ESEM_DEFAULT_RESULT_PROFILE)
         *   - ESEM - can check for lookup - check EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW to find such a lookup
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == TRUE)
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Egress only VLAN-PORT find by lookup only support ESEM lookup! the flag BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) should be set. flags = 0x%08X \n",
                             BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Egress only VLAN-PORT find by lookup only support ESEM lookup! the flag BCM_VLAN_PORT_DEFAULT (0x%08X) should not be set. flags = 0x%08X \n",
                             BCM_VLAN_PORT_DEFAULT, vlan_port->flags);
            }

            if ((vlan_port->criteria != BCM_VLAN_PORT_MATCH_PORT_VLAN)
                && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_PORT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, flags = 0x%08X! For Egress ESEM: applicable only with criteria BCM_VLAN_PORT_MATCH_PORT_VLAN (=%d)"
                             " or BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) or BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) . criteria = %d\n",
                             vlan_port->flags, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->criteria);
            }
        }

        /*
         * Verify Native AC find by lookup:
         *   - native flag must be set.
         *   - ingress_only or egress_only flag must be set.
         *   - port must be PWE or TUNNEL gport.
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) == TRUE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Find by lookup of Ingress Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X, criteria = %d\n",
                                 vlan_port->port, vlan_port->flags, vlan_port->criteria);
                }
            }
            else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == TRUE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Find by lookup of Egress virtual Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                                 vlan_port->port, vlan_port->flags);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, Find by lookup of Native LIF but no ingress_only (0x%08X) or egress_only (0x%08X) flags are set! flags = 0x%08X\n",
                             BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }
        }

    }
    else
    {
        /*
         * Verify that the gport is vlan port:
         */
        if (!BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vlan_port_id = 0x%08X is not a VLAN Port!!!\n",
                         vlan_port->vlan_port_id);
        }

        /*
         * Verify that the gport is allocated:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_is_allocated(unit, vlan_port->vlan_port_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Wrong setting. vlan_port_id = 0x%08X is not allocated!!!\n",
                         vlan_port->vlan_port_id);
        }

        /*
         * Don't allow data rerieval for a Default ESEM Default entry with no data 
         */
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
        {
            int default_dual_homing_val;

            default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);

            if (BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id) == default_dual_homing_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, An attempt to retrieve data for Virtual Egress Default entry with no VLAN-Port data (0x%08X)\n",
                             vlan_port->vlan_port_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_verify(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_GPORT_IS_VLAN_PORT(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, gport = 0x%08X is not a VLAN PORT\n", gport);
    }

    /*
     * Don't delete a reserved ESEM default entry
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Don't allow destroy of a reserved Egress ESEM Default entry
         */
        int default_ac_val, default_native_val, default_dual_homing_val, ac_profile;
        default_ac_val = dnx_data_esem.default_result_profile.default_ac_get(unit);
        default_native_val = dnx_data_esem.default_result_profile.default_native_get(unit);
        default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
        ac_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport);
        if ((ac_profile == default_ac_val) || (ac_profile == default_native_val)
            || (ac_profile == default_dual_homing_val))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, an attempt to destroy a reserved Virtual Egress Default entry (%d)\n", ac_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_default(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_value_ethertype_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    bcm_port_ethertype_t match_ethertype)
{
    SHR_FUNC_INIT_VARS(unit);
    if (match_ethertype == ETHERTYPE_INITIALIZATION)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                     DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION);
        SHR_EXIT();
    }
    switch (match_ethertype)
    {
        case ETHERTYPE_MPLS:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
            break;
        case ETHERTYPE_IPV4:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
            break;
        case ETHERTYPE_IPV6:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
            break;
        case ETHERTYPE_ARP:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", match_ethertype);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_value_ethertype_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    bcm_port_ethertype_t * match_ethertype)
{
    uint32 layer_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, inst_id, &layer_type));
    switch (layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
            *match_ethertype = ETHERTYPE_MPLS;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
            *match_ethertype = ETHERTYPE_IPV4;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
            *match_ethertype = ETHERTYPE_IPV6;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_ARP:
            *match_ethertype = ETHERTYPE_ARP;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION:
            *match_ethertype = ETHERTYPE_INITIALIZATION;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", layer_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_key_ethertype_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    bcm_port_ethertype_t match_ethertype)
{
    SHR_FUNC_INIT_VARS(unit);
    if (match_ethertype != 0)
    {
        switch (match_ethertype)
        {
            case ETHERTYPE_MPLS:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
                break;
            case ETHERTYPE_IPV4:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
                break;
            case ETHERTYPE_IPV6:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
                break;
            case ETHERTYPE_ARP:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", match_ethertype);
                break;
        }
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, 0x0, 0x0);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp,
    bcm_gport_t local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    int core = SOC_CORE_ALL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));
    /** Create TCAM access id */
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, DBAL_TABLE_IN_AC_TCAM_DB, 3, &entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_c_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_c_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_c_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_untagged(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PORT x VLAN in ingress native configuration.
 *          ISEM lookup (LIF x VID), result is ingress native ac
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 * Parameters:
 *              - bcm_gport_t port (in) - the L2 VPN LIF
 *              - bcm_vlan_t match_vlan (in) - the matched VID
 *              - bcm_gport_t local_in_lif (in) - the ingress native ac LIF
 */
static shr_error_e
dnx_vlan_port_create_ingress_native_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * set the vlan to match.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PORT x VLAN in ingress native configuration.
 *          ISEM lookup (LIF x VID), result is ingress native ac
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 * Parameters:
 *              - bcm_gport_t port (in) - the L2 VPN LIF
 *              - bcm_vlan_t match_vlan (in) - the matched VID
  *              - bcm_vlan_t match_inner_vlan (in) - the inner matched VID
 *              - bcm_gport_t local_in_lif (in) - the ingress native ac LIF
 */
static shr_error_e
dnx_vlan_port_create_ingress_native_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * set the vlans to match.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate qos egress network_qos profile for update AC's nwk_qos_idx mapping
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int out_lif_profile)
{
    uint32 entry_handle_id;
    bcm_vlan_port_t old_vlan_port;
    uint8 is_update = FALSE;
    dbal_enum_value_field_encap_qos_model_e qos_model;
    int nwk_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    bcm_vlan_port_t_init(&old_vlan_port);
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        old_vlan_port.vlan_port_id = vlan_port->vlan_port_id;
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &old_vlan_port));
        is_update = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_type_to_model(unit, vlan_port->egress_qos_model.egress_qos, &qos_model));
    nwk_qos_idx = DNX_QOS_EXPLICIT_IDX_L2(vlan_port->pkt_pri, vlan_port->pkt_cfi);

    
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                /*
                 * Handle ESEM AC Native:
                 */
                /** Get the PWE/Tunnel local Out-LIF using DNX Algo Gport Management */
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                /*
                 * Take table handle
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB,
                                 &entry_handle_id));
                /*
                 * Set keys:
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
                /*
                 * Set values:
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC_STAT);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_idx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, qos_model);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             out_lif_profile);
            }
            else
            {
                uint32 vlan_domain;
                /*
                 * Handle ESEM AC non-native
                 */
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
                /*
                 * Take table handle
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
                /*
                 * Set keys:
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
                /*
                 * Set values:
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_idx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, qos_model);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
                /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             out_lif_profile);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            shr_error_e retVal;
            /*
             * Take table handle
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_port->match_class_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
            /*
             * Check if the entry exist:
             * Note: the ESEM Namespace-vsi entry may be created by bcm_vxlan_network_domain_config_add,
             * thus need to check if already exist.
             */
            retVal = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (retVal == _SHR_E_NOT_FOUND)
            {
                /*
                 * The entry does not exist!
                 *   - Create it with result type set to default (ETPS_L2_FODO).
                 *   - Update relevant fields (vni=0 and out_lif_porfile).
                 */
                /*
                 * Set values:
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             out_lif_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else if (retVal == _SHR_E_NONE)
            {
                /*
                 * The entry exist!
                 *   - Verify it's result type is the default (ETPS_L2_FODO).
                 *   - Update relevant fields (out_lif_porfile).
                 */
                uint32 resultType;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &resultType));
                if (resultType != DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Error!, accessing ESEM table (%d) with the keys VLAN_DOMAIN = %d, VSI = %d, C_VID = %d but"
                                 " resultType =%d (supposed to be %d)\n",
                                 DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port->match_class_id, vlan_port->vsi,
                                 0, resultType, DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO);
                }

                /*
                 * Set values:
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             out_lif_profile);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                /*
                 * Return the error code!
                 */
                SHR_ERR_EXIT(retVal,
                             "Error!, accessing ESEM table (%d) with the keys VLAN_DOMAIN (namespace) = %d, VSI = %d, C_VID = %d failed!!!\n",
                             DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port->match_class_id, vlan_port->vsi, 0);
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            uint32 system_port;
            system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(vlan_port->port);
            /*
             * Take table handle
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_port->match_class_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
            /*
             * Set values:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_AC_STAT);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_idx);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, qos_model);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
            /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                         !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                         out_lif_profile);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add Esem match-info to SW state.
 * Called by bcm_dnx_vlan_port_create
 * Parameters: bcm_vlan_port_t (in)
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem_sw_state(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                /*
                 * Handle ESEM AC Native:
                 */
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                /** Get the PWE/Tunnel local Out-LIF using DNX Algo Gport Management */
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                             gport_hw_resources.local_out_lif);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
            }
            else
            {
                /*
                 * Handle ESEM AC non-native
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE,
                                         vlan_port->match_class_id);
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE,
                                         vlan_port->match_class_id);
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure VLAN PORT egress EEDB Mapping OutLif to
 *        egress AC information
 *
 * Called by bcm_dnx_vlan_port_create
 * \param [in] unit - the relevant unit.
 * \param [in] local_out_lif: Key to table.
 * \param [in] is_native_ac: is the EEDB entry native. 
 * \param [in] egress_ac_table_info: pointer to struct containing:
 *        [in] dbal_table_id: the id of the table.
 *       [in] result_type: result type of the entry.
 *       [in] outlif_profile: outlif profile associated with
 *       this entry.
 *       [in] next_outlif: next EEDB entry, pointed from this
 *       entry.
 *
   */
static shr_error_e
dnx_vlan_port_create_egress_match_eedb(
    int unit,
    uint32 local_out_lif,
    int is_native_ac,
    dnx_egress_ac_table_info_t * egress_ac_table_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egress_ac_table_info->dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 egress_ac_table_info->result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                 egress_ac_table_info->nwk_qos_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                 egress_ac_table_info->protection_path);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                 egress_ac_table_info->protection_pointer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                 egress_ac_table_info->qos_model);

    /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                 egress_ac_table_info->last_eth_layer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                 egress_ac_table_info->out_lif_profile);

    if (is_native_ac == TRUE)
    {
        /** Set pointer to next EEDB entry */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     egress_ac_table_info->next_outlif);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SW state of gport to forward information
 *       according to vlan_port struct and application type.
 *       Also returns the forward information for further use
 *       Called by bcm_dnx_vlan_port_create
 *
 * \param [in] unit - the relevant unit.
 * \param [in] vlan_port - VLAN-Port information, specifically
 *        the port
 * \param [in] ll_app_type - The Link-Layer In-LIF application
 *        type
 * \param [in] glob_out_lif - Global Out-LIF for forward info
 * \param [out] forward_info - Struct to be filled with forward
 *        information
 *
 */
static shr_error_e
vlan_port_gport_to_forward_information_set(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    uint32 glob_out_lif,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    bcm_gport_t destination;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * SW State should be filled only if ingress is configured because it represents
     * the learning information relevant for this gport
     */
    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    /*
     * Handle FEC and 1+1 protection case.
     * In case of FEC Protection use the FEC as the destination else
     * In case of 1+1 Protection use the MC as the destination.
     * otherwise use destination port as the destination.
     */
    if ((vlan_port->failover_port_id))
    {
        destination = vlan_port->failover_port_id;
    }
    else
    {
        if (vlan_port->failover_mc_group)
        {
            _SHR_GPORT_MCAST_SET(destination, _SHR_MULTICAST_ID_GET(vlan_port->failover_mc_group));
        }
        else
        {
            destination = vlan_port->port;
        }
    }

    /*
     * Protection will be added
     */
    if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
    {
        /*
         * Forwarding is done according to destination and Outlif
         */
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &(forward_info->destination)));
        /*
         * In case of FEC and MC Protection Global OutLIF must be 0.
         */
        if (vlan_port->failover_port_id || vlan_port->failover_mc_group)
        {
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        }
        else
        {
            forward_info->outlif = glob_out_lif;
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        }
    }
    else if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
    {
        /*
         * Forwarding is done according to destination only
         */
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &(forward_info->destination)));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: Forwarding info isn't relevant for VLAN-Translation LIFs, vlan_port_id = 0x%08X\n",
                     vlan_port->vlan_port_id);
    }

    /*
     * Fill destination (from Gport) info Forward Info table (SW state)
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit,
                                                       _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE),
                                                       vlan_port->vlan_port_id, forward_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Save vlan port flags in sw state dbal table
 *
 * \param [in] unit - the unit number
 * \param [in] vlan_port - pointer to the vlan port struct,
 *        which contains the relevant flags
 *
 * \return
 *   statis shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_port_ingress_lif_flags_save(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Save statistics, wide flags in SW state table, for later flags retreive
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_OUTER_AC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_STAT_ENABLED_FLAG, INST_SINGLE,
                                 (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE)) ? TRUE :
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_WIDE_ENABLE_FLAG, INST_SINGLE,
                                 (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE)) ? TRUE : FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read vlan port SW state dbal table and determine
 *        which (if any) flags were set: ingress wide enable,
 *        stat enable
 *
 * \param [in] unit - The unit number
 * \param [in] vlan_port - Pointer to the struct to which the
 *        flags will be assigned to
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
dnx_vlan_port_ingress_lif_flags_retreive(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 stat_enabled;
    uint32 ingress_wide_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Read the SW state table to find out which (if any) flag was set
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_OUTER_AC);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INGRESS_STAT_ENABLED_FLAG, INST_SINGLE, &stat_enabled));
    if (stat_enabled)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INGRESS_WIDE_ENABLE_FLAG, INST_SINGLE, &ingress_wide_enabled));
    if (ingress_wide_enabled)
    {
        vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This functions calculates the IN-LIF table id and
 * result_type.
 * \param [in] unit - relevant unit.
 * \param [in] vlan_port - A pointer to a struct containing:
 *        [in] flags - for logical layer 2 port,
 *        BCM_VLAN_PORT_xxx.
 *
 * \param [out] dbal_table_id - the associated table id.
 * \param [out] result_type - the IN-LIF type: Native or outer
 *        (P2P, MP).
 * \param [out] large_entry - whether large entry to be used for non native case.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_table_result_type_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dbal_tables_e * dbal_table_id,
    int *result_type,
    int *large_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    *large_entry = FALSE;
    /*
     * Native AC DBs
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        *dbal_table_id = DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION;
        /** If Cross-connect flag is not set -> MP */
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT))
        {
            /** Glifless (virtual AC) DB */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                *result_type = DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_VLAN_EDIT_VSI_MP;
            }
            /** Gliful (Non virtual) DB */
            else
            {
                *result_type = DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_MP;
            }
        }
        /** Cross-connect is not set -> P2P */
        else
        {
            /** Glifless (virtual AC) DB */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                *result_type = DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_VLAN_EDIT_VSI_P2P;
            }
            /** Gliful (Non virtual) DB */
            else
            {
                *result_type = DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_VSI_P2P;
            }
        }
    }
    /*
     * Outer ACs Database, located in VT1
     */
    else
    {
        /** AC_INFO_DB table commit is done in
         *  dnx_vlan_port_create_ingress_lif_info_set(), called by
         *  the api */
        *dbal_table_id = DBAL_TABLE_IN_AC_INFO_DB;
        /*
         * Handle which LIF type to create based on BCM_VLAN_PORT_CROSS_CONNECT flag:
         *      - P2P: BCM_VLAN_PORT_CROSS_CONNECT is set.
         *      - MP: BCM_VLAN_PORT_CROSS_CONNECT is not set.
         * If large entry terms apply - use large table entries
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE) ||
            DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id) || vlan_port->group ||
            (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE)))
        {
            *large_entry = TRUE;
            *result_type = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT) ?
                DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE : DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE;
        }
        else
        {
            *result_type = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT) ?
                DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE : DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -
* This functions calculates the OUT-LIF table id and
* result_type.
* \param [in] unit - relevant unit.
* \param [in] vlan_port - A pointer to a struct containing:
* \param [out] dbal_table_id - the associated table id.
* \param [out] result_type - the OUT-LIF type Native or outer
*        (MP, P2P).
* \param [out] outlif_phase - phase of the entry.
*
* \return
*   shr_error_e
*
* \remark
*   * None
* \see
* bcm_dnx_vlan_port_create
*/
static shr_error_e
dnx_vlan_port_create_egress_non_virtual_lif_table_result_type_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dbal_tables_e * dbal_table_id,
    int *result_type,
    int *outlif_phase)
{

    SHR_FUNC_INIT_VARS(unit);
    *dbal_table_id = DBAL_TABLE_EEDB_OUT_AC;
    /*
     * Result type can be:
     * 1. with next pointer for Native AC, entry will point on the overly tunnel given in vlan_port->tunnel_id parameter
     * 2. without next pointer for all others
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        *result_type = DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_BASIC_W_NEXT_POINTER;
    }
    else
    {
        *result_type = DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION;
    }
    /*
     * Native AC Database
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        *outlif_phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_AC;
    }
    /*
     * Outer AC Database
     */
    else
    {

        *outlif_phase = LIF_MNGR_OUTLIF_PHASE_AC;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function updates IN-LIF Info table of the created IN-LIF with
 * the following info:
 *  - result type.
 *  - Global IN-LIF.
 *  - vsi.
 *  - fodo_assignment_mode.
 *  - Learn information
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - the logical layer 2 flags - BCM_VLAN_PORT_xxx.
 * \param [in] local_in_lif - local IN LIF ID.
 * \param [in] ingress_ac_table_info - pointer to a struct
 *        containing:
 *       [in] dbal_table_id - ID of the associated table.
 *       [in] global_lif - global IN-LIF ID.
 *       [in] result_type - Result type of the associated entry.
 *       [in] vsi - the VSI. [in] fodo_assignment_mode - the
 *       forwarding domain assignment mode.
 * \param [in] large_entry - whether large entry used at non native case.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vlan_port_create
 */
shr_error_e
dnx_vlan_port_create_ingress_lif_info_set(
    int unit,
    uint32 flags,
    int local_in_lif,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    int large_entry)
{
    uint32 entry_handle_id, full_dbal_struct_data = 0;
    int is_native, has_vsi, is_p2p, is_native_virtual, is_virtual, has_global_lif, has_learning, has_protection;
    uint32 vlan_extend_pcp_dei_profile, pcp_dei_profile, pcp_dei_map_extend;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_native = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_NATIVE);
    is_p2p = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_CROSS_CONNECT);
    is_virtual = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    is_native_virtual = is_native && is_virtual;

    
    has_vsi = ((!is_native) && is_p2p) ? FALSE : TRUE;

    has_global_lif = (!is_native) || (is_native && !is_native_virtual);

    /*
     * Note: HW wise P2P Large In-LIF has learning info, but it's not supported by the SW
     */

    has_learning = (!is_p2p) && (!is_native_virtual);
    has_protection = (((!is_native) && large_entry) || (is_native && (!is_p2p) && (!is_virtual)));

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ingress_ac_table_info->dbal_table_id, &entry_handle_id));

    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);

    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 ingress_ac_table_info->result_type);

    /** AC only 1 bit for propagation profile, native ac use uniform, outer ac use pipe*/
    if (is_native)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                     DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                     DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE);
    }

    if (has_vsi)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, ingress_ac_table_info->vsi);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     ingress_ac_table_info->fodo_assignment_mode);
    }

    if (has_global_lif)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                     ingress_ac_table_info->in_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE,
                                     ingress_ac_table_info->global_lif);
    }

    /*
     * Set the Learn information for applicable In-LIF types
     */
    if (has_learning)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE,
                                     ingress_ac_table_info->learn_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                     ingress_ac_table_info->learn_context);
        /*
         *  Set the learn context and info for an Outer-AC
         */
        if (is_native == FALSE)
        {
            switch (ingress_ac_table_info->learn_context)
            {
                case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC:
                case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC:
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, DBAL_FIELD_DESTINATION,
                                     &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
                    dbal_entry_value_field32_set(unit, entry_handle_id,
                                                 DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, INST_SINGLE,
                                                 full_dbal_struct_data);
                    break;
                case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED:
                case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED:
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                                     &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                                     &ingress_ac_table_info->flush_group, &full_dbal_struct_data));
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
                                                 INST_SINGLE, full_dbal_struct_data);
                    break;
                case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED:
                    if (large_entry)
                    {
                        dbal_entry_value_field32_set(unit, entry_handle_id,
                                                     DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, INST_SINGLE,
                                                     LIF_LEARN_INFO_OPTIMIZED_PREFIX);
                    }
                    else
                    {
                        dbal_entry_value_field32_set(unit, entry_handle_id,
                                                     DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, INST_SINGLE,
                                                     LIF_LEARN_INFO_OPTIMIZED_PREFIX);
                    }
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported AC Learn Context - %d\n",
                                 ingress_ac_table_info->learn_context);
            }
        }

        /*
         *  Set the learn context and info for a Native-AC.
         *  The only supported native result type IN_ETH_AC_MP, uses the DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED
         *  context
         */
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                             &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                             &ingress_ac_table_info->flush_group, &full_dbal_struct_data));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
                                         INST_SINGLE, full_dbal_struct_data);
        }
    }

    /*
     * setting ingress protection information for appropriate result types
     */
    if (has_protection)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     ingress_ac_table_info->protection_pointer);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                     ingress_ac_table_info->protection_path);
    }

    if (is_p2p)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
    }

    /*
     * setting ingress default pcp_dei_extend_profile with explicit map profile 0
     */
    vlan_extend_pcp_dei_profile = 0;
    pcp_dei_profile = 0;
    pcp_dei_map_extend = DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_QOS_PROFILE, &pcp_dei_profile,
                     &vlan_extend_pcp_dei_profile));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_MAP_EXTEND, &pcp_dei_map_extend,
                     &vlan_extend_pcp_dei_profile));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE,
                                 vlan_extend_pcp_dei_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Create a virtual gport (Non-lif) for egress AC.
 *  The virtual gport is encoded with index for ESEM default result table,
 *  or a handle to egress vitual gport table according to criteria.
 *
 * \param [in] unit - the relevant unit.
 * \param [in,out] vlan_port - Vlan port information.
 *                 vlan_port_id is updated with gport encoded with virtual subtype.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Here create the virtual gport only. The related hw configuration is done
 *  in other functions.
 *
 * \see
 *  dnx_vlan_port_egress_port_default_create
 *  dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_id_egress_virtual_gport_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int esem_handle;
    uint32 sw_handle_alloc_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Create virtual gport based on flags:
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
    {
        /** Virtual gport is for ESEM-default match entries */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            /*
             * SW handle can be allocated WITH_ID
             */
            sw_handle_alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
            esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
        }

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single(unit,
                                                                                    sw_handle_alloc_flags,
                                                                                    NULL, &esem_handle));
        /** Encode the esem_handle as vitual gport with port-default subtype.*/
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(vlan_port->vlan_port_id, esem_handle);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }
    else
    {
        /** Virtual gport is for ESEM match entries */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            /*
             * SW handle can be allocated WITH_ID
             */
            sw_handle_alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
            esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
        }
        SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.allocate_single(unit,
                                                                                  sw_handle_alloc_flags,
                                                                                  NULL, &esem_handle));
        /** Encode the esem_handle as virtual gport with match subtype. */
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(vlan_port->vlan_port_id, esem_handle);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Configure ESEM_DEFAULT_RESULT_TABLE with vlan_port information.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] vlan_port - Vlan port information.
 * \param [in] out_lif_profile - out-lif-profile according to vlan_port.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The index is retrieved from vlan_port_id which is encoded as a virtual gport.
 *  The virtual gport is created by bcm_vlan_port_create with flag of
 *  BCM_VLAN_PORT_VLAN_TRANSLATION and criteria of BCM_VLAN_PORT_MATCH_NONE.
 *
 * \see
 *  dnx_vlan_port_id_egress_virtual_gport_create
 */
static shr_error_e
dnx_vlan_port_egress_port_default_create(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int out_lif_profile)
{
    uint32 entry_handle_id;
    int esem_default_result_profile;
    int egress_last_layer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Retrieve esem_default_result_profile from vlan_port_id*/
    esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /** Check esem_default_result_profile is valid*/
    if ((esem_default_result_profile < 0)
        || (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vlan_port_id (0x%08X) is not a valid virtual gport for esem default entry!\n",
                     vlan_port->vlan_port_id);
    }

    /** Determine if it is for out-ac*/
    egress_last_layer = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? FALSE : TRUE;
    /** Configure ESEM default table*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                               esem_default_result_profile);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Creates a virtual native-ac entry in the inlif table that can be used to edit inner-eth vlan tags.
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 * \param [in] inlif_info: Struct containing the
 *        following fields (in lif info):
 *       [in] dbal_table_id - ID of table.
 *       [in] result_type - result type of this entry
 *
 *
 * \return shr_error_e Standard error handling
 */
static shr_error_e
dnx_vlan_port_ingress_virtual_native_ac_allocate(
    int unit,
    bcm_vlan_port_t * vlan_port,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    int sw_handle;
    uint32 entry_handle_id;
    uint32 global_lif_flags;
    uint32 sw_handle_alloc_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    global_lif_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, global_lif_flags, NULL, inlif_info, NULL));
    /*
     * Next - allocate a SW handle and set the local lif in the SW DB
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        /*
         * SW handle can be allocated WITH_ID
         */
        sw_handle_alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
    }
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.allocate_single(unit,
                                                                               sw_handle_alloc_flags,
                                                                               NULL, &sw_handle));
    /*
     * Encode GPort subtype and type
     */
    BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SET(vlan_port->vlan_port_id, sw_handle);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    /*
     * Push to SW DB
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_VIRTUAL_NATIVE_AC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, INST_SINGLE, inlif_info->local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function for calculating Ingress Forwarding Domain (FODO) assignment mode.
 * \param [in] unit - Unit #
 * \param [in] vlan_port - vlan port info.
 * \param [in] ll_app_type - Link-Layer LIF application type.
 * \param [out] fodo_assignment_mode - Forwarding Domain (FODO)
 *        assignment mode.
 * \return shr_error_e Standard error handeling
 */

static shr_error_e
dnx_vlan_port_create_ingress_fodo_mode_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    int *fodo_assignment_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    *fodo_assignment_mode = FODO_ASSIGNMENT_MODE_INVALID;
    /*
     * AC Optimization LIF - Fodo is an In-LIF Base value plus a VID value
     */
    if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_VLAN;
    }
    else
    {
        /*
         * AC Service LIF or AC VLAN Translation - Fodo from the In-LIF
         */
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LIF;
    }

    if (*fodo_assignment_mode == FODO_ASSIGNMENT_MODE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "dnx_vlan_port_create_ingress_fodo_calc: Error - something went wrong!!! FODO mode was not set! flags = 0x%08X, vsi = %d, criteria = %d\n",
                     vlan_port->flags, vlan_port->vsi, vlan_port->criteria);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get LIF's inlif-profile
* \see
*  bcm_dnx_vlan_port_destroy
*/
static shr_error_e
dnx_ingress_inlif_profile_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 *in_lif_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    /*
     * Get all fields:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get in_lif_profile:
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_ingress_inlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int in_lif_profile,
    int *new_in_lif_profile,
    dbal_tables_e inlif_dbal_table_id)
{
    in_lif_profile_info_t in_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get in_lif_porfile data:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    /*
     * Update in_lif_porfile with the new incoming_orientation value:
     */
    in_lif_profile_info.egress_fields.in_lif_orientation = network_group_id;
    /*
     * Exchange in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, in_lif_profile, new_in_lif_profile, LIF, inlif_dbal_table_id));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Deallocs inlif-profile.
* \see
*  bcm_dnx_vlan_port_destroy
*/
shr_error_e
dnx_ingress_inlif_profile_dealloc(
    int unit,
    uint32 in_lif_profile)
{
    int new_in_lif_profile;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Free in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function updates SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW) with
 * the following info:
 * - flags
 * - vsi
 * - port
 * - match_vlan
 * - match_inner_vlan
 * - match_pcp
 * - nw group id
 * \see
 *  bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_match_info_set(
    int unit,
    int local_in_lif,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    int is_native_ac;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    /*
     * Take table handle and set core key, according to native indication.
     */
    if (is_native_ac)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    /*
     * Set inlif key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);
    /*
     * Set values:
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, 0);
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                   vlan_port->match_ethertype);
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_vlan);
            /*
             * Ethertype is not supported for Ingress Native.
             */
            if (!is_native_ac)
            {
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_vlan);
            dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                   vlan_port->match_ethertype);
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_vlan);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_inner_vlan);
            /*
             * Ethertype is not supported for Ingress Native.
             */
            if (!is_native_ac)
            {
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_vlan);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, vlan_port->match_pcp);
            dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                   vlan_port->match_ethertype);
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_vlan);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                         vlan_port->match_inner_vlan);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, vlan_port->match_pcp);
            dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                   vlan_port->match_ethertype);
            break;
        }

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_UNTAGGED);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                   vlan_port->match_ethertype);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "dnx_vlan_port_create_ingress_lif_match_info_set: Error!! criteria = %d is unknown!",
                         vlan_port->criteria);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the following info from SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW):
 * - port
 * - criteria
 * - match_vlan
 * - match_inner_vlan
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_match_info_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 result_type, tmp32bit;
    int is_native_ac;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle and set core key, according to native indication.
     * The In-LIF can be virtual native or non-virtual native -
     */
    is_native_ac =
        (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ? TRUE : FALSE;

    if (is_native_ac)
    {
        /*
         * Set table:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        /*
         * Set keys:
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, gport_hw_resources->local_in_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
        /*
         * Get values:
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        switch (result_type)
        {
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "dnx_vlan_port_create_ingress_lif_match_info_get: Error!! result_type = %d is unknown!",
                             result_type);
            }
        }
    }
    else
    {

        /*
         * Set table:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        /*
         * Set inlif key:
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, gport_hw_resources->local_in_lif);
        /*
         * Get values:
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        switch (result_type)
        {
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                break;
            }

            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_UNTAGGED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;

                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));

                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;

                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "dnx_vlan_port_create_ingress_lif_match_info_get: Error!! result_type = %d is unknown!",
                             result_type);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears the entry in the SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW).
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_lif_match_info_clear(
    int unit,
    int is_native_ac,
    int local_in_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle and set core key, according to native indication.
     */
    if (is_native_ac)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    /*
     * Set inlif key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_egress_outlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int out_lif_profile,
    int *new_out_lif_profile,
    dbal_tables_e dbal_table_id,
    uint32 flags)
{
    out_lif_profile_info_t out_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get out_lif_porfile data:
     */
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info, dbal_table_id));
    /*
     * Update out_lif_porfile with the new outgoing_orientation value:
     */
    out_lif_profile_info.out_lif_orientation = network_group_id;
    /*
     * Exchange out_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, out_lif_profile, new_out_lif_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Frees AC outlif-profile.
* \see
*  bcm_dnx_vlan_port_destroy
*/
shr_error_e
dnx_egress_ac_outlif_profile_free(
    int unit,
    dbal_tables_e dbal_table_id,
    int local_out_lif,
    uint32 result_type,
    bcm_vlan_t vsi,
    uint32 vlan_domain,
    uint32 esem_default_result_profile,
    uint32 system_port)
{
    uint32 entry_handle_id;
    uint32 out_lif_profile;
    int new_out_lif_profile;
    out_lif_profile_info_t out_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    switch (dbal_table_id)
    {
        case DBAL_TABLE_EEDB_OUT_AC:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            break;
        }

        case DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
            break;
        }

        case DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                                       esem_default_result_profile);
            break;
        }
        case DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error! table_id = %d is wrong!\n", dbal_table_id);
        }

    }

    /*
     * Get out_lif_profile:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));
    /*
     * Free out_lif_porfile:
     * It is done by calling exchange function with default profile data:
     */
    out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, out_lif_profile, &new_out_lif_profile));
    if (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB)
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE, new_out_lif_profile);
        }
    }
    else
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function determening the application of the
 * specified Link-Layer LIF
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains the configuration information that is required to
 *   determine the application type.
 * \param [out] ll_app_type - The type of Link-Layer AC 
 *   application that is determined by this function.
 * \return shr_error_e Standard error handeling
 */
static shr_error_e
dnx_vlan_port_set_ll_app_type(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e * ll_app_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    *ll_app_type = INVALID_AC_APPLICATION_TYPE;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VSI_BASE_VID))
    {
        /*
         * AC Optimization LIF
         */
        *ll_app_type = OPTIMIZATION_AC_APPLICATION_TYPE;
    }
    else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
    {
        *ll_app_type = VLAN_TRANSLATION_AC_APPLICATION_TYPE;
    }
    else
    {
        if (vlan_port->vsi == 0)
        {
            /*
             * AC Service LIF
             */
            *ll_app_type = SERVICE_AC_APPLICATION_TYPE;
        }
        else
        {
            /*
             * AC VLAN Translation LIF
             */
            *ll_app_type = VLAN_TRANSLATION_AC_APPLICATION_TYPE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function for storing the required AC learning info
 * within the In-LIF structure
 * \param [in] unit - Unit #
 * \param [in] ingress_ac_table_info - Pointer to an internal
 *        Ingress info structure that also holds the learn info.
 * \param [in] forward_info - Pointer to the forward information
 *        that also serves as a learn information.
 * \param [in] ll_app_type - The type of Link-Layer AC 
 *        application. Required in oredr to determine the learn
 *        info values.
 * \param [in] is_large_in_lif - Boolean that determines whether
 *        the configured In-LIF is of a Large format
 * \param [in] is_egress - Boolean that determines whether the
 *        In-LIF is symmetric.
 * \param [in] is_cross_connect - Boolean that determines
 *        whether the In-LIF is of cross-connect type.
 * \return shr_error_e Standard error handeling
 */
static shr_error_e
dnx_vlan_port_learn_info_fill(
    int unit,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    dnx_algo_gpm_forward_info_t * forward_info,
    ac_application_type_e ll_app_type,
    int is_large_in_lif,
    int is_egress,
    int is_cross_connect)
{
    int is_native =
        (ingress_ac_table_info->dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ? TRUE : FALSE;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Set the learn enable by default to FALSE for VLAN-Translation ACs and for asymmetrical Optimization ACs.
     * For Service ACs the learn is only temporarily disabled as it becomes enabled when the user associates the
     * VLAN-Port with a VSI using bcm_vswitch_port_add().
     */
    ingress_ac_table_info->learn_enable = FALSE;
    /*
     * Fill learn information for an Outer-AC
     */
    if (is_native == FALSE)
    {
        /*
         * Set a default learn context for the cases where no real learning is required, in order to use a
         * correct learn info size
         */
        if (is_large_in_lif)
        {
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED;
        }
        else
        {
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC;
        }

        /*
         * Cross-Connect ACs may require learning when the opposite side is MP. The learn information is not from
         * the In-LIF - It is taken from the Src-System-Port and Global Out-LIF
         * This option isn't supported by the API for now.
         */
        if (is_cross_connect)
        {
            ingress_ac_table_info->learn_enable = FALSE;
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED;
            ingress_ac_table_info->learn_info = 0;
        }
        else
        {
            /*
             * Set the learn context according to the AC application and whether it's large In-LIF entry.
             * Default values are used in case the learn info isn't required.
             */
            if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
            {
                if (is_large_in_lif)
                {
                    ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED;
                }
                else
                {
                    ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC;
                }
            }
            else if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
            {
                ingress_ac_table_info->learn_enable = TRUE;
                ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED;
            }

            /*
             * Set the learn information to the Forward information destination for all MP cases (even when not relevant)
             * The strength and is always 0, entry group setting isn't supported.
             */
            ingress_ac_table_info->learn_info = forward_info->destination;
        }
    }
    else
    {
        /*
         * Set the learn information to the Forward information destination for all MP cases (even when not relevant)
         * The strength is always 0, entry group setting isn't supported.
         */
        ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
        ingress_ac_table_info->learn_info = 0;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API creates a VLAN PORT.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   Relevant unit.
 *   \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 *  \b As \b input - \n
 *   vlan_port->flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   BCM_VLAN_PORT_STAT_xxx_ENABLE - marks enabling
 *   statistics. When on, a table entry with statistics will be
 *   chosen.
 *   vlan_port->vlan_port_id
 *   - if vlan_port->flags contain BCM_VLAN_PORT_WITH_ID, then
 *   this variable contain the gport that has to be encoded with
 *   at least a type and a subtype. 
 *   vlan_port->port - Gport,local or remote Physical or logical gport.
 *   vlan_port->criteria - logical layer 2 port match criteria.
 *   vlan_port->match_vlan - Outer VLAN ID to match.
 *   vlan_port->match_inner_vlan - Inner VLAN ID to match.
 *   vlan_port->tunnel_id - Pointer from native AC to next entry
 *   in the EEDB list. Only relevant for egress native AC entries
 *   signaled by flag BCM_VLAN_PORT_NATIVE.
 *   vlan_port->egress_qos_model - qos pipe or uniform model   
 *  \b As \b output - \n
 *   vlan_port->vlan_port_id - if vlan_port->flags does not
 *   contain BCM_VLAN_PORT_WITH_ID, the procedure will load
 *   this variable with the value of the newly allocated VLAN
 *   PORT ID (Gport encoded with at least a type and subtype).
 *   vlan_port->encap_id - contains the global outlif.
 * \par INDIRECT INPUT:
 *  \b vlan_port - see `vlan_port' in DIRECT_INPUT above.
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 * \b vlan_port - see `vlan_port' in DIRECT_INPUT above.
 * \b DBAL_TABLE_GPORT_TO_FORWARDING_SW_INFO -\n
 *  Fields DBAL_FIELD_DESTINATION and DBAL_FIELD_OUT_LIF are
 * updated.
 * \b DBAL_TABLE_IN_AC_INFO_DB -\n
 * Fields DBAL_FIELD_GLOB_IN_LIF and
 * DBAL_FIELD_VSI are updated.
 * \b DBAL_TABLE_INGRESS_PP_PORT -\n
 * field DBAL_FIELD_DEFAULT_LIF is updated.
 * \b DBAL_TABLE_EGRESS_DEFAULT_AC_PROF -\n
 * fields DBAL_FIELD_ACTION_PROFILE, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_NWK_QOS_IDX, DBAL_FIELD_LAYER_TYPES,
 * DBAL_FIELD_QOS_MODEL, DBAL_FIELD_VLAN_EDIT_PROFILE,
 * DBAL_FIELD_VLAN_EDIT_VID_1 and DBAL_FIELD_VLAN_EDIT_VID_2 are
 * updated.
 * \b DBAL_TABLE_IN_AC_S_C_VLAN_DB -\n
 * field DBAL_FIELD_IN_LIF is updated.
 * \b DBAL_TABLE_EEDB_OUT_AC -\n
 * fields DBAL_FIELD_VLAN_EDIT_VID_1,
 * DBAL_FIELD_VLAN_EDIT_VID_2, DBAL_FIELD_VLAN_EDIT_PROFILE,
 * DBAL_FIELD_ACTION_PROFILE, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_OAM_LIF_SET, DBAL_FIELD_NWK_QOS_IDX,
 * DBAL_FIELD_PROTECTION_PATH, DBAL_FIELD_PROTECTION_POINTER,
 * DBAL_FIELD_STAT_OBJECT_CMD and DBAL_FIELD_QOS_MODEL are
 * updated.
 * \b DBAL_TABLE_IN_AC_S_VLAN_DB -\n
 * field DBAL_FIELD_IN_LIF is updated.
 * \b DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB -\n
 * fields DBAL_FIELD_VLAN_EDIT_VID_1,
 * DBAL_FIELD_VLAN_EDIT_VID_2, DBAL_FIELD_ACTION_PROFILE,
 * DBAL_FIELD_VLAN_EDIT_PROFILE, DBAL_FIELD_NWK_QOS_IDX,
 * DBAL_FIELD_REMARK_PROFILE, DBAL_FIELD_QOS_MODEL,
 * DBAL_FIELD_OAM_LIF_SET, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_PROTECTION_PATH, DBAL_FIELD_PROTECTION_POINTER and
 * DBAL_FIELD_STAT_OBJECT_CMD are updated.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_port_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress, is_out_lif, is_lif, is_native_ac, is_virtual_ac, is_update;
    int global_lif, protection_pointer, ingress_large_entry = FALSE;
    lif_mngr_local_inlif_info_t inlif_info;
    lif_mngr_local_outlif_info_t outlif_info;
    uint32 global_lif_flags;
    dnx_ingress_ac_table_info_t ingress_ac_table_info;
    dnx_egress_ac_table_info_t egress_ac_table_info;
    dbal_tables_e inlif_dbal_table_id;
    ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;
    dnx_algo_gpm_forward_info_t forward_info = { 0 };
    bcm_vlan_port_t old_vlan_port;
    dbal_enum_value_field_encap_qos_model_e qos_model;
    int nwk_qos_idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);
    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    /*
     * Calc once - is the required vlan port a native (inner) AC lif.
     */
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    /*
     * Get an is_virtual indication
     */
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    /*
     * ESEM and Egress-Default-AC configurations allocate a non-LIF entry
     */
    is_out_lif = is_egress && !is_virtual_ac;
    is_lif = (is_ingress && !is_virtual_ac) || is_out_lif;
    /*************************************************************************************/
    /****************************** API parameters verification **************************/
    /*************************************************************************************/
    /*
     * Determine the Link-Layer LIF application type: Service, VLAN-Translation or Optimization
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_create_verify(unit, vlan_port, ll_app_type, is_virtual_ac));

    /*************************************************************************************/
    /****************************** SW configurations ************************************/
    /*************************************************************************************/

    
    bcm_vlan_port_t_init(&old_vlan_port);
    is_update = FALSE;
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        old_vlan_port.vlan_port_id = vlan_port->vlan_port_id;
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &old_vlan_port));
        is_update = TRUE;
    }

    /*
     * Initialize containers for ingress and egress AC tables (all types, including ESEM)
     */
    sal_memset(&ingress_ac_table_info, 0, sizeof(ingress_ac_table_info));
    ingress_ac_table_info.protection_pointer = dnx_data_failover.path_select.ing_no_protection_get(unit);
    sal_memset(&egress_ac_table_info, 0, sizeof(egress_ac_table_info));
    egress_ac_table_info.protection_pointer = dnx_data_failover.path_select.egr_no_protection_get(unit);
    /*
     * Initialize inlif info for allocation manager. Needed for: Ingress non virtual AC (native or not). OR Ingress
     * virtual AC (native) for glifless entry (with dummy global lif, local lif). See
     * dnx_vlan_port_ingress_virtual_native_ac_allocate Initialize outlif info for allocation manager. Needed for:
     * Egress non virtual AC (native or not).
     */
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_table_result_type_calc
                        (unit, vlan_port, &(ingress_ac_table_info.dbal_table_id),
                         &(ingress_ac_table_info.result_type), &ingress_large_entry));
        inlif_info.dbal_table_id = ingress_ac_table_info.dbal_table_id;
        inlif_info.dbal_result_type = ingress_ac_table_info.result_type;
        inlif_info.core_id = _SHR_CORE_ALL;
    }
    if (is_egress && !is_virtual_ac)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_non_virtual_lif_table_result_type_calc
                        (unit, vlan_port, &(egress_ac_table_info.dbal_table_id),
                         &(egress_ac_table_info.result_type), &(egress_ac_table_info.outlif_phase)));
        outlif_info.dbal_table_id = egress_ac_table_info.dbal_table_id;
        outlif_info.dbal_result_type = egress_ac_table_info.result_type;
        outlif_info.outlif_phase = egress_ac_table_info.outlif_phase;
    }

    /*************************************************************************************/
    /****************************** SW Allocations ************************************/
    /*************************************************************************************/
    /*
     * LIF allocation via DNX SW Algorithm for non virtual ACs (native and outer ACs)
     */
    if (is_lif)
    {
        global_lif_flags = LIF_MNGR_L2_GPORT_GLOBAL_LIF;
        global_lif = 0;
        /*
         * Note:
         * Flags BCM_VLAN_PORT_WITH_ID and BCM_VLAN_PORT_ENCAP_WITH_ID should indicate same global_lif
         * ID in case both are set, thus can take either one of them!
         * See dnx_vlan_port_create_verify.
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            /*
             * BCM_VLAN_PORT_WITH_ID means:
             * Use the specific LIF ID value from vlan_port->vlan_port_id
             */
            global_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
            global_lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ENCAP_WITH_ID))
        {
            /*
             * BCM_VLAN_PORT_ENCAP_WITH_ID means:
             * Use the specific LIF ID value from vlan_port->encap_id
             */
            global_lif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
            global_lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
        {
            /*
             * BCM_VLAN_PORT_ALLOC_SYMMETRIC means:
             * single side allocation of lif which is free in both sides
             */
            global_lif_flags |= LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF;
        }

        /*
         * Both ingress and egress are allocated
         */
        if (is_ingress && is_egress)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, global_lif_flags, &global_lif, &inlif_info, &outlif_info));
            /*
             * Update vlan_port_id with the allocated new global lif.
             * Encode subtype:
             */
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, 0, global_lif);
            vlan_port->encap_id = global_lif;
        }
        /*
         * Only ingress is allocated
         */
        else if (is_ingress)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, global_lif_flags, &global_lif, &inlif_info, NULL));
            /*
             * Update vlan_port_id with the allocated new global lif.
             * Encode subtype:
             */
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, global_lif);
            vlan_port->encap_id = 0;
        }
        /*
         * Only egress is allocated
         */
        else
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, global_lif_flags, &global_lif, NULL, &outlif_info));
            /*
             * Update vlan_port_id with the allocated new global lif.
             * Encode subtype:
             */
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_lif);
            vlan_port->encap_id = global_lif;
        }

        /*
         * Update vlan_port_id with the allocated new global lif.
         * Encode type:
         */
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);

        /*
         * Map Local In-LIF to Global In-LIF.
         * Note: mapping between Local and global Out-LIF is done by LIF DNX SW Algorithm via PEMLA.
         * see dnx_lif_lib_allocate.
         */
        if (is_ingress == TRUE)
        {
            ingress_ac_table_info.vsi = vlan_port->vsi;

            /*
             * Save wide, stat flags in SW state for get() api
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_lif_flags_save(unit, vlan_port));

            /*
             * Calculate IN-LIF info:
             *  - fodo - Forwarding Domain Assignment Mode
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                            (unit, vlan_port, ll_app_type, &(ingress_ac_table_info.fodo_assignment_mode)));

            /*
             * Allocate IN-LIF profile:
             * Since we are in vlan port create function, call the function with default inlif profile (no need to read it from inlif table):
             */
            inlif_dbal_table_id = ingress_ac_table_info.dbal_table_id;
            SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_alloc
                            (unit, vlan_port->ingress_network_group_id, DNX_IN_LIF_PROFILE_DEFAULT,
                             &(ingress_ac_table_info.in_lif_profile), inlif_dbal_table_id));
            /*
             * Forward Info and learning are for Service and Optimization application
             */
            if (ll_app_type != VLAN_TRANSLATION_AC_APPLICATION_TYPE)
            {
                /*
                 * Store the forwarding information for the gport in the dedicated SW DB
                 * and use it for filling the learn info
                 */
                SHR_IF_ERR_EXIT(vlan_port_gport_to_forward_information_set
                                (unit, vlan_port, ll_app_type, global_lif, &forward_info));
            }

            /*
             * Store learning information according to the Application type
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_learn_info_fill
                            (unit, &ingress_ac_table_info, &forward_info, ll_app_type, ingress_large_entry,
                             is_egress, _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT)));
            /*
             * Update Ingress SW state match info:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_set(unit, inlif_info.local_inlif, vlan_port));
            /*
             * Set the global outlif field only for symmetrical LIFs to use for learning,
             * same interface filtering etc
             */
            if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
            {
                ingress_ac_table_info.global_lif = global_lif;
            }
        }

        /*
         * Protection information
         */
        if (is_ingress == TRUE && DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
        {
            ingress_ac_table_info.protection_path = (vlan_port->ingress_failover_port_id) ? 0 : 1;
            DNX_FAILOVER_ID_GET(protection_pointer, vlan_port->ingress_failover_id);
            /** Encode IN-LIF protection pointer format from Failover ID */
            DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(ingress_ac_table_info.protection_pointer, protection_pointer);
        }
        if (is_egress == TRUE && DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->egress_failover_id))
        {
            egress_ac_table_info.protection_path = (vlan_port->egress_failover_port_id) ? 0 : 1;
            DNX_FAILOVER_ID_GET(egress_ac_table_info.protection_pointer, vlan_port->egress_failover_id);
        }

        /*
         * flush group information
         */
        if (is_ingress == TRUE && vlan_port->group)
        {
            ingress_ac_table_info.flush_group = vlan_port->group;
        }

        if (is_egress == TRUE)
        {
            /*
             * Native AC non virtual table.
             */
            if (is_native_ac)
            {
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                uint32 lif_flags;

                /*
                 * Get local out lifs and pointer to the next
                 *  tunnel in the EEDB linked list. 
                 */
                lif_flags =
                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, vlan_port->tunnel_id, lif_flags, &gport_hw_resources));

                /*
                 * In case the tunnel LIF was not found (the gport is valid but either it was deleted or not yet allocated,
                 * don't return an error, just set the next pointer to NULL.
                 */
                egress_ac_table_info.next_outlif = 0;

                if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
                {
                    egress_ac_table_info.next_outlif = gport_hw_resources.local_out_lif;
                }
                egress_ac_table_info.last_eth_layer = 0;
            }
            else
            {
                egress_ac_table_info.last_eth_layer = 1;
            }
        }
    }
    else if (is_virtual_ac)
    {
        /** Currently, virtual ac can't be created for both ingress and egress in the same call.*/
        if ((is_ingress == TRUE) && is_native_ac)
        {
            ingress_ac_table_info.vsi = vlan_port->vsi;
            /*
             * Calculate IN-LIF info:
             *  - fodo - Forwarding Domain Assignment Mode
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                            (unit, vlan_port, ll_app_type, &(ingress_ac_table_info.fodo_assignment_mode)));

            /*
             * Ingress native AC allocation
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_virtual_native_ac_allocate(unit, vlan_port, &inlif_info));
            /*
             * Update Ingress SW state match info:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_set(unit, inlif_info.local_inlif, vlan_port));
        }

        if (is_egress == TRUE)
        {
            /** Create a Non-LIF vlan port for ETH EVE */
            SHR_IF_ERR_EXIT(dnx_vlan_port_id_egress_virtual_gport_create(unit, vlan_port));
        }
    }

    /*
     * Allocate outlif profile for:
     *  - EEDB
     *  - ESEM Default
     *  - ESEM AC
     *  - ESEM Namespace-vsi or Namespace-port
     */
    if (is_out_lif == TRUE
        || (is_egress && vlan_port->criteria == BCM_VLAN_PORT_MATCH_NONE && is_virtual_ac) || (is_egress
                                                                                               &&
                                                                                               is_virtual_ac
                                                                                               &&
                                                                                               (vlan_port->criteria
                                                                                                ==
                                                                                                BCM_VLAN_PORT_MATCH_PORT_VLAN))
        || (is_egress && is_virtual_ac
            && ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                || (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_PORT))))
    {
        if ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN)
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) && (is_egress == TRUE && !is_out_lif))
        {
            SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                            (unit, vlan_port->egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE,
                             &(egress_ac_table_info.out_lif_profile),
                             DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, 0));
        }
        else if ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN)
                 && !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) && (is_egress == TRUE && !is_out_lif))
        {
            SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                            (unit, vlan_port->egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE,
                             &(egress_ac_table_info.out_lif_profile), DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0));
        }
        else if ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI))
        {
            SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                            (unit, vlan_port->egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE,
                             &(egress_ac_table_info.out_lif_profile), DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0));
        }
        else
        {
            /*
             * Allocate OUT-LIF profile:
             * Since we are in vlan port create function, call the function with default outlif profile (no need to read it from outlif table):
             */
            SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                            (unit, vlan_port->egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT,
                             &(egress_ac_table_info.out_lif_profile), egress_ac_table_info.dbal_table_id, 0));
        }
    }

    /*************************************************************************************/
    /****************************** HW configurations ************************************/
    /*************************************************************************************/

    if (is_ingress == TRUE)
    {
        /*
         * Set IN-LIF info:
         *  - result_type (P2P, MP, etc)
         *  - global_lif
         *  - vsi (only if MP)
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_info_set
                        (unit, vlan_port->flags, inlif_info.local_inlif, &ingress_ac_table_info, ingress_large_entry));
    }

    /*
     * Match criteia can be either PORT (default configuration per port)
     * or other (per VLAN for example)
     * Relevant for both ingress and egress
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            /*
             * BCM_VLAN_PORT_DEFAULT means creating entry in ESEM default entry
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                if ((is_egress == TRUE) && (is_virtual_ac == TRUE))
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_egress_port_default_create
                                    (unit, vlan_port, egress_ac_table_info.out_lif_profile));
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            if (is_ingress == TRUE)
            {
                /*
                 * Ingress default action
                 * Note: no need to check if Native because match port is not supported for Ingress Native.
                 * see dnx_vlan_port_create_verify.
                 */
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, inlif_info.local_inlif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_default
                                    (unit, vlan_port->port, inlif_info.local_inlif));
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * Four configuration modes:
             * 1. Symmetric (InLif + OutLif EEDB)
             * 2. Ingress only (InLif)
             * 3. Egress only (ESEM)
             * 4. Egress only (OutLif)
             */
            if (is_ingress == TRUE)
            {
                if (is_native_ac == FALSE)
                {
                    /*
                     * Case 1/2 above: Symmetric/Ingress only
                     */
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, inlif_info.local_inlif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, inlif_info.local_inlif));
                    }
                }
                else
                {
                    /*
                     * Ingress Native:
                     */
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_native_match_port_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, inlif_info.local_inlif));
                }
            }

            /*
             * Handle ESEM (non-native and native):
             */
            if (is_egress == TRUE && !is_out_lif)
            {
                /*
                 * Add ESEM match-info to SW DB:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state(unit, vlan_port));
                /*
                 * Configure the ESEM entry:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem
                                (unit, vlan_port, egress_ac_table_info.out_lif_profile));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            /*
             * Case 1/2 above: Symmetric/Ingress only
             */
            if (is_ingress == TRUE)
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, inlif_info.local_inlif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_c_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, inlif_info.local_inlif));
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            if (is_ingress == TRUE)
            {
                if (is_native_ac == FALSE)
                {
                    /*
                     * case 1/2: Symmetric/Ingress only
                     */
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, inlif_info.local_inlif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         inlif_info.local_inlif));
                    }
                }
                else
                {
                    /*
                     * Ingress Native:
                     */
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_native_match_port_vlan_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                     inlif_info.local_inlif));
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            if (is_ingress == TRUE)
            {
                /*
                 * case 1/2: Symmetric/Ingress only
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, inlif_info.local_inlif));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            if (is_ingress == TRUE)
            {
                /*
                 * case 1/2: Symmetric/Ingress only
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, inlif_info.local_inlif));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {

            /*
             * Add ESEM match-info to SW DB:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state(unit, vlan_port));
            /*
             * Configure the ESEM entry:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem
                            (unit, vlan_port, egress_ac_table_info.out_lif_profile));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {

            if (is_ingress == TRUE)
            {
                /*
                 * Symmetric/Ingress only:
                 */
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, inlif_info.local_inlif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_untagged
                                    (unit, vlan_port->port, inlif_info.local_inlif));
                }
            }

            break;
        }
        default:
        {
            break;
        }
    }

    if (is_egress == TRUE && is_out_lif)
    {
      /** egress qos*/
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_type_to_model(unit, vlan_port->egress_qos_model.egress_qos, &qos_model));
        nwk_qos_idx = DNX_QOS_EXPLICIT_IDX_L2(vlan_port->pkt_pri, vlan_port->pkt_cfi);
        egress_ac_table_info.qos_model = qos_model;
        egress_ac_table_info.nwk_qos_idx = nwk_qos_idx;

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_eedb
                        (unit, outlif_info.local_outlif, is_native_ac, &egress_ac_table_info));
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif, outlif_info.local_outlif));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, vsi, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and port from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    /*
     * Get port's vlan_domain
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the ESEM access keys from the given SW table.
 * In addition, it sets the ESEM access keys (if the ESEM table handler is not NULL).
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \param [out] vlan_port -pointer to vlan_port structure to update its vsi, match_vlan and port fields.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_egress_virtual_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 port, vsi, match_vlan;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and port from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &match_vlan));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    vlan_port->vsi = (bcm_vlan_t) vsi;
    vlan_port->match_vlan = (bcm_vlan_t) match_vlan;
    vlan_port->port = (bcm_gport_t) port;
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        uint32 vlan_domain;
        /*
         * Get port's vlan_domain
         */
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_C_VID, 0);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_vsi_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 vsi, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, &vlan_domain));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_vsi_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_t * vsi,
    uint32 *match_class_id)
{
    uint32 tmp32;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &tmp32));
    *vsi = (bcm_vlan_t) tmp32;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, match_class_id));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, *match_class_id);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, *vsi);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_C_VID, 0);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_port_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, system_port, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve system_port and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(port);
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the ESEM Namespace-port access keys from the given SW table.
 * In addition, it sets the ESEM access keys (if the ESEM table handler is not NULL).
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \param [out] vlan_port -pointer to vlan_port structure to update its port and port match_class_id fields.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_egress_virtual_namespace_port_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 port, system_port, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve system_port and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));
    vlan_port->port = (bcm_gport_t) port;
    vlan_port->match_class_id = vlan_domain;
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(port);
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_native_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 vsi, local_out_lif;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and local_out_lif from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, &local_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    /*
     * Update the EGRESS ESEM native handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the ESEM Native access keys from the given SW table.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \param [out] vlan_port -pointer to vlan_port structure to update its vsi and port fields.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_egress_virtual_native_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vsi, local_out_lif;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and local_out_lif from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, &local_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    /*
     * Convert local_out_lif back to gport:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL, (int) local_out_lif,
                     &(vlan_port->port)));
    vlan_port->vsi = (bcm_vlan_t) vsi;

    if (entry_handle_id != NULL)
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See Vlan.h for description
 */
shr_error_e
dnx_vlan_port_egress_virtual_ac_match_info_get(
    int unit,
    bcm_gport_t gport,
    uint32 sw_table_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Access SW DB
     */
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create bridge In-LIF:
 *   - Ingress only.
 *   - VSI = VLAN (by setting VSI = 0 and FODO assignment mode = base + vid)
 *   - criteria = none (at ingress port init, this default in-lif is set to each port)
 */
static shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_create(
    int unit,
    int *created_local_lif)
{
    uint32 entry_handle_id;
    lif_mngr_local_inlif_info_t inlif_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_AC_INFO_DB;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP;
    inlif_info.core_id = _SHR_CORE_ALL;
    /*
     * Alloc local in-lif (no global lif allocation)
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    /*
     * Update In-LIF table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, inlif_info.dbal_table_id, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, inlif_info.local_inlif);
    /*
     * Set values:
     * VSI = 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 inlif_info.dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_VLAN);
    /*
     * Set default learning
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, INST_SINGLE,
                                 LIF_LEARN_INFO_OPTIMIZED_PREFIX);
    /*
     * Zero all other fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Return local lif value
     */
    *created_local_lif = inlif_info.local_inlif;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create recycle port in-lif:
 *    - Ingress only
 *    - VSI = result from esem lookup (by setting VSI = 0 and FODO assignment mode = in_lif_id)
 *    - criteria = none (at bcm_port_control_set for recycle port (bcmPortControlOverlayRecycle),
 *                 set the default in-lif for the port.
 */
static shr_error_e
dnx_vlan_port_ingress_initial_recycle_lif_create(
    int unit,
    int *created_local_lif)
{
    uint32 entry_handle_id;
    lif_mngr_local_inlif_info_t inlif_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_AC_INFO_DB;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE;
    inlif_info.core_id = _SHR_CORE_ALL;
    /*
     * Alloc local in-lif (no global lif allocation)
     * we want IN_LIF_AC_MP_LARGE
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    /*
     * Update In-LIF table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, inlif_info.dbal_table_id, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, inlif_info.local_inlif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 inlif_info.dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LOOKUP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Return local lif value
     */
    *created_local_lif = inlif_info.local_inlif;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_set(
    int unit,
    int initial_bridge_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_BRIDGE_VLAN_PORT_IN_LIF, INST_SINGLE,
                                 initial_bridge_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_get(
    int unit,
    int *initial_bridge_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_BRIDGE_VLAN_PORT_IN_LIF, INST_SINGLE,
                               (uint32 *) initial_bridge_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create drop In-LIF:
 *   - no global or local lif allocation
 *   - P2P
 *   - destination: ingress trap, trap Id = "ingress trap with action drop"
 */
static shr_error_e
dnx_vlan_port_ingress_initial_drop_lif_create(
    int unit)
{
    uint32 drop_in_lif;
    uint32 entry_handle_id;
    uint32 trap_dest, snp_strength, fwd_strength, trap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Set Drop In-LIF:
     *   - no global\local lif allocation
     *   - P2P
     *   - destination: ingress trap, trap Id = "ingress trap with action drop"
     */
    drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, drop_in_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE);
    /** Configure ingress trap with action drop */
    bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, (int *) &trap_id);
    fwd_strength = dnx_data_trap.strength.max_strength_get(unit);
    snp_strength = 0;
    /*
     * Must initialize this variable since it is a uint32 but only 16 bits are used.
     * The procedure dbal_entry_value_field32_set() is not aware of that and uses
     * the full 32 bits.
     */
    trap_dest = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_SNP_STRENGTH, &snp_strength, &trap_dest));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_FWD_STRENGTH, &fwd_strength, &trap_dest));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_INGRESS_TRAP_ID, &trap_id, &trap_dest));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_DEST, INST_SINGLE, trap_dest);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create VTT5 dummy In-LIF:
 *   - no global or local lif allocation
 *   - MP
 */
static shr_error_e
dnx_vlan_port_ingress_initial_vtt5_dummy_lif_create(
    int unit)
{
    uint32 vtt5_dummy_in_lif;
    uint32 entry_handle_id;
    lif_mngr_local_inlif_info_t inlif_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_DUMMY;
    inlif_info.core_id = _SHR_CORE_ALL;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    vtt5_dummy_in_lif = inlif_info.local_inlif;
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DUMMY_LIF_LOOKUP, &entry_handle_id));
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_INDEX, INST_SINGLE, vtt5_dummy_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, vtt5_dummy_in_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_DUMMY);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_recycle_port_lif_set(
    int unit,
    int initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_RECYCLE_PORT_VLAN_PORT_IN_LIF,
                                 INST_SINGLE, initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_lif_set(
    int unit,
    int initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_VLAN_PORT_IN_LIF, INST_SINGLE,
                                 initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_lif_get(
    int unit,
    int *initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_VLAN_PORT_IN_LIF, INST_SINGLE,
                               (uint32 *) initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_default_recyle_lif_get(
    int unit,
    uint32 *default_recycle_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_RECYCLE_PORT_VLAN_PORT_IN_LIF,
                               INST_SINGLE, default_recycle_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_default_init(
    int unit)
{
    int initial_vlan_port = 0;
    int initial_recycle_vlan_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create Drop In-LIF:
     *   - no global\local lif allocation
     *   - P2P
     *   - destination: ingress trap, trap Id = "ingress trap with action drop"
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_drop_lif_create(unit));
    /*
     * Create VTT5 dummy In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_vtt5_dummy_lif_create(unit));
    /*
     * Create bridge In-LIF:
     *   - Ingress only.
     *   - VSI = VLAN (by setting VSI = 0 and FODO assignment mode = base + vid)
     *   - criteria = none (at ingress port init, this default in-lif is set to each port)
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_create(unit, &initial_vlan_port));
    /*
     * Save the initial bridge In-LIF gport:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_set(unit, initial_vlan_port));
    /*
     * Choose which LIF is the initial default In-LIF gport:
     * Set it to be drop LIF.
     */
    initial_vlan_port = (int) dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_set(unit, initial_vlan_port));
    /*
     * Create recycle port in-lif:
     *    - Ingress only
     *    - VSI = result from esem lookup (by setting VSI = 0 and FODO assignment mode = in_lif_id)
     *    - criteria = none (at bcm_port_control_set for recycle port (bcmPortControlOverlayRecycle),
     *                 set the default in-lif for the port.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_recycle_lif_create(unit, &initial_recycle_vlan_port));
    /*
     * Save the initial bridge In-LIF gport:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_recycle_port_lif_set(unit, initial_recycle_vlan_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_default_esem_init(
    int unit)
{
    int esem_handle, out_lif_profile, egress_last_layer;
    uint32 entry_handle_id;
    bcm_switch_network_group_t egress_network_group_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Reserve an AC default ESEM entry 
     */
    esem_handle = dnx_data_esem.default_result_profile.default_ac_get(unit);

    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single
                    (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, NULL, &esem_handle));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, 0);
    out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    egress_last_layer = TRUE;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reserve a Native-AC default ESEM entry 
     */
    esem_handle = dnx_data_esem.default_result_profile.default_native_get(unit);

    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single
                    (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, NULL, &esem_handle));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);

    egress_network_group_id = 1;
    SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                    (unit, egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT, &out_lif_profile,
                     DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, 0));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    egress_last_layer = FALSE;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get the forwarding information from Gport (used for retrieving protection parameters).
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vlan_port  -  see detail explanation at bcm_vlan_port_create()
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_vlan_port_gport_to_forward_information_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t destination;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, vlan_port->vlan_port_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));
    if ((forward_info.fwd_info_result_type !=
         DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        && (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_type (%d) is not supported for get api \r\n",
                     forward_info.fwd_info_result_type);
    }

    /*
     * returning learning information depends on whether the destination is FEC or MC or physical port
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination, &destination));
    if (_SHR_GPORT_IS_FORWARD_PORT(destination))
    {
        vlan_port->failover_port_id = destination;
    }
    else if (_SHR_GPORT_IS_MCAST(destination))
    {
        vlan_port->failover_mc_group = _SHR_GPORT_MCAST_GET(destination);
    }
    /*
     * Note: vlan_port->port is retrieved by SW DB (IN_LIF_MATCH_INFO_SW)
     * See dnx_vlan_port_create_ingress_lif_match_info_get.
     * Thus, no need to update it here.
     else
     {
     vlan_port->port = destination;
     }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns the default (local) lif value of the given port
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port(
    int unit,
    bcm_gport_t port,
    int *local_in_lif)
{
    uint32 port_default_lif;
    uint32 initial_drop_in_lif;
    int initial_bridge_in_lif;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Retrieve the port's default In-LIF from the HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_control_port_default_lif_get(unit, port, &port_default_lif));
    /*
     * In case the port.default_lif is one of the initial SDK default LIFs (drop-lif or simple-bridge-lif) -return error "not found"!
     */
    initial_drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, &initial_bridge_in_lif));
    if ((port_default_lif == initial_drop_in_lif) || (port_default_lif == initial_bridge_in_lif))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! port = %d, the port_default_lif = %d is one of SDK initial LIF defaults (drop-lif=%d, bridge-lif=%d)\n",
                     port, port_default_lif, initial_drop_in_lif, initial_bridge_in_lif);
    }

    *local_in_lif = (int) port_default_lif;
exit:
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X c_vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_c_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_c_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_c_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /*
  * This function returns the local-lif value of the port X vlan X vlan ISEM entry
  * \see
  * bcm_dnx_vlan_port_find
  */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /*
  * This function returns the local-lif value of the port X vlan X vlan X pcp X ethertype TCAM entry
  * \see
  * bcm_dnx_vlan_port_find
  */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 tmp32bit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the native local-lif value of the port X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_native_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 tmp32bit;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the native local-lif value of the port X vlan X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_native_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 tmp32bit;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the gport value of the Ingress Virtual Native LIF for the given local in-lif
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_native_virtual_match_sw_state(
    int unit,
    int local_in_lif,
    bcm_gport_t * vlan_port_id)
{
    uint32 entry_handle_id;
    uint32 gport;
    int is_end;
    uint32 resultType, local_inlif;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /*
     * Set iterator rules:
     * All the fields (resultType, local_in_lif) need to equal the given values
     */
    resultType = DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_VIRTUAL_NATIVE_AC;
    local_inlif = local_in_lif;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &local_inlif, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (is_end == TRUE)
    {
        /*
         * No entry found, return "not found"
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! No entry found in INGRESS_GPORT_TO_VLAN_PORT_INFO_SW matching this input! resultType = %d, local_in_lif = %d\n",
                     DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_VIRTUAL_NATIVE_AC, local_in_lif);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GPORT, &gport));
    *vlan_port_id = gport;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X untagged ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_untagged(
    int unit,
    bcm_gport_t port,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tmp32bit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, gport_info.internal_port_pp_info.core_id[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the Native/non-Native in-LIF info.
 * For non-Native in-LIF:
 * - flags
 * - vsi
 * - port
 * - match_vlan
 * - match_inner_vlan
 * - ingress nw group id
 * For Native in-LIF:
 * - flags
 * - vsi
 * \see
 *  bcm_dnx_vlan_port_create
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 in_lif_profile;
    uint32 vsi;
    bcm_failover_t failover_id, failover_id_decoded = 0;
    bcm_gport_t failover_port_id;
    uint32 dbal_struct_data;
    in_lif_profile_info_t in_lif_profile_info;
    int is_native, is_native_non_virtual;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Parameters from HW table
     */
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Retrieve Split Horizon ingress information:
     *  - Non-Native
     *  - Native non-virtual
     */
    is_native =
        (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ? TRUE : FALSE;

    is_native_non_virtual = is_native
        && (!BCM_DNX_IS_RESULT_TYPE_NATIVE_AC_VIRTUAL(gport_hw_resources->inlif_dbal_result_type));

    if ((!is_native) || is_native_non_virtual)
    {
        /*
         * Get value:
         * - ingress_network_group_id - via in_lif_profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
        vlan_port->ingress_network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;
    }

    
    if (((is_native == TRUE) &&
         (gport_hw_resources->inlif_dbal_result_type !=
          DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_VLAN_EDIT_ONLY))
        || ((is_native == FALSE)
            && (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources->inlif_dbal_result_type) == FALSE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &vsi));
        vlan_port->vsi = (bcm_vlan_t) vsi;
    }

    /*
     * Retrieve flush group information:
     *  - Non-Native - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE
     *  - Native  - DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_MP
     */
    if (((is_native == TRUE) &&
         (gport_hw_resources->inlif_dbal_result_type ==
          DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_MP)) ||
        ((is_native == FALSE) &&
         (gport_hw_resources->inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, INST_SINGLE,
                         &dbal_struct_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                         &(vlan_port->group), &dbal_struct_data));
    }

    /*
     * Determine Cross-Connect flags (P2P) according to table and result type
     */
    if (((is_native == FALSE)
         && (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources->inlif_dbal_result_type) == TRUE)) ||
        ((is_native == TRUE)
         && (BCM_DNX_IS_RESULT_TYPE_NATIVE_AC_CROSS_CONNECT(gport_hw_resources->inlif_dbal_result_type) == TRUE)))
    {
        vlan_port->flags |= BCM_VLAN_PORT_CROSS_CONNECT;
    }

    
    if (is_native == TRUE ||
        (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources->inlif_dbal_result_type) == FALSE))
    {
        uint32 fodo_assignment_mode;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE, &fodo_assignment_mode));
        if (fodo_assignment_mode == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_VLAN)
        {
            vlan_port->flags |= BCM_VLAN_PORT_VSI_BASE_VID;
        }
    }

    /*
     * Get the parameters from match info SW table:
     * - port
     * - criteria
     * - match_vlan
     * - match_inner_vlan
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get(unit, gport_hw_resources, vlan_port));

    /*
     * Read the SW state table to find out which (if any) flag was set
     */
    if (!BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_lif_flags_retreive(unit, vlan_port));
    }

    /*
     * Update Native flag:
     */
    if (is_native == TRUE)
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    /*
     * Get Protection parameters for supported formats
     */
    if (((is_native == TRUE) &&
         (gport_hw_resources->inlif_dbal_result_type ==
          DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_MP))
        || ((is_native == FALSE) &&
            ((gport_hw_resources->inlif_dbal_result_type ==
              DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE)
             || (gport_hw_resources->inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE))))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE, (uint32 *) (&failover_id)));
        if (failover_id != dnx_data_failover.path_select.ing_no_protection_get(unit))
        {
            /** Get Failovr ID from IN-LIF protection pointer format */
            DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(failover_id_decoded, failover_id);
            DNX_FAILOVER_SET(vlan_port->ingress_failover_id, failover_id_decoded, DNX_FAILOVER_TYPE_INGRESS);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                         (uint32 *) (&failover_port_id)));
        vlan_port->ingress_failover_port_id = (failover_port_id) ? 0 : 1;
    }

    /*
     * get forwarding info for protection
     * Note: protection info is not set for Native or for VLAN-Translation In-LIFs. 
     */
    if (((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) == FALSE) ||
         (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VSI_BASE_VID) == TRUE)) &&
        (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_gport_to_forward_information_get(unit, vlan_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the gport value of the ESEM LIF that is pointed by the
 * given input:
 *  - criteria - the matching criteria
 *  - flags - indicates native/non-native
 *  - vsi X local_out_lif (via port): for ESEM native.
 *  - vsi X vlan X vlan_domain (port): for ESEM non-native
 *  - vsi X vlan X vlan_domain (network_domain): for ESEM Namespace-vsi
 *  - system_port X vlan_domain (namespace): for ESEM Namespace-port
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_match_esem_sw_state(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 gport;
    int is_end;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                /*
                 * Handle ESEM AC Native:
                 */
                uint32 resultType, fodo_vsi, local_out_lif;
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC;
                /** Get local Out-LIF using DNX Algo Gport Management */
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                fodo_vsi = vlan_port->vsi;
                local_out_lif = gport_hw_resources.local_out_lif;
                /*
                 * Set iterator rules:
                 * All the fields (resultType, VSI, local_out_lif) need to equal the given values
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &fodo_vsi, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                 DBAL_CONDITION_EQUAL_TO, &local_out_lif, NULL));
            }
            else
            {
                /*
                 * Handle ESEM AC non-native
                 */
                uint32 resultType, fodo_vsi, outer_vlan_id, port_id;
                resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC;
                fodo_vsi = vlan_port->vsi;
                outer_vlan_id = vlan_port->match_vlan;
                port_id = vlan_port->port;
                /*
                 * Set iterator rules:
                 * All the fields (resultType, VSI, VLAN, port) need to equal the given values
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &fodo_vsi, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                 DBAL_CONDITION_EQUAL_TO, &outer_vlan_id, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &port_id, NULL));
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            uint32 resultType, fodo_vsi, network_domain;
            resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI;
            fodo_vsi = vlan_port->vsi;
            network_domain = vlan_port->match_class_id;
            /*
             * Set iterator rules:
             * All the fields (resultType, VSI, network_domain) need to equal the given values
             */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &fodo_vsi, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &network_domain, NULL));
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            uint32 resultType, port, esem_namespace;
            resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT;
            port = vlan_port->port;
            esem_namespace = vlan_port->match_class_id;
            /*
             * Set iterator rules:
             * All the fields (resultType, port, namespace) need to equal the given values
             */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &port, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &esem_namespace, NULL));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (is_end == TRUE)
    {
        /*
         * No entry found, return "not found"
         */

        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! No ESEM entry found matching this input! criteria = %d, vsi = %d, match_vlan = %d, match_class_id = %d, port = %d, , gport = 0x%08X, flags = 0x%08X\n",
                     vlan_port->criteria, vlan_port->vsi, vlan_port->match_vlan, vlan_port->match_class_id,
                     vlan_port->port, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GPORT, &gport));
    vlan_port->vlan_port_id = gport;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the following info from Port Default AC Profile table:
 *  - egress_network_group_id (via out_lif_profile)
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_port_default_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 ac_profile;
    uint32 out_lif_profile;
    out_lif_profile_info_t out_lif_profile_info;
    uint32 egress_last_layer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ac_profile from port using encoding
     */
    ac_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, ac_profile);
    /*
     * Get values:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));
    /*
     *  Retrieve the Split Horizon egress information
     */
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data
                    (unit, out_lif_profile, &out_lif_profile_info, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE));
    vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;
    /*
     * Native or non-native:
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &egress_last_layer));
    if (egress_last_layer == FALSE)
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_find_egress_qos_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 table_handle_id)
{
    dbal_enum_value_field_encap_qos_model_e qos_model;
    uint32 nwk_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, table_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                        &qos_model));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, table_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                                        &nwk_qos_idx));

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_to_model_type(unit, qos_model, &(vlan_port->egress_qos_model.egress_qos)));
    vlan_port->pkt_pri = DNX_QOS_EXPLICIT_IDX_PCP_GET(nwk_qos_idx);
    vlan_port->pkt_cfi = DNX_QOS_EXPLICIT_IDX_CFI_GET(nwk_qos_idx);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF info.
 * For ESEM non-Native LIF:
 *  -vsi
 *  -match_vlan
 *  -port
 *  - egress_network_group_id (via out_lif_profile)
 * For ESEM Native LIF:
 *  -vsi
 *  -port
 * For ESEM Namespace-vsi LIF:
 *  - vsi
 *  - match_class_id
 *  - egress_network_group_id (via out_lif_profile)
 * For ESEM Namespace-port LIF:
 *  - port
 *   -match_class_id
 *  - egress_network_group_id (via out_lif_profile)
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_esem_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 sw_table_handle_id;
    uint32 entry_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));
    
    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            uint32 esem_handle_id;
            /*
             * Handle ESEM AC Native:
             */
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_NATIVE;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, &esem_handle_id));
            /*
             * Get the ESEM keys (VSI, out_lif):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_get
                            (unit, sw_table_handle_id, &esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {
            uint32 esem_handle_id;
            uint32 out_lif_profile;
            out_lif_profile_info_t out_lif_profile_info;
            /*
             * Handle ESEM AC non-native:
             */
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (VSI, port):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_key_get(unit, sw_table_handle_id, &esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));

            /*
             *  Retrieve the Split Horizon egress information
             */
            SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data
                            (unit, out_lif_profile, &out_lif_profile_info, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB));
            vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            uint32 esem_handle_id;
            uint32 out_lif_profile;
            out_lif_profile_info_t out_lif_profile_info;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (VSI, match_class_id):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_vsi_key_get
                            (unit, sw_table_handle_id, &esem_handle_id, &vlan_port->vsi, &vlan_port->match_class_id));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            /*
             *  Retrieve the Split Horizon egress information
             */
            SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data
                            (unit, out_lif_profile, &out_lif_profile_info, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB));
            vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            uint32 esem_handle_id;
            uint32 out_lif_profile;
            out_lif_profile_info_t out_lif_profile_info;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (port, match_class_id):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_port_key_get
                            (unit, sw_table_handle_id, &esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));
            /*
             *  Retrieve the Split Horizon egress information
             */
            SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data
                            (unit, out_lif_profile, &out_lif_profile_info,
                             DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB));
            vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;

            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, vlan_port->vlan_port_id, entry_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the following info from EEDB table:
 *  - egress_network_group_id (via out_lif_profile)
 *  - tunnel_id (relevant in case of native AC)
 * If tunnel_id is not zero, flag BCM_VLAN_PORT_NATIVE is set.
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_eedb_info_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 out_lif_profile;
    uint32 next_local_out_lif = 0;
    out_lif_profile_info_t out_lif_profile_info;
    uint32 failover_id, failover_port_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));

    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources->local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->outlif_dbal_result_type);

    /*
     * Get values:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Determine if it's a Native Out-LIF and retrieve relevant information
     */
    if (gport_hw_resources->outlif_phase == LIF_MNGR_OUTLIF_PHASE_NATIVE_AC)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, &next_local_out_lif));

        /*
         * This result-type is for EEDB Native, see dnx_vlan_port_create_egress_non_virtual_lif_table_result_type_calc
         */
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    if (next_local_out_lif != 0)
    {
        /*
         * next_local_out_lif is the local out lif, get the related gport.
         * In case the tunnel LIF was not found (either it was deleted or not yet allocated,
         * don't return an error, just set the tunnel_id to INVALID gport.
         */
        vlan_port->tunnel_id = BCM_GPORT_INVALID;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                         _SHR_CORE_ALL, (int) next_local_out_lif, &(vlan_port->tunnel_id)));
    }
    else
    {
        vlan_port->tunnel_id = 0;
    }

    /**pcp/dei and qos model*/
    SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, entry_handle_id));

    /**
     * outlif profile
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                                        &out_lif_profile));

    /*
     * Retrieve the Split Horizon egress information
     */
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info, DBAL_TABLE_EEDB_OUT_AC));
    vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;

    /*
     * Retrieve Protection parameters
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE, &failover_id));

    if (failover_id != dnx_data_failover.path_select.egr_no_protection_get(unit))
    {
        DNX_FAILOVER_SET(vlan_port->egress_failover_id, failover_id, DNX_FAILOVER_TYPE_ENCAP);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, &failover_port_id));
        vlan_port->egress_failover_port_id = (failover_port_id) ? 0 : 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API retrieves VLAN Port gport according to a given VLAN Port criteria and match-info.
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in,out] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 *   [out] vlan_port.vlan_port_id - contains the found gport
 *   [in] vlan_port.criteria - logical layer 2 port match criteria.
 *   [in] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [in] vlan_port.vsi - virtual switching instance.
 *   [in] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [in] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [in] vlan_port.port - Physical or logical gport.
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 * flags field is used to signal which LIF the user is looking for; ingress-only, egress-only, symmetric,
 * native and virtual.
 * Thus, the following flags are relevant:
 *  BCM_VLAN_PORT_CREATE_EGRESS_ONLY
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY
 *  BCM_VLAN_PORT_NATIVE
 *  BCM_VLAN_PORT_VLAN_TRANSLATION
 * \see
 *   * None
 */
shr_error_e
dnx_vlan_port_find_lookup(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress, is_native_ac, is_virtual_ac;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Find the VLAN-PORT using lookup:
     *  - criteria
     *  - match_vlan
     *  - match_inner_vlan
     *  - flags
     *  - port
     *  - vsi
     */
    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    /*
     * For Ingress only LIFs and Symmetric LIFs, find the gport by finding the local in-lif using the criteria and match-info:
     */
    if (is_ingress == TRUE)
    {
        int local_in_lif = -1;

        switch (vlan_port->criteria)
        {
            case BCM_VLAN_PORT_MATCH_PORT:
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port(unit, vlan_port->port, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            {
                if (is_native_ac == FALSE)
                {
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_match_port_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_c_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            {

                if (is_native_ac == FALSE)
                {
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         &local_in_lif));
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_match_port_vlan_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                     &local_in_lif));
                }

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, &local_in_lif));
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, &local_in_lif));
                break;
            }

            case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
            {

                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_untagged
                                    (unit, vlan_port->port, &local_in_lif));
                }

                break;
            }

            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. criteria = %d is not supported for VLAN-Port find using lookup!!!\n",
                             vlan_port->criteria);
            }
        }

        /*
         * Retrieves the gport from the given local in-lif:
         */
        if (is_virtual_ac == FALSE)
        {
            /*
             * The local in-lif is not "virtual", retrieve it's gport using the GPM
             */
            if (is_native_ac == FALSE)
            {
                /*
                 * For AC LIF - it is shared between cores thus use SBC flag:
                 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS,
                                                            _SHR_CORE_ALL, local_in_lif, &(vlan_port->vlan_port_id)));
            }
            else
            {
                /*
                 * For Native LIF - it is dedicated per core thus use DPC flag:
                 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS,
                                                            _SHR_CORE_ALL, local_in_lif, &(vlan_port->vlan_port_id)));
            }
        }
        else
        {
            /*
             * The local in-lif is "virtual", look for it in the SW DB
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state(unit, (int) local_in_lif,
                                                                                     &(vlan_port->vlan_port_id)));
        }

    }
    else if (is_egress == TRUE)
    {
        /*
         * For Egress only, only ESEM is supported (Default-ESEM and EEDB are not supported for lookup):
         *   - check EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW to find such a lookup
         * See dnx_vlan_port_find_verify
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_match_esem_sw_state(unit, vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API retrieves VLAN Port information according to a given gport or
 * according to a given VLAN Port criteria and match-info (lookup).
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in,out] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 * For retrieves according to gport:
 *   [in] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [out] vlan_port.criteria - logical layer 2 port match criteria.
 *   [out] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [out] vlan_port.vsi - virtual switching instance.
 *   [out] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [out] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [out] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 * For retrieves according to criteria and match-info (lookup):
 *   [out] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [in] vlan_port.criteria - logical layer 2 port match criteria.
 *   [in] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [in] vlan_port.vsi - virtual switching instance.
 *   [in] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [in] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [in] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_port_find(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_find_verify(unit, vlan_port));
    /*
     * Find the VLAN-PORT using lookup:
     *   - criteria
     *   - match_vlan
     *   - match_inner_vlan
     *   - flags
     *   - port
     */
    if (vlan_port->vlan_port_id == 0)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_lookup(unit, vlan_port));
    }

    /*
     * Find the VLAN-PORT using its gport:
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
    {
        /*
         * Handle Ingress Virtual Native Vlan port
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * get Local In-LIF:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                         &gport_hw_resources));
        vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_VLAN_TRANSLATION;
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress(unit, &gport_hw_resources, vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port->vlan_port_id))
    {
        /*
         * Handle Egress Virtual Vlan ports:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_esem_info_get(unit, vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        /*
         * Handle Egress ESEM Default:
         */
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_DEFAULT;
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_port_default_info_get(unit, vlan_port));
    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        /*
         * Get local LIFs using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
        vlan_port->flags = 0;
        if (gport_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {

            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY))
            {
                vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
                vlan_port->encap_id = 0;
            }

            SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress(unit, &gport_hw_resources, vlan_port));
        }

        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * EEDB:
             */
            vlan_port->encap_id = gport_hw_resources.global_out_lif;
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
            {
                vlan_port->flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            }

            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_eedb_info_get(unit, &gport_hw_resources, vlan_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x VLAN x VLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x VLAN x VLAN x PCP x ETHERTYPE match in ingress configuration.
 *          TCAM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));
    /** Create TCAM access id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IN_AC_TCAM_DB, entry_access_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x untagged match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_untagged_clear(
    int unit,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Ingress - Clear AC lookup entry
     */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See header file bcm_int/dnx/vlan/vlan.h for description
 */
shr_error_e
dnx_vlan_port_destroy_ingress_non_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            /*
             * No matching, nothing to do
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                int initial_default_in_lif;
                /*
                 * Get initial default_in_lif (local in-lif)
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_get(unit, &initial_default_in_lif));
                /*
                 * Set port back to default in-LIF
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_default
                                (unit, vlan_port->port, initial_default_in_lif));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_cvlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * Clear the TCAM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                             vlan_port->match_ethertype, vlan_port->match_pcp));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * Clear the TCAM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                             vlan_port->match_ethertype, vlan_port->match_pcp));
            break;
        }

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_untagged_clear(unit, vlan_port->port));
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error, gport = 0x%08X contains unknown/unsupported criteria (=%d)!\n",
                         vlan_port->vlan_port_id, vlan_port->criteria);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN match in ingress native configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_native_match_port_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Ingress - Clear Native AC lookup entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN x VLAN match in ingress native configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_native_match_port_vlan_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used 
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle. 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Ingress - Clear Native AC lookup entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See header file bcm_int/dnx/vlan/vlan.h for description
 */
shr_error_e
dnx_vlan_port_destroy_ingress_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            /*
             * Not supported, should not get here
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error, gport = 0x%08X contains unsupported criteria (=%d) for Native In-LIF!\n",
                         vlan_port->vlan_port_id, vlan_port->criteria);
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_port_vlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_port_vlan_vlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error, gport = 0x%08X contains unknown/unsupported criteria (=%d) for Native In-LIF!\n",
                         vlan_port->vlan_port_id, vlan_port->criteria);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears IN-LIF Info table
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_lif_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears OUT-LIF Info table (EEDB)
 * \see
 *  bcm_dnx_vlan_port_destroy_egress
 */
static shr_error_e
dnx_vlan_port_destroy_egress_lif_info_clear(
    int unit,
    uint32 result_type,
    int local_out_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress AC at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_non_native_info_clear(
    int unit,
    bcm_vlan_t vsi,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress Native AC at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_native_info_clear(
    int unit,
    uint32 sw_table_handle_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_set(unit, sw_table_handle_id, entry_handle_id));
    /*
     * Clear entry in Native ESEM
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress Namespace-port at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_port_info_clear(
    int unit,
    uint32 system_port,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears entry at ESEM Default table
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_default_info_clear(
    int unit,
    int esem_default_handle)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_default_handle);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function frees an allocated LIF
 * Its frees the in-LIF and if the LIF is symmetric, also the out-LIF is freed.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_lif_free(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Dealloc IN-LIF (and OUT-LIF, if it is a symmetric LIF)
     */
    if (gport_hw_resources->local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_mngr_local_inlif_info_t inlif_info;
        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = gport_hw_resources->inlif_dbal_table_id;
        inlif_info.dbal_result_type = gport_hw_resources->inlif_dbal_result_type;
        inlif_info.core_id = _SHR_CORE_ALL;
        inlif_info.local_inlif = gport_hw_resources->local_in_lif;
        SHR_IF_ERR_EXIT(dnx_lif_lib_free
                        (unit, gport_hw_resources->global_in_lif, &inlif_info, gport_hw_resources->local_out_lif));
    }
    else
    {
        /*
         * Dealloc OUT-LIF only
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_free
                        (unit, gport_hw_resources->global_out_lif, NULL, gport_hw_resources->local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears Ingress outer ac SW state entry.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_outer_ac_sw_state_clear(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_OUTER_AC);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated in-LIF.
 * It frees all relevant allocated HW and SW resources, except for the In-LIF resource manager allocation.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_non_virtual(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 in_lif_profile;
    int is_native_ac;
    int is_p2p_ac;
    SHR_FUNC_INIT_VARS(unit);
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? TRUE : FALSE;
    is_p2p_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CROSS_CONNECT) ? TRUE : FALSE;
    /*
     * 1. Clear match table
     */
    if (is_native_ac == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_native_match_clear(unit, vlan_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_clear(unit, vlan_port));
    }

    /*
     * 2. Dealloc in-lif profiles:
     *      - in_lif_profile
     *      - qos_profile:
     *      qos_profile should be freed by the user.
     *      See bcm_qos_map_create and bcm_qos_map_destroy.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_get(unit, gport_hw_resources, &in_lif_profile));
    SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_dealloc(unit, in_lif_profile));
    /*
     * 3. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_match_info_clear
                    (unit, is_native_ac, gport_hw_resources->local_in_lif));
    /*
     * Clear forwarding info SW state DB
     * See vlan_port_gport_to_forward_information_set (which calls algo_gpm_gport_l2_forward_info_add)
     *
     * Forward Info and learning are for Service and Optimization application:
     */
    if ((gport_hw_resources->local_out_lif != DNX_ALGO_GPM_LIF_INVALID) ||
        (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VSI_BASE_VID)))
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, vlan_port->vlan_port_id));
    }

    /*
     * Clear SW state entry
     */
    if ((is_native_ac == FALSE) || (is_native_ac == TRUE && is_p2p_ac == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_outer_ac_sw_state_clear(unit, vlan_port->vlan_port_id));
    }

    /*
     * 4. Clear In-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_info_clear(unit, gport_hw_resources));
    /*
     * 5. Dealloc the IN-LIF
     * Note: because of symmetric LIF, need to free the IN-LIF and the OUT-LIF together thus
     * it is done after the Egress destroy function (see dnx_vlan_port_destroy_lif_free).
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears Ingress Virtual Native SW state  entry.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_virtual_native_ac_sw_state_clear(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_INGRESS_GPORT_TO_VLAN_PORT_INFO_SW_VIRTUAL_NATIVE_AC);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated Ingress Virtual Native LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_virtual_native(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int sw_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the Local In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vlan_port->vlan_port_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                     &gport_hw_resources));
    /*
     * 1. Clear match table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_clear(unit, vlan_port));
    /*
     * 2. Dealloc in-lif profiles:
     *      Ingress Virtual Native does not have in-lif profile, only qos_profile.
     *      qos_profile should be freed by the user.
     *      See bcm_qos_map_create and bcm_qos_map_destroy.
     *      Thus, nothing to do.
     */
    /*
     * 3. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_match_info_clear(unit, TRUE, gport_hw_resources.local_in_lif));
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_virtual_native_ac_sw_state_clear(unit, vlan_port->vlan_port_id));
    /*
     * 4. Clear In-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_info_clear(unit, &gport_hw_resources));
    /*
     * 5. Dealloc sw handle
     */
    sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.free_single(unit, sw_handle));
    /*
     * 6. Free the IN-LIF
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_lif_free(unit, &gport_hw_resources));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Default LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_default(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int esem_default_handle;
    SHR_FUNC_INIT_VARS(unit);
    esem_default_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, 0, 0, 0, 0, esem_default_handle, 0));
    /*
     * 2. Clear SW state DB
     * Note: no SW state DB, nothing to clear!
     */
    /*
     * 3. Clear the ESEM Default table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_default_info_clear(unit, esem_default_handle));
    /*
     * 4. Dealloc Egress Default handle
     */
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.free_single(unit, esem_default_handle));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_non_native(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get port's vlan_domain
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0, 0, vlan_port->vsi, vlan_domain, 0, 0));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Clear the ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native_info_clear(unit, vlan_port->vsi, vlan_domain));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM  Native LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_native(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * 1. Clear the Native ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_native_info_clear(unit, sw_table_handle_id));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Namespace-vsi LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_vsi(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    vlan_domain = vlan_port->match_class_id;
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0, 0, vlan_port->vsi, vlan_domain, 0, 0));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Clear the ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native_info_clear(unit, vlan_port->vsi, vlan_domain));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Namespace-port LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_port(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    uint32 system_port;
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    vlan_domain = vlan_port->match_class_id;
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(vlan_port->port);
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, 0, 0, vlan_port->vsi,
                     vlan_domain, 0, system_port));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Clear the ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_port_info_clear(unit, system_port, vlan_domain));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function destroys ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF.
 * This function deletes an allocated ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 sw_table_handle_id;
    uint32 entry_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));
    
    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            /*
             * Handle ESEM AC Native:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_native(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {
            /*
             * Handle ESEM AC non-native:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_vsi(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_port(unit, sw_table_handle_id, vlan_port));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, vlan_port->vlan_port_id, entry_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated (EEDB) out-LIF.
 * It frees all relevant allocated HW and SW resources, except for the Out-LIF resource manager allocation.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_eedb(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1. Remove Global to local mapping (from GLEM).
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources->global_out_lif));
    /*
     * 2. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_EEDB_OUT_AC, gport_hw_resources->local_out_lif,
                     gport_hw_resources->outlif_dbal_result_type, 0, 0, 0, 0));
    /*
     * 3. Clear SW state DB
     * Note: no SW state DB, nothing to clear!
     */
    /*
     * Clear forwarding info SW state DB
     * Forward Info is only for symmetric case.
     * Note: clearing forwarding info SW state DB is done in the Ingress function (see dnx_vlan_port_destroy_ingress)
     */
    /*
     * 4. Clear Out-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_lif_info_clear
                    (unit, gport_hw_resources->outlif_dbal_result_type, gport_hw_resources->local_out_lif));
    /*
     * 5. Dealloc the OUT-LIF
     * Note: done together with the IN-LIF free becasue of symmetric case (see dnx_vlan_port_destroy_lif_free).
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API destroys a VLAN PORT.
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport - VLAN PORT ID (Gport encoded with at least a type and subtype).
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark
 * Only LIF resources that were allocated/created during bcm_vlan_port_create are freed/destroyed.
 * LIF resources that are allocated/created later on (like bcm_port_match_add, etc) are not treated
 * by this API and should be handled by user.
 * \see
 *   None
 */
shr_error_e
bcm_dnx_vlan_port_destroy(
    int unit,
    bcm_gport_t gport)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_destroy_verify(unit, gport));
    bcm_vlan_port_t_init(&vlan_port);
    /*
     * Retrieve the VLAN-Port information:
     */
    vlan_port.vlan_port_id = gport;
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
    {
        /*
         * Ingress Virtual Native Vlan port
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_virtual_native(unit, &vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        /*
         * Handle Egress Virtual Vlan ports
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem(unit, &vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Egress Default AC:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_default(unit, &vlan_port));
    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 gpm_flags;
        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        /*
         * Get local LIFs using DNX Algo Gport Managment:
         * No need for strict check because it was already done in _get.
         */
        gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gpm_flags, &gport_hw_resources));
        if (gport_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_virtual(unit, &gport_hw_resources, &vlan_port));
        }

        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * EEDB:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_eedb(unit, &gport_hw_resources));
        }

        /*
         * Free the IN-LIF and OUT-LIF
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_lif_free(unit, &gport_hw_resources));
    }

exit:
    SHR_FUNC_EXIT;
}
