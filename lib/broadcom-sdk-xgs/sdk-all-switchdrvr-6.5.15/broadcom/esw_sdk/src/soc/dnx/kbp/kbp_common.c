/*
 * $Id: dbal_kbp_access.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#include <sal/types.h>
#include <soc/dnx/kbp/kbp_common.h>

#if defined(INCLUDE_KBP)

#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KBPDNX
#include <shared/bsl.h>
#include <soc/drv.h> 

const int g_kbp_compiled = 1;

const shr_error_e dnx_kbp_error_translation[KBP_STATUS_LAST_UNUSED] = {
        _SHR_E_NONE,              /* KBP_OK */
        _SHR_E_PARAM,             /* KBP_INVALID_ARGUMENT */
        _SHR_E_MEMORY,            /* KBP_OUT_OF_MEMORY */
        _SHR_E_INTERNAL,          /* KBP_INTERNAL_ERROR */
        _SHR_E_INTERNAL,          /* KBP_FATAL_TRANSPORT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_RANGES */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_FILE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_FORMAT */
        _SHR_E_INTERNAL,          /* KBP_UNALIGNED_RANGE */
        _SHR_E_INTERNAL,          /* KBP_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_DATA_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_MATCH */
        _SHR_E_INTERNAL,          /* KBP_OUT_OF_INDEX */
        _SHR_E_FULL,              /* KBP_OUT_OF_UDA */
        _SHR_E_FULL,              /* KBP_OUT_OF_AD */
        _SHR_E_INTERNAL,          /* KBP_OUT_OF_HITBIT */
        _SHR_E_FULL,              /* KBP_OUT_OF_DBA */
        _SHR_E_FULL,              /* KBP_OUT_OF_UIT */
        _SHR_E_INTERNAL,          /* KBP_LSN_FULL */
        _SHR_E_INTERNAL,          /* KBP_PARSE_ERROR */
        _SHR_E_INTERNAL,          /* KBP_INVALID_SEARCH_SPEC */
        _SHR_E_INTERNAL,          /* KBP_INVALID_TABLE_SPEC */
        _SHR_E_INTERNAL,          /* KBP_KEY_FIELD_MISSING */
        _SHR_E_INTERNAL,          /* KBP_KEY_GRAN_ERROR */
        _SHR_E_INTERNAL,          /* KBP_NUM_SEARCHES_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_NUM_SEARCHES_EXCEEDED_ON_SMT */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_KEY */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_INSTRUCTION_ID */
        _SHR_E_INTERNAL,          /* KBP_LTR_BUSY */
        _SHR_E_INTERNAL,          /* KBP_INVALID_LTR_ID */
        _SHR_E_INTERNAL,          /* KBP_INVALID_CB_ADDRESS */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DEVICE_TYPE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESOURCE_DBA */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESOURCE_UDA */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESOURCE_LSN */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESOURCE_RANGE_UNITS */
        _SHR_E_INTERNAL,          /* KBP_DB_ACTIVE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_KEY */
        _SHR_E_INTERNAL,          /* KBP_INVALID_MASTER_KEY */
        _SHR_E_INTERNAL,          /* KBP_ENTRY_ALREADY_ACTIVE */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_DB_ID */
        _SHR_E_INTERNAL,          /* KBP_KEY_WIDTH_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_KEY_FIELDS */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_AD */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_INSTRUCTIONS */
        _SHR_E_INTERNAL,          /* KBP_UNSUPPORTED_AD_SIZE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_COMPARE3_SCHEDULE */
        _SHR_E_INTERNAL,          /* KBP_ADV_PWR_CTRL_NOT_SUPPORTED_IN_COMPARE3 */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_SEARCHES */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_LPM_SEARCHES */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_ADVANCED_PWR_CTL_SEARCHES */
        _SHR_E_INTERNAL,          /* KBP_RESULT_NOT_VALID */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_AGGREGATE_AD_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_UNLOCKED */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_ALREADY_LOCKED */
        _SHR_E_INTERNAL,          /* KBP_RESOURCE_FIT_FAIL */
        _SHR_E_INTERNAL,          /* KBP_UDA_POWER_BUDGET_SATISFY_FAIL */
        _SHR_E_INTERNAL,          /* KBP_DBA_POWER_BUDGET_SATISFY_FAIL */
        _SHR_E_INTERNAL,          /* KBP_EXCESSIVE_DB_DUPLICATION */
        _SHR_E_INTERNAL,          /* KBP_INVALID_REGISTER_ADDRESS */
        _SHR_E_INTERNAL,          /* KBP_LINK_UP */
        _SHR_E_INTERNAL,          /* KBP_LINK_DOWN */
        _SHR_E_FULL,              /* KBP_EXHAUSTED_PCM_RESOURCE */
        _SHR_E_INTERNAL,          /* KBP_NOT_SAME_THREAD */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_KEY_FIELD */
        _SHR_E_INTERNAL,          /* KBP_INCONSISTENT_TABLE_ID_OFFSET */
        _SHR_E_INTERNAL,          /* KBP_INCONSISTENT_TABLE_ID_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_TABLE_ID */
        _SHR_E_INTERNAL,          /* KBP_MISSING_KEY */
        _SHR_E_INTERNAL,          /* KBP_XPT_REQUEST_ERROR */
        _SHR_E_INTERNAL,          /* KBP_XPT_RESULT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_INVALID_KEY_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RANGE */
        _SHR_E_INTERNAL,          /* KBP_ONE_ADDB_PER_DB */
        _SHR_E_INTERNAL,          /* KBP_AD_ALREADY_ADDED */
        _SHR_E_INTERNAL,          /* KBP_AD_NOT_YET_ADDED */
        _SHR_E_INTERNAL,          /* KBP_AD_ENTRY_ACTIVE */
        _SHR_E_INTERNAL,          /* KBP_AD_DB_ABSENT */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_INTERFACE_RESPONSE_BANDWIDTH */
        _SHR_E_INTERNAL,          /* KBP_MISSING_TABLE_ID_FIELD */
        _SHR_E_INTERNAL,          /* KBP_DYNAMIC_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_TABLE_ID */
        _SHR_E_INTERNAL,          /* KBP_SMT_DISABLED */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_THREAD_ID */
        _SHR_E_INTERNAL,          /* KBP_UNSUPPORTED_THREAD_ID */
        _SHR_E_INTERNAL,          /* KBP_BATCH_MODE_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_ENTRY_MISSING_RANGE */
        _SHR_E_INTERNAL,          /* KBP_INSTRUCTION_ALREADY_INSTALLED */
        _SHR_E_INTERNAL,          /* KBP_KEY_ALREADY_SET */
        _SHR_E_INTERNAL,          /* KBP_INSTRUCTION_NO_DB */
        _SHR_E_INTERNAL,          /* KBP_INSTRUCTION_NOT_INSTALLED */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_NO_INSTRUCTIONS */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_NO_DB */
        _SHR_E_INTERNAL,          /* KBP_MISSING_KEY_FIELD */
        _SHR_E_INTERNAL,          /* KBP_LPM_TERNARY */
        _SHR_E_INTERNAL,          /* KBP_LPM_RANGE */
        _SHR_E_INTERNAL,          /* KBP_LPM_INVALID_KEY_FORMAT */
        _SHR_E_INTERNAL,          /* KBP_MULTIPLE_PREFIX_FIELDS */
        _SHR_E_INTERNAL,          /* KBP_MULTIPLE_TBLID_FIELDS */
        _SHR_E_INTERNAL,          /* KBP_SMT_CONFIG_FAIL */
        _SHR_E_INTERNAL,          /* KBP_LPM_SMT1_UNSUPPORTED */
        _SHR_E_NOT_FOUND,         /* KBP_PREFIX_NOT_FOUND */
        _SHR_E_INTERNAL,          /* KBP_RANGE_ALREADY_SPECIFIED */
        _SHR_E_INTERNAL,          /* KBP_CLONE_KEY_FORMAT_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_SPECIFY_CASCADE_FOR_ALL_DB */
        _SHR_E_INTERNAL,          /* KBP_INVALID_CASCADE_NUM */
        _SHR_E_INTERNAL,          /* KBP_INVALID_INDEX_RANGE */
        _SHR_E_INTERNAL,          /* KBP_CASCADE_FLAG_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_MORE_THAN_TWO_CLONES_IN_SRCH */
        _SHR_E_INTERNAL,          /* KBP_PARENT_CLONE_WIDTH_NOT_SUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_PARENT_CLONE_PARALLEL_SRCH_NOT_SUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DCR_CONFIG_REG */
        _SHR_E_INTERNAL,          /* KBP_NBO_BIT_NOT_ENABLED */
        _SHR_E_INTERNAL,          /* KBP_SMT_BIT_NOT_ENABLED */
        _SHR_E_INTERNAL,          /* KBP_DUAL_PORT_BIT_NOT_ENABLED */
        _SHR_E_INTERNAL,          /* KBP_IDX_SUPPRESSION_BIT_NOT_ENABLED */
        _SHR_E_INTERNAL,          /* KBP_RANGE_BIT_NOT_ENABLED */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_IDENTITY_FAILED */
        _SHR_E_INTERNAL,          /* KBP_SERDES_RESET_FAIL */
        _SHR_E_INTERNAL,          /* KBP_SERDES_POWER_UP_FAIL */
        _SHR_E_INTERNAL,          /* KBP_MDIO_RW_TEST_FAIL */
        _SHR_E_INTERNAL,          /* KBP_CUSTOM_RANGE_WITH_AD */
        _SHR_E_INTERNAL,          /* KBP_CLONE_PARENT_KEY_MISSING */
        _SHR_E_INTERNAL,          /* KBP_ISSU_RESTORE_REQUIRED */
        _SHR_E_INTERNAL,          /* KBP_ISSU_RECONCILE_REQUIRED */
        _SHR_E_INTERNAL,          /* KBP_ISSU_RESTORE_DONE */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_NOT_EMPTY */
        _SHR_E_INTERNAL,          /* KBP_RECONCILE_NOT_STARTED */
        _SHR_E_INTERNAL,          /* KBP_NV_READ_WRITE_FAILED */
        _SHR_E_INTERNAL,          /* KBP_NV_DATA_CORRUPT */
        _SHR_E_INTERNAL,          /* KBP_INVALID_PREFIX_LENGTH */
        _SHR_E_INTERNAL,          /* KBP_ISSU_IN_PROGRESS */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_NO_SKIP_INIT_FLAG */
        _SHR_E_INTERNAL,          /* KBP_INSTRUCTION_BUSY */
        _SHR_E_INTERNAL,          /* KBP_ISSU_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_DB_ALREADY_ADDED_TO_INSTRUCTION */
        _SHR_E_INTERNAL,          /* KBP_ISFR_AB_CANNOT_REPAIR */
        _SHR_E_INTERNAL,          /* KBP_ISFR_ESR_NON_PARITY_ERRORS */
        _SHR_E_INTERNAL,          /* KBP_PWR_MODE_CANNOT_BE_SUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_CONFLICTING_SEARCH_PORTS */
        _SHR_E_INTERNAL,          /* KBP_OUT_OF_BMRS */
        _SHR_E_INTERNAL,          /* KBP_INVALID_LTR_NUMBER */
        _SHR_E_INTERNAL,          /* KBP_INVALID_SLTR_NUMBER */
        _SHR_E_INTERNAL,          /* KBP_TABLE_ID_AS_HOLE_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_RANGE_FIELDS_AS_HOLE_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_LPM_KEY_WITH_HOLE_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_PENDING_ENTRY */
        _SHR_E_INTERNAL,          /* KBP_HW_SHADOW_DATA_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_INVALID_AD_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_PARALLEL_SEARCHES */
        _SHR_E_INTERNAL,          /* KBP_WR_REG_ERROR */
        _SHR_E_INTERNAL,          /* KBP_WR_DBA_ERROR */
        _SHR_E_INTERNAL,          /* KBP_WR_UDA_ERROR */
        _SHR_E_INTERNAL,          /* KBP_READ_REG_ERROR */
        _SHR_E_INTERNAL,          /* KBP_READ_DBA_ERROR */
        _SHR_E_INTERNAL,          /* KBP_READ_UDA_ERROR */
        _SHR_E_INTERNAL,          /* KBP_SEARCH_ERROR */
        _SHR_E_INTERNAL,          /* KBP_VALID_BIT_NOT_MATCH */
        _SHR_E_INTERNAL,          /* KBP_PARITY_NOT_MATCH */
        _SHR_E_INTERNAL,          /* KBP_DATA_NOT_MATCH */
        _SHR_E_INTERNAL,          /* KBP_NO_RESP */
        _SHR_E_INTERNAL,          /* KBP_PARITY_ERROR */
        _SHR_E_INTERNAL,          /* KBP_POLL_TIME_OUT */
        _SHR_E_INTERNAL,          /* KBP_UNSUPPORTED_DEVICE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DEVICE_PTR */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DEVICE_DESC_PTR */
        _SHR_E_INTERNAL,          /* KBP_INVALID_FLAGS */
        _SHR_E_INTERNAL,          /* KBP_INVALID_KEY_TYPE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DEVICE_COUNT */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DB_TYPE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_INSTRUCTION */
        _SHR_E_INTERNAL,          /* KBP_INVALID_AB_NUMBER */
        _SHR_E_INTERNAL,          /* KBP_INVALID_META_LEN */
        _SHR_E_INTERNAL,          /* KBP_INVALID_BURST_RATE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_SERDES_SPEED */
        _SHR_E_INTERNAL,          /* KBP_INVALID_LTR_REG_TYPE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_SLTR_REG_TYPE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_LANE_VALUE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DUAL_PORT_LANE_VALUE */
        _SHR_E_INTERNAL,          /* KBP_ADD_DEL_ENTRY_TO_CLONE_DB */
        _SHR_E_INTERNAL,          /* KBP_AB_NOT_FOUND */
        _SHR_E_INTERNAL,          /* KBP_UNIT_NOT_FOUND */
        _SHR_E_INTERNAL,          /* KBP_INVALID_NUM_LANE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_START_LANE */
        _SHR_E_INTERNAL,          /* KBP_MEM_BIST_TOO_MANY_PARITY */
        _SHR_E_INTERNAL,          /* KBP_EXCEED_MAX_LANES */
        _SHR_E_INTERNAL,          /* KBP_UNSUPPORTED_DMA_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_DB_NOT_ASSOCIATED */
        _SHR_E_INTERNAL,          /* KBP_EXHAUSTED_KPU_RESOURCE */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_AD_BYTES */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_MAX_AD_BYTES_SMT */
        _SHR_E_INTERNAL,          /* KBP_SMT_RESULT_PORT_FIT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_DRIVER_BUSY */
        _SHR_E_INTERNAL,          /* KBP_CONTEXT_ID_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_DB_HANDLE_NOT_FOUND */
        _SHR_E_INTERNAL,          /* KBP_DB_ENTRY_HANDLE_NOT_FOUND */
        _SHR_E_INTERNAL,          /* KBP_COMPLEX_INSTR_ERROR */
        _SHR_E_INTERNAL,          /* KBP_INVALID_CI_OPCODE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_INSTR_OPCODE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESOURCE_BMP */
        _SHR_E_INTERNAL,          /* KBP_INVALID_DBA_BMP */
        _SHR_E_INTERNAL,          /* KBP_INVALID_UDA_BMP */
        _SHR_E_INTERNAL,          /* KBP_INVALID_USB_BMP */
        _SHR_E_INTERNAL,          /* KBP_RESOLUTION_NOT_POSSIBLE */
        _SHR_E_INTERNAL,          /* KBP_EXHAUSTED_DBA_PARALLEL_RESOURCES */
        _SHR_E_INTERNAL,          /* KBP_INVALID_HANDLE */
        _SHR_E_INTERNAL,          /* KBP_INVALID_META_PRIORITY */
        _SHR_E_INTERNAL,          /* KBP_INVALID_PRIORITY */
        _SHR_E_INTERNAL,          /* KBP_PARITY_FIFO_LIMIT_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_PARITY_FIFO_ENTRY_INVALID_IN_LIMIT */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_INDIRECTION */
        _SHR_E_INTERNAL,          /* KBP_WIDE_ACL_CONCURRENT_SEARCH_NOT_SUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_RESPONSE_OUT_OF_ORDER */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_HB */
        _SHR_E_INTERNAL,          /* KBP_ONE_HB_DB_PER_DB */
        _SHR_E_INTERNAL,          /* KBP_NEED_AD_FOR_HB */
        _SHR_E_INTERNAL,          /* KBP_HB_DB_ABSENT */
        _SHR_E_INTERNAL,          /* KBP_HB_NO_AD */
        _SHR_E_FULL,              /* KBP_DB_FULL */
        _SHR_E_INTERNAL,          /* KBP_DB_BROADCAST_AT_LOWER_INDEX_DEV */
        _SHR_E_INTERNAL,          /* KBP_DB_BROADCAST_LPM_NOT_IN_ALL_DEVICES */
        _SHR_E_INTERNAL,          /* KBP_TABLES_CREATION_NOT_ALLOWED */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_HALF_DBS */
        _SHR_E_INTERNAL,          /* KBP_DB_ALREADY_PAIRED */
        _SHR_E_INTERNAL,          /* KBP_DB_PAIRING_NOT_POSSIBLE */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_AGGREGATE_HALF_DB_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_DB_HAS_NO_HB */
        _SHR_E_INTERNAL,          /* KBP_EXHAUSTED_NV_MEMORY */
        _SHR_E_INTERNAL,          /* KBP_DUMMY_FILL_FIELD_UNSUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESET_MODE */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_DBS */
        _SHR_E_INTERNAL,          /* KBP_DB_SCALE_UP_NOT_POSSIBLE */
        _SHR_E_INTERNAL,          /* KBP_DB_SCALE_UP_AD_IS_MUST */
        _SHR_E_INTERNAL,          /* KBP_DB_SCALE_UP_EXCEEDED_MAX_AD_BYTES */
        _SHR_E_INTERNAL,          /* KBP_DB_PAIRING_SCALE_UP_NOT_POSSIBLE */
        _SHR_E_INTERNAL,          /* KBP_CRC_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_WRITE_ONLY_PROPERTY */
        _SHR_E_INTERNAL,          /* KBP_READ_ONLY_PROPERTY */
        _SHR_E_INTERNAL,          /* KBP_SERDES_FW_LOAD_FAIL */
        _SHR_E_INTERNAL,          /* KBP_EXCEED_MAX_INJECT_LOCATIONS */
        _SHR_E_INTERNAL,          /* KBP_ROP_RESPONSE_ERROR */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_NUM_DB */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_NUM_AD_DB */
        _SHR_E_INTERNAL,          /* KBP_EXCEEDED_NUM_HB_DB */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESULT_LENGTH */
        _SHR_E_INTERNAL,          /* KBP_INVALID_RESULT_ID */
        _SHR_E_INTERNAL,          /* KBP_WRONG_BITS_WRITE */
        _SHR_E_INTERNAL,          /* KBP_STATS_WRONG_VSF_TYPE */
        _SHR_E_INTERNAL,          /* KBP_STATS_WRONG_STATS_RECORD_LENGTH */
        _SHR_E_INTERNAL,          /* KBP_STATS_WRONG_STATS_POINTER_LENGTH */
        _SHR_E_INTERNAL,          /* KBP_STATS_GROUP_VSF_TYPES_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_STATS_INST_DB_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_STATS_ALREADY_ADDED */
        _SHR_E_INTERNAL,          /* KBP_STATS_NOT_YET_ADDED */
        _SHR_E_INTERNAL,          /* KBP_STATS_ENTRY_ACTIVE */
        _SHR_E_INTERNAL,          /* KBP_STATS_DB_ABSENT */
        _SHR_E_INTERNAL,          /* KBP_STATS_FAILED_TRANSLATION */
        _SHR_E_INTERNAL,          /* KBP_STATS_NULL_RECORD */
        _SHR_E_INTERNAL,          /* KBP_STATS_OUT_OF_INDEX_TRANS_RESOURCE */
        _SHR_E_INTERNAL,          /* KBP_TOO_MANY_COUNTER_DBS */
        _SHR_E_INTERNAL,          /* KBP_ONE_COUNTER_DB_PER_DB */
        _SHR_E_INTERNAL,          /* KBP_STATS_INVALID_STATS_RECORD_TYPE */
        _SHR_E_INTERNAL,          /* KBP_STATS_ERROR_STATS_CNTR_REVERSE_ADDR_XLAT */
        _SHR_E_INTERNAL,          /* KBP_COUNTER_DB_ABSENT */
        _SHR_E_INTERNAL,          /* KBP_WR_STAT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_RD_STAT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_STAT_COUNT_ERROR */
        _SHR_E_INTERNAL,          /* KBP_COUNTER_REFRESH_IN_PROGRESS */
        _SHR_E_INTERNAL,          /* KBP_STATS_FAILED_HOST_EVICTION */
        _SHR_E_INTERNAL,          /* KBP_OP2_MP_NOT_SUPPORTED */
        _SHR_E_INTERNAL,          /* KBP_INVALID_BB_CONFIG */
        _SHR_E_INTERNAL,          /* KBP_STATS_INVALID_FIELD_WIDTH */
        _SHR_E_INTERNAL,          /* KBP_STATS_INVALID_FIELD_TYPE */
        _SHR_E_INTERNAL,          /* KBP_DEVICE_FW_NOT_LOADED */
        _SHR_E_INTERNAL,          /* KBP_DUPLICATE_RECORD_FIELD */
        _SHR_E_INTERNAL,          /* KBP_DB_WB_LOCATION_BUSY */
        _SHR_E_INTERNAL,          /* KBP_DB_NO_COUNTERS */
        _SHR_E_INTERNAL,          /* KBP_DB_WRONG_ACCESS */
        _SHR_E_INTERNAL,          /* KBP_OP_TAP_INVALID_COMMAND */
        _SHR_E_INTERNAL,          /* KBP_OP_TAP_INVALID_ARGS */
        _SHR_E_INTERNAL,          /* KBP_INVALID_FEC_FRAME_LEN */
        _SHR_E_INTERNAL,          /* KBP_OP_PKT_BIST_MISMATCH_ERROR */
        _SHR_E_INTERNAL,          /* KBP_OP_PKT_BIST_DID_NOT_RUN */
        _SHR_E_INTERNAL,          /* KBP_OP_TAE_BIST_MISMATCH_FAILED */
        _SHR_E_INTERNAL,          /* KBP_OP_TAE_BIST_DID_NOT_RUN_FAILED */
        _SHR_E_INTERNAL,          /* KBP_OP_TAE_BIST_INCORRECT_NUM_LOOPS */
        _SHR_E_INTERNAL,          /* KBP_TAP_NUM_INSTRUCTIONS_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_DUPLICATE_INSTRUCTION_ID */
        _SHR_E_INTERNAL,          /* KBP_TAP_INSTRUCTION_RECORD_ALREADY_EXIST */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_LENGTH_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_RECORD_SIZE */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_SIZE_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_RECORD_TYPE */
        _SHR_E_INTERNAL,          /* KBP_TAP_INGRESS_REC_SIZE_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_TAP_EGRESS_REC_SIZE_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_OPCODE_EXT_START_LOC */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_OPCODE_EXT_LEN */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_OPCODE_EXT_VALUE */
        _SHR_E_INTERNAL,          /* KBP_TAP_DUPLICATE_OPCODE_EXT_VALUE */
        _SHR_E_INTERNAL,          /* KBP_TAP_MISSING_OPCODE_EXT_MAP */
        _SHR_E_INTERNAL,          /* KBP_TAP_OPCODE_EXT_LEN_NOT_SET */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_SET_SIZE */
        _SHR_E_INTERNAL,          /* KBP_TAP_NO_COUNTER_SCRUB_THREAD */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_DB_PTR_FIELD_MISSING */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_DB_ATTR_FIELD_MISSING */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_OPCODE_EXT_LEN_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_OPCODE_EXT_START_LOC_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_TAP_RECORD_FIELD_ALREADY_ADDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_DUPLICATE_RECORD_FIELD */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_RECORD_FIELD_TYPE */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_RECORD_OPCODE_LEN */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_RECORD_BYTE_COUNT_LEN */
        _SHR_E_INTERNAL,          /* KBP_TAP_MAX_DB_LIMIT_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_POINTER_LENGTH_INSUFFICIENT */
        _SHR_E_INTERNAL,          /* KBP_TAP_NUM_POINTER_FIELDS_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_MAX_POINTER_LENGTH_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_DUPLICATE_POINTER_FIELD */
        _SHR_E_INTERNAL,          /* KBP_TAP_POINTER_ALREADY_ADDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_NUM_ATTR_FIELDS_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_MAX_ATTR_LENGTH_EXCEEDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_DUPLICATE_ATTR_FIELD */
        _SHR_E_INTERNAL,          /* KBP_TAP_ATTRIBUTE_ALREADY_ADDED */
        _SHR_E_INTERNAL,          /* KBP_TAP_ATTRIBUTE_ABSENT */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_ATTR_VALUE */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_COUNTER_OFFSET_VALUE */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_ENTRY_NUMBER */
        _SHR_E_INTERNAL,          /* KBP_OP_TAP_WRONG_XPT */
        _SHR_E_INTERNAL,          /* KBP_INI_SRCH_TAE_FAILED */
        _SHR_E_INTERNAL,          /* KBP_TAP_COUNTER_VALUE_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_TAP_BULK_READ_COMPLETION_ERROR */
        _SHR_E_INTERNAL,          /* KBP_TAP_EVICTION_DATA_ERROR */
        _SHR_E_INTERNAL,          /* KBP_TAP_UNEXPECTED_DYNAMIC_EVICTION_ERROR */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_COUNTER_TYPE_DYNAMIC_EVICTION */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_HOST_ADDR_IN_DYNAMIC_EVICTIONS */
        _SHR_E_INTERNAL,          /* KBP_TAP_INVALID_HOST_ADDR_IN_BULKREAD_EVICTIONS */
        _SHR_E_INTERNAL,          /* KBP_TAP_BULKREAD_DYNAMIC_ERRORS */
        _SHR_E_INTERNAL,          /* KBP_TAP_FRAMEDROP */
        _SHR_E_INTERNAL,          /* KBP_TAP_PARTIAL_FRAMEDROP */
        _SHR_E_INTERNAL,          /* KBP_TAP_FRAME_COUNT_MISMATCH */
        _SHR_E_INTERNAL,          /* KBP_TAP_NO_FRAME_RECEIVED */
        _SHR_E_INTERNAL,          /* KBP_TAP_COUNTER_VALUE_TRUNCATED */
        _SHR_E_INTERNAL,          /* KBP_TAP_TAE_BIST_FAILED */
        _SHR_E_INTERNAL,          /* KBP_TAP_TAE_BIST_DID_NOT_RUN_FAILED */
        _SHR_E_INTERNAL           /* KBP_TAP_TAE_BIST_INCORRECT_NUM_LOOPS */
};

