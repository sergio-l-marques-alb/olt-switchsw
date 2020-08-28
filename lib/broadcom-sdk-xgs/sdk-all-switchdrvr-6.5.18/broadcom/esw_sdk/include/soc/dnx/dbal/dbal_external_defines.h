/** \file dbal_external_defines.h
 * dbal defines.
 * \n
 */
/*
 * $Id: $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef DBAL_EXTERNAL_DEFINES_H_INCLUDED
/*
 * {
 */
#define DBAL_EXTERNAL_DEFINES_H_INCLUDED

/*
 * }
 */

#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_system.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_hw_entity_groups.h>
#include <shared/shrextend/shrextend_debug.h>

/** \brief max number of dbal handles allowed to use in function to perform table action with.*/
#define DBAL_FUNC_NOF_ENTRY_HANDLES                     10

/** \brief used for fields instances APIs when there is no multiple instances for the field in a specific table */
#  define INST_SINGLE                                   (-1)

/** \brief used for fields instances APIs when all the instances of the field in current table are relevant */
#  define INST_ALL                                      (-2)

/** \brief used for range APIs to indicate that all range is required dbal_entry_key_field32_range_set() */
#  define DBAL_RANGE_ALL                                (-1)

/** \brief DBAL printable string for field max size  */
#  define DBAL_MAX_PRINTABLE_BUFFER_SIZE                        1024

/** \brief the size of field with type array in bits  */
#  define DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS                     512

/** \brief the size of field with type array in bytes  */
#  define DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES                    (BITS2BYTES(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

/** \brief the size of field with type array in words  */
#  define DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS                    (BITS2WORDS(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

/** \brief The nof parameters per field types */
/** Note: Incase DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS changed must update maxOccurs in dbal_types_definition.xsd to the same value */
#  define DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS                      43

/** \brief The nof result fields per result type */
/** Note: Incase DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE changed must update maxOccurs in dbal_types_definition.xsd to the same value */
#  define DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE      256

/** \brief Short names max length in DBAL - Limits the Fields/Tables/Groups names */
#define DBAL_MAX_STRING_LENGTH                                  72

/** \brief Extra-Long max length in DBAL - Formula and HW names and DNX_DATA */
#define DBAL_MAX_EXTRA_LONG_STRING_LENGTH                       400

/** \brief Long names max length in DBAL - Any concatenation of two names  */
#define DBAL_MAX_LONG_STRING_LENGTH                             (2*DBAL_MAX_EXTRA_LONG_STRING_LENGTH + DBAL_MAX_STRING_LENGTH)

/**
 *  \brief this enum represents all the predefine values that can be used for a field.
 *   predefine values are defined in the DBAL input. the user can use them instead of setting hardcoded values.
 */
typedef enum
{
    /** The Minimum value of a field, if not defined otherwise the value will be 0   */
    DBAL_PREDEF_VAL_MIN_VALUE,

    /** The Maximum value of a field, if not defined otherwise the value will be according to the field size. */
    DBAL_PREDEF_VAL_MAX_VALUE,

    /** The default value of a field, if not defined otherwise the value will be 0. the default value can be determind also
     *  according to the HW default value of the field */
    DBAL_PREDEF_VAL_DEFAULT_VALUE,

    /** set as Zero */
    DBAL_PREDEF_VAL_RESET_VALUE,

    DBAL_NOF_PREDEFINE_VALUES
} dbal_field_predefine_value_type_e;

/**
 * \brief Represents all the permission allowed for field.
 * Trigger permission means that a field can be written and
 * read, but we can't expect the field value from the read
 * action to be equal to the write action.
 */
typedef enum
{
    DBAL_PERMISSION_ALL,
    DBAL_PERMISSION_READONLY,
    DBAL_PERMISSION_WRITEONLY,
    DBAL_PERMISSION_TRIGGER,
    DBAL_NOF_PERMISSIONS,
} dbal_field_permission_e;

/**
 *  \brief enum that represents all the commit types available in: \n dbal_entry_get() dbal_entry_commit()
 * dbal_entry_delete() \n multiple flags can be used by "or" operation \n
 */
