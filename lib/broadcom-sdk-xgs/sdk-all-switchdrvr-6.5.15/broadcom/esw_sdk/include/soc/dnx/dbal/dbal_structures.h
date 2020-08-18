
/**
 *\file dbal_structures.h
 * Main typedefs and enum of dbal.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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

#  include <soc/dnx/pp_stage.h>
#  include <soc/dnx/dbal/auto_generated/dbal_defines_system.h>
#  include <soc/dnx/dbal/auto_generated/dbal_defines_max_allocations.h>
#  include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#  include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#  include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#  include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#  include <shared/shrextend/shrextend_debug.h>
#  include <shared/utilex/utilex_integer_arithmetic.h>
#  include <soc/dnxc/swstate/types/sw_state_hash_table.h>

#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#endif /* defined(INCLUDE_KBP) */

/*************
 *  DEFINES  *
 *************/
/** \brief Internal parameters max length in DBAL  */
#define DBAL_MAX_SHORT_STRING_LENGTH                    30

/** \brief Short names max length in DBAL - Limits the Fields/Tables/Groups names */
#define DBAL_MAX_STRING_LENGTH                          72

/** \brief Extra-Long max length in DBAL - Formula and HW names and DNX_DATA */
#define DBAL_MAX_EXTRA_LONG_STRING_LENGTH               160

/** \brief Long names max length in DBAL - Any concatenation of two names  */
#define DBAL_MAX_LONG_STRING_LENGTH                     (2*DBAL_MAX_EXTRA_LONG_STRING_LENGTH + DBAL_MAX_STRING_LENGTH)

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

#define DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS          50
/*
 * Max limitations
 */
/** \brief Max nof phyDB associated with a table */
#define DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE                 2

/** \brief Max number of APP DB ID for MDB tables */
#define DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE             64

/** \brief Basic number of bits for APP DB ID in MDB tables */
#define DBAL_APP_ID_BASIC_NOF_BITS                      6

/** \brief DBAL printable string for field max size  */
#  define DBAL_MAX_PRINTABLE_BUFFER_SIZE                1024


/*
 * DBAL internal System defines
 */
/** \brief used for fields instances APIs when there is no multiple instances for the field in a specific table */
#  define INST_SINGLE                                   (-1)

/** \brief used for fields instances APIs when all the instances of the field in current table are relevant */
#  define INST_ALL                                      (-2)

/** \brief used for range APIs to indicate that all range is required dbal_entry_key_field32_range_set() */
#  define DBAL_RANGE_ALL                                (-1)

/** \brief Default phyDB index in table */
#  define DBAL_PHY_DB_DEFAULT_INDEX                     0

/** \brief General define to describe an invalid parameter in DBAL APIs  */
#  define DBAL_INVALID_PARAM                            (-1)

/** \brief when working with multiple result types indicate that result type was not set for the handle */
#  define DBAL_RESULT_TYPE_NOT_INITIALIZED              (-1)

/** \brief number of entries handle to perform table action with */
#  define DBAL_SW_NOF_ENTRY_HANDLES                     10

/** \brief all DBAL tables (XML based and ACLs) */
#  define DBAL_NOF_DYNAMIC_AND_STATIC_TABLES            (DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES + DBAL_NOF_TABLES)

/** \brief all DBAL fields (XML based and ACLs)  */
#  define DBAL_NOF_DYNAMIC_AND_STATIC_FIELDS            (DBAL_NOF_DYNAMIC_FIELDS + DBAL_NOF_FIELDS)

/** \brief nof dynamic fields used by dbal_fields_field_create to create field after init (not by XML) */
#  define DBAL_NOF_DYNAMIC_FIELDS                       (DNX_DATA_MAX_FIELD_QUAL_USER_NOF +\
                                                        DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
/** \brief all DBAL fields (XML based and ACLs)  */
#  define DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS          (32)

/*
 * Fields/Key/Result size limitations - need to be removed
 */
/** \brief the size of field with type array in bytes  */
#  define DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES            (BITS2BYTES(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

