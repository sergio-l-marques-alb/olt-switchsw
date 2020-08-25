/**
 * \file switch_load_balancing.c
 *
 * Load balancing utilities, for DNX, mainly related to BCM APIs 
 *
 * Purpose:
 *   Utilities related to both logical LB-related operations and physical access
 *   with load balancing HW.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/*
 *  Exported functions have their descriptions in the switch_load_balancing.h file.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LB
/*
 * Includes
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm/switch.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * An MPLS label (20 bits) full mask
 */
#define SWITCH_LB_MPLS_LABEL_FULL_MASK SAL_UPTO_BIT(20)

/*
 * }
 */
/*
 * See switch_load_balancing.h file
 */
shr_error_e
dnx_switch_load_balancing_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    {
        /*
         * Initialize IPPB_MPLS_RESERVED_LABEL_TYPES
         */
        dnx_switch_lb_mpls_reserved_t mpls_reserved;

        mpls_reserved.reserved_current_label_indication =
            dnx_data_switch.load_balancing.initial_reserved_label_get(unit);
        mpls_reserved.reserved_current_label_force_layer =
            dnx_data_switch.load_balancing.initial_reserved_label_force_get(unit);
        mpls_reserved.reserved_next_label_indication =
            dnx_data_switch.load_balancing.initial_reserved_next_label_get(unit);
        mpls_reserved.reserved_next_label_valid = mpls_reserved.reserved_next_label_indication;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_set(unit, &mpls_reserved));
    }
    {
        /*
         * Initialize IPPB_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG
         */
        dnx_switch_lb_mpls_identification_t mpls_identification;

        mpls_identification.mpls_identification = 0;
        /*
         * Set bits corresponding to supported MPLS protocols. See dbal_enum_value_field_current_protocol_type_e.
         */
        mpls_identification.mpls_identification |= SAL_BIT(DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS);
        mpls_identification.mpls_identification |= SAL_BIT(DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA);
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_set(unit, &mpls_identification));
    }
    {
        /*
         * Initialize crc functions for the various load-balancing-clients.
         * Selection of crc-function is arbitrary and we only need to make sure
         * the same crc function is not selected twice.
         * There are, currently, 5 clients and 8 crc functions.
         */
        uint32 phys_lb_client;
        dnx_switch_lb_tcam_key_t tcam_key;
        dnx_switch_lb_tcam_result_t tcam_result;
        dnx_switch_lb_valid_t tcam_valid;
        int index;

        index = 0;
        /*
         * Set all masks to '0' so that all searches end up on this default (and single) tcam entry.
         */
        tcam_key.lb_destination = 0;
        tcam_key.lb_destination_mask = 0;
        tcam_key.outlif_0 = 0;
        tcam_key.outlif_0_mask = 0;

        for (phys_lb_client = 0; phys_lb_client < dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
             phys_lb_client++)
        {
            /*
             * Arbitrarily select crc-function to be the same as physical id
             * of client.
             */
            tcam_result.client_x_crc_16_sel[phys_lb_client] = phys_lb_client;
        }
        tcam_valid = 1;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_set(unit, index, &tcam_key, &tcam_result, tcam_valid));
    }

    /*
     * This is a WA that forces all the MPLS labels (terminated or not) to participate in the hash of the LB keys to resolve an
     * HW issue where none of the labels are taken into the hashing of the LB keys.
     */
    if (dnx_data_switch.hw_bug.feature_get(unit, dnx_data_switch_hw_bug_mpls_labels_included_in_hash))
    {
        uint32 entry_handle_id;
        /*
         * Create handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_MPLS_FORCE_LABELS_HL_TCAM, &entry_handle_id));
        /*
         * Set access ID (index), this TCAM has only one valid entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));

        /*
         * Set a full mask and actions such that all the MPLS labels will be forced into the hash
         */
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LABEL, 0, SWITCH_LB_MPLS_LABEL_FULL_MASK);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LABEL_INDICATION, INST_SINGLE, TRUE);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE, TRUE);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LABEL_INDICATION, INST_SINGLE, FALSE);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LABEL_VALID, INST_SINGLE, FALSE);

        /*
         * Set valid bit
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
        /*
         * Commit
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See switch_load_balancing.h file
 */
