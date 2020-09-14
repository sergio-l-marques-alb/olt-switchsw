/** \file hbmc_firmware.h
 * 
 * DNX hbmc firmware internal APIs to be used in hbmc module and diagnostics.
 * 
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_HBMC_HBMC_FIRMWARE_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_DRAM_HBMC_HBMC_FIRMWARE_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

/*
 * Run firmware operation on all channels
 */
#define DNX_HBMC_FIRMWARE_ALL_CHANNELS (-1)

/**
 * \brief
 *   HBMC firmware interrupt codes.
 *
 *   The HBM firmware functionality is controlled by interrupts issued
 *   to the Spico microprocessor using SBus commands.
 *   This enum contains the interrupt codes we can use.
 */
typedef enum
{
    /**
     * Get firmware revision.
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_REVISION = 0x0,
    /**
     * Get firmware build id.
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_BUILD_ID = 0x1,
    /**
     * Run operation on all channels.
     * Use this interrupt code to launch operations that run in test mode.
     * The corresponding input data:
     * data[7:0] - operation number to perform (see dnx_hbmc_firmware_operation_e).
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_ALL_CHANNELS = 0x30,
    /**
     * Run operation on a single channel.
     * Use this interrupt code to launch operations that run in mission mode.
     * The corresponding input data:
     * data[7:0] - operation number to perform (see dnx_hbmc_firmware_operation_e).
     * data[15:8] - channel to run.
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_A_SINGLE_CHANNEL = 0x31,
    /**
     * Get the result of the operation.
     * The corresponding input data:
     * data - result type to read (see dnx_hbmc_firmware_operation_result_type_e).
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT = 0x32,
    /**
     * Get the current value of a HBM param.
     * The corresponding input data:
     * data - parameter offset to read (see dnx_hbmc_firmware_param_e).
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_PARAMETER_VALUE = 0x33,
    /**
     * Setting an HBM param requires two interrupts.
     * First, this interrupt specifies the param to program.
     * Then, interrupt DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER_VALUE
     * must specify the parameter value and save it to memory.
     * The corresponding input data:
     * data - parameter offset to write (see dnx_hbmc_firmware_param_e).
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER = 0x34,
    /**
     * This interrupt is a part of two-step process to program a param value.
     * First, interrupt DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER
     * must specify the parameter.
     * Then, this interrupt can specify the param value and save the value to memory.
     * The corresponding input data:
     * data - parameter value to write.
     */
    DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER_VALUE = 0x35
} dnx_hbmc_firmware_interrupt_code_e;

/**
 * \brief
 *   HBMC firmware operations.
 *   Certain operations can be run in mission mode, while others must be
 *   executed while in test mode.
 *
 *   Note that Test Mode operations will perform a PHY/HBM reset prior
 *   to running to ensure the device is in a known good state prior to
 *   testing.
 *
 *   Can be run through the following interrupt codes:
 *     DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_ALL_CHANNELS
 *     DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_A_SINGLE_CHANNEL (only mission mode operations)
 *
 *   Details can be found in HBM PHY Firmware Programming guide.
 */
