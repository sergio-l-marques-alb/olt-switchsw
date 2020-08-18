/** \file init_pp.c
 * $Id$
 *
 * PP initialization procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <src/bcm/dnx/init/init_pp.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <soc/sand/sand_aux_access.h>
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
#define IPV4_UC                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC
    /** Forwarding code is IPV4_MC */
#define IPV4_MC                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC
    /** Forwarding code is IPV6_UC */
#define IPV6_UC                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC
    /** Forwarding code is IPV6_MC */
#define IPV6_MC                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC
    /** Forwarding code is BIER_MPLS */
#define BIER_MPLS                   DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS
    /** Forwarding code is BIER_TI */
#define BIER_TI                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_TI
    /** Forwarding code is INGRESS_TRAPPED */
#define INGRESS_TRAPPED             DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED
    /** Forwarding code is STACKING - Right*/
#define STACKING_RIGHT              DBAL_ENUM_FVAL_EGRESS_FWD_CODE_STACKING_PORT
    /** Forwarding code is STACKING - Left*/
#define STACKING_LEFT               DBAL_ENUM_FVAL_EGRESS_FWD_CODE_CPU_PORT
    /** Forwarding code is RCH_ENC */
#define RCH_ENC                     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_ENC
    /** Forwarding code is RCH_OUTLIF_PTCH_ENC */
#define RCH_OUTLIF_PTCH_ENC         DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC
    /** Forwarding code is BIER_RCH_ENC */
#define BIER_RCH_ENC                DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC
    /** Forwarding code is FCOE_FCF */
#define FCOE_FCF                    DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FCOE_FCF
    /** Forwarding code is FCOE_VFT */
#define FCOE_VFT                    DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FCOE_FCF_VFT
    /** Forwarding code is SRV6_SEGMENT_ENDPOINT */
#define SRV6_ENDPOINT               DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT
    /** Forwarding code is SRV6_TUNNEL_TERMINATED */
#define SRV6_TUNNEL_TERMINATED      DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_TUNNEL_TERMINATED
    /** Forwarding code is SRV6_ENDPOINT_PSP */
#define SRV6_ENDPOINT_PSP           DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP
    /** Forwarding code is TM */
#define TM                          DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TM
    /** Forwarding code is RAW_PROCESSING */
#define RAW_PROCESSING              DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING
    /** Forwarding code is REDIRECT2CPU */
#define REDIRECT2CPU                DBAL_ENUM_FVAL_EGRESS_FWD_CODE_REDIRECT2CPU
    /** Forwarding code is ERPP_TRAPPED */
#define ERPP_TRAPPED                DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ERPP_TRAPPED
    /** Forwarding code is RAW_PROCESSING */
#define RAW_PROCESSING              DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING
    /** Forwarding code is INGRESS_TRAPPED_IPV6 */
#define INGRESS_TRAPPED_IPV6        DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED_IPV6
    /** Forwarding code is MPLS_INJECTED_FROM_OAMP */
#define MPLS_INJECTED_FROM_OAMP     DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS_INJECTED_FROM_OAMP
    /** Forwarding code is IPV4_DO_NOT_EDIT */
#define IPV4_DO_NOT_EDIT            DBAL_ENUM_FVAL_EGRESS_FWD_CODE_DO_NOT_EDIT
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
    /** Inlif Decision 0 is PPH Inlif */
#define INLIF_DESCISION_0_INLIF                 DBAL_ENUM_FVAL_INLIF_DECISION_0_PPH_INLIF
    /** Inlif Decision 1 is PPH Inlif */
#define INLIF_DESCISION_1_INLIF                 DBAL_ENUM_FVAL_INLIF_DECISION_1_PPH_INLIF
    /** Inlif Decision 1 is PPH Inlif Extension */
#define INLIF_DESCISION_1_INLIF_EXTENSION       DBAL_ENUM_FVAL_INLIF_DECISION_1_INLIF_EXTENSION
    /** System Headers Mode is Jericho1 */
#define SYSTEM_HEADERS_MODE_JERICHO_MODE        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE
    /** System Headers Mode is Jericho2 */
#define SYSTEM_HEADERS_MODE_JERICHO2_MODE       DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE

    /** No Additional Headers */
#define ADDITIONAL_HEADERS_NONE                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE
    /** The additional Header is ELI for EVPN MC Label */
#define ADDITIONAL_HEADERS_ESI_EVPN             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI
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

    /** MPLS Header legth */
#define MPLS_BYTES_TO_ADD                       4
    /** VXLAN Header legth */
#define VXLAN_BYTES_TO_ADD                      8
    /** GENEVE Header legth */
#define GENEVE_BYTES_TO_ADD                     8
    /** GRE4 Header legth */
#define GRE4_BYTES_TO_ADD                       4
    /** GRE8 Header legth */
#define GRE8_BYTES_TO_ADD                       8
    /** GRE12 Header legth */
#define GRE12_BYTES_TO_ADD                      12
    /** UDP Header legth */
#define UDP_BYTES_TO_ADD                        8

/** Generates MPLS header_specific_information value:
 *   {Label-Type(2),Label(20),EXP-Type(1),EXP(3),TTL-Type(1),TTL(8)}*/
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET     33
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET          13
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET       12
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET             9
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET        8
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET             0
#define MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION(_label_type_, _label_val_, _exp_type_, _exp_val_, _ttl_type_, _ttl_val_) \
    ((((uint64)(_label_type_)) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_TYPE_OFFSET)   | \
     (((uint64)(_label_val_) ) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_LABEL_OFFSET)        | \
     (((uint64)(_exp_type_)  ) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_TYPE_OFFSET)     | \
     (((uint64)(_exp_val_)   ) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_EXP_OFFSET)          | \
     (((uint64)(_ttl_type_)  ) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_TYPE_OFFSET)     | \
     (((uint64)(_ttl_val_)   ) << MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION_TTL_OFFSET))

    /** Label Type=2, EXP Type=1, EXP=0, TTL Type=1, TTL=1*/
#define ESI_PROTOCOL_SPECIFIC_INFORMATION       MPLS_PROTOCOL_HEADER_SPECIFIC_INFORMATION(2, 0, 1, 0, 0, 1)
    /** Label Type=0, EXP Type=1, EXP=1, TTL Type=1, TTL=0*/
#define EL_PROTOCOL_SPECIFIC_INFORMATION        4864
    /** Label Type=1, Label=7, EXP Type=0, EXP=1, TTL Type=0, TTL=3*/
#define ELI_PROTOCOL_SPECIFIC_INFORMATION       8589992451
    /** VXLAN Protocol Specific Information*/
#define VXLAN_PROTOCOL_SPECIFIC_INFORMATION     8
    /** VXLAN GPE Protocol Specific Information*/
#define VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION 0xC
    /** GRE Protocol Specific Information*/
#define GRE_PROTOCOL_SPECIFIC_INFORMATION       0
    /** UDP Protocol Specific Information*/
#define UDP_PROTOCOL_SPECIFIC_INFORMATION       0
    /** VXLAN UDP Protocol Specific Information:
     *  source_port_type:  0x1
     *  (indicate the port is defined by protocol_specific
     *  information)
     *  source_port:      0x5000 destnation_port: 0x5555
     *  destination_port_type: 0x1
     *  (indicate the port is defined by protocol_specific
     *  information)
     *  destination_port: 0x5555 */
#define VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION (0x14002AAAB)
    /** GENEVE Protocol Specific Information*/
#define GENEVE_PROTOCOL_SPECIFIC_INFORMATION    0
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_GRE               47
    /** Next Protocol is GRE*/
#define CURRENT_NEXT_PROTOCOL_UDP               17
    /** Next Protocol is GENEVE*/
#define CURRENT_NEXT_PROTOCOL_GENEVE            6081
    /** Next Protocol is VXLAN*/
#define CURRENT_NEXT_PROTOCOL_VXLAN             4087

#define ERPP_FILTER_PER_FWD_CONTEXT_SIZE        11
    /** Max number of elements in the FLPB lookup accessing FIFO */
#define FLP_FULL_THRESHOLD                      800
    /** Size of FTMH LB key in bytes by default */
#define FTMH_LB_KEY_EXT_SIZE 3
    /** Size of FTMH Stacking Ext in bytes by default */
#define STACKING_EXT_SIZE 0

#define DEVICE_SCOPE_LIF_PROFILE_JR_MODE_ENABLED 1
#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ONLY_SYS_SCOPE_ENABLED 2
#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ENABLED 3
#define SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED 1

/*
 * Structure for initialization of tables for ETPPA_PRP_FORWARD_CODE_CAM, ERPP_FORWARD_CONTEXT_SELECTION_CAM
 */
typedef struct
{
    /*
     * Result fields - FORWARD_CODE and VALID bit
     */
    dbal_enum_value_field_egress_fwd_code_e forward_code;
    /*
     * Table result fields
     */
    dbal_enum_value_field_layer_types_e forward_layer_type;
    int forward_layer_type_mask;
    int pph_forwarding_layer_additional_info;
    int pph_forwarding_layer_additional_info_mask;
    int tm_action_type;
    int tm_action_type_mask;
    int forward_code_port_profile;
    int forward_code_port_profile_mask;
    int is_tdm;
    int is_tdm_mask;
    int is_applet;
    int is_applet_mask;
    int forward_layer_qualifier;
    int forward_layer_qualifier_mask;
    int ingress_trapped_by_fhei_type;
    int ingress_trapped_by_fhei_type_mask;
    int ace_value;
    int ace_value_mask;
} forward_context_id_cam_entry_t;

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
     * Key fields
     */
    int prt_recycle_profile;
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile;

    /*
     * Table result fields
     */
    int prt_qualifier_mode;
    int layer_offset;
    int context_sel;
    int kbr_valid_bitmap_4;
    int kbr_valid_bitmap_3;
    int kbr_valid_bitmap_2;
    int kbr_valid_bitmap_1;
    int ffc_instruction_3;
    int ffc_instruction_2;
    int ffc_instruction_1;
    int ffc_instruction_0;
    int src_system_port_prt_mode;
    int pp_port_enable;
    int tcam_mode_enable;

} prt_profile_config_t;

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
    int is_mpls;
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
    dbal_enum_value_field_egress_fwd_code_e forward_code;
    /*
     * Table result field - Enable/ Disable
     */
    int termination_enable;
    int protocol_ethernet_enable;
    int protocol_ipv4_uc_enable;
    int protocol_ipv4_mc_enable;
    int protocol_ipv6_uc_enable;
    int protocol_ipv6_mc_enable;
    int protocol_mpls_enable;
    int stp_enable;
    int vlan_membership_enable;
    int acceptable_frame_type_enable;
    int split_horizon_enable;
    int learn_enable;
    int fwd_context_is_pp_enable;
    int fwd_context_is_ipv4_or_ipv6;
} egress_per_fwd_code_config_table_t;

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
    dbal_enum_value_field_layer_types_e current_protocol_type;
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
    uint64 protocol_specific_information_add_header_0;
    dbal_fields_e protocol_specific_information_add_header_0_type;
    int start_current_next_protocol_enable_add_header_1;
    int start_current_next_protocol_add_header_1;
    int additional_headers_packet_size_enable_add_header_1;
    int additional_headers_packet_size_add_header_1;
    uint64 protocol_specific_information_add_header_1;
    dbal_fields_e protocol_specific_information_add_header_1_type;
    int start_current_next_protocol_enable_add_header_2;
    int start_current_next_protocol_add_header_2;
    int additional_headers_packet_size_enable_add_header_2;
    int additional_headers_packet_size_add_header_2;
    uint64 protocol_specific_information_add_header_2;
    dbal_fields_e protocol_specific_information_add_header_2_type;
} additional_headers_map_t;

typedef struct
{
    dbal_enum_value_field_irpp_2nd_parser_parser_context_e parser_ctx;
    dbal_enum_value_field_layer_types_e layer_type_before;
    dbal_enum_value_field_layer_types_e layer_type_after;
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
} forwarding_additional_info_config_t;

typedef struct
{
    /**
     * Key Field - Forwarding Context
     */
    dbal_enum_value_field_egress_fwd_code_e forward_code;
    /**
     * Result Fields - Filters Enablers
     */
    int tdm_discard;
    int rqp_discard;
    int invalid_otm;
    int dss_stacking;
    int lag_multicast;
    int exclude_src;
    int same_interface;
    int unacceptable_frame_type;
    int unknown_da;
    int split_horizon;
    int glem_pp_trap;
    int glem_non_pp_trap;
    int ttl_scoping;
    int ttl_zero;
    int ttl_one;
    int mtu_violation;
    int ipv4_filters;
    int ipv6_filters;
    int layer_4_filters;
} erpp_filter_per_fwd_context_t;

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
} jr_mode_pph_fwd_code_mapping_config_t;

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
} jr_mode_pph_vsi_src_mapping_config_t;

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
} jr_mode_pph_fhei_size_mapping_config_t;

