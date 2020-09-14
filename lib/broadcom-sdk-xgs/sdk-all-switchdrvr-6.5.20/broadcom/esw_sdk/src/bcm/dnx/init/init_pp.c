/** \file init_pp.c
 * $Id$
 *
 * PP initialization procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

 /*
  * Include files
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/compiler.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <src/bcm/dnx/init/init_pp.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <soc/sand/sand_aux_access.h>
#include <src/bcm/dnx/init/auto_generated/init_pp_aod_tables.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <src/bcm/dnx/multicast/multicast_pp.h>
#include <sal/core/boot.h>
#include <soc/register.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <bcm_int/dnx/switch/switch_em_false_hit.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * Forwarding Code definitions for ease of use
 */
    /** Forwarding code is Ethernet */
#define ETHERNET                    DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET
    /** Forwarding code is MPLS */
#define MPLS                        DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS
    /** Forwarding code is IPV4_UC */
#define IPV4_UC_R0                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0
#define IPV4_UC_R1                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1
    /** Forwarding code is IPV4_MC */
#define IPV4_MC_R0                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0
#define IPV4_MC_R1                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1
    /** Forwarding code is IPV6_UC */
#define IPV6_UC_R0                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0
#define IPV6_UC_R1                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1
    /** Forwarding code is IPV6_MC */
#define IPV6_MC_R0                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0
#define IPV6_MC_R1                  DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1
    /** Forwarding code is BIER_MPLS */
#define BIER_MPLS                   DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS
    /** Forwarding code is BIER_TI */
#define BIER_TI                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_TI
    /** Forwarding code is INGRESS_TRAPPED */
#define INGRESS_TRAPPED             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED_JR1_LEGACY
    /** Forwarding code is STACKING - Right*/
#define STACKING_RIGHT              DBAL_ENUM_FVAL_EGRESS_FWD_CODE_STACKING_PORT
    /** Forwarding code is STACKING - Left*/
#define STACKING_LEFT               DBAL_ENUM_FVAL_EGRESS_FWD_CODE_CPU_PORT
    /** Forwarding code is SRV6_SEGMENT_ENDPOINT */
#define SRV6_ENDPOINT               DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT
    /** Forwarding code is SRV6_USID_ENDPOINT */
#define SRV6_USID_ENDPOINT          DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_USID_ENDPOINT
    /** Forwarding code is SRV6_ENDPOINT_PSP */
#define SRV6_ENDPOINT_PSP           DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP
    /** Forwarding code is TM */
#define TM                          DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TM
    /** Forwarding code is RAW_PROCESSING */
#define RAW_PROCESSING              DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING
    /** Forwarding code is ERPP_TRAPPED */
#define ERPP_TRAPPED                DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ERPP_TRAPPED
    /** Forwarding code is MPLS_INJECTED_FROM_OAMP */
#define MPLS_INJECTED_FROM_OAMP     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS_INJECTED_FROM_OAMP
    /** Forwarding code is IPV4_DO_NOT_EDIT */
#define IPV4_DO_NOT_EDIT            DBAL_ENUM_FVAL_EGRESS_FWD_CODE_DO_NOT_EDIT
        /** Forwarding code is PPP */
#define PPP                         DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PPP
    /** Forwarding code is MIRROR_OR_SS */
#define MIRROR_OR_SS                DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MIRROR_OR_SS
    /** Forwarding code is BUG_CODE */
#define BUG_CODE                    DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE

    /** Stacking Right Code Port Profile */
#define STACKING_RIGHT_CODE_PORT_PROFILE        DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PORT_PROFILE_STACKING_PORT
    /** Stacking Left Code Port Profile */
#define STACKING_LEFT_CODE_PORT_PROFILE         DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PORT_PROFILE_CPU_PORT
    /** Stacking Left Code Port Profile */
#define RAW_PROCESSING_CODE_PORT_PROFILE         DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PORT_PROFILE_RAW_PROCESSING

    /** JR mode forward code override context */
#define ACE_CTXT_J_MODE_OVRRIDE                 DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE

    /** LIF EXTENSION TYPE NONE */
#define LIF_EXTENSION_TYPE_NONE                 DBAL_ENUM_FVAL_LIF_EXTENSION_TYPE_NONE
    /** LIF EXTENSION TYPE 1xOUT_LIF */
#define LIF_EXTENSION_TYPE_1XOUT_LIF            DBAL_ENUM_FVAL_LIF_EXTENSION_TYPE_1xOUT_LIF
    /** LIF EXTENSION TYPE 2xOUT_LIF */
#define LIF_EXTENSION_TYPE_2XOUT_LIF            DBAL_ENUM_FVAL_LIF_EXTENSION_TYPE_2xOUT_LIF
    /** LIF EXTENSION TYPE 3xOUT_LIF */
#define LIF_EXTENSION_TYPE_3XOUT_LIF            DBAL_ENUM_FVAL_LIF_EXTENSION_TYPE_3xOUT_LIF
    /** LIF EXTENSION TYPE IN_LIF_PLUS_IN_LIF_PROFILE */
#define LIF_EXTENSION_TYPE_INLIF_PROFILE        DBAL_ENUM_FVAL_LIF_EXTENSION_TYPE_IN_LIF_PLUS_IN_LIF_PROFILE
    /** LIF EXTENSION SIZE - 3 bytes */
#define LIF_EXTENSION_SIZE_1XOUT_LIF            3
    /** IN_LIF EXTENSION SIZE - 4 bytes */
#define LIF_EXTENSION_SIZE_IN_LIF_PROFILE       4

    /** Forwarding plus one header remarking ingress disabling profile */
#define FWD_PLUS_1_REMARK_INGRESS_DISABLE       DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_0
    /** Forwarding plus one header remarking ingress enabling profile */
#define FWD_PLUS_1_REMARK_INGRESS_ENABLE        DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_1

    /** J2_PP_GENERAL Generation Profile */
#define J2_PP_GENERAL                           DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_GENERAL
    /** J2_OAM Generation Profile */
#define J2_OAM                                  DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_JR2_OAM
    /** J2_TM Generation Profile */
#define J2_TM                                   DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_JR2_TM
    /** J_OAM Generation Profile */
#define J_OAM                                   DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_J_OAM
    /** J_TM Generation Profile */
#define J_TM                                    DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_J_TM
    /** TDM Generation Profile */
#define TDM                                     DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_TDM
    /** OTMH Generation Profile */
#define OTMH                                    DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_OTMH
    /** UDH Generation Profile */
#define UDH                                     DBAL_ENUM_FVAL_SYS_HDR_GENERATION_PROFILE_UDH

    /** Inlif Decision 0 is Forwarding Domain */
#define INLIF_DESCISION_0_FODO                  DBAL_ENUM_FVAL_INLIF_DECISION_0_FORWARDING_DOMAIN
    /** Inlif Decision 0 is Inlif */
#define INLIF_DESCISION_0_INLIF                 DBAL_ENUM_FVAL_INLIF_DECISION_0_INLIF
    /** Inlif Decision 1 is Inlif */
#define INLIF_DESCISION_1_INLIF                 DBAL_ENUM_FVAL_INLIF_DECISION_1_INLIF
    /** Inlif Decision 1 is Inlif Extension */
#define INLIF_DESCISION_1_INLIF_EXTENSION       DBAL_ENUM_FVAL_INLIF_DECISION_1_INLIF_EXTENSION
    /** System Headers Mode is Jericho1 */
#define SYSTEM_HEADERS_MODE_JERICHO_MODE        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE
    /** System Headers Mode is Jericho2 */
#define SYSTEM_HEADERS_MODE_JERICHO2_MODE       DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE

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

    /** MPLS Header length */
#define MPLS_BYTES_TO_ADD                       4
    /** VXLAN Header length */
#define VXLAN_BYTES_TO_ADD                      8
    /** GENEVE Header length */
#define GENEVE_BYTES_TO_ADD                     8
    /** GRE4 Header length */
#define GRE4_BYTES_TO_ADD                       4
    /** GRE8 Header length */
#define GRE8_BYTES_TO_ADD                       8
    /** GRE12 Header length */
#define GRE12_BYTES_TO_ADD                      12
    /** UDP Header length */
#define UDP_BYTES_TO_ADD                        8

/** Generates MPLS header_specific_information value:
 *   {Label-Type(2),Label(20),EXP-Type(1),EXP(3),TTL-Type(1),TTL(8)}*/
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET     33
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET          13
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET       12
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET             9
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET        8
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET             0
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(_protocol_specific_info_, _field_val_, _field_offset_)  \
    do {                                                                                                            \
      COMPILER_UINT64 field_val_64_bits;                                                                            \
      COMPILER_64_SET(field_val_64_bits, 0, _field_val_);                                                           \
      COMPILER_64_SHL(field_val_64_bits, _field_offset_);                                                           \
      COMPILER_64_OR(_protocol_specific_info_, field_val_64_bits);                                                  \
    } while (0);                                                                                                    \

    /** Control word encode*/
#define MPLS_CW_FLAG_BITS_WIDTH                        4
#define MPLS_CW_SPECIFIC_INFORMATION(val, seq_src) ((val<<1) | seq_src)
    /** VXLAN Protocol Specific Information*/
#define VXLAN_PROTOCOL_SPECIFIC_INFORMATION     8
    /** VXLAN GPE Protocol Specific Information*/
#define VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION 0xC
    /** GRE Protocol Specific Information*/
#define GRE_PROTOCOL_SPECIFIC_INFORMATION       0
    /** UDP Protocol Specific Information*/
#define UDP_PROTOCOL_SPECIFIC_INFORMATION       0
    /** VXLAN UDP Protocol Specific Information:
     *  destination_port_info: 0x1
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  destination_port: 4789
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set) source_port_info: 0x0
     *  (indicate the source port use load balancing key)
     *  source_port:      0x0000
     *  reserved: 0x0
     *   */
#define VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x256B)
    /** ESP UDP Protocol Specific Information:
     *  These values won't be updated by an API and will remain fixed.
     *  destination_port_info: 0x1
     *  destination_port:      500
     *  source_port_info:      0x1
     *  source_port:           500
     *  reserved: 0x0
     */
#define UDP_ESP_PROTOCOL_SPECIFIC_INFORMATION (0x7D203E9)
    /** VXLAN GPE UDP Protocol Specific Information:
     *  destination_port_info: 0x1
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set)
     *  destination_port: 4790
     *  (Note: this is default value, can be updated by API:
     *  bcm_Switch_control_set) source_port_info: 0x0 (indicate the
     *  source port use load balancing key) source_port:      0x0000
     *  reserved: 0x0
     *   */
#define VXLAN_GPE_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x256D)

    /** GENEVE Protocol Specific Information*/
#define GENEVE_PROTOCOL_SPECIFIC_INFORMATION    0
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_GRE               47
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_UDP               17
    /** Next Protocol is ESP*/
#define CURRENT_NEXT_PROTOCOL_ESP               50
    /** Next Protocol is GENEVE*/
#define CURRENT_NEXT_PROTOCOL_GENEVE            6081
    /** Next Protocol is VXLAN*/
#define CURRENT_NEXT_PROTOCOL_VXLAN             4087

/** next protocol value for Ethernet when next protocol namespace is ETHERTYPE */
#define ETHERTYPE_NEXT_PROTOCOL_ETHERNET       0x6558

#define ERPP_FILTER_PER_FWD_CONTEXT_SIZE        11
    /** Size of FTMH LB key in bytes by default */
#define FTMH_LB_KEY_EXT_SIZE 3
    /** Size of FTMH Stacking Ext in bytes by default */
#define STACKING_EXT_SIZE 0

#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_BOTH_ENABLED 2
#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ENABLED 3
#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED 1
#define SYSTEM_SCOPE_ENABLED_AND_ORIENTATION_ENABLED 3
#define DEVICE_SCOPE_ENABLED_AND_ORIENTATION_ENABLED 2

    /** Compensate CRC size for ERPP statistics */
#define ERPP_COMPENSATE_CRC_SIZE 2
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

/** layer qualifier for SRV6 ingress */
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET (0)
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET (1)
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET (2)

#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(qualifier_p, segment_left_is_zero) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET, segment_left_is_zero))
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(qualifier_p, segment_left_is_one) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET, segment_left_is_one))
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(qualifier_p, single_pass_termination) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET, single_pass_termination))

/** shorter names for field type enum:
 *  dbal_enum_value_field_forward_domain_assignment_mode_enum_e */
#define FODO_ASSIGNMENT_MODE_FROM_LIF    (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF)
#define FODO_ASSIGNMENT_MODE_FROM_VLAN   (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN)
#define FODO_ASSIGNMENT_MODE_FROM_LOOKUP (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP)

/** shorter names for field type enum:
 *  dbal_enum_value_field_fodo_pd_result_type_e */
#define PD_RESULT_TYPE_NOT_FOUND (DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_NOT_FOUND)
#define PD_RESULT_TYPE_FOUND     (DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND)

/** shorter names for field type enum:
 *  dbal_enum_value_field_forward_domain_mask_profile_e */
#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_0)
#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_12_FROM_PD_11_0)
#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_PD_17_0)

/**
 * Force bubble definition
 */
#define FORCE_BUBBLE_SYNC_COUNTER 1000

/**
 * Similar to FORCE_BUBBLE_SYNC_COUNTER, just for egress and in a factor of 32
 */
#define FORCE_EGRESS_BUBBLE_DELAY ((FORCE_BUBBLE_SYNC_COUNTER / 32) + 1)

typedef struct
{
    /*
     * Key fields - index to the register
     */
    int index;
    /*
     * Table result fields
     */
    int outlif_0_decision;
    int outlif_1_decision;
    int outlif_2_decision;
    int outlif_3_decision;
} outlif_decision_t;

typedef struct
{
    /*
     * Key fields - LIF Extension Type
     */
    dbal_enum_value_field_lif_extension_type_e lif_extension_type;
    /*
     * Table result fields
     */
    int outlif_extension_1;
    int outlif_extension_2;
    int outlif_extension_3;
    int inlif_extension_1;
    int inlif_extension_1_profile;
} outlif_extension_config_t;

typedef struct
{
    /*
     * Key fields - System Header Mode + MC FWD Code
     */
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;
    int is_mc_forward_code;
    /*
     * Table result fields
     */
    dbal_enum_value_field_inlif_decision_0_e inlif_decision_0;
    dbal_enum_value_field_inlif_decision_1_e inlif_decision_1;
} inlif_decision_config_t;

typedef struct
{
    /*
     * Key fields - LIF Additional Headers Profile + Context Additional Headers Profile
     */
    int lif_additional_headers_profile;
    dbal_enum_value_field_ctx_additional_header_profile_e context_additional_headers_profile;
    /*
     * Table result fields - Additional Headers Profile
     */
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile;
} additional_headers_profile_config_t;

typedef struct
{
    /*
     * Key field - System Header Generation Profile
     */
    int system_header_profile;
    /*
     * Table result fields
     */
    int bytes_to_add;
} system_header_profile_t;

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

typedef struct
{
    /*
     * Key field
     */
    dbal_enum_value_field_layer_types_e layer_type;
    /*
     * Table result fields
     */
    dbal_enum_value_field_erpp_1st_parser_parser_context_e parser_context;
} layer_type_to_mpls_upper_layer_protocol_t;

typedef struct
{
    /*
     * Key fields Current Protocol Namespace and Current Protocol Type
     */
    dbal_enum_value_field_current_protocol_type_e current_protocol_type;
    dbal_enum_value_field_etpp_next_protocol_namespace_e current_protocol_namespace;
    /*
     * Table result field - Current Next Protocol
     */
    int current_next_protocol;
} current_next_protocol_config_table_t;

typedef struct
{
    /*
     * Key fields - Additional Headers Profile
     */
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_headers_profile;
    /*
     * Table result fields
     */
    int start_current_next_protocol_enable_main_header;
    int start_current_next_protocol_main_header;
    int additional_headers_packet_size_enable_main_header;
    int additional_headers_packet_size_main_header;
    int start_current_next_protocol_enable_add_header_0;
    int start_current_next_protocol_add_header_0;
    int additional_headers_packet_size_enable_add_header_0;
    int additional_headers_packet_size_add_header_0;
    COMPILER_UINT64 protocol_specific_information_add_header_0;
    dbal_fields_e protocol_specific_information_add_header_0_type;
    int start_current_next_protocol_enable_add_header_1;
    int start_current_next_protocol_add_header_1;
    int additional_headers_packet_size_enable_add_header_1;
    int additional_headers_packet_size_add_header_1;
    COMPILER_UINT64 protocol_specific_information_add_header_1;
    dbal_fields_e protocol_specific_information_add_header_1_type;
    int start_current_next_protocol_enable_add_header_2;
    int start_current_next_protocol_add_header_2;
    int additional_headers_packet_size_enable_add_header_2;
    int additional_headers_packet_size_add_header_2;
    COMPILER_UINT64 protocol_specific_information_add_header_2;
    dbal_fields_e protocol_specific_information_add_header_2_type;
} additional_headers_map_t;

typedef struct
{
    dbal_enum_value_field_irpp_2nd_parser_parser_context_e parser_ctx;
    dbal_enum_value_field_layer_types_e layer_type_speculation;
    dbal_enum_value_field_layer_types_e layer_type_force_incorrect;
} per_2nd_stage_parsing_ctx_t;

typedef struct
{
    /**
     * Key field - Forwarding additional information (FAI)
     */
    uint32 forwarding_additional_info;
    /**
     * Table result fields
     */
    uint8 ipvx_fallback_to_bridge_enable;
    uint8 bum_traffic_enable;
    uint8 fwd_plus_1_remark_ingress_profile;
} forwarding_additional_info_config_t;