typedef enum
{
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_AND_HBM = 0x0,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_FIFO_POINTERS = 0x1,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESET_HBM = 0x2,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_POWER_ON_FLOW = 0x5,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_BYPASS_TEST = 0x7,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_ID = 0x8,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AWORD_TEST = 0x9,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AERR_TEST = 0xa,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DWORD_TEST = 0xb,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DERR_TEST = 0xc,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_LANE_REPAIR = 0xd,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AWORD_ILB_TEST = 0xe,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DWORD_ILB_TEST = 0xf,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_TEMP = 0x10,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_BURN_HARD_LANE_REPAIRS = 0x11,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_APPLY_HARD_LANE_REPAIRS_TO_PHY = 0x12,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RUN_SAMSUNG_MBIST = 0x13,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RUN_SK_HYNIX_MBIST = 0x14,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESET_MODE_REGISTERS = 0x15,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_CONFIG = 0x16,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AWORD_SLB_TEST = 0x17,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DWORD_SLB_TEST = 0x18,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_CHIPPING_TEST = 0x19,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RESERVED = 0x1a,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AERR_ILB_TEST = 0x1b,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_AERR_SLB_TEST = 0x1c,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DERR_ILB_TEST = 0x1d,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_DERR_SLB_TEST = 0x1e,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_RUN_MMT = 0x20,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_LANE_VERIFY = 0x21,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_START_MMT = 0x22,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_STOP_MMT = 0x23,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_LOAD_TMRS_CODE = 0x24,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_CATTRIP_TEST = 0x26,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_RUN_CELL_REPAIR_MODE = 0x29,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_RUN_FUSE_SCAN = 0x2a,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_LANE_REPAIRS = 0x2c,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_NEW_LANE_REPAIRS = 0x2d,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_SKH_TM = 0x2e,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_REAPPLY_CELL_REPAIRS = 0x2f,
    /** Mission Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_REPAIR_LANE_REPAIRS_BY_MODE = 0x30,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_BEGIN_COLLECTING_LANE_FAILURES = 0x32,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_CONVERT_LANE_FAILURES_TO_SOFT_REPAIRS = 0x33,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_COMP_LANE_REPAIR = 0x34,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_DWORD_WRITE = 0x35,
    /** Test Mode Operation */
    DNX_HBMC_FIRMWARE_OPERATION_HBM_DWORD_READ = 0x36
} dnx_hbmc_firmware_operation_e;

/**
 * \brief
 *   HBMC firmware params.
 *   Can be accessed through the following interrupt codes:
 *     DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_PARAMETER_VALUE
 *     DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER
 *     DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER_VALUE
 *
 *   Details can be found in HBM PHY Firmware Programming guide.
 */
