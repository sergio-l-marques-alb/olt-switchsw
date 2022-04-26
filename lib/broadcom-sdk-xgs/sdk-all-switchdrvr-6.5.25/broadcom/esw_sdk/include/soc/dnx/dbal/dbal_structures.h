
/**
 *\file dbal_structures.h
 * Main typedefs and enum of dbal.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DBAL_STRUCTURES_H_INCLUDED
#  define DBAL_STRUCTURES_H_INCLUDED

#  ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#  endif

/**************
 *  INCLUDES  *
 **************/

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif


#include <soc/dnx/dbal/dbal_external_defines.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/mcm/allenum.h>
#include <sal/core/thread.h>

#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#endif /* defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

/*************
 *  DEFINES  *
 *************/

 /** List of internal defines for DBAL */

/** \brief The nof structure-fields per structure  */
#  define DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS                   160

/** \brief The nof labels per tables/field type */
#  define DBAL_MAX_NOF_ENTITY_LABEL_TYPES                       10

/** \brief The nof conditions per access */
#  define DBAL_FIELD_MAX_NUM_OF_CONDITIONS                      5

/** \brief The nof parameters per field types, used in init only */
#  define DBAL_MAX_NUMBER_OF_RESULT_TYPES                       41

/** \brief the size in bits of the key buffer in the entry handle */
#  define DBAL_PHYSICAL_KEY_SIZE_IN_BITS                        480

/** \brief
 * The size in bits of the result buffer in the entry handle.
 * Must be multiply of 32! */
#  define DBAL_PHYSICAL_RES_SIZE_IN_BITS                        1600

/** \brief number of entries handle to perform table action with */
#  define DBAL_SW_NOF_ENTRY_HANDLES                             30

/** \brief number of mutex available for per table THREAD_PROTECTION_ENABLED */
#  define DBAL_NOF_SHARED_TABLE_MUTEXES                         10

/** \brief the size in bytes of the key buffer in the entry handle  */
#  define DBAL_PHYSICAL_KEY_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))

/** \brief the size in words of the key buffer in the entry handle  */
#  define DBAL_PHYSICAL_KEY_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))

/** \brief the size in bytes of the result buffer in the entry handle  */
#  define DBAL_PHYSICAL_RES_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_RES_SIZE_IN_BITS))

/** \brief the size in words of the result buffer in the entry handle  */
#  define DBAL_PHYSICAL_RES_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_RES_SIZE_IN_BITS))

/** \brief the size in words of max dbal HL memory   */
#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS            (80)

/** \brief the size in bytes of max dbal HL memory   */
#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES            (DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS*sizeof(uint32))

/** \brief The max key size for direct tables  */
#  define DBAL_TABLE_DIRECT_MAX_KEY_NOF_BITS              32
/**
 *  \brief
 *  Defining the number of valid cores in device.
 *  Use for CORE_ID value validation
 *  Need to be defined in DNX data.
 */
#define DBAL_MAX_NUM_OF_CORES                           (dnx_data_device.general.nof_cores_get(unit))

/** This macro returns the buffer size of the table to perform operations. (it is rounded to words for endianness) */
#define DBAL_TABLE_BUFFER_IN_BYTES(table)               WORDS2BYTES(BITS2WORDS(table->max_payload_size))

/** checks id the table is not direct, corresponds to the function: dbal_tables_is_non_direct */
#define DBAL_TABLE_IS_NONE_DIRECT(table)                 ((table->table_type != DBAL_TABLE_TYPE_DIRECT) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT))
#define DBAL_TABLE_IS_TCAM(table)                        ((table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) || (table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID))

/** key mask is allocated dynamically in the entry handle, the key mask is needed only for specific tables */
#define DBAL_TABLE_IS_KEY_MASK_REQUIRED(table)           (DBAL_TABLE_IS_TCAM(table) || (table->table_type == DBAL_TABLE_TYPE_LPM) || (table->table_type == DBAL_TABLE_TYPE_EM))


/** all the tcam types that manage entries by ID, is_commit means that for entry commit the MDB tcam need also to use
 *  the operation by ID (since the entry ID is allocated outside of the DBAL   */
#define DBAL_TABLE_IS_TCAM_BY_ID(table)          ((table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) || (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID))

/** if all fields were set, no need to merge the values */
#define DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle)        (entry_handle->nof_result_fields != entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields)

#define DBAL_TABLE_IS_TCAM_BY_KEY(table) (table->table_type == DBAL_TABLE_TYPE_TCAM)

/**
 *  \brief
 *  Macro that returns a struct to the result info in handle according to the result type
 */
#define DBAL_RES_INFO            entry_handle->table->multi_res_info[entry_handle->cur_res_type]

/**
 * \brief define the maximum number of static fields allowed in dbal table ETM_PP_DESCRIPTOR_EXPANSION
 */
#define ETM_PP_DESCRIPTOR_EXPANSION_NOF_STATIC_RES_TYPES 1

/**
 *\brief
 * Define the operation type for dbal handle take internal.
 * DBAL_HANDLE_TAKE_ALLOC allocate new handle.
 * DBAL_HANDLE_TAKE_CLEAR reuse existing handle.
 */
#define DBAL_HANDLE_TAKE_ALLOC 0
#define DBAL_HANDLE_TAKE_CLEAR 1


/** \brief The nof parameters per field types */
#define DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES               512

/** \brief The nof child's per field types */
#define DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS              20

/** \brief The nof defines per field types */
#define DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES             40


/** \brief Internal parameters max length in DBAL  */
#define DBAL_MAX_SHORT_STRING_LENGTH                    30

/** \brief Indicates that the size of a field should be its default size. */
#define DBAL_USE_DEFAULT_SIZE                           (-2)

/** \brief this result type value is used to set fields when the result type is not known. in this case all the
 *         result fields can be set on the handle but no action can be performed (commit/get/delete). */
#define DBAL_SUPERSET_RESULT_TYPE                        DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1

/** Max limitations - Need to change implementation to be dynamic and define those by auto-coder */

/** \brief max number of illegal values within the valid range */
#  define DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES             10

/** \brief Max number of variants in an encoding formula */
# define DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS            14

/** \brief Max number of HW value mapping to result type. In set, the first one will be used. */
#define DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE           8

/** \brief max number of registers for array of registers in H.L. direct */
#define DBAL_MAX_NUMBER_OF_HW_ELEMENTS                  100

/*
 * Max limitations
 */
/** \brief Max nof phyDB associated with a table */
#define DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE                 DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES

/** \brief Max number of APP DB ID for MDB tables */
#define DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE             64

/** \brief Basic number of bits for APP DB ID in MDB tables */
#define DBAL_APP_ID_BASIC_NOF_BITS                      6

/*
 * DBAL internal System defines
 */


/** \brief Default phyDB index in table */
#  define DBAL_PHY_DB_DEFAULT_INDEX                     0

/** \brief General define to describe an invalid parameter in DBAL APIs  */
#  define DBAL_INVALID_PARAM                            (-1)

/** \brief when working with multiple result types indicate that result type was not set for the handle */
#  define DBAL_RESULT_TYPE_NOT_INITIALIZED              (-1)

/** \brief all DBAL fields (XML based and ACLs)  */
#  define DBAL_NOF_DYNAMIC_AND_STATIC_FIELDS            (DBAL_NOF_DYNAMIC_FIELDS + DBAL_NOF_FIELDS)

/** \brief nof dynamic fields used by dbal_fields_field_create to create field after init (not by XML) */
#  define DBAL_NOF_DYNAMIC_FIELDS                       (DNX_DATA_MAX_FIELD_QUAL_USER_NOF +\
                                                        DNX_DATA_MAX_FIELD_ACTION_USER_NOF + \
                                                        DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF + \
                                                        DNX_DATA_MAX_FIELD_ACTION_PREDEFINED_NOF)
/** \brief all DBAL fields (XML based and ACLs)  */
#  define DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS          (32)

/*
 * Fields/Key/Result size limitations - need to be removed
 */

/** \brief the size in bytes od max dbal HL memory   */
#define DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER                                        \
    (UTILEX_MAX                                                                     \
    (UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_RES_SIZE_IN_WORDS),  \
         DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS))


/** -brief invalid value. Used by dbal db init field types.   */
#define DBAL_DB_INVALID                (-1)

