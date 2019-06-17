/**
 * \file algo_bfd.h Internal DNX L3 Managment APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_BFD_H_INCLUDED
/*
 * { 
 */
#define ALGO_BFD_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/**
 * Resource name defines for algo bfd 
 * \see
 * dnx_bfd_init_templates 
 * {
 */
#define DNX_BFD_TEMPLATE_OAMA_ACTION               "BFD MEP OAMA Action"

#define DNX_BFD_TEMPLATE_OAMB_ACTION               "BFD MEP OAMB Action"

#define DNX_BFD_TEMPLATE_DIP                       "BFD DIP"

#define DNX_BFD_TEMPLATE_YOUR_DISCR_RANGE          "BFD Your-Discr Range"

#define DNX_BFD_OAMP_TEMPLATE_TX_RATE              "BFD OAMP TX Rate"

#define DNX_BFD_OAMP_TEMPLATE_REQ_INTERVAL         "BFD OAMP Required Interval"

#define DNX_BFD_OAMP_TEMPLATE_MY_DISCR_RANGE_START "BFD OAMP My-Discr Range Start"

#define DNX_BFD_OAMP_TEMPLATE_TOS_TTL              "BFD OAMP TOS-TTL"

#define DNX_BFD_OAMP_TEMPLATE_IPV4_SRC_ADDR        "BFD OAMP IPv4 Source Addr"

#define DNX_BFD_OAMP_TEMPLATE_UDP_SPORT            "BFD OAMP UDP SPORT"

#define DNX_BFD_OAMP_TEMPLATE_MPLS_SPORT           "BFD OAMP MPLS SPORT"

#define DNX_BFD_TEMPLATE_OAMP_SINGLE_HOP_TOS       "BFD OAMP SINGLE-HOP TOS"

/**
 * }
 */

/** Structure used to store BFD MEP profiles as templates */
typedef struct
{
    /** 
     *  The trap code points to a defined trap - a possible
     *  destination for a received packet.  In each stage of the
     *  pipeline different traps with different forwarding
     *  strengths are encountered.  The trap with the highest
     *  forwrarding strength wins.
     */
    uint16 trap_code;

    /** The forwarding strength for the above trap code.   */
    uint8 forwarding_stregth;

    /** 
     *  Similarly to trap strength, snoop strength is used to
     *  to decide the destination of a copy of the original
     *  packet.  While trap strength and snoop strength can be
     *  different, trap code is also used for snoop code, so at
     *  the end of the pipeline it is possible for the resulting
     *  trap code and snoop code to be different (because one
     *  stage of the pipeline had the highest trap strength,
     *  while a different stage had the highest snoop strength.)
     */
    uint8 snoop_strength;
} dnx_bfd_mep_profile_t;

/** 
 *  Structure used to store BFD discriminator range as
 *  template.  In BFD packets, the discriminator is used to
 *  establish a sequence of packets between two endpoints, where
 *  each packet contains a "my discriminator" and a "your
 *  discriminator" field.  If device X sends BFD packets to
 *  device Y with my discriminator=A and your-discriminator=B,
 *  device Y should send BFD packet to device X with the values
 *  inverted: my discriminator=B and your-discriminator=A.  This
 *  setting limits the values allowed for my-discriminator.
 *  These limits are applied to the most significant 18 bits in
 *  a 32 bit word.  Therefore, even if range_min=range_max
 *  (which is currently the case for dune devices) there are
 *  2^14 possible values for my-discriminator.
 */
typedef struct
{
    uint32 range_min;
    uint32 range_max;
} dnx_bfd_discr_range_t;

/** 
 *  Structure used to store profiles that contain "type of
 *  service/time to live" profiles as template.  These
 *  values are used by the OAMP to construct BFD packets for
 *  transmission.
 */
typedef struct
{
    uint8 tos;
    uint8 ttl;
} dnx_bfd_tos_ttl_t;

/**
 * }
 */

/**
 * \brief - initialize templates for all BFD profile types. 
 *        Current BFD Templates:\n
 *  BFD MEP OAMA Action - action profiles for non-accelerated
 *  BFD endpoints (stored in OAMA table.)\n
 *  BFD MEP OAMB Action - action profiles for accelerated BFD
 *  endpoints (either to local or remote OAMP, stored in OAMB
 *  table.)\n
 *  BFD DIP - Destination IPs.  In non-accelerated BFD
 *  endpoints, these are only relevant for multi-hop.
 *  BFD Your-Discr Range - The upper and lower values of the
 *  most significant 18 bits of the my-discriminator field.
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
shr_error_e dnx_algo_bfd_init(
    int unit);

/**
 * \brief - Nothing to do here.
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
shr_error_e dnx_algo_bfd_deinit(
    int unit);

/**
 * \brief - Printing callback for the BFD MEP action templates, 
 *        which are used to track the references to BFD profile
 *        entries in the OAMA and OAMB tables.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The BFD 
 *        action profile structure dnx_bfd_mep_profile_t is
 *        defined and described above.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */

void dnx_bfd_mep_action_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD DIP template, which is 
 *        use to track the refernces to entries in the BFD DIP
 *        hardware table by multihop BFD endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  This 
 *        data is in the format of an IPv6 address, and is
 *        displayed as such.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_dip_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD Discriminator Range 
 *        template, which is used to track the references to the
 *        discriminator range registers by BFD endpoints that
 *        receive packets classified by discriminator.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The BFD
 *        discriminator range structure dnx_bfd_discr_range_t is
 *        defined and described above.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_your_discr_range_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD Tx rate 
 *        template, which is used to configure packet
 *        transmissions for BFD accelerated endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The Tx 
 *        rate is a 10-bit integer.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_tx_rate_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD required interval 
 *        template, which is used to negotiate the rate that
 *        remote endpoints transmit to BFD accelerated
 *        endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. 
 *        Intervals are 32-bit integers.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_req_int_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD My Discriminator Range
 *        Start template, which is used to track the references
 *        to the discriminator range start registers by
 *        accekerated BFD endpoints that have IPv4 or MPLS
 *        tunneling.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. This 
 *        offset is a 32-bit integer.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_bfd_discr_range_start_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD ToS TTL template. 
 *        These two values are used by the OAMP to construct BFD
 *        packet to transmit for accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. These
 *        values are one byte each.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_tos_ttl_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD source address 
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an IPv4 address - 4 bytes.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_ipv4_src_addr_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD UDP source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an IPv4 address - 4 bytes.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM sdnx_bfd_udp_src_port_profile_print_cbhell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_udp_src_port_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD MPLS source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an IPv4 address - 4 bytes.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_mpls_src_port_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the BFD MPLS single hop TOS
 *        template. This TOS value is used by the OAMP to
 *        construct the BFD packet to transmit for a single-hop
 *        IPv4 accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a TOS - 1 byte.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_single_hop_tos_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
