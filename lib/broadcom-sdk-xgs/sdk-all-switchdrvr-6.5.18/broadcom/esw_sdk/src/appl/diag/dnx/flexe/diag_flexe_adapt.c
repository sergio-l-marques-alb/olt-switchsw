/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <shared/bsl.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for access */
#include <bcm/l3.h>
#include <appl/diag/diag.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <shared/utilex/utilex_bitstream.h>

#include "diag_dnx_flexe.h"
#include "diag_flexe_adapt.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/sand/sand_mem.h>

#ifdef DIAG_FLEXE_CORE_DRIVER_AVALIABLE
#ifdef MIN
#undef MIN
#endif
#include "reg_operate.h"
#include "hal.h"
#include "b66sar_rx_tx.h"
#include "global_macro.h"
#include "module_mcmac.h"
#include "module_cpb.h"
#include "module_mux.h"
#include "module_demux.h"
#include "module_66bswitch.h"
#include "module_sar.h"
#include "module_flexe_oh.h"
#include "module_oam.h"
#include "top.h"
#endif

/*
    flexe core debug debug-id=x ....
*/
#ifdef DIAG_FLEXE_CORE_DRIVER_AVALIABLE
static diag_flexe_debug_func_info_t debug_func_info[] = {
    {
     1,
     "", NULL, NULL, NULL,
     "b66switch_dump(UINT_8 chip_id)", NULL, NULL, NULL,
     },
    {
     2,
     "", NULL, NULL, NULL,
     "cpb_egress_debug_print(UINT_8 chip_id)", NULL, NULL, NULL,
     },
    {
     3,
     "", NULL, NULL, NULL,
     "cpb_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)", NULL, NULL, NULL,
     },
    {
     4,
     "VALue_0=<ch_id>", NULL, NULL, NULL,
     "cpb_diag_clear(UINT_8 chip_id, UINT_8 ch_id)", NULL, NULL, NULL,
     },
    {
     7,
     "PARAMeter=<string>", NULL, NULL, NULL,
     "demux_phy_group_cfg_debug(char *string)", NULL, NULL, NULL,
     },
    {
     8,
     "PARAMeter=<string>", NULL, NULL, NULL,
     "demux_traffic_flexe_cfg_debug(char *string)", NULL, NULL, NULL,
     },
    {
     10,
     "PARAMeter=<string>", NULL, NULL, NULL,
     "mux_traffic_flexe_b_cfg_debug(char *string)", NULL, NULL, NULL,
     },
    {
     11,
     "PARAMeter=<string>", NULL, NULL, NULL,
     "mux_traffic_flexe_cfg_debug(char *string)", NULL, NULL, NULL,
     },
    {
     12,
     "", NULL, NULL, NULL,
     "sar_dump(UINT_8 chip_id)", NULL, NULL, NULL,
     },
    {
     13,
     "", NULL, NULL, NULL,
     "inf_ch_adp_dump(UINT_8 chip_id)", NULL, NULL, NULL,
     },

    {
     14,
     "", NULL, NULL, NULL,
     "flexe_mux_dump(UINT_8 unit)", NULL, NULL, NULL,
     },
    {
     15,
     "VALue_0=<ch>", NULL, NULL, NULL,
     "flexe_mux_ch_dump(UINT_8 unit, UINT_8 ch)", NULL, NULL, NULL,
     },
    {
     16,
     "VALue_0=<ts>", NULL, NULL, NULL,
     "flexe_mux_ts_dump(UINT_8 unit, UINT_8 ts)", NULL, NULL, NULL,
     },

    {
     17,
     "", NULL, NULL, NULL,
     "flexe_demux_dump(UINT_8 unit)", NULL, NULL, NULL,
     },
    {
     18,
     "VALue_0=<ch>", NULL, NULL, NULL,
     "flexe_demux_ch_dump(UINT_8 unit, UINT_8 ch)", NULL, NULL, NULL,
     },
    {
     19,
     "VALue_0=<ts>", NULL, NULL, NULL,
     "flexe_demux_ts_dump(UINT_8 unit, UINT_8 ts)", NULL, NULL, NULL,
     }
};