/** structure to map layer_type and the 1st context in the Egress Parser */
typedef struct
{
    dbal_enum_value_field_layer_types_e layer_type;
    dbal_enum_value_field_erpp_1st_parser_parser_context_e parser_context;
} layer_type_to_erpp_1st_parsing_context_t;

/** structure to map fwd_code and the 1st context in the Egress Parser */
typedef struct
{
    dbal_enum_value_field_jr_fwd_code_e fwd_code;
    dbal_enum_value_field_erpp_1st_parser_parser_context_e parser_context;
} jr_mode_fwd_code_to_erpp_1st_parsing_context_t;

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
  * Initialization table for forward_code_id_map, which configures ETPPA_PRP_FORWARD_CODE_CAM, ERPP_FORWARD_CONTEXT_SELECTION_CAM HW Tables.
  * Each line configures entry in the TCAM table. The entry of the TCAM is being consist of:Entry-Data and Entry-Action.
  * In case of match, the Entry Action field will be used.
  */
  /* *INDENT-OFF* */
static const forward_context_id_cam_entry_t forward_code_id_map[] = {
    /** Forward Code,    Layer Type,   LAYER TYPE Mask (DC),    FLAI,  FLAI_Mask (DC)*/
    { ERPP_TRAPPED,      0,                 31,              0,        15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                     2,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                          0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                       0, DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_ERPP_TRAPPED, 0},
    /** Forward Code,    Layer Type,   LAYER TYPE Mask (DC),    FLAI,  FLAI_Mask (DC)*/
    { INGRESS_TRAPPED,      0,                 31,              0,        15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           3,                     0,                     2,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                          0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        1,                       0,                    0,             0},
   /** Forward Code,    Layer Type,   LAYER TYPE Mask (DC),    FLAI,        FLAI_Mask (DC)*/
    { STACKING_RIGHT,       0,           31,                    0,          15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,        FWD Code Port Profile Mask*/
        0,           0,         STACKING_RIGHT_CODE_PORT_PROFILE,        0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask0,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                   0,                         0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask */
        0,                             0,               0,           3},
   /** Forward Code,    Layer Type,   LAYER TYPE Mask (DC),    FLAI,        FLAI_Mask (DC)*/
    { STACKING_LEFT,        0,           31,                    0,          15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,         STACKING_LEFT_CODE_PORT_PROFILE,        0,
   /** Is_TDM,      Is_TDM Mask ,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                   0,                         0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask */
        0,                             0,               0,             3},
   /* we mask the first lsb bit of the LAYER TYPE to hit also IPv6 */
   /** Forward Code,  Layer Type, LAYER TYPE Mask,    FLAI,        FLAI_Mask (bit 2 - care)*/
    { IPV4_DO_NOT_EDIT,  DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,    1,                 7,           2,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
       0,           0,           0,                     3,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
       0,           0,           0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
       0,                        0,                    0,            3},
   /** Forward Code,    Layer Type,             LAYER TYPE Mask,    FLAI,        FLAI_Mask - FLAI[1]==0*/
    {  IPV4_UC, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,          0,                 0,           13,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                        0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                      0,                    0,                            65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
         0,                       0,                  0,            0},

   /** Forward Code,    Layer Type,                 LAYER TYPE Mask,    FLAI,        FLAI_Mask - FLAI[1]==1*/
    {  IPV4_MC, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,           0,                 2,           13,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                  0,                    0,                            65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                  0,              0},
        
   /** Forward Code,    Layer Type,             LAYER TYPE Mask,   FLAI,       FLAI_Mask (DC)*/
    { ETHERNET, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,           0,             0,          15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                         0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                     0,                    0,                             65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                  0,            0},

   /** Forward Code,    Layer Type,             LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    {  MPLS, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,           0,                0,          11,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                         0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                      0,                    0,                            65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                       0,                    0,          0},
   /** Forward Code,    Layer Type,                 LAYER TYPE Mask,    FLAI,        FLAI_Mask - FLAI[1]==0*/
    {  IPV6_UC, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,               0,              0,          13,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                   0,                     0,                    0,                           65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                    0,              0},

   /** Forward Code,    Layer Type,                     LAYER TYPE Mask,    FLAI,        FLAI_Mask - FLAI[1]==1*/
    {   IPV6_MC,    DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,               0,              2,           13,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                    0,                         65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                   0,             0},

   /** Forward Code,    Layer Type,                     LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    {   BIER_MPLS,  DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,        0,                0,           15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
        0,           0,                     0,                    0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                   0,              0},

   /** Forward Code,    Layer Type,                     LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    {   BIER_TI,    DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI,            0,              0,           15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                   0,            0},
   /** Forward Code,    Layer Type,             LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    { FCOE_FCF, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE,         0,                  0,               15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,           0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask - vft_exists==0*/
        0,           0,           0,                     0,                    0,                 65531,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                   0,              0},

   /** Forward Code,    Layer Type,              LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    { FCOE_VFT, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE,           0,                 0,            15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                 0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask - vft_exists==1*/
        0,           0,           0,                     0,                    4,                 65531,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                    0,            0},

    /** Forward Code,   Layer Type,                             LAYER TYPE Mask,    FLAI,      FLAI_Mask (DC)*/
    { SRV6_ENDPOINT, DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT,           0,         0,        15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        0,           0,                 0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                 0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                    0,             0},
    /** Forward Code,            Layer Type,                         LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
     { SRV6_TUNNEL_TERMINATED,  DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND,        0,                0,            15,
    /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
         0,           0,           0,                     3,
    /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
         0,           0,           0,                     0,                    0,                 65535,
    /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
         0,                        0,                    0,            3},
   /** Forward Code,            Layer Type,                         LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    { SRV6_TUNNEL_TERMINATED,  DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND,        0,                0,            15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
        0,           0,           0,                     3,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,           0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                    0,            0},
   /** Forward Code,    Layer Type,                     LAYER TYPE Mask,    FLAI,        FLAI_Mask (DC)*/
    {   TM ,    DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,        0,             0,            15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
        0,           0,           0,                            0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
        0,           0,           0,                            0,                    0,                  65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                        0,                    0,             0},
   /** Forward Code,           Layer Type,                     LAYER TYPE Mask,    FLAI,        FLAI_Mask (bit 2 - care)*/
   { MPLS_INJECTED_FROM_OAMP, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,        0,  MPLS_INJECTED_OAMP_FAI,  11,
    /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
         0,           0,           0,                            0,
    /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
         0,           0,           0,                            0,                    0,                  65535,
    /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
         0,                        0,                    0,              0},
    /** Forward Code,  Layer Type, LAYER TYPE Mask,    FLAI,        FLAI_Mask (bit 2 - care)*/
     { RAW_PROCESSING,      0,        31,                 0,                    15,
    /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
          0,           0,        RAW_PROCESSING_CODE_PORT_PROFILE,               0,
    /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
          0,           0,           0,                            0,                    0,                  65535,
    /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
          0,                        0,                   0,              0},
    /** Forward Code,  Layer Type, LAYER TYPE Mask,    FLAI,        FLAI_Mask (bit 2 - care)*/
   { REDIRECT2CPU,      0,        31,                 0,                    15,
    /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
           0,           0,        RAW_PROCESSING_CODE_PORT_PROFILE,               0,
    /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,      FWD Layer Qualifier Mask (DC)*/
           0,           0,           0,                            0,                    0,                  65535,
    /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
           0,                        0,                 0,              0},
    /** Forward Code, Layer Type,   LAYER TYPE Mask (DC),    FLAI,  FLAI_Mask (DC)*/
    { MIRROR_OR_SS,      0,                 31,              0,        15,
   /** TM Action (SNP),   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        2,                     0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                          0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                       0,                    0,             0},
    /** Forward Code, Layer Type,   LAYER TYPE Mask (DC),    FLAI,  FLAI_Mask (DC)*/
    { MIRROR_OR_SS,      0,                 31,              0,        15,
   /** TM Action (MIRROR & SS),   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask*/
        3,                             0,                     0,                     0,
   /** Is_TDM,      Is_TDM Mask,     Is_Applet,            Is_Applet Mask,       FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,                     0,                     0,                          0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                       0,                    0,             0},
   /** Forward Code,    Layer Type,   LAYER TYPE Mask (DC),    FLAI,        FLAI_Mask (DC)*/
    {  BUG_CODE,           0,           31,                    0,           15,
   /** TM Action,   TM Action Mask,  FWD Code Port Profile,  FWD Code Port Profile Mask (DC)*/
        0,           0,           0,                     3,
   /** Is_TDM,  Is_TDM Mask ,   Is_Applet ,            Is_Applet Mask ,  FWD Layer Qualifier,     FWD Layer Qualifier Mask (DC)*/
        0,           0,           0,                     0,                    0,                 65535,
   /** Ingress Trapped,     Ingress Trapped Mask,   ACE_Value,   ACE_Value_Mask*/
        0,                       0,                     0,              0}
};



/*
 * Initialization table for forward_code_id_map, which configures ETPPA_PRP_FORWARD_CODE_CAM, ERPP_FORWARD_CONTEXT_SELECTION_CAM HW Tables.
 * Each line configures entry in the TCAM table. The entry of the TCAM is being consist of:Entry-Data and Entry-Action.
 * In case of match, the Entry Action field will be used.
 */
 /* *INDENT-OFF* */
static const forward_context_id_cam_entry_t forward_code_id_jr1_mode_map[] = {
  /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
  { INGRESS_TRAPPED,    0,         31,       7,      0,       0,      0,            0,                     0,
  /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               1,                0,
   /** ACE_Value,  ACE_Value_Mask*/
       0,              0},
  /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
  { INGRESS_TRAPPED,    0,         31,       7,      0,       0,      0,  STACKING_LEFT_CODE_PORT_PROFILE,  0,
  /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               1,                0,
   /** ACE_Value,                   ACE_Value_Mask*/
       0,              0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { STACKING_LEFT,    0,         31,       0,      15,       0,      1,   STACKING_LEFT_CODE_PORT_PROFILE,       0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               0,                0,
   /** ACE_Value,                   ACE_Value_Mask*/
       0,              0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { RAW_PROCESSING,    0,         31,       0,      15,       0,      0, RAW_PROCESSING_CODE_PORT_PROFILE,         0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               0,                0,
   /** ACE_Value,   ACE_Value_Mask*/
          0,              0},
  /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { ETHERNET,   DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,     0,         0,      0,       0,      0,            0,                     0,
  /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               0,                0,
  /** ACE_Value,   ACE_Value_Mask*/
       0,              0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { MPLS,             0,         31,         5,      0,       0,      0,            0,                     0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
        0,           0,        0,            0,             0,                65535,               0,                0,
  /** ACE_Value,   ACE_Value_Mask*/
        0,             0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { IPV4_UC,             0,         31,         1,      0,       0,      0,            0,                     0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
        0,           0,        0,            0,             0,                65535,               0,                0,
  /** ACE_Value,   ACE_Value_Mask*/
        0,               0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { IPV4_MC,             0,         31,        2,      0,       0,      0,            0,                     0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
          0,           0,        0,            0,             0,                65535,               0,                0,
   /** ACE_Value,   ACE_Value_Mask*/
          0,              0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { IPV6_UC,             0,         31,        3,      0,       0,      0,            0,                     0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               0,                0,
   /** ACE_Value,   ACE_Value_Mask*/
          0,              0},
   /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { IPV6_MC,             0,         31,        4,      0,       0,      0,            0,                     0,
   /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
         0,           0,        0,            0,             0,                65535,               0,                0,
   /** ACE_Value,   ACE_Value_Mask*/
          0,              0},
  /** Forward Code,  Layer Type, LType Mask, FLAI, FLAI_Mask, TMAI, TMAI Mask,  FWD Code Prt Prof,  FWD Code Port Profile Mask*/
   { TM,                0,         31,          0,      0,       0,      0,            0,                     0,
  /** Is_TDM, Is_TDM Mask, Is_Applet, Is_Applet Mask, FWD Layer Qual, FWD Layer Qual Mask, Ingress Trapped,  Ingress Trapped Mask*/
       0,           0,        0,            0,             0,                65535,               0,                0,
  /** ACE_Value,                   ACE_Value_Mask*/
   ACE_CTXT_J_MODE_OVRRIDE,              0},
};
 /*
  * Initialization table for egress_per_fwd_code_config_table_t structure, which configures DBAL_TABLE_EGRESS_PER_FORWARD_CODE Table.
  * Each line configures the Termination Forward Code Enable  bit corresponding to the Forward Code Type.
  * Index 0 Configures bit 0 to be valid.
  */

static const egress_per_fwd_code_config_table_t egress_per_fwd_code_config_table_map[] = {
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {       ETHERNET,                 1,                                1,                     0,        0,         0,              0,          0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
               1,                     1,                                1,                     1,              1,                 1,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {         MPLS,                   1,                                0,                     0,        0,         0,              0,          1,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
                0,                    0,                                0,                     0,               0,                1,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {        IPV4_UC,                 1,                                0,                     1,         0,        0,            0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
               0,                     0,                                0,                     0,               0,                1,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {        IPV4_MC,                 1,                                0,                     0,         1,        0,              0,              0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
               0,                     0,                                0,                     0,               0,                1,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {        IPV6_UC,                 1,                                0,                     0,         0,        1,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
               0,                     0,                                0,                     0,               0,                 1,                   1},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {        IPV6_MC,                 1,                                0,                     0,         0,        0,             1,             0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   1},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {       BIER_MPLS,                1,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {        BIER_TI,                 1,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {    INGRESS_TRAPPED,             0,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {    STACKING_RIGHT,              1,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {    STACKING_LEFT,               1,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {       FCOE_FCF,                 1,                                0,                     0,          0,       0,              0,            0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {       FCOE_VFT,                 1,                                0,                     0,          0,       0,             0,             0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {    SRV6_ENDPOINT,               1,                                0,                     0,          0,       0,             0,             0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                 1,                   0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {   SRV6_ENDPOINT_PSP,            1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {  SRV6_TUNNEL_TERMINATED,        1,                                0,                     0,          0,       0,             0,             0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,               0,                1,                    0},
   /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
    {       BUG_CODE,                 1,                                0,                     0,          0,       0,             0,             0,
    /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                      0,                                0,                     0,                0,                 1,                  0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   IPV4_DO_NOT_EDIT,            1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {       TM,                    1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   RAW_PROCESSING,            1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   REDIRECT2CPU,            1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   MPLS_INJECTED_FROM_OAMP,   1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   ERPP_TRAPPED,             1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   INGRESS_TRAPPED_IPV6,      1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0},
    /**      Forward Code,     Termination Forward Code Enable,  Ethernet FWD Code Enable,   IPv4_UC,   IPv4_MC,   IPv6_UC,       IPv6_MC,       MPLS */
     {   MIRROR_OR_SS,             1,                                0,                     0,        0,         0,              0,          0,
     /**     STP Enable,   VLAN_MEMBERSHIP_ENABLE,   ACCEPTABLE_FRAME_TYPE_ENABLE,   SPLIT_HORIZON_ENABLE, LEARN_ENABLE,     CONTEXT_IS_PP,          IPV4_OR_IPV6 */
              0,                    0,                                0,                     0,               0,                0,                    0}
};

 /*
  * Initialization table for outlif_decision_map, which configures DBAL_TABLE_EGRESS_SELECTED_GLOBAL_OUTLIFS Register.
  * Each line configures 7 bits from the Register according to the input index.
  * Index 0 configures DBAL_TABLE_EGRESS_SELECTED_GLOBAL_OUTLIFS [6:0].
  */

static const outlif_decision_t outlif_decision_map_jer2_mode[] = {
  /** Register Index, OutLif_0_Decision, OutLif_1_Decision, OutLif_2_Decision, OutLif_3_Decision*/
    { 0,              1,                 2,                 2,                 2},
    { 1,              1,                 3,                 2,                 2},
    { 2,              0,                 0,                 0,                 0},
    { 3,              1,                 3,                 3,                 2},
    { 4,              0,                 0,                 0,                 0},
    { 5,              0,                 0,                 0,                 0},
    { 6,              0,                 0,                 0,                 0},
    { 7,              1,                 3,                 3,                 3}
};

 static const outlif_decision_t outlif_decision_map_jer_mode[] = {
  /** Register Index, OutLif_0_Decision, OutLif_1_Decision, OutLif_2_Decision, OutLif_3_Decision*/
    { 0,              1,                 3,                 3,                 3},
    { 1,              1,                 1,                 1,                 1},
    { 4,              1,                 3,                 3,                 3},
    { 5,              1,                 1,                 1,                 1},
    { 6,              0,                 0,                 1,                 1},
    { 7,              0,                 0,                 0,                 0}
 };


 /*
  * Initialization table for outlif_extension_map, which configures DBAL_TABLE_EGRESS_SYSTEM_HEADERS_LIF_EXTENSION Table.
  * Each line configures the outlif extension.
  * Index 0 Configures line 0 - outlif 0 and line 1 configures outlif 1 etc.
  */
static const outlif_extension_config_t outlif_extension_map[]= {
 /** LIF Ext Type,                      OutLif_1_Extension, OutLif_2_Extension, OutLif_3_Extension, InLif_1_Extension, InLif_1_Extension_Profile*/
    {LIF_EXTENSION_TYPE_NONE,           0,                  0,                  0,                 0,                  0},
    {LIF_EXTENSION_TYPE_1XOUT_LIF,      1622,               0,                  0,                 0,                  0},
    {LIF_EXTENSION_TYPE_2XOUT_LIF,      1622,               918,                0,                 0,                  0},
    {LIF_EXTENSION_TYPE_3XOUT_LIF,      1622,               918,                214,               0,                  0},
    {LIF_EXTENSION_TYPE_INLIF_PROFILE,  0,                  0,                  0,                 1430,               1064}
};

 /*
  * Initialization table for prt_info_map, which configures INGRESS_PRT_INFO dbal table.
  *
  */
static const prt_profile_config_t jr2_prt_info_map[] = {
    /** PRT_RECYCLE_PROFILE,                                             PORT_TERMINATION_PTC_PROFILE,        PRT_QUAL_MODE, LAYER_OFFSET, CONTEXT_SEL, KBR_VALID_BITMAP_4, KBR_VALID_BITMAP_3, KBR_VALID_BITMAP_2, KBR_VALID_BITMAP_1, FFC_INSTRUCTION_3, FFC_2,   FFC_1,    FFC_0, SRC_SYS_PORT_PRT_MODE, PP_PORT_EN, TCAM_MODE_FULL_EN */
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE,                DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2,  1,           2,           1,                2,                4,                   0,                1,                 0,           261120,  258144,   245984,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE,                DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1,  1,           3,           0,                2,                0,                   0,                1,                 0,           261120,  258144,   238048,           1,               1,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE,             DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,  0,           0,           0,                0,                0,                   0,                1,                 0,                0,       0,   315616,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2,            DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,  1,           2,           1,                2,                4,                   0,                1,                 0,           261120,  258144,   245984,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1,    DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,  0,           8,           0,                0,                0,                   0,                1,                 0,                0,       0,   315616,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2,    DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,  0,           8,           0,                0,                0,                   0,                1,                 0,                0,       0,   315616,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE,                  DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH,  0,           0,           0,                0,                0,                   4,                1,                 0,           131552,       0,   315616,           2,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE,                   DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE,  0,           4,           0,                0,                0,                   0,                3,                 0,                0, 0x4D0E8,  0x380E0,           0,               1,              0},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2,                 DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH,  1,           2,           1,                2,                4,                   0,                1,            286848,           131552,  258144,   245984,           0,               0,              1},
    { DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE,            DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1,  1,           2,           1,                2,               12,                   0,                1,            328737,           261120,  258144,   245984,           0,               0,              1}

};

static const prt_profile_config_t j2c_prt_info_map[] = {
    /**PRT_RECYCLE_PROFILE,      PORT_TERMINATION_PTC_PROFILE,                    PRT_QUAL_MODE,    LAYER_OFFSET, CONTEXT_SEL, KBR_VALID_BITMAP_4, KBR_VALID_BITMAP_3, KBR_VALID_BITMAP_2, KBR_VALID_BITMAP_1, FFC_INSTRUCTION_3,  FFC_2,    FFC_1,    FFC_0,      SRC_SYS_PORT_PRT_MODE, PP_PORT_EN, TCAM_MODE_FULL_EN */
    {         0,           DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2,           1,               2,            1,              2,                   1,                   0,                 4,                 0,          1044480,  1032320,   983616,                0,               0,           0},
    {         0,           DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,        0,               0,            0,              0,                   1,                   0,                 0,                 0,                0,        0,  1262144,                0,               0,           0},
    {         13,          DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET,        0,               8,            0,              0,                   1,                   0,                 0,                 0,                0,        0,  1262144,                0,               0,           0},
    {         0,           DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1,           1,               3,            1,              2,                   1,                   0,                 4,                 0,          1044480,  1032320,   951232,                1,               1,           1}
};

/*
  * Initialization table for inlif_decision_config_map_jer_mode, which configures EGRESS_GLOBAL_INLIF_RESOLUTION Table.
  * Each line configures Inlif Decision 0 and Inlif Decision according to Jericho System Headers Mode and the MC Fwd Code.
  */

static const inlif_decision_config_t inlif_decision_config_map_jer_mode[] = {
/** System Headers Mode,               MC Forward Code, Inlif Decision 0 FoDo,   Inlif Decision 1 InLif*/
    {SYSTEM_HEADERS_MODE_JERICHO_MODE, 1,               INLIF_DESCISION_0_FODO,  INLIF_DESCISION_1_INLIF},
    {SYSTEM_HEADERS_MODE_JERICHO_MODE, 0,               INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION}
};
/*
 * Initialization table for inlif_decision_config_map_jer2_mode, which configures EGRESS_GLOBAL_INLIF_RESOLUTION Table.
 * Each line configures Inlif Decision 0 and Inlif Decision according to Jericho2 System Headers Mode and the MC Fwd Code.
 */
static const inlif_decision_config_t inlif_decision_config_map_jer2_mode[] = {
 /** System Headers Mode,               MC Forward Code, Inlif Decision 0 PPH,    Inlif Decision 1 InLif Extension*/
    {SYSTEM_HEADERS_MODE_JERICHO2_MODE, 1,               INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION},
    {SYSTEM_HEADERS_MODE_JERICHO2_MODE, 0,               INLIF_DESCISION_0_INLIF, INLIF_DESCISION_1_INLIF_EXTENSION}
};

/*
  * Initialization table for current_next_protocol_config_map, which configures DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE Table.
  * Each line configures the Current next protocol, based on the Current Protocol Type and the Current Protocol Namespace.
  */
static const current_next_protocol_config_table_t current_next_protocol_config_map[] = {
/**  Current Protocol Type,                              Next Protocol Namespace,                                             Current Next Protocol*/
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA,       DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_UA_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FCOE,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_FCOE_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PPPOE_SESSION, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_PPPOE_OVER_ETHERNET},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPAN,        DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE,               DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ERSPAN_OVER_GRE},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_IP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_IP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA,       DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_IP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UDP,           DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_LT2PV3,        DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_LT2PV3_OVER_IP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE,           DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_IP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET,      DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,                DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,                DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,                DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,                DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_BOS},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA,       DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS,                DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_BOS},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV4_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6,          DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN,         DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN_GPE,     DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_VXLAN_GPE_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_LT2PV3,        DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_LT2PV3_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE,        DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GENEVE_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE,           DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT,    DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GRE_OVER_UDP},
   { DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL,        DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_SRV6_OVER_IPV6},
   { DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,                DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_ETHERNET},
   { DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,                    DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS},
   { DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,                    DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_IPV6},
   { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,                    DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_VXLAN},
   { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UA,                 DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_MPLS_OVER_VXLAN}
};
 /*
  * Initialization table for additional_headers_profile_config_map, which configures DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP Table.
  * Each line configures the Additional Headers Profile, basec on the Context Additional Header Profile and LIF Additional Header Profile
  */

static const additional_headers_profile_config_t additional_headers_profile_config_map[] = {
   /** LIF Additional Headers Profile,                                                                                     Context Additional Headers Profile,                                         Additional Headers Profile*/
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE,                  ADDITIONAL_HEADERS_NONE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_3_AH,               ADDITIONAL_HEADERS_NONE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_2_AH,               ADDITIONAL_HEADERS_NONE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_1_AH,               ADDITIONAL_HEADERS_NONE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_NONE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_1_AH,               ADDITIONAL_HEADERS_ESI_EVPN},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_ESI_EVPN},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_ESI_EVPN},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOELOELI_OR_LIF_AHP_GRE4,                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_3_AH,               ADDITIONAL_HEADERS_CW_EL_ELI_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOELOELI_OR_LIF_AHP_GRE4,                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_CW_EL_ELI_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOELOELI_OR_LIF_AHP_GRE4,                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_CW_EL_ELI_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOFL_OR_LIF_AHP_GENEVE,                                          DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_2_AH,               ADDITIONAL_HEADERS_CW_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOFL_OR_LIF_AHP_GENEVE,                                          DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_CW_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOFL_OR_LIF_AHP_GENEVE,                                          DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_CW_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_2_AH,               ADDITIONAL_HEADERS_EL_ELI},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_EL_ELI},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_EL_ELI},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CW_OR_LIF_AHP_GRE12,                                              DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_1_AH,               ADDITIONAL_HEADERS_CW_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CW_OR_LIF_AHP_GRE12,                                              DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_CW_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CW_OR_LIF_AHP_GRE12,                                              DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_CW_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_FL_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,                       DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_2_MPLS_1_AH,               ADDITIONAL_HEADERS_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_FL_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,                       DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_1_MPLS_W_AH,               ADDITIONAL_HEADERS_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_FL_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,                       DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH,                   ADDITIONAL_HEADERS_FL_PWE},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE,    ADDITIONAL_HEADERS_VXLAN_GPE_UDP},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOFL_OR_LIF_AHP_GENEVE,                                          DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_GENEVE_UDP},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE,    ADDITIONAL_HEADERS_VXLAN_UDP},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_GRE4},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_WO_TNI_GRE8},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_TNI_GRE8},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE,                                                             DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CW_OR_LIF_AHP_GRE12,                                              DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_GRE12},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_FL_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,                       DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_IPV6_TNI_GRE8},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_GRE8_W_SN,                                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE,         ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN},
   
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOFL_OR_LIF_AHP_GENEVE,                                          DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(1)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY,                    DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(2)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(3)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CW_OR_LIF_AHP_GRE12,                                              DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(4)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_FL_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6,                       DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(5)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_CWOELOELI_OR_LIF_AHP_GRE4,                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(6)},
   {DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_GRE8_W_SN,                                                        DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP,                   ADDITIONAL_HEADERS_UDP_USER_DEFINED(7)},
};

 /*
  * Initialization table for system_header_profile_map, which configures DBAL_TABLE_EGRESS_SYSTEM_HEADER_GENERATION_TABLE Table.
  * Each line configures the specific system headers size (bytes to add), respectively to the generation profile.
  * Index 0 Configures line 0 - the size of system headers for the profile.
  */

