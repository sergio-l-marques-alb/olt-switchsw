/** \file bfd.c
 * $Id$
 *
 * BFD procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <bcm/bfd.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <bcm_int/dnx_dispatch.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <include/shared/utilex/utilex_framework.h>
#include <soc/dnx/swstate/auto_generated/access/algo_bfd_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** 
 *  Bits that prevent overlapping in endpoint IDs
 *  the endpoint ID will either be the local discriminator (for
 *  endpoints whose packets are classified by discriminator) or
 *  LIF ID (for the rest.)  Also, MEP IDs 0 - 12k are reserved
 *  for accelerated endpoints.  These bits will be added to
 *  prevent endpoint IDs overlapping between the different
 *  groups.
 */

/** 
 *  This bit makes sure non-accelerated endpoints don't use IDs
 *  reserved for accelerated endpoints
 */
#define BFD_MEP_ID_NON_ACC_BIT 22

/** 
 *  This bit makes sure that if one endpoint has x as its LIF
 *  and another has x as its discriminator, the endpoints
 *  will not have the same MEP ID
 */
#define BFD_MEP_ID_DISCR_CLASS 23

/** Composition of MEP ID returned value   */
#define BFD_MEP_ACTUAL_ID_NOF_BITS 12
#define BFD_TRAP_CODE_NOF_BITS 4
#define BFD_FWD_STRENGTH_NOF_BITS 3
#define BFD_SNOOP_STRENGTH_NOF_BITS 3

/** Offsets calculated from above composition */
#define BFD_MEP_ACTUAL_ID_OFFSET 0
#define BFD_TRAP_CODE_OFFSET (BFD_MEP_ACTUAL_ID_OFFSET + BFD_MEP_ACTUAL_ID_NOF_BITS)
#define BFD_FWD_STRENGTH_OFFSET (BFD_TRAP_CODE_OFFSET + BFD_TRAP_CODE_NOF_BITS)
#define BFD_SNOOP_STRENGTH_OFFSET (BFD_FWD_STRENGTH_OFFSET + BFD_FWD_STRENGTH_NOF_BITS)

/** 
 *  flags that may be used when creating a BFD endpoint
 */
#define BFD_SUPPORTED_FLAGS (BCM_BFD_ENDPOINT_UPDATE | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_MICRO_BFD | \
                           BCM_BFD_ENDPOINT_MULTIHOP | BCM_BFD_ENDPOINT_HW_ACCELERATION_SET )

/** 
 *  Flags that may be used only when defining an accelerated
 *  BFD endpoint
 */
#define BFD_ACC_ONLY_FLAGS (BCM_BFD_ENDPOINT_UPDATE | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_PWE_ACH | BCM_BFD_ENDPOINT_IN_HW | \
                            BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE | BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE | \
                            BCM_BFD_ENDPOINT_REMOTE_WITH_ID | BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | \
                            BCM_BFD_ECHO | \
                            BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME)

/** Flags that are supported by Jericho 2 */
#define BFD_ACC_FLAGS (BFD_SUPPORTED_FLAGS | BFD_ACC_ONLY_FLAGS)

/** 
 *  Macro - find if a has any bits with a value of "1" that b
 *  has with a value of "0"
 */
#define CHECK_FLAGS_INVERTED_MASK(a, b) ((a & ~b) != 0)

/**
 * Convert IPv4 (in uint32 format) to IPv6 (in uint8[16] 
 * format.) 
 * result should be "0::FFFF::IPv4-DIP"
 */
#define BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(ipv4_ip, ipv6_ip) \
    do {\
          uint32 ipv6_arr[4];\
          ipv6_arr[3] = 0x0000;\
          ipv6_arr[2] = 0x0000;\
          ipv6_arr[1] = 0xffff;\
          ipv6_arr[0] = ipv4_ip;\
          sal_memset(ipv6_ip, 0, sizeof(ipv6_ip));\
          utilex_pp_ipv6_address_long_to_struct(ipv6_arr, ipv6_ip);\
    } while (0)

/**
 * Reverse the above conversion: assume that the IPv6 address 
 * was originally an IPv4 address converted to IPv6, and extract 
 * the original IPv4 address 
 */
#define BFD_EXTRACT_IP4V_ADDRS_FROM_DEFAULT_IPV6_EXTENSION_ADDRESS(ipv6_ip, ipv4_ip)\
    do {\
          uint32 ipv6_arr[4];\
          utilex_pp_ipv6_address_struct_to_long(ipv6_ip, ipv6_arr);\
          ipv4_ip = ipv6_arr[0];\
    } while (0)

/** Macros that extract SW state endpoint ID flags */

/** "Classified by discriminator" flag */
#define ENDPOINT_CLASSIFIED_BY_DISCR(endpoint) UTILEX_GET_BIT(endpoint, BFD_MEP_ID_DISCR_CLASS)

/** "Accelerated" flag (the flag is actually "non-accelerated") */
#define ENDPOINT_IS_ACC(endpoint) !UTILEX_GET_BIT(endpoint, BFD_MEP_ID_NON_ACC_BIT)

/** End of macros that extract SW state endpoint ID flags */

/** 
 *  List of BFD tunnel types supported by Jericho 2
 *  This list is used to verify the user provided arguments
 *  when adding a new BFD endpoint - using a type not in this
 *  list will generate an error.
 */
static const uint8 supported_bfd_mep_types[] = {
    bcmBFDTunnelTypeUdp,
    bcmBFDTunnelTypeMpls,
    bcmBFDTunnelTypePweControlWord,
    bcmBFDTunnelTypePweRouterAlert,
    bcmBFDTunnelTypePweTtl,
    bcmBFDTunnelTypeMplsTpCc,
    bcmBFDTunnelTypePweGal
};

/** Longest name in the list below plus a small margin */
#define MAX_FIELD_NAME 32

/** Minimum value for BFD UPD SPORT */
#define MIN_SPORT_VAL 49152

/** Number of relevant bits in the int_pri field */
#define NOF_INT_PRI_BITS 5

/** Maximum value for exp field */
#define MAX_EXP_VAL 7

/** Maximum value for sampling_ratio field */
#define MAX_SAMPLING_RATIO 8

/** Maximum value for subnet_len field */
#define MAX_SUBNET_LENGTH 32

/** Maximum value for local_state and remote_state fields */
#define MAX_STATE 3

/** Maximum value for local_diag and remote_diag fields */
#define MAX_DIAG 9

/** Maximum value for loc_clear_thresh field */
#define MAX_LOC_CLEAR_THRESH 3

/** Mask for local_flags - other bits must be 0 */
#define LOCAL_FLAGS_BITS_MASK 0x3A

/** If not a short entry, bits under this mask must be 0 */
#define SHORT_ENTRY_BITS_MASK 0x3

/** MPLS/PWE label is only 20 bits */
#define MAX_MPLS_LABEL SAL_UPTO_BIT(20)

/** Structure needed to verify that a field is 0   */
typedef struct
{
    uint8 field_name[MAX_FIELD_NAME];
    uint16 field_offset;
    uint8 field_size;
} bfd_field_data_t;

/** Shortcut to convert a field to the table below   */
#define FIELD_INFO(x, y) {#y, UTILEX_OFFSETOF(x, y), sizeof(((x *)0)->y)}

/**
 *  This struct is used to store data temporarily for the
 *  process of creating, modifying, reading or deleting BFD
 *  endpoints.
 *  Any change in a BFD endpoint is spread across several
 *  functions.  This structure is used to save data that is
 *  determined in one function, to be applied in a subsequent
 *  stage of the creating/modifying/reading/deleting process.
 */
typedef struct
{
    /**
     *  Structure for data that will be read from templates or
     *  written to new entries.  Used in bcm_dnx_endpoint_create and
     *  bcm_dnx_endpoint_get.
     */
    struct
    {
        /** Data for writing to OAMA/OAMB   */
        oam_action_key_t action_write_key;
        oam_action_content_t action_content;
        uint32 dip_write_index;

        /** Data for discriminator range   */
        uint32 discr_range_min;
        uint32 discr_range_max;

        /** Data for writing to Destination IP table   */
        bcm_ip6_t dip_ipv6_address;

        /** Prefix for LIF table   */
        uint32 lif_key_prefix;

        /** Flags: write or not?   */
        uint32 action_write;
        uint32 dip_write;
        uint8 discr_range_write;
    } gathered_data;

    /**
     *  Structure for data that will be used to delete entries.
     *  Used in bcm_dnx_endpoint_destroy and in
     *  bcm_dnx_endpoint_create when used to modify an existing
     *  BFD endpoint.
     */
    struct
    {
        /** Data for deleting from OAMA/OAMB   */
        oam_action_key_t action_delete_key;

        /** Data for deleting from Destination IP table   */
        uint32 dip_delete_index;

        /** Flags: delete or not?   */
        uint8 action_delete;
        uint8 dip_delete;
    } deleted_data;

    /** ID of endpoint   */
    uint32 endpoint_id;

    /** Type of endpoint   */
    bcm_bfd_tunnel_type_t endpoint_type;

    /**
     *  Flags for BFD ep - Multihop, IPv6, Accelerated in HW, OAMB_profile
     *  This field uses DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS flags.
     */
    uint32 bfd_sw_state_flags;

    /** Data for writing to or deleting from OAMA/OAMB   */
    uint32 action_flags;

    /** Data for writing to classifier tables   */
    uint32 lif_id;

    /** Remote endpoint ID */
    uint32 rmep_index;

} bfd_temp_endpoint_data_t;

/** 
 *  A table containing information about fields in the
 *  bcm_bfd_endpoint_info_t that are not supported by Jericho
 *  2.  This list is used to verify that all these unsupported
 *  fields have been set to 0 when adding a BFD endpoint.
 */
static const bfd_field_data_t unsup_params_tbl[] = {
    FIELD_INFO(bcm_bfd_endpoint_info_t, vpn),
    FIELD_INFO(bcm_bfd_endpoint_info_t, vlan_pri),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_vlan_pri),
    FIELD_INFO(bcm_bfd_endpoint_info_t, vrf_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_dst_ip_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_dst_ip6_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_src_ip_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_src_ip6_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_ip_tos),
    FIELD_INFO(bcm_bfd_endpoint_info_t, inner_ip_ttl),
    FIELD_INFO(bcm_bfd_endpoint_info_t, mep_id_length),
    FIELD_INFO(bcm_bfd_endpoint_info_t, cpu_qid),
    FIELD_INFO(bcm_bfd_endpoint_info_t, auth),
    FIELD_INFO(bcm_bfd_endpoint_info_t, auth_index),
    FIELD_INFO(bcm_bfd_endpoint_info_t, tx_auth_seq),
    FIELD_INFO(bcm_bfd_endpoint_info_t, rx_auth_seq),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_mep_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_mep_id_length),
    FIELD_INFO(bcm_bfd_endpoint_info_t, mis_conn_mep_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, mis_conn_mep_id_length),
    FIELD_INFO(bcm_bfd_endpoint_info_t, pkt_vlan_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, rx_pkt_vlan_id)
};

