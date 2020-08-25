/*
 * \file bcm_int/dnx/mpls/mpls.h
 * Internal DNX MPLS APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _MPLS_H_INCLUDED__
/*
 * {
 */
#define _MPLS_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

#include <bcm/mpls.h>

/*
 * DEFINES
 * {
 */

/** MPLS GAL Label */
#define MPLS_LABEL_GAL 13
/** MAX value for MPLS reserved label */
#define MPLS_MAX_RESERVED_LABEL 15

/**
 * Maximal allowed MPLS label - 20LSB
 */
#define DNX_MPLS_LABEL_MAX(unit)    (unsigned int)(SAL_UPTO_BIT(dnx_data_mpls.general.nof_bits_mpls_label_get(unit)))

/**
 * Ckecks whether the given label is not larger than the allowed for MPLS label
 */
#define DNX_MPLS_LABEL_IN_RANGE(unit,_label_) (((unsigned int)_label_) <= DNX_MPLS_LABEL_MAX(unit))

#define DNX_VPN_ID_MASK           0xffff
#define DNX_VPN_IS_SET(_vpn_)       ((_vpn_) != 0)
#define DNX_VPN_ID_SET(_vpn_, _id_) ((_vpn_) = ((_id_) & DNX_VPN_ID_MASK))
#define DNX_VPN_ID_GET(_vpn_)       ((_vpn_) & DNX_VPN_ID_MASK)

    /** Current TTL Exp MPLS Label  */
#define TTL_EXP_CURRENT_MPLS_LABEL      DBAL_ENUM_FVAL_TTL_EXP_LABEL_INDEX_CURRENT_MPLS_LABEL
    /** Next TTL Exp MPLS Label  */
#define TTL_EXP_NEXT_MPLS_LABEL         DBAL_ENUM_FVAL_TTL_EXP_LABEL_INDEX_NEXT_MPLS_LABEL
    /** No MPLS Labels To terminate  */
#define NO_MPLS_LABELS_TO_TERMINATE     DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_NO_LABELS
    /** One MPLS Label To terminate  */
#define ONE_MPLS_LABEL_TO_TERMINATE     DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_ONE_LABEL
    /** Two MPLS Labels To terminate  */
#define TWO_MPLS_LABELS_TO_TERMINATE    DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_TWO_LABELS
    /** Three MPLS Labels To terminate  */
#define THREE_MPLS_LABEL_TO_TERMINATE   DBAL_ENUM_FVAL_LABELS_TO_TERMINATE_THREE_LABELS

/*
 * }
 */

/*
 * Types
 * {
 */

/**
 * \brief
 * MPLS labels match types
 * These types are supported for lookup in ISEM
 */
typedef enum
{
    DNX_MPLS_LABEL_MATCH_TYPE_INVALID = -1,

    DNX_MPLS_LABEL_MATCH_TYPE_SINGLE_LABEL = 0,
    DNX_MPLS_LABEL_MATCH_TYPE_TWO_LABELS = 1,
    DNX_MPLS_LABEL_MATCH_TYPE_INTF_PLUS_LABEL = 2,

    /*
     * Must be last
     */
    DNX_MPLS_LABEL_MATCH_TYPE_NOF
} dnx_mpls_match_type_e;

/*
 * }
 * Types
 */