typedef struct
{
    /*
     * Key fields - fwd_layer_type + fwd_layer_qual_lsb
     */
    dbal_enum_value_field_layer_types_e fwd_layer_type;
    int fwd_layer_qual_lsb;
    /*
     * Table result fields
     */
    dbal_enum_value_field_jr_fwd_code_e jr_fwd_code;
} jr_mode_fwd_code_mapping_config_t;

typedef struct
{
    /*
     * Key fields - jr_fwd_code + is_mc + outlif_is_rif
     */
    dbal_enum_value_field_jr_fwd_code_e jr_fwd_code;
    int is_mc;
    int outlif_is_rif;
    /*
     * Table result fields
     */
    dbal_enum_value_field_jr_vsi_src_e jr_vsi_src;
} jr_mode_vsi_src_mapping_config_t;

typedef struct
{
    /*
     * Key fields - jr_fwd_code + is_mc + outlif_is_rif
     */
    dbal_enum_value_field_jr_fwd_code_e jr_fwd_code;
    int inlif_eq_zero;
    int fwd_layer_idx;
    /*
     * Table result fields
     */
    dbal_enum_value_field_fhei_size_e fhei_size;
} jr_mode_fhei_size_mapping_config_t;

/*ECOLogic FFC structure*/
/**opcode: 0-get field from relative header (accoridng to index offset)
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

/* datas to fill DBAL_TABLE_FODO_ASSIGNMENT_MODE_MAPPING:
   fodo assignment mode x pd result type  -> fodo mask profile  */
typedef struct fodo_assignment_mode_mapping_s
{
    uint32 fodo_assignment_mode;
    uint32 pd_result_type;
    uint32 fodo_mask_profile;
} fodo_assignment_mode_mapping_t;

/* datas to fill DBAL_TABLE_FODO_ASSIGNMENT_PROFILE:
   fodo mask profile -> fodo mask, pd result mask
   */
typedef struct fodo_assignment_mask_profile_s
{
    uint32 fodo_mask_profile;
    uint8 fodo_is_masked;
    uint8 pd_result_is_fully_masked;
    uint8 pd_result_is_vlan_masked;
} fodo_assignment_mask_profile_t;

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
  * Initialization table for outlif_decision_map, which configures DBAL_TABLE_EGRESS_SELECTED_GLOBAL_OUTLIFS Register.
  * Each line configures 7 bits from the Register according to the input index.
  * Index 0 configures DBAL_TABLE_EGRESS_SELECTED_GLOBAL_OUTLIFS [6:0].
  */

static const outlif_decision_t outlif_decision_map_jr2_mode[] = {
  /** Register Index, OutLif_0_Decision, OutLif_1_Decision, OutLif_2_Decision, OutLif_3_Decision*/
    {0, 1, 2, 2, 2},
    {1, 1, 3, 2, 2},
    {2, 0, 0, 0, 0},
    {3, 1, 3, 3, 2},
    {4, 0, 0, 0, 0},
    {5, 0, 0, 0, 0},
    {6, 0, 0, 0, 0},
    {7, 1, 3, 3, 3}
};

static const outlif_decision_t outlif_decision_map_jer_mode[] = {
  /** Register Index, OutLif_0_Decision, OutLif_1_Decision, OutLif_2_Decision, OutLif_3_Decision*/
    {0, 1, 3, 3, 3},
    {1, 1, 1, 1, 1},
    {4, 1, 3, 3, 3},
    {5, 1, 1, 1, 1},
    {6, 0, 0, 1, 1},
    {7, 0, 0, 0, 0}
};

 /*
  * Initialization table for outlif_extension_map, which configures DBAL_TABLE_EGRESS_SYSTEM_HEADERS_LIF_EXTENSION Table.
  * Each line configures the outlif extension.
  * Index 0 Configures line 0 - outlif 0 and line 1 configures outlif 1 etc.
  */
static const outlif_extension_config_t outlif_extension_map[] = {
 /** LIF Ext Type,                      OutLif_1_Extension, OutLif_2_Extension, OutLif_3_Extension, InLif_1_Extension, InLif_1_Extension_Profile*/
    {LIF_EXTENSION_TYPE_NONE, 0, 0, 0, 0, 0},
    {LIF_EXTENSION_TYPE_1XOUT_LIF, 1622, 0, 0, 0, 0},
    {LIF_EXTENSION_TYPE_2XOUT_LIF, 1622, 918, 0, 0, 0},
    {LIF_EXTENSION_TYPE_3XOUT_LIF, 1622, 918, 214, 0, 0},
    {LIF_EXTENSION_TYPE_INLIF_PROFILE, 0, 0, 0, 1366, 1064}
};

/*
  * Initialization table for inlif_decision_config_map_jer_mode, which configures EGRESS_GLOBAL_INLIF_RESOLUTION Table.
  * Each line configures Inlif Decision 0 and Inlif Decision according to Jericho System Headers Mode and the MC Fwd Code.
  */

static const inlif_decision_config_t inlif_decision_config_map_jer_mode[] = {
/** System Headers Mode,               MC Forward Code, Inlif Decision 0 FoDo,   Inlif Decision 1 InLif*/
    {SYSTEM_HEADERS_MODE_JERICHO_MODE, 1, INLIF_DESCISION_0_FODO, INLIF_DESCISION_1_INLIF},
    {SYSTEM_HEADERS_MODE_JERICHO_MODE, 0, INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION}
};
/*
 * Initialization table for inlif_decision_config_map_jr2_mode, which configures EGRESS_GLOBAL_INLIF_RESOLUTION Table.
 * Each line configures Inlif Decision 0 and Inlif Decision according to Jericho2 System Headers Mode and the MC Fwd Code.
 */
static const inlif_decision_config_t inlif_decision_config_map_jr2_mode[] = {
 /** System Headers Mode,               MC Forward Code, Inlif Decision 0,    Inlif Decision 1 InLif Extension*/
    {SYSTEM_HEADERS_MODE_JERICHO2_MODE, 1, INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION},
    {SYSTEM_HEADERS_MODE_JERICHO2_MODE, 0, INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION}
};

/*
  * Initialization table for current_next_protocol_config_map, which configures DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE Table.
  * Each line configures the Current next protocol, based on the Current Protocol Type and the Current Protocol Namespace.
  */
static const current_next_protocol_config_table_t current_next_protocol_config_map[] = {
/**  Current Protocol Type,                              Next Protocol Namespace,                                             Current Next Protocol*/
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT,
     DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FCOE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_FCOE_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PPPOE_SESSION, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_PPPOE_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV2, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ERSPAN_OVER_GRE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV3, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ERSPAN_OVER_GRE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET_OVER_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UDP, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_IP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_BOS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN_GPE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_GPE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GENEVE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_UDP},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT,
     DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_SRV6_OVER_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_VXLAN},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_PPPOE},
    {DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GTP, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,
     DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GTPU_OVER_UDP}

};
 /*
  * Initialization table for additional_headers_profile_config_map, which configures DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP Table.
  * Each line configures the Additional Headers Profile, based on the Context Additional Header Profile and LIF Additional Header Profile
  */

static const additional_headers_profile_config_t additional_headers_profile_config_map[] = {
   /** LIF Additional Headers Profile,                                                                                     Context Additional Headers Profile,                                         Additional Headers Profile*/
    {DBAL_ENUM_FVAL_LIF_AHP_GENERAL_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE,
     ADDITIONAL_HEADERS_NONE},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_NONE},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_ESI_EVPN},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_CW_ESI_EVPN},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_FL_ESI_EVPN},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI,
     ADDITIONAL_HEADERS_CW_FL_ESI_EVPN},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_EL_ELI_PWE},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_FL_PWE},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_EL_ELI},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_CW_PWE},
    {DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,
     ADDITIONAL_HEADERS_FL_PWE},
    {DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN_GPE,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, ADDITIONAL_HEADERS_VXLAN_GPE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, ADDITIONAL_HEADERS_VXLAN_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GENEVE,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_GENEVE_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,
     ADDITIONAL_HEADERS_GRE4},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_WO_TNI_GRE8},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_TNI_GRE8},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE12,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_GRE12},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_IPV6_TNI_GRE8},
    {DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN,
     DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_1, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(1)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_2, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(2)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_3, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(3)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(4)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_5, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(5)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_6, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(6)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_7, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     ADDITIONAL_HEADERS_UDP_USER_DEFINED(7)},
    {DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_ESP, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,
     DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP},
    {DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_NONE, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP,
     ADDITIONAL_HEADERS_NONE},
    {DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_IP_ESP, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP,
     DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP}
};

 /*
  * Initialization table for system_header_profile_map, which configures DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE Table.
  * Each line configures the specific system headers size (bytes to add), respectively to the generation profile.
  * Index 0 Configures line 0 - the size of system headers for the profile.
  */

static const system_header_profile_t system_header_profile_map[] = {
  /** Generation Profile, Bytes to add*/
    {J2_PP_GENERAL, 40},
    {J2_OAM, 35},
    {J2_TM, 20},
    {J_OAM, 30},
    {J_TM, 25},
    {TDM, 18},
    {OTMH, 17},
    {UDH, 0}
};

 /*
  * Initialization table for per_layer_protocol, which configures INGRESS_PER_LAYER_PROTOCOL dbal table.
  */
static const per_layer_protocol_t per_layer_protocol[] = {
    /**            Layer Type,                   ETHERNET, ARP,   IPV4,  IPV6,  MPLS_TERM,  MPLS_FWD,  OAM,   UDP ,  BFD_SINGLE_HOP, BFD_MULTI_HOP, TCP,   ICMP,  ICMPV6, 8021_x_2, IGMP   BIER_MPLS*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ARP, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_TCP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_UDP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PTP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ICMP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
     TRUE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, TRUE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_RCH, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IGMP, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, TRUE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE,
     FALSE, FALSE, FALSE, FALSE},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE,
     FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}
};

static const per_2nd_stage_parsing_ctx_t per_2nd_stage_parsing_ctx[] = {
    /*
     * parser conext layer_type_before layer_type_after
     */
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NONE, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,
     DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,
     DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPVX_DEMUX_B1, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,
     DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_SPECULATIVEOAMB1, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,
     DBAL_ENUM_FVAL_LAYER_TYPES_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV4_NOETHB2, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,
     DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV6_NOETHB2, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,
     DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS64_B1, DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,
     DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS128_B1, DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,
     DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS256_B1, DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,
     DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERTI_B1, DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI,
     DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_B1, DBAL_ENUM_FVAL_LAYER_TYPES_TCP,
     DBAL_ENUM_FVAL_LAYER_TYPES_TCP},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_B1, DBAL_ENUM_FVAL_LAYER_TYPES_UDP,
     DBAL_ENUM_FVAL_LAYER_TYPES_UDP},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP, DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN,
     DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN}
};