typedef enum
{
    /** saves the handle after committing for future use.
     *  return the handle implicitly when using this option handle
     *  should be initiated to DBAL_SW_NOF_ENTRY_HANDLES
     *  One and only one of these flags must appear when committing an entry:
     *  DBAL_COMMIT, DBAL_COMMIT_UPDATE_ENTERY and DBAL_COMMIT_FORCE_ENTRY. see also dbal_non_direct_commit_mode_set to
     *  optimze performance of non-direct tables */
    DBAL_COMMIT = SAL_BIT(1),

    /**
     *  update existing entry, applicable only for non-direct tables still doing read modify write.
     *  One and only one of these flags must appear when committing an entry:
     *  DBAL_COMMIT, DBAL_COMMIT_UPDATE_ENTERY and DBAL_COMMIT_FORCE_ENTRY.*/
    DBAL_COMMIT_UPDATE = SAL_BIT(2),

    /** For non-direct tables only
     *  In case that the entry does not already exists - behavior
     *  is the same as DBAL_COMMIT.
     *  Otherwise - behavior is the same as
     *  DBAL_COMMIT_UPDATE_ENTERY.
     *  One and only one of these flags must appear when committing an entry:
     *  DBAL_COMMIT, DBAL_COMMIT_UPDATE_ENTERY and DBAL_COMMIT_FORCE_ENTRY.*/
    DBAL_COMMIT_FORCE = SAL_BIT(3),

    /** saves the handle after getting all the fields. after calling this API user can retrieve the field values by using the
     *  APIs dbal_entry_handle_value_XXX */
    DBAL_GET_ALL_FIELDS = SAL_BIT(4),

    /**
     *  NOT performing read before write - only supported during init otherwise it will not take effect */
    DBAL_COMMIT_OVERRUN = SAL_BIT(5),

    /** For TCAM tables only
     *  In case that not all result fields were set - fill these
     *  fields with default values */
    DBAL_COMMIT_OVERRIDE_DEFAULT = SAL_BIT(6),

    /** will not perform action prints used mainly to internal
     *  DBAL actions */
    DBAL_COMMIT_DISABLE_ACTION_PRINTS = SAL_BIT(7),

    /** this flag used only for debug purpose, in SBC tables there is a need to validate the other core value */
    DBAL_COMMIT_VALIDATE_OTHER_CORE = SAL_BIT(8),

    /** This flag is used for ignoring resource allocation errors should be used only from BCM shell */
    DBAL_COMMIT_IGNORE_ALLOC_ERROR = SAL_BIT(9),

    DBAL_COMMIT_NOF_OPCODES
} dbal_entry_action_flags_e;

typedef enum
{
    /** will return all entries including "0" entries and default entries only valid for SW and HL */
    DBAL_ITER_MODE_ALL,

    /** will return all entries without entries that equals to default value if not defined default value "0" is used */
    DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT,

    /** Valid only in dbal_iterator_action_commit. Performs commit in a non-blocking mode. (returns before the operation is done) */
    DBAL_ITER_MODE_ACTION_NON_BLOCKING,

    /** Valid only in dbal_iterator_get_next. Reads entry from DMA, after entries were triggered to gather in DMA from flush machine no rules/actions allowed. */
    DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER,

    /** will return all entries without entries that equals to default value (except read-only and trigger) if not defined default value "0" is used */
    DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE,

    DBAL_NOF_ITER_TYPES
} dbal_iterator_mode_e;

/**
 *  \brief enum that represents all the condition types \n when adding values to this enum need also to add
 * corresponding string to: dbal_condition_strings
 */
