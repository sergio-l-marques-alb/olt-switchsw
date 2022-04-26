/** \file init_pp_common.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _INIT_PP_COMMON_INCLUDED__
/*
 * {
 */
#define _INIT_PP_COMMON_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX families only!"
#endif

/** No Additional Headers */
#define ADDITIONAL_HEADERS_NONE                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE
/** The additional Headers are CW, ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_CW_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI_CW
/** The additional Header is ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_ESI_EVPN             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI
/** The additional Headers are CW, FL and ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_CW_FL_ESI_EVPN       DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFLOESI
/** The additional Header is FL and ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_FL_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FLOESI
/** The additional Headers are CW, EL and ELI for PWE, new RFC */
#define ADDITIONAL_HEADERS_CW_EL_ELI_PWE        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOELOELI
/** The additional Headers are CW, FL for PWE, old RFC */
#define ADDITIONAL_HEADERS_CW_FL_PWE            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFL
/** The additional Headers are EL, ELI for EVPN */
#define ADDITIONAL_HEADERS_EL_ELI               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ELOELI
/** The additional Header is CW for PWE */
#define ADDITIONAL_HEADERS_CW_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CW
/** The additional Header is FL for PWE */
#define ADDITIONAL_HEADERS_FL_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FL
/** The additional Headers are VXLAN-GPE and UDP */
#define ADDITIONAL_HEADERS_VXLAN_GPE_UDP        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE
/** The additional Headers are GENEVE and UDP */
#define ADDITIONAL_HEADERS_GENEVE_UDP           DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE
/** The additional Headers are VXLAN and UDP */
#define ADDITIONAL_HEADERS_VXLAN_UDP            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN
/** The additional Header is GRE4 */
#define ADDITIONAL_HEADERS_GRE4                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE4
/** The additional Header is GRE8 with LB-Key with TNI, or without TNI */
#define ADDITIONAL_HEADERS_WO_TNI_GRE8          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_KEY
/** The additional Header is GRE8 with LB-Key with TNI */
#define ADDITIONAL_HEADERS_TNI_GRE8             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V4
/** The additional Header is UDP */
#define ADDITIONAL_HEADERS_UDP                  DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP
/** The additional Header is GRE12 */
#define ADDITIONAL_HEADERS_GRE12                DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE12
/** The additional Header is GRE8 with LB-Key with TNIfrom entry over IPV6 */
#define ADDITIONAL_HEADERS_IPV6_TNI_GRE8        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V6
    /** The additional Header is GRE8 with LB-Key with TNIfrom entry over IPV6 */
#define ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN    DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_SN
    /** The additional headers are UDP, UDP and VXLAN */
#define ADDITIONAL_HEADERS_VXLAN_DOUBLE_UDP_ESP DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_DOUBLE_UDP_ESP

typedef struct
{
    /*
     * Key field
     */
    int layer_type_enum_val;
    /*
     * Table result fields
     */
    int is_ethernet;
    int is_arp;
    int is_ipv4;
    int is_ipv6;
    int is_mpls_term;
    int is_mpls_fwd;
    int is_oam;
    int is_udp;
    int is_bfd_single_hop;
    int is_bfd_multi_hop;
    int is_tcp;
    int is_icmp;
    int is_icmpv6;
    int is_8021_x_2;
    int is_igmp;
    int is_bier_mpls;
} per_layer_protocol_t;

/*
 * ECOLogic FFC structure
 * opcode: 0-get field from relative header (accoridng to index offset)
           1-get field from header (according to index of layer records)
           2-get field straight from the pbus
           3-take field from layer records according to index"
  index:   either an index of a layer in the packet header or
           an index offset to a layer in the header
*/
typedef struct ecol_ffc_s
{
    uint8 opcode;
    uint8 index;
    uint16 field_offset;
    uint8 field_size;
    uint8 key_offset;
} ecol_ffc_t;

/** build ecologic ffcs for 32b value, parameter is of type
 * ecol_ffc_t For ffc instruction 0 */
#define BUILD_ECOL_FFC_INSTRUCTION_32(ecol_ffc) ((ecol_ffc.opcode << 24) | (ecol_ffc.index << 21) \
                                  | (ecol_ffc.field_offset << 10) | (ecol_ffc.field_size << 5) \
                                  | (ecol_ffc.key_offset << 0))