static const system_header_profile_t system_header_profile_map[] = {
  /** Generation Profile, Bytes to add*/
    { J2_PP_GENERAL,      40},
    { J2_OAM,             35},
    { J2_TM,              20},
    { J_OAM,              30},
    { J_TM,               25},
    { TDM,                18},
    { OTMH,               17},
    { UDH,                0}
};

 /*
  * Initialization table for per_layer_protocol, which configures INGRESS_PER_LAYER_PROTOCOL dbal table.
  */
static const per_layer_protocol_t per_layer_protocol[] = {
    /**            Layer Type,                   ETHERNET, ARP,   IPV4,  IPV6,  MPLS,  OAM,   UDP ,  BFD_SINGLE_HOP, BFD_MULTI_HOP, TCP,   ICMP,  ICMPV6, 8021_x_2, IGMP   BIER_MPLS*/
    { DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,       TRUE,     FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,           FALSE,    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,           FALSE,    FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,           FALSE,    FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UA,        FALSE,    FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ARP,            FALSE,    TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_FCOE,           FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_TCP,            FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         TRUE,  FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_UDP,            FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,           FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP,  FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          TRUE,          FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PTP_EVENT,      FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PTP_GENERAL,    FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731,         FALSE,    FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ICMP,           FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, TRUE,  TRUE,   FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI,        FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,      FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, TRUE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_RCH,            FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE_SESSION,  FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT,  FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND,    FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IGMP,           FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    TRUE,  FALSE},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL,       FALSE,    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,          FALSE,         FALSE, FALSE, FALSE,  FALSE,    FALSE, FALSE}
};