typedef enum
{
    /** no condition, access will happen anyway
     *  XML indication: NONE */
    DBAL_CONDITION_NONE,

    /** access will happen only if key bigger than value
     *  XML indication: BIGGER_THAN */
    DBAL_CONDITION_BIGGER_THAN,

    /** access will happen only if key lower than value
     *  XML indication: LOWER_THAN */
    DBAL_CONDITION_LOWER_THAN,

    /** access will happen only if key equal to value
     *  XML indication: EQUAL_TO */
    DBAL_CONDITION_EQUAL_TO,

    /** access will happen only if key not equal to value
     *  XML indication: NOT_EQUAL_TO */
    DBAL_CONDITION_NOT_EQUAL_TO,

    /** access will happen only if key is even
     *  XML indication: IS_EVEN */
    DBAL_CONDITION_IS_EVEN,

    /** access will happen only if key is odd
     *  XML indication: IS_ODD */
    DBAL_CONDITION_IS_ODD,

    DBAL_NOF_CONDITION_TYPES
} dbal_condition_types_e;

/**
 *  \brief enum that represents all the iterator actions
 * corresponding string to: dbal_iterator_actions_strings
 */
typedef enum
{
    /** updating result fields */
    DBAL_ITER_ACTION_UPDATE = SAL_BIT(0),

    /** remove entries */
    DBAL_ITER_ACTION_DELETE = SAL_BIT(1),

    /** return entries */
    DBAL_ITER_ACTION_GET = SAL_BIT(2),

    /** clear hit bit */
    DBAL_ITER_ACTION_HIT_CLEAR = SAL_BIT(3),

    /** get hit bit */
    DBAL_ITER_ACTION_HIT_GET = SAL_BIT(4),

    /** read entries to DMA - do not return the entries */
    DBAL_ITER_ACTION_READ_TO_DMA = SAL_BIT(5),
} dbal_iterator_action_types_e;

/**
 *  \brief enum that represents the logical tables access method
 *         types
 */
typedef enum
{
    /** MDB table- Access mapping is known per table, but
     *  not per field. Specific field(s) can be stored in SW
     *  state */
    DBAL_ACCESS_METHOD_MDB,

    /** Hard logic - The Access mapping of the fields is known.
     *  can be either HW or SW state */
    DBAL_ACCESS_METHOD_TCAM_CS,

    /** Hard logic - The Access mapping of the fields is known.
     *  can be either HW or SW state */
    DBAL_ACCESS_METHOD_HARD_LOGIC,

    /** SW Only - Tables which stored in SW state  */
    DBAL_ACCESS_METHOD_SW_STATE,

    /** table that are created by the pemla and configured in init  */
    DBAL_ACCESS_METHOD_PEMLA,

    /** external tcam related tables there are 3 type of tables: LPM (FWD), TCAM (IPMC), TCAM_DIRECT (ACLs) */
    DBAL_ACCESS_METHOD_KBP,

    DBAL_NOF_ACCESS_METHODS
} dbal_access_method_e;

/**
 *  \brief enum logical tables types\n this enum also used to indicate if sw table should be allocated as hash table
 * or direct
 */
typedef enum
{
    /** No table type indication
     * XML indication: not used in XML */
    DBAL_TABLE_TYPE_NONE,

    /** Exact match logical table, also known as hash. XML indication: EM */
    DBAL_TABLE_TYPE_EM,

    /** TCAM indirect logical table, table type is TCAM Key fields uses mask, also entries has priorities. normally in TCAMs
     *  entries has ID (i.e accessID) that represents the position of the entry in the tcam (see other tcam table types), in
     *  type TCAM the entries are managed by KEY, meaning that the user perform actions like get/delete/update only with the
     *  key (without ID) when user wants to add entry it is possible to use ID (if the access layer support it). When
     *  committing to a TCAM table a key field that was not set will be treated as "don't care". this table type is used
     *  normally for FWD applications such as IPMC. XML indication: TCAM */
    DBAL_TABLE_TYPE_TCAM,

    /** TCAM direct logical table, when table type is TCAM_DIRECT the entry is given by key.
     *  When committing to a TCAM table - a key field that was not set will be treated as "don't care" XML indication:
     *  TCAM_DIRECT */
    DBAL_TABLE_TYPE_TCAM_DIRECT,

    /** TCAM indirect logical table, table type is TCAM_BY_ID user need to manage entries with ID see
     *  dbal_entry_handle_access_id_set(). When committing to a TCAM table - a key field that was not set will be treated as
     *  "don't care". this type of table is used usually to ACL applications. XML indication: TCAM_BY_ID */
    DBAL_TABLE_TYPE_TCAM_BY_ID,

    /** LPM logical table, prefix is determine by the mask of the key fields. 
     *  XML indication: LPM */
    DBAL_TABLE_TYPE_LPM,

    /** direct access logical table, entry is accessed to HW according to key XML indication: DIRECT */
    DBAL_TABLE_TYPE_DIRECT,

    DBAL_NOF_TABLE_TYPES
} dbal_table_type_e;