static const additional_headers_map_t additional_headers_map[] = {
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_NONE, 0, 0, 0, 0,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_ESI_EVPN, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_ESI_EVPN, 1, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /** Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_FL_ESI_EVPN, 1, 0, 1, 3 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /** Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_FL_ESI_EVPN, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_EL_ELI_PWE, 1, 0, 1, 3 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_FL_PWE, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_EL_ELI, 1, 0, 1, 2 * MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_CW_PWE, 0, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, 0), DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_FL_PWE, 1, 0, 1, MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     1, 1, 1, MPLS_BYTES_TO_ADD, COMPILER_64_INIT(0, DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION),
     DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_VXLAN_GPE_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_VXLAN, 1, VXLAN_BYTES_TO_ADD, COMPILER_64_INIT(0,
                                                                             VXLAN_GPE_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GENEVE_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + GENEVE_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     1, CURRENT_NEXT_PROTOCOL_GENEVE, 1, GENEVE_BYTES_TO_ADD, COMPILER_64_INIT(0, UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, GENEVE_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_GENEVE_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_VXLAN_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     1, 0, 0, 0, COMPILER_64_INIT(0, VXLAN_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     1, CURRENT_NEXT_PROTOCOL_VXLAN, 1, VXLAN_BYTES_TO_ADD, COMPILER_64_INIT(0,
                                                                             VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GRE4, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE4_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE4_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_WO_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8K_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_GRE12, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE12_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE12_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_IPV6_TNI_GRE8, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN, 1, CURRENT_NEXT_PROTOCOL_GRE, 1, GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 8), DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(1), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(2), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(3), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(4), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(5), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(6), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {ADDITIONAL_HEADERS_UDP_USER_DEFINED(7), 1, CURRENT_NEXT_PROTOCOL_UDP, 1, UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP, 1, CURRENT_NEXT_PROTOCOL_UDP, 1,
     UDP_BYTES_TO_ADD,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, UDP_ESP_PROTOCOL_SPECIFIC_INFORMATION),
     DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
     /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP, 1, CURRENT_NEXT_PROTOCOL_ESP, 0, 0,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
     /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
     0, 0, 0, 0, COMPILER_64_INIT(0, 0), DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION}
};

 /*
  * Initialization table for MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLE.
  * Layer type is mapped to parser context.
  * For MPLS BOS upper layer protocol valid values are: Ethernet, Ipv4 and Ipv6.
  * The value of MPLS Parser Context is default configuration, indicating the value is not used.
  */
static const layer_type_to_mpls_upper_layer_protocol_t layer_type_to_mpls_upper_layer_protocol[] = {
    /**            Layer Type,                          MPLS Upper Layer Protocol*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV4_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV6_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ARP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_TCP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_UDP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PTP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ICMP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_BIER_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_RCH, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IGMP, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1}
};
 /*
  * Initialization table for forwarding_additional_info_config_map.
  * It configures EGRESS_FORWARDING_ADDITIONAL_INFO for IpvX compatible MC and INGRESS_FORWARDING_ADDITIONAL_INFO for unknown address.
  * Each line configures 'IPvX is multicast compatible with Fallback-to-Bridge' according to Forwarding additional information (FAI)
  * and unknown address according to FAI.
  * FAI[1] IP forwarding: means IPvX is multicast compatible with Fallback-to-Bridge.
  *        Eth forwarding: means enabler for forwarding layer plus one remarking.
  * FAI[2] means Unknown address enable.
  */
static const forwarding_additional_info_config_t forwarding_additional_info_config_map[] = {
   /** FAI, IPvX is multicast compatible with Fallback-to-bridge enable, BUM Traffic enable, fwd_plus_1_ingress_remark_profile*/
    {0, 0, 0, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {1, 0, 1, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {2, 1, 0, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {3, 1, 1, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {4, 0, 0, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {5, 0, 1, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {6, 1, 0, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {7, 1, 1, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {8, 0, 0, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {9, 0, 1, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {10, 1, 0, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {11, 1, 1, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {12, 0, 0, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {13, 0, 1, FWD_PLUS_1_REMARK_INGRESS_DISABLE},
    {14, 1, 0, FWD_PLUS_1_REMARK_INGRESS_ENABLE},
    {15, 1, 1, FWD_PLUS_1_REMARK_INGRESS_ENABLE}
};

static const jr_mode_fwd_code_mapping_config_t jr_mode_fwd_code_map[] = {
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 0, DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 1, DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, 0, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, 1, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, 0, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, 1, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, 0, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, 1, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS},
      /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, 0, DBAL_ENUM_FVAL_JR_FWD_CODE_TM},
      /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL, 1, DBAL_ENUM_FVAL_JR_FWD_CODE_TM}

};

static const jr_mode_vsi_src_mapping_config_t jr_mode_vsi_src_map[] = {
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 1, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 1, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_VRF}

};

/** contains entries for fodo assignment mode mapping:
 *  fodo assignment mode x pd result type  -> fodo mask profile */
static const fodo_assignment_mode_mapping_t fodo_assignment_mode_mappings[] = {
    {FODO_ASSIGNMENT_MODE_FROM_LIF, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LIF, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LOOKUP, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LOOKUP, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD},
    {FODO_ASSIGNMENT_MODE_FROM_VLAN, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD},
    {FODO_ASSIGNMENT_MODE_FROM_VLAN, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD}
};

/** contains entries for fodo assignment mask profiles:
 * fodo mask profile -> fodo mask, pd result mask */
static const fodo_assignment_mask_profile_t fodo_assignment_mask_profiles[] = {
    /*
     * fodo mask profile fodo_is_masked pd_result_is_fully_masked pd_result_is_vlan_masked
     */
    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF, TRUE, FALSE, FALSE},
    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD, FALSE, TRUE, FALSE},
    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD, TRUE, FALSE, TRUE}
};
  /*
   *INDENT-ON* */
/*
 * }
 */
/*
 *
 */

/**
 * \brief - The function configures initial values for EGRESS_ETPP_GLOBAL
 *
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
static shr_error_e
egress_prp_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to EGRESS_ETPP_GLOBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX_MSB, INST_SINGLE, FALSE);

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable))
    {
         /** Enable stamping FTMH bypass */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_STAMP_FTMH_ENABLE, INST_SINGLE, TRUE);
    }
    else
    {
         /** Disable stamping FTMH bypass */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_STAMP_FTMH_ENABLE, INST_SINGLE, FALSE);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_FIXED_LATENCY_EN, INST_SINGLE, FALSE);
    if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_remove_crc_bytes))
    {
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_etpp_compensate_crc_size))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GENERAL_PACKET_SIZE_COMPENSATION,
                                         INST_SINGLE, 2);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_OFFSET, INST_SINGLE, 2);
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRP_PEMA_CONTEXT_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function is called on Init time
 * It configures the EGRESS_PARSER_GLOBAL table
 *
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
static shr_error_e
egress_parser_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id, is_tdm_mode_packet;
    int system_headers_mode;
    /*
     * For JR1 compat mode
     */
    uint32 udh_size = 0, crc_size = 0;
    uint32 lb_key_ext_size_bytes = 0, stacking_ext_size_bytes = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if ((system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO_MODE)
        && (dnx_data_headers.general.feature_get(unit, dnx_data_headers_general_jericho_mode_sys_header_is_supported) ==
            0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Jericho mode system header is not supported");
    }
    crc_size = dnx_data_headers.system_headers.crc_size_counted_in_pkt_len_get(unit);

    /*
     * Initilize TDM_MODE_IS_TDM_PACKET vr for tdm parser check
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TDM, &entry_handle_id));
    is_tdm_mode_packet = (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_MODE_IS_TDM_PACKET, INST_SINGLE,
                                 is_tdm_mode_packet);

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PARSER_GLOBAL, entry_handle_id));
    /*
     * ETPPA_UTILITY_REGSr
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BLOCK_IS_ETPP, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BLOCK_IS_ETPP_INV, INST_SINGLE, 0);
    /*
     * ETPPA_JERICHO_COMPATIBLE_REGISTERSr
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JERICHO_FORWARD_CODE_FOR_MPLS, INST_SINGLE, 5);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JERICHO_FHEI_SIZE_FOR_MPLS_POP, INST_SINGLE, 3);
    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        /*
         * ETPPA_FAP_GLOBAL_SYS_HEADER_CFGr
         */
        uint64 work_reg_64;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADERS_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);

        /*
         * ETPPA_SYSTEM_HEADERS_CG_1r
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_SIZE, INST_SINGLE, 12);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE);
        /** Mapped size 5 times the required size FTMH_LB_KEY_EXT_SIZE */
        COMPILER_64_SET(work_reg_64, 0x18, 0xC6318C63);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     work_reg_64);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        COMPILER_64_SET(work_reg_64, 0, STACKING_EXT_SIZE);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     work_reg_64);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_SIZE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_LAYER_IDX,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_HEADER_IDX,
                                     INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_SIZE, INST_SINGLE,
                                     6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_HEADER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_SIZE, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_HEADER_IDX, INST_SINGLE, 4);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_SIZE, INST_SINGLE, 7);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_HEADER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_SIZE, INST_SINGLE, 8);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_HEADER_IDX, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_SIZE, INST_SINGLE, 9);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_HEADER_IDX, INST_SINGLE, 4);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_OFFSET, INST_SINGLE, 0x12);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_SIZE, INST_SINGLE, 19);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_LAYER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_SIZE, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_LAYER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_HEADER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_OFFSET, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_SIZE, INST_SINGLE, 16);

        /*
         * ETPPA_PPH_BASE_FIELDS_CG_2r
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LEARN_EXT_PRESENT_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 18);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LEARN_EXT_PRESENT_CG_2_WIDTH, INST_SINGLE,
                                     1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE_CG_2_INNER_OFFSET, INST_SINGLE,
                                     16);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE_CG_2_WIDTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LIF_EXT_TYPE_CG_2_INNER_OFFSET, INST_SINGLE,
                                     13);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LIF_EXT_TYPE_CG_2_WIDTH, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_OFFSET_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_OFFSET_CG_2_WIDTH, INST_SINGLE,
                                     7);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_TYPE_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_TYPE_CG_2_WIDTH, INST_SINGLE,
                                     5);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_INDEX_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 90);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_INDEX_CG_2_WIDTH, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_PPH_FWD_LAYER_ADD_INFO_CG_2_INNER_OFFSET, INST_SINGLE, 86);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADD_INFO_CG_2_WIDTH,
                                     INST_SINGLE, 4);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_STRENGTH_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_STRENGTH_CG_2_WIDTH, INST_SINGLE,
                                     1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_PROFILE_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 75);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_PROFILE_CG_2_WIDTH, INST_SINGLE, 8);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARD_DOMAIN_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 57);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARD_DOMAIN_CG_2_WIDTH, INST_SINGLE, 18);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_CG_2_INNER_OFFSET, INST_SINGLE, 35);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_CG_2_WIDTH, INST_SINGLE, 22);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_NWK_QOS_CG_2_INNER_OFFSET, INST_SINGLE, 27);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_NWK_QOS_CG_2_WIDTH, INST_SINGLE, 8);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_TTL_CG_2_INNER_OFFSET, INST_SINGLE, 19);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_TTL_CG_2_WIDTH, INST_SINGLE, 8);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_END_OF_PACKET_EDITING_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 83);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_END_OF_PACKET_EDITING_CG_2_WIDTH,
                                     INST_SINGLE, 3);

    }
    /*
     * Jericho mode beginning
     * {
     */
    else
    {
        uint64 work_reg_64;

        lb_key_ext_size_bytes = dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get(unit) ? 1 : 0;
        stacking_ext_size_bytes = dnx_data_headers.system_headers.jr_mode_ftmh_stacking_ext_mode_get(unit) ? 2 : 0;

        /*
         * ETPPA_FAP_GLOBAL_SYS_HEADER_CFGr
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADERS_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);

        /*
         * ETPPA_SYSTEM_HEADERS_CG_1r
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_CG_1_SIZE, INST_SINGLE, 7);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_OFFSET, INST_SINGLE,
                                     9 + lb_key_ext_size_bytes);
        /*
         * Map system_header_configuration.ftmh_lb_key_ext_en to field size(3b--->5b)
         * 0: 0Byte
         * 1: 1Byte
         */
        COMPILER_64_SET(work_reg_64, 0, 0x20);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     work_reg_64);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_OFFSET, INST_SINGLE,
                                     9 + lb_key_ext_size_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_SIZE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 9 + lb_key_ext_size_bytes + stacking_ext_size_bytes);
        /*
         * Map system_header_configuration.FTMH_STACKING_EXT_ENABLE to field size(3b--->5b)
         * 0: 0Byte
         * 2: 2Byte
         */
        COMPILER_64_SET(work_reg_64, 0, 0x800);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     work_reg_64);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_OFFSET, INST_SINGLE, 10);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_SIZE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_LAYER_IDX,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_HEADER_IDX,
                                     INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 9 + lb_key_ext_size_bytes + stacking_ext_size_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_SIZE, INST_SINGLE,
                                     6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_HEADER_IDX, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_OFFSET, INST_SINGLE,
                                     9 + lb_key_ext_size_bytes + stacking_ext_size_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_SIZE, INST_SINGLE, 6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_HEADER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_EXT_CG_1_SIZE, INST_SINGLE, 8);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_HEADER_IDX, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXT_CG_1_SIZE, INST_SINGLE, 9);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_LAYER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_HEADER_IDX, INST_SINGLE, 4);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_SIZE, INST_SINGLE, 5);

        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_3_get(unit));

        /*
         * Base not present, hence size is 0
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_SIZE, INST_SINGLE, 0);

        /*
         * UDH Data size/offset is equal to values read from soc props
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_LAYER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_OFFSET, INST_SINGLE, udh_size);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_DATA_CG_1_SIZE, INST_SINGLE, udh_size);

        /*
         * ETPPA_PPH_BASE_FIELDS_CG_2r
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LEARN_EXT_PRESENT_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 54);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LEARN_EXT_PRESENT_CG_2_WIDTH, INST_SINGLE,
                                     1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE_CG_2_INNER_OFFSET, INST_SINGLE,
                                     52);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE_CG_2_WIDTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LIF_EXT_TYPE_CG_2_INNER_OFFSET, INST_SINGLE,
                                     55);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_LIF_EXT_TYPE_CG_2_WIDTH, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_OFFSET_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 41);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_OFFSET_CG_2_WIDTH, INST_SINGLE,
                                     7);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_TYPE_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_PARSING_START_TYPE_CG_2_WIDTH, INST_SINGLE,
                                     0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_INDEX_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_INDEX_CG_2_WIDTH, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_PPH_FWD_LAYER_ADD_INFO_CG_2_INNER_OFFSET, INST_SINGLE, 48);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADD_INFO_CG_2_WIDTH,
                                     INST_SINGLE, 4);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_STRENGTH_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 40);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_STRENGTH_CG_2_WIDTH, INST_SINGLE,
                                     1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_PROFILE_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 36);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_PROFILE_CG_2_WIDTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARD_DOMAIN_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 18);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARD_DOMAIN_CG_2_WIDTH, INST_SINGLE, 16);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_CG_2_INNER_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_CG_2_WIDTH, INST_SINGLE, 18);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_NWK_QOS_CG_2_INNER_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_NWK_QOS_CG_2_WIDTH, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_TTL_CG_2_INNER_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_TTL_CG_2_WIDTH, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_VALUE_1_CG_2_INNER_OFFSET, INST_SINGLE, 38);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_VALUE_1_CG_2_WIDTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_VALUE_2_CG_2_INNER_OFFSET, INST_SINGLE, 34);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_VALUE_2_CG_2_WIDTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_END_OF_PACKET_EDITING_CG_2_INNER_OFFSET,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_END_OF_PACKET_EDITING_CG_2_WIDTH,
                                     INST_SINGLE, 0);
    }
    /*
     * }
     * Jericho mode end
     */
    /*
     * Remove 2 CRC Bytes by default if this device support this function
     */
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_remove_crc_bytes))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_PACKET_SIZE_REMOVE_CRC, INST_SINGLE, 2);
    }
    else if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_remove_crc_bytes_capability))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_PACKET_SIZE_REMOVE_CRC, INST_SINGLE, 0);
    }
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_erpp_compensate_crc_size))
    {
        /** Compensate fabric CRC size for ERPP statistics */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_CRC_COMPENSATION, INST_SINGLE,
                                     ERPP_COMPENSATE_CRC_SIZE);
    }
    /*
     * This configuration is required for ERPP blocks.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NO_GLEM_ACCESS_USE_GLOBAL_OUTLIF, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        /*
         * Initialize virtual registers for JR1 mode system headers parsing.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERJR1SYSHDRS, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR1_FTMH_LB_KEY_PLUS_FTMH_STACKING_SIZE_IN_BYTES,
                                     INST_SINGLE, lb_key_ext_size_bytes + stacking_ext_size_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR1_UDH1_PLUS_UDH2_SIZE_IN_BYTES, INST_SINGLE,
                                     udh_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Configure network-offset to the current UDH size
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_NETWORK_OFFSET, INST_SINGLE, udh_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Configure network_offset_for_system_headers to the current UDH size
         */
        if (dnx_data_dev_init.
            general.feature_get(unit, dnx_data_dev_init_general_network_offset_for_system_headers_valid))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_NETWORK_OFFSET_FOR_SYSTEM_HEADERS,
                                         INST_SINGLE, udh_size);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /*
         * count UDH size + CRC size into network-offset ,used for packet size calculation in term stage
         */
        if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_etpp_compensate_crc_size))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_OFFSET, INST_SINGLE,
                                         udh_size + crc_size);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    /*
     * }
     * Jericho mode end
     */
    else
    {
        /*
         * count CRC size into network-offset ,used for packet size calculation in term stage
         */
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_remove_crc_bytes))
        {
            if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_etpp_compensate_crc_size))
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_OFFSET, INST_SINGLE, crc_size);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_SELECTED_GLOBAL_OUTLIFS
 * \param [in] unit - Relevant unit.
 * \param [in] index - Index to the register.
 * \param [in] outlif_0_decision - Decision for Outlif 0.
 * \param [in] outlif_1_decision - Decision for Outlif 1.
 * \param [in] outlif_2_decision - Decision for Outlif 2.
 * \param [in] outlif_3_decision - Decision for Outlif 3.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_selected_outlif_table_configure(
    int unit,
    int index,
    int outlif_0_decision,
    int outlif_1_decision,
    int outlif_2_decision,
    int outlif_3_decision)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to EGRESS_SELECTED_GLOBAL_OUTLIFS table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_SELECTED_GLOBAL_OUTLIFS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_0_DECISION, INST_SINGLE, outlif_0_decision);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_1_DECISION, INST_SINGLE, outlif_1_decision);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_2_DECISION, INST_SINGLE, outlif_2_decision);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_3_DECISION, INST_SINGLE, outlif_3_decision);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init EGRESS_SELECTED_GLOBAL_OUTLIFS register.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e static