/** \brief the size of field with type array in words  */
#  define DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS            (BITS2WORDS(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

/** \brief the size in bytes of the key buffer in the entry handle  */
#  define DBAL_PHYSICAL_KEY_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))

/** \brief the size in words of the key buffer in the entry handle  */
#  define DBAL_PHYSICAL_KEY_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))

/** \brief the size in bytes of the result buffer in the entry handle  */
#  define DBAL_PHYSICAL_RES_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_RES_SIZE_IN_BITS))

/** \brief the size in words of the result buffer in the entry handle  */
#  define DBAL_PHYSICAL_RES_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_RES_SIZE_IN_BITS))

/** \brief the size in words od max dbal HL memory   */
#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS            (80)

/** \brief the size in bytes od max dbal HL memory   */
#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES            (DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS*sizeof(uint32))

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
#  define DBAL_PHYSICAL_KEY_HITBIT_ACTION                 (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
/** \brief Issue a hitbit command  on primary */
#  define DBAL_PHYSICAL_KEY_HITBIT_PRIMARY                SAL_BIT(6)
/** \brief Issue a hitbit command on secondary */
#  define DBAL_PHYSICAL_KEY_HITBIT_SECONDARY              SAL_BIT(7)

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

#  define DBAL_HITBIT_NOT_SET_RULE (0)
#  define DBAL_HITBIT_SET_RULE     (1)

/* MDB AGE Support*/
/** \brief Issue a get age  command */
#  define DBAL_PHYSICAL_KEY_AGE_NONE                   (0)
/** \brief Issue a get hitbit command */
#  define DBAL_PHYSICAL_KEY_AGE_GET                    SAL_BIT(1)


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
    DBAL_NOF_FORMULA_OPERATION
} dbal_formula_operations_e;

/**
 *  \brief
 *  indicate the action type read, should not chage the state and write modify the state (add entry/delete entry).
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
#define DBAL_ENTRY_ATTR_HIT_GET          SAL_BIT(2)
#define DBAL_ENTRY_ATTR_HIT_CLEAR        SAL_BIT(3)
#define DBAL_ENTRY_ATTR_HIT_PRIMARY      SAL_BIT(4)
#define DBAL_ENTRY_ATTR_HIT_SECONDARY    SAL_BIT(5)
#define DBAL_NOF_ENTRY_ATTR_TYPE         (6)

/**
 *  \brief
 *  DBAL entry attributes types:
 *  PRIORITY  - entry priority
 *  AGE       - entry AGE
 *  HIT       - get entry hit bit
 */
typedef enum
{
    DBAL_ENTRY_ATTR_RES_PRIORITY,
    DBAL_ENTRY_ATTR_RES_AGE,
    DBAL_ENTRY_ATTR_RES_HIT,
    DBAL_NOF_ENTRY_ATTR_RES_TYPE
} dbal_entry_attr_res_type_e;

/**
 *  \brief Represents the modules that support bulk operations.
 */