/**
 *  \brief List of physical tables according to the XML definitions from the MDB
 */
typedef enum
{
    /** XML indication: not used in XML */
    DBAL_PHYSICAL_TABLE_NONE,
    /** XML indication: TCAM */
    DBAL_PHYSICAL_TABLE_TCAM,
    /** XML indication: KAPS_1 */
    DBAL_PHYSICAL_TABLE_KAPS_1,
    /** XML indication: KAPS_2 */
    DBAL_PHYSICAL_TABLE_KAPS_2,
    /** XML indication: ISEM_1 */
    DBAL_PHYSICAL_TABLE_ISEM_1,
    /** XML indication: INLIF_1 */
    DBAL_PHYSICAL_TABLE_INLIF_1,
    /** XML indication: IVSI */
    DBAL_PHYSICAL_TABLE_IVSI,
    /** XML indication: ISEM_2 */
    DBAL_PHYSICAL_TABLE_ISEM_2,
    /** XML indication: ISEM_3 */
    DBAL_PHYSICAL_TABLE_ISEM_3,
    /** XML indication: INLIF_2 */
    DBAL_PHYSICAL_TABLE_INLIF_2,
    /** XML indication: INLIF_3 */
    DBAL_PHYSICAL_TABLE_INLIF_3,
    /** XML indication: LEM */
    DBAL_PHYSICAL_TABLE_LEM,
    /** XML indication: IOEM_1 */
    DBAL_PHYSICAL_TABLE_IOEM_1,
    /** XML indication: IOEM_2 */
    DBAL_PHYSICAL_TABLE_IOEM_2,
    /** XML indication: MAP */
    DBAL_PHYSICAL_TABLE_MAP,
    /** XML indication: FEC_1 */
    DBAL_PHYSICAL_TABLE_FEC_1,
    /** XML indication: FEC_2 */
    DBAL_PHYSICAL_TABLE_FEC_2,
    /** XML indication: FEC_3 */
    DBAL_PHYSICAL_TABLE_FEC_3,
    /** XML indication: PPMC */
    DBAL_PHYSICAL_TABLE_PPMC,
    /** XML indication: GLEM_1 */
    DBAL_PHYSICAL_TABLE_GLEM_1,
    /** XML indication: GLEM_2 */
    DBAL_PHYSICAL_TABLE_GLEM_2,
    /** XML indication: EEDB_1 */
    DBAL_PHYSICAL_TABLE_EEDB_1,
    /** XML indication: EEDB_2 */
    DBAL_PHYSICAL_TABLE_EEDB_2,
    /** XML indication: EEDB_3 */
    DBAL_PHYSICAL_TABLE_EEDB_3,
    /** XML indication: EEDB_4 */
    DBAL_PHYSICAL_TABLE_EEDB_4,
    /** XML indication: EEDB_5 */
    DBAL_PHYSICAL_TABLE_EEDB_5,
    /** XML indication: EEDB_6 */
    DBAL_PHYSICAL_TABLE_EEDB_6,
    /** XML indication: EEDB_7 */
    DBAL_PHYSICAL_TABLE_EEDB_7,
    /** XML indication: EEDB_8 */
    DBAL_PHYSICAL_TABLE_EEDB_8,
    /** XML indication: EOEM_1 */
    DBAL_PHYSICAL_TABLE_EOEM_1,
    /** XML indication: EOEM_2 */
    DBAL_PHYSICAL_TABLE_EOEM_2,
    /** XML indication: ESEM */
    DBAL_PHYSICAL_TABLE_ESEM,
    /** XML indication: EVSI */
    DBAL_PHYSICAL_TABLE_EVSI,
    /** XML indication: SEXEM_1 */
    DBAL_PHYSICAL_TABLE_SEXEM_1,
    /** XML indication: SEXEM_2 */
    DBAL_PHYSICAL_TABLE_SEXEM_2,
    /** XML indication: SEXEM_3 */
    DBAL_PHYSICAL_TABLE_SEXEM_3,
    /** XML indication: LEXEM */
    DBAL_PHYSICAL_TABLE_LEXEM,
    /** XML indication: RMEP_EM */
    DBAL_PHYSICAL_TABLE_RMEP_EM,
    /** XML indication: KBP */
    DBAL_PHYSICAL_TABLE_KBP,

    DBAL_NOF_PHYSICAL_TABLES
} dbal_physical_tables_e;