/** \brief Issue a get hitbit command */
#  define DBAL_PHYSICAL_KEY_HITBIT_NONE                   (0)
/** \brief Issue a get hitbit command */
#  define DBAL_PHYSICAL_KEY_HITBIT_GET                    SAL_BIT(4)
/** \brief Issue a clear hitbit command */
#  define DBAL_PHYSICAL_KEY_HITBIT_CLEAR                  SAL_BIT(5)
/** \brief at least one hit bit command is present */
#  define DBAL_PHYSICAL_KEY_HITBIT_ACTION                 (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_CLEAR | DBAL_PHYSICAL_KEY_HITBIT_WRITE)
/** \brief Issue a hitbit command  on primary */
#  define DBAL_PHYSICAL_KEY_HITBIT_PRIMARY                SAL_BIT(6)
/** \brief Issue a hitbit command on secondary */
#  define DBAL_PHYSICAL_KEY_HITBIT_SECONDARY              SAL_BIT(7)
/** \brief Issue a write hitbit command */
#  define DBAL_PHYSICAL_KEY_HITBIT_WRITE                  SAL_BIT(8)

/** \brief hitbit is set for PRIMARY A ACCESS  */
#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A     SAL_BIT(0)
/** \brief hitbit is set for PRIMARY B ACCESS  */
#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B     SAL_BIT(1)
/** \brief hitbit is set for PRIMARY either ACCESS  */
#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A | DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B)

/** \brief hitbit is set for SECONDARY A ACCESS  */
#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A   SAL_BIT(2)
/** \brief hitbit is set for SECONDARY B ACCESS  */
#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B   SAL_BIT(3)
/** \brief hitbit is set for SECONDARY either ACCESS  */
#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A | DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B)

#  define DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A |   \
                                                                       DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B |   \
                                                                       DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A | \
                                                                       DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B)


#  define DBAL_NOF_HITBIT_COMBINATIONS (16)
#  define DBAL_NOF_FEC_HITBIT_COMBINATIONS (16)
#  define DBAL_NOF_KAPS_HITBIT_COMBINATIONS (4)
#  define DBAL_NOF_LEM_HITBIT_COMBINATIONS (4)
#  define DBAL_NOF_TCAM_HITBIT_COMBINATIONS (SAL_BIT(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES))

#  define DBAL_HITBIT_NOT_SET_RULE (0)
#  define DBAL_HITBIT_SET_RULE     (1)

/* MDB AGE Support*/
/** \brief Issue a get age  command */
#  define DBAL_PHYSICAL_KEY_AGE_NONE                   (0)
/** \brief Issue a get age command */
#  define DBAL_PHYSICAL_KEY_AGE_GET                    SAL_BIT(1)

/*
 * Macros for divide by 32 and modulu of 32
 */
#define DBAL_OPTIMIZED_ITERATOR_DIVIDE_32_SHIFT 5
#define DBAL_OPTIMIZED_ITERATOR_MOD_32_MASK     0x1F
#define DBAL_OPTIMIZED_ITERATOR_GET_WORD_INDEX(_key) (_key >> DBAL_OPTIMIZED_ITERATOR_DIVIDE_32_SHIFT)
#define DBAL_OPTIMIZED_ITERATOR_GET_BIT_OFFSET(_key) (_key & DBAL_OPTIMIZED_ITERATOR_MOD_32_MASK)

/** defines that relevant to dbal_table_field_info_t.field_indication_bm (the value means which bit is relevant to
 *  the indication) */
#define DBAL_FIELD_IND_IS_PARENT_FIELD                  0
#define DBAL_FIELD_IND_IS_FIELD_ENCODED                 1
#define DBAL_FIELD_IND_IS_CONST_VALID                   2
#define DBAL_FIELD_IND_IS_INSTANCE_REVERSE              3
#define DBAL_FIELD_IND_IS_VALID_INDICATION              4
#define DBAL_FIELD_IND_IS_FIELD_ADDRESS                 5 /** in case a field is L2 address or v6 address size validations can not be done */
#define DBAL_FIELD_IND_IS_FIELD_ENUM                    6
#define DBAL_FIELD_IND_IS_ALLOCATOR                     7
#define DBAL_FIELD_IND_IS_RANGED                        8
#define DBAL_FIELD_IND_IS_DYNAMIC                       9
#define DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID           10
/** indication that the field type is shared between all 
 *  images. It can't be overridden. */
#define DBAL_FIELD_IND_IS_COMMON                        11
/** indication that the field type is default,
   and can be overridden by a field type declared in a specific image. */
#define DBAL_FIELD_IND_IS_DEFAULT                       12
#define DBAL_FIELD_IND_IS_STANDARD_1                    13
/** Indication that the ket field for a KBP table is combined with the key field before it into a single segment.*/
#define DBAL_FIELD_IND_IS_PACKED                        14
/**  Indication that this field type is valid for current configuration */
#define DBAL_FIELD_IND_IS_VALID                         15
#define DBAL_NOF_FIELD_IND                             (16)

/**
 *  \brief table indication. See enum values below for details.
 *  Table indication are set by dbal_tables_indication_set function.
 *  dbal_tables_indication_set is called manually or by autocoder. 
 */
typedef enum {
    DBAL_TABLE_IND_IS_HITBIT_EN,                        /** Indicate if hitbit is enabled for this table*/
    DBAL_TABLE_IND_IS_HOOK_ACTIVE,                      /** In case that the hook is active the access to the
                                                          * table is done by dedicated functions. hooks are
                                                          * generally used to fix HW issues or support specific
                                                          * HW behaviors. only HL */

    DBAL_TABLE_IND_HAS_RESULT_TYPE,                     /** Indicates if table has the result_type field */
    DBAL_TABLE_IND_RANGE_SET_SUPPORTED,                 /** Set to 1 if the table support bulk operations
                                                          * (multiple entries with the same value) currently only
                                                          * memories with no encoding is supported (MDB,SW,TCAM_CS,
                                                          * KBP - not supported). */
    DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW,            /** Indicate that table result type field mapped to sw */
    DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED,               /** Indicate that table support priority */
    DBAL_TABLE_IND_IS_LEARNING_EN,                      /** Indicate that table support learning */
    DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED,            /** \brief see:dbal_tables_optimize_commit_mode_set() */
    DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE,             /** if the table is dedicated table for a specific image only */
    DBAL_TABLE_IND_IS_TABLE_DIRTY,                      /** indicates that an entry was added to this table. this indication can be changed dynamically
                                                          * that can be changed by CMD to allow the user to track all the tables that was hit  
                                                          */
    DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED,               /** Indicate the table has bitmap for occupied entries */
    DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD,             /** Indicate the table default entry is different than all zeros*/
    DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED,           /** If this indication enabled, the table will use mutex when an handle for the table is take. 
                                                          * this indication allow to the user to support multi threaded operations on the specific table.
                                                          * the mutex will be released once the handle will be freed. once this indication enabled 
                                                          * only one handle for this table can be used.
                                                          */
    DBAL_TABLE_IND_NO_VALIDATIONS,                       /** Allow user to skip table validations to improve performance:
                                                           * e.g: field value in range, prefix has no hole, skip action check,
                                                           * action validate */

    DBAL_TABLE_IND_KEY_CONST_VALUE_EXISTS,              /** indicate that the table has at least one key field that has const value */
    DBAL_TABLE_IND_IS_ASYMMETRIC,                       /** indicates the table has at least one unmapped access field */
    DBAL_TABLE_IND_ZERO_SIZE_PAYLOAD_SUPPORT,           /** indicates the table supports entry with 0 size payload */
    DBAL_TABLE_IND_LART_SUPPORT,                        /** indicates the table supports LPM A Result Types */
    DBAL_TABLE_IND_IS_NEW_ACCESS,                       /** indicates new access API needs to be used for this table */
    DBAL_NOF_TABLE_IND
} dbal_table_indications_e;

/**************
 *  TYPEDEFS  *
 **************/
/**
 *  \brief Represents the operation in dbal encoding formula
 */
typedef enum
{
    DBAL_FORMULA_OPERATION_ADD,
    DBAL_FORMULA_OPERATION_SUBTRACT,
    DBAL_FORMULA_OPERATION_MULTIPLY,
    DBAL_FORMULA_OPERATION_DIVIDE,
    DBAL_FORMULA_OPERATION_MODULO,
    /** log2 function support.
     *  Used for nof bits. 
     *  See DBAL_LOG2_FUNCTION for function name in xmls */
    DBAL_FORMULA_OPERATION_F_LOG2, 
    DBAL_NOF_FORMULA_OPERATION
} dbal_formula_operations_e;