typedef enum
{
    DBAL_BULK_MODULE_NONE,
    DBAL_BULK_MODULE_MDB_LEM_FLUSH,
    DBAL_BULK_MODULE_MDB_KAPS,
    DBAL_BULK_MODULE_KBP,

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
 *  \brief List of physical tables according to the XML definitions from the MDB
 */
typedef enum
{
    
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
     *  XML indication: ENUM  */
    DBAL_FIELD_PRINT_TYPE_ENUM,

    /** special field type, indicate the core id
     *  XML indication: SYSTEM_CORE  */
    DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE,

    /** field type from structure type has multiple fields that the concatenation of them creates the field. when printing it
     *  will print the struct decoded to struct fields.
     *  XML indication: STRUCTURE  */
    DBAL_FIELD_PRINT_TYPE_STRUCT,

    /** print format 0xa23
     *  XML indication: HEX */
    DBAL_FIELD_PRINT_TYPE_HEX,

    /** print format string of the related table ID
     *  XML indication: HEX */
    DBAL_FIELD_PRINT_TYPE_DBAL_TABLE,

    /** field type is string
     *  XML indication: STR */
    DBAL_FIELD_PRINT_TYPE_STRING,

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
 *  \brief enum logical tables types\n this enum also used to indicate if sw table should be allocated as hash table
 * or direct
 */
typedef enum
{
    /** No table type indication
     * XML indication: not used in XML */
    DBAL_TABLE_TYPE_NONE,

    /** Exact match logical table
     *  XML indication: EM */
    DBAL_TABLE_TYPE_EM,

    /** TCAM indirect logical table, when table type is TCAM user need to manage entries with ID
     *  see dbal_entry_handle_access_id_set.
     *  When committing to a TCAM table - a key field that
     *  was not set will be treated as "don't care"
     *  XML indication: TCAM */
    DBAL_TABLE_TYPE_TCAM,

    /** TCAM direct logical table, when table type is TCAM_DIRECT the entry is given by key.
     *  When committing to a TCAM table - a key field that
     *  was not set will be treated as "don't care"
     *  XML indication: TCAM_DIRECT */
    DBAL_TABLE_TYPE_TCAM_DIRECT,

    /** LPM logical table
     *  XML indication: LPM */
    DBAL_TABLE_TYPE_LPM,

    /** direct access logical table,
     *  entry is accessed according to key
     *  XML indication: DIRECT */
    DBAL_TABLE_TYPE_DIRECT,

    DBAL_NOF_TABLE_TYPES
} dbal_table_type_e;

/**
 *  \brief enum represents the handle status
 */
typedef enum
{
    DBAL_HANDLE_STATUS_AVAILABLE,
    DBAL_HANDLE_STATUS_IN_USE,
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
     *  XML indication: PREFIX */
    DBAL_VALUE_FIELD_ENCODE_PREFIX,

    /** add suffix tp value
     *  XML indication: SUFFIX */
    DBAL_VALUE_FIELD_ENCODE_SUFFIX,

    /** value-input_parm
     *  XML indication: SUBTRACT */
    DBAL_VALUE_FIELD_ENCODE_SUBTRACT,

    /** value+input_parm
     *  XML indication: ADD */
    DBAL_VALUE_FIELD_ENCODE_ADD,

    /** value*input_parm
     *  XML indication: MULTIPLE */
    DBAL_VALUE_FIELD_ENCODE_MULTIPLE,

    /** value/input_parm
     *  XML indication: DIVIDE */
    DBAL_VALUE_FIELD_ENCODE_DIVIDE,

    /** value%input_parm
     *  XML indication: MODULO */
    DBAL_VALUE_FIELD_ENCODE_MODULO,

    /** value=input_parm
     *  XML indication: HARD_VALUE */
    DBAL_VALUE_FIELD_ENCODE_HARD_VALUE,

    /** value=concatenation of all sub fields
     *  XML indication: BITWISE_NOT */
    DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT,

    /** value shifted by one and added valid bit to LSB
     *  XML indication: VALID_IND */
    DBAL_VALUE_FIELD_ENCODE_VALID_IND,

    /** Below are Encoding types that related to field types  */

    /** value=transform[enum_value]
     *  XML indication: not used in XML */
    DBAL_VALUE_FIELD_ENCODE_ENUM,

    /** value=concatenation of all sub fields
     *  XML indication: not used in XML */
    DBAL_VALUE_FIELD_ENCODE_STRUCT,

    DBAL_NOF_VALUE_FIELD_ENCODE_TYPES
} dbal_value_field_encode_types_e;

/**
 *  \brief enum that represents hard logic direct access types
 */
typedef enum
{
    /** memory access */
    DBAL_HL_ACCESS_MEMORY,

    /** register access   */
    DBAL_HL_ACCESS_REGISTER,

    /** SW access for specific field in table */
    DBAL_HL_ACCESS_SW,

    DBAL_NOF_HL_ACCESS_TYPES
} dbal_hard_logic_access_types_e;

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

    /** external tcam related tables  */
    DBAL_ACCESS_METHOD_KBP,

    DBAL_NOF_ACCESS_METHODS
} dbal_access_method_e;

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
#  define DBAL_CORE_ALL           (-17) 

/**
 *  \brief
 *  This value should be used prior to entry_get command, in
 *  case the user want to get the entry for ANY core table.
 *  Valid for use with DPC tables only
 */
#  define DBAL_CORE_DEFAULT        (0) 

/**
 *  \brief
 *  Used when initialized an dbal entry handle for a DPC table.
 *  Indicates tht a core ID should be specify.
 */
#  define DBAL_CORE_NOT_INTIATED   (-1) 

/**
 *  \brief this define is used to define the size of core fields
 *         in bits.
 */
#define DBAL_CORE_SIZE_IN_BITS     (1)  

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
} dbal_iterator_action_types_e;

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
     *  XML indication - "HIGH_SKIP_CMODEL" */
    DBAL_MATURITY_HIGH_SKIP_CMODEL,

    /** High, for cmodel only
     *  Table is ready to be part of the regression in device only
     *  XML indication - "HIGH_SKIP_DEVICE" */
    DBAL_MATURITY_HIGH_SKIP_DEVICE,

    DBAL_NOF_MATURITY_LEVELS
} dbal_maturity_level_e;