egress_selected_outlif_table_init(
    int unit)
{

    int nof_outlif_decision_entries;
    int entry;
    int system_headers_mode;
    outlif_decision_t *decision = NULL;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        decision = (outlif_decision_t *) outlif_decision_map_jr2_mode;
        nof_outlif_decision_entries = sizeof(outlif_decision_map_jr2_mode) / sizeof(outlif_decision_t);
    }
    else
    {
        decision = (outlif_decision_t *) outlif_decision_map_jer_mode;
        nof_outlif_decision_entries = sizeof(outlif_decision_map_jer_mode) / sizeof(outlif_decision_t);
    }

    /*
     * Initialize entries according to table
     */

    for (entry = 0; entry < nof_outlif_decision_entries; entry++)
    {
        /*
         * Write to Ingress EGRESS_SELECTED_GLOBAL_OUTLIFS table
         */
        SHR_IF_ERR_EXIT(egress_selected_outlif_table_configure
                        (unit, decision[entry].index,
                         decision[entry].outlif_0_decision,
                         decision[entry].outlif_1_decision,
                         decision[entry].outlif_2_decision, decision[entry].outlif_3_decision));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_SYSTEM_HEADERS_LIF_EXTENSION
 * \param [in] unit - Relevant unit.
 * \param [in] lif_extension_type - Lif Extension Type.
 * \param [in] outlif_extension_1 - Configuration for Outlif 1 Extension.
 * \param [in] outlif_extension_2 - Configuration for Outlif 1 Extension.
 * \param [in] outlif_extension_3 - Configuration for Outlif 1 Extension.
 * \param [in] inlif_extension_1 - Configuration for Inlif 1 Extension.
 * \param [in] inlif_extension_1_profile - Configuration for Inlif 1 Extension Profile.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_system_headers_lif_extension_configuration(
    int unit,
    int lif_extension_type,
    int outlif_extension_1,
    int outlif_extension_2,
    int outlif_extension_3,
    int inlif_extension_1,
    int inlif_extension_1_profile)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to EGRESS_SYSTEM_HEADERS_LIF_EXTENSION table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_SYSTEM_HEADERS_LIF_EXTENSION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_EXTENSION_TYPE, lif_extension_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_EXTENSION_1_CONFIG, INST_SINGLE,
                                 outlif_extension_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_EXTENSION_2_CONFIG, INST_SINGLE,
                                 outlif_extension_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_EXTENSION_3_CONFIG, INST_SINGLE,
                                 outlif_extension_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EXTENSION_1_CONFIG, INST_SINGLE,
                                 inlif_extension_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EXTENSION_1_PROFILE_CONFIG, INST_SINGLE,
                                 inlif_extension_1_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init EGRESS_SYSTEM_HEADERS_LIF_EXTENSION table.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e static
egress_system_headers_lif_extension_configuration_init(
    int unit)
{

    int nof_lif_extension_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_lif_extension_entries = sizeof(outlif_extension_map) / sizeof(outlif_extension_config_t);

    for (entry = 0; entry < nof_lif_extension_entries; entry++)
    {
        /*
         * Write to Ingress EGRESS_SYSTEM_HEADERS_LIF_EXTENSION table
         */
        SHR_IF_ERR_EXIT(egress_system_headers_lif_extension_configuration
                        (unit, outlif_extension_map[entry].lif_extension_type,
                         outlif_extension_map[entry].outlif_extension_1,
                         outlif_extension_map[entry].outlif_extension_2,
                         outlif_extension_map[entry].outlif_extension_3, outlif_extension_map[entry].inlif_extension_1,
                         outlif_extension_map[entry].inlif_extension_1_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        INGRESS_LBP_GLOBAL
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
static shr_error_e
ingress_lbp_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Write default values to INGRESS_LBP_GLOBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_GLOBAL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_RESERVED_BITS_SIZE, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_OUT_LIF_SIZE, INST_SINGLE, 22);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_SIZE, INST_SINGLE, 22);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_IN_LIF_PROFILE_SIZE, INST_SINGLE, 8);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_IDX_SIZE, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_LAYER_ADDITIONAL_INFO_SIZE, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FWD_DOMAIN_SIZE, INST_SINGLE, 18);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_END_OF_PACKET_EDITING_SIZE, INST_SINGLE, 3);

    /*
     * This is only checked under JR2 mode
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TM_DESTINATION_EXT_PRESENT, INST_SINGLE,
                                 dnx_data_headers.ftmh.add_dsp_ext_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLB_LEARN_PAYLOAD_START_BIT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELEPHANT_TRAP_LEARN_PAYLOAD_START_BIT, INST_SINGLE,
                                 0);
    if (system_headers_mode != SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_RIF_SIZE, INST_SINGLE,
                                     dnx_data_l3.rif.nof_rifs_get(unit));

        /*
         * UDH In JR1 system header mode, working at JR UDH mode add-udb-base=0;udh-size-compensation=1 AX UDH mode is not
         * supported by default
         */

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_ADD_UDH_BASE, INST_ALL, FALSE);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_UDH_SIZE_COMPENSATION_ENABLE, INST_ALL,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_FHEI_WITH_IN_LIF_PROFILE, INST_SINGLE,
                                     TRUE);

        /**
         *  The timestamp for ingress latency is 34 bits in IPP. In JR system headers mode the OAM TS header
         *  has 27 bits for timestamp, so it is shifting right the input. When ITPP receives the 27 bits
         *  timestamp from the header, it shifts it back left up to compensate. Some accuracy is lost in the process.
         *  Below code configures the right shift in the LBP
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_TIMESTAMP_RIGHT_SHIFT, INST_SINGLE,
                                     dnx_data_headers.system_headers.jr_mode_ftmh_oam_ts_right_shift_get(unit));
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (system_headers_mode != SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));
        /*
         * VR used by Up MEP ucode to indicate DSP extension present, only in JR1 Mode
         */
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_CFG_UP_MEP_STACKING_EXTENSION_PRESENT_INDICATION, INST_SINGLE,
                                     dnx_data_headers.system_headers.jr_mode_ftmh_stacking_ext_mode_get(unit));
        /*
         * VR used by Up MEP ucode to indicate LB key extension present, only in JR1 Mode
         */
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_CFG_UP_MEP_LOAD_BALANCING_EXTENSION_PRESENT_INDICATION, INST_SINGLE,
                                     dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get(unit));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (dnx_data_headers.ftmh.add_dsp_ext_get(unit))
    {
        /*
         * VR used by Up MEP ucode to indicate DSP extension present
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_UP_MEP_DSP_EXTENSION_PRESENT_INDICATION,
                                     INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_lbp_vlan_editing_configuration_init(
    int unit)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_LBP_VLAN_EDITING table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_VLAN_EDITING, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_IVE_SIZE_TYPE, INST_SINGLE, 2);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] layer_type - fwd layer type.
 * \param [in] fwd_layer_qual_lsb - fwd layer qualifier lsb
 * \param [in] fwd_code - jr fwd code

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_lbp_jr_fwd_code_map_table_configure(
    int unit,
    dbal_enum_value_field_layer_types_e layer_type,
    int fwd_layer_qual_lsb,
    dbal_enum_value_field_jr_fwd_code_e fwd_code)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_TYPE, layer_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_QUAL_LSB, fwd_layer_qual_lsb);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, INST_SINGLE, fwd_code);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] fwd_code - jr fwd code.
 * \param [in] is_mc - is multicast
 * \param [in] outlif_is_rif - outLif is RIF
 * \param [in] vsi_src - vsi-src

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_lbp_jr_vsi_src_map_table_configure(
    int unit,
    dbal_enum_value_field_jr_fwd_code_e fwd_code,
    int is_mc,
    int outlif_is_rif,
    dbal_enum_value_field_jr_vsi_src_e vsi_src)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, fwd_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_RIF, outlif_is_rif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_VSI_SRC, INST_SINGLE, vsi_src);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE including in_lif_selection
 * \param [in] unit - Relevant unit.
 * \param [in] fwd_code - jr fwd code.
 * \param [in] is_mc - is multicast
 * \param [in] outlif_is_rif - outLif is RIF
 * \param [in] in_lif_selection - 0 for In_lif(0) and 1 for IN_LIF(1)
 * \param [in] vsi_src - vsi-src

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_lbp_jr_vsi_src_in_lif_map_table_configure(
    int unit,
    dbal_enum_value_field_jr_fwd_code_e fwd_code,
    int is_mc,
    int outlif_is_rif,
    uint8 in_lif_selection,
    dbal_enum_value_field_jr_vsi_src_e vsi_src)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write values to DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, fwd_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_RIF, outlif_is_rif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_VSI_SRC, INST_SINGLE, vsi_src);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_IN_LIF_SRC, INST_SINGLE,
                                 (uint32) in_lif_selection);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        JR_FHEI_SIZE_MAPPING
 * \param [in] unit - Relevant unit.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_lbp_jr_fhei_size_map_table_configure(
    int unit)
{

    uint32 entry_handle_id;
    dbal_enum_value_field_jr_fwd_code_e fwd_code_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR_FHEI_SIZE_MAPPING table
     */


    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_FHEI_SIZE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_3B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


    for (fwd_code_index = DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE; fwd_code_index < DBAL_NOF_ENUM_JR_FWD_CODE_VALUES;
         fwd_code_index++)
    {
        if ((fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_TRILL)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_CPU_TRAP1)
            && (fwd_code_index != DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED0))
        {

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, fwd_code_index);
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, DBAL_RANGE_ALL,
                                             DBAL_RANGE_ALL);
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL,
                                             DBAL_RANGE_ALL);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_FHEI_SIZE_0B);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
    }


    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_8B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * For Fall-to-bridge, All the IPvX MC carry 8B FHEI
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_8B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MPLS swap from EEDB carry 5B FHEI
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, FALSE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_5B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MPLS PHP from EEDB carry 3B FHEI
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, TRUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_SIZE, INST_SINGLE, DBAL_ENUM_FVAL_FHEI_SIZE_3B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        IPPD_PPH_VSI_SOURCE
 *        IPPD_FORWARDING_TYPE_TO_CODE
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
static shr_error_e
ingress_lbp_jer_mode_configuration_init(
    int unit)
{
    int nof_entries;
    int entry;
    int roo_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_entries = sizeof(jr_mode_fwd_code_map) / sizeof(jr_mode_fwd_code_mapping_config_t);

    for (entry = 0; entry < nof_entries; entry++)
    {
        /*
         * Write to Ingress DBAL_TABLE_JR_FWD_CODE_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(ingress_lbp_jr_fwd_code_map_table_configure
                        (unit, jr_mode_fwd_code_map[entry].fwd_layer_type,
                         jr_mode_fwd_code_map[entry].fwd_layer_qual_lsb, jr_mode_fwd_code_map[entry].jr_fwd_code));
    }

    /*
     * Initialize entries according to table
     */
    nof_entries = sizeof(jr_mode_vsi_src_map) / sizeof(jr_mode_vsi_src_mapping_config_t);

    if (dnx_data_field.init.jr1_ipmc_inlif_get(unit) == 1)
    {
        for (entry = 0; entry < nof_entries; entry++)
        {
            /*
             * Write to Ingress DBAL_TABLE_JR_PPH_VSI_SRC_MAPPING_TABLE table
             */

            SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_map_table_configure
                            (unit, jr_mode_vsi_src_map[entry].jr_fwd_code,
                             jr_mode_vsi_src_map[entry].is_mc,
                             jr_mode_vsi_src_map[entry].outlif_is_rif, jr_mode_vsi_src_map[entry].jr_vsi_src));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_in_lif_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0, 1, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF));
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_in_lif_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0, 1, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF));
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_in_lif_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1, 0, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF));
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_in_lif_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 1, 0, 1, DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF));

    }

    /*
     * For Non ROO case, vsi should be invalid when
     * ROO is not needed.
     */
    roo_enable = dnx_data_field.init.roo_get(unit);
    if (!roo_enable)
    {
        /*
         * Write to Ingress DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_INVALID));

        /*
         * Write to Ingress DBAL_TABLE_JR_VSI_SRC_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(ingress_lbp_jr_vsi_src_map_table_configure
                        (unit, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0, 0, DBAL_ENUM_FVAL_JR_VSI_SRC_INVALID));

    }

    SHR_IF_ERR_EXIT(ingress_lbp_jr_fhei_size_map_table_configure(unit));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - The function configures initial values for
 *        INGRESS_LBP_VLAN_EDITING
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
static shr_error_e
ingress_vtt_global_configuration_init(
    int unit)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_VTT_GLOBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VTT_GLOBAL, &entry_handle_id));

    /** set interface 0 and 2 of exem to work with My MAC */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEXEM_INTERFACE_0_SEL, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEXEM_INTERFACE_2_SEL, INST_SINGLE, 1);
    /**set LLVP access mode */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXTEND_VCP_EN, INST_SINGLE,
                                 dnx_data_vlan.llvp.ext_vcp_en_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_port_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id, pp_port, mapped_pp_port;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_PP_PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        for (pp_port = 0; pp_port < dnx_data_port.general.nof_pp_ports_get(unit); pp_port++)
        {
            mapped_pp_port =
                ((core_id << utilex_log2_round_up(dnx_data_port.general.nof_pp_ports_get(unit))) | pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, INST_SINGLE, mapped_pp_port);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        ITPPD_ITPP_GENERAL_CFG and ITPP_ITPP_GENERAL_CFG registers
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
static shr_error_e
ingress_itpp_general_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_ITPP_GENERAL_CFG table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ITPP_GENERAL_CFG, &entry_handle_id));
    /** Set to field CUD_INVALID_VALUE its default value */
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_CUD_INVALID_VALUE, INST_SINGLE,
                                               DBAL_PREDEF_VAL_DEFAULT_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_TERMINATION, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Write default values to SYSTEM_HEADER_GLOBAL_CFG table for the outlif extension sizes
     * The first instance writes the size of LIF Extension 1xOutLIF - 3B
     * The second instance writes the size of LIF Extension 2xOutLIF - 2*1xOutLIF Size
     * The third instance writes the size of LIF Extension 2xOutLIF - 3*1xOutLIF Size
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 0, LIF_EXTENSION_SIZE_1XOUT_LIF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 1,
                                 2 * LIF_EXTENSION_SIZE_1XOUT_LIF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 2,
                                 3 * LIF_EXTENSION_SIZE_1XOUT_LIF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_EXT_SIZE, INST_SINGLE,
                                 LIF_EXTENSION_SIZE_IN_LIF_PROFILE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * The table enables termination of network headers.
     */
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ITPP_TERMINATE_NETWORK_HEADERS, entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** If FTMH.TM-Profile(2), Forwarding-Strength(1) == 7, do not terminate*/
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_eedb_forwarding_domain_vsd_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_VSD_ENABLE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {
        /*
         * Initialize entries according to table Start of 1 to skip the NONE value
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, 1, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
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
static shr_error_e
egress_eedb_svtag_indication_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_SVTAG_ENABLE, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, TRUE,
                                                        DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
        if (max_value > 0)
        {
            /*
             * Initialize entries according to table Start of 1 to skip the NONE value
             */
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, 1,
                                             DBAL_RANGE_ALL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_ENABLE, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init INGRESS_DA_TYPE_MAP table.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_da_type_map_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_DA_TYPE_MAP, &entry_handle_id));
    /** In case the Layer Qualifiers are DA_BC=DA_MC=1, the DA TYPE is BC */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** In case the Layer Qualifiers are DA_BC=0 DA_MC=1, the DA TYPE is MC */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** In case the Layer Qualifiers are DA_BC=DA_MC=0, the DA TYPE is UC */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] system_header_profile - System Header Generation Profile.
 * \param [in] bytes_to_add - bytes_to_add

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_system_header_generation_table_configure(
    int unit,
    int system_header_profile,
    int bytes_to_add)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_HDR_GENERATION_PROFILE, system_header_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTES_TO_ADD, INST_SINGLE, bytes_to_add);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP
 * \param [in] unit - Relevant unit.
 * \param [in] lif_additional_headers_profile - LIF Additional Headers Profile.
 * \param [in] context_additional_headers_profile - context_additional_headers_profile
 * \param [in] additional_headers_profile - Additional Headers Profile

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_table_configure(
    int unit,
    int lif_additional_headers_profile,
    int context_additional_headers_profile,
    int additional_headers_profile)
{

    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e ahp_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP table
     */
    switch (context_additional_headers_profile)
    {
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_UDP_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_UDP;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_GRE_GENEVE_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_GRE_GENEVE;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_VXLAN_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_VXLAN_VXLAN_GPE;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH:
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MPLS_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_MPLS;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP:
            table_id = DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_IP_MAP;
            ahp_field_id = DBAL_FIELD_LIF_AHP_IP;
            break;
        case DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE:
            /** Keep default Values */
            SHR_EXIT();
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "CTX additional header profile is unknown\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, ahp_field_id, lif_additional_headers_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CTX_ADDITIONAL_HEADER_PROFILE,
                               context_additional_headers_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES,
                                 INST_SINGLE, additional_headers_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_ADDITIONAL_HEADERS_PROFILE_TABLE table
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_profile_table_init(
    int unit)
{

    int nof_additional_header_profile_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_additional_header_profile_entries =
        sizeof(additional_headers_profile_config_map) / sizeof(additional_headers_profile_config_t);

    for (entry = 0; entry < nof_additional_header_profile_entries; entry++)
    {
        /*
         * Write to EGRESS_ADDITIONAL_HEADERS_PROFILE_TABLE table
         */
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_table_configure
                        (unit, additional_headers_profile_config_map[entry].lif_additional_headers_profile,
                         additional_headers_profile_config_map[entry].context_additional_headers_profile,
                         additional_headers_profile_config_map[entry].additional_headers_profile));
    }

exit:
    SHR_FUNC_EXIT;
}
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
shr_error_e
dnx_init_pp_egress_current_next_protocol_config_table_configure(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int current_next_protocol)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_PROTOCOL_TYPE, current_protocol_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PROTOCOL_NAMESPACE, current_protocol_namespace);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_NEXT_PROTOCOL, INST_SINGLE,
                                 current_next_protocol);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see .h file for description
 */