static const per_2nd_stage_parsing_ctx_t per_2nd_stage_parsing_ctx[] = {
    /* parser conext                                                        layer_type_before                                layer_type_after*/
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NONE,                    DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,      DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1,                  DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,            DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPVX_DEMUX_B1,           DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,                DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_SPECULATIVEOAMB1,   DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,                DBAL_ENUM_FVAL_LAYER_TYPES_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV4_NOETHB2,            DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,                DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV6_NOETHB2,            DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,                DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS64_B1,           DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,           DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS128_B1,          DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,           DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS256_B1,          DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,           DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERTI_B1,               DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI,             DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_B1,                  DBAL_ENUM_FVAL_LAYER_TYPES_TCP,                 DBAL_ENUM_FVAL_LAYER_TYPES_TCP},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_B1,                  DBAL_ENUM_FVAL_LAYER_TYPES_UDP,                 DBAL_ENUM_FVAL_LAYER_TYPES_UDP},
    {DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP,                     DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN,             DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN}
};

static const additional_headers_map_t  additional_headers_map[] = {
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_NONE,              0,                              0,                              0,                      0,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {   ADDITIONAL_HEADERS_ESI_EVPN,            1,                              0,                              1,                  MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    1,                          1,                              1,                      MPLS_BYTES_TO_ADD,      ESI_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    { ADDITIONAL_HEADERS_CW_EL_ELI_PWE,         1,                              0,                              1,                      3*MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              1,                      MPLS_BYTES_TO_ADD,              0,                      DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    1,                          1,                              1,             MPLS_BYTES_TO_ADD, EL_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    1,                          0,                              1,                      MPLS_BYTES_TO_ADD, ELI_PROTOCOL_SPECIFIC_INFORMATION,   DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_CW_FL_PWE,         1,                              0,                              1,                      2*MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              1,             MPLS_BYTES_TO_ADD,    0,  DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    1,                          1,                              1,            MPLS_BYTES_TO_ADD,          0,  DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_EL_ELI,            1,                              0,                      1,                 2*MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              1,                      MPLS_BYTES_TO_ADD, EL_PROTOCOL_SPECIFIC_INFORMATION,    DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    1,                          0,                              1,             MPLS_BYTES_TO_ADD, ELI_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_CW_PWE,            0,                              0,                      1,                   MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                      0,                      DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              1,                          MPLS_BYTES_TO_ADD,          0,                      DBAL_FIELD_CW_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_FL_PWE,            1,                              0,                              1,                   MPLS_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    1,                          1,                              1,             MPLS_BYTES_TO_ADD,           0, DBAL_FIELD_MPLS_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_VXLAN_GPE_UDP,     1,                      CURRENT_NEXT_PROTOCOL_UDP,       1,      (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,     VXLAN_GPE_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    1,                 CURRENT_NEXT_PROTOCOL_VXLAN,             1,                        VXLAN_BYTES_TO_ADD, VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_GENEVE_UDP,        1,                      CURRENT_NEXT_PROTOCOL_UDP,       1,     (UDP_BYTES_TO_ADD + GENEVE_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    1,                 CURRENT_NEXT_PROTOCOL_GENEVE,            1,                        GENEVE_BYTES_TO_ADD, UDP_PROTOCOL_SPECIFIC_INFORMATION, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,       GENEVE_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_GENEVE_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_VXLAN_UDP,         1,                      CURRENT_NEXT_PROTOCOL_UDP,       1,      (UDP_BYTES_TO_ADD + VXLAN_BYTES_TO_ADD),
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    1,                          0,                              0,                              0,        VXLAN_PROTOCOL_SPECIFIC_INFORMATION,  DBAL_FIELD_VXLAN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    1,                 CURRENT_NEXT_PROTOCOL_VXLAN,             1,                        VXLAN_BYTES_TO_ADD, VXLAN_UDP_PROTOCOL_SPECIFIC_INFORMATION, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_GRE4,              1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE4_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE4_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_WO_TNI_GRE8,       1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE8K_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_TNI_GRE8,          1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_UDP,               1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {     ADDITIONAL_HEADERS_GRE12,             1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE12_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE12_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    { ADDITIONAL_HEADERS_IPV6_TNI_GRE8,         1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    { ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN,     1,                      CURRENT_NEXT_PROTOCOL_GRE,              1,                   GRE8_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_GRE8SN_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(1),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(2),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(3),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(4),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(5),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(6),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION},
    /**     Additonal Headers Profile, Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size - Main Header */
    {  ADDITIONAL_HEADERS_UDP_USER_DEFINED(7),  1,                      CURRENT_NEXT_PROTOCOL_UDP,           1,                   UDP_BYTES_TO_ADD,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 0*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 1*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
    /** Start Current Next Protocol Enable, Start Current Next Protocol, Additional packet size Enable, Additional packet size, Protocol Specific Information, Protocol Specific Information - Type - Additional Header 2*/
                    0,                          0,                              0,                              0,                        0,                    DBAL_FIELD_NO_ADDITIONAL_HEADER_SPECIFIC_INFORMATION}
};

 /*
  * Initialization table for MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLE.
  * Layer type is mapped to parser context.
  * For MPLS BOS upper layer protocol valid values are: Ethernet, Ipv4 and Ipv6.
  * The value of MPLS Parser Context is default configuration, indicating the value is not used.
  */
static const layer_type_to_mpls_upper_layer_protocol_t layer_type_to_mpls_upper_layer_protocol[] = {
    /**            Layer Type,                          MPLS Upper Layer Protocol*/
    { DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,    DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,          DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV4_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV6_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UA,           DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ARP,               DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_FCOE,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_TCP,               DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_UDP,               DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP,    DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP,     DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PTP_EVENT,         DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PTP_GENERAL,       DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731,            DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ICMP,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI,           DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS,         DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_BIER_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_RCH,               DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE_SESSION,     DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT,     DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND,       DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_IGMP,              DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    { DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL,          DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1}
};
 /*
  * Initialization table for forwarding_additional_info_config_map.
  * It configures EGRESS_FORWARDING_ADDITIONAL_INFO for IpvX compatible MC and INGRESS_FORWARDING_ADDITIONAL_INFO for unknown address.
  * Each line configures 'IPvX is multicast compatible with Fallback-to-Bridge' according to Forwarding additional information (FAI)
  * and unknown address according to FAI.
  * FAI[1] means IPvX is multicast compatible with Fallback-to-Bridge.
  * FAI[2] means Unknown address enable.
  */
static const forwarding_additional_info_config_t forwarding_additional_info_config_map[] = {
   /** FAI,     IPvX is multicast compatible with Fallback-to-bridge enable, BUM Traffic enable*/
    {0,                                 0,                                  0},
    {1,                                 0,                                  0},
    {2,                                 1,                                  1},
    {3,                                 1,                                  1},
    {4,                                 0,                                  0},
    {5,                                 0,                                  0},
    {6,                                 1,                                  1},
    {7,                                 1,                                  1},
    {8,                                 0,                                  0},
    {9,                                 0,                                  0},
    {10,                                1,                                  1},
    {11,                                1,                                  1},
    {12,                                0,                                  0},
    {13,                                0,                                  0},
    {14,                                1,                                  1},
    {15,                                1,                                  1}
};

 /**
  * Initialization table for erpp_filter_per_fwd_context_map, which configures DBAL_TABLE_ERPP_FWD_CONTEXT_FILTER Table.
  * Each 2 lines enables/disables ERPP Filter per forwarding context.
  */
static const erpp_filter_per_fwd_context_t erpp_filter_per_fwd_context_map[] = {
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   ETHERNET,               TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               TRUE,               TRUE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      TRUE,       TRUE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   MPLS,                   TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      TRUE,       TRUE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   IPV4_UC,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      TRUE,       TRUE,     FALSE,        TRUE,        FALSE,         TRUE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   IPV4_MC,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       TRUE,       TRUE,       TRUE,     FALSE,        TRUE,        FALSE,         TRUE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   IPV6_UC,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      TRUE,       TRUE,     FALSE,        FALSE,        TRUE,         TRUE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   IPV6_MC,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       TRUE,       TRUE,       TRUE,     FALSE,        FALSE,        TRUE,         TRUE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   BIER_MPLS,              TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   BIER_TI,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   INGRESS_TRAPPED,        FALSE,      TRUE,       TRUE,       FALSE,          FALSE,          FALSE,      FALSE,              FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          FALSE,         FALSE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   STACKING_RIGHT,         FALSE,      TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   STACKING_LEFT,          FALSE,      TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   RCH_ENC,                TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   RCH_OUTLIF_PTCH_ENC,    TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   BIER_RCH_ENC,           TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   FCOE_FCF,               TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   FCOE_VFT,               TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   SRV6_ENDPOINT,          TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   SRV6_TUNNEL_TERMINATED, TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   TM,                     TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE},
    /** Forwarding Context, TDM_DISCARD, RQP_DISCARD, INVALID_OTM, DSS_STACKING, LAG_MULTICAST, EXCLUDE_SRC, SAME_INTERFACE, UNACCEPTABLE_FRAME_TYPE, UNKNOWN_DA*/
    {   BUG_CODE,               TRUE,       TRUE,       TRUE,       TRUE,           TRUE,           FALSE,      TRUE,               FALSE,              FALSE,
    /** SPLIT_HORIZON, GLEM_PP_TRAP, GLEM_NON_PP_TRAP, TTL_SCOPING, TTL_ZERO, TTL_ONE, MTU_VIOLATION, IPV4_FILTERS, IPV6_FILTERS, LAYER_4_FILTERS*/
            FALSE,          TRUE,           TRUE,       FALSE,      FALSE,     FALSE,     FALSE,        FALSE,        FALSE,        FALSE}
};

static const jr_mode_pph_fwd_code_mapping_config_t jr_mode_pph_fwd_code_map[] = {
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 0 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 1 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,        0  ,     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,        1 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,        0 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,        1 ,       DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,        0 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS},
     /** fwd_layer_type       fwd_layer_qual_lsb       jr_fwd_code*/
      {     DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,        1 ,      DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS}

};

static const jr_mode_pph_vsi_src_mapping_config_t jr_mode_pph_vsi_src_map[] = {
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0 , 1,DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0 , 1,DBAL_ENUM_FVAL_JR_VSI_SRC_OUT_LIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, 0 , 0,DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, 0 , 0,DBAL_ENUM_FVAL_JR_VSI_SRC_VRF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1 , 1,DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 1 , 1,DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1 , 0,DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF},
   /** jr_fwd_code                  is_mc       outlif-is-rif               jr-vsi-src*/
    {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 1 , 0,DBAL_ENUM_FVAL_JR_VSI_SRC_IN_RIF}

};

static const jr_mode_pph_fhei_size_mapping_config_t jr_mode_pph_fhei_size_map[] = {
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 0 , 4,DBAL_ENUM_FVAL_FHEI_SIZE_8B},
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1 , 4,DBAL_ENUM_FVAL_FHEI_SIZE_8B},
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 0 , 4,DBAL_ENUM_FVAL_FHEI_SIZE_8B},
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 1 , 4,DBAL_ENUM_FVAL_FHEI_SIZE_8B},
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, 0 , 1,DBAL_ENUM_FVAL_FHEI_SIZE_8B},
 /** jr_fwd_code                                                  inlif-eq-zero       fwd-layer-idx               fhei-size*/
  {     DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, 0 , 1,DBAL_ENUM_FVAL_FHEI_SIZE_8B}
};