/**
 *  \brief enum that represents the mdb tables image type status
 */
typedef enum
{
    /**
     * Table is from Std_1 image, and std1 is supported
     * Table is fully functional
     */
    DBAL_MDB_IMG_STD_1_ACTIVE,

    /**
     * Table is from Std_1 image, and std1 is NOT supported
     * Table's actions are ignored
     */
    DBAL_MDB_IMG_STD_1_NOT_ACTIVE,

    /**
     * Table is NOT from Std_1 image, but from user defined image
     * Table's actions are valid from Shell and CINT only
     */
    DBAL_MDB_IMG_NOT_STD_1,

    DBAL_NOF_MDB_IMG_TYPES
} dbal_mdb_table_image_type_e;

typedef enum
{
    /*
     * will return all entries including "0" entries and default entries only valid for SW and HL
     */
    DBAL_ITER_MODE_ALL,

    /*
     * will return all entries without entries that equals to default value if not defined default value "0" is used
     */
    DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT,

    /*
     * will return all entries without entries that equals to default value (except read-only and trigger) if not defined default value "0" is used
     */
    DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE,

    DBAL_NOF_ITER_TYPES
} dbal_iterator_mode_e;

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
     *  DBAL_COMMIT, DBAL_COMMIT_UPDATE_ENTERY and DBAL_COMMIT_FORCE_ENTRY.*/
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

    /** DBAL_COMMIT_NO_ID - not supported yet, this Flag is used only for TCAM, when used dbal_entry_handle_access_id_request should be called
     *  before commit to return the access_id allocated by access layer IF KBP will provide new API to allocate handle ID no
     *  need to support it DBAL_COMMIT_NO_ID */

    DBAL_COMMIT_NOF_OPCODES
} dbal_entry_action_flags_e;

/**
 *  \brief this enum represents all the predefine values that can be set for field.
 *   predefine values are values that can be
 */
typedef enum
{
    DBAL_PREDEF_VAL_MIN_VALUE,
    DBAL_PREDEF_VAL_MAX_VALUE,
    DBAL_PREDEF_VAL_DEFAULT_VALUE,
    DBAL_PREDEF_VAL_RESET_VALUE,

    DBAL_NOF_PREDEFINE_VALUES
} dbal_field_predefine_value_type_e;

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
    DBAL_ACTION_ENTRY_CLEAR,
    DBAL_ACTION_ENTRY_CLEAR_RANGE,
	DBAL_ACTION_ITERATOR_GET,
    DBAL_ACTION_TABLE_CLEAR,
    DBAL_ACTION_SKIP, /** in this case the action validation returns indication to skip the action. */

    DBAL_NOF_ACTIONS
} dbal_actions_e;


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
 * \brief Indication if dbal action to MDB should be apply to HW/SW shadow/both
 * The default is both
 * HW only is expected to be used when dynamic entries are set to MACT.
 * SW Shadow only is expected to be used when after flush machime is updating HW and shadow need to be synced.
 * Should be used in add and delete only.
 */