/** Number of rows in table above   */
#define BFD_NOF_JER2_UNSUP_PARAMS (sizeof(unsup_params_tbl) / sizeof(bfd_field_data_t))

/** 
 *  A table containing information about fields in the
 *  bcm_bfd_endpoint_info_t that in Jericho 2, may be used
 *  for accelerated endpoints only.  This list is used to
 *  verify that all these fields have been set to 0 when adding
 *  a non-accelerated BFD endpoint.
 */
static const bfd_field_data_t acc_only_params_tbl[] = {
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, bfd_period),
    FIELD_INFO(bcm_bfd_endpoint_info_t, dst_ip_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, dst_ip6_addr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, ip_tos),
    FIELD_INFO(bcm_bfd_endpoint_info_t, ip_ttl),
    FIELD_INFO(bcm_bfd_endpoint_info_t, udp_src_port),
    FIELD_INFO(bcm_bfd_endpoint_info_t, label),
    FIELD_INFO(bcm_bfd_endpoint_info_t, egress_if),
    FIELD_INFO(bcm_bfd_endpoint_info_t, mep_id),
    FIELD_INFO(bcm_bfd_endpoint_info_t, int_pri),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_state),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_diag),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_flags),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_min_tx),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_min_rx),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_min_echo),
    FIELD_INFO(bcm_bfd_endpoint_info_t, local_detect_mult),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_flags),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_state),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_discr),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_diag),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_min_tx),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_min_rx),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_min_echo),
    FIELD_INFO(bcm_bfd_endpoint_info_t, remote_detect_mult),
    FIELD_INFO(bcm_bfd_endpoint_info_t, sampling_ratio),
    FIELD_INFO(bcm_bfd_endpoint_info_t, loc_clear_threshold),
    FIELD_INFO(bcm_bfd_endpoint_info_t, ip_subnet_length),
    FIELD_INFO(bcm_bfd_endpoint_info_t, bfd_detection_time)
};

/** Number of rows in table above   */
#define BFD_NOF_JER2_ACC_ONLY_PARAMS (sizeof(acc_only_params_tbl) / sizeof(bfd_field_data_t))

/**
 *  Maximum size of bcm_bfd_endpoint_info_t fields that are
 *  not supported by Jericho 2
 */
#define MAX_ARG_FIELD_SIZE 40

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
 * \brief - Utility function: search an array for a certain 
 *        value.  If found, return index.  Otherwise return -1.
 * 
 * \param [in] array - array to search
 * \param [in] value - value for comparison
 * \param [in] array_len - length of array
 *   
 * \return
 *   int index if successful, -1 otherwise
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_find_in_uint8_array(
    const uint8 *array,
    uint8 value,
    uint8 array_len)
{
    int index;
    if (array != NULL)
    {
        for (index = 0; index < array_len; index++)
        {
            if (array[index] == value)
            {
                return index;
            }
        }
    }
    return -1;
}

