/** \file l2_learn.c
 * $Id$
 *
 * L2 learn related procedures for DNX.
 *
 * This file contains functions related to l2 learning.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/field/field_context.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/swstate/auto_generated/access/algo_l2_access.h>
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */
/** Default value for the maximum number of aggregated OLP (offline processor) commands in a DSP (Dune special packet) */
#define MAX_NOF_DSP_CMD_DEFAULT 7

/** Default value for the maximum aggregated events before sending a DSP command */
#define MAX_DSP_CMD_DELAY_DEFAULT 7

/** Default value for the minimum size of a DSP command in bytes  */
#define MIN_DSP_CMD_SIZE_DEFAULT 64

/** Default value for DSP identifier mask */
#define DSP_IDENTIFIER_MASK_DEFAULT 0xffff

/** DSP header sizes */
#define DSP_PTCH_HEADER_SIZE 2
#define DSP_ITMH_HEADER_SIZE 5
#define DSP_UDH_HEADER_SIZE 1
#define DSP_OTMH_HEADER_SIZE 2
#define DSP_ETH_HEADER_SIZE 14
#define DSP_VLAN_HEADER_SIZE 4

/** Entry Time to leave in Filter, counted in LEARN_FILTER_RES units */
#define LEARN_FILTER_ENTRY_TTL_DEFAULT 0x30

/** Core used for accessing the OLP */
#define OLP_CORE 0

/** FFC defines */

/** Offsets and widths in the 64b buffer that is sent to the mact entry */
#define LEARN_PAYLOAD_TOTAL_WIDTH           64
#define LEARN_PAYLOAD_TYPE_WIDTH            6
#define LEARN_PAYLOAD_TYPE_OFFSET           (LEARN_PAYLOAD_TOTAL_WIDTH - LEARN_PAYLOAD_TYPE_WIDTH)
#define LEARN_PAYLOAD_DESTINATION_WIDTH     21
#define LEARN_PAYLOAD_DESTINATION_OFFSET    (LEARN_PAYLOAD_TYPE_OFFSET - LEARN_PAYLOAD_DESTINATION_WIDTH)
#define LEARN_PAYLOAD_ENTRY_GROUP_WIDTH     4
#define LEARN_PAYLOAD_ENTRY_GROUP_OFFSET    (LEARN_PAYLOAD_DESTINATION_OFFSET - LEARN_PAYLOAD_ENTRY_GROUP_WIDTH)
#define LEARN_PAYLOAD_OUT_LIF_WIDTH         22
#define LEARN_PAYLOAD_OUT_LIF_OFFSET        (LEARN_PAYLOAD_ENTRY_GROUP_OFFSET - LEARN_PAYLOAD_OUT_LIF_WIDTH)
#define LEARN_PAYLOAD_STRENGTH_WIDTH        2
#define LEARN_PAYLOAD_STRENGTH_OFFSET       (LEARN_PAYLOAD_OUT_LIF_OFFSET - LEARN_PAYLOAD_STRENGTH_WIDTH)

/*
 * Jericho mode beginning
 * {
 */
#define LEARN_PAYLOAD_EEI_WIDTH             24
#define LEARN_PAYLOAD_EEI_OFFSET            (LEARN_PAYLOAD_ENTRY_GROUP_OFFSET - LEARN_PAYLOAD_EEI_WIDTH)
#define LEARN_PAYLOAD_STRENGTH_OFFSET_W_EEI (LEARN_PAYLOAD_EEI_OFFSET - LEARN_PAYLOAD_STRENGTH_WIDTH)
/*
 * }
 * Jericho mode end
 */

/** Offsets and widths in the learn data buffer that is the source */
#define LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET 47
#define LEARN_PAYLOAD_SOURCE_LEARN_DATA_WIDTH 48
#define LEARN_PAYLOAD_SOURCE_SRC_SYSTEM_PORT_OFFSET 31
#define LEARN_PAYLOAD_SOURCE_SRC_SYSTEM_PORT_WIDTH 16
#define LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET 23
#define LEARN_PAYLOAD_SOURCE_LPKGV_WIDTH 8
#define LEARN_PAYLOAD_SOURCE_GLOBAL_LIF_ID_OFFSET 1
#define LEARN_PAYLOAD_SOURCE_GLOBAL_LIF_ID_WIDTH 22
#define LEARN_PAYLOAD_SOURCE_LEARN_LIF_OFFSET 0
#define LEARN_PAYLOAD_SOURCE_LEARN_LIF_WIDTH 1

/** OLP headers */
#define OLP_HEADER_BITMAP_ETHERNET 0x1
#define OLP_HEADER_BITMAP_VLAN     0x2
#define OLP_HEADER_BITMAP_UDH      0x4
#define OLP_HEADER_BITMAP_ITMH     0x8
#define OLP_HEADER_BITMAP_OTMH     0x10
#define OLP_HEADER_BITMAP_PTCH2    0x20

/*
 * Forward MACT
 */

/*
 * Macro for building learn payload for InLIF context.                                                                             .
 *                                                                                                                                 .
 * Key offset (6b) - offset to the location in the created payload.
 * Field width (6b) - field width. How many bits to copy to the payload.
 * FFC data (7b) - Copy start bit from the concatenated 95 bits of the resources.
 */
#define LEARN_PAYLOAD_KEY_OFFSET_MASK 0x3f
#define LEARN_PAYLOAD_FIELD_WIDTH_MASK 0x3f
#define LEARN_PAYLOAD_FFC_DATA_MASK 0x7f
#define LEARN_PAYLOAD_KEY_OFFSET_WIDTH 6
#define LEARN_PAYLOAD_FIELD_WIDTH_WIDTH 6
#define LEARN_PAYLOAD_FFC_DATA_WIDTH 7

#define BUILD_LEARN_PAYLOAD_FFC(key_offset,field_width,ffc_data)  \
 (((key_offset) & LEARN_PAYLOAD_KEY_OFFSET_MASK) | \
 (((field_width) & LEARN_PAYLOAD_FIELD_WIDTH_MASK)<<LEARN_PAYLOAD_KEY_OFFSET_WIDTH) | \
  (((ffc_data) & LEARN_PAYLOAD_FFC_DATA_MASK)<<(LEARN_PAYLOAD_KEY_OFFSET_WIDTH+LEARN_PAYLOAD_FIELD_WIDTH_WIDTH)))

dnx_l2_cbs_t dnx_l2_cbs[BCM_MAX_NUM_UNITS];

/*
 * }
 */

