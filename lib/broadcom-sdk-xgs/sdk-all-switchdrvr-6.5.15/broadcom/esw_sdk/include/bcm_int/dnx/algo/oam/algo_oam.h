/**
 * \file algo_oam.h Internal DNX L3 Managment APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_OAM_H_INCLUDED
/*
 * { 
 */
#define ALGO_OAM_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm/types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

 /**
 * Resource name defines for algo oam 
 * \see
 * dnx_oam_init_templates 
 * {
 */
#define DNX_ALGO_OAM_GROUP_ID     "OAM_GROUP_ID"
#define DNX_ALGO_OAM_GROUP_ICC_PROFILE  "OAM_GROUP_ICC_PROFILE"

#define DNX_ALGO_OAM_PROFILE_ID_ING     "OAM_PROFILE_ID_ING"
#define DNX_ALGO_OAM_PROFILE_ID_EG      "OAM_PROFILE_ID_EG"
#define DNX_ALGO_OAM_PROFILE_ID_ING_ACC "OAM_PROFILE_ID_ING_ACC"
#define DNX_ALGO_OAM_PROFILE_ID_EG_ACC  "OAM_PROFILE_ID_EG_ACC"

/*
 *
 */
#define SW_STATE_ALGO_OAM_PROFILE_ID_ING     algo_oam_db.oam_profile_id_ing
#define SW_STATE_ALGO_OAM_PROFILE_ID_EG      algo_oam_db.oam_profile_id_eg
#define SW_STATE_ALGO_OAM_PROFILE_ID_ING_ACC algo_oam_db.oam_profile_id_ing_acc
#define SW_STATE_ALGO_OAM_PROFILE_ID_EG_ACC  algo_oam_db.oam_profile_id_eg_acc

#define DNX_ALGO_OAM_RMEP_ID_BELOW_THRESHOLD "OAM_RMEP_ID_BELOW_THRESHOLD"
#define DNX_ALGO_OAM_RMEP_ID_ABOVE_THRESHOLD "OAM_RMEP_ID_ABOVE_THRESHOLD"
#define DNX_OAM_TEMPLATE_ITMH_PRIORITY            "OAM ITMH PRIORITY PROFILE"
#define DNX_OAM_TEMPLATE_MPLS_PWE_EXP_TTL_PROFILE "OAM MPLS/PWE EXP+TTL PROFILE"
#define DNX_OAM_TEMPLATE_MEP_PROFILE "OAM MEP PROFILE"

#define DNX_ALGO_OAMP_PUNT_EVENT_PROFILE_TEMPLATE "OAMP_PUNT_EVENT_PROF_TEMPLATE"

#define DNX_ALGO_OAM_OAMP_SA_MAC_MSB_PROFILE_TEMPLATE "OAM_SA_MAC_MSB_PROFILE_TEMPLATE"
#define DNX_ALGO_OAM_OAMP_DA_MAC_MSB_PROFILE_TEMPLATE "OAM_DA_MAC_MSB_PROFILE_TEMPLATE"
#define DNX_ALGO_OAM_OAMP_DA_MAC_LSB_PROFILE_TEMPLATE "OAM_DA_MAC_LSB_PROFILE_TEMPLATE"
#define DNX_ALGO_OAM_OAMP_TPID_PROFILE_TEMPLATE "ETH OAM TPID PROFILE"

#define DNX_ALGO_PUNT_TABLE_START_ID 0
#define DNX_ALGO_PUNT_TABLE_COUNT 16

/** Flags used to indicate if oam oamp profile should be updated */
#define DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG             (0x1)
#define DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG                (0x2)
#define DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG                (0x4)
#define DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG                (0x8)
#define DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG                   (0x10)
#define DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG  (0x20)
#define DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG               (0x40)

/* oam_group_icc_profile_data_t  used to store OAM group ICC profiles as templates*/
typedef uint8 dnx_oam_group_icc_profile_data_t[6];

/**
 * \brief - Printing callback for the OAM group ICC templates, 
 *        which are used to track OAM group ICC data.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The OAM 
 *        group ICC profile structure dnx_oam_group_icc_profile_data_t is
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

void dnx_oam_group_icc_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/** 
 *  Structure used to store profiles that contain "traffic
 *  class/drop precedence" profiles as template.  These values
 *  are used by the OAMP to construct the ITMH header of OAM/BFD
 *  packets for transmission.
 */
typedef struct
{
    uint8 tc;
    uint8 dp;
} dnx_oam_itmh_priority_t;

/** 
 *  Structure used to store profiles that contain "MPLS/PWE
 *  push profiles" as template.  These values
 *  are used by the OAMP to construct the MPLS/PWE encapsulated
 *  OAM/BFD packets for transmission.
 */