shr_error_e
bcm_dnx_bfd_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_bfd_init not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function that provide the correct trap_id
 *        based on endpoint type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] trap_id - The id of the trap that was created.
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
dnx_bfd_endpoint_trap_id_get(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    int *trap_id)
{

    int flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (endpoint_info->type)
    {
        case bcmBFDTunnelTypeUdp:
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdIpv6, trap_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdIpv4, trap_id));
            }
        }
            break;
        case bcmBFDTunnelTypeMpls:
        case bcmBFDTunnelTypeMplsTpCc:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdMpls, trap_id));
        }
            break;
        case bcmBFDTunnelTypePweControlWord:
        case bcmBFDTunnelTypePweRouterAlert:
        case bcmBFDTunnelTypePweTtl:
        case bcmBFDTunnelTypePweGal:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdPwe, trap_id));
        }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported bcm bfd tunnel type");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write default values into struct, so they are later 
 *        written to the hardware tables.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_endpoint_default_fill(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Default BFD endpoint not supported yet");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Make sure that current BFD endpoint and new 
 *        parameter values can be combined legally.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] existing_endpoint_info - Data read from existing
 *        BFD endpoint.
 * \param [in] endpoint_info - New parameter values.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Parameter verification done in
 *     dnx_bfd_endpoint_create_verify() is not repeated here.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_endpoint_mod_verify(
    int unit,
    bcm_bfd_endpoint_info_t * existing_endpoint_info,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (existing_endpoint_info->type != endpoint_info->type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: type can not be updated.\n");
    }
    if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(existing_endpoint_info->type))
    {
        /** 
         *  If the MEP is classified by discriminator, check the
         *  discriminator provided by the user
         */
        if (existing_endpoint_info->local_discr != endpoint_info->local_discr)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: local discriminator may not be updated.\n");
        }
    }
    else
    {
        /** 
         *  If the MEP is not classified by discriminator, check the
         *  gport parameter provided by the user (this is used as the
         *  LIF for classification)
         */
        if (BCM_GPORT_LOCAL_GET(existing_endpoint_info->gport) != BCM_GPORT_LOCAL_GET(endpoint_info->gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: gport may not be updated.\n");
        }
    }
    if (_SHR_IS_FLAG_SET(existing_endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) !=
        _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Multihop configuration mismatch.\n");
    }
    if (_SHR_IS_FLAG_SET(existing_endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6) !=
        _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Switching from IPv4 to IPv6 or vice versa is not allowed.\n");
    }
    if (existing_endpoint_info->ipv6_extra_data_index != endpoint_info->ipv6_extra_data_index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Changing IPv6 extra data index is not allowed.\n");
    }

    if (_SHR_IS_FLAG_SET(existing_endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW) !=
        _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: an accelerated endpoint cannot be modified to non-"
                     "accelerated or vice versa.\n");
    }

    if ((BFD_TYPE_NOT_PWE(endpoint_info->type)) &&
        (existing_endpoint_info->udp_src_port != endpoint_info->udp_src_port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: udp_src_port may not be updated.\n");
    }

    if (existing_endpoint_info->remote_id != endpoint_info->remote_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: remote_id may not be updated.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function applies the local discriminator 
 *        configured for a new BFD endpoint to the discriminator
 *        range mechanism.  The API will update the template
 *        manager with the configured discriminator range and
 *        update the hw_data values for later HW write.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)  In this function,
 *        discr_range_write is set, and if it is TRUE, so are
 *        discr_range_min and discr_range_max.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Range is configured by the first endpoint.  In this case
 *     the values will be saved to be written to the hardware
 *     registers later.  Any endpoint added subsequently must
 *     fall within that range.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_discriminator_range_update(
    int unit,
    const bcm_bfd_endpoint_info_t * const endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    uint32 new_range;
    dnx_bfd_discr_range_t range;
    int dummy;
    uint8 write_hw;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /** 
     *  Pointer arguments are assumed to have been checked as
     *  non-zero.
     */

    /** 
     *  The discriminator range "resource" is treated as
     *  a "template"
     */
    new_range = (endpoint_info->local_discr >> DISCR_RANGE_BIT_SHIFT);

    /**
     *  Currently the min and max of the range are equal.  The
     *  resulting range is all values that have the same N most
     *  significant bits as the first endpoint defined, where N is
     *  "32 - DISCR_RANGE_BIT_SHIFT," which comes out to
     *  2^DISCR_RANGE_BIT_SHIFT possible values.
     */
    range.range_min = new_range;
    range.range_max = new_range;

    if(dnx_data_bfd.property.discriminator_type_update_get(unit) == 1)
    {
        /**
         * BFD MEP type in discriminator mode:
         * In this mode, discriminator MSB can very from one endpoint to another,
         * So the range, minimum has MSB=0, and the range maximum has MSB=1
         */
        UTILEX_SET_BIT(range.range_min, 0, 31 - DISCR_RANGE_BIT_SHIFT);
        UTILEX_SET_BIT(range.range_max, 1, 31 - DISCR_RANGE_BIT_SHIFT);
    }

    rv = algo_bfd_db.bfd_tyour_discr_range.allocate_single(unit, _SHR_CORE_ALL, NO_FLAGS, &range, NULL, &dummy,
                                                           &write_hw);
    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tyour_discr_range.profile_data_get
                        (unit, _SHR_CORE_ALL, DISCR_PROF, &dummy, &range));
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: BFD Your-Discriminator has been set.  The upper 18 bits must be %d."
                     "The upper 18 bits specified in local_discriminator are %d\n", range.range_min, new_range);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    hw_write_data->gathered_data.discr_range_write = write_hw;
    if (write_hw == TRUE)
    {
        /** 
         *  Save data for writing to HW table at the end.  Writing to
         *  hardware registers is done in
         *  dnx_bfd_endpoint_db_write_to_hw
         */
        hw_write_data->gathered_data.discr_range_min = range.range_min;
        hw_write_data->gathered_data.discr_range_max = range.range_max;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets up a trap and snoop for a new 
 *        BFD endpoint.  It also prepares to write to the OAMA
 *        or OAMB table, if necessary.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_mp_profile_sw_update(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    dnx_bfd_mep_profile_t profile_data;
    uint32 relevant_id;
    int profile_id, trap_id;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * In case of Accelerated MEP we can set up the trap with remote_gport field
     * or to use default configuration per MEP type.
    */
    if (!BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)
        && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_trap_id_get(unit, endpoint_info, &trap_id));
        profile_data.trap_code = trap_id;
        profile_data.forwarding_stregth = 7;
        profile_data.snoop_strength = 0;
    }
    else
    {
        relevant_id = endpoint_info->remote_gport;
        profile_data.trap_code = BCM_GPORT_TRAP_GET_ID(relevant_id);
        profile_data.forwarding_stregth = BCM_GPORT_TRAP_GET_STRENGTH(relevant_id);
        profile_data.snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(relevant_id);
    }

    /** 
     *  One of two diffrent templates is used, because the profiles
     *  come from two independent tables
     */
    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_OAMB_PROFILE))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamb_action.allocate_single
                        (unit, _SHR_CORE_ALL, NO_FLAGS, &profile_data, NULL, &profile_id, &write_hw));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oama_action.allocate_single
                        (unit, _SHR_CORE_ALL, NO_FLAGS, &profile_data, NULL, &profile_id, &write_hw));
    }

    hw_write_data->gathered_data.action_write = write_hw;
    hw_write_data->gathered_data.action_write_key.mp_profile = profile_id;

    if (write_hw == TRUE)
    {
        /** Set flags   */
        hw_write_data->action_flags = DNX_OAM_CLASSIFIER_INGRESS;
        /** Build entry key */
        hw_write_data->gathered_data.action_write_key.is_inject = 0;
        hw_write_data->gathered_data.action_write_key.oam_internal_opcode = 0;
        hw_write_data->gathered_data.action_write_key.is_my_mac = 0;
        hw_write_data->gathered_data.action_write_key.mp_type = DBAL_ENUM_FVAL_MP_TYPE_BFD;
        hw_write_data->gathered_data.action_write_key.mp_profile = profile_id;
        hw_write_data->gathered_data.action_write_key.da_is_mc = 0;

        /** Build entry data */
        hw_write_data->gathered_data.action_content.table_specific_fields.ingress_only_fields.ingress_trap_code =
            profile_data.trap_code;
        hw_write_data->gathered_data.action_content.forwarding_strength = profile_data.forwarding_stregth;
        hw_write_data->gathered_data.action_content.snoop_strength = profile_data.snoop_strength;
        hw_write_data->gathered_data.action_content.table_specific_fields.ingress_only_fields.is_up_mep = FALSE;
        hw_write_data->gathered_data.action_content.table_specific_fields.ingress_only_fields.meter_disable = FALSE;
        hw_write_data->gathered_data.action_content.oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL;
        if (_SHR_IS_FLAG_SET
            (hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_OAMB_PROFILE))
        {
            hw_write_data->action_flags |= DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;
            hw_write_data->gathered_data.action_content.oamb_counter_disable = FALSE;
        }
        else
        {
            hw_write_data->action_flags |= DNX_OAM_CLASSIFIER_LIF_ACTION;
            hw_write_data->gathered_data.action_content.oama_counter_disable_access_0 = FALSE;
            hw_write_data->gathered_data.action_content.oama_counter_disable_access_1 = FALSE;
            hw_write_data->gathered_data.action_content.oama_counter_disable_access_2 = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add a BFD DIP entry to the
 *        DIP hardware table, or if that entry already exists,
 *        increases the relevant counter.  This is achieved by
 *        using the template "BFD DIP"
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_dip_sw_update(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    uint8 write_hw;
    bcm_ip6_t ipv6_address;
    int dip_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_IS_IPV6))
    {
        /** IPv6 address */
        sal_memcpy(ipv6_address, endpoint_info->src_ip6_addr, sizeof(bcm_ip6_t));
    }
    else
    {
        /** IPv4 address */
        BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(endpoint_info->src_ip_addr, ipv6_address);
    }

    rv = algo_bfd_db.bfd_dip.allocate_single(unit, _SHR_CORE_ALL, NO_FLAGS, ipv6_address, NULL, &dip_id, &write_hw);
    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free DIP entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    hw_write_data->gathered_data.dip_write = write_hw;
    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        hw_write_data->gathered_data.dip_write_index = dip_id;
        sal_memcpy(hw_write_data->gathered_data.dip_ipv6_address, ipv6_address, sizeof(bcm_ip6_t));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function builds the ID of the newly created BFD 
 *        endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Structure of the calculated endpoint ID:\n
 *      The "endpoint_id" which will be used for packet
 *      classification is either the lower 22 bits of the local
 *      discriminator field (for endpoints that classify packets
 *      by discriminator) or the LIF ID (provided by the user in
 *      the gport field)\n
 *      The endpoint ID returned to the user and saved to the SW
 *      state table also superimposes two addition bits:\n
 *      if bit 22 is set, the endpoint is not accelerated.\n
 *      if bit 23 is set, the endpoint classifies packets by
 *      discriminator.
 *      hw_write_data->endpoint_id will be used for classification
 *      endpoint_info->id is returned to the user and saved to the
 *      SW state table.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_build_endpoint_id(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    uint32 lif_id, oam_id_nof_bits, full_entry_threshold;
    int alloc_flags = 0, mep_db_lim;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_oamp_mep_db_memory_type_t memory_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Classification is the same for accelerated and non-accelerated endpoints */
    if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
    {
        /** Only these bits are used as lookup key - do not use all available bits */
        lif_id = UTILEX_GET_BITS_RANGE(endpoint_info->local_discr, DISCR_RANGE_BIT_SHIFT - 1, 0);
        endpoint_info->id = lif_id;
        endpoint_info->id |= (SAL_BIT(BFD_MEP_ID_DISCR_CLASS) | SAL_BIT(BFD_MEP_ID_NON_ACC_BIT));
        hw_write_data->gathered_data.lif_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, endpoint_info->gport,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF |
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                                                           &gport_hw_resources));
        lif_id = gport_hw_resources.global_in_lif;
        endpoint_info->id = lif_id;
        endpoint_info->id |= SAL_BIT(BFD_MEP_ID_NON_ACC_BIT);
        hw_write_data->gathered_data.lif_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH;
    }
    hw_write_data->endpoint_id = lif_id;

    /**
     *  For accelerated endpoints, the MEP ID returned to the user is always discriminator based,
     *  even if the classification is not.
     */
    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        oam_id_nof_bits = dnx_data_oam.oamp.mep_id_nof_bits_get(unit);
        full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
        lif_id = UTILEX_GET_BITS_RANGE(endpoint_info->local_discr, oam_id_nof_bits - 1, 0);
        endpoint_info->id = lif_id;
        if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
        {
            hw_write_data->endpoint_id = lif_id;
        }

        if (lif_id >= full_entry_threshold)
        {
            if (((lif_id & SHORT_ENTRY_BITS_MASK) != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: illegal discriminator 0x%08X.  Resulting endpoint ID 0x%04X is above "
                             "full entry threshold 0x%04X, but is not a multiple of 4", endpoint_info->local_discr,
                             lif_id, full_entry_threshold);
            }
        }
        else
        {
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only self-contained endpoints are allowed in JR1 system headers mode");
            }
        }

        /** Limitations for short entries */
        mep_db_lim = dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
        if (mep_db_lim == 1)
        {
            SHR_IF_ERR_EXIT(dnx_bfd_oamp_short_entry_type_verify(unit, lif_id, endpoint_info));
        }

        /** Allocate this endpoint ID so that others cannot use it */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &lif_id));

        /** While you're at it, allocate remote endpoint ID as well */
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_REMOTE_WITH_ID))
        {
            alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_alloc(unit, alloc_flags, &endpoint_info->remote_id));

        /** Later it is saved to the SW state table */
        hw_write_data->rmep_index = endpoint_info->remote_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function writes a newly created BFD endpoint to 
 *        the designated SW state BFD endpoint table.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_save_endpoint_to_sw_state_table(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_endpoint_data_t * hw_write_data)
{
    uint32 entry_handle_id;
    dbal_entry_action_flags_e commit_flags;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    commit_flags = DBAL_COMMIT;

    /** 
     *  This flag is necessary if a dbal entry is updated
     *  instead being newly created
     */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE))
    {
        commit_flags = DBAL_COMMIT_UPDATE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_ID, endpoint_info->id);
    /** 
     * hw_write_data structure uses the same flags as the flags defined in BFD_ENDPOINT_SW_STATE_FLAGS
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_SW_STATE_FLAGS, INST_SINGLE,
                                 hw_write_data->bfd_sw_state_flags);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_BFD_TUNNEL_TYPE,
                                 INST_SINGLE, endpoint_info->type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_MEP_PROFILE, INST_SINGLE,
                                 hw_write_data->gathered_data.action_write_key.mp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIP_PROFILE, INST_SINGLE,
                                 hw_write_data->gathered_data.dip_write_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, hw_write_data->rmep_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, hw_write_data->endpoint_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function is the point of creating or modifying 
 *        endpoints - writing the data to the classifier (the
 *        LIF DB for non accelerated enpoints, or the ACC MEP DB
 *        for accelerated endpoints)
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_endpoint_db_add(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    dnx_oam_lif_entry_key_t lif_key;
    dnx_oam_lif_entry_value_t lif_value;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    flags = DNX_OAM_CLASSIFIER_INGRESS;
    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
        {
            flags |= DNX_OAM_CLASSIFIER_BFD;
        }
        SHR_IF_ERR_EXIT(dnx_oam_acc_mep_add
                        (unit, _SHR_CORE_ALL, flags, hw_write_data->endpoint_id, 0,
                         endpoint_info->id, hw_write_data->gathered_data.action_write_key.mp_profile));
    }
    else
    {
        lif_key.core_id = _SHR_CORE_ALL;
        lif_key.flags = DNX_OAM_CLASSIFIER_INGRESS;
        lif_key.oam_key_prefix = hw_write_data->gathered_data.lif_key_prefix;
        lif_key.oam_lif_id = hw_write_data->endpoint_id;
        sal_memset(lif_value.mdl_mp_type, 0, sizeof(lif_value.mdl_mp_type));
        lif_value.mdl_mp_type[0] = DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
        lif_value.mp_profile = hw_write_data->gathered_data.action_write_key.mp_profile;
        lif_value.counter_base = 0;
        lif_value.counter_interface = 0;
        SHR_IF_ERR_EXIT(dnx_oam_lif_add(unit, DBAL_COMMIT, &lif_key, &lif_value));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write the BFD endpoint data to the relevant data 
 *        tables, where needed.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables has been placed (see struct
 *        definition for reason.)
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
dnx_bfd_endpoint_db_write_to_hw(
    int unit,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (hw_write_data->gathered_data.discr_range_write)
    {
        /** Write data to discriminator range fields */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_GENERAL_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_YOUR_DISCR_RANGE_MIN, INST_SINGLE,
                                     hw_write_data->gathered_data.discr_range_min);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_YOUR_DISCR_RANGE_MAX, INST_SINGLE,
                                     hw_write_data->gathered_data.discr_range_max);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** BFD Echo doesn't use OAM Classifier */
    if (!_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        if (hw_write_data->gathered_data.action_write)
        {
            /** Write data to action table */
            SHR_IF_ERR_EXIT(dnx_oam_action_set
                            (unit, hw_write_data->action_flags, &hw_write_data->gathered_data.action_write_key,
                             &hw_write_data->gathered_data.action_content));
        }
    }

    if (hw_write_data->gathered_data.dip_write)
    {
        /** Write data to BFD DIP table   */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_DEST_IP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIP_PROFILE,
                                   hw_write_data->gathered_data.dip_write_index);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP, INST_SINGLE,
                                        hw_write_data->gathered_data.dip_ipv6_address);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function looks up an endpoint from the BFD SW 
 *        state MEP table, and reads the data to the struct
 *        referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of BFD endpoint to retrieve
 * \param [out] sw_read_data - Pointer to temporary structure to
 *        which data will be written for subsequent use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * endpoint_info was verified before this function was
 *     called.  sw_read_data was allocated using SHR_ALLOC.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_get_endpoint_from_sw_state_table(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bfd_temp_endpoint_data_t * sw_read_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_ID, endpoint);
    /** 
     * sw_read_data structure uses the same flags as the flags defined in BFD_ENDPOINT_SW_STATE_FLAGS
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_SW_STATE_FLAGS, INST_SINGLE,
                               &sw_read_data->bfd_sw_state_flags);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BCM_BFD_TUNNEL_TYPE,
                               INST_SINGLE, &sw_read_data->endpoint_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BFD_MEP_PROFILE, INST_SINGLE,
                               &sw_read_data->gathered_data.action_write_key.mp_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BFD_DIP_PROFILE, INST_SINGLE,
                               &sw_read_data->gathered_data.dip_write_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, &sw_read_data->rmep_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &sw_read_data->lif_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Needed for deleting and updating   */
    sw_read_data->deleted_data.dip_delete_index = sw_read_data->gathered_data.dip_write_index;
    sw_read_data->deleted_data.action_delete_key.mp_profile = sw_read_data->gathered_data.action_write_key.mp_profile;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function deletes a reference from the relevant 
 *        BFD action profile template, as part of the deletion
 *        of a BFD endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] hw_delete_data - struct to which data was 
 *        read from the SW state table, and to which data is
 *        written for later usage (deleting HW table entries.)
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
dnx_bfd_free_mp_profile_in_template(
    int unit,
    bfd_temp_endpoint_data_t * hw_delete_data)
{
    SHR_FUNC_INIT_VARS(unit);

    hw_delete_data->action_flags = DNX_OAM_CLASSIFIER_INGRESS;

    if (_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_OAMB_PROFILE))
    {
        /** 
         *  If the endpoint is accelerated to a local or remote OAMP,
         *  an OAMB entry and its respective template profile should
         *  be used.
         */
        hw_delete_data->action_flags |= DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamb_action.free_single
                        (unit, _SHR_CORE_ALL, hw_delete_data->deleted_data.action_delete_key.mp_profile,
                         &hw_delete_data->deleted_data.action_delete));
    }
    else
    {
        /** 
         *  Otherwise, an OAMA entry and its respective template profile should
         *  be used.
         */
        hw_delete_data->action_flags |= DNX_OAM_CLASSIFIER_LIF_ACTION;
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oama_action.free_single
                        (unit, _SHR_CORE_ALL, hw_delete_data->deleted_data.action_delete_key.mp_profile,
                         &hw_delete_data->deleted_data.action_delete));
    }

    if (hw_delete_data->deleted_data.action_delete)
    {
        /** Build key for deletion  */
        hw_delete_data->deleted_data.action_delete_key.mp_type = DBAL_ENUM_FVAL_MP_TYPE_BFD;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function deletes an entry from the BFD endpoint 
 *        SW state table as part of deletion of a BFD endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_delete_data - Data gathered earlier for 
 *        deletion.
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
dnx_bfd_remove_endpoint_from_sw_state_table(
    int unit,
    bfd_temp_endpoint_data_t * hw_delete_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_ID, hw_delete_data->endpoint_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function deletes an entry from the MDB table 
 *        INGRESS_OAM_LIF_DB or INGRESS_OAM_ACC_MEP_DB as part
 *        of the destruction or modification of a BFD endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_delete_data - Struct that contains data 
 *        gathered during the processing of the software tables.
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
dnx_bfd_endpoint_db_delete(
    int unit,
    bfd_temp_endpoint_data_t * hw_delete_data)
{
    dnx_oam_lif_entry_key_t lif_key;
    uint32 acc_oam_lif, flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Is the relevant MDB table OAM_LIF_DB or OAM_ACC_MEP_DB?   */
    if (_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        if (BFD_TYPE_NOT_PWE(hw_delete_data->endpoint_type))
        {
            acc_oam_lif = hw_delete_data->endpoint_id;
        }
        else
        {
                /** In this case, this is the key */
            acc_oam_lif = hw_delete_data->lif_id;
        }
        flags = DNX_OAM_CLASSIFIER_INGRESS;
        if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(hw_delete_data->endpoint_type))
        {
            flags |= DNX_OAM_CLASSIFIER_BFD;
        }
        SHR_IF_ERR_EXIT(dnx_oam_acc_mep_delete(unit, _SHR_CORE_ALL, flags, acc_oam_lif, 0));
    }
    else
    {
        lif_key.core_id = _SHR_CORE_ALL;
        lif_key.flags = DNX_OAM_CLASSIFIER_INGRESS;
        /** 
         *  If packets addressed to this BFD endpoint are classified
         *  by discriminator, the hardware knows this from the OAM
         *  key prefix
         */
        if (UTILEX_GET_BIT(hw_delete_data->endpoint_id, BFD_MEP_ID_DISCR_CLASS))
        {
            lif_key.oam_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD;
        }
        else
        {
            lif_key.oam_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH;
        }
         lif_key.oam_lif_id = UTILEX_GET_BITS_RANGE(hw_delete_data->endpoint_id, DISCR_RANGE_BIT_SHIFT - 1, 0);
        SHR_IF_ERR_EXIT(dnx_oam_lif_delete(unit, &lif_key));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function deletes entries from the BFD DIP table 
 *        and the BFD profile tables (OAMA or OAMB) as part of
 *        the destruction of a BFD endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_delete_data - Struct that contains data 
 *        gathered during the processing of the software tables.
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
dnx_bfd_endpoint_db_remove_from_hw(
    int unit,
    bfd_temp_endpoint_data_t * hw_delete_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (hw_delete_data->deleted_data.action_delete)
    {
        /** Write data to action table */
        SHR_IF_ERR_EXIT(dnx_oam_action_clear
                        (unit, hw_delete_data->action_flags, &hw_delete_data->deleted_data.action_delete_key));
    }

    if (hw_delete_data->deleted_data.dip_delete)
    {
        /** Write data to BFD DIP table   */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_DEST_IP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIP_PROFILE,
                                   hw_delete_data->deleted_data.dip_delete_index);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function checks if a BFD endpoint modification 
 *        entails modification of the destination IP field.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] existing_endpoint_info - Data read from existng
 *        BFD endpoint.
 * \param [in] endpoint_info - New parameter values.
 *   
 * \return
 *   uint8 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static uint8
dnx_bfd_check_dip_update(
    int unit,
    bcm_bfd_endpoint_info_t * existing_endpoint_info,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    /** Is DIP even relevant?   */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
    {
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
        {
            /** For IPv6, compare field src_ip6_addr  */
            if (sal_memcmp(existing_endpoint_info->src_ip6_addr, endpoint_info->src_ip6_addr, sizeof(bcm_ip6_t)))
            {
                return TRUE;
            }
        }
        else
        {
            /** For IPv4, compare field src_ip_addr   */
            if (existing_endpoint_info->src_ip_addr != endpoint_info->src_ip_addr)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * \brief - This function sets the flags to hw_write_data
 * This function is a part of the bcm_dnx_bfd_endpoint_create
 * API.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 *
 * \return
 *   uint32 - Returns the value of the flags suitable for
 *          structure bfd_temp_endpoint_data_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static uint32
dnx_bfd_endpoint_flags_to_sw_state_flags(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info)
{
    uint32 return_flags = 0;

    /** Check flags and set _IS_ACCELERATED*/
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED;
    }

    /** Check flags and set _BFD_IS_IPV6*/
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_IS_IPV6;
    }

    /** Check flags and set _MULTI_HOP_BFD*/
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_MULTI_HOP_BFD;
    }

    /** Check flags and set _BFD_ECHO*/
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO;
    }

    /** Check flags and set _USE_OAMB_PROFILE*/
    if (_SHR_IS_FLAG_SET(return_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED) ||
        _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_HW_ACCELERATION_SET))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_OAMB_PROFILE;
    }

    /** Check flags and set _EXPLICIT_DETECTION_TIME */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_EXPLICIT_DETECTION_TIME;
    }

    /** Check if My-BFD-DIP-destination is used */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_MY_BFD_DIP_DESTINATION;
    }

    /** Check if local discriminator is 0 */
    if (endpoint_info->local_discr == 0)
    {
        return_flags |= DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_LOCAL_DISCR_IS_NULL;
    }

    return return_flags;
}