/**
 * \brief enum that represents all logger modes
 */
typedef enum
{
    DBAL_LOGGER_MODE_REGULAR,
    DBAL_LOGGER_MODE_WRITE_ONLY,

    DBAL_NOF_LOGGER_MODES
} dbal_logger_mode_e;

/** structs **/

/**
 *  generic field info for table
 */
typedef struct
{
    /** the field id  */
    dbal_fields_e field_id;

    /** the field type  consider changing to dbal_value_field_encode_types_e  */
    dbal_field_types_defs_e field_type;

    /** full field size in table. notice: access layer can define a different bit size  */
    int field_nof_bits;

    /** the min value of the field (applicable only for fields less than 32 bits) */
    uint32 min_value;

    /** the max value of the field (applicable only for fields less than 32 bits)  */
    uint32 max_value;

    /** the const value of the field (applicable only for fields less than 32 bits) */
    uint32 const_value;

    /** offset to take in logical field the first valid bit to be used in table */
    int offset_in_logical_field;

    /** the field indication bit map each bit indicate property according to DBAL_FIELD_IND_ */
    uint32 field_indication_bm[1];

    /**   location (bit offset) in the handle buffer  */
    int bits_offset_in_buffer;

    /**  nof_instances in the table*/
    int nof_instances;

    /** Indication for read only fields */
    dbal_field_permission_e permission;

    /** Arr prefix - value that will be used to complete the value of the field. in those cases the field in table should
     *  be smaller than the field type size, to complete the full size the arr prefix should be added the value that will
     *  be set for this fields should be according to the full value, but only the affective bit will be set to HW */
    uint32 arr_prefix;

    /** the arr_prefix size that should be used */
    uint8 arr_prefix_size;

} dbal_table_field_info_t;

typedef struct
{
    /** the field id  */
    dbal_fields_e field_id;

    /** the field size in bits, if the field size == DBAL_USE_DEFAULT_SIZE,
     *  uses the default field size (from field type)  */
    uint32 field_nof_bits;

    /** Option to allow valid indication - only for result fields, when this is used the size of the field will be
     *  increased by 1 bit */
    uint8 is_valid_indication_needed;

    /** Indication that the field is ranged. This info is relevant only for KBP tables and is valid only for key
     *  fields */
    uint8 is_ranged;

    /** Arr prefix - value that will be used to complete the value of the field. in those cases the field in table should
     *  be smaller than the field type size, to complete the full size the arr prefix should be added the value that will
     *  be set for this fields should be according to the full value, but only the affective bit will be set to HW */
    uint8 arr_prefix;

    /** the arr_prefix size that should be used */
    uint8 arr_prefix_size;
} dbal_table_field_input_info_t;

/**
 * \brief
 *  per field printable information
 */
typedef struct
{
    dbal_fields_e field_id;
    char field_name[DBAL_MAX_LONG_STRING_LENGTH];
    char field_print_value[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
} dbal_printable_field_t;

/**
 * \brief
 *  full entry printable information
 */
typedef struct
{
    int nof_key_fields;
    int nof_res_fields;
    dbal_printable_field_t key_fields_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    dbal_printable_field_t res_fields_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} dbal_printable_entry_t;

#endif /* DBAL_EXTERNAL_DEFINES_H_INCLUDED */
