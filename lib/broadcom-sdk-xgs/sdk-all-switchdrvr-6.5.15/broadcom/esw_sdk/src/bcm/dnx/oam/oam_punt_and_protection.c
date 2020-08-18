
/** \file oam_punt_and_protection.c
 * $Id$
 *
 * OAM punt and protection configurationprocedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */

#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/bcm_int/dnx/oam/oam_punt_and_protection.h>
#include <include/bcm_int/dnx/algo/algo_gpm.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <src/bcm/dnx/oam/oam_oamp.h>

/*
 * }
 */

/*
 * Macros and defines for OAM punt and protection
 * {
 */

#define DNX_OAM_TRAP_ID_MSB_MASK 0xFF00
#define DNX_OAMP_PUNT_MSB        0x400

#define NOF_PTCH_HEADER_BYTES 2
#define NOF_ITMH_HEADER_BYTES 5
#define NOF_ITMH_HEADER_BYTES_JR1 4
#define NOF_PROP_HEADER_BYTES 12
#define NOF_PROP_JR1_HEADER_BYTES 7
#define NOF_FHEI_HEADER_BYTES 5

#define JR2_PPC_DEFAULT 0
#define DEFAULT_PROP_TYPE 1
#define NOF_PROP_FIELDS 15

#define PROP_FIELD_VALUE_BYTE_0 0x2
#define PROP_FIELD_VALUE_BYTE_1 0x0
#define PROP_FIELD_VALUE_BYTE_2 0xfa
#define PROP_FIELD_VALUE_BYTE_3 0x7

#define PROP_JR1_FIELD_VALUE       2
#define PROP_JR1_FIELD_BIT_OFFSET  4
#define PROP_JR1_FIELD_BYTE_OFFSET 6

#define OAMP_PROTECTION_PACKET_LEN 0x40

/*
 * }
 */

/*
 * Globals
 * {
 */

/*
 * } Globals
 */

/*
 * Functions
 * {
 */

/**
 * \brief
 *  Match an OAMP error type to a give trap type
 *   \param [in] unit     -  Unit ID
 *   \param [in] type     -  error trap type to map
 *   \param [in] error_type  - OAMP error type
 *
 * \return
 *   shr_error - Error indication
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_rx_trap_type_to_oam_error_type(
    int unit,
    bcm_rx_trap_t type,
    dbal_enum_value_field_oamp_error_type_e * error_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(error_type, _SHR_E_PARAM, "error_type");

    switch (type)
    {
        case bcmRxTrapOampChanTypeMissErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CHAN_TYPE_MISS;
            break;

        case bcmRxTrapOampTypeErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TYPE_MISS;
            break;

        case bcmRxTrapOampRmepErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_EMC_MISS;
            break;

        case bcmRxTrapOampMaidErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MAID_MISS;
            break;

        case bcmRxTrapOampFlexCrcMissErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_FLEX_CRC_MISS;
            break;

        case bcmRxTrapOampMdlErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MDL_MISS;
            break;

        case bcmRxTrapOampCcmIntrvErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CCM_INTRVL_MISS;
            break;

        case bcmRxTrapOampMyDiscErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_MY_DISC_MISS;
            break;

        case bcmRxTrapOampSrcIpErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_SRC_IP_MISS;
            break;

        case bcmRxTrapOampYourDiscErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_YOUR_DISC_MISS;
            break;

        case bcmRxTrapOampSrcPortErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_SRC_PORT_MISS;
            break;

        case bcmRxTrapOampRmepStateChange:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RMEP_STATE_CHANGE;
            break;

        case bcmRxTrapOampParityErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_PARITY_ERROR;
            break;

        case bcmRxTrapOampTimestampErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TIMESTAMP_MISS;
            break;

        case bcmRxTrapOampTrapErr:
            *error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TRAP_CODE_MISS;
            break;

        default:
           /**
            * Not an OAM punt/protection trap
            */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_punt_and_protection.h for info
 */