typedef enum
{
    DBAL_MDB_ACTION_APPLY_ALL = 0,
    DBAL_MDB_ACTION_APPLY_NONE = 1,
    DBAL_MDB_ACTION_APPLY_HW_ONLY = 2,
    DBAL_MDB_ACTION_APPLY_SW_SHADOW = 3,
    DBAL_NOF_MDB_ACTION_APPLY_TYPES
} dbal_mdb_action_apply_type_e;

/**
 *  \brief struct that holds the fields ID and the fields value
 */
typedef struct
{
    dbal_fields_e field_id;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
} dbal_field_data_t;

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
    dbal_formula_operations_e action[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1];
    struct dbal_offset_formula_t *val_as_formula;

} dbal_offset_formula_t;

/**
 *  \brief struct that centralized offset information, with the
 *         following info the offset is calculated
 */
typedef struct
{
    uint32 internal_inparam;
    dbal_offset_formula_t *formula;
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
} dbal_sub_struct_field_info_t;

/**
 *  \brief struct that centralized enum values information
 */
typedef struct
{
    uint32 value;
	int is_invalid;
    char name[DBAL_MAX_LONG_STRING_LENGTH];
} dbal_enum_decoding_info_t;

/**
 *  \brief struct that centralized defined values information
 */
typedef struct
{
    uint32 value;
    char name[DBAL_MAX_LONG_STRING_LENGTH];
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
} dbal_access_condition_info_t;

/**
 *  \brief struct that centralized iteraactions information
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

    /** Field type is resource allocated */
    uint8 is_allocator;

    /** Field type default_ value of field type: indication */
    uint8 is_default_value_valid;

    /** Field type default_ value of field type: value */
    uint32 default_value;

    /** max value of field type */
    uint32 max_value;

    /** min value of field type */
    uint32 min_value;

    /** const value of field type */
    uint32 const_value;

    /** const value valid indication */
    uint8 const_value_valid;

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

    /** Field type defines info */
    int nof_defines_values;
    dbal_defiens_info_t *defines_info;

    /** Field type that is used for the encoding of the child fields */
    dbal_field_types_defs_e refernce_field_id;

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
    /** Allow information loss, all result fields which only exist in the existing result type will be ignored */
    DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE,
    /** Allow information loss for fields which only exist in the existing result type and their value equals to default value */
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
     * Used only for HL_TCAM tables
     * Used for identification of the current access type (Key/KeyMask/Result)
     * */
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
    soc_mem_t memory[DBAL_MAX_NUMBER_OF_HW_ELEMENTS];
    soc_reg_t reg[DBAL_MAX_NUMBER_OF_HW_ELEMENTS];
    soc_field_t hw_field;

    /** if group off registers / memories should be accessed */
    char group_name[DBAL_MAX_STRING_LENGTH];

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
     *  offset is inside the hw field, if not the offset is iside
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
    /**
     * Condition for mapping.
     * Mapping is done only if condition is true.
     */
    dbal_access_condition_info_t *mapping_condition;

} dbal_hl_l2p_field_info_t;

typedef struct
{
    int num_of_access_fields;
    uint8 is_packed_fields;
    dbal_hl_l2p_field_info_t *l2p_fields_info; /**In case of TCAM: alloc only 3 elements for key, mask and result*/
} dbal_hl_l2p_info_t;

typedef struct
{
    /** if set, indicates for this result type that the default entry is not standard */
    int is_default_non_standard;

    dbal_hl_l2p_info_t l2p_hl_info[DBAL_NOF_HL_ACCESS_TYPES];
} dbal_hl_access_info_t;

/** \brief
 *  this flag is used for setting Pemla field mapping to not
 *  valid used whn a field is not defined in Application DB
 */
#define DBAL_PEMLA_FIELD_MAPPING_INVALID (0xFFFFFFFF)

typedef struct
{
    uint32 *key_fields_mapping;
    uint32 *result_fields_mapping;
} dbal_pemla_db_mapping_info_t;

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

} dbal_table_field_input_info_t;