/**
 *  \brief
 *  indicate the action type read, should not change the state and write modify the state (add entry/delete entry).
 *  SW only indicates that the access layer should can do SW actions without any HW reads
 */
typedef enum
{
    DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
    DBAL_ACTION_ACCESS_TYPE_READ,
    DBAL_ACTION_ACCESS_TYPE_WRITE,

    DBAL_NOF_ACTION_ACCESS_TYPES
} dbal_action_access_type_e;


/**
 *  \brief
 *  Enum to indicate is a key value is valid or not during iterating a table
 */
typedef enum
{
    /** The key value is valid */
    DBAL_KEY_IS_VALID,
    /** The key value is valid, but not passing the iterator rules */
    DBAL_KEY_IS_OUT_OF_ITERATOR_RULE,
    /** The key value is invalid */
    DBAL_KEY_IS_INVALID
} dbal_key_value_validity_e;


/**
 * \brief
 *  Describes the common DBAL action access functions
 */
typedef enum
{
    DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC,
    DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC,
    DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_GET,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR,
    DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET,
    /* not a real access function */
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_FLUSH_MACHINE_GET,
    DBAL_NOF_ACTION_ACCESS_FUNCS
} dbal_action_access_func_e;


#define DBAL_ENTRY_ATTR_PRIORITY         SAL_BIT(0)
#define DBAL_ENTRY_ATTR_AGE              SAL_BIT(1)
#define DBAL_ENTRY_ATTR_HIT_GET          SAL_BIT(2)   /** hitbit bits are parsed according to dbal_mdb_hitbit_to_string */
#define DBAL_ENTRY_ATTR_HIT_CLEAR        SAL_BIT(3)
#define DBAL_ENTRY_ATTR_HIT_PRIMARY      SAL_BIT(4)
#define DBAL_ENTRY_ATTR_HIT_SECONDARY    SAL_BIT(5)
#define DBAL_ENTRY_ATTR_CACHED           SAL_BIT(6)
#define DBAL_ENTRY_ATTR_INVALID          SAL_BIT(7)   /** when set means that the entry is in installed in the HW but not valid*/
#define DBAL_ENTRY_ATTR_NO_PAYLOAD       SAL_BIT(8)   /** Means entry is 0 size payload */
#define DBAL_NOF_ENTRY_ATTR_TYPE         (9)

/**
 *  \brief
 *  DBAL entry attributes types:
 *  PRIORITY  - entry priority
 *  AGE       - entry AGE
 *  HIT       - get entry hit bit
 *  CACHED    - get entry cache indication
 */
typedef enum
{
    DBAL_ENTRY_ATTR_RES_PRIORITY,
    DBAL_ENTRY_ATTR_RES_AGE,
    DBAL_ENTRY_ATTR_RES_HIT,
    DBAL_ENTRY_ATTR_RES_CACHED,
    DBAL_ENTRY_ATTR_RES_INVALID,
    DBAL_ENTRY_ATTR_RES_NO_PAYLOAD,
    DBAL_NOF_ENTRY_ATTR_RES_TYPE
} dbal_entry_attr_res_type_e;

/**
 *  \brief Represents the modules that support bulk operations.
 */
typedef enum
{
    DBAL_BULK_MODULE_NONE,
    DBAL_BULK_MODULE_MDB_LEM_FLUSH,
    DBAL_BULK_MODULE_MDB_LEM_NON_BLOCKING_FLUSH,
    DBAL_BULK_MODULE_MDB_KAPS,
    DBAL_BULK_MODULE_KBP_FWD,
    DBAL_BULK_MODULE_KBP_ACL,

    DBAL_NOF_BULK_MODULE
} dbal_bulk_module_e;

/**
 *  \brief dbal system status
 *
 */
typedef enum
{
    DBAL_STATUS_NOT_INITIALIZED,
    DBAL_STATUS_DBAL_INIT_DONE,
    DBAL_STATUS_DEVICE_INIT_DONE,

    DBAL_NOF_STATUS
} dbal_status_e;


/**
 *  \brief List of dbal logger types
 */
typedef enum dnx_dbal_logger_type_t
{
    DNX_DBAL_LOGGER_TYPE_API,
    DNX_DBAL_LOGGER_TYPE_ACCESS,
    DNX_DBAL_LOGGER_TYPE_DIAG,
    DNX_DBAL_LOGGER_TYPE_LAST
} dnx_dbal_logger_type_e;

/**
 *  \brief List of physical tables according to the XML definitions from the MDB
 */
typedef enum
{
    /** USE pp_stage instead of dbal_stage.
     * Find solution for PRT TCAM (that doesn't have its own stage).
     *  include <soc/dnx/pp_stage.h>   */
    DBAL_STAGE_NONE,
    DBAL_STAGE_PRT,
    DBAL_STAGE_LLR,
    DBAL_STAGE_VT1,
    DBAL_STAGE_VT2,
    DBAL_STAGE_VT3,
    DBAL_STAGE_VT4,
    DBAL_STAGE_VT5,
    DBAL_STAGE_FWD1,
    DBAL_STAGE_FWD2,
    DBAL_STAGE_IPMF1,
    DBAL_STAGE_IPMF2,
    DBAL_STAGE_IPMF3,
    DBAL_STAGE_EPMF,
    DBAL_NOF_STAGES
} dbal_stage_e;

/**
 * \brief
 * List of possible actions when committing an entry:
 * Normal - commit, error if the entry exists
 * Update - update, error if the entry doesn't exist
 * Force  - commit if the entry doesn't exist and update if the entry exists
 * */
typedef enum {
    DBAL_INDIRECT_COMMIT_MODE_NORMAL,
    DBAL_INDIRECT_COMMIT_MODE_UPDATE,
    DBAL_INDIRECT_COMMIT_MODE_FORCE,
    DBAL_NOF_INDIRECT_COMMIT_MODE
} dbal_indirect_commit_mode_e;

typedef struct
{
    dbal_stage_e stage;

    /**general info */
    int line_length_key_only;
    int line_length_with_key_size_prefix;
    int supports_half_entries;

    /**command memory info */
    soc_mem_t cmd_memory;
    soc_field_t cmd_write_field;
    soc_field_t cmd_read_field;
    soc_field_t cmd_compare_field;
    soc_field_t cmd_valid_field;
    soc_field_t cmd_key_field;
    soc_field_t cmd_mask_field;

    /**reply memory info */
    soc_mem_t reply_memory;
    soc_field_t reply_valid_field;
    soc_field_t reply_data_out_field;

    /** ECC FIX_EN register info*/
    soc_reg_t ecc_fix_en_reg;
} dbal_tcam_cs_stage_info_t;

/**
 *  \brief enum field types, field type used for print output\n this enum also used to indicate the field type for
 * getting field when adding values to this enum need also to add corresponding string to: dbal_field_type_strings
 * when adding new type, add it also to sal_field_type_e
 */
typedef enum
{
    /** No field type indication
     * XML indication: not used in XML */
    DBAL_FIELD_PRINT_TYPE_NONE,

    /** prints TRUE/FALSE
     *  XML indication: BOOL */
    DBAL_FIELD_PRINT_TYPE_BOOL,

    /** field type is uint32, prints unsigned values 32bit
     *  XML indication: UINT32 */
    DBAL_FIELD_PRINT_TYPE_UINT32,

    /** prints: x.x.x.x (32bit)
     *  XML indication: IPV4 */
    DBAL_FIELD_PRINT_TYPE_IPV4,

    /** prints: x:x:x:x:x:x:x:x (128bit)
     *  XML indication: IPV6 */
    DBAL_FIELD_PRINT_TYPE_IPV6,

    /** prints: x:x:x:x:x:x (48bit)
     *  In addition, when a value is set to a field  (key/value
     *  fields), using arr8 APIs, the order of the Array is modified
     *  from MAC address order to regular arr order.
     *  MAC address order: byte 0 = MS byte, byte 5 = LS byte
     *  regular array order: byte 0 = LS byte, byte 5 = MS byte
     *  XML indication: MAC */
    DBAL_FIELD_PRINT_TYPE_MAC,

    /** field type is char array, print format 0xAA:0xBB:0xCC..
     *  XML indication: ARRAY8 */
    DBAL_FIELD_PRINT_TYPE_ARRAY8,

    /** field type is uint32 array, print format 0xaabbccdd 0xaabbccdd
     *  XML indication: ARRAY32 */
    DBAL_FIELD_PRINT_TYPE_ARRAY32,

    /** print format 11010110...
     *  XML indication: BITMAP */
    DBAL_FIELD_PRINT_TYPE_BITMAP,

    /** print enum format prints string for each valid value
     *  XML indication: ENUM, only one allowed for enum field type */
    DBAL_FIELD_PRINT_TYPE_ENUM,

    /** special field type, indicate the core id
     *  XML indication: SYSTEM_CORE  */
    DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE,

    /** field type from structure type has multiple fields that the concatenation of them creates the field. when printing it
     *  will print the struct decoded to struct fields.
     *  XML indication: STRUCTURE, only one allowed for structure field type */
    DBAL_FIELD_PRINT_TYPE_STRUCTURE,

    /** print format 0xa23
     *  XML indication: HEX */
    DBAL_FIELD_PRINT_TYPE_HEX,

    /** print format string of the related table ID
     *  XML indication: HEX */
    DBAL_FIELD_PRINT_TYPE_DBAL_TABLE,

    /** field type is string
     *  XML indication: STR */
    DBAL_FIELD_PRINT_TYPE_STR,

    DBAL_NOF_FIELD_PRINT_TYPES
} dbal_field_print_type_e;