shr_error_e
dnx_oam_error_trap_type_set(
    int unit,
    bcm_rx_trap_t type,
    int trap_id_p)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_oamp_error_type_e error_type = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_PACKET;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ERROR_CODE_FROM_TRAP_ID_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, DNX_RX_TRAP_ID_TYPE_GET(trap_id_p));

    if (type == bcmRxTrapOampProtection)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PROTECTION, INST_SINGLE, TRUE);
    }
    else
    {
        if (type == bcmRxTrapOampChanTypeMissErr)
        {
                            /** Use this opportunity to configure OAMP_PORT */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_init_cpu_port_configuration(unit));
        }
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_oam_error_type(unit, type, &error_type));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ERROR_TYPE, INST_SINGLE, error_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PROTECTION, INST_SINGLE, FALSE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_punt_and_protection.h for info
 */
shr_error_e
dnx_oam_punt_and_protection_trap_create_verify(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int trap_id,
    uint8 *is_oam_punt_or_protection)
{
    int trap_id_msb;
    SHR_FUNC_INIT_VARS(unit);

    *is_oam_punt_or_protection = (((type >= bcmRxTrapOampTrapErr) && (type <= bcmRxTrapOampProtection)) ||
                                  (type == bcmRxTrapOampChanTypeMissErr) || (type == bcmRxTrapOampFlexCrcMissErr));

    if (*is_oam_punt_or_protection == TRUE)
    {
        if (!_SHR_IS_FLAG_SET(flags, BCM_RX_TRAP_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "OAMP Error traps must be created with WITH_ID flag\n");
        }

        trap_id_msb = trap_id & DNX_OAM_TRAP_ID_MSB_MASK;

        if (trap_id_msb != DNX_OAMP_PUNT_MSB)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "OAMP Error traps must be in range 0x400-0x4FF");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static void
copy_encoded_header(
    uint8 *dest_header,
    uint8 *src_header,
    uint32 size)
{
    int i;
    int src_byte = sizeof(uint32) - 1;
    for (i = 0; i < size; i++)
    {
        dest_header[i] = src_header[src_byte];
        if (src_byte % sizeof(uint32) == 0)
        {
            src_byte += 2 * sizeof(uint32) - 1;
        }
        else
        {
            src_byte--;
        }
    }
    return;
}

/**
 * \brief
 *  This function builds a PTCH header according to
 *  global configuration, which is done on
 *  initialization
 * \param [in] unit  - Device ID
 * \param [out] ptch_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; PTCH length is
 *        added to it.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_rx_trap_fill_ptch_header(
    int unit,
    uint8 *ptch_header,
    uint32 *total_header_len)
{
    uint32 pp_ssp, reserved = 0, prt_qual, ppc = JR2_PPC_DEFAULT;
    uint32 ptch_header_tmp = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PTCH_PP_SSP, INST_SINGLE, &pp_ssp);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE, &prt_qual);

    /** Read the values */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PTCH_HEADER, DBAL_FIELD_PTCH_PP_SSP, &pp_ssp, &ptch_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PTCH_HEADER, DBAL_FIELD_RESERVED, &reserved, &ptch_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PTCH_HEADER, DBAL_FIELD_PRT_QUALIFIER, &prt_qual, &ptch_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PTCH_HEADER, DBAL_FIELD_PTCH_PARSER_PROGRAM_CONTROL, &ppc, &ptch_header_tmp));

    copy_encoded_header(ptch_header, (uint8 *) &ptch_header_tmp, NOF_PTCH_HEADER_BYTES);

    *total_header_len += NOF_PTCH_HEADER_BYTES;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function builds an ITMH header from global
 *  configuration, which is done on initialization
 * \param [in] unit - Device ID
 * \param [in] config_p - Pointer to configuration
 *        structure that contains the destination port
 * \param [in] is_jr1_format - TRUE for Jericho1 mode,
 *        FALSE for Jericho 2 mode
 * \param [out] itmh_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; ITMH length is
 *        added to it.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_rx_trap_fill_itmh_header(
    int unit,
    bcm_rx_trap_config_t * config_p,
    uint8 is_jr1_format,
    uint8 *itmh_header,
    uint32 *total_header_len)
{
    uint32 entry_handle_id, itmh_ext_present = 0, tc, dp, snoop_cmd = 0, in_mirr_dis = 0;
    uint32 prop_type = DEFAULT_PROP_TYPE, inj_ext_present = 0, reserved = 0, num_bytes, itmh_header_tmp_len;
    uint32 *itmh_header_tmp = NULL;
    dbal_fields_e itmh, sub_field1, sub_field2;
    uint32 destination = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_TC, INST_SINGLE, &tc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_DP, INST_SINGLE, &dp);

    /** Read the values */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (is_jr1_format)
    {
        num_bytes = NOF_ITMH_HEADER_BYTES_JR1;
        itmh = DBAL_FIELD_ITMH_BASE_JR1;
        sub_field1 = DBAL_FIELD_ITMH_DEST_INFO_EXTENSION_EXISTS;
        sub_field2 = DBAL_FIELD_SNOOP_CMD;
    }
    else
    {
        num_bytes = NOF_ITMH_HEADER_BYTES;
        itmh = DBAL_FIELD_ITMH_BASE;
        sub_field1 = DBAL_FIELD_ITMH_EXTENSION_PRESENT;
        sub_field2 = DBAL_FIELD_SNIF_COMMAND_ID;
    }
    itmh_header_tmp_len = ((num_bytes - 1) / sizeof(uint32) + 1) * sizeof(uint32);
    SHR_ALLOC_SET_ZERO(itmh_header_tmp, itmh_header_tmp_len, "ITMH header for OAM protection",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (!is_jr1_format)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, itmh, DBAL_FIELD_INJECTED_AS_EXTENSION_PRESENT, &inj_ext_present, itmh_header_tmp));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, DBAL_FIELD_RESERVED, &reserved, itmh_header_tmp));
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, sub_field1, &itmh_ext_present, itmh_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, DBAL_FIELD_TC, &tc, itmh_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, sub_field2, &snoop_cmd, itmh_header_tmp));

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, config_p->dest_port, &destination));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, DBAL_FIELD_DESTINATION, &destination, itmh_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, DBAL_FIELD_DP, &dp, itmh_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, itmh, DBAL_FIELD_INBOUND_MIRROR_DISABLE, &in_mirr_dis, itmh_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, itmh, DBAL_FIELD_PPH_TYPE, &prop_type, itmh_header_tmp));

    copy_encoded_header(itmh_header, (uint8 *) itmh_header_tmp, num_bytes);
    *total_header_len += num_bytes;

    SHR_FREE(itmh_header_tmp);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function builds a JR2 format proprietary header
 *  from global configuration, which is done on
 *  initialization
 * \param [in] unit  - Device ID
 * \param [out] prop_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; proprietary header
 *        length is added to it.
 * \return
 *   None
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_rx_trap_fill_prop_header(
    int unit,
    uint8 *prop_header,
    uint32 *total_header_len)
{
    sal_memset(prop_header, 0, NOF_PROP_HEADER_BYTES);

    prop_header[0] = PROP_FIELD_VALUE_BYTE_0;
    prop_header[1] = PROP_FIELD_VALUE_BYTE_1;
    prop_header[2] = PROP_FIELD_VALUE_BYTE_2;
    prop_header[3] = PROP_FIELD_VALUE_BYTE_3;

    *total_header_len += NOF_PROP_HEADER_BYTES;
}