/**
 * \brief - Init the DMA channel of the OLP
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_olp_dma_init(
    int unit)
{
    int olp_channel;
    soc_dnx_fifodma_config_t fifodma_info;
    uint32 host_memory_size;
    uint32 entry_size;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if we are during warmboot the is_started may be incorrect, it 
     * may have been turned off during init, but this information 
     * was not synced to the external storage and this info was 
     * overwritten with incorrect info during warmboot recovery
     */
    if (!SOC_WARM_BOOT(unit))
    {
        /** Stop and free the channel if it is already started */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_olp, &is_started));

        if (is_started)
        {
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_olp, &olp_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_stop(unit, olp_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, olp_channel));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, soc_dnx_fifodma_src_olp, &olp_channel));

    host_memory_size = dnx_data_l2.dma.learning_fifo_dma_buffer_size_get(unit);

    /** In case no host memory was defined do not start the DMA */
    if (host_memory_size == 0)
    {
        SHR_EXIT();
    }

    entry_size = soc_reg_bytes(unit, OLP_HEAD_OF_CPU_DSPG_CMD_FIFOr);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &fifodma_info));
    fifodma_info.copyno = REG_PORT_ANY;
    fifodma_info.force_entry_size = 0;
    fifodma_info.is_mem = 0;
    fifodma_info.is_poll = 0;
    /** Round down the number of entries to the closest power of 2 */
    fifodma_info.max_entries = utilex_power_of_2(utilex_log2_round_down(host_memory_size / entry_size));
    fifodma_info.reg = OLP_HEAD_OF_CPU_DSPG_CMD_FIFOr;
    fifodma_info.threshold = dnx_data_l2.dma.learning_fifo_dma_threshold_get(unit);
    fifodma_info.timeout = dnx_data_l2.dma.learning_fifo_dma_timeout_get(unit);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start(unit, olp_channel, &fifodma_info, "dnx_olp_dma_handler"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init OLP for interworking with jericho
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_olp_jr_mode_init(
    int unit)
{
    uint8 vmv_value, vmv_size, offset;
    uint32 app_db_id;
    uint32 learn_payload_high = 0, learn_payload_low = 0;
    uint32 lpkgv_shift, lpkgv_mask, lpkgv_with_outlif;
    uint32 entry_handle_id;
    uint64 learn_payload;
    int app_db_size;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get MACT App DB */
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &app_db_id, &app_db_size));

    /** Get MACT VMV */
    SHR_IF_ERR_EXIT(mdb_get_vmv_size_value
                    (unit, DBAL_PHYSICAL_TABLE_LEM, MDB_EM_ENTRY_ENCODING_HALF, &vmv_size, &vmv_value));

    /** Configuration for translation from Jericho to Jericho-2 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_JR_TO_JR_2_TRANSLATION_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_TO_JR_2_VMV, INST_SINGLE, vmv_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_TO_JR_2_APP_DB, INST_SINGLE, app_db_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_DELETE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_DELETE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_INSERT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_LEARN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_REFRESH, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_REFRESH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_LEARN, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_LEARN_UNUSED);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_DEFRAG, INST_SINGLE,
                                 DBAL_NOF_ENUM_OLP_COMMAND_VALUES);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_ACK, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_ACK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_TRANSPLANT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_TRANSPLANT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_CMD_TYPE_CHANGE_FAIL, INST_SINGLE,
                                 DBAL_NOF_ENUM_OLP_COMMAND_VALUES);

    /** Construct init value of learn payload for type lif and destination */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_IRPP_FWD_STAGE_FORMATS,
                     DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_SINGLE_OUTLIF, &lpkgv_with_outlif));
    lpkgv_mask = dnx_data_l2.olp.lpkgv_mask_get(unit);
    lpkgv_shift = dnx_data_l2.olp.lpkgv_shift_get(unit);
    learn_payload_high |= (lpkgv_with_outlif & lpkgv_mask) << lpkgv_shift;
    COMPILER_64_SET(learn_payload, learn_payload_high, learn_payload_low);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_LIF_AND_DESTINATION_INIT_VALUE, INST_SINGLE,
                                 learn_payload);

    /** Set destination offset in learn payload */
    offset = dnx_data_l2.olp.destination_offset_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION_OFFSET, INST_SINGLE, offset);

    /** Set outlif offset in learn payload */
    offset = dnx_data_l2.olp.outlif_offset_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_OFFSET, INST_SINGLE, offset);

    /** Set eei offset in learn payload */
    offset = dnx_data_l2.olp.eei_offset_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_OFFSET, INST_SINGLE, offset);

    /** Set fec offset in learn payload */
    offset = dnx_data_l2.olp.fec_offset_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_OFFSET, INST_SINGLE, offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Configuration for translation from Jericho-2 to Jericho */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OLP_JR_2_TO_JR_TRANSLATION_CFG, entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_INSERT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_LEARN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_TRANSPLANT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_TRANSPLANT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_DELETE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_DELETE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_REFRESH, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_REFRESH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_LEARN, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_LEARN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_ACK, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_ACK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_FLUSH_REPLY, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_CHANE_FAIL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_2_CMD_TYPE_NOP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OLP_COMMAND_JR_CHANE_FAIL);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE == system_headers_mode)
    {
        /** set OLP to JR mode to interwork with Jericho in the system*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OLP_GENERAL_CONFIGURATION, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_JR_MODE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Handle OLP event that was copied by the DMA. Parse the OLP event and call the user's callback.
 *
 * \param [in] unit - unit id
 * \param [in] entry - OLP event in the format OLP_HEAD_OF_CPU_DSPG_CMD_FIFO
 * \param [in] entry_size - event size in bytes
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void
dnx_olp_dma_handler(
    int unit,
    void *entry,
    int entry_size)
{
    uint32 entry_handle_id;
    int cb_index;
    void *userdata;
    bcm_l2_addr_t l2addr;
    uint32 app_db_id_ivl;
    uint32 app_db_id_svl;
    int app_db_size_ivl;
    int app_db_size_svl;
    uint32 received_appdb_id;
    uint32 olp_cmd_type;
    dbal_tables_e table_id;
    uint32 dsp_key[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 dsp_payload[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    int dma_register_size_in_bits;
    int dsp_payload_size_in_bits;
    int bcm_command_type = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &app_db_id_svl, &app_db_size_svl));
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT_IVL, &app_db_id_ivl, &app_db_size_ivl));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_DMA_DSP_COMMAND, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, DBAL_TABLE_L2_DMA_DSP_COMMAND, 0, &dma_register_size_in_bits));

    /*
     * Update DBAL handle with the event's details including payload and key
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                    (unit, entry_handle_id, NULL, _SHR_CORE_ALL, dma_register_size_in_bits, entry));

    /** Get AppDB ID */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LEM_APP_DB,
                                                        INST_SINGLE, &received_appdb_id));

    /** OLP_COMMAND */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND,
                                                        INST_SINGLE, &olp_cmd_type));

    /** Get Payload */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PAYLOAD,
                                                            INST_SINGLE, dsp_payload));

    /** Get Key */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_KEY,
                                                            INST_SINGLE, dsp_key));

    /** Open a DBAL handle from the correct table according to the AppDB ID */
    if (received_appdb_id == app_db_id_svl)
    {
        table_id = DBAL_TABLE_FWD_MACT;
    }
    else if (received_appdb_id == app_db_id_ivl)
    {
        table_id = DBAL_TABLE_FWD_MACT_IVL;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Received AppDB ID is not legal.\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_L2_DMA_DSP_COMMAND, DBAL_FIELD_PAYLOAD, 0, 0, INST_SINGLE,
                     &dsp_payload_size_in_bits));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                    (unit, entry_handle_id, dsp_key, _SHR_CORE_ALL, dsp_payload_size_in_bits, dsp_payload));

    /** Parse the handle to bcm_l2_addr_t format */
    SHR_IF_ERR_EXIT(dnx_l2_addr_from_dbal_to_struct(unit, entry_handle_id, table_id, &l2addr));

    /** Convert HW command type to BCM command type */
    switch (olp_cmd_type)
    {
        case DBAL_ENUM_FVAL_OLP_COMMAND_LEARN:
            bcm_command_type = BCM_L2_CALLBACK_LEARN_EVENT;
            break;
        case DBAL_ENUM_FVAL_OLP_COMMAND_TRANSPLANT:
            bcm_command_type = BCM_L2_CALLBACK_MOVE_EVENT;
            break;
        case DBAL_ENUM_FVAL_OLP_COMMAND_DELETE:
            bcm_command_type = BCM_L2_CALLBACK_AGE_EVENT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported DSP command type arrived. olp_cmd_type = %d\n", olp_cmd_type);
    }

    /** Call the registered callbacks */
    for (cb_index = 0; cb_index < DNX_L2_CB_MAX; cb_index++)
    {
        if (dnx_l2_cbs[unit].entry[cb_index].callback != NULL)
        {
            userdata = dnx_l2_cbs[unit].entry[cb_index].userdata;
            dnx_l2_cbs[unit].entry[cb_index].callback(unit, &l2addr, bcm_command_type, userdata);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_VOID_FUNC_EXIT;;
}

/**
 * \brief - Stop and free the dma channel of the olp
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_olp_dma_deinit(
    int unit)
{
    int olp_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_olp, &olp_channel));

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, olp_channel));

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief -
 *  Verify parameters of bcm_dnx_l2_addr_msg_distribute_get.
 */
static int
dnx_l2_addr_msg_distribute_get_verify(
    int unit,
    bcm_l2_addr_distribute_t * distribution)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(distribution, _SHR_E_PARAM, "distribution parameter is NULL");

    /** Verify that we are in VLAN mode */
    DNX_L2_LEARN_MODE_VERIFY_AS_VLAN();

    /** Make sure that only one OLP command is specified */
    if (distribution->flags != BCM_L2_ADDR_DIST_ACK_EVENT &&
        distribution->flags != BCM_L2_ADDR_DIST_REFRESH_EVENT &&
        distribution->flags != BCM_L2_ADDR_DIST_AGED_OUT_EVENT &&
        distribution->flags != BCM_L2_ADDR_DIST_STATION_MOVE_EVENT &&
        distribution->flags != BCM_L2_ADDR_DIST_LEARN_EVENT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. Only one OLP command should be specified\n");
    }

    if (!DNX_VSI_VALID(unit, distribution->vid) && 
        ((distribution->vid != (bcm_vlan_t) - 1) || 
            (!dnx_data_l2.hw_bug.feature_get(unit, dnx_data_l2_hw_bug_age_out_and_refresh_profile_selection))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. distribution->vid %d is not a valid vsi.\n", distribution->vid);
    }

    if ((distribution->flags & (BCM_L2_ADDR_DIST_REFRESH_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT)) &&
        (distribution->vid != (bcm_vlan_t) - 1) &&
        dnx_data_l2.hw_bug.feature_get(unit, dnx_data_l2_hw_bug_age_out_and_refresh_profile_selection))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "In Jericho 2 A0, refresh and age out can be used only with vid==-1.\n");
    }

    if ((distribution->flags &
         (BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_ACK_EVENT))
        && distribution->vid == (bcm_vlan_t) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "In Jericho 2 A0, vid==-1 is allowed only for refresh and age out.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify parameters of bcm_dnx_l2_addr_msg_distribute_set.
 */
static int
dnx_l2_addr_msg_distribute_set_verify(
    int unit,
    bcm_l2_addr_distribute_t * distribution)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!distribution)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "distribution parameter is NULL");
    }

    /** Make sure that at least one OLP command is specified */
    if (!
        (distribution->flags &
         (BCM_L2_ADDR_DIST_ACK_EVENT | BCM_L2_ADDR_DIST_REFRESH_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
          BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. At least one OLP command should be specified\n");
    }

    /** Make sure that at least one distribution is specified */
    if (!(distribution->flags & (BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER | BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER |
                                 BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER | BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. At least one distribution queue should be specified\n");
    }

    if (dnx_data_l2.hw_bug.feature_get(unit, dnx_data_l2_hw_bug_age_out_and_refresh_profile_selection))
    {
        if (!DNX_VSI_VALID(unit, distribution->vid) && distribution->vid != (bcm_vlan_t) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. distribution->vid %d is not a valid vsi.\n",
                         distribution->vid);
        }

        if ((distribution->flags & (BCM_L2_ADDR_DIST_REFRESH_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT)) &&
            distribution->vid != (bcm_vlan_t) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "In Jericho 2 A0, refresh and age out can be used only with vid==-1.\n");
        }

        if ((distribution->flags &
             (BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_ACK_EVENT))
            && distribution->vid == (bcm_vlan_t) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "In Jericho 2 A0, vid==-1 is allowed only for refresh and age out.\n");
        }
    }
    else
    {
        if (!DNX_VSI_VALID(unit, distribution->vid))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. distribution->vid %d is not a valid vsi.\n",
                         distribution->vid);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fetch OLP pp port and its header type
 * 
 * \param [in] unit - unit id
 * \param [in] olp_pp_port - pp port configured for the OLP
 * \param [in] olp_in_port_header_type - type of the port from the OLP
 * \param [in] olp_out_port_header_type - type of the port to the OLP
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_olp_port_get(
    int unit,
    int *olp_pp_port,
    int *olp_in_port_header_type,
    int *olp_out_port_header_type)
{
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;
    bcm_pbmp_t pbmp;
    int num_of_ports;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the OLP port number */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, port_config.olp);

     /** Make sure that OLP port was configured. */
    BCM_PBMP_COUNT(pbmp, num_of_ports);
    if (num_of_ports != 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "One OLP port should be configured in the device.\n");
    }

    /** Take the first and only OLP port */
    BCM_PBMP_ITER(pbmp, *olp_pp_port)
    {
        /** There is only one OLP port on a device */
        break;
    }
    value.value = 0;
    key.type = bcmSwitchPortHeaderType;
    key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT;
    /** Get the switch port header type of the OLP port*/
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_get(unit, *olp_pp_port, key, &value));
    *olp_out_port_header_type = value.value;

    value.value = 0;
    key.type = bcmSwitchPortHeaderType;
    key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
    /** Get the switch port header type of the OLP port*/
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_get(unit, *olp_pp_port, key, &value));
    *olp_in_port_header_type = value.value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the learn properties of a vsi. New vsi profile is created if needed. Old profile will be deleted if the ref
 *        counter becomes zero.
 *
 * \param [in] unit - unit ID
 * \param [in] vsi - vsi to update
 * \param [in] old_vsi_learning_profile - existing vsi profile ID
 * \param [in] new_vsi_learning_profile - new vsi profile ID
 * \param [in] is_first - 1 if the new learning profile just got its first vsi reference. 0 otherwise.
 * \param [in] is_last - 1 if the old learning profile has a reference count of 0. 0 otherwise.
 * \param [in] vsi_learning_profile_info - info to set in the vsi_profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_vsi_learning_profile_hw_update(
    int unit,
    int vsi,
    int old_vsi_learning_profile,
    int new_vsi_learning_profile,
    uint8 is_first,
    uint8 is_last,
    dnx_vsi_learning_profile_info_t * vsi_learning_profile_info)
{

    uint32 entry_handle_id;
    uint8 interrupt_enable = 0;
    uint8 message_enable = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Update the vsi profile with the new parameters */

    /** Allocate the new profile in the HW if it is the first instance. */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LEARNING_PROFILE, new_vsi_learning_profile);

        /** In case of using -1 change the value to the max value of the field. */
        if (vsi_learning_profile_info->limit == -1)
        {
            dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_MAX_VALUE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT, INST_SINGLE,
                                         vsi_learning_profile_info->limit);
        }

        /** When a limit is set the interrupt enable should be set as well */
        interrupt_enable = 0;
        message_enable = 0;
        if (vsi_learning_profile_info->limit != 0)
        {
            interrupt_enable = 1;
            message_enable = 1;
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_ENABLE, INST_SINGLE, interrupt_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESSAGE_ENABLE, INST_SINGLE, message_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_FORWARDING_PROFILE, INST_SINGLE,
                                     vsi_learning_profile_info->event_forwarding_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, INST_SINGLE,
                                     vsi_learning_profile_info->aging_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CYCLE_PERIOD, INST_SINGLE,
                                     vsi_learning_profile_info->cycle_period);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CYCLE_MASK, INST_SINGLE,
                                     vsi_learning_profile_info->cycle_mask);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (old_vsi_learning_profile != new_vsi_learning_profile)
    {
        /** Update the VSI to point to the new profile */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LEARNING_PROFILE, INST_SINGLE,
                                     new_vsi_learning_profile);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Delete the old profile from the HW if it is the last instance. */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LEARNING_PROFILE, old_vsi_learning_profile);

        /** Delete the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the vsi profile id of a given VSI
 *
 * \param [in] unit - unit ID
 * \param [in] vsi - vsi to get its profile
 * \param [out] vsi_profile - vsi profile id of the given vsi
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_vsi_learning_profile_id_get(
    int unit,
    int vsi,
    int *vsi_profile)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the VSI's profile */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI_LEARNING_PROFILE,
                                                        INST_SINGLE, (uint32 *) vsi_profile));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Takes the old forwarding profile and updates the values that are valid from the new profile.
 * Set the result in the combined profile.
 *
 * \param [in] old_event_forwarding_profile_info - default values for the combined profile
 * \param [in] new_event_forwarding_profile_info - values to update the old profile
 * \param [out] combined_event_forwarding_profile_info - updated profile with the values of the new and old profiles
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_combine_old_forwarding_profile_with_new(
    dnx_event_forwarding_profile_info_t * old_event_forwarding_profile_info,
    dnx_event_forwarding_profile_info_t * new_event_forwarding_profile_info,
    dnx_event_forwarding_profile_info_t * combined_event_forwarding_profile_info)
{
    sal_memcpy(combined_event_forwarding_profile_info, old_event_forwarding_profile_info,
               sizeof(dnx_event_forwarding_profile_info_t));

    if (new_event_forwarding_profile_info->ack_distribution != DISTRIBUTION_NO_CHANGE)
    {
        combined_event_forwarding_profile_info->ack_distribution = new_event_forwarding_profile_info->ack_distribution;
    }

    if (new_event_forwarding_profile_info->delete_distribution != DISTRIBUTION_NO_CHANGE)
    {
        combined_event_forwarding_profile_info->delete_distribution =
            new_event_forwarding_profile_info->delete_distribution;
    }

    if (new_event_forwarding_profile_info->learn_distribution != DISTRIBUTION_NO_CHANGE)
    {
        combined_event_forwarding_profile_info->learn_distribution =
            new_event_forwarding_profile_info->learn_distribution;
    }

    if (new_event_forwarding_profile_info->refresh_distribution != DISTRIBUTION_NO_CHANGE)
    {
        combined_event_forwarding_profile_info->refresh_distribution =
            new_event_forwarding_profile_info->refresh_distribution;
    }

    if (new_event_forwarding_profile_info->transplant_distribution != DISTRIBUTION_NO_CHANGE)
    {
        combined_event_forwarding_profile_info->transplant_distribution =
            new_event_forwarding_profile_info->transplant_distribution;
    }
}