/**
 *  \brief enum pointer types\n this enum use to identify the pointer type when using void* APIs internal DBAL use.
 */
typedef enum
{
    /** un-initialized pointer type */
    DBAL_POINTER_TYPE_NONE,

    /** pointer type uint8  limited to 8 bit   */
    DBAL_POINTER_TYPE_UINT8,

    /** pointer type uint16  limited to 16 bit   */
    DBAL_POINTER_TYPE_UINT16,

    /** pointer type uint32 limited to 32 bit */
    DBAL_POINTER_TYPE_UINT32,

    /** pointer type uint8* */
    DBAL_POINTER_TYPE_ARR_UINT8,

    /** pointer type uint32* */
    DBAL_POINTER_TYPE_ARR_UINT32,

    /** pointer type uint64* */
    DBAL_POINTER_TYPE_UINT64,

    DBAL_NOF_POINTER_TYPES
} dbal_pointer_type_e;

/**
 *  \brief enum represents the handle status
 */
typedef enum
{
    /** handle is available for action */
    DBAL_HANDLE_STATUS_AVAILABLE,
	
    /** handle is used by DBAL */
    DBAL_HANDLE_STATUS_IN_USE,

	/** action was performed on handle, but the entry is not released */
    DBAL_HANDLE_STATUS_ACTION_PREFORMED,

    DBAL_NOF_ENTRY_HANDLE_STATUSES
} dbal_entry_handle_status_e;

/**
 *  \brief enum represents the handle status
 */
typedef enum
{
    DBAL_HL_ENTITY_FIELD,
    DBAL_HL_ENTITY_REGISTER,
    DBAL_HL_ENTITY_MEMORY,

    DBAL_NOF_HL_ENTITIES
} dbal_hl_entity_type_e;

/**
 *  \brief enum that represents all the encoding types for field values \n when adding values to this enum need also
 * to add corresponding string to: dbal_field_encode_type_strings
 */
typedef enum
{
    /** no encoding, value will stay the same
     * XML indication: NONE */
    DBAL_VALUE_FIELD_ENCODE_NONE,

    /** value shifted with prefix
     *  Supported for field only
     *  XML indication: PREFIX */
    DBAL_VALUE_FIELD_ENCODE_PREFIX,

    /** add suffix to value
     *  Supported for field only
     *  XML indication: SUFFIX */
    DBAL_VALUE_FIELD_ENCODE_SUFFIX,

    /** value*input_parm
     *  Supported only in table access (HL)
     *  XML indication: MULTIPLE */
    DBAL_VALUE_FIELD_ENCODE_MULTIPLY,

    /** value=concatenation of all sub fields
     *  XML indication: BITWISE_NOT */
    DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT,

    /** value shifted by one and added valid bit to LSB
     *  Supported for field only
     *  XML indication: VALID_IND */
    DBAL_VALUE_FIELD_ENCODE_VALID_IND,

    /** Below are Encoding types which relate to field types  */

    /** value=transform[enum_value]
     *  XML indication: not used in XML */
    DBAL_VALUE_FIELD_ENCODE_ENUM,

    DBAL_NOF_VALUE_FIELD_ENCODE_TYPES
} dbal_value_field_encode_types_e;

/**
 *  \brief enum that represents hard logic direct access types
 */
typedef enum
{
    /** No mapping for this access
     * Put in first for immediately detecting presence
     * of unmapped field, saving access to mem or reg */
    DBAL_HL_ACCESS_UNMAPPED,

    /** memory access */
    DBAL_HL_ACCESS_MEMORY,

    /** register access   */
    DBAL_HL_ACCESS_REGISTER,

    /** SW access for specific field in table */
    DBAL_HL_ACCESS_SW,

    DBAL_NOF_HL_ACCESS_TYPES
} dbal_hard_logic_access_types_e;

/**
 *  \brief enum that represents all the core modes
 */
typedef enum
{
    /** XML indication: not used in XML */
    DBAL_CORE_NONE,

    /** dedicated per code. XML indication: DPC */
    DBAL_CORE_MODE_DPC,

    /** Shared between cores. XML indication: SBC */
    DBAL_CORE_MODE_SBC,

    DBAL_NOF_CORE_MODE_TYPES
} dbal_core_mode_e;

/**
 *  \brief
 *  This value should be used prior to entry_commit command, in
 *  case the user want to commit the entry to tables in all
 *  cores. Valid for use with DPC tables only
 */
#  define DBAL_CORE_ALL           (-17) /** Defined as SOC_CORE_ALL for compilation time improvements */

/**
 *  \brief
 *  This value should be used prior to entry_get command, in
 *  case the user want to get the entry for ANY core table.
 *  Valid for use with DPC tables only
 */
#  define DBAL_CORE_DEFAULT        (0) /** Defined as SOC_CORE_DEFAULT for compilation time improvements */

/**
 *  \brief
 *  Used when initialized an dbal entry handle for a DPC table.
 *  Indicates tht a core ID should be specify.
 */
#  define DBAL_CORE_NOT_INITIALIZED   (-1) /** Defined as SOC_CORE_INVALID for compilation time improvements */

/**
 *  \brief this define is used to define the size of core fields
 *         in bits.
 */
#define DBAL_CORE_SIZE_IN_BITS     (dnx_data_device.general.core_max_nof_bits_get(unit))

/**
 *  \brief enum that represents the maturity level of a table
 *         according to regressions
 */
typedef enum
{
    /** Low
     *  Do not add thee table to the DB, table is invalid
     *  XML indication - "LOW" */
    DBAL_MATURITY_LOW,

    /** Medium
     *  Add the table to the Db so basic test can be tested
     *  table is not part of the regression
     *  XML indication - "PARTIALLY_FUNCTIONAL" */
    DBAL_MATURITY_PARTIALLY_FUNCTIONAL,

    /** High
     *  Table is ready to be part of the regression
     *  XML indication - "HIGH" */
    DBAL_MATURITY_HIGH,

    /** High, for device only
     *  Table is ready to be part of the regression in device only
     *  XML indication - "HIGH_SKIP_ADAPTER" */
    DBAL_MATURITY_HIGH_SKIP_ADAPTER,

    /** High, for Adapter only Table is ready to be part of the regression in device only XML indication -
     *  "HIGH_SKIP_DEVICE" */
    DBAL_MATURITY_HIGH_SKIP_DEVICE,

	/** High, when vendor dnx  . DBAL_MATURITY_PARTIALLY_FUNCTIONAL Otherwise
	 * Xml indication: HIGH_VENDOR_DNX */
	DBAL_MATURITY_HIGH_VENDOR_DNX,

    DBAL_NOF_MATURITY_LEVELS
} dbal_maturity_level_e;


/**
 *  \brief Represents all DBAL actions (actions that perform access)
 *  if new ACESS type wants to be fully compatible with DBAL needs to implement all the following actions.
 */