typedef struct
{
    uint8 ttl;
    uint8 exp;
} dnx_oam_ttl_exp_profile_t;

/** 
 *  Structure used to store profiles that contain "OAMP MEP profile"
 *  as template.  These values are used to update CCM/LM/DM packets
 *  constructured by OAMP for transmission.
 */
typedef struct
{
    /** Dual ended LM and SLM indications
     */
    uint8 piggy_back_lm;
    uint8 slm_lm;

    /** For Flexible 48 byte MAID - 
     *  disable verification of message
     *  by comparing MAID to field in MEP-DB entry
     */
    uint8 maid_check_dis;

    /** For BFD over IPv6 -
     *  disable verification of message
     *  by comparing IPv4 packet dip to IPv4 MEP sip in MEP-DB entry
     */
    uint8 src_ip_check_dis;

    /** For BFD discriminator with MEP type -
     * disable verification of your-discriminator, since
     * for endpoints with MSB=1, this verification would fail
     */
    uint8 your_disc_check_dis;

    /** LM report mode:enable/disable
     */
    uint8 report_mode_lm;

    /** DM report mode:enable/disable
     */
    uint8 report_mode_dm;

    /** RDI generation from scanner and/or RX process
     */
    uint8 rdi_gen_method;
    /** LM/DM rates at which OAMP need to TX
     */
    int dmm_rate;
    int lmm_rate;
    /** Only opcode 0, 1 used at this stage (0-7 available)
     */
    int opcode_0_rate;
    int opcode_1_rate;
    /** LM/DM offsets used to update OAM-TS for stamping purpose.
     */
    uint8 lmm_offset;
    uint8 lmr_offset;
    uint8 dmm_offset;
    uint8 dmr_offset;
    /** Phase profile. Strictly speaking not part of MEP profile but uses the same pointer
     */
    uint32 ccm_count;
    uint32 dmm_count;
    uint32 lmm_count;
    uint32 opcode_0_count;
    uint32 opcode_1_count;
    /** DM measurement type and 1DM time stamp encodings if type is 1DM
     */
    uint8 dm_measurement_type;
    /** Opcodes to send from OAMP
     */
    uint8 opcode_bit_map;

    /*
     * LMM/DMM DA OUI(MSB 24 bits) profile
     */
    uint8 oamp_mac_da_oui_prof;

    /*
     * OAM Statistics, TX/RX counting
     */
    uint8 mep_id_shift;
    uint8 opcode_mask;
    uint8 opcode_tx_statistics_enable;
    uint8 opcode_rx_statistics_enable;

} dnx_oam_mep_profile_t;

/**
 *  This struct is used to store new allocated profiles data
 *  required for OAM endpoint during create/modify in order to be set
 *  after all successfully allocations
 */
typedef struct
{
    /** Flags to indicate:
     *   1. new profiles that should be set.
     *   2. profiles that should be updated.
     *   3. profiles that should be zeroed.
     *   See DNX_OAM_OAMP_PROFILE_XXX
     *   */
    uint32 flags;

    uint8 itmh_profile;
    dnx_oam_itmh_priority_t itmh_priority;

    /**
     * Data for writing system port profile
     * PP port and port core for CCM
     * ethernet up-MEP; PP port profile
     * for all other enddpoints
     */
    uint16 pp_port_profile;
    uint32 system_port;

    uint8 outer_tpid_index;
    uint16 outer_tpid;

    uint8 inner_tpid_index;
    uint16 inner_tpid;

    uint8 sa_mac_msb_profile;
    bcm_mac_t sa_mac_msb;

    uint8 mpls_pwe_exp_ttl_profile;
    dnx_oam_ttl_exp_profile_t ttl_exp;

    uint8 mep_profile_index;
    dnx_oam_mep_profile_t mep_profile;

} oam_oamp_profile_hw_data_t;

/**
 * \brief - Printing callback for the MAC SA MSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_sa_mac_msb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the MAC DA MSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_da_mac_msb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the MAC DA LSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_da_mac_lsb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - initialize templates for all oam profile types. 
 *        Current oam  Templates:\n
 *  OAM group id - group ID for OAM groups\n
 *  OAM TC/DP - profile that contains TC and DP values for
 *  ITMH\n
 *  MPLS/PWE push profile - profile that contains TTL and EXP
 *  values
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
shr_error_e dnx_algo_oam_init(
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
shr_error_e dnx_algo_oam_deinit(
    int unit);

/**
 * \brief - Printing callback for the ETH OAM tpid profile template
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data(tpid).
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */

void dnx_oam_oamp_tpid_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