/**
 * \brief - Update the HW with the destribution of one OLP command type
 *
 * \param [in] unit - unit id
 * \param [in] olp_command - command type to update (DBAL_ENUM_FVAL_OLP_COMMAND_*)
 * \param [in] forwarding_distribution - Bitmap representing the queues for distribution
 *  [loopback (1b), MC (1b), CPU (1b), DMA to CPU (1b)]
 * \param [in] forwarding_profile_id - forwarding profile id to update
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_olp_event_distribute_hw_set(
    int unit,
    dbal_enum_value_field_olp_command_e olp_command,
    uint32 forwarding_profile_id,
    uint32 forwarding_distribution)
{
    uint32 entry_handle_id;
    uint8 distribution_mc, distribution_cpu, distribution_cpu_dma;    

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set forwarding profile data*/
    
    l2_db_context.fid_profiles_changed.set(unit, 1);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_DSP_EVENT_ROUTE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_FORWARDING_PROFILE, forwarding_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND, olp_command);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX, 0, 1);

    distribution_mc = GET_MC_DISTRIBUTION_FROM_BUFFER(forwarding_distribution);
    distribution_cpu = GET_CPU_DISTRIBUTION_FROM_BUFFER(forwarding_distribution);
    distribution_cpu_dma = GET_CPU_DMA_DISTRIBUTION_FROM_BUFFER(forwarding_distribution);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_DISTRIBUTION_MC, INST_SINGLE, distribution_mc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_DISTRIBUTION_CPU, INST_SINGLE, distribution_cpu);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_DISTRIBUTION_CPU_DMA, INST_SINGLE,
                                 distribution_cpu_dma);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update a forwarding profile with new info.
 *
 * \param [in] unit - unit ID
 * \param [in] old_event_forwarding_profile - old event forwarding profile.
 * \param [in] new_event_forwarding_profile - ID for the newly created forwarding profile.
 * \param [in] is_first - 1 if new event forwarding profile got its first reference. 0 otherwise.
 * \param [in] event_forwarding_profile_info - forwarding profile data
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_event_forwarding_profile_hw_set(
    int unit,
    int old_event_forwarding_profile,
    int new_event_forwarding_profile,
    uint8 is_first,
    dnx_event_forwarding_profile_info_t * event_forwarding_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_first)
    {
        /** Set forwarding profile data command type after command type */

        /** LEARN  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_LEARN,
                                                        new_event_forwarding_profile,
                                                        event_forwarding_profile_info->learn_distribution));

        /** TRANSPLANT  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_TRANSPLANT,
                                                        new_event_forwarding_profile,
                                                        event_forwarding_profile_info->transplant_distribution));

        /** DELETE  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_DELETE,
                                                        new_event_forwarding_profile,
                                                        event_forwarding_profile_info->delete_distribution));

        /** REFRESH  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_REFRESH,
                                                        new_event_forwarding_profile,
                                                        event_forwarding_profile_info->refresh_distribution));

        /** ACK  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_ACK,
                                                        new_event_forwarding_profile,
                                                        event_forwarding_profile_info->ack_distribution));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset event forwarding profile data in HW
 *
 * \param [in] unit - unit id
 * \param [in] event_forwarding_profile_id - profile to delete
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_event_forwarding_profile_hw_delete(
    int unit,
    int event_forwarding_profile_id)
{
    dnx_event_forwarding_profile_info_t event_forwarding_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&event_forwarding_profile_info, 0, sizeof(dnx_event_forwarding_profile_info_t));

    /** Reset forwarding profile data */

    /** LEARN  */
    SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_LEARN,
                                                    event_forwarding_profile_id,
                                                    event_forwarding_profile_info.learn_distribution));

    /** TRANSPLANT  */
    SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_TRANSPLANT,
                                                    event_forwarding_profile_id,
                                                    event_forwarding_profile_info.transplant_distribution));

    /** DELETE  */
    SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_DELETE,
                                                    event_forwarding_profile_id,
                                                    event_forwarding_profile_info.delete_distribution));

    /** REFRESH  */
    SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_REFRESH,
                                                    event_forwarding_profile_id,
                                                    event_forwarding_profile_info.refresh_distribution));

    /** ACK  */
    SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_ACK,
                                                    event_forwarding_profile_id,
                                                    event_forwarding_profile_info.ack_distribution));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the DSP forwarding distribution with default values according to the learn mode.
 *
 * \param [in] unit - unit ID
 * \param [in] learn_mode - learn mode for setting default forwarding profiles data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_event_forwarding_profile_init(
    int unit,
    dnx_learn_mode_t learn_mode)
{
    dnx_event_forwarding_profile_info_t forwarding_profile_info;
    int alloc_flags;
    uint8 first_reference;
    int forwarding_profile = DNX_FORWARDING_EVENT_PROFILE_DEFAULT_PROFILE_ID;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    alloc_flags = SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
    sal_memset(&forwarding_profile_info, 0, sizeof(dnx_event_forwarding_profile_info_t));

    switch (learn_mode)
    {
        case LEARN_MODE_DISTRIBUTED:
            forwarding_profile_info.refresh_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            forwarding_profile_info.learn_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            forwarding_profile_info.transplant_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            break;
        case LEARN_MODE_CENTRALIZED:
            forwarding_profile_info.delete_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            forwarding_profile_info.learn_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            forwarding_profile_info.ack_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            forwarding_profile_info.transplant_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 1, 0, 0);
            break;
        case LEARN_MODE_DMA:
            forwarding_profile_info.delete_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 0, 0, 1);
            forwarding_profile_info.learn_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 0, 0, 1);
            forwarding_profile_info.transplant_distribution = BUILD_OLP_DISTRIBUTION_BUFFER(0, 0, 0, 1);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong learn mode %d \n", learn_mode);
    }

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.clear(unit, _SHR_CORE_ALL));

    /** Update the vsi forwarding profile with the new parameters */
    SHR_IF_ERR_EXIT(algo_l2_db.
                    event_forwarding_profile.allocate_single(unit, _SHR_CORE_ALL, alloc_flags, &forwarding_profile_info,
                                                             NULL, &forwarding_profile, &first_reference));

    /** Update the HW with the new forwarding profile properties */
    SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_hw_set(unit, 0, 0, 1, &forwarding_profile_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the learn profiles, aging profiles and event forwarding profiles in the templates manager
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_learn_reset_profiles_templates(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.clear(unit, _SHR_CORE_ALL));

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.clear(unit, _SHR_CORE_ALL));

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.clear(unit, _SHR_CORE_ALL));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the learn profiles, aging profiles and event forwarding profiles in the HW
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_learn_reset_profiles_hw(
    int unit)
{
    uint8 fid_profiles_changed;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Reset only if the tables where updated before */
    l2_db_context.fid_profiles_changed.get(unit, &fid_profiles_changed);

    if (fid_profiles_changed) 
    {
        /** Point all the VSIs to learn profile 0 */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VSI_LEARNING_PROFILE));

        /** Reset all the learning profiles to default values */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO));

        /** Reset the forwarding events distribution profiles */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_OLP_DSP_EVENT_ROUTE));

        /** Reset the aging profiles */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the learn profiles and event forwarding profiles to its default values
 * Reset the templates and HW.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_learn_reset_profiles(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Reset the templates information */
    SHR_IF_ERR_EXIT(dnx_l2_learn_reset_profiles_templates(unit));

    /** Reset the profiles in the HW */
    SHR_IF_ERR_EXIT(dnx_l2_learn_reset_profiles_hw(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify that the learn mode flags are valid
 *
 * \param [in] unit - unit id
 * \param [in] learn_mode - learn mode type.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_learn_mode_set_verify(
    int unit,
    int learn_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify all learn mode combinations */
    if (learn_mode != BCM_L2_INGRESS_CENT &&
        learn_mode != BCM_L2_INGRESS_DIST &&
        learn_mode != BCM_L2_EGRESS_DIST &&
        learn_mode != BCM_L2_EGRESS_CENT &&
        learn_mode != (BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU) && learn_mode != (BCM_L2_EGRESS_CENT | BCM_L2_LEARN_CPU))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. Illegal learn mode.learn_mode %d\n", learn_mode);
    }

    /** Egress learning is not supported on Jer2 */
    if (learn_mode & (BCM_L2_EGRESS_DIST | BCM_L2_EGRESS_CENT))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Egress learning is not supported\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the vsi profiles with its default configurations.
 *
 * \param [in] unit - unit id
 */
static shr_error_e
dnx_l2_vsi_learning_profile_init(
    int unit)
{
    uint32 entry_handle_id;
    dnx_vsi_learning_profile_info_t vsi_learning_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&vsi_learning_profile_info, 0, sizeof(dnx_vsi_learning_profile_info_t));

    /*
     * Set the default limit in the default VSI learning profile
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));

    /** Key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LEARNING_PROFILE,
                               DNX_VSI_LEARNING_PROFILE_DEFAULT_PROFILE_ID);

    /** Result values */
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the OLP events distribution of a given VSI.
 *
 * \param [in] unit - unit ID
 * \param [in] vsi - vsi to update
 * \param [in] event_forwarding_profile_info - info to set in the event_forwarding_profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_event_forwarding_profile_update(
    int unit,
    int vsi,
    dnx_event_forwarding_profile_info_t * event_forwarding_profile_info)
{
    int old_vsi_learning_profile;
    int new_vsi_learning_profile;
    int new_event_forwarding_profile;
    dnx_vsi_learning_profile_info_t old_vsi_learning_profile_info;
    dnx_vsi_learning_profile_info_t new_vsi_learning_profile_info;
    dnx_event_forwarding_profile_info_t updated_event_forwarding_profile_info;
    dnx_event_forwarding_profile_info_t old_event_forwarding_profile_info;
    uint8 is_first, is_last;
    uint8 is_first_event_forwarding_profile = 0;
    int ref_count;
    int old_learning_ref_count;
    int old_forwarding_ref_count;
    uint8 last_reference;
    int allocate_failed;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify that we are in VLAN mode */
    DNX_L2_LEARN_MODE_VERIFY_AS_VLAN();

    /** Get the details of the default forwarding profile */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &old_vsi_learning_profile));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, old_vsi_learning_profile, &old_learning_ref_count,
                     (void *) &old_vsi_learning_profile_info));

    /** Get the forwarding profile's data */
    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, old_vsi_learning_profile_info.event_forwarding_profile, &ref_count,
                     (void *) &old_event_forwarding_profile_info));

    /** Combine old forwarding profile data with the new requested values */
    dnx_combine_old_forwarding_profile_with_new(&old_event_forwarding_profile_info,
                                                event_forwarding_profile_info, &updated_event_forwarding_profile_info);

    /** Allocate forwarding profile. This function increases the ref count.  */
    allocate_failed =
        algo_l2_db.event_forwarding_profile.allocate_single(unit, _SHR_CORE_ALL, 0,
                                                            &updated_event_forwarding_profile_info, NULL,
                                                            &new_event_forwarding_profile,
                                                            &is_first_event_forwarding_profile);

    sal_memcpy(&new_vsi_learning_profile_info, &old_vsi_learning_profile_info, sizeof(dnx_vsi_learning_profile_info_t));
    new_vsi_learning_profile_info.event_forwarding_profile = new_event_forwarding_profile;

    if (allocate_failed == _SHR_E_NONE)
    {
        /** Update the VSI learning profile with the new forwarding profile */
        SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &new_vsi_learning_profile_info, old_vsi_learning_profile,
                         NULL, &new_vsi_learning_profile, &is_first, &is_last));

        /*
         *  Since allocate is used on the forwarding profile, its reference count is increased even if it is not the first learning profile
         *  that is pointing to it. In this case reduce the reference count using free_single.
         */
        if (!is_first)
        {
            SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.free_single(unit,
                                                                            _SHR_CORE_ALL, new_event_forwarding_profile,
                                                                            &last_reference));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_l2_aging_profile_increase_ref(unit, old_vsi_learning_profile_info.aging_profile));
        }

        /*
         * In case the old vsi learning profile is not pointed by any vsi, the reference count of its
         * forwarding profile should be reduced.
         */
        if (is_last || (is_first && old_vsi_learning_profile == new_vsi_learning_profile))
        {
            SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.free_single(unit, _SHR_CORE_ALL,
                                                                            old_vsi_learning_profile_info.event_forwarding_profile,
                                                                            &last_reference));

            SHR_IF_ERR_EXIT(dnx_algo_l2_aging_profile_decrease_ref(unit, old_vsi_learning_profile_info.aging_profile));
        }
    }
    else
    {
        is_first = 1;
        is_last = 0;

        /** Get the old forwarding profile info */
        SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, old_vsi_learning_profile_info.event_forwarding_profile,
                         &old_forwarding_ref_count, (void *) &old_event_forwarding_profile_info));

        if (old_learning_ref_count == 1 && old_forwarding_ref_count == 1)
        {
            SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.replace_data
                            (unit, _SHR_CORE_ALL,
                             old_vsi_learning_profile_info.event_forwarding_profile,
                             &updated_event_forwarding_profile_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "No more forwarding profiles to complete the forwarding profile update");
        }
    }

    /** Update the HW with the new forwarding profile properties */
    SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_hw_set
                    (unit, old_vsi_learning_profile_info.event_forwarding_profile, new_event_forwarding_profile,
                     is_first_event_forwarding_profile, &updated_event_forwarding_profile_info));

    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_hw_update(unit, vsi, old_vsi_learning_profile, new_vsi_learning_profile,
                                                       is_first, is_last, &new_vsi_learning_profile_info));

    /** Reset the old forwarding profile's data if no refernce is pointing to the old forwarding profile */
    if ((old_learning_ref_count == 1) && (ref_count == 1)
        && (old_vsi_learning_profile_info.event_forwarding_profile != new_event_forwarding_profile))
    {
        SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_hw_delete
                        (unit, old_vsi_learning_profile_info.event_forwarding_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set forwarding profile 0 to the given distribution for refresh and/or age out events
 * 
 * \param [in] unit - unit id
 * \param [in] event_forwarding_profile_info - distribution of events
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_event_forwarding_profile_update_default(
    int unit,
    dnx_event_forwarding_profile_info_t * event_forwarding_profile_info)
{
    int default_forwarding_profile = 0;
    dnx_event_forwarding_profile_info_t old_event_forwarding_profile_info;
    dnx_event_forwarding_profile_info_t updated_event_forwarding_profile_info;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, 0, &ref_count, (void *) &old_event_forwarding_profile_info));

    /** Combine old forwarding profile data with the new requested values */
    dnx_combine_old_forwarding_profile_with_new(&old_event_forwarding_profile_info,
                                                event_forwarding_profile_info, &updated_event_forwarding_profile_info);

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.replace_data
                    (unit, _SHR_CORE_ALL, 0, &updated_event_forwarding_profile_info));

    if (event_forwarding_profile_info->delete_distribution != DISTRIBUTION_NO_CHANGE)
    {
        /** Age out */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_DELETE,
                                                        default_forwarding_profile,
                                                        event_forwarding_profile_info->delete_distribution));
    }

    if (event_forwarding_profile_info->refresh_distribution != DISTRIBUTION_NO_CHANGE)
    {
        /** REFRESH  */
        SHR_IF_ERR_EXIT(dnx_olp_event_distribute_hw_set(unit, DBAL_ENUM_FVAL_OLP_COMMAND_REFRESH,
                                                        default_forwarding_profile,
                                                        event_forwarding_profile_info->refresh_distribution));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the distribution of the OLP events related to a given vsi
 *
 * \param [in] unit - unit ID
 * \param [in] distribution
 *  * flags - flags has bitmap defined using BCM_L2_ADDR_DIST_*. \
 * The flags are describing to which OLP queues each OLP event
 * is distributed. Set only one OLP event from
 * BCM_L2_ADDR_DIST__* and the function returns its distribution
 * queues. When the function returns it sets all the configured
 * distribution bits of the event and the given event is left as
 * a set bit as well. Flags options: \
 * BCM_L2_ADDR_DIST_LEARN_EVENT - learning event. \
 * BCM_L2_ADDR_DIST_STATION_MOVE_EVENT  - transplant event. \
 * BCM_L2_ADDR_DIST_AGED_OUT_EVENT - aged out event. \
 * BCM_L2_ADDR_DIST_REFRESH_EVENT - age refresh event. \
 * BCM_L2_ADDR_DIST_ACK_EVENT - acknowledge event. \
 * \
 * BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER - Send events to the learning distributer which should be a multicast group. \
 * BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER - Send events to the CPU using a DSP. \
 * BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER - Send events to the DMA that copies them to the CPU's memory.
 *                                            A callback should be registered to handle the events. \
 * BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER - Do not send events to any distributer. \
 * \
 *  * vid - vid is vsi to fetch its profile.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_msg_distribute_get(
    int unit,
    bcm_l2_addr_distribute_t * distribution)
{
    int vsi_profile;
    dnx_vsi_learning_profile_info_t vsi_learning_profile_info;
    dnx_event_forwarding_profile_info_t event_forwarding_profile_info;
    int ref_count;
    uint32 orig_distribution_flags;
    uint32 olp_distribution;
    uint32 requested_event;
    uint32 vsi;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_msg_distribute_get_verify(unit, distribution));

    vsi = distribution->vid;

    /** In case of -1 the result is taken from an arbitrary vsi */
    if (distribution->vid == (bcm_vlan_t) - 1)
    {
        vsi = 1;
    }
    orig_distribution_flags = distribution->flags;

    /** Get the vsi learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &vsi_profile));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, vsi_profile, &ref_count, (void *) &vsi_learning_profile_info));

    /** Get the forwarding profile's data */
    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, vsi_learning_profile_info.event_forwarding_profile, &ref_count,
                     (void *) &event_forwarding_profile_info));

    /** Get only the event bits */
    requested_event = distribution->flags &
        (BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_ACK_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
         BCM_L2_ADDR_DIST_REFRESH_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT);

    /** Check which event should be retrieved */
    if (requested_event == BCM_L2_ADDR_DIST_LEARN_EVENT)
    {
        olp_distribution = event_forwarding_profile_info.learn_distribution;
    }
    else if (requested_event == BCM_L2_ADDR_DIST_ACK_EVENT)
    {
        olp_distribution = event_forwarding_profile_info.ack_distribution;
    }
    else if (requested_event == BCM_L2_ADDR_DIST_AGED_OUT_EVENT)
    {
        olp_distribution = event_forwarding_profile_info.delete_distribution;
    }
    else if (requested_event == BCM_L2_ADDR_DIST_REFRESH_EVENT)
    {
        olp_distribution = event_forwarding_profile_info.refresh_distribution;
    }
    else if (requested_event == BCM_L2_ADDR_DIST_STATION_MOVE_EVENT)
    {
        olp_distribution = event_forwarding_profile_info.transplant_distribution;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. One and only one event can be retrieved at the same time.\n");
    }

    /** Set the distribution bits of the event */
    if (GET_MC_DISTRIBUTION_FROM_BUFFER(olp_distribution))
    {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;
    }
    if (GET_CPU_DISTRIBUTION_FROM_BUFFER(olp_distribution))
    {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
    }
    if (GET_CPU_DMA_DISTRIBUTION_FROM_BUFFER(olp_distribution))
    {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER;
    }

    /** In case of no distribution set the no distributer flag */
    if (distribution->flags == orig_distribution_flags)
    {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER;
    }