typedef enum
{
    DBAL_ACTION_ENTRY_COMMIT,
    DBAL_ACTION_ENTRY_COMMIT_UPDATE,
    DBAL_ACTION_ENTRY_COMMIT_FORCE,
    DBAL_ACTION_ENTRY_COMMIT_RANGE,
    DBAL_ACTION_ENTRY_GET,
    DBAL_ACTION_ENTRY_GET_ALL,
    DBAL_ACTION_ENTRY_GET_ACCESS_ID,
    DBAL_ACTION_ITERATOR_GET,
    DBAL_ACTION_ENTRY_CLEAR,
    DBAL_ACTION_ENTRY_CLEAR_RANGE,  
    DBAL_ACTION_TABLE_CLEAR,
    DBAL_ACTION_SKIP, /** in this case the action validation returns indication to skip the action. */

    DBAL_NOF_ACTIONS
} dbal_actions_e;


/**
 * \brief Indication if dbal action to MDB should be apply to HW/SW shadow/both
 * The default is both
 * HW only is expected to be used when dynamic entries are set to MACT.
 * SW Shadow only is expected to be used when after flush machine is updating HW and shadow need to be synced.
 * Should be used in add and delete only.
 */
typedef enum
{
    DBAL_MDB_ACTION_APPLY_ALL,
    DBAL_MDB_ACTION_APPLY_NONE,
    DBAL_MDB_ACTION_APPLY_HW_ONLY,
    DBAL_MDB_ACTION_APPLY_SW_SHADOW,

    /** entry should be added to HW but Valid bit should be disabled (applicable for TCAM only)  */
    DBAL_MDB_ACTION_APPLY_ENTRY_INVALID,
    DBAL_NOF_MDB_ACTION_APPLY_TYPES
} dbal_mdb_action_apply_type_e;

/**
 *  \brief struct that holds the fields ID and the fields value
 */
typedef struct
{
    dbal_fields_e field_id;
    uint8 inst_idx;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
} dbal_field_data_t;

/**
 *  \brief
 *  Fields offset formula callback
 *  Unit, entry_handle are passed to cb by dbal
 *  Result should be placed inside the offset field
 */
typedef shr_error_e (
    *dnx_dbal_formula_offset_cb) (
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

/**
 *  \brief
 *  Fields offset formula callback
 *  Unit, entry_handle are passed to cb by dbal
 *  Result should be placed inside the offset field
 */
typedef shr_error_e (
    *dnx_dbal_field_type_illegal_value_cb) (
    int unit,
    uint32 value,
    uint8 *is_illegal);

/**
 *  \brief
 *  An encoding formula structure
 */
typedef struct dbal_offset_formula_t
{
    char as_string[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    int nof_vars;
    uint8 key_val;
    uint8 result_size;
    uint8 instance_index;
    uint32 val;
    dbal_fields_e field_id;
    int field_pos_in_table;
    int is_result;
    dbal_formula_operations_e action[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1];
    struct dbal_offset_formula_t *val_as_formula;
    dnx_dbal_formula_offset_cb formula_offset_cb;
} dbal_offset_formula_t;


typedef struct dbal_int_or_cb_s
{
    uint32 val;
    dnx_dbal_formula_offset_cb cb;
} dbal_int_or_cb_t;

/**
 *  \brief struct that centralized offset information, with the
 *         following info the offset is calculated
 */
typedef struct
{
    uint32 internal_inparam;
    dbal_int_or_cb_t *formula;
} dbal_offset_encode_info_t;

/**
 *  \brief struct that centralized field encode information, with the following info logical value to physical value
 * is calculated
 */
typedef struct
{
    dbal_value_field_encode_types_e encode_mode;
    uint32 input_param;
} dbal_field_type_encode_info_t;

/**
 *  \brief struct that centralized sub-field encoding information, with the following info the transformation from
 * sub-field to parent field is performed
 */
typedef struct
{
    dbal_fields_e sub_field_id;
    dbal_field_type_encode_info_t encode_info;
} dbal_sub_field_info_t;

/**
 *  \brief struct that centralized sub-field encoding information, with the following info the transformation from
 * sub-field to parent field is performed
 */
typedef struct
{
    dbal_fields_e struct_field_id;
    int offset;
    int length;
    /** Arr prefix - value that will be used to complete the value of the field. in those cases the field in table should
     *  be smaller than the field type size, to complete the full size the arr prefix should be added the value that will
     *  be set for this fields should be according to the full value, but only the affective bit will be set to HW */
    uint32 arr_prefix;

    /** 
     *  the arr_prefix size is a prefix size for the field. 
     *  It's a computed value:
     *  arr prefix size = max field size - table field size.
     *  With max field size = field type size.
     *  
     *  The arr prefix is a prefix value for the field.
     *  It's a computed value:
     *  msbs from arr prefix table entry value.
     *  with Nof msbs to take = arr prefix size
     */
    uint8 arr_prefix_size;

    /** ARR prefix table entry width - Per PP block are defined an ARR prefix table.
     *  The ARR table has a fixed width.
     *  This value is received from XML, used for arr_prefix and
     *  arr_prefix_size calculation.
     *  Shouldn't be used after init.
     */
    int arr_prefix_table_entry_width;

    /** ARR prefix table entry value - Per PP block are defined an ARR prefix table.
     *  A field is optionally associated with an ARR prefix table
     *  entry.
     *  This value is received from XML, used for arr_prefix and
     *  arr_prefix_size calculation.
     *  Shouldn't be used after init.
     */ 
    int arr_prefix_table_entry_value;

} dbal_sub_struct_field_info_t;

/**
 *  \brief struct that centralized enum values information
 */
typedef struct
{
    uint32 value;
    int is_invalid;
    char name[2*DBAL_MAX_STRING_LENGTH];
} dbal_enum_decoding_info_t;

/**
 *  \brief struct that centralized defined values information
 */
typedef struct
{
    uint32 value;
    char name[DBAL_MAX_STRING_LENGTH];
} dbal_defiens_info_t;

/**
 *  \brief struct that centralized condition information, with
 *         the following info the condition is calculated
 */
typedef struct
{
    dbal_condition_types_e type;
    uint32 value[UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS)];
    uint32 mask[UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS)];
    dbal_fields_e field_id;
    int field_pos_in_table; /** holds the field position in table to improve performance */
} dbal_access_condition_info_t;

/**
 *  \brief struct centralizing interaction information
 */
typedef struct
{
    dbal_iterator_action_types_e action_type;
    uint32 value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    dbal_fields_e field_id;
} dbal_iterator_actions_info_t;

/**
 *  \brief fields basic information
 */
typedef struct
{
    /** Field type name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** Field type max size in bits */
    uint32 max_size;

    /** Field type printing type */
    dbal_field_print_type_e print_type;

    /** the field indication bit map each bit indicate property according to DBAL_FIELD_IND_ */
    uint32 field_indication_bm[1];
    
    /** Field type default_ value of field type: value */
    uint32 default_value;

    /** max value of field type */
    uint32 max_value;

    /** min value of field type */
    uint32 min_value;

    /** const value of field type */
    uint32 const_value;

    /** the illegal values within the range on min-max values(applicable only for fields less than 32 bits) */
    int nof_illegal_values;
    uint32 illegal_values[DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES];

    /** Field type child fields info */
    int nof_child_fields;
    dbal_sub_field_info_t *sub_field_info;

    /** Field type struct fields info */
    int nof_struct_fields;
    dbal_sub_struct_field_info_t *struct_field_info;

    /** Field type enum info */
    int nof_enum_values;
    int nof_invalid_enum_values;
    dbal_enum_decoding_info_t *enum_val_info;

    /** callback to indicate illegal values for field type, this is used for cases that the illegal values can be dynamic
     *  to set a callback put it in dbal_field_types_init_illegal_value_cb_assign */
    dnx_dbal_field_type_illegal_value_cb illegal_value_cb;

    /** Field type defines info */
    int nof_defines_values;
    dbal_defiens_info_t *defines_info;

    /** Field type that is used for the encoding of the child fields */
    dbal_field_types_defs_e reference_field_id;

    /** Field type encoding info, when writing field to HW */
    dbal_field_type_encode_info_t encode_info;
} dbal_field_types_basic_info_t;

/**
 * \brief enum that represents all the valid access types for HL
 *        TCAM\n
 */
typedef enum
{
    DBAL_HL_TCAM_ACCESS_TYPE_KEY,
    DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK,
    DBAL_HL_TCAM_ACCESS_TYPE_RESULT,
    DBAL_NOF_HL_TCAM_ACCESS_TYPES
} dbal_hl_tcam_access_type_e;

/**
 * \brief enum for updating entry's result type policy\n
 */