shr_error_e
dnx_init_pp_egress_current_next_protocol_config_table_get(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int *current_next_protocol)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_PROTOCOL_TYPE, current_protocol_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PROTOCOL_NAMESPACE, current_protocol_namespace);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CURRENT_NEXT_PROTOCOL, INST_SINGLE,
                               (uint32 *) current_next_protocol);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_udp_next_protocol_enable_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set parsing value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERUDP, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4OUDP_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6OUDP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_gre_next_protocol_eth_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set parsing value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERGRE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH, INST_SINGLE, ETHERTYPE_NEXT_PROTOCOL_ETHERNET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE table
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_current_next_protocol_config_table_init(
    int unit)
{

    int nof_current_next_protocol_entries;
    int entry;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_current_next_protocol_entries =
        sizeof(current_next_protocol_config_map) / sizeof(current_next_protocol_config_table_t);

    for (entry = 0; entry < nof_current_next_protocol_entries; entry++)
    {
        /*
         * Write to EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE table
         */
        SHR_IF_ERR_EXIT(dnx_init_pp_egress_current_next_protocol_config_table_configure
                        (unit, current_next_protocol_config_map[entry].current_protocol_type,
                         current_next_protocol_config_map[entry].current_protocol_namespace,
                         current_next_protocol_config_map[entry].current_next_protocol));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_GLOBAL_INLIF_RESOLUTION
 * \param [in] unit - Relevant unit.
 * \param [in] system_headers_mode - System Header Mode - Jericho or Jericho2.
 * \param [in] is_mc_forward_code - Forward Code - IPVX MC or not MC
 * \param [in] inlif_decision_0 - Decision of Inlif Resolution 0
 * \param [in] inlif_decision_1 - Decision of Inlif Resolution 1

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_global_inlif_resolution_table_configure(
    int unit,
    int system_headers_mode,
    int is_mc_forward_code,
    int inlif_decision_0,
    int inlif_decision_1)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_GLOBAL_INLIF_RESOLUTION table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_GLOBAL_INLIF_RESOLUTION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADERS_MODE, system_headers_mode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FORWARD_CODE_IPVX_MC, is_mc_forward_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_DECISION_0, INST_SINGLE, inlif_decision_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_DECISION_1, INST_SINGLE, inlif_decision_1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures dummy glem entry for
 *        DBAL_TABLE_GLOBAL_LIF_EM
 * \param [in] unit - Relevant unit.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_glem_dummy_entry_configure(
    int unit)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * To avoid GLEM PP ERPP trap when global lif equals null_lif_value,
     * write dummy glem entry values to DBAL_TABLE_GLOBAL_LIF_EM table.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, 0, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init DBAL_TABLE_EGRESS_GLOBAL_INLIF_RESOLUTION table.
 * The function reads static table and configures the inlif decisions on init time.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
egress_global_inlif_resolution_table_init(
    int unit)
{

    int nof_global_inlif_config_entries;
    int entry;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Initialize entries according to table
     */
    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        nof_global_inlif_config_entries = sizeof(inlif_decision_config_map_jr2_mode) / sizeof(inlif_decision_config_t);
    }
    else
    {
        nof_global_inlif_config_entries = sizeof(inlif_decision_config_map_jer_mode) / sizeof(inlif_decision_config_t);
    }

    for (entry = 0; entry < nof_global_inlif_config_entries; entry++)
    {
        /*
         * Write to DBAL_TABLE_EGRESS_GLOBAL_INLIF_RESOLUTION table
         */
        if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            SHR_IF_ERR_EXIT(egress_global_inlif_resolution_table_configure
                            (unit, inlif_decision_config_map_jr2_mode[entry].system_headers_mode,
                             inlif_decision_config_map_jr2_mode[entry].is_mc_forward_code,
                             inlif_decision_config_map_jr2_mode[entry].inlif_decision_0,
                             inlif_decision_config_map_jr2_mode[entry].inlif_decision_1));
        }
        else
        {
            SHR_IF_ERR_EXIT(egress_global_inlif_resolution_table_configure
                            (unit, inlif_decision_config_map_jer_mode[entry].system_headers_mode,
                             inlif_decision_config_map_jer_mode[entry].is_mc_forward_code,
                             inlif_decision_config_map_jer_mode[entry].inlif_decision_0,
                             inlif_decision_config_map_jer_mode[entry].inlif_decision_1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE table.
 * The function reads static table and configures the system headers generation profile on init time.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
egress_system_header_generation_table_init(
    int unit)
{

    int nof_system_header_generation_profile_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_system_header_generation_profile_entries = sizeof(system_header_profile_map) / sizeof(system_header_profile_t);

    for (entry = 0; entry < nof_system_header_generation_profile_entries; entry++)
    {
        /*
         * Write to DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE table
         */
        SHR_IF_ERR_EXIT(egress_system_header_generation_table_configure
                        (unit, system_header_profile_map[entry].system_header_profile,
                         system_header_profile_map[entry].bytes_to_add));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Init INGRESS_PP_RECYCLE_COMMAND table.
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
static shr_error_e
ingress_prt_recycle_command_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_RECYCLE_COMMAND, &entry_handle_id));

    /** Configure PRT Recycle Profiles */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD, 2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD,
                               DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_CMD_PRT_PROFILE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_egress_parser_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERETH, &entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEFCOE, INST_SINGLE,
                                 DBAL_DEFINE_ETHERTYPE_FCOE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEPPP, INST_SINGLE, DBAL_DEFINE_ETHERTYPE_PPP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEPTP_GENERAL, INST_SINGLE,
                                 DBAL_DEFINE_ETHERTYPE_1588);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERIPV4, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLICMP, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_ICMP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIGMP, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_IGMP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERIPV6, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLICMPV6, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_ICMPV6);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERUDP, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_SINGLE_HOP, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_BFD_SINGLE_HOP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_MULTI_HOP, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_BFD_MULTI_HOP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENERAL_1, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_PTP_EVENT_MSG);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENERAL_2, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_PTP_GENERAL_MSG);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTETH, INST_SINGLE, DBAL_DEFINE_UDP_PORT_ETH);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTIPV6, INST_SINGLE, DBAL_DEFINE_UDP_PORT_IPV6);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENEVE, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_GENEVE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTMICROBFD, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_MICROBFD);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTL2TP, INST_SINGLE, DBAL_DEFINE_UDP_PORT_L2TP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTVXLAN_1, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_VXLAN);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTVXLAN_2, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_VXLAN_GPE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTIPV4, INST_SINGLE, DBAL_DEFINE_UDP_PORT_IPV4);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTMPLS, INST_SINGLE, DBAL_DEFINE_UDP_PORT_MPLS);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_SEAMLESS, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_SEAMLESS_BFD);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERVXLAN_GPE, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPT);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPV4, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPV4);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPV6, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPV6);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLMPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_MPLS);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Init  INGRESS_PRT_INFO dbal table.
 *
 * \param [in] unit - Relevant unit
 * \param [in] prt_dnx_data_table_index - Index that is used as a key to PRT dnx data table
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_prt_profile_configuration(
    int unit,
    int prt_dnx_data_table_index)
{

    uint32 entry_handle_id;
    int ffc_instruction_5;
    int ffc_instruction_4;
    int ffc_instruction_3;
    int ffc_instruction_2;
    int ffc_instruction_1;
    int ffc_instruction_0;
    const dnx_data_port_general_prt_configuration_t *prt_configuration;
    uint32 prt_ffc_width_start_offset;
    uint32 prt_ffc_start_input_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    prt_ffc_width_start_offset = dnx_data_port.general.prt_ffc_width_start_offset_get(unit);
    prt_ffc_start_input_offset = dnx_data_port.general.prt_ffc_start_input_offset_get(unit);
    prt_configuration = dnx_data_port.general.prt_configuration_get(unit, prt_dnx_data_table_index);
    /*
     * Build the values of PRT FFC instructions
     */
    ffc_instruction_5 =
        BUILD_PRT_FFC(prt_configuration->ffc_5_offset, prt_configuration->ffc_5_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_5_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_4 =
        BUILD_PRT_FFC(prt_configuration->ffc_4_offset, prt_configuration->ffc_4_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_4_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_3 =
        BUILD_PRT_FFC(prt_configuration->ffc_3_offset, prt_configuration->ffc_3_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_3_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_2 =
        BUILD_PRT_FFC(prt_configuration->ffc_2_offset, prt_configuration->ffc_2_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_2_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_1 =
        BUILD_PRT_FFC(prt_configuration->ffc_1_offset, prt_configuration->ffc_1_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_1_input_offset, prt_ffc_start_input_offset);
    ffc_instruction_0 =
        BUILD_PRT_FFC(prt_configuration->ffc_0_offset, prt_configuration->ffc_0_width, prt_ffc_width_start_offset,
                      prt_configuration->ffc_0_input_offset, prt_ffc_start_input_offset);

    /*
     * Write default values to INGRESS_PRT_INFO table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_RECYCLE_PROFILE,
                               prt_configuration->prt_recycle_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE,
                               prt_configuration->port_termination_ptc_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_MODE, INST_SINGLE,
                                 prt_configuration->prt_qual_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE,
                                 prt_configuration->layer_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 4,
                                 prt_configuration->kbr_valid_bitmap_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 3,
                                 prt_configuration->kbr_valid_bitmap_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 2,
                                 prt_configuration->kbr_valid_bitmap_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 1,
                                 prt_configuration->kbr_valid_bitmap_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 0,
                                 prt_configuration->kbr_valid_bitmap_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 5, ffc_instruction_5);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 4, ffc_instruction_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 3, ffc_instruction_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 2, ffc_instruction_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 1, ffc_instruction_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 0, ffc_instruction_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_PRT_MODE, INST_SINGLE,
                                 prt_configuration->src_sys_port_prt_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT_ENABLE, INST_SINGLE,
                                 prt_configuration->pp_port_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_MODE_FULL_ENABLE, INST_SINGLE,
                                 prt_configuration->tcam_mode_full_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PRT_MODE, INST_SINGLE,
                                 prt_configuration->context_select);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_prt_profile_configuration_init(
    int unit)
{
    const dnxc_data_table_info_t *prt_config_dnx_data_table_info;
    int index;
    SHR_FUNC_INIT_VARS(unit);

    prt_config_dnx_data_table_info = dnx_data_port.general.prt_configuration_info_get(unit);
    /*
     * Initialize entries according to dnx data table "prt_configuration"
     */
    for (index = 0; index < prt_config_dnx_data_table_info->key_size[0]; index++)
    {
        /*
         * Write to INGRESS_PRT_PROFILE table
         */
        SHR_IF_ERR_EXIT(ingress_prt_profile_configuration(unit, index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set the fodo_mask field in forwarding domain assignment profile configuration mapping table
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] fodo_is_masked - indicate how to set the fodo_mask
 * \param [in] entry_handle_id - dbal handle
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_set_fodo_mask(
    int unit,
    uint32 entry_handle_id,
    uint8 fodo_is_masked)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fodo_is_masked)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MIN_VALUE);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_set_pd_result_mask(
    int unit,
    uint32 entry_handle_id,
    uint8 pd_result_is_fully_masked,
    uint8 pd_result_is_vlan_masked)
{
    SHR_FUNC_INIT_VARS(unit);

    if (pd_result_is_fully_masked)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    }
    else if (pd_result_is_vlan_masked)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK, INST_SINGLE, BCM_VLAN_MAX);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MIN_VALUE);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * Configure entry in forwarding domain assignment profile configuration mapping table
 * Map the forwarding mask profile to forwarding domain mask base (mask forwarding domain resulting from the lif) and
 *   direct extraction priority decoder result mask (mask direct ext pd result).
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] fodo_mask_profile - fodo mask profile
 * \param [in] fodo_is_masked - indicate how to set the fodo_mask
 * \param [in] pd_result_is_fully_masked - indicate how to ste the pd_result_mask
 * \param [in] pd_result_is_vlan_masked - indicate how to ste the pd_result_mask
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_configure(
    int unit,
    uint32 fodo_mask_profile,
    uint8 fodo_is_masked,
    uint8 pd_result_is_fully_masked,
    uint8 pd_result_is_vlan_masked)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_PROFILE, &entry_handle_id));
    /*
     * fodo mask profile ->
     *      fodo mask, pd result mask
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, fodo_mask_profile);
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_set_fodo_mask
                    (unit, entry_handle_id, fodo_is_masked));
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_set_pd_result_mask
                    (unit, entry_handle_id, pd_result_is_fully_masked, pd_result_is_vlan_masked));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init forwarding domain assignment mode registers.
 * - forwarding domain assignment mode mapping table:
 * Map lif.forwarding_domain_assignment_mode and "direct extraction priority decoder result type" to forwarding domain
 * mask profile.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_mapping_init(
    int unit)
{
    int nof_fodo_assignment_mask_profile_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    nof_fodo_assignment_mask_profile_entries =
        sizeof(fodo_assignment_mask_profiles) / sizeof(fodo_assignment_mask_profile_t);

    for (entry = 0; entry < nof_fodo_assignment_mask_profile_entries; entry++)
    {
        SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_configure(unit,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].fodo_mask_profile,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].fodo_is_masked,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].pd_result_is_fully_masked,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].pd_result_is_vlan_masked));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Configure an entry forwarding domain assignment mode mapping table:
 * Map lif.forwarding_domain_assignment_mode and "direct extraction priority decoder result type" to forwarding domain
 * mask profile.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] fodo_assignment_mode - fodo assignment mode
 * \param [in] pd_result_type - pd result type
 * \param [in] fodo_mask_profile = fodo mask profile
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_forwarding_domain_assignment_mode_mapping_configure(
    int unit,
    uint32 fodo_assignment_mode,
    uint32 pd_result_type,
    uint32 fodo_mask_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_MODE_MAPPING, &entry_handle_id));

    /** fodo assignment mode x pd result type  -> fodo mask profile
     *  from lif             x found/not found -> lif
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, fodo_assignment_mode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_PD_RESULT_TYPE, pd_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, INST_SINGLE,
                                 fodo_mask_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init forwarding domain assignment mode registers.
 * - forwarding domain assignment mode mapping table:
 * Map lif.forwarding_domain_assignment_mode and "direct extraction priority decoder result type" to forwarding domain
 * mask profile.
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
ingress_forwarding_domain_assignment_mode_mapping_init(
    int unit)
{
    int nof_fodo_assignment_mode_mapping_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    nof_fodo_assignment_mode_mapping_entries =
        sizeof(fodo_assignment_mode_mappings) / sizeof(fodo_assignment_mode_mapping_t);

    for (entry = 0; entry < nof_fodo_assignment_mode_mapping_entries; entry++)
    {
        SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_mapping_configure(unit,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].fodo_assignment_mode,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].pd_result_type,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].fodo_mask_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_forwarding_domain_assignment_mode_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_mapping_init(unit));
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_mapping_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function calculates MPLS protocol specific information
 *          for EGRESS_ADDITIONAL_HEADERS_MAP_TABLE
 * \param [in] label_type
 * \param [in] label_val
 * \param [in] exp_type
 * \param [in] exp_val
 * \param [in] ttl_type
 * \param [in] ttl_val
 * \param [out] protocol_specific_info - MPLS protocol header
                specific information
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
egress_mpls_protocol_header_specific_information_get(
    uint32 label_type,
    uint32 label_val,
    uint32 exp_type,
    uint32 exp_val,
    uint32 ttl_type,
    uint32 ttl_val,
    COMPILER_UINT64 * protocol_specific_info)
{
    COMPILER_64_ZERO(*protocol_specific_info);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, label_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, label_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, exp_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, exp_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, ttl_type,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET);

    MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_FIELD_SET(*protocol_specific_info, ttl_val,
                                                        MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET);

    return;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] additional_headers_profile - Additional Headers Profile - key to the table
 * \param [in] start_current_next_protocol_enable_main_header - Enable/Disable start_current_next_protocol for Main Header
 * \param [in] start_current_next_protocol_main_header - start_current_next_protocol value for Main Header
 * \param [in] additional_headers_packet_size_enable_main_header - Enable/Disable Additional Headers Packet Size for Main Header
 * \param [in] additional_headers_packet_size_main_header - Additional Headers Packet Size for Main Header
 * \param [in] start_current_next_protocol_enable_add_header_0 - Enable/Disable start_current_next_protocol for Additional Header 0
 * \param [in] start_current_next_protocol_add_header_0 - start_current_next_protocol value for Additional Header 0
 * \param [in] additional_headers_packet_size_enable_add_header_0 - Enable/Disable Additional Headers Packet Size for Additional Header 0
 * \param [in] additional_headers_packet_size_add_header_0 - Additional Headers Packet Size value for Additional Header 0
 * \param [in] protocol_specific_information_add_header_0 - Protocol Specific Information value for Additional Header 0
 * \param [in] protocol_specific_information_add_header_0_type - Protocol Specific Information value for Additional Header 0
 * \param [in] start_current_next_protocol_enable_add_header_1 - Enable/Disable start_current_next_protocol for Additional Header 1
 * \param [in] start_current_next_protocol_add_header_1 - start_current_next_protocol value for Additional Header 1
 * \param [in] additional_headers_packet_size_enable_add_header_1 - Enable/Disable Additional Headers Packet Size for Additional Header 1
 * \param [in] additional_headers_packet_size_add_header_1 - Additional Headers Packet Size value for Additional Header 1
 * \param [in] protocol_specific_information_add_header_1 - Protocol Specific Information value for Additional Header 1
 * \param [in] protocol_specific_information_add_header_1_type - Protocol Specific Information value for Additional Header 1
 * \param [in] start_current_next_protocol_enable_add_header_2 - Enable/Disable start_current_next_protocol for Additional Header 2
 * \param [in] start_current_next_protocol_add_header_2 - start_current_next_protocol value for Additional Header 2
 * \param [in] additional_headers_packet_size_enable_add_header_2 - Enable/Disable Additional Headers Packet Size for Additional Header 2
 * \param [in] additional_headers_packet_size_add_header_2 - Additional Headers Packet Size value for Additional Header 2
 * \param [in] protocol_specific_information_add_header_2 - Protocol Specific Information value for Additional Header 2
 * \param [in] protocol_specific_information_add_header_2_type - Protocol Specific Information value for Additional Header 2
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_additional_headers_map_tables_configuration(
    int unit,
    int additional_headers_profile,
    int start_current_next_protocol_enable_main_header,
    int start_current_next_protocol_main_header,
    int additional_headers_packet_size_enable_main_header,
    int additional_headers_packet_size_main_header,
    int start_current_next_protocol_enable_add_header_0,
    int start_current_next_protocol_add_header_0,
    int additional_headers_packet_size_enable_add_header_0,
    int additional_headers_packet_size_add_header_0,
    COMPILER_UINT64 protocol_specific_information_add_header_0,
    dbal_fields_e protocol_specific_information_add_header_0_type,
    int start_current_next_protocol_enable_add_header_1,
    int start_current_next_protocol_add_header_1,
    int additional_headers_packet_size_enable_add_header_1,
    int additional_headers_packet_size_add_header_1,
    COMPILER_UINT64 protocol_specific_information_add_header_1,
    dbal_fields_e protocol_specific_information_add_header_1_type,
    int start_current_next_protocol_enable_add_header_2,
    int start_current_next_protocol_add_header_2,
    int additional_headers_packet_size_enable_add_header_2,
    int additional_headers_packet_size_add_header_2,
    COMPILER_UINT64 protocol_specific_information_add_header_2,
    dbal_fields_e protocol_specific_information_add_header_2_type)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES,
                               additional_headers_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 0,
                                 start_current_next_protocol_enable_main_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 1,
                                 start_current_next_protocol_enable_add_header_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 2,
                                 start_current_next_protocol_enable_add_header_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL_ENABLE, 3,
                                 start_current_next_protocol_enable_add_header_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 0,
                                 start_current_next_protocol_main_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 1,
                                 start_current_next_protocol_add_header_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 2,
                                 start_current_next_protocol_add_header_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 3,
                                 start_current_next_protocol_add_header_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 0,
                                 additional_headers_packet_size_enable_main_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 1,
                                 additional_headers_packet_size_enable_add_header_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 2,
                                 additional_headers_packet_size_enable_add_header_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE_ENABLE, 3,
                                 additional_headers_packet_size_enable_add_header_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 0,
                                 additional_headers_packet_size_main_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 1,
                                 additional_headers_packet_size_add_header_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 2,
                                 additional_headers_packet_size_add_header_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDITIONAL_HEADERS_PACKET_SIZE, 3,
                                 additional_headers_packet_size_add_header_2);
    dbal_entry_value_field64_set(unit, entry_handle_id, protocol_specific_information_add_header_0_type, 0,
                                 protocol_specific_information_add_header_0);
    dbal_entry_value_field64_set(unit, entry_handle_id, protocol_specific_information_add_header_1_type, 1,
                                 protocol_specific_information_add_header_1);
    dbal_entry_value_field64_set(unit, entry_handle_id, protocol_specific_information_add_header_2_type, 2,
                                 protocol_specific_information_add_header_2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_additional_headers_map_tables_configuration_init(
    int unit)
{

    int nof_additional_headers_map_tables_entries;
    int entry;
    COMPILER_UINT64 esi_protocol_specific_info;
    COMPILER_UINT64 el_protocol_specific_info;
    COMPILER_UINT64 eli_protocol_specific_info;
    COMPILER_UINT64 protocol_specific_information_header_1;
    COMPILER_UINT64 protocol_specific_information_header_2;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(esi_protocol_specific_info);
    COMPILER_64_ZERO(el_protocol_specific_info);
    COMPILER_64_ZERO(eli_protocol_specific_info);

    /** Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1 */
    egress_mpls_protocol_header_specific_information_get(2, 0, 1, 0, 0, 1, &esi_protocol_specific_info);
    /** Label Type=0, EXP Type=1, EXP=0, TTL Type=0, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(0, 0, 1, 0, 0, 0, &el_protocol_specific_info);
    /** Label Type=1, Label=7, EXP Type=1, EXP=0, TTL Type=1, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(1, 7, 1, 0, 1, 0, &eli_protocol_specific_info);

    /*
     * Initialize entries according to table
     */
    nof_additional_headers_map_tables_entries = sizeof(additional_headers_map) / sizeof(additional_headers_map_t);

    for (entry = 0; entry < nof_additional_headers_map_tables_entries; entry++)
    {
        protocol_specific_information_header_1 =
            additional_headers_map[entry].protocol_specific_information_add_header_1;
        protocol_specific_information_header_2 =
            additional_headers_map[entry].protocol_specific_information_add_header_2;

        if (additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_CW_ESI_EVPN ||
            additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_ESI_EVPN)
        {
            protocol_specific_information_header_2 = esi_protocol_specific_info;
        }
        else if (additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_CW_FL_ESI_EVPN ||
                 additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_FL_ESI_EVPN)
        {
            protocol_specific_information_header_1 = el_protocol_specific_info;
            protocol_specific_information_header_2 = esi_protocol_specific_info;
        }
        else if (additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_CW_EL_ELI_PWE ||
                 additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_EL_ELI)
        {
            protocol_specific_information_header_1 = el_protocol_specific_info;
            protocol_specific_information_header_2 = eli_protocol_specific_info;
        }
        else if (additional_headers_map[entry].additional_headers_profile == ADDITIONAL_HEADERS_FL_PWE)
        {
            protocol_specific_information_header_2 = el_protocol_specific_info;
        }

        /*
         * Write to EGRESS_ADDITIONAL_HEADERS_MAP_TABLE table
         */
        SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                        (unit, additional_headers_map[entry].additional_headers_profile,
                         additional_headers_map[entry].start_current_next_protocol_enable_main_header,
                         additional_headers_map[entry].start_current_next_protocol_main_header,
                         additional_headers_map[entry].additional_headers_packet_size_enable_main_header,
                         additional_headers_map[entry].additional_headers_packet_size_main_header,
                         additional_headers_map[entry].start_current_next_protocol_enable_add_header_0,
                         additional_headers_map[entry].start_current_next_protocol_add_header_0,
                         additional_headers_map[entry].additional_headers_packet_size_enable_add_header_0,
                         additional_headers_map[entry].additional_headers_packet_size_add_header_0,
                         additional_headers_map[entry].protocol_specific_information_add_header_0,
                         additional_headers_map[entry].protocol_specific_information_add_header_0_type,
                         additional_headers_map[entry].start_current_next_protocol_enable_add_header_1,
                         additional_headers_map[entry].start_current_next_protocol_add_header_1,
                         additional_headers_map[entry].additional_headers_packet_size_enable_add_header_1,
                         additional_headers_map[entry].additional_headers_packet_size_add_header_1,
                         protocol_specific_information_header_1,
                         additional_headers_map[entry].protocol_specific_information_add_header_1_type,
                         additional_headers_map[entry].start_current_next_protocol_enable_add_header_2,
                         additional_headers_map[entry].start_current_next_protocol_add_header_2,
                         additional_headers_map[entry].additional_headers_packet_size_enable_add_header_2,
                         additional_headers_map[entry].additional_headers_packet_size_add_header_2,
                         protocol_specific_information_header_2,
                         additional_headers_map[entry].protocol_specific_information_add_header_2_type));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