shr_error_e
dnx_switch_load_balancing_module_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See switch_load_balancing.h file
 */
shr_error_e
dnx_switch_control_t_name(
    int unit,
    bcm_switch_control_t switch_control,
    char **name_p)
{
    /**
     * Array of strings identifying, each, a value in the enumeration 'bcm_switch_control_t'.
     * Note that the strings omit the prefix 'bcmSwitch' which is common to all enum values.
     */
    static char *dnx_switch_control_names[] = {
        BCM_SWITCH_CONTROL_STR
    };
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name_p, _SHR_E_PARAM, "name_p");
    if (COUNTOF(dnx_switch_control_names) != bcmSwitch__Count)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: NAMES ARRAY OUT OF SYNC (num elements on BCM_SWITCH_CONTROL_STR (%d) is not equal to num elements on bcm_switch_control_t (%d)\n",
                     COUNTOF(dnx_switch_control_names), bcmSwitch__Count);
    }
    if (bcmSwitch__Count <= switch_control)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input switch control (%d) is higher than allowed (%d)\n", switch_control, bcmSwitch__Count);
    }
    *name_p = dnx_switch_control_names[switch_control];
exit:
    SHR_FUNC_EXIT;
}
/*
 * See switch_load_balancing.h file
 */
shr_error_e
dnx_switch_control_t_value(
    int unit,
    char *switch_control_name_p,
    uint32 *value_p)
{
    uint32 switch_control_index;
    char *name_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(switch_control_name_p, _SHR_E_PARAM, "switch_control_name_p");
    SHR_NULL_CHECK(value_p, _SHR_E_PARAM, "value_p");
    /*
     * Silence potential compiler's complaint
     */
    *value_p = -1;
    /*
     * Arbitrarily limit the number of characters to compare on any element of 'Dnx_switch_control_names'.
     * just choose an arbitrary number that makes sense.
     */
    for (switch_control_index = 0; switch_control_index < bcmSwitch__Count; switch_control_index++)
    {
        SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, switch_control_index, &name_p));
        if (sal_strncasecmp(name_p, switch_control_name_p, SWITCH_CONTROL_NAME_LIMIT) == 0)
        {
            *value_p = switch_control_index;
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_lb_physical_client_to_logical(
    int unit,
    uint32 physical_client,
    bcm_switch_control_t * logical_client_p)
{
    bcm_switch_control_t logical_client;
    uint32 client_hw_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(logical_client_p, _SHR_E_PARAM, "logical_client_p");
    if (physical_client >= dnx_data_switch.load_balancing.nof_lb_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "physical_client (%d) is out of range. Should be between %d and %d.\r\n",
                     physical_client, 0, dnx_data_switch.load_balancing.nof_lb_clients_get(unit) - 1);
    }
    client_hw_id = physical_client;
    logical_client = dnx_data_switch.load_balancing.lb_clients_get(unit, client_hw_id)->client_logical_id;
    *logical_client_p = logical_client;
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_lb_logical_client_to_physical(
    int unit,
    bcm_switch_control_t logical_client_in,
    uint32 *physical_client_p)
{
    bcm_switch_control_t logical_client;
    uint32 client_hw_id, client_hw_id_max;
    int found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(physical_client_p, _SHR_E_PARAM, "physical_client_p");
    client_hw_id_max = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    found = FALSE;
    for (client_hw_id = 0; client_hw_id < client_hw_id_max; client_hw_id++)
    {
        logical_client = dnx_data_switch.load_balancing.lb_clients_get(unit, client_hw_id)->client_logical_id;
        if (logical_client == logical_client_in)
        {
            /*
             * At this point, a match was found. The value of 'client_hw_id'
             * is the matching physical client id.
             */
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n"
                     "Input logical_client (%d) is not marked as a load_balancing client. No match. Quit.\r\n",
                     logical_client_in);
    }
    /*
     * Match was successful. 'client_hw_id' is the physical client.
     */
    *physical_client_p = client_hw_id;
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_lb_tcam_key_t_init(
    int unit,
    dnx_switch_lb_tcam_key_t * tcam_key_p)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(tcam_key_p, 0, sizeof(*tcam_key_p));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_lb_tcam_result_t_init(
    int unit,
    dnx_switch_lb_tcam_result_t * tcam_result_p)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(tcam_result_p, 0, sizeof(*tcam_result_p));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_lsms_crc_select_set(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p)
{
    uint32 entry_handle_id;
    uint32 max_client;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lsms_crc_select_p, _SHR_E_PARAM, "lsms_crc_select_p");
    /*
     * Verify that the index identifier of the 'client' is within range.
     * (It would anyway be failed by DBAL, but, just to make sure)
     */
    max_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    if (lsms_crc_select_p->lb_client >= max_client)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of load balancing client on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", lsms_crc_select_p->lb_client, max_client - 1);
    }
    /*
     * Verify that the bitmap identifier of the 'layer records' is within range.
     * (It would anyway be failed by DBAL, but, just to make sure)
     */
    if ((lsms_crc_select_p->lb_selection_bit_map &
         SAL_FROM_BIT(dnx_data_switch.load_balancing.nof_layer_records_from_parser_get(unit))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Bit selection input (0x%04X) extends beyond max bit allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n",
                     lsms_crc_select_p->lb_selection_bit_map,
                     dnx_data_switch.load_balancing.nof_layer_records_from_parser_get(unit));
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CLIET_FWD_BIT_SEL, &entry_handle_id));
    /*
     * Set the key.
     */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LB_CLIENT, lsms_crc_select_p->lb_client);
    /*
     * Set the LS/MB selection for that client (on all layers) - 'LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP_N'
     */
    dbal_entry_value_field16_set(unit, entry_handle_id,
                                 DBAL_FIELD_LB_SELECTION_BIT_MAP, INST_SINGLE, lsms_crc_select_p->lb_selection_bit_map);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_lsms_crc_select_get(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p)
{
    uint32 entry_handle_id;
    uint32 max_client;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lsms_crc_select_p, _SHR_E_PARAM, "lsms_crc_select_p");
    /*
     * Verify that the index identifier of the 'client' is within range.
     * (It would anyway be failed by DBAL, but, just to make sure)
     */
    max_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    if (lsms_crc_select_p->lb_client >= max_client)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of load balancing client on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", lsms_crc_select_p->lb_client, max_client - 1);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CLIET_FWD_BIT_SEL, &entry_handle_id));
    /*
     * Set the key.
     */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LB_CLIENT, lsms_crc_select_p->lb_client);
    /*
     * Get the LS/MB selection for that client (on all layers) - 'LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP_N'
     */
    dbal_value_field16_request(unit, entry_handle_id,
                               DBAL_FIELD_LB_SELECTION_BIT_MAP, INST_SINGLE,
                               &(lsms_crc_select_p->lb_selection_bit_map));
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_crc_seed_set(
    int unit,
    dnx_switch_lb_crc_seed_t * crc_seed_p)
{
    uint32 entry_handle_id;
    uint32 max_crc_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(crc_seed_p, _SHR_E_PARAM, "crc_seed_p");
    /*
     * Verify that the index of the crc function is within range.
     * (It would anyway be failed by DBAL, but, just to make sure)
     */
    max_crc_index = dnx_data_switch.load_balancing.nof_crc_functions_get(unit);
    if (crc_seed_p->lb_crc_function_index >= max_crc_index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of crc function on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", crc_seed_p->lb_crc_function_index, max_crc_index - 1);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SEEDS, &entry_handle_id));
    /*
     * Set the key.
     */
    dbal_entry_key_field16_set(unit,
                               entry_handle_id, DBAL_FIELD_LB_CRC_FUNCTION_INDEX, crc_seed_p->lb_crc_function_index);
    /*
     * Set the seed for that crc function - 'MPLS_CRC_16_SEED_FUNCION_*'
     */
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED, INST_SINGLE, crc_seed_p->lb_crc_seed);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_crc_seed_get(
    int unit,
    dnx_switch_lb_crc_seed_t * crc_seed_p)
{
    uint32 entry_handle_id;
    uint32 max_crc_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(crc_seed_p, _SHR_E_PARAM, "crc_seed_p");
    /*
     * Verify that the index of the crc function is within range.
     * (It would anyway be failed by DBAL, but, just to make sure)
     */
    max_crc_index = dnx_data_switch.load_balancing.nof_crc_functions_get(unit);
    if (crc_seed_p->lb_crc_function_index >= max_crc_index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of crc function on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", crc_seed_p->lb_crc_function_index, max_crc_index - 1);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SEEDS, &entry_handle_id));
    /*
     * Set the key.
     */
    dbal_entry_key_field16_set(unit,
                               entry_handle_id, DBAL_FIELD_LB_CRC_FUNCTION_INDEX, crc_seed_p->lb_crc_function_index);
    /*
     * Get the seed for that crc function - 'MPLS_CRC_16_SEED_FUNCION_*'
     */
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED, INST_SINGLE, &(crc_seed_p->lb_crc_seed));
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_mpls_identification_set(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p)
{
    uint32 entry_handle_id;
    uint32 bit_count, max_bits;
    uint32 bit_stream;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_identification_p, _SHR_E_PARAM, "mpls_identification_p");
    /*
     * Verify that the number of bits is not larger than the number of legitimate MPLS protocols in
     * dbal_enum_value_field_current_protocol_type_e. 
     */
    bit_stream = (uint32) (mpls_identification_p->mpls_identification);
    bit_count = utilex_nof_on_bits_in_long(bit_stream);
    max_bits = dnx_data_switch.load_balancing.num_valid_mpls_protocols_get(unit);
    if (bit_count > max_bits)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Number of MPLS protocols on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", bit_count, max_bits);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG, &entry_handle_id));
    /*
     * Set the single 32-bits field - 'MPLS_BIT_IDENTIFIER'
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_MPLS_BIT_IDENTIFIER, INST_SINGLE,
                                 mpls_identification_p->mpls_identification);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_mpls_identification_get(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_identification_p, _SHR_E_PARAM, "mpls_identification_p");
    /*
     * Make sure that input memory is cleared. Not really required but safer.
     */
    sal_memset(mpls_identification_p, 0, sizeof(*mpls_identification_p));
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG, &entry_handle_id));
    /*
     * Perform the DBAL read on all fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Set the single field - 'MPLS_BIT_IDENTIFIER'
     */
    dbal_entry_handle_value_field32_get(unit,
                                        entry_handle_id, DBAL_FIELD_MPLS_BIT_IDENTIFIER, INST_SINGLE,
                                        &(mpls_identification_p->mpls_identification));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_general_seeds_set(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(general_seeds_p, _SHR_E_PARAM, "general_seeds_p");
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_GENERAL_SEEDS, &entry_handle_id));
    /*
     * Set the various fields - 'lb_crc_seed_for_mpls_stack_0', ...
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_0, INST_SINGLE,
                                 general_seeds_p->mpls_stack_0);
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_1, INST_SINGLE,
                                 general_seeds_p->mpls_stack_1);
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_PARSER, INST_SINGLE,
                                 general_seeds_p->parser);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_general_seeds_get(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(general_seeds_p, _SHR_E_PARAM, "general_seeds_p");
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_GENERAL_SEEDS, &entry_handle_id));
    /*
     * Perform the DBAL read on all fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get the various fields - 'lb_crc_seed_for_mpls_stack_0', ...
     */
    dbal_entry_handle_value_field32_get(unit,
                                        entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_0, INST_SINGLE,
                                        &(general_seeds_p->mpls_stack_0));
    dbal_entry_handle_value_field32_get(unit,
                                        entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_1, INST_SINGLE,
                                        &(general_seeds_p->mpls_stack_1));
    dbal_entry_handle_value_field32_get(unit,
                                        entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_PARSER, INST_SINGLE,
                                        &(general_seeds_p->parser));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_mpls_reserved_label_set(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_reserved_p, _SHR_E_PARAM, "mpls_reserved_p");
    /*
     * Make sure that elements 'reserved_next_label_indication' and
     * 'reserved_next_label_valid' are the same.
     */
    if (mpls_reserved_p->reserved_next_label_indication != mpls_reserved_p->reserved_next_label_valid)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n"
                     "Element 'reserved_next_label_indication' (0x%04X) is not equal to\r\n"
                     "  'reserved_next_label_valid' (0x%04X). This is illegal. Quit.\r\n",
                     mpls_reserved_p->reserved_next_label_indication, mpls_reserved_p->reserved_next_label_valid);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_RESERVED_LABELS, &entry_handle_id));
    /*
     * Set the various fields - 'rsrvd_current_label_indication', 'rsrvd_current_label_force_layer', ...
     */
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_INDICATION, INST_SINGLE,
                                 mpls_reserved_p->reserved_current_label_indication);
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE,
                                 mpls_reserved_p->reserved_current_label_force_layer);
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_INDICATION, INST_SINGLE,
                                 mpls_reserved_p->reserved_next_label_indication);
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_VALID, INST_SINGLE,
                                 mpls_reserved_p->reserved_next_label_valid);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_mpls_reserved_label_get(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_reserved_p, _SHR_E_PARAM, "mpls_reserved_p");
    /*
     * Make sure that input memory is cleared. Not really required but safer.
     */
    sal_memset(mpls_reserved_p, 0, sizeof(*mpls_reserved_p));
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_RESERVED_LABELS, &entry_handle_id));
    /*
     * Perform the DBAL read on all fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get the various fields - 'rsrvd_current_label_indication', 'rsrvd_current_label_force_layer', ...
     */
    dbal_entry_handle_value_field16_get(unit,
                                        entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_INDICATION, INST_SINGLE,
                                        &(mpls_reserved_p->reserved_current_label_indication));
    dbal_entry_handle_value_field16_get(unit,
                                        entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE,
                                        &(mpls_reserved_p->reserved_current_label_force_layer));
    dbal_entry_handle_value_field16_get(unit,
                                        entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_INDICATION, INST_SINGLE,
                                        &(mpls_reserved_p->reserved_next_label_indication));
    dbal_entry_handle_value_field16_get(unit,
                                        entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_VALID, INST_SINGLE,
                                        &(mpls_reserved_p->reserved_next_label_valid));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_tcam_info_set(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t tcam_valid)
{
    uint32 entry_handle_id;
    uint32 nof_lb_crc_sel_tcam_entries;
    dnx_switch_ld_destination_t lb_destination_mask;
    dnx_switch_lb_outlif_0_t outlif_0_mask;
    uint32 num_clients;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_key_p, _SHR_E_PARAM, "tcam_key_p");
    SHR_NULL_CHECK(tcam_result_p, _SHR_E_PARAM, "tcam_result_p");
    nof_lb_crc_sel_tcam_entries = dnx_data_switch.load_balancing.nof_lb_crc_sel_tcam_entries_get(unit);
    if (index >= nof_lb_crc_sel_tcam_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index of entry to update (%d) is out of range. Should be between %d and %d.\r\n",
                     index, 0, nof_lb_crc_sel_tcam_entries - 1);
    }
    /*
     * Make sure that each client is configured to a unique crc function.
     */
    num_clients = sizeof(tcam_result_p->client_x_crc_16_sel) / sizeof(tcam_result_p->client_x_crc_16_sel[0]);
    if (num_clients > 1)
    {
        uint32 ii, jj;
        for (ii = 0; ii < (num_clients - 1); ii++)
        {
            dnx_switch_lb_client_crc_sel_t crc_sel;
            crc_sel = tcam_result_p->client_x_crc_16_sel[ii];
            for (jj = (ii + 1); jj < num_clients; jj++)
            {
                if (crc_sel == tcam_result_p->client_x_crc_16_sel[jj])
                {
                    bcm_switch_control_t logical_client_numeric[2];
                    char *string_enum_id[2];
                    SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical(unit, ii, &(logical_client_numeric[0])));
                    SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical(unit, jj, &(logical_client_numeric[1])));
                    SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client_numeric[0], &(string_enum_id[0])));
                    SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client_numeric[1], &(string_enum_id[1])));
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "\r\n"
                                 "Two physical clients (%d, %d) have the same CRC function (%d).\r\n"
                                 "==> The corresponding logical clients are (%s, %s).\r\n"
                                 "==> This is not allowed. Quit.\r\n",
                                 ii, jj, crc_sel, string_enum_id[0], string_enum_id[1]);
                }
            }
        }
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SELECTION_HL_TCAM, &entry_handle_id));
    /*
     * Set access ID (index)
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));
    /*
     * Set key and mask for key - 'destination' and 'outlif_0' group, mask 1 means bit is ignored
     */
    lb_destination_mask = ~(tcam_key_p->lb_destination_mask);
    dbal_entry_key_field32_masked_set(unit,
                                      entry_handle_id, DBAL_FIELD_LB_DESTINATION, tcam_key_p->lb_destination,
                                      lb_destination_mask);
    outlif_0_mask = ~(tcam_key_p->outlif_0_mask);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_0, tcam_key_p->outlif_0, outlif_0_mask);
    /*
     * Set result - Array of CLIENT_x_CRC_16_SEL
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_0_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_1_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[1]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_2_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[2]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_3_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[3]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_4_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[4]);
    /*
     * Set valid bit
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, tcam_valid);
    /*
     * Commit
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header on switch_load_balancing.h
 */