typedef enum
{
    /** Don't allow information loss, all result fields must exist in new result type */
    DBAL_RESULT_TYPE_UPDATE_MODE_STRICT,
    /* Allow information loss, all result fields which only exist in the existing result type will be ignored.
       In case of existing field with different range - take it's new default value */
    DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE,
    /* Allow information loss for fields which only exist in the existing result type and their value equals to default value.
       In case of existing field with different range - take it's new default value */
    DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT,
    DBAL_NOF_RESULT_TYPE_UPDATE_MODES
} dbal_result_type_update_mode_e;

/**
 *  \brief logical 2 physical HL access field info
 */
typedef struct
{
    dbal_fields_e field_id;

    /**
     * Instance index of this field in the interface
     */
    uint8 inst_idx;

    /**
     * Used only for HL_TCAM tables
     * Used for identification of the current access type (Key/KeyMask/Result)
     */
    dbal_hl_tcam_access_type_e hl_tcam_access_type;

    /**
     *  access_nof_bits and access_offset are used only in the
     *  parsing process. the real offset and size are located in
     *  nof_bits_in_multiple_result, offset_in_multiple_result
     */
    uint32 access_nof_bits;
    uint32 access_offset;

    /*
     * Encoding per access, id set, field value on handle will be
     * re-encoded while writing to HW
     */

    dbal_field_type_encode_info_t encode_info;

    /**
     *  field index in interface structure
     *  interface structure it the structure of type
     *  dbal_table_field_info_t
     */
    int field_pos_in_interface;

    /** field length (bits) in interface structure  */
    uint32 nof_bits_in_interface;

    /** field offset (bits) in interface structure  */
    uint32 offset_in_interface;

    /** hw entities of mapping mem/register, field   */
    soc_mem_t *memory;
    soc_reg_t *reg;

    /** if set, indicates for this field that the default entry is different than 0*/
    int is_default_non_standard;

    soc_field_t hw_field;
 
    /** group id. Group a set of register or memories  */
    dbal_hw_entity_group_e hw_entity_group_id; 

    /**
     *  array index of mem/reg
     *  Default is writing to all array's elements
     */
    dbal_offset_encode_info_t array_offset_info;

    /**
     *  entry index of mem, invalid for register
     *  Default is entire key
     */
    dbal_offset_encode_info_t entry_offset_info;

    /**
     *  Data offset in HW entity - in bits If hw field is set, the
     *  offset is inside the hw field, if not the offset is inside
     *  the entry
     */
    dbal_offset_encode_info_t data_offset_info;

    /**
     *  entry index of mem \ register
     *  Default is entire key
     */
    dbal_offset_encode_info_t block_index_info;

        /**
     *  group index of mem \ register Default is ALL
     */
    dbal_offset_encode_info_t group_offset_info;

    /**
     *  If set, the data is written to HW using this alias memory.
     */
    soc_mem_t alias_memory;

    /**
     *  If set, the data is written to HW using this alias register.
     */
    soc_reg_t alias_reg;

    /**
     *  Offset calculation for alias memory/register
     */
    dbal_offset_encode_info_t alias_data_offset_info;

    int nof_conditions;

    /** Condition for mapping. Mapping is done only if condition is true. */
    dbal_access_condition_info_t *mapping_condition;

} dbal_hl_l2p_field_info_t;

typedef struct
{
    int num_of_access_fields;
    dbal_hl_l2p_field_info_t *l2p_fields_info; /**In case of TCAM: alloc only 3 elements for key, mask and result*/
} dbal_hl_l2p_info_t;

typedef struct
{
    soc_mem_t memory;
} dbal_cs_access_info_t;

typedef struct
{
    /** number of bytes that are mapped to SW */
    int sw_payload_length_bytes;

    /** hold the formula for table size indicating the physical dbs according to the capacity of the SW table is set */
    char *table_size_str;
    dbal_int_or_cb_t table_size_formula;

} dbal_sw_access_info_t;

typedef struct
{
    /** if set, indicates for this result type that the default entry is not standard */
    int is_default_non_standard;

    dbal_hl_l2p_info_t l2p_hl_info[DBAL_NOF_HL_ACCESS_TYPES];
} dbal_hl_access_info_t;

/** \brief
 *  this flag is used for setting Pemla field mapping to not
 *  valid used when a field is not defined in Application DB
 */
#define DBAL_PEMLA_FIELD_MAPPING_INVALID (0xFFFFFFFF)

typedef struct
{
    uint32 *key_fields_mapping;
    uint32 *result_fields_mapping;
    dbal_hard_logic_access_types_e access_type;
    uint32 reg_mem_id;
    
} dbal_pemla_db_mapping_info_t;

#define DBAL_ATTR_FLAG_INVALID                   SAL_BIT(0) /** See DBAL_ENTRY_ATTR_INVALID */
#define DBAL_ATTR_FLAG_NO_PAYLOAD                SAL_BIT(1) /** See DBAL_ENTRY_ATTR_NO_PAYLOAD */

typedef struct
{
    /** HW ID received from user, this is used for tcam access */
    int entry_hw_id;

    /** in case the entry is relevant only for a specific core use this (supported only for MDB TCAM) */
    int core_id;

    /** Valid for non-direct tables.
     * Indicates (to the access layer) if the entry will be updated or it is a new one.
     * If the entry already exists, the payload is merged with the existing entry.
     * The access layer only needs to add this entry and remove the old one.
     */
    dbal_indirect_commit_mode_e indirect_commit_mode;

    /** inout parameter for cmd  See DBAL_PHYSICAL_KEY_HITBIT_X defines */
    uint16 hitbit;

    /* Flags for attributes */
    uint16 attr_flags;

    /** inout parameter for mdb aging  See DBAL_MSB_AGE defines */
    uint32 age;

    dbal_mdb_action_apply_type_e mdb_action_apply;

    /** Relevant for LPM tables the key prefix length according to k_mask, for TCAM it means the priority */
    uint32 prefix_length;

    uint32 key_size;
    uint32 payload_size;
    
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];

    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 p_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

} dbal_physical_entry_t;

typedef struct
{
    uint32 app_id;              /* The app_id associated with the last retrieved entry. */
    uint32 mdb_entry_index;     /* next logical entry index to be retrieved (direct/EEDB/EM-HW table) */
    int mdb_cluster_index;      /* The cluster index, used to iterate through all the clusters associated with a table */
    uint32 payload_basic_size;  /* The payload basic size associated with direct table (except EEDB/IN_LIF), in EEDB
                                 * does not include LL */

    /*
     * Flush machine iterator info:
     * This information id used to decide whether to iterate via flush machine or sw iterator
     */

    /** indication if user set a rule on the result type */
    uint8 has_rt_rule;

    /** indication if iterator is done via flush (does NOT indicate that ONLY flush machine is used) */
    uint8 iterate_in_flush_machine;

    /** indication if iterator is done via sw iterator (does NOT indicate that ONLY sw iterator is used)  */
    uint8 start_non_flush_iteration;

    /** Bitmap of result types which are iterated via sw iterator */
    uint32 result_types_in_non_flush_bitmap;
    /** hit bit flags if needed in the iterator get requests. */
    uint8 hit_bit_flags;
    /** age flags if needed in the iterator get requests. */
    uint8 age_flags;
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    kbp_db_t_p kbp_lpm_db_p;       /* KBPSDK DB pointer */
    kbp_ad_db_t_p kbp_lpm_ad_db_p; /* KBPSDK AD DB pointer */
    struct kbp_entry_iter *kbp_lpm_iter;        /* KBPSDK iterator pointer */
#endif                          /* defined(INCLUDE_KBP) */

    mdb_kaps_db_t_p mdb_lpm_db_p;       /* KAPS DB pointer */
    struct kaps_entry_iter *mdb_lpm_iter;        /* KAPS iterator pointer */

    SW_STATE_HASH_TABLE_ITER mdb_em_htb_iter;     /* Used to iterate over the adapter EM shadow DB. */
    SW_STATE_HASH_TABLE_ITER mdb_em_key_size;
    /** By default set to max payload, set to 0 to retrieve the payload size from SW shadow */
    SW_STATE_HASH_TABLE_ITER mdb_em_payload_size;

    uint32 mdb_em_ratios; /* The EM ratios for the row specified by mdb_entry/cluster_index. */
    uint32 mdb_em_way_index; /* The EM way_index of the current cluster. */
    uint32 mdb_em_emc_bank_enable; /* Used for MDB EM HW iterator to restore the EMC bank enables at deinit. */
    uint32 mdb_em_emp_bank_enable; /* Used for MDB EM HW iterator to restore the EMP bank enables at deinit. */

    dbal_mdb_action_apply_type_e mdb_action_apply;

} dbal_physical_entry_iterator_t;