/**
 * \brief - This function modifies the specified BFD endpoint.
 *  This function is a part of the bcm_dnx_bfd_endpoint_create
 *  API.  It reads the specified endpoint from the database,
 *  combines the new arguments with the existing arguments, and
 *  then writes the result to the database.
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_update_data - Pointer to struct where data 
 *        for writing to HW tables and deleting from them should
 *        be placed (see struct definition for reason.)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Currently on non-accelerated endpoints are supported,
 *     because the OAMP design is not complete.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_endpoint_update(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_update_data)
{
    bcm_bfd_endpoint_info_t *existing_endpoint_info;
    bfd_temp_oamp_mep_db_data_t *mep_hw_write_data = NULL;
    bfd_temp_oamp_rmep_db_data_t *rmep_hw_write_data = NULL;
    dnx_bfd_oamp_endpoint_t *mep_entry_values = NULL;
    dnx_oam_oamp_rmep_db_entry_t *rmep_entry_values = NULL;
    dnx_bfd_oamp_endpoint_t *new_mep_entry_values = NULL;
    dnx_oam_oamp_rmep_db_entry_t *new_rmep_entry_values = NULL;
    bfd_temp_oamp_db_delete_data_t *oamp_hw_delete_data = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate buffer for parameter verification   */
    existing_endpoint_info = NULL;
    SHR_ALLOC_SET_ZERO(existing_endpoint_info, sizeof(bcm_bfd_endpoint_info_t), "BFD endpoint data read for comparison",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get data for this endpoint */
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_bfd_endpoint_get(unit, endpoint_info->id, existing_endpoint_info));

    /** 
     *  Check that combining the new arguments with the existing
     *  endpoint results in a legal endpoint
     */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_mod_verify(unit, existing_endpoint_info, endpoint_info));

    /** Read data from SW state table to update struct   */
    SHR_IF_ERR_EXIT(dnx_bfd_get_endpoint_from_sw_state_table(unit, endpoint_info->id, hw_update_data));

    /** Initialize flags with SW state data */
    hw_update_data->bfd_sw_state_flags = dnx_bfd_endpoint_flags_to_sw_state_flags(unit, endpoint_info);

