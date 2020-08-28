/*
 * $Id: dnxc_fabric_cell.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#include <shared/bsl.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnxc/dnxc_fabric_cell.h>

static shr_error_e
soc_dnxc_fabric_cell_table_dest_fill(
    int unit,
    soc_dnxc_fabric_cell_parse_table_t * parse_table,
    uint32 nof_lines,
    soc_dnxc_fabric_cell_info_t * cell_info)
{
    int line;
    SHR_FUNC_INIT_VARS(unit);

    for (line = 0; line < nof_lines; line++)
    {
        switch (parse_table[line].dest_id)
        {
            case soc_dnxc_fabric_cell_dest_id_src_cell_type:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.cell_type;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_src_device:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_device;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_src_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_level;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_dest_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.destination_level;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_fip:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fip_link;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_fe1:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe1_link;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_fe2:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe2_link;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_fe3:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe3_link;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_is_inband:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.is_inband;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_ack:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.ack;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_pipe_id:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.pipe_id;
                break;
            case soc_dnxc_fabric_cell_dest_id_src_payload:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.payload.data;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_type:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_type;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_src_device:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.source_device;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_dst:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.dest;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_ind:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.is_last_cell;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_size:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_size;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_pipe_index:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.pipe_index;
                break;
            case soc_dnxc_fabric_cell_dest_id_cap_cell_payload:
                parse_table[line].dest = (uint32 *) cell_info->cell.captured_cell.data;
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "UNAVAIL line id");
                break;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_cell_parser(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    soc_dnxc_fabric_cell_parse_table_t * parse_table,
    uint32 nof_lines,
    soc_dnxc_fabric_cell_info_t * cell_info)
{
    int line, nof_uint32, i, rv;
    soc_dnxc_fabric_cell_entry_t val;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_dnxc_fabric_cell_table_dest_fill(unit, parse_table, nof_lines, cell_info);
    SHR_IF_ERR_EXIT(rv);

    sal_memset(val, 0x00, sizeof(soc_dnxc_fabric_cell_entry_t));

    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "Received Cell Parse\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "-------------------\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "Entry print: ")));
    for (i = 0; i < SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32; i++)
    {
        LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "0x%08x, "), entry[i]));
    }
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "\n")));

    for (line = 0; line < nof_lines; line++)
    {
        /*
         * Copy src to val
         */
        SHR_BITCOPY_RANGE(val, 0, entry, parse_table[line].src_start_bit, parse_table[line].length);

        /*
         * print val
         */
        nof_uint32 = (parse_table[line].length + 31) / 32;      /* round up */
        if (nof_uint32 == 1)
        {
            LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "%s: 0x%x"), parse_table[line].field_name, *val));
        }
        else
        {
            LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "%s: 0x%08x"), parse_table[line].field_name, *val));
            for (i = 1; i < nof_uint32; i++)
            {
                LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, ", 0x%08x"), val[i]));
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "\n")));

        /*
         * Copy val to dest
         */
        SHR_BITCOPY_RANGE(parse_table[line].dest, parse_table[line].dest_start_bit, val, 0, parse_table[line].length);

        SHR_BITCLR_RANGE(val, 0, parse_table[line].length);
    }
    LOG_DEBUG(BSL_LS_SOC_PKT, (BSL_META_U(unit, "-------------------\n\n")));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