typedef struct
{
    SW_STATE_HASH_TABLE_PTR hash_table_id;
    SW_STATE_HASH_TABLE_ITER hash_entry_index;

} dbal_sw_table_iterator_t;

typedef struct
{
    dbal_pointer_type_e pointer_type;
    void *value_returned_pointer;
    void *mask_returned_pointer;
} dbal_user_output_info_t;

/** struct that represent result type  */
typedef struct
{
    int result_type_nof_hw_values;

    uint32 result_type_hw_value[DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE];
    /* LPM-A Result Type (LART) index */
    uint8  lart_index;
    /**
     * result type name.
     * used for diagnostics and logging
     */
    char result_type_name[DBAL_MAX_STRING_LENGTH];

    /** entry payload_size in bits */
    int entry_payload_size;

    /** number of result field  */
    int nof_result_fields;

    /** result field information  */
    dbal_table_field_info_t *results_info;

    /** incase the result type is mapped according to a field */
    dbal_field_types_defs_e reference_field_id;

    uint8 is_disabled;

    /** result type has link list field */
    uint8 has_link_list;
} multi_res_info_t;

/**
 *  \brief struct that centralized DBAL action information */
typedef struct
{
    /** action name */
    char name[DBAL_MAX_SHORT_STRING_LENGTH];

    /** if set to 1 when logger enabled will print before the action the log, else after the action */
    int is_pre_acces_prints;

    /** flags that indicate which validation to make to the action */
    uint32 action_validation_flags;

    /** supported flags for action dbal_entry_action_flags_e */
    uint32 supported_flags;
} dbal_action_info_t;

typedef struct
{
    int nof_physical_tables;
    dbal_physical_tables_e physical_db_id[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];
    uint32 tcam_handler_id;
    uint32 app_id;
    int app_id_size;

} dbal_table_mdb_access_info_t;

typedef struct
{
    int sw_state_nof_entries;
    int sw_payload_length_bytes;

} dbal_table_sw_access_info_t;

/**
 *  \brief Logical table structure definition
 */
typedef struct
{
    /** General table parameters  */
    char table_name[DBAL_MAX_STRING_LENGTH];
    dbal_maturity_level_e maturity_level;
    int nof_labels;
    dbal_labels_e *table_labels;
    dbal_table_type_e table_type;

    /** the max capacity of the table only applicable for direct and TCAM tables for non direct should be according to  HW */
    int max_capacity;

    /** table indications bitmap see DBAL_TABLE_IND_   */
    uint32 indications_bm[1];

    /** Interface parameters information regarding the key and result fields  */
    int nof_key_fields;
    dbal_table_field_info_t *keys_info;

    /** the nof bits of the key, this is the total of bits used for the key, there are cases that the HW key can be
     *  smaller since core_id is not part of the key to HW (for DPC tables) */
    uint32 key_size;

    /** if core ID is present in the key (in DPC mode), those bits are reduced from the phy_entry buffer */
    uint32 core_id_nof_bits;

    /** allocator field is used for indication that in get next need to call
     *   resource manager to find the next available index
     */
    dbal_fields_e allocator_field_id;

    /* Pointer to bitmap array of table entries, each bit indicates an entry status, each core has it's own bitmap*/
    uint32 **iterator_optimized;

    /**
     * Nof result type for the table.
     * This is the total number or result types.
     * Disabled Results types per device are counted here.
     */
    int nof_result_types;
    multi_res_info_t *multi_res_info;

    int max_payload_size;
    int max_nof_result_fields;

    dbal_core_mode_e core_mode;
    dbal_access_method_e access_method; /** HL, PHY, SW_ONLY, PEMLA */

    /** when set, used for DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED */
    uint8 mutex_id;

    /******* Access Layer Info  ******/

    /** SW state access info */
    dbal_sw_access_info_t sw_access_info;
    
    /** MDB access info  */
    int nof_physical_tables;
    dbal_physical_tables_e physical_db_id[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];

    uint32 app_id; /** app_id, also use for kbp_db_id*/
    int app_id_size;

    /** DBAL stage info for TCAM CS and MDB TCAM.
     *  There is one extra "stage" for PRT TCAM (even though it is not a real stage). */
    dbal_stage_e dbal_stage;

    /**
     * Hard logic (or HL+SW) access mapping. access mapping per mem, reg, sw.
     * For disabled result types, the hl_mapping_multi_res is all 0.
     **/
    dbal_hl_access_info_t *hl_mapping_multi_res;

    /** Tcam CS access info (also used dbal_stage)  */
    dbal_cs_access_info_t *tcam_cs_mapping;

    /** Pemla info */
    dbal_pemla_db_mapping_info_t pemla_mapping;

    /** KBP access info (also uses app_id)  */
    void *kbp_handles;

} dbal_logical_table_t;

/** this struct contains only the printable table params  */
typedef struct
{
    char table_name[DBAL_MAX_STRING_LENGTH];
    char table_status[DBAL_MAX_SHORT_STRING_LENGTH];
    char maturity_level[DBAL_MAX_SHORT_STRING_LENGTH];
    char labels[DBAL_MAX_NOF_ENTITY_LABEL_TYPES * DBAL_MAX_SHORT_STRING_LENGTH];
    char table_type[DBAL_MAX_SHORT_STRING_LENGTH];
    char core_mode[DBAL_MAX_SHORT_STRING_LENGTH];
    char access_method[DBAL_MAX_SHORT_STRING_LENGTH];
    char range_set_supported[DBAL_MAX_SHORT_STRING_LENGTH];

} dbal_table_string_t;

/**
 * \brief
 *  Error information related to field_set/ field_get operations.
 */
typedef struct
{
    /**
     * error code related to the last operation
     */
    int error_exists;
    /**
     * The field ID related to the error
     */
    dbal_fields_e field_id;
    /**
     * Field instance index
     */
    uint8 inst_idx;
} field_error_info_t;

/**
 * \brief
 *  Structure to hold the dbal attributes information of an iterator
 */
typedef struct
{
    /** hit bit rule valid */
    uint8 hit_bit_rule_valid;

    /** hit bit rule indication for hit or no hit rule */
    uint8 hit_bit_rule_is_hit;

    /** hit bit rule indication for hitbit type */
    uint8 hit_bit_rule_type;

    /** hit bit action get flag */
    uint8 hit_bit_action_get;

    /** Indicate which attribute to get */
    uint8 hit_bit_action_get_type;

    /** hit bit action clear flag */
    uint8 hit_bit_action_clear;

    /**Indicate which attribute to clear */
    uint8 hit_bit_action_clear_type;

    /**Indicate if there is an aging rule on iterator */
    uint8 age_rule_valid;

    /**Indicate the aging rule read from entry value */
    uint32 age_rule_entry_value;

    /**Indicate the aging rule compare value */
    uint32 age_rule_compare_value;

}dbal_iterator_attribute_info_t;

typedef struct
{
    /** flag to indicates that the iterator is initiated */
    uint8 is_init;

    /** indicates for done iterating, Current entry is not valid */
    uint8 is_end;

    /**
     *  MDB only indicates the mdb table which is currently iterated
     *  over second physical DB
     */
    uint8 physical_db_index;

    /** indicates that the first key was used - and the key should
     *  be promoted (if false it is the first time we call to the iterator) */
    uint8 used_first_key;

    /** entries found counter */
    uint32 entries_counter;

    /** nof of key rules */
    uint32 nof_key_rules;

    /** key_rules_info */
    dbal_access_condition_info_t *key_rules_info;

    /** nof of value rules */
    uint32 nof_val_rules;

    /** val_rules_info */
    dbal_access_condition_info_t *val_rules_info;

    /** attributes_info rules and actions */
    dbal_iterator_attribute_info_t attrib_info;

    /** nof of actions */
    uint32 nof_actions;

    /** hold information of which actions to perform, from dbal_iterator_action_types_e */
    uint32 actions_bitmap;

    /** actions_info */
    dbal_iterator_actions_info_t *actions_info;

    /** Direct tables (H.L and SW direct) iterator data */
    uint32 max_num_of_iterations;

    /** SW HASH iterator data */
    dbal_sw_table_iterator_t sw_iterator;

    /** MDB iterator data */
    dbal_physical_entry_iterator_t mdb_iterator;

    dbal_iterator_mode_e mode;

} dbal_iterator_info_t;