/*
 * In JR1 mode, the mapping to Context is done from Forward Code, while in JR2 Context vs. Layer Type
 */
static const jr_mode_fwd_code_to_erpp_1st_parsing_context_t jr_mode_egress_parsing_context_map[] = {
    {DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE,  DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1},       /*DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV4_A1},      /*DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV4_A1},      /*DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV6_A1},      /*DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV6_A1},      /*DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS,    DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},      /*DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_TRILL*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_CPU_TRAP1*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED0*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_1*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_2*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_SNOOP_OR_MIRROR*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED1*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED2*/
    /*DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED3*/
    {DBAL_ENUM_FVAL_JR_FWD_CODE_TM, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_TM}
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/
    /*Unknown*/

};




static const layer_type_to_erpp_1st_parsing_context_t jr2_mode_egress_parsing_context_map[] = {

    {DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_TM},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL,       DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_TM},
    {DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,       DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV4,           DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV4_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_IPV6,           DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_IPV6_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_MPLS,           DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_MPLS_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT,  DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_SRV6ENDPOINT_A1},
    {DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND,    DBAL_ENUM_FVAL_ERPP_1ST_PARSER_PARSER_CONTEXT_SRV6_BEYOND_A1}

};

  /* *INDENT-ON* */
/*
 * }
 */
/*
 *
 */