shr_error_e
dnx_switch_load_balancing_tcam_info_get(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t * tcam_valid_p)
{
    uint32 entry_handle_id;
    uint32 nof_lb_crc_sel_tcam_entries;
    dnx_switch_ld_destination_t lb_destination_mask;
    dnx_switch_lb_outlif_0_t outlif_0_mask;
    uint32 field_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_key_p, _SHR_E_PARAM, "physical_client_p");
    SHR_NULL_CHECK(tcam_result_p, _SHR_E_PARAM, "tcam_result_p");
    SHR_NULL_CHECK(tcam_valid_p, _SHR_E_PARAM, "tcam_valid_p");
    nof_lb_crc_sel_tcam_entries = dnx_data_switch.load_balancing.nof_lb_crc_sel_tcam_entries_get(unit);
    if (index >= nof_lb_crc_sel_tcam_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index of entry to update (%d) is out of range. Should be between %d and %d.\r\n",
                     index, 0, nof_lb_crc_sel_tcam_entries - 1);
    }
    /*
     * Create handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SELECTION_HL_TCAM, &entry_handle_id));
    /*
     * Set access ID (index)
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));
    /*
     * Perform the DBAL read on all fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get mask for key - 'destination' and 'outlif_0' group, mask 1 means bit is ignored
     */
    /*
     * Destination
     */
    dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                 entry_handle_id, DBAL_FIELD_LB_DESTINATION,
                                                 &(tcam_key_p->lb_destination), &(lb_destination_mask));
    tcam_key_p->lb_destination_mask = ~(lb_destination_mask);
    /*
     * Make sure to zero all bits out-of-range
     */
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_DESTINATION, &field_size));
    tcam_key_p->lb_destination_mask &= SAL_UPTO_BIT(field_size);
    /*
     * Outlif_0
     */
    dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                 entry_handle_id, DBAL_FIELD_OUTLIF_0, &(tcam_key_p->outlif_0),
                                                 &(outlif_0_mask));
    tcam_key_p->outlif_0_mask = ~(outlif_0_mask);
    /*
     * Make sure to zero all bits out-of-range
     */
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_GLOB_OUT_LIF, &field_size));
    tcam_key_p->outlif_0_mask &= SAL_UPTO_BIT(field_size);
    /*
     * Get result - Array of CLIENT_x_CRC_16_SEL
     */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CLIENT_0_CRC_16_SEL, INST_SINGLE,
                                        &(tcam_result_p->client_x_crc_16_sel[0]));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CLIENT_1_CRC_16_SEL, INST_SINGLE,
                                        &(tcam_result_p->client_x_crc_16_sel[1]));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CLIENT_2_CRC_16_SEL, INST_SINGLE,
                                        &(tcam_result_p->client_x_crc_16_sel[2]));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CLIENT_3_CRC_16_SEL, INST_SINGLE,
                                        &(tcam_result_p->client_x_crc_16_sel[3]));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CLIENT_4_CRC_16_SEL, INST_SINGLE,
                                        &(tcam_result_p->client_x_crc_16_sel[4]));
    /*
     * Get valid bit
     */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, tcam_valid_p);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