static shr_error_e
dnx_diag_flexe_core_debug_function_list(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 ii;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    PRT_TITLE_SET("FlexE core debug function usage");
    PRT_COLUMN_ADD("INDex");
    PRT_COLUMN_ADD("diag synopsis");
    PRT_COLUMN_ADD("debug function prototype");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    for (ii = 0; ii < COUNTOF(debug_func_info); ii++)
    {
        PRT_CELL_SET("INDex=%d", debug_func_info[ii].debug_id);
        PRT_CELL_SET("%s", debug_func_info[ii].usage);
        PRT_CELL_SET("%s", debug_func_info[ii].fun_prototype);

        if (debug_func_info[ii].usage_2 != NULL)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("%s", debug_func_info[ii].usage_2);
            if (debug_func_info[ii].fun_prototype_2 != NULL)
            {
                PRT_CELL_SET("%s", debug_func_info[ii].fun_prototype_2);
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        if (debug_func_info[ii].usage_3 != NULL)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("%s", debug_func_info[ii].usage_3);
            if (debug_func_info[ii].fun_prototype_3 != NULL)
            {
                PRT_CELL_SET("%s", debug_func_info[ii].fun_prototype_3);
            }
            else
            {
                PRT_CELL_SET("");
            }
        }
        if (debug_func_info[ii].usage_4 != NULL)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("%s", debug_func_info[ii].usage_4);
            if (debug_func_info[ii].fun_prototype_4 != NULL)
            {
                PRT_CELL_SET("%s", debug_func_info[ii].fun_prototype_4);
            }
            else
            {
                PRT_CELL_SET("");
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_core_run_debug_function(
    int unit,
    uint32 debug_idx,
    uint32 *debug_value,
    char *debug_string,
    sh_sand_control_t * sand_control)
{
    RET_STATUS rc;
    UINT_8 chip_id;

    SHR_FUNC_INIT_VARS(unit);

    if (debug_idx == 0)
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_core_debug_function_list(unit, sand_control));
        SHR_EXIT();
    }

    chip_id = unit;
    switch (debug_idx)
    {
        case 1:
        {
            rc = b66switch_dump(chip_id);
            break;
        }
        case 2:
        {
            rc = cpb_egress_debug_print(chip_id);
            break;
        }
        case 3:
        {
            rc = cpb_egress_debug_print(unit);
            break;
        }
        case 4:
        {
            rc = cpb_diag_clear(unit, debug_value[0]);
            break;
        }
        case 7:
        {
            rc = demux_phy_group_cfg_debug(debug_string);
            break;
        }
        case 8:
        {
            rc = demux_traffic_flexe_cfg_debug(debug_string);
            break;
        }
        case 10:
        {
            rc = mux_traffic_flexe_b_cfg_debug(debug_string);
            break;
        }
        case 11:
        {
            rc = mux_traffic_flexe_cfg_debug(debug_string);
            break;
        }
        case 12:
        {
            rc = sar_dump(unit);
            break;
        }
        case 13:
        {
            rc = inf_ch_adp_dump(unit);
            break;
        }
        case 14:
        {
            rc = flexe_mux_dump(unit);
            break;
        }
        case 15:
        {
            rc = flexe_mux_ch_dump(unit, debug_value[0]);
            break;
        }
        case 16:
        {
            rc = flexe_mux_ts_dump(unit, debug_value[0]);
            break;
        }
        case 17:
        {
            rc = flexe_demux_dump(unit);
            break;
        }
        case 18:
        {
            rc = flexe_demux_ch_dump(unit, debug_value[0]);
            break;
        }
        case 19:
        {
            rc = flexe_demux_ts_dump(unit, debug_value[0]);
            break;
        }
        default:
        {
            cli_out("debug_idx=%d,  PARAMeter=%s\n", debug_idx, debug_string);
            cli_out("%-8s    %08X %08X\n", "VALue:", debug_value[0], debug_value[1]);
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }

    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
    wrapper for below function which is at Flexe driver
        regp_read/ram_read/regp_write/ram_write
        flexe_oh_instance_alm_get
        sar_oam_per_cnt_get/flexe_oam_per_cnt_get
        sar_oam_cnt_get/flexe_oam_cnt_get
        sar_oam_bas_cnt_get/flexe_oam_bas_cnt_get
        b66sar_rx_ctrl_cfg_get/b66sar_tx_ctrl_cfg_get
        mcmac_diag_clear/cpb_diag_clear/flexe_macrx_diag_clear
        top_rst_glb_fuci_soft_n_reset
*/

/**
 * \brief
 *   wrapper function regp_read
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_regp_read(
    int unit,
    int address,
    uint32 *value)
{
    UINT_8 chip_id;
    RET_STATUS rc;
    UINT_32 u32_data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    chip_id = unit;
    rc = regp_read(chip_id, address, 0, &u32_data);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    *value = u32_data;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to function ram_read
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [out] value - memory value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_ram_read(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    UINT_8 chip_id;
    RET_STATUS rc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    chip_id = unit;
    rc = ram_read(chip_id, TABLE_BASE_ADDR, address, index, width, (UINT_32 *) value);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to function regp_write
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_regp_write(
    int unit,
    int address,
    uint32 value)
{
    UINT_8 chip_id;
    RET_STATUS rc;

    SHR_FUNC_INIT_VARS(unit);

    chip_id = unit;
    rc = regp_write(chip_id, address, 0, value);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to read flexE core memory
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [in] value - memory value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_ram_write(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    UINT_8 chip_id;
    RET_STATUS rc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    chip_id = unit;
    rc = ram_write(chip_id, TABLE_BASE_ADDR, address, index, width, (UINT_32 *) value);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_core_oh_instance_alm_get(
    int unit,
    uint8_t instance_id,
    uint16_t * oh_status)
{
    RET_STATUS rc;
    UINT_16 status;
    UINT_8 chip_id;

    SHR_FUNC_INIT_VARS(unit);
    if (instance_id >= DIAG_FLEXE_PHY_INSTANCE_NUM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invald instance_id %d", instance_id);
    }
    SHR_NULL_CHECK(oh_status, _SHR_E_PARAM, "oh_status");

    chip_id = unit;
    rc = flexe_oh_instance_alm_get(chip_id, instance_id, &status);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    *oh_status = status;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_core_oam_cnt_get(
    int unit,
    int channel,
    diag_flexe_oam_cnt_t * oam_cnt)
{
    RET_STATUS rc;
    UINT_8 chip_id;
    /*
     * sar_oam_per_cnt_get 
     */
    UINT_32 sar_bip8_cnt, sar_bei_cnt;
    /*
     * flexe_oam_per_cnt_get 
     */
    UINT_32 flexe_bip8_cnt, flexe_bei_cnt;
    /*
     * sar_oam_cnt_get/flexe_oam_cnt_get 
     */
    UINT_32 sar_packet_cnt, flexe_packet_cnt;
    /*
     * sar_oam_bas_cnt_get/flexe_oam_bas_cnt_get 
     */
    UINT_32 sar_bas_packet_cnt, flexe_bas_packet_cnt;
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(channel, 0, DIAG_FLEXE_CHANNEL_NUM - 1, _SHR_E_PARAM, "Invald channel %d", channel);
    SHR_NULL_CHECK(oam_cnt, _SHR_E_PARAM, "oam_cnt");
    chip_id = unit;

     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = sar_oam_per_cnt_get(chip_id, channel, &sar_bip8_cnt, &sar_bei_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = flexe_oam_per_cnt_get(chip_id, channel, &flexe_bip8_cnt, &flexe_bei_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = sar_oam_cnt_get(chip_id, channel, &sar_packet_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = flexe_oam_cnt_get(chip_id, channel, &flexe_packet_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = sar_oam_bas_cnt_get(chip_id, channel, &sar_bas_packet_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    rc = flexe_oam_bas_cnt_get(chip_id, channel, &flexe_bas_packet_cnt);
    if (rc != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    oam_cnt->sar_bip8_cnt = sar_bip8_cnt;
    oam_cnt->sar_bei_cnt = sar_bei_cnt;
    oam_cnt->flexe_bip8_cnt = flexe_bip8_cnt;
    oam_cnt->flexe_bei_cnt = flexe_bei_cnt;
    oam_cnt->sar_packet_cnt = sar_packet_cnt;
    oam_cnt->flexe_packet_cnt = flexe_packet_cnt;
    oam_cnt->sar_bas_packet_cnt = sar_bas_packet_cnt;
    oam_cnt->flexe_bas_packet_cnt = flexe_bas_packet_cnt;

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow connect from sar to interlaken.
*/
shr_error_e
dnx_diag_flexe_flow_sar_2_ilkn_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    uint8 chip_id = unit;
    uint32 sar_en, tx_chan_map;
    int sar_channel;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    /*
     * get SAR channel 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &sar_channel));

    b66sar_tx_ctrl_cfg_get(chip_id, sar_channel, SEG_EN, (UINT_32 *) & sar_en);
    b66sar_tx_ctrl_cfg_get(chip_id, sar_channel, TX_CHAN_MAP, (UINT_32 *) & tx_chan_map);

    if ((sar_en) && (tx_chan_map != 0x200))
    {
        SHR_IF_ERR_EXIT(dnx_flexe_ilkn_channel_2_logicals_port(unit, tx_chan_map, 0, dst_port));
        SHR_EXIT();
    }
    else
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow connect from interlaken to sar.
*/
shr_error_e
dnx_diag_flexe_flow_ilkn_2_sar_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    uint8 chip_id = unit;
    uint32 mon_en, rx_chan_map;
    int sar_channel, ilu_channel;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    /*
     * interlaken channel 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &ilu_channel));

    for (sar_channel = 0; sar_channel < DIAG_FLEXE_CHANNEL_NUM; sar_channel++)
    {
        b66sar_rx_ctrl_cfg_get(chip_id, sar_channel, MON_EN, (UINT_32 *) & mon_en);
        b66sar_rx_ctrl_cfg_get(chip_id, sar_channel, RX_CHAN_MAP, (UINT_32 *) & rx_chan_map);
        if ((mon_en) && (ilu_channel == rx_chan_map))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get(unit, DNX_ALGO_PORT_TYPE_FLEXE_SAR,
                                                                         sar_channel, dst_port));
            SHR_EXIT();
        }
    }

    *dst_port = SOC_PORT_INVALID;
    SHR_EXIT();
    /*
     * SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
     */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_core_counter_clear(
    int unit)
{
    UINT_8 chip_id;
    RET_STATUS rc;

    chip_id = unit;

    /*
     * Clear mcmac cnt. 
     */
    rc = mcmac_diag_clear(chip_id, 255);
    if (rc != RET_SUCCESS)
    {
        cli_out("Err %s, %d\n", __FUNCTION__, __LINE__);
    }

    /*
     * Clear cpb egress cnt. 
     */
    rc = cpb_diag_clear(chip_id, 255);
    if (rc != RET_SUCCESS)
    {
        cli_out("Err %s, %d\n", __FUNCTION__, __LINE__);
    }

    /*
     * Clear flexe macrx cnt. 
     */
    rc = flexe_macrx_diag_clear(chip_id, 255);
    if (rc != RET_SUCCESS)
    {
        cli_out("Err %s, %d\n", __FUNCTION__, __LINE__);
    }

    return _SHR_E_NONE;
}

shr_error_e
dnx_diag_flexe_core_reset(
    int unit)
{
    UINT_8 chip_id = unit;
    top_rst_glb_fuci_soft_n_reset(chip_id, 0);
    top_rst_glb_fuci_soft_n_reset(chip_id, 1);

    return 0;
}

#else
/**
 * \brief
 *   wrapper function regp_read
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_regp_read(
    int unit,
    int address,
    uint32 *value)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   wrapper function to function ram_read
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [out] value - memory value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_ram_read(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   wrapper function to function regp_write
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_regp_write(
    int unit,
    int address,
    uint32 value)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   wrapper function to read flexE core memory
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [in] value - memory value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_ram_write(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_core_oh_instance_alm_get(
    int unit,
    uint8_t instance_id,
    uint16_t * oh_status)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_core_oam_cnt_get(
    int unit,
    int channel,
    diag_flexe_oam_cnt_t * oam_cnt)
{
    return _SHR_E_UNAVAIL;
}

/*
    get the flow connect from sar to interlaken.
*/
shr_error_e
dnx_diag_flexe_flow_sar_2_ilkn_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    return _SHR_E_UNAVAIL;
}

/*
    get the flow connect from interlaken to sar.
*/
shr_error_e
dnx_diag_flexe_flow_ilkn_2_sar_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
dnx_diag_flexe_core_counter_clear(
    int unit)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
dnx_diag_flexe_core_reset(
    int unit)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
dnx_diag_flexe_core_run_debug_function(
    int unit,
    uint32 debug_idx,
    uint32 *debug_value,
    char *debug_string,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

#endif