/** 
 *  First section: software tables - software state
 *  and templates
 */

    /** Change data in software memory: templates */

    if (existing_endpoint_info->remote_gport != endpoint_info->remote_gport)
    {
        /** 
         *  if remote_gport is different, a different profile is
         *  needed
         */

        /** 
         *  Delete reference to action profile.
         */
        SHR_IF_ERR_EXIT(dnx_bfd_free_mp_profile_in_template(unit, hw_update_data));

        /** Add new reference to action profile   */
        SHR_IF_ERR_EXIT(dnx_bfd_mp_profile_sw_update(unit, endpoint_info, hw_update_data));
    }

    /** Should the destination IP be modified?   */
    if (dnx_bfd_check_dip_update(unit, existing_endpoint_info, endpoint_info) == TRUE)
    {
        /** Delete reference to DIP */
        if (!_SHR_IS_FLAG_SET(existing_endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
        {
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_dip.free_single
                            (unit, _SHR_CORE_ALL, hw_update_data->deleted_data.dip_delete_index,
                             &hw_update_data->deleted_data.dip_delete));
        }

        /** Add new reference to DIP profile   */
        if (!_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
        {
            SHR_IF_ERR_EXIT(dnx_bfd_dip_sw_update(unit, endpoint_info, hw_update_data));
        }
    }

    /** For accelerated endpoints */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        SHR_ALLOC_SET_ZERO(mep_hw_write_data, sizeof(bfd_temp_oamp_mep_db_data_t),
                           "Data saved to write to OAMP MEP DB table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_hw_write_data, sizeof(bfd_temp_oamp_rmep_db_data_t),
                           "Data saved to write to OAMP RMEP DB table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(mep_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure used to call OAMP MEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_entry_values, sizeof(dnx_oam_oamp_rmep_db_entry_t),
                           "Structure used to call OAMP RMEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(new_mep_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure used to call OAMP MEP DB create", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(new_rmep_entry_values, sizeof(dnx_oam_oamp_rmep_db_entry_t),
                           "Structure used to call OAMP RMEP DB create", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(oamp_hw_delete_data, sizeof(bfd_temp_oamp_db_delete_data_t),
                           "Flags that indicated if HW entries should be deleted", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /** Read from OAMP MEP DB table */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_get(unit, endpoint_info->id, mep_entry_values));

        /** Read from OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, endpoint_info->remote_id, FALSE, rmep_entry_values));

        /** Modify the relevant reference counts */
        SHR_IF_ERR_EXIT(dnx_bfd_modify_oamp_data_in_templates(unit, endpoint_info, existing_endpoint_info,
                                                              mep_entry_values, rmep_entry_values, oamp_hw_delete_data,
                                                              mep_hw_write_data, rmep_hw_write_data));

        /** Build structure for creating OAMP MEP DB entry */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_build_param_for_oamp_mep_db
                        (unit, endpoint_info, mep_hw_write_data, new_mep_entry_values));

        /** Build structure for creating OAMP RMEP DB entry */
        dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(unit, endpoint_info, rmep_hw_write_data,
                                                         new_rmep_entry_values);
    }

    /** End of data modification in templates section   */

    /** Modify data in SW state table   */
    if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
    {
        /** Make sure this value is not modified! */
        hw_update_data->endpoint_id = hw_update_data->lif_id;
    }
    SHR_IF_ERR_EXIT(dnx_bfd_save_endpoint_to_sw_state_table(unit, endpoint_info, hw_update_data));

/** 
 *  End of first section: software tables - software state
 *  and templates
 */

/** From this point, writing to software tables is forbidden. */

/** Second section: writing to hardware tables   */

    /** Save the resulting endpoint to the databases   */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_to_hw(unit, hw_update_data));

    /** For accelerated endpoints */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_to_oamp_hw(unit, mep_hw_write_data, rmep_hw_write_data));

        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_delete_from_oamp_hw
                        (unit, mep_entry_values, rmep_entry_values, oamp_hw_delete_data));

         /** Write to OAMP MEP DB table */
        new_mep_entry_values->flags |= DNX_OAMP_BFD_MEP_UPDATE;
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_set(unit, endpoint_info, new_mep_entry_values));

         /** Write to OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_add(unit, endpoint_info->remote_id, FALSE, TRUE, new_rmep_entry_values));
    }

/** End of second section: writing to hardware tables */

exit:
    SHR_FREE(mep_hw_write_data);
    SHR_FREE(rmep_hw_write_data);
    SHR_FREE(mep_entry_values);
    SHR_FREE(rmep_entry_values);
    SHR_FREE(new_mep_entry_values);
    SHR_FREE(new_rmep_entry_values);
    SHR_FREE(existing_endpoint_info);
    SHR_FREE(oamp_hw_delete_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function gets the flags from sw_read_data
 * This function is a part of the bcm_dnx_bfd_endpoint_get 
 * API. 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] sw_state_flags - Flags that were read from
 *             SW state table.
 * \param [out] endpoint_info - Pointer to the structure to
 *        which the parameters will be written.
 *   
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_bfd_endpoint_endpoint_flags_from_sw_state_flags(
    int unit,
    uint32 sw_state_flags,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    uint32 return_flags = 0;

    /** Check flags if EP is using IPv6 */
    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_IS_IPV6))
    {
        return_flags |= BCM_BFD_ENDPOINT_IPV6;
    }

    /** Check flags if EP is multihop */
    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_MULTI_HOP_BFD))
    {
        return_flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }

    /** Check flags if EP is BFD Echo */
    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        return_flags |= BCM_BFD_ECHO;
    }

    /** Check flags if EP is accelerated */
    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        return_flags |= BCM_BFD_ENDPOINT_IN_HW;
    }

    /** Check flags if detection time is explicit */
    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_EXPLICIT_DETECTION_TIME))
    {
        return_flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
    }

    if (_SHR_IS_FLAG_SET(sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_MY_BFD_DIP_DESTINATION))
    {
        endpoint_info->flags2 = BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION;
    }

    endpoint_info->flags = return_flags;
}

/**
 * \brief - This function creates a default BFD endpoint.
 * This function is a part of the bcm_dnx_bfd_endpoint_create 
 * API. 
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_endpoint_create_default(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Write default parameters to endpoint structure */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_default_fill(unit, endpoint_info, hw_write_data));

    /** Save the resulting endpoint to the databases   */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_to_hw(unit, hw_write_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates a non-default BFD endpoint.
 * This function is a part of the bcm_dnx_bfd_endpoint_create 
 * API. 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 * \param [out] hw_write_data - Pointer to struct where data 
 *        for writing to HW tables should be placed (see struct
 *        definition for reason.)
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
dnx_bfd_endpoint_create_non_default(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    bfd_temp_oamp_mep_db_data_t *mep_hw_write_data = NULL;
    bfd_temp_oamp_rmep_db_data_t *rmep_hw_write_data = NULL;
    dnx_bfd_oamp_endpoint_t *mep_entry_values = NULL;
    dnx_oam_oamp_rmep_db_entry_t *rmep_entry_values = NULL;
    dnx_oam_remote_endpoint_info_t rmep_info;
    int max_bfd_period_value;
    uint16 rmep_index_const;
    SHR_FUNC_INIT_VARS(unit);
    hw_write_data->gathered_data.dip_write = FALSE;

/** 
 *  First section: software tables - software state
 *  and templates
 */

    /** Set correct flags to hw_write_data, based on endpoint_info flags */
    hw_write_data->bfd_sw_state_flags = dnx_bfd_endpoint_flags_to_sw_state_flags(unit, endpoint_info);

    /** Adjust discriminator range if necessary */
    if ((endpoint_info->local_discr != 0) && BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_discriminator_range_update(unit, endpoint_info, hw_write_data));
    }

    /** BFD Echo doesn't use OAM Classifier */
    if (!_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        /** Connect to a new/existing MEP profile */
        SHR_IF_ERR_EXIT(dnx_bfd_mp_profile_sw_update(unit, endpoint_info, hw_write_data));
    }

    /** Connect to a new/existing DIP entry */
    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_MULTI_HOP_BFD) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_dip_sw_update(unit, endpoint_info, hw_write_data));
    }

    /** Calculate the BFD endpoint ID */
    SHR_IF_ERR_EXIT(dnx_bfd_build_endpoint_id(unit, endpoint_info, hw_write_data));

    /** Now that endpoint ID is known, bfd_period can be compared to max value */

    /**
     * Sampling period is quantified by a 10 bit value.  The longest period supported
     * corresponds to the maximum 10-bit value
     */
    max_bfd_period_value = dnx_bfd_get_bfd_period(unit, endpoint_info->id, SAL_UPTO_BIT(10), 0);
    if (endpoint_info->bfd_period > max_bfd_period_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: bfd_period %d too high to support, maximum value is %d",
                     endpoint_info->bfd_period, max_bfd_period_value);
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        SHR_ALLOC_SET_ZERO(mep_hw_write_data, sizeof(bfd_temp_oamp_mep_db_data_t),
                           "Data saved to write to OAMP MEP DB table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_hw_write_data, sizeof(bfd_temp_oamp_rmep_db_data_t),
                           "Data saved to write to OAMP RMEP DB table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(mep_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure used to call OAMP MEP DB create", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_entry_values, sizeof(dnx_oam_oamp_rmep_db_entry_t),
                           "Structure used to call OAMP RMEP DB create", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        mep_hw_write_data->endpoint_id = endpoint_info->id;
        rmep_hw_write_data->endpoint_id = endpoint_info->id;

        /** Templates for endpoint accelerated to local OAMP   */
        SHR_IF_ERR_EXIT(dnx_bfd_write_oamp_data_to_templates
                        (unit, endpoint_info, mep_hw_write_data, rmep_hw_write_data));

        /** Build structure for creating OAMP MEP DB entry */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_build_param_for_oamp_mep_db
                        (unit, endpoint_info, mep_hw_write_data, mep_entry_values));

        /** Build structure for creating OAMP RMEP DB entry */
        dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(unit, endpoint_info, rmep_hw_write_data, rmep_entry_values);

        /** Save the RMEP index to the SW state table */
        rmep_info.oam_id = endpoint_info->id;
        rmep_info.rmep_id = DNX_BFD_RMEP_SW_INFO_NAME_CONST;
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_remote_endpoint_info_set(unit, endpoint_info->remote_id, &rmep_info));
    }

    /** Save the endpoint to the BFD SW state table   */
    SHR_IF_ERR_EXIT(dnx_bfd_save_endpoint_to_sw_state_table(unit, endpoint_info, hw_write_data));