/**
 * \brief
 *  This function builds a JR1 format proprietary header
 *  from global configuration, which is done on
 *  initialization
 * \param [in] unit  - Device ID
 * \param [out] prop_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; proprietary header
 *        length is added to it.
 * \return
 *   None
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_rx_trap_fill_jr1_prop_header(
    int unit,
    uint8 *prop_header,
    uint32 *total_header_len)
{
    sal_memset(prop_header, 0, NOF_PROP_HEADER_BYTES);

    prop_header[PROP_JR1_FIELD_BYTE_OFFSET] |= (PROP_JR1_FIELD_VALUE >> PROP_JR1_FIELD_BIT_OFFSET);

    *total_header_len += NOF_PROP_JR1_HEADER_BYTES;
}

/**
 * \brief
 *  This function builds a 5-byte trap/snoop JR2
 *  format FHEI header from global configuration,
 *  which is done on initialization
 * \param [in] unit  - Device ID
 * \param [in] trap_type - ID of trap to put in header
 * \param [out] fhei_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; ITMH length is
 *        added to it.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_rx_trap_fill_fhei_header(
    int unit,
    int trap_type,
    uint8 *fhei_header,
    uint32 *total_header_len)
{
    uint32 trap_code = trap_type, qualifier = 0, fhei_type = DBAL_ENUM_FVAL_FHEI_TYPE_TRAP_SNOOP;
    uint32 fhei_header_tmp[(NOF_FHEI_HEADER_BYTES - 1) / sizeof(uint32) + 1];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(fhei_header_tmp, 0, sizeof(fhei_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_SNIFF, DBAL_FIELD_FHEI_TYPE, &fhei_type, fhei_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_SNIFF, DBAL_FIELD_CODE, &trap_code, fhei_header_tmp));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_SNIFF, DBAL_FIELD_QUALIFIER, &qualifier, fhei_header_tmp));

    copy_encoded_header(fhei_header, (uint8 *) fhei_header_tmp, NOF_FHEI_HEADER_BYTES);
    *total_header_len += NOF_FHEI_HEADER_BYTES;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function builds a 5-byte trap/snoop JR1
 *  format FHEI header from global configuration,
 *  which is done on initialization
 * \param [in] unit  - Device ID
 * \param [in] trap_type - ID of trap to put in header
 * \param [out] fhei_header - pointer to which the
 *        header is written
 * \param [out] total_header_len - pointer to the
 *        cumulative header length; ITMH length is
 *        added to it.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_rx_trap_fill_jr1_fhei_header(
    int unit,
    int trap_type,
    uint8 *fhei_header,
    uint32 *total_header_len)
{
    uint32 trap_code = trap_type, qualifier = 0, reserved = 0;
    uint32 fhei_header_dword[NOF_FHEI_HEADER_BYTES / sizeof(uint32) + 1];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_JR1, DBAL_FIELD_CPU_TRAP_CODE_JR1, &trap_code, fhei_header_dword));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_JR1, DBAL_FIELD_CPU_TRAP_CODE_QUALIFIER, &qualifier,
                     fhei_header_dword));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_FHEI_5B_TRAP_JR1, DBAL_FIELD_RESERVED, &reserved, fhei_header_dword));

    sal_memcpy(fhei_header, fhei_header_dword, NOF_FHEI_HEADER_BYTES);
    *total_header_len += NOF_FHEI_HEADER_BYTES;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_punt_and_protection.h for info
 */