typedef struct
{
    int handle_id;

    dbal_tables_e table_id;

    /**  pointer to the corresponding table  */
    dbal_logical_table_t *table;

    /**
     *  all key fields added to the table according to their position in the table
     *  it saves the actual field ID in this position, to support subfields
     */
    dbal_fields_e key_field_ids[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    /** per field ID the the range of entries to set  */
    uint32 key_field_ranges[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    /**
     *  all value fields added to the table according to their position in the table
     *  it saves the actual field ID in this position, to support subfields
     */
    dbal_fields_e value_field_ids[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];

    uint8 nof_key_fields;

    uint8 nof_ranged_fields;
    uint8 nof_result_fields;
    uint8 entry_merged;

    /**  Handle status according to enum */
    dbal_entry_handle_status_e handle_status;

    /**
     * core_id holds the core to perform the action, in table that works in core_mode by input if the core ID was not added
     * we use the default value
     */
    int core_id;

    /**
     * the current result type value init with -1 valid values can
     * be 0 - DBAL_MAX_NUMBER_OF_RESULT_TYPES, define value DBAL_RESULT_TYPE_NOT_INITIALIZED for NOT_INITIALIZED.
     */
    int cur_res_type;

    /**
     *   information about get fields that will be updated after entry get
     *   memory for the information is allocated dynamically only when needed in order to improve memory usage
     *   the allocation takes place when dbal_entry_value_field_request() is called by the user
     *   the memory is freed automatically when clearing or releasing the entry handle
     */
    dbal_user_output_info_t *user_output_info;

    /**  information about get attribute that will be updated after entry get Holds the attribute return pointer */
    uint32 *attribute_info[DBAL_NOF_ENTRY_ATTR_RES_TYPE];

    /**
     *  error information that is saved in the entry handle during
     *  field set/get procedure call, this info is returned to user
     *  only after calling entry commit/get/delete
     */
    field_error_info_t error_info;

    /**  if this field is set no need to perform read before write. only applicable during init corresponds to the flag
     *   DBAL_COMMIT_OVERRUN_ENTERY */
    uint8 overrun_entry;

    /**  indicate that all fields are requested. */
    uint8 get_all_fields;

    /**  indicate that access_id in the phy entry was set, applicable only for TCAM tables. */
    uint8 access_id_set;

    /** indicate if rollback and comparison journals are currently on if DNX_DBAL_JOURNAL_ROLLBACK_FLAG
     *  and/or DNX_DBAL_JOURNAL_COMPARE_FLAG are set */
    uint8 er_flags;

    /**  buffers that contains all the fields values must be at the end since smart memory handling is used see
     *   dbal_actions_access_entry_get and dbal_entry_handle_copy_internal */
    dbal_physical_entry_t phy_entry;

} dbal_entry_handle_t;

typedef struct
{
    char name[DBAL_MAX_STRING_LENGTH];
    uint32 val;
    uint32 min_bit;
    uint32 max_bit;
    uint32 len;
} dbal_sub_field_signal_info_t;

/*****************************************************PHYSICAL TABLE DEFENITIONS***************************************************************/

/* DBAL_MDB_ENTRY_FLAGS */
/** flags that are used to pass info between DBAL and MDB when performing entry actions such as add/get/delete */

/** \brief  
  * this enum is used to set flags for dbal_mdb_entry using PHYSICAL_TABLE_ENTRY_ADD/ PHYSICAL_TABLE_ENTRY_GET/ PHYSICAL_TABLE_ENTRY_DELETE 
  * to set a flag use:  SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_XX);
  * to read a flag use: SHR_IS_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_XX);
  */
typedef enum {
    DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS,       /** indicate if the entry includes link list (applicable only for eedb tables) */
    
    DBAL_NOF_DBAL_MDB_ENTRY_FLAGS
} dbal_mdb_entry_flags;


typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_ADD) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_GET) (
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_DELETE) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_CLEAR) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);

typedef shr_error_e(
    *PHYSICAL_TABLE_DEFAULT_VALUES_SET) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_INIT) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_DEINIT) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_INIT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_GET_NEXT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_DEINIT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

typedef struct
{
    PHYSICAL_TABLE_ENTRY_ADD entry_add;
    PHYSICAL_TABLE_ENTRY_GET entry_get;
    PHYSICAL_TABLE_ENTRY_DELETE entry_delete;
    PHYSICAL_TABLE_CLEAR table_clear;
    PHYSICAL_TABLE_DEFAULT_VALUES_SET table_default_values_set;
    PHYSICAL_TABLE_INIT table_init;
    PHYSICAL_TABLE_DEINIT table_deinit;
    PHYSICAL_TABLE_ITERATOR_INIT iterator_init;
    PHYSICAL_TABLE_ITERATOR_GET_NEXT iterator_get_next;
    PHYSICAL_TABLE_ITERATOR_DEINIT iterator_deinit;

} dbal_physical_table_actions_t;

typedef struct
{
    uint8 group_hw_entity_num;
    int   hw_entity_id[DBAL_MAX_NUMBER_OF_HW_ELEMENTS];
    uint8 is_reg;
} hl_group_info_t;

typedef struct
{
    uint32 nof_groups;
    hl_group_info_t *group_info;
} hl_groups_info_t;

typedef struct
{
    /** indicates the number of tables that had error during init because of HW issue */
    int nof_tables_with_error;

    dbal_logical_table_t logical_tables[DBAL_NOF_DYNAMIC_AND_STATIC_TABLES];

    /** struct to all hl hw entity groups  */
    hl_groups_info_t groups_info;

} dbal_logical_tables_info_t;

/** Allows to override default field print type
 * DBAL_LOGGER_OVER_PRINT_TYPE_DEFAULT - prints as defined in the field print type
 * DBAL_LOGGER_PRINT_TYPE_AS_HEX - prints all numeric types in hexa
 */
typedef enum
{
    DBAL_LOGGER_PRINT_TYPE_DEFAULT = 0,
    DBAL_LOGGER_PRINT_TYPE_AS_HEX,

    DBAL_LOGGER_NOF_PRINT_TYPES
} dbal_logger_print_type_e;

typedef struct
{
    /** Global flag to disable logger prints, currently used under result type resolution */
    uint8 disable_logger;

    /** logger - indicate that is locked on specific table, per user request */
    dbal_tables_e user_log_locked_table;

    /** logger - indicate that is locked on specific table, per dbal internal decision */
    dbal_tables_e internal_log_locked_table;

    /** output file for logs, only if exists by property */
    FILE *dbal_file;

    /** logger mode, (write only) */
    dbal_logger_mode_e logger_mode;

    /** logger print type - overrides default field print type */
    dbal_logger_print_type_e print_type;

} dbal_logger_info_t;

/** struct that centralize protection info for table related to: DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED */
typedef struct
{
    sal_mutex_t pre_table_mutex;
    dbal_tables_e table_id; /** relevant dbal table that associated to this mutex, in case DBAL_TABLE_EMPTY means that mutex is available.*/
} dbal_specific_table_protection_info_t;

typedef struct
{
    uint32 action_types_btmp;
    uint8 hit_indication_rule;
    uint8 hit_indication_rule_mask;
    uint32 key_rule_buf[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 key_rule_mask_buf[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 value_rule_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_rule_mask_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_action_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_action_mask_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 key_rule_size;
    uint32 value_rule_size;
    uint32 value_action_size;
} dbal_flush_shadow_info_t;

typedef struct
{
    dbal_entry_handle_t *entry_handles_pool;
    dbal_iterator_info_t iterators_pool[DBAL_SW_NOF_ENTRY_HANDLES];
    dbal_logger_info_t logger_info;
    dbal_status_e status;
    int is_std_1;

    /** used for indicate that the pemla access is applicable (after loading the pemla driver)*/
    dbal_status_e pemla_status; 

    /** mutex that is used for taking handle since there are multiple threads that are using DBAL */
    sal_mutex_t handle_mutex;

    /** pool of threads that ca be used for table protecation. see DBAL_NOF_SHARED_TABLE_MUTEXES */
    dbal_specific_table_protection_info_t protect_info[DBAL_NOF_SHARED_TABLE_MUTEXES]; 
} dbal_mngr_info_t;

#endif /* DBAL_STRUCTURES_H_INCLUDED */