/**
* See kbp_common.h
*/
uint32
dnx_kbp_calculate_prefix_length(
    uint32 *payload_mask,
    uint32 max_key_size_in_bits)
{
    int uint32_index;
    uint32 max_key_size_in_uint32 = BITS2WORDS(max_key_size_in_bits);
    uint32 prefix_length = 0;

    for (uint32_index = max_key_size_in_uint32 - 1; uint32_index >= 0; uint32_index--)
    {
        /*
         * If the prefix is full or if this is the MSB, ignore the prefix MSB masking
         */
        if (payload_mask[uint32_index] == SAL_UINT32_MAX)
        {
            prefix_length += SAL_UINT32_NOF_BITS;
        }
        else
        {
            prefix_length += SAL_UINT32_NOF_BITS - (utilex_msb_bit_on(~payload_mask[uint32_index]) + 1);
            break;
        }
    }

    return prefix_length;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_prefix_len_to_mask(
    int unit,
    int prefix_length,
    uint32 max_key_size_in_bits,
    dbal_physical_entry_t * entry)
{
    uint32 max_key_size_in_uint32 = BITS2WORDS(max_key_size_in_bits);
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry->k_mask, 0x0, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS * sizeof(entry->k_mask[0]));

    if ((prefix_length >= 0) && (prefix_length <= max_key_size_in_bits))
    {
        int ii;
        for (ii = 0; ii < max_key_size_in_uint32; ii++)
        {
            if (prefix_length > ii * SAL_UINT32_NOF_BITS)
            {
                int local_prefix_length = prefix_length - ii * SAL_UINT32_NOF_BITS;
                entry->k_mask[max_key_size_in_uint32 - 1 - ii] =
                    _shr_ip_mask_create(local_prefix_length >
                                        SAL_UINT32_NOF_BITS ? SAL_UINT32_NOF_BITS : local_prefix_length);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Error: Prefix must be between 0 and %d, prefix given: %d.\n",
                     max_key_size_in_bits, prefix_length);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_buffer_dbal_to_kbp(
    int unit,
    uint32 nof_bits,
    uint32 *data_in,
    uint8 *data_out)
{
    uint32 nof_bytes = BITS2BYTES(nof_bits);
    int32 byte_index;
    int byte_counter = 0;
    int32 uint32_index;
    uint8 align_bits = BYTES2BITS(nof_bytes) - nof_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    /** Encode the DBAL data to KBPSDK data */
    byte_index = nof_bytes;
    uint32_index = 0;
    while (byte_index--)
    {
        int data_offset = (((nof_bytes - byte_index - 1) % 4) * 8);
        byte_counter++;
        data_out[byte_index] = (data_in[uint32_index] >> data_offset) & 0xFF;
        if ((byte_counter) % 4 == 0)
        {
            uint32_index++;
        }
    }

    /** Perform MSB alignment, if needed */
    if (align_bits != 0)
    {
        uint8 data_remainder = 0x00;
        byte_index = nof_bytes;
        while (byte_index--)
        {
            uint8 tmp_data_out = (data_out[byte_index] << align_bits) | data_remainder;
            data_remainder = data_out[byte_index] >> (8 - align_bits);
            data_out[byte_index] = tmp_data_out;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_buffer_kbp_to_dbal(
    int unit,
    uint32 nof_bits,
    uint8 *data_in,
    uint32 *data_out)
{
    uint32 nof_bytes = BITS2BYTES(nof_bits);
    uint32 nof_uint32 = BITS2WORDS(nof_bits);
    int32 byte_index;
    int32 uint32_index;
    int byte_counter = 0;
    uint8 align_bits = BYTES2BITS(nof_bytes) - nof_bits;
    uint32 tmp_data_out;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    /** Encode the KBPSDK data to DBAL data */
    byte_index = nof_bytes;
    uint32_index = 0;
    data_out[uint32_index] = 0x00000000;

    while (byte_index--)
    {
        int data_offset = (((nof_bytes - byte_index - 1) % 4) * 8);
        byte_counter++;
        tmp_data_out = data_in[byte_index] << (data_offset);
        data_out[uint32_index] |= tmp_data_out;

        if ((byte_counter) % 4 == 0)
        {
            uint32_index++;
            data_out[uint32_index] = 0x0;
        }
    }

    /** Revert previous MSB alignment, if needed */
    if (align_bits != 0)
    {
        uint32 data_remainder = 0x00000000;
        uint32_index = nof_uint32;
        while (uint32_index--)
        {
            tmp_data_out = (data_out[uint32_index] >> align_bits) | data_remainder;
            data_remainder = (data_out[uint32_index] << (32 - align_bits));
            data_out[uint32_index] = tmp_data_out;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_entry_lpm_get(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    uint8 *hitbit)
{
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    struct kbp_hb *hb_entry = NULL;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    uint32 payload_size_in_bytes = BITS2BYTES(payload_size_in_bits);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, key_size_in_bits, key, data));

    /** Retrieve the db_entry */
    kbp_db_get_prefix_handle(db_p, data, prefix_length, &db_entry);
    if (db_entry == NULL)
    {
        /** Exit without error print. */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    DNX_KBP_TRY(kbp_entry_get_ad(db_p, db_entry, &ad_entry));
    if (ad_entry == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_entry_get_ad() Failed\n");
    }

    /** Retrieve the hb_entry if necessary */
    if (*hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
    {
        /** If hitbit is not configured for this entry, hb_entry will be NULL and the hitbit won't be retrieved  */
        DNX_KBP_TRY(kbp_entry_get_hb(db_p, db_entry, &hb_entry));
    }

    sal_memset(asso_data, 0x0, sizeof(asso_data[0]) * payload_size_in_bytes);

    DNX_KBP_TRY(kbp_ad_db_get(ad_db_p, ad_entry, asso_data));

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, payload_size_in_bits, asso_data, payload));

    if ((hb_entry != NULL) && (*hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION))
    {
        uint8 clear;
        uint32 hitbit_val = 0;
        if (*hitbit & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
        {
            clear = 1;
        }
        else
        {
            clear = 0;
        }
        DNX_KBP_TRY(kbp_hb_entry_get_bit_value(hb_db_p, hb_entry, &hitbit_val, clear));

        /** Set/reset the hitbit accessed indication accordingly */
        if (hitbit_val != 0)
        {
            *hitbit |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED;
        }
        else
        {
            *hitbit &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_entry_lpm_add(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    uint8 is_for_update,
    uint8 hitbit,
    uint8 is_default_entry)
{
    int res;
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    struct kbp_hb *hb_entry = NULL;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, key_size_in_bits, key, data));
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, payload_size_in_bits, payload, asso_data));

    /** Verify that the update flag is used properly. */
    kbp_db_get_prefix_handle(db_p, data, prefix_length, &db_entry);
    if ((db_entry == NULL) && (is_for_update == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The update flag was used on an entry that doesn't exists.\n");
    }
    else if ((db_entry != NULL) && (is_for_update == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "This entry already exists, to modify it the update flag should be used.\n");
    }

    if (is_for_update)
    {
        /** Update the entry */
        DNX_KBP_TRY(kbp_entry_get_ad(db_p, db_entry, &ad_entry));
        DNX_KBP_TRY(kbp_ad_db_update_entry(ad_db_p, ad_entry, asso_data));
    }
    else
    {
        /** Add the entry */
        DNX_KBP_TRY(kbp_db_add_prefix(db_p, data, prefix_length, &db_entry));

        if (is_default_entry)
        {
            enum kbp_db_entry_meta_priority db_meta_priority;
            /** Get the default DB entry meta priority */
            DNX_KBP_TRY(kbp_db_get_property(db_p, KBP_PROP_ENTRY_META_PRIORITY, &db_meta_priority));
            /** Set the default entry meta priority */
            DNX_KBP_TRY(kbp_entry_set_property(db_p, db_entry, KBP_ENTRY_PROP_META_PRIORITY, KBP_ENTRY_META_PRIORITY_2, db_meta_priority));
        }

        res = kbp_ad_db_add_entry(ad_db_p, asso_data, &ad_entry);
        if (res != KBP_OK)
        {
            
            kbp_db_delete_entry(db_p, db_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_ad_db_add_entry Failed\n");
        }

        res = kbp_entry_add_ad(db_p, db_entry, ad_entry);
        if (res != KBP_OK)
        {
            
            kbp_db_delete_entry(db_p, db_entry);
            kbp_ad_db_delete_entry(ad_db_p, ad_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_entry_add_ad Failed\n");
        }

        if (hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
        {
            res = kbp_hb_db_add_entry(hb_db_p, &hb_entry);
            if (res != KBP_OK)
            {
                
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_hb_db_add_entry Failed\n");
            }
            res = kbp_entry_add_hb(db_p, db_entry, hb_entry);
            if (res != KBP_OK)
            {
                
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);
                kbp_hb_db_delete_entry(hb_db_p, hb_entry);
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_entry_add_hb Failed\n");
            }
        }
    }

    

    res = kbp_db_install(db_p);
    if (res != KBP_OK)
    {
        
        kbp_db_delete_entry(db_p, db_entry);
        kbp_ad_db_delete_entry(ad_db_p, ad_entry);
        if (hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
        {
            kbp_hb_db_delete_entry(hb_db_p, hb_entry);
        }

        if (res == KBP_OUT_OF_DBA || res == KBP_OUT_OF_UIT || res == KBP_OUT_OF_UDA || res == KBP_OUT_OF_AD
            || res == KBP_EXHAUSTED_PCM_RESOURCE)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, " Error: Table is full, %s\n", kbp_get_status_string(res));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_db_install failed with: %s.\n", kbp_get_status_string(res));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_entry_lpm_delete(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 prefix_length)
{
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    struct kbp_hb *hb_entry = NULL;
    uint8 data[DBAL_PHYSICAL_KEY_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, key_size_in_bits, key, data));

    /** Retrieve the db_entry */
    DNX_KBP_TRY(kbp_db_get_prefix_handle(db_p, data, prefix_length, &db_entry));
    if (db_entry == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found.");
    }

    /** Retrieve the ad_entry */
    DNX_KBP_TRY(kbp_entry_get_ad(db_p, db_entry, &ad_entry));
    if (ad_entry == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: kbp_entry_get_ad() Failed\n");
    }

    /** Always attempt to delete the hb_entry*/
    if (hb_db_p != NULL)
    {
        DNX_KBP_TRY(kbp_entry_get_hb(db_p, db_entry, &hb_entry));
    }

    DNX_KBP_TRY(kbp_db_delete_entry(db_p, db_entry));
    DNX_KBP_TRY(kbp_ad_db_delete_entry(ad_db_p, ad_entry));
    if (hb_entry != NULL)
    {
        DNX_KBP_TRY(kbp_hb_db_delete_entry(hb_db_p, hb_entry));
    }

    

    DNX_KBP_TRY(kbp_db_install(db_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
uint8
dnx_kbp_device_enabled(
    int unit)
{
    return (SAL_BOOT_PLISIM
            || (!SAL_BOOT_PLISIM
                && ((dnx_data_elk.general.device_type_get(unit) == DNX_KBP_DEV_TYPE_BCM52311) ||
                    (dnx_data_elk.general.device_type_get(unit) == DNX_KBP_DEV_TYPE_BCM52321)))) ? TRUE : FALSE;
}

/**
* See kbp_common.h
*/
uint8
dnx_kbp_interface_enabled(
    int unit)
{
    bcm_pbmp_t port_pbmp;
    shr_error_e res;
    /*
     * Get all the ELK ports for given core.
     */
    res = (dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &port_pbmp));
    if (SHR_FAILURE(res))
    {
        return FALSE;
    }
    else
    {
        return (!SAL_BOOT_PLISIM && !dnx_data_elk.connectivity.feature_get(unit, dnx_data_elk_connectivity_blackhole)
              && BCM_PBMP_NOT_NULL(port_pbmp))
            ? TRUE : FALSE;
    }
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_valid_key_width(
    int unit,
    uint32 width,
    uint32 *valid_width)
{
    SHR_FUNC_INIT_VARS(unit);

    if (width == 0)
    {
        *valid_width = 0;
    }
    else if (width <= 80)
    {
        *valid_width = 80;
    }
    else if (width <= 160)
    {
        *valid_width = 160;
    }
    else if (width <= 320)
    {
        *valid_width = 320;
    }
    else if (width <= 640)
    {
        *valid_width = 640;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid KBP key width %d. Key cannot be bigger than 640\n", width);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
shr_error_e
dnx_kbp_valid_result_width(
    int unit,
    uint32 width,
    uint32 *valid_width)
{
    SHR_FUNC_INIT_VARS(unit);

    if (width == 0)
    {
        *valid_width = 0;
    }
    else if (width <= 24)
    {
        *valid_width = 24;
    }
    else if (width <= 32)
    {
        *valid_width = 32;
    }
    else if (width <= 48)
    {
        *valid_width = 48;
    }
    else if (width <= 64)
    {
        *valid_width = 64;
    }
    else if (width <= 128)
    {
        *valid_width = 128;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid KBP result width %d. Result cannot be bigger than 128\n", width);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_common.h
*/
int
dnx_kbp_file_read_func(
    void *handle,
    uint8 * buffer,
    uint32 size,
    uint32 offset)
{
    size_t result;

    if (!handle)
    {
        return SOC_E_FAIL;
    }

    if (0 != fseek(handle, offset, SEEK_SET))
    {
        return SOC_E_FAIL;
    }

    result = fread(buffer, 1, size, handle);
    if (result < size)
    {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

/**
* See kbp_common.h
*/
int
dnx_kbp_file_write_func(
    void *handle,
    uint8 * buffer,
    uint32 size,
    uint32 offset)
{
    size_t result;

    if (!handle)
    {
        return SOC_E_UNIT;
    }

    if (0 != fseek(handle, offset, SEEK_SET))
    {
        return SOC_E_FAIL;
    }

    result = fwrite(buffer, 1, size, handle);
    if (result != size)
    {
        return SOC_E_MEMORY;
    }
    fflush(handle);

    return SOC_E_NONE;
}

#else

/* *INDENT-OFF* */
const int g_kbp_compiled = 0;
uint8 dnx_kbp_device_enabled(int unit) { return 0; }
uint8 dnx_kbp_interface_enabled(int unit) { return 0; }
shr_error_e dnx_kbp_valid_key_width(int unit, uint32 width, uint32 *valid_width) { return 0; }
shr_error_e dnx_kbp_valid_result_width(int unit, uint32 width, uint32 *valid_width) { return 0; }
int dnx_kbp_file_read_func(void *handle, uint8 * buffer, uint32 size, uint32 offset) { return 0; }
int dnx_kbp_file_write_func(void *handle, uint8 * buffer, uint32 size, uint32 offset) { return 0; }
/* *INDENT-ON* */

#endif /* defined(INCLUDE_KBP) */