/**
 *  generic field info for table
 */
typedef struct
{
    /** the field id  */
    dbal_fields_e field_id;

    /** the field type  */
    dbal_field_types_defs_e field_type;

    /** full field size in table. notice: access layer can define a different bit size  */
    int field_nof_bits;

    /** the min value of the field (applicable only for fields less than 32 bits) */
    uint32 min_value;

    /** the max value of the field (applicable only for fields less than 32 bits)  */
    uint32 max_value;

    /** the const value of the field (applicable only for fields less than 32 bits) */
    uint32 const_value;

    /** const value valid indication */
    uint8 const_value_valid;

    /** the illegal values within the range on min-max values(applicable only for fields less than 32 bits) */
    int nof_illegal_values;
    uint32 illegal_values[DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES];

    /** offset to take in logical field the first valid bit to be used in table */
    int offset_in_logical_field;

    /**   location (bit offset) in the handle buffer  */
    int bits_offset_in_buffer;

    /**  nof_instances in the table*/
    int nof_instances;

    /** Option to allow valid indication on init */
    uint8 is_valid_indication_needed;

    /** Indication for read only fields */
    dbal_field_permission_e permission;

    /** Indication for sw fields */
    uint8 is_sw_field;

    /** Indication that the field is connected to allocation resource */
    uint8 is_allocator;

    /** Arr prefix - value that will be used to complete the value of the field. in those cases the field in table should
     *  be smaller than the field type size, to complete the full size the arr prefix should be added the value that will
     *  be set for this fields should be according to the full value, but only the affective bit will be set to HW */
    uint8 arr_prefix;

    /** the arr_prefix size that should be used */
    uint8 arr_prefix_size;

    /** Indication for reverse order of instances */
    uint8 is_instances_reverse_order;
} dbal_table_field_info_t;

typedef struct
{
    /** HW ID received from user, this is used for tcam access */
    int entry_hw_id;

    uint8 is_update;

    /*See DBAL_PHYSICAL_KEY_HITBIT_X defines*/
    /** inout parameter for cmd   */
    uint8 hitbit;

    /*See DBAL_MSB_AGE defines*/
    /** inout parameter for mdb aging   */
    uint32 age;

    dbal_mdb_action_apply_type_e mdb_action_apply;
    /** Holds TRUE if the EEDB entry is expected to have LL and FALSE otherwise */
    uint8 eedb_ll;

    uint32 key_size;
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];

    /** Relevant for LPM tables the key prefix length according to k_mask, for KBP TCAM it means the priority */
    uint32 prefix_length;

    uint32 payload_size;
    uint32 payload_offset;
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 p_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
} dbal_physical_entry_t;

