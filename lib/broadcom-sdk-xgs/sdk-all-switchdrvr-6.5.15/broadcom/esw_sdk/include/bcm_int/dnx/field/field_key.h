/**
 * \file bcm_int/dnx/field/field_key.h
 *
 *
 * Field Processor definitions for Key for DNX
 *
 * Purpose:
 *     'Field Processor' (FP) definitions for KEY functions for other field modules for Packet Processor.
 *     Devices starting at DNX
 * Note:
 *      This file is added on top of the existing files in 'legacy' (ported
 *      from DPP).
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FIELD_FIELD_KEY_H_INCLUDED
/* { */
#define FIELD_FIELD_KEY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Include files
 * {
 */
#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>

/*
 * }
 */
/*
 * Defines
 * {
 */
/**
 * If 'DNX_FIELD_KEY_USE_HALF_KEY_ALLOCATION_ONLY' is set to a non-zero value then
 * allocation on keys (specifically for Direct Extraction) is done in 'quanta's of
 * 'half key' (80 bits). Otherwise, it is done in resolution of one bit and the
 * number of allocated bits is exactly the size of the key template while the location
 * is dynamic. (This is referred to as 'bit-range' allocation.)
 * \see dnx_algo_field_key_id_allocate
 * \see dnx_algo_field_key_id_bit_range_allocate
 */
#define DNX_FIELD_KEY_USE_HALF_KEY_ALLOCATION_ONLY  0

/* Starting offsets within keys for key templates.*/
/** Default starting offset in key for a lookup fg in key is 0 */
#define DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET 0
/** Direct extraction has a starting offset of 0, that changes after attach.*/
#define DNX_FIELD_KEY_DE_STARTING_OFFSET 0
/** Hashing has a strating offset of 0*/
#define DNX_FIELD_KEY_HASH_STARTING_OFFSET 0
/** Compare allocates the msbs of the key, therefore the key-size is subtracted from single key size */
#define DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(_unit) \
       (dnx_data_field.tcam.key_size_single_get(_unit) - \
        dnx_data_field.stage.stage_info_get(_unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size)
/** State Table allocates the msbs of the key, therefore the key-size is subtracted from single key size */
#define DNX_FIELD_KEY_STATE_TABLE_STARTING_OFFSET(_unit) \
    (dnx_data_field.tcam.key_size_single_get(_unit) - dnx_data_field.state_table.key_size_get(_unit))
/*
 * }
 */
/*
 * Typedefs
 * {
 */
/**
 * Structure containing the information about a qualifier including the attach information.
 */
typedef struct
{
    /** The size in bits of the qualifier*/
    uint32 size;
    /**
     * Offset from base - type of base depends on FCC type
     */
    int offset;
    /**
     * Layer ID, either absolute or relative to forwarding, depending in type
     */
    int index;
    /**
     * ffc type
     */
    dnx_field_ffc_type_e ffc_type;
    /**
     * FFC ranges that can be used by this qualifier
     */
    uint8 ranges;
    /**
     * The input type given in attach_info.
     */
    dnx_field_input_type_e input_type;
    /**
     * For iPMF2 only, whether we use the native metadata (Metadata 2).
     * If FALSE, The other fields in dnx_field_key_attached_qual_info_t refer to the iPMF1 FFC that sends the 
     * data to the initial key.
     */
    int native_pbus;
} dnx_field_key_attached_qual_info_t;

/**
* This structure hold the input parameters for dnx_field_group_create function
* This is not part of field SW state
*/
typedef struct
{
    /** Field stage, for which stage the Field Group was created*/
    dnx_field_stage_e field_stage;
    /**
     *  Database type: The database may be:TCAM, Direct Table,
     *  or Direct Extraction.
     */
    dnx_field_group_type_e fg_type;

    /** Mapping of the qualifier inside the key*/
    dnx_field_key_template_t key_template;

    /** Specify which key size needs to be allocated by key module*/
    dnx_field_key_length_type_e key_length;
    /**
     * Access parameter for KBR (See, for example, DBAL table KLEAP_IPMF1_KBR_INFO (IPPC_PMF_KBR_PASS_1/))
     * Used from TCAM/MDB/EXEM - used to know how to perform lookup in the databases
     */
    dnx_field_app_db_id_t app_db_id;
    /**
     *This struct hold additional info for FFC config,
     * Used only for header type Qualifier to indicate in which layer number should be taken from
     * the number qualifier information is aligned to what held in key_template
     */
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    /**
     * Compare attributes - in case the context has compare mode - which key/tcam result to compare
     * CMP1 : this FG will be compared against CMP1, internally SDK will try to allocate key B
     * CMP2 : this FG will be compared against CMP2, internally SDK will try to allocate key D*
     */
    dnx_field_group_compare_id_e compare_id;

} dnx_field_key_attach_info_in_t;

/**
* This structure hold the instruction parameters for the ffc hw configuration
* This is not part of field SW state
*/
typedef struct
{
    /** 
      * Specify which ffc type should be configured (HEADER/META/LAYER-RECORD/etc)
      */
    dbal_fields_e ffc_type;
    /** 
      * Specifies the offset of the qualifier data in the pbus that ffc should take the content from.
      */
    int offset;
    /** 
      * Specifies the header number that the qualifier data would be taken from 
      */
    int field_index;
    /** 
      * Specifies the offset in the key that the qualifier data will be written to
      */
    uint32 key_offset;
    /** 
      * Specifies the size of the data to be extracted from header and written into the key
      */
    uint32 size;
    /** 
      * Specifies the 32'b instruction, as it appears in the hw
      */
    uint32 full_instruction;
} dnx_field_key_ffc_instruction_info_t;

/**
* This structure hold the information required for hw configuration of the ffc
* It holds the ffc id allocated for this qualifier and the instruction parameters for the ffc hw configuration 
* This is not part of field SW state
*/
typedef struct
{
    /** 
      * Specifies the ffc id allocated for the given qualifier (or part of qualifier)
      */
    uint32 ffc_id;
    /**
    * The key on which the FFC is located.
    */
    dbal_enum_value_field_field_key_e key_id;
    /** 
      * Specifies the ffc instruction for the given ffc_id
      */
    dnx_field_key_ffc_instruction_info_t ffc_instruction;
} dnx_field_key_ffc_hw_info_t;

/**
* This structure hold the initial ffc information, including the ffc_id in IPMF1 and the hw instruction
* for this FFC
* This is not part of field SW state
*/
typedef struct
{
    /** 
      * This structure hold the information required for hw configuration of the ffc
      */
    dnx_field_key_ffc_hw_info_t ffc;
    /** 
      * Specifies the offset in the initial key-id for the allocated ffc
      */
    uint32 key_dest_offset;
} dnx_field_key_initial_ffc_info_t;

/**
* This structure holds the complete info for the ffc. It includes the ffc info, and initial ffc information
* in case it is relevant including the ffc_id and the instruction for all stages.
* This is not part of field SW state
*/
typedef struct
{
    /**
    * This structure hold the information required for hw configuration of the ffc
    * It holds the ffc id allocated for this qualifier and the hw instruction parameters for the ffc configuration
    */
    dnx_field_key_ffc_hw_info_t ffc;
    /**
    * This structure hold the initial ffc information, including the ffc_id in IPMF1 and the hw instruction
    * for this FFC
    */
    dnx_field_key_initial_ffc_info_t ffc_initial;
} dnx_field_key_ffc_and_initial_ffc_info_t;

/**
* This structure holds the complete info required for the ffc configuration per qualifier.
*/
typedef struct
{
    /**
    * The qualifier type that the information belongs to
    */
    dnx_field_qual_t qual_type;
    /**
    * This structure holds the complete info for the ffc. It includes the ffc info, and initial ffc information
    * in case it is relevant including the ffc_id and the instruction for all stages.
    */
    dnx_field_key_ffc_and_initial_ffc_info_t ffc_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL];
} dnx_field_key_qualifier_ffc_info_t;

/**
  * This structure holds the info respectfully to qualifier per field group.
*/
typedef struct
{
    /**
    * This structure holds the complete info required for the ffc configuration per qualifier.
    */
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
} dnx_field_key_group_key_info_t;

/**
* This structure holds the complete info required for the ffc configuration per field group.
*/
typedef struct
{
    /**
    * This structure holds the info respectfully to key_id per field group. For 80/160 bit key,
    * only the first element will be initialized. For the 320'b key, both elements will be initialized.
    */
    dnx_field_key_group_key_info_t key;
} dnx_field_key_group_ffc_info_t;
/*
 * }
 */

/*
 * Prototypes
 * {
 */
/**
* \brief
*  Get the FFC id's bitmap, that were set in HW to build key
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id of which FFC needs to be read
* \param [out] ffc_id   - Array FFC ID's that are configured in KBR HW for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_kbr_ffc_array_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC]);
/**
 * \brief
 *  Init the values of dnx_field_key_template_t type
 * \param [in] unit   - Device id
 * \param [in] key_template_p  - Key to init its value
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_template_t_init(
    int unit,
    dnx_field_key_template_t * key_template_p);

/**
* \brief
*  Init structure dnx_field_key_attach_info_in_t
* \param [in] unit           - Device Id
* \param [in] key_in_info_p  - pointer to structure dnx_field_key_attach_info_in_t
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_attach_info_in_t_init(
    int unit,
    dnx_field_key_attach_info_in_t * key_in_info_p);

/**
* \brief
*  Init Structure dnx_field_qual_attach_info_t to invalid params
* \param [in] unit           - Device Id
* \param [in] qual_info_p  - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_info_p);

/**
* \brief
*  Init Structure dnx_field_key_id_t to invalid params
* \param [in] unit           - Device Id
* \param [in] key_p          - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_id_t_init(
    int unit,
    dnx_field_key_id_t *key_p);

/**
* \brief
*  Init Structure dnx_field_bit_range_t to invalid params
* \param [in] unit           - Device Id
* \param [in] bit_range_p    - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_bit_range_t_init(
    int unit,
    dnx_field_bit_range_t * bit_range_p);

/**
 * \brief
 *  Detach all context ID key information, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly).
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [in,out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 *   Sub param of the key_id_p - bit_range     -
 *   \b As \b input - \n
 *     If bit_range is invalid then this parameter is ignored and allocation
 *     is done per 'half key'.
 *     If bit_range is valid then the boolean 'bit_range_p->bit_range_was_aligned'
 *     indicates whether to align the allocated 'key template' and the element
 *     'bit_range_p->bit_range_aligned_offset' indicates on which bit (in 'key template'
 *     to align).
 *   \b As \b output - \n
 *     This procedure loads bit_range with information regarding bit-range allocated on specified key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Detach PMF context per KEY for the special contexts (aka compare, hash, state table)
 * \param [in] unit            - Device ID
 * \param [in] stage           - Field stage
 * \param [in] context_id      - Context ID to perform the key detach for
 * \param [in] key_id_p        - Structure that holds the array of Key Id to deallocate
 * \param [in] key_template_p  - Key Template
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_context_feature_key_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p);

/**
 * \brief
 *  Attach PMF context per KEY for the special contexts (aka compare, hash, state table)
 * \param [in] unit               - Device ID
 * \param [in] context_id         - Context ID ID to perform the key attach for
 * \param [in] key_in_info_p      - First key in info look at dnx_field_key_attach_info_in_t
 * \param [in] key_id_p          - Pointer type of dnx_field_key_id_t, holds an array of allocated keys
                                    needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_context_feature_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Detach PMF context per KEY for compare
 * \param [in] unit                    - Device ID
 * \param [in] context_id              - Context ID ID to perform the key attach for
 * \param [in] compare_key_info_p      - Key Id and key template
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_compare_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * compare_key_info_p);

/**
 * \brief
 *  This function will build key for context ID based on qual
 * \param [in] unit             - Device ID
 * \param [in] field_stage      - Field Stage (PMF1/2/3...)
 * \param [in] qual_types_p     - Key qualifiers
 * \param [in] starting_offset  - Offset to start from in key
 * \param [out] key_template_p  - Hold the constructed key
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * qual_types_p,
    uint32 starting_offset,
    dnx_field_key_template_t * key_template_p);

/**
 * \brief
 * Get information about an initial FFC by given initial key and context.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context Id to get its info
 * \param [in] initial_key_id   - Initial key to get its info
 * \param [in] ffc_id           - FFC ID to get its info
 * \param [out] ipmf1_ffc_initial_p  - Pointer to dnx_field_key_initial_ffc_info_t to hold initial ffc info
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_single_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e initial_key_id,
    uint32 ffc_id,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p);

/**
 * \brief
 * Get information on the key that is attached to specified context
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context Id to get its info
 * \param [in] key_id_p         - structure that holds an array of key ids for which the KBR will be updated
                                - bit_range - is a subparam of key_id_p that holds all information regarding
                                  the bit-range allocated on key (specified in 'key_id', above) for this Context ID.
 * \param [in,out] key_out_info_p  - Pointer to dnx_field_key_attach_info_in_t to hold attached key info
 * \param [out] group_ffc_info_p  - Pointer to dnx_field_key_group_ffc_info_t to hold full ffc info
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_attach_info_in_t * key_out_info_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p);

/*
 * Procedures related to SW STATE for 'key'
 * {
 */
/**
 * \brief
 *   Initialize SW STATE for the whole 'KEY' module.
 * \param [in] unit     - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored. Root is 'dnx_field_keys_per_stage_allocation_sw'
 * \see
 *   dnx_field_keys_per_stage_allocation_t
 */
shr_error_e dnx_field_key_sw_state_init(
    int unit);
/*
 * }
 */
/*
 * Utility procedures to be used by, say, diag_dnx_field and ctest_dnx_field
 * {
 */
/**
 * \brief
 *  This function will indicate key allocation state: Is there any occupied key
 *  on specified pair - stage and context
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for searching key occupation state
 * \param [in] context_id            -
 *   dnx_field_context_t. Context id (program) to use for searching key occupation state
 * \param [in] group_type            -
 *   dnx_field_group_type_e. Group type to use for searching key occupation state
 *   If set to DNX_FIELD_GROUP_TYPE_INVALID then this input is ignored (i.e., group
 *   type is 'do not care').
 * \param [in] bit_range_only            -
 *   int. If this input is 'zero' then ignore it. If non-zero then search only
 *   'field group types' which support the 'bit-range' feature. If feature is
 *   not soppurted, this procedure will flag 'not occupied'
 * \param [out] none_occupied_p            -
 *   int pointer. This procedure loads pointed memory by a non-zero value
 *   if no key is occupied for this stage/context pair.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is based on keys occupation as stored on SW state.
 * \see
 *   * None
 */
shr_error_e dnx_field_key_is_any_key_occupied(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e group_type,
    int bit_range_only,
    int *none_occupied_p);
/*
 * }
 */

shr_error_e dnx_field_key_compare_mode_single_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_single_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_double_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_compare_mode_double_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_hash_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id);