/** 
 *  End of first section: software tables - software state
 *  and templates
 */

/** From this point, writing to software tables is forbidden. */

/** Second section: writing to hardware tables   */

    /** BFD Echo doesn't use OAM Classifier */
    if (!_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        /** Save the resulting endpoint to the databases */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_add(unit, endpoint_info, hw_write_data));
    }

    /** Write to the HW tables   */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_to_hw(unit, hw_write_data));

    /** Write to the HW tables that begin with "OAMP_"   */
    if (_SHR_IS_FLAG_SET(hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_to_oamp_hw(unit, mep_hw_write_data, rmep_hw_write_data));

        /** Write to OAMP MEP DB table */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_set(unit, endpoint_info, mep_entry_values));

        /** Write to OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_add(unit, endpoint_info->remote_id, FALSE, FALSE, rmep_entry_values));

        /** Get the constant for the RMEP index tables */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_get_const_for_oamp_rmep_index_db
                        (unit, mep_entry_values->mep_type, &rmep_index_const));

        /** Write to RMEP index DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_index_db_add
                        (unit, endpoint_info->id, rmep_index_const, endpoint_info->remote_id));
    }

/** End of second section: writing to hardware tables */
exit:
    SHR_FREE(mep_hw_write_data);
    SHR_FREE(rmep_hw_write_data);
    SHR_FREE(mep_entry_values);
    SHR_FREE(rmep_entry_values);
    SHR_FUNC_EXIT;
}

/*
 * Argument verification for bcm_dnx_bfd_endpoint_create
 */
static shr_error_e
dnx_bfd_endpoint_create_verify(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    int index, max_bfd_period_value;
    uint8 *field_val = NULL, *zero_array = NULL, cmp_len;
    uint8 *info_byte_array;
    bcm_ip6_t blank_ip6;
    uint8 is_bfd_echo;
    uint32 max_fec_id, max_lif_id, eg_if_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(endpoint_info, _SHR_E_PARAM, "endpoint_info");

    /** Allocate array for fields verified to be 0   */
    SHR_ALLOC(field_val, MAX_ARG_FIELD_SIZE, "Fields are copied here for verification",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Allocate zero array to compare to those fields   */
    SHR_ALLOC_SET_ZERO(zero_array, MAX_ARG_FIELD_SIZE, "Zero array for comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** 
     *  For convenience, the struct is treated as an array of
     *  uint8
     */
    info_byte_array = (uint8 *) endpoint_info;

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IPV6))
    {
        if (endpoint_info->ipv6_extra_data_index == 0 && _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW)
            && endpoint_info->id < dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: for self contained accelerated BFD endpoint over IPv6, ipv6_extra_data_index must be set to a non zero value.\n");
        }
        if (endpoint_info->src_ip_addr != 0 || endpoint_info->dst_ip_addr != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for BFD endpoint over IPv6, IPv4 addresses must be set to zero.\n");
        }
    }
    else
    {
        bcm_ip6_t ip = { 0 };
        if (sal_memcmp(endpoint_info->src_ip6_addr, ip, sizeof(ip)) != 0
            || sal_memcmp(endpoint_info->dst_ip6_addr, ip, sizeof(ip)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for BFD endpoint over IPv4, IPv6 addresses must be set to zero.\n");
        }
    }

    /** For non accelerated endpoints */
    if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        if (CHECK_FLAGS_INVERTED_MASK(endpoint_info->flags, BFD_SUPPORTED_FLAGS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: some flags are not supported for NON ACCELERATED endpoint;\n"
                         "the value 0x%08X has bits that fall outside"
                         "the supported flags mask, 0x%08X\n", endpoint_info->flags, BFD_SUPPORTED_FLAGS);
        }
    }
    else
    {
        /** For accelerated endpoints */
        if (CHECK_FLAGS_INVERTED_MASK(endpoint_info->flags, BFD_ACC_FLAGS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: some flags are not supported; the value 0x%08X has bits that fall outside"
                         "the supported flags mask, 0x%08X\n", endpoint_info->flags, BFD_ACC_FLAGS);
        }
    }

    if (dnx_find_in_uint8_array(supported_bfd_mep_types, endpoint_info->type, sizeof(supported_bfd_mep_types)) == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: type %d is not supported in this device", endpoint_info->type);
    }

    /** Check that all unsupported parameters are 0   */
    for (index = 0; index < BFD_NOF_JER2_UNSUP_PARAMS; index++)
    {
        sal_memset(field_val, 0, MAX_ARG_FIELD_SIZE);
        cmp_len = unsup_params_tbl[index].field_size;
        sal_memcpy(field_val, &info_byte_array[unsup_params_tbl[index].field_offset], cmp_len);
        if (sal_memcmp(field_val, zero_array, cmp_len) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: field %s is not supported in this device.  Value must be 0",
                         unsup_params_tbl[index].field_name);
        }
    }

    if (CHECK_FLAGS_INVERTED_MASK(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: some flags in flags2 are not supported; the value 0x%08X has bits that fall outside "
                     "the supported flags mask, 0x%08X\n", endpoint_info->flags2,
                     BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION);
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: MY_DIP_DESTINATION flag is set, but type is not multi-hop");
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        is_bfd_echo = _SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO);

        /** accelerated endpoint */
        if (endpoint_info->udp_src_port < MIN_SPORT_VAL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: udp_src_port %d not valid for accelerated endpoints, minimum value is %d",
                         endpoint_info->udp_src_port, MIN_SPORT_VAL);
        }

        if (endpoint_info->bfd_period < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: illegal negative value for bfd_period");
        }

        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE))
        {
            /**
             * When updating an endpoint, we already know its ID, so we know
             * whether it's a short-entry or full-entry endpoint.
             */

            /**
             * Sampling period is quantified by a 10 bit value.  The longest period supported
             * corresponds to the maximum 10-bit value
             */
            max_bfd_period_value = dnx_bfd_get_bfd_period(unit, endpoint_info->id, SAL_UPTO_BIT(10), 0);
            if (endpoint_info->bfd_period > max_bfd_period_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: bfd_period %d too high to support, maximum value is %d",
                             endpoint_info->bfd_period, max_bfd_period_value);
            }
        }

        if (UTILEX_GET_BITS_RANGE(endpoint_info->int_pri, sizeof(int) - 1, NOF_INT_PRI_BITS) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: int_pri 0x%08X not valid, value must be between 0 and 0x%08X",
                         endpoint_info->int_pri, SAL_BIT(NOF_INT_PRI_BITS) - 1);
        }

        if (endpoint_info->type != bcmBFDTunnelTypeUdp)
        {
            if (endpoint_info->label > MAX_MPLS_LABEL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: label 0x%08X not valid, value must be between 0 and 0x%08X",
                             endpoint_info->label, MAX_MPLS_LABEL);
            }

            if (endpoint_info->egress_label.exp > MAX_EXP_VAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: egress_label.exp %d not valid, value must be between 0 and %d",
                             endpoint_info->egress_label.exp, MAX_EXP_VAL);
            }
        }
        else
        {
            if (endpoint_info->egress_label.exp != 0xFF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: egress_label.exp should be 0xFF for UDP endpoints");
            }
        }

        if (endpoint_info->sampling_ratio > MAX_SAMPLING_RATIO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: sampling_ratio %d not valid, value must be between 0 and %d",
                         endpoint_info->sampling_ratio, MAX_SAMPLING_RATIO);
        }

        if (endpoint_info->ip_subnet_length > MAX_SUBNET_LENGTH)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: ip_subnet_length %d not valid, value must be between 0 and %d",
                         endpoint_info->ip_subnet_length, MAX_SUBNET_LENGTH);
        }

        if ((endpoint_info->local_state > MAX_STATE) || (is_bfd_echo && (endpoint_info->local_state != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: local_state %d not valid, value must be between 0 and %d",
                         endpoint_info->local_state, MAX_STATE);
        }

        if ((endpoint_info->local_diag > MAX_DIAG) || (is_bfd_echo && (endpoint_info->local_diag != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: local_diag %d not valid, value must be between 0 and %d",
                         endpoint_info->local_diag, MAX_DIAG);
        }

        if (CHECK_FLAGS_INVERTED_MASK(endpoint_info->local_flags, LOCAL_FLAGS_BITS_MASK))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: local_flags 0x%02X not valid, set only bits masked by 0x%02X",
                         endpoint_info->local_flags, LOCAL_FLAGS_BITS_MASK);
        }

        if ((endpoint_info->remote_state > MAX_STATE) || (is_bfd_echo && (endpoint_info->remote_state != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: remote_state %d not valid, value must be between 0 and %d",
                         endpoint_info->remote_state, MAX_STATE);
        }

        if ((endpoint_info->remote_diag > MAX_DIAG) || (is_bfd_echo && (endpoint_info->remote_diag != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: remote_diag %d not valid, value must be between 0 and %d",
                         endpoint_info->remote_diag, MAX_DIAG);
        }

        if (CHECK_FLAGS_INVERTED_MASK(endpoint_info->remote_flags, LOCAL_FLAGS_BITS_MASK))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: remote_flags 0x%02X not valid, set only bits masked by 0x%02X",
                         endpoint_info->remote_flags, LOCAL_FLAGS_BITS_MASK);
        }

        eg_if_val = BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
        if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

            if (eg_if_val > max_fec_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: FEC ID 0x%08X not valid, value must be between 0 and 0x%08X",
                             eg_if_val, max_fec_id);
            }

            if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: egress interface is a FEC but tx_gport is valid");
            }
        }
        else
        {
            if (BCM_L3_ITF_TYPE_IS_LIF(endpoint_info->egress_if))
            {
                max_lif_id = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit) - 1;
                if (eg_if_val > max_lif_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: LIF ID 0x%08X not valid, value must be between 0 and 0x%08X",
                                 eg_if_val, max_lif_id);
                }
                if (endpoint_info->tx_gport == BCM_GPORT_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: egress interface is a LIF but tx_gport is not valid");
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: provided interface is not a valid FEC nor an out-LIF.\n");
            }
        }
        if (endpoint_info->loc_clear_threshold > MAX_LOC_CLEAR_THRESH)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: loc_clear_threshold %d not valid, value must be between 0 and %d",
                         endpoint_info->loc_clear_threshold, MAX_LOC_CLEAR_THRESH);
        }

        if (endpoint_info->type == bcmBFDTunnelTypeUdp &&
                !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_MULTIHOP) &&
                !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ECHO) &&
                endpoint_info->ip_ttl != 255) {
                       SHR_ERR_EXIT(_SHR_E_PARAM,
                    "Error: for single-hop or micro-bfd endpoints the ip-ttl must be set to 255.");
        }

        if (endpoint_info->type == bcmBFDTunnelTypeMpls &&
                endpoint_info->ip_ttl != 1) {
                       SHR_ERR_EXIT(_SHR_E_PARAM,
                    "Error: for mpls endpoints the ip-ttl must be set to 1.");
        }
    }
    else
    {
        /** Non-Accelerated BFD endpoints require valid trap */
        if (!BCM_GPORT_IS_TRAP(endpoint_info->remote_gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: value for remote gport, %d, is not a valid trap",
                         endpoint_info->remote_gport);
        }

        /** Non-accelerated endpoint */
        for (index = 0; index < BFD_NOF_JER2_ACC_ONLY_PARAMS; index++)
        {
            sal_memset(field_val, 0, MAX_ARG_FIELD_SIZE);
            cmp_len = acc_only_params_tbl[index].field_size;
            sal_memcpy(field_val, &info_byte_array[acc_only_params_tbl[index].field_offset], cmp_len);
            if (sal_memcmp(field_val, zero_array, cmp_len) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for non-accelerated endpoints, field %s must have a value of 0",
                             acc_only_params_tbl[index].field_name);
            }
        }
        if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: for non-accelerated endpoints, field tx_gport must have a value of BCM_GPORT_INVALID");
        }

        if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION))
        {
            /** For non-accelerated endpoints, when this feature is used, no need for SIP */
            if (endpoint_info->src_ip_addr != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: for non-accelerated endpoints, when using my-BFD-DIP destination, SIP should be 0");
            }

            sal_memset(blank_ip6, 0, sizeof(bcm_ip6_t));
            if (!sal_memcmp(endpoint_info->src_ip6_addr, blank_ip6, sizeof(bcm_ip6_t)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: for non-accelerated endpoints, when using my-BFD-DIP destination, the IPv6 SIP should be all 0s");
            }
        }
    }