/**
 * \brief - * Init forward_code_id_map_dbal_configure table.
 * Based on the incoming parameters the function configures the Egress FORWARD Context ID table
 *
 * \param [in] unit -                                       Relevant unit.
 * \param [in] pph_forwarding_layer_additional_info -       PPH FLAI value
 * \param [in] pph_forwarding_layer_additional_info_mask -  PPH FLAI mask
 * \param [in] forward_layer_type -                         Forward Layer Type
 * \param [in] forward_layer_type_mask -                    Forward Layer Type Mask
 * \param [in] tm_action_type -                        FTMH TM Action Type
 * \param [in] tm_action_type_mask -                   FTMH TM Action Type Mask
 * \param [in] forward_code_port_profile -                  Forward Code Port Profile
 * \param [in] forward_code_port_profile_mask -             Forward Code Port Profile Mask
 * \param [in] is_tdm -                                     Is TDM value
 * \param [in] is_tdm_mask -                                Is TDM Mask
 * \param [in] is_applet -                                  Is Applet
 * \param [in] is_applet_mask -                             Is Applet Mask
 * \param [in] forward_layer_qualifier -                    Forward Layer Qualifier Field Value
 * \param [in] forward_layer_qualifier_mask -               Forward Layer Qualifier  Mask
 * \param [in] ingress_trapped_by_fhei_type -               Ingress Trapped by FHEI Field Value
 * \param [in] ingress_trapped_by_fhei_type_mask -          Ingress Trapped by FHEI Mask
 * \param [in] ace_value -                                  ACE value
 * \param [in] ace_value_mask -                             ACE value Mask
 * \param [in] forward_code -                               Relevant Forward Code Value
 * \param [in] valid -                                      Valid bit indication
 * \param [in] index -                                      Relevant Index to the table.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e
forward_context_id_map_dbal_configure(
    int unit,
    int pph_forwarding_layer_additional_info,
    int pph_forwarding_layer_additional_info_mask,
    dbal_enum_value_field_layer_types_e forward_layer_type,
    int forward_layer_type_mask,
    int tm_action_type,
    int tm_action_type_mask,
    int forward_code_port_profile,
    int forward_code_port_profile_mask,
    int is_tdm,
    int is_tdm_mask,
    int is_applet,
    int is_applet_mask,
    int forward_layer_qualifier,
    int forward_layer_qualifier_mask,
    int ingress_trapped_by_fhei_type,
    int ingress_trapped_by_fhei_type_mask,
    int ace_value,
    int ace_value_mask,
    dbal_enum_value_field_egress_fwd_code_e forward_code,
    int valid,
    int index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to ERPP_FORWARD_CODE_SELECTION table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_FORWARD_CODE_SELECTION, &entry_handle_id));
    if (!
        ((ace_value == DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_ERPP_TRAPPED)
         || (ace_value == ACE_CTXT_J_MODE_OVRRIDE)))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO,
                                     INST_SINGLE, pph_forwarding_layer_additional_info);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO_MASK,
                                     INST_SINGLE, pph_forwarding_layer_additional_info_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_TYPE, INST_SINGLE,
                                     forward_layer_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_TYPE_MASK, INST_SINGLE,
                                     forward_layer_type_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE, INST_SINGLE, tm_action_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE_MASK, INST_SINGLE,
                                     tm_action_type_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE_PORT_PROFILE, INST_SINGLE,
                                     forward_code_port_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_PORT_PROFILE_MASK, INST_SINGLE,
                                     forward_code_port_profile_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM_MASK, INST_SINGLE, is_tdm_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_APPLET, INST_SINGLE, is_applet);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_APPLET_MASK, INST_SINGLE,
                                     is_applet_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_QUALIFIER, INST_SINGLE,
                                     forward_layer_qualifier);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_QUALIFIER_MASK, INST_SINGLE,
                                     forward_layer_qualifier_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE, INST_SINGLE,
                                     ingress_trapped_by_fhei_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE_MASK, INST_SINGLE,
                                     ingress_trapped_by_fhei_type_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE, forward_code);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Write to ETPP_FORWARD_CODE_SELECTION table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ETPP_FORWARD_CODE_SELECTION, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO,
                                 INST_SINGLE, pph_forwarding_layer_additional_info);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO_MASK,
                                 INST_SINGLE, pph_forwarding_layer_additional_info_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_TYPE, INST_SINGLE, forward_layer_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_TYPE_MASK, INST_SINGLE,
                                 forward_layer_type_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE, INST_SINGLE, tm_action_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE_MASK, INST_SINGLE,
                                 tm_action_type_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE_PORT_PROFILE, INST_SINGLE,
                                 forward_code_port_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_PORT_PROFILE_MASK, INST_SINGLE,
                                 forward_code_port_profile_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM_MASK, INST_SINGLE, is_tdm_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_APPLET, INST_SINGLE, is_applet);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_APPLET_MASK, INST_SINGLE, is_applet_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_QUALIFIER, INST_SINGLE,
                                 forward_layer_qualifier);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_LAYER_QUALIFIER_MASK, INST_SINGLE,
                                 forward_layer_qualifier_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE, INST_SINGLE,
                                 ingress_trapped_by_fhei_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE_MASK, INST_SINGLE,
                                 ingress_trapped_by_fhei_type_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACE_VALUE, INST_SINGLE, ace_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACE_VALUE_MASK, INST_SINGLE, ace_value_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, INST_SINGLE, forward_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init forward_code_id_map table.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
egress_forward_code_map_init(
    int unit)
{

    int nof_forward_code_id_map_table_entries;
    int entry;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        nof_forward_code_id_map_table_entries = sizeof(forward_code_id_map) / sizeof(forward_context_id_cam_entry_t);
    }
    else
    {
        nof_forward_code_id_map_table_entries =
            sizeof(forward_code_id_jr1_mode_map) / sizeof(forward_context_id_cam_entry_t);
    }

    for (entry = 0; entry < nof_forward_code_id_map_table_entries; entry++)
    {
        /*
         * Write to Ingress EGRESS_ETPP_FORWARD_CONTEXT_SELECTION table
         */
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            SHR_IF_ERR_EXIT(forward_context_id_map_dbal_configure
                            (unit, forward_code_id_map[entry].pph_forwarding_layer_additional_info,
                             forward_code_id_map[entry].pph_forwarding_layer_additional_info_mask,
                             forward_code_id_map[entry].forward_layer_type,
                             forward_code_id_map[entry].forward_layer_type_mask,
                             forward_code_id_map[entry].tm_action_type,
                             forward_code_id_map[entry].tm_action_type_mask,
                             forward_code_id_map[entry].forward_code_port_profile,
                             forward_code_id_map[entry].forward_code_port_profile_mask,
                             forward_code_id_map[entry].is_tdm, forward_code_id_map[entry].is_tdm_mask,
                             forward_code_id_map[entry].is_applet, forward_code_id_map[entry].is_applet_mask,
                             forward_code_id_map[entry].forward_layer_qualifier,
                             forward_code_id_map[entry].forward_layer_qualifier_mask,
                             forward_code_id_map[entry].ingress_trapped_by_fhei_type,
                             forward_code_id_map[entry].ingress_trapped_by_fhei_type_mask,
                             forward_code_id_map[entry].ace_value,
                             forward_code_id_map[entry].ace_value_mask,
                             forward_code_id_map[entry].forward_code, 1, entry));
        }
        else
        {
            SHR_IF_ERR_EXIT(forward_context_id_map_dbal_configure
                            (unit, forward_code_id_jr1_mode_map[entry].pph_forwarding_layer_additional_info,
                             forward_code_id_jr1_mode_map[entry].pph_forwarding_layer_additional_info_mask,
                             forward_code_id_jr1_mode_map[entry].forward_layer_type,
                             forward_code_id_jr1_mode_map[entry].forward_layer_type_mask,
                             forward_code_id_jr1_mode_map[entry].tm_action_type,
                             forward_code_id_jr1_mode_map[entry].tm_action_type_mask,
                             forward_code_id_jr1_mode_map[entry].forward_code_port_profile,
                             forward_code_id_jr1_mode_map[entry].forward_code_port_profile_mask,
                             forward_code_id_jr1_mode_map[entry].is_tdm,
                             forward_code_id_jr1_mode_map[entry].is_tdm_mask,
                             forward_code_id_jr1_mode_map[entry].is_applet,
                             forward_code_id_jr1_mode_map[entry].is_applet_mask,
                             forward_code_id_jr1_mode_map[entry].forward_layer_qualifier,
                             forward_code_id_jr1_mode_map[entry].forward_layer_qualifier_mask,
                             forward_code_id_jr1_mode_map[entry].ingress_trapped_by_fhei_type,
                             forward_code_id_jr1_mode_map[entry].ingress_trapped_by_fhei_type_mask,
                             forward_code_id_jr1_mode_map[entry].ace_value,
                             forward_code_id_jr1_mode_map[entry].ace_value_mask,
                             forward_code_id_jr1_mode_map[entry].forward_code, 1, entry));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        EGRESS_PER_FORWARD_CODE
 * \param [in] unit - Relevant unit.
 * \param [in] forward_code - Forward Code, index to the register.
 * \param [in] termination_enable - Enable/Disable the Termination per FWD Code.
 * \param [in] protocol_ethernet_enable - Enable/Disable in case the FWD Code is Ethernet.
 * \param [in] protocol_ipv4_uc_enable - Enable/Disable in case the FWD Code is IPv4_UC.
 * \param [in] protocol_ipv4_mc_enable - Enable/Disable in case the FWD Code is IPv4_MC.
 * \param [in] protocol_ipv6_uc_enable - Enable/Disable in case the FWD Code is IPv6_UC.
 * \param [in] protocol_ipv6_mc_enable - Enable/Disable in case the FWD Code is IPv6_MC.
 * \param [in] protocol_mpls_enable - Enable/Disable in case the FWD Code is MPLS.
 * \param [in] stp_enable - Enable/Disable STP.
 * \param [in] vlan_membership_enable - Enable/Disable Egress Membership filter per Forward Code.
 * \param [in] acceptable_frame_type_enable - Enable/Disable acceptable frame type per FWD Code.
 * \param [in] split_horizon_enable - Enable/Disable split horizon per FWD Code.
 * \param [in] learn_enable - Enable/Disable learning per FWD Context.
 * \param [in] fwd_context_is_pp_enable - Enable/Disable FWD context is pp.
 * \param [in] fwd_context_is_ipv4_or_ipv6 - Indicates whether the fwd code is ipv4 or ipv6.

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
egress_per_forward_code_configuration_table_configure(
    int unit,
    int forward_code,
    int termination_enable,
    int protocol_ethernet_enable,
    int protocol_ipv4_uc_enable,
    int protocol_ipv4_mc_enable,
    int protocol_ipv6_uc_enable,
    int protocol_ipv6_mc_enable,
    int protocol_mpls_enable,
    int stp_enable,
    int vlan_membership_enable,
    int acceptable_frame_type_enable,
    int split_horizon_enable,
    int learn_enable,
    int fwd_context_is_pp_enable,
    int fwd_context_is_ipv4_or_ipv6)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to EGRESS_PER_FORWARD_CODE register
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_FORWARD_CODE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, forward_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_ENABLE, INST_SINGLE, termination_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETHERNET_ENABLE, INST_SINGLE,
                                 protocol_ethernet_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_UC_ENABLE, INST_SINGLE,
                                 protocol_ipv4_uc_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_MC_ENABLE, INST_SINGLE,
                                 protocol_ipv4_mc_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_UC_ENABLE, INST_SINGLE,
                                 protocol_ipv6_uc_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_MC_ENABLE, INST_SINGLE,
                                 protocol_ipv6_mc_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_ENABLE, INST_SINGLE,
                                 protocol_mpls_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STP_ENABLE, INST_SINGLE, stp_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE,
                                 vlan_membership_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE,
                                 acceptable_frame_type_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                 split_horizon_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE_PER_FWD_CONTEXT, INST_SINGLE,
                                 learn_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_IS_PP_ENABLE, INST_SINGLE,
                                 fwd_context_is_pp_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_IPV4_OR_IPV6, INST_SINGLE,
                                 fwd_context_is_ipv4_or_ipv6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init EGRESS_PER_FORWARD_CODE register.
 * The function reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
shr_error_e static
egress_per_forward_code_configuration_table_init(
    int unit)
{

    int nof_termination_per_forward_code_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_termination_per_forward_code_entries =
        sizeof(egress_per_fwd_code_config_table_map) / sizeof(egress_per_fwd_code_config_table_t);

    for (entry = 0; entry < nof_termination_per_forward_code_entries; entry++)
    {
        /*
         * Write to Egress EGRESS_PER_FORWARD_CODE table
         */
        SHR_IF_ERR_EXIT(egress_per_forward_code_configuration_table_configure
                        (unit, egress_per_fwd_code_config_table_map[entry].forward_code,
                         egress_per_fwd_code_config_table_map[entry].termination_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_ethernet_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_ipv4_uc_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_ipv4_mc_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_ipv6_uc_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_ipv6_mc_enable,
                         egress_per_fwd_code_config_table_map[entry].protocol_mpls_enable,
                         egress_per_fwd_code_config_table_map[entry].stp_enable,
                         egress_per_fwd_code_config_table_map[entry].vlan_membership_enable,
                         egress_per_fwd_code_config_table_map[entry].acceptable_frame_type_enable,
                         egress_per_fwd_code_config_table_map[entry].split_horizon_enable,
                         egress_per_fwd_code_config_table_map[entry].learn_enable,
                         egress_per_fwd_code_config_table_map[entry].fwd_context_is_pp_enable,
                         egress_per_fwd_code_config_table_map[entry].fwd_context_is_ipv4_or_ipv6));
    }

exit:
    SHR_FUNC_EXIT;
}

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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_STAMP_FTMH_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_FIXED_LATENCY_EN, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_OFFSET, INST_SINGLE, 2);
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

    uint32 entry_handle_id;
    int system_headers_mode;
    /*
     * For JR1 compat mode
     */
    uint32 udh_size = 0;
    uint32 lb_key_ext_size_bytes = 0, stacking_ext_size_bytes = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PARSER_GLOBAL, &entry_handle_id));
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
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     0x18C6318C63);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_OFFSET, INST_SINGLE,
                                     10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_SIZE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_LAYER_IDX,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_HEADER_IDX,
                                     INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 10 + FTMH_LB_KEY_EXT_SIZE + STACKING_EXT_SIZE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_SIZE, INST_SINGLE,
                                     6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_HEADER_IDX, INST_SINGLE, 3);
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
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_OFFSET, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EXT_CG_1_SIZE, INST_SINGLE, 19);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_LAYER_IDX, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_BASE_CG_1_OFFSET, INST_SINGLE, 1);
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
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_LB_KEY_EXT_CG_1_MAPPED_SIZE, INST_SINGLE, 0x20);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_HEADER_IDX, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_OFFSET, INST_SINGLE, 10);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_EXT_CG_1_SIZE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_HEADER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 9 + lb_key_ext_size_bytes + stacking_ext_size_bytes);
        /*
         * Map system_header_configuration.FTMH_STACKING_EXT_ENABLE to field size(3b--->5b)
         * 0: 0Byte
         * 2: 2Byte
         */
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_STACKING_EXT_CG_1_MAPPED_SIZE, INST_SINGLE,
                                     0x800);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_HEADER_IDX, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_OFFSET, INST_SINGLE, 10);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIER_EXT_CG_1_SIZE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_LAYER_IDX,
                                     INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_HEADER_IDX,
                                     INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_OFFSET,
                                     INST_SINGLE, 10);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_SPECIFIC_EXT_CG_1_SIZE, INST_SINGLE,
                                     0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_LAYER_IDX, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_HEADER_IDX, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSH_CG_1_OFFSET, INST_SINGLE, 9);
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
     * This configuration is required for ERPP blocks.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NO_GLEM_ACCESS_USE_GLOBAL_OUTLIF, INST_SINGLE, 1);

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
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERSYSHDRS, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FTMH_EXT_SIZE_IN_BYTES, INST_SINGLE,
                                     lb_key_ext_size_bytes + stacking_ext_size_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_UDH_SIZE_IN_BYTES, INST_SINGLE, udh_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    /*
     * }
     * Jericho mode end
     */

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
        decision = (outlif_decision_t *) outlif_decision_map_jer2_mode;
        nof_outlif_decision_entries = sizeof(outlif_decision_map_jer2_mode) / sizeof(outlif_decision_t);
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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TM_DESTINATION_EXT_PRESENT, INST_SINGLE, 0);
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
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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
 *        DBAL_TABLE_JR_PPH_FWD_CODE_MAPPING_TABLE
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
ingress_lbp_jr_pph_fwd_code_map_table_configure(
    int unit,
    dbal_enum_value_field_layer_types_e layer_type,
    int fwd_layer_qual_lsb,
    dbal_enum_value_field_jr_fwd_code_e fwd_code)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR_PPH_FWD_CODE_MAPPING_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_PPH_FWD_CODE_MAPPING_TABLE, &entry_handle_id));

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
 *        DBAL_TABLE_JR_PPH_VSI_SRC_MAPPING_TABLE
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
ingress_lbp_jr_pph_vsi_src_map_table_configure(
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
     * Write default values to DBAL_TABLE_JR_PPH_FWD_CODE_MAPPING_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_PPH_VSI_SRC_MAPPING_TABLE, &entry_handle_id));

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
 *        JR_PPH_FHEI_SIZE_MAPPING
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
ingress_lbp_jr_pph_fhei_size_map_table_configure(
    int unit)
{

    uint32 entry_handle_id;
    dbal_enum_value_field_jr_fwd_code_e fwd_code_index;
    int nof_entries, entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_JR_PPH_FHEI_SIZE_MAPPING table
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_PPH_FHEI_SIZE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_SIZE_3B);
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

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_FHEI_SIZE_NULL);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
    }

    nof_entries = sizeof(jr_mode_pph_fhei_size_map) / sizeof(jr_mode_pph_fhei_size_mapping_config_t);

    for (entry = 0; entry < nof_entries; entry++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE,
                                   jr_mode_pph_fhei_size_map[entry].jr_fwd_code);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_EQ_ZERO,
                                   jr_mode_pph_fhei_size_map[entry].inlif_eq_zero);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_LAYER_IDX,
                                   jr_mode_pph_fhei_size_map[entry].fwd_layer_idx);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_SIZE, INST_SINGLE,
                                     jr_mode_pph_fhei_size_map[entry].fhei_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

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
ingress_lbp_jer_mode_pph_configuration_init(
    int unit)
{
    int nof_entries;
    int entry;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_EXIT();
    }

    /*
     * Initialize entries according to table
     */
    nof_entries = sizeof(jr_mode_pph_fwd_code_map) / sizeof(jr_mode_pph_fwd_code_mapping_config_t);

    for (entry = 0; entry < nof_entries; entry++)
    {
        /*
         * Write to Ingress DBAL_TABLE_JR_PPH_FWD_CODE_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(ingress_lbp_jr_pph_fwd_code_map_table_configure
                        (unit, jr_mode_pph_fwd_code_map[entry].fwd_layer_type,
                         jr_mode_pph_fwd_code_map[entry].fwd_layer_qual_lsb,
                         jr_mode_pph_fwd_code_map[entry].jr_fwd_code));
    }

    /*
     * Initialize entries according to table
     */
    nof_entries = sizeof(jr_mode_pph_vsi_src_map) / sizeof(jr_mode_pph_vsi_src_mapping_config_t);

    for (entry = 0; entry < nof_entries; entry++)
    {
        /*
         * Write to Ingress DBAL_TABLE_JR_PPH_VSI_SRC_MAPPING_TABLE table
         */
        SHR_IF_ERR_EXIT(ingress_lbp_jr_pph_vsi_src_map_table_configure
                        (unit, jr_mode_pph_vsi_src_map[entry].jr_fwd_code,
                         jr_mode_pph_vsi_src_map[entry].is_mc,
                         jr_mode_pph_vsi_src_map[entry].outlif_is_rif, jr_mode_pph_vsi_src_map[entry].jr_vsi_src));
    }

    SHR_IF_ERR_EXIT(ingress_lbp_jr_pph_fhei_size_map_table_configure(unit));

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
    int system_headers_mode;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_ITPP_GENERAL_CFG table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ITPP_GENERAL_CFG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_CUD_INVALID_VALUE, INST_SINGLE, 0);
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
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
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
    int entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE, &entry_handle_id));
    /*
     * Initialize entries according to table
     * Start of 1 to skip the NONE value
     */
    for (entry = 1; entry < DBAL_NOF_ENUM_EEDB_TYPE_VSD_ENABLE_VALUES; entry++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, entry);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_TUNNEL_ADD_HEADERS_PROF_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ADDITIONAL_HEADER_PROFILE,
                               lif_additional_headers_profile);
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
        nof_global_inlif_config_entries = sizeof(inlif_decision_config_map_jer2_mode) / sizeof(inlif_decision_config_t);
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
                            (unit, inlif_decision_config_map_jer2_mode[entry].system_headers_mode,
                             inlif_decision_config_map_jer2_mode[entry].is_mc_forward_code,
                             inlif_decision_config_map_jer2_mode[entry].inlif_decision_0,
                             inlif_decision_config_map_jer2_mode[entry].inlif_decision_1));
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
 * \param [in] prt_recycle_profile - PRT Recycle Profile
 * \param [in] port_termination_ptc_profile - Port Termination PTC Profile
 * \param [in] prt_qualifier_mode - PRT Qualifier Mode
 * \param [in] layer_offset - Layer Offset
 * \param [in] kbr_valid_bitmap_4 - Value of KBR_VALID_BITMAP_4
 * \param [in] kbr_valid_bitmap_3 - Value of KBR_VALID_BITMAP_3
 * \param [in] kbr_valid_bitmap_2 - Value of KBR_VALID_BITMAP_2
 * \param [in] kbr_valid_bitmap_1 - Value of KBR_VALID_BITMAP_1
 * \param [in] ffc_instruction_3 - Value of FFC_INSTRUCTION_3
 * \param [in] ffc_instruction_2 - Value of FFC_INSTRUCTION_2
 * \param [in] ffc_instruction_1 - Value of FFC_INSTRUCTION_1
 * \param [in] ffc_instruction_0 - Value of FFC_INSTRUCTION_0
 * \param [in] src_system_port_prt_mode - Source system port PRT mode
 * \param [in] pp_port_enable - Value of PP_PORT_ENABLE
 * \param [in] tcam_mode_enable - Value of TCAM_MODE_FULL_ENABLE. '1' means enabled.
 * \param [in] context_sel - Decide PRT parsing-context mode. '1' means KBR , 0 from PP port.
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
ingress_prt_profile_configuration(
    int unit,
    int prt_recycle_profile,
    int port_termination_ptc_profile,
    int prt_qualifier_mode,
    int layer_offset,
    int kbr_valid_bitmap_4,
    int kbr_valid_bitmap_3,
    int kbr_valid_bitmap_2,
    int kbr_valid_bitmap_1,
    int ffc_instruction_3,
    int ffc_instruction_2,
    int ffc_instruction_1,
    int ffc_instruction_0,
    int src_system_port_prt_mode,
    int pp_port_enable,
    int tcam_mode_enable,
    int context_sel)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to INGRESS_PRT_INFO table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PRT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_RECYCLE_PROFILE, prt_recycle_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE,
                               port_termination_ptc_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_MODE, INST_SINGLE, prt_qualifier_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_OFFSET, INST_SINGLE, layer_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 4, kbr_valid_bitmap_4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 3, kbr_valid_bitmap_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 2, kbr_valid_bitmap_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_VALID_BITMAP, 1, kbr_valid_bitmap_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 3, ffc_instruction_3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 2, ffc_instruction_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 1, ffc_instruction_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_INSTRUCTION, 0, ffc_instruction_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT_PRT_MODE, INST_SINGLE,
                                 src_system_port_prt_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT_ENABLE, INST_SINGLE, pp_port_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_MODE_FULL_ENABLE, INST_SINGLE,
                                 tcam_mode_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PRT_MODE, INST_SINGLE, context_sel);
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
    int entry;
    int nof_entry;
    const prt_profile_config_t *prt_info_map;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_J2C(unit))
    {
        nof_entry = sizeof(j2c_prt_info_map) / sizeof(prt_profile_config_t);
        prt_info_map = j2c_prt_info_map;
    }
    else
    {
        nof_entry = sizeof(jr2_prt_info_map) / sizeof(prt_profile_config_t);
        prt_info_map = jr2_prt_info_map;
    }

    /*
     * Initialize entries according to table
     */
    for (entry = 0; entry < nof_entry; entry++)
    {
        /*
         * Write to index 0, 16, 28, 29, 32, 62 on INGRESS_PRT_PROFILE table
         */
        SHR_IF_ERR_EXIT(ingress_prt_profile_configuration(unit, prt_info_map[entry].prt_recycle_profile,
                                                          prt_info_map[entry].port_termination_ptc_profile,
                                                          prt_info_map[entry].prt_qualifier_mode,
                                                          prt_info_map[entry].layer_offset,
                                                          prt_info_map[entry].kbr_valid_bitmap_4,
                                                          prt_info_map[entry].kbr_valid_bitmap_3,
                                                          prt_info_map[entry].kbr_valid_bitmap_2,
                                                          prt_info_map[entry].kbr_valid_bitmap_1,
                                                          prt_info_map[entry].ffc_instruction_3,
                                                          prt_info_map[entry].ffc_instruction_2,
                                                          prt_info_map[entry].ffc_instruction_1,
                                                          prt_info_map[entry].ffc_instruction_0,
                                                          prt_info_map[entry].src_system_port_prt_mode,
                                                          prt_info_map[entry].pp_port_enable,
                                                          prt_info_map[entry].tcam_mode_enable,
                                                          prt_info_map[entry].context_sel));
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
    uint32 entry_handle_id;
    uint32 fodo_assignment_mode_id;
    uint32 fodo_assignment_modes[] = { DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LIF,
        DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LOOKUP,
        DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_VLAN
    };
    uint32 nof_fodo_assignment_modes = (sizeof(fodo_assignment_modes) / sizeof(fodo_assignment_modes[0]));
    uint32 fodo_assignment_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * init forwarding domain mask profiles mapping table.  This is 1:1 mapping lif.forwarding_domain_assignment_mode to
     * forwarding domain mask profile.  We ignore the priority decoder result type key
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_MODE_MAPPING, &entry_handle_id));

    for (fodo_assignment_mode_id = 0; fodo_assignment_mode_id < nof_fodo_assignment_modes; fodo_assignment_mode_id++)
    {
        fodo_assignment_mode = fodo_assignment_modes[fodo_assignment_mode_id];

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, fodo_assignment_mode);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_TYPE, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_PROFILE, INST_SINGLE,
                                     fodo_assignment_mode);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_PROFILE, &entry_handle_id));

    /** init forwarding domain masks profile mapping table  */

    /*
     * for "Forwarding domain equal to Forwarding domain in LIF table" mode: the result from LIF table is masked
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_PROFILE,
                               DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LIF);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MIN_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * for "Forwarding domain equal to LIF-index" mode: the result from SEM table is masked PD1 set the result of SEM
     * table as PD1 result.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_PROFILE,
                               DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LOOKUP);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MIN_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * for "Forwarding domain equal to Forwarding domain in LIF table + VID" mode: we mask both the result from LIF
     * table and the result from SEM table. PD1 set the VID as PD1 result.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_PROFILE,
                               DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_VLAN);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    /*
     * only vlan is masked.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK, INST_SINGLE, BCM_VLAN_MAX);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
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
    uint64 protocol_specific_information_add_header_0,
    dbal_fields_e protocol_specific_information_add_header_0_type,
    int start_current_next_protocol_enable_add_header_1,
    int start_current_next_protocol_add_header_1,
    int additional_headers_packet_size_enable_add_header_1,
    int additional_headers_packet_size_add_header_1,
    uint64 protocol_specific_information_add_header_1,
    dbal_fields_e protocol_specific_information_add_header_1_type,
    int start_current_next_protocol_enable_add_header_2,
    int start_current_next_protocol_add_header_2,
    int additional_headers_packet_size_enable_add_header_2,
    int additional_headers_packet_size_add_header_2,
    uint64 protocol_specific_information_add_header_2,
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
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_additional_headers_map_tables_entries = sizeof(additional_headers_map) / sizeof(additional_headers_map_t);

    for (entry = 0; entry < nof_additional_headers_map_tables_entries; entry++)
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
 * \brief - The function configures initial values for
 *        DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL
 * \param [in] unit - Relevant unit.
 * \param [in] layer_type_enum_val - The LAYER_TYPE
 * \param [in] is_ethernet - ETHERNET type
 * \param [in] is_arp - ARP type
 * \param [in] is_ipv4 - IPv4 type
 * \param [in] is_ipv6 - IPv6 type
 * \param [in] is_mpls - MPLS type
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
    int is_mpls,
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Write default values to DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_ENABLE, INST_SINGLE, is_ethernet);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_ENABLE, INST_SINGLE, is_mpls);
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
                                                                 per_layer_protocol[entry].is_mpls,
                                                                 per_layer_protocol[entry].is_udp,
                                                                 per_layer_protocol[entry].is_bfd_single_hop,
                                                                 per_layer_protocol[entry].is_bfd_multi_hop,
                                                                 per_layer_protocol[entry].is_icmpv6,
                                                                 per_layer_protocol[entry].is_igmp,
                                                                 per_layer_protocol[entry].is_8021_x_2,
                                                                 per_layer_protocol[entry].is_icmp,
                                                                 per_layer_protocol[entry].is_bier_mpls));
    }

    for (entry = 0; entry < nof_layres_to_config; entry++)
    {
        /*
         * Write to the indexes of EGRESS_PER_LAYER_PROTOCOL table
         */
        SHR_IF_ERR_EXIT(egress_per_layer_protocol_configuration(unit, per_layer_protocol[entry].layer_type_enum_val,
                                                                per_layer_protocol[entry].is_ipv4,
                                                                per_layer_protocol[entry].is_ipv6,
                                                                per_layer_protocol[entry].is_mpls,
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_FUNC_INIT_VARS(unit);

    nof_ctx_to_config = sizeof(per_2nd_stage_parsing_ctx) / sizeof(per_2nd_stage_parsing_ctx_t);

    /*
     * Write default values to INGRESS_MAP_PARSING_CTX_LAYER_PROTOCOL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_MAP_PARSING_CTX_LAYER_PROTOCOL, &entry_handle_id));
    for (ii = 0; ii < nof_ctx_to_config; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_2ND_PARSER_PARSER_CONTEXT,
                                   per_2nd_stage_parsing_ctx[ii].parser_ctx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES_BEFORE, INST_SINGLE,
                                     per_2nd_stage_parsing_ctx[ii].layer_type_before);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES_AFTER, INST_SINGLE,
                                     per_2nd_stage_parsing_ctx[ii].layer_type_after);
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
egress_global_esem_configuration_init(
    int unit)
{
    uint32 entry_handle_id;
    int entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ESEM_GLOBAL, &entry_handle_id));
    /*
     * Initialize entries according to table
     * Start of 1 to skip the NONE value
     */
    for (entry = 1; entry < DBAL_NOF_ENUM_EEDB_TYPE_EM_BUSTER_VALUES; entry++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, entry);
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
    int outlif_profile_width;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the maximum number of RIFs from dnx_data */
    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);

    /** Get the local outlif and local outlif profile widths*/
    local_outlif_width = dnx_data_lif.out_lif.local_outlif_width_get(unit);
    outlif_profile_width = dnx_data_lif.out_lif.outlif_profile_width_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_GLOBAL, &entry_handle_id));
    /*
     * Set the maximum number of RIFs
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_RIFS, INST_SINGLE, nof_rifs);
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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLP_FULL_THRESHOLD, INST_SINGLE, FLP_FULL_THRESHOLD);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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
    }

exit:
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
    int system_headers_mode, same_interface_bits;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    same_interface_bits = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_SAME_INTERFACE_FILTER, &entry_handle_id));
    /**
     * Enabling the same interface per  the following configuration:
     * Ports-Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = (Device/System scope), SYS_PORT_EQUAL = 1, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_LIF_PROFILE);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_LIF_PROFILE_JR_MODE_ENABLED);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = (Device/System scope), SYS_PORT_EQUAL = 1, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_LIF_PROFILE);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_JR_MODE_LIF_PROFILE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_JR_MODE_LIF_PROFILE_DISABLE);

    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-Equal && Out-LIF-is-Port && In-LIF-is-Port
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = (Device/System scope), SYS_PORT_EQUAL = 1, INLIF_IS_PORT = 1, OUTLIF_IS_PORT = 1
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
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_LIF_PROFILE);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   DEVICE_SCOPE_JR_MODE_LIF_PROFILE);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-are not Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = 2'b01, SYS_PORT_EQUAL = 0, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
     * Position of the enable bit is:
     * LIFS_EQUAL = 1, SAME_IF_MAPPED_BY_OUTLIF_PROFILE = 2'b00, SAME_IF_FILTER_MAPPED_BY_FWD_CONTEXT = 2'b01.
     * When the Forwarding Context is Ethernet or MPLS - the same interface is enabled.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   SYSTEM_SCOPE_LIF_PROFILE);
    }
    else
    {
        if (same_interface_bits == 3)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ONLY_SYS_SCOPE_ENABLED);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED);
        }
    }
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Enabling the same interface per  the following configuration:
     * Ports-are not Equal && !Out-LIF-is-Port && !In-LIF-is-Port && LIFs-Equal
     * In this case the address to the memory is indexed as follows:
     * SAME_IF_MAPPED_BY_INLIF_PROFILE = 2'b01, SYS_PORT_EQUAL = 0, INLIF_IS_PORT = 0, OUTLIF_IS_PORT = 0
     * Position of the enable bit is:
     * LIFS_EQUAL = 1, SAME_IF_MAPPED_BY_OUTLIF_PROFILE = 2'b00, SAME_IF_FILTER_MAPPED_BY_FWD_CONTEXT = 2'b01.
     * When the Forwarding Context is Ethernet or MPLS - the same interface is enabled.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_EQUAL, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_IS_PORT, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_IS_PORT, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_FILTER_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFS_EQUAL, TRUE);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                   SYSTEM_SCOPE_LIF_PROFILE);
    }
    else
    {
        if (same_interface_bits == 3)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_2B_ENABLED);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE_FILTER,
                                       SYSTEM_SCOPE_LIF_PROFILE_JR_MODE_1B_ENABLED);
        }
    }
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_SAME_INTERFACE_FILTER, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
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
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, TRUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set termination when routing-Enable and ptc-profile is 2ND_PASS_DROP_AND_CONTINUE
     * For RCH termination, in drop and continue application, at 2nd pass
     * ptc profile value is set by bcm_port_control_set with type bcmPortControlOverlayRecycle
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE,
                               DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_2ND_PASS_DROP_AND_CONTINUE);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set trap enable when no Routing-Enable and My-MAC is on
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTEXT_L3_LIF, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ROUTING_ENABLE, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MY_MAC, TRUE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAYER_TERMINATION, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS_TERMINATION_TRAP_ENABLE, INST_SINGLE,
                                 TRUE);
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
 * \brief -
 * Init erpp_filter_per_fwd_context_map table.
 * Based on the incoming parameters the function configures the ERPP Filter per fwd context
 *
 * \param [in] unit                    -    Relevant unit.
 * \param [in] forward_code            -    Forward Code.
 * \param [in] tdm_discard             -    TDM Discard Enable/Disable.
 * \param [in] rqp_discard             -    RQP Discard Enable/Disable.
 * \param [in] invalid_otm             -    Invalid OTM Enable/Disable.
 * \param [in] dss_stacking            -    DSS Stacking Enable/Disable.
 * \param [in] lag_multicast           -    LAG Multicast Enable/Disable.
 * \param [in] exclude_src             -    Exclude Src Enable/Disable.
 * \param [in] same_interface          -    Same interface Enable/Disable.
 * \param [in] unacceptable_frame_type -    Unacceptable Frame type Enable/Disable.
 * \param [in] unknown_da              -    Unknown DA Enable/Disable.
 * \param [in] split_horizon           -    Split Horizon Enable/Disable.
 * \param [in] glem_pp_trap            -    GLEM PP trap Enable/Disable.
 * \param [in] glem_non_pp_trap        -    GLEM NON PP trap Enable/Disable.
 * \param [in] ttl_scoping             -    TTL Scoping Enable/Disable.
 * \param [in] ttl_zero                -    TTL Zero Enable/Disable.
 * \param [in] ttl_one                 -    TTL One Enable/Disable.
 * \param [in] mtu_violation           -    MTU Violation Enable/Disable.
 * \param [in] ipv4_filters            -    IPV4 Filters Enable/Disable.
 * \param [in] ipv6_filters            -    IPV6 Filters Enable/Disable.
 * \param [in] layer_4_filters         -    Layer 4 Filters Enable/Disable.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */
static shr_error_e
egress_erpp_filter_per_fwd_context_map_configure(
    int unit,
    dbal_enum_value_field_egress_fwd_code_e forward_code,
    int tdm_discard,
    int rqp_discard,
    int invalid_otm,
    int dss_stacking,
    int lag_multicast,
    int exclude_src,
    int same_interface,
    int unacceptable_frame_type,
    int unknown_da,
    int split_horizon,
    int glem_pp_trap,
    int glem_non_pp_trap,
    int ttl_scoping,
    int ttl_zero,
    int ttl_one,
    int mtu_violation,
    int ipv4_filters,
    int ipv6_filters,
    int layer_4_filters)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to Ingress EGRESS_ETPP_FORWARD_CONTEXT_SELECTION table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_FWD_CONTEXT_FILTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE, forward_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_DISCARD, INST_SINGLE, tdm_discard);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RQP_DISCARD, INST_SINGLE, rqp_discard);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_OTM, INST_SINGLE, invalid_otm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSS_STACKING, INST_SINGLE, dss_stacking);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_MULTICAST, INST_SINGLE, lag_multicast);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXCLUDE_SRC, INST_SINGLE, exclude_src);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE, INST_SINGLE, same_interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNACCEPTABLE_FRAME_TYPE, INST_SINGLE,
                                 unacceptable_frame_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_DA, INST_SINGLE, unknown_da);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON, INST_SINGLE, split_horizon);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_PP_TRAP, INST_SINGLE, glem_pp_trap);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_NON_PP_TRAP, INST_SINGLE, glem_non_pp_trap);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_SCOPING, INST_SINGLE, ttl_scoping);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ZERO, INST_SINGLE, ttl_zero);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ONE, INST_SINGLE, ttl_one);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_VIOLATION, INST_SINGLE, mtu_violation);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_FILTERS, INST_SINGLE, ipv4_filters);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_FILTERS, INST_SINGLE, ipv6_filters);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_4_FILTERS, INST_SINGLE, layer_4_filters);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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

    int nof_filter_per_fwd_context_table_entries;
    int entry;
    int system_headers_mode;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize entries according to table
     */
    nof_filter_per_fwd_context_table_entries =
        sizeof(erpp_filter_per_fwd_context_map) / sizeof(erpp_filter_per_fwd_context_t);

    for (entry = 0; entry < nof_filter_per_fwd_context_table_entries; entry++)
    {
        /*
         * Write to Egress ERPP_FWD_CONTEXT_FILTER table
         */
        SHR_IF_ERR_EXIT(egress_erpp_filter_per_fwd_context_map_configure
                        (unit, erpp_filter_per_fwd_context_map[entry].forward_code,
                         erpp_filter_per_fwd_context_map[entry].tdm_discard,
                         erpp_filter_per_fwd_context_map[entry].rqp_discard,
                         erpp_filter_per_fwd_context_map[entry].invalid_otm,
                         erpp_filter_per_fwd_context_map[entry].dss_stacking,
                         erpp_filter_per_fwd_context_map[entry].lag_multicast,
                         erpp_filter_per_fwd_context_map[entry].exclude_src,
                         erpp_filter_per_fwd_context_map[entry].same_interface,
                         erpp_filter_per_fwd_context_map[entry].unacceptable_frame_type,
                         erpp_filter_per_fwd_context_map[entry].unknown_da,
                         erpp_filter_per_fwd_context_map[entry].split_horizon,
                         erpp_filter_per_fwd_context_map[entry].glem_pp_trap,
                         erpp_filter_per_fwd_context_map[entry].glem_non_pp_trap,
                         erpp_filter_per_fwd_context_map[entry].ttl_scoping,
                         erpp_filter_per_fwd_context_map[entry].ttl_zero,
                         erpp_filter_per_fwd_context_map[entry].ttl_one,
                         erpp_filter_per_fwd_context_map[entry].mtu_violation,
                         erpp_filter_per_fwd_context_map[entry].ipv4_filters,
                         erpp_filter_per_fwd_context_map[entry].ipv6_filters,
                         erpp_filter_per_fwd_context_map[entry].layer_4_filters));
    }

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
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ZERO, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL
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
egress_parsing_start_type_map_table_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode;
    int nof_contexts_in_struct;
    int context_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));

        nof_contexts_in_struct =
            sizeof(jr2_mode_egress_parsing_context_map) / sizeof(layer_type_to_erpp_1st_parsing_context_t);

        for (context_idx = 0; context_idx < nof_contexts_in_struct; context_idx++)
        {

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES,
                                       jr2_mode_egress_parsing_context_map[context_idx].layer_type);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_START_TYPE, INST_SINGLE,
                                         jr2_mode_egress_parsing_context_map[context_idx].parser_context);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_JR_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
        nof_contexts_in_struct =
            sizeof(jr_mode_egress_parsing_context_map) / sizeof(jr_mode_fwd_code_to_erpp_1st_parsing_context_t);

        for (context_idx = 0; context_idx < nof_contexts_in_struct; context_idx++)
        {
            /** in JR mode, the mapping is done via Fwd_Code to Context and not via Layer_Type as in JR2 mode */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_FWD_CODE,
                                       jr_mode_egress_parsing_context_map[context_idx].fwd_code);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_START_TYPE, INST_SINGLE,
                                         jr_mode_egress_parsing_context_map[context_idx].parser_context);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

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
    int jr_lb_key_mode = 0;
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
    max_vsi_value = dnx_data_l2.vsi.nof_vsis_get(unit);
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
        jr_lb_key_mode = dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get(unit);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                                     jr_lb_key_mode);

        /** Write stacking extension size in bytes */
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
                               DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /**Enable Same interface per BRIDGE IVL*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                               DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL);
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
/*
 * See h file for description
 */