egress_additional_headers_mpls_control_word_set(
    int unit,
    int cw)
{
    int flags_bits_lsb = 0, flags_bits_val = 0, nof_ahp_entries;
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e header;
    int protocol_specific_information_add_header;
    COMPILER_UINT64 esi_protocol_specific_info;
    COMPILER_UINT64 el_protocol_specific_info;
    COMPILER_UINT64 eli_protocol_specific_info;
    COMPILER_UINT64 work_reg_64;
    SHR_FUNC_INIT_VARS(unit);

    flags_bits_lsb = dnx_data_mpls.general.mpls_control_word_flag_bits_lsb_get(unit);
    flags_bits_val = (cw >> flags_bits_lsb) & UTILEX_BITS_MASK(MPLS_CW_FLAG_BITS_WIDTH - 1, 0);
    protocol_specific_information_add_header = MPLS_CW_SPECIFIC_INFORMATION(flags_bits_val, 0);

    COMPILER_64_ZERO(esi_protocol_specific_info);
    COMPILER_64_ZERO(el_protocol_specific_info);
    COMPILER_64_ZERO(eli_protocol_specific_info);

    /** Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1 */
    egress_mpls_protocol_header_specific_information_get(2, 0, 1, 0, 0, 1, &esi_protocol_specific_info);
    /** Label Type=0, EXP Type=1, EXP=0, TTL Type=0, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(0, 0, 1, 0, 0, 0, &el_protocol_specific_info);
    /** Label Type=1, Label=7, EXP Type=1, EXP=0, TTL Type=1, TTL=0 */
    egress_mpls_protocol_header_specific_information_get(1, 7, 1, 0, 1, 0, &eli_protocol_specific_info);

    nof_ahp_entries = sizeof(additional_headers_map) / sizeof(additional_headers_map_t);

    /*
     * Update the Flag bits in control word,which
     * is from addtional header profile
     */
    for (header = 0; header < nof_ahp_entries; header++)
    {
        if (additional_headers_map[header].additional_headers_profile == ADDITIONAL_HEADERS_CW_FL_ESI_EVPN)
        {
            /** ADDITIONAL_HEADERS_CW_FL_ESI_EVPN*/
            COMPILER_64_SET(work_reg_64, 0, protocol_specific_information_add_header);
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[header].additional_headers_profile,
                             additional_headers_map[header].start_current_next_protocol_enable_main_header,
                             additional_headers_map[header].start_current_next_protocol_main_header,
                             additional_headers_map[header].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[header].additional_headers_packet_size_main_header,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[header].start_current_next_protocol_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_add_header_0,
                             work_reg_64,
                             additional_headers_map[header].protocol_specific_information_add_header_0_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[header].start_current_next_protocol_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_add_header_1,
                             el_protocol_specific_info,
                             additional_headers_map[header].protocol_specific_information_add_header_1_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[header].start_current_next_protocol_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_add_header_2,
                             esi_protocol_specific_info,
                             additional_headers_map[header].protocol_specific_information_add_header_2_type));
        }
        else if (additional_headers_map[header].additional_headers_profile == ADDITIONAL_HEADERS_CW_EL_ELI_PWE)
        {
            /** ADDITIONAL_HEADERS_CW_EL_ELI_PWE*/
            COMPILER_64_SET(work_reg_64, 0, protocol_specific_information_add_header);
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[header].additional_headers_profile,
                             additional_headers_map[header].start_current_next_protocol_enable_main_header,
                             additional_headers_map[header].start_current_next_protocol_main_header,
                             additional_headers_map[header].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[header].additional_headers_packet_size_main_header,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[header].start_current_next_protocol_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_add_header_0,
                             work_reg_64,
                             additional_headers_map[header].protocol_specific_information_add_header_0_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[header].start_current_next_protocol_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_add_header_1,
                             el_protocol_specific_info,
                             additional_headers_map[header].protocol_specific_information_add_header_1_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[header].start_current_next_protocol_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_add_header_2,
                             eli_protocol_specific_info,
                             additional_headers_map[header].protocol_specific_information_add_header_2_type));
        }
        else if (additional_headers_map[header].additional_headers_profile == ADDITIONAL_HEADERS_CW_FL_PWE)
        {
            /** ADDITIONAL_HEADERS_CW_FL_PWE*/
            COMPILER_64_SET(work_reg_64, 0, protocol_specific_information_add_header);
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[header].additional_headers_profile,
                             additional_headers_map[header].start_current_next_protocol_enable_main_header,
                             additional_headers_map[header].start_current_next_protocol_main_header,
                             additional_headers_map[header].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[header].additional_headers_packet_size_main_header,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[header].start_current_next_protocol_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[header].start_current_next_protocol_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_add_header_1,
                             work_reg_64,
                             additional_headers_map[header].protocol_specific_information_add_header_1_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[header].start_current_next_protocol_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_add_header_2,
                             additional_headers_map[header].protocol_specific_information_add_header_2,
                             additional_headers_map[header].protocol_specific_information_add_header_2_type));
        }
        else if (additional_headers_map[header].additional_headers_profile == ADDITIONAL_HEADERS_CW_ESI_EVPN)
        {
            /** ADDITIONAL_HEADERS_CW_ESI_EVPN*/
            COMPILER_64_SET(work_reg_64, 0, protocol_specific_information_add_header);
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[header].additional_headers_profile,
                             additional_headers_map[header].start_current_next_protocol_enable_main_header,
                             additional_headers_map[header].start_current_next_protocol_main_header,
                             additional_headers_map[header].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[header].additional_headers_packet_size_main_header,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[header].start_current_next_protocol_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[header].start_current_next_protocol_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_add_header_1,
                             work_reg_64,
                             additional_headers_map[header].protocol_specific_information_add_header_1_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[header].start_current_next_protocol_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_add_header_2,
                             esi_protocol_specific_info,
                             additional_headers_map[header].protocol_specific_information_add_header_2_type));
        }
        else if (additional_headers_map[header].additional_headers_profile == ADDITIONAL_HEADERS_CW_PWE)
        {
            /** ADDITIONAL_HEADERS_CW_PWE*/
            COMPILER_64_SET(work_reg_64, 0, protocol_specific_information_add_header);
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[header].additional_headers_profile,
                             additional_headers_map[header].start_current_next_protocol_enable_main_header,
                             additional_headers_map[header].start_current_next_protocol_main_header,
                             additional_headers_map[header].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[header].additional_headers_packet_size_main_header,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[header].start_current_next_protocol_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[header].additional_headers_packet_size_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0,
                             additional_headers_map[header].protocol_specific_information_add_header_0_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[header].start_current_next_protocol_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[header].additional_headers_packet_size_add_header_1,
                             additional_headers_map[header].protocol_specific_information_add_header_1,
                             additional_headers_map[header].protocol_specific_information_add_header_1_type,
                             additional_headers_map[header].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[header].start_current_next_protocol_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[header].additional_headers_packet_size_add_header_2,
                             work_reg_64,
                             additional_headers_map[header].protocol_specific_information_add_header_2_type));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Configure initial values for Port Termination
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
static shr_error_e
ingress_prt_configuration_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ingress_prt_recycle_command_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_prt_profile_configuration_init(unit));