/** build ecologic ffcs for 16b value, parameter is of type
 * ecol_ffc_t For ffc instruction 1/2 */
#define BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc) ((ecol_ffc.opcode << 22) | (ecol_ffc.index << 19) \
                                  | (ecol_ffc.field_offset << 8) | (ecol_ffc.field_size << 4) \
                                  | (ecol_ffc.key_offset << 0))

/** layer qualifier for SRV6 (SL!=0) Endpoint */
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET (0)
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET (1)
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET (2)

#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(qualifier_p, segment_left_is_zero) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET, segment_left_is_zero))
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(qualifier_p, segment_left_is_one) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET, segment_left_is_one))
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(qualifier_p, single_pass_termination) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET, single_pass_termination))

/** layer qualifier for IPV6 when SRV6 SL=0 (Egress node) */
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET (2)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE (5)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET (7)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE (1)

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(qualifier_p, _first_add_header_exists_) \
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE, \
                                _first_add_header_exists_))

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(qualifier_p, _first_add_header_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE, \
                                _first_add_header_))

#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(qualifier_p, _srv6_sl0_single_pass_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE, \
                                _srv6_sl0_single_pass_))

/**
 * \brief -
 *   Configure initial values for ESEM.
 *   It disables EM lookup in case of coming AC from ingress.
 *
 * \param [in] unit - Relevant unit.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_eedb_type_buster_enable_init(
    int unit);

/**
 * \brief -
 * Init EGRESS_EEDB_TYPE table.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e egress_eedb_forwarding_domain_vsd_enable_init(
    int unit);

/**
 * \brief -
 * Init EGRESS_EEDB_TYPE table.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e egress_eedb_svtag_indication_enable_init(
    int unit);

/**
 * \brief -
 *  Init INGRESS_PRT_INFO table.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_prt_profile_configuration_init(
    int unit);
/**
 * \brief - The function configures initial values for
 *        INGRESS_LBP_VLAN_EDITING
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   Error inidication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_lbp_vlan_editing_configuration_init(
    int unit);

/**
 * \brief
 * The function is used to configure KBR output and ingress parsing context on SDK init
 *   \param [in] unit       -  Relevant unit.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_ippe_parser_context_mapping(
    int unit);

/** \param [in] unit - Relevant unit.
 *
 * \return
 *   Error inidication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_ippb_l4_protocols_config(
    int unit);

/**
 * \brief - The function configures initial values for
 *        MAPPED_PP_PORT field
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_port_general_configuration_init(
    int unit);

/**
 * \brief -
 * Init INGRESS_DA_TYPE_MAP table.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e ingress_da_type_map_init(
    int unit);

/** \param [in] unit - Relevant unit.
 *
 * \return
 *   Error inidication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_stat_fec_ecmp_mapping_init(
    int unit);

/** \param [in] unit - Relevant unit.
 *
 * \return
 *   Error inidication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ire_packet_generator_init(
    int unit);

/**
 * \brief Enable udp next protocol when IPV4 is the next protocol
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_udp_next_protocol_enable_init(
    int unit);

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL
 * \param [in] unit - Relevant unit.
 * \param [in] layer_type_enum_val - The LAYER_TYPE
 * \param [in] is_ipv4 - IPv4 type
 * \param [in] is_ipv6 - IPv6 type
 * \param [in] is_mpls - MPLS type
 * \param [in] is_oam - OAM type
 * \param [in] is_tcp - TCP type
 * \param [in] is_udp - UDP type

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ipv4,
    int is_ipv6,
    int is_mpls,
    int is_oam,
    int is_tcp,
    int is_udp);

/**
 * \brief -
 *  Configure speculative learning enable per layer_protocol
 *  This configuration enable/disable the process learning information from mpls_port lif.
 *  unrelated to speculative parsing of the layer above MPLS stack
 *
 * \param [in] unit - Relevant unit.
 * \param [in] layer_entry - Relevant layer type index .
 * \param [in] enable - 0 or 1, for enable/disable.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_layer_protocol_allow_speculative_learning(
    int unit,
    int layer_entry,
    int enable);

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL
 * \param [in] unit - Relevant unit.
 * \param [in] layer_type_enum_val - The LAYER_TYPE
 * \param [in] is_ethernet - ETHERNET type
 * \param [in] is_arp - ARP type
 * \param [in] is_ipv4 - IPv4 type
 * \param [in] is_ipv6 - IPv6 type
 * \param [in] is_mpls_term - MPLS term type
 * \param [in] is_mpls_fwd - MPLS fwd type
 * \param [in] is_udp - UDP type
 * \param [in] is_bfd_single_hop - BFD Single hop type
 * \param [in] is_bfd_multi_hop - BFD Multi hop type
 * \param [in] is_icmpv6 - ICMPV6 type
 * \param [in] is_igmp - IGMP type
 * \param [in] is_8021_x_2 - 8021 X 2 type
 * \param [in] is_icmp - ICMP type
 * \param [in] is_bier_mpls - BIER_MPLS type

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ethernet,
    int is_arp,
    int is_ipv4,
    int is_ipv6,
    int is_mpls_term,
    int is_mpls_fwd,
    int is_udp,
    int is_bfd_single_hop,
    int is_bfd_multi_hop,
    int is_icmpv6,
    int is_igmp,
    int is_8021_x_2,
    int is_icmp,
    int is_bier_mpls);

/**
 * \brief -
 *   Init pph base size according to system headers mode
 * \param [in] unit - Relevant unit
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pph_base_size_init(
    int unit);

/**
 * \brief
 * init table enables termination of network headers.
 */