shr_error_e dnx_field_key_hash_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p);

shr_error_e dnx_field_key_state_table_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id);

shr_error_e dnx_field_key_state_table_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p);

shr_error_e dnx_field_key_qual_attach_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
 * \brief
 *  Configure the KBP opcode on the required contexts.
 * \param [in] unit        - Device Id
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] context_id  - Context ID.
 * \param [in] kbp_info_p  - structure that holds KBP info, including the array of segments to configure.
 *                           The information given per segment is the fg_idx and pointers(indexes) into the fg_info.
 *                           We need those pointers in order to extract the attach information about a given qualifier.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_ifwd2_acl_ffc_update_for_new_context
 */
shr_error_e dnx_field_key_kbp_set(
    int unit,
    int opcode_id,
    dnx_field_context_t context_id,
    dnx_field_group_kbp_info_t * kbp_master_key_info_p);

/**
 * \brief
 *  This function manages the available FFCs for a newly created ACL context by
 *  masking out the ffc's that are used by original FWD context.
 *
 * \param [in] unit           - Device Id
 * \param [in] fwd_context_id - The FWD context that comes before the ACL context.
 * \param [in] acl_context_id - The newly created ACL cntext
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_ifwd2_acl_ffc_init
 */
shr_error_e dnx_field_key_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id);

/**
 * \brief
 *  This function manages the available for ACL context ffc by
 *  masking out the ffc's that are used by FWD context.
 *  The function goes over all ifwd2 contexts and checks which ffc's were marked as used by
 *  FWD context. Those ffc's are set as 'used' in the ffc resource manager, thus will never
 *  be allocated for the ACL context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_ifwd2_acl_ffc_init(
    int unit);

/**
 * \brief
 *  This function verifies that ACL Key resources were not used by other applications (such as Pemla).
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_ifwd2_acl_key_verify(
    int unit);

/* } */
/* } */
#endif
