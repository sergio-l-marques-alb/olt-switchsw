/**
 *  \file bcm_int/dnx/rx/rx.h 
 *  $Id$ 
 *  Internal DNX RX APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$ 
 */

#ifndef _RX_API_INCLUDED__
/*
 * { 
 */
#define _RX_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm/rx.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <include/bcm_int/dnx/algo/algo_gpm.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>

#define DNX_RX_ERPP_PMF_COUNTER_NOF                 (0x2)
#define DNX_RX_MTU_PROFILE_SHIFT                    (0x3)
#define DNX_RX_MTU_PROFILE_MASK                     (0x7)
#define DNX_RX_COMPRESSED_LAYER_TYPE_MASK       (0x7)

#define DNX_RX_MTU_ENTRY_INDEX_SET(_mtu_profile, _cmp_layer_type) \
                                    (((_mtu_profile & DNX_RX_MTU_PROFILE_MASK) << DNX_RX_MTU_PROFILE_SHIFT)   \
                                    | (_cmp_layer_type & DNX_RX_COMPRESSED_LAYER_TYPE_MASK))

#define DNX_RX_MTU_PROFILE_FROM_INDEX_GET(_index) \
		                                ((_index >> DNX_RX_MTU_PROFILE_SHIFT) & DNX_RX_MTU_PROFILE_MASK)

/**
 * snoop command reserved for snoop all packet to CPU
 */
#define DNX_RX_SNOOP_CMD_TO_CPU (1)

/**
  *\brief - data of a ERPP trap action 
  * Holds all ERPP trap actions configured by the user using
  * dnx_rx_trap_set()
 */
typedef struct dnx_rx_trap_erpp_config_s
{
    /** when set traffic class is valid */
    uint8 tc_valid;

    /** traffic class value */
    uint8 tc;

    /** when set, drop presedence is valid */
    uint8 dp_valid;

    /**  drop presedence value */
    uint8 dp;

    /** when set, quality of service is valid */
    uint8 qos_id_valid;

    /** quality of service value */
    uint8 qos_id;

    /** when set, PP DSP is valid */
    uint8 pp_dsp_valid;

    /** PP DSP value */
    uint8 pp_dsp;

    /** when set, packet is discarded */
    uint8 discard;

    /** When Snoop profile is valid*/
    uint8 snoop_profile_valid;

    /** Snoop profile */
    uint8 snoop_profile;

    /** When Mirror profile is valid*/
    uint8 mirror_profile_valid;

    /** Mirror profile */
    uint8 mirror_profile;

    /** When CUD Outlif_or_MCDB pointer is valid*/
    uint8 outlif_or_mcdb_ptr_valid;

    /** CUD Outlif_or_MCDB pointer */
    uint32 outlif_or_mcdb_ptr;

    /** When Counter_profile is valid*/
    uint8 counter_profile_valid[DNX_RX_ERPP_PMF_COUNTER_NOF];

    /** Counter_profile */
    uint8 counter_profile[DNX_RX_ERPP_PMF_COUNTER_NOF];

     /** When Counter_ptr is valid*/
    uint8 counter_ptr_valid[DNX_RX_ERPP_PMF_COUNTER_NOF];

    /** Counter_ptr */
    uint32 counter_ptr[DNX_RX_ERPP_PMF_COUNTER_NOF];

} dnx_rx_trap_erpp_config_t;

/**
 * \brief ETPP trap action data for template manager.
 */
typedef struct dnx_rx_trap_etpp_action_s
{
    /** When set packet is dropped */
    uint8 is_pkt_drop;

    /** When set, recycling packet is in high priority */
    uint8 is_recycle_high_priority;

    /** When set, only the first 128B of the Forward packet are copied to the recycle port */
    uint8 is_recycle_crop_pkt;

    /** When set, Aligner appends the original FTMH to the Recycle Packet */
    uint8 is_recycle_append_ftmh;

    /** forward recycle command. this field is used for providing additional information to IRPP */
    uint8 recycle_cmd;

    /** ingress trap id stamped on FHEI */
    dbal_enum_value_field_ingress_trap_id_e ingress_trap_id;
} dnx_rx_trap_etpp_action_t;

/**
 * \brief recycle trap configuration.
 */
typedef struct dnx_rx_trap_recycle_cmd_config_s
{
    /** forward recycle strength, used for providing the strength of recycle ingress trap */
    uint8 recycle_strength;

    /** ingress trap id to indicate the recycle ingress trap */
    dbal_enum_value_field_ingress_trap_id_e ingress_trap_id;
} dnx_rx_trap_recycle_cmd_config_t;

/**
 * \brief ETPP trap action profile configuration.
 */
typedef struct dnx_rx_trap_etpp_config_s
{
    /** ETPP trap action */
    dnx_rx_trap_etpp_action_t action;

    /** Recycle command configuration */
    dnx_rx_trap_recycle_cmd_config_t recycle_cmd_config;
} dnx_rx_trap_etpp_config_t;