/**
 * \brief - Initialize MPLS module. \n
 * Set default value in termination profile label table.
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_mpls_module_init(
    int unit);

/**
 * \brief - Deinitialize MPLS module. \n
 * Doing nothing at the moment. \n
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_mpls_module_deinit(
    int unit);

/**
 * \brief - Fill eei info according to field_id 
 *          DBAL_FIELD_EEI_MPLS_POP_COMMAND EEI structure:
 *  
 *          Identifier[23:20] - pop command (8),
 *                              push command (0),
 *                              encapsulation pointer (3)
 *          Reserved[19:11]
 *          Remark-profile[10:8] - retreived from qos_map_id
 *          Model-is-Pipe-MSB[7] - always 1 for php
 *          Upper-Layer-Type[4] - irrelevant, not used
 *          Model-is-Pipe[5] - always 1 for php
 *          CW[4] - irrelevant, not used
 *          Upper-Layer-Type[3:0] - irrelevant, not used
 * 
 * \param [in] unit - Relevant unit
 * \param [in] qos_map_id - Quality Of Service
 * \param [in] qos_model - qos model is pipe or uniform
 * \param [in] ttl_model - ttl model is pipe or uniform
 * \param [in] upper_layer_type - upper layer protocol
 * \param [out] full_field_val - Returns the eei buffer
 *   
 * \return
 *   Error indication according to shr_error_e enum 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_php_information_fill(
    int unit,
    int qos_map_id,
    dbal_enum_value_field_model_type_e qos_model,
    dbal_enum_value_field_model_type_e ttl_model,
    dbal_enum_value_field_jr_fwd_code_e upper_layer_type,
    uint32 *full_field_val);

/**
 * \brief - Get eei info according to field_id
 * Only qos_map_id is relevant as other fields are fixed
 *
 * \param [in] unit - Relevant unit
 * \param [in] full_field_val - Returns the eei buffer
 * \param [out] qos_map_id - Quality Of Service
 * \param [out] qos_model - qos model is pipe or uniform
 * \param [out] ttl_model - ttl model is pipe or uniform 
 * \param [out] upper_layer_type - upper layer protocol
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_php_information_retrieve(
    int unit,
    uint32 full_field_val,
    int *qos_map_id,
    dbal_enum_value_field_model_type_e * qos_model,
    dbal_enum_value_field_model_type_e * ttl_model,
    dbal_enum_value_field_jr_fwd_code_e * upper_layer_type);

/**
 * \brief - Fill eei info according to field_id 
 *          DBAL_FIELD_EEI_ENCAPSULATION_POINTER EEI structure:
 *  
 *          Identifier[23:20] - pop command  (8),
 *                              swap command (9)
 *                              push command (0-7),
 *                              encapsulation pointer (3,0b11xx)
 *          enc_pointer [15:0] outlif pointer[15:0] 
 *          Host-index[19:16] - host idx
 *          enc-pointer-MSB[21:20] - out lif pointer[17:16]
 * 
 * \param [in] unit - Relevant unit
 * \param [in] out_lif - out lif
 * \param [in] host_idx - host index
 * \param [out] full_field_val - Returns the eei buffer
 *   
 * \return
 *   Error indication according to shr_error_e enum 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_enc_pointer_information_fill(
    int unit,
    bcm_if_t out_lif,
    uint8 host_idx,
    uint32 *full_field_val);

/**
 * \brief - Get eei info according to field_id
 *
 * \param [in] unit - Relevant unit
 * \param [in] full_field_val - Returns the eei buffer
 * \param [out] out_lif - Out LIF
 * \param [out] host_idx - Host Index
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_enc_pointer_information_retrieve(
    int unit,
    uint32 full_field_val,
    bcm_if_t * out_lif,
    uint8 *host_idx);

/**
 * \brief - Fill eei info according to field_id 
 *          DBAL_FIELD_EEI_MPLS_PUSH_COMMAND EEI structure:
 *  
 *          Identifier[23:20] - pop command  (8),
 *                              swap command (9)
 *                              push command (0-7),
 *                              encapsulation pointer (3,0b11xx)
 *          mpls_label [19:0] label for push 
 *          push_cmd [22:20] - push command
 * 
 * \param [in] unit - Relevant unit
 * \param [in] push_cmd - push command
 * \param [in] mpls_label - mpls label
 * \param [out] full_field_val - Returns the eei buffer
 *   
 * \return
 *   Error indication according to shr_error_e enum 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_push_information_fill(
    int unit,
    uint32 mpls_label,
    int push_cmd,
    uint32 *full_field_val);

/**
 * \brief - Get eei info according to field_id
 *
 * \param [in] unit - Relevant unit
 * \param [in] full_field_val - Returns the eei buffer
 * \param [out] push_cmd - push command
 * \param [out] mpls_label - mpls label
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_eei_push_information_retrieve(
    int unit,
    uint32 full_field_val,
    uint32 *mpls_label,
    int *push_cmd);

/*
 * }
 */

#endif/*_MPLS_H_INCLUDED__*/