typedef enum
{
    DNX_HBMC_FIRMWARE_PARAM_HBM_RESERVED = 0x00,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MAX_TIMEOUT = 0x01,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT1_CYCLES = 0x02,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT2_CYCLES = 0x03,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT3_CYCLES = 0x04,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT4_CYCLES = 0x05,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT5_CYCLES = 0x06,
    DNX_HBMC_FIRMWARE_PARAM_HBM_RW_LATENCY_OFFSET = 0x07,
    DNX_HBMC_FIRMWARE_PARAM_HBM_LATENCY_ODD_N_EVEN = 0x08,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER0 = 0x0a,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER1 = 0x0b,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER2 = 0x0c,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER3 = 0x0d,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER4 = 0x0e,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER5 = 0x0f,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER6 = 0x10,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER7 = 0x11,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER8 = 0x12,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG0 = 0x13,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG1 = 0x14,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG2 = 0x15,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG3 = 0x16,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG4 = 0x17,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG5 = 0x18,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG6 = 0x19,
    DNX_HBMC_FIRMWARE_PARAM_HBM_LBP_DRV_IMP = 0x1a,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DELAY_CONFIG_DLL = 0x1b,
    DNX_HBMC_FIRMWARE_PARAM_HBM_IGNORE_PHYUPD_HANDSHAKE = 0x1c,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TUPDMRS_CYCLES = 0x1d,
    DNX_HBMC_FIRMWARE_PARAM_HBM_T_RDLAT_OFFSET = 0x1e,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_REPAIR_MODE = 0x1f,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_PATTERN = 0x20,
    DNX_HBMC_FIRMWARE_PARAM_HBM_SAMSUNG_MBIST_HARD_REPAIR_CYCLES = 0x21,
    DNX_HBMC_FIRMWARE_PARAM_HBM_HARD_LANE_REPAIR_CYCLES = 0x22,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_BANK_ADDRESS_END = 0x24,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_ROW_ADDRESS_END = 0x25,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_COLUMN_ADDRESS_END = 0x26,
    DNX_HBMC_FIRMWARE_PARAM_HBM_FREQ = 0x27,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DIV_MODE = 0x28,
    DNX_HBMC_FIRMWARE_PARAM_HBM_CKE_EXIT_STATE = 0x29,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER0 = 0x2a,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER1 = 0x2b,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER2 = 0x2c,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER3 = 0x2d,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER4 = 0x2e,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER5 = 0x2f,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER6 = 0x30,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER7 = 0x31,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER8 = 0x32,
    DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR0 = 0x33,
    DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR1 = 0x34,
    DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR2 = 0x35,
    DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR3 = 0x36,
    DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR4 = 0x37,
    DNX_HBMC_FIRMWARE_PARAM_MMT_LFSR_COMPARE_STICKY = 0x38,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_T_RDLAT_OFFSET = 0x39,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER15 = 0x3a,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER15 = 0x3b,
    DNX_HBMC_FIRMWARE_PARAM_HBM_BYPASS_TESTMODE_RESET = 0x3c,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DISABLE_ADDR_LANE_REPAIR = 0x3d,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_CONFIGURATION = 0x3e,
    DNX_HBMC_FIRMWARE_PARAM_HBM_BYPASS_REPAIR_ON_RESET = 0x3f,
    DNX_HBMC_FIRMWARE_PARAM_HBM_STACK_HEIGHT = 0x40,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MANUFACTURING_ID = 0x41,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DENSITY = 0x42,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MANUALLY_CONFIGURE_ID = 0x43,
    DNX_HBMC_FIRMWARE_PARAM_HBM_PARITY_LATENCY = 0x44,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_PARITY_LATENCY = 0x45,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD0 = 0x49,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD1 = 0x4a,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD2 = 0x4b,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD3 = 0x4c,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD4 = 0x4d,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD5 = 0x4e,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD6 = 0x4f,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD0 = 0x50,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD1 = 0x51,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD2 = 0x52,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD3 = 0x53,
    DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD4 = 0x54,
    DNX_HBMC_FIRMWARE_PARAM_MMT_AWORD_MISR0 = 0x55,
    DNX_HBMC_FIRMWARE_PARAM_MMT_AWORD_MISR1 = 0x56,
    DNX_HBMC_FIRMWARE_PARAM_MMT_T_RDLAT_OFFSET = 0x57,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER0 = 0x58,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER1 = 0x59,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER2 = 0x5a,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER3 = 0x5b,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER4 = 0x5c,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER5 = 0x5d,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER6 = 0x5e,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER7 = 0x5f,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER8 = 0x60,
    DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER16 = 0x61,
    DNX_HBMC_FIRMWARE_PARAM_HBM_SAMSUNG_MBIST_WORKAROUND = 0x62,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DWORD_OP = 0x63,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_TCR_SETTING = 0x64,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_SUPPLY_CONDITIONING = 0x65,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODEL_NUMBER = 0x66,
    DNX_HBMC_FIRMWARE_PARAM_HBM_DATE_CODE = 0x67,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_VERIFY = 0x68,
    DNX_HBMC_FIRMWARE_PARAM_HBM_SAVE_SOFT_LANE_REPAIRS = 0x69,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_READ_REPEAT = 0x6a,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_COLUMN_RANGE = 0x6b,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_BURST_LENGTH = 0x6c,
    DNX_HBMC_FIRMWARE_PARAM_HBM_AWORD_MODE = 0x6d,
    DNX_HBMC_FIRMWARE_PARAM_HBM_SPICO_FREQ = 0x6e,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TM0 = 0x6f,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TM1 = 0x70,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TM2 = 0x71,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TM3 = 0x72,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TM4 = 0x73,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MODE1_LANE_REPAIR_LIMIT = 0x76,
    DNX_HBMC_FIRMWARE_PARAM_HBM_TOTAL_LANE_REPAIR_LIMIT = 0x77,
    DNX_HBMC_FIRMWARE_PARAM_HBM_USE_MMT_IN_COMP_REPAIR = 0x78,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_RDDATA_EN = 0x79,
    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_ROW_RANGE = 0x7a
} dnx_hbmc_firmware_param_e;