/**
 * \brief MTU Profile configuration for Template manager.
 */
typedef struct dnx_rx_trap_mtu_profile_config_s
{
    /** MTU value */
    int mtu;

    /** Forward trap strength value */
    uint8 fwd_strength;

    /** Snoop trap strength value */
    uint8 snp_strength;

    /** Trap Action profile */
    uint8 trap_action_profile;

    /** The Compressed forward layer type from the user */
    uint8 compressed_layer_type;

    /** Indicates if the MTU profile allocation should be 2bit */
    uint8 is_profile_2b;
} dnx_rx_trap_mtu_profile_config_t;

/**
 * \brief defination of DNX header ihformation
 */
typedef struct dnx_rx_parse_headers_s
{
    /** Headers stack */
    bcm_pkt_dnx_t dnx_header_stack[BCM_PKT_NOF_DNX_HEADERS];
    /** Number of DNX headers */
    uint8 dnx_header_count;
    /** Length of total headers */
    uint32 header_length;
    /** TRUE: then packet is trapped to CPU and trap-code/qualifier are valid */
    uint8 is_trapped;
    /** PPH.FHEI.Code */
    uint32 cpu_trap_code;
    /** PPH.FHEI.Qualifier */
    uint32 cpu_trap_qualifier;
    /** FTMH.TM-Action-Type */
    bcm_pkt_dnx_ftmh_action_type_t frwrd_type;
    /** FTMH.TC */
    uint32 tc;
    /** FTMH.DP */
    uint32 dp;
} dnx_rx_parse_headers_t;

int dnx_rx_packet_parse(
    int unit,
    bcm_pkt_t * pkt);

/**
 * \brief - Initialize dnx rx module
 * Will init all relevant info, such as Traps etc..
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * DNX data related RX module information
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Rx HW related regs/mems
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rx_init(
    int unit);

/**
 * \brief - Deinitialize dnx rx module
 *  Will deinit all relevant info, such as Traps etc..
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rx_deinit(
    int unit);

 /*
  * Procedures for rx_trap.c {
  */