typedef struct
{
    uint32 mdb_entry_index;     /* next logical entry index to be retrieved (direct/EEDB table) */
    int mdb_cluster_index;      /* The cluster index, used to iterate through all the clusters associated with a table */
    uint32 payload_basic_size;  /* The payload basic size associated with direct table (except EEDB/IN_LIF), in EEDB
                                 * does not include LL */

    /**
     * As we have iterator usage with rules, not every entry that is found by access layer should be returned
     * In that case, the solution might be an recursive call of iterator_get_next
     * Recursive call might override the stack in very large tables, thus we need to go with iterative solution.
     * This flag is indicating to an higher level that we go back from access with no valid entry, so we need to continue getting next entry.
     */
    uint8 continue_calling_mdb_get_next;

    /*
     * Flush machine iterator info:
     * This information id used to decide whether to iterate via flush machine or sw iterator
     */

    /** indication if user set a rule on the result type */
    uint8 has_rt_rule;

    /** indication if iterator is done via flush (does NOT indicate that ONLY flush machine is used) */
    uint8 iterate_in_flush_machine;
    /** number of rules in flush machine, each rule represents a result type */
    int nof_flush_rules;
    /** Bitmap of result types which are iterated via flush machine */
    uint32 result_types_in_flush_bitmap;

    /** indication if iterator is done via sw iterator (does NOT indicate that ONLY sw iterator is used)  */
	uint8 start_non_flush_iteration;
    /** number of rules in sw iterator, each rule represents a result type */
	int nof_non_flush_rules;
 	/** Bitmap of result types which are iterated via sw iterator */
    uint32 result_types_in_non_flush_bitmap;
    /** hit bit flags if needed in the iterator get requests. */
    uint8 hit_bit_flags;
    /** age flags if needed in the iterator get requests. */
    uint8 age_flags;
#if defined(INCLUDE_KBP)
    mdb_kaps_db_t_p mdb_lpm_db_p;       /* KBPSDK DB pointer */
    mdb_kaps_ad_db_t_p mdb_lpm_ad_db_p; /* KBPSDK AD DB pointer */
    struct kbp_entry_iter *mdb_lpm_iter;        /* KBPSDK iterator pointer */
#endif                          /* defined(INCLUDE_KBP) */

    SW_STATE_HASH_TABLE_ITER mdb_em_htb_iter;     /* Used to iterate over the adapter EM shadow DB. */
    SW_STATE_HASH_TABLE_ITER mdb_em_key_size;
    SW_STATE_HASH_TABLE_ITER mdb_em_payload_size;

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

typedef struct
{
    int result_type_nof_hw_values;

    uint32 result_type_hw_value[DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE];
    /**
     * result type name.
     * used for diagnostics and logging
     */
    char result_type_name[DBAL_MAX_STRING_LENGTH];

    /** check if necessary */
    int entry_payload_size;

    /** Zero padding in buffer, used in MDB multiple result tables */
    int zero_padding;

    /** number of result field  */
    int nof_result_fields;

    /** result field information  */
    dbal_table_field_info_t *results_info;

    /** incase the result type is mapped according to a field */
    dbal_field_types_defs_e refernce_field_id;

    uint8 is_disabled;
} multi_res_info_t;

typedef struct
{
    int todo;
} dbal_table_restrictions_info_t;

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

    /** set to 1 if the table support bulk operations (multiple entries with the same value) currently only memories with
     *  no encoding is supported (MDB,SW,TCAM_CS,KBP - not supported). */
    int range_set_supported;

    /** Interface parameters information regarding the key and result fields  */
    int nof_key_fields;
    dbal_table_field_info_t *keys_info;

    /** the nof bits of the key */
    uint32 key_size;

    /** if core ID is present in the key (in DPC mode), those bits are reduced from the phy_entry buffer */
    uint32 core_id_nof_bits;

    /** indicates if table has the result_type field */
    uint8 has_result_type;
    
    uint8 result_type_mapped_to_sw;

    /** allocator field is used for indication that in get next need to call
     *   resource manager to find the next available index
     */
    dbal_fields_e allocator_field_id;

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

    /** Access Layer Info  */

    int is_hook_active; /** in case that the hook is active the access to the table is done by dedicated functions. hooks are generally used to fix HW issues or support specific HW behaviors */

    /** SW state parameters  */
    int sw_state_nof_entries;
    int sw_payload_length_bytes;
    /** hold the formula for table size indicating the physical dbs according to the capacity of the SW table is set */
    char *table_size_str;

    /** MDB info  */
    int nof_physical_tables;
    dbal_physical_tables_e physical_db_id[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];

    uint32 app_id; /** app_id, also use for kbp_db_id*/
    int app_id_size;
    dbal_mdb_table_image_type_e mdb_image_type;

    /*
     * DBAL stage info for TCAM CS and MDB TCAM.
     * There is one extra "stage" for PRT TCAM (even though it is not a real stage).
     */
    dbal_stage_e dbal_stage;

    /*
     * Hard logic (or HL+SW)
     * Structure is allocated statically because its index is corresponding to the access_typ enum
     * the unify_res_mapping field indicates that all result types are mapped to the same HW place.
     * For disabled result types, the hl_mapping_multi_res is all 0.
     */
    dbal_hl_access_info_t *hl_mapping_multi_res;

    /** Pemla info (also uses app_id)  */
    dbal_pemla_db_mapping_info_t pemla_mapping;

    /** KBP access info (also uses app_id and mdb_image_type)  */
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
     * True if error occurred
     */
    uint8 error_exists;
    /**
     * The field ID related to the error
     */
    dbal_fields_e field_id;
} field_error_info_t;

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
     *  be promoted (if false it is the first time we call to the iterator */
    uint8 used_first_key;

    /** entries found counter */
    uint32 entries_counter;

    /** nof of key rules */
    uint32 nof_key_rules;

    /** key_rules_info */
    dbal_access_condition_info_t *key_rules_info;

    /** nof of value rules */
    uint32 nof_val_rules;

    /** key_rules_info */
    dbal_access_condition_info_t *val_rules_info;

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
    uint8 hitbit_cmd;
    uint8 age_cmd;

    /** Total number of fields  */
    uint8 num_of_fields;

    /**  Not in used, multiple transaction support */
    uint8 trans_id;

    /**  Handle status according to enum */
    dbal_entry_handle_status_e handle_status;

    /**  buffers that contains all the fields values */
    dbal_physical_entry_t phy_entry;

    /** Indicates (to the access layer) if the entry is updated or
     * new one, valid for non-direct tables.
     * If the entry already exists, payload is merged with the
     * existing entry. The access layer only needs to add this entry
     * and remove the old one.
     */
    uint8 is_entry_update;

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

    /**  information about get fields that will be updated after entry get */
    dbal_user_output_info_t user_output_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];

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
    uint32 er_flags;

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
typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_ADD) (
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_GET) (
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_DELETE) (
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry);