/**
 * \brief
 *   Struct to hold firmware information.
 */
typedef struct
{
    /** Revision of the HBM firmware */
    uint32 revision;
    /** Build ID of the HBM firmware */
    uint32 build_id;
} dnx_hbmc_firmware_info_t;

/**
 * \brief
 *   Holds all spare_data results.
 *
 *   Some of the operations have data results that requires multiple registers.
 *   The results can be get by reading the hbm_spare result type.
 */
typedef struct dnx_hbmc_firmware_operation_result_hbm_spare_s
{
    /** Data for all hbm_spare_* */
    uint32 hbm_spare_data[DNX_DATA_MAX_DRAM_FIRMWARE_NOF_HBM_SPARE_DATA_RESULTS];
} dnx_hbmc_firmware_operation_result_hbm_spare_t;

/**
 * \brief - Get firmware information
 *
 * \param [in] unit - unit number
 * \param [in] hbmc_index - hbmc index
 * \param [out] firmware_info - firmware information
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * none
 * \see
 *   * none
 */
shr_error_e dnx_hbmc_firmware_info_get(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_info_t * firmware_info);

/**
 * \brief
 *   Launch firmware operation and check for errors.
 *
 * \param [in] unit - unit id.
 * \param [in] hbmc_index  - hbmc index.
 * \param [in] hbmc_channel - hbmc channel. Use DNX_HBMC_FIRMWARE_ALL_CHANNELS to run operation on all channels.
 * \param [in] operation - firmware operation to run.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_firmware_launch_operation(
    int unit,
    int hbmc_index,
    int hbmc_channel,
    dnx_hbmc_firmware_operation_e operation);

/**
 * \brief
 *   Get operation's data results from hbm_spare.
 *
 *   Some of the operations have data results that can be get by reading
 *   the hbm_spare result type. This can be achieved by calling the
 *   "get operation result" interrupt with the hbm_spare value.
 *
 *   The data has a different meaning, depends on the operation.
 *   For more info refer to the firmware programming guide.
 *
 * \param [in] unit - unit id.
 * \param [in] hbmc_index - hbmc index.
 * \param [in] nof_hbm_spares - number of required HBM spare data results to read.
 * \param [out] hbm_spare - a struct to hold the result of all hbm_spare.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_firmware_operation_result_spare_get(
    int unit,
    int hbmc_index,
    uint32 nof_hbm_spares,
    dnx_hbmc_firmware_operation_result_hbm_spare_t * hbm_spare);

/**
 * \brief
 *   The HBM firmware has many user-configurable parameters.
 *   Setting an HBM parameter requires two interrupts:
 *   First interrupt to specify the parameter to program,
 *   and second interrupt to specify the parameter value.
 *
 * \param [in] unit - unit id.
 * \param [in] hbmc_index  - hbmc index.
 * \param [in] param - parameter to set.
 * \param [in] value - the parameter value.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_firmware_param_set(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_param_e param,
    uint32 value);

/**
 * \brief
 *   The HBM firmware has many user-configurable parameters.
 *   The current value of any parameter can be queried with
 *   this function.
 *
 * \param [in] unit - unit id.
 * \param [in] hbmc_index  - hbmc index.
 * \param [in] param - parameter to get.
 * \param [out] value - the parameter value.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_firmware_param_get(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_param_e param,
    uint32 *value);

/**
 * \brief - Initialize the HBM phy through firmware
 *
 * \param [in] unit - unit number
 * \param [in] hbmc_index - HBMC index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_firmware_phy_init(
    int unit,
    int hbmc_index);

#endif /* _BCMINT_DNX_DRAM_HBMC_HBMC_FIRMWARE_H_INCLUDED */