shr_error_e
dnx_oam_error_trap_set(
    int unit,
    int trap_type,
    bcm_rx_trap_config_t * config_p)
{
    uint32 entry_handle_id, total_header_len = 0, destination;
    uint8 ptch_header[NOF_PTCH_HEADER_BYTES], itmh_header[NOF_ITMH_HEADER_BYTES] =
        { 0 }, prop_header[NOF_PROP_HEADER_BYTES] =
    {
    0}, fhei_header[NOF_FHEI_HEADER_BYTES] =
    {
    0};
    uint32 ptch_header_size, itmh_header_size, prop_header_size, fhei_header_size, back_offset;
    uint8 headers[64] = { 0 };
    uint8 zeros[16] = { 0 };

    uint8 oamp_tx_format, jr1_format, is_format_jr1;
    dbal_enum_value_field_oamp_error_type_e error_type;
    uint32 is_protection;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ERROR_CODE_FROM_TRAP_ID_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, trap_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_ERROR_TYPE, INST_SINGLE, &error_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_PROTECTION, INST_SINGLE, &is_protection);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (is_protection == TRUE)
    {
        /** Protection trap */
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        /** PTCH header is the same for JR1 and JR2 */
        SHR_IF_ERR_EXIT(dnx_rx_trap_fill_ptch_header(unit, ptch_header, &total_header_len));

        is_format_jr1 = (oamp_tx_format == jr1_format);

        SHR_IF_ERR_EXIT(dnx_rx_trap_fill_itmh_header(unit, config_p, is_format_jr1, itmh_header, &total_header_len));

        if (oamp_tx_format == jr1_format)
        {
                /** Jericho 1 headers */
            dnx_rx_trap_fill_jr1_prop_header(unit, prop_header, &total_header_len);
            SHR_IF_ERR_EXIT(dnx_rx_trap_fill_jr1_fhei_header(unit, trap_type, fhei_header, &total_header_len));
        }
        else
        {
                /** Jericho 2 headers */
            dnx_rx_trap_fill_prop_header(unit, prop_header, &total_header_len);
            SHR_IF_ERR_EXIT(dnx_rx_trap_fill_fhei_header(unit, trap_type, fhei_header, &total_header_len));
        }

        ptch_header_size = NOF_PTCH_HEADER_BYTES;
        itmh_header_size = is_format_jr1 ? NOF_ITMH_HEADER_BYTES_JR1 : NOF_ITMH_HEADER_BYTES;
        prop_header_size = is_format_jr1 ? NOF_PROP_JR1_HEADER_BYTES : NOF_PROP_HEADER_BYTES;
        fhei_header_size = NOF_FHEI_HEADER_BYTES;

        back_offset = ptch_header_size;
        sal_memcpy(headers + sizeof(headers) - back_offset, ptch_header, ptch_header_size);
        back_offset += itmh_header_size;
        sal_memcpy(headers + sizeof(headers) - back_offset, itmh_header, itmh_header_size);
        back_offset += prop_header_size;
        sal_memcpy(headers + sizeof(headers) - back_offset, prop_header, prop_header_size);
        back_offset += fhei_header_size;
        sal_memcpy(headers + sizeof(headers) - back_offset, fhei_header, fhei_header_size);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PROTECTION_PACKET_HEADER, &entry_handle_id));
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_HEADER_PART_2, INST_SINGLE,
                                        headers);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PROTECTION_PACKET_HEADER, &entry_handle_id));
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_HEADER_PART_1, INST_SINGLE,
                                        zeros);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        if (total_header_len > OAMP_PROTECTION_PACKET_LEN)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "OAMP total header length is greater than OAMP_PROTECTION_PACKET_LEN");
        }

        /*
         * update the header length field in the configuration register
         */
        if (is_format_jr1)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_OAMP_JR1_FORMAT_PROTECTION_PACKET_HEADER, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_LEN, INST_SINGLE,
                                         OAMP_PROTECTION_PACKET_LEN);
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_OAMP_JR2_FORMAT_PROTECTION_PACKET_HEADER, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_LEN, INST_SINGLE,
                                         OAMP_PROTECTION_PACKET_LEN);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Punt trap */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_OAMP_PUNT_PACKET_TRAP_CODES_AND_DESTINATIONS, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ERROR_TYPE, error_type);
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE,
                                                                     config_p->dest_port, &destination));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_DESTINATION, INST_SINGLE, destination);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_TRAP_CODE, INST_SINGLE, trap_type);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_punt_and_protection.h for info
 */