exit:
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_layer_protocol_allow_speculative_learning(
    int unit,
    int layer_entry,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_entry);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECULATIVE_PROTOCOL_ETH_ENABLE, INST_SINGLE,
                                 enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_per_layer_protocol_configuration(
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
    int is_bier_mpls)
{

    uint32 entry_handle_id;
    int system_headers_mode;
    int allow_speculative_learning;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Write default values to DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_ENABLE, INST_SINGLE, is_ethernet);

    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
        (!dnx_data_headers.
         general.feature_get(unit, dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE,
                                     is_ethernet);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ARP_ENABLE, INST_SINGLE, is_arp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IP_ENABLE, INST_SINGLE,
                                 (is_ipv4 | is_ipv6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_TERM_ENABLE, INST_SINGLE,
                                 is_mpls_term);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_FWD_ENABLE, INST_SINGLE, is_mpls_fwd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_SINGLE_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_single_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_MULTI_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_multi_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMPV6_ENABLE, INST_SINGLE, is_icmpv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IGMP_ENABLE, INST_SINGLE, is_igmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_8021_X_2_ENABLE, INST_SINGLE, is_8021_x_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMP_ENABLE, INST_SINGLE, is_icmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BIER_MPLS_ENABLE, INST_SINGLE,
                                 is_bier_mpls);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (!dnx_data_mpls.general.mpls_speculative_learning_handling_get(unit))
    {
        allow_speculative_learning = 1;
    }
    else
    {
        allow_speculative_learning = is_ethernet;
    }
    SHR_IF_ERR_EXIT(ingress_layer_protocol_allow_speculative_learning
                    (unit, layer_type_enum_val, allow_speculative_learning));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ipv4,
    int is_ipv6,
    int is_mpls,
    int is_oam,
    int is_tcp,
    int is_udp)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_ENABLE, INST_SINGLE, is_mpls);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_OAM_ENABLE, INST_SINGLE, is_oam);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_TCP_ENABLE, INST_SINGLE, is_tcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_FHEI_MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING
 * \param [in] unit - Relevant unit.
 * \param [in] layer_type - layer_type.
 * \param [in] parser_context - parser_context.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_configure(
    int unit,
    dbal_enum_value_field_layer_types_e layer_type,
    dbal_enum_value_field_erpp_1st_parser_parser_context_e parser_context)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_CONTEXT, INST_SINGLE, parser_context);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL
 * \param [in] unit - Relevant unit.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ingress_egress_per_layer_protocol_configuration_init(
    int unit)
{
    int entry;
    int nof_layres_to_config;
    uint32 ingress_layer_check[DBAL_NOF_ENUM_LAYER_TYPES_VALUES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    nof_layres_to_config = sizeof(per_layer_protocol) / sizeof(per_layer_protocol_t);
    /*
     * Initialize entries according to the table
     */
    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        /*
         * Write to the indexes of INGRESS_PER_LAYER_PROTOCOL table
         */
        SHR_IF_ERR_EXIT(ingress_per_layer_protocol_configuration(unit, per_layer_protocol[entry].layer_type_enum_val,
                                                                 per_layer_protocol[entry].is_ethernet,
                                                                 per_layer_protocol[entry].is_arp,
                                                                 per_layer_protocol[entry].is_ipv4,
                                                                 per_layer_protocol[entry].is_ipv6,
                                                                 per_layer_protocol[entry].is_mpls_term,
                                                                 per_layer_protocol[entry].is_mpls_fwd,
                                                                 per_layer_protocol[entry].is_udp,
                                                                 per_layer_protocol[entry].is_bfd_single_hop,
                                                                 per_layer_protocol[entry].is_bfd_multi_hop,
                                                                 per_layer_protocol[entry].is_icmpv6,
                                                                 per_layer_protocol[entry].is_igmp,
                                                                 per_layer_protocol[entry].is_8021_x_2,
                                                                 per_layer_protocol[entry].is_icmp,
                                                                 per_layer_protocol[entry].is_bier_mpls));
        ingress_layer_check[per_layer_protocol[entry].layer_type_enum_val] = 1;
    }

    if (!dnx_data_mpls.general.mpls_speculative_learning_handling_get(unit))
    {
        for (entry = 0; entry < DBAL_NOF_ENUM_LAYER_TYPES_VALUES; entry++)
        {
            if (ingress_layer_check[entry] == 0)
            {
                SHR_IF_ERR_EXIT(ingress_layer_protocol_allow_speculative_learning(unit, entry, 1));
            }
        }
    }

    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        /*
         * Write to the indexes of EGRESS_PER_LAYER_PROTOCOL table
         */
        SHR_IF_ERR_EXIT(egress_per_layer_protocol_configuration(unit, per_layer_protocol[entry].layer_type_enum_val,
                                                                per_layer_protocol[entry].is_ipv4,
                                                                per_layer_protocol[entry].is_ipv6,
                                                                per_layer_protocol[entry].is_mpls_term,
                                                                per_layer_protocol[entry].is_oam,
                                                                per_layer_protocol[entry].is_tcp,
                                                                per_layer_protocol[entry].is_udp));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_map_parsing_ctx_layer_potocol(
    int unit)
{
    int ii = 0;
    uint32 entry_handle_id;
    int nof_ctx_to_config;
    int mpls_second_stage_parser_handling;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mpls_second_stage_parser_handling = dnx_data_mpls.general.mpls_second_stage_parser_handling_get(unit);

    nof_ctx_to_config = sizeof(per_2nd_stage_parsing_ctx) / sizeof(per_2nd_stage_parsing_ctx_t);

    /*
     * Write default values to INGRESS_MAP_PARSING_CTX_LAYER_PROTOCOL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_MAP_PARSING_CTX_LAYER_PROTOCOL, &entry_handle_id));
    for (ii = 0; ii < nof_ctx_to_config; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_2ND_PARSER_PARSER_CONTEXT,
                                   per_2nd_stage_parsing_ctx[ii].parser_ctx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES_SPECULATION, INST_SINGLE,
                                     per_2nd_stage_parsing_ctx[ii].layer_type_speculation);
        if (!mpls_second_stage_parser_handling)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES_FORCE_INCORRECT, INST_SINGLE,
                                         per_2nd_stage_parsing_ctx[ii].layer_type_force_incorrect);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES_FORCE_INCORRECT, INST_SINGLE,
                                         per_2nd_stage_parsing_ctx[ii].layer_type_speculation);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL
 * \param [in] unit - Relevant unit.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_init(
    int unit)
{

    int entry;
    int nof_layres_to_config;

    SHR_FUNC_INIT_VARS(unit);

    nof_layres_to_config =
        sizeof(layer_type_to_mpls_upper_layer_protocol) / sizeof(layer_type_to_mpls_upper_layer_protocol_t);

    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        /*
         * Write to the indexes of MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_configure
                        (unit, layer_type_to_mpls_upper_layer_protocol[entry].layer_type,
                         layer_type_to_mpls_upper_layer_protocol[entry].parser_context));
    }
exit:
    SHR_FUNC_EXIT;
}
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
static shr_error_e
egress_eedb_type_buster_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ESEM_GLOBAL, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {
        /*
         * Initialize entries according to table Start of 1 to skip the NONE value
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, 1, DBAL_RANGE_ALL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EM_BUSTER_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Configure initial values for Number of RIFs,
*    Local Outlif Size and Local Outlif Profile Size.
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
static shr_error_e
egress_global_eedb_configuration_init(
    int unit)
{
    uint32 entry_handle_id;
    int nof_rifs;
    int local_outlif_width;
    int outlif_profile_width, system_headers_mode, iop_mode_outlif_selection;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the maximum number of RIFs from dnx_data */
    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);

    /** Get the local outlif and local outlif profile widths*/
    local_outlif_width = dnx_data_lif.out_lif.local_outlif_width_get(unit);
    outlif_profile_width = dnx_data_lif.out_lif.outlif_profile_width_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    iop_mode_outlif_selection = dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_outlif_selection);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_GLOBAL, &entry_handle_id));
    /*
     * Set the maximum number of RIFs
     */

    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ||
        (iop_mode_outlif_selection == FALSE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_RIFS, INST_SINGLE, nof_rifs);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_RIFS, INST_SINGLE, 0);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUTLIF_SIZE_IN_BITS, INST_SINGLE,
                                 local_outlif_width);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_PROFILE_SIZE_IN_BITS, INST_SINGLE,
                                 outlif_profile_width);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_cfg_in_lif_null_value_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 egress_in_lif_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    egress_in_lif_val = dnx_data_lif.global_lif.egress_in_lif_null_value_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_IN_LIF_NULL_VALUE, INST_SINGLE,
                                 egress_in_lif_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function sets the EGW full threshold in order to enable FLP FIFO for KBP traffic.
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
static shr_error_e
ingress_egw_full_threshold_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_EGW_FULL_THRESHOLD, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLP_FULL_THRESHOLD, INST_SINGLE,
                                 dnx_data_elk.general.flp_full_treshold_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function enables traffic. It configures DATA_PATH_ENABLE.
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
static shr_error_e
ingress_ippe_traffic_enable(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Configure DATA_PATH_ENABLE so that to enable traffic on init */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GLOBAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_PATH_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_ippe_parser_context_mapping(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ii = 0, hw_context_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Configure ingress parsing context 1-to-1 mapping for ITMH_A1, ETH_A1 and RCH_A1 */
    for (ii = 0; ii < DBAL_NOF_ENUM_IRPP_1ST_PARSER_PARSER_CONTEXT_VALUES; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, ii, &hw_context_val));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_PARSING_CONTEXT_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_PARSING_CONTEXT, hw_context_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INST_SINGLE, ii);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_fallback_to_bridge_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 ipvx_fallback_to_bridge_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_FORWARDING_ADDITIONAL_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, fwd_additional_info);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IPMC_ELIGIBLE_MAPPING, INST_SINGLE,
                                ipvx_fallback_to_bridge_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure EGRESS_FORWARDING_ADDITIONAL_INFO with Forwarding layer plus one
 *          header remarking profile.
 * \param [in] unit - Relevant unit
 * \param [in] fwd_additional_info - Relevant forwarding additional info
 * \param [in] fwd_plus_1_remark_ingress_profile -
 *             DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_0:
 *                  Disable Forwarding layer plus one header remarking.
 *             DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_1:
 *                  Enable Forwarding layer plus one header remarking.
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
egress_fwd_plus_1_remark_ingress_profile_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 fwd_plus_1_remark_ingress_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_FORWARDING_ADDITIONAL_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, fwd_additional_info);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_REMARK_INGRESS_PROFILE, INST_SINGLE,
                                fwd_plus_1_remark_ingress_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure INGRESS_FORWARDING_ADDITIONAL_INFO.
 *          Configure Unknown Address enable (BUM traffic).
 * \param [in] unit - Relevant unit
 * \param [in] fwd_additional_info - Relevant forwarding additional info
 * \param [in] bum_traffic_enable - '1' means BUM traffic (unknown address) enable
 *                                      '0' means BUM traffic (unknown address) disable
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
ingress_forwarding_additional_info_to_bum_traffic_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 bum_traffic_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_FORWARDING_ADDITIONAL_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, fwd_additional_info);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_BUM_TRAFFIC_ENABLE, INST_SINGLE, bum_traffic_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Initialize EGRESS_FORWARDING_ADDITIONAL_INFO according to FAI
 * and INGRESS_ADDITIONAL_INFO_TO_UNKNOWN_ADDRESS_MAP according to FAI
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
static shr_error_e
ingress_egress_forwarding_additional_info_configuration_init(
    int unit)
{
    uint32 nof_map_tables_entries;
    uint32 entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_map_tables_entries =
        sizeof(forwarding_additional_info_config_map) / sizeof(forwarding_additional_info_config_t);

    for (entry = 0; entry < nof_map_tables_entries; entry++)
    {
        SHR_IF_ERR_EXIT(egress_fallback_to_bridge_configuration(unit,
                                                                forwarding_additional_info_config_map
                                                                [entry].forwarding_additional_info,
                                                                forwarding_additional_info_config_map
                                                                [entry].ipvx_fallback_to_bridge_enable));

        SHR_IF_ERR_EXIT(ingress_forwarding_additional_info_to_bum_traffic_configuration(unit,
                                                                                        forwarding_additional_info_config_map
                                                                                        [entry].forwarding_additional_info,
                                                                                        forwarding_additional_info_config_map
                                                                                        [entry].bum_traffic_enable));
        SHR_IF_ERR_EXIT(egress_fwd_plus_1_remark_ingress_profile_configuration
                        (unit, forwarding_additional_info_config_map[entry].forwarding_additional_info,
                         forwarding_additional_info_config_map[entry].fwd_plus_1_remark_ingress_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_exclude_source_filter_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Configure ERPP_EXCLUDE_SOURCE according to all possible values for forwarding strength in order to enable exclude source filter */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_EXCLUDE_SOURCE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXCLUDE_SOURCE_MAPPED, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
     * Set default values in SAME_INTERFACE_FILTER table according to jericho mode.
 * \param [in] unit - Unit
 * \return shr_error_e Standard error handling
 */
static shr_error_e
egress_same_interface_filter_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode, same_interface_bits, orientation;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    same_interface_bits = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    orientation = dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_SAME_INTERFACE_FILTER, &entry_handle_id));

    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        /**
         * Enabling the same interface per  the following configuration:
         * Ports-Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
         * In this case the address to the memory is indexed as follows:
         * SAME_IF_MAPPED_BY_INLIF_PROFILE = (Device scope), SYS_PORT_EQUAL = 1, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
         * Position of the enable bit is:
         * LIFS_EQUAL = 1, SAME_IF_MAPPED_BY_OUTLIF_PROFILE = ALL, SAME_IF_FILTER_MAPPED_BY_FWD_CONTEXT = 2'b01.
         * When the Forwarding Context is Ethernet or MPLS - the same interface is enabled.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_LIF_PROFILE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** Device scope filtering for IOP mode, conditions are different only in respect to the soc properties chosen*/
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        /** Device Scope filtering when both Device and System Scope in case both modes are enabled with a soc property */
        if (same_interface_bits == JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
        {
            /** Only Device Scope is configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       DEVICE_SCOPE_LIF_PROFILE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Both Device and System Scope are configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_BOTH_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        /** Device Scope filtering when only System Scope is enabled with a soc property.*/
        else if (same_interface_bits == JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
        {
            /** Only Device Scope is configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       DEVICE_SCOPE_LIF_PROFILE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Only System Scope are configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        else if ((same_interface_bits == JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES) && (orientation > 0))
        {
            /** Only Device Scope is configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       DEVICE_SCOPE_LIF_PROFILE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Only Device Scope is configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       DEVICE_SCOPE_ENABLED_AND_ORIENTATION_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        else
        {
            /** Device Scope filtering in case enabled with bcm_port_control_set API*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       DEVICE_SCOPE_LIF_PROFILE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-Equal && Out-LIF-is-Port && In-LIF-is-Port
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = (Device scope - default), SYS_PORT_EQUAL = 1, INLIF_IS_PORT = 1, OUTLIF_IS_PORT = 1
     * Position of the enable bit is:
     * LIFS_EQUAL = 1, SAME_IF_MAPPED_BY_OUTLIF_PROFILE = ALL, SAME_IF_FILTER_MAPPED_BY_FWD_CONTEXT = 2'b01.
     * When the Forwarding Context is Ethernet or MPLS - the same interface is enabled.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                               DEVICE_SCOPE_LIF_PROFILE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-are not Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = System Scope, SYS_PORT_EQUAL = 0, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
     * Position of the enable bit is:
     * LIFS_EQUAL = 1, SAME_IF_MAPPED_BY_OUTLIF_PROFILE = 2'b00, SAME_IF_FILTER_MAPPED_BY_FWD_CONTEXT = 2'b01.
     * When the Forwarding Context is Ethernet or MPLS - the same interface is enabled.
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   SYSTEM_SCOPE_LIF_PROFILE);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** System Scope configuration in IOP mode*/
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        /** System Scope filtering when both Device and System Scope in case both modes are enabled with a soc property */
        if (same_interface_bits == JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
        {
            /** Only System Scope is configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Both Device and System Scope are configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_BOTH_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        /** System Scope filtering when only System Scope is enabled with a soc property.*/
        else if (same_interface_bits == JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
        {
            /** Only System Scope are configured as enabled*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            if (orientation > 0)
            {
                /** Only Device Scope is configured as enabled*/
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                           SYSTEM_SCOPE_ENABLED_AND_ORIENTATION_ENABLED);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
     * Configure the VTT termination table so that termination will occur when routing is enabled and
     * when either we have a my-mac or compatible-MC.
     * A trap will be enabled when my-mac is found, but there's no routing-enable.
     * Both enablers are not dependent on the values of PTC-Profile or the Context-L3-LIF notion.
 * \param [in] unit - Unit
 * \return shr_error_e Standard error handling
 */
static shr_error_e
ingress_eth_termination_mapping_table_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ETHERNET_TERMINATION_ACTION, &entry_handle_id));
    /*
     * Set termination when Routing-Enable and My-MAC
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, TRUE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set termination when Routing-Enable and Compatible-MC
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, FALSE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set termination when Routing-Enable and both My-MAC and Compatible-MC
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, TRUE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set termination when Routing-enable and is-context-l3-sel, and not my-mac.
     * For ip tunnel termination application.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, TRUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, TRUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set trap enable when no Routing-Enable and My-MAC is on
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, TRUE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS_TERMINATION_TRAP_ENABLE, INST_SINGLE,
                                 TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set termination when routing-Enable and ptc_routing_enable_profile is ALWAYSTERMINATE
     * 1.For RCH termination, in drop and continue application, at 2nd pass
     * ptc profile value is set by bcm_port_control_set with type bcmPortControlOverlayRecycle
     * 2. For RAW MPLS
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1,
                               DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_ALWAYSTERMINATE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS_TERMINATION_TRAP_ENABLE, INST_SINGLE,
                                 FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Update termination disable when vtt2 NEVER_TERMINATION is enabled */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    {
        uint32 ptc_profile_never_terminate;
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1,
                         DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_NEVERTERMINATE, &ptc_profile_never_terminate));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_ROUTING_ENABLE_PROFILE_VTT1,
                                   ptc_profile_never_terminate);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS_TERMINATION_TRAP_ENABLE, INST_SINGLE,
                                 FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
     * Set default values for forwarding context mapping for same interface filter.
     * Same interface is enabled for ETHERNET and MPLS forwarding.
 * \param [in] unit - Unit
 * \return shr_error_e Standard error handling
 */
static shr_error_e
egress_mapping_forward_context_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_egress_fwd_code_e forward_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_FWD_CONTEXT, &entry_handle_id));

    for (forward_context = 0; forward_context < DBAL_NOF_ENUM_EGRESS_FWD_CODE_VALUES; forward_context++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE, forward_context);
        if ((forward_context == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET)
            || (forward_context == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER,
                                         INST_SINGLE, TRUE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER,
                                         INST_SINGLE, FALSE);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
egress_ingress_trapped_by_fhei_configure_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_INGRESS_TRAPPED_BY_FHEI, &entry_handle_id));
    /** Enable for all FHEI Types*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, DBAL_ENUM_FVAL_FHEI_TYPE_TRAP_SNOOP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init erpp_filter_per_fwd_context_map table.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
egress_erpp_filter_per_fwd_context_map_init(
    int unit)
{

    int system_headers_mode;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * JR mode, egress always resolve TTL as 0 for bridge service,
     * so shut down this check
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_FWD_CONTEXT_FILTER, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE,
                                   DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_EQ_0, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FTMH_LB_KEY_EXT_SIZE  and FTMH_STACKING_EXT_SIZE in SYSTEM_HEADER_GLOBAL_CFG table
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
static shr_error_e
egress_global_system_header_configuration_init(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode;
    uint32 max_vsi_value;
    uint32 jr_lb_key_ext_size_bytes = 0;
    uint32 jr_stacking_ext_size_bytes = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Write to SYSTEM_HEADER_GLOBAL_CFG table
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));

    /*
     * Initiate the mim-vsi as invalid value
     */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, (int *) &max_vsi_value));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_IN_MAC_VSI, INST_SINGLE, max_vsi_value + 1);

    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                                     FTMH_LB_KEY_EXT_SIZE);

        /** Write default stacking extension size in bytes */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                                     STACKING_EXT_SIZE);
    }
    else
    {
        /** Set size of FTMH LB-Key Extension, 0 indicates FTMH LB-Key Extension is not present */
        jr_lb_key_ext_size_bytes = dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get(unit) ? 1 : 0;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                                     jr_lb_key_ext_size_bytes);

        /** Set size of FTMH Stacking Extension, 0 indicates FTMH Stacking Extension is not present*/
        jr_stacking_ext_size_bytes = dnx_data_headers.system_headers.jr_mode_ftmh_stacking_ext_mode_get(unit) ? 2 : 0;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                                     jr_stacking_ext_size_bytes);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