exit:
    SHR_FREE(field_val);
    SHR_FREE(zero_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create or modify a BFD endpoint 
 * Creating a new BFD endpoint entails adding a new entry to the
 * OAM_LIF_DB table or OAM_ACC_MEP_DB table, and referencing an 
 * entry in the OAMA table or the OAMB table, and also an entry 
 * in the BFD DIP table for multicast, or if necessary creating 
 * such entries. 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for 
 *        description of fields
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Currently only non-accelerated endpoints are supported,
 *     because the OAMP design is not complete.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_bfd_endpoint_create(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    
    uint8 default_bfd_mep = FALSE;
    /******************************************************/

    bfd_temp_endpoint_data_t *hw_write_data = NULL;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(unit, endpoint_info->id);

    SHR_ALLOC_SET_ZERO(hw_write_data, sizeof(bfd_temp_endpoint_data_t), "Data saved to write to hard-logic tables",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_bfd_endpoint_create_verify(unit, endpoint_info));

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_update(unit, endpoint_info, hw_write_data));
    }
    else if (default_bfd_mep == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_create_default(unit, endpoint_info, hw_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_create_non_default(unit, endpoint_info, hw_write_data));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the relevant Destination IP from 
 *        a template.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - Pointer to the structure to 
 *        which the parameters will be written.
 * \param [in] sw_read_data - Pointer to temporary structure 
 *        that now contains data needed for lookup.
 *   
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * endpoint_info was verified before this function was
 *     called.  sw_read_data was allocated using SHR_ALLOC.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_get_dip_from_template(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * sw_read_data)
{
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_dip.profile_data_get
                    (unit, _SHR_CORE_ALL, sw_read_data->gathered_data.dip_write_index, &dummy,
                     sw_read_data->gathered_data.dip_ipv6_address));

    if (_SHR_IS_FLAG_SET(sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_IS_IPV6))
    {
        /** IPv6 address */
        sal_memcpy(endpoint_info->src_ip6_addr, sw_read_data->gathered_data.dip_ipv6_address,
                   sizeof(endpoint_info->src_ip6_addr));
    }
    else
    {
        /** IPv4 address */
        BFD_EXTRACT_IP4V_ADDRS_FROM_DEFAULT_IPV6_EXTENSION_ADDRESS(sw_read_data->gathered_data.dip_ipv6_address,
                                                                   endpoint_info->src_ip_addr);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function constructs the remote_gport argument 
 *        for the endpoint info structure using data from the
 *        relevant action template profile.  The remote_gport
 *        argument is supposed to have the action profile
 *        parameters embedded in its bits.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - Pointer to the structure to 
 *        which the parameters will be written.
 * \param [in] sw_read_data - Pointer to temporary structure 
 *        that now contains data needed for lookup.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * endpoint_info was verified before this function was
 *     called.  sw_read_data was allocated using SHR_ALLOC.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_get_mp_profile_from_template(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * sw_read_data)
{
    int dummy;
    dnx_bfd_mep_profile_t profile_data;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_OAMB_PROFILE))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamb_action.profile_data_get(unit, _SHR_CORE_ALL,
                                                                     sw_read_data->gathered_data.
                                                                     action_write_key.mp_profile, &dummy,
                                                                     &profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oama_action.profile_data_get(unit, _SHR_CORE_ALL,
                                                                     sw_read_data->gathered_data.
                                                                     action_write_key.mp_profile, &dummy,
                                                                     &profile_data));
    }

    BCM_GPORT_TRAP_SET(endpoint_info->remote_gport, profile_data.trap_code, profile_data.forwarding_stregth,
                       profile_data.snoop_strength);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Extract the value used for classifying BFD packets 
 *        for this endpoint from the endpoint ID and place it in
 *        the relevant field - local_discriminator or gport.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of BFD endpoint to retrieve
 * \param [out] endpoint_info - Pointer to the structure to 
 *        which the parameters will be written.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * endpoint_info was verified before this function was
 *     called.
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_get_classifying_value(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    int dummy;
    uint32 lif_id;
    dnx_bfd_discr_range_t range;
    SHR_FUNC_INIT_VARS(unit);

    lif_id = UTILEX_GET_BITS_RANGE(endpoint, DISCR_RANGE_BIT_SHIFT - 1, 0);
    if (ENDPOINT_CLASSIFIED_BY_DISCR(endpoint) || ENDPOINT_IS_ACC(endpoint))
    {
        /** The discriminator is relevant for all accelerated and UDP endpoints */
        endpoint_info->local_discr = lif_id;

        /** The upper bits should match the discriminator range   */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tyour_discr_range.profile_data_get(unit, _SHR_CORE_ALL, 0, &dummy, &range));
        endpoint_info->local_discr |= (range.range_min << DISCR_RANGE_BIT_SHIFT);
    }
    else
    {
        endpoint_info->gport = lif_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification for bcm_dnx_bfd_endpoint_get
 */
static shr_error_e
dnx_bfd_endpoint_get_verify(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(endpoint_info, _SHR_E_PARAM, "endpoint_info");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve parameters for a BFD endpoint 
 * The data is read from the templates used for reference 
 * counter, or from the hardware tables themselves. 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of BFD endpoint to retrieve
 * \param [out] endpoint_info - Pointer to the structure to 
 *        which the parameters will be written.
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
bcm_dnx_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    bfd_temp_endpoint_data_t *sw_read_data = NULL;
    bfd_temp_oamp_mep_db_data_t *mep_sw_read_data = NULL;
    bfd_temp_oamp_rmep_db_data_t *rmep_sw_read_data = NULL;
    dnx_bfd_oamp_endpoint_t *mep_entry_values = NULL;
    dnx_oam_oamp_rmep_db_entry_t *rmep_entry_values = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(sw_read_data, sizeof(bfd_temp_endpoint_data_t), "Data gathered from tables for searching other tables",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_bfd_endpoint_get_verify(unit, endpoint, endpoint_info));

    /** Find endpoint in SW state table and read data   */
    SHR_IF_ERR_EXIT(dnx_bfd_get_endpoint_from_sw_state_table(unit, endpoint, sw_read_data));

    /** If endpoint found in SW state table, it is valid   */
    endpoint_info->id = endpoint;
    endpoint_info->type = sw_read_data->endpoint_type;

    /** Get the flags set in sw_read_data and set them to endpoint_info   */
    dnx_bfd_endpoint_endpoint_flags_from_sw_state_flags(unit, sw_read_data->bfd_sw_state_flags, endpoint_info);

    if (!_SHR_IS_FLAG_SET
        (sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_MY_BFD_DIP_DESTINATION))
    {
        /** Read destination IP from template   */
        if (_SHR_IS_FLAG_SET(sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_MULTI_HOP_BFD))
        {
            SHR_IF_ERR_EXIT(dnx_bfd_get_dip_from_template(unit, endpoint_info, sw_read_data));
        }
    }

    /** Read action profile from template   */
    SHR_IF_ERR_EXIT(dnx_bfd_get_mp_profile_from_template(unit, endpoint_info, sw_read_data));

    /** Get classifying value - discriminator or LIF   */
    SHR_IF_ERR_EXIT(dnx_bfd_get_classifying_value(unit, endpoint, endpoint_info));

    /** Is the local discriminator 0? */
    if (_SHR_IS_FLAG_SET(sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_LOCAL_DISCR_IS_NULL))
    {
        endpoint_info->local_discr = 0;
    }

    /** If endpoint is accelerated, get more parameters */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_IN_HW))
    {
        if (!BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
        {
                /** Restore gport from ACC_MEP_DB key */
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, sw_read_data->lif_id,
                             &endpoint_info->gport));
        }

        SHR_ALLOC_SET_ZERO(mep_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure used to call OAMP MEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_entry_values, sizeof(dnx_oam_oamp_rmep_db_entry_t),
                           "Structure used to call OAMP RMEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(mep_sw_read_data, sizeof(bfd_temp_oamp_mep_db_data_t),
                           "Parameters extracted from data read from OAMP MEP DB table", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_sw_read_data, sizeof(bfd_temp_oamp_rmep_db_data_t),
                           "Parameters extracted from data read from OAMP RMEP DB table", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        /** Read from OAMP MEP DB table */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_get(unit, endpoint, mep_entry_values));

        sal_memcpy(endpoint_info->dst_ip6_addr, mep_entry_values->ipv6_dip, sizeof(bcm_ip6_t));
        sal_memcpy(endpoint_info->src_ip6_addr, mep_entry_values->ipv6_sip, sizeof(bcm_ip6_t));

        /** Read RMEP index from MDB index table */
        endpoint_info->remote_id = sw_read_data->rmep_index;

        /** Read from OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, endpoint_info->remote_id, FALSE, rmep_entry_values));

        /** Write OAMP MEP DB data to target and temporary structures */
        dnx_bfd_endpoint_db_analyze_param_from_oamp_mep_db(unit, endpoint_info, mep_sw_read_data, mep_entry_values);

        /** For BFD Echo your discriminator is irrelevant and should be 0 */
        if (_SHR_IS_FLAG_SET(sw_read_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
        {
            endpoint_info->remote_discr = 0;
        }

        /** Write OAMP RMEP DB data to target and temporary structures */
        dnx_bfd_endpoint_db_analyze_param_from_oamp_rmep_db(unit, endpoint_info, rmep_sw_read_data, rmep_entry_values);

        /** Read data from templates for endpoint accelerated to local OAMP   */
        SHR_IF_ERR_EXIT(dnx_bfd_read_oamp_data_from_templates
                        (unit, endpoint_info, mep_sw_read_data, rmep_sw_read_data));
    }

exit:
    SHR_FREE(sw_read_data);
    SHR_FREE(mep_entry_values);
    SHR_FREE(rmep_entry_values);
    SHR_FREE(mep_sw_read_data);
    SHR_FREE(rmep_sw_read_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete a BFD endpoint.
 * Everything connected to the endpoint is removed - the 
 * endpoint entry is removed from the LIF DB or the ACC MEP DB, 
 * and the reference counts for its DIP and profile are 
 * decremented (or they are completely removed if the count 
 * reaches 0.) 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint - ID of BFD endpoint to destroy
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
bcm_dnx_bfd_endpoint_destroy(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    bfd_temp_endpoint_data_t *hw_delete_data = NULL;
    bfd_temp_oamp_db_delete_data_t *oamp_hw_delete_data = NULL;
    dnx_bfd_oamp_endpoint_t *mep_entry_values = NULL;
    dnx_oam_oamp_rmep_db_entry_t *rmep_entry_values = NULL;
    uint16 rmep_index_const;
    uint8 dummy;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Allocate structures to use existing function   */
    SHR_ALLOC_SET_ZERO(hw_delete_data, sizeof(bfd_temp_endpoint_data_t),
                       "Data read from SW state table for indexes to delete", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Find endpoint in SW state table and read data   */
    SHR_IF_ERR_EXIT(dnx_bfd_get_endpoint_from_sw_state_table(unit, endpoint, hw_delete_data));

    /** 
     *  First section: software tables - software state
     *  and templates
     */

    /** Delete reference to discriminator range */
    if (!_SHR_IS_FLAG_SET
        (hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_LOCAL_DISCR_IS_NULL)
        && (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(hw_delete_data->endpoint_type)))
    {
        /** 
         *  We don't actually delete anything in hardware, but if the
         *  reference counter reaches 0, the next BFD endpoint added
         *  will determine the new discriminator range.
         */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tyour_discr_range.free_single(unit, _SHR_CORE_ALL, 0, &dummy));
    }

    /** BFD Echo doesn't use classifier */
    if (!_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        /** Delete reference to action profile */
        SHR_IF_ERR_EXIT(dnx_bfd_free_mp_profile_in_template(unit, hw_delete_data));
    }

    /** Delete reference to DIP */
    if (_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_MULTI_HOP_BFD) &&
        !_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags,
                          DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_USE_MY_BFD_DIP_DESTINATION))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_dip.free_single
                        (unit, _SHR_CORE_ALL, hw_delete_data->deleted_data.dip_delete_index,
                         &hw_delete_data->deleted_data.dip_delete));
    }

    /** Write index to struct to delete from SW state table   */
    hw_delete_data->endpoint_id = endpoint;

    /** Delete entry from SW state table   */
    SHR_IF_ERR_EXIT(dnx_bfd_remove_endpoint_from_sw_state_table(unit, hw_delete_data));

    /** If endpoint is accelerated, remove from relevant templates */
    if (_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        SHR_ALLOC_SET_ZERO(mep_entry_values, sizeof(dnx_bfd_oamp_endpoint_t),
                           "Structure used to call OAMP MEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(rmep_entry_values, sizeof(dnx_oam_oamp_rmep_db_entry_t),
                           "Structure used to call OAMP RMEP DB get", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(oamp_hw_delete_data, sizeof(bfd_temp_oamp_db_delete_data_t),
                           "Flags that indicated if HW entries should be deleted", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /** Read from OAMP MEP DB table */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_get(unit, endpoint, mep_entry_values));

        /** Read from OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, hw_delete_data->rmep_index, FALSE, rmep_entry_values));

        SHR_IF_ERR_EXIT(dnx_bfd_delete_oamp_data_from_templates
                        (unit, mep_entry_values, rmep_entry_values, oamp_hw_delete_data));

        SHR_IF_ERR_EXIT(dnx_oam_sw_state_remote_endpoint_info_delete(unit, hw_delete_data->rmep_index));

        /** Free MEP ID */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, endpoint));

        /** Free RMEP ID */
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_free(unit, hw_delete_data->rmep_index));
    }

    /**
     *  End of first section: software tables - software state
     *  and templates
     */

    /** From this point, writing to software tables is forbidden. */

    /** Second section: writing to hardware tables   */

    /** BFD Echo doesn't use classifier */
    if (!_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_ECHO))
    {
        /** Delete entry from OAM_LIF_DB or OAM_ACC_MEP_DB table   */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_delete(unit, hw_delete_data));
    }

    /** Delete entries from HW tables for last references   */
    SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_remove_from_hw(unit, hw_delete_data));

    if (_SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_IS_ACCELERATED))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_delete_from_oamp_hw
                        (unit, mep_entry_values, rmep_entry_values, oamp_hw_delete_data));

        /** Delete endpoint from OAMP MEP DB table */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_bfd_endpoint_clear
                        (unit, endpoint,
                         _SHR_IS_FLAG_SET(hw_delete_data->bfd_sw_state_flags,
                                          DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_IS_IPV6)));

        /** Delete remote endpoint from OAMP RMEP DB table */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_remove(unit, hw_delete_data->rmep_index, FALSE));

        /** Delete from RMEP index DB table */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_get_const_for_oamp_rmep_index_db
                        (unit, mep_entry_values->mep_type, &rmep_index_const));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_index_db_remove(unit, endpoint, rmep_index_const));
    }

    /** End of second section: writing to hardware tables */

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_delete_data);
    SHR_FREE(mep_entry_values);
    SHR_FREE(rmep_entry_values);
    SHR_FREE(oamp_hw_delete_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete all BFD endpoints. 
 * This function creates an iterator for the BFD endpoint SW 
 * state table, then uses bcm_dnx_bfd_endpoint_destroy on each 
 * endpoint in that table. 
 * 
 * \param [in] unit - Number of hardware unit used.
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
bcm_dnx_bfd_endpoint_destroy_all(
    int unit)
{
    uint32 entry_handle_id, endpoint_id;
    int is_end;
    uint32 *field_val;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id));

    /** Allocate array for reading key */
    field_val = NULL;
    SHR_ALLOC(field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS * sizeof(uint32),
              "Array for reading profile table entry key", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Create iterator   */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Read first entry   */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /** Loop over all entries in table   */
    while (!is_end)
    {
        /** Get endpoint ID from the entry key  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_BFD_ENDPOINT_ID, field_val));
        endpoint_id = field_val[0];

        /** Use the endpoint ID to destroy the endpoint   */
        SHR_IF_ERR_EXIT(bcm_dnx_bfd_endpoint_destroy(unit, endpoint_id));

        /** Read next entry   */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Enable registration of BFD events.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] event_types -
*   Enable registration of these event_types
*   \param [in] cb -
*   Call back for the bfd events.
*   \param [in] user_data -
*   Pointer to user data which will be returned when
*   callback is invoked
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - invalid event_type
*   shr_error_exists - event already registered
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * Warmboot is not supported. Application has to
*   * re-register events after warmboot.
* \see
*   * \ref bcm_dnx_bfd_event_register
*/

shr_error_e
bcm_dnx_bfd_event_register(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb,
    void *user_data)
{
    uint32 result;
    bcm_bfd_event_type_t event_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Event type not supported");
    }

    for (event_i = 0; event_i < bcmBFDEventCount; event_i++)
    {
        if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i))
        {
            if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", event_i);
            }
            _g_bfd_event_cb[unit][event_i] = cb;
            _g_bfd_event_ud[unit][event_i] = user_data;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Disable registration of BFD events.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] event_types -