shr_error_e ingress_itpp_general_network_header_termination_init(
    int unit);

/**
 * \brief -
 *   Configure In-Lif null value egress stage
 *
 * \param [in] unit - Relevant unit.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_cfg_in_lif_null_value_init(
    int unit);

/**
 * \brief - The function configures initial values for
 *        EGRESS_INGRESS_TRAPPED_BY_FHEI table
 * \param [in] unit - Relevant unit.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_ingress_trapped_by_fhei_configure_init(
    int unit);

/**
 * \brief - Configure EGRESS_FORWARDING_ADDITIONAL_INFO.
 *          Configure IPMC fallback to bridge.
 * \param [in] unit - Relevant unit
 * \param [in] fwd_additional_info - Relevant forwarding additional info
 * \param [in] ipvx_fallback_to_bridge_enable - '1' means IPvX is multicast compatible with Fallback-to-Bridge
 *                                              '0' means IPvX is not IPvX multicast compatible with Fallback-to-Bridge
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_fallback_to_bridge_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 ipvx_fallback_to_bridge_enable);

/**
 * \brief -
 * Init forwarding domain assignment mode registers.
 * 2 registers need to be configured:
 * - forwarding domain assignment mode mapping table:
 * Map lif.forwarding_domain_assignment_mode and "direct extraction priority decoder result type" to forwarding domain
 * mask profile.
 * Note: The mapping is lif.forwarding_domain_assignment_mode to forwarding domain mask profile.
 *       We ignore the "direct extraction priority decoder result type" key.
 * - forwarding domain assignment profile configuration mapping table:
 * Map the forwarding mask profile to forwarding domain mask base (mask forwarding domain resulting from the lif) and
 *   direct extraction priority decoder result mask (mask direct ext pd result).
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e ingress_forwarding_domain_assignment_mode_init(
    int unit);

/**
 * \brief
     * Set default values for forwarding context mapping for same interface filter.
     * Same interface is enabled for ETHERNET and MPLS forwarding.
 * \param [in] unit - Unit
 * \return shr_error_e Standard error handling
 */
shr_error_e egress_mapping_forward_context_configure_init(
    int unit);

/**
 * \brief
    Set DNX_OUT_LIF_PROFILE_DEFAULT_PORT_PROFILE as default value for Port outlif profile for all egress pp ports
    This is required for ETPP outlif profile management per port

 * \param [in] unit - Relevant unit
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_port_outlif_profile_configuration_init(
    int unit);

/**
 * \brief -
 *  Init PEMLA_CFG_PARSER table.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_egress_parser_configuration_init(
    int unit);

/**
 * \brief set GRE next protocol when ETH is the next protocol
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ingress_gre_next_protocol_eth_init(
    int unit);

/**
 * \brief - The function configures initial values for
 *        EGRESS_ADDITIONAL_HEADERS_MAP_TABLE table
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_map_tables_configuration_init(
    int unit);

/**
 * \brief - Configure EXCLUDE_SOURCE_MAPPED for enabling Exclude Source filter
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_exclude_source_filter_configure_init(
    int unit);
#endif/*_INIT_PP_COMMON_INCLUDED__*/
