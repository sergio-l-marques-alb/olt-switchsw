/** \file dnx_ire_packet_utils.c
 *  Miscellaneous routines for managing packet sending via IRE
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <soc/dnx/utils/dnx_ire_packet_utils.h>

/*
 * see dnx_ire_packet_utils.h
 */
shr_error_e
dnx_ire_packet_init(
    int unit,
    dnx_ire_packet_control_info_t *packet_info)
{
    uint32 valid_bytes;

    SHR_FUNC_INIT_VARS(unit);

    valid_bytes = UTILEX_MIN(packet_info->valid_bytes, DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES);

    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 0));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_EOPf, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SOPf, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_BEf, valid_bytes));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IRE_REGISTER_INTERFACE_PACKET_DATA_BITS_511_0r, 0, 0, packet_info->packet_data_0));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, IRE_REGISTER_INTERFACE_PACKET_DATA_BITS_511_0r, 1, 0, packet_info->packet_data_1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_SEND_MODEf, packet_info->packet_mode));
    if(packet_info->packet_mode == DNX_IRE_PACKET_MODE_CONST_RATE)
    {
        uint32 clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
        uint32 packet_khz = packet_info->pakcet_rate_khz;
        uint32 clocks_per_packet = clock_khz/packet_khz;
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_TIMER_CYCLESf, clocks_per_packet));
    }
    else if(packet_info->packet_mode == DNX_IRE_PACKET_MODE_SINGLE_SHOT)
    {
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_NOF_PACKETSf, packet_info->nof_packets));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_ire_packet_utils.h
 */
shr_error_e
dnx_ire_packet_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, ENABLE_DATA_PATHf, 1));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_ire_packet_utils.h
 */
shr_error_e
dnx_ire_packet_end(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, REG_PORT_ANY, REGI_PKT_SEND_DATAf, 0));

exit:
    SHR_FUNC_EXIT;
}