shr_error_e
dnx_oam_error_trap_get(
    int unit,
    int trap_type,
    bcm_rx_trap_config_t * config_p)
{
    uint32 entry_handle_id, error_type, is_protection, dest_encoded;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ERROR_CODE_FROM_TRAP_ID_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, trap_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_ERROR_TYPE, INST_SINGLE, &error_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_PROTECTION, INST_SINGLE, &is_protection);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (is_protection == FALSE)
    {
        /** Punt trap */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_OAMP_PUNT_PACKET_TRAP_CODES_AND_DESTINATIONS, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ERROR_TYPE, error_type);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAMP_DESTINATION, INST_SINGLE, &dest_encoded);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE,
                                                                      dest_encoded, &config_p->dest_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_punt_and_protection.h for info
 */
shr_error_e
dnx_oam_error_trap_destroy(
    int unit,
    int trap_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ERROR_CODE_FROM_TRAP_ID_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, trap_type);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Configure the parameters for protection packets generated
*  by the OAMP.
* \param [in] unit  - Relevant unit.
* \param [in] packet_header - Pointer to the structure,
* containing information about the protection packet header.
*
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_oam_protection_packet_header_set(
    int unit,
    bcm_pkt_blk_t * packet_header)
{
    uint8 *value_array8 = NULL;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PROTECTION_PACKET_HEADER, &entry_handle_id));

    /** If length is more than 64 bytes, remainder goes in part 2 */
    if (packet_header->len > DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES)
    {
        sal_memcpy(value_array8, &packet_header->data[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES],
                   packet_header->len - DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
    }
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_HEADER_PART_2, INST_SINGLE,
                                    value_array8);

    /** Beginning of header goes in part 1 */
    sal_memcpy(value_array8, packet_header->data, UTILEX_MIN(packet_header->len, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES));

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_HEADER_PART_1, INST_SINGLE,
                                    value_array8);

    /** Length of header */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PACKET_LEN, INST_SINGLE,
                                 packet_header->len);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