exit:
    DBAL_FUNC_FREE_VARS SHR_FUNC_EXIT;
}

/**
 * \brief - Update the distribution of the OLP events related to a given vsi
 *
 * \param [in] unit - unit ID
 * \param [in] distribution
 *   * flags - flags has bitmap defined using.
 * BCM_L2_ADDR_DIST_*. The flags are describing to which OLP
 * queues each OLP event is distributed.
 *
 * Flags options:
 * BCM_L2_ADDR_DIST_LEARN_EVENT - learning event.
 * BCM_L2_ADDR_DIST_STATION_MOVE_EVENT  - transplant event.
 * BCM_L2_ADDR_DIST_AGED_OUT_EVENT - aged out event.
 * BCM_L2_ADDR_DIST_REFRESH_EVENT - age refresh event.
 * BCM_L2_ADDR_DIST_ACK_EVENT - acknowledge event.
 *
 * BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER - Send events to the learning distributer which should be a multicast group.
 * BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER - Send events to the CPU using a DSP.
 * BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER - Send events to the DMA that copies them to the CPU's memory.
 *                                            A callback should be registered to handle the events.
 * BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER - Do not send events to any distributer.
 *   * vid - vid is vsi to update.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_msg_distribute_set(
    int unit,
    bcm_l2_addr_distribute_t * distribution)
{
    dnx_event_forwarding_profile_info_t event_forwarding_profile_info;
    uint32 loopback, mc, cpu, cpu_dma;
    uint32 distribution_buffer;
    uint32 vsi;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_msg_distribute_set_verify(unit, distribution));

    vsi = distribution->vid;

    /** Loopback queue is not supported in Jericho2. The loopback queue is never configured. */
    loopback = 0;

    mc = ((distribution->flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER) ? 1 : 0);
    cpu = ((distribution->flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER) ? 1 : 0);
    cpu_dma = ((distribution->flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER) ? 1 : 0);

    distribution_buffer = BUILD_OLP_DISTRIBUTION_BUFFER(loopback, mc, cpu, cpu_dma);

    if ((distribution_buffer == 0) && ((distribution->flags & BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER) == 0))
    {
        /** At least one distribution flag must be set */
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "At least one distribution flag must be set\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }

    event_forwarding_profile_info.learn_distribution =
        ((distribution->flags & BCM_L2_ADDR_DIST_LEARN_EVENT) ? distribution_buffer : DISTRIBUTION_NO_CHANGE);
    event_forwarding_profile_info.ack_distribution =
        ((distribution->flags & BCM_L2_ADDR_DIST_ACK_EVENT) ? distribution_buffer : DISTRIBUTION_NO_CHANGE);
    event_forwarding_profile_info.delete_distribution =
        ((distribution->flags & BCM_L2_ADDR_DIST_AGED_OUT_EVENT) ? distribution_buffer : DISTRIBUTION_NO_CHANGE);
    event_forwarding_profile_info.refresh_distribution =
        ((distribution->flags & BCM_L2_ADDR_DIST_REFRESH_EVENT) ? distribution_buffer : DISTRIBUTION_NO_CHANGE);
    event_forwarding_profile_info.transplant_distribution =
        ((distribution->flags & BCM_L2_ADDR_DIST_STATION_MOVE_EVENT) ? distribution_buffer : DISTRIBUTION_NO_CHANGE);

    /** vsi == -1 means updating the default profile */
    if (vsi != (bcm_vlan_t) - 1)
    {
        SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_update(unit, vsi, &event_forwarding_profile_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_update_default(unit, &event_forwarding_profile_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** See .h for reference */
shr_error_e
dnx_l2_learn_mode_set(
    int unit,
    int bcm_learn_mode)
{
    dbal_enum_value_field_ingress_opportunistic_learning_e opportunistic_learning;
    dbal_enum_value_field_ingress_learning_mode_e ingress_learning;
    dnx_learn_mode_t learn_mode;
    uint32 entry_handle_id;
    uint32 use_learn_extension = FALSE;
    int routed_learn_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_learn_mode_set_verify(unit, bcm_learn_mode));

    opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_PIPE_LEARNING;
    ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_INGRESS_LEARNING;
    learn_mode = LEARN_MODE_DISTRIBUTED;

    switch (bcm_learn_mode)
    {
        case BCM_L2_INGRESS_CENT:
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_INGRESS_LEARNING;
            learn_mode = LEARN_MODE_CENTRALIZED;
            break;
        }
        case (BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU):
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_INGRESS_LEARNING;
            learn_mode = LEARN_MODE_DMA;
            break;
        }
        case BCM_L2_INGRESS_DIST:
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_INGRESS_LEARNING;
            break;
        }
        case BCM_L2_EGRESS_DIST:
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_EGRESS_LEARNING;
            opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING;
            use_learn_extension = TRUE;
            break;
        }
        case BCM_L2_EGRESS_CENT:
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_EGRESS_LEARNING;
            opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING;
            learn_mode = LEARN_MODE_CENTRALIZED;
            use_learn_extension = TRUE;
            break;
        }
        case (BCM_L2_EGRESS_CENT | BCM_L2_LEARN_CPU):
        {
            ingress_learning = DBAL_ENUM_FVAL_INGRESS_LEARNING_MODE_EGRESS_LEARNING;
            opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING;
            learn_mode = LEARN_MODE_DMA;
            use_learn_extension = TRUE;
            break;
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_context_default_sys_header_learn_ext(unit, !use_learn_extension));

    /** Taking a handle of L2_LEARNING_GENERAL_INFO_SW table  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARNING_GENERAL_INFO_SW, &entry_handle_id));

    /** Save the bcm_learn_mode in dbal SW state */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARNING_MODE, INST_SINGLE, bcm_learn_mode);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Reset the learn profiles and forwarding profiles to the default values */
    SHR_IF_ERR_EXIT(dnx_l2_learn_reset_profiles(unit));

    /** Initialize the vsi profiles with their default configurations. */
    SHR_IF_ERR_EXIT(dnx_l2_vsi_learning_profile_init(unit));

    /** Set defaults according to the learn mode */
    SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_init(unit, learn_mode));
    SHR_IF_ERR_EXIT(dnx_aging_profile_init(unit, learn_mode));

    /*
     * Ingress opportunistic learning mode must be set to opportunistic in 2 cases:
     * 1. Routed learning is enabled
     * 2. Egress learning mode is set
     */
    SHR_IF_ERR_EXIT(dnx_switch_control_routed_learn_get(unit, &routed_learn_flags));
    if (routed_learn_flags != 0)
    {
        opportunistic_learning = DBAL_ENUM_FVAL_INGRESS_OPPORTUNISTIC_LEARNING_OPPORTUNISTIC_LEARNING;
    }
    /*
     * Update the opportunistic learning mode (pipe or opportunistic)
     */
    /*
     * Update the ingress learning mode (ingress or egress) and opportunistic learning (pipe or opportunistic)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OPPORTUNISTIC_LEARNING, INST_SINGLE,
                                 opportunistic_learning);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LEARNING_MODE, INST_SINGLE,
                                 ingress_learning);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h for reference
 */