typedef shr_error_e(
    *PHYSICAL_TABLE_CLEAR) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
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
    char physical_name[DBAL_MAX_STRING_LENGTH];
    dbal_physical_tables_e physical_db_type;
    dbal_core_mode_e physical_core_mode;
    int nof_entries;
    dbal_physical_table_actions_t *table_actions;

} dbal_physical_table_def_t;

shr_error_e dbal_physical_table_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_def_t ** physical_table);

const char *dbal_physical_table_name_get(
    int unit,
    dbal_physical_tables_e physical_table_id);

typedef struct
{
    dbal_physical_table_def_t physical_tables[DBAL_NOF_PHYSICAL_TABLES];

} dbal_physical_mngr_info_t;

typedef struct
{
    uint32 nof_memories;
    char memory_name[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    char memory_mapped_name[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    uint32 nof_registers;
    char register_name[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    char register_mapped_name[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

} hl_porting_info_t;

typedef struct
{
    char  group_name[DBAL_MAX_STRING_LENGTH];
    uint8 group_hw_entity_num;
    char  hw_entity_name[DBAL_MAX_NUMBER_OF_HW_ELEMENTS][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
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

    /** struct to all porting info relevant for devices after JR2 */
    hl_porting_info_t hl_porting_info;

    dbal_logical_table_t logical_tables[DBAL_NOF_DYNAMIC_AND_STATIC_TABLES];

    /** struct to all hl hw entity groups  */
    hl_groups_info_t groups_info;

} dbal_logical_tables_info_t;

typedef struct
{
    /** Global flag to disable logger prints, currently used under resolt type resolution */
    uint8 disable_logger;

    /** logger - indicate that is locked on specific table, per user request */
    dbal_tables_e user_log_locked_table;

    /** logger - indicate that is locked on specific table, per dbal internal decision */
    dbal_tables_e internal_log_locked_table;

    /** output file for logs, only if exists by property */
    FILE *dbal_file;

} dbal_logger_info_t;

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
    dbal_entry_handle_t entry_handles_pool[DBAL_SW_NOF_ENTRY_HANDLES];
    dbal_iterator_info_t iterators_pool[DBAL_SW_NOF_ENTRY_HANDLES];
    dbal_logger_info_t logger_info;
    dbal_status_e status;
    sal_mutex_t handle_mutex;
} dbal_mngr_info_t;

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

#endif /* DBAL_STRUCTURES_H_INCLUDED */