*   Disable registration of these event_types
*   \param [in] cb -
*   Call back for the bfd events.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_event_unregister
*/
shr_error_e
bcm_dnx_bfd_event_unregister(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb)
{
    bcm_bfd_event_type_t event_i;

    SHR_FUNC_INIT_VARS(unit);

    for (event_i = 0; event_i < bcmBFDEventCount; event_i++)
    {
        if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i))
        {
            if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb))
            {
                /*
                 * A different calblack is registered for this event. Return error
                 */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "A different callback is registered for %d ", event_i);
            }
            _g_bfd_event_cb[unit][event_i] = NULL;
            _g_bfd_event_ud[unit][event_i] = NULL;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets the value of the my-BFD-DIP destination.
 *          This value can then be used to create BFD endpoints that use
 *          DIPs that will not be stored in the 16-entry hard logic table,
 *          thus allowing more than 16 different DIPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] destination - value to write to register.
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
dnx_bfd_my_dip_destination_set(
    int unit,
    uint32 destination)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_MY_DIP_DESTINATION, &entry_handle_id));

    /** Set value for field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_DIP_DESTINATION, INST_SINGLE, destination);

    /** Write value to table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the value of the my-BFD-DIP destination.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] destination - pointer to which result should be written.
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
dnx_bfd_my_dip_destination_get(
    int unit,
    uint32 *destination)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_MY_DIP_DESTINATION, &entry_handle_id));

    /** Set pointer for field reading */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MY_DIP_DESTINATION, INST_SINGLE, destination);

    /** Read value from table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