ingress_per_trap_context_configuration_init(
    int unit)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_TRAP_CONTEXT_ENABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Enable ICMP_REDIRECT, UC_LOOSE_RPF, UC_STRICT_RPF, FACILITY_INVALID for IPV4 Private UC context in FWD2 stage*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                 DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_LOOSE_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_STRICT_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_INVALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Enable ICMP_REDIRECT, UC_LOOSE_RPF, UC_STRICT_RPF, FACILITY_INVALID for IPV6 Private UC context in FWD2 stage*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                 DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_LOOSE_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_STRICT_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_INVALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_opportunistic_learning_always_transplant))
    {
        /** Enable ICMP_REDIRECT, UC_LOOSE_RPF, UC_STRICT_RPF, FACILITY_INVALID for IPV4 Private UC LPM only context in FWD2 stage*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                     DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_LOOSE_RPF, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_STRICT_RPF, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_INVALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Enable ICMP_REDIRECT, UC_LOOSE_RPF, UC_STRICT_RPF, FACILITY_INVALID for IPV6 Private UC LPM only context in FWD2 stage*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                     DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_LOOSE_RPF, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UC_STRICT_RPF, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FACILITY_INVALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    /** Enable ICMP_REDIRECT, MC_EXPLICIT_RPF and MC_SIP_BASED_RPF for IPV4 Private MC context in FWD2 stage*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                 DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_EXPLICIT_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_EXPLICIT_RPF_ECMP, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_SIP_BASED_RPF, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Enable ICMP_REDIRECT, MC_EXPLICIT_RPF and MC_SIP_BASED_RPF for IPV6 Private MC context in FWD2 stage*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, INST_SINGLE,
                                 DNX_RX_TRAP_SAME_INTERFACE_CONTEXT_PROFILE_IP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_EXPLICIT_RPF, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_EXPLICIT_RPF_ECMP, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_SIP_BASED_RPF, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_stat_fec_ecmp_mapping_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP table - take stat_id mapping from stat_id on fecs, ecmps
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FEC_STAT_OBJECT_PROFILE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FEC_STATISTIC_PROFILE_MAP_TWO_INDIRECT_COUNTER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECMP_STATISTIC_PROFILE_MAP_ECMP_GROUP);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ire_packet_generator_init(
    int unit)
{
    uint8 is_ire_packet_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ire_packet_generator_info.is_init(unit, &is_ire_packet_init));
    if (!is_ire_packet_init)
    {
        SHR_IF_ERR_EXIT(ire_packet_generator_info.init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

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
static shr_error_e
ingress_ippb_l4_protocols_config(
    int unit)
{
    uint32 entry_handle_id;
    int ipv_tcp_protocol = 6;
    int ipv_udp_protocol = 17;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to L_3_PROTOCOL_L_4_VALUES table, setting the device recognizable TCP and UDP protocol values for IPv4 header.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_PROTOCOL_L4_VALUES, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_TCP_PROTOCL, INST_SINGLE, ipv_tcp_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_UDP_PROTOCL, INST_SINGLE, ipv_udp_protocol);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** \param [in] unit - Relevant unit.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
bare_metal_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Currently supporting single parser, thus full network headers should be sent to Egress */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRAP_INGRESS_FWD_ACTION_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_INDEX_VALUE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_INDEX_OVERWRITE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * configure ECOLogic for SRV6 segment endpoint.
 * Cut SIDSs in SRv6 packets so that we can access the SID
 * to swap
 * It calculates BTK_OFFSET_IN_BYTES = end_of_SRv6 offset + 0x10
 *               BTC_OFFSET_IN_BYTES = end of SRv6 offset - 0x10
 *               + (SRv6 segment lefts - 0x1) << 4
 *
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
static shr_error_e
ingress_ecologic_srv6_segment_endpoint_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    ecol_ffc_t ecol_ffc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&ecol_ffc, 0, sizeof(ecol_ffc_t));
    /** configure SRV6 segment endpoint */

    /*
     * Configure context selection
     */

    /** set access id (context_id in ECOL_IDENTIFICATION_CAM table
     *  We'll use the hw value of dbal
     * enum field type: DBAL_FIELD_TYPE_DEF_ECOL_CONTEXT_ID. We
     * access the field type using
     * DBAL_FIELD_ECOL_CONTEXT_ID field   */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_SEGMENT_ENDPOINT,
                     &access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier_mask, TRUE);

    layer_qualifier = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier, FALSE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier, FALSE);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure context attributes
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_SEGMENT_ENDPOINT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    /*
     * FFC instruction 0 and 1 are 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE, 0);
    /*
     * access SRv6 header: segment left field
     */
    /*
     * Header size: 144*8b
     * Msb offset SRV6 segment lefts: 3*8b
     * lsb offset segment lefts: header size - Msb offset SRV6 segment lefts - segment left size 144*8 - 3*8 - 8:
     * In ecol_ffc_t: op_code, index, field_offset, field size, key offset
     * { 0, 0, (144 * 8 - 3*8 - 8), 7 , 0}
     * op_code: 0: from relative header
     * index: -1 (indicate to take one before relative header)
     * Relative header is IPV4, to SRV6 is relative header -1
     * field offset: relative to current header, where current header is MSB aligned.
     * output field size: field size + 1
     */
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 7;
    ecol_ffc.field_offset = (144 * 8 - 3 * 8 - 8);
    ecol_ffc.field_size = 7;
    ecol_ffc.key_offset = 0;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);
    /*
     * set btc offset shift in bytes = 0x08. In bits: 0 0000 1000
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x08);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_MODE_USE_CAM_TABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_SRC, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_OP_B_SRC_USE_CFG);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_CFG, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_CALCULATION_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_CALCULATION_MODE_OP_A_MINUS_OP_B);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_ADD_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_BTC_OFFSET_ADD_MODE_SHIFT);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x8);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * set header size
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE_CAM, entry_handle_id));

    /** set access id (header_size in ECOL_HEADER_SIZE_CAM table
     * We'll use the hw value of dbal enum field type:
     * DBAL_FIELD_TYPE_DEF_ECOL_HEADER_SIZE_INDEX.
     * We access the field type using
     * DBAL_FIELD_ECOL_HEADER_SIZE_INDEX field   */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                     DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT, &access_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VARIABLE_0, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_SEGMENT_ENDPOINT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE, INST_SINGLE, 4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * configure ECOLogic for SRV6 egress node in 1 pass
 * Cut all SIDSs in SRv6 packets to terminates all SIDs and
 * access inner forwarding header
 * One pass is recognized by a bit on the Srv6 qualifier.
 * It calculates BTK_OFFSET_IN_BYTES = end_of_SRv6 offset
 *               BTC_OFFSET_IN_BYTES = end of SRv6 offset
 *               + (SRv6.ext_len - 0x0) << 3
 *
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
static shr_error_e
ingress_ecologic_srv6_egress_tunnel_1_pass_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    ecol_ffc_t ecol_ffc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** configure SRV6 segment endpoint */

    /*
     * Configure context selection
     */

    /** set access id (context_id in ECOL_IDENTIFICATION_CAM table
     *  We'll use the hw value of dbal
     * enum field type: DBAL_FIELD_TYPE_DEF_ECOL_CONTEXT_ID. We
     * access the field type using
     * DBAL_FIELD_ECOL_CONTEXT_ID field   */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_1_PASS,
                     &access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(&layer_qualifier_mask, TRUE);

    layer_qualifier = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier, FALSE);
    LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(&layer_qualifier, TRUE);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure context attributes
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_1_PASS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    /*
     * FFC instruction 0 and 1 are 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE, 0);
    /*
     * access SRv6 header: segment left field
     */
    /*
     * Header size: 144*8b
     * Msb offset SRV6.ext_len: 8b
     * lsb offset segment lefts: header size - Msb offset SRV6 segment lefts - segment left size 144*8 - 8 - 8:
     * In ecol_ffc_t: op_code, index, field_offset, field size, key offset
     * { 0, 0, (144 * 8 - 8 - 8), 7 , 0}
     * op_code: 0: from relative header
     * index: -1 (indicate to take one before relative header)
     * field offset: relative to current header, where current header is MSB aligned.
     * output field size: field size + 1
     */
    ecol_ffc.opcode = 0;
    ecol_ffc.index = 7;
    ecol_ffc.field_offset = (144 * 8 - 8 - 8);
    ecol_ffc.field_size = 7;
    ecol_ffc.key_offset = 0;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE,
                                 BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);
    /*
     * set btc offset shift in bytes = 0x8. In bits: 0 0000 1000
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x8);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_MODE_USE_CAM_TABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_SRC, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_OP_B_SRC_USE_CFG);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_OP_B_CFG, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_NOF_HEADERS_CALCULATION_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_NOF_HEADERS_CALCULATION_MODE_OP_A_MINUS_OP_B);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_ADD_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_BTC_OFFSET_ADD_MODE_SHIFT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x8);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * set header size
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE_CAM, entry_handle_id));

    /** set access id (header_size in ECOL_HEADER_SIZE_CAM table
     * We'll use the hw value of dbal enum field type:
     * DBAL_FIELD_TYPE_DEF_ECOL_HEADER_SIZE_INDEX.
     * We access the field type using
     * DBAL_FIELD_ECOL_HEADER_SIZE_INDEX field   */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX, DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE,
                     &access_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VARIABLE_0, 0, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_HEADER_SIZE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE_INDEX,
                               DBAL_ENUM_FVAL_ECOL_HEADER_SIZE_INDEX_SRV6_EGRESS_NODE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_HEADER_SIZE, INST_SINGLE, 3);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * configure ECOLogic for SRV6 egress node in 2 passes

 * Cut only MAX_SID of 1_PASS in eco logic
 * The rest will be compensated in ETPP, using ecycle outlif information

 * The limitation is to assure that Eco-logic + fwd layer index are not overflowing
 *   the ITPP termination capabilities, which will lead to a interrupt.

 * access inner forwarding header
 * One pass is recognized by a bit on the Srv6 qualifier, so checking this bit is off
 * It calculates BTK_OFFSET_IN_BYTES = end_of_SRv6 offset
 *               BTC_OFFSET_IN_BYTES = end of SRv6 offset
 *               + max_sids_for_1_pass << 4 (shift 4 as each sid is 16 Bytes)
 *
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
static shr_error_e
ingress_ecologic_srv6_egress_tunnel_2_pass_init(
    int unit)
{
    uint32 access_id;
    uint32 entry_handle_id;
    uint32 layer_qualifier;
    uint32 layer_qualifier_mask;
    uint32 btc_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** configure SRV6 segment endpoint */

    /*
     * Configure context selection
     */

    /** set access id (context_id in ECOL_IDENTIFICATION_CAM table
     *  We'll use the hw value of dbal
     * enum field type: DBAL_FIELD_TYPE_DEF_ECOL_CONTEXT_ID. We
     * access the field type using
     * DBAL_FIELD_ECOL_CONTEXT_ID field   */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ECOL_CONTEXT_ID, DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_2_PASS,
                     &access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECOL_IDENTIFICATION_CAM, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND);
    layer_qualifier_mask = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier_mask, TRUE);
    LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(&layer_qualifier_mask, TRUE);

    layer_qualifier = 0;
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(&layer_qualifier, TRUE);
    LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(&layer_qualifier, FALSE);
    LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(&layer_qualifier, FALSE);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_PREVIOUSLAYER, layer_qualifier,
                                      layer_qualifier_mask);

    
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                                      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER, 0, 0);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_FWD_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_SNP_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_RCY_MRR_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PP_PORT_PROFILE, 0, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_ECOL_PTC_PROFILE, 0, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure context attributes
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECOL_CONTEXT_ATTRIBUTES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_CONTEXT_ID,
                               DBAL_ENUM_FVAL_ECOL_CONTEXT_ID_SRV6_EGRESS_NODE_2_PASS);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    /*
     * FFC instruction 0 and 1 are 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_FFC_INSTRUCTION_2, INST_SINGLE, 0);

    /*
     * BTK - Parsing_layer_index - 1 (offset of the the SRH) + 8, to get to the start of hte SID list
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTK_OFFSET_SHIFT_IN_BYTES, INST_SINGLE, 0x8);

    /*
     * BTC - Parsing_layer_index - 1 (offset of the the SRH) + {8[SRH] + (max_sids_1_pass+1) * 16}
     */
    btc_offset = 0x8 + (16 * (dnx_data_srv6.termination.max_nof_terminated_sids_usp_1pass_get(unit) + 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_BASE_IDX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECOL_OFFSET_BASE_IDX_PARSING_LAYER_MINUS_ONE_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECOL_BTC_OFFSET_SHIFT_IN_BYTES, INST_SINGLE,
                                 btc_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_ecologic_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_segment_endpoint_init(unit));
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_egress_tunnel_1_pass_init(unit));
    SHR_IF_ERR_EXIT(ingress_ecologic_srv6_egress_tunnel_2_pass_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Init force bubble configuration
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
static shr_error_e
dnx_force_bubble_cfg_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_FORCE_BUBBLE_CFG, &entry_handle_id));

    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_bubble_pulse_width_too_short))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_BUBBLES, INST_SINGLE, 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_BUBBLE_IGNORE_PULSE_WIDTH, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNC_COUNTER, INST_SINGLE,
                                 FORCE_BUBBLE_SYNC_COUNTER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_BUBBLE_DELAY, INST_SINGLE,
                                 FORCE_EGRESS_BUBBLE_DELAY);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_FULL_ENABLE, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * See h file for description
 */
shr_error_e
dnx_general_pp_init(
    int unit)
{
    uint8 is_image_standard_1 = FALSE;
    SHR_FUNC_INIT_VARS(unit);

#ifndef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_compare_init) && (!SAL_BOOT_PLISIM))
    {
        soc_reg_above_64_val_t data;
        bcm_core_t core;

        soc_reg_t reg = dnx_data_graphical.diag.counters_get(unit, dnx_graphical_spb, 0)->regs[0];

        SHR_IF_ERR_EXIT(dnx_switch_control_mdb_exact_match_false_hit_set(unit, 0));
        /*
         * Call soft reset to clear counters
         */
        SHR_IF_ERR_EXIT(dnx_drv_soc_dnx_soft_init_no_fabric(unit));
        /** and also read the counters which are not cleared by soft reset */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, core, 0, data));
        }
    }
#endif
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_em_init_done.set(unit, TRUE));

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_pp_jr1_smae_interface_init(unit));
    }
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_image_standard_1));
    SHR_IF_ERR_EXIT(dnx_pp_automatic_tables_init(unit));
    SHR_IF_ERR_EXIT(egress_eedb_type_buster_enable_init(unit));
    SHR_IF_ERR_EXIT(egress_eedb_forwarding_domain_vsd_enable_init(unit));
    SHR_IF_ERR_EXIT(egress_eedb_svtag_indication_enable_init(unit));
    SHR_IF_ERR_EXIT(ingress_prt_configuration_init(unit));
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        SHR_IF_ERR_EXIT(ingress_ecologic_init(unit));
    }
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_init(unit));
    SHR_IF_ERR_EXIT(ingress_lbp_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_eth_termination_mapping_table_init(unit));
    SHR_IF_ERR_EXIT(ingress_lbp_vlan_editing_configuration_init(unit));
    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(ingress_lbp_jer_mode_configuration_init(unit));
    }

    SHR_IF_ERR_EXIT(ingress_egw_full_threshold_set(unit));
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_data_path_hw))
    {
        SHR_IF_ERR_EXIT(ingress_ippe_traffic_enable(unit));
    }
    if (dnx_data_ingress_cs.parser.feature_get(unit, dnx_data_ingress_cs_parser_parsing_context_map_enable))
    {
        SHR_IF_ERR_EXIT(ingress_ippe_parser_context_mapping(unit));
    }
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_l4_protocol_fields_config_enable))
    {
        SHR_IF_ERR_EXIT(ingress_ippb_l4_protocols_config(unit));
    }
    SHR_IF_ERR_EXIT(ingress_vtt_global_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_port_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_itpp_general_configuration_init(unit));
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_da_type_map_enable))
    {
        SHR_IF_ERR_EXIT(ingress_da_type_map_init(unit));
    }
    SHR_IF_ERR_EXIT(ingress_per_trap_context_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_egress_parser_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_egress_forwarding_additional_info_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_stat_fec_ecmp_mapping_init(unit));
    SHR_IF_ERR_EXIT(egress_prp_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_parser_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_selected_outlif_table_init(unit));
    SHR_IF_ERR_EXIT(egress_system_headers_lif_extension_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_system_header_generation_table_init(unit));
    SHR_IF_ERR_EXIT(egress_global_system_header_configuration_init(unit));

    if (!(soc_is(unit, JERICHO2_A0_DEVICE) && soc_sand_is_emulation_system(unit) != 0))
    {
        SHR_IF_ERR_EXIT(egress_global_inlif_resolution_table_init(unit));
    }
    SHR_IF_ERR_EXIT(egress_global_eedb_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_exclude_source_filter_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_same_interface_filter_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_mapping_forward_context_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_ingress_trapped_by_fhei_configure_init(unit));
    SHR_IF_ERR_EXIT(ire_packet_generator_init(unit));

    
    if (is_image_standard_1)
    {
        SHR_IF_ERR_EXIT(ingress_egress_per_layer_protocol_configuration_init(unit));
        SHR_IF_ERR_EXIT(egress_additional_headers_profile_table_init(unit));
        SHR_IF_ERR_EXIT(ingress_gre_next_protocol_eth_init(unit));
        SHR_IF_ERR_EXIT(ingress_udp_next_protocol_enable_init(unit));
        SHR_IF_ERR_EXIT(egress_current_next_protocol_config_table_init(unit));
        SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration_init(unit));
        SHR_IF_ERR_EXIT(ingress_map_parsing_ctx_layer_potocol(unit));
        SHR_IF_ERR_EXIT(egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_init(unit));
        SHR_IF_ERR_EXIT(egress_erpp_filter_per_fwd_context_map_init(unit));

    }

    SHR_IF_ERR_EXIT(dnx_field_context_ifwd2_kbp_acl_init(unit));

    /*
     * Bare metal init part
     */
    if (!is_image_standard_1)
    {
        SHR_IF_ERR_EXIT(bare_metal_configuration_init(unit));
    }

    if (dnx_data_lif.global_lif.null_lif_get(unit))
    {
        SHR_IF_ERR_EXIT(egress_glem_dummy_entry_configure(unit));
    }

    SHR_IF_ERR_EXIT(egress_cfg_in_lif_null_value_init(unit));
    SHR_IF_ERR_EXIT(dnx_force_bubble_cfg_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_header_map_tables_configuration_udp_ports_clear(
    int unit,
    uint32 udp_ports_profile)
{
    int nof_additional_headers_map_tables_entries;
    int entry;
    SHR_FUNC_INIT_VARS(unit);

    nof_additional_headers_map_tables_entries = sizeof(additional_headers_map) / sizeof(additional_headers_map_t);

    /*
     * search for init configuration of the additional header profile
     */
    for (entry = 0; entry < nof_additional_headers_map_tables_entries; entry++)
    {
        if (additional_headers_map[entry].additional_headers_profile ==
            ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_ports_profile))
        {
            /*
             * Write to EGRESS_ADDITIONAL_HEADERS_MAP_TABLE table
             */
            SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration
                            (unit, additional_headers_map[entry].additional_headers_profile,
                             additional_headers_map[entry].start_current_next_protocol_enable_main_header,
                             additional_headers_map[entry].start_current_next_protocol_main_header,
                             additional_headers_map[entry].additional_headers_packet_size_enable_main_header,
                             additional_headers_map[entry].additional_headers_packet_size_main_header,
                             additional_headers_map[entry].start_current_next_protocol_enable_add_header_0,
                             additional_headers_map[entry].start_current_next_protocol_add_header_0,
                             additional_headers_map[entry].additional_headers_packet_size_enable_add_header_0,
                             additional_headers_map[entry].additional_headers_packet_size_add_header_0,
                             additional_headers_map[entry].protocol_specific_information_add_header_0,
                             additional_headers_map[entry].protocol_specific_information_add_header_0_type,
                             additional_headers_map[entry].start_current_next_protocol_enable_add_header_1,
                             additional_headers_map[entry].start_current_next_protocol_add_header_1,
                             additional_headers_map[entry].additional_headers_packet_size_enable_add_header_1,
                             additional_headers_map[entry].additional_headers_packet_size_add_header_1,
                             additional_headers_map[entry].protocol_specific_information_add_header_1,
                             additional_headers_map[entry].protocol_specific_information_add_header_1_type,
                             additional_headers_map[entry].start_current_next_protocol_enable_add_header_2,
                             additional_headers_map[entry].start_current_next_protocol_add_header_2,
                             additional_headers_map[entry].additional_headers_packet_size_enable_add_header_2,
                             additional_headers_map[entry].additional_headers_packet_size_add_header_2,
                             additional_headers_map[entry].protocol_specific_information_add_header_2,
                             additional_headers_map[entry].protocol_specific_information_add_header_2_type));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_header_map_tables_configuration_udp_ports_update(
    int unit,
    uint32 udp_src_port,
    uint32 udp_dest_port,
    uint32 udp_ports_profile)
{
    uint32 entry_handle_id;
    uint32 tmp;
    uint32 additional_header_specific_information[2];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES,
                               ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_ports_profile));

    /*
     * By setting TRUE, UDP destination port is set by protocol specific information.
     */
    tmp = TRUE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp,
                                                    additional_header_specific_information));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, DBAL_FIELD_SOURCE_PORT_TYPE, &tmp,
                     additional_header_specific_information));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT, &udp_dest_port,
                                                    additional_header_specific_information));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_SOURCE_PORT, &udp_src_port,
                                                    additional_header_specific_information));

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                     INST_SINGLE, additional_header_specific_information);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egress_additional_headers_map_tables_configuration_vxlan_udp_dest_update(
    int unit,
    uint32 udp_dest_port,
    dbal_enum_value_field_etpp_additional_header_profile_stack_attributes_e additional_header_profile)
{

    uint32 entry_handle_id;
    uint32 tmp;
    uint32 additional_header_specific_information[2];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if ((additional_header_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN)
        && (additional_header_profile != DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE))
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "Only vxlan or vxlan gpe are expected to have their udp dest port updated. ");

    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));

    /*
     * additional header profile: vxlan or vxlan gpe
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES,
                               additional_header_profile);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, 1,
                                   additional_header_specific_information);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * update specific information with: destination port from next protocol: destination port type = 0
     */
    tmp = FALSE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                    DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp,
                                                    additional_header_specific_information));
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, 1,
                                     additional_header_specific_information);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_CURRENT_NEXT_PROTOCOL, 2, udp_dest_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
int
dnx_init_pp_layer_types_value_get(
    int unit,
    uint32 prototype)
{
    uint32 hw_enum_value;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_LAYER_TYPES, prototype, &hw_enum_value));

    return hw_enum_value;
exit:
    SHR_FUNC_EXIT;
}