shr_error_e
dnx_l2_learn_mode_get(
    int unit,
    int *bcm_learn_mode)
{
    uint32 entry_handle_id;
    uint32 learn_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle of L2_LEARNING_GENERAL_INFO_SW table in order to get the learn_mode */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARNING_GENERAL_INFO_SW, &entry_handle_id));
    /** Get the learn_mode from dbal SW state */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LEARNING_MODE, INST_SINGLE, &learn_mode);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *bcm_learn_mode = learn_mode;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the OLP registers with its default configuration.
 */
int
dnx_l2_learn_init_olp(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_GENERAL_CONFIGURATION, &entry_handle_id));

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FILTER_DROP_DUPLICATE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FILTER_ENTRY_TTL, INST_SINGLE,
                                 LEARN_FILTER_ENTRY_TTL_DEFAULT);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OLP_DSP_ENGINE_CONFIGURATION, entry_handle_id));

    /** Key fields for cammand destination learning */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION,
                               DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_LEARNING);

    /** Result values for cammand destination learning */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_DSP_SIZE, INST_SINGLE, MIN_DSP_CMD_SIZE_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_DSP_CMD, INST_SINGLE, MAX_NOF_DSP_CMD_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CMD_DELAY, INST_SINGLE,
                                 MAX_DSP_CMD_DELAY_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_GENERATION_ENABLE, INST_SINGLE, 1);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Key fields for cammand destination shadow */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION,
                               DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_SHADOW);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** DSP identifier configurations */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OLP_GENERAL_CONFIGURATION, entry_handle_id));

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_IDENTIFIER_MASK, INST_SINGLE,
                                 DSP_IDENTIFIER_MASK_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_IDENTIFIER_CHECK, INST_SINGLE, 1);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Init OLP for interworking with Jericho */
    SHR_IF_ERR_EXIT(dnx_l2_olp_jr_mode_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the learn limits default configurations.
 */
int
dnx_l2_init_learn_limits(
    int unit)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    /** check limits by default */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_VSI_LIMIT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_MACT_DB_LIMIT, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, DBAL_TABLE_FWD_MACT, &table));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ENTRIES_LIMIT, INST_SINGLE,
                                 table->max_capacity / MACT_EM_NOF_ENTRIES);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_LIMIT_EXCEED_CONFIGURATION, entry_handle_id));
    dbal_entry_key_field8_range_set(unit, entry_handle_id, DBAL_FIELD_IS_HOST_REQUEST, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_CHECK_PASS, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_STATIC, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_IF_EXCEED_MACT_LIMIT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_IF_EXCEED_VSI_LIMIT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the learn key context. Configure profiles for VSIs with properties defining the learn key format and AppDb ID of
 * the VSI.
 */
shr_error_e
dnx_l2_init_learn_key_context(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_KEY_CONSTRUCTION, &entry_handle_id));

    /*
     * SVL
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_SVL);

    /** App DB ID   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_IDX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    /** VLAN source */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_SOURCE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VLAN_SOURCE_VSI);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * IVL VLAN-Edit-VID-1
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_IVL_VLAN_EDIT_VID_1_VSI);

    /** App DB ID   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_IDX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_IVL);

    /** VLAN source */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_SOURCE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VLAN_SOURCE_VLAN_EDIT_VID_1_VSI);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * IVL VLAN-Edit-VID-2
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_IVL_VLAN_EDIT_VID_2_VSI);

    /** App DB ID   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_IDX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_IVL);

    /** VLAN source */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_SOURCE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VLAN_SOURCE_VLAN_EDIT_VID_2_VSI);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * IVL OUTER_VID_VSI
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_IVL_OUTER_VID_VSI);

    /** App DB ID   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_IDX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_IVL);

    /** VLAN source */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_SOURCE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VLAN_SOURCE_OUTER_VID_VSI);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * IVL INNER_VID_VSI
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_IVL_INNER_VID_VSI);

    /** App DB ID   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_IDX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_IVL);

    /** VLAN source */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_SOURCE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VLAN_SOURCE_INNER_VID_VSI);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize the learn payload context.
 * Configure instructions for building the learn payload. Payload source is taken from
 *   the following resources (total of 95 bits):
 * LIF.Learn-Data (48b)
 * Src-System-port (16b)
 * Learn-Payload-Key-Gen-Var (LPKGV) (8b)
 * In-LIF.Global-LIF-ID (22b)
 * In-PP-Port.Learn-LIF (1b)
 */