shr_error_e
dnx_general_pp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ingress_prt_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_init(unit));
    SHR_IF_ERR_EXIT(ingress_lbp_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_eth_termination_mapping_table_init(unit));
    SHR_IF_ERR_EXIT(ingress_lbp_vlan_editing_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_lbp_jer_mode_pph_configuration_init(unit));
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_data_path_hw))
    {
        SHR_IF_ERR_EXIT(ingress_ippe_traffic_enable(unit));
    }
    SHR_IF_ERR_EXIT(ingress_vtt_global_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_port_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_itpp_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_per_trap_context_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_egress_per_layer_protocol_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_egress_parser_configuration_init(unit));
    SHR_IF_ERR_EXIT(ingress_map_parsing_ctx_layer_potocol(unit));
    SHR_IF_ERR_EXIT(ingress_egress_forwarding_additional_info_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_forward_code_map_init(unit));
    SHR_IF_ERR_EXIT(egress_prp_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_parser_general_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_selected_outlif_table_init(unit));
    SHR_IF_ERR_EXIT(egress_system_headers_lif_extension_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_eedb_forwarding_domain_vsd_enable_init(unit));
    SHR_IF_ERR_EXIT(egress_system_header_generation_table_init(unit));
    SHR_IF_ERR_EXIT(egress_global_system_header_configuration_init(unit));

#ifdef DNX_EMULATION_1_CORE
    if (!(SOC_IS_JERICHO_2_A0(unit) && soc_sand_is_emulation_system(unit) != 0))
#endif
    {
        SHR_IF_ERR_EXIT(egress_global_inlif_resolution_table_init(unit));
    }

    SHR_IF_ERR_EXIT(egress_additional_headers_profile_table_init(unit));
    SHR_IF_ERR_EXIT(egress_global_esem_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_global_eedb_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_per_forward_code_configuration_table_init(unit));
    SHR_IF_ERR_EXIT(egress_current_next_protocol_config_table_init(unit));
    SHR_IF_ERR_EXIT(egress_additional_headers_map_tables_configuration_init(unit));
    SHR_IF_ERR_EXIT(egress_same_interface_filter_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_mapping_forward_context_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_ingress_trapped_by_fhei_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_erpp_filter_per_fwd_context_map_init(unit));
    SHR_IF_ERR_EXIT(egress_parsing_start_type_map_table_configure_init(unit));
    SHR_IF_ERR_EXIT(egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_context_ifwd2_kbp_acl_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_general_pp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The respective DeInit functions need to be put here.
     */
    SHR_FUNC_EXIT;
}