/**
* \brief  
*  Create and id for given trap type, i.e allocate an id in relevant pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit ID
*   \param [in] flags    -  Can be BCM_RX_TRAP_WITH_ID or 0.
*   \param [in] type     -  bcmRxTrapType.. to create
*   \param [in] trap_id_p  -  Pointer type of int \n
*               \b As \b output - \n
*               Pointer will be filled by the allocated trap id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_create(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id_p);

/**
* \brief  
*  Get the HW Trap id of the supplied trap type.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit ID
*   \param [in] flags    -  This API does not support flags.
*   \param [in] type     -  bcmRxTrapType.. to get
*   \param [out] trap_id_p  -  HW Trap ID associated with the supplied trap_type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_get(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id_p);

/**
* \brief  
*  Get the trap type for the supplied trap id.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit ID
*   \param [in] flags    -  This API does not support flags.
*   \param [in] trap_id  -  trap_id supplied
*   \param [out] bcm_trap_type_p     -  bcmRxTrapType.. of trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_from_id_get(
    int unit,
    int flags,
    int trap_id,
    bcm_rx_trap_t * bcm_trap_type_p);

/**
* \brief
*  Set the trap action profile: trap stregnth and action.
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  trap id to condifugre
*   \param [in] config_p   -  action profile configuration
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
shr_error_e dnx_rx_trap_set(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * config_p);

/**
* \brief  
*  Get the trap action profile: trap stregnth and action.
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  trap id to get its info, action profile
*   \param [in] config_p   -  Pointer type of bcm_rx_trap_config_t \n
*               \b As \b output - \n
*               Pointer will be filled by action profile configuration
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
shr_error_e dnx_rx_trap_get(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * config_p);

/**
* \brief  
*  Destroy previously created trap, i.e. deallocate from relevant trap pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit Id
*   \param [in] trap_id  -  trap id to destroy
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_type_destroy(
    int unit,
    int trap_id);

/**
* \brief  
*  Configure all trap related that is relevant for device initiation
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_init(
    int unit);

/**
* \brief  
*  Set the trap action profile: trap stregnth and action.
* \par DIRECT INPUT: 
*   \param [in] unit     -  unit id
*   \param [in] key_p   -  protocol trap key parameters \n
*               \b As \b input - \n
*               Pointer contains protocol type, protocol arguments and trap profile
*   \param [in] trap_gport    -  gport encoding trap action profile
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
shr_error_e dnx_rx_trap_protocol_set(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p,
    bcm_gport_t trap_gport);

/**
* \brief  
*  Get configuration of previously created protocol trap
* \par DIRECT INPUT: 
*   \param [in] unit     -  unit Id
*   \param [in] key_p   -  protocol trap key parameters \n
*               \b As \b input - \n
*               Pointer contains protocol type, protocol arguments and trap profile
*   \param [out] trap_gport_p    -  trap gport 
*               \b As \b output - \n
*               Encoding trap action profile
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_get(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p,
    bcm_gport_t * trap_gport_p);

/**
* \brief  
*  Destroy previously created protocol trap, i.e. clear relevanat table entry content 
* \par DIRECT INPUT: 
*   \param [in] unit     -  unit Id
*   \param [in] key_p   -  protocol trap key parameters \n
*               \b As \b input - \n
*               Pointer contains protocol type, protocol arguments and trap profile
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_protocol_clear(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p);

/**
* \brief  
*  This function purpose is to set protocol trap profiles
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit Id
*   \param [in] port  - either In-PP-Port or gport for InLIF
*   \param [in] protocol_profiles_p  -  protocol trap profiles struct
*               \b As \b input - \n
*               Pointer contains the trap profiles of each protocol
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
shr_error_e dnx_rx_trap_protocol_profiles_set(
    int unit,
    bcm_gport_t port,
    bcm_rx_trap_protocol_profiles_t * protocol_profiles_p);

/**
* \brief  
*  This function purpose is to get protocol trap profiles configuration
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit Id
*   \param [in] port  - either In-PP-Port or gport for InLIF
*   \param [out] protocol_profiles_p  -  protocol trap profiles struct
*               \b As \b output - \n
*               Pointer contains trap profiles of each protocol
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
shr_error_e dnx_rx_trap_protocol_profiles_get(
    int unit,
    bcm_gport_t port,
    bcm_rx_trap_protocol_profiles_t * protocol_profiles_p);

/**
* \brief
*  This function purpose is to set the trap gport to be connected to the supplied trap type.
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] flags           -  Flags
*   \param [in] trap_type       - BCM Trap type
*   \param [in] gport   - Gport encoded as trap or mirror 
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
shr_error_e dnx_rx_trap_action_profile_set(
    int unit,
    uint32 flags,
    bcm_rx_trap_t trap_type,
    bcm_gport_t gport);

/**
* \brief
*  This function purpose is to get the trap gport that is connected to the supplied trap type.
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] trap_type       - BCM Trap type
*   \param [out] gport_p   - Gport encoded as trap or mirror 
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
shr_error_e dnx_rx_trap_action_profile_get(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t * gport_p);

/**
* \brief
*  This function purpose is to clear the action_profile associated with the supplied trap type.
* \par DIRECT INPUT:
*   \param [in] unit       -  unit Id
*   \param [in] trap_type  - BCM Trap type
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
shr_error_e dnx_rx_trap_action_profile_clear(
    int unit,
    bcm_rx_trap_t trap_type);

/**
* \brief
*  The function creates and sets a programmable trap, based on the input of the user.
*
* \par DIRECT INPUT:
*   \param [in] unit                - Unit Id
*   \param [in] prog_index          - Programmable Trap ID
*   \param [in] prog_config_p       - Programmable trap configuration
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
shr_error_e dnx_rx_trap_prog_set(
    int unit,
    uint8 prog_index,
    bcm_rx_trap_prog_config_t * prog_config_p);

/**
* \brief
*  The function returns the configuration of the programmable trap associated with the supplied ID.
*
* \par DIRECT INPUT:
*   \param [in] unit                - Unit Id
*   \param [in] prog_index          - Programmable Trap ID
*   \param [out] prog_config_p      - Programmable trap configuration
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
shr_error_e dnx_rx_trap_prog_get(
    int unit,
    uint8 prog_index,
    bcm_rx_trap_prog_config_t * prog_config_p);

/**
* \brief
*  This function sets the MTU value and Trap Action-Profile per LIF/RIF/Port
*
* \par DIRECT INPUT:
*   \param [in] unit                  - Unit Id
*   \param [in] mtu_config_p          - MTU Configuration to be set
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
shr_error_e dnx_rx_mtu_set(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p);

/**
* \brief
*  This function gets the MTU value and Trap Action-Profile per LIF/RIF/Port
*
* \par DIRECT INPUT:
*   \param [in] unit                  - Unit Id
*   \param [out] mtu_config_p         - MTU Configuration retrieved
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
shr_error_e dnx_rx_mtu_get(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p);

/**
* \brief
*  The function creates and sets a lif trap, based on the input of the user.
*
* \par DIRECT INPUT:
*   \param [in] unit              - Unit Id
*   \param [in] flags             - flags
*   \param [in] lif_config_p    -  LIF trap configuration
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
shr_error_e dnx_rx_trap_lif_set(
    int unit,
    uint32 flags,
    bcm_rx_trap_lif_config_t * lif_config_p);

/**
* \brief
*  The function retrieves the lif trap configuration, based on the input of the user.
*
* \par DIRECT INPUT:
*   \param [in] unit                    - Unit Id
*   \param [out] lif_config_p    -  LIF trap configuration
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
shr_error_e dnx_rx_trap_lif_get(
    int unit,
    bcm_rx_trap_lif_config_t * lif_config_p);

/*
  * } Procedures for rx_trap.c 
 */

/*
 * } 
 */
#endif/*_RX_API_INCLUDED__*/