shr_error_e
dnx_l2_init_learn_payload_context(
    int unit)
{
    uint32 entry_handle_id;
    uint32 learn_ffc_instruction;
    uint32 lpkgv_hw_value_no_outlif, lpkgv_hw_value_single_outlif;
    /*
     * Jericho mode beginning
     * {
     */
    uint32 lpkgv_hw_value_eei_fec;
    int system_headers_mode;
    /*
     * }
     * Jericho mode end
     */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_PAYLOAD_BUILD, &entry_handle_id));

    /*
     * Initialize Learn-Payload-Key-Gen-Var HW values by conerting from the DBAL enum values
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_IRPP_FWD_STAGE_FORMATS,
                     DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_NO_OUTLIF, &lpkgv_hw_value_no_outlif));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_IRPP_FWD_STAGE_FORMATS,
                     DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_SINGLE_OUTLIF,
                     &lpkgv_hw_value_single_outlif));

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_IRPP_FWD_STAGE_FORMATS,
                         DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_EEI_FEC, &lpkgv_hw_value_eei_fec));
    }
    /*
     * }
     * Jericho mode end
     */

    /*
     * MACT Payload Context for Non-Global In-LIF of standard size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Entry-Group[3] := LPKGV[6] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Learn-Strength[0] := LPKGV[7] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value using the DBAL conversion to HW value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_no_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Symmetric Global In-LIF of standard size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Global LIF ID to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_GLOBAL_LIF_ID_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Entry-Group[3] := LPKGV[6] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Learn-Strength[0] := LPKGV[7] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Asymmetric Global In-LIF of standard size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_BASIC);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Learn data[42:21] to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 21));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Entry-Group[3] := LPKGV[6] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Learn-Strength[0] := LPKGV[7] */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Non-Global In-LIF of extended size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Learn data[24:21] to entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET,
                                                    (LEARN_PAYLOAD_ENTRY_GROUP_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 21));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** LPKGV[7] to the MSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Learn data[25] to the LSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 25));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_no_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Symmetric Global In-LIF of extended size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Global LIF ID to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_GLOBAL_LIF_ID_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Learn data[24:21] to entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET,
                                                    (LEARN_PAYLOAD_ENTRY_GROUP_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 21));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** LPKGV[7] to the MSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Learn data[25] to the LSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 25));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 5, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 5, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Asymmetric Global In-LIF of extended size
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_EXTENDED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** Learn data[20:0] to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Learn data[42:21] to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 21));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Learn data[46:43] to entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET,
                                                    (LEARN_PAYLOAD_ENTRY_GROUP_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 43));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** LPKGV[7] to the MSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Learn data[47] to the LSB of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 47));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 5, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 5, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for and Optimized Non-Global In-LIF
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** {Learn data[4:0], Src System Port} to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_SRC_SYSTEM_PORT_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** LPKGV[6] to the first part of entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** Learn data[47:44] to the second part of entry group and
     *  the first part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (4 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 44));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** LPKGV[7] to second part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_no_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Optimized Symmetric Global In-LIF
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** {Learn data[4:0], Src System Port} to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_SRC_SYSTEM_PORT_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Global LIF ID to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_GLOBAL_LIF_ID_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** LPKGV[6] to the first part of entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Learn data[47:44] to the second part of entry group and
     *  the first part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (4 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 44));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** LPKGV[7] to second part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 5, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 5, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * MACT Payload Context for Optimized Asymmetric Global In-LIF
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                               DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_OPTIMIZED);

    /** LPKGV[5:0] to type */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                    (LEARN_PAYLOAD_TYPE_WIDTH - 1), LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

    /** {Learn data[4:0], Src System Port} to destination */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                    (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                    LEARN_PAYLOAD_SOURCE_SRC_SYSTEM_PORT_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

    /** Learn data[26:5] to Out LIF */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_OUT_LIF_OFFSET,
                                                    (LEARN_PAYLOAD_OUT_LIF_WIDTH - 1),
                                                    (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 5));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

    /** LPKGV[6] to the first part of entry group */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET + 3,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

    /** Learn data[47:44] to the second part of entry group and
     *  the first part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET + 1,
                                                    (4 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 44));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

    /** LPKGV[7] to second part of learn strength */
    learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                    (1 - 1), (LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET + 7));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 5, learn_ffc_instruction);

    /** Select which FFCs to use by configuring the KBR */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 5, 1);

    /*
     * Set the Learn-Payload-Key-Gen-Var value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                 lpkgv_hw_value_single_outlif);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Jericho mode beginning
     * {
     */

    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        /*
         * MACT Payload Context for Asymmetric EEI+FEC of standard size
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                                   DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_EXTENDED_EEI);

        /** LPKGV[5:0] to type */
        learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_TYPE_OFFSET,
                                                        (LEARN_PAYLOAD_TYPE_WIDTH - 1),
                                                        LEARN_PAYLOAD_SOURCE_LPKGV_OFFSET);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 0, learn_ffc_instruction);

        /** Learn data[20:0] to destination */
        learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_DESTINATION_OFFSET,
                                                        (LEARN_PAYLOAD_DESTINATION_WIDTH - 1),
                                                        LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 1, learn_ffc_instruction);

        /** Learn data[44:21] to EEI */
        learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_EEI_OFFSET,
                                                        (LEARN_PAYLOAD_EEI_WIDTH - 1),
                                                        (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 21));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 2, learn_ffc_instruction);

        /** Learn data[46:45] to entry-group[1:0] */
        learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_ENTRY_GROUP_OFFSET,
                                                        (2 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 45));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 3, learn_ffc_instruction);

        /** Learn-Strength[0] := Learn data[47] */
        learn_ffc_instruction = BUILD_LEARN_PAYLOAD_FFC(LEARN_PAYLOAD_STRENGTH_OFFSET,
                                                        (1 - 1), (LEARN_PAYLOAD_SOURCE_LEARN_DATA_OFFSET + 47));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FFC_INSTRUCTION, 4, learn_ffc_instruction);

        /** Select which FFCs to use by configuring the KBR */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 0, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 1, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 2, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 3, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_KBR_VALID, 4, 1);

        /*
         * Set the Learn-Payload-Key-Gen-Var value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_KEY_GEN_VAR, INST_SINGLE,
                                     lpkgv_hw_value_eei_fec);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                     DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

        /** setting the entry with the default values */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    /*
     * }
     * Jericho mode end
     */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Build an ITMH header for the DSPs generated by the OLP.
 *
 * \param [in] unit - unit ID
 * \param [in] learn_msg_config - configuration for building the ITMH.
 * \param [out] encoded_itmh_header - buffer for filling the built ITMH.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_build_itmh_header_for_olp(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config,
    uint32 *encoded_itmh_header)
{
    uint32 field_in_struct_val;
    bcm_gport_t gport;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** ITMH_EXTENSION_PRESENT */
    field_in_struct_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_ITMH_EXTENSION_PRESENT,
                                                    &field_in_struct_val, encoded_itmh_header));
    /** TC */
    field_in_struct_val = learn_msg_config->priority;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_TC, &field_in_struct_val, encoded_itmh_header));

    /** SNIF_COMMAND_ID */
    field_in_struct_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_SNIF_COMMAND_ID,
                                                    &field_in_struct_val, encoded_itmh_header));

    /** DESTINATION */

    if (learn_msg_config->flags & BCM_L2_LEARN_MSG_DEST_MULTICAST)
    {
        /** Destination is multicast */
        BCM_GPORT_MCAST_SET(gport, learn_msg_config->dest_group);
    }
    else
    {
        /** Gport destination */
        gport = learn_msg_config->dest_port;
    }

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport, &destination));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_DESTINATION, &destination, encoded_itmh_header));

    /** DP  */
    field_in_struct_val = learn_msg_config->color;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_DP, &field_in_struct_val, encoded_itmh_header));

    /** INBOUND_MIRROR_DISABLE  */
    field_in_struct_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_INBOUND_MIRROR_DISABLE,
                                                    &field_in_struct_val, encoded_itmh_header));

    /** PPH_TYPE  */
    field_in_struct_val = (uint32) DBAL_ENUM_FVAL_PPH_TYPE_NONE;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_PPH_TYPE, &field_in_struct_val, encoded_itmh_header));

    /** INJECTED_AS_EXTENSION_PRESENT  */
    field_in_struct_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_INJECTED_AS_EXTENSION_PRESENT,
                                                    &field_in_struct_val, encoded_itmh_header));

    /** RESERVED  */
    field_in_struct_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_ITMH_BASE,
                                                    DBAL_FIELD_RESERVED, &field_in_struct_val, encoded_itmh_header));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Decide which headers should be in the DSPs of the OLP \ 
 * Out from the OLP: \ 
 * Jer2, INJECTED_2 - DSPoETHoUDHoITMHoPTCH2 
 * Jer mode, TM - DSPoETHoUDHoITMH  (UDH existance according to soc properties) \ 
 *  \
 * Into the OLP: \ 
 * Jer2, RAW - DSPoETH \ 
 * Jer mode, RAW - DSPoETHoUDH (UDH existance according to soc properties) \ 
 * Jer mode, TM - DSPoETHoUDHoOTMH (UDH existance according to soc properties)
 * 
 * \param [in] unit - unit id 
 * \param [in] olp_port_type - type of the OLP port (BCM_SWITCH_PORT_HEADER_TYPE_ETH, BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2 etc.)
 * \param [in] learn_msg_config - OLP configuration struct
 * \param [out] olp_headers_bitmap - bitmap of headers to construct (OLP_HEADER_BITMAP_*)
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_get_required_olp_headers(
    int unit,
    int olp_port_type,
    bcm_l2_learn_msgs_config_t * learn_msg_config,
    uint32 *olp_headers_bitmap)
{
    int injection_with_user_header_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    *olp_headers_bitmap = 0;

    if (learn_msg_config->flags & BCM_L2_LEARN_MSG_ETH_ENCAP)
    {
        *olp_headers_bitmap |= OLP_HEADER_BITMAP_ETHERNET;

        if (BCM_VLAN_VALID(learn_msg_config->vlan))
        {
            *olp_headers_bitmap |= OLP_HEADER_BITMAP_VLAN;
        }
    }

    

    if (injection_with_user_header_enable)
    {
        *olp_headers_bitmap |= OLP_HEADER_BITMAP_UDH;
    }

    *olp_headers_bitmap |= OLP_HEADER_BITMAP_ITMH;
    *olp_headers_bitmap |= OLP_HEADER_BITMAP_PTCH2;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the type of packet received by the OLP
 *
 * \param [in] unit - unit ID
 * \param [in] learn_msg_config - configuration parameters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_configure_olp_received_packets(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config)
{
    uint32 entry_handle_id;
    uint32 olp_headers_bitmap;

    /** Points to the dsp_identifier_id in the DSP */
    uint32 identifier_bytes_to_skip = 0;

    /** Allows the OLP validate that the DSP is a valid OLP packet */
    uint32 dsp_identifier_id = 0;

    /** holds the size of header that comes before the OLP commands */
    uint32 first_dsp_cmd_offset = 0;

    bcm_port_t olp_pp_port;
    int olp_out_port_header_type;
    int olp_in_port_header_type;
    uint8 enable_identifier_check = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the OLP port number and type */
    SHR_IF_ERR_EXIT(dnx_l2_olp_port_get(unit, &olp_pp_port, &olp_in_port_header_type, &olp_out_port_header_type));

    SHR_IF_ERR_EXIT(dnx_l2_get_required_olp_headers
                    (unit, olp_in_port_header_type, learn_msg_config, &olp_headers_bitmap));

    if (learn_msg_config->flags & BCM_L2_LEARN_MSG_ETH_ENCAP)
    {
        enable_identifier_check = 1;

        /** DSP check is done only when ether type for DSP exists. In this case the Ether type is used as verification bits */
        dsp_identifier_id = learn_msg_config->ether_type;

        /*
         *  first_dsp_cmd_offset is assuming that the DSP is only changing its header from ITMH to OTMH. (in case of a TM port)
         *  The Ethernet part stays untouched
         */
        if (BCM_VLAN_VALID(learn_msg_config->vlan))
        {
            first_dsp_cmd_offset = DSP_ETH_HEADER_SIZE + DSP_VLAN_HEADER_SIZE;
        }
        else
        {
            first_dsp_cmd_offset = DSP_ETH_HEADER_SIZE;
        }
    }

    if (olp_out_port_header_type == BCM_SWITCH_PORT_HEADER_TYPE_TM)
    {
        first_dsp_cmd_offset += DSP_OTMH_HEADER_SIZE;
    }

    if (olp_headers_bitmap & OLP_HEADER_BITMAP_UDH)
    {
        first_dsp_cmd_offset += 1;
    }

    if (enable_identifier_check)
    {
        /** Point to the ether type which is the last field before the OLP command */
        identifier_bytes_to_skip = first_dsp_cmd_offset - 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_GENERAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_IDENTIFIER_ID, INST_SINGLE, dsp_identifier_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_IDENTIFIER_BYTES_TO_SKIP, INST_SINGLE,
                                 identifier_bytes_to_skip);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_FIRST_CMD_OFFSET, INST_SINGLE,
                                 first_dsp_cmd_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DSP_IDENTIFIER_CHECK, INST_SINGLE,
                                enable_identifier_check);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Use the headers bitmap to select the result type for the OLP headers configuration
 * 
 * \param [in] unit - unit id
 * \param [in] olp_headers_bitmap - bitmap with the possible header types (OLP_HEADER_BITMAP_*) 
 * \param [out] res_type_idx - selected result type for the register OLP_HEADER_CONFIGURATION
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_select_dsp_result_type(
    int unit,
    uint32 olp_headers_bitmap,
    dbal_enum_value_result_type_olp_header_configuration_e * res_type_idx)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (olp_headers_bitmap)
    {
        case (OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH | OLP_HEADER_BITMAP_PTCH2):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_UDH_O_ITMH_PTCH2;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH | OLP_HEADER_BITMAP_PTCH2):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_O_UDH_ITMH_PTCH2;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_VLAN | OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH | OLP_HEADER_BITMAP_PTCH2):
            *res_type_idx =
                DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_UDH_ITMH_PTCH2;
            break;
        case (OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_UDH_O_ITMH;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_O_UDH_ITMH;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_VLAN | OLP_HEADER_BITMAP_UDH | OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_UDH_ITMH;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_ITMH | OLP_HEADER_BITMAP_PTCH2):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_O_ITMH_PTCH2;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_VLAN | OLP_HEADER_BITMAP_ITMH | OLP_HEADER_BITMAP_PTCH2):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_ITMH_PTCH2;
            break;
        case (OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ITMH;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETHERNET_O_ITMH;
            break;
        case (OLP_HEADER_BITMAP_ETHERNET | OLP_HEADER_BITMAP_VLAN | OLP_HEADER_BITMAP_ITMH):
            *res_type_idx = DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_ITMH;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "OLP header combination is not supported\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the DSPs going out of the OLP
 *
 * \param [in] unit - unit id
 * \param [in] learn_msg_config - user parameters for building the DSPs from the OLP
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_configure_olp_transmit_packets(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config)
{
    uint32 entry_handle_id;
    uint32 encoded_itmh_header[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint8 olp_header_padding[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    int dsp_header_size = 0;
    uint32 field_val;
    int olp_pp_port;
    int olp_in_port_header_type;
    int olp_out_port_header_type;
    uint32 olp_headers_bitmap;

    dbal_enum_value_field_olp_command_destination_e command_destination;
    dbal_enum_value_result_type_olp_header_configuration_e res_type_idx = DBAL_NOF_RESULT_TYPE_OLP_HEADER_CONFIGURATION;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get OLP port and header types */
    SHR_IF_ERR_EXIT(dnx_l2_olp_port_get(unit, &olp_pp_port, &olp_in_port_header_type, &olp_out_port_header_type));

    SHR_IF_ERR_EXIT(dnx_l2_get_required_olp_headers
                    (unit, olp_in_port_header_type, learn_msg_config, &olp_headers_bitmap));

    SHR_IF_ERR_EXIT(dnx_l2_select_dsp_result_type(unit, olp_headers_bitmap, &res_type_idx));

    SHR_IF_ERR_EXIT(dnx_l2_build_itmh_header_for_olp(unit, learn_msg_config, &(encoded_itmh_header[0])));

    /** Choose which queue should be updated (shadow or learning) */
    if (learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING)
    {
        command_destination = DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_LEARNING;
    }
    else
    {
        command_destination = DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_SHADOW;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_HEADER_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION, command_destination);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type_idx);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_OLP_HEADER_PADDING, INST_SINGLE,
                                    olp_header_padding);

    /** Build Ethernet header if needed */
    if (olp_headers_bitmap & OLP_HEADER_BITMAP_ETHERNET)
    {
        dsp_header_size += DSP_ETH_HEADER_SIZE;
        /** Check of VLAN is needed in the ethernet */
        if (olp_headers_bitmap & OLP_HEADER_BITMAP_VLAN)
        {
            dsp_header_size += DSP_VLAN_HEADER_SIZE;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, learn_msg_config->tpid);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE,
                                         learn_msg_config->vlan_prio);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VID1, INST_SINGLE, learn_msg_config->vlan);
        }

        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA, INST_SINGLE,
                                        learn_msg_config->dst_mac_addr);
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_SA, INST_SINGLE,
                                        learn_msg_config->src_mac_addr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, INST_SINGLE,
                                     learn_msg_config->ether_type);
    }

    if (olp_headers_bitmap & OLP_HEADER_BITMAP_ITMH)
    {
        dsp_header_size += DSP_ITMH_HEADER_SIZE;
        /** Write ITMH */
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_BASE, INST_SINGLE,
                                         &(encoded_itmh_header[0]));
    }

    /** Write the OLP port to PTCH2 */
    if (olp_headers_bitmap & OLP_HEADER_BITMAP_PTCH2)
    {
        dsp_header_size += DSP_PTCH_HEADER_SIZE;
        field_val = olp_pp_port;
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PTCH_2, INST_SINGLE, &field_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (olp_headers_bitmap & OLP_HEADER_BITMAP_UDH)
    {
        dsp_header_size += DSP_UDH_HEADER_SIZE;
    }

    /*
     * Configure the size of the transmitted DSP headers
     */
    if (dsp_header_size > 63 || dsp_header_size < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DSP header size is higher than possible. header_size %d, header_type %d.\n",
                     dsp_header_size, res_type_idx);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_DSP_ENGINE_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION, command_destination);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_HEADER_SIZE, INST_SINGLE, dsp_header_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - verification function for bcm_dnx_l2_learn_msgs_config_set
 *
 * \param [in] unit - unit ID
 * \param [in] learn_msg_config - parameters for configuring the DSP's header. See l2.h for more details.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_learn_msgs_config_set_verify(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(learn_msg_config, _SHR_E_PARAM, "learn_msg_config");

    /*
     * has to update at least one message queue
     */
    if (!(learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) && !(learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "At least one of the flags BCM_L2_LEARN_MSG_SHADOW or BCM_L2_LEARN_MSG_LEARNING should be set");
    }

    if (learn_msg_config->color > 3)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Drop presedence - learn_msg_config->color should be in the range 0-3");
    }

    if (learn_msg_config->priority > 7)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority should be in the range 0-7");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure headers of OLP messages. OLP messages are always sent with ITMH header and can also have Ethernet header with or
 *        without VLAN tag.
 *
 * \param [in] unit - unit ID
 * \param [in] learn_msg_config - parameters for configuring the DSP's header. \
 *  * flags - flags describing which OLP queue to configure and other instructions for the header's construction. \
 * BCM_L2_LEARN_MSG_DEST_MULTICAST - destination of the learning message is a multicast group. \
 *  'dest_group' in use. If the flag is not present then the destination is uc and 'dest_port' in use. \
 * BCM_L2_LEARN_MSG_ETH_ENCAP - OLP message are encapsulated in Etherenet header. Use the specified Ethernet parameters. \
 * BCM_L2_LEARN_MSG_LEARNING - configure OLP messages going to the OLP's distribution. (HW learning) \
 * BCM_L2_LEARN_MSG_SHADOW - configure OLP messages to the CPU. (BCM_L2_LEARN_MSG_SHADOW or BCM_L2_LEARN_MSG_LEARNING must be set) \
 *  * dest_port - destination port. \
 *  * dest_group - destination group used when BCM_L2_LEARN_MSG_DEST_MULTICAST is set. \
 *  * vlan - vlan identifier. Set to BCM_VLAN_INVALID for untagged Ethernet. \
 *  * tpid - vlan TPID. \
 *  * vlan_prio - vlan priority. \
 *  * ether_type - type for Ethernet II frame or 802.1Q tag. \
 *  * src_mac_addr - source MAC address. \
 *  * dst_mac_addr - destination MAC address. \
 *  * priority - traffic class, Range: 0..7 \
 *  * color - drop precedence, Range: 0..3
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_learn_msgs_config_set(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_learn_msgs_config_set_verify(unit, learn_msg_config));

    SHR_IF_ERR_EXIT(dnx_l2_configure_olp_transmit_packets(unit, learn_msg_config));

    SHR_IF_ERR_EXIT(dnx_l2_configure_olp_received_packets(unit, learn_msg_config));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the configured parameters of the OLP DSP header. Set the flags parameter to BCM_L2_LEARN_MSG_LEARNING or
 *        BCM_L2_LEARN_MSG_SHADOW and receive the configured parameters of this queue.
 *
 * \param [in] unit - unit ID
 * \param [in,out] learn_msg_config - parameters configuring headers of OLP messages (DSP messages).
 *  * flags - set BCM_L2_LEARN_MSG_LEARNING or BCM_L2_LEARN_MSG_SHADOW to select which parameters to receive. \
 * BCM_L2_LEARN_MSG_DEST_MULTICAST - destination of the learning message is a multicast group. \
 *  'dest_group' in use. If the flag is not present then the destination is uc and 'dest_port' in use. \
 * BCM_L2_LEARN_MSG_ETH_ENCAP - OLP message are encapsulated in Etherenet header. Use the specified Ethernet parameters. \
 * BCM_L2_LEARN_MSG_LEARNING - configure OLP messages going to the OLP's distribution. (HW learning) \
 * BCM_L2_LEARN_MSG_SHADOW - configure OLP messages to the CPU. (BCM_L2_LEARN_MSG_SHADOW or BCM_L2_LEARN_MSG_LEARNING must be set) \
 *  * dest_port - destination port. \
 *  * dest_group - destination group used when BCM_L2_LEARN_MSG_DEST_MULTICAST is set. \
 *  * vlan - vlan identifier. Set to BCM_VLAN_INVALID for untagged Ethernet. \
 *  * tpid - vlan TPID. \
 *  * vlan_prio - vlan priority. \
 *  * ether_type - type for Ethernet II frame or 802.1Q tag. \
 *  * src_mac_addr - source MAC address. \
 *  * dst_mac_addr - destination MAC address. \
 *  * priority - traffic class, Range: 0..7 \
 *  * color - drop precedence, Range: 0..3 \
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_learn_msgs_config_get(
    int unit,
    bcm_l2_learn_msgs_config_t * learn_msg_config)
{
    uint32 entry_handle_id;
    uint32 result_type;
    uint32 itmh_header[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_in_struct_val;
    uint32 field_val;
    uint32 destination = 0;
    uint32 dest_val;
    dbal_fields_e dbal_dest_type;
    bcm_gport_t gport;
    uint32 command_destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Choose which queue should be updated (shadow or learning) */
    if (learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING)
    {
        command_destination = DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_LEARNING;
    }
    else
    {
        command_destination = DBAL_ENUM_FVAL_OLP_COMMAND_DESTINATION_SHADOW;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_HEADER_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION, command_destination);

    /** Fetch all the fields and populate the input struct */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    /** Fill the input struct according to the result type */
    switch (result_type)
    {
        case DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_UDH_ITMH_PTCH2:
        case DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_WITH_VLAN_O_ITMH_PTCH2:
        {
            /** Populate VLAN TAG info */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &field_val));
            learn_msg_config->vlan_prio = (uint16) field_val;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, &field_val));
            learn_msg_config->tpid = (uint16) field_val;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID1, INST_SINGLE, &field_val));
            learn_msg_config->vlan = (bcm_vlan_t) field_val;

            /** No break in purpose */
        }
        case DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_O_ITMH_PTCH2:
        case DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_ETH_O_UDH_ITMH_PTCH2:
        {
            /** Populate ETHERNET info */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_SA, INST_SINGLE, learn_msg_config->src_mac_addr));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_DA, INST_SINGLE, learn_msg_config->dst_mac_addr));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, INST_SINGLE, &field_val));
            learn_msg_config->ether_type = (uint16) field_val;

            learn_msg_config->flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;

            /** No break in purpose */
        }
        case DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_UDH_O_ITMH_PTCH2:
        {
            /** Populate ITMH info */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_ITMH_BASE, INST_SINGLE, itmh_header));

            /** priority */
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_ITMH_BASE,
                                                            DBAL_FIELD_TC, &field_in_struct_val, itmh_header));
            learn_msg_config->priority = (uint8) field_in_struct_val;

            /** color */
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_ITMH_BASE,
                                                            DBAL_FIELD_DP, &field_in_struct_val, itmh_header));
            learn_msg_config->color = (uint8) field_in_struct_val;

            /** dest_group or dest_port according to destination type */
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_ITMH_BASE,
                                                            DBAL_FIELD_DESTINATION, &destination, itmh_header));

            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                            (unit, DBAL_FIELD_DESTINATION, destination, &dbal_dest_type, &dest_val));

            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, destination, &gport));

            if (dbal_dest_type == DBAL_FIELD_MC_ID)
            {
                learn_msg_config->flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST;
                learn_msg_config->dest_group = BCM_GPORT_MCAST_GET(gport);
            }
            else
            {
                learn_msg_config->dest_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong result type for OLP_HEADER_CONFIGURATION");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verification function for bcm_dnx_l2_addr_register
 *
 * \param [in] unit - Relevant unit
 * \param [in] callback - Callback function that will be called when age out/transplant or learn event raised
 * \param [in] userdata - Context to be added as a parameter when the callback is called
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_addr_register_verify(
    int unit,
    bcm_l2_addr_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(callback, _SHR_E_PARAM, "callback");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register a callback for handling OLP events.
 *    Register a callback routine for BCM L2 subsystem.
 *    It will be called whenever a MAC entry is learnt/aged out or transplanted.
 *
 * \param [in] unit - Relevant unit
 * \param [in] callback - Callback function that will be called when age out/transplant or learn event raised
 * \param [in] userdata - Context to be added as a parameter when the callback is called
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_register(
    int unit,
    bcm_l2_addr_callback_t callback,
    void *userdata)
{

    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_register_verify(unit, callback, userdata));

    /** Check if given function is already registered (with the same data) */
    for (i = 0; i < DNX_L2_CB_MAX; i++)
    {
        if ((dnx_l2_cbs[unit].entry[i].callback == callback) && (dnx_l2_cbs[unit].entry[i].userdata == userdata))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The callback is already registered!");
        }
    }

    if (dnx_l2_cbs[unit].count >= DNX_L2_CB_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum number of registered callbacks is %d", DNX_L2_CB_MAX);
    }

    /** Add to the list of callbacks. Increase counter. */
    for (i = 0; i < DNX_L2_CB_MAX; i++)
    {
        if (dnx_l2_cbs[unit].entry[i].callback == NULL)
        {
            dnx_l2_cbs[unit].entry[i].callback = callback;
            dnx_l2_cbs[unit].entry[i].userdata = userdata;
            dnx_l2_cbs[unit].count++;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verification function for bcm_dnx_l2_addr_unregister
 *
 * \param [in] unit - Relevant unit
 * \param [in] callback - Callback function that should be unregistered. It is called when age out/transplant or learn event raised
 * \param [in] userdata - Context that is added as a parameter when the callback is called
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_addr_unregister_verify(
    int unit,
    bcm_l2_addr_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(callback, _SHR_E_PARAM, "callback");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback for handling OLP events.
 *     Unregister a callback routine for BCM L2 subsystem
 *     that is called whenever a MAC entry is learnt/aged out or transplanted.
 *
 * \param [in] unit - Relevant unit
 * \param [in] callback - Callback function that should be unregistered. It is called when age out/transplant or learn event raised
 * \param [in] userdata - Context that is added as a parameter when the callback is called
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_addr_unregister(
    int unit,
    bcm_l2_addr_callback_t callback,
    void *userdata)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_addr_unregister_verify(unit, callback, userdata));

    if (dnx_l2_cbs[unit].count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The callback doesn't exist!");
    }

    for (i = 0; i < DNX_L2_CB_MAX; i++)
    {
        if (dnx_l2_cbs[unit].entry[i].callback == callback && dnx_l2_cbs[unit].entry[i].userdata == userdata)
        {
            dnx_l2_cbs[unit].entry[i].callback = NULL;
            dnx_l2_cbs[unit].entry[i].userdata = NULL;
            dnx_l2_cbs[unit].count--;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
